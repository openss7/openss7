/*****************************************************************************

 @(#) $Id: tr.h,v 0.9.2.10 2008-04-29 07:10:45 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-29 07:10:45 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tr.h,v $
 Revision 0.9.2.10  2008-04-29 07:10:45  brian
 - updating headers for release

 Revision 0.9.2.9  2007/08/13 19:09:37  brian
 - updated headers, formatting

 Revision 0.9.2.8  2007/08/03 13:35:01  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.7  2007/02/13 14:05:29  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __SS7_TR_H__
#define __SS7_TR_H__

#ident "@(#) $RCSfile: tr.h,v $ $Name:  $($Revision: 0.9.2.10 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

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
#define TR_ADDR_REQ		25	/* Address request */

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
#define TR_ADDR_ACK		27	/* Address acknowledgement */

#define TR_QOS_SEL1		0x0501

typedef struct {
	t_uscalar_t type;		/* Always TR_QOS_SEL1 */
	t_uscalar_t flags;		/* Return option */
	t_uscalar_t seq_ctrl;		/* Sequence Control */
	t_uscalar_t priority;		/* Message priority */
} TR_qos_sel1_t;

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
 *  TR_ABORT_IND originator
 */
#define TR_PROVIDER		0x0001
#define TR_USER			0x0002

/*
 *  TR_ABORT abort causes
 */
#define TR_ABTC_APPL_UNREC_MSG_TYPE		0x0100	/* unrecognized message type */
#define TR_ABTC_APPL_UNREC_TRANS_ID		0x0101	/* unrecognized transaction id */
#define TR_ABTC_APPL_BAD_XACT_PORTION		0x0102	/* badly formatted transaction portion */
#define TR_ABTC_APPL_INCORRECT_XACT_PORTION	0x0103	/* incorrect transaction portion */
#define TR_ABTC_APPL_RESOURCE_LIMITATION	0x0104	/* resource limitation */

#define TR_ABTC_PRIV_UNREC_PKG_TYPE		0x0201	/* unrecognized package type */
#define TR_ABTC_PRIV_INCORRECT_XACT_PORTION	0x0202	/* incorrect transaction portion */
#define TR_ABTC_PRIV_BAD_XACT_PORTION		0x0203	/* badly structured transaction portion */
#define TR_ABTC_PRIV_UNASSIGNED_RESP_TRANS_ID	0x0204	/* unassigned responding transaction id */
#define TR_ABTC_PRIV_PERM_TO_RELEASE_PROB	0x0205	/* permission to release problem */
#define TR_ABTC_PRIV_RESOURCE_UNAVAIL		0x0206	/* resource unavailable */
#define TR_ABTC_PRIV_UNREC_DIALOG_PORTION_ID	0x0207	/* unrecognized dialogue portion id */
#define TR_ABTC_PRIV_BAD_DIALOG_PORTION		0x0208	/* badly structured dialogue portion */
#define TR_ABTC_PRIV_MISSING_DIALOG_PORTION	0x0209	/* missing dialogue portion */
#define TR_ABTC_PRIV_INCONSIST_DIALOG_PORTION	0x020a	/* inconsistent dialogue portion */

/*
 *  TR_INFO_REQ.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_info_req {
	t_uscalar_t PRIM_type;		/* Always TR_INFO_REQ */
} TR_info_req_t;

/*
 *  TR_INFO_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_info_ack {
	t_scalar_t PRIM_type;		/* Always TR_INFO_ACK */
	t_scalar_t TSDU_size;		/* maximum TSDU size */
	t_scalar_t ETSDU_size;		/* maximum ETSDU size */
	t_scalar_t CDATA_size;		/* connect data size */
	t_scalar_t DDATA_size;		/* discon data size */
	t_scalar_t ADDR_size;		/* address size */
	t_scalar_t OPT_size;		/* options size */
	t_scalar_t TIDU_size;		/* transaction i/f data unit size */
	t_scalar_t SERV_type;		/* service type */
	t_scalar_t CURRENT_state;	/* current state */
	t_scalar_t PROVIDER_flag;	/* type of TR provider */
	t_scalar_t TRPI_version;	/* version # of trpi that is supported */
} TR_info_ack_t;

/*
 *  TR_BIND_REQ.  This primitive consists of one M_PROTO message block.
 */
