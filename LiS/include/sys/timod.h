/*
 *  sys/timod.h
 *
 *  Copyright (C) 1998 Ole Husgaard <sparre@login.dknet.dk>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 *  MA 02139, USA.
 *
 */

#ident "@(#) LiS timod.h 2.1 4/4/99 11:34:28 "

#ifndef _SYS_TIMOD_H
#define _SYS_TIMOD_H

#define TIMOD		('T' << 8)
#define TI_GETINFO	(TIMOD + 140)	/* Get information		*/
#define TI_OPTMGMT	(TIMOD + 141)	/* Options management		*/
#define TI_BIND		(TIMOD + 142)	/* Bind				*/
#define TI_UNBIND	(TIMOD + 143)	/* Unbind			*/

#endif
