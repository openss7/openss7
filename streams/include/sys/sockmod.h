/*****************************************************************************

 @(#) $Id: sockmod.h,v 0.9.2.1 2004/02/29 19:10:08 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/02/29 19:10:08 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LINUX_SOCKMOD_H__
#define __LINUX_SOCKMOD_H__

#define O_SI_GETUDATA	(('I'<<8)|101)
#define SI_SHUTDOWN	(('I'<<8)|102)
#define SI_LISTEN	(('I'<<8)|103)
#define SI_SETMYNAME	(('I'<<8)|104)
#define SI_SETPEERNAME	(('I'<<8)|105)
#define SI_GETINTRANSIT	(('I'<<8)|106)
#define SI_TCL_LINK	(('I'<<8)|107)
#define SI_TCL_UNLINK	(('I'<<8)|108)
#define SI_SOCKPARAMS	(('I'<<8)|109)
#define SI_GETUDATA	(('I'<<8)|110)

struct si_sockparams {
	int sp_family;
	int sp_type;
	int sp_protocol;
};

struct o_si_udata {
	int tidusize;
	int addrsize;
	int optsize;
	int etsdusize;
	int servtype;
	int so_state;
	int so_options;
	int tsdusize;
};

struct si_udata {
	int tidusize;
	int addrsize;
	int optsize;
	int etsdusize;
	int servtype;
	int so_state;
	int so_options;
	int tsdusize;
	struct si_sockparams sockparams;
};

#endif				/* __LINUX_SOCKMOD_H__ */
