/*****************************************************************************

 @(#) $RCSfile: sua.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/13 12:01:40 $

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/07/13 12:01:40 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sua.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/13 12:01:40 $"

static char const ident[] =
    "$RCSfile: sua.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/13 12:01:40 $";

#include <sys/os7/compat.h>

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
#define SUA_SPLASH	SUA_DEVICE	" - " \
			SUA_COPYRIGHT	"\n"

#ifdef LINUX
MODULE_AUTHOR(SUA_CONTACT);
MODULE_DESCRIPTION(SUA_DESCRIP);
MODULE_SUPPORTED_DEVICE(SUA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SUA_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sua");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SUA_DRV_ID	CONFIG_STREAMS_SUA_MODID
#define SUA_DRV_NAME	CONFIG_STREAMS_SUA_NAME
#define SUA_CMAJORS	CONFIG_STREAMS_SUA_NMAJORS
#define SUA_CMAJOR_0	CONFIG_STREAMS_SUA_MAJOR
#define SUA_UNITS	CONFIG_STREAMS_SUA_NMINORS
#endif

/*
 *  ================================================================================
 *
 *  STREAMS Definitions
 *
 *  ================================================================================
 */

#define DRV_ID		SUA_DRV_ID
#define DRV_NAME	SUA_DRV_NAME
#define CMAJORS		SUA_CMAJORS
#define CMAJOR_0	SUA_CMAJOR_0
#define UNITS		SUA_UNITS
#ifdef MODULE
#define DRV_BANNER	SUA_BANNER
#else				/* MODULE */
#define DRV_BANNER	SUA_SPLASH
#endif				/* MODULE */

static struct module_info sua_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted *//* XXX */
	.mi_maxpsz = 512,		/* Max packet size accepted *//* XXX */
	.mi_hiwat = 8 * 512,		/* Hi water mark *//* XXX */
	.mi_lowat = 1 * 512,		/* Lo water mark *//* XXX */
};

static struct qinit sua_u_rinit = {
	.qi_putp = ua_rput,		/* Read put (msg from below) */
	.qi_srvp = ua_rsrv,		/* Read queue service */
	.qi_qopen = ua_open,		/* Each open */
	.qi_qclose = ua_close,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &sua_minfo,		/* Information */
	.qi_mstat = NULL,		/* Statistics */
};

static struct qinit sua_u_winit = {
	.qi_putp = ua_wput,		/* Write put (msg from above) */
	.qi_srvp = ua_wsrv,		/* Write queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &sua_minfo,		/* Information */
	.qi_mstat = NULL,		/* Statistics */
};

static struct qinit sua_l_rinit = {
	.qi_putp = ua_rput,		/* Read put (msg from below) */
	.qi_srvp = ua_rsrv,		/* Read queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &sua_minfo,		/* Information */
	.qi_mstat = NULL,		/* Statistics */
};

static struct qinit sua_l_winit = {
	.qi_putp = ua_wput,		/* Write put (msg from above) */
	.qi_srvp = ua_wsrv,		/* Write queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &sua_minfo,		/* Information */
	.qi_mstat = NULL,		/* Statistics */
};

MODULE_STATIC struct streamtab sua_info = {
	.st_rdinit = &sua_u_rinit,	/* Upper read queue */
	.st_wrinit = &sua_u_winit,	/* Upper write queue */
	.st_muxrinit = &sua_l_rinit,	/* Lower read queue */
	.st_muxwinit = &sua_l_winit,	/* Lower write queue */
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
	SUA_CMAJOR_0,			/* Major device number */
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
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SUA driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the SUA driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw sua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &suainfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
sua_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&sua_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
sua_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&sua_cdev, major)) < 0)
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
sua_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &suainfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
sua_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
suaterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (sua_majors[mindex]) {
			if ((err = sua_unregister_strdev(sua_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					sua_majors[mindex]);
			if (mindex)
				sua_majors[mindex] = 0;
		}
	}
	if ((err = sua_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
suainit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = sua_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		suaterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = sua_register_strdev(sua_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					sua_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				suaterminate();
				return (err);
			}
		}
		if (sua_majors[mindex] == 0)
			sua_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(sua_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = sua_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(suainit);
module_exit(suaterminate);

#endif				/* LINUX */
