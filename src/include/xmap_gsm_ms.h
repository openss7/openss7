/*****************************************************************************

 @(#) src/include/xmap_gsm_ms.h

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

#ifndef __XMAP_GSM_MS_H__
#define __XMAP_GSM_MS_H__

#include <xom.h>
#include <xmap.h>
#include <xmap_gsm.h>
#include <xmap_gsm_om.h>
#include <xmap_gsm_ss.h>

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-ms(1) }
 */
#define OMP_O_MAP_GSM_MS_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x01"

/* Intermediate object identifier macro: */
#define mapP_gsm_ms(X) (OMP_O_MAP_GSM_MS_PKG# #X)

/* Application Contexts */
/* This application context is used between HLR and VLR for location updating
 * procedures. */
#define OMP_O_MAP_NETWORK_LOC_UP_CONTEXT			mapP_acId(\x01)
#define OMP_O_MAP_NETWORK_LOC_UP_CONTEXT_V3			mapP_acId(\x01\x03)
#define OMP_O_MAP_NETWORK_LOC_UP_CONTEXT_V2			mapP_acId(\x01\x02)
#define OMP_O_MAP_NETWORK_LOC_UP_CONTEXT_V1			mapP_acId(\x01\x01)

/* This application context is used between HLR and VLR or between HLR and SGSN
 * for location cancellation procedures.  For the HLR-SGSN interface only
 * version 3 of this application context is applicable. */
#define OMP_O_MAP_LOCATION_CANCELLATION_CONTEXT			mapP_acId(\x02)
#define OMP_O_MAP_LOCATION_CANCELLATION_CONTEXT_V3		mapP_acId(\x02\x03)
#define OMP_O_MAP_LOCATION_CANCELLATION_CONTEXT_V2		mapP_acId(\x02\x02)
#define OMP_O_MAP_LOCATION_CANCELLATION_CONTEXT_V1		mapP_acId(\x02\x01)

/* This application context is used between HLR and VLR or between HLR and SGSN
 * for location register restart procedures.  For the HLR-SGSN interface version
 * 1 and version 2 of this application context are applicable. */
#define OMP_O_MAP_RESET_CONTEXT					mapP_acId(\x0A)
#define OMP_O_MAP_RESET_CONTEXT_V2				mapP_acId(\x0A\x02)
#define OMP_O_MAP_RESET_CONTEXT_V1				mapP_acId(\x0A\x01)

/* This application context is used for handover procedures between MSCs. */
#define OMP_O_MAP_HANDOVER_CONTROL_CONTEXT			mapP_acId(\x0B)
#define OMP_O_MAP_HANDOVER_CONTROL_CONTEXT_V3			mapP_acId(\x0B\x03)
#define OMP_O_MAP_HANDOVER_CONTROL_CONTEXT_V2			mapP_acId(\x0B\x02)
#define OMP_O_MAP_HANDOVER_CONTROL_CONTEXT_V1			mapP_acId(\x0B\x01)

/* This application context is used for equipment checking between MSC and EIR
 * or between SGSN and EIR.  For the SGSN-EIR interface version 1 and version 2
 * and version 3 of this application context are applicable. */
#define OMP_O_MAP_EQUIPMENT_MNGT_CONTEXT			mapP_acId(\x0D)
#define OMP_O_MAP_EQUIPMENT_MNGT_CONTEXT_V3			mapP_acId(\x0D\x03)
#define OMP_O_MAP_EQUIPMENT_MNGT_CONTEXT_V2			mapP_acId(\x0D\x02)
#define OMP_O_MAP_EQUIPMENT_MNGT_CONTEXT_V1			mapP_acId(\x0D\x01)

/* This application context is used for authentication information retrieval
 * between HLR and VLR or between HLR and SGSN.  For the HLR-SGSN interface
 * version 1 and version 2 and version 3 of this application context are
 * applicable. */
#define OMP_O_MAP_INFO_RETRIEVAL_CONTEXT			mapP_acId(\x0E)
#define OMP_O_MAP_INFO_RETRIEVAL_CONTEXT_V3			mapP_acId(\x0E\x03)
#define OMP_O_MAP_INFO_RETRIEVAL_CONTEXT_V2			mapP_acId(\x0E\x02)
#define OMP_O_MAP_INFO_RETRIEVAL_CONTEXT_V1			mapP_acId(\x0E\x01)

/* This application context is used for information retrieval between VLRs. */
#define OMP_O_MAP_INTER_VLR_INFO_RETRIEVAL_CONTEXT		mapP_acId(\x0F)
#define OMP_O_MAP_INTER_VLR_INFO_RETRIEVAL_CONTEXT_V3		mapP_acId(\x0F\x03)
#define OMP_O_MAP_INTER_VLR_INFO_RETRIEVAL_CONTEXT_V2		mapP_acId(\x0F\x02)
#define OMP_O_MAP_INTER_VLR_INFO_RETRIEVAL_CONTEXT_V1		mapP_acId(\x0F\x01)

/* This application context is used for stand alone subscriber data management
 * between HLR and VLR or between HLR and SGSN.  For the HLR-SGSN interface,
 * only version 3 of this application context is applicable. */
#define OMP_O_MAP_SUBSCRIBER_DATA_MNGT_CONTEXT			mapP_acId(\x10)
#define OMP_O_MAP_SUBSCRIBER_DATA_MNGT_CONTEXT_V3		mapP_acId(\x10\x03)
#define OMP_O_MAP_SUBSCRIBER_DATA_MNGT_CONTEXT_V2		mapP_acId(\x10\x02)
#define OMP_O_MAP_SUBSCRIBER_DATA_MNGT_CONTEXT_V1		mapP_acId(\x10\x01)

/* This application context is used between HLR and VLR or between HLR and SGSN
 * for MS purging procedures.  For the SGSN-HLR interface only version 3 of this
 * application context is applicable. */
#define OMP_O_MAP_MS_PURGING_CONTEXT				mapP_acId(\x1B)
#define OMP_O_MAP_MS_PURGING_CONTEXT_V3				mapP_acId(\x1B\x03)
#define OMP_O_MAP_MS_PURGING_CONTEXT_V2				mapP_acId(\x1B\x02)
#define OMP_O_MAP_MS_PURGING_CONTEXT_V1				mapP_acId(\x1B\x01)
/* purgingPackage-v3 */

/* This application context is used between HLR and VLR or between HLR and SGSN
 * for subscriber information enquiry procedures. */
#define OMP_O_MAP_SUBSCRIBER_INFO_ENQUIRY_CONTEXT		mapP_acId(\x1C)
#define OMP_O_MAP_SUBSCRIBER_INFO_ENQUIRY_CONTEXT_V3		mapP_acId(\x1C\x03)
/* subscriberInfomrationEnquiryPackage-v3 */

/* This application context is used between gsmSCF and HLR or between gsmSCF and
 * GMLC or between gsmSCF and NPLR for any time information enquiry procedures. */
#define OMP_O_MAP_ANY_TIME_INFO_ENQUIRY_CONTEXT			mapP_acId(\x1D)
#define OMP_O_MAP_ANY_TIME_INFO_ENQUIRY_CONTEXT_V3		mapP_acId(\x1D\x03)
/* anyTimeInformationEnquiryPackage-v3 */

/* This application context is used between HLR and SGSN for grps location
 * updating procedures. */
#define OMP_O_MAP_GPRS_LOCATION_UPDATE_CONTEXT			mapP_acId(\x20)
#define OMP_O_MAP_GPRS_LOCATION_UPDATE_CONTEXT_V3		mapP_acId(\x20\x03)
/* gprsLocationUpdatingPackage-v3 */

/* This application context is used between HLR and GGSN when retrieving gprs
 * location information. */
#define OMP_O_MAP_GPRS_LOCATION_INFO_RETRIEVAL_CONTEXT		mapP_acId(\x21)
#define OMP_O_MAP_GPRS_LOCATION_INFO_RETRIEVAL_CONTEXT_V4	mapP_acId(\x21\x04)
#define OMP_O_MAP_GPRS_LOCATION_INFO_RETRIEVAL_CONTEXT_V3	mapP_acId(\x21\x03)
/* gprsInterrogationPackage-v4 */

/* This application context is used between HLR and GGSN to inform that network
 * requested PDP-context activation has failed. */
#define OMP_O_MAP_FAILURE_REPORT_CONTEXT			mapP_acId(\x22)
#define OMP_O_MAP_FAILURE_REPORT_CONTEXT_V3			mapP_acId(\x22\x03)
/* failureReportingPackage-v3 */

/* This application context is used between HLR and GGSN for notifying that GPRS
 * subscriber is present again. */
#define OMP_O_MAP_GPRS_NOTIFY_CONTEXT				mapP_acId(\x23)
#define OMP_O_MAP_GPRS_NOTIFY_CONTEXT_V3			mapP_acId(\x23\x03)
/* grpsNotifyingPackage-v3 */

/* This application context is used between HLR and MSC (Visited MSC or Gateway
 * MSC) for service termination services within IST procedures. */
#define OMP_O_MAP_SERVICE_TERMINATION_CONTEXT			mapP_acId(\x09)
#define OMP_O_MAP_SERVICE_TERMINATION_CONTEXT_V3		mapP_acId(\x09\x03)
/* serviceTerminationPackage-v3 */

/* This application context is used between VLR and gsmSCF for Mobility
 * Management event notification procedures. */
#define OMP_O_MAP_MM_EVENT_REPORTING_CONTEXT			mapP_acId(\x2A)
#define OMP_O_MAP_MM_EVENT_REPORTING_CONTEXT_V3			mapP_acId(\x2A\x03)
/* mm-EventReportingPackage-v3 */

/* This application context is used between gsmSCF and HLR for any time
 * information handling procedures. */
#define OMP_O_MAP_ANY_TIME_INFO_HANDLING_CONTEXT		mapP_acId(\x2B)
#define OMP_O_MAP_ANY_TIME_INFO_HANDLING_CONTEXT_V3		mapP_acId(\x2B\x03)
/* anyTimeInformationHandlingPackage-v3 */

/* This application context is used between HLR and gsmSCF for Subscriber Data
 * mondification notification procedures. */
#define OMP_O_MAP_SUBSCRIBER_DATA_MODIFICATION_CONTEXT		mapP_acId(\x16)
#define OMP_O_MAP_SUBSCRIBER_DATA_MODIFICATION_CONTEXT_V3	mapP_acId(\x16\x03)

/* This application context is used between VLR and HLR or SGSN and HLR for
 * reporting of authentication failures. */
#define OMP_O_MAP_AUTHENTICATION_FAILURE_REPORT_CONTEXT		mapP_acId(\x17)
#define OMP_O_MAP_AUTHENTICATION_FAILURE_REPORT_CONTEXT_V3	mapP_acId(\x17\x03)
/* authenticationFailureReportPackage-v3 */

/* OM class names (prefixed MAP_C_) */

#if 0
/* location registration operations */
#define OMP_O_MAP_C_UPDATE_LOCATION_ARG				mapP_gsm_ms(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_UPDATE_LOCATION_RES				mapP_gsm_ms(\x87\x6A)	/* 1002 */
#define OMP_O_MAP_C_CANCEL_LOCATION_ARG				mapP_gsm_ms(\x87\x6B)	/* 1003 */
#define OMP_O_MAP_C_CANCEL_LOCATION_RES				mapP_gsm_ms(\x87\x6C)	/* 1004 */
#define OMP_O_MAP_C_PURGE_MS_ARG				mapP_gsm_ms(\x87\x6D)	/* 1005 */
#define OMP_O_MAP_C_PURGE_MS_RES				mapP_gsm_ms(\x87\x6E)	/* 1006 */
#define OMP_O_MAP_C_SEND_IDENTIFICATION_ARG			mapP_gsm_ms(\x87\x6F)	/* 1007 */
#define OMP_O_MAP_C_SEND_IDENTIFICATION_RES			mapP_gsm_ms(\x87\x70)	/* 1008 */
/* gprs location registration operations */
#define OMP_O_MAP_C_UPDATE_GPRS_LOCATION_ARG			mapP_gsm_ms(\x87\x71)	/* 1009 */
#define OMP_O_MAP_C_UPDATE_GPRS_LOCATION_RES			mapP_gsm_ms(\x87\x72)	/* 1010 */
/* subscriber information enquiry operations */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_INFO_ARG			mapP_gsm_ms(\x87\x73)	/* 1011 */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_INFO_RES			mapP_gsm_ms(\x87\x74)	/* 1012 */
/* any time information enquiry operations */
#define OMP_O_MAP_C_ANY_TIME_INTERROGATION_ARG			mapP_gsm_ms(\x87\x75)	/* 1013 */
#define OMP_O_MAP_C_ANY_TIME_INTERROGATION_RES			mapP_gsm_ms(\x87\x76)	/* 1014 */
/* any time information handling operations */
#define OMP_O_MAP_C_ANY_TIME_SUBSCRIPTION_INTERROGATION_ARG	mapP_gsm_ms(\x87\x77)	/* 1015 */
#define OMP_O_MAP_C_ANY_TIME_SUBSCRIPTION_INTERROGATION_RES	mapP_gsm_ms(\x87\x78)	/* 1016 */
#define OMP_O_MAP_C_ANY_TIME_MODIFICATION_ARG			mapP_gsm_ms(\x87\x79)	/* 1017 */
#define OMP_O_MAP_C_ANY_TIME_MODIFICATION_RES			mapP_gsm_ms(\x87\x7A)	/* 1018 */
/* subscriber data modification notification operations */
#define OMP_O_MAP_C_NOTE_SUBSCRIBER_DATA_MODIFIED_ARG		mapP_gsm_ms(\x87\x7B)	/* 1019 */
#define OMP_O_MAP_C_NOTE_SUBSCRIBER_DATA_MODIFIED_RES		mapP_gsm_ms(\x87\x7C)	/* 1020 */
/* handover operations */
#define OMP_O_MAP_C_PREPARE_HO_ARG				mapP_gsm_ms(\x87\x7D)	/* 1021 */
#define OMP_O_MAP_C_PREPARE_HO_RES				mapP_gsm_ms(\x87\x7E)	/* 1022 */
#define OMP_O_MAP_C_SEND_END_SIGNAL_ARG				mapP_gsm_ms(\x87\x7F)	/* 1023 */
#define OMP_O_MAP_C_SEND_END_SIGNAL_RES				mapP_gsm_ms(\x87\x00)	/* 1024 */
#define OMP_O_MAP_C_PROCESS_ACCESS_SIGNALLING_ARG		mapP_gsm_ms(\x88\x01)	/* 1025 */
#define OMP_O_MAP_C_FORWARD_ACCESS_SIGNALLING_ARG		mapP_gsm_ms(\x88\x02)	/* 1026 */
#define OMP_O_MAP_C_PREPARE_SUBSEQUENT_HO_ARG			mapP_gsm_ms(\x88\x03)	/* 1027 */
#define OMP_O_MAP_C_PREPARE_SUBSEQUENT_HO_RES			mapP_gsm_ms(\x88\x04)	/* 1028 */
/* authentication management operations */
#define OMP_O_MAP_C_SEND_AUTHENTICATION_INFO_ARG		mapP_gsm_ms(\x88\x05)	/* 1029 */
#define OMP_O_MAP_C_SEND_AUTHENTICATION_INFO_RES		mapP_gsm_ms(\x88\x06)	/* 1030 */
#define OMP_O_MAP_C_AUTHENTICATION_FAILURE_REPORT_ARG		mapP_gsm_ms(\x88\x07)	/* 1031 */
#define OMP_O_MAP_C_AUTHENTICATION_FAILURE_REPORT_RES		mapP_gsm_ms(\x88\x08)	/* 1032 */
/* IMEI management operations */
#define OMP_O_MAP_C_CHECK_IMEI_ARG				mapP_gsm_ms(\x88\x09)	/* 1033 */
#define OMP_O_MAP_C_CHECK_IMEI_RES				mapP_gsm_ms(\x88\x0A)	/* 1034 */
/* subscriber management operations */
#define OMP_O_MAP_C_INSERT_SUBSCRIBER_DATA_ARG			mapP_gsm_ms(\x88\x0B)	/* 1035 */
#define OMP_O_MAP_C_INSERT_SUBSCRIBER_DATA_RES			mapP_gsm_ms(\x88\x0C)	/* 1006 */
#define OMP_O_MAP_C_DELETE_SUBSCRIBER_DATA_ARG			mapP_gsm_ms(\x88\x0D)	/* 1037 */
#define OMP_O_MAP_C_DELETE_SUBSCRIBER_DATA_RES			mapP_gsm_ms(\x88\x0E)	/* 1038 */
/* fault recovery operations */
#define OMP_O_MAP_C_RESET_ARG					mapP_gsm_ms(\x88\x0F)	/* 1039 */
#define OMP_O_MAP_C_FORWARD_CHECK_SS_INDICATION_ARG		mapP_gsm_ms(\x88\x10)	/* 1040 */
#define OMP_O_MAP_C_RESTORE_DATA_ARG				mapP_gsm_ms(\x88\x11)	/* 1041 */
#define OMP_O_MAP_C_RESTORE_DATA_RES				mapP_gsm_ms(\x88\x12)	/* 1042 */
/* gprs location information retrieval operations */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_FOR_GPRS_ARG		mapP_gsm_ms(\x88\x13)	/* 1043 */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_FOR_GPRS_RES		mapP_gsm_ms(\x88\x14)	/* 1044 */
/* failure reporting operations */
#define OMP_O_MAP_C_FAILURE_REPORT_ARG				mapP_gsm_ms(\x88\x15)	/* 1045 */
#define OMP_O_MAP_C_FAILURE_REPORT_RES				mapP_gsm_ms(\x88\x16)	/* 1046 */
/* gprs notification operations */
#define OMP_O_MAP_C_NOTE_MS_PRESENT_FOR_GPRS_ARG		mapP_gsm_ms(\x88\x17)	/* 1047 */
#define OMP_O_MAP_C_NOTE_MS_PRESENT_FOR_GPRS_RES		mapP_gsm_ms(\x88\x18)	/* 1048 */
/* mobility management operations */
#define OMP_O_MAP_C_NOTE_MM_EVENT_ARG				mapP_gsm_ms(\x88\x19)	/* 1049 */
#define OMP_O_MAP_C_NOTE_MM_EVENT_RES				mapP_gsm_ms(\x88\x10)	/* 1050 */
#endif

