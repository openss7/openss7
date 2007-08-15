/*****************************************************************************

 @(#) $RCSfile: ip_to_dlpi.c,v $ $Name:  $($Revision: 0.9.2.31 $) $Date: 2007/08/15 05:35:42 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

 Last Modified $Date: 2007/08/15 05:35:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ip_to_dlpi.c,v $
 Revision 0.9.2.31  2007/08/15 05:35:42  brian
 - GPLv3 updates

 Revision 0.9.2.30  2007/08/14 03:31:08  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: ip_to_dlpi.c,v $ $Name:  $($Revision: 0.9.2.31 $) $Date: 2007/08/15 05:35:42 $"

static char const ident[] =
    "$RCSfile: ip_to_dlpi.c,v $ $Name:  $($Revision: 0.9.2.31 $) $Date: 2007/08/15 05:35:42 $";

#include <sys/os7/compat.h>

#undef ptrace

#include <linux/bitops.h>

#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <net/arp.h>

#include <sys/dlpi.h>

#define IP2XINET_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define IP2XINET_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define IP2XINET_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation. All Rights Reserved."
#define IP2XINET_REVISION	"LfS $RCSfile: ip_to_dlpi.c,v $ $Name:  $ ($Revision: 0.9.2.31 $) $Date: 2007/08/15 05:35:42 $"
#define IP2XINET_DEVICE		"SVR 4.2 STREAMS INET DLPI Drivers (NET4)"
#define IP2XINET_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define IP2XINET_LICENSE	"GPL"
#define IP2XINET_BANNER		IP2XINET_DESCRIP	"\n" \
				IP2XINET_EXTRA		"\n" \
				IP2XINET_REVISION	"\n" \
				IP2XINET_COPYRIGHT	"\n" \
				IP2XINET_DEVICE		"\n" \
				IP2XINET_CONTACT
#define IP2XINET_SPLASH		IP2XINET_DEVICE		"\n" \
				IP2XINET_REVISION

#ifdef LINUX
MODULE_AUTHOR(IP2XINET_CONTACT);
MODULE_DESCRIPTION(IP2XINET_DESCRIP);
MODULE_SUPPORTED_DEVICE(IP2XINET_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(IP2XINET_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ip_to_dlpi");
#endif
#endif				/* LINUX */

#ifdef LFS
#define IP2XINET_DRV_ID		CONFIG_STREAMS_IP2XINET_MODID
#define IP2XINET_DRV_NAME	CONFIG_STREAMS_IP2XINET_NAME
#define IP2XINET_CMAJORS	CONFIG_STREAMS_IP2XINET_NMAJORS
#define IP2XINET_CMAJOR_0	CONFIG_STREAMS_IP2XINET_MAJOR
#define IP2XINET_UNITS		CONFIG_STREAMS_IP2XINET_NMINORS
#endif				/* LFS */

#define DRV_ID		IP2XINET_DRV_ID
#define DRV_NAME	IP2XINET_DRV_NAME
#define CMAJORS		IP2XINET_CMAJORS
#define CMAJOR_0	IP2XINET_CMAJOR_0
#define UNITS		IP2XINET_UNITS

#ifdef MODULE
#define DRV_BANNER	IP2XINET_BANNER
#else				/* MODULE */
#define DRV_BANNER	IP2XINET_SPLASH
#endif				/* MODULE */

#ifndef UNITS
#define UNITS 8
#endif

STATIC struct module_info ip2xinet_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = 8192,		/* Max packet size accepted */
	.mi_hiwat = 8192,		/* Hi water mark */
	.mi_lowat = 1024,		/* Lo water mark */
};

STATIC int streamscall ip2xinet_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int streamscall ip2xinet_close(queue_t *, int, cred_t *);

STATIC int streamscall ip2xinet_ursrv(queue_t *);
STATIC int streamscall ip2xinet_uwput(queue_t *, mblk_t *);

STATIC struct qinit ip2xinet_urinit = {
	.qi_srvp = ip2xinet_ursrv,	/* Read service */
	.qi_qopen = ip2xinet_open,	/* Each open */
	.qi_qclose = ip2xinet_close,	/* Last close */
	.qi_minfo = &ip2xinet_minfo,	/* Information */
};

STATIC struct qinit ip2xinet_uwinit = {
	.qi_putp = ip2xinet_uwput,	/* Read put (message from below) */
	.qi_minfo = &ip2xinet_minfo,	/* Information */
};

STATIC int streamscall ip2xinet_lrput(queue_t *, mblk_t *);
STATIC int streamscall ip2xinet_lwsrv(queue_t *);

STATIC struct qinit ip2xinet_lrinit = {
	.qi_putp = ip2xinet_lrput,	/* Read put (message from below) */
	.qi_minfo = &ip2xinet_minfo,	/* Information */
};

STATIC struct qinit ip2xinet_lwinit = {
	.qi_srvp = ip2xinet_lwsrv,	/* Read service */
	.qi_minfo = &ip2xinet_minfo,	/* Information */
};

STATIC struct streamtab ip2xinet_info = {
	.st_rdinit = &ip2xinet_urinit,	/* Upper read queue */
	.st_wrinit = &ip2xinet_uwinit,	/* Upper write queue */
	.st_muxrinit = &ip2xinet_lrinit,	/* Lower read queue */
	.st_muxwinit = &ip2xinet_lwinit,	/* Lower write queue */
};

/*************************************************************************
 *
 *  Copyright (C)  1998-2003 The Software Group Limited.
 *
 *************************************************************************/

/*
 * This driver provides a mapping between Linux IP and an underlying
 * DLPI streams driver.  The DLPI messages are the same as what
 * IP on a System V system would expect.
 *
 * TSG uses this driver to allow it's IP/X.25 software to work on
 * Linux.  Since that driver is called xinet, you can probably guess
 * how the name "ip2xinet" came about.
 *
 * It creates a device "/dev/ip2xinet".  Open this device and link it
 * to the driver below.
 *
 * This driver supports up to 8 network devices: ip2x0-7.
 * They can be configured using the ifconfig program.
 * 
 * A few things are required in order to make this work:
 * 1)  You must create a static arp entry in the table indicating
 *     the remote machines that we are directly connected to.  If
 *     you look at ip2xinet_hw_tx, you will see how this address ends up
 *     being used.
 *     eg.  arp -s 10.1.1.2 0A:01:01:02:00:00
 *
 * 2)  The driver underneath must have the routing information
 *     so that it can figure out how to send the IP datagram
 *     that it has received.  This step is specific to that driver.
 */

