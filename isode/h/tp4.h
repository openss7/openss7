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

#ifndef __ISODE_TP4_H__
#define __ISODE_TP4_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * Autoconf should make a bunch of this go away and most of these things should be driven by
 * autoconf test results rather than system type.  FIXME: do that.  --bb (2007-10-21)
 */

/* tp4.h - TP4 abstractions */

/* 
 * Header: /xtel/isode/isode/h/RCS/tp4.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 *
 * Log: tp4.h,v
 * Revision 9.0  1992/06/16  12:17:57  isode
 * Release 8.0
 *
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

/*  */

#ifdef	BSD_TP4
#ifndef	SOCK_STREAM
#include <sys/socket.h>
#endif
#include <netiso/iso.h>
#include <netiso/iso_errno.h>
#include <netiso/tp_user.h>

union sockaddr_osi {
	struct sockaddr_iso osi_sockaddr;
	char osi_bigaddr[104];
};

union osi_control_msg {
	struct {
		struct cmsghdr ocm_cmhdr;
		char ocm_cmdata[128 - sizeof(struct cmsghdr)];
	} ocm_control;

	char ocm_data[128];
};

int gen2tp4(), tp42gen();

#define	CLTS			/* have CL-mode transport service */

#ifndef	_DGRAM_
#include "dgram.h"
#endif

int start_clts_server();

#define	start_clts_client	start_clts_server

#define	join_clts_server(fd,sock) \
		join_dgram_aux ((fd), (struct sockaddr *) (sock), 0)
#define	join_clts_client(fd,sock) \
		join_dgram_aux ((fd), (struct sockaddr *) (sock), 1)

#define	read_clts_socket	read_dgram_socket
#define	write_clts_socket	write_dgram_socket
#define	close_clts_socket	close_dgram_socket

#define	select_clts_socket	select_dgram_socket
#define	check_clts_socket	check_dgram_socket
#endif

/*    SunLink OSI */

#ifdef SUN_TP4
#if	defined(SUNLINK_6_0) && !defined(SUNLINK_5_2)
#define	SUNLINK_5_2
#endif

#ifndef	SUNLINK_6_0
#include <sys/ieee802.h>
#else
#include <net/if_ieee802.h>
#endif
#ifndef	SOCK_STREAM
#include <sys/socket.h>
#endif
#include <netosi/osi.h>
#ifdef	SUNLINK_5_2
#include <netosi/osi_profile.h>
#endif
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

#ifdef XTI_TP
#include <xti.h>
#define select_tp4_socket selsocket
#endif

#ifdef TLI_TP
#include <tiuser.h>
#define select_tp4_socket selsocket
#endif

/*  */

#ifdef	BSD_TP4
#define	close_tp4_socket	close
#define	select_tp4_socket	selsocket
#endif

#ifdef	SUN_TP4
#define	close_tp4_socket	close
#define	select_tp4_socket	selsocket
#endif

int close_tp4_socket();
int select_tp4_socket();

#endif

#endif				/* __ISODE_TP4_H__ */
