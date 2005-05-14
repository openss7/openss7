/*****************************************************************************

 @(#) $RCSfile: ip_strm_mod.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/05/14 08:26:13 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/05/14 08:26:13 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: ip_strm_mod.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/05/14 08:26:13 $"

static char const ident[] =
    "$RCSfile: ip_strm_mod.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/05/14 08:26:13 $";

#include "os7/compat.h"

#include <linux/slab.h>

#include <linux/bitops.h>
#include <linux/net.h>
#include <linux/in.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/if_arp.h>

#include <sys/dlpi.h>

#define IP_TO_STREAMS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 STREAMS FOR LINUX"
#define IP_TO_STREAMS_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define IP_TO_STREAMS_COPYRIGHT		"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define IP_TO_STREAMS_REVISION		"LfS $RCSfile: ip_strm_mod.c,v $ $Name:  $ ($Revision: 0.9.2.11 $) $Date: 2005/05/14 08:26:13 $"
#define IP_TO_STREAMS_DEVICE		"SVR 4.2 STREAMS IP STREAMS Module (IP_TO_STREAMS)"
#define IP_TO_STREAMS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define IP_TO_STREAMS_LICENSE		"GPL"
#define IP_TO_STREAMS_BANNER		IP_TO_STREAMS_DESCRIP	"\n" \
					IP_TO_STREAMS_EXTRA	"\n" \
					IP_TO_STREAMS_REVISION	"\n" \
					IP_TO_STREAMS_COPYRIGHT	"\n" \
					IP_TO_STREAMS_DEVICE	"\n" \
					IP_TO_STREAMS_CONTACT
#define IP_TO_STREAMS_SPLASH		IP_TO_STREAMS_DEVICE	"\n" \
					IP_TO_STREAMS_REVISION

#ifdef LINUX
MODULE_AUTHOR(IP_TO_STREAMS_CONTACT);
MODULE_DESCRIPTION(IP_TO_STREAMS_DESCRIP);
MODULE_SUPPORTED_DEVICE(IP_TO_STREAMS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(IP_TO_STREAMS_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ip_strm_mod");
#endif
#endif				/* LINUX */

#ifdef LFS
#define IP_TO_STREAMS_MOD_ID	CONFIG_STREAMS_IP_TO_STREAMS_MODID
#define IP_TO_STREAMS_MOD_NAME	CONFIG_STREAMS_IP_TO_STREAMS_NAME
#endif				/* LFS */

#define MOD_ID		IP_TO_STREAMS_MOD_ID
#define MOD_NAME	IP_TO_STREAMS_MOD_NAME

#ifdef MODULE
#define MOD_BANNER	IP_TO_STREAMS_BANNER
#else				/* MODULE */
#define MOD_BANNER	IP_TO_STREAMS_SPLASH
#endif				/* MODULE */

STATIC struct module_info ip_to_streams_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 20000,		/* Hi water mark */
	.mi_lowat = 2000,		/* Lo water mark */
};

STATIC int ip_to_streams_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int ip_to_streams_close(queue_t *, int, cred_t *);

STATIC int ip_to_streams_rput(queue_t *, mblk_t *);
STATIC int ip_to_streams_rsrv(queue_t *);
STATIC int ip_to_streams_wput(queue_t *, mblk_t *);
STATIC int ip_to_streams_wsrv(queue_t *);

STATIC struct qinit ip_to_streams_rinit = {
	.qi_putp = ip_to_streams_rput,	/* Read put (message from below) */
	.qi_srvp = ip_to_streams_rsrv,	/* Read service */
	.qi_qopen = ip_to_streams_open,	/* Each open */
	.qi_qclose = ip_to_streams_close,	/* Last close */
	.qi_minfo = &ip_to_streams_minfo,	/* Information */
};

STATIC struct qinit ip_to_streams_winit = {
	.qi_putp = ip_to_streams_wput,	/* Read put (message from below) */
	.qi_srvp = ip_to_streams_wsrv,	/* Read service */
	.qi_minfo = &ip_to_streams_minfo,	/* Information */
};

STATIC struct streamtab ip_to_streams_info = {
	.st_rdinit = &ip_to_streams_rinit,	/* Upper read queue */
	.st_wrinit = &ip_to_streams_winit,	/* Upper write queue */
};

/************************************************************************
*									*
*                   Gcom DLPI <-> Linux SKBUF Driver		        *
*									*
*************************************************************************
*									*
*                  Copyright (C) 1995,1996,1997 Gcom, Inc		*
*									*
*									*
*	Author: Among others Mikel L. Matthews Gcom, Inc.		*
*									*
* Copyright (C) 1997-1999  Mikel Matthews, Gcom, Inc <mikel@gcom.com>	*
*                          Dave Grothe, Gcom, Inc <dave@gcom.com>	*
*									*
************************************************************************/
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 * This is a pushable streams module that bridges a DLPI stream to
 * an IP masquerading as a network driver.
 *
 * The way you use it is as follows:
 *
 * Open your DLPI stream file.
 *
 * Push this module.
 *
 * Do DLPI attach and bind.
 *
 * Send SIOCSIFNAME ioctl with the desired interface name.
 *
 * The ioctl causes the module to register itself with IP as a network
 * interface driver.
 *
 * Do the appropriate ifconfig and route commands.
 *
 * You can tear down the IP side by doing ifconfig <name> off.  You can
 * tear down the whole thing by closing the streams file.
 */

#ifdef LFS
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
#define	KERNEL_2_0
#else
#define	KERNEL_2_1
# if LINUX_VERSION_CODE > KERNEL_VERSION(2,3,0)
# define KERNEL_2_3
# endif
#endif
#endif

#include "ip_strm_mod.h"

/************************************************************************
*                        Function Prototypes                            *
************************************************************************/

/*
 * Functions in this file
 */
extern int ip_strm_init(struct ism_dev *dev);

extern int ip_to_streams_conn_req(ip_to_streams_minor_t * minor_ptr, mblk_t *mp, int retry);
extern int ip_to_streams_proto(ip_to_streams_minor_t *, mblk_t *, int);

