/*****************************************************************************

 @(#) $Id: omp_dmi.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/09/29 14:08:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: omp_dmi.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __OMP_DMI_H__
#define __OMP_DMI_H__

#ident "@(#) $RCSfile: omp_dmi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * This secton sets out the symbols which are defined in the <omp_dmi.h>
 * header.  Where the values of the symbols are indicated, the values are an
 * integral part of the interface.  Where a value is not given, the value on a
 * particular system will be determined by the vendor or by an administrator.
 */

/*
 * The <omp_dmi.h> header declares the interface functions, the structures
 * passed to and from those functions, and the defined constants used by the
 * functions and structures.  All application programs which include this
 * header must first include the OSI-Abstract-Data Manipulation header
 * <xom.h>.
 */

#include <xom.h>

/*
 * All Object Identifiers are represented by constants defined in the headers.
 * These constants are used with the macros defined in the XOM API (reference
 * XOM). A constant is defined to represent the Object Identifier of the DMI
 * Management Contents package.
 *
 * Every application program which makes use of a class or other Object
 * Identifier must explicitly import it into every compilation unit (C source
 * program) which uses it. Each such class or Object Identifier name must be
 * explicitly exported from just one compilation unit.
 *
 * In the header file, OM class constants are prefixed with OMP_O_ to denote
 * that they are OM classes. However, when using the OM_IMPORT and OM_EXPORT
 * macros, the base names (without the OMP_O_ prefix) should be used. For
 * example:
 *
 * OM_IMPORT(C_DMI_ALARM_STATUS)
 */

/* DMI Package Definition */
#define OMP_O_SMI_PART2 "\x2A\x86\x3A\x00\x88\x1A\x06\x04"

/* DMI Managed Object OID Definitions */
#define OMP_O_DMI_O_ALARM_RECORD			"\x59\x03\x02\x03\x01"
#define OMP_O_DMI_O_ATTRIBUTE_VALUE_CHANGE_RECORD	"\x59\x03\x02\x03\x02"
#define OMP_O_DMI_O_DISCRIMINATOR			"\x59\x03\x02\x03\x03"
#define OMP_O_DMI_O_EVENT_FORWARDING_DISCRIMINATOR	"\x59\x03\x02\x03\x04"
#define OMP_O_DMI_O_EVENT_LOG_RECORD			"\x59\x03\x02\x03\x05"
#define OMP_O_DMI_O_LOG					"\x59\x03\x02\x03\x06"
#define OMP_O_DMI_O_LOG_RECORD				"\x59\x03\x02\x03\x07"
#define OMP_O_DMI_O_OBJECT_CREATION_RECORD		"\x59\x03\x02\x03\x08"
#define OMP_O_DMI_O_OBJECT_DELETION_RECORD		"\x59\x03\x02\x03\x09"
#define OMP_O_DMI_O_RELATIONSHIP_CHANGE_RECORD		"\x59\x03\x02\x03\x0A"
#define OMP_O_DMI_O_SECURITY_ALARM_REPORT_RECORD	"\x59\x03\x02\x03\x0B"
#define OMP_O_DMI_O_STATE_CHANGE_RECORD			"\x59\x03\x02\x03\x0C"
#define OMP_O_DMI_O_SYSTEM				"\x59\x03\x02\x03\x0D"
#define OMP_O_DMI_O_TOP					"\x59\x03\x02\x03\x0E"

/* DMI Attribute OID Definitions */
#define OMP_O_DMI_A_ACTIVE_DESTINATION			"\x59\x03\x02\x07\x31"
#define OMP_O_DMI_A_ADDITIONAL_INFORMATION		"\x59\x03\x02\x07\x06"
#define OMP_O_DMI_A_ADDITIONAL_TEXT			"\x59\x03\x02\x07\x07"
#define OMP_O_DMI_A_ADMINISTRATIVE_STATE		"\x59\x03\x02\x07\x1F"
#define OMP_O_DMI_A_ALARM_STATUS			"\x59\x03\x02\x07\x20"
#define OMP_O_DMI_A_ALLOMORPHS				"\x59\x03\x02\x07\x32"
#define OMP_O_DMI_A_ATTRIBUTE_IDENTIFIER_LIST		"\x59\x03\x02\x07\x08"
#define OMP_O_DMI_A_ATTRIBUTE_LIST			"\x59\x03\x02\x07\x09"
#define OMP_O_DMI_A_ATTRIBUTE_VALUE_CHANGE_DEFINITION	"\x59\x03\x02\x07\x0A"
#define OMP_O_DMI_A_AVAILABILITY_STATUS			"\x59\x03\x02\x07\x21"
#define OMP_O_DMI_A_BACK_UP_DESTINATION_LIST		"\x59\x03\x02\x07\x33"
#define OMP_O_DMI_A_BACK_UP_OBJECT			"\x59\x03\x02\x07\x28"
#define OMP_O_DMI_A_BACKED_UP_OBJECT			"\x59\x03\x02\x07\x29"
#define OMP_O_DMI_A_BACKED_UP_STATUS			"\x59\x03\x02\x07\x0B"
#define OMP_O_DMI_A_CAPACITY_ALARM_THRESHOLD		"\x59\x03\x02\x07\x34"
#define OMP_O_DMI_A_CONFIRMED_MODE			"\x59\x03\x02\x07\x35"
#define OMP_O_DMI_A_CONTROL_STATUS			"\x59\x03\x02\x07\x22"
#define OMP_O_DMI_A_CORRELATED_NOTIFICATIONS		"\x59\x03\x02\x07\x0C"
#define OMP_O_DMI_A_CORRUPTED_PD_US_RECEIVED_COUNTER	"\x59\x03\x02\x07\x48"
#define OMP_O_DMI_A_CORRUPTED_PD_US_RECEIVED_THRESHOLD	"\x59\x03\x02\x07\x59"
#define OMP_O_DMI_A_CURRENT_LOG_SIZE			"\x59\x03\x02\x07\x36"
#define OMP_O_DMI_A_DESTINATION				"\x59\x03\x02\x07\x37"
#define OMP_O_DMI_A_DISCRIMINATOR_CONSTRUCT		"\x59\x03\x02\x07\x38"
#define OMP_O_DMI_A_DISCRIMINATOR_ID			"\x59\x03\x02\x07\x01"
#define OMP_O_DMI_A_EVENT_TIME				"\x59\x03\x02\x07\x0D"
#define OMP_O_DMI_A_EVENT_TYPE				"\x59\x03\x02\x07\x0E"
#define OMP_O_DMI_A_INCOMING_CONNECTION_REJECT_ERROR_COUNTER \
							"\x59\x03\x02\x07\x49"
