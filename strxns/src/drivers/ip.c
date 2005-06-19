/*****************************************************************************

 @(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/06/19 09:27:33 $

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

 Last Modified $Date: 2005/06/19 09:27:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ip.c,v $
 Revision 0.9.2.2  2005/06/19 09:27:33  brian
 - working up IP NPI driver

 Revision 0.9.2.1  2005/06/16 20:41:45  brian
 - Start of the IP NPI driver.

 *****************************************************************************/

#ident "@(#) $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/06/19 09:27:33 $"

static char const ident[] =
    "$RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/06/19 09:27:33 $";

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
#define IP_REVISION	"OpenSS7 $RCSfile: ip.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/06/19 09:27:33 $"
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
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size accepted */
	.mi_hiwat = 1 << 15,	/* Hi water mark */
	.mi_lowat = 1 << 10,	/* Lo water mark */
};

STATIC int ip_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int ip_close(queue_t *, int, cred_t *);

STATIC int ip_rput(queue_t *, mblk_t *);
STATIC int ip_rsrv(queue_t *);

STATIC struct qinit ip_rinit = {
	.qi_putp = ip_rput,	/* Read put (msg from below) */
	.qi_srvp = ip_rsrv,	/* Read queue service */
	.qi_qopen = ip_open,	/* Each open */
	.qi_qclose = ip_close,	/* Last close */
	.qi_minfo = &ip_minfo,	/* Information */
};

STATIC int ip_wput(queue_t *, mblk_t *);
STATIC int ip_wsrv(queue_t *);

STATIC struct qinit ip_winit = {
	.qi_putp = ip_wput,	/* Write put (msg from above) */
	.qi_srvp = ip_wsrv,	/* Write queue service */
	.qi_minfo = &ip_minfo,	/* Information */
};

MODULE_STATIC struct streamtab ip_info = {
	.st_rdinit = &ip_rinit,	/* Upper read queue */
	.st_wrinit = &ip_winit,	/* Lower read queue */
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
 *  Primary data structure.
 */
struct ip;
typedef struct ip ip_t;
struct ip {
	ip_t *next;			/* linkage for master list */
	ip_t **prev;			/* linkage for master list */
	major_t cmajor;			/* major device number */
	minor_t cminor;			/* minor device number */
	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	cred_t cred;			/* credentials of opener */
	atomic_t refcnt;		/* reference count */
	spinlock_t qlock;		/* queue lock */
	queue_t *rwait;			/* RD queue waiting on lock */
	queue_t *wwait;			/* WR queue waiting on lock */
	int users;			/* lock holders */
	int bcid;			/* bufcall id */
	np_ulong i_flags;		/* interface flags */
	np_ulong i_state;		/* interface state */
	np_ulong state;			/* IP state */
	ip_t *bnext;			/* linkage for bind hash */
	ip_t **bprev;			/* linkage for bind hash */
	struct ip_bind_bucket *bindb;	/* linkage for bind hash */
	unsigned char tos;		/* IP type of service */
	unsigned char ttl;		/* IP time to live */
	struct sockaddr_storage addr;	/* bound addresses */
	size_t anum;			/* number of sockaddr_in addresses in addr */
	uint8_t protoids[16];		/* bound protocol ids */
	size_t pnum;			/* number of protocol ids in protoids */
};

/*
 *  Bind buckets, caches and hashes.
 */
struct ip_bind_bucket {
	struct ip_bind_bucket *next;	/* linkage of bind buckets for hash slot */
	struct ip_bind_bucket **prev;	/* linkage of bind buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short port;		/* port number (host order) */
	struct ip *owners;		/* list of owners of this protocol/port combination */
	struct ip *default;		/* default destinations for this protocol */
};
struct ip_hash_bucket {
	rwlock_t lock;
	struct ip_bind_bucket *list;
};

/*
 *  State changes.
 */
#ifdef DEBUG
STATIC const char *state_name(long state)
{
	switch (state) {
	case NS_UNBND:
		return ("NS_UNBND");
	case NS_WACK_BREQ:
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:
		return ("NS_WACK_UREQ");
	case NS_IDLE:
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:
		return ("NS_WACK_DREQ11");
	case NS_NOSTATES:
		return ("NS_NOSTATES");
	default:
		return ("(unknown)");
	}
}
#endif
STATIC void
ip_set_state(struct ip *ip, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, ip, state_name(state), state_name(ip->i_state)));
	ip->i_state = state;
}
STATIC long
ip_get_state(struct ip *ip)
{
	return (ip->i_state);
}

/*
 *  =========================================================================
 *
 *  IP Message Transmission.
 *
 *  =========================================================================
 */
#if HAVE_KFUNC_DST_OUTPUT
STATIC INLINE in ip_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;
#ifdef NETIF_F_TSO
	ip_select_ident_more(iph, &rt->u.dst, NULL, 0);
#else
	ip_select_ident(iph, &rt->u.dst, NULL);
#endif
	ip_send_check(iph);
#if HAVE_KFUNC_IP_DST_OUTPUT
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev, ip_dst_output);
#else
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev, dst_output);
#endif
}
#else
STATIC INLINE in ip_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;
	if (skb->len > dst_pmtu(&rt->u.dst)) {
		rare();
		return ip_fragment(skb, skb->dst->output);
	} else {
		iph->frag_off != __constant_htons(IP_DF);
		ip_send_check(iph);
		return skb->dst->output(skb);
	}
}
#endif

