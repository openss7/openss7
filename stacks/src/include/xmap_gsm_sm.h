/*****************************************************************************

 @(#) $Id: xmap_gsm_sm.h,v 0.9.2.1 2009-03-13 11:20:25 brian Exp $

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

 $Log: xmap_gsm_sm.h,v $
 Revision 0.9.2.1  2009-03-13 11:20:25  brian
 - doc and header updates

 *****************************************************************************/

#ifndef __XMAP_GSM_SM_H__
#define __XMAP_GSM_SM_H__

#ident "@(#) $RCSfile: xmap_gsm_sm.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-sm(9) }
 */
#define OMP_O_MAP_GSM_SM_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x09"

/* Intermediate object identifier macro: */
#define mapP_gsm_sm(X) (OMP_O_MAP_GSM_SM_PKG# #X)

/* mwdMngtPackage */
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V3		mapP_acId(\x18\x03) /* { map-ac mwdMngt(24) version3(3) } */
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V2		mapP_acId(\x18\x02) /* { map-ac mwdMngt(24) version2(2) } */
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V1		mapP_acId(\x18\x01) /* { map-ac mwdMngt(24) version1(1) } */

/* alertingPackage */
#define OMP_O_MAP_SHORT_MSG_ALERT_CONTEXT_V2	mapP_acId(\x17\x02) /* { map-ac shortMsgAlert(23) version2(2) } */
#define OMP_O_MAP_SHORT_MSG_ALERT_CONTEXT_V1	mapP_acId(\x17\x01) /* { map-ac shortMsgAlert(23) version1(1) } */

/* mo-ShortMsgRelayPackage */
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V3	mapP_acId(\x15\x03) /* { map-ac shortMsgMO-Relay(21) version3(3) } */
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V2	mapP_acId(\x15\x02) /* { map-ac shortMsgMO-Relay(21) version2(2) } */
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V1	mapP_acId(\x15\x01) /* { map-ac shortMsgMO-Relay(21) version1(1) } */

/* mt-ShortMsgRelayPackage */
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V3	mapP_acId(\x19\x03) /* { map-ac shortMstMT-Relay(25) version3(3) } */
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V2	mapP_acId(\x19\x02) /* { map-ac shortMstMT-Relay(25) version2(2) } */
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V1	mapP_acId(\x15\x01) /* { map-ac shortMstMO-Relay(21) version1(1) } */

/* shortMsgGatewayPackage */
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V3	mapP_acId(\x14\x03) /* { map-ac shortMsgGateway(20) version3(3) } */
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V2	mapP_acId(\x14\x02) /* { map-ac shortMsgGateway(20) version2(2) } */
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V1	mapP_acId(\x14\x01) /* { map-ac shortMsgGateway(20) version1(1) } */

/* Application-Context-Name-List object to use in session with map_bind() to
 * restrict service responder indications to the listed application context
 * names. */

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

#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_SM_PARAM			mapP_gsm_sm(\xC6\x29) /* 9001 */
#define OMP_O_MAP_C_ADDITIONAL_NUMBER				mapP_gsm_sm(\xC6\x2A) /* 9002 */
#define OMP_O_MAP_C_ALERT_SERVICE_CENTRE_ARG			mapP_gsm_sm(\xC6\x2B) /* 9003 */ /* MAP_SHORT_MSG_ALERT_CONTEXT */
#define OMP_O_MAP_C_ALERT_SERVICE_CENTRE_RES			mapP_gsm_sm(\xC6\x2C) /* 9004 */ /* MAP_SHORT_MSG_ALERT_CONTEXT */
#define OMP_O_MAP_C_INFORM_SERVICE_CENTRE_ARG			mapP_gsm_sm(\xC6\x2D) /* 9005 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_LOCATION_INFO_WITH_LMSI			mapP_gsm_sm(\xC6\x2E) /* 9006 */
#define OMP_O_MAP_C_MESSAGE_WAITING_LIST_FULL_PARAM		mapP_gsm_sm(\xC6\x2F) /* 9007 */
#define OMP_O_MAP_C_MO_FORWARD_SM_ARG				mapP_gsm_sm(\xC6\x30) /* 9008 */ /* MAP_SHORT_MSG_MO_RELAY_CONTEXT */
#define OMP_O_MAP_C_MO_FORWARD_SM_RES				mapP_gsm_sm(\xC6\x31) /* 9009 */ /* MAP_SHORT_MSG_MO_RELAY_CONTEXT */
#define OMP_O_MAP_C_MT_FORWARD_SM_ARG				mapP_gsm_sm(\xC6\x32) /* 9010 */ /* MAP_SHORT_MSG_MT_RELAY_CONTEXT */
#define OMP_O_MAP_C_MT_FORWARD_SM_RES				mapP_gsm_sm(\xC6\x33) /* 9011 */ /* MAP_SHORT_MSG_MT_RELAY_CONTEXT */
#define OMP_O_MAP_C_READY_FOR_SM_ARG				mapP_gsm_sm(\xC6\x34) /* 9012 */
#define OMP_O_MAP_C_READY_FOR_SM_RES				mapP_gsm_sm(\xC6\x35) /* 9013 */
#define OMP_O_MAP_C_REPORT_SM_DELIVERY_STATUS_ARG		mapP_gsm_sm(\xC6\x36) /* 9014 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_REPORT_SM_DELIVERY_STATUS_RES		mapP_gsm_sm(\xC6\x37) /* 9015 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_SM_ARG			mapP_gsm_sm(\xC6\x38) /* 9016 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_SM_RES			mapP_gsm_sm(\xC6\x39) /* 9017 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SEND_INFO_FOR_MO_SMS_ARG			mapP_gsm_sm(\xC6\x3A) /* 9018 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SEND_INFO_FOR_MO_SMS_RES			mapP_gsm_sm(\xC6\x3B) /* 9019 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SEND_INFO_FOR_MT_SMS_ARG			mapP_gsm_sm(\xC6\x3C) /* 9020 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SEND_INFO_FOR_MT_SMS_RES			mapP_gsm_sm(\xC6\x3D) /* 9021 */ /* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define OMP_O_MAP_C_SM_DELIVERY_FAILURE_CAUSE			mapP_gsm_sm(\xC6\x3E) /* 9022 */
#define OMP_O_MAP_C_SM_RP_DA					mapP_gsm_sm(\xC6\x3F) /* 9023 */
#define OMP_O_MAP_C_SM_RP_OA					mapP_gsm_sm(\xC6\x40) /* 9024 */
#define OMP_O_MAP_C_SUB_BUSY_FOR_MT_SMS_PARAM			mapP_gsm_sm(\xC6\x41) /* 9025 */
#define OMP_O_MAP_C_MT_FORWARD_SM_VGCS_ARG			mapP_gsm_sm(\xC6\x42) /* 9026 */
#define OMP_O_MAP_C_MT_FORWARD_SM_VGCS_RES			mapP_gsm_sm(\xC6\x43) /* 9027 */
#define OMP_O_MAP_C_DISPATCHER_LIST				mapP_gsm_sm(\xC6\x44) /* 9028 */