#ifdef GCOM
extern void netman_hex_mp(mblk_t *, char *);
extern void Rsys_print_traced_token(char *bufp);
extern void Rsys_hex_print(char *, unsigned char *, int);
#endif

int ip_to_streamsdevflag = 0;

/************************************************************************
*                       Storage Declarations                            *
************************************************************************/

unsigned long ip_to_streams_debug_mask = 0;

#define IP_STRM_MTU (1500)

/************************************************************************
*                           ip_to_streams_open                          *
*************************************************************************
*									*
* The streams open routine. Called when this is pushed on to the stack	*
*									*
************************************************************************/
/**
 *  @internal
 *  @brief STREAMS open routine
 *  @param q the read queue pointer of the newly created queue pair
 *  @param devp a pointer to the dev_t (ignored for modules)
 *  @param oflag open flags
 *  @param sflag streams flag, should always be MODOPEN
 *  @param credp pointer to a cred_t credential structure of the pushing
 *  process
 *
 *  This is the STREAMS open entry point to the IP_STRM module.  This entry
 *  point is called when the IP_STRM module is first pushed onto the stack as
 *  well as each time that a module is pushed above it.
 */
STATIC int
ip_to_streams_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *credp)
{
	int err;
	ip_to_streams_minor_t *minor_ptr;

	if (ip_to_streams_debug_mask & (DBG_OPEN))
		cmn_err(CE_NOTE, "ip_to_streams_open: q=%p sflag=%d", q, sflag);

	err = 0;
	if (q->q_ptr != NULL)
		goto quit;	/* already open */
	err = ENXIO;
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		goto quit;
	err = ENOMEM;
	if ((minor_ptr = (ip_to_streams_minor_p)
	     kmem_alloc(sizeof(ip_to_streams_minor_t), KM_NOSLEEP)) == NULL)
		goto quit;

	memset(minor_ptr, 0, sizeof(*minor_ptr));	/* clear to zero */

	minor_ptr->dl_magic = DL_MAGIC;
	q->q_ptr = (caddr_t) minor_ptr;
	WR(q)->q_ptr = (caddr_t) minor_ptr;

	minor_ptr->dl_rdq = q;
	minor_ptr->dl_wrq = WR(q);
	minor_ptr->dl_q = q;
	minor_ptr->dl_err_prim = -1;	/* ensure no retry */

	strcpy(minor_ptr->myname, "is");	/* initial name Ip/Streams */
#if defined(KERNEL_2_3)
	strcpy(minor_ptr->mydev.name, "is");	/* initial name Ip/Streams */
#else
	minor_ptr->mydev.name = minor_ptr->myname;
#endif
	minor_ptr->mydev.init = ip_strm_init;
	minor_ptr->mydev.priv = minor_ptr;

	if (ip_to_streams_debug_mask & (DBG_OPEN))
		cmn_err(CE_CONT, "\nip_to_streams_open succeeded");

	qprocson(q);
	return (0);		/* success */
      quit:
	return (err);

}				/* ip_to_streams_open */

/************************************************************************
*                          ip_to_streams_close                          *
*************************************************************************
*									*
* Called when closing the stream					*
*									*
************************************************************************/
/**
 *  @internal
 *  @brief STREAMS close routine
 *  @param q pointer to the read queue of the queue pair
 *  @param oflag open flags
 *  @param credp pointer to a credentials structure
 *
 *  
 */
STATIC int
ip_to_streams_close(queue_t *q, int oflag, cred_t *credp)
{
	ip_to_streams_minor_t *minor_ptr;
	(void) oflag;
	(void) credp;
	if (ip_to_streams_debug_mask & (DBG_OPEN))
		cmn_err(CE_NOTE, "%s: q=%p ", __FUNCTION__, q);
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", MOD_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif
	minor_ptr = (ip_to_streams_minor_t *) q->q_ptr;
	if (minor_ptr != NULL && minor_ptr->dl_magic == DL_MAGIC) {
		if (ip_to_streams_debug_mask & (DBG_OPEN))
			cmn_err(CE_NOTE, "%s: %s", __FUNCTION__, minor_ptr->myname);
		if (minor_ptr->dev_registered != 0)	/* still open to IP */
			unregister_netdev(&minor_ptr->mydev);
		minor_ptr->dl_magic = ~DL_MAGIC;
	} else {
		if (ip_to_streams_debug_mask & (DBG_OPEN))
			cmn_err(CE_NOTE, "%s: invalid minor ptr q_ptr=%p", __FUNCTION__, q->q_ptr);
		goto quit;
	}
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	kmem_free(q->q_ptr, sizeof(*minor_ptr));
	q->q_ptr = NULL;	/* zot the q ptrs */
	WR(q)->q_ptr = NULL;	/* zot the q ptrs */
	goto quit;
      quit:
	return (0);
}

