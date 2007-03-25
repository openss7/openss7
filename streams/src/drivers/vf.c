/*****************************************************************************

 @(#) $RCSfile: vf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/03/25 06:00:17 $

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

 Last Modified $Date: 2007/03/25 06:00:17 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: vf.c,v $
 Revision 0.9.2.11  2007/03/25 06:00:17  brian
 - flush corrections

 Revision 0.9.2.10  2006/12/18 10:08:58  brian
 - updated headers for release

 Revision 0.9.2.9  2006/12/18 07:32:40  brian
 - lfs device names, autoload clone minors, device numbering, missing manpages

 Revision 0.9.2.8  2006/10/27 23:19:34  brian
 - changes for 2.6.18 kernel

 Revision 0.9.2.7  2006/10/12 10:22:47  brian
 - removed redundant debug flags

 Revision 0.9.2.6  2006/07/25 06:39:07  brian
 - expanded minor device numbers and optimization and locking corrections

 Revision 0.9.2.5  2006/07/24 09:01:15  brian
 - results of udp2 optimizations

 Revision 0.9.2.4  2005/12/28 09:48:02  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.3  2005/12/09 18:01:41  brian
 - profiling copy

 Revision 0.9.2.2  2005/10/07 09:34:14  brian
 - more testing and corrections

 Revision 0.9.2.1  2005/09/11 02:38:59  brian
 - added experimental verification driver

 *****************************************************************************/

#ident "@(#) $RCSfile: vf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/03/25 06:00:17 $"

static char const ident[] = "$RCSfile: vf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/03/25 06:00:17 $";

/*
 *  This driver provides some capabilities for testing Linux Fast-STREAMS.  It functions as a Null
 *  Stream device as described in the SVR 4 STREAMS Programmer's Guide, the Loop-Around driver also
 *  described in the SVR4 SPG and the Multiplexing Driver described there.  In addition, it provides
 *  specialized ioctl(2) commands for running internal tests on STREAMS utility functions.  It's
 *  primary purpose is to provide a test capability for the test-streams(8) conformance test program
 *  and the testsuite-streams autotest suite.  It might have some use in providing an example of a
 *  more complicated driver and module.
 */

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"

#define VF_DESCRIP	"UNIX/SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define VF_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define VF_REVISION	"LfS $RCSfile: vf.c,v $ $Name:  $ ($Revision: 0.9.2.11 $) $Date: 2007/03/25 06:00:17 $"
#define VF_DEVICE	"SVR 4.2 STREAMS Verification Driver (VF)"
#define VF_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define VF_LICENSE	"GPL"
#define VF_BANNER	VF_DESCRIP	"\n" \
			VF_COPYRIGHT	"\n" \
			VF_REVISION	"\n" \
			VF_DEVICE	"\n" \
			VF_CONTACT	"\n"
#define VF_SPLASH	VF_DEVICE	" - " \
			VF_REVISION	"\n"

#ifdef CONFIG_STREAMS_VF_MODULE
MODULE_AUTHOR(VF_CONTACT);
MODULE_DESCRIPTION(VF_DESCRIP);
MODULE_SUPPORTED_DEVICE(VF_DEVICE);
MODULE_LICENSE(VF_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-vf");
#endif
#endif

#ifndef CONFIG_STREAMS_VF_NAME
#error CONFIG_STREAMS_VF_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_VF_MAJOR
#error CONFIG_STREAMS_VF_MAJOR must be defined.
#endif
#ifndef CONFIG_STREAMS_VF_MODID
#error CONFIG_STREAMS_VF_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_VF_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module id number for STREAMS-vf driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_VF_MODID));
MODULE_ALIAS("streams-driver-vf");
#endif

major_t major = CONFIG_STREAMS_VF_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for STREAMS-vf driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_VF_MAJOR) "-*");
MODULE_ALIAS("/dev/vf");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_VF_MAJOR));
MODULE_ALIAS("/dev/streams/vf");
MODULE_ALIAS("/dev/streams/vf/*");
MODULE_ALIAS("/dev/streams/clone/vf");
#endif
#endif

