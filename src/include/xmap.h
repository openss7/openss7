/*****************************************************************************

 @(#) $Id: xmap.h,v 1.1.2.1 2009-06-21 11:23:46 brian Exp $

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

 Last Modified $Date: 2009-06-21 11:23:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap.h,v $
 Revision 1.1.2.1  2009-06-21 11:23:46  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __XMAP_H__
#define __XMAP_H__

/*
 * The <xmap.h> header declares the inteface functions, the structures passed to
 * and from those functions, and the defined constants used by the functions and
 * structure.
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
 *   xom-packages(1) xmap(1) common(1) }
 */
#define OMP_O_MAP_COMMON_PKG \
	"\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x01"	/* 1.3.6.1.4.1.29591.1.1.1 */

#define OMP_O_MAP_GSM_NETWORK \
	"\x04\x00\x00\x01\x00" /* 0.4.0.0.1 */
	/* { itu-t(0) identified-origanization(4) etsi(0) mobile-domain(0) gsm-Network(1) } */

#define mapP_gsmNetwork(X) (OMP_O_MAP_GSM_NETWORK# #X)

#define OMP_O_MAP_GSM_AC \
	mapP_gsmNetwork(\x00) /* 0.4.0.0.1.0 */
	/* { itu-t(0) identified-origanization(4) etsi(0) mobile-domain(0) gsm-Network(1) ac(0) } */

#define mapP_acId(X) (OMP_O_MAP_GSM_AC# #X)

#define OMP_O_MAP_GSM_AS \
	mapP_gsmNetwork(\x01) /* 0.4.0.0.1.1 */

#define mapP_asId(X) (OMP_O_MAP_GSM_AS# #X)

/* Defined constants */

/* Intermediate object identifier macro */
#define mapP_comn(X) (OMP_O_MAP_COMMON_PKG# #X)

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
#define OMP_O_MAP_C_ABORT_ARGUMENT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ABORT_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ACCEPT_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ACSE_ARGS			mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ADDRESS			mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_APPLICATION_CONTEXT_LIST	mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ASSOC_ARGUMENT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ASSOC_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_CLOSE_ARGUMENT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_CONTEXT			mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_EXTENSION			mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_GENERIC_SERVICE_ARGUMENT	mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_GENERIC_SERVICE_RESULT	mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_MAP_ASSOC_ARGS		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_NOTICE_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_OPEN_ARGUMENT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_OPERATION_ARGUMENT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_OPERATION_ERROR		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_OPERATION_REJECT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_OPERATION_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_P_ABORT_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_PRESENTATION_CONTEXT	mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_PRESENTATION_LAYER_ARGS	mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_REFUSE_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_RELEASE_ARGUMENT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_RELEASE_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_SERVICE_ARGUMENT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_SERVICE_ERROR		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_SERVICE_REJECT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_SERVICE_RESULT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_SESSION			mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_TITLE			mapP_comn(\x87\x69)	/* 1001 */

#define OMP_O_MAP_C_PROPRIETARY_ARGS		mapP_comn(\x87\x69)	/* 1001 */

/* Error classes: */
#define OMP_O_MAP_C_BAD_ARGUMENT		mapP_comn(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ERROR			mapP_comn(\x87\x69)	/* 1001 */

/* Error attributes: */
#define MAP_A_OM_ATTRIBUTE						((OM_type)10021)
#define MAP_A_OM_BAD_ARGUMENT						((OM_type)10022)
#define MAP_A_OM_CLASS							((OM_type)10023)
#define MAP_A_OM_INDEX							((OM_type)10024)
#define MAP_A_OM_SUBOBJECT						((OM_type)10025)
#define MAP_A_PARAMETER							((OM_type)10026)
#define MAP_A_PROBLEM							((OM_type)10028)

/* The OM syntax names that are defined are listed below. */
#define MAP_S_TBCD_STRING		((OM_syntax) 32)
#define MAP_S_SCCP_ADDRESS_STRING	((OM_syntax) 33)

/* The OM attribute names that are defined are listed below. */