#define OMP_O_DMI_A_INCOMING_CONNECTION_REJECT_ERROR_THRESHOLD \
							"\x59\x03\x02\x07\x5A"
#define OMP_O_DMI_A_INCOMING_CONNECTION_REQUESTS_COUNTER \
							"\x59\x03\x02\x07\x4A"
#define OMP_O_DMI_A_INCOMING_CONNECTION_REQUESTS_THRESHOLD \
							"\x59\x03\x02\x07\x5B"
#define OMP_O_DMI_A_INCOMING_DISCONNECT_COUNTER		"\x59\x03\x02\x07\x4B"
#define OMP_O_DMI_A_INCOMING_DISCONNECT_ERROR_COUNTER	"\x59\x03\x02\x07\x4C"
#define OMP_O_DMI_A_INCOMING_DISCONNECT_ERROR_THRESHOLD	"\x59\x03\x02\x07\x5C"
#define OMP_O_DMI_A_INCOMING_PROTOCOL_ERROR_COUNTER	"\x59\x03\x02\x07\x4D"
#define OMP_O_DMI_A_INCOMING_PROTOCOL_ERROR_THRESHOLD	"\x59\x03\x02\x07\x5D"
#define OMP_O_DMI_A_INTERVALS_OF_DAY			"\x59\x03\x02\x07\x39"
#define OMP_O_DMI_A_LOG_FULL_ACTION			"\x59\x03\x02\x07\x3A"
#define OMP_O_DMI_A_LOG_ID				"\x59\x03\x02\x07\x02"
#define OMP_O_DMI_A_LOG_RECORD_ID			"\x59\x03\x02\x07\x03"
#define OMP_O_DMI_A_LOGGING_TIME			"\x59\x03\x02\x07\x3B"
#define OMP_O_DMI_A_MANAGED_OBJECT_CLASS		"\x59\x03\x02\x07\x3C"
#define OMP_O_DMI_A_MANAGED_OBJECT_INSTANCE		"\x59\x03\x02\x07\x3D"
#define OMP_O_DMI_A_MAX_LOG_SIZE			"\x59\x03\x02\x07\x3E"
#define OMP_O_DMI_A_MEMBER				"\x59\x03\x02\x07\x2A"
#define OMP_O_DMI_A_MONITORED_ATTRIBUTES		"\x59\x03\x02\x07\x0F"
#define OMP_O_DMI_A_NAME_BINDING			"\x59\x03\x02\x07\x3F"
#define OMP_O_DMI_A_NOTIFICATION_IDENTIFIER		"\x59\x03\x02\x07\x10"
#define OMP_O_DMI_A_NUMBER_OF_RECORDS			"\x59\x03\x02\x07\x40"
#define OMP_O_DMI_A_OBJECT_CLASS			"\x59\x03\x02\x07\x41"
#define OMP_O_DMI_A_OCTETS_RECEIVED_COUNTER		"\x59\x03\x02\x07\x4E"
#define OMP_O_DMI_A_OCTETS_RECEIVED_THRESHOLD		"\x59\x03\x02\x07\x5E"
#define OMP_O_DMI_A_OCTETS_RETRANSMITTED_ERROR_COUNTER	"\x59\x03\x02\x07\x4F"
#define OMP_O_DMI_A_OCTETS_RETRANSMITTED_THRESHOLD	"\x59\x03\x02\x07\x5F"
#define OMP_O_DMI_A_OCTETS_SENT_COUNTER			"\x59\x03\x02\x07\x50"
#define OMP_O_DMI_A_OCTETS_SENT_THRESHOLD		"\x59\x03\x02\x07\x60"
#define OMP_O_DMI_A_OPERATIONAL_STATE			"\x59\x03\x02\x07\x23"
#define OMP_O_DMI_A_OUTGOING_CONNECTION_REJECT_ERROR_COUNTER \
							"\x59\x03\x02\x07\x51"
#define OMP_O_DMI_A_OUTGOING_CONNECTION_REJECT_ERROR_THRESHOLD \
							"\x59\x03\x02\x07\x61"
#define OMP_O_DMI_A_OUTGOING_CONNECTION_REQUESTS_COUNTER \
							"\x59\x03\x02\x07\x52"
#define OMP_O_DMI_A_OUTGOING_CONNECTION_REQUESTS_THRESHOLD \
							"\x59\x03\x02\x07\x62"
