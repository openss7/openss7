/*****************************************************************************

 @(#) $Id: xmap_gsm_ch.h,v 1.1.2.2 2010-11-28 14:21:44 brian Exp $

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

 Last Modified $Date: 2010-11-28 14:21:44 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm_ch.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:44  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:23:46  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __XMAP_GSM_CH_H__
#define __XMAP_GSM_CH_H__

#include <xom.h>
#include <xmap.h>
#include <xmap_gsm.h>

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-ch(3) }
 */
#define OMP_O_MAP_GSM_CH_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x03"

/* This application context is used between HLR and VLR for roaming number
 * enquiry procedures. */
#define OMP_O_MAP_ROAMING_NUMBER_ENQUIRY_CONTEXT		mapP_acId(\x03)
#define OMP_O_MAP_ROAMING_NUMBER_ENQUIRY_CONTEXT_V3		mapP_acId(\x03\x03)
#define OMP_O_MAP_ROAMING_NUMBER_ENQUIRY_CONTEXT_V2		mapP_acId(\x02\x02)
#define OMP_O_MAP_ROAMING_NUMBER_ENQUIRY_CONTEXT_V1		mapP_acId(\x01\x01)

/* This application context is used for the call control transfer procedure
 * between the VMSC and the GMSC. */
#define OMP_O_MAP_CALL_CONTROL_TRANSFER_CONTEXT			mapP_acId(\x06)
#define OMP_O_MAP_CALL_CONTROL_TRANSFER_CONTEXT_V4		mapP_acId(\x06\x04)
#define OMP_O_MAP_CALL_CONTROL_TRANSFER_CONTEXT_V3		mapP_acId(\x06\x03)

/* This application context is used betwen GMSC and HLR or between GMSC and NPLR
 * or between gsmSCF and HLR when retrieving location information.  For
 * GMSC-NPLR interface version 1, version 2 and version 3 of this application
 * context are applicable. */
#define OMP_O_MAP_LOCATION_INFO_RETRIEVAL_CONTEXT		mapP_acId(\x05)
#define OMP_O_MAP_LOCATION_INFO_RETRIEVAL_CONTEXT_V3		mapP_acId(\x05\x03)
#define OMP_O_MAP_LOCATION_INFO_RETRIEVAL_CONTEXT_V2		mapP_acId(\x05\x02)
#define OMP_O_MAP_LOCATION_INFO_RETRIEVAL_CONTEXT_V1		mapP_acId(\x05\x01)

/* This application context is used between HLR and VLR for reporting
 * procedures. */
#define OMP_O_MAP_REPORTING_CONTEXT				mapP_acId(\x07)
#define OMP_O_MAP_REPORTING_CONTEXT_V3				mapP_acId(\x07\x03)

/* This application context is used between MSC (Visited MSC or Gateway MSC) and
 * HLR for alerting services within IST procedures. */
#define OMP_O_MAP_IST_ALERTING_CONTEXT				mapP_acId(\x04)
#define OMP_O_MAP_IST_ALERTING_CONTEXT_V3			mapP_acId(\x04\x03)

/* This application context is used between GMSC and VMSC for resource
 * management purpose. */
#define OMP_O_MAP_RESOURCE_MANAGEMENT_CONTEXT			mapP_acId(\x2C)
#define OMP_O_MAP_RESOURCE_MANAGEMENT_CONTEXT_V3		mapP_acId(\x2C\x03)