/* Class names for data types: */
#define OMP_O_MAP_C_ADD_INFO					mapP_gsm_ms(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_ALLOWED_UMTS_ALGORITHMS			mapP_gsm_ms(\x87\x6A)	/* 1002 */
#define OMP_O_MAP_C_AMBR					mapP_gsm_ms(\x87\x6B)	/* 1003 */
#define OMP_O_MAP_C_ANY_TIME_INTERROGATION_ARG			mapP_gsm_ms(\x87\x6C)	/* 1004 */
#define OMP_O_MAP_C_ANY_TIME_INTERROGATION_RES			mapP_gsm_ms(\x87\x6D)	/* 1005 */
#define OMP_O_MAP_C_ANY_TIME_MODIFICATION_ARG			mapP_gsm_ms(\x87\x6E)	/* 1006 */
#define OMP_O_MAP_C_ANY_TIME_MODIFICATION_RES			mapP_gsm_ms(\x87\x6F)	/* 1007 */
#define OMP_O_MAP_C_ANY_TIME_SUBSCRIPTION_INTERROGATION_ARG	mapP_gsm_ms(\x87\x70)	/* 1008 */
#define OMP_O_MAP_C_ANY_TIME_SUBSCRIPTION_INTERROGATION_RES	mapP_gsm_ms(\x87\x71)	/* 1009 */
#define OMP_O_MAP_C_APN_CONFIGURATION_PROFILE			mapP_gsm_ms(\x87\x72)	/* 1010 */
#define OMP_O_MAP_C_APN_CONFIGURATION				mapP_gsm_ms(\x87\x72)	/* 1010 */
#define OMP_O_MAP_C_AUTHENTICATION_FAILURE_REPORT_ARG		mapP_gsm_ms(\x87\x73)	/* 1011 */
#define OMP_O_MAP_C_AUTHENTICATION_FAILURE_REPORT_RES		mapP_gsm_ms(\x87\x74)	/* 1012 */
#define OMP_O_MAP_C_AUTHENTICATION_QUINTUPLET			mapP_gsm_ms(\x87\x75)	/* 1013 */
#define OMP_O_MAP_C_AUTHENTICATION_SET_LIST			mapP_gsm_ms(\x87\x76)	/* 1014 */
#define OMP_O_MAP_C_AUTHENTICATION_TRIPLET			mapP_gsm_ms(\x87\x77)	/* 1015 */
#define OMP_O_MAP_C_BASIC_SERVICE_CRITERIA			mapP_gsm_ms(\x87\x78)	/* 1016 */
#define OMP_O_MAP_C_BASIC_SERVICE_LIST				mapP_gsm_ms(\x87\x79)	/* 1017 */
#define OMP_O_MAP_C_BEARER_SERVICE_LIST				mapP_gsm_ms(\x87\x7A)	/* 1018 */
#define OMP_O_MAP_C_BSSMAP_SERVICE_HANDOVER_INFO		mapP_gsm_ms(\x87\x7B)	/* 1019 */
#define OMP_O_MAP_C_BSSMAP_SERVICE_HANDOVER_LIST		mapP_gsm_ms(\x87\x7C)	/* 1020 */
#define OMP_O_MAP_C_CALL_BARRING_DATA				mapP_gsm_ms(\x87\x7D)	/* 1021 */
#define OMP_O_MAP_C_CALL_FORWARDING_DATA			mapP_gsm_ms(\x87\x7E)	/* 1022 */
#define OMP_O_MAP_C_CAMEL_SUBSCRIPTION_INFO			mapP_gsm_ms(\x87\x7F)	/* 1023 */
#define OMP_O_MAP_C_CANCEL_LOCATION_ARG				mapP_gsm_ms(\x88\x00)	/* 1024 */
#define OMP_O_MAP_C_CANCEL_LOCATION_RES				mapP_gsm_ms(\x88\x01)	/* 1025 */
#define OMP_O_MAP_C_CHECK_IMEI_ARG				mapP_gsm_ms(\x88\x02)	/* 1026 */
#define OMP_O_MAP_C_CHECK_IMEI_RES				mapP_gsm_ms(\x88\x03)	/* 1027 */
#define OMP_O_MAP_C_CHOSEN_RADIO_RESOURCE_INFORMATION		mapP_gsm_ms(\x88\x04)	/* 1028 */
#define OMP_O_MAP_C_CODEC_LIST					mapP_gsm_ms(\x88\x05)	/* 1029 */
#define OMP_O_MAP_C_CONTEXT_ID_LIST				mapP_gsm_ms(\x88\x06)	/* 1030 */
#define OMP_O_MAP_C_CSG_SUBSCRIPTION_DATA_LIST			mapP_gsm_ms(\x88\x07)	/* 1031 */
#define OMP_O_MAP_C_CSG_SUBSCRIPTION_DATA			mapP_gsm_ms(\x88\x08)	/* 1032 */
#define OMP_O_MAP_C_CUG_FEATURE_LIST				mapP_gsm_ms(\x88\x09)	/* 1033 */
#define OMP_O_MAP_C_CUG_FEATURE					mapP_gsm_ms(\x88\x0A)	/* 1034 */
#define OMP_O_MAP_C_CUG_INFO					mapP_gsm_ms(\x88\x0B)	/* 1035 */
#define OMP_O_MAP_C_CUG_SUBSCRIPTION_LIST			mapP_gsm_ms(\x88\x0C)	/* 1036 */
#define OMP_O_MAP_C_CUG_SUBSCRIPTION				mapP_gsm_ms(\x88\x0D)	/* 1037 */
#define OMP_O_MAP_C_CURRENT_SECURITY_CONTEXT			mapP_gsm_ms(\x88\x0E)	/* 1038 */
#define OMP_O_MAP_C_D_CSI					mapP_gsm_ms(\x88\x0F)	/* 1039 */
#define OMP_O_MAP_C_DELETE_SUBSCRIBER_DATA_ARG			mapP_gsm_ms(\x88\x10)	/* 1040 */
#define OMP_O_MAP_C_DELETE_SUBSCRIBER_DATA_RES			mapP_gsm_ms(\x88\x11)	/* 1041 */
#define OMP_O_MAP_C_DESTINATION_NUMBER_CRITERIA			mapP_gsm_ms(\x88\x12)	/* 1042 */
#define OMP_O_MAP_C_DESTINATION_NUMBER_LENGTH_LIST		mapP_gsm_ms(\x88\x13)	/* 1043 */
#define OMP_O_MAP_C_DESTINATION_NUMBER_LIST			mapP_gsm_ms(\x88\x14)	/* 1044 */
#define OMP_O_MAP_C_DP_ANALYSED_INFO_CRITERIA_LIST		mapP_gsm_ms(\x88\x15)	/* 1045 */
#define OMP_O_MAP_C_DP_ANALYSED_INFO_CRITERIUM			mapP_gsm_ms(\x88\x16)	/* 1046 */
#define OMP_O_MAP_C_EPS_AUTHENTICATION_SET_LIST			mapP_gsm_ms(\x88\x17)	/* 1047 */
#define OMP_O_MAP_C_EPS_AV					mapP_gsm_ms(\x88\x18)	/* 1048 */
#define OMP_O_MAP_C_EPS_DATA_LIST				mapP_gsm_ms(\x88\x19)	/* 1201 */
#define OMP_O_MAP_C_EPS_INFO					mapP_gsm_ms(\x88\x19)	/* 1049 */
#define OMP_O_MAP_C_EPS_QOS_SUBSCRIBED				mapP_gsm_ms(\x88\x1A)	/* 1050 */
#define OMP_O_MAP_C_EPS_SUBSCRIPTION_DATA			mapP_gsm_ms(\x88\x1B)	/* 1051 */
#define OMP_O_MAP_C_EPS_SUBSCRIPTION_DATA_WITHDRAW		mapP_gsm_ms(\x88\x1C)	/* 1052 */
#define OMP_O_MAP_C_EXT_BASIC_SERVICE_GROUP_LIST		mapP_gsm_ms(\x88\x1D)	/* 1053 */
#define OMP_O_MAP_C_EXT_CALL_BAR_FEATURE_LIST			mapP_gsm_ms(\x88\x1E)	/* 1054 */
#define OMP_O_MAP_C_EXT_CALL_BAR_INFO				mapP_gsm_ms(\x88\x1F)	/* 1055 */
#define OMP_O_MAP_C_EXT_CALL_BARRING_FEATURE			mapP_gsm_ms(\x88\x20)	/* 1056 */
#define OMP_O_MAP_C_EXT_CALL_BARRING_INFO_FOR_CSE		mapP_gsm_ms(\x88\x21)	/* 1057 */
#define OMP_O_MAP_C_EXTERNAL_CLIENT_LIST			mapP_gsm_ms(\x88\x22)	/* 1058 */
#define OMP_O_MAP_C_EXTERNAL_CLIENT				mapP_gsm_ms(\x88\x23)	/* 1059 */
#define OMP_O_MAP_C_EXT_EXTERNAL_CLIENT_LIST			mapP_gsm_ms(\x88\x24)	/* 1060 */
#define OMP_O_MAP_C_EXT_FORWARDING_INFO_FOR_CSE			mapP_gsm_ms(\x88\x25)	/* 1061 */
#define OMP_O_MAP_C_EXT_FORW_FEATURE_LIST			mapP_gsm_ms(\x88\x26)	/* 1062 */
#define OMP_O_MAP_C_EXT_FORW_FEATURE				mapP_gsm_ms(\x88\x27)	/* 1063 */
#define OMP_O_MAP_C_EXT_FORW_INFO				mapP_gsm_ms(\x88\x28)	/* 1064 */
#define OMP_O_MAP_C_EXT_SS_DATA					mapP_gsm_ms(\x88\x29)	/* 1065 */
#define OMP_O_MAP_C_EXT_SS_INFO_FOR_CSE				mapP_gsm_ms(\x88\x2A)	/* 1066 */
#define OMP_O_MAP_C_EXT_SS_INFO_LIST				mapP_gsm_ms(\x88\x2B)	/* 1067 */
#define OMP_O_MAP_C_EXT_SS_INFO					mapP_gsm_ms(\x88\x2C)	/* 1068 */
#define OMP_O_MAP_C_FAILURE_REPORT_ARG				mapP_gsm_ms(\x88\x2D)	/* 1069 */
#define OMP_O_MAP_C_FAILURE_REPORT_RES				mapP_gsm_ms(\x88\x2E)	/* 1070 */
#define OMP_O_MAP_C_FORWARD_ACCESS_SIGNALLING_ARG		mapP_gsm_ms(\x88\x2F)	/* 1071 */
#define OMP_O_MAP_C_FORWARD_CHECK_SS_INDICATION_ARG		mapP_gsm_ms(\x88\x30)	/* 1072 */
#define OMP_O_MAP_C_GMLC_LIST					mapP_gsm_ms(\x88\x31)	/* 1073 */
#define OMP_O_MAP_C_GPRS_CAMEL_TDP_DATA_LIST			mapP_gsm_ms(\x88\x32)	/* 1074 */
#define OMP_O_MAP_C_GPRS_CAMEL_TDP_DATA				mapP_gsm_ms(\x88\x38)	/* 1080 */
#define OMP_O_MAP_C_GPRS_CSI					mapP_gsm_ms(\x88\x33)	/* 1075 */
#define OMP_O_MAP_C_GPRS_DATA_LIST				mapP_gsm_ms(\x88\x34)	/* 1076 */
#define OMP_O_MAP_C_GPRS_MS_CLASS				mapP_gsm_ms(\x88\x35)	/* 1077 */
#define OMP_O_MAP_C_GPRS_SUBSCRIPTION_DATA			mapP_gsm_ms(\x88\x36)	/* 1078 */
#define OMP_O_MAP_C_GPRS_SUBSCRIPTION_DATA_WITHDRAW		mapP_gsm_ms(\x88\x37)	/* 1079 */
#define OMP_O_MAP_C_GSM_SECURITY_CONTEXT_DATA			mapP_gsm_ms(\x88\x39)	/* 1081 */
#define OMP_O_MAP_C_INSERT_SUBSCRIBER_DATA_ARG			mapP_gsm_ms(\x88\x3C)	/* 1084 */
#define OMP_O_MAP_C_INSERT_SUBSCRIBER_DATA_RES			mapP_gsm_ms(\x88\x3D)	/* 1085 */
#define OMP_O_MAP_C_LCS_INFORMATION				mapP_gsm_ms(\x88\x3E)	/* 1086 */
#define OMP_O_MAP_C_LCS_PRIVACY_CLASS				mapP_gsm_ms(\x88\x3F)	/* 1087 */
#define OMP_O_MAP_C_LCS_PRIVACY_EXCEPTION_LIST			mapP_gsm_ms(\x88\x40)	/* 1088 */
#define OMP_O_MAP_C_LOCATION_AREA				mapP_gsm_ms(\x88\x41)	/* 1089 */
#define OMP_O_MAP_C_LOCATION_INFORMATION_GPRS			mapP_gsm_ms(\x88\x42)	/* 1090 */
#define OMP_O_MAP_C_LOCATION_INFORMATION			mapP_gsm_ms(\x88\x43)	/* 1091 */
#define OMP_O_MAP_C_LSA_DATA_LIST				mapP_gsm_ms(\x88\x44)	/* 1092 */
#define OMP_O_MAP_C_LSA_DATA					mapP_gsm_ms(\x88\x45)	/* 1093 */
#define OMP_O_MAP_C_LSA_IDENTITY_LIST				mapP_gsm_ms(\x88\x46)	/* 1094 */
#define OMP_O_MAP_C_LSA_INFORMATION				mapP_gsm_ms(\x88\x47)	/* 1095 */
#define OMP_O_MAP_C_LSA_INFORMATION_WITHDRAW			mapP_gsm_ms(\x88\x48)	/* 1096 */
#define OMP_O_MAP_C_M_CSI					mapP_gsm_ms(\x88\x49)	/* 1097 */
#define OMP_O_MAP_C_MG_CSI					mapP_gsm_ms(\x88\x4A)	/* 1098 */
#define OMP_O_MAP_C_MNP_INFO_RES				mapP_gsm_ms(\x88\x4B)	/* 1099 */
#define OMP_O_MAP_C_MOBILITY_TRIGGERS				mapP_gsm_ms(\x88\x4C)	/* 1100 */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CB_INFO		mapP_gsm_ms(\x88\x4E)	/* 1102 */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CF_INFO		mapP_gsm_ms(\x88\x4F)	/* 1103 */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CSI		mapP_gsm_ms(\x88\x50)	/* 1104 */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_IP_SM_GW_DATA	mapP_gsm_ms(\x88\x4D)	/* 1101 */
#define OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_ODB_DATA		mapP_gsm_ms(\x88\x51)	/* 1105 */
#define OMP_O_MAP_C_MOLR_CLASS					mapP_gsm_ms(\x88\x52)	/* 1106 */
#define OMP_O_MAP_C_MOLR_LIST					mapP_gsm_ms(\x88\x53)	/* 1107 */
#define OMP_O_MAP_C_MSISDN_BS_LIST				mapP_gsm_ms(\x88\x54)	/* 1108 */
#define OMP_O_MAP_C_MSISDN_BS					mapP_gsm_ms(\x88\x55)	/* 1109 */
#define OMP_O_MAP_C_MT_SMS_CAMEL_TDP_CRITERIA_LIST		mapP_gsm_ms(\x88\x56)	/* 1110 */
#define OMP_O_MAP_C_MT_SMS_CAMEL_TDP_CRITERIA			mapP_gsm_ms(\x88\x57)	/* 1111 */
#define OMP_O_MAP_C_NOTE_MM_EVENT_ARG				mapP_gsm_ms(\x88\x58)	/* 1112 */
#define OMP_O_MAP_C_NOTE_MM_EVENT_RES				mapP_gsm_ms(\x88\x59)	/* 1113 */
#define OMP_O_MAP_C_NOTE_MS_PRESENT_FOR_GPRS_ARG		mapP_gsm_ms(\x88\x5A)	/* 1114 */
#define OMP_O_MAP_C_NOTE_MS_PRESENT_FOR_GPRS_RES		mapP_gsm_ms(\x88\x5B)	/* 1115 */
#define OMP_O_MAP_C_NOTE_SUBSCRIBER_DATA_MODIFIED_ARG		mapP_gsm_ms(\x88\x5C)	/* 1116 */
#define OMP_O_MAP_C_NOTE_SUBSCRIBER_DATA_MODIFIED_RES		mapP_gsm_ms(\x88\x5D)	/* 1117 */
#define OMP_O_MAP_C_O_BCSM_CAMEL_TDP_CRITERIA_LIST		mapP_gsm_ms(\x88\x5E)	/* 1118 */
#define OMP_O_MAP_C_O_BCSM_CAMEL_TDP_CRITERIA			mapP_gsm_ms(\x88\x5F)	/* 1119 */
#define OMP_O_MAP_C_O_BCSM_CAMEL_TDP_DATA_LIST			mapP_gsm_ms(\x88\x60)	/* 1120 */
#define OMP_O_MAP_C_O_BCSM_CAMEL_TDP_DATA			mapP_gsm_ms(\x88\x61)	/* 1121 */
#define OMP_O_MAP_C_O_CAUSE_VALUE_CRITERIA			mapP_gsm_ms(\x88\x62)	/* 1122 */
#define OMP_O_MAP_C_O_CSI					mapP_gsm_ms(\x88\x63)	/* 1123 */
#define OMP_O_MAP_C_ODB_DATA					mapP_gsm_ms(\x88\x64)	/* 1124 */
#define OMP_O_MAP_C_ODB_INFO					mapP_gsm_ms(\x88\x65)	/* 1125 */
#define OMP_O_MAP_C_PAGING_AREA					mapP_gsm_ms(\x88\x66)	/* 1126 */
#define OMP_O_MAP_C_PDN_GW_IDENTITY				mapP_gsm_ms(\x88\x67)	/* 1127 */
#define OMP_O_MAP_C_PDN_GW_UPDATE				mapP_gsm_ms(\x88\x68)	/* 1128 */
#define OMP_O_MAP_C_PDP_CONTEXT_INFO_LIST			mapP_gsm_ms(\x88\x69)	/* 1129 */
#define OMP_O_MAP_C_PDP_CONTEXT_INFO				mapP_gsm_ms(\x88\x6A)	/* 1130 */
#define OMP_O_MAP_C_PDP_CONTEXT					mapP_gsm_ms(\x88\x6B)	/* 1131 */
#define OMP_O_MAP_C_PLMN_CLIENT_LIST				mapP_gsm_ms(\x88\x6C)	/* 1132 */
#define OMP_O_MAP_C_PREPARE_HO_ARG				mapP_gsm_ms(\x88\x6D)	/* 1133 */
#define OMP_O_MAP_C_PREPARE_HO_RES				mapP_gsm_ms(\x88\x6E)	/* 1134 */
#define OMP_O_MAP_C_PREPARE_SUBSEQUENT_HO_ARG			mapP_gsm_ms(\x88\x6F)	/* 1135 */
#define OMP_O_MAP_C_PREPARE_SUBSEQUENT_HO_RES			mapP_gsm_ms(\x88\x70)	/* 1136 */
#define OMP_O_MAP_C_PROCESS_ACCESS_SIGNALLING_ARG		mapP_gsm_ms(\x88\x71)	/* 1137 */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_INFO_ARG			mapP_gsm_ms(\x88\x72)	/* 1138 */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_INFO_RES			mapP_gsm_ms(\x88\x73)	/* 1139 */
#define OMP_O_MAP_C_PS_SUBSCRIBER_STATE				mapP_gsm_ms(\x88\x74)	/* 1140 */
#define OMP_O_MAP_C_PURGE_MS_ARG				mapP_gsm_ms(\x88\x75)	/* 1141 */
#define OMP_O_MAP_C_PURGE_MS_RES				mapP_gsm_ms(\x88\x76)	/* 1142 */
#define OMP_O_MAP_C_QUINTUPLET_LIST				mapP_gsm_ms(\x88\x77)	/* 1143 */
#define OMP_O_MAP_C_RADIO_RESOURCE_LIST				mapP_gsm_ms(\x88\x78)	/* 1144 */
#define OMP_O_MAP_C_RADIO_RESOURCE				mapP_gsm_ms(\x88\x79)	/* 1145 */
#define OMP_O_MAP_C_RELOCATION_NUMBER_LIST			mapP_gsm_ms(\x88\x7A)	/* 1146 */
#define OMP_O_MAP_C_RELOCATION_NUMBER				mapP_gsm_ms(\x88\x7B)	/* 1147 */
#define OMP_O_MAP_C_REQUESTED_INFO				mapP_gsm_ms(\x88\x7C)	/* 1148 */
#define OMP_O_MAP_C_REQUESTED_SUBSCRIPTION_INFO			mapP_gsm_ms(\x88\x7D)	/* 1149 */
#define OMP_O_MAP_C_RESET_ARG					mapP_gsm_ms(\x88\x7E)	/* 1150 */
#define OMP_O_MAP_C_RESTORE_DATA_ARG				mapP_gsm_ms(\x88\x7F)	/* 1151 */
#define OMP_O_MAP_C_RESTORE_DATA_RES				mapP_gsm_ms(\x89\x00)	/* 1152 */
#define OMP_O_MAP_C_RESYNCHRONISATION_INFO			mapP_gsm_ms(\x89\x01)	/* 1153 */
#define OMP_O_MAP_C_SELECTED_UMTS_ALGORITHMS			mapP_gsm_ms(\x89\x02)	/* 1154 */
#define OMP_O_MAP_C_SEND_AUTHENTICATION_INFO_ARG		mapP_gsm_ms(\x89\x03)	/* 1155 */
#define OMP_O_MAP_C_SEND_AUTHENTICATION_INFO_RES		mapP_gsm_ms(\x89\x04)	/* 1156 */
#define OMP_O_MAP_C_SEND_END_SIGNAL_ARG				mapP_gsm_ms(\x89\x05)	/* 1157 */
#define OMP_O_MAP_C_SEND_END_SIGNAL_RES				mapP_gsm_ms(\x89\x06)	/* 1158 */
#define OMP_O_MAP_C_SEND_IDENTIFICATION_ARG			mapP_gsm_ms(\x89\x07)	/* 1159 */
#define OMP_O_MAP_C_SEND_IDENTIFICATION_RES			mapP_gsm_ms(\x89\x08)	/* 1160 */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_FOR_GPRS_ARG		mapP_gsm_ms(\x89\x09)	/* 1161 */
#define OMP_O_MAP_C_SEND_ROUTING_INFO_FOR_GPRS_RES		mapP_gsm_ms(\x89\x0A)	/* 1162 */
#define OMP_O_MAP_C_SERVICE_TYPE_LIST				mapP_gsm_ms(\x89\x0B)	/* 1163 */
#define OMP_O_MAP_C_SERVICE_TYPE				mapP_gsm_ms(\x89\x0C)	/* 1164 */
#define OMP_O_MAP_C_SGSN_CAMEL_SUBSCRIPTION_INFO		mapP_gsm_ms(\x89\x0D)	/* 1165 */
#define OMP_O_MAP_C_SGSN_CAPABILITY				mapP_gsm_ms(\x89\x0E)	/* 1166 */
#define OMP_O_MAP_C_SMS_CAMEL_TDP_DATA_LIST			mapP_gsm_ms(\x89\x0F)	/* 1167 */
#define OMP_O_MAP_C_SMS_CAMEL_TDP_DATA				mapP_gsm_ms(\x89\x10)	/* 1168 */
#define OMP_O_MAP_C_SMS_CSI					mapP_gsm_ms(\x89\x11)	/* 1169 */
#define OMP_O_MAP_C_SS_CAMEL_DATA				mapP_gsm_ms(\x89\x12)	/* 1170 */
#define OMP_O_MAP_C_SS_CSI					mapP_gsm_ms(\x89\x13)	/* 1171 */
#define OMP_O_MAP_C_SS_EVENT_LIST				mapP_gsm_ms(\x89\x14)	/* 1172 */
#define OMP_O_MAP_C_SUBSCRIBER_DATA				mapP_gsm_ms(\x89\x15)	/* 1173 */
#define OMP_O_MAP_C_SUBSCRIBER_INFO				mapP_gsm_ms(\x89\x17)	/* 1175 */
#define OMP_O_MAP_C_SUBSCRIBER_STATE				mapP_gsm_ms(\x89\x18)	/* 1176 */
#define OMP_O_MAP_C_SUPER_CHARGER_INFO				mapP_gsm_ms(\x89\x19)	/* 1177 */
#define OMP_O_MAP_C_SUPPORTED_CODECS_LIST			mapP_gsm_ms(\x89\x1A)	/* 1178 */
#define OMP_O_MAP_C_T_BCSM_CAMEL_TDP_CRITERIA_LIST		mapP_gsm_ms(\x89\x1B)	/* 1179 */
#define OMP_O_MAP_C_T_BCSM_CAMEL_TDP_CRITERIA			mapP_gsm_ms(\x89\x1C)	/* 1180 */
#define OMP_O_MAP_C_T_BCSM_CAMEL_TDP_DATA_LIST			mapP_gsm_ms(\x89\x1D)	/* 1181 */
#define OMP_O_MAP_C_T_BCSM_CAMEL_TDP_DATA			mapP_gsm_ms(\x89\x1E)	/* 1182 */
#define OMP_O_MAP_C_T_CAUSE_VALUE_CRITERIA			mapP_gsm_ms(\x89\x1F)	/* 1183 */
#define OMP_O_MAP_C_T_CSI					mapP_gsm_ms(\x89\x20)	/* 1184 */
#define OMP_O_MAP_C_TELESERVICE_LIST				mapP_gsm_ms(\x89\x21)	/* 1185 */
#define OMP_O_MAP_C_TPDU_TYPE_CRITERION				mapP_gsm_ms(\x89\x22)	/* 1186 */
#define OMP_O_MAP_C_TRIPLET_LIST				mapP_gsm_ms(\x89\x23)	/* 1187 */
#define OMP_O_MAP_C_UESBI_IU					mapP_gsm_ms(\x89\x24)	/* 1188 */
#define OMP_O_MAP_C_UMTS_SECURITY_CONTEXT_DATA			mapP_gsm_ms(\x89\x25)	/* 1189 */
#define OMP_O_MAP_C_UPDATE_GPRS_LOCATION_ARG			mapP_gsm_ms(\x89\x26)	/* 1190 */
#define OMP_O_MAP_C_UPDATE_GPRS_LOCATION_RES			mapP_gsm_ms(\x89\x27)	/* 1191 */
#define OMP_O_MAP_C_UPDATE_LOCATION_ARG				mapP_gsm_ms(\x89\x28)	/* 1192 */
#define OMP_O_MAP_C_UPDATE_LOCATION_RES				mapP_gsm_ms(\x89\x29)	/* 1193 */
#define OMP_O_MAP_C_VBS_DATA_LIST				mapP_gsm_ms(\x89\x2A)	/* 1194 */
#define OMP_O_MAP_C_VGCS_DATA_LIST				mapP_gsm_ms(\x89\x2B)	/* 1195 */
#define OMP_O_MAP_C_VLR_CAMEL_SUBSCRIPTION_INFO			mapP_gsm_ms(\x89\x2C)	/* 1196 */
#define OMP_O_MAP_C_VLR_CAPABILITY				mapP_gsm_ms(\x89\x2D)	/* 1197 */
#define OMP_O_MAP_C_VOICE_BROADCAST_DATA			mapP_gsm_ms(\x89\x2E)	/* 1198 */
#define OMP_O_MAP_C_VOICE_GROUP_CALL_DATA			mapP_gsm_ms(\x89\x2F)	/* 1199 */
#define OMP_O_MAP_C_ZONE_CODE_LIST				mapP_gsm_ms(\x89\x30)	/* 1200 */

