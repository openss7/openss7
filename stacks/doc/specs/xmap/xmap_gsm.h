/*****************************************************************************

 @(#) $Id: xmap_gsm.h,v 0.9.2.2 2009-03-13 11:20:24 brian Exp $

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

 Last Modified $Date: 2009-03-13 11:20:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm.h,v $
 Revision 0.9.2.2  2009-03-13 11:20:24  brian
 - doc and header updates

 Revision 0.9.2.1  2009/03/12 14:52:35  brian
 - added XMAP documentation

 *****************************************************************************/

#ifndef __XMAP_GSM_H__
#define __XMAP_GSM_H__

#ident "@(#) $RCSfile: xmap_gsm.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2008-2009 Monavacon Limited."

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

/* Service-error problem value macro: */
#define mapP_problem(X) ((OM_sint)(5000 + X))

/* The OM syntax names that are defined are listed below. */
#define MAP_S_ADDRESS_STRING		((OM_syntax) 34)

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

#define OMP_O_MAP_C_DATA_MISSING_PARAM			mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_EXTENSIBLE_SYSTEM_FAILURE_PARAM	mapP_gsm(\x8F\x52)	/* 2002 */
#define OMP_O_MAP_C_EXTENSION_CONTAINTER		mapP_gsm(\x8F\x53)	/* 2003 */
#define OMP_O_MAP_C_FACILITY_NOT_SUP_PARAM		mapP_gsm(\x8F\x54)	/* 2004 */
#define OMP_O_MAP_C_GSM_ABORT_ARGUMENT			mapP_gsm(\x8F\x55)	/* 2005 */
#define OMP_O_MAP_C_GSM_ABORT_RESULT			mapP_gsm(\x8F\x56)	/* 2006 */
#define OMP_O_MAP_C_GSM_ACCEPT_RESULT			mapP_gsm(\x8F\x57)	/* 2007 */
#define OMP_O_MAP_C_GSM_CLOSE_ARGUMENT			mapP_gsm(\x8F\x58)	/* 2008 */
#define OMP_O_MAP_C_GSM_CONTEXT				mapP_gsm(\x8F\x59)	/* 2009 */
#define OMP_O_MAP_C_GSM_MAP_ASSOC_ARGS			mapP_gsm(\x8F\x5A)	/* 2010 */
#define OMP_O_MAP_C_GSM_NOTICE_RESULT			mapP_gsm(\x8F\x5B)	/* 2011 */
#define OMP_O_MAP_C_GSM_OPEN_ARGUMENT			mapP_gsm(\x8F\x5C)	/* 2012 */
#define OMP_O_MAP_C_GSM_P_ABORT_RESULT			mapP_gsm(\x8F\x5D)	/* 2013 */
#define OMP_O_MAP_C_GSM_REFUSE_RESULT			mapP_gsm(\x8F\x5E)	/* 2014 */
#define OMP_O_MAP_C_GSM_RELEASE_ARGUMENT		mapP_gsm(\x8F\x5F)	/* 2015 */
#define OMP_O_MAP_C_GSM_RELEASE_RESULT			mapP_gsm(\x8F\x60)	/* 2016 */
#define OMP_O_MAP_C_GSM_SERVICE_ARGUMENT		mapP_gsm(\x8F\x61)	/* 2017 */
#define OMP_O_MAP_C_GSM_SERVICE_ERROR			mapP_gsm(\x8F\x62)	/* 2018 */
#define OMP_O_MAP_C_GSM_SERVICE_REJECT			mapP_gsm(\x8F\x63)	/* 2019 */
#define OMP_O_MAP_C_GSM_SERVICE_RESULT			mapP_gsm(\x8F\x64)	/* 2020 */
#define OMP_O_MAP_C_INCOMPATIBLE_TERMINAL_PARAM		mapP_gsm(\x8F\x65)	/* 2021 */
#define OMP_O_MAP_C_PCS_EXTENSIONS			mapP_gsm(\x8F\x66)	/* 2022 */
#define OMP_O_MAP_C_PRIVATE_EXTENSION_LIST		mapP_gsm(\x8F\x67)	/* 2023 */
#define OMP_O_MAP_C_PRIVATE_EXTENSION			mapP_gsm(\x8F\x68)	/* 2024 */
#define OMP_O_MAP_C_RESOURCE_LIMITATION_PARAM		mapP_gsm(\x8F\x69)	/* 2025 */
#define OMP_O_MAP_C_SPECIFIC_INFORMATION		mapP_gsm(\x8F\x6A)	/* 2026 */
#define OMP_O_MAP_C_SYSTEM_FAILURE_PARAM		mapP_gsm(\x8F\x6B)	/* 2027 */
#define OMP_O_MAP_C_UNEXPECTED_DATA_PARAM		mapP_gsm(\x8F\x6C)	/* 2028 */

