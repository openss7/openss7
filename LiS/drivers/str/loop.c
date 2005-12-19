/*****************************************************************************

 @(#) $RCSfile: loop.c,v $ $Name:  $($Revision: 1.1.1.3.4.6 $) $Date: 2005/12/18 06:37:53 $

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

 Last Modified $Date: 2005/12/18 06:37:53 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: loop.c,v $ $Name:  $($Revision: 1.1.1.3.4.6 $) $Date: 2005/12/18 06:37:53 $"

/*                               -*- Mode: C -*- 
 * loop.c --- Streams loopback driver, as of Sun manual 
 * Author          : Graham Wheeler
 * Created On      : Sat Oct  7 05:01:31 1995
 * Last Modified By: David Grothe
 * RCS Id          : $Id: loop.c,v 1.1.1.3.4.6 2005/12/18 06:37:53 brian Exp $
 * Purpose         : provide loopback streams driver
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros,
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */

/*  -------------------------------------------------------------------  */

#include <sys/LiS/module.h>	/* should be first */

#include <sys/LiS/config.h>

#include <sys/stream.h>
#include <sys/stropts.h>
#include <linux/stat.h>		/* for S_IFCHR */
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */

/*			  Module definition structs                      */

/* Module info for the loopback driver
 */
static struct module_info loop_minfo = {
	0,				/* id */
	"loop",				/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* high water mark */
	4096L				/* low water mark */
};

/* These are the entry points to the driver: open, close, write side put and
 * service procedures and read side service procedure.
 */
static int _RP loop_open(queue_t *, dev_t *, int, int, cred_t *);
static int _RP loop_close(queue_t *, int, cred_t *);
static int _RP loop_wput(queue_t *, mblk_t *);
static int _RP loop_wsrv(queue_t *);
static int _RP loop_rsrv(queue_t *);

/* qinit structures (rd and wr side) 
 */
static struct qinit loop_rinit = {
	NULL,				/* put */
	loop_rsrv,			/* service */
	loop_open,			/* open */
	loop_close,			/* close */
	NULL,				/* admin */
	&loop_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit loop_winit = {
	loop_wput,			/* put */
	loop_wsrv,			/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&loop_minfo,			/* info */
	NULL				/* stat */
};

/* streamtab for the loopback driver.
 */
struct streamtab sloop_info = {
	&loop_rinit,			/* read queue */
	&loop_winit,			/* write queue */
	NULL,				/* mux read queue */
	NULL				/* mux write queue */
};

/*  -------------------------------------------------------------------  */

/*			    Module implementation                        */

#include <sys/LiS/loop.h>

/*  -------------------------------------------------------------------  */

#define NLOOP 21		/* number of clone devs supported */

struct loop {				/* info for each loopback device */
	queue_t *qptr;
	queue_t *oqptr;
	int use_putnext;		/* instead of service queue */
	int use_bufcall;		/* instead of putnext */
	int msglvl;			/* # msgs to queue before sending upstream */
	int msgcnt;			/* # msgs in the queue */
	int timr;			/* time out amount while msgs queued */
	toid_t timr_hndl;		/* for running timer */
	int mark;			/* "mark" the next message(s) from above */
	int minor_nr;			/* minor number of this instance */
	int catlvl;			/* # msgs to concat before sending upstream */
	int copy_bfr;			/* copy the message before looping */
	mblk_t *saved_msg;		/* msg saved for contatenation */
	char *copy_buf;			/* for copyin/out */
	int copy_buf_size;		/* length of copy_buf data */
	loop_xparent_t xp;		/* copied in from user space */
	int deny_2nd_open;		/* deny a subsequent open of this minor */
	int burst;			/* send a burst of messages upstream */
};

/* Arry.  w/ opened devices info & number of openened devices
 */
static struct loop loop_loop[NLOOP];
static int loop_cnt = NLOOP;

/*  -------------------------------------------------------------------  */

static void
make_node(char *name, int maj, int mnr)
{
	int rtn;

	rtn = lis_unlink(name);
	if (rtn < 0 && rtn != -ENOENT) {
		printk("unlink %s: error %d\n", name, rtn);
		return;
	}

	if (maj == 0) {		/* don't make the node */
		printk("unlink %s: OK\n", name);
		return;
	}

	rtn = lis_mknod(name, 0666 | S_IFCHR, UMKDEV(maj, mnr));
	if (rtn == 0)
		printk("mknod %s: OK\n", name);
	else
		printk("mknod %s: error %d\n", name, rtn);
}

void
sloop_init(void)
{
	make_node("/dev/loop_clone", CLONE__CMAJOR_0, SLOOP__CMAJOR_0);
	make_node("/dev/loop.1", SLOOP__CMAJOR_0, 1);
	make_node("/dev/loop.2", SLOOP__CMAJOR_0, 2);
	make_node("/dev/loop.9", SLOOP__CMAJOR_0, 9);
	make_node("/dev/loop.255", SLOOP__CMAJOR_0, 255);

}				/* loop_init */

void
sloop_term(void)
{
	make_node("/dev/loop_clone", 0, 0);
	make_node("/dev/loop.1", 0, 0);
	make_node("/dev/loop.2", 0, 0);
	make_node("/dev/loop.9", 0, 0);
	make_node("/dev/loop.255", 0, 0);

}				/* loop_term */

/*  -------------------------------------------------------------------  */

static int _RP
loop_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
	static struct loop z;
	struct loop *loop;
	int dev;

