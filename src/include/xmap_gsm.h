/*****************************************************************************

 @(#) $Id: xmap_gsm.h,v 1.1.2.1 2009-06-21 11:23:46 brian Exp $

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

 $Log: xmap_gsm.h,v $
 Revision 1.1.2.1  2009-06-21 11:23:46  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __XMAP_GSM_H__
#define __XMAP_GSM_H__

#ident "@(#) $RCSfile: xmap_gsm.h,v $ $Name:  $($Revision: 1.1.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/*
 * The <xmap.h> and <xmap_gsm.h> headers declare the interface functions, the
 * structures passed to and from those functions, and the defined constants used
 * by the functions and structure.
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
 *   xom-packages(1) xmap(1) gsm(2) }
 */
#define OMP_O_MAP_GSM_PKG \
	"\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02"	/* 1.3.6.1.4.1.29591.1.1.2 */

#define OMP_O_MAP_GSM_NETWORK \
	"\x04\x00\x00\x01\x00"	/* 0.4.0.0.1 */
	/* { itu-t(0) identified-origanization(4) etsi(0) mobile-domain(0) gsm-Network(1) } */

#define mapP_gsmNetwork(X) (OMP_O_MAP_GSM_NETWORK# #X)

#define OMP_O_MAP_GSM_AC \
	mapP_gsmNetwork(\x00)	/* 0.4.0.0.1.0 */
	/* { itu-t(0) identified-origanization(4) etsi(0) mobile-domain(0) gsm-Network(1) ac(0) } */

#define mapP_acId(X) (OMP_O_MAP_GSM_AC# #X)

#define OMP_O_MAP_GSM_AS \
	mapP_gsmNetwork(\x01)	/* 0.4.0.0.1.1 */

#define mapP_asId(X) (OMP_O_MAP_GSM_AS# #X)

#define OMP_O_MAP_GSM_DIALOGUE_AS \
	mapP_asId(\x01\x01)
	/* { gsm-NetworkId as-Id map-DialoguePDU(1) version1(1) } */

#define OMP_O_MAP_GSM_PROTECTED_DIALOGUE_AS \
	mapP_asID(\x03\x01)
	/* { gsm-NetworkId as-Id map-ProtectedDialoguePDU(3) version1(1) } */

/* Defined constants */

/* Intermediate object identifier macro */
#define mapP_gsm(X) (OMP_O_MAP_GSM_PKG# #X)

/* The OM syntax names that are defined are listed below. */
#define MAP_S_ADDRESS_STRING					((OM_syntax) 34)
#define MAP_S_SUBADDRESS_STRING					((OM_syntax) 35)

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

#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_SM_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ACCESS_NETWORK_SIGNAL_INFO			mapP_gsm(\x8F\x51)	/* 2001 */	/* GSM */
#define OMP_O_MAP_C_ATI_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ATM_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ATSI_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_BASIC_SERVICE_CODE				mapP_gsm(\x8F\x51)	/* 2001 */	/* GSM */
#define OMP_O_MAP_C_BEARER_SERV_NOT_PROV_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_BUSY_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_CALL_BARRED_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_OR_LAI	mapP_gsm(\x8F\x52)	/* 2002 */	/* GSM */
#define OMP_O_MAP_C_CUG_REJECT_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_DATA_MISSING_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_EMLPP_INFO					mapP_gsm(\x8F\x53)	/* 2003 */	/* GSM */
#define OMP_O_MAP_C_EXT_BASIC_SERVICE_CODE			mapP_gsm(\x8F\x51)	/* 2001 */	/* GSM */
#define OMP_O_MAP_C_EXTENSIBLE_CALL_BARRED_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_EXTENSIBLE_SYSTEM_FAILURE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_EXTENSION_CONTAINER				mapP_gsm(\x8F\x54)	/* 2004 */	/* GSM */
#define OMP_O_MAP_C_EXTERNAL_SIGNAL_INFO			mapP_gsm(\x8F\x55)	/* 2005 */	/* GSM */
#define OMP_O_MAP_C_EXT_EXTERNAL_SIGNAL_INFO			mapP_gsm(\x8F\x56)	/* 2006 */	/* GSM */
#define OMP_O_MAP_C_FACILITY_NOT_SUP_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_FORWARDING_FAILED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_FORWARDING_VIOLATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
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
#define OMP_O_MAP_C_GSM_SERVICE_ARGUMENT			mapP_gsm(\x8F\x63)	/* 2019 */	/* GSM */
#define OMP_O_MAP_C_GSM_SERVICE_ERROR				mapP_gsm(\x8F\x64)	/* 2020 */	/* GSM */
#define OMP_O_MAP_C_GSM_SERVICE_REJECT				mapP_gsm(\x8F\x55)	/* 2021 */	/* GSM */
#define OMP_O_MAP_C_GSM_SERVICE_RESULT				mapP_gsm(\x8F\x66)	/* 2022 */	/* GSM */
#define OMP_O_MAP_C_HLR_LIST					mapP_gsm(\x8F\x67)	/* 2023 */	/* GSM */
#define OMP_O_MAP_C_IDENTITY					mapP_gsm(\x8F\x51)	/* 2001 */	/* GSM */
#define OMP_O_MAP_C_ILLEGAL_EQUIPMENT_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ILLEGAL_SS_OPERATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ILLEGAL_SUBSCRIBER_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_IMSI_WITH_LMSI				mapP_gsm(\x8F\x51)	/* 2001 */	/* GSM */
#define OMP_O_MAP_C_INCOMPATIBLE_TERMINAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_INFORMATION_NOT_AVAILABLE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_LCS_CLIENT_EXTERNAL_ID			mapP_gsm(\x8F\x68)	/* 2024 */	/* GSM */
#define OMP_O_MAP_C_LONG_TERM_DENIAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_MC_SS_INFO					mapP_gsm(\x8F\x69)	/* 2025 */	/* GSM */
#define OMP_O_MAP_C_MESSAGE_WAIT_LIST_FULL_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_MM_EVENT_NOT_SUPPORTED_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NAEA_PREFERRED_CI				mapP_gsm(\x8F\x6A)	/* 2026 */	/* GSM */
#define OMP_O_MAP_C_NO_GROUP_CALL_NB_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NO_ROAMING_NB_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NO_SUBSCRIBER_REPLY_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_NUMBER_CHANGE_PARAM				mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_OR_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_PCS_EXTENSIONS				mapP_gsm(\x8F\x6B)	/* 2027 */	/* GSM */
#define OMP_O_MAP_C_POSITION_METHOD_FAILURE_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_PRIVATE_EXTENSION_LIST			mapP_gsm(\x8F\x6C)	/* 2028 */	/* GSM */
#define OMP_O_MAP_C_PRIVATE_EXTENSION				mapP_gsm(\x8F\x6D)	/* 2029 */	/* GSM */
#define OMP_O_MAP_C_PW_REGISTRATION_FAILURE_CAUSE		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_RESOURCE_LIMITATION_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_ROAMING_NOT_ALLOWED_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SECURE_TRANSPORT_ERROR_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SHORT_TERM_DENIAL_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SLR_ARG_EXTENSION_CONTAINER			mapP_gsm(\x8F\x6E)	/* 2030 */	/* GSM */
#define OMP_O_MAP_C_SLR_ARG_PCS_EXTENSIONS			mapP_gsm(\x8F\x6F)	/* 2031 */	/* GSM */
#define OMP_O_MAP_C_SM_DELIVERY_FAILURE_CAUSE			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SPECIFIC_INFORMATION			mapP_gsm(\x8F\x70)	/* 2032 */	/* GSM */
#define OMP_O_MAP_C_SS_INCOMPATIBILITY_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SS_NOT_AVAILABLE_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SS_SUBSCRIPTION_VIOLATION_PARAM		mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SUB_BUSY_FOR_MT_SMS_PARAM			mapP_gsm(\x87\x69)	/* 1001 */	/* GSM ERR */
#define OMP_O_MAP_C_SUBSCRIBER_IDENTITY				mapP_gsm(\x8F\x51)	/* 2001 */	/* GSM */
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