/* The OM attribute names that are defined are listed below. */

#define MAP_DESTINATION_REFERENCE			((OM_type)12001)
#define MAP_EXTENSION_CONTAINER				((OM_type)12002)
#define MAP_EXTENSIONS					((OM_type)12003)
#define MAP_EXT_ID					((OM_type)12004)
#define MAP_EXT_TYPE					((OM_type)12005)
#define MAP_ORIGINATING_REFERENCE			((OM_type)12006)
#define MAP_PCS_EXTENSION				((OM_type)12007)
#define MAP_PCS_EXTENSIONS				((OM_type)12008)
#define MAP_PRIVATE_EXTENSION_LIST			((OM_type)12009)
#define MAP_PRIVATE_EXTENSION				((OM_type)12010)
#define MAP_PROBLEM_DIAGNOSTIC				((OM_type)12011)
#define MAP_REFUSE_REASON				((OM_type)12012)
#define MAP_RELEASE_METHOD				((OM_type)12013)
#define MAP_SPECIFIC_INFORMATION			((OM_type)12014)

/* MAP_Release_Method: */
#define MAP_T_NORMAL_RELEASE				((OM_enumeration) 0)
#define MAP_T_PREARRANGED_END				((OM_enumeration) 1)

/* MAP_Problem_Diagnostic: */
#define MAP_T_ABNORMAL_EVENT_DETECTED_BY_PEER		((OM_enumeration) 0)
#define MAP_T_RESPONSE_REJECTED_BY_THE_PEER		((OM_enumeration) 1)
#define MAP_T_ABNORMAL_EVENT_RECEIVED_FROM_THE_PEER	((OM_enumeration) 2)
#define MAP_T_MESSAGE_CANNOT_BE_DELIVERED_TO_THE_PEER	((OM_enumeration) 3)

/* MAP_Provider_Reason: */
#define MAP_E_PROVIDER_MALFUNCTION			((OM_sint)22101)
#define MAP_E_SUPPORTING_DIALOGUE_TRANSACTION_RELEASED	((OM_sint)22102)
#define MAP_E_RESOURCE_LIMITATION			((OM_sint)22103)
#define MAP_E_MAINTENANCE_ACTIVITY			((OM_sint)22104)
#define MAP_E_VERSION_INCOMPATIBILITY			((OM_sint)22105)
#define MAP_E_ABNORMAL_MAP_DIALOGUE			((OM_sint)22106)

/* MAP_T_Provider_Error: */
#define MAP_E_DUPLICATED_INVOKE_ID			((OM_sint)22200)
#define MAP_E_NOT_SUPPORTED_SERVICE			((OM_sint)22201)
#define MAP_E_MISTYPED_PARAMETER			((OM_sint)22202)
#define MAP_E_RESOURCE_LIMITATION			((OM_sint)22203)
#define MAP_E_INITIATING_RELEASE			((OM_sint)22204)
#define MAP_E_UNEXPECTED_RESPONSE_FROM_THE_PEER		((OM_sint)22205)
#define MAP_E_SERVICE_COMPLETION_FAILURE		((OM_sint)22206)
#define MAP_E_NO_RESPONSE_FROM_THE_PEER			((OM_sint)22207)
#define MAP_E_INVALID_RESPONSE_RECEIVED			((OM_sint)22208)

