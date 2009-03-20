/*****************************************************************************

 @(#) $Id: xmap_gsm_ls.h,v 0.9.2.2 2009-03-20 18:27:40 brian Exp $

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

 Last Modified $Date: 2009-03-20 18:27:40 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm_ls.h,v $
 Revision 0.9.2.2  2009-03-20 18:27:40  brian
 - documentation and headers

 Revision 0.9.2.1  2009-03-13 11:20:25  brian
 - doc and header updates

 *****************************************************************************/

#ifndef __XMAP_GSM_LS_H__
#define __XMAP_GSM_LS_H__

#ident "@(#) $RCSfile: xmap_gsm_ls.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2008-2009 Monavacon Limited."

#include <xom.h>
#include <xmap.h>
#include <xmap_gsm.h>

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-ls(7) }
 */
#define OMP_O_MAP_GSM_LS_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x07"

/* Intermediate object identifier macro: */
#define mapP_gsm_ls(X) (OMP_O_MAP_GSM_LS_PKG# #X)

/* This application context is used for location service gateway procedures. */
#define OMP_O_MAP_LOCATION_SVC_GATEWAY_CONTEXT		mapP_acId(\x25)
#define OMP_O_MAP_LOCATION_SVC_GATEWAY_CONTEXT_V3	mapP_acId(\x25\x03)
/* locationCvsGatewayPackage-v3 */

/* This application context is used for location service enquiry procedures. */
#define OMP_O_MAP_LOCATION_SVC_ENQUIRY_CONTEXT		mapP_acId(\x26)
#define OMP_O_MAP_LOCATION_SVC_ENQUIRY_CONTEXT_V3	mapP_acId(\x26\x03)
/* locationSvcEnquiryPackage-v3
 * locationSvcReportingPackage-v3 */

/* OM class names (prefixed MAP_C_) */
#define OMP_O_MAP_C_AREA				mapP_gsm_ls(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_AREA_DEFINITION			mapP_gsm_ls(\x87\x6A)	/* 1002 */
#define OMP_O_MAP_C_AREA_EVENT_INFO			mapP_gsm_ls(\x87\x6B)	/* 1003 */
#define OMP_O_MAP_C_DEFERRED_MT_LR_DATA			mapP_gsm_ls(\x87\x6C)	/* 1004 */
#define OMP_O_MAP_C_LCS_CLIENT_ID			mapP_gsm_ls(\x87\x6D)	/* 1005 */
#define OMP_O_MAP_C_LCS_CLIENT_NAME			mapP_gsm_ls(\x87\x6E)	/* 1006 */
#define OMP_O_MAP_C_LCS_CODEWORD			mapP_gsm_ls(\x87\x6F)	/* 1007 */
#define OMP_O_MAP_C_LCS_LOCATION_INFO			mapP_gsm_ls(\x87\x70)	/* 1008 */
#define OMP_O_MAP_C_LCS_PRIVACY_CHECK			mapP_gsm_ls(\x87\x71)	/* 1009 */
#define OMP_O_MAP_C_LCS_QOS				mapP_gsm_ls(\x87\x72)	/* 1010 */
#define OMP_O_MAP_C_LCS_REQUESTOR_ID			mapP_gsm_ls(\x87\x73)	/* 1011 */
#define OMP_O_MAP_C_LOCATION_TYPE			mapP_gsm_ls(\x87\x74)	/* 1012 */
#define OMP_O_MAP_C_PERIODIC_LDR_INFO			mapP_gsm_ls(\x87\x75)	/* 1013 */
#define OMP_O_MAP_C_PLMN_LIST				mapP_gsm_ls(\x87\x76)	/* 1014 */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_LOCATION_ARG	mapP_gsm_ls(\x87\x77)	/* 1015 */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_LOCATION_RES	mapP_gsm_ls(\x87\x78)	/* 1016 */
#define OMP_O_MAP_C_REPORTING_PLMN			mapP_gsm_ls(\x87\x79)	/* 1007 */
#define OMP_O_MAP_C_REPORTING_PLMN_LIST			mapP_gsm_ls(\x87\x7A)	/* 1018 */
#define OMP_O_MAP_C_RESPONSE_TIME			mapP_gsm_ls(\x87\x7B)	/* 1019 */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_LCS_ARG		mapP_gsm_ls(\x87\x7C)	/* 1020 */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_LCS_RES		mapP_gsm_ls(\x87\x7D)	/* 1021 */
#define OMP_O_MAP_C_SUBSCRIBER_LOCATION_REPORT_ARG	mapP_gsm_ls(\x87\x7E)	/* 1022 */
#define OMP_O_MAP_C_SUBSCRIBER_LOCATION_REPORT_RES	mapP_gsm_ls(\x87\x7F)	/* 1023 */

