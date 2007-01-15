/*****************************************************************************

 @(#) $RCSfile: sl-mux.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:56 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/01/15 11:33:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sl-mux.c,v $
 Revision 0.9.2.1  2007/01/15 11:33:56  brian
 - added new and old signalling link utilities

 *****************************************************************************/

#ident "@(#) $RCSfile: sl-mux.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:56 $"

static char const ident[] = "$RCSfile: sl-mux.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:33:56 $";

/*
 *  This is a signalling link multiplexer: its primary purpose is to provide a single nexus for the
 *  convenient arrangement and handling of signalling link interfaces.  Signalling links are opened
 *  on other drivers and attached to a specific PPA using the procedures associated with that
 *  device.  The signalling link stream is then linked under this multiplexing driver using the
 *  I_PLINK input-output control.  Once linked, configuration input-output controls specific to this
 *  driver are used to assign a global PPA and a CLEI to the signalling link.  Streams opened on
 *  this SL-MUX driver can then attach to any defined global PPA or CLEI which results in
 *  atttachment of the upper multiplex stream to the appropriate lower multiplex stream.
 */


/**
 * sl_w_ioctl: - process M_IOCTL message
 * @q: active queue (upper write queue)
 * @mp: the message
 *
 * Linking of streams: streams are linked under the multiplexing driver by opening an upper stream
 * and then linking a signalling link stream under the multiplexing driver.  Then the SL_SETLINK
 * input-output control is used with the multiplexer id to set the global-PPA and CLEI associated
 * with the signalling link.  The SL_GETLINK input-output control can be used at a later date to
 * determine the multiplexer id for a given signalling link stream.
 */
