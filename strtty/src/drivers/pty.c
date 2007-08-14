/*****************************************************************************

 @(#) $RCSfile: pty.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2007/08/14 03:04:30 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 03:04:30 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: pty.c,v $
 Revision 0.9.2.9  2007/08/14 03:04:30  brian
 - GPLv3 header update

 Revision 0.9.2.8  2007/07/14 01:36:52  brian
 - make license explicit, add documentation

 Revision 0.9.2.7  2007/03/28 13:44:46  brian
 - updates to syncrhonization, release notes and documentation

 Revision 0.9.2.6  2007/03/25 19:02:24  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.5  2007/03/25 06:00:56  brian
 - flush corrections

 Revision 0.9.2.4  2006/12/18 07:37:00  brian
 - resolve device numbering

 Revision 0.9.2.3  2006/10/12 09:37:40  brian
 - completed much of the strtty package

 Revision 0.9.2.2  2006/10/03 13:53:26  brian
 - changes to pass make check target
 - added some package config.h files
 - removed AUTOCONFIG_H from Makefile.am's
 - source code changes for compile
 - added missing manual pages
 - renamed conflicting manual pages
 - parameterized include Makefile.am
 - updated release notes

 Revision 0.9.2.1  2006/09/29 11:40:06  brian
 - new files for strtty package and manual pages

 *****************************************************************************/

#ident "@(#) $RCSfile: pty.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2007/08/14 03:04:30 $"

static char const ident[] =
    "$RCSfile: pty.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2007/08/14 03:04:30 $";

/*
 *  This is the start of a STREAMS pseudo-terminal (pty) driver for Linux.  It
 *  consists of two devices, a master device (ptm) and a slave device (pts).
 *  When a slave device is opened, the resulting minor device number indicates
 *  the minor device number of the master to open.  The two are then linked.
 *
 *  Note that there are no LFS or LIS defines here.  This is because the LIS
 *  Stream head cannot handle terminals anyway.  This package only works with
 *  LFS.
 */

#include <sys/os7/compat.h>

#include <linux/termios.h>
#include <sys/termios.h>
#include <sys/pty.h>

#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

#define PTY_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PTY_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define PTY_REVISION	"OpenSS7 $RCSfile: pty.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2007/08/14 03:04:30 $"
#define PTY_DEVICE	"SVR 4.2 STREAMS Pseudo-Terminal Driver (PTY)"
#define PTY_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define PTY_LICENSE	"GPL v2"
#define PTY_BANNER	PTY_DESCRIP	"\n" \
			PTY_COPYRIGHT	"\n" \
			PTY_REVISION	"\n" \
			PTY_DEVICE	"\n" \
			PTY_CONTACT
#define PTY_SPLASH	PTY_DEVICE	" - " \
			PTY_REVISION

#if defined(CONFIG_STREAMS_PTM_MODULE) || defined(CONFIG_STREAMS_PTS_MODULE)
#define CONFIG_STREAMS_PTY_MODULE 1
#endif

