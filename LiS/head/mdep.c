/*
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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

#ident "@(#) LiS mdep.c 2.1 4/4/99 11:34:24 "

#if defined(__MSDOS__)				/* DOS version */
#include "dos-mdep.c"
#elif defined(LINUX)				/* Linux version */
#include "linux-mdep.c"
#elif defined(USER)				/* user-level version */
#include "user-mdep.c"
#elif defined(SYS_SCO)				/* SCO user-level version */
#include "user-mdep.c"
#elif defined(SYS_54)				/* SCO user-level version */
#include "user-mdep.c"
#elif defined(QNX)				/* QNX driver version */
#include "qnx-mdep.c"
#elif defined(PORTABLE)				/* just the portable part */
#include "port-mdep.c"
#endif