/* OM attribute names (prefixed MAP_) */

#define MAP_A_ACCESS_RESTRICTION_DATA				((OM_type) 3001)
#define MAP_A_ACCESS_TYPE					((OM_type) 3002)
#define MAP_A_ADD_CAPABILITY					((OM_type) 3003)
#define MAP_A_ADD_INFO						((OM_type) 3004)
#define MAP_A_ADDITIONAL_REQUESTED_CAMEL_SUBSCRIPTION_INFO	((OM_type) 3006)
#define MAP_A_ADDITIONAL_VECTORS_ARE_FOR_EPS			((OM_type) 3008)
#define MAP_A_ADD_LCS_PRIVACY_EXCEPTION_LIST			((OM_type) 3009)
#define MAP_A_ALL_EPS_DATA					((OM_type) 3001)
#define MAP_A_ALL_GPRS_DATA					((OM_type) 3002)
#define MAP_A_ALL_LSA_DATA					((OM_type) 3004)
#define MAP_A_ALLOWED_GSM_ALGORITHMS				((OM_type) 3005)
#define MAP_A_ALLOWED_UMTS_ALGORITHMS				((OM_type) 3006)
#define MAP_A_ALTERNATIVE_CHANNEL_TYPE				((OM_type) 3007)
#define MAP_A_AMBR						((OM_type) 3008)
#define MAP_A_APM_OI_REPLACEMENT_WINDOW				((OM_type) 3010)
#define MAP_A_APN						((OM_type) 3011)
#define MAP_A_APN_CONFIGURATION					((OM_type) 3012)
#define MAP_A_APN_CONFIGURATION_PROFILE				((OM_type) 3013)
#define MAP_A_APN_IN_USE					((OM_type) 3014)
#define MAP_A_APN_OI_REPLACEMENT				((OM_type) 3015)
#define MAP_A_APN_SUBSCRIBED					((OM_type) 3016)
#define MAP_A_ARP						((OM_type) 3017)
#define MAP_A_ASSUMED_IDLE					((OM_type) 3019)
#define MAP_A_AUTHENTICATION_QUINTUPLET				((OM_type) 3020)
#define MAP_A_AUTHENTICATION_SET_LIST				((OM_type) 3021)
#define MAP_A_AUTHENTICATION_TRIPLET				((OM_type) 3022)
#define MAP_A_AUTN						((OM_type) 3023)
#define MAP_A_AUTS						((OM_type) 3024)
#define MAP_A_BASIC_SERVICE_CRITERIA				((OM_type) 3026)
#define MAP_A_BASIC_SERVICE_LIST				((OM_type) 3028)
#define MAP_A_BEARER_SERVICE_LIST				((OM_type) 3029)
#define MAP_A_BMUEF						((OM_type) 3030)
#define MAP_A_BROADCAST_INIT_ENTITLEMENT			((OM_type) 3031)
#define MAP_A_BSSMAP_SERVICE_HANDOVER				((OM_type) 3032)
#define MAP_A_BSSMAP_SERVICE_HANDOVER_INFO			((OM_type) 3033)
#define MAP_A_BSSMAP_SERVICE_HANDOVER_LIST			((OM_type) 3034)
#define MAP_A_CALL_BARRING_DATA					((OM_type) 3035)
#define MAP_A_CALL_BARRING_INFO_FOR_CSE				((OM_type) 3038)
#define MAP_A_CALL_FORWARDING_DATA				((OM_type) 3039)
#define MAP_A_CALL_TYPE_CRITERIA				((OM_type) 3041)
#define MAP_A_CAMEL_BUSY					((OM_type) 3042)
#define MAP_A_CAMEL_CAPABILITY_HANDLING				((OM_type) 3043)
#define MAP_A_CAMEL_SUBSCRIPTION_INFO				((OM_type) 3044)
#define MAP_A_CAMEL_SUBSCRIPTION_INFO_WITHDRAW			((OM_type) 3045)
#define MAP_A_CANCELLATION_TYPE					((OM_type) 3046)
#define MAP_A_CATEGORY						((OM_type) 3047)
#define MAP_A_CAUSE_VALUE					((OM_type) 3048)
#define MAP_A_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_OR_LAI		((OM_type) 3049)
#define MAP_A_CHARGING_CHARACTERISTICS				((OM_type) 3050)
#define MAP_A_CHARGING_CHARACTERISTICS_WITHDRAW			((OM_type) 3051)
#define MAP_A_CHARGING_ID					((OM_type) 3052)
#define MAP_A_CHOSEN_CHANNEL_INFO				((OM_type) 3053)
#define MAP_A_CHOSEN_RADIO_RESOURCE_INFORMATION			((OM_type) 3054)
#define MAP_A_CHOSEN_SPEECH_VERSION				((OM_type) 3055)
#define MAP_A_CK						((OM_type) 3056)
#define MAP_A_CLIENT_IDENTITY					((OM_type) 3058)
#define MAP_A_CODEC1						((OM_type) 3059)
#define MAP_A_CODEC2						((OM_type) 3060)
#define MAP_A_CODEC3						((OM_type) 3061)
#define MAP_A_CODEC4						((OM_type) 3062)
#define MAP_A_CODEC5						((OM_type) 3063)
#define MAP_A_CODEC6						((OM_type) 3064)
#define MAP_A_CODEC7						((OM_type) 3065)
#define MAP_A_CODEC8						((OM_type) 3066)
#define MAP_A_COMPLETE_DATA_LIST_INCLUDED			((OM_type) 3067)
#define MAP_A_CONTEXT_ID					((OM_type) 3068)
#define MAP_A_CONTEXT_ID_LIST					((OM_type) 3069)
#define MAP_A_CS_ALLOCATION_RETENTION_PRIORITY			((OM_type) 3070)
#define MAP_A_CSG_ID						((OM_type) 3071)
#define MAP_A_CSG_SUBSCRIPTION_DATA				((OM_type) 3072)
#define MAP_A_CSG_SUBSCRIPTION_DATA_LIST			((OM_type) 3073)
#define MAP_A_CSG_SUBSCRIPTION_DELETED				((OM_type) 3074)
#define MAP_A_CSI_ACTIVE					((OM_type) 3075)
#define MAP_A_CS_LCS_NOT_SUPPORTED_BY_UE			((OM_type) 3076)
#define MAP_A_CUG_FEATURE_LIST					((OM_type) 3077)
#define MAP_A_CUG_INDEX						((OM_type) 3078)
#define MAP_A_CUG_INFO						((OM_type) 3079)
#define MAP_A_CUG_SUBSCRIPTION					((OM_type) 3081)
#define MAP_A_CUG_SUBSCRIPTOIN_LIST				((OM_type) 3082)
#define MAP_A_CURRENT_LOCATION					((OM_type) 3083)
#define MAP_A_CURRENT_LOCATION_RETRIEVED			((OM_type) 3084)
#define MAP_A_CURRENTLY_USED_CODEC				((OM_type) 3085)
#define MAP_A_CURRENT_SECURITY_CONTEXT				((OM_type) 3086)
#define MAP_A_DEFAULT_CALL_HANDLING				((OM_type) 3088)
#define MAP_A_DEFAULT_CONTEXT					((OM_type) 3089)
#define MAP_A_DEFAULT_SESSION_HANDLING				((OM_type) 3090)
#define MAP_A_DEFAULT_SMS_HANDLING				((OM_type) 3091)
#define MAP_A_DESTINATION_NUMBER				((OM_type) 3092)
#define MAP_A_DESTINATION_NUMBER_CRITERIA			((OM_type) 3093)
#define MAP_A_DESTINATION_NUMBER_LENGTH				((OM_type) 3094)
#define MAP_A_DESTINATION_NUMBER_LENGTH_LIST			((OM_type) 3095)
#define MAP_A_DESTINATION_NUMBER_LIST				((OM_type) 3096)
#define MAP_A_DIALED_NUMBER					((OM_type) 3097)
#define MAP_A_D_IM_CSI						((OM_type) 3098)
#define MAP_A_DP_ANALYSED_INFO_CRITERIA_LIST			((OM_type) 3099)
#define MAP_A_DP_ANALYSED_INFO_CRITERIUM			((OM_type) 3100)
#define MAP_A_EMLPP_INFO					((OM_type) 3101)
#define MAP_A_ENCRYPTION_ALGORITHM				((OM_type) 3102)
#define MAP_A_ENCRYPTION_INFO					((OM_type) 3103)
#define MAP_A_EPS_AUTHENTICATION_SET_LIST			((OM_type) 3104)
#define MAP_A_EPS_AV						((OM_type) 3105)
#define MAP_A_EPS_DATA_LIST					((OM_type) 3106)
#define MAP_A_EPS_INFO						((OM_type) 3107)
#define MAP_A_EPS_QOS_SUBSCRIBED				((OM_type) 3108)
#define MAP_A_EPS_SUBSCRIPTION_DATA				((OM_type) 3109)
#define MAP_A_EPS_SUBSCRIPTION_WITHDRAW				((OM_type) 3110)
#define MAP_A_EQUIPMENT_STATUS					((OM_type) 3111)
#define MAP_A_EVENT_MET						((OM_type) 3112)
#define MAP_A_EXPIRATION_DATE					((OM_type) 3113)
#define MAP_A_EXT2_QOS_SUBSCRIBED				((OM_type) 3114)
#define MAP_A_EXT3_QOS_SUBSCRIBED				((OM_type) 3115)
#define MAP_A_EXT_BASIC_SERVICE_CODE				((OM_type) 3116)
#define MAP_A_EXT_BEARER_SERVICE_CODE				((OM_type) 3117)
#define MAP_A_EXT_CALL_BARRING_FEATURE				((OM_type) 3118)
#define MAP_A_EXTERNAL_CLIENT					((OM_type) 3121)
#define MAP_A_EXTERNAL_CLIENT_LIST				((OM_type) 3122)
#define MAP_A_EXT_EXTERNAL_CLIENT_LIST				((OM_type) 3123)
#define MAP_A_EXT_FORW_FEATURE					((OM_type) 3124)
#define MAP_A_EXT_QOS_SUBSCRIBED				((OM_type) 3125)
#define MAP_A_EXT_SS_INFO					((OM_type) 3126)
#define MAP_A_EXT_TELESERVICE_CODE				((OM_type) 3127)
#define MAP_A_FAILURE_CAUSE					((OM_type) 3128)
#define MAP_A_FORWARDING_INFO_FOR_CSE				((OM_type) 3133)
#define MAP_A_FORWARDING_OPTION					((OM_type) 3134)
#define MAP_A_FREEZE_N_TMSI					((OM_type) 3135)
#define MAP_A_FREEZE_P_TMSI					((OM_type) 3136)
#define MAP_A_FREEZE_TMSI					((OM_type) 3137)
#define MAP_A_GEODETIC_INFORMATION				((OM_type) 3138)
#define MAP_A_GEOGRAPHICAL_INFORMATION				((OM_type) 3139)
#define MAP_A_GERAN_CLASSMARK					((OM_type) 3140)
#define MAP_A_GERAN_CODEC_LIST					((OM_type) 3141)
#define MAP_A_GGSN_ADDRESS					((OM_type) 3142)
#define MAP_A_GGSN_NUMBER					((OM_type) 3143)
#define MAP_A_GMLC						((OM_type) 3144)
#define MAP_A_GMLC_LIST						((OM_type) 3145)
#define MAP_A_GMLC_LIST_WITHDRAW				((OM_type) 3146)
#define MAP_A_GMLC_RESTRICTION					((OM_type) 3147)
#define MAP_A_GPRS_CAMEL_TDP_DATA				((OM_type) 3148)
#define MAP_A_GPRS_CAMEL_TDP_DATA_LIST				((OM_type) 3149)
#define MAP_A_GPRS_CSI						((OM_type) 3150)
#define MAP_A_GPRS_DATA_LIST					((OM_type) 3151)
#define MAP_A_GPRS_EHANCEMENT_SUPPORT_INDICATOR			((OM_type) 3152)
#define MAP_A_GPRS_MS_CLASS					((OM_type) 3153)
#define MAP_A_GPRS_SUBSCRIPTION_DATA				((OM_type) 3154)
#define MAP_A_GPRS_SUBSCRIPTION_DATA_WITHDRAW			((OM_type) 3155)
#define MAP_A_GPRS_TRIGGER_DETECTION_POINT			((OM_type) 3156)
#define MAP_A_GRPS_CSI						((OM_type) 3158)
#define MAP_A_GSMSCF_ADDRESS					((OM_type) 3159)
#define MAP_A_GSM_SECURITY_CONTEXT_DATA				((OM_type) 3160)
#define MAP_A_HANDOVER_NUMBER					((OM_type) 3161)
#define MAP_A_HLR_LIST						((OM_type) 3162)
#define MAP_A_HLR_NUMBER					((OM_type) 3163)
#define MAP_A_HO_NUMBER_NOT_REQUESTED				((OM_type) 3164)
#define MAP_A_HOP_COUNTER					((OM_type) 3165)
#define MAP_A_ICS_INDICATOR					((OM_type) 3166)
#define MAP_A_IDENTITY						((OM_type) 3167)
#define MAP_A_IK						((OM_type) 3168)
#define MAP_A_IMEISV						((OM_type) 3170)
#define MAP_A_IMMEDIATE_RESPONSE_PREFERRED			((OM_type) 3171)
#define MAP_A_IMSI_WITH_LMSI					((OM_type) 3173)
#define MAP_A_INFORM_PREVIOUS_NETWORK_ENTITY			((OM_type) 3174)
#define MAP_A_INTEGRITY_PROTECTION_ALGORITHM			((OM_type) 3175)
#define MAP_A_INTEGRITY_PROTECTION_INFO				((OM_type) 3176)
#define MAP_A_INTER_CUG_RESTRICTIONS				((OM_type) 3177)
#define MAP_A_INTRA_CUG_OPTIONS					((OM_type) 3178)
#define MAP_A_ISR_INFORMATION					((OM_type) 3179)
#define MAP_A_IU_AVAILABLE_CODEC_LIST				((OM_type) 3183)
#define MAP_A_IU_CURRENT_USED_CODEC				((OM_type) 3184)
#define MAP_A_IU_SELECTED_CODEC					((OM_type) 3185)
#define MAP_A_IU_SUPPORTED_CODECS_LIST				((OM_type) 3186)
#define MAP_A_KASME						((OM_type) 3187)
#define MAP_A_KEY_STATUS					((OM_type) 3189)
#define MAP_A_KSI						((OM_type) 3190)
#define MAP_A_LAC						((OM_type) 3191)
#define MAP_A_LCS_INFORMATION					((OM_type) 3194)
#define MAP_A_LCS_PRIVACY_CLASS					((OM_type) 3195)
#define MAP_A_LCS_PRIVACY_EXCEPTION_LIST			((OM_type) 3196)
#define MAP_A_LMU_INDICATOR					((OM_type) 3198)
#define MAP_A_LOCATION_AREA					((OM_type) 3199)
#define MAP_A_LOCATION_INFORMATION				((OM_type) 3200)
#define MAP_A_LOCATION_INFORMATION_GPRS				((OM_type) 3201)
#define MAP_A_LOCATION_NUMBER					((OM_type) 3202)
#define MAP_A_LONG_GROUP_ID					((OM_type) 3205)
#define MAP_A_LONG_GROUP_ID_SUPPORTED				((OM_type) 3206)
#define MAP_A_LSA_ACTIVE_MODE_INDICATOR				((OM_type) 3207)
#define MAP_A_LSA_ATTRIBUTES					((OM_type) 3208)
#define MAP_A_LSA_DATA						((OM_type) 3209)
#define MAP_A_LSA_DATA_LIST					((OM_type) 3210)
#define MAP_A_LSA_IDENTITY					((OM_type) 3211)
#define MAP_A_LSA_IDENTITY_LIST					((OM_type) 3212)
#define MAP_A_LSA_INFORMATION					((OM_type) 3213)
#define MAP_A_LSA_INFORMATION_WITHDRAW				((OM_type) 3214)
#define MAP_A_LSA_ONLY_ACCESS_INDICATOR				((OM_type) 3215)
#define MAP_A_MATCH_TYPE					((OM_type) 3216)
#define MAP_A_MAX_REQUESTED_BANDWIDTH_DL			((OM_type) 3217)
#define MAP_A_MAX_REQUESTED_BANDWIDTH_UL			((OM_type) 3218)
#define MAP_A_M_CSI						((OM_type) 3219)
#define MAP_A_MC_SS_INFO					((OM_type) 3220)
#define MAP_A_MG_CSI						((OM_type) 3221)
#define MAP_A_MM_CODE						((OM_type) 3222)
#define MAP_A_MNP_INFO_RES					((OM_type) 3223)
#define MAP_A_MNP_REQUESTED_INFO				((OM_type) 3224)
#define MAP_A_MOBILE_NOT_REACHABLE_REASON			((OM_type) 3225)
#define MAP_A_MOBILITY_TRIGGERS					((OM_type) 3226)
#define MAP_A_MODIFICATION_REQUEST_FOR_CB_INFO			((OM_type) 3227)
#define MAP_A_MODIFICATION_REQUEST_FOR_CF_INFO			((OM_type) 3228)
#define MAP_A_MODIFICATION_REQUEST_FOR_CSI			((OM_type) 3229)
#define MAP_A_MODIFICATION_REQUEST_FOR_IP_SM_GW_DATA		((OM_type) 3230)
#define MAP_A_MODIFICATION_REQUEST_FOR_ODB_DATA			((OM_type) 3231)
#define MAP_A_MODIFY_CSI_STATE					((OM_type) 3232)
#define MAP_A_MODIFY_NOTIFICATION_TO_CSE			((OM_type) 3233)
#define MAP_A_MODIFY_REGISTRATION_STATUS			((OM_type) 3234)
#define MAP_A_MOLR_CLASS					((OM_type) 3235)
#define MAP_A_MOLR_LIST						((OM_type) 3236)
#define MAP_A_MO_SMS_CSI					((OM_type) 3237)
#define MAP_A_MS_CLASSMARK					((OM_type) 3238)
#define MAP_A_MS_CLASSMARK_2					((OM_type) 3239)
#define MAP_A_MSISDN_BS						((OM_type) 3242)
#define MAP_A_MSISDN_BS_LIST					((OM_type) 3243)
#define MAP_A_MS_NETWORK_CAPABILITY				((OM_type) 3244)
#define MAP_A_MS_NOT_REACHABLE					((OM_type) 3245)
#define MAP_A_MS_RADIO_ACCESS_CAPABILITY			((OM_type) 3246)
#define MAP_A_MT_SMS_CAMEL_TDP_CRITERIA				((OM_type) 3247)
#define MAP_A_MT_SMS_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3248)
#define MAP_A_MT_SMS_CSI					((OM_type) 3249)
#define MAP_A_MT_SMS_TPDU_TYPE					((OM_type) 3250)
#define MAP_A_MULTICALL_BEARER_INFO				((OM_type) 3251)
#define MAP_A_MULTIPLE_BEARER_NOT_SUPPORTED			((OM_type) 3252)
#define MAP_A_MULTIPLE_BEARER_REQUESTED				((OM_type) 3253)
#define MAP_A_NET_DET_NOT_REACHABLE				((OM_type) 3255)
#define MAP_A_NETWORK_ACCESS_MODE				((OM_type) 3256)
#define MAP_A_NOTIFICATION_TO_CSE				((OM_type) 3258)
#define MAP_A_NOTIFICATION_TO_MS_USER				((OM_type) 3259)
#define MAP_A_NOT_PROVIDED_FROM_SGSN				((OM_type) 3260)
#define MAP_A_NOT_PROVIDED_FROM_VLR				((OM_type) 3261)
#define MAP_A_NSAPI						((OM_type) 3262)
#define MAP_A_NUMBER_OF_REQUESTED_ADDITIONAL_VECTORS		((OM_type) 3263)
#define MAP_A_NUMBER_OF_REQUESTED_VECTORS			((OM_type) 3264)
#define MAP_A_O_BCSM_CAMEL_TDP_CRITERIA				((OM_type) 3266)
#define MAP_A_O_BCSM_CAMEL_TDP_DATA				((OM_type) 3268)
#define MAP_A_O_BCSM_CAMEL_TDP_DATA_LIST			((OM_type) 3269)
#define MAP_A_O_BCSM_TRIGGER_DETECTION_POINT			((OM_type) 3270)
#define MAP_A_O_CAUSE_VALUE_CRITERIA				((OM_type) 3271)
#define MAP_A_ODB						((OM_type) 3273)
#define MAP_A_ODB_DATA						((OM_type) 3274)
#define MAP_A_ODB_GENERAL_DATA					((OM_type) 3275)
#define MAP_A_ODB_HPLMN_DATA					((OM_type) 3276)
#define MAP_A_ODB_INFO						((OM_type) 3277)
#define MAP_A_OFFERED_CAMEL4_CSIS_IN_SGSN			((OM_type) 3279)
#define MAP_A_OFFERED_CAMEL4_CSIS_IN_VLR			((OM_type) 3280)
#define MAP_A_OFFERED_CAMEL4_FUNCTIONALITIES			((OM_type) 3281)
#define MAP_A_OFFERED_CAMMEL4_CSIS				((OM_type) 3282)
#define MAP_A_O_IM_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3283)
#define MAP_A_O_IM_CSI						((OM_type) 3284)
#define MAP_A_PAGING_AREA_CAPABILITY				((OM_type) 3286)
#define MAP_A_PC_LCS_NOT_SUPPORTED_BY_UE			((OM_type) 3288)
#define MAP_A_PDN_GW_ALLOCATION_TYPE				((OM_type) 3289)
#define MAP_A_PDN_GW_IDENTITY					((OM_type) 3290)
#define MAP_A_PDN_GW_IPV4_ADDRESS				((OM_type) 3291)
#define MAP_A_PDN_GW_IPV6_ADDRESS				((OM_type) 3292)
#define MAP_A_PDN_GW_NAME					((OM_type) 3293)
#define MAP_A_PDN_GW_UPDATE					((OM_type) 3294)
#define MAP_A_PDP_ADDRESS					((OM_type) 3295)
#define MAP_A_PDP_CHARGING_CHARACTERISTICS			((OM_type) 3296)
#define MAP_A_PDP_CONTEXT					((OM_type) 3297)
#define MAP_A_PDP_CONTEXT_ACTIVE				((OM_type) 3298)
#define MAP_A_PDP_CONTEXT_ID					((OM_type) 3299)
#define MAP_A_PDP_CONTEXT_IDENTIFIER				((OM_type) 3300)
#define MAP_A_PDP_CONTEXT_INFO					((OM_type) 3301)
#define MAP_A_PDP_TYPE						((OM_type) 3302)
#define MAP_A_PLMN_CLIENT_LIST					((OM_type) 3303)
#define MAP_A_PREFERENTIAL_CUG_INDICATOR			((OM_type) 3304)
#define MAP_A_PREVIOUS_LAI					((OM_type) 3305)
#define MAP_A_PROVISIONED_SS					((OM_type) 3306)
#define MAP_A_PS_ATTACHED_NOT_REACHABLE_FOR_PAGING		((OM_type) 3307)
#define MAP_A_PS_ATTACHED_REACHABLE_FOR_PAGING			((OM_type) 3308)
#define MAP_A_PS_DETACHED					((OM_type) 3309)
#define MAP_A_PS_PDP_ACTIVE_NOT_REACHABLE_FOR_PAGING		((OM_type) 3310)
#define MAP_A_PS_PDP_ACTIVE_REACHABLE_FOR_PAGING		((OM_type) 3311)
#define MAP_A_PS_SUBSCRIBER_STATE				((OM_type) 3312)
#define MAP_A_QOS2_NEGOTIATED					((OM_type) 3313)
#define MAP_A_QOS2_REQUESTED					((OM_type) 3314)
#define MAP_A_QOS2_SUBSCRIBED					((OM_type) 3315)
#define MAP_A_QOS3_NEGOTIATED					((OM_type) 3316)
#define MAP_A_QOS3_REQUESTED					((OM_type) 3317)
#define MAP_A_QOS3_SUBSCRIBED					((OM_type) 3318)
#define MAP_A_QOS_CLASS_IDENTIFIER				((OM_type) 3319)
#define MAP_A_QOS_NEGOTIATED					((OM_type) 3320)
#define MAP_A_QOS_REQUESTED					((OM_type) 3321)
#define MAP_A_QOS_SUBSCRIBED					((OM_type) 3322)
#define MAP_A_QUADRUPLET_LIST					((OM_type) 3323)
#define MAP_A_RAB_CONFIGURATION_INDICATOR			((OM_type) 3324)
#define MAP_A_RAB_ID						((OM_type) 3325)
#define MAP_A_RADIO_RESOURCE					((OM_type) 3326)
#define MAP_A_RADIO_RESOURCE_INFORMATION			((OM_type) 3327)
#define MAP_A_RADIO_RESOURCE_LIST				((OM_type) 3328)
#define MAP_A_RANAP_SERVICE_HANDOVER				((OM_type) 3329)
#define MAP_A_RAND						((OM_type) 3330)
#define MAP_A_RE_ATTEMPT					((OM_type) 3331)
#define MAP_A_REGIONAL_SUBSCRIPTION_DATA			((OM_type) 3332)
#define MAP_A_REGIONAL_SUBSCRIPTION_IDENTIFIER			((OM_type) 3333)
#define MAP_A_REGIONAL_SUBSCRIPTION_RESPONSE			((OM_type) 3334)
#define MAP_A_RELOCATION_NUMBER					((OM_type) 3335)
#define MAP_A_RELOCATION_NUMBER_LIST				((OM_type) 3336)
#define MAP_A_REQUESTED_CAMEL_SUBSCRIPTION_INFO			((OM_type) 3337)
#define MAP_A_REQUESTED_DOMAIN					((OM_type) 3338)
#define MAP_A_REQUESTED_EQUIPMENT_INFO				((OM_type) 3339)
#define MAP_A_REQUESTED_NODE_TYPE				((OM_type) 3341)
#define MAP_A_REQUESTED_SS_INFO					((OM_type) 3342)
#define MAP_A_REQUESTED_SUBSCRIPTION_INFO			((OM_type) 3343)
#define MAP_A_REQUESTING_PLMN_ID				((OM_type) 3344)
#define MAP_A_RE_SYNCHRONISATION_INFO				((OM_type) 3345)
#define MAP_A_RFSP_ID						((OM_type) 3346)
#define MAP_A_RNC_ADDRESS					((OM_type) 3347)
#define MAP_A_ROAMING_RESTRICTED_IN_SGSN			((OM_type) 3348)
#define MAP_A_ROAMING_RESTRICTION				((OM_type) 3349)
#define MAP_A_ROUTEING_AREA_IDENTITY				((OM_type) 3350)
#define MAP_A_ROUTEING_NUMBER					((OM_type) 3351)
#define MAP_A_SEGMENTATION_PROHIBITED				((OM_type) 3353)
#define MAP_A_SELECTED_GSM_ALGORITHM				((OM_type) 3354)
#define MAP_A_SELECTED_LSA_ID					((OM_type) 3355)
#define MAP_A_SELECTED_LSA_IDENTITY				((OM_type) 3356)
#define MAP_A_SELECTED_RAB_ID					((OM_type) 3357)
#define MAP_A_SELECTED_UMTS_ALGORITHM				((OM_type) 3358)
#define MAP_A_SELECTED_UMTS_ALGORITHMS				((OM_type) 3359)
#define MAP_A_SEND_SUBSCRIBER_DATA				((OM_type) 3360)
#define MAP_A_SERVED_PARTY_IP_ADDRESS				((OM_type) 3361)
#define MAP_A_SERVICE_KEY					((OM_type) 3362)
#define MAP_A_SERVICE_TYPE					((OM_type) 3363)
#define MAP_A_SERVICE_TYPE_IDENTITY				((OM_type) 3364)
#define MAP_A_SERVICE_TYPE_LIST					((OM_type) 3365)
#define MAP_A_SERVING_NODE_TYPE_INDICATOR			((OM_type) 3366)
#define MAP_A_SGSN_ADDRESS					((OM_type) 3367)
#define MAP_A_SGSN_CAMEL_SUBSCRIPTION_INFO			((OM_type) 3368)
#define MAP_A_SGSN_CAPABILITY					((OM_type) 3369)
#define MAP_A_SGSN_MME_SEPARATION_SUPPORTED			((OM_type) 3370)
#define MAP_A_SKIP_SUBSCRIBER_DATA_UPDATE			((OM_type) 3372)
#define MAP_A_SMS_CALL_BARRING_SUPPORT_INDICATOR		((OM_type) 3373)
#define MAP_A_SMS_CAMEL_TDP_DATA				((OM_type) 3374)
#define MAP_A_SMS_CAMEL_TDP_DATA_LIST				((OM_type) 3375)
#define MAP_A_SMS_TRIGGER_DETECTION_POINT			((OM_type) 3376)
#define MAP_A_SOLA_SUPPORT_INDICATOR				((OM_type) 3377)
#define MAP_A_SOLSA_SUPPORT_INDICATOR				((OM_type) 3378)
#define MAP_A_SPECIFIC_CSI_DETECTED_LIST			((OM_type) 3379)
#define MAP_A_SPECIFIC_CSI_WITHDRAW				((OM_type) 3380)
#define MAP_A_SRES						((OM_type) 3381)
#define MAP_A_SS_CAMEL_DATA					((OM_type) 3382)
#define MAP_A_SS_CSI						((OM_type) 3384)
#define MAP_A_SS_EVENT_LIST					((OM_type) 3386)
#define MAP_A_SS_INFO_FOR_CSE					((OM_type) 3387)
#define MAP_A_STN_SR						((OM_type) 3391)
#define MAP_A_STN_SR_WITHDRAW					((OM_type) 3392)
#define MAP_A_SUBSCRIBER_DATA_STORED				((OM_type) 3393)
#define MAP_A_SUBSCRIBER_IDENTITY				((OM_type) 3394)
#define MAP_A_SUBSCRIBER_STATE					((OM_type) 3396)
#define MAP_A_SUBSCRIBER_STATUS					((OM_type) 3397)
#define MAP_A_SUPER_CHARGER_SUPPORTED_IN_HLR			((OM_type) 3398)
#define MAP_A_SUPER_CHARGER_SUPPORTED_IN_SERVING_NETWORK_ENTITY	((OM_type) 3399)
#define MAP_A_SUPPORTED_FEATURES				((OM_type) 3401)
#define MAP_A_SUPPORTED_RAT_TYPES_INDICATOR			((OM_type) 3403)
#define MAP_A_SUPPORTED_SGSN_CAMEL_PHASES			((OM_type) 3404)
#define MAP_A_SUPPORTED_VLR_CAMEL_PHASES			((OM_type) 3405)
#define MAP_A_TARGET_CELL_ID					((OM_type) 3406)
#define MAP_A_TARGET_MSC_NUMBER					((OM_type) 3407)
#define MAP_A_TARGET_RNC_ID					((OM_type) 3408)
#define MAP_A_T_BCSM_CAMEL_TDP_CRITERIA				((OM_type) 3409)
#define MAP_A_T_BCSM_CAMEL_TDP_DATA				((OM_type) 3411)
#define MAP_A_T_BCSM_CAMEL_TDP_DATA_LIST			((OM_type) 3412)
#define MAP_A_T_BCSM_TRIGGER_DETECTION_POINT			((OM_type) 3413)
#define MAP_A_T_CAUSE_VALUE_CRITERIA				((OM_type) 3414)
#define MAP_A_TEID_FOR_GN_AND_GP				((OM_type) 3416)
#define MAP_A_TEID_FOR_IU					((OM_type) 3417)
#define MAP_A_TELESERVICE_LIST					((OM_type) 3418)
#define MAP_A_TIF_CSI						((OM_type) 3419)
#define MAP_A_TIF_CSI_NOTIFICATION_TO_CSE			((OM_type) 3420)
#define MAP_A_TPDU_TYPE_CRITERION				((OM_type) 3421)
#define MAP_A_TRACE_PROPAGATION_LIST				((OM_type) 3422)
#define MAP_A_TRANSACTION_ID					((OM_type) 3423)
#define MAP_A_TRIPLET_LIST					((OM_type) 3424)
#define MAP_A_TYPE_OF_UPDATE					((OM_type) 3425)
#define MAP_A_UESBI_IU						((OM_type) 3426)
#define MAP_A_UESBI_IUA						((OM_type) 3427)
#define MAP_A_UESBI_IUB						((OM_type) 3428)
#define MAP_A_UMTS_SECURITY_CONTEXT_DATA			((OM_type) 3429)
#define MAP_A_USED_RAT_TYPE					((OM_type) 3430)
#define MAP_A_UTRAN_CODEC_LIST					((OM_type) 3431)
#define MAP_A_VBS_GROUP_INDICATION				((OM_type) 3432)
#define MAP_A_VBS_SUBSCRIPTION_DATA				((OM_type) 3433)
#define MAP_A_VGCS_GROUP_INDICATION				((OM_type) 3434)
#define MAP_A_VGCS_SUBSCRIPTION_DATA				((OM_type) 3435)
#define MAP_A_VLR_CAMEL_SUBSCRIPTION_DATA			((OM_type) 3437)
#define MAP_A_VLR_CAMEL_SUBSCRIPTION_INFO			((OM_type) 3438)
#define MAP_A_VLR_CAPABILITY					((OM_type) 3439)
#define MAP_A_VLR_NUMBER					((OM_type) 3440)
#define MAP_A_VOICE_BROADCAST_DATA				((OM_type) 3441)
#define MAP_A_VOICE_GROUP_CALL_DATA				((OM_type) 3442)
#define MAP_A_VPLMN_ADDRESS_ALLOWED				((OM_type) 3443)
#define MAP_A_VT_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3444)
#define MAP_A_VT_CSI						((OM_type) 3445)
#define MAP_A_VT_IM_BCSM_CAMEL_TDP_CRITERIA_LIST		((OM_type) 3446)
#define MAP_A_VT_IM_CSI						((OM_type) 3447)
#define MAP_A_WRONG_PASSWORD_ATTEMPTS_COUNTER			((OM_type) 3448)
#define MAP_A_XRES						((OM_type) 3449)
#define MAP_A_ZONE_CODE						((OM_type) 3450)