#define MAP_APPLICATION_CONTEXT_NAME		((OM_type)11001)
#define MAP_DESTINATION_ADDRESS			((OM_type)11002)
#define MAP_DESTINNATION_REFERENCE		((OM_type)11003)
#define MAP_ORIGINATING_ADDRESS			((OM_type)11004)
#define MAP_ORIGINATING_REFERENCE		((OM_type)11005)
#define MAP_SPECIFIC_INFORMATION		((OM_type)11006)
#define MAP_ASSOCIATION_ID			((OM_type)11007)
#define MAP_RESPONDING_ADDRESS			((OM_type)11008)
#define MAP_REFUSE_REASON			((OM_type)11009)
#define MAP_PROVIDER_ERROR			((OM_type)11010)
#define MAP_RELEASE_METHOD			((OM_type)11011)
#define MAP_USER_REASON				((OM_type)11012)
#define MAP_DIAGNOSTIC_INFORMATION		((OM_type)11013)
#define MAP_PROVIDER_REASON			((OM_type)11014)
#define MAP_SOURCE				((OM_type)11015)
#define MAP_PROBLEM_DIAGNOSTIC			((OM_type)11016)

/* common parameters */
#define MAP_INVOKE_ID				((OM_type)11017)
#define MAP_LINKED_ID				((OM_type)11018)
#define MAP_SERVICE_PROVIDER_ERROR		((OM_type)11019)
#define MAP_SERVICE_USER_ERROR			((OM_type)11020)
#define MAP_ALL_INFORMATION_SENT		((OM_type)11021)

/* numbering and identification parameters */
#define MAP_ADDITIONAL_NUMBER			((OM_type)11067) /* XXX */
#define MAP_ADDITIONAL_V_GMLC_ADDRESS		((OM_type)11074)
#define MAP_APN					((OM_type)11063)
#define MAP_B_SUBSCRIBER_NUMBER			((OM_type)11069)
#define MAP_B_SUBSCRIBER_SUBADDRESS		((OM_type)11060)
#define MAP_CALLED_NUMBER			((OM_type)11048)
#define MAP_CALLING_NUMBER			((OM_type)11049)
#define MAP_CURRENT_LOCATION_AREA_ID		((OM_type)11028)
#define MAP_FORWARDED_TO_NUMBER			((OM_type)11044)
#define MAP_FORWARDED_TO_SUBADDRESS		((OM_type)11047)
#define MAP_GGSN_ADDRESS			((OM_type)11062)
#define MAP_GMSC_ADDRESS			((OM_type)11055)
#define MAP_GROUP_ID				((OM_type)11057)
#define MAP_GSMSCF_ADDRESS			((OM_type)11069)
#define MAP_HANDOVER_NUMBER			((OM_type)11043)
#define MAP_H_GMLC_ADDRESS			((OM_type)11071)
#define MAP_HLR_ID				((OM_type)11037)
#define MAP_HLR_NUMBER				((OM_type)11035)
#define MAP_IMEI				((OM_type)11024)
#define MAP_IMEISV				((OM_type)11025)
#define MAP_IMSI				((OM_type)11022) /* XXX */
#define MAP_LMSI				((OM_type)11038) /* XXX */
#define MAP_LMU_NUMBER				((OM_type)11061)
#define MAP_LOCATION_INFORMATION_FOR_GPRC	((OM_type)11054)
#define MAP_LOCATION_INFORMATION		((OM_type)11053)
#define MAP_LONG_FORWARDED_TO_NUMBER		((OM_type)11045)
#define MAP_LONG_FTN_SUPPORTED			((OM_type)11046)
#define MAP_MLC_NUMBER				((OM_type)11062)
#define MAP_MSC_NUMBER				((OM_type)11033) /* XXX */
#define MAP_MSISDN_ALERT			((OM_type)11052)
#define MAP_MS_ISDN				((OM_type)11039)
#define MAP_MULTICALL_BEARER_INFORMATION	((OM_type)11063)
#define MAP_MULTIPLE_BEARER_NOT_SUPPORTED	((OM_type)11065)
#define MAP_MULTIPLE_BEARER_REQUESTED		((OM_type)11064)
#define MAP_NETWORK_NODE_NUMBER			((OM_type)11064) /* XXX */
#define MAP_NORTH_AMERICAN_EQUAL_ACCESS_PIC	((OM_type)11058)
#define MAP_OMC_ID				((OM_type)11040)
#define MAP_ORIGINAL_DIALED_NUMBER		((OM_type)11050)
#define MAP_ORIGINATING_ENTITY_NUMBER		((OM_type)11032)
#define MAP_PDP_ADDRESS				((OM_type)11066)
#define MAP_PDP_CHARGING_CHARACTERISTICS	((OM_type)11066)
#define MAP_PDP_TYPE				((OM_type)11065)
#define MAP_PPR_ADDRESS				((OM_type)11072)
#define MAP_PREVIOUS_LOCATION_AREA_ID		((OM_type)11026)
#define MAP_P_TMSI				((OM_type)11068)
#define MAP_RAB_ID				((OM_type)11068)
#define MAP_RELOCATION_NUMBER_LIST		((OM_type)11042)
#define MAP_ROAMING_NUMBER			((OM_type)11041)
#define MAP_ROUTEING_NUMBER			((OM_type)11073)
#define MAP_SELECTED_RAB_ID			((OM_type)11067)
#define MAP_SERVICE_CENTRE_ADDRESS		((OM_type)11051) /* XXX */
#define MAP_SERVING_CELL_ID			((OM_type)11059)
#define MAP_SGSN_ADDRESS			((OM_type)11061)
#define MAP_SGSN_NUMBER				((OM_type)11060) /* XXX */
#define MAP_STORED_LOCATION_AREA_ID		((OM_type)11027)
#define MAP_TARGET_CELL_ID			((OM_type)11030)
#define MAP_TARGET_LOCATION_AREA_ID		((OM_type)11029)
#define MAP_TARGET_MSC_NUMBER			((OM_type)11034)
#define MAP_TARGET_RNC_ID			((OM_type)11031)
#define MAP_TMSI				((OM_type)11023)
#define MAP_V_GMLC_ADDRESS			((OM_type)11070)
#define MAP_VLR_NUMBER				((OM_type)11036)
#define MAP_VMSC_ADDRESS			((OM_type)11056)



