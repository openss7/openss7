/*****************************************************************************

 @(#) $Id: xmap_sms.h,v 0.9.2.1 2009-03-05 15:51:26 brian Exp $

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

 Last Modified $Date: 2009-03-05 15:51:26 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_sms.h,v $
 Revision 0.9.2.1  2009-03-05 15:51:26  brian
 - new files for map library

 *****************************************************************************/

#ifndef __XMAP_SMS_H__
#define __XMAP_SMS_H__

#ident "@(#) $RCSfile: xmap_sms.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/* SMS parameters */

#define MAP_SM_RP_DA				((OM_type)12001)
#define MAP_SM_RP_OA				((OM_type)12002)
#define MAP_MWD_STATUS				((OM_type)12003)
#define MAP_SM_RP_UI				((OM_type)12004)
#define MAP_SM_RP_PRI				((OM_type)12005)
#define MAP_SM_DELIVERY_OUTCOME			((OM_type)12006)
#define MAP_MORE_MESSAGES_TO_SEND		((OM_type)12007)
#define MAP_ALERT_REASON			((OM_type)12008)
#define MAP_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM	((OM_type)12009)
#define MAP_ALERT_REASON_INDICATOR		((OM_type)12010)
#define MAP_ADDITIONAL_SM_DELIVERY_OUTCOME	((OM_type)12011)
#define MAP_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM ((OM_type)12012)
#define MAP_DELIVERY_OUTCOME_INDICATOR		((OM_type)12013)
#define MAP_GPRS_NODE_INDICATOR			((OM_type)12014)
#define MAP_GRPS_SUPPORT_INDICATOR		((OM_type)12015)
#define MAP_SM_RP_MTI				((OM_type)12016)
#define MAP_SM_RP_SMEA				((OM_type)12017)

/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define MAP_C_SEND_ROUTING_INFO_FOR_SM_ARG
/* (M) (M=) MAP_INVOKE_ID
 * (M) (M=) MAP_MS_ISDN
 * (M) (M=) MAP_SM_RP_PRI
 * (M) (M=) MAP_SERVICE_CENTRE_ADDRESS
 * (C) (C=) MAP_SM_RP_MTI
 * (C) (C=) MAP_SM_RP_SMEA
 * (C) (C=) MAP_GPRS_SUPPORT_INDICATOR
 */

#define MAP_C_SEND_ROUTING_INFO_FOR_SM_RES
/* (M=)(M=) MAP_INVOKE_ID
 * (C )(C=) MAP_IMSI
 * (C )(C=) MAP_NETWORK_NODE_NUMBER
 * (C )(C=) MAP_LMSI
 * (C )(C=) MAP_GPRS_NODE_INDICATOR
 * (C )(C=) MAP_ADDITIONAL_NUMBER
 * (C )(C=) MAP_SERVICE_USER_ERROR
 * (- )(O) MAP_SERVICE_PROVIDER_ERROR
 */

/* MAP_SHORT_MSG_MO_RELAY_CONTEXT */
#define MAP_C_MO_FORWARD_SHORT_MESSAGE_ARG
/* (M)(M=) MAP_INVOKE_ID
 * (M)(M=) MAP_SM_RP_DA
 * (M)(M=) MAP_SM_RP_OA
 * (M)(M=) MAP_SM_RP_UI
 * (C)(C=) MAP_IMSI
 */
#define MAP_C_MO_FORWARD_SHORT_MESSAGE_RES
/* (M=)(M=) MAP_INVOKE_ID
 * (C )(C=) MAP_SM_RP_UI
 * (C )(C=) MAP_SERVICE_USER_ERROR
 * (- )(O) MAP_SERVICE_PROVIDER_ERROR
 */

/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define MAP_C_REPORT_SM_DELIVERY_STATUS_ARG
/* (M)(M=) MAP_INVOKE_ID
 * (M)(M=) MAP_MS_ISDN
 * (M)(M=) MAP_SERVICE_CENTER_ADDRESS
 * (M)(M=) MAP_SM_DELIVERY_OUTCOME
 * (C)(C=) MAP_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM
 * (C)(C=) MAP_GPRS_SUPPORT_INDICATOR
 * (C)(C=) MAP_DELIVERY_OUTCOME_INDICATOR
 * (C)(C=) MAP_ADDITIONALSM_DELIVERY_OUTCOME
 * (C)(C=) MAP_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM
 */
#define MAP_C_REPORT_SM_DELIVERY_STATUS_RES
/* (M=)(M=) MAP_INVOKE_ID
 * (C )(C=) MAP_MSISDN_ALERT
 * (C )(C=) MAP_SERVICE_USER_ERROR
 * (- )(O ) MAP_SERVICE_PROVIDER_ERROR
 */

#define MAP_C_READY_FOR_SM_ARG
/* (M)(M=) MAP_INVOKE_ID
 * (C)(C=) MAP_IMSI
 * (C)(C=) MAP_TMSI
 * (M)(M=) MAP_ALERT_REASON
 * (C)(C=) MAP_ALERT_REASON_INDICATOR
 */
#define MAP_C_READY_FOR_SM_RES
/* (M=)(M=) MAP_INVOKE_ID
 * (C )(C) MAP_SERVICE_USER_ERROR
 * (- )(O) MAP_SERVICE_PROVIDER_ERROR
 */

/* MAP_SHORT_MSG_ALERT_CONTEXT */
#define MAP_C_ALERT_SERVICE_CENTER_ARG
/* (M)(M=) MAP_INVOKE_ID
 * (M)(M=) MAP_MSISDN_ALERT
 * (M)(M=) MAP_SERVICE_CENTER_ADDRESS
 */