/* OM class names (prefixed MAP_C_) */
/* call handling operations */
#define OMP_O_MAP_C_CALL_REPORT_DATA			mapP_gsm_ch(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_CAMEL_INFO				mapP_gsm_ch(\x87\x6A)	/* 1002 */
#define OMP_O_MAP_C_CAMEL_ROUTING_INFO			mapP_gsm_ch(\x87\x6B)	/* 1003 */
#define OMP_O_MAP_C_CCBS_INDICATORS			mapP_gsm_ch(\x87\x6C)	/* 1004 */
#define OMP_O_MAP_C_CUG_CHECK_INFO			mapP_gsm_ch(\x87\x6D)	/* 1005 */
#define OMP_O_MAP_C_EVENT_REPORT_DATA			mapP_gsm_ch(\x87\x6E)	/* 1006 */
#define OMP_O_MAP_C_EXTENDED_ROUTING_INFO		mapP_gsm_ch(\x87\x6F)	/* 1007 */
#define OMP_O_MAP_C_FORWARDING_DATA			mapP_gsm_ch(\x87\x70)	/* 1008 */
#define OMP_O_MAP_C_GMSC_CAMEL_SUBSCRIPTION_INFO	mapP_gsm_ch(\x87\x71)	/* 1009 */
#define OMP_O_MAP_C_IST_ALERT_ARG			mapP_gsm_ch(\x87\x72)	/* 1010 */
#define OMP_O_MAP_C_IST_ALERT_RES			mapP_gsm_ch(\x87\x73)	/* 1011 */
#define OMP_O_MAP_C_IST_COMMAND_ARG			mapP_gsm_ch(\x87\x74)	/* 1012 */
#define OMP_O_MAP_C_IST_COMMAND_RES			mapP_gsm_ch(\x87\x75)	/* 1013 */
#define OMP_O_MAP_C_PROVIDE_ROAMING_NUMBER_ARG		mapP_gsm_ch(\x87\x76)	/* 1014 */
#define OMP_O_MAP_C_PROVIDE_ROAMING_NUMBER_RES		mapP_gsm_ch(\x87\x77)	/* 1015 */
#define OMP_O_MAP_C_RELEASE_RESOURCES_ARG		mapP_gsm_ch(\x87\x78)	/* 1016 */
#define OMP_O_MAP_C_RELEASE_RESOURCES_RES		mapP_gsm_ch(\x87\x79)	/* 1017 */
#define OMP_O_MAP_C_REMOTE_USER_FREE_ARG		mapP_gsm_ch(\x87\x7A)	/* 1018 */
#define OMP_O_MAP_C_REMOTE_USER_FREE_RES		mapP_gsm_ch(\x87\x7B)	/* 1019 */
#define OMP_O_MAP_C_RESUME_CALL_HANDLING_ARG		mapP_gsm_ch(\x87\x7C)	/* 1020 */
#define OMP_O_MAP_C_RESUME_CALL_HANDLING_RES		mapP_gsm_ch(\x87\x7D)	/* 1021 */
#define OMP_O_MAP_C_ROUTING_INFO			mapP_gsm_ch(\x87\x7E)	/* 1022 */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_ARG		mapP_gsm_ch(\x87\x7F)	/* 1023 */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_RES		mapP_gsm_ch(\x88\x00)	/* 1024 */
#define OMP_O_MAP_C_SET_REPORTING_STATE_ARG		mapP_gsm_ch(\x88\x01)	/* 1025 */
#define OMP_O_MAP_C_SET_REPORTING_STATE_RES		mapP_gsm_ch(\x88\x02)	/* 1026 */
#define OMP_O_MAP_C_STATUS_REPORT_ARG			mapP_gsm_ch(\x88\x03)	/* 1027 */
#define OMP_O_MAP_C_STATUS_REPORT_RES			mapP_gsm_ch(\x88\x04)	/* 1028 */
#define OMP_O_MAP_C_UU_DATA				mapP_gsm_ch(\x88\x05)	/* 1029 */