/*
 * The following enumerations tags and enumeration constants are defined for use
 * as values of the corresponding OM attributes:
 */

/* MAP_T_Refuse_reason: */

#define MAP_T_NO_REASON_GIVEN				0
#define MAP_T_INVALID_DESTINATION_REFERENCE		1
#define MAP_T_INVALID_ORIGINATING_REFERENCE		2
#define MAP_T_APPLICATION_CONTEXT_NOT_SUPPORTED		3
#define MAP_T_TRANSPORT_PROTECTION_NOT_ADEQUATE		4

#define MAP_T_REMOTE_NOT_NOT_REACHABLE			5
#define MAP_T_POTENTIAL_VERSION_INCOMPATIBLITY		6
#define MAP_T_SECURED_TRANSPORT_NOT_POSSIBLE		7

/* MAP_T_Release_method: */
#define MAP_T_NORMAL_RELEASE				1
#define MAP_T_PREARRANGED_END				2

/* map-UserAbortChoice CHOICE { */

/* MAP_T_User_reason: */
/* userSpecificReason [0] IMPLICIT NULL */
#define MAP_T_USER_SPECIFIC_REASON			0
/* userResourceLimitation [1] IMPLICIT NULL */
#define MAP_T_RESOURCE_LIMITATION_CONGESTION		1
/* resourceUnavailable [2] IMPLICIT ENUMERATED */
#define MAP_T_RESOURCE_UNAVAILABLE			2
/* applicationProcedureCancellation [3] IMPLICIT ENUMERATED */
#define MAP_T_APPLICATION_PROCEDURE_CANCELLATION	3
#define MAP_T_PROCEDURE_ERROR				4