#define OMP_O_DMI_A_OUTGOING_DISCONNECT_COUNTER		"\x59\x03\x02\x07\x53"
#define OMP_O_DMI_A_OUTGOING_DISCONNECT_ERROR_COUNTER	"\x59\x03\x02\x07\x54"
#define OMP_O_DMI_A_OUTGOING_DISCONNECT_ERROR_THRESHOLD	"\x59\x03\x02\x07\x63"
#define OMP_O_DMI_A_OUTGOING_PROTOCOL_ERROR_COUNTER	"\x59\x03\x02\x07\x55"
#define OMP_O_DMI_A_OUTGOING_PROTOCOL_ERROR_THRESHOLD	"\x59\x03\x02\x07\x64"
#define OMP_O_DMI_A_OWNER				"\x59\x03\x02\x07\x2B"
#define OMP_O_DMI_A_PACKAGES				"\x59\x03\x02\x07\x42"
#define OMP_O_DMI_A_PDUS_RECEIVED_COUNTER		"\x59\x03\x02\x07\x56"
#define OMP_O_DMI_A_PDUS_RECEIVED_THRESHOLD		"\x59\x03\x02\x07\x65"
#define OMP_O_DMI_A_PDUS_RETRANSMITTED_ERROR_COUNTER	"\x59\x03\x02\x07\x57"
#define OMP_O_DMI_A_PDUS_RETRANSMITTED_ERROR_THRESHOLD	"\x59\x03\x02\x07\x66"
#define OMP_O_DMI_A_PDUS_SENT_COUNTER			"\x59\x03\x02\x07\x58"
#define OMP_O_DMI_A_PDUS_SENT_THRESHOLD			"\x59\x03\x02\x07\x67"
#define OMP_O_DMI_A_PEER				"\x59\x03\x02\x07\x2C"
#define OMP_O_DMI_A_PERCEIVED_SEVERITY			"\x59\x03\x02\x07\x11"
#define OMP_O_DMI_A_PRIMARY				"\x59\x03\x02\x07\x2D"
#define OMP_O_DMI_A_PROBABLE_CAUSE			"\x59\x03\x02\x07\x12"
#define OMP_O_DMI_A_PROCEDURAL_STATUS			"\x59\x03\x02\x07\x24"
#define OMP_O_DMI_A_PROPOSED_REPAIR_ACTIONS		"\x59\x03\x02\x07\x13"
#define OMP_O_DMI_A_PROVIDER_OBJECT			"\x59\x03\x02\x07\x2E"
#define OMP_O_DMI_A_RELATIONSHIP_CHANGE_DEFINITION	"\x59\x03\x02\x07\x14"
#define OMP_O_DMI_A_SCHEDULER_NAME			"\x59\x03\x02\x07\x43"
#define OMP_O_DMI_A_SECONDARY				"\x59\x03\x02\x07\x2F"
#define OMP_O_DMI_A_SECURITY_ALARM_CAUSE		"\x59\x03\x02\x07\x15"
#define OMP_O_DMI_A_SECURITY_ALARM_DETECTOR		"\x59\x03\x02\x07\x16"
#define OMP_O_DMI_A_SECURITY_ALARM_SEVERITY		"\x59\x03\x02\x07\x17"
#define OMP_O_DMI_A_SERVICE_PROVIDER			"\x59\x03\x02\x07\x18"
#define OMP_O_DMI_A_SERVICE_USER			"\x59\x03\x02\x07\x19"
#define OMP_O_DMI_A_SOURCE_INDICATOR			"\x59\x03\x02\x07\x1A"
#define OMP_O_DMI_A_SPECIFIC_PROBLEMS			"\x59\x03\x02\x07\x1B"
#define OMP_O_DMI_A_STANDBY_STATUS			"\x59\x03\x02\x07\x25"
#define OMP_O_DMI_A_START_TIME				"\x59\x03\x02\x07\x44"
#define OMP_O_DMI_A_STATE_CHANGE_DEFINITION		"\x59\x03\x02\x07\x1C"
#define OMP_O_DMI_A_STOP_TIME				"\x59\x03\x02\x07\x45"
#define OMP_O_DMI_A_SUPPORTED_FEATURES			"\x59\x03\x02\x07\x46"
#define OMP_O_DMI_A_SYSTEM_ID				"\x59\x03\x02\x07\x04"
#define OMP_O_DMI_A_SYSTEM_TITLE			"\x59\x03\x02\x07\x05"
#define OMP_O_DMI_A_THRESHOLD_INFO			"\x59\x03\x02\x07\x1D"
#define OMP_O_DMI_A_TREND_INDICATION			"\x59\x03\x02\x07\x1E"
#define OMP_O_DMI_A_UNKNOWN_STATUS			"\x59\x03\x02\x07\x26"
#define OMP_O_DMI_A_USAGE_STATE				"\x59\x03\x02\x07\x27"
#define OMP_O_DMI_A_USER_OBJECT				"\x59\x03\x02\x07\x30"
#define OMP_O_DMI_A_WEEK_MASK				"\x59\x03\x02\x07\x47"

/* DMI Attribute Group OID Definitions */
#define OMP_O_DMI_A_RELATIONSHIPS			"\x59\x03\x02\x08\x02"
#define OMP_O_DMI_A_STATE				"\x59\x03\x02\x08\x01"

/* DMI Notification OID Definitions */
#define OMP_O_DMI_N_ATTRIBUTE_VALUE_CHANGE		"\x59\x03\x02\x0A\x01"
#define OMP_O_DMI_N_COMMUNICATIONS_ALARM		"\x59\x03\x02\x0A\x02"
#define OMP_O_DMI_N_ENVIRONMENTAL_ALARM			"\x59\x03\x02\x0A\x03"
#define OMP_O_DMI_N_EQUIPMENT_ALARM			"\x59\x03\x02\x0A\x04"
#define OMP_O_DMI_N_INTEGRITY_VIOLATION			"\x59\x03\x02\x0A\x05"
#define OMP_O_DMI_N_OBJECT_CREATION			"\x59\x03\x02\x0A\x06"
#define OMP_O_DMI_N_OBJECT_DELETION			"\x59\x03\x02\x0A\x07"
#define OMP_O_DMI_N_OPERATIONAL_VIOLATION		"\x59\x03\x02\x0A\x08"
#define OMP_O_DMI_N_PHYSICAL_VIOLATION			"\x59\x03\x02\x0A\x09"
#define OMP_O_DMI_N_PROCESSING_ERROR_ALARM		"\x59\x03\x02\x0A\x0A"
#define OMP_O_DMI_N_QUALITYOF_SERVICE_ALARM		"\x59\x03\x02\x0A\x0B"
#define OMP_O_DMI_N_RELATIONSHIP_CHANGE			"\x59\x03\x02\x0A\x0C"
#define OMP_O_DMI_N_SECURITY_SERVICE_OR_MECHANISM_VIOLATION \
							"\x59\x03\x02\x0A\x0D"
#define OMP_O_DMI_N_STATE_CHANGE			"\x59\x03\x02\x0A\x0E"
#define OMP_O_DMI_N_TIME_DOMAIN_VIOLATION		"\x59\x03\x02\x0A\x0F"

