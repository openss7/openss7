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

#ifndef __CL_H_CONS_H__
#define __CL_H_CONS_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* cons.h - CONS abstractions */

/* 
 * Header: /f/iso/h/RCS/cons.h,v 5.0 88/07/21 14:38:47 mrose Rel
 *
 * Contributed by Keith Ruttle, CAMTEC Electronics Ltd
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

#ifndef	_CONS_
#define	_CONS_

/* 
 *
 * #defines for generic addressing & TSEL encoded addresses.
 *
 */

#ifndef _INTERNET_
#include <sys/socket.h>
#endif
#include <sys/protosw.h>
#include <sys/domain.h>
#include <sys/socketvar.h>
#include <sys/errno.h>
#include <net/if.h>

#ifdef CAMTEC_CCL
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netccl/ccl.h>

typedef struct ccliovec CONN_DB;

#define close_cons_socket     close
#define	select_cons_socket    selsocket
#endif

int start_cons_client();
int start_cons_server();
int join_cons_client();
int join_cons_server();
int read_cons_socket();
int write_cons_socket();
int close_cons_socket();
int select_cons_socket();

struct NSAPaddr *if2gen();
CONN_DB *gen2if();

#define	ADDR_LOCAL	0
#define	ADDR_REMOTE	1
#define	ADDR_LISTEN	2
#define SEPARATOR ':'

#define MAXNSDU	1024
#endif

#endif				/* __CL_H_CONS_H__ */