/* MAP_T_Diagnostic_information: */
/* resourceUnavailable [2] IMPLICIT ENUMERATED */
#define MAP_T_SHORT_TERM_PROBLEM			0
#define MAP_T_LONG_TERM_PROBLEM				1
/* applicationProcedureCancellation [3] IMPLICIT ENUMERATED */
#define MAP_T_HANDOVER_CANCELLATION			0
#define MAP_T_RADIO_CHANNEL_RELEASED			1
#define MAP_T_NETWORK_PATH_RELEASE			2
#define MAP_T_CALL_RELEASE				3
#define MAP_T_ASSOCIATED_PROCEDURE_FAILURE		4
#define MAP_T_TANDEM_DIALOGUE_RELEASED			5
#define MAP_T_REMOTE_OPERATIONS_FAILURE			6

/* MAP_T_Provider_reason: */
#define MAP_T_PROVIDER_MALFUNCTION			1
#define MAP_T_SUPPORTING_DIALOGUE_TRANSACTION_RELEASED	2
#define MAP_T_RESOURCE_LIMITATION			3
#define MAP_T_MAINTENANCE_ACTIVITY			4
#define MAP_T_VERSION_INCOMPATIBILITY			5
#define MAP_T_ABNORMAL_MAP_DIALOGUE			6

/* MAP_T_Source: */
#define MAP_T_MAP_PROBLEM				1
#define MAP_T_TC_PROBLEM				2
#define MAP_T_NETWORK_SERVICE_PROBLEM			3

/* MAP_T_Problem_diagnostic: */
#define MAP_T_ABNORMAL_EVENT_DETECTED_BY_PEER		1
#define MAP_T_RESPONSE_REJECTED_BY_THE_PEER		2
#define MAP_T_ABNORMAL_EVENT_RECEIVED_FROM_THE_PEER	3
#define MAP_T_MESSAGE_CANNOT_BE_DELIVERED_TO_THE_PEER	4

/* MAP_T_Abort_Source: */
#define MAP_T_ABORT_SOURCE_ACSE_SERVICE_USER		1
#define MAP_T_ABORT_SOURCE_ACSE_SERVICE_PROVIDER	2

/* MAP_T_Abort_Diagnostic: */
#define MAP_T_ABORT_DIAGNOSTIC_NO_REASON_GIVEN					1
#define MAP_T_ABORT_DIAGNOSTIC_PROTOCOL_ERROR					2
#define MAP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNIZED	3
#define MAP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED		4
#define MAP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_FAILURE				5
#define MAP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_REQUIRED				6

/* MAP_T_MAP_Abort_Souce: */

/* MAP_T_ACSE_Service_User: */
#define MAP_T_ACSE_SERVICE_USER_NULL						 1
#define MAP_T_ACSE_SERVICE_USER_NO_REASON_GIVEN					 2
#define MAP_T_ACSE_SERVICE_USER_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED		 3
#define MAP_T_ACSE_SERVICE_USER_CALLING_AP_TITLE_NOT_RECOGNIZED			 4
#define MAP_T_ACSE_SERVICE_USER_CALLING_AP_INVOCATION_IDENTIFIER_NOT_RECOGNIZED	 5
#define MAP_T_ACSE_SERVICE_USER_CALLING_AE_QUALIFIER_NOT_RECOGNIZED		 6
#define MAP_T_ACSE_SERVICE_USER_CALLING_AE_INVOCATION_IDENTIFIER_NOT_RECOGNIZED	 7
#define MAP_T_ACSE_SERVICE_USER_CALLED_AP_TITLE_NOT_RECOGNIZED			 8
#define MAP_T_ACSE_SERVICE_USER_CALLED_AP_INVOCATION_IDENTIFIER_NOT_RECOGNIZED	 9
#define MAP_T_ACSE_SERVICE_USER_CALLED_AE_QUALIFIER_NOT_RECOGNIZED		10
#define MAP_T_ACSE_SERVICE_USER_CALLED_AE_INVOCATION_IDENTIFIER_NOT_RECOGNIZED	11
#define MAP_T_ACSE_SERVICE_USER_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNIZED	12
#define MAP_T_ACSE_SERVICE_USER_AUTHENTICATION_MECHANISM_NAME_REQUIRED		13
#define MAP_T_ACSE_SERVICE_USER_AUTHENTICATION_FAILURE				14
#define MAP_T_ACSE_SERVICE_USER_AUTHENTICATION_REQUIRED				15

