/*****************************************************************************

 @(#) $Id: $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: $ by $Author: $

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * events.h --- streams events
 * Author          :  Francisco J. Ballesteros & Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: Francisco J. Ballesteros
 * Last Modified On: Tue Sep 26 15:20:42 1995
 * Update Count    : 2
 * RCS Id          : $Id: events.h,v 1.3 1996/01/27 00:40:25 dave Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) complete, untested, compiled
 * Prefix(es)      : lis
 * Requeriments    : 
 * Purpose         : provide streams events
 *                 : 
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */

#ifndef _EVENTS_H
#define _EVENTS_H 1

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

#ifndef _SYS_TYPES_H
#if	defined(LINUX)
#include <linux/types.h>
#else
#include <sys/types.h>
#endif
#endif

#ifdef __KERNEL__

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/strconfig.h>	/* config definitions */
#include <sys/LiS/share.h>	/* generid defs */
/*  -------------------------------------------------------------------  */
/*				    Types                                */
/* Stream event info
 */
typedef
struct strevent {
    struct strevent *se_next;	/* next event for this stream or NULL*/
    struct strevent *se_prev;	/* previous event for this stream or last
				 * event if this is the first one*/
    pid_t se_pid;		/* process to be signaled */
    short se_evs;		/* events wanted */
} strevent_t;

#if 0
/* I don't know if it's a good idea to keep the strinfo structs around
 * other structs. It's good to debug memory issues, but we should reimplement
 * completelly the STREAMS memory management. 
 * I guess it would be better just to alloc raw memory pages and avoid 
 * memory fragmentation by using the knowledge of what's likely to be requested
 * what's likely to be freed and what the sizes are.
 * When one of those pages get w/ count 0 (i.e., no used chunk inside) we could
 * just give it back to the kernel.
 * More on, we could keep initialized structs in a pre-initialized state, so
 * only the very first time they're use they're filled. Later on, the initial
 * initialization process could be skipped.
 * There was an article in UNSENIX (don't remember exactly where) about this.
 * -- nemo
 */

struct strinfo {};
#endif /* 0 */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

#if __LIS_INTERNAL__
extern struct strevent *lis_sefreelist; /* list of free stream events */
extern struct strevent *lis_secachep;   /* reserve store of free str events */
#endif

#if 0
/* see long comment above -- nemo */
extern struct strinfo lis_strinfo[]; /* keeps track of allocated events	*/
#endif

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

#if __LIS_INTERNAL__
/* get events for pid in list
 * STATUS: complete, untested
 */
extern  short
lis_get_elist_ent( strevent_t *list, pid_t pid );

/* add event to list
 * STATUS: complete, untested
 */
extern int
lis_add_to_elist( strevent_t **list, pid_t pid, short events );

/* del event from list
 * rets non-zero if not-found
 * STATUS: complete, untested
 */
extern int
lis_del_from_elist( strevent_t **list, pid_t pid, short events );

/*
 * Free the entire elist
 */
extern void
lis_free_elist( strevent_t **list);
#endif

/*  -------------------------------------------------------------------  */
#endif /* __KERNEL__ */
#endif /*!_EVENT_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
