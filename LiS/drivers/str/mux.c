/*****************************************************************************

 @(#) $RCSfile: mux.c,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2005/12/20 15:11:41 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/12/20 15:11:41 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: mux.c,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2005/12/20 15:11:41 $"

static char const ident[] =
    "$RCSfile: mux.c,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2005/12/20 15:11:41 $";

/*
 *  This driver provides a multiplexing driver as an example and a test program.
 */

#include <stdbool.h>

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>	/* for local_bh_disable */
#include <linux/ctype.h>	/* for isdigit */

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#ifdef LFS
#include <sys/strsubr.h>
#endif
#include <sys/ddi.h>

#ifdef LFS
#include "sys/config.h"
#endif

#define MUX_DESCRIP	"UNIX/SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MUX_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define MUX_REVISION	"LfS $RCSfile: mux.c,v $ $Name:  $($Revision: 1.1.2.6 $) $Date: 2005/12/20 15:11:41 $"
#define MUX_DEVICE	"SVR 4.2 STREAMS Multiplexing Driver (MUX)"
#define MUX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MUX_LICENSE	"GPL"
#define MUX_BANNER	MUX_DESCRIP	"\n" \
			MUX_COPYRIGHT	"\n" \
			MUX_REVISION	"\n" \
			MUX_DEVICE	"\n" \
			MUX_CONTACT	"\n"
#define MUX_SPLASH	MUX_DEVICE	" - " \
			MUX_REVISION	"\n"

#if defined LIS && defined MODULE
#define CONFIG_STREAMS_MUX_MODULE MODULE
#endif

#ifdef CONFIG_STREAMS_MUX_MODULE
MODULE_AUTHOR(MUX_CONTACT);
MODULE_DESCRIPTION(MUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MUX_DEVICE);
MODULE_LICENSE(MUX_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mux");
#endif
#endif

#ifndef CONFIG_STREAMS_MUX_NAME
#ifdef LIS
#define CONFIG_STREAMS_MUX_NAME MUX__DRV_NAME
#endif
#ifdef LFS
#error CONFIG_STREAMS_MUX_NAME must be defined.
#endif
#endif
#ifndef CONFIG_STREAMS_MUX_MAJOR
#ifdef LIS
#define CONFIG_STREAMS_MUX_MAJOR MUX__CMAJOR_0
#endif
#ifdef LFS
#error CONFIG_STREAMS_MUX_MAJOR must be defined.
#endif
#endif
#ifndef CONFIG_STREAMS_MUX_MODID
#ifdef LIS
#define CONFIG_STREAMS_MUX_MODID MUX__ID
#endif
#ifdef LFS
#error CONFIG_STREAMS_MUX_MODID must be defined.
#endif
#endif

modID_t modid = CONFIG_STREAMS_MUX_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module id number for STREAMS-mux driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_MUX_MODID));
MODULE_ALIAS("streams-driver-mux");
#endif

major_t major = CONFIG_STREAMS_MUX_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for STREAMS-mux driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_MUX_MAJOR) "-*");
MODULE_ALIAS("/dev/mux");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_MUX_MAJOR));
MODULE_ALIAS("/dev/streams/mux");
MODULE_ALIAS("/dev/streams/mux/*");
#endif
#endif

#ifdef LIS
#define STRMINPSZ   0
#define STRMAXPSZ   4096
#define STRHIGH	    5120
#define STRLOW	    1024
#endif

