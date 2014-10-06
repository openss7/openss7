/*****************************************************************************

 @(#) $Id: map.h,v 1.1.2.2 2010-11-28 14:21:47 brian Exp $

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

 Last Modified $Date: 2010-11-28 14:21:47 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: map.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:47  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:33  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SS7_MAP_H__
#define __SS7_MAP_H__

#define MAP_INFO_REQ		 0	/* Information request */
#define MAP_BIND_REQ		 1	/* Bind to network address */
#define MAP_UNBIND_REQ		 2	/* Unbind from network address */
#define MAP_OPTMGMT_REQ		 3	/* Options management request */
#define MAP_OPEN_REQ		 4	/* Open dialogue request */
#define MAP_OPEN_RES		 5	/* Accept dialogue request */
#define MAP_REFUSE_RES		 6	/* Refuse dialogue request */
#define MAP_SERV_REQ		 7	/* Service request */
#define MAP_SERV_RES		 8	/* Service response */
#define MAP_DELIM_REQ		 9	/* Delimination request */
#define MAP_CLOSE_REQ		10	/* Close request */
#define MAP_ABORT_REQ		11	/* Abort request */

#define MAP_BIND_ACK		12	/* Bind acknowledgement */
#define MAP_OK_ACK		13	/* Correct acknowledgement */
#define MAP_ERROR_ACK		14	/* Error acknowledgement */
#define MAP_OPEN_IND		15	/* Open dialogue indication */
#define MAP_OPEN_CON		16	/* Open dialogue confirmation */
#define MAP_REFUSE_IND		17	/* Refused dialogue indication */
#define MAP_SERV_IND		18	/* Service indication */
#define MAP_SERV_CON		19	/* Service confirmation */
#define MAP_DELIM_IND		20	/* Delimination indication */
#define MAP_CLOSE_IND		21	/* Close dialogue indication */
#define MAP_ABORT_IND		22	/* Abort dialogure indication */
#define MAP_NOTICE_IND		23	/* Delivery notice indication */

#define MAP_STC1_REQ		24	/* Secure transport class 1 request */
#define MAP_STC2_REQ		25	/* Secure transport class 2 request */
#define MAP_STC3_REQ		26	/* Secure transport class 3 request */
#define MAP_STC4_REQ		27	/* Secure transport class 4 request */

#define MAP_STC1_IND		28	/* Secure transport class 1 indication */
#define MAP_STC2_IND		29	/* Secure transport class 2 indication */
#define MAP_STC3_IND		30	/* Secure transport class 3 indication */
#define MAP_STC4_IND		31	/* Secure transport class 4 indication */

/*
 * MAP_INFO_REQ: - one M_PROTO or M_PCPROTO message block.
 */
typedef struct MAP_info_req {
	t_scalar_t PRIM_type;		/* always MAP_INFO_REQ */
} MAP_info_req_t;

/*
 * MAP_INFO_ACK: - one M_PCPROTO message block.
 */
typedef struct MAP_info_ack {
	t_scalar_t PRIM_type;		/* always MAP_INFO_ACK */
	t_scalar_t TSDU_size;		/* maximum TSDU size */
	t_scalar_t ETSDU_size;		/* maximum ETSDU size */
	t_scalar_t CDATA_size;		/* connect data size */
	t_scalar_t DDATA_size;		/* disconnect data size */
	t_scalar_t ADDR_size;		/* address size */
	t_scalar_t ADDR_length;		/* address length */
	t_scalar_t ADDR_offset;		/* address offset */
	t_scalar_t QOS_length;		/* default QOS values length */
	t_scalar_t QOS_offset;		/* default QOS values offset */
	t_scalar_t QOS_range_length;	/* QOS range length */
	t_scalar_t QOS_range_offset;	/* QOS range offset */
	t_scalar_t OPTIONS_flags;	/* bit masking for options */
	t_scalar_t TIDU_size;		/* transaction interface data size */
	t_scalar_t SERV_type;		/* service type */
	t_scalar_t CURRENT_state;	/* current state */
	t_scalar_t PROVIDER_type;	/* type of provider */
	t_scalar_t NODU_size;		/* optimal TSDU size */
	t_scalar_t PROTOID_length;	/* length of bound protocol ids */
	t_scalar_t PROTOID_offset;	/* offset of bound protocol ids */
	t_scalar_t MAPI_version;	/* supported MAPI version number */
} MAP_info_ack_t;

