/*****************************************************************************

 @(#) $RCSfile: ldl.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/03/07 08:58:54 $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/03/07 08:58:54 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: ldl.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/03/07 08:58:54 $"

static char const ident[] =
    "$RCSfile: ldl.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/03/07 08:58:54 $";

#define _SVR4_SOURCE
#define _LIS_SOURCE

#include "compat.h"

#include <linux/bitops.h>

#include <linux/netdevice.h>	/* Linux netdevice header */
#include <linux/if_arp.h>	/* Linux netdevice types */
#include <linux/if_ether.h>	/* Linux ethernet defs */
#include <linux/skbuff.h>	/* Linux socket buffer */
#include <net/pkt_sched.h>	/* Linux queue disciplines */

#include <sys/dlpi.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
#define	KERNEL_2_0
#else
#define	KERNEL_2_1
# if LINUX_VERSION_CODE > KERNEL_VERSION(2,3,0)
# define KERNEL_2_3
# endif
#endif

#include <sys/ldl.h>

#define LDL_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LDL_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define LDL_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation. All Rights Reserved."
#define LDL_REVISION	"LfS $RCSfile: ldl.c,v $ $Name:  $ ($Revision: 0.9.2.11 $) $Date: 2005/03/07 08:58:54 $"
#define LDL_DEVICE	"SVR 4.2 STREAMS INET DLPI Drivers (NET4)"
#define LDL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LDL_LICENSE	"GPL"
#define LDL_BANNER	LDL_DESCRIP	"\n" \
			LDL_EXTRA	"\n" \
			LDL_REVISION	"\n" \
			LDL_COPYRIGHT	"\n" \
			LDL_DEVICE	"\n" \
			LDL_CONTACT
#define LDL_SPLASH	LDL_DEVICE	"\n" \
			LDL_REVISION

#ifdef LINUX
MODULE_AUTHOR(LDL_CONTACT);
MODULE_DESCRIPTION(LDL_DESCRIP);
MODULE_SUPPORTED_DEVICE(LDL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LDL_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* LINUX */

#ifdef LFS
#define LDL_DRV_ID	CONFIG_STREAMS_LDL_MODID
#define LDL_DRV_NAME	CONFIG_STREAMS_LDL_NAME
#define LDL_CMAJORS	CONFIG_STREAMS_LDL_NMAJORS
#define LDL_CMAJOR_0	CONFIG_STREAMS_LDL_MAJOR
#define LDL_UNITS	CONFIG_STREAMS_LDL_NMINORS
#endif				/* LFS */

#define DRV_ID		LDL_DRV_ID
#define DRV_NAME	LDL_DRV_NAME
#define CMAJORS		LDL_CMAJORS
#define CMAJOR_0	LDL_CMAJOR_0
#define UNITS		LDL_UNITS

#ifdef MODULE
#define DRV_BANNER	LDL_BANNER
#else				/* MODULE */
#define DRV_BANNER	LDL_SPLASH
#endif				/* MODULE */

STATIC struct module_info ldl_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 4,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 0x10000,		/* Hi water mark */
	.mi_lowat = 0x04000,		/* Lo water mark */
};

STATIC struct module_stat ldl_rdmstat = { };
STATIC struct module_stat ldl_wrmstat = { };

STATIC int ldl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int ldl_close(queue_t *, int, cred_t *);

STATIC int ldl_rsrv(queue_t *);
STATIC int ldl_wput(queue_t *, mblk_t *);
STATIC int ldl_wsrv(queue_t *);

STATIC struct qinit ldl_rinit = {
	.qi_srvp = ldl_rsrv,		/* Read service */
	.qi_qopen = ldl_open,		/* Each open */
	.qi_qclose = ldl_close,		/* Last close */
	.qi_minfo = &ldl_minfo,		/* Information */
	.qi_mstat = &ldl_rdmstat,	/* Statistics */
};

STATIC struct qinit ldl_winit = {
	.qi_putp = ldl_wput,		/* Write put (message from below) */
	.qi_srvp = ldl_wsrv,		/* Write service */
	.qi_minfo = &ldl_minfo,		/* Information */
	.qi_mstat = &ldl_wrmstat,	/* Statistics */
};

struct streamtab ldl_info = {
	.st_rdinit = &ldl_rinit,	/* Upper read queue */
	.st_wrinit = &ldl_winit,	/* Upper write queue */
};

/*
 *  ldl: Yet another unfinished DLPI driver
 *
 *  Version: 0.4.1
 *
 *  Copyright (C) 1998, 1999 Ole Husgaard (sparre@login.dknet.dk)
 *
 *  Token ring support by Dave Grothe Copyright (C) 1999
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * 
 * 
 * This driver does not attempt to do any hardware programming. Instead
 * this driver is an interface between the existing Linux network device
 * driver interface and the DLPI interface.
 * 
 * It will also work in cooperation with Linux protocol drivers. You can
 * BIND the IP protocol to a Linux netdevice that is also used by the
 * Linux IP socket interface:
 * Incoming packets are delivered to this DLPI driver *and* to the Linux
 * IP socket interface. But packets sent out by this DLPI driver are *not*
 * seen by the Linux IP socket interface, and packets send out by the Linux
 * IP socket interface are *not* seen by this driver. This means that the
 * Linux IP code will see a response to an IP protocol packet sent out from
 * this DLPI driver.
 * To avoid this sort of confusion, you should do one of:
 *   1) Use this driver for listening only.
 *   2) Use protocols *not* used by the Linux socket code.
 *   3) Tell Linux that the interface is down (does this work?)
 *
 * The PPA (device number) used for ATTACH is determined by looking at
 * the Linux netdevice list. The first netdevice has PPA 0. You can see
 * the device list in /proc/net/dev, or you can do a LDL_FINDPPA IOCTL if
 * you know the netdevice name.
 * You can OR the PPA with one of the LDL_FRAME_xxx constants defined in
 * ldl.h if you want to use other framing types that the default ethernet II.
 * 
 * Currently only ethernet devices and the loopback device are supported.
 * 
 * 
 * TODO: Sending IP packets from ldl to the native Linux IP layer across
 * the loopback interface no longer works. The native IP receive code now
 * checks a field skb->dst to see if the packet is bogus. This field is set
 * on receive from a network card, but due to some optimizations this is
 * set before transmit when using the loopback interface. The skb->dst field
 * should point to an entry in the ipv4 FIB, and I do not think that there is
 * any way to get at this without changing the native Linux ipv4 code.
 * And it looks like doing this would either slow down the native ipv4 code,
 * open up a security hole, or make the kernel significantly bigger.
 * Another option could be to change this driver to check for ipv4 on the
 * loopback interface. When this is detected, we could set skb->dst.
 * 
 */

#if defined(KERNEL_2_3)

#define ldldev		net_device
#define driver_started(dev)	 1
#define	START_BH_ATOMIC(dev)	spin_lock_bh(&(dev)->queue_lock)
#define	END_BH_ATOMIC(dev)	spin_unlock_bh(&(dev)->queue_lock)

#else

#define ldldev		device
#define netif_queue_stopped(dev) ((dev)->tbusy)
#define driver_started(dev)	 ((dev)->start)
#define	START_BH_ATOMIC(dev)	start_bh_atomic()
#define	END_BH_ATOMIC(dev)	end_bh_atomic()

#endif

#if defined(ARPHRD_IEEE802_TR)
#define	IS_ARPHRD_IEEE802_TR(dev) (dev)->type == ARPHRD_IEEE802_TR
#else
#define	IS_ARPHRD_IEEE802_TR(dev) 0
#endif

#define LDL_MAX_HDR_LEN	64

#define MAXADDRLEN	8
#define MAXSAPLEN	8
#define MAXDLSAPLEN	(MAXSAPLEN + MAXADDRLEN)

#define MINDATA		46
#define MAXDATA		4096

#define DONE		0
#define RETRY		1

typedef struct {
	unsigned char sap[MAXSAPLEN];
} sap_t;

/*
 *  Private per stream flags
 */
#define LDLFLAG_PROMISC_SAP     0x0400	/* Promiscous SAP mode */
#define LDLFLAG_SET_ADDR	0x1000	/* Changing physical address */
#define LDLFLAG_HANGUP		0x2000	/* Deferred M_HANGUP needed */
#define LDLFLAG_HANGUP_DONE	0x4000	/* M_HANGUP completed */
#define LDLFLAG_PRIVATE		0x7400	/* Private flag mask */

/*
 *  Transmission priorities
 */
#if defined(TC_PRIO_BULK)
#define LDLPRI_LO	TC_PRIO_BULK
#define LDLPRI_MED	TC_PRIO_BESTEFFORT
#define LDLPRI_HI	TC_PRIO_INTERACTIVE
#elif defined(SOPRI_BACKGROUND)
#define LDLPRI_LO	SOPRI_BACKGROUND
#define LDLPRI_MED	SOPRI_NORMAL
#define LDLPRI_HI	SOPRI_INTERACTIVE
#else
#define LDLPRI_LO       0
#define LDLPRI_MED      0
#define LDLPRI_HI       0
#endif

/*
 *  How long to backout in case of congestion
 */
#define CONGESTION_BACKOFF_TICKS ((HZ/200 <= 1) ? 2 : (HZ/200))

/*
 *  The following structures are a bit complicated, but they give efficient
 *  code in the receive data code path and fair behaviour in the transmit
 *  path when local transmit congestion is happening.
 *
 *  Data is received from a native Linux netdevice as follows:
 *  - First a struct packet_type that identifies the device and/or protocol
 *    must be registered with the Linux kernel using the dev_add_pack()
 *    function.
 *  - When a packet (with the right protocol) is received (on the right
 *    device) a callback function (a field of the struct packet_type) is
 *    called. Here the callback function is rcv_func(). The callback function
 *    takes three arguments:
 *	struct sk_buff *skb	Native Linux network data buffer
 *	struct device *dev	The native Linux device that data arrived on
 *	struct packet_type *pt	The packet type that matched protocol/device
 *    The callback is not called during the receive interrupt, but just after.
 *    The data is copied into a STREAMS data message. (We could use esballoc())
 *  - When the receive function should stop, dev_remove_pack() must be called.
 *
 *  The following considerations are taken:
 *  - For every packet_type (but the first) listening on a Linux netdevice,
 *    a sk_buff structure is copied. The data is not copied, but the sk_buff
 *    is large (compared to mblk_t). This copy should be avoided by using
 *    only a single packet_type when several DLPI devices are bound to the
 *    same SAP on the same device. (Have a look at function net_bh() in
 *    file net/core/dev.c of the Linux kernel if you want to see what happens.)
 *  - When several DLPI devices are bound to the same SAP on the same device,
 *    only the message blocks (not the data blocks) should be copied.
 *    (Use dupmsg() instead of copymsg() whenever possible).
 *  - When several DLPI devices are bound to the same SAP on the same device,
 *    the DLPI devices that data must be delivered to should be trivially easy
 *    to find.
 *  - Subsequent (peer) bindings must be supported by the DLPI device. (This
 *    complicates things a bit.)
 *  - We want to limit the amount of data we have in the Linux netdevice
 *    transmit queue at any time to be fair to the native Linux networking
 *    code.
 *
 *  To facilitate this the following structures are used:
 *  struct ndev
 *    This is the internal representation of a native Linux netdevice. It has
 *    the following members:
 *	- A pointer to the native Linux netdevice.
 *	- The maximum byte count that is allowed to be on the transmission
 *        queue of this netdevice.
 *	- The current byte count that this driver currently has on the
 *	  transmission queue of this netdevice.
 *	- A timer handle for a timeout to restart after congestion has been
 *	  detected.
 *	- A list of the endpoints attached to this netdevice.
 *  struct pt
 *    This is the native Linux struct packet_type with some extra fields. The
 *    first member is a struct packet_type so we can use a simple typecast in
 *    the receive callback function. Apart from a magic number for debugging
 *    purposes, the only other member is a pointer to the first entry of a
 *    linked list of listeners for this packet_type.
 *  struct sap
 *    This represents a DLPI binding (and also a listener for a certain packet
 *    type). It has the following members:
 *	- A pointer to the DLPI stream that is bound (is listening).
 *	- A pointer to the struct pt that is used for receiving.
 *	- A pointer to the next entry on the linked list of listeners for this
 *	  packet type.
 *	- A pointer to the next entry on a linked list of subsequent (peer)
 *	  bindings on the same DLPI stream.
 *      - The SAP that is bound (length and type determined from dl->framing).
 */

/* Netdevice structure */
struct ndev {
	dl_ulong magic;			/* Debugging paranoia (==0 iff free) */
	struct ldldev *dev;		/* Linux netdevice */
	struct dl *endpoints;		/* List of endpoints attached to this */
	struct ndev *next;		/* List of all struct ndev */
	atomic_t wr_cur;		/* Currently on write queue */
	int wr_max;			/* Max. outstanding on write queue */
	int wr_min;			/* Min. outstanding on write queue */
	toid_t tx_congest_timer;	/* Tx. congestion backoff restart timer */
	int sleeping;			/* Tx. sleeping flag */
};

/* Packet type structure */
struct pt {
	struct packet_type pt;		/* Packet type filter */
	dl_ulong magic;			/* Debugging paranoia (==0 iff free) */
	rwlock_t lock;			/* listening SAP list synchronization */
	struct sap *listen;		/* listening SAP list (==NULL iff free) */
	struct pt *next;		/* List of all struct pt */
};

/* Binding structure */
struct sap {
	sap_t sap;			/* Bound SAP */
	dl_ulong magic;			/* Debugging paranoia (==0 iff free) */
	struct dl *dl;			/* DLPI minor pointer (==NULL iff free) */
	struct pt *pt;			/* Packet type filter */
	struct sap *next_listen;	/* Listening sap list (same pkt. type) */
	struct sap *next_sap;		/* Subsequent sap list (same dl) */
};

/* DLPI endpoint structure */
struct dl {
	dl_ulong magic;			/* Debugging paranoia (==0 iff free) */
	queue_t *rq;			/* Read queue */
	ulong dlstate;			/* State */
	dl_ulong flags;			/* Current flags */
	dl_ulong lost_rcv, lost_send;	/* Statistics */
	dl_ulong priority;		/* Current priority */
	struct dl *next_open;		/* List of open devices */
	struct ndev *ndev;		/* Our netdevice, known after ATTACH */
	struct dl *next_ndev;		/* List of endpoints on same netdevice */
	int bufwait;			/* Flag: We are waiting for a buffer */
	int addr_len;			/* Link physical address length */
	int sap_len;			/* Link SAP length */
	int machdr_len;			/* MAC header length */
	int machdr_reserve;		/* MAC header alloc length */
	int mtu;			/* DLSDU */
	struct sap *sap;		/* Primary SAP */
	struct sap *subs;		/* Subsequent (peer) SAPs */
	dl_ulong framing;		/* Data link framing used */
	unsigned char oui[3];		/* OUI for SNAP framing */

	int (*wantsframe) (struct dl * dl, unsigned char *fr, int len);
	mblk_t *(*rcvind) (struct dl * dl, mblk_t *dp);
	int (*mkhdr) (struct dl * dl, unsigned char *dst, int datalen, struct sk_buff * skb);
};

/* Magic numbers */
#define NDEV_MAGIC	0xA5B4C3D1
#define PT_MAGIC	0xA5B4C3D2
#define SAP_MAGIC	0xA5B4C3D3
#define DL_MAGIC	0xA5B4C3D4

/* Default netdevice write queue water marks */
#define DEV_WR_MIN 0x08000	/* Low water: 32 Kb */
#define DEV_WR_MAX 0x10000	/* High water: 64 Kb */

ldl_gstats_ioctl_t ldl_gstats;
#define	ginc(field)	atomic_inc(&ldl_gstats.field)
STATIC unsigned long ldl_debug_mask;

STATIC struct pt *first_pt = NULL;
STATIC spinlock_t first_pt_lock = SPIN_LOCK_UNLOCKED;
STATIC struct ndev *first_ndev = NULL;
STATIC struct dl dl_dl[LDL_N_MINOR] = { {}, };

STATIC struct dl *first_open = NULL;
STATIC spinlock_t first_open_lock = SPIN_LOCK_UNLOCKED;

STATIC int n_hangup = 0;
STATIC int pt_n_alloc = 0;
STATIC int sap_n_alloc = 0;
STATIC int ndev_n_alloc = 0;

STATIC char *ldl_pkt_type(unsigned saptype);

STATIC int rcv_func(struct sk_buff *skb, struct ldldev *dev, struct packet_type *pt);
STATIC INLINE int tx_func_raw(struct dl *dl, mblk_t *mp);

STATIC void
ldl_bfr_dump(char *msg, void *ptr, int len, int alldata)
{
	unsigned char *p = ptr;
	int bytes = 0;
	int trunc = 0;
	int nl = 0;

	printk("%s -- %d bytes\n", msg, len);

	if (!alldata && len > 24) {
		len = 24;
		trunc = 1;
	}

	for (; len > 0; len--, p++) {
		printk("%02x ", *p);
		nl = 0;
		if (!trunc && ++bytes == 16) {
			printk("\n");
			nl = 1;
			bytes = 0;
		}
	}

	if (!nl)
		printk("\n");
}

STATIC void
ldl_mp_dump(char *msg, mblk_t *mp, int alldata)
{
	ldl_bfr_dump(msg, mp->b_rptr, mp->b_wptr - mp->b_rptr, alldata);
}

STATIC void
ldl_mp_data_dump(char *msg, mblk_t *mp, int alldata)
{
	for (; mp != NULL && mp->b_datap->db_type != M_DATA; mp = mp->b_cont) ;
	if (mp == NULL)
		return;

	ldl_mp_dump(msg, mp, alldata);
}

STATIC void
ldl_skbuff_dump(char *msg, struct sk_buff *skb, int alldata)
{
#define	L	unsigned long
	int cnt;

	if (msg != NULL)
		printk("%s:\n", msg);

	printk("head=%lx data=%lx tail=%lx end=%lx truesize=%d\n"
	       "h.raw=%lx nh.raw=%lx mac.raw=%lx type=%s len=%d\n", (L) skb->head, (L) skb->data,
	       (L) skb->tail, (L) skb->end, skb->truesize, (L) skb->h.raw, (L) skb->nh.raw,
	       (L) skb->mac.raw, ldl_pkt_type(skb->pkt_type), skb->len);
	if (alldata)
		cnt = skb->tail - skb->head;	/* dump everything in the buffer */
	else
		alldata = cnt = 64;	/* make alldata non-zero */

	ldl_bfr_dump("Dump from skb->head", skb->head, cnt, alldata);

#undef L
}

STATIC char *
ldl_framing_type(unsigned long framing)
{
	switch (framing & LDL_FRAME_MASK) {
	case LDL_FRAME_EII:
		return ("LDL_FRAME_EII");
	case LDL_FRAME_802_2:
		return ("LDL_FRAME_802_2");
	case LDL_FRAME_SNAP:
		return ("LDL_FRAME_SNAP");
	case LDL_FRAME_802_3:
		return ("LDL_FRAME_802_3");
	case LDL_FRAME_RAW_LLC:
		return ("LDL_FRAME_RAW_LLC");
	}
	return ("LDL_FRAME_UNKNOWN");
}

