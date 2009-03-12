/*****************************************************************************

 @(#) $Id: xmap_gsm.h,v 0.9.2.1 2009-03-12 14:52:35 brian Exp $

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

 Last Modified $Date: 2009-03-12 14:52:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm.h,v $
 Revision 0.9.2.1  2009-03-12 14:52:35  brian
 - added XMAP documentation

 *****************************************************************************/

#ifndef __XMAP_GSM_H__
#define __XMAP_GSM_H__

#ident "@(#) $RCSfile: xmap_gsm.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

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

#define OMP_O_MAP_C_EXTENSION_CONTAINTER		mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_ABORT_ARGUMENT			mapP_gsm(\x8F\x52)	/* 2002 */
/* (M) (M=) MAP_USER_REASON
 * (U) (C=) MAP_DIAGNOSTIC_INFORMATION */
#define OMP_O_MAP_C_GSM_ABORT_RESULT			mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_ACCEPT_RESULT			mapP_gsm(\x8F\x53)	/* 2003 */
/* (U) (C=) MAP_APPLICATION_CONTEXT_NAME
 * (U) (C=) MAP_RESPONDING_ADDRESS
 * (U) (C=) MAP_SPECIFIC_INFORMATION */
#define OMP_O_MAP_C_GSM_CLOSE_ARGUMENT			mapP_gsm(\x8F\x54)	/* 2004 */
/* (M) (- ) MAP_RELEASE_METHOD
 * (U) (C=) MAP_SPECIFIC_INFORMATION */
#define OMP_O_MAP_C_GSM_CONTEXT				mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_MAP_ASSOC_ARGS			mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_NOTICE_RESULT			mapP_gsm(\x8F\x55)	/* 2005 */
/* (M) MAP_PROBLEM_DIAGNOSTIC */
#define OMP_O_MAP_C_GSM_OPEN_ARGUMENT			mapP_gsm(\x8F\x56)	/* 2006 */
/* (M) (M=) MAP_APPLICATION_CONTEXT_NAME
 * (M) (M=) MAP_DESTINATION_ADDRESS
 * (U) (C=) MAP_DESTINATION_REFERENCE
 * (U) (O ) MAP_ORIGINATING_ADDRESS
 * (U) (C=) MAP_ORIGINATING_REFERENCE
 * (U) (C=) MAP_SPECIFIC_INFORMATION */
#define OMP_O_MAP_C_GSM_P_ABORT_RESULT			mapP_gsm(\x8F\x57)	/* 2007 */
/* (M) MAP_PROVIDER_REASON
 * (M) MAP_SOURCE */
#define OMP_O_MAP_C_GSM_REFUSE_RESULT			mapP_gsm(\x8F\x58)	/* 2008 */
/* (M) (C=) MAP_REFUSE_REASON
 * (U) (C=) MAP_APPLICATION_CONTEXT_NAME
 * (U) (C=) MAP_RESPONDING_ADDRESS
 * (U) (C=) MAP_SPECIFIC_INFORMATION
 * (-) (O ) MAP_PROVIDER_ERROR */
#define OMP_O_MAP_C_GSM_RELEASE_ARGUMENT		mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_RELEASE_RESULT			mapP_gsm(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_GSM_SERVICE_ARGUMENT		mapP_gsm(\x8F\x59)	/* 2009 */
#define OMP_O_MAP_C_GSM_SERVICE_RESULT			mapP_gsm(\x8F\x5A)	/* 2010 */
#define OMP_O_MAP_C_PCS_EXTENSIONS			mapP_gsm(\x8F\x5B)	/* 2011 */
#define OMP_O_MAP_C_PRIVATE_EXTENSION			mapP_gsm(\x8F\x5C)	/* 2012 */
#define OMP_O_MAP_C_PRIVATE_EXTENSION_LIST		mapP_gsm(\x8F\x5D)	/* 2013 */
#define OMP_O_MAP_C_SPECIFIC_INFORMATION		mapP_gsm(\x8F\x5E)	/* 2014 */
#define OMP_O_MAP_C_USER_ABORT_CHOICE			mapP_gsm(\x8F\x5F)	/* 2015 */

/* The OM syntax names that are defined are listed below. */
#define MAP_S_ADDRESS_STRING		((OM_syntax) 34)

/* The OM attribute names that are defined are listed below. */