/* OM attribute names (prefixed MAP_) */
#define MAP_A_ACCURACY_FULFILMENT_INDICATOR		((OM_type)9001)
#define MAP_A_ADDITIONAL_LCS_CAPABILITY_SETS		((OM_type)9002)
#define MAP_A_ADDITIONAL_V_GMLC_ADDRESS			((OM_type)9004)
#define MAP_A_ADD_LOCATION_ESTIMATE			((OM_type)9005)
#define MAP_A_AGE_OF_LOCATION_ESTIMATE			((OM_type)9006)
#define MAP_A_AREA_DEFINITION				((OM_type)9008)
#define MAP_A_AREA_EVENT_INFO				((OM_type)9009)
#define MAP_A_AREA_IDENTIFICATION			((OM_type)9010)
#define MAP_A_AREA_LIST					((OM_type)9011)
#define MAP_A_AREA_TYPE					((OM_type)9012)
#define MAP_A_CALL_SESSION_RELATED			((OM_type)9013)
#define MAP_A_CALL_SESSION_UNRELATED			((OM_type)9014)
#define MAP_A_CELL_ID_OR_SAI				((OM_type)9015)
#define MAP_A_DATA_CODING_SCHEME			((OM_type)9016)
#define MAP_A_DEFERRED_LOCATION_EVENT_TYPE		((OM_type)9017)
#define MAP_A_DEFERRED_MT_LR_DATA			((OM_type)9018)
#define MAP_A_DEFERRED_MT_LR_RESPONSE_INDICATOR		((OM_type)9019)
#define MAP_A_GERAN_POSITIONING_DATA			((OM_type)9022)
#define MAP_A_H_GMLC_ADDRESS				((OM_type)9024)
#define MAP_A_HORIZONTAL_ACCURACY			((OM_type)9025)
#define MAP_A_INTERVAL_TIME				((OM_type)9028)
#define MAP_A_LCS_APN					((OM_type)9029)
#define MAP_A_LCS_CLIENT_DIALED_BY_MS			((OM_type)9030)
#define MAP_A_LCS_CLIENT_EXTENAL_ID			((OM_type)9031)
#define MAP_A_LCS_CLIENT_ID				((OM_type)9032)
#define MAP_A_LCS_CLIENT_NAME				((OM_type)9034)
#define MAP_A_LCS_CLIENT_TYPE				((OM_type)9035)
#define MAP_A_LCS_CODEWORD				((OM_type)9036)
#define MAP_A_LCS_CODEWORD_STRING			((OM_type)9037)
#define MAP_A_LCS_EVENT					((OM_type)9038)
#define MAP_A_LCS_FORMAT_INDICATOR			((OM_type)9039)
#define MAP_A_LCS_LOCATION_INFO				((OM_type)9040)
#define MAP_A_LCS_PRIORITY				((OM_type)9041)
#define MAP_A_LCS_PRIVACY_CHECK				((OM_type)9042)
#define MAP_A_LCS_QOS					((OM_type)9043)
#define MAP_A_LCS_REFERENCE_NUMBER			((OM_type)9044)
#define MAP_A_LCS_REQUESTOR_ID				((OM_type)9045)
#define MAP_A_LOCATION_ESTIMATE				((OM_type)9048)
#define MAP_A_LOCATION_ESTIMATE_TYPE			((OM_type)9049)
#define MAP_A_LOCATION_TYPE				((OM_type)9050)
#define MAP_A_MLC_NUMBER				((OM_type)9051)
#define MAP_A_MO_LR_SHORT_CIRCUIT_INDICATOR		((OM_type)9052)
#define MAP_A_NA_ESRD					((OM_type)9054)
#define MAP_A_NA_ESRK					((OM_type)9055)
#define MAP_A_NAME_STRING				((OM_type)9056)
#define MAP_A_OCCURENCE_INFO				((OM_type)9058)
#define MAP_A_PERIODIC_LDR_INFO				((OM_type)9059)
#define MAP_A_PLMN_ID					((OM_type)9060)
#define MAP_A_PLMN_LIST					((OM_type)9061)
#define MAP_A_PLMN_LIST_PRIORITIZED			((OM_type)9062)
#define MAP_A_PPR_ADDRESS				((OM_type)9063)
#define MAP_A_PRIVACY_OVERRIDE				((OM_type)9064)
#define MAP_A_PSEUDONUM_INDICATOR			((OM_type)9065)
#define MAP_A_RAN_PERIODIC_LOCATION_SUPPORT		((OM_type)9066)
#define MAP_A_RAN_TECHNOLOGY				((OM_type)9067)
#define MAP_A_REPORTING_AMOUNT				((OM_type)9068)
#define MAP_A_REPORTING_INTERVAL			((OM_type)9069)
#define MAP_A_REPORTING_PLMN				((OM_type)9070)
#define MAP_A_REPORTING_PLMN_LIST			((OM_type)9071)
#define MAP_A_REQUESTOR_ID_STRING			((OM_type)9072)
#define MAP_A_RESPONSE_TIME				((OM_type)9073)
#define MAP_A_RESPONSE_TIME_CATEGORY			((OM_type)9074)
#define MAP_A_SEQUENCE_NUMBER				((OM_type)9076)
#define MAP_A_SLR_ARG_EXTENSION_CONTAINER		((OM_type)9077)
#define MAP_A_SUPPORTED_GAD_SHAPES			((OM_type)9078)
#define MAP_A_TARGET_MS					((OM_type)9080)
#define MAP_A_TERMINATION_CAUSE				((OM_type)9081)
#define MAP_A_UTRAN_POSITIONING_DATA			((OM_type)9082)
#define MAP_A_VELOCITY_ESTIMATE				((OM_type)9083)
#define MAP_A_VELOCITY_REQUEST				((OM_type)9084)
#define MAP_A_VERTICAL_ACCURACY				((OM_type)9085)
#define MAP_A_VERTICAL_COORDINATE_REQUEST		((OM_type)9086)

