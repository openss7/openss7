/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __NTP_NTP_CONFIG_H__
#define __NTP_NTP_CONFIG_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* config file for ntp compilation */

#define SUN_FLT_BUG		/* sun 3 problems */
#define SUN_3_3			/* Sun 3.3 strangeness */
#define SETTICKADJ		/* attempt to modify kernel's `tickadj' variable at run time */
#define DEBUG			/* enable optional debugging trace */
#define REFCLOCK		/* define if you have a reference clock attached to your machine.
				   (untested by UMD) */

#undef	 BROADCAST_NTP		/* experimental support for broadcast NTP */
#undef XADJTIME2		/* experimental support for second-order clock adjustment */
#undef PSTI			/* define along with REFCLOCK if you have a PSTI clock attached
				   that you'd like to use a a reference clock */
#undef XTAL	1		/* 0 for line freq clock, or 1 for crystal controlled clock
				   (default) */
#undef	 NOSWAP			/* allow use of plock() to prevent swapping */

/* if you dont have random/srandom - use the below workaround */
#if 0
#define srandom(x) srand(x)
#define random()	rand()
#endif

#ifndef	WAYTOOBIG
#define	WAYTOOBIG	1000.0	/* Too many seconds to correct, something is really wrong */
#endif

#ifndef	XTAL
#define	XTAL	1		/* crystal controlled clock by default */
#endif

#ifndef	NTPINITFILE
#define	NTPINITFILE	"/usr/adm/ntp.conf"
#endif

#ifndef	NTPDRIFTCOMP
#define	NTPDRIFTCOMP	"/usr/adm/ntp.drift"
#endif

#endif				/* __NTP_NTP_CONFIG_H__ */