/* OM attribute names (prefixed MAP_) */
#define MAP_A_ADDITIONAL_SIGNAL_INFO				((OM_type)5001)
#define MAP_A_ALLOWED_SERVICES					((OM_type)5004)
#define MAP_A_BASIC_SERVICE_2					((OM_type)5006)
#define MAP_A_BASIC_SERVICE_GROUP_2				((OM_type)5008)
#define MAP_A_CALL_DIVERSION_TREATMENT_INDICATOR		((OM_type)5009)
#define MAP_A_CALL_OUTCOME					((OM_type)5011)
#define MAP_A_CALL_REFERENCE_NUMBER				((OM_type)5013)
#define MAP_A_CALL_REPORT_DATA					((OM_type)5014)
#define MAP_A_CALL_TERMINATION_INDICATOR			((OM_type)5015)
#define MAP_A_CAMEL_INFO					((OM_type)5016)
#define MAP_A_CAMEL_ROUTING_INFO				((OM_type)5017)
#define MAP_A_CCBS_CALL						((OM_type)5018)
#define MAP_A_CCBS_INDICATORS					((OM_type)5020)
#define MAP_A_CCBS_MONITORING					((OM_type)5021)
#define MAP_A_CCBS_SUBSCRIBER_STATUS				((OM_type)5023)
#define MAP_A_CUG_CHECK_INFO					((OM_type)5024)
#define MAP_A_CUG_OUTGOING_ACCESS				((OM_type)5026)
#define MAP_A_CUG_SUBSCRIPTION_FLAG				((OM_type)5027)
#define MAP_A_EVENT_REPORT_DATA					((OM_type)5029)
#define MAP_A_EXTENDED_ROUTING_INFO				((OM_type)5030)
#define MAP_A_FORWARDING_DATA					((OM_type)5035)
#define MAP_A_FORWARDING_INTERROGATION_REQUIRED			((OM_type)5036)
#define MAP_A_FORWARDING_REASON					((OM_type)5038)
#define MAP_A_GMSC_ADDRESS					((OM_type)5039)
#define MAP_A_GMSC_CAMEL_SUBSCRIPTION_INFO			((OM_type)5040)
#define MAP_A_GMSC_OR_GSMSCF_ADDRESS				((OM_type)5041)
#define MAP_A_GSM_BEARER_CAPABILITY				((OM_type)5042)
#define MAP_A_GSMSCF_INITIATED_CALL				((OM_type)5043)
#define MAP_A_INTERROGATION_TYPE				((OM_type)5045)
#define MAP_A_KEEP_CCBS_CALL_INDICATOR				((OM_type)5049)
#define MAP_A_MONITORING_MODE					((OM_type)5053)
#define MAP_A_MSRN						((OM_type)5056)
#define MAP_A_MT_ROAMING_RETRY					((OM_type)5057)
#define MAP_A_MT_ROAMING_RETRY_SUPPORTED			((OM_type)5058)
#define MAP_A_NETWORK_SIGNAL_INFO_2				((OM_type)5061)
#define MAP_A_NUMBER_OF_FORWARDING				((OM_type)5062)
#define MAP_A_OFFERED_CAMEL4_CSIS_IN_INTERROGATING_NODE		((OM_type)5067)
#define MAP_A_OFFERED_CAMEL4_CSIS_IN_VMSC			((OM_type)5068)
#define MAP_A_OR_CAPABILITY					((OM_type)5069)
#define MAP_A_OR_INTERROGATION					((OM_type)5070)
#define MAP_A_OR_NOT_SUPPORTED_IN_GMSC				((OM_type)5071)
#define MAP_A_PRE_PAGING_SUPPORTED				((OM_type)5073)
#define MAP_A_RELEASE_RESOURCES_SUPPORTED			((OM_type)5074)
#define MAP_A_REPLACE_B_NUMBER					((OM_type)5075)
#define MAP_A_ROAMING_NUMBER					((OM_type)5076)
#define MAP_A_ROUTING_INFO					((OM_type)5077)
#define MAP_A_ROUTING_INFO_2					((OM_type)5078)
#define MAP_A_RUF_OUTCOME					((OM_type)5079)
#define MAP_A_SS_LIST_2						((OM_type)5081)
#define MAP_A_SUPPORTED_CAMEL_PHASES_IN_INTERROGATING_NODE	((OM_type)5084)
#define MAP_A_SUPPORTED_CAMEL_PHASES_IN_VMSC			((OM_type)5085)
#define MAP_A_SUPPORTED_CCBS_PHASE				((OM_type)5086)
#define MAP_A_SUPPRESS_INCOMING_CALL_BARRING			((OM_type)5087)
#define MAP_A_SUPPRESSION_OF_ANNOUNCEMENT			((OM_type)5088)
#define MAP_A_SUPPRESS_MT_SS					((OM_type)5089)
#define MAP_A_SUPPRESS_T_CSI					((OM_type)5090)
#define MAP_A_SUPPRESS_VT_CSI					((OM_type)5091)
#define MAP_A_UNAVAILABILITY_CAUSE				((OM_type)5095)
#define MAP_A_UU_DATA						((OM_type)5096)
#define MAP_A_UUI						((OM_type)5097)
#define MAP_A_UU_INDICATOR					((OM_type)5098)
#define MAP_A_UUSCF_INTERACTION					((OM_type)5099)
#define MAP_A_VMSC_ADDRESS					((OM_type)5100)

