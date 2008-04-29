/*****************************************************************************

 @(#) $RCSfile: wait.c,v $ $Name:  $($Revision: 1.1.1.2.4.5 $) $Date: 2008-04-29 08:33:13 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2008-04-29 08:33:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: wait.c,v $
 Revision 1.1.1.2.4.5  2008-04-29 08:33:13  brian
 - update headers for Affero release

 Revision 1.1.1.2.4.4  2007/08/14 10:47:02  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: wait.c,v $ $Name:  $($Revision: 1.1.1.2.4.5 $) $Date: 2008-04-29 08:33:13 $"

static char const ident[] = "$RCSfile: wait.c,v $ $Name:  $($Revision: 1.1.1.2.4.5 $) $Date: 2008-04-29 08:33:13 $";

/*                               -*- Mode: C -*- 
 * wait.c --- wait queues management
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: wait.c,v 1.1.1.2.4.5 2008-04-29 08:33:13 brian Exp $
 * Purpose         : keep close all the wait related stuff.
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Francisco J. Ballesteros
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach me by email to
 *    nemo@ordago.uc3m.es
 */

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/strport.h>
#include <sys/strconfig.h>	/* config definitions */
#include <sys/LiS/share.h>	/* streams shared defs */
#include <sys/LiS/wait.h>	/* interface */
#include <sys/LiS/head.h>	/* stream head */
#include <sys/LiS/queue.h>	/* streams queues */
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

#define	AINC(v)		K_ATOMIC_INC(&(v))
#define	ADEC(v)		K_ATOMIC_DEC(&(v))

/*  -------------------------------------------------------------------  */
/*				 Global vars                             */
long lis_wakeup_close_wt_cnt;
long lis_sleep_on_close_wt_cnt;

extern lis_atomic_t lis_in_syscall;	/* in head.c */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/* 
 * When we are about to sleep, and we have the stream head unlocked,
 * check to see if we need to run the queues.  Often we sleep after having
 * sent something downstream to the driver, so this is a good place
 * to check for queues to run.
 */
/*  -------------------------------------------------------------------  */
int
lis_sleep_on_close_wt(void *q_str)
{
	stdata_t *sd = (stdata_t *) q_str;
	int rslt;

	if (sd == NULL || sd->magic != STDATA_MAGIC)
		return (-ENOENT);

	lis_clear_and_save_sigs(sd);	/* undo pending signals */
	lis_sleep_on_close_wt_cnt++;
	K_ATOMIC_DEC(&lis_in_syscall);	/* "done" with a system call */
	lis_runqueues();
	rslt = lis_down(&sd->sd_close_wt);
	K_ATOMIC_INC(&lis_in_syscall);	/* processing a system call */
	lis_restore_sigs(sd);	/* restore old signals */
	return (rslt);

}				/* lis_sleep_on_close_wt */

/*  -------------------------------------------------------------------  */
void
lis_wakeup_close_wt(void *q_str)
{
	stdata_t *sd = (stdata_t *) q_str;

	if (sd != NULL && sd->magic == STDATA_MAGIC
	    && F_ISSET(sd->sd_flag, (STRCLOSEWT | STRFLUSHWT))
	    ) {
		lis_wakeup_close_wt_cnt++;
		lis_up(&sd->sd_close_wt);
	}

}				/* lis_wakeup_close_wt */

/*  -------------------------------------------------------------------  */
int
lis_sleep_on_wioc(struct stdata *sd, char *f, int l)
{
	int rslt;

	K_ATOMIC_DEC(&lis_in_syscall);	/* "done" with a system call */
	lis_runqueues();
	rslt = lis_down_fcn(&sd->sd_wioc, f, l);
	K_ATOMIC_INC(&lis_in_syscall);	/* processing a system call */
	return (rslt);

}				/* lis_sleep_on_wioc */

/*  -------------------------------------------------------------------  */
/*
 * Called to wait for the driver to backenable its queue and wake up
 * a waiting writer.  The stream head write queue is locked when this
 * routine is called.  We unlock it just prior to waiting on the semaphore.
 */
int
lis_sleep_on_wwrite(struct stdata *sd)
{
	int ret;

	AINC(sd->sd_wrcnt);
	lis_unlockq(sd->sd_wq);
	K_ATOMIC_DEC(&lis_in_syscall);	/* "done" with a system call */
	lis_runqueues();
	if ((ret = lis_down(&sd->sd_wwrite)) < 0)
		ADEC(sd->sd_wrcnt);
	else /* no error */ if ((ret = lis_lockq(sd->sd_wq)) < 0) {	/* relock queue */
		ADEC(sd->sd_wrcnt);	/* back out */
		lis_up(&sd->sd_wwrite);
	}

	K_ATOMIC_INC(&lis_in_syscall);	/* processing a system call */
	return (ret);

}				/* lis_sleep_on_wwrite */

