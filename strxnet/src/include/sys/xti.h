/*****************************************************************************

 @(#) $Id: xti.h,v 0.9.2.1 2004/04/05 12:39:05 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 2001-2004 OpenSS7 Corporation.  All Rights Reserved.

                                  PUBLIC LICENSE

     This license is provided without fee, provided that the above copy-
     right notice and this public license must be retained on all copies,
     extracts, compilations and derivative works.  Use or distribution of
     this work in a manner that restricts its use except as provided here
     will render this license void.

     The author(s) hereby waive any and all other restrictions in respect
     of their copyright in this software and its associated documentation.
     The authors(s) of this software place in the public domain any novel
     methods or processes which are embodied in this software.

     The author(s) undertook to write it for the sake of the advancement
     of the Arts and Sciences, but it is provided as is, and the author(s)
     will not take any responsibility in it.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/05 12:39:05 $ by $Author: brian $

 *****************************************************************************/

#ifndef _XTI_XTI_H
#define _XTI_XTI_H

#ident "@(#) $RCSfile: xti.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

#ifndef t_scalar_t
/**
 * \ingroup libxti
 * XTI signed scalar type.
 *
 * Strangely enough, t_uscalar_t is defined in stropts.h for SUSv2 and SUSv3
 * systems, however, t_scalar_t is only defined here.
 */
typedef int32_t t_scalar_t;
#define t_scalar_t t_scalar_t
#endif				/* !defined t_scalar_t */

#if !defined _STROPTS_H || defined NEED_T_USCALAR_T
#ifndef t_uscalar_t
/**
 * \ingroup libxti
 * XTI unsigned scalar type.
 *
 * This type is not defined in the XTI Library as it is supposed to be defined
 * in stropts.h according to SUSv2 and SUSv3.
 */
typedef u_int32_t t_uscalar_t;
#define t_uscalar_t t_uscalar_t
#endif				/* !defined t_uscalar_t */
#endif				/* !defined _STROPTS_H || defined NEED_T_USCALAR_T */

/* 
 * The following are the error codes needed by both the kernel
 * level transport providers and the user level library.
 */
#define TBADADDR	 1	/* incorrect addr format */
#define TBADOPT		 2	/* incorrect option format */
#define TACCES		 3	/* incorrect permissions */
#define TBADF		 4	/* illegal transport fd */
#define TNOADDR		 5	/* couldn't allocate addr */
#define TOUTSTATE	 6	/* out of state */
#define TBADSEQ		 7	/* bad call sequence number */
#define TSYSERR		 8	/* system error */
#define TLOOK		 9	/* event requires attention */
#define TBADDATA	10	/* illegal amount of data */
#define TBUFOVFLW	11	/* buffer not large enough */
#define TFLOW		12	/* flow control */
#define TNODATA		13	/* no data */
#define TNODIS		14	/* discon_ind not found on queue */
#define TNOUDERR	15	/* unitdata error not found */
#define TBADFLAG	16	/* bad flags */
#define TNOREL		17	/* no ord rel found on queue */
#define TNOTSUPPORT	18	/* primitive/action not supported */
#define TSTATECHNG	19	/* state is in process of changing */
/* 
 *  The following are XPG3 and up.
 */
#define TNOSTRUCTYPE	20	/* unsupported struct-type requested */
#define TBADNAME	21	/* invalid transport provider name */
#define TBADQLEN	22	/* qlen is zero */
#define TADDRBUSY	23	/* address in use */
/* 
 *  The following are _XOPEN_SOURCE (XPG4 and up).
 */
#if defined _XOPEN_SOURCE || defined __KERNEL__
#define TINDOUT		24	/* outstanding connection indications */
#define TPROVMISMATCH	25	/* transport provider mismatch */
#define TRESQLEN	26	/* resfd specified to accept w/qlen >0 */
#define TRESADDR	27	/* resfd not bound to same addr as fd */
#define TQFULL		28	/* incoming connection queue full */
#define TPROTO		29	/* XTI protocol error */
#endif				/* defined _XOPEN_SOURCE || defined __KERNEL__ */

/* 
 * The following are the events returned from t_look().
 */
#define T_LISTEN	0x0001	/* connection indication received */
#define T_CONNECT	0x0002	/* connection confirmation received */
#define T_DATA		0x0004	/* normal data received */
#define T_EXDATA	0x0008	/* expedited data received */
#define T_DISCONNECT	0x0010	/* disconnection received */
#define T_UDERR		0x0040	/* datagram error indication */
#define T_ORDREL	0x0080	/* orderly release indication */
#define T_GODATA	0x0100	/* sending normal data is again possible */
#define T_GOEXDATA	0x0200	/* sending expedited data is again possible */
#define T_LEAFCHANGE	0x0400	/* status of a leaf has changed */
#define T_EVENTS	0x0800
#define T_ERROR		(~0)

