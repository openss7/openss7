/*****************************************************************************

 @(#) $RCSfile: m3ua.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:10:17 $

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

 Last Modified $Date: 2006/03/07 01:10:17 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua.c,v $
 Revision 0.9.2.11  2006/03/07 01:10:17  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: m3ua.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:10:17 $"

static char const ident[] =
    "$RCSfile: m3ua.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:10:17 $";

#include <sys/os7/compat.h>

#include "m3ua.h"
#include "m3ua_data.h"

/*
 *  M3UA MULTIPLEXOR
 *  -------------------------------------------------------------------------
 *  This is a multiplexing driver for M3UA.  When M3UA is opened by its control
 *  stream (typically a ua configuration daemon), it provides a control
 *  channel for configuration and routing.  When opened normally, by a user
 *  process, it provides an OpenSS7 MTP Provider.  OpenSS7 MTP Provider
 *  streams are I_LINKed and I_PLINKed under the multiplexor for Signalling
 *  Gateway (SG) Use by the M3UA control stream or M3UA user stream.  MTP
 *  Provider streams can be opened for Application Server (AS) use.  Transport
 *  streams (typically SCTP) are I_LINKed or I_PLINKed under the multiplexor
 *  for both SG and AS use by the UA/LM control stream.
 */

#define M3UA_DESCRIP	"M3UA STREAMS MULTIPLEXING DRIVER."
#define M3UA_REVISION	"OpenSS7 $RCSfile: m3ua.c,v $ $Name:  $ ($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:10:17 $"
#define M3UA_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corp.  All Rights Reserved."
#define M3UA_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define M3UA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define M3UA_LICENSE	"GPL"
#define M3UA_BANNER	M3UA_DESCRIP	"\n" \
			M3UA_REVISION	"\n" \
			M3UA_COPYRIGHT	"\n" \
			M3UA_DEVICE	"\n" \
			M3UA_CONTACT
#define M3UA_SPLASH	M3UA_DESCRIP	"\n" \
			M3UA_REVISION

#ifdef LINUX
MODULE_AUTHOR(M3UA_CONTACT);
MODULE_DESCRIPTION(M3UA_DESCRIP);
MODULE_SUPPORTED_DEVICE(M3UA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M3UA_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m3ua");
#endif
#endif				/* LINUX */

#ifdef LFS
#define M3UA_DRV_ID	CONFIG_STREAMS_M3UA_MODID
#define M3UA_DRV_NAME	CONFIG_STREAMS_M3UA_NAME
#define M3UA_CMAJORS	CONFIG_STREAMS_M3UA_NMAJORS
#define M3UA_CMAJOR_0	CONFIG_STREAMS_M3UA_MAJOR
#define M3UA_UNITS	CONFIG_STREAMS_M3UA_NMINORS
#endif				/* LFS */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		M3UA_DRV_ID
#define DRV_NAME	M3UA_DRV_NAME
#define CMAJORS		M3UA_CMAJORS
#define CMAJOR_0	M3UA_CMAJOR_0
#define UNITS		M3UA_UNITS
#ifdef MODULE
#define DRV_BANNER	M3UA_BANNER
#else				/* MODULE */
#define DRV_BANNER	M3UA_SPLASH
#endif				/* MODULE */

static struct module_info m3ua_minfo = {
	DRV_ID,				/* Module ID number */
	DRV_NAME,			/* Module name */
	1,				/* Min packet size accepted *//* XXX */
	512,				/* Max packet size accepted *//* XXX */
	8 * 512,			/* Hi water mark *//* XXX */
	1 * 512				/* Lo water mark *//* XXX */
};

static struct qinit m3ua_rinit = {
	ua_rput,			/* Read put (msg from below) */
	ua_rsrv,			/* Read queue service */
	ua_open,			/* Each open */
	ua_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&m3ua_minfo,			/* Information */
	NULL				/* Statistics */
};

static struct qinit m3ua_winit = {
	ua_wput,			/* Write put (msg from above) */
	ua_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&m3ua_minfo,			/* Information */
	NULL				/* Statistics */
};

MODULE_STATIC struct streamtab m3uainfo = {
	&m3ua_rinit,			/* Upper read queue */
	&m3ua_winit,			/* Upper write queue */
	&m3ua_rinit,			/* Lower read queue */
	&m3ua_winit			/* Lower write queue */
};

STATIC int
m3ua_init_caches(void)
{
	return (0);
}
STATIC int
m3ua_term_caches(void)
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

static dp_t *m3ua_opens_list = NULL;
static lp_t *m3ua_links_list = NULL;

static struct ua_driver m3ua_dinfo = {
	M3UA_CMAJOR,			/* Major device number */
	M3UA_CMINOR,			/* Number of minor devices */
	sizeof(mtp_t),			/* Private structure size */
	NULL,				/* Current control queue */
	&m3ua_lmq_u_ops,		/* LMQ User operations */
	&m3ua_ss7_u_ops,		/* SS7 User operations */
	&m3ua_opens_list,		/* Opened structures list */
	&m3ua_links_list		/* LInked structures list */
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
MODULE_PARM_DESC(modid, "Module ID for the INET driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the INET driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw m3ua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &m3uainfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
m3ua_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&m3ua_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
m3ua_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&m3ua_cdev, major)) < 0)
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
m3ua_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &m3uainfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
m3ua_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
m3uaterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (m3ua_majors[mindex]) {
			if ((err = m3ua_unregister_strdev(m3ua_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					m3ua_majors[mindex]);
			if (mindex)
				m3ua_majors[mindex] = 0;
		}
	}
	if ((err = m3ua_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
m3uainit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = m3ua_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		m3uaterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = m3ua_register_strdev(m3ua_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					m3ua_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				m3uaterminate();
				return (err);
			}
		}
		if (m3ua_majors[mindex] == 0)
			m3ua_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(m3ua_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = m3ua_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(m3uainit);
module_exit(m3uaterminate);

#endif				/* LINUX */
