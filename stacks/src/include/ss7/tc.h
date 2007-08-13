/*****************************************************************************

 @(#) $Id: tc.h,v 0.9.2.10 2007/08/13 19:09:37 brian Exp $

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

 Last Modified $Date: 2007/08/13 19:09:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tc.h,v $
 Revision 0.9.2.10  2007/08/13 19:09:37  brian
 - updated headers, formatting

 Revision 0.9.2.9  2007/08/03 13:35:01  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.8  2007/02/13 14:05:29  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __SS7_TC_H__
#define __SS7_TC_H__

#ident "@(#) $RCSfile: tc.h,v $ $Name:  $($Revision: 0.9.2.10 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

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
	t_uscalar_t type;		/* Always TC_QOS_SEL1 */
	t_uscalar_t flags;		/* Return option */
	t_uscalar_t seq_ctrl;		/* Sequence Control */
	t_uscalar_t priority;		/* Message priority */
} TC_qos_sel1_t;

/*
 * TCPI interface states
 */
#define TCS_UNBND		 0	/* TC user not bound to network address */
#define TCS_WACK_BREQ		 1	/* Awaiting acknowledgement of N_BIND_REQ */
#define TCS_WACK_UREQ		 2	/* Pending acknowledgement for N_UNBIND_REQ */
#define TCS_IDLE		 3	/* Idle, no connection */
#define TCS_WACK_OPTREQ		 4	/* Pending acknowledgement of N_OPTMGMT_REQ */
#define TCS_WACK_RRES		 5	/* Pending acknowledgement of N_RESET_RES */
#define TCS_WCON_CREQ		 6	/* Pending confirmation of N_CONN_REQ */
#define TCS_WRES_CIND		 7	/* Pending response of N_CONN_REQ */
#define TCS_WACK_CRES		 8	/* Pending acknowledgement of N_CONN_RES */
#define TCS_DATA_XFER		 9	/* Connection-mode data transfer */
#define TCS_WCON_RREQ		10	/* Pending confirmation of N_RESET_REQ */
#define TCS_WRES_RIND		11	/* Pending response of N_RESET_IND */
#define TCS_WACK_DREQ6		12	/* Waiting ack of N_DISCON_REQ */
#define TCS_WACK_DREQ7		13	/* Waiting ack of N_DISCON_REQ */
#define TCS_WACK_DREQ9		14	/* Waiting ack of N_DISCON_REQ */
#define TCS_WACK_DREQ10		15	/* Waiting ack of N_DISCON_REQ */
#define TCS_WACK_DREQ11		16	/* Waiting ack of N_DISCON_REQ */

#define TCS_NOSTATES	17

/*
 * TC_ERROR_ACK error return code values
 */
#define TCBADADDR		 1	/* Incorrect address format/illegal address information */
#define TCBADOPT		 2	/* Options in incorrect format or contain illegal
					   information */
#define TCACCESS		 3	/* User did not have proper permissions */
#define TCNOADDR		 5	/* TC Provider could not allocate address */
#define TCOUTSTATE		 6	/* Primitive was issues in wrong sequence */
#define TCBADSEQ		 7	/* Sequence number in primitive was incorrect/illegal */
#define TCSYSERR		 8	/* UNIX system error occurred */
#define TCBADDATA		10	/* User data spec. outside range supported by TC provider */
#define TCBADFLAG		16	/* Flags specified in primitive were illegal/incorrect */
#define TCNOTSUPPORT		18	/* Primitive type not supported by the TC provider */
#define TCBOUND			19	/* Illegal second attempt to bind listener or default
					   listener */
#define TCBADQOSPARAM		20	/* QOS values specified are outside the range supported by
					   the TC provider */
#define TCBADQOSTYPE		21	/* QOS structure type specified is not supported by the TC
					   provider */
#define TCBADTOKEN		22	/* Token used is not associated with an open stream */
#define TCNOPROTOID		23	/* Protocol id could not be allocated */

/*
 *  TC_ABORT_IND originator
 */
#define TC_PROVIDER		0x0001
#define TC_USER			0x0002

/*
 *  TC_ABORT abort reasons
 */
