/*****************************************************************************

 @(#) $RCSfile: mx_mux.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/12/18 10:51:15 $

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

 Last Modified $Date: 2006/12/18 10:51:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mx_mux.c,v $
 Revision 0.9.2.2  2006/12/18 10:51:15  brian
 - subpackaging changes for release

 Revision 0.9.2.1  2006/03/20 12:23:31  brian
 - first cut of zaptel driver

 *****************************************************************************/

#ident "@(#) $RCSfile: mx_mux.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/12/18 10:51:15 $"

static char const ident[] = "$RCSfile: mx_mux.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/12/18 10:51:15 $";

/*
 *  This is an MX multiplexing driver.  Its purpose is to allow a single device /dev/streams/matrix
 *  to represent all multiplex device streams in the system.  A configuration script opens the
 *  appropriate device drivers and links (I_PLINKs) them under this multiplexer to form a single
 *  view of the device driver.  It is then possible to open a single MX upper stream on this
 *  multiplexer and link it, for example, under the MG multiplexer.
 *
 *
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>

#define MX_MUX_DESCRIP		"MX MULTIPLEX (MX-MUX) STREAMS MULTIPLEXING DRIVER."
#define MX_MUX_REVISION		"LfS $RCSfile: mx_mux.c,v $ $Name:  $ ($Revision: 0.9.2.2 $) $Date: 2006/12/18 10:51:15 $"
#define MX_MUX_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define MX_MUX_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MX_MUX_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MX_MUX_LICENSE		"GPL"
#define MX_MUX_BANNER		MX_MUX_DESCRIP		"\n" \
				MX_MUX_REVISION		"\n" \
				MX_MUX_COPYRIGHT	"\n" \
				MX_MUX_DEVICE		"\n" \
				MX_MUX_CONTACT		"\n"
#define MX_MUX_SPLASH		MX_MUX_DESCRIP		" - " \
				MX_MUX_REVISION

#ifdef LINUX
MODULE_AUTHOR(MX_MUX_CONTACT);
MODULE_DESCRIPTION(MX_MUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MX_MUX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MX_MUX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mx_mux");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_MX_MUX_NAME
#ifdef LIS
#define CONFIG_STREAMS_MX_MUX_NAME MX_MUX__DRV_NAME
#endif				/* LIS */
#ifdef LFS
#error CONFIG_STREAMS_MX_MUX_NAME must be defined.
#endif				/* LFS */
#endif				/* CONFIG_STREAMS_MX_MUX_NAME */
#ifndef CONFIG_STREAMS_MX_MUX_MAJOR
#ifdef LIS
#define CONFIG_STREAMS_MX_MUX_MAJOR MX_MUX__CMAJOR_0
#endif				/* LIS */
#ifdef LFS
#error CONFIG_STREAMS_MX_MUX_MAJOR must be defined.
#endif				/* LFS */
#endif				/* CONFIG_STREAMS_MX_MUX_MAJOR */
#ifndef CONFIG_STREAMS_MX_MUX_MODID
#ifdef LIS
#define CONFIG_STREAMS_MX_MUX_MODID MX_MUX__ID
#endif				/* LIS */
#ifdef LFS
#error CONFIG_STREAMS_MX_MUX_MODID must be defined.
#endif				/* LFS */
#endif				/* CONFIG_STREAMS_MX_MUX_MODID */

modID_t modid = CONFIG_STREAMS_MX_MUX_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module id number for STREAMS MX-MUX driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_MX_MUX_MODID));
MODULE_ALIAS("streams-driver-mx_mux");
#endif				/* MODULE_ALIAS */

major_t major = CONFIG_STREAMS_MX_MUX_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, uint, 0);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for STREAMS MX-MUX driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_MX_MUX_MAJOR));
MODULE_ALIAS("/dev/mx");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_MX_MUX_MAJOR));
MODULE_ALIAS("/dev/streams/mx");
MODULE_ALIAS("/dev/streams/mx/*");
MODULE_ALIAS("/dev/streams/clone/mx");
#endif				/* LFS */
#endif				/* MODULE_ALIAS */

#ifdef LIS
#define STRMINPSZ	0
#define STRMAXPSZ	4096
#define STRHIGH		5120
#define STRLOW		1024
#endif				/* LIS */