#if 0
/* Attributes used defined in <xmap_gsm.h> */
#define MAP_A_ADDITIONAL_NUMBER				((OM_type)9003)
#define MAP_A_AGE_OF_LOCATION_INFORMATION		((OM_type)9007)
#define MAP_A_EXTENSION					((OM_type)9020)
#define MAP_A_EXTENSION_CONTAINER			((OM_type)9021)
#define MAP_A_GPRS_NODE_INDICATOR			((OM_type)9023)
#define MAP_A_IMEI					((OM_type)9026)
#define MAP_A_IMSI					((OM_type)9027)
#define MAP_A_LCS_CLIENT_INTERNAL_ID			((OM_type)9033)
#define MAP_A_LCS_SERVICE_TYPE_ID			((OM_type)9046)
#define MAP_A_LMSI					((OM_type)9047)
#define MAP_A_MSISDN					((OM_type)9053)
#define MAP_A_NETWORK_NODE_NUMBER			((OM_type)9057)
#define MAP_A_SAI_PRESENT				((OM_type)9075)
#define MAP_A_SUPPORTED_LCS_CAPABILITY_SETS		((OM_type)9079)
#define MAP_A_V_GMLC_ADDRESS				((OM_type)9087)
#endif

/* OM enumerations (prefixed MAP_T_) */