	if (sflag == CLONEOPEN) {
		for (dev = 1; dev < loop_cnt; dev++) {
			if (loop_loop[dev].qptr == NULL)
				break;
		}
	} else
		dev = MINOR(*devp);

	if (dev >= loop_cnt)
		return -ENODEV;

	loop = &loop_loop[dev];
	*devp = makedevice(getmajor(*devp), dev);
	if (q->q_ptr) {		/* already open */
		if (loop->deny_2nd_open)
			return -EBUSY;
		return 0;	/* success */
	} else			/* first open */
		*loop = z;	/* clear structure */

	WR(q)->q_ptr = (char *) loop;
	q->q_ptr = (char *) loop;
	loop->qptr = WR(q);
	loop->minor_nr = dev;

	MODGET();

	return 0;		/* success */
}

/*  -------------------------------------------------------------------  */

static void _RP
loop_timeout(caddr_t arg)
{
	struct loop *loop = (struct loop *) arg;

	if (loop->qptr != NULL) {
		enableok(WR(loop->qptr));	/* allow qenable to work */
		qenable(WR(loop->qptr));
	}

}				/* loop_timeout */

/*  -------------------------------------------------------------------  */

/*
 * loop_bufcall
 *
 * Called from the STREAMS bufcall mechanism.
 *
 * The argument is a pointer to a queue to enable.
 */
static void _RP
loop_bufcall(long q_ptr)
{
	enableok((queue_t *) q_ptr);	/* allow qenable to work */
	qenable((queue_t *) q_ptr);

}				/* loop_bufcall */

/*  -------------------------------------------------------------------  */

/*
 * Concatenate the given message with the "saved" message in the
 * loop structure.  This allows the user to build multi-segment
 * messages that later get sent upstream.
 */
static mblk_t *
loop_concat(struct loop *loop, mblk_t *mp)
{
	mblk_t *lastmp;

	if (loop->saved_msg == NULL) {
		loop->saved_msg = mp;
		return (mp);
	}

	for (lastmp = loop->saved_msg; lastmp->b_cont != NULL; lastmp = lastmp->b_cont) ;

	lastmp->b_cont = mp;	/* hook new onto saved msg */
	mp = loop->saved_msg;	/* mp becomes new concatenated msg */
	loop->saved_msg = NULL;	/* no longer saved */
	return (mp);		/* return entire message */

}				/* loop_concat */

/*  -------------------------------------------------------------------  */

int
loop_request_copyio(queue_t *wq, struct loop *loop, mblk_t *mp, int msg_type, void *uptr,
		    int nbytes)
{
	struct copyreq *rq = (struct copyreq *) mp->b_rptr;
	mblk_t *dp = mp->b_cont;

	mp->b_wptr = mp->b_rptr + sizeof(*rq);
	rq->cq_addr = uptr;
	rq->cq_size = nbytes;
	rq->cq_flag = 0;
	rq->cq_private = (mblk_t *) loop;

	if (dp != NULL && msg_type == M_COPYIN) {
		mp->b_cont = NULL;	/* discard data */
		freemsg(dp);
	}

	mp->b_datap->db_type = msg_type;	/* M_COPYIN or M_COPYOUT */
	qreply(wq, mp);
	return (0);

}				/* loop_request_copyio */

