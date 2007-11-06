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

#ifndef __CL_H_TP4_H__
#define __CL_H_TP4_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* tp4.h - TP4 abstractions */

/* 
 * Header: /f/iso/h/RCS/tp4.h,v 5.0 88/07/21 14:39:38 mrose Rel
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

#ifndef	_TP4_
#define	_TP4_

/* TLI */

#if	defined (WIN)
#define	TLI
#endif

#ifdef	TLI
#include <tiuser.h>

				/* why aren't these in <tiuser.h>? */

#define	T_NULL	0x00

extern int t_errno;

int t_close();

#define	TLIMAXCONN	1
#endif

/* SunLink OSI */

#ifdef SUN_TP4
#include <sys/ieee802.h>
#ifndef	SOCK_STREAM
#include <sys/socket.h>
#endif
#include <netosi/osi.h>
#include <netosi/osi_addr.h>
#include <netosi/osi_error.h>
#include <netosi/tp_event.h>

#define	MSG_OOB         0x1	/* process out-of-band data */

struct tp4pkt {
	union {
		TP_MSG tp_msg;
		TP_MSG_CONNECT tp_connect;
		TP_MSG_DATA tp_data;
		TP_MSG_X_DATA tp_x_data;
		TP_MSG_DISCONNECT tp_disconnect;
	} tp_un;
#define tp4_event	tp_un.tp_msg.tp_event
#define tp4_called 	tp_un.tp_connect.dst_address
#define tp4_calling 	tp_un.tp_connect.src_address
#define tp4_expedited	tp_un.tp_connect.expedited_selected
#define tp4_qos		tp_un.tp_connect.tp_qos
#define tp4_eot		tp_un.tp_data.eot
#define tp4_reason	tp_un.tp_disconnect.reason
};

struct tp4pkt *newtp4pkt();

#define	freetp4pkt(tp)	cfree ((char *) (tp))

int gen2tp4(), tp42gen();
#endif

#ifdef	TLI
#define	close_tp4_socket	t_close
#define	select_tp4_socket	selsocket
#endif

#ifdef	SUN_TP4
#define	close_tp4_socket	close
#define	select_tp4_socket	selsocket
#endif

int close_tp4_socket();
int select_tp4_socket();

#endif

#endif				/* __CL_H_TP4_H__ */