/************************************************************************
*                         Module Information                            *
************************************************************************/

/*
 * This structure is private to each device. It is used to pass
 * packets in and out, so there is place for a packet
 */

#define NUMIP2XINET 8

struct ip2xinet_priv {
	struct net_device_stats stats;
	int state;
	int packetlen;
	u8 *packetdata;
} ip2xinet_private[NUMIP2XINET];

struct ip2xinet_state {
	int ip2x_dlstate;		/* TSG: DLPI user state */
	int myminor;			/* store getminor result */
	queue_t *lowerq;		/* lower write queue corresponding to myminor */
	queue_t *readq;			/* upper read queue for my minor (just in case we care) */
	int index;			/* Unique ID of lower stream */

} ip2xinet_status;

extern struct net_device ip2xinet_devs[];

/* These are the flags in the statusword */
#define UNLINKED        0x20	/* our addition to DLPI states */

int ip2xinetdevflag = 0;

void ip2xinet_rx(struct net_device *dev, struct sk_buff *skb);
int ip2xinet_send_down_bind(queue_t *q);
int init_linuxip(void);
void cleanup_linuxip(void);
int ip2xinet_num_ip_opened;
spinlock_t *ip2xinet_lock;
int ip2_m_number;

int ip2xinetinit(void);

static int ip2xinet_numopen = 0;	/* How many times ip2xinet was opened as a STREAMS device */

char kernel_version[] = UTS_RELEASE;

int
ip2xinetinit(void)
{
	if (0 == init_linuxip())
		return 1;
	else
		return 0;
}

/************************************************************************
 *
 *  Function Name: ip2xinet_open
 *  Title: IP2XINET driver Open Routine
 *
 *  Description:
 *	Open a queue (device)
 *
 *  Arguments:
 *	q	- read queue pointer
 *	dev	- major/minor device number
 *	flag	- file open flags
 *	sflag	- STREAM open flags
 *
 *  Return Value:
 *	dev	- minor device
 *
 ************************************************************************/

int streamscall
ip2xinet_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
	mblk_t *bp;
	minor_t minor;
	struct stroptions *sop;

	/* already open */
	if (sflag != CLONEOPEN)
		return ENXIO;

	spin_lock(ip2xinet_lock);

	/* Can only open one time */
	if (ip2xinet_numopen) {
		spin_unlock(ip2xinet_lock);
		return ENXIO;
	} else
		ip2xinet_numopen = 1;

	if (q->q_count != 0)
		printk("ip2x level:q_count is %lu", (ulong) q->q_count);

	/* Set up the flow control parameters and send them up to the stream head.  */
	minor = getminor(*devp);
	if ((bp = allocb(sizeof(struct stroptions), BPRI_LO)) == NULL) {
		spin_unlock(ip2xinet_lock);
		printk("%s: allocb failed", __FUNCTION__);
		return ENOMEM;
	}

	ip2xinet_status.myminor = minor;
	ip2xinet_status.ip2x_dlstate = DL_UNATTACHED;

	/* These are dummy to keep track of the fact the device is open */
	q->q_ptr = (char *) &ip2xinet_numopen;
	WR(q)->q_ptr = (char *) &ip2xinet_numopen;

	spin_unlock(ip2xinet_lock);

	bp->b_datap->db_type = M_SETOPTS;
	bp->b_wptr += sizeof(struct stroptions);

	sop = (struct stroptions *) bp->b_rptr;
	sop->so_flags = SO_HIWAT | SO_LOWAT;
	sop->so_hiwat = ip2xinet_minfo.mi_hiwat;
	sop->so_lowat = ip2xinet_minfo.mi_lowat;
	putnext(q, bp);

	*devp = makedevice(getmajor(*devp), 0);
	return 0;
}

/************************************************************************
 *
 *  Function Name: ip2xinet_close
 *  Title: IP2XINET Driver Close Routine
 *
 *  Description:
 *	Close an ip2xinet queue (device)
 *
 *  Arguments:
 *	q	- read queue pointer
 *
 *  Return Value:
 *	none
 *
 ************************************************************************/

int streamscall
ip2xinet_close(queue_t *q, int oflag, cred_t *credp)
{
	(void) oflag;
	(void) credp;
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL || OTHER(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			DRV_NAME, __FUNCTION__);
		goto quit;
	}
#endif				/* defined LIS */
	qprocsoff(q);
	spin_lock(ip2xinet_lock);
	ip2xinet_numopen = 0;
	flushq(WR(q), FLUSHALL);
	q->q_ptr = NULL;
	WR(q)->q_ptr = NULL;
	spin_unlock(ip2xinet_lock);
	goto quit;
      quit:
	return (0);
}

/************************************************************************
 *
 *  Function Name: ip2xinet_uwput
 *  Title: IP2XINET Upper Write Put Routine
 *
 *  Description:
 *	Handle messages coming downstream. Should be no data in these
 *      just stupid stuff like flushq
 *
 *  Arguments:
 *	q	- upper write queue
 *	mp	- pointer to an message block
 *
 *  Return Value:
 *	none
 *
 ************************************************************************/

