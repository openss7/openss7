/*****************************************************************************

 @(#) $RCSfile: wait.h,v $ $Name:  $($Revision: 1.1.1.2.4.6 $) $Date: 2008-04-29 08:33:16 $

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

 Last Modified $Date: 2008-04-29 08:33:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: wait.h,v $
 Revision 1.1.1.2.4.6  2008-04-29 08:33:16  brian
 - update headers for Affero release

 Revision 1.1.1.2.4.5  2007/08/14 10:47:11  brian
 - GPLv3 header update

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * wait.h --- waiting for events...
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: wait.h,v 1.1.1.2.4.6 2008-04-29 08:33:16 brian Exp $
 * Purpose         : provide user waitentials
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Francisco J. Ballesteros
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach me by email to
 *    nemo@ordago.uc3m.es
 */

#ifndef _WAIT_H
#define _WAIT_H 1

#ident "@(#) $RCSfile: wait.h,v $ $Name:  $($Revision: 1.1.1.2.4.6 $) $Date: 2008-04-29 08:33:16 $"

#ifdef __KERNEL__
/*  -------------------------------------------------------------------  */
/*                               Dependencies                            */

#ifndef _HEAD_H
#include <sys/LiS/head.h>	/* stream head */
#endif

/*  -------------------------------------------------------------------  */
/*                         Shared global variables                       */

#define ONESEC (1000000/HZ)	/* ? */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/* The functions below are to achieve mutual exclusion on the stream
 * head. They use the sd_wopen wait queue to sleep and the sd_lock char to
 * flag the lock.
 * The lock/unlock are the complete ones.
 * if you just want to wait you may use wait_on/wake_up.
 *
 * You can lock(L)/wait(W)/sleep(S) on the following events:
 * L/S wopen:  the 1st opener completes its job
 * W/S wwrite: there's room in the write queue to issue a downstream msg.
 * W/S wread: msg arrives at the stream head.
 * L/S wioc:  process completes it's ioctl()
 *   S wiocing: answer to ioc arrived
 * STATUS: complete(?), untested
 */
#ifdef __LIS_INTERNAL__
extern int lis_sleep_on_wopen(struct stdata *sd);
extern int lis_sleep_on_wioc(struct stdata *sd, char *f, int l);
extern int lis_sleep_on_wwrite(struct stdata *sd);
extern int lis_sleep_on_wread(struct stdata *sd);
extern int lis_sleep_on_wiocing(struct stdata *sd);
extern int lis_sleep_on_read_sem(struct stdata *sd);
#endif

/*  -------------------------------------------------------------------  */

#define lis_wait_on_wwrite(sd)	lis_sleep_on_wwrite(sd)

#define lis_wait_on_wread(sd)	lis_sleep_on_wread(sd)

#define lis_lock_wioc(sd)	lis_sleep_on_wioc(sd,__FILE__,__LINE__)

/*  -------------------------------------------------------------------  */

#ifdef __LIS_INTERNAL__
extern void lis_wake_up_wwrite(struct stdata *sd);	/* wait.c */
extern void lis_wake_up_all_wwrite(struct stdata *sd);	/* wait.c */
extern void lis_wake_up_wread(struct stdata *sd);	/* wait.c */
extern void lis_wake_up_all_wread(struct stdata *sd);	/* wait.c */
extern void lis_wake_up_read_sem(struct stdata *sd);	/* wait.c */
extern void lis_wake_up_all_read_sem(struct stdata *sd);	/* wait.c */
#endif

#define lis_wake_up_wiocing(sd)						\
				{					\
				    lis_up(&sd->sd_wiocing);		\
				}

/*  -------------------------------------------------------------------  */
#define lis_unlock_wioc(sd)						\
				{					\
				    lis_up(&sd->sd_wioc);		\
				}

/*  -------------------------------------------------------------------  */

/* Sched & cancel timeouts
 */
typedef void tmout_fcn_t(ulong);

#ifdef __LIS_INTERNAL__
extern void lis_tmout(struct timer_list *tl, tmout_fcn_t * fn, long arg, long ticks);

extern void lis_untmout(struct timer_list *tl);
#endif

#endif				/* __KERNEL__ */

#endif				/* !_WAIT_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