STATIC struct module_info vf_minfo = {
	.mi_idnum = CONFIG_STREAMS_VF_MODID,
	.mi_idname = CONFIG_STREAMS_VF_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat vf_urstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat vf_uwstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat vf_lrstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat vf_lwstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

/* private structure */
struct vf {
	struct vf *next;		/* list linkage */
	struct vf **prev;		/* list linkage */
	struct vf *upper;		/* other upper stream */
	struct vf *lower;		/* other lower stream */
	queue_t *rq;			/* this rd queue */
	queue_t *wq;			/* this wr queue */
	dev_t dev;			/* device number */
	int index;			/* multiplex index */
};

STATIC spinlock_t vf_lock = SPIN_LOCK_UNLOCKED;
STATIC struct vf *vf_opens = NULL;
STATIC struct vf *vf_links = NULL;

/*
 *  Locking
 */
#if defined CONFIG_STREAMS_NOIRQ || defined _TEST

#define spin_lock_str(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)

#else

#define spin_lock_str(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)

#endif

#define MUX_UP 1
#define MUX_DOWN 2

STATIC streams_fastcall int
vf_uwput(queue_t *q, mblk_t *mp)
{
	struct vf *vf = q->q_ptr, *bot, *top;
	int err;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	{
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
		unsigned long flags;

		switch (ioc->iocblk.ioc_cmd) {
		case I_LINK:
		case I_PLINK:
		{
			struct linkblk *l;

			if (!mp->b_cont)
				goto einval;
			if (!(bot = kmem_alloc(sizeof(*bot), KM_NOSLEEP)))
				goto enomem;
			l = (typeof(l)) mp->b_cont->b_rptr;

			spin_lock_str(&vf_lock, flags);
			bot->next = vf_links;
			bot->prev = &vf_links;
			vf_links = bot;
			bot->index = l->l_index;
			bot->rq = RD(l->l_qtop);
			bot->wq = l->l_qtop;
			bot->upper = NULL;
			bot->lower = NULL;
			noenable(bot->rq);
			l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = bot;
			spin_unlock_str(&vf_lock, flags);

			goto ack;
		}
		case I_UNLINK:
		case I_PUNLINK:
		{
			struct linkblk *l;

			if (!mp->b_cont)
				goto einval;
			l = (typeof(l)) mp->b_cont->b_rptr;

			spin_lock_str(&vf_lock, flags);
			for (bot = vf_links, bot; bot = bot->next)
				if (bot->index = l->l_index)
					break;
			if (bot) {
				l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = NULL;
				if ((*(bot->prev) = bot->next)) {
					bot->next = NULL;
					bot->prev = &bot->next;
				}
				bot->upper = NULL;
				bot->lower = NULL;
				kmem_free(bot, sizeof(*bot));
				/* hang up all upper streams that feed this stream */
				for (top = vf_opens; top; top = top->next) {
					if (top->lower == bot) {
						putnextctl(top->rq, M_HANGUP);
						top->lower = NULL;
					}
				}
				/* disconnect lower streams that feed this stream */
				for (top = vf_links; top; top = top->next)
					if (top->lower == bot)
						top->lower == NULL;
			}
			spin_unlock_str(&vf_lock, flags);
			if (!bot)
				goto einval;
			goto ack;
		}
		case MUX_SET:
		{
			int l_index;

			/* Connect the calling upper stream to the indexed lower stream for both
			   directions of flow. */
			if (ioc->iocblk.ioc_count != sizeof(l_index))
				goto einval;
			if (!mp->b_cont)
				goto einval;
			l_index = *((typeof(l_index)) *) mp->b_cont->b_rptr;
			spin_lock_str(&vf_lock, flags);
			for (bot = vf_links; bot; bot = bot->next)
				if (bot->index == l_index)
					break;
			if (bot) {
				if (vf->upper == NULL && vf->lower == NULL
				    && bot->upper == NULL && bot->lower == NULL) {
					vf->lower = bot;
					bot->upper = vf;
					enableok(vf->wq);
					enableok(bot->rq);
					qenable(vf->wq);
					qenable(bot->rq);
				} else
					bot = NULL;
			}
			spin_unlock_str(&vf_lock, flags);
			if (!bot)
				goto einval;
			goto ack;
		}
		case MUX_UP:
		{
			int l_index;

			/* Connect the indexed lower stream to the calling upper stream for the
			   upstream path */
			if (ioc->iocblk.ioc_count != sizeof(l_index))
				goto einval;
			if (!mp->b_cont)
				goto einval;
			l_index = *((typeof(l_index)) *) mp->b_cont->b_rptr;
			spin_lock_str(&vf_lock, flags);
			for (bot = vf_links; bot; bot = bot->next)
				if (bot->index == l_index)
					break;
			if (bot) {
				if (bot->upper == NULL && bot->lower == NULL) {
					bot->upper = vf;
					enableok(bot->rq);
					qenable(bot->rq);
				} else
					bot = NULL;
			}
			spin_unlock_str(&vf_lock, flags);
			if (!bot)
				goto einval;
			goto ack;
		}
		case MUX_DOWN:
		{
			int l_index;

			/* Connect the calling upper stream to the indexed lower stream for the
			   downstream path. */
			if (ioc->iocblk.ioc_count != sizeof(l_index))
				goto einval;
			if (!mp->b_cont)
				goto einval;
			l_index = *((typeof(l_index)) *) mp->b_cont->b_rptr;
			spin_lock_str(&vf_lock, flags);
			for (bot = vf_links; bot; bot = bot->next)
				if (bot->index == l_index)
					break;
			if (bot) {
				if (vf->upper == NULL && vf->lower == NULL) {
					vf->lower = bot;
					enableok(vf->wq);
					qenable(vf->wq);
				} else
					bot = NULL;
			}
			spin_unlock_str(&vf_lock, flags);
			if (!bot)
				goto einval;
			goto ack;
		}
		case LOOP_SET:
		{
			dev_t dev;

			/* Connect the calling upper stream to the specified other upper stream for
			   both directions of flow. */
			if (ioc->iocblk.ioc_count != sizeof(dev))
				goto einval;
			if (!mp->b_cont)
				goto einval;
			dev = *((typeof(dev)) *) mp->b_cont->b_rptr;
			spin_lock_str(&vf_lock, flags);
			for (top = vf_opens; top; top = top->next)
				if (top->dev == dev)
					break;
			if (top) {
				if (vf->upper == NULL && vf->lower == NULL
				    && top->upper == NULL && top->lower == NULL) {
					vf->upper = top;
					top->upper = vf;
					enableok(vf->wq);
					enableok(top->wq);
					qenable(vf->wq);
					qenable(top->wq);
				} else
					top = NULL;
			}
			spin_unlock_str(&vf_lock, flags);
			if (!top)
				goto einval;
			goto ack;
		}
		case LOOP_UP:
		{
			dev_t dev;

			/* Connect the specified other upper stream to the calling upper stream for
			   the upstream direction of flow. */
			if (ioc->iocblk.ioc_count != sizeof(dev))
				goto einval;
			if (!mp->b_cont)
				goto einval;
			dev = *((typeof(dev)) *) mp->b_cont->b_rptr;
			spin_lock_str(&vf_lock, flags);
			for (top = vf_opens; top; top = top->next)
				if (top->dev == dev)
					break;
			if (top) {
				if (top->upper == NULL && top->lower == NULL) {
					top->upper = vf;
					enableok(top->wq);
					qenable(top->wq);
				} else
					top = NULL;
			}
			spin_unlock_str(&vf_lock, flags);
			if (!top)
				goto einval;
			goto ack;
		}
		case LOOP_DOWN:
		{
			dev_t dev;

			/* Connect the calling upper stream to the other specified upper stream for
			   the downstream direction of flow. */
			if (ioc->iocblk.ioc_count != sizeof(dev))
				goto einval;
			if (!mp->b_cont)
				goto einval;
			dev = *((typeof(dev)) *) mp->b_cont->b_rptr;
			spin_lock_str(&vf_lock, flags);
			for (top = vf_opens; top; top = top->next)
				if (top->dev == dev)
					break;
			if (top) {
				if (vf->upper == NULL && vf->lower == NULL) {
					vf->upper = top;
					enableok(vf->wq);
					qenable(vf->wq);
				} else
					top = NULL;
			}
			spin_unlock_str(&vf_lock, flags);
			if (!top)
				goto einval;
			goto ack;
		}
		default:
			if (vf->upper || vf->lower)
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
			break;
		      ack:
			mp->b_datap->db_type = M_IOCACK;
			ioc->iocblk.ioc_count = 0;
			ioc->iocblk.ioc_error = 0;
			ioc->iocblk.ioc_rval = 0;
			qreply(q, mp);
			break;
		}
		break;
	}
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		spin_lock_str(&vf_lock, flags);
		if (vf->lower) {
			queue_t *wq;

			wq = vf->lower->wq;
			spin_unlock_str(&vf_lock, flags);
			putnext(wq, mp);
		} else if (vf->upper) {
			queue_t *rq;

			rq = vf->upper->rq;
			spin_unlock_str(&vf_lock, flags);
			/* switch sense of flush flags */
			switch (mp->b_rptr[0] & (FLUSHW | FLUSHR | FLUSHRW)) {
			case FLUSHR:
				mp->b_rptr[0] = (mp->b_rptr[0] & ~FLUSHR) | FLUSHW;
				break;
			case FLUSHW:
				mp->b_rptr[0] = (mp->b_rptr[0] & ~FLUSHW) | FLUSHR;
				break;
			}
			putnext(rq, mp);
		} else {
			spin_unlock_str(&vf_lock, flags);
			if (mp->b_rptr[0] & FLUSHR) {
				if (mp->b_rptr[0] & FLUSHBAND)
					flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
				else
					flushq(RD(q), FLUSHDATA);
				mp->b_rptr[0] &= ~FLUSHW;
				qreply(q, mp);
				break;
			}
			freemsg(mp);
		}
		break;
	default:
	      passmsg:
		spin_lock_str(&vf_lock, flags);
		if (vf->upper || vf->lower) {
			if (!q->q_first) {
				queue_t *oq = NULL;

				if (vf->upper)
					oq = vf->upper->rq;
				if (vf->lower)
					oq = vf->lower->wq;
				spin_unlock_str(&vf_lock, flags);
				if (oq) {
					if (mp->b_datap->db_type >= QPCTL
					    || bcanputnext(oq, mp->b_band)) {
						putnext(oq, mp);
						break;
					}
				}
			} else
				spin_unlock_str(&vf_lock, flags);
			putq(q, mp);
		} else {
			spin_unlock_str(&vf_lock, flags);
			freemsg(mp);
			putnextctl2(OTHERQ(q), M_ERROR, ENXIO, ENXIO);
		}
		break;
	}
	return (0);
}

