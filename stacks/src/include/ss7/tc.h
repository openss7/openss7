/*****************************************************************************

 @(#) $Id: tc.h,v 0.9.2.3 2004/10/12 05:54:09 brian Exp $

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

 Last Modified $Date: 2004/10/12 05:54:09 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_TC_H__
#define __SS7_TC_H__

#ident "@(#) $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

/*
 *  Primitive definitions for TC-Users and TC-Providers.
 */

#define TC_INFO_REQ		 0	/* Information request */
#define TC_BIND_REQ		 1	/* Bind to network address */
#define TC_UNBIND_REQ		 2	/* Unbind from network address */
#define TC_SUBS_BIND_REQ	 3	/* Subsequent bind to network address */
#define TC_SUBS_UNBIND_REQ	 4	/* Subsequent unbind from network address */
#define TC_OPTMGMT_REQ		 5	/* Options management */
#define TC_UNI_REQ		 6	/* Unidirectional request */
#define TC_BEGIN_REQ		 7	/* Begin transaction request */
#define TC_BEGIN_RES		 8	/* Begin transaction response */
#define TC_CONT_REQ		 9	/* Continue transaction request */
#define TC_END_REQ		10	/* End transaction request */
#define TC_ABORT_REQ		11	/* User abort request */

#define TC_INFO_ACK		12	/* Information acknowledgement */
#define TC_BIND_ACK		13	/* Bound to network address */
#define TC_SUBS_BIND_ACK	14	/* Bound to network address */
#define TC_OK_ACK		15	/* Success acknowledgement */
#define TC_ERROR_ACK		16	/* Error acknowledgement */
#define TC_OPTMGMT_ACK		17	/* Optionas management acknowledgement */
#define TC_UNI_IND		18	/* Unidirectional indication */
#define TC_BEGIN_IND		19	/* Begin transaction indication */
#define TC_BEGIN_CON		20	/* Begin transaction confirmation-Continue */
#define TC_CONT_IND		21	/* Continue transaction indication */
#define TC_END_IND		22	/* End transaction indication */
#define TC_ABORT_IND		23	/* TC-User abort indication */
#define TC_NOTICE_IND		24	/* Network Service Provider notice */

/*
 *  Additional primitives for component handling.
 */
#define TC_INVOKE_REQ		26	/* Invocation of an operation */
#define TC_RESULT_REQ		27	/* Result of a successful operation */
#define TC_ERROR_REQ		28	/* Error reply to an invoked operation */
#define TC_CANCEL_REQ		29	/* Termination of an operation invocation */
#define TC_REJECT_REQ		30	/* Rejection of a component */

#define TC_INVOKE_IND		32	/* Invocation of an operation */
#define TC_RESULT_IND		33	/* Result of a successful operation */
#define TC_ERROR_IND		34	/* Error reply to an invoked operation */
#define TC_CANCEL_IND		35	/* Termination of an operation invocation */
#define TC_REJECT_IND		36	/* Rejection of a component */

#define TC_QOS_SEL1		0x0701

typedef struct {
	ulong type;			/* Always TC_QOS_SEL1 */
	ulong flags;			/* Return option */
	ulong seq_ctrl;			/* Sequence Control */
	ulong priority;			/* Message priority */
} TC_qos_sel1_t;

/*
 * TCPI interface states
 */
#define TCS_UNBND	0	/* TC user not bound to network address */
#define TCS_WACK_BREQ	1	/* Awaiting acknowledgement of N_BIND_REQ */
#define TCS_WACK_UREQ	2	/* Pending acknowledgement for N_UNBIND_REQ */
#define TCS_IDLE	3	/* Idle, no connection */
#define TCS_WACK_OPTREQ	4	/* Pending acknowledgement of N_OPTMGMT_REQ */
#define TCS_WACK_RRES	5	/* Pending acknowledgement of N_RESET_RES */
#define TCS_WCON_CREQ	6	/* Pending confirmation of N_CONN_REQ */
#define TCS_WRES_CIND	7	/* Pending response of N_CONN_REQ */
#define TCS_WACK_CRES	8	/* Pending acknowledgement of N_CONN_RES */
#define TCS_DATA_XFER	9	/* Connection-mode data transfer */
#define TCS_WCON_RREQ	10	/* Pending confirmation of N_RESET_REQ */
#define TCS_WRES_RIND	11	/* Pending response of N_RESET_IND */
#define TCS_WACK_DREQ6	12	/* Waiting ack of N_DISCON_REQ */
#define TCS_WACK_DREQ7	13	/* Waiting ack of N_DISCON_REQ */
#define TCS_WACK_DREQ9	14	/* Waiting ack of N_DISCON_REQ */
#define TCS_WACK_DREQ10	15	/* Waiting ack of N_DISCON_REQ */
#define TCS_WACK_DREQ11	16	/* Waiting ack of N_DISCON_REQ */

