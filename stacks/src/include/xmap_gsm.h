/*****************************************************************************

 @(#) $Id: xmap_gsm.h,v 0.9.2.3 2009-03-13 11:20:25 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-03-13 11:20:25 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm.h,v $
 Revision 0.9.2.3  2009-03-13 11:20:25  brian
 - doc and header updates

 Revision 0.9.2.1  2009-03-12 14:52:35  brian
 - added XMAP documentation

 *****************************************************************************/

#ifndef __XMAP_GSM_H__
#define __XMAP_GSM_H__

#ident "@(#) $RCSfile: xmap_gsm.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2008-2009 Monavacon Limited."

/*
 * The <xmap.h> and <xmap_gsm.h> headers declare the interface functions, the
 * structures passed to and from those functions, and the defined constants used
 * by the functions and structure.
 *
 * All application programs which include this header must first include the
 * OSI-Abstract-DATA Manipulation header <xom.h>.
 *
 * All Object Identifiers are represented by constants defined in the headers.
 * These constants are used with the macros defined in the XOM API (see
 * reference XOM).  A constant is defined to represent the Object Identifier of
 * the Mobile Application Part package:
 *
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) }
 */
#define OMP_O_MAP_GSM_PKG \
	"\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02"	/* 1.3.6.1.4.1.29591.1.1.2 */

#define OMP_O_MAP_GSM_NETWORK \
	"\x04\x00\x00\x01\x00"	/* 0.4.0.0.1 */
	/* { itu-t(0) identified-origanization(4) etsi(0) mobile-domain(0) gsm-Network(1) } */

#define mapP_gsmNetwork(X) (OMP_O_MAP_GSM_NETWORK# #X)

#define OMP_O_MAP_GSM_AC \
	mapP_gsmNetwork(\x00)	/* 0.4.0.0.1.0 */
	/* { itu-t(0) identified-origanization(4) etsi(0) mobile-domain(0) gsm-Network(1) ac(0) } */

#define mapP_acId(X) (OMP_O_MAP_GSM_AC# #X)

#define OMP_O_MAP_GSM_AS \
	mapP_gsmNetwork(\x01)	/* 0.4.0.0.1.1 */

#define mapP_asId(X) (OMP_O_MAP_GSM_AS# #X)

#define OMP_O_MAP_GSM_DIALOGUE_AS \
	mapP_asId(\x01\x01)
	/* { gsm-NetworkId as-Id map-DialoguePDU(1) version1(1) } */

#define OMP_O_MAP_GSM_PROTECTED_DIALOGUE_AS \
	mapP_asID(\x03\x01)
	/* { gsm-NetworkId as-Id map-ProtectedDialoguePDU(3) version1(1) } */

/* Defined constants */

/* Intermediate object identifier macro */
#define mapP_gsm(X) (OMP_O_MAP_GSM_PKG# #X)

/* OM class names (prefixed MAP_C_) */

/*
 * Every application program that makes use of a class or other Object
 * Identifier must explicitly import it into every compilation unit (C source
 * program) which uses it.  Each such calss or Object Identifier neame must be
 * explicitly exported from just one compilation unit.
 *
 * In the header file, OM class constants are prefixed with the OMP_O_ prefix
 * to denote that they are OM classes, However, when using the OM_IMPORT and
 * OM_EXPORT macros, the base names (without the OMP_O_ prefix) should be used.
 * For example:
 *
 * OM_IMPORT(MAP_C_RESULT)
 */