STATIC char *
ldl_pkt_type(unsigned saptype)
{
	switch (saptype) {
#ifdef ETH_P_ECHO
	case ETH_P_ECHO:
		return ("ETH_P_ECHO");
#endif
	case ETH_P_PUP:
		return ("ETH_P_PUP");
	case ETH_P_IP:
		return ("ETH_P_IP");
	case ETH_P_X25:
		return ("ETH_P_X25");
	case ETH_P_ARP:
		return ("ETH_P_ARP");
	case ETH_P_BPQ:
		return ("ETH_P_BPQ");
	case ETH_P_DEC:
		return ("ETH_P_DEC");
	case ETH_P_DNA_DL:
		return ("ETH_P_DNA_DL");
	case ETH_P_DNA_RC:
		return ("ETH_P_DNA_RC");
	case ETH_P_DNA_RT:
		return ("ETH_P_DNA_RT");
	case ETH_P_LAT:
		return ("ETH_P_LAT");
	case ETH_P_DIAG:
		return ("ETH_P_DIAG");
	case ETH_P_CUST:
		return ("ETH_P_CUST");
	case ETH_P_SCA:
		return ("ETH_P_SCA");
	case ETH_P_RARP:
		return ("ETH_P_RARP");
	case ETH_P_ATALK:
		return ("ETH_P_ATALK");
	case ETH_P_AARP:
		return ("ETH_P_AARP");
	case ETH_P_IPX:
		return ("ETH_P_IPX");
	case ETH_P_IPV6:
		return ("ETH_P_IPV6");
	case ETH_P_802_3:
		return ("ETH_P_802_3");
	case ETH_P_AX25:
		return ("ETH_P_AX25");
	case ETH_P_ALL:
		return ("ETH_P_ALL");
	case ETH_P_802_2:
		return ("ETH_P_802_2");
	case ETH_P_SNAP:
		return ("ETH_P_SNAP");
	case ETH_P_DDCMP:
		return ("ETH_P_DDCMP");
	case ETH_P_WAN_PPP:
		return ("ETH_P_WAN_PPP");
	case ETH_P_PPP_MP:
		return ("ETH_P_PPP_MP");
	case ETH_P_LOCALTALK:
		return ("ETH_P_LOCALTALK");
	case ETH_P_PPPTALK:
		return ("ETH_P_PPPTALK");
	case ETH_P_TR_802_2:
		return ("ETH_P_TR_802_2");
	case ETH_P_MOBITEX:
		return ("ETH_P_MOBITEX");
	case ETH_P_CONTROL:
		return ("ETH_P_CONTROL");
	case ETH_P_IRDA:
		return ("ETH_P_IRDA");
	case 0:
		return ("NULL(0)");
	}

	return ("ETH_P_UNKNOWN");
}

/****************************************************************************/
/*                                                                          */
/*  Packet type and SAP handling.                                           */
/*                                                                          */
/****************************************************************************/

/*
 *  sap_create  - create and add another packet type (sap) to a device.
 *
 *  Returns 0 on success, -1 on failure.
 *
 *  Notice that sap_create is always called under SPLSTR()
 */
STATIC int
sap_create(struct dl *dl, sap_t dlsap, dl_ushort saptype)
{
	struct pt *pt, *npt;
	struct sap *sap;

	ASSERT(dl != NULL);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT(dl->ndev != NULL);

	saptype = htons(saptype);

	if ((sap = ALLOC(sizeof(*sap))) == NULL)
		return -1;
	++sap_n_alloc;
	memset(sap, 0, sizeof *sap);

	sap->magic = SAP_MAGIC;
	sap->dl = dl;
	sap->sap = dlsap;
	if (dl->sap == NULL) {
		/* This is a primary bind */
		sap->next_sap = NULL;
		dl->sap = sap;
	} else {
		/* This is a subsequent bind */
		sap->next_sap = dl->subs;
		dl->subs = sap;
	}

	/* Does Linux already pass us this packet type on this device? */
	npt = NULL;
	spin_lock(&first_pt_lock);
	do {
		for (pt = first_pt; pt; pt = pt->next)
			if (pt->pt.type == saptype && pt->pt.dev == dl->ndev->dev)
				break;
		if (pt == NULL) {
			if (npt == NULL) {
				spin_unlock(&first_pt_lock);
				if ((npt = ALLOC(sizeof(*pt))) == NULL) {
					FREE(sap);
					sap_n_alloc--;
					return -1;
				}
				++pt_n_alloc;
				memset(npt, 0, sizeof *npt);
				spin_lock(&first_pt_lock);
			} else {
				npt->next = first_pt;
				first_pt = pt = npt;
			}
		} else if (npt != NULL) {
			FREE(npt);
			npt = NULL;
			pt_n_alloc--;
		}
	} while (pt == NULL);
	sap->pt = pt;

	if (pt->listen == NULL) {
		/* New, unused packet_type */
		ASSERT(pt->magic == 0);
		/* No need to synchronize with rcv_func() */
		sap->next_listen = pt->listen;
		pt->listen = sap;
		rwlock_init(&pt->lock);
		pt->magic = PT_MAGIC;
		pt->pt.type = saptype;
		pt->pt.dev = dl->ndev->dev;
		pt->pt.func = rcv_func;
#if HAVE_KMEMB_STRUCT_PACKET_TYPE_AF_PACKET_PRIV
		pt->pt.af_packet_priv = (void *) 1;	/* indicate "new style" packet handler */
#elif HAVE_KMEMB_STRUCT_PACKET_TYPE_DATA
		pt->pt.data = (void *) 1;	/* indicate "new style" packet handler */
#else
#error Must have HAVE_KMEMB_STRUCT_PACKET_TYPE_DATA or HAVE_KMEMB_STRUCT_PACKET_TYPE_AF_PACKET_PRIV defined.
#endif
#if HAVE_KMEM_STRUCT_PACKET_TYPE_NEXT
		pt->pt.next = NULL;
#elif HAVE_KMEMB_STRUCT_PACKET_TYPE_LIST
		pt->pt.list = (struct list_head)LIST_HEAD_INIT(pt->pt.list);
#else
#error Must have HAVE_KMEMB_STRUCT_PACKET_TYPE_NEXT or HAVE_KMEMB_STRUCT_PACKET_TYPE_LIST defined.
#endif
		spin_unlock(&first_pt_lock);
		dev_add_pack(&pt->pt);
	} else {
		/* Re-use of packet_type */
		ASSERT(pt->magic == PT_MAGIC);
		ASSERT(pt->pt.type == saptype);
		ASSERT(pt->pt.func == rcv_func);

		write_lock(&pt->lock);
		sap->next_listen = pt->listen;
		pt->listen = sap;
		write_unlock(&pt->lock);
		spin_unlock(&first_pt_lock);
	}

	return 0;
}

/*
 *  sap_destroy  - remove and destroy a packet type (sap) from a device.
 *
 *  Returns 0 on success, -1 if not found.
 */
STATIC int
sap_destroy(struct dl *dl, struct sap *sap)
{
	pl_t psw;
	struct pt *pt, *opt;
	struct sap **sapp_dl, **sapp_pt;

	ASSERT(dl != NULL);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT(sap != NULL);
	ASSERT(sap->magic == SAP_MAGIC);
	ASSERT(dl->subs != NULL || sap == dl->sap);

	pt = sap->pt;
	ASSERT(pt != NULL);
	ASSERT(pt->magic == PT_MAGIC);

	SPLSTR(psw);
	spin_lock(&first_pt_lock);
	write_lock(&pt->lock);
	if (pt->listen == sap && sap->next_listen == NULL) {
		/* It is the last use of this packet_type */
		write_unlock(&pt->lock);
		if (pt == first_pt)
			first_pt = pt->next;
		else {
			for (opt = first_pt; opt->next != NULL; opt = opt->next)
				if (opt->next == pt) {
					opt->next = pt->next;
					break;
				}
		}
		spin_unlock(&first_pt_lock);
		dev_remove_pack(&pt->pt);
		pt->magic = 0;
		FREE(pt);
		--pt_n_alloc;
	} else {
		spin_unlock(&first_pt_lock);
		sapp_pt = &pt->listen;
		for (;;) {
			ASSERT(*sapp_pt != NULL);
			if (*sapp_pt == NULL) {
				/* 
				 * Not found, but it should be there:
				 * Emergency brake
				 */
				write_unlock(&pt->lock);
				SPLX(psw);
				return -1;
			}
			if (*sapp_pt == sap)
				break;
			sapp_pt = &(*sapp_pt)->next_listen;
		}
		*sapp_pt = sap->next_listen;
		write_unlock(&pt->lock);
	}

	if (dl->sap == sap) {
		ASSERT(dl->subs == NULL);
		dl->sap = NULL;
	} else {
		sapp_dl = &dl->subs;
		for (;;) {
			ASSERT(*sapp_dl != NULL);
			if (*sapp_dl == NULL) {
				/* 
				 * Not found, but it should be there:
				 * Emergency brake
				 */
				SPLX(psw);
				return -1;
			}
			ASSERT((*sapp_dl)->magic == SAP_MAGIC);
			ASSERT((*sapp_dl)->dl == dl);
			if (*sapp_dl == sap)
				break;
			sapp_dl = &(*sapp_dl)->next_sap;
		}
		*sapp_dl = sap->next_sap;
	}

	sap->dl = NULL;
	sap->pt = NULL;
	sap->magic = 0;
	FREE(sap);
	--sap_n_alloc;

	SPLX(psw);
	return 0;
}

/*
 *  sap_destroy_all  - remove and destroy all packet types (sap) from a device.
 */
STATIC void
sap_destroy_all(struct dl *dl)
{
	int ret;

	ASSERT(dl != NULL);

	while (dl->subs) {
		ASSERT(dl->subs->pt != NULL);
		ret = sap_destroy(dl, dl->subs);
		ASSERT(ret == 0);
		if (ret != 0)
			return;	/* Emergency brake */
	}
	if (dl->sap) {
		ASSERT(dl->sap->pt != NULL);
		ret = sap_destroy(dl, dl->sap);
		ASSERT(ret == 0);
	}
}

/****************************************************************************/
/*                                                                          */
/*  Hangup handling.                                                        */
/*                                                                          */
/****************************************************************************/

/*
 *  hangup_set  - set endpoint flag for hangup
 *
 *  Not reentrant.
 */
STATIC INLINE void
hangup_set(struct dl *dl)
{
	if ((dl->flags & LDLFLAG_HANGUP) == 0) {
		/* 
		 *  A hangup should never happen on an unattached device.
		 */
		ASSERT(dl->dlstate != DL_UNATTACHED);

		/* 
		 *  The hangup has not already been done.
		 */
		ASSERT((dl->flags & LDLFLAG_HANGUP_DONE) != 0);

		/* 
		 *  This endpoint should already have been disassociated
		 *  from the netdevice.
		 */
		ASSERT(dl->ndev == NULL);

		dl->flags |= LDLFLAG_HANGUP;	/* Set flag */
		++n_hangup;	/* Update count */
	}
}

/*
 *  hangup_do  - do the actual hangup on an endpoint
 */
STATIC INLINE void
hangup_do(struct dl *dl)
{
	pl_t psw;

	SPLSTR(psw);

	ASSERT((dl->flags & LDLFLAG_HANGUP) != 0);

	/* Has the hangup been done already? */
	if ((dl->flags & LDLFLAG_HANGUP_DONE) != 0) {
		SPLX(psw);
		return;
	}

	ASSERT(n_hangup > 0);

	/* 
	 *  In the transient states a hangup cannot easily be done.
	 *  A check of the hangup flag retries the hangup when these states
	 *  are left.
	 */
	if (dl->dlstate == DL_UNBIND_PENDING || dl->dlstate == DL_SUBS_UNBIND_PND) {
		SPLX(psw);
		return;
	}

	/* 
	 *  Go into the DL_UNATTACHED state before hanging up.
	 *  This ensures that all resources associated with the endpoint
	 *  are released. In particular, no more unitdata indications
	 *  should come from this endpoint.
	 */
	if (dl->dlstate == DL_IDLE) {
		sap_destroy_all(dl);
		dl->dlstate = DL_UNBOUND;
	}
	if (dl->dlstate == DL_UNBOUND) {
		STATIC void ndev_release(struct dl *dl);

		ndev_release(dl);
		dl->addr_len = 0;
		dl->ndev = NULL;
		dl->dlstate = DL_UNATTACHED;
	}
	ASSERT(dl->dlstate == DL_UNATTACHED);

	SPLX(psw);

	if (putctl(dl->rq->q_next, M_HANGUP)) {
		SPLSTR(psw);
		if ((dl->flags & LDLFLAG_HANGUP_DONE) == 0) {
			dl->flags |= LDLFLAG_HANGUP_DONE;
			--n_hangup;
		}
		SPLX(psw);
	} else
		printk("ldl: cannot send M_HANGUP\n");
}

/****************************************************************************/
/*                                                                          */
/*  Netdevice handling.                                                     */
/*                                                                          */
/****************************************************************************/

/*
 *  ndev_find  - find netdevice
 *
 *  Not reentrant, returns NULL if not found.
 */
STATIC INLINE struct ndev *
ndev_find(struct ldldev *dev)
{
	struct ndev *ndev;

	for (ndev = first_ndev; ndev; ndev = ndev->next)
		if (ndev->dev == dev)
			break;
	return ndev;
}

/*
 *  ndev_get  - create or find netdevice
 *
 *  Not reentrant, returns NULL on failure.
 */
STATIC struct ndev *
ndev_get(dl_ulong ppa)
{
	dl_ulong i;
	struct ldldev *dev;
	struct ndev *ndev;

	/* Find the Linux netdevice to attach to */
	for (dev = dev_base, i = 0; dev; dev = dev->next, ++i)
		if (ppa == i)
			break;

	if (dev == NULL)
		return NULL;
	ASSERT(ppa == i);

	if ((ndev = ndev_find(dev)) == NULL) {
		if ((ndev = ALLOC(sizeof(*ndev))) == NULL)
			return NULL;
		++ndev_n_alloc;
		memset(ndev, 0, sizeof *ndev);
		ndev->magic = NDEV_MAGIC;
		ndev->dev = dev;
		ndev->wr_max = DEV_WR_MAX;
		ndev->wr_min = DEV_WR_MIN;
	}

	return ndev;
}

/*
 *  ndev_attach  - attach endpoint to a netdevice
 *
 *  Not reentrant.
 */
STATIC void
ndev_attach(struct ndev *ndev, struct dl *dl)
{
	ASSERT(ndev != NULL);
	ASSERT(ndev->magic == NDEV_MAGIC);

	ASSERT(dl != NULL);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT(dl->ndev == NULL);

	dl->ndev = ndev;
	dl->next_ndev = ndev->endpoints;
	ndev->endpoints = dl;
}

/*
 *  ndev_free  - free netdevice
 *
 *  Not reentrant.
 */
STATIC void
ndev_free(struct ndev *ndev)
{
	ASSERT(ndev != NULL);
	ASSERT(ndev->magic == NDEV_MAGIC);
	ASSERT(ndev->dev == NULL);

	if (atomic_read(&ndev->wr_cur) > 0 || ndev->endpoints != NULL)
		return;		/* Have to wait to free */

	if (ndev->tx_congest_timer) {
		/* Drop old timer */
		untimeout(ndev->tx_congest_timer);
		ndev->tx_congest_timer = 0;
	}
	ndev->magic = 0;
	FREE(ndev);
	--ndev_n_alloc;
}

/*
 *  ndev_release  - detach from netdevice
 *
 *  Not reentrant.
 */
STATIC void
ndev_release(struct dl *dl)
{
	struct ndev *ndev = dl->ndev;
	struct dl **dlp;

	ASSERT(ndev != NULL);
	ASSERT(ndev->magic == NDEV_MAGIC);
	for (dlp = &ndev->endpoints; *dlp; dlp = &(*dlp)->next_ndev) {
		ASSERT((*dlp)->magic == DL_MAGIC);
		ASSERT((*dlp)->ndev == ndev);
		if (*dlp == dl)
			break;
	}
	ASSERT(*dlp == dl);
	*dlp = dl->next_ndev;
	dl->ndev = NULL;
	if (ndev->endpoints == NULL) {
		/* No more attached endpoints: Try to free. */
		if (ndev->dev != NULL) {
			/* 
			 * We may get unloaded while the driver below
			 * has buffers from us still queued.  Bad news
			 * since we have a destructor function pointer
			 * pointing back to us.  So we have to flush
			 * the driver's queues before we go away.  Of
			 * course this also flushes buffers for other
			 * clients of the driver, but that's just the
			 * way has to be.  DMG 8/25/00
			 */
			START_BH_ATOMIC(ndev->dev);
			qdisc_reset(ndev->dev->qdisc);
			END_BH_ATOMIC(ndev->dev);
			ndev->dev = NULL;
		}
		ndev_free(ndev);
	}
}

/*
 *  ndev_down  - handle netdevice shutdown
 *
 *  The hard flag is set if the netdevice is removed.
 *  Not reentrant.
 */
STATIC void
ndev_down(struct ndev *ndev, int hard)
{
	struct dl *dl;

	ASSERT(ndev->magic == NDEV_MAGIC);

	if (hard)
		ndev->dev = NULL;
	for (dl = ndev->endpoints; dl; dl = dl->next_ndev) {
		ASSERT(dl->magic == DL_MAGIC);

		if (hard || (dl->flags & LDLFLAG_SET_ADDR) == 0) {
			ndev_release(dl);
			hangup_set(dl);
		}
	}
}

/*
 *  ndev_wr_wakeup_endp  - wakeup endpoints
 *
 *  Not reentrant.
 */
STATIC void
ndev_wr_wakeup_endp(struct ndev *ndev)
{
	struct dl *dl;

	ASSERT(ndev->magic == NDEV_MAGIC);

	for (dl = ndev->endpoints; dl; dl = dl->next_ndev) {
		ASSERT(dl->magic == DL_MAGIC);
		ASSERT(dl->ndev == ndev);

		qenable(WR(dl->rq));
	}
}

/*
 *  ndev_wr_wakeup  - End device write "sleep".
 *
 *  Not reentrant.
 */
STATIC void
ndev_wr_wakeup(struct ndev *ndev)
{
	ASSERT(ndev->magic == NDEV_MAGIC);
	ASSERT(ndev->sleeping);

	if (ndev->tx_congest_timer) {
		untimeout(ndev->tx_congest_timer);	/* Drop old timer */
		ndev->tx_congest_timer = 0;
	}
	ndev->sleeping = 0;

	ndev_wr_wakeup_endp(ndev);
}

STATIC void
tx_congestion_timeout(caddr_t dp)
{
	pl_t psw;
	struct ndev *ndev = (struct ndev *) dp;

	SPLSTR(psw);

	ASSERT(ndev->magic == NDEV_MAGIC);
	if (ndev->tx_congest_timer != 0) {
		ndev->tx_congest_timer = 0;
		ndev_wr_wakeup_endp(ndev);
	}

	SPLX(psw);
}

/*
 *  ndev_wr_sleep  - Start device write "sleep".
 *
 *  Not reentrant.
 */
STATIC void
ndev_wr_sleep(struct ndev *ndev)
{
	ASSERT(!ndev->sleeping);

	if (!ndev->tx_congest_timer)
		ndev->tx_congest_timer =
		    timeout(tx_congestion_timeout, (caddr_t) ndev, CONGESTION_BACKOFF_TICKS);
	if (ndev->tx_congest_timer)
		ndev->sleeping = 1;
	else
		printk("ldl: ndev_wr_sleep() failed\n");
}

/*
 *  This is the skb destructor callback.
 *  Need to adjust amount of outstanding data on the netdevice write queue,
 *  and possibly free our ndev structure if no longer in use.
 */
