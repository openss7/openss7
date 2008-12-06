/*****************************************************************************

 @(#) $RCSfile: vtoip.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 09:06:06 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-12-06 09:06:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: vtoip.c,v $
 Revision 0.9.2.1  2008-12-06 09:06:06  brian
 - added working files

 *****************************************************************************/

#ident "@(#) $RCSfile: vtoip.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 09:06:06 $"

static char const ident[] = "$RCSfile: vtoip.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 09:06:06 $";

/*
 * This is a Y.1452 VToIP (Voice Trunking over IP) module.  It pushes over a UDP Stream that
 * provides connectivity to the peer VToIP system.  The upper boundary service interface is the MXI
 * interface.  The lower boundary service interface is the UDP-TPI interface.
 *
 * In general, the UDP Stream may be opened, options configured, bound to a local IP address and
 * port number, and connected to a remote IP address and port number.  This module can then be
 * pushed.  Pushing the module will flush the Stream and any data messages received on the Stream
 * will be discarded until the Stream is configured, enabled and connected.
 *
 * Once the module is pushed, the MXI Stream can be linked under the MATRIX multiplexing driver and
 * the channels available in the multiplex facility will be made available to the switching matrix.
 */