/* DMI Parameter OID Definitions */
#define OMP_O_DMI_S_MISCELLANEOUS_ERROR			"\x59\x03\x02\x05\x01"

/* Intermediate object identifier macro */
#define mpP_dmi(X) (OMP_O_SMI_PART2##X)

/* OM Class Object Identifiers */
#define OMP_O_C_DMI_ADDITIONAL_INFORMATION		mpP_dmi(\xD8\x25)
#define OMP_O_C_DMI_ALARM_INFO				mpP_dmi(\xD8\x26)
#define OMP_O_C_DMI_ALARM_STATUS			mpP_dmi(\xD8\x27)
#define OMP_O_C_DMI_ALLOMORPHS				mpP_dmi(\xD8\x28)
#define OMP_O_C_DMI_ATTRIBUTE_IDENTIFIER_LIST		mpP_dmi(\xD8\x29)
#define OMP_O_C_DMI_ATTRIBUTE_LIST			mpP_dmi(\xD8\x2A)
#define OMP_O_C_DMI_ATTRIBUTE_VALUE_CHANGE_DEFINITION	mpP_dmi(\xD8\x2B)
#define OMP_O_C_DMI_ATTRIBUTE_VALUE_CHANGE_INFO		mpP_dmi(\xD8\x2C)
#define OMP_O_C_DMI_AVAILABILITY_STATUS			mpP_dmi(\xD8\x2D)
#define OMP_O_C_DMI_BACK_UP_DESTINATION_LIST		mpP_dmi(\xD8\x2E)
#define OMP_O_C_DMI_BACK_UP_RELATIONSHIP_OBJECT		mpP_dmi(\xD8\x2F)
#define OMP_O_C_DMI_CAPACITY_ALARM_THRESHOLD		mpP_dmi(\xD8\x30)
#define OMP_O_C_DMI_CONTROL_STATUS			mpP_dmi(\xD8\x31)
#define OMP_O_C_DMI_CORRELATED_NOTIFICATIONS		mpP_dmi(\xD8\x32)
#define OMP_O_C_DMI_CORRELATED_NOTIFICATIONS_1		mpP_dmi(\xD8\x33)
#define OMP_O_C_DMI_COUNTER_THRESHOLD			mpP_dmi(\xD8\x34)
#define OMP_O_C_DMI_DESTINATION				mpP_dmi(\xD8\x35)
#define OMP_O_C_DMI_DOWN				mpP_dmi(\xD8\x36)
#define OMP_O_C_DMI_GAUGE_THRESHOLD			mpP_dmi(\xD8\x37)
#define OMP_O_C_DMI_GROUP_OBJECTS			mpP_dmi(\xD8\x38)
#define OMP_O_C_DMI_INTERVALS_OF_DAY			mpP_dmi(\xD8\x39)
#define OMP_O_C_DMI_MANAGEMENT_EXTENSION		mpP_dmi(\xD8\x3A)
#define OMP_O_C_DMI_MONITORED_ATTRIBUTES		mpP_dmi(\xD8\x3B)
#define OMP_O_C_DMI_MULTIPLE				mpP_dmi(\xD8\x3C)
#define OMP_O_C_DMI_NOTIFY_THRESHOLD			mpP_dmi(\xD8\x3D)
#define OMP_O_C_DMI_OBJECT_INFO				mpP_dmi(\xD8\x3E)
#define OMP_O_C_DMI_OBSERVED_VALUE			mpP_dmi(\xD8\x3F)
#define OMP_O_C_DMI_PACKAGES				mpP_dmi(\xD8\x40)
#define OMP_O_C_DMI_PRIORITISED_OBJECT			mpP_dmi(\xD8\x41)
#define OMP_O_C_DMI_PROBABLE_CAUSE			mpP_dmi(\xD8\x42)
#define OMP_O_C_DMI_PROCEDURAL_STATUS			mpP_dmi(\xD8\x43)
#define OMP_O_C_DMI_PROPOSED_REPAIR_ACTIONS		mpP_dmi(\xD8\x44)
#define OMP_O_C_DMI_RELATIONSHIP_CHANGE_INFO		mpP_dmi(\xD8\x45)
#define OMP_O_C_DMI_SECURITY_ALARM_DETECTOR		mpP_dmi(\xD8\x46)
#define OMP_O_C_DMI_SECURITY_ALARM_INFO			mpP_dmi(\xD8\x47)
#define OMP_O_C_DMI_SERVICE_USER			mpP_dmi(\xD8\x48)
#define OMP_O_C_DMI_SETOF_ATTRIBUTE_VALUE_CHANGE_DEFINITION \
							mpP_dmi(\xD8\x49)
#define OMP_O_C_DMI_SETOF_CORRELATED_NOTIFICATIONS	mpP_dmi(\xD8\x4A)
#define OMP_O_C_DMI_SETOF_COUNTER_THRESHOLD		mpP_dmi(\xD8\x4B)
#define OMP_O_C_DMI_SETOF_GAUGE_THRESHOLD		mpP_dmi(\xD8\x4C)
#define OMP_O_C_DMI_SETOF_INTERVALS_OF_DAY		mpP_dmi(\xD8\x4D)
#define OMP_O_C_DMI_SETOF_PRIORITISED_OBJECT		mpP_dmi(\xD8\x4E)
#define OMP_O_C_DMI_SETOF_SUPPORTED_FEATURES		mpP_dmi(\xD8\x4F)
#define OMP_O_C_DMI_SETOF_WEEK_MASK			mpP_dmi(\xD8\x50)
#define OMP_O_C_DMI_SIMPLE_NAME_TYPE			mpP_dmi(\xD8\x51)
#define OMP_O_C_DMI_SPECIFIC_IDENTIFIER			mpP_dmi(\xD8\x52)
#define OMP_O_C_DMI_SPECIFIC_PROBLEMS			mpP_dmi(\xD8\x53)
#define OMP_O_C_DMI_STATE_CHANGE_INFO			mpP_dmi(\xD8\x54)
#define OMP_O_C_DMI_STOP_TIME				mpP_dmi(\xD8\x55)
#define OMP_O_C_DMI_SUPPORTED_FEATURES			mpP_dmi(\xD8\x56)
#define OMP_O_C_DMI_SYSTEM_ID				mpP_dmi(\xD8\x57)
#define OMP_O_C_DMI_SYSTEM_TITLE			mpP_dmi(\xD8\x58)
#define OMP_O_C_DMI_THRESHOLD_INFO			mpP_dmi(\xD8\x59)
#define OMP_O_C_DMI_THRESHOLD_LEVEL_IND			mpP_dmi(\xD8\x5A)
#define OMP_O_C_DMI_TIDE_MARK				mpP_dmi(\xD8\x5B)
#define OMP_O_C_DMI_TIDE_MARK_INFO			mpP_dmi(\xD8\x5C)
#define OMP_O_C_DMI_TIME24				mpP_dmi(\xD8\x5D)
#define OMP_O_C_DMI_UP					mpP_dmi(\xD8\x5E)
#define OMP_O_C_DMI_WEEK_MASK				mpP_dmi(\xD8\x5F)