#if 0
/* Attributes used defined in <xmap_gsm.h> */
#define MAP_A_ADDITIONAL_INFO					((OM_type) 3005)
#define MAP_A_ADDITIONAL_SUBSCRIPTIONS				((OM_type) 3007)
#define MAP_A_AGE_OF_LOCATION_INFORMATION			((OM_type) 3000)
#define MAP_A_ALL_INFORMATION_SENT				((OM_type) 3003)
#define MAP_A_AN_APDU						((OM_type) 3009)
#define MAP_A_ASCI_CALL_REFERENCE				((OM_type) 3018)
#define MAP_A_BASIC_SERVICE_GROUP_LIST				((OM_type) 3027)
#define MAP_A_BASIC_SERVICE					((OM_type) 3025)
#define MAP_A_CALL_BARRING_FEATURE_LIST				((OM_type) 3036)
#define MAP_A_CALL_BARRING_INFO					((OM_type) 3037)
#define MAP_A_CALL_PRIORITY					((OM_type) 3040)
#define MAP_A_CKSN						((OM_type) 3057)
#define MAP_A_CUG_INTERLOCK					((OM_type) 3080)
#define MAP_A_D_CSI						((OM_type) 3087)
#define MAP_A_EXTENSION_CONTAINER				((OM_type) 3120)
#define MAP_A_EXTENSION						((OM_type) 3119)
#define MAP_A_FORWARDED_TO_NUMBER				((OM_type) 3129)
#define MAP_A_FORWARDED_TO_SUBADDRESS				((OM_type) 3130)
#define MAP_A_FORWARDING_FEATURE_LIST				((OM_type) 3131)
#define MAP_A_FORWARDING_INFO					((OM_type) 3132)
#define MAP_A_GROUP_ID						((OM_type) 3157)
#define MAP_A_IMEI						((OM_type) 3169)
#define MAP_A_IMSI						((OM_type) 3172)
#define MAP_A_IST_ALERT_TIMER					((OM_type) 3180)
#define MAP_A_IST_INFORMATION_WITHDRAW				((OM_type) 3181)
#define MAP_A_IST_SUPPORT_INDICATOR				((OM_type) 3182)
#define MAP_A_KC						((OM_type) 3188)
#define MAP_A_LAI_FIXED_LENGTH					((OM_type) 3192)
#define MAP_A_LCS_CLIENT_INTERNAL_ID				((OM_type) 3193)
#define MAP_A_LMSI						((OM_type) 3197)
#define MAP_A_LONG_FORWARDED_TO_NUMBER				((OM_type) 3203)
#define MAP_A_LONG_FTN_SUPPORTED				((OM_type) 3204)
#define MAP_A_MSC_NUMBER					((OM_type) 3240)
#define MAP_A_MSISDN						((OM_type) 3241)
#define MAP_A_NAEA_PREFERRED_CI					((OM_type) 3254)
#define MAP_A_NO_REPLY_CONDITION_TIME				((OM_type) 3257)
#define MAP_A_NUMBER_PORTABILITY_STATUS				((OM_type) 3265)
#define MAP_A_O_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3267)
#define MAP_A_O_CSI						((OM_type) 3272)
#define MAP_A_OFFERED_CAMEL4_CSIS				((OM_type) 3278)
#define MAP_A_PAGING_AREA					((OM_type) 3285)
#define MAP_A_PASSWORD						((OM_type) 3287)
#define MAP_A_REQUESTED_INFO					((OM_type) 3340)
#define MAP_A_SAI_PRESENT					((OM_type) 3352)
#define MAP_A_SGSN_NUMBER					((OM_type) 3371)
#define MAP_A_SS_CODE						((OM_type) 3383)
#define MAP_A_SS_DATA						((OM_type) 3385)
#define MAP_A_SS_LIST						((OM_type) 3388)
#define MAP_A_SS_STATUS						((OM_type) 3389)
#define MAP_A_SS_SUBSCRIPTION_OPTION				((OM_type) 3390)
#define MAP_A_SUBSCRIBER_INFO					((OM_type) 3395)
#define MAP_A_SUPPORTED_CAMEL_PHASES				((OM_type) 3400)
#define MAP_A_SUPPORTED_LCS_CAPABILITY_SETS			((OM_type) 3402)
#define MAP_A_T_BCSM_CAMEL_TDP_CRITERIA_LIST			((OM_type) 3410)
#define MAP_A_T_CSI						((OM_type) 3415)
#define MAP_A_V_GMLC_ADDRESS					((OM_type) 3436)
#endif