#define OMP_O_MAP_C_EXTENSION_CONTAINTER			mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_ABORT_ARGUMENT				mapP_gsm(\x8F\x52)	/* 2002 */
#define OMP_O_MAP_C_GSM_ABORT_RESULT				mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_ACCEPT_RESULT				mapP_gsm(\x8F\x53)	/* 2003 */
#define OMP_O_MAP_C_GSM_CLOSE_ARGUMENT				mapP_gsm(\x8F\x54)	/* 2004 */
#define OMP_O_MAP_C_GSM_CONTEXT					mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_MAP_ASSOC_ARGS				mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_NOTICE_RESULT				mapP_gsm(\x8F\x55)	/* 2005 */
#define OMP_O_MAP_C_GSM_OPEN_ARGUMENT				mapP_gsm(\x8F\x56)	/* 2006 */
#define OMP_O_MAP_C_GSM_P_ABORT_RESULT				mapP_gsm(\x8F\x57)	/* 2007 */
#define OMP_O_MAP_C_GSM_REFUSE_RESULT				mapP_gsm(\x8F\x58)	/* 2008 */
#define OMP_O_MAP_C_GSM_RELEASE_ARGUMENT			mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_RELEASE_RESULT				mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_SERVICE_ARGUMENT			mapP_gsm(\x8F\x59)	/* 2009 */
#define OMP_O_MAP_C_GSM_SERVICE_RESULT				mapP_gsm(\x8F\x5A)	/* 2010 */
#define OMP_O_MAP_C_PCS_EXTENSIONS				mapP_gsm(\x8F\x5B)	/* 2011 */
#define OMP_O_MAP_C_PRIVATE_EXTENSION				mapP_gsm(\x8F\x5C)	/* 2012 */
#define OMP_O_MAP_C_PRIVATE_EXTENSION_LIST			mapP_gsm(\x8F\x5D)	/* 2013 */
#define OMP_O_MAP_C_SPECIFIC_INFORMATION			mapP_gsm(\x8F\x5E)	/* 2014 */
#define OMP_O_MAP_C_USER_ABORT_CHOICE				mapP_gsm(\x8F\x5F)	/* 2015 */

/* The OM syntax names that are defined are listed below. */
#define MAP_S_ADDRESS_STRING		((OM_syntax) 34)

/* The OM attribute names that are defined are listed below. */

#define MAP_APPLICATION_PROCEDURE_CANCELLATION			((OM_type)12026)
#define MAP_DESTINATION_REFERENCE				((OM_type)12007)
#define MAP_EXTENSION_CONTAINER					((OM_type)12022)
#define MAP_EXTENSIONS						((OM_type)12021)
#define MAP_EXT_ID						((OM_type)12019)
#define MAP_EXT_TYPE						((OM_type)12018)
#define MAP_ORIGINATING_REFERENCE				((OM_type)12008)
#define MAP_P_ABORT_CAUSE					((OM_type)12012)
#define MAP_P_ABORT_REASON					((OM_type)12011)
#define MAP_PCS_EXTENSION					((OM_type)12017)
#define MAP_PCS_EXTENSIONS					((OM_type)12002)
#define MAP_PRIVATE_EXTENSION_LIST				((OM_type)12001)
#define MAP_PRIVATE_EXTENSION					((OM_type)12020)
#define MAP_PROBLEM_DIAGNOSTIC					((OM_type)12006)
#define MAP_PROVIDER_ERROR					((OM_type)12016)
#define MAP_PROVIDER_REASON					((OM_type)12009)
#define MAP_REFUSE_REASON					((OM_type)12014)
#define MAP_RELEASE_METHOD					((OM_type)12005)
#define MAP_REPORT_CAUSE					((OM_type)12013)
#define MAP_RESOURCE_UNAVAILABLE				((OM_type)12025)
#define MAP_SOURCE						((OM_type)12010)
#define MAP_SPECIFIC_INFORMATION				((OM_type)12004)
#define MAP_USER_ABORT_REASON					((OM_type)12003)
#define MAP_USER_ERROR						((OM_type)12015)
#define MAP_USER_RESOURCE_LIMITATION				((OM_type)12024)
#define MAP_USER_SPECIFIC_REASON				((OM_type)12023)

#define MAP_VL_DESTINATION_REFERENCE	20
#define MAP_VL_ORIGINATING_REFERENCE	20

#define MAP_VN_PRIVATE_EXTENSION	10

/*
 * The following enumerations tags and enumeration constants are defined for use
 * as values of the corresponding OM attributes:
 */

/* MAP_T_Release_Method: */
typedef enum {
	MAP_T_NORMAL_RELEASE = 0,
	MAP_T_PREARRANGED_END,
} MAP_T_Release_Method;