#if 0
/* Attributes used defined in <xmap_gsm.h> */
#define MAP_A_ALERTING_PATTERN					((OM_type)5002)
#define MAP_A_ALL_INFORMATION_SENT				((OM_type)5003)
#define MAP_A_BASIC_SERVICE					((OM_type)5005)
#define MAP_A_BASIC_SERVICE_GROUP				((OM_type)5007)
#define MAP_A_CALL_INFO						((OM_type)5010)
#define MAP_A_CALL_PRIORITY					((OM_type)5012)
#define MAP_A_CCBS_FEATURE					((OM_type)5019)
#define MAP_A_CCBS_POSSIBLE					((OM_type)5022)
#define MAP_A_CUG_INTERLOCK					((OM_type)5025)
#define MAP_A_D_CSI						((OM_type)5028)
#define MAP_A_EXTENSION						((OM_type)5031)
#define MAP_A_EXTENSION_CONTAINER				((OM_type)5032)
#define MAP_A_FORWARDED_TO_NUMBER				((OM_type)5033)
#define MAP_A_FORWARDED_TO_SUBADDRESS				((OM_type)5034)
#define MAP_A_FORWARDING_OPTIONS				((OM_type)5037)
#define MAP_A_IMSI						((OM_type)5044)
#define MAP_A_IST_ALERT_TIMER					((OM_type)5046)
#define MAP_A_IST_INFORMATION_WITHDRAW				((OM_type)5047)
#define MAP_A_IST_SUPPORT_INDICATOR				((OM_type)5048)
#define MAP_A_LMSI						((OM_type)5050)
#define MAP_A_LONG_FORWARDED_TO_NUMBER				((OM_type)5051)
#define MAP_A_LONG_FTN_SUPPORTED				((OM_type)5052)
#define MAP_A_MSC_NUMBER					((OM_type)5054)
#define MAP_A_NAEA_PREFERRED_CI					((OM_type)5059)
#define MAP_A_NUMBER_PORTABILITY_STATUS				((OM_type)5063)
#define MAP_A_O_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type)5064)
#define MAP_A_O_CSI						((OM_type)5065)
#define MAP_A_OFFERED_CAMEL4_CSIS				((OM_type)5066)
#define MAP_A_NETWORK_SIGNAL_INFO				((OM_type)5060)
#define MAP_A_MSISDN						((OM_type)5055)
#define MAP_A_PAGING_AREA					((OM_type)5072)
#define MAP_A_SS_LIST						((OM_type)5080)
#define MAP_A_SUBSCRIBER_INFO					((OM_type)5082)
#define MAP_A_SUPPORTED_CAMEL_PHASES				((OM_type)5083)
#define MAP_A_T_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type)5092)
#define MAP_A_T_CSI						((OM_type)5093)
#define MAP_A_TRANSLATED_B_NUMBER				((OM_type)5094)
#endif

/* OM enumerations (prefixed MAP_T_) */

/* MAP_T_Monitoring_Mode: */
typedef enum {
	MAP_T_A_SIDE = 0,
	MAP_T_B_SIDE = 1,
} MAP_T_Monitoring_Mode;

/* MAP_T_Call_Outcome: */
typedef enum {
	MAP_T_SUCCESS = 0,
	MAP_T_FAILURE = 1,
	MAP_T_BUSY = 2,
} MAP_T_Call_Outcome;

