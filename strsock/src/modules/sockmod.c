/*****************************************************************************

 @(#) $RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/03 13:53:18 $

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

 Last Modified $Date: 2006/10/03 13:53:18 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/03 13:53:18 $"

static char const ident[] =
    "$RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/03 13:53:18 $";

/*
 *  SOCKMOD - A socket module for Linux Fast-STREAMS.
 *
 *  Discussion: There are three approaches to providing sockets with STREAMS as follows:
 *
 *  1. Provide a library (libsocket(3)) and cooperating sockmod(4) module much in the same manner as
 *     the libxnet(3) library and the timod(4) module.  This is what I call the SVR 3.2 approach.
 *     Much of the translation between the sockets API and the underlying TPI provider is performed
 *     by the library.  The library uses ioctl(2) and getmsg(2)/putmsg(2) calls to effect the
 *     translation.
 *
 *     When a socket is opened using the socket(3), socketpair(3) or accept(3) subroutines, the
 *     library checks the /etc/netconfig file and determines a device path name to open the
 *     corresponding TPI device.  The sockmod(4) module is pushed and then the library communicates
 *     with the TPI device using normal TPI messagein (getmsg(2)/putmsg(2)) and a few special
 *     sockmod(4) ioctl(2) calls.
 *
 *     This mechanism is rather inefficient and presents many challenges with regard to state
 *     synchronization.  This approach is obsolete.  This version of sockmod(4) supports the SI_
 *     ioctl(2) commands for backward compatibility (to what??) only.
 *
 *  2. Provide a library (libsocket(3)) and cooperating sockmod(4) module.
 *
 *     When a socket is opened using the socket(3) or socketpair(3) library subroutines, the library
 *     checks the /etc/netconfig file and determines the TPI device to open and then pushes the
 *     sockmod(4) module on that device.  Pushing the sockmod(4) module transforms the Stream into a
 *     socket (by acquiring an inode from the sockfs(3) and transforming the open file descriptor).
 *     The transformed file descriptor represents a file within the sockfs(5) filesystem, and under
 *     Linux, is a true socket, at least down to the struct socket.  An ioctl(2) on the freshly
 *     created socket informs.  All other socket API calls use the system call interface.
 *
 *     If the sockmod(4) module is ever popped, the smod_close() transforms the socket back into a
 *     Stream before the module is popped.
 *
 *  3. Provide a library (libsocket(3)) and cooperating socksys(4) driver.
 *     
 *     When a socket is opened using the socket(3) or socketpair(3) library subroutines, the library
 *     opens the socksys(4) driver and uses the SIOCSOCKSYS input-output control with the SO_SOCKET
 *     or SO_SOCKPAIR command to open a socket (or pair of sockets) of the appropriate type.  The
 *     socksys(4) driver knows what driver to open internally according to the population of an
 *     internal mapping table performed from user space during system intialization frrom the
 *     /etc/sock2path file with the SIOCPROTO and SIOCXPROTO input-output controls.  The returned
 *     file descriptors represent files within the sockfs(5) filesystem, and under Linux, are true
 *     sockets, at least down to the struct socket.  All other socket API calls use the system call
 *     interface.
 */
#include <sys/os7/compat.h>
#include <linux/socket.h>

/*
   These are for TPI definitions 
 */
#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif
#if defined HAVE_SOCKMOD_H
#   include <sockmod.h>
#else
#   include <sys/sockmod.h>
#endif

#define SMOD_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SMOD_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SMOD_REVISION	"OpenSS7 $RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/03 13:53:18 $"
#define SMOD_DEVICE	"SVR 3.2 STREAMS Socket Module for TPI Devices (SOCKMOD)"
#define SMOD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SMOD_LICENSE	"GPL"
#define SMOD_BANNER	SMOD_DESCRIP	"\n" \
			SMOD_COPYRIGHT	"\n" \
			SMOD_REVISION	"\n" \
			SMOD_DEVICE	"\n" \
			SMOD_CONTACT
#define SMOD_SPLASH	SMOD_DEVICE	" - " \
			SMOD_REVISION

