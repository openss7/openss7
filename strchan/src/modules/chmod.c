/*****************************************************************************

 @(#) $RCSfile: chmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/30 13:01:06 $

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

 Last Modified $Date: 2006/11/30 13:01:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: chmod.c,v $
 Revision 0.9.2.1  2006/11/30 13:01:06  brian
 - added working files

 *****************************************************************************/

#ident "@(#) $RCSfile: chmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/30 13:01:06 $"

static char const ident[] = "$RCSfile: chmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/30 13:01:06 $";

/*
 *  CH-MOD: This is a channel module that can be pushed over one end of a Streams-based pipe to form
 *  a communications channel similar to that provided by a multiplex driver.  The major purpose of
 *  this module is for testing, but it can also be used to simulate a channel within a system.
 */

#define LFS_SOURCE
#define MPS_SOURCE

#include <sys/strlog.h>

#ifndef DB_TYPE
#define DB_TYPE(__mp) ((__mp)->b_datap->db_type)
#endif				/* DB_TYPE */

enum {
	CH_ENABLE_BIT = 0,
#define CH_ENABLE_BIT	CH_ENABLE_BIT
	CH_LOCK_BIT,
#define CH_LOCK_BIT	CH_LOCK_BIT
};

static modID_t modid = MOD_ID;

static fastcall __unlikely int
ch_m_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
}

static fastcall __unlikely int
ch_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & ((q->q_flag & QREADR) ? FLUSHR : FLUSHW)) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	/* do a pipemod flush sense reversal too */
	switch (mp->b_rptr[0] & FLUSHRW) {
	case FLUSHR:
		mp->b_rptr[0] &= ~FLUSHRW;
		mp->b_rptr[0] |= FLUSHW;
		break;
	case FLUSHW:
		mp->b_rptr[0] &= ~FLUSHRW;
		mp->b_rptr[0] |= FLUSHR;
		break;
	case FLUSHRW:
		mp->b_rptr[0] &= ~FLUSHRW;
		mp->b_rptr[0] |= FLUSHRW;
		break;
	}
	putnext(q, mp);
	return (0);
}

static noinline fastcall __unlikely int
ch_msg_slow(struct ch *ch, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return ch_m_data(ch, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ch_m_proto(ch, q, mp);
	case M_FLUSH:
		return ch_m_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return ch_m_ioctl(ch, q, mp);
	case M_READ:
		return ch_m_read(ch, q, mp);
	default:
		swerr();
		freemsg(mp);
		return (0);
	}
}

/**
 * ch_msg: - process message from queue
 * @q: queue
 * @mp: the message to process
 *
 * This simply flows data messages through with flow control and intercepts
 * all other messages.
 */
static inline fastcall __hot int
ch_msg(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);

	if (test_bit(CH_ENABLE_BIT, &ch->flags)) {
		if (likely(DB_TYPE(mp) == M_DATA)) {
			if (likely(bcanputnext(q, mp->b_band))) {
				putnext(q, mp);
				return (0);
			}
			return (-EBUSY);
		} else if (DB_TYPE(mp) == M_PROTO) {
			if (*(uint32_t *) mp->b_rptr = CH_DATA_REQ) {
				if (likely(bcanputnext(q, mp->b_band))) {
					*(uint32_t *) mp->b_rptr = CH_DATA_IND;
					putnext(q, mp);
					return (0);
				}
				return (-EBUSY);
			}
			/* go slow */
		}
	}
	return ch_msg_slow(ch, q, mp);
}

/**
 * ch_putp: - put procedure
 * @q: queue to which to put
 * @mp: message to put
 *
 * Simple flow-through queueing service procedure.
 */
static streamscall __hot int
ch_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || ch_msg(q, mp)) {
		if (putq(q, mp)) {
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

/**
 * ch_srvp: - service procedure.
 * @q: queue to service
 *
 * Simple draining service procedure.
 */
static streamscall __hot_get int
ch_srvp(queue_t *q, mblk_t *mp)
{
	mblk_t *mp;

	if (likely((mp = getq(q)))) {
		do {
			if (ch_msg(q, mp)) {
				if (putbq(q, mp)) {
					swerr();
					freemsg(mp);
				}
				break;
			}
		} while (unlikely((mp = getq(q))));
	}
	if (_WR(q)->q_first)
		qenable(_WR(q));
	return (0);
}

static caddr_t ch_head;
static atomic_t ch_numb;

static streamscall __unlikely int
ch_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct ch *ch;
	int err;

	if (!SAMESTR(_WR(q))) {
		strlog(modid, 0, 15, SL_ERROR, "%s: can only push on pipe or fifo", MOD_NAME);
		return (ENXIO);	/* can only be pushed on pipe or fifo */
	}
	if ((err = mi_open_comm(&ch_head, sizeof(*ch), q, devp, oflags, sflag, crp)))
		return (err);
	ch = CH_PRIV(q);
	bzero(ch, sizeof(*ch));
	ch->sid = atomic_add_return(&ch_numb, 1);
	/* FIXME initialize structure */
	qprocson(q);
	return (0);
}

static streamscall __unlikely int
ch_qclose(queue_t *q, dev_t *devp, int oflags)
{
	int err;

	qprocsoff(q);
	if ((err = mi_close_comm(&ch_head, q)))
		return (err);
	return (0);
}

/*
 *  STREAMS Definitions.
 */
static struct qinit ch_winit = {
	.qi_putp = ch_putp,
	.qi_srvp = ch_srvp,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_wstat,
};

static struct qinit ch_rinit = {
	.qi_putp = ch_putp,
	.qi_srvp = ch_srvp,
	.qi_qopen = ch_open,
	.qi_qclose = ch_close,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_rstat,
};

struct streamtab chmodinfo = {
	.st_rdinit = &ch_rinit,
	.st_wrinit = &ch_winit,
};

/*
 * Linux and STREAMS registration.
 */

#ifdef module_param
module_param(modid, ushort, 0);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for the CH-MOD module. (0 for allocation.)");

static struct fmodsw ch_fmod = {
	.f_name = "chmod",
	.f_str = &chmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static void
cmterminate(void)
{
	int err;

	if ((err = unregister_strmod(modid, &ch_fmod)) < 0)
		strlog(modid, 0, 15, SL_ERROR | SL_NOPUTBUF, "unregister_strmod() failed %d", -err);
	return;
}
static int
cminit(void)
{
	int err;

	atomic_set(&ch_numb, 0);
	if ((err = register_strmod(modid, &ch_fmod)) < 0)
		return (err);
	if (modid == 0)
		modid = err;
	return (0);
}

module_init(cminit);
module_term(cmterminate);
