/*****************************************************************************

 @(#) $Id: ttychars.h,v 0.9.2.3 2008-04-28 17:46:03 brian Exp $

 -----------------------------------------------------------------------------

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

 Last Modified $Date: 2008-04-28 17:46:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ttychars.h,v $
 Revision 0.9.2.3  2008-04-28 17:46:03  brian
 - updates for release

 Revision 0.9.2.2  2007/08/14 03:04:33  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/10/12 09:30:56  brian
 - added header files for package

 *****************************************************************************/

#ifndef __SYS_TTYCHARS_H__
#define __SYS_TTYCHARS_H__

#ident "@(#) $RCSfile: ttychars.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

struct ttychars {
	char tc_erase;			/* erase last character */
	char tc_kill;			/* erase entire line */
	char tc_intrc;			/* interrupt */
	char tc_quitc;			/* quit */
	char tc_startc;			/* start output */
	char tc_stopc;			/* stop output */
	char tc_eofc;			/* end of file */
	char tc_brkc;			/* input delimiter (like nl) */
	char tc_suspc;			/* stop process signal */
	char tc_dsuspc;			/* delayed stop process signal */
	char tc_rprntc;			/* reprint line */
	char tc_flushc;			/* flush output (toggles) */
	char tc_werasec;		/* word erase */
	char tc_lnextc;			/* literal next character */
};

#define CTRL(c)	((c)&037)

#define	BSD_CERASE  0177
#define BSD_CKILL   CTRL('u')
#define BSD_CINTR   CTRL('c')
#define BSD_CQUIT   034
#define BSD_CSTART  CTRL('q')
#define BSD_CSTOP   CTRL('s')
#define BSD_CEOF    CTRL('d')
#define BSD_CEOT    BSD_CEOF
#define BSD_CBRK    0377
#define BSD_CSUSP   CTRL('z')
#define BSD_CDSUSP  CTRL('y')
#define BSD_CRPRNT  CTRL('r')
#define BSD_CFLUSH  CTRL('o')
#define BSD_CWERASE CTRL('w')
#define BSD_CLNEXT  CTRL('v')

#endif				/* __SYS_TTYCHARS_H__ */