/* The OM attribute names that are defined are listed below. */

#define MAP_A_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM				((OM_type)10001)	/* GSM ERR */
#define MAP_A_ABSENT_SUBSCRIBER_REASON					((OM_type)10002)	/* GSM ERR */
#define MAP_A_ACCESS_NETWORK_PROTOCOL_ID				((OM_type)12001)	/* GSM */
#define MAP_A_ADDITIONAL_ABSENT_SUBSCRIBER_DIAGNOSTIC_SM		((OM_type)10003)	/* GSM ERR */
#define MAP_A_ADDITIONAL_INFO						((OM_type)20001)	/* GSM Common */
#define MAP_A_ADDITIONAL_NETWORK_RESOURCE				((OM_type)10004)	/* GSM ERR */
#define MAP_A_ADDITIONAL_NUMBER						((OM_type)20002)	/* GSM common */
#define MAP_A_ADDITIONAL_SUBSCRIPTIONS					((OM_type)20003)	/* GSM common */
#define MAP_A_AGE_OF_LOCATION_INFORMATION				((OM_type)12002)	/* GSM */
#define MAP_A_ALERTING_PATTERN						((OM_type)12003)	/* GSM */
#define MAP_A_ALL_INFORMATION_SENT					((OM_type)20004)	/* GSM common */
#define MAP_A_AN_APDU							((OM_type)20005)	/* GSM common */
#define MAP_A_APPLICATION_CONTEXT					((OM_type)12004)	/* GSM */
#define MAP_A_ASCI_CALL_REFERENCE					((OM_type)12005)	/* GSM */
#define MAP_A_BASIC_SERVICE_GROUP_LIST					((OM_type)20006)	/* GSM common */
#define MAP_A_BASIC_SERVICE_GROUP					((OM_type)20007)	/* GSM common */
#define MAP_A_BASIC_SERVICE						((OM_type)20008)	/* GSM common */
#define MAP_A_BEARER_SERVICE						((OM_type)10006)	/* GSM ERR */
#define MAP_A_CALL_BARRING_CAUSE					((OM_type)10008)	/* GSM ERR */
#define MAP_A_CALL_BARRING_FEATURE_LIST					((OM_type)20009)	/* GSM common */
#define MAP_A_CALL_BARRING_INFO						((OM_type)20010)	/* GSM common */
#define MAP_A_CALL_INFO							((OM_type)20011)	/* GSM common */
#define MAP_A_CALL_PRIORITY						((OM_type)20012)	/* GSM common */
#define MAP_A_CCBS_BUSY							((OM_type)10009)	/* GSM ERR */
#define MAP_A_CCBS_FEATURE						((OM_type)20013)	/* GSM common */
#define MAP_A_CCBS_POSSIBLE						((OM_type)10010)	/* GSM ERR */
#define MAP_A_CELL_GLOBAL_ID_OR_SERVICE_AREA_ID_FIXED_LENGTH		((OM_type)12006)	/* GSM */
#define MAP_A_CKSN							((OM_type)20014)	/* GSM common */
#define MAP_A_CUG_INTERLOCK						((OM_type)20015)	/* GSM common */
#define MAP_A_CUG_REJECT_CAUSE						((OM_type)10011)	/* GSM ERR */
#define MAP_A_D_CSI							((OM_type)20016)	/* GSM common */
#define MAP_A_DEFAULT_PRIORITY						((OM_type)12007)	/* GSM */
#define MAP_A_DESTINATION_REFERENCE					((OM_type)12008)	/* GSM */
#define MAP_A_DIAGNOSTIC_INFO						((OM_type)10007)	/* GSM ERR */
#define MAP_A_EMLPP_PRIORITY						((OM_type)12009)	/* GSM */
#define MAP_A_EXT_BEARER_SERVICE					((OM_type)10012)	/* GSM ERR */
#define MAP_A_EXTENSIBLE_CALL_BARRED_PARAM				((OM_type)10013)	/* GSM ERR */
#define MAP_A_EXTENSIBLE_SYSTEM_FAILURE_PARAM				((OM_type)10014)	/* GSM ERR */
#define MAP_A_EXTENSION_CONTAINER					((OM_type)12000)	/* GSM */
#define MAP_A_EXTENSION							((OM_type)12001)	/* GSM */
#define MAP_A_EXTENSIONS						((OM_type)12002)	/* GSM */
#define MAP_A_EXTERNAL_ADDRESS						((OM_type)12003)	/* GSM */
#define MAP_A_EXT_ID							((OM_type)12004)	/* GSM */
#define MAP_A_EXT_PROTOCOL_ID						((OM_type)12005)	/* GSM */
#define MAP_A_EXT_SS_STATUS						((OM_type)12006)	/* GSM */
#define MAP_A_EXT_TELESERVICE						((OM_type)10017)	/* GSM ERR */
#define MAP_A_EXT_TYPE							((OM_type)12007)	/* GSM */
#define MAP_A_FORWARDED_TO_NUMBER					((OM_type)20017)	/* GSM common */
#define MAP_A_FORWARDED_TO_SUBADDRESS					((OM_type)20018)	/* GSM common */
#define MAP_A_FORWARDING_FEATURE_LIST					((OM_type)20019)	/* GSM common */
#define MAP_A_FORWARDING_INFO						((OM_type)20020)	/* GSM common */
#define MAP_A_FORWARDING_OPTIONS					((OM_type)20021)	/* GSM common */
#define MAP_A_GLOBAL_CELL_ID						((OM_type)12001)	/* GSM */
#define MAP_A_GPRS_CONNECTION_SUSPENDED					((OM_type)10018)	/* GSM ERR */
#define MAP_A_GPRS_NODE_INDICATOR					((OM_type)20022)	/* GSM common */
#define MAP_A_GROUP_ID							((OM_type)20023)	/* GSM common */
#define MAP_A_HLR_ID							((OM_type)12001)	/* GSM */
#define MAP_A_IMEI							((OM_type)12001)	/* GSM */
#define MAP_A_IMSI							((OM_type)12001)	/* GSM */
#define MAP_A_IST_ALERT_TIMER						((OM_type)20024)	/* GSM common */
#define MAP_A_IST_INFORMATION_WITHDRAW					((OM_type)20025)	/* GSM common */
#define MAP_A_IST_SUPPORT_INDICATOR					((OM_type)20026)	/* GSM common */
#define MAP_A_KC							((OM_type)20027)	/* GSM common */
#define MAP_A_LAI_FIXED_LENGTH						((OM_type)12001)	/* GSM */
#define MAP_A_LCS_CLIENT_INTERNAL_ID					((OM_type)12001)	/* GSM */
#define MAP_A_LCS_SERVICE_TYPE_ID					((OM_type)12001)	/* GSM */
#define MAP_A_LMSI							((OM_type)12001)	/* GSM */
#define MAP_A_LONG_FORWARDED_TO_NUMBER					((OM_type)20028)	/* GSM common */
#define MAP_A_LONG_FTN_SUPPORTED					((OM_type)20029)	/* GSM common */
#define MAP_A_MAXIMUM_ENTITLED_PRIORITY					((OM_type)12001)	/* GSM */
#define MAP_A_MAX_MC_BEARERS						((OM_type)12001)	/* GSM */
#define MAP_A_MC_BEARERS						((OM_type)12001)	/* GSM */
#define MAP_A_MSC_NUMBER						((OM_type)20030)	/* GSM common */
#define MAP_A_MSISDN							((OM_type)20031)	/* GSM common */
#define MAP_A_NAEA_CIC							((OM_type)12001)	/* GSM */
#define MAP_A_NAEA_PREFERRED_CIC					((OM_type)12001)	/* GSM */
#define MAP_A_NAEA_PREFERRED_CI						((OM_type)20032)	/* GSM common */
#define MAP_A_NA_ESRK_REQUEST						((OM_type)12001)	/* GSM */
#define MAP_A_NBR_SB							((OM_type)12001)	/* GSM */
#define MAP_A_NBR_USER							((OM_type)12001)	/* GSM */
#define MAP_A_NEEDED_LCS_CAPABILITY_NOT_SUPPORTED_IN_SERVING_NODE	((OM_type)10019)	/* GSM ERR */
#define MAP_A_NETWORK_NODE_NUMBER					((OM_type)20033)	/* GSM common */
#define MAP_A_NETWORK_RESOURCE						((OM_type)10020)	/* GSM ERR */
#define MAP_A_NETWORK_SIGNAL_INFO					((OM_type)20034)	/* GSM common */
#define MAP_A_NO_REPLY_CONDITION_TIME					((OM_type)20035)	/* GSM common */
#define MAP_A_NUMBER_PORTABILITY_STATUS					((OM_type)20036)	/* GSM common */
#define MAP_A_O_BCSM_CAMEL_TDP_CRITERIA_LIST				((OM_type)20037)	/* GSM common */
#define MAP_A_O_CSI							((OM_type)20038)	/* GSM common */
#define MAP_A_OFFERED_CAMEL4_CSIS					((OM_type)20039)	/* GSM common */
#define MAP_A_ORIGINATING_REFERENCE					((OM_type)12001)	/* GSM */
#define MAP_A_P_ABORT_CAUSE						((OM_type)12001)	/* GSM */
#define MAP_A_P_ABORT_REASON						((OM_type)12001)	/* GSM */
#define MAP_A_PAGING_AREA						((OM_type)20040)	/* GSM common */
#define MAP_A_PASSWORD							((OM_type)20041)	/* GSM common */
#define MAP_A_PCS_EXTENSION						((OM_type)12001)	/* GSM */
#define MAP_A_PCS_EXTENSIONS						((OM_type)12001)	/* GSM */
#define MAP_A_POSITION_METHOD_FAILURE_DIAGNOSTIC			((OM_type)10021)	/* GSM ERR */
#define MAP_A_PRIVATE_EXTENSION_LIST					((OM_type)12001)	/* GSM */
#define MAP_A_PRIVATE_EXTENSION						((OM_type)12001)	/* GSM */
#define MAP_A_PROBLEM_DIAGNOSTIC					((OM_type)12001)	/* GSM */
#define MAP_A_PROTECTED_PAYLOAD						((OM_type)10022)	/* GSM ERR */
#define MAP_A_PROTOCOL_ID						((OM_type)12001)	/* GSM */
#define MAP_A_PROVIDER_REASON						((OM_type)12001)	/* GSM */
#define MAP_A_PW_REGISTRATION_FAILURE_CAUSE				((OM_type)10033)	/* GSM ERR */
#define MAP_A_REFUSE_REASON						((OM_type)12001)	/* GSM */
#define MAP_A_RELEASE_METHOD						((OM_type)12001)	/* GSM */
#define MAP_A_REPORT_CAUSE						((OM_type)12001)	/* GSM */
#define MAP_A_REQUESTED_INFO						((OM_type)20042)	/* GSM common */
#define MAP_A_ROAMING_NOT_ALLOWED_CAUSE					((OM_type)10023)	/* GSM ERR */
#define MAP_A_SAI_PRESENT						((OM_type)20043)	/* GSM common */
#define MAP_A_SECURITY_HEADER						((OM_type)10024)	/* GSM ERR */
#define MAP_A_SGSN_NUMBER						((OM_type)20044)	/* GSM common */
#define MAP_A_SHAPE_OF_LOCATION_ESTIMATED_NOT_SUPPORTED			((OM_type)10025)	/* GSM ERR */
#define MAP_A_SIGNAL_INFO						((OM_type)12001)	/* GSM */
#define MAP_A_SLR_ARG_PCS_EXTENSIONS					((OM_type)12001)	/* GSM */
#define MAP_A_SM_ENUMERATED_DELIVERY_FAILURE_CAUSE			((OM_type)10026)	/* GSM ERR */
#define MAP_A_SOURCE							((OM_type)12001)	/* GSM */
#define MAP_A_SPECIFIC_INFORMATION					((OM_type)12001)	/* GSM */
#define MAP_A_SS_CODE							((OM_type)12001)	/* GSM */
#define MAP_A_SS_DATA							((OM_type)20045)	/* GSM common */
#define MAP_A_SS_LIST							((OM_type)20046)	/* GSM common */
#define MAP_A_SS_STATUS							((OM_type)12001)	/* GSM */
#define MAP_A_SS_SUBSCRIPTION_OPTION					((OM_type)20047)	/* GSM common */
#define MAP_A_SUBSCRIBER_INFO						((OM_type)20048)	/* GSM common */
#define MAP_A_SUPPORTED_CAMEL_PHASES					((OM_type)20049)	/* GSM common */
#define MAP_A_SUPPORTED_LCS_CAPABILITY_SETS				((OM_type)20050)	/* GSM common */
#define MAP_A_T_BCSM_CAMEL_TDP_CRITERIA_LIST				((OM_type)20051)	/* GSM common */
#define MAP_A_T_CSI							((OM_type)20052)	/* GSM common */
#define MAP_A_TELESERVICE						((OM_type)10029)	/* GSM ERR */
#define MAP_A_TMSI							((OM_type)12001)	/* GSM */
#define MAP_A_TRANSLATED_B_NUMBER					((OM_type)20053)	/* GSM common */
#define MAP_A_UNAUTHORIZED_LCS_CLIENT_DIAGNOSTIC			((OM_type)10030)	/* GSM ERR */
#define MAP_A_UNAUTHORIZED_MESSAGE_ORIGINATOR				((OM_type)10031)	/* GSM ERR */
#define MAP_A_UNKNOWN_SUBSCRIBER_DIAGNOSTIC				((OM_type)10032)	/* GSM ERR */
#define MAP_A_V_GMLC_ADDRESS						((OM_type)20054)	/* GSM common */