/* MAP_T_Problem_Diagnostic: */
typedef enum {
	MAP_T_ABNORMAL_EVENT_DETECTED_BY_PEER = 0,
	MAP_T_RESPONSE_REJECTED_BY_THE_PEER,
	MAP_T_ABNORMAL_EVENT_RECEIVED_FROM_THE_PEER,
	MAP_T_MESSAGE_CANNOT_BE_DELIVERED_TO_THE_PEER,
} MAP_T_Problem_Diagnostic;

/* MAP_T_Provider_Reason: */
typedef enum {
	MAP_T_PROVIDER_MALFUNCTION = 0,
	MAP_T_SUPPORTING_DIALOGUE_TRANSACTION_RELEASED,
	MAP_T_RESOURCE_LIMITATION,
	MAP_T_MAINTENANCE_ACTIVITY,
	MAP_T_VERSION_INCOMPATIBILITY,
	MAP_T_ABNORMAL_MAP_DIALOGUE,
} MAP_T_Provider_Reason;

/* MAP_T_Source: */
typedef enum {
	MAP_T_MAP_PROBLEM = 0,
	MAP_T_TC_PROBLEM,
	MAP_T_NETWORK_SERVICE_PROBLEM,
} MAP_T_Source;

/* MAP_T_P_Abort_Reason: */
typedef enum {
	MAP_T_ABNORMAL_DIALOGUE = 0,
	MAP_T_INVALID_PDU,
} MAP_T_P_Abort_Reason;

/* MAP_T_P_Abort_Cause: */
typedef enum {
	MAP_T_UNRECOGNIZED_MESSAGE_TYPE = 0,
	MAP_T_UNRECOGNIZED_TRANSACTION_ID,
	MAP_T_BADLY_FORMATTED_TRANSACTION_PORTION,
	MAP_T_INCORRECT_TRANSACTION_PORTION,
	MAP_T_RESOURCE_LIMITATION,
	MAP_T_ABNORMAL_DIALOGUE,
	MAP_T_NO_COMMON_DIALOGUE_PORTION,
} MAP_T_P_Abort_Cause;

/* MAP_T_Report_Cause: */
typedef enum {
	MAP_T_NO_TRANSLATIONS_FOR_ADDRESS_OF_SUCH_NATURE = 0,
	MAP_T_NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS,
	MAP_T_SUBSYSTEM_CONGESTION,
	MAP_T_SUBSYSTEM_FAILURE,
	MAP_T_UNEQUIPPED_USER,
	MAP_T_MTP_FAILUIRE,
	MAP_T_NETWORK_CONGESTION,
	MAP_T_SCCP_UNQUALIFIED,
	MAP_T_ERROR_IN_MESSAGE_TRANSPORT,
	MAP_T_ERROR_IN_LOCAL_PROCESSING,
	MAP_T_DESTINATION_CANNOT_PERFORM_REASSEMBLY,
	MAP_T_SCCP_FAILURE,
	MAP_T_HOP_COUNTER_VIOLATION,
	MAP_T_SEGMENTATION_NOT_SUPPORTED,
	MAP_T_SEGMENTATION_FAILURE,
	/* ANSI SCCP only */
	MAP_T_MESSAGE_CHANGE_FAILURE = 247,
	MAP_T_INVALID_INS_ROUTING_REQUEST,
	MAP_T_INVALID_ISNI_ROUTING_REQUEST,
	MAP_T_UNAUTHORIZED_MESSAGE,
	MAP_T_MESSAGE_INCOMPATIBILITY,
	MAP_T_CANNOT_PERFORM_ISNI_CONSTRAINED_ROUTING,
	MAP_T_REDUNDANT_ISNI_CONSTRAINED_ROUTING_INFORMATION,
	MAP_T_UNABLE_TO_PERFORM_ISNI_IDENTIFICATION,
} MAP_T_Report_Cause;

/* MAP_T_Refuse_Reason: */
typedef enum {
	MAP_T_NO_REASON_GIVEN = 0,
	MAP_T_INVALID_DESTINATION_REFERENCE,
	MAP_T_INVALID_ORIGINATING_REFERENCE,
	MAP_T_ENCAPSULATED_AC_NOT_SUPPORTED,
	MAP_T_TRANSPORT_PROTECTION_NOT_ADEQUATE,
	/* not on wire */
	MAP_T_REMOTE_NODE_NOT_REACHABLE,
	MAP_T_APPLICATION_CONTEXT_NOT_SUPPORTED,
	MAP_T_POTENTIAL_VERSION_INCOMPATIBILITY,
	MAP_T_SECURED_TRANSPORT_NOT_POSSIBLE,
} MAP_T_Refuse_Reason;