/* Application-Wide ITU Q.773 abort reasons */
#define TCAP_AAB_UNREC_MSG_TYPE			0x0a00	/* unrecognized message type */
#define TCAP_AAB_UNREC_TRANS_ID			0x0a01	/* unrecognized transaction id */
#define TCAP_AAB_BAD_XACT_PORTION		0x0a02	/* badly formatted transaction portion */
#define TCAP_AAB_INCORRECT_XACT_PORTION		0x0a03	/* incorrect transaction portion */
#define TCAP_AAB_RESOURCE_LIMITATION		0x0a04	/* resource limitation */
/* Private-TCAP ANSI T1.114 abort reasons */
#define TCAP_PAB_UNREC_PKG_TYPE			0x1701	/* unrecognized package type */
#define TCAP_PAB_INCORRECT_XACT_PORTION		0x1702	/* incorrect transaction portion */
#define TCAP_PAB_BAD_XACT_PORTION		0x1703	/* badly structured transaction portion */
#define TCAP_PAB_UNASSIGNED_RESP_TRANS_ID	0x1704	/* unassigned responding transaction id */
#define TCAP_PAB_PERM_TO_RELEASE_PROB		0x1705	/* permission to release problem */
#define TCAP_PAB_RESOURCE_UNAVAIL		0x1706	/* resource unavailable */
#define TCAP_PAB_UNREC_DIALOG_PORTION_ID	0x1707	/* unrecognized dialogue portion id */
#define TCAP_PAB_BAD_DIALOG_PORTION		0x1708	/* badly structured dialogue portion */
#define TCAP_PAB_MISSING_DIALOG_PORTION		0x1709	/* missing dialogue portion */
#define TCAP_PAB_INCONSIST_DIALOG_PORTION	0x170a	/* inconsistent dialogue portion */

/*
 *  TC_REJECT problem codes
 */
/* Application Wide ITU Q.773 reject problem codes */
#define TCAP_ARJ_GN_UNRECOGNIZED_COMPONENT	0x0000	/* unrecognized component */
#define TCAP_ARJ_GN_MISTYPED_COMPONENT		0x0001	/* mistyped component */
#define TCAP_ARJ_GN_BADLY_STRUCTURED_COMPONENT	0x0002	/* badly structured component */
#define TCAP_ARJ_IN_DUPLICATE_INVOKE_ID		0x0100	/* duplicate invoke id */
#define TCAP_ARJ_IN_UNRECOGNIZED_OPERATION	0x0101	/* unrecognized operation */
#define TCAP_ARJ_IN_MISTYPED_PARAMETER		0x0102	/* mistyped parameter */
#define TCAP_ARJ_IN_RESOURCE_LIMITATION		0x0103	/* resource limitation */
#define TCAP_ARJ_IN_INITIATING_RELEASE		0x0104	/* initiating release */
#define TCAP_ARJ_IN_UNRECOGNIZED_LINKED_ID	0x0105	/* unrecognized linked id */
#define TCAP_ARJ_IN_LINKED_RESPONSE_EXPECTED	0x0106	/* linked response expected */
#define TCAP_ARJ_IN_UNEXPECTED_LINKED_OPERATION	0x0107	/* unexpected linked operation */
#define TCAP_ARJ_RR_UNRECOGNIZED_INVOKE_ID	0x0200	/* urecognized invoke id */
#define TCAP_ARJ_RR_RETURN_RESULT_UNEXPECTED	0x0201	/* return result unexpected */
#define TCAP_ARJ_RR_MISTYPED_PARAMETER		0x0202	/* mistyped parameter */
#define TCAP_ARJ_RE_UNRECOGNIZED_INVOKE_ID	0x0300	/* unrecognized invoke id */
#define TCAP_ARJ_RE_RETURN_ERROR_UNEXPECTED	0x0301	/* return error unexpected */
#define TCAP_ARJ_RE_UNRECOGNIZED_ERROR		0x0302	/* unrecognized error */
#define TCAP_ARJ_RE_UNEXPECTED_ERROR		0x0303	/* unexpected error */
#define TCAP_ARJ_RE_MISTYPED_PARAMETER		0x0304	/* mistyped parameter */
/* Private TCAP ANSI T1.114 reject problem codes */
#define TCAP_PRJ_GN_UNRECOGNIZED_COMPONENT_TYPE	0x0101	/* unrecognized component type */
#define TCAP_PRJ_GN_INCORRECT_COMPONENT_PORTION	0x0102	/* incorrect component portion */
#define TCAP_PRJ_GN_BADLY_STRUCTURED_COMP_PRTN	0x0103	/* badly structure component portion */
#define TCAP_PRJ_GN_INCORRECT_COMPONENT_CODING	0x0104	/* incorrect component coding */
#define TCAP_PRJ_IN_DUPLICATE_INVOCATION	0x0201	/* duplicate invocation */
#define TCAP_PRJ_IN_UNRECOGNIZED_OPERATION	0x0202	/* unrecognized operation */
#define TCAP_PRJ_IN_INCORRECT_PARAMETER		0x0203	/* incorrect parameter */
#define TCAP_PRJ_IN_UNRECOGNIZED_CORRELATION_ID	0x0204	/* unrecognized correlation id */
#define TCAP_PRJ_RR_UNRECOGNIZED_CORRELATION_ID	0x0301	/* unrecognized correlation id */
#define TCAP_PRJ_RR_UNEXPECTED_RETURN_RESULT	0x0302	/* unexpected return result */
#define TCAP_PRJ_RR_INCORRECT_PARAMETER		0x0303	/* incorrect parameter */
#define TCAP_PRJ_RE_UNRECOGNIZED_CORRELATION_ID	0x0401	/* unrecognized correlation id */
#define TCAP_PRJ_RE_UNEXPECTED_RETURN_ERROR	0x0402	/* unexpected return error */
#define TCAP_PRJ_RE_UNRECOGNIZED_ERROR		0x0403	/* unrecognized error */
#define TCAP_PRJ_RE_UNEXPECTED_ERROR		0x0404	/* unexpected error */
#define TCAP_PRJ_RE_INCORRECT_PARAMETER		0x0405	/* incorrect parameter */