#ifdef LINUX
MODULE_AUTHOR(SMOD_CONTACT);
MODULE_DESCRIPTION(SMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(SMOD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SMOD_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sockmod");
#endif
#endif				/* LINUX */

#ifndef SMOD_MOD_NAME
#define SMOD_MOD_NAME		"sockmod"
#endif

#ifndef SMOD_MOD_ID
#define SMOD_MOD_ID		0
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		SMOD_MOD_ID
#define MOD_NAME	SMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SMOD_BANNER
#else				/* MODULE */
#define MOD_BANNER	SMOD_SPLASH
#endif				/* MODULE */

static struct module_info smod_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

static struct module_stat smod_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat smod_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

static streamscall int smod_open(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int smod_close(queue_t *, int, cred_t *);

static streamscall int smod_rput(queue_t *, mblk_t *);
static streamscall int smod_wput(queue_t *, mblk_t *);

static struct qinit smod_rinit = {
	.qi_putp = smod_rput,		/* Read put (message from below) */
	.qi_qopen = smod_open,		/* Each open */
	.qi_qclose = smod_close,	/* Last close */
	.qi_minfo = &smod_minfo,	/* Information */
	.qi_mstat = &smod_rstat,	/* Statistics */
};

static struct qinit smod_winit = {
	.qi_putp = smod_wput,		/* Write put (message from above) */
	.qi_minfo = &smod_minfo,	/* Information */
	.qi_mstat = &smod_wstat,	/* Statistics */
};

MODULE_STATIC struct streamtab sockmodinfo = {
	.st_rdinit = &smod_rinit,	/* Upper read queue */
	.st_wrinit = &smod_winit,	/* Upper write queue */
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Private Datastructure ctors and dtors
 *  
 *  -------------------------------------------------------------------------
 */
struct smod {
	queue_t *rq;			/* module read queue */
	queue_t *wq;			/* module write queue */
	queue_t *hq;			/* stream head read queue */
	ulong state;			/* module state */
	ulong oldstate;			/* module state */
	ulong flags;			/* module flags */
	mblk_t *iocblk;			/* ioctl being processed */
	ulong qlen;			/* bind ack queue length */
	ulong cons;			/* outstanding connection indications */
};

#define SMOD_PRIV(__q) ((struct smod *)((__q)->q_ptr))

/*
 *  -------------------------------------------------------------------------
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  -------------------------------------------------------------------------
 */
static kmem_cache_t *smod_priv_cachep = NULL;
static __unlikely int
smod_init_caches(void)
{
	if (!smod_priv_cachep
	    && !(smod_priv_cachep =
		 kmem_cache_create(MOD_NAME, sizeof(struct smod), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_WARN, "%s: %s: Cannot allocate smod_priv_cachep", MOD_NAME,
			__FUNCTION__);
		return (-ENOMEM);
	}
	return (0);
}

static __unlikely int
smod_term_caches(void)
{
	if (smod_priv_cachep) {
		if (kmem_cache_destroy(smod_priv_cachep)) {
			cmn_err(CE_WARN, "%s: %s: did not destroy smod_priv_cachep", MOD_NAME,
				__FUNCTION__);
			return (-EBUSY);
		}
	}
	return (0);
}

static struct smod *
smod_alloc_priv(queue_t *q)
{
	struct smod *priv;

	if ((priv = kmem_cache_alloc(smod_priv_cachep, SLAB_ATOMIC))) {
		bzero(priv, sizeof(*priv));
		priv->rq = q;
		priv->wq = WR(q);
		priv->hq = q->q_next;
		priv->state = TS_UNBND;	/* assume we are in the correct initial state */
		priv->oldstate = -1UL;
		priv->flags = 0;
		priv->qlen = 0;
		priv->cons = 0;
		/* we are a module with no service routine so our hiwat, lowat shouldn't matter;
		   however, our minpsz, maxpsz do because we are the first queue under the stream
		   head.  We do not want to alter the characteristics of the transport packet sizes
		   so we copy them here. This will allow the stream head to exhibit the same
		   behaviour as before we were pushed. */
		priv->wq->q_minpsz = priv->wq->q_next->q_minpsz;
		priv->wq->q_maxpsz = priv->wq->q_next->q_maxpsz;
		q->q_ptr = WR(q)->q_ptr = priv;
	}
	return (priv);
}

static void
smod_free_priv(queue_t *q)
{
	struct smod *priv;

	if ((priv = (typeof(priv)) q->q_ptr)) {
		q->q_ptr = WR(q)->q_ptr = NULL;
		priv->rq = NULL;
		priv->wq = NULL;
		priv->hq = NULL;
		priv->state = -1UL;
		priv->oldstate = -1UL;
		priv->flags = 0;
		priv->qlen = 0;
		priv->cons = 0;
		if (priv->iocblk)
			freemsg(xchg(&priv->iocblk, NULL));
		kmem_cache_free(smod_priv_cachep, priv);
	}
	return;
}

static __hot_put int
split_buffer(mblk_t *mp, int offset)
{
	unsigned char *ptr = mp->b_rptr + offset;

	if (ptr > mp->b_wptr)
		return (-EINVAL);
	if (ptr < mp->b_wptr) {
		mblk_t *dp;

		if ((dp = copyb(mp))) {
			dp->b_datap->db_type = M_DATA;
			dp->b_rptr += offset;
			mp->b_datap->db_type = M_PROTO;
			mp->b_wptr = ptr;
			linkb(mp, dp);
			return (0);
		}
		return (-ENOSR);
	}
	mp->b_datap->db_type = M_PROTO;
	return (0);
}

static streamscall __hot_get int
smod_rput(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 *  smod_wput_slow - slow write put procedure
 *  @q: the write queue
 *  @mp: the message to put
 */
static noinline streams_fastcall __unlikely int
smod_wput_slow(queue_t *q, mblk_t *mp)
{
	struct smod *priv = q->q_ptr;

#if defined LIS
	/* LiS has this nasty bug where it breaks a STREAMS-based pipe in two _before_ popping
	   modules and, of course, does not properly suppress queue procedures while closing.  We
	   can check this as many times as we would like on SMP and the q->q_next pointer could be
	   invalidate immediately after we check it.  Never use LiS. */
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	switch (mp->b_datap->db_type) {
		union T_primitives *p;
		struct iocblk *ioc;
		mblk_t *dp;
		int err;

	case M_IOCTL:
		/* Most of the ioctls provided here are to acheive atomic and thread-safe
		   operations on the stream for use by the Sockets library.  Each ioctl takes a TPI
		   message in the buffer and results in sending the TPI message downstream.  We
		   strip off and keep the io control block for latter response correlation.  We also 
		   track the state of the stream and the number of outstanding connection
		   indications. */
		ioc = (typeof(ioc)) mp->b_rptr;
		err = -EFAULT;
		if (!(dp = unlinkb(mp)))
			goto error;
		dp->b_datap->db_type = (ioc->ioc_cmd == SI_GETUDATA) ? M_PCPROTO : M_PROTO;
		p = (typeof(p)) dp->b_rptr;
		err = -EINVAL;
#if defined LIS
		/* LiS does not implement the SVR 4 Stream head correctly.  It is the
		   responsibility of the Stream head, according to the SVR 4.2 SPG, to convert
		   legacy character device, file, terminal and socket intput-output controls to
		   I_STR input-output controls.  When the Stream head works correctly (as it does
		   for Linux Fast-STREAMS) we will never receive a TRANPARENT ioctl here. */
		if (ioc->ioc_count == TRANSPARENT)
			return smod_ioctl_slow(q, mp);
#else				/* defined LIS */
		if (ioc->ioc_count == TRANSPARENT)
			goto error;
#endif				/* defined LIS */
		switch (ioc->ioc_cmd) {
#ifdef O_SI_OPTMGMT
		case O_SI_OPTMGMT:
			err = TNOTSUPPORT;
			goto error;
#endif				/* O_SI_OPTMGMT */
#ifdef SI_OPTMGMT
		case SI_OPTMGMT:
			if (p->type == T_OPTMGMT_REQ && ioc->ioc_count >= sizeof(p->optmgmt_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
#ifdef TS_WACK_OPTREQ
				priv->state = TS_WACK_OPTREQ;
#endif
				break;
			}
			goto error;
#endif				/* SI_OPTMGMT */
#ifdef O_SI_BIND
		case O_SI_BIND:
			err = TNOTSUPPORT;
			goto error;
#endif				/* O_SI_BIND */
#if defined SI_BIND
		case SI_BIND:
#endif				/* defined SI_BIND */
		case SI_LISTEN:
			if (p->type == T_BIND_REQ && ioc->ioc_count >= sizeof(p->bind_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_BREQ;
				break;
			}
			goto error;
		case O_SI_GETUDATA:
			err = TNOTSUPPORT;
			goto error;
		case SI_GETUDATA:
			if (p->type == T_INFO_REQ && ioc->ioc_count >= sizeof(p->info_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				break;
			}
			goto error;
#if defined SI_ACCEPT
		case SI_ACCEPT:
#endif
		case SI_SETMYNAME:
			if (p->type == T_CONN_RES && ioc->ioc_count >= sizeof(p->conn_res)) {
				int doff = sizeof(p->conn_res);

				if (p->conn_res.OPT_length
				    && doff < p->conn_res.OPT_offset + p->conn_res.OPT_length)
					doff = p->conn_res.OPT_offset + p->conn_res.OPT_length;
				if ((err = split_buffer(dp, doff)) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_CRES;
				break;
			}
			if (p->type == T_DISCON_REQ && ioc->ioc_count >= sizeof(p->discon_req)) {
				if ((err = split_buffer(dp, sizeof(p->discon_req))) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_DREQ7;
				break;
			}
			goto error;
#if defined SI_CONNECT
		case SI_CONNECT:
#endif
		case SI_SETPEERNAME:
			if (p->type == T_CONN_REQ && ioc->ioc_count >= sizeof(p->conn_req)) {
				int doff = sizeof(p->conn_req);

				if (p->conn_req.OPT_length
				    && doff < p->conn_req.OPT_offset + p->conn_req.OPT_length)
					doff = p->conn_req.OPT_offset + p->conn_req.OPT_length;
				if (p->conn_req.DEST_length
				    && doff < p->conn_req.DEST_offset + p->conn_req.DEST_length)
					doff = p->conn_req.DEST_offset + p->conn_req.DEST_length;
				if ((err = split_buffer(dp, doff)) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_CREQ;
				break;
			}
			if (p->type == T_DISCON_REQ && ioc->ioc_count >= sizeof(p->discon_req)) {
				if ((err = split_buffer(dp, sizeof(p->discon_req))) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_DREQ7;
				break;
			}
			goto error;
		case SI_SHUTDOWN:
		case SI_GETINTRANSIT:
		case SI_TCL_LINK:
		case SI_TCL_UNLINK:
		case SI_SOCKPARAMS:
		case SIOCATMARK:
		case SIOCSPGRP:
		case SIOCGPGRP:
#if 0
		case SIOCHIWAT:
		case SIOGHIWAT:
		case SIOCLOWAT:
		case SIOGLOWAT:
		case SIOCPROTO:
		case SIOCGETNAME:
		case SIOCGETPEER:
		case SIOCXPROTO:
		case SIOCSOCKSYS:
		{
			/* First step is to collect the command and the file descriptor, then the
			   arguments once the command is known. */
			mp->b_datap->db_type = M_COPYIN;
			goto error;
		}
		case FIOCGPGRP:
		case FIOCSPGRP:
		case FIONREAD:
		case FIONBIO:
		case TIOCINQ:
		case TIOOUTQ:
#endif
		default:
			break;
		      error:
			return (err);
		}
		break;
#if 0
	case M_IOCDATA:
	{
		struct copyresp *cp = (typeof(cp)) mp->b_rptr;

		switch (cp->cp_cmd) {
		case SIOCSOCKSYS:
			if (cp->cp_priv == NULL) {
				/* First copyin response. */
			} else {
				/* Subsequent copyin response. */
				switch ((int) (long) (cp->cp_priv->b_prev)) {
				case 1:	/* second copyin response */
					/* Now we have the argument list. */
					switch (*(cp->cp_priv->b_datap->db_base)) {
					case SO_SOCKET:
					case SO_ACCEPT:
					case SO_BIND:
					case SO_CONNECT:
					case SO_GETPEERNAME:
					case SO_GETSOCKNAME:
					case SO_LISTEN:
					case SO_RECV:
					case SO_RECVFROM:
					case SO_SEND:
					case SO_SENDTO:
					case SO_SETSOCKOPT:
					case SO_SHUTDOWN:
					case SO_GETIPDOMAIN:
					case SO_SETIPDOMAIN:
					case SO_SETREUID:
					case SO_SETREGID:
					case SO_GETTIME:
					case SO_SETTIME:
					case SO_GETITIMER:
					case SO_SETITIMER:
					case SO_SELECT:
					case SO_ADJTIME:
					case SO_RECVMSG:
					case SO_SENDMSG:
					case SO_SOCKPAIR:
					default:
					}
					goto error;
				case 2:	/* third copyin/copyout response */
				case 3:	/* forth copyin/copyout response */
				case 4:	/* fifth copyin/copyout response */
				}
			}
		}
		break;
	}
#endif
	default:
		/* Unrecongnized STREAMS messages are simply passed along. Because we have no
		   service procedure, flow control is never performed by the module when passing
		   messages along. */
		break;
	}
	putnext(q, mp);
	return (0);
}

static streamscall __hot_out int
smod_wput(queue_t *q, mblk_t *mp)
{
	union T_primitives *p;

#if defined LIS
	/* LiS has this nasty bug where it breaks a STREAMS-based pipe in two _before_ popping
	   modules and, of course, does not properly suppress queue procedures while closing.  We
	   can check this as many times as we would like on SMP and the q->q_next pointer could be
	   invalidate immediately after we check it.  Never use LiS. */
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	/* fast path for data */
	if (unlikely(mp->b_datap->db_type != M_PROTO))
		goto go_slow;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->type)))
		goto go_slow;
	p = (typeof(p)) mp->b_rptr;
	/* conveniently none of these are > 31 */
	if (unlikely(!((1 << p->type)
		       & ((1 << T_UNITDATA_REQ) | (1 << T_EXDATA_REQ) | (1 << T_DATA_REQ)))))
		goto go_slow;
	putnext(q, mp);
	return (0);

      go_slow:
	return smod_wput_slow(q, mp);
}

#define SOCKMOD_HANGUP 01
#define SOCKMOD_EPROTO 02

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */

/*
   LiS does not offer us a way to wait for an allocation.  Solaris, OSF and Linux Fast-STREAMS do. 
 */
#if !defined BPRI_WAITOK
#   if defined BPRI_FT
#	define BPRI_WAITOK BPRI_FT
#   else
#	define BPRI_WAITOK BPRI_HI
#   endif
#endif

static __unlikely void
smod_pop(queue_t *q)
{
	struct smod *priv = (typeof(priv)) q->q_ptr;
	mblk_t *mp;

	switch (priv->state) {
	case TS_WREQ_ORDREL:
		if (!(priv->flags & SOCKMOD_EPROTO)) {
			if ((mp = allocb(sizeof(struct T_ordrel_req), BPRI_WAITOK))) {
				struct T_ordrel_req *prim = (typeof(prim)) mp->b_wptr;

				mp->b_wptr = (unsigned char *) (prim + 1);
				mp->b_datap->db_type = M_PROTO;
				prim->PRIM_type = T_ORDREL_REQ;
				qreply(q, mp);
			}
		}
		/* fall through */
	case TS_DATA_XFER:
		if ((mp = allocb(sizeof(struct T_discon_req), BPRI_WAITOK))) {
			struct T_discon_req *prim = (typeof(prim)) mp->b_wptr;

			mp->b_wptr = (unsigned char *) (prim + 1);
			mp->b_datap->db_type = M_PROTO;
			prim->PRIM_type = T_DISCON_REQ;
			prim->SEQ_number = 0;
			qreply(q, mp);
		}
		break;
	case TS_IDLE:
	default:
		break;
	}
	if ((priv->flags & SOCKMOD_EPROTO)) {
		if ((mp = allocb(2, BPRI_WAITOK))) {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = 0;
			*(mp->b_wptr)++ = 0;
			qreply(q, mp);
		}
#	    if defined M_ERROR_UNDOES_M_HANGUP
		priv->flags &= ~(SOCKMOD_EPROTO | SOCKMOD_HANGUP);
#	    else		/* defined M_ERROR_UNDOES_M_HANGUP */
		priv->flags &= ~(SOCKMOD_EPROTO);
#	    endif		/* defined M_ERROR_UNDOES_M_HANGUP */
	}
#   if defined M_UNHANGUP
	if ((priv->flags & SOCKMOD_HANGUP)) {
		if ((mp = allocb(0, BPRI_WAITOK))) {
			mp->b_datap->db_type = M_UNHANGUP;
			qreply(q, mp);
		}
		priv->flags &= ~SOCKMOD_HANGUP;
	}
#   endif			/* defined M_UNHANGUP */
}

/* Approaches: how about creating a new socket in the sockfs(5) for hacked address family AF_STREAMS
 * and then reset the address famliy in the socket after it is created?   Maybe even a slimmer
 * version of that which creates only the sockfs inode and then uses the stream head instead of a
 * struct sock? */

#if 0
static int sock_tpi_release(struct socket *);
static int sock_tpi_bind(struct socket *, struct sockaddr *, int);
static int sock_tpi_connect(struct socket *, struct sockaddr *, int, int);
static int sock_tpi_socketpair(struct socket *, struct socket *);
static int sock_tpi_accept(struct socket *, struct socket *, int);
static int sock_tpi_getname(struct socket *, struct sockaddr *, int *, int);
static unsigned int sock_tpi_poll(struct file *, struct socket *, struct poll_table_struct *);
static int sock_tpi_ioctl(struct socket *, unsigned int, unsigned long);

#ifdef HAVE_KMEMB_PROTO_OPS_COMPAT_IOCTL
static int sock_tpi_compat_ioctl(struct socket *, unsigned int, unsigned long);
#endif				/* HAVE_KMEMB_PROTO_OPS_COMPAT_IOCTL */

static int sock_tpi_listen(struct socket *, int);
static int sock_tpi_shutdown(struct socket *, int);
static int sock_tpi_setsockopt(struct socket *, int, int, char __user *, int);

#ifdef HAVE_KMEMB_PROTO_OPS_COMPAT_SETSOCKOPT
static int sock_tpi_compat_setsockopt(struct socket *, int, int, char __user *, int);
#endif				/* HAVE_KMEMB_PROTO_OPS_COMPAT_SETSOCKOPT */

static int sock_tpi_getsockopt(struct socket *, int, int, char __user *, int __user *);

#ifdef HAVE_KMEMB_PROTO_OPS_COMPAT_GETSOCKOPT
static int sock_tpi_compat_getsockopt(struct socket *, int, int, char __user *, int __user *);
#endif				/* HAVE_KMEMB_PROTO_OPS_COMPAT_GETSOCKOPT */

#ifndef HAVE_KTYPE_STRUCT_KIOCB
/* These are 2.6.17 socket operations: */
static int sock_tpi_sendmsg(struct kiocb *iocb, struct socket *, struct msghdr *, size_t);
static int sock_tpi_recvmsg(struct kiocb *iocb, struct socket *, struct msghdr *, int);
#else
/* These are 2.4.20 socket operations: */
static int sock_tpi_sendmsg(struct socket *, struct msghdr *, int, struct scm_cookie *);
static int sock_tpi_recvmsg(struct socket *, struct msghdr *, int, int, struct scm_cookie *);
#endif

static int sock_tpi_mmap(struct file *, struct socket *, struct vm_area_struct *);
static int sock_tpi_sendpage(struct socket *, struct page *, int, size_t, int);

struct proto_ops tpi_socket_ops = {
	.family = 0,		/* huh? what a bad place to put this */
#ifdef HAVE_KMEMB_STRUCT_PROTO_OPS_OWNER
	.owner = THIS_MODULE,
#endif				/* HAVE_KMEMB_STRUCT_PROTO_OPS_OWNER */
	.release = &sock_tpi_release,
	.bind = &sock_tpi_bind,
	.connect = &sock_tpi_connect,
	.socketpair = &sock_tpi_socketpair,
	.accept = &sock_tpi_accept,
	.getname = &sock_tpi_getname,
	.poll = &sock_tpi_poll,
	.ioctl = &sock_tpi_ioctl,
#ifdef HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_IOCTL
	.compat_ioctl = &sock_tpi_compat_ioctl,
#endif				/* HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_IOCTL */
	.listen = &sock_tpi_listen,
	.shutdown = &sock_tpi_shutdown,
	.setsockopt = &sock_tpi_setsockopt,
	.getsockopt = &sock_tpi_getsockopt,
#ifdef HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_GETSOCKOPT
	.compat_setsockopt = &sock_tpi_compat_setsockopt,
#endif				/* HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_GETSOCKOPT */
#ifdef HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_SETSOCKOPT
	.compat_getsockopt = &sock_tpi_compat_getsockopt,
#endif				/* HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_SETSOCKOPT */
	.sendmsg = &sock_tpi_sendmsg,
	.recvmsg = &sock_tpi_recvmsg,
	.mmap = &sock_tpi_mmap,
	.sendpage = &sock_tpi_sendpage,
};
#endif

/**
 * smod_open - open the sockmod module
 * @q: read queue
 * @devp: device to open
 * @flag: flags to open call
 * @sflag: should be just MODOPEN
 * @crp: pointer to opening process' credentials
 *
 * Some applications programs that are too knowledgable about the internal organization of STREAMS
 * have the habit of popping the timod module and pushing the sockmod module and then popping
 * sockmod and pushing timod.  Therefore, it cannot be assumed that timod is being pushed onto a
 * freshly opened Stream as is the case when the XTI library is pushing the module.  Therefore, we
 * should immediately issue a capability request to determine the state of the TPI Stream.
 *
 * It is possible under Linux-Fast STREAMS that we can just transform the Stream into a socket.
 * This can be accomplished most easily by setting the inode to look like a socket (place a struct
 * socket inside of it and set i_sock) and implement the socket.ops as above.
 *
 * The trick here is when sockmod is pushed to transform the Stream head into a socket (from the
 * filesystem and system call perspective).  This can be accomplished most eaily in a similar manner
 * to the way that Streams are linked under a multiplexing driver: the procedures associated with
 * the file operations are replaced with socket procedures.  Also, the associated inode is changed
 * to appear the same as a socket inode.  This is best accomplished by allocating a new inode from
 * the sockfs.  The stream head can be referenced with the f_private data pointer.  The sockmod
 * instance can reference the new inode with the q->q_ptr.
 */
static streamscall int
smod_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err = 0;

	if (q->q_ptr != NULL)
		goto quit;	/* already open */
	err = ENXIO;
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		goto quit;	/* can't be opened as driver */
	err = ENOMEM;
	if (!(smod_alloc_priv(q)))
		goto quit;
#if 0
	/* generate immediate information request */
	if ((err = t_info_req(q)) < 0) {
		smod_free_priv(q);
		return (-err);
	}
#endif
	qprocson(q);
	return (0);
      quit:
	return (err);
}

static streamscall int
smod_close(queue_t *q, int oflag, cred_t *crp)
{
	(void) oflag;
	(void) crp;
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", MOD_NAME, __FUNCTION__);
		goto quit;
	}
	/* LiS has this nasty bug where it breaks a STREAMS-based pipe in two _before_ popping
	   modules and, of course, does not properly suppress queue procedures while closing.  We
	   can check this as many times as we would like on SMP and the q->q_next pointer could be
	   invalidate immediately after we check it.  Never use LiS. */
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	smod_pop(q);
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	smod_free_priv(q);
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

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SOCKMOD module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sockmod_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sockmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC __unlikely int
smod_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&sockmod_fmod)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
smod_unregister_strmod(void)
{
	int err;

	if ((err = unregister_strmod(&sockmod_fmod)) < 0)
		return (err);
	return (0);
}

#if defined WITH_32BIT_CONVERSION
struct sockmod_trans {
	unsigned int cmd;
	void *opaque;
};

STATIC sockmod_trans sockmod_trans_map[] = {
	{.cmd = O_SI_GETUDATA,}
	, {.cmd = SI_SHUTDOWN,}
	, {.cmd = SI_LISTEN,}
	, {.cmd = SI_SETMYNAME,}
	, {.cmd = SI_SETPEERNAME,}
	, {.cmd = SI_GETINTRANSIT,}
	, {.cmd = SI_TCL_LINK,}
	, {.cmd = SI_TCL_UNLINK,}
	, {.cmd = SI_SOCKPARAMS,}
	, {.cmd = SI_GETUDATA,}
	, {.cmd = SIOCHIWAT,}
	, {.cmd = SIOGHIWAT,}
	, {.cmd = SIOCLOWAT,}
	, {.cmd = SIOGLOWAT,}
	, {.cmd = SIOCATMARK,}
	, {.cmd = SIOCGPGRP,}
	, {.cmd = SIOCSPGRP,}
	, {.cmd = SIOCPROTO,}
	, {.cmd = SIOCGETNAME,}
	, {.cmd = SIOCGETPEER,}
	, {.cmd = SIOCXPROTO,}
	, {.cmd = SIOCSOCKSYS,}
	, {.cmd = FIOCGPGRP,}
	, {.cmd = FIOCSPGRP,}
	, {.cmd = FIONREAD,}
	, {.cmd = FIONBIO,}
	, {.cmd = TIOCINQ,}
	, {.cmd = TIOOUTQ,}
	, {.cmd = 0,}
};

STATIC __unlikely void
smod_ioctl32_unregister(void)
{
	struct sockmod_trans *t;

	for (t = sockmod_trans_map; t->cmd != 0; t++) {
		streams_unregister_ioctl32(t->opaque);
		t->opaque = NULL;
	}
	return;
}

STATIC __unlikely int
smod_ioctl32_register(void)
{
	struct sockmod_trans *t;

	for (t = sockmod_trans_map; t->cmd != 0; t++) {
		if ((t->opaque = streams_register_ioctl32(t->cmd)) == NULL) {
			smod_ioctl32_unregister();
			return (-ENOMEM);
		}
	}
	return (0);
}

#endif				/* defined WITH_32BIT_CONVERSION */

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC __unlikely int
smod_register_strmod(void)
{
	int err;

	if ((err = lis_register_strmod(&sockmodinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC __unlikely int
smod_unregister_strmod(void)
{
	int err;

	if ((err = lis_unregister_strmod(&sockmodinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sockmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = smod_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
#if defined WITH_32BIT_CONVERSION
	if ((err = smod_ioctl32_register())) {
		cmn_err(CE_WARN, "%s: could not register 32bit ioctls, err = %d", MOD_NAME, err);
		smod_term_caches();
		return (err);
	}
#endif				/* defined WITH_32BIT_CONVERSION */
	if ((err = smod_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
#if defined WITH_32BIT_CONVERSION
		smod_ioctl32_unregister();
#endif				/* defined WITH_32BIT_CONVERSION */
		smod_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sockmodterminate(void)
{
	int err;

	if ((err = smod_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
#if defined WITH_32BIT_CONVERSION
	smod_ioctl32_unregister();
#endif				/* defined WITH_32BIT_CONVERSION */
	if ((err = smod_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sockmodinit);
module_exit(sockmodterminate);

#endif				/* LINUX */