/* MAP_T_User_Error: */
typedef enum {
#define MAP_T_INITIATING_RELEASE			 0
#define MAP_T_UNKNOWN_SUBSCRIBER			 1
#define MAP_T_UNKNOWN_MSC				 3
#define MAP_T_SECURE_TRANSPORT_ERROR			 4
#define MAP_T_UNIDENTIFIED_SUBSCRIBER			 5
#define MAP_T_ABSENT_SUBSCRIBER_SM			 6
#define MAP_T_UNKNOWN_EQUIPMENT				 7
#define MAP_T_ROAMING_NOT_ALLOWED			 8
#define MAP_T_ILLEGAL_SUBSCRIBER			 9
#define MAP_T_BEARER_SERVICE_NOT_PROVISIONED		10
#define MAP_T_TELESERVICE_NOT_PROVISIONED		11
#define MAP_T_ILLEGAL_EQUIPMENT				12
#define MAP_T_CALL_BARRED				13
#define MAP_T_FORWARDING_VIOLATION			14
#define MAP_T_CUG_REJECT				15
#define MAP_T_ILLEGAL_SS_OPERATION			16
#define MAP_T_SS_ERROR_STATUS				17
#define MAP_T_SS_NOT_AVAILABLE				18
#define MAP_T_SS_SUBSCRIPTION_VIOLATION			19
#define MAP_T_SS_INCOMPATIBILITY			20
#define MAP_T_FACILITY_NOT_SUPPORTED			21
#define MAP_T_NO_HANDOVER_NUMBER_AVAILABLE		25
#define MAP_T_SUBSEQUENT_HANDOVER_FAILURE		26
#define MAP_T_ABSENT_SUBSCRIBER				27
#define MAP_T_INCOMPATIBLE_TERMINAL			28
#define MAP_T_SHORT_TERM_DENIAL				29
#define MAP_T_LONG_TERM_DENIAL				30
#define MAP_T_SUBSCRIBER_BUSY_FOR_MT_SMS		31
#define MAP_T_SMS_DELIVERY_FAILURE			32
#define MAP_T_MESSAGE_WAITING_LIST_FULL			33
#define MAP_T_SYSTEM_FAILURE				34
#define MAP_T_DATA_MISSING				35
#define MAP_T_UNEXPECTED_DATA_VALUE			36
#define MAP_T_PW_REGISTRATION_FAILURE			37
#define MAP_T_NEGATIVE_PW_CHECK				38
#define MAP_T_NO_ROAMING_NUMBER_AVAILABLE		39
#define MAP_T_TRACING_BUFFER_FULL			40
#define MAP_T_TARGET_CELL_OUTSIDE_GROUP_CALL_AREA	42
#define MAP_T_NUMBER_OF_PW_ATTEMPTS_VIOLATION		43
#define MAP_T_NUMBER_CHANGED				44
#define MAP_T_BUSY_SUBSCRIBER				45
#define MAP_T_NO_SUBSCRIBER_REPLY			46
#define MAP_T_FORWARDING_FAILED				47
#define MAP_T_OR_NOT_ALLOWED				48
#define MAP_T_ATI_NOT_ALLOWED				49
#define MAP_T_NO_GROUP_CALL_NUMBER_AVAILABLE		50
#define MAP_T_RESOURCE_LIMITATION			51
#define MAP_T_UNAUTHORIZED_REQUESTING_NETWORK		52
#define MAP_T_UNAUTHORIZED_LCS_CLIENT			53
#define MAP_T_POSITION_METHOD_FAILURE			54
#define MAP_T_UNKNOWN_OR_UNREACHABLE_LCS_CLIENT		58
#define MAP_T_MM_EVENT_NOT_SUPPORTED			59
#define MAP_T_ATSI_NOT_ALLOWED				60
#define MAP_T_ATM_NOT_ALLOWED				61
#define MAP_T_INFORMATION_NOT_AVAILABLE			62
#define MAP_T_UNKNOWN_ALPHABET				71
#define MAP_T_USSD_BUSY					72
} MAP_T_User_Error;

