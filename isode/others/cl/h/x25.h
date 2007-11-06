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

#ifndef __CL_H_X25_H__
#define __CL_H_X25_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* x25.h - X.25 abstractions */

/* 
 * Header: /f/iso/h/RCS/x25.h,v 5.0 88/07/21 14:39:42 mrose Rel
 *
 * Contributed by John Pavel, Department of Trade and Industry/National
 * Physical Laboratory in the UK
 *
 *
 * Log
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef	_X25_
#define	_X25_

/* 
 *
 * #defines for generic addressing & TSEL encoded addresses.
 *
 */

#ifdef	SUN_X25
#include <sys/ioctl.h>
#include <sys/param.h>
#undef	min
#undef	max
#include <sys/systm.h>
#undef	insque
#undef	remque
#include <sys/mbuf.h>
#include <sundev/syncstat.h>
#include <netx25/x25_pk.h>
#include <netx25/x25_ctl.h>
#include <netx25/x25_ioctl.h>
#endif

#ifndef	_INTERNET_
#include <sys/socket.h>
#endif

#include <sys/protosw.h>
#include <sys/domain.h>
#include <sys/socketvar.h>
#include <sys/errno.h>
#include <net/if.h>

#ifdef	CAMTEC
#include <cci.h>
typedef struct ccontrolp CONN_DB;

#undef  NTPUV
#define	NTPUV	2		/* CAMTEC allows only 2 iov's per read/write */
#endif

#ifdef	CAMTEC_CCL
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netccl/ccl.h>

typedef struct ccliovec CONN_DB;
#endif

#ifdef UBC_X25
#include <netccitt/x25_sockaddr.h>

typedef struct x25_sockaddr CONN_DB;

#endif

#ifdef	SUN_X25
#define close_x25_socket     close
#define	select_x25_socket    selsocket
#define read_x25_socket      read
#define write_x25_socket     write
#endif

#if	defined(UBC_X25) || defined(CAMTEC_CCL)
#define close_x25_socket     close
#define	select_x25_socket    selsocket
#endif

int start_x25_client();
int start_x25_server();
int join_x25_client();
int join_x25_server();
int read_x25_socket();
int write_x25_socket();
int close_x25_socket();
int select_x25_socket();

struct NSAPaddr *if2gen();
CONN_DB *gen2if();

#define	ADDR_LOCAL	0
#define	ADDR_REMOTE	1
#define	ADDR_LISTEN	2
#define SEPARATOR ':'

#define MAXNSDU	1024
#endif

#endif				/* __CL_H_X25_H__ */
