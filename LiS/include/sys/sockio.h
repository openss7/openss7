/* 
 * SVR4 style ifstats.  Needed for source code compatibility with
 * SVR4 drivers that want this structure.
 *
 * Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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
#ident "@(#) LiS sockio.h 2.1 4/4/99 11:34:28 "

#ifndef IFSTATS_DEFINED

#define IFSTATS_DEFINED                 /* for other include files */

struct ifstats 
{
    long	 ifs_ipackets ;
    long	 ifs_opackets ;
    long	 ifs_oerrors ;
    long	 ifs_ierrors ;
    void	*ifs_addrs ;
    void	*ifs_next ;
    long	 ifs_mtu ;
    char	*ifs_name;
    short	 ifs_unit;
    short	 ifs_active;
    int		 ifs_collisions;
} ;

#endif