STATIC streamscall int
ip2xinet_uwput(queue_t *q, mblk_t *mp)
{

	int i;

	spin_lock(ip2xinet_lock);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
			qenable(q);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
			else
				flushq(RD(q), FLUSHDATA);
			if (!putq(RD(q), mp))
				freemsg(mp);
		} else
			freemsg(mp);
		break;

	case M_IOCTL:
		/* Process at least the I_LINK, I_UNLINK */

		/* THINKME: Failure to correctly process I_LINK/I_UNLINK while returning correctly
		   a nack to stream head will leave us in a possibly totally screwed up DLPI state
		   from which we have to somehow recover.  The possible problematic states are
		   DL_UNBOUND, any DL_PENDING states Note: if we stay in UNATTACHED on I_LINK
		   failure or in IDLE on I_UNLINK failure we're ok as long as the private data
		   structure stuff is consistent with the state */

	{
		struct iocblk *iocp;
		mblk_t *nmp;
		dl_attach_req_t *attach;
		struct linkblk *lp;

		iocp = (struct iocblk *) mp->b_rptr;

#if 0
#ifdef DEBUG
		pkt_debug(X25DBIOCTL) KPRINTF("%s size %d\n", x25dbiocmsg(iocp->ioc_cmd),
					      x25dbmsgsize(mp));
#endif
#endif

		switch ((unsigned) iocp->ioc_cmd) {
		case I_LINK:
			iocp->ioc_error = 0;
			iocp->ioc_rval = 0;
			iocp->ioc_count = 0;

			lp = (struct linkblk *) mp->b_cont->b_rptr;
			/* Use only one xinet queue for all devices */
			ip2xinet_status.lowerq = lp->l_qbot;
			ip2xinet_status.index = lp->l_index;

			/* Only one read q to get data from xinet */
			ip2xinet_status.readq = RD(q);

			/* These are dummy ones to indicate the queues are being used */
			ip2xinet_status.lowerq->q_ptr = (char *) &ip2xinet_numopen;
			RD(ip2xinet_status.lowerq)->q_ptr = (char *) &ip2xinet_numopen;

			if ((nmp = allocb(sizeof(union DL_primitives), BPRI_LO)) == NULL) {
				iocp->ioc_error = ENOSR;
				mp->b_datap->db_type = M_IOCNAK;
				if (!putq(RD(q), mp))
					freemsg(mp);
				spin_unlock(ip2xinet_lock);
				printk("pktioctl: I_LINK failed: allocb failed");
				return (0);
			}

			/* Setup and send an ATTACH */
			nmp->b_datap->db_type = M_PROTO;
			nmp->b_wptr += DL_ATTACH_REQ_SIZE;

			attach = (dl_attach_req_t *) nmp->b_rptr;
			attach->dl_primitive = DL_ATTACH_REQ;
			attach->dl_ppa = ip2xinet_status.myminor;
			ip2xinet_status.ip2x_dlstate = DL_ATTACH_PENDING;

			/* experience shows that an I_LINKed queue needs to be enabled so that the
			   service routine will be run. */
			qenable(ip2xinet_status.lowerq);
			if (!putq(ip2xinet_status.lowerq, nmp))
				freemsg(nmp);

			/* all went well */
			mp->b_datap->db_type = M_IOCACK;
			if (!putq(RD(q), mp))
				freemsg(mp);	/* fall back on timout */
			break;

		case I_UNLINK:
		{
			struct linkblk *lp;

			iocp->ioc_error = 0;
			iocp->ioc_rval = 0;
			iocp->ioc_count = 0;
			lp = (struct linkblk *) mp->b_cont->b_rptr;

			/* Ignore the DLPI state, the stack is being torn down regardless.  */
			ip2xinet_status.ip2x_dlstate = UNLINKED;
			/* can't transmit any more */
			for (i = 0; i < NUMIP2XINET; i++) {
				struct ip2xinet_priv *privptr = &ip2xinet_private[i];

				if (privptr->state == 1)
					netif_stop_queue(&(ip2xinet_devs[i]));
			}

			flushq(q, FLUSHALL);
			flushq(RD(lp->l_qbot), FLUSHALL);

			ip2xinet_status.readq = NULL;
			ip2xinet_status.lowerq = NULL;
			mp->b_datap->db_type = M_IOCACK;
			if (!putq(RD(q), mp))
				freemsg(mp);	/* fall back on timout */

			break;
		}

		default:
			iocp->ioc_error = EINVAL;
			mp->b_datap->db_type = M_IOCNAK;
			if (!putq(RD(q), mp))
				freemsg(mp);	/* fall back on timout */
			break;
		}

	}
		break;

	case M_DATA:
	case M_PCPROTO:
	case M_PROTO:
	default:
		printk("ip2xinet_uwput: unexpected type=0x%x", mp->b_datap->db_type);
		freemsg(mp);
		break;
	}
	spin_unlock(ip2xinet_lock);
	return (0);
}

/************************************************************************
 *
 * Function Name: ip2xinet_ursrv
 * Title: IP2XINET Upper Read Service routine
 *
 * Description:
 *      Send all of the messages on this queue up to the next driver.
 *      We don't worry about doing flow control.  This is used because in
 *      the places where we might want to call putnext, we call putq
 *      instead.
 *
 ************************************************************************/
STATIC streamscall int
ip2xinet_ursrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		putnext(q, mp);
	}
	return (0);
}

/************************************************************************
 *
 * Function Name: ip2xinet_lwsrv
 * Title: IP2XINET Lower Write Service routine
 *
 * Description:
 *      Send all of the messages on this queue down to the next driver.
 *      If we discover that we can't do a put, then stop the Linux
 *      devices from sending us stuff.
 *
 ************************************************************************/
STATIC streamscall int
ip2xinet_lwsrv(queue_t *q)
{
	mblk_t *mp;
	int allsent = 1;
	int i;
	struct net_device *dev = ip2xinet_devs;
	struct ip2xinet_priv *privp;

	while ((mp = getq(q))) {
		/* M_PROTO's should be last on the list.  If it is something else, then it should
		   be ahead, and we can just go ahead and put it down. */
		if (mp->b_datap->db_type == M_PROTO) {
			if (canputnext(q)) {
				putnext(q, mp);
			} else {
				noenable(q);
				if (!putbq(q, mp))
					freemsg(mp);	/* FIXME */
				enableok(q);
				allsent = 0;
				break;
			}
		} else {
			putnext(q, mp);
		}
	}

	/* Handle the flow control.  If we were able to send everything then it is ok for the
	   kernel to send us more stuff.  Otherwise it is not ok.  Go through all of the devices
	   and set the appropriate state. */
	spin_lock(ip2xinet_lock);
	for (i = 0; i < NUMIP2XINET; i++, dev++) {
		privp = (struct ip2xinet_priv *) dev->priv;
		if (privp->state == 1 && ip2xinet_status.ip2x_dlstate == DL_IDLE) {
			if (allsent) {
				netif_start_queue(dev);	/* kernel can transmit */
			} else {
				netif_stop_queue(dev);	/* We are flow controlled. */
			}
		}
	}
	spin_unlock(ip2xinet_lock);

	return (0);
}