/************************************************************************
*                          ip_to_streams_ioctl                          *
*************************************************************************
*									*
* Have a look at an ioctl received from the user.  Return 1 if the	*
* ioctl is to be forwarded downstream.  Return 0 if we handled it here.	*
*									*
************************************************************************/
int
ip_to_streams_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *iocp;
	ip_to_streams_minor_t *minor_ptr;
	mblk_t *xmp = mp->b_cont;
	int result = 0;

	iocp = (struct iocblk *) mp->b_rptr;	/* the request header */
	if ((mp->b_wptr - mp->b_rptr) < sizeof(struct iocblk *))	/* small */
		return (1);	/* let driver deal with it */

	minor_ptr = (ip_to_streams_minor_t *) q->q_ptr;
	switch (iocp->ioc_cmd) {
	case SIOCSIFNAME:	/* set interface name */
		/* 
		 * This ioctl is sent by the streams user.  It is the last step
		 * in making this stream available to IP.  Once we know the
		 * name we register the device with IP as an interface.
		 *
		 * As an added feature, the streams user can set the name
		 * to the empty string and we will unregister from IP.  When
		 * we unregister we leave the streams side alone so that a
		 * simple "set if-name" ioctl will make the interface appear
		 * under IP again.
		 */
		if (xmp != NULL && *xmp->b_rptr != 0) {	/* name specified */
			strncpy(minor_ptr->myname, xmp->b_rptr, sizeof(minor_ptr->myname));
#if defined(KERNEL_2_3)
			strcpy(minor_ptr->mydev.name, minor_ptr->myname);
#else
			minor_ptr->mydev.name = minor_ptr->myname;
#endif
			if ((result = register_netdev(&minor_ptr->mydev)) != 0)
				printk("ip_to_streams_ioctl: " "register_netdev(%s) failed: %d\n",
				       minor_ptr->myname, result);
			else
				printk("ip_to_streams_ioctl: SIOCSIFNAME: %s\n", minor_ptr->myname);

			minor_ptr->dev_registered = result == 0;
		} else {	/* nullify name, unregister */

			unregister_netdev(&minor_ptr->mydev);
			strcpy(minor_ptr->myname, "is");
			minor_ptr->contype = 0;	/* not connected */
			minor_ptr->ip_open = 0;	/* not open to IP now */
		}

		if (xmp != NULL) {
			freemsg(xmp);	/* don't return any data */
			mp->b_cont = NULL;
		}
		break;
	default:
		printk("ip_to_streams_ioctl: undefined ioctl: 0x%x\n", iocp->ioc_cmd);
		result = -EINVAL;
		break;
	}

	/* 
	 * If you exit the switch then we are going to reply to the
	 * ioctl and not forward it downstream.
	 */
	if (result) {		/* error */
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_count = 0;	/* no data */
	} else {		/* success */

		mp->b_datap->db_type = M_IOCACK;
		if ((xmp = mp->b_cont) == NULL)	/* no chained buf */
			iocp->ioc_count = 0;	/* no data */
		else		/* a response */
			iocp->ioc_count = xmp->b_wptr - xmp->b_rptr;
	}

	iocp->ioc_error = result;
	qreply(q, mp);
	return (0);		/* do not forward */

}				/* ip_to_streams_ioctl */

/************************************************************************
*                           ip_to_streams_wput                          *
*************************************************************************
*									*
* Write side put routine						*
*									*
************************************************************************/

STATIC int
ip_to_streams_wput(queue_t *q, mblk_t *mp)
{
	ip_to_streams_minor_t *minor_ptr;

	if (ip_to_streams_debug_mask & DBG_PUT)
		cmn_err(CE_CONT, "\nip_to_streams_wput: q=%p mp=%p\n", q, mp);

	minor_ptr = (ip_to_streams_minor_t *) q->q_ptr;

	/* make sure the data structure is valid */
	if (minor_ptr == (ip_to_streams_minor_t *) NULL || minor_ptr->dl_magic != DL_MAGIC) {
		if (ip_to_streams_debug_mask & DBG_SQUAWK)
			cmn_err(CE_NOTE, "ip_to_streams_wput: bad minor");

		freemsg(mp);
		return (0);
	}

	switch (mp->b_datap->db_type) {
	case M_DATA:
		if (canputnext(q))	/* data uses flow control */
			putnext(q, mp);
		else if (!putq(q, mp))
			freemsg(mp);
		break;

	case M_PROTO:
	case M_PCPROTO:
	{
		int err;
		union DL_primitives *dlp;

		err = 0;
		dlp = (union DL_primitives *) mp->b_rptr;

		switch (dlp->dl_primitive) {
		case DL_UNITDATA_REQ:
		{
			if (ip_to_streams_debug_mask & DBG_WPUT)
				printk("ip_to_streams_wput: DL_UNITDATA_REQ:\n");

			if (canputnext(q))	/* data uses flow control */
				putnext(q, mp);
			else if (!putq(q, mp))
				freemsg(mp);
			return (0);
		}

		case DL_INFO_REQ:
		{
			if (ip_to_streams_debug_mask & DBG_WPUT)
				printk("ip_to_streams_wput: DL_INFO_REQ:\n");

			putnext(q, mp);
			break;
		}

		case DL_BIND_REQ:
		{
			dl_bind_req_t *dlp;

			if (ip_to_streams_debug_mask & DBG_WPUT)
				printk("ip_to_streams_wput:DL_BIND_REQ:\n");

			minor_ptr->dlstate = DL_BIND_PENDING;

			dlp = (dl_bind_req_t *) mp->b_rptr;
			minor_ptr->dl_sap = dlp->dl_sap;	/* save SAP */

			putnext(q, mp);
			break;
		}

		case DL_UNBIND_REQ:
		{
			if (ip_to_streams_debug_mask & DBG_WPUT)
				printk("DL_UNBIND_REQ:\n");
			minor_ptr->dlstate = DL_UNBIND_PENDING;
			minor_ptr->dl_sap = 0;

			putnext(q, mp);
			break;
		}

		case DL_ATTACH_REQ:
		{
			if (ip_to_streams_debug_mask & DBG_WPUT)
				printk("DL_ATTACH_REQ:\n");

			minor_ptr->dlstate = DL_ATTACH_PENDING;
			putnext(q, mp);

			break;
		}

		case DL_DETACH_REQ:
		{
			if (ip_to_streams_debug_mask & DBG_WPUT)
				printk("DL_DETACH_REQ:\n");

			minor_ptr->dlstate = DL_DETACH_PENDING;
			putnext(q, mp);
			break;
		}
			break;
		}
		break;
	}

	case M_IOCTL:
		if (ip_to_streams_ioctl(q, mp))	/* forward */
			putnext(q, mp);
		break;

	case M_FLUSH:
		if (*mp->b_rptr & FLUSHW) {
			flushq(q, FLUSHDATA);
		}

		if (*mp->b_rptr & FLUSHR) {
			flushq(RD(q), FLUSHDATA);
			*mp->b_rptr &= ~FLUSHW;
			qreply(q, mp);
		} else
			freemsg(mp);

		break;

	default:
		freemsg(mp);
		break;

	}			/* switch */

	return (0);

}				/* ip_to_streams_wput */