/*
 *  TC_INFO_REQ
 */
typedef struct TC_info_req {
	t_uscalar_t PRIM_type;		/* Always TC_INFO_REQ */
} TC_info_req_t;

/*
 *  TC_INFO_ACK
 */
typedef struct TC_info_ack {
	t_scalar_t PRIM_type;		/* always TC_INFO_ACK */
	t_scalar_t TSDU_size;		/* maximum TSDU size */
	t_scalar_t ETSDU_size;		/* maximum ETSDU size */
	t_scalar_t CDATA_size;		/* connect data size */
	t_scalar_t DDATA_size;		/* disconnect data size */
	t_scalar_t ADDR_size;		/* maximum address size */
	t_scalar_t OPT_size;		/* maximum options size */
	t_scalar_t TIDU_size;		/* transaction interface data size */
	t_scalar_t SERV_type;		/* service type */
	t_scalar_t CURRENT_state;	/* current state */
	t_scalar_t PROVIDER_flag;	/* provider flags */
	t_scalar_t TCI_version;		/* TCI version */
} TC_info_ack_t;

/*
 *  TC_BIND_REQ
 */
typedef struct TC_bind_req {
	t_uscalar_t PRIM_type;
	t_uscalar_t ADDR_length;	/* address length */
	t_uscalar_t ADDR_offset;	/* address offset */
	t_uscalar_t XACT_number;	/* maximum outstanding transaction reqs. */
	t_uscalar_t BIND_flags;		/* bind flags */
} TC_bind_req_t;

/*
 *  TC_BIND_ACK
 */
typedef struct TC_bind_ack {
	t_uscalar_t PRIM_type;
	t_uscalar_t ADDR_length;
	t_uscalar_t ADDR_offset;
	t_uscalar_t XACT_number;
	t_uscalar_t TOKEN_value;
} TC_bind_ack_t;

/*
 *  TC_SUBS_BIND_REQ
 */
typedef struct TC_subs_bind_req {
	t_uscalar_t PRIM_type;
} TC_subs_bind_req_t;

/*
 *  TC_SUBS_BIND_ACK
 */
typedef struct TC_subs_bind_ack {
	t_uscalar_t PRIM_type;
} TC_subs_bind_ack_t;

/*
 *  TC_SUBS_UNBIND_REQ
 */
typedef struct TC_subs_unbind_req {
	t_uscalar_t PRIM_type;
} TC_subs_unbind_req_t;

/*
 *  TC_UNBIND_REQ
 */
typedef struct TC_unbind_req {
	t_uscalar_t PRIM_type;		/* Always TC_UNBIND_REQ */
} TC_unbind_req_t;

/*
 *  TC_OK_ACK
 */
typedef struct TC_ok_ack {
	t_uscalar_t PRIM_type;		/* Always TC_OK_ACK */
	t_uscalar_t CORRECT_prim;	/* correct primitive */
} TC_ok_ack_t;

/*
 *  TC_ERROR_ACK
 */
