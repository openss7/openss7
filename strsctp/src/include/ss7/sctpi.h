/*****************************************************************************

 @(#) $Id: sctpi.h,v 0.9 2004/06/20 20:35:47 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/06/20 20:35:47 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SCTPI_H__
#define __SCTPI_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

/*
 *  This is a STREAMS-based definition of the SCTP (Stream Control
 *  Transmission Protocol) RFC 2960.  It is based as closely as possible on
 *  the UNIX International, Inc. TLI (Transport Layer Interface) and the XTI
 *  (X/Open Transport Interface).  This interface was developed for the
 *  OpenSS7 package and implementation of the SIGTRAN protocols M2PA, M2UA,
 *  M3UA and SUA.
 */

typedef unsigned long sctp_ulong;
typedef unsigned short sctp_ushort;
typedef unsigned char sctp_uchar;

#define SCTP_CONN_REQ		 0	/* ASSOCIATE */
#define SCTP_CONN_RES		 1	/* ASSOCIATE */
#define SCTP_DISCON_REQ		 2	/* ABORT */
#define SCTP_DATA_REQ		 3	/* SEND */
#define SCTP_EXDATA_REQ		 4	/* SEND */
#define SCTP_INFO_REQ		 5	/* STATUS */
#define SCTP_BIND_REQ		 6	/* INITIALIZE */
#define SCTP_UNBIND_REQ		 7	/* DESTROY */
#define SCTP_UNITDATA_REQ	 8	/* SEND */
#define SCTP_OPTMGMT_REQ	 9	/* SETPRIMARY */
#define SCTP_ORDREL_REQ		10	/* SHUTDOWN */

#define SCTP_CONN_IND		11	/* incoming association */
#define SCTP_CONN_CON		12	/* ASSOCIATE result */
#define SCTP_DISCON_IND		13	/* received ABORT */
#define SCTP_DATA_IND		14	/* RECEIVE */
#define SCTP_EXDATA_IND		15	/* RECEIVE */
#define SCTP_INFO_ACK		16	/* STATUS result */
#define SCTP_BIND_ACK		17	/* INITIALIZE result */
#define SCTP_ERROR_ACK		18	/* any error result */
#define SCTP_OK_ACK		19	/* any ok result */
#define SCTP_UNITDATA_IND	20	/* RECEIVE */
#define SCTP_UDERROR_IND	21	/* RECEIVE */
#define SCTP_OPTMGMT_ACK	22	/* SETPRIMARY result */
#define SCTP_ORDREL_IND		23	/* recevied SHUTDOWN */

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_CONN_REQ */
	sctp_ulong sctp_dest_length;	/* assoc id and destination transport address length */
	sctp_ulong sctp_dest_offset;	/* offset in this message */
	sctp_ulong sctp_opt_length;	/* options includes number of streams */
	sctp_ulong sctp_opt_offset;	/* offset in this messsage */
} sctp_conn_req_t;

/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/

