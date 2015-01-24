/*****************************************************************************

 @(#) src/include/xmap_gsm_sm.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

#ifndef __XMAP_GSM_SM_H__
#define __XMAP_GSM_SM_H__

#include <xom.h>
#include <xmap.h>
#include <xmap_gsm.h>

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-sm(5) }
 */
#define OMP_O_MAP_GSM_SM_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x05"

/* Intermediate object identifier macro: */
#define mapP_gsm_sm(X) (OMP_O_MAP_GSM_SM_PKG# #X)

/* This application context is used for short message gateway procedures. */
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT			mapP_acId(\x14)
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V3			mapP_acId(\x14\x03)
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V2			mapP_acId(\x14\x02)
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V1			mapP_acId(\x14\x01)

/* This application context is used between MSC and IWMSC or between SGSN and
   IWMSC for mobile originated short message relay procedures.  For the
   SGSN-IWMSC interface version 1, version 2 and version 3 of this pplication
   context are applicable. */
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT			mapP_acId(\x15)
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V3			mapP_acId(\x15\x03)
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V2			mapP_acId(\x15\x02)
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V1			mapP_acId(\x15\x01)

/* This application context is used for short message alerting procedures. */
#define OMP_O_MAP_SHORT_MSG_ALERT_CONTEXT			mapP_acId(\x17)
#define OMP_O_MAP_SHORT_MSG_ALERT_CONTEXT_V2			mapP_acId(\x17\x02)
#define OMP_O_MAP_SHORT_MSG_ALERT_CONTEXT_V1			mapP_acId(\x17\x01)

/* This application context is used between VLR and HLR or between SGSN and
   HLR for short message wiating data management procedures.  For the SGSN-HLR 
   interface only version 3 of this pplication context is applicable. */
#define OMP_O_MAP_MWD_MNGT_CONTEXT				mapP_acId(\x18)
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V3				mapP_acId(\x18\x03)
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V2				mapP_acId(\x18\x02)
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V1				mapP_acId(\x18\x01)

/* This application context is used between GMSC and MSC or between GMSC and
   SGSN for mobile terminating short message relay procedures.  For the
   GMSC-SGSN interface version 2 and version 3 of this application context and 
   the equivalent version 1 application context are applicable. */
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT			mapP_acId(\x19)
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V3			mapP_acId(\x19\x03)
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V2			mapP_acId(\x19\x02)
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V1			mapP_acId(\x15\x01)

/* This application context is used between SMS-GMSC and MSC for mobile
   termination short message relay procedures for VGCS. */
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_VGCS_CONTEXT		mapP_acId(\x29)
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_VGCS_CONTEXT_V3		mapP_acId(\x29\x03)

/* Application-Context-Name-List object to use in session with map_bind() to
   restrict service responder indications to the listed application context
   names. */

#define MAP_SM_ACNAME_LIST \
	{ \
		OM_OID_DESC(OM_CLASS, \
			    MAP_C_APPLICATION_CONTEXT_NAME_LIST), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_MWD_MNGT_CONTEXT_V3), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_MWD_MNGT_CONTEXT_V2), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_MWD_MNGT_CONTEXT_V1), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_ALERT_CONTEXT_V2), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_ALERT_CONTEXT_V1), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_MO_RELAY_CONTEXT_V3), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_MO_RELAY_CONTEXT_V2), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_MO_RELAY_CONTEXT_V1), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_MT_RELAY_CONTEXT_V3), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_MT_RELAY_CONTEXT_V2), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_MT_RELAY_CONTEXT_V1), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_GATEWAY_CONTEXT_V3), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_GATEWAY_CONTEXT_V2), \
		OM_OID_DESC(MAP_APPLICATION_CONTEXT_NAME, \
			    MAP_SHORT_MSG_GATEWAY_CONTEXT_V1), \
		OM_NULL_DESCRIPTOR \
	}

