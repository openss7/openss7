/*                               -*- Mode: C -*- 
 * share.h --- streams entry points from the file ops
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: Francisco J. Ballesteros
 * Last Modified On: Fri Sep 29 17:33:41 1995
 * Update Count    : 6
 * RCS Id          : $Id: share.h,v 1.6 1996/01/27 00:40:29 dave Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) Unknown, Use with caution!
 * Prefix(es)      : lis
 * Requeriments    : 
 * Purpose         : provide glue for lis vs os
 *                 : 
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Francisco J. Ballesteros
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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
 *    You can reach me by email to
 *    nemo@ordago.uc3m.es
 */

#ifndef _SHARE_H
#define _SHARE_H 1

#ident "@(#) LiS share.h 2.2 12/20/01 21:03:25 "

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */
/* We put here every linux kernel specific include to 
 * help developing internal code
 */



/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/*  -------------------------------------------------------------------  */
/* This defines values returned by some utility functions
 */
enum { STR_ERR = 0, STR_OK = 1 };

#define lis_min(a,b)	(((a)<(b))?(a):(b))
#define lis_max(a,b)	(((a)>(b))?(a):(b))
#ifndef min
#define	min		lis_min
#define	max		lis_max
#endif
/*  -------------------------------------------------------------------  */

/* Bit flags
 */
#define F_ISSET(f,v)	((f) & (v))
#define F_SET(f,v)	((f) |= (v))
#define F_CLR(f,v)	((f) &= ~(v))

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef VOID
#define VOID void
#endif

#ifdef __KERNEL__
/* extract values from a char pointer and advance the pointer */
int lis_getint(unsigned char **p) ;				/* msg.c */

void lis_putbyte(unsigned char **p, unsigned char byte) ;	/* msg.c */

#if 0
void	lis_bzero(void *ptr, int cnt) ;				/* head.c */
#else
#define lis_bzero(ptr, cnt) memset(ptr, 0, cnt);
#endif
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif /*!_SHARE_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