/* MAP_Source: */
#define MAP_T_MAP_PROBLEM				((OM_enumeration)-0)
#define MAP_T_TC_PROBLEM				((OM_enumeration) 1)
#define MAP_T_NETWORK_SERVICE_PROBLEM			((OM_enumeration) 2)

/* MAP_T_Provider_Error: */
#define MAP_E_PROCEDURE_ERROR				((OM_sint)23300)
#define MAP_E_RESOURCE_LIMITATION_CONGESTION		((OM_sint)23301)
#define MAP_E_RESOURCE_UNAVAILABLE			((OM_sint)23302)
#define MAP_E_APPLICATION_PROCEDURE_CANCELLATION	((OM_sint)23303)

/* MAP_T_Diagnostic_Information: for MAP_E_RESOURCE_UNAVAILABLE */
#define MAP_T_SHORT_TERM_RESOURCE_LIMITATION		((OM_enumeration) 0)
#define MAP_T_LONG_TERM_RESOURCE_LIMITATION		((OM_enumeration) 1)

/* MAP_T_Diagnostic_Information: for MAP_E_APPLICATION_PROCEDURE_CANCELLATION */
#define MAP_T_HANDOVER_CANCELLATION			((OM_enumeration) 0)
#define MAP_T_RADIO_CHANNEL_RELEASE			((OM_enumeration) 1)
#define MAP_T_NETWORK_PATH_RELEASE			((OM_enumeration) 2)
#define MAP_T_CALL_RELEASE				((OM_enumeration) 3)
#define MAP_T_ASSOCIATED_PROCEDURE_FAILURE		((OM_enumeration) 4)
#define MAP_T_TANDEM_DIALOGUE_RELEASE			((OM_enumeration) 5)
#define MAP_T_REMOTE_OPERATIONS_FAILURE			((OM_enumeration) 6)

/* MAP_P_Abort_Reason: */
#define MAP_T_ABNORMAL_DIALOGUE				((OM_enumeration) 0)
#define MAP_T_INVALID_PDU				((OM_enumeration) 1)

/* MAP_P_Abort_Cause: */
#define MAP_T_UNRECOGNIZED_MESSAGE_TYPE			((OM_enumeration) 0)
#define MAP_T_UNRECOGNIZED_TRANSACTION_ID		((OM_enumeration) 1)
#define MAP_T_BADLY_FORMATTED_TRANSACTION_PORTION	((OM_enumeration) 2)
#define MAP_T_INCORRECT_TRANSACTION_PORTION		((OM_enumeration) 3)
#define MAP_T_RESOURCE_LIMITATION			((OM_enumeration) 4)
#define MAP_T_ABNORMAL_DIALOGUE				((OM_enumeration) 5)
#define MAP_T_NO_COMMON_DIALOGUE_PORTION		((OM_enumeration) 6)

#define MAP_T_UNRECOGNIZED_PACKAGE_TYPE			((OM_enumeration) 1281)
#define MAP_T_INCORRECT_TRANS_PORTION			((OM_enumeration) 1282)
#define MAP_T_BADLY_STRUCTURED_TRANS_PORTION		((OM_enumeration) 1283)
#define MAP_T_UNASSIGNED_RESPONDING_TRANS_ID		((OM_enumeration) 1284)
#define MAP_T_PERMISSION_TO_RELEASE_PROBLEM		((OM_enumeration) 1285)
#define MAP_T_RESOURCE_UNAVAILABLE			((OM_enumeration) 1286)