#define SCTP_INITIALIZE_REQ	0
#define SCTP_INITIALIZE_CON	1
#define SCTP_ASSOCIATE_REQ	2
#define SCTP_ASSOCIATE_CON	3
#define SCTP_SHUTDOWN_REQ	5
#define SCTP_SHUTDOWN_CON	6
#define SCTP_ABORT_REQ		7
#define SCTP_ERROR_ACK		8

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_OK_ACK */
	sctp_ulong sctp_state;		/* state of the interface */
	sctp_long sctp_correct_prim;	/* correct primitive */
};

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_ERR_ACK */
	sctp_ulong sctp_state;		/* state of the interface */
	sctp_long sctp_error_prim;	/* primitive in error */
	sctp_ulong sctp_error;		/* error code */
} sctp_err_ack_t;

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_INITIALIZE_REQ */
	sctp_ushort sctp_port;		/* local port number or 0 for any port */
	sctp_ushort sctp_num_addr;	/* number of local addresses in list */
	sctp_ulong sctp_addrs[0];	/* list of local addresses */
} sctp_initialize_req_t;
/*
   followed by list of addresses 
 */

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_INITIALIZE_CON */
	sctp_ulong sctp_state;		/* state of the interface */
	sctp_ulong sctp_instance_id;	/* instance id */
} sctp_initialize_con_t;

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_ASSOCIATE_REQ */
	sctp_ulong sctp_instance_id;	/* instance identifier */
	sctp_dta_t sctp_dta;		/* destination transport address */
	sctp_ulong sctp_streams;	/* number of outbound streams requested */
} sctp_associate_req_t;

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_ASSOCIATE_CON */
	sctp_ulong sctp_state;		/* state of the interface */
	sctp_ulong sctp_assoc_id;	/* association identifier */
	sctp_ulong sctp_streams;	/* number of outgoing streams provided */
	sctp_ulong sctp_num_dtas;	/* number of destination transport addrs */
	sctp_dta_t sctp_dtas[0];	/* list of destination transport addresses */
} sctp_associate_con_t;
/*
   followed by list of destination transport addresses 
 */

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_SHUTDOWN_REQ */
	sctp_ulong sctp_assoc_id;	/* association identifier */
} sctp_shutdown_req_t;

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_SHUTDOWN_CON */
	sctp_ulong sctp_state;		/* state of the interface */
} sctp_shutdown_con_t;

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_ABORT_REQ */
	sctp_ulong sctp_assoc_id;	/* association id */
	sctp_ulong sctp_cause_code;	/* cause code, 0 if none */
} sctp_abort_req_t;

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_SEND_REQ */
	sctp_ulong sctp_assoc_id;	/* association id */
	sctp_ulong sctp_context;	/* context id of message, 0 if none */
	sctp_ulong sctp_stream_id;	/* stream to send on, 0 if none */
	sctp_ulong sctp_lifetime;	/* lifetime of user data, 0 if none */
	sctp_dta_t sctp_dta;		/* dta to send on, 0 if none */
	sctp_ulong sctp_flags;		/* delivery flags */
	sctp_ulong sctp_payload;	/* payload protocol id */
} sctp_send_req_t;

#define SCTP_FLAG_UNORDER   0x00000001	/* unordered delivery */
#define SCTP_FLAG_NO_BUNDLE 0x00000002	/* do not bundle with other user data */

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_RECEIVE_IND */
	sctp_ulong sctp_assoc_id;	/* association id */
	sctp_ulong sctp_stream_id;	/* stream buffer received on */
	sctp_ulong sctp_ssn;		/* stream sequence number */
	sctp_ulong sctp_flags;		/* reception flags */
	sctp_ulong sctp_dn;		/* delivery number (?) */
	sctp_ulong sctp_payload;	/* payload protocol id */
} sctp_receive_ind_t;

#define SCTP_FLAG_PARTIAL   0x00000004	/* partial data received */

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_SET_PRIMARY_REQ */
	sctp_ulong sctp_assoc_id;	/* association id */
	sctp_dta_t sctp_dta;		/* destination transport address */
	sctp_dta_t sctp_sta;		/* source transport address */
} sctp_set_primary_req_t;

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_STATUS_REQ */
	sctp_ulong sctp_assoc_id;	/* association id */
} sctp_status_req_t;

typedef struct {
	sctp_dta_t sctp_dta;		/* path dta */
	sctp_dta_t sctp_sta;		/* path sta */
	sctp_ulong sctp_srtt;		/* most recent SRTT on primary path */
	sctp_ulong sctp_rto;		/* RTO on primary path */
} sctp_pdat_t;

typedef struct {
	sctp_long sctp_primitive;	/* SCTP_STATUS_ACK */
	sctp_ulong sctp_assoc_state;	/* association connection state */
	sctp_ulong sctp_rwd;		/* current receive window size */
	sctp_ulong sctp_cwnd;		/* current congestion window size */
	sctp_ulong sctp_num_unack;	/* number of unacknowledged data chunks */
	sctp_ulong sctp_num_pending;	/* number of data chunks pending receipt */
	sctp_pdat_t sctp_primary;	/* primary path data */
	sctp_ulong sctp_num_dtas;	/* number of destination transport addresses */
	sctp_pdat_t sctp_paths[0];	/* other path data */
} sctp_status_ack_t;

#endif				/* __SCTPI_H__ */
