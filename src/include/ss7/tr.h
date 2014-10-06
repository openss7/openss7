/*****************************************************************************

 @(#) $Id: tr.h,v 1.1.2.2 2010-11-28 14:21:48 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2010-11-28 14:21:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tr.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:48  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:34  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SS7_TR_H__
#define __SS7_TR_H__

/* This file can be processed by doxygen(1). */

#define TR_INFO_REQ		 1	/* Information request */
#define TR_BIND_REQ		 2	/* Bind to network address */
#define TR_UNBIND_REQ		 3	/* Unbind from network address */
#define TR_OPTMGMT_REQ		 4	/* Options management request */
#define TR_UNI_REQ		 5	/* Unidirectional request */
#define TR_BEGIN_REQ		 6	/* Begin transaction request */
#define TR_BEGIN_RES		 7	/* Begin transaction response */
#define TR_CONT_REQ		 8	/* Continue transaction request */
#define TR_END_REQ		 9	/* End transaction request */
#define TR_ABORT_REQ		10	/* Abort transaction request */
#define TR_ADDR_REQ		11	/* Address request */
#define TR_CAPABILITY_REQ	12	/* Capability request */

#define TR_INFO_ACK		13	/* Information acknowledgement */
#define TR_BIND_ACK		14	/* Bound to network address */
#define TR_OK_ACK		15	/* Success acknowledgement */
#define TR_ERROR_ACK		16	/* Error acknowledgement */
#define TR_OPTMGMT_ACK		17	/* Options management acknowledgement */
#define TR_UNI_IND		18	/* Unidirectional indication */
#define TR_BEGIN_IND		19	/* Begin transaction indication */
#define TR_BEGIN_CON		20	/* Begin transaction confirmation */
#define TR_CONT_IND		21	/* Continue transaction indication */
#define TR_END_IND		22	/* End transaction indication */
#define TR_ABORT_IND		23	/* Abort transaction indication */
#define TR_NOTICE_IND		24	/* Error indication */
#define TR_ADDR_ACK		25	/* Address acknowledgement */
#define TR_CAPABILITY_ACK	26	/* Capability acknowledgement */

#define TR_COORD_REQ		35	/* coordinated withdrawal request */
#define TR_COORD_RES		36	/* coordinated withdrawal response */
#define TR_COORD_IND		37	/* coordinated withdrawal indication */
#define TR_COORD_CON		38	/* coordinated withdrawal confirmation */
#define TR_STATE_REQ		39	/* subsystem state request */
#define TR_STATE_IND		40	/* subsystem state indication */
#define TR_PCSTATE_IND		41	/* pointcode state indication */
#define TR_TRAFFIC_IND		42	/* traffic mix indication */

#define TR_QOS_SEL1		0x0501

typedef struct {
	t_scalar_t type;		/* Always TR_QOS_SEL1 */
	t_scalar_t flags;		/* Return option */
	t_scalar_t seq_ctrl;		/* Sequence Control */
	t_scalar_t priority;		/* Message priority */
} TR_qos_sel1_t;

/*
 * TRPI interface states
 */
#define TRS_UNBND	 0	/**< Unbound. */
#define TRS_WACK_BREQ	 1	/**< Waiting for TR_BIND_REQ ack. */
#define TRS_WACK_UREQ	 2	/**< Waiting for TR_UNBIND_REQ ack. */
#define TRS_IDLE	 3	/**< Idle. */
#define TRS_WACK_OPTREQ	 4	/**< Waiting for TR_OPTMGMT_REQ ack. */
#define TRS_WACK_CREQ	 5	/**< Waiting for TR_BEGIN_REQ ack. */
#define TRS_WCON_CREQ	 6	/**< Waiting for TR_BEGIN_REQ confirmation. */
#define TRS_WRES_CIND	 7	/**< Waiting for TR_BEGIN_IND response. */
#define TRS_WACK_CRES	 8	/**< Waiting for TR_BEGIN_RES ack. */
#define TRS_DATA_XFER	 9	/**< Data transfer. */
#define TRS_WACK_DREQ6	10	/**< Waiting for TR_END_REQ/TR_ABORT_REQ ack. */
#define TRS_WACK_DREQ7	11	/**< Waiting for TR_END_REQ/TR_ABORT_REQ ack. */
#define TRS_WACK_DREQ9	12	/**< Waiting for TR_END_REQ/TR_ABORT_REQ ack. */
#define TRS_NOSTATES	13

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
#define TRBADQOSPARAM	20	/* QOS values specified are outside the range supported by the TR provider */
#define TRBADQOSTYPE	21	/* QOS structure type specified is not supported by the TR provider */
#define TRBADTOKEN	22	/* Token used is not associated with an open stream */
#define TRNOPROTOID	23	/* Protocol id could not be allocated */

