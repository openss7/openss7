/*                               -*- Mode: C -*- 
 * errmsg.h --- streams error messages
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: Francisco J. Ballesteros
 * Last Modified On: Tue Sep 26 15:18:21 1995
 * Update Count    : 3
 * RCS Id          : $Id: errmsg.c,v 1.2 1995/12/27 01:02:20 dave Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) complete, untested, compiled
 * Prefix(es)      : lis
 * Requeriments    : 
 * Purpose         : provide streams errmsg
 *                 : 
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Francisco J. Ballesteros, Denis Froschauer
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
 *    You can reach us by email to any of
 *    nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 */

#ident "@(#) LiS errmsg.c 2.2 11/27/00 21:16:56 "

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/strport.h>
#include <stdarg.h>
#include <sys/strconfig.h>	/* config definitions */
#include <sys/LiS/share.h>	/* generid defs */
#include <sys/LiS/errmsg.h>	/* module interface */
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */
/*				    Types                                */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

/*  -------------------------------------------------------------------  */
/*			       Local functions                           */


/* This will just panic
 */
static void
lis_do_panic(char *msg)
{
    PANIC(msg);
}/*lis_do_panic*/

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */


/* This function will report a msg w/ the error condition and take appropriate
 * actions depending on the error level. 
 * actions will be:
 * LIS_PANIC: issue a panic msg and panic the kernel
 * LIS_ERROR: issue an error.
 * LIS_WARN:  issue a warning msg.
 * LIS_DEBUG: issue a debug msg.
 * msg will be formatted like in: 
 *      "some_msg_prefix (your_function_name): fmt_msg\n"
 */

void
lis_error( int lvl, const char *fname, const char * fmt, ...)
{
	char buf[1024];
	char *fmtmsg;
	extern int vsprintf (char *, const char *, va_list);
	va_list args;
	sprintf(buf,"LiS %s (%s):",lis_errmsg(lvl),(fname?fname:"unknown"));
	for(fmtmsg=buf; *fmtmsg; fmtmsg++)
	    ;
	va_start (args, fmt);
	vsprintf (fmtmsg, fmt, args);
	va_end (args);
	if (lvl==LIS_PANIC)
	    lis_do_panic(buf);
	else
	    PRINTK("%s\n",buf);
}/*lis_error*/


/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
