/*****************************************************************************

 @(#) $Id: stropts.h,v 0.9.2.26 2006/09/18 13:52:41 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>

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

 Last Modified $Date: 2006/09/18 13:52:41 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stropts.h,v $
 Revision 0.9.2.26  2006/09/18 13:52:41  brian
 - added doxygen markers to sources

 Revision 0.9.2.25  2006/07/13 12:55:48  brian
 - updated documentation for sk_buffs

 Revision 0.9.2.24  2006/03/03 10:57:11  brian
 - 32-bit compatibility support, updates for release

 Revision 0.9.2.23  2006/02/20 10:59:20  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ifndef __SYS_STREAMS_STROPTS_H__
#define __SYS_STREAMS_STROPTS_H__

#ident "@(#) $RCSfile: stropts.h,v $ $Name:  $($Revision: 0.9.2.26 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/**
 * @file
 * @brief Defines the interface to STREAMS(9) character device special files.
 * @{ */

#ifndef __SYS_STROPTS_H__
//#warning "Do no include sys/streams/stropts.h directly, include sys/stropts.h instead."
#endif

#ifndef HAVE_LINUX_FAST_STREAMS
#define HAVE_LINUX_FAST_STREAMS
#endif				/* HAVE_LINUX_FAST_STREAMS */

#ifdef __KERNEL__
#undef STR			/* defined in some asm/hw_irq.h */
#include <linux/types.h>	/* for pid_t */
#else
#include <sys/types.h>		/* for uid_t */
#endif

#ifndef t_uscalar_t
#ifdef __LP64__
typedef u_int32_t t_uscalar_t;
#else				/* __LP64__ */
typedef unsigned long int t_uscalar_t;
#endif				/* __LP64__ */
#define t_uscalar_t t_uscalar_t
#endif

#define __SID		('S' << 8)
#define STR		('S'<<8)	/* for UnixWare/Solaris compatibility */

#undef __NOT_IMPLEMENTED__

/**
 * @name STREAMS Input-Output Controls
 *
 * These are standard (SUS/POSIX) input-output control command definitions for
 * STREAMS character device special files.  All of these are implemented by
 * Linux Fast-STREAMS with SVR4 compatible numbering.  The first group are
 * SVR3 compatible.  The later groups are SVR4 compatible.  All are SUS/POSIX
 * compatible.
 *
 * @{ */

/* Following are SVR3 compatibile definitions. */

#define I_NREAD			(__SID | 1)	/**< Number of bytes in first message. */
#define I_PUSH			(__SID | 2)	/**< Push a STREAMS module onto the Stream. */
#define I_POP			(__SID | 3)	/**< Pop the top STREAMS module from the Stream. */
#define I_LOOK			(__SID | 4)	/**< Retrieve the name of topmost module on Stream. */
#define I_FLUSH			(__SID | 5)	/**< Flush a Stream. */
#define I_SRDOPT		(__SID | 6)	/**< Set read options for a Stream. */
#define I_GRDOPT		(__SID | 7)	/**< Get read options for a Stream. */
#define I_STR			(__SID | 8)	/**< Formulate a STREAMS ioctl(). */
#define I_SETSIG		(__SID | 9)	/**< Set signals for a Stream. */
#define I_GETSIG		(__SID |10)	/**< Get signals for a Stream. */
#define I_FIND			(__SID |11)	/**< Find a module in a Stream. */
#define I_LINK			(__SID |12)	/**< Temporarily link a Stream under a mux. */
#define I_UNLINK		(__SID |13)	/**< Temporarily unlink a Stream from a mux. */
#define I_RECVFD		(__SID |14)	/**< Receive a file descriptor. Non-EFT definition. */
#define I_PEEK			(__SID |15)	/**< Copy first message without removing it. */
#define I_FDINSERT		(__SID |16)	/**< Insert a file pointer into a message. */
#define I_SENDFD		(__SID |17)	/**< Send a file descriptor on a STREAMS pipe. */
#define I_E_RECVFD		(__SID |18)	/**< Receive a file descriptor. EFT definition. */

/* These were numbered differently in SVR3. */

#define I_PLINK			(__SID |22)	/* R3(19)   *//**< Permanently link a Stream under a mux. */
#define I_PUNLINK		(__SID |23)	/* R3(20)   *//**< Permanently unlink a Stream from a mux. */

/* Following are SVR4 compatible definitions. */