/************************************************************************
 *
 *  Function Name: ip2xinet_lrput
 *  Title: IP2XINET Lower Read Put Routine
 *
 *  Description:
 *	Handle messages coming upstream. Valid messages will have
 *	a DLPI header associated with them. 
 *      Since we can't be flow controlled by Linux TCP/IP, don't need lrsrv
 *
 *  Arguments:
 *	q	- upper read queue 
 *	mp	- pointer to an message block
 *
 *  Return Value:
 *	none
 *
 ************************************************************************/

STATIC streamscall int
ip2xinet_lrput(queue_t *q, mblk_t *mp)
{
	struct iocblk *iocp;
	union DL_primitives *dp;
	struct ip2xinet_priv *privptr;
	struct net_device *dev;
	int i;

	spin_lock(ip2xinet_lock);

	/* use the first open ip device */
	for (i = 0; i < NUMIP2XINET; i++) {
		privptr = &ip2xinet_private[i];

		if (privptr->state == 1)
			break;
	}
	if (i == NUMIP2XINET)
		i = 0;		/* All devices closed, pick the 1st one */
	/* send data up to ip through the 1st open device */
	dev = &ip2xinet_devs[i];

	switch (mp->b_datap->db_type) {
	case M_CTL:
		freemsg(mp);
		break;

	case M_DATA:
		/* NOTE: We don't expect any M_DATA messages from xinet */
		freemsg(mp);
		break;

	case M_PROTO:
	case M_PCPROTO:
		dp = (union DL_primitives *) mp->b_rptr;

#if 0
#ifdef DEBUG
		printk("ip2xinet_lrput: %s size=%d\n", x25dbdlpmsg(dp->dl_primitive),
		       x25dbmsgsize(mp));
#endif
#endif

		switch (dp->dl_primitive) {
		case DL_BIND_ACK:

			/* if we're in in BNDPND and receive a BIND_ACK we go to IDLE */
			ip2xinet_status.ip2x_dlstate = DL_IDLE;

			/* If we're DL_IDLE, then dev is open and the kernel can transmit */
			for (i = 0; i < NUMIP2XINET; i++) {
				privptr = &ip2xinet_private[i];

				if (privptr->state == 1)
					netif_start_queue(&(ip2xinet_devs[i]));
			}
			freemsg(mp);	/* Frees bind_ack no longer needed */
			break;

		case DL_INFO_ACK:

			/* NOTE: currently we don't send info_req to xinet */

			freemsg(mp);
			break;

		case DL_ERROR_ACK:
			switch (ip2xinet_status.ip2x_dlstate) {
			case DL_ATTACH_PENDING:
				/* if we receive ERROR_ACK and we're in ATTACH_PEND go into
				   UNATTACHED */
				ip2xinet_status.ip2x_dlstate = DL_UNATTACHED;
				freemsg(mp);
				break;

			case DL_BIND_PENDING:
				/* if we're in BNDPND and receive an ERR ack we go to UNBND, */
				ip2xinet_status.ip2x_dlstate = DL_UNBOUND;
				freemsg(mp);
				break;

			case DL_UNBIND_PENDING:
				/* If we're in UNBIND_PEND and we receive ERROR_ACK we go into IDLE 
				 */
				ip2xinet_status.ip2x_dlstate = DL_IDLE;
				freemsg(mp);
				break;

			case DL_DETACH_PENDING:
				/* If we're in DETACH_PEND and receive and ERROR_ACK we go into
				   UNBND */
				ip2xinet_status.ip2x_dlstate = DL_UNBOUND;
				freemsg(mp);
				break;
			default:
				freemsg(mp);
				break;

			}
			break;

		case DL_UNITDATA_IND:
			/* if we're in IDLE we can get DL_UNITDATA_IND with data and call the guy
			   who would normally receive data from interrupt handler. */

			/* Check state: can't transmit if dev is closed :-) Note: we have to check
			   both the dlpi state and dev->start because during a close the DLPI state 
			   could remain DL_IDLE if we couldn't allocate mblk for UNBIND_REQ. There
			   are many ways in which the dev->start could be 1 but dlpi state - not
			   DL_IDLE. */
			if (ip2xinet_status.ip2x_dlstate == DL_IDLE && privptr->state == 1) ;
			{
				mblk_t *newmp;
				char *buf;
				int len, tmplen;
				struct ethhdr *eth;
				struct sk_buff *skb;

				newmp = unlinkb(mp);

				freemsg(mp);
				mp = newmp;

				/* 1st pass through.  figure out the len */
				for (len = sizeof(struct ethhdr); newmp != NULL;
				     newmp = newmp->b_cont)
					len += (newmp->b_wptr - newmp->b_rptr);

				/* ALLOCATE skb of length len+2, COPY from mp chain to skb */

				skb = dev_alloc_skb(len + 2);
				if (!skb) {
					printk("ip2xinet rx: failed to allocate an skb\n");
					freemsg(mp);
					break;
				}
				skb_reserve(skb, 2);	/* align IP on 16B boundary */
				/* The packet has been retrieved from the transmission medium.
				   Build an skb around it, so upper layers can handle it */
				buf = skb_put(skb, len);
				for (newmp = mp, tmplen = sizeof(struct ethhdr); newmp != NULL;
				     newmp = newmp->b_cont) {
					bcopy(newmp->b_rptr, buf + tmplen,
					      newmp->b_wptr - newmp->b_rptr);
					tmplen += (newmp->b_wptr - newmp->b_rptr);
				}
				eth = (struct ethhdr *) buf;

				/* I am not sure it's necessary, but just in case... */

				memcpy(eth->h_source, dev->dev_addr, dev->addr_len);
				memcpy(eth->h_dest, dev->dev_addr, dev->addr_len);
				eth->h_proto = 0x8;	/* ETH_P_IP in network order */
				eth->h_source[ETH_ALEN - 1] ^= 0x01;	/* say src is us xor 1 */

				/* send it to ip2xinet_rx for handling */
				ip2xinet_rx(dev, skb);
			}
			freemsg(mp);
			break;
		case DL_UDERROR_IND:
			freemsg(mp);
			break;

		case DL_OK_ACK:
			switch (dp->ok_ack.dl_correct_primitive) {

			case DL_ATTACH_REQ:
				/* if we're in ATTACH_PEND and we received OK_ACK1 change state to
				   UNBND */
				ip2xinet_status.ip2x_dlstate = DL_UNBOUND;
				freemsg(mp);
				/* We just completed building up the X.25 stack below us. If IP is
				   already above us, we need to send down the bind that we would
				   normally do when IP opens us.  This allows us to restart the
				   X.25 stack without restarting TCP/IP. */
				if (ip2xinet_num_ip_opened != 0)
					ip2xinet_send_down_bind(WR(q));
				break;

			case DL_UNBIND_REQ:
				/* If we're in UNBIND_PEND and receive OK_ACK1 we go to UNBND. */
				ip2xinet_status.ip2x_dlstate = DL_UNBOUND;
				freemsg(mp);
				break;

			case DL_DETACH_REQ:
				/* If we're in DETACH_PEND and receive OK_ACK1 we go to UNATT */
				ip2xinet_status.ip2x_dlstate = DL_UNATTACHED;
				freemsg(mp);
				break;

			default:
				freemsg(mp);
				break;
			}
			break;

		default:
			printk("ip2xinet_lrput: bad prim=0x%lx", (ulong) dp->dl_primitive);
			freemsg(mp);
			break;
		}
		break;

	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
			qenable(q);
		}
		if (mp->b_rptr[0] & FLUSHW) {
			mp->b_rptr[0] &= ~FLUSHR;
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
			else
				flushq(WR(q), FLUSHDATA);
			qenable(WR(q));
			if (!putq(WR(q), mp))
				freemsg(mp);
		} else
			freemsg(mp);
		break;

	case M_HANGUP:
		/* send it to the guy that linked us up, what he does is his problem. */
		if (!putq(ip2xinet_status.readq, mp))
			freemsg(mp);
		break;

	case M_IOCACK:
		iocp = (struct iocblk *) mp->b_rptr;
		if (iocp->ioc_cmd == SIOCSIFMTU) {
			/* The set MTU ioctl was a success Rejoice :-) */
			freemsg(mp);
		} else if (!putq(ip2xinet_status.readq, mp))
			freemsg(mp);
		break;

	case M_IOCNAK:
		iocp = (struct iocblk *) mp->b_rptr;
		if (iocp->ioc_cmd == SIOCSIFMTU) {
			/* The set MTU ioctl was a failure From looking at xinet code this is *
			   impossible, so ignore it */

			freemsg(mp);
		} else if (!putq(ip2xinet_status.readq, mp))
			freemsg(mp);
		break;

	default:
		printk("ip2xinet_lrput: bad type=%d", mp->b_datap->db_type);
		freemsg(mp);
		break;
	}

	spin_unlock(ip2xinet_lock);
	return (0);
}