/************************************************************************
*                        ip_to_streams_allocb                           *
*************************************************************************
*									*
* Allocate a STREAMS buffer.  Trigger retry mechanism if fail.		*
*									*
* Parameters:								*
*	q	The queue to use for retries.				*
*		NULL q defeats retry procedure.				*
*	mp	The message to insert into the queue if retrying.	*
*		A NULL mp means nothing to insert into the queue	*
*		but the retry occurs anyway.				*
*	type	The type of message to allocate (M_PROTO, ...)		*
*	size	The size of the STREAMS buffer to allocate.		*
*	priority The buffer allocation priority.			*
*	head	Queue the buffer at the head of the queue, not tail.	*
*	bufcall_id Pointer to integer where bufcall id is to be stored.	*
*									*
* Returns:								*
*	ptr	If buffer was allocated.				*
*	NULL	If buffer not allocated.  bufcall() has been called	*
*		and the 'mp' has been placed into the 'q'.		*
*									*
************************************************************************/
mblk_t *
ip_to_streams_allocb(queue_t *q, mblk_t *mp, int type, int size, int priority, int head,
		     int *bufcall_id)
{
	mblk_t *msg;

	if (ip_to_streams_debug_mask & DBG_ALLOCB)
		cmn_err(CE_CONT, "\nip_to_streams_allocb: called\n");

	msg = allocb(size, priority);

	if (msg != (mblk_t *) NULL) {	/* buffer allocated */
		msg->b_datap->db_type = type;
		return (msg);
	}
	return ((mblk_t *) NULL);

}				/* ip_to_streams_allocb */

/************************************************************************
*                          ip_to_streams_merror                         *
*************************************************************************
*									*
* Send an M_ERROR upstream.						*
*									*
* We will use the mp given to us if we can.				*
*									*
************************************************************************/
int
ip_to_streams_m_error(ip_to_streams_minor_t * minor_ptr, mblk_t *mp, int errno, int retry)
{
	if (ip_to_streams_debug_mask & DBG_SQUAWK)
		cmn_err(CE_CONT, "\nip_to_streams_m_error: called\n");

	if (minor_ptr == (ip_to_streams_minor_t *) NULL) {
		if (ip_to_streams_debug_mask & DBG_SQUAWK)
			cmn_err(CE_NOTE, "ip_to_streams_m_error: bad or detached minor %p",
				minor_ptr);

		if (mp != (mblk_t *) NULL)
			freemsg(mp);
		return (1);	/* message "sent" */
	}

	flushq(minor_ptr->dl_rdq, FLUSHALL);	/* ensure queue empty */
	minor_ptr->dl_retry_proto = 0;	/* TLI proto to retry */
	minor_ptr->dl_err_prim = -1;	/* for error ack retry */
	minor_ptr->dlstate = 0;	/* go to null state */

	if (mp == (mblk_t *) NULL) {
		mp = ip_to_streams_allocb(minor_ptr->dl_wrq, mp, M_ERROR, 1, BPRI_HI, retry,
					  &minor_ptr->dl_bufcall_id);

		minor_ptr->dl_m_error = errno | 0x100;	/* set retry flag */
		if (mp == (mblk_t *) NULL)
			return (0);	/* deferred */
	} else {
		mp->b_datap->db_type = M_ERROR;
		mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	}

	*mp->b_wptr++ = errno;	/* plant error code */

	minor_ptr->dl_m_error = 0;	/* unset retry flag */

#ifdef GCOM
	if (ip_to_streams_debug_mask & (DBG_UPR_PROTOS | DBG_MERROR))
		rsys_decode_msg(rsys_nextq_name(minor_ptr->dl_rdq), "ip_to_streams_m_error: To", mp,
				0);
#endif

	putnext(minor_ptr->dl_rdq, mp);
	return (0);
}				/* ip_to_streams_merror */

/************************************************************************
*                          ip_to_streams_error_ack                      *
*************************************************************************
*									*
* Send an error_ack upstream.						*
*									*
* Return 1 if succeed, 0 if deferred.					*
*									*
************************************************************************/
int
ip_to_streams_error_ack(ip_to_streams_minor_t * minor_ptr, mblk_t *mp, long err_prim,
			long tli_error, long unix_error, int retry)
{
	dl_error_ack_t *p;

	if (ip_to_streams_debug_mask & DBG_SQUAWK)
		cmn_err(CE_CONT, "\nip_to_streams_error_ack: called\n");

	if (mp == (mblk_t *) NULL || mp->b_datap->db_lim - mp->b_datap->db_base < sizeof(*p)
	    || (!retry && (ip_to_streams_debug_mask & DBG_ALLOCB))
	    ) {			/* must allocate a buffer */
		if (mp != (mblk_t *) NULL)
			freemsg(mp);
		mp = ip_to_streams_allocb(minor_ptr->dl_rdq, (mblk_t *) NULL, M_PCPROTO, sizeof(*p),
					  BPRI_MED, retry, &minor_ptr->dl_bufcall_id);
		if (mp == (mblk_t *) NULL) {
			minor_ptr->dl_err_prim = err_prim;
			minor_ptr->dl_tli_err = tli_error;
			minor_ptr->dl_unix_err = unix_error;
			return (0);	/* deferred */
		}
	} else {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	}

	minor_ptr->dl_err_prim = -1;	/* ensure no retry */

	/* 
	 * Have a buffer, build error_ack and send it
	 */
	mp->b_rptr = mp->b_datap->db_base;
	mp->b_wptr = mp->b_datap->db_base;

	p = (dl_error_ack_t *) mp->b_wptr;
	p->dl_primitive = DL_ERROR_ACK;
	p->dl_error_primitive = err_prim;
	p->dl_errno = tli_error;
	p->dl_unix_errno = unix_error;
	mp->b_wptr += sizeof(*p);

	putnext(minor_ptr->dl_rdq, mp);
	return (1);		/* succeed */

}				/* ip_to_streams_error_ack */