#define MAP_T_UNRECOGNIZED_PACKAGE_TYPE			((OM_enumeration)  1)
#define MAP_T_INCORRECT_TRANSACTION_PORTION		((OM_enumeration)  2)
#define MAP_T_BADLY_STRUCTURED_TRANSACTION_PORTION	((OM_enumeration)  3)
#define MAP_T_UNASSIGNED_RESPONDING_TRANSACTION_ID	((OM_enumeration)  4)
#define MAP_T_PERMISSION_TO_RELEASE_PROBLEM		((OM_enumeration)  5)
#define MAP_T_RESOURCE_UNAVAILABLE			((OM_enumeration)  6)
#define MAP_T_UNRECOGNIZED_DIALOGUE_PORTION_ID		((OM_enumeration)  7)
#define MAP_T_BADLY_STRUCTURED_DIALOGUE_PORTION		((OM_enumeration)  8)
#define MAP_T_MISSING_DIALOGUE_PORTION			((OM_enumeration)  9)
#define MAP_T_INCONSISTENT_DIALOGUE_PORTION		((OM_enumeration) 10)

/* MAP_Report_Cause: */
#define MAP_T_NO_TRANSLATION_FOR_ADDRESS_OF_SUCH_NATURE	((OM_enumeration)   0)
#define MAP_T_NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS	((OM_enumeration)   1)
#define MAP_T_SUBSYSTEM_CONGESTION			((OM_enumeration)   2)
#define MAP_T_SUBSYSTEM_FAILURE				((OM_enumeration)   3)
#define MAP_T_UNEQUIPPED_USER				((OM_enumeration)   4)
#define MAP_T_MTP_FAILURE				((OM_enumeration)   5)
#define MAP_T_NETWORK_CONGESTION			((OM_enumeration)   6)
#define MAP_T_SCCP_UNQUALIFIED				((OM_enumeration)   7)
#define MAP_T_ERROR_IN_MESSAGE_TRANSPORT		((OM_enumeration)   8)
#define MAP_T_ERROR_IN_LOCAL_PROCESSING			((OM_enumeration)   9)
#define MAP_T_DESTINATION_CANNOT_PERFORM_REASSEMBLY	((OM_enumeration)  10)
#define MAP_T_SCCP_FAILURE				((OM_enumeration)  11)
#define MAP_T_HOP_COUNTER_VIOLATION			((OM_enumeration)  12)
#define MAP_T_SEGMENTATION_NOT_SUPPORTED		((OM_enumeration)  13)
#define MAP_T_SEGMENTATION_FAILURE			((OM_enumeration)  14)

#define MAP_T_MESSAGE_CHANGE_FAILURE			((OM_enumeration) 247)
#define MAP_T_INVALID_INS_ROUTING_REQUEST		((OM_enumeration) 248)
#define MAP_T_INVALID_ISNI_ROUTING_REQUEST		((OM_enumeration) 249)
#define MAP_T_UNAUTHORIZED_MESSAGE			((OM_enumeration) 250)
#define MAP_T_MESSAGE_INCOMPATIBILITY			((OM_enumeration) 251)
#define MAP_T_CANNOT_PERFORM_ISNI_CONSTRAINED_ROUTING	((OM_enumeration) 252)
#define MAP_T_REDUNDANT_ISNI_CONSTRAINED_ROUTING_INFORMATION ((OM_enumeration) 253)
#define MAP_T_UNABLE_TO_PERFORM_ISNI_IDENTIFICATION	((OM_enumeration) 254)

/* MAP_Refuse_Reason: */
#define MAP_T_NO_REASON_GIVEN				((OM_enumeration) 0)
#define MAP_T_INVALID_DESTINATION_REFERENCE		((OM_enumeration) 1)
#define MAP_T_INVALID_ORIGINATING_REFERENCE		((OM_enumeration) 2)
#define MAP_T_ENCAPSULATED_AC_NOT_SUPPORTED		((OM_enumeration) 3)
#define MAP_T_TRANSPORT_PROTECTION_NOT_ADEQUATE		((OM_enumeration) 4)
#define MAP_T_REMOTE_NODE_NOT_REACHABLE			((OM_enumeration) -1)
#define MAP_T_APPLICATION_CONTEXT_NOT_SUPPORTED		((OM_enumeration) -2)
#define MAP_T_POTENTIAL_VERSION_INCOMPATIBILITY		((OM_enumeration) -3)
#define MAP_T_SECURED_TRANSPORT_NOT_POSSIBLE		((OM_enumeration) -4)