/*
 *  If an upper write put procedure encounters flow control on the queue beyond the connected lower
 *  write queue or the connected upper read queue, it places the message back on its queue and waits
 *  fro the lower write queue or upper read queue service procedrue to enable it when flow control
 *  subsides.  If the upper write queue is disconnected (not connected yet or has disconnected), the
 *  write put procedure will either generate an M_ERROR (not connected yet) or M_HANGUP
 *  (disconnected) message upstream.  Note that the M_ERROR at the STREAM head will result in an
 *  M_FLUSH message being sent downstream, but an M_HANGUP will not.  We, therefore, flush the queue
 *  if invoked while disconnected.
 *
 *  The upper write service procedure is invoked only by the connected lower write or connected
 *  upper read service procedure when the queue is back enabled.  This causes the backlog to clear.
 */
STATIC streams_fastcall int
vf_uwsrv(queue_t *q)
{
	struct vf *vf = q->q_ptr;
	unsigned long flags;
	queue_t oq = NULL;
	mblk_t *mp;

	spin_lock_str(&vf_lock, flags);
	if (vf->upper)
		oq = vf->upper->rq;
	if (vf->lower)
		oq = vf->lower->wq;
	spin_unlock_str(&vf_lock, flags);
	if (oq) {
		while ((mp = getq(q))) {
			if (mp->b_datap->db_type >= QPCTL || bcanputnext(oq, mp->b_band)) {
				putnext(oq, mp);
				continue;
			}
			putbq(q, mp);
			break;
		}
	} else
		flushq(q, FLUSHALL);
	return (0);
}

