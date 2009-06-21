/*****************************************************************************

 @(#) $Id: xmap_gsm_xx.h,v 1.1.2.1 2009-06-21 11:23:46 brian Exp $

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

 Last Modified $Date: 2009-06-21 11:23:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm_xx.h,v $
 Revision 1.1.2.1  2009-06-21 11:23:46  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __XMAP_GSM_XX_H__
#define __XMAP_GSM_XX_H__

#ident "@(#) $RCSfile: xmap_gsm_xx.h,v $ $Name:  $($Revision: 1.1.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

#define OMP_O_MAP_C_ABORT_ARGUMENT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_ABORT_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_SM_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ACCEPT_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_ACCESS_NETWORK_SIGNAL_INFO			mapP_gsm(\x8F\x51)	/* 2001 */	/* GSM */
#define OMP_O_MAP_C_ACSE_ARGS					mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_ACTIVATE_SS_ARG				mapP_gsm_sm(\x87\x69)	/* 1001 */	/* GSM SS */
#define OMP_O_MAP_C_ACTIVATE_SS_RES				mapP_gsm_sm(\x87\x6A)	/* 1002 */	/* GSM SS */
#define OMP_O_MAP_C_ACTIVATE_TRACE_MODE_ARG			mapP_gsm_om(\x87\x69)	/* 1001 */	/* GSM OM */
#define OMP_O_MAP_C_ACTIVATE_TRACE_MODE_RES			mapP_gsm_om(\x87\x6A)	/* 1002 */	/* GSM OM */
#define OMP_O_MAP_C_ADD_INFO					mapP_gsm_ms(\x87\x69)	/* 1001 */	/* GSM MS */
#define OMP_O_MAP_C_ADDITIONAL_NUMBER				mapP_gsm_sm(\xC6\x2A)	/* 9002 */	/* GSM SM */
#define OMP_O_MAP_C_ADDRESS					mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_ALERT_SERVICE_CENTRE_ARG			mapP_gsm_sm(\xC6\x2B)	/* 9003 */	/* GSM SM */
#define OMP_O_MAP_C_ALERT_SERVICE_CENTRE_RES			mapP_gsm_sm(\xC6\x2C)	/* 9004 */	/* GSM SM */
#define OMP_O_MAP_C_ALLOWED_UMTS_ALGORITHMS			mapP_gsm_ms(\x87\x6A)	/* 1002 */	/* GSM MS */
#define OMP_O_MAP_C_AMBR					mapP_gsm_ms(\x87\x6B)	/* 1003 */	/* GSM MS */
#define OMP_O_MAP_C_ANY_TIME_INTERROGATION_ARG			mapP_gsm_ms(\x87\x6C)	/* 1004 */	/* GSM MS */
#define OMP_O_MAP_C_ANY_TIME_INTERROGATION_RES			mapP_gsm_ms(\x87\x6D)	/* 1005 */	/* GSM MS */
#define OMP_O_MAP_C_ANY_TIME_MODIFICATION_ARG			mapP_gsm_ms(\x87\x6E)	/* 1006 */	/* GSM MS */
#define OMP_O_MAP_C_ANY_TIME_MODIFICATION_RES			mapP_gsm_ms(\x87\x6F)	/* 1007 */	/* GSM MS */
#define OMP_O_MAP_C_ANY_TIME_SUBSCRIPTION_INTERROGATION_ARG	mapP_gsm_ms(\x87\x70)	/* 1008 */	/* GSM MS */
#define OMP_O_MAP_C_ANY_TIME_SUBSCRIPTION_INTERROGATION_RES	mapP_gsm_ms(\x87\x71)	/* 1009 */	/* GSM MS */
#define OMP_O_MAP_C_APN_CONFIGURATION				mapP_gsm_ms(\x87\x72)	/* 1010 */	/* GSM MS */
#define OMP_O_MAP_C_APN_CONFIGURATION_PROFILE			mapP_gsm_ms(\x87\x72)	/* 1010 */	/* GSM MS */
#define OMP_O_MAP_C_APPLICATION_CONTEXT_LIST			mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_AREA_DEFINITION				mapP_gsm_ls(\x87\x6A)	/* 1002 */	/* GSM LS */
#define OMP_O_MAP_C_AREA_EVENT_INFO				mapP_gsm_ls(\x87\x6B)	/* 1003 */	/* GSM LS */
#define OMP_O_MAP_C_AREA					mapP_gsm_ls(\x87\x69)	/* 1001 */	/* GSM LS */
#define OMP_O_MAP_C_ASSOC_ARGUMENT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_ASSOC_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_ATI_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ATM_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ATSI_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_AUTHENTICATION_FAILURE_REPORT_ARG		mapP_gsm_ms(\x87\x73)	/* 1011 */	/* GSM MS */
#define OMP_O_MAP_C_AUTHENTICATION_FAILURE_REPORT_RES		mapP_gsm_ms(\x87\x74)	/* 1012 */	/* GSM MS */
#define OMP_O_MAP_C_AUTHENTICATION_QUINTUPLET			mapP_gsm_ms(\x87\x75)	/* 1013 */	/* GSM MS */
#define OMP_O_MAP_C_AUTHENTICATION_SET_LIST			mapP_gsm_ms(\x87\x76)	/* 1014 */	/* GSM MS */
#define OMP_O_MAP_C_AUTHENTICATION_TRIPLET			mapP_gsm_ms(\x87\x77)	/* 1015 */	/* GSM MS */
#define OMP_O_MAP_C_BAD_ARGUMENT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_BASIC_SERVICE_CODE				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM */
#define OMP_O_MAP_C_BASIC_SERVICE_CRITERIA			mapP_gsm_ms(\x87\x78)	/* 1016 */	/* GSM MS */
#define OMP_O_MAP_C_BASIC_SERVICE_GROUP_LIST			mapP_gsm_sm(\x87\x6B)	/* 1003 */	/* GSM SS */
#define OMP_O_MAP_C_BASIC_SERVICE_LIST				mapP_gsm_ms(\x87\x79)	/* 1017 */	/* GSM MS */
#define OMP_O_MAP_C_BEARER_SERVICE_LIST				mapP_gsm_ms(\x87\x7A)	/* 1018 */	/* GSM MS */
#define OMP_O_MAP_C_BEARER_SERV_NOT_PROV_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_BSSMAP_SERVICE_HANDOVER_INFO		mapP_gsm_ms(\x87\x7B)	/* 1019 */	/* GSM MS */
#define OMP_O_MAP_C_BSSMAP_SERVICE_HANDOVER_LIST		mapP_gsm_ms(\x87\x7C)	/* 1020 */	/* GSM MS */
#define OMP_O_MAP_C_BUSY_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_CALL_BARRED_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_CALL_BARRING_DATA				mapP_gsm_ms(\x87\x7D)	/* 1021 */	/* GSM MS */
#define OMP_O_MAP_C_CALL_BARRING_FEATURE_LIST			mapP_gsm_sm(\x87\x6D)	/* 1005 */	/* GSM SS */
#define OMP_O_MAP_C_CALL_BARRING_FEATURE			mapP_gsm_sm(\x87\x6C)	/* 1004 */	/* GSM SS */
#define OMP_O_MAP_C_CALL_BARRING_INFO				mapP_gsm_sm(\x87\x6E)	/* 1006 */	/* GSM SS */
#define OMP_O_MAP_C_CALL_FORWARDING_DATA			mapP_gsm_ms(\x87\x7E)	/* 1022 */	/* GSM MS */
#define OMP_O_MAP_C_CALL_REPORT_DATA				mapP_gsm_ch(\x87\x69)	/* 1001 */	/* GSM CH */
#define OMP_O_MAP_C_CAMEL_INFO					mapP_gsm_ch(\x87\x6A)	/* 1002 */	/* GSM CH */
#define OMP_O_MAP_C_CAMEL_ROUTING_INFO				mapP_gsm_ch(\x87\x6B)	/* 1003 */	/* GSM CH */
#define OMP_O_MAP_C_CAMEL_SUBSCRIPTION_INFO			mapP_gsm_ms(\x87\x7F)	/* 1023 */	/* GSM MS */
#define OMP_O_MAP_C_CANCEL_LOCATION_ARG				mapP_gsm_ms(\x88\x00)	/* 1024 */	/* GSM MS */
#define OMP_O_MAP_C_CANCEL_LOCATION_RES				mapP_gsm_ms(\x88\x01)	/* 1025 */	/* GSM MS */
#define OMP_O_MAP_C_CCBS_DATA					mapP_gsm_sm(\x87\x6F)	/* 1007 */	/* GSM SS */
#define OMP_O_MAP_C_CCBS_FEATURE_LIST				mapP_gsm_sm(\x87\x71)	/* 1009 */	/* GSM SS */
#define OMP_O_MAP_C_CCBS_FEATURE				mapP_gsm_sm(\x87\x70)	/* 1008 */	/* GSM SS */
#define OMP_O_MAP_C_CCBS_INDICATORS				mapP_gsm_ch(\x87\x6C)	/* 1004 */	/* GSM CH */
#define OMP_O_MAP_C_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_OR_LAI	mapP_gsm(\x8F\x52)	/* 2002 */	/* GSM */
#define OMP_O_MAP_C_CHECK_IMEI_ARG				mapP_gsm_ms(\x88\x02)	/* 1026 */	/* GSM MS */
#define OMP_O_MAP_C_CHECK_IMEI_RES				mapP_gsm_ms(\x88\x03)	/* 1027 */	/* GSM MS */
#define OMP_O_MAP_C_CHOSEN_RADIO_RESOURCE_INFORMATION		mapP_gsm_ms(\x88\x04)	/* 1028 */	/* GSM MS */
#define OMP_O_MAP_C_CLOSE_ARGUMENT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_CODEC_LIST					mapP_gsm_ms(\x88\x05)	/* 1029 */	/* GSM MS */
#define OMP_O_MAP_C_CONTEXT_ID_LIST				mapP_gsm_ms(\x88\x06)	/* 1030 */	/* GSM MS */
#define OMP_O_MAP_C_CONTEXT					mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_CSG_SUBSCRIPTION_DATA_LIST			mapP_gsm_ms(\x88\x07)	/* 1031 */	/* GSM MS */
#define OMP_O_MAP_C_CSG_SUBSCRIPTION_DATA			mapP_gsm_ms(\x88\x08)	/* 1032 */	/* GSM MS */
#define OMP_O_MAP_C_CUG_CHECK_INFO				mapP_gsm_ch(\x87\x6D)	/* 1005 */	/* GSM CH */
#define OMP_O_MAP_C_CUG_FEATURE_LIST				mapP_gsm_ms(\x88\x09)	/* 1033 */	/* GSM MS */
#define OMP_O_MAP_C_CUG_FEATURE					mapP_gsm_ms(\x88\x0A)	/* 1034 */	/* GSM MS */
#define OMP_O_MAP_C_CUG_INFO					mapP_gsm_ms(\x88\x0B)	/* 1035 */	/* GSM MS */
#define OMP_O_MAP_C_CUG_REJECT_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_CUG_SUBSCRIPTION_LIST			mapP_gsm_ms(\x88\x0C)	/* 1036 */	/* GSM MS */
#define OMP_O_MAP_C_CUG_SUBSCRIPTION				mapP_gsm_ms(\x88\x0D)	/* 1037 */	/* GSM MS */
#define OMP_O_MAP_C_CURRENT_SECURITY_CONTEXT			mapP_gsm_ms(\x88\x0E)	/* 1038 */	/* GSM MS */
#define OMP_O_MAP_C_DATA_MISSING_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_DATA_MISSING_PARAM				mapP_gsm(\x90\x35)	/* 2101 */	/* GSM */
#define OMP_O_MAP_C_D_CSI					mapP_gsm_ms(\x88\x0F)	/* 1039 */	/* GSM MS */
#define OMP_O_MAP_C_DEACTIVATE_SS_ARG				mapP_gsm_sm(\x87\x72)	/* 1010 */	/* GSM SS */
#define OMP_O_MAP_C_DEACTIVATE_SS_RES				mapP_gsm_sm(\x87\x73)	/* 1011 */	/* GSM SS */
#define OMP_O_MAP_C_DEACTIVATE_TRACE_MODE_ARG			mapP_gsm_om(\x87\x6B)	/* 1003 */	/* GSM OM */
#define OMP_O_MAP_C_DEACTIVATE_TRACE_MODE_RES			mapP_gsm_om(\x87\x6C)	/* 1004 */	/* GSM OM */
#define OMP_O_MAP_C_DEFERRED_MT_LR_DATA				mapP_gsm_ls(\x87\x6C)	/* 1004 */	/* GSM LS */
#define OMP_O_MAP_C_DELETE_SUBSCRIBER_DATA_ARG			mapP_gsm_ms(\x88\x10)	/* 1040 */	/* GSM MS */
#define OMP_O_MAP_C_DELETE_SUBSCRIBER_DATA_RES			mapP_gsm_ms(\x88\x11)	/* 1041 */	/* GSM MS */
#define OMP_O_MAP_C_DESTINATION_NUMBER_CRITERIA			mapP_gsm_ms(\x88\x12)	/* 1042 */	/* GSM MS */
#define OMP_O_MAP_C_DESTINATION_NUMBER_LENGTH_LIST		mapP_gsm_ms(\x88\x13)	/* 1043 */	/* GSM MS */
#define OMP_O_MAP_C_DESTINATION_NUMBER_LIST			mapP_gsm_ms(\x88\x14)	/* 1044 */	/* GSM MS */
#define OMP_O_MAP_C_DISPATCHER_LIST				mapP_gsm_sm(\xC6\x44)	/* 9028 */	/* GSM SM */
#define OMP_O_MAP_C_DP_ANALYSED_INFO_CRITERIA_LIST		mapP_gsm_ms(\x88\x15)	/* 1045 */	/* GSM MS */
#define OMP_O_MAP_C_DP_ANALYSED_INFO_CRITERIUM			mapP_gsm_ms(\x88\x16)	/* 1046 */	/* GSM MS */
#define OMP_O_MAP_C_EMLPP_INFO					mapP_gsm(\x8F\x53)	/* 2003 */	/* GSM */
#define OMP_O_MAP_C_EPS_AUTHENTICATION_SET_LIST			mapP_gsm_ms(\x88\x17)	/* 1047 */	/* GSM MS */
#define OMP_O_MAP_C_EPS_AV					mapP_gsm_ms(\x88\x18)	/* 1048 */	/* GSM MS */
#define OMP_O_MAP_C_EPS_DATA_LIST				mapP_gsm_ms(\x88\x19)	/* 1201 */	/* GSM MS */
#define OMP_O_MAP_C_EPS_INFO					mapP_gsm_ms(\x88\x19)	/* 1049 */	/* GSM MS */
#define OMP_O_MAP_C_EPS_QOS_SUBSCRIBED				mapP_gsm_ms(\x88\x1A)	/* 1050 */	/* GSM MS */
#define OMP_O_MAP_C_EPS_SUBSCRIPTION_DATA			mapP_gsm_ms(\x88\x1B)	/* 1051 */	/* GSM MS */
#define OMP_O_MAP_C_EPS_SUBSCRIPTION_DATA_WITHDRAW		mapP_gsm_ms(\x88\x1C)	/* 1052 */	/* GSM MS */
#define OMP_O_MAP_C_ERASE_CC_ENTRY_ARG				mapP_gsm_sm(\x87\x74)	/* 1012 */	/* GSM SS */
#define OMP_O_MAP_C_ERASE_CC_ENTRY_RES				mapP_gsm_sm(\x87\x75)	/* 1013 */	/* GSM SS */
#define OMP_O_MAP_C_ERASE_SS_ARG				mapP_gsm_sm(\x87\x76)	/* 1014 */	/* GSM SS */
#define OMP_O_MAP_C_ERASE_SS_RES				mapP_gsm_sm(\x87\x77)	/* 1015 */	/* GSM SS */
#define OMP_O_MAP_C_ERROR					mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_EVENT_REPORT_DATA				mapP_gsm_ch(\x87\x6E)	/* 1006 */	/* GSM CH */
#define OMP_O_MAP_C_EXT_BASIC_SERVICE_CODE			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM */
#define OMP_O_MAP_C_EXT_BASIC_SERVICE_GROUP_LIST		mapP_gsm_ms(\x88\x1D)	/* 1053 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_CALL_BAR_FEATURE_LIST			mapP_gsm_ms(\x88\x1E)	/* 1054 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_CALL_BAR_INFO				mapP_gsm_ms(\x88\x1F)	/* 1055 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_CALL_BARRING_FEATURE			mapP_gsm_ms(\x88\x20)	/* 1056 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_CALL_BARRING_INFO_FOR_CSE		mapP_gsm_ms(\x88\x21)	/* 1057 */	/* GSM MS */
#define OMP_O_MAP_C_EXTENDED_ROUTING_INFO			mapP_gsm_ch(\x87\x6F)	/* 1007 */	/* GSM CH */
#define OMP_O_MAP_C_EXTENSIBLE_CALL_BARRED_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_EXTENSIBLE_SYSTEM_FAILURE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_EXTENSIBLE_SYSTEM_FAILURE_PARAM		mapP_gsm(\x90\x36)	/* 2102 */	/* GSM */
#define OMP_O_MAP_C_EXTENSION_CONTAINER				mapP_gsm(\x8F\x54)	/* 2004 */	/* GSM */
#define OMP_O_MAP_C_EXTENSION					mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_EXTERNAL_CLIENT_LIST			mapP_gsm_ms(\x88\x22)	/* 1058 */	/* GSM MS */
#define OMP_O_MAP_C_EXTERNAL_CLIENT				mapP_gsm_ms(\x88\x23)	/* 1059 */	/* GSM MS */
#define OMP_O_MAP_C_EXTERNAL_SIGNAL_INFO			mapP_gsm(\x8F\x55)	/* 2005 */	/* GSM */
#define OMP_O_MAP_C_EXT_EXTERNAL_CLIENT_LIST			mapP_gsm_ms(\x88\x24)	/* 1060 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_EXTERNAL_SIGNAL_INFO			mapP_gsm(\x8F\x56)	/* 2006 */	/* GSM */
#define OMP_O_MAP_C_EXT_FORWARDING_INFO_FOR_CSE			mapP_gsm_ms(\x88\x25)	/* 1061 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_FORW_FEATURE_LIST			mapP_gsm_ms(\x88\x26)	/* 1062 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_FORW_FEATURE				mapP_gsm_ms(\x88\x27)	/* 1063 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_FORW_INFO				mapP_gsm_ms(\x88\x28)	/* 1064 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_SS_DATA					mapP_gsm_ms(\x88\x29)	/* 1065 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_SS_INFO_FOR_CSE				mapP_gsm_ms(\x88\x2A)	/* 1066 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_SS_INFO_LIST				mapP_gsm_ms(\x88\x2B)	/* 1067 */	/* GSM MS */
#define OMP_O_MAP_C_EXT_SS_INFO					mapP_gsm_ms(\x88\x2C)	/* 1068 */	/* GSM MS */
#define OMP_O_MAP_C_FACILITY_NOT_SUP_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_FACILITY_NOT_SUP_PARAM			mapP_gsm(\x90\x37)	/* 2103 */	/* GSM */
#define OMP_O_MAP_C_FAILURE_REPORT_ARG				mapP_gsm_ms(\x88\x2D)	/* 1069 */	/* GSM MS */
#define OMP_O_MAP_C_FAILURE_REPORT_RES				mapP_gsm_ms(\x88\x2E)	/* 1070 */	/* GSM MS */
#define OMP_O_MAP_C_FORWARD_ACCESS_SIGNALLING_ARG		mapP_gsm_ms(\x88\x2F)	/* 1071 */	/* GSM MS */
#define OMP_O_MAP_C_FORWARD_CHECK_SS_INDICATION_ARG		mapP_gsm_ms(\x88\x30)	/* 1072 */	/* GSM MS */
#define OMP_O_MAP_C_FORWARD_GROUP_CALL_SIGNALLING_ARG		mapP_gsm_gc(\x87\x69)	/* 1001 */	/* GSM GC */
#define OMP_O_MAP_C_FORWARDING_DATA				mapP_gsm_ch(\x87\x70)	/* 1008 */	/* GSM CH */
#define OMP_O_MAP_C_FORWARDING_FAILED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_FORWARDING_FEATURE_LIST			mapP_gsm_sm(\x87\x79)	/* 1017 */	/* GSM SS */
#define OMP_O_MAP_C_FORWARDING_FEATURE				mapP_gsm_sm(\x87\x78)	/* 1016 */	/* GSM SS */
#define OMP_O_MAP_C_FORWARDING_INFO				mapP_gsm_sm(\x87\x7A)	/* 1018 */	/* GSM SS */
#define OMP_O_MAP_C_FORWARDING_VIOLATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_GENERIC_SERVICE_ARGUMENT			mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_GENERIC_SERVICE_INFO			mapP_gsm_sm(\x87\x7B)	/* 1019 */	/* GSM SS */
#define OMP_O_MAP_C_GENERIC_SERVICE_RESULT			mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_GET_PASSWORD_ARG				mapP_gsm_sm(\x87\x7C)	/* 1020 */	/* GSM SS */
#define OMP_O_MAP_C_GET_PASSWORD_RES				mapP_gsm_sm(\x87\x7D)	/* 1021 */	/* GSM SS */
#define OMP_O_MAP_C_GMLC_LIST					mapP_gsm_ms(\x88\x31)	/* 1073 */	/* GSM MS */
#define OMP_O_MAP_C_GMSC_CAMEL_SUBSCRIPTION_INFO		mapP_gsm_ch(\x87\x71)	/* 1009 */	/* GSM CH */
#define OMP_O_MAP_C_GPRS_CAMEL_TDP_DATA_LIST			mapP_gsm_ms(\x88\x32)	/* 1074 */	/* GSM MS */
#define OMP_O_MAP_C_GPRS_CSI					mapP_gsm_ms(\x88\x33)	/* 1075 */	/* GSM MS */
#define OMP_O_MAP_C_GPRS_DATA_LIST				mapP_gsm_ms(\x88\x34)	/* 1076 */	/* GSM MS */
#define OMP_O_MAP_C_GPRS_MS_CLASS				mapP_gsm_ms(\x88\x35)	/* 1077 */	/* GSM MS */
#define OMP_O_MAP_C_GPRS_SUBSCRIPTION_DATA			mapP_gsm_ms(\x88\x36)	/* 1078 */	/* GSM MS */
#define OMP_O_MAP_C_GPRS_SUBSCRIPTION_DATA_WITHDRAW		mapP_gsm_ms(\x88\x37)	/* 1079 */	/* GSM MS */
#define OMP_O_MAP_C_GRPS_CAMEL_TDP_DATA				mapP_gsm_ms(\x88\x38)	/* 1080 */	/* GSM MS */
#define OMP_O_MAP_C_GSM_ABORT_ARGUMENT				mapP_gsm(\x8F\x57)	/* 2007 */	/* GSM */
#define OMP_O_MAP_C_GSM_ABORT_RESULT				mapP_gsm(\x8F\x58)	/* 2008 */	/* GSM */
#define OMP_O_MAP_C_GSM_ACCEPT_RESULT				mapP_gsm(\x8F\x59)	/* 2009 */	/* GSM */
#define OMP_O_MAP_C_GSM_CLOSE_ARGUMENT				mapP_gsm(\x8F\x5A)	/* 2010 */	/* GSM */
#define OMP_O_MAP_C_GSM_CONTEXT					mapP_gsm(\x8F\x5B)	/* 2011 */	/* GSM */
#define OMP_O_MAP_C_GSM_MAP_ASSOC_ARGS				mapP_gsm(\x8F\x5C)	/* 2012 */	/* GSM */
#define OMP_O_MAP_C_GSM_NOTICE_RESULT				mapP_gsm(\x8F\x5D)	/* 2013 */	/* GSM */
#define OMP_O_MAP_C_GSM_OPEN_ARGUMENT				mapP_gsm(\x8F\x5E)	/* 2014 */	/* GSM */
#define OMP_O_MAP_C_GSM_P_ABORT_RESULT				mapP_gsm(\x8F\x5F)	/* 2015 */	/* GSM */
#define OMP_O_MAP_C_GSM_REFUSE_RESULT				mapP_gsm(\x8F\x60)	/* 2016 */	/* GSM */
#define OMP_O_MAP_C_GSM_RELEASE_ARGUMENT			mapP_gsm(\x8F\x61)	/* 2017 */	/* GSM */
#define OMP_O_MAP_C_GSM_RELEASE_RESULT				mapP_gsm(\x8F\x62)	/* 2018 */	/* GSM */
#define OMP_O_MAP_C_GSM_SECURITY_CONTEXT_DATA			mapP_gsm_ms(\x88\x39)	/* 1081 */	/* GSM MS */
#define OMP_O_MAP_C_GSM_SERVICE_ARGUMENT			mapP_gsm(\x8F\x63)	/* 2019 */	/* GSM */
#define OMP_O_MAP_C_GSM_SERVICE_ERROR				mapP_gsm(\x8F\x64)	/* 2020 */	/* GSM */
#define OMP_O_MAP_C_GSM_SERVICE_REJECT				mapP_gsm(\x8F\x55)	/* 2021 */	/* GSM */
#define OMP_O_MAP_C_GSM_SERVICE_RESULT				mapP_gsm(\x8F\x66)	/* 2022 */	/* GSM */
#define OMP_O_MAP_C_HLR_LIST					mapP_gsm(\x8F\x67)	/* 2023 */	/* GSM */
#define OMP_O_MAP_C_IDENTITY					mapP_gsm(\x88\x3A)	/* 1082 */	/* GSM */
#define OMP_O_MAP_C_ILLEGAL_EQUIPMENT_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ILLEGAL_SS_OPERATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ILLEGAL_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_IMSI_WITH_LMSI				mapP_gsm(\x88\x3B)	/* 1083 */	/* GSM */
#define OMP_O_MAP_C_INCOMPATIBLE_TERMINAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_INCOMPATIBLE_TERMINAL_PARAM			mapP_gsm(\x90\x38)	/* 2104 */	/* GSM */
#define OMP_O_MAP_C_INFORMATION_NOT_AVAILABLE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_INFORM_SERVICE_CENTRE_ARG			mapP_gsm_sm(\xC6\x2D)	/* 9005 */	/* GSM SM */
#define OMP_O_MAP_C_INSERT_SUBSCRIBER_DATA_ARG			mapP_gsm_ms(\x88\x3C)	/* 1084 */	/* GSM MS */
#define OMP_O_MAP_C_INSERT_SUBSCRIBER_DATA_RES			mapP_gsm_ms(\x88\x3D)	/* 1085 */	/* GSM MS */
#define OMP_O_MAP_C_INTERROGATE_SS_ARG				mapP_gsm_sm(\x87\x7E)	/* 1022 */	/* GSM SS */
#define OMP_O_MAP_C_INTERROGATE_SS_RES				mapP_gsm_sm(\x88\x0F)	/* 1023 */	/* GSM SS */
#define OMP_O_MAP_C_IST_ALERT_ARG				mapP_gsm_ch(\x87\x72)	/* 1010 */	/* GSM CH */
#define OMP_O_MAP_C_IST_ALERT_RES				mapP_gsm_ch(\x87\x73)	/* 1011 */	/* GSM CH */
#define OMP_O_MAP_C_IST_COMMAND_ARG				mapP_gsm_ch(\x87\x74)	/* 1012 */	/* GSM CH */
#define OMP_O_MAP_C_IST_COMMAND_RES				mapP_gsm_ch(\x87\x75)	/* 1013 */	/* GSM CH */
#define OMP_O_MAP_C_LCS_CLIENT_EXTERNAL_ID			mapP_gsm(\x8F\x68)	/* 2024 */	/* GSM */
#define OMP_O_MAP_C_LCS_CLIENT_ID				mapP_gsm_ls(\x87\x6D)	/* 1005 */	/* GSM LS */
#define OMP_O_MAP_C_LCS_CLIENT_NAME				mapP_gsm_ls(\x87\x6E)	/* 1006 */	/* GSM LS */
#define OMP_O_MAP_C_LCS_CODEWORD				mapP_gsm_ls(\x87\x6F)	/* 1007 */	/* GSM LS */
#define OMP_O_MAP_C_LCS_INFORMATION				mapP_gsm_ms(\x88\x3E)	/* 1086 */	/* GSM MS */
#define OMP_O_MAP_C_LCS_LOCATION_INFO				mapP_gsm_ls(\x87\x70)	/* 1008 */	/* GSM LS */
#define OMP_O_MAP_C_LCS_PRIVACY_CHECK				mapP_gsm_ls(\x87\x71)	/* 1009 */	/* GSM LS */
#define OMP_O_MAP_C_LCS_PRIVACY_CLASS				mapP_gsm_ms(\x88\x3F)	/* 1087 */	/* GSM MS */
#define OMP_O_MAP_C_LCS_PRIVACY_EXCEPTION_LIST			mapP_gsm_ms(\x88\x40)	/* 1088 */	/* GSM MS */
#define OMP_O_MAP_C_LCS_QOS					mapP_gsm_ls(\x87\x72)	/* 1010 */	/* GSM LS */
#define OMP_O_MAP_C_LCS_REQUESTOR_ID				mapP_gsm_ls(\x87\x73)	/* 1011 */	/* GSM LS */
#define OMP_O_MAP_C_LOCATION_AREA				mapP_gsm_ms(\x88\x41)	/* 1089 */	/* GSM MS */
#define OMP_O_MAP_C_LOCATION_INFORMATION_GPRS			mapP_gsm_ms(\x88\x42)	/* 1090 */	/* GSM MS */
#define OMP_O_MAP_C_LOCATION_INFORMATION			mapP_gsm_ms(\x88\x43)	/* 1091 */	/* GSM MS */
#define OMP_O_MAP_C_LOCATION_INFO_WITH_LMSI			mapP_gsm_sm(\xC6\x2E)	/* 9006 */	/* GSM SM */
#define OMP_O_MAP_C_LOCATION_TYPE				mapP_gsm_ls(\x87\x74)	/* 1012 */	/* GSM LS */
#define OMP_O_MAP_C_LONG_TERM_DENIAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_LSA_DATA_LIST				mapP_gsm_ms(\x88\x44)	/* 1092 */	/* GSM MS */
#define OMP_O_MAP_C_LSA_DATA					mapP_gsm_ms(\x88\x45)	/* 1093 */	/* GSM MS */
#define OMP_O_MAP_C_LSA_IDENTITY_LIST				mapP_gsm_ms(\x88\x46)	/* 1094 */	/* GSM MS */
#define OMP_O_MAP_C_LSA_INFORMATION				mapP_gsm_ms(\x88\x47)	/* 1095 */	/* GSM MS */
#define OMP_O_MAP_C_LSA_INFORMATION_WITHDRAW			mapP_gsm_ms(\x88\x48)	/* 1096 */	/* GSM MS */
#define OMP_O_MAP_C_MAP_ASSOC_ARGS				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_M_CSI					mapP_gsm_ms(\x88\x49)	/* 1097 */	/* GSM MS */
#define OMP_O_MAP_C_MC_SS_INFO					mapP_gsm(\x8F\x69)	/* 2025 */	/* GSM */
#define OMP_O_MAP_C_MESSAGE_WAIT_LIST_FULL_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_MG_CSI					mapP_gsm_ms(\x88\x4A)	/* 1098 */	/* GSM MS */
#define OMP_O_MAP_C_MM_EVENT_NOT_SUPPORTED_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_MNP_INFO_RES				mapP_gsm_ms(\x88\x4B)	/* 1099 */	/* GSM MS */
#define OMP_O_MAP_C_MOBILITY_TRIGGERS				mapP_gsm_ms(\x88\x4C)	/* 1100 */	/* GSM MS */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CB_INFO		mapP_gsm_ms(\x88\x4E)	/* 1102 */	/* GSM MS */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CF_INFO		mapP_gsm_ms(\x88\x4F)	/* 1103 */	/* GSM MS */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CSI		mapP_gsm_ms(\x88\x50)	/* 1104 */	/* GSM MS */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_IP_SM_GW_DATA	mapP_gsm_ms(\x88\x4D)	/* 1101 */	/* GSM MS */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_ODB_DATA		mapP_gsm_ms(\x88\x51)	/* 1105 */	/* GSM MS */
#define OMP_O_MAP_C_MO_FORWARD_SM_ARG				mapP_gsm_sm(\xC6\x30)	/* 9008 */	/* GSM SM */
#define OMP_O_MAP_C_MO_FORWARD_SM_RES				mapP_gsm_sm(\xC6\x31)	/* 9009 */	/* GSM SM */
#define OMP_O_MAP_C_MOLR_CLASS					mapP_gsm_ms(\x88\x52)	/* 1106 */	/* GSM MS */
#define OMP_O_MAP_C_MOLR_LIST					mapP_gsm_ms(\x88\x53)	/* 1107 */	/* GSM MS */
#define OMP_O_MAP_C_MSISDN_BS_LIST				mapP_gsm_ms(\x88\x54)	/* 1108 */	/* GSM MS */
#define OMP_O_MAP_C_MSISDN_BS					mapP_gsm_ms(\x88\x55)	/* 1109 */	/* GSM MS */
#define OMP_O_MAP_C_MT_FORWARD_SM_ARG				mapP_gsm_sm(\xC6\x32)	/* 9010 */	/* GSM SM */
#define OMP_O_MAP_C_MT_FORWARD_SM_RES				mapP_gsm_sm(\xC6\x33)	/* 9011 */	/* GSM SM */
#define OMP_O_MAP_C_MT_FORWARD_SM_VGCS_ARG			mapP_gsm_sm(\xC6\x42)	/* 9026 */	/* GSM SM */
#define OMP_O_MAP_C_MT_FORWARD_SM_VGCS_RES			mapP_gsm_sm(\xC6\x43)	/* 9027 */	/* GSM SM */
#define OMP_O_MAP_C_MT_SMS_CAMEL_TDP_CRITERIA_LIST		mapP_gsm_ms(\x88\x56)	/* 1110 */	/* GSM MS */
#define OMP_O_MAP_C_MT_SMS_CAMEL_TDP_CRITERIA			mapP_gsm_ms(\x88\x57)	/* 1111 */	/* GSM MS */
#define OMP_O_MAP_C_NAEA_PREFERRED_CI				mapP_gsm(\x8F\x6A)	/* 2026 */	/* GSM */
#define OMP_O_MAP_C_NO_GROUP_CALL_NB_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NO_ROAMING_NB_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NO_SUBSCRIBER_REPLY_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NOTE_MM_EVENT_ARG				mapP_gsm_ms(\x88\x58)	/* 1112 */	/* GSM MS */
#define OMP_O_MAP_C_NOTE_MM_EVENT_RES				mapP_gsm_ms(\x88\x59)	/* 1113 */	/* GSM MS */
#define OMP_O_MAP_C_NOTE_MS_PRESENT_FOR_GPRS_ARG		mapP_gsm_ms(\x88\x5A)	/* 1114 */	/* GSM MS */
#define OMP_O_MAP_C_NOTE_MS_PRESENT_FOR_GPRS_RES		mapP_gsm_ms(\x88\x5B)	/* 1115 */	/* GSM MS */
#define OMP_O_MAP_C_NOTE_SUBSCRIBER_DATA_MODIFIED_ARG		mapP_gsm_ms(\x88\x5C)	/* 1116 */	/* GSM MS */
#define OMP_O_MAP_C_NOTE_SUBSCRIBER_DATA_MODIFIED_RES		mapP_gsm_ms(\x88\x5D)	/* 1117 */	/* GSM MS */
#define OMP_O_MAP_C_NOTICE_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_NOTIFY_USSD_ARG				mapP_gsm_sm(\x88\x00)	/* 1024 */	/* GSM SS */
#define OMP_O_MAP_C_NOTIFY_USSD_RES				mapP_gsm_sm(\x88\x01)	/* 1025 */	/* GSM SS */
#define OMP_O_MAP_C_NUMBER_CHANGE_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_O_BCSM_CAMEL_TDP_CRITERIA_LIST		mapP_gsm_ms(\x88\x5E)	/* 1118 */	/* GSM MS */
#define OMP_O_MAP_C_O_BCSM_CAMEL_TDP_CRITERIA			mapP_gsm_ms(\x88\x5F)	/* 1119 */	/* GSM MS */
#define OMP_O_MAP_C_O_BCSM_CAMEL_TDP_DATA_LIST			mapP_gsm_ms(\x88\x60)	/* 1120 */	/* GSM MS */
#define OMP_O_MAP_C_O_BCSM_CAMEL_TDP_DATA			mapP_gsm_ms(\x88\x61)	/* 1121 */	/* GSM MS */
#define OMP_O_MAP_C_O_CAUSE_VALUE_CRITERIA			mapP_gsm_ms(\x88\x62)	/* 1122 */	/* GSM MS */
#define OMP_O_MAP_C_O_CSI					mapP_gsm_ms(\x88\x63)	/* 1123 */	/* GSM MS */
#define OMP_O_MAP_C_ODB_DATA					mapP_gsm_ms(\x88\x64)	/* 1124 */	/* GSM MS */
#define OMP_O_MAP_C_ODB_INFO					mapP_gsm_ms(\x88\x65)	/* 1125 */	/* GSM MS */
#define OMP_O_MAP_C_OPEN_ARGUMENT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_OPERATION_ARGUMENT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_OPERATION_ERROR				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_OPERATION_REJECT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_OPERATION_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_OR_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_P_ABORT_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_PAGING_AREA					mapP_gsm_ms(\x88\x66)	/* 1126 */	/* GSM MS */
#define OMP_O_MAP_C_PCS_EXTENSIONS				mapP_gsm(\x8F\x6B)	/* 2027 */	/* GSM */
#define OMP_O_MAP_C_PDN_GW_IDENTITY				mapP_gsm_ms(\x88\x67)	/* 1127 */	/* GSM MS */
#define OMP_O_MAP_C_PDN_GW_UPDATE				mapP_gsm_ms(\x88\x68)	/* 1128 */	/* GSM MS */
#define OMP_O_MAP_C_PDP_CONTEXT_INFO_LIST			mapP_gsm_ms(\x88\x69)	/* 1129 */	/* GSM MS */
#define OMP_O_MAP_C_PDP_CONTEXT_INFO				mapP_gsm_ms(\x88\x6A)	/* 1130 */	/* GSM MS */
#define OMP_O_MAP_C_PDP_CONTEXT					mapP_gsm_ms(\x88\x6B)	/* 1131 */	/* GSM MS */
#define OMP_O_MAP_C_PERIODIC_LDR_INFO				mapP_gsm_ls(\x87\x75)	/* 1013 */	/* GSM LS */
#define OMP_O_MAP_C_PLMN_CLIENT_LIST				mapP_gsm_ms(\x88\x6C)	/* 1132 */	/* GSM MS */
#define OMP_O_MAP_C_PLMN_LIST					mapP_gsm_ls(\x87\x76)	/* 1014 */	/* GSM LS */
#define OMP_O_MAP_C_POSITION_METHOD_FAILURE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_PREPARE_GROUP_CALL_ARG			mapP_gsm_gc(\x87\x6A)	/* 1002 */	/* GSM GC */
#define OMP_O_MAP_C_PREPARE_GROUP_CALL_RES			mapP_gsm_gc(\x87\x6B)	/* 1003 */	/* GSM GC */
#define OMP_O_MAP_C_PREPARE_HO_ARG				mapP_gsm_ms(\x88\x6D)	/* 1133 */	/* GSM MS */
#define OMP_O_MAP_C_PREPARE_HO_RES				mapP_gsm_ms(\x88\x6E)	/* 1134 */	/* GSM MS */
#define OMP_O_MAP_C_PREPARE_SUBSEQUENT_HO_ARG			mapP_gsm_ms(\x88\x6F)	/* 1135 */	/* GSM MS */
#define OMP_O_MAP_C_PREPARE_SUBSEQUENT_HO_RES			mapP_gsm_ms(\x88\x70)	/* 1136 */	/* GSM MS */
#define OMP_O_MAP_C_PRESENTATION_CONTEXT			mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_PRESENTATION_LAYER_ARGS			mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_PRIVATE_EXTENSION_LIST			mapP_gsm(\x8F\x6C)	/* 2028 */	/* GSM */
#define OMP_O_MAP_C_PRIVATE_EXTENSION				mapP_gsm(\x8F\x6D)	/* 2029 */	/* GSM */
#define OMP_O_MAP_C_PROCESS_ACCESS_SIGNALLING_ARG		mapP_gsm_ms(\x88\x71)	/* 1137 */	/* GSM MS */
#define OMP_O_MAP_C_PROCESS_GROUP_CALL_SIGNALLING_ARG		mapP_gsm_gc(\x87\x6C)	/* 1004 */	/* GSM GC */
#define OMP_O_MAP_C_PROCESS_USSD_ARG				mapP_gsm_sm(\x88\x02)	/* 1026 */	/* GSM SS */
#define OMP_O_MAP_C_PROCESS_USSD_RES				mapP_gsm_sm(\x88\x03)	/* 1027 */	/* GSM SS */
#define OMP_O_MAP_C_PROPRIETARY_ARGS				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_PROVIDE_ROAMING_NUMBER_ARG			mapP_gsm_ch(\x87\x76)	/* 1014 */	/* GSM CH */
#define OMP_O_MAP_C_PROVIDE_ROAMING_NUMBER_RES			mapP_gsm_ch(\x87\x77)	/* 1015 */	/* GSM CH */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_INFO_ARG			mapP_gsm_ms(\x88\x72)	/* 1138 */	/* GSM MS */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_INFO_RES			mapP_gsm_ms(\x88\x73)	/* 1139 */	/* GSM MS */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_LOCATION_ARG		mapP_gsm_ls(\x87\x77)	/* 1015 */	/* GSM LS */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_LOCATION_RES		mapP_gsm_ls(\x87\x78)	/* 1016 */	/* GSM LS */
#define OMP_O_MAP_C_PS_SUBSCRIBER_STATE				mapP_gsm_ms(\x88\x74)	/* 1140 */	/* GSM MS */
#define OMP_O_MAP_C_PURGE_MS_ARG				mapP_gsm_ms(\x88\x75)	/* 1141 */	/* GSM MS */
#define OMP_O_MAP_C_PURGE_MS_RES				mapP_gsm_ms(\x88\x76)	/* 1142 */	/* GSM MS */
#define OMP_O_MAP_C_PW_REGISTRATION_FAILURE_CAUSE		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_QUINTUPLET_LIST				mapP_gsm_ms(\x88\x77)	/* 1143 */	/* GSM MS */
#define OMP_O_MAP_C_RADIO_RESOURCE_LIST				mapP_gsm_ms(\x88\x78)	/* 1144 */	/* GSM MS */
#define OMP_O_MAP_C_RADIO_RESOURCE				mapP_gsm_ms(\x88\x79)	/* 1145 */	/* GSM MS */
#define OMP_O_MAP_C_READY_FOR_SM_ARG				mapP_gsm_sm(\xC6\x34)	/* 9012 */	/* GSM SM */
#define OMP_O_MAP_C_READY_FOR_SM_RES				mapP_gsm_sm(\xC6\x35)	/* 9013 */	/* GSM SM */
#define OMP_O_MAP_C_REFUSE_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_REGISTER_CC_ENTRY_ARG			mapP_gsm_sm(\x88\x04)	/* 1028 */	/* GSM SS */
#define OMP_O_MAP_C_REGISTER_CC_ENTRY_RES			mapP_gsm_sm(\x88\x05)	/* 1029 */	/* GSM SS */
#define OMP_O_MAP_C_REGISTER_PASSWORD_ARG			mapP_gsm_sm(\x88\x06)	/* 1030 */	/* GSM SS */
#define OMP_O_MAP_C_REGISTER_PASSWORD_RES			mapP_gsm_sm(\x88\x07)	/* 1031 */	/* GSM SS */
#define OMP_O_MAP_C_REGISTER_SS_ARG				mapP_gsm_sm(\x88\x08)	/* 1032 */	/* GSM SS */
#define OMP_O_MAP_C_REGISTER_SS_RES				mapP_gsm_sm(\x88\x09)	/* 1033 */	/* GSM SS */
#define OMP_O_MAP_C_RELEASE_ARGUMENT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_RELEASE_RESOURCES_ARG			mapP_gsm_ch(\x87\x78)	/* 1016 */	/* GSM CH */
#define OMP_O_MAP_C_RELEASE_RESOURCES_RES			mapP_gsm_ch(\x87\x79)	/* 1017 */	/* GSM CH */
#define OMP_O_MAP_C_RELEASE_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_RELOCATION_NUMBER_LIST			mapP_gsm_ms(\x88\x7A)	/* 1146 */	/* GSM MS */
#define OMP_O_MAP_C_RELOCATION_NUMBER				mapP_gsm_ms(\x88\x7B)	/* 1147 */	/* GSM MS */
#define OMP_O_MAP_C_REMOTE_USER_FREE_ARG			mapP_gsm_ch(\x87\x7A)	/* 1018 */	/* GSM CH */
#define OMP_O_MAP_C_REMOTE_USER_FREE_RES			mapP_gsm_ch(\x87\x7B)	/* 1019 */	/* GSM CH */
#define OMP_O_MAP_C_REPORTING_PLMN_LIST				mapP_gsm_ls(\x87\x7A)	/* 1018 */	/* GSM LS */
#define OMP_O_MAP_C_REPORTING_PLMN				mapP_gsm_ls(\x87\x79)	/* 1007 */	/* GSM LS */
#define OMP_O_MAP_C_REPORT_SM_DELIVERY_STATUS_ARG		mapP_gsm_sm(\xC6\x36)	/* 9014 */	/* GSM SM */
#define OMP_O_MAP_C_REPORT_SM_DELIVERY_STATUS_RES		mapP_gsm_sm(\xC6\x37)	/* 9015 */	/* GSM SM */
#define OMP_O_MAP_C_REQUESTED_INFO				mapP_gsm_ms(\x88\x7C)	/* 1148 */	/* GSM MS */
#define OMP_O_MAP_C_REQUESTED_SUBSCRIPTION_INFO			mapP_gsm_ms(\x88\x7D)	/* 1149 */	/* GSM MS */
#define OMP_O_MAP_C_RESET_ARG					mapP_gsm_ms(\x88\x7E)	/* 1150 */	/* GSM MS */
#define OMP_O_MAP_C_RESOURCE_LIMITATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_RESOURCE_LIMITATION_PARAM			mapP_gsm(\x90\x39)	/* 2105 */	/* GSM */
#define OMP_O_MAP_C_RESPONSE_TIME				mapP_gsm_ls(\x87\x7B)	/* 1019 */	/* GSM LS */
#define OMP_O_MAP_C_RESTORE_DATA_ARG				mapP_gsm_ms(\x88\x7F)	/* 1151 */	/* GSM MS */
#define OMP_O_MAP_C_RESTORE_DATA_RES				mapP_gsm_ms(\x89\x00)	/* 1152 */	/* GSM MS */
#define OMP_O_MAP_C_RESUME_CALL_HANDLING_ARG			mapP_gsm_ch(\x87\x7C)	/* 1020 */	/* GSM CH */
#define OMP_O_MAP_C_RESUME_CALL_HANDLING_RES			mapP_gsm_ch(\x87\x7D)	/* 1021 */	/* GSM CH */
#define OMP_O_MAP_C_RESYNCHRONISATION_INFO			mapP_gsm_ms(\x89\x01)	/* 1153 */	/* GSM MS */
#define OMP_O_MAP_C_ROAMING_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_LCS_ARG			mapP_gsm_ls(\x87\x7C)	/* 1020 */	/* GSM LS */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_LCS_RES			mapP_gsm_ls(\x87\x7D)	/* 1021 */	/* GSM LS */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_SM_ARG			mapP_gsm_sm(\xC6\x38)	/* 9016 */	/* GSM SM */
#define OMP_O_MAP_C_ROUTING_INFO_FOR_SM_RES			mapP_gsm_sm(\xC6\x39)	/* 9017 */	/* GSM SM */
#define OMP_O_MAP_C_ROUTING_INFO				mapP_gsm_ch(\x87\x7E)	/* 1022 */	/* GSM CH */
#define OMP_O_MAP_C_SECURE_TRANSPORT_ERROR_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SELECTED_UMTS_ALGORITHMS			mapP_gsm_ms(\x89\x02)	/* 1154 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_AUTHENTICATION_INFO_ARG		mapP_gsm_ms(\x89\x03)	/* 1155 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_AUTHENTICATION_INFO_RES		mapP_gsm_ms(\x89\x04)	/* 1156 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_END_SIGNAL_ARG				mapP_gsm_ms(\x89\x05)	/* 1157 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_END_SIGNAL_RES				mapP_gsm_ms(\x89\x06)	/* 1158 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_GROUP_CALL_END_SIGNAL_ARG		mapP_gsm_gc(\x87\x6D)	/* 1005 */	/* GSM GC */
#define OMP_O_MAP_C_SEND_GROUP_CALL_END_SIGNAL_RES		mapP_gsm_gc(\x87\x6E)	/* 1006 */	/* GSM GC */
#define OMP_O_MAP_C_SEND_GROUP_CALL_INFO_ARG			mapP_gsm_gc(\x87\x6F)	/* 1007 */	/* GSM GC */
#define OMP_O_MAP_C_SEND_GROUP_CALL_INFO_RES			mapP_gsm_gc(\x87\x70)	/* 1008 */	/* GSM GC */
#define OMP_O_MAP_C_SEND_IDENTIFICATION_ARG			mapP_gsm_ms(\x89\x07)	/* 1159 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_IDENTIFICATION_RES			mapP_gsm_ms(\x89\x08)	/* 1160 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_IMSI_ARG				mapP_gsm_om(\x87\x6D)	/* 1005 */	/* GSM OM */
#define OMP_O_MAP_C_SEND_IMSI_RES				mapP_gsm_om(\x87\x6E)	/* 1006 */	/* GSM OM */
#define OMP_O_MAP_C_SEND_INFO_FOR_MO_SMS_ARG			mapP_gsm_sm(\xC6\x3A)	/* 9018 */	/* GSM SM */
#define OMP_O_MAP_C_SEND_INFO_FOR_MO_SMS_RES			mapP_gsm_sm(\xC6\x3B)	/* 9019 */	/* GSM SM */
#define OMP_O_MAP_C_SEND_INFO_FOR_MT_SMS_ARG			mapP_gsm_sm(\xC6\x3C)	/* 9020 */	/* GSM SM */
#define OMP_O_MAP_C_SEND_INFO_FOR_MT_SMS_RES			mapP_gsm_sm(\xC6\x3D)	/* 9021 */	/* GSM SM */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_ARG			mapP_gsm_ch(\x87\x7F)	/* 1023 */	/* GSM CH */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_FOR_GPRS_ARG		mapP_gsm_ms(\x89\x09)	/* 1161 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_FOR_GPRS_RES		mapP_gsm_ms(\x89\x0A)	/* 1162 */	/* GSM MS */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_RES			mapP_gsm_ch(\x88\x00)	/* 1024 */	/* GSM CH */
#define OMP_O_MAP_C_SERVICE_ARGUMENT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_SERVICE_ERROR				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_SERVICE_REJECT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_SERVICE_RESULT				mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_SERVICE_TYPE_LIST				mapP_gsm_ms(\x89\x0B)	/* 1163 */	/* GSM MS */
#define OMP_O_MAP_C_SESSION					mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_SET_REPORTING_STATE_ARG			mapP_gsm_ch(\x88\x01)	/* 1025 */	/* GSM CH */
#define OMP_O_MAP_C_SET_REPORTING_STATE_RES			mapP_gsm_ch(\x88\x02)	/* 1026 */	/* GSM CH */
#define OMP_O_MAP_C_SETVICE_TYPE				mapP_gsm_ms(\x89\x0C)	/* 1164 */	/* GSM MS */
#define OMP_O_MAP_C_SGSN_CAMEL_SUBSCRIPTION_DATA		mapP_gsm_ms(\x89\x0D)	/* 1165 */	/* GSM MS */
#define OMP_O_MAP_C_SGSN_CAPABILITY				mapP_gsm_ms(\x89\x0E)	/* 1166 */	/* GSM MS */
#define OMP_O_MAP_C_SHORT_TERM_DENIAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SLR_ARG_EXTENSION_CONTAINER			mapP_gsm(\x8F\x6E)	/* 2030 */	/* GSM */
#define OMP_O_MAP_C_SLR_ARG_PCS_EXTENSIONS			mapP_gsm(\x8F\x6F)	/* 2031 */	/* GSM */
#define OMP_O_MAP_C_SM_DELIVERY_FAILURE_CAUSE			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SM_RP_DA					mapP_gsm_sm(\xC6\x3F)	/* 9023 */	/* GSM SM */
#define OMP_O_MAP_C_SM_RP_OA					mapP_gsm_sm(\xC6\x40)	/* 9024 */	/* GSM SM */
#define OMP_O_MAP_C_SMS_CAMEL_TDP_DATA_LIST			mapP_gsm_ms(\x89\x0F)	/* 1167 */	/* GSM MS */
#define OMP_O_MAP_C_SMS_CAMEL_TDP_DATA				mapP_gsm_ms(\x89\x10)	/* 1168 */	/* GSM MS */
#define OMP_O_MAP_C_SMS_CSI					mapP_gsm_ms(\x89\x11)	/* 1169 */	/* GSM MS */
#define OMP_O_MAP_C_SPECIFIC_INFORMATION			mapP_gsm(\x8F\x70)	/* 2032 */	/* GSM */
#define OMP_O_MAP_C_SS_CAMEL_DATA				mapP_gsm_ms(\x89\x12)	/* 1170 */	/* GSM MS */
#define OMP_O_MAP_C_SS_CSI					mapP_gsm_ms(\x89\x13)	/* 1171 */	/* GSM MS */
#define OMP_O_MAP_C_SS_DATA					mapP_gsm_sm(\x88\x0A)	/* 1034 */	/* GSM SS */
#define OMP_O_MAP_C_SS_EVENT_LIST				mapP_gsm_ms(\x89\x14)	/* 1172 */	/* GSM MS */
#define OMP_O_MAP_C_SS_EVENT_SPECIFICATION			mapP_gsm_sm(\x88\x0B)	/* 1035 */	/* GSM SS */
#define OMP_O_MAP_C_SS_FOR_BS_CODE				mapP_gsm_sm(\x88\x0C)	/* 1036 */	/* GSM SS */
#define OMP_O_MAP_C_SS_INCOMPATIBILITY_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SS_INFO					mapP_gsm_sm(\x88\x0D)	/* 1037 */	/* GSM SS */
#define OMP_O_MAP_C_SS_INVOCATION_NOTIFICATION_ARG		mapP_gsm_sm(\x88\x0E)	/* 1038 */	/* GSM SS */
#define OMP_O_MAP_C_SS_INVOCATION_NOTIFICATION_RES		mapP_gsm_sm(\x88\x0F)	/* 1039 */	/* GSM SS */
#define OMP_O_MAP_C_SS_LIST					mapP_gsm_sm(\x88\x10)	/* 1040 */	/* GSM SS */
#define OMP_O_MAP_C_SS_NOT_AVAILABLE_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SS_SUBSCRIPTION_OPTION			mapP_gsm_sm(\x88\x11)	/* 1041 */	/* GSM SS */
#define OMP_O_MAP_C_SS_SUBSCRIPTION_VIOLATION_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_STATE_ATTRIBUTES				mapP_gsm_gc(\x87\x71)	/* 1009 */	/* GSM GC */
#define OMP_O_MAP_C_STATUS_REPORT_ARG				mapP_gsm_ch(\x88\x03)	/* 1027 */	/* GSM CH */
#define OMP_O_MAP_C_STATUS_REPORT_RES				mapP_gsm_ch(\x88\x04)	/* 1028 */	/* GSM CH */
#define OMP_O_MAP_C_SUB_BUSY_FOR_MT_SMS_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SUBSCRIBER_DATA				mapP_gsm_ms(\x89\x15)	/* 1173 */	/* GSM MS */
#define OMP_O_MAP_C_SUBSCRIBER_IDENTITY				mapP_gsm(\x89\x16)	/* 1174 */	/* GSM */
#define OMP_O_MAP_C_SUBSCRIBER_INFO				mapP_gsm_ms(\x89\x17)	/* 1175 */	/* GSM MS */
#define OMP_O_MAP_C_SUBSCRIBER_LOCATION_REPORT_ARG		mapP_gsm_ls(\x87\x7E)	/* 1022 */	/* GSM LS */
#define OMP_O_MAP_C_SUBSCRIBER_LOCATION_REPORT_RES		mapP_gsm_ls(\x87\x7F)	/* 1023 */	/* GSM LS */
#define OMP_O_MAP_C_SUBSCRIBER_STATE				mapP_gsm_ms(\x89\x18)	/* 1176 */	/* GSM MS */
#define OMP_O_MAP_C_SUPER_CHARGER_INFO				mapP_gsm_ms(\x89\x19)	/* 1177 */	/* GSM MS */
#define OMP_O_MAP_C_SUPPORTED_CODECS_LIST			mapP_gsm_ms(\x89\x1A)	/* 1178 */	/* GSM MS */
#define OMP_O_MAP_C_SYSTEM_FAILURE_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SYSTEM_FAILURE_PARAM			mapP_gsm(\x90\x3A)	/* 2106 */	/* GSM */
#define OMP_O_MAP_C_TARGET_CELL_OUTSIDE_GCA_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_T_BCSM_CAMEL_TDP_CRITERIA_LIST		mapP_gsm_ms(\x89\x1B)	/* 1179 */	/* GSM MS */
#define OMP_O_MAP_C_T_BCSM_CAMEL_TDP_CRITERIA			mapP_gsm_ms(\x89\x1C)	/* 1180 */	/* GSM MS */
#define OMP_O_MAP_C_T_BCSM_CAMEL_TDP_DATA_LIST			mapP_gsm_ms(\x89\x1D)	/* 1181 */	/* GSM MS */
#define OMP_O_MAP_C_T_BCSM_CAMEL_TDP_DATA			mapP_gsm_ms(\x89\x1E)	/* 1182 */	/* GSM MS */
#define OMP_O_MAP_C_T_CAUSE_VALUE_CRITERIA			mapP_gsm_ms(\x89\x1F)	/* 1183 */	/* GSM MS */
#define OMP_O_MAP_C_T_CSI					mapP_gsm_ms(\x89\x20)	/* 1184 */	/* GSM MS */
#define OMP_O_MAP_C_TELESERVICE_LIST				mapP_gsm_ms(\x89\x21)	/* 1185 */	/* GSM MS */
#define OMP_O_MAP_C_TELESERV_NOT_PROV_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_TITLE					mapP_comn(\x87\x69)	/* 1001 */	/* MAP */
#define OMP_O_MAP_C_TPDU_TYPE_CRITERION				mapP_gsm_ms(\x89\x22)	/* 1186 */	/* GSM MS */
#define OMP_O_MAP_C_TRACE_DEPTH_LIST				mapP_gsm_om(\x87\x6F)	/* 1007 */	/* GSM OM */
#define OMP_O_MAP_C_TRACE_EVENT_LIST				mapP_gsm_om(\x87\x70)	/* 1008 */	/* GSM OM */
#define OMP_O_MAP_C_TRACE_INTERFACE_LIST			mapP_gsm_om(\x87\x71)	/* 1009 */	/* GSM OM */
#define OMP_O_MAP_C_TRACE_PROPAGATION_LIST			mapP_gsm_om(\x87\x72)	/* 1010 */	/* GSM OM */
#define OMP_O_MAP_C_TRACING_BUFFER_FULL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_TRIPLET_LIST				mapP_gsm_ms(\x89\x23)	/* 1187 */	/* GSM MS */
#define OMP_O_MAP_C_UESBI_IU					mapP_gsm_ms(\x89\x24)	/* 1188 */	/* GSM MS */
#define OMP_O_MAP_C_UMTS_SECURITY_CONTEXT_DATA			mapP_gsm_ms(\x89\x25)	/* 1189 */	/* GSM MS */
#define OMP_O_MAP_C_UNAUTHORIZED_LCS_CLIENT_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNAUTHORIZED_REQUESTING_NETWORK_PARAM	mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNEXPECTED_DATA_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNEXPECTED_DATA_PARAM			mapP_gsm(\x90\x3B)	/* 2107 */	/* GSM */
#define OMP_O_MAP_C_UNIDENTIFIED_SUB_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNKNOWN_OR_UNREACHABLE_LCS_CLIENT_PARAM	mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UNKNOWN_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_UPDATE_GPRS_LOCATION_ARG			mapP_gsm_ms(\x89\x26)	/* 1190 */	/* GSM MS */
#define OMP_O_MAP_C_UPDATE_GPRS_LOCATION_RES			mapP_gsm_ms(\x89\x27)	/* 1191 */	/* GSM MS */
#define OMP_O_MAP_C_UPDATE_LOCATION_ARG				mapP_gsm_ms(\x89\x28)	/* 1192 */	/* GSM MS */
#define OMP_O_MAP_C_UPDATE_LOCATION_RES				mapP_gsm_ms(\x89\x29)	/* 1193 */	/* GSM MS */
#define OMP_O_MAP_C_USSD_ARG					mapP_gsm_sm(\x88\x12)	/* 1042 */	/* GSM SS */
#define OMP_O_MAP_C_USSD_RES					mapP_gsm_sm(\x88\x13)	/* 1043 */	/* GSM SS */
#define OMP_O_MAP_C_UU_DATA					mapP_gsm_ch(\x88\x05)	/* 1029 */	/* GSM CH */
#define OMP_O_MAP_C_VBS_DATA_LIST				mapP_gsm_ms(\x89\x2A)	/* 1194 */	/* GSM MS */
#define OMP_O_MAP_C_VGCS_DATA_LIST				mapP_gsm_ms(\x89\x2B)	/* 1195 */	/* GSM MS */
#define OMP_O_MAP_C_VLR_CAMEL_SUBSCRIPTION_INFO			mapP_gsm_ms(\x89\x2C)	/* 1196 */	/* GSM MS */
#define OMP_O_MAP_C_VLR_CAPABILITY				mapP_gsm_ms(\x89\x2D)	/* 1197 */	/* GSM MS */
#define OMP_O_MAP_C_VOICE_BROADCAST_DATA			mapP_gsm_ms(\x89\x2E)	/* 1198 */	/* GSM MS */
#define OMP_O_MAP_C_VOICE_GROUP_CALL_DATA			mapP_gsm_ms(\x89\x2F)	/* 1199 */	/* GSM MS */
#define OMP_O_MAP_C_ZONE_CODE_LIST				mapP_gsm_ms(\x89\x30)	/* 1200 */	/* GSM MS */