/************************************************************************
*                         ip_to_streams_ok_ack	                        *
*************************************************************************
*									*
* Send an ok_ack upstream.						*
*									*
* Return 1 if succeed, 0 if deferred.					*
*									*
************************************************************************/
int
ip_to_streams_ok_ack(ip_to_streams_minor_t * minor_ptr, mblk_t *mp, long ok_prim, int retry)
{
	dl_ok_ack_t *p;

	if (ip_to_streams_debug_mask & DBG_SQUAWK)
		cmn_err(CE_CONT, "\nip_to_streams_ok_ack: called\n");

	if (minor_ptr == (ip_to_streams_minor_t *) NULL || minor_ptr->dl_magic != DL_MAGIC) {
		if (ip_to_streams_debug_mask & DBG_SQUAWK)
			cmn_err(CE_NOTE, "ip_to_streams_ok_ack: bad minor");

		if (mp != (mblk_t *) NULL)
			freemsg(mp);
		return (1);
	}

	if (mp == (mblk_t *) NULL || mp->b_datap->db_lim - mp->b_datap->db_base < sizeof(*p)
	    || (!retry && (ip_to_streams_debug_mask & DBG_ALLOCB))
	    ) {			/* must allocate a buffer */
		if (mp != (mblk_t *) NULL)
			freemsg(mp);
		mp = ip_to_streams_allocb(minor_ptr->dl_rdq, (mblk_t *) NULL, M_PCPROTO, sizeof(*p),
					  BPRI_MED, retry, &minor_ptr->dl_bufcall_id);
		if (mp == (mblk_t *) NULL) {
			minor_ptr->dl_err_prim = ok_prim;
			minor_ptr->dl_tli_err = 0;
			minor_ptr->dl_unix_err = 0;
			return (0);	/* deferred */
		}
	} else {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	}

	minor_ptr->dl_err_prim = -1;	/* ensure no retry */

	/* 
	 * Have a buffer, build ok_ack and send it
	 */
	mp->b_rptr = mp->b_datap->db_base;
	mp->b_wptr = mp->b_datap->db_base;

	p = (dl_ok_ack_t *) mp->b_wptr;
	p->dl_primitive = DL_OK_ACK;
	p->dl_correct_primitive = ok_prim;
	mp->b_wptr += sizeof(*p);

	putnext(minor_ptr->dl_rdq, mp);
	return (1);		/* succeed */

}				/* ip_to_streams_ok_ack */

/************************************************************************
*                          ip_to_streams_rsrv                           *
*************************************************************************
*									*
* Upstream retry procedure.						*
*									*
* Return 1 for success, 0 for deferred.					*
*									*
************************************************************************/

STATIC int
ip_to_streams_rsrv(queue_t *q)
{
	ip_to_streams_minor_t *minor_ptr;
	int done;

	if (ip_to_streams_debug_mask & DBG_SVC)
		cmn_err(CE_CONT, "\nip_to_streams_rsrv: q=%p\n", q);

	minor_ptr = (ip_to_streams_minor_t *) q->q_ptr;

	if (minor_ptr == (ip_to_streams_minor_t *) NULL || minor_ptr->dl_magic != DL_MAGIC
	    || minor_ptr->dl_rdq != q) {
		return (0);
	}

	done = 1;
	if (minor_ptr->dl_m_error > 0) {
		flushq(q, FLUSHALL);	/* ensure queue empty */
		ip_to_streams_m_error(minor_ptr, (mblk_t *) NULL, minor_ptr->dl_m_error, 1);
		return (0);
	}

	if (minor_ptr->dl_err_prim >= 0) {
		flushq(q, FLUSHALL);	/* ensure queue empty */
		if (minor_ptr->dl_tli_err == 0) {
			done =
			    ip_to_streams_ok_ack(minor_ptr, (mblk_t *) NULL, minor_ptr->dl_err_prim,
						 1);
		} else {
			done =
			    ip_to_streams_error_ack(minor_ptr, (mblk_t *) NULL,
						    minor_ptr->dl_err_prim, minor_ptr->dl_tli_err,
						    minor_ptr->dl_unix_err, 1);
		}
	}

	if (!done)
		return (0);	/* still retrying error/ok acks */

	/* 
	 * Try other protos
	 */

	return (0);

}				/* ip_to_streams_rsrv */

/************************************************************************
*                             ip_to_streams_wsrv                        *
*************************************************************************
*									*
* This is the downstream retry routine.					*
*									*
************************************************************************/

STATIC int
ip_to_streams_wsrv(queue_t *q)
{
	ip_to_streams_minor_t *minor_ptr;
	mblk_t *mp;
	struct ism_dev *dev;
	int wakeup = 0;

	if (ip_to_streams_debug_mask & DBG_SVC)
		cmn_err(CE_CONT, "\nip_to_streams_wsrv: q=%p\n", q);

	minor_ptr = (ip_to_streams_minor_t *) q->q_ptr;

	if (minor_ptr == (ip_to_streams_minor_t *) NULL || minor_ptr->dl_magic != DL_MAGIC) {
		return (0);	/* bad minor */
	}

	dev = &minor_ptr->mydev;
	for (;;) {
		if ((mp = getq(minor_ptr->dl_wrq)) == NULL) {
			if (netif_queue_stopped(dev)) {
				netif_start_queue(dev);
				wakeup = 1;
			}
			break;	/* done */
		}

		if (mp->b_datap->db_type == M_DATA) {
			if (canputnext(q)) {
				putnext(q, mp);	/* send downstream */
				if (netif_queue_stopped(dev)) {
					netif_start_queue(dev);
					wakeup = 1;
				}
			} else {
				netif_stop_queue(dev);
				if (!putbq(q, mp))
					freemsg(mp);	/* FIXME */
				return (0);	/* quit */
			}
			minor_ptr->stats.tx_packets++;
		} else {
			if (ip_to_streams_proto(minor_ptr, mp, 1) == 0) {	/* deferred, queued 
										 */
				netif_stop_queue(dev);	/* stop output from above */
				return (0);	/* quit */
			}

			if (netif_queue_stopped(dev)) {
				netif_start_queue(dev);
				wakeup = 1;
			}
		}
	}

	if (wakeup)
		netif_wake_queue(dev);	/* have netif look at our queue */

	return (0);

}				/* ip_to_streams_wsrv */

