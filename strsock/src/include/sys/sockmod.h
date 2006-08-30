/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 *****************************************************************************/

#ifndef __SYS_SOCKMOD_H__
#define __SYS_SOCKMOD_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2006 OpenSS7 Corporation."

#define SOCKMOD_IOC_MAGIC	'I'

#define O_SI_GETUDATA	((SOCKMOD_IOC_MAGIC<<8)|101)
#define SI_SHUTDOWN	((SOCKMOD_IOC_MAGIC<<8)|102)
#define SI_LISTEN	((SOCKMOD_IOC_MAGIC<<8)|103)
#define SI_SETMYNAME	((SOCKMOD_IOC_MAGIC<<8)|104)
#define SI_SETPEERNAME	((SOCKMOD_IOC_MAGIC<<8)|105)
#define SI_GETINTRANSIT	((SOCKMOD_IOC_MAGIC<<8)|106)
#define SI_TCL_LINK	((SOCKMOD_IOC_MAGIC<<8)|107)
#define SI_TCL_UNLINK	((SOCKMOD_IOC_MAGIC<<8)|108)
#define SI_SOCKPARAMS	((SOCKMOD_IOC_MAGIC<<8)|109)
#define SI_GETUDATA	((SOCKMOD_IOC_MAGIC<<8)|110)

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

#if 0
#define O_SI_GETUDATA		_IORW(SOCKMOD_IOC_MAGIC, 101, struct o_si_udata)
#define SI_SHUTDOWN		_IORW(SOCKMOD_IOC_MAGIC, 102, void)
#define SI_LISTEN		_IORW(SOCKMOD_IOC_MAGIC, 103, void)
#define SI_SETMYNAME		_IORW(SOCKMOD_IOC_MAGIC, 104, void)
#define SI_SETPEERNAME		_IORW(SOCKMOD_IOC_MAGIC, 105, void)
#define SI_GETINTRANSIT		_IORW(SOCKMOD_IOC_MAGIC, 106, void)
#define SI_TCL_LINK		_IORW(SOCKMOD_IOC_MAGIC, 107, void)
#define SI_TCL_UNLINK		_IORW(SOCKMOD_IOC_MAGIC, 108, void)
#define SI_SOCKPARAMS		_IORW(SOCKMOD_IOC_MAGIC, 109, struct si_sockparams)
#define SI_GETUDATA		_IORW(SOCKMOD_IOC_MAGIC, 110, struct si_udata)
#endif

#endif				/* __SYS_SOCKMOD_H__ */