#define TCS_NOSTATES	17

/*
 * TC_ERROR_ACK error return code values
 */
#define TCBADADDR	1	/* Incorrect address format/illegal address information */
#define TCBADOPT	2	/* Options in incorrect format or contain illegal information */
#define TCACCESS	3	/* User did not have proper permissions */
#define TCNOADDR	5	/* TC Provider could not allocate address */
#define TCOUTSTATE	6	/* Primitive was issues in wrong sequence */
#define TCBADSEQ	7	/* Sequence number in primitive was incorrect/illegal */
#define TCSYSERR	8	/* UNIX system error occurred */
#define TCBADDATA	10	/* User data spec. outside range supported by TC provider */
#define TCBADFLAG	16	/* Flags specified in primitive were illegal/incorrect */
#define TCNOTSUPPORT	18	/* Primitive type not supported by the TC provider */
#define TCBOUND		19	/* Illegal second attempt to bind listener or default listener */
#define TCBADQOSPARAM	20	/* QOS values specified are outside the range supported by the TC
				   provider */
#define TCBADQOSTYPE	21	/* QOS structure type specified is not supported by the TC provider 
				 */
#define TCBADTOKEN	22	/* Token used is not associated with an open stream */
#define TCNOPROTOID	23	/* Protocol id could not be allocated */

/*
 *  TC_INFO_REQ
 */
typedef struct TC_info_req {
	ulong PRIM_type;
} TC_info_req_t;

/*
 *  TC_INFO_ACK
 */
typedef struct TC_info_ack {
	long PRIM_type;
	long TSDU_size;
	long ETSDU_size;
	long CDATA_size;
	long DDATA_size;
	long ADDR_size;
	long OPT_size;
	long TIDU_size;
	long SERV_type;
	long CURRENT_state;
	long PROVIDER_flag;
	long TRPI_version;
} TC_info_ack_t;

/*
 *  TC_BIND_REQ
 */
typedef struct TC_bind_req {
	ulong PRIM_type;
	ulong ADDR_length;		/* address length */
	ulong ADDR_offset;		/* address offset */
	ulong XACT_number;		/* maximum outstanding transaction reqs. */
	ulong BIND_flags;		/* bind flags */
} TC_bind_req_t;

/*
 *  TC_BIND_ACK
 */
typedef struct TC_bind_ack {
	ulong PRIM_type;
	ulong ADDR_length;
	ulong ADDR_offset;
	ulong XACT_number;
	ulong TOKEN_value;
} TC_bind_ack_t;

/*
 *  TC_SUBS_BIND_REQ
 */
typedef struct TC_subs_bind_req {
	ulong PRIM_type;
} TC_subs_bind_req_t;

/*
 *  TC_SUBS_BIND_ACK
 */
typedef struct TC_subs_bind_ack {
	ulong PRIM_type;
} TC_subs_bind_ack_t;

/*
 *  TC_OK_ACK
 */
typedef struct TC_ok_ack {
	ulong PRIM_type;
	ulong CORRECT_prim;
} TC_ok_ack_t;

/*
 *  TC_ERROR_ACK
 */
typedef struct TC_error_ack {
	ulong PRIM_type;
	ulong ERROR_prim;
	ulong TRPI_error;
	ulong UNIX_error;
	ulong DIALOG_id;
	ulong INVOKE_id;
} TC_error_ack_t;

/*
 *  TC_OPTMGMT_REQ
 */
typedef struct TC_optmgmt_req {
	ulong PRIM_type;
	ulong OPT_length;
	ulong OPT_offset;
	ulong MGMT_flags;
} TC_optmgmt_req_t;

/*
 *  TC_OPTMGMT_ACK
 */
