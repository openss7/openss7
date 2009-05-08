/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __XMAP_GSM_SS_H__
#define __XMAP_GSM_SS_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2008-2009 Monavacon Limited."

#include <xom.h>
#include <xmap.h>
#include <xmap_gsm.h>

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-ss(4) }
 */
#define OMP_O_MAP_GSM_SS_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x04"

/* Intermediate object identifier macro: */
#define mapP_gsm_ss(X) (OMP_O_MAP_GSM_SS_PKG# #X)

/* This application context is used for functional-like SS handling procedures
 * between VLR and HLR. */
#define OMP_O_MAP_NETWORK_FUNCTIONAL_SS_CONTEXT			mapP_acId(\x12)
#define OMP_O_MAP_NETWORK_FUNCTIONAL_SS_CONTEXT_V2		mapP_acId(\x12\x02)
#define OMP_O_MAP_NETWORK_FUNCTIONAL_SS_CONTEXT_V1		mapP_acId(\x12\x01)

/* This application context is used for handling stimulii-like procedures
 * between HLR and VLR and gsmSCF, and between HLR and VLR. */
#define OMP_O_MAP_NETWORK_UNSTRUCTURED_SS_CONTEXT		mapP_acId(\x13)
#define OMP_O_MAP_NETWORK_UNSTRUCTURED_SS_CONTEXT_V2		mapP_acId(\x13\x02)
#define OMP_O_MAP_NETWORK_UNSTRUCTURED_SS_CONTEXT_V1		mapP_acId(\x13\x01)

/* This application context is used between the MSC and the gsmSCF and between
 * the HLR and the gsmSCF Suplementary Service invocation notification
 * procedures. */
#define OMP_O_MAP_SS_INVOCATION_NOTIFICATION_CONTEXT		mapP_acId(\x24)
#define OMP_O_MAP_SS_INVOCATION_NOTIFICATION_CONTEXT_V3		mapP_acId(\x24\x03)

/* This application context is used between VLR and the HLR for subscriber
 * control of call completion services. */
#define OMP_O_MAP_CALL_COMPLETION_CONTEXT			mapP_acId(\x07)
#define OMP_O_MAP_CALL_COMPLETION_CONTEXT_V3			mapP_acId(\x07\x03)