#define MAP_VL_DESTINATION_REFERENCE	20
#define MAP_VL_ORIGINATING_REFERENCE	20

#define MAP_VN_PRIVATE_EXTENSION	10

/*
 * The following enumerations tags and enumeration constants are defined for use
 * as values of the corresponding OM attributes:
 */

/* MAP_T_Release_Method: */
typedef enum {
	MAP_T_NORMAL_RELEASE = 0,
	MAP_T_PREARRANGED_END = 1,
} MAP_T_Release_Method;

/* MAP_T_Problem_Diagnostic: */
typedef enum {
	MAP_T_ABNORMAL_EVENT_DETECTED_BY_PEER = 0,
	MAP_T_RESPONSE_REJECTED_BY_THE_PEER = 1,
	MAP_T_ABNORMAL_EVENT_RECEIVED_FROM_THE_PEER = 2,
	MAP_T_MESSAGE_CANNOT_BE_DELIVERED_TO_THE_PEER = 3,
} MAP_T_Problem_Diagnostic;

/* MAP_T_Provider_Reason: */
typedef enum {
	MAP_T_PROVIDER_MALFUNCTION = 0,
	MAP_T_SUPPORTING_DIALOGUE_TRANSACTION_RELEASED = 1,
	MAP_T_RESOURCE_LIMITATION = 2,
	MAP_T_MAINTENANCE_ACTIVITY = 3,
	MAP_T_VERSION_INCOMPATIBILITY = 4,
	MAP_T_ABNORMAL_MAP_DIALOGUE = 5,
} MAP_T_Provider_Reason;