/* Operation codes (MAP_T_Operation_Code): */
#define MAP_T_PROVIDE_SUBSCRIBER_LOCATION		83
#define MAP_T_SEND_ROUTING_INFO_FOR_LCS			85
#define MAP_T_SUBSCRIBER_LOCATION_REPORT		86

/* MAP_T_Area_Type: */
typedef enum {
	MAP_T_COUNTRY_CODE = 0,
	MAP_T_PLMN_ID = 1,
	MAP_T_LOCATION_AREA_ID = 2,
	MAP_T_ROUTING_AREA_ID = 3,
	MAP_T_CELL_GLOBAL_ID = 4,
	MAP_T_UTRAN_CELL_ID = 5,
} MAP_T_Area_Type;

/* MAP_T_Occurence_Info: */
typedef enum{
	MAP_T_ONE_TIME_EVENT = 0,
	MAP_T_MULTIPLE_TIME_EVENT = 1,
} MAP_T_Occurence_Info;

/* MAP_T_Deferred_Location_Event_Type: */
typedef enum {
	MAP_T_MS_AVAILABLE = 0,
	MAP_T_ENTERING_INTO_AREA = 1,
	MAP_T_LEAVING_FROM_AREA = 2,
	MAP_T_BEING_INSIDE_AREA = 3,
	MAP_T_PERIODIC_LDR = 4,
} MAP_T_Deferred_Location_Event_Type;

/* MAP_T_Termination_Cause: */
typedef enum {
	MAP_T_NORMAL = 0,
	MAP_T_ERROR_UNDEFINED = 1,
	MAP_T_INTERNAL_TIMEOUT = 2,
	MAP_T_CONGESTION = 3,
	MAP_T_MT_LR_RESTART = 4,
	MAP_T_PRIVACY_VIOLATION = 5,
	MAP_T_SHAPE_OF_LOCATION_ESTIMATE_NOT_SUPPORTED = 6,
	MAP_T_SUBSCRIBER_TERMINATION = 7,
	MAP_T_UE_TERMINATION = 8,
	MAP_T_NETWORK_TERMINATION = 9,
} MAP_T_Termination_Cause;

/* MAP_T_LCS_Client_Type: */
typedef enum {
	MAP_T_EMERGENCY_SERVICES = 0,
	MAP_T_VALUE_ADDED_SERVICES = 1,
	MAP_T_PLMN_OPERATOR_SERVICES = 2,
	MAP_T_LAWFUL_INTERCEPT_SERVICES = 3,
} MAP_T_LCS_Client_Type;

#if 0
/* MAP_T_LCS_Client_Internal_ID: */
typedef enum {
	MAP_T_BROADCAST_SERVICE = 0,
	MAP_T_O_AND_M_HPLMN = 1,
	MAP_T_O_AND_M_VPLMN = 2,
	MAP_T_ANONYMOUS_LOCATION = 3,
	MAP_T_TARGET_MS_SUBSCRIBED_SERVICE = 4,
} MAP_T_LCS_Client_Internal_ID;
#endif

/* MAP_T_LCS_Format_Indicator: */
typedef enum {
	MAP_T_LOGICAL_NAME = 0,
	MAP_T_E_MAIL_ADDRESS = 1,
	MAP_T_MSISDN = 2,
	MAP_T_URL = 3,
	MAP_T_SIP_URL = 4,
} MAP_T_LCS_Format_Indicator;

/* MAP_T_LCS_Capability_Sets: */
typedef enum {
	MAP_T_LCS_CAPABILITY_SET_1 = 0,
	MAP_T_LCS_CAPABILITY_SET_2 = 1,
	MAP_T_LCS_CAPABILITY_SET_3 = 2,
	MAP_T_LCS_CAPABILITY_SET_4 = 3,
	MAP_T_LCS_CAPABILITY_SET_5 = 4,
} MAP_T_LCS_Capability_Sets;