typedef struct TR_bind_req {
	t_uscalar_t PRIM_type;		/* Always TR_BIND_REQ */
	t_uscalar_t ADDR_length;	/* address length */
	t_uscalar_t ADDR_offset;	/* address offset */
	t_uscalar_t XACT_number;	/* maximum outstanding transaction reqs. */
	t_uscalar_t BIND_flags;		/* bind flags */
} TR_bind_req_t;

/*
 *  TR_BIND_ACK.  This primitive consists of one M_PROTO message block.
 */
typedef struct TR_bind_ack {
	t_uscalar_t PRIM_type;		/* Always TR_BIND_ACK */
	t_uscalar_t ADDR_length;	/* address length */
	t_uscalar_t ADDR_offset;	/* address offset */
	t_uscalar_t XACT_number;	/* open transactions */
	t_uscalar_t TOKEN_value;	/* value of "token" assigned to stream */
} TR_bind_ack_t;

/*
 *  TR_ADDR_REQ.  This primitive consists of one M_PROTO message block.
 */
typedef struct TR_addr_req {
	t_uscalar_t PRIM_type;		/* Always TR_ADDR_REQ */
	t_uscalar_t TRANS_id;		/* Transaction id */
} TR_addr_req_t;

/*
 *  TR_ADDR_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_addr_ack {
	t_uscalar_t PRIM_type;		/* Always TR_ADDR_ACK */
	t_uscalar_t LOCADDR_length;	/* local address length */
	t_uscalar_t LOCADDR_offset;	/* local address offset */
	t_uscalar_t REMADDR_length;	/* remote address length */
	t_uscalar_t REMADDR_offset;	/* remote address offset */
} TR_addr_ack_t;

/*
 *  TR_UNBIND_REQ.  This primtive consists of one M_PROTO message block.
 */
typedef struct TR_unbind_req {
	t_uscalar_t PRIM_type;		/* Always TR_UNBIND_REQ */
} TR_unbind_req_t;

/*
 *  TR_OPTMGMT_REQ.  This primtive consists of one M_PROTO message block.
 */
typedef struct TR_optmgmt_req {
	t_uscalar_t PRIM_type;		/* Always T_OPTMGMT_REQ */
	t_uscalar_t OPT_length;		/* options length */
	t_uscalar_t OPT_offset;		/* options offset */
	t_uscalar_t MGMT_flags;		/* options data flags */
} TR_optmgmt_req_t;

/*
 *  TR_OPTMGMT_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_optmgmt_ack {
	t_uscalar_t PRIM_type;		/* Always T_OPTMGMT_ACK */
	t_uscalar_t OPT_length;		/* options length */
	t_uscalar_t OPT_offset;		/* options offset */
	t_uscalar_t MGMT_flags;		/* options data flags */
} TR_optmgmt_ack_t;

/*
 *  TR_OK_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_ok_ack {
	t_uscalar_t PRIM_type;		/* Always T_OK_ACK */
	t_uscalar_t CORRECT_prim;	/* correct primitive */
} TR_ok_ack_t;

/*
 *  TR_ERROR_ACK.  This primitive consists of one M_PCPROTO message block.
 */
typedef struct TR_error_ack {
	t_uscalar_t PRIM_type;		/* Always T_ERROR_ACK */
	t_uscalar_t ERROR_prim;		/* primitive in error */
	t_uscalar_t TRPI_error;		/* TRPI error code */
	t_uscalar_t UNIX_error;		/* UNIX error code */
	t_uscalar_t TRANS_id;		/* Transaction id */
} TR_error_ack_t;

/*
 *  TR_UNI_REQ.  This primitive consists of one M_PROTO message block followed
 *  by one or more M_DATA blocks.
 */
