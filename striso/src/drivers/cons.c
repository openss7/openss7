/*****************************************************************************

 @(#) $RCSfile: cons.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 07:05:03 $

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

 Last Modified $Date: 2007/08/14 07:05:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cons.c,v $
 Revision 0.9.2.3  2007/08/14 07:05:03  brian
 - GNUv3 header update

 Revision 0.9.2.2  2007/08/12 16:00:24  brian
 - updates, still working up CONS

 Revision 0.9.2.1  2006/10/10 10:12:40  brian
 - added losts of striso files

 *****************************************************************************/

#ident "@(#) $RCSfile: cons.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 07:05:03 $"

static char const ident[] =
    "$RCSfile: cons.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/14 07:05:03 $";

/*
 *  This is an X.223 CONS driver.
 *
 *  The driver implements X.223 (support of CONS using X.25) as a multiplexing pseudo-device driver.
 *  Per X.223, it implements the X.25 PLP in support of the CONS service interface.  The driver
 *  presents a NPI CONS service interface at the upper multiplex.
 *
 *  Data links Streams providing the data link service using the DLPI interface are linked beneath
 *  the multiplexing driver.  Data link Streams should be attached, bound and connected.  Data link
 *  Streams can be Streams of any type.  In support of X.223 they can be LLC2 connections per
 *  ISO/IEC 8881, ISDN connections per X.612 ISO/IEC 9574, X.21/X.21 bis PADS or X.30 ISDN TAs per
 *  X.613 ISO/IEC 10588, or PSTN per X.614 ISO/IEC 10732.
 *
 *  Routing:
 *
 *  X.121 address-based routing is performed by associating a network title (IS) or address (ES)
 *  with each data link to form a routing table.  Network titles or addresses are associated with a
 *  data link once it has been linked beneath the multiplexing driver using input-output controls.
 *  Packets received that are associated with external addresses are routed out another data link or
 *  rejected.  Packets received that are associated with a locally assigned address are processed
 *  against the CONS interface.
 *
 */

/*
 * vim: tw=100
 */
