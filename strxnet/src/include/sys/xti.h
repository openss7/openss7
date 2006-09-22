/*****************************************************************************

 @(#) $Id: xti.h,v 0.9.2.9 2006/09/22 20:59:27 brian Exp $

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

 Last Modified $Date: 2006/09/22 20:59:27 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti.h,v $
 Revision 0.9.2.9  2006/09/22 20:59:27  brian
 - prepared header file for use with doxygen, touching many lines

 Revision 0.9.2.8  2006/09/18 13:52:45  brian
 - added doxygen markers to sources

 Revision 0.9.2.7  2006/02/23 12:00:10  brian
 - corrections for 64bit and 32/64bit compatibility
 - updated headers

 *****************************************************************************/

#ifndef _SYS_XTI_H
#define _SYS_XTI_H

#ident "@(#) $RCSfile: xti.h,v $ $Name:  $($Revision: 0.9.2.9 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/**
  * @weakgroup xnet OpenSS7 XNET Library
  * @{
  * @file
  * XTI header file.
  * */

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

#ifndef __KERNEL__
/**
  * @name XTI Library Functions
  * These are the formal version of the XTI library functions.  They are strong
  * aliased to the thread-safe "__xnet_t_*_r()" versions below.  Where a "_r"
  * version does not exist the "__xnet_t_*()" version below is used in its
  * stead.
  * @{ */
/* *INDENT-OFF* */
/** XTI Library Function: t_accept - accept a connection request. */
extern int t_accept(int, int, const struct t_call *);
/** XTI Library Function: t_addleaf - add a leaf to point to multipoint
  * connection. */
extern int t_addleaf(int, int, struct netbuf *);
/** XTI Library Function: t_alloc - allocate a library structure. */
extern char *t_alloc(int, int, int);
/** XTI Library Function: t_bind - bind an address to a transport endpoint. */
extern int t_bind(int, const struct t_bind *, struct t_bind *);
/** XTI Library Function: t_close - close a transport endpoint. */
extern int t_close(int);
/** XTI Library Function: t_connect - establish a connection. */
extern int t_connect(int, const struct t_call *, struct t_call *);
/** XTI Library Function: t_error - produce error message. */
extern int t_error(const char *);
/** XTI Library Function: t_free - free a library structure. */
extern int t_free(void *, int);
/** XTI Library Function: t_getinfo - get protocol-specific service information.  */
extern int t_getinfo(int, struct t_info *);
/** XTI Library Function: t_getprotaddr - get protocol addresses. */
extern int t_getprotaddr(int, struct t_bind *, struct t_bind *);
/** XTI Library Function: t_getstate - get the current state. */
extern int t_getstate(int);
/** XTI Library Function: t_listen - listen for a connection indication. */
extern int t_listen(int, struct t_call *);
/** XTI Library Function: t_look - look at current event on a transport
  * endpoint.  */
extern int t_look(int);
/** XTI Library Function: t_open - establish a transport endpoint. */
extern int t_open(const char *, int, struct t_info *);
/** XTI Library Function: t_optmgmt - manage options for a transport endpoint.  */
extern int t_optmgmt(int, const struct t_optmgmt *, struct t_optmgmt *);
/** XTI Library Function: t_rcv - receive data or expedited data on a
  * connection.  */
extern int t_rcv(int, char *, unsigned int, int *);
/** XTI Library Function: t_rcvconnect - receive the confirmation from a
  * connection request. */
extern int t_rcvconnect(int, struct t_call *);
/** XTI Library Function: t_rcvdis - retrieve information from disconnect. */
extern int t_rcvdis(int, struct t_discon *);
/** XTI Library Function: t_rcvleafchange - acknowledge receipt of a leaf change
  * indication. */
extern int t_rcvleafchange(int, struct t_leaf_status *);
/** XTI Library Function: t_rcvopt - receive data with options. */
extern int t_rcvopt(int fd, struct t_unitdata *optdata, int *flags);
/** XTI Library Function: t_rcvrel - acknowledge receipt of an orderly release
  * indication. */
extern int t_rcvrel(int);
/** XTI Library Function: t_rcvreldata - receive an orderly release indication
  * or confirmation containing user data. */
extern int t_rcvreldata(int, struct t_discon *);
/** XTI Library Function: t_rcvudata - receive a data unit. */
extern int t_rcvudata(int, struct t_unitdata *, int *);
/** XTI Library Function: t_rcvuderr - receive a unit data error indication. */
extern int t_rcvuderr(int, struct t_uderr *);
/** XTI Library Function: t_rcvv - receive data or expedited data sent over a
  * connection and put the data into one or more noncontiguous buffers. */
extern int t_rcvv(int, struct t_iovec *, unsigned int, int *);
/** XTI Library Function: t_rcvvopt - receive data with options into one or more
  * noncontiguous buffers. */
extern int t_rcvvopt(int fd, const struct t_unitdata *optdata, const
		     struct t_iovec *iov, unsigned int iovcount, int flags);
/** XTI Library Function: t_rcvvudata - receive a data unit into one or more
  * noncontiguous buffers. */
extern int t_rcvvudata(int, struct t_unitdata *, struct t_iovec *, unsigned int, int *);
/** XTI Library Function: t_removeleaf - remove a leaf from a point to
  * multipoint connection. */
extern int t_removeleaf(int, int, int);
/** XTI Library Function: t_snd - send data or expedited data over a connection.
  */
extern int t_snd(int, char *, unsigned int, int);
/** XTI Library Function: t_snddis - send user-initiated disconnect request. */
extern int t_snddis(int, const struct t_call *);
/** XTI Library Function: t_sndopt - send data with options. */
extern int t_sndopt(int fd, const struct t_unitdata *optdata, int flags);
/** XTI Library Function: t_sndrel - initiate an orderly release. */
extern int t_sndrel(int);
/** XTI Library Function: t_sndreldata - initiate or respond to an orderly
  * release with user data. */
extern int t_sndreldata(int, struct t_discon *);
/** XTI Library Function: t_sndudata - send a data unit. */
extern int t_sndudata(int, const struct t_unitdata *);
/** XTI Library Function: t_sndv - send data or expedited data, from one or more
  * noncontiguous buffers, on a connection. */
extern int t_sndv(int, const struct t_iovec *, unsigned int, int);
/** XTI Library Function: t_sndvopt - send data with options from one or more
  * non-contiguous buffers. */
extern int t_sndvopt(int fd, const struct t_unitdata *optdata, const
		     struct t_iovec *iov, unsigned int iovcount, int flags);
/** XTI Library Function: t_sndvudata - send a data unit from one or more
  * non-contiguous buffers. */
extern int t_sndvudata(int, struct t_unitdata *, struct t_iovec *, unsigned int);
/** XTI Library Function: t_strerror - generate error message string. */
extern const char *t_strerror(int);
/** XTI Library Function: t_sync - synchronise transport library. */
extern int t_sync(int);
/** XTI Library Function: t_sysconf - get configurable XTI variables. */
extern int t_sysconf(int);
/** XTI Library Function: t_unbind - disable a transport endpoint. */
extern int t_unbind(int);
/* *INDENT-ON* */
/** @} */

/*
 * These are non-thread-safe versions of the above functions.
 */
extern int __xnet_t_accept(int, int, const struct t_call *);
extern int __xnet_t_addleaf(int, int, struct netbuf *);
extern char *__xnet_t_alloc(int, int, int);
extern int __xnet_t_bind(int, const struct t_bind *, struct t_bind *);
extern int __xnet_t_close(int);
extern int __xnet_t_connect(int, const struct t_call *, struct t_call *);
extern int __xnet_t_error(const char *);
extern int __xnet_t_free(void *, int);
extern int __xnet_t_getinfo(int, struct t_info *);
extern int __xnet_t_getprotaddr(int, struct t_bind *, struct t_bind *);
extern int __xnet_t_getstate(int);
extern int __xnet_t_listen(int, struct t_call *);
extern int __xnet_t_look(int);
extern int __xnet_t_open(const char *, int, struct t_info *);
extern int __xnet_t_optmgmt(int, const struct t_optmgmt *, struct t_optmgmt *);
extern int __xnet_t_rcv(int, char *, unsigned int, int *);
extern int __xnet_t_rcvconnect(int, struct t_call *);
extern int __xnet_t_rcvdis(int, struct t_discon *);
extern int __xnet_t_rcvleafchange(int, struct t_leaf_status *);
extern int __xnet_t_rcvopt(int fd, struct t_unitdata *optdata, int *flags);
extern int __xnet_t_rcvrel(int);
extern int __xnet_t_rcvreldata(int, struct t_discon *);
extern int __xnet_t_rcvudata(int, struct t_unitdata *, int *);
extern int __xnet_t_rcvuderr(int, struct t_uderr *);
extern int __xnet_t_rcvv(int, struct t_iovec *, unsigned int, int *);
extern int __xnet_t_rcvvopt(int fd, const struct t_unitdata *optdata, const
			    struct t_iovec *iov, unsigned int iovcount, int flags);
extern int __xnet_t_rcvvudata(int, struct t_unitdata *, struct t_iovec *, unsigned int, int *);
extern int __xnet_t_removeleaf(int, int, int);
extern int __xnet_t_snd(int, char *, unsigned int, int);
extern int __xnet_t_snddis(int, const struct t_call *);
extern int __xnet_t_sndopt(int fd, const struct t_unitdata *optdata, int flags);
extern int __xnet_t_sndrel(int);
extern int __xnet_t_sndreldata(int, struct t_discon *);
extern int __xnet_t_sndudata(int, const struct t_unitdata *);
extern int __xnet_t_sndv(int, const struct t_iovec *, unsigned int, int);
extern int __xnet_t_sndvopt(int fd, const struct t_unitdata *optdata, const
			    struct t_iovec *iov, unsigned int iovcount, int flags);
extern int __xnet_t_sndvudata(int, struct t_unitdata *, struct t_iovec *, unsigned int);
extern const char *__xnet_t_strerror(int);
extern int __xnet_t_sync(int);
extern int __xnet_t_sysconf(int);
extern int __xnet_t_unbind(int);

/*
 * These are thread-safe versions of the XTI Library functions.  Functions that
 * do not appear on this list (with an "_r") do not require thread protection
 * (normally because they can contain a thread cancellation point and they do
 * not accept a file descriptor for which read lock protection in required.)
 */
extern int __xnet_t_accept_r(int fd, int resfd, const struct t_call *call);
extern int __xnet_t_addleaf_r(int fd, int leafid, struct netbuf *addr);
extern char *__xnet_t_alloc_r(int fd, int type, int fields);
extern int __xnet_t_bind_r(int fd, const struct t_bind *req, struct t_bind *ret);
extern int __xnet_t_close_r(int fd);
extern int __xnet_t_connect_r(int fd, const struct t_call *sndcall, struct t_call *rcvcall);
extern int __xnet_t_getinfo_r(int fd, struct t_info *info);
extern int __xnet_t_getstate_r(int fd);
extern int __xnet_t_listen_r(int fd, struct t_call *call);
extern int __xnet_t_look_r(int fd);
extern int __xnet_t_open_r(const char *path, int oflag, struct t_info *info);
extern int __xnet_t_optmgmt_r(int fd, const struct t_optmgmt *req, struct t_optmgmt *ret);
extern int __xnet_t_rcv_r(int fd, char *buf, unsigned int nbytes, int *flags);
extern int __xnet_t_rcvconnect_r(int fd, struct t_call *call);
extern int __xnet_t_rcvdis_r(int fd, struct t_discon *discon);
extern int __xnet_t_rcvleafchange_r(int fd, struct t_leaf_status *change);
extern int __xnet_t_rcvrel_r(int fd);
extern int __xnet_t_rcvreldata_r(int fd, struct t_discon *discon);
extern int __xnet_t_rcvopt_r(int fd, struct t_unitdata *optdata, int *flags);
extern int __xnet_t_rcvudata_r(int fd, struct t_unitdata *unitdata, int *flags);
extern int __xnet_t_rcvv_r(int fd, struct t_iovec *iov, unsigned int iovcount, int *flags);
extern int __xnet_t_rcvvudata_r(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
				unsigned int iovcount, int *flags);
extern int __xnet_t_removeleaf_r(int fd, int leafid, int reason);
extern int __xnet_t_snd_r(int fd, char *buf, unsigned int nbytes, int flags);
extern int __xnet_t_snddis_r(int fd, const struct t_call *call);
extern int __xnet_t_sndrel_r(int fd);
extern int __xnet_t_sndreldata_r(int fd, struct t_discon *discon);
extern int __xnet_t_sndopt_r(int fd, const struct t_unitdata *optdata, int flags);
extern int __xnet_t_sndvopt_r(int fd, const struct t_unitdata *optdata, const struct t_iovec *iov,
			      unsigned int iovcount, int flags);
extern int __xnet_t_sndudata_r(int fd, const struct t_unitdata *unitdata);
extern int __xnet_t_sndv_r(int fd, const struct t_iovec *iov, unsigned int iovcount, int flags);
extern int __xnet_t_sndvudata_r(int fd, struct t_unitdata *unitdata, struct t_iovec *iov,
				unsigned int iovcount);
extern int __xnet_t_unbind_r(int fd);

#endif				/* __KERNEL__ */

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

// vim: ft=cpp com=sr\:/**,mb\:\ *,eb\:\ */,sr\:/*,mb\:*,eb\:*/,b\:TRANS