/* Attribute Name Constants */
#define DMI_ADDITIONAL_INFORMATION			((OM_type)11301)
#define DMI_ADDITIONAL_TEXT				((OM_type)11302)
#define DMI_A_E_TITLE					((OM_type)11303)
#define DMI_ALARM_STATUS				((OM_type)11304)
#define DMI_APPLICATION					((OM_type)11305)
#define DMI_ARM_TIME					((OM_type)11306)
#define DMI_ATTRIBUTE					((OM_type)11307)
#define DMI_ATTRIBUTE_ID				((OM_type)11308)
#define DMI_ATTRIBUTE_IDENTIFIER_LIST			((OM_type)11309)
#define DMI_ATTRIBUTE_LIST				((OM_type)11310)
#define DMI_ATTRIBUTE_VALUE_CHANGE_DEFINITION		((OM_type)11311)
#define DMI_AVAILABILITY_STATUS				((OM_type)11312)
#define DMI_BACKED_UP_STATUS				((OM_type)11313)
#define DMI_BACK_UP_OBJECT				((OM_type)11314)
#define DMI_CAPACITY_ALARM_THRESHOLD			((OM_type)11315)
#define DMI_COMPARISON_LEVEL				((OM_type)11316)
#define DMI_CONTINUAL					((OM_type)11317)
#define DMI_CONTROL_STATUS				((OM_type)11318)
#define DMI_CORRELATED_NOTIFICATIONS			((OM_type)11319)
#define DMI_COUNTER_THRESHOLD				((OM_type)11320)
#define DMI_CURRENT_TIDE_MARK				((OM_type)11321)
#define DMI_DAYS_OF_WEEK				((OM_type)11322)
#define DMI_DETAILS					((OM_type)11323)
#define DMI_DISTINGUISHED_NAME				((OM_type)11324)
#define DMI_DOWN					((OM_type)11325)
#define DMI_FEATURE_IDENTIFIER				((OM_type)11326)
#define DMI_FEATURE_INFO				((OM_type)11327)
#define DMI_GAUGE_THRESHOLD				((OM_type)11328)
#define DMI_GLOBAL_VALUE				((OM_type)11329)
#define DMI_HIGH					((OM_type)11330)
#define DMI_HOUR					((OM_type)11331)
#define DMI_IDENTIFIER					((OM_type)11332)
#define DMI_INFORMATION					((OM_type)11333)
#define DMI_INTEGER					((OM_type)11334)
#define DMI_INTERVALS_OF_DAY				((OM_type)11335)
#define DMI_INTERVAL_END				((OM_type)11336)
#define DMI_INTERVAL_START				((OM_type)11337)
#define DMI_LOCAL_VALUE					((OM_type)11338)
#define DMI_LOW						((OM_type)11339)
#define DMI_MANAGEMENT_EXTENSION			((OM_type)11340)
#define DMI_MAX_TIDE_MARK				((OM_type)11341)
#define DMI_MECHANISM					((OM_type)11342)
#define DMI_MIN_TIDE_MARK				((OM_type)11343)
#define DMI_MINUTE					((OM_type)11344)
#define DMI_MONITORED_ATTRIBUTES			((OM_type)11345)
#define DMI_MULTIPLE					((OM_type)11346)
#define DMI_NAME					((OM_type)11347)
#define DMI_NEW_ATTRIBUTE_VALUE				((OM_type)11348)
#define DMI_NO_OBJECT					((OM_type)11349)
#define DMI_NOTHING					((OM_type)11350)
#define DMI_NOTIFICATION_IDENTIFIER			((OM_type)11351)
#define DMI_NOTIFICATION_ON_OFF				((OM_type)11352)
#define DMI_NOTIFY_HIGH					((OM_type)11353)
#define DMI_NOTIFY_LOW					((OM_type)11354)
#define DMI_NOTIFY_ON_OFF				((OM_type)11355)
#define DMI_NUMBER					((OM_type)11356)
#define DMI_OBJECT					((OM_type)11357)
#define DMI_OBJECT_CLASS				((OM_type)11358)
#define DMI_OBJECT_INSTANCE				((OM_type)11359)
#define DMI_OBJECT_NAME					((OM_type)11360)
#define DMI_OBSERVED_VALUE				((OM_type)11361)
#define DMI_OFFSET_VALUE				((OM_type)11362)
#define DMI_OID						((OM_type)11363)
#define DMI_OLD_ATTRIBUTE_VALUE				((OM_type)11364)
#define DMI_PACKAGES					((OM_type)11365)
#define DMI_PERCEIVED_SEVERITY				((OM_type)11366)
#define DMI_PREVIOUS_TIDE_MARK				((OM_type)11367)
#define DMI_PRIORITISED_OBJECT				((OM_type)11368)
#define DMI_PRIORITY					((OM_type)11369)
#define DMI_PROBABLE_CAUSE				((OM_type)11370)
#define DMI_PROCEDURAL_STATUS				((OM_type)11371)
#define DMI_PROPOSED_REPAIR_ACTIONS			((OM_type)11372)
#define DMI_REAL					((OM_type)11373)
#define DMI_RELATIONSHIP_CHANGE_DEFINITION		((OM_type)11374)
#define DMI_RESET_TIME					((OM_type)11375)
#define DMI_SECURITY_ALARM_CAUSE			((OM_type)11376)
#define DMI_SECURITY_ALARM_DETECTOR			((OM_type)11377)
#define DMI_SECURITY_ALARM_SEVERITY			((OM_type)11378)
#define DMI_SERVICE_PROVIDER				((OM_type)11379)
#define DMI_SERVICE_USER				((OM_type)11380)
#define DMI_SIGNIFICANCE				((OM_type)11381)
#define DMI_SINGLE					((OM_type)11382)
#define DMI_SOURCE_INDICATOR				((OM_type)11383)
#define DMI_SOURCE_OBJECT_INST				((OM_type)11384)
#define DMI_SPECIFIC					((OM_type)11385)
#define DMI_SPECIFIC_IDEN6IFIER				((OM_type)11386)
#define DMI_SPECIFIC_IDENTIFIER_INTEGER_2		((OM_type)11387)
#define DMI_SPECIFIC_IDENTIFIER_OBJECT_IDENTIFIER_1	((OM_type)11388)
#define DMI_SPECIFIC_PROBLEMS				((OM_type)11389)
#define DMI_STATE_CHANGE_DEFINITION			((OM_type)11390)
#define DMI_STRING					((OM_type)11391)
#define DMI_SUPPORTED_FEATURES				((OM_type)11392)
#define DMI_THRESHOLD					((OM_type)11393)
#define DMI_THRESHOLD_INFO				((OM_type)11394)
#define DMI_THRESHOLD_LEVEL				((OM_type)11395)
#define DMI_TREND_INDICATION				((OM_type)11396)
#define DMI_TRIGGERED_THRESHOLD				((OM_type)11397)
#define DMI_UP						((OM_type)11398)
#define DMI_WEEK_MASK					((OM_type)11399)