STATIC void
ndev_skb_destruct(struct sk_buff *skb)
{
	struct ndev *ndev;

	ASSERT(skb != NULL);

#ifdef KERNEL_2_1
	if (skb_cloned(skb))
		return;
#else
	if (skb->data_skb != skb)
		return;
#endif

	ndev = (struct ndev *) skb->sk;
	skb->sk = NULL;
	ASSERT(ndev != NULL);
	ASSERT(ndev->magic == NDEV_MAGIC);

	ASSERT(atomic_read(&ndev->wr_cur) >= skb->truesize);
	atomic_sub(skb->truesize, &ndev->wr_cur);

	if (ndev->dev != NULL) {
		if (atomic_read(&ndev->wr_cur) <= ndev->wr_min) {
			if (ndev->sleeping)
				ndev_wr_wakeup(ndev);
		}
	} else {
		ASSERT(ndev->endpoints == NULL);

		if (atomic_read(&ndev->wr_cur) == 0)
			ndev_free(ndev);
	}
}

#if defined(KERNEL_2_1)

STATIC int
ndev_xmit(struct ndev *ndev, struct sk_buff *skb)
{
	pl_t psw;
	int ret;

	ASSERT(skb != NULL);
	ASSERT(ndev != NULL);
	ASSERT(ndev->magic == NDEV_MAGIC);
	ASSERT(ndev->dev != NULL);

	skb->mac.raw = skb->data;
	skb->dev = ndev->dev;
	atomic_add(skb->truesize, &ndev->wr_cur);
	(struct ndev *) skb->sk = ndev;
	skb->destructor = ndev_skb_destruct;

	if (atomic_read(&ndev->wr_cur) <= ndev->wr_max) {
		switch (dev_queue_xmit(skb)) {
		case NET_XMIT_SUCCESS:
		default:
			return DONE;

		case NET_XMIT_DROP:
			return RETRY;

		case NET_XMIT_CN:
		case NET_XMIT_POLICED:
			ret = DONE;
			break;
		}
	} else {
		kfree_skb(skb);
		ret = RETRY;
	}

	SPLSTR(psw);
	if (!ndev->sleeping)
		ndev_wr_sleep(ndev);
	SPLX(psw);
	return ret;
}

#elif defined(too_complicated_KERNEL_2_1)

STATIC int
ndev_xmit(struct ndev *ndev, struct sk_buff *skb)
{
	pl_t psw;
	struct ldldev *dev;

	ASSERT(skb != NULL);
	ASSERT(ndev != NULL);
	ASSERT(ndev->magic == NDEV_MAGIC);
	ASSERT(ndev->dev != NULL);

	skb->mac.raw = skb->data;
	skb->dev = dev = ndev->dev;
	atomic_add(skb->truesize, &ndev->wr_cur);
	(struct ndev *) skb->sk = ndev;
	skb->destructor = ndev_skb_destruct;

	if (atomic_read(&ndev->wr_cur) <= ndev->wr_max) {
		struct Qdisc *q;

		START_BH_ATOMIC(dev);
		q = dev->qdisc;
		ASSERT(q != NULL);
		if (q->enqueue) {
			int ret;

			ret = q->enqueue(skb, q);
			qdisc_wakeup(dev);
			END_BH_ATOMIC(dev);
			if (ret == 1)
				return DONE;
		} else {
			END_BH_ATOMIC(dev);
			if (dev_queue_xmit(skb) >= 0)
				return DONE;
		}
		SPLSTR(psw);
		if (!ndev->sleeping)
			ndev_wr_sleep(ndev);
		SPLX(psw);
		return RETRY;
	} else {
		SPLSTR(psw);
		if (!ndev->sleeping)
			ndev_wr_sleep(ndev);
		SPLX(psw);
		kfree_skb(skb);
		return RETRY;
	}
}

#else

STATIC int
ndev_xmit(struct ndev *ndev, struct sk_buff *skb, int pri)
{
	atomic_add(skb->truesize, &ndev->wr_cur);
	(struct ndev *) skb->sk = ndev;
	skb->dev = ndev->dev;
	skb->arp = 1;
	skb->destructor = ndev_skb_destruct;
	dev_queue_xmit(skb, ndev->dev, pri);
	return DONE;
}
#endif

/****************************************************************************/
/*                                                                          */
/*  Netdevice notifier handling.                                            */
/*                                                                          */
/****************************************************************************/

/*
 *  device_notification  - device notification callback.
 */
STATIC int
device_notification(struct notifier_block *notifier, unsigned long event, void *ptr)
{
	struct ldldev *dev = ptr;
	struct ndev *d;
	struct dl *dl;
	pl_t psw;

	SPLSTR(psw);

#ifdef KERNEL_2_1
	if (event == NETDEV_DOWN || event == NETDEV_UNREGISTER) {
#else
	if (event == NETDEV_DOWN) {
#endif
		if ((d = ndev_find(dev)) != NULL) {
			ndev_down(d, event != NETDEV_DOWN);
			if (n_hangup) {
				SPLX(psw);
				spin_lock(&first_open_lock);
				for (dl = first_open; dl; dl = dl->next_open)
					if ((dl->flags & LDLFLAG_HANGUP) != 0)
						hangup_do(dl);
				spin_unlock(&first_open_lock);
				return NOTIFY_DONE;
			}
		}
	}

	SPLX(psw);

	return NOTIFY_DONE;
}

STATIC struct notifier_block dl_notifier = {
	device_notification,
	NULL,
	0
};

STATIC INLINE int
notifier_register(void)
{
	return register_netdevice_notifier(&dl_notifier);
}

STATIC INLINE int
notifier_unregister(void)
{
	return unregister_netdevice_notifier(&dl_notifier);
}

/****************************************************************************/
/*                                                                          */
/*  bufcall() handling.                                                     */
/*                                                                          */
/****************************************************************************/

STATIC void
dl_bufcallback(long idx)
{
	struct dl *dl = &dl_dl[idx];

	ASSERT(dl->rq != NULL);
	ASSERT(dl->bufwait);

	dl->bufwait = 0;
	qenable(WR(dl->rq));
}

STATIC int
dl_bufcall(struct dl *dl, mblk_t *mp, int size)
{
	ASSERT(!dl->bufwait);
	if ((dl->bufwait = bufcall(size, BPRI_HI, dl_bufcallback, dl - dl_dl)) == 0) {
		printk("ldl: bufcall failed\n");
		freemsg(mp);
		return DONE;
	}
	return RETRY;
}

/****************************************************************************/
/*                                                                          */
/*  A few utility routines.                                                 */
/*                                                                          */
/****************************************************************************/

/*
 *  Translate from DLPI priority to Linux netdevice priority
 */
STATIC INLINE int
pri_dlpi2netdevice(dl_ulong pri)
{
	ASSERT(pri >= 0);
	ASSERT(pri <= 100);

	return (pri < 33) ? LDLPRI_HI : (pri < 66) ? LDLPRI_MED : LDLPRI_LO;
}

/*
 *  Check if a msgblk can be reused for a reply.
 *
 *  If the data block of the first message block is large
 *  enough, any remaining message blocks in the message are
 *  freed, read and write pointers are reset, and 1 is returned.
 *  Otherwise, the message is untouched and 0 is returned.
 */
STATIC INLINE int
reuse_msg(mblk_t *mp, dl_ushort size)
{
	mblk_t *bp;

	ASSERT(mp != NULL);
	ASSERT(mp->b_datap != NULL);

	if (mp->b_datap->db_lim - mp->b_datap->db_base < size) ;
	return 0;
	if (mp->b_datap->db_ref != 1)
		return 0;

	if ((bp = unlinkb(mp)) != NULL)
		freemsg(bp);
	mp->b_wptr = mp->b_rptr = mp->b_datap->db_base;

	return 1;
}

/*
 *  Convert mp to DL_OK_ACK.
 */
STATIC INLINE void
make_dl_ok_ack(mblk_t *mp, dl_ulong primitive)
{
	dl_ok_ack_t *ackp;

	/* ASSERT(reuse_msg(mp, DL_OK_ACK_SIZE)); */

	mp->b_datap->db_type = M_PCPROTO;
	ackp = (dl_ok_ack_t *) mp->b_datap->db_base;
	ackp->dl_primitive = DL_OK_ACK;
	ackp->dl_correct_primitive = primitive;
	mp->b_wptr += DL_OK_ACK_SIZE;
}

/*
 *  Convert mp to DL_ERROR_ACK.
 */
STATIC INLINE void
make_dl_error_ack(mblk_t *mp, dl_ulong primitive, dl_ulong err, dl_ulong uerr)
{
	dl_error_ack_t *ackp;

	/* ASSERT(reuse_msg(mp, DL_ERROR_ACK_SIZE)); */

	mp->b_datap->db_type = M_PCPROTO;
	ackp = (dl_error_ack_t *) mp->b_datap->db_base;
	ackp->dl_primitive = DL_ERROR_ACK;
	ackp->dl_error_primitive = primitive;
	ackp->dl_errno = err;
	ackp->dl_unix_errno = uerr;
	mp->b_wptr += DL_ERROR_ACK_SIZE;
}

/*
 *  Create DL_OK_ACK reply
 *
 *  On success, DONE is returned and *mp points to the ack.
 *  On failure, RETRY is returned and *mp is untouched.
 *  (But if bufcall fails *mp is freed and set to NULL
 *   and a bogus DONE is returned to avoid hanging.)
 */
STATIC INLINE int
do_ok_ack(struct dl *dl, mblk_t **mp, dl_ulong primitive)
{
	if (!reuse_msg(*mp, DL_OK_ACK_SIZE)) {
		mblk_t *bp;

		if ((bp = allocb(DL_OK_ACK_SIZE, BPRI_HI)) == NULL) {
			if (dl_bufcall(dl, *mp, DL_OK_ACK_SIZE) == RETRY)
				return RETRY;
			else {
				*mp = NULL;
				return DONE;
			}
		}
		freemsg(*mp);
		*mp = bp;
	}
	make_dl_ok_ack(*mp, primitive);
	return DONE;
}

/*
 *  Send DL_ERROR_ACK reply
 *
 *  On success, DONE is returned and mp has been reused or freed.
 *  On failure, RETRY is returned and mp is untouched.
 *  (But if bufcall fails mp is freed and a bogus DONE is returned
 *   to avoid hanging.)
 */
STATIC int
reply_error_ack(struct dl *dl, mblk_t *mp, dl_ulong primitive, dl_ulong err, dl_ulong uerr)
{
	if (!reuse_msg(mp, DL_ERROR_ACK_SIZE)) {
		mblk_t *bp;

		if ((bp = allocb(DL_ERROR_ACK_SIZE, BPRI_HI)) == NULL)
			return dl_bufcall(dl, mp, DL_ERROR_ACK_SIZE);
		freemsg(mp);
		mp = bp;
	}
	make_dl_error_ack(mp, primitive, err, uerr);
	putnext(dl->rq, mp);
	ginc(error_ack_cnt);
	return DONE;
}

/****************************************************************************/
/*                                                                          */
/*  Frame type support routines.                                            */
/*                                                                          */
/*  For every different kind of data link framing, we set up three          */
/*  functions to handle it:                                                 */
/*                                                                          */
/*  int xxx_want(struct dl *dl, unsigned char *fr, int len);                */
/*    Given a raw frame, return 0 or 1 to indicate if the given stream      */
/*    wants to receive this frame. In case of a memory allocation failure,  */
/*    the frame may have been truncated, but this function is still called  */
/*    in order to get correct frame drop statistics.                        */
/*                                                                          */
/*  mblk_t *xxx_rcvind(struct dl *dl, mblk_t *dp);                          */
/*    On entry, dp contains a raw data link frame that the stream has       */
/*    indicated it wants to receive (xxx_want function above)               */
/*    This function should remove the data link header from dp and create   */
/*    an DL_UNITDATA_IND message.                                           */
/*    If succesfull, a pointer to the resulting DL_UNITDATA_IND message     */
/*    is returned.                                                          */
/*    In case of failure, dp should be freed, and NULL should be returned.  */
/*                                                                          */
/*  int xxx_mkhdr(struct dl *dl, unsigned char *src, unsigned char *dst,    */
/*                int datalen, struct sk_buff *skb);                        */
/*    On entry, src and dst are pointers to the source and destination      */
/*    DLSAPs, and skb is a pointer to a sk_buff that has just been          */
/*    allocated and has had dl->machdr_reserve bytes reserved at the        */
/*    beginning.                                                            */
/*    The length of src and dst has been validated, but the function must   */
/*    validate their contents and fill in the header.                       */
/*    If succesfull, the header has been filled in and 1 is returned.       */
/*    In case of failure, 0 is returned.                                    */
/*                                                                          */
/****************************************************************************/

/*
 *  Ethernet II
 */

STATIC int
eth_ii_want(struct dl *dl, unsigned char *fr, int len)
{
	ASSERT(dl->sap_len == 2);
	if (len < 14)
		return 0;
	if (*(short *) (fr + 12) != *(short *) dl->sap->sap.sap) {
		struct sap *sap = dl->subs;

		while (sap != NULL) {
			if (*(short *) (fr + 12) == *(short *) sap->sap.sap)
				break;
			sap = sap->next_sap;
		}
		if (sap == NULL)
			return 0;
	}
	if (memcmp(fr, dl->ndev->dev->dev_addr, 6) && memcmp(fr, dl->ndev->dev->broadcast, 6))
		return 0;
	return 1;
}

STATIC mblk_t *
eth_ii_rcvind(struct dl *dl, mblk_t *dp)
{
	mblk_t *bp;
	dl_unitdata_ind_t *ud;
	unsigned short dsap = ntohs(*(unsigned short *) (dp->b_rptr + 12));

	ASSERT(dl->sap_len == 2);
	ASSERT(dl->addr_len == 6);

	if ((bp = allocb(DL_UNITDATA_IND_SIZE + 16, BPRI_LO)) == NULL) {
		freeb(dp);
		return NULL;
	}

	bp->b_datap->db_type = M_PROTO;
	ud = (dl_unitdata_ind_t *) bp->b_rptr;
	ud->dl_primitive = DL_UNITDATA_IND;
	ud->dl_dest_addr_length = 8;
	ud->dl_dest_addr_offset = DL_UNITDATA_IND_SIZE + 8;
	ud->dl_src_addr_length = 8;
	ud->dl_src_addr_offset = DL_UNITDATA_IND_SIZE;
	ud->dl_group_address = *dp->b_rptr & 1;
	bp->b_wptr += DL_UNITDATA_IND_SIZE;

	memcpy(bp->b_wptr, dp->b_rptr + 6, 6);
	bp->b_wptr += 6;
	*(unsigned short *) (bp->b_wptr) = dsap;
	bp->b_wptr += 2;

	memcpy(bp->b_wptr, dp->b_rptr, 6);
	bp->b_wptr += 6;
	*(unsigned short *) (bp->b_wptr) = dsap;
	bp->b_wptr += 2;

	dp->b_rptr += 14;

	linkb(bp, dp);
	return bp;
}

STATIC int
eth_ii_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	unsigned char *hdr;
	unsigned short dsap;

	dsap = htons(*(unsigned short *) (dst + 6));
	hdr = skb_push(skb, 14);
	memcpy(hdr, dst, 6);
	memcpy(hdr + 6, dl->ndev->dev->dev_addr, 6);
	*(unsigned short *) (hdr + 12) = dsap;

	if (dsap != *(unsigned short *) dl->sap->sap.sap) {
		struct sap *sap;

		for (sap = dl->subs; sap; sap = sap->next_sap)
			if (dsap == *(unsigned short *) sap->sap.sap)
				return 1;
		return 0;
	}
	return 1;
}

/*
 *  Ethernet 802.2
 */
STATIC int
eth_8022_want(struct dl *dl, unsigned char *fr, int len)
{
	ASSERT(dl->sap_len == 1);

	/* 
	 * The LDLFLAG_RAW check will disappear when we
	 * get the RAW LLC frametype to work.
	 * It means that when running in RAW mode, any value of the
	 * control field will be accepted.
	 */
	if (len < 17 || (!(dl->flags & LDLFLAG_RAW) && *(fr + 16) != 0x03))
		return 0;
	if (ntohs(*(unsigned short *) (fr + 12)) < 3)
		return 0;
	if (dl->flags & LDLFLAG_PROMISC_SAP)
		return 1;
	if (*(fr + 14) != dl->sap->sap.sap[0]) {
		struct sap *sap = dl->subs;
		while (sap != NULL) {
			if (*(fr + 14) == sap->sap.sap[0])
				break;
			sap = sap->next_sap;
		}
		if (sap == NULL)
			return 0;
	}
#if 0
	{
		int i;
		printk("eth_8022_want: SAP checked, MAC@ not yet ...");
		for (i = 0; i < 16; i++)
			printk("%02x ", fr[i]);
		printk("\n");
	}
#endif
	/* 
	 * Start filtering on MAC addresses ...
	 */
	if (!memcmp(fr, dl->ndev->dev->dev_addr, 6))
		return 1;
	if (!memcmp(fr, dl->ndev->dev->broadcast, 6))
		return 1;
	/* 
	 * Check the registered multicast address list ...
	 */
	{
		struct dev_mc_list *dmi = dl->ndev->dev->mc_list;
		while (dmi != NULL)
			if (!memcmp(fr, dmi->dmi_addr, 6))
				return 1;
			else
				dmi = dmi->next;
	}
	return 0;
}

STATIC mblk_t *
eth_8022_rcvind(struct dl *dl, mblk_t *dp)
{
	mblk_t *bp;
	dl_unitdata_ind_t *ud;
	unsigned short len;

	ASSERT(dl->sap_len == 1);
	ASSERT(dl->addr_len == 6);

	if ((bp = allocb(DL_UNITDATA_IND_SIZE + 14, BPRI_LO)) == NULL) {
		freeb(dp);
		return NULL;
	}

	bp->b_datap->db_type = M_PROTO;
	ud = (dl_unitdata_ind_t *) bp->b_rptr;
	ud->dl_primitive = DL_UNITDATA_IND;
	ud->dl_dest_addr_length = 7;
	ud->dl_dest_addr_offset = DL_UNITDATA_IND_SIZE + 7;
	ud->dl_src_addr_length = 7;
	ud->dl_src_addr_offset = DL_UNITDATA_IND_SIZE;
	ud->dl_group_address = *dp->b_rptr & 1;
	bp->b_wptr += DL_UNITDATA_IND_SIZE;

	memcpy(bp->b_wptr, dp->b_rptr + 6, 6);
	bp->b_wptr += 6;
	*bp->b_wptr++ = *(dp->b_rptr + 15);

	memcpy(bp->b_wptr, dp->b_rptr, 6);
	bp->b_wptr += 6;
	*bp->b_wptr++ = *(dp->b_rptr + 14);

	len = ntohs(*(unsigned short *) (dp->b_rptr + 12)) - 3;
	dp->b_rptr += 17;
	if (dp->b_wptr - dp->b_rptr > len)
		dp->b_wptr = dp->b_rptr + len;

	linkb(bp, dp);
	return bp;
}