STATIC void ip_xmit_msg(uint32_t daddr, uint32_t saddr, uint8_t protoid, mblk_t *mp, struct ip *ip)
{
	struct rtable *rt = NULL;
	ensure(mp, return);
	if (!ip_route_output(&rt, daddr, 0, 0, 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = msgdsize(mp);
		size_t tlen = plen + sizeof(struct iphdr);
		usual(hlen);
		usual(plen);
		/* IMPLEMENTATION NOTE:- Sockets ensures that there is sufficient headroom in the passed in sk_buff,
		   fills out the IP header, calculates the checksum and sends the buffer.  For STREAMS, no socket
		   buffer is passed in, just a message block, so a new socket buffer is allocated with the necessary
		   headroom and the message blocks are copied into the socket buffer. */
		if (skb = allocb_skb(hlen + tlen, GFP_ATOMIC)) {
			mblk_t *mp;
			struct iphdr *iph;
			unsigned char *data;
			skb_reserve(skb, hlen);
			/* find headers */
			iph = (typeof(iph)) __skb_put(skb, tlen);
			sh = (typeof(sh)) (iph + 1);
			data = (unsigned char *) (sh);
			skb->dst = &rt->u.dst;
			skb->priority = 0;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = ip->tos;
			iph->ttl = ip->ttl;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = protoid;
			iph->tot_len = htons(len);
			skb->nh.iph = iph;
#if ! HAVE_KFUNC_DST_OUTPUT
#if HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS
			__ip_select_ident(iph, &rt->u.dst);
#elif HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS
			__ip_select_ident(iph, &rt->u.dat, 0);
#else
#error HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS or HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS must be defined.
#endif
			/* For sockets the passed in sk_buff represents a single packet.  For STREAMS, the passed in
			   mblk_t pointer is possible a message buffer chain and we must iterate along the b_cont
			   pointer. */
			for (bp = mp; bp; bp = bp->b_cont) {
				int blen = bp->b_wptr - bp->b_rptr;
				if (blen > 0) {
					bcopy(bp->b_rptr, data blen);
					data += blen;
				} else
					rare();
			}
#if HAVE_KFUNC_DST_OUTPUT
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, ip_queue_xmit);
#else
			ip_queue_xmit(skb);
#endif
		} else
			rare();
	} else
		rare();
	freemsg(mp);
	return;
}

STATIC int ip_unitdata_req(queue_t *q, struct sockaddr_in *dest, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	struct sockaddr_in *srce = &ip->addr;
	uint32_t saddr = srce->sin_addr.s_addr;
	uint32_t daddr = dest->sin_addr.s_addr;
	uint8_t protoid = ntohs(dest->sin_port);
	ip_xmit_msg(daddr, saddr, protoid, mp, ip);
	return (0);
}

/*
 *  ===================================================================
 *
 *  NPI Provider --> NPI User (Transport) Primitives
 *
 *  ===================================================================
 */
/*
 *  M_ERROR		    - error out the stream
 *  ---------------------------------------------------------------
 */