/*
 *  ASSOC_flags - association flags
 */
#define TR_PERMISSION	(1<<0)	/* permission to respond */

/*
 *  TR_INFO_REQ:- one M_PROTO or M_PCPROTO message block.
 */
typedef struct TR_info_req {
	t_scalar_t PRIM_type;		/* Always TR_INFO_REQ */
} TR_info_req_t;

/*
 *  TR_INFO_ACK:- one M_PCPROTO message block.
 */
typedef struct TR_info_ack {
	t_scalar_t PRIM_type;		/* Always TR_INFO_ACK */
	t_scalar_t TSDU_size;		/* maximum TR_CONT_REQ data size */
	t_scalar_t ETSDU_size;		/* maximum TR_UNI_REQ data size */
	t_scalar_t CDATA_size;		/* maximum TR_BEGIN_REQ data size */
	t_scalar_t DDATA_size;		/* maximum TR_END_REQ data size */
	t_scalar_t ADDR_size;		/* address size */
	t_scalar_t OPT_size;		/* maximum options size */
	t_scalar_t TIDU_size;		/* maximum SCCP-fragment data size */
	t_scalar_t SERV_type;		/* service type */
	t_scalar_t CURRENT_state;	/* current state */
	t_scalar_t PROVIDER_flag;	/* type of TR provider */
	t_scalar_t TRPI_version;	/* version # of trpi that is supported */
} TR_info_ack_t;

/*
 * SERV_type - service type
 */
#define TR_STRUCTURED	(1<<0)	/* structured dialogues */
#define TR_UNSTRUCTURED	(1<<1)	/* unstructured dialogues */

/*
 * PROVIDER_flag - provider flags
 */

#define TR_ITUT		(1<<9)	/* ITU-T based APPLICATION TCAP */
#define TR_ANSI		(1<<10)	/* ANSI based PRIVATE TCAP */

/*
 *  TR_BIND_REQ:- one M_PROTO message block.
 */
typedef struct TR_bind_req {
	t_scalar_t PRIM_type;		/* Always TR_BIND_REQ */
	t_scalar_t ADDR_length;		/* address length */
	t_scalar_t ADDR_offset;		/* address offset */
	t_uscalar_t XACT_number;	/* maximum outstanding transaction reqs. */
	t_scalar_t BIND_flags;		/* bind flags */
} TR_bind_req_t;

/*
 *  TR_BIND_ACK:- one M_PCPROTO message block.
 */
typedef struct TR_bind_ack {
	t_scalar_t PRIM_type;		/* Always TR_BIND_ACK */
	t_scalar_t ADDR_length;		/* address length */
	t_scalar_t ADDR_offset;		/* address offset */
	t_uscalar_t XACT_number;	/* open transactions */
	t_uscalar_t TOKEN_value;	/* value of "token" assigned to stream */
} TR_bind_ack_t;

/*
 *  TR_ADDR_REQ:- one M_PROTO or M_PCPROTO message block.
 */
typedef struct TR_addr_req {
	t_scalar_t PRIM_type;		/* Always TR_ADDR_REQ */
	t_scalar_t TRANS_id;		/* Transaction id */
} TR_addr_req_t;

/*
 *  TR_ADDR_ACK:- one M_PCPROTO or M_PCPROTO message block.
 */
typedef struct TR_addr_ack {
	t_scalar_t PRIM_type;		/* Always TR_ADDR_ACK */
	t_scalar_t LOCADDR_length;	/* local address length */
	t_scalar_t LOCADDR_offset;	/* local address offset */
	t_scalar_t REMADDR_length;	/* remote address length */
	t_scalar_t REMADDR_offset;	/* remote address offset */
	t_scalar_t TRANS_id;		/* Transaction id */
} TR_addr_ack_t;