/************************************************************************
*                           ip_to_streams_proto                         *
*************************************************************************
*									*
* Handle a DLPI proto message from above.				*
*									*
* Return 1 if succeed in handling msg, 0 if deferred.			*
*									*
************************************************************************/
int
ip_to_streams_proto(ip_to_streams_minor_t * minor_ptr, mblk_t *mp, int retry)
{
	queue_t *q = minor_ptr->dl_wrq;
	long *lp;

	if (ip_to_streams_debug_mask & DBG_SQUAWK)
		cmn_err(CE_CONT, "\nip_to_streams_proto: called\n");

	lp = (long *) mp->b_rptr;

	switch (*lp) {
	case DL_UNITDATA_REQ:
		if (canputnext(q)) {
			putnext(q, mp);
			return (1);
		}

		if (!putq(q, mp))
			freemsg(mp);

		break;

	default:
		if (canputnext(q)) {
			putnext(q, mp);
			return (1);
		}

		if (!putq(q, mp))
			freemsg(mp);
		break;
	}

	return (0);		/* deferred */

}				/* ip_to_streams_proto */

/************************************************************************
*                           ip_to_streams_rput                          *
*************************************************************************
*									*
* Handle a message from below.						*
*									*
************************************************************************/
STATIC int
ip_to_streams_rput(queue_t *q, mblk_t *mp)
{
	ip_to_streams_minor_t *minor_ptr = q->q_ptr;
	struct ism_dev *dev;
	int convert_to_skbuf(ip_to_streams_minor_t *, mblk_t *);

	if (minor_ptr == NULL) {
		cmn_err(CE_WARN, "ip_to_streams_rput: q->q_ptr NULL");
		freemsg(mp);
		return (0);
	}

	dev = &minor_ptr->mydev;
	if (ip_to_streams_debug_mask & DBG_PUT)
		cmn_err(CE_CONT, "\nip_to_streams_rput: q=%p mp=%p minor:0x%p, dev:0x%p\n", q, mp,
			minor_ptr, dev);

	/* checking, checking, checking... */
	if (minor_ptr == (ip_to_streams_minor_t *) NULL) {
		if (ip_to_streams_debug_mask & DBG_SQUAWK)
			cmn_err(CE_NOTE, "ip_to_streams_rput: bad stream");

		if (mp != (mblk_t *) NULL)
			freemsg(mp);

		/* mark an error, so the world can see it */
		minor_ptr->stats.rx_errors++;
		return (0);
	}

	/* 
	 * the way this is structured, you can convert the buffer to a linux skbuf
	 * and also send it upstairs to the process what has the stream open.
	 * Or you could pass this over to another 'device driver' that looks
	 * like a device that tcp_dump can look at.
	 * This way you could have a hook for tcp_dump into this area.
	 */
	switch (mp->b_datap->db_type) {
	case M_DATA:
	{
		if (minor_ptr->contype == IP) {
			/* if we are putting up to IP, convert to sk buff */
			if (convert_to_skbuf(minor_ptr, mp) != 0) {
				minor_ptr->stats.rx_errors++;
				break;
			}
			minor_ptr->stats.rx_packets++;
		} else {
			/* It will go here if we are not putting to IP */
			if (canputnext(q))
				putnext(q, mp);
			else if (!putq(q, mp))
				freemsg(mp);
		}
		break;
	}

	case M_PROTO:
	case M_PCPROTO:
	{
		int err;
		union DL_primitives *dlp;

		err = 0;
		dlp = (union DL_primitives *) mp->b_rptr;

		switch (dlp->dl_primitive) {
		case DL_BIND_ACK:
			if (ip_to_streams_debug_mask & DBG_WPUT)
				printk("ip_to_streams_rput: DL_BIND_ACK:\n");

			minor_ptr->dlstate = DL_DATAXFER;
			putnext(q, mp);
			break;

		case DL_UNITDATA_IND:
			/* 
			 * When we get the unitdata indication, we need to determine
			 * where to put it.  If an ip stream is connected here, we need
			 * to convert the msg buf to a linux sk buf and send it up
			 * to IP.
			 * 
			 * If ip is not connected, then we just need to do a putnext.
			 */
			if (minor_ptr->contype == IP) {
				if (convert_to_skbuf(minor_ptr, mp) != 0) {
					minor_ptr->stats.rx_errors++;
					break;
				}
				minor_ptr->stats.rx_packets++;
			} else {
				if (canputnext(q))
					putnext(q, mp);
				else if (!putq(q, mp))
					freemsg(mp);
			}
			break;

		default:	/* everything else just goes upstream */
			if (canputnext(q))
				putnext(q, mp);
			else if (!putq(q, mp))
				freemsg(mp);
			break;

		}
		break;
	}

	case M_FLUSH:
	{
		if (*mp->b_rptr & FLUSHR) {
			flushq(q, FLUSHDATA);
		}

		if (*mp->b_rptr & FLUSHW) {
			flushq(WR(q), FLUSHDATA);
			*mp->b_rptr &= ~FLUSHR;

			qreply(q, mp);
		} else
			freemsg(mp);

		break;
	}

	default:		/* send upstream */
	{
		putnext(q, mp);
		break;
	}

	}			/* switch */

	return (0);

}				/* ip_to_streams_rput */

/************************************************************************
*                           convert_to_skbuf                          	*
*************************************************************************
*									*
* Convert from a streams msg buffer to a Linux skbuf			*
*									*
************************************************************************/
int
convert_to_skbuf(ip_to_streams_minor_t * minor_ptr, mblk_t *mp)
{
	struct sk_buff *skb;
	int len;
	mblk_t *tmp = mp;
	char *ctmp;

	if ((mp->b_datap->db_type == M_PROTO)
	    || (mp->b_datap->db_type == M_PCPROTO)
	    )
		tmp = mp->b_cont;

	len = msgdsize(tmp);

	if (ip_to_streams_debug_mask & DBG_SQUAWK)
		cmn_err(CE_CONT, "\nconvert_to_skbuf: minor_ptr 0x%p, mp 0x%p, len %d\n", minor_ptr,
			mp, len);

	skb = dev_alloc_skb(len + 4);

	if (skb == NULL) {
		if (ip_to_streams_debug_mask & DBG_SQUAWK)
			cmn_err(CE_CONT, "\nconvert_to_skbuf: could not allocate %d bytes\n", len);
		return (-1);
	}

	/* set up the skb data structure */
	skb->dev = &minor_ptr->mydev;
	skb_reserve(skb, 2);
	skb->protocol = htons(ETH_P_IP);
	skb->pkt_type = PACKET_HOST;
	skb->h.raw = skb->data;
	skb->mac.raw = skb->data;
#if defined(KERNEL_2_1)
	skb->nh.raw = skb->data;
#endif
	skb->ip_summed = CHECKSUM_NONE;

	ctmp = skb->data;

	/* 
	 * skip over proto stuff, and copy the data over.
	 */

	while (tmp) {
		len = (tmp->b_wptr - tmp->b_rptr);
		ctmp = skb_put(skb, len);
		memcpy(ctmp, tmp->b_rptr, len);
#ifdef GCOM
		if (ip_to_streams_debug_mask & DBG_SQUAWK)
			Rsys_hex_print("convert_to_skbuf:", ctmp, len);
#endif
		tmp = tmp->b_cont;
	}

	/* make sure we don't lose any buffers */
	freemsg(mp);

	/* inform system that an IP packet is ready to go */
	netif_rx(skb);

	return (0);
}