/* Enumerations */

/* enum(AdministrativeState) */
#define DMI_ADMINISTRATIVE_STATE_LOCKED			0
#define DMI_ADMINISTRATIVE_STATE_UNLOCKED		1
#define DMI_ADMINISTRATIVE_STATE_SHUTTING_DOWN		2

/* enum(LogFullAction) */
#define DMI_LOG_FULL_ACTION_WRAP			0
#define DMI_LOG_FULL_ACTION_HALT			1

/* enum(OperationalState) */
#define DMI_OPERATIONAL_STATE_DISABLED			0
#define DMI_OPERATIONAL_STATE_ENABLED			1

/* enum(PerceivedSeverity) */
#define DMI_PERCEIVED_SEVERITY_INDETERMINATE		0
#define DMI_PERCEIVED_SEVERITY_CRITICAL			1
#define DMI_PERCEIVED_SEVERITY_MAJOR			2
#define DMI_PERCEIVED_SEVERITY_MINOR			3
#define DMI_PERCEIVED_SEVERITY_WARNING			4
#define DMI_PERCEIVED_SEVERITY_CLEARED			5

/* enum(SourceIndicator) */
#define DMI_SOURCE_INDICATOR_RESOURCE_OPERATION		0
#define DMI_SOURCE_INDICATOR_MANAGEMENT_OPERATION	1
#define DMI_SOURCE_INDICATOR_UNKNOWN			2

/* enum(TrendIndication) */
#define DMI_TREND_INDICATION_LESS_SEVERE		0
#define DMI_TREND_INDICATION_NO_CHANGE			1
#define DMI_TREND_INDICATION_MORE_SEVERE		2

/* enum(UsageState) */
#define DMI_USAGE_STATE_IDLE				0
#define DMI_USAGE_STATE_ACTIVE				1
#define DMI_USAGE_STATE_BUSY				2

/* Integer Lists */

/* integer(priority) */
#define DMI_PRIORITY_LOWEST				0
#define DMI_PRIORITY_HIGHEST				127

/* integer(alarm-Status) */
#define DMI_ALARM_STATUS_UNDER_REPAIR			0
#define DMI_ALARM_STATUS_CRITICAL			1
#define DMI_ALARM_STATUS_MAJOR				2
#define DMI_ALARM_STATUS_MINOR				3
#define DMI_ALARM_STATUS_ALARM_OUTSTANDING		4

/* integer(availability-Status) */
#define DMI_AVAILABILITY_STATUS_IN_TEST			0
#define DMI_AVAILABILITY_STATUS_FAILED			1
#define DMI_AVAILABILITY_STATUS_POWER_OFF		2
#define DMI_AVAILABILITY_STATUS_OFF_LINE		3
#define DMI_AVAILABILITY_STATUS_OFF_DUTY		4
#define DMI_AVAILABILITY_STATUS_DEPENDENCY		5
#define DMI_AVAILABILITY_STATUS_DEGRADED		6
#define DMI_AVAILABILITY_STATUS_NOT_INSTALLED		7
#define DMI_AVAILABILITY_STATUS_LOG_FULL		8

/* integer(control-Status) */
#define DMI_CONTROL_STATUS_SUBJECT_TO_TEST		0
#define DMI_CONTROL_STATUS_PART_OF_SERVICES_LOCKED	1
#define DMI_CONTROL_STATUS_RESERVED_FOR_TEST		2
#define DMI_CONTROL_STATUS_SUSPENDED			3

/* integer(max-Log-Size) */
#define DMI_MAX_LOG_SIZE_UNLIMITED			0

/* integer(procedural-Status)*/
#define DMI_PROCEDURAL_STATUS_INITIALIZATION_REQUIRED	0
#define DMI_PROCEDURAL_STATUS_NOT_INITIALIZED		1
#define DMI_PROCEDURAL_STATUS_INITIALIZING		2
#define DMI_PROCEDURAL_STATUS_REPORTING			3
#define DMI_PROCEDURAL_STATUS_TERMINATING		4