/* MAP_T_Provider_Error: */
typedef enum {
	MAP_T_DUPLICATED_INVOKE_ID = 0,
	MAP_T_NOT_SUPPORTED_SERVICE,
	MAP_T_MISTYPED_PARAMETER,
	MAP_T_RESOURCE_LIMITATION,
	MAP_T_INITIATING_RELEASE,
	MAP_T_UNEXPECTED_RESPONSE_FROM_THE_PEER,
	MAP_T_SERVICE_COMPLETION_FAILURE,
	MAP_T_NO_RESPONSE_FROM_THE_PEER,
	MAP_T_INVALID_RESPONSE_RECEIVED,
} MAP_T_Provider_Error;

/* MAP_T_Resource_Unavailable_Reason: */
typedef enum {
	MAP_T_SHORT_TERM_RESOURCE_LIMITATION = 0,
	MAP_T_LONG_TERM_RESOURCE_LIMITATION,
} MAP_T_Resource_Unavailable_Reason;

/* MAP_T_Procedure_Cancellation_Reason: */
typedef enum {
	MAP_T_HANDOVER_CANCELLATION = 0,
	MAP_T_RADIO_CHANNEL_RELEASE,
	MAP_T_NETWORK_PATH_RELEASE,
	MAP_T_CALL_RELEASE,
	MAP_T_ASSOCIATED_PROCEDURE_FAILURE,
	MAP_T_TANDEM_DIALOGUE_RELEASE,
	MAP_T_REMOTE_OPERATIONS_FAILURE,
} MAP_T_Procedure_Cancellation_Reason;

/* Service-Error problem values: */
#define MAP_E_INITIATING_RELEASE				mapP_problem(xx)

#define MAP_E_SYSTEM_FAILURE					mapP_problem(34)
#define MAP_E_DATA_MISSING					mapP_problem(35)
#define MAP_E_UNEXPECTED_DATA_VALUE				mapP_problem(36)
#define MAP_E_FACILITY_NOT_SUPPORTED				mapP_problem(21)
#define MAP_E_INCOMPATIBLE_TERMINAL				mapP_problem(28)
#define MAP_E_RESOURCE_LIMITATION				mapP_problem(51)

/* Service-Error parameter classes: */
#define OMP_O_MAP_C_SYSTEM_FAILURE_PARAM			mapP_gsm(\x97\x5C)	/* 3034 */
#define OMP_O_MAP_C_DATA_MISSING_PARAM				mapP_gsm(\x97\x5D)	/* 3035 */
#define OMP_O_MAP_C_UNEXPECTED_DATA_PARAM			mapP_gsm(\x97\x5E)	/* 3036 */
#define OMP_O_MAP_C_FACILITY_NOT_SUP_PARAM			mapP_gsm(\x97\x4D)	/* 3021 */
#define OMP_O_MAP_C_INCOMPATIBLE_TERMINAL_PARAM			mapP_gsm(\x97\x54)	/* 3028 */
#define OMP_O_MAP_C_RESOURCE_LIMITATION_PARAM			mapP_gsm(\x97\x6B)	/* 3051 */

#define OMP_O_MAP_C_EXTENSIBLE_SYSTEM_FAILURE_PARAM		mapP_gsm()
#define OMP_O_MAP_C_BEARER_SERVICE_CODE				mapP_gsm()

