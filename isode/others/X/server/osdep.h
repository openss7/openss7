/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

#ifndef __X_SERVER_OSDEP_H__
#define __X_SERVER_OSDEP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* XConsortium: osdep.h,v 1.15 88/09/06 15:50:47 jim Exp */

#ifndef NULL
#define NULL 0
#endif

#define BUFSIZE 4096
#define BUFWATERMARK 8192
#define MAXBUFSIZE (1 << 18)
#if (NOFILE <= 128)		/* 128 is value of MAXCLIENTS in dix layer */
#define MAXSOCKS (NOFILE - 1)
#else
#define MAXSOCKS 128
#endif
#define mskcnt ((MAXSOCKS + 31) / 32)	/* size of bit array */

#if (mskcnt==1)
#define BITMASK(i) (1 << (i))
#define MASKIDX(i) 0
#endif
#if (mskcnt>1)
#define BITMASK(i) (1 << ((i) & 31))
#define MASKIDX(i) ((i) >> 5)
#endif

#define MASKWORD(buf, i) buf[MASKIDX(i)]
#define BITSET(buf, i) MASKWORD(buf, i) |= BITMASK(i)
#define BITCLEAR(buf, i) MASKWORD(buf, i) &= ~BITMASK(i)
#define GETBIT(buf, i) (MASKWORD(buf, i) & BITMASK(i))

#if (mskcnt==1)
#define COPYBITS(src, dst) dst[0] = src[0]
#define CLEARBITS(buf) buf[0] = 0
#define MASKANDSETBITS(dst, b1, b2) dst[0] = (b1[0] & b2[0])
#define ORBITS(dst, b1, b2) dst[0] = (b1[0] | b2[0])
#define UNSETBITS(dst, b1) (dst[0] &= ~b1[0])
#define ANYSET(src) (src[0])
#endif
#if (mskcnt==2)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] & b2[0]);\
		      dst[1] = (b1[1] & b2[1])
#define ORBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] | b2[0]);\
		      dst[1] = (b1[1] | b2[1])
#define UNSETBITS(dst, b1) \
                      dst[0] &= ~b1[0]; \
                      dst[1] &= ~b1[1]
#define ANYSET(src) (src[0] || src[1])
#endif
#if (mskcnt==3)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0; buf[2] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] & b2[0]);\
		      dst[1] = (b1[1] & b2[1]);\
		      dst[2] = (b1[2] & b2[2])
#define ORBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] | b2[0]);\
		      dst[1] = (b1[1] | b2[1]);\
		      dst[2] = (b1[2] | b2[2])
#define UNSETBITS(dst, b1) \
                      dst[0] &= ~b1[0]; \
                      dst[1] &= ~b1[1]; \
                      dst[2] &= ~b1[2]
#define ANYSET(src) (src[0] || src[1] || src[2])
#endif
#if (mskcnt==4)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];\
		      dst[3] = src[3]
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
                      dst[0] = (b1[0] & b2[0]);\
                      dst[1] = (b1[1] & b2[1]);\
                      dst[2] = (b1[2] & b2[2]);\
                      dst[3] = (b1[3] & b2[3])
#define ORBITS(dst, b1, b2)  \
                      dst[0] = (b1[0] | b2[0]);\
                      dst[1] = (b1[1] | b2[1]);\
                      dst[2] = (b1[2] | b2[2]);\
                      dst[3] = (b1[3] | b2[3])
#define UNSETBITS(dst, b1) \
                      dst[0] &= ~b1[0]; \
                      dst[1] &= ~b1[1]; \
                      dst[2] &= ~b1[2]; \
                      dst[3] &= ~b1[3]
#define ANYSET(src) (src[0] || src[1] || src[2] || src[3])
#endif

#if (mskcnt>4)
#define COPYBITS(src, dst) bcopy((caddr_t) src, (caddr_t) dst,\
				 mskcnt*sizeof(long))
#define CLEARBITS(buf) bzero((caddr_t) buf, mskcnt*sizeof(long))
#define MASKANDSETBITS(dst, b1, b2)  \
		      { int cri;			\
			for (cri=0; cri<mskcnt; cri++)	\
		          dst[cri] = (b1[cri] & b2[cri]) }
#define ORBITS(dst, b1, b2)  \
		      { int cri;			\
		      for (cri=0; cri<mskcnt; cri++)	\
		          dst[cri] = (b1[cri] | b2[cri]) }
#define UNSETBITS(dst, b1) \
		      { int cri;			\
		      for (cri=0; cri<mskcnt; cri++)	\
		          dst[cri] &= ~b1[cri];  }
/*
 * If mskcnt>4, then ANYSET is a routine defined in WaitFor.c.
 *
 * #define ANYSET(src) (src[0] || src[1] || src[2] || src[3] || src[4] ...)
 */
#endif

typedef struct _connectionInput {
	int used;			/* is this client connected */
	char *buffer;			/* contains current client input */
	char *bufptr;			/* pointer to current start of data */
	int bufcnt;			/* count of bytes in buffer */
	int lenLastReq;
	int size;
} ConnectionInput;

typedef struct _osComm {
	int fd;
	unsigned char *buf;
	int bufsize;
	int count;
} OsCommRec, *OsCommPtr;

void Notice();

#ifdef ISOCONN
#ifdef ISODEBUG
extern int isodexbug;
#endif				/* ISODEBUG */
/*
 * For want of a better place to put these for the moment...
 * 
 * Should really appear in system headers eventuallement
 */

#define FamilyIso 3

#define UNIX_IO 0
#define ISODE_IO 1

/*
 * The following could all be optimised by putting it in the 
 * OsCommRec
 * so we would not have to double array index for every IO...
 * 
 * but i'm not sure that that rec is available in all IO call places
 */
extern int fd2family[];

extern int (*acceptfn[]) ();
extern int (*getpeerfn[]) ();
extern int (*readfn[]) ();
extern int (*writefn[]) ();
extern int (*readvfn[]) ();
extern int (*writevfn[]) ();
extern int (*closefn[]) ();

#define SAccept(fd, a, b) \
acceptfn[fd2family[fd]]((fd), (a), (b))

#define SGetPeerName(fd, ptr, len) \
getpeerfn[fd2family[(fd)]]((fd), (ptr), (len))

#define SRead(fd, data, size, dum) \
readfn[fd2family[(fd)]]((fd), (data), (size), (dum))

#define SWrite(fd, data, size) \
writefn[fd2family[(fd)]]((fd), (data), (size))

#define SWritev(fd, iov, iovcnt) \
writevfn[fd2family[(fd)]]((fd), (iov), (iovcnt))

#define SClose(fd) \
closefn[fd2family[(fd)]]((fd))

#endif				/* ISOCONN */

#endif				/* __X_SERVER_OSDEP_H__ */
