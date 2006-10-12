/*****************************************************************************

 @(#) $Id: termios.h,v 0.9.2.1 2006/10/12 09:30:56 brian Exp $

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

 Last Modified $Date: 2006/10/12 09:30:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: termios.h,v $
 Revision 0.9.2.1  2006/10/12 09:30:56  brian
 - added header files for package

 *****************************************************************************/

#ifndef __SYS_TERMIOS_H__
#define __SYS_TERMIOS_H__

#ident "@(#) $RCSfile: termios.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#define _POSIX_VDISABLE	    0

#define CTRL(c)	    ((c)&037)
#define IBSHIFT	    16
#define NCC	    8
#define NCCS	    19

#ifndef _LINUX_TERMIOS_H
typedef unsigned long tcflag_t;
typedef unsigned char cc_t;
typedef unsigned long speed_t;

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
};
#else				/* _LINUX_TERMIOS_H */
typedef unsigned long o_tcflag_t;
typedef unsigned char o_cc_t;
typedef unsigned long o_speed_t;

struct o_termios {
	o_tcflag_t c_iflag;
	o_tcflag_t c_oflag;
	o_tcflag_t c_cflag;
	o_tcflag_t c_lflag;
	o_cc_t c_cc[NCCS];
};
#endif				/* _LINUX_TERMIOS_H */

#define VINTR		0
#define VQUIT		1
#define VERASE		2
#define VKILL		3
#define VEOF		4

#ifndef _LINUX_TERMIOS_H
#define VEOL		5
#define VEOL2		6
#define VMIN		4
#else				/* _LINUX_TERMIOS_H */
#define O_VEOL		5
#define O_VEOL2		6
#define O_VMIN		4
#endif				/* _LINUX_TERMIOS_H */

#define VTIME		5
#define VSWTCH		7
#define VSTART		8
#define VSTOP		9
#define VSUSP		10
#define VDSUSP		11
#define VREPRINT	12
#define VDISCARD	13
#define VWERASE		14
#define VLNEXT		15

#define CNULL		0
#define CDEL		0177
#define CESC		'\\'
#define CINTR		0177
#define CQUIT		034
#define CERASE		'#'
#define CKILL		'@'
#define CEOT		CTRL('d')
#define CEOL		0
#define CEOL2		0
#define CEOF		CTRL('d')
#define CSTART		CTLR('q')
#define CSTOP		CTRL('s')
#define CSWTCH		CTRL('z')
#define CNSWTCH		0
#define CSUSP		CTRL('z')
#define CDSUSP		CTRL('y')
#define CRPRNT		CTRL('r')
#define CFLUSH		CTRL('o')
#define CWERASE		CTRL('w')
#define CLNEXT		CTRL('v')

#define CMIN		06
#define CTIME		01

#define IGNBRK		0000001
#define BRKINT		0000002
#define IGNPAR		0000004
#define PARMRK		0000010
#define INPCK		0000020
#define ISTRIP		0000040
#define INLCR		0000100
#define IGNCR		0000200
#define ICRNL		0000400
#define IUCLC		0001000
#define IXON		0002000
#define IXANY		0004000
#define IXOFF		0010000
#define IMAXBEL		0020000
#define DOSMODE		0100000

#define OPOST		0000001
#define OLCUC		0000002
#define ONLCR		0000004
#define OCRNL		0000010
#define ONOCR		0000020
#define ONLRET		0000040
#define OFILL		0000100
#define OFDEL		0000200

#define NLDLY		0000400
#define NL0		0000000
#define NL1		0000400

#define CRDLY		0003000
#define CR0		0000000
#define CR1		0001000
#define CR2		0002000
#define CR3		0003000

#define TABDLY		0014000
#define TAB0		0000000
#define TAB1		0004000
#define TAB2		0010000
#define TAB3		0014000
#define XTABS		0014000

#define BSDLY		0020000
#define BS0		0000000
#define BS1		0020000

#define VTDLY		0040000
#define VT0		0000000
#define VT1		0040000

#define FFDLY		0100000
#define FF0		0000000
#define FF1		0100000

#define PAGEOUT		0200000
#define WRAP		0400000

