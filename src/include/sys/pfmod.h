/*****************************************************************************

 @(#) src/include/sys/pfmod.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

#ifndef _NET_NIT_PF_H
/** Filter structure for PCIOCSETF input-output control.
  */
struct packetfilt {
	uint8_t Pf_Priority;			/**< Priority of the filter. */
	uint8_t Pf_FilterLen;			/**< Length of the command list in the filter. */
	uint16_t Pf_Filter[ENMAXFILTERS];	/**< Filter command list. */
};
#endif				/* _NET_NIT_PF_H */

/** Extended packet filter structure.
  */
struct Pf_ext_packetfilt {
	uint8_t Pf_Priority;			/**< Priority of the filter. */
	uint32_t Pf_FilterLen;		/**< Length of the command list in the filter. */
	uint16_t Pf_Filter[PF_MAXFILTERS];	/**< Filter command list. */
};

/*
 *  We now allow specification of up to MAXFILTERS (short) words of a filter
 *  command list to be applied to incoming packets to determine if
 *  those packets should be given to a particular open ethernet file.
 *  Alternatively, PF_MAXFILTERS and Pf_ext_packetfilt structure can be
 *  used in case even bigger filter command list is needed.
 *
 *  In this context, "word" means a short (16-bit) integer.
 *
 *  Each open enet file specifies the filter command list via ioctl.
 *  Each filter command list specifies a sequence of actions that leaves a
 *  boolean value on the top of an internal stack.  Each word of the
 *  command list specifies an action from the set {PUSHLIT, PUSHZERO,
 *  PUSHWORD+N} which respectively push the next word of the filter, zero,
 *  or word N of the incoming packet on the stack, and a binary operator
 *  from the set {EQ, LT, LE, GT, GE, AND, OR, XOR} which operates on the
 *  top two elements of the stack and replaces them with its result.  The
 *  special action NOPUSH and the special operator NOP can be used to only
 *  perform the binary operation or to only push a value on the stack.
 *
 *  If the final value of the filter operation is true, then the packet is
 *  accepted for the open file which specified the filter.
 */

#ifdef __AIX_SOURCE

/*  these must sum to sizeof (ushort_t)!  */
#define	PF_NBPA	        10	/* # bits / action */
#define	PF_NBPO	         6	/* # bits / operator */

/*  binary operators  */
#define	PF_NOP		( 0 << PF_NBPA)
#define	PF_EQ		( 1 << PF_NBPA)
#define	PF_LT		( 2 << PF_NBPA)
#define	PF_LE		( 3 << PF_NBPA)
#define	PF_GT		( 4 << PF_NBPA)
#define	PF_GE		( 5 << PF_NBPA)
#define	PF_AND		( 6 << PF_NBPA)
#define	PF_OR		( 7 << PF_NBPA)
#define	PF_XOR		( 8 << PF_NBPA)
#define	PF_COR		( 9 << PF_NBPA)
#define	PF_CAND	        (10 << PF_NBPA)
#define	PF_CNOR	        (11 << PF_NBPA)
#define	PF_CNAND	(12 << PF_NBPA)
#define	PF_NEQ		(13 << PF_NBPA)

/*  stack actions  */
#define	PF_NOPUSH	 0
#define	PF_PUSHLIT	 1
#define	PF_PUSHZERO	 2
#define	PF_PUSHONE	 3
#define	PF_PUSHFFFF	 4
#define	PF_PUSHFF00	 5
#define	PF_PUSH00FF	 6
#define	PF_PUSHWORD	16

#define PF_CMD(x)
#define PF_OP(x)    (((x)>>PF_NBPA)&0x3f)

#else				/* __AIX_SOURCE */

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
#define	ENF_SL		(14 << ENF_NBPA)
#define	ENF_SR		(15 << ENF_NBPA)
#define	ENF_ADD		(16 << ENF_NBPA)
#define	ENF_SUB		(17 << ENF_NBPA)
#define ENF_MUL         (18 << ENF_NBPA)
#define ENF_DIV         (19 << ENF_NBPA)
#define ENF_REM         (20 << ENF_NBPA)
#define ENF_SWAP        (21 << ENF_NBPA)

/*  stack actions  */
#define	ENF_NOPUSH	 0	/* do not push */
#define	ENF_PUSHLIT	 1	/* push the next program word on the stack */
#define	ENF_PUSHZERO	 2	/* push 0x0000 on the stack */
#define	ENF_PUSHONE	 3	/* push 0x0001 on the stack */
#define	ENF_PUSHFFFF	 4	/* push 0xffff on the stack */
#define	ENF_PUSHFF00	 5	/* push 0xff00 on the stack */
#define	ENF_PUSH00FF	 6	/* push 0x00ff on the stack */
#define	ENF_LOAD_OFFSET	 7	/* load next program word into offset register */
#define	ENF_BRTR	 8	/* branch if the stack's top element is true */
#define	ENF_BRFL	 9	/* branch if the stack's top element is false */
#define	ENF_POP		10	/* pop the top element from the stack */
#define	ENF_PUSHLEN	11	/* push the length of the packet on the stack */
#define	ENF_PUSHOFFSET	12	/* push the offset register onto the stack */
#define	ENF_POPOFFSET	13	/* pop the offset register from the stack */
#define	ENF_DUP		14	/* duplicate the value on the stack */
#define	ENF_PUSHWORD	16	/* push the n'th short-word of the message on the stack */

#endif				/* __AIX_SOURCE */

#endif				/* _SYS_PFMOD_H */