/*
 * TR_CAPABILITY_REQ:- one M_PROTO or M_PCPROTO message block.
 */
typedef struct TR_capability_req {
	t_scalar_t PRIM_type;		/* Always TR_CAPABILITY_REQ */
	t_uscalar_t CAP_bits1;		/* Capability bits 1 */
} TR_capability_req_t;

/*
 * TR_CAPABILITY_ACK:- of one M_PROTO or M_PCPROTO message block.
 *
 * Note that TRANS_id returns a spare transaction id that will not be allocated for
 * some period of time in the future and can be used within a reasonable period by
 * the caller.
 */
typedef struct TR_capability_ack {
	t_scalar_t PRIM_type;		/* Always TR_CAPABILITY_ACK */
	t_uscalar_t CAP_bits1;		/* Capability bits #1 */
	struct TR_info_ack INFO_ack;	/* Info acknowledgement. */
	t_uscalar_t TOKEN_value;	/* Accept token value. */
	t_uscalar_t TRANS_id;		/* Transaction id. */
} TR_capability_ack_t;

#define TRC1_INFO	(1<<0)	/* Request/contains TR_info_ack. */
#define TRC1_TOKEN	(1<<1)	/* Request/contains acceptor token. */
#define TRC1_TRANS_ID	(1<<1)	/* Request/contains TRANS_id. */
#define TRC1_CAP_BITS2	(1<<31)	/* Contains extensions (unused). */

/*
 *  TR_UNBIND_REQ:- one M_PROTO message block.
 */
typedef struct TR_unbind_req {
	t_scalar_t PRIM_type;		/* Always TR_UNBIND_REQ */
} TR_unbind_req_t;

/*
 *  TR_OPTMGMT_REQ:- one M_PROTO or M_PCPROTO message block.
 */
typedef struct TR_optmgmt_req {
	t_scalar_t PRIM_type;		/* Always T_OPTMGMT_REQ */
	t_scalar_t OPT_length;		/* options length */
	t_scalar_t OPT_offset;		/* options offset */
	t_scalar_t MGMT_flags;		/* options data flags */
} TR_optmgmt_req_t;

/*
 *  TR_OPTMGMT_ACK:- one M_PCPROTO message block.
 */
typedef struct TR_optmgmt_ack {
	t_scalar_t PRIM_type;		/* Always T_OPTMGMT_ACK */
	t_scalar_t OPT_length;		/* options length */
	t_scalar_t OPT_offset;		/* options offset */
	t_scalar_t MGMT_flags;		/* options data flags */
} TR_optmgmt_ack_t;

/*
 *  TR_OK_ACK:- one M_PCPROTO message block.
 */
typedef struct TR_ok_ack {
	t_scalar_t PRIM_type;		/* Always T_OK_ACK */
	t_scalar_t CORRECT_prim;	/* correct primitive */
} TR_ok_ack_t;

/*
 *  TR_ERROR_ACK:- one M_PCPROTO message block.
 */
typedef struct TR_error_ack {
	t_scalar_t PRIM_type;		/* Always T_ERROR_ACK */
	t_scalar_t ERROR_prim;		/* primitive in error */
	t_scalar_t TRPI_error;		/* TRPI error code */
	t_scalar_t UNIX_error;		/* UNIX error code */
} TR_error_ack_t;

/*
 *  TR_UNI_REQ.  This primitive consists of one M_PROTO message block followed
 *  by one or more M_DATA blocks containing the dialogue portion and component
 *  sequence for the message.
 */
typedef struct TR_uni_req {
	t_scalar_t PRIM_type;		/* Always TR_UNI_REQ */
	t_scalar_t DEST_length;		/* Destination address length */
	t_scalar_t DEST_offset;		/* Destination address offset */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
} TR_uni_req_t;

/*
 *  TR_UNI_IND.  This primitive consists of one M_PROTO message block followed
 *  by one or more M_DATA blocks containing the dialogue portion and component
 *  sequence for the message.  Options may contain SCCP quality of service options
 *  and TCAP protocol variant.
 */
