/*****************************************************************************

 @(#) $RCSfile: sth.c,v $ $Name:  $($Revision: 0.9.2.110 $) $Date: 2005/10/22 19:58:19 $

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

 Last Modified $Date: 2005/10/22 19:58:19 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sth.c,v $ $Name:  $($Revision: 0.9.2.110 $) $Date: 2005/10/22 19:58:19 $"

static char const ident[] =
    "$RCSfile: sth.c,v $ $Name:  $($Revision: 0.9.2.110 $) $Date: 2005/10/22 19:58:19 $";

//#define __NO_VERSION__

#include <stdbool.h>		/* for bool type, true and false */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* for FASTCALL(), fastcall */
#include <linux/sched.h>	/* for send_sig_info() */
#include <linux/spinlock.h>
#include <linux/fs.h>		/* for file */
#include <linux/file.h>		/* for fget() */
#include <linux/poll.h>		/* for poll_wait */
#include <linux/highmem.h>	/* for kmap, kunmap */
#include <linux/uio.h>		/* for iovec */

#include <asm/ioctls.h>		/* for TGETS, etc. */
#include <linux/termios.h>	/* for struct termios, etc. */
#include <linux/sockios.h>	/* for FIOCGETOWN, etc. */

#ifndef __user
#define __user
#endif

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "src/kernel/strsched.h"	/* for allocstr */
#include "src/kernel/strlookup.h"	/* for cmin_get() */
#include "sth.h"		/* extern verification */
#include "src/kernel/strsysctl.h"	/* for sysctls */
#include "src/kernel/strsad.h"	/* for autopush */
#include "src/kernel/strutil.h"	/* for q locking and puts and gets */
#include "src/kernel/strattach.h"	/* for do_fattach/do_fdetach */
#if 0
#include "src/kernel/strpipe.h"	/* for do_spipe */
#endif
#include "src/drivers/clone.h"	/* for (un)register_clone() */

#define STH_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define STH_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define STH_REVISION	"LfS $RCSfile: sth.c,v $ $Name:  $($Revision: 0.9.2.110 $) $Date: 2005/10/22 19:58:19 $"
#define STH_DEVICE	"SVR 4.2 STREAMS STH Module"
#define STH_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define STH_LICENSE	"GPL"
#define STH_BANNER	STH_DESCRIP	"\n" \
			STH_COPYRIGHT	"\n" \
			STH_REVISION	"\n" \
			STH_DEVICE	"\n" \
			STH_CONTACT	"\n"
#define STH_SPLASH	STH_DEVICE	" - " \
			STH_REVISION	"\n"

#ifdef CONFIG_STREAMS_STH_MODULE
MODULE_AUTHOR(STH_CONTACT);
MODULE_DESCRIPTION(STH_DESCRIP);
MODULE_SUPPORTED_DEVICE(STH_DEVICE);
MODULE_LICENSE(STH_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sth");
#endif
#endif				/* CONFIG_STREAMS_STH_MODULE */

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7
#define QR_RETRY	8

#ifndef CONFIG_STREAMS_STH_NAME
//#define CONFIG_STREAMS_STH_NAME "sth"
#error "CONFIG_STREAMS_STH_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_STH_MODID
//#define CONFIG_STREAMS_STH_MODID 0
#error "CONFIG_STREAMS_STH_MODID must be defined."
#endif

modID_t modid = CONFIG_STREAMS_STH_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module identification number for STH module.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_STH_MODID));
MODULE_ALIAS("streams-module-sth");
#endif

struct module_info str_minfo = {
	.mi_idnum = CONFIG_STREAMS_STH_MODID,
	.mi_idname = CONFIG_STREAMS_STH_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

int str_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp);
int str_close(queue_t *q, int oflag, cred_t *crp);

int strrput(queue_t *q, mblk_t *mp);

static struct qinit str_rinit = {
	.qi_putp = strrput,
	.qi_qopen = str_open,
	.qi_qclose = str_close,
	.qi_minfo = &str_minfo,
};

int strwput(queue_t *q, mblk_t *mp);
int strwsrv(queue_t *q);

static struct qinit str_winit = {
	.qi_putp = strwput,
	.qi_srvp = strwsrv,
	.qi_minfo = &str_minfo,
};

struct streamtab str_info = {
	.st_rdinit = &str_rinit,
	.st_wrinit = &str_winit,
};

/* some faster macros for known input */

#define _WR(__rq) ((__rq) + 1)
#define _RD(__wq) ((__wq) - 1)

#define stri_lookup(__f) (__f)->private_data

#ifdef HAVE_KMEMB_STRUCT_INODE_I_LOCK
#define stri_trylock(__i)   (int)({ spin_lock(&(__i)->i_lock); 0; })
#define stri_lock(__i)	    spin_lock(&(__i)->i_lock)
#define stri_unlock(__i)    spin_unlock(&(__i)->i_lock);
#else
#define stri_trylock(__i)   down_interruptible(&(__i)->i_sem)
#define stri_lock(__i)	    down(&(__i)->i_sem)
#define stri_unlock(__i)    up(&(__i)->i_sem)
#endif

/**
 *  stri_insert:    - inserta  reference to a stream head into a file pointer
 *  @file:	file pointer for stream
 *  @sd:	stream head
 */
STATIC INLINE void
stri_insert(struct file *file, struct stdata *sd)
{
	struct inode *inode = sd->sd_inode;

	/* FIXME: character device opens on external inodes alreay locks the inode before calling
	   the character device open procedure, so we do sd->sd_inode which is the internal
	   filesystem inode, however, stri_acquire and stri_remove might use the external
	   filesystem inode, but that's ok to because its locked at this point too. */

	/* always hold the inode spinlock while lookup up the STREAM head */
	printd(("%s: locking inode %p\n", __FUNCTION__, inode));
	stri_lock(inode);
	ctrace(stri_lookup(file) = sd_get(sd));
	printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
	stri_unlock(inode);
}

/**
 *  stri_acquire:   - acquire a reference to a stream head from a file pointer
 *  @file:	file pointer to stream
 */
STATIC INLINE struct stdata *
stri_acquire(struct file *file)
{
	struct stdata *sd;
	struct inode *inode = file->f_dentry->d_inode;

	/* always hold the inode spinlock while lookup up the STREAM head */
	printd(("%s: locking inode %p\n", __FUNCTION__, inode));
	stri_lock(inode);
	ctrace(sd = sd_get(stri_lookup(file)));
	printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
	stri_unlock(inode);
	return (sd);
}

/**
 *  stri_remove:    - acquire and remove a stream head from a file pointer
 *  @file:	file pointer to stream
 *
 *  Same as stri_acquire(), but also remove the reference from the file pointer.
 *
 *  Call this with the inode sempahore already held.
 */
STATIC INLINE struct stdata *
stri_remove(struct file *file)
{
	struct stdata *sd;

	assert(file);
#if 0
	/* always hold the inode spinlock while lookup up the STREAM head */
	printd(("%s: locking inode %p\n", __FUNCTION__, inode));
	stri_lock(inode);
#endif
	sd = stri_lookup(file);
	stri_lookup(file) = NULL;
#if 0				/* leave locked */
	printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
	stri_unlock(inode);
#endif
	return (sd);
}

#undef verify_area
#define verify_area(__x,__y,__z) (access_ok((__x),(__y),(__z)) ? 0 : -EFAULT)

/**
 *  strinsert:	- insert a stream head to an inode
 *  @inode:	inode to which to attach the stream head
 *  @sd:	the stream head
 *  @dev:	the original device number that qopen() was called with
 *
 *  Hold the inode semaphore while calling this function.
 */
STATIC INLINE int
strinsert(struct inode *inode, struct stdata *sd)
{
	assert(inode);
	assert(sd);
	assert(!sd->sd_inode);
	{
		struct cdevsw *cdev;

		ptrace(("adding stream %p to inode %p\n", sd, inode));
		/* don't let the inode go away while we are linked to it */
		/* no matter what filesystem it belongs to */
		sd->sd_inode = igrab(inode);
		cdev = sd->sd_cdevsw;
		assert(cdev);
		list_add(&sd->sd_list, &cdev->d_stlist);
		assert(cdev->d_inode);
		cdev->d_inode->i_nlink++;
		/* should always be successful */
		assert(sd->sd_inode != NULL);
		/* link into clone list */
		sd->sd_clone = (void *) inode->i_pipe;
		inode->i_pipe = (void *) sd;
	}
	return (0);
}

/**
 *  strremove_locked:	- remove a stream head from an inode
 *  @inode:	inode from which to remove the stream head
 *  @sd:	the stream head
 *
 *  Hold the inode semaphore while calling this function.  Don't forget to put the inode once the
 *  semaphore is released.
 */
STATIC INLINE void
strremove_locked(struct inode *inode, struct stdata *sd)
{
	assert(sd);
	assert(inode);

	/* we need to hold the inode semaphore while doing this */
	{
		struct cdevsw *cdev;
		struct stdata **sdp;

		ptrace(("removing stream %p from inode %p\n", sd, inode));
		for (sdp = (struct stdata **) &(inode->i_pipe); *sdp && *sdp != sd;
		     sdp = &((*sdp)->sd_clone)) ;
		assert(*sdp && *sdp == sd);
		/* delete stream head from clone list */
		*sdp = sd->sd_clone;
		sd->sd_clone = NULL;
		/* remove from device list */
		cdev = sd->sd_cdevsw;
		assert(cdev);
		assert(cdev->d_inode);
		cdev->d_inode->i_nlink--;
		ensure(sd->sd_list.next, INIT_LIST_HEAD(&sd->sd_list));
		list_del_init(&sd->sd_list);
		sd->sd_inode = NULL;
	}
}

/**
 *  strremove:	- remove a stream head from its inode
 *  @sd:	the stream head
 *
 *  Called before strlastclose() on a detached Stream.
 */
STATIC INLINE void
strremove(struct stdata *sd)
{
	struct inode *inode;

	if ((inode = sd->sd_inode)) {
		stri_lock(inode);
		strremove_locked(inode, sd);
		stri_unlock(inode);
		iput(inode);
	}
}

/**
 *  strinccounts:   - increment open counts
 *  @sd:	STREAM head
 *  @oflag:	open flags
 *
 *  Increments the sd_opens, sd_readers, sd_writers counts as appropriate.  The reverse is performed
 *  by strdeccounts().
 *
 *  LOCKING: Caller must hold a stream head write lock (or have a private stream head).
 */
STATIC INLINE dev_t
strinccounts(struct file *file, struct stdata *sd, int oflag)
{
	sd->sd_opens++;
	if (oflag & FREAD)
		sd->sd_readers++;
	if (oflag & FWRITE)
		sd->sd_writers++;
	sd->sd_file = file;
	return (sd->sd_dev);
}

STATIC INLINE bool
strdetached(struct stdata *sd)
{
	/* test for detached close needed */
	if (sd->sd_opens == 0 && !(sd->sd_flag & (STWOPEN | STPLEX | STRMOUNT))
	    && !test_and_set_bit(STRCLOSE_BIT, &sd->sd_flag))
		return (true);
	return (false);
}

/**
 *  strdeccounts:   - decrement STREAM counts and possibly wake other
 *  @sd:	STREAM head
 *  @oflag:	open() flags
 *
 *  Decrement the sd_opens, sd_readers and sd_writers counts as appropriate and possibly wake the
 *  other end of a STREAMS-based pipe.  Reverses the steps taken by strinccounts().
 *
 *  Return Value: Returns false (0) when this is not the last close.  Returns true (1) when this is
 *  the last close (and we are not linked under a multiplexing driver).
 *
 *  Notes: When the STREAM head is multiplexed (or is in the process of being multiplexed), or is
 *  mounted (or is in the process of being mounted) do not pop modules or perform any other STREAM
 *  head actions because the STREAM head queue pair is owned by the link or mount point.  We will
 *  have to perform close actions when the STREAM is unlinked or when a pending link operation
 *  fails, or unmounted or when a pending fattach operation fails. Leave the open count at zero so
 *  that the link/unlink code knows what is going on. Another open in the meantime might boost the
 *  count.
 */
