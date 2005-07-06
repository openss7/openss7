/*****************************************************************************

 @(#) $RCSfile: cd_hdlc.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/07/05 22:45:21 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/07/05 22:45:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: cd_hdlc.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/07/05 22:45:21 $"

static char const ident[] =
    "$RCSfile: cd_hdlc.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/07/05 22:45:21 $";

/*
 *  This is an HDLC (High-Level Data Link Control) module which
 *  provides HDLC capabilities when pushed over a Channel Interface (CHI)
 *  stream.  The module provides the Communications Device Interface (CDI).
 *  This supports both STYLE1 and STYLE2 providers.  Attach addresses are
 *  passed through to the Channel Interface (CHI) stream below permitting this
 *  module to be pushed immediately over a freshly opened Channel Interface
 *  (CHI) stream.
 *
 *  The HDLC is intended to be linked under the Q920 multiplexing driver to be
 *  accessed by LAPD and LAPF modules that are subsequently pushed under IDSN
 *  and Frame Relay drivers.
 */
#include <os7/compat.h>

#include <sys/cdi.h>
#include <sys/cdi_hdlc.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/hdlc_ioctl.h>

#include "cd/cd.h"

#define CD_HDLC_DESCRIP		"ISO 3309/4335 HDLC: (High-Level Data Link Control) STREAMS MODULE."
#define CD_HDLC_REVISION	"OpenSS7 $RCSfile: cd_hdlc.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/07/05 22:45:21 $"
#define CD_HDLC_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define CD_HDLC_DEVICES		"Supports OpenSS7 Channel Drivers."
#define CD_HDLC_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define CD_HDLC_LICENSE		"GPL"
#define CD_HDLC_BANNER		CD_HDLC_DESCRIP		"\n" \
				CD_HDLC_COPYRIGHT	"\n" \
				CD_HDLC_REVISION	"\n" \
				CD_HDLC_DEVICES		"\n" \
				CD_HDLC_CONTACT		"\n"
#define CD_HDLC_SPLASH		CD_HDLC_DESCRIP		"\n" \
				CD_HDLC_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(CD_HDLC_CONTACT);
MODULE_DESCRIPTION(CD_HDLC_DESCRIP);
MODULE_SUPPORTED_DEVICE(CD_HDLC_DEVICES);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CD_HDLC_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-cd_hdlc");
#endif
#endif				/* LINUX */

#ifdef LFS
#define CD_HDLC_MOD_ID		CONFIG_STREAMS_CD_HDLC_MODID
#define CD_HDLC_MOD_NAME	CONFIG_STREAMS_CD_HDLC_NAME
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define MOD_ID		CD_HDLC_MOD_ID
#define MOD_NAME	CD_HDLC_MOD_NAME

STATIC struct module_info hdlc_winfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME "-wr",	/* Module name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC struct module_info hdlc_rinfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME "-rd",	/* Module name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC int hdlc_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int hdlc_close(queue_t *, int, cred_t *);

STATIC struct qinit hdlc_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:hdlc_open,		/* Each open */
	qi_qclose:hdlc_close,		/* Last close */
	qi_minfo:&hdlc_rinfo,		/* Information */
};

STATIC struct qinit hdlc_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&hdlc_winfo,		/* Information */
};

STATIC struct streamtab cd_hdlcinfo = {
	st_rdinit:&hdlc_rinit,		/* Upper read queue */
	st_wrinit:&hdlc_winit,		/* Upper write queue */
};

STATIC int
hdlc_init_caches(void)
{
	return (0);
}
STATIC int

hdlc_term_caches(void)
{
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC struct str *hdlc_list = NULL;
STATIC int
hdlc_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct str *str;
		/* test for multiple push */
		for (str = hdlc_list; str; str = str->next) {
			if (str->u.dev.cmajor == cmajor && str->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(str = cd_alloc_priv(q, &hdlc_list, devp, crp, CD_HDLC))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
hdlc_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	cd_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = MOD_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the CD-HDLC module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw hdlc_fmod = {
	.f_name = MOD_NAME,
	.f_str = &cd_hdlcinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
hdlc_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&hdlc_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
hdlc_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&hdlc_fmod)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
hdlc_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&cd_hdlcinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
hdlc_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&cd_hdlcinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
cd_hdlcinit(void)
{
	int err;
#ifdef MODULE
	cmn_err(CE_NOTE, CD_HDLC_BANNER);	/* banner message */
#else
	cmn_err(CE_NOTE, CD_HDLC_SPLASH);	/* console splash */
#endif
	if ((err = hdlc_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = hdlc_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		hdlc_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
cd_hdlcterminate(void)
{
	int err;
	if ((err = hdlc_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = hdlc_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(cd_hdlcinit);
module_exit(cd_hdlcterminate);

#endif				/* LINUX */