/* OM attribute names (prefixed MAP_) */
#define MAP_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM			((OM_type)9001)
#define MAP_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM		((OM_type)9002)
#define MAP_ADDITIONAL_NUMBER					((OM_type)9003) /* XXX */
#define MAP_ADDITIONAL_SM_DELIVERY_OUTCOME			((OM_type)9004)
#define MAP_ALERT_REASON_INDICATOR				((OM_type)9005)
#define MAP_ALERT_REASON					((OM_type)9006)
#define MAP_DELIVERY_OUTCOME_INDICATOR				((OM_type)9007)
#define MAP_GPRS_CONNECTION_SUSPENDED				((OM_type)9008)
#define MAP_GPRS_NODE_INDICATOR					((OM_type)9009)
#define MAP_GPRS_SUPPORT_INDICATOR				((OM_type)9010)
#define MAP_IMSI						((OM_type)9011) /* XXX */ /* XXX */
#define MAP_LMSI						((OM_type)9012) /* XXX */ /* XXX */
#define MAP_LOCATION_INFO_WITH_LMSI				((OM_type)9013)
#define MAP_MORE_MESSAGES_TO_SEND				((OM_type)9014)
#define MAP_MSC_NUMBER						((OM_type)9015) /* XXX */
#define MAP_MSISDN						((OM_type)9016)
#define MAP_MW_STATUS						((OM_type)9017)
#define MAP_NETWORK_NODE_NUMBER					((OM_type)9018) /* XXX */
#define MAP_NO_SM_RP_DA						((OM_type)9019)
#define MAP_NO_SM_RP_OA						((OM_type)9020)
#define MAP_SERVICE_CENTRE_ADDRESS				((OM_type)9021) /* XXX */
#define MAP_SGSN_NUMBER						((OM_type)9022) /* XXX */
#define MAP_SM_DELIVERY_OUTCOME					((OM_type)9023)
#define MAP_SM_ENUMERATED_DELIVERY_FAILURE_CAUSE		((OM_type)9024)
#define MAP_SM_RP_DA						((OM_type)9025)
#define MAP_SM_RP_MTI						((OM_type)9026)
#define MAP_SM_RP_OA						((OM_type)9027)
#define MAP_SM_RP_PRI						((OM_type)9028)
#define MAP_SM_RP_SMEA						((OM_type)9029)
#define MAP_SM_RP_UI						((OM_type)9030)
#define MAP_SM_DELIVERY_NOT_INTENDED				((OM_type)9031)
#define MAP_IP_SM_GW_INDICATOR					((OM_type)9032)
#define MAP_IP_SM_GW_SM_DELIVERY_OUTCOME			((OM_type)9033)
#define MAP_IP_GW_ABSENT_SUBSCRIBER_DIAGNOSTIC			((OM_type)9034)

/* Service-Error problem values */
#define MAP_E_ABSENT_SUBSCRIBER_SM				mapP_problem( 6)
#define MAP_E_MESSAGE_WAITING_LIST_FULL				mapP_problem(33)
#define MAP_E_SM_DELIVERY_FAILURE				mapP_problem(32)
#define MAP_E_SUBSCRIBER_BUSY_FOR_MT_SMS			mapP_problem(31)

/* MAP_T_Mw_Status bits: */
typedef enum {
	MAP_T_SC_ADDRESS_NOT_INCLUDED = 0,
	MAP_T_MNRF_SET,
	MAP_T_MCEF_SET,
	MAP_T_MNRG_SET,
} MAP_T_Mw_Status;

/* MAP_T_Alert_Reason: */
typedef enum {
	MAP_T_MS_PRESENT = 0,
	MAP_T_MEMORY_AVAILABLE,
} MAP_T_Alert_Reason;

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

/* MAP_T_SM_Delivery_Not_Intended: */
typedef enum {
	MAP_T_ONLY_IMSI_REQUSTED = 0,
	MAP_T_ONLY_MCC_MNC_REQUESTED,
} MAP_T_SM_Delivery_Not_Intended;

#endif				/* __XMAP_GSM_SM_H__ */