#define MAP_A_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM			((OM_type)10001)	/* GSM ERR */
#define MAP_A_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM			((OM_type) 7001)	/* GSM SM */
#define MAP_A_ABSENT_SUBSCRIBER_REASON				((OM_type)10002)	/* GSM ERR */
#define MAP_A_ACCESS_NETWORK_PROTOCOL_ID			((OM_type)12001)	/* GSM */
#define MAP_A_ACCESS_RESTRICTION_DATA				((OM_type) 3001)	/* GSM MS */
#define MAP_A_ACCESS_TYPE					((OM_type) 3002)	/* GSM MS */
#define MAP_A_ACCURACY_FULFILMENT_INDICATOR			((OM_type) 9001)	/* GSM LS */
#define MAP_A_ADD_CAPABILITY					((OM_type) 3003)	/* GSM MS */
#define MAP_A_ADD_INFO						((OM_type) 3004)	/* GSM MS */
#define MAP_A_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM	((OM_type)10003)	/* GSM ERR */
#define MAP_A_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM	((OM_type) 7002)	/* GSM SM */
#define MAP_A_ADDITIONAL_INFO					((OM_type) 3005)	/* GSM MS */
#define MAP_A_ADDITIONAL_INFO					((OM_type) 8001)	/* GSM GC */
#define MAP_A_ADDITIONAL_INFO					((OM_type)20001)	/* GSM */
#define MAP_A_ADDITIONAL_LCS_CAPABILITY_SETS			((OM_type) 9002)	/* GSM LS */
#define MAP_A_ADDITIONAL_NETWORK_RESOURCE			((OM_type)10004)	/* GSM ERR */
#define MAP_A_ADDITIONAL_NUMBER					((OM_type) 7003)	/* GSM SM */
#define MAP_A_ADDITIONAL_NUMBER					((OM_type) 9003)	/* GSM LS */
#define MAP_A_ADDITIONAL_NUMBER					((OM_type)20002)	/* GSM */
#define MAP_A_ADDITIONAL_REQUESTED_CAMEL_SUBSCRIPTION_INFO	((OM_type) 3006)	/* GSM MS */
#define MAP_A_ADDITIONAL_SIGNAL_INFO				((OM_type) 5001)	/* GSM CH */
#define MAP_A_ADDITIONAL_SM_DELIVERY_OUTCOME			((OM_type) 7004)	/* GSM SM */
#define MAP_A_ADDITIONAL_SUBSCRIPTIONS				((OM_type) 3007)	/* GSM MS */
#define MAP_A_ADDITIONAL_SUBSCRIPTIONS				((OM_type) 8002)	/* GSM GC */
#define MAP_A_ADDITIONAL_SUBSCRIPTIONS				((OM_type)20003)	/* GSM */
#define MAP_A_ADDITIONAL_VECTORS_ARE_FOR_EPS			((OM_type) 3008)	/* GSM MS */
#define MAP_A_ADDITIONAL_V_GMLC_ADDRESS				((OM_type) 9004)	/* GSM LS */
#define MAP_A_ADD_LCS_PRIVACY_EXCEPTION_LIST			((OM_type) 3009)	/* GSM MS */
#define MAP_A_ADD_LOCATION_ESTIMATE				((OM_type) 9005)	/* GSM LS */
#define MAP_A_AGE_OF_LOCATION_ESTIMATE				((OM_type) 9006)	/* GSM LS */
#define MAP_A_AGE_OF_LOCATION_INFORMATION			((OM_type) 3000)	/* GSM MS */
#define MAP_A_AGE_OF_LOCATION_INFORMATION			((OM_type) 6022)	/* GSM */
#define MAP_A_AGE_OF_LOCATION_INFORMATION			((OM_type) 9007)	/* GSM LS */
#define MAP_A_ALERTING_PATTERN					((OM_type) 5002)	/* GSM CH */
#define MAP_A_ALERTING_PATTERN					((OM_type) 6001)	/* GSM SS */
#define MAP_A_ALERTING_PATTERN					((OM_type) 6005)	/* GSM */
#define MAP_A_ALERT_REASON_INDICATOR				((OM_type) 7005)	/* GSM SM */
#define MAP_A_ALERT_REASON					((OM_type) 7006)	/* GSM SM */
#define MAP_A_ALL_EPS_DATA					((OM_type) 3001)	/* GSM MS */
#define MAP_A_ALL_GPRS_DATA					((OM_type) 3002)	/* GSM MS */
#define MAP_A_ALL_INFORMATION_SENT				((OM_type) 3003)	/* GSM MS */
#define MAP_A_ALL_INFORMATION_SENT				((OM_type) 5003)	/* GSM CH */
#define MAP_A_ALL_INFORMATION_SENT				((OM_type)20004)	/* GSM */
#define MAP_A_ALL_LSA_DATA					((OM_type) 3004)	/* GSM MS */
#define MAP_A_ALLOWED_GSM_ALGORITHMS				((OM_type) 3005)	/* GSM MS */
#define MAP_A_ALLOWED_SERVICES					((OM_type) 5004)	/* GSM CH */
#define MAP_A_ALLOWED_UMTS_ALGORITHMS				((OM_type) 3006)	/* GSM MS */
#define MAP_A_ALTERNATIVE_CHANNEL_TYPE				((OM_type) 3007)	/* GSM MS */
#define MAP_A_AMBR						((OM_type) 3008)	/* GSM MS */
#define MAP_A_AN_APDU						((OM_type) 3009)	/* GSM MS */
#define MAP_A_AN_APDU						((OM_type) 8003)	/* GSM GC */
#define MAP_A_AN_APDU						((OM_type)20005)	/* GSM */
#define MAP_A_ANCHOR_MSC_ADDRESS				((OM_type) 8004)	/* GSM GC */
#define MAP_A_APM_OI_REPLACEMENT_WINDOW				((OM_type) 3010)	/* GSM MS */
#define MAP_A_APN_CONFIGURATION					((OM_type) 3012)	/* GSM MS */
#define MAP_A_APN_CONFIGURATION_PROFILE				((OM_type) 3013)	/* GSM MS */
#define MAP_A_APN_IN_USE					((OM_type) 3014)	/* GSM MS */
#define MAP_A_APN_OI_REPLACEMENT				((OM_type) 3015)	/* GSM MS */
#define MAP_A_APN						((OM_type) 3011)	/* GSM MS */
#define MAP_A_APN_SUBSCRIBED					((OM_type) 3016)	/* GSM MS */
#define MAP_A_APPLICATION_CONTEXT				((OM_type)12002)	/* GSM */
#define MAP_A_AREA_DEFINITION					((OM_type) 9008)	/* GSM LS */
#define MAP_A_AREA_EVENT_INFO					((OM_type) 9009)	/* GSM LS */
#define MAP_A_AREA_IDENTIFICATION				((OM_type) 9010)	/* GSM LS */
#define MAP_A_AREA_LIST						((OM_type) 9011)	/* GSM LS */
#define MAP_A_AREA_TYPE						((OM_type) 9012)	/* GSM LS */
#define MAP_A_ARP						((OM_type) 3017)	/* GSM MS */
#define MAP_A_ASCI_CALL_REFERENCE				((OM_type) 3018)	/* GSM MS */
#define MAP_A_ASCI_CALL_REFERENCE				((OM_type) 6007)	/* GSM */
#define MAP_A_ASCI_CALL_REFERENCE				((OM_type) 7007)	/* GSM SM */
#define MAP_A_ASCI_CALL_REFERENCE				((OM_type) 8005)	/* GSM GC */
#define MAP_A_ASSUMED_IDLE					((OM_type) 3019)	/* GSM MS */
#define MAP_A_AUTHENTICATION_QUINTUPLET				((OM_type) 3020)	/* GSM MS */
#define MAP_A_AUTHENTICATION_SET_LIST				((OM_type) 3021)	/* GSM MS */
#define MAP_A_AUTHENTICATION_TRIPLET				((OM_type) 3022)	/* GSM MS */
#define MAP_A_AUTN						((OM_type) 3023)	/* GSM MS */
#define MAP_A_AUTS						((OM_type) 3024)	/* GSM MS */
#define MAP_A_BASIC_SERVICE_2					((OM_type) 5006)	/* GSM CH */
#define MAP_A_BASIC_SERVICE_CODE				((OM_type) 6003)	/* GSM SS */
#define MAP_A_BASIC_SERVICE_CRITERIA				((OM_type) 3026)	/* GSM MS */
#define MAP_A_BASIC_SERVICE_GROUP_2				((OM_type) 5008)	/* GSM CH */
#define MAP_A_BASIC_SERVICE_GROUP_LIST				((OM_type) 3027)	/* GSM MS */
#define MAP_A_BASIC_SERVICE_GROUP_LIST				((OM_type) 6005)	/* GSM SS */
#define MAP_A_BASIC_SERVICE_GROUP_LIST				((OM_type)20006)	/* GSM */
#define MAP_A_BASIC_SERVICE_GROUP				((OM_type) 5007)	/* GSM CH */
#define MAP_A_BASIC_SERVICE_GROUP				((OM_type) 6004)	/* GSM SS */
#define MAP_A_BASIC_SERVICE_GROUP				((OM_type)20007)	/* GSM */
#define MAP_A_BASIC_SERVICE_LIST				((OM_type) 3028)	/* GSM MS */
#define MAP_A_BASIC_SERVICE					((OM_type)10005)	/* GSM */
#define MAP_A_BASIC_SERVICE					((OM_type)10005)	/* GSM ERR */
#define MAP_A_BASIC_SERVICE					((OM_type) 3025)	/* GSM MS */
#define MAP_A_BASIC_SERVICE					((OM_type) 5005)	/* GSM CH */
#define MAP_A_BASIC_SERVICE					((OM_type) 6002)	/* GSM SS */
#define MAP_A_BASIC_SERVICE					((OM_type)20008)	/* GSM */
#define MAP_A_BEARER_SERVICE_LIST				((OM_type) 3029)	/* GSM MS */
#define MAP_A_BEARER_SERVICE					((OM_type)10006)	/* GSM */
#define MAP_A_BEARER_SERVICE					((OM_type)10006)	/* GSM ERR */
#define MAP_A_BMSC_LIST						((OM_type) 4001)	/* GSM OM */
#define MAP_A_BMSC_TRACE_DEPTH					((OM_type) 4002)	/* GSM OM */
#define MAP_A_BMUEF						((OM_type) 3030)	/* GSM MS */
#define MAP_A_BROADCAST_INIT_ENTITLEMENT			((OM_type) 3031)	/* GSM MS */
#define MAP_A_BSSMAP_SERVICE_HANDOVER_INFO			((OM_type) 3033)	/* GSM MS */
#define MAP_A_BSSMAP_SERVICE_HANDOVER_LIST			((OM_type) 3034)	/* GSM MS */
#define MAP_A_BSSMAP_SERVICE_HANDOVER				((OM_type) 3032)	/* GSM MS */
#define MAP_A_B_SUBSCRIBER_NUMBER				((OM_type) 6006)	/* GSM SS */
#define MAP_A_B_SUBSCRIBER_SUBADDRESS				((OM_type) 6007)	/* GSM SS */
#define MAP_A_CALL_BARRING_CAUSE				((OM_type)10008)	/* GSM */
#define MAP_A_CALL_BARRING_CAUSE				((OM_type)10008)	/* GSM ERR */
#define MAP_A_CALL_BARRING_DATA					((OM_type) 3035)	/* GSM MS */
#define MAP_A_CALL_BARRING_FEATURE_LIST				((OM_type) 3036)	/* GSM MS */
#define MAP_A_CALL_BARRING_FEATURE_LIST				((OM_type) 6009)	/* GSM SS */
#define MAP_A_CALL_BARRING_FEATURE_LIST				((OM_type)20009)	/* GSM */
#define MAP_A_CALL_BARRING_FEATURE				((OM_type) 6008)	/* GSM SS */
#define MAP_A_CALL_BARRING_INFO_FOR_CSE				((OM_type) 3038)	/* GSM MS */
#define MAP_A_CALL_BARRING_INFO					((OM_type) 3037)	/* GSM MS */
#define MAP_A_CALL_BARRING_INFO					((OM_type) 6010)	/* GSM SS */
#define MAP_A_CALL_BARRING_INFO					((OM_type)20010)	/* GSM MS */
#define MAP_A_CALL_DIVERSION_TREATMENT_INDICATOR		((OM_type) 5009)	/* GSM CH */
#define MAP_A_CALL_FORWARDING_DATA				((OM_type) 3039)	/* GSM MS */
#define MAP_A_CALL_INFO						((OM_type) 5010)	/* GSM CH */
#define MAP_A_CALL_INFO						((OM_type) 6011)	/* GSM SS */
#define MAP_A_CALL_INFO						((OM_type)20011)	/* GSM */
#define MAP_A_CALL_ORIGINATOR					((OM_type) 8006)	/* GSM GC */
#define MAP_A_CALL_OUTCOME					((OM_type) 5011)	/* GSM CH */
#define MAP_A_CALL_PRIORITY					((OM_type) 3040)	/* GSM MS */
#define MAP_A_CALL_PRIORITY					((OM_type) 5012)	/* GSM CH */
#define MAP_A_CALL_PRIORITY					((OM_type)20012)	/* GSM */
#define MAP_A_CALL_REFERENCE_NUMBER				((OM_type) 5013)	/* GSM CH */
#define MAP_A_CALL_REPORT_DATA					((OM_type) 5014)	/* GSM CH */
#define MAP_A_CALL_SESSION_RELATED				((OM_type) 9013)	/* GSM LS */
#define MAP_A_CALL_SESSION_UNRELATED				((OM_type) 9014)	/* GSM LS */
#define MAP_A_CALL_TERMINATION_INDICATOR			((OM_type) 5015)	/* GSM CH */
#define MAP_A_CALL_TYPE_CRITERIA				((OM_type) 3041)	/* GSM MS */
#define MAP_A_CAMEL_BUSY					((OM_type) 3042)	/* GSM MS */
#define MAP_A_CAMEL_CAPABILITY_HANDLING				((OM_type) 3043)	/* GSM MS */
#define MAP_A_CAMEL_INFO					((OM_type) 5016)	/* GSM CH */
#define MAP_A_CAMEL_ROUTING_INFO				((OM_type) 5017)	/* GSM CH */
#define MAP_A_CAMEL_SUBSCRIPTION_INFO				((OM_type) 3044)	/* GSM MS */
#define MAP_A_CAMEL_SUBSCRIPTION_INFO_WITHDRAW			((OM_type) 3045)	/* GSM MS */
#define MAP_A_CANCELLATION_TYPE					((OM_type) 3046)	/* GSM MS */
#define MAP_A_CATEGORY						((OM_type) 3047)	/* GSM MS */
#define MAP_A_CAUSE_VALUE					((OM_type) 3048)	/* GSM MS */
#define MAP_A_CCBS_BUSY						((OM_type)10009)	/* GSM */
#define MAP_A_CCBS_BUSY						((OM_type)10009)	/* GSM ERR */
#define MAP_A_CCBS_CALL						((OM_type) 5018)	/* GSM CH */
#define MAP_A_CCBS_DATA						((OM_type) 6012)	/* GSM SS */
#define MAP_A_CCBS_FEATURE_LIST					((OM_type) 6014)	/* GSM SS */
#define MAP_A_CCBS_FEATURE					((OM_type) 5019)	/* GSM CH */
#define MAP_A_CCBS_FEATURE					((OM_type) 6013)	/* GSM SS */
#define MAP_A_CCBS_FEATURE					((OM_type)20013)	/* GSM */
#define MAP_A_CCBS_INDEX					((OM_type) 6015)	/* GSM SS */
#define MAP_A_CCBS_INDICATORS					((OM_type) 5020)	/* GSM CH */
#define MAP_A_CCBS_MONITORING					((OM_type) 5021)	/* GSM CH */
#define MAP_A_CCBS_POSSIBLE					((OM_type)10010)	/* GSM */
#define MAP_A_CCBS_POSSIBLE					((OM_type)10010)	/* GSM ERR */
#define MAP_A_CCBS_POSSIBLE					((OM_type) 5022)	/* GSM CH */
#define MAP_A_CCBS_REQUEST_STATE				((OM_type) 6016)	/* GSM SS */
#define MAP_A_CCBS_SUBSCRIBER_STATUS				((OM_type) 5023)	/* GSM CH */
#define MAP_A_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_FIXED_LENGTH	((OM_type)12003)	/* GSM */
#define MAP_A_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_OR_LAI		((OM_type) 3049)	/* GSM MS */
#define MAP_A_CELL_ID						((OM_type) 8007)	/* GSM GC */
#define MAP_A_CELL_ID_OR_SAI					((OM_type) 9015)	/* GSM LS */
#define MAP_A_CHARGING_CHARACTERISTICS				((OM_type) 3050)	/* GSM MS */
#define MAP_A_CHARGING_CHARACTERISTICS_WITHDRAW			((OM_type) 3051)	/* GSM MS */
#define MAP_A_CHARGING_ID					((OM_type) 3052)	/* GSM MS */
#define MAP_A_CHOSEN_CHANNEL_INFO				((OM_type) 3053)	/* GSM MS */
#define MAP_A_CHOSEN_RADIO_RESOURCE_INFORMATION			((OM_type) 3054)	/* GSM MS */
#define MAP_A_CHOSEN_SPEECH_VERSION				((OM_type) 3055)	/* GSM MS */
#define MAP_A_CIPHERING_ALGORITHM				((OM_type) 8008)	/* GSM GC */
#define MAP_A_CK						((OM_type) 3056)	/* GSM MS */
#define MAP_A_CKSN						((OM_type) 3057)	/* GSM MS */
#define MAP_A_CKSN						((OM_type) 8009)	/* GSM GC */
#define MAP_A_CKSN						((OM_type)20014)	/* GSM */
#define MAP_A_CLIENT_IDENTITY					((OM_type) 3058)	/* GSM MS */
#define MAP_A_CLI_RESTRICTION_OPTION				((OM_type) 6017)	/* GSM SS */
#define MAP_A_CODEC1						((OM_type) 3059)	/* GSM MS */
#define MAP_A_CODEC2						((OM_type) 3060)	/* GSM MS */
#define MAP_A_CODEC3						((OM_type) 3061)	/* GSM MS */
#define MAP_A_CODEC4						((OM_type) 3062)	/* GSM MS */
#define MAP_A_CODEC5						((OM_type) 3063)	/* GSM MS */
#define MAP_A_CODEC6						((OM_type) 3064)	/* GSM MS */
#define MAP_A_CODEC7						((OM_type) 3065)	/* GSM MS */
#define MAP_A_CODEC8						((OM_type) 3066)	/* GSM MS */
#define MAP_A_CODEC_INFO					((OM_type) 8010)	/* GSM GC */
#define MAP_A_COMPLETE_DATA_LIST_INCLUDED			((OM_type) 3067)	/* GSM MS */
#define MAP_A_CONTEXT_ID_LIST					((OM_type) 3069)	/* GSM MS */
#define MAP_A_CONTEXT_ID					((OM_type) 3068)	/* GSM MS */
#define MAP_A_CS_ALLOCATION_RETENTION_PRIORITY			((OM_type) 3070)	/* GSM MS */
#define MAP_A_CSG_ID						((OM_type) 3071)	/* GSM MS */
#define MAP_A_CSG_SUBSCRIPTION_DATA_LIST			((OM_type) 3073)	/* GSM MS */
#define MAP_A_CSG_SUBSCRIPTION_DATA				((OM_type) 3072)	/* GSM MS */
#define MAP_A_CSG_SUBSCRIPTION_DELETED				((OM_type) 3074)	/* GSM MS */
#define MAP_A_CSI_ACTIVE					((OM_type) 3075)	/* GSM MS */
#define MAP_A_CS_LCS_NOT_SUPPORTED_BY_UE			((OM_type) 3076)	/* GSM MS */
#define MAP_A_CUG_CHECK_INFO					((OM_type) 5024)	/* GSM CH */
#define MAP_A_CUG_FEATURE_LIST					((OM_type) 3077)	/* GSM MS */
#define MAP_A_CUG_INDEX						((OM_type) 3078)	/* GSM MS */
#define MAP_A_CUG_INFO						((OM_type) 3079)	/* GSM MS */
#define MAP_A_CUG_INTERLOCK					((OM_type) 3080)	/* GSM MS */
#define MAP_A_CUG_INTERLOCK					((OM_type) 5025)	/* GSM CH */
#define MAP_A_CUG_INTERLOCK					((OM_type)20015)	/* GSM */
#define MAP_A_CUG_OUTGOING_ACCESS				((OM_type) 5026)	/* GSM CH */
#define MAP_A_CUG_REJECT_CAUSE					((OM_type)10011)	/* GSM */
#define MAP_A_CUG_REJECT_CAUSE					((OM_type)10011)	/* GSM ERR */
#define MAP_A_CUG_SUBSCRIPTION_FLAG				((OM_type) 5027)	/* GSM CH */
#define MAP_A_CUG_SUBSCRIPTION					((OM_type) 3081)	/* GSM MS */
#define MAP_A_CUG_SUBSCRIPTOIN_LIST				((OM_type) 3082)	/* GSM MS */
#define MAP_A_CURRENT_LOCATION					((OM_type) 3083)	/* GSM MS */
#define MAP_A_CURRENT_LOCATION_RETRIEVED			((OM_type) 3084)	/* GSM MS */
#define MAP_A_CURRENTLY_USED_CODEC				((OM_type) 3085)	/* GSM MS */
#define MAP_A_CURRENT_SECURITY_CONTEXT				((OM_type) 3086)	/* GSM MS */
#define MAP_A_DATA_CODING_SCHEME				((OM_type) 9016)	/* GSM LS */
#define MAP_A_D_CSI						((OM_type) 3087)	/* GSM MS */
#define MAP_A_D_CSI						((OM_type) 5028)	/* GSM CH */
#define MAP_A_D_CSI						((OM_type)20016)	/* GSM */
#define MAP_A_DEFAULT_CALL_HANDLING				((OM_type) 3088)	/* GSM MS */
#define MAP_A_DEFAULT_CONTEXT					((OM_type) 3089)	/* GSM MS */
#define MAP_A_DEFAULT_PRIORITY					((OM_type)12004)	/* GSM */
#define MAP_A_DEFAULT_PRIORITY					((OM_type) 6018)	/* GSM SS */
#define MAP_A_DEFAULT_SESSION_HANDLING				((OM_type) 3090)	/* GSM MS */
#define MAP_A_DEFAULT_SMS_HANDLING				((OM_type) 3091)	/* GSM MS */
#define MAP_A_DEFERRED_LOCATION_EVENT_TYPE			((OM_type) 9017)	/* GSM LS */
#define MAP_A_DEFERRED_MT_LR_DATA				((OM_type) 9018)	/* GSM LS */
#define MAP_A_DEFERRED_MT_LR_RESPONSE_INDICATOR			((OM_type) 9019)	/* GSM LS */
#define MAP_A_DELIVERY_OUTCOME_INDICATOR			((OM_type) 7008)	/* GSM SM */
#define MAP_A_DESTINATION_NUMBER_CRITERIA			((OM_type) 3093)	/* GSM MS */
#define MAP_A_DESTINATION_NUMBER_LENGTH_LIST			((OM_type) 3095)	/* GSM MS */
#define MAP_A_DESTINATION_NUMBER_LENGTH				((OM_type) 3094)	/* GSM MS */
#define MAP_A_DESTINATION_NUMBER_LIST				((OM_type) 3096)	/* GSM MS */
#define MAP_A_DESTINATION_NUMBER				((OM_type) 3092)	/* GSM MS */
#define MAP_A_DESTINATION_REFERENCE				((OM_type)12005)	/* GSM */
#define MAP_A_DIAGNOSTIC_INFO					((OM_type)10007)	/* GSM */
#define MAP_A_DIAGNOSTIC_INFO					((OM_type)10007)	/* GSM ERR */
#define MAP_A_DIALED_NUMBER					((OM_type) 3097)	/* GSM MS */
#define MAP_A_D_IM_CSI						((OM_type) 3098)	/* GSM MS */
#define MAP_A_DISPATCHER_LIST					((OM_type) 7010)	/* GSM SM */
#define MAP_A_DISPATCHER					((OM_type) 7009)	/* GSM SM */
#define MAP_A_DOWNLINK_ATTACHED					((OM_type) 8011)	/* GSM GC */
#define MAP_A_DP_ANALYSED_INFO_CRITERIA_LIST			((OM_type) 3099)	/* GSM MS */
#define MAP_A_DP_ANALYSED_INFO_CRITERIUM			((OM_type) 3100)	/* GSM MS */
#define MAP_A_DUAL_COMMUNICATION				((OM_type) 8012)	/* GSM GC */
#define MAP_A_EMERGENCY_MODE_RESET_COMMAND_FLAG			((OM_type) 8013)	/* GSM GC */
#define MAP_A_EMLPP_INFO					((OM_type) 3101)	/* GSM MS */
#define MAP_A_EMLPP_PRIORITY					((OM_type) 6018)	/* GSM */
#define MAP_A_ENCRYPTION_ALGORITHM				((OM_type) 3102)	/* GSM MS */
#define MAP_A_ENCRYPTION_INFO					((OM_type) 3103)	/* GSM MS */
#define MAP_A_EPS_AUTHENTICATION_SET_LIST			((OM_type) 3104)	/* GSM MS */
#define MAP_A_EPS_AV						((OM_type) 3105)	/* GSM MS */
#define MAP_A_EPS_DATA_LIST					((OM_type) 3106)	/* GSM MS */
#define MAP_A_EPS_INFO						((OM_type) 3107)	/* GSM MS */
#define MAP_A_EPS_QOS_SUBSCRIBED				((OM_type) 3108)	/* GSM MS */
#define MAP_A_EPS_SUBSCRIPTION_DATA				((OM_type) 3109)	/* GSM MS */
#define MAP_A_EPS_SUBSCRIPTION_WITHDRAW				((OM_type) 3110)	/* GSM MS */
#define MAP_A_EQUIPMENT_STATUS					((OM_type) 3111)	/* GSM MS */
#define MAP_A_EVENT_MET						((OM_type) 3112)	/* GSM MS */
#define MAP_A_EVENT_REPORT_DATA					((OM_type) 5029)	/* GSM CH */
#define MAP_A_EXPIRATION_DATE					((OM_type) 3113)	/* GSM MS */
#define MAP_A_EXT2_QOS_SUBSCRIBED				((OM_type) 3114)	/* GSM MS */
#define MAP_A_EXT3_QOS_SUBSCRIBED				((OM_type) 3115)	/* GSM MS */
#define MAP_A_EXT_BASIC_SERVICE_CODE				((OM_type) 3116)	/* GSM MS */
#define MAP_A_EXT_BEARER_SERVICE_CODE				((OM_type) 3117)	/* GSM MS */
#define MAP_A_EXT_BEARER_SERVICE				((OM_type)10012)	/* GSM ERR */
#define MAP_A_EXT_CALL_BARRING_FEATURE				((OM_type) 3118)	/* GSM MS */
#define MAP_A_EXTENDED_ROUTING_INFO				((OM_type) 5030)	/* GSM CH */
#define MAP_A_EXTENSIBLE_CALL_BARRED_PARAM			((OM_type)10013)	/* GSM */
#define MAP_A_EXTENSIBLE_CALL_BARRED_PARAM			((OM_type)10013)	/* GSM ERR */
#define MAP_A_EXTENSIBLE_SYSTEM_FAILURE_PARAM			((OM_type)10014)	/* GSM */
#define MAP_A_EXTENSIBLE_SYSTEM_FAILURE_PARAM			((OM_type)10014)	/* GSM ERR */
#define MAP_A_EXTENSION_CONTAINER				((OM_type)10016)	/* GSM ERR */
#define MAP_A_EXTENSION_CONTAINER				((OM_type)12007)	/* GSM */
#define MAP_A_EXTENSION_CONTAINER				((OM_type) 3120)	/* GSM MS */
#define MAP_A_EXTENSION_CONTAINER				((OM_type) 5032)	/* GSM CH */
#define MAP_A_EXTENSION_CONTAINER				((OM_type) 6020)	/* GSM SS */
#define MAP_A_EXTENSION_CONTAINER				((OM_type) 7012)	/* GSM SM */
#define MAP_A_EXTENSION_CONTAINER				((OM_type) 8015)	/* GSM GC */
#define MAP_A_EXTENSION_CONTAINER				((OM_type) 9021)	/* GSM LS */
#define MAP_A_EXTENSION						((OM_type)10015)	/* GSM ERR */
#define MAP_A_EXTENSION						((OM_type)12006)	/* GSM */
#define MAP_A_EXTENSION						((OM_type) 3119)	/* GSM MS */
#define MAP_A_EXTENSION						((OM_type) 5031)	/* GSM CH */
#define MAP_A_EXTENSION						((OM_type) 6019)	/* GSM SS */
#define MAP_A_EXTENSION						((OM_type) 7011)	/* GSM SM */
#define MAP_A_EXTENSION						((OM_type) 8014)	/* GSM GC */
#define MAP_A_EXTENSION						((OM_type) 9020)	/* GSM LS */
#define MAP_A_EXTENSIONS					((OM_type)12008)	/* GSM */
#define MAP_A_EXTERNAL_ADDRESS					((OM_type)12009)	/* GSM */
#define MAP_A_EXTERNAL_CLIENT_LIST				((OM_type) 3122)	/* GSM MS */
#define MAP_A_EXTERNAL_CLIENT					((OM_type) 3121)	/* GSM MS */
#define MAP_A_EXT_EXTERNAL_CLIENT_LIST				((OM_type) 3123)	/* GSM MS */
#define MAP_A_EXT_FORW_FEATURE					((OM_type) 3124)	/* GSM MS */
#define MAP_A_EXT_ID						((OM_type)12010)	/* GSM */
#define MAP_A_EXT_PROTOCOL_ID					((OM_type)12011)	/* GSM */
#define MAP_A_EXT_QOS_SUBSCRIBED				((OM_type) 3125)	/* GSM MS */
#define MAP_A_EXT_SS_INFO					((OM_type) 3126)	/* GSM MS */
#define MAP_A_EXT_SS_STATUS					((OM_type) 6021)	/* GSM */
#define MAP_A_EXT_TELESERVICE_CODE				((OM_type) 3127)	/* GSM MS */
#define MAP_A_EXT_TELESERVICE					((OM_type)10017)	/* GSM ERR */
#define MAP_A_EXT_TYPE						((OM_type)12012)	/* GSM */
#define MAP_A_FAILURE_CAUSE					((OM_type) 3128)	/* GSM MS */
#define MAP_A_FORWARDED_TO_NUMBER				((OM_type) 3129)	/* GSM MS */
#define MAP_A_FORWARDED_TO_NUMBER				((OM_type) 5033)	/* GSM CH */
#define MAP_A_FORWARDED_TO_NUMBER				((OM_type) 6021)	/* GSM SS */
#define MAP_A_FORWARDED_TO_NUMBER				((OM_type)20017)	/* GSM */
#define MAP_A_FORWARDED_TO_SUBADDRESS				((OM_type) 3130)	/* GSM MS */
#define MAP_A_FORWARDED_TO_SUBADDRESS				((OM_type) 5034)	/* GSM CH */
#define MAP_A_FORWARDED_TO_SUBADDRESS				((OM_type) 6022)	/* GSM SS */
#define MAP_A_FORWARDED_TO_SUBADDRESS				((OM_type)20018)	/* GSM */
#define MAP_A_FORWARDING_DATA					((OM_type) 5035)	/* GSM CH */
#define MAP_A_FORWARDING_FEATURE_LIST				((OM_type) 3131)	/* GSM MS */
#define MAP_A_FORWARDING_FEATURE_LIST				((OM_type) 6024)	/* GSM SS */
#define MAP_A_FORWARDING_FEATURE_LIST				((OM_type)20019)	/* GSM */
#define MAP_A_FORWARDING_FEATURE				((OM_type) 6023)	/* GSM SS */
#define MAP_A_FORWARDING_INFO_FOR_CSE				((OM_type) 3133)	/* GSM MS */
#define MAP_A_FORWARDING_INFO					((OM_type) 3132)	/* GSM MS */
#define MAP_A_FORWARDING_INFO					((OM_type) 6025)	/* GSM SS */
#define MAP_A_FORWARDING_INFO					((OM_type)20020)	/* GSM */
#define MAP_A_FORWARDING_INTERROGATION_REQUIRED			((OM_type) 5036)	/* GSM CH */
#define MAP_A_FORWARDING_OPTION					((OM_type) 3134)	/* GSM MS */
#define MAP_A_FORWARDING_OPTIONS				((OM_type) 5037)	/* GSM CH */
#define MAP_A_FORWARDING_OPTIONS				((OM_type) 6026)	/* GSM SS */
#define MAP_A_FORWARDING_OPTIONS				((OM_type)20021)	/* GSM */
#define MAP_A_FORWARDING_REASON					((OM_type) 5038)	/* GSM CH */
#define MAP_A_FREEZE_N_TMSI					((OM_type) 3135)	/* GSM MS */
#define MAP_A_FREEZE_P_TMSI					((OM_type) 3136)	/* GSM MS */
#define MAP_A_FREEZE_TMSI					((OM_type) 3137)	/* GSM MS */
#define MAP_A_GENERIC_SERVICE_INFO				((OM_type) 6027)	/* GSM SS */
#define MAP_A_GEODETIC_INFORMATION				((OM_type) 3138)	/* GSM MS */
#define MAP_A_GEOGRAPHICAL_INFORMATION				((OM_type) 3139)	/* GSM MS */
#define MAP_A_GERAN_CLASSMARK					((OM_type) 3140)	/* GSM MS */
#define MAP_A_GERAN_CODEC_LIST					((OM_type) 3141)	/* GSM MS */
#define MAP_A_GERAN_POSITIONING_DATA				((OM_type) 9022)	/* GSM LS */
#define MAP_A_GGSN_ADDRESS					((OM_type) 3142)	/* GSM MS */
#define MAP_A_GGSN_LIST						((OM_type) 4003)	/* GSM OM */
#define MAP_A_GGSN_NUMBER					((OM_type) 3143)	/* GSM MS */
#define MAP_A_GGSN_TRACE_DEPTH					((OM_type) 4004)	/* GSM OM */
#define MAP_A_GLOBAL_CELL_ID					((OM_type) 6012)	/* GSM */
#define MAP_A_GMLC_LIST						((OM_type) 3145)	/* GSM MS */
#define MAP_A_GMLC_LIST_WITHDRAW				((OM_type) 3146)	/* GSM MS */
#define MAP_A_GMLC						((OM_type) 3144)	/* GSM MS */
#define MAP_A_GMLC_RESTRICTION					((OM_type) 3147)	/* GSM MS */
#define MAP_A_GMSC_ADDRESS					((OM_type) 5039)	/* GSM CH */
#define MAP_A_GMSC_CAMEL_SUBSCRIPTION_INFO			((OM_type) 5040)	/* GSM CH */
#define MAP_A_GMSC_OR_GSMSCF_ADDRESS				((OM_type) 5041)	/* GSM CH */
#define MAP_A_GPRS_CAMEL_TDP_DATA_LIST				((OM_type) 3149)	/* GSM MS */
#define MAP_A_GPRS_CAMEL_TDP_DATA				((OM_type) 3148)	/* GSM MS */
#define MAP_A_GPRS_CONNECTION_SUSPENDED				((OM_type)10018)	/* GSM */
#define MAP_A_GPRS_CONNECTION_SUSPENDED				((OM_type)10018)	/* GSM ERR */
#define MAP_A_GPRS_CSI						((OM_type) 3150)	/* GSM MS */
#define MAP_A_GPRS_DATA_LIST					((OM_type) 3151)	/* GSM MS */
#define MAP_A_GPRS_EHANCEMENT_SUPPORT_INDICATOR			((OM_type) 3152)	/* GSM MS */
#define MAP_A_GPRS_MS_CLASS					((OM_type) 3153)	/* GSM MS */
#define MAP_A_GPRS_NODE_INDICATOR				((OM_type) 7013)	/* GSM SM */
#define MAP_A_GPRS_NODE_INDICATOR				((OM_type) 9023)	/* GSM LS */
#define MAP_A_GPRS_NODE_INDICATOR				((OM_type)20022)	/* GSM */
#define MAP_A_GPRS_SUBSCRIPTION_DATA				((OM_type) 3154)	/* GSM MS */
#define MAP_A_GPRS_SUBSCRIPTION_DATA_WITHDRAW			((OM_type) 3155)	/* GSM MS */
#define MAP_A_GPRS_SUPPORT_INDICATOR				((OM_type) 7014)	/* GSM SM */
#define MAP_A_GPRS_TRIGGER_DETECTION_POINT			((OM_type) 3156)	/* GSM MS */
#define MAP_A_GROUP_CALL_NUMBER					((OM_type) 8016)	/* GSM GC */
#define MAP_A_GROUP_ID						((OM_type) 3157)	/* GSM MS */
#define MAP_A_GROUP_ID						((OM_type) 8017)	/* GSM GC */
#define MAP_A_GROUP_ID						((OM_type)20023)	/* GSM */
#define MAP_A_GROUP_KEY_NUMBER_VK_ID				((OM_type) 8019)	/* GSM GC */
#define MAP_A_GROUP_KEY						((OM_type) 8018)	/* GSM GC */
#define MAP_A_GRPS_CSI						((OM_type) 3158)	/* GSM MS */
#define MAP_A_GSM_BEARER_CAPABILITY				((OM_type) 5042)	/* GSM CH */
#define MAP_A_GSMSCF_ADDRESS					((OM_type) 3159)	/* GSM MS */
#define MAP_A_GSMSCF_INITIATED_CALL				((OM_type) 5043)	/* GSM CH */
#define MAP_A_GSM_SECURITY_CONTEXT_DATA				((OM_type) 3160)	/* GSM MS */
#define MAP_A_HANDOVER_NUMBER					((OM_type) 3161)	/* GSM MS */
#define MAP_A_H_GMLC_ADDRESS					((OM_type) 9024)	/* GSM LS */
#define MAP_A_HLR_ID						((OM_type)12013)	/* GSM */
#define MAP_A_HLR_LIST						((OM_type) 3162)	/* GSM MS */
#define MAP_A_HLR_NUMBER					((OM_type) 3163)	/* GSM MS */
#define MAP_A_HO_NUMBER_NOT_REQUESTED				((OM_type) 3164)	/* GSM MS */
#define MAP_A_HOP_COUNTER					((OM_type) 3165)	/* GSM MS */
#define MAP_A_HORIZONTAL_ACCURACY				((OM_type) 9025)	/* GSM LS */
#define MAP_A_ICS_INDICATOR					((OM_type) 3166)	/* GSM MS */
#define MAP_A_IDENTITY						((OM_type) 3167)	/* GSM MS */
#define MAP_A_IK						((OM_type) 3168)	/* GSM MS */
#define MAP_A_IMEI						((OM_type) 3169)	/* GSM MS */
#define MAP_A_IMEI						((OM_type) 6009)	/* GSM */
#define MAP_A_IMEI						((OM_type) 9026)	/* GSM LS */
#define MAP_A_IMEISV						((OM_type) 3170)	/* GSM MS */
#define MAP_A_IMMEDIATE_RESPONSE_PREFERRED			((OM_type) 3171)	/* GSM MS */
#define MAP_A_IMSI						((OM_type) 3172)	/* GSM MS */
#define MAP_A_IMSI						((OM_type) 5044)	/* GSM CH */
#define MAP_A_IMSI						((OM_type) 6006)	/* GSM */
#define MAP_A_IMSI						((OM_type) 6028)	/* GSM SS */
#define MAP_A_IMSI						((OM_type) 7015)	/* GSM SM */
#define MAP_A_IMSI						((OM_type) 8020)	/* GSM GC */
#define MAP_A_IMSI						((OM_type) 9027)	/* GSM LS */
#define MAP_A_IMSI_WITH_LMSI					((OM_type) 3173)	/* GSM MS */
#define MAP_A_INFORM_PREVIOUS_NETWORK_ENTITY			((OM_type) 3174)	/* GSM MS */
#define MAP_A_INTEGRITY_PROTECTION_ALGORITHM			((OM_type) 3175)	/* GSM MS */
#define MAP_A_INTEGRITY_PROTECTION_INFO				((OM_type) 3176)	/* GSM MS */
#define MAP_A_INTER_CUG_RESTRICTIONS				((OM_type) 3177)	/* GSM MS */
#define MAP_A_INTERROGATION_TYPE				((OM_type) 5045)	/* GSM CH */
#define MAP_A_INTERVAL_TIME					((OM_type) 9028)	/* GSM LS */
#define MAP_A_INTRA_CUG_OPTIONS					((OM_type) 3178)	/* GSM MS */
#define MAP_A_IP_GW_ABSENT_SUBSCRIBER_DIAGNOSTIC		((OM_type) 7016)	/* GSM SM */
#define MAP_A_IP_SM_GW_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM		((OM_type) 7017)	/* GSM SM */
#define MAP_A_IP_SM_GW_INDICATOR				((OM_type) 7018)	/* GSM SM */
#define MAP_A_IP_SM_GW_SM_DELIVERY_OUTCOME			((OM_type) 7019)	/* GSM SM */
#define MAP_A_ISR_INFORMATION					((OM_type) 3179)	/* GSM MS */
#define MAP_A_IST_ALERT_TIMER					((OM_type) 3180)	/* GSM MS */
#define MAP_A_IST_ALERT_TIMER					((OM_type) 5046)	/* GSM CH */
#define MAP_A_IST_ALERT_TIMER					((OM_type)20024)	/* GSM */
#define MAP_A_IST_INFORMATION_WITHDRAW				((OM_type) 3181)	/* GSM MS */
#define MAP_A_IST_INFORMATION_WITHDRAW				((OM_type) 5047)	/* GSM CH */
#define MAP_A_IST_INFORMATION_WITHDRAW				((OM_type)20025)	/* GSM */
#define MAP_A_IST_SUPPORT_INDICATOR				((OM_type) 3182)	/* GSM MS */
#define MAP_A_IST_SUPPORT_INDICATOR				((OM_type) 5048)	/* GSM CH */
#define MAP_A_IST_SUPPORT_INDICATOR				((OM_type)20026)	/* GSM */
#define MAP_A_IU_AVAILABLE_CODEC_LIST				((OM_type) 3183)	/* GSM MS */
#define MAP_A_IU_CURRENT_USED_CODEC				((OM_type) 3184)	/* GSM MS */
#define MAP_A_IU_SELECTED_CODEC					((OM_type) 3185)	/* GSM MS */
#define MAP_A_IU_SUPPORTED_CODECS_LIST				((OM_type) 3186)	/* GSM MS */
#define MAP_A_KASME						((OM_type) 3187)	/* GSM MS */
#define MAP_A_KC						((OM_type) 3188)	/* GSM MS */
#define MAP_A_KC						((OM_type) 8021)	/* GSM GC */
#define MAP_A_KC						((OM_type)20027)	/* GSM */
#define MAP_A_KEEP_CCBS_CALL_INDICATOR				((OM_type) 5049)	/* GSM CH */
#define MAP_A_KEY_STATUS					((OM_type) 3189)	/* GSM MS */
#define MAP_A_KSI						((OM_type) 3190)	/* GSM MS */
#define MAP_A_LAC						((OM_type) 3191)	/* GSM MS */
#define MAP_A_LAI_FIXED_LENGTH					((OM_type)12014)	/* GSM */
#define MAP_A_LAI_FIXED_LENGTH					((OM_type) 3192)	/* GSM MS */
#define MAP_A_LCS_APN						((OM_type) 9029)	/* GSM LS */
#define MAP_A_LCS_CLIENT_DIALED_BY_MS				((OM_type) 9030)	/* GSM LS */
#define MAP_A_LCS_CLIENT_EXTENAL_ID				((OM_type) 9031)	/* GSM LS */
#define MAP_A_LCS_CLIENT_ID					((OM_type) 9032)	/* GSM LS */
#define MAP_A_LCS_CLIENT_INTERNAL_ID				((OM_type) 3193)	/* GSM MS */
#define MAP_A_LCS_CLIENT_INTERNAL_ID				((OM_type) 6014)	/* GSM */
#define MAP_A_LCS_CLIENT_INTERNAL_ID				((OM_type) 9033)	/* GSM LS */
#define MAP_A_LCS_CLIENT_NAME					((OM_type) 9034)	/* GSM LS */
#define MAP_A_LCS_CLIENT_TYPE					((OM_type) 9035)	/* GSM LS */
#define MAP_A_LCS_CODEWORD					((OM_type) 9036)	/* GSM LS */
#define MAP_A_LCS_CODEWORD_STRING				((OM_type) 9037)	/* GSM LS */
#define MAP_A_LCS_EVENT						((OM_type) 9038)	/* GSM LS */
#define MAP_A_LCS_FORMAT_INDICATOR				((OM_type) 9039)	/* GSM LS */
#define MAP_A_LCS_INFORMATION					((OM_type) 3194)	/* GSM MS */
#define MAP_A_LCS_LOCATION_INFO					((OM_type) 9040)	/* GSM LS */
#define MAP_A_LCS_PRIORITY					((OM_type) 9041)	/* GSM LS */
#define MAP_A_LCS_PRIVACY_CHECK					((OM_type) 9042)	/* GSM LS */
#define MAP_A_LCS_PRIVACY_CLASS					((OM_type) 3195)	/* GSM MS */
#define MAP_A_LCS_PRIVACY_EXCEPTION_LIST			((OM_type) 3196)	/* GSM MS */
#define MAP_A_LCS_QOS						((OM_type) 9043)	/* GSM LS */
#define MAP_A_LCS_REFERENCE_NUMBER				((OM_type) 9044)	/* GSM LS */
#define MAP_A_LCS_REQUESTOR_ID					((OM_type) 9045)	/* GSM LS */
#define MAP_A_LCS_SERVICE_TYPE_ID				((OM_type) 6015)	/* GSM */
#define MAP_A_LCS_SERVICE_TYPE_ID				((OM_type) 9046)	/* GSM LS */
#define MAP_A_LMSI						((OM_type) 3197)	/* GSM MS */
#define MAP_A_LMSI						((OM_type) 5050)	/* GSM CH */
#define MAP_A_LMSI						((OM_type) 6011)	/* GSM */
#define MAP_A_LMSI						((OM_type) 7020)	/* GSM SM */
#define MAP_A_LMSI						((OM_type) 9047)	/* GSM LS */
#define MAP_A_LMU_INDICATOR					((OM_type) 3198)	/* GSM MS */
#define MAP_A_LOCATION_AREA					((OM_type) 3199)	/* GSM MS */
#define MAP_A_LOCATION_ESTIMATE					((OM_type) 9048)	/* GSM LS */
#define MAP_A_LOCATION_ESTIMATE_TYPE				((OM_type) 9049)	/* GSM LS */
#define MAP_A_LOCATION_INFORMATION_GPRS				((OM_type) 3201)	/* GSM MS */
#define MAP_A_LOCATION_INFORMATION				((OM_type) 3200)	/* GSM MS */
#define MAP_A_LOCATION_INFO_WITH_LMSI				((OM_type) 7021)	/* GSM SM */
#define MAP_A_LOCATION_NUMBER					((OM_type) 3202)	/* GSM MS */
#define MAP_A_LOCATION_TYPE					((OM_type) 9050)	/* GSM LS */
#define MAP_A_LONG_FORWARDED_TO_NUMBER				((OM_type) 3203)	/* GSM MS */
#define MAP_A_LONG_FORWARDED_TO_NUMBER				((OM_type) 5051)	/* GSM CH */
#define MAP_A_LONG_FORWARDED_TO_NUMBER				((OM_type) 6029)	/* GSM SS */
#define MAP_A_LONG_FORWARDED_TO_NUMBER				((OM_type)20028)	/* GSM */
#define MAP_A_LONG_FTN_SUPPORTED				((OM_type) 3204)	/* GSM MS */
#define MAP_A_LONG_FTN_SUPPORTED				((OM_type) 5052)	/* GSM CH */
#define MAP_A_LONG_FTN_SUPPORTED				((OM_type) 6030)	/* GSM SS */
#define MAP_A_LONG_FTN_SUPPORTED				((OM_type)20029)	/* GSM */
#define MAP_A_LONG_GROUP_ID					((OM_type) 3205)	/* GSM MS */
#define MAP_A_LONG_GROUP_ID_SUPPORTED				((OM_type) 3206)	/* GSM MS */
#define MAP_A_LSA_ACTIVE_MODE_INDICATOR				((OM_type) 3207)	/* GSM MS */
#define MAP_A_LSA_ATTRIBUTES					((OM_type) 3208)	/* GSM MS */
#define MAP_A_LSA_DATA_LIST					((OM_type) 3210)	/* GSM MS */
#define MAP_A_LSA_DATA						((OM_type) 3209)	/* GSM MS */
#define MAP_A_LSA_IDENTITY_LIST					((OM_type) 3212)	/* GSM MS */
#define MAP_A_LSA_IDENTITY					((OM_type) 3211)	/* GSM MS */
#define MAP_A_LSA_INFORMATION					((OM_type) 3213)	/* GSM MS */
#define MAP_A_LSA_INFORMATION_WITHDRAW				((OM_type) 3214)	/* GSM MS */
#define MAP_A_LSA_ONLY_ACCESS_INDICATOR				((OM_type) 3215)	/* GSM MS */
#define MAP_A_MATCH_TYPE					((OM_type) 3216)	/* GSM MS */
#define MAP_A_MAXIMUM_ENTITLED_PRIORITY				((OM_type)12015)	/* GSM */
#define MAP_A_MAXIMUM_ENTITLEMENT_PRIORITY			((OM_type) 6031)	/* GSM SS */
#define MAP_A_MAX_MC_BEARERS					((OM_type) 6019)	/* GSM */
#define MAP_A_MAX_REQUESTED_BANDWIDTH_DL			((OM_type) 3217)	/* GSM MS */
#define MAP_A_MAX_REQUESTED_BANDWIDTH_UL			((OM_type) 3218)	/* GSM MS */
#define MAP_A_MC_BEARERS					((OM_type) 6020)	/* GSM */
#define MAP_A_M_CSI						((OM_type) 3219)	/* GSM MS */
#define MAP_A_MC_SS_INFO					((OM_type) 3220)	/* GSM MS */
#define MAP_A_MG_CSI						((OM_type) 3221)	/* GSM MS */
#define MAP_A_MGW_EVENT_LIST					((OM_type) 4000)	/* GSM OM */
#define MAP_A_MGW_INTERFACE_LIST				((OM_type) 4005)	/* GSM OM */
#define MAP_A_MGW_LIST						((OM_type) 4006)	/* GSM OM */
#define MAP_A_MGW_TRACE_DEPTH					((OM_type) 4007)	/* GSM OM */
#define MAP_A_MLC_NUMBER					((OM_type) 9051)	/* GSM LS */
#define MAP_A_MM_CODE						((OM_type) 3222)	/* GSM MS */
#define MAP_A_MNP_INFO_RES					((OM_type) 3223)	/* GSM MS */
#define MAP_A_MNP_REQUESTED_INFO				((OM_type) 3224)	/* GSM MS */
#define MAP_A_MOBILE_NOT_REACHABLE_REASON			((OM_type) 3225)	/* GSM MS */
#define MAP_A_MOBILITY_TRIGGERS					((OM_type) 3226)	/* GSM MS */
#define MAP_A_MODIFICATION_REQUEST_FOR_CB_INFO			((OM_type) 3227)	/* GSM MS */
#define MAP_A_MODIFICATION_REQUEST_FOR_CF_INFO			((OM_type) 3228)	/* GSM MS */
#define MAP_A_MODIFICATION_REQUEST_FOR_CSI			((OM_type) 3229)	/* GSM MS */
#define MAP_A_MODIFICATION_REQUEST_FOR_IP_SM_GW_DATA		((OM_type) 3230)	/* GSM MS */
#define MAP_A_MODIFICATION_REQUEST_FOR_ODB_DATA			((OM_type) 3231)	/* GSM MS */
#define MAP_A_MODIFY_CSI_STATE					((OM_type) 3232)	/* GSM MS */
#define MAP_A_MODIFY_NOTIFICATION_TO_CSE			((OM_type) 3233)	/* GSM MS */
#define MAP_A_MODIFY_REGISTRATION_STATUS			((OM_type) 3234)	/* GSM MS */
#define MAP_A_MOLR_CLASS					((OM_type) 3235)	/* GSM MS */
#define MAP_A_MOLR_LIST						((OM_type) 3236)	/* GSM MS */
#define MAP_A_MO_LR_SHORT_CIRCUIT_INDICATOR			((OM_type) 9052)	/* GSM LS */
#define MAP_A_MONITORING_MODE					((OM_type) 5053)	/* GSM CH */
#define MAP_A_MORE_MESSAGES_TO_SEND				((OM_type) 7022)	/* GSM SM */
#define MAP_A_MO_SMS_CSI					((OM_type) 3237)	/* GSM MS */
#define MAP_A_MS_CLASSMARK_2					((OM_type) 3239)	/* GSM MS */
#define MAP_A_MS_CLASSMARK					((OM_type) 3238)	/* GSM MS */
#define MAP_A_MSC_NUMBER					((OM_type) 3240)	/* GSM MS */
#define MAP_A_MSC_NUMBER					((OM_type) 5054)	/* GSM CH */
#define MAP_A_MSC_NUMBER					((OM_type) 7023)	/* GSM SM */
#define MAP_A_MSC_NUMBER					((OM_type)20030)	/* GSM */
#define MAP_A_MSC_S_EVENT_LIST					((OM_type) 4008)	/* GSM OM */
#define MAP_A_MSC_S_INTERFACE_LIST				((OM_type) 4009)	/* GSM OM */
#define MAP_A_MSC_S_LIST					((OM_type) 4010)	/* GSM OM */
#define MAP_A_MSC_S_TRACE_DEPTH					((OM_type) 4011)	/* GSM OM */
#define MAP_A_MSISDN_BS_LIST					((OM_type) 3243)	/* GSM MS */
#define MAP_A_MSISDN_BS						((OM_type) 3242)	/* GSM MS */
#define MAP_A_MSISDN						((OM_type) 3241)	/* GSM MS */
#define MAP_A_MSISDN						((OM_type) 5055)	/* GSM CH */
#define MAP_A_MSISDN						((OM_type) 6032)	/* GSM SS */
#define MAP_A_MSISDN						((OM_type) 7024)	/* GSM SM */
#define MAP_A_MSISDN						((OM_type) 9053)	/* GSM LS */
#define MAP_A_MSISDN						((OM_type)20031)	/* GSM */
#define MAP_A_MS_NETWORK_CAPABILITY				((OM_type) 3244)	/* GSM MS */
#define MAP_A_MS_NOT_REACHABLE					((OM_type) 3245)	/* GSM MS */
#define MAP_A_MS_RADIO_ACCESS_CAPABILITY			((OM_type) 3246)	/* GSM MS */
#define MAP_A_MSRN						((OM_type) 5056)	/* GSM CH */
#define MAP_A_MT_ROAMING_RETRY					((OM_type) 5057)	/* GSM CH */
#define MAP_A_MT_ROAMING_RETRY_SUPPORTED			((OM_type) 5058)	/* GSM CH */
#define MAP_A_MT_SMS_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3248)	/* GSM MS */
#define MAP_A_MT_SMS_CAMEL_TDP_CRITERIA				((OM_type) 3247)	/* GSM MS */
#define MAP_A_MT_SMS_CSI					((OM_type) 3249)	/* GSM MS */
#define MAP_A_MT_SMS_TPDU_TYPE					((OM_type) 3250)	/* GSM MS */
#define MAP_A_MULTICALL_BEARER_INFO				((OM_type) 3251)	/* GSM MS */
#define MAP_A_MULTIPLE_BEARER_NOT_SUPPORTED			((OM_type) 3252)	/* GSM MS */
#define MAP_A_MULTIPLE_BEARER_REQUESTED				((OM_type) 3253)	/* GSM MS */
#define MAP_A_MW_STATUS						((OM_type) 7025)	/* GSM SM */
#define MAP_A_NAEA_CIC						((OM_type) 6013)	/* GSM */
#define MAP_A_NAEA_PREFERRED_CIC				((OM_type)12016)	/* GSM */
#define MAP_A_NAEA_PREFERRED_CI					((OM_type) 3254)	/* GSM MS */
#define MAP_A_NAEA_PREFERRED_CI					((OM_type) 5059)	/* GSM CH */
#define MAP_A_NAEA_PREFERRED_CI					((OM_type)20032)	/* GSM */
#define MAP_A_NA_ESRD						((OM_type) 9054)	/* GSM LS */
#define MAP_A_NA_ESRK						((OM_type) 9055)	/* GSM LS */
#define MAP_A_NA_ESRK_REQUEST					((OM_type)12017)	/* GSM */
#define MAP_A_NAME_STRING					((OM_type) 9056)	/* GSM LS */
#define MAP_A_NBR_SB						((OM_type)12018)	/* GSM */
#define MAP_A_NBR_SB						((OM_type) 6033)	/* GSM SS */
#define MAP_A_NBR_SN						((OM_type) 6034)	/* GSM SS */
#define MAP_A_NBR_USER						((OM_type)12019)	/* GSM */
#define MAP_A_NBR_USER						((OM_type) 6035)	/* GSM SS */
#define MAP_A_NEEDED_LCS_CAPABILITY_NOT_SUPPORTED_IN_SERVING_NODE	((OM_type)10019)	/* GSM */
#define MAP_A_NEEDED_LCS_CAPABILITY_NOT_SUPPORTED_IN_SERVING_NODE	((OM_type)10019)	/* GSM ERR */
#define MAP_A_NET_DET_NOT_REACHABLE				((OM_type) 3255)	/* GSM MS */
#define MAP_A_NETWORK_ACCESS_MODE				((OM_type) 3256)	/* GSM MS */
#define MAP_A_NETWORK_NODE_NUMBER				((OM_type) 7026)	/* GSM SM */
#define MAP_A_NETWORK_NODE_NUMBER				((OM_type) 9057)	/* GSM LS */
#define MAP_A_NETWORK_NODE_NUMBER				((OM_type)20033)	/* GSM */
#define MAP_A_NETWORK_RESOURCE					((OM_type)10020)	/* GSM */
#define MAP_A_NETWORK_RESOURCE					((OM_type)10020)	/* GSM ERR */
#define MAP_A_NETWORK_SIGNAL_INFO_2				((OM_type) 5061)	/* GSM CH */
#define MAP_A_NETWORK_SIGNAL_INFO				((OM_type) 5060)	/* GSM CH */
#define MAP_A_NETWORK_SIGNAL_INFO				((OM_type) 6036)	/* GSM SS */
#define MAP_A_NETWORK_SIGNAL_INFO				((OM_type)20034)	/* GSM */
#define MAP_A_NO_REPLY_CONDITION_TIME				((OM_type) 3257)	/* GSM MS */
#define MAP_A_NO_REPLY_CONDITION_TIME				((OM_type) 6037)	/* GSM SS */
#define MAP_A_NO_REPLY_CONDITION_TIME				((OM_type)20035)	/* GSM */
#define MAP_A_NO_SM_RP_DA					((OM_type) 7027)	/* GSM SM */
#define MAP_A_NO_SM_RP_OA					((OM_type) 7028)	/* GSM SM */
#define MAP_A_NOTIFICATION_TO_CSE				((OM_type) 3258)	/* GSM MS */
#define MAP_A_NOTIFICATION_TO_MS_USER				((OM_type) 3259)	/* GSM MS */
#define MAP_A_NOT_PROVIDED_FROM_SGSN				((OM_type) 3260)	/* GSM MS */
#define MAP_A_NOT_PROVIDED_FROM_VLR				((OM_type) 3261)	/* GSM MS */
#define MAP_A_NSAPI						((OM_type) 3262)	/* GSM MS */
#define MAP_A_NUMBER_OF_FORWARDING				((OM_type) 5062)	/* GSM CH */
#define MAP_A_NUMBER_OF_REQUESTED_ADDITIONAL_VECTORS		((OM_type) 3263)	/* GSM MS */
#define MAP_A_NUMBER_OF_REQUESTED_VECTORS			((OM_type) 3264)	/* GSM MS */
#define MAP_A_NUMBER_PORTABILITY_STATUS				((OM_type) 3265)	/* GSM MS */
#define MAP_A_NUMBER_PORTABILITY_STATUS				((OM_type) 5063)	/* GSM CH */
#define MAP_A_NUMBER_PORTABILITY_STATUS				((OM_type)20036)	/* GSM */
#define MAP_A_O_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3267)	/* GSM MS */
#define MAP_A_O_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 5064)	/* GSM CH */
#define MAP_A_O_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type)20037)	/* GSM */
#define MAP_A_O_BCSM_CAMEL_TDP_CRITERIA				((OM_type) 3266)	/* GSM MS */
#define MAP_A_O_BCSM_CAMEL_TDP_DATA_LIST			((OM_type) 3269)	/* GSM MS */
#define MAP_A_O_BCSM_CAMEL_TDP_DATA				((OM_type) 3268)	/* GSM MS */
#define MAP_A_O_BCSM_TRIGGER_DETECTION_POINT			((OM_type) 3270)	/* GSM MS */
#define MAP_A_O_CAUSE_VALUE_CRITERIA				((OM_type) 3271)	/* GSM MS */
#define MAP_A_OCCURENCE_INFO					((OM_type) 9058)	/* GSM LS */
#define MAP_A_O_CSI						((OM_type) 3272)	/* GSM MS */
#define MAP_A_O_CSI						((OM_type) 5065)	/* GSM CH */
#define MAP_A_O_CSI						((OM_type)20038)	/* GSM */
#define MAP_A_ODB_DATA						((OM_type) 3274)	/* GSM MS */
#define MAP_A_ODB_GENERAL_DATA					((OM_type) 3275)	/* GSM MS */
#define MAP_A_ODB_HPLMN_DATA					((OM_type) 3276)	/* GSM MS */
#define MAP_A_ODB_INFO						((OM_type) 3277)	/* GSM MS */
#define MAP_A_ODB						((OM_type) 3273)	/* GSM MS */
#define MAP_A_OFFERED_CAMEL4_CSIS_IN_INTERROGATING_NODE		((OM_type) 5067)	/* GSM CH */
#define MAP_A_OFFERED_CAMEL4_CSIS_IN_SGSN			((OM_type) 3279)	/* GSM MS */
#define MAP_A_OFFERED_CAMEL4_CSIS_IN_VLR			((OM_type) 3280)	/* GSM MS */
#define MAP_A_OFFERED_CAMEL4_CSIS_IN_VMSC			((OM_type) 5068)	/* GSM CH */
#define MAP_A_OFFERED_CAMEL4_CSIS				((OM_type) 3278)	/* GSM MS */
#define MAP_A_OFFERED_CAMEL4_CSIS				((OM_type) 5066)	/* GSM CH */
#define MAP_A_OFFERED_CAMEL4_CSIS				((OM_type)20039)	/* GSM */
#define MAP_A_OFFERED_CAMEL4_FUNCTIONALITIES			((OM_type) 3281)	/* GSM MS */
#define MAP_A_OFFERED_CAMMEL4_CSIS				((OM_type) 3282)	/* GSM MS */
#define MAP_A_O_IM_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3283)	/* GSM MS */
#define MAP_A_O_IM_CSI						((OM_type) 3284)	/* GSM MS */
#define MAP_A_OMC_ID						((OM_type) 4012)	/* GSM OM */
#define MAP_A_ONGOING_CALL					((OM_type) 7029)	/* GSM SM */
#define MAP_A_OR_CAPABILITY					((OM_type) 5069)	/* GSM CH */
#define MAP_A_ORIGINATING_REFERENCE				((OM_type)12020)	/* GSM */
#define MAP_A_OR_INTERROGATION					((OM_type) 5070)	/* GSM CH */
#define MAP_A_OR_NOT_SUPPORTED_IN_GMSC				((OM_type) 5071)	/* GSM CH */
#define MAP_A_OVERRIDE_CATEGORY					((OM_type) 6038)	/* GSM SS */
#define MAP_A_P_ABORT_CAUSE					((OM_type)12021)	/* GSM */
#define MAP_A_P_ABORT_REASON					((OM_type)12022)	/* GSM */
#define MAP_A_PAGING_AREA_CAPABILITY				((OM_type) 3286)	/* GSM MS */
#define MAP_A_PAGING_AREA					((OM_type) 3285)	/* GSM MS */
#define MAP_A_PAGING_AREA					((OM_type) 5072)	/* GSM CH */
#define MAP_A_PAGING_AREA					((OM_type)20040)	/* GSM */
#define MAP_A_PASSWORD						((OM_type) 3287)	/* GSM MS */
#define MAP_A_PASSWORD						((OM_type) 6039)	/* GSM SS */
#define MAP_A_PASSWORD						((OM_type)20041)	/* GSM */
#define MAP_A_PC_LCS_NOT_SUPPORTED_BY_UE			((OM_type) 3288)	/* GSM MS */
#define MAP_A_PCS_EXTENSION					((OM_type)12023)	/* GSM */
#define MAP_A_PCS_EXTENSIONS					((OM_type)12024)	/* GSM */
#define MAP_A_PDN_GW_ALLOCATION_TYPE				((OM_type) 3289)	/* GSM MS */
#define MAP_A_PDN_GW_IDENTITY					((OM_type) 3290)	/* GSM MS */
#define MAP_A_PDN_GW_IPV4_ADDRESS				((OM_type) 3291)	/* GSM MS */
#define MAP_A_PDN_GW_IPV6_ADDRESS				((OM_type) 3292)	/* GSM MS */
#define MAP_A_PDN_GW_NAME					((OM_type) 3293)	/* GSM MS */
#define MAP_A_PDN_GW_UPDATE					((OM_type) 3294)	/* GSM MS */
#define MAP_A_PDP_ADDRESS					((OM_type) 3295)	/* GSM MS */
#define MAP_A_PDP_CHARGING_CHARACTERISTICS			((OM_type) 3296)	/* GSM MS */
#define MAP_A_PDP_CONTEXT_ACTIVE				((OM_type) 3298)	/* GSM MS */
#define MAP_A_PDP_CONTEXT_IDENTIFIER				((OM_type) 3300)	/* GSM MS */
#define MAP_A_PDP_CONTEXT_ID					((OM_type) 3299)	/* GSM MS */
#define MAP_A_PDP_CONTEXT_INFO					((OM_type) 3301)	/* GSM MS */
#define MAP_A_PDP_CONTEXT					((OM_type) 3297)	/* GSM MS */
#define MAP_A_PDP_TYPE						((OM_type) 3302)	/* GSM MS */
#define MAP_A_PERIODIC_LDR_INFO					((OM_type) 9059)	/* GSM LS */
#define MAP_A_PLMN_CLIENT_LIST					((OM_type) 3303)	/* GSM MS */
#define MAP_A_PLMN_ID						((OM_type) 9060)	/* GSM LS */
#define MAP_A_PLMN_LIST						((OM_type) 9061)	/* GSM LS */
#define MAP_A_PLMN_LIST_PRIORITIZED				((OM_type) 9062)	/* GSM LS */
#define MAP_A_POSITION_METHOD_FAILURE_DIAGNOSTIC		((OM_type)10021)	/* GSM */
#define MAP_A_POSITION_METHOD_FAILURE_DIAGNOSTIC		((OM_type)10021)	/* GSM ERR */
#define MAP_A_PPR_ADDRESS					((OM_type) 9063)	/* GSM LS */
#define MAP_A_PREFERENTIAL_CUG_INDICATOR			((OM_type) 3304)	/* GSM MS */
#define MAP_A_PRE_PAGING_SUPPORTED				((OM_type) 5073)	/* GSM CH */
#define MAP_A_PREVIOUS_LAI					((OM_type) 3305)	/* GSM MS */
#define MAP_A_PRIORITY						((OM_type) 8022)	/* GSM GC */
#define MAP_A_PRIVACY_OVERRIDE					((OM_type) 9064)	/* GSM LS */
#define MAP_A_PRIVATE_EXTENSION_LIST				((OM_type)12025)	/* GSM */
#define MAP_A_PRIVATE_EXTENSION					((OM_type)12026)	/* GSM */
#define MAP_A_PROBLEM_DIAGNOSTIC				((OM_type)12027)	/* GSM */
#define MAP_A_PROTECTED_PAYLOAD					((OM_type)10022)	/* GSM */
#define MAP_A_PROTECTED_PAYLOAD					((OM_type)10022)	/* GSM ERR */
#define MAP_A_PROTOCOL_ID					((OM_type)12028)	/* GSM */
#define MAP_A_PROVIDER_REASON					((OM_type)12029)	/* GSM */
#define MAP_A_PROVISIONED_SS					((OM_type) 3306)	/* GSM MS */
#define MAP_A_PS_ATTACHED_NOT_REACHABLE_FOR_PAGING		((OM_type) 3307)	/* GSM MS */
#define MAP_A_PS_ATTACHED_REACHABLE_FOR_PAGING			((OM_type) 3308)	/* GSM MS */
#define MAP_A_PS_DETACHED					((OM_type) 3309)	/* GSM MS */
#define MAP_A_PSEUDONUM_INDICATOR				((OM_type) 9065)	/* GSM LS */
#define MAP_A_PS_PDP_ACTIVE_NOT_REACHABLE_FOR_PAGING		((OM_type) 3310)	/* GSM MS */
#define MAP_A_PS_PDP_ACTIVE_REACHABLE_FOR_PAGING		((OM_type) 3311)	/* GSM MS */
#define MAP_A_PS_SUBSCRIBER_STATE				((OM_type) 3312)	/* GSM MS */
#define MAP_A_QOS2_NEGOTIATED					((OM_type) 3313)	/* GSM MS */
#define MAP_A_QOS2_REQUESTED					((OM_type) 3314)	/* GSM MS */
#define MAP_A_QOS2_SUBSCRIBED					((OM_type) 3315)	/* GSM MS */
#define MAP_A_QOS3_NEGOTIATED					((OM_type) 3316)	/* GSM MS */
#define MAP_A_QOS3_REQUESTED					((OM_type) 3317)	/* GSM MS */
#define MAP_A_QOS3_SUBSCRIBED					((OM_type) 3318)	/* GSM MS */
#define MAP_A_QOS_CLASS_IDENTIFIER				((OM_type) 3319)	/* GSM MS */
#define MAP_A_QOS_NEGOTIATED					((OM_type) 3320)	/* GSM MS */
#define MAP_A_QOS_REQUESTED					((OM_type) 3321)	/* GSM MS */
#define MAP_A_QOS_SUBSCRIBED					((OM_type) 3322)	/* GSM MS */
#define MAP_A_QUADRUPLET_LIST					((OM_type) 3323)	/* GSM MS */
#define MAP_A_RAB_CONFIGURATION_INDICATOR			((OM_type) 3324)	/* GSM MS */
#define MAP_A_RAB_ID						((OM_type) 3325)	/* GSM MS */
#define MAP_A_RADIO_RESOURCE_INFORMATION			((OM_type) 3327)	/* GSM MS */
#define MAP_A_RADIO_RESOURCE_LIST				((OM_type) 3328)	/* GSM MS */
#define MAP_A_RADIO_RESOURCE					((OM_type) 3326)	/* GSM MS */
#define MAP_A_RANAP_SERVICE_HANDOVER				((OM_type) 3329)	/* GSM MS */
#define MAP_A_RAND						((OM_type) 3330)	/* GSM MS */
#define MAP_A_RAN_PERIODIC_LOCATION_SUPPORT			((OM_type) 9066)	/* GSM LS */
#define MAP_A_RAN_TECHNOLOGY					((OM_type) 9067)	/* GSM LS */
#define MAP_A_RE_ATTEMPT					((OM_type) 3331)	/* GSM MS */
#define MAP_A_REFUSE_REASON					((OM_type)12030)	/* GSM */
#define MAP_A_REGIONAL_SUBSCRIPTION_DATA			((OM_type) 3332)	/* GSM MS */
#define MAP_A_REGIONAL_SUBSCRIPTION_IDENTIFIER			((OM_type) 3333)	/* GSM MS */
#define MAP_A_REGIONAL_SUBSCRIPTION_RESPONSE			((OM_type) 3334)	/* GSM MS */
#define MAP_A_RELEASE_GROUP_CALL				((OM_type) 8023)	/* GSM GC */
#define MAP_A_RELEASE_METHOD					((OM_type)12031)	/* GSM */
#define MAP_A_RELEASE_RESOURCES_SUPPORTED			((OM_type) 5074)	/* GSM CH */
#define MAP_A_RELOCATION_NUMBER_LIST				((OM_type) 3336)	/* GSM MS */
#define MAP_A_RELOCATION_NUMBER					((OM_type) 3335)	/* GSM MS */
#define MAP_A_REPLACE_B_NUMBER					((OM_type) 5075)	/* GSM CH */
#define MAP_A_REPORT_CAUSE					((OM_type)12032)	/* GSM */
#define MAP_A_REPORTING_AMOUNT					((OM_type) 9068)	/* GSM LS */
#define MAP_A_REPORTING_INTERVAL				((OM_type) 9069)	/* GSM LS */
#define MAP_A_REPORTING_PLMN_LIST				((OM_type) 9071)	/* GSM LS */
#define MAP_A_REPORTING_PLMN					((OM_type) 9070)	/* GSM LS */
#define MAP_A_REQUESTED_CAMEL_SUBSCRIPTION_INFO			((OM_type) 3337)	/* GSM MS */
#define MAP_A_REQUESTED_DOMAIN					((OM_type) 3338)	/* GSM MS */
#define MAP_A_REQUESTED_EQUIPMENT_INFO				((OM_type) 3339)	/* GSM MS */
#define MAP_A_REQUESTED_INFO					((OM_type) 3340)	/* GSM MS */
#define MAP_A_REQUESTED_INFO					((OM_type) 8024)	/* GSM GC */
#define MAP_A_REQUESTED_INFO					((OM_type)20042)	/* GSM */
#define MAP_A_REQUESTED_NODE_TYPE				((OM_type) 3341)	/* GSM MS */
#define MAP_A_REQUESTED_SS_INFO					((OM_type) 3342)	/* GSM MS */
#define MAP_A_REQUESTED_SUBSCRIPTION_INFO			((OM_type) 3343)	/* GSM MS */
#define MAP_A_REQUESTING_PLMN_ID				((OM_type) 3344)	/* GSM MS */
#define MAP_A_REQUESTOR_ID_STRING				((OM_type) 9072)	/* GSM LS */
#define MAP_A_RESPONSE_TIME_CATEGORY				((OM_type) 9074)	/* GSM LS */
#define MAP_A_RESPONSE_TIME					((OM_type) 9073)	/* GSM LS */
#define MAP_A_RE_SYNCHRONISATION_INFO				((OM_type) 3345)	/* GSM MS */
#define MAP_A_RFSP_ID						((OM_type) 3346)	/* GSM MS */
#define MAP_A_RNC_ADDRESS					((OM_type) 3347)	/* GSM MS */
#define MAP_A_RNC_INTERFACE_LIST				((OM_type) 4013)	/* GSM OM */
#define MAP_A_RNC_LIST						((OM_type) 4000)	/* GSM OM */
#define MAP_A_RNC_TRACE_DEPTH					((OM_type) 4014)	/* GSM OM */
#define MAP_A_ROAMING_NOT_ALLOWED_CAUSE				((OM_type)10023)	/* GSM */
#define MAP_A_ROAMING_NOT_ALLOWED_CAUSE				((OM_type)10023)	/* GSM ERR */
#define MAP_A_ROAMING_NUMBER					((OM_type) 5076)	/* GSM CH */
#define MAP_A_ROAMING_RESTRICTED_IN_SGSN			((OM_type) 3348)	/* GSM MS */
#define MAP_A_ROAMING_RESTRICTION				((OM_type) 3349)	/* GSM MS */
#define MAP_A_ROUTEING_AREA_IDENTITY				((OM_type) 3350)	/* GSM MS */
#define MAP_A_ROUTEING_NUMBER					((OM_type) 3351)	/* GSM MS */
#define MAP_A_ROUTING_INFO_2					((OM_type) 5078)	/* GSM CH */
#define MAP_A_ROUTING_INFO					((OM_type) 5077)	/* GSM CH */
#define MAP_A_RUF_OUTCOME					((OM_type) 5079)	/* GSM CH */
#define MAP_A_SAI_PRESENT					((OM_type) 3352)	/* GSM MS */
#define MAP_A_SAI_PRESENT					((OM_type) 9075)	/* GSM LS */
#define MAP_A_SAI_PRESENT					((OM_type)20043)	/* GSM */
#define MAP_A_SECURITY_HEADER					((OM_type)10024)	/* GSM */
#define MAP_A_SECURITY_HEADER					((OM_type)10024)	/* GSM ERR */
#define MAP_A_SEGMENTATION_PROHIBITED				((OM_type) 3353)	/* GSM MS */
#define MAP_A_SELECTED_GSM_ALGORITHM				((OM_type) 3354)	/* GSM MS */
#define MAP_A_SELECTED_LSA_IDENTITY				((OM_type) 3356)	/* GSM MS */
#define MAP_A_SELECTED_LSA_ID					((OM_type) 3355)	/* GSM MS */
#define MAP_A_SELECTED_RAB_ID					((OM_type) 3357)	/* GSM MS */
#define MAP_A_SELECTED_UMTS_ALGORITHM				((OM_type) 3358)	/* GSM MS */
#define MAP_A_SELECTED_UMTS_ALGORITHMS				((OM_type) 3359)	/* GSM MS */
#define MAP_A_SEND_SUBSCRIBER_DATA				((OM_type) 3360)	/* GSM MS */
#define MAP_A_SEQUENCE_NUMBER					((OM_type) 9076)	/* GSM LS */
#define MAP_A_SERVED_PARTY_IP_ADDRESS				((OM_type) 3361)	/* GSM MS */
#define MAP_A_SERVICE_CENTRE_ADDRESS				((OM_type) 7030)	/* GSM SM */
#define MAP_A_SERVICE_INDICATOR					((OM_type) 6040)	/* GSM SS */
#define MAP_A_SERVICE_KEY					((OM_type) 3362)	/* GSM MS */
#define MAP_A_SERVICE_TYPE_IDENTITY				((OM_type) 3364)	/* GSM MS */
#define MAP_A_SERVICE_TYPE_LIST					((OM_type) 3365)	/* GSM MS */
#define MAP_A_SERVICE_TYPE					((OM_type) 3363)	/* GSM MS */
#define MAP_A_SERVING_NODE_TYPE_INDICATOR			((OM_type) 3366)	/* GSM MS */
#define MAP_A_SGSN_ADDRESS					((OM_type) 3367)	/* GSM MS */
#define MAP_A_SGSN_CAMEL_SUBSCRIPTION_INFO			((OM_type) 3368)	/* GSM MS */
#define MAP_A_SGSN_CAPABILITY					((OM_type) 3369)	/* GSM MS */
#define MAP_A_SGSN_LIST						((OM_type) 4015)	/* GSM OM */
#define MAP_A_SGSN_MME_SEPARATION_SUPPORTED			((OM_type) 3370)	/* GSM MS */
#define MAP_A_SGSN_NUMBER					((OM_type) 3371)	/* GSM MS */
#define MAP_A_SGSN_NUMBER					((OM_type) 7031)	/* GSM SM */
#define MAP_A_SGSN_NUMBER					((OM_type)20044)	/* GSM */
#define MAP_A_SGSN_TRACE_DEPTH					((OM_type) 4016)	/* GSM OM */
#define MAP_A_SHAPE_OF_LOCATION_ESTIMATED_NOT_SUPPORTED		((OM_type)10025)	/* GSM */
#define MAP_A_SHAPE_OF_LOCATION_ESTIMATED_NOT_SUPPORTED		((OM_type)10025)	/* GSM ERR */
#define MAP_A_SIGNAL_INFO					((OM_type)12033)	/* GSM */
#define MAP_A_SKIP_SUBSCRIBER_DATA_UPDATE			((OM_type) 3372)	/* GSM MS */
#define MAP_A_SLR_ARG_EXTENSION_CONTAINER			((OM_type) 9077)	/* GSM LS */
#define MAP_A_SLR_ARG_PCS_EXTENSIONS				((OM_type)12034)	/* GSM */
#define MAP_A_SM_DELIVERY_NOT_INTENDED				((OM_type) 7032)	/* GSM SM */
#define MAP_A_SM_DELIVERY_OUTCOME				((OM_type) 7033)	/* GSM SM */
#define MAP_A_SM_ENUMERATED_DELIVERY_FAILURE_CAUSE		((OM_type)10026)	/* GSM */
#define MAP_A_SM_ENUMERATED_DELIVERY_FAILURE_CAUSE		((OM_type)10026)	/* GSM ERR */
#define MAP_A_SM_RP_DA						((OM_type) 7034)	/* GSM SM */
#define MAP_A_SM_RP_MTI						((OM_type) 7035)	/* GSM SM */
#define MAP_A_SM_RP_OA						((OM_type) 7036)	/* GSM SM */
#define MAP_A_SM_RP_PRI						((OM_type) 7037)	/* GSM SM */
#define MAP_A_SM_RP_SMEA					((OM_type) 7038)	/* GSM SM */
#define MAP_A_SM_RP_UI						((OM_type) 7039)	/* GSM SM */
#define MAP_A_SM_RP_UI						((OM_type) 8025)	/* GSM GC */
#define MAP_A_SMS_CALL_BARRING_SUPPORT_INDICATOR		((OM_type) 3373)	/* GSM MS */
#define MAP_A_SMS_CAMEL_TDP_DATA_LIST				((OM_type) 3375)	/* GSM MS */
#define MAP_A_SMS_CAMEL_TDP_DATA				((OM_type) 3374)	/* GSM MS */
#define MAP_A_SMS_TRIGGER_DETECTION_POINT			((OM_type) 3376)	/* GSM MS */
#define MAP_A_SOLA_SUPPORT_INDICATOR				((OM_type) 3377)	/* GSM MS */
#define MAP_A_SOLSA_SUPPORT_INDICATOR				((OM_type) 3378)	/* GSM MS */
#define MAP_A_SOURCE						((OM_type)12035)	/* GSM */
#define MAP_A_SPECIFIC_CSI_DETECTED_LIST			((OM_type) 3379)	/* GSM MS */
#define MAP_A_SPECIFIC_CSI_WITHDRAW				((OM_type) 3380)	/* GSM MS */
#define MAP_A_SPECIFIC_INFORMATION				((OM_type)12036)	/* GSM */
#define MAP_A_SRES						((OM_type) 3381)	/* GSM MS */
#define MAP_A_SS_CAMEL_DATA					((OM_type) 3382)	/* GSM MS */
#define MAP_A_SS_CODE						((OM_type)10027)	/* GSM */
#define MAP_A_SS_CODE						((OM_type)10027)	/* GSM ERR */
#define MAP_A_SS_CODE						((OM_type)12037)	/* GSM */
#define MAP_A_SS_CODE						((OM_type) 3383)	/* GSM MS */
#define MAP_A_SS_CODE						((OM_type) 6041)	/* GSM SS */
#define MAP_A_SS_CSI						((OM_type) 3384)	/* GSM MS */
#define MAP_A_SS_DATA						((OM_type) 3385)	/* GSM MS */
#define MAP_A_SS_DATA						((OM_type) 6042)	/* GSM SS */
#define MAP_A_SS_DATA						((OM_type)20045)	/* GSM */
#define MAP_A_SS_EVENT_LIST					((OM_type) 3386)	/* GSM MS */
#define MAP_A_SS_EVENT						((OM_type) 6043)	/* GSM SS */
#define MAP_A_SS_EVENT_SPECIFICATION				((OM_type) 6045)	/* GSM SS */
#define MAP_A_SS_EVENT_SPEC					((OM_type) 6044)	/* GSM SS */
#define MAP_A_SS_INFO_FOR_CSE					((OM_type) 3387)	/* GSM MS */
#define MAP_A_SS_LIST_2						((OM_type) 5081)	/* GSM CH */
#define MAP_A_SS_LIST						((OM_type) 3388)	/* GSM MS */
#define MAP_A_SS_LIST						((OM_type) 5080)	/* GSM CH */
#define MAP_A_SS_LIST						((OM_type)20046)	/* GSM */
#define MAP_A_SS_STATUS						((OM_type)10028)	/* GSM */
#define MAP_A_SS_STATUS						((OM_type)10028)	/* GSM ERR */
#define MAP_A_SS_STATUS						((OM_type)12038)	/* GSM */
#define MAP_A_SS_STATUS						((OM_type) 3389)	/* GSM MS */
#define MAP_A_SS_STATUS						((OM_type) 6046)	/* GSM SS */
#define MAP_A_SS_SUBSCRIPTION_OPTION				((OM_type) 3390)	/* GSM MS */
#define MAP_A_SS_SUBSCRIPTION_OPTION				((OM_type) 6047)	/* GSM SS */
#define MAP_A_STATE_ATTRIBUTES					((OM_type) 8026)	/* GSM GC */
#define MAP_A_STN_SR						((OM_type) 3391)	/* GSM MS */
#define MAP_A_STN_SR_WITHDRAW					((OM_type) 3392)	/* GSM MS */
#define MAP_A_SUBSCRIBER_DATA_STORED				((OM_type) 3393)	/* GSM MS */
#define MAP_A_SUBSCRIBER_IDENTITY				((OM_type) 3394)	/* GSM MS */
#define MAP_A_SUBSCRIBER_INFO					((OM_type) 3395)	/* GSM MS */
#define MAP_A_SUBSCRIBER_INFO					((OM_type) 5082)	/* GSM CH */
#define MAP_A_SUBSCRIBER_INFO					((OM_type)20048)	/* GSM */
#define MAP_A_SUBSCRIBER_STATE					((OM_type) 3396)	/* GSM MS */
#define MAP_A_SUBSCRIBER_STATUS					((OM_type) 3397)	/* GSM MS */
#define MAP_A_SUPER_CHARGER_SUPPORTED_IN_HLR			((OM_type) 3398)	/* GSM MS */
#define MAP_A_SUPER_CHARGER_SUPPORTED_IN_SERVING_NETWORK_ENTITY	((OM_type) 3399)	/* GSM MS */
#define MAP_A_SUPPORTED_CAMEL_PHASES_IN_INTERROGATING_NODE	((OM_type) 5084)	/* GSM CH */
#define MAP_A_SUPPORTED_CAMEL_PHASES_IN_VMSC			((OM_type) 5085)	/* GSM CH */
#define MAP_A_SUPPORTED_CAMEL_PHASES				((OM_type) 3400)	/* GSM MS */
#define MAP_A_SUPPORTED_CAMEL_PHASES				((OM_type) 5083)	/* GSM CH */
#define MAP_A_SUPPORTED_CAMEL_PHASES				((OM_type)20049)	/* GSM */
#define MAP_A_SUPPORTED_CCBS_PHASE				((OM_type) 5086)	/* GSM CH */
#define MAP_A_SUPPORTED_FEATURES				((OM_type) 3401)	/* GSM MS */
#define MAP_A_SUPPORTED_GAD_SHAPES				((OM_type) 9078)	/* GSM LS */
#define MAP_A_SUPPORTED_LCS_CAPABILITY_SETS			((OM_type) 3402)	/* GSM MS */
#define MAP_A_SUPPORTED_LCS_CAPABILITY_SETS			((OM_type) 9079)	/* GSM LS */
#define MAP_A_SUPPORTED_LCS_CAPABILITY_SETS			((OM_type)20050)	/* GSM */
#define MAP_A_SUPPORTED_RAT_TYPES_INDICATOR			((OM_type) 3403)	/* GSM MS */
#define MAP_A_SUPPORTED_SGSN_CAMEL_PHASES			((OM_type) 3404)	/* GSM MS */
#define MAP_A_SUPPORTED_VLR_CAMEL_PHASES			((OM_type) 3405)	/* GSM MS */
#define MAP_A_SUPPRESS_INCOMING_CALL_BARRING			((OM_type) 5087)	/* GSM CH */
#define MAP_A_SUPPRESSION_OF_ANNOUNCEMENT			((OM_type) 5088)	/* GSM CH */
#define MAP_A_SUPPRESS_MT_SS					((OM_type) 5089)	/* GSM CH */
#define MAP_A_SUPPRESS_T_CSI					((OM_type) 5090)	/* GSM CH */
#define MAP_A_SUPPRESS_VT_CSI					((OM_type) 5091)	/* GSM CH */
#define MAP_A_TALKER_CHANNEL_PARAMETER				((OM_type) 8027)	/* GSM GC */
#define MAP_A_TALKER_PRIORITY					((OM_type) 8028)	/* GSM GC */
#define MAP_A_TARGET_CELL_ID					((OM_type) 3406)	/* GSM MS */
#define MAP_A_TARGET_MSC_NUMBER					((OM_type) 3407)	/* GSM MS */
#define MAP_A_TARGET_MS						((OM_type) 9080)	/* GSM LS */
#define MAP_A_TARGET_RNC_ID					((OM_type) 3408)	/* GSM MS */
#define MAP_A_T_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3410)	/* GSM MS */
#define MAP_A_T_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 5092)	/* GSM CH */
#define MAP_A_T_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type)20051)	/* GSM */
#define MAP_A_T_BCSM_CAMEL_TDP_CRITERIA				((OM_type) 3409)	/* GSM MS */
#define MAP_A_T_BCSM_CAMEL_TDP_DATA_LIST			((OM_type) 3412)	/* GSM MS */
#define MAP_A_T_BCSM_CAMEL_TDP_DATA				((OM_type) 3411)	/* GSM MS */
#define MAP_A_T_BCSM_TRIGGER_DETECTION_POINT			((OM_type) 3413)	/* GSM MS */
#define MAP_A_T_CAUSE_VALUE_CRITERIA				((OM_type) 3414)	/* GSM MS */
#define MAP_A_T_CSI						((OM_type) 3415)	/* GSM MS */
#define MAP_A_T_CSI						((OM_type) 5093)	/* GSM CH */
#define MAP_A_T_CSI						((OM_type)20052)	/* GSM */
#define MAP_A_TEID_FOR_GN_AND_GP				((OM_type) 3416)	/* GSM MS */
#define MAP_A_TEID_FOR_IU					((OM_type) 3417)	/* GSM MS */
#define MAP_A_TELESERVICE_LIST					((OM_type) 3418)	/* GSM MS */
#define MAP_A_TELESERVICE					((OM_type)10029)	/* GSM */
#define MAP_A_TELESERVICE					((OM_type)10029)	/* GSM ERR */
#define MAP_A_TELESERVICE					((OM_type) 8029)	/* GSM GC */
#define MAP_A_TERMINATION_CAUSE					((OM_type) 9081)	/* GSM LS */
#define MAP_A_TIF_CSI_NOTIFICATION_TO_CSE			((OM_type) 3420)	/* GSM MS */
#define MAP_A_TIF_CSI						((OM_type) 3419)	/* GSM MS */
#define MAP_A_TMSI						((OM_type) 6008)	/* GSM */
#define MAP_A_TMSI						((OM_type) 8030)	/* GSM GC */
#define MAP_A_TPDU_TYPE_CRITERION				((OM_type) 3421)	/* GSM MS */
#define MAP_A_TRACE_DEPTH_LIST					((OM_type) 4017)	/* GSM OM */
#define MAP_A_TRACE_EVENT_LIST					((OM_type) 4018)	/* GSM OM */
#define MAP_A_TRACE_INTERFACE_LIST				((OM_type) 4019)	/* GSM OM */
#define MAP_A_TRACE_NE_TYPE_LIST				((OM_type) 4020)	/* GSM OM */
#define MAP_A_TRACE_PROPAGATION_LIST				((OM_type) 3422)	/* GSM MS */
#define MAP_A_TRACE_RECORDING_SESSION_REFERENCE			((OM_type) 4021)	/* GSM OM */
#define MAP_A_TRACE_REFERENCE_2					((OM_type) 4022)	/* GSM OM */
#define MAP_A_TRACE_REFERENCE					((OM_type) 4023)	/* GSM OM */
#define MAP_A_TRACE_SUPPORT_INDICATOR				((OM_type) 4024)	/* GSM OM */
#define MAP_A_TRACE_TYPE					((OM_type) 4025)	/* GSM OM */
#define MAP_A_TRANSACTION_ID					((OM_type) 3423)	/* GSM MS */
#define MAP_A_TRANSLATED_B_NUMBER				((OM_type) 5094)	/* GSM CH */
#define MAP_A_TRANSLATED_B_NUMBER				((OM_type) 6048)	/* GSM SS */
#define MAP_A_TRANSLATED_B_NUMBER				((OM_type)20053)	/* GSM */
#define MAP_A_TRIPLET_LIST					((OM_type) 3424)	/* GSM MS */
#define MAP_A_TYPE_OF_UPDATE					((OM_type) 3425)	/* GSM MS */
#define MAP_A_UESBI_IUA						((OM_type) 3427)	/* GSM MS */
#define MAP_A_UESBI_IUB						((OM_type) 3428)	/* GSM MS */
#define MAP_A_UESBI_IU						((OM_type) 3426)	/* GSM MS */
#define MAP_A_UMTS_SECURITY_CONTEXT_DATA			((OM_type) 3429)	/* GSM MS */
#define MAP_A_UNAUTHORIZED_LCS_CLIENT_DIAGNOSTIC		((OM_type)10030)	/* GSM */
#define MAP_A_UNAUTHORIZED_LCS_CLIENT_DIAGNOSTIC		((OM_type)10030)	/* GSM ERR */
#define MAP_A_UNAUTHORIZED_MESSAGE_ORIGINATOR			((OM_type)10031)	/* GSM */
#define MAP_A_UNAUTHORIZED_MESSAGE_ORIGINATOR			((OM_type)10031)	/* GSM ERR */
#define MAP_A_UNAVAILABILITY_CAUSE				((OM_type) 5095)	/* GSM CH */
#define MAP_A_UNKNOWN_SUBSCRIBER_DIAGNOSTIC			((OM_type)10032)	/* GSM */
#define MAP_A_UNKNOWN_SUBSCRIBER_DIAGNOSTIC			((OM_type)10032)	/* GSM ERR */
#define MAP_A_UPLINK_ATTACHED					((OM_type) 8031)	/* GSM GC */
#define MAP_A_UPLINK_FREE					((OM_type) 8032)	/* GSM GC */
#define MAP_A_UPLINK_REJECT_COMMAND				((OM_type) 8033)	/* GSM GC */
#define MAP_A_UPLINK_RELEASE_COMMAND				((OM_type) 8034)	/* GSM GC */
#define MAP_A_UPLINK_RELEASE_INDICATION				((OM_type) 8035)	/* GSM GC */
#define MAP_A_UPLINK_REQUEST_ACK				((OM_type) 8037)	/* GSM GC */
#define MAP_A_UPLINK_REQUEST					((OM_type) 8036)	/* GSM GC */
#define MAP_A_UPLINK_SEIZED_COMMAND				((OM_type) 8038)	/* GSM GC */
#define MAP_A_USED_RAT_TYPE					((OM_type) 3430)	/* GSM MS */
#define MAP_A_USSD_DATA_CODING_SCHEME				((OM_type) 6049)	/* GSM SS */
#define MAP_A_USSD_STRING					((OM_type) 6050)	/* GSM SS */
#define MAP_A_UTRAN_CODEC_LIST					((OM_type) 3431)	/* GSM MS */
#define MAP_A_UTRAN_POSITIONING_DATA				((OM_type) 9082)	/* GSM LS */
#define MAP_A_UU_DATA						((OM_type) 5096)	/* GSM CH */
#define MAP_A_UU_INDICATOR					((OM_type) 5098)	/* GSM CH */
#define MAP_A_UUI						((OM_type) 5097)	/* GSM CH */
#define MAP_A_UUSCF_INTERACTION					((OM_type) 5099)	/* GSM CH */
#define MAP_A_VBS_GROUP_INDICATION				((OM_type) 3432)	/* GSM MS */
#define MAP_A_VBS_SUBSCRIPTION_DATA				((OM_type) 3433)	/* GSM MS */
#define MAP_A_VELOCITY_ESTIMATE					((OM_type) 9083)	/* GSM LS */
#define MAP_A_VELOCITY_REQUEST					((OM_type) 9084)	/* GSM LS */
#define MAP_A_VERTICAL_ACCURACY					((OM_type) 9085)	/* GSM LS */
#define MAP_A_VERTICAL_COORDINATE_REQUEST			((OM_type) 9086)	/* GSM LS */
#define MAP_A_VGCS_GROUP_INDICATION				((OM_type) 3434)	/* GSM MS */
#define MAP_A_VGCS_SUBSCRIPTION_DATA				((OM_type) 3435)	/* GSM MS */
#define MAP_A_V_GMLC_ADDRESS					((OM_type) 3436)	/* GSM MS */
#define MAP_A_V_GMLC_ADDRESS					((OM_type) 9087)	/* GSM LS */
#define MAP_A_V_GMLC_ADDRESS					((OM_type)20054)	/* GSM */
#define MAP_A_VLR_CAMEL_SUBSCRIPTION_DATA			((OM_type) 3437)	/* GSM MS */
#define MAP_A_VLR_CAMEL_SUBSCRIPTION_INFO			((OM_type) 3438)	/* GSM MS */
#define MAP_A_VLR_CAPABILITY					((OM_type) 3439)	/* GSM MS */
#define MAP_A_VLR_NUMBER					((OM_type) 3440)	/* GSM MS */
#define MAP_A_VMSC_ADDRESS					((OM_type) 5100)	/* GSM CH */
#define MAP_A_VOICE_BROADCAST_DATA				((OM_type) 3441)	/* GSM MS */
#define MAP_A_VOICE_GROUP_CALL_DATA				((OM_type) 3442)	/* GSM MS */
#define MAP_A_VPLMN_ADDRESS_ALLOWED				((OM_type) 3443)	/* GSM MS */
#define MAP_A_VSTK						((OM_type) 8039)	/* GSM GC */
#define MAP_A_VSTK_RAND						((OM_type) 8040)	/* GSM GC */
#define MAP_A_VT_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3444)	/* GSM MS */
#define MAP_A_VT_CSI						((OM_type) 3445)	/* GSM MS */
#define MAP_A_VT_IM_BCSM_CAMEL_TDP_CRITERIA_LIST		((OM_type) 3446)	/* GSM MS */
#define MAP_A_VT_IM_CSI						((OM_type) 3447)	/* GSM MS */
#define MAP_A_WRONG_PASSWORD_ATTEMPTS_COUNTER			((OM_type) 3448)	/* GSM MS */
#define MAP_A_XRES						((OM_type) 3449)	/* GSM MS */
#define MAP_A_ZONE_CODE						((OM_type) 3450)	/* GSM MS */

#endif				/* __XMAP_GSM_XX_H__ */