STATIC struct module_info mux_minfo = {
	.mi_idnum = CONFIG_STREAMS_MUX_MODID,
	.mi_idname = CONFIG_STREAMS_MUX_NAME,
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
#endif

/* private structures */
struct mux {
	struct mux *next;		/* list linkage */
	struct mux **prev;		/* list linkage */
	struct mux *other;		/* upper or lower */
	queue_t *rq;			/* this rd queue */
	queue_t *wq;			/* this wr queue */
	dev_t dev;			/* device number */
};

/* blank structure for use by I_UNLINK/I_PUNLINK */
STATIC struct mux no_mux = {
	.next = NULL,
	.prev = &no_mux.next,
	.other = NULL,
	.rq = NULL,
	.wq = NULL,
	.dev = 0,
};

STATIC rwlock_t mux_lock = RW_LOCK_UNLOCKED;
STATIC struct mux *mux_opens = NULL;
STATIC struct mux *mux_links = NULL;

#define MUX_UP 1
#define MUX_DOWN 2

#ifdef LIS
#define streamscall _RP
#endif

STATIC streamscall int
mux_uwput(queue_t *q, mblk_t *mp)
{
	struct mux *mux = q->q_ptr, *bot;
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

			write_lock_bh(&mux_lock);
			bot->next = mux_links;
			bot->prev = &mux_links;
			mux_links = bot;
			bot->dev = l->l_index;
			bot->rq = RD(l->l_qtop);
			bot->wq = l->l_qtop;
			bot->other = NULL;
			noenable(bot->rq);
			l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = bot;
			write_unlock_bh(&mux_lock);

			goto ack;
		}
		case I_UNLINK:
		case I_PUNLINK:
		{
			struct linkblk *l;

			trace();
			if (!mp->b_cont) {
				ptrace(("Error path taken!\n"));
				goto einval;
			}
			l = (typeof(l)) mp->b_cont->b_rptr;

			write_lock_bh(&mux_lock);
			for (bot = mux_links; bot; bot = bot->next)
				if (bot->dev == l->l_index)
					break;
			if (bot) {
				struct mux *top;

				/* Note that lower multiplex driver put and service procedures must
				   be prepared to be invoked even after the M_IOCACK for the
				   I_UNLINK or I_PUNLINK ioctl has been returned.  This is because
				   the setq(9) back to the STREAM head is not performed until after
				   the acknowledgement has been received.  We set q->q_ptr to a null
				   multiplex struture to keep the lower STREAM functioning until the
				   setq(9) is performed. */

				l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = &no_mux;
				if ((*(bot->prev) = bot->next)) {
					bot->next = NULL;
					bot->prev = &bot->next;
				}
				bot->other = NULL;
				kmem_free(bot, sizeof(*bot));
				/* hang up all upper streams that feed this lower stream */
				for (top = mux_opens; top; top = top->next) {
					if (top->other == bot) {
						putnextctl(top->rq, M_HANGUP);
						top->other = NULL;
					}
				}
			}
			write_unlock_bh(&mux_lock);
			if (!bot)
				goto einval;
			goto ack;
		}
		case MUX_UP:
		{
			int l_index;

			trace();
			if (ioc->iocblk.ioc_count != sizeof(int))
				goto einval;
			if (!mp->b_cont)
				goto einval;
			l_index = *(int *) mp->b_cont->b_rptr;
			write_lock_bh(&mux_lock);
			for (bot = mux_links; bot; bot = bot->next)
				if (bot->dev == l_index)
					break;
			if (bot) {
				if (bot->other == NULL) {
					bot->other = mux;
					enableok(bot->rq);
					qenable(bot->rq);
				} else
					bot = NULL;
			}
			write_unlock_bh(&mux_lock);
			if (!bot)
				goto einval;
			goto ack;
		}
		case MUX_DOWN:
		{
			int l_index;

			trace();
			if (ioc->iocblk.ioc_count != sizeof(int))
				goto einval;
			if (!mp->b_cont)
				goto einval;
			l_index = *(int *) mp->b_cont->b_rptr;
			write_lock_bh(&mux_lock);
			for (bot = mux_links; bot; bot = bot->next)
				if (bot->dev == l_index)
					break;
			if (bot) {
				if (mux->other == NULL) {
					mux->other = bot;
					enableok(mux->wq);
					qenable(mux->wq);
				} else
					bot = NULL;
			}
			write_unlock_bh(&mux_lock);
			if (!bot)
				goto einval;
			goto ack;
		}
		default:
			ptrace(("Error path taken!\n"));
			if (mux->other)
				goto passmsg;
		      einval:
			err = -EINVAL;
			goto nak;
		      enomem:
			err = -ENOMEM;
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
		break;
	}
	case M_FLUSH:
	{
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
		}

		read_lock_bh(&mux_lock);
		if (mux->other) {
			queue_t *wq;

			if ((wq = mux->other->wq)) {
				putnext(wq, mp);
				read_unlock_bh(&mux_lock);
				return (0);
			}
		}
		read_unlock_bh(&mux_lock);

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
	default:
	      passmsg:
	{
		queue_t *wq = NULL;

		read_lock_bh(&mux_lock);
		if (mux->other)
			wq = mux->other->wq;
		read_unlock_bh(&mux_lock);

		/* if not linked behave like echo driver */
		if (!wq)
			wq = RD(q);

		/* Check the QSVCBUSY flag in MP drivers to avoid missequencing of messages when
		   service procedure is running concurrent with put procedure */
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

STATIC streamscall int
mux_lrput(queue_t *q, mblk_t *mp)
{
	struct mux *mux = q->q_ptr;

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
	{
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
		}

		read_lock_bh(&mux_lock);
		if (mux->other) {
			queue_t *rq;

			if ((rq = mux->other->rq)) {
				putnext(rq, mp);
				read_unlock_bh(&mux_lock);
				return (0);
			}
		}
		read_unlock_bh(&mux_lock);

		if (!(mp->b_flag & MSGNOLOOP)) {
			if (mp->b_rptr[0] & FLUSHW) {
				if (mp->b_rptr[0] & FLUSHBAND)
					flushband(WR(q), mp->b_rptr[1], FLUSHALL);
				else
					flushq(WR(q), FLUSHALL);
				mp->b_rptr[0] &= ~FLUSHR;
				/* pretend to be STREAM head */
				mp->b_flag |= MSGNOLOOP;
				qreply(q, mp);
				return (0);
			}
		}
		freemsg(mp);
		break;
	}
	default:
		/* check the QSVCBUSY flag in MP drivers to avoid missequencing of messages when
		   service procedure is running concurrent with put procedure */
		if (!q->q_first && !(q->q_flag & QSVCBUSY)) {
			read_lock_bh(&mux_lock);
			if (mux->other) {
				queue_t *rq;

				if ((rq = mux->other->rq)
				    && (mp->b_datap->db_type >= QPCTL
					|| bcanputnext(rq, mp->b_band))) {
					putnext(rq, mp);
					read_unlock_bh(&mux_lock);
					return (0);
				}
			}
			read_unlock_bh(&mux_lock);
		}
		putq(q, mp);
		break;
	}
	return (0);
}