STATIC streams_fastcall int
vf_ursrv(queue_t *q)
{
	struct vf *vf = q->q_ptr, *top, *bot;
	unsigned long flags;

	/* Find the other queues feeding this one and enable them. */
	spin_lock_str(&vf_lock, flags);
	for (top = vf_opens; top; top = top->next)
		if (top->upper == vf)
			qenable(top->wq);
	for (bot = vf_links; bot; bot = bot->next)
		if (bot->upper == vf)
			qenable(bot->rq);
	spin_unlock_str(&vf_lock, flags);
	return (0);
}

STATIC streams_fastcall int
vf_lrput(queue_t *q, mblk_t *mp)
{
	struct vf *vf = q->q_ptr;
	unsigned long flags;

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		spin_lock_str(&vf_lock, flags);
		if (vf->upper) {
			queue_t *rq;

			rq = vf->upper->rq;
			spin_unlock_str(&vf_lock, flags);
			putnext(rq, mp);
		} else if (vf->lower) {
			queue_t *wq;

			wq = vf->lower->wq;
			spin_unlock_str(&vf_lock, flags);
			/* switch sense of flush flags */
			switch (mp->b_rptr[0] & (FLUSHW | FLUSHR | FLUSHRW)) {
			case FLUSHR:
				mp->b_rptr[0] = (mp->b_rptr[0] & ~FLUSHR) | FLUSHW;
				break;
			case FLUSHW:
				mp->b_rptr[0] = (mp->b_rptr[0] & ~FLUSHW) | FLUSHR;
				break;
			}
			putnext(wq, mp);
		} else {
			spin_unlock_str(&vf_lock, flags);
			if (!(mp->b_flags & MSGNOLOOP)) {
				if (mp->b_rptr[0] & FLUSHW) {
					if (mp->b_rptr[0] & FLUSHBAND)
						flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
					else
						flushq(WR(q), FLUSHDATA);
					mp->b_rptr[0] &= ~FLUSHR;
					mp->b_flag |= MSGNOLOOP;
					qreply(q, mp);
					break;
				}
			}
			freemsg(mp);
		}
		break;
	default:
	{
		if (!q->q_first) {
			queue_t *oq = NULL;

			spin_lock_str(&vf_lock, flags);
			if (vf->upper)
				oq = vf->upper - rq;
			if (vf->lower)
				oq = vf->lower->wq;
			spin_unlock_str(&vf_lock, flags);
			if (oq) {
				if (mp->b_datap->db_type >= QPCTL || bcanputnext(oq, mp->b_band)) {
					putnext(oq, mp);
					break;
				}
			}
		}
		putq(q, mp);
		break;
	}
	}
}