typedef struct TC_error_ack {
	t_uscalar_t PRIM_type;
	t_uscalar_t ERROR_prim;
	t_uscalar_t TRPI_error;
	t_uscalar_t UNIX_error;
	t_uscalar_t DIALOG_id;
	t_uscalar_t INVOKE_id;
} TC_error_ack_t;

/*
 *  TC_OPTMGMT_REQ
 */
typedef struct TC_optmgmt_req {
	t_uscalar_t PRIM_type;
	t_uscalar_t OPT_length;
	t_uscalar_t OPT_offset;
	t_uscalar_t MGMT_flags;
} TC_optmgmt_req_t;

/*
 *  TC_OPTMGMT_ACK
 */
typedef struct TC_optmgmt_ack {
	t_uscalar_t PRIM_type;
	t_uscalar_t OPT_length;
	t_uscalar_t OPT_offset;
	t_uscalar_t MGMT_flags;
} TC_optmgmt_ack_t;

/*
 *  TC_UNI_REQ,  Send unidirctional message.  One M_PROTO block followed by one or more M_DATA
 *  blocks containing User Information.  Components to be delivered in the unstructured dialog must
 *  have been previously provided with the same Dialog Id and using the component handling request
 *  primitives.  An Application Context is required if there is User Information in attached M_DATA
 *  blocks.
 *
 *  Note:   Source Address may be implicitly associatedw tih the access point at which the primitive
 *	    is being issued.
 *
 *  Note:   Dialog identifier has only local significance and is used between the local TC-User and
 *	    TC-Provider to refer to a dialog.
 */
typedef struct TC_uni_req {
	t_uscalar_t PRIM_type;		/* Always TC_UNI_REQ */
	t_uscalar_t SRC_length;		/* Source address length */
	t_uscalar_t SRC_offset;		/* Source address offset */
	t_uscalar_t DEST_length;	/* Destination address length */
	t_uscalar_t DEST_offset;	/* Destination address offset */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
} TC_uni_req_t;

/*
 *  TC_UNI_RES,  Received unidirectional message.  One M_PROTO block followed by one or more M_DATA
 *  blocks containing User Information.  Components to be delivered from the unstructured dialog
 *  will be indicated using the component handling indication primitives.  An Application Context
 *  will be present where there is User Information in attached M_DATA blocks.
 *
 *  Note:   When QOS is provided by SCCP, QOS must be passed up to the TC-User.
 *
 *  Note:   When Application Context is provided in the corresponding message, it must be passed up
 *	    in the indication.
 */
typedef struct TC_uni_ind {
	t_uscalar_t PRIM_type;		/* Always TC_UNI_IND */
	t_uscalar_t SRC_length;		/* Source address length */
	t_uscalar_t SRC_offset;		/* Source address offset */
	t_uscalar_t DEST_length;	/* Destination address length */
	t_uscalar_t DEST_offset;	/* Destination address offset */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t COMP_flags;		/* Components preset flag */
} TC_uni_ind_t;

/*
 *  TC_BEGIN_REQ.  Requests the opening of a dialog.  One M_PROTO block followed by one or more
 *  M_DATA blocks containing User Information.  Components to be delivered in the structured dialog
 *  must have been previously provided wtih the same Dialog Id and using the component handling
 *  request primitives.  An Application Context is required if there is User Information in attached
 *  M_DATA blocks.
 *
 *  Also T_QUERY_REQ for ANSI.
 */
typedef struct TC_begin_req {
	t_uscalar_t PRIM_type;		/* Always TC_BEGIN_REQ */
	t_uscalar_t SRC_length;		/* Source address length */
	t_uscalar_t SRC_offset;		/* Source address offset */
	t_uscalar_t DEST_length;	/* Destination address length */
	t_uscalar_t DEST_offset;	/* Destination address offset */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t COMP_flags;		/* For use with ANSI QWP/QWOP */
} TC_begin_req_t;

typedef struct TC_begin_req TC_query_req;

/*
 *  TC_BEGIN_IND.  Indicates the opening of a dialog.  One M_PROTO block followed by one or more
 *  M_DATA blocks containing User Information.  Components to be delivered in the structured dialog
 *  will be subsequently indicated with the same Dialog Id and using the component handling
 *  indication primitives.  An Application Context is present if there is User Information in
 *  attached M_DATA blocks.
 *
 *  Also T_QUERY_IND for ANSI.
 */