/*
 * MAP_BIND_REQ: - one M_PROTO message block.
 */
typedef struct MAP_bind_req {
	t_scalar_t PRIM_type;		/* always MAP_BIND_REQ */
	t_scalar_t ADDR_length;		/* length of protocol address */
	t_scalar_t ADDR_offset;		/* offset of protocol address */
	t_scalar_t DIAIND_number;	/* requested number of dialogue indications to be queued */
	t_scalar_t BIND_flags;		/* Bind flags */
	t_scalar_t APPL_CTX_length;	/* Application context length */
	t_scalar_t APPL_CTX_offset;	/* Application context name offset */
} MAP_bind_req_t;

/*
 * MAP_BIND_ACK: - one M_PROTO message block.
 */
typedef struct MAP_bind_ack {
	t_scalar_t PRIM_type;		/* always MAP_BIND_ACK */
	t_scalar_t ADDR_length;		/* length of protocol address */
	t_scalar_t ADDR_offset;		/* offset of protocol address */
	t_scalar_t DIAIND_number;	/* requested number of dialogue indications to be queued */
	t_scalar_t BIND_flags;		/* Bind flags */
	t_scalar_t APPL_CTX_length;	/* Application context length */
	t_scalar_t APPL_CTX_offset;	/* Application context name offset */
} MAP_bind_ack_t;

/*
 * MAP_UNBIND_REQ: - one M_PROTO message block.
 */
typedef struct MAP_unbind_req {
	t_scalar_t PRIM_type;		/* always MAP_UNBIND_REQ */
} MAP_unbind_req_t;

/*
 * MAP_OPTMGMT_REQ: - one M_PROTO or M_PCPROTO message block.
 */
typedef struct MAP_optmgmt_req {
	t_scalar_t PRIM_type;		/* always MAP_OPTMGMT_REQ */
} MAP_optmgmt_req_t;

/*
 * MAP_ADDR_REQ: - one M_PROTO or M_PCPROTO message block.
 */
