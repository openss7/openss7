/*****************************************************************************

 @(#) $RCSfile: sl_tpi.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:10 $

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

 Last Modified $Date: 2004/08/26 23:38:10 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sl_tpi.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:10 $"

static char const ident[] = "$RCSfile: sl_tpi.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:10 $";

/*
 *  This is a SL/SDT (Signalling Link/Signalling Data Terminal) module which
 *  cam be pushed over a TLI transport to effect an OpenSS7 Signalling Link or
 *  Signalling Data Terminal.
 */
#include "compat.h"

#include <tihdr.h>
// #include <xti.h>
#include <xti_inet.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#define SS7IP_DESCRIP	"SS7/IP SIGNALLING LINK (SL) STREAMS MODULE."
#define SS7IP_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SS7IP_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SS7IP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SS7IP_LICENSE	"GPL"
#define SS7IP_BANNER	SS7IP_DESCRIP	"\n" \
			SS7IP_COPYRIGHT	"\n" \
			SS7IP_DEVICE	"\n" \
			SS7IP_CONTACT
#define SS7IP_SPLASH	SS7IP_DEVICE	" - " \
			SS7IP_REVISION	"\n" \

#ifdef LINUX
MODULE_AUTHOR(SS7IP_CONTACT);
MODULE_DESCRIPTION(SS7IP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SS7IP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SS7IP_LICENSE);
#endif
#endif				/* LINUX */

#ifdef LFS
#define SL_TPI_MOD_ID CONFIG_STREAMS_SL_TPI_MODID
#define SL_TPI_MOD_NAME CONFIG_STREAMS_SL_TPI_NAME
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */
STATIC struct module_info sl_minfo = {
	mi_idnum:SL_TPI_MOD_ID,		/* Module ID number */
	mi_idname:SL_TPI_MOD_NAME,		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC int sl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sl_close(queue_t *, int, cred_t *);

STATIC int sl_rput(queue_t *, mblk_t *);
STATIC int sl_rsrv(queue_t *);

STATIC struct qinit sl_rinit = {
	qi_putp:sl_rput,		/* Read put (msg from below) */
	qi_srvp:sl_rsrv,		/* Read queue service */
	qi_qopen:sl_open,		/* Each open */
	qi_qclose:sl_close,		/* Last close */
	qi_minfo:&sl_minfo,		/* Information */
};
STATIC int sl_wput(queue_t *, mblk_t *);
STATIC int sl_wsrv(queue_t *);

STATIC struct qinit sl_winit = {
	qi_putp:sl_wput,		/* Write put (msg from above) */
	qi_srvp:sl_wsrv,		/* Write queue service */
	qi_minfo:&sl_minfo,		/* Information */
};

STATIC struct streamtab sl_info = {
	st_rdinit:&sl_rinit,		/* Upper read queue */
	st_wrinit:&sl_winit,		/* Upper write queue */
};

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7

/*
 *  =========================================================================
 *
 *  SS7IP-SL Private Structure
 *
 *  =========================================================================
 */

typedef struct sl {
	struct sl *next;		/* list linkage */
	struct sl **prev;		/* list linkage */
	size_t refcnt;			/* structure reference count */
	spinlock_t lock;		/* structure lock */
	ushort cmajor;			/* major device number */
	ushort cminor;			/* minor device number */
	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	spinlock_t qlock;		/* queue lock */
	queue_t *rwait;			/* RD queue waiting on qlock */
	queue_t *wwait;			/* WR queue waiting on qlock */
	uint rbid;			/* rd bufcall id */
	uint wbid;			/* wr bufcall id */
	uint state;			/* interface state */
	uint style;			/* interface style */
	uint version;			/* interface version */
	uint flags;			/* interface flags */
	mblk_t *rbuf;			/* normal data reassembly */
	mblk_t *xbuf;			/* expedited data reassembly */
	lmi_option_t option;		/* LMI options */
	struct {
		bufq_t rb;		/* receive buffer */
		bufq_t tb;		/* transmission buffer */
		bufq_t rtb;		/* retransmission buffer */
		struct {
			ulong t1;	/* SL T1 timer */
			ulong t2;	/* SL T2 timer */
			ulong t3;	/* SL T3 timer */
			ulong t4;	/* SL T4 timer */
			ulong t5;	/* SL T5 timer */
			ulong t6;	/* SL T6 timer */
			ulong t7;	/* SL T7 timer */
		} timer;		/* SL timers */
		sl_config_t config;	/* SL configuration */
		sl_statem_t statem;	/* SL state machine */
		sl_notify_t notify;	/* SL notification options */
		sl_stats_t stats;	/* SL statistics */
		sl_stats_t stamp;	/* SL statistics timestamps */
		sl_stats_t statsp;	/* SL statistics periods */
	} sl;
	struct {
		bufq_t tb;		/* transmission buffer */
		mblk_t *tx_cmp;		/* tx compression buffer */
		mblk_t *rx_cmp;		/* rx compression buffer */
		uint tx_repeat;		/* tx compression count */
		uint rx_repeat;		/* rx compression count */
		struct {
			ulong t8;	/* SDT T8 timer */
		} timer;
		sdt_config_t config;	/* SDT configuration */
		sdt_statem_t statem;	/* SDT state machine */
		sdt_notify_t notify;	/* SDT notification options */
		sdt_stats_t stats;	/* SDT statistics */
		sdt_stats_t stamp;	/* SDT statistics timestamps */
		sdt_stats_t statsp;	/* SDT statistics periods */
	} sdt;
	struct {
		long tickbytes;		/* traffic bytes per tick */
		long bytecount;		/* traffic bytes this tick */
		long timestamp;		/* traffic last tick */
		struct {
			ulong t9;
		} timer;
		sdl_config_t config;	/* SDL configuration */
		sdl_statem_t statem;	/* SDL state machine variables */
		sdl_notify_t notify;	/* SDL notification options */
		sdl_stats_t stats;	/* SDL statistics */
		sdl_stats_t stamp;	/* SDL statistics timestamps */
		sdl_stats_t statsp;	/* SDL statistics periods */
	} sdl;
	struct {
		uint state;		/* transport state */
		uint serv_type;		/* transport service type */
		uint sequence;		/* transport conind sequence no */
		size_t pdu_size;	/* transport maximum pdu size */
		size_t opt_size;	/* transport maximum options size */
		size_t add_size;	/* transport addrress size */
		struct sockaddr loc;	/* transport local address */
		struct sockaddr rem;	/* transport remote address */
	} t;
} sl_t;

#define PRIV(__q) ((sl_t *)(__q)->q_ptr)

/*
 *  =========================================================================
 *
 *  Locking
 *
 *  =========================================================================
 */
STATIC INLINE int
sl_trylockq(queue_t *q)
{
	int res;
	sl_t *s = PRIV(q);
	if (!(res = spin_trylock(&s->qlock))) {
		if (q == s->rq)
			s->rwait = q;
		if (q == s->wq)
			s->wwait = q;
	}
	return (res);
}
STATIC INLINE void
sl_unlockq(queue_t *q)
{
	sl_t *s = PRIV(q);
	spin_unlock(&s->qlock);
	if (s->rwait)
		qenable(xchg(&s->rwait, NULL));
	if (s->wwait)
		qenable(xchg(&s->wwait, NULL));
}

/*
 *  =========================================================================
 *
 *  Buffer Allocation
 *
 *  =========================================================================
 */
/*
 *  BUFSRV calls service routine
 *  -------------------------------------------------------------------------
 */
STATIC void
sl_bufsrv(long data)
{
	queue_t *q = (queue_t *) data;
	if (q) {
		sl_t *sl = PRIV(q);
		if (q == sl->rq) {
			if (sl->rbid) {
				sl->rbid = 0;
				sl->refcnt--;
			}
		}
		if (q == sl->wq) {
			if (sl->wbid) {
				sl->wbid = 0;
				sl->refcnt--;
			}
		}
		qenable(q);
	}
}

/*
 *  UNBUFCALL
 *  -------------------------------------------------------------------------
 */
STATIC void
sl_unbufcall(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->rbid) {
		unbufcall(xchg(&sl->rbid, 0));
		sl->refcnt--;
	}
	if (sl->wbid) {
		unbufcall(xchg(&sl->wbid, 0));
		sl->refcnt--;
	}
}

/*
 *  ALLOCB
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
sl_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;
	if ((mp = allocb(size, prior)))
		return (mp);
	rare();
	{
		sl_t *sl = PRIV(q);
		if (q == sl->rq) {
			if (!sl->rbid) {
				sl->rbid = bufcall(size, prior, &sl_bufsrv, (long) q);
				sl->refcnt++;
			}
			return (NULL);
		}
		if (q == sl->wq) {
			if (!sl->wbid) {
				sl->wbid = bufcall(size, prior, &sl_bufsrv, (long) q);
				sl->refcnt++;
			}
			return (NULL);
		}
		swerr();
		return (NULL);
	}
}

/*
 *  ESBALLOC
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
sl_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;
	if ((mp = esballoc(base, size, prior, frtn)))
		return (mp);
	rare();
	{
		sl_t *sl = PRIV(q);
		if (q == sl->rq) {
			if (!sl->rbid) {
				sl->rbid = esbbcall(prior, &sl_bufsrv, (long) q);
				sl->refcnt++;
			}
			return (NULL);
		}
		if (q == sl->wq) {
			if (!sl->wbid) {
				sl->wbid = esbbcall(prior, &sl_bufsrv, (long) q);
				sl->refcnt++;
			}
			return (NULL);
		}
		swerr();
		return (NULL);
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  FAST buffer allocation/deallocation for compression/repetition
 *
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *sl_bufpool = NULL;
STATIC spinlock_t sl_bufpool_lock;

/*
 *  FAST ALLOCB
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
sl_fast_allocb(size_t size, int prior)
{
	mblk_t *mp;
	spin_lock(&sl_bufpool_lock);
	{
		if (size <= FASTBUF && prior == BPRI_HI && (mp = sl_bufpool)) {
			sl_bufpool = mp->b_cont;
			mp->b_cont = NULL;
			mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
		} else {
			mp = allocb(size, prior);
		}
	}
	spin_unlock(&sl_bufpool_lock);
	return (mp);
}

/*
 *  FAST FREEMSG
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sl_fast_freemsg(mblk_t *mp)
{
	mblk_t *bp, *bp_next = mp;
	while ((bp = bp_next)) {
		bp_next = bp->b_cont;
		if (bp->b_datap->db_ref == 1) {
			spin_lock(&sl_bufpool_lock);
			{
				bp->b_cont = xchg(&sl_bufpool, bp);
			}
			spin_unlock(&sl_bufpool_lock);
		} else if (bp->b_datap->db_ref > 1)
			/*
			   other references, use normal free mechanism 
			 */
			ctrace(freeb(bp));
	}
}

/*
 *  FAST DUPB
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
sl_fast_dupb(mblk_t *mp)
{
	int len;
	mblk_t *dp = NULL;
	if (mp && (len = mp->b_wptr - mp->b_rptr) > 0 && (dp = sl_fast_allocb(FASTBUF, BPRI_HI))) {
		bcopy(mp->b_rptr, dp->b_wptr, len);
		dp->b_wptr += len;
	}
	return (dp);
}

/*
 *  FAST INIT and TERM
 *  -------------------------------------------------------------------------
 */
STATIC void
sl_bufpool_init(void)
{
	spin_lock_init(&sl_bufpool_lock);
}
STATIC void
sl_bufpool_alloc(int n)
{
	psw_t flags;
	spin_lock_irqsave(&sl_bufpool_lock, flags);
	{
		while (n--)
			sl_fast_freemsg(allocb(FASTBUF, BPRI_LO));
	}
	spin_unlock_irqrestore(&sl_bufpool_lock, flags);
}
STATIC void
sl_bufpool_dealloc(int n)
{
	psw_t flags;
	spin_lock_irqsave(&sl_bufpool_lock, flags);
	{
		mblk_t *bp, *bp_next = sl_bufpool;
		while (n-- && (bp = bp_next)) {
			bp_next = bp->b_cont;
			ctrace(freeb(bp));
		}
		sl_bufpool = bp_next;
	}
	spin_unlock_irqrestore(&sl_bufpool_lock, flags);
}
STATIC void
sl_bufpool_term(void)
{
	psw_t flags;
	spin_lock_irqsave(&sl_bufpool_lock, flags);
	{
		mblk_t *bp, *bp_next = sl_bufpool;
		while ((bp = bp_next)) {
			bp_next = bp->b_cont;
			ctrace(freeb(bp));
		}
		sl_bufpool = bp_next;
	}
	spin_unlock_irqrestore(&sl_bufpool_lock, flags);
}

/*
 *  ========================================================================
 *
 *  PRIMITIVES
 *
 *  ========================================================================
 */
/*
 *  ------------------------------------------------------------------------
 *
 *  Primitive sent upstream
 *
 *  ------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, int err)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	if ((mp = sl_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sl->t.state = TS_NOSTATES;
		sl->state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_ERROR\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  M_HANGUP
 *  -----------------------------------
 */
