/*****************************************************************************

 @(#) $RCSfile: ptem.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/29 11:40:07 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/09/29 11:40:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ptem.c,v $
 Revision 0.9.2.1  2006/09/29 11:40:07  brian
 - new files for strtty package and manual pages

 *****************************************************************************/

#ident "@(#) $RCSfile: ptem.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/29 11:40:07 $"

static char const ident[] = "$RCSfile: ptem.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/29 11:40:07 $";

/*
 * PTEM - Pseudo Terminal Emulation Module.
 *
 * Pushed over LDTERM to provide a pseudo terminal on the slave side.
 */


/*
   ptem -- STREAMS pseudo-terminal emulation module 

Description

   ptem  is  a  STREAMS  module that when used in conjunction with a line
   discipline  and  pseudo  terminal  driver  emulates  a  terminal.  See
   [6]pseudo(1).

   The  ptem  module must be pushed [see I_PUSH, [7]streamio(7)] onto the
   slave  side  of  a pseudo terminal STREAM, before the ldterm module is
   pushed.

   On  the  write-side,  the  TCSETA,  TCSETAF,  TCSETAW, TCGETA, TCSETS,
   TCSETSW, TCSETSF, TCGETS, TCSBRK, JWINSIZE, TIOCGWINSZ, and TIOCSWINSZ
   termio  [8]ioctl(2) messages are processed and acknowledged. A hang up
   (such  as  stty  0)  is  converted to a zero length M_DATA message and
   passed downstream. termio cflags window row and column information are
   stored  locally  one  per  stream. M_DELAY messages are discarded. All
   other messages are passed downstream unmodified.

   On  the read-side all messages are passed upstream unmodified with the
   following  exceptions.  All  M_READ  and M_DELAY messages are freed in
   both  directions. An ioctl TCSBRK is converted to an [9]M_BREAK(D7str)
   message  and  passed  upstream  and  an  acknowledgement  is  returned
   downstream.    An    ioctl    TIOCSIGNAL    is   converted   into   an
   [10]M_PCSIG(D7str) message, and passed upstream and an acknowledgement
   is  returned downstream. Finally an ioctl TIOCREMOTE is converted into
   an [11]M_CTL(D7str) message, acknowledged, and passed upstream.

References

   [12]crash(1M),      [13]ioctl(2),      [14]ldterm(7),     [15]pckt(7),
   [16]pseudo(1), [17]pty(7), [18]streamio(7), [19]stty(1), [20]termio(7)
     _________________________________________________________________

   [21]© 2002 Caldera International, Inc. All rights reserved.
   UnixWare 7 Release 7.1.3 - 30 October 2002
*/
