/*****************************************************************************

 @(#) $RCSfile: strxnet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/04/22 12:08:33 $

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

 Last Modified $Date: 2004/04/22 12:08:33 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strxnet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/04/22 12:08:33 $"

static char const ident[] = "$RCSfile: strxnet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/04/22 12:08:33 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/smp_lock.h>
#include <linux/slab.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include <tihdr.h>

#include "strdebug.h"
//#include "strxnet.h"		/* extern verification */
#include "strreg.h"

#define XNET_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define XNET_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define XNET_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/04/22 12:08:33 $"
#define XNET_DEVICE	"SVR 4.2 Sockets Library NET4 Support"
#define XNET_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define XNET_LICENSE	"GPL"
#define XNET_BANNER	XNET_DESCRIP	"\n" \
			XNET_COPYRIGHT	"\n" \
			XNET_REVISION	"\n" \
			XNET_DEVICE	"\n" \
			XNET_CONTACT	"\n"
#define XNET_SPLASH	XNET_DEVICE	" - " \
			XNET_REVISION	"\n"

MODULE_AUTHOR(XNET_CONTACT);
MODULE_DESCRIPTION(XNET_DESCRIP);
MODULE_SUPPORTED_DEVICE(XNET_DEVICE);
MODULE_LICENSE(XNET_LICENSE);

#ifndef CONFIG_STREAMS_XNET_NAME
#define CONFIG_STREAMS_XNET_NAME "xnet"
//#error "CONFIG_STREAMS_XNET_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_XNET_MAJOR
#define CONFIG_STREAMS_XNET_MAJOR 0
//#error "CONFIG_STREAMS_XNET_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_XNET_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for STREAMS NET4 driver (0 for allocation).");

typedef struct xnet_protosw {
	struct list_head list;
	struct {
		uint family;
		uint type;
		uint protocol;
	} prot;
	struct T_info_ack info;
	/* need functions for converting options */
//	int (*getops) ();
//	int (*setops) ();
} xnet_protosw_t;

struct xnet_protosw *xnet_protosw[MAX_CHRDEV + 1] = { NULL, };