/*
 * The first part of the driver was all streams.
 * The second half is what talks to Linux.
 * They used to be in separate files, but for easier compilation in
 * the streams directory, I have joined them.
 */

#define IP_SAP          0x800	/* SAP for IP protocol - currently enet type */
#define ARP_SAP         0x806	/* SAP for ARP */

/* This is a load-time options */
static int eth = 0;			/* Call yourself "ethX". Default is "ip2x0"/"ip2x1" */

int ip2xinet_eth;
int ip2xinet_num_ip_opened = 0;

void ip2xinet_rx(struct net_device *dev, struct sk_buff *skb);

void
do_BUG(const char *file, int line)
{
	return;
}

int
ip2xinet_send_down_bind(queue_t *q)
{
	mblk_t *mp;
	dl_bind_req_t *bindmp;

	if ((mp = allocb(sizeof(union DL_primitives), BPRI_LO)) == NULL) {
		printk("ip2xopen: failed: allocb failed");
		return -EAGAIN;	/* Other drivers seem to return this on error */
	}
	ip2xinet_status.ip2x_dlstate = DL_BIND_PENDING;

	mp->b_datap->db_type = M_PROTO;
	mp->b_wptr += DL_BIND_REQ_SIZE;
	bindmp = (dl_bind_req_t *) mp->b_rptr;
	bindmp->dl_primitive = DL_BIND_REQ;
	bindmp->dl_sap = IP_SAP;
	if (!putq(q, mp))
		freemsg(mp);
	return 0;
}

/*
 * Open and close
 */

int
ip2xinet_devopen(struct net_device *dev)
{
	int i;
	int err;
	struct ip2xinet_priv *privp = (struct ip2xinet_priv *) dev->priv;

	spin_lock(ip2xinet_lock);

	/* BEFORE ANYTHING CHECK THAT the streams I_LINK SUCCEEDED */
	if (!
	    (ip2xinet_status.ip2x_dlstate == DL_UNBOUND
	     || (ip2xinet_num_ip_opened != 0 && ip2xinet_status.ip2x_dlstate == DL_IDLE))) {
		/* Normally we'd do the I_LINK, this would set us up into the UNBOUND state but
		   something went wrong.  Either the I_LINK has not completed yet, or it failed. In 
		   any case we're not in the shape to succeed so return a failure code and exit. */
		spin_unlock(ip2xinet_lock);
		return -EAGAIN;	/* Other drivers seem to return this on error */
	}
	/* Send a DL_BIND DOWN */
	if (ip2xinet_num_ip_opened == 0) {
		if ((err = ip2xinet_send_down_bind(ip2xinet_status.lowerq)) != 0) {
			spin_unlock(ip2xinet_lock);
			return err;
		}
	}
	ip2xinet_num_ip_opened++;

	/* Assign the hardware address of the board: use "\0IP2Xx", where x is 0 to 7. The first
	   byte is '\0': a safe choice with regard to multicast */
	for (i = 0; i < ETH_ALEN; i++)
		dev->dev_addr[i] = "\0IP2X0"[i];
	dev->dev_addr[ETH_ALEN - 1] += (dev - ip2xinet_devs);	/* the number */

	privp->state = 1;
	if (ip2xinet_status.ip2x_dlstate == DL_IDLE)
		netif_start_queue(dev);	/* kernel can transmit */
	else
		netif_stop_queue(dev);	/* wait until DL_IDLE, then kernel can tx */

	spin_unlock(ip2xinet_lock);
	return 0;
}

