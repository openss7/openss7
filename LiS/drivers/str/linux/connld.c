/*****************************************************************************

 @(#) $RCSfile: connld.c,v $ $Name:  $($Revision: 1.1.1.4.4.7 $) $Date: 2005/12/18 06:37:58 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/12/18 06:37:58 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: connld.c,v $ $Name:  $($Revision: 1.1.1.4.4.7 $) $Date: 2005/12/18 06:37:58 $"

/*
 *  connld.c - unique pipe generator
 *
 *  On the first open of this module, a connection to it's stream head
 *  is made, but nothing else.  On subsequent calls, however, the open
 *  does the following:
 *  . generate a unique pipe
 *  . return one endpoint of the pipe to replace itself, i.e, by returning
 *    the near endpoint's device number
 *  . send the far endpoint's file via an M_PASSFP message to the
 *    the far endpoint of its own pipe, to be picked up via an
 *    I_RECVFD ioctl.
 *  Thus, this module can only be pushed on a pipe (not even on
 *  a single-ended FIFO).
 *
 *  Note that this module will never handle message traffic.
 *
 *  Copyright (C)  2000  John A. Boyd Jr.  protologos, LLC
 */

#include <sys/LiS/module.h>	/* must be VERY first include */

#include <sys/LiS/config.h>

#include <sys/stream.h>
#include <sys/stropts.h>
#include <linux/errno.h>
#include <sys/cmn_err.h>

#include <sys/LiS/head.h>
#include <sys/osif.h>

/*
 *  Some configuration sanity checks
 */
#ifndef CONNLD__MOD
#error Not configured
#endif

#ifndef CONNLD__ID
#define CONNLD__ID 0xaabc
#endif

#define  MOD_ID   CONNLD__ID
#define  MOD_NAME "connld"

/*
 *  function prototypes
 */
static int _RP connld_open(queue_t *, dev_t *, int, int, cred_t *);
static int _RP connld_close(queue_t *, int, cred_t *);
static int _RP connld_wput(queue_t *q, mblk_t *mp);
static int _RP connld_rput(queue_t *q, mblk_t *mp);

/*
 *  module structure
 */
static struct module_info connld_minfo = {
	MOD_ID,				/* id */
	MOD_NAME,			/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* highwater mark */
	512L				/* lowwater mark */
};

static struct qinit connld_rinit = {
	connld_rput,			/* put */
	NULL,				/* service */
	connld_open,			/* open */
	connld_close,			/* close */
	NULL,				/* admin */
	&connld_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit connld_winit = {
	connld_wput,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&connld_minfo,			/* info */
	NULL				/* stat */
};

struct streamtab connld_info = {
	&connld_rinit,			/* read queue init */
	&connld_winit,			/* write queue init */
	NULL,				/* lower mux read queue init */
	NULL				/* lower mux write queue init */
};

/*
 *  open
 */
static int _RP
connld_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
	stdata_t *head = q->q_str;

	if (!head || head->magic != STDATA_MAGIC) {
		cmn_err(CE_CONT, "connld_open(): bad reference to stream head\n");
		return -EINVAL;
	}
	if (!F_ISSET(head->sd_flag, STFIFO)) {
		cmn_err(CE_CONT, "connld_open(): not even a FIFO!!\n");
		return -EPIPE;
	}
	if (!head->sd_peer || head->sd_peer == head) {
		cmn_err(CE_CONT, "connld_open(): this STREAM just is a FIFO, not a pipe\n");
		return -EPIPE;
	}
#if 0 && defined(DEBUG)
	cmn_err(CE_CONT, "connld_open(q@0x%p, 0x%x, 0x%x,0x%x, ...)#%d " "h@0x%p/%d/%d f@0x%p/%d\n",
		q, *devp, flag, sflag, (int) q->q_ptr, head, (head ? LIS_SD_REFCNT(head) : 0),
		(head ? LIS_SD_OPENCNT(head) : 0), head->sd_file,
		(head->sd_file ? F_COUNT(head->sd_file) : 0));
#endif