STATIC int
eth_8022_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	unsigned char *hdr;
	unsigned char dsap;

	dsap = *(dst + 6);
	if (dsap != dl->sap->sap.sap[0]) {
		struct sap *sap;

		for (sap = dl->subs; sap; sap = sap->next_sap)
			if (dsap == sap->sap.sap[0])
				break;
		if (sap == NULL)
			return 0;
	}

	hdr = skb_push(skb, 17);
	memcpy(hdr, dst, 6);
	memcpy(hdr + 6, dl->ndev->dev->dev_addr, 6);
	*(unsigned short *) (hdr + 12) = htons(datalen + 3);
	*(hdr + 14) = *(hdr + 15) = dsap;
	*(hdr + 16) = 3;
	return 1;
}

/*
 *  Ethernet RAW 802.2
 */
STATIC int
eth_raw8022_want(struct dl *dl, unsigned char *fr, int len)
{
	ASSERT(dl->sap_len == 1);

	if (len < 17 || ntohs(*(unsigned short *) (fr + 12)) < 3)
		return 0;
	if (*(unsigned short *) (fr + 14) == 0xffff)
		return 0;	/* "raw" 802.3 IPX */
	if (dl->flags & LDLFLAG_PROMISC_SAP)
		return 1;
	if (*(fr + 14) != dl->sap->sap.sap[0]) {
		struct sap *sap = dl->subs;
		while (sap != NULL) {
			if (*(fr + 14) == sap->sap.sap[0])
				break;
			sap = sap->next_sap;
		}
		if (sap == NULL)
			return 0;
	}
	if (memcmp(fr, dl->ndev->dev->dev_addr, 6) && memcmp(fr, dl->ndev->dev->broadcast, 6))
		return 0;
	return 1;
}

STATIC mblk_t *
eth_raw8022_rcvind(struct dl *dl, mblk_t *dp)
{
	mblk_t *bp;
	dl_unitdata_ind_t *ud;
	unsigned short len;

	ASSERT(dl->sap_len == 1);
	ASSERT(dl->addr_len == 6);

	if ((bp = allocb(DL_UNITDATA_IND_SIZE + 14, BPRI_LO)) == NULL) {
		freeb(dp);
		return NULL;
	}

	bp->b_datap->db_type = M_PROTO;
	ud = (dl_unitdata_ind_t *) bp->b_rptr;
	ud->dl_primitive = DL_UNITDATA_IND;
	ud->dl_dest_addr_length = 7;
	ud->dl_dest_addr_offset = DL_UNITDATA_IND_SIZE + 7;
	ud->dl_src_addr_length = 7;
	ud->dl_src_addr_offset = DL_UNITDATA_IND_SIZE;
	ud->dl_group_address = *dp->b_rptr & 1;
	bp->b_wptr += DL_UNITDATA_IND_SIZE;

	memcpy(bp->b_wptr, dp->b_rptr + 6, 6);
	bp->b_wptr += 6;
	*bp->b_wptr++ = *(dp->b_rptr + 15);

	memcpy(bp->b_wptr, dp->b_rptr, 6);
	bp->b_wptr += 6;
	*bp->b_wptr++ = *(dp->b_rptr + 14);

	len = ntohs(*(unsigned short *) (dp->b_rptr + 12));
	dp->b_rptr += 14;
	if (dp->b_wptr - dp->b_rptr > len)
		dp->b_wptr = dp->b_rptr + len;

	linkb(bp, dp);
	return bp;
}

STATIC int
eth_raw8022_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	unsigned char *hdr;

	hdr = skb_push(skb, 14);
	memcpy(hdr, dst, 6);
	memcpy(hdr + 6, dl->ndev->dev->dev_addr, 6);
	*(unsigned short *) (hdr + 12) = htons(datalen);
	*(hdr + 14) = *(dst + 6);
	*(hdr + 15) = dl->sap->sap.sap[0];
	return 1;
}

/*
 *  Ethernet SNAP
 */

STATIC int
eth_snap_want(struct dl *dl, unsigned char *fr, int len)
{
	ASSERT(dl->sap_len == 2);
	if (len < 22 || ntohs(*(unsigned short *) (fr + 12)) < 8
	    || *(unsigned short *) (fr + 14) != 0xAAAA || *(fr + 16) != 0x03
	    || memcmp(dl->oui, fr + 17, 3))
		return 0;

	if (memcmp(fr + 20, dl->sap->sap.sap, 2)) {
		struct sap *sap = dl->subs;

		while (sap != NULL) {
			if (!memcmp(fr + 20, sap->sap.sap, 2))
				break;
			sap = sap->next_sap;
		}
		if (sap == NULL)
			return 0;
	}

	if (memcmp(fr, dl->ndev->dev->dev_addr, 6) && memcmp(fr, dl->ndev->dev->broadcast, 6))
		return 0;

	return 1;
}

STATIC mblk_t *
eth_snap_rcvind(struct dl *dl, mblk_t *dp)
{
	mblk_t *bp;
	dl_unitdata_ind_t *ud;
	unsigned short len;
	unsigned short dsap = ntohs(*(unsigned short *) (dp->b_rptr + 17));

	ASSERT(dl->sap_len == 2);
	ASSERT(dl->addr_len == 6);

	if ((bp = allocb(DL_UNITDATA_IND_SIZE + 22, BPRI_LO)) == NULL) {
		freeb(dp);
		return NULL;
	}

	bp->b_datap->db_type = M_PROTO;
	ud = (dl_unitdata_ind_t *) bp->b_rptr;
	ud->dl_primitive = DL_UNITDATA_IND;
	ud->dl_dest_addr_length = 8;
	ud->dl_dest_addr_offset = DL_UNITDATA_IND_SIZE + 8;
	ud->dl_src_addr_length = 8;
	ud->dl_src_addr_offset = DL_UNITDATA_IND_SIZE;
	ud->dl_group_address = *dp->b_rptr & 1;
	bp->b_wptr += DL_UNITDATA_IND_SIZE;

	memcpy(bp->b_wptr, dp->b_rptr + 6, 6);
	bp->b_wptr += 6;
	*(unsigned short *) (bp->b_wptr) = dsap;
	bp->b_wptr += 2;

	memcpy(bp->b_wptr, dp->b_rptr, 6);
	bp->b_wptr += 6;
	*(unsigned short *) (bp->b_wptr) = dsap;
	bp->b_wptr += 2;

	len = ntohs(*(unsigned short *) (dp->b_rptr + 12)) - 8;
	dp->b_rptr += 22;
	if (dp->b_wptr - dp->b_rptr > len)
		dp->b_wptr = dp->b_rptr + len;

	linkb(bp, dp);
	return bp;
}

STATIC int
eth_snap_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	unsigned char *hdr;
	unsigned short dsap;

	dsap = *(unsigned short *) (dst + 6);
	if (dsap != *(unsigned short *) dl->sap->sap.sap) {
		struct sap *sap = dl->subs;

		while (sap != NULL) {
			if (dsap == *(unsigned short *) sap->sap.sap)
				break;
			sap = sap->next_sap;
		}
		if (sap == NULL)
			return 0;
	}

	hdr = skb_push(skb, 22);
	memcpy(hdr, dst, 6);
	memcpy(hdr + 6, dl->ndev->dev->dev_addr, 6);
	*(unsigned short *) (hdr + 12) = htons(datalen + 8);
	*(unsigned short *) (hdr + 14) = 0xaaaa;
	*(hdr + 16) = 3;
	memcpy(hdr + 17, dl->oui, 3);
	*(unsigned short *) (hdr + 20) = dsap;
	return 1;
}

/*
 *  Ethernet 802.3 IPX
 */

STATIC int
ipx_8023_want(struct dl *dl, unsigned char *fr, int len)
{
	ASSERT(dl->sap_len == 0);
	/* Must have a complete IPX header */
	if (len < 44 || ntohs(*(unsigned short *) (fr + 12)) < 30)
		return 0;
	/* Must have no IPX checksum */
	if (ntohs(*(unsigned short *) (fr + 14)) != 0xffff)
		return 0;
	if (memcmp(fr, dl->ndev->dev->dev_addr, 6) && memcmp(fr, dl->ndev->dev->broadcast, 6))
		return 0;
	return 1;
}

STATIC mblk_t *
ipx_8023_rcvind(struct dl *dl, mblk_t *dp)
{
	mblk_t *bp;
	dl_unitdata_ind_t *ud;
	unsigned short len;

	ASSERT(dl->sap_len == 0);
	ASSERT(dl->addr_len == 6);

	if ((bp = allocb(DL_UNITDATA_IND_SIZE + 12, BPRI_LO)) == NULL) {
		freeb(dp);
		return NULL;
	}

	bp->b_datap->db_type = M_PROTO;
	ud = (dl_unitdata_ind_t *) bp->b_rptr;
	ud->dl_primitive = DL_UNITDATA_IND;
	ud->dl_dest_addr_length = 6;
	ud->dl_dest_addr_offset = DL_UNITDATA_IND_SIZE;
	ud->dl_src_addr_length = 6;
	ud->dl_src_addr_offset = DL_UNITDATA_IND_SIZE + 6;
	ud->dl_group_address = *dp->b_rptr & 1;
	bp->b_wptr += DL_UNITDATA_IND_SIZE;

	memcpy(bp->b_wptr, dp->b_rptr, 12);
	bp->b_wptr += 12;

	len = *(unsigned short *) (dp->b_rptr + 12);
	dp->b_rptr += 14;
	if (dp->b_wptr - dp->b_rptr > len)
		dp->b_wptr = dp->b_rptr + len;

	linkb(bp, dp);
	return bp;
}

STATIC int
ipx_8023_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	unsigned char *hdr;

	hdr = skb_push(skb, 14);
	memcpy(hdr, dst, 6);
	memcpy(hdr + 6, dl->ndev->dev->dev_addr, 6);
	*(unsigned short *) (hdr + 12) = htons(datalen);
	return 1;
}

/*
 * Token Ring 802.2 LLC
 *
 * The bound sap must match the dsap of the received frame but the
 * control field can be anything at all.  LLC uses _real_ control
 * fields for link layer protocol.
 *
 * For received data the entire header is returned even though we
 * extract the addresses into the unitdata indication header.
 *
 * For transmitted data the caller has built the header by the
 * time we get the frame.  We transmit the frame as-is.
 */

/*
 * Token ring header.
 *
 * Note that there is no length field as there is in an Ethernet
 * header.  IP packet types have to be encoded inside a SNAP header
 * beyond the LLC header.
 */
typedef struct tr_hdr {
	u8 acf;
	u8 fcf;
	u8 dst_addr[6];
	u8 src_addr[6];
	/* 
	 * Routing control bytes: Only present if the routing bit
	 * is set in the first byte of the src_addr.
	 */
	u8 bl;				/* brdcst & lgth */
	u8 df;				/* direction & lrgst frm bits */

} tr_hdr_t;

/*
 * After a variable amount of routing data you come to the
 * LLC header.
 */
typedef struct tr_llc_frm_hdr {
	u8 llc_dsap;			/* destination SAP address */
	u8 llc_ssap;			/* source SAP address */
	u8 llc_ctl[1];			/* control field */

} tr_llc_frm_hdr_t;

/* 
 * A few bits from the token ring header.
 */
#define SADDR_0_RTE_PRES        0x80	/* 1=rte info present, 0=none */
#define RCF_0_LLLLL             0x1F	/* length bits */
#define FCF_FF          	0xC0	/* frame type bits */
#define     FCF_MAC     	0x00	/* MAC frame */
#define     FCF_LLC     	0x40	/* LLC frame */

STATIC int
tr_8022_want(struct dl *dl, unsigned char *fr, int len)
{				/* patterned after eth_8022_want */
	tr_hdr_t *trp;
	tr_llc_frm_hdr_t *llcp;
	int rtelgth;

#if 0
	{
		int i;
		printk("tr_8022_want:\n");
		for (i = 0; i < 16; i++)
			printk("%02x ", fr[i]);
		printk("\n");
		for (; i < 32; i++)
			printk("%02x ", fr[i]);
		printk("\n");
	}
#endif

	trp = (tr_hdr_t *) fr;
	ASSERT(dl->sap_len == 1);
	if (len < sizeof(tr_hdr_t) - 2 + sizeof(tr_llc_frm_hdr_t))
		return 0;
	/* 
	 * Decode the type of frame.  If a MAC frame, we don't want it.
	 * If an LLC frame, we will look at it.
	 */
	if ((trp->fcf & FCF_FF) != FCF_LLC)	/* must be LLC type frame */
		return (0);	/* we don't want it */

	/* 
	 * Find out how much routing info is present and skip over it.
	 * Establish llcp pointing to the LLC header.
	 *
	 * The length in the RCF includes the two bytes of routing control
	 * at the front of the RCF.  These two bytes are included in our
	 * header structure.
	 */
	if (trp->src_addr[0] & SADDR_0_RTE_PRES) {	/* has source route field */
		rtelgth = trp->bl & RCF_0_LLLLL;
		if (rtelgth < 2)	/* count includes rte ctl fld */
			return 0;	/* ill-formed, don't want it */

		if (rtelgth & 0x01)	/* must be pairs of bytes */
			return 0;	/* ill-formed, don't want it */

		llcp = (tr_llc_frm_hdr_t *) (fr + sizeof(*trp) - 2 + rtelgth);
	} else {		/* no source route field */
		/* don't skip as many bytes */
		llcp = (tr_llc_frm_hdr_t *) (fr + sizeof(*trp) - 2);
	}

	/* 
	 * Now that we know where the SAPs are and that it's an LLC frame
	 * we can search for a dlsap match.
	 *
	 * The LDLFLAG_RAW check will disappear when we
	 * get the RAW LLC frametype to work.
	 * It means that when running in RAW mode, any value of the
	 * control field will be accepted.
	 */
	if (!(dl->flags & LDLFLAG_RAW) && llcp->llc_ctl[0] != 0x03)
		return 0;

	if (dl->flags & LDLFLAG_PROMISC_SAP)
		return 1;	/* wants everything */

	if (llcp->llc_dsap != dl->sap->sap.sap[0]) {
		struct sap *sap = dl->subs;
		while (sap != NULL) {
			if (llcp->llc_dsap == sap->sap.sap[0])
				break;
			sap = sap->next_sap;
		}
		if (sap == NULL)
			return 0;
	}

	return 1;
}

/*
 * Strip the token ring addresses from the frame.  Also strip out the
 * route information and the LLC header.
 */
STATIC mblk_t *
tr_8022_rcvind(struct dl *dl, mblk_t *dp)
{
	mblk_t *bp;
	dl_unitdata_ind_t *ud;
	tr_hdr_t *trp;
	tr_llc_frm_hdr_t *llcp;
	int rtelgth = 0;

	ASSERT(dl->sap_len == 1);
	ASSERT(dl->addr_len == 6);

	trp = (tr_hdr_t *) dp->b_rptr;
	if (trp->src_addr[0] & SADDR_0_RTE_PRES) {	/* has source route field */
		rtelgth = trp->bl & RCF_0_LLLLL;
	}

	llcp = (tr_llc_frm_hdr_t *) (dp->b_rptr + sizeof(*trp) - 2 + rtelgth);

	if ((bp = allocb(DL_UNITDATA_IND_SIZE + 14, BPRI_LO)) == NULL) {
		freeb(dp);
		return NULL;
	}

	bp->b_datap->db_type = M_PROTO;
	ud = (dl_unitdata_ind_t *) bp->b_rptr;
	ud->dl_primitive = DL_UNITDATA_IND;
	ud->dl_dest_addr_length = 7;
	ud->dl_dest_addr_offset = DL_UNITDATA_IND_SIZE + 7;
	ud->dl_src_addr_length = 7;
	ud->dl_src_addr_offset = DL_UNITDATA_IND_SIZE;
	ud->dl_group_address = 0;
	bp->b_wptr += DL_UNITDATA_IND_SIZE;

	memcpy(bp->b_wptr, trp->dst_addr, 6);
	bp->b_wptr += 6;
	*bp->b_wptr++ = llcp->llc_dsap;
	memcpy(bp->b_wptr + 6, trp->src_addr, 6);
	bp->b_wptr += 6;
	*bp->b_wptr++ = llcp->llc_ssap;

	dp->b_rptr += sizeof(*trp) - 2 + rtelgth + sizeof(tr_llc_frm_hdr_t);

	linkb(bp, dp);
	return bp;
}

/*
 * Data buffer contains the data part of an LLC frame.  Add the LLC
 * header and the token ring MAC header.
 */
STATIC int
tr_8022_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	tr_hdr_t *trp;
	tr_llc_frm_hdr_t *llcp;
	unsigned char dsap;

	dsap = *(dst + 6);
	if (dsap != dl->sap->sap.sap[0]) {
		struct sap *sap;

		for (sap = dl->subs; sap; sap = sap->next_sap)
			if (dsap == sap->sap.sap[0])
				break;
		if (sap == NULL)
			return 0;
	}

	/* 
	 * Omit the router control field and turn off the source route
	 * bit in the source address.  This may or may not prove to
	 * be correct.
	 */
	trp = (tr_hdr_t *) skb_push(skb, sizeof(tr_hdr_t) - 2 + sizeof(*llcp));

	trp->acf = 0x10;	/* canned */
	trp->fcf = 0x40;	/* canned */
	memcpy(trp->dst_addr, dst, 6);
	memcpy(trp->src_addr, dl->ndev->dev->dev_addr, 6);
	trp->src_addr[0] &= ~SADDR_0_RTE_PRES;	/* no route */

	llcp = (tr_llc_frm_hdr_t *) (((char *) (trp + 1)) - 2);
	llcp->llc_dsap = dsap;
	llcp->llc_ssap = dsap;
	llcp->llc_ctl[0] = 0x03;

	return 1;
}

/*
 *  HDLC Raw
 */
STATIC int
hdlc_raw_want(struct dl *dl, unsigned char *fr, int len)
{
	return 1;		/* take everything */
}

STATIC mblk_t *
hdlc_raw_rcvind(struct dl *dl, mblk_t *dp)
{
	mblk_t *bp;
	dl_unitdata_ind_t *ud;

	if ((bp = allocb(DL_UNITDATA_IND_SIZE + 14, BPRI_LO)) == NULL) {
		freeb(dp);
		return NULL;
	}

	bp->b_datap->db_type = M_PROTO;
	ud = (dl_unitdata_ind_t *) bp->b_rptr;
	ud->dl_primitive = DL_UNITDATA_IND;
	ud->dl_dest_addr_length = 0;
	ud->dl_dest_addr_offset = 0;
	ud->dl_src_addr_length = 0;
	ud->dl_src_addr_offset = 0;
	ud->dl_group_address = 0;
	bp->b_wptr += DL_UNITDATA_IND_SIZE;

	linkb(bp, dp);
	return bp;
}

STATIC int
hdlc_raw_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	return 1;
}

/*
 *  FDDI 802.2
 */

STATIC int
fddi_8022_want(struct dl *dl, unsigned char *fr, int len)
{
	ASSERT(0);
	return 0;
}

STATIC mblk_t *
fddi_8022_rcvind(struct dl *dl, mblk_t *dp)
{
	ASSERT(0);
	return NULL;
}

STATIC int
fddi_8022_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	ASSERT(0);
	return 0;
}

/*
 *  FDDI SNAP
 */

STATIC int
fddi_snap_want(struct dl *dl, unsigned char *fr, int len)
{
	ASSERT(0);
	return 0;
}

STATIC mblk_t *
fddi_snap_rcvind(struct dl *dl, mblk_t *dp)
{
	ASSERT(0);
	return NULL;
}

STATIC int
fddi_snap_mkhdr(struct dl *dl, unsigned char *dst, int datalen, struct sk_buff *skb)
{
	ASSERT(0);
	return 0;
}