typedef struct TR_uni_req {
	t_uscalar_t PRIM_type;		/* Always TR_UNI_REQ */
	t_uscalar_t DEST_length;	/* Destination address length */
	t_uscalar_t DEST_offset;	/* Destination address offset */
	t_uscalar_t ORIG_length;	/* Originating address length */
	t_uscalar_t ORIG_offset;	/* Originating address offset */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_uni_req_t;

/*
 *  TR_UNI_IND.  This primitive consists of one M_PROTO message block followed
 *  by one or more M_DATA blocks.
 */
typedef struct TR_uni_ind {
	t_uscalar_t PRIM_type;		/* Always TR_UNI_REQ */
	t_uscalar_t DEST_length;	/* Destination address length */
	t_uscalar_t DEST_offset;	/* Destination address offset */
	t_uscalar_t ORIG_length;	/* Originating address length */
	t_uscalar_t ORIG_offset;	/* Originating address offset */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_uni_ind_t;

/*
 *  TR_BEGIN_REQ.
 */
typedef struct TR_begin_req {
	t_uscalar_t PRIM_type;		/* Always TR_BEGIN_REQ */
	t_uscalar_t CORR_id;		/* Correlation id */
	t_uscalar_t ASSOC_flags;	/* Association flags */
	t_uscalar_t DEST_length;	/* Destination address length */
	t_uscalar_t DEST_offset;	/* Destination address offset */
	t_uscalar_t ORIG_length;	/* Originating address length */
	t_uscalar_t ORIG_offset;	/* Originating address offset */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_begin_req_t;

/*
 *  TR_BEGIN_IND.
 */
typedef struct TR_begin_ind {
	t_uscalar_t PRIM_type;		/* Always TR_BEGIN_IND */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t ASSOC_flags;	/* Association flags */
	t_uscalar_t DEST_length;	/* Destination address length */
	t_uscalar_t DEST_offset;	/* Destination address offset */
	t_uscalar_t ORIG_length;	/* Originating address length */
	t_uscalar_t ORIG_offset;	/* Originating address offset */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_begin_ind_t;

/*
 *  TR_BEGIN_RES.
 *
 *  This primitive represents the first TR-CONTINUE response to a TR-BEGIN
 *  indication.
 */
typedef struct TR_begin_res {
	t_uscalar_t PRIM_type;		/* Always TR_BEGIN_RES */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t ASSOC_flags;	/* Association flags */
	t_uscalar_t ORIG_length;	/* Originating address length */
	t_uscalar_t ORIG_offset;	/* Originating address offset */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_begin_res_t;

/*
 *  TR_BEGIN_CON.
 *
 *  This primitive represents the first TR-CONTINUE configuration of a
 *  TR-BEGIN request.
 */
typedef struct TR_begin_con {
	t_uscalar_t PRIM_type;		/* Always TR_BEGIN_CON */
	t_uscalar_t CORR_id;		/* Correlation Id */
	t_uscalar_t ASSOC_flags;	/* Association flags */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t ORIG_length;	/* Originating address length */
	t_uscalar_t ORIG_offset;	/* Originating address offset */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_begin_con_t;

/*
 *  TR_CONT_REQ.
 */
typedef struct TR_cont_req {
	t_uscalar_t PRIM_type;		/* Always TR_CONT_REQ */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t ASSOC_flags;	/* Association flags */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_cont_req_t;

/*
 *  TR_CONT_IND.
 */
typedef struct TR_cont_ind {
	t_uscalar_t PRIM_type;		/* Always TR_CONT_IND */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t ASSOC_flags;	/* Association flags */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_cont_ind_t;

/*
 *  TR_END_REQ.
 */
typedef struct TR_end_req {
	t_uscalar_t PRIM_type;		/* Always TR_END_REQ */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t TERM_scenario;	/* Termination scenario */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_end_req_t;

/*
 *  TR_END_IND.
 */
typedef struct TR_end_ind {
	t_uscalar_t PRIM_type;		/* Always TR_END_IND */
	t_uscalar_t CORR_id;		/* Correlation id */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_end_ind_t;

/*
 *  TR_ABORT_REQ.
 */
typedef struct TR_abort_req {
	t_uscalar_t PRIM_type;		/* Always TR_ABORT_REQ */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t ABORT_cause;	/* Cause of the abort */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
} TR_abort_req_t;

/*
 *  TR_ABORT_IND.
 */
typedef struct TR_abort_ind {
	t_uscalar_t PRIM_type;		/* Always TR_ABORT_IND */
	t_uscalar_t CORR_id;		/* Correlation id */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t OPT_length;		/* Options structure length */
	t_uscalar_t OPT_offset;		/* Options structure offset */
	t_uscalar_t ABORT_cause;	/* Cause of the abort */
	t_uscalar_t ORIGINATOR;		/* Originator P or U */
} TR_abort_ind_t;

/*
 *  TR_NOTICE_IND.
 */
typedef struct TR_notice_ind {
	t_uscalar_t PRIM_type;		/* Always TR_NOTICE_IND */
	t_uscalar_t CORR_id;		/* Correlation id */
	t_uscalar_t TRANS_id;		/* Transaction id */
	t_uscalar_t REPORT_cause;	/* SCCP return cause */
} TR_notice_ind_t;

#endif				/* __SS7_TR_H__ */