/*  -------------------------------------------------------------------  */

int
loop_iocdata(queue_t *wq, mblk_t *mp)
{
	struct copyresp *rsp = (struct copyresp *) mp->b_rptr;
	struct loop *loop = (struct loop *) rsp->cp_private;
	struct iocblk *iocb;

	iocb = (struct iocblk *) rsp;
	switch (rsp->cp_cmd) {
	case LOOP_XPARENT_COPYIN:
	case LOOP_XPARENT_COPYOUT:
	{
		mblk_t *dp;

		dp = mp->b_cont;
		if (rsp->cp_rval != 0) {
			printk("loop_iocdata: copyin request failed\n");
			return (-EINVAL);
		}

		if (dp == NULL || dp->b_datap->db_type != M_DATA) {
			printk("loop_iocdata: attached bfr NULL or not M_DATA\n");
			return (-EINVAL);
		}

		if (msgdsize(dp) != sizeof(loop_xparent_t)) {
			printk("loop_iocdata: expected %d bytes, got %d\n", sizeof(loop_xparent_t),
			       msgdsize(dp));
			return (-EINVAL);
		}

		loop->xp = *((loop_xparent_t *) dp->b_rptr);
		mp->b_cont = NULL;
		freemsg(dp);

		/* 
		 * If we fetched the xparent struct for a copy in then issue
		 * another copy in for the actual data from user space.
		 * If it was for a copy out then return the requested data
		 * in an M_DATA and request it to be copied out to the user's
		 * space.
		 *
		 * Alter the command by oring in an 0xf00 so that when the
		 * iocdata for these requests comes down we can tell that
		 * they are for the final copy functions.  Handled in separate
		 * cases below.
		 */
		switch (rsp->cp_cmd) {
		case LOOP_XPARENT_COPYIN:
			rsp->cp_cmd |= 0xf00;	/* alter the command */
			if (loop_request_copyio
			    (wq, loop, mp, M_COPYIN, loop->xp.p_arg, loop->xp.i_arg) < 0) {
				printk("loop_iocdata: loop_request_copyio failed\n");
				return (-EINVAL);
			}
			return (1);	/* keep exchanging msgs w/strm hd */

		case LOOP_XPARENT_COPYOUT:
			if (loop->copy_buf == NULL)
				return (-EINVAL);

			if (loop->copy_buf_size < loop->xp.i_arg)
				loop->xp.i_arg = loop->copy_buf_size;

			dp = allocb(loop->xp.i_arg, BPRI_LO);
			if (dp == NULL)
				return (-ENOMEM);

			memcpy(dp->b_wptr, loop->copy_buf, loop->xp.i_arg);
			dp->b_wptr += loop->xp.i_arg;
			linkb(mp, dp);
			rsp->cp_cmd |= 0xf00;	/* alter the command */
			if (loop_request_copyio
			    (wq, loop, mp, M_COPYOUT, loop->xp.p_arg, loop->xp.i_arg) < 0) {
				printk("loop_iocdata: loop_request_copyio failed\n");
				return (-EINVAL);
			}
			return (1);	/* keep exchanging msgs w/strm hd */
		}
		break;
	}

		/* 
		 * This is the actual data that the user wanted us to copy in.
		 */
	case LOOP_XPARENT_COPYIN | 0xf00:
	{
		mblk_t *dp;

		dp = mp->b_cont;
		if (rsp->cp_rval != 0) {
			printk("loop_iocdata: copyin request failed\n");
			return (-EINVAL);
		}

		if (dp == NULL || dp->b_datap->db_type != M_DATA) {
			printk("loop_iocdata: attached bfr NULL or not M_DATA\n");
			return (-EINVAL);
		}

		if (msgdsize(dp) != loop->xp.i_arg) {
			printk("loop_iocdata: expected %d bytes, got %d\n", loop->xp.i_arg,
			       msgdsize(dp));
			return (-1);
		}

		if (loop->copy_buf != NULL)
			FREE(loop->copy_buf);

		loop->copy_buf = MALLOC(loop->xp.i_arg);
		if (loop->copy_buf == NULL)
			return (-ENOMEM);

		memcpy(loop->copy_buf, dp->b_rptr, loop->xp.i_arg);
		loop->copy_buf_size = loop->xp.i_arg;
		return (0);	/* send iocack now */
	}

		/* 
		 * This is the result of copying out data to the user.
		 */
	case LOOP_XPARENT_COPYOUT | 0xf00:
	{
		if (rsp->cp_rval != 0) {
			printk("loop_iocdata: copyin request failed\n");
			return (-EINVAL);
		}

		return (0);	/* send iocack now */
	}

	default:
		printk("loop_iocdata: cmd=0x%x unknown\n", rsp->cp_cmd);
		return (-EINVAL);
	}

	return (0);

}				/* loop_iocdata */