typedef struct TR_uni_ind {
	t_scalar_t PRIM_type;		/* Always TR_UNI_REQ */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t DEST_length;		/* Destination address length */
	t_scalar_t DEST_offset;		/* Destination address offset */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
} TR_uni_ind_t;

/*
 *  TR_BEGIN_REQ.  This primitive consists of one M_PROTO message block followed by
 *  zero or more M_DATA blocks containing the dialogue portion and component sequence
 *  of the transaction.  Options may contain SCCP quality of service parameters and
 *  TCAP protocol variant.
 */
typedef struct TR_begin_req {
	t_scalar_t PRIM_type;		/* Always TR_BEGIN_REQ */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t DEST_length;		/* Destination address length */
	t_scalar_t DEST_offset;		/* Destination address offset */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
	t_scalar_t ASSOC_flags;		/* Association flags */
} TR_begin_req_t;

/*
 *  TR_BEGIN_IND:- one M_PROTO message block followed by one or more M_DATA message
 *  blocks containing the dialogue portion and component sequence for the
 *  transaction.  Options may contain SCCP quality of service parameters and TCAP
 *  protocol variant.
 */
typedef struct TR_begin_ind {
	t_scalar_t PRIM_type;		/* Always TR_BEGIN_IND */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t DEST_length;		/* Destination address length */
	t_scalar_t DEST_offset;		/* Destination address offset */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
	t_scalar_t ASSOC_flags;		/* Association flags */
} TR_begin_ind_t;

/*
 *  TR_BEGIN_RES:- one M_PROTO message block followed by one or more M_DATA message
 *  blocks containing the dialogue portion and component sequence for the
 *  transaction.  Options may contain SCCP quality of service parameters.
 *
 *  This primitive represents the first TR-CONTINUE response to a TR-BEGIN
 *  indication.
 */
typedef struct TR_begin_res {
	t_scalar_t PRIM_type;		/* Always TR_BEGIN_RES */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
	t_scalar_t ASSOC_flags;		/* Association flags */
	t_scalar_t ACCEPTOR_id;		/* Token of accepting stream */
} TR_begin_res_t;

/*
 *  TR_BEGIN_CON: - one M_PROTO message block followed by one or more M_DATA message
 *  blocks containing the dialogue portion and component sequence for the
 *  transaction.  Options may contain SCCP quality of service parameters.
 *
 *  This primitive represents the first TR-CONTINUE configuration of a
 *  TR-BEGIN request.
 */
typedef struct TR_begin_con {
	t_scalar_t PRIM_type;		/* Always TR_BEGIN_CON */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
	t_scalar_t ASSOC_flags;		/* Association flags */
} TR_begin_con_t;

/*
 *  TR_CONT_REQ: - one M_PROTO message block followed by one or more M_DATA message
 *  blocks containing the dialogue portion and component sequence for the
 *  transaction.  Options may contain SCCP quality of service parameters.
 */
typedef struct TR_cont_req {
	t_scalar_t PRIM_type;		/* Always TR_CONT_REQ */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
	t_scalar_t ASSOC_flags;		/* Association flags */
} TR_cont_req_t;

/*
 *  TR_CONT_IND:- one M_PROTO message block followed by one or more M_DATA message
 *  blocks contianing the dialogue oprtion and component sequence for the
 *  transaction.  Options may contain SCCP quality of service parameters.
 */
typedef struct TR_cont_ind {
	t_scalar_t PRIM_type;		/* Always TR_CONT_IND */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
	t_scalar_t ASSOC_flags;		/* Association flags */
} TR_cont_ind_t;

/*
 *  TR_END_REQ:- one M_PROTO message block followed by zero or more M_DATA message
 *  blocks containing the dialogue portion and component sequence for the
 *  transaction.  Options may contain SCCP quality of service parameters.  Attached
 *  M_DATA message blocks and SCCP QoS parameters are ignored for prearranged
 *  termination scenarios.
 */
typedef struct TR_end_req {
	t_scalar_t PRIM_type;		/* Always TR_END_REQ */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t TERM_scenario;	/* Termination scenario */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
} TR_end_req_t;

/*
 * TERM_scenario - termination scenarios
 */
