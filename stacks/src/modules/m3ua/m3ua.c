/*****************************************************************************

 @(#) $RCSfile: m3ua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:37:57 $

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

 Last Modified $Date: 2004/08/26 23:37:57 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: m3ua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:37:57 $"

static char const ident[] =
    "$RCSfile: m3ua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:37:57 $";

#include "compat.h"

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
#define M3UA_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corp.  All Rights Reserved."
#define M3UA_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define M3UA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define M3UA_LICENSE	"GPL"
#define M3UA_BANNER	M3UA_DESCRIP	"\n" \
			M3UA_COPYRIGHT	"\n" \
			M3UA_DEVICE	"\n" \
			M3UA_CONTACT	"\n"

#ifdef LINUX
MODULE_AUTHOR(M3UA_CONTACT);
MODULE_DESCRIPTION(M3UA_DESCRIP);
MODULE_SUPPORTED_DEVICE(M3UA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M3UA_LICENSE);
#endif
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

static struct module_info m3ua_minfo = {
	0,				/* Module ID number */
	"m3ua",				/* Module name */
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

MODULE_STATIC struct streamtab m3ua_info = {
	&m3ua_rinit,			/* Upper read queue */
	&m3ua_winit,			/* Upper write queue */
	&m3ua_rinit,			/* Lower read queue */
	&m3ua_winit			/* Lower write queue */
};

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
 *  LiS MODULE INITIALIZATION
 *
 *  =========================================================================
 */

static m3ua_initialized = 0;

#ifndef LIS_REGISTERED
static inline void m3ua_init(void)
#else
__initfunc(void m3ua_init(void))
#endif
{
	if (m3ua_initialized)
		return;
	m3ua_initialized = 1;
	printk(KERN_INFO M2UA_BANNER);	/* console splash */
#ifndef LIS_REGISTERED
	if (lis_register_strdev(M2UA_CMAJOR, &m3ua_info, M2UA_NMINOR, m3ua_minfo.mi_idname) < 0) {
		cmn_err(CE_NOTE, "m3ua: couldn't register driver!\n");
		m3ua_minfo.mi_idnum = 0;
	}
	m3ua_minfo.mi_idnum = M2UA_CMAJOR;
#endif
	m3ua_driver = &m3ua_dinfo;
}

#ifndef LIS_REGISTERED
static inline void m3ua_terminate(void)
#else
__initfunc(void m3ua_terminate(void))
#endif
{
	if (!m3ua_initialized)
		return;
	m3ua_initialized = 0;
#ifndef LIS_REGISTERED
	if (m3ua_minfo.mi_idnum)
		if (lis_unregister_strdev(m3ua_minfo.mi_idnum) < 0)
			cmn_err(CE_WARN, "m3ua: couldn't unregister driver!\n");
#endif
	m3ua_driver = NULL;
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
	m3ua_init();
	return (0);
}
void cleanup_module(void)
{
	m3ua_terminate();
	return;
}
#endif