#define I_SWROPT		(__SID |19)	/* HOM(20)  *//**< Set write options for a Stream. */
#define I_GWROPT		(__SID |20)	/* HOM(21)  *//**< Get write options for a Stream. */
#define I_LIST			(__SID |21)	/* HOM(22)  *//**< List modules on a Stream. */

#define I_FLUSHBAND		(__SID |28)	/* HOM(19)  *//**< Flush a band of a Stream. */
#define I_CKBAND		(__SID |29)	/* HOM(24)  *//**< Check if message of given band is available. */
#define I_GETBAND		(__SID |30)	/* HOM(25)  *//**< Get priority band of first message on Stream. */
#define I_ATMARK		(__SID |31)	/* HOM(23)  *//**< Check if first message is marked. */
#define I_SETCLTIME		(__SID |32)	/* HOM(27)  *//**< Set close timeout. */
#define I_GETCLTIME		(__SID |33)	/* HOM(28)  *//**< Get close timeout. */
#define I_CANPUT		(__SID |34)	/* HOM(26)  *//**< Test a band for flow control. */
/** @} */

/**
  * @name Solaris specific Input-Output Controls
  *
  * The first of these were not part of SVID but were Solaris defs; the last are
  * Solaris specific.  #I_ANCHOR eventually moved to fill start again in the old
  * slot at 24.
  *
  * @{ */
#define I_ANCHOR		(__SID |24)	/* S(37)    *//**< Set an module anchor. */

#ifdef __NOT_IMPLEMENTED__
#define I_SETEV			(__SID |24)	/* Solaris  *//**< Set an event (obsolete). */
#define I_GETEV			(__SID |25)	/* Solaris  *//**< Get an event (obsolete). */
#define I_STREV			(__SID |26)	/* Solaris  *//**< Create Stream event (obsolete). */
#define I_UNSTREV		(__SID |27)	/* Solaris  *//**< Destroy Stream event (obsolete). */
#endif				/* __NOT_IMPLEMENTED__ */

#define I_SERROPT		(__SID |35)	/* Solaris  *//**< Set error options. */
#define I_GERROPT		(__SID |36)	/* Solaris  *//**< Get error options. */
#define I_ESETSIG		(__SID |37)	/* Solaris  *//**< Extended set signals. */
#define I_EGETSIG		(__SID |38)	/* Solaris  *//**< Exterded get signals. */

#ifdef __NOT_IMPLEMENTED__
#define __I_PUSH_NOCTTY		(__SID |39)	/* Solaris  *//**< Internal. */
#define I_STRFT			(__SID |40)	/* Solaris  *//**< Internal. */
#define I_STRFTON		(__SID |41)	/* Solaris  *//**< Internal. */
#define I_STRFTOFF		(__SID |42)	/* Solaris  *//**< Internal. */
#define I_STRFTSTR		(__SID |43)	/* Solaris  *//**< Internal. */
#define I_STRFTALL		(__SID |44)	/* Solaris  *//**< Internal. */
#define I_STRFTZERO		(__SID |45)	/* Solaris  *//**< Internal. */
#define _I_MUXID2FD		(__SID |46)	/* Solaris  *//**< Internal. */
#define _I_INSERT		(__SID |47)	/* Solaris  *//**< Internal. */
#define _I_REMOVE		(__SID |48)	/* Solaris  *//**< Internal. */
#endif				/* __NOT_IMPLEMENTED__ */
/** @} */

/**
  * @name UnixWare specific Input-Output Controls
  *
  * These overlap with the more useful Solaris definitions.  Linux Fast-STREAMS
  * does not currently implement these, but the useful ones from the set above.
  *
  * @{ */
#ifdef __NOT_IMPLEMENTED__
#define I_S_RECVFD		(__SID |35)	/* UnixWare *//**< Secure receive file descriptor. */
#define I_STATS			(__SID |36)	/* UnixWare *//**< Get statistics. */
#define I_BIGPIPE		(__SID |37)	/* UnixWare *//**< Get large pipe. */
#define I_GETTP			(__SID |38)	/* UnixWare *//**< Get transport provider. */
#endif				/* __NOT_IMPLEMENTED__ */
/** @} */

/**
  * @name HP-UX, OSF, AIX Secured Input-Output Controls
  *
  * Linux Fast-STREAMS does not currently implement these.
  *
  * @{ */