#define TR_TERM_UNSPECIFIED	0	/* termination unspecified */
#define TR_TERM_BASIC		1	/* termination basic */
#define TR_TERM_PREARRANGED	2	/* termination prearranged */

/*
 *  TR_END_IND:- one M_PROTO message block followed by zero or more M_DATA message
 *  blocks containing the dialogue portion and component sequence for the
 *  transaction.  Options may contain SCCP quality of service parameters.
 */
typedef struct TR_end_ind {
	t_scalar_t PRIM_type;		/* Always TR_END_IND */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
} TR_end_ind_t;

/*
 *  TR_ABORT_REQ
 */
typedef struct TR_abort_req {
	t_scalar_t PRIM_type;		/* Always TR_ABORT_REQ */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t ABORT_cause;		/* Cause of the abort */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
} TR_abort_req_t;

/*
 *  TR_ABORT_IND.
 */
typedef struct TR_abort_ind {
	t_scalar_t PRIM_type;		/* Always TR_ABORT_IND */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t OPT_length;		/* Options structure length */
	t_scalar_t OPT_offset;		/* Options structure offset */
	t_scalar_t ABORT_cause;		/* Cause of the abort */
	t_scalar_t ORIGINATOR;		/* Originator P or U */
} TR_abort_ind_t;

/*
 *  ABORT_cause - abort causes
 */

/* [APPLICATION 10] IMPLICIT INTEGER (0x4a01xx) */
#define TR_A_UNREC_MSG_TYPE		0x00	/* unrecognized message type */
#define TR_A_UNREC_TRANS_ID		0x01	/* unrecognized xact id */
#define TR_A_BAD_XACT_PORTION		0x02	/* badly formatted xact portion */
#define TR_A_INCORRECT_XACT_PORTION	0x03	/* incorrect xact portion */
#define TR_A_RESOURCE_LIMITATION	0x04	/* resource limitation */

/* [PRIVATE 27] IMPLICIT INTEGER (0xd701xx) */
#define TR_P_UNREC_PKG_TYPE		0x01	/* unrecognized package type */
#define TR_P_INCORRECT_XACT_PORTION	0x02	/* incorrect xact portion */
#define TR_P_BAD_XACT_PORTION		0x03	/* badly structured xact portion */
#define TR_P_UNASSIGNED_RESP_TRANS_ID	0x04	/* unassigned responding xact id */
#define TR_P_PERM_TO_RELEASE_PROB	0x05	/* permission to release problem */
#define TR_P_RESOURCE_UNAVAIL		0x06	/* resource unavailable */

#define TR_P_UNREC_DIALOG_PORTION_ID	0x07	/* unrecognized dialog portion id */
#define TR_P_BAD_DIALOG_PORTION		0x08	/* badly structured dialog portion */
#define TR_P_MISSING_DIALOG_PORTION	0x09	/* missing dialog portion */
#define TR_P_INCONSIST_DIALOG_PORTION	0x0a	/* inconsistent dialog portion */

/*
 *  ORIGINATOR - originator of abort
 */
#define TR_UNKNOWN	0x00	/* originator unknown */
#define TR_USER		0x01	/* remote user */
#define TR_PROVIDER	0x02	/* local or remote provider */

/*
 *  TR_NOTICE_IND.
 */
typedef struct TR_notice_ind {
	t_scalar_t PRIM_type;		/* Always TR_NOTICE_IND */
	t_scalar_t TRANS_id;		/* Transaction id */
	t_scalar_t DEST_length;		/* Destination address length */
	t_scalar_t DEST_offset;		/* Destination address offset */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t REPORT_cause;	/* SCCP return cause */
} TR_notice_ind_t;

/*
 * REPORT_cause - report causes
 *
 * These constants have the same value as the NPI-SCCP ERROR_types for N_UDERROR_IND and
 * N_NOTICE_IND.
 */