/* MAP_T_Privacy_Check_Related_Action: */
typedef enum {
	MAP_T_ALLOWED_WITHOUT_NOTIFICATION = 0,
	MAP_T_ALLOWED_WITH_NOTIFICATION = 1,
	MAP_T_ALLOWED_IF_NO_RESPONSE = 2,
	MAP_T_RESTRICTED_IF_NO_RESPONSE = 3,
	MAP_T_NOT_ALLOWED = 4,
} MAP_T_Privacy_Check_Related_Action;

/* MAP_T_Location_Estimate_Type: */
typedef enum {
	MAP_T_CURRENT_LOCATION = 0,
	MAP_T_CURRENT_OR_LAST_KNOWN_LOCATION = 1,
	MAP_T_INITIAL_LOCATION = 2,
	MAP_T_ACTIVATE_DEFERRED_LOCATION = 3,
	MAP_T_CANCEL_DEFERRED_LOCATION = 4,
	MAP_T_NOTIFICATION_VERIFICATION_ONLY = 5,
} MAP_T_Location_Estimate_Type;

/* MAP_T_Supported_GAD_Shapes: */
typedef enum {
	MAP_T_ELLIPSOID_POINT = 0,
	MAP_T_ELLIPSOID_POINT_WITH_UNCERTAINTY_CIRCLE = 1,
	MAP_T_ELLISPOID_POINT_WITH_UNCERTAINTY_ELLIPSE = 2,
	MAP_T_POLYGON = 3,
	MAP_T_ELLIPSOID_POINT_WITH_ALTITUDE = 4,
	MAP_T_ELLIPSOID_POINT_WITH_ALTITUDE_AND_UNCERTAINTY_ELLIPSOID = 5,
	MAP_T_ELLISPOID_ARC = 6,
} MAP_T_Supported_GAD_Shapes;

/* MAP_T_Accuracy_Fulfilment_Indicator: */
typedef enum {
	MAP_T_REQUESTED_ACCURACY_FULFILLED = 0,
	MAP_T_REQUESTED_ACCURACY_NOT_FULFILLED = 1,
} MAP_T_Accuracy_Fulfilment_Indicator;

/* MAP_T_RAN_Technology: */
typedef enum {
	MAP_T_GSM = 0,
	MAP_T_UMTS = 1,
} MAP_T_RAN_Technology;

/* MAP_T_Response_Time_Category: */
typedef enum {
	MAP_T_LOW_DELAY = 0,
	MAP_T_DELAY_TOLERANT = 1,
} MAP_T_Response_Time_Category;

/* MAP_T_LCS_Event: */
typedef enum {
	MAP_T_EMERGENCY_CALL_ORIGINATION = 0,
	MAP_T_EMERGENCY_CALL_RELEASE = 1,
	MAP_T_MO_LR = 2,
	MAP_T_DEFERRED_MT_LR_RESPONSE = 3,
	MAP_T_DEFERRED_MO_LR_TTTP_INITIATION = 4,
} MAP_T_LCS_Event;

#if 0
/*
 * SERVICE ERRORS
 */
/* Service-Error problem values (MAP_T_User_Error): */
#define MAP_E_POSITION_METHOD_FAILURE			54
#define MAP_E_UNAUTHORIZED_LCS_CLIENT			53
#define MAP_E_UNAUTHORIZED_REQUESTING_NETWORK		52
#define MAP_E_UNKNOWN_OR_UNREACHABLE_LCS_CLIENT		58

/* Service-Error parameter classes: */
#define OMP_O_MAP_C_POSITION_METHOD_FAILURE_PARAM		mapP_gsm_ls(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_UNAUTHORIZED_LCS_CLIENT_PARAM		mapP_gsm_ls(\x8F\x52)	/* 2002 */
#define OMP_O_MAP_C_UNAUTHORIZED_REQUESTING_NETWORK_PARAM	mapP_gsm_ls(\x8F\x53)	/* 2003 */
#define OMP_O_MAP_C_UNKNOWN_OR_UNREACHABLE_LCS_CLIENT_PARAM	mapP_gsm_ls(\x8F\x54)	/* 2004 */