/*
 * The higher levels take care of making this non-reentrant (it's
 * called with bh's disabled).
 */
/************************************************************************
*                           ip_strm_xmit                          	*
*************************************************************************
*									*
* This routine will convert from the Linux native buffer types to 	*
* a streams msgblk and then send down the buffer.			*
*									*
* We are not returning erros, here for a reason.  This is the same as if*
* IP 'lost' data.  When the bind completes, or a call completes, we 	*
* will then send the data out.						*
*									*
************************************************************************/

#if defined(KERNEL_2_1)
#define SKB_FREE(skb)    dev_kfree_skb(skb)
#else
#define SKB_FREE(skb)    dev_kfree_skb(skb, FREE_WRITE)
#endif

static int
ip_strm_xmit(struct sk_buff *skb, struct ism_dev *dev)
{
	ip_to_streams_minor_t *ipptr = (ip_to_streams_minor_t *) dev->priv;
	dl_unitdata_req_t *dl;
	mblk_t *mp, *mpt;

	if (ip_to_streams_debug_mask & DBG_PUT)
		cmn_err(CE_CONT, "\nip_strm_xmit: skb 0x%p dev 0x%p\n", skb, dev);

	/* make sure everything is ok */
	if (skb == NULL || dev == NULL) {
		if (ip_to_streams_debug_mask & DBG_PUT)
			cmn_err(CE_CONT, "\nip_strm_xmit: skb NULL or dev NULL\n");
		return (0);
	}

	/* is someone trying to confuse us? */
	if (ipptr == NULL) {
		if (ip_to_streams_debug_mask & DBG_PUT)
			cmn_err(CE_CONT, "\nip_strm_xmit: ipptr NULL\n");
		ipptr->stats.tx_errors++;
		SKB_FREE(skb);	/* don't want a rexmit */
		return (0);
	}

	if (ipptr->dl_magic != DL_MAGIC) {	/* not already open */
		if (ip_to_streams_debug_mask & DBG_PUT)
			cmn_err(CE_CONT, "\nip_strm_xmit: ipptr->dl_magic\n");
		ipptr->stats.tx_errors++;
		SKB_FREE(skb);	/* don't want a rexmit */
		return (0);
	}

	if (ipptr->dlstate != DL_DATAXFER) {	/* BIND not complete? */
		if (ip_to_streams_debug_mask & DBG_PUT)
			cmn_err(CE_CONT, "\nip_strm_xmit: state != DL_DATAXFER\n");
		ipptr->stats.tx_errors++;
		SKB_FREE(skb);	/* don't want a rexmit */
		return (0);
	}

	/* convert to a streams message */
	/* put a proto message in front of the data */

	mpt = allocb(sizeof(dl_unitdata_req_t), BPRI_MED);

	if (mpt == NULL) {
		if (ip_to_streams_debug_mask & DBG_PUT)
			cmn_err(CE_CONT, "\nip_strm_xmit: mpt NULL\n");
		ipptr->stats.tx_errors++;
		SKB_FREE(skb);	/* don't want a rexmit */
		return (0);
	}

	mpt->b_datap->db_type = M_PROTO;

	mp = allocb(skb->len, BPRI_MED);

	if (mp == NULL) {
		if (ip_to_streams_debug_mask & DBG_PUT)
			cmn_err(CE_CONT, "\nip_strm_xmit: mp NULL\n");
		ipptr->stats.tx_errors++;
		SKB_FREE(skb);	/* don't want a rexmit */
		freemsg(mpt);
		return (0);
	}

	mpt->b_cont = mp;	/* hook the message off the proto */

	mpt->b_wptr += sizeof(dl_unitdata_req_t);

	dl = (dl_unitdata_req_t *) mpt->b_rptr;

	dl->dl_primitive = DL_UNITDATA_REQ;
	dl->dl_dest_addr_length = 0;
	dl->dl_dest_addr_offset = 0;

	/* copy data over to the streams buffer */
	memcpy(mp->b_rptr, skb->data, skb->len);

#ifdef GCOM
	if (ip_to_streams_debug_mask & DBG_SQUAWK)
		Rsys_hex_print("ip_strm_xmit:", mp->b_rptr, skb->len);
#endif

	/* set write pointer to correct place */
	mp->b_wptr += skb->len;

	/* free the skb buffer */
	SKB_FREE(skb);

	/* send it down to the streams modules */

	if (canputnext(ipptr->dl_wrq)) {
		putnext(ipptr->dl_wrq, mpt);
		ipptr->stats.tx_packets++;
	} else {
		netif_stop_queue(dev);
		if (!putq(ipptr->dl_wrq, mpt))
			freemsg(mpt);
	}

	if (ip_to_streams_debug_mask & DBG_PUT)
		cmn_err(CE_CONT, "\nip_strm_xmit: putnext complete\n");

	return (0);
}

#undef SKB_FREE

/************************************************************************
*                           ip_strm_ioctl                               *
*************************************************************************
*									*
*									*
************************************************************************/
int
ip_strm_ioctl(struct ism_dev *dev, struct ifreq *ifr, int cmd)
{
	return (-EINVAL);

}				/* ip_strm_ioctl */