#ifdef __NOT_IMPLEMENTED__
#define I_STR_ATTR		(__SID |60)	/* HPUX,OSF *//**< Secure I_STR. */
#define I_PEEK_ATTR		(__SID |61)	/* HPUX,OSF *//**< Secure I_PEEK. */
#define I_FDINSERT_ATTR		(__SID |62)	/* HPUX,OSF *//**< Secure I_FDINSERT. */
#define I_SENDFD_ATTR		(__SID |63)	/* HPUX,OSF *//**< Secure I_SENDFD. */
#define I_RECVFD_ATTR		(__SID |64)	/* HPUX,OSF *//**< Secure I_RECVFD. */
#define I_GETMSG_ATTR		(__SID |65)	/* HPUX,OSF *//**< Secure I_GETMSG. */
#define I_PUTMSG_ATTR		(__SID |66)	/* HPUX,OSF *//**< Secure I_PUTMSG. */
#define I_GETPMSG_ATTR		(__SID |67)	/* HPUX,OSF *//**< Secure I_GETPMSG. */
#define I_PUTPMSG_ATTR		(__SID |68)	/* HPUX,OSF *//**< Secure I_PUTPMSG. */
#endif				/* __NOT_IMPLEMENTED__ */
/** @} */

/**
  * @name HP-UX, OSF, AIX, Mac OT specific Input-Output Controls
  *
  * OSF defintions.  Linux Fast-STREAMS implements all of these (except
  * I_TILDE) for ioctl() emulation of system calls.
  *
  * @{ */
#ifdef __NOT_IMPLEMENTED__
#define I_TILDE			(__SID |37)	/**< tty tilde option (O). */
#endif				/* __NOT_IMPLEMENTED__ */

#define I_GETMSG		(__SID |40)	/* HPUX,OSF *//**< getmsg() syscall emulation. */
#define I_PUTMSG		(__SID |41)	/* HPUX,OSF *//**< putmsg() syscall emulation. */
#define I_GETPMSG		(__SID |42)	/* HO,M(40) *//**< getpmsg() syscall emulation. */
#define I_PUTPMSG		(__SID |43)	/* HO,M(41) *//**< putpmsg() syscall emulation. */
#define I_PIPE			(__SID |44)	/* HO,M(49) *//**< pipe() syscall emulation. */
#define I_FIFO			(__SID |45)	/* HO,M(51) *//**< FIFO syscall emulation. */
#define I_ISASTREAM		(__SID |46)	/* HO(14)   *//**< isastream() syscall emulation. */
/** @} */

/** @name Mac OT specific Input-Output Controls
  *
  * Mac OT definitions.  The commands #I_FATTACH and #I_FDETACH are implemented
  * by Linux Fast-STREAMS for ioctl() emulation of system calls.
  *
  * @{ */
#ifdef __NOT_IMPLEMENTED__
#define I_POLL			(__SID |42)	/* Mac OT   *//**< */
#define I_SETDELAY		(__SID |43)	/* Mac OT   *//**< */
#define I_GETDELAY		(__SID |44)	/* Mac OT   *//**< */
#define I_RUN_QUEUES		(__SID |45)	/* Mac OT   *//**< */

#define I_AUTOPUSH		(__SID |48)	/* Mac OT   *//**< sad(4) syscall emulation. */
#define I_HEAP_REPORT		(__SID |50)	/* Mac OT   *//**< Heap debugging. */
#endif				/* __NOT_IMPLEMENTED__ */

#define I_FATTACH		(__SID |52)	/* M,L(244) *//**< fattach() syscall emulation. */
#define I_FDETACH		(__SID |53)	/* M,L(245) *//**< fdefatch() syscall emulation. */
/** @} */

/** @name HP-UX specific Input-Output Controls
  *
  * None of these are implemented by Linux Fast-STREAMS.
  *
  * @{ */
#ifdef __NOT_IMPLEMENTED__
#define I_LINK_DEV		(__SID | 31)	/* HPUX     *//**< */
#define I_PLINK_DEV		(__SID | 32)	/* HPUX     *//**< */

#define I_ALTSTRTAB_ACTIVE	(__SID | 50)	/* HPUX     *//**< */
#define I_ALTSTRTAB_FUTURE	(__SID | 51)	/* HPUX     *//**< */

