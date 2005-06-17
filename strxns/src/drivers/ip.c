/*****************************************************************************

 @(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/06/16 20:41:45 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2005/06/16 20:41:45 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ip.c,v $
 Revision 0.9.2.1  2005/06/16 20:41:45  brian
 - Start of the IP NPI driver.

 *****************************************************************************/

#ident "@(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/06/16 20:41:45 $"

static char const ident[] = "$RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/06/16 20:41:45 $";

/*
   This driver provides the functionality of an IP (Internet Protocol) hook
   similar to raw sockets, with the exception that the hook acts as a port
   bound intercept for IP packets for the bound protocol ids.  This dirver is
   used primarily by OpenSS7 protocol test module (e.g. for SCTP) and for
   applications where entire ranges of port numbers for an existing protocol
   id must be intercepted (e.g. for RTP/RTCP).  This driver uses  hook into
   the Linux IP protocol tables and passes packets tranparently on to the
   underlying protocol in which it is not interested (bound).  The driver
   uses the NPI (Network Provider Interface) API.
*/

#include "os7/compat.h"
#include <linux/bitops.h>

#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>

#undef ASSERT

#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>

#define IP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define IP_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define IP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define IP_REVISION	"OpenSS7 $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/06/16 20:41:45 $"
#define IP_DEVICE	"SVR 4.2 STREAMS NPI IP Driver"
#define IP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define IP_LICENSE	"GPL"
#define IP_BANNER	IP_DESCRIP	"\n" \
			IP_EXTRA	"\n" \
			IP_REVISION	"\n" \
			IP_COPYRIGHT	"\n" \
			IP_DEVICE	"\n" \
			IP_CONTACT
#define IP_SPLASH	IP_DESCRIP	"\n" \
			IP_REVISION

#ifdef LINUX
MODULE_AUTHOR(IP_CONTACT);
MODULE_DESCRIPTION(IP_DESCRIP);
MODULE_SUPPORTED_DEVICE(IP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(IP_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ip");
#endif
#endif				/* LINUX */

#ifdef LFS
#define IP_DRV_ID	CONFIG_STREAMS_IP_MODID
#define IP_DRV_NAME	CONFIG_STREAMS_IP_NAME
#define IP_CMAJORS	CONFIG_STREAMS_IP_NMAJORS
#define IP_CMAJOR_0	CONFIG_STREAMS_IP_MAJOR
#define IP_UNITS	CONFIG_STREAMS_IP_NMINORS
#endif

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_IP_MODID));
MODULE_ALIAS("streams-driver-ip");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_IP_MAJOR));
MODULE_ALIAS("/dev/streams/ip");
MODULE_ALIAS("/dev/streams/ip/*");
MODULE_ALIAS("/dev/streams/clone/ip");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(IP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(IP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(IP_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(IP_CMAJOR_0) "-" __stringify(IP_CMINOR));
MODULE_ALIAS("/dev/ip");
MODULE_ALIAS("/dev/inet/ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		IP_DRV_ID
#define DRV_NAME	IP_DRV_NAME
#define CMAJORS		IP_CMAJORS
#define CMAJOR_0	IP_CMAJOR_0
#define UNITS		IP_UNITS
#ifdef MODULE
#define DRV_BANNER	IP_BANNER
#else				/* MODULE */
#define DRV_BANNER	IP_SPLASH
#endif				/* MODULE */

STATIC struct module_info ip_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC int ip_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int ip_close(queue_t *, int, cred_t *);

STATIC int ip_rput(queue_t *, mblk_t *);
STATIC int ip_rsrv(queue_t *);

STATIC struct qinit ip_rinit = {
	.qi_putp = ip_rput,		/* Read put (msg from below) */
	.qi_srvp = ip_rsrv,		/* Read queue service */
	.qi_qopen = ip_open,		/* Each open */
	.qi_qclose = ip_close,		/* Last close */
	.qi_minfo = &ip_minfo,		/* Information */
};

STATIC int ip_wput(queue_t *, mblk_t *);
STATIC int ip_wsrv(queue_t *);

STATIC struct qinit ip_winit = {
	.qi_putp = ip_wput,		/* Write put (msg from above) */
	.qi_srvp = ip_wsrv,		/* Write queue service */
	.qi_minfo = &ip_minfo,		/* Information */
};

MODULE_STATIC struct streamtab ip_info = {
	.st_rdinit = &ip_rinit,		/* Upper read queue */
	.st_wrinit = &ip_winit,		/* Lower read queue */
};

/*
   Queue put and service return values 
 */
#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7

/*
 */