/************************************************************************
*                           enet_statistics                          	*
*************************************************************************
*									*
* return a pointer to the stats area					*
*									*
************************************************************************/
static struct enet_statistics *
get_stats(struct ism_dev *dev)
{
	ip_to_streams_minor_t *ipptr = (ip_to_streams_minor_t *) dev->priv;

	if (ipptr == NULL)
		return (NULL);

	return (&ipptr->stats);
}

/************************************************************************
*                           ip_strm_open                          	*
*************************************************************************
*									*
* This routine gets called when the ifconfig command is used to set up	*
* the IP address and device.						*
*									*
* We have to provide linkage between the streams and linux calls and	*
* data structure.  The streams side has already been done, now the linux*
* side has to finish up the job.					*
*									*
* The minor structure allocated by the streams open routine contains	*
* the dev structure used by ip.  The "priv" pointer in the dev struct	*
* points back to the minor structure.					*
*									*
************************************************************************/

int
ip_strm_open(struct ism_dev *dev)
{
	ip_to_streams_minor_t *ipptr = (ip_to_streams_minor_t *) dev->priv;

	if (ip_to_streams_debug_mask & (DBG_OPEN))
		cmn_err(CE_CONT, "ip_strm_open: dev 0x%p\n", dev);

	/* if the stream has not been opened, fail */
	if (ipptr == NULL)
		return (EIO);

	if (ip_to_streams_debug_mask & (DBG_OPEN))
		printk("ip_strm_open: %s: ipptr = 0x%px\n", ipptr->myname, ipptr);

	dev->flags |= IFF_POINTOPOINT;

	if (ip_to_streams_debug_mask & (DBG_OPEN)) {
		printk("ip_strm_open: ipptr: 0x%lx, dl_wrq: 0x%lx\n", (unsigned long) ipptr,
		       (unsigned long) ipptr->dl_wrq);
	}

	if (ipptr->dl_wrq == NULL) {
		if (ip_to_streams_debug_mask & (DBG_OPEN)) {
			printk("ip_strm_open: dl_wrq not setup correctly\n");
		}
		return (EIO);
	}

	/* this flag is used to control where we send the reads from the streams modules.  If it is 
	   IP, then we will convert to a LINUX sk_buf and send it up to ip, otherwise we send it to 
	   the process that has opened the stream. */
	ipptr->contype = IP;
	ipptr->ip_open = 1;

	return 0;
}

/************************************************************************
*                          ip_strm_close                                *
*************************************************************************
*									*
* Called when IP wants to close our device.				*
*									*
* The STREAMS side is still set up, so we just stop steering incoming	*
* packets to IP.							*
*									*
* Loadable modules treat this as the last thing before module unload.	*
*									*
* In our case the stream is still open and a new ifconfig can re-	*
* instate this IP interface.						*
*									*
************************************************************************/
int
ip_strm_close(struct ism_dev *dev)
{
	ip_to_streams_minor_t *ipptr = (ip_to_streams_minor_t *) dev->priv;

	netif_stop_queue(dev);
	ipptr->contype = 0;
	ipptr->ip_open = 0;
	return (0);

}				/* ip_strm_close */

/************************************************************************
*                           ip_strm_init                          	*
*************************************************************************
*									*
* Initialize the dlpi -> sk_buf device. 				*
*									*
************************************************************************/
int
ip_strm_init(struct ism_dev *dev)
{
#ifndef KERNEL_2_1
	int i;
#endif

	dev->mtu = IP_STRM_MTU;
	dev->hard_start_xmit = ip_strm_xmit;
	dev->do_ioctl = ip_strm_ioctl;
	dev->hard_header = 0;
	dev->hard_header_len = 0;
	dev->addr_len = 0;
	dev->tx_queue_len = 100;
	dev->type = ARPHRD_DLCI;	/* 0x0001 */
	dev->rebuild_header = 0;
	dev->open = ip_strm_open;
	dev->flags |= IFF_POINTOPOINT;
#ifdef KERNEL_2_1
	dev->addr_len = 4;
#else
	dev->family = AF_INET;
	dev->pa_addr = 0;
	dev->pa_brdaddr = 0;
	dev->pa_mask = 0;
	dev->pa_alen = 4;
#endif
	dev->irq = 0;
	memset(dev->dev_addr, 0, MAX_ADDR_LEN);
	dev->get_stats = get_stats;
	netif_start_queue(dev);

#if !defined(KERNEL_2_1)
	for (i = 0; i < DEV_NUMBUFFS; i++)
		skb_queue_head_init(&dev->buffs[i]);
#endif

	return (0);
}

#ifdef LINUX
modID_t modid = MOD_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for IP_STRMS.");
#endif				/* LINUX */

#if defined LFS
STATIC struct fmodsw ip_to_streams_fmod = {
	.f_name = MOD_NAME,
	.f_str = &ip_to_streams_info,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
ip_to_streams_register_module(void)
{
	int err;
	if ((err = register_strmod(&ip_to_streams_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};
STATIC void
ip_to_streams_unregister_module(void)
{
	return (void) unregister_strmod(&ip_to_streams_fmod);
}
#elif defined LIS
STATIC int
ip_to_streams_register_module(void)
{
	int ret;
	if ((ret = lis_register_strmod(&ip_to_streams_info, MOD_NAME)) != LIS_NULL_MID) {
		if (modid == 0)
			modid = ret;
		return (0);
	}
	/* LiS is not too good on giving informative errors here. */
	return (EIO);
}
STATIC void
ip_to_streams_unregister_module(void)
{
	/* LiS provides detailed error here when they are discarded. */
	return (void) lis_unregister_strmod(&ip_to_streams_info);
}
#endif

STATIC int __init
ip_to_streams_init(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = ip_to_streams_register_module())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, -err);
		return (err);
	}
	return (0);
};

STATIC void __exit
ip_to_streams_exit(void)
{
	ip_to_streams_unregister_module();
	return;
}

module_init(ip_to_streams_init);
module_exit(ip_to_streams_exit);