#define I_SETCOW		(__SID | 80)	/* HPUX     *//**< */
#define I_SETCOPYAVOID		(__SID | 81)	/* HPUX     *//**< */
#define I_ASYNC_ACK		(__SID | 82)	/* HPUX     *//**< */
#define I_SET_XTI		(__SID | 84)	/* HPUX     *//**< */
#define I_GET_STH_WROFF		(__SID | 85)	/* HPUX     *//**< */
#define I_GET_STRMSGSZ		(__SID | 86)	/* HPUX     *//**< */
#define I_GET_STRCTLSZ		(__SID | 87)	/* HPUX     *//**< */
#define I_GET_PUSHCNT		(__SID | 88)	/* HPUX     *//**< */

#define I_SET_HIWAT		(__SID | 91)	/* HPUX     *//**< */
#define I_SET_LOWAT		(__SID | 92)	/* HPUX     *//**< */

#define I_GET_HIWAT		(__SID | 97)	/* HPUX     *//**< */
#define I_GET_LOWAT		(__SID | 98)	/* HPUX     *//**< */
#endif				/* __NOT_IMPLEMENTED__ */
/** @} */

/**
  * @name LiS specific Input-Output Controls
  *
  * Only the getpmsg(), putpmsg(), pipe(), fattach() and fdetach() versions of
  * these are implemented for library compatibility with LiS libraries (and this
  * LiS applications).
  *
  * @{ */
#define I_LIS_GETPMSG		(__SID | 42)	/* L 2.18.4 *//**< getpmsg() syscall emulation. */
#define I_LIS_PUTPMSG		(__SID | 43)	/* L 2.18.4 *//**< putpmsg() syscall emulation. */

#define I_LIS_PIPE		(__SID |243)	/* L 2.18.0 *//**< pipe() syscall emulation. */
#define I_LIS_FATTACH		(__SID |244)	/* L 2.18.0 *//**< fattach() syscall emulation. */
#define I_LIS_FDETACH		(__SID |245)	/* L 2.18.0 *//**< fdetach() syscall emaulation. */

#ifdef __NOT_IMPLEMENTED__
#define I_LIS_SEMTIME 		(__SID |239)	/* L 2.18.0 *//**< */
#define I_LIS_LOCKS 		(__SID |240)	/* L 2.18.0 *//**< */
#define I_LIS_SDBGMSK2 		(__SID |241)	/* L 2.18.0 *//**< */
#define I_LIS_QRUN_STATS	(__SID |242)	/* L 2.18.0 *//**< */

#define I_LIS_PRNTQUEUES 	(__SID |246)	/* L 2.18.0 *//**< */
#define I_LIS_PRNTSPL		(__SID |247)	/* L 2.18.0 *//**< */
#define I_LIS_GET_MAXMSGMEM	(__SID |248)	/* L 2.18.0 *//**< */
#define I_LIS_SET_MAXMSGMEM	(__SID |249)	/* L 2.18.0 *//**< */
#define I_LIS_GET_MAXMEM	(__SID |250)	/* L 2.18.0 *//**< */
#define I_LIS_SET_MAXMEM	(__SID |251)	/* L 2.18.0 *//**< */
#define I_LIS_GETSTATS 		(__SID |252)	/* L 2.18.0 *//**< */
#define I_LIS_PRNTSTRM 		(__SID |253)	/* L 2.18.0 *//**< */
#define I_LIS_PRNTMEM		(__SID |254)	/* L 2.18.0 *//**< */
#define I_LIS_SDBGMSK  		(__SID |255)	/* L 2.18.0 *//**< */
#endif				/* __NOT_IMPLEMENTED__ */
/** @} */

/**
  * @name Special Timeouts
  *
  * A special timeout for use as an argument to the #I_GETCLTIME or within the
  * struct strioctl#ic_timout member in the argument to the #I_STR input-output
  * control commands.
  *
  * @{ */
#define INFTIM		(-1UL)		/**< Default streamio(7) timeout. */
/** @} */

/**
  * @name Maximum Module Name Size
  *
  * Maximum module name sizes for use in structures containing module names
  * passed as arguments to or returned from the #I_LOOK, #I_LIST, #I_PUSH
  * input-output control commands.
  *
  * @{ */
#define FMNAMESZ	8		/**< Max module name size (compatible w/ UnixWare and Solaris). */
/** @} */

/**
  * @name Flush Bits
  * Flush bits for use with #I_FLUSH and #I_FLUSHBAND.
  * @{ */