/* OM class names (prefixed MAP_C_) */
/* supplementary service handling operations */
#define OMP_O_MAP_C_ACTIVATE_SS_ARG			mapP_gsm_sm(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ACTIVATE_SS_RES			mapP_gsm_sm(\x87\x6A)	/* 1002 */
#define OMP_O_MAP_C_BASIC_SERVICE_GROUP_LIST		mapP_gsm_sm(\x87\x6B)	/* 1003 */
#define OMP_O_MAP_C_CALL_BARRING_FEATURE		mapP_gsm_sm(\x87\x6C)	/* 1004 */
#define OMP_O_MAP_C_CALL_BARRING_FEATURE_LIST		mapP_gsm_sm(\x87\x6D)	/* 1005 */
#define OMP_O_MAP_C_CALL_BARRING_INFO			mapP_gsm_sm(\x87\x6E)	/* 1006 */
#define OMP_O_MAP_C_CCBS_DATA				mapP_gsm_sm(\x87\x6F)	/* 1007 */
#define OMP_O_MAP_C_CCBS_FEATURE			mapP_gsm_sm(\x87\x70)	/* 1008 */
#define OMP_O_MAP_C_CCBS_FEATURE_LIST			mapP_gsm_sm(\x87\x71)	/* 1009 */
#define OMP_O_MAP_C_DEACTIVATE_SS_ARG			mapP_gsm_sm(\x87\x72)	/* 1010 */
#define OMP_O_MAP_C_DEACTIVATE_SS_RES			mapP_gsm_sm(\x87\x73)	/* 1011 */
#define OMP_O_MAP_C_ERASE_CC_ENTRY_ARG			mapP_gsm_sm(\x87\x74)	/* 1012 */
#define OMP_O_MAP_C_ERASE_CC_ENTRY_RES			mapP_gsm_sm(\x87\x75)	/* 1013 */
#define OMP_O_MAP_C_ERASE_SS_ARG			mapP_gsm_sm(\x87\x76)	/* 1014 */
#define OMP_O_MAP_C_ERASE_SS_RES			mapP_gsm_sm(\x87\x77)	/* 1015 */
#define OMP_O_MAP_C_FORWARDING_FEATURE			mapP_gsm_sm(\x87\x78)	/* 1016 */
#define OMP_O_MAP_C_FORWARDING_FEATURE_LIST		mapP_gsm_sm(\x87\x79)	/* 1017 */
#define OMP_O_MAP_C_FORWARDING_INFO			mapP_gsm_sm(\x87\x7A)	/* 1018 */
#define OMP_O_MAP_C_GENERIC_SERVICE_INFO		mapP_gsm_sm(\x87\x7B)	/* 1019 */
#define OMP_O_MAP_C_GET_PASSWORD_ARG			mapP_gsm_sm(\x87\x7C)	/* 1020 */
#define OMP_O_MAP_C_GET_PASSWORD_RES			mapP_gsm_sm(\x87\x7D)	/* 1021 */
#define OMP_O_MAP_C_INTERROGATE_SS_ARG			mapP_gsm_sm(\x87\x7E)	/* 1022 */
#define OMP_O_MAP_C_INTERROGATE_SS_RES			mapP_gsm_sm(\x88\x0F)	/* 1023 */
#define OMP_O_MAP_C_NOTIFY_USSD_ARG			mapP_gsm_sm(\x88\x00)	/* 1024 */
#define OMP_O_MAP_C_NOTIFY_USSD_RES			mapP_gsm_sm(\x88\x01)	/* 1025 */
#define OMP_O_MAP_C_PROCESS_USSD_ARG			mapP_gsm_sm(\x88\x02)	/* 1026 */
#define OMP_O_MAP_C_PROCESS_USSD_RES			mapP_gsm_sm(\x88\x03)	/* 1027 */
#define OMP_O_MAP_C_REGISTER_CC_ENTRY_ARG		mapP_gsm_sm(\x88\x04)	/* 1028 */
#define OMP_O_MAP_C_REGISTER_CC_ENTRY_RES		mapP_gsm_sm(\x88\x05)	/* 1029 */
#define OMP_O_MAP_C_REGISTER_PASSWORD_ARG		mapP_gsm_sm(\x88\x06)	/* 1030 */
#define OMP_O_MAP_C_REGISTER_PASSWORD_RES		mapP_gsm_sm(\x88\x07)	/* 1031 */
#define OMP_O_MAP_C_REGISTER_SS_ARG			mapP_gsm_sm(\x88\x08)	/* 1032 */
#define OMP_O_MAP_C_REGISTER_SS_RES			mapP_gsm_sm(\x88\x09)	/* 1033 */
#define OMP_O_MAP_C_SS_DATA				mapP_gsm_sm(\x88\x0A)	/* 1034 */
#define OMP_O_MAP_C_SS_EVENT_SPECIFICATION		mapP_gsm_sm(\x88\x0B)	/* 1035 */
#define OMP_O_MAP_C_SS_FOR_BS_CODE			mapP_gsm_sm(\x88\x0C)	/* 1036 */
#define OMP_O_MAP_C_SS_INFO				mapP_gsm_sm(\x88\x0D)	/* 1037 */
#define OMP_O_MAP_C_SS_INVOCATION_NOTIFICATION_ARG	mapP_gsm_sm(\x88\x0E)	/* 1038 */
#define OMP_O_MAP_C_SS_INVOCATION_NOTIFICATION_RES	mapP_gsm_sm(\x88\x0F)	/* 1039 */
#define OMP_O_MAP_C_SS_LIST				mapP_gsm_sm(\x88\x10)	/* 1040 */
#define OMP_O_MAP_C_SS_SUBSCRIPTION_OPTION		mapP_gsm_sm(\x88\x11)	/* 1041 */
#define OMP_O_MAP_C_USSD_ARG				mapP_gsm_sm(\x88\x12)	/* 1042 */
#define OMP_O_MAP_C_USSD_RES				mapP_gsm_sm(\x88\x13)	/* 1043 */