/* Service-Error attributes: */
#define MAP_NETWORK_RESOURCE					((OM_type)5000)
#define MAP_EXTENSIBLE_SYSTEM_FAILURE_PARAM			((OM_type)5001)
#define MAP_ADDITIONAL_NETWORK_RESOURCE				((OM_type)5002)
#define MAP_SHAPE_OF_LOCATION_ESTIMATED_NOT_SUPPORTED		((OM_type)5003)
#define MAP_NEEDED_LCS_CAPABILITY_NOT_SUPPORTED_IN_SERVING_NODE	((OM_type)5004)
#define MAP_UNKNOWN_SUBSCRIBER_DIAGNOSTIC			((OM_type)5005) /* XXX IN */
#define MAP_ROAMING_NOT_ALLOWED_CAUSE				((OM_type)5006) /* XXX SB */
#define MAP_ABSENT_SUBSCRIBER_REASON				((OM_type)5007) /* XXX CH */
#define MAP_CCBS_POSSIBLE					((OM_type)5008) /* XXX CH */
#define MAP_CCBS_BUSY						((OM_type)5009) /* XXX CH */
#define MAP_CALL_BARRING_CAUSE					((OM_type)5010) /* XXX CH */
#define MAP_EXTENSIBLE_CALL_BARRED_PARAM			((OM_type)5011)
#define MAP_UNAUTHORIZED_MESSAGE_ORIGINATOR			((OM_type)5012) /* XXX CH */
#define MAP_CUG_REJECT_CAUSE					((OM_type)5013) /* XXX CH */
#define MAP_BEARER_SERVICE					((OM_type)5014) /* XXX SS */
#define MAP_TELESERVICE						((OM_type)5015) /* XXX SS */
#define MAP_SS_CODE						((OM_type)5016) /* XXX SS */
#define MAP_BASIC_SERVICE					((OM_type)5017) /* XXX SS */
#define MAP_SS_STATUS						((OM_type)5018) /* XXX SS */
#define MAP_GPRS_CONNECTION_SUSPENDED				((OM_type)5019) /* XXX SM */
#define MAP_SS_ENUMERATED_DELIVERY_FAILURE_CAUSE		((OM_type)5020)
#define MAP_DIAGNOSTIC_INFO					((OM_type)5021)
#define MAP_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM			((OM_type)5022) /* XXX SM */
#define MAP_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM		((OM_type)5023) /* XXX SM */
#define MAP_UNAUTHORIZED_LCS_CLIENT_DIAGNOSTIC			((OM_type)5024) /* XXX LS */
#define MAP_POSITION_METHOD_FAILURE_DIAGNOSTIC			((OM_type)5025) /* XXX LS */
#define MAP_SECURITY_HEADER					((OM_type)5026)
#define MAP_PROTECTED_PAYLOAD					((OM_type)5027)

/* MAP_T_Network_Resource: */
typedef enum {
	MAP_T_PLMN = 0,
	MAP_T_HLR,
	MAP_T_VLR,
	MAP_T_PVLR,
	MAP_T_CONTROLLING_MSC,
	MAP_T_VMSC,
	MAP_T_EIR,
	MAP_T_RSS,
} MAP_T_Network_Resource;

/* MAP_T_Additional_Network_Resource: */
typedef enum {
	MAP_T_SGSN = 0,
	MAP_T_GGSN,
	MAP_T_GMLC,
	MAP_T_GSM_SCF,
	MAP_T_NPLR,
	MAP_T_AUC,
} MAP_T_Additional_Network_Resource;

/* MAP_T_Unknown_Subscriber_Diagnostic: */
typedef enum {
	MAP_T_IMSI_UNKNOWN = 0,
	MAP_T_GPRS_SUBSCRIPTION_UNKNOWN,
	MAP_T_NPDB_MISMATCH,
} MAP_T_Unknown_Subscriber_Diagnostic;

/* MAP_T_Roaming_Not_Allowed_Cause: */
typedef enum {
	MAP_T_PLMN_ROAMING_NOT_ALLOWED = 0,
	MAP_T_OPERATOR_DETERMINED_BARRING,
} MAP_T_Roaming_Not_Allowed_Cause;

/* MAP_T_Absent_Subscriber_Reason: */
typedef enum {
	MAP_T_IMSI_DETACH = 0,
	MAP_T_RESTRICTED_AREA,
	MAP_T_NO_PAGE_RESPONSE,
	MAP_T_PURGED_MS,
} MAP_T_Absent_Subscriber_Reason;

/* MAP_T_Call_Barring_Cause: */
typedef enum {
	MAP_T_BARRING_SERVICE_ACTIVE = 0,
	MAP_T_OPERATOR_BARRING,
} MAP_T_Call_Barring_Cause;