#if 0
/* MAP_T_Supported_CAMEL_Phases: */
typedef enum {
	MAP_T_PHASE_1 = 0,
	MAP_T_PHASE_2 = 1,
	MAP_T_PHASE_3 = 2,
	MAP_T_PHASE_4 = 3,
} MAP_T_Supported_CAMEL_Phases;
#endif

#if 0
/* MAP_T_Offered_CAMEL4_CSIs: */
typedef enum {
	MAP_T_O_CSI = 0,
	MAP_T_D_CSI = 1,
	MAP_T_VT_CSI = 2,
	MAP_T_T_CSI = 3,
	MAP_T_MT_SMS_CSI = 4,
	MAP_T_MG_CSI = 5,
	MAP_T_PSI_ENHANCEMENTS = 6,
} MAP_T_Offered_CAMEL4_CSIs;
#endif

/* MAP_T_CCBS_Subscriber_Status: */
typedef enum {
	MAP_T_CCBS_NOT_IDLE = 0,
	MAP_T_CCBS_IDLE = 1,
	MAP_T_CCBS_NOT_REACHABLE = 2,
} MAP_T_CCBS_Subscriber_Status;

/* MAP_T_Call_Termination_Indicator: */
typedef enum {
	MAP_T_TERMINATE_CALL_ACTIVITY_REFERRED = 0,
	MAP_T_TERMINATE_ALL_CALL_ACTIVITIES = 1,
} MAP_T_Call_Termination_Indicator;

/* MAP_T_RUF_Outcome: */
typedef enum {
	MAP_T_ACCEPTED = 0,
	MAP_T_REJECTED = 1,
	MAP_T_NO_RESPONSE_FROM_FREE_MS = 2,
	MAP_T_NO_RESPONSE_FROM_BUSY_MS = 3,
	MAP_T_UDUB_FROM_FREE_MS = 4,
	MAP_T_UDUB_FROM_BUSY_MS = 5,
} MAP_T_RUF_Outcome;

/* MAP_T_Interrogation_Type: */
typedef enum {
	MAP_T_BASIC_CALL = 0,
	MAP_T_FORWARDING = 2,
} MAP_T_Interrogation_Type;

/* MAP_T_Forwarding_Reason: */
typedef enum {
	MAP_T_NOT_REACHABLE = 0,
	MAP_T_BUSY = 1,
	MAP_T_NO_REPLY = 2,
} MAP_T_Forwarding_Reason;

#if 0
/* MAP_T_IST_Support_Indicator: */
typedef enum {
	MAP_T_BASIC_IST_SUPPORTED = 0,
	MAP_T_IST_COMMAND_SUPPORTED = 1,
} MAP_T_IST_Support_Indicator;
#endif

/* MAP_T_Suppress_MT_SS: */
typedef enum {
	MAP_T_SUPPRESS_CUG = 0,
	MAP_T_SUPPRESS_CCBS = 1,
} MAP_T_Suppress_MT_SS;

#if 0
/* MAP_T_Number_Portability_Status: */
typedef enum {
	MAP_T_NOT_KNOWN_TO_BE_PORTED = 0,
	MAP_T_OWN_NUMBER_PORTED_OUT = 1,
	MAP_T_FOREIGN_NUMBER_PORTED_TO_FOREIGN_NETWORK = 2,
	MAP_T_OWN_NUMBER_NOT_PORTED_OUT = 4,
	MAP_T_FOREIGN_NUMBER_PORTED_IN = 5,
} MAP_T_Number_Portability_Status;
#endif

/* MAP_T_Unavailability_Cause: */
typedef enum {
	MAP_T_BEARER_SERVICE_NOT_PROVISIONED = 1,
	MAP_T_TELESERVICE_NOT_PROVISIONED = 2,
	MAP_T_ABSENT_SUBSCRIBER = 3,
	MAP_T_BUSY_SUBSCRIBER = 4,
	MAP_T_CALL_BARRED = 5,
	MAP_T_CUG_REJECT = 6,
} MAP_T_Unavailability_Cause;

