/*****************************************************************************

 @(#) tirdwr.c,v (0.9.2.8) 2003/10/21 21:50:22

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified 2003/10/21 21:50:22 by brian

 *****************************************************************************/

#ident "@(#) tirdwr.c,v (0.9.2.8) 2003/10/21 21:50:22"

static char const ident[] =
    "tirdwr.c,v (0.9.2.8) 2003/10/21 21:50:22";

/* 
 *  This is TIRDWR an TLI read/write module for TPI Version 2 transport
 *  service providers supporting the T_OPTDATA_IND primitives.
 *
 *  The purpose of this module is simple: to make a connected transport
 *  provider appear to be a STREAMS-based pipe (s_pipe(3)).
 */

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/kmem.h>
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>

#include <tihdr.h>

#include "strdebug.h"

#include "sys/config.h"

#define TIRDWR_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TIRDWR_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define TIRDWR_REVISION		"LfS tirdwr.c,v (0.9.2.8) 2003/10/21 21:50:22"
#define TIRDWR_DEVICE		"SVR 4.2 STREAMS Read Write Module for TLI Devices (TIRDWR)"
#define TIRDWR_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TIRDWR_LICENSE		"GPL"
#define TIRDWR_BANNER		TIRDWR_DESCRIP		"\n" \
				TIRDWR_COPYRIGHT	"\n" \
				TIRDWR_REVISION		"\n" \
				TIRDWR_DEVICE		"\n" \
				TIRDWR_CONTACT		"\n"
#define TIRDWR_SPLASH		TIRDWR_DEVICE		" - " \
				TIRDWR_REVISION		"\n"

MODULE_AUTHOR(TIRDWR_CONTACT);
MODULE_DESCRIPTION(TIRDWR_DESCRIP);
MODULE_SUPPORTED_DEVICE(TIRDWR_DEVICE);
MODULE_LICENSE(TIRDWR_LICENSE);

#ifndef CONFIG_STREAMS_TIRDWR_NAME
//#define CONFIG_STREAMS_TIRDWR_NAME "tirdwr"
#error "CONFIG_STREAMS_TIRDWR_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_TIRDWR_MODID
//#define CONFIG_STREAMS_TIRDWR_MODID 16
#error "CONFIG_STREAMS_TIRDWR_MODID must be defined."
#endif

static modID_t modid = CONFIG_STREAMS_TIRDWR_MODID;
MODULE_PARM(modid, "b");
MODULE_PARM_DESC(modid, "Module ID for TIRDWR.");

