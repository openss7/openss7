/*****************************************************************************

 @(#) $Id: poll.h,v 1.1.1.1.12.5 2007/08/14 10:47:10 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

 Last Modified $Date: 2007/08/14 10:47:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: poll.h,v $
 Revision 1.1.1.1.12.5  2007/08/14 10:47:10  brian
 - GPLv3 header update

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * poll.h --- poll() syscall
 * Author          : Fco. J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: poll.h,v 1.1.1.1.12.5 2007/08/14 10:47:10 brian Exp $
 * Purpose         : STREAMS poll() mechanism
 * ----------------______________________________________________
 *
 *  Copyright (C) 1995  Francisco J. Ballesteros, Graham Wheeler
 *  Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to
 *    nemo@ordago.uc3m.es, gram@aztec.co.za
 */

#ifndef _POLL_H
#define _POLL_H 1

#ident "@(#) $RCSfile: poll.h,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2007/08/14 10:47:10 $"

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/*  -------------------------------------------------------------------  */
/*				    Types                                */

/* Poll list head structure.  
 */
struct pollhead {
	struct polldat *ph_list;	/* list of pollers */
};

/* Data necessary to notify process sleeping in poll(2)
 * when an event has occurred.
 */
struct polldat {
	struct polldat *pd_next;	/* next in poll list */
	struct polldat *pd_prev;	/* previous in poll list */
	struct pollhead *pd_headp;	/* backptr to pollhead */
	short pd_events;		/* events being polled */
	void (*pd_fn) (long);		/* event callback fn */
	long pd_arg;			/* arg to fn */
};
typedef struct pollhead pollhead_t;
typedef struct polldat polldat_t;

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

#ifdef __KERNEL__
/* poll() syscall entry point
 */
#ifdef __LIS_INTERNAL__
int lis_syspoll(char *ubuff, unsigned long n, int tmout);
#endif
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif				/* !_POLL_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