typedef struct TC_optmgmt_ack {
	ulong PRIM_type;
	ulong OPT_length;
	ulong OPT_offset;
	ulong MGMT_flags;
} TC_optmgmt_ack_t;

/*
 *  TC_UNI_REQ,  Send unidirctional message.  One M_PROTO block followed by
 *  one or more M_DATA blocks containing User Information.  Components to be
 *  delivered in the unstructured dialog must have been previously provided
 *  with the same Dialog Id and using the component handling request
 *  primitives.  An Application Context is required if there is User
 *  Information in attached M_DATA blocks.
 *
 *  Note:   Source Address may be implicitly associatedw tih the access point
 *          at which the primitive is being issued.
 *
 *  Note:   Dialog identifier has only local significance and is used between
 *          the local TC-User and TC-Provider to refer to a dialog.
 */
typedef struct TC_uni_req {
	ulong PRIM_type;		/* Always TC_UNI_REQ */
	ulong SRC_length;		/* Source address length */
	ulong SRC_offset;		/* Source address offset */
	ulong DEST_length;		/* Destination address length */
	ulong DEST_offset;		/* Destination address offset */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
} TC_uni_req_t;

/*
 *  TC_UNI_RES,  Received unidirectional message.  One M_PROTO block
 *  followed by one or more M_DATA blocks containing User Information.
 *  Components to be delivered from the unstructured dialog will be indicated
 *  using the component handling indication primitives.  An Application
 *  Context will be present where there is User Information in attached M_DATA
 *  blocks.
 *
 *  Note:   When QOS is provided by SCCP, QOS must be passed up to the
 *          TC-User.
 *
 *  Note:   When Application Context is provided in the corresponding message,
 *          it must be passed up in the indication.
 */
typedef struct TC_uni_ind {
	ulong PRIM_type;		/* Always TC_UNI_IND */
	ulong SRC_length;		/* Source address length */
	ulong SRC_offset;		/* Source address offset */
	ulong DEST_length;		/* Destination address length */
	ulong DEST_offset;		/* Destination address offset */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong COMP_flags;		/* Components preset flag */
} TC_uni_ind_t;

/*
 *  TC_BEGIN_REQ.  Requests the opening of a dialog.  One M_PROTO block
 *  followed by one or more M_DATA blocks containing User Information.
 *  Components to be delivered in the structured dialog must have been
 *  previously provided wtih the same Dialog Id and using the component
 *  handling request primitives.  An Application Context is required if there
 *  is User Information in attached M_DATA blocks.
 *
 *  Also T_QUERY_REQ for ANSI.
 */
typedef struct TC_begin_req {
	ulong PRIM_type;		/* Always TC_BEGIN_REQ */
	ulong SRC_length;		/* Source address length */
	ulong SRC_offset;		/* Source address offset */
	ulong DEST_length;		/* Destination address length */
	ulong DEST_offset;		/* Destination address offset */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong COMP_flags;		/* For use with ANSI QWP/QWOP */
} TC_begin_req_t;

typedef struct TC_begin_req TC_query_req;

/*
 *  TC_BEGIN_IND.  Indicates the opening of a dialog.  One M_PROTO block
 *  followed by one or more M_DATA blocks containing User Information.
 *  Components to be delivered in the structured dialog will be subsequently
 *  indicated with the same Dialog Id and using the component handling
 *  indication primitives.  An Application Context is present if there is User
 *  Information in attached M_DATA blocks.
 *
 *  Also T_QUERY_IND for ANSI.
 */
typedef struct TC_begin_ind {
	ulong PRIM_type;		/* Always TC_BEGIN_IND */
	ulong SRC_length;		/* Source address length */
	ulong SRC_offset;		/* Source address offset */
	ulong DEST_length;		/* Destination address length */
	ulong DEST_offset;		/* Destination address offset */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong COMP_flags;		/* For use with ANSI QWP/QWOP */
} TC_begin_ind_t;

typedef struct TC_begin_ind TC_query_ind;

/*
 *  TC_END_REQ.
 *
 *  Also TC_RESP_REQ for ANSI.
 */
typedef struct TC_end_req {
	ulong PRIM_type;		/* Always TC_END_REQ */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong TERM_scenario;		/* Reason for termination */
} TC_end_req_t;