/****************************************************************************/
/*  End of: Frame type support routines.                                    */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*  Receive side Linux socket driver interface routines.                    */
/*                                                                          */
/****************************************************************************/

STATIC INLINE void
dl_rcv_put(mblk_t *dp, struct dl *dl, int copy)
{
	mblk_t *mp;

	if (dp == NULL) {
		if (copy == 0)
			freeb(dp);
		++dl->lost_rcv;
		return;
	}

	if (copy != 0 && (dp = dupb(dp)) == NULL) {
		++dl->lost_rcv;
		ginc(net_rx_drp_cnt);
		return;
	}

	ginc(net_rx_cnt);
	if ((dl->flags & LDLFLAG_RAW) == 0) {
		if ((mp = dl->rcvind(dl, dp)) == NULL) {
			freeb(dp);
			++dl->lost_rcv;
			return;
		}
	} else
		mp = dp;
#if 1
	/* 
	 * Insert in our read queue and send upstream from the read
	 * service routine.  Do not do putnexts from interrupt level.
	 */
	if (canput(dl->rq)) {
		if (!putq(dl->rq, mp))
			freemsg(mp);
		ginc(unitdata_q_cnt);
	} else {
		freemsg(mp);
		ginc(unitdata_drp_cnt);
		++dl->lost_rcv;
	}
#else
	if (qsize(dl->rq) == 0 && canputnext(dl->rq)) {
		if (ldl_debug_mask & LDL_DEBUG_UDIND)
			ldl_mp_data_dump("ldl_unitdata_ind", mp,
					 ldl_debug_mask & LDL_DEBUG_ALLDATA);
		putnext(dl->rq, mp);
		ginc(unitdata_ind_cnt);
	} else if (canput(dl->rq)) {
		if (!putq(dl->rq, mp))
			freemsg(mp);
		ginc(unitdata_q_cnt);
	} else {
		freemsg(mp);
		ginc(unitdata_drp_cnt);
		++dl->lost_rcv;
	}
#endif
}

void
mblk_destructor(char *arg)
{
	kfree_skb((struct sk_buff *) arg);
}

/*
 *  Our "interrupt" function
 *
 *  This is called from the Linux networking code whenever
 *  a packet comes in from the device.
 *
 *  It is not a real interrupt function as the driver has
 *  returned from the hardware interrupt when this is called.
 *
 *  For speed, we should avoid queueing the packet on our read
 *  queue AND use dupmsg() if the packet is to be sent upstream
 *  on more than one interface. But we cannot just putnext() our
 *  message and dupmsg() it later (upstreams modules may have
 *  changed or deallocated the message in the meantime).
 *  To get around this problem we defer the putnext() until
 *  we have done our first dupmsg() or we know that no dupmsg()
 *  is needed.
 *  But we can only avoid queueing if the read queue is empty;
 *  otherwise our M_PROTO message could go upstream before any
 *  M_PCPROTO messages queued on our read queue.
 */
STATIC int
rcv_func(struct sk_buff *skb, struct ldldev *dev, struct packet_type *pt)
{
	mblk_t *dp;
	struct dl *dl, *last = NULL;
	struct sap *sap;
#if 0
	struct ethhdr *hdr = (struct ethhdr *) skb->mac.raw;
#endif
	unsigned char *fr_ptr, fr_buf[LDL_MAX_HDR_LEN];
	int fr_len;
	struct free_rtn mblk_rtn;

	ASSERT(dev->type == ARPHRD_ETHER || dev->type == ARPHRD_LOOPBACK
	       || dev->type == ARPHRD_IEEE802 || IS_ARPHRD_IEEE802_TR(dev)
	       || dev->type == ARPHRD_HDLC);
	/* ARPHRD_FDDI */

	if (skb_is_nonlinear(skb)) {
		struct sk_buff *b;
		/* FIXME untested code */
		printk("ldl: non linear skb");
		b = skb_clone(skb, GFP_ATOMIC);
		kfree_skb(skb);
		if (b == NULL)
			return 0;
		if (skb_linearize(b, GFP_ATOMIC)) {
			kfree_skb(b);
			return 0;
		}
		skb = b;
	}
#if 1
	mblk_rtn.free_func = mblk_destructor;
	mblk_rtn.free_arg = (char *) skb;
	fr_len = skb->tail - skb->mac.raw;
	if ((dp = esballoc(skb->mac.raw - 2, fr_len + 2, BPRI_LO, &mblk_rtn)) != NULL) {
		dp->b_rptr = dp->b_wptr += 2;
		fr_ptr = dp->b_rptr;
		dp->b_wptr += fr_len;
		skb_get(skb);
	} else {		/* We still need the frame type for correct drop stats */
		fr_ptr = &fr_buf[0];
		fr_len = min(skb->end - skb->mac.raw, LDL_MAX_HDR_LEN);
		ASSERT(fr_len > 0);
		memcpy(fr_buf, skb->mac.raw, fr_len);
	}
#else
	if ((dp = allocb(2 + fr_len, BPRI_LO)) != NULL) {
		/* 
		 *  Prepare the data block.
		 *  The header is removed later for those that do no want it.
		 *  The two extra unused bytes makes the data more processor
		 *  cache-efficient in the higher level protocols (at least for
		 *  ethernet).
		 */
		dp->b_rptr = dp->b_wptr += 2;
		dp->b_datap->db_type = M_DATA;
		memcpy(dp->b_wptr, skb->mac.raw, fr_len);
		fr_ptr = dp->b_rptr;
		dp->b_wptr += fr_len;
	} else {		/* We still need the frame type for correct drop stats */
		fr_ptr = &fr_buf[0];
		fr_len = min(skb->end - skb->mac.raw, LDL_MAX_HDR_LEN);
		ASSERT(fr_len > 0);
		memcpy(fr_buf, skb->mac.raw, fr_len);
	}
#endif

	if (ldl_debug_mask & LDL_DEBUG_RCV_FUNC)
		ldl_skbuff_dump("ldl_rcv_func: skb", skb, ldl_debug_mask & LDL_DEBUG_ALLDATA);
#ifdef KERNEL_2_1
	dev_kfree_skb(skb);
#else
	dev_kfree_skb(skb, FREE_WRITE);
#endif

	if (ldl_debug_mask & LDL_DEBUG_RCV_FUNC)
		ldl_bfr_dump("ldl_rcv_func", fr_ptr, fr_len, ldl_debug_mask & LDL_DEBUG_ALLDATA);

	read_lock(&((struct pt *) pt)->lock);
	ASSERT(((struct pt *) pt)->magic == PT_MAGIC);

	for (sap = ((struct pt *) pt)->listen; sap != NULL; sap = sap->next_listen) {
		dl = sap->dl;
		ASSERT(dl != NULL);
		ASSERT(dl->magic == DL_MAGIC);
		ASSERT(dl->rq != NULL);
		ASSERT(dl->ndev->dev == dev);
		if (dl->wantsframe(dl, fr_ptr, fr_len)) {
			if (last != NULL)
				dl_rcv_put(dp, last, 1);
			last = dl;
		}
	}

	if (last != NULL) {
		/* Deliver to the last listener */
		dl_rcv_put(dp, last, 0);
	} else {
		/* Nobody wanted this */
		if (dp != NULL)
			freeb(dp);
	}
	read_unlock(&((struct pt *) pt)->lock);

	return 0;
}

/****************************************************************************/
/*                                                                          */
/*  Transmit side Linux socket driver interface routines.                   */
/*                                                                          */
/****************************************************************************/

/*
 *  Try to send an unitdata error upstream
 */
STATIC void
send_uderror(struct dl *dl, char *addr, int addrlen, dl_ulong err, dl_ulong uerr)
{
	mblk_t *mp;
	dl_uderror_ind_t *nakp;

	if ((mp = allocb(DL_UDERROR_IND_SIZE + addrlen, BPRI_HI)) == NULL)
		return;
	mp->b_datap->db_type = M_PCPROTO;
	nakp = (dl_uderror_ind_t *) mp->b_wptr;
	nakp->dl_primitive = DL_UDERROR_IND;
	if (addrlen) {
		nakp->dl_dest_addr_length = addrlen;
		nakp->dl_dest_addr_offset = DL_UDERROR_IND_SIZE;
		memcpy(mp->b_rptr + DL_UDERROR_IND_SIZE, addr, addrlen);
	} else {
		nakp->dl_dest_addr_length = 0;
		nakp->dl_dest_addr_offset = 0;
	}
	nakp->dl_unix_errno = uerr;
	nakp->dl_errno = err;
	mp->b_wptr += DL_UDERROR_IND_SIZE + addrlen;

	putnext(dl->rq, mp);
	ginc(uderror_ind_cnt);
}

/*
 *  Internal transmission failure error codes
 */
#define TXE_OUTSTATE	1
#define TXE_BADADDR	2
#define TXE_BADMTU	3
#define TXE_NOMEM	4
#define	TXE_NULL	5
#define	TXE_BADPRIO	6

STATIC int
tx_failed(struct dl *dl, mblk_t *mp, int err)
{
	dl_unitdata_req_t *reqp = (dl_unitdata_req_t *) mp->b_rptr;
	static dl_unitdata_req_t dummy;	/* all zeros */

	ginc(net_tx_fail_cnt);
	if (ldl_debug_mask & LDL_DEBUG_TX) {
		ldl_mp_dump("ldl: tx_failed", mp, ldl_debug_mask & LDL_DEBUG_ALLDATA);
		printk("ldl: tx_failed(%d)\n", err);
	}

	if (mp->b_datap->db_ref == 0) {	/* bug hunting */
		printk("ldl: message with ref-count zero\n");
		return DONE;
	}

	if (mp->b_datap->db_type == M_DATA)	/* not a unitdata req */
		reqp = &dummy;
	else
		ASSERT(reqp->dl_primitive == DL_UNITDATA_REQ);

	switch (err) {
	case TXE_OUTSTATE:
		send_uderror(dl, mp->b_rptr + reqp->dl_dest_addr_offset, reqp->dl_dest_addr_length,
			     DL_OUTSTATE, 0);
		break;
	case TXE_BADADDR:
		send_uderror(dl, mp->b_rptr + reqp->dl_dest_addr_offset, reqp->dl_dest_addr_length,
			     DL_BADADDR, 0);
		break;
	case TXE_BADMTU:
		send_uderror(dl, mp->b_rptr + reqp->dl_dest_addr_offset, reqp->dl_dest_addr_length,
			     DL_BADDATA, 0);
		break;
	case TXE_NOMEM:
	case TXE_NULL:
		send_uderror(dl, mp->b_rptr + reqp->dl_dest_addr_offset, reqp->dl_dest_addr_length,
			     DL_UNDELIVERABLE, 0);
		break;
	case TXE_BADPRIO:
		send_uderror(dl, mp->b_rptr + reqp->dl_dest_addr_offset, reqp->dl_dest_addr_length,
			     DL_UNSUPPORTED, 0);
		break;
	default:
		ASSERT(0);
	}

	freemsg(mp);
	return DONE;
}

/*
 *  Return transmission priority, or -1 in case of congestion, or -2 if invalid
 */
#ifndef KERNEL_2_1
STATIC INLINE int
tx_pri(struct dl *dl, dl_unitdata_req_t * reqp)
{
	int p_min, p_max;

	if (reqp != NULL) {
		if (reqp->dl_priority.dl_min == DL_QOS_DONT_CARE)
			p_min = 100;
		else
			p_min = reqp->dl_priority.dl_min;
		if (reqp->dl_priority.dl_max == DL_QOS_DONT_CARE)
			p_max = 0;
		else
			p_max = reqp->dl_priority.dl_max;
		if (p_min < 0 || p_max < 0 || p_min > 100 || p_max > 100)
			return -2;	/* invalid */

		if (p_max < dl->priority)
			p_max = dl->priority;
		if (p_max > p_min)
			p_max = p_min;

		p_min = pri_dlpi2netdevice(p_min);
		p_max = pri_dlpi2netdevice(p_max);
	} else {
		p_min = LDLPRI_LO;
		p_max = pri_dlpi2netdevice(dl->priority);
	}

	/* Use the highest priority that has free netdevice buffers */
	while (skb_queue_len(dl->ndev->dev->buffs + p_max) > dl->ndev->dev->tx_queue_len
	       && p_max <= p_min)
		++p_max;
	if (p_max > p_min) {
		pl_t psw;

		SPLSTR(psw);
		if (!dl->ndev->sleeping)
			ndev_wr_sleep(dl->ndev);
		SPLX(psw);
		return -1;
	}
	return p_max;
}
#else
STATIC INLINE int
tx_pri(struct dl *dl, dl_unitdata_req_t * reqp)
{
	int p_min, p_max;

	if (reqp != NULL) {
		if (reqp->dl_priority.dl_max == DL_QOS_DONT_CARE)
			p_max = dl->priority;
		else {
			p_max = reqp->dl_priority.dl_max;
			if (p_max < dl->priority)
				p_max = dl->priority;
		}
		if (reqp->dl_priority.dl_min != DL_QOS_DONT_CARE) {
			p_min = reqp->dl_priority.dl_min;
			if (p_max > p_min)
				p_max = p_min;
		}
	} else
		p_max = dl->priority;

	if (p_max < 0 || p_max > 100)
		return -2;	/* invalid */

	return pri_dlpi2netdevice(p_max);
}
#endif

STATIC INLINE int
tx_func_proto(struct dl *dl, mblk_t *mp)
{
	dl_unitdata_req_t *reqp;
	mblk_t *dmp;
	struct sk_buff *skb;
	int pri, dlen;

	ginc(unitdata_req_cnt);
	ASSERT(mp->b_datap->db_type == M_PROTO || mp->b_datap->db_type == M_PCPROTO);
	ASSERT(dl->magic == DL_MAGIC);

	if (ldl_debug_mask & LDL_DEBUG_UDREQ)
		ldl_mp_data_dump("ldl_unitdata_req", mp, ldl_debug_mask & LDL_DEBUG_ALLDATA);

	if (dl->dlstate != DL_IDLE)
		return tx_failed(dl, mp, TXE_OUTSTATE);

	reqp = (dl_unitdata_req_t *) mp->b_rptr;
	ASSERT(reqp->dl_primitive == DL_UNITDATA_REQ);

	if ((dl->flags & LDLFLAG_RAW) != 0) {	/* raw means don't chk addrs */
		int rslt;
		dmp = mp->b_cont;
		ASSERT(dmp != NULL);	/* needs data buffer */
		if (dmp == NULL)
			return tx_failed(dl, mp, TXE_NULL);
		rslt = tx_func_raw(dl, dmp);	/* send raw pkt */
		if (rslt == DONE) {
			unlinkb(mp);	/* unlink b_cont */
			freemsg(mp);	/* free unitdata header */
		}		/* else leave lnkd for retry */
		return rslt;
	}

	if (reqp->dl_dest_addr_length != dl->addr_len + dl->sap_len)
		return tx_failed(dl, mp, TXE_BADADDR);

	if ((dmp = mp->b_cont) != NULL) {
		ASSERT(dmp->b_datap->db_type == M_DATA);
		dlen = msgdsize(dmp);
		if (dlen > dl->mtu)
			return tx_failed(dl, mp, TXE_BADMTU);
	} else
		dlen = 0;

	if ((pri = tx_pri(dl, reqp)) < 0)
		return pri == -1 ? RETRY : tx_failed(dl, mp, TXE_BADPRIO);

	if ((skb = alloc_skb(dlen + dl->machdr_reserve, GFP_ATOMIC)) == NULL)
		return tx_failed(dl, mp, TXE_NOMEM);
#ifndef KERNEL_2_1
	skb->free = 1;
#endif

	ASSERT(dlen == 0 || dmp != NULL);

	/* 
	 *  Fill in the MAC header
	 */
	skb_reserve(skb, dl->machdr_reserve);
#ifdef KERNEL_2_1
	skb->nh.raw = skb->data;
#endif
	if (dl->mkhdr(dl, (char *) reqp + reqp->dl_dest_addr_offset, dlen, skb) != 1) {
#ifdef KERNEL_2_1
		kfree_skb(skb);
#else
		kfree_skb(skb, FREE_WRITE);
#endif
		return tx_failed(dl, mp, TXE_BADADDR);
	}

	/* 
	 *  Fill in the data portion
	 */
	while (dmp) {
		void *p;
		int dlen;

		ASSERT(dmp->b_datap->db_type == M_DATA);

		dlen = dmp->b_wptr - dmp->b_rptr;
		ASSERT(dlen > 0);
		p = skb_put(skb, dlen);
		ASSERT(p != NULL);
		memcpy(p, dmp->b_rptr, dlen);
		dmp = dmp->b_cont;
	}
	ASSERT(skb->len == dlen + dl->machdr_len);

	if (ldl_debug_mask & LDL_DEBUG_TX)
		ldl_bfr_dump("ldl_tx_func", skb->data, skb->len,
			     ldl_debug_mask & LDL_DEBUG_ALLDATA);
#ifdef KERNEL_2_1
	skb->priority = pri;	/* Set priority */
	if (ndev_xmit(dl->ndev, skb) == RETRY)
		return RETRY;
#else
	if (ndev_xmit(dl->ndev, skb, pri) == RETRY)
		return RETRY;
#if 0
	atomic_add(skb->truesize, &ndev->wr_cur);
	(struct ndev *) skb->sk = dl->ndev;
	skb->dev = dl->ndev->dev;	/* Set outgoing netdevice */
	skb->free = 1;		/* Free skb after transmission */
	skb->arp = 1;		/* To avoid header rebuild */
	skb->destructor = ndev_skb_destruct;	/* Set "done" callback */
	dev_queue_xmit(skb, dl->ndev->dev, pri);	/* Send it */
#endif
#endif
	freemsg(mp);
	ginc(net_tx_cnt);
	return DONE;
}

STATIC INLINE int
tx_func_raw(struct dl *dl, mblk_t *mp)
{
	mblk_t *omp = mp;
	struct sk_buff *skb;
	int pri, dlen;

	ASSERT(mp->b_datap->db_type == M_DATA);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT((dl->flags & LDLFLAG_RAW) != 0);

	if (dl->dlstate != DL_IDLE || !driver_started(dl->ndev->dev)
	    || netif_queue_stopped(dl->ndev->dev))
		return tx_failed(dl, mp, TXE_OUTSTATE);

	if ((pri = tx_pri(dl, NULL)) < 0)
		return pri == -1 ? RETRY : tx_failed(dl, mp, TXE_BADPRIO);

	dlen = msgdsize(mp);
	if (dlen > dl->mtu || dlen < dl->machdr_len)
		return tx_failed(dl, mp, TXE_BADMTU);

	if ((skb = alloc_skb(dlen, GFP_ATOMIC)) == NULL)
		return tx_failed(dl, mp, TXE_NOMEM);
#ifdef KERNEL_2_1
	skb->nh.raw = skb->data + dl->machdr_len;
#else
	skb->free = 1;
#endif

	while (mp) {
		void *p;
		int dlen;

		ASSERT(mp->b_datap->db_type == M_DATA);

		dlen = mp->b_wptr - mp->b_rptr;
		ASSERT(dlen > 0);
		p = skb_put(skb, dlen);
		ASSERT(p != NULL);
		memcpy(p, mp->b_rptr, dlen);
		mp = mp->b_cont;
	}
	ASSERT(skb->len == dlen);

	if (ldl_debug_mask & LDL_DEBUG_TX)
		ldl_bfr_dump("ldl_tx_func", skb->data, skb->len,
			     ldl_debug_mask & LDL_DEBUG_ALLDATA);
#ifdef KERNEL_2_1
	skb->priority = pri;
	if (ndev_xmit(dl->ndev, skb) == RETRY)
		return RETRY;
#else
	if (ndev_xmit(dl->ndev, skb, pri) == RETRY)
		return RETRY;
#endif
	freemsg(omp);		/* free original message */
	ginc(net_tx_cnt);

	return DONE;
}

