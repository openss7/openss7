/*                               -*- Mode: C -*- 
 * errmsg.h --- streams error messages
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: Francisco J. Ballesteros
 * Last Modified On: Tue Sep 26 15:19:24 1995
 * Update Count    : 2
 * RCS Id          : $Id: errmsg.h,v 1.1 1995/12/19 15:58:14 dave Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) complete, untested, compiled
 * Prefix(es)      : lis
 * Requeriments    : 
 * Purpose         : provide streams errmsg
 *                 : 
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Francisco J. Ballesteros
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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

#ifndef _ERRMSG_H
#define _ERRMSG_H 1

#ident "@(#) LiS errmsg.h 2.1 4/4/99 11:34:28 "

#ifdef __KERNEL__

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

enum lis_errclass_t { LIS_PANIC, LIS_ERROR, LIS_WARN, LIS_DEBUG };

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */


/* This function will report a msg w/ the error condition and take appropriate
 * actions depending on the error level. 
 * actions will be:
 * LIS_PANIC: issue a panic msg and panic the kernel
 * LIS_ERROR: issue an error.
 * LIS_WARN:  issue a warning msg.
 * LIS_DEBUG: issue a debug msg.
 * msg will be formatted like: 
 *      "some_msg_prefix (your_function_name): fmt_msg\n"
 */

extern void
lis_error( int lvl, const char *fname, const char * fmt, ...);

/*  -------------------------------------------------------------------  */
#endif /* __KERNEL__ */
#endif /*!_ERRMSG_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