	if (q->q_ptr) {
		struct file *f0, *f1, *oldf = head->sd_file;
		int error;

		if (head != FILE_STR(oldf)) {
			cmn_err(CE_CONT, "connld_open(): invalid sd_file 0x%p\n", head->sd_file);
			return -EINVAL;
		}
#if 0 && defined(DEBUG)
		cmn_err(CE_CONT, "connld_open(): opening file@0x%p/%d - creating pipe...\n", oldf,
			(oldf ? F_COUNT(oldf) : 0));
#endif

		if ((error = lis_get_pipe(&f0, &f1)) < 0) {
			cmn_err(CE_CONT, "connld_open(): get_pipe() failed: %d\n", error);
			return error;
		}
#if 0 && defined(DEBUG)
		cmn_err(CE_CONT,
			"connld_open(...)" " >> pipe 0x%x(h@0x%p/%d/%d) <=X=> 0x%x(h@0x%p/%d/%d)\n",
			GET_I_RDEV(FILE_INODE(f0)), FILE_STR(f0), LIS_SD_REFCNT(FILE_STR(f0)),
			LIS_SD_OPENCNT(FILE_STR(f0)), GET_I_RDEV(FILE_INODE(f1)), FILE_STR(f1),
			LIS_SD_REFCNT(FILE_STR(f1)), LIS_SD_OPENCNT(FILE_STR(f1))
		    );
#endif

		/* 
		 *  send one of the new pipe's ends to the peer of the connld pipe
		 */
		if ((error = lis_sendfd(head, -1, f1)) < 0) {
			cmn_err(CE_CONT, "connld_open(): sendfd(h@0x%p,-1,f@0x%p) failed - %d\n",
				head, f1, error);
			lis_strclose(FILE_INODE(f0), f0);
			lis_strclose(FILE_INODE(f1), f1);
			return error;
		}

		/* 
		 *  replace the current opening file's inode
		 */
		if (!lis_old_inode(oldf, FILE_INODE(f0))) {
			lis_strclose(FILE_INODE(f0), f0);
			return -EINVAL;
		}
		/* 
		 *  we now have an unused file pointer to the new pipe end (f0)
		 *  we must discard, and we need to get all the reference counts
		 *  right for both the original and the new pipe end (net change:
		 *  old pipe end: refcnt--, new pipe end: opencnt--) so that
		 *  the stropen in progress will finish appropriately using the
		 *  new pipe end, with the original left as if no open was even
		 *  attempted.
		 *
		 *  FIXME: stropen() is holding head->sd_opening when it calls
		 *  open_mods(), which gets us here, but doclose also tries to
		 *  hold it.  To prevent a deadlock, I'm switching the lock here
		 *  from the old head to the new one, leaving it open during the
		 *  intervening fput(), but it might be better to have stropen
		 *  call open_mods() with no locks held...  Dave?... - JB 12/6/02
		 */
		{
			struct stdata *oldhead = head;

			*devp = GET_I_RDEV(FILE_INODE(f0));

			K_ATOMIC_DEC(&oldhead->sd_refcnt);
			head = FILE_STR(f0);
			SET_FILE_STR(oldf, head);
			lis_up(&oldhead->sd_opening);	/* release this before fput */

			K_ATOMIC_INC(&head->sd_refcnt);
			K_ATOMIC_INC(&head->sd_opencnt);	/* prevent opencnt->0 */
			fput(f0);	/* calls strclose */

			K_ATOMIC_DEC(&head->sd_opencnt);
			lis_down(&head->sd_opening);

#if 0 && defined(DEBUG)
			cmn_err(CE_CONT,
				"connld_open(...)"
				" >> replaced h@0x%p/--%d/%d -> h@0x%p/%d/--%d\n", oldhead,
				LIS_SD_REFCNT(oldhead), LIS_SD_OPENCNT(oldhead), head,
				LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head));
#endif
		}

#if 0 && defined(DEBUG)
		cmn_err(CE_CONT,
			"connld_open(q@0x%xp, 0x%x, 0x%x,0x%x, ...)#%d " "head@0x%p/%d/%d OK\n",
			(int) q, *devp, flag, sflag, (int) q->q_ptr, head, LIS_SD_REFCNT(head),
			LIS_SD_OPENCNT(head));
#endif