typedef struct TC_end_req TC_resp_req_t;

/*
 *  TC_END_IND.
 *
 *  Also TC_RESP_IND for ANSI.
 */
typedef struct TC_end_ind {
	ulong PRIM_type;		/* Always TC_END_IND */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong COMP_flags;		/* Components present flag */
} TC_end_ind_t;

typedef struct TC_end_ind TC_resp_ind_t;

/*
 *  TC_CONT_REQ.  The first TC_CONT_REQ after a TC_BEGIN_IND requests
 *  that the dialog be confirmed and may contain the Source address and
 *  Application Context parameters.  Once these have been provided on the
 *  first TC_CONT_REQ, they are in place for the remainder of the dialog.
 *  Subsequent TC_CONT_REQ primitives do not contain the SRC and CONTEXT
 *  parameters.
 *
 *  Also TC_CONV_REQ for ANSI.
 */
typedef struct TC_begin_res {
	ulong PRIM_type;		/* Always TC_CONT_REQ */
	ulong SRC_length;		/* Source address length */
	ulong SRC_offset;		/* Source address offset */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong COMP_flags;		/* For use with ANSI CWP/CWOP */
} TC_begin_res_t;

typedef struct TC_cont_req {
	ulong PRIM_type;		/* Always TC_CONT_REQ */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong COMP_flags;		/* For use with ANSI CWP/CWOP */
} TC_cont_req_t;

typedef struct TC_cont_req TC_conv_req_t;

/*
 *  TC_CONT_IND.  The first TC_CONT_IND after a TC_BEGIN_REQ indicates
 *  that the dialog is confirmed but may contain the Source address and
 *  Application Context parameters.  Once these have been provided on the
 *  first TC_CONT_IND, they are in place for the remainder of the dialog.
 *  Subsequent TC_CONT_IND primitives will not contain the SRC and CONTEXT
 *  parameters.
 *
 *  Also TC_CONV_IND for ASNI.
 */
typedef struct TC_begin_con {
	ulong PRIM_type;		/* Always TC_CONT_IND */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong COMP_flags;		/* For use with ANSI CWP/CWOP */
} TC_begin_con_t;

typedef struct TC_cont_ind {
	ulong PRIM_type;		/* Always TC_CONT_IND */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong COMP_flags;		/* For use with ANSI CWP/CWOP */
} TC_cont_ind_t;

typedef struct TC_cont_ind TC_conv_ind_t;

/*
 *  TC_ABORT_REQ.
 *
 *  Note:   Application context is only present if the abort reason indicates
 *          "application context not supported".
 */
typedef struct TC_abort_req {
	ulong PRIM_type;		/* Always TC_ABORT_REQ */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong ABORT_reason;		/* Abort reason */
} TC_abort_req_t;

/*
 *  TC_ABORT_IND.
 *
 *  Note:   Application context is only present if the abort reason indicates
 *          "application context not supported".
 */
typedef struct TC_abort_ind {
	ulong PRIM_type;		/* Always TC_ABORT_IND */
	ulong OPT_length;		/* Options associated with the primitive */
	ulong OPT_offset;		/* Options associated wtih the primitive */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong ABORT_reason;		/* Abort reason */
	ulong ORIGINATOR;		/* Either User or Provider originated */
} TC_abort_ind_t;

/*
 *  TC_NOTICE_IND.
 */
typedef struct TC_notice_ind {
	ulong PRIM_type;		/* Always TC_NOTICE_IND */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong REPORT_cause;		/* Report cause */
} TC_notice_ind_t;

/*
 *  Component handling primitives.
 */

/*
 *  TC_INVOKE_REQ.  This primitive is one M_PROTO message block followed by
 *  zero or more M_DATA blocks containing the parameters of the operation.
 */
typedef struct TC_invoke_req {
	ulong PRIM_type;		/* Always TC_INVOKE_REQ */
	ulong DIALOG_id;		/* Dialog identifier */
	ulong PROTOCOL_class;		/* Application protocol class */
	ulong INVOKE_id;		/* Invoke Identifier */
	ulong LINKED_id;		/* Linked Invoke Identifier */
	ulong OPERATION;		/* Requested operation to invoke */
	ulong MORE_flag;		/* Not last */
	ulong TIMEOUT;			/* Timeout */
} TC_invoke_req_t;

