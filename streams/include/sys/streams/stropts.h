/*****************************************************************************

 @(#) $Id: stropts.h,v 0.9.2.25 2006/07/13 12:55:48 brian Exp $

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

 Last Modified $Date: 2006/07/13 12:55:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stropts.h,v $
 Revision 0.9.2.25  2006/07/13 12:55:48  brian
 - updated documentation for sk_buffs

 Revision 0.9.2.24  2006/03/03 10:57:11  brian
 - 32-bit compatibility support, updates for release

 Revision 0.9.2.23  2006/02/20 10:59:20  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ifndef __SYS_STREAMS_STROPTS_H__
#define __SYS_STREAMS_STROPTS_H__

#ident "@(#) $RCSfile: stropts.h,v $ $Name:  $($Revision: 0.9.2.25 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/**
 * @file
 * This file defines the interface to a STREAMS(9) character device special
 * file.
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
#define I_NREAD		(__SID | 1)
#define I_PUSH		(__SID | 2)
#define I_POP		(__SID | 3)
#define I_LOOK		(__SID | 4)
#define I_FLUSH		(__SID | 5)
#define I_SRDOPT	(__SID | 6)
#define I_GRDOPT	(__SID | 7)
#define I_STR		(__SID | 8)
#define I_SETSIG	(__SID | 9)
#define I_GETSIG	(__SID |10)
#define I_FIND		(__SID |11)
#define I_LINK		(__SID |12)
#define I_UNLINK	(__SID |13)
#define I_RECVFD	(__SID |14)	/* non-EFT definition */
#define I_PEEK		(__SID |15)
#define I_FDINSERT	(__SID |16)
#define I_SENDFD	(__SID |17)
#define I_E_RECVFD	(__SID |18)	/* Unixware: EFT definition */
#define I_SWROPT	(__SID |19)
#define I_GWROPT	(__SID |20)
#define I_LIST		(__SID |21)
#define I_PLINK		(__SID |22)
#define I_PUNLINK	(__SID |23)

#define	I_SETEV		(__SID |24)	/* Solaris (obsolete) */
#define	I_GETEV		(__SID |25)	/* Solaris (obsolete) */
#define	I_STREV		(__SID |26)	/* Solaris (obsolete) */
#define	I_UNSTREV	(__SID |27)	/* Solaris (obsolete) */

#define I_FLUSHBAND	(__SID |28)
#define I_CKBAND	(__SID |29)
#define I_GETBAND	(__SID |30)
#define I_ATMARK	(__SID |31)
#define I_SETCLTIME	(__SID |32)
#define I_GETCLTIME	(__SID |33)
#define I_CANPUT	(__SID |34)

/* from here the numbering gets implementation specific */

#define I_SERROPT	(__SID |35)	/* Solaris */
#define I_GERROPT	(__SID |36)	/* Solaris */
#define I_ANCHOR	(__SID |37)	/* Solaris */

#define I_S_RECVFD	(__SID |35)	/* UnixWare */
#define I_STATS		(__SID |36)	/* UnixWare */
#define I_BIGPIPE	(__SID |37)	/* UnixWare */
#define I_GETTP		(__SID |38)	/* UnixWare */

#define I_GETMSG	(__SID |40)	/* HP-UX, OSF */
#define I_PUTMSG	(__SID |41)	/* HP-UX, OSF */
#define I_GETPMSG	(__SID |42)	/* HP-UX, OSF, LiS 251, Mac OT 40 */
#define I_PUTPMSG	(__SID |43)	/* HP-UX, OSF, LiS 250, Mac OT 41 */
#define I_PIPE		(__SID |44)	/* HP-UX, OSF, LiS 254/243, Mac OT 49 */
#define I_FIFO		(__SID |45)	/* HP-UX, OSF, Mac OT 51 */
#define I_ISASTREAM	(__SID |46)

#define I_AUTOPUSH	(__SID |48)	/* Mac OT */
#define I_HEAP_REPORT	(__SID |50)	/* Mac OT */

#define I_FATTACH	(__SID |52)	/* LiS (252 then 244) */
#define I_FDETACH	(__SID |53)	/* LiS (253 then 245) */

#define INFTIM		(-1UL)

#define FMNAMESZ	8	/* compatibility w/UnixWare/Solaris.  */

#define FLUSHR		(1<<0)	/* Flush read queues.  */
#define FLUSHW		(1<<1)	/* Flush write queues.  */
#define FLUSHRW		(FLUSHR|FLUSHW)	/* Flush read and write queues.  */
#define FLUSHBAND	(1<<2)	/* Flush only specified band.  */

#define MAPINOK		(1<<0)	/* map instead of copyin */
#define NOMAPIN		(1<<1)	/* normal copyin */
#define REMAPOK		(1<<2)	/* page flip instead of copyout */
#define NOREMAP		(1<<3)	/* normal copyout */

#ifdef __KERNEL__
#define S_INPUT_BIT	0
#define S_HIPRI_BIT	1
#define S_OUTPUT_BIT	2
#define S_MSG_BIT	3
#define S_ERROR_BIT	4
#define S_HANGUP_BIT	5
#define S_RDNORM_BIT	6
#define S_WRNORM_BIT	S_OUTPUT_BIT
#define S_RDBAND_BIT	7
#define S_WRBAND_BIT	8
#define S_BANDURG_BIT	9
#endif

/* SIGPOLL bits */
#define S_INPUT		(1<<0)
#define S_HIPRI		(1<<1)
#define S_OUTPUT	(1<<2)
#define S_MSG		(1<<3)
#define S_ERROR		(1<<4)
#define S_HANGUP	(1<<5)
#define S_RDNORM	(1<<6)
#define S_WRNORM	S_OUTPUT
#define S_RDBAND	(1<<7)
#define S_WRBAND	(1<<8)
#define S_BANDURG	(1<<9)
#define S_ALL		(S_INPUT|S_HIPRI|S_OUTPUT|S_MSG|S_ERROR|S_HANGUP|S_RDNORM|S_WRNORM|S_RDBAND|S_WRBAND|S_BANDURG)

#define RS_HIPRI	(1<<0)	/* only read hi priority messages */
#define STRUIO_POSTPONE	(1<<3)	/* Solaris */
#define STRUIO_MAPIN	(1<<4)	/* Solaris */
#define RS_EXDATA	(1<<5)	/* Mac OT */
#define RS_ALLOWAGAIN	(1<<6)	/* Mac OT */
#define RS_DELIMITMSG	(1<<7)	/* Mac OT */

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

#define RERRNORM	(0<<0)
#define RERRNONPERSIST	(1<<0)
#define WERRNORM	(0<<1)
#define WERRNONPERSIST	(1<<1)

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

#endif				/* __SYS_STREAMS_STROPTS_H__ */