/* OM enumeration types (prefixed MAP_T_) */

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

/* MAP_T_PDN_GW_Allocation_Type: */
typedef enum {
	MAP_T_STATIC = 0,
	MAP_T_DYNAMIC = 1,
} MAP_T_PDN_GW_Allocation_Type;

/* MAP_T_Failure_Cause: */
typedef enum {
	MAP_T_WRONG_USER_RESPONSE = 0,
	MAP_T_WRONG_NETWORK_SIGNATURE = 1,
} MAP_T_Failure_Cause;

/* MAP_T_Access_Type: */
typedef enum {
	MAP_T_CALL = 0,
	MAP_T_EMERGENCY_CALL = 1,
	MAP_T_LOCATION_UPDATING = 2,
	MAP_T_SUPPLEMENTARY_SERVICE = 3,
	MAP_T_SHORT_MESSAGE = 4,
	MAP_T_GPRS_ATTACH = 5,
	MAP_T_ROUTING_AREA_UPDATING = 6,
	MAP_T_SERVICE_REQUEST = 7,
	MAP_T_PDP_CONTEXT_ACTIVATION = 8,
	MAP_T_PDP_CONTEXT_DEACTIVATION = 9,
	MAP_T_GPRS_DETACH = 10,
} MAP_T_Access_Type;