/* MAP_T_CUG_Reject_Cause: */
typedef enum {
	MAP_T_INCOMING_CALLS_BARRED_WITHIN_CUG = 0,
	MAP_T_SUBSCRIBER_NOT_MEMBER_OF_CUG,
	MAP_T_REQUESTED_BASIC_SERVICE_VIOLATES_CUG_CONSTRAINTS = 5,
	MAP_T_CALLED_PARTY_SS_INTERACTION_VIOLATION = 7,
} MAP_T_CUG_Reject_Cause;

/* MAP_T_SM_Enumerated_Delivery_Failure_Cause: */
typedef enum {
	MAP_T_MEMORY_CAPACITY_EXCEEDED = 0,
	MAP_T_EQUIPMENT_PROTOCOL_ERROR,
	MAP_T_EQUIPMENT_NOT_SM_EQUIPPED,
	MAP_T_UNKNOWN_SERVICE_CENTER,
	MAP_T_SC_CONGESTION,
	MAP_T_INVALID_SME_ADDRESS,
	MAP_T_SUBSCRIBER_NOT_SC_SUBSCRIBER,
} MAP_T_SM_Enumerated_Delivery_Failure_Cause;

/* MAP_T_Unauthorized_LCS_Client_Diagnostic: */
typedef enum {
	MAP_T_NO_ADDITIONAL_INFORMATION = 0,
	MAP_T_CLIENT_NOT_IN_MS_PRIVACY_EXCEPTION_LIST,
	MAP_T_CALL_TO_CLIENT_NOT_SETUP,
	MAP_T_PRIVACY_OVERRIDE_NOT_APPLICABLE,
	MAP_T_DISALLOWED_BY_LOCAL_REGULATORY_REQUIREMENTS,
	MAP_T_UNAUTHORIZED_PRIVACY_CLASS,
	MAP_T_UNAUTHORIZED_CALL_SESSION_UNRELATED_EXTERNAL_CLIENT,
	MAP_T_UNAUTHORIZED_CALL_SESSION_RELATED_EXTERNAL_CLIENT,
} MAP_T_Unauthorized_LCS_Client_Diagnostic;

/* MAP_T_Position_Method_Failure_Diagnostic: */
typedef enum {
	MAP_T_CONGESTION = 0,
	MAP_T_INSUFFICIENT_RESOURCES,
	MAP_T_INSUFFICIENT_MEASUREMENT_DATA,
	MAP_T_INCONSISTENT_MEASUREMENT_DATA,
	MAP_T_LOCATION_PROCEDURE_NOT_COMPLETED,
	MAP_T_LOCATION_PROCEDURE_NOT_SUPPORTED_BY_TARGET_MS,
	MAP_T_QOS_NOT_ATTAINABLE,
	MAP_T_POSITION_METHOD_NOT_AVAILABLE_IN_NETWORK,
	MAP_T_POSITION_METHOD_NOT_AVAILABLE_IN_LOCATION_AREA,
} MAP_T_Position_Method_Failure_Diagnostic;

/* Common Data Types: */

#define OMP_O_MAP_C_ACCESS_NETWORK_SIGNAL_INFO			mapP_gsm()
#define OMP_O_MAP_C_BASIC_SERVICE_CODE				mapP_gsm()
#define OMP_O_MAP_C_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_OR_LAI	mapP_gsm()
#define OMP_O_MAP_C_EMLPP_INFO					mapP_gsm()
#define OMP_O_MAP_C_EXT_BASIC_SERVICE_CODE			mapP_gsm()
#define OMP_O_MAP_C_EXTERNAL_SIGNAL_INFO			mapP_gsm()
#define OMP_O_MAP_C_EXT_EXTERNAL_SIGNAL_INFO			mapP_gsm()
#define OMP_O_MAP_C_HLR_LIST					mapP_gsm()
#define OMP_O_MAP_C_IDENTITY					mapP_gsm()
#define OMP_O_MAP_C_IMSI_WITH_LMSI				mapP_gsm()
#define OMP_O_MAP_C_LCS_CLIENT_EXTERNAL_ID			mapP_gsm()
#define OMP_O_MAP_C_MC_SS_INFO					mapP_gsm()
#define OMP_O_MAP_C_NAEA_PREFERRED_CI				mapP_gsm()
#define OMP_O_MAP_C_SUBSCRIBER_IDENTITY				mapP_gsm()
#define OMP_O_MAP_C_SUBSCRIBER_ID				mapP_gsm()

