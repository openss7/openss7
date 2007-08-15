/*****************************************************************************

 @(#) $Id: strlog.h,v 1.1.1.1.12.7 2007/08/14 10:47:10 brian Exp $

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

 $Log: strlog.h,v $
 Revision 1.1.1.1.12.7  2007/08/14 10:47:10  brian
 - GPLv3 header update

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * strlog.h --- streams strlog cntl handling
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: 
 * Last Modified On: 
 * Update Count    : 0
 * RCS Id          : $Id: strlog.h,v 1.1.1.1.12.7 2007/08/14 10:47:10 brian Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) Unknown, Use with caution!
 * Prefix(es)      : lis
 * Requeriments    : 
 * Purpose         : here you have utilites to handle str strlogs.
 *                 : 
 * ----------------______________________________________________
 */

#ifndef _STRLOG_H
#define _STRLOG_H 1

#ident "@(#) $RCSfile: strlog.h,v $ $Name:  $($Revision: 1.1.1.1.12.7 $) $Date: 2007/08/14 10:47:10 $"

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/*  -------------------------------------------------------------------  */
/*				    Types                                */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/*  -------------------------------------------------------------------  */
/* strlog - send a message to STREAMS log driver
 *
 * STATUS: incomplete, untested
 */
#ifdef __KERNEL__
#ifdef __LIS_INTERNAL__
extern int lis_strlog(short mid, short sid, char level, unsigned short flags, char *fmt, ...)
    __attribute__ ((format(printf, 5, 6)));
#endif
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif				/* !_STRLOG_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
