/*****************************************************************************

 @(#) $Id: ip_strm_mod.h,v 0.9.2.6 2008-04-25 11:39:33 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2008-04-25 11:39:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ip_strm_mod.h,v $
 Revision 0.9.2.6  2008-04-25 11:39:33  brian
 - updates to AGPLv3

 Revision 0.9.2.5  2007/08/15 05:35:46  brian
 - GPLv3 updates

 Revision 0.9.2.4  2007/08/14 03:31:19  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/06/17 01:57:38  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __LOCAL_IP_STRM_MOD_H__
#define __LOCAL_IP_STRM_MOD_H__

#ident "@(#) $RCSfile: ip_strm_mod.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#if defined(__KERNEL__)

#include <linux/skbuff.h>
#include <linux/netdevice.h>

/*
 * Somewhere between 2.3.35 and 2.3.51 tbusy disappeared and was
 * replaced by the netif_start/stop_queue mechanism.  We assume
 * that 2.3.x means the newer style.  So if you have problems with
 * this with a 2.2.35-ish kernel, download a newer kernel source.
 */
#ifdef KERNEL_2_3		/* 2.3, 2.4 kernel or beyond */
#define	ism_dev		net_device
#ifndef enet_statistics
#define enet_statistics	net_device_stats
#endif
#else				/* 2.0 - 2.2 kernel */
#define	ism_dev		device
#define netif_start_queue(dev)	(dev)->tbusy = 0
#define netif_stop_queue(dev)	(dev)->tbusy = 1
#define netif_wake_queue(dev)	mark_bh(NET_BH)
#define netif_queue_stopped(dev) ((dev)->tbusy)
#endif

typedef struct ip_to_streams {
	unsigned long dl_magic;
	unsigned long dl_sap;
	queue_t *dl_q;
	queue_t *dl_rdq;
	queue_t *dl_wrq;
	int dl_err_prim;
	int dlstate;
	int dl_m_error;
	int dl_tli_err;
	int dl_unix_err;
	int dl_reason;
	int dl_retry_proto;
	int dl_lower_ptr;
	int dl_bufcall_id;
	int contype;
	int ip_open;
	int dev_registered;
	struct enet_statistics stats;
	char myname[16];		/* something for mydev.name to point to */
	struct ism_dev mydev;		/* a device struct, not a pointer */
} ip_to_streams_minor_t, *ip_to_streams_minor_p;

#endif

#define DBG_OPEN 	0x001
#define DBG_WPUT 	0x002
#define DBG_PUT 	0x004
#define DBG_SQUAWK 	0x008
#define DBG_UPR_PROTOS 	0x010
#define DBG_SVC 	0x020
#define DBG_DATA 	0x040
#define DBG_ALLOCB 	0x080
#define DBG_MERROR 	0x100
#define DBG_ERROR_ACK	0x200
#define DBG_SQUAWKP	0x400

#define DL_MAGIC	0x12345

/*
 * Private ioctl to set interface name.
 *
 * Might be defined in sockios.h already.  We will use the same
 * value that sockios.h would use.
 */
#ifndef SIOCSIFNAME
#define SIOCSIFNAME     0x8923	/* ioctl code */
#endif

#define IP	0x1

#endif				/* __LOCAL_IP_STRM_MOD_H__ */