typedef struct TC_begin_ind {
	t_uscalar_t PRIM_type;		/* Always TC_BEGIN_IND */
	t_uscalar_t SRC_length;		/* Source address length */
	t_uscalar_t SRC_offset;		/* Source address offset */
	t_uscalar_t DEST_length;	/* Destination address length */
	t_uscalar_t DEST_offset;	/* Destination address offset */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t COMP_flags;		/* For use with ANSI QWP/QWOP */
} TC_begin_ind_t;

typedef struct TC_begin_ind TC_query_ind;

/*
 *  TC_END_REQ.
 *
 *  Also TC_RESP_REQ for ANSI.
 */
typedef struct TC_end_req {
	t_uscalar_t PRIM_type;		/* Always TC_END_REQ */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t TERM_scenario;	/* Reason for termination */
} TC_end_req_t;

typedef struct TC_end_req TC_resp_req_t;

/*
 *  TC_END_IND.
 *
 *  Also TC_RESP_IND for ANSI.
 */
typedef struct TC_end_ind {
	t_uscalar_t PRIM_type;		/* Always TC_END_IND */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t COMP_flags;		/* Components present flag */
} TC_end_ind_t;

typedef struct TC_end_ind TC_resp_ind_t;

/*
 *  TC_CONT_REQ.  The first TC_CONT_REQ after a TC_BEGIN_IND requests that the dialog be confirmed
 *  and may contain the Source address and Application Context parameters.  Once these have been
 *  provided on the first TC_CONT_REQ, they are in place for the remainder of the dialog.
 *  Subsequent TC_CONT_REQ primitives do not contain the SRC and CONTEXT parameters.
 *
 *  Also TC_CONV_REQ for ANSI.
 */
typedef struct TC_begin_res {
	t_uscalar_t PRIM_type;		/* Always TC_CONT_REQ */
	t_uscalar_t SRC_length;		/* Source address length */
	t_uscalar_t SRC_offset;		/* Source address offset */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t COMP_flags;		/* For use with ANSI CWP/CWOP */
} TC_begin_res_t;

typedef struct TC_cont_req {
	t_uscalar_t PRIM_type;		/* Always TC_CONT_REQ */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t COMP_flags;		/* For use with ANSI CWP/CWOP */
} TC_cont_req_t;

typedef struct TC_cont_req TC_conv_req_t;

/*
 *  TC_CONT_IND.  The first TC_CONT_IND after a TC_BEGIN_REQ indicates that the dialog is confirmed
 *  but may contain the Source address and Application Context parameters.  Once these have been
 *  provided on the first TC_CONT_IND, they are in place for the remainder of the dialog.
 *  Subsequent TC_CONT_IND primitives will not contain the SRC and CONTEXT parameters.
 *
 *  Also TC_CONV_IND for ASNI.
 */
typedef struct TC_begin_con {
	t_uscalar_t PRIM_type;		/* Always TC_BEGIN_CON */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t COMP_flags;		/* For use with ANSI CWP/CWOP */
} TC_begin_con_t;

typedef struct TC_cont_ind {
	t_uscalar_t PRIM_type;		/* Always TC_CONT_IND */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t COMP_flags;		/* For use with ANSI CWP/CWOP */
} TC_cont_ind_t;

typedef struct TC_cont_ind TC_conv_ind_t;

/*
 *  TC_ABORT_REQ.
 *
 *  Note:   Application context is only present if the abort reason indicates "application context
 *	    not supported".
 */
typedef struct TC_abort_req {
	t_uscalar_t PRIM_type;		/* Always TC_ABORT_REQ */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t ABORT_reason;	/* Abort reason */
} TC_abort_req_t;

/*
 *  TC_ABORT_IND.
 *
 *  Note:   Application context is only present if the abort reason indicates "application context
 *	    not supported".
 */
typedef struct TC_abort_ind {
	t_uscalar_t PRIM_type;		/* Always TC_ABORT_IND */
	t_uscalar_t OPT_length;		/* Options associated with the primitive */
	t_uscalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t ABORT_reason;	/* Abort reason */
	t_uscalar_t ORIGINATOR;		/* Either User or Provider originated */
} TC_abort_ind_t;

/*
 *  TC_NOTICE_IND.
 */
typedef struct TC_notice_ind {
	t_uscalar_t PRIM_type;		/* Always TC_NOTICE_IND */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t REPORT_cause;	/* Report cause */
} TC_notice_ind_t;

/*
 *  Component handling primitives.
 */

/*
 *  TC_INVOKE_REQ.  This primitive is one M_PROTO message block followed by zero or more M_DATA
 *  blocks containing the parameters of the operation.
 */