static struct module_info tirdwr_minfo = {
	mi_idnum:CONFIG_STREAMS_TIRDWR_MODID,
	mi_idname:CONFIG_STREAMS_TIRDWR_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  Private Datastructure ctors and dtors
 *
 *  -------------------------------------------------------------------------
 */
struct tirdwr {
	spinlock_t lock;		/* callback lock */
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */
static int tirdwr_rput(queue_t *q, mblk_t *mp)
{
	/* On the way up we simply strip T_DATA_IND from the message and deliver just the M_DATA
	   portion.  What this accomplishes is similar to RPROTDIS, but only for T_DATA_IND
	   messages.  T_EXDATA_IND or T_OPTDATA_IND messages will be passed transparently so that
	   an EBADMSG is generated at the stream head and the reader will know to invoke getmsg or
	   error out the stream. */
	if (mp->b_datap->db_type == M_PROTO) {
		union T_primitives *p;
		mblk_t *dp;
		p = (typeof(p)) mp->b_rptr;
		switch (p->type) {
		case T_DATA_IND:
			if ((dp = unlinkb(mp))) {
				/* Although Magic Garden p. 525 says that we ignore message
				   delimitors, we restore message delimitors in case the RMSGN or
				   RMSGD read options are set at the stream head.  Otherwise they
				   will be ignored anyway. */
				dp->b_flag |= p->data_ind.MORE_flag ? 0 : MSGDELIM;
				putnext(q, dp);
			}
			freeb(mp);
			return (0);
		case T_DISCON_IND:
			if ((dp = unlinkb(mp))) {
				dp->b_flag |= MSGDELIM;
				/* Although Magic Garden p. 525 says that we ignore message
				   delimitors, we restore message delimitors in case the RMSGN or
				   RMSGD read options are set at the stream head.  Otherwise they
				   will be ignored anyway. */
				putnext(q, dp);
			}
			freeb(mp);
			putnextctl(q, M_HANGUP);
			return (0);
		case T_ORDREL_IND:
			if ((dp = unlinkb(mp))) {
				dp->b_flag |= MSGDELIM;
				/* Although Magic Garden p. 525 says that we ignore message
				   delimitors, we restore message delimitors in case the RMSGN or
				   RMSGD read options are set at the stream head.  Otherwise they
				   will be ignored anyway. */
				putnext(q, dp);
			}
			putnext(q, mp);
			return (0);
		}
	}
	putnext(q, mp);
	return (0);
}

static void tirdwr_buffer(long arg);

static int do_w_data(queue_t *q, mblk_t *mp, unsigned long *flagsp)
{
	struct tirdwr *tirdwr = q->q_ptr;
	union T_primitives *p;
	mblk_t *dp;
	if ((dp = allocb(sizeof(*p), BPRI_MED))) {
		dp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) dp->b_wptr;
		dp->b_wptr += sizeof(p->data_req);
		p->data_req.PRIM_type = T_DATA_REQ;
		p->data_req.MORE_flag = dp->b_flag & MSGDELIM ? 0 : 1;
		/* Although Magic Garden p. 525 says that we ignore message delimitors, we restore
		   message delimitors in case the RMSGN or RMSGD read options are set at the stream 
		   head.  Otherwise they will be ignored anyway. */
		linkb(dp, mp);
		spin_unlock_irqrestore(&tirdwr->lock, flagsp);
		putnext(q, dp);
		spin_lock_irqsave(&tirdwr->lock, *flagsp);
		return (0);
	} else {
		bcid_t bcid;
		bcid = bufcall(sizeof(p->data_req), BPRI_MED, tirdwr_buffer, (long) q);
		putq(q, dp);
		spin_unlock_irqrestore(&tirdwr->lock, flagsp);
		return (-ENOBUFS);
	}
}

static void tirdwr_buffer(long arg)
{
	queue_t *q = (queue_t *) arg;
	struct tirdwr *tirdwr = q->q_ptr;
	unsigned long flags;
	mblk_t *mp;
	spin_lock_irqsave(&tirdwr->lock, flags);
	while ((mp = getq(q))
			&& !do_w_data(q, mp, &flags)) ;
	spin_unlock_irqrestore(&tirdwr->lock, flags);
}

static int tirdwr_wput(queue_t *q, mblk_t *mp)
{
	struct tirdwr *tirdwr = q->q_ptr;
	unsigned long flags;
	spin_lock_irqsave(&tirdwr->lock, flags);
	if (mp->b_datap->db_type > QPCTL) {
		spin_unlock_irqrestore(&tirdwr->lock, flags);
		putnext(q, mp);
		return (0);
	}
	if (q->q_count) {
		putq(q, mp);
		spin_unlock_irqrestore(&tirdwr->lock, flags);
		return (0);
	}
	if (mp->b_datap->db_type == M_DATA) {
		do_w_data(q, mp, &flags);
		spin_unlock_irqrestore(&tirdwr->lock, flags);
		return (0);
	}
	spin_unlock_irqrestore(&tirdwr->lock, flags);
	putnext(q, mp);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static int tirdwr_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	wq = WR(q);
	if (sflag == MODOPEN || wq->q_next != NULL) {
		struct tirdwr *tirdwr;
		if (!(tirdwr = kmem_alloc(sizeof(*tirdwr), KM_NOSLEEP))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		spin_lock_init(&tirdwr->lock);
		/* we are a module with no service procedure so or hiwat, lowat don't matter;
		   however, or minpsz, maxpsz do because we are the first queue under the stream
		   head. */
		wq->q_minpsz = wq->q_next->q_minpsz;
		wq->q_maxpsz = wq->q_next->q_maxpsz;
		wq->q_ptr = q->q_ptr = tirdwr;
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);		/* can't be opened as driver */
}
static int tirdwr_close(queue_t *q, int oflag, cred_t *crp)
{
	struct tirdwr *tirdwr = q->q_ptr;
	if (!tirdwr)
		return (ENXIO);
	kmem_free(tirdwr, sizeof(*tirdwr));
	MOD_DEC_USE_COUNT;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
static struct qinit tirdwr_rinit = {
	qi_putp:tirdwr_rput,
	qi_qopen:tirdwr_open,
	qi_qclose:tirdwr_close,
	qi_minfo:&tirdwr_minfo,
};

static struct qinit tirdwr_winit = {
	qi_putp:tirdwr_wput,
	qi_minfo:&tirdwr_minfo,
};

static struct streamtab tirdwr_info = {
	st_rdinit:&tirdwr_rinit,
	st_wrinit:&tirdwr_winit,
};

static struct fmodsw tirdwr_fmod = {
	f_name:CONFIG_STREAMS_TIRDWR_NAME,
	f_str:&tirdwr_info,
	f_flag:0,
	f_kmod:THIS_MODULE,
};

static int __init tirdwr_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO TIRDWR_BANNER);
#else
	printk(KERN_INFO TIRDWR_SPLASH);
#endif
	if ((err = register_strmod(modid, &tirdwr_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};
static void __exit tirdwr_exit(void)
{
	int err;
	if ((err = unregister_strmod(modid, &tirdwr_fmod)) < 0)
		return (void) (err);
	return (void) (0);
};

module_init(tirdwr_init);
module_exit(tirdwr_exit);