/*
 *  TC_INVOKE_IND.  This primitive is one M_PROTO message block followed by
 *  zero or more M_DATA blocks containing the parameters of the operation.
 *
 *  Note:   Dialog Id is ignored for Class 4 (TC_UNI_IND) operations.
 */
typedef struct TC_invoke_ind {
	ulong PRIM_type;		/* Always TC_INVOKE_IND */
	ulong DIALOG_id;		/* Dialog identifier */
	ulong OP_class;			/* Application operation class */
	ulong INVOKE_id;		/* Invoke Identifier */
	ulong LINKED_id;		/* Linked Invoke Identifier */
	ulong OPERATION;		/* Requested operation to invoke */
	ulong MORE_flag;		/* Not last */
} TC_invoke_ind_t;

/*
 *  TC_RESULT_REQ.  This primitive consists of one M_PROTO message block
 *  followed by zero or more M_DATA blocks containing the parameters of the
 *  operation.
 */
typedef struct TC_result_req {
	ulong PRIM_type;		/* Always TC_RESULT_REQ */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong INVOKE_id;		/* Invoke Identifier */
	ulong OPERATION;		/* Requested operation result */
	ulong MORE_flag;		/* Not last */
} TC_result_req_t;

/*
 *  TC_RESULT_IND.  This primitive consists of one M_PROTO message block
 *  followed by zero or more M_DATA blocks containing the parameters of the
 *  operation.
 *
 *  This primitive is only valid (expected) for operation class 1 and 3.
 */
typedef struct TC_result_ind {
	ulong PRIM_type;		/* Always TC_RESULT_IND */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong INVOKE_id;		/* Invoke Identifier */
	ulong OPERATION;		/* Requested operation result */
	ulong MORE_flag;		/* Not last */
} TC_result_ind_t;

/*
 *  TC_ERROR_REQ.  This primitive consists of one M_PROTO message block
 *  followed by zero or more M_DATA blocks containing the parameters of the
 *  error.
 */
typedef struct TC_error_req {
	ulong PRIM_type;		/* Always TC_ERROR_REQ */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong INVOKE_id;		/* Invoke Identifier */
	ulong ERROR_code;		/* Error code */
	ulong MORE_flag;		/* Not last */
} TC_error_req_t;

/*
 *  TC_ERROR_IND.  This primitive consists of one M_PROTO message block
 *  followed by zero or more M_DATA blocks containing the parameters of the
 *  error.
 */
typedef struct TC_error_ind {
	ulong PRIM_type;		/* Always TC_ERROR_IND */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong INVOKE_id;		/* Invoke Identifier */
	ulong ERROR_code;		/* Error code */
} TC_error_ind_t;

/*
 *  TC_REJECT_REQ.  This primitive consists of one M_PROTO message block.
 */
typedef struct TC_reject_req {
	ulong PRIM_type;		/* Always TC_REJECT_REQ */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong INVOKE_id;		/* Invoke identifier */
	ulong PROBLEM_code;		/* Problem code */
} TC_reject_req_t;

/*
 *  TC_REJECT_IND.  This primitive consists of one M_PROTO message block.
 */
typedef struct TC_reject_ind {
	ulong PRIM_type;		/* Always TC_REJECT_IND */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong INVOKE_id;		/* Invoke identifier */
	ulong ORIGINATOR;		/* Either User, Local or Remote */
	ulong PROBLEM_code;		/* Problem code */
} TC_reject_ind_t;

/*
 *  TC_CANCEL_REQ.  This primitive consists of one M_PROTO message block.
 */
typedef struct TC_cancel_req {
	ulong PRIM_type;		/* Always TC_CANCEL_REQ */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong INVOKE_id;		/* Invoke identifier */
} TC_cancel_req_t;

/*
 *  TC_CANCEL_IND.  This primitive consists of one M_PROTO message block.
 */
typedef struct TC_cancel_ind {
	ulong PRIM_type;		/* Always TC_CANCEL_REQ */
	ulong DIALOG_id;		/* Dialog Identifier */
	ulong INVOKE_id;		/* Invoke identifier */
} TC_cancel_ind_t;

#endif				/* __SS7_TC_H__ */
