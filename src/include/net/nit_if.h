/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef _NET_NIT_IF_H
#define _NET_NIT_IF_H

/* This file can be processed with doxygen(1). */

/** @addtogroup nit
  * @{ */

/** @file
  * Network Interface Tap interface (nit_if) header file.  */

/*
 * STREAMS Network Interface Tab interface "nit_if" header file.
 */

/** @name Old SNIT interface input-output controls.
  * @{ */
#define NIOCBIND	_IOW(p, 3, struct ifreq)	/**< Bind to interface. */
#define NIOCSFLAGS	_IOW(p, 4, u_long)		/**< Set nit_if flags. */
#define NIOCGFLAGS	_IOWR(p, 5, u_long)		/**< Get nit_if flags. */
#define NIOCSSNAP	_IOW(p, 6, u_long)		/**< Set snapshot length. */
#define NIOCGSNAP	_IOWR(p, 7, u_long)		/**< Get Snapshot length. */
/** @} */

/** @name Old SNIT interface user-visible flags.
  * @{ */
#define NI_PROMISC	(1<<0)		/**< Put interface into promiscuous mode. */
#define NI_TIMESTAMP	(1<<1)		/**< Prepend a timestamp header. */
#define NI_LEN		(1<<2)		/**< Prepend a length header. */
#define NI_DROPS	(1<<3)		/**< Prepend a packet drops header. */
#define NI_USERBITS	(0x0f)		/**< User visible flags mask. */
/** @} */

/** @name Old prepended headers.
  *
  * Depending on the state of the flag bit set with the NIOCSFLAGS input-output
  * control, each packet will have one or more headers prepended.  These headers
  * are packed contiguous to the packet and themselves with no intervening
  * space.  Their lenghts are chosen to avoid alignment problems.  Prepending
  * occurs in the oder of the header definitions given below, with each
  * successive header appearing in front of everythi9ng that's already in place.
  * @{ */

/** Header for NI_LEN flag bit.
  *
  * This header is prepended to each packet when the NI_LEN flag bit is set.  It
  * contains the packet's length as of the time it was received, including the
  * link-lvel header.  If does not account for any of the headers that nit_if
  * adds nor for trimming the packet to the snapshot length.
  */
struct nit_iflen {
	u_long nh_pkglen;
};

/** Header for NI_DROPS flag bit.
  *
  * This header is prepended to each packet when the NI_DROPS flag bit is set.
  * It records the cumulative number of packets dropped on this stream because
  * of flow control requirements since the stream was opened.
  */
struct nit_ifdrops {
	u_long nh_drops;
};

/** Header for NI_TIMESTAMP flags bit.
  *
  * This  header is prepended to each packet when the NI_TIMESTAMP flag bit is
  * set.  It contains the timestamp of the arrival time of the packet.
  */
struct nit_iftime {
	struct timeval nh_timestamp;
};

/** @} */

/** @} */

#endif				/* _NET_NIT_IF_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
