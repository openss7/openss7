/*
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

#ifndef _DLPIDRIVER
#define _DLPIDRIVER

#ident "@(#) LiS dlpidriver.h 2.1 4/4/99 11:34:28 "

#define MAXDEVICE	1

#define DLDRIOC	('D'<<8)
#define SETADDR	(DLDRIOC|1)
#define GETADDR	(DLDRIOC|2)

#ifdef _KERNEL

#undef STATIC
#ifdef DEBUG
#define STATIC
#else
#define STATIC static
#endif

#endif /* _KERNEL */

#endif /* _DLPIDRIVER */



