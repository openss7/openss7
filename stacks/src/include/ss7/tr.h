/*****************************************************************************

 @(#) $Id: tr.h,v 0.9.2.2 2004/08/30 06:19:39 brian Exp $

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

 Last Modified $Date: 2004/08/30 06:19:39 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_TR_H__
#define __SS7_TR_H__

#ident "@(#) $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#define TR_INFO_REQ		 0	/* Information request */
#define TR_BIND_REQ		 1	/* Bind to network address */
#define TR_UNBIND_REQ		 2	/* Unbind from network address */
#define TR_OPTMGMT_REQ		 5	/* Options management request */
#define	TR_UNI_REQ		 6	/* Unidirectional request */
#define TR_BEGIN_REQ		 7	/* Begin transaction request */
#define TR_BEGIN_RES		 8	/* Begin transaction response-Continue request */
#define	TR_CONT_REQ		 9	/* Continue transaction request */
#define TR_END_REQ		10	/* End transaction request */
#define TR_ABORT_REQ		11	/* Abort transaction request */

#define TR_INFO_ACK		12	/* Information acknowledgement */
#define TR_BIND_ACK		13	/* Bound to network address */
#define TR_OK_ACK		15	/* Success acknowledgement */
#define TR_ERROR_ACK		16	/* Error acknowledgement */
#define TR_OPTMGMT_ACK		17	/* Options management acknowledgement */
#define	TR_UNI_IND		18	/* Unidirectional indication */
#define TR_BEGIN_IND		19	/* Begin transaction indication */
#define TR_BEGIN_CON		20	/* Begin transaction confirmation-Continue ind */
#define TR_CONT_IND		21	/* Continue transaction indication */
#define TR_END_IND		22	/* End transaction indication */
#define TR_ABORT_IND		23	/* Abort transaction indication */
#define TR_NOTICE_IND		24	/* Error indication */

#define TR_QOS_SEL1		0x0501

typedef struct {
	ulong type;			/* Always TR_QOS_SEL1 */
	ulong flags;			/* Return option */
	ulong seq_ctrl;			/* Sequence Control */
	ulong priority;			/* Message priority */
} TR_qos_sel1_t;

#define TR_PROVIDER		0x0001
#define TR_USER			0x0002

/*
 * TRPI interface states
 */
#define TRS_UNBND	0	/* TR user not bound to network address */
#define TRS_WACK_BREQ	1	/* Awaiting acknowledgement of N_BIND_REQ */
#define TRS_WACK_UREQ	2	/* Pending acknowledgement for N_UNBIND_REQ */
#define TRS_IDLE	3	/* Idle, no connection */
#define TRS_WACK_OPTREQ	4	/* Pending acknowledgement of N_OPTMGMT_REQ */
#define TRS_WACK_RRES	5	/* Pending acknowledgement of N_RESET_RES */
#define TRS_WCON_CREQ	6	/* Pending confirmation of N_CONN_REQ */
#define TRS_WRES_CIND	7	/* Pending response of N_CONN_REQ */
#define TRS_WACK_CRES	8	/* Pending acknowledgement of N_CONN_RES */
#define TRS_DATA_XFER	9	/* Connection-mode data transfer */
#define TRS_WCON_RREQ	10	/* Pending confirmation of N_RESET_REQ */
#define TRS_WRES_RIND	11	/* Pending response of N_RESET_IND */
#define TRS_WACK_DREQ6	12	/* Waiting ack of N_DISCON_REQ */
#define TRS_WACK_DREQ7	13	/* Waiting ack of N_DISCON_REQ */
#define TRS_WACK_DREQ9	14	/* Waiting ack of N_DISCON_REQ */
#define TRS_WACK_DREQ10	15	/* Waiting ack of N_DISCON_REQ */
#define TRS_WACK_DREQ11	16	/* Waiting ack of N_DISCON_REQ */

#define TRS_NOSTATES	17

/*
 * TR_ERROR_ACK error return code values
 */
#define TRBADADDR	1	/* Incorrect address format/illegal address information */
#define TRBADOPT	2	/* Options in incorrect format or contain illegal information */
#define TRACCESS	3	/* User did not have proper permissions */
#define TRNOADDR	5	/* TR Provider could not allocate address */
#define TROUTSTATE	6	/* Primitive was issues in wrong sequence */
#define TRBADSEQ	7	/* Sequence number in primitive was incorrect/illegal */
#define TRSYSERR	8	/* UNIX system error occurred */
#define TRBADDATA	10	/* User data spec. outside range supported by TR provider */
#define TRBADFLAG	16	/* Flags specified in primitive were illegal/incorrect */
#define TRNOTSUPPORT	18	/* Primitive type not supported by the TR provider */
#define TRBOUND		19	/* Illegal second attempt to bind listener or default listener */
#define TRBADQOSPARAM	20	/* QOS values specified are outside the range supported by the TR
				   provider */