STATIC streams_fastcall int
vf_lrsrv(queue_t *q)
{
	struct vf *vf = q->q_ptr;
	unsigned long flags;
	queue_t oq = NULL;
	mblk_t *mp;

	spin_lock_str(&vf_lock, flags);
	if (vf->upper)
		oq = vf->upper->rq;
	if (vf->lower)
		oq = vf->lower->wq;
	spin_unlock_str(&vf_lock, flags);
	if (oq) {
		while ((mp = getq(q))) {
			if (mp->b_datap->db_type >= QPCTL || bcanputnext(oq, mp->b_band)) {
				putnext(oq, mp);
				continue;
			}
			putbq(q, mp);
			break;
		}
	} else
		noenable(q);
	return (0);
}

STATIC streams_fastcall int
vf_lwsrv(queue_t *q)
{
	struct vf *vf = q->q_ptr, *top, *bot;
	unsigned long flags;

	/* Find the other queues feeding this one and enable them. */
	spin_lock_str(&vf_lock, flags);
	for (top = vf_opens; top; top = top->next)
		if (top->upper == vf)
			qenable(top->wq);
	for (bot = vf_links; bot; bot = bot->next)
		if (bot->upper == vf)
			qenable(bot->rq);
	spin_unlock_str(&vf_lock, flags);
	return (0);
}