static int xnet_rsrv(queue_t *q)
{
	struct xnet *xnet = q->q_ptr;
	mblk_t *mp;
	while ((mp = getq(q))) {
		switch (mp->b_datap->db_type) {
		case M_PCPROTO:
		case M_PROTO:
			switch (*(ulong *) mp->b_rptr) {
			case T_CONN_IND:
			case T_CONN_CON:
			case T_DISCON_IND:
			case T_DATA_IND:
			case T_EXDATA_IND:
			case T_INFO_ACK:
			case T_BIND_ACK:
			case T_ERROR_ACK:
			case T_OK_ACK:
			case T_UNITDATA_IND:
			case T_UDERROR_IND:
			case T_OPTMGMT_ACK:
			case T_ORDREL_IND:
			default:
				break;
			}
		case M_DATA:
		case M_DELAY:
			break;
		case M_RSE:
		case M_PCRSE:
		{
			/* socket underwent state change */
			struct inet_event *e = (typeof(e)) mp->b_rptr;
			int oldstate = xchg(&xnet->tcp_state, e->state);
			if (e->state == oldstate)
				break;	/* no state change */
			switch (xnet->p.prot.type) {
			case SOCK_STREAM:
			case SOCK_SEQPACKET:
				switch (xnet->p.info.CURRENT_state) {
				case TS_WCON_CREQ:
					switch (e->state) {
					case TCP_ESTABLISHED:
						t_conn_con(q, &xnet->dst, NULL, NULL);
						break;
					case TCP_TIME_WAIT:
					case TCP_CLOSE:
						t_discon_ind(q, &xnet->dst, T_PROVIDER, 0, NULL,
							     NULL);
						break;
					}
					break;
				case TS_WIND_ORDREL:
					switch (e->state) {
					case TCP_TIME_WAIT:
					case TCP_CLOSE:
						t_ordrel_ind(q);
						break;
					case TCP_FIN_WAIT1:
					case TCP_FIN_WAIT2:
						break;
					}
					break;
				case TS_DATA_XFER:
					switch (e->state) {
					case TCP_CLOSE_WAIT:
						t_ordrel_ind(q);
						break;
					case TCP_TIME_WAIT:
					case TCP_CLOSE:
						t_discon_ind(q, NULL, T_PROVIDER, 0, NULL, NULL);
						break;
					}
					break;
				case TS_WREQ_ORDREL:
					switch (e->state) {
					case TCP_TIME_WAIT:
					case TCP_CLOSE:
						switch (oldstate) {
						case TCP_CLOSE_WAIT:
							t_discon_ind(q, NULL, T_PROVIDER, 0, NULL,
								     NULL);
							break;
						}
						break;
					}
					break;
				case TS_IDLE:
				case TS_WRES_CIND:
					switch (e->state) {
					case TCP_LAST_ACK:
						switch (oldstate) {
						case TCP_CLOSE_WAIT:
							break;
						}
						break;
					case TCP_TIME_WAIT:
					case TCP_CLOSE:
						switch (oldstate) {
						case TCP_FIN_WAIT2:
						case TCP_LAST_ACK:
							break;
						case TCP_LISTEN:
						{
							mblk_t *cp;
							/* state change was on child */
							xnet->tcp_state = TCP_LISTEN;
							/* look for the child */
							if ((cp = t_seq_find(xnet, mp)))
								t_disconn_ind(q, NULL, T_PROVIDER,
									      0, cp, NULL);
							break;
						}
						}
						break;
					case TCP_ESTABLISHED:
						switch (oldstate) {
						case TCP_LISTEN:
							/* state change was on child */
							xnet->tcp_state = TCP_LISTEN;
							xnet_conn_ind(q, mp);
							break;
						}
						break;
					}
					break;
				}
				break;
			case SOCK_DGRAM:
			case SOCK_RAW:
			case SOCK_RDM:
				break;
			}
			break;
		}
		case M_CTL:
		case M_READ:
		{
			/* socket is now writeable */
			struct inet_event *e = (typeof(e)) mp->b_rptr;
			if (xnet->sock) {
				switch (xnet->p.prot.type) {
				case SOCK_STREAM:
				case SOCK_SEQPACKET:
					switch (xnet->p.info.CURRENT_state) {
					case TS_DATA_XFER:
					case TS_WREQ_ORDREL:
						qenable(WR(q));
						break;
					}
					break;
				case SOCK_DGRAM:
				case SOCK_RAW:
				case SOCK_RDM:
					switch (xnet->p.info.CURRENT_state) {
					case TS_IDLE:
						qenable(WR(q));
						break;
					}
					break;
				}
			}
			break;
		}
		}
	}
	freemsg(mp);
	return (0);
}