int
ip2xinet_release(struct net_device *dev)
{
	queue_t *q;
	mblk_t *mp;
	struct ip2xinet_priv *privp = (struct ip2xinet_priv *) dev->priv;

	spin_lock(ip2xinet_lock);
	privp->state = 0;
	netif_stop_queue(dev);	/* can't transmit any more */
	ip2xinet_num_ip_opened--;
	/* BEFORE ANYTHING CHECK THAT we're in IDLE */
	if (ip2xinet_status.ip2x_dlstate != DL_IDLE) {

		/* Normally we'd do the I_UNBIND, from DL_IDLE In all other cases we ignore the
		   dlpi state as we'll unlink soon */
		spin_unlock(ip2xinet_lock);
		return 0;
	}
	/* Send a DL_UNBIND DOWN */
	if (ip2xinet_num_ip_opened == 0) {
		q = ip2xinet_status.lowerq;
		if ((mp = allocb(sizeof(union DL_primitives), BPRI_LO)) == NULL) {
			printk("ip2xopen: failed: allocb failed");
			spin_unlock(ip2xinet_lock);
			return 0;	/* Other drivers seem to return this on error */
		}
		ip2xinet_status.ip2x_dlstate = DL_UNBIND_PENDING;
		if (mp) {
			dl_unbind_req_t *unbindmp;

			mp->b_datap->db_type = M_PROTO;
			mp->b_wptr += DL_UNBIND_REQ_SIZE;
			unbindmp = (dl_unbind_req_t *) mp->b_rptr;
			unbindmp->dl_primitive = DL_UNBIND_REQ;
			if (!putq(q, mp))
				freemsg(mp);
		}
	}
	spin_unlock(ip2xinet_lock);
	return 0;
}

/*
 * Configuration changes (passed on by ifconfig)
 * Not that we actually do anything with them.
 */
int
ip2xinet_config(struct net_device *dev, struct ifmap *map)
{
	spin_lock(ip2xinet_lock);
	if (dev->flags & IFF_UP) {	/* can't act on a running interface */
		spin_unlock(ip2xinet_lock);
		return -EBUSY;
	}

	/* Don't allow changing the I/O address */
	if (map->base_addr != dev->base_addr) {
		spin_unlock(ip2xinet_lock);
		printk(KERN_WARNING "ip2xinet: Can't change I/O address\n");
		return -EOPNOTSUPP;
	}

	/* Don't allow changing the IRQ */
	if (map->irq != dev->irq) {
		spin_unlock(ip2xinet_lock);
		printk(KERN_WARNING "ip2xinet: Can't change IRQ\n");
		return -EOPNOTSUPP;
	}

	/* ignore other fields */
	spin_unlock(ip2xinet_lock);
	return 0;
}

/*
 * Receive a packet: retrieve, encapsulate and pass over to upper levels
 *
 * This routine is called from the lrput routine.  We should already hold
 * the driver lock when we are called from there.
 */
void
ip2xinet_rx(struct net_device *dev, struct sk_buff *skb)
{
	struct ip2xinet_priv *privp = (struct ip2xinet_priv *) dev->priv;

	/* The packet has been retrieved from the transmission medium. Build an skb around it, so
	   upper layers can handle it */

	/* Write metadata, and then pass to the receive level */
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	skb->ip_summed = CHECKSUM_UNNECESSARY;	/* don't check it */
	privp->stats.rx_packets++;
	netif_rx(skb);
	return;
}

/*
 * Transmit a packet (low level interface)
 *
 * This routine is called from ip2xinet_tx. That function
 * grabbed the driver lock when it was called.
 */