/*  -------------------------------------------------------------------  */
void
lis_wake_up_wwrite(struct stdata *sd)
{
	int err;

	if ((err = lis_lockq(sd->sd_wq)) < 0) {
		lis_stream_error(sd, err, err);
		return;
	}

	if (K_ATOMIC_READ(&sd->sd_wrcnt) > 0) {
		ADEC(sd->sd_wrcnt);
		lis_unlockq(sd->sd_wq);
		lis_up(&sd->sd_wwrite);
	} else
		lis_unlockq(sd->sd_wq);

}				/* lis_wake_up_wwrite */

/*  -------------------------------------------------------------------  */
void
lis_wake_up_all_wwrite(struct stdata *sd)
{
	/* 
	 * No queue locking, just wake up everybody and let them contend
	 */
	while (K_ATOMIC_READ(&sd->sd_wrcnt) > 0) {
		ADEC(sd->sd_wrcnt);
		lis_up(&sd->sd_wwrite);
	}

}				/* lis_wake_up_all_wwrite */

/*  -------------------------------------------------------------------  */
int
lis_sleep_on_wread(struct stdata *sd)
{
	int ret;

	AINC(sd->sd_rdcnt);	/* inc while q locked */
	lis_unlockq(sd->sd_rq);	/* unlock to let rsrv run */
	K_ATOMIC_DEC(&lis_in_syscall);	/* "done" with a system call */
	lis_runqueues();

	if ((ret = lis_down(&sd->sd_wread)) < 0)
		ADEC(sd->sd_rdcnt);
	else /* no error */ if ((ret = lis_lockq(sd->sd_rq)) < 0) {	/* relock queue */
		ADEC(sd->sd_rdcnt);	/* back out */
		lis_up(&sd->sd_wread);
	}

	K_ATOMIC_INC(&lis_in_syscall);	/* processing a system call */
	return (ret);

}				/* lis_sleep_on_wread */

/*  -------------------------------------------------------------------  */
void
lis_wake_up_wread(struct stdata *sd)
{
	int err;

	if ((err = lis_lockq(sd->sd_rq)) < 0) {
		lis_stream_error(sd, err, err);
		return;
	}
	if (K_ATOMIC_READ(&sd->sd_rdcnt) > 0) {	/* someone is reading */
		ADEC(sd->sd_rdcnt);
		lis_unlockq(sd->sd_rq);
		lis_up(&sd->sd_wread);
	} else
		lis_unlockq(sd->sd_rq);

}				/* lis_wake_up_wread */

/*  -------------------------------------------------------------------  */
void
lis_wake_up_all_wread(struct stdata *sd)
{
	/* 
	 * No queue locking, just wake up everybody and let them contend
	 */
	while (K_ATOMIC_READ(&sd->sd_rdcnt) > 0) {
		ADEC(sd->sd_rdcnt);
		lis_up(&sd->sd_wread);
	}

}				/* lis_wake_up_all_wread */

/*  -------------------------------------------------------------------  */
int
lis_sleep_on_read_sem(struct stdata *sd)
{
	int ret;

	AINC(sd->sd_rdsemcnt);
	if ((ret = lis_down(&sd->sd_read_sem)) < 0)	/* now sleep */
		ADEC(sd->sd_rdcnt);

	return (ret);

}				/* lis_sleep_on_read_sem */

/*  -------------------------------------------------------------------  */
void
lis_wake_up_read_sem(struct stdata *sd)
{
	if (K_ATOMIC_READ(&sd->sd_rdsemcnt) > 0) {	/* someone is reading */
		ADEC(sd->sd_rdsemcnt);
		lis_up(&sd->sd_read_sem);
	}

}				/* lis_wake_up_read_sem */

/*  -------------------------------------------------------------------  */
void
lis_wake_up_all_read_sem(struct stdata *sd)
{
	while (K_ATOMIC_READ(&sd->sd_rdsemcnt) > 0) {
		ADEC(sd->sd_rdsemcnt);
		lis_up(&sd->sd_read_sem);
	}

}				/* lis_wake_up_all_read_sem */

/*  -------------------------------------------------------------------  */
int
lis_sleep_on_wiocing(struct stdata *sd)
{
	int ret;

	K_ATOMIC_DEC(&lis_in_syscall);	/* "done" with a system call */
	lis_runqueues();
	ret = lis_down(&sd->sd_wiocing);
	K_ATOMIC_INC(&lis_in_syscall);	/* processing a system call */
	return (ret);

}				/* lis_sleep_on_wiocing */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