/*
 *  The lower write service procedure is used merely for backenabling across the multiplexing
 *  driver.  We never put messages to the lower write queue, but put them to the next queue below
 *  the lower write queue.  When a bcanput fails on the next queue to the lower write queue, a back
 *  enable will invoke the lower write queue service procedure which can then be used to explicitly
 *  enable the upper write queue(s) feeding the lower write queue.
 *
 *  Note: This would be more efficient if we kept a separate list of feeding STREAMS instead of
 *  walking the entire list of upper STREAMS.
 */
STATIC streamscall int
mux_lwsrv(queue_t *q)
{
	struct mux *mux = q->q_ptr;
	struct mux *top;

	/* Find the upper queues feeding this one and enable them. */
	read_lock_bh(&mux_lock);
	for (top = mux_opens; top; top = top->next)
		if (top->other == mux)
			qenable(top->wq);
	read_unlock_bh(&mux_lock);
	return (0);
}

/*
 *  If the upper write put procedure encounters flow control on the queue beyond the accepting lower
 *  write queue, it places the message back on its queue and waits for the lower write queue service
 *  procedure to enable it when congestion has cleared.  If the upper write queue is disconnected
 *  (not connected yet or has disconnected), the write put procedure will either generate an M_ERROR
 *  (not connected yet) or M_HANGUP (disconnected) message upstream.  Note that M_ERROR at the
 *  STREAM head will result in an M_FLUSH being sent downstream, but an M_HANGUP will not.  We
 *  therefore, flush the queue if invoked while disconnected.
 *
 *  The upper write service procedure is invoked only by the lower write serivce procedure when the
 *  lower write queue is back enabled.  This causes the backlog to clear.
 */
