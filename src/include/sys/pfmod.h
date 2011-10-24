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

#ifndef _SYS_PFMOD_H
#define _SYS_PFMOD_H

/* This file can be processed with doxygen(1). */

/** @addtogroup pfmod
  * @{ */

/** @file
  * Packet Filter Module (pfmod) header file. */

/*
 * Packet Filter "pfmod" Header File.
 */

/** @name pfmod Packet Filter Module Input/Output Controls
  * Input/output controls for the packet filter module.
  * @{
  */
#define PFIOC			('P'<<8)
#define PFIOCSETF		(PFIOC|1)	/**< Set current packet filter. */
/** @} */

/** @} */

#define ENMAXFILTERS		255		/**< Maximum old packet filter short words. */
#define PF_MAXFILTERS		2047		/**< Maximum new packet filter short words. */

/** Filter structure for PCIOCSETF input-output control.
  */
struct packetfilt {
	uchar_t Pf_Priority;			/**< Priority of the filter. */
	uchar_t Pf_FilterLen;			/**< Length of the command list in the filter. */
	ushort_t Pf_Filter[ENMAXFILTERS];	/**< Filter command list. */
};

/** Extended packet filter structure.
  */
struct Pf_ext_packetfilt {
	uchar_t Pf_Priority;			/**< Priority of the filter. */
	unsigned int Pf_FilterLen;		/**< Length of the command list in the filter. */
	ushort_t Pf_Filter[PF_MAXFILTERS];	/**< Filter command list. */
};

#endif				/* _SYS_PFMOD_H */

