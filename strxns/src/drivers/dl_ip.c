/*****************************************************************************

 @(#) $RCSfile: dl_ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/04/13 18:32:50 $

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

 Last Modified $Date: 2006/04/13 18:32:50 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl_ip.c,v $
 Revision 0.9.2.1  2006/04/13 18:32:50  brian
 - working up DL and NP drivers.

 *****************************************************************************/

#ident "@(#) $RCSfile: dl_ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/04/13 18:32:50 $"

static char const ident[] = "$RCSfile: dl_ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/04/13 18:32:50 $";

/*
 *  This is a DLPI driver for the IP subsystem.  The purpose of the driver is to directly access the
 *  packet layer beneath IP for the purposes of network interface taps and testing (primarily for
 *  testing SCTP and RTP).  It is effectively equivalent to a raw socket, except that it has the
 *  ability to intercept IP packets.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define d_tst_bit(nr,addr)	test_bit(nr,addr)
#define d_set_bit(nr,addr)	__set_bit(nr,addr)
#define d_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#include <sys/dlpi.h>

#define DL_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define DL_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define DL_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define DL_REVISION	"OpenSS7 $RCSfile: dl_ip.c,v $ $Name:  $ ($Revision: 0.9.2.1 $) $Date: 2006/04/13 18:32:50 $"
#define DL_DEVICE	"SVR 4.2 STREAMS DLPI DL_IP Data Link Provider"
#define DL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DL_LICENSE	"GPL"
#define DL_BANNER	DL_DESCRIP	"\n" \
			DL_EXTRA	"\n" \
			DL_COPYRIGHT	"\n" \
			DL_DEVICE	"\n" \
			DL_CONTACT
#define DL_SPLASH	DL_DESCRIP	"\n" \
			DL_REVISION

#ifdef LINUX
MODULE_AUTHOR(DL_CONTACT);
MODULE_DESCRIPTION(DL_DESCRIP);
MODULE_SUPPORTED_DEVICE(DL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DL_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-dl_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define DL_IP_DRV_ID	CONFIG_STREAMS_DL_IP_MODID
#define DL_IP_DRV_NAME	CONFIG_STREAMS_DL_IP_NAME
#define DL_IP_CMAJORS	CONFIG_STREAMS_DL_IP_NMAJOR
#define DL_IP_CMAJOR_0	CONFIG_STREAMS_DL_IP_MAJOR
#define DL_IP_UNITS	CONFIG_STREAMS_DL_IP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_DL_MODID));
MODULE_ALIAS("streams-driver-dl_ip");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_DL_MAJOR));
MODULE_ALIAS("/dev/streams/dl_ip");
MODULE_ALIAS("/dev/streams/dl_ip/*");
MODULE_ALIAS("/dev/streams/clone/dl_ip");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-" __stringify(DL_CMINOR));
MODULE_ALIAS("/dev/dl_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ===================
 */

#define DRV_ID		DL_IP_DRV_ID
#define CMAJORS		DL_IP_CMAJORS
#define CMAJOR_0	DL_IP_CMAJOR_0
#define UNITS		DL_IP_UNITS
#ifdef MODULE
#define DRV_BANNER	DL_IP_BANNER
#else				/* MODULE */
#define DRV_BANNER	DL_IP_SPLASH
#endif				/* MODULE */

STATIC struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_hiwat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat dl_mstat = {
};

/* Upper multiplex is a DL provider following the DLPI. */

STATIC streamscall int dl_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int dl_qclose(queue_t *, int, cred_t *);

STATIC struct qinit dl_rinit = {
	.qi_putp = &ss7_oput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_osrv,		/* Read service procedure */
	.qi_qopen = &dl_qopen,		/* Each open */
	.qi_qclose = &dl_qclose,	/* Last close */
	.qi_minfo = &dl_minfo,		/* Module information */
	.qi_mstat = &dl_mstat,		/* Module statistics */
};

STATIC struct qinit dl_winit = {
	.qi_putp = &ss7_iput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_isrv,		/* Read service procedure */
	.qi_minfo = &dl_minfo,		/* Module information */
	.qi_mstat = &dl_mstat,		/* Module statistics */
};

STATIC struct streamtab dl_info = {
	.st_rdinit = &dl_rinit,		/* Upper read queue */
	.st_wrinit = &dl_winit,		/* Upper write queue */
};

/*
 *  Addressing:
 *
 *  PPAs are network interface device indexes.  PPA 0 is for all devices.
 *
 *  DLSAPs are IP Protocol numbers.  Initially it is only supporting IPv4.
 */