STATIC int
m_hangup(queue_t *q, int err)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	if ((mp = sl_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sl->t.state = TS_NOSTATES;
		sl->state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_HANGUP\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, caddr_t ppa_ptr, size_t ppa_len)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = sl_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sl->state;
		p->lmi_max_sdu = sl->sdt.config.m + 1 + ((sl->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((sl->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_ok_ack(queue_t *q, long prim)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl->state) {
		case LMI_ATTACH_PENDING:
			sl->state = LMI_DISABLED;
			break;
		case LMI_DETACH_PENDING:
			sl->state = LMI_UNATTACHED;
			break;
		default:
			break;
		}
		p->lmi_state = sl->state;
		printd(("%s: %p: <- LMI_OK_ACK\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ack(queue_t *q, long prim, ulong reason, ulong errno)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	lmi_error_ack_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_error_primitive = prim;
		switch (sl->state) {
		case LMI_ATTACH_PENDING:
			sl->state = LMI_UNATTACHED;
			break;
		case LMI_DETACH_PENDING:
			sl->state = LMI_DISABLED;
			break;
		case LMI_ENABLE_PENDING:
			sl->state = LMI_DISABLED;
			break;
		case LMI_DISABLE_PENDING:
			sl->state = LMI_ENABLED;
			break;
		default:
			break;
		}
		p->lmi_state = sl->state;
		printd(("%s: %p: <- LMI_ERROR_ACK\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_enable_con(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ENABLE_CON;
		assure(sl->state == LMI_ENABLE_PENDING);
		sl->state = LMI_ENABLED;
		p->lmi_state = sl->state;
		printd(("%s: %p: <- LMI_ENABLE_CON\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_disable_con(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DISABLE_CON;
		assure(sl->state == LMI_DISABLE_PENDING);
		sl->state = LMI_DISABLED;
		p->lmi_state = sl->state;
		printd(("%s: %p: <- LMI_DISABLE_CON\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;
	if ((mp = sl_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ind(queue_t *q, ulong errno, ulong reason)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = sl->state;
		printd(("%s: %p: <- LMI_ERROR_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_stats_ind(queue_t *q, ulong interval)
{
	if (canputnext(PRIV(q)->rq)) {
		sl_t *sl = PRIV(q);
		mblk_t *mp;
		lmi_stats_ind_t *p;
		if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = jiffies;
			printd(("%s: %p: <- LMI_STATS_IND\n", SL_TPI_MOD_NAME, sl));
			putnext(sl->rq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, ulong oid, ulong level)
{
	if (canputnext(PRIV(q)->rq)) {
		sl_t *sl = PRIV(q);
		mblk_t *mp;
		lmi_event_ind_t *p;
		if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = jiffies;
			p->lmi_severity = level;
			printd(("%s: %p: <- LMI_EVENT_IND\n", SL_TPI_MOD_NAME, sl));
			putnext(sl->rq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  SL_PDU_IND
 *  -----------------------------------
 *  We don't actually use SL_PDU_INDs, we pass along M_DATA messages.
 */
STATIC INLINE int
sl_pdu_ind(queue_t *q, mblk_t *dp)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_pdu_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_PDU_IND;
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_PDU_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_link_congested_ind(queue_t *q, ulong cong, ulong disc)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_link_cong_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTED_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_link_congestion_ceased_ind(queue_t *q, ulong cong, ulong disc)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_link_cong_ceased_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		p->sl_timestamp = jiffies;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTION_CEASED_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieved_message_ind(queue_t *q, mblk_t *dp)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_retrieved_msg_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_RETRIEVED_MESSAGE_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieval_complete_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_retrieval_comp_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		printd(("%s: %p: <- SL_RETRIEVAL_COMPLETE_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_rb_cleared_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_rb_cleared_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RB_CLEARED_IND;
		printd(("%s: %p: <- SL_RB_CLEARED_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_bsnt_ind(queue_t *q, ulong bsnt)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_bsnt_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_BSNT_IND;
		p->sl_bsnt = bsnt;
		printd(("%s: %p: <- SL_BSNT_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_in_service_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_in_service_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_IN_SERVICE_IND;
		printd(("%s: %p: <- SL_IN_SERVICE_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_out_of_service_ind(queue_t *q, ulong reason)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_out_of_service_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = jiffies;
		p->sl_reason = reason;
		printd(("%s: %p: <- SL_OUT_OF_SERVICE_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_remote_processor_outage_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_rem_proc_out_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = jiffies;
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_OUTAGE_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_remote_processor_recovered_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_rem_proc_recovered_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = jiffies;
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_RECOVERED_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RTB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_rtb_cleared_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_rtb_cleared_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RTB_CLEARED_IND;
		printd(("%s: %p: <- SL_RTB_CLEARED_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieval_not_possible_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_retrieval_not_poss_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		printd(("%s: %p: <- SL_RETRIEVAL_NOT_POSSIBLE_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_bsnt_not_retrievable_ind(queue_t *q, ulong bsnt)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_bsnt_not_retr_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		p->sl_bsnt = bsnt;
		printd(("%s: %p: <- SL_BSNT_NOT_RETRIEVABLE_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
sl_optmgmt_ack(queue_t *q, caddr_t opt_ptr, size_t opt_len, ulong flags)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_optmgmt_ack_t *p;
	if ((mp = sl_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_OPTMGMT_ACK;
		p->opt_length = opt_len;
		p->opt_offset = opt_len ? sizeof(*p) : 0;
		p->mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- SL_OPTMGMT_ACK\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_NOTIFY_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_notify_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sl_notify_ind_t *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_NOTIFY_IND;
		printd(("%s: %p: <- SL_NOTIFY_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDT_RC_SIGNAL_UNIT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_signal_unit_ind(queue_t *q, mblk_t *dp, ulong count)
{
	if (count) {
		sl_t *sl = PRIV(q);
		if (canputnext(sl->rq)) {
			mblk_t *mp;
			sdt_rc_signal_unit_ind_t *p;
			if ((mp = allocb(sizeof(*p), BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = ((typeof(p)) mp->b_wptr)++;
				p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
				p->sdt_count = count;
				mp->b_cont = dp;
				// printd(("%s: %p: <- SDT_RC_SIGNAL_UNIT_IND\n", SL_TPI_MOD_NAME,
				// sl));
				putnext(sl->rq, mp);
				return (QR_ABSORBED);
			}
			rare();
			return (-ENOBUFS);
		}
		rare();
		return (-EBUSY);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SDT_RC_CONGESTION_ACCEPT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_accept_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sdt_rc_congestion_accept_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_ACCEPT_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_CONGESTION_DISCARD_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_discard_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sdt_rc_congestion_discard_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_DISCARD_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_NO_CONGESTION_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_no_congestion_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sdt_rc_no_congestion_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
		printd(("%s: %p: <- SDT_RC_NO_CONGESTION_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_IAC_CORRECT_SU_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_iac_correct_su_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (canputnext(sl->rq)) {
		mblk_t *mp;
		sdt_iac_correct_su_ind_t *p;
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
			printd(("%s: %p: <- SDT_IAC_CORRECT_SU_IND\n", SL_TPI_MOD_NAME, sl));
			putnext(sl->rq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  SDT_IAC_ABORT_PROVING_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_iac_abort_proving_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sdt_iac_abort_proving_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
		printd(("%s: %p: <- SDT_IAC_ABORT_PROVING_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_LSC_LINK_FAILURE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_lsc_link_failure_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sdt_lsc_link_failure_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
		printd(("%s: %p: <- SDT_LSC_LINK_FAILURE_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_txc_transmission_request_ind(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	sdt_txc_transmission_request_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		printd(("%s: %p: <- SDT_TXC_TRANSMISSION_REQUEST_IND\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  ------------------------------------------------------------------------
 */

/*
 *  T_CONN_REQ
 *  ------------------------------------------------------------------------
 *  Generate a connection request as part of an LMI_ENABLE_REQ on a connection
 *  oriented client-side transport.
 */
STATIC INLINE int
t_conn_req(queue_t *q)
{
	sl_t *sl = PRIV(q);
	caddr_t dst_ptr = (caddr_t) &sl->t.rem, opt_ptr = NULL;
	size_t dst_len = sl->t.add_size, opt_len = 0;
	mblk_t *mp, *dp = NULL;
	struct T_conn_req *p;
	if ((mp = sl_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mp->b_cont = dp;
		assure(sl->t.state == TS_IDLE);
		sl->t.state = TS_WACK_CREQ;
		printd(("%s: %p: <- T_CONN_REQ\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_CONN_RES
 *  ------------------------------------------------------------------------
 *  Respond to a T_CONN_IND when in the LMI_ENABLED state and we are in server
 *  mode on a connection-oriented transport.
 */
STATIC INLINE int
t_conn_res(queue_t *q, long seq)
{
	sl_t *sl = PRIV(q);
	caddr_t opt_ptr = NULL;
	size_t opt_len = 0;
	mblk_t *mp, *dp = NULL;
	struct T_conn_res *p;
	if ((mp = sl_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_CONN_RES;
		p->ACCEPTOR_id = (t_uscalar_t) RD(sl->wq->q_next);	/* accept on indicating
									   queue */
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		sl->t.sequence = seq;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mp->b_cont = dp;
		assure(sl->t.state == TS_WRES_CIND);
		sl->t.state = TS_WACK_CRES;
		printd(("%s: %p: <- T_CONN_RES\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_DISCON_REQ
 *  ------------------------------------------------------------------------
 *  Perform a disconnect when performing an LMI_DISABLE_REQ on a
 *  connection-oriented transport which is not orderly release capable.
 */
STATIC INLINE int
t_discon_req(queue_t *q, ulong seq)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp, *dp = NULL;
	struct T_discon_req *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_DISCON_REQ;
		p->SEQ_number = seq;
		mp->b_cont = dp;
		switch (sl->t.state) {
		case TS_WCON_CREQ:
			sl->t.state = TS_WACK_DREQ6;
			break;
		case TS_WRES_CIND:
			sl->t.state = TS_WACK_DREQ7;
			break;
		case TS_DATA_XFER:
			sl->t.state = TS_WACK_DREQ9;
			break;
		case TS_WIND_ORDREL:
			sl->t.state = TS_WACK_DREQ10;
			break;
		case TS_WREQ_ORDREL:
			sl->t.state = TS_WACK_DREQ11;
			break;
		default:
			swerr();
			sl->t.state = TS_NOSTATES;
			break;
		}
		printd(("%s: %p: <- T_DISCON_REQ\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_DATA_REQ
 *  ------------------------------------------------------------------------
 *  Send normal data on a connection oriented transport.
 */
STATIC INLINE mblk_t *
t_data_req(queue_t *q)
{
	mblk_t *mp = NULL;
	if (canputnext(q)) {
		struct T_data_req *p;
		if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = 0;
		}
	}
	return (mp);
}

/*
 *  T_EXDATA_REQ
 *  ------------------------------------------------------------------------
 *  Send expedited data on a connection oriented transport.  (We never do
 *  this.)
 */
STATIC INLINE mblk_t *
t_exdata_req(queue_t *q)
{
	mblk_t *mp = NULL;
	if (bcanputnext(q, 1)) {
		struct T_exdata_req *p;
		if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = 0;
		}
	}
	return (mp);
}

/*
 *  T_INFO_REQ
 *  ------------------------------------------------------------------------
 *  Request information about the provider.  This can be performed at the time
 *  that the module is pushed, particularly to discover if we have been pushed
 *  over a connection-oriented transport, whether the transport supports
 *  orderly release and whether the transport is connectionless.  Also to
 *  discover the connection state of the transport to determine whether we
 *  need to attach or not.
 */
STATIC INLINE int
t_info_req(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	struct T_info_req *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_INFO_REQ;
		assure(sl->t.state == TS_NOSTATES);
		printd(("%s: %p: <- T_INFO_REQ\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->wq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_BIND_REQ
 *  ------------------------------------------------------------------------
 *  Binding is performed as part of the LMI_ENABLE_REQ on both connectionless
 *  and connection oriented transports.  The address is taken directly from
 *  the local address provided in the PPA.  The bind will be a listening
 *  stream if the transport is connection oriented and in server mode.
 */
STATIC INLINE int
t_bind_req(queue_t *q)
{
	sl_t *sl = PRIV(q);
	caddr_t add_ptr = (caddr_t) &sl->t.loc;
	size_t add_len = sl->t.add_size;
#if 0
	ulong conind = ((sl->t.serv_type == T_COTS || sl->t.serv_type == T_COTS_ORD)
			&& (sl->sdl.ifmode == SDL_MODE_SERVER)) ? 1 : 0;
#else
	/*
	   always listen on COTS 
	 */
	ulong conind = (sl->t.serv_type == T_COTS || sl->t.serv_type == T_COTS_ORD) ? 1 : 0;
#endif
	mblk_t *mp;
	struct T_bind_req *p;
	if ((mp = sl_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = conind;
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		assure(sl->t.state == TS_UNBND);
		sl->t.state = TS_WACK_BREQ;
		printd(("%s: %p: <- T_BIND_REQ\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->wq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_UNBIND_REQ
 *  ------------------------------------------------------------------------
 *  Unbinding is performed as part of the LMI_DISABLE_REQ on both
 *  connectionless and connection-oriented transports.
 */
STATIC INLINE int
t_unbind_req(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	struct T_unbind_req *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_UNBIND_REQ;
		assure(sl->t.state == TS_IDLE);
		sl->t.state = TS_WACK_UREQ;
		printd(("%s: %p: <- T_UNBIND_REQ\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->wq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_UNITDATA_REQ
 *  ------------------------------------------------------------------------
 *  Unidata are sent on connectionless transports when in the LMI_ENABLED
 *  state.
 */
STATIC INLINE mblk_t *
t_unitdata_req(queue_t *q)
{
	mblk_t *mp = NULL;
	if (canputnext(q)) {
		sl_t *sl = PRIV(q);
		caddr_t dst_ptr = (caddr_t) &sl->t.rem, opt_ptr = NULL;
		size_t dst_len = sl->t.add_size, opt_len = 0;
		struct T_unitdata_req *p;
		if ((mp = sl_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_UNITDATA_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
	}
	return (mp);
}

/*
 *  T_OPTMGMT_REQ
 *  ------------------------------------------------------------------------
 *  Options may be requested for particular transport types once the module is
 *  pushed over the transport.
 */
STATIC INLINE int
t_optmgmt_req(queue_t *q, caddr_t opt_ptr, size_t opt_len, ulong flags)
{
	if (canputnext(q)) {
		sl_t *sl = PRIV(q);
		mblk_t *mp;
		struct T_optmgmt_req *p;
		if ((mp = sl_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_OPTMGMT_REQ;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->MGMT_flags = flags;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			printd(("%s: %p: <- T_OPTMGMT_REQ\n", SL_TPI_MOD_NAME, sl));
			putnext(sl->wq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  T_ORDREL_REQ
 *  ------------------------------------------------------------------------
 *  Orderly release is performed as part of the LMI_DISABLE_REQ on capable
 *  connection oriented transports.
 */
STATIC INLINE int
t_ordrel_req(queue_t *q)
{
	if (canputnext(q)) {
		sl_t *sl = PRIV(q);
		mblk_t *mp;
		struct T_ordrel_req *p;
		if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_ORDREL_REQ;
			assure(sl->t.state == TS_DATA_XFER);
			printd(("%s: %p: <- T_ORDREL_REQ\n", SL_TPI_MOD_NAME, sl));
			putnext(sl->wq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  T_OPTDATA_REQ
 *  ------------------------------------------------------------------------
 *  Option data rather than data or exdata may be sent on some connection
 *  oriented transports where control over data options is necessary.
 */
STATIC INLINE int
t_optdata_req(queue_t *q, mblk_t *dp)
{
	if (canputnext(q)) {
		sl_t *sl = PRIV(q);
		caddr_t opt_ptr = NULL;
		size_t opt_len = 0;
		int flags = 0;
		mblk_t *mp;
		struct T_optdata_req *p;
		if ((mp = sl_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = (flags & T_ODF_EX) ? 1 : 0;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			assure(sl->t.state == TS_IDLE);
			printd(("%s: %p: <- T_OPTDATA_REQ\n", SL_TPI_MOD_NAME, sl));
			putnext(sl->wq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  T_ADDR_REQ
 *  ------------------------------------------------------------------------
 *  Not useful.
 */
STATIC INLINE int
t_addr_req(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	struct T_addr_req *p;
	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_ADDR_REQ;
		printd(("%s: %p: <- T_ADDR_REQ\n", SL_TPI_MOD_NAME, sl));
		putnext(sl->wq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE FUNCTIONS
 *
 *  =========================================================================
 */
/*
 *  ------------------------------------------------------------------------
 *
 *  Timers
 *
 *  ------------------------------------------------------------------------
 */
enum { t1, t2, t3, t4, t5, t6, t7, t8, t9 };
STATIC INLINE mblk_t *
sl_alloc_timeout(ulong timer)
{
	mblk_t *mp;
	if ((mp = allocb(sizeof(ulong), BPRI_HI))) {
		mp->b_datap->db_type = M_PCRSE;
		*((ulong *) mp->b_wptr)++ = timer;
	}
	return (mp);
}
STATIC void
sl_t1_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sl.timer.t1, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t1))) {
			printd(("%s: %p: t1 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			__printd(("%s: %p: t1 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sl.timer.t1 = timeout(&sl_t1_timeout, (caddr_t) sl, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
sl_t2_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sl.timer.t2, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t2))) {
			printd(("%s: %p: t2 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			__printd(("%s: %p: t2 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sl.timer.t2 = timeout(&sl_t2_timeout, (caddr_t) sl, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
sl_t3_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sl.timer.t3, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t3))) {
			printd(("%s: %p: t3 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			__printd(("%s: %p: t3 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sl.timer.t3 = timeout(&sl_t3_timeout, (caddr_t) sl, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
sl_t4_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sl.timer.t4, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t4))) {
			printd(("%s: %p: t4 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			__printd(("%s: %p: t4 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sl.timer.t4 = timeout(&sl_t4_timeout, (caddr_t) sl, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
sl_t5_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sl.timer.t5, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t5))) {
			printd(("%s: %p: t5 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			__printd(("%s: %p: t5 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sl.timer.t5 = timeout(&sl_t5_timeout, (caddr_t) sl, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
sl_t6_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sl.timer.t6, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t6))) {
			printd(("%s: %p: t6 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			__printd(("%s: %p: t6 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sl.timer.t6 = timeout(&sl_t6_timeout, (caddr_t) sl, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
sl_t7_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sl.timer.t7, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t7))) {
			printd(("%s: %p: t7 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			__printd(("%s: %p: t7 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sl.timer.t7 = timeout(&sl_t7_timeout, (caddr_t) sl, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
sl_t8_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sdt.timer.t8, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t8))) {
			printd(("%s: %p: t8 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			__printd(("%s: %p: t8 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sdt.timer.t8 = timeout(&sl_t8_timeout, (caddr_t) sl, 10);	/* back off 
											 */
		}
	}
	return;
}
STATIC void
sl_t9_timeout(caddr_t data)
{
	sl_t *sl = (sl_t *) data;
	if (xchg(&sl->sdl.timer.t9, 0)) {
		mblk_t *mp;
		if ((mp = sl_alloc_timeout(t9))) {
			// printd(("%s: %p: t9 timeout at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			sl->refcnt--;
			sl_rput(sl->rq, mp);
		} else {
			// __printd(("%s: %p: t9 timeout collision\n", SL_TPI_MOD_NAME, sl));
			sl->sdl.timer.t9 = timeout(&sl_t9_timeout, (caddr_t) sl, 1);	/* back off 
											 */
		}
	}
	return;
}
STATIC INLINE void
sl_timer_stop(queue_t *q, const uint t)
{
	sl_t *sl = PRIV(q);
	switch (t) {
	case t1:
		if (sl->sl.timer.t1) {
			printd(("%s: %p: stopping t1 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sl.timer.t1, 0));
			sl->refcnt--;
		}
		break;
	case t2:
		if (sl->sl.timer.t2) {
			printd(("%s: %p: stopping t2 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sl.timer.t2, 0));
			sl->refcnt--;
		}
		break;
	case t3:
		if (sl->sl.timer.t3) {
			printd(("%s: %p: stopping t3 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sl.timer.t3, 0));
			sl->refcnt--;
		}
		break;
	case t4:
		if (sl->sl.timer.t4) {
			printd(("%s: %p: stopping t4 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sl.timer.t4, 0));
			sl->refcnt--;
		}
		break;
	case t5:
		if (sl->sl.timer.t5) {
			printd(("%s: %p: stopping t5 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sl.timer.t5, 0));
			sl->refcnt--;
		}
		break;
	case t6:
		if (sl->sl.timer.t6) {
			printd(("%s: %p: stopping t6 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sl.timer.t6, 0));
			sl->refcnt--;
		}
		break;
	case t7:
		if (sl->sl.timer.t7) {
			printd(("%s: %p: stopping t7 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sl.timer.t7, 0));
			sl->refcnt--;
		}
		break;
	case t8:
		if (sl->sdt.timer.t8) {
			printd(("%s: %p: stopping t8 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sdt.timer.t8, 0));
			sl->refcnt--;
		}
		break;
	case t9:
		if (sl->sdl.timer.t9) {
			// printd(("%s: %p: stopping t9 at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			untimeout(xchg(&sl->sdl.timer.t9, 0));
			sl->refcnt--;
		}
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sl_timer_start(queue_t *q, const uint t)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	spin_lock_irqsave(&sl->qlock, flags);
	{
		sl_timer_stop(q, t);
		switch (t) {
		case t1:
			printd(("%s: %p: starting t1 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl,
				sl->sl.config.t1 * 10, jiffies));
			sl->sl.timer.t1 = timeout(&sl_t1_timeout, (caddr_t) sl, sl->sl.config.t1);
			sl->refcnt++;
			break;
		case t2:
			printd(("%s: %p: starting t2 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl,
				sl->sl.config.t2 * 10, jiffies));
			sl->sl.timer.t2 = timeout(&sl_t2_timeout, (caddr_t) sl, sl->sl.config.t2);
			sl->refcnt++;
			break;
		case t3:
			printd(("%s: %p: starting t3 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl,
				sl->sl.config.t3 * 10, jiffies));
			sl->sl.timer.t3 = timeout(&sl_t3_timeout, (caddr_t) sl, sl->sl.config.t3);
			sl->refcnt++;
			break;
		case t4:
			printd(("%s: %p: starting t4 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl,
				sl->sl.statem.t4v * 10, jiffies));
			sl->sl.timer.t4 = timeout(&sl_t4_timeout, (caddr_t) sl, sl->sl.statem.t4v);
			sl->refcnt++;
			break;
		case t5:
			printd(("%s: %p: starting t5 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl,
				sl->sl.config.t5 * 10, jiffies));
			sl->sl.timer.t5 = timeout(&sl_t5_timeout, (caddr_t) sl, sl->sl.config.t5);
			sl->refcnt++;
			break;
		case t6:
			printd(("%s: %p: starting t6 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl,
				sl->sl.config.t6 * 10, jiffies));
			sl->sl.timer.t6 = timeout(&sl_t6_timeout, (caddr_t) sl, sl->sl.config.t6);
			sl->refcnt++;
			break;
		case t7:
			printd(("%s: %p: starting t7 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl,
				sl->sl.config.t7 * 10, jiffies));
			sl->sl.timer.t7 = timeout(&sl_t7_timeout, (caddr_t) sl, sl->sl.config.t7);
			sl->refcnt++;
			break;
		case t8:
			printd(("%s: %p: starting t8 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl,
				sl->sdt.config.t8 * 10, jiffies));
			sl->sdt.timer.t8 = timeout(&sl_t8_timeout, (caddr_t) sl, sl->sdt.config.t8);
			sl->refcnt++;
			break;
		case t9:
			// printd(("%s: %p: starting t9 %lu ms at %lu\n", SL_TPI_MOD_NAME, sl, (ulong)
			// (10), jiffies));
			sl->sdl.timer.t9 = timeout(&sl_t9_timeout, (caddr_t) sl, 1);
			sl->refcnt++;
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&sl->qlock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Duration Statistics
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC void
sl_is_stats(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.stamp.sl_dur_unavail)
		sl->sl.stats.sl_dur_unavail += jiffies - xchg(&sl->sl.stamp.sl_dur_unavail, 0);
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
	if (sl->sl.stamp.sl_dur_unavail_failed)
		sl->sl.stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_failed, 0);
	sl->sl.stamp.sl_dur_in_service = jiffies;
}
STATIC void
sl_oos_stats(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.stamp.sl_dur_in_service)
		sl->sl.stats.sl_dur_in_service +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_in_service, 0);
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
	if (sl->sl.stamp.sl_dur_unavail_failed)
		sl->sl.stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_failed, 0);
	sl->sl.stamp.sl_dur_unavail = jiffies;
}
STATIC void
sl_rpo_stats(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
}
STATIC void
sl_rpr_stats(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  SL State Machines
 *
 *  -------------------------------------------------------------------------
 */
#define SN_OUTSIDE(lower,middle,upper) \
	(  ( (lower) <= (upper) ) \
	   ? ( ( (middle) < (lower) ) || ( (middle) > (upper) ) ) \
	   : ( ( (middle) < (lower) ) && ( (middle) > (upper) ) ) \
	   )

/*
 *  -----------------------------------------------------------------------
 *
 *  STATE MACHINES:- The order of the state machine primitives below may seem somewhat disorganized at first
 *  glance; however, they have been ordered by dependency because they are all inline functions.  You see, the L2
 *  state machine does not required multiple threading because there is never a requirement to invoke the
 *  individual state machines concurrently.  This works out good for the driver, because a primitive action
 *  expands inline to the necessary procedure, while the source still takes the appearance of the SDL diagrams in
 *  the SS7 specification for inspection and debugging.
 *
 *  -----------------------------------------------------------------------
 */

#define sl_cc_stop sl_cc_normal
STATIC INLINE void
sl_cc_normal(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_timer_stop(q, t5);
	sl->sl.statem.cc_state = SL_STATE_IDLE;
}

STATIC INLINE void
sl_rc_stop(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_cc_normal(q);
	sl->sl.statem.rc_state = SL_STATE_IDLE;
}

STATIC INLINE void
sl_aerm_stop(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sdt.statem.aerm_state = SDT_STATE_IDLE;
	sl->sdt.statem.Ti = sl->sdt.config.Tin;
}

STATIC INLINE void
sl_iac_stop(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.iac_state != SL_STATE_IDLE) {
		sl_timer_stop(q, t3);
		sl_timer_stop(q, t2);
		sl_timer_stop(q, t4);
		sl_aerm_stop(q);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.iac_state = SL_STATE_IDLE;
	}
}

STATIC INLINE void
sl_txc_send_sios(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_timer_stop(q, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92)
		sl_timer_stop(q, t6);
	sl->sl.statem.lssu_available = 1;
	sl->sl.statem.tx.sio = LSSU_SIOS;
}

STATIC INLINE void
sl_poc_stop(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.poc_state = SL_STATE_IDLE;
}

STATIC INLINE void
sl_eim_stop(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sdt.statem.eim_state = SDT_STATE_IDLE;
	sl_timer_stop(q, t8);
}

STATIC INLINE void
sl_suerm_stop(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_eim_stop(q);
	sl->sdt.statem.suerm_state = SDT_STATE_IDLE;
}

STATIC INLINE int
sl_lsc_link_failure(queue_t *q, ulong reason)
{
	sl_t *sl = PRIV(q);
	int err;
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		if (sl->sl.statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
			if ((err = sl_out_of_service_ind(q, reason)))
				return (err);
			sl->sl.statem.failure_reason = reason;
			sl_iac_stop(q);	/* ok if not aligning */
			sl_timer_stop(q, t1);	/* ok if not running */
			sl_suerm_stop(q);	/* ok if not running */
			sl_rc_stop(q);
			sl_txc_send_sios(q);
			sl_poc_stop(q);	/* ok if not ITUT */
			sl->sl.statem.emergency = 0;
			sl->sl.statem.local_processor_outage = 0;
			sl->sl.statem.remote_processor_outage = 0;	/* ok if not ANSI */
			sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		}
		return (QR_DONE);
	}
	return sdt_lsc_link_failure_ind(q);
}

STATIC INLINE void
sl_txc_send_sib(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.tx.sio = LSSU_SIB;
	sl->sl.statem.lssu_available = 1;
}

STATIC INLINE void
sl_txc_send_sipo(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_timer_stop(q, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92)
		sl_timer_stop(q, t6);
	sl->sl.statem.tx.sio = LSSU_SIPO;
	sl->sl.statem.lssu_available = 1;
}

STATIC INLINE void
sl_txc_send_sio(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.tx.sio = LSSU_SIO;
	sl->sl.statem.lssu_available = 1;
}

STATIC INLINE void
sl_txc_send_sin(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.tx.sio = LSSU_SIN;
	sl->sl.statem.lssu_available = 1;
}

STATIC INLINE void
sl_txc_send_sie(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.tx.sio = LSSU_SIE;
	sl->sl.statem.lssu_available = 1;
}

STATIC INLINE void
sl_txc_send_msu(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.rtb.q_count)
		sl_timer_start(q, t7);
	sl->sl.statem.msu_inhibited = 0;
	sl->sl.statem.lssu_available = 0;
}

STATIC INLINE void
sl_txc_send_fisu(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_timer_stop(q, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92 && !(sl->option.popt & SS7_POPT_PCR))
		sl_timer_stop(q, t6);
	sl->sl.statem.msu_inhibited = 1;
	sl->sl.statem.lssu_available = 0;
}

STATIC INLINE void
sl_txc_fsnx_value(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.tx.X.fsn != sl->sl.statem.rx.X.fsn)
		sl->sl.statem.tx.X.fsn = sl->sl.statem.rx.X.fsn;
}

STATIC INLINE void
sl_txc_nack_to_be_sent(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.tx.N.bib = sl->sl.statem.tx.N.bib ? 0 : sl->sl.statem.ib_mask;
}

STATIC INLINE int
sl_lsc_rtb_cleared(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	if (sl->sl.statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->sl.statem.remote_processor_outage = 0;
		if (sl->sl.statem.local_processor_outage)
			return (QR_DONE);
		if ((err = sl_remote_processor_recovered_ind(q)))
			return (err);
		if ((err = sl_rtb_cleared_ind(q)))
			return (err);
		sl_txc_send_msu(q);
		sl->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
	}
	return (QR_DONE);
}

STATIC void sl_check_congestion(queue_t *q);

STATIC INLINE void
sl_txc_bsnr_and_bibr(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int pcr = sl->option.popt & SS7_POPT_PCR;
	sl->sl.statem.tx.R.bsn = sl->sl.statem.rx.R.bsn;
	sl->sl.statem.tx.R.bib = sl->sl.statem.rx.R.bib;
	if (sl->sl.statem.clear_rtb) {
		bufq_purge(&sl->sl.rtb);
		sl->sl.statem.Ct = 0;
		sl_check_congestion(q);
		sl->sl.statem.tx.F.fsn = (sl->sl.statem.tx.R.bsn + 1) & sl->sl.statem.sn_mask;
		sl->sl.statem.tx.L.fsn = sl->sl.statem.tx.R.bsn;
		sl->sl.statem.tx.N.fsn = sl->sl.statem.tx.R.bsn;
		sl->sl.statem.tx.N.fib = sl->sl.statem.tx.R.bib;
		sl->sl.statem.Z = (sl->sl.statem.tx.R.bsn + 1) & sl->sl.statem.sn_mask;
		sl->sl.statem.z_ptr = NULL;
		/*
		   FIXME: handle error return 
		 */
		sl_lsc_rtb_cleared(q);
		sl->sl.statem.clear_rtb = 0;
		sl->sl.statem.rtb_full = 0;
		return;
	}
	if (sl->sl.statem.tx.F.fsn != ((sl->sl.statem.tx.R.bsn + 1) & sl->sl.statem.sn_mask)) {
		if (sl->sl.statem.sib_received) {
			sl->sl.statem.sib_received = 0;
			sl_timer_stop(q, t6);
		}
		do {
			ctrace(freemsg(bufq_dequeue(&sl->sl.rtb)));
			sl->sl.statem.Ct--;
			sl->sl.statem.tx.F.fsn =
			    (sl->sl.statem.tx.F.fsn + 1) & sl->sl.statem.sn_mask;
		} while (sl->sl.statem.tx.F.fsn !=
			 ((sl->sl.statem.tx.R.bsn + 1) & sl->sl.statem.sn_mask));
		sl_check_congestion(q);
		if (sl->sl.rtb.q_count == 0) {
			sl_timer_stop(q, t7);
		} else {
			sl_timer_start(q, t7);
		}
		if (!pcr
		    || (sl->sl.rtb.q_msgs < sl->sl.config.N1
			&& sl->sl.rtb.q_count < sl->sl.config.N2))
			sl->sl.statem.rtb_full = 0;
		if (SN_OUTSIDE(sl->sl.statem.tx.F.fsn, sl->sl.statem.Z, sl->sl.statem.tx.L.fsn)
		    || !sl->sl.rtb.q_count) {
			sl->sl.statem.Z = sl->sl.statem.tx.F.fsn;
			sl->sl.statem.z_ptr = sl->sl.rtb.q_head;
		}
	}
	if (pcr)
		return;
	if (sl->sl.statem.tx.N.fib != sl->sl.statem.tx.R.bib) {
		if (sl->sl.statem.sib_received) {
			sl->sl.statem.sib_received = 0;
			sl_timer_stop(q, t6);
		}
		sl->sl.statem.tx.N.fib = sl->sl.statem.tx.R.bib;
		sl->sl.statem.tx.N.fsn = (sl->sl.statem.tx.F.fsn - 1) & sl->sl.statem.sn_mask;
		if ((sl->sl.statem.z_ptr = sl->sl.rtb.q_head) != NULL)
			sl->sl.statem.retrans_cycle = 1;
		return;
	}
}

STATIC INLINE void
sl_txc_sib_received(queue_t *q)
{
	sl_t *sl = PRIV(q);
	/*
	   FIXME: consider these variations for all 
	 */
	if (sl->option.pvar == SS7_PVAR_ANSI_92 && sl->sl.statem.lssu_available)
		if (sl->sl.statem.tx.sio != LSSU_SIB)
			return;
	if (sl->option.pvar != SS7_PVAR_ITUT_93 && !sl->sl.rtb.q_count)
		return;
	if (!sl->sl.statem.sib_received) {
		sl_timer_start(q, t6);
		sl->sl.statem.sib_received = 1;
	}
	sl_timer_start(q, t7);
}

STATIC INLINE void
sl_txc_clear_rtb(queue_t *q)
{
	sl_t *sl = PRIV(q);
	bufq_purge(&sl->sl.rtb);
	sl->sl.statem.Ct = 0;
	sl->sl.statem.clear_rtb = 1;
	sl->sl.statem.rtb_full = 0;	/* added */
	/*
	   FIXME: should probably follow more of the ITUT flush_buffers stuff like reseting Z and
	   FSNF, FSNL, FSNT. 
	 */
	sl_check_congestion(q);
}

STATIC INLINE void
sl_txc_clear_tb(queue_t *q)
{
	sl_t *sl = PRIV(q);
	bufq_purge(&sl->sl.tb);
	flushq(sl->wq, FLUSHDATA);
	sl->sl.statem.Cm = 0;
	sl_check_congestion(q);
}

STATIC INLINE void
sl_txc_flush_buffers(queue_t *q)
{
	sl_t *sl = PRIV(q);
	bufq_purge(&sl->sl.rtb);
	sl->sl.statem.rtb_full = 0;
	sl->sl.statem.Ct = 0;
	bufq_purge(&sl->sl.tb);
	flushq(sl->wq, FLUSHDATA);
	sl->sl.statem.Cm = 0;
	sl->sl.statem.Z = 0;
	sl->sl.statem.z_ptr = NULL;
	/*
	   Z =0 error in ITUT 93 and ANSI 
	 */
	sl->sl.statem.Z = sl->sl.statem.tx.F.fsn =
	    (sl->sl.statem.tx.R.bsn + 1) & sl->sl.statem.sn_mask;
	sl->sl.statem.tx.L.fsn = sl->sl.statem.rx.R.bsn;
	sl->sl.statem.rx.T.fsn = sl->sl.statem.rx.R.bsn;
	sl_timer_stop(q, t7);
	return;
}

STATIC INLINE void
sl_rc_fsnt_value(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.rx.T.fsn = sl->sl.statem.tx.N.fsn;
}

STATIC INLINE int
sl_txc_retrieval_request_and_fsnc(queue_t *q, sl_ulong fsnc)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	int err;
	sl->sl.statem.tx.C.fsn = fsnc & (sl->sl.statem.sn_mask);
	/*
	 *  FIXME: Q.704/5.7.2 states:
	 *
	 *  5.7.2   If a changeover order or acknowledgement containing an
	 *  unreasonable value of the forward sequence number is received, no
	 *  buffer updating or retrieval is performed, and new traffic is
	 *  started on the alternative signalling link(s).
	 *
	 *  It will be necessary to check FSNC for "reasonableness" here and
	 *  flush RTB and TB and return retrieval-complete indication with a
	 *  return code of "unreasonable FSNC".
	 *
	 *  (Tell the SIGTRAN working group and M2UA guys about this!)
	 */
	while (sl->sl.rtb.q_count && sl->sl.statem.tx.F.fsn != ((fsnc + 1) & sl->sl.statem.sn_mask)) {
		ctrace(freemsg(bufq_dequeue(&sl->sl.rtb)));
		sl->sl.statem.Ct--;
		sl->sl.statem.tx.F.fsn = (sl->sl.statem.tx.F.fsn + 1) & sl->sl.statem.sn_mask;
	}
	while ((mp = bufq_dequeue(&sl->sl.tb))) {
		sl->sl.statem.Cm--;
		bufq_queue(&sl->sl.rtb, mp);
		sl->sl.statem.Ct++;
		if (!sl->sl.statem.Cm)
			qenable(sl->wq);
	}
	sl->sl.statem.Z = sl->sl.statem.tx.F.fsn =
	    (sl->sl.statem.tx.C.fsn + 1) & sl->sl.statem.sn_mask;
	while ((mp = bufq_dequeue(&sl->sl.rtb))) {
		sl->sl.statem.Ct--;
		if ((err = sl_retrieved_message_ind(q, mp)))
			return (err);
	}
	sl->sl.statem.rtb_full = 0;
	if ((err = sl_retrieval_complete_ind(q)))
		return (err);
	sl->sl.statem.Cm = 0;
	sl->sl.statem.Ct = 0;
	sl->sl.statem.tx.N.fsn = sl->sl.statem.tx.L.fsn = sl->sl.statem.tx.C.fsn;
	return (QR_DONE);
}

STATIC INLINE void
sl_daedt_fisu(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->option.popt & SS7_POPT_XSN) {
		*((sl_ushort *) mp->b_wptr)++ =
		    htons(sl->sl.statem.tx.N.bsn | sl->sl.statem.tx.N.bib);
		*((sl_ushort *) mp->b_wptr)++ =
		    htons(sl->sl.statem.tx.N.fsn | sl->sl.statem.tx.N.fib);
		*((sl_ushort *) mp->b_wptr)++ = 0;
	} else {
		*((sl_uchar *) mp->b_wptr)++ = (sl->sl.statem.tx.N.bsn | sl->sl.statem.tx.N.bib);
		*((sl_uchar *) mp->b_wptr)++ = (sl->sl.statem.tx.N.fsn | sl->sl.statem.tx.N.fib);
		*((sl_uchar *) mp->b_wptr)++ = 0;
	}
}

STATIC INLINE void
sl_daedt_lssu(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->option.popt & SS7_POPT_XSN) {
		*((sl_ushort *) mp->b_wptr)++ =
		    htons(sl->sl.statem.tx.N.bsn | sl->sl.statem.tx.N.bib);
		*((sl_ushort *) mp->b_wptr)++ =
		    htons(sl->sl.statem.tx.N.fsn | sl->sl.statem.tx.N.fib);
		*((sl_ushort *) mp->b_wptr)++ = htons(1);
	} else {
		*((sl_uchar *) mp->b_wptr)++ = (sl->sl.statem.tx.N.bsn | sl->sl.statem.tx.N.bib);
		*((sl_uchar *) mp->b_wptr)++ = (sl->sl.statem.tx.N.fsn | sl->sl.statem.tx.N.fib);
		*((sl_uchar *) mp->b_wptr)++ = 1;
	}
	*((sl_uchar *) mp->b_wptr)++ = (sl->sl.statem.tx.sio);
	switch (sl->sl.statem.tx.sio) {
	case LSSU_SIO:
		printd(("%s: %p: <- SIO\n", SL_TPI_MOD_NAME, sl));
		break;
	case LSSU_SIN:
		printd(("%s: %p: <- SIN\n", SL_TPI_MOD_NAME, sl));
		break;
	case LSSU_SIE:
		printd(("%s: %p: <- SIE\n", SL_TPI_MOD_NAME, sl));
		break;
	case LSSU_SIOS:
		printd(("%s: %p: <- SIOS\n", SL_TPI_MOD_NAME, sl));
		break;
	case LSSU_SIPO:
		printd(("%s: %p: <- SIPO\n", SL_TPI_MOD_NAME, sl));
		break;
	case LSSU_SIB:
		printd(("%s: %p: <- SIB\n", SL_TPI_MOD_NAME, sl));
		break;
	}
}

STATIC INLINE void
sl_daedt_msu(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	int len = msgdsize(mp);
	if (sl->option.popt & SS7_POPT_XSN) {
		((sl_ushort *) mp->b_rptr)[0] =
		    htons(sl->sl.statem.tx.N.bsn | sl->sl.statem.tx.N.bib);
		((sl_ushort *) mp->b_rptr)[1] =
		    htons(sl->sl.statem.tx.N.fsn | sl->sl.statem.tx.N.fib);
		((sl_ushort *) mp->b_rptr)[2] = htons(len - 6 < 512 ? len - 6 : 511);
	} else {
		((sl_uchar *) mp->b_rptr)[0] = (sl->sl.statem.tx.N.bsn | sl->sl.statem.tx.N.bib);
		((sl_uchar *) mp->b_rptr)[1] = (sl->sl.statem.tx.N.fsn | sl->sl.statem.tx.N.fib);
		((sl_uchar *) mp->b_rptr)[2] = (len - 3 < 64 ? len - 3 : 63);
	}
}

STATIC INLINE mblk_t *
sl_txc_transmission_request(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp = NULL;
	int pcr;
	if (sl->sl.statem.txc_state != SL_STATE_IN_SERVICE)
		return (mp);
	pcr = sl->option.popt & SS7_POPT_PCR;
	if (sl->sl.statem.lssu_available) {
		if ((mp = allocb(7, BPRI_HI))) {
			if (sl->sl.statem.tx.sio == LSSU_SIB)
				sl->sl.statem.lssu_available = 0;
			sl->sl.statem.tx.N.fsn = sl->sl.statem.tx.L.fsn;
			sl->sl.statem.tx.N.bib = sl->sl.statem.tx.N.bib;
			sl->sl.statem.tx.N.bsn =
			    (sl->sl.statem.tx.X.fsn - 1) & sl->sl.statem.sn_mask;
			sl->sl.statem.tx.N.fib = sl->sl.statem.tx.N.fib;
			sl_daedt_lssu(q, mp);
		}
		return (mp);
	}
	if (sl->sl.statem.msu_inhibited) {
		if ((mp = allocb(6, BPRI_HI))) {
			sl->sl.statem.tx.N.fsn = sl->sl.statem.tx.L.fsn;
			sl->sl.statem.tx.N.bib = sl->sl.statem.tx.N.bib;
			sl->sl.statem.tx.N.bsn =
			    (sl->sl.statem.tx.X.fsn - 1) & sl->sl.statem.sn_mask;
			sl->sl.statem.tx.N.fib = sl->sl.statem.tx.N.fib;
			sl_daedt_fisu(q, mp);
		}
		return (mp);
	}
	if (pcr) {
		if ((sl->sl.rtb.q_msgs < sl->sl.config.N1)
		    && (sl->sl.rtb.q_count < sl->sl.config.N2)) {
			sl->sl.statem.forced_retransmission = 0;
			sl->sl.statem.rtb_full = 0;
		}
		if (sl->sl.tb.q_count && sl->sl.statem.rtb_full) {
			sl->sl.statem.forced_retransmission = 1;
		}
	}
	if ((!pcr && sl->sl.statem.retrans_cycle)
	    || (pcr
		&& (sl->sl.statem.forced_retransmission
		    || (!sl->sl.tb.q_count && sl->sl.rtb.q_count)))) {
		mblk_t *bp;
		if ((bp = sl->sl.statem.z_ptr) && !(mp = dupmsg(bp)))
			return (mp);
		if (!bp && pcr) {
			if ((bp = sl->sl.statem.z_ptr = sl->sl.rtb.q_head) && !(mp = dupmsg(bp)))
				return (mp);
			sl->sl.statem.Z = sl->sl.statem.tx.F.fsn;
		}
		if (mp) {
			sl->sl.statem.z_ptr = bp->b_next;
			if (pcr) {
				sl->sl.statem.tx.N.fsn = sl->sl.statem.Z;
				sl->sl.statem.tx.N.fib = sl->sl.statem.tx.N.fib;
				sl->sl.statem.tx.N.bsn =
				    (sl->sl.statem.tx.X.fsn - 1) & sl->sl.statem.sn_mask;
				sl->sl.statem.tx.N.bib = sl->sl.statem.tx.N.bib;
				sl->sl.statem.Z = (sl->sl.statem.Z + 1) & sl->sl.statem.sn_mask;
			} else {
				sl->sl.statem.tx.N.fsn =
				    (sl->sl.statem.tx.N.fsn + 1) & sl->sl.statem.sn_mask;
				sl->sl.statem.tx.N.fib = sl->sl.statem.tx.N.fib;
				sl->sl.statem.tx.N.bsn =
				    (sl->sl.statem.tx.X.fsn - 1) & sl->sl.statem.sn_mask;
				sl->sl.statem.tx.N.bib = sl->sl.statem.tx.N.bib;
			}
			sl_daedt_msu(q, mp);
			if (sl->sl.statem.tx.N.fsn == sl->sl.statem.tx.L.fsn
			    || sl->sl.statem.z_ptr == NULL)
				sl->sl.statem.retrans_cycle = 0;
		}
		return (mp);
	}
	if ((!pcr && (!sl->sl.tb.q_count || sl->sl.statem.rtb_full))
	    || (pcr && (!sl->sl.tb.q_count && !sl->sl.rtb.q_count))) {
		if ((mp = allocb(6, BPRI_HI))) {
			sl->sl.statem.tx.N.fsn = sl->sl.statem.tx.L.fsn;
			sl->sl.statem.tx.N.bib = sl->sl.statem.tx.N.bib;
			sl->sl.statem.tx.N.bsn =
			    (sl->sl.statem.tx.X.fsn - 1) & sl->sl.statem.sn_mask;
			sl->sl.statem.tx.N.fib = sl->sl.statem.tx.N.fib;
			sl_daedt_fisu(q, mp);
		}
		return (mp);
	} else {
		spin_lock(&sl->sl.tb.q_lock);
		if ((mp = bufq_head(&sl->sl.tb)) && (mp = dupmsg(mp))) {
			mblk_t *bp = bufq_dequeue(&sl->sl.tb);
			sl->sl.statem.Cm--;
			if (!sl->sl.statem.Cm)
				qenable(sl->wq);
			sl->sl.statem.tx.L.fsn =
			    (sl->sl.statem.tx.L.fsn + 1) & sl->sl.statem.sn_mask;
			sl->sl.statem.tx.N.fsn = sl->sl.statem.tx.L.fsn;
			if (!sl->sl.rtb.q_count)
				sl_timer_start(q, t7);
			bufq_queue(&sl->sl.rtb, bp);
			sl->sl.statem.Ct++;
			sl_rc_fsnt_value(q);
			if (pcr) {
				if ((sl->sl.rtb.q_msgs >= sl->sl.config.N1)
				    || (sl->sl.rtb.q_count >= sl->sl.config.N2)) {
					sl->sl.statem.rtb_full = 1;
					sl->sl.statem.forced_retransmission = 1;
				}
			} else {
				if ((sl->sl.rtb.q_msgs >= sl->sl.config.N1)
				    || (sl->sl.rtb.q_count >= sl->sl.config.N2)
				    || (sl->sl.statem.tx.L.fsn ==
					((sl->sl.statem.tx.F.fsn - 2) & sl->sl.statem.sn_mask)))
					sl->sl.statem.rtb_full = 1;
			}
			sl->sl.statem.tx.N.bib = sl->sl.statem.tx.N.bib;
			sl->sl.statem.tx.N.bsn =
			    (sl->sl.statem.tx.X.fsn - 1) & sl->sl.statem.sn_mask;
			sl->sl.statem.tx.N.fib = sl->sl.statem.tx.N.fib;
			sl_daedt_msu(q, mp);
		}
		spin_unlock(&sl->sl.tb.q_lock);
		return (mp);
	}
}

STATIC INLINE void
sl_daedr_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sdt.statem.daedr_state = SDT_STATE_IN_SERVICE;
	sl->sdl.statem.rx_state = SDL_STATE_IN_SERVICE;
	sl->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_RX_RUNNING);
}

STATIC INLINE void
sl_rc_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.rc_state == SL_STATE_IDLE) {
		sl->sl.statem.rx.X.fsn = 0;
		sl->sl.statem.rx.X.fib = sl->sl.statem.ib_mask;
		sl->sl.statem.rx.F.fsn = 0;
		sl->sl.statem.rx.T.fsn = sl->sl.statem.sn_mask;
		sl->sl.statem.rtr = 0;	/* Basic only (note 1). */
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->sl.statem.msu_fisu_accepted = 1;
		else
			sl->sl.statem.msu_fisu_accepted = 0;
		sl->sl.statem.abnormal_bsnr = 0;
		sl->sl.statem.abnormal_fibr = 0;	/* Basic only (note 1). */
		sl->sl.statem.congestion_discard = 0;
		sl->sl.statem.congestion_accept = 0;
		sl_daedr_start(q);
		sl->sl.statem.rc_state = SL_STATE_IN_SERVICE;
		return;
		/*
		 *   Note 1 - Although rtr and abnormal_fibr are only applicable to the Basic procedure (and
		 *   not PCR), these state machine variables are never examined by PCR routines, so PCR and
		 *   basic can share the same start procedures.
		 */
	}
}

STATIC INLINE void
sl_rc_reject_msu_fisu(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.msu_fisu_accepted = 0;
}

STATIC INLINE void
sl_rc_accept_msu_fisu(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.msu_fisu_accepted = 1;
}

STATIC INLINE void
sl_rc_retrieve_fsnx(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_txc_fsnx_value(q);	/* error in 93 spec */
	sl->sl.statem.congestion_discard = 0;
	sl->sl.statem.congestion_accept = 0;
	sl_cc_normal(q);
	sl->sl.statem.rtr = 0;	/* basic only */
}

STATIC INLINE void
sl_rc_align_fsnx(queue_t *q)
{
	sl_txc_fsnx_value(q);
}

STATIC INLINE int
sl_rc_clear_rb(queue_t *q)
{
	sl_t *sl = PRIV(q);
	bufq_purge(&sl->sl.rb);
	// flushq(sl->rq, FLUSHDATA);
	sl->sl.statem.Cr = 0;
	return sl_rb_cleared_ind(q);
}

STATIC INLINE int
sl_rc_retrieve_bsnt(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.rx.T.bsn = (sl->sl.statem.rx.X.fsn - 1) & 0x7F;
	return sl_bsnt_ind(q, sl->sl.statem.rx.T.bsn);
}

STATIC INLINE void
sl_cc_busy(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.cc_state == SL_STATE_NORMAL) {
		sl_txc_send_sib(q);
		sl_timer_start(q, t5);
		sl->sl.statem.cc_state = SL_STATE_BUSY;
	}
}

STATIC INLINE void
sl_rc_congestion_discard(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		sl->sl.statem.congestion_discard = 1;
		sl_cc_busy(q);
		return;
	}
	sdt_rc_congestion_discard_ind(q);
}

STATIC INLINE void
sl_rc_congestion_accept(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		sl->sl.statem.congestion_accept = 1;
		sl_cc_busy(q);
		return;
	}
	sdt_rc_congestion_accept_ind(q);
}

STATIC INLINE void
sl_rc_no_congestion(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		sl->sl.statem.congestion_discard = 0;
		sl->sl.statem.congestion_accept = 0;
		sl_cc_normal(q);
		sl_txc_fsnx_value(q);
		if (sl->sl.statem.rtr == 1) {	/* rtr never set for PCR */
			sl_txc_nack_to_be_sent(q);
			sl->sl.statem.rx.X.fib = sl->sl.statem.rx.X.fib ? 0 : sl->sl.statem.ib_mask;
		}
		return;
	}
	sdt_rc_no_congestion_ind(q);
}

STATIC INLINE void
sl_lsc_congestion_discard(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_rc_congestion_discard(q);
	sl->sl.statem.l3_congestion_detect = 1;
}

STATIC INLINE void
sl_lsc_congestion_accept(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_rc_congestion_accept(q);
	sl->sl.statem.l3_congestion_detect = 1;
}

STATIC INLINE void
sl_lsc_no_congestion(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl_rc_no_congestion(q);
	sl->sl.statem.l3_congestion_detect = 0;
}

STATIC INLINE void
sl_lsc_sio(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_INITIAL_ALIGNMENT:
		break;
	default:
		sl_timer_stop(q, t1);	/* ok if not running */
		sl_out_of_service_ind(q, SL_FAIL_RECEIVED_SIO);
		sl->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIO;
		sl_rc_stop(q);
		sl_suerm_stop(q);
		sl_poc_stop(q);	/* ok if ANSI */
		sl_txc_send_sios(q);
		sl->sl.statem.emergency = 0;
		/*
		   FIXME: reinspect 
		 */
		sl->sl.statem.local_processor_outage = 0;
		sl->sl.statem.remote_processor_outage = 0;	/* ok if ITUT */
		sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
}

STATIC INLINE int
sl_lsc_alignment_not_possible(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	if ((err = sl_out_of_service_ind(q, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)))
		return (err);
	sl->sl.statem.failure_reason = SL_FAIL_ALIGNMENT_NOT_POSSIBLE;
	sl_rc_stop(q);
	sl_txc_send_sios(q);
	sl->sl.statem.local_processor_outage = 0;
	sl->sl.statem.emergency = 0;
	sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	return (QR_DONE);
}

STATIC INLINE void
sl_iac_sio(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(q, t2);
		if (sl->sl.statem.emergency) {
			sl->sl.statem.t4v = sl->sl.config.t4e;
			printd(("%s: Sending SIE at %lu\n", SL_TPI_MOD_NAME, jiffies));
			sl_txc_send_sie(q);
		} else {
			sl->sl.statem.t4v = sl->sl.config.t4n;
			sl_txc_send_sin(q);
		}
		sl_timer_start(q, t3);
		sl->sl.statem.iac_state = SL_STATE_ALIGNED;
		break;
	case SL_STATE_PROVING:
		sl_timer_stop(q, t4);
		sl_aerm_stop(q);
		sl_timer_start(q, t3);
		sl->sl.statem.iac_state = SL_STATE_ALIGNED;
		break;
	}
}

STATIC INLINE void
sl_iac_sios(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.iac_state) {
	case SL_STATE_ALIGNED:
	case SL_STATE_PROVING:
		sl_timer_stop(q, t4);	/* ok if not running */
		sl_lsc_alignment_not_possible(q);
		sl_aerm_stop(q);	/* ok if not running */
		sl_timer_stop(q, t3);	/* ok if not running */
		sl->sl.statem.emergency = 0;
		sl->sl.statem.iac_state = SL_STATE_IDLE;
		break;
	}
}

STATIC INLINE void
sl_lsc_sios(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
	case SL_STATE_ALIGNED_NOT_READY:
		sl_timer_stop(q, t1);	/* ok to stop if not running */
	case SL_STATE_IN_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(q, SL_FAIL_RECEIVED_SIOS);
		sl->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIOS;
		sl_suerm_stop(q);	/* ok to stop if not running */
		sl_rc_stop(q);
		sl_poc_stop(q);	/* ok if ANSI */
		sl_txc_send_sios(q);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.local_processor_outage = 0;
		sl->sl.statem.remote_processor_outage = 0;	/* ok if ITU */
		sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
}

STATIC INLINE void
sl_lsc_no_processor_outage(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->sl.statem.processor_outage = 0;
		if (!sl->sl.statem.l3_indication_received)
			return;
		sl->sl.statem.l3_indication_received = 0;
		sl_txc_send_msu(q);
		sl->sl.statem.local_processor_outage = 0;
		sl_rc_accept_msu_fisu(q);
		sl->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
	}
}

STATIC INLINE void
sl_poc_remote_processor_recovered(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.poc_state) {
	case SL_STATE_REMOTE_PROCESSOR_OUTAGE:
		sl_lsc_no_processor_outage(q);
		sl->sl.statem.poc_state = SL_STATE_IDLE;
		return;
	case SL_STATE_BOTH_PROCESSORS_OUT:
		sl->sl.statem.poc_state = SL_STATE_LOCAL_PROCESSOR_OUTAGE;
		return;
	}
}

STATIC INLINE int
sl_lsc_fisu_msu_received(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
		if ((err = sl_in_service_ind(q)))
			return (err);
		if (sl->option.pvar == SS7_PVAR_ITUT_93)
			sl_rc_accept_msu_fisu(q);	/* unnecessary */
		sl_timer_stop(q, t1);
		sl_txc_send_msu(q);
		sl->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if ((err = sl_in_service_ind(q)))
			return (err);
		sl_timer_stop(q, t1);
		sl->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl_poc_remote_processor_recovered(q);
			return sl_remote_processor_recovered_ind(q);
		case SS7_PVAR_ANSI_92:
			sl->sl.statem.remote_processor_outage = 0;
			return sl_remote_processor_recovered_ind(q);
		default:
			/*
			 *  A deviation from the SDLs has been placed here to limit the number of remote
			 *  processor recovered indications which are delivered to L3.  One indication is
			 *  sufficient.
			 */
			if (sl->sl.statem.remote_processor_outage) {
				sl->sl.statem.remote_processor_outage = 0;
				return sl_remote_processor_recovered_ind(q);
			}
			break;
		}
		break;
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_poc_remote_processor_outage(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.poc_state) {
	case SL_STATE_IDLE:
		sl->sl.statem.poc_state = SL_STATE_REMOTE_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_LOCAL_PROCESSOR_OUTAGE:
		sl->sl.statem.poc_state = SL_STATE_BOTH_PROCESSORS_OUT;
		return;
	}
}

STATIC INLINE void
sl_lsc_sib(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_txc_sib_received(q);
		break;
	}
}

STATIC INLINE int
sl_lsc_sipo(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl_timer_stop(q, t1);
			if ((err = sl_remote_processor_outage_ind(q)))
				return (err);
			sl_poc_remote_processor_outage(q);
			sl->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
			break;
		case SS7_PVAR_ANSI_92:
			sl_timer_stop(q, t1);
			if ((err = sl_remote_processor_outage_ind(q)))
				return (err);
			sl->sl.statem.remote_processor_outage = 1;
			sl->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
			break;
		}
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			if ((err = sl_remote_processor_outage_ind(q)))
				return (err);
			sl_poc_remote_processor_outage(q);
			sl_timer_stop(q, t1);
			sl->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
			break;
		case SS7_PVAR_ANSI_92:
			if ((err = sl_remote_processor_outage_ind(q)))
				return (err);
			sl->sl.statem.remote_processor_outage = 1;
			sl_timer_stop(q, t1);
			sl->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
			break;
		}
		break;
	case SL_STATE_IN_SERVICE:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl_txc_send_fisu(q);
			if ((err = sl_remote_processor_outage_ind(q)))
				return (err);
			sl_poc_remote_processor_outage(q);
			sl->sl.statem.processor_outage = 1;	/* remote? */
			sl->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
			break;
		case SS7_PVAR_ANSI_92:
			sl_txc_send_fisu(q);
			if ((err = sl_remote_processor_outage_ind(q)))
				return (err);
			sl->sl.statem.remote_processor_outage = 1;
			sl_rc_align_fsnx(q);
			sl_cc_stop(q);
			sl->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
			break;
		}
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			if ((err = sl_remote_processor_outage_ind(q)))
				return (err);
			sl_poc_remote_processor_outage(q);
			break;
		case SS7_PVAR_ANSI_92:
			sl->sl.statem.remote_processor_outage = 1;
			return sl_remote_processor_outage_ind(q);
		default:
			/*
			 *  A deviation from the SDLs has been placed here to limit the number of remote
			 *  processor outage indications which are delivered to L3.  One indication is
			 *  sufficient.
			 */
			if (!sl->sl.statem.remote_processor_outage) {
				sl->sl.statem.remote_processor_outage = 1;
				return sl_remote_processor_outage_ind(q);
			}
			break;
		}
		break;
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_poc_local_processor_outage(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.poc_state) {
	case SL_STATE_IDLE:
		sl->sl.statem.poc_state = SL_STATE_LOCAL_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_REMOTE_PROCESSOR_OUTAGE:
		sl->sl.statem.poc_state = SL_STATE_BOTH_PROCESSORS_OUT;
		return;
	}
}

STATIC INLINE void
sl_eim_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sdt.statem.Ce = 0;
	sl->sdt.statem.interval_error = 0;
	sl->sdt.statem.su_received = 0;
	sl_timer_start(q, t8);
	sl->sdt.statem.eim_state = SDT_STATE_MONITORING;
}

STATIC INLINE void
sl_suerm_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->option.popt & SS7_POPT_HSL)
		sl_eim_start(q);
	else {
		sl->sdt.statem.Cs = 0;
		sl->sdt.statem.Ns = 0;
		sl->sdt.statem.suerm_state = SDT_STATE_IN_SERVICE;
	}
}

STATIC INLINE void
sl_lsc_alignment_complete(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state == SL_STATE_INITIAL_ALIGNMENT) {
		sl_suerm_start(q);
		sl_timer_start(q, t1);
		if (sl->sl.statem.local_processor_outage) {
			if (sl->option.pvar != SS7_PVAR_ANSI_92)
				sl_poc_local_processor_outage(q);
			sl_txc_send_sipo(q);
			if (sl->option.pvar != SS7_PVAR_ITUT_93)	/* possible error */
				sl_rc_reject_msu_fisu(q);
			sl->sl.statem.lsc_state = SL_STATE_ALIGNED_NOT_READY;
		} else {
			sl_txc_send_msu(q);	/* changed from send_fisu for Q.781 */
			sl_rc_accept_msu_fisu(q);	/* error in ANSI spec? */
			sl->sl.statem.lsc_state = SL_STATE_ALIGNED_READY;
		}
	}
}

STATIC INLINE void
sl_lsc_sin(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
		sl_out_of_service_ind(q, SL_FAIL_RECEIVED_SIN);
		sl->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIN;
		sl_suerm_stop(q);
		sl_rc_stop(q);
		sl_txc_send_sios(q);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(q, SL_FAIL_RECEIVED_SIN);
		sl->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIN;
		sl_suerm_stop(q);
		sl_rc_stop(q);
		sl_poc_stop(q);	/* ok if not ITUT */
		sl_txc_send_sios(q);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.local_processor_outage = 0;
		sl->sl.statem.remote_processor_outage = 0;	/* ok if not ANSI */
		sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
}

STATIC INLINE void
sl_aerm_set_ti_to_tie(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sdt.statem.aerm_state == SDT_STATE_IDLE)
		sl->sdt.statem.Ti = sl->sdt.config.Tie;
}

STATIC INLINE void
sl_aerm_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sdt.statem.Ca = 0;
	sl->sdt.statem.aborted_proving = 0;
	sl->sdt.statem.aerm_state = SDT_STATE_MONITORING;
}

STATIC INLINE void
sl_iac_sin(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(q, t2);
		if (sl->sl.statem.emergency) {
			sl->sl.statem.t4v = sl->sl.config.t4e;
			sl_txc_send_sie(q);
		} else {
			sl->sl.statem.t4v = sl->sl.config.t4n;
			sl_txc_send_sin(q);
		}
		sl_timer_start(q, t3);
		sl->sl.statem.iac_state = SL_STATE_ALIGNED;
		return;
	case SL_STATE_ALIGNED:
		sl_timer_stop(q, t3);
		if (sl->sl.statem.t4v == sl->sl.config.t4e)
			sl_aerm_set_ti_to_tie(q);
		sl_aerm_start(q);
		sl_timer_start(q, t4);
		sl->sl.statem.further_proving = 0;
		sl->sl.statem.Cp = 0;
		sl->sl.statem.iac_state = SL_STATE_PROVING;
		return;
	}
}

STATIC INLINE void
sl_lsc_sie(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
		sl_out_of_service_ind(q, SL_FAIL_RECEIVED_SIE);
		sl->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIE;
		sl_suerm_stop(q);
		sl_rc_stop(q);
		sl_txc_send_sios(q);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(q, SL_FAIL_RECEIVED_SIE);
		sl->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIE;
		sl_suerm_stop(q);
		sl_rc_stop(q);
		sl_poc_stop(q);	/* ok if not ITUT */
		sl_txc_send_sios(q);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.local_processor_outage = 0;
		sl->sl.statem.remote_processor_outage = 0;	/* ok if not ANSI */
		sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
}

STATIC INLINE void
sl_iac_sie(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(q, t2);
		if (sl->sl.statem.emergency) {
			sl->sl.statem.t4v = sl->sl.config.t4e;
			sl_txc_send_sie(q);
		} else {
			sl->sl.statem.t4v = sl->sl.config.t4e;	/* yes e */
			sl_txc_send_sin(q);
		}
		sl_timer_start(q, t3);
		sl->sl.statem.iac_state = SL_STATE_ALIGNED;
		return;
	case SL_STATE_ALIGNED:
		printd(("%s: Receiving SIE at %lu\n", SL_TPI_MOD_NAME, jiffies));
		sl->sl.statem.t4v = sl->sl.config.t4e;
		sl_timer_stop(q, t3);
		sl_aerm_set_ti_to_tie(q);
		sl_aerm_start(q);
		sl_timer_start(q, t4);
		sl->sl.statem.further_proving = 0;
		sl->sl.statem.Cp = 0;
		sl->sl.statem.iac_state = SL_STATE_PROVING;
		return;
	case SL_STATE_PROVING:
		if (sl->sl.statem.t4v == sl->sl.config.t4e)
			return;
		sl_timer_stop(q, t4);
		sl->sl.statem.t4v = sl->sl.config.t4e;
		sl_aerm_stop(q);
		sl_aerm_set_ti_to_tie(q);
		sl_aerm_start(q);
		sl_timer_start(q, t4);
		sl->sl.statem.further_proving = 0;
		return;
	}
}

/*
 *  --------------------------------------------------------------------------
 *
 *  These congestion functions are implementation dependent.  We should define
 *  a congestion onset level and set congestion accept at that point.  We
 *  should also define a second congestion onset level and set congestion
 *  discard at that point.  For STREAMS, the upstream congestion can be
 *  detected in two ways: 1) canputnext(): is the upstream module flow
 *  controlled; and, 2) canput(): are we flow controlled.  If the upstream
 *  module is flow controlled, then we can accept MSUs and place them on our
 *  own read queue.  If we are flow contolled, then we have no choice but to
 *  discard the message.  In addition, and because upstream message processing
 *  times are likely more sensitive to the number of backlogged messages than
 *  they are to the number of backlogged message octets, we have some
 *  configurable thresholds of backlogging and keep track of backlogged
 *  messages.
 *
 *  --------------------------------------------------------------------------
 */

STATIC INLINE void
sl_rb_congestion_function(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (!sl->sl.statem.l3_congestion_detect) {
		if (sl->sl.statem.l2_congestion_detect) {
			if (sl->sl.statem.Cr <= sl->sl.config.rb_abate && canputnext(sl->rq)) {
				sl_rc_no_congestion(q);
				sl->sl.statem.l2_congestion_detect = 0;
			}
		} else {
			if (sl->sl.statem.Cr >= sl->sl.config.rb_discard || !canput(sl->rq)) {
				sl_rc_congestion_discard(q);
				sl->sl.statem.l2_congestion_detect = 1;
			} else if (sl->sl.statem.Cr >= sl->sl.config.rb_accept
				   || !canputnext(sl->rq)) {
				sl_rc_congestion_accept(q);
				sl->sl.statem.l2_congestion_detect = 1;
			}
		}
	}
}

STATIC INLINE void
sl_rc_signal_unit(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	int pcr = sl->option.popt & SS7_POPT_PCR;
	if (sl->sl.statem.rc_state != SL_STATE_IN_SERVICE) {
		ctrace(freemsg(mp));
		return;
	}
	/*
	 *  Note: the driver must check that the length of the frame is within appropriate bounds as specified by
	 *  the DAEDR in Q.703.  If the length of the frame is incorrect, it should indicate daedr_error- _frame
	 *  rather than daedr_received_frame.
	 */
	if (sl->option.popt & SS7_POPT_XSN) {
		sl->sl.statem.rx.R.bsn = ntohs(*((sl_ushort *) mp->b_rptr)) & 0x0fff;
		sl->sl.statem.rx.R.bib = ntohs(*((sl_ushort *) mp->b_rptr)++) & 0x8000;
		sl->sl.statem.rx.R.fsn = ntohs(*((sl_ushort *) mp->b_rptr)) & 0x0fff;
		sl->sl.statem.rx.R.fib = ntohs(*((sl_ushort *) mp->b_rptr)++) & 0x8000;
		sl->sl.statem.rx.len = ntohs(*((sl_ushort *) mp->b_rptr)++) & 0x01ff;
	} else {
		sl->sl.statem.rx.R.bsn = *mp->b_rptr & 0x7f;
		sl->sl.statem.rx.R.bib = *mp->b_rptr++ & 0x80;
		sl->sl.statem.rx.R.fsn = *mp->b_rptr & 0x7f;
		sl->sl.statem.rx.R.fib = *mp->b_rptr++ & 0x80;
		sl->sl.statem.rx.len = *mp->b_rptr++ & 0x3f;
	}
	if (sl->sl.statem.rx.len == 1) {
		sl->sl.statem.rx.sio = *mp->b_rptr++ & 0x07;
	}
	if (sl->sl.statem.rx.len == 2) {
		sl->sl.statem.rx.sio = *mp->b_rptr++ & 0x07;
		sl->sl.statem.rx.sio = *mp->b_rptr++ & 0x07;
	}
#if 0
	ptrace(("%s: rx: bsn=%x, bib=%x, fsn=%x, fib=%x, len=%d, sio=%d\n", SL_TPI_MOD_NAME,
		sl->sl.statem.rx.R.bsn, sl->sl.statem.rx.R.bib, sl->sl.statem.rx.R.fsn,
		sl->sl.statem.rx.R.fib, sl->sl.statem.rx.len, sl->sl.statem.rx.sio));
#endif
	if (((sl->sl.statem.rx.len) == 1) || ((sl->sl.statem.rx.len) == 2)) {
		switch (sl->sl.statem.rx.sio) {
		case LSSU_SIO:{
			printd(("%s: %p: -> SIO\n", SL_TPI_MOD_NAME, sl));
			sl_iac_sio(q);
			sl_lsc_sio(q);
			break;
		}
		case LSSU_SIN:{
			printd(("%s: %p: -> SIN\n", SL_TPI_MOD_NAME, sl));
			sl_iac_sin(q);
			sl_lsc_sin(q);
			break;
		}
		case LSSU_SIE:{
			printd(("%s: %p: -> SIE\n", SL_TPI_MOD_NAME, sl));
			sl_iac_sie(q);
			sl_lsc_sie(q);
			break;
		}
		case LSSU_SIOS:{
			printd(("%s: %p: -> SIOS\n", SL_TPI_MOD_NAME, sl));
			sl_iac_sios(q);
			sl_lsc_sios(q);
			break;
		}
		case LSSU_SIPO:{
			printd(("%s: %p: -> SIPO\n", SL_TPI_MOD_NAME, sl));
			sl_lsc_sipo(q);
			break;
		}
		case LSSU_SIB:{
			printd(("%s: %p: -> SIB\n", SL_TPI_MOD_NAME, sl));
			sl_lsc_sib(q);
			break;
		}
		}
		ctrace(freemsg(mp));
		return;
	}
	if (SN_OUTSIDE
	    (((sl->sl.statem.rx.F.fsn - 1) & sl->sl.statem.sn_mask), sl->sl.statem.rx.R.bsn,
	     sl->sl.statem.rx.T.fsn)) {
		if (sl->sl.statem.abnormal_bsnr) {
			sl_lsc_link_failure(q, SL_FAIL_ABNORMAL_BSNR);
			sl->sl.statem.rc_state = SL_STATE_IDLE;
			ctrace(freemsg(mp));
			return;
		} else {
			sl->sl.statem.abnormal_bsnr = 1;
			sl->sl.statem.unb = 0;
			ctrace(freemsg(mp));
			return;
		}
	}
	if (sl->sl.statem.abnormal_bsnr) {
		if (sl->sl.statem.unb == 1) {
			sl->sl.statem.abnormal_bsnr = 0;
		} else {
			sl->sl.statem.unb = 1;
			ctrace(freemsg(mp));
			return;
		}
	}
	if (pcr) {
		sl_lsc_fisu_msu_received(q);
		sl_txc_bsnr_and_bibr(q);
		sl->sl.statem.rx.F.fsn = (sl->sl.statem.rx.R.bsn + 1) & sl->sl.statem.sn_mask;
		if (!sl->sl.statem.msu_fisu_accepted) {
			ctrace(freemsg(mp));
			return;
		}
		sl_rb_congestion_function(q);
		if (sl->sl.statem.congestion_discard) {
			sl_cc_busy(q);
			ctrace(freemsg(mp));
			return;
		}
		if ((sl->sl.statem.rx.R.fsn == sl->sl.statem.rx.X.fsn)
		    && (sl->sl.statem.rx.len > 2)) {
			sl->sl.statem.rx.X.fsn =
			    (sl->sl.statem.rx.X.fsn + 1) & sl->sl.statem.sn_mask;
			bufq_queue(&sl->sl.rb, mp);
			sl->sl.statem.Cr++;
			if (sl->sl.statem.congestion_accept)
				sl_cc_busy(q);
			else
				sl_txc_fsnx_value(q);
			return;
		} else {
			ctrace(freemsg(mp));
			return;
		}
		return;
	}
	if (sl->sl.statem.rx.R.fib == sl->sl.statem.rx.X.fib) {
		if (sl->sl.statem.abnormal_fibr) {
			if (sl->sl.statem.unf == 1) {
				sl->sl.statem.abnormal_fibr = 0;
			} else {
				sl->sl.statem.unf = 1;
				ctrace(freemsg(mp));
				return;
			}
		}
		sl_lsc_fisu_msu_received(q);
		sl_txc_bsnr_and_bibr(q);
		sl->sl.statem.rx.F.fsn = (sl->sl.statem.rx.R.bsn + 1) & sl->sl.statem.sn_mask;
		if (!sl->sl.statem.msu_fisu_accepted) {
			ctrace(freemsg(mp));
			return;
		}
		sl_rb_congestion_function(q);
		if (sl->sl.statem.congestion_discard) {
			sl->sl.statem.rtr = 1;
			ctrace(freemsg(mp));
			sl_cc_busy(q);
			return;
		}
		if ((sl->sl.statem.rx.R.fsn == sl->sl.statem.rx.X.fsn)
		    && (sl->sl.statem.rx.len > 2)) {
			sl->sl.statem.rx.X.fsn =
			    (sl->sl.statem.rx.X.fsn + 1) & sl->sl.statem.sn_mask;
			sl->sl.statem.rtr = 0;
			bufq_queue(&sl->sl.rb, mp);
			sl->sl.statem.Cr++;
			if (sl->sl.statem.congestion_accept)
				sl_cc_busy(q);
			else
				sl_txc_fsnx_value(q);
			return;
		}
		if ((sl->sl.statem.rx.R.fsn ==
		     ((sl->sl.statem.rx.X.fsn - 1) & sl->sl.statem.sn_mask))) {
			ctrace(freemsg(mp));
			return;
		} else {
			if (sl->sl.statem.congestion_accept) {
				sl->sl.statem.rtr = 1;
				sl_cc_busy(q);	/* not required? */
				ctrace(freemsg(mp));
				return;
			} else {
				sl_txc_nack_to_be_sent(q);
				sl->sl.statem.rtr = 1;
				sl->sl.statem.rx.X.fib =
				    sl->sl.statem.rx.X.fib ? 0 : sl->sl.statem.ib_mask;
				ctrace(freemsg(mp));
				return;
			}
		}
	} else {
		if (sl->sl.statem.abnormal_fibr) {
			sl_lsc_link_failure(q, SL_FAIL_ABNORMAL_FIBR);
			ctrace(freemsg(mp));
			return;
		}
		if (sl->sl.statem.rtr == 1) {
			sl_txc_bsnr_and_bibr(q);
			sl->sl.statem.rx.F.fsn =
			    (sl->sl.statem.rx.R.bsn + 1) & sl->sl.statem.sn_mask;
			ctrace(freemsg(mp));
			return;
		}
		sl->sl.statem.abnormal_fibr = 1;
		sl->sl.statem.unf = 0;
		ctrace(freemsg(mp));
		return;
	}
}

STATIC INLINE void
sl_lsc_stop(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
		sl_iac_stop(q);	/* ok if not running */
		sl_timer_stop(q, t1);	/* ok if not running */
		sl_rc_stop(q);
		sl_suerm_stop(q);	/* ok if not running */
		sl_poc_stop(q);	/* ok if not running or not ITUT */
		sl_txc_send_sios(q);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.local_processor_outage = 0;
		sl->sl.statem.remote_processor_outage = 0;	/* ok of not ANSI */
		sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
}

STATIC INLINE void
sl_lsc_clear_rtb(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->sl.statem.local_processor_outage = 0;
		sl_txc_send_fisu(q);
		sl_txc_clear_rtb(q);
	}
}

STATIC INLINE void
sl_iac_correct_su(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		if (sl->sl.statem.iac_state == SL_STATE_PROVING) {
			if (sl->sl.statem.further_proving) {
				sl_timer_stop(q, t4);
				sl_aerm_start(q);
				sl_timer_start(q, t4);
				sl->sl.statem.further_proving = 0;
			}
		}
		return;
	}
	sdt_iac_correct_su_ind(q);
}

STATIC INLINE void
sl_iac_abort_proving(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		if (sl->sl.statem.iac_state == SL_STATE_PROVING) {
			sl->sl.statem.Cp++;
			if (sl->sl.statem.Cp == sl->sl.config.M) {
				sl_lsc_alignment_not_possible(q);
				sl_timer_stop(q, t4);
				sl_aerm_stop(q);
				sl->sl.statem.emergency = 0;
				sl->sl.statem.iac_state = SL_STATE_IDLE;
				return;
			}
			sl->sl.statem.further_proving = 1;
		}
		return;
	}
	sdt_iac_abort_proving_ind(q);
}

#define sl_lsc_flush_buffers sl_lsc_clear_buffers
STATIC INLINE int
sl_lsc_clear_buffers(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			return (QR_DONE);
		case SS7_PVAR_ANSI_92:
			if ((err = sl_rtb_cleared_ind(q)))
				return (err);
			sl->sl.statem.local_processor_outage = 0;	/* ??? */
			return (QR_DONE);
		}
	case SL_STATE_INITIAL_ALIGNMENT:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			return (QR_DONE);
		case SS7_PVAR_ANSI_92:
			if ((err = sl_rtb_cleared_ind(q)))
				return (err);
			sl->sl.statem.local_processor_outage = 0;
			return (QR_DONE);
		}
	case SL_STATE_ALIGNED_NOT_READY:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			return (QR_DONE);
		case SS7_PVAR_ANSI_92:
			if ((err = sl_rtb_cleared_ind(q)))
				return (err);
			sl->sl.statem.local_processor_outage = 0;
			sl_txc_send_fisu(q);
			sl->sl.statem.lsc_state = SL_STATE_ALIGNED_READY;
			return (QR_DONE);
		}
	case SL_STATE_PROCESSOR_OUTAGE:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl_txc_flush_buffers(q);
			sl->sl.statem.l3_indication_received = 1;
			if (sl->sl.statem.processor_outage)
				return (QR_DONE);
			sl->sl.statem.l3_indication_received = 0;
			sl_txc_send_msu(q);
			sl->sl.statem.local_processor_outage = 0;
			sl_rc_accept_msu_fisu(q);
			sl->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
			return (QR_DONE);
		case SS7_PVAR_ANSI_92:
			sl->sl.statem.local_processor_outage = 0;
			if ((err = sl_rc_clear_rb(q)))
				return (err);
			sl_rc_accept_msu_fisu(q);
			sl_txc_send_fisu(q);
			sl_txc_clear_tb(q);
			sl_txc_clear_rtb(q);
			return (QR_DONE);
		}
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_lsc_continue(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		if (sl->sl.statem.processor_outage)
			return;
		sl->sl.statem.l3_indication_received = 0;
		sl_txc_send_msu(q);
		sl->sl.statem.local_processor_outage = 0;
		sl_rc_accept_msu_fisu(q);
		sl->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
	}
}

STATIC INLINE void
sl_poc_local_processor_recovered(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.poc_state) {
	case SL_STATE_LOCAL_PROCESSOR_OUTAGE:
		sl_lsc_no_processor_outage(q);
		sl->sl.statem.poc_state = SL_STATE_IDLE;
		return;
	case SL_STATE_BOTH_PROCESSORS_OUT:
		sl->sl.statem.poc_state = SL_STATE_REMOTE_PROCESSOR_OUTAGE;
		return;
	}
}

#define sl_lsc_resume sl_lsc_local_processor_recovered
STATIC INLINE void
sl_lsc_local_processor_recovered(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		sl->sl.statem.local_processor_outage = 0;
		return;
	case SL_STATE_INITIAL_ALIGNMENT:
		sl->sl.statem.local_processor_outage = 0;
		return;
	case SL_STATE_ALIGNED_READY:
		return;
	case SL_STATE_ALIGNED_NOT_READY:
		if (sl->option.pvar != SS7_PVAR_ANSI_92)
			sl_poc_local_processor_recovered(q);
		sl->sl.statem.local_processor_outage = 0;
		sl_txc_send_fisu(q);
		if (sl->option.pvar == SS7_PVAR_ITUT_96)
			sl_rc_accept_msu_fisu(q);
		sl->sl.statem.lsc_state = SL_STATE_ALIGNED_READY;
		return;
	case SL_STATE_PROCESSOR_OUTAGE:
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl_poc_local_processor_recovered(q);
			sl_rc_retrieve_fsnx(q);
			sl_txc_send_fisu(q);	/* note 3: in fisu BSN <= FSNX-1 */
			sl->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
		case SS7_PVAR_ANSI_92:
			sl->sl.statem.local_processor_outage = 0;
			sl_rc_accept_msu_fisu(q);
			if (sl->sl.statem.remote_processor_outage) {
				sl_txc_send_fisu(q);
				sl_remote_processor_outage_ind(q);
				return;
			}
			sl_txc_send_msu(q);
			sl->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
			return;
		}
		return;
	}
}

#define sl_lsc_level_3_failure sl_lsc_local_processor_outage
STATIC INLINE void
sl_lsc_local_processor_outage(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_INITIAL_ALIGNMENT:
		sl->sl.statem.local_processor_outage = 1;
		return;
	case SL_STATE_ALIGNED_READY:
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->sl.statem.local_processor_outage = 1;
		else
			sl_poc_local_processor_outage(q);
		sl_txc_send_sipo(q);
		if (sl->option.pvar != SS7_PVAR_ITUT_93)	/* possible error 93 specs */
			sl_rc_reject_msu_fisu(q);
		sl->sl.statem.lsc_state = SL_STATE_ALIGNED_NOT_READY;
		return;
	case SL_STATE_IN_SERVICE:
		if (sl->option.pvar == SS7_PVAR_ANSI_92) {
			sl->sl.statem.local_processor_outage = 1;
		} else {
			sl_poc_local_processor_outage(q);
			sl->sl.statem.processor_outage = 1;
		}
		sl_txc_send_sipo(q);
		sl_rc_reject_msu_fisu(q);
		if (sl->option.pvar == SS7_PVAR_ANSI_92) {
			sl_rc_align_fsnx(q);
			sl_cc_stop(q);
		}
		sl->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_PROCESSOR_OUTAGE:
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->sl.statem.local_processor_outage = 1;
		else
			sl_poc_local_processor_outage(q);
		sl_txc_send_sipo(q);
		return;
	}
}

STATIC INLINE void
sl_iac_emergency(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.iac_state) {
	case SL_STATE_PROVING:
		sl_txc_send_sie(q);
		sl_timer_stop(q, t4);
		sl->sl.statem.t4v = sl->sl.config.t4e;
		sl_aerm_stop(q);
		sl_aerm_set_ti_to_tie(q);
		sl_aerm_start(q);
		sl_timer_start(q, t4);
		sl->sl.statem.further_proving = 0;
		return;
	case SL_STATE_ALIGNED:
		sl_txc_send_sie(q);
		sl->sl.statem.t4v = sl->sl.config.t4e;
		return;
	case SL_STATE_IDLE:
	case SL_STATE_NOT_ALIGNED:
		sl->sl.statem.emergency = 1;
	}
}

STATIC INLINE void
sl_lsc_emergency(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.emergency = 1;
	sl_iac_emergency(q);	/* added to pass Q.781/Test 1.20 */
}

STATIC INLINE void
sl_lsc_emergency_ceases(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.emergency = 0;
}

STATIC INLINE void
sl_iac_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.iac_state == SL_STATE_IDLE) {
		sl_txc_send_sio(q);
		sl_timer_start(q, t2);
		sl->sl.statem.iac_state = SL_STATE_NOT_ALIGNED;
	}
}

STATIC INLINE void
sl_daedt_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sdt.statem.daedt_state = SDT_STATE_IN_SERVICE;
	sl->sdl.statem.tx_state = SDL_STATE_IN_SERVICE;
	sl->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING);
}

STATIC INLINE void
sl_txc_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	sl->sl.statem.forced_retransmission = 0;	/* ok if basic */
	sl->sl.statem.sib_received = 0;
	sl->sl.statem.Ct = 0;
	sl->sl.statem.rtb_full = 0;
	sl->sl.statem.clear_rtb = 0;	/* ok if ITU */
	if (sl->option.pvar == SS7_PVAR_ANSI_92) {
		sl->sl.statem.tx.sio = LSSU_SIOS;
		sl->sl.statem.lssu_available = 1;
	}
	sl->sl.statem.msu_inhibited = 0;
	if (sl->option.popt & SS7_POPT_XSN) {
		sl->sl.statem.sn_mask = 0x7fff;
		sl->sl.statem.ib_mask = 0x8000;
	} else {
		sl->sl.statem.sn_mask = 0x7f;
		sl->sl.statem.ib_mask = 0x80;
	}
	sl->sl.statem.tx.L.fsn = sl->sl.statem.tx.N.fsn = sl->sl.statem.sn_mask;
	sl->sl.statem.tx.X.fsn = 0;
	sl->sl.statem.tx.N.fib = sl->sl.statem.tx.N.bib = sl->sl.statem.ib_mask;
	sl->sl.statem.tx.F.fsn = 0;
	sl->sl.statem.Cm = 0;
	sl->sl.statem.Z = 0;
	sl->sl.statem.z_ptr = NULL;	/* ok if basic */
	if (sl->sl.statem.txc_state == SL_STATE_IDLE) {
		if (sl->option.pvar != SS7_PVAR_ANSI_92)
			sl->sl.statem.lssu_available = 0;
		sl_daedt_start(q);
	}
	sl->sl.statem.txc_state = SL_STATE_IN_SERVICE;
	return;
}

STATIC INLINE void
sl_lsc_start(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		sl_rc_start(q);
		sl_txc_start(q);	/* Note 2 */
		if (sl->sl.statem.emergency)
			sl_iac_emergency(q);
		sl_iac_start(q);
		sl->sl.statem.lsc_state = SL_STATE_INITIAL_ALIGNMENT;
	}
}

/*
 *  Note 2: There is a difference here between ANSI_92 and ITUT_93/96 in that
 *  the transmitters in the ANSI_92 case may transmit one or two SIOSs before
 *  transmitting the first SIO of the initial alignment procedure.  ITUT will
 *  continue idling FISU or LSSU as before the start, then transmit the first
 *  SIO.  These are equivalent.  Because the LSC is in the OUT OF SERVICE
 *  state, the transmitters should be idling SIOS anyway.
 */

STATIC INLINE int
sl_lsc_retrieve_bsnt(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		return sl_rc_retrieve_bsnt(q);
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_retrieval_request_and_fsnc(queue_t *q, sl_ulong fsnc)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		return sl_txc_retrieval_request_and_fsnc(q, fsnc);
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_aerm_set_ti_to_tin(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sdt.statem.aerm_state == SDT_STATE_IDLE)
		sl->sdt.statem.Ti = sl->sdt.config.Tin;
}

/*
 *  This power-on sequence should only be performed once, regardless of how
 *  many times the device driver is opened or closed.  This initializes the
 *  transmitters to send SIOS and should never be changed hence.
 */

STATIC INLINE void
sl_lsc_power_on(queue_t *q)
{
	sl_t *sl = PRIV(q);
	switch (sl->sl.statem.lsc_state) {
	case SL_STATE_POWER_OFF:
		trace();
		sl_txc_start(q);	/* Note 3 */
		sl_txc_send_sios(q);	/* not necessary for ANSI */
		sl_aerm_set_ti_to_tin(q);
		sl->sl.statem.local_processor_outage = 0;
		sl->sl.statem.emergency = 0;
		sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
}

/*
 *  Note 3: There is a difference here between ANSI_92 and ITUT_93/96 in that
 *  the transmitters in the ITUT case may transmit one or two FISUs before
 *  transmitting SIOS on initial power-up.  ANSI will send SIOS on power-up.
 *  ANSI is the correct procedure as transmitters should always idle SIOS on
 *  power-up.
 */

/*
 *  The transmit congestion algorithm is an implementation dependent algorithm
 *  but is suggested as being based on TB and/or RTB buffer occupancy.  With
 *  STREAMS we can use octet count buffer occupancy over message count
 *  occupancy, because congestion in transmission is more related to octet
 *  count (because it determines transmission latency).
 *
 *  We check the total buffer occupancy and apply the necessary congestion
 *  control signal as per configured abatement, onset and discard thresholds.
 */
STATIC void
sl_check_congestion(queue_t *q)
{
	sl_t *sl = PRIV(q);
	unsigned int occupancy = sl->wq->q_count + sl->sl.tb.q_count + sl->sl.rtb.q_count;
	int old_cong_status = sl->sl.statem.cong_status;
	int old_disc_status = sl->sl.statem.disc_status;
	int multi = sl->option.popt & SS7_POPT_MPLEV;
	switch (sl->sl.statem.cong_status) {
	case 0:
		if (occupancy >= sl->sl.config.tb_onset_1) {
			sl->sl.statem.cong_status = 1;
			if (occupancy >= sl->sl.config.tb_discd_1) {
				sl->sl.statem.disc_status = 1;
				if (!multi)
					break;
				if (occupancy >= sl->sl.config.tb_onset_2) {
					sl->sl.statem.cong_status = 2;
					if (occupancy >= sl->sl.config.tb_discd_2) {
						sl->sl.statem.disc_status = 2;
						if (occupancy >= sl->sl.config.tb_onset_3) {
							sl->sl.statem.cong_status = 3;
							if (occupancy >= sl->sl.config.tb_discd_3) {
								sl->sl.statem.disc_status = 3;
							}
						}
					}
				}
			}
		}
		break;
	case 1:
		if (occupancy < sl->sl.config.tb_abate_1) {
			sl->sl.statem.cong_status = 0;
			sl->sl.statem.disc_status = 0;
		} else {
			if (!multi)
				break;
			if (occupancy >= sl->sl.config.tb_onset_2) {
				sl->sl.statem.cong_status = 2;
				if (occupancy >= sl->sl.config.tb_discd_2) {
					sl->sl.statem.disc_status = 2;
					if (occupancy >= sl->sl.config.tb_onset_3) {
						sl->sl.statem.cong_status = 3;
						if (occupancy >= sl->sl.config.tb_discd_3) {
							sl->sl.statem.disc_status = 3;
						}
					}
				}
			}
		}
		break;
	case 2:
		if (!multi) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->sl.config.tb_abate_2) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			if (occupancy < sl->sl.config.tb_abate_1) {
				sl->sl.statem.cong_status = 0;
				sl->sl.statem.disc_status = 0;
			}
		} else if (occupancy >= sl->sl.config.tb_onset_3) {
			sl->sl.statem.cong_status = 3;
			if (occupancy >= sl->sl.config.tb_discd_3) {
				sl->sl.statem.disc_status = 3;
			}
		}
		break;
	case 3:
		if (!multi) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->sl.config.tb_abate_3) {
			sl->sl.statem.cong_status = 2;
			sl->sl.statem.disc_status = 2;
			if (occupancy < sl->sl.config.tb_abate_2) {
				sl->sl.statem.cong_status = 1;
				sl->sl.statem.disc_status = 1;
				if (occupancy < sl->sl.config.tb_abate_1) {
					sl->sl.statem.cong_status = 0;
					sl->sl.statem.disc_status = 0;
				}
			}
		}
		break;
	}
	if (sl->sl.statem.cong_status != old_cong_status
	    || sl->sl.statem.disc_status != old_disc_status) {
		if (sl->sl.statem.cong_status < old_cong_status)
			sl_link_congestion_ceased_ind(q, sl->sl.statem.cong_status,
						      sl->sl.statem.disc_status);
		else {
			if (sl->sl.statem.cong_status > old_cong_status) {
				if (sl->sl.notify.events & SL_EVT_CONGEST_ONSET_IND
				    && !sl->sl.stats.sl_cong_onset_ind[sl->sl.statem.
								       cong_status]++) {
					sl_link_congested_ind(q, sl->sl.statem.cong_status,
							      sl->sl.statem.disc_status);
					return;
				}
			} else {
				if (sl->sl.notify.events & SL_EVT_CONGEST_DISCD_IND
				    && !sl->sl.stats.sl_cong_discd_ind[sl->sl.statem.
								       disc_status]++) {
					sl_link_congested_ind(q, sl->sl.statem.cong_status,
							      sl->sl.statem.disc_status);
					return;
				}
			}
			sl_link_congested_ind(q, sl->sl.statem.cong_status,
					      sl->sl.statem.disc_status);
		}
	}
}

STATIC INLINE void
sl_txc_message_for_transmission(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	bufq_queue(&sl->sl.tb, mp);
	sl->sl.statem.Cm++;
	sl_check_congestion(q);
}

STATIC INLINE int
sl_lsc_pdu(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	mblk_t *dp = mp;
	int hlen = (sl->option.popt & SS7_POPT_XSN) ? 6 : 3;
	ensure(dp, return (-EFAULT));
	if (sl->sl.tb.q_count > 1024)
		return (-ENOBUFS);
	if (dp->b_datap->db_type == M_DATA)
		mp = NULL;
	else
		dp = mp->b_cont;
	ensure(dp, return (-EFAULT));
	if (mp) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr = mp->b_datap->db_base;
		mp->b_wptr = mp->b_rptr + hlen;
	} else if (dp->b_datap->db_base - dp->b_rptr >= hlen) {
		mp = dp;
		mp->b_rptr -= hlen;
	} else if ((mp = sl_allocb(q, hlen, BPRI_MED))) {
		mp->b_cont = dp;
		mp->b_wptr = mp->b_rptr + hlen;
	} else {
		return (-ENOBUFS);
	}
	sl_txc_message_for_transmission(q, mp);
	return (QR_ABSORBED);
}

STATIC INLINE void
sl_aerm_su_in_error(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sdt.statem.aerm_state == SDT_STATE_MONITORING) {
		sl->sdt.statem.Ca++;
		if (sl->sdt.statem.Ca == sl->sdt.statem.Ti) {
			sl->sdt.statem.aborted_proving = 1;
			sl_iac_abort_proving(q);
			sl->sdt.statem.Ca--;
			sl->sdt.statem.aerm_state = SDT_STATE_IDLE;
		}
	}
}

STATIC INLINE void
sl_aerm_correct_su(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sdt.statem.aerm_state == SDT_STATE_IDLE) {
		if (sl->sdt.statem.aborted_proving) {
			sl_iac_correct_su(q);
			sl->sdt.statem.aborted_proving = 0;
		}
	}
}

STATIC INLINE void
sl_suerm_su_in_error(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		sl->sdt.statem.Cs++;
		if (sl->sdt.statem.Cs >= sl->sdt.config.T) {
			sl_lsc_link_failure(q, SL_FAIL_SUERM_EIM);
			sl->sdt.statem.Ca--;
			sl->sdt.statem.suerm_state = SDT_STATE_IDLE;
			return;
		}
		sl->sdt.statem.Ns++;
		if (sl->sdt.statem.Ns >= sl->sdt.config.D) {
			sl->sdt.statem.Ns = 0;
			if (sl->sdt.statem.Cs)
				sl->sdt.statem.Cs--;
		}
	}
}

STATIC INLINE void
sl_eim_su_in_error(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sdt.statem.eim_state == SDT_STATE_MONITORING)
		sl->sdt.statem.interval_error = 1;
}

STATIC INLINE void
sl_suerm_correct_su(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		sl->sdt.statem.Ns++;
		if (sl->sdt.statem.Ns >= sl->sdt.config.D) {
			sl->sdt.statem.Ns = 0;
			if (sl->sdt.statem.Cs)
				sl->sdt.statem.Cs--;
		}
	}
}

STATIC INLINE void
sl_eim_correct_su(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sdt.statem.eim_state == SDT_STATE_MONITORING)
		sl->sdt.statem.su_received = 1;
}

STATIC INLINE void
sl_daedr_correct_su(queue_t *q)
{
	sl_eim_correct_su(q);
	sl_suerm_correct_su(q);
	sl_aerm_correct_su(q);
}

/*
 *  Hooks to underlying driver
 *  -----------------------------------
 */
STATIC INLINE void
sl_daedr_su_in_error(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		sl_eim_su_in_error(q);
		sl_suerm_su_in_error(q);
		sl_aerm_su_in_error(q);
	} else if (sl->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		/*
		   cancel compression 
		 */
		if (sl->sdt.rx_cmp) {
			printd(("SU in error\n"));
			sl_fast_freemsg(xchg(&sl->sdt.rx_cmp, NULL));
		}
	}
}

STATIC INLINE void
sl_daedr_received_bits(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		printd(("%s: %p: -> SL_RC_SIGNAL_UNIT\n", SL_TPI_MOD_NAME, sl));
		sl_rc_signal_unit(q, mp);
		sl_daedr_correct_su(q);
		return;
	} else if (sl->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		int i, len, mlen = (sl->option.popt & SS7_POPT_XSN) ? 8 : 5;
		// printd(("%s: %p: -> SL_DAEDR_RECEIVED_BITS\n", SL_TPI_MOD_NAME, sl));
		if (mp) {
			len = msgdsize(mp);
			if (sl->sdt.rx_cmp) {
				if (len > mlen || len != msgdsize(sl->sdt.rx_cmp))
					goto no_match;
				for (i = 0; i < len; i++)
					if (mp->b_rptr[i] != sl->sdt.rx_cmp->b_rptr[i])
						goto no_match;
				sl->sdt.rx_repeat++;
				sl->sdt.stats.rx_sus_compressed++;
				ctrace(freemsg(mp));
				return;
			      no_match:
				if (sl->sdt.rx_repeat) {
#if 0
					mblk_t *cp;
					if ((cp = dupb(sl->sdt.rx_cmp))) {
						if (sdt_rc_signal_unit_ind(q, cp, sl->sdt.rx_repeat)
						    != QR_ABSORBED) {
							sl->sdt.stats.rx_buffer_overflows++;
							ctrace(freemsg(cp));
						}
					}
#endif
					sl_daedr_correct_su(q);
					sl->sdt.rx_repeat = 0;
				}
			}
			if (len <= mlen) {
				if (sl->sdt.rx_cmp
				    || (sl->sdt.rx_cmp = sl_fast_allocb(mlen, BPRI_HI))) {
					bcopy(mp->b_rptr, sl->sdt.rx_cmp->b_rptr, len);
					sl->sdt.rx_cmp->b_wptr += len;
					sl->sdt.rx_repeat = 0;
				}
			}
			if (sdt_rc_signal_unit_ind(q, mp, 1) != QR_ABSORBED) {
				sl->sdt.stats.rx_buffer_overflows++;
				ctrace(freemsg(mp));
			}
			sl_daedr_correct_su(q);
		} else
			swerr();
		return;
	}
	trace();
}

STATIC INLINE mblk_t *
sl_daedt_transmission_request(queue_t *q)
{
	sl_t *sl = PRIV(q);
	mblk_t *mp;
	if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		mp = sl_txc_transmission_request(q);
	} else if (sl->sdt.statem.daedt_state != SDT_STATE_IDLE) {
		if ((mp = bufq_dequeue(&sl->sdt.tb))) {
			int len = msgdsize(mp);
			int hlen = (sl->option.popt & SS7_POPT_XSN) ? 6 : 3;
			int mlen = hlen + 2;
			if (!sl->sdt.tb.q_count)
				qenable(sl->wq);	/* back-enable */
			if (mlen < hlen)
				goto dont_repeat;
			if (mlen == hlen + 1 || mlen == hlen + 2) {
				int li, sio;
				if (sl->option.popt & SS7_POPT_XSN) {
					li = ((mp->b_rptr[5] << 8) | mp->b_rptr[4]) & 0x1ff;
					sio = mp->b_rptr[6];
				} else {
					li = mp->b_rptr[2] & 0x3f;
					sio = mp->b_rptr[3];
				}
				if (li != mlen - hlen)
					goto dont_repeat;
				switch (sio) {
				case LSSU_SIO:
				case LSSU_SIN:
				case LSSU_SIE:
				case LSSU_SIOS:
				case LSSU_SIPO:
					break;
				case LSSU_SIB:
				default:
					goto dont_repeat;
				}
			}
			if (sl->sdt.tx_cmp || (sl->sdt.tx_cmp = sl_fast_allocb(mlen, BPRI_HI))) {
				mblk_t *cp = sl->sdt.tx_cmp;
				if (len > mlen)
					len = hlen;
				cp->b_rptr = cp->b_datap->db_base;
				bcopy(mp->b_rptr, cp->b_rptr, len);
				cp->b_wptr = cp->b_rptr + len;
				/*
				   always correct length indicator 
				 */
				if (sl->option.popt && SS7_POPT_XSN) {
					cp->b_rptr[4] &= 0x00;
					cp->b_rptr[5] &= 0xfe;
					cp->b_rptr[4] += (len - hlen);
				} else {
					cp->b_rptr[2] &= 0xc0;
					cp->b_rptr[2] += (len - hlen);
				}
				sl->sdt.tx_repeat = 0;
				goto done;
			}
		      dont_repeat:
			if (sl->sdt.tx_cmp)
				sl_fast_freemsg(xchg(&sl->sdt.tx_cmp, NULL));
		} else if (sl->sdt.tx_cmp && (mp = dupmsg(sl->sdt.tx_cmp))) {
			sl->sdt.stats.tx_sus_repeated++;
			sl->sdt.tx_repeat++;
		}
	} else
		return (NULL);
      done:
	if (!mp) {
		sl->sdt.stats.tx_buffer_overflows++;
		trace();
	}
	return (mp);
}

/*
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */
/*
 *  RX WAKEUP
 *  -----------------------------------
 */
STATIC int
sl_rx_wakeup(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.rb.q_msgs && canputnext(q)) {
		switch (sl->sl.statem.lsc_state) {
		case SL_STATE_INITIAL_ALIGNMENT:
		case SL_STATE_ALIGNED_READY:
		case SL_STATE_ALIGNED_NOT_READY:
			/*
			   we shouldn't have receive buffers around in these states 
			 */
			swerr();
			bufq_purge(&sl->sl.rb);
			return (-EFAULT);
		case SL_STATE_OUT_OF_SERVICE:
			/*
			   we keep receive buffers hanging around in these states 
			 */
			return (0);
		case SL_STATE_PROCESSOR_OUTAGE:
			if (sl->sl.statem.local_processor_outage) {
				/*
				   we can't deliver 
				 */
				return (0);
			}
			/*
			   fall thru 
			 */
		case SL_STATE_IN_SERVICE:
			/*
			   when in service we deliver as many buffers as we can 
			 */
			do {
				mblk_t *mp;
				mp = bufq_dequeue(&sl->sl.rb);
				sl->sl.statem.Cr--;
				putnext(q, mp);
			} while (sl->sl.rb.q_msgs && canputnext(q));
			return (0);
		}
	}
	return (0);
}

/*
 *  TX WAKEUP
 *  -----------------------------------
 */
STATIC int
sl_tx_wakeup(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if ((sl->sl.statem.lsc_state != SL_STATE_POWER_OFF
	     && sl->sl.statem.txc_state == SL_STATE_IN_SERVICE)
	    || (sl->sl.statem.lsc_state == SL_STATE_POWER_OFF
		&& sl->sdt.statem.daedt_state != SDT_STATE_IDLE)) {
		mblk_t *mp, *dp;
		long tdiff;
		int size;
		for (;;) {
			if ((tdiff = jiffies - sl->sdl.timestamp) < 0) {
				/*
				   throttle back for a t9 
				 */
				// printd(("%s: %p: %s throttling back a tick\n", SL_TPI_MOD_NAME, sl,
				// __FUNCTION__));
				if (!sl->sdl.timer.t9)
					sl_timer_start(q, t9);
				return (0);
			}
			if (tdiff > 0) {
				sl->sdl.bytecount = 0;
				sl->sdl.timestamp = jiffies;
			}
			if (sl->t.serv_type == T_CLTS)
				mp = t_unitdata_req(q);
			else
				mp = t_data_req(q);
			if (!mp || !canputnext(q) || !(dp = sl_daedt_transmission_request(q))) {
				sl->sdt.stats.tx_buffer_overflows++;
				printd(("%s: %p: %s buffer overflow\n", SL_TPI_MOD_NAME, sl,
					__FUNCTION__));
				return (0);
			}
			size = msgdsize(dp);
			mp->b_cont = dp;
			// printd(("%s: %p: <- T_UNITDATA_REQ [%d]\n", SL_TPI_MOD_NAME, sl, size));
			putnext(q, mp);
			sl->sdt.stats.tx_bytes += size;
			sl->sdt.stats.tx_sus++;
			sl->sdl.bytecount += size;
			while (sl->sdl.bytecount >= sl->sdl.tickbytes) {
				sl->sdl.bytecount -= sl->sdl.tickbytes;
				sl->sdl.timestamp++;
			}
			if (sl->sdl.config.ifclock == SDL_CLOCK_TICK
			    && !(sl->option.popt & SS7_POPT_PCR)
			    && (sl->sl.statem.rtb_full || sl->sl.statem.msu_inhibited
				|| sl->sl.statem.lssu_available || !sl->sl.tb.q_msgs)) {
				if (!sl->sdl.timer.t9)
					sl_timer_start(q, t9);
				// printd(("%s: %p: %s sleeping for a tick\n", SL_TPI_MOD_NAME, sl,
				// __FUNCTION__));
				return (0);
			}
		}
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Events
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T1 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t1_expiry(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	if ((err = sl_out_of_service_ind(q, SL_FAIL_T1_TIMEOUT)))
		return (err);
	sl->sl.statem.failure_reason = SL_FAIL_T1_TIMEOUT;
	sl_rc_stop(q);
	sl_suerm_stop(q);
	sl_txc_send_sios(q);
	sl->sl.statem.emergency = 0;
	if (sl->sl.statem.lsc_state == SL_STATE_ALIGNED_NOT_READY) {
		sl_poc_stop(q);	/* ok if ANSI */
		sl->sl.statem.local_processor_outage = 0;
	}
	sl->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	return (QR_DONE);
}

/*
 *  T2 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t2_expiry(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	if (sl->sl.statem.iac_state == SL_STATE_NOT_ALIGNED) {
		if ((err = sl_lsc_alignment_not_possible(q)))
			return (err);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.iac_state = SL_STATE_IDLE;
	}
	return (QR_DONE);
}

/*
 *  T3 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t3_expiry(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	if (sl->sl.statem.iac_state == SL_STATE_ALIGNED) {
		if ((err = sl_lsc_alignment_not_possible(q)))
			return (err);
		sl->sl.statem.emergency = 0;
		sl->sl.statem.iac_state = SL_STATE_IDLE;
	}
	return (QR_DONE);
}

/*
 *  T4 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t4_expiry(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.iac_state == SL_STATE_PROVING) {
		if (sl->sl.statem.further_proving) {
			sl_aerm_start(q);
			sl_timer_start(q, t4);
			sl->sl.statem.further_proving = 0;
		} else {
			sl_lsc_alignment_complete(q);
			sl_aerm_stop(q);
			sl->sl.statem.emergency = 0;
			sl->sl.statem.iac_state = SL_STATE_IDLE;
		}
	}
	return (QR_DONE);
}

/*
 *  T5 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t5_expiry(queue_t *q)
{
	sl_t *sl = PRIV(q);
	if (sl->sl.statem.cc_state == SL_STATE_BUSY) {
		sl_txc_send_sib(q);
		sl_timer_start(q, t5);
	}
	return (QR_DONE);
}

/*
 *  T6 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t6_expiry(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	if ((err = sl_lsc_link_failure(q, SL_FAIL_CONG_TIMEOUT)))
		return (err);
	sl->sl.statem.sib_received = 0;
	sl_timer_stop(q, t7);
	return (QR_DONE);
}

/*
 *  T7 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t7_expiry(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	if ((err = sl_lsc_link_failure(q, SL_FAIL_ACK_TIMEOUT)))
		return (err);
	sl_timer_stop(q, t6);
	if (sl->option.pvar == SS7_PVAR_ITUT_96)
		sl->sl.statem.sib_received = 0;
	return (QR_DONE);
}

/*
 *  T8 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t8_expiry(queue_t *q)
{
	sl_t *sl = PRIV(q);
	int err;
	if (sl->sdt.statem.eim_state == SDT_STATE_MONITORING) {
		sl_timer_start(q, t8);
		if (sl->sdt.statem.su_received) {
			sl->sdt.statem.su_received = 0;
			if (!sl->sdt.statem.interval_error) {
				if ((sl->sdt.statem.Ce -= sl->sdt.config.De) < 0)
					sl->sdt.statem.Ce = 0;
				return (QR_DONE);
			}
		}
		sl->sdt.statem.Ce += sl->sdt.config.Ue;
		if (sl->sdt.statem.Ce > sl->sdt.config.Te) {
			if ((err = sl_lsc_link_failure(q, SL_FAIL_SUERM_EIM))) {
				sl->sdt.statem.Ce -= sl->sdt.config.Ue;
				return (err);
			}
			sl->sdt.statem.eim_state = SDT_STATE_IDLE;
		}
		sl->sdt.statem.interval_error = 0;
	}
	return (QR_DONE);
}

/*
 *  T9 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t9_expiry(queue_t *q)
{
	return sl_tx_wakeup(PRIV(q)->wq);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LM User -> LM Provider (SS7IP) Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		return lmi_info_ack(q, NULL, 0);
	}
	return lmi_error_ack(q, LMI_INFO_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 *  Attachment requires a local address as a PPA (Physical Point of Attachment).  The local
 *  address is necessary for T_BIND on both connection oriented and connectionless transports.
 *  For style 2 transports, we copy the PPA and bind the transport with T_BIND.  Style 1
 *  transports are already bound and this primitive is invalid for style 1 transports.
 */
STATIC int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int mlen = mp->b_wptr - mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state != LMI_UNUSABLE) {
			if (sl->style == LMI_STYLE2) {
				if (sl->state == LMI_UNATTACHED) {
					sl->state = LMI_ATTACH_PENDING;
					if (mlen - sizeof(*p) >= sl->t.add_size) {
						bcopy(p->lmi_ppa, &sl->t.loc, sl->t.add_size);
						/*
						   start bind in motion 
						 */
						return t_bind_req(q);
					} else {
						ptrace(("%s: PROTO: bad ppa (too short)\n",
							SL_TPI_MOD_NAME));
						return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_BADPPA,
								     EMSGSIZE);
					}
				} else {
					ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
					return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_OUTSTATE,
							     EPROTO);
				}
			} else {
				ptrace(("%s: PROTO: primitive not support for style\n",
					SL_TPI_MOD_NAME));
				return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
			}
		} else {
			/*
			   wait for stream to become usable 
			 */
			return (-EAGAIN);
		}
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 *  Detaching requires that the stream be detached from the local address.  This results in a
 *  T_UNBIND connection oriented and connectionless transports.  For style 2 transports, we 
 *  perform the unbinding operation.  For style 1 transports, we were already bound and this
 *  primitive is invalid for style 1 transports.
 */
STATIC int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state != LMI_UNUSABLE) {
			if (sl->style == LMI_STYLE2) {
				if (sl->state == LMI_DISABLED) {
					sl->state = LMI_DETACH_PENDING;
					bzero(&sl->t.loc, sl->t.add_size);
					/*
					   start unbind in motion 
					 */
					return t_unbind_req(q);
				} else {
					ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
					return lmi_error_ack(q, LMI_DETACH_REQ, LMI_OUTSTATE,
							     EPROTO);
				}
			} else {
				ptrace(("%s: PROTO: primitive not support for style\n",
					SL_TPI_MOD_NAME));
				return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
			}
		} else {
			/*
			   wait for stream to become usable 
			 */
			return (-EAGAIN);
		}
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return lmi_error_ack(q, LMI_DETACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 *  For style 1 connection oriented transports, we are completely connected and can simply
 *  acknowledge the enable.  For style 1 connectionless transports we need to know the remote
 *  address of the peer and require a remote address as part of the enable.  For style 2
 *  connection oriented transports we need to know the remote address for connection or if no
 *  remote address is provided, simply stay listening.  (Note: style 2 connection oriented
 *  transports are quite insecure for this reason.  Style 1 is preferrable for connection
 *  oriented transports for a number of reasons).
 */
STATIC int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	int mlen;
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mlen = mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state != LMI_UNUSABLE) {
			if (sl->state == LMI_DISABLED) {
				sl->state = LMI_ENABLE_PENDING;
				if (sl->t.serv_type == T_CLTS) {
					assure(sl->t.state == TS_IDLE);
					if (mlen - sizeof(*p) < sl->t.add_size)
						goto emsgsize;
					bcopy(p->lmi_rem, &sl->t.rem, sl->t.add_size);
					return lmi_enable_con(q);
				} else {
					if (sl->style == LMI_STYLE1) {
						assure(sl->t.state == TS_DATA_XFER);
						return lmi_enable_con(q);
					} else {
						assure(sl->t.state == TS_IDLE);
						if (mlen == sizeof(*p))
							/*
							   wait for T_CONN_IND 
							 */
							return (QR_DONE);
						if (mlen - sizeof(*p) < sl->t.add_size)
							goto emsgsize;
						bcopy(p->lmi_rem, &sl->t.rem, sl->t.add_size);
						/*
						   start connection in motion 
						 */
						return t_conn_req(q);
					}
				}
			} else {
				ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
				return lmi_error_ack(q, LMI_ENABLE_REQ, LMI_OUTSTATE, EPROTO);
			}
		} else {
			/*
			   wait for stream to become usable 
			 */
			return (-EAGAIN);
		}
	}
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return lmi_error_ack(q, LMI_ENABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 *  For style 1 connection oriented transports, we stay completely connected and can simply
 *  acknowledge the disable.  For style 1 connectionless transports we may also simply
 *  acknowledge the disable.  Fort style 2 connection oriented transports we need to
 *  disconnect the connection.
 */
STATIC int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state != LMI_UNUSABLE) {
			if (sl->state == LMI_ENABLED) {
				sl->state = LMI_DISABLE_PENDING;
				if (sl->style == LMI_STYLE1 || sl->t.serv_type == T_CLTS)
					return lmi_disable_con(q);
				/*
				   start disconnect in motion 
				 */
				if (sl->t.serv_type == T_COTS_ORD)
					return t_ordrel_req(q);
				return t_discon_req(q, 0);
			} else {
				ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
				return lmi_error_ack(q, LMI_DISABLE_REQ, LMI_OUTSTATE, EPROTO);
			}
		} else {
			/*
			   wait for stream to become available 
			 */
			return (-EAGAIN);
		}
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return lmi_error_ack(q, LMI_DISABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		ptrace(("%s: PROTO: Primitive is not supported\n", SL_TPI_MOD_NAME));
		return lmi_error_ack(q, LMI_OPTMGMT_REQ, LMI_NOTSUPP, EOPNOTSUPP);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return lmi_error_ack(q, LMI_OPTMGMT_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL User -> SL Provider (SS7IP) Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
sl_send_data(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	int ret;
	psw_t flags;
	if (sl->state != LMI_ENABLED) {
		swerr();
		return (-EPROTO);
	}
	spin_lock_irqsave(&sl->lock, flags);
	{
		if (sl->sl.statem.lsc_state != SL_STATE_POWER_OFF)
			ret = sl_lsc_pdu(q, mp);
		else {
			if (sl->sdt.tb.q_count > 1024) {
				swerr();
				ret = -EBUSY;
			} else {
				printd(("%s: %p: Queuing SDT data\n", SL_TPI_MOD_NAME, sl));
				bufq_queue(&sl->sdt.tb, mp);
				ret = QR_ABSORBED;
			}
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC int
sl_pdu_req(queue_t *q, mblk_t *mp)
{
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			int ret;
			spin_lock_irqsave(&sl->lock, flags);
			{
				ret = sl_lsc_pdu(q, mp);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (ret);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC int
sl_emergency_req(queue_t *q, mblk_t *mp)
{
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_emergency(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC int
sl_emergency_ceases_req(queue_t *q, mblk_t *mp)
{
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_emergency_ceases(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
STATIC int
sl_start_req(queue_t *q, mblk_t *mp)
{
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_start(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
STATIC int
sl_stop_req(queue_t *q, mblk_t *mp)
{
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_stop(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC int
sl_retrieve_bsnt_req(queue_t *q, mblk_t *mp)
{
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			int err;
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				err = sl_lsc_retrieve_bsnt(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (err);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC int
sl_retrieval_request_and_fsnc_req(queue_t *q, mblk_t *mp)
{
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			int err;
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				err = sl_lsc_retrieval_request_and_fsnc(q, p->sl_fsnc);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (err);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC int
sl_resume_req(queue_t *q, mblk_t *mp)
{
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_resume(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC int
sl_clear_buffers_req(queue_t *q, mblk_t *mp)
{
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			int err;
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				err = sl_lsc_clear_buffers(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (err);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC int
sl_clear_rtb_req(queue_t *q, mblk_t *mp)
{
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_clear_rtb(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC int
sl_local_processor_outage_req(queue_t *q, mblk_t *mp)
{
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_local_processor_outage(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC int
sl_congestion_discard_req(queue_t *q, mblk_t *mp)
{
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_congestion_discard(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
STATIC int
sl_congestion_accept_req(queue_t *q, mblk_t *mp)
{
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_congestion_accept(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
STATIC int
sl_no_congestion_req(queue_t *q, mblk_t *mp)
{
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_no_congestion(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
STATIC int
sl_power_on_req(queue_t *q, mblk_t *mp)
{
	sl_power_on_req_t *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		if (sl->state == LMI_ENABLED) {
			psw_t flags;
			spin_lock_irqsave(&sl->lock, flags);
			{
				sl_lsc_power_on(q);
			}
			spin_unlock_irqrestore(&sl->lock, flags);
			return (QR_DONE);
		}
		ptrace(("%s: PROTO: out of state\n", SL_TPI_MOD_NAME));
		return (-EPROTO);
	}
	ptrace(("%s: PROTO: M_PROTO block too short\n", SL_TPI_MOD_NAME));
	return (-EMSGSIZE);
}

#if 0
/*
 *  SL_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
sl_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_NOTIFY_REQ
 *  -----------------------------------
 */
STATIC int
sl_notify_req(queue_t *q, mblk_t *mp)
{
}
#endif

/*
 *  SDT_DAEDT_TRANSMISSION_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedt_transmission_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	int ret;
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		if (sl->sdt.tb.q_count > 1024)
			return (-EBUSY);
		else {
			bufq_queue(&sl->sdt.tb, mp->b_cont);
			ret = (QR_TRIMMED);
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}

/*
 *  SDT_DAEDT_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedt_start_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl_daedt_start(q);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (QR_DONE);
}

/*
 *  SDT_DAEDR_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedr_start_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl_daedr_start(q);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (QR_DONE);
}

/*
 *  SDT_AERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_start_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl_aerm_start(q);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (QR_DONE);
}

/*
 *  SDT_AERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_stop_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl_aerm_stop(q);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (QR_DONE);
}

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tin_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl_aerm_set_ti_to_tin(q);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (QR_DONE);
}

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tie_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl_aerm_set_ti_to_tie(q);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (QR_DONE);
}

/*
 *  SDT_SUERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_start_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl_suerm_start(q);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (QR_DONE);
}

/*
 *  SDT_SUERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_stop_req(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	if (sl->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl_suerm_stop(q);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (QR_DONE);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  TPI-Provider -> TPI-User (SS7IP) Primitives
 *
 *  ------------------------------------------------------------------------
 */
STATIC int
sl_recv_data(queue_t *q, mblk_t *mp)
{
	sl_daedr_received_bits(q, mp);
	return (QR_ABSORBED);
}

/*
 *  T_CONN_IND
 *  ------------------------------------------------------------------------
 */
STATIC int
t_conn_ind(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	struct T_conn_ind *p = (typeof(p)) mp->b_rptr;
	assure(sl->t.state == TS_IDLE);
	sl->t.state = TS_WRES_CIND;
	if ((sl->state != LMI_UNUSABLE) && (sl->style == LMI_STYLE2)
	    && (sl->state == LMI_ENABLE_PENDING)) {
		if (p->SRC_length)
			bcopy((char *) p + p->SRC_offset, &sl->t.rem, p->SRC_length);
		return t_conn_res(q, p->SEQ_number);
	}
	/*
	   refuse connection 
	 */
	return t_discon_req(q, p->SEQ_number);
}

/*
 *  T_CONN_CON
 *  ------------------------------------------------------------------------
 *  For style 2 transports, we have succeeded in enabling the transport.
 */
STATIC int
t_conn_con(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->state != LMI_UNUSABLE) {
		assure(sl->t.state == TS_WCON_CREQ);
		sl->t.state = TS_DATA_XFER;
		if ((sl->style == LMI_STYLE2) && (sl->state == LMI_ENABLE_PENDING))
			return lmi_enable_con(q);
	}
	return (QR_DONE);
}

/*
 *  T_DISCON_IND
 *  ------------------------------------------------------------------------
 */
STATIC int
t_discon_ind(queue_t *q, mblk_t *mp)
{
	int err;
	sl_t *sl = PRIV(q);
	struct T_discon_ind *p = (typeof(p)) mp->b_rptr;
	if (sl->state != LMI_UNUSABLE) {
		if (sl->t.state != TS_DATA_XFER) {
			sl->t.state = TS_IDLE;
			if (sl->style == LMI_STYLE2) {
				if (sl->state == LMI_DISABLE_PENDING)
					return lmi_disable_con(q);
				if (sl->state == LMI_ENABLE_PENDING)
					return lmi_error_ack(q, LMI_ENABLE_REQ, LMI_SYSERR,
							     p->DISCON_reason);
			}
			return (QR_DONE);
		}
		if (sl->t.state == TS_DATA_XFER) {
			sl->t.state = TS_IDLE;
			if (sl->state == LMI_ENABLED
			    && sl->sl.statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
				if (sl->sl.notify.events & SL_EVT_FAIL_SUERM_EIM)
					if ((err = lmi_event_ind(q, SL_EVT_FAIL_SUERM_EIM, 0)))
						return (err);
				if ((err = sl_lsc_link_failure(q, SL_FAIL_SUERM_EIM)))
					return (err);
				ptrace(("%s: Link Failed: SUERM/EIM\n", SL_TPI_MOD_NAME));
			}
			return m_hangup(q, EPIPE);
		}
	}
	return (QR_DONE);
}

/*
 *  T_DATA_IND
 *  ------------------------------------------------------------------------
 */
STATIC int
t_data_ind_slow(queue_t *q, mblk_t *mp, int more)
{
	/*
	 *  Normally we receive data unfragmented and in a single M_DATA
	 *  block.  This slower routine handles the circumstances where we
	 *  receive fragmented data or data that is chained together in
	 *  multiple M_DATA blocks.
	 */
	sl_t *sl = PRIV(q);
	mblk_t *newp = NULL, *dp = mp->b_cont;
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;
	seldom();
	if (dp->b_cont) {
		/*
		 *  We have a chaned M_DATA blocks: pull them up.
		 */
		if (!pullupmsg(dp, -1)) {
			/*
			   normally only fail because of no buffer 
			 */
			if (!(newp = msgpullup(dp, -1))) {
				/*
				   normally only fail because of no buffer 
				 */
				if (!sl->rbid) {
					sl->rbid =
					    bufcall(xmsgsize(dp), BPRI_MED, &sl_bufsrv, (long) q);
					sl->refcnt++;
				}
				return (-ENOBUFS);
			}
			dp = newp;
		}
	}
	if (more) {
		/*
		 *  We have a partial delivery.  Chain normal message
		 *  together.  We might have a problem with messages split
		 *  over multiple streams?  Treat normal and expedited
		 *  separately.
		 */
		if (sl->rbuf)
			linkb(sl->rbuf, dp);
		else
			sl->rbuf = dp;
	} else {
		if (sl->rbuf) {
			linkb(sl->rbuf, dp);
			dp = xchg(&sl->rbuf, NULL);
		}
		sl_recv_data(q, mp);
	}
	return (newp ? QR_DONE : QR_TRIMMED);
}
STATIC int
t_data_ind(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->state == LMI_ENABLED
	    &&
	    ((sl->sl.statem.lsc_state != SL_STATE_POWER_OFF
	      && sl->sl.statem.rc_state == SL_STATE_IN_SERVICE)
	     || (sl->sl.statem.lsc_state == SL_STATE_POWER_OFF
		 && sl->sdt.statem.daedr_state != SDT_STATE_IDLE))) {
		mblk_t *dp = mp->b_cont;
		struct T_data_ind *p = (typeof(p)) mp->b_rptr;
		if (!p->MORE_flag && !dp->b_cont) {
			sl_recv_data(q, dp);
			return (QR_TRIMMED);	/* absorbed data */
		} else {
			return t_data_ind_slow(q, mp, p->MORE_flag);
		}
	}
	/*
	   ignore data in other states 
	 */
	return (QR_DONE);
}

/*
 *  T_EXDATA_IND
 *  ------------------------------------------------------------------------
 */
STATIC int
t_exdata_ind_slow(queue_t *q, mblk_t *mp, int more)
{
	/*
	 *  Normally we receive data unfragmented and in a single M_DATA
	 *  block.  This slower routine handles the circumstances where we
	 *  receive fragmented data or data that is chained together in
	 *  multiple M_DATA blocks.
	 */
	sl_t *sl = PRIV(q);
	mblk_t *newp = NULL, *dp = mp->b_cont;
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
	seldom();
	if (dp->b_cont) {
		/*
		 *  We have a chaned M_DATA blocks: pull them up.
		 */
		if (!pullupmsg(dp, -1)) {
			/*
			   normally only fail because of no buffer 
			 */
			if (!(newp = msgpullup(dp, -1))) {
				/*
				   normally only fail because of no buffer 
				 */
				if (!sl->rbid) {
					sl->rbid =
					    bufcall(xmsgsize(dp), BPRI_MED, &sl_bufsrv, (long) q);
					sl->refcnt++;
				}
				return (-ENOBUFS);
			}
			dp = newp;
		}
	}
	if (more) {
		/*
		 *  We have a partial delivery.  Chain normal message
		 *  together.  We might have a problem with messages split
		 *  over multiple streams?  Treat normal and expedited
		 *  separately.
		 */
		if (sl->xbuf)
			linkb(sl->xbuf, dp);
		else
			sl->xbuf = dp;
	} else {
		if (sl->xbuf) {
			linkb(sl->xbuf, dp);
			dp = xchg(&sl->xbuf, NULL);
		}
		sl_recv_data(q, dp);
	}
	return (newp ? QR_DONE : QR_TRIMMED);
}
STATIC int
t_exdata_ind(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->state == LMI_ENABLED
	    &&
	    ((sl->sl.statem.lsc_state != SL_STATE_POWER_OFF
	      && sl->sl.statem.rc_state == SL_STATE_IN_SERVICE)
	     || (sl->sl.statem.lsc_state == SL_STATE_POWER_OFF
		 && sl->sdt.statem.daedr_state != SDT_STATE_IDLE))) {
		mblk_t *dp = mp->b_cont;
		struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
		if (!p->MORE_flag && !dp->b_cont) {
			sl_recv_data(q, dp);
			return (QR_TRIMMED);	/* absorbed data */
		} else
			return t_exdata_ind_slow(q, mp, p->MORE_flag);
	}
	/*
	   ignore data in other states 
	 */
	return (QR_DONE);
}

/*
 *  T_INFO_ACK
 *  ------------------------------------------------------------------------
 *  This is a response to our request for information about the transport
 *  provider.  We must first get this response before doing anything with the
 *  transport.  The transport state is set to TS_NOSTATES until we know the
 *  state of the transport.
 */
STATIC int
t_info_ack(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	struct T_info_ack *p = ((typeof(p)) mp->b_rptr);
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl->t.pdu_size = p->TSDU_size;
		if (p->TIDU_size && p->TIDU_size < p->TSDU_size)
			sl->t.pdu_size = p->TIDU_size;
		if ((sl->sdt.config.m =
		     sl->t.pdu_size - 1 - ((sl->option.popt & SS7_POPT_XSN) ? 6 : 3)) < 272)
			cmn_err(CE_WARN, "%s: transport provider TDU_size is too small %ld",
				SL_TPI_MOD_NAME, sl->sdt.config.m);
		if ((sl->t.add_size = p->ADDR_size) > sizeof(struct sockaddr))
			cmn_err(CE_WARN, "%s: transport provider ADDR_size is too large %ld",
				SL_TPI_MOD_NAME, (long) p->ADDR_size);
		sl->t.opt_size = p->OPT_size;
		sl->t.state = p->CURRENT_state;
		sl->t.serv_type = p->SERV_type;
		switch (sl->t.serv_type) {
		case T_COTS:
		case T_COTS_ORD:
			if (sl->t.state == TS_DATA_XFER) {
				/*
				   no attachment required 
				 */
				sl->state = LMI_DISABLED;
				sl->style = LMI_STYLE1;
				return (QR_DONE);
			}
			if (sl->t.state == TS_UNBND) {
				sl->state = LMI_UNATTACHED;
				sl->style = LMI_STYLE2;
				return (QR_DONE);
			}
			break;
		case T_CLTS:
			if (sl->t.state == TS_IDLE) {
				/*
				   no attachment required 
				 */
				sl->state = LMI_DISABLED;
				sl->style = LMI_STYLE1;
				return (QR_DONE);
			}
			if (sl->t.state == TS_UNBND) {
				sl->state = LMI_UNATTACHED;
				sl->style = LMI_STYLE2;
				return (QR_DONE);
			}
			break;
		}
		swerr();
		return m_error(q, EFAULT);
	}
	return m_error(q, EFAULT);
}

/*
 *  T_BIND_ACK
 *  ------------------------------------------------------------------------
 *  We should only get bind acks when we are attaching for a Style 2 transport.  Otherwise, we
 *  should just echo the state change and otherwise ignore the ack.
 */
STATIC int
t_bind_ack(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->state != LMI_UNUSABLE) {
		assure(sl->t.state == TS_WACK_BREQ);
		sl->t.state = TS_IDLE;
		if ((sl->style == LMI_STYLE2) && (sl->state == LMI_ATTACH_PENDING))
			return lmi_ok_ack(q, LMI_ATTACH_REQ);
	}
	return (QR_DONE);
}

/*
 *  T_ERROR_ACK
 *  ------------------------------------------------------------------------
 */
STATIC int
t_error_ack(queue_t *q, mblk_t *mp)
{
	struct T_error_ack *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sl_t *sl = PRIV(q);
		switch (sl->t.state) {
		case TS_WACK_OPTREQ:
			assure(p->ERROR_prim == T_OPTMGMT_REQ);
			swerr();
			return (-EFAULT);
		case TS_WACK_BREQ:
			assure(p->ERROR_prim == T_BIND_REQ);
			if (sl->state != LMI_UNUSABLE) {
				sl->t.state = TS_UNBND;
				if ((sl->style == LMI_STYLE2) && (sl->state == LMI_ATTACH_PENDING)) {
					ptrace(("%s: ERROR: got T_ERROR_ACK from TPI\n",
						SL_TPI_MOD_NAME));
					return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_BADPPA,
							     p->UNIX_error);
				}
			}
			return (QR_DONE);
		case TS_WACK_UREQ:
			assure(p->ERROR_prim == T_UNBIND_REQ);
			if (sl->state != LMI_UNUSABLE) {
				sl->t.state = TS_IDLE;
				if ((sl->style == LMI_STYLE2) && (sl->state == LMI_DETACH_PENDING)) {
					ptrace(("%s: ERROR: got T_ERROR_ACK from TPI\n",
						SL_TPI_MOD_NAME));
					return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_SYSERR,
							     p->UNIX_error);
				}
			}
			return (QR_DONE);
		case TS_WACK_CREQ:
			assure(p->ERROR_prim == T_CONN_REQ);
			if (sl->state != LMI_UNUSABLE) {
				sl->t.state = TS_IDLE;
				if ((sl->style == LMI_STYLE2) && (sl->state == LMI_ENABLE_PENDING)) {
					ptrace(("%s: ERROR: got T_ERROR_ACK from TPI\n",
						SL_TPI_MOD_NAME));
					return lmi_error_ack(q, LMI_ENABLE_REQ, LMI_SYSERR,
							     p->UNIX_error);
				}
			}
			return (QR_DONE);
		case TS_WACK_CRES:
			assure(p->ERROR_prim == T_CONN_RES);
			if (sl->state != LMI_UNUSABLE) {
				sl->t.state = TS_WRES_CIND;
				if ((sl->style == LMI_STYLE2) && (sl->state == LMI_ENABLE_PENDING)) {
					ptrace(("%s: ERROR: got T_ERROR_ACK from TPI\n",
						SL_TPI_MOD_NAME));
					return lmi_error_ack(q, LMI_ENABLE_REQ, LMI_SYSERR,
							     p->UNIX_error);
				}
			}
			/*
			   try refusing the connection 
			 */
			return t_discon_req(q, sl->t.sequence);
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			assure(p->ERROR_prim == T_DISCON_REQ);
			return (QR_DONE);
		case TS_WACK_DREQ9:
			assure(p->ERROR_prim == T_DISCON_REQ);
			return m_error(q, EFAULT);
		case TS_WIND_ORDREL:
			assure(p->ERROR_prim == T_ORDREL_REQ);
			sl->t.state = TS_DATA_XFER;
			if (sl->state == LMI_ENABLE_PENDING || sl->state == LMI_DISABLE_PENDING)
				return t_discon_req(q, 0);
			return (QR_DONE);
		default:
		case TS_UNBND:
		case TS_IDLE:
		case TS_WCON_CREQ:
		case TS_WRES_CIND:
		case TS_DATA_XFER:
		case TS_WREQ_ORDREL:
		case TS_NOSTATES:
			swerr();
			return m_error(q, EFAULT);
		}
	}
	swerr();
	return m_error(q, EFAULT);
}

/*
 *  T_OK_ACK
 *  ------------------------------------------------------------------------
 */
STATIC int
t_ok_ack(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	struct T_ok_ack *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		switch (sl->t.state) {
		case TS_WACK_UREQ:
			assure(p->CORRECT_prim == T_UNBIND_REQ);
			if (sl->state != LMI_UNUSABLE) {
				sl->t.state = TS_UNBND;
				if ((sl->style == LMI_STYLE2) && (sl->state == LMI_DETACH_PENDING))
					return lmi_ok_ack(q, LMI_DETACH_REQ);
			}
			return (QR_DONE);
		case TS_WACK_CREQ:
			assure(p->CORRECT_prim == T_CONN_REQ);
			if (sl->state != LMI_UNUSABLE)
				sl->t.state = TS_WCON_CREQ;
			/*
			   wait for T_CONN_CON 
			 */
			return (QR_DONE);
		case TS_WACK_CRES:
			assure(p->CORRECT_prim == T_CONN_RES);
			if (sl->state != LMI_UNUSABLE) {
				sl->t.state = TS_DATA_XFER;
				if ((sl->style = LMI_STYLE2) && (sl->state == LMI_ENABLE_PENDING))
					return lmi_enable_con(q);
			}
			return (QR_DONE);
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			assure(p->CORRECT_prim == T_DISCON_REQ);
			return (QR_DONE);
		case TS_WACK_DREQ9:
			assure(p->CORRECT_prim == T_DISCON_REQ);
			if (sl->state != LMI_UNUSABLE) {
				sl->t.state = TS_IDLE;
				if ((sl->style == LMI_STYLE2) && (sl->state == LMI_DISABLE_PENDING))
					return lmi_disable_con(q);
			}
			return (QR_DONE);
		default:
		case TS_WACK_OPTREQ:
		case TS_WACK_BREQ:
		case TS_UNBND:
		case TS_IDLE:
		case TS_WCON_CREQ:
		case TS_WRES_CIND:
		case TS_DATA_XFER:
		case TS_WREQ_ORDREL:
		case TS_WIND_ORDREL:
		case TS_NOSTATES:
			swerr();
			return m_error(q, EFAULT);
		}
	}
	swerr();
	return m_error(q, EFAULT);
}

/*
 *  T_UNITDATA_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
t_unitdata_ind(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->state == LMI_ENABLED) {
		if (((sl->sl.statem.lsc_state != SL_STATE_POWER_OFF
		      && sl->sl.statem.rc_state == SL_STATE_IN_SERVICE)
		     || (sl->sl.statem.lsc_state == SL_STATE_POWER_OFF
			 && sl->sdt.statem.daedr_state != SDT_STATE_IDLE))) {
			mblk_t *dp = mp->b_cont;
#if 0
			struct T_unitdata_ind *p = (typeof(p)) mp->b_rptr;
			/*
			   check source of packet 
			 */
			if (p->SRC_length
			    && !memcmp(mp->b_rptr + p->SRC_offset, &sl->t.rem, p->SRC_length)) {
#endif
				sl_recv_data(q, dp);
				return (QR_TRIMMED);
#if 0
			}
#endif
			/*
			   ignore packets not from remote address 
			 */
		}
		ptrace(("%s: %p: lsc_state = %ld, rc_state = %ld, daedr_state = %ld\n", SL_TPI_MOD_NAME,
			sl, sl->sl.statem.lsc_state, sl->sl.statem.rc_state,
			sl->sdt.statem.daedr_state));
		return (QR_DONE);
	}
	printd(("%s: %p: T_UNITDATA received while not LMI_ENABLED\n", SL_TPI_MOD_NAME, sl));
	/*
	   ignore data in other states 
	 */
	return (QR_DONE);
}

/*
 *  T_UDERROR_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
t_uderror_ind(queue_t *q, mblk_t *mp)
{
	(void) mp;
	sl_daedr_su_in_error(q);
	return (QR_DONE);
}

/*
 *  T_OPTMGMT_ACK
 *  ------------------------------------------------------------------------
 */
STATIC int
t_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/*
	   ignore 
	 */
	return (QR_DONE);
}

/*
 *  T_ORDREL_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
t_ordrel_ind(queue_t *q, mblk_t *mp)
{
	int err;
	sl_t *sl = PRIV(q);
	if (sl->state != LMI_UNUSABLE) {
		if (sl->t.state == TS_WIND_ORDREL) {
			sl->t.state = TS_IDLE;
			if ((sl->style == LMI_STYLE2) && (sl->state == LMI_DISABLE_PENDING))
				return lmi_disable_con(q);
			return (QR_DONE);
		}
		if (sl->t.state == TS_DATA_XFER) {
			sl->t.state = TS_WREQ_ORDREL;
			if (sl->state == LMI_ENABLED
			    && sl->sl.statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
				if (sl->sl.notify.events & SL_EVT_FAIL_SUERM_EIM)
					if ((err = lmi_event_ind(q, SL_EVT_FAIL_SUERM_EIM, 0)))
						return (err);
				if ((err = sl_lsc_link_failure(q, SL_FAIL_SUERM_EIM)))
					return (err);
				ptrace(("%s: Link Failed: SUERM/EIM\n", SL_TPI_MOD_NAME));
			}
			if ((err = t_ordrel_req(q)))
				return (err);
			return m_hangup(q, EPIPE);
		}
	}
	return (QR_DONE);
}

/*
 *  T_OPTDATA_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
t_optdata_ind(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	if (sl->state == LMI_ENABLED
	    &&
	    ((sl->sl.statem.lsc_state != SL_STATE_POWER_OFF
	      && sl->sl.statem.rc_state == SL_STATE_IN_SERVICE)
	     || (sl->sl.statem.lsc_state == SL_STATE_POWER_OFF
		 && sl->sdt.statem.daedr_state != SDT_STATE_IDLE))) {
		mblk_t *dp = mp->b_cont;
		struct T_optdata_ind *p = (typeof(p)) mp->b_rptr;
		if (!(p->DATA_flag & T_ODF_MORE) && !dp->b_cont) {
			sl_recv_data(q, dp);
			return (QR_TRIMMED);	/* absorbed data */
		} else {
			if (p->DATA_flag & T_ODF_EX)
				return t_exdata_ind_slow(q, mp, (p->DATA_flag & T_ODF_MORE));
			else
				return t_data_ind_slow(q, mp, (p->DATA_flag & T_ODF_MORE));
		}
	}
	/*
	   ignore data in other states 
	 */
	return (QR_DONE);
}

/*
 *  T_ADDR_ACK
 *  ------------------------------------------------------------------------
 */
STATIC int
t_addr_ack(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/*
	   ignore 
	 */
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *
 *  SL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int
sl_test_config(sl_t * sl, sl_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
sl_commit_config(sl_t * sl, sl_config_t * arg)
{
	return (-EOPNOTSUPP);
}
#endif
STATIC int
sl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.statem;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.statem = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.statsp;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.statsp = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.stats;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			bzero(&sl->sl.stats, sizeof(sl->sl.stats));
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.notify;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.notify.events |= arg->events;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		sl_t *sl = PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.notify.events &= ~(arg->events);
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDT IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_test_config(sl_t * sl, sdt_config_t * arg)
{
	int ret = 0;
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	do {
		if (!arg->t8)
			arg->t8 = sl->sdt.config.t8;
		if (!arg->Tin)
			arg->Tin = sl->sdt.config.Tin;
		if (!arg->Tie)
			arg->Tie = sl->sdt.config.Tie;
		if (!arg->T)
			arg->T = sl->sdt.config.T;
		if (!arg->D)
			arg->D = sl->sdt.config.D;
		if (!arg->Te)
			arg->Te = sl->sdt.config.Te;
		if (!arg->De)
			arg->De = sl->sdt.config.De;
		if (!arg->Ue)
			arg->Ue = sl->sdt.config.Ue;
		if (!arg->N)
			arg->N = sl->sdt.config.N;
		if (!arg->m)
			arg->m = sl->sdt.config.m;
		if (!arg->b)
			arg->b = sl->sdt.config.b;
		else if (arg->b != sl->sdt.config.b) {
			ret = -EINVAL;
			break;
		}
	} while (0);
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}
STATIC int
sdt_commit_config(sl_t * sl, sdt_config_t * arg)
{
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		sdt_test_config(sl, arg);
		sl->sdt.config = *arg;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (0);
}
STATIC int
sdt_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdt.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdt.config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_test_config(sl, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_commit_config(sl, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdt.statem;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccmreset(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	(void) sl;
	(void) mp;
	fixme(("%s: Master reset\n", SL_TPI_MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
sdt_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdt.statsp;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdt.statsp = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdt.stats;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccstats(queue_t *q, mblk_t *mp)
{
	psw_t flags;
	sl_t *sl = PRIV(q);
	(void) mp;
	spin_lock_irqsave(&sl->lock, flags);
	{
		bzero(&sl->sdt.stats, sizeof(sl->sdt.stats));
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (0);
}
STATIC int
sdt_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdt.notify;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdt.notify = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdt.notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccabort(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	int ret = 0;
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&sl->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_test_config(sl_t * sl, sdl_config_t * arg)
{
	(void) sl;
	(void) arg;
	fixme(("%s: FIXME: write this function\n", SL_TPI_MOD_NAME));
	return (0);
}
STATIC void
sdl_commit_config(sl_t * sl, sdl_config_t * arg)
{
	long tdiff;
	sl->sdl.config = *arg;
	/*
	   reshape traffic 
	 */
	if ((tdiff = sl->sdl.timestamp - jiffies) > 0)
		sl->sdl.bytecount += sl->sdl.tickbytes * tdiff;
	else
		sl->sdl.bytecount = 0;
	sl->sdl.timestamp = jiffies;
	sl->sdl.tickbytes = sl->sdl.config.ifrate / HZ / 8;
	if (sl->sdl.tickbytes < 1)
		sl->sdl.tickbytes = 1;
	while (sl->sdl.bytecount >= sl->sdl.tickbytes) {
		sl->sdl.bytecount -= sl->sdl.tickbytes;
		sl->sdl.timestamp++;
	}
}
STATIC int
sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdl.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			if (!(ret = sdl_test_config(sl, arg)))
				sdl_commit_config(sl, arg);
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = sdl_test_config(sl, arg);
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sdl_commit_config(sl, arg);
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdl.statem;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccmreset(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	(void) sl;
	(void) arg;
	fixme(("%s: FIXME: Support master reset\n", SL_TPI_MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdl.statsp;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		fixme(("%s: FIXME: check these settings\n", SL_TPI_MOD_NAME));
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdl.statsp = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdl.stats;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&sl->lock, flags);
	{
		bzero(&sl->sdl.stats, sizeof(sl->sdl.stats));
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (0);
}
STATIC int
sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdl.notify;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdl.notify.events |= arg->events;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_t *sl = PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdl.notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccdisctx(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl->sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (0);
}
STATIC int
sdl_ioccconntx(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&sl->lock, flags);
	{
		sl->sdl.config.ifflags |= SDL_IF_TX_RUNNING;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;
	switch (type) {
	case __SID:
	{
		switch (nr) {
		case _IOC_NR(I_STR):
		case _IOC_NR(I_LINK):
		case _IOC_NR(I_PLINK):
		case _IOC_NR(I_UNLINK):
		case _IOC_NR(I_PUNLINK):
			(void) lp;
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", SL_TPI_MOD_NAME, nr));
			ret = -EINVAL;
			break;
		default:
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", SL_TPI_MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SL_IOC_MAGIC:
	{
		if (count < size || PRIV(q)->state == LMI_UNATTACHED) {
			return -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sl_iocgoptions(q, mp);
			break;
		case _IOC_NR(SL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sl_iocsoptions(q, mp);
			break;
		case _IOC_NR(SL_IOCGCONFIG):	/* sl_config_t */
			ret = sl_iocgconfig(q, mp);
			break;
		case _IOC_NR(SL_IOCSCONFIG):	/* sl_config_t */
			ret = sl_iocsconfig(q, mp);
			break;
		case _IOC_NR(SL_IOCTCONFIG):	/* sl_config_t */
			ret = sl_ioctconfig(q, mp);
			break;
		case _IOC_NR(SL_IOCCCONFIG):	/* sl_config_t */
			ret = sl_ioccconfig(q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATEM):	/* sl_statem_t */
			ret = sl_iocgstatem(q, mp);
			break;
		case _IOC_NR(SL_IOCCMRESET):	/* sl_statem_t */
			ret = sl_ioccmreset(q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATSP):	/* lmi_sta_t */
			ret = sl_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SL_IOCSSTATSP):	/* lmi_sta_t */
			ret = sl_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATS):	/* sl_stats_t */
			ret = sl_iocgstats(q, mp);
			break;
		case _IOC_NR(SL_IOCCSTATS):	/* sl_stats_t */
			ret = sl_ioccstats(q, mp);
			break;
		case _IOC_NR(SL_IOCGNOTIFY):	/* sl_notify_t */
			ret = sl_iocgnotify(q, mp);
			break;
		case _IOC_NR(SL_IOCSNOTIFY):	/* sl_notify_t */
			ret = sl_iocsnotify(q, mp);
			break;
		case _IOC_NR(SL_IOCCNOTIFY):	/* sl_notify_t */
			ret = sl_ioccnotify(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SL ioctl %d\n", SL_TPI_MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDT_IOC_MAGIC:
	{
		if (count < size || PRIV(q)->state == LMI_UNATTACHED) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDT_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdt_iocgoptions(q, mp);
			break;
		case _IOC_NR(SDT_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdt_iocsoptions(q, mp);
			break;
		case _IOC_NR(SDT_IOCGCONFIG):	/* sdt_config_t */
			ret = sdt_iocgconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCSCONFIG):	/* sdt_config_t */
			ret = sdt_iocsconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCTCONFIG):	/* sdt_config_t */
			ret = sdt_ioctconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCCCONFIG):	/* sdt_config_t */
			ret = sdt_ioccconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATEM):	/* sdt_statem_t */
			ret = sdt_iocgstatem(q, mp);
			break;
		case _IOC_NR(SDT_IOCCMRESET):	/* sdt_statem_t */
			ret = sdt_ioccmreset(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATSP):	/* lmi_sta_t */
			ret = sdt_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SDT_IOCSSTATSP):	/* lmi_sta_t */
			ret = sdt_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATS):	/* sdt_stats_t */
			ret = sdt_iocgstats(q, mp);
			break;
		case _IOC_NR(SDT_IOCCSTATS):	/* sdt_stats_t */
			ret = sdt_ioccstats(q, mp);
			break;
		case _IOC_NR(SDT_IOCGNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocgnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCSNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocsnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCCNOTIFY):	/* sdt_notify_t */
			ret = sdt_ioccnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCCABORT):	/* */
			ret = sdt_ioccabort(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SDT ioctl %d\n", SL_TPI_MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || sl->state == LMI_UNATTACHED) {
			ptrace(("%s: ERROR: ioctl count = %d, size = %d, state = %d\n", SL_TPI_MOD_NAME,
				count, size, sl->state));
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdl_iocgoptions(q, mp);
			break;
		case _IOC_NR(SDL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdl_iocsoptions(q, mp);
			break;
		case _IOC_NR(SDL_IOCGCONFIG):	/* sdl_config_t */
			ret = sdl_iocgconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCSCONFIG):	/* sdl_config_t */
			ret = sdl_iocsconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCTCONFIG):	/* sdl_config_t */
			ret = sdl_ioctconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCCCONFIG):	/* sdl_config_t */
			ret = sdl_ioccconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATEM):	/* sdl_statem_t */
			ret = sdl_iocgstatem(q, mp);
			break;
		case _IOC_NR(SDL_IOCCMRESET):	/* sdl_statem_t */
			ret = sdl_ioccmreset(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATSP):	/* sdl_stats_t */
			ret = sdl_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SDL_IOCSSTATSP):	/* sdl_stats_t */
			ret = sdl_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATS):	/* sdl_stats_t */
			ret = sdl_iocgstats(q, mp);
			break;
		case _IOC_NR(SDL_IOCCSTATS):	/* sdl_stats_t */
			ret = sdl_ioccstats(q, mp);
			break;
		case _IOC_NR(SDL_IOCGNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocgnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCSNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocsnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCCNOTIFY):	/* sdl_notify_t */
			ret = sdl_ioccnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCCDISCTX):	/* */
			ret = sdl_ioccdisctx(q, mp);
			break;
		case _IOC_NR(SDL_IOCCCONNTX):	/* */
			ret = sdl_ioccconntx(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SDL ioctl %d\n", SL_TPI_MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		return (QR_PASSALONG);
	}
	if (ret >= 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sl_t *sl = PRIV(q);
	ulong oldstate = sl->state;
	/*
	   Fast Path 
	 */
	if ((prim = *(ulong *) mp->b_rptr) == SL_PDU_REQ) {
		printd(("%s: %p: -> SL_PDU_REQ\n", SL_TPI_MOD_NAME, sl));
		if ((rtn = sl_pdu_req(q, mp)) < 0)
			sl->state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = lmi_info_req(q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = lmi_attach_req(q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = lmi_detach_req(q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = lmi_enable_req(q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = lmi_disable_req(q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = lmi_optmgmt_req(q, mp);
		break;
	case SL_PDU_REQ:
		printd(("%s: %p: -> SL_PDU_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_pdu_req(q, mp);
		break;
	case SL_EMERGENCY_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_emergency_req(q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_CEASES_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_emergency_ceases_req(q, mp);
		break;
	case SL_START_REQ:
		printd(("%s: %p: -> SL_START_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_start_req(q, mp);
		break;
	case SL_STOP_REQ:
		printd(("%s: %p: -> SL_STOP_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_stop_req(q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		printd(("%s: %p: -> SL_RETRIEVE_BSNT_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_retrieve_bsnt_req(q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		printd(("%s: %p: -> SL_RETRIEVAL_REQUEST_AND_FSNC\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_retrieval_request_and_fsnc_req(q, mp);
		break;
	case SL_RESUME_REQ:
		printd(("%s: %p: -> SL_RESUME_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_resume_req(q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		printd(("%s: %p: -> SL_CLEAR_BUFFERS_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_clear_buffers_req(q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		printd(("%s: %p: -> SL_CLEAR_RTB_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_clear_rtb_req(q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		printd(("%s: %p: -> SL_LOCAL_PROCESSOR_OUTAGE_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_local_processor_outage_req(q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		printd(("%s: %p: -> SL_CONGESTION_DISCARD_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_congestion_discard_req(q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		printd(("%s: %p: -> SL_CONGESTION_ACCEPT_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_congestion_accept_req(q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		printd(("%s: %p: -> SL_NO_CONGESTION_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_no_congestion_req(q, mp);
		break;
	case SL_POWER_ON_REQ:
		printd(("%s: %p: -> SL_POWER_ON_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sl_power_on_req(q, mp);
		break;
	case SDT_DAEDT_TRANSMISSION_REQ:
		// printd(("%s: %p: -> SDT_DAEDT_TRANSMISSION_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_daedt_transmission_req(q, mp);
		break;
	case SDT_DAEDT_START_REQ:
		printd(("%s: %p: -> SDT_DAEDT_START_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_daedt_start_req(q, mp);
		break;
	case SDT_DAEDR_START_REQ:
		printd(("%s: %p: -> SDT_DAEDR_START_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_daedr_start_req(q, mp);
		break;
	case SDT_AERM_START_REQ:
		printd(("%s: %p: -> SDT_AERM_START_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_aerm_start_req(q, mp);
		break;
	case SDT_AERM_STOP_REQ:
		printd(("%s: %p: -> SDT_AERM_STOP_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_aerm_stop_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIN_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_aerm_set_ti_to_tin_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIE_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_aerm_set_ti_to_tie_req(q, mp);
		break;
	case SDT_SUERM_START_REQ:
		printd(("%s: %p: -> SDT_SUERM_START_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_suerm_start_req(q, mp);
		break;
	case SDT_SUERM_STOP_REQ:
		printd(("%s: %p: -> SDT_SUERM_STOP_REQ\n", SL_TPI_MOD_NAME, sl));
		rtn = sdt_suerm_stop_req(q, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		sl->state = oldstate;
	return (rtn);
}
STATIC int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sl_t *sl = PRIV(q);
	ulong oldstate = sl->t.state;
	/*
	   Fast Path 
	 */
	if ((prim = *((ulong *) mp->b_rptr)) == T_UNITDATA_IND) {
		// printd(("%s: %p: -> T_UNITDATA_IND [%d]\n", SL_TPI_MOD_NAME, sl,
		// msgdsize(mp->b_cont)));
		if ((rtn = t_unitdata_ind(q, mp)) < 0)
			sl->t.state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case T_CONN_IND:
		printd(("%s: %p: -> T_CONN_IND\n", SL_TPI_MOD_NAME, sl));
		rtn = t_conn_ind(q, mp);
		break;
	case T_CONN_CON:
		printd(("%s: %p: -> T_CONN_CON\n", SL_TPI_MOD_NAME, sl));
		rtn = t_conn_con(q, mp);
		break;
	case T_DISCON_IND:
		printd(("%s: %p: -> T_DISCON_IND\n", SL_TPI_MOD_NAME, sl));
		rtn = t_discon_ind(q, mp);
		break;
	case T_DATA_IND:
		printd(("%s: %p: -> T_DATA_IND\n", SL_TPI_MOD_NAME, sl));
		rtn = t_data_ind(q, mp);
		break;
	case T_EXDATA_IND:
		printd(("%s: %p: -> T_EXDATA_IND\n", SL_TPI_MOD_NAME, sl));
		rtn = t_exdata_ind(q, mp);
		break;
	case T_INFO_ACK:
		printd(("%s: %p: -> T_INFO_ACK\n", SL_TPI_MOD_NAME, sl));
		rtn = t_info_ack(q, mp);
		break;
	case T_BIND_ACK:
		printd(("%s: %p: -> T_BIND_ACK\n", SL_TPI_MOD_NAME, sl));
		rtn = t_bind_ack(q, mp);
		break;
	case T_ERROR_ACK:
		printd(("%s: %p: -> T_ERROR_ACK\n", SL_TPI_MOD_NAME, sl));
		rtn = t_error_ack(q, mp);
		break;
	case T_OK_ACK:
		printd(("%s: %p: -> T_OK_ACK\n", SL_TPI_MOD_NAME, sl));
		rtn = t_ok_ack(q, mp);
		break;
	case T_UNITDATA_IND:
		// printd(("%s: %p: -> T_UNITDATA_IND [%d]\n", SL_TPI_MOD_NAME, sl,
		// msgdsize(mp->b_cont)));
		rtn = t_unitdata_ind(q, mp);
		break;
	case T_UDERROR_IND:
		printd(("%s: %p: -> T_UDERROR_IND\n", SL_TPI_MOD_NAME, sl));
		rtn = t_uderror_ind(q, mp);
		break;
	case T_OPTMGMT_ACK:
		printd(("%s: %p: -> T_OPTMGMT_ACK\n", SL_TPI_MOD_NAME, sl));
		rtn = t_optmgmt_ack(q, mp);
		break;
	case T_ORDREL_IND:
		printd(("%s: %p: -> T_ORDREL_IND\n", SL_TPI_MOD_NAME, sl));
		rtn = t_ordrel_ind(q, mp);
		break;
	case T_OPTDATA_IND:
		printd(("%s: %p: -> T_OPTDATA_IND\n", SL_TPI_MOD_NAME, sl));
		rtn = t_optdata_ind(q, mp);
		break;
	case T_ADDR_ACK:
		printd(("%s: %p: -> T_ADDR_ACK\n", SL_TPI_MOD_NAME, sl));
		rtn = t_addr_ack(q, mp);
		break;
	default:
		swerr();
		rtn = (-EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		sl->t.state = oldstate;
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_w_data(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	(void) sl;
	printd(("%s: %p: -> M_DATA (above)\n", SL_TPI_MOD_NAME, sl));
	return sl_send_data(q, mp);
}
STATIC int
sl_r_data(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	(void) sl;
	printd(("%s: %p: -> M_DATA (below)\n", SL_TPI_MOD_NAME, sl));
	return sl_recv_data(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_RSE, M_PCRSE Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_r_pcrse(queue_t *q, mblk_t *mp)
{
	sl_t *sl = PRIV(q);
	int rtn;
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		switch (*(ulong *) mp->b_rptr) {
		case t1:
			printd(("%s: %p: t1 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t1_expiry(q);
			break;
		case t2:
			printd(("%s: %p: t2 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t2_expiry(q);
			break;
		case t3:
			printd(("%s: %p: t3 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t3_expiry(q);
			break;
		case t4:
			printd(("%s: %p: t4 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t4_expiry(q);
			break;
		case t5:
			printd(("%s: %p: t5 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t5_expiry(q);
			break;
		case t6:
			printd(("%s: %p: t6 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t6_expiry(q);
			break;
		case t7:
			printd(("%s: %p: t7 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t7_expiry(q);
			break;
		case t8:
			printd(("%s: %p: t8 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t8_expiry(q);
			break;
		case t9:
			// printd(("%s: %p: t9 expiry at %lu\n", SL_TPI_MOD_NAME, sl, jiffies));
			rtn = sl_t9_expiry(q);
			break;
		default:
			swerr();
			rtn = -EFAULT;
			break;
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sl_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag)
{
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	return (QR_PASSALONG);
}
STATIC int
sl_w_flush(queue_t *q, mblk_t *mp)
{
	return sl_m_flush(q, mp, FLUSHW);
}
STATIC int
sl_r_flush(queue_t *q, mblk_t *mp)
{
	return sl_m_flush(q, mp, FLUSHR);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
sl_r_prim(queue_t *q, mblk_t *mp)
{
	/*
	   Fast Path 
	 */
	if (mp->b_datap->db_type == M_DATA)
		return sl_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sl_r_pcrse(q, mp);
	case M_FLUSH:
		return sl_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
sl_w_prim(queue_t *q, mblk_t *mp)
{
	/*
	   Fast Path 
	 */
	if (mp->b_datap->db_type == M_DATA)
		return sl_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_w_proto(q, mp);
	case M_FLUSH:
		return sl_w_flush(q, mp);
	case M_IOCTL:
		return sl_w_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  PUTQ Put Routine
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sl_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *), int (*wakeup) (queue_t *))
{
	int rtn = 0;
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type < QPCTL || q->q_count) {
		putq(q, mp);
		return (0);
	}
	if (sl_trylockq(q)) {
		do {
			/*
			   Fast Path 
			 */
			if ((rtn = proc(q, mp)) == QR_DONE) {
				ctrace(freemsg(mp));
				break;
			}
			switch (rtn) {
			case QR_DONE:
				ctrace(freemsg(mp));
			case QR_ABSORBED:
				break;
			case QR_STRIP:
				if (mp->b_cont)
					putq(q, mp->b_cont);
			case QR_TRIMMED:
				ctrace(freeb(mp));
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
				ctrace(freemsg(mp));
				break;
			case QR_DISABLE:
				putq(q, mp);
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
				putq(q, mp);
				break;
			}
		} while (0);
		if (wakeup)
			wakeup(q);
		sl_unlockq(q);
	} else {
		putq(q, mp);
	}
	return (rtn);
}

/*
 *  SRVQ Service Routine
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sl_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *), int (*wakeup) (queue_t *))
{
	int rtn = 0;
	ensure(q, return (-EFAULT));
	if (sl_trylockq(q)) {
		mblk_t *mp;
		while ((mp = getq(q))) {
			/*
			   Fast Path 
			 */
			if ((rtn = proc(q, mp)) == QR_DONE) {
				ctrace(freemsg(mp));
				continue;
			}
			switch (rtn) {
			case QR_DONE:
				ctrace(freemsg(mp));
			case QR_ABSORBED:
				continue;
			case QR_STRIP:
				if (mp->b_cont)
					putbq(q, mp->b_cont);
			case QR_TRIMMED:
				ctrace(freeb(mp));
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
				ptrace(("%s: ERROR: (q dropping) %d\n", SL_TPI_MOD_NAME, rtn));
				ctrace(freemsg(mp));
				continue;
			case QR_DISABLE:
				ptrace(("%s: ERROR: (q disabling) %d\n", SL_TPI_MOD_NAME, rtn));
				noenable(q);
				putbq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must have scheduled bufcall */
			case -EBUSY:	/* proc must have failed canput */
			case -ENOMEM:	/* proc must have scheduled bufcall */
			case -EAGAIN:	/* proc must re-enable on some event */
				if (mp->b_datap->db_type < QPCTL) {
					ptrace(("%s: ERROR: (q stalled) %d\n", SL_TPI_MOD_NAME, rtn));
					putbq(q, mp);
					break;
				}
				/*
				 *  Be careful not to put a priority
				 *  message back on the queue.
				 */
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					mp->b_band = 255;
					putq(q, mp);
					break;
				}
				ptrace(("%s: ERROR: (q dropping) %d\n", SL_TPI_MOD_NAME, rtn));
				ctrace(freemsg(mp));
				continue;
			}
			break;
		}
		if (wakeup)
			wakeup(q);
		sl_unlockq(q);
	}
	return (rtn);
}

STATIC int
sl_rput(queue_t *q, mblk_t *mp)
{
	return sl_putq(q, mp, &sl_r_prim, &sl_rx_wakeup);
}
STATIC int
sl_rsrv(queue_t *q)
{
	return sl_srvq(q, &sl_r_prim, &sl_rx_wakeup);
}
STATIC int
sl_wput(queue_t *q, mblk_t *mp)
{
	return sl_putq(q, mp, &sl_w_prim, &sl_tx_wakeup);
}
STATIC int
sl_wsrv(queue_t *q)
{
	return sl_srvq(q, &sl_w_prim, &sl_tx_wakeup);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *sl_priv_cachep = NULL;
STATIC int
sl_init_caches(void)
{
	if (!sl_priv_cachep
	    && !(sl_priv_cachep =
		 kmem_cache_create("sl_priv_cachep", sizeof(sl_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate sl_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized module private structure cace\n", SL_TPI_MOD_NAME));
	return (0);
}
STATIC void
sl_term_caches(void)
{
	if (sl_priv_cachep) {
		if (kmem_cache_destroy(sl_priv_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sl_priv_cachep", __FUNCTION__);
		else
			printd(("%s: destroyed sl_priv_cachep\n", SL_TPI_MOD_NAME));
	}
	return;
}
STATIC sl_t *
sl_alloc_priv(queue_t *q, sl_t ** slp, ushort cmajor, ushort cminor)
{
	sl_t *sl;
	if ((sl = kmem_cache_alloc(sl_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated module private structure\n", SL_TPI_MOD_NAME));
		bzero(sl, sizeof(*sl));
		sl->cmajor = cmajor;
		sl->cminor = cminor;
		sl->rq = RD(q);
		sl->rq->q_ptr = sl;
		sl->refcnt++;
		sl->wq = WR(q);
		sl->wq->q_ptr = sl;
		sl->refcnt++;
		spin_lock_init(&sl->qlock); /* "sl-queue-lock" */
		sl->rwait = NULL;
		sl->wwait = NULL;
		sl->version = 1;
		sl->state = LMI_UNUSABLE;
		sl->style = LMI_STYLE1;
		spin_lock_init(&sl->lock); /* "sl-priv-lock" */
		if ((sl->next = *slp))
			sl->next->prev = &sl->next;
		sl->prev = slp;
		*slp = sl;
		sl->refcnt++;
		printd(("%s: linked module private structure\n", SL_TPI_MOD_NAME));
		/*
		   TPI configuration defaults 
		 */
		sl->t.state = TS_NOSTATES;
		sl->t.serv_type = T_CLTS;
		sl->t.sequence = 0;
		sl->t.pdu_size = 272 + 1 + 3;
		sl->t.opt_size = -1UL;
		sl->t.add_size = sizeof(struct sockaddr);
		/*
		   LMI configuration defaults 
		 */
		sl->option.pvar = SS7_PVAR_ITUT_88;
		sl->option.popt = 0;
		/*
		   SDL configuration defaults 
		 */
		sl->sdl.config.ifflags = 0;
		sl->sdl.config.iftype = SDL_TYPE_PACKET;
		sl->sdl.config.ifrate = 1544000;
		sl->sdl.config.ifgtype = SDL_GTYPE_UDP;
		sl->sdl.config.ifgrate = 10000000;
		sl->sdl.config.ifmode = SDL_MODE_PEER;
		sl->sdl.config.ifgmode = SDL_GMODE_NONE;
		sl->sdl.config.ifgcrc = SDL_GCRC_NONE;
		sl->sdl.config.ifclock = SDL_CLOCK_SHAPER;
		sl->sdl.config.ifcoding = SDL_CODING_NONE;
		sl->sdl.config.ifframing = SDL_FRAMING_NONE;
		sl->sdl.timestamp = jiffies;
		sl->sdl.tickbytes = sl->sdl.config.ifrate / HZ / 8;
		sl->sdl.bytecount = 0;
		/*
		   rest zero 
		 */
		/*
		   SDT configuration defaults 
		 */
		bufq_init(&sl->sdt.tb);
		sl_bufpool_alloc(2);
		sl->sdt.config.Tin = 4;
		sl->sdt.config.Tie = 1;
		sl->sdt.config.T = 64;
		sl->sdt.config.D = 256;
		sl->sdt.config.t8 = 100 * HZ / 1000;
		sl->sdt.config.Te = 577169;
		sl->sdt.config.De = 9308000;
		sl->sdt.config.Ue = 144292000;
		sl->sdt.config.N = 16;
		sl->sdt.config.m = 272;
		sl->sdt.config.b = 8;
		sl->sdt.config.f = SDT_FLAGS_ONE;
		/*
		   SL configuration defaults 
		 */
		bufq_init(&sl->sl.rb);
		bufq_init(&sl->sl.tb);
		bufq_init(&sl->sl.rtb);
		sl->sl.config.t1 = 45 * HZ;
		sl->sl.config.t2 = 5 * HZ;
		sl->sl.config.t2l = 20 * HZ;
		sl->sl.config.t2h = 100 * HZ;
		sl->sl.config.t3 = 1 * HZ;
		sl->sl.config.t4n = 8 * HZ;
		sl->sl.config.t4e = 500 * HZ / 1000;
		sl->sl.config.t5 = 100 * HZ / 1000;
		sl->sl.config.t6 = 4 * HZ;
		sl->sl.config.t7 = 1 * HZ;
		sl->sl.config.rb_abate = 3;
		sl->sl.config.rb_accept = 6;
		sl->sl.config.rb_discard = 9;
		sl->sl.config.tb_abate_1 = 128 * 272;
		sl->sl.config.tb_onset_1 = 256 * 272;
		sl->sl.config.tb_discd_1 = 384 * 272;
		sl->sl.config.tb_abate_2 = 512 * 272;
		sl->sl.config.tb_onset_2 = 640 * 272;
		sl->sl.config.tb_discd_2 = 768 * 272;
		sl->sl.config.tb_abate_3 = 896 * 272;
		sl->sl.config.tb_onset_3 = 1024 * 272;
		sl->sl.config.tb_discd_3 = 1152 * 272;
		sl->sl.config.N1 = 127;
		sl->sl.config.N2 = 8192;
		sl->sl.config.M = 5;
		printd(("%s: setting module private structure defaults\n", SL_TPI_MOD_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", SL_TPI_MOD_NAME));
	return (sl);
}
STATIC void
sl_free_priv(queue_t *q)
{
	sl_t *sl = PRIV(q);
	psw_t flags;
	ensure(sl, return);
	spin_lock_irqsave(&sl->lock, flags);
	{
		if (sl->sdt.rx_cmp)
			sl_fast_freemsg(xchg(&sl->sdt.rx_cmp, NULL));
		if (sl->sdt.tx_cmp)
			sl_fast_freemsg(xchg(&sl->sdt.tx_cmp, NULL));
		if (sl->rbuf)
			ctrace(freemsg(xchg(&sl->rbuf, NULL)));
		if (sl->xbuf)
			ctrace(freemsg(xchg(&sl->xbuf, NULL)));
		sl_unbufcall(q);
		sl_timer_stop(q, t1);
		sl_timer_stop(q, t2);
		sl_timer_stop(q, t3);
		sl_timer_stop(q, t4);
		sl_timer_stop(q, t5);
		sl_timer_stop(q, t6);
		sl_timer_stop(q, t7);
		sl_timer_stop(q, t8);
		sl_timer_stop(q, t9);
		bufq_purge(&sl->sdt.tb);
		sl_bufpool_dealloc(2);
		bufq_purge(&sl->sl.rb);
		bufq_purge(&sl->sl.tb);
		bufq_purge(&sl->sl.rtb);
		if ((*sl->prev = sl->next))
			sl->next->prev = sl->prev;
		sl->next = NULL;
		sl->prev = NULL;
		sl->refcnt--;
		sl->rq->q_ptr = NULL;
		sl->refcnt--;
		sl->wq->q_ptr = NULL;
		sl->refcnt--;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	printd(("%s: unlinked module private structure\n", SL_TPI_MOD_NAME));
	if (sl->refcnt) {
		assure(sl->refcnt);
		printd(("%s: WARNING: sl->refcnt = %d\n", SL_TPI_MOD_NAME, sl->refcnt));
	}
	kmem_cache_free(sl_priv_cachep, sl);
	printd(("%s: freed module private structure\n", SL_TPI_MOD_NAME));
	return;
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *  Open is called on the first open of a character special device stream
 *  head; close is called on the last close of the same device.
 */
sl_t *sl_list = NULL;
STATIC int
sl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;		/* for now */
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		if (!(sl_alloc_priv(q, &sl_list, getmajor(*devp), getminor(*devp)))) {
			MOD_DEC_USE_COUNT;
			return ENOMEM;
		}
		/*
		   generate immediate information request 
		 */
		t_info_req(q);
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return EIO;
}
STATIC int
sl_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sl_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization (For unregistered driver.)
 *
 *  =========================================================================
 */
STATIC int sl_initialized = 0;
STATIC void
sl_init(void)
{
	unless(sl_initialized, return);
	cmn_err(CE_NOTE, SS7IP_BANNER);	/* console splash */
	if ((sl_initialized = sl_init_caches()))
		return;
	sl_bufpool_init();
	if (!(sl_initialized = lis_register_strmod(&sl_info, SL_TPI_MOD_NAME))) {
		sl_bufpool_term();
		sl_term_caches();
		cmn_err(CE_WARN, "%s: couldn't register module", SL_TPI_MOD_NAME);
		return;
	}
	sl_initialized = 1;
	return;
}
STATIC void
sl_terminate(void)
{
	int err;
	ensure(sl_initialized, return);
	if ((err = lis_unregister_strmod(&sl_info)))
		cmn_err(CE_PANIC, "%s: couldn't unregister module", SL_TPI_MOD_NAME);
	sl_initialized = 0;
	sl_bufpool_term();
	sl_term_caches();
	return;
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int
init_module(void)
{
	sl_init();
	if (sl_initialized < 0)
		return sl_initialized;
	return (0);
}

void
cleanup_module(void)
{
	sl_terminate();
	return;
}