/* MAP_T_Source: */
typedef enum {
	MAP_T_MAP_PROBLEM = 0,
	MAP_T_TC_PROBLEM = 1,
	MAP_T_NETWORK_SERVICE_PROBLEM = 2,
} MAP_T_Source;

/* MAP_T_P_Abort_Reason: */
typedef enum {
	MAP_T_ABNORMAL_DIALOGUE = 0,
	MAP_T_INVALID_PDU = 1,
} MAP_T_P_Abort_Reason;

/* MAP_T_P_Abort_Cause: */
typedef enum {
	MAP_T_UNRECOGNIZED_MESSAGE_TYPE = 0,
	MAP_T_UNRECOGNIZED_TRANSACTION_ID = 1,
	MAP_T_BADLY_FORMATTED_TRANSACTION_PORTION = 2,
	MAP_T_INCORRECT_TRANSACTION_PORTION = 3,
	MAP_T_RESOURCE_LIMITATION = 4,
	MAP_T_ABNORMAL_DIALOGUE = 5,
	MAP_T_NO_COMMON_DIALOGUE_PORTION = 6,
} MAP_T_P_Abort_Cause;

/* MAP_T_Report_Cause: */
typedef enum {
	MAP_T_NO_TRANSLATIONS_FOR_ADDRESS_OF_SUCH_NATURE = 0,
	MAP_T_NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS = 1,
	MAP_T_SUBSYSTEM_CONGESTION = 2,
	MAP_T_SUBSYSTEM_FAILURE = 3,
	MAP_T_UNEQUIPPED_USER = 4,
	MAP_T_MTP_FAILUIRE = 5,
	MAP_T_NETWORK_CONGESTION = 6,
	MAP_T_SCCP_UNQUALIFIED = 7,
	MAP_T_ERROR_IN_MESSAGE_TRANSPORT = 8,
	MAP_T_ERROR_IN_LOCAL_PROCESSING = 9,
	MAP_T_DESTINATION_CANNOT_PERFORM_REASSEMBLY = 10,
	MAP_T_SCCP_FAILURE = 11,
	MAP_T_HOP_COUNTER_VIOLATION = 12,
	MAP_T_SEGMENTATION_NOT_SUPPORTED = 13,
	MAP_T_SEGMENTATION_FAILURE = 14,
	/* ANSI SCCP only */
	MAP_T_MESSAGE_CHANGE_FAILURE = 247,
	MAP_T_INVALID_INS_ROUTING_REQUEST = 248,
	MAP_T_INVALID_ISNI_ROUTING_REQUEST = 249,
	MAP_T_UNAUTHORIZED_MESSAGE = 250,
	MAP_T_MESSAGE_INCOMPATIBILITY = 251,
	MAP_T_CANNOT_PERFORM_ISNI_CONSTRAINED_ROUTING = 252,
	MAP_T_REDUNDANT_ISNI_CONSTRAINED_ROUTING_INFORMATION = 253,
	MAP_T_UNABLE_TO_PERFORM_ISNI_IDENTIFICATION = 254,
} MAP_T_Report_Cause;