/* OM attribute names (prefixed MAP_) */
#define MAP_A_BASIC_SERVICE_CODE			((OM_type)6003)
#define MAP_A_B_SUBSCRIBER_NUMBER			((OM_type)6006)
#define MAP_A_B_SUBSCRIBER_SUBADDRESS			((OM_type)6007)
#define MAP_A_CALL_BARRING_FEATURE			((OM_type)6008)
#define MAP_A_CCBS_DATA					((OM_type)6012)
#define MAP_A_CCBS_FEATURE_LIST				((OM_type)6014)
#define MAP_A_CCBS_INDEX				((OM_type)6015)
#define MAP_A_CCBS_REQUEST_STATE			((OM_type)6016)
#define MAP_A_CLI_RESTRICTION_OPTION			((OM_type)6017)
#define MAP_A_DEFAULT_PRIORITY				((OM_type)6018)
#define MAP_A_FORWARDING_FEATURE			((OM_type)6023)
#define MAP_A_GENERIC_SERVICE_INFO			((OM_type)6027)
#define MAP_A_MAXIMUM_ENTITLEMENT_PRIORITY		((OM_type)6031)
#define MAP_A_NBR_SN					((OM_type)6034)
#define MAP_A_OVERRIDE_CATEGORY				((OM_type)6038)
#define MAP_A_SERVICE_INDICATOR				((OM_type)6040)
#define MAP_A_SS_EVENT					((OM_type)6043)
#define MAP_A_SS_EVENT_SPEC				((OM_type)6044)
#define MAP_A_SS_EVENT_SPECIFICATION			((OM_type)6045)
#define MAP_A_USSD_DATA_CODING_SCHEME			((OM_type)6049)
#define MAP_A_USSD_STRING				((OM_type)6050)

#if 0
/* Attributes used defined in <xmap_gsm.h> */
#define MAP_A_ALERTING_PATTERN				((OM_type)6001)
#define MAP_A_BASIC_SERVICE				((OM_type)6002)
#define MAP_A_BASIC_SERVICE_GROUP			((OM_type)6004)
#define MAP_A_BASIC_SERVICE_GROUP_LIST			((OM_type)6005)
#define MAP_A_CALL_BARRING_FEATURE_LIST			((OM_type)6009)
#define MAP_A_CALL_BARRING_INFO				((OM_type)6010)
#define MAP_A_CALL_INFO					((OM_type)6011)
#define MAP_A_CCBS_FEATURE				((OM_type)6013)
#define MAP_A_EXTENSION					((OM_type)6019)
#define MAP_A_EXTENSION_CONTAINER			((OM_type)6020)
#define MAP_A_FORWARDED_TO_NUMBER			((OM_type)6021)
#define MAP_A_FORWARDED_TO_SUBADDRESS			((OM_type)6022)
#define MAP_A_FORWARDING_FEATURE_LIST			((OM_type)6024)
#define MAP_A_FORWARDING_INFO				((OM_type)6025)
#define MAP_A_FORWARDING_OPTIONS			((OM_type)6026)
#define MAP_A_IMSI					((OM_type)6028)
#define MAP_A_LONG_FORWARDED_TO_NUMBER			((OM_type)6029)
#define MAP_A_LONG_FTN_SUPPORTED			((OM_type)6030)
#define MAP_A_MSISDN					((OM_type)6032)
#define MAP_A_NBR_SB					((OM_type)6033)
#define MAP_A_NBR_USER					((OM_type)6035)
#define MAP_A_NETWORK_SIGNAL_INFO			((OM_type)6036)
#define MAP_A_NO_REPLY_CONDITION_TIME			((OM_type)6037)
#define MAP_A_PASSWORD					((OM_type)6039)
#define MAP_A_SS_CODE					((OM_type)6041)
#define MAP_A_SS_DATA					((OM_type)6042)
#define MAP_A_SS_STATUS					((OM_type)6046)
#define MAP_A_SS_SUBSCRIPTION_OPTION			((OM_type)6047)
#define MAP_A_TRANSLATED_B_NUMBER			((OM_type)6048)
#endif