#define FLUSHR		(1<<0)		/**< Flush read queues.  */
#define FLUSHW		(1<<1)		/**< Flush write queues.  */
#define FLUSHRW		(FLUSHR|FLUSHW)	/**< Flush read and write queues.  */
#define FLUSHBAND	(1<<2)		/**< Flush only specified band.  */
/** @} */

/**
  * @name Memory Mapping Flags
  * These flags are not currently implemented by Linux Fast-STREAMS.
  * @{ */
#define MAPINOK		(1<<0)		/**< mapin(9) instead of copyin(9). */
#define NOMAPIN		(1<<1)		/**< Normal copyin(9). */
#define REMAPOK		(1<<2)		/**< Page flip instead of copyout(9). */
#define NOREMAP		(1<<3)		/**< Normal copyout(9). */
/** @} */

#ifdef __KERNEL__
#define S_INPUT_BIT	0		/**< Signal on input bit. */
#define S_HIPRI_BIT	1		/**< Signal on high priority input bit. */
#define S_OUTPUT_BIT	2		/**< Signal on output bit. */
#define S_MSG_BIT	3		/**< Signal on #M_SIG message bit. */
#define S_ERROR_BIT	4		/**< Signal on #M_ERROR message bit. */
#define S_HANGUP_BIT	5		/**< Signal on #M_HANGUP message bit. */
#define S_RDNORM_BIT	6		/**< Signal on normal read bit. */
#define S_WRNORM_BIT	S_OUTPUT_BIT	/**< Signal on normal write bit. */
#define S_RDBAND_BIT	7		/**< Signal on banded read bit. */
#define S_WRBAND_BIT	8		/**< Signal on banded write bit. */
#define S_BANDURG_BIT	9		/**< Signal on band urgent (#ATMARK) bit. */
#endif

/**
  * @name SIGPOLL Bits
  *
  * Bits for use with #I_GETSIG and #I_SETSIG.  These bits are used in the
  * argument to #I_SETSIG and returned from #I_GETSIG input-output controls.
  * Also for use with the struct strsetsig#ss_events member in the argument to
  * #I_ESETSIG and #I_EGETSIG.  They correspond to poll() bits.
  *
  * @{ */
#define S_INPUT		(1<<0)		/**< Signal on input. */
#define S_HIPRI		(1<<1)		/**< Signal on high priority. */
#define S_OUTPUT	(1<<2)		/**< Signal on output. */
#define S_MSG		(1<<3)		/**< Signal on M_SIG(9) message. */
#define S_ERROR		(1<<4)		/**< Signal on M_ERROR(9) message. */
#define S_HANGUP	(1<<5)		/**< Signal on M_HANGUP(9) message. */
#define S_RDNORM	(1<<6)		/**< Signal on normal read. */
#define S_WRNORM	S_OUTPUT	/**< Signal on normal write. */
#define S_RDBAND	(1<<7)		/**< Signal on banded read. */
#define S_WRBAND	(1<<8)		/**< Signal on banded write. */
#define S_BANDURG	(1<<9)		/**< Signal on band urgent (ATMARK). */
#define S_ALL		(S_INPUT\
			|S_HIPRI\
			|S_OUTPUT\
			|S_MSG\
			|S_ERROR\
			|S_HANGUP\
			|S_RDNORM\
			|S_WRNORM\
			|S_RDBAND\
			|S_WRBAND\
			|S_BANDURG)	/**< Bitmask of all signals */
/** @} */

/**
  * @name Get Message Flags
  * Flags for use with getmsg().
  * @{
  *
  * @def RS_HIPRI
  *	Only get high priority messages.  This is the standard flag to getmsg()
  *	used to request retrieval of high priority messages only.
  *
  * @def RS_EXDATA
  *	Only get expedited data messages (Mac OT).  This option is Mac
  *	OpenTransport specific and non-standard.  It requests the retrieval of
  *	expedited data messages only.
  */
#define RS_HIPRI	(1<<0)		/**< Only get high priority messages. */
#define STRUIO_POSTPONE	(1<<3)	/* Solaris */
#define STRUIO_MAPIN	(1<<4)	/* Solaris */
#define RS_EXDATA	(1<<5)	/* Mac OT */
#define RS_ALLOWAGAIN	(1<<6)	/* Mac OT */
#define RS_DELIMITMSG	(1<<7)	/* Mac OT */
/** @} */

