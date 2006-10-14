/*****************************************************************************

 @(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/14 06:37:27 $

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

 Last Modified $Date: 2006/10/14 06:37:27 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: v401p.c,v $
 Revision 0.9.2.1  2006/10/14 06:37:27  brian
 - added manpages, module, drivers, headers from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/14 06:37:27 $"

static char const ident[] = "$RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/14 06:37:27 $";

/*
 * This is an MX (Multiplex) STREAMS device driver for the Varion V401P card.
 * When Stream is opened on this driver it is a Multiplex (MX) Stream.
 * Multiplex (MX) Streams represent an entire span and provide timeslotting
 * of channels with in the data Stream.  MX Streams are bound to a specific
 * span.  The data stream is not demultiplexed by the driver and is
 * unswitched.  MX Streams are suitable for linking under an MX (Matrix) or
 * MG (Media Gateway) multiplexing driver.  The MX OR MG driver performs
 * Time-Space-Time switching as required.
 *
 * This is deviation from previous approaches for providing SS7 on the X400P
 * cards.  When an MX Stream for each span of a card is linked under an Matrix
 * driver, the user can then open a Channel (CH) Stream on the matrix, bind it
 * to any specific channel under the realm of the matrix switch, and have a
 * clear-channel data path for use in SS7, LAPB, LAPD, LAPF, whatever.  An SS7
 * SDL module is then pushed over the CH Stream to provide SS7 DAEDR/DAEDT
 * AERM/SUERM functions, and an SDT and SL module can be pushed from there to
 * form a complete MTP2 Signalling Link.
 *
 * With this approach, the Matrix switch can perform switching of channels
 * with codec (mu-law/A-law) conversion, digital padding, echo suppresion and
 * the like.  Switching is high performance regardless of it being done in
 * software.  We can also take the MX (Mulitplex) group blocks and pass them
 * over some network (PW3E, VLAN) without much manipulation at all.  Circuit
 * associated signalling information (if any) is passed with the MX blocks and
 * can be processed by MF or R2 controllers in the Matrix or beyond.
 *
 * A management Stream is provided on this device driver to perform management
 * functions.
 */