STATIC INLINE bool
strdeccounts(struct stdata *sd, int oflag)
{
	bool last = false;
	bool wake = false;

	swlock(sd);

	if ((oflag & FREAD) && --sd->sd_readers <= 0) {
		sd->sd_readers = 0;
		wake = true;
	}
	if ((oflag & FWRITE) && --sd->sd_writers <= 0) {
		sd->sd_writers = 0;
		wake = true;
	}
	if (--sd->sd_opens <= 0) {
		sd->sd_opens = 0;
		last = strdetached(sd);
	}
	if (wake && (sd->sd_flag & STRISPIPE) && sd->sd_other
	    && waitqueue_active(&sd->sd_other->sd_waitq))
		wake_up_interruptible(&sd->sd_other->sd_waitq);

	swunlock(sd);

	return (last);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  STREAMS IO Controls
 *
 *  -------------------------------------------------------------------------
 */

STATIC inline pid_t
task_session(struct task_struct *t)
{
#if HAVE_KMEMB_STRUCT_TASK_STRUCT_SIGNAL
	return (t->signal->session);
#else
	return (t->session);
#endif
}

STATIC inline pid_t
task_pgrp(struct task_struct *t)
{
#if HAVE_KMEMB_STRUCT_TASK_STRUCT_SIGNAL
	return (t->signal->pgrp);
#else
	return (t->pgrp);
#endif
}

STATIC inline pid_t
pgrp_session(pid_t pgrp)
{
#if HAVE_SESSION_OF_PGRP_ADDR
	static pid_t (*session_of_pgrp) (pid_t) =
	    (typeof(session_of_pgrp)) HAVE_SESSION_OF_PGRP_ADDR;
#endif
	return (session_of_pgrp(pgrp));
}

/**
 *  straccess:   - check error and access conditions on a stream
 *  @sd:	the stream head
 *  @flag:	bit flags specifying what checks to perform
 *
 *  @flag takes the following:
 *      %FREAD		- perform read checks
 *      %FWRITE		- perform write checks
 *      %FEXCL		- perform tty setting io checks
 *      %FCREAT		- perform open checks only
 *      %FTRUNC		- perform close checks only (no errors)
 *      %FNDELAY	- don't return -ESTRPIPE with FREAD or FWRITE
 *      %FAPPEND	- no error for STRHUP, STRDERR or STWRERR
 *
 *      If no flags are set, simple io checks are performed.  Simple checks include I_PUSH, I_POP,
 *      I_LINK, I_PLINK.  Close, I_UNLINK, I_PUNLINK should not use this test.
 *
 *  The following is what POSIX says about reads and writes to controlling terminals:
 *
 *  "If a process is in the foreground process group of its controlling terminal, read operations
 *   shall be allowed...  Any attempts by a process in a background process group to read from its
 *   controlling terminal causes its process group to be sent a SIGTTIN signal unless one of the
 *   following special cases applies: if the reading process is ignoring or blocking the SIGTTIN
 *   signal, or if the process group of the read process is orphaned, the read() shall return -1,
 *   with errno set to [EIO] and no signal shall be sent."
 *
 *  "If a process is in the foreground process group of its controlling terminal, write operations
 *   shall be allowed...  Attempts by a process in a background process group to write to its
 *   controlling terminal shall cause the process group to be sent a SIGTTOU signal unless one of
 *   the following special cases applies: if TOSTOP is not set, or it TOSTOP is set and the process
 *   is ignoring or blocking the SIGTTOU signal, the process is allowed to write to the terminal and
 *   the SIGTTOU signal is not sent.  If TOSTOP is set, and the process group of the writing process
 *   is orphaned, and the writing process is not ignoring or block the SIGTTOU signal, the write()
 *   shall return -1, with errno set to [EIO] and no signal shall be sent."
 *
 *  "Certain calls that set terminal parameters are treated in the same fashion as write(), except
 *   that TOSTOP is ignored; that is, the effect is identical to that of terminal writes when TOSTOP
 *   is set (see Local Modes, tcdrain(), tcflow(), tcflush(), tcsendbreak(), tcsetattr() and
 *   tcsetpgrp()).
 *
 *  Use an explicit constant for @access so that many instructions will be inlined out.
 *
 */
STATIC inline streams_fastcall int
straccess(struct stdata *sd, const int access)
{
#if HAVE_IS_IGNORED_ADDR
	static int (*is_ignored) (int sig) = (typeof(is_ignored)) HAVE_IS_IGNORED_ADDR;
#endif
#if HAVE_IS_ORPHANED_PGRP_ADDR
	static int (*is_orphaned_pgrp) (int pgrp) =
	    (typeof(is_orphaned_pgrp)) HAVE_IS_ORPHANED_PGRP_ADDR;
#endif
	register int flags = sd->sd_flag;

	if ((access & (FREAD | FWRITE))) {
		/* POSIX semantics for pipes and FIFOs */
		if (flags & (STRISFIFO | STRISPIPE)) {
			if (flags & STRISPIPE) {
				if ((access & (FREAD | FWRITE)) && sd->sd_wq->q_next != sd->sd_rq
				    && (sd->sd_other == NULL || (sd->sd_other->sd_flag & STRCLOSE)))
					goto estrpipe;
			}
			if (flags & STRISFIFO) {
				if ((access & FREAD) && sd->sd_writers == 0)
					goto estrpipe;
				if ((access & FWRITE) && sd->sd_readers == 0)
					goto estrpipe;
			}
		}
	}
	/* no errors for close */
	if (!(access & FTRUNC)) {
		if ((flags & (STPLEX | STRCLOSE | STRDERR | STWRERR | STRHUP))) {
			if (flags & STPLEX)
				if (!(access & FCREAT))
					return (-EINVAL);
			if (flags & STRCLOSE) {
				if (!(flags & STRISTTY))
					return (-EIO);
				return (-ENOTTY);
			}
			if (!(access & FAPPEND)) {
				if (flags & STRHUP) {
					/* POSIX: open(): "[EIO] The path argument names a STREAMS
					   file and a hangup or error occured during the open()." */
					if (access & FWRITE)
						return (-ENXIO);	/* for TTY's too? */
					if (access & FCREAT)
						return (-EIO);
					if (!(access & FREAD))
						return (-ENXIO);
					if (!(access & FNDELAY))
						return (-ESTRPIPE);
				}
				if (flags & STRDERR) {
					if (access & FREAD) {
						if (sd->sd_eropt & RERRNONPERSIST)
							clear_bit(STRDERR_BIT, &sd->sd_flag);
						return (-sd->sd_rerror);
					} else if (!(access & FWRITE))
						return (-EIO);
				}
				if (flags & STWRERR) {
					if (access & FWRITE) {
						if (sd->sd_eropt & WERRNONPERSIST)
							clear_bit(STWRERR_BIT, &sd->sd_flag);
						if (sd->sd_wropt & SNDPIPE)
							send_sig_info(SIGPIPE, (struct siginfo *) 1,
								      current);
						return (-sd->sd_werror);
					} else if (!(access & FREAD))
						return (-EIO);
				}
			}
		}
	}
	if (!(access & (FREAD | FWRITE | FEXCL)))
		return (0);
	if (!(flags & (STRISFIFO | STRISPIPE | STRISTTY)))
		return (0);
	/* POSIX semantics for controlling terminals */
	if (flags & STRISTTY) {
		pid_t pgrp = task_pgrp(current);

		if (pgrp == sd->sd_pgrp)
			return (0);
		/* not in forground process group */
		if (access & FREAD) {
			if (is_ignored(SIGTTIN))
				return (-EIO);
			if (is_orphaned_pgrp(pgrp))
				return (-EIO);
			kill_pg(pgrp, SIGTTIN, 1);
			return (-ERESTARTSYS);
		}
		if ((access & (FWRITE | FEXCL)) && ((access & FEXCL) || (flags & STRTOSTOP))) {
			if (is_ignored(SIGTTOU))
				return (0);
			if (is_orphaned_pgrp(pgrp))
				return (-EIO);
			kill_pg(pgrp, SIGTTOU, 1);
			return (-ERESTARTSYS);
		}
	}
	return (0);
      estrpipe:
	if (!(access & FNDELAY))
		return (-ESTRPIPE);
	if (!(access & FWRITE))
		return (0);
	/* Note: we want to send the signal to the current thread in a thread group, because this
	   write belongs only to that thread.  In that way, threads handle this signal separately
	   and only receive signals for their own actions. */
	send_sig(SIGPIPE, current, 1);
	return (-EPIPE);
}

STATIC streams_fastcall int
straccess_slow(struct stdata *sd, int access)
{
	return straccess(sd, access);
}

STATIC inline streams_fastcall int
straccess_unlocked(struct stdata *sd, const int access)
{
	int err;

	srlock(sd);
	err = straccess_slow(sd, access);
	srunlock(sd);
	return (err);
}

/* Two lock checking versions - do this yourself if you want straccess inlined */
STATIC inline streams_fastcall int
straccess_rlock(struct stdata *sd, const int access)
{
	int err;

	srlock(sd);
	if ((err = straccess(sd, access)))
		srunlock(sd);
	return (err);
}

STATIC inline streams_fastcall int
straccess_wlock(struct stdata *sd, const int access)
{
	int err;

	swlock(sd);
	if ((err = straccess(sd, access)))
		swunlock(sd);
	return (err);
}

/**
 *  straccess_wakeup: - wait to perform operations on a stream
 *  @sd: stream head
 *  @f_flags: file flags
 *  @timeo: pointer to remaining time
 *  @access: access control flags
 *
 *  Like straccess() but always returns ESTRPIPE instead of zero on reads from broken or hung up
 *  pipes.  It also returns ESTRPIPE when there are no corresponding readers or writers or a write
 *  is attempted on a broken or hung up pipe.  ESTRPIPE tells the wait loop not to block further.
 */
STATIC int
straccess_wakeup(struct stdata *sd, const int f_flags, long *timeo, const int access)
{
	int err;

	if (unlikely((err = straccess_slow(sd, access)) != 0))
		return (err);
	if (unlikely(signal_pending(current)))
		return ((*timeo == MAX_SCHEDULE_TIMEOUT) ? -ERESTARTSYS : -EINTR);
	if (unlikely(*timeo == 0))
		return ((f_flags & FNDELAY) ? -EAGAIN : -ETIME);
	return (0);
}

STATIC mblk_t *
alloc_proto(ssize_t psize, ssize_t dsize, size_t wroff, int type, uint bpri)
{
	mblk_t *mp = NULL, *dp = NULL;

	/* yes, POSIX says we can send zero length control parts */
	if (psize >= 0) {
		ptrace(("Allocating cntl part %d bytes\n", psize));
		if ((mp = allocb(psize, bpri))) {
			mp->b_datap->db_type = type;
			mp->b_wptr = mp->b_rptr + psize;
		} else
			return (mp);
	}
	if (dsize >= 0) {
		ptrace(("Allocating data part %d bytes\n", dsize));
		if ((dp = allocb(dsize + wroff, bpri))) {
			dp->b_datap->db_type = M_DATA;
			dp->b_rptr += wroff;
			dp->b_wptr = dp->b_rptr + dsize;
			mp = linkmsg(mp, dp);
			/* STRHOLD feature in strwput uses this */
			if (psize < 0)
				dp->b_flag |= MSGDELIM;
		} else {
			if (mp)
				freemsg(mp);
			return (dp);
		}
	}
	return (mp);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Handling for SIGPOLL and STREAMS events
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  str_find_thread_group_leader: - given a thread find the thread group leader
 *  @procp:	the process
 *
 *  Finds the thread group leader for process @procp.  We want to only reference the thread group
 *  leader because the thread group leader is the only process in the thread group to delay
 *  destruction on exit (it just zombies).  Because threads in the thread group do not inform us
 *  when they die, we have to do it this way.  When the last thread in the thread group dies, the
 *  STREAM will be closed, and we will unregister from the thread group leader.  When we send
 *  SIGPOLL, we use kill_proc which selects a viable thread in the thread group.
 */
STATIC inline struct task_struct *
str_find_thread_group_leader(const struct task_struct *procp)
{
	const struct task_struct *p = procp;

	if (!thread_group_leader(p)) {
		read_lock(&tasklist_lock);
		do {
			p = next_thread(p);
			if (p == procp) {
				swerr();
				break;
			}
		} while (!thread_group_leader(p));
		read_unlock(&tasklist_lock);
	}
	return ((struct task_struct *) p);
}

/**
 *  str_find_file_descriptor: - find a file descriptor given a file pointer
 *  @procp:	the process
 *  @file:	the file pointer
 *
 *  For SIGPOLL we want to put the file descriptor in the siginfo structure so that select can be
 *  used as well as poll, and for any asynchronous I/O.  So, what we do is find the file descriptor
 *  from the file pointer by walking the process' open file descriptor list.  Unfortunately fasync
 *  does this for us, but ioctl does not.
 */
STATIC inline int
str_find_file_descriptor(const struct task_struct *procp, const struct file *file)
{
	struct files_struct *files = procp->files;
	int fd, max;

	read_lock(&files->file_lock);
	max = files->max_fdset;
	for (fd = 0; fd <= max && files->fd[fd] != file; fd++) ;
	if (fd > files->max_fdset)
		fd = ~0;
	read_unlock(&files->file_lock);
	return (fd > max ? ~0 : fd);
}

STATIC inline streams_fastcall struct strevent *
__strevent_find(const struct stdata *sd)
{
	struct task_struct *p, *c = current;
	struct strevent *se;

	for (se = sd->sd_siglist; se && ((p = se->se_procp) != c) && (p->tgid != c->tgid);
	     se = se->se_next) ;
	return (se);
}

/**
 *  __strevent_register: - register the calling process for STREAMS events
 *  @file:	current file pointer for the STREAM
 *  @sd:	the STREAM head
 *  @events:	the bitmask of events
 *
 *  When @events is zero, POSIX tells us to unregister.  This function must be called with the
 *  STREAM head write locked.  We cache the overall bitwise OR of all registered events in the
 *  sd->sd_sigflags member so that we can avoid traversing the list for ignored events.  This is
 *  faster than other implementations.
 *
 *  [EINVAL] @events was zero indicating to unregister, however, the calling process' thread group
 *           was not on the list.
 *
 *  [EAGAIN] A &strevent structure could not be allocated.
 */
STATIC streams_fastcall int
__strevent_register(const struct file *file, struct stdata *sd, const unsigned long events)
{
	struct task_struct *p, *c = current;
	struct strevent *se, **sep;
	int err = 0;

	printd(("%s: registering streams events %lu\n", __FUNCTION__, events));
	for (sep = &sd->sd_siglist;
	     (se = *sep) && ((p = se->se_procp) != c) && (p->tgid != c->tgid); sep = &se->se_next) ;
	if (se) {
		printd(("%s: found existing registration se = %p\n", __FUNCTION__, se));
		if (events) {
			/* update */
			printd(("%s: updating events se = %p\n", __FUNCTION__, se));
			se->se_events = events;
		} else {
			/* delete */
			printd(("%s: deleting events se = %p\n", __FUNCTION__, se));
			*sep = se->se_next;
			sefree(se);
		}
		{
			unsigned long new_events = 0;

			/* recalc sig flags */
			printd(("%s: recalculating events se = %p\n", __FUNCTION__, se));
			for (se = sd->sd_siglist; se; se = se->se_next)
				new_events |= se->se_events;
			sd->sd_sigflags = new_events;
			printd(("%s: new overall events %lu\n", __FUNCTION__, new_events));
		}
	} else if (!events) {
		ptrace(("Error path taken!\n"));
		err = -EINVAL;	/* POSIX not on list */
	} else if (!(se = sealloc())) {
		ptrace(("Error path taken!\n"));
		err = -EAGAIN;	/* POSIX says EAGAIN not ENOSR */
	} else {
		struct task_struct *procp;
		int fd;

		/* create */
		/* we only reference thread group leaders because they cannot unhash in our faces,
		   but will just zombie if they exit before the others in the thread group. */
		procp = str_find_thread_group_leader(c);
		/* siginfo wants a file descriptor */
		fd = str_find_file_descriptor(procp, file);
		se->se_procp = procp;
		se->se_events = events;
		se->se_fd = fd;
		printd(("%s: creating siglist events %lu, proc %p, fd %d\n", __FUNCTION__, events,
			procp, fd));
		/* calc sig flags */
		sd->sd_sigflags |= events;
		ptrace(("%s: new events %lu\n", __FUNCTION__, sd->sd_sigflags));
		/* link it in */
		se->se_next = sd->sd_siglist;
		sd->sd_siglist = se;
	}
	return (err);
}

/**
 *  strevent_register: - locking version of __strevent_register
 *  @file:	current file pointer for the STREAM
 *  @sd:	the STREAM head
 *  @events:	events to register (zero for unregister)
 *
 *  POSIX says that if the argument (events) is zero it means to remove the calling process from the
 *  list.  If the calling process is not on the list, return [EINVAL].
 */
STATIC INLINE int
strevent_register(const struct file *file, struct stdata *sd, const unsigned long events)
{
	int err;

	printd(("%s: registering streams events %lu\n", __FUNCTION__, events));
	swlock(sd);
	err = __strevent_register(file, sd, events);
	swunlock(sd);
	return (err);
}
STATIC INLINE int
strevent_unregister(const struct file *file, struct stdata *sd)
{
	printd(("%s: unregistering streams events\n", __FUNCTION__));
	return strevent_register(file, sd, 0);
}

/**
 *  strevent:	- signal events to requesting processes
 *  @sd:	the stream head
 *  @events:	bitwise OR of events to signal
 *  @band:	band number for S_RDBAND and S_WRBAND signals
 *
 *  LOCKING: Call this function with a STREAM head read or write lock held across the call.
 *
 *  Note the oddity that S_OUTPUT and S_WRNORM and the same bit.
 */
streams_fastcall STATIC void
strevent(struct stdata *sd, const int events, unsigned char band)
{
	int code;

	switch (events) {
	case (S_HIPRI):
		code = POLL_PRI;
		break;
	case (S_INPUT | S_RDNORM):
	case (S_INPUT | S_RDBAND):
		code = POLL_IN;
		break;
	case (S_WRNORM):
	case (S_WRBAND):
		code = POLL_OUT;
		break;
	case (S_MSG):
		code = POLL_MSG;
		break;
	case (S_ERROR):
		code = POLL_ERR;
		break;
	case (S_HANGUP):
		code = POLL_HUP;
		break;
	default:
		swerr();
		return;
	}
	/* check cache */
	if (sd->sd_sigflags & events) {
		struct strevent *se;
		struct siginfo si;
		int bits, sig;
		pid_t pid;

		for (se = sd->sd_siglist; se; se = se->se_next) {
			if ((bits = (se->se_events & events)) == 0)
				continue;
			if (!(bits & S_RDBAND) || !(se->se_events & S_BANDURG)) {
				si.si_signo = sig = SIGPOLL;
				si.si_code = code;
				si.si_band = band;
				si.si_fd = se->se_fd;
			} else {
				si.si_signo = sig = SIGURG;
				si.si_code = SI_KERNEL;
			}

			pid = se->se_procp->pid;

			{
#if HAVE_KILL_PROC_INFO_ADDR
				static int (*kill_proc_info) (int sig, struct siginfo * sip,
							      pid_t pid) =
				    (typeof(kill_proc_info)) HAVE_KILL_PROC_INFO_ADDR;
#endif
				/* kill_proc_info will do the right thing visa vi thread groups */
				if (kill_proc_info(sig, &si, pid))
					kill_proc(pid, sig, 1);	/* force */
			}
		}
	}
	/* do the BSD O_ASYNC list too */
	kill_fasync(&sd->sd_fasync, SIGPOLL, code);
}

/**
 *  strsignal:	- process an M_PCSIG or M_SIG message.
 *  @sd:	the stream head
 *  @mp:	the M_PCSIG or M_SIG message
 *
 *  MG 7.9.6:-
 *  "M_SIG, M_PCSIG -- these messages send signals to the process group.  The first byte
 *   of the message contains the number of the signal to send.  For an M_PCSIG message the signal is
 *   sent immediately.  For an M_SIG message it is placed on the read queue and the signal is sent
 *   when the message is read from the queue by strread().  If the signall is SIGPOLL, it will be
 *   sent only to the processes that requested it with the I_SETSIG ioctl.  Other signals are sent
 *   to a process only if the stream is associated with the control terminal (see 7.11.2)."
 *
 *  Note that this function always consumes the message @mp.
 *
 *  SVR 4 SPG:-
 *  "STREAMS allows modules and drivers to cause a signal to be sent to user process(es) through an
 *   %M_SIG or %M_PCSIG message.  The first bye of the message specifies the signal for the Stream
 *   head to generate.  If the signal is not %SIGPOLL, the signal is sent to the process group
 *   associated with the Stream.  If the signal is %SIGPOLL, the signal is only sent to processes
 *   that have registered for the signal by using the %I_SETSIG ioctl(2).
 *
 *   An %M_SIG message can be used by modules or drivers that wish to insert an explicit inband
 *   signal into a message stream.  For example, this message can be sent to the user process
 *   immediately before a particular service interface message to gain the immediate attention of
 *   the user process.  When the %M_SIG message reaches the head of the Stream head read queue, a
 *   signal is generated and the %M_SIG message is removed.  This leaves the service interface
 *   message as the next message to be processed by the user.  Use of the %M_SIG message is
 *   typically defined as part of the service interface of the driver or module."
 *
 *  SVR 4 SPG:-
 *  "M_SIG:- Sent upstream by modules and drivers to post a signal to a process. When the message
 *   reaches the front of the Stream head read queue, it evaluates the first data byte of the
 *   message as a signal number, defined in <sys/signal.h>.  (Note that the signal is not generated
 *   until it reaches the front of the Stream head read queue.)  The associated signal will be sent
 *   to process(es) under the following conditions:
 *
 *   If the signal is %SIGPOLL, it will be sent only to those processes that have explicitly
 *   registered to receive the signal [see %I_SETSIG in streamio(7)].
 *
 *   If the signal is not %SIGPOLL, and the Stream containing the sending module or driver is a
 *   controlling tty, the signal is sent to the associated process group.  A Stream becomes the
 *   controlling tty for its process group if, on open(2), a module or driver sends an %M_SETOPTS
 *   message to the Stream head with the %SO_ISTTY flag set.
 *
 *   If the signal is not %SIGPOLL and the Stream is not a controlling tty, no signal is sent,
 *   except in case of %SIOCSPGRP and %TIOCSPGRP.  These two ioctls set the process group field in the
 *   Stream head so the Stream can generate signals even if it is not a controlling tty."
 *  
 */
STATIC inline void
strsignal(struct stdata *sd, mblk_t *mp)
{
	int sig = mp->b_rptr[0];
	int band = mp->b_band;

	if (sig == SIGPOLL)
		strevent(sd, S_MSG, band);
	else if (test_bit(STRISTTY_BIT, &sd->sd_flag) || sd->sd_pgrp > 0)
		/* Note: to send SIGHUP to the session leader, use M_HANGUP. */
		kill_pg(sd->sd_pgrp, sig, 1);
	freemsg(mp);
}

/*
 *  Locked version of above.  This function is called with the Stream head read locked and the
 *  Stream frozen.  This separate version is so that we do not release and reacquire locks if not
 *  necessary.  Return true if we had to release locks.
 */
STATIC inline bool
strsignal_locked(struct stdata *sd, mblk_t *mp, unsigned long *plp)
{
	if ((mp->b_rptr[0] == SIGPOLL) || test_bit(STRISTTY_BIT, &sd->sd_flag) || sd->sd_pgrp > 0) {
		zwunlock(sd, *plp);
		srunlock(sd);
		strsignal(sd, mp);
		srlock(sd);
		*plp = zwlock(sd);
		return (true);
	}
	return (false);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Wait Queues and Sleep tests for various purposes.
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  strgetq: - get a message from a stream ala getpmsg
 *  @sd: stream head
 *  @q: stream head read queue
 *  @flags: flags from getpmsg (%MSG_HIPRI or zero)
 *  @band: band from which to retrieve message
 *  @plp: pointer to save irq flags.
 *
 *  LOCKING: This function must be called with the Stream head read locked and the Stream frozen.
 *
 *  We freeze the Stream so that we can do getq() in one place and put it back with putbq() in
 *  another (or search the queue and use rmvq()) without races with other procedures acting on the
 *  queue.  Also there needs to be atomicity between setting or clearing the STRPRI bit and placing
 *  or removing an M_PCPROTO from the queue.  The write lock acheives that atomicity too.  Perhaps
 *  we should not even use the STRPRI bit, because we could always check
 *  q->q_first->b_datap->db_type with the queue locked, but that is 3 pointer dereferences.  But,
 *  perhaps we don't need to use atomic bit operations on the STRPRI bit inside the locks.
 *
 *  NOTICES: Note that M_PCPROTO is the only priority message that we place on the stream head read
 *  queue, so if the STRPRI bit is set, there is one (and only one) there.  We leave the STRPRI bit
 *  set because we need to release locks between taking the message off and putting it back.
 *  strread() and strgetpmsg() will clear the bit if necessary after the final disposition of the
 *  message is known.
 */
STATIC inline mblk_t *
strgetq(struct stdata *sd, queue_t *q, const int flags, const int band, unsigned long *plp)
{
	mblk_t *b = NULL;

	/* like a mini service procedure */
	while ((b = q->q_first)) {
		switch (b->b_datap->db_type) {
		case M_SIG:
		{
			int err;

			if (!b->b_next || b->b_next->b_datap->db_type != M_SIG)
				clear_bit(STRMSIG_BIT, &sd->sd_flag);
			prlock(sd);
			rmvq(q, b);
			prunlock(sd);
			if (strsignal_locked(sd, b, plp)) {
				/* released and reacquired locks, recheck access */
				if ((err = straccess_slow(sd, FREAD)))
					return (ERR_PTR(err));
				if (signal_pending(current))
					return ERR_PTR(-EINTR);
			}
			continue;
		}
		case M_PCPROTO:
			if (flags == MSG_BAND)
				return ERR_PTR(-EBADMSG);
			clear_bit(STRPRI_BIT, &sd->sd_flag);
			break;
		case M_PROTO:
		case M_DATA:
			if (flags == MSG_HIPRI)
				return ERR_PTR(-EBADMSG);
			if (b->b_band < band)
				return ERR_PTR(-EBADMSG);
			break;
		default:
		case M_PASSFP:
			return ERR_PTR(-EBADMSG);
		}
		if (b->b_next && b->b_next->b_datap->db_type == M_SIG)
			set_bit(STRMSIG_BIT, &sd->sd_flag);
		prlock(sd);
		rmvq(q, b);
		prunlock(sd);
		break;
	}
	return (b);
}

STATIC inline void
strputbq(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	/* Like putbq() but handles STRPRI bit and under queue locks */
	if (mp->b_datap->db_type >= QPCTL)
		set_bit(STRPRI_BIT, &sd->sd_flag);
	if (mp->b_datap->db_type == M_SIG)
		set_bit(STRMSIG_BIT, &sd->sd_flag);
	else
		clear_bit(STRMSIG_BIT, &sd->sd_flag);
	insq(q, q->q_first, mp);
}

STATIC streams_fastcall mblk_t *
strgetq_slow(struct stdata *sd, queue_t *q, const int flags, const int band, unsigned long *plp)
{
	return strgetq(sd, q, flags, band, plp);
}

/**
 *  strwaitgetq: - wait to get a message from a stream ala getpmsg
 *  @sd: STREAM head
 *  @flags: flags from getpmsg (%MSG_HIPRI or zero)
 *  @band: priority band from which to get message
 *
 *  LOCKING: This function must be called with the Stream head read locked and the Stream frozen.
 *
 *  NOTICES: The call to this function can be pig slow: we are going to block anyway.
 *
 */
STATIC mblk_t *
strwaitgetq(struct stdata *sd, queue_t *q, const int flags, const int band, unsigned long *plp)
{
	DECLARE_WAITQUEUE(wait, current);
	mblk_t *mp;
	int err;

	add_wait_queue(&sd->sd_waitq, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		set_bit(RSLEEP_BIT, &sd->sd_flag);
		if ((err = straccess_slow(sd, FREAD))) {
			mp = ERR_PTR(err);
			break;
		}
		if (signal_pending(current)) {
			mp = ERR_PTR(-EINTR);
			break;
		}
		if ((mp = strgetq_slow(sd, q, flags, band, plp)))
			break;
		zwunlock(sd, *plp);
		srunlock(sd);
		schedule();
		srlock(sd);	/* Note access checked above. */
		*plp = zwlock(sd);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&sd->sd_waitq, &wait);
	return (mp);
}

/**
 *  strsendmread: - send an M_READ(9) message if required
 *  @sd: STREAM head
 *  @len: length of read attempt
 *
 *  LOCKING: Call this function with the Stream frozen and a Stream head read lock.
 */
STATIC int
strsendmread(struct stdata *sd, const unsigned long len, unsigned long *plp)
{
	mblk_t *b;
	int err;

	/* Note that we allocate a buffer sufficient to hold the data so that the M_READ message
	   can be easily transformed into an M_DATA message */
	/* Need to drop the locks while sleeping for a buffer. */
	zwunlock(sd, *plp);
	srunlock(sd);
	if ((b = allocb(len, BPRI_WAITOK))) {
		b->b_datap->db_type = M_READ;
		*((unsigned long *) b->b_rptr) = len;
		b->b_wptr = b->b_rptr + sizeof(&len);
		if (!(err = straccess_rlock(sd, FREAD))) {
			ctrace(put(sd->sd_wq, b));
			trace();
		} else
			freemsg(b);
	} else if (!(err = straccess_rlock(sd, FNDELAY)))	/* XXX: why FNDELAY? */
		err = -ENOSR;
	*plp = zwlock(sd);
	return (err);
}

/**
 *  strtestgetq: - get a message from the read queue or wait
 *  @sd:	STREAM head
 *  @q:		STREAM head read queue
 *  @f_flags:	file flags
 *  @flags:	getpmsg flags
 *  @band:	priority band number
 *  @len:	length of read request
 *
 *  CONTEXT: Call this function with a len of zero (explicit constant) and the mread check should be
 *  inlined out for calls subsequent to the first one.
 *
 *  LOCKING: This function must be called with the Stream read locked and the Stream frozen.
 *
 *  NOTICES: Strangely, Magic Garden Chapter 7 says that ldterm sets the stream head into raw mode,
 *  that an M_READ message is issued downstream with every read and that ldterm responds immediately
 *  with a 0 length message to create a short read.  So that is one M_READ for each and every write.
 *  UnixWare says to send an M_DATA containing the number of bytes of read request downstream
 *  whenever, the driver or module has requested it with SO_MREADON in M_SETOPTS, no data is
 *  currently available at the stream head, and the reader is about to sleep.  The following
 *  satisfies the later.
 *
 *  AIX ldterm documentation says that "[t]he M_READ is sent by the stream head to noitfy downstream
 *  modules when an application has issued a read request and not enough data is queued at the
 *  stream head to satisfy the request.  The message contains the number of characters requested by
 *  the application.  If the request can be satisfied, the M_READ message block is transformed into
 *  an M_DATA block with the requested data appended.  This message is then forwarded upstream."
 *  This appears consistent with the UnixWare description.
 *
 *  HPUX says, the M_READ "message is sent by the stream head to notify downstream modules when an
 *  application has issued a read request and there is not enough data queued at the stream head to
 *  satsify the request.  The M_READ is sent downstream normally when ldterm is operating in
 *  non-cannonical input mode.
 *
 *  SUX says, that M_READ is a "[h]igh priority control message used to indicate the occurence of a
 *  read(2) when there are no data on the stream head read queue." And, "M_READ is generated by the
 *  stream head and sent downstream for a read(2) system call if no messages are waiting to be read
 *  at the stream head and if read notification has been enabled.  Read notification is enabled with
 *  SO_MREADON flag of M_SETOPTS message and disabled by use of the SO_MREADOFF flag.   The message
 *  content is set to the value of the nbyte paramter (the number of bytes to be read) in the
 *  read(2) call.  M_READ is intended to notify modules and drivers of the occurence of a read.  It
 *  is also intended to support communication between streams that reside in separate processors.
 *  The use of the M_READ message is developer dependent.  Modules may take specific action and pass
 *  on or free the M_READ message.  Modules that do not recognize this message must pass it on.  All
 *  other drivers may or may not take action and then free the message."
 */
STATIC inline mblk_t *
strtestgetq(struct stdata *sd, queue_t *q, const int f_flags, const int flags, const int band,
	    const unsigned long len, unsigned long *plp)
{
	mblk_t *mp;
	int err;

	/* maybe we'll get lucky... */
	/* also we need to trigger QWANTR bit and empty queue backenabling */
	if ((mp = strgetq(sd, q, flags, band, plp)))
		return (mp);

	/* only here it there's nothing left on the queue */

	/* check hangup blocking criteria */
	if ((err = straccess_slow(sd, FREAD)))
		return ERR_PTR(err);

	if (signal_pending(current))
		return ERR_PTR(-ERESTARTSYS);

	/* check nodelay */
	if ((f_flags & FNDELAY))
		return ERR_PTR(-EAGAIN);

	/* also we need to trigger M_READ messages, but only if blocking for first time */
	/* about to block, generate M_READ(9) if required */
	if (len != 0 && test_bit(SNDMREAD_BIT, &sd->sd_flag))
		if ((err = strsendmread(sd, len, plp)))
			return ERR_PTR(err);

	return strwaitgetq(sd, q, flags, band, plp);
}

/**
 *  strgetfp: - get a file pointer from a stream
 *  @sd: stream head
 *  @q: stream head read queue
 *  @plp: pointer to save irq flags.
 *
 *  LOCKING: This function must be called with the Stream head read locked and the Stream frozen.
 *
 *  We freeze the Stream so that we can do getq() in one place and put it back with putbq() in
 *  another (or search the queue and use rmvq()) without races with other procedures acting on the
 *  queue.  Also there needs to be atomicity between setting or clearing the STRPRI bit and placing
 *  or removing an M_PCPROTO from the queue.  The write lock acheives that atomicity too.  Perhaps
 *  we should not even use the STRPRI bit, because we could always check
 *  q->q_first->b_datap->db_type with the queue locked, but that is 3 pointer dereferences.  But,
 *  perhaps we don't need to use atomic bit operations on the STRPRI bit inside the locks.
 */
STATIC inline mblk_t *
strgetfp(struct stdata *sd, queue_t *q, unsigned long *plp)
{
	mblk_t *b = NULL;

	/* like a mini service procedure */
	while ((b = q->q_first)) {
		switch (b->b_datap->db_type) {
		case M_SIG:
		{
			int err;

			if (!b->b_next || b->b_next->b_datap->db_type != M_SIG)
				clear_bit(STRMSIG_BIT, &sd->sd_flag);
			prlock(sd);
			rmvq(q, b);
			prunlock(sd);
			if (strsignal_locked(sd, b, plp)) {
				/* release and reacquire locks, recheck access */
				if ((err = straccess_slow(sd, FREAD)))
					return ERR_PTR(err);
				if (signal_pending(current))
					return ERR_PTR(-EINTR);
			}
			continue;
		}
		case M_PASSFP:
			break;
		default:
		case M_PCPROTO:
		case M_PROTO:
		case M_DATA:
			return ERR_PTR(-EBADMSG);
		}
		prlock(sd);
		rmvq(q, b);
		prunlock(sd);
		break;
	}
	return (b);
}

STATIC streams_fastcall mblk_t *
strgetfp_slow(struct stdata *sd, queue_t *q, unsigned long *plp)
{
	return strgetfp(sd, q, plp);
}

STATIC mblk_t *
strwaitgetfp(struct stdata *sd, queue_t *q, unsigned long *plp)
{
	DECLARE_WAITQUEUE(wait, current);
	mblk_t *mp;
	int err;

	add_wait_queue(&sd->sd_waitq, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		set_bit(RSLEEP_BIT, &sd->sd_flag);
		if ((err = straccess_slow(sd, FREAD))) {
			mp = ERR_PTR(err);
			break;
		}
		if (signal_pending(current)) {
			mp = ERR_PTR(-EINTR);
			break;
		}
		if ((mp = strgetfp_slow(sd, q, plp)))
			break;
		zwunlock(sd, *plp);
		srunlock(sd);
		schedule();
		srlock(sd);	/* Note access checked above. */
		*plp = zwlock(sd);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&sd->sd_waitq, &wait);
	return (mp);
}

STATIC inline mblk_t *
strtestgetfp(struct stdata *sd, queue_t *q, const int f_flags, unsigned long *plp)
{
	mblk_t *mp;
	int err;

	/* maybe we get lucky... */
	if ((mp = strgetfp(sd, q, plp)))
		return (mp);

	/* only here if there is nothing left on the queue */

	/* check hangup blocking criteria */
	if ((err = straccess_slow(sd, FREAD)))
		return ERR_PTR(err);

	/* check nodelay */
	if ((f_flags & FNDELAY))
		return ERR_PTR(-EAGAIN);

	if (signal_pending(current))
		return ERR_PTR(-ERESTARTSYS);

	return strwaitgetfp(sd, q, plp);
}

/**
 *  __strwaitband: - timed wait to perform write on a flow controlled band
 *  @sd: stream head
 *  @band: band number
 *
 *  DESCRIPTION: Wait for the specified band to pass flow control restrictions.  Test for flow
 *  control before calling this function.  Call it with the STREAM head at least read locked and
 *  write access already checked.
 *
 *  LOCKING: Must be called with a single read lock held on the STREAM head.
 */
STATIC int
__strwaitband(struct stdata *sd, int band)
{
	/* wait for band to become available */
	DECLARE_WAITQUEUE(wait, current);
	int err = 0;

	add_wait_queue(&sd->sd_waitq, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if ((err = straccess_slow(sd, FWRITE)))
			break;
		if (signal_pending(current)) {
			err = -EINTR;
			break;
		}
		/* have read lock and access is ok */
		prlock(sd);
		if (bcanputnext(sd->sd_wq, band)) {
			prunlock(sd);
			break;
		}
		prunlock(sd);
		set_bit(WSLEEP_BIT, &sd->sd_flag);
		srunlock(sd);
		schedule();
		srlock(sd);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&sd->sd_waitq, &wait);
	return (err);
}

/**
 *  strwaitband: - check whether we have to sleep waiting for a band
 *  @sd: stream head
 *  @f_flags: file flags
 *  @band: band number
 *  @flags: message flags
 *
 *  DESCRIPTION:  Inline checks leading up to blocking.  POSIX blocking semantics for write(),
 *  putmsg(), putpmsg() and %I_FDINSERT ioctl().
 *
 *  POSIX putmsg(2)/putpmsg(2) says "The putmsg() function shall block if the STREAM write queue is
 *  full due to internal flow control conditions, with the following exceptions: - For high-priority
 *  messages, putmsg() shall not block on this condition and continues processing the message. - For
 *  other messages, putmsg() shall not block but shall fail when the write queue is full and
 *  O_NONBLOCK is set.
 *
 *  POSIX ioctl(2) says "For non-priority messages, I_FDINSERT shall block if the STREAM write queue
 *  is full due to internal flow control conditions.  For priority messages, I_FDINSERT does not
 *  block on this condition.  For non-priority messages, I_FDINSERT does not block when the write
 *  queue is full and O_NONBLOCK is set.  Instead, it fails and sets errno to [EAGAIN].
 *
 *  Never check flow control or block for high priority messages.  For normal messages, check the
 *  band.  Do not sleep if FNDELAY is set unless we are doing old TTY semantics (that always blocks
 *  on write regardless of the setting of FNDELAY).
 *
 *  LOCKING: must hold a read lock on the stream head.
 */
STATIC inline streams_fastcall int
strwaitband(struct stdata *sd, const int f_flags, const int band, const int flags)
{
	if (flags == MSG_HIPRI)
		return (0);

	/* have read lock and access was ok */
	prlock(sd);
	if (bcanputnext(sd->sd_wq, band)) {
		prunlock(sd);
		return (0);
	}
	prunlock(sd);

	if ((f_flags & FNDELAY) && !test_bit(STRNDEL_BIT, &sd->sd_flag))
		return (-EAGAIN);

	if (signal_pending(current))
		return (-ERESTARTSYS);

	return __strwaitband(sd, band);
}

/*
 *  __strwaitopen: - wait to open a stream head
 *  @sd: stream head
 *  @access: access to check
 *
 *  LOCKING: call with no locks held.
 */
STATIC int
__strwaitopen(struct stdata *sd, const int access)
{
	DECLARE_WAITQUEUE(wait, current);
	int err;

	add_wait_queue(&sd->sd_waitq, &wait);
	/* Wait for the STWOPEN bit.  Only one open can be performed on a stream at a time. See
	   Magic Garden. */
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if ((err = straccess_slow(sd, access)))
			break;
		if (signal_pending(current)) {
			err = -EINTR;
			break;
		}
		if (!test_and_set_bit(STWOPEN_BIT, &sd->sd_flag))
			break;
		swunlock(sd);
		schedule();
		swlock(sd);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&sd->sd_waitq, &wait);
	return (err);
}

/*
 *  strwaitopen: - wait to open a stream head
 *  @sd: stream head
 *
 *  LOCKING: acquires a STREAM head write lock that will be held on success and dropped on failure.
 */
STATIC inline int
strwaitopen(struct stdata *sd, const int access)
{
	int err;

	if (signal_pending(current))
		return (-ERESTARTSYS);
	if (!(err = straccess_wlock(sd, access))) {
		if (!test_and_set_bit(STWOPEN_BIT, &sd->sd_flag))
			return (0);
		if ((err = __strwaitopen(sd, access)))
			swunlock(sd);
	}
	return (err);
}

STATIC void strlastclose(struct stdata *sd, int oflag);

STATIC inline streams_fastcall void
strwakeopen_swunlock(struct stdata *sd)
{
	bool detached;

	/* release open bit */
	clear_bit(STWOPEN_BIT, &sd->sd_flag);
	if (!(detached = strdetached(sd))
	    && waitqueue_active(&sd->sd_waitq))
		wake_up_interruptible(&sd->sd_waitq);
	swunlock(sd);
	if (detached) {
		strremove(sd);
		strlastclose(sd, 0);
	}
}

/**
 *  strwakeopen: - wake waiters waiting to open a stream
 *  @sd: stream head
 */
STATIC inline streams_fastcall void
strwakeopen(struct stdata *sd)
{
	/* release open bit */
	swlock(sd);
	strwakeopen_swunlock(sd);
}

STATIC inline streams_fastcall void
strwakepoll(struct stdata *sd)
{
	if (waitqueue_active(&sd->sd_polllist))
		wake_up_interruptible(&sd->sd_polllist);
}

STATIC inline streams_fastcall void
strwakeread(struct stdata *sd)
{
	if (test_and_clear_bit(RSLEEP_BIT, &sd->sd_flag))
		wake_up_interruptible(&sd->sd_waitq);
	strwakepoll(sd);
}

STATIC inline streams_fastcall void
strwakewrite(struct stdata *sd)
{
	if (test_and_clear_bit(WSLEEP_BIT, &sd->sd_flag))
		wake_up_interruptible(&sd->sd_waitq);
	strwakepoll(sd);
}

STATIC inline streams_fastcall void
strwakeiocwait(struct stdata *sd)
{
	if (test_bit(IOCWAIT_BIT, &sd->sd_flag))
		wake_up_interruptible(&sd->sd_waitq);
}

STATIC inline streams_fastcall void
strwakeall(struct stdata *sd)
{
	bool wake = false;

	if (test_and_clear_bit(RSLEEP_BIT, &sd->sd_flag))
		wake = true;
	if (test_and_clear_bit(WSLEEP_BIT, &sd->sd_flag))
		wake = true;
	if (test_bit(IOCWAIT_BIT, &sd->sd_flag))
		wake = true;
	if (wake && waitqueue_active(&sd->sd_waitq))
		wake_up_interruptible(&sd->sd_waitq);
	strwakepoll(sd);
}

/**
 *  __strwaitfifo: - wait for readers or writers of a FIFO on open
 *  @sd: FIFO stream head
 *  @oflag:	file flags to open()
 *
 *  POSIX blocking semantics for open of a FIFO.  Wait until there is at least one reader and one
 *  writer of the FIFO.  For efficiency don't call this function when FNDELAY is set, or when
 *  FREAD|FWRITE is set.  Note that we no longer hold the open bit nor hold any locks.
 */
STATIC int
__strwaitfifo(struct stdata *sd, const int oflag)
{
	DECLARE_WAITQUEUE(wait, current);
	int err = 0;

	add_wait_queue(&sd->sd_waitq, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if ((err = straccess_slow(sd, FCREAT)))
			break;
		if (signal_pending(current)) {
			err = -EINTR;
			break;
		}
		if (sd->sd_readers >= 1 && sd->sd_writers >= 1)
			break;
		schedule();
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&sd->sd_waitq, &wait);
	return (err);
}

STATIC inline int
strwaitfifo(struct stdata *sd, const int oflag)
{
	if ((oflag & (FREAD | FWRITE)) == (FREAD | FWRITE))
		return (0);

	if (sd->sd_readers >= 1 && sd->sd_writers >= 1)
		return (0);

	if ((oflag & (FNDELAY | FREAD)) == (FNDELAY | FREAD))
		return (0);

	if ((oflag & (FNDELAY | FWRITE)) == (FNDELAY | FWRITE))
		return (-ENXIO);

	return __strwaitfifo(sd, oflag);
}

STATIC void
strwaitqueue(struct stdata *sd, queue_t *q)
{
	DECLARE_WAITQUEUE(wait, current);
	long timeo = sd->sd_closetime;
	struct queinfo *qu = ((struct queinfo *) _RD(q));

	set_bit(QWCLOSE_BIT, &q->q_flag);
	add_wait_queue(&qu->qu_qwait, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (timeo == 0)
			break;
		if (!q->q_first)
			break;
		if (signal_pending(current))
			break;
		timeo = schedule_timeout(timeo);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&qu->qu_qwait, &wait);
	clear_bit(QWCLOSE_BIT, &q->q_flag);
}

/**
 *  strwaitclose:   - wait for queues to drain on close
 *  @sd:	STREAM head that is closing
 *  @oflag:	open flags for the last file pointer (closing file pointer)
 *
 *  If STRHOLD is set, then be sure to release any messages held on the stream head's write queue,
 *  so, start with the stream head write queue instead of the stream below the stream head write
 *  queue.
 */
STATIC INLINE streams_fastcall void
strwaitclose(struct stdata *sd, int oflag)
{
	cred_t *crp;
	queue_t *q;
	bool wait;

	assert(sd);
	q = sd->sd_wq;
	assert(q);

	/* POSIX close() semantics for STREAMS */
	wait = (!(oflag & FNDELAY) && (sd->sd_closetime != 0) && !signal_pending(current));

	/* STREAM head first */
	if (wait && q->q_first)
		ctrace(strwaitqueue(sd, q));
	crp = current_creds;
	while ((q = SAMESTR(sd->sd_wq) ? sd->sd_wq->q_next : NULL)) {
		if (wait && q->q_first)
			ctrace(strwaitqueue(sd, q));
		ctrace(qdetach(_RD(q), oflag, crp));
		trace();
	}
	/* STREAM head last */
	ctrace(qdetach(sd->sd_rq, oflag, crp));
	/* procs are off for the STREAM head, flush queues now to dump M_PASSFP. */
	flushq(sd->sd_rq, FLUSHALL);
	flushq(sd->sd_wq, FLUSHALL);
	trace();
}

/**
 *  __strwaitioctl:   - wait for the IOCWAIT bit on a stream
 *  @sd:	STREAM head
 *  @timeo:	remaining wait time
 *
 *  LOCKING: call with no locks held.
 */
STATIC int
__strwaitioctl(struct stdata *sd, unsigned long *timeo, int access)
{
	DECLARE_WAITQUEUE(wait, current);
	int err = 0;

	add_wait_queue(&sd->sd_waitq, &wait);
	/* Wait for the IOCWAIT bit.  Only one ioctl can be performed on a stream at a time. See
	   Magic Garden.  However, only wait as long as we would have waited for a response to our
	   input output control. */
	if (timeo) {
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if ((err = straccess_wakeup(sd, 0, timeo, access)))
				break;
			if (!test_and_set_bit(IOCWAIT_BIT, &sd->sd_flag))
				break;
			srunlock(sd);
			*timeo = schedule_timeout(*timeo);
			srlock(sd);
		}
	} else {
		for (;;) {
			set_current_state(TASK_UNINTERRUPTIBLE);
			if (!test_and_set_bit(IOCWAIT_BIT, &sd->sd_flag))
				break;
			srunlock(sd);
			schedule();
			srlock(sd);
		}
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&sd->sd_waitq, &wait);
	return (err);
}

STATIC inline int
strwaitioctl(struct stdata *sd, unsigned long *timeo, int access)
{
	int err;

	if (!(err = straccess_rlock(sd, access))) {
		if (!test_and_set_bit(IOCWAIT_BIT, &sd->sd_flag))
			return (0);
		if ((err = __strwaitioctl(sd, timeo, access)))
			srunlock(sd);
	}
	return (err);
}

STATIC inline streams_fastcall void
strwakeioctl(struct stdata *sd)
{
	mblk_t *mp;

	/* outdate old id */
	++sd->sd_iocid;
	/* clean out remaining blocks */
	if ((mp = xchg(&sd->sd_iocblk, NULL))) {
		swerr();
		ctrace(freemsg(mp));
	}
	clear_bit(IOCWAIT_BIT, &sd->sd_flag);
	if (waitqueue_active(&sd->sd_waitq))
		wake_up_all(&sd->sd_waitq);
	srunlock(sd);		/* to balance strwaitioctl */
}

STATIC mblk_t *
__strwaitiocack(struct stdata *sd, unsigned long *timeo, int access)
{
	DECLARE_WAITQUEUE(wait, current);
	mblk_t *mp;
	int err;

	/* We are waiting for a response to our downwards ioctl message.  Wait until the message
	   arrives or the io check fails. */
	add_wait_queue(&sd->sd_waitq, &wait);
	if (timeo) {
		do {
			set_current_state(TASK_INTERRUPTIBLE);
			if ((err = straccess_wakeup(sd, 0, timeo, access))) {
				mp = ERR_PTR(err);
				break;
			}
			if ((mp = xchg(&sd->sd_iocblk, NULL)))
				break;
			srunlock(sd);
			*timeo = schedule_timeout(*timeo);
			srlock(sd);
		} while (1);
	} else {
		/* timeo is NULL for no timeout no signals */
		do {
			set_current_state(TASK_UNINTERRUPTIBLE);
			if ((mp = xchg(&sd->sd_iocblk, NULL)))
				break;
			srunlock(sd);
			schedule();
			srlock(sd);
		} while (1);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&sd->sd_waitq, &wait);
	return (mp);
}

STATIC inline mblk_t *
strwaitiocack(struct stdata *sd, unsigned long *timeo, int access)
{
	mblk_t *mp;

	/* might already have a response */
	if ((mp = xchg(&sd->sd_iocblk, NULL)))
		return (mp);

	return __strwaitiocack(sd, timeo, access);
}

STATIC streams_fastcall int
strwakeiocack(struct stdata *sd, mblk_t *mp)
{
	ptrace(("%s: received ioctl response\n", __FUNCTION__));
	if (!sd->sd_iocblk && test_bit(IOCWAIT_BIT, &sd->sd_flag)) {
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;

		if (ioc->iocblk.ioc_id == sd->sd_iocid) {
			if ((mp = xchg(&sd->sd_iocblk, mp))) {
				swerr();
				ctrace(freemsg(mp));
			}
			/* might not be sleeping yet */
			if (waitqueue_active(&sd->sd_waitq))
				wake_up_all(&sd->sd_waitq);
			return (true);
		} else
			ptrace(("%s: ioctl response has wrong iocid\n", __FUNCTION__));
	} else
		ptrace(("%s: not expecting ioctl response\n", __FUNCTION__));
	return (false);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Handling for sending M_IOCTL
 *
 *  -------------------------------------------------------------------------
 */

STATIC int
strbcopyin(const void *from, void *to, size_t len, bool user)
{
	if (user)
		return copyin(from, to, len);
	bcopy(from, to, len);
	return (0);
}

STATIC int
strbcopyout(const void *from, void *to, size_t len, bool user)
{
	if (user)
		return copyout(from, to, len);
	bcopy(from, to, len);
	return (0);
}

STATIC int
strcopyinb(const caddr_t uaddr, mblk_t **dpp, size_t len, bool user)
{
	mblk_t *dp;

	if (!len)
		return (0);
	if (user && !access_ok(VERIFY_READ, uaddr, len))
		return (-EFAULT);
	if (!(dp = *dpp) || (dp->b_datap->db_lim < dp->b_rptr + len)) {
		/* no room in current buffer */
		if (dp)
			ctrace(freemsg(dp));
		/* get a new data buffer -- use the one we just freed if possible, or block */
		if (!((*dpp) = dp = allocb(len, BPRI_WAITOK)))
			return (-ENOSR);
	}
	if (strbcopyin(uaddr, dp->b_rptr, len, user))
		return (-EFAULT);
	dp->b_wptr = dp->b_rptr + len;
	return (0);
}

STATIC int
strcopyoutb(mblk_t *dp, caddr_t uaddr, size_t len, int user)
{
	if (!len)
		return (0);
	if (!dp)
		return (-EIO);
	if (user && !access_ok(VERIFY_WRITE, uaddr, len))
		return (-EFAULT);
	if (dp->b_wptr < dp->b_rptr + len)
		return (-EMSGSIZE);
	{
		const caddr_t b_rptr = (caddr_t) dp->b_rptr;

		if (strbcopyout(b_rptr, uaddr, len, user))
			return (-EFAULT);
	}
	return (0);
}

/**
 *  strmcopyout: - copy out a buffer chain
 *  @mp: buffer chain to copy out
 *  @uaddr: user address to copy to
 *  @len: maximum length to copy
 *  @protodis: only M_DATA messages
 *  @user: to user space (otherwise kernel)
 *
 *  RETURN: Returns the number of bytes copyied out.  All message blocks in the passed in message
 *  are freed.
 */
STATIC size_t
strmcopyout(mblk_t *mp, caddr_t uaddr, size_t len, bool protdis, bool user)
{
	mblk_t *b, *b_cont;
	size_t copied = 0;

	for (b_cont = mp; (b = b_cont); b_cont = unlinkb(b), freeb(b)) {
		ssize_t blen, dlen;

		b_cont = b->b_cont;
		if ((blen = b->b_wptr - b->b_rptr) <= 0)
			continue;
		if (protdis && b->b_datap->db_type != M_DATA)
			continue;
		if ((dlen = min(blen, len - copied)) > 0) {
			const caddr_t b_rptr = (caddr_t) b->b_rptr;

			strbcopyout(b_rptr, uaddr, dlen, user);
			uaddr += dlen;
			copied += dlen;
		}
	}
	return (copied);
}

STATIC int
strdoioctl_str(struct stdata *sd, struct strioctl *ic, const int access, const bool user)
{
	union ioctypes *ioc;
	mblk_t *mb;
	long timeo;
	int err = 0;

	if (ic->ic_len < 0 || ic->ic_len > sysctl_str_strmsgsz)
		/* POSIX less than zero or larger than maximum data part. */
		return (-EINVAL);

	if (!access_ok(VERIFY_WRITE, ic->ic_dp, ic->ic_len))
		return (-EFAULT);

	while (!(mb = allocb(sizeof(*ioc), BPRI_WAITOK))) ;

	mb->b_datap->db_type = M_IOCTL;

	ioc = (typeof(ioc)) mb->b_rptr;
	mb->b_wptr = mb->b_rptr + sizeof(*ioc);

	bzero(ioc, sizeof(*ioc));
	ioc->iocblk.ioc_cmd = ic->ic_cmd;
	ioc->iocblk.ioc_cr = current_creds;
	ioc->iocblk.ioc_count = ic->ic_len;

	/* implicit copyin for I_STR ioctl */
	if ((err = strcopyinb(ic->ic_dp, &mb->b_cont, ic->ic_len, user))) {
		freemsg(mb);
		return (err);
	}

	if (ic->ic_timout == INFTIM) {
		timeo = MAX_SCHEDULE_TIMEOUT;
	} else if (ic->ic_timout == 0) {
		timeo = sd->sd_ioctime;
	} else if (ic->ic_timout < INFTIM) {
		/* POSIX says if ic_timout < -1 return EINVAL */
		freemsg(mb);
		return (-EINVAL);
	} else {
		timeo = ic->ic_timout * HZ;
	}

	if ((err = strwaitioctl(sd, &timeo, access))) {
		freemsg(mb);
		return (err);
	}

	/* protected by IOCWAIT bit */
	ioc->iocblk.ioc_id = sd->sd_iocid;

	do {
		ctrace(put(sd->sd_wq, mb));
		trace();

		ptrace(("waiting for response\n"));
		mb = strwaitiocack(sd, &timeo, access);

		if (IS_ERR(mb)) {
			err = PTR_ERR(mb);
			ptrace(("Error path taken! err = %d\n", err));
			break;
		}

		assert(mb);

		ioc = (typeof(ioc)) mb->b_rptr;

		assert(ioc);

		switch (mb->b_datap->db_type) {
		case M_IOCNAK:
			err = ioc->iocblk.ioc_error;
			/* SVR 4 SPG says if error is zero return EINVAL */
			if (err == 0)
				err = EINVAL;
			/* must return negative errors */
			err = err > 0 ? -err : err;
			ptrace(("Error path taken! err = %d\n", err));
			break;
		case M_IOCACK:
			/* implicit copyout for I_STR ioctl */
			if (ioc->iocblk.ioc_count > 0) {
				if (!(err = strcopyoutb(mb->b_cont, ic->ic_dp,
							ioc->iocblk.ioc_count, user))) {
					ic->ic_len = ioc->iocblk.ioc_count;
					err = ioc->iocblk.ioc_rval;
				} else
					ptrace(("Error path taken! err = %d\n", err));
			} else
				err = ioc->iocblk.ioc_rval;
			break;
		case M_COPYIN:
		case M_COPYOUT:
			/* yes we permit I_STR ioctls to do this */
			if (mb->b_datap->db_type == M_COPYIN)
				err = strcopyinb(ioc->copyreq.cq_addr, &mb->b_cont,
						 ioc->copyreq.cq_size, user);
			else
				err = strcopyoutb(mb->b_cont, ioc->copyreq.cq_addr,
						  ioc->copyreq.cq_size, user);
			mb->b_datap->db_type = M_IOCDATA;
			if (!err) {
				ioc->copyresp.cp_rval = (caddr_t) 0;
				continue;
			}
			/* SVR 4 SPG says no response to M_IOCDATA with error */
			ioc->copyresp.cp_rval = (caddr_t) 1;
			ctrace(put(sd->sd_wq, mb));
			trace();
			goto abort;
		default:
			never();
			err = -EIO;
			break;
		}
		freemsg(mb);
		break;
	} while (1);
      abort:
	strwakeioctl(sd);
	return (err);
}

STATIC int
strdoioctl_trans(struct stdata *sd, unsigned int cmd, unsigned long arg, const int access,
		 const bool user)
{
	union ioctypes *ioc;
	mblk_t *mb, *db;
	long timeo;
	int err = 0;

	while (!(mb = allocb(sizeof(*ioc), BPRI_WAITOK))) ;

	mb->b_datap->db_type = M_IOCTL;

	ioc = (typeof(ioc)) mb->b_rptr;
	mb->b_wptr = mb->b_rptr + sizeof(*ioc);

	bzero(ioc, sizeof(*ioc));
	ioc->iocblk.ioc_cmd = cmd;
	ioc->iocblk.ioc_cr = current_creds;
	ioc->iocblk.ioc_count = TRANSPARENT;

	/* no implicit copyin for TRANPARENT ioctl */
	while (!(db = allocb(sizeof(arg), BPRI_WAITOK))) ;

	db->b_datap->db_type = M_DATA;

	*((typeof(&arg)) db->b_rptr) = arg;
	db->b_wptr = db->b_rptr + sizeof(arg);

	mb->b_cont = db;

	timeo = sd->sd_ioctime;

	if ((err = strwaitioctl(sd, &timeo, access))) {
		ctrace(freemsg(mb));
		return (err);
	}

	/* protected by IOCWAIT bit */
	ioc->iocblk.ioc_id = sd->sd_iocid;

	do {
		ctrace(put(sd->sd_wq, mb));
		trace();

		mb = strwaitiocack(sd, &timeo, access);

		if (IS_ERR(mb)) {
			err = PTR_ERR(mb);
			break;
		}

		ioc = (typeof(ioc)) mb->b_rptr;
		switch (mb->b_datap->db_type) {
		case M_IOCNAK:
			err = ioc->iocblk.ioc_error;
			/* SVR 4 SPG says if error is zero return EINVAL */
			if (err == 0)
				err = EINVAL;
			/* must return negative errors */
			err = err > 0 ? -err : err;
			ptrace(("Error path taken! err = %d\n", err));
			break;
		case M_IOCACK:
			/* no implicit copyout for TRANPARENT ioctl */
			err = ioc->iocblk.ioc_rval;
			break;
		case M_COPYIN:
		case M_COPYOUT:
			if (mb->b_datap->db_type == M_COPYIN)
				err = strcopyinb(ioc->copyreq.cq_addr, &mb->b_cont,
						 ioc->copyreq.cq_size, user);
			else
				err = strcopyoutb(mb->b_cont, ioc->copyreq.cq_addr,
						  ioc->copyreq.cq_size, user);
			mb->b_datap->db_type = M_IOCDATA;
			if (!err) {
				ioc->copyresp.cp_rval = (caddr_t) 0;
				continue;
			}
			/* SVR 4 SPG says no response to M_IOCDATA with error */
			ioc->copyresp.cp_rval = (caddr_t) 1;
			ctrace(put(sd->sd_wq, mb));
			trace();
			goto abort;
		default:
			never();
			err = -EIO;
			break;
		}
		ctrace(freemsg(mb));
		break;
	} while (1);
      abort:
	strwakeioctl(sd);
	return (err);

}

/**
 *  strdoioctl_link: - perform an %I_LINK, %I_PLINK, %I_UNLINK, %I_PUNLINK ioctl
 *  @sd: multiplexing driver control STREAM head
 *  @l: prepared link block
 *  @cmd: %I_LINK, %I_PLINK, %I_UNLINK or %I_PUNLINK
 *
 *  Perform an %I_LINK, %I_PLINK, %I_UNLINK or %I_PUNLINK ioctl as part of the user linking or
 *  unlinking a STREAM underneath a multiplexing driver.  This function blocks and can return an
 *  error.  It will not be interrupted after the M_IOCTL has been sent.  It should be called with no
 *  locks held.
 */
STATIC int
strdoioctl_link(const struct file *file, struct stdata *sd, struct linkblk *l, unsigned int cmd,
		const int access)
{
	union ioctypes *ioc;
	struct linkblk *lbp;
	mblk_t *mb, *db;
	long timeo;
	int err = 0;

	while (!(mb = allocb(sizeof(*ioc), BPRI_WAITOK))) ;

	mb->b_datap->db_type = M_IOCTL;

	ioc = (typeof(ioc)) mb->b_rptr;
	mb->b_wptr = mb->b_rptr + sizeof(*ioc);

	bzero(ioc, sizeof(*ioc));
	ioc->iocblk.ioc_cmd = cmd;
	ioc->iocblk.ioc_cr = current_creds;
	ioc->iocblk.ioc_count = sizeof(*l);

	while (!(db = allocb(sizeof(*lbp), BPRI_WAITOK))) ;

	db->b_datap->db_type = M_DATA;

	lbp = (typeof(lbp)) db->b_rptr;
	db->b_wptr = db->b_rptr + sizeof(*lbp);

	bzero(lbp, sizeof(*lbp));
	lbp->l_index = l->l_index;
	lbp->l_qtop = l->l_qtop;
	lbp->l_qbot = l->l_qbot;

	mb->b_cont = db;

	timeo = sd->sd_ioctime;

	if ((err = strwaitioctl(sd, &timeo, access))) {
		freemsg(mb);
		return (err);
	}

	/* protected by IOCWAIT bit */
	ioc->iocblk.ioc_id = sd->sd_iocid;

	do {
		/* could we place it directly to qtop? */
		ctrace(put(sd->sd_wq, mb));
		trace();

		mb = strwaitiocack(sd, NULL, access);

		switch (mb->b_datap->db_type) {
		case M_IOCNAK:
			ioc = (typeof(ioc)) mb->b_rptr;
			err = ioc->iocblk.ioc_error;
			/* SVR 4 SPG says return EINVAL, but for I_LINK return ENXIO */
			if (err == 0)
				err = ENXIO;
			/* must return negative errors */
			err = err > 0 ? -err : err;
			ptrace(("Error path taken! err = %d\n", err));
			break;
		case M_IOCACK:
			err = 0;
			break;
		case M_COPYIN:
		case M_COPYOUT:
			/* wrong response */
			ioc = (typeof(ioc)) mb->b_rptr;
			mb->b_datap->db_type = M_IOCDATA;
			/* SVR 4 SPG says no response to M_IOCDATA with error */
			ioc->copyresp.cp_rval = (caddr_t) 1;
			ctrace(put(sd->sd_wq, mb));
			trace();
			goto abort;
		default:
			never();
			err = -EIO;
			break;
		}
		freemsg(mb);
		break;
	} while (1);
      abort:
	strwakeioctl(sd);
	return (err);
}

/**
 *  strirput: - intercept IOCTL response messages 
 *  @sd: STREAM head
 *  @q: STREAM head read queue
 *  @mp: received message
 *
 *  Intercpet M_IOCACK, M_IOCNAK messages on a multiplexed queue pair.  When sending the M_IOCTL for
 *  an I_UNLINK or I_PUNLINK operation, there is no stream head attached to process the response.
 */
STATIC int
strirput(queue_t *q, mblk_t *mp)
{
	struct stdata *sd;

	assert(q);
	assert(mp);

	sd = qstream(q);

	assert(sd);

	switch (mp->b_datap->db_type) {
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		if (ctrace(strwakeiocack(sd, mp)))
			return (0);
	}
	return ((*sd->sd_muxinit->qi_putp) (q, mp));
}

/**
 *  strdoioctl_unlink: - perform an I_UNLINK ioctl
 *  @sd: multiplexing driver control STREAM head
 *  @l: prepared link block
 *
 *  Perform an I_UNLINK ioctl to unlink temporary links as part of an active or detached close of a
 *  STREAM.  This ioctl waits forever and returns no errors.  The multiplexing driver MUST
 *  acknowledge all I_UNLINK M_IOCTL messages with an M_IOCACK or M_IOCNAK.  This function is called
 *  with no locks held and with the STRCLOSE bit set on the control STREAM head.  All normal ioctl
 *  operations MUST check the STRCLOSE flag when exiting a wait.
 */
STATIC inline void
strdoioctl_unlink(struct stdata *sd, struct linkblk *l)
{
	union ioctypes *ioc;
	struct linkblk *lbp;
	mblk_t *mb, *db;
	struct qinit qi, *qi_old;

	/* wake everybody up before we block -- the STRCLOSE bit is set */
	if (waitqueue_active(&sd->sd_waitq))
		wake_up_all(&sd->sd_waitq);

	/* block here till we get gosh darn buffers */
	while (!(mb = allocb(sizeof(*ioc), BPRI_WAITOK))) ;

	mb->b_datap->db_type = M_IOCTL;

	ioc = (typeof(ioc)) mb->b_rptr;
	mb->b_wptr = mb->b_rptr + sizeof(*ioc);

	bzero(ioc, sizeof(*ioc));
	ioc->iocblk.ioc_cmd = I_UNLINK;
	ioc->iocblk.ioc_cr = current_creds;
	ioc->iocblk.ioc_count = sizeof(*l);

	while (!(db = allocb(sizeof(*lbp), BPRI_WAITOK))) ;

	db->b_datap->db_type = M_DATA;

	lbp = (typeof(lbp)) db->b_rptr;
	db->b_wptr = db->b_rptr + sizeof(*lbp);

	bzero(lbp, sizeof(*lbp));
	lbp->l_index = l->l_index;
	lbp->l_qtop = l->l_qtop;
	lbp->l_qbot = l->l_qbot;

	mb->b_cont = db;

	/* block here 'till we get the IOCWAIT_BIT */
	while (strwaitioctl(sd, NULL, FTRUNC)) ;	/* no errors please */

	ioc->iocblk.ioc_id = sd->sd_iocid;

	qi_old = sd->sd_rq->q_qinfo;
	if (test_bit(STPLEX_BIT, &sd->sd_flag)) {
		/* When multiplexed, there is no strrput procedure on the STREAM head read queue
		   pair to respond to a message.  It is, therefore, necessary to intercept ioctl
		   response messages (M_IOCACK, M_IOCNAK, M_COPYIN and M_COPYOUT) from the
		   multiplexed read queue.  This is done by taking a copy of the read queue qinit
		   structure, overwriting our strirput() procedure and jamming the q_qinfo pointer
		   on the queue. */
		qi.qi_putp = &strirput;	/* intercept, above */
		sd->sd_muxinit = qi_old;
		sd->sd_rq->q_qinfo = &qi;
	}

	do {
		/* place it directly to qtop, don't trust modules in between */
		ctrace(put(l->l_qtop, mb));
		trace();

		mb = strwaitiocack(sd, NULL, FTRUNC);	/* no errors please */

		ioc = (typeof(ioc)) mb->b_rptr;
		switch (mb->b_datap->db_type) {
		case M_IOCACK:
			break;
		case M_IOCNAK:
			swerr();
			break;
		case M_COPYIN:
		case M_COPYOUT:
			swerr();
			mb->b_datap->db_type = M_IOCDATA;
			/* SVR 4 SPG says no response to M_IOCDATA with error */
			ioc->copyresp.cp_rval = (caddr_t) 1;
			ctrace(put(sd->sd_wq, mb));
			trace();
			goto abort;
		default:
			never();
			break;
		}
		freemsg(mb);
		break;
	} while (1);
      abort:
	sd->sd_rq->q_qinfo = qi_old;
	strwakeioctl(sd);
	return;
}

/**
 *  strsizecheck: - check size of control or data part
 *  @sd: stream head
 *  @type: type (M_DATA or M_PROTO, M_PCPROTO)
 *  @size: size of control or data part
 *
 *  RETURN: Returns a (negative) error number, or the (positive or zero) valid size for the next
 *  block (write() semantics).
 *
 *  NOTICES: Make @type and explicit constant for efficient inlining.
 */
STATIC inline streams_fastcall ssize_t
strsizecheck(const struct stdata *sd, const msg_type_t type, ssize_t size)
{
	if (type == M_DATA) {
		ssize_t sd_maxpsz;

		if (size < sd->sd_minpsz)
			return (-ERANGE);
		if ((sd_maxpsz = sd->sd_maxpsz) < 0)
			sd_maxpsz = INT_MAX;
		if (size > sd_maxpsz) {
			if (sd->sd_minpsz == 0)
				size = sd_maxpsz;
			else
				return (-ERANGE);
		}
		if (size > sysctl_str_strmsgsz)
			return (-ERANGE);
	} else {
		if (size > sysctl_str_strctlsz)
			return (-ERANGE);
	}
	return ((ssize_t) size);
}

#if 0
/**
 *  strwsizecheck: - check sizes and verify buffers
 *  @sd: stream head
 *  @iov: input output vector
 *  @count: number of vectores
 *  @user: copying from user
 *
 *  DESCRIPTION: Performs a size check and verifies buffers.
 *
 *  RETURN: Returns a negative error number or the zero or positive amount of data (the data part)
 *  to be written, for writev(), write() and sendpage().
 */
STATIC inline int
strwsizecheck(const struct stdata *sd, const struct iovec *iov, const unsigned long count,
	      const int user)
{
	long long nbytes = 0;
	int i;

	if (count == 0 || count > IOV_MAX)
		/* POSIX says EINVAL for writev() */
		return (-EINVAL);

	/* count and verify vectors */
	for (i = 0; i < count; i++) {
		ssize_t len;

		if ((len = iov[i].iov_len) > 0) {
			if (user && !access_ok(VERIFY_READ, iov[i].iov_base, len))
				return (-EFAULT);
			/* check for overflow */
			if (nbytes > UIO_MAXIOV - len)
				/* POSIX says EINVAL for writev() */
				return (-EINVAL);
			nbytes += len;
		}
	}

	return strsizecheck(sd, M_DATA, nbytes);
}
#endif

/**
 *  strpsizecheck: - check sizes and verify buffers
 *  @sd: stream head
 *  @cptr: cntl part pointer
 *  @clen: cntl part length
 *  @dptr: data part pointer
 *  @dlen: data part length
 *  @user: copying from user
 *
 *  DESCRIPTION: Performs a size check and verifies buffers.
 *
 *  RETURN: Returns a negative error number or the zero or positive amount of data (the data part)
 *  to be written, for putmsg(), putpmsg() and %I_FDINSERT.  If there is just a data part, we apply
 *  write() rules to q_maxpsz.
 */
STATIC inline ssize_t
strpsizecheck(const struct stdata *sd, const struct strbuf *ctlp, const struct strbuf *datp,
	      const int user)
{
	ssize_t clen = -1;
	ssize_t dlen = -1;

	if (ctlp && (clen = ctlp->len) > 0 && user && !access_ok(VERIFY_READ, ctlp->buf, clen))
		return (-EFAULT);
	if (datp && (dlen = datp->len) > 0 && user && !access_ok(VERIFY_READ, datp->buf, dlen))
		return (-EFAULT);
	{
		ssize_t err;

		if (clen >= 0) {
			if ((err = strsizecheck(sd, M_PROTO, clen)) < -1)
				return (err);
		}
		if (dlen >= 0) {
			if ((err = strsizecheck(sd, M_DATA, dlen)) < -1)
				return (err);
			if ((size_t) dlen > (size_t) err) {
				/* control part apply putpmsg() rules */
				if (clen >= 0)
					return (-ERANGE);
				/* just a data part apply write() rules */
				dlen = err;
			}
		}
	}
	return (dlen);
}

/**
 *  strallocpmsg: - allocate a message for write(), putmsg((), putpmsg(), %I_FDINSERT ioctl()
 *  @sd: stream head
 *  @ctlp: strbuf describing cntl part
 *  @datp: strbuf describing data part
 *  @band: band number
 *  @flags: putpmsg flags
 *
 *  Note: POSIX putmsg(2)/putpmsg(2) says "The putmsg() function shall also block, unless prevented
 *  by lack of internal resources, while waiting for the availability of message blocks in the
 *  STREAM, regardless of priority or whether O_NONBLOCK has been specified.  No partial message is
 *  sent."
 *
 *  Note: POSIX ioctl(2) says "I_FDINSERT also blocks, unless prevented by lack of internal
 *  resources, waiting for the availability of message blocks in the STREAM, regardless of priority
 *  or whether O_NONBLOCK has been specified.  No partial message is sent."
 *
 *  So, we go to great lengths to release the Stream head read lock, wait on buffers, perform copyin
 *  operations (that can also sleep) and then reacquire the Stream head read lock, rechecking write
 *  access permissions.
 */
STATIC inline mblk_t *
strallocpmsg(struct stdata *sd, const struct strbuf *ctlp, const struct strbuf *datp,
	     const int band, const int flags, const int user)
{
	mblk_t *mp;
	int err = 0;

	srunlock(sd);
	do {
		int clen = ctlp ? ctlp->len : -1;
		int dlen = datp ? datp->len : -1;
		int norm = (flags == MSG_BAND);
		int type = norm ? M_PROTO : M_PCPROTO;
		mblk_t *dp;

		/* cannot wait on message blocks with STREAM head read locked */
		if ((mp = alloc_proto(clen, dlen, sd->sd_wroff, type, BPRI_WAITOK))) {
			dp = mp;
			/* copyin can sleep */
			if (clen > 0) {
				if (!user)
					bcopy(ctlp->buf, dp->b_rptr, clen);
				else if ((err = copyin(ctlp->buf, dp->b_rptr, clen)))
					break;
				dp = dp->b_cont;
			}
			if (dlen > 0) {
				if (!user)
					bcopy(datp->buf, dp->b_rptr, dlen);
				else if ((err = copyin(datp->buf, dp->b_rptr, dlen)))
					break;
				dp = dp->b_cont;
			}
			mp->b_band = norm ? band : 0;
		} else
			err = -ENOSR;
	} while (0);
	srlock(sd);

	if (mp) {
		if (!err && !(err = straccess(sd, FWRITE)))
			return (mp);
		freemsg(mp);
	}
	return ERR_PTR(err);
}

/**
 *  strputpmsg_common: - like strputpmsg with a few exceptions
 *  @file: file pointer for stream
 *  @ctlp: valid ctl part (could be NULL)
 *  @datp: valid dat part (could be NULL)
 *  @band: proper band number
 *  @flags: proper putpmsg flags
 *
 *  Except for breaking large data buffers into q_maxpsz chunks when q_minpsz is zero and delimiting
 *  messages, strputpmst_common() with no control part should function the same as the initial
 *  checks for write().
 */
STATIC mblk_t *
strputpmsg_common(const struct file *file, const struct strbuf *ctlp, const struct strbuf *datp,
		  const int band, const int flags)
{
	struct stdata *sd = stri_lookup(file);
	int err = 0;

	if ((err = strpsizecheck(sd, ctlp, datp, 1)) < -1)
		return ERR_PTR(err);
	if ((err = strwaitband(sd, file->f_flags, band, flags)))
		return ERR_PTR(err);
	return strallocpmsg(sd, ctlp, datp, band, flags, 1);
}

#if 0
STATIC mblk_t *
strwrite_common(const struct file *file, const char *buf, size_t len, loff_t *ppos)
{
	struct stdata *sd = stri_lookup(file);
	int err = 0;
	struct strbuf dat;
	mblk_t *mp;

	(void) ppos;		/* ppos is ignored for STREAMS files */

	dat.buf = (char *) buf;
	dat.len = len;
	dat.maxlen = -1;

	if ((err = strsizecheck(sd, M_DATA, len)) < 0)
		return ERR_PTR(err);

	dat.len = err;		/* this is how much we will really write */

	if ((err = strwaitband(sd, file->f_flags, band, flags)))
		return ERR_PTR(err);

	if (IS_ERR(mp = strallocpmsg(sd, NULL, &dat, 0 MSG_BAND, 1)))
		return (mp);

	if (dat.len == len)	/* full write */
		if (test_bit(STRDELIM_BIT, &sd->sd_flag))
			mp->b_flag |= MSGDELIM;

}
#endif

/**
 *  strunlink: - I_UNLINK temporary links on close
 *  @stp: multiplexing driver to close
 *
 *  Unlinks all the temporary lower multiplexer streams on @stp as part of the active or detached
 *  close operation.
 *
 *  Note the STPLEX bit is left set while performing the unlink operation so nothing can happen on
 *  each linked STREAM except for stropen() and strclose().  stropen() will just increase the
 *  sd_opens count and call down open procedures.  strclose() cannot do a last close because the
 *  STPLEX bit is set.  All other operations will fail with [EINVAL] (STREAM linked downstream from
 *  a multiplexing driver).
 */
STATIC void
strunlink(struct stdata *stp)
{
	struct stdata *sd;

	while (ctrace(sd = sd_get(stp->sd_links))) {
		bool detached;

		{
			struct linkblk *l;

			stp->sd_links = xchg(&sd->sd_link_next, NULL);
			l = xchg(&sd->sd_linkblk, NULL);

			/* no errors, no timeout version -- expects no lock */
			strdoioctl_unlink(stp, l);

			freelk(l);
		}
		swlock(sd);
		setsq(sd->sd_rq, NULL);
		setq(sd->sd_rq, &str_rinit, &str_winit);
		clear_bit(STPLEX_BIT, &sd->sd_flag);
		detached = strdetached(sd);
		swunlock(sd);
		if (detached) {
			strremove(sd);
			strlastclose(sd, 0);
		}

		ctrace(sd_put(&sd));	/* could be final put */
	}
}

#if defined CONFIG_STREAMS_FIFO_MODULE || !defined CONFIG_STREAMS_FIFO \
 || defined CONFIG_STREAMS_PIPE_MODULE || !defined CONFIG_STREAMS_PIPE \
 || defined CONFIG_STREAMS_SOCK_MODULE || !defined CONFIG_STREAMS_SOCK
EXPORT_SYMBOL(strwsrv);
#endif

#if !HAVE_KILL_SL_EXPORT
#if HAVE_KILL_SL_ADDR
static int (*kill_sl_func) (pid_t, int, int) = (typeof(kill_sl_func)) HAVE_KILL_SL_ADDR;

#define kill_sl kill_sl_func
#else
STATIC int
__kill_sl_info(int sig, struct siginfo *info, pid_t sess)
{
#if HAVE_SEND_GROUP_SIG_INFO_ADDR
	static int (*send_group_sig_info) (int, struct siginfo *, struct task_struct *) =
	    (typeof(send_group_sig_info)) HAVE_SEND_GROUP_SIG_INFO_ADDR;
#endif
	struct task_struct *p;
	int retval = -ESRCH;

	do_each_task_pid(sess, PIDTYPE_SID, p) {
		int err;

		if (!p->signal->leader)
			continue;
		err = send_group_sig_info(sig, info, p);
		if (retval)
			retval = err;
	}
	while_each_task_pid(sess, PIDTYPE_SID, p);
	return (retval);
}

int
kill_sl_info(int sig, struct siginfo *info, pid_t sess)
{
	int error;

	read_lock(&tasklist_lock);
	error = __kill_sl_info(sig, info, sess);
	read_unlock(&tasklist_lock);
	return (error);
}

int
kill_sl(pid_t sess, int sig, int priv)
{
	return kill_sl_info(sig, (void *) (long) (priv != 0), sess);
}
#endif
#endif

STATIC inline streams_fastcall void
strhangup(struct stdata *sd)
{
	/* If we are a pipe and are fattached, and the other end of the pipe is closing or has
	   closed (which is why the M_HANGUP was sent), then the STREAM head is supposed to be
	   unmounted, and if it is not opened, closed. */
	if (!test_and_set_bit(STRHUP_BIT, &sd->sd_flag)) {
		strwakeall(sd);	/* only interruptible */
		strevent(sd, S_HANGUP, 0);
		/* If we are a control terminal, we are supposed to send SIGHUP to the session
		   leader.  The terminal controller (ldterm) should not send us an M_HANGUP message 
		   if CLOCAL is set. Linux sends SIGCONT after SIGHUP on consoles. Solaris sends
		   SIGTSTP to the foreground process group that doesn't make much sense (its a
		   hack). */
		if (test_bit(STRISTTY_BIT, &sd->sd_flag))
			kill_sl(sd->sd_session, SIGHUP, 1);
		if (test_bit(STRISPIPE_BIT, &sd->sd_flag))
			if (test_bit(STRMOUNT_BIT, &sd->sd_flag)) {
				/* TODO: fdetach the inode and possibly close the stream */
			}
	}
}

/**
 *  strlastclose: - do the last close of a stream
 *  @sd:	the STREAM head to close
 *  @oflag:	open flags to call qi_qclose procedures with
 *
 *  The reason that we seperate this function from strclose() is that it is also used when a
 *  multiplexed stream is unlinked and all file pointers are already closed.
 *
 *  POSIX: General Terminal I/O says:
 *
 *  "The last process to close a terminal device file shall cause any output to be sent to the
 *   device and any input to be discarded.  If HUPCL is set in the control mode and the
 *   communications port supports a disconnect function, the terminal device shall perform a
 *   disconnect."
 *
 *  POSIX: close():-
 *
 *  "If a STREAMS-based fildes is closed and the calling process was previously registered to
 *   receive a %SIGPOLL signal for events associated with that STREAM, the calling process shall be
 *   unregistered for events associated with that STREAM.  The last close() for a STREAM shall cause
 *   the STREAM associated with fildes to be dismantled.  If %O_NONBLOCK is not set and there have
 *   been no signals posted for the STREAM, and if there is data on the module's write queue,
 *   close() shall wait for an unspecified time (for each module and driver) for any output to drain
 *   before dismantling the STREAM.  The time delay can be changed via the %I_SETCLTIME ioctl()
 *   request.  If the %O_NONBLOCK flag is set, or if there are any pending signals, close() shall
 *   not wait for output to drain, and shall dismantle the STREAM immediately."
 *
 *  "If the implementation supports STREAMS-based pipes, and filedes is associated with one end of a
 *   pipe, the last close() shall cause a hangup to occur on the other end of the pipe.  In
 *   addition, if the other end of the pipe has been named by fattach(), then the last close() shall
 *   for the named end to be detached by fdetach().  If the named end has no open file descriptors
 *   associated with it and gets detached, the STREAM associated with that end shall also be
 *   dismantled."
 *
 *  "If fildes refers to the master side of a pseudo-terminal, and this is the last close, a SIGHUP
 *   signal shall be sent to the controlling process, if any, for which the slave side of the
 *   pseudo-terminal is the controlling terminal.  It is unspecified whether closing the master side
 *   of the pseudo-terminal flushes all queue intput and output."
 *
 *  "If fildes refers to the slave side of a STREAMS-based pseudo-terminal, a zero-length message
 *   may be sent to the master."
 *
 *  Discussion:  For STREAMS-based pipes, FIFOs, pseudo-terminals, and even regular STREAMS, much of
 *  this behavior can be accomplished by sending downstream an M_HANGUP message.  For controlling
 *  terminals on the slave side this will cause the controlling session leader to be sent a SIGHUP
 *  as well as hanging up the STREAM.  For pipes and FIFOs, this will set STRHUP and wake processes
 *  at the other end of the pipe or FIFO.  For regular stream, the modules and driver will receive a
 *  M_HANGUP message.
 */
STATIC void
strlastclose(struct stdata *sd, int oflag)
{
	struct stdata *sd_other;

	ptrace(("last close of stream %p\n", sd));
	/* First order of business is to wake everybody up.  We have already set the STRCLOSE bit
	   by this point and when the waiters wake up, straccess() will kick them out with an
	   appropriate error. */

	if (waitqueue_active(&sd->sd_waitq))
		wake_up_all(&sd->sd_waitq);

	trace();
	if ((sd_other = sd->sd_other)) {
		trace();
		/* Perhaps strlastclose() should first send a M_HANGUP message downstream in the
		   same fashion as needs to be done for pipes and master pseudo-terminals, however
		   rather than generating the message we perform the actions on the other stream
		   head directly. */
		strhangup(sd->sd_other);
		ctrace(sd_put(&sd->sd_other));
		/* we do not free the stream head (or stream head queue pair) until the other
		   stream head does this too */
	}

	/* 1st step: unlink any (temporary) linked streams */
	ctrace(strunlink(sd));

	/* 2nd step: call the close routine of each module and pop the module. */
	/* 3rd step: call the close routine of the driver and qdetach the driver */
	ctrace(strwaitclose(sd, oflag));

	/* this balances holding the module in stralloc() and stropen() */
	ctrace(cdrv_put(sd->sd_cdevsw));
	sd->sd_cdevsw = NULL;

	/* not last put, but it had better be the next to last if not a pipe */
	assure(sd_other != NULL || atomic_read(&((struct shinfo *) sd)->sh_refs) == 2);

	/* this sd_put() balances the original allocation of the stream */
	ctrace(sd_put(&sd));	/* not last put */
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  FILE OPERATIONS
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  strllseek: - llseek file operation on stream
 *  @file: user file pointer for the open stream
 *  @off: offset to which to seek
 *  @when: from whence to seek
 */
STATIC loff_t
strllseek(struct file *file, loff_t off, int whence)
{
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (-ESPIPE);
}

/**
 *  strpoll: - poll file operation on stream
 *  @file: user file pointer for the open stream
 *  @poll: poll table pointer
 */
unsigned int
strpoll(struct file *file, struct poll_table_struct *poll)
{
	struct stdata *sd;
	unsigned int mask;

	if ((sd = stri_lookup(file))) {
		mask = 0;
		poll_wait(file, &sd->sd_polllist, poll);
		if (test_bit(STRDERR_BIT, &sd->sd_flag) || test_bit(STWRERR_BIT, &sd->sd_flag))
			mask |= POLLERR;
		if (test_bit(STRHUP_BIT, &sd->sd_flag))
			mask |= POLLHUP;
		if (test_bit(STRPRI_BIT, &sd->sd_flag))
			mask |= POLLPRI;
		if (test_bit(STRMSIG_BIT, &sd->sd_flag))
			mask |= POLLMSG;
		if (test_bit(STPLEX_BIT, &sd->sd_flag))
			mask |= POLLNVAL;
		prlock(sd);
		if (canget(sd->sd_rq))
			mask |= POLLIN | POLLRDNORM;
		if (bcangetany(sd->sd_rq))
			mask |= POLLIN | POLLRDBAND;
		if (canputnext(sd->sd_wq))
			mask |= POLLOUT | POLLWRNORM;
		if (bcanputnextany(sd->sd_wq))
			mask |= POLLOUT | POLLWRBAND;
		prunlock(sd);
	} else
		mask = POLLNVAL;
	return (mask);
}

EXPORT_SYMBOL(strpoll);

STATIC unsigned int
_strpoll(struct file *file, struct poll_table_struct *poll)
{
	struct stdata *sd;
	unsigned int mask;

	/* VFS doesn't check this */
	if (!(file->f_mode & (FREAD | FWRITE)))
		mask = POLLNVAL;
	else if (likely((sd = stri_acquire(file)) != NULL)) {
		if (straccess_rlock(sd, (FCREAT | FAPPEND)) == 0) {
			if (likely(!sd->sd_directio || !sd->sd_directio->poll))
				mask = strpoll(file, poll);
			else
				mask = sd->sd_directio->poll(file, poll);
			srunlock(sd);
		} else
			mask = POLLNVAL;
		ctrace(sd_put(&sd));
	} else
		mask = POLLNVAL;
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (mask);
}

/**
 *  strmmap: - mmap file operation for a stream
 *  @file: user file pointer for the open stream
 *  @vma: pointer to a virtual memory area structure
 */
STATIC int
strmmap(struct file *file, struct vm_area_struct *vma)
{
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (-ENODEV);
}

#ifndef O_CLONE
#define O_CLONE (O_CREAT|O_EXCL)
#endif

/**
 *  stralloc:	- allocate and initialize a STREAM head
 *  @dev:	device number to open
 */
STATIC struct stdata *
stralloc(dev_t dev)
{
	struct stdata *sd;
	struct cdevsw *cdev;

	/* Note: for pipes, FIFOs and sockets the device returned is a STREAM head rather than a
	   separate character device driver. */
	if (!(cdev = cdrv_get(getmajor(dev))))
		return ERR_PTR(-ENXIO);
	/* we don't have a stream yet (or want a new one), so allocate one */
	if (!(sd = allocstr()))
		goto no_sd;
	/* noenable() the write queue so the STRHOLD function works correctly */
	sd->sd_wq->q_flag |= QNOENB;
	/* initialization of the stream head */
	sd->sd_dev = dev;
	sd->sd_flag = STWOPEN;
	sd->sd_cdevsw = cdev;
	/* I think I had the send options for pipes and regular streams backwards: according to
	   POSIX write(2p) manpage, pipes are supposed to ignore zero-length messages by default
	   and normal STREAMS are supposed to pass them by default. */
	switch (cdev->d_mode & S_IFMT) {
	case S_IFIFO:
		sd->sd_flag |= (cdev->d_flag & D_CLONE) ? STRISPIPE : STRISFIFO;
		sd->sd_wropt = SNDPIPE;	/* special write ops */
		setq(sd->sd_rq, cdev->d_str->st_rdinit, cdev->d_str->st_wrinit);
		break;
	case S_IFSOCK:
		sd->sd_flag |= STRISSOCK;
		sd->sd_wropt = SNDPIPE;	/* special write ops */
		setq(sd->sd_rq, cdev->d_str->st_rdinit, cdev->d_str->st_wrinit);
		/* cache minpsz and maxpsz */
		break;
	default:
	case S_IFCHR:
		sd->sd_wropt = SNDZERO;	/* default write ops */
		setq(sd->sd_rq, &str_rinit, &str_winit);
		/* cache minpsz and maxpsz */
		break;
	}
	/* cache minpsz and maxpsz */
	sd->sd_minpsz = sd->sd_wq->q_minpsz;
	sd->sd_maxpsz = sd->sd_wq->q_maxpsz;
	/* don't graft onto inode just yet */
	sd->sd_inode = NULL;
	/* done setup, do the open */
	return (sd);
      no_sd:
	rare();			/* this is highly unlikely as we block allocating stream heads */
	ctrace(cdrv_put(cdev));
	/* POSIX open() reference page also allows [ENOMEM] here. */
	return ERR_PTR(-ENOSR);
}

STATIC int strclose(struct inode *inode, struct file *file);

/**
 *  stropen: - open file operation for a stream
 *  @inode: shadow special filesystem device inode
 *  @file: shadow special filesystem file pointer
 *
 *  stropen() can be called called by cdev_open() with an snode, fifo_open() or sock_open() with an
 *  inode, or a direct open of a mounted internal shadow special filesystem snode.  The @file and
 *  @inode represented here could be external inode or internal snode and file pointer.  For
 *  external inodes, the file->private_data points to a dev_t containing the device number for the
 *  driver.  Internal snodes have NULL file->private_data pointers.  Only fifo (pipe) and socket
 *  (UNIX domain) inodes (regardless of S_IFMT) pass external inodes to this function.
 *
 *  Locking:  To avoid open/close races (that plagued LiS), we can simply hold the inode semaphore
 *  across calls to open and close until a STREAM head is either created or destroyed.  This does
 *  not have to include I_PUSH/I_POP, but should probably include I_LINK/I_UNLINK I_PLINK/I_PUNLINK.
 *
 *  Regardles of whether we use just the STWOPEN bit or the inode semaphore, any driver qopen(9)
 *  procedure that sleeps in the procedure will block others from opening the STREAM head.
 *
 *  Notices: Always set sd->sd_file to the file pointer before calling qopen() or qattach().
 *  Because the module/driver open procedure is called on every open or I_PUSH, and the STWOPEN bit
 *  is held ensuring a single open at a time, the module/driver can rely on this pointer to know the
 *  file pointer to its qi_qopen() procedure.  connld(4) especially requires this facility.  Because
 *  qi_close() is only called on the last close (and it might be a detached linked stream) the file
 *  pointer is meaningless for qclose() and qdetach().
 */
STATIC int
stropen(struct inode *inode, struct file *file)
{
	int err, oflag, sflag;
	struct stdata *sd;
	cred_t *crp = current_creds;

	ptrace(("opening a stream\n"));
	oflag = make_oflag(file);
	sflag = ((oflag & O_CLONE) == O_CLONE) ? CLONEOPEN : DRVOPEN;

	/* always lock inode */
	printd(("%s: locking inode %p\n", __FUNCTION__, inode));
	if ((err = stri_trylock(inode)))
		goto error;
	// spin_lock(&inode->i_lock);
	/* first find out of we already have a stream head, or we need a new one anyway */
	if (sflag != CLONEOPEN) {
		ptrace(("driver open in effect\n"));
		ctrace(sd = sd_get((struct stdata *) inode->i_pipe));
	} else {
		ptrace(("clone open in effect\n"));
		sd = NULL;
	}

	if (!sd) {
		dev_t dev;

		/* need new STREAM head */
		dev = file->private_data ? *((dev_t *) file->private_data) : inode->i_ino;
		ptrace(("no stream head, allocating stream for dev %hu:%hu\n", getmajor(dev),
			getminor(dev)));
		if (IS_ERR(sd = stralloc(dev))) {
			err = PTR_ERR(sd);
			goto up_error;
		}
		/* if we just allocated stream head we already hold the STWOPEN bit */
		dev = strinccounts(file, sd, oflag);
		ptrace(("performing qopen() on sd %p\n", sd));
		/* 1st step: attach the driver and call its open routine */
		if ((err = qopen(sd->sd_rq, &dev, oflag, sflag, crp))) {
			ptrace(("Error path taken for sd %p\n", sd));
			qdelete(sd->sd_rq);	/* cancel original queue pair allocation */
			ctrace(cdrv_put(xchg(&sd->sd_cdevsw, NULL)));	/* cancel hold on module */
			goto up_put_error;	/* will destroy new stream head */
		}
		/* 2nd step: check for redirected return */
		if (dev != sd->sd_dev) {
			/* This only occurs if we allocated a new STREAM head and the device number
			   returned from the qopen() is different from the device number we passed
			   it.  In this case, we need to attach the new stream head to the final
			   inode rather than the initial inode. */
			struct cdevsw *cdev;

			ptrace(("open redirected on sd %p to dev %hu:%hu\n", sd, getmajor(dev),
				getminor(dev)));
			printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
			stri_unlock(inode);
			if (!(cdev = cdrv_get(getmajor(dev)))) {
				err = -ENODEV;
				ptrace(("Error path taken for sd %p\n", sd));
				/* should be qclose instead */
				qdetach(sd->sd_rq, oflag, crp);
				goto up_put_error;
			}
			/* we need a new snode (inode in the device shadow directory) */
			/* qattach() should have already cleaned up the stream head and queue pairs 
			 */
			if ((err = spec_reparent(file, cdev, dev))) {
				ptrace(("Error path taken for sd %p\n", sd));
				/* should be qclose instead */
				qdetach(sd->sd_rq, oflag, crp);
				ctrace(cdrv_put(cdev));
				goto up_put_error;
			}
			sd->sd_dev = dev;
			ctrace(cdrv_put(sd->sd_cdevsw));
			sd->sd_cdevsw = cdev;
			/* FIXME: pull stuff out of qattach here. */
			inode = file->f_dentry->d_inode;
			printd(("%s: locking inode %p\n", __FUNCTION__, inode));
			stri_lock(inode);
		}
		ptrace(("publishing sd %p\n", sd));
		strinsert(inode, sd);	/* publish to inode, device */
		printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
		stri_unlock(inode);
		ptrace(("performing autopush() on sd %p\n", sd));
		/* 3rd step: autopush modules and call their open routines */
		if ((err = autopush(sd, sd->sd_cdevsw, &dev, oflag, MODOPEN, crp))) {
			ptrace(("Error path taken for sd %p\n", sd));
			goto wake_error;
		}
		ctrace(sd_get(sd));	/* to balance sd_put() after put_error, below. */
	} else {
		/* FIXME: Push existing stream open stuff down to str_open() (again). */
		queue_t *wq, *wq_next;
		dev_t dev;

		printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
		stri_unlock(inode);
		/* already have STREAM head */
		if ((err = strwaitopen(sd, FCREAT))) {
			ptrace(("Error path taken for sd %p\n", sd));
			goto put_error;
		}
		dev = strinccounts(file, sd, oflag);
		swunlock(sd);
		/* already open: we walk down the queue chain calling open on each of the modules
		   and the driver (but not the stream head) */
		wq = sd->sd_wq;
		wq_next = SAMESTR(wq) ? wq->q_next : NULL;
		while ((wq = wq_next)) {
			int new_sflag;

			wq_next = SAMESTR(wq) ? wq->q_next : NULL;
			new_sflag = wq_next ? MODOPEN : sflag;
			/* calling qopen for module/driver */
			if ((err = qopen(_RD(wq), &dev, oflag, new_sflag, crp))) {
				ptrace(("Error path taken for sd %p\n", sd));
				break;
			}
		}
	}
      wake_error:
	ptrace(("performing strwakeopen() on sd %p\n", sd));
	strwakeopen(sd);
	if (!err) {
		stri_insert(file, sd);	/* publish to file pointer */
		/* do this if no error and FIFO *after* releasing open bit */
		if (sd->sd_flag & STRISFIFO)
			/* POSIX blocking semantics for FIFOS */
			err = strwaitfifo(sd, oflag);
	}
	if (err) {
		ptrace(("performing strclose() on sd %p\n", sd));
		sd_put(&sd);	/* avoid assure() statement in strclose() */
		strclose(inode, file);
		goto error;
	}
      put_error:
	ctrace(sd_put(&sd));
      error:
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return ((err < 0) ? err : -err);
      up_error:
	printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
	stri_unlock(inode);
	goto error;
      up_put_error:
	printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
	stri_unlock(inode);
	ctrace(sd_put(&sd));
	goto error;
}

/**
 *  strflush: - flush file operation for a stream
 *  @file: user file pointer for stream
 *
 *  sys_close() calls filp_close() that calls f_op->flush().  We don't actualy do any flushing
 *  because this is not necessarily the last close.  If we ever support multiple simultaenous
 *  partial readers or writers we could tear down partials here.
 *
 *  TODO: Because this is the only function from which we can return an error return value, we need
 *  to investigate a number of things about the stream at this point and decide whether to return an
 *  error code now.  Returning an error here does not stop the strclose function from being called
 *  afterward. (XXX: I think this is now covered with the call to straccess() below.)
 */
STATIC int
strflush(struct file *file)
{
	int err = -ENOSTR;
	struct stdata *sd;

	if (likely((sd = stri_acquire(file)) != NULL)) {
		if (!(err = straccess_rlock(sd, FTRUNC)))
			srunlock(sd);
		ctrace(sd_put(&sd));
	}
	/* Not a separate system call, only called before strclose. */
	return (err);
}

/**
 *  strclose: - close file operation for a stream
 *  @inode: shadow special filesystem inode for the stream
 *  @file: shadow special file pointer for the stream
 *
 *  This function is called by fput() on final put of a file pointer associated with the file.
 *  After this function returns a dput() will be peformed on the dentry and a mntput() on the mount.
 *
 *  When the last close is about to occur we will get a d_delete() operation followed by a d_iput()
 *  operation if special dops are assigned to the dentry.  The d_iput() chains or passes to an
 *  iput() which uses a superblock put_inode() operation followed by a superblock delete_inode()
 *  operation.
 *
 *  POSIX: "If a STREAMS-based @filedes is closed and the calling process was previously registered
 *  to receive a %SIGPOLL signal for events associated with that STREAM, the calling process shall
 *  be unregistered for events associated with the STREAM.  The last close() for a STREAM shall
 *  cause the STREAM associated with @filedes to be dismantled.  If %O_NONBLOCK is not set and there
 *  have been no signals posed for the STREAM, and if there is data on the module's write queue,
 *  close() shall wait for an unspecified time (for each module and driver) for any output to drain
 *  before dismantling the STREAM.  The time delay can be changed via an %I_SETCLTIME ioctl()
 *  request.  If the %O_NONBLOCK flag is set, or if there are any pending signals, close() shall not
 *  wait for output to drain, and shall dismantle the STREAM immediately."
 *
 *  "If the implementation supports STREAMS-based pipes, and @filedes is associated with one end of
 *  a pipe, the last close() shall cause a hangup to occur at the other end of the pipe.  In
 *  addition, if the other end of the pipe has been named by fattach(), then the last close() shall
 *  force the named end to be detached by fdetach().  If the named end has no open file descriptors
 *  associated with it and gets detached, the STREAM associated with that end shall also be
 *  dismantled."
 *
 *  "If @filedes refers to the master side of a pseudo-terminal, and this is the last close, a
 *  SIGHUP signal shall be sent to the controlling process, if any, for which the slave side of the
 *  pseudo-terminal is the controlling terminal.  It is unspecified whether closing the master side
 *  of the pseudo-terminal flushes all queue input and output."
 *
 *  "if @fildes refers to the slave side of a pseudo-terminal, a zero-length message may be sent to
 *  the master."
 *
 *  "Where there is outstanding cancelable asyncrhonous I/O operation against @fildes when close()
 *  is called, that I/O operation may be canceled.  An I/O operation that is not canceled completes
 *  as if the close() blocked until the operations completed.  The close() operation itself need not
 *  block awaiting such I/O completion.  Whether any I/O operation is canceled, and which I/O
 *  operation may be canceled upon close(), is implementation-defined."
 *
 *  -- see close(2p) manual *  page.
 *
 *  RETURN VALUE: The return value from this function is ignored.  It is the return value from the
 *  flush() above that is used in return to the close(2) call.
 *
 *  NOTICES: Note that it is the fact that we have set our sd_readers to zero or our sd_writers to
 *  zero that tells the other end of the pipe how to reads and writes and such; however, we need to
 *  wake the other end blocked on read or write to that fact before it is forced to time out.
 *
 *  Note that when we are linked under a multiplexer, we do not tear down the stream at this point,
 *  but simply return (after detaching from the file pointer and performing reader/writer counting).
 *  The multiplexer stream will be torn down and the stream head deallocated only after the
 *  multiplexer is itself unlinked.
 *
 *  Note that because we must always do an stri_lookup before acquiring a reference or taking a lock
 *  on the stream head, it is possible that one thread is closing while another is performing the
 *  same or another system call.  Therefore, it is possible to have stri_lookup(file) return NULL,
 *  even though the file pointer still exists.
 *
 *  Note, to avoid races, we hold the STWOPEN bit while closing.  This simplifies things and
 *  protects the sd_opens, sd_readers and sd_writers members of the stream head.
 *
 *  Note: @inode could be an extenal filesystem inode instead of the snode.
 *
 *  Notes: I ran into some problems where one process was opening a clone device and getting a
 *  reference to the stream head from the inode (in stropen()) while it was in strlastclose().  This
 *  is the classical STREAMS open/close race.  Here we take and hold the inode semaphore until we
 *  know whether it is the last close or not, and, if it is, we remove the STREAM head from the
 *  inode before releasing the semaphore.  That way, when stropen() acquires the semaphore, any
 *  stream head attached to the inode is valid.
 *
 */
STATIC int
strclose(struct inode *inode, struct file *file)
{
	struct stdata *sd;
	int err = 0;

	assert(inode);
	assert(file);

	ptrace(("closing stream on inode %p\n", inode));
	printd(("%s: locking inode %p\n", __FUNCTION__, inode));
	stri_lock(inode);
	if (likely((sd = stri_remove(file)) != NULL)) {
		int oflag = make_oflag(file);

		ptrace(("closing stream %p\n", sd));
		/* Always deregister STREAMS events */
		strevent_unregister(file, sd);

		/* don't want any opens of this stream from the inode until we know whether it is
		   the last close or not.  If it is the last close remove the stream head from the
		   inode before releasing the semaphore. */
		if (strdeccounts(sd, oflag)) {
			ptrace(("last close of stream %p\n", sd));
			/* remove from the inode clone list */
			strremove_locked(inode, sd);
			printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
			stri_unlock(inode);
			ptrace(("putting inode %p\n", inode));
			ptrace(("inode %p no %lu refcount now %d\n", inode, inode->i_ino,
				atomic_read(&inode->i_count) - 1));
			iput(inode);	/* to cancel igrab() from strinsert */
			/* closing stream head */
			ctrace(strlastclose(sd, oflag));
		} else {
			printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
			stri_unlock(inode);
		}

		/* this put balances the sd_get() in stri_insert() */
		ctrace(sd_put(&sd));	/* could be final */
	} else {
		err = -EIO;
		printd(("%s: unlocking inode %p\n", __FUNCTION__, inode));
		stri_unlock(inode);
	}
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (err);
}

/**
 *  strfasync: - fasync file operation for a stream
 *  @fd: user file descriptor
 *  @file: user file pointer
 *  @on: aysnc io flag
 */
STATIC int
strfasync(int fd, struct file *file, int on)
{
	struct stdata *sd;
	int err;

	/* VFS doesn't check this */
	if (!(file->f_mode & (FREAD | FWRITE)))
		err = -EBADF;
	else if (likely((sd = stri_acquire(file)) != NULL)) {
		if ((err = straccess_rlock(sd, FNDELAY)) == 0) {
			if ((err = fasync_helper(fd, file, on, &sd->sd_fasync)) >= 0 && on) {
				if (file->f_owner.pid == 0) {
					struct task_struct *c = current;

					file->f_owner.pid = (-sd->sd_pgrp) ? : c->pid;
					file->f_owner.uid = c->uid;
					file->f_owner.euid = c->euid;
				}
			}
			srunlock(sd);
		}
		ctrace(sd_put(&sd));
	} else
		err = -ENOSTR;
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (err);
}

STATIC streams_fastcall mblk_t *
strlinkmsg(mblk_t *first, mblk_t *mp)
{
	mblk_t **bp;

	for (bp = &first; (*bp); bp = &(*bp)->b_cont) ;
	(*bp) = mp;
	return (first);
}

STATIC inline streams_fastcall ssize_t
strmsgcount(mblk_t *b, bool protdis)
{
	ssize_t count = 0;

	for (; b; b = b->b_cont)
		if ((b->b_wptr > b->b_rptr)
		    && (protdis || b->b_datap->db_type == M_DATA))
			count += b->b_wptr - b->b_rptr;
	return (count);
}

/**
 *  strread: - read file operations for a stream
 *  @file: file pointer for the stream
 *  @buf: user buffer for read data
 *  @len: length of the user buffer
 *  @ppos: seek position
 *
 *  This performs the read() file operation for a stream directly and provides whatever optimization
 *  can be acheived by mapping mblks into separate io vector components.
 *
 *  A read() from a STREAMS file can read data in three different modes: byte-stream mode,
 *  message-nondiscard mode, and message-discard mode.  The default shall be byte-stream mode.  This
 *  can eb changed using the %I_SRDOPT ioctl() request, and can be tested with the %I_GRDOPT
 *  ioctl().  In byte-stream mode, read() shall retrieve data from the STREAM until as many bytes as
 *  were requested are transferred, or until there is no more data to be retrieved.  Byte-stream
 *  mode ignores message boundaries.
 *
 *  In STREAMS message-nondiscard mode, read() shall retrieve data until as many bytes as were
 *  requested are transferred, or until a message boundary is reached.  If read() does not retrieve
 *  all the data in a message, the remaining data shall be left on the STREAM, and can be retrieved
 *  by the next read() call.  Message-discard mode also retrieves data until as many bytes as were
 *  requested are transferred, or a message boundary is reached.  However, unread data remaining in
 *  the message after the read() returns shall be discarded, and shall not be available for a
 *  subsequent read(), getmsg() or getpmsg() call.
 *
 *  How read() handles zero-byte STREAMS messages is determined by the current read mode setting.
 *  In byte-stream mode, read() shall accept data until it has read @nbyte bytes, or until there is
 *  no more data to read, or until a zero-byte message block is encountered.  The read() function
 *  shall then return the number of bytes read, and place the zero-byte message back on the STREAM to
 *  be retrieved by the next read(), getmsg() or getpmsg().  In message-nondiscard mode, or
 *  message-discard mode, a zero-byte message shall return 0 and the message shall be removed from
 *  the STREAM.  When a zero-byte message is read as the first message on a STREAM, the message
 *  shall be removed from the STREAM and 0 shall be returned, regardless of the read mode.
 *
 *  A read() from a STREAMS file shall return the data in the mesage at the front of the STREAM head
 *  read queue, regardless of the priority band of the message.
 *
 *  By default, STREAMs are in control-normal mode, in which a read() from a STREAMS file can only
 *  process messages that contain a data part but do not contain a control part.  The read() shall
 *  fail if a message containing a control part is encountered at the STREAM head.  This default
 *  action can be changed by placing the STREAM in either control-data mode or control-discard mode
 *  with the %I_SRDOPT ioctl() command.  In control-data mode, read() shall convert any control part
 *  to data and pass it to the application before passing any data part originally present in the
 *  same message.  In control-discard mode, read() shall discard message control parts but return to
 *  the process any data part in the message.
 *
 *  In support of TCP out-of-band data, messages on the stream head may be marked by the downstream
 *  module or driver with the %MSGMARK flag in the @b_flag member of the struct msgb structure.
 *  While reading data, if read() has read data and then encounters a marked message, it returns the
 *  amount of data read so far (short read).  This is an indication to the user that there is marked
 *  data on the stream.  A subsequent read reads the marked data.  Whether a message on the stream
 *  head is marked can be determined by the user with the %I_ATMARK ioctl(2) command.
 *
 *  In addition, read() shall fail if the STREAM head had processing an asyncrhonous error before
 *  the call.  In this case, the value of errno(3) shall not reflect the result of read(), but
 *  reflect the prior error.  If a hangup occurs on the STREAM being read, read() shall continue to
 *  operate normally until the STREAM head read queue is empty.  Thereafter, it shall return 0.
 *
 *  The read() function shal fail if:
 *
 *  [%EAGAIN]
 *	The %O_NONBLOCK flag is set for the file descriptor and the process would be delayed.
 *  [%EBADF]
 *	The @file argument is not a valid file pointer open for reading.
 *  [%EBADMSG]
 *	The file is a STREAM file that is set to control-normal mode and the message waiting to be
 *	read includes a control part.
 *  [%EINTR]
 *	The read operation was terminated due to the receipt of a signal, and no data was
 *	transferred.
 *  [%EINVAL]
 *	The STREAM or multiplexer referenced by @file is linked (direclty or indirectly) downstream
 *	from a multiplexer.
 *  [%EIO]
 *	The process is a member of a background process attempting to read from its controlling
 *	terminal, the process is ignoring or blocking the SIGTTIN signal, or the process group is
 *	orphaned.  This error may also be generated for implementation-defined reasons.
 *
 *  %RPROTNORM
 *	Fail read with [%EBADMSG] if a message containing a control part is at the front of the
 *	STREAM head read queue.
 *  %RPROTDAT
 *	Deliver the control part of a message as data when a process issues a read.
 *  %RPROTDIS
 *	Discard the control part of a message, delivering any data portion when a process issues a
 *	read.
 *
 *  %RNORM
 *	Byte-stream mode, the default.
 *  %RMSGD
 *	Message-discard mode.
 *  %RMSGN
 *	Message-nondiscard mode.
 *  %RFILL (AIX only)
 *	Read mode.  This mode prevents completion of any read(2) request until one of three
 *	conditions occurs: 1) the entire user buffer is filled; 2) An end of file occurs; or, 3) the
 *	stream head receits an M_MI_READ_END message.
 *
 *	Several control messages support the %RFILL mode.  They are used by modules to manipulate
 *	data being placed in user buffers at the stream head.  These messages are muliplexed under a
 *	single M_MI message type.  THe message subtype, pointed to by the b_rptr member is one of
 *	the following:
 *
 *	%M_MI_READ_SEEK
 *	    Provides rand access data retrieval.  An application and a cooperating module can gather
 *	    large data blocks from a slow, high-latency, or unreliable link, while minimizing the
 *	    number of system calls required, and relieving the protocol modules of large buffering
 *	    requirements.
 *
 *	    The M_MI_READ_SEEK message subtype is followed by two long words, as in a standard seek
 *	    call.  The first word is an origin indicator as follows:
 *	    
 *	    0  - start of buffer
 *	    1  - current position
 *	    2  - end of buffer
 *
 *	    The second word is a signed offset from the specified origin.
 *
 *	%M_MI_READ_RESET
 *	    Discards any data previously delivered to partially satisfy an %RFILL mode read request.
 *
 *	%M_MI_REQD_END
 *	    Completes the current %RFILL mode read request with whatever data has already been
 *	    delivered.
 *
 *  NOTICES: POSIX says that it is our option whether to check for errors before determining whether
 *  the number of bytes requested was zero (0), or the other way around.  But that it is a good idea
 *  to check for errors first.  Checking for errors first allows an application program to test for
 *  at least some errors by generating a zero-byte read.
 *
 *  POSIX says that the zero-length message causes a return of zero (0) in any mode when no data has
 *  yet been read. Yes, this does signal end of file.  A zero length marked message is one that has
 *  already been consumed, so it gets discarded. SVR 4 does not replace zero length delimited
 *  messages back on the queue. POSIX says that in normal (byte-stream) mode that we place the zero
 *  length message back on the queue and return the amount of data read so far. All read modes
 *  terminate on a zero length (M_DATA) message.
 *
 *  It is not terribly clear how to handle marked messages. A number of UNIX variations document
 *  that read(2) terminates when data is read and a marked message is encountered, allowing
 *  subsequent reads to read the marked message.  What we do, then, is if we have read something
 *  already (i.e. @xferd is non-zero) and the message is marked, we terminate the read. If we
 *  have not yet read anything (i.e. @xferd is zero) then we will consume the marked message.
 *  In this way we never return zero, which could be confused with an end-of-file.
 *
 */
ssize_t
strread(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	struct stdata *sd = stri_lookup(file);
	queue_t *q = sd->sd_rq;
	ssize_t xferd, mread = nbytes;
	bool ndelay, stop;
	mblk_t *mp, *first;
	int err = 0;

	if (ppos != &file->f_pos && ppos && *ppos != 0) {
		ptrace(("Error path taken! ppos = %p, &file->f_pos = %p, *ppos = %ld, file->f_pos = %ld\n", ppos, &file->f_pos, (long) *ppos, (long) file->f_pos));
		return (-ESPIPE);
	}

	if (nbytes == 0)
		return (0);

	if (!access_ok(VERIFY_WRITE, buf, nbytes))
		return (-EFAULT);

	if ((err = straccess_rlock(sd, (FREAD | FNDELAY))))
		return (err);

	{
		/* ensure that compiler sees these as loop invariant */
		const bool svr4mode = (test_bit(STRDELIM_BIT, &sd->sd_flag) != 0);
		const bool bytemode = ((sd->sd_rdopt & (RMSGN | RMSGD)) == 0);
		const bool discard = ((sd->sd_rdopt & RMSGD) != 0);
		const bool protnorm = ((sd->sd_rdopt & (RPROTDAT | RPROTDIS)) == 0);
		const bool protdis = ((sd->sd_rdopt & RPROTDIS) != 0);
		unsigned long pl;

		/* Protect atomicity of STRPRI, strtestgetq() and strputbq() operation */
		pl = zwlock(sd);

		/* Block if there is no data to be read (and generate M_READ if required). */

		for (mp = NULL, first = NULL, xferd = 0, stop = false;
		     !stop && (mp = strtestgetq(sd, q, file->f_flags, MSG_BAND, 0, mread, &pl))
		     && !IS_ERR(mp); mread = 0, first = strlinkmsg(first, mp)) {
			/* remember first block flag and band */
			msg_type_t type;

			type = mp->b_datap->db_type;

			if (type != M_DATA && protnorm)
				goto ebadmsg;

			{
				ssize_t count = strmsgcount(mp, protdis);

				if (type == M_DATA) {
					/* Only look at MSGMARK and MSGDELIM on M_DATA messages. */
					if ((mp->b_flag & MSGMARK))
						stop = true;
					else if ((mp->b_flag & MSGDELIM) || !svr4mode)
						stop = !bytemode;
					if (count == 0) {
						if (!stop && xferd)
							/* terminal end of file, put it back */
							goto putback;
						/* its an M_READ boundary message */
						goto stop;
					}
				} else
					stop = !bytemode;
				xferd += count;
			}

			if (xferd >= nbytes) {
				if (xferd > nbytes && !discard) {
					mblk_t *dp, *b;
					ssize_t position, blen;

					/* Not discarding the excess, so duplicate the message,
					   trim the original and put original back. Work with the
					   duplicate after releasing locks. */
					if (!(dp = dupmsg((mp))))
						goto enosr;
					/* trim head of original */
					for (position = 0, b = mp; b; b = b->b_cont) {
						if ((blen = b->b_wptr - b->b_rptr) <= 0)
							continue;
						if (protdis && b->b_datap->db_type != M_DATA)
							continue;
						if ((position += blen) >= nbytes) {
							b->b_rptr += nbytes - (position - blen);
							break;	/* done */
						}
						b->b_rptr = b->b_wptr;
					}
					/* put original back */
					strputbq(sd, q, mp);
					/* proceed with duplicate */
					mp = dp;
				}
				/* All read modes stop once the request is satisfied. */
				goto stop;
			}

			/* Terminate read when and a message boundary has been reached in message
			   mode, a zero-length data message is received, or the count is
			   satistfied. */
			continue;
		      enosr:
			err = -ENOSR;
			goto putback;
		      ebadmsg:
			err = -EBADMSG;
			goto putback;
		      putback:
			strputbq(sd, q, mp);
			break;
		      stop:
			stop = true;
			continue;
		}

		if (IS_ERR(mp)) {
			err = PTR_ERR(mp);
			ptrace(("error is %d\n", err));
		}

		/* Ok, now everything is ready to copyout() the message, but because copyout() can
		   sleep (page-fault and vm-wait) we need to release the locks on the Stream */

		zwunlock(sd, pl);

		/* capture condition before releasing lock */
		ndelay = (test_bit(STRNDEL_BIT, &sd->sd_flag) != 0);

		srunlock(sd);

		/* transfer the message outside the locks */
		xferd = strmcopyout(first, buf, nbytes, protdis, true);
	}

	/* Per current POSIX specs, only return and error when no read data has been transferred. */
	if (xferd > 0)
		return (xferd);

	switch (err) {
	case -EAGAIN:
		ptrace(("ndelay is %d\n", (int) ndelay));
		if (!ndelay)
			break;
		trace();
		/* For old TTY semantics we are supposed to return zero (0) instead of [EAGAIN]. */
		err = 0;
		break;
	case -ESTRPIPE:
		trace();
		/* If we have hit the end of a pipe or FIFO return zero (0) instead of [ESTRPIPE]. */
		err = 0;
		break;
	case 0:
		trace();
		break;
	default:
		ptrace(("error is %d\n", err));
		break;
	}
	return (err);
}

EXPORT_SYMBOL(strread);

STATIC int _strgetpmsg(struct file *, struct strbuf __user *, struct strbuf __user *, int __user *,
		       int __user *);

#if !defined HAVE_UNLOCKED_IOCTL
#if !defined HAVE_PUTPMSG_GETPMSG_SYS_CALLS || defined LFS_GETMSG_PUTMSG_ULEN
STATIC ssize_t
_strread_getpmsg(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
	struct strpmsg *sg = (struct strpmsg *) buf;

	/* read emulation of the getpmsg system call: the problem with this approach is that it
	   almost completely destroys the ability to have a 64-bit application running against a
	   32-bit kernel because the pointers cannot be properly converted. */

	return _strgetpmsg(file, &sg->ctlbuf, &sg->databuf, &sg->band, &sg->flags);
}
#endif
#endif

STATIC ssize_t
_strread(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
	struct stdata *sd;
	int err;

#if !defined HAVE_UNLOCKED_IOCTL
#if !defined HAVE_PUTPMSG_GETPMSG_SYS_CALLS || defined LFS_GETMSG_PUTMSG_ULEN
	/* FIXME: we really need to do these as an (unlocked) ioctl now */
	if (unlikely(len == LFS_GETMSG_PUTMSG_ULEN)) {
		err = _strread_getpmsg(file, buf, len, ppos);
		goto exit;
	}
#endif
#endif

#if 0				/* vfs_read() checks this */
	if (!(file->f_mode & FREAD))
		err = -EBADF;
	else
#endif
	if (likely((sd = stri_acquire(file)) != NULL)) {
		if (likely(!sd->sd_directio || !sd->sd_directio->read))
			err = strread(file, buf, len, ppos);
		else
			err = sd->sd_directio->read(file, buf, len, ppos);
		ctrace(sd_put(&sd));
	} else
		err = -ENOSTR;
	goto exit;
      exit:
	/* We want to give the driver queues an opportunity to run. */
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (err);
}

/**
 *  strhold: - perform STRHOLD message coallescing  on write
 *  @sd: stream head
 *  @buf: user buffer
 *  @nbytes: total bytes
 *
 *  DESCRIPTION:  Performs the message coallescing feature of SVR 4.  If we can fit the write into
 *  an existing buffer on sd_wq, then add it to the end and let it go.  If there is one there and we
 *  can't fit our write into it, we still have to let it go.
 *
 *  RETURN: Returns a (negative) error number, zero (0) when there was nothing written, otherwise a
 *  postive integer indicated the amount of data written (which should always be nbytes).
 *
 *  LOCKING: Called with a Stream head read lock.  Release it before sleeping.
 */
streams_fastcall STATIC ssize_t
strhold(struct stdata *sd, const int f_flags, const char *buf, ssize_t nbytes)
{
	char fastbuf[FASTBUF];
	ssize_t err;

	if (!sd->sd_wq->q_first)
		return (0);

	if (nbytes == 0 || nbytes >= FASTBUF || test_bit(STRDELIM_BIT, &sd->sd_flag)
	    || sd->sd_wroff > 0)
		nbytes = 0;

	srunlock(sd);

	/* copyin can sleep - so do this outside locks */
	if (nbytes && (err = copyin(buf, fastbuf, nbytes)))
		return (err);

	srlock(sd);

	if ((err = straccess_slow(sd, FWRITE | FNDELAY)) == 0) {

		if ((err = strwaitband(sd, f_flags, 0, MSG_BAND)) == 0) {
			mblk_t *b;

			prlock(sd);
			if ((b = getq(sd->sd_wq))) {
				if (nbytes && b->b_datap->db_lim > b->b_wptr + nbytes) {
					bcopy(fastbuf, b->b_wptr, nbytes);
					b->b_wptr += nbytes;
					err = nbytes;
				}
				/* write locked and verified */
				putnext(sd->sd_wq, b);
			}
			prunlock(sd);
		}
	}

	return (err);
}

STATIC int _strputpmsg(struct file *, struct strbuf __user *, struct strbuf __user *, int, int);

/**
 *  strwrite: - write file operation for a stream
 *  @file: file pointer for the stream
 *  @buf: user buffer to write
 *  @len: length of user data
 *  @ppos: seek position
 *
 *  NOTICES: The STREAM head used to have a writev() file operation, but things got too complex.
 *  Now it falls back on the do_writev_readv() function in the kernel to break writev() down into
 *  individual write() operation calls.  This means that we will delimit each vector if delimitation
 *  is turned on.  So what.
 *
 *  strwrite() logic is from Magic Garden section 7.9.  SVR4 does not have a write side
 *  service procedure, but we do use a write side put procedure to invoke common write side
 *  processing between strwrite and strputpmsg, and even strsendpages.
 *
 *  Note that I used to thing that we needed an strwritev(), otherwise, vfs_writev() will break it
 *  into pieces and deliver it to strwrite() and we will not delimit messages properly (each segment
 *  will be delimited instead of the whole).  We used to do the opposite of what vfs_writev() does,
 *  we had strwrite() call strwritev() with a single segment.
 *
 *  I would have liked to convert this all over to a single straiowrite() call, but, vfs_writev()
 *  chokes (EINVAL) if there is no writev() or write() file operation.  vfs_write() will block on
 *  the aio_write() operation if available, but that is little help.  It is mostly because there is
 *  no writev() equivalent for aio_write().
 *
 *  Note also, that if glibc doesn't think that the kernel UIO_MAXIOV is big enough, it reports a
 *  larger value and breaks it down into individual write() calls on its own.  That is not good, as
 *  it will break delimited messages anyway.  So why not just assume that delimited messages doesn't
 *  work with writev() and leave it at that?
 */
ssize_t
strwrite(struct file *file, const char __user *buf, size_t nbytes, loff_t *ppos)
{
	struct stdata *sd = stri_lookup(file);
	ssize_t err, q_maxpsz, written = 0;
	int access;

	if (!access_ok(VERIFY_READ, buf, nbytes))
		return (-EFAULT);

	if ((q_maxpsz = err = strsizecheck(sd, M_DATA, nbytes)) < 0)
		return (err);

	if (q_maxpsz == 0 && !(sd->sd_wropt & SNDZERO))
		return (0);

	if (unlikely(test_bit(STRHOLD_BIT, &sd->sd_flag) != 0))
		if ((err = strhold(sd, file->f_flags, buf, nbytes)) != 0)
			return (err);

	/* first access check is with FNDELAY to generate pipe signals */
	access = FWRITE | FNDELAY;

	do {
		mblk_t *b;
		size_t block;

		/* Note: if q_minpsz is zero and the nbytes length is greater than q_maxpsz, we are 
		   supposed to break the message down into separate q_maxpsz segments. */

		/* the following can sleep */

		block = min(nbytes - written, q_maxpsz);

		/* POSIX says always blocks awaiting message blocks */
		if (!(b = allocb(block + sd->sd_wroff, BPRI_WAITOK))) {
			err = -ENOSR;
			break;
		}

		b->b_rptr += sd->sd_wroff;
		b->b_wptr += sd->sd_wroff;

		if ((err = copyin(buf + written, b->b_wptr, block)) == 0) {

			b->b_wptr += block;

			/* locks on */
			if ((err = straccess_rlock(sd, access)) == 0) {

				if (written + block >= nbytes
				    && test_bit(STRDELIM_BIT, &sd->sd_flag))
					/* If we performed a full write of the requested number of
					   bytes, we set the MSGDELIM flag to indicate that a full
					   write was performed.  If we perform a partial write this 
					   flag will not be set. */
					b->b_flag |= MSGDELIM;

				/* possibly wait for message band */
				err = strwaitband(sd, file->f_flags, 0, MSG_BAND);
				srunlock(sd);
			}
		}

		/* send off the message block */
		if (likely(err == 0)) {
			/* We don't really queue these, see strwput(). */
			ctrace(put(sd->sd_wq, b));
			trace();
			written += block;
			/* subsequent access checks should return -ESTRPIPE instead of generating
			   signals so that we can simply return the amount written. */
			access = FWRITE;
		} else {
			freeb(b);
			break;
		}

	} while (written < nbytes);

	return ((written > 0) ? written : err);
}

EXPORT_SYMBOL(strwrite);

#if !defined HAVE_UNLOCKED_IOCTL
#if !defined HAVE_PUTPMSG_GETPMSG_SYS_CALLS || defined LFS_GETMSG_PUTMSG_ULEN
STATIC ssize_t
_strwrite_putpmsg(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
	struct strpmsg __user *sp = (struct strpmsg *) buf;
	int band, flags;
	int err;

	/* write emulation of the putpmsg system call: the problem with this approach is that it
	   almost completely destroys the ability to have a 64-bit application running against a
	   32-bit kernel because the pointers cannot be properly converted. */
	if ((err = copyin(&sp->band, &band, sizeof(&sp->band))))
		return (err);
	if ((err = copyin(&sp->flags, &flags, sizeof(&sp->flags))))
		return (err);
	return _strputpmsg(file, &sp->ctlbuf, &sp->databuf, band, flags);
}
#endif
#endif

STATIC ssize_t
_strwrite(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
	struct stdata *sd;
	int err;

#if !defined HAVE_UNLOCKED_IOCTL
#if !defined HAVE_PUTPMSG_GETPMSG_SYS_CALLS || defined LFS_GETMSG_PUTMSG_ULEN
	/* FIXME: we really need to do these as an (unlocked) ioctl now */
	if (len == LFS_GETMSG_PUTMSG_ULEN) {
		err = _strwrite_putpmsg(file, buf, len, ppos);
		goto exit;
	}
#endif
#endif

#if 0				/* vfs_write() checks this */
	if (!(file->f_mode & FWRITE))
		err = -EBADF;
	else
#endif
	if (likely((sd = stri_acquire(file)) != NULL)) {
		if (!sd->sd_directio || !sd->sd_directio->write)
			err = strwrite(file, buf, len, ppos);
		else
			err = sd->sd_directio->write(file, buf, len, ppos);
		ctrace(sd_put(&sd));
	} else
		err = -ENOSTR;
	goto exit;
      exit:
	/* We want to give the driver queues an opportunity to run. */
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (err);
}

/**
 *  __strfreepage: - free routine function to free a esballoc'ed page
 *  @data: &struct page pointer passed as a caddr_t argument
 */
STATIC void
__strfreepage(caddr_t data)
{
	struct page *page = (struct page *) data;

	(void) page;		/* shut up compiler */
	kunmap(page);
}

/**
 *  strwaitpage: - wait to send a page
 *  @sd: stream head
 *  @f_flags: file flags
 *  @size: size of the data to write
 *  @prio: priority of data
 *  @band: priority band for data
 *  @type: message block type
 *  @base: base address
 *  @frtn: free routine to free the allocated page
 *  @timeo: the amount of time remaining to wait
 */
STATIC mblk_t *
strwaitpage(struct stdata *sd, const int f_flags, size_t size, int prio, int band, int type,
	    caddr_t base, struct free_rtn *frtn, long *timeo)
{
	mblk_t *mp;
	int err;

	if (unlikely(size > sysctl_str_strmsgsz))
		return ERR_PTR(-ERANGE);

	if ((err = straccess_rlock(sd, FWRITE)))
		return ERR_PTR(err);

	if (type == M_DATA
	    && (sd->sd_minpsz > size || (size > (size_t) sd->sd_maxpsz && sd->sd_minpsz != 0)))
		return ERR_PTR(-ERANGE);

	if ((band == -1) || bcanputnext(sd->sd_wq, band))

		if ((band != -1 && !bcanputnext(sd->sd_wq, band))) {
			/* wait for band to become available */
			DECLARE_WAITQUEUE(wait, current);

			add_wait_queue(&sd->sd_waitq, &wait);
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				set_bit(WSLEEP_BIT, &sd->sd_flag);
				if ((err = straccess_wakeup(sd, f_flags, timeo, FWRITE)))
					break;
				if (type == M_DATA
				    && (sd->sd_minpsz > size
					|| (size > (size_t) sd->sd_maxpsz && sd->sd_minpsz != 0))) {
					err = -ERANGE;
					break;
				}
				if (bcanputnext(sd->sd_wq, band)) {
					break;
				}
				srunlock(sd);
				*timeo = schedule_timeout(*timeo);
				srlock(sd);
			}
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&sd->sd_waitq, &wait);
		}
	srunlock(sd);
	if (err)
		return ERR_PTR(err);
	if ((mp = esballoc(base, size, prio, frtn))) {
		mp->b_datap->db_type = type;
		mp->b_band = (band == -1) ? 0 : band;
		return (mp);
	}
	return ERR_PTR(-ENOSR);
}

/**
 *  strsendpage: - sendpage file operation for a stream
 *  @file: file pointer for the stream
 *  @page: page descriptor
 *  @offset: offset into the page
 *  @size: amount of data within the page
 *  @ppos: seek position
 *  @more: more pages to send
 */
ssize_t
strsendpage(struct file *file, struct page *page, int offset, size_t size, loff_t *ppos, int more)
{
	mblk_t *mp;
	struct stdata *sd = stri_lookup(file);
	queue_t *q;

	/* FIXME: we really need to take a read lock on the stream head */
	/* FIXME: or we could reassess sd->sd_wq->q_next on each wait loop */
	if (!(q = sd->sd_wq->q_next))
		goto espipe;
	if (q->q_minpsz > size || size > (size_t) q->q_maxpsz || size > sysctl_str_strmsgsz)
		goto erange;
	if (ppos == &file->f_pos) {
		char *base = kmap(page) + offset;
		struct free_rtn frtn = { __strfreepage, (caddr_t) page };
		long timeo = ((file->f_flags & FNDELAY)
			      && !test_bit(STRNDEL_BIT, &sd->sd_flag)) ? 0 : MAX_SCHEDULE_TIMEOUT;

		mp = strwaitpage(sd, file->f_flags, size, BPRI_MED, 0, M_DATA, base, &frtn, &timeo);
		if (IS_ERR(mp)) {
			kunmap(page);
			return PTR_ERR(mp);
		}
		if (!more)
			mp->b_flag |= MSGDELIM;
		/* use put instead of putnext because of STRHOLD feature */
		ctrace(put(sd->sd_wq, mp));
		trace();
		/* We want to give the driver queues an opportunity to run. */
		if (this_thread->flags & (QRUNFLAGS))
			runqueues();
		return (size);
	}
      erange:
	return (-ERANGE);
      espipe:
	return (-ESPIPE);
}

EXPORT_SYMBOL(strsendpage);

STATIC ssize_t
_strsendpage(struct file *file, struct page *page, int offset, size_t size, loff_t *ppos, int more)
{
	struct stdata *sd;
	int err = -ENOSTR;

	if (likely((sd = stri_acquire(file)) != NULL)) {
		if (likely(!sd->sd_directio || !sd->sd_directio->sendpage))
			err = strsendpage(file, page, offset, size, ppos, more);
		else
			err = sd->sd_directio->sendpage(file, page, offset, size, ppos, more);
		ctrace(sd_put(&sd));
	}
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (err);
}

/**
 *  strputpmsg: - putpmsg file operation for a stream
 *  @file: file pointer for the stream
 *  @ctlp: &struct strbuf pointer describing the ctrl part of the message
 *  @datp: &struct strbuf pointer describing the data part of the message
 *  @band: priority band to which to put
 *  @flags: normal or high priority flag
 *
 *  strputpmsg() implements the putpmsg(2) and putmsg(2) system calls for STREAMS.
 *
 *  NOTICES: GLIBC2 puts -1 in band when it is called as putmsg().
 */
int
strputpmsg(struct file *file, struct strbuf __user *ctlp, struct strbuf __user *datp, int band,
	   int flags)
{
	struct stdata *sd = stri_lookup(file);
	mblk_t *mp;
	struct strbuf ctl, dat;
	int err;

	if (band != -1) {
		if (flags != MSG_HIPRI && flags != MSG_BAND)
			return (-EINVAL);
		if (flags == MSG_HIPRI) {
			if (band != 0)
				return (-EINVAL);
		} else {
			if (band < 0 || band > 255)
				return (-EINVAL);
		}
	} else {
		if (flags != RS_HIPRI && flags != 0)	/* RS_NORM */
			return (-EINVAL);
		band = 0;
		if (flags == 0)
			flags = MSG_BAND;
	}
	/* Now band and flags are both correct according to putpmsg() */

	if (ctlp) {
		if ((err = copyin(ctlp, &ctl, sizeof(*ctlp))))
			return (err);
	} else {
		ctl.buf = NULL;
		ctl.len = -1;
		ctl.maxlen = -1;
	}
	if (datp) {
		if ((err = copyin(datp, &dat, sizeof(*datp))))
			return (err);
	} else {
		dat.buf = NULL;
		dat.len = -1;
		dat.maxlen = -1;
	}

	if (flags == MSG_HIPRI) {
		/* need an M_PCPROTO block and zero or more M_DATA message blocks */
		if (ctl.len < 0)
			/* POSIX says: "If no control part is specified and flags is set to
			   RS_HIPRI (MSG_HIPRI), ... fail and set errno to [EINVAL]." */
			return (-EINVAL);
	} else {
		/* need zero or one M_PROTO block and zero or more M_DATA message blocks */
		if (ctl.len < 0) {
			/* no control part */
			if (dat.len < 0)
				/* no data part */
				/* POSIX says: "If a control part part and data part are not
				   specified and flags is set to MSG_BAND, no message shall be sent
				   and 0 shall be returned." */
				return (0);
			if ((dat.len == 0) && !(sd->sd_wropt & SNDZERO))
				return (0);
		}
	}
	if ((err = straccess_rlock(sd, (FWRITE | FNDELAY))) == 0) {
		mp = strputpmsg_common(file, &ctl, &dat, band, flags);
		srunlock(sd);
		if (!IS_ERR(mp)) {
			/* use put instead of putnext because of STRHOLD feature */
			ctrace(put(sd->sd_wq, mp));
			trace();
		} else
			err = PTR_ERR(mp);
	}
	return (err);
}

EXPORT_SYMBOL(strputpmsg);

STATIC int
_strputpmsg(struct file *file, struct strbuf __user *ctlp, struct strbuf __user *datp, int band,
	    int flags)
{
	struct stdata *sd;
	int err;

	/* not checked when coming though ioctl */
	if (!(file->f_mode & FWRITE))
		err = -EBADF;
	else if (likely((sd = stri_acquire(file)) != NULL)) {
		if (likely(!sd->sd_directio || !sd->sd_directio->putpmsg))
			err = strputpmsg(file, ctlp, datp, band, flags);
		else
			err = sd->sd_directio->putpmsg(file, ctlp, datp, band, flags);
		ctrace(sd_put(&sd));
	} else
		err = -ENOSTR;
	/* We want to give the driver queues an opportunity to run. */
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (err);
}

/**
 *  strgetpmsg: - getpmsg file operation for a stream
 *  @file: file pointer for the stream
 *  @ctlp: &struct strbuf pointer receiving the ctrl part of the message
 *  @datp: &struct strbuf pointer receiving the data part of the message
 *  @bandp: pointer to integer to return the priority band of the received message
 *  @flagsp: pointer to integer to return the normal or high priority flag
 *
 *  strgetpmsg() implements the getpmsg(2) and getmsg(2) system calls for STREAMS.
 *
 *  NOTICES: If a hangup occurs on a stream from which messages are to be retrieved, getpmsg()
 *  continues to operate normally, until the stream head read queue is empty.  Thereafter it returns
 *  0 in the len fields of @ctlp and @datp.
 *
 *  GLIBC2 puts NULL in bandp for getmsg().
 */
int
strgetpmsg(struct file *file, struct strbuf __user *ctlp, struct strbuf __user *datp,
	   int __user *bandp, int __user *flagsp)
{
	struct stdata *sd = stri_lookup(file);
	queue_t *q = sd->sd_rq;
	mblk_t *mp = NULL;
	struct strbuf ctl, dat;
	int flags, band = 0;
	int retval = 0;
	int err;

	if (!flagsp)
		return (-EINVAL);
	if (!access_ok(VERIFY_WRITE, flagsp, sizeof(*flagsp)))
		return (-EFAULT);
	if ((err = copyin(flagsp, &flags, sizeof(*flagsp))))
		return (err);
	if (bandp) {
		if (!access_ok(VERIFY_WRITE, bandp, sizeof(*bandp)))
			return (-EFAULT);
		if ((err = copyin(bandp, &band, sizeof(*bandp))))
			return (err);
		/* operate as getpmsg(2s) */
		switch (flags) {
		case MSG_HIPRI:	/* also RS_HIPRI */
			if (band != 0 && band != -1)
				return (-EINVAL);
			if (band == -1) {
				band = 0;
				bandp = NULL;	/* signal lower down that this is getmsg() */
			}
			break;
		case 0:
			/* careful, our library puts getmsg() flags in getpmsg() but then puts -1
			   in band */
			if (band != -1)
				return (-EINVAL);
			else {
				flags = MSG_ANY;
				band = 0;
				bandp = NULL;	/* signal lower down that this is getmsg() */
			}
			break;
		case MSG_ANY:
			if (band != 0)
				return (-EINVAL);
			break;
		case MSG_BAND:
			if (0 > band || band > 255)
				return (-EINVAL);
			break;
		default:
			return (-EINVAL);
		}
	} else {
		/* operate as getmsg(2) */
		switch (flags) {
		case RS_HIPRI:
		case 0:	/* RS_NORM */
			break;
		default:
			return (-EINVAL);
		}
	}

	if (ctlp) {
		if (!access_ok(VERIFY_WRITE, ctlp, sizeof(*ctlp)))
			return (-EFAULT);
		if ((err = copyin(ctlp, &ctl, sizeof(*ctlp))))
			return (err);
		printd(("%s: ctl.len = %d\n", __FUNCTION__, ctl.len));
		printd(("%s: ctl.maxlen = %d\n", __FUNCTION__, ctl.maxlen));
		if (ctl.maxlen < 0)
			ctl.maxlen = -1;
		else {
			printd(("%s: ctl.buf = %p\n", __FUNCTION__, ctl.buf));
			if (!access_ok(VERIFY_WRITE, ctl.buf, ctl.maxlen))
				return (-EFAULT);
			trace();
		}
	} else {
		ctl.maxlen = -1;
		ctl.buf = NULL;
	}
	ctl.len = -1;

	if (datp) {
		if (!access_ok(VERIFY_WRITE, datp, sizeof(*datp)))
			return (-EFAULT);
		if ((err = copyin(datp, &dat, sizeof(*datp))))
			return (err);
		printd(("%s: dat.len = %d\n", __FUNCTION__, dat.len));
		printd(("%s: dat.maxlen = %d\n", __FUNCTION__, dat.maxlen));
		if (dat.maxlen < 0)
			dat.maxlen = -1;
		else {
			printd(("%s: dat.buf = %p\n", __FUNCTION__, dat.buf));
			if (!access_ok(VERIFY_WRITE, dat.buf, dat.maxlen))
				return (-EFAULT);
			trace();
		}
	} else {
		dat.maxlen = -1;
		dat.buf = NULL;
	}
	dat.len = -1;

	if (!(err = straccess_rlock(sd, (FREAD | FNDELAY)))) {
		mblk_t *chp = NULL, *dhp = NULL;
		unsigned long pl;

		/* Protect taking the message off the queue, reading some of it, and then placing
		   the remainder back onto the queue. */
		pl = zwlock(sd);
		{
			ssize_t mread;

			mread = datp ? ((datp->maxlen > 0) ? datp->maxlen : 0) : 0;
			if (IS_ERR(mp = strtestgetq(sd, q, file->f_flags, flags, band, mread, &pl))
			    || !mp) {
				err = PTR_ERR(mp);
				goto unlock_error;
			}

			/* got something - set return flags */
			flags =
			    (mp->b_datap->db_type < QPCTL) ? ((bandp) ? MSG_BAND : 0) : MSG_HIPRI;
			band = mp->b_band;
		}
		{
			mblk_t *b, **bp = &mp;
			mblk_t **ctp = &chp, **dtp = &dhp;
			unsigned short b_flag = mp->b_flag;

			/* Note: POSIX says: "If the high priority control part of the message is
			   consumed, the message shall be placed back on the queue as a normal
			   message of band 0. Subsequent getmsg() and getpmsg() calls shall
			   retrieve the remainder of the message.  If, however, a priority message
			   arrives or already exists on the STREAM head, the subsequent call to
			   getmsg() or getpmsg() shall retrieve the higher priority message before
			   returning the remainder of the message that was put back. */

			/* XXX: So, what is done below is to rip message blocks off of the original
			   message, duplicate any overlap blocks, restore initial message block
			   flags, put the tail back and work with the head. */
			while ((b = *bp)) {
				struct strbuf *part;
				mblk_t ***head;
				ssize_t blen, dlen;
				const bool data = (b->b_datap->db_type == M_DATA);

				part = data ? &dat : &ctl;
				head = data ? &dtp : &ctp;

				ptrace(("Processing %s block\n", data ? "M_DATA" : "M_(PC)PROTO"));

				if ((blen = b->b_wptr - b->b_rptr) <= 0)
					blen = 0;

				dlen = (part->len > 0) ? part->maxlen - part->len : part->maxlen;

				if (part->maxlen == -1 || (blen && !dlen)) {
					/* no room - leave in tail */
					retval |= data ? MOREDATA : MORECTL;
					bp = &b->b_cont;
					continue;	/* leave in tail */
				} else if ((dlen = min(blen, dlen)) == blen) {
					/* full block - remove from tail */
					if (((*bp) = b->b_cont))
						b->b_cont = NULL;
				} else if ((b = dupb(b))) {
					/* partial: duplicate */
					retval |= data ? MOREDATA : MORECTL;
					(*bp)->b_rptr += dlen;
					b->b_wptr -= (blen - dlen);
					bp = &(*bp)->b_cont;
				} else {
					/* world of pain - put message back together */
					(*ctp) = mp;
					(*dtp) = (*bp);
					(*bp) = dhp;
					strputbq(sd, q, chp);
					ctl.len = -1;
					dat.len = -1;
					err = -ENOSR;
					goto unlock_error;
				}
				/* all problems solved with one more level of indirection */
				/* move full block or duplicate partial to head */
				(**head) = b;
				(*head) = &b->b_cont;
				if ((part->len += dlen) < dlen)
					part->len = dlen;
			}
			if (mp) {
				/* put tail back on queue */
				/* restore first block flags */
				mp->b_flag = (mp->b_flag & ~(MSGMARK | MSGDELIM)) | b_flag;

				/* XXX: We could mark the message with the owning thread so that
				   only a subsequent getmsg()/getpmsg()/read() in the current
				   thread will collect the remainder of the data, but the caller
				   really needs to be congnizant that if threads are used, maxlen
				   should indeed cover the maximum expected message size, or
				   threads should not be used.  Well, another way is to use POSIX
				   mandatory file locks on the stream. */
				strputbq(sd, q, mp);
			}
		}

	      unlock_error:
		zwunlock(sd, pl);
		srunlock(sd);

		switch (err) {
		case -EAGAIN:
			if (!test_bit(STRNDEL_BIT, &sd->sd_flag))
				break;
			/* fall through */
		case -ESTRPIPE:	/* STREAM (PIPE, FIFO, TTY) hung up */

			/* POSIX says: If a hangup occurs on the STREAM from which message are
			   retrieved, getmsg() and getpmsg() shall continue to operate normally, as 
			   described above, until the STREAM head read queue is empty.  Thereafter, 
			   they shall return 0 in the len members of ctlptr and dataptr. */

			/* Note that, as noted by Stevens, this is different from a terminal
			   end-of-file which results from a zero-length M_DATA message that returns 
			   -1 in ctlp->len and 0 in datp->len. */

			ctl.len = 0;
			dat.len = 0;
			/* fall through */
		case 0:
		{
			mblk_t *b, *b_cont;
			ssize_t len;

			/* copyout control part */
			for (len = 0, b = chp; b; b_cont = unlinkb(b), freeb(b), b = b_cont) {
				ssize_t blen;

				if ((blen = b->b_wptr - b->b_rptr) <= 0)
					continue;
				ptrace(("Copying out cntl part %d bytes\n", blen));
				copyout(b->b_rptr, ctl.buf + len, blen);
				len += blen;
			}
			/* copyout data part */
			for (len = 0, b = dhp; b; b_cont = unlinkb(b), freeb(b), b = b_cont) {
				ssize_t blen;

				if ((blen = b->b_wptr - b->b_rptr) <= 0)
					continue;
				ptrace(("Copying out data part %d bytes\n", blen));
				copyout(b->b_rptr, dat.buf + len, blen);
				len += blen;
			}
			/* copy out return values */
			if (ctlp)
				put_user(ctl.len, &ctlp->len);
			if (datp)
				put_user(dat.len, &datp->len);
			if (bandp)
				put_user(band, bandp);
			if (flagsp)
				put_user(flags, flagsp);
			return (retval);
		}
		}
	}
	return (err);
}

EXPORT_SYMBOL(strgetpmsg);

STATIC int
_strgetpmsg(struct file *file, struct strbuf __user *ctlp, struct strbuf __user *datp,
	    int __user *bandp, int __user *flagsp)
{
	struct stdata *sd;
	int err;

	/* not checked when we come in through ioctl */
	if (!(file->f_mode & FREAD))
		err = -EBADF;
	else if (likely((sd = stri_acquire(file)) != NULL)) {
		if (likely(!sd->sd_directio || !sd->sd_directio->getpmsg))
			err = strgetpmsg(file, ctlp, datp, bandp, flagsp);
		else
			err = sd->sd_directio->getpmsg(file, ctlp, datp, bandp, flagsp);
		ctrace(sd_put(&sd));
	} else
		err = -ENOSTR;
	/* We want to give the driver queues an opportunity to run. */
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (err);
}

/**
 *  strfattach: - fattach system call
 *  @file: file pointer for stream
 *  @path: path to which to fattach the stream
 */
STATIC inline int
strfattach(struct file *file, const char *path)
{
#if HAVE_KERNEL_FATTACH_SUPPORT
	return do_fattach(file, path);	/* see strattach.c */
#else
	return (-ENOSYS);
#endif
}

/**
 *  strfdetach: - fdetach system call
 *  @path: path from which to fdetach the stream
 */
STATIC int
strfdetach(const char *path)
{
#if HAVE_KERNEL_FATTACH_SUPPORT
	return do_fdetach(path);	/* see strattach.c */
#else
	return (-ENOSYS);
#endif
}

#if 0
/**
 *  strpipe: - pipe system call
 *  @fds: array to which to return two file descriptors
 */
STATIC int
strpipe(int fds[2])
{
#if HAVE_KERNEL_PIPE_SUPPORT
	return do_spipe(fds);	/* see strpipe.c */
#else
	return (-ENOSYS);
#endif
}
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  IO CONTROLS
 *
 *  -------------------------------------------------------------------------
 */

/* 
 *  TTY IO CONTROLS
 *  -------------------------------------------------------------------------
 *  Some TTY IO controls can be handled directly by the STREAM head.  Others that are known to the
 *  STREAM head are converted into %I_STR %M_IOCTL and passed downstream.  Others that are unknown to
 *  the STREAM head are convered to %TRANSPARENT %M_IOCTL and passed downstream.
 */

/**
 *  stty_tiocgsid:  - get the session id for a controlling terminal
 */
STATIC inline streams_fastcall int
tty_tiocgsid(const struct file *file, struct stdata *sd, unsigned long arg)
{
	if (!test_bit(STRISTTY_BIT, &sd->sd_flag))
		return (-ENOTTY);
	if (sd->sd_session <= 0)
		return (-ENOTTY);
	return put_user(sd->sd_session, (pid_t *) arg);
}

/*
 *  Get the process group that receives other than %SIGPOLL signals.
 */
STATIC inline streams_fastcall int
tty_tiocgpgrp(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;

#if 0
	/* some implementations permit this, some do not */
	/* processes can get this from a non-controlling tty */
	if (!test_bit(STRISTTY_BIT, &sd->sd_flag))
		return (-ENOTTY);
#endif
	if ((err = straccess_rlock(sd, FREAD)) == 0) {
		pid_t pgrp;

		pgrp = sd->sd_pgrp;
		srunlock(sd);

		err = put_user(sd->sd_pgrp, (pid_t *) arg);
	}
	return (err);
}

/*
 *  Set the process group to receive other than %SIGPOLL signals.
 */
STATIC inline streams_fastcall int
tty_tiocspgrp(const struct file *file, struct stdata *sd, unsigned long arg)
{
	pid_t pgrp, *valp = (typeof(valp)) arg;
	int err;

#if 0
	/* some implementations permit this, some do not */
	/* processes can set this on a non-controlling tty */
	if (!test_bit(STRISTTY_BIT, &sd->sd_flag))
		return (-ENOTTY);
	if (sd->sd_session != task_session(current))
		return (-ENOTTY);
#endif
	if (get_user(pgrp, (pid_t *) arg))
		return (-EFAULT);
	if (pgrp < 0)
		return (-EINVAL);
	/* signals cannot be sent to other process groups */
	if (pgrp_session(pgrp) != task_session(current))
		return (-EPERM);
	if ((err = straccess_wlock(sd, FEXCL)) == 0) {
		sd->sd_pgrp = pgrp;
		swunlock(sd);
	}
	return (err);
}

STATIC int
 STREAMS_FASTCALL(str_i_atmark(const struct file *file, struct stdata *sd, unsigned long arg));

/**
 *  sock_siocatmark:	- process %SIOCATMARK ioctl
 *  @file:	file pointer
 *  @sd:	STREAM head
 *  @arg:	@arg passed to ioctl
 *
 *  %SIOCATMARK should return true when all urgent data has been read.  We do the same as %I_ATMARK
 *  with the %ANYMARK flag.  Note that this ioctl is pointless if we cannot read and so FREAD access
 *  is tested in strioctl() with straccess().
 */
STATIC inline streams_fastcall int
sock_siocatmark(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;

	if ((err = str_i_atmark(file, sd, ANYMARK)) < 0)
		return (err);
	return put_user(&err, (int *) arg);
}

STATIC inline streams_fastcall int
sock_siocgpgrp(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;

	if (!test_bit(STRISSOCK_BIT, &sd->sd_flag))
		return (-ENOTSOCK);
	if ((err = straccess_rlock(sd, FREAD)) == 0) {
		pid_t pgrp;

		pgrp = sd->sd_pgrp;
		srunlock(sd);

		err = put_user(pgrp, (pid_t *) arg);
	}
	return (err);
}

STATIC inline streams_fastcall int
sock_siocspgrp(const struct file *file, struct stdata *sd, unsigned long arg)
{
	pid_t pgrp, *valp = (typeof(valp)) arg;
	int err;

	if (!test_bit(STRISSOCK_BIT, &sd->sd_flag))
		return (-ENOTSOCK);
	if (get_user(pgrp, (pid_t *) arg))
		return (-EFAULT);
	if (pgrp < 0)
		return (-EINVAL);
	/* signals cannot be sent to other process groups */
	if (pgrp_session(pgrp) != task_session(current))
		return (-EPERM);
	if ((err = straccess_wlock(sd, FEXCL)) == 0) {
		sd->sd_pgrp = pgrp;
		swunlock(sd);
	}
	return (err);
}

STATIC inline streams_fastcall int
file_fiogetown(const struct file *file, struct stdata *sd, unsigned long arg)
{
	if (test_bit(STRISTTY_BIT, &sd->sd_flag))
		return tty_tiocspgrp(file, sd, arg);
	else if (test_bit(STRISSOCK_BIT, &sd->sd_flag))
		return sock_siocspgrp(file, sd, arg);
	else {
		int err;

		if ((err = straccess_rlock(sd, FREAD)) == 0) {
			pid_t owner;

			owner = file->f_owner.pid;
			srunlock(sd);

			owner = (owner < 0) ? -owner : 0;

			err = put_user(owner, (pid_t *) arg);
		}
		return (err);
	}
}

STATIC inline streams_fastcall int
file_fiosetown(struct file *file, struct stdata *sd, unsigned long arg)
{
	if (test_bit(STRISTTY_BIT, &sd->sd_flag))
		return tty_tiocgpgrp(file, sd, arg);
	else if (test_bit(STRISSOCK_BIT, &sd->sd_flag))
		return sock_siocgpgrp(file, sd, arg);
	else {
		pid_t owner;
		int err;

		if (get_user(owner, (pid_t *) arg))
			return (-EFAULT);
		if ((err = straccess_wlock(sd, FEXCL))) {
			sd->sd_pgrp = (owner < 0) ? -owner : 0;
			swunlock(sd);

			file->f_owner.pid = owner;
			file->f_owner.uid = current->uid;
			file->f_owner.euid = current->euid;
		}
		return (err);
	}
}

/* 
 *  STREAMS IO CONTROLS
 *  -------------------------------------------------------------------------
 */

/**
 *  str_i_atmark: - perform streamio(7) %I_ATMARK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Checks whether the next messages on the stream head read queue is marked.  @arg is an integer
 *  value indicating which messages to check as follows:
 *
 *  %ANYMARK
 *	check whether the next message on the stream head read queue is marked.
 *
 *  %LASTMARK
 *	check whether the next message on the stream head read queue is the last marked message
 *	(i.e. it is not followed by another marked message).
 *
 *  Marked messages are messages that have the %MSGMARK flag set in the message @b_flag field.
 *
 *  A number of Unices document that the %ANYMARK and %LASTMARK flags can be combined in inclusive
 *  bitwise-OR, and that when combined, %ANYMARK supersedes %LASTMARK.
 *
 *  When a marked message is encountered during a read(2), the read terminates with the number of
 *  bytes already read.  A short read is an indication to the user to test whether the next message
 *  is a marked message using %I_ATMARK with the %ANYMARK flag.  Because it could be possible that
 *  two marked messages are on the read queue, when %ANYMARK returns true (1), the user should test
 *  if the subsequent message is also marked with %LASTMARK.  It is not necessary to walk the entire
 *  read queue looking for marked messages when %LASTMARK is specified, it is only necessary to
 *  check whether the message following the marked message is also marked.
 *
 *  Upon success, %I_ATMARK returns false (0) to indicate that the stream head read queue does not
 *  meet the mark criteria, or true (1) to indicate that it meets the mark criteria.  Upon failure,
 *  %I_ATMARK returns minus one (-1) and sets errno(3) to an appropriate error number as follows:
 *
 *  [%EINVAL]
 *	@arg is not %ANYMARK or %LASTMARK
 *  [%EIO]
 *	@file is a stream that is closed.
 *  [%EBADF]
 *	@file is not open for reading.
 *  [%EINVAL]
 *	@file is a stream that is linked under a multiplexing driver.
 *  [%EIO]
 *	@file is a stream that is closing or closed.
 *  [%ENXIO]
 *      @file is a stream that has hung up
 *  [%EPIPE]
 *      @file is a pipe and the other end is closing or has closed.
 *  [%ESTRPIPE]
 *	@file is a pipe and there are no writers.
 *
 *  If an %M_ERROR was sent indicating a read error and the read error is still persistent, that
 *  error could be returned also.
 *
 *  LOCKING: str_i_atmark() is called with the stream head read or write locked to protect the
 *  stream head queue pair.  A read lock is taken while examining the read side queue to protect
 *  from messages being added to or removed from the queue during examination of the messages on the
 *  queue.
 */
STATIC streams_fastcall int
str_i_atmark(const struct file *file, struct stdata *sd, unsigned long arg)
{
	queue_t *q = sd->sd_rq;
	mblk_t *b;
	int err = -EINVAL;

	if (!(arg & ~(ANYMARK | LASTMARK)) && (arg & (ANYMARK | LASTMARK))) {
		if (!(err = straccess_rlock(sd, (FREAD | FNDELAY)))) {
			unsigned long pl;

			pl = qrlock(q);
			if (arg == LASTMARK) {
				if ((b = q->q_first) && b->b_flag & MSGMARK) {
					if ((b = b->b_next) && b->b_flag & MSGMARK)
						err = 0;
					else
						err = 1;
				} else
					err = 0;
			} else {
				if ((b = q->q_first) && b->b_flag & MSGMARK)
					err = 1;
				else
					err = 0;
			}
			qrunlock(q, pl);
			srunlock(sd);
		}
	}
	return (err);
}

/**
 *  str_i_canput: - perform streamio(7) %I_CANPUT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Checks whether messages can be written to the queue band specified by @arg.  @arg is an integer
 *  that contains the queue band to test for flow control.  Linux-Fast STREAMS adds the extension
 *  that @arg can also be one of the following values:
 *
 *  %ANYBAND
 *	instead of testing a specified band, test whether any (existing) band is writable.  This is
 *	equivalent to the poll %POLLWRBAND flag.
 *
 *  Upon success, @retval is false (0) if the queue band @arg is flow controlled, true (1) if the
 *  queue band is not flow controlled.  Upon failure, @retval is -1 and errno(3) is set to one of
 *  the following error numbers:
 *
 *  [%EINVAL]
 *	@arg is outside the range 0 to 255, is other than %ANYBAND, and does not represent a valid
 *	priority band.
 *
 *  Any of the errors returned by putpmsg(2s) may be returned in errno(3).  Any error recieved in an
 *  %M_ERROR message indicating a write side error for the stream will be returned in errno(3).  See
 *  straccess() for additional errrors.
 *
 *  LOCKING: str_i_canput() must be called with the stream head read or write lock held across the
 *  call to protect the stream head structure and queues as well as the q->q_next pointers along the
 *  stream.
 *
 *  Use poll(2) if you want speed.
 */
STATIC int
str_i_canput(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err = -EINVAL;

	if (arg < 256 || arg == ANYBAND) {
		if (!(err = straccess_rlock(sd, (FWRITE | FNDELAY)))) {
			queue_t *q = sd->sd_wq;

			prlock(sd);
			if (arg != ANYBAND)
				err = bcanputnext(q, arg) ? 1 : 0;
			else
				err = bcanputnextany(q) ? 1 : 0;
			prunlock(sd);
			srunlock(sd);
		}
	}
	return (err);
}

/**
 *  str_i_ckband: - perform streamio(7) %I_CKBAND ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Use poll(2) if you want speed.
 */
STATIC int
str_i_ckband(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err = -EINVAL;

	if (arg < 256 || arg == ANYBAND) {
		if (!(err = straccess_rlock(sd, (FREAD | FNDELAY)))) {
			queue_t *q = sd->sd_rq;

			if (arg != ANYBAND)
				err = bcanget(q, arg) ? 1 : 0;
			else
				err = bcangetany(q) ? 1 : 0;
			srunlock(sd);
		}
	}
	return (err);
}

/**
 *  str_i_fdinsert: - perform streamio(7) %I_FDINSERT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  [%EFAULT]
 *	A user pointer or length references user data outside the user's valid address space.
 *
 *  [%EINVAL]
 *	An argument was invalid, or the stream attached to the passed in file descriptor is invalid,
 *	or has closed.
 *
 *  [%ERANGE]
 *  [%ENOSR]
 *
 *  CONTEXT: Before we get here we should have already taken a reference to the stream head, and
 *  taken a stream head read lock: both of which are held across the call.  Also, the stream is
 *  checked for the ability to perform write operations before the call and inside the locks.
 *
 *  NOTICES: We do not take a full reference to the queue pointer at the bottom of the stream.
 *  It is the responsibility of the driver receiving the message to ensure that the queue reference
 *  is still valid before using it.
 *
 *  Rarely used, doesn't have to bee too fast.
 */
STATIC int
str_i_fdinsert(const struct file *file, struct stdata *sd, unsigned long arg)
{
	struct strfdinsert fdi, *valp = (typeof(valp)) arg;
	t_uscalar_t token = 0;
	int err;

	if (!(file->f_mode & FWRITE))
		return (-EBADF);
	if ((err = copyin(valp, &fdi, sizeof(fdi))))
		return (err);

	/* POSIX ioctl(2) manpage says that the offset must also be properly aligned in the buffer
	   to hold a t_uscalar_t, but we don't really care, we use a bcopy to move in the pointer.
	   It is the responsibility of the driver receiving the fd insert to know the type and
	   structure of the control message and know whether it is properly aligned or not. */
	if (fdi.offset < 0 || fdi.ctlbuf.len < fdi.offset + sizeof(token)
	    || (fdi.offset & (sizeof(token) - 1)) || (fdi.flags != 0 && fdi.flags != RS_HIPRI))
		return (-EINVAL);

	/* POSIX ioctl(2) manpage says that if the data buffer length is zero (0) that there is no
	   data part. This means that we cannot send zero length data. */
	if (fdi.databuf.len <= 0)
		fdi.databuf.len = -1;

	if ((err = straccess_rlock(sd, (FWRITE | FNDELAY))) == 0) {
		struct file *f2;

		/* This is how we find the queue pointer to use in the message. */
		if ((f2 = fget(fdi.fildes)) && f2->f_op && f2->f_op->release == &strclose) {
			struct stdata *sd2;

			if (ctrace(sd2 = sd_get(stri_lookup(f2)))) {
				/* POSIX says to return ENXIO when the passed in stream is hung up. 
				   Note that we do not care if the stream is linked under a
				   multiplexing driver or not, we can still pass its queue pointer.
				   For closing, it is the responsibility of the receiving driver to
				   determine whether the referenced queue pair still exists before
				   acting upon the message.  We return an error (EIO) if the Stream
				   is closing at the time that it is referenced. We also return an
				   error if there is a read or write error on the inserted Stream. */
				if (!(err = straccess_rlock(sd2, FCREAT | FREAD | FWRITE))) {
					queue_t *qbot;

					/* Magic Garden 7.9.7 says that the queue to use is the
					   queue pointer at the bottom of the stream. It doesn't
					   say that it is the read queue pointer, but we already
					   know that from experience. */
					for (qbot = sd2->sd_wq; SAMESTR(qbot);
					     qbot = qbot->q_next) ;
					token = (typeof(token)) _RD(qbot);
					srunlock(sd2);
				}
				ctrace(sd_put(&sd2));
			} else
				err = -EIO;
			fput(f2);
		} else
			err = -EINVAL;
		if (!err) {
			mblk_t *mp;

			mp = strputpmsg_common(file, &fdi.ctlbuf, &fdi.databuf, 0, fdi.flags);
			if (!IS_ERR(mp)) {
				bcopy(&token, mp->b_rptr + fdi.offset, sizeof(token));
				srunlock(sd);
				/* use put instead of putnext because of STRHOLD feature */
				ctrace(put(sd->sd_wq, mp));
				trace();
				return (0);
			} else
				err = PTR_ERR(mp);
		}
		srunlock(sd);
	}
	return (err);
}

/**
 *  str_i_find: - perform streamio(7) %I_FIND ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Rarely used, can be slow.
 *
 *  ACCESS: Don't care about STRHUP, STRDERR or STWRERR.
 */
STATIC int
str_i_find(const struct file *file, struct stdata *sd, unsigned long arg)
{
	char name[FMNAMESZ + 1];
	queue_t *wq = sd->sd_wq;
	int err;

	printd(("%s: copying string from user\n", __FUNCTION__));
	if ((err = strncpy_from_user(name, (const char *) arg, FMNAMESZ + 1)) < 0) {
		ptrace(("Error path taken! err = %d\n", err));
		return (err);
	}
	if (err < 1 || err > FMNAMESZ) {
		ptrace(("Error path taken! strnlen = %d\n", err));
		return (-EINVAL);
	}

	if (!(err = straccess_rlock(sd, FAPPEND))) {
		for (wq = sd->sd_wq; wq; wq = SAMESTR(wq) ? wq->q_next : NULL)
			if (!strncmp(wq->q_qinfo->qi_minfo->mi_idname, name, FMNAMESZ))
				err = 1;
		srunlock(sd);
	}
	return (err);
}

/**
 *  str_i_flushband: - perform streamio(7) %I_FLUSHBAND ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC inline streams_fastcall int
str_i_flushband(const struct file *file, struct stdata *sd, unsigned long arg)
{
	struct bandinfo bi;
	mblk_t *mp;
	int err;

	if ((err = copyin((typeof(&bi)) arg, &bi, sizeof(bi)))) {
		ptrace(("Error path taken! err = %d\n", err));
		return (err);
	}

	if ((bi.bi_flag & ~(FLUSHR | FLUSHW | FLUSHRW))
	    || !(bi.bi_flag & (FLUSHR | FLUSHW | FLUSHRW))) {
		ptrace(("Error path taken!\n"));
		return (-EINVAL);
	}

	if (!(mp = allocb(2, BPRI_WAITOK))) {
		ptrace(("Error path taken!\n"));
		return (-ENOSR);
	}

	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = bi.bi_flag | FLUSHBAND;
	*mp->b_wptr++ = bi.bi_pri;

	if (!(err = straccess_rlock(sd, (FREAD | FWRITE | FNDELAY | FEXCL)))) {
		srunlock(sd);
		ctrace(put(sd->sd_wq, mp));
		trace();
	} else {
		ptrace(("Error path taken! err = %d\n", err));
		freeb(mp);
	}

	return (err);
}

/**
 *  str_i_flush: - perform streamio(7) %I_FLUSH ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Notes: OpenSolaris sends an unsupported ioctl downstream and blocks on the reply.  Why???
 *  M_FLUSH messages need to be handled directly by the module or driver put() procedure and, in
 *  fact, for a D_MP stream, the M_FLUSH, if echoed back up, should arrive back at the stream head
 *  BEFORE THE RETURN FROM THE CALL TO put()
 *
 *  When sending an M_FLUSH down from the STREAM head don't set the MSGNOLOOP flag.  Only set this
 *  flag when replying downstream with a M_FLUSH that arrived on the read side.  This helps with
 *  STREAMS-based pipes whose other STREAM head will not reply with the M_FLUSH if we set it now.
 */
STATIC inline streams_fastcall int
str_i_flush(const struct file *file, struct stdata *sd, unsigned long arg)
{
	mblk_t *mp;
	int err = 0;

	if (((arg & ~(FLUSHR | FLUSHW | FLUSHRW))) || !((arg & (FLUSHR | FLUSHW | FLUSHRW)))) {
		ptrace(("Error path taken!\n"));
		return (-EINVAL);
	}

	if (!(mp = allocb(1, BPRI_WAITOK))) {
		ptrace(("Error path taken!\n"));
		return (-ENOSR);
	}
	ptrace(("message block %p allocated\n", mp));

	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = arg;

	if (!(err = straccess_rlock(sd, (FREAD | FWRITE | FNDELAY | FEXCL)))) {
		srunlock(sd);
		ptrace(("putting message %p\n", mp));
		ctrace(put(sd->sd_wq, mp));
		trace();
	} else {
		ptrace(("Error path taken! err = %d\n", err));
		freeb(mp);
	}

	return (err);
}

/**
 *  str_i_getband: - perform streamio(7) %I_GETBAND ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC inline streams_fastcall int
str_i_getband(const struct file *file, struct stdata *sd, unsigned long arg)
{
	queue_t *q = sd->sd_rq;
	int band = -1;
	int err;

	if (!access_ok(VERIFY_WRITE, arg, sizeof(band))) {
		ptrace(("Error path taken!\n"));
		return (-EFAULT);
	}

	if (!(err = straccess_rlock(sd, (FREAD | FNDELAY)))) {
		unsigned long pl;

		pl = qrlock(q);
		if (q->q_first) {
			band = q->q_first->b_band;
			qrunlock(q, pl);
		} else {
			qrunlock(q, pl);
			ptrace(("Error path taken!\n"));
			err = -ENODATA;
		}
		srunlock(sd);
	}
	if (!err)
		err = copyout(&band, (int *) arg, sizeof(band));

	return (err);
}

/**
 *  str_i_getcltime: - perform streamio(7) %I_GETCLTIME ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC int
str_i_getcltime(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;

	if (!(err = straccess_rlock(sd, FAPPEND))) {
		int closetime = drv_hztomsec(sd->sd_closetime);

		srunlock(sd);
		err = copyout(&closetime, (typeof(&closetime)) arg, sizeof(closetime));
	}
	return (err);
}

/**
 *  str_i_setcltime: - perform streamio(7) %I_SETCLTIME ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC int
str_i_setcltime(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int closetime;
	int err;

	if (!(err = copyin((int *) arg, &closetime, sizeof(closetime)))) {
		if (0 > closetime || closetime >= 300000) {
			ptrace(("Error path taken!\n"));
			return (-EINVAL);
		}
		if (!(err = straccess_unlocked(sd, (FAPPEND | FEXCL))))
			sd->sd_closetime = drv_msectohz(closetime);
	}
	return (err);
}

/**
 *  str_i_getsig: - perform streamio(7) %I_GETSIG ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  POSIX says that if the stream is not registered for any signals to return EINVAL to %I_GETSIG.
 *
 *  ACCESS: Ignore STRHUP, STRDERR, STWRERR.
 */
STATIC int
str_i_getsig(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int flags;
	int err;

	if (!access_ok(VERIFY_WRITE, arg, sizeof(int))) {
		ptrace(("Error path taken!\n"));
		return (-EFAULT);
	}
	ptrace(("**** Access ok!\n"));
	if (!(err = straccess_rlock(sd, FAPPEND))) {
		struct strevent *se;

		if ((se = __strevent_find(sd)))
			flags = se->se_events;
		else {
			ptrace(("Error path taken!\n"));
			err = -EINVAL;
		}
		srunlock(sd);
	}
	if (!err)
		err = copyout(&flags, (typeof(&flags)) arg, sizeof(flags));
	return (err);
}

/**
 *  str_i_setsig: - perform streamio(7) %I_SETSIG ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  LOCKING: This function takes a STREAM head write lock to protect the sd_siglist member.
 *
 *  ACCESS: No errors on STRHUP, STRDERR, STWRERR, just STPLEX and STRCLOSE (i.e. FAPPEND is set).
 *  %I_SETSIG only affects the calling process, thus controlling terminal restrictions do not apply
 *  (i.e. FEXCL is not set).
 */
STATIC int
str_i_setsig(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;

	if ((arg & ~S_ALL))
		return (-EINVAL);

	if (!(err = straccess_wlock(sd, FAPPEND))) {
		/* quick check */
		if (arg || (sd->sd_sigflags & S_ALL))
			err = __strevent_register(file, sd, arg);
		else {
			ptrace(("Error path taken!\n"));
			err = -EINVAL;
		}
		swunlock(sd);
	}

	return (err);
}

/**
 *  str_i_grdopt: - perform streamio(7) %I_GRDOPT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC int
str_i_grdopt(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int rdopt;
	int err;

	if (!access_ok(VERIFY_WRITE, arg, sizeof(rdopt))) {
		ptrace(("Error path taken!\n"));
		return (-EFAULT);
	}
	if (!(err = straccess_rlock(sd, FAPPEND))) {
		rdopt = sd->sd_rdopt & (RMODEMASK | RPROTMASK);
		srunlock(sd);
		err = copyout(&rdopt, (typeof(&rdopt)) arg, sizeof(rdopt));
	}
	return (err);
}

/**
 *  str_i_srdopt: - perform streamio(7) %I_SRDOPT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  %RPROTNORM
 *	Fail read with [%EBADMSG] if a message containing a control part is at the front of the
 *	STREAM head read queue.
 *  %RPROTDAT
 *	Deliver the control part of a message as data when a process issues a read.
 *  %RPROTDIS
 *	Discard the control part of a message, delivering any data portion when a process issues a
 *	read.
 *
 *  Setting more than one of the above protocol flags will result in an error of [%EINVAL].
 *
 *  %RNORM
 *	Byte-stream mode, the default.
 *  %RMSGD
 *	Message-discard mode.
 *  %RMSGN
 *	Message-nondiscard mode.
 *  %RFILL (AIX only)
 *	Read mode.  This mode prevents completion of any read(2) request until one of three
 *	conditions occurs: 1) the entire user buffer is filled; 2) An end of file occurs; or, 3) the
 *	stream head receits an M_MI_READ_END message.
 *
 *
 *  The bitwise inclusive OR or RMSGD and RMSGN will return [%EINVAL].  The bitwise inclusive OR of
 *  RNORM and either RMSGD or RMSGN will result in the other flag overriding RNORM which is the
 *  default (and has the value zero).
 */
STATIC int
str_i_srdopt(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int rdopt;
	int err;

	if ((arg & ~(RMODEMASK | RPROTMASK)))
		return (-EINVAL);

	rdopt = arg & (RMODEMASK | RPROTMASK);

	switch (rdopt & RMODEMASK) {
	case RNORM:
	case RMSGD:
	case RMSGN:
		break;
	default:
		return (-EINVAL);
	}
	switch (rdopt & RPROTMASK) {
	case RPROTNORM:
	case RPROTDAT:
	case RPROTDIS:
		break;
	case 0:
		/* Note: older applications know nothing of these flags and defaults to RPROTNORM
		   operation. */
		rdopt |= RPROTNORM;
		break;
	default:
		return (-EINVAL);
	}

	if (!(err = straccess_wlock(sd, (FREAD | FNDELAY | FEXCL)))) {
		sd->sd_rdopt = (sd->sd_rdopt & ~(RMODEMASK | RPROTMASK)) | rdopt;
		swunlock(sd);
	}
	return (err);
}

/**
 *  str_i_gwropt: - perform streamio(7) %I_GWROPT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC int
str_i_gwropt(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int wropt;
	int err;

	if (!access_ok(VERIFY_WRITE, arg, sizeof(wropt))) {
		ptrace(("Error path taken!\n"));
		return (-EFAULT);
	}
	if (!(err = straccess_rlock(sd, FAPPEND))) {
		wropt = sd->sd_wropt & (SNDZERO | SNDPIPE | SNDHOLD);
		srunlock(sd);
		err = copyout(&wropt, (typeof(&wropt)) arg, sizeof(wropt));
	}
	return (err);
}

/**
 *  str_i_swropt: - perform streamio(7) %I_SWROPT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Conformance: POSIX only talks of %SNDZERO and does not mention %SNDPIPE or %SNDHOLD.  This
 *  implementation will not return %EINVAL as proscribed by POSIX when @arg includes either %SNDPIPE
 *  or %SNDHOLD or both.  Portable programs will not rely on negative behavior.
 */
STATIC int
str_i_swropt(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int wropt = arg & (SNDZERO | SNDPIPE | SNDHOLD);
	int err;

	if (arg & ~(SNDZERO | SNDPIPE | SNDHOLD))
		return (-EINVAL);

	if (!(err = straccess_wlock(sd, (FWRITE | FNDELAY | FEXCL)))) {
		sd->sd_wropt = (sd->sd_wropt & ~(SNDZERO | SNDPIPE | SNDHOLD)) | wropt;
		if (wropt & SNDHOLD)
			set_bit(STRHOLD_BIT, &sd->sd_flag);
		else
			clear_bit(STRHOLD_BIT, &sd->sd_flag);
		swunlock(sd);
	}
	return (err);
}

extern struct fmodsw sth_fmod;

/**
 *  str_i_xlink: - perform streamio(7) %I_LINK or %I_PLINK ioctl
 *  @file: user file pointer
 *  @mux: multiplexing driver control STREAM head
 *  @arg: ioctl argument
 *  @cmd: ioctl command, always %I_LINK or %I_PLINK
 *
 *  NOTICES: POSIX and most implementations require an acyclic graph of linked streams.  For
 *  example, see Magic Garden Section 7.9.7 %I_LINK, which says: "Check for multiplexor cycles.
 *  Multiplexor configurations must form a directed acyclic graph.  Internally, STREAMS keeps a
 *  picture of this graph and when a new link is made, a node is added to it.  As each %I_LINK is
 *  issued, the graph is checked to see if the %I_LINK could make a cycle occur; and if it could,
 *  the ioctl() failes with the error code [%EINVAL]."
 *
 *  Now, the only time that the graph could have a cycle is if there is a non-persistent link of a
 *  pipe and both ends of the pipe are linked to the same link chain.  For persistent links, it is
 *  necessary but not sufficient that any stream of a driver is linked under any stream of another
 *  driver and visa versa.  It is also necessary that the upper multiplexing driver stream is
 *  actually routing messages to all lower linked streams, otherwise, there is no actual cycle.
 *
 *  To support "interesting" multiplexing driver arrangements, do not check for cycles at all at
 *  this point.  This is a TODO for later.
 *
 *  Multi-Threading: The best way to avoid deadlocks appears to be to set the STWOPEN bit on both
 *  the control STREAM and the STREAM to be linked.  Multi-process opens will block, but all
 *  reads, writes and ioctls to either STREAM will succeed (except for %I_(P)UNLINK, %I_PUSH and
 *  %I_POP, that will also block).  A multi-threaded close will fail to dismantle either STREAM and
 *  we can check for dismantle conditions after the linking has succeeded or failed.
 *
 *  ACCESS: Errors on STRHUP, STRDERR, STWRERR, STPLEX, STRCLOSE, also if controlling tty and
 *  attempted from a background process.
 */
STATIC int
str_i_xlink(const struct file *file, struct stdata *mux, unsigned long arg, const unsigned int cmd)
{
	struct linkblk *l;
	struct file *f;
	struct stdata *sd;
	struct streamtab *st;
	int err;

	err = -EINVAL;
	if (mux->sd_flag & (STRISFIFO | STRISPIPE)) {
		ptrace(("Error path taken!\n"));
		goto error;
	}
	if (!mux->sd_cdevsw || !mux->sd_cdevsw->d_str) {
		ptrace(("Error path taken!\n"));
		goto error;
	}
	st = mux->sd_cdevsw->d_str;
	if (!st->st_muxrinit || !st->st_muxwinit) {
		ptrace(("Error path taken!\n"));
		goto error;
	}

	err = -ENOSR;
	if (unlikely(!(l = alloclk()))) {
		ptrace(("Error path taken!\n"));
		goto error;
	}

	if ((err = strwaitopen(mux, (FREAD | FWRITE | FEXCL | FNDELAY)))) {
		ptrace(("Error path taken!\n"));
		goto free_error;
	}

	err = -EBADF;
	if (!(f = fget(arg))) {
		ptrace(("Error path taken!\n"));
		goto unlock_error;
	}

	err = -EINVAL;
	if (!f->f_op || f->f_op->release != &strclose) {	/* not a stream */
		ptrace(("Error path taken!\n"));
		goto fput_error;
	}
	if (!(sd = stri_acquire(f)) || (sd == mux)) {
		ptrace(("Error path taken!\n"));
		goto fput_error;
	}

	if ((err = strwaitopen(sd, 0))) {
		ptrace(("Error path taken!\n"));
		goto sdput_error;
	}

	err = -ENOSR;
	if ((err = setsq(sd->sd_rq, (struct fmodsw *) mux->sd_cdevsw))) {
		ptrace(("Error path taken!\n"));
		goto unlock2_error;
	}

	swunlock(sd);
	{
		queue_t *qtop, *qbot;

		qbot = sd->sd_wq;
		qbot->q_ptr = _RD(qbot)->q_ptr = NULL;
		for (qtop = mux->sd_wq; qtop && SAMESTR(qtop); qtop = qtop->q_next) ;
		l->l_qtop = qtop;
		l->l_qbot = qbot;
	}
	swunlock(mux);

	/* no locks held */
	err = strdoioctl_link(file, mux, l, cmd, 0);

	if (!err) {
		struct stdata **sdp;

		/* protected by STWOPEN bits */
		sdp = (cmd == I_LINK) ? &mux->sd_links : &mux->sd_cdevsw->d_plinks;
		sd->sd_linkblk = l;
		sd->sd_link_next = xchg(sdp, sd);	/* MP-SAFE on mux */

		swlock(sd);
		setq(sd->sd_rq, st->st_muxrinit, st->st_muxwinit);
		set_bit(STPLEX_BIT, &sd->sd_flag);
		/* clear open bit and wake waiters */
		strwakeopen_swunlock(sd);
		err = l->l_index;
	} else {
		ptrace(("Error path taken! err = %d\n", err));
		setsq(sd->sd_rq, NULL);
		strwakeopen(sd);
	}
	strwakeopen(mux);

	ctrace(sd_put(&sd));	/* could be last put */
	fput(f);
	if (err < 0)
		freelk(l);
      error:
	return (err);
      unlock2_error:
	clear_bit(STWOPEN_BIT, &sd->sd_flag);
	swunlock(sd);
      sdput_error:
	ctrace(sd_put(&sd));
      fput_error:
	fput(f);
      unlock_error:
	clear_bit(STWOPEN_BIT, &mux->sd_flag);
	swunlock(mux);
      free_error:
	freelk(l);
	return (err);
}

/**
 *  str_i_link: - perform streamio(7) %I_LINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Connects two STREAMs, where @file is the file pointer to the STREAM connected to the
 *  multiplexing driver, and @arg is the file descriptor of the STREAM connected to another driver.
 *  The STREAM designated by @arg is connected below the multiplexing driver.  The STREAM designated
 *  by @arg is connected below the multiplexing driver.  %I_LINK requires the multiplexing driver to
 *  send an acknowledgement message to the sTREAM head regarding the connection.  This call shall
 *  return a multiplexer ID number (an identifier used to disconnect the multiplexer; see %I_UNLINK)
 *  on success and a negative error number on failure.
 *
 *  This command uses an implementation-defined default timeout interval.  The
 *  implementation-defined timeout interval for STREAMShas historically been 15 seconds.
 *
 *  str_i_link() shall fail if:
 *
 *  [%ENXIO]
 *	A hangup was received on the STREAM associated with @file or @arg.
 *  [%ETIME]
 *	A timeout ocurred before an acknowledgement message was received at the STREAM head.
 *  [%EAGAIN] or [%ENOSR]
 *	Unable to allocate STREAMS storage to perform the %I_LINK.
 *  [%EBADF]
 *	The @arg argument is not a valid, open file descriptor.
 *  [%EINVAL]
 *	The @file argument does not support multiplexing.
 *  [%EINVAL]
 *	@arg is not a STREAM.
 *  [%EINVAL]
 *	@arg is already connected downstream from a multiplexing driver.
 *  [%EINVAL]
 *	The specified %I_LINK operation would connect the STREAM head in more than one place in the
 *	multiplexed STREAM.
 *
 *  An %I_LINK operation can also fail while waiting for the multiplexing driver to acknowledge the
 *  request, if a message indicating an error or a hangup is received at the STREAM head of @file.
 *  In addition, an error code can be returned in the positive or negative acknowledgement message.
 *  For these cases, %I_LINK failes with errno(3) set tot he value in the message.
 */
STATIC inline streams_fastcall int
str_i_link(const struct file *file, struct stdata *sd, unsigned long arg)
{
	return str_i_xlink(file, sd, arg, I_LINK);
}

/**
 *  str_i_plink: - perform streamio(7) %I_PLINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Creates a persistent connection between two STREAMs, where @file is the file pointer to the
 *  STREAM connected to the multiplexing driver, and @arg is the file descriptor of the STREAM
 *  connected to another driver.  This call shall create a persistent connection which can exist
 *  even if the file pointer @file associated with the upper STREAM to the multiplexing driver is
 *  closed.  The STREAM designated by @arg gets connected via a persistent connection below the
 *  multiplexing driver.  %I_PLINK request the multiplexing driver to send an acknowledgement message
 *  to the STREAM head.  This call shall return a multiplexer ID number (an identifier that may be
 *  used to disconnect the multiplexer; see %I_PUNLINK) on success, and a negative error number on
 *  failure.
 *
 *  This command uses an implementation-defined default timeout interval.  The
 *  implementation-defined timeout interval for STREAMShas historically been 15 seconds.
 *
 *  str_i_plink() shall fail if:
 *
 *  [%ENXIO]
 *	A hangup was received on the STREAM associated with @file.
 *  [%ETIME]
 *	A timeout ocurred awaiting the acknowledgement message at the STREAM head.
 *  [%EAGAIN] or [%ENOSR]
 *	Unable to allocate STREAMS storage to perform the %I_PLINK operation.
 *  [%EINVAL]
 *	The @file argument does not support multiplexing.
 *  [%EINVAL]
 *	@arg is not a STREAM.
 *  [%EINVAL]
 *	@arg is not a valid, open file descriptor.
 *  [%EINVAL]
 *	@arg is a STREAM already connected downstream of a multiplexing driver.
 *  [%EINVAL]
 *	The specified %I_PLINK operation would connect the STREAM head in more than one place in the
 *	multiplexed STREAM.
 *
 *  An %I_PLINK can also fail while waiting for the multiplexing driver to acknowledge the request,
 *  if a message indicating an error or a hangup is received at the STREAM head of @file.  In
 *  addition, an error code can be returned in the positive or negative acknowledgement message.
 *  For these cases, %I_PLINK shall fail with errno(3) set to the value in the message.
 */
STATIC inline streams_fastcall int
str_i_plink(const struct file *file, struct stdata *sd, unsigned long arg)
{
	return str_i_xlink(file, sd, arg, I_PLINK);
}

/**
 *  str_i_xunlink: - perform streamio(7) %I_UNLINK or %I_PUNLINK ioctl
 *  @file: user file pointer (NULL when called from strlastclose())
 *  @sd: stream head
 *  @arg: ioctl argument
 *  @cmd: ioctl command, always %I_UNLINK or %I_PUNLINK
 *
 *  This function processes both permanent links and temporary links.  Permanent links are hung off
 *  of the device structure, because their lifespan in the same as the driver lifespan.  Temporary
 *  links are hung off of the stream head, because their lifespan is that of the stream head.  If
 *  the stream head is closed, all temporary links are unlinked (by separate means, see
 *  strunlink()).
 *
 *  Multi-Threading: The best way to avoid deadlocks appears to be to set the STWOPEN bit on both
 *  the control STREAM and the STREAM to be unlinked.  Multi-process opens will block, but all
 *  reads, writes and ioctls to either STREAM will succeed (except for %I_(P)LINK, %I_PUSH and
 *  %I_POP, that will also block).  A multi-threaded close will fail to dismantle either STREAM and
 *  we can check for dismantle conditions after the unlinking has succeeded or failed.
 *
 *  Notices: When arg is MUXID_ALL, the unlinking process may fail to unlink all links.  The
 *  proceedure will stop at the failed unlinking and return the error code for the failure.  It may
 *  take a subsequent MUXID_ALL call to complete the job.
 */
STATIC streams_fastcall int
str_i_xunlink(struct file *file, struct stdata *mux, unsigned long index, const unsigned int cmd)
{
	struct stdata *sd, **sdp;
	int err;
	const int access = (FREAD | FWRITE | FEXCL | FNDELAY);

	err = -EINVAL;
	if (mux->sd_flag & (STRISFIFO | STRISPIPE))
		goto error;

	if ((err = strwaitopen(mux, access)))
		goto error;

	sdp = (cmd == I_UNLINK) ? &mux->sd_links : &mux->sd_cdevsw->d_plinks;

	err = -EINVAL;
	if (index != MUXID_ALL) {
		for (; *sdp && (*sdp)->sd_linkblk->l_index != index; sdp = &(*sdp)->sd_link_next) ;
		if (!(*sdp))
			goto unlock_error;
	}
	swunlock(mux);

	err = 0;
	if (ctrace(sd = sd_get(*sdp))) {
		do {
			struct linkblk *l;

			/* no errors for unlinked Stream */
			if ((err = strwaitopen(sd, FTRUNC)))
				goto wait_error;
			swunlock(sd);

			/* protected by STWOPEN bit */
			l = sd->sd_linkblk;

			/* no locks held */
			if ((err = strdoioctl_link(file, mux, l, cmd, access)))
				goto ioctl_error;

			/* protected by STWOPEN bit */
			*sdp = xchg(&sd->sd_link_next, NULL);
			freelk(xchg(&sd->sd_linkblk, NULL));

			swlock(sd);
			setsq(sd->sd_rq, NULL);
			setq(sd->sd_rq, &str_rinit, &str_winit);
			clear_bit(STPLEX_BIT, &sd->sd_flag);
			strwakeopen_swunlock(sd);
			ctrace(sd_put(&sd));

		} while (index == MUXID_ALL && (sd = *sdp));
	}
      wakemux_exit:
	strwakeopen(mux);
	return (err);
      ioctl_error:
	strwakeopen(sd);
      wait_error:
	ctrace(sd_put(&sd));
	goto wakemux_exit;
      unlock_error:
	clear_bit(STWOPEN_BIT, &mux->sd_flag);
	swunlock(mux);
      error:
	return (err);
}

/**
 *  str_i_unlink: - perform streamio(7) %I_UNLINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  Disconnects the two STREAMs specified by @file and @arg.  @file is the file pointer to the
 *  STREAM connected to the multiplexing driver.  The @arg argument is the multiplexer ID number
 *  that was returned by the %I_LINK ioctl() command when a STREAM was connected downstream from the
 *  multiplexing driver.  If @arg is %MUXID_ALL, then all STREAMs that were connected to @file shall
 *  be disconnected.  As in %I_LINK, this command requires acknowledgement.
 *
 *  This command uses an implementation-defined default timeout interval.  The
 *  implementation-defined timeout interval for STREAMShas historically been 15 seconds.
 *
 *  The ioctl() function with the %I_UNLINK command shall fail if:
 *
 *  [%ENXIO]
 *	Hangup received on @file.
 *  [%ETIME]
 *	Timeout before acknowledgement message was received at STREAM head.
 *  [%EAGAIN] or [%ENOSR]
 *	Unable to allocate buffers for the acknowledgement message.
 *  [%EINVAL]
 *	Invalid multiplexor ID number.
 *
 *  An %I_UNLINK can also fail while waiting for the multiplexing driver to acknowledge the request
 *  if a message indicating an error or a hangup is received at the STREAM head of @file.  In
 *  addition, an error code can be returned in the positive or negative acknowledgement message.
 *  For these cases, %I_UNLINK shall faile with errno(3) set to the value in the message.
 */
STATIC inline streams_fastcall int
str_i_unlink(struct file *file, struct stdata *sd, unsigned long arg)
{
	return str_i_xunlink(file, sd, arg, I_UNLINK);
}

/**
 *  str_i_punlink: - perform streamio(7) %I_PUNLINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  This command uses an implementation-defined default timeout interval.  The
 *  implementation-defined timeout interval for STREAMShas historically been 15 seconds.
 */
STATIC inline streams_fastcall int
str_i_punlink(struct file *file, struct stdata *sd, unsigned long arg)
{
	return str_i_xunlink(file, sd, arg, I_PUNLINK);
}

/**
 *  str_i_list: - perform streamio(7) %I_LIST ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  %I_LIST is a racey specification: it assumes that the caller is the only one that can push or
 *  pop modules from the STREAM.  It should not be relied upon otherwise.  The list is accurate for
 *  the moment that the list was compiled.
 */
STATIC int
str_i_list(const struct file *file, struct stdata *sd, unsigned long arg)
{
#define STR_MLIST_FAST 8
	struct str_list sl;
	struct str_mlist smlist[STR_MLIST_FAST], *sm = smlist;
	size_t smlist_size = sizeof(sl);
	int err;

	if (arg == 0) {
		/* return number of modules and drivers */
		if (!(err = straccess_rlock(sd, FAPPEND))) {
			int drivers;

			drivers = (sd->sd_flag & (STRISFIFO | STRISPIPE)) ? 0 : 1;
			err = sd->sd_pushcnt + drivers;
			srunlock(sd);
		}
		return (err);
	}

	if ((err = copyin((typeof(&sl)) arg, &sl, sizeof(sl))))
		return (err);

	if (sl.sl_nmods < 0 || sl.sl_modlist == NULL)
		return (-EINVAL);

	/* Need to build list in kernel space with locks on */

	if (sl.sl_nmods > STR_MLIST_FAST) {
		smlist_size = sizeof(*sm) * sl.sl_nmods;
		sm = kmem_alloc(smlist_size, KM_SLEEP);
	}

	if (!(err = straccess_rlock(sd, FAPPEND))) {
		struct queue *q;
		size_t size = 0;
		int i = 0;
		int drivers = (sd->sd_flag & (STRISFIFO | STRISPIPE)) ? 0 : 1;

		if ((sl.sl_nmods = min(sd->sd_pushcnt + drivers, sl.sl_nmods))) {
			struct str_mlist *smp;

			for (q = sd->sd_wq->q_next, i = 0, smp = sm;
			     q && i < sl.sl_nmods; q = q->q_next, i++, smp++) {
				strncpy(smp->l_name, _RD(q)->q_qinfo->qi_minfo->mi_idname,
					FMNAMESZ);
				size += sizeof(*smp);
			}
		}
		srunlock(sd);

		err = 0;

		if ((sl.sl_nmods = i) && (err = copyout(sm, sl.sl_modlist, size)))
			goto error;

		err = copyout(&sl.sl_nmods, (typeof(&sl.sl_nmods)) arg, sizeof(sl.sl_nmods));
	}
      error:

	if (sm != smlist)
		kmem_free(sm, smlist_size);

	return (err);
#undef STR_MLIST_FAST
}

/**
 *  str_i_look: - perform streamio(7) %I_LOOK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC int
str_i_look(const struct file *file, struct stdata *sd, unsigned long arg)
{
	char fmname[FMNAMESZ + 1];
	queue_t *q = sd->sd_wq;
	int err;

	if (!(err = straccess_rlock(sd, FAPPEND))) {
		if (SAMESTR(q) && (q = q->q_next))
			snprintf(fmname, FMNAMESZ + 1, _RD(q)->q_qinfo->qi_minfo->mi_idname);
		else
			err = -EINVAL;
		srunlock(sd);
	}
	if (!err)
		err = copyout(fmname, (char *) arg, FMNAMESZ + 1);
	return (err);
}

/**
 *  str_i_nread: - perform streamio(7) %I_NREAD ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC inline streams_fastcall int
str_i_nread(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err, msgs, bytes;

	if (!(err = straccess_rlock(sd, (FREAD | FNDELAY)))) {
		queue_t *q = sd->sd_rq;
		unsigned long pl;

		pl = qrlock(q);
		if ((msgs = qsize(q)))
			bytes = msgdsize(q->q_first);
		else
			bytes = 0;
		qrunlock(q, pl);
		srunlock(sd);
		if (!(err = copyout(&bytes, (typeof(&bytes)) arg, sizeof(bytes))))
			err = msgs;
	}
	return (err);
}

/**
 *  str_i_peek: - perform streamio(7) %I_PEEK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  FIXME: This is a tough one.  Rework this code.
 */
STATIC inline streams_fastcall int
str_i_peek(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err, rtn = 0;
	struct strpeek sp;

	if (!arg)
		return (-EINVAL);
	if (!access_ok(VERIFY_WRITE, arg, sizeof(sp)))
		return (-EFAULT);
	if ((err = copyin((typeof(&sp)) arg, &sp, sizeof(sp))))
		return (err);

	if (sp.flags != 0 && sp.flags != RS_HIPRI)
		return (-EINVAL);

	if (sp.ctlbuf.maxlen < 0) {
		sp.ctlbuf.maxlen = -1;
		sp.ctlbuf.buf = NULL;
	} else if (!access_ok(VERIFY_WRITE, sp.ctlbuf.buf, sp.ctlbuf.maxlen))
		return (-EFAULT);
	sp.ctlbuf.len = -1;

	if (sp.databuf.maxlen < 0) {
		sp.databuf.maxlen = -1;
		sp.databuf.buf = NULL;
	} else if (!access_ok(VERIFY_WRITE, sp.databuf.buf, sp.databuf.maxlen))
		return (-EFAULT);
	sp.databuf.len = -1;

	if (!(err = straccess_rlock(sd, FREAD | FNDELAY))) {
		mblk_t *b, *dp = NULL;
		unsigned long pl;
		queue_t *q = sd->sd_rq;

		pl = qrlock(q);

		if ((b = q->q_first)) {
			if (sp.flags == 0 || b->b_datap->db_type >= QPCTL)
				if (!(dp = dupmsg(b)))
					err = -ENOSR;
		}

		qrunlock(q, pl);
		srunlock(sd);

		if ((b = dp)) {
			ssize_t clen = sp.ctlbuf.maxlen;
			ssize_t dlen = sp.databuf.maxlen;

			rtn = 1;
			sp.flags = b->b_datap->db_type > QPCTL ? RS_HIPRI : 0;
			sp.ctlbuf.len = -1;
			sp.databuf.len = -1;

			for (; b; b = b->b_cont) {
				ssize_t blen, copylen;

				if ((blen = b->b_wptr - b->b_rptr) <= 0)
					continue;
				if (b->b_datap->db_type == M_DATA) {

					/* goes in data part */
					if (dlen <= 0)
						continue;
					if (sp.databuf.len < 0)
						sp.databuf.len = 0;
					copylen = (blen > dlen) ? dlen : blen;
					if (copyout
					    (b->b_rptr, sp.databuf.buf + sp.databuf.len, copylen)) {
						err = -EFAULT;
						break;
					}
					sp.databuf.len += copylen;
					dlen -= copylen;
				} else {
					/* goes in ctrl part */
					if (clen <= 0)
						continue;
					if (sp.ctlbuf.len < 0)
						sp.ctlbuf.len = 0;
					copylen = (blen > clen) ? clen : blen;
					if (copyout
					    (b->b_rptr, sp.ctlbuf.buf + sp.ctlbuf.len, copylen)) {
						err = -EFAULT;
						break;
					}
					sp.ctlbuf.len += copylen;
					clen -= copylen;
				}
			}
			if (err >= 0) {
				put_user(sp.ctlbuf.len, &((typeof(&sp)) arg)->ctlbuf.len);
				put_user(sp.databuf.len, &((typeof(&sp)) arg)->databuf.len);
				put_user(sp.flags, &((typeof(&sp)) arg)->flags);
			}
			/* done with duplicate */
			freemsg(dp);
		}
	}
	if (err < 0)
		return (err);
	return (rtn);
}

/**
 *  str_i_push: - perform streamio(7) %I_PUSH ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  See Magic Garden 7.9.7 %I_PUSH.
 *
 *  [EFAULT]	arg points outside the user's address space
 *  [EINVAL]	a module by that name does not exist
 *  [EIO]	the stream has closed or is closing
 *  [ENXIO]	the stream is hung up
 *  [EPIPE]	the other end of a pipe has closed
 *  [EINTR]	a signal was received before the operation could complete
 *  [EINVAL]	the maximum module push limit has been reached (LiS does ENOSR here)
 *
 *  Also, any error returned by the module's qi_qopen procedure can also be returned.
 */
STATIC int
str_i_push(struct file *file, struct stdata *sd, unsigned long arg)
{
	char name[FMNAMESZ + 1];
	struct fmodsw *fmod;
	int err;

	printd(("%s: copying string from user\n", __FUNCTION__));
	if ((err = strncpy_from_user(name, (const char *) arg, FMNAMESZ + 1)) < 0) {
		ptrace(("Error path taken! err = %d\n", err));
		return (err);
	}
	if (err < 1 || err > FMNAMESZ) {
		ptrace(("Error path taken! strnlen = %d\n", err));
		return (-EINVAL);
	}

	/* MG also says to check STREAMS memory limit. */

	/* Note that this will also demand load streams_name.ko, if required. */
	if ((fmod = fmod_find(name))) {
		if (!(err = strwaitopen(sd, 0))) {
			swunlock(sd);

			/* Note sd->sd_pushcnt and sd->sd_file are protected by STWOPEN bit. */

			if (sd->sd_pushcnt < sysctl_str_nstrpush) {

				dev_t dev = sd->sd_dev;
				int oflag = make_oflag(file);
				cred_t *crp = current_creds;

				sd->sd_file = file;	/* always before open */

				if (!(err = qattach(sd, fmod, &dev, oflag, MODOPEN, crp))) {
					bool wasctty = (test_bit(STRISTTY_BIT, &sd->sd_flag) != 0);

					/* Modules not supposed to change dev! connld(4) maybe on
					   open(), never on %I_PUSH. */
					assure(dev == sd->sd_dev);

					sd->sd_pushcnt++;

					if (!wasctty && test_bit(STRISTTY_BIT, &sd->sd_flag)) {
						/* TODO: MG says that if the %STRISTTY flag is set
						   at this point (that is, STRISTTY was set by the
						   module using M_SETOPTS) to make the stream a
						   controlling terminal (i.e.  redirect standard
						   input, output and error here). */
						/* This is also where sd_session and sd_pgrp
						   members are initialized to appropriate values */
						sd->sd_session = task_session(current);
						sd->sd_pgrp = task_pgrp(current);
					}
				} else {
					/* POSIX says ENXIO on module open function failure. (And
					   in effect, this is all that modules should return as an
					   error: drivers can return more informative errors.) It
					   is a question here whether to leave modules on their
					   honor or to set the error number for them. */
					ptrace(("Error path taken! err = %d\n", err));
					assure(err == -ENXIO);
				}
			} else {
				ptrace(("Error path taken!\n"));
				err = -EINVAL;
			}
			strwakeopen(sd);
		} else {
			ptrace(("Error path taken! err = %d\n", err));
		}
		fmod_put(fmod);
	} else {
		ptrace(("Error path taken!\n"));
		err = -EINVAL;
	}

	return (err);
}

/**
 *  str_i_pop:	- perform streamio(7) %I_POP ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 *
 *  See Magic Garden 7.9.7 %I_POP.
 *
 *  [EIO]	the stream is closed or closing.
 *  [ENXIO]	the stream is not a FIFO or pipe and the device has hung up.
 *  [EPIPE]	the stream is a pipe and the other end has closed.
 *  [EINVAL]	there are no modules to pop.
 *  [EPERM]	only anchored modules exist and the called does not have adequate privilege to pop
 *		further modules.
 */
STATIC int
str_i_pop(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;

	if (!(err = strwaitopen(sd, 0))) {
		swunlock(sd);

		/* protected by STWOPEN bit */
		if (sd->sd_pushcnt > 0) {

			/* TODO: should use capabilities instead of UID */
			if (sd->sd_pushcnt >= sd->sd_nanchor || current_creds->cr_uid == 0) {
				int oflag = make_oflag(file);
				cred_t *crp = current_creds;

				sd->sd_pushcnt--;
				qdetach(_RD(sd->sd_wq->q_next), oflag, crp);
			} else {
				ptrace(("Error path taken!\n"));
				err = -EPERM;
			}
		} else {
			ptrace(("Error path taken!\n"));
			err = -EINVAL;
		}
		strwakeopen(sd);
	} else {
		ptrace(("Error path taken! err = %d\n", err));
	}
	return (err);
}

/**
 *  freefd_func: - free function for file descriptor
 *  @arg: file pointer as caddr_t argument
 *
 *  NOTICES:  There is one problem with this approach: a final fput() of a file pointer can sleep,
 *  so if these messages are flushed from the read queue of the receiving Stream head in STREAMS
 *  scheduler context from an M_FLUSH message passed from below, the fput() could sleep while soft
 *  interrupts are locked out.  That is not good.  FC4 (and others) complains loudly in the logs
 *  when this happens.  To avoid this, keep file pointers open until passed file pointers have been
 *  received by the other end or flushed.  Another option would be to refuse to flush them from the
 *  read queue, and leave them hanging around until the queue pair is closed (under user context).
 */
STATIC void
freefd_func(caddr_t arg)
{
	struct file *file = (struct file *) arg;

	/* sneaky trick to free the file pointer when mblk freed, this means that M_PASSFP messages 
	   flushed from a queue will free the file pointers referenced by them */
	trace();
	printd(("%s: file pointer %p count is now %d\n", __FUNCTION__, file, file_count(file) - 1));
	fput(file);
	return;
}

/**
 *  str_i_sendfd: - perform streamio(7) %I_SENDFD ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @arg: ioctl argument
 */
STATIC inline streams_fastcall int
str_i_sendfd(const struct file *file, struct stdata *sd, unsigned long arg)
{
	struct stdata *s2;
	struct file *f2;
	int err;
	bool fifo, pipe;

	if ((err = straccess_rlock(sd, (FWRITE | FEXCL | FNDELAY))))
		goto exit;

	fifo = (test_bit(STRISFIFO_BIT, &sd->sd_flag) != 0);
	pipe = (test_bit(STRISPIPE_BIT, &sd->sd_flag) != 0);

	err = -EINVAL;		/* POSIX says EINVAL */
	if (!fifo && !pipe)
		goto unlock_exit;

	s2 = sd->sd_other;
	if (fifo || (pipe && !s2))
		s2 = sd;

	if ((err = straccess_rlock(s2, (FREAD | FEXCL | FNDELAY))))
		goto unlock_exit;

	err = -EBADF;
	if (!(f2 = fget(arg)))
		goto unlock2_exit;

	printd(("%s: file pointer %p count is now %d\n", __FUNCTION__, f2, file_count(f2)));

	/* Ok, with this approach we have a problem with passing a file pointer that is associated
	   with the stream head to which it is passed.  The problem is that if the M_PASSFP message
	   containing a file pointer to the stream head sits on that stream head's read queue when
	   the last file descriptor is closed, that stream head will never close.  Therefore, if the
	   file pointer passed is associated with the stream head to which we are passing the file
	   pointer, we return EINVAL.  In general it is not very useful to pass a stream head its own 
	   file pointer. */
	err = -EINVAL;
	if (f2->f_op && f2->f_op->release == &strclose && stri_lookup(f2) == s2)
		goto fput_exit;

	/* There is also some problem with sending one pipe end's file pointer to the other,
	   however, we new flush Stream head queues on close as well as on qput(). */

	/* SVR 4 SPG says that this message is placed directly on the read queue of the target
	   stream head. */
	err = -EAGAIN;
	if (!canput(s2->sd_rq))
		goto fput_exit;

	{
		mblk_t *mp;
		frtn_t freefd = { freefd_func, (caddr_t) f2 };

		err = -ENOSR;
		if (!(mp = esballoc(NULL, 0, BPRI_MED, &freefd)))
			goto fput_exit;

		mp->b_datap->db_type = M_PASSFP;
		mp->b_rptr = mp->b_wptr = (unsigned char *) &mp->b_datap->db_frtnp->free_arg;
		mp->b_wptr += sizeof(caddr_t);
		put(s2->sd_rq, mp);
		err = 0;
	}

      unlock2_exit:
	srunlock(s2);
      unlock_exit:
	srunlock(sd);
      exit:
	return (err);
      fput_exit:
	printd(("%s: file pointer %p count is now %d\n", __FUNCTION__, f2, file_count(f2) - 1));
	fput(f2);
	goto unlock2_exit;
}

/**
 *  str_i_recvfd: - receive file descriptor streamio(7) %I_RECVFD ioctl
 *  @file: user file pointer for the current open stream
 *  @sd: stream head of current open stream
 *  @arg: ioctl argument, a pointer to a &struct strrecvfd structure
 *
 *  NOTICES: POSIX (and UnixWare) says to return ENXIO if the Stream is hung up, but we don't
 *  initially.  We will attempt to read the queue while STRHUP is set.  Once the queue is empty and
 *  the Stream is hung up, strtestgetfp() will return -ESTRPIPE that we convert into ENXIO.  This is
 *  consistent with read() and getpmsg() behavior and makes sense here.
 */
STATIC inline streams_fastcall int
str_i_recvfd(const struct file *file, struct stdata *sd, unsigned long arg)
{
	struct strrecvfd *srp = (typeof(srp)) arg;
	int fd, err;

	if (!arg)
		return (-EINVAL);

	if (!access_ok(VERIFY_WRITE, srp, sizeof(*srp)))
		return (-EFAULT);

	if ((err = fd = get_unused_fd()) >= 0) {
		queue_t *q = sd->sd_rq;
		mblk_t *mp = NULL;

		if (!(err = straccess_rlock(sd, (FREAD | FEXCL | FNDELAY)))) {
			unsigned long pl;

			/* protect atomicity of STRPRI bit, getq() and putbq() operations */
			pl = zwlock(sd);
			if (IS_ERR(mp = strtestgetfp(sd, q, file->f_flags, &pl)))
				err = PTR_ERR(mp);
			zwunlock(sd, pl);
			srunlock(sd);
		}
		if (!err) {
			struct strrecvfd sr;
			struct file *f2;

			/* we now have a M_PASSFP message in mp */
			f2 = *(struct file **) mp->b_rptr;
			printd(("%s: file pointer %p count is now %d\n", __FUNCTION__, f2,
				file_count(f2)));
			sr.fd = fd;
			sr.uid = f2->f_uid;
			sr.gid = f2->f_gid;
			if (!(err = copyout(&sr, (typeof(&sr)) arg, sizeof(sr)))) {
				fd_install(fd, f2);
				printd(("%s: file pointer %p count is now %d\n", __FUNCTION__, f2,
					file_count(f2)));
				/* we need to do another get because an fput will be done when the
				   mblk is freed */
				get_file(f2);
				printd(("%s: file pointer %p count is now %d\n", __FUNCTION__, f2,
					file_count(f2)));
				freemsg(mp);
				printd(("%s: file pointer %p count is now %d\n", __FUNCTION__, f2,
					file_count(f2)));
			} else {
				/* shouldn't happen, we verified the area before! */
				putbq(q, mp);
			}
		}
		if (err) {
			/* only when we can't block because of hang up */
			if (err == -ESTRPIPE)
				err = -ENXIO;	/* that's what POSIX says */
			put_unused_fd(fd);
		}
	}
	return (err);
}

/**
 *  str_i_str: - perform streamio(7) %I_STR ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @arg: ioctl argument, pointer to a &struct strioctl structure
 *
 *  Constructs an internal STREAMS ioctl() message from the data pointed to by @arg, and sends that
 *  message downstream.
 *
 *  This mechanism is provided to send ioctl() request to downstream modules and drivers.  It allows
 *  information to be sent with ioctl(), and returns to the process any information sent upstream by
 *  the downstream recipient.  %I_STR shall block until the system responds with either a positive
 *  or a negative acknowledgement message, or until the request times out after some period of time.
 *  If the request times out, it shall fail with errno() set to [%ETIME].
 *
 *  At most, one %I_STR can be active on a STREAM.  Further %I_STR calls shall block util the active
 *  %I_STR completes at the STREAM head.  The default timeout interval for these requests is 15
 *  seconds (15,000 milliseconds).  The %O_NONBLOCK flag has not effect on this call.
 *
 *  To send requests downstream, the application shall ensure that @arg points to a stuct strioctl
 *  structure.
 *
 *  The @ic_cmd member is the internal ioctl() command intended for a downstream module or drvier
 *  and @ic_timout is the number of seconds an %I_STR request shall wait for acknowledgement before
 *  timing out.  If @ic_timout is minus one (-1), it specifies that the timeout shall be unlimited.
 *  If @ic_timout is zero (0), it specifies that an implementation-defined default interval is to be
 *  used (usually 15 seconds).  @ic_len is the number of bytes in the data argument, and @ic_dp is a
 *  pointer to the data argument.  The @ic_len member has two uses: on input, it contains the length
 *  of the data argument passed in, and on return from the command, it contains the number of bytes
 *  being returned to the process (the buffer pointed to by @ic_dp should be large enough to contain
 *  the maximum amount of data that any module or the driver in the STREAM can return).
 *
 *  The STREAM head shall convert the information pointed to by the struct strioctl structure to an
 *  internal ioctl() command and send it downstream.
 *
 *  The ioctl() function with the %I_STR command shall fail if:
 *
 *  [%EAGAIN] or [%ENOSR]
 *	Unable to allocate buffers for the ioctl() message.
 *  [%EINVAL]
 *	The @ic_len member is less than 0 or larger than the maximum configured size of the data
 *	part of a message, or @ic_timout is less than minus one (-1).
 *  [%ENXIO]
 *	Hangup received on @file.
 *  [%ETIME]
 *	A downstream ioctl() timed out before acknowledgement was received.
 *
 *  An %I_STR can also fail while waiting for an acknowledgement if a message indicating an error or
 *  a hangup is received at the STREAM head.  In addition, an error code can be returned in the
 *  positive or negative acknowledgement message, in the event the ioctl() command sent downstream
 *  fails.  For these cases, %I_STR shall fail with errno(3) set to the value in the message.
 */
STATIC int
str_i_str(const struct file *file, struct stdata *sd, unsigned long arg)
{
	struct strioctl ic;
	int err, rval;

	if ((err = copyin((typeof(&ic)) arg, &ic, sizeof(ic)))) {
		ptrace(("Error path taken! err = %d\n", err));
		return (err);
	}
	if ((rval = err = strdoioctl_str(sd, &ic, (FREAD | FWRITE | FEXCL | FNDELAY), 1)) < 0) {
		ptrace(("Error path taken! err = %d\n", err));
		return (err);
	}
	/* POSIX says to copy out the ic_len member upon success. */
	if ((err = copyout(&ic.ic_len, &((typeof(&ic)) arg)->ic_len, sizeof(ic.ic_len)))) {
		ptrace(("Error path taken! err = %d\n", err));
		return (err);
	}
	return (rval);
}

/**
 *  str_i_gerropt: - get error options streamio(7) %I_GERROPT ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @arg: ioctl argument, pointer to an integer to contain the returned error options
 */
STATIC int
str_i_gerropt(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int eropt;
	int err;

	if (!access_ok(VERIFY_WRITE, arg, sizeof(eropt))) {
		ptrace(("Error path taken!\n"));
		return (-EFAULT);
	}
	if (!(err = straccess_rlock(sd, FAPPEND))) {
		eropt = sd->sd_eropt & (RERRNONPERSIST | WERRNONPERSIST);
		srunlock(sd);
		err = copyout(&eropt, (typeof(&eropt)) arg, sizeof(eropt));
	}
	return (err);
}

/**
 *  str_i_serropt: - set error options streamio(7) %I_SERROPT ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @arg: ioctl argument, integer error options to set
 */
STATIC int
str_i_serropt(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int eropt = arg;
	int err;

	if ((eropt & ~(RERRNONPERSIST | WERRNONPERSIST)))
		return (-EINVAL);

	if (!(err = straccess_wlock(sd, FEXCL))) {
		sd->sd_eropt = (sd->sd_eropt & ~(RERRNONPERSIST | WERRNONPERSIST)) | eropt;
		swunlock(sd);
	}
	return (err);
}

/**
 *  str_i_anchor: - set a push anchor streamio(7) %I_ANCHOR ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @arg: ioctl argument, integer index at which point to position anchor
 */
STATIC int
str_i_anchor(const struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;

	if (!(err = straccess_wlock(sd, FEXCL))) {
		sd->sd_nanchor = sd->sd_pushcnt;
		swunlock(sd);
	}
	return (err);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  ADDITIONAL PRIVATE IO CONTROLS
 *
 *  -------------------------------------------------------------------------
 * 
 *  Linux Fast-STREAMS System Call Emulation
 *  ----------------------------------------
 */

/**
 *  str_i_getpmsg: - perform getpmsg(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @arg: ioctl argument, a pointer to the &struct strpmsg structure
 */
STATIC inline streams_fastcall int
str_i_getpmsg(struct file *file, struct stdata *sd, unsigned long arg)
{
	struct strpmsg __user *sg = (struct strpmsg *) arg;

	return _strgetpmsg(file, &sg->ctlbuf, &sg->databuf, &sg->band, &sg->flags);
}

/**
 *  str_i_putpmsg: - perform putpmsg(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @arg: ioctl argument, a pointer to the &struct strpmsg structure
 */
STATIC inline streams_fastcall int
str_i_putpmsg(struct file *file, struct stdata *sd, unsigned long arg)
{
	struct strpmsg *sp = (struct strpmsg *) arg;
	int band, flags;
	int err;

	if ((err = copyin(&sp->band, &band, sizeof(&sp->band))))
		return (err);
	if ((err = copyin(&sp->flags, &flags, sizeof(&sp->flags))))
		return (err);
	return _strputpmsg(file, &sp->ctlbuf, &sp->databuf, band, flags);
}

/**
 *  str_i_fattach: - perform fattach(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @arg: a pointer to a character string describing the path
 */
STATIC int
str_i_fattach(struct file *file, struct stdata *sd, unsigned long arg)
{
	char path[256];
	int err;

	if ((err = strncpy_from_user(path, (const char *) arg, 256)) < 0)
		return (err);

	if ((err = straccess_wlock(sd, FEXCL))) {
		err = strfattach(file, path);
		swunlock(sd);
	}
	return (err);
}

/**
 *  str_i_fdetach: - perform fdetach(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @arg: a pointer to a character string describing the path
 *
 *  Note the file and stream are unimportant.  This is a system call emulation.
 */
STATIC int
str_i_fdetach(struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;
	char path[256];

	if ((err = strncpy_from_user(path, (const char *) arg, 256)) < 0)
		return (err);

	return strfdetach(path);
}

#if 0
/**
 *  str_i_pipe: - perform pipe(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @arg: pointer to array into which to receive two file descriptors
 *
 *  Note the file and stream are unimportant.  This is a system call emulation.
 */
STATIC int
str_i_pipe(struct file *file, struct stdata *sd, unsigned long arg)
{
	int fds[2];
	int err;

	if (!access_ok(VERIFY_WRITE, arg, sizeof(fds)))
		return (-EFAULT);

	if (!(err = strpipe(fds)))
		copyout(fds, (int *) arg, sizeof(fds));

	return (err);
}
#else
/**
 *  str_i_pipe: - support pipe(2) system call
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @arg: pointer to array into which to receive two file descriptors
 *
 *  This is done a little different.  Open up one pipe device (/dev/sfx) another pipe device and
 *  then do ioctl(fd1, I_PIPE, fd2) to link them together into a bidirectional pipe.
 */
STATIC int
str_i_pipe(struct file *file, struct stdata *sd, unsigned long arg)
{
	int err;

	if (!(err = straccess_rlock(sd, FCREAT))) {

		err = -EINVAL;
		if (test_bit(STRISPIPE_BIT, &sd->sd_flag)
		    && sd->sd_other == NULL && sd->sd_wq->q_next == sd->sd_rq) {
			struct file *f2;

			err = -EBADF;
			if ((f2 = fget(arg)) && f2->f_op && f2->f_op->release == &strclose) {
				struct stdata *sd2;

				err = -EIO;
				if (ctrace(sd2 = sd_get(stri_lookup(f2)))) {

					err = -EINVAL;
					if (sd != sd2 && !(err = straccess_rlock(sd2, FCREAT))) {

						err = -EINVAL;
						if (test_bit(STRISPIPE_BIT, &sd2->sd_flag)
						    && sd2->sd_other == NULL
						    && sd2->sd_wq->q_next == sd2->sd_rq) {
							unsigned long pl, pl2;

							pl = pwlock(sd);
							pl2 = pwlock(sd2);

							/* weld 'em together */
							sd->sd_other = sd_get(sd2);
							sd2->sd_other = sd_get(sd);

							sd->sd_wq->q_next = sd2->sd_rq;
							sd2->sd_wq->q_next = sd->sd_rq;

							pwunlock(sd2, pl2);
							pwunlock(sd, pl);
							err = 0;
						}
						srunlock(sd2);
					}
					ctrace(sd_put(&sd2));
				}
				fput(f2);
			}
		}
		srunlock(sd);
	}
	return (err);
}
#endif

STATIC int
str_i_default(const struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg,
	      size_t len, int access)
{
	if (len == TRANSPARENT && _IOC_SIZE(cmd) != 0)
		len = _IOC_SIZE(cmd);
	if (len == TRANSPARENT)
		return strdoioctl_trans(sd, cmd, arg, access, 1);

	/* pretend it was issued as an %I_STR ioctl */
	{
		struct strioctl ic;

		ic.ic_cmd = cmd;
		ic.ic_timout = 0;
		ic.ic_len = len;
		ic.ic_dp = (caddr_t) arg;

		return strdoioctl_str(sd, &ic, access, 1);
	}
}

/**
 *  strioctl: - ioctl file operation for a stream
 *  @inode: shadow special filsystem device inode
 *  @file: shadown special filesystem file pointer
 *  @cmd: ioctl command
 *  @arg: ioctl arg
 *
 *  The taking of references and locking strategy for system calls is as follows:
 *
 *  Reference counting is performed on the stream head structure to keep it from being deallocated
 *  at an inopportune time.
 *
 *  To protect the stream head, the stream head read/write lock is used.  The stream head read/write
 *  lock protects both the members of the stdata structure as well as the pointers to the stream
 *  head queues at the top of the structure.
 *
 *  When taking a reference to the stream head, it is acquired from the specfs filesystem inode.
 *  Because it is (vaguely) possible that the stream head has been detached while a reference to the
 *  inode still exists, we take a reference to the stream head.  This ensures that the stream head
 *  is not deallocted until we release our reference.
 *
 *  When processing any system call, we must decide first off if we need to change anything in the
 *  stdata structure, change sd_wq or sd_rq pointers, or change any q->q_next pointer in the entire
 *  stream.  All of these things are protected by the stream head read/write lock.  If we want to
 *  change any of these things, we need to take the head write lock.  Otherwise we still need to
 *  take the stream head read lock to protect dereferencing of the q->q_next pointers in the stream.
 *  After acquiring either lock, we need to check the sd->sd_wq->q_next pointer if we intend to put
 *  any messages to the stream.  For the sd->sd_wq->q_next pointer we can alternately check the
 *  %STRHUP flag, which is set whenever the driver has detached.  (The stream head write lock is
 *  aquired before setting this flag.)  This flag also protects the sd->sd_other pointer that is
 *  used to point to the stream head at the other end of a pipe (i.e., when %STRISPIPE is
 *  set).
 */
int
strioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct stdata *sd = stri_lookup(file);
	int access = FNDELAY;		/* default access check is for io */
	int locking = FREAD;		/* default locking is head read lock */
	int length = TRANSPARENT;	/* default size */
	int err;

	if (unlikely(!sd))
		return (-EIO);

	switch (_IOC_TYPE(cmd)) {
	case _IOC_TYPE(I_STR):
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(I_ATMARK):
			printd(("%s: got I_ATMARK\n", __FUNCTION__));
			return str_i_atmark(file, sd, arg);
		case _IOC_NR(I_CANPUT):
			printd(("%s: got I_CANPUT\n", __FUNCTION__));
			return str_i_canput(file, sd, arg);
		case _IOC_NR(I_CKBAND):
			printd(("%s: got I_CKBAND\n", __FUNCTION__));
			return str_i_ckband(file, sd, arg);
		case _IOC_NR(I_FDINSERT):
			printd(("%s: got I_FDINSERT\n", __FUNCTION__));
			return str_i_fdinsert(file, sd, arg);
		case _IOC_NR(I_FIND):
			printd(("%s: got I_FIND\n", __FUNCTION__));
			return str_i_find(file, sd, arg);
		case _IOC_NR(I_FLUSHBAND):
			printd(("%s: got I_FLUSHBAND\n", __FUNCTION__));
			return str_i_flushband(file, sd, arg);
		case _IOC_NR(I_FLUSH):
			printd(("%s: got I_FLUSH\n", __FUNCTION__));
			return str_i_flush(file, sd, arg);
		case _IOC_NR(I_GETBAND):
			printd(("%s: got I_GETBAND\n", __FUNCTION__));
			return str_i_getband(file, sd, arg);
		case _IOC_NR(I_GETCLTIME):
			printd(("%s: got I_GETCLTIME\n", __FUNCTION__));
			return str_i_getcltime(file, sd, arg);
		case _IOC_NR(I_GETSIG):
			printd(("%s: got I_GETSIG\n", __FUNCTION__));
			return str_i_getsig(file, sd, arg);
		case _IOC_NR(I_GRDOPT):
			printd(("%s: got I_GRDOPT\n", __FUNCTION__));
			return str_i_grdopt(file, sd, arg);
		case _IOC_NR(I_GWROPT):
			printd(("%s: got I_GWROPT\n", __FUNCTION__));
			return str_i_gwropt(file, sd, arg);
		case _IOC_NR(I_LINK):
			printd(("%s: got I_LINK\n", __FUNCTION__));
			return str_i_link(file, sd, arg);
		case _IOC_NR(I_LIST):
			printd(("%s: got I_LIST\n", __FUNCTION__));
			return str_i_list(file, sd, arg);
		case _IOC_NR(I_LOOK):
			printd(("%s: got I_LOOK\n", __FUNCTION__));
			return str_i_look(file, sd, arg);
		case _IOC_NR(I_NREAD):
			printd(("%s: got I_NREAD\n", __FUNCTION__));
			return str_i_nread(file, sd, arg);
		case _IOC_NR(I_PEEK):
			printd(("%s: got I_PEEK\n", __FUNCTION__));
			return str_i_peek(file, sd, arg);
		case _IOC_NR(I_PLINK):
			printd(("%s: got I_PLINK\n", __FUNCTION__));
			return str_i_plink(file, sd, arg);
		case _IOC_NR(I_POP):
			printd(("%s: got I_POP\n", __FUNCTION__));
			return str_i_pop(file, sd, arg);
		case _IOC_NR(I_PUNLINK):
			printd(("%s: got I_PUNLINK\n", __FUNCTION__));
			return str_i_punlink(file, sd, arg);
		case _IOC_NR(I_PUSH):
			printd(("%s: got I_PUSH\n", __FUNCTION__));
			return str_i_push(file, sd, arg);
		case _IOC_NR(I_RECVFD):
			printd(("%s: got I_RECVFD\n", __FUNCTION__));
			return str_i_recvfd(file, sd, arg);
		case _IOC_NR(I_SENDFD):
			printd(("%s: got I_SENDFD\n", __FUNCTION__));
			return str_i_sendfd(file, sd, arg);
		case _IOC_NR(I_SETCLTIME):
			printd(("%s: got I_SETCLTIME\n", __FUNCTION__));
			return str_i_setcltime(file, sd, arg);
		case _IOC_NR(I_SETSIG):
			printd(("%s: got I_SETSIG\n", __FUNCTION__));
			return str_i_setsig(file, sd, arg);
		case _IOC_NR(I_SRDOPT):
			printd(("%s: got I_SRDOPT\n", __FUNCTION__));
			return str_i_srdopt(file, sd, arg);
		case _IOC_NR(I_STR):
			printd(("%s: got I_STR\n", __FUNCTION__));
			return str_i_str(file, sd, arg);
		case _IOC_NR(I_SWROPT):
			printd(("%s: got I_SWROPT\n", __FUNCTION__));
			return str_i_swropt(file, sd, arg);
		case _IOC_NR(I_UNLINK):
			printd(("%s: got I_UNLINK\n", __FUNCTION__));
			return str_i_unlink(file, sd, arg);
			/* are these Solaris specific? */
		case _IOC_NR(I_SERROPT):
			printd(("%s: got I_SERROPT\n", __FUNCTION__));
			return str_i_serropt(file, sd, arg);
		case _IOC_NR(I_GERROPT):
			printd(("%s: got I_GERROPT\n", __FUNCTION__));
			return str_i_gerropt(file, sd, arg);
		case _IOC_NR(I_ANCHOR):
			printd(("%s: got I_ANCHOR\n", __FUNCTION__));
			return str_i_anchor(file, sd, arg);
			/* Linux Fast-STREAMS special ioctls */
		case _IOC_NR(I_GETPMSG):	/* getpmsg syscall emulation */
			printd(("%s: got I_GETPMSG\n", __FUNCTION__));
			return str_i_getpmsg(file, sd, arg);
		case _IOC_NR(I_PUTPMSG):	/* putpmsg syscall emulation */
			printd(("%s: got I_PUTPMSG\n", __FUNCTION__));
			return str_i_putpmsg(file, sd, arg);
		case _IOC_NR(I_PIPE):	/* pipe syscall emulation */
			printd(("%s: got I_PIPE\n", __FUNCTION__));
			return str_i_pipe(file, sd, arg);
		case _IOC_NR(I_FATTACH):	/* fattach syscall emulation */
			printd(("%s: got I_FATTACH\n", __FUNCTION__));
			return str_i_fattach(file, sd, arg);
		case _IOC_NR(I_FDETACH):	/* fdetach syscall emulation */
			printd(("%s: got I_FDETACH\n", __FUNCTION__));
			return str_i_fdetach(file, sd, arg);
#if (_IOC_TYPE(I_STR) != _IOC_TYPE(TCGETS))
		}
		break;
	case _IOC_TYPE(TCGETS):
		switch (_IOC_NR(cmd)) {
#endif
#if 0				/* let these go tranparent */
		case _IOC_NR(TCGETX):	/* SVID */
		case _IOC_NR(TCSETX):	/* SVID */
		case _IOC_NR(TCSETXF):	/* SVID */
		case _IOC_NR(TCSETXW):	/* SVID */
			/* unknown, leave them transparent */
			break;
#endif
		case _IOC_NR(TCSETS):	/* const struct termios * *//* SVID *//* XXX */
		case _IOC_NR(TCSETSW):	/* const struct termios * *//* SVID *//* XXX */
		case _IOC_NR(TCSETSF):	/* const struct termios * *//* SVID *//* XXX */
		case _IOC_NR(TIOCSLCKTRMIOS):	/* const struct termios * *//* XXX */
			access |= FEXCL;
		case _IOC_NR(TCGETS):	/* struct termios * *//* SVID *//* XXX */
		case _IOC_NR(TIOCGLCKTRMIOS):	/* struct termios * *//* XXX */
			length = sizeof(struct termios);
			break;
		case _IOC_NR(TCSETA):	/* const struct termio * *//* SVID *//* XXX */
		case _IOC_NR(TCSETAW):	/* const struct termio * *//* SVID *//* XXX */
		case _IOC_NR(TCSETAF):	/* const struct termio * *//* SVID *//* XXX */
			access |= FEXCL;
		case _IOC_NR(TCGETA):	/* struct termio * *//* SVID *//* XXX */
			length = sizeof(struct termio);
			break;
		case _IOC_NR(TCSBRK):	/* int *//* SVID *//* XXX */
		case _IOC_NR(TCXONC):	/* int *//* SVID *//* XXX */
		case _IOC_NR(TCFLSH):	/* int *//* SVID *//* XXX */
#ifdef TCDSET
		case _IOC_NR(TCDSET):
#endif
			access |= FEXCL;
			break;
#if 0				/* let these go tranparent */
		case _IOC_NR(TIOCFLUSH):	/* int *//* BSD */
			access |= FEXCL;
			/* just use arg, make them transparent */
			break;
		case _IOC_NR(TIOCSCTTY):	/* int *//* XXX */
		case _IOC_NR(TCSBRKP):	/* int *//* used by tcsendbreak *//* XXX */
		case _IOC_NR(TIOCSPTLCK):	/* int *//* Lock/unlock Pty *//* XXX */
			/* just use arg, make them transparent */
			break;
#endif
		case _IOC_NR(TIOCNXCL):	/* void *//* BSD *//* XXX */
		case _IOC_NR(TIOCEXCL):	/* void *//* BSD *//* XXX */
			access |= FEXCL;
			length = 0;	/* no argument - simple return */
			break;
		case _IOC_NR(TIOCCONS):	/* void *//* BSD *//* XXX */
		case _IOC_NR(TIOCNOTTY):	/* void *//* XXX */
		case _IOC_NR(TIOCSERCONFIG):	/* void *//* XXX */
			length = 0;	/* no argument - simple return */
			break;
#if 0
		case _IOC_NR(FIOCLEX):	/* void *//* XXX */
		case _IOC_NR(FIONCLEX):	/* void *//* XXX */
			/* Note: FIOCLEX and FIONCLEX (close_on_exec) and handled by sys_ioctl() */
			length = 0;	/* no argument - simple return */
			break;
		case _IOC_NR(FIOASYNC):	/* const int * *//* XXX */
			/* Note: FIOASYNC is handled by sys_ioctl() and strfasync() and FASYNC. */
		case _IOC_NR(FIONBIO):	/* const int * *//* XXX */
			/* Note: FIONBIO (nonblocking io) is handled by sys_ioctl() and FNDELAY. */
			access |= FEXCL;
			length = sizeof(int);
			break;
		case _IOC_NR(FIOQSIZE):
			/* Note: FIOQSIZE is intercepted by sys_ioctl() */
			length = sizeof(loff_t);
			break;
#endif
		case _IOC_NR(TIOCOUTQ):	/* int * *//* BSD *//* XXX */
#if (_IOC_TYPE(TIOCINQ) == _IOC_TYPE(TCGETS))
		case _IOC_NR(TIOCINQ):	/* int * *//* also FIONREAD *//* XXX */
#endif
#if ((_IOC_TYPE(FIONREAD) == _IOC_TYPE(TCGETS)) && (FIONREAD != TIOCINQ))
		case _IOC_NR(FIONREAD):	/* int * *//* XXX */
#endif
			length = sizeof(int);
			break;
#if 0
		case _IOC_NR(TIOCHPCL):	/* void *//* BSD */
		case _IOC_NR(TIOCSTOP):	/* void *//* BSD */
		case _IOC_NR(TIOCSTART):	/* void *//* BSD */
			access |= FEXCL;
			length = 0;	/* no argument - simple return */
			break;
#endif
		case _IOC_NR(TIOCSBRK):	/* void *//* BSD *//* XXX */
		case _IOC_NR(TIOCCBRK):	/* void *//* BSD *//* XXX */
			access |= FEXCL;
			length = 0;	/* no argument - simple return */
			break;
		case _IOC_NR(TIOCGSID):	/* pid_t * *//* SVID *//* XXX */
			printd(("%s: got TIOCGSID\n", __FUNCTION__));
			return tty_tiocgsid(file, sd, arg);
		case _IOC_NR(TIOCGPGRP):	/* pid_t * *//* SVID *//* XXX */
			printd(("%s: got TIOCGPGRP\n", __FUNCTION__));
			return tty_tiocgpgrp(file, sd, arg);
		case _IOC_NR(TIOCSPGRP):	/* const pid_t * *//* SVID *//* XXX */
			printd(("%s: got TIOCSPGRP\n", __FUNCTION__));
			return tty_tiocspgrp(file, sd, arg);
		case _IOC_NR(TIOCSTI):	/* const char * *//* BSD *//* XXX */
			access |= FEXCL;
			length = sizeof(char);
			break;
		case _IOC_NR(TIOCLINUX):	/* const char * and more *//* XXX */
			/* hmmm */
			break;
		case _IOC_NR(TIOCSWINSZ):	/* const struct winsize * *//* XXX */
			access |= FEXCL;
		case _IOC_NR(TIOCGWINSZ):	/* struct winsize * *//* BSD *//* XXX */
			length = sizeof(struct winsize);
			break;
		case _IOC_NR(TIOCMSET):	/* const int * *//* SVID *//* XXX */
		case _IOC_NR(TIOCMBIS):	/* const int * *//* SVID *//* XXX */
		case _IOC_NR(TIOCMBIC):	/* const int * *//* SVID *//* XXX */
		case _IOC_NR(TIOCSSOFTCAR):	/* const int * *//* XXX */
		case _IOC_NR(TIOCPKT):	/* const int * *//* BSD *//* XXX */
		case _IOC_NR(TIOCSETD):	/* const int * *//* BSD *//* XXX */
		case _IOC_NR(TIOCSERSWILD):	/* const int * *//* XXX */
			access |= FEXCL;
		case _IOC_NR(TIOCMGET):	/* int * *//* SVID *//* XXX */
		case _IOC_NR(TIOCGSOFTCAR):	/* int * *//* XXX */
		case _IOC_NR(TIOCGETD):	/* int * *//* BSD *//* XXX */
		case _IOC_NR(TIOCGPTN):	/* unsigned int * *//* Get Pty Number (of pty-mux device) *//* XXX */
		case _IOC_NR(TIOCSERGWILD):	/* int * *//* XXX */
		case _IOC_NR(TIOCSERGETLSR):	/* int * *//* Get line status register *//* XXX */
			length = sizeof(int);
			break;
#if 0
		case _IOC_NR(TIOCSSERIAL):	/* const struct serial_struct * *//* XXX */
			access |= FEXCL;
		case _IOC_NR(TIOCGSERIAL):	/* struct serial_struct * *//* XXX */
			length = sizeof(struct serial_struct);
			break;
#endif
#ifdef TIOCTTYGSTRUCT
		case _IOC_NR(TIOCTTYGSTRUCT):	/* struct tty_struct * *//* debuggin only */
			length = sizeof(struct tty_struct);
			break;
#endif
#if 0
		case _IOC_NR(TIOCSERGSTRUCT):	/* struct async_struct * *//* debugging only *//* XXX */
			length = sizeof(struct async_struct);
			break;
		case _IOC_NR(TIOCSERSETMULTI):	/* const struct serial_multiport_struct * *//* Set multiport config *//* XXX */
			access |= FEXCL;
		case _IOC_NR(TIOCSERGETMULTI):	/* struct serial_multiport_struct * *//* Get multiport config *//* XXX */
			length = sizeof(struct serial_multiport_struct);
			break;
#endif
#if 0
		case _IOC_NR(TIOCSETP):	/* const struct sgttyb * *//* BSD */
		case _IOC_NR(TIOCSETN):	/* const struct sgttyb * *//* BSD */
			access |= FEXCL;
		case _IOC_NR(TIOCGETP):	/* struct sgttyb * *//* BSD */
			length = sizeof(struct sgttyb);
			break;
		case _IOC_NR(TIOCSETC):	/* const struct tchars * *//* BSD */
			access |= FEXCL;
		case _IOC_NR(TIOCGETC):	/* struct tchars * *//* BSD */
			length = sizeof(struct tchars);
			break;
		case _IOC_NR(TIOCSLTC):	/* const struct ltchars * *//* BSD */
			access |= FEXCL;
		case _IOC_NR(TIOCGLTC):	/* struct ltchars * *//* BSD */
			length = sizeof(struct ltchars);
			break;
		case _IOC_NR(TIOCLBIC):	/* const int * *//* BSD */
		case _IOC_NR(TIOCLBIS):	/* const int * *//* BSD */
		case _IOC_NR(TIOCLSET):	/* const int * *//* BSD */
			access |= FEXCL;
			length = sizeof(int);
			break;
		case _IOC_NR(TIOCLGET):	/* void *//* BSD */
			length = 0;
			break;
#endif
#if 0				/* let these go tranparent */
		case _IOC_NR(TIOCMIWAIT):	/* wait for change on serial input lines *//* XXX */
		case _IOC_NR(TIOCGICOUNT):	/* read serial port inline interrupt counts *//* XXX */
		case _IOC_NR(TIOCGHAYESESP):	/* Get Hayes ESP config *//* XXX */
		case _IOC_NR(TIOCSHAYESESP):	/* Set Hayes ESP config *//* XXX */
		case _IOC_NR(FIOQSIZE):	/* XXX */
			break;
#endif
#if 0				/* let these go tranparent */
		case _IOC_NR(TIOCWINSZ):	/* BSD */
		case _IOC_NR(TIOCUCNTL):	/* BSD */
			break;
		case _IOC_NR(TIOCSDTR):	/* BSD */
		case _IOC_NR(TIOCCDTR):	/* BSD */
		case _IOC_NR(TIOCREMOTE):	/* AT&T */
		case _IOC_NR(TIOCSIGNAL):
		case _IOC_NR(LDSETT):
		case _IOC_NR(LDSMAP):
		case _IOC_NR(DIOCSETP):
			access |= FEXCL;
			break;
#endif
#if (_IOC_TYPE(TCGETS) != _IOC_TYPE(FIOGETOWN))
		}
		break;
	case _IOC_TYPE(FIOGETOWN):
		switch (_IOC_NR(cmd)) {
#endif
		case _IOC_NR(FIOGETOWN):	/* pid_t * */
			printd(("%s: got FIOGETOWN\n", __FUNCTION__));
			return file_fiogetown(file, sd, arg);
			break;
		case _IOC_NR(FIOSETOWN):	/* const pid_t * */
			printd(("%s: got FIOSETOWN\n", __FUNCTION__));
			return file_fiosetown(file, sd, arg);
#if 0
		case _IOC_NR(FIOCLEX):	/* void *//* XXX */
		case _IOC_NR(FIONCLEX):	/* void *//* XXX */
			/* Note: FIOCLEX and FIONCLEX (close_on_exec) and handled by sys_ioctl() */
			length = 0;	/* no argument - simple return */
			break;
		case _IOC_NR(FIOASYNC):	/* const int * *//* XXX */
			/* Note: FIOASYNC is handled by sys_ioctl() and strfasync() and FASYNC. */
		case _IOC_NR(FIONBIO):	/* const int * *//* XXX */
			/* Note: FIONBIO (nonblocking io) is handled by sys_ioctl() and FNDELAY. */
			access |= FEXCL;
			length = sizeof(int);
			break;
		case _IOC_NR(FIOQSIZE):
			/* Note: FIOQSIZE is intercepted by sys_ioctl() */
			length = sizeof(loff_t);
			break;
#endif
#if (_IOC_TYPE(TIOCINQ) == _IOC_TYPE(FIOGETOWN))
		case _IOC_NR(TIOCINQ):
			length = sizeof(int);
			break;
#endif
#if ((_IOC_TYPE(FIONREAD) == _IOC_TYPE(FIOGETOWN)) && (FIONREAD != TIOCINQ))
		case _IOC_NR(FIONREAD):	/* int * *//* XXX */
			length = sizeof(int);
			break;
#endif
#if (_IOC_TYPE(FIOGETOWN) != _IOC_TYPE(SIOCATMARK))
		}
		break;
	case _IOC_TYPE(SIOCATMARK):
		switch (_IOC_NR(cmd)) {
#endif
		case _IOC_NR(SIOCATMARK):	/* int * */
			printd(("%s: got SIOCATMARK\n", __FUNCTION__));
			access |= FREAD;
			return sock_siocatmark(file, sd, arg);
		case _IOC_NR(SIOCSPGRP):	/* const pid_t * */
			printd(("%s: got SIOCSPGRP\n", __FUNCTION__));
			access |= FEXCL;
			length = sizeof(pid_t);
			return sock_siocspgrp(file, sd, arg);
		case _IOC_NR(SIOCGPGRP):	/* pid_t * */
			printd(("%s: got SIOCGPGRP\n", __FUNCTION__));
			length = sizeof(pid_t);
			return sock_siocgpgrp(file, sd, arg);
		}
		break;
	}
	if (locking & FWRITE)
		swlock(sd);
	else if (locking & FREAD)
		srlock(sd);
	if ((access != (FREAD | FWRITE)) || unlikely((err = straccess_slow(sd, access)) != 0))
		err = str_i_default(file, sd, cmd, arg, length, access);
	if (locking & FWRITE)
		swunlock(sd);
	else if (locking & FREAD)
		srunlock(sd);
	return (err);
}