static struct int
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (struct iocblk *) mp->b_rptr;

	switch (ioc->ioc_cmd) {
	case I_LINK:
	case I_PLINK:
	{
		struct linkblk *l;

		if (!mp->b_cont)
			mi_copy_done(q, mp, EINVAL);

		l = (struct linkblk *) mp->b_cont->b_rptr;

		if (!(bot = kmem_alloc(sizeof(*bot), KM_NOSLEEP)))
			mi_copy_done(q, mp, ENOMEM);

		write_lock_str(&mux_lock, flags);
		bot->next = mux_links;
		bpt->prev = &mux_links;
		mux_links = bot;
		bot->dev = l->l_index;
		bot->rq = RD(l->l_qtop);
		bot->wq = l->l_qtop;
		bot->other = NULL;
		noenable(bot->rq);
		l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = (void *) bot;
		write_unlock_str(&mux_lock, flags);
		mi_copy_done(q, mp, 0);
		return (0);
	}
	case I_UNLINK:
	case I_PUNLINK:
	{
		struct linkblk *l;

		if (!mp->b_cont)
			mi_copy_done(q, mp, EINVAL);

		l = (struct linkblk *) mp->b_cont->b_rptr;

		write_lock_str(&mux_lock, flags);
		for (bot = mux_list; bot; bot = bot->next)
			if (bot->dev == l->l_index)
				break;
		if (!bot) {
			write_unlock_str(&mux_lock, flags);
			mi_copy_done(q, mp, EINVAL);
			return (0);
		}
		/* Note that the lower multiplex driver put and service procedures must be prepared
		   to be invoked event after the M_IOCACK for the I_UNLINK or I_PUNLINK ioctl has
		   been returned.  THis is because the setq(9) back to the Stream head is not
		   performed until after the acknowledgement has been received.  We set q->q_ptr to
		   a null multiplex structure to keep the lower Stream functioning until the setq(9)
		   is performed. */
		l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = &no_mux;
		if ((*bot->prev = bot->next)) {
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
		write_unlock_str(&mux_lock, flags);
		mi_copy_done(q, mp, 0);
		return (0);
	}
	case SL_SETLINK:
	{
		struct sl_mux_ppa *sm;

		/* This input-output control is used to set the global-PPA and CLEI associated with
		   a lower multiplex stream.  The argument is an sl_mux_ppa structure that contains
		   the multiplex id, the 32-bit PPA, and a CLEI string of up to 32 characters in
		   length. */
		mi_copyin(q, mp, NULL, sizeof(struct sl_mux_ppa));
		return (0);
	}
	case SL_GETLINK:
	{
		/* This input-output control is used to obtain the multiplex-id assocated with a
		   lower multiplex stream.  The argument is an sl_mux_ppa structure that contains a
		   32-bit PPA or CLEI string of up to 32 characters in length.  It returns the
		   multiplex id in the same structure. */
		mi_copyin(q, mp, NULL, sizeof(struct sl_mux_ppa));
		return (0);
	}
	default:
		if (mux->other && mux->other->rq) {
			if (bcanputnext(mux->other->rq, mp->b_band)) {
				putnext(mux->other->rq, mp);
				return (0);
			}
			return (-EBUSY);
		}
		break;
	}
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

static struct int
sl_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (struct copyresp *) mp->b_rptr;
	struct sl_mux_ppa sm;
	unsigned long flags;
	struct sl_mux *bot, *mux;
	mblk_t *dp, *bp;

	switch (cp->cp_cmd) {
	case SL_SETLINK:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			/* This input-output control is used to set the global-PPA and CLEI
			   associated with a lower multiplex stream.  The argument is an sl_mux_ppa 
			   structure that contains the multiplex id, the 32-bit PPA, and a CLEI
			   string of up to 32 characters in length. */
			bcopy(dp->b_rptr, &sm, sizeof(sm));
			/* The assigned global PPA must be non-zero. */
			if (sm.sm_ppa == 0) {
				mi_copy_done(q, mp, EINVAL);
				return (0);
			}
			/* The assigned CLEI must be non-null. */
			if (sm.sm_clei[0] == '\0') {
				mi_copy_done(q, mp, EINVAL);
				return (0);
			}
			write_lock_str(&mux_lock, flags);
			/* Find the stream with the associated MUXID. */
			for (bot = mux_list; bot; bot = bot->next)
				if (bot->dev == sm.sm_index)
					break;
			if (!bot) {
				write_unlock_str(&mux_lock, flags);
				mi_copy_done(q, mp, EINVAL);
				return (0);
			}
			/* The global PPA to assign must be unique. */
			for (mux = mux_list; mux; mux = mux->next)
				if (mux != bot && mux->ppa == sm.sm_ppa)
					break;
			if (mux) {
				write_unlock_str(&mux_lock, flags);
				mi_copy_done(q, mp, EINVAL);
				return (0);
			}
			/* The CLEI to assign must also be unique. */
			for (mux = mux_list; mux; mux = mux->next)
				if (mux != bot && strncmp(mux->clei, sm.sm_clei, 32) == 0)
					break;
			if (mux) {
				write_unlock_str(&mux_lock, flags);
				mi_copy_done(q, mp, EINVAL);
				return (0);
			}
			bot->ppa = sm.sm_ppa;
			bcopy(bot->clei, sm.sm_clei, 32);
			write_unlock_str(&mux_lock, flags);
			mi_copy_done(q, mp, 0);
			return (0);
		case -1:
			return (0);
		}
		break;
	case SL_GETLINK:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			/* This input-output control is used to obtain the multiplex-id assocated
			   with a lower multiplex stream.  The argument is an sl_mux_ppa structure
			   that contains a 32-bit PPA or CLEI string of up to 32 characters in
			   length.  It returns the multiplex id in the same structure. */
			bcopy(dp->b_rptr, &sm, sizeof(sm));
			if (sm.sm_ppa != 0) {
				/* If the supplied global PPA is non-zero, then search on PPA. */
				read_lock_str(&mux_lock, flags);
				for (bot = mux_list; bot; bot = bot->next)
					if (bot->ppa == sm.sm_ppa)
						break;
				if (!bot) {
					read_unlock_str(&mux_lock, flags);
					mi_copy_done(q, mp, EINVAL);
					return (0);
				}
				strncpy(sm.sm_clei, bot->clei, 32);
				sm.sm_index = bot->dev;
				read_unlock_str(&mux_lock, flags);
				if ((bp = mi_copyout_alloc(q, mp, NULL, sizeof(sm)))) {
					bcopy(&sm, bp->b_rptr, sizeof(sm));
					mi_copyout(q, mp);
				}
				return (0);
			} else if (sm.sm_clei[0] != '\0') {
				/* If the supplied CLEI is non-null, then search on CLEI. */
				read_lock_str(&mux_lock, flags);
				for (bot = mux_list; bot; bot = bot->next)
					if (strncmp(sm.sm_clei, bot->clei, 32) == 0)
						break;
				if (!bot) {
					read_unlock_str(&mux_lock, flags);
					mi_copy_done(q, mp, EINVAL);
					return (0);
				}
				sm.sm_ppa = bot->ppa;
				sm.sm_index = bot->dev;
				read_unlock_str(&mux_lock, flags);
				if ((bp = mi_copyout_alloc(q, mp, NULL, sizeof(sm)))) {
					bcopy(&sm, bp->b_rptr, sizeof(sm));
					mi_copyout(q, mp);
				}
				return (0);
			} else {
				/* If PPA is zero and CLEI is null, return the first item in the
				   list. */
				read_lock_str(&mux_lock, flags);
				bot = mux_list;
				if (!bot) {
					read_unlock_str(&mux_lock, flags);
					mi_copy_done(q, mp, EINVAL);
					return (0);
				}
				sm.sm_ppa = bot->ppa;
				strncpy(sm.sm_clei, bot->clei, 32);
				sm.sm_index = bot->dev;
				read_unlock_str(&mux_lock, flags);
				if ((bp = mi_copyout_alloc(q, mp, NULL, sizeof(sm)))) {
					bcopy(&sm, bp->b_rptr, sizeof(sm));
					mi_copyout(q, mp);
				}
				return (0);
			}
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			mi_copy_done(q, mp, 0);
			return (0);
		case -1:
			return (0);
		}
		break;
	default:
		if (mux->other && mux->other->rq) {
			if (bcanputnext(mux->other->rq, mp->b_band)) {
				putnext(mux->other->rq, mp);
				return (0);
			}
			return (-EBUSY);
		}
		break;
	}
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