/*  -------------------------------------------------------------------  */

static int _RP
loop_wput(queue_t *q, mblk_t *mp)
{
	struct loop *loop;
	int rtn_count = 0;

	loop = (struct loop *) q->q_ptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	{
		struct iocblk *iocb;
		int error;

		iocb = (struct iocblk *) mp->b_rptr;

		switch (iocb->ioc_cmd) {
		case LOOP_SET:
		{
			int to;

			if (iocb->ioc_count != sizeof(int)) {
				printk("Expected ioctl len %d, got %d\n", sizeof(int),
				       iocb->ioc_count);

				error = EINVAL;
				goto iocnak;
			}
			to = *(int *) mp->b_cont->b_rptr;

			if (to >= loop_cnt || to < 0 || !loop_loop[to].qptr) {
				error = ENXIO;
				goto iocnak;
			}
			if (loop->oqptr || loop_loop[to].oqptr) {
				error = EBUSY;
				goto iocnak;
			}
			loop->oqptr = RD(loop_loop[to].qptr);
			loop_loop[to].oqptr = RD(q);
			break;
		}

		case LOOP_PUTNXT:
			loop->use_putnext = 1;	/* instead of service queue */
			break;

		case LOOP_MSGLVL:
			if (mp->b_cont != NULL)
				loop->msglvl = *(int *) mp->b_cont->b_rptr;
			else {
				printk("loop_wput: no data buffer in ioctl\n");
				error = EINVAL;
				goto iocnak;
			}
			break;

		case LOOP_CONCAT:
			if (mp->b_cont != NULL)
				loop->catlvl = *(int *) mp->b_cont->b_rptr;
			else {
				printk("loop_wput: no data buffer in ioctl\n");
				error = EINVAL;
				goto iocnak;
			}
			break;

		case LOOP_TIMR:
			if (mp->b_cont != NULL)
				loop->timr = *(int *) mp->b_cont->b_rptr;
			else {
				printk("loop_wput: no data buffer in ioctl\n");
				error = EINVAL;
				goto iocnak;
			}
			break;

		case LOOP_MARK:
			loop->mark++;	/* mark next message */
			break;

		case LOOP_GET_DEV:	/* get minor number */
		{
			int *devp;

			if (iocb->ioc_count != sizeof(int)) {
				printk("Expected ioctl len %d, got %d\n", sizeof(int),
				       iocb->ioc_count);

				error = EINVAL;
				goto iocnak;
			}

			if (mp->b_cont != NULL)
				devp = (int *) mp->b_cont->b_rptr;
			else {
				printk("loop_wput: no data buffer in ioctl\n");
				error = EINVAL;
				goto iocnak;
			}
			*devp = loop->minor_nr;	/* return minor device nr */
			rtn_count = sizeof(int);
		}
			break;

		case LOOP_BUFCALL:
			loop->use_bufcall++;
			break;

		case LOOP_COPY:
			loop->copy_bfr = 1;
			break;

		case LOOP_BURST:
			loop->burst = *(int *) mp->b_cont->b_rptr;
			break;

			/* 
			 * Fetch the loop module's xparent structure from user space.
			 * When it arrives then copy data into or out of kernel space
			 * as requested.
			 */
		case LOOP_XPARENT_COPYIN:
		case LOOP_XPARENT_COPYOUT:
		{
			void **ptr_ptr;
			void *uptr;

			if (mp->b_cont == NULL) {
				printk("loop_wput: LOOP_XPARENT_COPYIN/OUT: no M_DATA\n");
				error = EINVAL;
				goto iocnak;
			}

			ptr_ptr = (void **) mp->b_cont->b_rptr;
			uptr = *ptr_ptr;
			if (loop_request_copyio(q, loop, mp, M_COPYIN, uptr, sizeof(loop_xparent_t))
			    < 0) {
				error = EINVAL;
				goto iocnak;
			}
			return (0);	/* don't respond w/iocack yet */
		}

		case LOOP_DENY_OPEN:
			loop->deny_2nd_open ^= 1;	/* flip flag */
			break;

		case LOOP_FLUSH:
		{
			mblk_t *fmp;

			if (loop->oqptr == NULL) {
				error = EINVAL;
				goto iocnak;
			}

			if ((fmp = allocb(4, BPRI_MED)) == NULL) {
				error = ENOMEM;
				goto iocnak;
			}

			fmp->b_datap->db_type = M_FLUSH;
			*fmp->b_wptr++ = FLUSHR;
			putnext(loop->oqptr, fmp);
			if (mp->b_cont != NULL) {	/* follow with data */
				fmp = mp->b_cont;
				mp->b_cont = NULL;
				putnext(loop->oqptr, fmp);
			}
		}
			break;

		default:
			printk("Expected ioctl %x, got %x\n", LOOP_SET, iocb->ioc_cmd);
			error = EINVAL;
		      iocnak:{
				mp->b_datap->db_type = M_IOCNAK;
				iocb->ioc_error = error;
				qreply(q, mp);
			}
			return (0);
		}		/* end of switch on ioc_cmd */

		mp->b_datap->db_type = M_IOCACK;
		iocb->ioc_count = rtn_count;
		qreply(q, mp);

		break;
	}

	case M_IOCDATA:
	{
		struct iocblk *iocb;
		int err;

		iocb = (struct iocblk *) mp->b_rptr;
		if ((err = loop_iocdata(q, mp)) < 0) {
			mp->b_datap->db_type = M_IOCNAK;
			iocb->ioc_cmd &= 0xff;	/* original cmnd */
			iocb->ioc_count = 0;
			iocb->ioc_error = -err;
			mp->b_wptr = mp->b_rptr + sizeof(*iocb);
			qreply(q, mp);
			return (0);
		}

		if (err > 0)	/* more message xchg going on */
			break;

		mp->b_datap->db_type = M_IOCACK;
		iocb->ioc_cmd &= 0xff;	/* original cmnd */
		iocb->ioc_count = 0;
		iocb->ioc_error = 0;
		mp->b_wptr = mp->b_rptr + sizeof(*iocb);
		qreply(q, mp);	/* finally ack the ioctl */
		break;
	}

	case M_FLUSH:
	{
		if (*mp->b_rptr & FLUSHW)
			flushq(q, FLUSHDATA);
		if (*mp->b_rptr & FLUSHR) {
			flushq(RD(q), FLUSHDATA);
			*mp->b_rptr &= ~FLUSHW;
			qreply(q, mp);
		} else
			freemsg(mp);
		break;
	}
	default:
		if (loop->oqptr == NULL) {
			putctl1(RD(q)->q_next, M_ERROR, ENXIO);
			freemsg(mp);
			break;
		}

		if (loop->burst) {
			mblk_t *new_mp;

			for (--loop->burst; loop->burst > 0; loop->burst--) {
				new_mp = copymsg(mp);
				if (new_mp == NULL) {
					printk("loop_wput:burst: can't copy message.\n");
					break;
				}
				putnext(loop->oqptr, new_mp);
			}
			/* 
			 * Fall through and send original with rest of options
			 */
		} else if (loop->copy_bfr) {
			mblk_t *new_mp;

			new_mp = copymsg(mp);
			if (new_mp == NULL)
				printk("loop_wput: can't copy message.  Sending original\n");
			else {
				freemsg(mp);
				mp = new_mp;
			}
		}

		if (loop->mark) {
			loop->mark--;
			mp->b_flag |= MSGMARK;
		}

		if (loop->catlvl > 0) {	/* concatenating msgs */
			mp = loop_concat(loop, mp);	/* do it */
			if (--loop->catlvl > 0) {	/* send it yet? */
				loop->saved_msg = mp;	/* no, save it */
				break;	/* and exit */
			}
		}

		if (loop->use_putnext && bcanputnext(loop->oqptr, mp->b_band))
			putnext(loop->oqptr, mp);
		else if (loop->use_bufcall) {
			noenable(q);	/* prevent queue enable */
			putqf(q, mp);
			bufcall(0, 0, loop_bufcall, (long) q);
		} else {
			loop->msgcnt++;	/* count enqueued message */
			if (loop->msglvl > 0 || loop->timr != 0)
				noenable(q);	/* prevent queue enable */

			/* ensure timer running before putting into queue */
			if (loop->timr != 0 && loop->timr_hndl == 0)
				loop->timr_hndl = timeout(loop_timeout, (caddr_t) loop, loop->timr);

			putqf(q, mp);
			if (loop->timr_hndl == 0 && loop->msgcnt > loop->msglvl)
				qenable(q);
		}
		break;

	}

	return (0);
}

