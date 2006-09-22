/*****************************************************************************

 @(#) $Id: tiuser.h,v 0.9.2.10 2006/09/22 20:59:27 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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

 *****************************************************************************/

#ifndef _SYS_TIUSER_H
#define _SYS_TIUSER_H

#ident "@(#) $RCSfile: tiuser.h,v $ $Name:  $($Revision: 0.9.2.10 $) Copyright (c) 2001-2004 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @ingroup xnet
  * @{
  * @file
  * Transport Interface User header file.
  * */

#include <sys/tpi.h>		/* common definitions */

/*
 * Transport Interface User header file.
 */

/***
  * @name Events for t_look()
  * The following are the events returned from t_look().
  * @{ */
#define T_LISTEN	0x0001	/**< Connection indication received. */
#define T_CONNECT	0x0002	/**< Connection confirmation received. */
#define T_DATA		0x0004	/**< Normal data received. */
#define T_EXDATA	0x0008	/**< Expedited data received. */
#define T_DISCONNECT	0x0010	/**< Disconnection received. */
#define T_ERROR		0x0020	/**< Fatal error. */
#define T_UDERR		0x0040	/**< Datagram error indication. */
#define T_ORDREL	0x0080	/**< Orderly release indication. */
/*
 * The following are added (by XTI).
 */
#define T_GODATA	0x0100	/**< Normal flow control subsided. */
#define T_GOEXDATA	0x0200	/**< Expedited flow control subsided. */
#define T_EVENTS	0x00ff	/**< Event mask. */
/** @} */

/*
 * Protocol-specific service limits.
 * Provides information on protocol service limits for the transport endpoint.
 */
struct t_info {
	t_scalar_t addr;	/**< Max size of the transport protocol address */
	t_scalar_t options;	/**< Max number of bytes of protocol-specific options */
	t_scalar_t tsdu;	/**< Max size of a transport service data unit. */
	t_scalar_t etsdu;	/**< Max size of expedited transport service data unit. */
	t_scalar_t connect;	/**< Max amount of data allowed on connection establishment
				     functions. */
	t_scalar_t discon;	/**< Max data allowed on t_snddis(), t_rcvdis(),
				     t_sndreldata() and t_rcvreldata() functions.  */
	t_scalar_t servtype;	/**< Service type supported by transport provider. */
	t_scalar_t flags;	/**< Service flags */
};

/*
 * Network buffer.
 */
struct netbuf {
	unsigned int maxlen;	/**< Maximum length of output. */
	unsigned int len;	/**< Length of input. */
	char *buf;		/**< Pointer to the buffer. */
};

/*
 * Bind structure.
 * Format of the address and options arguments of bind.
 */
struct t_bind {
	struct netbuf addr;	/**< Address to which to bind. */
	unsigned int qlen;	/**< Max outstanding connection indications. */
};

/*
 * Options management structure.
 */
struct t_optmgmt {
	struct netbuf opt;	/**< Options to manage. */
#if __SVID
	long flags;		/**< Options management flags (TLI). */
#else					/* __SVID */
	t_scalar_t flags;	/**< Options management flags (XTI). */
#endif					/* __SVID */
};

/*
 * Disconnection structure.
 */
struct t_discon {
	struct netbuf udata;	/**< User data. */
	int reason;		/**< Reason code. */
	int sequence;		/**< Sequence number. */
};

/*
 * Call structure.
 */
struct t_call {
	struct netbuf addr;	/**< Address. */
	struct netbuf opt;	/**< Options. */
	struct netbuf udata;	/**< User data. */
	int sequence;		/**< Sequence number. */
};

/*
 * Datagram structure.
 */
struct t_unitdata {
	struct netbuf addr;	/**< Address. */
	struct netbuf opt;	/**< Options. */
	struct netbuf udata;	/**< User data. */
};

/*
 * Unitdata error structure.
 */
struct t_uderr {
	struct netbuf addr;	/**< Address. */
	struct netbuf opt;	/**< Options. */
#if __SVID
	long error;		/**< Error code (TLI). */
#else					/* __SVID */
	t_scalar_t error;	/**< Error code (XTI). */
#endif					/* __SVID */
};

/**
  * @name Structure Types
  * Structure types for use with t_alloc().
  * The following are structure types used when dynamically allocating the above
  * structures via t_alloc().
  * @{ */
#define T_BIND		1	/**< Allocate t_bind structure. */
#define T_OPTMGMT	2	/**< Allocate t_optmgmt structure. */
#define T_CALL		3	/**< Allocate t_call structure. */
#define T_DIS		4	/**< Allocate t_discon structure. */
#define T_UNITDATA	5	/**< Allocate t_unitdata structure. */
#define T_UDERROR	6	/**< Allocate t_uderr structure. */
#define T_INFO		7	/**< Allocate t_info structure. */
/** @} */

/**
  * @name Member Bits
  * Member bits for use with t_alloc().
  * The following bits specify which fields of the above structures should be
  * allocated by t_alloc().
  * @{ */
#define T_ADDR		0x01	/**< Address. */
#define T_OPT		0x02	/**< Options. */
#define T_UDATA		0x04	/**< User data. */
#if 0
#define T_ALL		0x07	/**< All the above fields. */
#else
#define T_ALL		0xffff	/**< All the above fields. */
#endif
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
#if 0
#define T_BADSTATE	8	/**< Illegal state. */
#endif
#define T_FAKE		8	/**< Illegal state. */
#define T_NOSTATES	9
/** @} */

/**
  * @name User-Level Events
  * User-level events.
  * @{ */
#define T_OPEN		0
#define T_BIND		1
#define T_OPTMGMT	2
#define T_UNBIND	3
#define T_CLOSE		4
#define T_SNDUDATA	5
#define T_RCVUDATA	6
#define T_RCVUDERR	7
#define T_CONNECT1	8
#define T_CONNECT2	9
#define T_RCVCONNECT	10
#define T_LISTN		11
#define T_ACCEPT1	12
#define T_ACCEPT2	13
#define T_ACCEPT3	14
#define T_SND		15
#define T_RCV		16
#define T_SNDDIS1	17
#define T_SNDDIS2	18
#define T_RCVDIS1	19
#define T_RCVDIS2	20
#define T_RCVDIS3	21
#define T_SNDREL	22
#define T_RCVREL	23
#define T_PASSCON	24
#define T_NOEVENTS	25
/** @} */

#ifndef __KERNEL__

/**
  * @var extern int t_errno;
  *
  * TLI error return.
  * t_errno is a modifiable lvalue of type int.  The above definition is typical
  * of a single-threaded environment.  In a multi-threading environment a
  * typical definition of t_errno is:
  */
#define t_errno (*(_t_errno()))
extern int *_t_errno(void);

/**
  * @name TLI Library Functions
  * @{
  */
/** TLI Library Function: t_accept - accept a connection request. */
extern int t_accept(int fd, int resfd, struct t_call *call);
extern int __tli_t_accept(int fd, int resfd, struct t_call *call);

/** TLI Library Function: t_alloc - allocate a library structure. */
extern char *t_alloc(int fd, int struct_type, int fields);
extern char *__tli_t_alloc(int fd, int struct_type, int fields);

/** TLI Library Function: t_bind - bind an address to a transport endpoint. */
extern int t_bind(int fd, struct t_bind *req, struct t_bind *ret);
extern int __tli_t_bind(int fd, struct t_bind *req, struct t_bind *ret);

/** TLI Library Function: t_close - close a transport endpoint. */
extern int t_close(int fd);
extern int __tli_t_close(int fd);

/** TLI Library Function: t_connect - establish a connection. */
extern int t_connect(int fd, struct t_call *sndcall, struct t_call *rcvcall);
extern int __tli_t_connect(int fd, struct t_call *sndcall, struct t_call *rcvcall);

/** TLI Library Function: t_error - produce error message. */
extern void t_error(const char *errmsg);
extern void __tli_t_error(const char *errmsg);

/** TLI Library Function: t_free - free a library structure. */
extern int t_free(char *ptr, int struct_type);
extern int __tli_t_free(char *ptr, int struct_type);

/** TLI Library Function: t_getinfo - get protocol-specific service information. */
extern int t_getinfo(int fd, struct t_info *info);
extern int __tli_t_getinfo(int fd, struct t_info *info);

/** TLI Library Function: t_getstate - get the current state. */
extern int t_getstate(int fd);
extern int __tli_t_getstate(int fd);

/** TLI Library Function: t_listen - listen for a connection indication. */
extern int t_listen(int fd, struct t_call *call);
extern int __tli_t_listen(int fd, struct t_call *call);

/** TLI Library Function: t_look - look at current event on a transport endpoint. */
extern int t_look(int fd);
extern int __tli_t_look(int fd);

#if 0
extern int t_nonblocking(int fd);	/* not an XTI function. */
extern int __tli_t_nonblocking(int fd);	/* not an XTI function. */
#endif
/** TLI Library Function: t_open - establish a transport endpoint. */
extern int t_open(const char *path, int oflag, struct t_info *info);
extern int __tli_t_open(const char *path, int oflag, struct t_info *info);

/** TLI Library Function: t_optmgmt - manage options for a transport endpoint. */
extern int t_optmgmt(int fd, struct t_optmgmt *req, struct t_optmgmt *ret);
extern int __tli_t_optmgmt(int fd, struct t_optmgmt *req, struct t_optmgmt *ret);

/** TLI Library Function: t_rcv - receive data or expedited data on a connection. */
extern int t_rcv(int fd, char *buf, unsigned nbytes, int *flags);
extern int __tli_t_rcv(int fd, char *buf, unsigned nbytes, int *flags);

/** TLI Library Function: t_rcvconnect - receive the confirmation from a connection request. */
extern int t_rcvconnect(int fd, struct t_call *call);
extern int __tli_t_rcvconnect(int fd, struct t_call *call);

/** TLI Library Function: t_rcvdis - retrieve information from disconnect. */
extern int t_rcvdis(int fd, struct t_discon *discon);
extern int __tli_t_rcvdis(int fd, struct t_discon *discon);

/** TLI Library Function: t_rcvrel - acknowledge receipt of an orderly release indication. */
extern int t_rcvrel(int fd);
extern int __tli_t_rcvrel(int fd);

/** TLI Library Function: t_rcvudata - receive a data unit. */
extern int t_rcvudata(int fd, struct t_unitdata *unitdata, int *flags);
extern int __tli_t_rcvudata(int fd, struct t_unitdata *unitdata, int *flags);

/** TLI Library Function: t_rcvuderr - receive a unit data error indication. */
extern int t_rcvuderr(int fd, struct t_uderr *uderr);
extern int __tli_t_rcvuderr(int fd, struct t_uderr *uderr);

/** TLI Library Function: t_snd - send data or expedited data over a connection. */
extern int t_snd(int fd, char *buf, unsigned nbytes, int flags);
extern int __tli_t_snd(int fd, char *buf, unsigned nbytes, int flags);

/** TLI Library Function: t_snddis - send user-initiated disconnect request. */
extern int t_snddis(int fd, struct t_call *call);
extern int __tli_t_snddis(int fd, struct t_call *call);

/** TLI Library Function: t_sndrel - initiate an orderly release. */
extern int t_sndrel(int fd);
extern int __tli_t_sndrel(int fd);

/** TLI Library Function: t_sndudata - send a data unit. */
extern int t_sndudata(int fd, struct t_unitdata *unitdata);
extern int __tli_t_sndudata(int fd, struct t_unitdata *unitdata);

/** TLI Library Function: t_strerror - generate error message string. */
extern char *t_strerror(int);
extern char *__tli_t_strerror(int);

/** TLI Library Function: t_sync - synchronise transport library. */
extern int t_sync(int fd);
extern int __tli_t_sync(int fd);

/** TLI Library Function: t_unbind - disable a transport endpoint. */
extern int t_unbind(int fd);
extern int __tli_t_unbind(int fd);

/** TLI Library - deprecated direct access to error list. */
extern char *t_errlist[];
extern char *__tli_t_errlist[];
extern char *t_errstr[];
extern char *__tli_t_errstr[];

/** TLI Library - deprecated length of error list. */
extern int t_nerr;
extern int t_nerr;

/** @} */

#endif				/* __KERNEL__ */

#endif				/* _SYS_TIUSER_H */

/** @} */

// vim: ft=cpp com=sr\:/**,mb\:\ *,eb\:\ */,sr\:/*,mb\:*,eb\:*/,b\:TRANS