/* MAP_T_Specific_CSI_Deleted_List: */
typedef enum {
	MAP_T_O_CSI = 0,
	MAP_T_SS_CSI = 1,
	MAP_T_TIF_CSI = 2,
	MAP_T_D_CSI = 3,
	MAP_T_VT_CSI = 4,
	MAP_T_MO_SMS_CSI = 5,
	MAP_T_M_CSI = 6,
	MAP_T_GPRS_CSI = 7,
	MAP_T_T_CSI = 8,
	MAP_T_MT_SMS_CSI = 9,
	MAP_T_MG_CSI = 10,
	MAP_T_O_IM_CSI = 11,
	MAP_T_D_IM_CSI = 12,
	MAP_T_VT_IM_CSI = 13,
} MAP_T_Specific_CSI_Deleted_List;

/* MAP_T_Cancellation_Type: */
typedef enum {
	MAP_T_UPDATE_PROCEDURE = 0,
	MAP_T_SUBSCRIPTION_WITHDRAW = 1,
} MAP_T_Cancellation_Type;

/* MAP_T_Type_Of_Update: */
typedef enum {
	MAP_T_SGSN_CHANGE = 0,
	MAP_T_MME_CHANGE = 1,
} MAP_T_Type_Of_Update;

/* MAP_T_Requested_Equipment_Info: (bits) */
typedef enum {
	MAP_T_EQUIPMENT_STATUS = 0,
	MAP_T_BMUEF = 1,
} MAP_T_Requested_Equipment_Info;

/* MAP_T_Equipment_Status: */
typedef enum {
	MAP_T_WHITE_LISTED = 0,
	MAP_T_BLACK_LISTED = 1,
	MAP_T_GREY_LISTED = 2,
} MAP_T_Equipment_Status;

/* MAP_T_Intra_CUG_Options: */
typedef enum {
	MAP_T_NO_CUG_RESTRICTIONS = 0,
	MAP_T_CUG_IC_CALL_BARRED = 1,
	MAP_T_CUG_OG_CALL_BARRED = 2,
} MAP_T_Intra_CUG_Options;

/* MAP_T_Specific_CSI_Withdraw: (bits) */
typedef enum {
	MAP_T_O_CSI = 0,
	MAP_T_SS_CSI = 1,
	MAP_T_TIF_CSI = 2,
	MAP_T_D_CSI = 3,
	MAP_T_VT_CSI = 4,
	MAP_T_MO_SMS_CSI = 5,
	MAP_T_M_CSI = 6,
	MAP_T_GPRS_CSI = 7,
	MAP_T_T_CSI = 8,
	MAP_T_MT_SMS_CSI = 9,
	MAP_T_MG_CSI = 10,
	MAP_T_O_IM_CSI = 11,
	MAP_T_D_IM_CSI = 12,
	MAP_T_VT_IM_CSI = 13,
} MAP_T_Specific_CSI_Withdraw;

/* MAP_T_Regional_Subscription_Response: */
typedef enum {
	MAP_T_NETWORK_NODE_AREA_RESTRICTED = 0,
	MAP_T_TOO_MANY_ZONE_CODES = 1,
	MAP_T_ZONE_CODES_CONFLICT = 2,
	MAP_T_REGIONAL_SUBSC_NOT_SUPPORTED = 3,
} MAP_T_Regional_Subscription_Response;

/* MAP_T_Match_Type: */
typedef enum {
	MAP_T_INHIBITING = 0,
	MAP_T_ENABLING = 1,
} MAP_T_Match_Type;

/* MAP_T_Default_Call_Handling: */
typedef enum {
	MAP_T_CONTINUE_CALL = 0,
	MAP_T_RELEASE_CALL = 1,
} MAP_T_Default_Call_Handling;

/* MAP_T_IST_Information: (bits) */
typedef enum {
	MAP_T_UPDATE_MME = 0,
	MAP_T_CANCEL_SGSN = 1,
} MAP_T_IST_Information;

/* MAP_T_GMLC_Restriction: */
typedef enum {
	MAP_T_GMLC_LIST = 0,
	MAP_T_HOME_COUNTRY = 1,
} MAP_T_GMLC_Restriction;

/* MAP_T_Notifiction_To_MS_User: */
typedef enum {
	MAP_T_NOTIFY_LOCATION_ALLOWED = 0,
	MAP_T_NOTIFY_AND_VERIFY_LOCATION_ALLOWED_NO_REPONSE = 1,
	MAP_T_NOTIFY_AND_VERIFY_LOCATION_ALLOWED_IF_NO_RESPONSE = 2,
	MAP_T_LOCATION_NOT_ALLOWED = 3,
} MAP_T_Notifiction_To_MS_User;

/* MAP_T_Key_Status: */
typedef enum {
	MAP_T_OLD = 0,
	MAP_T_NEW = 1,
} MAP_T_Key_Status;

/* MAP_T_GPRS_Trigger_Detection_Point: */
typedef enum {
	MAP_T_ATTACH = 1,
	MAP_T_ATTACH_CHANGE_OF_POSITION = 2,
	MAP_T_PDP_CONTEXT_ESTABLISHMENT = 11,
	MAP_T_PDP_CONTEXT_ESTABLISHMENT_ACKNOWLEDGEMENT = 12,
	MAP_T_PDP_CONTEXT_CHANGE_OF_POSITION = 14,
} MAP_T_GPRS_Trigger_Detection_Point;

/* MAP_T_Default_Session_Handling: */
typedef enum {
	MAP_T_CONTINUE_TRANSACTION = 0,
	MAP_T_RELEASE_TRANSACTION = 1,
} MAP_T_Default_Session_Handling;

/* MAP_T_Subscriber_Status: */
typedef enum {
	MAP_T_SERVICE_GRANTED = 0,
	MAP_T_OPERATOR_DETERMINED_BARRING = 1,
} MAP_T_Subscriber_Status;

/* MAP_T_Network_Acess_Mode: */
typedef enum {
	MAP_T_PACKET_AND_CIRCUIT = 0,
	MAP_T_ONLY_CIRCUIT = 1,
	MAP_T_ONLY_PACKET = 2,
} MAP_T_Network_Acess_Mode;

/* MAP_T_Access_Restriction_Data: (bits) */
typedef enum {
	MAP_T_UTRAN_NOT_ALLOWED = 0,
	MAP_T_GERAN_NOT_ALLOWED = 1,
	MAP_T_GAN_NOT_ALLOWED = 2,
	MAP_T_I_HSPA_EVOLUTION_NOT_ALLOWED = 3,
	MAP_T_E_UTRAN_NOT_ALLOWED = 4,
	MAP_T_HO_TO_NON_3GPP_ACCESS_NOT_ALLOWED = 5,
} MAP_T_Access_Restriction_Data;

/* MAP_T_ODB_General_Data: (bits) */
typedef enum {
	MAP_T_ALL_OG_CALLS_BARRED = 0,
	MAP_T_INTERNATIONAL_OG_CALLS_BARRED = 1,
	MAP_T_INTERNATIONAL_OG_CALLS_NOT_TO_HPLMN_COUNTRY_BARRED = 2,
	MAP_T_INTERZONAL_OG_CALLS_BARRED = 6,
	MAP_T_INTERZONAL_OG_CALLS_NO_TO_HPLMN_COUNTRY_BARRED = 7,
	MAP_T_INTERZONAL_OG_CALLS_AND_INTERNATIONAL_OG_CALLS_NOT_TO_HPLMN_COUNTRY_BARRED = 8,
	MAP_T_PREMIUM_RATE_INFORMATION_OG_CALLS_BARRED = 3,
	MAP_T_PREMIUM_RATE_ENTERTAINMENT_OG_CALLS_BARRED = 4,
	MAP_T_SS_ACCESS_BARRED = 5,
	MAP_T_ALL_ECT_BARRED = 9,
	MAP_T_CHARGEABLE_ECT_BARRED = 10,
	MAP_T_INTERNATIONAL_ECT_BARRED = 11,
	MAP_T_INTERZONAL_ECT_BARRED = 12,
	MAP_T_DOUBLY_CHARGEABLE_ECT_BARRED = 13,
	MAP_T_MULTIPLE_ECT_BARRED = 14,
	MAP_T_ALL_PACKET_ORIENTED_SERVICE_BARRED = 15,
	MAP_T_ROAMER_ACCESS_TO_HPLMN_AP_BARRED = 16,
	MAP_T_ROAMER_ACCESS_TO_VPLMN_AP_BARRED = 17,
	MAP_T_ROAMING_OUTSIDE_PLMN_OG_CALLS_BARRED = 18,
	MAP_T_ALL_IC_CALLS_BARRED = 19,
	MAP_T_ROAMING_OUTSIDE_PLMN_IC_CALLS_BARRED = 20,
	MAP_T_ROAMING_OUTSIDE_PLMN_I_COUNTRY_IC_CALLS_BARRED = 21,
	MAP_T_ROAMING_OUTSIDE_PLMN_BARRED = 22,
	MAP_T_ROAMING_OUTSIDE_PLMN_COUNTRY_BARRED = 23,
	MAP_T_REGISTRATION_ALL_CF_BARRED = 24,
	MAP_T_REGISTRATION_CF_NOT_TO_HPLMN_BARRED = 25,
	MAP_T_REGISTRATION_INTERZONAL_CF_BARRED = 26,
	MAP_T_REGISTRATION_INTERZONAL_CF_NOT_TO_HPLMN_BARRED = 27,
	MAP_T_REGISTRATION_INTERATIONAL_CF_BARRED = 28,
} MAP_T_ODB_General_Data;