STATIC int n_error_reply(queue_t *q, long uerrno)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;
	switch (uerrno) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	case 0:
	case 1:
	case 2:
		return (err);
	}
	if ((mp = ip_allocb(sp, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = (err < 0) ? -err : err;
		*(mp->b_wptr)++ = (err < 0) ? -err : err;
		putnext(sp->rq, mp);
		local_bh_disable();
		bh_lock_ip(sp);
		ip_unbind(sp);
		bh_unlock_ip(sp);
		local_bh_enable();
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  N_INFO_ACK		16 - Information Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int n_info_ack(queue_t *q)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;
	N_info_ack *p;
	N_qos_sel_info_ip_t *qos;
	N_qos_range_info_ip_t *qor;
	size_t add_len = ip->anum * sizeof(struct sockaddr_in);
	size_t qos_len = sizeof(*qos);
	size_t qor_len = sizeof(*qor);
	size_t pro_len = ip->pnum * sizeof(uint8_t);
	if (!(mp = ip_allocb(q, sizeof(*p) + add_len + qos_len + qor_len + pro_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_ACK;
	p->NSDU_size = 65535;
	p->ENSDU_size = 0;
	p->CDATA_size = 0;
	p->DDATA_size = 0;
	p->ADDR_size = sizeof(struct sockaddr_storage);
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
	p->QOS_range_length = qor_len;
	p->QOS_range_offset = qor_len ? sizeof(*p) + add_len + qos_len : 0;
	p->OPTIONS_flags = 0;
	p->NIDU_size = 65535;
	p->SERV_type = N_CLNS;
	p->CURRENT_state = ip_get_state(ip);
	p->PROVIDER_type = N_SNICFP;
	p->NODU_size = ip->pmtu ? : 536;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + qor_len : 0;
	p->NPI_version = N_VERSION_2;
	mp->b_wptr += sizeof(*p);
	if (add_len) {
		bcopy(&ip->addr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	qos = (typeof(qos)) mp->b_wptr;
	qos->n_qos_type = N_QOS_SEL_INFO_IP;
	qos->ttl = ip->ttl;
	qos->tos = ip->tos;
	mp->b_ptr += qos_len;
	qor = (typeof(qor)) mp->b_wptr;
	qor->n_qos_type = N_QOS_RANGE_INFO_IP;
	mp->b_wptr += qor_len;
	if (pro_len) {
		bcopy(&ip->prot, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	putnext(ip->rq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	goto error;
      error:
	return (err);
}

/*
 *  N_BIND_ACK		17 - NS User bound to network address
 *  ---------------------------------------------------------------
 */
STATIC int n_bind_ack(queue_t *q)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	mblk_t *mp;
	N_bind_ack_t *p;
	size_t add_len = sp->anum * sizeof(struct sockaddr_in);
	size_t pro_len = sp->pnum * sizeof(uint8_t);
	if (ip_get_state(ip) != NS_WACK_BREQ)
		goto outstate;
	if (!(mp = ip_allocb(q, sizeof(*p) + add_len + pro_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = 0;
	p->TOKEN_value = 0;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
	mp->b_wptr += sizeof(*p);
	if (add_len) {
		bcopy(&ip->addr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	if (pro_len) {
		bcopy(&ip->prot, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	ip_set_state(ip, NS_IDLE);
	putnext(ip->rq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	goto error;
      outstate:
	err = -EFAULT;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      error:
	return (err);
}

/*
 *  N_ERROR_ACK		18 - Error Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int n_error_ack(queue_t *q, int prim, int err)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;
	N_error_ack_t *p;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	case 0:
		return (err);
	case -EADDRINUSE:
		err = NBOUND;
		break;
	case -EADDRNOTAVAIL:
		err = NNOADDR;
		break;
	case -EACCES:
		err = NACCESS;
		break;
	case -EOPNOTSUPP:
		err = NNOTSUPPORT;
		break;
	}
	if (!(mp = ip_allocb(ip, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (N_error_ack_t *) mp->b_wptr;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = prim;
	p->NPI_error = (err < 0) ? NSYSERR : err;
	p->UNIX_error = (err < 0) ? -err : 0;
	mp->b_wptr += sizeof(*p);
	switch (ip_get_state(ip)) {
	case NS_WACK_OPTREQ:
	case NS_WACK_UREQ:
	case NS_WCON_CREQ:
		ip_set_state(ip, NS_IDLE);
		break;
	case NS_WCON_RREQ:
		ip_set_state(ip, NS_DATA_XFER);
		break;
	case NS_WACK_BREQ:
		ip_set_state(ip, NS_UNBND);
		break;
	case NS_WACK_CRES:
		ip_set_state(ip, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ6:
		ip_set_state(ip, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		ip_set_state(ip, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		ip_set_state(ip, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		ip_set_state(ip, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		ip_set_state(ip, NS_WRES_RIND);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs normally when we
		   send NOUTSTATE or NNOTSUPPORT or are responding to an N_OPTMGMT_REQ in other than the NS_IDLE state. 
		 */
	}
	putnext(ip->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  N_OK_ACK		19 - Success Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int n_ok_ack(queue_t *q, np_ulong prim, np_ulong seq, np_ulong tok)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;
	N_ok_ack_t *p;
	if (!(mp = ip_alloc(q, sizeof(*p), BRPI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch (ip_get_state(ip)) {
	case NS_WACK_OPTREQ:
		ip_set_state(ip, NS_IDLE);
		break;
	case NS_WACK_RRES:
		ip_set_state(ip, NS_DATA_XFER);
		break;
	case NS_WACK_UREQ:
		ip_set_state(ip, NS_UNBND);
		break;
	case NS_WACK_CRES:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		swerr();
		ip_set_state(ip, NS_IDLE);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs normally when we are 
		   responding to a N_OPTMGMT_REQ in other than the NS_IDLE state. */
	}
	putnext(ip->rq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	goto error;
      error:
	return (err);
}

/*
 *  N_UNITDATA_IND	20 - Connection-less data receive indication
 *  ---------------------------------------------------------------
 */
STATIC int n_unitdata_ind(queue_t *q, unsigned char *src_ptr, size_t src_len, unsigned char *dst_ptr, size_t dst_len,
			  mblk_t *db)
{
	struct ip *ip = IP_PRIV(q);
	mblk_t *mp;
	N_unitdata_ind_t *p;
	if (!(canputnext(ip->rq)))
		goto ebusy;
	if (!(mp = ip_alloc(q, sizeof(*p) + src_len + dst_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
	p->ERROR_type = 0;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = db;
	putnext(ip->rq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: read queue is flow controlled\n", DRV_NAME, ip));
	goto error;
      error:
	return (err);
}

/*
 *  N_UDERROR_IND	21 - UNITDATA Error Indication
 *  -------------------------------------------------------------------
 */
STATIC int n_uderror_ind(queue_t *q, unsigned char *dest_ptr, size_t dest_len, np_ulong error, mblk_t *db)
{
	struct ip *ip = IP_PRIV(q);
	N_uderror_ind_t *p;
	if (!canputnext(ip->rq))
		goto ebusy;
	if (!(mp = ip_allocb(q, sizeof(*p) + dest_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UDERROR_IND;
	p->DEST_offset = dest_len ? sizeof(*p) : 0;
	p->DEST_length = dest_len;
	p->RESERVED_field = 0;
	p->ERROR_type = error;
	mp->b_wptr += sizeof(*p);
	if (dest_len) {
		bcopy(dest_ptr, mp->b_wptr, dest_len);
		mp->b_wptr += dest_len;
	}
	mp->b_cont = db;
	putnext(ip->rq, mp);
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: read queue is flow controlled\n", DRV_NAME, ip));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: no buffers\n", DRV_NAME, ip));
	goto error;
      error:
	return (err);
}

STATIC int n_uderror_reply(queue_t *q, void *dest_ptr, size_t dest_len, long error, mblk_t *db)
{
	int err;
	switch (error) {
	case -EINVAL:
		return n_error_reply(q, err);
	case NBADADDR:
	case NBADDATA:
		break;
	case NOUTSTATE:
		return n_error_reply(q, -EPROTO);
	}
	if ((err = n_uderror_ind(q, dest_ptr, dest_len, err, db)) == QR_DONE)
		return (QR_STRIP);
	return (err);
}

/*
 *  ===================================================================
 *
 *  NPI User --> NPI Provider (IP) Primitives
 *
 *  ===================================================================
 */
/*
 *  N_INFO_REQ		 5 - Information request
 *  -------------------------------------------------------------------
 */
STATIC int n_info_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	(void) ip;
	return n_info_ack(q);
}

/*
 *  N_BIND_REQ		 6 - Bind a NS user to network address
 *  -------------------------------------------------------------------
 *  It is possible to bind more than one address to a stream.  Each address is formatted as a
 *  sockaddr_in up to the size of sockaddr_storage (which is 8 addresses on Linux).  If the
 *  DEFAULT_DEST flag is set, then the port number is insignificant (and should be coded zero) or an
 *  address is not provided.  When DEFAULT_DEST is not specified, the port number is significant.
 *  Once bound, all packets that arrive for the specified address(es) and port number
 *  combindation(s) are delivered to the bound stream.  For DEFAULT_DEST streams, all packets that
 *  arrive for the specified address(es) that have port numbers that are bound to no other address
 *  will be delivered to the DEFAULT_DEST stream.  DEFAULT_DEST streams that are bound with no
 *  address (or to a wildcard address) will have all packets delivered that are bound to no other
 *  address or port number combination.  Protocol ids must always be specified.
 */
STATIC int n_bind_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ip_get_state(ip) != NS_UNBND)
		goto outstate;
	ip_set_state(ip, NS_WACK_BREQ);
	if (p->BIND_flags && p->BIND_flags != DEFAULT_DEST)
		goto badflag;
	if (p->ADDR_length && (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto badaddr;
	if (p->PROTOID_length && (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length))
		goto badprotoid;
	if (p->PROTOID_length == 0)
		goto noprotoid;
	if (p->PROTOID_length > 1)
		goto badprotoid2;
	{
		struct sockaddr_storage sa;
		struct sockaddr_in *add_in = (typeof(add_in)) & sa;
		size_t add_len = p->ADDR_length;
		size_t anum;
		unsigned char *protoids = (typeof(protoids)) (mp->b_rptr + p->PROTOID_offset);
		size_t pnum = p->PROTOID_length;
		ulong flags = p->BIND_flags;
		if (add_len) {
			if (add_len < sizeof(struct sockaddr_in))
				goto badaddr;
			if (add_len > sizeof(struct sockaddr_storage))
				goto badaddr;
			anum = add_len / sizeof(struct sockaddr_in);
			if (add_len != anum * sizeof(struct sockaddr_in))
				goto badaddr;
			/* avoid alignment problems */
			bcopy(mp->b_rptr + p->ADDR_offset, add_in, add_len);
			if (add_in->sin_family != AF_INET)
				goto badaddr;
		} else {
			anum = 1;
			add_in->sin_family = AF_INET;
			add_in->sin_port = 0;
			add_in->sin_addr.s_addr = INADDR_ANY;
		}
		if ((err = ip_bind_req(q, add_in, anum, protoids, pnum, flags)) < 0)
			switch (err) {
			case -EADDRNOTAVAIL:
				goto bound;
			case -EDESTADDRREQ:
				goto noaddr;
			case -EACCES:
				goto access;
			default:
				goto provspec;
			}
		if ((err = n_bind_ack(q, p->CONIND_number)) == 0)
			return (QR_DONE);
		ip_unbind_req(q);
		return (err);
	}
      provspec:
	err = err;
	ptrace(("%s: %p: provider specific UNIX error %d\n", DRV_NAME, ip, -err));
      access:
	err = NACCESS;
	ptrace(("%s: %p: no permission for requested address\n", DRV_NAME, ip));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: could not allocate address\n", DRV_NAME, ip));
	goto error;
      bound:
	err = NBOUND;
	ptrace(("%s; %p: default address is already bound\n", DRV_NAME, ip));
	goto error;
      noprotoid:
	err = NNOPROTOID;
	ptrace(("%s: %p: could not allocate protocol id\n", DRV_NAME, ip));
	goto error;
      badflag:
	err = NBADFLAG;
	ptrace(("%s: %p: invalid flags\n", DRV_NAME, ip));
	goto error;
      baddaddr:
	err = NBADADDR;
	ptrace(("%s: %p: address is invalid\n", DRV_NAME, ip));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      error:
	return n_error_ack(q, N_BIND_REQ, err);
}

/*
 *  N_UNBIND_REQ	     7 - Unbind NS user from network address
 *  -------------------------------------------------------------------
 */
STATIC int n_unbind_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ip_get_state(ip) != NS_IDLE)
		goto outstate;
	ip_set_state(ip, NS_WACK_UREQ);
	if ((err = ip_unbind_req(q)))
		goto error;
	return n_ok_ack(ip, N_UNBIND_REQ, err);
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      error:
	return n_error_ack(ip, N_UNBIND_REQ, err);
}

/*
 *  N_OPTMGMT_REQ	     9 - Options management request
 *  -------------------------------------------------------------------
 */
STATIC int n_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_info_ip_t *q = (typeof(q)) (mp->b_rptr + p->QOS_offset);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ip_get_state(ip) == NS_IDLE)
		ip_set_state(ip, NS_WACK_OPTREQ);
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt;
		if (q->n_qos_type != N_QOS_SEL_INFO_IP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
	}
	if (p->QOS_length) {
		if (q->tos != -1UL)
			ip->tos = q->tos;
		if (q->ttl != -1UL)
			ip->ttl = q->ttl;
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		ip->flags |= IP_FLAG_DEFAULT_RC_SEL;
	else
		ip->flags &= ~IP_FLAG_DEFAULT_RC_SEL;
	return n_ok_ack(q, N_OPTMGMT_REQ, 0, 0);
      badopt2:
	err = NBADOPT;
	ptrace(("%s: %p: QOS options were invalid\n", DRV_NAME, ip));
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: QOS structure type not supported\n", DRV_NAME, ip));
	goto error;
      badopt:
	err = NBADOPT;
	ptrace(("%s: %p: QOS options were invalid\n", DRV_NAME, ip));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      error:
	return n_error_ack(q, N_OPTMGMT_REQ, err);
}

/*
 *  N_UNITDATA_REQ	    xx - Unitdata request
 *  -------------------------------------------------------------------
 *  N_UNITDATA_REQ does not have a source address field.  We use the source address to which the
 *  stream is bound.  In the event that the stream is bound as a DEFAULT_DEST with no address or
 *  bound to with DEFAULT_DEST to an address with no port number, or if the stream is bound to
 *  multiple protocol identifiers, then the protocol identifier, source address and source port
 *  number used will be the last received protocol identifier, source address or port number.  If
 *  there is no received protocol identifier, source address or port number to use, then the request
 *  will fail and a N_UDERROR_IND will be sent indicating the failure.
 */
STATIC int n_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	int err;
	size_t mlen;
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in dest;
	size_t dlen = 0;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (ip_get_state(ip) != NS_IDLE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badaddr;
	if (!mp->b_cont)
		goto baddata;
	if ((mlen = msgdsize(mp->b_cont)) == 0 || mlen > 65535)
		goto baddata2;
	{
		size_t dlen = p->DEST_length;
		if (dlen == 0)
			goto noaddr;
		if (dlen != sizeof(struct sockaddr_in))
			goto badaddr2;
		/* avoid alignemnt problems */
		bcopy(mp->b_rptr + p->DEST_offset, &dest, dlen);
		if (dest->sin_family != AF_INET)
			goto badaddr;
		if (dest->sin_port == 0)
			goto badaddr;
		if (dest->sin_addr.s_addr == INADDR_ANY)
			goto badaddr;
		if ((err = ip_unitdata_req(q, &dest, mp->b_cont)) < 0)
			switch (err) {
			case -EDESTADDRREQ:
				goto noaddr;
			case -EMSGSIZE:
				goto baddata;
			case -EACCES:
				goto access;
			default:
				goto provspec;
			}
		return (QR_DONE);
	}
      baddata2:
	err = NBADDATA;
	ptrace(("%s: %p: invalid amount of user data %u\n", DRV_NAME, ip, mlen));
	goto error;
      baddata:
	err = NBADDATA;
	ptrace(("%s: %p: invalid amount of user data\n", DRV_NAME, ip));
	goto error;
      badaddr2:
	err = NBADADDR;
	ptrace(("%s: %p: invalid address format\n", DRV_NAME, ip));
	goto error;
      noaddr:
	err = NBADADDR;
	ptrace(("%s: %p: could not assign address\n", DRV_NAME, ip));
	goto error;
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: invalid address format\n", DRV_NAME, ip));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: would place interface out of state\n", DRV_NAME, ip));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: invalid primitive format\n", DRV_NAME, ip));
	goto error;
      error:
	return n_uderror_reply(q, &dest, dlen, err, mp->b_cont);
}

STATIC INLINE int ip_trylockq(queue_t *q)
{
	int res;
	struct ip *ip = IP_PRIV(q);
	ip_lock_bh(ip);
	if (!(res = !ip->users++)) {
		if (q == ip->rq)
			ip->rwait = q;
		if (q == ip->wq)
			ip->wwait = q;
	}
	ip_unlock_bh(ip);
	return (res);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int ip_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	t_uscalar_t prim;
	struct ip *ip = IP_PRIV(q);
	t_uscalar_t oldstate = ip_get_state(ip);
	if (mp->b_wptr >= mp->b_rptr + sizeof(ulong))
		switch ((prim = *((t_uscalar_t *) mp->b_rptr))) {
		case N_CONN_REQ:
		case N_CONN_RES:
		case N_DISCON_REQ:
		case N_DATA_REQ:
		case N_EXDATA_REQ:
			rtn = -EOPNOTSUPP;
			break;
		case N_INFO_REQ:
			rtn = n_info_req(q, mp);
			break;
		case N_BIND_REQ:
			rtn = n_bind_req(q, mp);
			break;
		case N_UNBIND_REQ:
			rtn = n_unbind_req(q, mp);
			break;
		case N_UNITDATA_REQ:
			rtn = n_unitdata_req(q, mp);
			break;
		case N_OPTMGMT_REQ:
			rtn = n_optmgmt_req(q, mp);
			break;
		case N_DATACK_REQ:
		case N_RESET_REQ:
		case N_RESET_RES:
			rtn = -EOPNOTSUPP;
			break;
		case N_CONN_IND:
		case N_CONN_CON:
		case N_DISCON_IND:
		case N_DATA_IND:
		case N_EXDATA_IND:
		case N_INFO_ACK:
		case N_BIND_ACK:
		case N_ERROR_ACK:
		case N_OK_ACK:
		case N_UNITDATA_IND:
		case N_UDERROR_IND:
		case N_DATACK_IND:
		case N_RESET_IND:
		case N_RESET_CON:
			rtn = -EPROTO;
			break;
		default:
			rtn = -EOPNOTSUPP;
			break;
		}
	if (rtn < 0) {
		seldom();
		ip_set_state(ip, oldstate);
		/* The put and srv procedures do not recognize all errors.  Sometimes we return an error to here just
		   to restore the previous state.  */
		switch (rtn) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EOPNOTSUPP:
			return n_error_ack(q, rtn);
		case -EPROTO:
			return n_error_reply(q, -EPROTO);
		default:
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int ip_w_data(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	return n_write(ip, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int ip_w_other(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type, ip->cminor);
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int ip_r_other(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type, ip->cminor);
#if 0
	putnext(q, mp);
	return (QR_ABSORBED);
#else
	return (-EOPNOTSUPP);
#endif
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int ip_r_data(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	return ip_recv_msg(ip, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int ip_r_error(queue_t *q, mblk_t *mp)
{
	struct ip *ip = IP_PRIV(q);
	rare();
	return ip_recv_err(ip, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int ip_m_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(OTHERQ(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(OTHERQ(q), FLUSHALL);
		qreply(q, mp);
		return (QR_ABSORBED);
	}
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */

/*
 *  NPI Write Message
 */
/*
 *  IP Read Message
 */
STATIC INLINE int ip_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ip_r_data(q, mp);
	case M_ERROR:
		return ip_r_error(q, mp);
	default:
		return ip_r_other(q, mp);
	}
}
STATIC INLINE int ip_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ip_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ip_w_proto(q, mp);
	case M_FLUSH:
		return ip_m_flush(q, mp);
	default:
		return ip_w_other(q, mp);
	}
}

/*
 *  PUTQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int ip_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0, locked;
	ensure(mp, return (-EFAULT));
	ensure(q, freemsg(mp); return (-EFAULT));
	if (mp->b_datap->db_type < QPCTL && q->q_count) {
		if (!putq(q, mp))
			freemsg(mp);
		return (0);
	}
	if ((locked = ip_trylockq(q)) || mp->b_datap->db_type == M_FLUSH) {
		do {
			/* Fast Path */
			if ((rtn = (*proc) (q, mp)) == QR_DONE) {
				freemsg(mp);
				break;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				break;
			case QR_STRIP:
				if (mp->b_cont)
					if (!putq(q, mp->b_cont))
						freemsg(mp->b_cont);
			case QR_TRIMMED:
				freeb(mp);
				break;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					break;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					break;
				}
				rtn = -EOPNOTSUPP;
			default:
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				break;
			case QR_DISABLE:
				if (!putq(q, mp))
					freemsg(mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					break;
				}
			case -ENOBUFS:
			case -EBUSY:
			case -ENOMEM:
			case -EAGAIN:
			case QR_RETRY:
				if (!putq(q, mp))
					freemsg(mp);
				break;
			}
		} while (0);
		if (locked)
			ip_unlockq(q);
	} else {
		seldom();
		if (!putq(q, mp))
			freemsg(mp);
	}
	return (rtn);
}

/*
 *  SRVQ Service Routine
 *  -----------------------------------
 */
STATIC INLINE int ip_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0;
	ensure(q, return (-EFAULT));
	if (ip_trylockq(q)) {
		mblk_t *mp;
		while ((mp = getq(q))) {
			/* Fast Path */
			if ((rtn = (*proc) (q, mp)) == QR_DONE) {
				freemsg(mp);
				continue;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
			case QR_STRIP:
				if (mp->b_cont)
					if (!putbq(q, mp->b_cont))
						freemsg(mp->b_cont);
			case QR_TRIMMED:
				freeb(mp);
				continue;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					continue;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					continue;
				}
				rtn = -EOPNOTSUPP;
			default:
				pswerr(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				pswerr(("ERROR: (q disabling) %d\n", rtn));
				noenable(q);
				if (!putbq(q, mp))
					freemsg(mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must schedule bufcall */
			case -EBUSY:	/* proc must fail canput */
			case -ENOMEM:	/* proc must schedule re-enable */
			case -EAGAIN:	/* proc must schedule re-enable */
				if (mp->b_datap->db_type < QPCTL) {
					_printd(("ERROR: (q stalled) %d\n", rtn));
					if (!putbq(q, mp))
						freemsg(mp);
					break;
				}
				/* Be careful not to put a priority message back on the queue. */
				switch (mp->b_datap->db_type) {
				case M_PCPROTO:
					mp->b_datap->db_type = M_PROTO;
					break;
				default:
					_printd(("ERROR: (q dropping) %d\n", rtn));
					freemsg(mp);
					continue;
				}
				mp->b_band = 255;
				if (!putq(q, mp))
					freemsg(mp);
				break;
			}
			break;
		}
		ip_unlockq(q);
	}
	return (rtn);
}

STATIC int ip_rput(queue_t *q, mblk_t *mp)
{
	return (int) ip_putq(q, mp, &ip_r_prim);
}
STATIC int ip_rsrv(queue_t *q)
{
	return (int) ip_srvq(q, &ip_r_prim);
}
STATIC int ip_wput(queue_t *q, mblk_t *mp)
{
	return (int) ip_putq(q, mp, &ip_w_prim);
}
STATIC int ip_wsrv(queue_t *q)
{
	return (int) ip_srvq(q, &ip_w_prim);
}

/*
 *  =========================================================================
 *
 *  Bottom end Linux IP hooks.
 *
 *  =========================================================================
 */
typedef int (*ip_rcv_fnc_t) (struct sk_buff *);

STATIC struct ip *ip_lookup(uint8_t proto, uint32_t daddr, uint16_t dport, ip_rcv_fnc_t * fncp)
{
	strict ip *result = NULL;
	int snum = ntohs(dport);
	struct ip_bind_bucket *ib;
	struct ip_hash_bucket *hp = &ip_bind_hash[ip_bind_hashfn(snum)];
	read_lock(&hp->lock);
	for (ib = hp->list; ib && ib->port != snum; ib = ib->next) ;
	if (ib) {
		struct ip *ip;
		int hiscore = 0;
		for (ip = ib->owners; ip; ip = ip->bnext) {
			int score = 0;
			if (ip_get_state(ip) == NS_UNBND)
				continue;
			if (!(ip->userlocks & IP_BINDPORT_LOCK))
				continue;
			if (ip->sport) {
				if (ip->sport != dport)
					continue;
				score++;
				if (ip->anum) {
					struct sockaddr_in *sa;
					if (!(sa = ip_find_saddr(ip, daddr)))
						continue;
					score++;
				}
			}
			if (score > 0) {
				result = ip;
				break;
			}
			if (score > hiscore) {
				hiscore = score;
				result = ip;
			}
		}
	}
	read_unlock(&hp->lock);
	usual(result);
	if (result) {
		ip_hold(result);
		return (result);
	}
	return NULL;
}


STATIC void ip_free(char *data)
{
	struct sk_buff *skb = (typeof(skb)) data;
	_trace();
	ensure(skb, return);
	kfree_skb(skb);
	return;
}
STATIC int ip_v4_rcv(struct sk_buff *skb)
{
	mblk_t *mp;
	struct ip *ip;
	struct iphdr *iph = skb->nh.iph;
	frtn_t fr = { &ip_free, (char *) skb };
	ip_rcv_fnc_t next_v4_rcv = NULL;
	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
	if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0)
		goto linear_fail;
	if (!(ip = ip_lookup(proto, iph->daddr, dport, &next_v4_rcv)))
		goto no_ip_stream;
	if (!ip->rq || !canput(ip->rq))
		goto flow_controlled;
	/* include a hidden IP header */
	if (!(mp = esballoc(skb->nh.raw, skb->len + (skb->data - skb->nh.raw))))
		goto no_buffers;
	mp->b_datap->db_type = M_DATA;
	mp->b_wptr = mp->b_rptr + skb->len + (skb->data - skb->nh.raw);
	/* trim the ip header */
	mp->b_rptr += skb->h.raw - skb->nh.raw;
	skb->dev = NULL;
	if (!putq(ip->rq, mp))
		goto put_failed;
	ip_put(ip);
	return (0);
      put_failed:
	ip_put(ip);
	ptrace(("%s: %p: ERROR: could not put frame to request stream\n", DRV_NAME, ip));
	goto free_it;
      no_buffers:
	ptrace(("%s: %p: ERROR: could not allocate mblk\n", DRV_NAME, ip));
	goto discard_it;
      flow_controlled:
	ptrace(("%s: %p: ERROR: upstream flow control\n", DRV_NAME, ip));
	ip_put(ip);
	goto discard_it;
      no_ip_stream:
	ptrace(("%s: %p: ERROR: no stream\n", DRV_NAME, ip));
	if (next_v4_rcv != NULL)
		return (*next_v4_rcv) (skb);
	goto discard_it;
      linear_fail:
	ptrace(("%s: %p: ERROR: could not linearize socket buffer\n", DRV_NAME, ip));
	goto discard_it;
      bad_pkt_type:
	ptrace(("%s: %p: ERROR: packet not PACKET_HOST\n", DRV_NAME, ip));
	goto discard_it;
      discard_it:
	ptrace(("%s: %p: ERROR: discarding message\n", DRV_NAME, ip));
	kfree_skb(skb);
	return (0);
      free_it:
	ptrace(("%s: %p: ERROR: discarding message\n", DRV_NAME, ip));
	freemsg(mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *ip_priv_cachep = NULL;
STATIC int ip_init_caches(void)
{
	if (!ip_priv_cachep
	    && !(ip_priv_cachep =
		 kmem_cache_create("ip_priv_cachep", sizeof(ip_t), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate ip_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	return (0);
}
STATIC int ip_term_caches(void)
{
	if (ip_priv_cachep) {
		if (kmem_cache_destroy(ip_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ip_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed ip_priv_cachep\n", DRV_NAME));
	}
	return (0);
}
STATIC INLINE struct ip *ip_get(void)
{
	struct ip *ip;
	if ((ip = kmem_cache_alloc(ip_priv_cachep, SLAB_ATOMIC))) {
		bzero(ip, sizeof(*ip));
		atomic_set(&ip->refcnt, 1);
	}
	return (ip);
}
STATIC INLINE void ip_hold(struct ip *ip)
{
	if (ip)
		atomic_inc(&ip->refcnt);
}
STATIC INLINE void ip_put(sturct ip * ip)
{
	if (ip)
		if (atomic_dec_and_test(&ip->refcnt)) {
			kmem_cache_free(ip_priv_cachep, ip);
		}
}
STATIC ip_t *ip_alloc_priv(queue_t *q, ip_t ** slp, major_t cmajor, minor_t cminor, cred_t *crp)
{
	ip_t *ip;
	if ((ip = ip_get())) {
		ip->cmajor = cmajor;
		ip->cminor = cminor;
		ip_hold(ip);
		ip->rq = RD(q);
		ip->rq->q_ptr = ip;
		ip_hold(ip);
		ip->wq = WR(q);
		ip->wq->q_ptr = ip;
		ip->cred = *crp;
		spin_lock_init(&ip->qlock);
		ip->rwait = NULL;
		ip->wwait = NULL;
		ip->users = 0;
		ip_set_state(ip, NS_UNBND);
		spin_lock_init(&ip->lock);
		ip_hold(ip);
		if ((ip->next = *slp))
			ip->next->prev = &ip->next;
		ip->prev = slp;
		*slp = ip;
		qprocson(q);
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", DRV_NAME));
	return (ip);
}
STATIC void ip_free_priv(queue_t *q)
{
	struct ip *ip;
	ensure(q, return);
	ip = IP_PRIV(q);
	ensure(ip, return);
	qprocsoff(q);
	printd(("%s: unlinking private structure, reference count = %d\n", DRV_NAME, atomic_read(&ip->refcnt)));
	spin_lock_bh(&ip->lock);
	{
		__ss_unbufcall(q);
		printd(("%s: removed bufcalls, reference count = %d\n", DRV_NAME, atomic_read(&ip->refcnt)));
		spin_lock(&ip_lock);
		if ((*ip->prev = ip->next))
			ip->next->prev = ip->prev;
		ip->next = NULL;
		ip->prev = NULL;
		spin_unlock(&ip_lock);
		ip_put(ip);
		printd(("%s: unlinked, reference count = %d\n", DRV_NAME, atomic_read(&ip->refcnt)));
		ip->rq->q_ptr = NULL;
		ip->rq = NULL;
		ip_put(ip);
		ip->wq->q_ptr = NULL;
		ip->wq = NULL;
		ip_put(ip);
	}
	spin_unlock_bh(&ip->lock);
	assure(atomic_read(&ip->refcnt) == 1);
	ip_put(ip);		/* should normally be final put */
	return;
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int ip_majors[IP_CMAJORS] = { IP_CMAJOR_0, };
STATIC int ip_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int mindex = 0, err;
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	ip_t *ip, **ipp = &ip_opens;
	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: can't push as module\n", DRV_NAME));
		return (EIO);
	}
	if (cmajor != IP_CMAJOR_0 || cminor < FIRST_CMINOR || cminor > LAST_CMINOR) {
		return (ENXIO);
	}
	cminor = FREE_CMINOR;
	spin_lock_bh(&ip_lock);
	for (; *ipp; ipp = &(*ipp)->next) {
		if (cmajor != (*ipp)->cmajor)
			break;
		if (cmajor == (*ipp)->cmajor) {
			if (cminor < (*ipp)->cminor)
				break;
			if (cminor == (*ipp)->cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= IP_CMAJORS || !(cmajor = ip_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= IP_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_bh(&ip_lock);
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(ip = ip_alloc_priv(q, ipp, cmajor, cminor, crp))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		spin_unlock_bh(&ip_lock);
		return (ENOMEM);
	}
	/* Create all but raw sockets at open time.  For raw sockets, we do not know the protocol to create until the
	   socket is bound to a protocol.  For all others, the protocol is known and the socket is created so that we
	   can accept options management on unbound sockets. */
	spin_unlock_bh(&ip_lock);
	if ((err = ip_sock_init(ip)) < 0) {
		ptrace(("%s: ERROR: from ip_sock_init %d\n", DRV_NAME, -err));
		ip_free_priv(q);
		return (-err);
	}
	qprocson(q);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int ip_close(queue_t *q, int flag, cred_t *crp)
{
	ip_t *ip = PRIV(q);
	(void) flag;
	(void) crp;
	(void) ip;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, ip->cmajor, ip->cminor));
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer", DRV_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	ip_free_priv(q);
	goto quit;
      quit:
	return (0);
}

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
MODULE_PARM_DESC(modid, "Module ID for the IP driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the IP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw ip_cdev = {
	.d_name = DRV_NAME,
	.d_str = &ip_info,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int ip_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&ip_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int ip_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&ip_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int ip_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &ip_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int ip_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit ipterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (ip_majors[mindex]) {
			if ((err = ip_unregister_strdev(ip_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME, ip_majors[mindex]);
			if (mindex)
				ip_majors[mindex] = 0;
		}
	}
	if ((err = ip_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init ipinit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = ip_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		ipterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = ip_register_strdev(ip_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME, ip_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d", DRV_NAME, err);
				ip_terminate();
				return (err);
			}
		}
		if (ip_majors[mindex] == 0)
			ip_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(ip_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = ip_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(ipinit);
module_exit(ipeterminate);

#endif				/* LINUX */
