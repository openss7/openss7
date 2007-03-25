/*****************************************************************************

 @(#) $RCSfile: cd_daed.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:07:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/03/07 01:07:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cd_daed.c,v $
 Revision 0.9.2.11  2006/03/07 01:07:05  brian
 - binary compatible callouts

 *****************************************************************************/

#ident "@(#) $RCSfile: cd_daed.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:07:05 $"

static char const ident[] =
    "$RCSfile: cd_daed.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:07:05 $";

#include <sys/os7/compat.h>

#include <sys/cdi.h>
#include <sys/cdi_daed.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/hdlc_ioctl.h>

#include "cd/cd.h"

#define CD_DAED_DESCRIP		"Q.703/T1.111.3 DAED: (Delimination Alignment and Error Detection) STREAMS MODULE."
#define CD_DAED_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define CD_DAED_REVISION	"OpenSS7 $RCSfile: cd_daed.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:07:05 $"
#define CD_DAED_DEVICE		"SVR 4.2 STREAMS CDI DAED Module for SS7 Channel Devices (DAED)."
#define CD_DAED_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define CD_DAED_LICENSE		"GPL"
#define CD_DAED_BANNER		CD_DAED_DESCRIP		"\n" \
				CD_DAED_COPYRIGHT	"\n" \
				CD_DAED_REVISION	"\n" \
				CD_DAED_DEVICE		"\n" \
				CD_DAED_CONTACT		"\n"
#define CD_DAED_SPLASH		CD_DAED_DEVICE		" - " \
				CD_DAED_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(CD_DAED_CONTACT);
MODULE_DESCRIPTION(CD_DAED_DESCRIP);
MODULE_SUPPORTED_DEVICE(CD_DAED_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CD_DAED_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-cd_daed");
#endif
#endif				/* LINUX */

#ifdef LFS
#define CD_DAED_MOD_ID		CONFIG_STREAMS_CD_DAED_MODID
#define CD_DAED_MOD_NAME	CONFIG_STREAMS_CD_DAED_NAME
#endif				/* LFS */

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define MOD_ID		CD_DAED_MOD_ID
#define MOD_NAME	CD_DAED_MOD_NAME

STATIC struct module_info daed_winfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME "-wr",	/* Module name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC struct module_info daed_rinfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME "-rd",	/* Module name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC streamscall int daed_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int daed_close(queue_t *, int, cred_t *);

STATIC struct qinit daed_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:daed_open,		/* Each open */
	qi_qclose:daed_close,		/* Last close */
	qi_minfo:&daed_rinfo,		/* Information */
};

STATIC struct qinit daed_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&daed_winfo,		/* Information */
};

STATIC struct streamtab cd_daedinfo = {
	st_rdinit:&daed_rinit,		/* Upper read queue */
	st_wrinit:&daed_winit,		/* Upper write queue */
};

STATIC int
daed_init_caches(void)
{
	return (0);
}

STATIC int
daed_term_caches(void)
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
STATIC struct str *daed_list = NULL;
STATIC streamscall int
daed_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
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
		for (str = daed_list; str; str = str->next) {
			if (str->u.dev.cmajor == cmajor && str->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(str = cd_alloc_priv(q, &daed_list, devp, crp, CD_DAED))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		qprocson(q);
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
daed_close(queue_t *q, int flag, cred_t *crp)
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
MODULE_PARM_DESC(modid, "Module ID for the CD-DAED module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw daed_fmod = {
	.f_name = MOD_NAME,
	.f_str = &cd_daedinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
daed_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&daed_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
daed_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&daed_fmod)) < 0)
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
daed_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&cd_daedinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&cd_daedinfo);
		return (err);
	}
	return (0);
}

STATIC int
daed_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&cd_daedinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
cd_daedinit(void)
{
	int err;
#ifdef MODULE
	cmn_err(CE_NOTE, CD_DAED_BANNER);	/* banner message */
#else
	cmn_err(CE_NOTE, CD_DAED_SPLASH);	/* console splash */
#endif
	if ((err = daed_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = daed_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		daed_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
cd_daedterminate(void)
{
	int err;
	if ((err = daed_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = daed_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(cd_daedinit);
module_exit(cd_daedterminate);

#endif				/* LINUX */