/* MAP_T_ACSE_Service_Provider: */
#define MAP_T_ACSE_SERVICE_PROVIDER_NULL			1
#define MAP_T_ACSE_SERVICE_PROVIDER_NO_REASON_GIVEN		2
#define MAP_T_ACSE_SERVICE_PROVIDER_NO_COMMON_ACSE_VERSION	3

/* MAP_T_Assoc_Result: */
#define MAP_T_ACCEPT				1
#define MAP_T_REJECT_PERMANENT			2
#define MAP_T_REJECT_TRANSIENT			3

/* MAP_T_Asynchronous: */
#define MAP_T_FALSE				1
#define MAP_T_TRUE				2

/* MAP_E_Problem: */
#define MAP_E_BAD_ADDRESS			1001
#define MAP_E_BAD_ARGUMENT			1002
#define MAP_E_BAD_CLASS				1003
#define MAP_E_BAD_CONTEXT			1004
#define MAP_E_BAD_ERROR				1005
#define MAP_E_BAD_LINKED_REPLY			1006
#define MAP_E_BAD_PROCEDURAL_USE		1007
#define MAP_E_BAD_RESULT			1008
#define MAP_E_BAD_SESSION			1009
#define MAP_E_BAD_SYNTAX			1010
#define MAP_E_BAD_TITLE				1011
#define MAP_E_BAD_VALUE				1012
#define MAP_E_BAD_WORKSPACE			1013
#define MAP_E_BROKEN_SESSION			1014
#define MAP_E_COMMUNICATIONS_PROBLEM		1015
#define MAP_E_EXCLUSIVE_ATTRIBUTE		1016
#define MAP_E_INVALID_CONNECTION_ID		1017
#define MAP_E_INVALID_ATTRIBUTE_ID		1018
#define MAP_E_MISCELLANEOUS			1019
#define MAP_E_MISSING_ATTRIBUTE			1020
#define MAP_E_NOT_MULTI_VALUED			1021
#define MAP_E_NOT_SUPPORTED			1022
#define MAP_E_NO_SUCH_OPERATION			1023
#define MAP_E_REPLY_LIMIT_EXCEEDED		1024
#define MAP_E_SESSION_TERMINATED		1025
#define MAP_E_SYSTEM_ERROR			1026
#define MAP_E_TIME_LIMIT_EXCEEDED		1027
#define MAP_E_TOO_MANY_OPERATIONS		1028
#define MAP_E_TOO_MANY_SESSIONS			1029

/* MAP_T_Modify_Operator: */
#define MAP_T_REPLACE		0
#define MAP_T_ADD_VALUES	1
#define MAP_T_REMOVE_VALUES	2
#define MAP_T_SET_TO_DEFAULT	3

/* The typedef name MAP_status is defined as: */
typedef int MAP_status;

/* The following constants are defined: */
#define MAP_SUCCESS			((MAP_status)0)
#define MAP_NO_WORKSPACE		((MAP_status)1)
#define MAP_INVALID_SESSION		((MAP_status)2)
#define MAP_INVALID_ASSOCIATION		((MAP_status)3)
#define MAP_INVALID_INVOKE_ID		((MAP_status)4)
#define MAP_INSUFFICIENT_RESOURCES	((MAP_status)5)

/* The following constants are defined: */
#define MAP_ACTIVATE				0
#define MAP_DEACTIVATE				1
#define MAP_QUERY_STATE				2
#define MAP_QUERY_SUPPORTED			3

/* The following structures are defined: */
typedef struct {
	OM_object_identifier feature;
	OM_sint request;
	OM_boolean response;
} MAP_feature;

typedef struct {
	OM_private_object bound_session;
	OM_boolean activated;
} MAP_waiting_sessions;

/* The following constants, of type OM_Object, are defined: */
#define MAP_ABSENT_OBJECT		((OM_object)0)
#define MAP_DEFAULT_CONTEXT		((OM_object)0)
#define MAP_DEFAULT_SESSION		((OM_object)0)
#define MAP_DEFAULT_ASSOCIATION		((OM_object)0)