/* MAP_T_Refuse_Reason: */
typedef enum {
	MAP_T_NO_REASON_GIVEN = 0,
	MAP_T_INVALID_DESTINATION_REFERENCE = 1,
	MAP_T_INVALID_ORIGINATING_REFERENCE = 2,
	MAP_T_ENCAPSULATED_AC_NOT_SUPPORTED = 3,
	MAP_T_TRANSPORT_PROTECTION_NOT_ADEQUATE = 4,
	/* not on wire */
	MAP_T_REMOTE_NODE_NOT_REACHABLE = -1,
	MAP_T_APPLICATION_CONTEXT_NOT_SUPPORTED = -2,
	MAP_T_POTENTIAL_VERSION_INCOMPATIBILITY = -3,
	MAP_T_SECURED_TRANSPORT_NOT_POSSIBLE = -4,
} MAP_T_Refuse_Reason;

/* MAP_T_User_Error: */
typedef enum {
	MAP_E_INITIATING_RELEASE = 0,
	MAP_E_UNKNOWN_SUBSCRIBER = 1,
	MAP_E_UNKNOWN_MSC = 3,
	MAP_E_SECURE_TRANSPORT_ERROR = 4,
	MAP_E_UNIDENTIFIED_SUBSCRIBER = 5,
	MAP_E_ABSENT_SUBSCRIBER_SM = 6,
	MAP_E_UNKNOWN_EQUIPMENT = 7,
	MAP_E_ROAMING_NOT_ALLOWED = 8,
	MAP_E_ILLEGAL_SUBSCRIBER = 9,
	MAP_E_BEARER_SERVICE_NOT_PROVISIONED = 10,
	MAP_E_TELESERVICE_NOT_PROVISIONED = 11,
	MAP_E_ILLEGAL_EQUIPMENT = 12,
	MAP_E_CALL_BARRED = 13,
	MAP_E_FORWARDING_VIOLATION = 14,
	MAP_E_CUG_REJECT = 15,
	MAP_E_ILLEGAL_SS_OPERATION = 16,
	MAP_E_SS_ERROR_STATUS = 17,
	MAP_E_SS_NOT_AVAILABLE = 18,
	MAP_E_SS_SUBSCRIPTION_VIOLATION = 19,
	MAP_E_SS_INCOMPATIBILITY = 20,
	MAP_E_FACILITY_NOT_SUPPORTED = 21,
	MAP_E_NO_HANDOVER_NUMBER_AVAILABLE = 25,
	MAP_E_SUBSEQUENT_HANDOVER_FAILURE = 26,
	MAP_E_ABSENT_SUBSCRIBER = 27,
	MAP_E_INCOMPATIBLE_TERMINAL = 28,
	MAP_E_SHORT_TERM_DENIAL = 29,
	MAP_E_LONG_TERM_DENIAL = 30,
	MAP_E_SUBSCRIBER_BUSY_FOR_MT_SMS = 31,
	MAP_E_SM_DELIVERY_FAILURE = 32,
	MAP_E_MESSAGE_WAITING_LIST_FULL = 33,
	MAP_E_SYSTEM_FAILURE = 34,
	MAP_E_DATA_MISSING = 35,
	MAP_E_UNEXPECTED_DATA_VALUE = 36,
	MAP_E_PW_REGISTRATION_FAILURE = 37,
	MAP_E_NEGATIVE_PW_CHECK = 38,
	MAP_E_NO_ROAMING_NUMBER_AVAILABLE = 39,
	MAP_E_TRACING_BUFFER_FULL = 40,
	MAP_E_TARGET_CELL_OUTSIDE_GROUP_CALL_AREA = 42,
	MAP_E_NUMBER_OF_PW_ATTEMPTS_VIOLATION = 43,
	MAP_E_NUMBER_CHANGED = 44,
	MAP_E_BUSY_SUBSCRIBER = 45,
	MAP_E_NO_SUBSCRIBER_REPLY = 46,
	MAP_E_FORWARDING_FAILED = 47,
	MAP_E_OR_NOT_ALLOWED = 48,
	MAP_E_ATI_NOT_ALLOWED = 49,
	MAP_E_NO_GROUP_CALL_NUMBER_AVAILABLE = 50,
	MAP_E_RESOURCE_LIMITATION = 51,
	MAP_E_UNAUTHORIZED_REQUESTING_NETWORK = 52,
	MAP_E_UNAUTHORIZED_LCS_CLIENT = 53,
	MAP_E_POSITION_METHOD_FAILURE = 54,
	MAP_E_UNKNOWN_OR_UNREACHABLE_LCS_CLIENT = 58,
	MAP_E_MM_EVENT_NOT_SUPPORTED = 59,
	MAP_E_ATSI_NOT_ALLOWED = 60,
	MAP_E_ATM_NOT_ALLOWED = 61,
	MAP_E_INFORMATION_NOT_AVAILABLE = 62,
	MAP_E_UNKNOWN_ALPHABET = 71,
	MAP_E_USSD_BUSY = 72,
} MAP_T_User_Error;