/* integer(standby-Status) */
#define DMI_STANDBY_STATUS_HOT_STANDBY			0
#define DMI_STANDBY_STATUS_COLD_STANDBY			1
#define DMI_STANDBY_STATUS_PROVIDING_SERVICE		2

/* bitstring(daysOfWeek) */
#define DMI_DAYS_OF_WEEK_SUNDAY				0
#define DMI_DAYS_OF_WEEK_MONDAY				1
#define DMI_DAYS_OF_WEEK_TUESDAY			2
#define DMI_DAYS_OF_WEEK_WEDNESDAY			3
#define DMI_DAYS_OF_WEEK_THURSDAY			4
#define DMI_DAYS_OF_WEEK_FRIDAY				5
#define DMI_DAYS_OF_WEEK_SATURDAY			6

/* objectIdentifier(probable-Cause) */
#define DMI_PROBABLE_CAUSE_ADAPTER_ERROR		"\x59\x03\x02\x00\x00\x01"
#define DMI_PROBABLE_CAUSE_APPLICATION_SUBSYSTEM_FAILURE \
							"\x59\x03\x02\x00\x00\x02"
#define DMI_PROBABLE_CAUSE_BANDWIDTH_REDUCED		"\x59\x03\x02\x00\x00\x03"
#define DMI_PROBABLE_CAUSE_CALL_ESTABLISHMENT_ERROR	"\x59\x03\x02\x00\x00\x04"
#define DMI_PROBABLE_CAUSE_COMMUNICATIONS_PROTOCOL_ERROR \
							"\x59\x03\x02\x00\x00\x05"
#define DMI_PROBABLE_CAUSE_COMMUNICATIONS_SUBSYSTEM_FAILURE \
							"\x59\x03\x02\x00\x00\x06"
#define DMI_PROBABLE_CAUSE_CONFIGURATION_OR_CUSTOMIZATION_ERROR \
							"\x59\x03\x02\x00\x00\x07"
#define DMI_PROBABLE_CAUSE_CONGESTION			"\x59\x03\x02\x00\x00\x08"
#define DMI_PROBABLE_CAUSE_CORRUPT_DATA			"\x59\x03\x02\x00\x00\x09"
#define DMI_PROBABLE_CAUSE_CPU_CYCLES_LIMIT_EXCEEDED	"\x59\x03\x02\x00\x00\x0A"
#define DMI_PROBABLE_CAUSE_DATA_SET_OR_MODEM_ERROR	"\x59\x03\x02\x00\x00\x0B"
#define DMI_PROBABLE_CAUSE_DEGRADED_SIGNAL		"\x59\x03\x02\x00\x00\x0C"
#define DMI_PROBABLE_CAUSE_DTE_DCE_INTERFACE_ERROR	"\x59\x03\x02\x00\x00\x0D"
#define DMI_PROBABLE_CAUSE_ENCLOSURE_DOOR_OPEN		"\x59\x03\x02\x00\x00\x0E"
#define DMI_PROBABLE_CAUSE_EQUIPMENT_MALFUNCTION	"\x59\x03\x02\x00\x00\x0F"
#define DMI_PROBABLE_CAUSE_EXCESSIVE_VIBRATION		"\x59\x03\x02\x00\x00\x10"
#define DMI_PROBABLE_CAUSE_FILE_ERROR			"\x59\x03\x02\x00\x00\x11"
#define DMI_PROBABLE_CAUSE_FIRE_DETECTED		"\x59\x03\x02\x00\x00\x12"
#define DMI_PROBABLE_CAUSE_FLOOD_DETECTED		"\x59\x03\x02\x00\x00\x13"
#define DMI_PROBABLE_CAUSE_FRAMING_ERROR		"\x59\x03\x02\x00\x00\x14"
#define DMI_PROBABLE_CAUSE_HEATING_OR_VENTILATION_OR_COOLING_SYSTEM_PROBLEM \
							"\x59\x03\x02\x00\x00\x15"
#define DMI_PROBABLE_CAUSE_HUMIDITY_UNACCEPTABLE	"\x59\x03\x02\x00\x00\x16"
#define DMI_PROBABLE_CAUSE_INPUT_OUTPUT_DEVICE_ERROR	"\x59\x03\x02\x00\x00\x17"
#define DMI_PROBABLE_CAUSE_INPUT_DEVICE_ERROR		"\x59\x03\x02\x00\x00\x18"
#define DMI_PROBABLE_CAUSE_LAN_ERROR			"\x59\x03\x02\x00\x00\x19"
#define DMI_PROBABLE_CAUSE_LEAK_DETECTED		"\x59\x03\x02\x00\x00\x1A"
#define DMI_PROBABLE_CAUSE_LOCAL_NODE_TRANSMISSION_ERROR \
							"\x59\x03\x02\x00\x00\x1B"
#define DMI_PROBABLE_CAUSE_LOSS_OF_FRAME		"\x59\x03\x02\x00\x00\x1C"
#define DMI_PROBABLE_CAUSE_LOSS_OF_SIGNAL		"\x59\x03\x02\x00\x00\x1D"
#define DMI_PROBABLE_CAUSE_MATERIAL_SUPPLY_EXHAUSTED	"\x59\x03\x02\x00\x00\x1E"
#define DMI_PROBABLE_CAUSE_MULTIPLEXER_PROBLEM		"\x59\x03\x02\x00\x00\x1F"
#define DMI_PROBABLE_CAUSE_OUT_OF_MEMORY		"\x59\x03\x02\x00\x00\x20"
#define DMI_PROBABLE_CAUSE_OUTPUT_DEVICE_ERROR		"\x59\x03\x02\x00\x00\x21"
#define DMI_PROBABLE_CAUSE_PERFORMANCE_DEGRADED		"\x59\x03\x02\x00\x00\x22"
#define DMI_PROBABLE_CAUSE_POWER_PROBLEM		"\x59\x03\x02\x00\x00\x23"
#define DMI_PROBABLE_CAUSE_PRESSURE_UNACCEPTABLE	"\x59\x03\x02\x00\x00\x24"
#define DMI_PROBABLE_CAUSE_PROCESSOR_PROBLEM		"\x59\x03\x02\x00\x00\x25"
#define DMI_PROBABLE_CAUSE_PUMP_FAILURE			"\x59\x03\x02\x00\x00\x26"
#define DMI_PROBABLE_CAUSE_QUEUE_SIZE_EXCEEDED		"\x59\x03\x02\x00\x00\x27"
#define DMI_PROBABLE_CAUSE_RECEIVE_FAILURE		"\x59\x03\x02\x00\x00\x28"
#define DMI_PROBABLE_CAUSE_RECEIVER_FAILURE		"\x59\x03\x02\x00\x00\x29"
#define DMI_PROBABLE_CAUSE_REMOTE_NODE_TRANSMISSION_ERROR \
							"\x59\x03\x02\x00\x00\x2A"
