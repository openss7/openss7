/*****************************************************************************

 @(#) $Id: strtty.h,v 1.1.2.2 2010-11-28 14:21:49 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2010-11-28 14:21:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strtty.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:49  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:38  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_STRTTY_H__
#define __SYS_STRTTY_H__

#define LARGEBUFSZ	512

#define TTIPRI	28
#define TTOPRI	29

/* internal state */
#define TIMEOUT	0000001		/* delay timeout in progress */
#define WOPEN	0000002		/* waiting for open to complete */
#define ISOPEN	0000004		/* device is open */
#define TBLOCK	0000010
#define CARR_ON	0000020		/* software copy of carrier-present */
#define BUSY	0000040		/* output in progress */
#define WIOC	0000100		/* wait for ioctl to complete */
#define WGETTY	0000200		/* opened by supergetty , waiting for getty */
#define TTSTOP	0000400		/* output stopped by ^s */
#undef EXTPROC                  /* conflict from include/asm-generic/termbits.h */
#define EXTPROC	0001000		/* external processing */
#define TAC	0002000
#define CLESC	0004000		/* last char escape */
#define RTO	0010000		/* raw timeout */
#define TTIOW	0020000
#define TTXON	0040000
#define TTXOFF	0100000

/* l_output status */
#define CPRS		0100000

/* device commands */
#define T_OUTPUT	0
#define T_TIME		1
#define T_SUSPEND	2
#define T_RESULE	3
#define T_BLOCK		4
#define T_UNBLOCK	5
#define T_RFLUSH	6
#define T_WFLUSH	7
#define T_BREAK		8
#define T_INPUT		9
#define T_DISCONNECT	10
#define T_PARM		11
#define T_SWTCH		12

/* M_CTL message commands */
#define MC_NO_CANON	0	/* tell ldterm not to do line editing */
#define MC_DO_CANON	1	/* tell ldterm to do line editing */
#define MC_CANONQUERY	2	/* ask tty about canonical processing (not pty) */
#define MC_PART_CANON	3	/* tell ldterm to do some processing (not pty) */

#endif				/* __SYS_STRTTY_H__ */
