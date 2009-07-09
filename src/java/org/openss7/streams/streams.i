/*****************************************************************************

 @(#) $RCSfile: streams.i,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2009-07-09 12:05:15 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2009-07-09 12:05:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: streams.i,v $
 Revision 1.1.2.2  2009-07-09 12:05:15  brian
 - updated implementation

 Revision 1.1.2.1  2009-06-21 11:36:48  brian
 - added files to new distro

 *****************************************************************************/

%module Streams
%{
#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#include <sys/stropts.h>
#include <sys/poll.h>
#include <fcntl.h>
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifndef __EXCEPTIONS
#define __EXCEPTIONS 1
#endif

#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
%}

%include "typemaps.i"
%include "arrays_java.i"
%include "various.i"

%javaconst(1);

#include "stropts.h"

#define __SID		0x5300

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
#define I_PLINK			(__SID |22)	/* R3(19)   *//**< Permanently link a Stream under a mux. */
#define I_PUNLINK		(__SID |23)	/* R3(20)   *//**< Permanently unlink a Stream from a mux. */
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
#define I_ANCHOR		(__SID |24)	/* S(37)    *//**< Set an module anchor. */
#define I_SERROPT		(__SID |35)	/* Solaris  *//**< Set error options. */
#define I_GERROPT		(__SID |36)	/* Solaris  *//**< Get error options. */
#define I_ESETSIG		(__SID |37)	/* Solaris  *//**< Extended set signals. */
#define I_EGETSIG		(__SID |38)	/* Solaris  *//**< Exterded get signals. */
#define I_GETMSG		(__SID |40)	/* HPUX,OSF *//**< getmsg() syscall emulation. */
#define I_PUTMSG		(__SID |41)	/* HPUX,OSF *//**< putmsg() syscall emulation. */
#define I_GETPMSG		(__SID |42)	/* HO,M(40) *//**< getpmsg() syscall emulation. */
#define I_PUTPMSG		(__SID |43)	/* HO,M(41) *//**< putpmsg() syscall emulation. */
#define I_PIPE			(__SID |44)	/* HO,M(49) *//**< pipe() syscall emulation. */
#define I_FIFO			(__SID |45)	/* HO,M(51) *//**< FIFO syscall emulation. */
#define I_ISASTREAM		(__SID |46)	/* HO(14)   *//**< isastream() syscall emulation. */
#define I_AUTOPUSH		(__SID |48)	/* Mac OT   *//**< sad(4) syscall emulation. */
#define I_HEAP_REPORT		(__SID |50)	/* Mac OT   *//**< Heap debugging. */
#define I_FATTACH		(__SID |52)	/* M,L(244) *//**< fattach() syscall emulation. */
#define I_FDETACH		(__SID |53)	/* M,L(245) *//**< fdefatch() syscall emulation. */
#define I_LIS_GETPMSG		(__SID | 42)	/* L 2.18.4 *//**< getpmsg() syscall emulation. */
#define I_LIS_PUTPMSG		(__SID | 43)	/* L 2.18.4 *//**< putpmsg() syscall emulation. */
#define I_LIS_PIPE		(__SID |243)	/* L 2.18.0 *//**< pipe() syscall emulation. */
#define I_LIS_FATTACH		(__SID |244)	/* L 2.18.0 *//**< fattach() syscall emulation. */
#define I_LIS_FDETACH		(__SID |245)	/* L 2.18.0 *//**< fdetach() syscall emaulation. */