/**
  * @name Read Options
  *
  * Read mode and protocol bits for use with #I_SRDOPT and #I_GRDOPT.  OSF also
  * adds @c RFILL and @c RPROTCOMPRESS but these are incompatible with i386 ABI.
  * Mac OT gets these wrong (or maybe it just follows OSF).
  *
  * @{ */
#define RNORM		(0)	/**< Byte-stream mode. */
#define RMSGD		(1<<0)	/**< Message discard mode. */
#define RMSGN		(1<<1)	/**< Message non-discard mode. */

#define RMODEMASK	(RNORM|RMSGD|RMSGN)	/**< Mode mask. */

#define RPROTDAT	(1<<2)	/**< Proto part as data. */
#define RPROTDIS	(1<<3)	/**< Proto part discarded. */
#define RPROTNORM	(1<<4)	/**< Proto part fail read EBADMSG. */

#define RPROTMASK	(RPROTDAT|RPROTDIS|RPROTNORM)	/**< Proto bit mask. */
/** @} */

/**
  * @name Write Options
  *
  * Write option bits for use with #I_SWROPT and #I_GWROPT.  SUS/POSIX does not
  * define all of these.
  *
  * @{ */
#define SNDZERO		(1<<0)	/**< Permit sending of zero length messages. */
#define SNDPIPE		(1<<1)	/**< Sending on a disconnected pipe send SIGPIPE. */
#define SNDHOLD		(1<<2)	/**< Coallese small messages. */
/** @} */

/**
  * @name Stream Error Options
  *
  * Error options for use with #I_GERROPT and #I_SERROPT.  This is @e Solaris
  * specific behaviour.
  *
  * @{
  *     @def RERRNORM
  *	    Normal SVR4 error behaviour on read.
  *     @def RERRNONPERSIST
  *	    Non-persistent error behaviour on read.
  *     @def WERRNORM
  *	    Normal SVR4 error behaviour on write.
  *     @def WERRNONPERSIST
  *	    Non-persistent error behaviour on write.
  */
#define RERRNORM	(0<<0)	/**< Normal SVR4 error behaviour on read. */
#define RERRNONPERSIST	(1<<0)	/**< Non-persistent error behaviour on read. */
#define WERRNORM	(0<<1)	/**< Normal SVR4 error behaviour on write. */
#define WERRNONPERSIST	(1<<1)	/**< Non-persistent error behaviour on write. */
/** @} */

/**
  * @name Marking Bits
  *
  * Marking bits for use as an argument to the #I_ATMARK input-output control
  * command.
  * @{ */
#define ANYMARK		(1<<0)	/**< Indicate whether at a marked byte. */
#define LASTMARK	(1<<1)	/**< Indicate whether at last marked byte. */
/** }@ */

/**
  * @name Multiplex Values
  *
  * Special multiplex identifiers for use as an argument to the #I_PUNLINK
  * input-output control command.
  * @{ */
#define MUXID_ALL	(-1)	/**< Unlink all links from under multiplexer. */
/** @} */

/**
  * @name Get and Put Priority Message Bits
  *
  * Message flag bits for use with getpmsg() and putpmsg().  Not all flags are
  * SUS/POSIX flags.  The UnixWare #MSG_DISCARD and MSG_PEEKIOTCL flags are not
  * implemented.
  *
  * @{ */
#define MSG_HIPRI	(1<<0)	/**< Put or get a high priority message. */
#define MSG_ANY		(1<<1)	/**< Get a message of any band. */
#define MSG_BAND	(1<<2)	/**< Put or get a message of specified band. */
#define MSG_DISCARD	(1<<3)	/* UnixWare */
#define MSG_PEEKIOCTL	(1<<4)	/* UnixWare */
/** @} */

/**
  * @name Get Message Return Values
  * Return values possibly returned by getmsg() and getpmsg().
  * @{ */
#define MORECTL		1	/**< More control part available. */
#define MOREDATA	2	/**< More data part available. */
/** @} */

/**
  * Band information structure.
  * For use with as the argument to the #I_FLUSHBAND input-output control
  * command.
  */
struct bandinfo {
	unsigned char bi_pri;	/**< Priority of band to flush. */
	int bi_flag;		/**< Flush flags. */
};

/**
  * STREAMS buffer.
  * For use to describe control and data parts for calls to getmsg(), getpmsg(), putmsg(),
  * putpmsg().
  */
struct strbuf {
	int maxlen;		/**< Maximum length of buffer for output. */
	int len;		/**< Length of buffer for input. */
	char *buf;		/**< Pointer to the buffer. */
};