/* OM enumerations (prefixed MAP_T_) */

/* MAP_T_Service_Indicator: */
typedef enum {
	MAP_T_CLIR_INVOKED = 0,
	MAP_T_CAMEL_INVOKED = 1,
} MAP_T_Service_Indicator;

/* MAP_T_Cli_Restriction_Option: */
typedef enum {
	MAP_T_PERMANENT = 0,
	MAP_T_TEMPORARY_DEFAULT_RESTRICTED = 1,
	MAP_T_TEMPORARY_DEFAULT_ALLOWED = 2,
} MAP_T_Cli_Restriction_Option;

/* MAP_T_Alerting_Pattern: */
typedef enum {
	MAP_T_ALERTING_LEVEL_0 = 0x00,
	MAP_T_ALERTING_LEVEL_1 = 0x01,
	MAP_T_ALERTING_LEVEL_2 = 0x02,
	MAP_T_ALERTING_CATEGORY_1 = 0x04,
	MAP_T_ALERTING_CATEGORY_2 = 0x05,
	MAP_T_ALERTING_CATEGORY_3 = 0x06,
	MAP_T_ALERTING_CATEGORY_4 = 0x07,
	MAP_T_ALERTING_CATEGORY_5 = 0x08,
} MAP_T_Alerting_Pattern;

/* MAP_T_CCBS_Request_State: */
typedef enum {
	MAP_T_REQUEST = 0,
	MAP_T_RECALL = 1,
	MAP_T_ACTIVE = 2,
	MAP_T_COMPLETED = 3,
	MAP_T_SUSPENDED = 4,
	MAP_T_FROZEN = 5,
	MAP_T_DELETED = 6,
} MAP_T_CCBS_Request_State;

/* MAP_T_Override_Category: */
typedef enum {
	MAP_T_OVERRIDE_ENABLED = 0,
	MAP_T_OVERRIDE_DISABLED = 1,
} MAP_T_Override_Category;

/* Operation codes (MAP_T_Operation_Code): */
/* supplementary service handling operations */
#define MAP_T_REGISTER_SS				10
#define MAP_T_ERASE_SS					11
#define MAP_T_ACTIVATE_SS				12
#define MAP_T_DEACTIVATE_SS				13
#define MAP_T_INTERROGATE_SS				14
#define MAP_T_PROCESS_UNSTRUCTURED_SS_REQUEST		59
#define MAP_T_UNSTRUCTURED_SS_REQUEST			60
#define MAP_T_UNSTRUCTURED_SS_NOTIFY			61
#define MAP_T_REGISTER_PASSWORD				17
#define MAP_T_GET_PASSWORD				18
#define MAP_T_SS_INVOCATION_NOTIFICATION		72
#define MAP_T_REGISTER_CC_ENTRY				76
#define MAP_T_ERASE_CC_ENTRY				77

#if 0
/*
 * SERVICE ERRORS
 */
/* Service-Error problem values (MAP_T_User_Error): */
#define MAP_E_ILLEGAL_SS_OPERATION			16
#define MAP_E_LONG_TERM_DENIAL				30
#define MAP_E_NEGATIVE_PW_CHECK				38
#define MAP_E_NUMBER_OF_PW_ATTEMPTS_VIOLATION		43
#define MAP_E_PW_REGISTRATION_FAILURE			37
#define MAP_E_SHORT_TERM_DENIAL				29
#define MAP_E_SS_ERROR_STATUS				17
#define MAP_E_SS_INCOMPATIBILITY			20
#define MAP_E_SS_NOT_AVAILABLE				18
#define MAP_E_SS_SUBSCRIPTION_VIOLATION			19
#define MAP_E_UNKNOWN_ALPHABET				71
#define MAP_E_USSD_BUSY					72