#define MAP_C_ALERT_SERVICE_CENTER_RES
/* (M=)(M=) MAP_INVOKE_ID
 * (C )(C=) MAP_SERVICE_USER_ERROR
 * (- )(O) MAP_SERVICE_PROVIDER_ERROR
 */

/* MAP_SHORT_MSG_GATEWAY_CONTEXT */
#define MAP_C_INFORM_SERVICE_CENTER_ARG
/* (M)(M=) MAP_INVOKE_ID
 * (C)(C=) MAP_MSISDN_ALERT
 * (C)(C=) MAP_MWD_STATUS
 * (C)(C=) MAP_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM
 * (C)(C=) MAP_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM
 */

#define MAP_C_SEND_INFO_FOR_MT_SMS_ARG
/* (M)(M=) MAP_INVOKE_ID
 * (M)(M=) MAP_SM_RP_DA
 * (C)(C ) MAP_IMSI
 */
#define MAP_C_SEND_INFO_FOR_MT_SMS_RES
/* (M=)(M=) MAP_INVOKE_ID
 * (C )(C=) MAP_MS_ISDN
 * (C )(C=) MAP_SERVICE_USER_ERROR
 * (- )(O) MAP_SERVICE_PROVIDER_ERROR
 */

#define MAP_C_SEND_INFO_FOR_MO_SMS_ARG
/* (M)(M=) MAP_INVOKE_ID
 * (M)(M=) MAP_SERVICE_CENTER_ADDRESS
 */
#define MAP_C_SEND_INFO_FOR_MO_SMS_RES
/* (M=)(M=) MAP_INVOKE_ID
 * (C )(C=) MAP_MS_ISDN
 * (C )(C=) MAP_SERVICE_USER_ERROR
 * (- )(O ) MAP_SERVICE_PROVIDER_ERROR
 */

/* MAP_SHORT_MSG_MT_RELAY_CONTEXT */
#define MAP_C_MT_FORWARD_SHORT_MESSAGE_ARG
/* (M)(M=) MAP_INVOKE_ID
 * (M)(M=) MAP_SM_RP_DA
 * (M)(M=) MAP_SM_RP_OA
 * (M)(M=) MAP_SM_RP_UI
 * (C)(C=) MAP_MORE_MESSAGES_TO_SEND
 */
#define MAP_C_MT_FORWARD_SHORT_MESSAGE_RES
/* (M=)(M=) MAP_INVOKE_ID
 * (C )(C=) MAP_SM_RP_UI
 * (C )(C=) MAP_SERVICE_USER_ERROR
 * (- )(O) MAP_SERVICE_PROVIDER_ERROR
 */

/* mwdMngtPackage */
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V3		mapP_acId(\x18\x03)
 /* { map-ac mwdMngt(24) version3(3) } */
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V2		mapP_acId(\x18\x02)
 /* { map-ac mwdMngt(24) version2(2) } */
#define OMP_O_MAP_MWD_MNGT_CONTEXT_V1		mapP_acId(\x18\x01)
 /* { map-ac mwdMngt(24) version1(1) } */

/* alertingPackage */
#define OMP_O_MAP_SHORT_MSG_ALERT_CONTEXT_V2	mapP_acId(\x17\x02)
 /* { map-ac shortMsgAlert(23) version2(2) } */
#define OMP_O_MAP_SHORT_MSG_ALERT_CONTEXT_V1	mapP_acId(\x17\x01)
 /* { map-ac shortMsgAlert(23) version1(1) } */

/* mo-ShortMsgRelayPackage */
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V3	mapP_acId(\x15\x03)
 /* { map-ac shortMsgMO-Relay(21) version3(3) } */
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V2	mapP_acId(\x15\x02)
 /* { map-ac shortMsgMO-Relay(21) version2(2) } */
#define OMP_O_MAP_SHORT_MSG_MO_RELAY_CONTEXT_V1	mapP_acId(\x15\x01)
 /* { map-ac shortMsgMO-Relay(21) version1(1) } */

/* mt-ShortMsgRelayPackage */
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V3	mapP_acId(\x19\x03)
 /* { map-ac shortMstMT-Relay(25) version3(3) } */
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V2	mapP_acId(\x19\x02)
 /* { map-ac shortMstMT-Relay(25) version2(2) } */
#define OMP_O_MAP_SHORT_MSG_MT_RELAY_CONTEXT_V1	mapP_acId(\x15\x01)
 /* { map-ac shortMstMO-Relay(21) version1(1) } */

/* shoftMsgGatewayPackage */
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V3	mapP_acId(\x14\x03)
 /* { map-ac shortMsgGateway(20) version3(3) } */
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V2	mapP_acId(\x14\x02)
 /* { map-ac shortMsgGateway(20) version2(2) } */
#define OMP_O_MAP_SHORT_MSG_GATEWAY_CONTEXT_V1	mapP_acId(\x14\x01)
 /* { map-ac shortMsgGateway(20) version1(1) } */

/* Application-Context-Name-List object to use in session with map_bind() to
 * restrict service responder indications to the listed application context
 * names. */

#define MAP_SMS_ACNAME_LIST \
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

MAP_status map_sms_service_rcv(OM_private_object assoc,
			       OM_sint * primitive_return,
			       OM_private_object * arguments_return,
			       OM_private_object * component_return,
			       OM_sint * invoke_id_return);

MAP_status map_sms_service_req(OM_private_object assoc, OM_object arguments,
			       OM_object component, OM_sint * invoke_id_return);

MAP_status map_sms_service_rsp(OM_private_object assoc, OM_object arguments,
			       OM_object component, OM_sint invoke_id);

#endif				/* __XMAP_SMS_H__ */
