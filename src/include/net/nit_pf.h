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

#ifndef _NET_NIT_PF_H
#define _NET_NIT_PF_H

/* This file can be processed with doxygen(1). */

/** @addtogroups nit
  * @{ */

/** @file
  * Network Interface Tap packet filter (nit_pf) header file.  */

#define ENMAXFILTERS		255		/**< Maximum old packet filter short words. */

/** Filter structure for NIOCSETF input-output control.
  */
struct packetfilt {
	uint8_t Pf_Priority;			/**< Priority of the filter. */
	uint8_t Pf_FilterLen;			/**< Length of the command list in the filter. */
	uint16_t Pf_Filter[ENMAXFILTERS];	/**< Filter command list. */
};

#ifndef NIOC
#define NIOC ('p' << 8)
#endif

/** @name Old SNIT packet filter input-output controls.
  * @{ */
#define NIOCSETF	_IOW('p', 2, struct packetfilt)	/**< Set packet filter. */
/** @} */

/*  these must sum to sizeof (ushort_t)!  */
#define	ENF_NBPA	10	/* # bits / action */
#define	ENF_NBPO	 6	/* # bits / operator */

/*  binary operators  */
#define	ENF_NOP		( 0 << ENF_NBPA)
#define	ENF_EQ		( 1 << ENF_NBPA)
#define	ENF_LT		( 2 << ENF_NBPA)
#define	ENF_LE		( 3 << ENF_NBPA)
#define	ENF_GT		( 4 << ENF_NBPA)
#define	ENF_GE		( 5 << ENF_NBPA)
#define	ENF_AND		( 6 << ENF_NBPA)
#define	ENF_OR		( 7 << ENF_NBPA)
#define	ENF_XOR		( 8 << ENF_NBPA)
#define	ENF_COR		( 9 << ENF_NBPA)
#define	ENF_CAND	(10 << ENF_NBPA)
#define	ENF_CNOR	(11 << ENF_NBPA)
#define	ENF_CNAND	(12 << ENF_NBPA)
#define	ENF_NEQ		(13 << ENF_NBPA)

/*  stack actions  */
#define	ENF_NOPUSH	 0  /* do not push */
#define	ENF_PUSHLIT	 1  /* push the next program word on the stack */
#define	ENF_PUSHZERO	 2  /* push 0x0000 on the stack */
#define	ENF_PUSHONE	 3  /* push 0x0001 on the stack */
#define	ENF_PUSHFFFF	 4  /* push 0xffff on the stack */
#define	ENF_PUSHFF00	 5  /* push 0xff00 on the stack */
#define	ENF_PUSH00FF	 6  /* push 0x00ff on the stack */
#define	ENF_PUSHWORD	16  /* push the n'th short-word of the message on the stack */

/** @} */

#endif				/* _NET_NIT_PF_H */