#define TRBADQOSTYPE	21	/* QOS structure type specified is not supported by the TR provider 
				 */
#define TRBADTOKEN	22	/* Token used is not associated with an open stream */
#define TRNOPROTOID	23	/* Protocol id could not be allocated */

/*
 *  TR_INFO_REQ.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_info_req {
	ulong PRIM_type;		/* Always TR_INFO_REQ */
} TR_info_req_t;

/*
 *  TR_INFO_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_info_ack {
	long PRIM_type;			/* Always TR_INFO_ACK */
	long TSDU_size;			/* maximum TSDU size */
	long ETSDU_size;		/* maximum ETSDU size */
	long CDATA_size;		/* connect data size */
	long DDATA_size;		/* discon data size */
	long ADDR_size;			/* address size */
	long OPT_size;			/* options size */
	long TIDU_size;			/* transaction i/f data unit size */
	long SERV_type;			/* service type */
	long CURRENT_state;		/* current state */
	long PROVIDER_flag;		/* type of TR provider */
	long TRPI_version;		/* version # of trpi that is supported */
} TR_info_ack_t;

/*
 *  TR_BIND_REQ.  This primitive consists of one M_PROTO message block.
 */
typedef struct TR_bind_req {
	ulong PRIM_type;		/* Always TR_BIND_REQ */
	ulong ADDR_length;		/* address length */
	ulong ADDR_offset;		/* address offset */
	ulong XACT_number;		/* maximum outstanding transaction reqs. */
	ulong BIND_flags;		/* bind flags */
} TR_bind_req_t;

/*
 *  TR_BIND_ACK.  This primitive consists of one M_PROTO message block.
 */
typedef struct TR_bind_ack {
	ulong PRIM_type;		/* Always TR_BIND_ACK */
	ulong ADDR_length;		/* address length */
	ulong ADDR_offset;		/* address offset */
	ulong XACT_number;		/* open transactions */
	ulong TOKEN_value;		/* value of "token" assigned to stream */
} TR_bind_ack_t;

/*
 *  TR_UNBIND_REQ.  This primtive consists of one M_PROTO message block.
 */
typedef struct TR_unbind_req {
	ulong PRIM_type;		/* Always TR_UNBIND_REQ */
} TR_unbind_req_t;

/*
 *  TR_OPTMGMT_REQ.  This primtive consists of one M_PROTO message block.
 */
typedef struct TR_optmgmt_req {
	ulong PRIM_type;		/* Always T_OPTMGMT_REQ */
	ulong OPT_length;		/* options length */
	ulong OPT_offset;		/* options offset */
	ulong MGMT_flags;		/* options data flags */
} TR_optmgmt_req_t;

/*
 *  TR_OPTMGMT_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_optmgmt_ack {
	ulong PRIM_type;		/* Always T_OPTMGMT_ACK */
	ulong OPT_length;		/* options length */
	ulong OPT_offset;		/* options offset */
	ulong MGMT_flags;		/* options data flags */
} TR_optmgmt_ack_t;

/*
 *  TR_OK_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_ok_ack {
	ulong PRIM_type;		/* Always T_OK_ACK */
	ulong CORRECT_prim;		/* correct primitive */
} TR_ok_ack_t;

/*
 *  TR_ERROR_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_error_ack {
	ulong PRIM_type;		/* Always T_ERROR_ACK */
	ulong ERROR_prim;		/* primitive in error */
	ulong TRPI_error;		/* TRPI error code */
	ulong UNIX_error;		/* UNIX error code */
	ulong TRANS_id;			/* Transaction id */
} TR_error_ack_t;

/*
 *  TR_UNI_REQ.  This primitive consists of one M_PROTO message block followed
 *  by one or more M_DATA blocks.
 */