/* Service-Error attributes: */
#define MAP_A_UNAUTHORIZED_LCS_CLIENT_DIAGNOSTIC	((OM_type)9101)
#define MAP_A_POSITION_METHOD_FAILURE_DIAGNOSTIC	((OM_type)9102)

/* Service-Error enumerations: */

/* MAP_T_Unauthorized_LCS_Client_Diagnostic: */
typedef enum {
	MAP_T_NO_ADDITIONAL_INFORMATION = 0,
	MAP_T_CLIENT_NOT_IN_MS_PRIVACY_EXCEPTION_LIST,
	MAP_T_CALL_TO_CLIENT_NOT_SETUP,
	MAP_T_PRIVACY_OVERRIDE_NOT_APPLICABLE,
	MAP_T_LOCAL_REGULATORY_REQUIREMENTS,
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
#endif

/* Uses:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_OR_LAI <xmap_gsm.h>
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 * OMP_O_MAP_C_LCS_CLIENT_EXTERNAL_ID <xmap_gsm.h>
 * OMP_O_MAP_C_SLR_ARG_EXTENSION_CONTAINER <xmap_gsm.h>
 * OMP_O_MAP_C_SUBSCRIBER_IDENTITY <xmap_gsm.h>
 *
 * OMP_O_MAP_C_ADDITIONAL_NUMBER <xmap_gsm_sm.h>
 *
 * OMP_O_MAP_C_AREA (this file)
 * OMP_O_MAP_C_AREA_DEFINITION (this file)
 * OMP_O_MAP_C_AREA_EVENT_INFO (this file)
 * OMP_O_MAP_C_DEFERRED_MT_LR_DATA (this file)
 * OMP_O_MAP_C_LCS_CLIENT_ID (this file)
 * OMP_O_MAP_C_LCS_CLIENT_NAME (this file)
 * OMP_O_MAP_C_LCS_CODEWORD (this file)
 * OMP_O_MAP_C_LCS_LOCATION_INFO (this file)
 * OMP_O_MAP_C_LCS_PRIVACY_CHECK (this file)
 * OMP_O_MAP_C_LCS_QOS (this file)
 * OMP_O_MAP_C_LCS_REQUESTOR_ID (this file)
 * OMP_O_MAP_C_LOCATION_TYPE (this file)
 * OMP_O_MAP_C_PERIODIC_LDR_INFO (this file)
 * OMP_O_MAP_C_PLMN_LIST (this file)
 * OMP_O_MAP_C_REPORTING_PLMN (this file)
 * OMP_O_MAP_C_REPORTING_PLMN_LIST (this file)
 * OMP_O_MAP_C_RESPONSE_TIME (this file)
 *
 * MAP_T_Accuracy_Fulfilment_Indicator (this file)
 * MAP_T_Area_Type (this file)
 * MAP_T_Deferred_Location_Event_Type (this file)
 * MAP_T_LCS_Capability_Sets (this file)
 * MAP_T_LCS_Client_Internal_ID (this file)
 * MAP_T_LCS_Client_Type (this file)
 * MAP_T_LCS_Event (this file)
 * MAP_T_LCS_Format_Indicator (this file)
 * MAP_T_Location_Estimate_Type (this file)
 * MAP_T_Occurence_Info (this file)
 * MAP_T_Privacy_Check_Related_Action (this file)
 * MAP_T_RAN_Technology (this file)
 * MAP_T_Response_Time_Category (this file)
 * MAP_T_Supported_GAD_Shapes (this file)
 * MAP_T_Termination_Cause
 */

/* Errors:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 *
 * MAP_T_Position_Method_Failure_Diagnostic (this file)
 * MAP_T_Unauthorized_LCS_Client_Diagnostic (this file)
 */


#endif				/* __XMAP_GSM_LS_H__ */