#define MAP_PRIVATE_EXTENSION_LIST			((OM_type)12001)
#define MAP_PCS_EXTENSIONS				((OM_type)12002)
#define MAP_USER_ABORT_REASON				((OM_type)12003)
#define MAP_SPECIFIC_INFORMATION			((OM_type)12004)
#define MAP_RELEASE_METHOD				((OM_type)12005)
#define MAP_PROBLEM_DIAGNOSTIC				((OM_type)12006)
#define MAP_DESTINATION_REFERENCE			((OM_type)12007)
#define MAP_ORIGINATING_REFERENCE			((OM_type)12008)
#define MAP_PROVIDER_REASON				((OM_type)12009)
#define MAP_SOURCE					((OM_type)12010)
#define MAP_P_ABORT_REASON				((OM_type)12011)
#define MAP_P_ABORT_CAUSE				((OM_type)12012)
#define MAP_REPORT_CAUSE				((OM_type)12013)
#define MAP_REFUSE_REASON				((OM_type)12014)
#define MAP_USER_ERROR					((OM_type)12015)
#define MAP_PROVIDER_ERROR				((OM_type)12016)
#define MAP_PCS_EXTENSION				((OM_type)12017)
#define MAP_EXT_TYPE					((OM_type)12018)
#define MAP_EXT_ID					((OM_type)12019)
#define MAP_PRIVATE_EXTENSION				((OM_type)12020)
#define MAP_EXTENSIONS					((OM_type)12021)
#define MAP_EXTENSION_CONTAINER				((OM_type)12022)
#define MAP_USER_SPECIFIC_REASON			((OM_type)12023)
#define MAP_USER_RESOURCE_LIMITATION			((OM_type)12024)
#define MAP_RESOURCE_UNAVAILABLE			((OM_type)12025)
#define MAP_APPLICATION_PROCEDURE_CANCELLATION		((OM_type)12026)

#define MAP_VL_DESTINATION_REFERENCE	20
#define MAP_VL_ORIGINATING_REFERENCE	20

#define MAP_VN_PRIVATE_EXTENSION	10

/*
 * The following enumerations tags and enumeration constants are defined for use
 * as values of the corresponding OM attributes:
 */

/* MAP_T_Release_Method: */
#define MAP_T_NORMAL_RELEASE	0
#define MAP_T_PREARRANGED_END	1

/* MAP_T_Problem_Diagnostic: */
#define MAP_T_ABNORMAL_EVENT_DETECTED_BY_PEER			0
#define MAP_T_RESPONSE_REJECTED_BY_THE_PEER			1
#define MAP_T_ABNORMAL_EVENT_RECEIVED_FROM_THE_PEER		2
#define MAP_T_MESSAGE_CANNOT_BE_DELIVERED_TO_THE_PEER		3

/* MAP_T_Provider_Reason: */
#define MAP_T_PROVIDER_MALFUNCTION			0	/* not on wire */
#define MAP_T_SUPPORTING_DIALOGUE_TRANSACTION_RELEASED	1	/* not on wire */
#define MAP_T_RESOURCE_LIMITATION			2	/* not on wire */
#define MAP_T_MAINTENANCE_ACTIVITY			3	/* not on wire */
#define MAP_T_VERSION_INCOMPATIBILITY			4	/* not on wire */
#define MAP_T_ABNORMAL_MAP_DIALOGUE			5	/* not on wire */

/* MAP_T_Source: */
#define MAP_T_MAP_PROBLEM		0
#define MAP_T_TC_PROBLEM		1
#define MAP_T_NETWORK_SERVICE_PROBLEM	2

/* MAP_T_P_Abort_Reason: */
#define MAP_T_ABNORMAL_DIALOGUE		0
#define MAP_T_INVALID_PDU		1

/* MAP_T_P_Abort_Cause: */
#define MAP_T_UNRECOGNIZED_MESSAGE_TYPE			0
#define MAP_T_UNRECOGNIZED_TRANSACTION_ID		1
#define MAP_T_BADLY_FORMATTED_TRANSACTION_PORTION	2
#define MAP_T_INCORRECT_TRANSACTION_PORTION		3
#define MAP_T_RESOURCE_LIMITATION			4
#define MAP_T_ABNORMAL_DIALOGUE				5
#define MAP_T_NO_COMMON_DIALOGUE_PORTION		6