void
ip2xinet_hw_tx(char *buf, int len, struct net_device *dev)
{
	/* This function deals with hw details, while all other procedures are rather
	   device-independent */
	struct iphdr *ih, *iph;
	struct ethhdr *eth;
	struct ip2xinet_priv *privp;
	queue_t *q;
	mblk_t *mp, *nmp;
	dl_unitdata_req_t *req;
	int mylen;

	/* sanity check */
	if (len < sizeof(struct ethhdr) + sizeof(struct iphdr)) {
		printk("ip2xinet: Hmm... packet too short (%i octets)\n", len);
		return;
	}
	/* Ethhdr is 14 bytes, but the kernel arranges for iphdr to be aligned (i.e., ethhdr is
	   unaligned) */
	ih = (struct iphdr *) (buf + sizeof(struct ethhdr));

	/* Ok, now the packet is ready for transmission: */

	/* Here we do a putq to the bottom q.  */

	privp = &ip2xinet_private[dev - ip2xinet_devs];
	q = ip2xinet_status.lowerq;

	/* THIS IS WHERE WE ALLOCATE UNITDATA_REQ and send data down */
	if ((mp = allocb(sizeof(struct iphdr) + DL_UNITDATA_REQ_SIZE, BPRI_LO)) == NULL) {
		printk("ip2xhwtx: failed: allocb failed");
		return;
	}
	mp->b_datap->db_type = M_PROTO;
	mp->b_wptr += (sizeof(struct iphdr) + DL_UNITDATA_REQ_SIZE);

	/* xinet expects a DLPI header ahead of the datagram, as in Unix. The destination address
	   in this header needs to be the next hop address.  We're going to get this from the
	   destination address in the Ethernet header and rely upon froute/x25route having added a
	   static ARP entry with:

	   IP address of machine at other end of circuit

	   MAC address equal to IP address of that same machine

	   Though the IP address of IP datagrams passed down to us may be many hops away, the
	   destination Ethernet address will always be the next hop IP address. */
	eth = (struct ethhdr *) (buf);
	iph = (struct iphdr *) (mp->b_rptr + DL_UNITDATA_REQ_SIZE);
	iph->saddr = ih->saddr;	/* likely unused by xinet */
	iph->daddr = (eth->h_dest[3] << 24)	/* next hop address */
	    +(eth->h_dest[2] << 16)
	    + (eth->h_dest[1] << 8)
	    + (eth->h_dest[0]);
	iph->check = 0;

	req = (dl_unitdata_req_t *) mp->b_rptr;
	req->dl_primitive = DL_UNITDATA_REQ;
	req->dl_dest_addr_length = 4;
	req->dl_dest_addr_offset = DL_UNITDATA_REQ_SIZE + (int) &((struct iphdr *) 0)->daddr;

	/* Copy from buf to mp, make everything right, then send the stuff to xinet IF WE CAN.
	   Could we use esballoc here? */

	mylen = len - sizeof(struct ethhdr);
	if ((nmp = allocb(mylen, BPRI_LO)) == NULL) {
		printk("ip2xhwtx: failed: allocb failed");
		freemsg(mp);
		return;
	}
	linkb(mp, nmp);
	bcopy(buf + sizeof(struct ethhdr), nmp->b_rptr, mylen);
	nmp->b_wptr += mylen;
	if (!putq(q, mp))
		freemsg(mp);
	privp->stats.tx_packets++;
}

/*
 * Transmit a packet (called by the kernel)
 */
int
ip2xinet_tx(struct sk_buff *skb, struct net_device *dev)
{
	if (skb == NULL) {
		return 0;
	}
	if (skb->len < (sizeof(struct ethhdr) + sizeof(struct iphdr))) {
		dev_kfree_skb(skb);
		return 0;
	}
	spin_lock(ip2xinet_lock);
	if (netif_queue_stopped(dev)) {
		spin_unlock(ip2xinet_lock);
		return -EBUSY;
	}

	dev->trans_start = jiffies;	/* save the timestamp */
	ip2xinet_hw_tx(skb->data, skb->len, dev);
	spin_unlock(ip2xinet_lock);
	dev_kfree_skb(skb);	/* release it */

	return 0;		/* zero == done */
}

/*
 * Ioctl commands 
 */
int
ip2xinet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	return 0;
}

/*
 * Return statistics to the caller
 */
struct net_device_stats *
ip2xinet_stats(struct net_device *dev)
{
	struct ip2xinet_priv *priv = (struct ip2xinet_priv *) dev->priv;

	/* The only stats we keep are transmitted and received packets. Note: xinet stats are kind
	   of useless since the structure ifstats doesn't exist under linux.  So rather than keep
	   track of stats in xinet we do it in ip2xinet. */
	return &priv->stats;
}

int
ip2xinet_rebuild_header(struct sk_buff *skb)
{
	struct ethhdr *eth = (struct ethhdr *) (skb->data);

	// return arp_find(&(eth->h_dest),skb);
	return arp_find(eth->h_dest, skb);
}

/*
 * This function builds the hardware header from the source and destination
 * hardware addresses that were previousely retrieved, its job is to organise
 * the information passed to it as arguments
 */
int
ip2xinet_hard_header(struct sk_buff *skb, struct net_device *dev, unsigned short type,
		     void *daddr, void *saddr, unsigned len)
{
	struct ethhdr *eth = (struct ethhdr *) skb_push(skb, ETH_HLEN);

	/* Set the protocol type. For a packet of type ETH_P_802_3 we put the length in here
	   instead. It is up to the 802.2 layer to carry protocol information. */

	if (type != ETH_P_802_3)
		eth->h_proto = htons(type);
	else
		eth->h_proto = htons(len);

	/* Set the source hardware address. */

	if (saddr)
		memcpy(eth->h_source, saddr, dev->addr_len);
	else
		memcpy(eth->h_source, dev->dev_addr, dev->addr_len);

	/* Anyway, the loopback-device should never use this function... */

	if (dev->flags & IFF_LOOPBACK) {
		memset(eth->h_dest, 0, dev->addr_len);
		return (dev->hard_header_len);
	}

	if (daddr) {
		memcpy(eth->h_dest, daddr, dev->addr_len);
		return dev->hard_header_len;
	}

	return -dev->hard_header_len;
}

/*
 * The "change_mtu" method is usually not needed.
 * If you need it, it must be like this.
 *
 * The change in MTU (Maximum Transfer Unit)
 * must be communicated to xinet
 */
int
ip2xinet_change_mtu(struct net_device *dev, int new_mtu)
{
	/* check ranges */

	if ((new_mtu < 68) || (new_mtu > 1500))
		return -EINVAL;

	/* Do anything you need, and accept the value */

	spin_lock(ip2xinet_lock);
	dev->mtu = new_mtu;	/* accept the new value */
	spin_unlock(ip2xinet_lock);
	return 0;
}

/*
 * The init function (sometimes called probe).
 * It is invoked by register_netdev()
 *
 * NOTE: This is different from the init() function that can be called from
 *       streams
 */