/****************************************************************************/
/* End of: Linux socket driver interface routines.                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*  Write service routine helpers.                                          */
/*                                                                          */
/*  These are all inline functions used in the big switch                   */
/*                                                                          */
/****************************************************************************/

STATIC INLINE int
ws_info(struct dl *dl, mblk_t *mp)
{
	dl_info_ack_t *ackp;
	dl_qos_cl_sel1_t *qos;
	dl_qos_cl_range1_t *qos_range;
	int len;

	len = DL_INFO_ACK_SIZE + sizeof(dl_qos_cl_sel1_t) + sizeof(dl_qos_cl_range1_t);
	if (dl->dlstate != DL_UNATTACHED)
		len += dl->addr_len;	/* Broadcast address */
	if (dl->dlstate == DL_IDLE)
		len += dl->addr_len + dl->sap_len;	/* DLSAP */

	if (!reuse_msg(mp, len)) {
		mblk_t *bp;

		if ((bp = allocb(len, BPRI_HI)) == NULL)
			return dl_bufcall(dl, mp, DL_ERROR_ACK_SIZE);
		freemsg(mp);
		mp = bp;
	}

	mp->b_datap->db_type = M_PCPROTO;
	ackp = (dl_info_ack_t *) mp->b_wptr;
	ackp->dl_primitive = DL_INFO_ACK;
	ackp->dl_min_sdu = MINDATA;
	ackp->dl_reserved = 0;
	ackp->dl_current_state = dl->dlstate;
	ackp->dl_sap_length = (dl->dlstate == DL_UNATTACHED) ? -2 : -dl->sap_len;
	ackp->dl_service_mode = DL_CLDLS;
	ackp->dl_provider_style = DL_STYLE2;
	ackp->dl_version = DL_VERSION_2;
	ackp->dl_growth = 0;
	mp->b_wptr += DL_INFO_ACK_SIZE;

	qos = (dl_qos_cl_sel1_t *) mp->b_wptr;
	qos->dl_qos_type = DL_QOS_CL_SEL1;
	qos->dl_trans_delay = DL_UNKNOWN;
	qos->dl_priority = dl->priority;
	qos->dl_protection = DL_NONE;
	qos->dl_residual_error = DL_UNKNOWN;
	ackp->dl_qos_length = sizeof(dl_qos_cl_sel1_t);
	ackp->dl_qos_offset = (char *) qos - (char *) ackp;
	mp->b_wptr += sizeof(dl_qos_cl_sel1_t);

	qos_range = (dl_qos_cl_range1_t *) mp->b_wptr;
	qos_range->dl_qos_type = DL_QOS_CL_RANGE1;
	qos_range->dl_trans_delay.dl_target_value = DL_UNKNOWN;
	qos_range->dl_trans_delay.dl_accept_value = DL_UNKNOWN;
	qos_range->dl_priority.dl_min = 100;
	qos_range->dl_priority.dl_max = 0;
	qos_range->dl_protection.dl_min = DL_NONE;
	qos_range->dl_protection.dl_max = DL_NONE;
	qos_range->dl_residual_error = DL_UNKNOWN;
	ackp->dl_qos_range_length = sizeof(dl_qos_cl_range1_t);
	ackp->dl_qos_range_offset = (char *) qos_range - (char *) ackp;
	mp->b_wptr += sizeof(dl_qos_cl_range1_t);

	if (dl->dlstate != DL_UNATTACHED) {
		int ofs = DL_INFO_ACK_SIZE + sizeof(dl_qos_cl_sel1_t) + sizeof(dl_qos_cl_range1_t);

		ASSERT(dl->ndev != NULL);
		ASSERT(dl->ndev->dev != NULL);

		ackp->dl_sap_length = -dl->sap_len;
		ackp->dl_max_sdu = dl->ndev->dev->mtu;

		switch (dl->ndev->dev->type) {
		case ARPHRD_ETHER:
			ackp->dl_mac_type = DL_ETHER;
			break;
		case ARPHRD_LOOPBACK:
			ackp->dl_mac_type = DL_LOOP;
			break;
		case ARPHRD_FDDI:
			ackp->dl_mac_type = DL_FDDI;
			break;
		case ARPHRD_IEEE802:
#if defined(ARPHRD_IEEE802_TR)
		case ARPHRD_IEEE802_TR:
#endif
			ackp->dl_mac_type = DL_TPR;
			break;
		case ARPHRD_HDLC:
			ackp->dl_mac_type = DL_HDLC;
			break;
		default:
			ackp->dl_mac_type = DL_OTHER;
			break;
		}

		ackp->dl_brdcst_addr_offset = ofs;
		ackp->dl_brdcst_addr_length = dl->addr_len;
		memcpy(mp->b_wptr, dl->ndev->dev->broadcast, dl->addr_len);
		mp->b_wptr += dl->addr_len;

		if (dl->dlstate == DL_IDLE) {
			ASSERT(dl->sap != NULL);
			ofs += dl->addr_len;
			ackp->dl_addr_offset = ofs;
			ackp->dl_addr_length = dl->addr_len + dl->sap_len;
			memcpy(mp->b_wptr, dl->ndev->dev->dev_addr, dl->addr_len);
			mp->b_wptr += dl->addr_len;
			if (dl->framing == LDL_FRAME_EII || dl->framing == LDL_FRAME_SNAP)
				*(unsigned short *) mp->b_wptr =
				    ntohs(*(unsigned short *) &dl->sap->sap.sap[0]);
			else
				memcpy(mp->b_wptr, &dl->sap->sap.sap[0], dl->sap_len);
			mp->b_wptr += dl->sap_len;
			ackp->dl_max_sdu = dl->ndev->dev->mtu;
		} else {
			if ((dl->flags & LDLFLAG_PEDANTIC_STANDARD) != 0)
				ackp->dl_sap_length = 0;
			ackp->dl_addr_length = 0;
			ackp->dl_addr_offset = 0;
		}
	} else {
		ackp->dl_sap_length = 0;
		ackp->dl_mac_type = DL_OTHER;
		ackp->dl_max_sdu = MAXDATA;
		ackp->dl_addr_length = 0;
		ackp->dl_addr_offset = 0;
		ackp->dl_brdcst_addr_length = 0;
		ackp->dl_brdcst_addr_offset = 0;
	}

	putnext(dl->rq, mp);

	return DONE;
}

STATIC INLINE int
ws_phys_addr(struct dl *dl, mblk_t *mp)
{
	dl_phys_addr_req_t *reqp;
	dl_phys_addr_ack_t *ackp;
	int len;

	if (dl->dlstate == DL_UNATTACHED)
		return reply_error_ack(dl, mp, DL_PHYS_ADDR_REQ, DL_OUTSTATE, 0);

	reqp = (dl_phys_addr_req_t *) mp->b_rptr;
	switch (reqp->dl_addr_type) {
	case DL_CURR_PHYS_ADDR:
		len = DL_PHYS_ADDR_ACK_SIZE + dl->addr_len;
		if (!reuse_msg(mp, len)) {
			mblk_t *bp;

			if ((bp = allocb(len, BPRI_HI)) == NULL)
				return dl_bufcall(dl, mp, DL_ERROR_ACK_SIZE);
			freemsg(mp);
			mp = bp;
		}

		mp->b_datap->db_type = M_PCPROTO;
		ackp = (dl_phys_addr_ack_t *) mp->b_wptr;
		ackp->dl_primitive = DL_PHYS_ADDR_ACK;
		ackp->dl_addr_length = dl->addr_len;
		ackp->dl_addr_offset = DL_PHYS_ADDR_ACK_SIZE;
		mp->b_wptr += DL_PHYS_ADDR_ACK_SIZE;
		memcpy(mp->b_wptr, dl->ndev->dev->dev_addr, dl->addr_len);
		mp->b_wptr += dl->addr_len;
		putnext(dl->rq, mp);
		return DONE;

	case DL_FACT_PHYS_ADDR:
		/* 
		 *  I do not think that there is a general way of
		 *  getting the factory default address of a
		 *  native Linux netdevice.
		 */
	default:
		return reply_error_ack(dl, mp, DL_PHYS_ADDR_REQ, DL_NOTSUPPORTED, 0);
	}
}

STATIC INLINE int
ws_set_phys_addr(struct dl *dl, mblk_t *mp)
{
	dl_set_phys_addr_req_t *reqp;
	int err;
	mblk_t *ack_mp;
	pl_t psw;

	if (dl->dlstate == DL_UNATTACHED)
		return reply_error_ack(dl, mp, DL_SET_PHYS_ADDR_REQ, DL_OUTSTATE, 0);
	if (dl->ndev->dev->set_mac_address == NULL)
		return reply_error_ack(dl, mp, DL_SET_PHYS_ADDR_REQ, DL_NOTSUPPORTED, 0);

	reqp = (dl_set_phys_addr_req_t *) mp->b_rptr;

	if (reqp->dl_addr_length != dl->addr_len)
		return reply_error_ack(dl, mp, DL_SET_PHYS_ADDR_REQ, DL_BADADDR, 0);

	/* 
	 *  After we have begun playing with the device,
	 *  we do not want to run out of buffers for
	 *  our (ok or error) ack.
	 *  So we allocate a buffer for the ack now.
	 */
	ASSERT(DL_ERROR_ACK_SIZE >= DL_OK_ACK_SIZE);
	if ((ack_mp = allocb(DL_ERROR_ACK_SIZE, BPRI_HI)) == NULL)
		return dl_bufcall(dl, ack_mp, DL_ERROR_ACK_SIZE);

	SPLSTR(psw);
	dl->flags |= LDLFLAG_SET_ADDR;
	SPLX(psw);
	err = dev_close(dl->ndev->dev);
	SPLSTR(psw);
	dl->flags &= ~LDLFLAG_SET_ADDR;
	SPLX(psw);
	if (err) {
		freemsg(mp);
		make_dl_error_ack(ack_mp, DL_SET_PHYS_ADDR_REQ, DL_SYSERR, -err);
		putnext(dl->rq, ack_mp);
		ginc(error_ack_cnt);
		return DONE;
	}
	dl->ndev->dev->set_mac_address(dl->ndev->dev, mp->b_rptr + reqp->dl_addr_offset);
	freemsg(mp);
	err = dev_open(dl->ndev->dev);
	if (err) {
		/* Unable to re-open device */
		freemsg(ack_mp);
		SPLSTR(psw);
		hangup_set(dl);
		SPLX(psw);
		hangup_do(dl);
		return DONE;
	}
	ASSERT((dl->ndev->dev->flags & IFF_UP) != 0);
	ASSERT((dl->ndev->dev->flags & IFF_RUNNING) != 0);

	make_dl_ok_ack(ack_mp, DL_SET_PHYS_ADDR_REQ);
	putnext(dl->rq, ack_mp);
	ginc(ok_ack_cnt);

	return DONE;
}

STATIC INLINE int
ws_attach(struct dl *dl, mblk_t *mp)
{
	struct ndev *ndev;
	struct ldldev *dev;
	dl_attach_req_t *reqp;
	dl_ulong framing, ppa;
	pl_t psw;

	ginc(attach_req_cnt);
	if (dl->dlstate != DL_UNATTACHED)
		return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_OUTSTATE, 0);

	reqp = (dl_attach_req_t *) mp->b_rptr;

	ppa = reqp->dl_ppa & ~LDL_FRAME_MASK;
	framing = reqp->dl_ppa & LDL_FRAME_MASK;
	if (framing != LDL_FRAME_EII && framing != LDL_FRAME_802_2 && framing != LDL_FRAME_SNAP
	    && framing != LDL_FRAME_802_3 && framing != LDL_FRAME_RAW_LLC)
		return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);

	SPLSTR(psw);

	/* Find the device to attach to */
	ndev = ndev_get(ppa);
	if (ndev == NULL) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);
	}
	dev = ndev->dev;
	ASSERT(dev != NULL);

	switch (dev->type) {
	case ARPHRD_ETHER:
	case ARPHRD_LOOPBACK:
		if (framing != LDL_FRAME_EII && framing != LDL_FRAME_802_2
		    && framing != LDL_FRAME_SNAP && framing != LDL_FRAME_802_3
		    && framing != LDL_FRAME_RAW_LLC) {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);
		}
		break;
	case ARPHRD_FDDI:
		if (framing == 0)
			framing = LDL_FRAME_SNAP;	/* Default */
		if (framing != LDL_FRAME_802_2 && framing != LDL_FRAME_SNAP
		    && framing != LDL_FRAME_RAW_LLC) {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);
		}
		break;
	case ARPHRD_IEEE802:	/* claims to be for token ring */
#if defined(ARPHRD_IEEE802_TR)
	case ARPHRD_IEEE802_TR:
#endif
		if (framing != LDL_FRAME_802_2 && framing != LDL_FRAME_SNAP
		    && framing != LDL_FRAME_RAW_LLC) {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);
		}
		break;
	case ARPHRD_HDLC:
		if (framing != LDL_FRAME_RAW_LLC) {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);
		}
		break;
	default:
		printk("ldl: ws_attach: Unknown dev->type (%d)\n", (int) (dev->type));
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);
	}

	if (do_ok_ack(dl, &mp, DL_ATTACH_REQ) == RETRY) {
		SPLX(psw);
		return RETRY;
	} else if (mp == NULL) {
		SPLX(psw);
		return DONE;	/* Bogus, bufcall failed */
	}

	dl->dlstate = DL_UNBOUND;
	dl->addr_len = ETH_ALEN;
	dl->framing = framing;
	switch (dl->framing) {
	case LDL_FRAME_EII:
		dl->mtu = 1500;
		dl->machdr_len = 14;
		dl->machdr_reserve = 16;
		dl->sap_len = 2;
		dl->wantsframe = eth_ii_want;
		dl->rcvind = eth_ii_rcvind;
		dl->mkhdr = eth_ii_mkhdr;
		break;
	case LDL_FRAME_SNAP:
		dl->oui[0] = dl->oui[1] = dl->oui[2] = '\0';
		dl->sap_len = 2;
		if (dev->type == ARPHRD_FDDI) {
			dl->mtu = 4470;
			dl->machdr_len = 21;
			dl->machdr_reserve = 24;
			dl->wantsframe = fddi_snap_want;
			dl->rcvind = fddi_snap_rcvind;
			dl->mkhdr = fddi_snap_mkhdr;
		} else {
			dl->mtu = 1492;
			dl->machdr_len = 22;
			dl->machdr_reserve = 24;
			dl->wantsframe = eth_snap_want;
			dl->rcvind = eth_snap_rcvind;
			dl->mkhdr = eth_snap_mkhdr;
		}
		break;
	case LDL_FRAME_802_2:
		dl->sap_len = 1;
		switch (dev->type) {
		case ARPHRD_IEEE802:	/* token ring */
#if defined(ARPHRD_IEEE802_TR)
		case ARPHRD_IEEE802_TR:
#endif
		      token_ring_case:
			dl->sap_len = 1;
			dl->mtu = 1514;	/* matchs T.R. driver */
			dl->machdr_len = sizeof(tr_hdr_t) - 2 + sizeof(tr_llc_frm_hdr_t);
			dl->machdr_reserve = dl->machdr_len + 64;
			dl->wantsframe = tr_8022_want;
			dl->rcvind = tr_8022_rcvind;
			dl->mkhdr = tr_8022_mkhdr;
			break;
		case ARPHRD_FDDI:
			dl->mtu = 4475;
			dl->machdr_len = 16;
			dl->machdr_reserve = 24;
			dl->wantsframe = fddi_8022_want;
			dl->rcvind = fddi_8022_rcvind;
			dl->mkhdr = fddi_8022_mkhdr;
			break;
		case ARPHRD_ETHER:
		default:
			dl->mtu = 1497;
			dl->machdr_len = 17;
			dl->machdr_reserve = 24;
			dl->wantsframe = eth_8022_want;
			dl->rcvind = eth_8022_rcvind;
			dl->mkhdr = eth_8022_mkhdr;
			break;
		}
		break;
	case LDL_FRAME_RAW_LLC:
		dl->sap_len = 1;
		dl->flags |= LDLFLAG_RAW;	/* implicitly raw */
		switch (dev->type) {
		case ARPHRD_IEEE802:	/* token ring */
#if defined(ARPHRD_IEEE802_TR)
		case ARPHRD_IEEE802_TR:
#endif
			goto token_ring_case;

		case ARPHRD_FDDI:
			/* Not implemented */
			ASSERT(0);
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);
		case ARPHRD_HDLC:
			if (dev->mtu)
				dl->mtu = dev->mtu;
			else
				dl->mtu = 1500;
			dl->machdr_len = 0;
			dl->machdr_reserve = 0;
			dl->wantsframe = hdlc_raw_want;
			dl->rcvind = hdlc_raw_rcvind;
			dl->mkhdr = hdlc_raw_mkhdr;
			break;

		case ARPHRD_ETHER:
		default:
			dl->mtu = 1500;
			dl->machdr_len = 14;
			dl->machdr_reserve = 16;
			dl->wantsframe = eth_raw8022_want;
			dl->rcvind = eth_raw8022_rcvind;
			dl->mkhdr = eth_raw8022_mkhdr;
			break;
		}
		break;
	case LDL_FRAME_802_3:
		dl->mtu = 1500;
		dl->machdr_len = 14;
		dl->machdr_reserve = 16;
		dl->sap_len = 0;
		dl->wantsframe = ipx_8023_want;
		dl->rcvind = ipx_8023_rcvind;
		dl->mkhdr = ipx_8023_mkhdr;
		break;
	default:
		ASSERT(0);
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_ATTACH_REQ, DL_BADPPA, 0);
	}

	ndev_attach(ndev, dl);

	SPLX(psw);

	if (ldl_debug_mask & LDL_DEBUG_ATTACH)
		printk("ldl: ws_attach: " "ppa=%lx dev=\"%s\" framing=%s\n", ppa, dev->name,
		       ldl_framing_type(framing));

	putnext(dl->rq, mp);
	ginc(ok_ack_cnt);

	return DONE;
}

STATIC INLINE int
ws_detach(struct dl *dl, mblk_t *mp)
{
	pl_t psw;

	ginc(detach_req_cnt);
	if (dl->dlstate != DL_UNBOUND)
		return reply_error_ack(dl, mp, DL_DETACH_REQ, DL_OUTSTATE, 0);

	if (do_ok_ack(dl, &mp, DL_DETACH_REQ) == RETRY)
		return RETRY;
	else if (mp == NULL)
		return DONE;	/* Bogus, bufcall failed */

	SPLSTR(psw);
	ASSERT(dl->dlstate == DL_UNBOUND);
	dl->dlstate = DL_UNATTACHED;
	dl->addr_len = 0;
	ndev_release(dl);
	SPLX(psw);

	putnext(dl->rq, mp);
	ginc(ok_ack_cnt);

	return DONE;
}

