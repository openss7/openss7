/*****************************************************************************

 @(#) $RCSfile: tua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:15 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/26 23:38:15 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:15 $"

static char const ident[] =
    "$RCSfile: tua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:15 $";

#include "compat.h"

#include "tua.h"
#include "tua_data.h"

/*
 *  TUA MULTIPLEXOR
 *  -------------------------------------------------------------------------
 *  This is a multiplexing driver for TUA.  When TUA is opened by its control
 *  stream (typically a ua configuration daemon), it provides a control
 *  channel for configuration and routing.  When opened normally, by a user
 *  process, it provides an OpenSS7 TCAP Provider.  OpenSS7 TCAP Provider
 *  streams are I_LINKed and I_PLINKed under the multiplexor for Signalling
 *  Gateway (SG) Use by the TUA control stream or TUA user stream.  TCAP
 *  Provider streams can be opened for Application Server (AS) use.  Transport
 *  streams (typically SCTP) are I_LINKed or I_PLINKed under the multiplexor
 *  for both SG and AS use by the UA/LM control stream.
 */

#define TUA_DESCRIP	"TUA STREAMS MULTIPLEXING DRIVER."
#define TUA_REVISION	"LfS $RCSfile: tua.c,v $ $Name:  $ ($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:15 $"
#define TUA_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define TUA_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define TUA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TUA_LICENSE	"GPL"
#define TUA_BANNER	TUA_DESCRIP	"\n" \
			TUA_REVISION	"\n" \
			TUA_COPYRIGHT	"\n" \
			TUA_DEVICE	"\n" \
			TUA_CONTACT	"\n"
#define TUA_SPLASH	TUA_DEVICE	" - " \
			TUA_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(TUA_CONTACT);
MODULE_DESCRIPTION(TUA_DESCRIP);
MODULE_SUPPORTED_DEVICE(TUA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TUA_LICENSE);
#endif
#endif				/* LINUX */

#ifdef LFS
#define TUA_DRV_ID	CONFIG_STREAMS_TUA_MODID
#define TUA_DRV_NAME	CONFIG_STREAMS_TUA_NAME
#define TUA_CMAJORS	CONFIG_STREAMS_TUA_NMAJORS
#define TUA_CMAJOR_0	CONFIG_STREAMS_TUA_MAJOR
#define TUA_NMINOR	CONFIG_STREAMS_TUA_NMINORS
#endif

static struct module_info tua_minfo = {
	TUA_DRV_ID,			/* Module ID number */
	TUA_DRV_NAME,			/* Module name */
	1,				/* Min packet size accepted *//* XXX */
	512,				/* Max packet size accepted *//* XXX */
	8 * 512,			/* Hi water mark *//* XXX */
	1 * 512				/* Lo water mark *//* XXX */
};
static struct qinit tua_rinit = {
	ua_rput,			/* Read put (msg from below) */
	ua_rsrv,			/* Read queue service */
	ua_open,			/* Each open */
	ua_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&tua_minfo,			/* Information */
	NULL				/* Statistics */
};
static struct qinit tua_winit = {
	ua_wput,			/* Write put (msg from above) */
	ua_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&tua_minfo,			/* Information */
	NULL				/* Statistics */
};
MODULE_STATIC struct streamtab tua_info = {
	&tua_rinit,			/* Upper read queue */
	&tua_winit,			/* Upper write queue */
	&tua_rinit,			/* Lower read queue */
	&tua_winit			/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */

static dp_t *tua_opens_list = NULL;
static lp_t *tua_links_list = NULL;

static struct ua_driver tua_dinfo = {
	TUA_CMAJOR_0,			/* Major device number */
	TUA_NMINOR,			/* Number of minor devices */
	sizeof(tcap_t),			/* Private structure size */
	NULL,				/* Current control queue */
	&tua_lmq_u_ops,			/* LMQ User operations */
	&tua_ss7_u_ops,			/* SS7 User operations */
	&tua_opens_list,		/* Opened structures list */
	&tua_links_list			/* Linked strcutures list */
};

/*
 *  =========================================================================
 *  
 *  LiS MODULE INITIALIZATION
 *  
 *  =========================================================================
 */

static tua_initialized = 0;

#ifndef LiS_REGISTERED
static inline void
tua_init(void)
#else
__initfunc(void tua_init(void))
#endif
{
	if (tua_initialized)
		return;
	printk(KERN_INFO TUA_BANNER);	/* console splash */
#ifndef LIS_REGISTERED
	if (lis_register_strdev(TUA_CMAJOR_0, &tua_info, TUA_NMINOR, tua_minfo.mi_idname) < 0) {
		cmn_err(CE_NOTE, "tua: couldn't register driver!\n");
		tua_minfo.mi_idnum = 0;
	}
	tua_minfo.mi_idnum = TUA_DRV_ID;
#endif
	tua_driver = &tua_dinfo;
}

#ifndef LIS_REGISTERED
static inline void
tua_terminate(void)
#else
__initfunc(void tua_terminate(void))
#endif
{
	if (!tua_initialized)
		return;
	tua_intialized = 0;
#ifndef LIS_REGISTERED
	if (tua_minfo.mi_idnum)
		if (lis_unregister_strdev(tua_minfo.mi_idnum) < 0)
			cmn_err(CE_WARN, "tua: couldn't unregister driver!\n");
#endif
	tua_driver = NULL;
}

/*
 *  =========================================================================
 *  
 *  LINUX KERNEL MODULE INITIALIZATION
 *  
 *  =========================================================================
 */

#ifdef MODULE
int
init_module(void)
{
	tua_init();
	return (0);
}

void
cleanup_module(void)
{
	tua_terminate();
	return;
}
#endif