static int xnet_wsrv(queue_t *q)
{
	struct xnet *xnet = q->q_ptr;
	mblk_t *mp;
	while ((mp = getq(q))) {
		switch (mp->b_datap->db_type) {
		case M_PCPROTO:
		case M_PROTO:
			switch (*(ulong *) mp->b_rptr) {
			case T_CONN_REQ:
			{
				const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
				if (xnet->p.info.SERV_type == T_CLTS)
					goto tnotsupport;
				if (xnet->p.info.CURRENT_state != TS_IDLE)
					goto toutstate;
				if (mp->b_wptr < mp->b_rptr + sizeof(*p) ||
				    mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length ||
				    mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
					goto einval;
				else {
					xnet_addr_t *dst =
					    (typeof(dst)) (mp->b_rptr + p->DEST_offset);
					if (p->DEST_length < sizeof(*dst))
						goto tbadaddr;
					if (dst->sin_port == 0)
						goto tbadaddr;
					if (dst->sin_family != xnet->p.prot.family)
						goto tbadaddr;
					if (ss->cred.cr_uid != 0 &&
					    htons(dst->sin_port) == IPPROTO_RAW)
						goto tacces;
				}
			}
			case T_CONN_RES:
			case T_DISCON_REQ:
			case T_DATA_REQ:
			case T_EXDATA_REQ:
			case T_INFO_REQ:
			case T_BIND_REQ:
			case T_UNBIND_REQ:
			case T_UNITDATA_REQ:
			case T_OPTMGMT_REQ:
			case T_ORDREL_REQ:
			default:
			}
			break;
		case M_DATA:
		case M_DELAY:
		case M_CTL:
		case M_RSE:
		case M_PCRSE:
		case M_READ:
		{
			/* socket is now readible */
			struct inet_event *e = (typeof(e)) mp->b_rptr;
			switch (xnet->p.prot.type) {
			case SOCK_STREAM:
			case SOCK_SEQPACKET:
				switch (xnet->p.info.CURRENT_state) {
				case TS_IDLE:
				case TS_WRES_CIND:
					if (e->state == TCP_LISTEN)
						break;
				case TS_DATA_XFER:
				case TS_WIND_ORDREL:
				case TS_WREQ_ORDREL:
					inet_sock_recvmsg(q);
					break;
				}
				break;
			case SOCK_DGRAM:
			case SOCK_RAW:
			case SOCK_RDM:
				switch (xnet->p.info.CURRENT_state) {
				case TS_IDLE:
					inet_sock_recvmsg(q);
					break;
				}
				break;
			}
			freemsg(mp);
			return (0);
		}
		case M_ERROR:
		{
			/* socket has generated an error */
			struct inet_event *e = (typeof(e)) mp->b_rptr;
			err = sock_error(e->sk);
			if (xnet->sock) {
				switch (xnet->p.prot.type) {
				case SOCK_STREAM:
				case SOCK_SEQPACKET:
					switch (xnet->p.info.CURRENT_state) {
					case TS_IDLE:
						/* FIXME: save error, indicate hangup or something */
						break;
					default:
						break;
					}
					break;
				case SOCK_DGRAM:
				case SOCK_RAW:
				case SOCK_RDM:
					switch (xnet->p.info.CURRENT_state) {
					case TS_IDLE:
						/* FIXME: generate uderr */
						break;
					default:
						break;
					}
					break;
				}
			}
			freemsg(mp);
			return (0);
		}
		}
	}
}

static int xnet_wput(queue_t *q, mblk_t *mp)
{
	struct xnet *xnet = q->q_ptr;
	union ioctypes *ioc;
	int err = 0, rval = 0;
	mblk_t *dp = mp->b_cont;
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
	case M_DATA:
	case M_DELAY:
		/* handle rest in service procedure */
		putq(q, mp);
		return (0);
	case M_FLUSH:
		/* always do M_FLUSH in put procedure */
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			queue_t *rq = RD(q);
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(rq, mp->b_rptr[1], FLUSHALL);
			else
				flushq(rq, FLUSHALL);
			qreply(q, mp);
			return (0);
		}
		break;
	case M_IOCTL:
		/* we can handle M_IOCTL right here in the put procedure because we know that there 
		   cannot be more than one M_IOCTL in progress at any point in time */
		ioc = (typeof(ioc)) mp->b_rptr;
		switch (ioc->iocblk.ioc_cmd) {
		case XNET_ADD:
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0)
				goto nak;
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_addr = *(void **) dp->b_rptr;
				ioc->copyreq.cq_size = sizeof(struct xnet_protosw);
				ioc->copyreq.cq_flag = 0;
				xnet->iocstate = 1;
				qreply(q, mp);
				return (0);
			}
			err = -EFAULT;
			if (!dp || dp->b_wptr < dp->b_rptr + sizeof(struct xnet_protosw))
				goto nak;
			xnet->iocstate = 1;
			goto xnet_add_state1;
		case XNET_DEL:
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0)
				goto nak;
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_addr = *(void **) dp->b_rptr;
				ioc->copyreq.cq_size = sizeof(struct xnet_protosw);
				ioc->copyreq.cq_flag = 0;
				xnet->iocstate = 1;
				qreply(q, mp);
				return (0);
			}
			err = -EFAULT;
			if (!dp || dp->b_wptr < dp->b_rptr + sizeof(struct xnet_protosw))
				xnet->iocstate = 1;
			goto xnet_del_state1;
		}
		err = -EOPNOTSUPP;
		goto nak;
	case M_IOCDATA:
		ioc = (typeof(ioc)) mp->b_rptr;
		switch (ioc->copyresp.cp_cmd) {
		case XNET_ADD:
			err = -(long) ioc->copyresp.cp_rval;
			if (err == 0) {
				struct xnet_protosw *p;
				switch (xnet->iocstate) {
				case 1:
				      xnet_add_state1:
					p = (typeof(p)) dp->b_rptr;
					err = -ENODEV;
					/* allocate and copy */
					/* or better, just save dp */
					if ((err = xnet_add_proto(p, minor)))
						goto nak;
					/* done */
					xnet->iocstate = 0;
					goto ack;
				}
				err = -EIO;
			}
			goto nak;
		case XNET_DEL:
			err = -(long) ioc->copyresp.cp_rval;
			if (err == 0) {
				struct xnet_protosw *p;
				switch (xnet->iocstate) {
				case 1:
				      xnet_del_state1:
					err = -ENOENT;
					if (!(p = xnet_protosw[minor]))
						goto nak;
					if ((err = xnet_del_proto(p, minor)))
						goto nak;
					/* free the block */
					/* done */
					xnet->iocstate = 0;
					goto ack;
				}
				err = -EIO;
			}
			goto nak;
		}
		return (0);
	}
	freemsg(mp);
	return (0);
      nak:
	xnet->iocstate = 0;
	mp->b_datap->db_type = M_IOCNAK;
	ioc->iocblk.ioc_rval = -1;
	ioc->iocblk.ioc_error = -err;
	qreply(q, mp);
	return (0);
      ack:
	xnet->iocstate = 0;
	mp->b_datap->db_type = M_IOCACK;
	ioc->iocblk.ioc_rval = rval;
	ioc->iocblk.ioc_error = 0;
	qreply(q, mp);
	return (0);
}