STATIC streamscall int
mux_uwsrv(queue_t *q)
{
	struct mux *mux = q->q_ptr;
	queue_t *wq = NULL;
	mblk_t *mp;

	read_lock_bh(&mux_lock);
	if (mux->other)
		wq = mux->other->wq;
	read_unlock_bh(&mux_lock);

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

/*
 *  The upper read service procedure is invoked only by back enabling (because we do not ever place
 *  any messages on the upper read queue but always put them to the next read queue).  When invoked,
 *  we need to find all of the lower multiplexed STREAMS that feed into this STREAM and manually
 *  enable them.  This permits flow control to work across the multiplexing driver.
 *
 *  Note: This would be more efficient if we kept a separate list of feeding STREAMS instead of
 *  walking the entire list of upper STREAMS.
 */
STATIC streamscall int
mux_ursrv(queue_t *q)
{
	struct mux *mux = q->q_ptr;
	struct mux *bot;
	bool found = false;

	/* Find the lower queues feeding this one and enable them. */
	read_lock_bh(&mux_lock);
	for (bot = mux_links; bot; bot = bot->next)
		if (bot->other == mux) {
			qenable(bot->rq);
			found = true;
		}
	read_unlock_bh(&mux_lock);

	/* echo behaviour otherwise */
	if (!found)
		qenable(WR(q));

	return (0);
}

/*
 *  If the lower read put procedure encounters flow control on the queue beyond the accepting upper
 *  read queue, or if the lower read put procedure is invoked for a lower stream that is not
 *  connected across the multiplexer, it places the message back on its queue and waits for the
 *  upper read queue service procedure to enable it when congestion has cleared, or when a
 *  connection is formed.  If the upper read queue disconnects from the lower read queue (or has not
 *  connected yet), we leave these messages on the queue to (potentially) be processed by the stream
 *  head after unlinking, or to be processed by an upper read queue after connection across the
 *  multiplexer.
 */
STATIC streamscall int
mux_lrsrv(queue_t *q)
{
	struct mux *mux = q->q_ptr;
	queue_t *rq = NULL;
	mblk_t *mp;

	read_lock_bh(&mux_lock);
	if (mux->other)
		rq = mux->other->rq;
	if (rq) {
		while ((mp = getq(q))) {
			if (mp->b_datap->db_type >= QPCTL || bcanputnext(rq, mp->b_band)) {
				putnext(rq, mp);
				continue;
			}
			putbq(q, mp);
			break;
		}
	} else
		noenable(q);
	read_unlock_bh(&mux_lock);
	return (0);
}

STATIC streamscall int
mux_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct mux *mux, **muxp = &mux_opens;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next)
		return (ENXIO);	/* can't open as module */
	if (!(mux = kmem_alloc(sizeof(*mux), KM_NOSLEEP)))	/* we could sleep */
		return (ENOMEM);	/* no memory */
	bzero(mux, sizeof(*mux));
	switch (sflag) {
	case CLONEOPEN:
		if (cminor < 1)
			cminor = 2;
	case DRVOPEN:
	{
		major_t dmajor = 0;
		minor_t dminor = 0;

		if (cminor < 1)
			return (ENXIO);
		write_lock_bh(&mux_lock);
		for (; *muxp && (dmajor = getmajor((*muxp)->dev)) < cmajor; muxp = &(*muxp)->next) ;
		for (; *muxp && dmajor == getmajor((*muxp)->dev) &&
		     getminor(makedevice(cmajor, cminor)) != 0; muxp = &(*muxp)->next) {
			dminor = getminor((*muxp)->dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor) {
				if (sflag == CLONEOPEN)
					cminor++;
				else {
					write_unlock_bh(&mux_lock);
					kmem_free(mux, sizeof(*mux));
					return (EIO);	/* bad error */
				}
			}
		}
		if (getminor(makedevice(cmajor, cminor)) == 0) {
			write_unlock_bh(&mux_lock);
			kmem_free(mux, sizeof(*mux));
			return (EBUSY);	/* no minors left */
		}
		mux->dev = *devp = makedevice(cmajor, cminor);
		if ((mux->next = *muxp))
			mux->next->prev = &mux->next;
		mux->prev = muxp;
		*muxp = mux;
		mux->rq = q;
		mux->wq = WR(q);
		noenable(mux->wq);
		mux->rq->q_ptr = mux->wq->q_ptr = mux;
		write_unlock_bh(&mux_lock);
		qprocson(q);
		return (0);
	}
	}
	return (ENXIO);
}

