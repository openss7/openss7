/*                               -*- Mode: C -*- 
 * fifo.h --- fifo driver 
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: 
 * Last Modified On: 
 * Update Count    : 0
 * RCS Id          : $Id: fifo.h,v 1.2 1996/01/27 00:40:25 dave Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) Unknown, Use with caution!
 * Prefix(es)      : 
 * Requeriments    : 
 * Purpose         : implement STREAMS fifos
 *                 : 
 * ----------------______________________________________________
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
 */

#ifndef _FIFO_H
#define _FIFO_H 1

#ident "@(#) LiS fifo.h 2.1 4/4/99 11:34:29 "

/*  -------------------------------------------------------------------  */
/*                               Dependencies                            */

#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* streams config symbols are here */
#endif
#ifndef _MOD_H
#include <sys/LiS/mod.h>	/* streams module symbols & types */
#endif

/*  -------------------------------------------------------------------  */
/*                         Shared global variables                       */

extern struct streamtab lis_fifo_strtab; /* streamtab for streams fifo's */

#endif /*!_FIFO_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