/* 
 *  Protocol switch CTOS and DTORS
 *  -------------------------------------------------------------------------
 */
static int xnet_add_proto(struct xnet_protosw *p, unsigned short minor)
{
	if (xnet_protosw[minor] != NULL)
		return (-EBUSY);
	xnet_protosw[minor] = p;
	return (0);
}
static int xnet_del_proto(struct xnet_protosw *p, unsigned short minor)
{
	if (xnet_protosw[minor] != p)
		return (-EPERM);
	xnet_protosw[minor] = NULL;
	return (0);
}

static const struct xnet_protosw xnet_protos[] = {
	{{PF_INET, SOCK_RAW, IPPROTO_ICMP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
	{{PF_INET, SOCK_RAW, IPPROTO_IGMP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
	{{PF_INET, SOCK_RAW, IPPROTO_IPIP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
	{{PF_INET, SOCK_STREAM, IPPROTO_TCP},
	 {T_INFO_ACK, T_INVALID, 1, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_COTS_ORD, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
	{{PF_INET, SOCK_RAW, IPPROTO_EGP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
	{{PF_INET, SOCK_RAW, IPPROTO_PUP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
	{{PF_INET, SOCK_DGRAM, IPPROTO_UDP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
	{{PF_INET, SOCK_RAW, IPPROTO_IDP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
	{{PF_INET, SOCK_RAW, IPPROTO_RAW},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID, sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}, NULL, NULL},
};

#define IP_CMINOR 32
static int xnet_init_proto(void)
{
	int i, j, err = 0;
	for (i = IP_CMINOR, j = 0; j < sizeof(xnet_protos) / sizeof(typeof(xnet_protos[0]));
	     i++, j++)
		if ((err = xnet_add_proto(&xnet_protos[j], i))) {
			while (j--)
				xnet_del_proto(&xnet_protos[j], --i);
			return (err);
		}
	return (0);
}
static void xnet_term_proto(void)
{
	int i, j, err = 0;
	for (i = IP_CMINOR, j = 0; j < sizeof(xnet_protos) / sizeof(typeof(xnet_protos[0]));
	     i++, j++)
		err = xnet_del_proto(&xnet_protos[j], i);
	return (err);
}

/* 
 *  Private structure CTORS and DTORS
 *  -------------------------------------------------------------------------
 */
static kmem_cache_t *xnet_priv_cachep = NULL;
static int xnet_init_caches(void)
{
	if (!xnet_priv_cachep &&
	    !(xnet_priv_cachep =
	      kmem_cache_create("xnet_priv_cachep", sizeof(xnet_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xnet_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	}
	return (0);
}
static void xnet_term_caches(void)
{
	if (xnet_priv_cachep) {
		if (kmem_cache_destroy(xchg(&xnet_priv_cachep, NULL)))
			cmn_err(CE_WARN, "%s: did not destroy xnet_priv_cachep", __FUNCTION__);
	}
	return;
}

/* 
 *  OPEN and CLOSE
 *  -------------------------------------------------------------------------
 *  There are a whole bunch of cloneable minors.  Each clonable minor
 *  represents a different socket type.  We also provide a registration
 *  function that permits definitions for new socket types to be added.
 *  Included are definitions for AF_UNIX, AF_INET and AF_INET6.
 */
static int xnet_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct xnet *xnet;
	struct xnet_protosw *proto;
	unsigned short cmajor = getmajor(*devp);
	unsigned short cminor = getminor(*devp);
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next)
		return (EIO);	/* can't push as module */
	if (cmajor != major || cminor < 1 || cminor > 255)
		return (ENXIO);
	if (!(proto = xnet_protosw[cminor]))
		return (ENXIO);	/* no such minor */
	if ((xnet = kmem_cache_alloc(xnet_priv_cachep, SLAB_ATOMIC))) {
		xnet->cmajor = cmajor;
		xnet->cminor = cminor;
		xnet->rq = RD(q);
		xnet->wq = WR(q);
		xnet->cred = *crp;
		xnet->conq = NULL;
		xnet->coninds = 0;
		xnet->p = proto;
		q->q_ptr = WR(q)->q_ptr = xnet;
		return (0);
	}
	return (ENOMEM);
}
static int xnet_close(queue_t *q, int oflag, cred_t *crp)
{
	struct xnet *xnet;
	if ((xnet = q->q_ptr)) {
		while (xnet->conq) {
			freemsg(xchg(&xnet->conq, xnet->conq->b_next));
			xnet->coninds--;
		}
		q->q_ptr = WR(q)->q_ptr = NULL;
		kmem_cache_free(xnet_priv_cachep, xnet);
	}
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */
static struct qinit xnet_rinit = {
	qi_putp:putq,
	qi_srvp:xnet_rsrv,
	qi_qopen:xnet_open,
	qi_qclose:xnet_close,
	qi_minfo:&xnet_minfo,
};

static struct qinit xnet_winit = {
	qi_putp:xnet_wput,
	qi_srvp:xnet_wsrv,
	qi_minfo:&xnet_minfo,
};

static struct streamtab xnet_info = {
	st_rdinit:&xnet_rinit,
	st_wrinit:&xnet_winit,
};

static struct cdevsw xnet_cdev = {
	d_name:CONFIG_STREAMS_XNET_NAME,
	d_str:&xnet_info,
	d_flag:D_CLONE,
	d_fop:NULL,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

static int __init xnet_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO XNET_BANNER);
#else
	printk(KERN_INFO XNET_SPLASH);
#endif
	if ((err = register_strdev(makedevice(major, 0), &xnet_cdev)) < 0)
		return (err);
	if (err > 0)
		major = err;
	return (0);
}
static void __exit xnet_exit(void)
{
	int err;
	dev_t dev = makedevice(major, 0);
	if ((err = unregister_strdev(dev, &xnet_cdev)))
		return (void) (err);
	return (void) (0);
}

module_init(xnet_init);
module_exit(xnet_exit);
