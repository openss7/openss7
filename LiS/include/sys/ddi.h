/*                               -*- Mode: C -*- 
 * ddi.h --- 
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: 
 * Last Modified On: 
 * Update Count    : 0
 * RCS Id          : $Id: ddi.h,v 1.1 1995/12/19 15:52:08 dave Exp $
 * Usage           : see below :)
 * Required        : see below :)
 * Status          : ($State: Exp $) Unknown, Use with caution!
 * Prefix(es)      : 
 * Requeriments    : 
 * Purpose         : 
 *                 : 
 *
 * Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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

#ifndef _DDI_H
#define _DDI_H 1

#ident "@(#) LiS ddi.h 2.4 08/24/04 10:34:30 "

#include <sys/LiS/genconf.h>

/* This file has to be updated with the Linux counterpart of SVR4 <sys/ddi.h>
 * header file. 
 */
#ifdef makedevice
#undef makedevice
#endif
#ifdef getmajor
#undef getmajor
#endif
#ifdef getminor
#undef getminor
#endif

#define makedevice		lis_makedevice
#define	getmajor		lis_getmajor
#define	getminor		lis_getminor

#endif /*!_DDI_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
