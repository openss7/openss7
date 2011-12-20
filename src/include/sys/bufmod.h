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

#ifndef _SYS_BUFMOD_H
#define _SYS_BUFMOD_H

/* This file can be processed with doxygen(1). */

/** @addtogroup bufmod
  * @{ */

/** @file
  * Buffer Module (bufmod) header file. */

/*
 * Buffer Module "bufmod" Header File.
 */

/** @name bufmod Input/Output Controls
  * Input/output controls for the buffer module.
  * @{
  */
#define SBIOC			('B'<<8)
#define SBIOCSTIME		(SBIOC|1)	/**< Set the delivery timeout. */
#define SBIOCGTIME		(SBIOC|2)	/**< Get the delivery timeout.  */
#define SBIOCCTIME		(SBIOC|3)	/**< Clear the delivery timeout.  */
#define SBIOCSCHUNK		(SBIOC|4)	/**< Set the chunksize in words. */
#define SBIOCGCHUNK		(SBIOC|5)	/**< Get the chunksize in words. */
#define SBIOCSSNAP		(SBIOC|6)	/**< Set the snapshot length in bytes. */
#define SBIOCGSNAP		(SBIOC|7)	/**< Get the snapshot length in bytes. */
#define SBIOCSFLAGS		(SBIOC|8)	/**< Set the buffering mode flags. */
#define SBIOCGFLAGS		(SBIOC|9)	/**< Get the buffering mode flags. */
/** @} */

/** @name default Default Chunk Size
  * Defult chunk size.
  * @{ */
#define SB_DFLT_CHUNK		8192		/**< Default chunk size. */
/** @} */

/** @name sb_flags Buffering Flags
  * Flags for use with SBIOCSFLAGS and SBIOCGFLAGS.
  * @{ */
#define SB_SEND_ON_WRITE	(1<<0)		/**< Deliver chunks on write side message. */
#define SB_NO_HEADER		(1<<1)		/**< Do no add header structure to data. */
#define SB_NO_PROTO_CVT		(1<<2)		/**< Do not convert M_PROTO to data. */
#define SB_DEFER_CHUNK		(1<<3)		/**< Defer chunk generation for one message. */
#define SB_NO_DROPS		(1<<4)		/**< Do not drop messages under flow control. */
#define SB_HIPRI_OOB            (1<<5)          /**< Deliver high-priority M_PROTO out of band. */
#define SB_NO_MREAD             (1<<6)          /**< Do not set stream head for M_READ. */
/** @} */

/** @name state Buffering State
  * State of the buffering.
  * @{ */
#define SB_FRCVD		1		/**< First message in time window received. */
/** @} */

/** Structure for use with M_DATA message blocks.
  *
  * When adding a given message to an accumulating chunk, the module first converts any leading
  * M_PROTO message blocks to M_DATA.  It the constructs an sb_hdr structure and prepends it to the
  * message, and pads the result out to force its length to a multiple of a machine-dependent
  * alignment size guaranteed to be at least sizeof(ulong_t).  It then adds the padded message to
  * the chunk.
  *
  * @member sb_origlen is the original length of the message after the M_PROTO to M_DATA conversion,
  * but before truncating or adding the header.
  *
  * @member sb_msglen is the length of the message after truncation, but before adding the header.
  *
  * @member sb_totlen is the length of the message after truncation, and including both the header,
  * itself and the trailing padding bytes.
  *
  * @member sb_drops is the cumulative number of messages dropped by the module due to stream
  * read-side flow control or resource exhaustion.
  *
  * @member sb_timestamp is the packet arrival time expressed as a 'struct timeval'.  Note that
  * whether the 32-bit or 64-bit version of struct timeval is used depends on the architecture of
  * the caller to the input-output control that activated the addition of headers.
  */
struct sb_hdr {
	uint sbh_origlen;
	uint sbh_msglen;
	uint sbh_totlen;
	uint sbh_drops;
#if defined(_LP64)  || defined (_I32LPx)
	struct timeval32 sbh_timestamp;
#else
	struct timeval sbh_timestamp;
#endif
};

/** @} */

#endif				/* _SYS_BUFMOD_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
