/*****************************************************************************

 @(#) $RCSfile: sua.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:59 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2004/08/21 10:14:59 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sua.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:59 $"

static char const ident[] =
    "$RCSfile: sua.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:59 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/cmn_err.h>

#include "sua.h"
#include "sua_data.h"

/*
 *  SUA MULTIPLEXOR
 *  -------------------------------------------------------------------------
 *  This is a multiplexing driver for SUA.  When SUA is opened by its control
 *  stream (typically a ua configuration daemon), it provides a control
 *  channel for configuration and routing.  When opened normally, by a user
 *  process, it provides an OpenSS7 SCCP Provider.  OpenSS7 SCCP Provider
 *  streams are I_LINKed and I_PLINKed under the multiplexor for Signalling
 *  Gateway (SG) Use by the SUA control stream or SUA user stream.  SCCP
 *  Provider streams can be opened for Application Server (AS) use.  Transport
 *  streams (typically SCTP) are I_LINKed or I_PLINKed under the multiplexor
 *  for both SG and AS use by the UA/LM control stream.
 */

#define SUA_DESCRIP	"SUA STREAMS MULTIPLEXING DRIVER."
#define SUA_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SUA_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define SUA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SUA_LICENSE	"GPL"
#define SUA_BANNER	SUA_DESCRIP	"\n" \
			SUA_COPYRIGHT	"\n" \
			SUA_DEVICE	"\n" \
			SUA_CONTACT	"\n"

#ifdef MODULE
MODULE_AUTHOR(SUA_CONTACT);
MODULE_DESCRIPTION(SUA_DESCRIP);
MODULE_SUPPORTED_DEVICE(SUA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SUA_LICENSE);
#endif
#define MODULE_STATIC static
#else
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif

/*
 *  ================================================================================
 *
 *  STREAMS Definitions
 *
 *  ================================================================================
 */

static struct module_info sua_minfo = {
	SUA_MODULE_ID,			/* Module ID number */
	"sua",				/* Module name */
	1,				/* Min packet size accepted *//* XXX */
	512,				/* Max packet size accepted *//* XXX */
	8 * 512,			/* Hi water mark *//* XXX */
	1 * 512				/* Lo water mark *//* XXX */
};

static struct qinit sua_u_rinit = {
	ua_rput,			/* Read put (msg from below) */
	ua_rsrv,			/* Read queue service */
	ua_open,			/* Each open */
	ua_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&sua_minfo,			/* Information */
	NULL				/* Statistics */
};
static struct qinit sua_u_winit = {
	ua_wput,			/* Write put (msg from above) */
	ua_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&sua_minfo,			/* Information */
	NULL				/* Statistics */
};
static struct qinit sua_l_rinit = {
	ua_rput,			/* Read put (msg from below) */
	ua_rsrv,			/* Read queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&sua_minfo,			/* Information */
	NULL				/* Statistics */
};
static struct qinit sua_l_winit = {
	ua_wput,			/* Write put (msg from above) */
	ua_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&sua_minfo,			/* Information */
	NULL				/* Statistics */
};

MODULE_STATIC struct streamtab sua_info = {
	&sua_u_rinit,			/* Upper read queue */
	&sua_u_winit,			/* Upper write queue */
	&sua_l_rinit,			/* Lower read queue */
	&sua_l_winit			/* Lower write queue */
};

/*
 *  =========================================================================
 *  
 *  OPEN and CLOSE
 *  
 *  =========================================================================
 */

static dp_t *sua_opens_list = NULL;
static lp_t *sua_links_list = NULL;

static struct ua_driver sua_dinfo = {
	SUA_CMAJOR,			/* Major device number */
	SUA_CMINOR,			/* Number of minor devices */
	sizeof(sccp_t),			/* Private structure size */
	NULL,				/* Current control queue */
	&sua_lmq_u_ops,			/* LMQ User operations */
	&sua_ss7_u_ops,			/* SS7 User operations */
	&sua_opens_list,		/* Opened structures list */
	&sua_links_list			/* LInked structures list */
};

/*
 *  =========================================================================
 *  
 *  LiS MODULE INITIALIZATION
 *  
 *  =========================================================================
 */

static sua_initialized = 0;

#ifndef LiS_REGISTERED
static inline void sua_init(void)
#else
__initfunc(void sua_init(void))
#endif
{
	if (sua_initialized)
		return;
	printk(KERN_INFO SUA_BANNER);	/* console splash */
#ifndef LIS_REGISTERED
	if (lis_register_strdev(SUA_CMAJOR, &sua_info, SUA_NMINOR, sua_minfo.mi_idname) < 0) {
		cmn_err(CE_NOTE, "sua: couldn't register driver!\n");
		sua_minfo.mi_idnum = 0;
	}
	sua_minfo.mi_idnum = SUA_CMAJOR;
#endif
	sua_driver = &sua_dinfo;
}

#ifndef LIS_REGISTERED
static inline void sua_terminate(void)
#else
__initfunc(void sua_terminate(void))
#endif
{
	if (!sua_initialized)
		return;
	sua_intialized = 0;
#ifndef LIS_REGISTERED
	if (sua_minfo.mi_idnum)
		if (lis_unregister_strdev(sua_minfo.mi_idnum) < 0)
			cmn_err(CE_WARN, "sua: couldn't unregister driver!\n");
#endif
	sua_driver = NULL;
}

/*
 *  =========================================================================
 *  
 *  LINUX KERNEL MODULE INITIALIZATION
 *  
 *  =========================================================================
 */

#ifdef MODULE
int init_module(void)
{
	sua_init();
	return (0);
}
void cleanup_module(void)
{
	sua_terminate();
	return;
}
#endif