/* OM class names (prefixed MAP_C_) */
#define OMP_O_MAP_C_ADDITIONAL_NUMBER			mapP_gsm_sm(\xC6\x2A)	/* 9002 */
#define OMP_O_MAP_C_ALERT_SERVICE_CENTRE_ARG		mapP_gsm_sm(\xC6\x2B)	/* 9003 */	/* MAP_SHORT_MSG_ALERT_CONTEXT */
#define OMP_O_MAP_C_ALERT_SERVICE_CENTRE_RES		mapP_gsm_sm(\xC6\x2C)	/* 9004 */	/* MAP_SHORT_MSG_ALERT_CONTEXT */
#define OMP_O_MAP_C_DISPATCHER_LIST			mapP_gsm_sm(\xC6\x44)	/* 9028 */
#define OMP_O_MAP_C_INFORM_SERVICE_CENTRE_ARG		mapP_gsm_sm(\xC6\x2D)	/* 9005 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_LOCATION_INFO_WITH_LMSI		mapP_gsm_sm(\xC6\x2E)	/* 9006 */
#define OMP_O_MAP_C_MO_FORWARD_SM_ARG			mapP_gsm_sm(\xC6\x30)	/* 9008 */	/* MAP_SHORT_MSG_MO_RELAY_CONTEXT */
#define OMP_O_MAP_C_MO_FORWARD_SM_RES			mapP_gsm_sm(\xC6\x31)	/* 9009 */	/* MAP_SHORT_MSG_MO_RELAY_CONTEXT */
#define OMP_O_MAP_C_MT_FORWARD_SM_ARG			mapP_gsm_sm(\xC6\x32)	/* 9010 */	/* MAP_SHORT_MSG_MT_RELAY_CONTEXT */
#define OMP_O_MAP_C_MT_FORWARD_SM_RES			mapP_gsm_sm(\xC6\x33)	/* 9011 */	/* MAP_SHORT_MSG_MT_RELAY_CONTEXT */
#define OMP_O_MAP_C_MT_FORWARD_SM_VGCS_ARG		mapP_gsm_sm(\xC6\x42)	/* 9026 */
#define OMP_O_MAP_C_MT_FORWARD_SM_VGCS_RES		mapP_gsm_sm(\xC6\x43)	/* 9027 */
#define OMP_O_MAP_C_READY_FOR_SM_ARG			mapP_gsm_sm(\xC6\x34)	/* 9012 */
#define OMP_O_MAP_C_READY_FOR_SM_RES			mapP_gsm_sm(\xC6\x35)	/* 9013 */
#define OMP_O_MAP_C_REPORT_SM_DELIVERY_STATUS_ARG	mapP_gsm_sm(\xC6\x36)	/* 9014 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_REPORT_SM_DELIVERY_STATUS_RES	mapP_gsm_sm(\xC6\x37)	/* 9015 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_SM_ARG		mapP_gsm_sm(\xC6\x38)	/* 9016 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_SM_RES		mapP_gsm_sm(\xC6\x39)	/* 9017 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SEND_INFO_FOR_MO_SMS_ARG		mapP_gsm_sm(\xC6\x3A)	/* 9018 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SEND_INFO_FOR_MO_SMS_RES		mapP_gsm_sm(\xC6\x3B)	/* 9019 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SEND_INFO_FOR_MT_SMS_ARG		mapP_gsm_sm(\xC6\x3C)	/* 9020 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SEND_INFO_FOR_MT_SMS_RES		mapP_gsm_sm(\xC6\x3D)	/* 9021 */	/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SM_RP_DA				mapP_gsm_sm(\xC6\x3F)	/* 9023 */
#define OMP_O_MAP_C_SM_RP_OA				mapP_gsm_sm(\xC6\x40)	/* 9024 */