#ifdef LINUX
#ifdef CONFIG_STREAMS_PTY_MODULE
MODULE_AUTHOR(PTY_CONTACT);
MODULE_DESCRIPTION(PTY_DESCRIP);
MODULE_SUPPORTED_DEVICE(PTY_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(PTY_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-pty");
MODULE_ALIAS("streams-ptm");
MODULE_ALIAS("streams-pts");
#endif				/* MODULE_ALIAS */
#endif				/* CONFIG_STREAMS_PTY_MODULE */
#endif				/* LINUX */

#define PTM_DRV_ID	CONFIG_STREAMS_PTM_MODID
#define PTM_DRV_NAME	CONFIG_STREAMS_PTM_NAME
#define PTM_CMAJORS	CONFIG_STREAMS_PTM_NMAJORS
#define PTM_CMAJOR_0	CONFIG_STREAMS_PTM_MAJOR
#define PTM_UNITS	CONFIG_STREAMS_PTM_NMINORS

#define PTS_DRV_ID	CONFIG_STREAMS_PTS_MODID
#define PTS_DRV_NAME	CONFIG_STREAMS_PTS_NAME
#define PTS_CMAJORS	CONFIG_STREAMS_PTS_NMAJORS
#define PTS_CMAJOR_0	CONFIG_STREAMS_PTS_MAJOR
#define PTS_UNITS	CONFIG_STREAMS_PTS_NMINORS

#if (PTM_CMAJORS != PTS_CMAJORS) || (PTM_UNITS != PTS_UNITS)
#error The number of master and slave devices must be the same.
#endif

#define PTY_DRV_NAME	"pty"

#ifdef LINUX
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PTM_MODID));
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PTS_MODID));
MODULE_ALIAS("streams-driver-ptm");
MODULE_ALIAS("streams-driver-pts");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_PTM_MAJOR));
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_PTS_MAJOR));
MODULE_ALIAS("/dev/streams/ptm");
MODULE_ALIAS("/dev/streams/ptm/*");
MODULE_ALIAS("/dev/streams/clone/ptm");
MODULE_ALIAS("/dev/streams/ptm/ptmx");
MODULE_ALIAS("/dev/streams/pts");
MODULE_ALIAS("/dev/streams/pts/*");
MODULE_ALIAS("/dev/streams/clone/pts");
MODULE_ALIAS("char-major-" __stringify(PTM_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(PTM_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(PTM_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/ptm");
MODULE_ALIAS("char-major-" __stringify(PTS_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(PTS_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(PTS_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/pts");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ==========================================================================
 */

/* The master device. */

STATIC struct module_info ptm_minfo = {
	.mi_idnum = PTM_DRV_ID,		/* Module ID number */
	.mi_idname = PTM_DRV_NAME,	/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

STATIC struct module_stat ptm_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

STATIC streamscall int ptm_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int ptm_qclose(queue_t *, int, cred_t *);

STATIC streamscall int ptm_rput(queue_t *, mblk_t *);
STATIC streamscall int ptm_wput(queue_t *, mblk_t *);
STATIC streamscall int ptm_wsrv(queue_t *);
STATIC streamscall int ptm_rsrv(queue_t *);

STATIC struct qinit ptm_rinit = {
	.qi_putp = ptm_rput,		/* Put procedure (message from below) */
	.qi_srvp = ptm_rsrv,		/* Read service procedure. */
	.qi_qopen = ptm_qopen,		/* Each open */
	.qi_qclose = ptm_qclose,	/* Last close */
	.qi_minfo = &ptm_minfo,		/* Module information */
	.qi_mstat = &ptm_mstat,		/* Module statistics */
};

STATIC struct qinit ptm_winit = {
	.qi_putp = ptm_wput,		/* Put procedure (message from above) */
	.qi_srvp = ptm_wsrv,		/* Write service procedure. */
	.qi_minfo = &ptm_minfo,		/* Module information */
	.qi_mstat = &ptm_mstat,		/* Module statistics */
};

MODULE_STATIC struct streamtab ptm_info = {
	.st_rdinit = &ptm_rinit,	/* Upper read queue */
	.st_wrinit = &ptm_winit,	/* Upper write queue */
};

/* The slave device. */

STATIC struct module_info pts_minfo = {
	.mi_idnum = PTS_DRV_ID,		/* Module ID number */
	.mi_idname = PTS_DRV_NAME,	/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

STATIC struct module_stat pts_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

STATIC streamscall int pts_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int pts_qclose(queue_t *, int, cred_t *);

STATIC streamscall int pts_rput(queue_t *, mblk_t *);
STATIC streamscall int pts_wput(queue_t *, mblk_t *);
STATIC streamscall int pts_wsrv(queue_t *);
STATIC streamscall int pts_rsrv(queue_t *);

STATIC struct qinit pts_rinit = {
	.qi_putp = pts_rput,		/* Put procedure (message from below). */
	.qi_srvp = pts_rsrv,		/* Read service procedure. */
	.qi_qopen = pts_qopen,		/* Each open */
	.qi_qclose = pts_qclose,	/* Last close */
	.qi_minfo = &pts_minfo,		/* Module information */
	.qi_mstat = &pts_mstat,		/* Module statistics */
};

STATIC struct qinit pts_winit = {
	.qi_putp = pts_wput,		/* Put procedure (message from above) */
	.qi_srvp = pts_wsrv,		/* Write service procedure. */
	.qi_minfo = &pts_minfo,		/* Module information */
	.qi_mstat = &pts_mstat,		/* Module statistics */
};

MODULE_STATIC struct streamtab pts_info = {
	.st_rdinit = &pts_rinit,	/* Upper read queue */
	.st_wrinit = &pts_winit,	/* Upper write queue */
};

/*
 * Primary data structures.
 */

#ifdef __LP64__
#   undef   WITH_32BIT_CONVERSION
#   define  WITH_32BIT_CONVERSION 1
#endif

/* private structure */
struct pty;				/* common master/slave structure */
struct ptc;				/* control structure */

#define PTY_TYPE_ABSENT	0	/* no stream present */
#define PTY_TYPE_SLAVE	1	/* unlocked slave */
#define PTY_TYPE_MASTER	2	/* master (always locked) */
#define PTY_TYPE_LOCKED	3	/* locked slave */

struct pty {
	queue_t *rq;
	queue_t *wq;
	major_t major;
	int type;
	int oflag;
	cred_t creds;
};

struct ptc {
	rwlock_t lock;
	struct pty ptm;
	struct pty pts;
	struct ptc *next;
	struct ptc **prev;
	minor_t minor;
};

STATIC int ptm_majors[PTM_CMAJORS] = { PTM_CMAJOR_0, };
STATIC int pts_majors[PTS_CMAJORS] = { PTS_CMAJOR_0, };

STATIC kmem_cachep_t ptc_priv_cachep = NULL;

STATIC rwlock_t pty_lock = RW_LOCK_UNLOCKED;

/**
 * ptc_alloc_priv - allocate a private structure for the open routine
 * @q: read queue of opening Stream
 * @ptmp: place in list
 * @mindex: major device number index
 * @devp: pointer to device number
 * @oflag: open flags
 * @crp: credentials pointer
 */
STATIC struct ptm *
ptc_alloc_priv(queue_t *q, struct ptc **ptcp, int mindex, dev_t *devp, int oflag, cred_t *crp)
{
	struct ptc *ptc;

	if (likely((ptc = kmem_cache_alloc(ptc_priv_cachep, GFP_ATOMIC)) != NULL)) {
		bzero(ptc, sizeof(*ptc));

		rwlock_init(&ptc->lock);

		ptc->ptm.rq = q;
		ptc->ptm.wq = WR(q);
		ptc->ptm.major = getmajor(*devp);
		ptc->ptm.type = PTY_TYPE_MASTER;
		ptc->ptm.oflag = oflag;
		ptc->ptm.creds = *crp;

		ptc->pts.major = pts_majors[mindex];
		ptc->pts.type = PTY_TYPE_LOCKED;

		/* protected by pty_lock */
		if ((ptc->next = *ptcp))
			ptc->next->prev = &ptc->next;
		ptc->prev = ptcp;

		ptc->minor = getminor(*devp);
	}
	return ((struct ptm *) &ptc->ptm);
}

#if defined CONFIG_STREAMS_NOIRQ || defined _TEST

#define spin_lock_str(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)
#define write_lock_str(__lkp, __flags) \
	do { (void)__flags; write_lock_bh(__lkp); } while (0)
#define write_unlock_str(__lkp, __flags) \
	do { (void)__flags; write_unlock_bh(__lkp); } while (0)
#define read_lock_str(__lkp, __flags) \
	do { (void)__flags; read_lock_bh(__lkp); } while (0)
#define read_unlock_str(__lkp, __flags) \
	do { (void)__flags; read_unlock_bh(__lkp); } while (0)
#define local_save_str(__flags) \
	do { (void)__flags; local_bh_disable(); } while (0)
#define local_restore_str(__flags) \
	do { (void)__flags; local_bh_enable(); } while (0)

#else

#define spin_lock_str(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)
#define write_lock_str(__lkp, __flags) \
	write_lock_irqsave(__lkp, __flags)
#define write_unlock_str(__lkp, __flags) \
	write_unlock_irqrestore(__lkp, __flags)
#define read_lock_str(__lkp, __flags) \
	read_lock_irqsave(__lkp, __flags)
#define read_unlock_str(__lkp, __flags) \
	read_unlock_irqrestore(__lkp, __flags)
#define local_save_str(__flags) \
	local_irq_save(__flags)
#define local_restore_str(__flags) \
	local_irq_restore(__flags)

#endif

/**
 * ptc_free_priv - deallocate a private structure for the close routine
 * @q: read queue of closing Stream
 */
STATIC noinline void
ptc_free_priv(struct ptc *c, struct pty *closing, struct pty *other)
{
	unsigned long flags;
	mblk_t *mp;
	int type;

	/* hangup the other end, if necessary */
	mp = allocb(0, BPRI_WAITOK);

	write_lock_str(&pty_lock, flags);
	{
		write_lock(&c->lock);

		type = closing->type;

		closing->rq = NULL;
		closing->wq = NULL;
		closing->type = PTY_TYPE_ABSENT;

		if (other->rq) {
			if (type == PTY_TYPE_MASTER)
				/* Masters send M_HANGUP to slaves. */
				mp->b_datap->db_type = M_HANGUP;
			else
				/* Slaves send zero-length data message to master. */
				mp->b_flag |= MSGDELIM;
			putnext(other->rq, mp);
			write_unlock(&c->lock);
		} else {
			freemsg(mp);

			/* remove from master list */
			if ((*c->prev = c->next))
				c->next->prev = c->prev;
			c->next = NULL;

			write_unlock(&c->lock);

			/* free it */
			kmem_cache_free(ptc_priv_cachep, c);
		}
	}
	write_lock_str(&pty_lock, flags);
}

/*
 *  PUT and SERVICE Procedures
 */

/*
 * SVR 4.2 STREAMS Programmer's Guide says everybody's got to have a put
 * procedure.
 */
STATIC streamscall __unlikely int
ptm_rput(queue_t *q, mblk_t *mp)
{
	if (!putq(q, mp))
		freemsg(mp);
	return (0);
}

STATIC streamscall __unlikely int
pts_rput(queue_t *q, mblk_t *mp)
{
	if (!putq(q, mp))
		freemsg(mp);
	return (0);
}

/*
 * The driver never places message on the read queue, therefore, the read
 * service procedure is only invoked when backenabled from upstream.  Back
 * enabling is due to flow control that causes a message to be put (back) to
 * the other Stream's write queue so the driver needs to explicitly enable the
 * write queue now.
 */
STATIC void
pty_rsrv(queue_t *q, struct ptc *c, queue_t **qp)
{
	queue_t *wq;

	read_lock(&c->lock);
	{
		if ((wq = *qp) != NULL) {
			enableok(wq);
			qenable(wq);
		}
	}
	read_unlock(&c->lock);
}

STATIC streamscall int
ptm_rsrv(queue_t *q)
{
	struct ptc *c = q->q_ptr;

	assert(c);
	pty_rsrv(q, c, &c->pts.wq);
	return (0);
}

STATIC streamscall int
pts_rsrv(queue_t *q)
{
	struct ptc *c = q->q_ptr;

	assert(c);
	pty_rsrv(q, c, &c->ptm.wq);
	return (0);
}

/*
 *  The driver only puts messages on its write queue when they fail flow
 *  control on the queue beyond the other read queue.  When flow control
 *  subsides, the read service procedure on the other Stream will explicitly
 *  enable us.
 */
STATIC void
pty_wsrv(queue_t *q, struct ptc *c, queue_t **qp)
{
	queue_t *rq;
	mblk_t *mp;

	read_lock(&c->lock);
	{
		/* 
		 * Neither the master nor the slave are allowed to close while
		 * the service procedure is running, otherwise, rq could be
		 * made invalid.  All we need is a spin here because interrupt
		 * service routines cannot close streams.  Therefore, locks
		 * are only really taken for SMP.
		 */
		if ((rq = *qp) != NULL) {
			while ((mp = getq(q))) {
				if (mp->b_datap->db_type >= QPCTL || bcanputnext(rq, mp->b_band)) {
					putnext(rq, mp);
					continue;
				}
				/* putbq() can't fail after getq() */
				putbq(q, mp);
				break;
			}
		} else
			flushq(q, FLUSHALL);
	}
	read_unlock(&c->lock);
}

STATIC streamscall int
ptm_wsrv(queue_t *q)
{
	struct ptc *c = q->q_ptr;

	pty_wsrv(q, c, &c->pts.rq);
	return (0);
}

STATIC streamscall int
pts_wsrv(queue_t *q)
{
	struct ptc *c = q->q_ptr;

	pty_wsrv(q, c, &c->ptm.rq);
	return (0);
}

STATIC streams_noinline __unlikely void
pty_m_flush(queue_t *wq, mblk_t *mp, queue_t *rq)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(wq, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(wq, FLUSHDATA);
	}
	if (rq) {
		/* switch sense of flush flags and pass to other end */
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
		/* reflect it back up our own Stream for proper flushing even after a hangup at the 
		   other end */
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(RD(wq), mp->b_rptr[1], FLUSHDATA);
			else
				flushq(RD(wq), FLUSHDATA);
			mp->b_rptr[0] &= ~FLUSHW;
			putnext(RD(wq), mp);
		} else {
			freemsg(mp);
		}
	}
}

STATIC streams_noinline __unlikely void
pty_m_hangup(queue_t *q, mblk_t *mp)
{
	/* transform message into a hangup */
	mp->b_datap->db_type = M_HANGUP;
	mp->b_band = 0;
	mp->b_wptr = mp->b_rptr;
	if (mp->b_cont) {
		freemsg(mp->b_cont);
		mp->b_cont = NULL;
	}
	qreply(q, mp);
}

STATIC streams_fastcall __hot_put void
pty_m_other(queue_t *wq, mblk_t *mp, queue_t *rq)
{
	if (unlikely(rq == NULL)) {
		pty_m_hangup(wq, mp);
		return;
	}
	if (unlikely(wq->q_first != NULL))
		goto do_putq;
	if (unlikely(wq->q_flag & QSVCBUSY))
		goto do_putq;
	if (likely(mp->b_datap->db_type < QPCTL))
		if (unlikely(!bcanputnext(rq, mp->b_band)))
			goto do_putq;
	putnext(rq, mp);
	return;

      do_putq:
	if (!putq(wq, mp))
		freemsg(mp);
	return;
}

STATIC streams_fastcall __hot_put void
pty_wput(queue_t *q, mblk_t *mp, struct ptc *c, queue_t **qp)
{
	read_lock(&c->lock);
	{
		int type = mp->b_datap->db_type;

		if (likely(type != M_FLUSH))
			pty_m_other(q, mp, *qp);
		else
			pty_m_flush(q, mp, *qp);
	}
	read_unlock(&c->lock);
}

STATIC streamscall __hot_put int
ptm_wput(queue_t *q, mblk_t *mp)
{
	struct ptc *c = q->q_ptr;
	int type;

	if (likely((type = mp->b_datap->db_type) != M_IOCTL && type != M_IOCDATA)) {
		pty_wput(q, mp, c, &c->pts.rq);
		return (0);
	} else {
		int rval = 0, error = 0, count = 0;
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
		unsigned long flags;

		switch (type) {
		case M_IOCTL:
			switch (ioc->iocblk.ioc_cmd) {
			default:
				/* if its not for us */
				pty_wput(q, mp, c, &c->pts.rq);
				break;
			case ISPTM:
				/* Determines whether the file descriptor is of an open master
				   device. UnixWare returns the device number on success.  OSF
				   documents the return of the device number on success.  Solaris
				   returns zero on success and non-zero (well, -1) on failure.  The 
				   library ptsname() routine can easily do an fstat(2) on the
				   Stream to retrieve its device number (appropriately converted).
				   The only portable method would be if the return value is not
				   minus one (-1) if it is zero, do an fstat(2) and get the device
				   number, otherwise the return value can be used as the device
				   number. */
				rval = makedevice(c->ptm.major, c->minor);
				goto ack_it;
			case UNLKPT:
				/* Unlocks the slave. */

				write_lock_str(&c->lock, flags);
				if (c->pts.rq == NULL || c->pts.type == PTY_TYPE_ABSENT) {
					write_unlock_str(&c->lock, flags);
					rval = -1;
					error = EINVAL;
					goto nak_it;
				}
				/* auto grant */
				c->pts.creds = *ioc->iocblk.ioc_cr;
				c->pts.type = PTY_TYPE_SLAVE;
				/* ok to unlock it if it is already unlocked, this will just do a
				   grant */
				write_unlock_str(&c->lock, flags);
				goto ack_it;

#ifdef PTSSTTY
			case PTSSTTY:
				/* Grants access, sets controlling terminal. */
				/* Controlling terminal will be set when ptem is pushed. */
				/* fall through */
#endif				/* PTSSTTY */

			case ZONEPT:
				/* Grants access. */
				write_lock_str(&c->lock, flags);
				c->pts.creds = *ioc->iocblk.ioc_cr;	/* grant */
				write_unlock_str(&c->lock, flags);
				goto ack_it;
#ifdef TIOCGPTN
			case TIOCGPTN:	/* passes unsigned int * *//* Get Pty Number (of pty-mux device) Linux defines this. */
				if (ioc->iocblk.ioc_count == TRANSPARENT) {
					/* need to do copyout */
					ioc->copyreq.cq_addr = *(caddr_t *) mp->b_cont->b_rptr;
					ioc->copyreq.cq_size = sizeof(unsigned int);
					ioc->copyreq.cq_flag = 0;
					ioc->copyreq.cq_private = NULL;
					mp->b_datap->db_type = M_COPYOUT;
					qreply(q, mp);
					break;
				}
				*(unsigned int *) mp->b_cont->b_rptr = c->minor;

				/* implicit copyout */
				count = sizeof(unsigned int);
				rval = 0;
				error = 0;
				goto ack_it;
#endif				/* TIOCGPTN */
#ifdef TIOCSPTLCK
			case TIOCSPTLCK:	/* passes int * *//* Lock/unlock Pty: Linux deinfes this. */
			{
				if (ioc->iocblk.ioc_count == TRANSPARENT) {
					/* need to do copyin */
					ioc->copyreq.cq_addr = *(caddr_t *) mp->b_cont->b_rptr;
					ioc->copyreq.cq_size = sizeof(int);
					ioc->copyreq.cq_flag = 0;
					ioc->copyreq.cq_private = NULL;
					mp->b_datap->db_type = M_COPYIN;
					qreply(q, mp);
					break;
				}
				if (ioc->iocblk.ioc_count < sizeof(int)) {
					rval = -1;
					error = EINVAL;
					count = 0;
					goto nak_it;
				}
				write_lock_str(&c->lock, flags);
				if (c->pts.rq == NULL || c->pts.type == PTY_TYPE_ABSENT) {
					write_unlock_str(&c->lock, flags);
					rval = -1;
					error = EINVAL;
					count = 0;
					goto nak_it;
				}
				/* autogrant */
				c->pts.creds = *ioc->iocblk.ioc_cr;
				/* always grant regardless of locking or unlocking */
				/* ok to lock locked or unlock unlocked */
				if (*(int *) mp->b_cont->b_rptr)
					c->pts.type = PTY_TYPE_LOCKED;
				else
					c->pts.type = PTY_TYPE_SLAVE;
				write_unlock_str(&c->lock, flags);
				rval = 0;
				error = 0;
				count = 0;
				goto ack_it;
			}
#endif				/* TIOCSPTLCK */
			}
			break;
		case M_IOCDATA:
			switch (ioc->copyresp.cp_cmd) {
#ifdef TIOCGPTN
			case TIOCGPTN:
				if (ioc->copyresp.cp_rval == 0) {
					/* copyout successful */
					mp->b_datap->db_type = M_IOCACK;
					ioc->iocblk.ioc_rval = 0;
					ioc->iocblk.ioc_error = 0;
					qreply(q, mp);
					break;
				}
				break;
#endif				/* TIOCGPTN */
#ifdef TIOCSPTLCK
			case TIOCSPTLCK:
				if (ioc->copyresp.cp_rval == 0) {
					/* copyin successful */
					write_lock_str(&c->lock, flags);
					if (c->pts.rq == NULL || c->pts.type == PTY_TYPE_ABSENT) {
						write_unlock_str(&c->lock, flags);
						rval = -1;
						error = EINVAL;
						goto nak_it;
					}
					/* autogrant */
					c->pts.creds = *ioc->iocblk.ioc_cr;
					/* always grant regardless of locking or unlocking */
					/* ok to lock locked or unlock unlocked */
					if (*(int *) mp->b_cont->b_rptr)
						c->pts.type = PTY_TYPE_LOCKED;
					else
						c->pts.type = PTY_TYPE_SLAVE;
					write_unlock_str(&c->lock, flags);
					count = TRANSPARENT;
					rval = 0;
					error = 0;
					goto ack_it;
				}
				break;
#endif				/* TIOCSPTLCK */
			}
			break;
		      ack_it:
			mp->b_datap->db_type = M_IOCACK;
			goto finish;
		      nak_it:
			mp->b_datap->db_type = M_IOCNAK;
			goto finish;
		      finish:
			ioc->iocblk.ioc_count = count;
			ioc->iocblk.ioc_rval = rval;
			ioc->iocblk.ioc_error = error;
			qreply(q, mp);
			return (0);
		}
	}
	return (0);
}

STATIC streamscall __hot_put int
pts_wput(queue_t *q, mblk_t *mp)
{
	struct ptc *c = q->q_ptr;

	pty_wput(q, mp, c, &c->ptm.rq);
	return (0);
}

/*
 *  OPEN and CLOSE Routines
 */

STATIC struct ptc *ptc_opens = NULL;

/**
 * ptm_open - PTM driver STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 */
STATIC streamscall int
ptm_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct ptc **ptcp = &ptc_opens;
	unsigned long flags;

	if (q->q_ptr != NULL)
		/* Only one open is allowed on a master device. */
		return (ENXIO);	/* already open */

	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", PTM_DRV_NAME));
		return (EIO);
	}
	/* Linux Fast-STREAMS always passes internal major device number (module id).  Note also,
	   however, that strconf-sh attempts to allocate module ids that are identical to the base
	   major device number anyway. */
	if (cmajor != PTM_DRV_ID)
		return (ENXIO);

	/* we can only be clone opened */
	if (sflag != CLONEOPEN || cminor != 0)
		return (ENXIO);

	/* A master device is available only if it and its corresponding slave device are not
	   already open.  When the master device is opened, the corresponding slave device is
	   automatically locked out.  Only one open is allowed on a master device. */
	write_lock_str(&pty_lock, flags);
	for (; *ptcp; ptcp = &(*ptcp)->next) {
		if (cmajor != (*ptcp)->ptm.major)
			break;
		if (cmajor == (*ptcp)->ptm.major) {
			if (cminor < (*ptcp)->minor)
				break;
			if (cminor == (*ptcp)->minor) {
				if (++cminor >= PTM_UNITS) {
					if (++mindex >= PTM_CMAJORS
					    || !(cmajor = ptm_majors[mindex]))
						break;
					cminor = 0;	/* start next major */
				}
				continue;
			}
		}
	}
	if (mindex >= PTM_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", PTM_DRV_NAME));
		write_unlock_str(&pty_lock, flags);
		return (ENXIO);
	}
	_printd(("%s: opened character device %d:%d\n", PTM_DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!ptc_alloc_priv(q, ptcp, mindex, devp, oflag, crp)) {
		ptrace(("%s: ERROR: No memory\n", PTM_DRV_NAME));
		write_unlock_str(&pty_lock, flags);
		return (ENOMEM);
	}
	write_unlock_str(&pty_lock, flags);
	qprocson(q);
	return (0);
}

/**
 * ptm_qclose - PTM driver STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 *
 * When the master device is closed, an M_HANGUP message is sent to the slave
 * device, which renders the device unusable.  The process on the slave side
 * gets the errno ENXIO when attempting to write on that streams, but it can
 * read any data remaining on the stream head read queue.  When all the data
 * have been read, read returns 0, indicating that the stream can no longer be
 * used (terminal end-of-file).
 */
STATIC streamscall int
ptm_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct ptc *c = q->q_ptr;

	printd(("%s: closing character device %d:%d\n", PTM_DRV_NAME, c->ptm.major, c->minor));
	/* make sure procedures are off */
	qprocsoff(q);
	ptc_free_priv(c, &c->ptm, &c->pts);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/**
 * pts_open - PTS driver STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 */
STATIC streamscall int
pts_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	unsigned long flags;
	struct ptc *c;

	if ((c = q->q_ptr) != NULL) {
		/* already open, check permissions */
		if (crp->cr_uid == 0)
			goto access_ok;
		if (crp->cr_uid == 5 && !(oflag & FREAD))
			goto access_ok;
		if (crp->cr_uid == c->pts.creds.cr_uid)
			goto access_ok;
		return (EPERM);
	}

	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", PTS_DRV_NAME));
		return (EIO);
	}
	/* Linux Fast-STREAMS always passes internal major device number (module id).  Note also,
	   however, that strconf-sh attempts to allocate module ids that are identical to the base
	   major device number anyway. */
	if (cmajor != PTS_DRV_ID)
		return (ENXIO);

	/* Cannot clone open the slave. */
	if (sflag == CLONEOPEN)
		return (ENXIO);

	read_lock(&pty_lock);
	{
		/* Search for the master. */
		for (c = ptc_opens; c; c = c->next)
			if (c->pts.major == cmajor)
				if (c->minor == cminor)
					break;
		if (c == NULL) {
			/* no master */
			read_unlock(&pty_lock);
			return (ENXIO);
		}
		write_lock_str(&c->lock, flags);
		{
			if (c->pts.type == PTY_TYPE_LOCKED) {
				/* OSF says that if UNLKPT is not used in conjunction with the
				   master, the opening of the corresponding slave will fail with
				   EPERM. */
				write_unlock_str(&c->lock, flags);
				read_unlock(&pty_lock);
				/* But, conversely, POSIX open(2) says EAGAIN. */
				return (EAGAIN);
			}

			/* its unlocked, but check your permissions */
			if (crp->cr_uid == 0)
				goto good_open;
			if (crp->cr_uid == 5 && !(oflag & FREAD))
				goto good_open;
			if (crp->cr_uid == c->pts.creds.cr_uid)
				goto good_open;
		}
		write_unlock_str(&c->lock, flags);
	}
	read_unlock(&pty_lock);
	return (EPERM);

      good_open:
	{
		q->q_ptr = WR(q)->q_ptr = c;

		c->pts.rq = q;
		c->pts.wq = WR(q);
		c->pts.type = PTY_TYPE_SLAVE;
		c->pts.oflag = oflag;
		c->pts.creds = *crp;
	}
	write_unlock_str(&c->lock, flags);
	read_unlock(&pty_lock);

	_printd(("%s: opened character device %d:%d\n", PTS_DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);

	qprocson(q);
      access_ok:
	return (0);
}

/**
 * pts_qclose - PTS driver STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 *
 * On the last close of the slave device, a zero-length message is sent to the
 * master side.  When the master side issues a read and 0 is returned, the
 * user decides whether to issue a close, which dismantles the
 * pseudo-terminal, or not to close the master device so that the pseudo-tty
 * subsystem will be available for another user to open the slave device.
 */
STATIC streamscall int
pts_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct ptc *c = q->q_ptr;

	printd(("%s: closing character device %d:%d\n", PTS_DRV_NAME, c->pts.major, c->minor));
	/* make sure procedures are off */
	qprocsoff(q);
	ptc_free_priv(c, &c->pts, &c->ptm);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/*
 *  Private struct reference counting, allocation, deallocation and cache
 */
STATIC __unlikely int
pty_term_caches(void)
{
	int err = 0;

	if (ptc_priv_cachep != NULL) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(ptc_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ptc_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else {
			_printd(("%s: destroyed ptc_priv_cachep\n", PTM_DRV_NAME));
			ptc_priv_cachep = NULL;
		}
#else
		kmem_cache_destroy(ptc_priv_cachep);
#endif
	}
	return (err);
}

STATIC __unlikely int
pty_init_caches(void)
{
	if (ptc_priv_cachep == NULL) {
		ptc_priv_cachep =
		    kmem_cache_create("ptc_priv_cachep", sizeof(struct ptc), 0,
				      SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (ptc_priv_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocation ptc_priv_cachep", __FUNCTION__);
			pty_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialized ptc driver private structure cache\n", PTM_DRV_NAME));
	}
	return (0);
}

/*
 *  Registration and initialization
 */
#ifdef LINUX
/*
 *  Linux registration
 */
unsigned short ptm_modid = PTM_DRV_ID;
unsigned short pts_modid = PTS_DRV_ID;

#ifdef LINUX
#ifdef CONFIG_STREAMS_PTY_MODULE
#ifndef module_param
MODULE_PARM(ptm_modid, "h");
MODULE_PARM(pts_modid, "h");
#else
module_param(ptm_modid, ushort, 0444);
module_param(pts_modid, ushort, 0444);
#endif
MODULE_PARM_DESC(ptm_modid, "Module ID for the PTM driver. (0 for allocation.)");
MODULE_PARM_DESC(pts_modid, "Module ID for the PTS driver. (0 for allocation.)");

major_t ptm_major = PTM_CMAJOR_0;
major_t pts_major = PTS_CMAJOR_0;

#ifndef module_param
MODULE_PARM(ptm_major, "h");
MODULE_PARM(pts_major, "h");
#else
module_param(ptm_major, uint, 0444);
module_param(pts_major, uint, 0444);
#endif
MODULE_PARM_DESC(ptm_major, "Device number for the PTM driver. (0 for allocation.)");
MODULE_PARM_DESC(pts_major, "Device number for the PTS driver. (0 for allocation.)");
#endif
#endif				/* CONFIG_STREAMS_PTY_MODULE */
#endif				/* LINUX */

/*
 *  Linux Fast-STREAMS Registration
 */
STATIC struct cdevsw ptm_cdev = {
	.d_name = PTM_DRV_NAME,
	.d_str = &ptm_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC struct devnode ptmx = {
	.n_name = "ptmx",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC __unlikely int
ptm_register_strdev(major_t major)
{
	int err;

#if 0
	if ((err = register_strdev(&ptm_cdev, major)) < 0)
		return (err);
#else
	/* Because we are stealing Linux's device number for pseudo-terminals, we do not want to
	   publish this device to the external file system nor create device nodes.  Leave it in
	   the specfs(5) so that it can be accessed with /dev/streams/ptm/ptmx. */
	if ((err = register_strdrv(&ptm_cdev)) < 0)
		return (err);
#endif
	if ((err = register_strnod(&ptm_cdev, &ptmx, 0)) < 0)
		pswerr(("Could not register ptmx node, err = %d\n", err));
	return (0);
}

STATIC __unlikely int
ptm_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strnod(&ptm_cdev, 0)) < 0)
		pswerr(("Could not unregister ptmx node, err = %d\n", err));
#if 0
	if ((err = unregister_strdev(&ptm_cdev, major)) < 0)
		return (err);
#else
	if ((err = unregister_strdrv(&ptm_cdev)) < 0)
		return (err);
#endif
	return (0);
}

STATIC struct cdevsw pts_cdev = {
	.d_name = PTS_DRV_NAME,
	.d_str = &pts_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC __unlikely int
pts_register_strdev(major_t major)
{
	int err;

#if 0
	if ((err = register_strdev(&pts_cdev, major)) < 0)
		return (err);
#else
	/* Because we are stealing Linux's device number for pseudo-terminals, we do not want to
	   publish this device to the external file system nor create device nodes.  Leave it in
	   the specfs(5) so that it can be accessed with /dev/streams/pts/N. */
	if ((err = register_strdrv(&pts_cdev)) < 0)
		return (err);
#endif
	return (0);
}

STATIC __unlikely int
pts_unregister_strdev(major_t major)
{
	int err;

#if 0
	if ((err = unregister_strdev(&pts_cdev, major)) < 0)
		return (err);
#else
	if ((err = unregister_strdrv(&pts_cdev)) < 0)
		return (err);
#endif
	return (0);
}

MODULE_STATIC void __exit
ptyterminate(void)
{
	int err, mindex;

	/* Unregister slave device (normaly only one major). */
	for (mindex = PTS_CMAJORS - 1; mindex >= 0; mindex--) {
		if (pts_majors[mindex]) {
			if ((err = pts_unregister_strdev(pts_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", PTS_DRV_NAME,
					pts_majors[mindex]);
			if (mindex)
				pts_majors[mindex] = 0;
		}
	}
	/* Unregister master device (normaly only one major). */
	for (mindex = PTM_CMAJORS - 1; mindex >= 0; mindex--) {
		if (ptm_majors[mindex]) {
			if ((err = ptm_unregister_strdev(ptm_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", PTM_DRV_NAME,
					ptm_majors[mindex]);
			if (mindex)
				ptm_majors[mindex] = 0;
		}
	}
	if ((err = pty_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", PTY_DRV_NAME);
	return;
}

MODULE_STATIC int __init
ptyinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, PTY_BANNER);
	if ((err = pty_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", PTY_DRV_NAME, err);
		ptyterminate();
		return (err);
	}
	/* Register master device (normaly only one major). */
	for (mindex = 0; mindex < PTM_CMAJORS; mindex++) {
		if ((err = ptm_register_strdev(ptm_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", PTM_DRV_NAME,
					ptm_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					PTM_DRV_NAME, err);
				ptyterminate();
				return (err);
			}
		}
		if (ptm_majors[mindex] == 0)
			ptm_majors[mindex] = err;
		if (ptm_major == 0)
			ptm_major = ptm_majors[0];
	}
	/* Register slave device (normaly only one major). */
	for (mindex = 0; mindex < PTS_CMAJORS; mindex++) {
		if ((err = pts_register_strdev(pts_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", PTS_DRV_NAME,
					pts_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					PTS_DRV_NAME, err);
				ptyterminate();
				return (err);
			}
		}
		if (pts_majors[mindex] == 0)
			pts_majors[mindex] = err;
		if (pts_major == 0)
			pts_major = pts_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(ptyinit);
module_exit(ptyterminate);