STATIC struct module_info mx_minfo = {
	.mi_idnum = CONFIG_STREAMS_MX_MUX_MODID,
	.mi_idname = CONFIG_STREAMS_MX_MUX_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

#ifdef LIS
#define trace() while (0) { }
#define ptrace(__x) while (0) { }
#define printd(__x) while (0) { }
#define pswerr(__x) while (0) { }
#define ctrace(__x) __x

#define QSVCBUSY QRUNNING

union ioctypes {
	struct iocblk iocblk;
	struct copyreq copyreq;
	struct copyresp copyresp;
};
#endif				/* LIS */

/* private structures */
struct mx {
	struct mx *next;		/* list linkage */
	struct mx **prev;		/* list linkage */
	struct mx *other;		/* upper or lower */
	queue_t *rq;			/* this rd queue */
	queue_t *wq;			/* this wr queue */
	dev_t dev;			/* device number */
};

/* blank structure for use by I_UNLINK/I_PUNLINK */
STATIC struct mx no_mx = {
	.next = NULL,
	.prev = &no_mx.next,
	.other = NULL,
	.rq = NULL,
	.wq = NULL,
	.dev = 0,
};

STATIC rwlock_t mx_lock = RW_LOCK_UNLOCKED;
STATIC struct mx *mx_opens = NULL;
STATIC struct mx *mx_links = NULL;

#define MUX_UP		1
#define MUX_DOWN	2

#ifdef LIS
#define streamscall _RP
#endif				/* LIS */

/**
 *  mx_uwput: - upper stream write put procedure
 *  @q: write queue
 *  @mp: message to put
 */
STATIC streamscall int
mx_uwput(queue_t *q, mblk_t *mp)
{
	struct mx *mx = q->q_ptr, *bot;
	int err;

	trace();
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	{
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;

		trace();
		switch (ioc->iocblk.ioc_cmd) {
		case I_LINK:
		case I_PLINK:
		{
			struct linkblk *l;

			trace();
			if (!mp->b_cont) {
				ptrace(("Error path taken!\n"));
				goto einval;
			}
			if (!(bot = kmem_alloc(sizeof(*bot), KM_NOSLEEP))) {
				ptrace(("Error path taken!\n"));
				goto enomem;
			}
			l = (typeof(l)) mp->b_cont->b_rptr;

			write_lock_bh(&mx_lock);
			bot->next = mx_links;
			bot->prev = &mx_links;
			mx_links = bot;
			bot->dev = l->l_index;
			bot->rq = RD(l->l_qtop);
			bot->wq = l->l_qtop;
			bot->other = NULL;
			noenable(bot->rq);
			l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = bot;
			write_unlock_bh(&mx_lock);

			goto ack;
		}
		}
	case I_UNLINK:
	case I_PUNLINK:
	{
		struct linkblk *l;

		trace();
		if (!mp->bcont) {
			ptrace(("Error path taken!\n"));
			goto einval;
		}
		l = (typeof(l)) mp->b_cont->b_rptr;

		write_lock_bh(&mx_lock);
		for (bot = mx_links; bot; bot = bot->next) {
#ifdef __LP64__
			if (iocp->iocblk.ioc_flag == IOC_ILP32) {
				if ((int32_t) bot->dev == (int32_t) l->l_index)
					break;
			} else
#endif				/* __LP64__ */
			{
				if (bot->dev == l->l_index)
					break;
			}
		}
		if (bot) {
			struct mx *top;

			/* Note that the multiplex driver put and service procedures must be
			   prepared to be invoked even after the M_IOCACK for the I_UNLINK or
			   I_PUNLINK ioctl has been returned.  This is because the setq(9) back to
			   the Stream head procedures is not performed until after the
			   acknowledgement has been received.  We set q->q_ptr to a null multiplex
			   structure to keep the lower Stream functioning until the setq(9) is
			   performed. */
			l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = &no_mx;
			if ((*(bot->prev) = bot->next)) {
				bot->next = NULL;
				bot->prev = &bot->next;
			}
			bot->other = NULL;
			kmem_free(bot, sizeof(*bot));
			/* FIXME: with the write lock held, we need to unmap this lower multiplex
			   stream from whatever maps it belongs to */
			/* hang up all upper streams that feed this lower stream */
			for (top = mx_opens; top; top = top->next) {
				if (top->other == bot) {
					putnextctl(top->rq, M_HANGUP);
					top->other = NULL;
				}
			}
		}
		write_unlock_bh(&mx_lock);
		if (!bot)
			goto einval;
		goto ack;
	}
	case MX_MAP:
	{
	}
	case MX_UNMAP:
	{
	}
	default:
		ptrace(("Error path taken!\n"));
		if (mx->other)
			goto passmsg;
	      einval:
		err = -EINVAL;
		goto nak;
	      nak:
		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_error = -err;
		ioc->iocblk.ioc_rval = -1;
		qreply(q, mp);
		return (0);
	      ack:
		mp->b_datap->db_type = M_IOCACK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_error = 0;
		ioc->iocblk.ioc_rval = 0;
		qreply(q, mp);
		return (0);
	}
	case M_FLUSH:
	{
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
		}
		read_lock_bh(&mx_lock);
		if (mx->other) {
			queue_t *wq;

			if ((wq = mx->other->wq)) {
				putnext(wq, mp);
				read_unlock_bh(&mx_lock);
				return (0);
			}
		}
		read_unlock_bh(&mx_lock);

		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(RD(q), mp->b_rptr[1], FLUSHALL);
			else
				flushq(RD(q), FLUSHALL);
			mp->b_rptr[0] &= ~FLUSHW;
			qreply(q, mp);
			return (0);
		}
		freemsg(mp);
		break;
	}
	case M_DATA:
	{
		/* If we are in zaptel mode or have been otherwise assigned a default channel,
		   determine the channel number and create a proper MX primitive to the appropriate
		   lower Stream (the one that the channel maps to).  If there is no channel in the
		   channel map or no default has been set (or has been removed) generate an
		   M_ERROR(EPROTO) upstream or an M_HANGUP as appropriate. */
	}
	case M_PROTO:
	case M_PCPROTO:
	{
		/* We always accept MX primitives unless we are in zaptel mode. */
		ulong prim;

		if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
			/* need to error ack */
		}
		switch ((prim = *(ulong *) mp->b_rptr)) {
		case MX_INFO_REQ:
		{
			struct MX_info_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_OPTMGMT_REQ:
		{
			struct MX_optmgmt_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_ATTACH_REQ:
		{
			struct MX_attach_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_ENABLE_REQ:
		{
			struct MX_enable_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_CONNECT_REQ:
		{
			struct MX_connect_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_DATA_REQ:
		{
			struct MX_data_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_DISCONNECT_REQ:
		{
			struct MX_disconnect_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_DISABLE_REQ:
		{
			struct MX_info_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_DETACH_REQ:
		{
			struct MX_info_req *p = (typeof(p)) mp->b_rptr;
			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		default:
		}
	}
	default:
	      passmsg:
	{
		queue_t *wq = NULL;

		read_lock_bh(&mx_lock);
		if (mx->other)
			wq = mx->other->wq;
		read_unlock_bh(&mx_lock);

		/* if not linked behave like echo driver */
		if (!wq)
			wq = RD(q);

		/* Check the QSVCBUSY flag in MP drivers to avoid missequencing of messages when
		   service procedure is running concurrent with put procedure. */
		if (mp->b_datap->db_type < QPCTL
		    && (q->q_first || (q->q_flag & QSVCBUSY) || !bcanputnext(wq, mp->b_band)))
			putq(q, mp);
		else
			putnext(wq, mp);
		break;
	}
	}
	return (0);
}

/**
 *  mx_lrput: - lower stream read put procedure
 *  @q: write queue
 *  @mp: message to put
 */
STATIC streamscall int
mx_lrput(queue_t *q, mblk_t *mp)
{
}

/**
 *  mux_lwsrv: - lower stream write service procedure
 *  @q: lower write queue
 *
 *  The lower write service procedure is used merely for backenabling across the multiplexing
 *  driver.  We never put messages to the lower write queue, but put them to the next queue below
 *  the lower write queue.  When bcanput fails on the next queue to the lower write queue, a back
 *  enable will invoke the lower write queue service procedure which can then be used to explicitly
 *  enable the upper write queue(s) feeding the lower write queue.
 */
STATIC streamscall int
mx_lwsrv(queue_t *q)
{
	struct mx *mx = q->q_ptr;
	struct mx *top;

	/* Find the upper queues feeding this one and enable them. */
	read_lock_bh(&mx_lock);
	for (top = mx_opens; top; top = top->next)
		qenable(mx->wq);
	read_unlock_bh(&mx_lock);
	return (0);
}

/**
 *  mx_uwsrv: - upper stream write service procedure
 *  @q: upper write queue
 *
 *  If the upper write put procedure encounters flow control on the queue beyond the accepting lower
 *  write queue, it places the message back on its queue and waits for the lower write queue service
 *  procedure to enable it when congestion has cleared.  If the upper write queue is disconnected
 *  (not connected yet or has disconnected), the write put procedure will either generate an M_ERROR
 *  (not connected yet) or M_HANGUP (disconnected) message upstream.  Note that M_ERROR at the
 *  Stream head will result in an M_FLUSH being sent downstream, but an M_HANGUP will not.  We
 *  therefore, flush the queue if invoked while disconnected.
 *
 *  The upper write service procedure is invoked only by the lower write service procedure when the
 *  lower write queue is back enabled.  This causes the backlog to clear.
 */
STATIC streamscall int
mx_uwsrv(queue_t *q)
{
	struct mx *mx = q->q_ptr;
	queue_t *wq == NULL;
	mblk_t *mp;

	/* FIXME: what we need to do here is, for each message in the loop, find the lower Stream
	   to which the message should be passed, check for flow control and pass it.  If no lower
	   Stream exists for the message, we need to provide an error response back upstream. */
	read_lock_bh(&mx_lock);
	if (mx->other)
		wq = mx->other->wq;
	read_unlock_bh(&mx_lock);

	if (!wq)
		wq = RD(q);

	while ((mp = getq(q))) {
		if (mp->b_datap->db_type >= QPCTL || bcanputnext(wq, mp->b_band)) {
			putnext(wq, mp);
			continue;
		}
		putbq(q, mp);
		break;
	}
	return (0);
}