#if !defined _SYS_TIHDR_H
/* 
 * The following are the flag definitions needed by the
 * user level library routines.
 */
#define T_MORE		0x001	/* more data */
#define T_EXPEDITED	0x002	/* expedited data */
#define T_PUSH		0x004	/* send data immediately */
#define T_NEGOTIATE	0x004	/* set opts */
#define T_CHECK		0x008	/* check opts */
#define T_DEFAULT	0x010	/* get default opts */
#define T_SUCCESS	0x020	/* successful */
#define T_FAILURE	0x040	/* failure */
/* 
 *  The following are _XOPEN_SOURCE (XPG4 and up).
 */
#if defined _XOPEN_SOURCE || defined __KERNEL__
#define T_CURRENT	0x080	/* get current options */
#define T_PARTSUCCESS	0x100	/* partial success */
#define T_READONLY	0x200	/* read-only */
#define T_NOTSUPPORT	0x400	/* not supported */
#endif				/* defined _XOPEN_SOURCE || defined __KERNEL__ */
#endif				/* !defined _SYS_TIHDR_H */

/* 
 *  Service type defines.
 */
#define T_COTS		1	/* Connection-mode transport service. */
#define T_COTS_ORD	2	/* Connection-mode with orderly release. */
#define T_CLTS		3	/* Connectionless-mode transport service. */

/* 
 *  Flags defines (other info about the transport provider).
 */
#define T_SENDZERO	0x001	/* supports 0-length TSDUs */
#define T_ORDRELDATA	0x002	/* supports orderly release data */
#define T_XPG4_1	0x100	/* XPG4 and higher */

#if defined _XOPEN_SOURCE || defined __KERNEL__
/* 
 * The following are structure types used when dynamically allocating the
 * above structures via t_alloc().
 */
#define T_BIND		1	/* allocate t_bind structure */
#define T_OPTMGMT	2	/* allocate t_optmgmt structure */
#define T_CALL		3	/* allocate t_call structure */
#define T_DIS		4	/* allocate t_discon structure */
#define T_UNITDATA	5	/* allocate t_unitdata structure */
#define T_UDERROR	6	/* allocate t_uderr structure */
#define T_INFO		7	/* allocate t_info structure */
#else				/* defined _XOPEN_SOURCE || defined __KERNEL__ */
#define T_BIND_STR	1	/* allocate t_bind structure */
#define T_OPTMGMT_STR	2	/* allocate t_optmgmt structure */
#define T_CALL_STR	3	/* allocate t_call structure */
#define T_DIS_STR	4	/* allocate t_discon structure */
#define T_UNITDATA_STR	5	/* allocate t_unitdata structure */
#define T_UDERROR_STR	6	/* allocate t_uderr structure */
#define T_INFO_STR	7	/* allocate t_info structure */
#endif				/* defined _XOPEN_SOURCE || defined __KERNEL__ */

/* 
 * The following bits specify which fields of the above structures should be
 * allocated by t_alloc().
 */
#define T_ADDR		0x0001	/* address */
#define T_OPT		0x0002	/* options */
#define T_UDATA		0x0004	/* user data */
#define T_ALL		0xffff	/* all the above fields supported */

/* 
 * The following are the states for the user.
 */
#define T_UNINIT	0	/* unitialized state */
#define T_UNBND		1	/* unbound */
#define T_IDLE		2	/* idle */
#define T_OUTCON	3	/* outgoing connection pending */
#define T_INCON		4	/* incoming connection pending */
#define T_DATAXFER	5	/* data transfer */
#define T_OUTREL	6	/* outgoing release pending */
#define T_INREL		7	/* incoming release pending */

/* 
 * General purpose defines.
 */
#define T_YES		1
#define T_NO		0
#define T_UNUSED	(-1)
#define T_NULL		0
#define T_ABSREQ	0x8000

#define T_LEAF_NOCHANGE		0
#define T_LEAF_CONNECTED	1
#define T_LEAF_DISCONNECTED	2

#if defined _XOPEN_SOURCE || defined __KERNEL__
#if !defined _SYS_TIHDR_H
#define T_INFINITE	(-1)	/* The corresponding size is unbounded. */
#define T_INVALID	(-2)	/* The corresponding element is invalid. */
#endif				/* !defined _SYS_TIHDR_H */

/* 
 * General definitions for option management
 */
#define T_UNSPEC  (~0 - 2)	/* applicable to u_long, t_scalar_t, char .. */
#define T_ALLOPT  0

#define T_ALIGN(p)	(((uintptr_t)(p) + sizeof(long)-1) & ~(sizeof(long)-1))