/*  -------------------------------------------------------------------  */

static int _RP
loop_wsrv(queue_t *q)
{
	mblk_t *mp;
	register struct loop *loop;

	loop = (struct loop *) q->q_ptr;

	if (loop->use_bufcall == 0 && loop->timr_hndl == 0 && loop->msgcnt <= loop->msglvl)
		return (0);

	if (loop->timr_hndl != 0) {
		untimeout(loop->timr_hndl);
		loop->timr_hndl = 0;
	}

	while ((mp = getq(q)) != NULL) {
		if (loop->oqptr == NULL) {	/* other side closed */
			freemsg(mp);
			flushq(q, FLUSHALL);
			break;
		}
		if (mp->b_datap->db_type <= QPCTL && !bcanputnext(loop->oqptr, mp->b_band)) {
			putbqf(q, mp);
			if (loop->use_bufcall)
				bufcall(0, 0, loop_bufcall, (long) q);
			break;
		}

		if (loop->use_bufcall)
			loop->use_bufcall--;

		putnext(loop->oqptr, mp);
		if (--loop->msgcnt <= loop->msglvl)
			break;
	}

	return (0);
}

/*  -------------------------------------------------------------------  */

static int _RP
loop_rsrv(queue_t *q)
{
	struct loop *loop;

	loop = (struct loop *) q->q_ptr;
	if (loop->oqptr == NULL)
		return (0);

	qenable(WR(loop->oqptr));
	return (0);
}