/* MAP_T_Provider_Error: */
typedef enum {
	MAP_T_DUPLICATED_INVOKE_ID = 0,
	MAP_T_NOT_SUPPORTED_SERVICE = 1,
	MAP_T_MISTYPED_PARAMETER = 2,
	MAP_T_RESOURCE_LIMITATION = 3,
	MAP_T_INITIATING_RELEASE = 4,
	MAP_T_UNEXPECTED_RESPONSE_FROM_THE_PEER = 5,
	MAP_T_SERVICE_COMPLETION_FAILURE = 6,
	MAP_T_NO_RESPONSE_FROM_THE_PEER = 7,
	MAP_T_INVALID_RESPONSE_RECEIVED = 8,
} MAP_T_Provider_Error;

/* MAP_T_Resource_Unavailable_Reason: */
typedef enum {
	MAP_T_SHORT_TERM_RESOURCE_LIMITATION = 0,
	MAP_T_LONG_TERM_RESOURCE_LIMITATION = 1,
} MAP_T_Resource_Unavailable_Reason = 2;

/* MAP_T_Procedure_Cancellation_Reason: */
typedef enum {
	MAP_T_HANDOVER_CANCELLATION = 0,
	MAP_T_RADIO_CHANNEL_RELEASE = 1,
	MAP_T_NETWORK_PATH_RELEASE = 2,
	MAP_T_CALL_RELEASE = 3,
	MAP_T_ASSOCIATED_PROCEDURE_FAILURE = 4,
	MAP_T_TANDEM_DIALOGUE_RELEASE = 5,
	MAP_T_REMOTE_OPERATIONS_FAILURE = 6,
} MAP_T_Procedure_Cancellation_Reason;

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

#if 0
/* Service-Error problem values: */
#define MAP_E_INITIATING_RELEASE				mapP_problem(xx)

#define MAP_E_SYSTEM_FAILURE					mapP_problem(34)
#define MAP_E_DATA_MISSING					mapP_problem(35)
#define MAP_E_UNEXPECTED_DATA_VALUE				mapP_problem(36)
#define MAP_E_FACILITY_NOT_SUPPORTED				mapP_problem(21)
#define MAP_E_INCOMPATIBLE_TERMINAL				mapP_problem(28)
#define MAP_E_RESOURCE_LIMITATION				mapP_problem(51)

/* Service-Error parameter classes: */

/* Service-Error attributes: */
#endif

/* MAP_T_Network_Resource: */
typedef enum {
	MAP_T_PLMN = 0,
	MAP_T_HLR = 1,
	MAP_T_VLR = 2,
	MAP_T_PVLR = 3,
	MAP_T_CONTROLLING_MSC = 4,
	MAP_T_VMSC = 5,
	MAP_T_EIR = 6,
	MAP_T_RSS = 7,
} MAP_T_Network_Resource;

/* MAP_T_Additional_Network_Resource: */
typedef enum {
	MAP_T_SGSN = 0,
	MAP_T_GGSN = 1,
	MAP_T_GMLC = 2,
	MAP_T_GSM_SCF = 3,
	MAP_T_NPLR = 4,
	MAP_T_AUC = 5,
} MAP_T_Additional_Network_Resource;

/* MAP_T_Unknown_Subscriber_Diagnostic: */
typedef enum {
	MAP_T_IMSI_UNKNOWN = 0,
	MAP_T_GPRS_SUBSCRIPTION_UNKNOWN = 1,
	MAP_T_NPDB_MISMATCH = 2,
} MAP_T_Unknown_Subscriber_Diagnostic;