/* The following integer constants are defined: */
#define MAP_OPEN_IND			15
#define MAP_OPEN_CON			16
#define MAP_REFUSE_IND			17
#define MAP_SERV_IND			18
#define MAP_SERV_CON			19
#define MAP_DELIM_IND			20
#define MAP_CLOSE_IND			21
#define MAP_ABORT_IND			22
#define MAP_NOTICE_IND			23
#define MAP_STC1_IND			28
#define MAP_STC2_IND			29
#define MAP_STC3_IND			30
#define MAP_STC4_IND			31

#define MAP_COMPLETETED			 1
#define MAP_INCOMING			 2
#define MAP_NOTHING			 3
#define MAP_OUTSTANDING			 4
#define MAP_PARTIAL			 5
#define MAP_NO_VALID_FILE_DESCRIPTOR	-1

#define MAP_MAX_OUTSTANDING_OPERATIONS		1024

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

extern OM_workspace map_initialize(void);

extern void mp_shutdown(OM_workspace workspace);

extern MAP_status mp_validate_object(OM_workspace workspace,
				     OM_object test_object,
				     OM_private_object * bad_argument_return);

extern MAP_status map_get_last_error(OM_workspace workspace,
				     OM_uint32 * additional_error_return);

extern MAP_status map_negotiate(MAP_feature feature_list[],
				OM_workspace workspace);

extern MAP_status map_wait(MAP_waiting_sessions bound_session_list[],
			   OM_workspace workspace, OM_uint32 timeout);

extern MAP_status map_receive(OM_private_object session,
			      OM_private_object context,
			      OM_sint * primitive_return,
			      OM_sint * completion_flag_return,
			      MAP_status * operation_notification_status_return,
			      OM_private_object * result_or_argument_return,
			      OM_uint32 * dialog_id_return);

/** @brief create a new MAP application entity invocation (TC User)
  * @param session features of the new session
  * @param workspace OM workspace for the MAP OM package
  * @param open_session_return
  *
  * The map_bind() function accepts a session specification and a MAP OM package
  * workspace and, when successful, returns a private OM object that represents
  * the session.  This session may be used to identify the MAP application
  * entity invocation (TC User) in the further functions.
  */
extern MAP_status map_bind(OM_object session, OM_workspace workspace,
			   OM_private_object * open_session_return);

/** @brief destroy and existing MAP application entity invocation (TC User)
  * @param session the session to destroy
  *
  * The map_unbind() function accepts an existing session object (one returned
  * from a call to map_bind()) and removes the session from the workspace.  The
  * session may no longer be used to identify the MAP application entity
  * invocation (TC User).
  */
extern MAP_status map_unbind(OM_private_object session);

/** @brief establish a MAP AP Invocation with a remote MAP AE
  * @param session session representing the local MAP AEI
  * @param context information on the context of the MAP AP
  * @param argument arguments of the MAP AP Invocation
  * @param result_return result of a syncrhonous operation
  * @param dialog_id_return an identifier for the dialog
  */
extern MAP_status map_open_req(OM_private_object session,
			       OM_private_object context, OM_object argument,
			       OM_private_object * result_return,
			       OM_uint32 * dialog_id_return);

extern MAP_status map_open_rsp(OM_private_object session,
			       OM_private_object context, OM_object response,
			       OM_uint32 dialog_id);

extern MAP_status map_refuse(OM_private_object session,
			     OM_private_object context, OM_object refusal,
			     OM_uint32 dialog_id);

extern MAP_status map_close(OM_private_object session, OM_uint32 dialog_id);

extern MAP_status map_abort(OM_private_object session, OM_uint32 dialog_id);

extern MAP_status map_delim_req(OM_private_object session, OM_uint32 dialog_id);

extern MAP_status map_service_req(OM_private_object session,
				  OM_private_object context, OM_object argument,
				  OM_private_object * result_return,
				  OM_uint32 * invoke_id_return);

extern MAP_status map_service_rsp(OM_private_object session,
				  OM_private_object context, OM_object response,
				  OM_uint32 invoke_id);
#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif
#endif				/* __XMAP_H__ */
// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
