/*
 *  sys/LiS/tpicommon.h
 *
 *  Copyright (C) 1998 Ole Husgaard <sparre@login.dknet.dk>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 *  MA 02139, USA.
 *
 *
 *  This header file contains some declarations that are common to both
 *  sys/tihdr.h and xti.h (or sys/tiuser.h if you are a bit old-fashioned).
 *
 *  You should never include this file directly.
 *
 */

#ident "@(#) LiS tpicommon.h 2.2 8/30/99 15:35:38 "

#ifndef _LIS_TPICOMMON_H
#define _LIS_TPICOMMON_H

typedef long t_scalar_t;
typedef unsigned long t_uscalar_t;

/*
 *  Transport service error numbers
 */
#define TBADADDR	 1 /* Bad address format			*/
#define TBADOPT		 2 /* Bad options format			*/
#define TACCES		 3 /* Bad permissions				*/
#define TBADF		 4 /* Bad file descriptor			*/
#define TNOADDR		 5 /* Unable to allocate an address		*/
#define TOUTSTATE	 6 /* Would place interface out of state	*/
#define TBADSEQ		 7 /* Bad call sequence number			*/
#define TSYSERR		 8 /* System error				*/
#define TLOOK		 9 /* Has to t_look() for event			*/
#define TBADDATA	10 /* Bad amount of data			*/
#define TBUFOVFLW	11 /* Buffer was too small			*/
#define TFLOW		12 /* Would block due to flow control		*/
#define TNODATA		13 /* No data indication			*/
#define TNODIS		14 /* No disconnect indication			*/
#define TNOUDERR	15 /* No unitdata error indication		*/
#define TBADFLAG	16 /* Bad flags					*/
#define TNOREL		17 /* No orderly release indication		*/
#define TNOTSUPPORT	18 /* Not supported				*/
#define TSTATECHNG	19 /* State is currently changing		*/
#define TNOSTRUCTYPE	20 /* Structure type not supported		*/
#define TBADNAME	21 /* Bad transport provider name		*/
#define TBADQLEN	22 /* Listener queue length limit is zero	*/
#define TADDRBUSY	23 /* Address already in use			*/
#define TINDOUT		24 /* Outstanding connect indications		*/
#define TPROVMISMATCH	25 /* Not same transport provider		*/
#define TRESQLEN	26 /* Connection acceptor has qlen > 0		*/
#define TRESADDR	27 /* Conn. acceptor bound to different address	*/
#define TQFULL		28 /* Connection indicator queue is full	*/
#define TPROTO		29 /* Protocol error				*/

/*
 *  Transport service types
 */
#define T_COTS		1	/* Connection oriented transport service */
#define T_COTS_ORD	2	/* COTS with orderly release		 */
#define T_CLTS		3	/* Connectionless transport service	 */

/*
 *  Options management
 */
#define T_NEGOTIATE	0x0004	/* Negotiate options			*/
#define T_CHECK		0x0008	/* Check options			*/
#define T_DEFAULT	0x0010	/* Get default options			*/
#define T_CURRENT	0x0080	/* Get current options			*/

#define T_SUCCESS	0x0020	/* Succes				*/
#define T_FAILURE	0x0040	/* Failure				*/
#define T_PARTSUCCESS	0x0100	/* Partial success		 	*/
#define T_READONLY	0x0200	/* Option is read only			*/
#define T_NOTSUPPORT	0x0400	/* Option is not supported		*/

#define T_ALLOPT	0	/* All options at a level		*/

struct t_opthdr {
	t_uscalar_t len;	/* Option length, incl. header		*/
	t_uscalar_t level;	/* Option level				*/
	t_uscalar_t name;	/* Option name				*/
	t_uscalar_t status;	/* Negotiation result			*/
};

#define _T_ALIGN_SIZE sizeof(t_uscalar_t)

#define _T_ALIGN_OFS(p, o)						\
	((char *)(((unsigned long)(p) - (o) + _T_ALIGN_SIZE - 1)	\
		    & ~(_T_ALIGN_SIZE - 1)) + (o))

#define _T_ALIGN_OFFSET(p)						\
	((unsigned long)(p) & (_T_ALIGN_SIZE - 1))

#define _T_ALIGN_OFS_OFS(p, l, o) _T_ALIGN_OFS((char *)(p) + l, (o))

#define _T_OPT_FIRSTHDR_OFS(b, l, o)					\
	((struct t_opthdr *)(						\
	  (_T_ALIGN_OFS((b), (o)) + sizeof(struct t_opthdr)		\
	   <= (char *)(b) + (l)) ?					\
	   _T_ALIGN_OFS((b), (o)) : NULL))

#define _T_OPT_NEXTHDR_OFS(b, l, p, o)					\
	((struct t_opthdr *)(						\
	  (_T_ALIGN_OFS_OFS((p), (p)->len + sizeof(struct t_opthdr), (o)) \
	   <= ((char *)(b) + (l))) ?					\
	  _T_ALIGN_OFS_OFS((p), (p)->len, (o)) : NULL			\
	))

#define _T_OPT_DATA_OFS(p, o)						\
	((unsigned char *)(_T_ALIGN_OFS((struct t_opthdr *)(p)+1, (o))))

#define T_OPT_FIRSTHDR(b)   _T_OPT_FIRSTHDR_OFS((b)->buf, (b)->len, 0)
#define T_OPT_NEXTHDR(b, p) _T_OPT_NEXTHDR_OFS((b)->buf, (b)->len, p, 0)
#define T_OPT_DATA(p)       _T_OPT_DATA_OFS((p), 0)

/*
 *  General purpose constants
 */
#define T_YES		1	/* Yes/on/true				*/
#define T_NO		0	/* No/off/false				*/
#define T_INFINITE	-1	/* No limit				*/
#define T_INVALID	-2	/* No sense				*/
#define T_SNDZERO	1	/* Must match <sys/stropts.h>: SNDZERO	*/

#endif /* _LIS_TPICOMMON_H */