#define INFTIM		(-1)		/**< Default streamio(7) timeout. */
#define FMNAMESZ	8		/**< Max module name size (compatible w/ UnixWare and Solaris). */
#define FLUSHR		(1<<0)		/**< Flush read queues.  */
#define FLUSHW		(1<<1)		/**< Flush write queues.  */
#define FLUSHRW		(FLUSHR|FLUSHW)	/**< Flush read and write queues.  */
#define FLUSHBAND	(1<<2)		/**< Flush only specified band.  */
#define MAPINOK		(1<<0)		/**< mapin(9) instead of copyin(9). */
#define NOMAPIN		(1<<1)		/**< Normal copyin(9). */
#define REMAPOK		(1<<2)		/**< Page flip instead of copyout(9). */
#define NOREMAP		(1<<3)		/**< Normal copyout(9). */
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
#define RS_HIPRI	(1<<0)		/**< Only get high priority messages. */
#define STRUIO_POSTPONE	(1<<3)	/* Solaris */
#define STRUIO_MAPIN	(1<<4)	/* Solaris */
#define RS_EXDATA	(1<<5)	/* Mac OT */
#define RS_ALLOWAGAIN	(1<<6)	/* Mac OT */
#define RS_DELIMITMSG	(1<<7)	/* Mac OT */
#define RNORM		(0)	/**< Byte-stream mode. */
#define RMSGD		(1<<0)	/**< Message discard mode. */
#define RMSGN		(1<<1)	/**< Message non-discard mode. */
#define RFILL		(1<<5)	/**< Byte-stream fill mode. */
#define RMODEMASK	(RNORM|RMSGD|RMSGN|RFILL)	/**< Mode mask. */
#define RPROTDAT	(1<<2)	/**< Proto part as data. */
#define RPROTDIS	(1<<3)	/**< Proto part discarded. */
#define RPROTNORM	(1<<4)	/**< Proto part fail read EBADMSG. */
#define RPROTCOMPRESS	(1<<6)	/**< Proto part compressed. */
#define RPROTMASK	(RPROTDAT|RPROTDIS|RPROTNORM|RPROTCOMPRESS)	/**< Proto bit mask. */
#define SNDZERO		(1<<0)	/**< Permit sending of zero length messages. */
#define SNDPIPE		(1<<1)	/**< Sending on a disconnected pipe send SIGPIPE. */
#define SNDHOLD		(1<<2)	/**< Coallese small messages. */
#define SNDELIM		(1<<3)	/**< Delimit writes with MGSDELIM. */
#define SNDMREAD	(1<<4)	/**< Send M_READ messages. */
#define RERRNORM	(0<<0)	/**< Normal SVR4 error behaviour on read. */
#define RERRNONPERSIST	(1<<0)	/**< Non-persistent error behaviour on read. */
#define WERRNORM	(0<<1)	/**< Normal SVR4 error behaviour on write. */
#define WERRNONPERSIST	(1<<1)	/**< Non-persistent error behaviour on write. */
#define ANYMARK		(1<<0)	/**< Indicate whether at a marked byte. */
#define LASTMARK	(1<<1)	/**< Indicate whether at last marked byte. */
#define MUXID_ALL	(-1)	/**< Unlink all links from under multiplexer. */
#define MSG_HIPRI	(1<<0)	/**< Put or get a high priority message. */
#define MSG_ANY		(1<<1)	/**< Get a message of any band. */
#define MSG_BAND	(1<<2)	/**< Put or get a message of specified band. */
#define MSG_DISCARD	(1<<3)	/* UnixWare */
#define MSG_PEEKIOCTL	(1<<4)	/* UnixWare */
#define MORECTL		1	/**< More control part available. */
#define MOREDATA	2	/**< More data part available. */

struct bandinfo {
	unsigned char bi_pri;	/**< Priority of band to flush. */
	int bi_flag;		/**< Flush flags. */
};
%apply char *BYTE { char *buf }
struct strbuf {
	int maxlen;		/**< Maximum length of buffer for output. */
	int len;		/**< Length of buffer for input. */
	char *buf;		/**< Pointer to the buffer. */
};
struct strpeek {
	struct strbuf ctlbuf;	/**< Control part STREAMS buffer. */
	struct strbuf databuf;	/**< Data part STREAMS buffer. */
	long flags;		/**< getmsg(2) flags. */
};
struct strfdinsert {
	struct strbuf ctlbuf;	/**< Control part for putmsg(2). */
	struct strbuf databuf;	/**< Data part for putmsg(2). */
	long flags;		/**< Flags for putmsg(2). */
	int fildes;		/**< File descriptor to insert (Mac OT has long here). */
	int offset;		/**< Offset within control part for insertion. */
};
%apply char *BYTE { char *ic_dp }
struct strioctl {
	int ic_cmd;		/**< Command to perform. */
	int ic_timout;		/**< ioctl(2) timeout period. */
	int ic_len;		/**< Size of data buffer. */
	char *ic_dp;		/**< Pointer to data buffer. */
};
#if 0
struct strrecvfd {
	int fd;			/**< File descriptor. */
	uid_t uid;		/**< User id. */
	gid_t gid;		/**< Group id. */
	char fill[8];		/**< UnixWare/Solaris compatibility. */
};
#endif
struct str_mlist {
	char l_name[FMNAMESZ + 1];	/**< Module name. */
};
struct str_list {
	int sl_nmods;			/**< Number of modules names in list. */
	struct str_mlist *sl_modlist;	/**< List (array) of module names. */
};
#if 0
struct strsigset {
	pid_t ss_pid;		/**< Process id (group id if negative). */
	long ss_events;		/**< STREAMS signal events. */
};
#endif
struct strpmsg {
	struct strbuf ctlbuf;	/**< ctlbuf argumnet. */
	struct strbuf databuf;	/**< databuf argumnet. */
	int band;		/**< bandp or band argument. */
	int flags;		/**< flagsp or flags argument (long under Mac OT). */
};

extern int ioctl(int fd, unsigned long int request, ...);
extern int isastream(int fd);
extern int getmsg(int fd, struct strbuf *INOUT, struct strbuf *INOUT, int *INOUT);
extern int getpmsg(int fd, struct strbuf *INOUT, struct strbuf *INOUT, int *INOUT, int *INOUT);
extern int putmsg(int fd, const struct strbuf *OUTPUT, const struct strbuf *OUTPUT, int flags);
extern int putpmsg(int fd, const struct strbuf *OUTPUT, const struct strbuf *OUTPUT, int band, int flags);
extern int fattach(int fd, const char *OUTPUT);
extern int fdetach(const char *OUTPUT);
extern int pipe(int fds[2]);

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("openss7StreamsJNI");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}


// vim: ft=c
