/*****************************************************************************

 @(#) $Id: lki.h,v 0.9.2.2 2005/05/14 08:30:45 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:30:45 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_LKI_H__
#define __SS7_LKI_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

typedef lmi_long lk_long;
typedef lmi_ulong lk_ulong;
typedef lmi_ushort lk_ushort;
typedef lmi_uchar lk_uchar;

#define LK_PROTO_BASE				   80L

#define LK_DSTR_FIRST				(   1L + LK_PROTO_BASE )
#define LK_MESSAGE_FOR_DISTRIBUTION_REQ		(   2L + LK_PROTO_BASE )
#define LK_MESSAGE_FOR_ROUTING_REQ		(   3L + LK_PROTO_BASE )
#define LK_NETWORK_CONFIGURATION_CHANGE_REQ	(   4L + LK_PROTO_BASE )
#define LK_MANAGEMENT_INHIBIT_REQ		(   5L + LK_PROTO_BASE )
#define LK_MANAGEMENT_UNINHIBIT_REQ		(   6L + LK_PROTO_BASE )
#define LK_ROUTE_UNINHIBIT_REQ			(   7L + LK_PROTO_BASE )
#define LK_DSTR_LAST				(   7L + LK_PROTO_BASE )

#define LK_USTR_LAST				(  -1L - LK_PROTO_BASE )
#define LK_MESSAGE_FOR_DISTRIBUTION_IND		(  -1L - LK_PROTO_BASE )
#define LK_MESSAGE_FOR_ROUTING_IND		(  -2L - LK_PROTO_BASE )
#define LK_MANAGEMENT_INHIBITED_IND		(  -3L - LK_PROTO_BASE )
#define LK_MANAGEMENT_INHIBIT_DENIED_IND	(  -4L - LK_PROTO_BASE )
#define LK_MANAGEMENT_UNINHIBIT_DENIED_IND	(  -5L - LK_PROTO_BASE )
#define LK_ROUTE_UNINHIBIT_ABANDONED_IND	(  -6L - LK_PROTO_BASE )
#define LK_TIME_CONTROLLED_CHANGEOVER_IND	(  -7L - LK_PROTO_BASE )
#define LK_USTR_FIRST				(  -7L - LK_PROTO_BASE )

/*
 *  LKI PROTOCOL PRIMITIVES
 */

#define LK_REASON_UNSPECIFIED		0	/* unspecified (unknown) */
#define LK_REASON_INHIBITED		1	/* the link is inhibited */
#define LK_REASON_UNINHIBITED		2	/* the link is uninhibited */
#define LK_REASON_INHIBITING		3	/* inhibiting in progress */
#define LK_REASON_UNINHIBITING		4	/* uninhibiting in progress */
#define LK_REASON_BUSY			5	/* the action is already in progress */
#define LK_REASON_INACCESSIBLE		6	/* the adjacent SP is inaccessible */
#define LK_REASON_CRITICAL		7	/* the link is critical to a route */
#define LK_REASON_MAXIMUM_RETRIES	8	/* maximum number of retries attempted */

/*
 *  LK_MANAGEMENT_INHIBIT_DENIED_IND, M_PROTO
 */
typedef struct {
	lk_long primitive;		/* LK_MANAGEMENT_INHIBIT_DENIED_IND */
	lk_ulong reason;		/* reason for denying inhibit */
} lk_local_inhibit_denied_ind_t;

/*
 *  LK_MANAGEMENT_UNINHIBIT_DENIED_IND, M_PROTO
 */
typedef struct {
	lk_long primitive;		/* LK_MANAGEMENT_UNINHIBIT_DENIED_IND */
	lk_ulong reason;		/* reason for denying inhibit */
} lk_management_uninhibit_denied_ind_t;

/*
 *  LK_MSU_REQ, options M_PROTO type, with M_DATA block(s)
 */

typedef struct {
	lk_long lk_primitive;
	lk_ulong lk_network_ind;	/* network indicator */
	lk_ulong lk_msg_prior;		/* message priority */
	lk_ulong lk_service_ind;	/* service indicator */
	lk_ulong lk_opc;		/* originating point code */
	lk_ulong lk_dpc;		/* destination point code */
	lk_ulong lk_sls;		/* signalling link selection */
} lk_msu_req_t;

union LK_primitives {
	lk_long lk_primitive;
	lk_msu_req_t msu_req;
};

#define LK_MSU_REQ_SIZE			sizeof(lk_msu_req_t)

#define LK_PRIM_MAX_SIZE		sizeof(LK_primitives)

#endif				/* __SS7_LKI_H__ */