typedef struct MAP_addr_req {
	t_scalar_t PRIM_type;		/* always MAP_ADDR_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
} MAP_addr_req_t;

/*
 * MAP_ADDR_ACK: - on M_PROTO or M_PCPROTO message block.
 */
typedef struct MAP_addr_ack {
	t_scalar_t PRIM_type;		/* always MAP_ADDR_ACK */
	t_scalar_t LOCADDR_length;	/* Local address length */
	t_scalar_t LOCADDR_offset;	/* Local address offset */
	t_scalar_t REMADDR_length;	/* Remote address length */
	t_scalar_t REMADDR_offset;	/* Remote address offset */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
} MAP_addr_ack_t;

/*
 * MAP_CAPABILITY_REQ: one M_PROTO or M_PCPROTO message block.
 */
typedef struct MAP_capability_req {
	t_scalar_t PRIM_type;		/* always MAP_CAPABILITY_REQ */
	t_scalar_t CAP_bits1;		/* Capability bits 1 */
} MAP_capability_req_t;

/*
 * MAP_CAPABILITY_ACK: one M_PROTO or M_PCPROTO message block.
 */
typedef struct MAP_capability_ack {
	t_scalar_t PRIM_type;		/* always MAP_CAPABILITY_ACK */
	t_scalar_t CAP_bits1;		/* Capability bits 1 */
	struct MAP_info_ack INFO_ack;	/* Info acknowledgement */
	t_scalar_t TOKEN_value;		/* Accept token value */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t INVOKE_id;		/* Invoke identifier */
} MAP_capability_ack_t;

#define MAP_C1_INFO		(1<<0)
#define MAP_C1_TOKEN		(1<<1)
#define MAP_C1_DIALOG_ID	(1<<2)
#define MAP_C1_INVOKE_ID	(1<<2)
#define MAP_C1_CAP_BITS2	(1<<31)

/*
 * MAP_OK_ACK: - one M_PCPROTO message block.
 */
typedef struct MAP_ok_ack {
	t_scalar_t PRIM_type;		/* always MAP_OK_ACK */
	t_scalar_t CORRECT_prim;	/* primitive received correctly */
} MAP_ok_ack_t;

/*
 * MAP_ERROR_ACK: - one M_PCPROTO message block.
 */
typedef struct MAP_error_ack {
	t_scalar_t PRIM_type;		/* always MAP_ERROR_ACK */
	t_scalar_t ERROR_prim;		/* primitive in error */
	t_scalar_t MAP_error;		/* MAP error */
	t_scalar_t UNIX_error;		/* UNIX system error number */
} MAP_error_ack_t;

/*
 * MAP_OPEN_REQ: - one M_PROTO message block followed by zero or more M_DATA
 * message blocks containing specific-information.  When originating address
 * is unspecified, the bound originating address is used.  Options may include
 * source reference, and destination reference. (3GPP TS 29.002 7.3.1)
 */
typedef struct MAP_open_req {
	t_scalar_t PRIM_type;		/* always MAP_OPEN_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t APPL_CTX_length;	/* Application context length */
	t_scalar_t APPL_CTX_offset;	/* Application context name offset */
	t_scalar_t DEST_length;		/* Destination address length */
	t_scalar_t DEST_offset;		/* Destination address offset */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_open_req_t;

/*
 * MAP_OPEN_IND:- one M_PROTO message block followed by zero or more M_DATA
 * message blocks containing specific-information.  When originating address
 * is not specified, the bound originating address applies.  Options may
 * include source reference and destination reference.  (3GPP TS 29.002 7.3.1)
 */
typedef struct MAP_open_ind {
	t_scalar_t PRIM_type;		/* always MAP_OPEN_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t APPL_CTX_length;	/* Application context length */
	t_scalar_t APPL_CTX_offset;	/* Application context name offset */
	t_scalar_t DEST_length;		/* Destination address length */
	t_scalar_t DEST_offset;		/* Destination address offset */
	t_scalar_t ORIG_length;		/* Originating address length */
	t_scalar_t ORIG_offset;		/* Originating address offset */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_open_ind_t;

/*
 * MAP_OPEN_RES:- one M_PROTO message block followed by zero or more M_DATA
 * message blocks containing specific-information. (3GPP TS 29.002 7.3.1)
 */
typedef struct MAP_open_res {
	t_scalar_t PRIM_type;		/* always MAP_OPEN_RES */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t APPL_CTX_length;	/* Application context length */
	t_scalar_t APPL_CTX_offset;	/* Application context name offset */
	t_scalar_t RESP_length;		/* Responding address length */
	t_scalar_t RESP_offset;		/* Responding address offset */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_open_res_t;

/*
 * MAP_OPEN_CON: - one M_PROTO message block followed by zero or more M_DATA
 * message blocks containing specific-information. (3GPP TS 29.002 7.3.1)
 */
typedef struct MAP_open_con {
	t_scalar_t PRIM_type;		/* always MAP_OPEN_CON */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t APPL_CTX_length;	/* Application context length */
	t_scalar_t APPL_CTX_offset;	/* Application context name offset */
	t_scalar_t RESP_length;		/* Responding address length */
	t_scalar_t RESP_offset;		/* Responding address offset */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_open_con_t;

/*
 * MAP_REFUSE_RES:- one M_PROTO message block followed by zero or more M_DATA
 * blocks containing specific-information. (3GPP TS 29.002 7.3.1)
 */
typedef struct MAP_refuse_res {
	t_scalar_t PRIM_type;		/* always MAP_REFUSE_RES */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t APPL_CTX_length;	/* Application context length */
	t_scalar_t APPL_CTX_offset;	/* Application context name offset */
	t_scalar_t RESP_length;		/* Responding address length */
	t_scalar_t RESP_offset;		/* Responding address offset */
	t_scalar_t REFUSE_reason;	/* Refuse reason */
} MAP_refuse_res_t;

/*
 * MAP_REFUSE_IND:- one M_PROTO message block followed by zero or more M_DATA
 * blocks containing specific-information. (3GPP TS 29.002 7.3.1)
 */
typedef struct MAP_refuse_ind {
	t_scalar_t PRIM_type;		/* always MAP_REFUSE_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t APPL_CTX_length;	/* Application context length */
	t_scalar_t APPL_CTX_offset;	/* Application context name offset */
	t_scalar_t RESP_length;		/* Responding address length */
	t_scalar_t RESP_offset;		/* Responding address offset */
	t_scalar_t REFUSE_reason;	/* Refuse reason */
	t_scalar_t PROVIDER_error;	/* Provider error */
} MAP_refuse_ind_t;

/* REFUSE_reason - refuse reason values. */
#define MAP_REFR_UNSPECIFIED		0	/* no reason given */
#define MAP_REFR_APPL_CTX_NOT_SUPP	1	/* application-context not supported */
#define MAP_REFR_INVALID_DEST_REF	2	/* invalid destination reference */
#define MAP_REFR_INVALID_ORIG_REF	3	/* invalid originating reference */
#define MAP_REFR_UNREACHABLE		4	/* remote node not reachable */
#define MAP_REFR_VERSION_INCOMPAT	5	/* potential version incompatibility */
#define MAP_REFR_NO_SECURITY		6	/* secured transport not possible */
#define MAP_REFR_PROTECTION		7	/* transport protection not adequate */

/*
 * MAP_SERV_REQ:- one M_PROTO message block followed by zero or more M_DATA
 * message blocks containing the parameters of the operaiton.
 *
 * This primitive maps onto a TC-INVOKE request primitive.
 */
typedef struct MAP_serv_req {
	t_scalar_t PRIM_type;		/* always MAP_SERV_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t PROTOCOL_class;	/* Application protocol class */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t LINKED_id;		/* Linked Invoke Identifier */
	t_uscalar_t OPERATION;		/* Requested operation to invoke */
	t_uscalar_t MORE_flag;		/* Not last */
	t_uscalar_t TIMEOUT;		/* Timeout */
} MAP_serv_req_t;

/*
 * MAP_SERV_IND:- one M_PROTO message block followed by zero or more M_DATA
 * message blocks containing the parameters of the operation.
 *
 * This primitive maps from a TC-INVOKE indication primitive.
 */
typedef struct MAP_serv_ind {
	t_scalar_t PRIM_type;		/* always MAP_SERV_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t OP_class;		/* Application operation class */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t LINKED_id;		/* Linked Invoke Identifier */
	t_uscalar_t OPERATION;		/* Requested operation to invoke */
	t_uscalar_t MORE_flag;		/* Not last */
} MAP_serv_ind_t;

/*
 * MAP_SERV_RES
 *
 * This primitive maps onto TC-RESULT-L, TC-U-ERROR, TC-U-REJECT or TC-INVOKE
 * request primitives.  The mapping is determined by the parameters contained
 * in the response primitive.
 */
typedef struct MAP_serv_res {
	t_scalar_t PRIM_type;		/* always MAP_SERV_RES */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t OPERATION;		/* Requested operation result */
	t_uscalar_t ERROR_code;		/* Error code */
	t_uscalar_t PROBLEM_code;	/* Problem code */
	t_uscalar_t MORE_flag;		/* Not last */
} MAP_serv_res_t;

/*
 * MAP_SERV_CON
 *
 * This primitive maps from a TC-RESULT-L, TC-U-ERROR, TC-INVOKE or
 * TC-L-CANCEL indication primitive.  A TC-INVOKE only maps to this primitive
 * for TC class 4 operations where the operation is used to pass a result of
 * another class 2 or class 4 operation.
 */
typedef struct MAP_serv_con {
	t_scalar_t PRIM_type;		/* always MAP_SERV_CON */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_uscalar_t INVOKE_id;		/* Invoke Identifier */
	t_uscalar_t OPERATION;		/* Requested operation result */
	t_uscalar_t ERROR_code;		/* Error code */
	t_uscalar_t ORIGINATOR;		/* Either User, Local or Remote */
	t_uscalar_t PROBLEM_code;	/* Problem code */
} MAP_serv_con_t;

/* PROVIDER_error - provider error values. */
#define MAP_PERR_DUPLICATE_INVOKE_ID
#define MAP_PERR_NOT_SUPPORTED_SERVICE
#define MAP_PERR_MISTYPED_PARAMETER
#define MAP_PERR_RESOURCE_LIMITATION
#define MAP_PERR_INITIATING_RELEASE
#define MAP_PERR_UNEXPECTED_RESPONSE
#define MAP_PERR_SERVICE_COMPLETION_FAILURE
#define MAP_PERR_NO_RESPONSE
#define MAP_PERR_INVALID_RESPONSE

/* USER_error - user error values. */
#define MAP_UERR_SYSTEM_FAILURE
#define MAP_UERR_DATA_MISSING
#define MAP_UERR_UNEXPECTED_DATA_VALUE
#define MAP_UERR_RESOURCE_LIMITATION
#define MAP_UERR_INITIATING_RELEASE
#define MAP_UERR_FACILITY_NOT_SUPPORTED
#define MAP_UERR_INCOMPATIBLE_TERMINAL
#define MAP_UERR_UNKNOWN_SUBSCRIBER
#define MAP_UERR_NUMBER_CHANGED
#define MAP_UERR_UNKNOWN_MSC
#define MAP_UERR_UNIDENTIFIED_SUBSCRIBER
#define MAP_UERR_UNALLOCATED_ROAMING_NUMBER
#define MAP_UERR_UNKNOWN_EQUIPMENT
#define MAP_UERR_UNKNOWN_LOCATION_AREA
#define MAP_UERR_ROAMING_NOT_ALLOWED
#define MAP_UERR_ILLEGAL_SUBSCRIBER
#define MAP_UERR_BEARER_SERVICE_NOT_PROVISIONED
#define MAP_UERR_TELESERVICE_NOT_PROVISIONED
#define MAP_UERR_ILLEGAL_EQUIPMENT
#define MAP_UERR_NO_HANDOVER_UMBER
#define MAP_UERR_SUBSEQUENT_HANDOVER_FAILURE
#define MAP_UERR_TARGET_CELL_OUTSIDE_AREA
#define MAP_UERR_TRACING_BUFFER_FULL
#define MAP_UERR_NO_ROAMING_NUMBER
#define MAP_UERR_ABSENT_SUBSCRIBER
#define MAP_UERR_BUSY_SUBSCRIBER
#define MAP_UERR_NO_SUBSCRIBER_REPLY
#define MAP_UERR_FORWARDING_VIOLATION
#define MAP_UERR_CUG_REJECT
#define MAP_UERR_CALL_BARRED
#define MAP_UERR_OPTIMAL_ROUTING_DISALLOWED
#define MAP_UERR_FORWARDING_FAILED
#define MAP_UERR_SS_CALL_BARRED
#define MAP_UERR_SS_ILLEGAL_OPERATION
#define MAP_UERR_SS_ERROR_STATUS
#define MAP_UERR_SS_SUBSCRIPTION_VIOLATION
#define MAP_UERR_SS_INCOMPATIBILITY
#define MAP_UERR_SS_NEGATIVE_PASSWORD_CHECK
#define MAP_UERR_SS_PASSWORD_REGISTRATION_FAILURE
#define MAP_UERR_SS_EXCESSIVE_PASSWORD_ATTEMPTS
#define MAP_UERR_SS_USSD_BUSY
#define MAP_UERR_SS_UNKNOWN_ALPHABET
#define MAP_UERR_SS_SHORT_TERM_DENIAL
#define MAP_UERR_SS_LONG_TERM_DENIAL
#define MAP_UERR_SM_MEMORY_CAPACITY_EXCEEDED
#define MAP_UERR_SM_MS_PROTOCOL_ERROR
#define MAP_UERR_SM_MS_NOT_EQUIPPED
#define MAP_UERR_SM_UNKNOWN_SERVICE_CENTER
#define MAP_UERR_SM_SC_CONGESTION
#define MAP_UERR_SM_INVALID_SME_ADDRESS
#define MAP_UERR_SM_NOT_SC_SUBSCRIBER
#define MAP_UERR_SM_MESSAGE_WAITING_LIST_FULL
#define MAP_UERR_SM_SUB_BUSY_ANOTHER_SM
#define MAP_UERR_SM_SUB_BUSY_TIME_EXCEEDED
#define MAP_UERR_SM_ABSENT_SUBSCRIBER_SM
#define MAP_UERR_LS_UNAUTH_REQUESTING_NET
#define MAP_UERR_LS_UNAUTH_LSC_CLIENT
#define MAP_UERR_LS_UNAUTH_LSC_CLIENT_NOT_IN_PEL
#define MAP_UERR_LS_UNAUTH_LSC_CALL_NOT_SETUP
#define MAP_UERR_LS_UNAUTH_LSC_DISALLOWED
#define MAP_UERR_LS_UNAUTH_LSC_PRIVACY_CLASS
#define MAP_UERR_LS_UNAUTH_LSC_UNRELATED_EXT_CLIENT
#define MAP_UERR_LS_UNAUTH_LSC_RELATED_EXT_CLIENT
#define MAP_UERR_LS_UNAUTH_LSC_NA_PRIVACY_OVERRIDE
#define MAP_UERR_LS_PM_CONGESTION
#define MAP_UERR_LS_PM_INSUFFICIENT_RESOURCES
#define MAP_UERR_LS_PM_INSUFFICIENT_MEASUREMENT_DATA
#define MAP_UERR_LS_PM_INCONSISTENT_MEASUREMENT_DATA
#define MAP_UERR_LS_PM_LOCATION_PROCEDURE_INCOMPLETE
#define MAP_UERR_LS_PM_QOS_NOT_ATTAINABLE
#define MAP_UERR_LS_PM_METHOD_NOT_AVAILABLE_NETWORK
#define MAP_UERR_LS_PM_METHOD_NOT_AVAILABLE_LOCAL
#define MAP_UERR_LS_PM_UNKNOWN_LCS_CLIENT

/*
 * MAP_DELIM_REQ:- one M_PROTO message block. (3GPP TS 29.002 7.3.3)
 */
typedef struct MAP_delim_req {
	t_scalar_t PRIM_type;		/* always MAP_DELIM_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
} MAP_delim_req_t;

/*
 * MAP_DELIM_IND:- one M_PROTO message block. (3GPP TS 29.002 7.3.3)
 */
typedef struct MAP_delim_ind {
	t_scalar_t PRIM_type;		/* always MAP_DELIM_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
} MAP_delim_ind_t;

/*
 * MAP_CLOSE_REQ:- one M_PROTO message block followed by zero or more M_DATA
 * message blocks containint specific-information. (3GPP TS 29.002 7.3.2).
 */
typedef struct MAP_close_req {
	t_scalar_t PRIM_type;		/* always MAP_CLOSE_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t TERM_scenario;	/* Method of release */
} MAP_close_req_t;

/*
 * RELEASE_method - release method values
 *
 * Note that these values map directly onto the corresponding transaction
 * termination scenarios.
 */
#define MAP_RSL_UNSPECIFIED	0	/* termination unspecified */
#define MAP_RLS_NORMAL		1	/* termination basic */
#define MAP_RLS_PREARRANGED	2	/* termination prearranged */

/*
 * MAP_CLOSE_IND:- one M_PROTO message block followed by zero or more M_DATA
 * message blocks containing specific-information. (3GPP TS 29.002 7.3.2)
 */
typedef struct MAP_close_ind {
	t_scalar_t PRIM_type;		/* always MAP_CLOSE_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
} MAP_close_ind_t;

/*
 * MAP_ABORT_REQ:- one M_PROTO message block followed by zero or more M_DATA
 * message blocks containntg specific-information.  Options may include
 * diagnostic information. (3GPP TS 29.002 7.3.4) This primitive maps to the
 * MAP-U-ABORT request primitive.
 */
typedef struct MAP_abort_req {
	t_scalar_t PRIM_type;		/* always MAP_ABORT_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_scalar_t ABORT_reason;	/* Abort reason (user reasons only) */
} MAP_abort_req_t;

/*
 * MAP_ABORT_IND:- one M_PROTO message block followed by zero or more M_DATA
 * message blocks containing specific-information.  Options may include
 * diagnostic information.  (3GPP TS 29.002 7.3.4) This primitive maps form
 * the MAP-U-ABORT indication and MAP-P-ABORT indication primitives.
 */
typedef struct MAP_abort_ind {
	t_scalar_t PRIM_type;		/* always MAP_ABORT_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
	t_scalar_t ABORT_reason;	/* Abort reason */
	t_scalar_t SOURCE;		/* Source */
} MAP_abort_ind_t;

/* SOURCE:- - problem source values. */
#define MAP_PSRC_UNKNOWN	0	/* source Unknown */
#define MAP_PSRC_USER		1	/* source User */
#define MAP_PSRC_MAP		2	/* source MAP */
#define MAP_PSRC_TC		3	/* source TC */
#define MAP_PSRC_NSP		4	/* source SCCP */

/* ABORT_reason:- user reason values, when SOURCE == MAP_PSRC_USER. */
#define MAP_USRR_RESOURCE_LIMITATION	1
#define MAP_USRR_CONGESTION		2
#define MAP_USRR_RESOURE_UNAVAILABLE	3
#define MAP_USRR_NONCONGESTION		4
#define MAP_USRR_CANCELLATION		5
#define MAP_USRR_DIAGNOSTIC_CANCEL	6
#define MAP_USRR_PROCEDURE_ERROR	7
#define MAP_USRR_PROCEDURE_TERMINATION	8

/* ABORT_reason - provider reason values, when SOURCE != MAP_PSRC_USER. */
#define MAP_PRVR_UNSPECIFIED		0	/* source Unknown */
#define MAP_PRVR_PROVIDER_MALFUNCTION	1	/* source MAP or TC */
#define MAP_PRVR_DIALOG_RELEASED	2	/* source TC */
#define MAP_PRVR_RESOURCE_LIMITATION	3	/* source MAP or TC */
#define MAP_PRVR_MAINTENANCE_ACTIVITY	4	/* source MAP or NSP */
#define MAP_PRVR_VERSION_INCOMPAT	5	/* source TC */
#define MAP_PRVR_ABNORMAL_MAP_DIALOG	6	/* source MAP */

/*
 * MAP_NOTICE_IND: - one M_PROTO message block. (3GPP TS 29.002 7.3.6)
 */
typedef struct MAP_notice_ind {
	t_scalar_t PRIM_type;		/* always MAP_NOTICE_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t PROBLEM_diag;	/* Problem Diagnostic */
} MAP_notice_ind_t;

/* PROBLEM_diag - problem diagnostic values. */
#define MAP_PDIA_ABNORMAL_EVENT_PEER	1	/* abnormal event detected by the peer */
#define MAP_PDIA_RESPONSE_REJECTED	2	/* response rejected by the peer */
#define MAP_PDIA_ABNORMAL_EVENT_LOCAL	3	/* abnormal event received from the peer */
#define MAP_PDIA_UNDELIVERABLE		4	/* message cannot be delivered to the peer */

/*
 * MAP_STC1_REQ
 */
typedef struct MAP_stc1_req {
	t_scalar_t PRIM_type;		/* alwasy MAP_STC1_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_stc1_req_t;

/*
 * MAP_STC1_IND
 */
typedef struct MAP_stc1_ind {
	t_scalar_t PRIM_type;		/* always MAP_STC1_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_stc1_ind_t;

/*
 * MAP_STC2_REQ
 */
typedef struct MAP_stc2_req {
	t_scalar_t PRIM_type;		/* always MAP_STC2_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_stc2_req_t;

/*
 * MAP_STC2_IND
 */
typedef struct MAP_stc2_ind {
	t_scalar_t PRIM_type;		/* always MAP_STC2_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_stc2_ind_t;

/*
 * MAP_STC3_REQ
 */
typedef struct MAP_stc3_req {
	t_scalar_t PRIM_type;		/* always MAP_STC3_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_stc3_req_t;

/*
 * MAP_STC3_IND
 */
typedef struct MAP_stc3_ind {
	t_scalar_t PRIM_type;		/* always MAP_STC3_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_stc3_ind_t;

/*
 * MAP_STC4_REQ
 */
typedef struct MAP_stc4_req {
	t_scalar_t PRIM_type;		/* always MAP_STC4_REQ */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_stc4_req_t;

/*
 * MAP_STC4_IND
 */
typedef struct MAP_stc4_ind {
	t_scalar_t PRIM_type;		/* always MAP_STC4_IND */
	t_scalar_t DIALOG_id;		/* Dialog Identifier */
	t_scalar_t OPT_length;		/* Options associated with the primitive */
	t_scalar_t OPT_offset;		/* Options associated wtih the primitive */
} MAP_stc4_ind_t;

union MAP_primitives {
	t_scalar_t type;
	struct MAP_info_req info_req;
	struct MAP_info_ack info_ack;
	struct MAP_bind_req bind_req;
	struct MAP_bind_ack bind_ack;
	struct MAP_unbind_req unbind_req;
	struct MAP_optmgmt_req optmgmt_req;
	struct MAP_addr_req addr_req;
	struct MAP_addr_ack addr_ack;
	struct MAP_capability_req capability_req;
	struct MAP_capability_ack capabiltiy_ack;
	struct MAP_ok_ack ok_ack;
	struct MAP_error_ack error_ack;
	struct MAP_open_req open_req;
	struct MAP_open_ind open_ind;
	struct MAP_open_res open_res;
	struct MAP_open_con open_con;
	struct MAP_refuse_req refuse_req;
	struct MAP_refuse_ind refuse_ind;
	struct MAP_serv_req serv_req;
	struct MAP_serv_ind serv_ind;
	struct MAP_serv_res serv_res;
	struct MAP_serv_con serv_con;
	struct MAP_delim_req delim_req;
	struct MAP_delim_ind delim_ind;
	struct MAP_close_req close_req;
	struct MAP_close_ind close_ind;
	struct MAP_abort_req u_abort_req;
	struct MAP_abort_ind abort_ind;
	struct MAP_notice_ind notice_ind;
	struct MAP_stc1_req stc1_req;
	struct MAP_stc1_ind stc1_ind;
	struct MAP_stc2_req stc2_req;
	struct MAP_stc2_ind stc2_ind;
	struct MAP_stc3_req stc3_req;
	struct MAP_stc3_ind stc3_ind;
	struct MAP_stc4_req stc4_req;
	struct MAP_stc4_ind stc4_ind;
};

#endif				/* __SS7_MAP_H__ */