STATIC int
vf_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct vf *vf, **vfp = &vf_opens;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	unsigned long flags;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next)
		return (ENXIO);	/* can't open as module */
	if (!(vf = kmem_alloc(sizeof(*vf), KM_NOSLEEP)))	/* we could sleep */
		return (ENOMEM);	/* no memory */
	bzero(vf, sizeof(*vf));
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
		spin_lock_str(&vf_lock, flags);
		for (; *vfp && (dmajor = getmajor((*vfp)->dev)) < cmajor; vfp = &(*vfp)->next) ;
		for (; *vfp && dmajor == getmajor((*vfp)->dev)
		     && getminor(makedevice(cmajor, cminor)) != 0; vfp = &(*vfp)->next) {
			dminor = getminor((*vfp)->dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor) {
				if (sflag == CLONEOPEN)
					cminor++;
				else {
					spin_unlock_str(&vf_lock, flags);
					kmem_free(vf, sizeof(*vf));
					return (EIO);	/* bad error */
				}
			}
		}
		if (getminor(makedevice(cmajor, cminor)) == 0) {
			spin_unlock_str(&vf_lock, flags);
			kmem_free(vf, sizeof(*vf));
			return (EBUSY);	/* no minors left */
		}
		vf->dev = *devp = makedevice(cmajor, cminor);
		if ((vf->next = *vfp))
			vf->next->prev = &vf->next;
		vf->prev = vfp;
		*vfp = vf;
		vf->rq = q;
		vf->wq = WR(q);
		noenable(vf->wq);
		vf->rq->q_ptr = vf->wq->q_ptr = vf;
		spin_unlock_str(&vf_lock, flags);
		qprocson(q);
		return (0);
	}
	}
	return (ENXIO);
}

STATIC int
vf_close(queue_t *q, int oflag, cred_t *crp)
{
	struct vf *p;
	unsigned long flags;

	if ((p = q->q_ptr) == NULL)
		return (0);	/* already closed */
	qprocsoff(q);
	spin_lock_str(&vf_lock, flags);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	q->q_ptr = OTHERQ(q)->q_ptr = NULL;
	spin_unlock_str(&vf_lock, flags);
	return (0);
}

STATIC struct qinit vf_urqinit = {
	.qi_srvp = vf_ursrv,
	.qi_qopen = vf_open,
	.qi_qclose = vf_close,
	.qi_minfo = &vf_minfo,
	.qi_mstat = &vf_urstat,
};

STATIC struct qinit vf_uwqinit = {
	.qi_putp = vf_uwput,
	.qi_srvp = vf_uwsrv,
	.qi_minfo = &vf_minfo,
	.qi_mstat = &vf_uwstat,
};

STATIC struct qinit vf_lrqinit = {
	.qi_putp = vf_lrput,
	.qi_srvp = vf_lrsrv,
	.qi_minfo = &vf_minfo,
	.qi_mstat = &vf_lrstat,
};

STATIC struct qinit vf_lwqinit = {
	.qi_srvp = vf_lwsrv,
	.qi_minfo = &vf_minfo,
	.qi_mstat = &vf_lwstat,
};

STATIC struct streamtab vf_info = {
	.st_rdinit = &vf_urqinit,
	.st_wrinit = &vf_uwqinit,
	.st_muxrinit = &vf_lrqinit,
	.st_muxwinit = &vf_lwqinit,
};

STATIC struct cdevsw vf_cdev = {
	.d_name = CONFIG_STREAMS_LOG_NAME,
	.d_str = &vf_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_VF_MODULE
STATIC
#endif
int __init
vf_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_VF_MODULE
	printk(KERN_INFO VF_BANNER);
#else
	printk(KERN_INFO VF_SPLASH);
#endif
	vf_minfo.mi_idnum = modid;
	if ((err = register_strdev(&vf_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
}

#ifdef CONFIG_STREAMS_VF_MODULE
STATIC
#endif
void __exit
vf_exit(void)
{
	unregister_strdev(&vf_cdev, major);
}

#ifdef CONFIG_STREAMS_VF_MODULE
module_init(vf_init);
module_exit(vf_exit);
#endif
