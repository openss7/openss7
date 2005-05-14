/*****************************************************************************

 @(#) $Id: tiuser.h,v 0.9.2.2 2005/05/14 08:30:48 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:30:48 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_TIUSER_H
#define _SYS_TIUSER_H

#ident "@(#) $RCSfile: tiuser.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2004 OpenSS7 Corporation."

/* 
 * The following are the events returned from t_look().
 */
#define T_LISTEN	0x0001	/* connection indication received */
#define T_CONNECT	0x0002	/* connection confirmation received */
#define T_DATA		0x0004	/* normal data received */
#define T_EXDATA	0x0008	/* expedited data received */
#define T_DISCONNECT	0x0010	/* disconnection received */
#define T_ERROR		0x0020	/* fatal error */
#define T_UDERR		0x0040	/* datagram error indication */
#define T_ORDREL	0x0080	/* orderly release indication */
#define T_EVENTS	0x00ff	/* event mask */

/* 
 *  \struct t_info
 *  Protocol-specific service limits.  Provides information on protocol
 *  service limits for the transport endpoint.
 */
struct t_info {
	t_scalar_t addr;		/* max size of the transport protocol address */
	t_scalar_t options;		/* max number of bytes of protocol-specific options */
	t_scalar_t tsdu;		/* max size of a transport service data unit. */
	t_scalar_t etsdu;		/* max size of expedited transport service data unit. */
	t_scalar_t connect;		/* max amount of data allowed on connection establishment
					   functions. */
	t_scalar_t discon;		/* max data allowed on t_snddis(), t_rcvdis(),
					   t_sndreldata() and t_rcvreldata() functions. */
	t_scalar_t servtype;		/* Service type supported by transport provider. */
};

/* 
 * netbuf structure.
 */
struct netbuf {
	unsigned int maxlen;
	unsigned int len;
	char *buf;
};

/* 
 *  Format of the address and options arguments of bind.
 */
struct t_bind {
	struct netbuf addr;
	unsigned int qlen;
};

/* 
 * Options management structure.
 */
struct t_optmgmt {
	struct netbuf opt;
	t_scalar_t flags;
};

/* 
 * Disconnection structure.
 */
struct t_discon {
	struct netbuf udata;		/* user data */
	int reason;			/* reason code */
	int sequence;			/* sequence number */
};

/* 
 * Call structure.
 */
struct t_call {
	struct netbuf addr;		/* address */
	struct netbuf opt;		/* options */
	struct netbuf udata;		/* user data */
	int sequence;			/* sequence number */
};

/* 
 * Datagram structure.
 */
struct t_unitdata {
	struct netbuf addr;		/* address */
	struct netbuf opt;		/* options */
	struct netbuf udata;		/* user data */
};

/* 
 * Unitdata error structure.
 */
struct t_uderr {
	struct netbuf addr;		/* address */
	struct netbuf opt;		/* options */
	t_scalar_t error;		/* error code */
};

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

/* 
 * The following bits specify which fields of the above structures should be
 * allocated by t_alloc().
 */
#define T_ADDR		0x01	/* address */
#define T_OPT		0x02	/* options */
#define T_UDATA		0x04	/* user data */
#define T_ALL		0x07	/* all the above fields */

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
#define T_BADSTATE	8	/* illegal state */

#ifndef __KERNEL__

/* 
 * extern int t_errno;
 * 
 * TLI error return.  t_errno is a modifiable lvalue of type int.  The above
 * definition is typical of a single-threaded environment.  In a
 * multi-threading environment a typical definition of t_errno is:
 */
extern int *_t_errno __P((void));
#define t_errno (*(_t_errno()))

/* 
 *  TLI LIBRARY FUNCTIONS
 */
/* TLI Library Function: t_accept - accept a connection request */
extern int t_accept __P((int fd, int resfd, struct t_call * call));
/* TLI Library Function: t_alloc - allocate a library structure */
extern char *t_alloc __P((int fd, int struct_type, int fields));
/* TLI Library Function: t_bind - bind an address to a transport endpoint */
extern int t_bind __P((int fd, struct t_bind * req, struct t_bind * ret));
/* TLI Library Function: t_close - close a transport endpoint */
extern int t_close __P((int fd));
/* TLI Library Function: t_connect - establish a connection */
extern int t_connect __P((int fd, struct t_call * sndcall, struct t_call * rcvcall));
/* TLI Library Function: t_error - produce error message */
extern void t_error __P((const char *errmsg));
/* TLI Library Function: t_free - free a library structure */
extern int t_free __P((char *ptr, int struct_type));
/* TLI Library Function: t_getinfo - get protocol-specific service information */
extern int t_getinfo __P((int fd, struct t_info * info));
/* TLI Library Function: t_getstate - get the current state */
extern int t_getstate __P((int fd));
/* TLI Library Function: t_listen - listen for a connection indication */
extern int t_listen __P((int fd, struct t_call * call));
/* TLI Library Function: t_look - look at current event on a transport endpoint */
extern int t_look __P((int fd));
extern int t_nonblocking __P((int fd));
/* TLI Library Function: t_open - establish a transport endpoint */
extern int t_open __P((const char *path, int oflag, struct t_info * info));
/* TLI Library Function: t_optmgmt - manage options for a transport endpoint */
extern int t_optmgmt __P((int fd, struct t_optmgmt * req, struct t_optmgmt * ret));
/* TLI Library Function: t_rcv - receive data or expedited data on a connection */
extern int t_rcv __P((int fd, char *buf, unsigned nbytes, int *flags));
/* TLI Library Function: t_rcvconnect - receive the confirmation from a connection request */
extern int t_rcvconnect __P((int fd, struct t_call * call));
/* TLI Library Function: t_rcvdis - retrieve information from disconnect */
extern int t_rcvdis __P((int fd, struct t_discon * discon));
/* TLI Library Function: t_rcvrel - acknowledge receipt of an orderly release indication */
extern int t_rcvrel __P((int fd));
/* TLI Library Function: t_rcvudata - receive a data unit */
extern int t_rcvudata __P((int fd, struct t_unitdata * unitdata, int *flags));
/* TLI Library Function: t_rcvuderr - receive a unit data error indication */
extern int t_rcvuderr __P((int fd, struct t_uderr * uderr));
/* TLI Library Function: t_snd - send data or expedited data over a connection */
extern int t_snd __P((int fd, char *buf, unsigned nbytes, int flags));
/* TLI Library Function: t_snddis - send user-initiated disconnect request */
extern int t_snddis __P((int fd, struct t_call * call));
/* TLI Library Function: t_sndrel - initiate an orderly release */
extern int t_sndrel __P((int fd));
/* TLI Library Function: t_sndudata - send a data unit */
extern int t_sndudata __P((int fd, struct t_unitdata * unitdata));
/* TLI Library Function: t_strerror - generate error message string */
extern char *t_strerror __P((int));
/* TLI Library Function: t_sync - synchronise transport library */
extern int t_sync __P((int fd));
/* TLI Library Function: t_unbind - disable a transport endpoint */
extern int t_unbind __P((int fd));
/* TLI Library - deprecated direct access to error list */
extern char *t_errlist[];
/* TLI Library - deprecated length of error list */
extern int t_nerr;

#endif				/* __KERNEL__ */

#endif				/* _SYS_TIUSER_H */