/* Service-Error parameter classes: */
#define OMP_O_MAP_C_ILLEGAL_SS_OPERATION_PARAM		mapP_gsm_ss(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_LONG_TERM_DENIAL_PARAM		mapP_gsm_ss(\x8F\x52)	/* 2002 */
#define OMP_O_MAP_C_PW_REGISTRATION_FAILURE_CAUSE	mapP_gsm_ss(\x8F\x53)	/* 2003 */
#define OMP_O_MAP_C_SHORT_TERM_DENIAL_PARAM		mapP_gsm_ss(\x8F\x54)	/* 2004 */
#define OMP_O_MAP_C_SS_INCOMPATIBILITY_PARAM		mapP_gsm_ss(\x8F\x55)	/* 2005 */
#define OMP_O_MAP_C_SS_NOT_AVAILABLE_PARAM		mapP_gsm_ss(\x8F\x56)	/* 2006 */
#define OMP_O_MAP_C_SS_SUBSCRIPTION_VIOLATION_PARAM	mapP_gsm_ss(\x8F\x58)	/* 2008 */

/* Service-Error attributes: */
#define MAP_A_BEARER_SERVICE				((OM_type)6101)
#define MAP_A_TELESERVICE				((OM_type)6102)
#define MAP_A_SS_CODE					((OM_type)6103)
#define MAP_A_BASIC_SERVICE				((OM_type)6104)
#define MAP_A_SS_STATUS					((OM_type)6105)
#define MAP_A_PW_REGISTRATION_FAILURE_CAUSE		((OM_type)6106)

/* Service-Error enumerations: */

/* MAP_T_PW_Registration_Failure_Cause: */
typedef enum {
	MAP_T_UNDETERMINED = 0,
	MAP_T_INVALID_FORMAT,
	MAP_T_NEW_PASSWORDS_MISMATCH,
} MAP_T_PW_Registration_Failure_Cause;
#endif

/* Uses:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 * OMP_O_MAP_C_BASIC_SERVICE_CODE <xmap_gsm.h>
 * OMP_O_MAP_C_EXTERNAL_SIGNAL_INFO <xmap_gsm.h>
 *
 * OMP_O_MAP_C_BASIC_SERVICE_GROUP_LIST (this file)
 * OMP_O_MAP_C_CALL_BARRING_FEATURE (this file)
 * OMP_O_MAP_C_CALL_BARRING_FEATURE_LIST (this file)
 * OMP_O_MAP_C_CALL_BARRING_INFO (this file)
 * OMP_O_MAP_C_CCBS_DATA (this file)
 * OMP_O_MAP_C_CCBS_FEATURE (this file)
 * OMP_O_MAP_C_CCBS_FEATURE_LIST (this file)
 * OMP_O_MAP_C_FORWARDING_FEATURE (this file)
 * OMP_O_MAP_C_FORWARDING_FEATURE_LIST (this file)
 * OMP_O_MAP_C_FORWARDING_INFO (this file)
 * OMP_O_MAP_C_GENERIC_SERVICE_INFO (this file)
 * OMP_O_MAP_C_SS_DATA (this file)
 * OMP_O_MAP_C_SS_EVENT_SPECIFICATION (this file)
 * OMP_O_MAP_C_SS_SUBSCRIPTION_OPTION (this file)
 *
 * MAP_T_Alerting_Pattern (this file)
 * MAP_T_CCBS_Request_State (this file)
 * MAP_T_Cli_Restriction_Option (this file)
 * MAP_T_Override_Category (this file)
 * MAP_T_Service_Indicator (this file)
 */

/* Errors:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 * OMP_O_MAP_C_BASIC_SERVICE_CODE <xmap_gsm.h>
 *
 * MAP_T_PW_Registration_Failure_Cause (this file)
 */

#endif				/* __XMAP_GSM_SS_H__ */
