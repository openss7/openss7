/*****************************************************************************

 @(#) $RCSfile: xti.h,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/04/06 12:33:12 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; either version 2.1 of the License, or (at your option)
 any later version.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/06 12:33:12 $ by $Author: brian $

 $Log: xti.h,v $
 Revision 0.9.2.2  2004/04/06 12:33:12  brian
 - Working up header files.

 Revision 0.9.2.1  2004/04/05 12:39:05  brian
 - Working up XNET release.

 Revision 0.9  2004/04/03 22:37:08  brian
 - Initial cut of new strxnet rpm release.

 Revision 0.9  2004/04/03 12:44:17  brian
 - Initial cut of new strinet package.

 Revision 0.9.4.1  2004/01/12 23:32:44  brian
 - Updated LiS-2.16.18 gcom release to autoconf.

 Revision 0.9.2.3  2004/01/07 11:34:12  brian
 - Updated copyright dates.

 Revision 0.9.2.2  2003/12/23 09:59:48  brian
 - Added base header files.

 Revision 0.9.2.1  2003/12/23 05:46:43  brian
 - Updates to XTI library header files.

 *****************************************************************************/

#ifndef _XTI_H
#define _XTI_H

#ident "@(#) $RCSfile: xti.h,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/04/06 12:33:12 $"

#ifdef __KERNEL__
#error **** 
#error **** This is a user-space header file.  It should never be included by
#error **** kernel modules.  You should include <sys/xti.h> instead.
#error **** 
#endif

#include <unistd.h>
#include <features.h>

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

#include <sys/xti.h>

/* 
 * extern int t_errno;
 * 
 * XTI error return.  t_errno is a modifiable lvalue of type int.  The above
 * definition is typical of a single-threaded environment.  In a
 * multi-threading environment a typical definition of t_errno is:
 */
extern int *_t_errno(void);
#define t_errno (*(_t_errno()))

/* 
 * iov maximum
 */
#define T_IOV_MAX  16		/* Maximum number of scatter/gather buffers. The value is not
				   mandatory.  The value must be at least 16. */

/* 
 *  Definitions for t_sysconf
 */
#ifndef _SC_T_IOV_MAX
#define _SC_T_IOV_MAX	1
#endif

/* 
 *  IO vector structure.
 */
struct t_iovec {
	void *iov_base;			/* The base of the vector component. */
	size_t iov_len;			/* The length of the vector component. */
};

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
#if defined _XOPEN_SOURCE
	t_scalar_t flags;		/* Other info about the transport provider. */
	t_scalar_t tidu;		/* max size of interface transport data unit. */
#endif
};

/* 
 * netbuf structure.
 */
struct netbuf {
	unsigned int maxlen;
	unsigned int len;
	char *buf;
};

#if 0
#undef t_opthdr
/* 
 * t_opthdr structure
 */
struct t_opthdr {
	t_uscalar_t len;		/* total length of option; i.e, sizeof (struct t_opthdr) +
					   length of option value in bytes */
	t_uscalar_t level;		/* protocol affected */
	t_uscalar_t name;		/* option name */
	t_uscalar_t status;		/* status value */
	/* followed by option value(s) */
};
#endif

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
 * Leaf status structure.
 */
struct t_leaf_status {
	int leafid;			/* leaf whose address has changed */
	int status;			/* status: T_CONNECT or T_DISCONNECT */
	int reason;			/* disconnect reason */
};

/* 
 * The following are structure types used when dynamically allocating the
 * above structures via t_alloc().
 */
#if defined _XPG4_2 || defined _XOPEN_SOURCE || defined __KERNEL__
#define T_BIND		1	/* allocate t_bind structure */
#define T_OPTMGMT	2	/* allocate t_optmgmt structure */
#define T_CALL		3	/* allocate t_call structure */
#define T_DIS		4	/* allocate t_discon structure */
#define T_UNITDATA	5	/* allocate t_unitdata structure */
#define T_UDERROR	6	/* allocate t_uderr structure */
#define T_INFO		7	/* allocate t_info structure */
#else				/* defined _XPG4_2 || defined _XOPEN_SOURCE || defined __KERNEL__ */
#define T_BIND_STR	1	/* allocate t_bind structure */
#define T_OPTMGMT_STR	2	/* allocate t_optmgmt structure */
#define T_CALL_STR	3	/* allocate t_call structure */
#define T_DIS_STR	4	/* allocate t_discon structure */
#define T_UNITDATA_STR	5	/* allocate t_unitdata structure */
#define T_UDERROR_STR	6	/* allocate t_uderr structure */
#define T_INFO_STR	7	/* allocate t_info structure */
#endif				/* defined _XPG4_2 || defined _XOPEN_SOURCE || defined __KERNEL__ */

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
#define T_FAKE		8
#define T_HACK		12

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

#ifndef __P
#ifdef __STDC__
#define __P(x)	x
#else
#define __P(x)	()
#endif
#endif

/* 
 *  XTI LIBRARY FUNCTIONS
 */