/*  -------------------------------------------------------------------  */

static int _RP
loop_close(queue_t *q, int dummy, cred_t *credp)
{
	struct loop *loop;
	static struct loop z;

	loop = (struct loop *) q->q_ptr;
	loop->qptr = NULL;

	if (loop->timr_hndl != 0) {
		untimeout(loop->timr_hndl);
		loop->timr_hndl = 0;
	}

	if (loop->oqptr) {
		((struct loop *) loop->oqptr->q_ptr)->qptr = NULL;
		((struct loop *) loop->oqptr->q_ptr)->oqptr = NULL;
		putctl(loop->oqptr->q_next, M_HANGUP);
		loop->oqptr = NULL;
	}
	flushq(q, FLUSHALL);
	flushq(WR(q), FLUSHALL);
	if (loop->saved_msg != NULL) {
		freemsg(loop->saved_msg);
		loop->saved_msg = NULL;
	}

	if (loop->copy_buf != NULL) {
		FREE(loop->copy_buf);
		loop->copy_buf = NULL;
	}

	*loop = z;		/* clear structure */

	MODPUT();
	return (0);
}

#if !defined __NO_VERSION__

#ifdef KERNEL_2_5
int
loop_init_module(void)
#else
int
init_module(void)
#endif
{
	int ret = lis_register_strdev(SLOOP__CMAJOR_0, &sloop_info, NLOOP, "loop");

	if (ret < 0) {
		printk("loop.init_module: Unable to register driver.\n");
		return ret;
	}
	return 0;
}

#ifdef KERNEL_2_5
void
loop_cleanup_module(void)
#else
void
cleanup_module(void)
#endif
{
	if (lis_unregister_strdev(SLOOP__CMAJOR_0) < 0)
		printk("loop.cleanup_module: Unable to unregister driver.\n");
	else
		printk("loop.cleanup_module: Unregistered, ready to be unloaded.\n");
	return;
}

#ifdef KERNEL_2_5
module_init(loop_init_module);
module_exit(loop_cleanup_module);
#endif
#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL");
#endif

#endif				/* !defined __NO_VERSION__ */