typedef struct TC_invoke_req {
	t_uscalar_t PRIM_type;		/* Always TC_INVOKE_REQ */
	t_uscalar_t DIALOG_id;		/* Dialog identifier */
	t_uscalar_t PROTOCOL_class;	/* Application protocol class */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t LINKED_id;		/* Linked Invoke Identifier */
	t_uscalar_t OPERATION;		/* Requested operation to invoke */
	t_uscalar_t MORE_flag;		/* Not last */
	t_uscalar_t TIMEOUT;		/* Timeout */
} TC_invoke_req_t;

/*
 *  TC_INVOKE_IND.  This primitive is one M_PROTO message block followed by zero or more M_DATA
 *  blocks containing the parameters of the operation.
 *
 *  Note:   Dialog Id is ignored for Class 4 (TC_UNI_IND) operations.
 */
typedef struct TC_invoke_ind {
	t_uscalar_t PRIM_type;		/* Always TC_INVOKE_IND */
	t_uscalar_t DIALOG_id;		/* Dialog identifier */
	t_uscalar_t OP_class;		/* Application operation class */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t LINKED_id;		/* Linked Invoke Identifier */
	t_uscalar_t OPERATION;		/* Requested operation to invoke */
	t_uscalar_t MORE_flag;		/* Not last */
} TC_invoke_ind_t;

/*
 *  TC_RESULT_REQ.  This primitive consists of one M_PROTO message block followed by zero or more
 *  M_DATA blocks containing the parameters of the operation.
 */
typedef struct TC_result_req {
	t_uscalar_t PRIM_type;		/* Always TC_RESULT_REQ */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t OPERATION;		/* Requested operation result */
	t_uscalar_t MORE_flag;		/* Not last */
} TC_result_req_t;

/*
 *  TC_RESULT_IND.  This primitive consists of one M_PROTO message block followed by zero or more
 *  M_DATA blocks containing the parameters of the operation.
 *
 *  This primitive is only valid (expected) for operation class 1 and 3.
 */
typedef struct TC_result_ind {
	t_uscalar_t PRIM_type;		/* Always TC_RESULT_IND */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t OPERATION;		/* Requested operation result */
	t_uscalar_t MORE_flag;		/* Not last */
} TC_result_ind_t;

/*
 *  TC_ERROR_REQ.  This primitive consists of one M_PROTO message block followed by zero or more
 *  M_DATA blocks containing the parameters of the error.
 */
typedef struct TC_error_req {
	t_uscalar_t PRIM_type;		/* Always TC_ERROR_REQ */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t ERROR_code;		/* Error code */
	t_uscalar_t MORE_flag;		/* Not last */
} TC_error_req_t;

/*
 *  TC_ERROR_IND.  This primitive consists of one M_PROTO message block followed by zero or more
 *  M_DATA blocks containing the parameters of the error.
 */
typedef struct TC_error_ind {
	t_uscalar_t PRIM_type;		/* Always TC_ERROR_IND */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t ERROR_code;		/* Error code */
} TC_error_ind_t;

/*
 *  TC_REJECT_REQ.  This primitive consists of one M_PROTO message block.
 */
typedef struct TC_reject_req {
	t_uscalar_t PRIM_type;		/* Always TC_REJECT_REQ */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke identifier */
	t_uscalar_t PROBLEM_code;	/* Problem code */
} TC_reject_req_t;

/*
 *  TC_REJECT_IND.  This primitive consists of one M_PROTO message block.
 */
typedef struct TC_reject_ind {
	t_uscalar_t PRIM_type;		/* Always TC_REJECT_IND */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke identifier */
	t_uscalar_t ORIGINATOR;		/* Either User, Local or Remote */
	t_uscalar_t PROBLEM_code;	/* Problem code */
} TC_reject_ind_t;

/*
 *  TC_CANCEL_REQ.  This primitive consists of one M_PROTO message block.
 */
typedef struct TC_cancel_req {
	t_uscalar_t PRIM_type;		/* Always TC_CANCEL_REQ */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke identifier */
} TC_cancel_req_t;

/*
 *  TC_CANCEL_IND.  This primitive consists of one M_PROTO message block.
 */
typedef struct TC_cancel_ind {
	t_uscalar_t PRIM_type;		/* Always TC_CANCEL_REQ */
	t_uscalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke identifier */
} TC_cancel_ind_t;

#endif				/* __SS7_TC_H__ */
