/*****************************************************************************

 @(#) $Id: tiuser.h,v 0.9.2.3 2006/09/01 08:53:03 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/09/01 08:53:03 $ by $Author: brian $

 *****************************************************************************/

#ifndef _TIUSER_H
#define _TIUSER_H

#ident "@(#) $RCSfile: tiuser.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @weakgroup xnet
  * @{ */

/** @file
  * Transport Interface User header file. */

#include <sys/tiuser.h>

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

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

/*
  * @name TLI Library Functions
  * @{
  */
/* TLI Library Function: t_accept - accept a connection request. */
extern int t_accept(int fd, int resfd, struct t_call *call);

/* TLI Library Function: t_alloc - allocate a library structure. */
extern char *t_alloc(int fd, int struct_type, int fields);

/* TLI Library Function: t_bind - bind an address to a transport endpoint. */
extern int t_bind(int fd, struct t_bind *req, struct t_bind *ret);

/* TLI Library Function: t_close - close a transport endpoint. */
extern int t_close(int fd);

/* TLI Library Function: t_connect - establish a connection. */
extern int t_connect(int fd, struct t_call *sndcall, struct t_call *rcvcall);

/* TLI Library Function: t_error - produce error message. */
extern void t_error(const char *errmsg);

/* TLI Library Function: t_free - free a library structure. */
extern int t_free(char *ptr, int struct_type);

/* TLI Library Function: t_getinfo - get protocol-specific service information. */
extern int t_getinfo(int fd, struct t_info *info);

/* TLI Library Function: t_getstate - get the current state. */
extern int t_getstate(int fd);

/* TLI Library Function: t_listen - listen for a connection indication. */
extern int t_listen(int fd, struct t_call *call);

/* TLI Library Function: t_look - look at current event on a transport endpoint. */
extern int t_look(int fd);

#if 0
extern int t_nonblocking(int fd);	/* not an XTI function. */
#endif
/* TLI Library Function: t_open - establish a transport endpoint. */
extern int t_open(const char *path, int oflag, struct t_info *info);

/* TLI Library Function: t_optmgmt - manage options for a transport endpoint. */
extern int t_optmgmt(int fd, struct t_optmgmt *req, struct t_optmgmt *ret);

/* TLI Library Function: t_rcv - receive data or expedited data on a connection. */
extern int t_rcv(int fd, char *buf, unsigned nbytes, int *flags);

/* TLI Library Function: t_rcvconnect - receive the confirmation from a connection request. */
extern int t_rcvconnect(int fd, struct t_call *call);

/* TLI Library Function: t_rcvdis - retrieve information from disconnect. */
extern int t_rcvdis(int fd, struct t_discon *discon);

/* TLI Library Function: t_rcvrel - acknowledge receipt of an orderly release indication. */
extern int t_rcvrel(int fd);

/* TLI Library Function: t_rcvudata - receive a data unit. */
extern int t_rcvudata(int fd, struct t_unitdata *unitdata, int *flags);

/* TLI Library Function: t_rcvuderr - receive a unit data error indication. */
extern int t_rcvuderr(int fd, struct t_uderr *uderr);

/* TLI Library Function: t_snd - send data or expedited data over a connection. */
extern int t_snd(int fd, char *buf, unsigned nbytes, int flags);

/* TLI Library Function: t_snddis - send user-initiated disconnect request. */
extern int t_snddis(int fd, struct t_call *call);

/* TLI Library Function: t_sndrel - initiate an orderly release. */
extern int t_sndrel(int fd);

/* TLI Library Function: t_sndudata - send a data unit. */
extern int t_sndudata(int fd, struct t_unitdata *unitdata);

/* TLI Library Function: t_strerror - generate error message string. */
extern char *t_strerror(int);

/* TLI Library Function: t_sync - synchronise transport library. */
extern int t_sync(int fd);

/* TLI Library Function: t_unbind - disable a transport endpoint. */
extern int t_unbind(int fd);

/* TLI Library - deprecated direct access to error list. */
extern char *t_errlist[];
extern char *t_errstr[];

/* TLI Library - deprecated length of error list. */
extern int t_nerr;
extern int t_nerr;

/* @} */

/**
  * @name TLI Internal Library Functions
  * @{
  */
extern int __tli_t_accept(int fd, int resfd, struct t_call *call);
extern char *__tli_t_alloc(int fd, int struct_type, int fields);
extern int __tli_t_bind(int fd, struct t_bind *req, struct t_bind *ret);
extern int __tli_t_close(int fd);
extern int __tli_t_connect(int fd, struct t_call *sndcall, struct t_call *rcvcall);
extern void __tli_t_error(const char *errmsg);
extern int __tli_t_free(char *ptr, int struct_type);
extern int __tli_t_getinfo(int fd, struct t_info *info);
extern int __tli_t_getstate(int fd);
extern int __tli_t_listen(int fd, struct t_call *call);
extern int __tli_t_look(int fd);
#if 0
extern int __tli_t_nonblocking(int fd);	/* not an XTI function. */
#endif
extern int __tli_t_open(const char *path, int oflag, struct t_info *info);
extern int __tli_t_optmgmt(int fd, struct t_optmgmt *req, struct t_optmgmt *ret);
extern int __tli_t_rcv(int fd, char *buf, unsigned nbytes, int *flags);
extern int __tli_t_rcvconnect(int fd, struct t_call *call);
extern int __tli_t_rcvdis(int fd, struct t_discon *discon);
extern int __tli_t_rcvrel(int fd);
extern int __tli_t_rcvudata(int fd, struct t_unitdata *unitdata, int *flags);
extern int __tli_t_rcvuderr(int fd, struct t_uderr *uderr);
extern int __tli_t_snd(int fd, char *buf, unsigned nbytes, int flags);
extern int __tli_t_snddis(int fd, struct t_call *call);
extern int __tli_t_sndrel(int fd);
extern int __tli_t_sndudata(int fd, struct t_unitdata *unitdata);
extern char *__tli_t_strerror(int);
extern int __tli_t_sync(int fd);
extern int __tli_t_unbind(int fd);
extern char *__tli_t_errlist[];
extern char *__tli_t_errstr[];
/** @} */


#endif				/* __KERNEL__ */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _TIUSER_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