STATIC INLINE int
ws_bind(struct dl *dl, mblk_t *mp)
{
	dl_bind_req_t *reqp;
	dl_bind_ack_t *ackp;
	sap_t dlsap;
	unsigned short saptype;
	pl_t psw;
	int len;

	ginc(bind_req_cnt);
	SPLSTR(psw);
	if (dl->dlstate != DL_UNBOUND) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_BIND_REQ, DL_OUTSTATE, 0);
	}

	reqp = (dl_bind_req_t *) mp->b_rptr;

	if (reqp->dl_service_mode != DL_CLDLS) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_BIND_REQ, DL_UNSUPPORTED, 0);
	}
	if (reqp->dl_xidtest_flg != 0) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_BIND_REQ, DL_NOAUTO, 0);
	}

	switch (dl->framing) {
	case LDL_FRAME_SNAP:
		if (reqp->dl_sap <= 0xffff) {
			*(unsigned short *) &dlsap.sap[0] = htons((unsigned short) reqp->dl_sap);
			/* saptype = htons(ETH_P_802_2); */
			saptype = ETH_P_802_2;
			break;
		} else {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_BIND_REQ, DL_BADADDR, 0);
		}
	case LDL_FRAME_EII:
		if (reqp->dl_sap <= 0xffff) {
			*(unsigned short *) &dlsap.sap[0] = htons((unsigned short) reqp->dl_sap);
			/* saptype = *(unsigned short *)&dlsap.sap[0]; */
			saptype = (unsigned short) reqp->dl_sap;
			break;
		} else {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_BIND_REQ, DL_BADADDR, 0);
		}
	case LDL_FRAME_802_2:
	case LDL_FRAME_RAW_LLC:
		if (reqp->dl_sap <= 0xff) {
			dlsap.sap[0] = reqp->dl_sap;
			saptype = ETH_P_802_2;
			break;
		} else {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_BIND_REQ, DL_BADADDR, 0);
		}
	case LDL_FRAME_802_3:
		if (reqp->dl_sap == 0) {
			/* saptype = htons(ETH_P_802_3); */
			saptype = ETH_P_802_3;
			break;
		} else {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_BIND_REQ, DL_BADADDR, 0);
		}
	default:
		ASSERT(0);
		return reply_error_ack(dl, mp, DL_BIND_REQ, DL_BADADDR, 0);
	}

	if (sap_create(dl, dlsap, saptype) < 0) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_BIND_REQ, DL_SYSERR, ENOMEM);
	}

	len = DL_BIND_ACK_SIZE + dl->addr_len + dl->sap_len;
	if (!reuse_msg(mp, len)) {
		mblk_t *bp;

		if ((bp = allocb(len, BPRI_HI)) == NULL) {
			SPLX(psw);
			return dl_bufcall(dl, mp, len);
		}
		freemsg(mp);
		mp = bp;
	}

	mp->b_datap->db_type = M_PCPROTO;
	ackp = (dl_bind_ack_t *) mp->b_wptr;
	ackp->dl_primitive = DL_BIND_ACK;
	ackp->dl_sap = reqp->dl_sap;
	ackp->dl_addr_length = dl->addr_len + dl->sap_len;
	ackp->dl_addr_offset = DL_BIND_ACK_SIZE;
	ackp->dl_max_conind = 0;
	ackp->dl_xidtest_flg = 0;
	mp->b_wptr += DL_BIND_ACK_SIZE;
	memcpy(mp->b_wptr, dl->ndev->dev->dev_addr, dl->addr_len);
	mp->b_wptr += dl->addr_len;
	if (dl->framing == LDL_FRAME_EII || dl->framing == LDL_FRAME_SNAP)
		*(unsigned short *) mp->b_wptr = ntohs(*(unsigned short *) &dlsap.sap[0]);
	else
		memcpy(mp->b_wptr, &dl->sap->sap.sap[0], dl->sap_len);
	mp->b_wptr += dl->sap_len;

	dl->dlstate = DL_IDLE;
	SPLX(psw);

	if (ldl_debug_mask & LDL_DEBUG_BIND)
		printk("ldl: ws_bind: " "dl_sap=%lx framing=%s pkt-type=%s\n", reqp->dl_sap,
		       ldl_framing_type(dl->framing), ldl_pkt_type(saptype));

	putnext(dl->rq, mp);

	return DONE;
}

STATIC INLINE int
ws_unbind(struct dl *dl, mblk_t *mp)
{
	pl_t psw;

	ginc(unbind_req_cnt);
	SPLSTR(psw);
	if (dl->dlstate != DL_IDLE) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_UNBIND_REQ, DL_OUTSTATE, 0);
	}

	if (do_ok_ack(dl, &mp, DL_UNBIND_REQ) == RETRY) {
		SPLX(psw);
		return RETRY;
	} else if (mp == NULL) {
		SPLX(psw);
		return DONE;	/* Bogus, bufcall failed */
	}

	/* Change state */
	dl->dlstate = DL_UNBIND_PENDING;
	SPLX(psw);

	sap_destroy_all(dl);

	if (putctl1(dl->rq->q_next, M_FLUSH, FLUSHRW) == 0)
		printk("ldl: cannot flush stream on unbind\n");

	/* Change state */
	SPLSTR(psw);
	ASSERT(dl->dlstate == DL_UNBIND_PENDING);
	dl->dlstate = DL_UNBOUND;
	if ((dl->flags & LDLFLAG_HANGUP) != 0) {
		SPLX(psw);
		freemsg(mp);
		hangup_do(dl);
		return DONE;
	}
	SPLX(psw);

	putnext(dl->rq, mp);
	ginc(ok_ack_cnt);

	return DONE;
}

STATIC INLINE int
ws_subs_bind(struct dl *dl, mblk_t *mp)
{
	dl_subs_bind_req_t *reqp;
	dl_subs_bind_ack_t *ackp;
	int len;
	pl_t psw;

	ginc(subs_bind_req_cnt);
	SPLSTR(psw);
	if (dl->dlstate != DL_IDLE) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_OUTSTATE, 0);
	}
	if (dl->framing == LDL_FRAME_802_3) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_UNSUPPORTED, 0);
	}

	reqp = (dl_subs_bind_req_t *) mp->b_rptr;

	if (reqp->dl_subs_bind_class == DL_HIERARCHICAL_BIND) {
		unsigned char oui[3];

		if (dl->framing != LDL_FRAME_SNAP) {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_UNSUPPORTED, 0);
		}
		if (reqp->dl_subs_sap_length != 3) {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_BADADDR, 0);
		}
		ASSERT(dl->sap_len == 2);
		memcpy(oui, mp->b_rptr + reqp->dl_subs_sap_offset, 3);

		len = DL_SUBS_BIND_ACK_SIZE + 3;
		if (!reuse_msg(mp, len)) {
			mblk_t *bp;

			if ((bp = allocb(len, BPRI_HI)) == NULL) {
				SPLX(psw);
				return dl_bufcall(dl, mp, len);
			}
			freemsg(mp);
			mp = bp;
		}

		/* change dl->oui */
		memcpy(dl->oui, oui, 3);

		SPLX(psw);

		mp->b_datap->db_type = M_PCPROTO;
		ackp = (dl_subs_bind_ack_t *) mp->b_wptr;
		ackp->dl_primitive = DL_SUBS_BIND_ACK;
		ackp->dl_subs_sap_length = 3;
		ackp->dl_subs_sap_offset = DL_SUBS_BIND_ACK_SIZE;
		mp->b_wptr += DL_SUBS_BIND_ACK_SIZE;
		memcpy(mp->b_wptr, dl->oui, 3);
		mp->b_wptr += 3;

		putnext(dl->rq, mp);
		return DONE;
	} else if (reqp->dl_subs_bind_class == DL_PEER_BIND) {
		sap_t dlsap;
		dl_ushort saptype;
		int saplen = reqp->dl_subs_sap_length;
		unsigned char *sapptr = mp->b_rptr + reqp->dl_subs_sap_offset;

		switch (dl->framing) {
		case LDL_FRAME_EII:
			if (saplen != 2) {
				SPLX(psw);
				return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_BADADDR, 0);
			}
			*(unsigned short *) &dlsap.sap[0] = htons(*(unsigned short *) sapptr);
			saptype = *(unsigned short *) sapptr;
			break;
		case LDL_FRAME_802_2:
		case LDL_FRAME_RAW_LLC:
			if (saplen != 1) {
				SPLX(psw);
				return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_BADADDR, 0);
			}
			dlsap.sap[0] = *sapptr;
			saptype = ETH_P_802_2;

			break;
		case LDL_FRAME_SNAP:
			if (saplen != 2) {
				SPLX(psw);
				return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_BADADDR, 0);
			}
			*(unsigned short *) &dlsap.sap[0] = htons(*(unsigned short *) sapptr);
			saptype = ETH_P_802_2;
			break;
		case LDL_FRAME_802_3:
		default:
			ASSERT(0);
			freemsg(mp);
			return DONE;
		}

		len = DL_SUBS_BIND_ACK_SIZE + saplen;
		if (!reuse_msg(mp, len)) {
			mblk_t *bp;

			if ((bp = allocb(len, BPRI_HI)) == NULL) {
				SPLX(psw);
				return dl_bufcall(dl, mp, len);
			}
			freemsg(mp);
			mp = bp;
		}

		if (sap_create(dl, dlsap, saptype) < 0) {
			SPLX(psw);
			return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_TOOMANY, 0);
		}
		SPLX(psw);

		mp->b_datap->db_type = M_PCPROTO;
		ackp = (dl_subs_bind_ack_t *) mp->b_wptr;
		ackp->dl_primitive = DL_SUBS_BIND_ACK;
		ackp->dl_subs_sap_length = saplen;
		ackp->dl_subs_sap_offset = DL_SUBS_BIND_ACK_SIZE;
		mp->b_wptr += DL_SUBS_BIND_ACK_SIZE;
		if (dl->framing == LDL_FRAME_EII || dl->framing == LDL_FRAME_SNAP)
			*(unsigned short *) mp->b_wptr = ntohs(*(unsigned short *) &dlsap.sap[0]);
		else
			memcpy(mp->b_wptr, dlsap.sap, saplen);
		mp->b_wptr += saplen;

		putnext(dl->rq, mp);
		return DONE;
	} else {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_SUBS_BIND_REQ, DL_UNSUPPORTED, 0);
	}
}

STATIC INLINE int
ws_subs_unbind(struct dl *dl, mblk_t *mp)
{
	dl_subs_unbind_req_t *reqp;
	int saplen;
	unsigned char *sapptr;
	struct sap *sap;
	sap_t dlsap;
	pl_t psw;

	ginc(subs_unbind_req_cnt);
	if (dl->dlstate != DL_IDLE)
		return reply_error_ack(dl, mp, DL_SUBS_UNBIND_REQ, DL_OUTSTATE, 0);

	if (dl->subs != NULL)
		return reply_error_ack(dl, mp, DL_SUBS_UNBIND_REQ, DL_BADADDR, 0);

	reqp = (dl_subs_unbind_req_t *) mp->b_rptr;
	saplen = reqp->dl_subs_sap_length;
	sapptr = mp->b_rptr + reqp->dl_subs_sap_offset;

	switch (dl->framing) {
	case LDL_FRAME_EII:
	case LDL_FRAME_SNAP:
		ASSERT(dl->sap_len == 2);
		if (saplen != 2)
			return reply_error_ack(dl, mp, DL_SUBS_UNBIND_REQ, DL_BADADDR, 0);
		*(unsigned short *) &dlsap.sap[0] = htons(*(unsigned short *) sapptr);
		break;
	case LDL_FRAME_802_2:
	case LDL_FRAME_RAW_LLC:
		ASSERT(dl->sap_len == 1);
		if (saplen != 1)
			return reply_error_ack(dl, mp, DL_SUBS_UNBIND_REQ, DL_BADADDR, 0);
		dlsap.sap[0] = *sapptr;
		break;
	case LDL_FRAME_802_3:
	default:
		return reply_error_ack(dl, mp, DL_SUBS_UNBIND_REQ, DL_BADADDR, 0);
	}

	for (sap = dl->subs; sap != NULL && memcmp(dlsap.sap, sap->sap.sap, dl->sap_len);
	     sap = sap->next_sap) ;

	if (sap == NULL)
		return reply_error_ack(dl, mp, DL_SUBS_UNBIND_REQ, DL_BADADDR, 0);

	ASSERT(!memcmp(dlsap.sap, sap->sap.sap, dl->sap_len));
	ASSERT(sap->magic == SAP_MAGIC);
	ASSERT(sap->dl == dl);

	/* Change state */
	SPLSTR(psw);
	if (dl->dlstate != DL_IDLE) {
		SPLX(psw);
		return reply_error_ack(dl, mp, DL_SUBS_UNBIND_REQ, DL_OUTSTATE, 0);
	}
	dl->dlstate = DL_SUBS_UNBIND_PND;
	SPLX(psw);

	if (do_ok_ack(dl, &mp, DL_SUBS_UNBIND_REQ) == RETRY)
		return RETRY;
	else if (mp == NULL)
		return DONE;	/* Bogus, bufcall failed */

	if (sap_destroy(dl, sap) < 0) {
		dl->dlstate = DL_IDLE;
		return reply_error_ack(dl, mp, DL_SUBS_UNBIND_REQ, DL_BADADDR, 0);
	}

	/* Change state */
	SPLSTR(psw);
	ASSERT(dl->dlstate == DL_SUBS_UNBIND_PND);
	dl->dlstate = DL_IDLE;
	if ((dl->flags & LDLFLAG_HANGUP) != 0) {
		hangup_set(dl);
		SPLX(psw);
		freemsg(mp);
		hangup_do(dl);
		return DONE;
	}
	SPLX(psw);

	putnext(dl->rq, mp);
	ginc(ok_ack_cnt);

	return DONE;
}

STATIC INLINE int
ws_udqos(struct dl *dl, mblk_t *mp)
{
	dl_udqos_req_t *reqp;
	dl_qos_cl_sel1_t *qos;
	long priority;

	ginc(udqos_req_cnt);
	if (dl->dlstate != DL_IDLE)
		return reply_error_ack(dl, mp, DL_UDQOS_REQ, DL_OUTSTATE, 0);

	reqp = (dl_udqos_req_t *) mp->b_rptr;
	if (reqp->dl_qos_length < sizeof(dl_qos_cl_sel1_t)
	    || reqp->dl_qos_offset < DL_UDQOS_REQ_SIZE)
		return reply_error_ack(dl, mp, DL_UDQOS_REQ, DL_BADQOSTYPE, 0);

	qos = (dl_qos_cl_sel1_t *) (mp->b_rptr + reqp->dl_qos_offset);
	if (qos->dl_qos_type != DL_QOS_CL_SEL1)
		return reply_error_ack(dl, mp, DL_UDQOS_REQ, DL_BADQOSTYPE, 0);
	if (qos->dl_trans_delay >= 0)
		return reply_error_ack(dl, mp, DL_UDQOS_REQ, DL_BADQOSPARAM, 0);
	if (qos->dl_protection >= 0 && qos->dl_protection != DL_NONE)
		return reply_error_ack(dl, mp, DL_UDQOS_REQ, DL_BADQOSPARAM, 0);
	if (qos->dl_residual_error >= 0)
		return reply_error_ack(dl, mp, DL_UDQOS_REQ, DL_BADQOSPARAM, 0);
	if (qos->dl_priority > 100)
		return reply_error_ack(dl, mp, DL_UDQOS_REQ, DL_BADQOSPARAM, 0);

	if (qos->dl_priority >= 0 && qos->dl_priority <= 100)
		priority = qos->dl_priority;
	else
		priority = dl->priority;

	if (do_ok_ack(dl, &mp, DL_UDQOS_REQ) == RETRY)
		return RETRY;
	else if (mp == NULL)
		return DONE;	/* Bogus, bufcall failed */

	dl->priority = priority;

	putnext(dl->rq, mp);
	ginc(ok_ack_cnt);

	return DONE;
}

STATIC INLINE int
ws_promiscon(struct dl *dl, mblk_t *mp)
{
	dl_promiscon_req_t *reqp;

	/* FIXME: DL_UNATTACHED state should be allowed */
	if (dl->dlstate != DL_UNBOUND && dl->dlstate != DL_IDLE)
		return reply_error_ack(dl, mp, DL_PROMISCON_REQ, DL_OUTSTATE, 0);

	/* FIXME: Other frametypes should be allowed */
	if (dl->framing != LDL_FRAME_802_2 && dl->framing != LDL_FRAME_RAW_LLC)
		return reply_error_ack(dl, mp, DL_PROMISCON_REQ, DL_NOTSUPPORTED, 0);

	reqp = (dl_promiscon_req_t *) mp->b_rptr;
	switch (reqp->dl_level) {
	case DL_PROMISC_SAP:
		if (do_ok_ack(dl, &mp, DL_PROMISCON_REQ) == RETRY)
			return RETRY;
		if (mp != NULL) {
			dl->flags |= LDLFLAG_PROMISC_SAP;
			putnext(dl->rq, mp);
			ginc(ok_ack_cnt);
		}
		return DONE;
	case DL_PROMISC_PHYS:
		/* TODO */
	case DL_PROMISC_MULTI:
		/* TODO */
	default:
		return reply_error_ack(dl, mp, DL_PROMISCON_REQ, DL_NOTSUPPORTED, 0);
	}
}

STATIC INLINE int
ws_promiscoff(struct dl *dl, mblk_t *mp)
{
	dl_promiscoff_req_t *reqp;

	/* FIXME: DL_UNATTACHED state should be allowed */
	if (dl->dlstate != DL_UNBOUND && dl->dlstate != DL_IDLE)
		return reply_error_ack(dl, mp, DL_PROMISCOFF_REQ, DL_OUTSTATE, 0);

	/* FIXME: Other frametypes should be allowed */
	if (dl->framing != LDL_FRAME_802_2 && dl->framing != LDL_FRAME_RAW_LLC)
		return reply_error_ack(dl, mp, DL_PROMISCOFF_REQ, DL_NOTSUPPORTED, 0);

	reqp = (dl_promiscoff_req_t *) mp->b_rptr;
	switch (reqp->dl_level) {
	case DL_PROMISC_SAP:
		if (do_ok_ack(dl, &mp, DL_PROMISCOFF_REQ) == RETRY)
			return RETRY;
		if (mp != NULL) {
			dl->flags &= ~LDLFLAG_PROMISC_SAP;
			putnext(dl->rq, mp);
			ginc(ok_ack_cnt);
		}
		return DONE;
	case DL_PROMISC_PHYS:
		/* TODO */
	case DL_PROMISC_MULTI:
		/* TODO */
	default:
		return reply_error_ack(dl, mp, DL_PROMISCOFF_REQ, DL_NOTSUPPORTED, 0);
	}
}

STATIC INLINE int
ws_enabmulti(struct dl *dl, mblk_t *mp)
{
	dl_enabmulti_req_t *reqp;

	if (dl->dlstate == DL_UNATTACHED)
		return reply_error_ack(dl, mp, DL_ENABMULTI_REQ, DL_OUTSTATE, 0);

	reqp = (dl_enabmulti_req_t *) mp->b_rptr;

	switch (dev_mc_add
		(dl->ndev->dev, mp->b_rptr + reqp->dl_addr_offset, reqp->dl_addr_length, 0)) {
	case 0:
		if (do_ok_ack(dl, &mp, DL_ENABMULTI_REQ) == RETRY)
			return RETRY;
		else if (mp == NULL)
			return DONE;	/* Bogus, bufcall failed */
		break;

	default:
		return reply_error_ack(dl, mp, DL_ENABMULTI_REQ, DL_NOTSUPPORTED, 0);
	}

	putnext(dl->rq, mp);
	ginc(ok_ack_cnt);
	return DONE;
}

