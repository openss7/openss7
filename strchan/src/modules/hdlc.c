/*****************************************************************************

 @(#) $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 06:47:37 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

 Last Modified $Date: 2007/08/14 06:47:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: hdlc.c,v $
 Revision 0.9.2.3  2007/08/14 06:47:37  brian
 - GPLv3 header update

 Revision 0.9.2.2  2006/11/30 13:05:27  brian
 - checking in working copies

 Revision 0.9.2.1  2006/10/17 11:56:00  brian
 - copied files into new packages from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 06:47:37 $"

static char const ident[] =
    "$RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 06:47:37 $";

/*
 *  This is a pushable STREAMS module that provides the High-Level Data Link
 *  Control.  It presents a Communication Device Interface (CDI) at the upper
 *  service interface and uses a Channel Interface (CHI) at the lower service
 *  interface.  This module is for raw channel devices and support HDLC in
 *  software.  Some devices might support HDLC in device firmware, in which
 *  case, the device driver itself will present the CDI interface.  The CDI
 *  interface presented supports the lapb(4), lapf(4), lapd(4), lapm(4) data
 *  link modules and the sl(4) SS7 link module.
 *
 *  This module is originally intended for use with the V401P card, but can be
 *  used with any raw channel device.
 *
 *  This module can also present a pseudo-device driver interface to provide
 *  access to HDLC device driver implementations in the Linux kernel.  Also,
 *  raw CHI streams can be linked under the pseudo-device driver and then
 *  addressed by interface (multiplex) identifier from the upper multiplex.
 */
