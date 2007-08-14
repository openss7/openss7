/*****************************************************************************

 @(#) $Id: ttyio.h,v 0.9.2.2 2007/08/14 03:04:33 brian Exp $

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

 Last Modified $Date: 2007/08/14 03:04:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ttyio.h,v $
 Revision 0.9.2.2  2007/08/14 03:04:33  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/10/12 09:30:56  brian
 - added header files for package

 *****************************************************************************/

#ifndef __SYS_TTYIO_H__
#define __SYS_TTYIO_H__

#ident "@(#) $RCSfile: ttyio.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* terminal special characters */
struct tchars {
	char t_intrc;			/* interrupt */
	char t_quitc;			/* quit */
	char t_startc;			/* start output */
	char t_stopc;			/* stop output */
	char t_eofc;			/* end-of-file (EOF) */
	char t_brkc;			/* input delimiter (like nl) */
};

/* local terminal special chars */
struct ltchars {
	char t_suspc;			/* stop process signal */
	char t_dsuspc;			/* delayed stop process signal */
	char t_rprntc;			/* reprint line */
	char t_flushc;			/* flush output (toggles) */
	char t_werasc;			/* word erase */
	char t_lnextc;
};

/* 4.3BDS flags */

#define CRTBS		0x00010000	/* do backspacing for crt */
#define PRTERA		0x00020000	/* \ ... / erase */
#define CRTERA		0x00040000	/* " \b " to wipe out char */
#define TILDE		0x00080000	/* hazetine tilde kluge */
#define MDMBUF		0x00100000	/* start/stop output on carrier intr */
#define LITOUT		0x00200000	/* literal output */
#define BSD_TOSTOP	0x00400000	/* SIGSTOP on background output */
#define BSD_FLUSHO	0x00800000	/* flush output to terminal */
#define NOHANG		0x01000000	/* no SIGHUP on carrier drop */
#define CRTKIL		0x04000000	/* kill line with " \b " */
#define PASS8		0x08000000
#define CTLECH		0x10000000	/* echo control chars as ^X */
#define BSD_PENDIN	0x20000000	/* tp->t_rawq needs reread */
#define DECCTQ		0x40000000	/* only ^Q starts after ^S */
#define BSD_NOFLSH	0x80000000	/* no output flush on signal */

#define LCRTBS		0x0001	/* do backspacing for crt */
#define LPRTERA		0x0002	/* \ ... / erase */
#define LCRTERA		0x0004	/* " \b " to wipe out char */
#define LTILDE		0x0008	/* hazetine tilde kluge */
#define LMDMBUF		0x0010	/* start/stop output on carrier intr */
#define LLITOUT		0x0020	/* literal output */
#define LTOSTOP		0x0040	/* SIGSTOP on background output */
#define LFLUSHO		0x0080	/* flush output to terminal */
#define LNOHANG		0x0100	/* no SIGHUP on carrier drop */
#define LCRTKIL		0x0400	/* kill line with " \b " */
#define LPASS8		0x0800
#define LCTLECH		0x1000	/* echo control chars as ^X */
#define LPENDIN		0x2000	/* tp->t_rawq needs reread */
#define LDECCTQ		0x4000	/* only ^Q starts after ^S */
#define LNOFLSH		0x8000	/* no output flush on signal */

#define tIOC		('t'<<8)

/* 4.3BSD/XENIX ioctls */
#ifndef _LINUX_TERMIOS_H
#define TIOCGETD	(tIOC|0)
#define TIOCSETD	(tIOC|1)
#else				/* _LINUX_TERMIOS_H */
#define O_TIOCGETD	(tIOC|0)
#define O_TIOCSETD	(tIOC|1)
#endif				/* _LINUX_TERMIOS_H */

#define TIOCHPCL	(tIOC|2)

#ifndef _LINUX_TERMIOS_H
#define TIOCGETP	(tIOC|8)
#define TIOCSETP	(tIOC|9)
#define TIOCSETN	(tIOC|10)
#define TIOCEXCL	(tIOC|13)
#define TIOCNXCL	(tIOC|14)
#else				/* _LINUX_TERMIOS_H */
#define O_TIOCGETP	(tIOC|8)
#define O_TIOCSETP	(tIOC|9)
#define O_TIOCSETN	(tIOC|10)
#define O_TIOCEXCL	(tIOC|13)
#define O_TIOCNXCL	(tIOC|14)
#endif				/* _LINUX_TERMIOS_H */

#define TIOCFLUSH	(tIOC|16)	/* Flush buffers */
#define TIOCSETC	(tIOC|17)	/* write struct tchars - set special characters */
#define TIOCGETC	(tIOC|18)	/* read struct tchars - get special characters */

/* 4.3BSD  */
#define TIOCLBIS	(tIOC|127)	/* bis local mode bits */
#define TIOCLBIC	(tIOC|126)	/* bic local mode bits */
#define TIOCLSET	(tIOC|125)	/* set entire local mode word */
#define TIOCLGET	(tIOC|124)	/* get local modes */

#ifndef _LINUX_TERMIOS_H
#define TIOCSBRK	(tIOC|123)	/* set break bit */
#define TIOCCBRK	(tIOC|122)	/* clear break bit */
#else				/* _LINUX_TERMIOS_H */
#define O_TIOCSBRK	(tIOC|123)	/* set break bit */
#define O_TIOCCBRK	(tIOC|122)	/* clear break bit */
#endif				/* _LINUX_TERMIOS_H */

#define TIOCSDTR	(tIOC|121)	/* set data terminal ready mode */
#define TIOCCDTR	(tIOC|120)	/* clear data terminal ready */
#define TIOCSLTC	(tIOC|117)	/* write struct ltchars - set local special chars */
#define TIOCGLTC	(tIOC|116)	/* read struct ltchars - get local special chars */

#ifndef _LINUX_TERMIOS_H
#define TIOCOUTQ	(tIOC|115)	/* driver output queue size */
#define TIOCNOTTY	(tIOC|113)	/* void tty association */
#else				/* _LINUX_TERMIOS_H */
#define O_TIOCOUTQ	(tIOC|115)	/* driver output queue size */
#define O_TIOCNOTTY	(tIOC|113)	/* void tty association */
#endif				/* _LINUX_TERMIOS_H */

#define TIOCSTOP	(tIOC|111)	/* stop output, like ^s */
#define TIOCSTART	(tIOC|110)	/* start output, like ^q */

/* Version 7 */
#define	DIOC		('d'<<8)

#define DIOCGETP	(DIOC|8)	/* V7 version of TIOCGETP */
#define DIOCSETP	(DIOC|9)	/* V7 version of TIOCSETP */

#endif				/* __SYS_TTYIO_H__ */