/* MAP Generic Service Errors: */
#define MAP_E_SYSTEM_FAILURE				((OM_sint) 22000 + 34)
#define MAP_E_DATA_MISSING				((OM_sint) 22000 + 35)
#define MAP_E_UNEXPECTED_DATA_VALUE			((OM_sint) 22000 + 36)
#define MAP_E_FACILITY_NOT_SUPPORTED			((OM_sint) 22000 + 21)
#define MAP_E_INCOMPATIBLE_TERMINAL			((OM_sint) 22000 + 28)
#define MAP_E_RESOURCE_LIMITATION			((OM_sint) 22000 + 51)

/* MAP_Network_Resource: */
#define MAP_T_PLMN					((OM_enumeration) 0)
#define MAP_T_HLR					((OM_enumeration) 1)
#define MAP_T_VLR					((OM_enumeration) 2)
#define MAP_T_PVLR					((OM_enumeration) 3)
#define MAP_T_CONTROLLING_MSC				((OM_enumeration) 4)
#define MAP_T_VMSC					((OM_enumeration) 5)
#define MAP_T_EIR					((OM_enumeration) 6)
#define MAP_T_RSS					((OM_enumeration) 7)

/* MAP_Additional_Network_Resource: */
#define MAP_T_SGSN					((OM_enumeration) 0)
#define MAP_T_GGSN					((OM_enumeration) 1)
#define MAP_T_GMLC					((OM_enumeration) 2)
#define MAP_T_GSM_SCF					((OM_enumeration) 3)
#define MAP_T_NPLR					((OM_enumeration) 4)
#define MAP_T_AUC					((OM_enumeration) 5)

#define MAP_VL_DESTINATION_REFERENCE	20
#define MAP_VL_ORIGINATING_REFERENCE	20

#define MAP_VN_PRIVATE_EXTENSION	10

/*
 * The following enumerations tags and enumeration constants are defined for use
 * as values of the corresponding OM attributes:
 */