		return 0;
	} else {
		/* 
		 *  first time in - check STCONNLD flag, then set it
		 */
		if (F_ISSET(head->sd_flag, STCONNLD)) {
			cmn_err(CE_CONT, "connld_open(): previously pushed 'connld'\n");
			return -EINVAL;
		}
		SET_SD_FLAG(head, STCONNLD);

		/* 
		 *  set q_ptr as an open count flag
		 */
		{
			char *cp = (char *) q->q_ptr;

			q->q_ptr = (void *) ++cp;
		}

		MODGET();

		return 0;
	}
}

/*
 *  close
 */
static int _RP
connld_close(queue_t *q, int flag, cred_t *credp)
{
	stdata_t *head = q->q_str;

#if 0 && defined(DEBUG)
	cmn_err(CE_CONT, "connld_close(q@0x%p, 0x%x, ...) head 0x%p\n", q, flag, head);
#endif

	q->q_ptr = NULL;

	MODPUT();

	qprocsoff(q);

	if (!head || head->magic != STDATA_MAGIC) {
		cmn_err(CE_CONT, "connld_open(): bad reference to stream head\n");
		return -EINVAL;
	}
	CLR_SD_FLAG(head, STCONNLD);

	return 0;
}

/*
 *  wput
 */
static int _RP
connld_wput(queue_t *q, mblk_t *mp)
{
	/* 
	 *  connld is a module, but will at the bottom of a STREAM,
	 *  like a driver.  Thus, IOCTLs must be NAK'ed, like a driver,
	 *  not passed down, like a module.  Otherwise, STREAMS might
	 *  hang waiting for the IOCTL response message.
	 *
	 *  connld should not be expected to process any other message
	 *  traffic, so we discard all other messages.
	 */
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		if (q->q_next && SAMESTR(q->q_next)) {
			putnext(q, mp);
		} else {
			mp->b_datap->db_type = M_IOCNAK;
			qreply(q, mp);
		}
		break;
	default:
#if 0 && defined(DEBUG)
		cmn_err(CE_CONT, "connld_wput(q@0x%p,m@0x%p) message discarded\n", q, mp);
#endif
		freemsg(mp);
		break;
	}
	return 0;
}

/*
 *  rput
 */
static int _RP
connld_rput(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);

	return 0;
}

#if !defined __NO_VERSION__

/*
 *  Linux loadable module interface
 */

#ifdef KERNEL_2_5
int
connld_init_module(void)
#else
int
init_module(void)
#endif
{
	int ret = lis_register_strmod(&connld_info, MOD_NAME);

	if (ret < 0) {
		cmn_err(CE_CONT, "%s - unable to register module.\n", MOD_NAME);
		return ret;
	}

	return 0;
}

#ifdef KERNEL_2_5
void
connld_cleanup_module(void)
#else
void
cleanup_module(void)
#endif
{
	if (lis_unregister_strmod(&connld_info) < 0)
		cmn_err(CE_CONT, "%s - unable to unregister module.\n", MOD_NAME);
	return;
}

#ifdef KERNEL_2_5
module_init(connld_init_module);
module_exit(connld_cleanup_module);
#endif

#if defined(LINUX)		/* linux kernel */

#ifdef __attribute_used__
#undef __attribute_used__
#endif
#define __attribute_used__

#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL v2");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("John Boyd <jaboydjr@protologos.net>");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("STREAMS unique pipe generator pseudo-module");
#endif
#if defined(MODULE_ALIAS)
MODULE_ALIAS("streams-" __stringify(LIS_OBJNAME));
#endif

#endif				/* LINUX */
#endif				/* !defined __NO_VERSION__ */
