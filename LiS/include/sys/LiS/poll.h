/*                               -*- Mode: C -*- 
 * poll.h --- poll() syscall
 * Author          : Fco. J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: poll.h,v 1.3 1996/01/27 00:40:27 dave Exp $
 * Purpose         : STREAMS poll() mechanism
 * ----------------______________________________________________
 *
 *  Copyright (C) 1995  Francisco J. Ballesteros, Graham Wheeler
 *  Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 *
 *    You can reach us by email to
 *    nemo@ordago.uc3m.es, gram@aztec.co.za
 */


#ifndef _POLL_H
#define _POLL_H 1

#ident "@(#) LiS poll.h 2.1 4/4/99 11:34:29 "

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/*  -------------------------------------------------------------------  */
/*				    Types                                */

/* Poll list head structure.  
 */
struct pollhead {
	struct polldat	*ph_list;	/* list of pollers */
};

/* Data necessary to notify process sleeping in poll(2)
 * when an event has occurred.
 */
struct polldat {
	struct polldat	*pd_next;  /* next in poll list */
	struct polldat	*pd_prev;  /* previous in poll list */
	struct pollhead *pd_headp; /* backptr to pollhead */
	short		pd_events; /* events being polled */
	void    (*pd_fn)(long);    /* event callback fn */
	long            pd_arg;	   /* arg to fn */
};
typedef struct pollhead pollhead_t;
typedef struct polldat  polldat_t;

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */


#ifdef __KERNEL__
/* poll() syscall entry point
 */
int lis_syspoll(char *ubuff, unsigned long n, int tmout);
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif /*!_POLL_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