typedef struct TR_uni_req {
	ulong PRIM_type;		/* Always TR_UNI_REQ */
	ulong DEST_length;		/* Destination address length */
	ulong DEST_offset;		/* Destination address offset */
	ulong ORIG_length;		/* Originating address length */
	ulong ORIG_offset;		/* Originating address offset */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_uni_req_t;

/*
 *  TR_UNI_IND.  This primitive consists of one M_PROTO message block followed
 *  by one or more M_DATA blocks.
 */
typedef struct TR_uni_ind {
	ulong PRIM_type;		/* Always TR_UNI_REQ */
	ulong DEST_length;		/* Destination address length */
	ulong DEST_offset;		/* Destination address offset */
	ulong ORIG_length;		/* Originating address length */
	ulong ORIG_offset;		/* Originating address offset */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_uni_ind_t;

/*
 *  TR_BEGIN_REQ.
 */
typedef struct TR_begin_req {
	ulong PRIM_type;		/* Always TR_BEGIN_REQ */
	ulong CORR_id;			/* Correlation id */
	ulong DEST_length;		/* Destination address length */
	ulong DEST_offset;		/* Destination address offset */
	ulong ORIG_length;		/* Originating address length */
	ulong ORIG_offset;		/* Originating address offset */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_begin_req_t;

/*
 *  TR_BEGIN_IND.
 */
typedef struct TR_begin_ind {
	ulong PRIM_type;		/* Always TR_BEGIN_IND */
	ulong TRANS_id;			/* Transaction id */
	ulong DEST_length;		/* Destination address length */
	ulong DEST_offset;		/* Destination address offset */
	ulong ORIG_length;		/* Originating address length */
	ulong ORIG_offset;		/* Originating address offset */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_begin_ind_t;

/*
 *  TR_BEGIN_RES.
 *
 *  This primitive represents the first TR-CONTINUE response to a TR-BEGIN
 *  indication.
 */
typedef struct TR_begin_res {
	ulong PRIM_type;		/* Always TR_BEGIN_RES */
	ulong TRANS_id;			/* Transaction id */
	ulong ORIG_length;		/* Originating address length */
	ulong ORIG_offset;		/* Originating address offset */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_begin_res_t;

/*
 *  TR_BEGIN_CON.
 *
 *  This primitive represents the first TR-CONTINUE configuration of a
 *  TR-BEGIN request.
 */
typedef struct TR_begin_con {
	ulong PRIM_type;		/* Always TR_BEGIN_CON */
	ulong CORR_id;			/* Correlation Id */
	ulong TRANS_id;			/* Transaction id */
	ulong ORIG_length;		/* Originating address length */
	ulong ORIG_offset;		/* Originating address offset */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_begin_con_t;

/*
 *  TR_CONT_REQ.
 */
typedef struct TR_cont_req {
	ulong PRIM_type;		/* Always TR_CONT_REQ */
	ulong TRANS_id;			/* Transaction id */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_cont_req_t;

/*
 *  TR_CONT_IND.
 */
typedef struct TR_cont_ind {
	ulong PRIM_type;		/* Always TR_CONT_IND */
	ulong TRANS_id;			/* Transaction id */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_cont_ind_t;

/*
 *  TR_END_REQ.
 */
typedef struct TR_end_req {
	ulong PRIM_type;		/* Always TR_END_REQ */
	ulong TRANS_id;			/* Transaction id */
	ulong TERM_scenario;		/* Termination scenario */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_end_req_t;

/*
 *  TR_END_IND.
 */
typedef struct TR_end_ind {
	ulong PRIM_type;		/* Always TR_END_IND */
	ulong CORR_id;			/* Correlation id */
	ulong TRANS_id;			/* Transaction id */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_end_ind_t;

/*
 *  TR_ABORT_REQ.
 */
typedef struct TR_abort_req {
	ulong PRIM_type;		/* Always TR_ABORT_REQ */
	ulong TRANS_id;			/* Transaction id */
	ulong ABORT_cause;		/* Cause of the abort */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
} TR_abort_req_t;

/*
 *  TR_ABORT_IND.
 */
typedef struct TR_abort_ind {
	ulong PRIM_type;		/* Always TR_ABORT_IND */
	ulong CORR_id;			/* Correlation id */
	ulong TRANS_id;			/* Transaction id */
	ulong OPT_length;		/* Options structure length */
	ulong OPT_offset;		/* Options structure offset */
	ulong ABORT_cause;		/* Cause of the abort */
	ulong ORIGINATOR;		/* Originator P or U */
} TR_abort_ind_t;

/*
 *  TR_NOTICE_IND.
 */
typedef struct TR_notice_ind {
	ulong PRIM_type;		/* Always TR_NOTICE_IND */
	ulong CORR_id;			/* Correlation id */
	ulong TRANS_id;			/* Transaction id */
	ulong REPORT_cause;		/* SCCP return cause */
} TR_notice_ind_t;

#endif				/* __SS7_TR_H__ */
