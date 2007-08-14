/*****************************************************************************

 @(#) $Id: xti.h,v 0.9.2.6 2007/08/14 12:17:13 brian Exp $

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

 Last Modified $Date: 2007/08/14 12:17:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti.h,v $
 Revision 0.9.2.6  2007/08/14 12:17:13  brian
 - GPLv3 header updates

 Revision 0.9.2.5  2006/09/25 12:10:09  brian
 - updated and doxygenified headers

 Revision 0.9.2.4  2006/09/18 13:52:37  brian
 - added doxygen markers to sources

 Revision 0.9.2.3  2005/05/14 08:30:49  brian
 - copyright header correction

 Revision 0.9.2.2  2004/09/02 09:31:14  brian
 - Synchronization with other packages.

 Revision 0.9.2.1  2004/05/12 08:01:39  brian
 - Added in xti library and STREAMS modules.

 Revision 1.1.2.2  2004/04/13 13:05:20  brian
 - Minor adjustments to ATM exposed definitions.

 Revision 1.1.2.1  2004/04/13 12:12:52  brian
 - Rearranged header files.

 *****************************************************************************/

#ifndef _SYS_XTI_H
#define _SYS_XTI_H

#ident "@(#) $RCSfile: xti.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/**
  * @weakgroup xnet OpenSS7 XNET Library
  * @{ */

/** @file
  * XTI header file.  */

/*
 * XTI header file.
 */

#ifndef t_scalar_t
/** XTI signed scalar type.
  * Strangely enough, t_uscalar_t is defined in stropts.h for SUSv2 and SUSv3
  * systems, however, t_scalar_t is only defined here.  */
typedef int32_t t_scalar_t;

#define t_scalar_t t_scalar_t
#endif				/* !defined t_scalar_t */

#if !defined _STROPTS_H || defined NEED_T_USCALAR_T
#ifndef t_uscalar_t
/** XTI unsigned scalar type.
  * This type is not defined in the XTI Library as it is supposed to be defined
  * in stropts.h according to SUSv2 and SUSv3.  */
typedef u_int32_t t_uscalar_t;

#define t_uscalar_t t_uscalar_t
#endif				/* !defined t_uscalar_t */
#endif				/* !defined _STROPTS_H || defined NEED_T_USCALAR_T */

#include <sys/tpi.h>		/* common TLI, XTI, TI definitions */
#include <sys/xti_xti.h>	/* for XTI_GENERIC level options */

#define T_ALIGN(p)	(((uintptr_t)(p) + sizeof(t_uscalar_t)-1) & ~(sizeof(t_uscalar_t)-1))

#if 0
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
 * @code
 * struct t_opthdr *T_OPT_FIRSTHDR(struct netbuf *nbp):
 * @endcode
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
#endif

/**
  * @var extern int t_errno;
  * XTI error return.  t_errno is a modifiable lvalue of type int.  The above
  * definition is typical of a single-threaded environment.  In a
  * multi-threading environment a typical definition of t_errno is:
  *
  * @code
  * #define t_errno (*(_t_errno()))
  * extern int *_t_errno(void);
  * @endcode
  */
#define t_errno (*(_t_errno()))
extern int *_t_errno(void);

/**
  * iov maximum.
  */
#define T_IOV_MAX  16		/**< Maximum number of scatter/gather buffers.
				  The value is not mandatory.  The value must be
				  at least 16. */

/**
  * IO vector structure.
  */
struct t_iovec {
	void *iov_base;		/**< The base of the vector component. */
	size_t iov_len;		/**< The length of the vector component. */
};

/**
  * Network buffer.
  */
struct netbuf {
	unsigned int maxlen;	/**< Maximum length of output. */
	unsigned int len;	/**< Length of input. */
	char *buf;		/**< Pointer to the buffer. */
};

/**
  * Protocol-specific service limits.
  * Provides information on protocol service limits for the transport endpoint.
  */
struct t_info {
	t_scalar_t addr;	/**< Max size of the transport protocol address. */
	t_scalar_t options;	/**< Max number of bytes of protocol-specific options. */
	t_scalar_t tsdu;	/**< Max size of a transport service data unit. */
	t_scalar_t etsdu;	/**< Max size of expedited transport service data unit. */
	t_scalar_t connect;	/**< Max amount of data allowed on connection establishment
				   functions. */
	t_scalar_t discon;	/**< Max data allowed on t_snddis(), t_rcvdis(),
				   t_sndreldata() and t_rcvreldata() functions. */
	t_scalar_t servtype;	/**< Service type supported by transport provider. */
#if defined(_XOPEN_SOURCE)
	t_scalar_t flags;	/**< Other info about the transport provider. */
	t_scalar_t tidu;	/**< Max size of interface transport data unit. */
#endif
};

/**
  * Bind structure.
  * Format of the address and options arguments of bind.
  */
struct t_bind {
	struct netbuf addr;	/**< Address to which to bind. */
	unsigned int qlen;	/**< Max outstanding connection indications. */
};

/**
  * Options management structure.
  */
struct t_optmgmt {
	struct netbuf opt;	/**< Options to manage. */
	t_scalar_t flags;	/**< Options management flags (XTI). */
};

/**
  * Disconnection structure.
  */
struct t_discon {
	struct netbuf udata;	/**< User data. */
	int reason;		/**< Reason code. */
	int sequence;		/**< Sequence number. */
};

/**
  * Call structure.
  */
struct t_call {
	struct netbuf addr;	/**< Address. */
	struct netbuf opt;	/**< Options. */
	struct netbuf udata;	/**< User data. */
	int sequence;		/**< Sequence number. */
};

/**
  * Datagram structure.
  */
struct t_unitdata {
	struct netbuf addr;	/**< Address. */
	struct netbuf opt;	/**< Options. */
	struct netbuf udata;	/**< User data. */
};

/**
  * Unitdata error structure.
  */
struct t_uderr {
	struct netbuf addr;	/**< Address. */
	struct netbuf opt;	/**< Options. */
	t_scalar_t error;	/**< Error code (XTI). */
};

/*
 * Leaf status structure.
 */
struct t_leaf_status {
	int leafid;		/**< Leaf whose address has changed. */
	int status;		/**< Status: T_CONNECT or T_DISCONNECT. */
	int reason;		/**< Disconnect reason. */
};

#define t_leaf_status t_leaf_status

/** @name Leaf Status
  * @{ */
#define T_LEAF_NOCHANGE		0	/**< No change in leaf status. */
#define T_LEAF_CONNECTED	1	/**< Leaf has connected. */
#define T_LEAF_DISCONNECTED	2	/**< Leaf has disconnected. */
/** @} */

/**
  * @name XTI Events
  * The following are the events returned from t_look().
  * @{ */
#define T_LISTEN	0x0001	/**< Connection indication received. */
#define T_CONNECT	0x0002	/**< Connection confirmation received. */
#define T_DATA		0x0004	/**< Normal data received. */
#define T_EXDATA	0x0008	/**< Expedited data received. */
#define T_DISCONNECT	0x0010	/**< Disconnection received. */
#define T_UDERR		0x0040	/**< Datagram error indication. */
#define T_ORDREL	0x0080	/**< Orderly release indication. */
#define T_GODATA	0x0100	/**< Sending normal data is again possible. */
#define T_GOEXDATA	0x0200	/**< Sending expedited data is again possible. */
#define T_LEAFCHANGE	0x0400	/**< Status of a leaf has changed. */
#define T_EVENTS	0x0800	/**< Events mask. (OSF style: the Solaris style would be 0x0fff.) */
#define T_ERROR		(~0)	/**< Error return value (should really be minus one). */
/** @} */

/**
  * @name Structure Types
  * The following are structure types used when dynamically allocating the above
  * structures via t_alloc().
  * @{ */
#if defined(_XOPEN_SOURCE)
#define T_BIND		1	/**< Allocate t_bind structure. */
#define T_OPTMGMT	2	/**< Allocate t_optmgmt structure. */
#define T_CALL		3	/**< Allocate t_call structure. */
#define T_DIS		4	/**< Allocate t_discon structure. */
#define T_UNITDATA	5	/**< Allocate t_unitdata structure. */
#define T_UDERROR	6	/**< Allocate t_uderr structure. */
#define T_INFO		7	/**< Allocate t_info structure. */
#else
#define T_BIND_STR	1	/**< Allocate t_bind structure. */
#define T_OPTMGMT_STR	2	/**< Allocate t_optmgmt structure. */
#define T_CALL_STR	3	/**< Allocate t_call structure. */
#define T_DIS_STR	4	/**< Allocate t_discon structure. */
#define T_UNITDATA_STR	5	/**< Allocate t_unitdata structure. */
#define T_UDERROR_STR	6	/**< Allocate t_uderr structure. */
#define T_INFO_STR	7	/**< Allocate t_info structure. */
#endif
/** @} */

/**
  * @name Structure Field Bits
  * The following bits specify which fields of the above structures should be
  * allocated by t_alloc().
  * @{ */
#define T_ADDR		0x0001	/**< Address. */
#define T_OPT		0x0002	/**< Options. */
#define T_UDATA		0x0004	/**< User data. */
#define T_ALL		0xffff	/**< All the above fields supported. */
/** @} */

/**
  * @name User States
  * The following are the states for the user.
  * @{ */
#define T_UNINIT	0	/**< Unitialized state. */
#define T_UNBND		1	/**< Unbound. */
#define T_IDLE		2	/**< Idle. */
#define T_OUTCON	3	/**< Outgoing connection pending. */
#define T_INCON		4	/**< Incoming connection pending. */
#define T_DATAXFER	5	/**< Data transfer. */
#define T_OUTREL	6	/**< Outgoing release pending. */
#define T_INREL		7	/**< Incoming release pending. */
/** @} */

/**
  * @name General Constants
  * General purpose defines that are used either as option values or are used in
  * the information fields in the t_info structure.
  * @{ */
#define T_YES		1	/**< General purpose option value indicating true. */
#define T_NO		0	/**< General purpose option value indicating false. */
#define T_UNUSED	(-1)	/**< General purpose option value indicating unused. */
#define T_NULL		0	/**< Rather redundant seeming definition. */
#define T_ABSREQ	0x8000	/**< Options that are absolute requirements should have this bit set
				  somewhere, but it seems to be unused anywhere. */
#if defined(_XOPEN_SOURCE)
#define T_INFINITE	(-1)	/**< The corresponding size is unbounded. */
#define T_INVALID	(-2)	/**< The corresponding element is invalid. */
#endif
/** @} */

#endif				/* !defined _SYS_XTI_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