/* MAP_T_Supported_Features: (bits) */
typedef enum {
	MAP_T_ODB_ALL = 0,
	MAP_T_ODB_HPLMN_APN = 1,
	MAP_T_ODB_VPLMN_APN = 2,
	MAP_T_REG_SUB = 3,
} MAP_T_Supported_Features;

/* MAP_T_LSA_Only_Access_Indicator: */
typedef enum {
	MAP_T_ACCESS_OUTSIDE_LSAS_ALLOWED = 0,
	MAP_T_ACCESS_OUTSIDE_LSAS_RESTRICTED = 1,
} MAP_T_LSA_Only_Access_Indicator;

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

/* MAP_T_Modification_Instruction: */
typedef enum {
	MAP_T_DEACTIVATE = 0,
	MAP_T_ACTIVATE = 1,
} MAP_T_Modification_Instruction;

/* MAP_T_Requested_CAMEL_Subscription_Info: */
typedef enum {
	MAP_T_O_CSI = 0,
	MAP_T_T_CSI = 1,
	MAP_T_VT_CSI = 2,
	MAP_T_TIF_CSI = 3,
	MAP_T_GPRS_CSI = 4,
	MAP_T_MO_SMS_CSI = 5,
	MAP_T_SS_CSI = 6,
	MAP_T_M_CSI = 7,
	MAP_T_D_CSI = 8,
} MAP_T_Requested_CAMEL_Subscription_Info;

/* MAP_T_Additional_Requested_CAMEL_Subscription_Info: */
typedef enum {
	MAP_T_MT_SMS_CSI = 0,
	MAP_T_MG_CSI = 1,
	MAP_T_O_IM_CSI = 2,
	MAP_T_D_IM_CSI = 3,
	MAP_T_VT_IM_CSI = 4,
} MAP_T_Additional_Requested_CAMEL_Subscription_Info;

/* MAP_T_SMS_Trigger_Detection_Point: */
typedef enum {
	MAP_T_SMS_COLLECTED_INFO = 1,
	MAP_T_SMS_DELIVERY_REQUEST = 2,
} MAP_T_SMS_Trigger_Detection_Point;

/* MAP_T_O_Bcsm_Trigger_Detection_Point: */
typedef enum {
	MAP_T_COLLECTED_INFO = 2,
	MAP_T_ROUTE_SELECT_FAILURE = 4,
} MAP_T_O_Bcsm_Trigger_Detection_Point;

/* MAP_T_Call_Type_Criteria: */
typedef enum {
	MAP_T_FORWARDED = 0,
	MAP_T_NOT_FORWARDED = 1,
} MAP_T_Call_Type_Criteria;

/* MAP_T_ODB_HPLMN_Data: (bits) */
typedef enum {
	MAP_T_PLMN_SPECIFIC_BARRING_TYPE1 = 0,
	MAP_T_PLMN_SPECIFIC_BARRING_TYPE2 = 1,
	MAP_T_PLMN_SPECIFIC_BARRING_TYPE3 = 2,
	MAP_T_PLMN_SPECIFIC_BARRING_TYPE4 = 3,
} MAP_T_ODB_HPLMN_Data;

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

/* MAP_T_Not_Reachable_Reason: */
typedef enum {
	MAP_T_MS_PURGED = 0,
	MAP_T_IMSI_DETACHED = 1,
	MAP_T_RESTRICTED_AREA = 2,
	MAP_T_NOT_REGISTERED = 3,
} MAP_T_Not_Reachable_Reason;

/* MAP_T_Domain_Type: */
typedef enum {
	MAP_T_CS_DOMAIN = 0,
	MAP_T_PS_DOMAIN = 1,
} MAP_T_Domain_Type;

/* MAP_T_Requesting_Node_Type: */
typedef enum {
	MAP_T_VLR = 0,
	MAP_T_SGSN = 1,
	MAP_T_S_CSCF = 2,
	MAP_T_BSF = 3,
	MAP_T_GAN_AAA_SERVER = 4,
	MAP_T_WLAN_AAA_SERVER = 5,
	MAP_T_MME = 16,
	MAP_T_MME_SGSN = 17,
} MAP_T_Requesting_Node_Type;

/* MAP_T_Supported_RAT_Types: (bits) */
typedef enum {
	MAP_T_UTRAN = 0,
	MAP_T_GERAN = 1,
	MAP_T_GAN = 2,
	MAP_T_I_HSPA_EVOLUTION = 3,
	MAP_T_E_UTRAN = 4,
} MAP_T_Supported_RAT_Types;

/* MAP_T_Default_SMS_Handling: */
typedef enum {
	MAP_T_CONTINUE_TRANSACTION = 0,
	MAP_T_RELEASE_TRANSACTION = 1,
} MAP_T_Default_SMS_Handling;

/* MAP_T_T_BCSM_Trigger_Detection_Point: */
typedef enum {
	MAP_T_TERM_ATTEMPT_AUTHORIZED = 12,
	MAP_T_T_BUSY = 13,
	MAP_T_T_NO_ANSWER = 14,
} MAP_T_T_BCSM_Trigger_Detection_Point;

/* MAP_T_MT_SMS_TPDU_Type: */
typedef enum {
	MAP_T_SMS_DELIVER = 0,
	MAP_T_SMS_SUBMIT_REPORT = 1,
	MAP_T_SMS_STATUS_REPORT = 2,
} MAP_T_MT_SMS_TPDU_Type;

/* MAP_T_Used_RAT_Type: */
typedef enum {
	MAP_T_UTRAN = 0,
	MAP_T_GERAN = 1,
	MAP_T_GSN = 2,
	MAP_T_I_HSPA_EVOLUTION = 3,
	MAP_T_E_UTRAN = 4,
} MAP_T_Used_RAT_Type;

#if 0
/* MAP_T_IST_Support_Indicator: */
typedef enum {
	MAP_T_BASIC_IST_SUPPORT = 0,
	MAP_T_IST_COMMAND_SUPPORTED = 1,
} MAP_T_IST_Support_Indicator;

/* MAP_T_Additional_Subscriptions: (bits) */
typedef enum {
	MAP_T_PRIVILEGED_UP_LINK_REQUEST = 0,
	MAP_T_EMERGENCY_UP_LINK_REQUEST = 1,
	MAP_T_EMERGENCY_RESET = 2,
} MAP_T_Additional_Subscriptions;
#endif

/* Operation codes (MAP_T_Operation_Code): */
/* location registration operations */
#define MAP_T_UPDATE_LOCATION				 2
#define MAP_T_CANCEL_LOCATION				 3
#define MAP_T_PURGE_MS					67
#define MAP_T_SEND_IDENTIFICATION			55
/* gprs location registration operations */
#define MAP_T_UPDATE_GPRS_LOCATION			23
/* subscriber information enquiry operations */
#define MAP_T_PROVIDE_SUBSCRIBER_INFO			70
/* any time information enquiry operations */
#define MAP_T_ANY_TIME_INTERROGATION			71
/* any time information handling operations */
#define MAP_T_ANY_TIME_SUBSCRIPTION_INTERROGATION	62
#define MAP_T_ANY_TIME_MODIFICATION			65
/* subscriber data modification notification operations */
#define MAP_T_NOTE_SUBSCRIBER_DATA_MODIFIED		 5
/* handover operations */
#define MAP_T_PREPARE_HANDOVER				68
#define MAP_T_SEND_END_SIGNAL				29
#define MAP_T_PROCESS_ACCESS_SIGNALLING			33
#define MAP_T_FORWARD_ACCESS_SIGNALLING			34
#define MAP_T_PREPARE_SUBSEQUENT_HANDOVER		69
/* authentication management operations */
#define MAP_T_SEND_AUTHENTICATION_INFO			56
#define MAP_T_AUTHENTICATION_FAILURE_REPORT		15
/* IMEI management operations */
#define MAP_T_CHECK_IMEI				43
/* subscriber management operations */
#define MAP_T_INSERT_SUBSCRIBER_DATA			 7
#define MAP_T_DELETE_SUBSCRIBER_DATA			 8
/* fault recovery operations */
#define MAP_T_RESET					37
#define MAP_T_FORWARD_CHECK_SS_INDICATION		38
#define MAP_T_RESTORE_DATA				57
/* GPRS location information retrieval operations */
#define MAP_T_SEND_ROUTING_INFO_FOR_GPRS		24
/* failure reporting operations */
#define MAP_T_FAILURE_REPORT				25
/* gprs notification operations */
#define MAP_T_NOTE_MS_PRESENT_FOR_GPRS			26
/* mobility management operations */
#define MAP_T_NOTE_MM_EVENT				89

#if 0
/*
 * SERVICE ERRORS
 */

/* Service-Error problem values (MAP_T_User_Error): */
/* identification and numbering errors */
#define MAP_E_UNKNOWN_SUBSCRIBER			 1
#define MAP_E_NUMBER_CHANGED				44
#define MAP_E_UNKNOWN_MSC				 3
#define MAP_E_UNIDENTIFIED_SUBSCRIBER			 5
#define MAP_E_UNKNOWN_EQUIPMENT				 7
/* subscription errors */
#define MAP_E_ROAMING_NOT_ALLOWED			 8
#define MAP_E_ILLEGAL_SUBSCRIBER			 9
#define MAP_E_ILLEGAL_EQUIPMENT				12
#define MAP_E_BEARER_SERVICE_NOT_PROVISIONED		10
#define MAP_E_TELESERVICE_NOT_PROVISIONED		11
/* handover errors */
#define MAP_E_NO_HANDOVER_NUMBER_AVAILABLE		25
#define MAP_E_SUBSEQUENT_HANDOVER_FAILURE		26
#define MAP_E_TARGET_CELL_OUTSIDE_GROUP_CALL_AREA	42
/* any time interrogation errors */
#define MAP_E_ATI_NOT_ALLOWED				49
/* any time information handling errors */
#define MAP_E_ATM_NOT_ALLOWED				61
#define MAP_E_ATSI_NOT_ALLOWED				60
#define MAP_E_INFORMATION_NOT_AVAILABLE			62
/* mobility management errors */
#define MAP_E_MM_EVENT_NOT_SUPPORTED			59

/* Service-Error parameter classes: */
/* identification and numbering errors */
#define OMP_O_MAP_C_NUMBER_CHANGE_PARAM			mapP_gsm_ms(\x8F\x51)	/* 2001 */
#define OMP_O_MAP_C_UNIDENTIFIED_SUB_PARAM		mapP_gsm_ms(\x8F\x52)	/* 2002 */
#define OMP_O_MAP_C_UNKNOWN_SUBSCRIBER_PARAM		mapP_gsm_ms(\x8F\x53)	/* 2003 */
/* subscription errors */
#define OMP_O_MAP_C_BEARER_SERV_NOT_PROV_PARAM		mapP_gsm_ms(\x8F\x54)	/* 2004 */
#define OMP_O_MAP_C_ILLEGAL_EQUIPMENT_PARAM		mapP_gsm_ms(\x8F\x55)	/* 2005 */
#define OMP_O_MAP_C_ILLEGAL_SUBSCRIBER_PARAM		mapP_gsm_ms(\x8F\x56)	/* 2006 */
#define OMP_O_MAP_C_ROAMING_NOT_ALLOWED_PARAM		mapP_gsm_ms(\x8F\x57)	/* 2007 */
#define OMP_O_MAP_C_TELESERV_NOT_PROV_PARAM		mapP_gsm_ms(\x8F\x58)	/* 2008 */
/* handover errors */
#define OMP_O_MAP_C_TARGET_CELL_OUTSIDE_GCA_PARAM	mapP_gsm_ms(\x8F\x59)	/* 2009 */
/* any time interrogation errors */
#define OMP_O_MAP_C_ATI_NOT_ALLOWED_PARAM		mapP_gsm_ms(\x8F\x5A)	/* 2010 */
/* any time information handling errors */
#define OMP_O_MAP_C_ATM_NOT_ALLOWED_PARAM		mapP_gsm_ms(\x8F\x5B)	/* 2011 */
#define OMP_O_MAP_C_ATSI_NOT_ALLOWED_PARAM		mapP_gsm_ms(\x8F\x5C)	/* 2012 */
#define OMP_O_MAP_C_INFORMATION_NOT_AVAILABLE_PARAM	mapP_gsm_ms(\x8F\x5D)	/* 2013 */
/* mobility management errors */
#define OMP_O_MAP_C_MM_EVENT_NOT_SUPPORTED_PARAM	mapP_gsm_ms(\x8F\x5E)	/* 2014 */

/* Service-Error attributes: */
/* identification and numbering errors */
#define MAP_A_UNKNOWN_SUBSCRIBER_DIAGNOSTIC		((OM_type)3101)
/* subscription errors */
#define MAP_A_ROAMING_NOT_ALLOWED_CAUSE			((OM_type)3102)
/* handover errors */
/* any time interrogation errors */
/* any time information handling errors */
/* mobility management errors */

/* Service-Error enumerations: */

/* identification and numbering errors */

/* MAP_T_Unknown_Subscriber_Diagnostic: */
typedef enum {
	MAP_T_IMSI_UNKNOWN = 0,
	MAP_T_GPRS_SUBSCRIPTION_UNKNOWN = 1,
	MAP_T_NPDB_MISMATCH = 2,
} MAP_T_Unknown_Subscriber_Diagnostic;

/* subscription errors */

/* MAP_T_Roaming_Not_Allowed_Cause: */
typedef enum {
	MAP_T_PLMN_ROAMING_NOT_ALLOWED = 0,
	MAP_T_OPERATOR_DETERMINED_BARRING = 3,
} MAP_T_Roaming_Not_Allowed_Cause;

/* handover errors */
/* any time interrogation errors */
/* any time information handling errors */
/* mobility management errors */
#endif