/* MAP_T_Reporting_State: */
typedef enum {
	MAP_T_STOP_MONITORING = 0,
	MAP_T_START_MONITORING = 1,
} MAP_T_Reporting_State;

/* Operation codes (MAP_T_Operation_Code): */
/* call handling operations */
#define MAP_T_SEND_ROUTING_INFO				22
#define MAP_T_PROVIDE_ROAMING_NUMBER			 4
#define MAP_T_RESUME_CALL_HANDLING			 6
#define MAP_T_SET_REPORTING_STATE			73
#define MAP_T_STATUS_REPORT				74
#define MAP_T_REMOTE_USER_FREE				75
#define MAP_T_IST_ALERT					87
#define MAP_T_IST_COMMAND				88
#define MAP_T_RELEASE_RESOURCES				20

#if 0
/*
 * SERVICE ERRORS
 */

/* Service-Error problem values (MAP_T_User_Error): */
/* call handling errors */
#define MAP_E_ABSENT_SUBSCRIBER				27
#define MAP_E_BUSY_SUBSCRIBER				45
#define MAP_E_CALL_BARRED				13
#define MAP_E_CUG_REJECT				15
#define MAP_E_FORWARDING_FAILED				47
#define MAP_E_FORWARDING_VIOLATION			14
#define MAP_E_NO_ROAMING_NUMBER_AVAILABLE		39
#define MAP_E_NO_SUBSCRIBER_REPLY			46
#define MAP_E_OR_NOT_ALLOWED				48

/* Service-Error parameter classes: */
/* call handling errors */
#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_PARAM		mapP_gsm_ch(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_BUSY_SUBSCRIBER_PARAM		mapP_gsm_ch(\x8F\x52)	/* 2002 */
#define OMP_O_MAP_C_CALL_BARRED_PARAM			mapP_gsm_ch(\x8F\x53)	/* 2003 */
#define OMP_O_MAP_C_CUG_REJECT_PARAM			mapP_gsm_ch(\x8F\x54)	/* 2004 */
#define OMP_O_MAP_C_FORWARDING_FAILED_PARAM		mapP_gsm_ch(\x8F\x55)	/* 2005 */
#define OMP_O_MAP_C_FORWARDING_VIOLATION_PARAM		mapP_gsm_ch(\x8F\x56)	/* 2006 */
#define OMP_O_MAP_C_NO_ROAMING_NB_PARAM			mapP_gsm_ch(\x8F\x57)	/* 2007 */
#define OMP_O_MAP_C_NO_SUBSCRIBER_REPLY_PARAM		mapP_gsm_ch(\x8F\x58)	/* 2008 */
#define OMP_O_MAP_C_OR_NOT_ALLOWED_PARAM		mapP_gsm_ch(\x8F\x59)	/* 2009 */

/* Service-Error attributes: */
/* call handling errors */
#define MAP_A_ABSENT_SUBSCRIBER_REASON			((OM_type)5101)
#define MAP_A_CALL_BARRING_CAUSE			((OM_type)5102)
#define MAP_A_CCBS_BUSY					((OM_type)5103)
#define MAP_A_CCBS_POSSIBLE				((OM_type)5104)
#define MAP_A_CUG_REJECT_CAUSE				((OM_type)5105)
#define MAP_A_UNAUTHORIZED_MESSAGE_ORIGINATOR		((OM_type)5106)

/* Service-Error enumerations: */
/* call handling errors */

/* MAP_T_Absent_Subscriber_Reason: */
typedef enum {
	MAP_T_IMSI_DETACH = 0,
	MAP_T_RESTRICTED_AREA = 1,
	MAP_T_NO_PAGE_RESPONSE = 2,
	MAP_T_MS_PURGED = 3,
} MAP_T_Absent_Subscriber_Reason;

/* MAP_T_Call_Barring_Cause: */
typedef enum {
	MAP_T_BARRING_SERVICE_ACTIVE = 0,
	MAP_T_OPERATOR_BARRING = 1,
} MAP_T_Call_Barring_Cause;