/* XTI Library Function: t_accept - accept a connection request */
extern int t_accept __P((int, int, const struct t_call *));
/* XTI Library Function: t_addleaf - add a leaf to point to multipoint connection */
extern int t_addleaf __P((int, int, struct netbuf *));
/* XTI Library Function: t_alloc - allocate a library structure */
extern char *t_alloc __P((int, int, int));
/* XTI Library Function: t_bind - bind an address to a transport endpoint */
extern int t_bind __P((int, const struct t_bind *, struct t_bind *));
/* XTI Library Function: t_close - close a transport endpoint */
extern int t_close __P((int));
/* XTI Library Function: t_connect - establish a connection */
extern int t_connect __P((int, const struct t_call *, struct t_call *));
/* XTI Library Function: t_error - produce error message */
extern int t_error __P((const char *));
/* XTI Library Function: t_free - free a library structure */
extern int t_free __P((void *, int));
/* XTI Library Function: t_getinfo - get protocol-specific service information */
extern int t_getinfo __P((int, struct t_info *));
/* XTI Library Function: t_getprotaddr - get protocol addresses */
extern int t_getprotaddr __P((int, struct t_bind *, struct t_bind *));
/* XTI Library Function: t_getstate - get the current state */
extern int t_getstate __P((int));
/* XTI Library Function: t_listen - listen for a connection indication */
extern int t_listen __P((int, struct t_call *));
/* XTI Library Function: t_look - look at current event on a transport endpoint */
extern int t_look __P((int));
/* XTI Library Function: t_open - establish a transport endpoint */
extern int t_open __P((const char *, int, struct t_info *));
/* XTI Library Function: t_optmgmt - manage options for a transport endpoint */
extern int t_optmgmt __P((int, const struct t_optmgmt *, struct t_optmgmt *));
/* XTI Library Function: t_rcv - receive data or expedited data on a connection */
extern int t_rcv __P((int, char *, unsigned int, int *));
/* XTI Library Function: t_rcvconnect - receive the confirmation from a connection request */
extern int t_rcvconnect __P((int, struct t_call *));
/* XTI Library Function: t_rcvdis - retrieve information from disconnect */
extern int t_rcvdis __P((int, struct t_discon *));
/* XTI Library Function: t_rcvleafchange - acknowledge receipt of a leaf change indication */
extern int t_rcvleafchange __P((int, struct t_leaf_status *));
/* XTI Library Function: t_rcvrel - acknowledge receipt of an orderly release indication */
extern int t_rcvrel __P((int));
/* XTI Library Function: t_rcvreldata - receive an orderly release indication or confirmation
   containing user data */
extern int t_rcvreldata __P((int, struct t_discon *));
/* XTI Library Function: t_rcvudata - receive a data unit */
extern int t_rcvudata __P((int, struct t_unitdata *, int *));
/* XTI Library Function: t_rcvuderr - receive a unit data error indication */
extern int t_rcvuderr __P((int, struct t_uderr *));
/* XTI Library Function: t_rcvv - receive data or expedited data sent over a connection and put the 
   data into one or more noncontiguous buffers */
extern int t_rcvv __P((int, struct t_iovec *, unsigned int, int *));
/* XTI Library Function: t_rcvvudata - receive a data unit into one or more noncontiguous buffers */
extern int t_rcvvudata __P((int, struct t_unitdata *, struct t_iovec *, unsigned int, int *));
/* XTI Library Function: t_removeleaf - remove a leaf from a point to multipoint connection */
extern int t_removeleaf __P((int, int, int));
/* XTI Library Function: t_snd - send data or expedited data over a connection */
extern int t_snd __P((int, char *, unsigned int, int));
/* XTI Library Function: t_snddis - send user-initiated disconnect request */
extern int t_snddis __P((int, const struct t_call *));
/* XTI Library Function: t_sndrel - initiate an orderly release */
extern int t_sndrel __P((int));
/* XTI Library Function: t_sndreldata - initiate or respond to an orderly release with user data */
extern int t_sndreldata __P((int, struct t_discon *));
/* XTI Library Function: t_sndudata - send a data unit */
extern int t_sndudata __P((int, const struct t_unitdata *));
/* XTI Library Function: t_sndv - send data or expedited data, from one or more noncontiguous
   buffers, on a connection */
extern int t_sndv __P((int, const struct t_iovec *, unsigned int, int));
/* XTI Library Function: t_sndvudata - send a data unit from one or more non-contiguous buffers */
extern int t_sndvudata __P((int, struct t_unitdata *, struct t_iovec *, unsigned int));
/* XTI Library Function: t_strerror - generate error message string */
extern const char *t_strerror __P((int));
/* XTI Library Function: t_sync - synchronise transport library */
extern int t_sync __P((int));
/* XTI Library Function: t_sysconf - get configurable XTI variables */
extern int t_sysconf __P((int));
/* XTI Library Function: t_unbind - disable a transport endpoint */
extern int t_unbind __P((int));

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _XTI_H */