/* OM attribute names (prefixed MAP_) */
#define MAP_A_ADDITIONAL_SM_DELIVERY_OUTCOME		((OM_type)7004)
#define MAP_A_ALERT_REASON_INDICATOR			((OM_type)7005)
#define MAP_A_ALERT_REASON				((OM_type)7006)
#define MAP_A_DELIVERY_OUTCOME_INDICATOR		((OM_type)7008)
#define MAP_A_DISPATCHER				((OM_type)7009)
#define MAP_A_DISPATCHER_LIST				((OM_type)7010)
#define MAP_A_GPRS_SUPPORT_INDICATOR			((OM_type)7014)
#define MAP_A_IP_GW_ABSENT_SUBSCRIBER_DIAGNOSTIC	((OM_type)7016)
#define MAP_A_IP_SM_GW_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM	((OM_type)7017)
#define MAP_A_IP_SM_GW_INDICATOR			((OM_type)7018)
#define MAP_A_IP_SM_GW_SM_DELIVERY_OUTCOME		((OM_type)7019)
#define MAP_A_LOCATION_INFO_WITH_LMSI			((OM_type)7021)
#define MAP_A_MORE_MESSAGES_TO_SEND			((OM_type)7022)
#define MAP_A_MW_STATUS					((OM_type)7025)
#define MAP_A_NO_SM_RP_DA				((OM_type)7027)
#define MAP_A_NO_SM_RP_OA				((OM_type)7028)
#define MAP_A_ONGOING_CALL				((OM_type)7029)
#define MAP_A_SERVICE_CENTRE_ADDRESS			((OM_type)7030)
#define MAP_A_SM_DELIVERY_NOT_INTENDED			((OM_type)7032)
#define MAP_A_SM_DELIVERY_OUTCOME			((OM_type)7033)
#define MAP_A_SM_RP_DA					((OM_type)7034)
#define MAP_A_SM_RP_MTI					((OM_type)7035)
#define MAP_A_SM_RP_OA					((OM_type)7036)
#define MAP_A_SM_RP_PRI					((OM_type)7037)
#define MAP_A_SM_RP_SMEA				((OM_type)7038)
#define MAP_A_SM_RP_UI					((OM_type)7039)

#if 0
/* Attributes used defined in <xmap_gsm.h> */
#define MAP_A_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM		((OM_type)7001)
#define MAP_A_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM ((OM_type)7002)
#define MAP_A_ADDITIONAL_NUMBER				((OM_type)7003)
#define MAP_A_ASCI_CALL_REFERENCE			((OM_type)7007)
#define MAP_A_EXTENSION					((OM_type)7011)
#define MAP_A_EXTENSION_CONTAINER			((OM_type)7012)
#define MAP_A_GPRS_NODE_INDICATOR			((OM_type)7013)
#define MAP_A_IMSI					((OM_type)7015)
#define MAP_A_LMSI					((OM_type)7020)
#define MAP_A_MSC_NUMBER				((OM_type)7023)
#define MAP_A_MSISDN					((OM_type)7024)
#define MAP_A_NETWORK_NODE_NUMBER			((OM_type)7026)
#define MAP_A_SGSN_NUMBER				((OM_type)7031)
#endif

/* OM enumerations (prefixed MAP_T_) */

/* MAP_T_Alert_Reason: */
typedef enum {
	MAP_T_MS_PRESENT = 0,
	MAP_T_MEMORY_AVAILABLE,
} MAP_T_Alert_Reason;

/* MAP_T_Mw_Status bits: */
typedef enum {
	MAP_T_SC_ADDRESS_NOT_INCLUDED = 0,
	MAP_T_MNRF_SET,
	MAP_T_MCEF_SET,
	MAP_T_MNRG_SET,
} MAP_T_Mw_Status;

/* MAP_T_SM_Delivery_Not_Intended: */
typedef enum {
	MAP_T_ONLY_IMSI_REQUSTED = 0,
	MAP_T_ONLY_MCC_MNC_REQUESTED,
} MAP_T_SM_Delivery_Not_Intended;

/* MAP_T_SM_Delivery_Outcome: */
typedef enum {
	MAP_T_MEMORY_CAPACITY_EXCEEDED = 0,
	MAP_T_ABSENT_SUBSCRIBER,
	MAP_T_SUCCESSFUL_TRANSFER,
} MAP_T_SM_Delivery_Outcome;