/* Uses:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_ACCESS_NETWORK_SIGNAL_INFO <xmap_gsm.h>
 * OMP_O_MAP_C_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_OR_LAI <xmap_gsm.h>
 * OMP_O_MAP_C_EMLPP_INFO <xmap_gsm.h>
 * OMP_O_MAP_C_EXT_BASIC_SERVICE_CODE <xmap_gsm.h>
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 * OMP_O_MAP_C_HLR_LIST <xmap_gsm.h>
 * OMP_O_MAP_C_LCS_CLIENT_EXTERNAL_ID <xmap_gsm.h>
 * OMP_O_MAP_C_MC_SS_INFO <xmap_gsm.h>
 * OMP_O_MAP_C_NAEA_PREFERRED_CI <xmap_gsm.h>
 * OMP_O_MAP_C_IDENTITY <xmap_gsm.h>
 * OMP_O_MAP_C_IMSI_WITH_LMSI <xmap_gsm.h>
 * OMP_O_MAP_C_SUBSCRIBER_IDENTITY <xmap_gsm.h>
 *
 * OMP_O_MAP_C_TRACE_PROPAGATION_LIST <xmap_gsm_om.h>
 *
 * OMP_O_MAP_C_BASIC_SERVICE_GROUP_LIST <xmap_gsm_ss.h>
 * OMP_O_MAP_C_SS_FOR_BS_CODE <xmap_gsm_ss.h>
 * OMP_O_MAP_C_SS_LIST <xmap_gsm_ss.h>
 * OMP_O_MAP_C_SS_SUBSCRIPTION_OPTION <xmap_gsm_ss.h>
 *
 * OMP_O_MAP_C_ADD_INFO (this file)
 * OMP_O_MAP_C_ALLOWED_UMTS_ALGORITHMS (this file)
 * OMP_O_MAP_C_AMBR (this file)
 * OMP_O_MAP_C_APN_CONFIGURATION (this file)
 * OMP_O_MAP_C_APN_CONFIGURATION_PROFILE (this file)
 * OMP_O_MAP_C_AUTHENTICATION_QUINTUPLET (this file)
 * OMP_O_MAP_C_AUTHENTICATION_SET_LIST (this file)
 * OMP_O_MAP_C_AUTHENTICATION_TRIPLET (this file)
 * OMP_O_MAP_C_BASIC_SERVICE_CRITERIA (this file)
 * OMP_O_MAP_C_BASIC_SERVICE_LIST (this file)
 * OMP_O_MAP_C_BEARER_SERVICE_LIST (this file)
 * OMP_O_MAP_C_BSSMAP_SERVICE_HANDOVER_INFO (this file)
 * OMP_O_MAP_C_BSSMAP_SERVICE_HANDOVER_LIST (this file)
 * OMP_O_MAP_C_CALL_BARRING_DATA (this file)
 * OMP_O_MAP_C_CALL_FORWARDING_DATA (this file)
 * OMP_O_MAP_C_CAMEL_SUBSCRIPTION_INFO (this file)
 * OMP_O_MAP_C_CODEC_LIST (this file)
 * OMP_O_MAP_C_CONTEXT_ID_LIST (this file)
 * OMP_O_MAP_C_CSG_SUBSCRIPTION_DATA (this file)
 * OMP_O_MAP_C_CSG_SUBSCRIPTION_DATA_LIST (this file)
 * OMP_O_MAP_C_CUG_FEATURE_LIST (this file)
 * OMP_O_MAP_C_CUG_INFO (this file)
 * OMP_O_MAP_C_CUG_SUBSCRIPTION (this file)
 * OMP_O_MAP_C_CUG_SUBSCRIPTION_LIST (this file)
 * OMP_O_MAP_C_CURRENT_SECURITY_CONTEXT (this file)
 * OMP_O_MAP_C_D_CSI (this file)
 * OMP_O_MAP_C_DESTINATION_NUMBER_CRITERIA (this file)
 * OMP_O_MAP_C_DESTINATION_NUMBER_LENGTH_LIST (this file)
 * OMP_O_MAP_C_DESTINATION_NUMBER_LIST (this file)
 * OMP_O_MAP_C_DP_ANALYSED_INFO_CRITERIA_LIST (this file)
 * OMP_O_MAP_C_DP_ANALYSED_INFO_CRITERIUM (this file)
 * OMP_O_MAP_C_EPS_AUTHENTICATION_SET_LIST (this file)
 * OMP_O_MAP_C_EPS_AV (this file)
 * OMP_O_MAP_C_EPS_DATA_LIST (this file)
 * OMP_O_MAP_C_EPS_QOS_SUBSCRIBED (this file)
 * OMP_O_MAP_C_EPS_SUBSCRIPTION_DATA (this file)
 * OMP_O_MAP_C_EPS_SUBSCRIPTION_DATA_WITHDRAW (this file)
 * OMP_O_MAP_C_EPS_INFO (this file)
 * OMP_O_MAP_C_EXT_BASIC_SERVICE_GROUP_LIST (this file)
 * OMP_O_MAP_C_EXT_CALL_BAR_FEATURE_LIST (this file)
 * OMP_O_MAP_C_EXT_CALL_BAR_INFO (this file)
 * OMP_O_MAP_C_EXT_CALL_BARRING_FEATURE (this file)
 * OMP_O_MAP_C_EXT_CALL_BARRING_INFO_FOR_CSE (this file)
 * OMP_O_MAP_C_EXTERNAL_CLIENT (this file)
 * OMP_O_MAP_C_EXTERNAL_CLIENT_LIST (this file)
 * OMP_O_MAP_C_EXT_EXTERNAL_CLIENT_LIST (this file)
 * OMP_O_MAP_C_EXT_FORWARDING_INFO_FOR_CSE (this file)
 * OMP_O_MAP_C_EXT_FORW_FEATURE (this file)
 * OMP_O_MAP_C_EXT_FORW_FEATURE_LIST (this file)
 * OMP_O_MAP_C_EXT_FORW_INFO (this file)
 * OMP_O_MAP_C_EXT_SS_DATA (this file)
 * OMP_O_MAP_C_EXT_SS_INFO (this file)
 * OMP_O_MAP_C_EXT_SS_INFO_FOR_CSE (this file)
 * OMP_O_MAP_C_EXT_SS_INFO_LIST (this file)
 * OMP_O_MAP_C_GMLC_LIST (this file)
 * OMP_O_MAP_C_GPRS_CAMEL_TDP_DATA (this file)
 * OMP_O_MAP_C_GPRS_CAMEL_TDP_DATA_LIST (this file)
 * OMP_O_MAP_C_GPRS_CSI (this file)
 * OMP_O_MAP_C_GPRS_DATA_LIST (this file)
 * OMP_O_MAP_C_GPRS_MS_CLASS (this file)
 * OMP_O_MAP_C_GPRS_SUBSCRIPTION_DATA (this file)
 * OMP_O_MAP_C_GPRS_SUBSCRIPTION_DATA_WITHDRAW (this file)
 * OMP_O_MAP_C_GSM_SECURITY_CONTEXT_DATA (this file)
 * OMP_O_MAP_C_LCS_INFORMATION (this file)
 * OMP_O_MAP_C_LCS_PRIVACY_CLASS (this file)
 * OMP_O_MAP_C_LCS_PRIVACY_EXCEPTION_LIST (this file)
 * OMP_O_MAP_C_LOCATION_AREA (this file)
 * OMP_O_MAP_C_LOCATION_INFORMATION (this file)
 * OMP_O_MAP_C_LOCATION_INFORMATION_GPRS (this file)
 * OMP_O_MAP_C_LSA_DATA (this file)
 * OMP_O_MAP_C_LSA_DATA_LIST (this file)
 * OMP_O_MAP_C_LSA_IDENTITY_LIST (this file)
 * OMP_O_MAP_C_LSA_INFORMATION (this file)
 * OMP_O_MAP_C_LSA_INFORMATION_WITHDRAW (this file)
 * OMP_O_MAP_C_M_CSI (this file)
 * OMP_O_MAP_C_MG_CSI (this file)
 * OMP_O_MAP_C_MNP_INFO_RES (this file)
 * OMP_O_MAP_C_MOBILITY_TRIGGERS (this file)
 * OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CB_INFO (this file)
 * OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CF_INFO (this file)
 * OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_CSI (this file)
 * OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_IP_SM_GW_DATA (this file)
 * OMP_O_MAP_C_MODIFICATION_REQUEST_FOR_ODB_DATA (this file)
 * OMP_O_MAP_C_MOLR_CLASS (this file)
 * OMP_O_MAP_C_MOLR_LIST (this file)
 * OMP_O_MAP_C_MSISDN_BS (this file)
 * OMP_O_MAP_C_MT_SMS_CAMEL_TDP_CRITERIA (this file)
 * OMP_O_MAP_C_MT_SMS_CAMEL_TDP_CRITERIA_LIST (this file)
 * OMP_O_MAP_C_O_BCSM_CAMEL_TDP_CRITERIA (this file)
 * OMP_O_MAP_C_O_BCSM_CAMEL_TDP_CRITERIA_LIST (this file)
 * OMP_O_MAP_C_O_BCSM_CAMEL_TDP_DATA (this file)
 * OMP_O_MAP_C_O_BCSM_CAMEL_TDP_DATA_LIST (this file)
 * OMP_O_MAP_C_O_CAUSE_VALUE_CRITERIA (this file)
 * OMP_O_MAP_C_O_CSI (this file)
 * OMP_O_MAP_C_ODB_DATA (this file)
 * OMP_O_MAP_C_ODB_INFO (this file)
 * OMP_O_MAP_C_PAGING_AREA (this file)
 * OMP_O_MAP_C_PDN_GW_IDENTITY (this file)
 * OMP_O_MAP_C_PDN_GW_UPDATE (this file)
 * OMP_O_MAP_C_PDP_CONTEXT (this file)
 * OMP_O_MAP_C_PDP_CONTEXT_INFO (this file)
 * OMP_O_MAP_C_PDP_CONTEXT_INFO_LIST (this file)
 * OMP_O_MAP_C_PLMN_CLIENT_LIST (this file)
 * OMP_O_MAP_C_PS_SUBSCRIBER_STATE (this file)
 * OMP_O_MAP_C_QUINTUPLET_LIST (this file)
 * OMP_O_MAP_C_RADIO_RESOURCE (this file)
 * OMP_O_MAP_C_RADIO_RESOURCE_LIST (this file)
 * OMP_O_MAP_C_RELOCATION_NUMBER (this file)
 * OMP_O_MAP_C_RELOCATION_NUMBER_LIST (this file)
 * OMP_O_MAP_C_REQUESTED_INFO (this file)
 * OMP_O_MAP_C_REQUESTED_SUBSCRIPTION_INFO (this file)
 * OMP_O_MAP_C_RESYNCHRONISATION_INFO (this file)
 * OMP_O_MAP_C_SELECTED_UMTS_ALGORITHMS (this file)
 * OMP_O_MAP_C_SERVICE_TYPE (this file)
 * OMP_O_MAP_C_SERVICE_TYPE_LIST (this file)
 * OMP_O_MAP_C_SGSN_CAMEL_SUBSCRIPTION_INFO (this file)
 * OMP_O_MAP_C_SGSN_CAPABILITY (this file)
 * OMP_O_MAP_C_SMS_CAMEL_TDP_DATA (this file)
 * OMP_O_MAP_C_SMS_CAMEL_TDP_DATA_LIST (this file)
 * OMP_O_MAP_C_SMS_CSI (this file)
 * OMP_O_MAP_C_SS_CAMEL_DATA (this file)
 * OMP_O_MAP_C_SS_CSI (this file)
 * OMP_O_MAP_C_SS_EVENT_LIST (this file)
 * OMP_O_MAP_C_SUBSCRIBER_INFO (this file)
 * OMP_O_MAP_C_SUBSCRIBER_STATE (this file)
 * OMP_O_MAP_C_SUPER_CHARGER_INFO (this file)
 * OMP_O_MAP_C_SUPPORTED_CODECS_LIST (this file)
 * OMP_O_MAP_C_T_BCSM_CAMEL_TDP_CRITERIA (this file)
 * OMP_O_MAP_C_T_BCSM_CAMEL_TDP_CRITERIA_LIST (this file)
 * OMP_O_MAP_C_T_BCSM_CAMEL_TDP_DATA (this file)
 * OMP_O_MAP_C_T_BCSM_CAMEL_TDP_DATA_LIST (this file)
 * OMP_O_MAP_C_T_CAUSE_VALUE_CRITERIA (this file)
 * OMP_O_MAP_C_T_CSI (this file)
 * OMP_O_MAP_C_TELESERVICE_LIST (this file)
 * OMP_O_MAP_C_TPDU_TYPE_CRITERION (this file)
 * OMP_O_MAP_C_TRIPLET_LIST (this file)
 * OMP_O_MAP_C_UESBI_IU (this file)
 * OMP_O_MAP_C_UMTS_SECURITY_CONTEXT_DATA (this file)
 * OMP_O_MAP_C_VBS_DATA_LIST (this file)
 * OMP_O_MAP_C_VGCS_DATA_LIST (this file)
 * OMP_O_MAP_C_VLR_CAMEL_SUBSCRIPTION_INFO (this file)
 * OMP_O_MAP_C_VLR_CAPABILITY (this file)
 * OMP_O_MAP_C_VOICE_BROADCAST_DATA (this file)
 * OMP_O_MAP_C_VOICE_GROUP_CALL_DATA (this file)
 * OMP_O_MAP_C_ZONE_CODE_LIST (this file)
 *
 * MAP_T_Access_Restriction_Data (this file)
 * MAP_T_Access_Type (this file)
 * MAP_T_Additional_Requested_CAMEL_Subscription_Info (this file)
 * MAP_T_Additional_Subscriptions (this file)
 * MAP_T_Call_Type_Criteria (this file)
 * MAP_T_Cancellation_Type (this file)
 * MAP_T_Default_Call_Handling (this file)
 * MAP_T_Default_Session_Handling (this file)
 * MAP_T_Default_SMS_Handling (this file)
 * MAP_T_Domain_Type (this file)
 * MAP_T_Equipment_Status (this file)
 * MAP_T_Failure_Cause (this file)
 * MAP_T_GMLC_Restriction (this file)
 * MAP_T_GPRS_Trigger_Detection_Point (this file)
 * MAP_T_Intra_CUG_Options (this file)
 * MAP_T_IST_Information (this file)
 * MAP_T_IST_Support_Indicator (this file)
 * MAP_T_Key_Status (this file)
 * MAP_T_LCS_Client_Internal_ID (this file)
 * MAP_T_LSA_Only_Access_Indicator (this file)
 * MAP_T_Match_Type (this file)
 * MAP_T_Modification_Instruction (this file)
 * MAP_T_MT_SMS_TPDU_Type (this file)
 * MAP_T_Network_Acess_Mode (this file)
 * MAP_T_Notifiction_To_MS_User (this file)
 * MAP_T_Not_Reachable_Reason (this file)
 * MAP_T_Number_Portability_Status (this file)
 * MAP_T_O_Bcsm_Trigger_Detection_Point (this file)
 * MAP_T_ODB_General_Data (this file)
 * MAP_T_ODB_HPLMN_Data (this file)
 * MAP_T_Offered_CAMEL4_CSIs (this file)
 * MAP_T_PDN_GW_Allocation_Type (this file)
 * MAP_T_Regional_Subscription_Response (this file)
 * MAP_T_Requested_CAMEL_Subscription_Info (this file)
 * MAP_T_Requested_Equipment_Info (this file)
 * MAP_T_Requesting_Node_Type (this file)
 * MAP_T_SMS_Trigger_Detection_Point (this file)
 * MAP_T_Specific_CSI_Deleted_List (this file)
 * MAP_T_Specific_CSI_Withdraw (this file)
 * MAP_T_Subscriber_Status (this file)
 * MAP_T_Supported_CAMEL_Phases (this file)
 * MAP_T_Supported_Features (this file)
 * MAP_T_Supported_RAT_Types (this file)
 * MAP_T_T_BCSM_Trigger_Detection_Point (this file)
 * MAP_T_Type_Of_Update (this file)
 * MAP_T_Used_RAT_Type (this file)
 */

/* Errors:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 *
 * MAP_T_Unknown_Subscriber_Diagnostic (this file)
 * MAP_T_Roaming_Not_Allowed_Cause (this file)
 */

#endif				/* __XMAP_GSM_MS_H__ */