/* MAP_T_User_Error: */
#define MAP_E_INITIATING_RELEASE			((OM_sint)(22000 +  0))
#define MAP_E_UNKNOWN_SUBSCRIBER			((OM_sint)(22000 +  1))
#define MAP_E_UNKNOWN_MSC				((OM_sint)(22000 +  3))
#define MAP_E_SECURE_TRANSPORT_ERROR			((OM_sint)(22000 +  4))
#define MAP_E_UNIDENTIFIED_SUBSCRIBER			((OM_sint)(22000 +  5))
#define MAP_E_ABSENT_SUBSCRIBER_SM			((OM_sint)(22000 +  6))
#define MAP_E_UNKNOWN_EQUIPMENT				((OM_sint)(22000 +  7))
#define MAP_E_ROAMING_NOT_ALLOWED			((OM_sint)(22000 +  8))
#define MAP_E_ILLEGAL_SUBSCRIBER			((OM_sint)(22000 +  9))
#define MAP_E_BEARER_SERVICE_NOT_PROVISIONED		((OM_sint)(22000 + 10))
#define MAP_E_TELESERVICE_NOT_PROVISIONED		((OM_sint)(22000 + 11))
#define MAP_E_ILLEGAL_EQUIPMENT				((OM_sint)(22000 + 12))
#define MAP_E_CALL_BARRED				((OM_sint)(22000 + 13))
#define MAP_E_FORWARDING_VIOLATION			((OM_sint)(22000 + 14))
#define MAP_E_CUG_REJECT				((OM_sint)(22000 + 15))
#define MAP_E_ILLEGAL_SS_OPERATION			((OM_sint)(22000 + 16))
#define MAP_E_SS_ERROR_STATUS				((OM_sint)(22000 + 17))
#define MAP_E_SS_NOT_AVAILABLE				((OM_sint)(22000 + 18))
#define MAP_E_SS_SUBSCRIPTION_VIOLATION			((OM_sint)(22000 + 19))
#define MAP_E_SS_INCOMPATIBILITY			((OM_sint)(22000 + 20))
#define MAP_E_FACILITY_NOT_SUPPORTED			((OM_sint)(22000 + 21))
#define MAP_E_NO_HANDOVER_NUMBER_AVAILABLE		((OM_sint)(22000 + 25))
#define MAP_E_SUBSEQUENT_HANDOVER_FAILURE		((OM_sint)(22000 + 26))
#define MAP_E_ABSENT_SUBSCRIBER				((OM_sint)(22000 + 27))
#define MAP_E_INCOMPATIBLE_TERMINAL			((OM_sint)(22000 + 28))
#define MAP_E_SHORT_TERM_DENIAL				((OM_sint)(22000 + 29))
#define MAP_E_LONG_TERM_DENIAL				((OM_sint)(22000 + 30))
#define MAP_E_SUBSCRIBER_BUSY_FOR_MT_SMS		((OM_sint)(22000 + 31))
#define MAP_E_SMS_DELIVERY_FAILURE			((OM_sint)(22000 + 32))
#define MAP_E_MESSAGE_WAITING_LIST_FULL			((OM_sint)(22000 + 33))
#define MAP_E_SYSTEM_FAILURE				((OM_sint)(22000 + 34))
#define MAP_E_DATA_MISSING				((OM_sint)(22000 + 35))
#define MAP_E_UNEXPECTED_DATA_VALUE			((OM_sint)(22000 + 36))
#define MAP_E_PW_REGISTRATION_FAILURE			((OM_sint)(22000 + 37))
#define MAP_E_NEGATIVE_PW_CHECK				((OM_sint)(22000 + 38))
#define MAP_E_NO_ROAMING_NUMBER_AVAILABLE		((OM_sint)(22000 + 39))
#define MAP_E_TRACING_BUFFER_FULL			((OM_sint)(22000 + 40))
#define MAP_E_TARGET_CELL_OUTSIDE_GROUP_CALL_AREA	((OM_sint)(22000 + 42))
#define MAP_E_NUMBER_OF_PW_ATTEMPTS_VIOLATION		((OM_sint)(22000 + 43))
#define MAP_E_NUMBER_CHANGED				((OM_sint)(22000 + 44))
#define MAP_E_BUSY_SUBSCRIBER				((OM_sint)(22000 + 45))
#define MAP_E_NO_SUBSCRIBER_REPLY			((OM_sint)(22000 + 46))
#define MAP_E_FORWARDING_FAILED				((OM_sint)(22000 + 47))
#define MAP_E_OR_NOT_ALLOWED				((OM_sint)(22000 + 48))
#define MAP_E_ATI_NOT_ALLOWED				((OM_sint)(22000 + 49))
#define MAP_E_NO_GROUP_CALL_NUMBER_AVAILABLE		((OM_sint)(22000 + 50))
#define MAP_E_RESOURCE_LIMITATION			((OM_sint)(22000 + 51))
#define MAP_E_UNAUTHORIZED_REQUESTING_NETWORK		((OM_sint)(22000 + 52))
#define MAP_E_UNAUTHORIZED_LCS_CLIENT			((OM_sint)(22000 + 53))
#define MAP_E_POSITION_METHOD_FAILURE			((OM_sint)(22000 + 54))
#define MAP_E_UNKNOWN_OR_UNREACHABLE_LCS_CLIENT		((OM_sint)(22000 + 58))
#define MAP_E_MM_EVENT_NOT_SUPPORTED			((OM_sint)(22000 + 59))
#define MAP_E_ATSI_NOT_ALLOWED				((OM_sint)(22000 + 60))
#define MAP_E_ATM_NOT_ALLOWED				((OM_sint)(22000 + 61))
#define MAP_E_INFORMATION_NOT_AVAILABLE			((OM_sint)(22000 + 62))
#define MAP_E_UNKNOWN_ALPHABET				((OM_sint)(22000 + 71))
#define MAP_E_USSD_BUSY					((OM_sint)(22000 + 72))

#endif				/* __XMAP_GSM_H__ */