#define TR_RC_NO_ADDRESS_TYPE_TRANSLATION		0x2000
#define TR_RC_NO_ADDRESS_TRANSLATION			0x2001
#define TR_RC_SUBSYSTEM_CONGESTION			0x2002
#define TR_RC_SUBSYSTEM_FAILURE				0x2003
#define TR_RC_UNEQUIPPED_USER				0x2004
#define TR_RC_MTP_FAILURE				0x2005
#define TR_RC_NETWORK_CONGESTION			0x2006
#define TR_RC_UNQUALIFIED				0x2007
#define TR_RC_MESSAGE_TRANSPORT_ERROR			0x2008
#define TR_RC_LOCAL_PROCESSING_ERROR			0x2009
#define TR_RC_NO_REASSEMBLY_AT_DESTINATION		0x200a
#define TR_RC_SCCP_FAILURE				0x200b
#define TR_RC_SCCP_HOP_COUNTER_VIOLATION		0x200c
#define TR_RC_SEGMENTATION_NOT_SUPPORTED		0x200d
#define TR_RC_SEGMENTATION_FAILURE			0x200e

#define TR_RC_MESSAGE_CHANGE_FAILURE			0x20f7
#define TR_RC_INVALID_INS_ROUTING_REQUEST		0x20f8
#define TR_RC_INVALID_INSI_ROUTING_REQUEST		0x20f9
#define TR_RC_UNAUTHORIZED_MESSAGE			0x20fa
#define TR_RC_MESSAGE_INCOMPATIBILITY			0x20fb
#define TR_RC_CANNOT_PERFORM_ISNI_CONSTRAINED_ROUTING	0x20fc
#define TR_RC_REDUNDANT_ISNI_CONSTRAINED_ROUTING_INFO	0x20fd
#define TR_RC_UNABLE_TO_PERFORM_ISNI_IDENTIFICATION	0x20fe

/*
 *  TR_COORD_REQ.
 */
typedef struct TR_coord_req {
	t_scalar_t PRIM_type;		/* alwyas TR_COORD_REQ */
	t_scalar_t ADDR_length;		/* affected subsystem */
	t_scalar_t ADDR_offset;
} TR_coord_req_t;

/*
 *  TR_COORD_RES.
 */
typedef struct TR_coord_res {
	t_scalar_t PRIM_type;		/* always TR_COORD_RES */
	t_scalar_t ADDR_length;		/* affected subsystem */
	t_scalar_t ADDR_offset;
} TR_coord_res_t;

/*
 *  TR_COORD_IND.
 */
typedef struct TR_coord_ind {
	t_scalar_t PRIM_type;		/* alwyas TR_COORD_IND */
	t_scalar_t ADDR_length;		/* affected subsystem */
	t_scalar_t ADDR_offset;
	t_scalar_t SMI;			/* subsystem multiplicity indicator */
} TR_coord_ind_t;

/*
 *  TR_COORD_CON.
 */
typedef struct TR_coord_con {
	t_scalar_t PRIM_type;		/* always TR_COORD_CON */
	t_scalar_t ADDR_length;		/* affected subsystem */
	t_scalar_t ADDR_offset;
	t_scalar_t SMI;			/* subsystem multiplicity indicator */
} TR_coord_con_t;

/*
 * TR_STATE_REQ.
 */
typedef struct TR_state_req {
	t_scalar_t PRIM_type;		/* always TR_STATE_REQ */
	t_scalar_t ADDR_length;		/* affected subsystem */
	t_scalar_t ADDR_offset;
	t_scalar_t STATUS;		/* user status */
} TR_state_req_t;

/*
 * TR_STATE_IND.
 */
typedef struct TR_state_ind {
	t_scalar_t PRIM_type;		/* always TR_STATE_IND */
	t_scalar_t ADDR_length;		/* affected subsystem */
	t_scalar_t ADDR_offset;
	t_scalar_t STATUS;		/* user status */
	t_scalar_t SMI;			/* subsystem multiplicity indicator */
} TR_state_ind_t;

/*
 * SMI - subsystem multiplicity indicator
 *
 * These constants are the same as the values of the protocol field in ITU-T Rec. Q.713 (2001)
 * and ANSI T1.112/2000.
 */
#define TR_SMI_MULTIPLICITY_UNKNOWN	0
#define TR_SMI_SOLITARY			1
#define TR_SMI_DUPLICATED		2

/*
 * TR_PCSTATE_IND.
 */