EXPORT_SYMBOL(strioctl);

STATIC long
_strioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct stdata *sd;
	int err = -ENOSTR;

	if (likely((sd = stri_acquire(file)) != NULL)) {
		if (likely(!sd->sd_directio || !sd->sd_directio->ioctl))
			err = strioctl(file, cmd, arg);
		else
			err = sd->sd_directio->ioctl(file, cmd, arg);
		ctrace(sd_put(&sd));
	}
	if (this_thread->flags & (QRUNFLAGS))
		runqueues();	/* after every system call */
	return (err);
}

STATIC int
_strioctl_locked(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	/* never need the inode anyway */
	return _strioctl(file, cmd, arg);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Stream Head file operations
 *
 *  -------------------------------------------------------------------------
 */
struct file_operations strm_f_ops ____cacheline_aligned = {
	.owner = THIS_MODULE,
	.llseek = strllseek,
	.read = _strread,
	.write = _strwrite,
	.poll = _strpoll,
#if HAVE_UNLOCKED_IOCTL
	.unlocked_ioctl = _strioctl,
#endif
	.ioctl = _strioctl_locked,
	.mmap = strmmap,
	.open = stropen,
	.flush = strflush,
	.release = strclose,
	.fasync = strfasync,
	.sendpage = _strsendpage,
#ifdef HAVE_PUTPMSG_GETPMSG_FILE_OPS
	.getpmsg = _strgetpmsg,
	.putpmsg = _strputpmsg,
#endif
};

EXPORT_SYMBOL(strm_f_ops);

/**
 *  strwput: - stream head write queue put procedure
 *  @q: pointer to the queue to which to put the message
 *  @mp: the message to put on the queue
 *
 *  Ala SVR4 se do not have a real write side service procedure and we do not normally put messages
 *  to the write side queue for the stream head.  The only time that we put a message on the write
 *  side queue is for STRHOLD processing where we want to temporarily defer a message for some short
 *  interval (10ms) while waiting for another.  For all this to work, the write queue needs the
 *  QNOENAB flag set.
 *
 *  To avoid a bunch of locking work in strwrite(), strputpmsg() and friends, the put procedure on
 *  the STREAM head write queue is called without STREAM head locks, so we need to take locks and
 *  check a bunch of things here, before attempting a putnext().
 */
int
strwput(queue_t *q, mblk_t *mp)
{
	struct stdata *sd;

	assert(q);
	assert(mp);

	sd = qstream(q);

	assert(sd);

	if (!q->q_next) {
		/* nothing we can do */
		freemsg(mp);
		swerr();
	} else {
		unsigned long pl;
		mblk_t *bp;

		pl = zwlock(sd);
		if ((bp = q->q_first)) {
			rmvq(q, bp);
			zwunlock(sd, pl);
			/* delayed one has to go - can't delay the other */
			ctrace(putnext(q, bp));
			ctrace(putnext(q, mp));
		} else if (test_bit(STRDELIM_BIT, &sd->sd_flag)
			   || !test_bit(STRHOLD_BIT, &sd->sd_flag)
			   || bp == mp
			   || mp->b_datap->db_type != M_DATA
			   || mp->b_flag & MSGDELIM
			   || mp->b_cont
			   || mp->b_wptr == mp->b_rptr
			   || mp->b_wptr > mp->b_rptr + (FASTBUF >> 1)) {
			/* Feature not activated, old message with more data, or not M_DATA, or
			   delimited, or longer than one block or a zero-length message, or can't
			   hold another write same size. */
			zwunlock(sd, pl);
			ctrace(putnext(q, mp));
			trace();
		} else {
			/* new M_DATA message with more room */
			insq(q, NULL, mp);	/* putq without locks */
			zwunlock(sd, pl);
			/* TODO: need to handle 10ms timeout */
		}
	}
	return (0);
}

EXPORT_SYMBOL(strwput);

/**
 *  strwsrv: - STREAM head write queue service procedure
 *  @q: write queue to service
 *
 *  We don't actually ever put a message on the write queue, we just use the service procedure to
 *  wake up any synchronous or asynchronous waiters waiting for flow control to subside.  This
 *  permit back-enabling from the module beneath the stream head to work properly.  So, for example,
 *  when we do a canputnext(sd->sd_wq) it sets the QWANTW on sd->sd_wq->q_next if the module below
 *  the stream head is flow controlled.  When congestion subsides to the low water mark, the queue
 *  will backenable us and we will run and wake up any waiters blocked on flow control.
 *
 *  Note that we must also trigger stream events here.  Two in particular are S_WRNORM and S_WRBAND.
 *  We altered qbackenable to set the QBACK bit on the queue and the QB_BACK bit on a queue band
 *  when it backenables the queue.  This is an indication that flow control has subsided for the
 *  queue or queue band (or that the queue or queue band was just emptied).  We use these flags only
 *  for signalling streams events.
 */
int
strwsrv(queue_t *q)
{
	struct stdata *sd;
	unsigned long pl;
	struct qband *qb;
	int band;
	unsigned char be[NBAND] = { 0, };

	assert(q);

	sd = qstream(q);

	assert(sd);

	pl = qrlock(q);
	if (test_and_clear_bit(QBACK_BIT, &q->q_flag))
		be[0] = 1;
	for (qb = q->q_bandp, band = q->q_nband; qb; qb = qb->qb_next, band--)
		if (test_and_clear_bit(QB_BACK_BIT, &qb->qb_flag))
			be[band] = 1;
	qrunlock(q, pl);

	if (be[0])
		strevent(sd, S_WRNORM, 0);
	for (band = q->q_nband; band > 0; band--)
		if (be[band])
			strevent(sd, S_WRBAND, band);

	strwakewrite(sd);
	return (0);
}

/*
 *  Read Message Handling
 *  =========================================================================
 */

STATIC inline streams_fastcall int
str_m_pcproto(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	unsigned long pl;

	/* MG 7.9.6:- "M_DATA, M_PROTO, M_PCPROTO, M_PASSFP -- after checking the message type, the 
	   messae is placed on the stream head read queue.  If the message is an M_PCPROTO the
	   STRPRI flag is set in the stream head, indicating the presence of an M_PCPROTO message.
	   If this bit is already set, indicating than an M_PCPROTO message is already present, the 
	   new message is discarded.  The relevant processes are then woken up or signalled. */
	/* protect atomicity of STRPRI bit and putq() */
	pl = zwlock(sd);
	if (test_and_set_bit(STRPRI_BIT, &sd->sd_flag)) {
		zwunlock(sd, pl);
		freemsg(mp);
	} else {
		insq(q, NULL, mp);	/* frozen equivalent to putq() */
		zwunlock(sd, pl);
		strwakeread(sd);
		strevent(sd, S_HIPRI, 0);
	}
	return (0);
}

STATIC inline streams_fastcall int
str_m_data(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	int band = mp->b_band;

	putq(q, mp);
	strwakeread(sd);
	strevent(sd, (S_INPUT | (band ? S_RDBAND : S_RDNORM)), band);
	return (0);
}

STATIC inline streams_fastcall int
str_m_flush(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	/* Notes: OpenSolaris has this RFLUSHPCPROT read option to not flush M_PCPROTO messages
	   from the STREAM head.  I don't really understand the need for this. Because strrput
	   ensures that there is only one M_PCPROTO message on the STREAM head read queue, if one
	   exists, it is first on the queue.  It would be easy enough to hold onto it, but I don't
	   see the need. */
	trace();
	if (mp->b_rptr[0] & FLUSHR) {
		trace();
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else {
			flushq(q, FLUSHALL);
			/* If we flushed a priority message, we need to fix up the STRPRI bit which 
			   is protected by the queue write lock.  Note that if another M_PCPROTO
			   message arrives between the flushq() and the zwlock() it deserves to be
			   discarded under the M_PCPROTO case above anyway. */
			if (test_bit(STRPRI_BIT, &sd->sd_flag)
			    || test_bit(STRMSIG_BIT, &sd->sd_flag)) {
				mblk_t *b;
				unsigned long pl;

				pl = zwlock(sd);
				b = q->q_first;
				if (!b || b->b_datap->db_type != M_PCPROTO)
					clear_bit(STRPRI_BIT, &sd->sd_flag);
				if (!b || b->b_datap->db_type != M_SIG)
					clear_bit(STRMSIG_BIT, &sd->sd_flag);
				zwunlock(sd, pl);
			}
		}
		mp->b_rptr[0] &= ~FLUSHR;
	}
	/* We set MSGNOLOOP on flush messages returned downstream that if echoed back up in error
	   are freed at the stream head. */
	trace();
	if (!(mp->b_flag & MSGNOLOOP) && _WR(q)->q_next) {
		trace();
		if (mp->b_rptr[0] & FLUSHW) {
			trace();
			if (mp->b_rptr[1] & FLUSHBAND)
				flushband(_WR(q), mp->b_rptr[1], FLUSHALL);
			else
				flushq(_WR(q), FLUSHALL);
			mp->b_flag |= MSGNOLOOP;
			ctrace(qreply(q, mp));
			trace();
			return (0);
		}
	}
	trace();
	freemsg(mp);
	trace();
	return (0);
}

#ifndef RERRMASK
#define RERRMASK (RERRNORM|RERRNONPERSIST)
#endif
#ifndef WERRMASK
#define WERRMASK (WERRNORM|WERRNONPERSIST)
#endif

STATIC inline streams_fastcall int
str_m_setopts(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	struct stroptions *so = (typeof(so)) mp->b_rptr;

	if (so->so_flags & (SO_MREADON | SO_MREADOFF)) {
		if (so->so_flags & SO_MREADON)
			set_bit(SNDMREAD_BIT, &sd->sd_flag);
		if (so->so_flags & SO_MREADOFF)
			clear_bit(SNDMREAD_BIT, &sd->sd_flag);
	}
	if (so->so_flags & (SO_NDELON | SO_NDELOFF)) {
		if (so->so_flags & SO_NDELON)
			set_bit(STRNDEL_BIT, &sd->sd_flag);
		if (so->so_flags & SO_NDELOFF)
			clear_bit(STRNDEL_BIT, &sd->sd_flag);
	}
	if (so->so_flags & SO_STRHOLD)
		set_bit(STRHOLD_BIT, &sd->sd_flag);
	if (so->so_flags & (SO_ISTTY | SO_ISNTTY)) {
		if (so->so_flags & SO_ISTTY) {
			set_bit(STRISTTY_BIT, &sd->sd_flag);
			/* FIXME: Do we have to do more here? This bit indicates that this Stream
			   is a controlling tty.  Do we need to figure out the process group of the 
			   user and set sd->sd_pgrp? (Done by I_PUSH, see str_i_push().) */
		}
		if (so->so_flags & SO_ISNTTY)
			clear_bit(STRISTTY_BIT, &sd->sd_flag);
	}
	if (so->so_flags & (SO_TOSTOP | SO_TONSTOP)) {
		if (so->so_flags & SO_TOSTOP)
			set_bit(STRTOSTOP_BIT, &sd->sd_flag);
		if (so->so_flags & SO_TONSTOP)
			clear_bit(STRTOSTOP_BIT, &sd->sd_flag);
	}
	if (so->so_flags & (SO_DELIM | SO_NODELIM)) {
		if (so->so_flags & SO_DELIM)
			set_bit(STRDELIM_BIT, &sd->sd_flag);
		if (so->so_flags & SO_NODELIM)
			clear_bit(STRDELIM_BIT, &sd->sd_flag);
	}
	if (so->so_flags & SO_READOPT)
		sd->sd_rdopt = so->so_flags & (RMODEMASK | RPROTMASK);
	if (so->so_flags & SO_WROFF)
		sd->sd_wroff = so->so_wroff;
	if (so->so_flags & SO_MINPSZ)
		strqset(q, QMINPSZ, 0, so->so_minpsz);
	if (so->so_flags & SO_MAXPSZ)
		strqset(q, QMAXPSZ, 0, so->so_maxpsz);
	if (so->so_flags & SO_BAND) {
		if (so->so_flags & SO_HIWAT)
			strqset(q, QHIWAT, so->so_band, so->so_hiwat);
		if (so->so_flags & SO_LOWAT)
			strqset(q, QLOWAT, so->so_band, so->so_lowat);
	} else {
		if (so->so_flags & SO_HIWAT)
			strqset(q, QHIWAT, 0, so->so_hiwat);
		if (so->so_flags & SO_LOWAT)
			strqset(q, QLOWAT, 0, so->so_lowat);
	}
	if (so->so_flags & SO_ERROPT)
		sd->sd_eropt = so->so_erropt & (RERRMASK | WERRMASK);
	/* don't do these two */
	if (so->so_flags & SO_MAXBLK) ;
	if (so->so_flags & SO_COPYOPT) ;
	return (0);
}

STATIC inline streams_fastcall int
str_m_sig(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	unsigned long pl;

	/* MG 7.9.6:- "M_SIG, M_PCSIG -- these messages send signals to the process group. The
	   first byte of the message contains the number of the signal to send.  For an M_PCSIG
	   message the signal is sent immediately.  For an M_SIG message it is placed on the read
	   queue and the signal is sent when the message is read from the queue by strread().  If
	   the signal is SIGPOLL, it will be sent only to the processes that requested it with the
	   I_SETSIG ioctl.  Other signals are sent to a process only if the stream is associated
	   with the control terminal (see 7.11.2)." */
	pl = zwlock(sd);
	insq(q, NULL, mp);	/* frozen equivalent to putq() */
	if (q->q_first->b_datap->db_type == M_SIG)
		set_bit(STRMSIG_BIT, &sd->sd_flag);
	zwunlock(sd, pl);
	strwakeread(sd);
	return (0);
}

STATIC inline streams_fastcall int
str_m_pcsig(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	strsignal(sd, mp);
	return (0);
}

STATIC inline streams_fastcall int
str_m_error(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	/* MG 7.9.6:- "M_ERROR -- when this message type is received, the first byte of the message 
	   is put into the stream head (sd_rerror) and the STRDERR flag is set.  The second byte is 
	   placed in sd_werror and the STWERR flag is set.  If the error code in the message has
	   the special value NOERROR, the corresponding flag is not set. However, if the error flag 
	   is set, a read owr write will fail, and the value returned to the process in errno is
	   taken from sd_rerror or sd_werror.  The error condition prevents all further reads and
	   writes to the stream until it is closed, or the stream head receives a NOERROR code in
	   an M_ERROR message.  The relevant processes are then awoken or signalled.  Finally, an
	   M_FLUSH message is sent downstream to discard all data queued in the stream. */
	int what = 0;

	if (mp->b_wptr < mp->b_rptr + 2) {
		/* First we want to handle some backwards compatibility: originally, M_ERROR
		   message contained only one byte of information that applied both to the read and 
		   the write side.  This permits the older messages to be sent as well as the newer 
		   ones. */
		if (mp->b_wptr <= mp->b_rptr) {
			/* If it is a zero-length * M_ERROR message, then assume it is the same as
			   an M_ERROR message with (NOERROR,NOERROR). */
			mp->b_wptr = mp->b_rptr + 2;
			mp->b_rptr[0] = NOERROR;
			mp->b_rptr[1] = NOERROR;
		} else if (mp->b_wptr == mp->b_rptr + 1) {
			/* If it is a single byte message containing XXX, assume that it is the
			   same as (XXX,XXX). */
			mp->b_wptr = mp->b_rptr + 2;
			mp->b_rptr[1] = mp->b_rptr[0];
		}
	}
	if (mp->b_rptr[0] != (typeof(mp->b_rptr[0])) NOERROR) {
		sd->sd_rerror = mp->b_rptr[0];
		if (!test_and_set_bit(STRDERR_BIT, &sd->sd_flag)) {
			strwakeread(sd);
			what |= FLUSHR;
		}
	} else {
		clear_bit(STRDERR_BIT, &sd->sd_flag);
		sd->sd_rerror = NOERROR;
	}
	if (mp->b_rptr[1] != (typeof(mp->b_rptr[1])) NOERROR) {
		sd->sd_werror = mp->b_rptr[1];
		if (!test_and_set_bit(STWRERR_BIT, &sd->sd_flag)) {
			strwakewrite(sd);
			what |= FLUSHW;
		}
	} else {
		clear_bit(STWRERR_BIT, &sd->sd_flag);
		sd->sd_werror = NOERROR;
	}
	/* only interruptible and only if IOCWAIT bit set */
	strwakeiocwait(sd);
	/* send a flush if required */
	if (what) {
		strevent(sd, S_ERROR, 0);
		if (_WR(q)->q_next) {
			mp->b_datap->db_type = M_FLUSH;
			mp->b_band = 0;
			mp->b_rptr[0] = what;
			mp->b_rptr[1] = 0;
			mp->b_flag = 0;
			qreply(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}

STATIC inline streams_fastcall int
str_m_hangup(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	/* MG 7.9.6:- "M_HANGUP -- if this message is received, the stream head is marked hung up.
	   ANy subsequent reads from the stream will return zero, and writes will fail with the EIO 
	   error code.  As with the other message types, the relevant processes are awoken or
	   signalled. */
	/* POSIX:- "If a modem disconnect is detected by the terminal interface for a controlling
	   terminal, and if CLOCAL is not set in the c_cflag field for the terminal (see Control
	   Modes), the SIGHUP signal shall be sent to the controlling process for which the
	   terminal is the controlling terminal. Unless other arrangements have been made, this
	   shall cause the controlling process to terminated (see exit()).  Any subsequent read
	   from the terminal device shall return the value of zero, indicating end-of-file; see
	   read(). Then, processes that read a terminal file and test for end-of-file can terminate 
	   appropriately after a disconnect.  If the EIO condition as specified in read() also
	   exists, it is unspecified whether on (sic) EOF condition or [EIO] is returned.  Any
	   subsequent write() to the terminal device shall return -1, with errno set to [EIO],
	   until the device is closed." */
	strhangup(sd);
	freemsg(mp);
	return (0);
}

STATIC inline streams_fastcall int
str_m_unhangup(struct stdata *sd, queue_t *q, mblk_t *mp)
{
#if 0
	if (test_and_clear_bit(STRHUP_BIT, &sd->sd_flag))
		strevent(sd, S_HANGUP, 0);
#else
	clear_bit(STRHUP_BIT, &sd->sd_flag);
#endif
	freemsg(mp);
	return (0);
}

STATIC inline streams_fastcall int
str_m_copy(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	if (!ctrace(strwakeiocack(sd, mp))) {
		if (_WR(q)->q_next) {
			union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;

			/* allow module or driver to free cp_private */
			ptrace(("Rejecting M_COPYIN/M_COPYOUT.\n"));
			mp->b_datap->db_type = M_IOCDATA;
			mp->b_wptr = mp->b_rptr + sizeof(ioc->copyresp);
			ioc->copyresp.cp_rval = (caddr_t) 1;
			qreply(q, mp);
		} else {
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

STATIC inline streams_fastcall int
str_m_ioc(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	if (!ctrace(strwakeiocack(sd, mp)))
		freemsg(mp);
	return (0);
}

STATIC inline streams_fastcall int
str_m_ioctl(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	/* MG 7.9.6:- "M_IOCTL -- It does not make sense to receive on of these messages at the
	   stream head, so rather than quietly discarding it, a response is generated by converting 
	   it into an M_IOCNAK message, which is then sent back up the stream." These messages
	   could arrive here if we are a pipe or FIFO and, for example, a I_STR input-output
	   control were to be invoked on the stream head at the other side.  This is possibly a
	   common mistake, and responding in this way keeps processes from getting hung in the
	   ioctl at the opposite stream head. */
	if (_WR(q)->q_next) {
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;

		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_error = EINVAL;
		ioc->iocblk.ioc_rval = -1;
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

STATIC inline streams_fastcall int
str_m_letsplay(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	struct strlp *lp = (typeof(lp)) mp->b_rptr;

	if (lp->lp_count == sd->sd_pushcnt) {
		if (_WR(q)->q_next) {
			/* we don't queue data in the stream head write queue */
			mp->b_datap->db_type = M_BACKDONE;
			qreply(q, mp);
			return (0);
		}
		freemsg(mp);
	} else {
		mp->b_datap->db_type = M_DONTPLAY;
		ctrace(put(lp->lp_queue, mp));
		trace();
	}
	return (0);
}

STATIC inline streams_fastcall int
str_m_other(struct stdata *sd, queue_t *q, mblk_t *mp)
{
	/* Other messages are silently discarded */
	freemsg(mp);
	return (0);
}

/**
 *  strrput: - STREAM head read queue put procedure
 *  @q: pointer to the queue to which to put the message
 *  @mp: the message to put on the queue
 *
 *  The only priority message that we put on the queue is the M_PCPROTO message (and only one of
 *  those is allowed at a time).  The only normal messages that we put on the queue are the M_DATA,
 *  M_PROTO, M_PASSFP and M_SIG messages.  All other messages are dealt with directly.  In that way,
 *  when the driver beneath the stream head replies to most messages, such as M_IOCTL, the reply is
 *  handled immediately and without the need to invoke the STREAMS scheduler.  Also, this procedure
 *  uses atomic bit operations and other SMP safeguards and is run without any synchronization
 *  queues, meaning that put calls are never deferred.
 *
 *  NOTICES: Note that there is no read service procedure, even though we put to the queue.  This is
 *  ok because we are at the end of the stream, so our QFULL and QBFULL flags will still be checked
 *  and failure returned to (and QWANTW and QBWANTW flags set by) canput() and bcanput() in the
 *  proper circumstances, meaning the back-enabling will occur when getq() and rmvq() are called.
 *
 *  For a description of most of these operations, see Magic Garden Section 7.9.6.
 *
 *  In stead of putting everything in one big case statement (as is the practice for STREAMS), we
 *  do it with inlines so that we have a call stack in debug mode.
 */
int
strrput(queue_t *q, mblk_t *mp)
{
	struct stdata *sd;
	int err;

	assert(q);
	assert(mp);

	sd = qstream(q);

	assert(sd);

	switch (mp->b_datap->db_type) {
	case M_PCPROTO:	/* bi - protocol info */
		printd(("%s: got M_PCPROTO\n", __FUNCTION__));
		err = str_m_pcproto(sd, q, mp);
		break;
	case M_DATA:		/* bi - data */
	case M_PROTO:		/* bi - protocol info */
	case M_PASSFP:		/* bi - pass file pointer */
		printd(("%s: got M_DATA, M_PROTO or M_PASSFP\n", __FUNCTION__));
		err = str_m_data(sd, q, mp);
		break;
	case M_FLUSH:		/* bi - flush queues */
		printd(("%s: got M_FLUSH\n", __FUNCTION__));
		err = str_m_flush(sd, q, mp);
		break;
	case M_SETOPTS:	/* up - set stream head options */
	case M_PCSETOPTS:	/* up - set stream head options */
		printd(("%s: got M_SETOPTS, M_PCSETOPTS\n", __FUNCTION__));
		err = str_m_setopts(sd, q, mp);
		break;
	case M_SIG:		/* up - signal */
		printd(("%s: got M_SIG\n", __FUNCTION__));
		err = str_m_sig(sd, q, mp);
		break;
	case M_PCSIG:		/* up - signal */
		printd(("%s: got M_PCSIG\n", __FUNCTION__));
		err = str_m_pcsig(sd, q, mp);
		break;
	case M_ERROR:		/* up - report error */
		printd(("%s: got M_ERROR\n", __FUNCTION__));
		err = str_m_error(sd, q, mp);
		break;
	case M_HANGUP:		/* up - report hangup */
		printd(("%s: got M_HANGUP\n", __FUNCTION__));
		err = str_m_hangup(sd, q, mp);
		break;
	case M_UNHANGUP:	/* up - report recovery */
		printd(("%s: got M_UNHANGUP\n", __FUNCTION__));
		err = str_m_unhangup(sd, q, mp);
		break;
	case M_COPYIN:		/* up - copy data from user */
	case M_COPYOUT:	/* up - copy data to user */
		printd(("%s: got M_COPYIN or M_COPYOUT\n", __FUNCTION__));
		err = str_m_copy(sd, q, mp);
		break;
	case M_IOCACK:		/* up - acknolwedge ioctl */
	case M_IOCNAK:		/* up - refuse ioctl */
		printd(("%s: got M_IOCACK or M_IOCNAK\n", __FUNCTION__));
		err = str_m_ioc(sd, q, mp);
		break;
	case M_IOCTL:		/* dn - io control */
		printd(("%s: got M_IOCTL\n", __FUNCTION__));
		err = str_m_ioctl(sd, q, mp);
		break;
	case M_LETSPLAY:	/* up - AIX only */
		printd(("%s: got M_LETSPLAY\n", __FUNCTION__));
		err = str_m_letsplay(sd, q, mp);
		break;
	default:
		printd(("%s: got other message\n", __FUNCTION__));
#if 0
	case M_BREAK:		/* dn - request to send "break" */
	case M_DELAY:		/* dn - request delay on output */
	case M_IOCDATA:	/* dn - copy data response */
	case M_READ:		/* dn - read notification */
	case M_STOP:		/* dn - suspend output */
	case M_START:		/* dn - resume output */
	case M_STARTI:		/* dn - resume input */
	case M_STOPI:		/* dn - suspend input */
	case M_BACKWASH:	/* dn - backwash clearing */
	case M_BACKDONE:	/* dn - backwash clearning done */
	case M_DONTPLAY:	/* dn - direct io not permitted */
	case M_CTL:		/* bi - control info */
	case M_PCCTL:		/* bi - control info */
	case M_EVENT:		/* -- - Solaris only? */
	case M_PCEVENT:	/* -- - Solaris only? */
	case M_RSE:		/* -- - reserved */
	case M_PCRSE:		/* -- - reserved */
#endif
		err = str_m_other(sd, q, mp);
		break;
	}
	return (err);
}

#if defined CONFIG_STREAMS_FIFO_MODULE || !defined CONFIG_STREAMS_FIFO \
 || defined CONFIG_STREAMS_PIPE_MODULE || !defined CONFIG_STREAMS_PIPE \
 || defined CONFIG_STREAMS_SOCK_MODULE || !defined CONFIG_STREAMS_SOCK
EXPORT_SYMBOL(strrput);
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  str_open:	- STREAMS qopen procedure for stream heads
 *  @q:		read queue of stream to open
 *  @devp:	pointer to a dev_t from which to read and into which to return the device number
 *  @oflag:	open flags
 *  @sflag:	STREAMS flags (%DRVOPEN or %MODOPEN or %CLONEOPEN)
 *  @crp:	pointer to user's credentials structure
 *
 *  These could be separated into str_open, fifo_qopen and pipe_qopen but it is simpler to just
 *  leave them all in one.
 */
int
str_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct stdata *sd;
	int err = 0;

	if (!(sd = qstream(q)))
		return (-EIO);

	/* Linux cannot currently specify no read-write open flags.  O_RDONLY is defined as zero
	   (0) ala the historical SVR3 behaviour despite longstanding POSIX recommendations to the
	   contrary.  But we check anyway in case that changes in the future. */

	/* must be opened for at least read or write */
	if ((sd->sd_flag & (STRISFIFO | STRISPIPE | STRISSOCK)) && !(oflag & (FWRITE | FREAD)))
		return (-EINVAL);

	/* already open */
	if (q->q_ptr)
		return (0);

	/* cannot be opened as a module */
	if (sflag == MODOPEN || _WR(q)->q_next != NULL)
		return (-EIO);

	/* stream head first open */
	if (!(sd->sd_flag & (STRISPIPE | STRISFIFO | STRISSOCK))) {
		if ((err = qattach(sd, (struct fmodsw *) sd->sd_cdevsw, devp, oflag, sflag, crp)))
			goto error;
		/* qattach() above does the right thing with regard to setq() */
	} else if (sd->sd_flag & STRISFIFO) {
		/* fifos cannot be clone opened */
		if (sflag == CLONEOPEN)
			return (-ENXIO);
		/* start off life as a fifo: named pipe */
		_WR(q)->q_next = q;
	} else if (sd->sd_flag & STRISPIPE) {
		static int pipe_minor = 0;
		static spinlock_t pipe_lock = SPIN_LOCK_UNLOCKED;
		minor_t minor;

		/* pipes must be clone opened */
		if (sflag != CLONEOPEN)
			return (-ENXIO);

		spin_lock(&pipe_lock);
		minor = ++pipe_minor;
		spin_unlock(&pipe_lock);

		/* start off life as a fifo: unidirectional unnamed pipe */
		_WR(q)->q_next = q;

		*devp = makedevice(getmajor(*devp), minor);
		/* don't worry, if we wrap we will reuse inodes but not Stream heads */
	} else if (sd->sd_flag & STRISSOCK) {
		/* leave as null device for now */
	}
	/* lastly, attach our privates and return */
	q->q_ptr = _WR(q)->q_ptr = sd;
      error:
	return (err > 0 ? -err : err);
}

EXPORT_SYMBOL(str_open);

/**
 *  str_close: - STREAMS qclose procedure for stream heads
 *  @q: read queue of stream to close
 *  @oflag: open flags
 *  @crp: pointer to user's credentials structure
 *
 *  XXX: Should really move all of strlastclose() to here.
 */
int
str_close(queue_t *q, int oflag, cred_t *crp)
{
	if (q->q_ptr == NULL)
		return (-ENXIO);
	q->q_ptr = _WR(q)->q_ptr = NULL;
	return (0);
}

EXPORT_SYMBOL(str_close);

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

/* Note: we do not register stream heads - the above operations are exported and used by all
   regular STREAMS devices */

/* Perhaps we should...  The stream head should be entered into the fmodsw table at position 0,
   with module id 0 */

/* We actually register it a position 1 with module id 1 */

/* Note that we use D_MP flag here because all our put and service routines are MP-SAFE and we don't
 * want to have any synchronization queues attached to the stream head. */

struct fmodsw sth_fmod = {
	.f_name = CONFIG_STREAMS_STH_NAME,
	.f_str = &str_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#if 0
/* bleedin' mercy! */
STATIC void
put_filesystem(struct file_system_type *fs)
{
#if HAVE_KFUNC_MODULE_PUT
	module_put(fs->owner);
#else
	if (fs->owner)
		__MOD_DEC_USE_COUNT(fs->owner);
#endif
}
#endif

#ifdef CONFIG_STREAMS_STH_MODULE
STATIC
#endif
int __init
sth_init(void)
{
	int result;

#ifdef CONFIG_STREAMS_STH_MODULE
	printk(KERN_INFO STH_BANNER);
#else
	printk(KERN_INFO STH_SPLASH);
#endif
	str_minfo.mi_idnum = modid;
	if ((result = register_strmod(&sth_fmod)) < 0)
		goto no_strmod;
	if (modid == 0 && result >= 0)
		modid = result;
	return (0);
      no_strmod:
	return (result);
}

#ifdef CONFIG_STREAMS_STH_MODULE
STATIC
#endif
void __exit
sth_exit(void)
{
	unregister_strmod(&sth_fmod);
}

#ifdef CONFIG_STREAMS_STH_MODULE
module_init(sth_init);
module_exit(sth_exit);
#endif				/* CONFIG_STREAMS_STH_MODULE */