STATIC streamscall int
mux_close(queue_t *q, int oflag, cred_t *crp)
{
	struct mux *p;

	if ((p = q->q_ptr) == NULL)
		return (0);	/* already closed */
	qprocsoff(q);
	write_lock_bh(&mux_lock);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	q->q_ptr = OTHERQ(q)->q_ptr = NULL;
	write_unlock_bh(&mux_lock);
	return (0);
}

STATIC struct qinit mux_urqinit = {
	.qi_srvp = mux_ursrv,
	.qi_qopen = mux_open,
	.qi_qclose = mux_close,
	.qi_minfo = &mux_minfo,
};

STATIC struct qinit mux_uwqinit = {
	.qi_putp = mux_uwput,
	.qi_srvp = mux_uwsrv,
	.qi_minfo = &mux_minfo,
};

STATIC struct qinit mux_lrqinit = {
	.qi_putp = mux_lrput,
	.qi_srvp = mux_lrsrv,
	.qi_minfo = &mux_minfo,
};

STATIC struct qinit mux_lwqinit = {
	.qi_srvp = mux_lwsrv,
	.qi_minfo = &mux_minfo,
};

STATIC struct streamtab mux_info = {
	.st_rdinit = &mux_urqinit,
	.st_wrinit = &mux_uwqinit,
	.st_muxrinit = &mux_lrqinit,
	.st_muxwinit = &mux_lwqinit,
};

#ifdef LFS
STATIC struct cdevsw mux_cdev = {
	.d_name = CONFIG_STREAMS_MUX_NAME,
	.d_str = &mux_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};
#endif

#ifdef CONFIG_STREAMS_MUX_MODULE
STATIC
#endif
int __init
mux_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_MUX_MODULE
	printk(KERN_INFO MUX_BANNER);
#else
	printk(KERN_INFO MUX_SPLASH);
#endif
	mux_minfo.mi_idnum = modid;
#ifdef LFS
	if ((err = register_strdev(&mux_cdev, major)) < 0)
		return (err);
#endif
#ifdef LIS
	if ((err = register_strdev(major, &mux_info, 255, CONFIG_STREAMS_MUX_NAME)) < 0)
		return (err);
#endif
	if (major == 0 && err > 0)
		major = err;
	return (0);
}

#ifdef CONFIG_STREAMS_MUX_MODULE
STATIC
#endif
void __exit
mux_exit(void)
{
#ifdef LFS
	unregister_strdev(&mux_cdev, major);
#endif
#ifdef LIS
	lis_unregister_strdev(major);
#endif
}

#ifdef CONFIG_STREAMS_MUX_MODULE
module_init(mux_init);
module_exit(mux_exit);
#endif