/* MAP_T_CUG_Reject_Cause: */
typedef enum {
	MAP_T_INCOMING_CALLS_BARRED_WITHIN_CUG = 0,
	MAP_T_SUBSCRIBER_NOT_MEMBER_OF_CUG = 1,
	MAP_T_REQUESTED_BASIC_SERVICE_VIOLATES_CUG_CONSTRAINTS = 5,
	MAP_T_CALLED_PARTY_SS_INTERACTION_VIOLATION = 7,
} MAP_T_CUG_Reject_Cause;
#endif

/* Uses:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_EXT_BASIC_SERVICE_CODE <xmap_gsm.h>
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 * OMP_O_MAP_C_EXTERNAL_SIGNAL_INFO <xmap_gsm.h>
 * OMP_O_MAP_C_EXT_EXTERNAL_SIGNAL_INFO <xmap_gsm.h>
 * OMP_O_MAP_C_NAEA_PREFERRED_CI <xmap_gsm.h>
 *
 * OMP_O_MAP_C_CCBS_FEATURE <xmap_gsm_ss.h>
 * OMP_O_MAP_C_SS_LIST <xmap_gsm_ss.h>
 *
 * OMP_O_MAP_C_D_CSI <xmap_gsm_ms.h>
 * OMP_O_MAP_C_O_BCSM_CAMEL_TDP_CRITERIA_LIST <xmap_gsm_ms.h>
 * OMP_O_MAP_C_O_CSI <xmap_gsm_ms.h>
 * OMP_O_MAP_C_PAGING_AREA <xmap_gsm_ms.h>
 * OMP_O_MAP_C_SUBSCRIBER_INFO <xmap_gsm_ms.h>
 * OMP_O_MAP_C_T_BCSM_CAMEL_TDP_CRITERIA_LIST <xmap_gsm_ms.h>
 * OMP_O_MAP_C_T_CSI <xmap_gsm_ms.h>
 *
 * OMP_O_MAP_C_CALL_REPORT_DATA (this file)
 * OMP_O_MAP_C_CAMEL_INFO (this file)
 * OMP_O_MAP_C_CAMEL_ROUTING_INFO (this file)
 * OMP_O_MAP_C_CCBS_INDICATORS (this file)
 * OMP_O_MAP_C_CUG_CHECK_INFO (this file)
 * OMP_O_MAP_C_EVENT_REPORT_DATA (this file)
 * OMP_O_MAP_C_EXTENDED_ROUTING_INFO (this file)
 * OMP_O_MAP_C_FORWARDING_DATA (this file)
 * OMP_O_MAP_C_GMSC_CAMEL_SUBSCRIPTION_INFO (this file)
 * OMP_O_MAP_C_ROUTING_INFO (this file)
 * OMP_O_MAP_C_UU_DATA (this file)
 *
 * MAP_T_Call_Outcome
 * MAP_T_Call_Termination_Indicator
 * MAP_T_CCBS_Subscriber_Status
 * MAP_T_Forwarding_Reason
 * MAP_T_Interrogation_Type
 * MAP_T_IST_Support_Indicator
 * MAP_T_Monitoring_Mode
 * MAP_T_Number_Portability_Status
 * MAP_T_Offered_CAMEL4_CSIs
 * MAP_T_Reporting_State
 * MAP_T_RUF_Outcome
 * MAP_T_Supported_CAMEL_Phases
 * MAP_T_Suppress_MT_SS
 * MAP_T_Unavailability_Cause
 *
 */

/* Errors:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_EXT_BASIC_SERVICE_CODE <xmap_gsm.h>
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 *
 * OMP_O_MAP_C_EXTENSIBLE_CALL_BARRED_PARAM
 *
 * MAP_T_Absent_Subscriber_Reason (this file)
 * MAP_T_Call_Barring_Cause (this file)
 * MAP_T_CUG_Reject_Cause (this file)
 */

#endif				/* __XMAP_GSM_CH_H__ */
