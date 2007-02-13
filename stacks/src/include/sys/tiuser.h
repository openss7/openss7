/*****************************************************************************

 @(#) $Id: tiuser.h,v 0.9.2.5 2007/02/13 14:05:30 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/02/13 14:05:30 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tiuser.h,v $
 Revision 0.9.2.5  2007/02/13 14:05:30  brian
 - corrected ulong and long for 32-bit compat

 Revision 0.9.2.4  2006/09/18 13:52:37  brian
 - added doxygen markers to sources

 Revision 0.9.2.3  2006/03/07 01:05:00  brian
 - rationalized to strxnet tiuser.h

 *****************************************************************************/

#ifndef _SYS_TIUSER_H
#define _SYS_TIUSER_H

#ident "@(#) $RCSfile: tiuser.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * Transport Interface User header file.  */

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

#endif				/* _SYS_TIUSER_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