typedef struct TR_pcstate_ind {
	t_scalar_t PRIM_type;		/* always TR_PCSTATE_IND */
	t_scalar_t ADDR_length;		/* affected point code */
	t_scalar_t ADDR_offset;
	t_scalar_t STATUS;		/* status */
} TR_pcstate_ind_t;

/*
 * STATUS - subsystem status for use in TR_STATE and TR_PCSTATE primitives.
 *
 * These constants and macros are the same as used by SCCP in N_UDERROR_IND reports.  In the
 * macros, the argument "cong" is a congestion status or restricted importance level from 0 to
 * 8.
 */

/* these two are application only to TR_STATE primitives */
#define TR_STATUS_USER_IN_SERVICE			1
#define TR_STATUS_USER_OUT_OF_SERVICE			2

/* the following are applicable to TR_PCSTATE primitives */
#define TR_STATUS_REMOTE_SCCP_AVAILABLE			3
#define TR_STATUS_REMOTE_SCCP_UNAVAILABLE		4
#define TR_STATUS_REMOTE_SCCP_UNEQUIPPED		5
#define TR_STATUS_REMOTE_SCCP_INACCESSIBLE		6
#define TR_STATUS_REMOTE_SCCP_CONGESTED(cong)		(7 + cong)

#define TR_STATUS_SIGNALLING_POINT_INACCESSIBLE		16
#define TR_STATUS_SIGNALLING_POINT_CONGESTED(cong)	(17 + cong)
#define TR_STATUS_SIGNALLING_POINT_ACCESSIBLE		26

/*
 * TR_TRAFFIC_IND
 */
typedef struct TR_traffic_ind {
	t_scalar_t PRIM_type;		/* always TR_TRAFFIC_IND */
	t_scalar_t ADDR_length;		/* affected user */
	t_scalar_t ADDR_offset;
	t_scalar_t TRAFFIC_mix;		/* traffic mix */
} TR_traffic_ind_t;

/*
 * TRAFFIC_mix - offered traffic mix
 *
 * These constants and macros are the same as used by NPI-SCCP in N_TRAFFIC_IND primitives.
 */
#define TR_TMIX_ALL_PREFFERED_NO_BACKUP		1
#define TR_TMIX_ALL_PREFERRED_SOME_BACKUP	2
#define TR_TMIX_ALL_PREFERRED_ALL_BACKUP	3
#define TR_TMIX_SOME_PREFERRED_NO_BACKUP	4
#define TR_TMIX_SOME_PREFERRED_SOME_BACKUP	5
#define TR_TMIX_NO_PREFERRED_NO_BACKUP		6
#define TR_TMIX_ALL				7
#define TR_TMIX_SOME				8
#define TR_TMIX_NONE				9

union TR_primitives {
	t_scalar_t type;
	struct TR_info_req info_req;
	struct TR_bind_req bind_req;
	struct TR_unbind_req unbind_req;
	struct TR_optmgmt_req optmgmt_req;
	struct TR_uni_req uni_req;
	struct TR_begin_req begin_req;
	struct TR_begin_res begin_res;
	struct TR_cont_req cont_req;
	struct TR_end_req end_req;
	struct TR_abort_req abort_req;
	struct TR_addr_req addr_req;
	struct TR_capability_req capability_req;
	struct TR_info_ack info_ack;
	struct TR_bind_ack bind_ack;
	struct TR_ok_ack ok_ack;
	struct TR_error_ack error_ack;
	struct TR_optmgmt_ack optmgmt_ack;
	struct TR_uni_ind uni_ind;
	struct TR_begin_ind begin_ind;
	struct TR_begin_con begin_con;
	struct TR_cont_ind cont_ind;
	struct TR_end_ind end_ind;
	struct TR_abort_ind abort_ind;
	struct TR_notice_ind notice_ind;
	struct TR_addr_ack addr_ack;
	struct TR_capability_ack capability_ack;
	struct TR_coord_req coord_req;
	struct TR_coord_res coord_res;
	struct TR_coord_ind coord_ind;
	struct TR_coord_con coord_con;
	struct TR_state_req state_req;
	struct TR_state_ind state_ind;
	struct TR_pcstate_ind pcstate_ind;
	struct TR_traffic_ind traffic_ind;
};

#endif				/* __SS7_TR_H__ */