/* MAP_T_Supported_CAMEL_Phases: */
typedef enum {
	MAP_T_PHASE_1 = 0,
	MAP_T_PHASE_2 = 1,
	MAP_T_PHASE_3 = 2,
	MAP_T_PHASE_4 = 3,
} MAP_T_Supported_CAMEL_Phases;

/* MAP_T_Roaming_Not_Allowed_Cause: */
typedef enum {
	MAP_T_PLMN_ROAMING_NOT_ALLOWED = 0,
	MAP_T_OPERATOR_DETERMINED_BARRING = 1,
} MAP_T_Roaming_Not_Allowed_Cause;

/* MAP_T_Absent_Subscriber_Reason: */
typedef enum {
	MAP_T_IMSI_DETACH = 0,
	MAP_T_RESTRICTED_AREA = 1,
	MAP_T_NO_PAGE_RESPONSE = 2,
	MAP_T_PURGED_MS = 3,
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

/* MAP_T_SM_Enumerated_Delivery_Failure_Cause: */
typedef enum {
	MAP_T_MEMORY_CAPACITY_EXCEEDED = 0,
	MAP_T_EQUIPMENT_PROTOCOL_ERROR = 1,
	MAP_T_EQUIPMENT_NOT_SM_EQUIPPED = 2,
	MAP_T_UNKNOWN_SERVICE_CENTER = 3,
	MAP_T_SC_CONGESTION = 4,
	MAP_T_INVALID_SME_ADDRESS = 5,
	MAP_T_SUBSCRIBER_NOT_SC_SUBSCRIBER = 6,
} MAP_T_SM_Enumerated_Delivery_Failure_Cause;

/* MAP_T_Unauthorized_LCS_Client_Diagnostic: */
typedef enum {
	MAP_T_NO_ADDITIONAL_INFORMATION = 0,
	MAP_T_CLIENT_NOT_IN_MS_PRIVACY_EXCEPTION_LIST = 1,
	MAP_T_CALL_TO_CLIENT_NOT_SETUP = 2,
	MAP_T_PRIVACY_OVERRIDE_NOT_APPLICABLE = 3,
	MAP_T_DISALLOWED_BY_LOCAL_REGULATORY_REQUIREMENTS = 4,
	MAP_T_UNAUTHORIZED_PRIVACY_CLASS = 5,
	MAP_T_UNAUTHORIZED_CALL_SESSION_UNRELATED_EXTERNAL_CLIENT = 6,
	MAP_T_UNAUTHORIZED_CALL_SESSION_RELATED_EXTERNAL_CLIENT = 7,
} MAP_T_Unauthorized_LCS_Client_Diagnostic;

/* MAP_T_Position_Method_Failure_Diagnostic: */
typedef enum {
	MAP_T_CONGESTION = 0,
	MAP_T_INSUFFICIENT_RESOURCES = 1,
	MAP_T_INSUFFICIENT_MEASUREMENT_DATA = 2,
	MAP_T_INCONSISTENT_MEASUREMENT_DATA = 3,
	MAP_T_LOCATION_PROCEDURE_NOT_COMPLETED = 4,
	MAP_T_LOCATION_PROCEDURE_NOT_SUPPORTED_BY_TARGET_MS = 5,
	MAP_T_QOS_NOT_ATTAINABLE = 6,
	MAP_T_POSITION_METHOD_NOT_AVAILABLE_IN_NETWORK = 7,
	MAP_T_POSITION_METHOD_NOT_AVAILABLE_IN_LOCATION_AREA = 8,
} MAP_T_Position_Method_Failure_Diagnostic;

/* Common Data Types: */

/* MAP_T_Protocol_Id: */
typedef enum {
	MAP_T_GSM_0408 = 1,
	MAP_T_GSM_0806 = 2,
	MAP_T_BSSMAP = 3,	/* reserved */
	MAP_T_ETS_300102_1 = 4,
} MAP_T_Protocol_Id;

/* MAP_T_Ext_Protocol_Id: */
typedef enum {
	MAP_T_ETS_300356 = 1,
} MAP_T_Ext_Protocol_Id;

/* MAP_T_Access_Network_Protocol_Id: */
typedef enum {
	MAP_T_TS3G_48006 = 1,
	MAP_T_TS3G_25413 = 2,
} MAP_T_Access_Network_Protocol_Id;

/* MAP_T_LCS_Client_Internal_ID: */
typedef enum {
	MAP_T_BROADCAST_SERVICE = 0,
	MAP_T_O_AND_M_HPLMN = 1,
	MAP_T_O_AND_M_VPLMN = 2,
	MAP_T_ANONYMOUS_LOCATION = 3,
	MAP_T_TARGET_MS_SUBSCRIBED_SERVICE = 4,
} MAP_T_LCS_Client_Internal_ID;

/* MAP_T_LCS_Service_Type_Id: */
typedef enum {
	/* The values of LCSServiceTypeID are defined according to 3GPP TS 22.071. */
	MAP_T_EMERGENCY_SERVICES = 0,
	MAP_T_EMERGENCY_ALERT_SERVICES = 1,
	MAP_T_PERSON_TRACKING = 2,
	MAP_T_FLEET_MANAGEMENT = 3,
	MAP_T_ASSET_MANAGEMENT = 4,
	MAP_T_TRAFFIC_CONGESTION_REPORTING = 5,
	MAP_T_ROADSIDE_ASSISTANCE = 6,
	MAP_T_ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE = 7,
	MAP_T_NAVIGATION = 8,
	MAP_T_CITY_SIGHSEEING = 9,
	MAP_T_LOCALIZED_ADVERTISING = 10,
	MAP_T_MOBILE_YELLOW_PAGES = 11,
	MAP_T_TRAFFIC_AND_PUBLIC_TRANSPORTATION_INFO = 12,
	MAP_T_WEATHER = 13,
	MAP_T_ASSET_AND_SERVICE_FINDING = 14,
	MAP_T_GAMING = 15,
	MAP_T_FIND_YOUR_FRIEND = 16,
	MAP_T_DATING = 17,
	MAP_T_CHATTING = 18,
	MAP_T_ROUTE_FINDING = 19,
	MAP_T_WHERE_AM_I = 20,
} MAP_T_LCS_Service_Type_Id;

/* MAP_T_Number_Portability_Status: */
typedef enum {
	MAP_T_NOT_KNOWN_TO_BE_PORTED = 0,
	MAP_T_OWN_NUMBER_PORTED_OUT = 1,
	MAP_T_FOREIGN_NUMBER_PORTED_TO_FOREIGN_NETWORK = 2,
	MAP_T_OWN_NUMBER_NOT_PORTED_OUT = 4,
	MAP_T_FOREIGN_NUMBER_PORTED_IN = 5,
} MAP_T_Number_Portability_Status;

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

/* MAP_T_PW_Registration_Failure_Cause: */
typedef enum {
	MAP_T_UNDETERMINED = 0,
	MAP_T_INVALID_FORMAT,
	MAP_T_NEW_PASSWORDS_MISMATCH,
} MAP_T_PW_Registration_Failure_Cause;

/* MAP_T_Operation_Code: */
typedef enum {
	MAP_T_UPDATE_LOCATION = 2,
	MAP_T_CANCEL_LOCATION = 3,
	MAP_T_PROVIDE_ROAMING_NUMBER = 4,
	MAP_T_NOTE_SUBSCRIBER_DATA_MODIFIED = 5,
	MAP_T_RESUME_CALL_HANDLING = 6,
	MAP_T_INSERT_SUBSCRIBER_DATA = 7,
	MAP_T_DELETE_SUBSCRIBER_DATA = 8,
	MAP_T_REGISTER_SS = 10,
	MAP_T_ERASE_SS = 11,
	MAP_T_ACTIVATE_SS = 12,
	MAP_T_DEACTIVATE_SS = 13,
	MAP_T_INTERROGATE_SS = 14,
	MAP_T_AUTHENTICATION_FAILURE_REPORT = 15,
	MAP_T_REGISTER_PASSWORD = 17,
	MAP_T_GET_PASSWORD = 18,
	MAP_T_RELEASE_RESOURCES = 20,
	MAP_T_MT_FORWARD_SM_VGCS = 21,
	MAP_T_SEND_ROUTING_INFO = 22,
	MAP_T_UPDATE_GPRS_LOCATION = 23,
	MAP_T_SEND_ROUTING_INFO_FOR_GPRS = 24,
	MAP_T_FAILURE_REPORT = 25,
	MAP_T_NOTE_MS_PRESENT_FOR_GPRS = 26,
	MAP_T_SEND_END_SIGNAL = 29,
	MAP_T_PROCESS_ACCESS_SIGNALLING = 33,
	MAP_T_FORWARD_ACCESS_SIGNALLING = 34,
	MAP_T_RESET = 37,
	MAP_T_FORWARD_CHECK_SS_INDICATION = 38,
	MAP_T_PREPARE_GROUP_CALL = 39,
	MAP_T_SEND_GROUP_CALL_END_SIGNAL = 40,
	MAP_T_PROCESS_GROUP_CALL_SIGNALLING = 41,
	MAP_T_FORWARD_GROUP_CALL_SIGNALLING = 42,
	MAP_T_CHECK_IMEI = 43,
	MAP_T_MT_FORWARD_SM = 44,
	MAP_T_SEND_ROUTING_INFO_FOR_SM = 45,
	MAP_T_MO_FORWARD_SM = 46,
	MAP_T_REPORT_SM_DELIVERY_STATUS = 47,
	MAP_T_ACTIVATE_TRACE_MODE = 50,
	MAP_T_DEACTIVATE_TRACE_MODE = 51,
	MAP_T_SEND_IDENTIFICATION = 55,
	MAP_T_SEND_AUTHENTICATION_INFO = 56,
	MAP_T_RESTORE_DATA = 57,
	MAP_T_SEND_IMSI = 58,
	MAP_T_PROCESS_UNSTRUCTURED_SS_REQUEST = 59,
	MAP_T_UNSTRUCTURED_SS_REQUEST = 60,
	MAP_T_UNSTRUCTURED_SS_NOTIFY = 61,
	MAP_T_ANY_TIME_SUBSCRIPTION_INTERROGATION = 62,
	MAP_T_INFORM_SERVICE_CENTRE = 63,
	MAP_T_ALERT_SERVICE_CENTRE = 64,
	MAP_T_ANY_TIME_MODIFICATION = 65,
	MAP_T_READY_FOR_SM = 66,
	MAP_T_PURGE_MS = 67,
	MAP_T_PREPARE_HANDOVER = 68,
	MAP_T_PREPARE_SUBSEQUENT_HANDOVER = 69,
	MAP_T_PROVIDE_SUBSCRIBER_INFO = 70,
	MAP_T_ANY_TIME_INTERROGATION = 71,
	MAP_T_SS_INVOCATION_NOTIFICATION = 72,
	MAP_T_SET_REPORTING_STATE = 73,
	MAP_T_STATUS_REPORT = 74,
	MAP_T_REMOTE_USER_FREE = 75,
	MAP_T_REGISTER_CC_ENTRY = 76,
	MAP_T_ERASE_CC_ENTRY = 77,
	MAP_T_PROVIDE_SUBSCRIBER_LOCATION = 83,
	MAP_T_SEND_GROUP_CALL_INFO = 84,
	MAP_T_SEND_ROUTING_INFO_FOR_LCS = 85,
	MAP_T_SUBSCRIBER_LOCATION_REPORT = 86,
	MAP_T_IST_ALERT = 87,
	MAP_T_IST_COMMAND = 88,
	MAP_T_NOTE_MM_EVENT = 89,
} MAP_T_Operation_Code;

#endif				/* __XMAP_GSM_H__ */