STATIC INLINE int
ws_disabmulti(struct dl *dl, mblk_t *mp)
{
	dl_disabmulti_req_t *reqp;

	if (dl->dlstate == DL_UNATTACHED)
		return reply_error_ack(dl, mp, DL_DISABMULTI_REQ, DL_OUTSTATE, 0);

	reqp = (dl_disabmulti_req_t *) mp->b_rptr;
	return 0;
}

STATIC INLINE int
ws_error(struct dl *dl, mblk_t *mp, dl_ulong primitive, dl_ulong err)
{
	if (reply_error_ack(dl, mp, primitive, err, 0) == RETRY)
		return RETRY;

	return DONE;
}

/****************************************************************************/
/* End of: Write queue service routine helpers.                             */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*  IOCTL routine helpers.                                                  */
/*                                                                          */
/****************************************************************************/

STATIC INLINE int
ioc_nak(struct dl *dl, mblk_t *mp)
{
	mp->b_datap->db_type = M_IOCNAK;
	qreply(WR(dl->rq), mp);
	return DONE;
}

STATIC INLINE int
ioc_setflags(struct dl *dl, struct iocblk *iocp, mblk_t *mp)
{
	struct ldl_flags_ioctl *flg;
	mblk_t *dp;
	pl_t psw;

	ASSERT(iocp->ioc_cmd == LDL_SETFLAGS);

	if (mp->b_cont == NULL || mp->b_cont->b_datap->db_type != M_DATA)
		return ioc_nak(dl, mp);	/* No M_DATA block for flags */
	if (iocp->ioc_count != sizeof(struct ldl_flags_ioctl))
		return ioc_nak(dl, mp);	/* Bad data size */
	dp = mp->b_cont;
	ASSERT(dp->b_wptr - dp->b_rptr >= sizeof(struct ldl_flags_ioctl));
	flg = (struct ldl_flags_ioctl *) dp->b_rptr;
	flg->mask &= ~LDLFLAG_PRIVATE;	/* Cannot set private flags */
	SPLSTR(psw);
	dl->flags = (dl->flags & ~flg->mask) | (flg->flags & flg->mask);
	SPLX(psw);
	flg->flags = dl->flags;

	mp->b_datap->db_type = M_IOCACK;
	iocp->ioc_error = 0;
	iocp->ioc_rval = 0;

	qreply(WR(dl->rq), mp);
	return DONE;
}

STATIC INLINE int
ioc_findppa(struct dl *dl, struct iocblk *iocp, mblk_t *mp)
{
	struct ldldev *dev;
	dl_ulong ppa;
	mblk_t *dp;

	ASSERT(iocp->ioc_cmd == LDL_FINDPPA);

	if (mp->b_cont == NULL || mp->b_cont->b_datap->db_type != M_DATA)
		return ioc_nak(dl, mp);	/* No M_DATA block for name */

	if (iocp->ioc_count <= 0)
		return ioc_nak(dl, mp);	/* Empty name */

	dp = mp->b_cont;

	/* Find device with this name */
	for (dev = dev_base, ppa = 0; dev != NULL; dev = dev->next, ++ppa)
		if (!strcmp(dev->name, dp->b_rptr))
			break;
	if (dev == NULL)
		return ioc_nak(dl, mp);	/* No such device */

	dp->b_wptr = dp->b_rptr + sizeof(dl_ulong);
	*(dl_ulong *) dp->b_rptr = ppa;

	mp->b_datap->db_type = M_IOCACK;
	iocp->ioc_count = sizeof(dl_ulong);
	iocp->ioc_error = 0;
	iocp->ioc_rval = 0;

	qreply(WR(dl->rq), mp);
	return DONE;
}

STATIC INLINE int
ioc_getname(struct dl *dl, struct iocblk *iocp, mblk_t *mp)
{
	mblk_t *dp;
	int len;

	ASSERT(iocp->ioc_cmd == LDL_GETNAME);

	if (dl->dlstate == DL_UNATTACHED)
		return ioc_nak(dl, mp);	/* Not attached to a device */

	ASSERT(dl->ndev != NULL);
	ASSERT(dl->ndev->dev != NULL);

	len = strlen(dl->ndev->dev->name) + 1;
	if ((dp = allocb(len, BPRI_HI)) == NULL)
		return RETRY;
	memcpy(dp->b_rptr, dl->ndev->dev->name, len);
	dp->b_wptr += len;
	if (mp->b_cont != NULL)
		freemsg(unlinkb(mp));
	linkb(mp, dp);

	mp->b_datap->db_type = M_IOCACK;
	iocp->ioc_count = len;
	iocp->ioc_error = 0;
	iocp->ioc_rval = 0;

	qreply(WR(dl->rq), mp);
	return DONE;
}

STATIC INLINE int
ioc_getgstats(struct dl *dl, struct iocblk *iocp, mblk_t *mp)
{
	mblk_t *dp;

	ASSERT(iocp->ioc_cmd == LDL_GETGSTATS);
	if (iocp->ioc_count != sizeof(ldl_gstats))
		return ioc_nak(dl, mp);	/* wrong size struct */

	if ((dp = allocb(sizeof(ldl_gstats), BPRI_HI)) == NULL)
		return RETRY;

	memcpy(dp->b_rptr, &ldl_gstats, sizeof(ldl_gstats));
	dp->b_wptr += sizeof(ldl_gstats);
	if (mp->b_cont != NULL)
		freemsg(unlinkb(mp));
	linkb(mp, dp);

	mp->b_datap->db_type = M_IOCACK;
	iocp->ioc_count = sizeof(ldl_gstats);
	iocp->ioc_error = 0;
	iocp->ioc_rval = 0;

	qreply(WR(dl->rq), mp);
	return DONE;
}

STATIC INLINE int
ioc_set_debug_mask(struct dl *dl, struct iocblk *iocp, mblk_t *mp)
{
	mblk_t *dp;
	unsigned long *lp;

	ASSERT(iocp->ioc_cmd == LDL_SETDEBUG);

	if (mp->b_cont == NULL || mp->b_cont->b_datap->db_type != M_DATA)
		return ioc_nak(dl, mp);	/* No M_DATA block for flags */
	if (iocp->ioc_count != sizeof(ldl_debug_mask))
		return ioc_nak(dl, mp);	/* Bad data size */
	dp = mp->b_cont;
	ASSERT(dp->b_wptr - dp->b_rptr >= sizeof(ldl_debug_mask));
	lp = (unsigned long *) dp->b_rptr;
	ldl_debug_mask = *lp;

	mp->b_datap->db_type = M_IOCACK;
	iocp->ioc_error = 0;
	iocp->ioc_rval = 0;

	qreply(WR(dl->rq), mp);
	return DONE;
}

/****************************************************************************/
/* End of: IOCTL routine helpers.                                           */
/****************************************************************************/

STATIC int
ldl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	dev_t i;

/* printk("Before open: pt_n_alloc=%d sap_n_alloc=%d ndev_n_alloc=%d\n",
	pt_n_alloc, sap_n_alloc, ndev_n_alloc); */
	spin_lock(&first_open_lock);

	if (sflag == CLONEOPEN) {
		for (i = 1; i < LDL_N_MINOR; i++)
			if (dl_dl[i].rq == NULL)
				break;
		if (i == LDL_N_MINOR) {
			spin_unlock(&first_open_lock);
			return ENXIO;
		}
	} else {
		if ((i = getminor(*devp)) >= LDL_N_MINOR) {
			spin_unlock(&first_open_lock);
			return EBUSY;
		}
	}
	/* *devp = makedevice(major(*devp), i); */
	*devp = MKDEV(MAJOR(*devp), i);

	if (q->q_ptr != NULL) {
		spin_unlock(&first_open_lock);
		return 0;
	}

	ASSERT(dl_dl[i].magic == 0);
	dl_dl[i].magic = DL_MAGIC;
	dl_dl[i].rq = q;
	dl_dl[i].lost_rcv = dl_dl[i].lost_send = 0;
	dl_dl[i].bufwait = 0;
	dl_dl[i].dlstate = DL_UNATTACHED;
	dl_dl[i].priority = 50;
	dl_dl[i].subs = NULL;
	dl_dl[i].flags = LDLFLAG_DEFAULT;
	q->q_ptr = WR(q)->q_ptr = &dl_dl[i];

	dl_dl[i].next_open = first_open;
	first_open = &dl_dl[i];

	if (dl_dl[i].next_open == NULL) {
		spin_unlock(&first_open_lock);
		if (notifier_register() < 0)
			printk("ldl: ldl_open: Unable to add notifier\n");
	} else
		spin_unlock(&first_open_lock);

	qprocson(q);
	return 0;
}

STATIC int
ldl_close(queue_t *q, int flag, cred_t *crp)
{
	struct dl **dlp, *dl = (struct dl *) q->q_ptr;
	pl_t psw;

	ASSERT(dl != NULL);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT(dl->rq == q);

	SPLSTR(psw);

	if (dl->bufwait) {
		unbufcall(dl->bufwait);
		dl->bufwait = 0;
	}

	if (dl->dlstate == DL_IDLE) {
		sap_destroy_all(dl);
		dl->dlstate = DL_UNBOUND;
	}
	if (dl->dlstate != DL_UNATTACHED) {
		ndev_release(dl);
		dl->addr_len = 0;
		dl->ndev = NULL;
		dl->dlstate = DL_UNATTACHED;
	}

	SPLX(psw);

	spin_lock(&first_open_lock);
	dl->rq = q->q_ptr = WR(q)->q_ptr = NULL;
	dl->magic = 0;

	for (dlp = &first_open; *dlp; dlp = &(*dlp)->next_open)
		if (*dlp == dl)
			break;

	if (*dlp != NULL)
		*dlp = dl->next_open;
	else
		printk("ldl: ldl_close: Endpoint not on open list\n");

	if (first_open == NULL) {
		spin_unlock(&first_open_lock);
		if (notifier_unregister() < 0)
			printk("ldl: ldl_close: Unable to remove notifier\n");
	} else
		spin_unlock(&first_open_lock);

	qprocsoff(q);
	return 0;
}

STATIC int
write_service_raw(struct dl *dl, mblk_t *mp)
{
	ASSERT(dl != NULL);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT(!dl->bufwait);

	ASSERT(mp != NULL);
	ASSERT(mp->b_datap != NULL);
	ASSERT(mp->b_rptr != NULL);

	ASSERT(mp->b_datap->db_type == M_DATA);

	if ((dl->flags & LDLFLAG_RAW) != 0) {
		if (ldl_debug_mask & LDL_DEBUG_UDREQ)
			ldl_mp_data_dump("ldl_unitdata_req", mp,
					 ldl_debug_mask & LDL_DEBUG_ALLDATA);
		ginc(unitdata_req_cnt);
		return tx_func_raw(dl, mp);
	}

	printk("dl: Unexpected raw M_DATA message.\n");
	freemsg(mp);

	return DONE;
}

STATIC int
write_service(struct dl *dl, mblk_t *mp)
{
	dl_ulong primitive;

	ASSERT(dl != NULL);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT(!dl->bufwait);

	ASSERT(mp != NULL);
	ASSERT(mp->b_datap != NULL);
	ASSERT(mp->b_rptr != NULL);

	ASSERT(mp->b_datap->db_type == M_PROTO || mp->b_datap->db_type == M_PCPROTO);

	primitive = ((union DL_primitives *) mp->b_rptr)->dl_primitive;

	if (primitive == DL_UNITDATA_REQ)
		return tx_func_proto(dl, mp);

	switch (primitive) {
	case DL_INFO_REQ:
		return ws_info(dl, mp);
	case DL_PHYS_ADDR_REQ:
		return ws_phys_addr(dl, mp);
	case DL_ATTACH_REQ:
		return ws_attach(dl, mp);
	case DL_DETACH_REQ:
		return ws_detach(dl, mp);
	case DL_BIND_REQ:
		return ws_bind(dl, mp);
	case DL_UNBIND_REQ:
		return ws_unbind(dl, mp);
	case DL_SUBS_BIND_REQ:
		return ws_subs_bind(dl, mp);
	case DL_SUBS_UNBIND_REQ:
		return ws_subs_unbind(dl, mp);
	case DL_UDQOS_REQ:
		return ws_udqos(dl, mp);
	case DL_PROMISCON_REQ:
		return ws_promiscon(dl, mp);
	case DL_PROMISCOFF_REQ:
		return ws_promiscoff(dl, mp);

	case DL_ENABMULTI_REQ:
		return ws_enabmulti(dl, mp);
	case DL_DISABMULTI_REQ:
		return ws_disabmulti(dl, mp);
	case DL_CONNECT_REQ:
	case DL_CONNECT_RES:
	case DL_TOKEN_REQ:
	case DL_DISCONNECT_REQ:
	case DL_RESET_REQ:
	case DL_RESET_RES:
	case DL_DATA_ACK_REQ:
	case DL_REPLY_REQ:
	case DL_REPLY_UPDATE_REQ:
	case DL_XID_REQ:
	case DL_TEST_REQ:
	case DL_SET_PHYS_ADDR_REQ:
	case DL_GET_STATISTICS_REQ:
		return ws_error(dl, mp, primitive, DL_NOTSUPPORTED);
	default:
		return ws_error(dl, mp, primitive, DL_BADPRIM);
	}
}

STATIC int
do_ioctl(struct dl *dl, mblk_t *mp)
{
	struct iocblk *iocp;

	ginc(ioctl_cnt);
	ASSERT(dl != NULL);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT(mp != NULL);
	ASSERT(mp->b_datap->db_type == M_IOCTL);

	iocp = (struct iocblk *) mp->b_rptr;

	switch (iocp->ioc_cmd) {
	case LDL_SETFLAGS:
		return ioc_setflags(dl, iocp, mp);
	case LDL_FINDPPA:
		return ioc_findppa(dl, iocp, mp);
	case LDL_GETNAME:
		return ioc_getname(dl, iocp, mp);
	case LDL_GETGSTATS:
		return ioc_getgstats(dl, iocp, mp);
	case LDL_SETDEBUG:
		return ioc_set_debug_mask(dl, iocp, mp);
	default:
		return ioc_nak(dl, mp);
	}
}

/*
 *  Write queue put routine
 */
STATIC int
ldl_wput(queue_t *q, mblk_t *mp)
{
	unsigned char msg_type = mp->b_datap->db_type;
	dl_ulong primitive;

	/* Keep most common case out of switch for optimum performance */
	if (msg_type == M_PROTO) {
		primitive = ((union DL_primitives *) mp->b_rptr)->dl_primitive;
		if (primitive == DL_UNITDATA_REQ && q->q_count != 0) {	/* keep msgs FIFO */
			if (!putq(q, mp))
				freemsg(mp);
			ginc(unitdata_req_q_cnt);
			return (0);
		}
		if (write_service((struct dl *) q->q_ptr, mp) == DONE)
			return (0);
	} else {
		switch (msg_type) {
		case M_PCPROTO:
			if (write_service((struct dl *) q->q_ptr, mp) == DONE)
				return (0);
			break;
		case M_DATA:
			if (q->q_count != 0) {	/* keep msgs FIFO */
				if (!putq(q, mp))
					freemsg(mp);
				return (0);
			}
			if (write_service_raw((struct dl *) q->q_ptr, mp) == DONE)
				return (0);
			break;
		case M_FLUSH:
			if (*mp->b_rptr & FLUSHW) {
				flushq(q, FLUSHALL);
				*mp->b_rptr &= ~FLUSHW;
			}
			if (*mp->b_rptr & FLUSHR) {
				flushq(RD(q), FLUSHALL);
				qreply(q, mp);
			} else
				freemsg(mp);
			return (0);
		case M_IOCTL:
			if (do_ioctl((struct dl *) q->q_ptr, mp) == DONE)
				return (0);
			break;
		default:
			freemsg(mp);
			return (0);
		}
	}
	if (!putq(q, mp))
		freemsg(mp);
	return (0);
}

/*
 *  Write queue service routine
 */
STATIC int
ldl_wsrv(queue_t *q)
{
	mblk_t *mp;
	struct dl *dl = (struct dl *) q->q_ptr;

	ASSERT(q != NULL);
	ASSERT(dl != NULL);
	ASSERT(dl->magic == DL_MAGIC);
	ASSERT(!dl->bufwait);

	while ((mp = getq(q)) != NULL) {
		if (mp->b_datap->db_type != M_DATA) {
			ASSERT(mp->b_datap->db_type == M_PROTO
			       || mp->b_datap->db_type == M_PCPROTO);

			if (write_service(dl, mp) == RETRY) {
				if (!putbq(q, mp))
					freemsg(mp);	/* FIXME */
				return (0);
			}
		} else if (write_service_raw(dl, mp) == RETRY) {
			if (!putbq(q, mp))
				freemsg(mp);	/* FIXME */
			return (0);
		}
	}
	return (0);
}

/*
 *  Read queue service routine
 */
STATIC int
ldl_rsrv(queue_t *q)
{
	mblk_t *mp;
	while (canputnext(q)) {
		if ((mp = getq(q)) == NULL)
			break;

		if (ldl_debug_mask & LDL_DEBUG_UDIND)
			ldl_mp_data_dump("ldl_unitdata_ind", mp,
					 ldl_debug_mask & LDL_DEBUG_ALLDATA);
		putnext(q, mp);
		ginc(unitdata_ind_cnt);
	}
	return (0);
}

#ifdef LINUX
unsigned short modid = DRV_ID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module ID number for LDL driver (0 for allocation).");

unsigned short major = CMAJOR_0;
MODULE_PARM(major, "h");
MODULE_PARM_DESC(major, "Major device number for LDL driver (0 for allocation).");
#endif				/* LINUX */

#ifdef LFS
STATIC struct cdevsw ldl_cdev = {
	.d_name = DRV_NAME,
	.d_str = &ldl_info,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};
int __init
ldl_init(void)
{
	int err;
	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = register_strdev(&ldl_cdev, major)) < 0)
		return (err);
	if (err > 0)
		major = err;
	return (0);
}

void __exit
ldl_exit(void)
{
	return (void) unregister_strdev(&ldl_cdev, major);
}

#ifdef CONFIG_STREAMS_LDL_MODULE
module_init(ldl_init);
module_exit(ldl_exit);
#endif

#elif defined LIS

STATIC int ldl_initialized = 0;
STATIC void
ldl_init(void)
{
	int err;
	if (ldl_initialized != 0)
		return;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = lis_register_strdev(major, &ldl_info, UNITS, DRV_NAME)) < 0) {
		cmn_err(CE_WARN, "%s: Cannot register major %d\n", DRV_NAME, major);
		ldl_initialized = err;
		return;
	}
	ldl_initialized = 1;
	if (major == 0 && err > 0) {
		major = err;
		ldl_initialized = 2;
	}
	return;
}
STATIC void
ldl_terminate(void)
{
	int err;
	if (ldl_initialized <= 0)
		return;
	if (major) {
		if ((err = lis_unregister_strdev(major)) < 0)
			cmn_err(CE_PANIC, "%s: Cannot unregister major %d\n", DRV_NAME, major);
		major = 0;
	}
	ldl_initialized = 0;
	return;
}

int
init_module(void)
{
	(void) major;
	ldl_init();
	if (ldl_initialized < 0)
		return ldl_initialized;
	return (0);
}

void
cleanup_module(void)
{
	return ldl_terminate();
}

#endif
