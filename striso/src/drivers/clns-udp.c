/*****************************************************************************

 @(#) $RCSfile: clns-udp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:24 $

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

 Last Modified $Date: 2007/08/12 15:20:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: clns-udp.c,v $
 Revision 0.9.2.1  2007/08/12 15:20:24  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: clns-udp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:24 $"

static char const ident[] = "$RCSfile: clns-udp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:24 $";

/*
 *  This is an X.233 CLNS driver for RFC 1070 ISO-UDP.
 *
 *  The driver implements X.233 (CLNP) as a pseudo-device driver.  The driver presents an NPI CLNS
 *  service interface to the upper multiplex.  It only provides a CLNS service interface, and only
 *  accepts addresses in the form of RFC 1069/1070.
 *
 *  This driver is implemented as a multiplexing driver.  It supports the linking of multiple NP-UDP
 *  driver Streams bound to UDP Port Number 147.  It will send packets to the same UDP port number
 *  to which the linked Stream is bound.  This must be a provider Stream provided by the np-udp
 *  driver from the strxns package.  Each Stream must be bound before being linked.  Multiple
 *  Streams bound to individual local IP addresses, or a single wildcard IP address bound Stream
 *  (DEFAULT_DESTINATION), can be linked.
 *
 *  The Stream can either be temporarily or permanently linked.  When permanently linked, care
 *  should be taken to unlink the Stream as part of the shutdown procedure, otherwise the driver
 *  will refuse to unload.
 *
 *  Routing is performed with a specialized NSAP address (see RFC 1069, RFC 1070) which includes the
 *  IP address of the peer.  Binding is performed to a specific or wildcard address.  Unitdata
 *  messages are sent to a sepcific IP address.  Source addresses are chosen by the lower multiplex
 *  Stream (outgoing interface for DEFAULT_DESTINATION NP-IP provider Streams, and bound address for
 *  normal NP-IP provider Streams).  Source ports are the same as the port to which the lower Stream
 *  is bound.
 *
 *  This approach is very efficient.  The driver can merely adjust the DEST_offset and DEST_length
 *  in the user provided N_UNITDATA_REQ primitive to create an N_UNITDATA_REQ primitive for the
 *  NP-UDP driver and fill out a portion of the headroom in the associated M_DATA with the CLNP
 *  protocol header.
 */
