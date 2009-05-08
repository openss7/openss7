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

#ifndef __XMAP_GSM_ERR_H__
#define __XMAP_GSM_ERR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2008-2009 Monavacon Limited."

/* Error classes: */
#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_SM_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ATI_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ATM_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ATSI_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_BEARER_SERV_NOT_PROV_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_BUSY_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_CALL_BARRED_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_CUG_REJECT_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_DATA_MISSING_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_EXTENSIBLE_CALL_BARRED_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_EXTENSIBLE_SYSTEM_FAILURE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_FACILITY_NOT_SUP_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_FORWARDING_FAILED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_FORWARDING_VIOLATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ILLEGAL_EQUIPMENT_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ILLEGAL_SS_OPERATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ILLEGAL_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_INCOMPATIBLE_TERMINAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_INFORMATION_NOT_AVAILABLE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_LONG_TERM_DENIAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_MESSAGE_WAIT_LIST_FULL_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_MM_EVENT_NOT_SUPPORTED_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NO_GROUP_CALL_NB_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NO_ROAMING_NB_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NO_SUBSCRIBER_REPLY_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NUMBER_CHANGE_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_OR_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_POSITION_METHOD_FAILURE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_PW_REGISTRATION_FAILURE_CAUSE		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_RESOURCE_LIMITATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ROAMING_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SECURE_TRANSPORT_ERROR_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SHORT_TERM_DENIAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SM_DELIVERY_FAILURE_CAUSE			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SS_INCOMPATIBILITY_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SS_NOT_AVAILABLE_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SS_SUBSCRIPTION_VIOLATION_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SUB_BUSY_FOR_MT_SMS_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SYSTEM_FAILURE_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_TARGET_CELL_OUTSIDE_GCA_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_TELESERV_NOT_PROV_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_TRACING_BUFFER_FULL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNAUTHORIZED_LCS_CLIENT_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNAUTHORIZED_REQUESTING_NETWORK_PARAM	mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNEXPECTED_DATA_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNIDENTIFIED_SUB_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNKNOWN_OR_UNREACHABLE_LCS_CLIENT_PARAM	mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNKNOWN_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */

/* Error attributes: */
#define MAP_A_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM				((OM_type)10001)	/* GSM ERR */
#define MAP_A_ABSENT_SUBSCRIBER_REASON					((OM_type)10002)	/* GSM ERR */
#define MAP_A_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM		((OM_type)10003)	/* GSM ERR */
#define MAP_A_ADDITIONAL_NETWORK_RESOURCE				((OM_type)10004)	/* GSM ERR */
#define MAP_A_BEARER_SERVICE						((OM_type)10006)	/* GSM ERR */
#define MAP_A_CALL_BARRING_CAUSE					((OM_type)10008)	/* GSM ERR */
#define MAP_A_CCBS_BUSY							((OM_type)10009)	/* GSM ERR */
#define MAP_A_CCBS_POSSIBLE						((OM_type)10010)	/* GSM ERR */
#define MAP_A_CUG_REJECT_CAUSE						((OM_type)10011)	/* GSM ERR */
#define MAP_A_DIAGNOSTIC_INFO						((OM_type)10007)	/* GSM ERR */
#define MAP_A_EXT_BEARER_SERVICE					((OM_type)10012)	/* GSM ERR */
#define MAP_A_EXTENSIBLE_CALL_BARRED_PARAM				((OM_type)10013)	/* GSM ERR */
#define MAP_A_EXTENSIBLE_SYSTEM_FAILURE_PARAM				((OM_type)10014)	/* GSM ERR */
#define MAP_A_EXTENSION_CONTAINER					((OM_type)10016)	/* GSM ERR */
#define MAP_A_EXTENSION							((OM_type)10015)	/* GSM ERR */
#define MAP_A_EXT_TELESERVICE						((OM_type)10017)	/* GSM ERR */
#define MAP_A_GPRS_CONNECTION_SUSPENDED					((OM_type)10018)	/* GSM ERR */
#define MAP_A_NEEDED_LCS_CAPABILITY_NOT_SUPPORTED_IN_SERVING_NODE	((OM_type)10019)	/* GSM ERR */
#define MAP_A_NETWORK_RESOURCE						((OM_type)10020)	/* GSM ERR */
#define MAP_A_POSITION_METHOD_FAILURE_DIAGNOSTIC			((OM_type)10021)	/* GSM ERR */
#define MAP_A_PROTECTED_PAYLOAD						((OM_type)10022)	/* GSM ERR */
#define MAP_A_ROAMING_NOT_ALLOWED_CAUSE					((OM_type)10023)	/* GSM ERR */
#define MAP_A_SECURITY_HEADER						((OM_type)10024)	/* GSM ERR */
#define MAP_A_SHAPE_OF_LOCATION_ESTIMATED_NOT_SUPPORTED			((OM_type)10025)	/* GSM ERR */
#define MAP_A_SM_ENUMERATED_DELIVERY_FAILURE_CAUSE			((OM_type)10026)	/* GSM ERR */
#define MAP_A_UNAUTHORIZED_LCS_CLIENT_DIAGNOSTIC			((OM_type)10030)	/* GSM ERR */
#define MAP_A_UNAUTHORIZED_MESSAGE_ORIGINATOR				((OM_type)10031)	/* GSM ERR */
#define MAP_A_UNKNOWN_SUBSCRIBER_DIAGNOSTIC				((OM_type)10032)	/* GSM ERR */

#if 0
/* Attributes used defined in <xmap_gsm.h> */
#define MAP_A_BASIC_SERVICE						((OM_type)10005)	/* GSM ERR */
#define MAP_A_SS_CODE							((OM_type)10027)	/* GSM ERR */
#define MAP_A_SS_STATUS							((OM_type)10028)	/* GSM ERR */
#define MAP_A_TELESERVICE						((OM_type)10029)	/* GSM ERR */
#endif

#endif				/* __XMAP_GSM_ERR_H__ */