int
ip2xinet_devinit(struct net_device *dev)
{
	/* Assign other fields in dev, using ether_setup() and some hand assignments */
	ether_setup(dev);
	dev->open = ip2xinet_devopen;
	dev->stop = ip2xinet_release;
	dev->set_config = ip2xinet_config;
	dev->hard_start_xmit = ip2xinet_tx;
	dev->do_ioctl = ip2xinet_ioctl;
	dev->get_stats = ip2xinet_stats;
	dev->change_mtu = ip2xinet_change_mtu;
	dev->rebuild_header = ip2xinet_rebuild_header;
	dev->hard_header = ip2xinet_hard_header;
	dev->hard_header_len = ETH_HLEN;
	dev->flags |= IFF_NOARP;
	dev->type = ARPHRD_ETHER;
	dev->addr_len = 6;	/* fake ethernet address */
	dev->tx_queue_len = 10;

	/* dev->dev_addr is handled in the open() */

	dev->flags &= ~IFF_BROADCAST;	/* X25 doesn't broadcast */
	dev->flags &= ~IFF_MULTICAST;	/* X25 doesn't multicast */

	/* Then, allocate the priv field. This encloses the statistics and a few private fields.  */
	dev->priv = &ip2xinet_private[dev - ip2xinet_devs];
	if (dev->priv == NULL)
		return -ENOMEM;
	memset(dev->priv, 0, sizeof(struct ip2xinet_priv));
#ifdef HAVE_KFUNC_DEV_INIT_BUFFERS
	dev_init_buffers(dev);
#endif
	ip2xinet_status.ip2x_dlstate = UNLINKED;
	return 0;
}

struct net_device ip2xinet_devs[NUMIP2XINET];

/*
 * Finally, the module stuff
 */

int
init_linuxip(void)
{

	int result, i, device_present = 0;
	struct net_device *dev = ip2xinet_devs;

	ip2xinet_eth = eth;	/* copy the cfg datum in the non-static place */

	/* call them "ip2x0"... "ip2x7" */
	for (i = 0; i < NUMIP2XINET; i++, dev++) {
		memset(dev, 0, sizeof(struct net_device));
		memcpy(dev->name, "ip2x0", 6);
		dev->name[4] = (char) ('0' + i);
		dev->init = ip2xinet_devinit;
		/* the rest of the fields are filled in by ip2xinet_devinit */
	}

	dev = ip2xinet_devs;
	for (i = 0; i < NUMIP2XINET; i++, dev++)
		if ((result = register_netdev(dev)))
			printk("ip2xinet: error %i registering device \"%s\"\n", result, dev->name);
		else
			device_present++;

	return device_present ? 0 : -ENODEV;
}

void
cleanup_linuxip(void)
{
	int i;

	for (i = 0; i < NUMIP2XINET; i++) {
		unregister_netdev(&ip2xinet_devs[i]);
	}
}

#ifdef LINUX
unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID number for IP2XINET driver (0 for allocation).");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for IP2XINET driver (0 for allocation).");
#endif				/* LINUX */

#ifdef LFS
STATIC struct cdevsw ip2xinet_cdev = {
	.d_name = DRV_NAME,
	.d_str = &ip2xinet_info,
	.d_flag = D_MTPERQ,		/* consistent with LiS */
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};
int __init
ip2xinet_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_IP2XINET_MODULE
	cmn_err(CE_NOTE, IP2XINET_BANNER);
#else
	cmn_err(CE_NOTE, IP2XINET_SPLASH);
#endif
	if ((err = register_strdev(&ip2xinet_cdev, major)) < 0)
		return (err);
	if (err > 0)
		major = err;
	return (0);
}

void __exit
ip2xinet_exit(void)
{
	return (void) unregister_strdev(&ip2xinet_cdev, major);
}

#ifdef CONFIG_STREAMS_IP2XINET_MODULE
module_init(ip2xinet_init);
module_exit(ip2xinet_exit);
#endif

#else
#ifdef LIS

STATIC int ip2xinet_initialized = 0;
STATIC void
ip2xinet_init(void)
{
	int err;

	if (ip2xinet_initialized != 0)
		return;
	cmn_err(CE_NOTE, IP2XINET_BANNER);	/* console splash */
	if (!(ip2xinet_lock = kmem_alloc(sizeof(spinlock_t), KM_NOSLEEP))) {
		ip2xinet_initialized = -ENOMEM;
		return;
	}
	spin_lock_init(ip2xinet_lock);
	if (!(err = ip2xinetinit())) {
		kmem_free((void *) ip2xinet_lock, sizeof(*ip2xinet_lock));
		ip2xinet_lock = NULL;
		ip2xinet_initialized = err;
		return;
	}
	if ((err = lis_register_strdev(major, &ip2xinet_info, UNITS, DRV_NAME)) < 0) {
		cmn_err(CE_WARN, "%s: Cannot register major %d\n", DRV_NAME, major);
		kmem_free((void *) ip2xinet_lock, sizeof(*ip2xinet_lock));
		ip2xinet_lock = NULL;
		ip2xinet_initialized = err;
		return;
	}
	ip2xinet_initialized = 1;
	if (major == 0 && err > 0) {
		int clonemajor = lis_clone_major();

		major = err;
		/* Remove the old /dev/ip2xinet node.  We are about to create a new one, and that
		   call may fail if the old one is still there. We don't actually care if the
		   unlink call fails, just as long as the node isn't there. */
		lis_unlink("/dev/ip2xinet");
		if ((err =
		     lis_mknod("/dev/ip2xinet", 0666 | S_IFCHR, MKDEV(clonemajor, major))) < 0) {
			kmem_free((void *) ip2xinet_lock, sizeof(*ip2xinet_lock));
			ip2xinet_lock = NULL;
			lis_unregister_strdev(major);
			ip2xinet_initialized = err;
			return;
		}
		ip2xinet_initialized = 2;
	}
	return;
}
STATIC void
ip2xinet_terminate(void)
{
	int err;

	if (ip2xinet_initialized <= 0)
		return;
	if (major) {
		if ((err = lis_unregister_strdev(major)) < 0)
			cmn_err(CE_PANIC, "%s: Cannot unregister major %d\n", DRV_NAME, major);
		major = 0;
	}
	cleanup_linuxip();
	if (ip2xinet_initialized == 2)
		lis_unlink("/dev/ip2xinet");
	kmem_free((void *) ip2xinet_lock, sizeof(*ip2xinet_lock));
	ip2xinet_lock = NULL;
	ip2xinet_initialized = 0;
	return;
}

int
init_module(void)
{
	(void) major;
	ip2xinet_init();
	if (ip2xinet_initialized < 0)
		return ip2xinet_initialized;
	return (0);
}

void
cleanup_module(void)
{
	return ip2xinet_terminate();
}

#endif
#endif