#define DMI_PROBABLE_CAUSE_RESOURCE_AT_OR_NEARING_CAPACITY \
							"\x59\x03\x02\x00\x00\x2B"
#define DMI_PROBABLE_CAUSE_RESPONSE_TIME_EXCESSIVE	"\x59\x03\x02\x00\x00\x2C"
#define DMI_PROBABLE_CAUSE_RETRANSMISSION_RATE_EXCESSIVE \
							"\x59\x03\x02\x00\x00\x2D"
#define DMI_PROBABLE_CAUSE_SOFTWARE_ERROR		"\x59\x03\x02\x00\x00\x2E"
#define DMI_PROBABLE_CAUSE_SOFTWARE_PROGRAM_ABNORMALLY_TERMINATED \
							"\x59\x03\x02\x00\x00\x2F"
#define DMI_PROBABLE_CAUSE_SOFTWARE_PROGRAM_ERROR	"\x59\x03\x02\x00\x00\x30"
#define DMI_PROBABLE_CAUSE_STORAGE_CAPACITY_PROBLEM	"\x59\x03\x02\x00\x00\x31"
#define DMI_PROBABLE_CAUSE_TEMPERATURE_UNACCEPTABLE	"\x59\x03\x02\x00\x00\x32"
#define DMI_PROBABLE_CAUSE_THRESHOLD_CROSSED		"\x59\x03\x02\x00\x00\x33"
#define DMI_PROBABLE_CAUSE_TIMING_PROBLEM		"\x59\x03\x02\x00\x00\x34"
#define DMI_PROBABLE_CAUSE_TOXIC_LEAK_DETECTED		"\x59\x03\x02\x00\x00\x35"
#define DMI_PROBABLE_CAUSE_TRANSMIT_FAILURE		"\x59\x03\x02\x00\x00\x36"
#define DMI_PROBABLE_CAUSE_TRANSMITTER_FAILURE		"\x59\x03\x02\x00\x00\x37"
#define DMI_PROBABLE_CAUSE_UNDERLYING_RESOURCE_UNAVAILABLE \
							"\x59\x03\x02\x00\x00\x38"
#define DMI_PROBABLE_CAUSE_VERSION_MISMATCH		"\x59\x03\x02\x00\x00\x39"

/* objectIdentifier(security-Alarm-Cause) */
#define DMI_SECURITY_ALARM_CAUSE_AUTHENTICATION_FAILURE	"\x59\x03\x02\x00\x01\x01"
#define DMI_SECURITY_ALARM_CAUSE_BREACH_OF_CONFIDENTIALITY \
							"\x59\x03\x02\x00\x01\x02"
#define DMI_SECURITY_ALARM_CAUSE_CABLE_TAMPER		"\x59\x03\x02\x00\x01\x03"
#define DMI_SECURITY_ALARM_CAUSE_DELAYED_INFORMATION	"\x59\x03\x02\x00\x01\x04"
#define DMI_SECURITY_ALARM_CAUSE_DENIAL_OF_SERVICE	"\x59\x03\x02\x00\x01\x05"
#define DMI_SECURITY_ALARM_CAUSE_DUPLICATE_INFORMATION	"\x59\x03\x02\x00\x01\x06"
#define DMI_SECURITY_ALARM_CAUSE_INFORMATION_MISSING	"\x59\x03\x02\x00\x01\x07"
#define DMI_SECURITY_ALARM_CAUSE_INFORMATION_MODIFICATION_DETECTED \
							"\x59\x03\x02\x00\x01\x08"
#define DMI_SECURITY_ALARM_CAUSE_INFORMATION_OUT_OF_SEQUENCE \
							"\x59\x03\x02\x00\x01\x09"
#define DMI_SECURITY_ALARM_CAUSE_INTRUSION_DETECTION	"\x59\x03\x02\x00\x01\x0A"
#define DMI_SECURITY_ALARM_CAUSE_KEY_EXPIRED		"\x59\x03\x02\x00\x01\x0B"
#define DMI_SECURITY_ALARM_CAUSE_NON_REPUDIATION_FAILURE \
							"\x59\x03\x02\x00\x01\x0C"
#define DMI_SECURITY_ALARM_CAUSE_OUT_OF_HOURS_ACTIVITY	"\x59\x03\x02\x00\x01\x0D"
#define DMI_SECURITY_ALARM_CAUSE_OUT_OF_SERVICE		"\x59\x03\x02\x00\x01\x0E"
#define DMI_SECURITY_ALARM_CAUSE_PROCEDURAL_ERROR	"\x59\x03\x02\x00\x01\x0F"
#define DMI_SECURITY_ALARM_CAUSE_UNAUTHORIZED_ACCESS_ATTEMPTED \
							"\x59\x03\x02\x00\x01\x10"
#define DMI_SECURITY_ALARM_CAUSE_UNEXPECTED_INFORMATION	"\x59\x03\x02\x00\x01\x11"
#define DMI_SECURITY_ALARM_CAUSE_UNSPECIFIED_REASON	"\x59\x03\x02\x00\x01\x12"

#endif				/* __OMP_DMI_H__ */
