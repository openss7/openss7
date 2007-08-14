/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * share.h --- streams entry points from the file ops
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: Francisco J. Ballesteros
 * Last Modified On: Fri Sep 29 17:33:41 1995
 * Update Count    : 6
 * RCS Id          : $Id: share.h,v 1.1.1.2.4.3 2005/12/18 05:41:24 brian Exp $
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
 *    You can reach me by email to
 *    nemo@ordago.uc3m.es
 */

#ifndef _SHARE_H
#define _SHARE_H 1

#ident "@(#) $RCSfile: share.h,v $ $Name:  $($Revision: 1.1.1.2.4.3 $) $Date: 2005/12/18 05:41:24 $"

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
int _RP lis_getint(unsigned char **p);	/* msg.c */

void _RP lis_putbyte(unsigned char **p, unsigned char byte);	/* msg.c */

#if 0
void lis_bzero(void *ptr, int cnt);	/* head.c */
#else
#define lis_bzero(ptr, cnt) memset(ptr, 0, cnt);
#endif
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif				/* !_SHARE_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