#define MAP_ACCESS_NETWORK_PROTOCOL_ID				((OM_type)6004)
#define MAP_AGE_OF_LOCATION_INFORMATION				((OM_type)6022)
#define MAP_ALERTING_PATTERN					((OM_type)6005)
#define MAP_ASCI_CALL_REFERENCE					((OM_type)6007)
#define MAP_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_FIXED_LENGTH	((OM_type)6016)
#define MAP_EMLPP_PRIORITY					((OM_type)6018)
#define MAP_EXT_PROTOCOL_ID					((OM_type)6003)
#define MAP_EXT_SS_STATUS					((OM_type)6021)
#define MAP_GLOBAL_CELL_ID					((OM_type)6012)
#define MAP_HLR_ID						((OM_type)6010)
#define MAP_IMEI						((OM_type)6009)
#define MAP_IMSI						((OM_type)6006) /* XXX */
#define MAP_LAI_FIXED_LENGTH					((OM_type)6017)
#define MAP_LCS_CLIENT_INTERNAL_ID				((OM_type)6014)
#define MAP_LCS_SERVICE_TYPE_ID					((OM_type)6015)
#define MAP_LMSI						((OM_type)6011) /* XXX */
#define MAP_MAX_MC_BEARERS					((OM_type)6019)
#define MAP_MC_BEARERS						((OM_type)6020)
#define MAP_NAEA_CIC						((OM_type)6013)
#define MAP_PROTOCOL_ID						((OM_type)6001)
#define MAP_SIGNAL_INFO						((OM_type)6002)
#define MAP_TMSI						((OM_type)6008)

/* MAP_T_Protocol_Id: */
typedef enum {
	MAP_T_GSM_0408 = 1,
	MAP_T_GSM_0806,
	MAP_T_BSSMAP,		/* reserved */
	MAP_T_ETS_300102_1,
} MAP_T_Protocol_Id;

/* MAP_T_Ext_Protocol_Id: */
typedef enum {
	MAP_T_ETS_300356 = 1,
} MAP_T_Ext_Protocol_Id;

/* MAP_T_Access_Network_Protocol_Id: */
typedef enum {
	MAP_T_TS3G_48006 = 1,
	MAP_T_TS3G_25413,
} MAP_T_Access_Network_Protocol_Id;

/* MAP_T_LCS_Client_Internal_Id: */
typedef enum {
	MAP_T_BROADCAST_SERVICE = 0,
	MAP_T_O_AND_M_HPLMN,
	MAP_T_O_AND_M_VPLMN,
	MAP_T_ANONYMOUS_LOCATION,
	MAP_T_TARGET_MS_SUBSCRIBED_SERVICE,
} MAP_T_LCS_Client_Internal_Id;

/* MAP_T_LCS_Service_Type_Id: */
typedef enum {
	/* The values of LCSServiceTypeID are defined according to 3GPP TS 22.071. */
	MAP_T_EMERGENCY_SERVICES = 0,
	MAP_T_EMERGENCY_ALERT_SERVICES,
	MAP_T_PERSON_TRACKING,
	MAP_T_FLEET_MANAGEMENT,
	MAP_T_ASSET_MANAGEMENT,
	MAP_T_TRAFFIC_CONGESTION_REPORTING,
	MAP_T_ROADSIDE_ASSISTANCE,
	MAP_T_ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE,
	MAP_T_NAVIGATION,
	MAP_T_CITY_SIGHSEEING,
	MAP_T_LOCALIZED_ADVERTISING,
	MAP_T_MOBILE_YELLOW_PAGES,
	MAP_T_TRAFFIC_AND_PUBLIC_TRANSPORTATION_INFO,
	MAP_T_WEATHER,
	MAP_T_ASSET_AND_SERVICE_FINDING,
	MAP_T_GAMING,
	MAP_T_FIND_YOUR_FRIEND,
	MAP_T_DATING,
	MAP_T_CHATTING,
	MAP_T_ROUTE_FINDING,
	MAP_T_WHERE_AM_I,
} MAP_T_LCS_Service_Type_Id;

#endif				/* __XMAP_GSM_H__ */