#if !defined _SYS_TIHDR_H
/* 
 * The following T_OPT_FIRSTHDR, T_OPT_NEXTHDR and T_OPT_DATA macros have the
 * semantics required by the standard. They are used to store and read
 * multiple variable length objects delimited by a 'header' descriptor and the
 * variable length value content while allowing aligned access to each in an
 * arbitrary memory buffer.
 *
 * The required minimum alignment (based on types used internally in the
 * specification for header and data alignment is "sizeof(t_uscalar_t)"
 *
 * The definitions shown for macro bodies are examples only and other forms
 * are not only possible but are specifically permitted.
 *
 * The examples shown assume that the implementation chooses to align the
 * header and data parts at the required minimum of "sizeof(t_uscalar_t).
 * Stricter alignment is permitted by an implementation.
 *
 * Helper macros starting with "_T" prefix are used below.  These are not a
 * requirement of the specification and only used for constructing example
 * macro body definitions.
 */

/* 
 * Helper macro aligns to sizeof(t_uscalar_t) boundary.
 */
#define _T_USCALAR_ALIGN(p) \
	(((uintptr_t)(p) + (sizeof (t_scalar_t)-1)) & ~(sizeof (t_scalar_t)-1))

/* 
 * Get aligned start of first option header.  This implementation assumes
 * option buffers are allocated by t_alloc() and hence aligned to start any
 * sized object (including option header) is guaranteed.
 *
 * \code
 * struct t_opthdr *T_OPT_FIRSTHDR(struct netbuf *nbp):
 * \endcode
 *
 */
#define T_OPT_FIRSTHDR(nbp) \
	((((char *)(nbp)->buf + sizeof (struct t_opthdr)) <= \
	  (char *)(nbp)->buf + (nbp)->len) ? \
	 (struct t_opthdr *)(nbp)->buf : (struct t_opthdr *)0)

/* 
 * unsigned char *T_OPT_DATA(struct t_opthdr *tohp):
 * Get aligned start of data part after option header
 *
 * This implementation assumes "sizeof (t_uscalar_t)" as the alignment size
 * for its option data and option header with no padding in "struct t_opthdr"
 * definition.
 */
#define T_OPT_DATA(tohp) \
	((unsigned char *)(tohp) + sizeof (struct t_opthdr))

/* 
 * struct t_opthdr *_T_NEXTHDR(char *pbuf, unsigned int buflen, struct t_opthdr *popt):
 *
 * Helper macro for defining T_OPT_NEXTHDR macro.  This implementation assumes
 * "sizeof (t_uscalar_t)" as the alignment for its option data and option
 * header.  Also it assumes "struct t_opthdr" definitions contain no padding.
 */
#define _T_NEXTHDR(pbuf, buflen, popt) \
	(((char *) (popt) + _T_USCALAR_ALIGN((popt)->len) + sizeof(struct t_opthdr) <= \
	  ((char *) (pbuf) + (buflen))) \
	 ? (struct t_opthdr *) ((char *) (popt) + _T_USCALAR_ALIGN((popt)->len)) \
	 : (struct t_opthdr *) 0)

/* 
 * struct t_opthdr *T_OPT_NEXTHDR(struct netbuf *nbp, \
 * struct t_opthdr *tohp):
 * Skip to next option header
 * This implementation assumes "sizeof (t_uscalar_t)" as the alignment size
 * for its option data and option header.
 */
#define T_OPT_NEXTHDR(nbp, tohp)  _T_NEXTHDR((nbp)->buf, \
                                      (nbp)->len, (tohp))

#define OPT_NEXTHDR(pbuf, buflen, popt)	(struct t_opthdr *) \
	(((char *)(popt) + T_ALIGN((popt)->len) < \
	  (char *)(pbuf) + (buflen)) \
	 ?  ((char *)(popt) + T_ALIGN((popt)->len)) \
	 : 0)

#endif				/* !defined _SYS_TIHDR_H */

/* OPTIONS ON XTI LEVEL */

/* 
 *  XTI Level
 */
#define XTI_GENERIC 0xffff

/* 
 *  XTI-level Options
 */
#define XTI_DEBUG	0x0001	/* enable debugging */
#define XTI_LINGER	0x0080	/* linger on close if data present */
#define XTI_RCVBUF	0x1002	/* receive buffer size */
#define XTI_RCVLOWAT	0x1004	/* receive low-water mark */
#define XTI_SNDBUF	0x1001	/* send buffer size */
#define XTI_SNDLOWAT	0x1003	/* send low-water mark */

/* 
 * Structure used with linger option.
 */
struct t_linger {
	t_scalar_t l_onoff;		/* option on/off */
	t_scalar_t l_linger;		/* linger time */
};
#endif				/* defined _XOPEN_SOURCE || defined __KERNEL__ */

#endif				/* !defined _XTI_XTI_H */