/* MAP_T_Report_Cause: */
#define MAP_T_NO_TRANSLATIONS_FOR_ADDRESS_OF_SUCH_NATURE	  0
#define MAP_T_NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS		  1
#define MAP_T_SUBSYSTEM_CONGESTION				  2
#define MAP_T_SUBSYSTEM_FAILURE					  3
#define MAP_T_UNEQUIPPED_USER					  4
#define MAP_T_MTP_FAILUIRE					  5
#define MAP_T_NETWORK_CONGESTION				  6
#define MAP_T_SCCP_UNQUALIFIED					  7
#define MAP_T_ERROR_IN_MESSAGE_TRANSPORT			  8
#define MAP_T_ERROR_IN_LOCAL_PROCESSING				  9
#define MAP_T_DESTINATION_CANNOT_PERFORM_REASSEMBLY		 10
#define MAP_T_SCCP_FAILURE					 11
#define MAP_T_HOP_COUNTER_VIOLATION				 12
#define MAP_T_SEGMENTATION_NOT_SUPPORTED			 13
#define MAP_T_SEGMENTATION_FAILURE				 14
#define MAP_T_MESSAGE_CHANGE_FAILURE				247	/* ANSI SCCP only */
#define MAP_T_INVALID_INS_ROUTING_REQUEST			248	/* ANSI SCCP only */
#define MAP_T_INVALID_ISNI_ROUTING_REQUEST			249	/* ANSI SCCP only */
#define MAP_T_UNAUTHORIZED_MESSAGE				250	/* ANSI SCCP only */
#define MAP_T_MESSAGE_INCOMPATIBILITY				251	/* ANSI SCCP only */
#define MAP_T_CANNOT_PERFORM_ISNI_CONSTRAINED_ROUTING		252	/* ANSI SCCP only */
#define MAP_T_REDUNDANT_ISNI_CONSTRAINED_ROUTING_INFORMATION	253	/* ANSI SCCP only */
#define MAP_T_UNABLE_TO_PERFORM_ISNI_IDENTIFICATION		254	/* ANSI SCCP only */

/* MAP_T_Refuse_Reason: */
#define MAP_T_NO_REASON_GIVEN				0
#define MAP_T_INVALID_DESTINATION_REFERENCE		1
#define MAP_T_INVALID_ORIGINATING_REFERENCE		2
#define MAP_T_ENCAPSULATED_AC_NOT_SUPPORTED		3
#define MAP_T_TRANSPORT_PROTECTION_NOT_ADEQUATE		4
#define MAP_T_REMOTE_NODE_NOT_REACHABLE			5	/* not on wire */
#define MAP_T_APPLICATION_CONTEXT_NOT_SUPPORTED		6	/* not on wire */
#define MAP_T_POTENTIAL_VERSION_INCOMPATIBILITY		7	/* not on wire */
#define MAP_T_SECURED_TRANSPORT_NOT_POSSIBLE		8	/* not on wire */

/* MAP_T_User_Error: */
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

/* MAP_T_Provider_Error: */
#define MAP_T_DUPLICATED_INVOKE_ID			0
#define MAP_T_NOT_SUPPORTED_SERVICE			1
#define MAP_T_MISTYPED_PARAMETER			2
#define MAP_T_RESOURCE_LIMITATION			3
#define MAP_T_INITIATING_RELEASE			4
#define MAP_T_UNEXPECTED_RESPONSE_FROM_THE_PEER		5
#define MAP_T_SERVICE_COMPLETION_FAILURE		6
#define MAP_T_NO_RESPONSE_FROM_THE_PEER			7
#define MAP_T_INVALID_RESPONSE_RECEIVED			8

/* MAP_T_Resource_Unavailable_Reason: */
#define MAP_T_SHORT_TERM_RESOURCE_LIMITATION	0
#define MAP_T_LONG_TERM_RESOURCE_LIMITATION	1

/* MAP_T_Procedure_Cancellation_Reason: */
#define MAP_T_HANDOVER_CANCELLATION		0
#define MAP_T_RADIO_CHANNEL_RELEASE		1
#define MAP_T_NETWORK_PATH_RELEASE		2
#define MAP_T_CALL_RELEASE			3
#define MAP_T_ASSOCIATED_PROCEDURE_FAILURE	4
#define MAP_T_TANDEM_DIALOGUE_RELEASE		5
#define MAP_T_REMOTE_OPERATIONS_FAILURE		6

#endif				/* __XMAP_GSM_H__ */