/* MAP_T_SM_RP_MTI: */
typedef enum {
	MAP_T_SMS_DELIVER = 0,
	MAP_T_STATUS_REPORT,
} MAP_T_SM_RP_MTI;

/* Operation codes (MAP_T_Operation_Code): */
#define MAP_T_SEND_ROUTING_INFO_FOR_SM			45
#define MAP_T_MO_FORWARD_SM				46
#define MAP_T_MT_FORWARD_SM				44
#define MAP_T_REPORT_SM_DELIVERY_STATUS			47
#define MAP_T_ALERT_SERVICE_CENTRE			64
#define MAP_T_INFORM_SERVICE_CENTRE			63
#define MAP_T_READY_FOR_SM				66
#define MAP_T_MT_FORWARD_SM_VGCS			21

#if 0
/*
 * SERVICE ERRORS
 */
/* Service-Error problem values (MAP_T_User_Error): */
#define MAP_E_ABSENT_SUBSCRIBER_SM			mapP_problem( 6)
#define MAP_E_MESSAGE_WAITING_LIST_FULL			mapP_problem(33)
#define MAP_E_SM_DELIVERY_FAILURE			mapP_problem(32)
#define MAP_E_SUBSCRIBER_BUSY_FOR_MT_SMS		mapP_problem(31)

/* Service-Error parameter classes: */
#define OMP_O_MAP_C_SUB_BUSY_FOR_MT_SMS_PARAM		mapP_gsm_sm(\xC6\x41)	/* 9025 */
#define OMP_O_MAP_C_SM_DELIVERY_FAILURE_CAUSE		mapP_gsm_sm(\xC6\x3E)	/* 9022 */
#define OMP_O_MAP_C_MESSAGE_WAIT_LIST_FULL_PARAM	mapP_gsm_sm(\xC6\x2F)	/* 9007 */

/* Service-Error attributes: */
#define MAP_A_GPRS_CONNECTION_SUSPENDED				((OM_type)7101)
#define MAP_A_SM_ENUMERATED_DELIVERY_FAILURE_CAUSE		((OM_type)7102)
#define MAP_A_DIAGNOSTIC_INFO					((OM_type)7103)
#define MAP_A_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM			((OM_type)9001)
#define MAP_A_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM	((OM_type)9002)

/* Service-Error enumerations: */

/* MAP_T_SM_Enumerated_Delivery_Failure_Cause: */
typedef enum {
	MAP_T_MEMORY_CAPACITY_EXCEEDED = 0,
	MAP_T_EQUIPMENT_PROTOCOL_ERROR,
	MAP_T_EQUIPMENT_NOT_SM_EQUIPPED,
	MAP_T_UNKNOWN_SERVICE_CENTRE,
	MAP_T_SC_CONGESTION,
	MAP_T_INVALID_SME_ADDRESS,
	MAP_T_SUBSCRIBER_NOT_SC_SUBSCRIBER,
} MAP_T_SM_Enumerated_Delivery_Failure_Cause;
#endif


/* Uses:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 *
 * OMP_O_MAP_C_ADDITIONAL_NUMBER (this file)
 * OMP_O_MAP_C_SM_RP_DA (this file)
 * OMP_O_MAP_C_SM_RP_OA (this file)
 * OMP_O_MAP_C_DISPATCHER_LIST (this file)
 * OMP_O_MAP_C_LOCATION_INFO_WITH_LMSI (this file)
 *
 * MAP_T_Alert_Reason (this file)
 * MAP_T_SM_Delivery_Not_Intended (this file)
 * MAP_T_SM_Delivery_Outcome (this file)
 */

/* Errors:
 *
 * OMP_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 *
 * MAP_T_SM_Enumerated_Delivery_Failure_Cause
 */

#endif				/* __XMAP_GSM_SM_H__ */
