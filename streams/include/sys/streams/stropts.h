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
 * @{
 */

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

/**
 * @name STREAMS Input-Output Controls
 * These are input-output control command definitions for STREAMS character
 * device special files.
 * @addtogroup streamio STREAMS Input-Output Controls
 * @{
 */
#define I_NREAD		(__SID | 1)
#define I_PUSH		(__SID | 2)
#define I_POP		(__SID | 3)
#define I_LOOK		(__SID | 4)
#define I_FLUSH		(__SID | 5)	/**< Flush a Stream. */
#define I_SRDOPT	(__SID | 6)	/**< Set read options for a Stream. */
#define I_GRDOPT	(__SID | 7)	/**< Get read options for a Stream. */
#define I_STR		(__SID | 8)	/**< Formulate a STREAMS ioctl(). */
#define I_SETSIG	(__SID | 9)	/**< Set signals for a Stream. */
#define I_GETSIG	(__SID |10)	/**< Get signals for a Stream. */
#define I_FIND		(__SID |11)	/**< Find a module in a Stream. */
#define I_LINK		(__SID |12)	/**< Temporarily link a Stream under a mux. */
#define I_UNLINK	(__SID |13)	/**< Temporarily unlink a Stream from a mux. */
#define I_RECVFD	(__SID |14)	/**< Receive a file descriptor. Non-EFT definition. */
#define I_PEEK		(__SID |15)
#define I_FDINSERT	(__SID |16)
#define I_SENDFD	(__SID |17)
#define I_E_RECVFD	(__SID |18)	/**< Receive a file descriptor. Unixware: EFT definition. */
#define I_SWROPT	(__SID |19)	/**< Set write options for a Stream. */
#define I_GWROPT	(__SID |20)	/**< Get write options for a Stream. */
#define I_LIST		(__SID |21)	/**< List modules on a Stream. */
#define I_PLINK		(__SID |22)	/**< Permanently link a Stream under a mux. */
#define I_PUNLINK	(__SID |23)	/**< Permanently unlink a Stream from a mux. */

#define	I_SETEV		(__SID |24)	/**< Set an event (Solaris, @e obsolete). */
#define	I_GETEV		(__SID |25)	/**< Get an event (Solaris, @e obsolete). */
#define	I_STREV		(__SID |26)	/**< Create Stream event (Solaris, @e obsolete). */
#define	I_UNSTREV	(__SID |27)	/**< Destroy Stream event (Solaris, @e obsolete). */

#define I_FLUSHBAND	(__SID |28)
#define I_CKBAND	(__SID |29)
#define I_GETBAND	(__SID |30)
#define I_ATMARK	(__SID |31)
#define I_SETCLTIME	(__SID |32)
#define I_GETCLTIME	(__SID |33)
#define I_CANPUT	(__SID |34)
/** @} */

/**
 * @name Non-Standard STREAMS Input-Output Controls
 * The numbering of ioctl() above #I_CANPUT (34) is implementation specific.
 * @addtogroup streamio STREAMS Input-Output Controls
 * @{
 *    @def I_SERROPT
 *	   Set error options (Solaris).  This is a Solaris specific option.
 *	   Argument values are #RERRNORM, #RERRNONPERSIST, #WERRNORM and
 *	   #WERRNONPERSIST.
 */
#define I_SERROPT	(__SID |35)	/**< Set error options (Solaris). */
#define I_GERROPT	(__SID |36)	/**< Get error options (Solaris). */
#define I_ANCHOR	(__SID |37)	/**< Set an module anchor (Solaris). */

#define I_S_RECVFD	(__SID |35)	/**< Secure receive file descriptor (UnixWare). */
#define I_STATS		(__SID |36)	/**< Get statistics (UnixWare). */
#define I_BIGPIPE	(__SID |37)	/**< Get large pipe (UnixWare). */
#define I_GETTP		(__SID |38)	/**< Get transport provider (UnixWare). */

#define I_GETMSG	(__SID |40)	/**< getmsg() emulation (HP-UX, OSF). */
#define I_PUTMSG	(__SID |41)	/**< putmsg() emulation (HP-UX, OSF). */
#define I_GETPMSG	(__SID |42)	/**< getpmsg() emulation (HP-UX, OSF, LiS: @c 251, Mac OT: @c 40). */
#define I_PUTPMSG	(__SID |43)	/**< putpmsg() emulation (HP-UX, OSF, LiS: @c 250, Mac OT: @c 41). */
#define I_PIPE		(__SID |44)	/**< pipe() emulation (HP-UX, OSF, LiS: @c 254/243, Mac OT: @c 49). */
#define I_FIFO		(__SID |45)	/**< FIFO emulation (HP-UX, OSF, Mac OT: @c 51). */
#define I_ISASTREAM	(__SID |46)	/**< isastream() emulation */

#define I_AUTOPUSH	(__SID |48)	/**< sad(4) emulation (Mac OT). */
#define I_HEAP_REPORT	(__SID |50)	/**< Heap debugging (Mac OT). */

#define I_FATTACH	(__SID |52)	/* fattach() emulation (LiS: @c 252 then @c 244). */
#define I_FDETACH	(__SID |53)	/* fdefatch() emulation (LiS: @c 253 then @c 245). */
/** @} */

#define INFTIM		(-1UL)		/**< Default streamio(7) timeout. */

#define FMNAMESZ	8	/**< Max module name size (compatible w/ UnixWare and Solaris). */

#define FLUSHR		(1<<0)	/**< Flush read queues.  */
#define FLUSHW		(1<<1)	/**< Flush write queues.  */
#define FLUSHRW		(FLUSHR|FLUSHW)	/**< Flush read and write queues.  */
#define FLUSHBAND	(1<<2)	/**< Flush only specified band.  */

#define MAPINOK		(1<<0)	/**< mapin(9) instead of copyin(9). */
#define NOMAPIN		(1<<1)	/**< Normal copyin(9). */
#define REMAPOK		(1<<2)	/**< Page flip instead of copyout(9). */
#define NOREMAP		(1<<3)	/**< Normal copyout(9). */

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
 * Bits for use with #I_GETSIG and #I_SETSIG.  These bits are used in the
 * argument to #I_SETSIG and returned from #I_GETSIG input-output controls.
 * They correspond to poll() bits.
 * @addtogroup streamio STREAMS Input-Output Controls
 * @{
 */
/* SIGPOLL bits */
#define S_INPUT		(1<<0)		/**< Signal on input. */
#define S_HIPRI		(1<<1)		/**< Signal on high priority. */
#define S_OUTPUT	(1<<2)		/**< Signal on output. */
#define S_MSG		(1<<3)		/**< Signal on #M_SIG message. */
#define S_ERROR		(1<<4)		/**< Signal on #M_ERROR message. */
#define S_HANGUP	(1<<5)		/**< Signal on #M_HANGUP message. */
#define S_RDNORM	(1<<6)		/**< Signal on normal read. */
#define S_WRNORM	S_OUTPUT	/**< Signal on normal write. */
#define S_RDBAND	(1<<7)		/**< Signal on banded read. */
#define S_WRBAND	(1<<8)		/**< Signal on banded write. */
#define S_BANDURG	(1<<9)		/**< Signal on band urgent (ATMARK). */
#define S_ALL		(S_INPUT|S_HIPRI|S_OUTPUT|S_MSG|S_ERROR|S_HANGUP|S_RDNORM|S_WRNORM|S_RDBAND|S_WRBAND|S_BANDURG)
/**< Bitmask of all signals */
/** @} */

/**
 * @name Get Message Flags
 * Flags for use with getmsg().
 * @addtogroup stropts STREAMS Options
 * @{
 * @def RS_HIPRI
 *	Only get high priority messages.  This is the standard flag to getmsg()
 *	used to request retrieval of high priority messages only.
 * @def RS_EXDATA
 *	Only get expedited data messages (Mac OT).  This option is Mac
 *	OpenTransport specific and non-standard.  It requests the retrieval of
 *	expedited data messages only.
 */
#define RS_HIPRI	(1<<0)		/**< Only get high priority messages. */
#define STRUIO_POSTPONE	(1<<3)		/**< (Solaris). */
#define STRUIO_MAPIN	(1<<4)		/**< (Solaris). */
#define RS_EXDATA	(1<<5)		/**< (Mac OT). */
#define RS_ALLOWAGAIN	(1<<6)		/**< (Mac OT). */
#define RS_DELIMITMSG	(1<<7)		/**< (Mac OT). */
/** @} */

/* Mac OT gets these wrong */
#define RNORM		(0)	/* byte-stream mode */
#define RMSGD		(1<<0)	/* message discard mode */
#define RMSGN		(1<<1)	/* message non-discard mode */
#define RMODEMASK	(RNORM|RMSGD|RMSGN)	/* mode mask */
#define RPROTDAT	(1<<2)	/* proto part as data */
#define RPROTDIS	(1<<3)	/* proto part discarded */
#define RPROTNORM	(1<<4)	/* proto part fail read EBADMSG */
#define RPROTMASK	(RPROTDAT|RPROTDIS|RPROTNORM)	/* proto bit mask */
/* OSF also adds RFILL and RPROTCOMPRESS but these are incompatible with i386 ABI */

#define SNDZERO		(1<<0)
#define SNDPIPE		(1<<1)
#define SNDHOLD		(1<<2)

/**
 * @name Stream Error Options
 * Error options for use with #I_GERROPT and #I_SERROPT.  This is @e Solaris
 * specific behaviour.
 * @addtogroup streamio STREAMS Input-Output Controls
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
#define RERRNORM	(0<<0) /**< Normal SVR4 error behaviour on read. */
#define RERRNONPERSIST	(1<<0) /**< Non-persistent error behaviour on read. */
#define WERRNORM	(0<<1) /**< Normal SVR4 error behaviour on write. */
#define WERRNONPERSIST	(1<<1) /**< Non-persistent error behaviour on write. */
/** @} */

#define ANYMARK		(1<<0)
#define LASTMARK	(1<<1)

#define MUXID_ALL	(-1)

#define MSG_HIPRI	(1<<0)
#define MSG_ANY		(1<<1)
#define MSG_BAND	(1<<2)
#define MSG_DISCARD	(1<<3)	/* UnixWare */
#define MSG_PEEKIOCTL	(1<<4)	/* UnixWare */

#define MORECTL		1
#define MOREDATA	2

struct bandinfo {
	unsigned char bi_pri;
	int bi_flag;
};

struct strbuf {
	int maxlen;
	int len;
	char *buf;
};

struct strpeek {
	struct strbuf ctlbuf;
	struct strbuf databuf;
	long flags;
};

struct strfdinsert {
	struct strbuf ctlbuf;		/* ctrl part for putmsg(2) */
	struct strbuf databuf;		/* data part for putmsg(2) */
	long flags;			/* flags for putmsg(2) */
	int fildes;			/* file descriptor to insert *//* Mac OT has long here */
	int offset;			/* offset within control part for insertion */
};

struct strioctl {
	int ic_cmd;			/* command to perform */
	int ic_timout;			/* ioctl timeout period */
	int ic_len;			/* size of data buffer */
	char *ic_dp;			/* addr of data buffer */
};

struct strrecvfd {
	int fd;				/* file descriptor */
	uid_t uid;			/* user id */
	gid_t gid;			/* group id */
	char fill[8];			/* UnixWare/Solaris compatibility */
};

struct str_mlist {
	char l_name[FMNAMESZ + 1];
};

struct str_list {
	int sl_nmods;
	struct str_mlist *sl_modlist;
};

struct strsigset {
	pid_t ss_pid;
	long ss_events;
};

/* for ioctl emulation of getmsg() getpmsg() putmsg() putpmsg() matches Mac OT */
struct strpmsg {
	struct strbuf ctlbuf;
	struct strbuf databuf;
	int band;
	int flags;			/* actually long for Mac OT */
};

#if 0
/* this approach might be better */
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

/* 2.6.8 thinks this stupid number is legitimate, but doesn't have unlocked_ioctl upgrade your
   kernel please! */
#ifdef __LP64__
#define LFS_GETMSG_PUTMSG_ULEN	(0xedbca987edbca988UL)
#else
//#define LFS_GETMSG_PUTMSG_ULEN	(-0x12345678UL)
#define LFS_GETMSG_PUTMSG_ULEN	(0xedbca988UL)
#endif

/** @} */

#endif				/* __SYS_STREAMS_STROPTS_H__ */