#ifndef _LINUX_TERMIOS_H
#define CBAUD		0000017
#else				/* _LINUX_TERMIOS_H */
#define O_CBAUD		0000017
#endif				/* _LINUX_TERMIOS_H */
#define CSIZE		0000060
#define CS5		0000000
#define CS6		0000020
#define CS7		0000040
#define CS8		0000060
#define CSTOPB		0000100
#define CREAD		0000200
#define PARENB		0000400
#define PARODD		0001000
#define HUPCL		0002000
#define CLOCAL		0004000
#define RCV1EN		0010000
#define XMT1EN		0020000
#define LOBLK		0040000
#define XCLUDE		0100000
#ifndef _LINUX_TERMIOS_H
#define CIBAUD		03600000
#else				/* _LINUX_TERMIOS_H */
#define O_CIBAUD	03600000
#endif				/* _LINUX_TERMIOS_H */
#define PAREXT		04000000

#define ISIG		0000001
#define ICANON		0000002
#define XCASE		0000004
#define ECHO		0000010
#define ECHOE		0000020
#define ECHOK		0000040
#define ECHONL		0000100
#define NOFLSH		0000200
#define TOSTOP		0000400
#define ECHOCTL		0001000
#define ECHOPRT		0002000
#define ECHOKE		0004000
#define DEFECHO		0010000

#ifndef _LINUX_TERMIOS_H
#define FLUSHO		0020000
#else				/* _LINUX_TERMIOS_H */
#define O_FLUSHO	0020000
#endif				/* _LINUX_TERMIOS_H */

#define PENDIN		0040000
#define IXEXTERN	0100000

#define TIOC		('T'<<8)
#ifndef _LINUX_TERMIOS_H
#define TCGETA		(TIOC|1)
#define TCSETA		(TIOC|2)
#define TCSETAW		(TIOC|3)
#define TCSETAF		(TIOC|4)
#define TCSBRK		(TIOC|5)
#define TCXONC		(TIOC|6)
#define TCFLSH		(TIOC|7)

#define TIOCGWINSZ	(TIOC|104)
#define TIOCSWINSZ	(TIOC|103)

#define TCGETS		(TIOC|13)
#define TCSETS		(TIOC|14)
#define TCSETSW		(TIOC|15)
#define TCSETSF		(TIOC|16)
#else				/* _LINUX_TERMIOS_H */
#define O_TCGETA	(TIOC|1)
#define O_TCSETA	(TIOC|2)
#define O_TCSETAW	(TIOC|3)
#define O_TCSETAF	(TIOC|4)
#define O_TCSBRK	(TIOC|5)
#define O_TCXONC	(TIOC|6)
#define O_TCFLSH	(TIOC|7)

#define O_TIOCGWINSZ	(TIOC|104)
#define O_TIOCSWINSZ	(TIOC|103)

#define O_TCGETS	(TIOC|13)
#define O_TCSETS	(TIOC|14)
#define O_TCSETSW	(TIOC|15)
#define O_TCSETSF	(TIOC|16)
#endif				/* _LINUX_TERMIOS_H */

#define tIOC		('t'<<8)

#define N_TIOCREMOTE	(TIOC|105)

#define TIOCREMOTE	(tIOC|30)
#define TIOCSIGNAL	(tIOC|31)

#define TCIFLUSH	0
#define TCOFLUSH	1
#define TCIOFLUSH	2

#define TCOOFF		0
#define TCOON		1
#define TCIOFF		2
#define TCION		3

#define B0		0000000
#define B50		0000001
#define B75		0000002
#define B110		0000003
#define B134		0000004
#define B150		0000005
#define B200		0000006
#define B300		0000007
#define B600		0000010
#define B1200		0000011
#define B1800		0000012
#define B2400		0000013
#define B4800		0000014
#define B9600		0000015
#define B19200		0000016
#define B38400		0000017

#ifndef _LINUX_TERMIOS_H
struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};
#else				/* _LINUX_TERMIOS_H */
struct o_winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};
#endif				/* _LINUX_TERMIOS_H */

#endif				/* __SYS_TERMIOS_H__ */