static int
sl_w_msg(queue_t *q, mblk_t *mp)
{
	struct sl *mux = SL_PRIV(q);

	switch (DB_TYPE(mp)) {
	case M_IOCTL:
		return sl_w_ioctl(mux, q, mp);
	case M_IOCDATA:
		return sl_w_iocdata(mux, q, mp);
	}
	if (mux->other && mux->other->rq) {
		if (bcanputnext(mux->other->rq, mp->b_band)) {
			putnext(mux->other->rq, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);
	return (0);
}

static int
sl_r_msg(queue_t *q, mblk_t *mp)
{
	struct sl *mux = SL_PRIV(q);

	if (mux->other && mux->other->rq) {
		if (bcanputnext(mux->other->rq, mp->b_band)) {
			putnext(mux->other->rq, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);
	return (0);
}


static streamscall int
sl_uwput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
sl_uwsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sl_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall int
sl_ursrv(queue_t *q)
{
	struct sl *mux = SL_PRIV(q);

	if (mux->other && mux->other->rq)
		qenable(mux->other->rq);
}
static streamscall int
sl_lwsrv(queue_t *q)
{
	struct sl *mux = SL_PRIV(q);

	if (mux->other && mux->other->wq)
		qenable(mux->other->wq);
}
static streamscall int
sl_lrsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sl_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall int
sl_lrput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

static struct qinit sl_urinit = {
	.qi_srvp = sl_ursrv,
	.qi_qopen = sl_qopen,
	.qi_qclose = sl_qclose,
	.qi_minfo = &sl_minfo,
	.qi_mstat = &sl_urstat,
};
static struct qinit sl_uwinit = {
	.qi_putp = sl_uwput,
	.qi_srvp = sl_uwsrv,
	.qi_minfo = &sl_minfo,
	.qi_mstat = &sl_uwstat,
};
static struct qinit sl_lrinit = {
	.qi_putp = sl_lrput,
	.qi_srvp = sl_lrsrv,
	.qi_minfo = &sl_minfo,
	.qi_mstat = &sl_lrstat,
};
static struct qinit sl_lwinit = {
	.qi_srvp = sl_lwsrv,
	.qi_minfo = &sl_minfo,
	.qi_mstat = &sl_lwstat,
};

static struct streamtab sl_info = {
	.st_rdinit = &sl_urinit,
	.st_wrinit = &sl_uwinit,
	.st_muxrinit = &sl_lrinit,
	.st_muxwinit = &sl_lwinit,
};

static struct cdevsw sl_cdev = {
	.d_name = CONFIG_STREAMS_SL_MUX_NAME,
	.d_str = &sl_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static __init int
sl_mux_init(void)
{
	if ((err = register_strdev(&sl_cdev, major)) < 0)
		return (err);
	if (major == 0)
		major == err;
	return (0);
}

static __exit void
sl_mux_exit(void)
{
	unregister_strdev(&sl_cdev, major);
}

module_init(sl_mux_init);
module_exit(sl_mux_exit);