/**
  * STREAMS peek buffer.
  * For use as the argument to the #I_PEEK input-output control command.
  */
struct strpeek {
	struct strbuf ctlbuf;	/**< Control part STREAMS buffer. */
	struct strbuf databuf;	/**< Data part STREAMS buffer. */
	long flags;		/**< getmsg(2) flags. */
};

/**
  * STREAMS file descriptor insert structure.
  * For use as the argument to the #I_FDINSERT input-output control command.
  */
struct strfdinsert {
	struct strbuf ctlbuf;	/**< Control part for putmsg(2). */
	struct strbuf databuf;	/**< Data part for putmsg(2). */
	long flags;		/**< Flags for putmsg(2). */
	int fildes;		/**< File descriptor to insert (Mac OT has long here). */
	int offset;		/**< Offset within control part for insertion. */
};

/**
  * STREAMS input-output control structure.
  * For use as the argument to the #I_STR input-output control command.
  */
struct strioctl {
	int ic_cmd;		/**< Command to perform. */
	int ic_timout;		/**< ioctl(2) timeout period. */
	int ic_len;		/**< Size of data buffer. */
	char *ic_dp;		/**< Pointer to data buffer. */
};

/**
  * STREAMS receive filed descriptor structure.
  * For use as the argument to the #I_RECVFD input-output control command.
  */
struct strrecvfd {
	int fd;			/**< File descriptor. */
	uid_t uid;		/**< User id. */
	gid_t gid;		/**< Group id. */
	char fill[8];		/**< UnixWare/Solaris compatibility. */
};

/**
  * STREAMS module name structure.
  * For use for the elements of the array pointed to by the struct str_list structure.
  */
struct str_mlist {
	char l_name[FMNAMESZ + 1];	/**< Module name. */
};

/**
  * STREAMS module list structure.
  * For use as the argumnet to the #I_LIST input-output control command.
  */
struct str_list {
	int sl_nmods;			/**< Number of modules names in list. */
	struct str_mlist *sl_modlist;	/**< List (array) of module names. */
};

/**
  * STREAMS signal set structure.
  * For use as the argument to the #I_EGETSIG and #I_ESETSIG input-output control commands.
  */
struct strsigset {
	pid_t ss_pid;		/**< Process id (group id if negative). */
	long ss_events;		/**< STREAMS signal events. */
};

/**
  * STREAMS priority message structure.
  * For use as the argument to the #I_GETPMSG or #I_PUTPMSG input-output control
  * commands.  These input-output control commands emulate getmsg(), getpmsg(),
  * putmsg() and putpmsg().  This structure matches the Mac OT structure of the
  * same name (and purpose).
  */
struct strpmsg {
	struct strbuf ctlbuf;	/**< ctlbuf argumnet. */
	struct strbuf databuf;	/**< databuf argumnet. */
	int band;		/**< bandp or band argument. */
	int flags;		/**< flagsp or flags argument (long under Mac OT). */
};

#if 0
/* this approach might be better (not)! */
struct strgetpmsg {
	struct strbuf *ctlptr;
	struct strbuf *dataptr;
	int *bandp;
	int *flagsp;
};

struct strputpmsg {
	struct strbuf *ctlptr;
	struct strbuf *dataptr;
	int band;
	int flags;
};
#endif

/**
  * @name Special Read/Write Length
  *
  * This is a special read/write length for using the LiS-style read-write
  * interface for getmsg(), getpmsg(), putmsg() and putpmsg().  It works ok for
  * older systems that do not have ioctl_unlocked(), also for those that do not
  * have ioctl_compat().
  * 
  * 2.6.8 thinks this stupid number is legitimate, but doesn't have
  * unlocked_ioctl upgrade your kernel please!
  * 
  * @{
  */
#ifdef __LP64__
#define LFS_GETMSG_PUTMSG_ULEN	(0xedbca987edbca988UL)
#else
//#define LFS_GETMSG_PUTMSG_ULEN        (-0x12345678UL)
#define LFS_GETMSG_PUTMSG_ULEN	(0xedbca988UL)
#endif
/** @} */

/** @} */

#endif				/* __SYS_STREAMS_STROPTS_H__ */

// vim: ft=cpp com=sr\:/**,mb\:\ *,eb\:\ */,sr\:/*,mb\:*,eb\:*/,b\:TRANS
