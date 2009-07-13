/*****************************************************************************

 @(#) $RCSfile: xtcap.i,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-07-13 07:13:37 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2009-07-13 07:13:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xtcap.i,v $
 Revision 1.1.2.1  2009-07-13 07:13:37  brian
 - changes for multiple distro build

 Revision 1.1.2.1  2009-06-21 11:36:49  brian
 - added files to new distro

 *****************************************************************************/

%module Xtcap
%{
#include <sys/types.h>
#include <stdint.h>
%}

%include "typemaps.i"
// %include "arrays_java.i"
// %include "various.i"

%apply int *OUTPUT { OM_sint *primitive_return }
%apply int *OUTPUT { TCAP_status *operation_notification_status_return }
//%apply unsigned int *OUTPUT { OM_uint32 *invoke_id_return, OM_uint32 *additional_error_return, OM_value_position *total_number, OM_string_length *string_offset, OM_boolean *instance }
//%apply char **STRING_ARRAY { char **om_strerrors }
//%apply unsigned short[] { OM_type_list }
//%apply unsigned int[] { OM_string_length * }
// %apply char *BYTE { OM_workspace }

typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// %javaconst(1);
// %javaconst(0) TCAP_C_ABORT_ARGUMENT;
// %javaconst(0) TCAP_C_ABORT_RESULT;
// %javaconst(0) TCAP_C_ACCEPT_RESULT;
// %javaconst(0) TCAP_C_ACSE_ARGS;
// %javaconst(0) TCAP_C_ADDRESS;
// %javaconst(0) TCAP_C_APPLICATION_CONTEXT_LIST;
// %javaconst(0) TCAP_C_ASSOC_ARGUMENT;
// %javaconst(0) TCAP_C_ASSOC_RESULT;
// %javaconst(0) TCAP_C_CLOSE_ARGUMENT;
// %javaconst(0) TCAP_C_CONTEXT;
// %javaconst(0) TCAP_C_EXTENSION;
// %javaconst(0) TCAP_C_GENERIC_SERVICE_ARGUMENT;
// %javaconst(0) TCAP_C_GENERIC_SERVICE_RESULT;
// %javaconst(0) TCAP_C_MAP_ASSOC_ARGS;
// %javaconst(0) TCAP_C_NOTICE_RESULT;
// %javaconst(0) TCAP_C_OPEN_ARGUMENT;
// %javaconst(0) TCAP_C_OPERATION_ARGUMENT;
// %javaconst(0) TCAP_C_OPERATION_ERROR;
// %javaconst(0) TCAP_C_OPERATION_REJECT;
// %javaconst(0) TCAP_C_OPERATION_RESULT;
// %javaconst(0) TCAP_C_P_ABORT_RESULT;
// %javaconst(0) TCAP_C_PRESENTATION_CONTEXT;
// %javaconst(0) TCAP_C_PRESENTATION_LAYER_ARGS;
// %javaconst(0) TCAP_C_REFUSE_RESULT;
// %javaconst(0) TCAP_C_RELEASE_ARGUMENT;
// %javaconst(0) TCAP_C_RELEASE_RESULT;
// %javaconst(0) TCAP_C_SERVICE_ARGUMENT;
// %javaconst(0) TCAP_C_SERVICE_ERROR;
// %javaconst(0) TCAP_C_SERVICE_REJECT;
// %javaconst(0) TCAP_C_SERVICE_RESULT;
// %javaconst(0) TCAP_C_SESSION;
// %javaconst(0) TCAP_C_TITLE;
// %javaconst(0) TCAP_C_PROPRIETARY_ARGS;
// %javaconst(0) TCAP_C_BAD_ARGUMENT;
// %javaconst(0) TCAP_C_ERROR;

#define boolean boolean_

%include "xom.h"

#undef boolean

#define OMP_O_TCAP_COMMON_PKG                     "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01"

#define OMP_D_TCAP_C_ABORT_ARGUMENT               "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x69"
#define OMP_D_TCAP_C_ABORT_RESULT                 "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6A"
#define OMP_D_TCAP_C_ACCEPT_RESULT                "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6B"
#define OMP_D_TCAP_C_ACSE_ARGS                    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6C"
#define OMP_D_TCAP_C_ADDRESS                      "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6D"
#define OMP_D_TCAP_C_APPLICATION_CONTEXT_LIST     "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6E"
#define OMP_D_TCAP_C_ASSOC_ARGUMENT               "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6F"
#define OMP_D_TCAP_C_ASSOC_RESULT                 "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x70"
#define OMP_D_TCAP_C_CLOSE_ARGUMENT               "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x71"
#define OMP_D_TCAP_C_CONTEXT                      "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x72"
#define OMP_D_TCAP_C_EXTENSION                    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x73"
#define OMP_D_TCAP_C_GENERIC_SERVICE_ARGUMENT     "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x74"
#define OMP_D_TCAP_C_GENERIC_SERVICE_RESULT       "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x75"
#define OMP_D_TCAP_C_MAP_ASSOC_ARGS               "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x76"
#define OMP_D_TCAP_C_NOTICE_RESULT                "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x77"
#define OMP_D_TCAP_C_OPEN_ARGUMENT                "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x78"
#define OMP_D_TCAP_C_OPERATION_ARGUMENT           "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x79"
#define OMP_D_TCAP_C_OPERATION_ERROR              "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7A"
#define OMP_D_TCAP_C_OPERATION_REJECT             "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7B"
#define OMP_D_TCAP_C_OPERATION_RESULT             "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7C"
#define OMP_D_TCAP_C_P_ABORT_RESULT               "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7D"
#define OMP_D_TCAP_C_PRESENTATION_CONTEXT         "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7E"
#define OMP_D_TCAP_C_PRESENTATION_LAYER_ARGS      "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7F"
#define OMP_D_TCAP_C_REFUSE_RESULT                "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x00"
#define OMP_D_TCAP_C_RELEASE_ARGUMENT             "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x01"
#define OMP_D_TCAP_C_RELEASE_RESULT               "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x02"
#define OMP_D_TCAP_C_SERVICE_ARGUMENT             "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x03"
#define OMP_D_TCAP_C_SERVICE_ERROR                "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x04"
#define OMP_D_TCAP_C_SERVICE_REJECT               "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x05"
#define OMP_D_TCAP_C_SERVICE_RESULT               "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x06"
#define OMP_D_TCAP_C_SESSION                      "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x08"
#define OMP_D_TCAP_C_TITLE                        "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x09"
#define OMP_D_TCAP_C_PROPRIETARY_ARGS             "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x0A"
#define OMP_D_TCAP_C_BAD_ARGUMENT                 "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x0B"
#define OMP_D_TCAP_C_ERROR                        "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x0C"

const OM_string TCAP_C_ABORT_ARGUMENT               = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x69" };
const OM_string TCAP_C_ABORT_RESULT                 = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6A" };
const OM_string TCAP_C_ACCEPT_RESULT                = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6B" };
const OM_string TCAP_C_ACSE_ARGS                    = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6C" };
const OM_string TCAP_C_ADDRESS                      = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6D" };
const OM_string TCAP_C_APPLICATION_CONTEXT_LIST     = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6E" };
const OM_string TCAP_C_ASSOC_ARGUMENT               = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x6F" };
const OM_string TCAP_C_ASSOC_RESULT                 = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x70" };
const OM_string TCAP_C_CLOSE_ARGUMENT               = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x71" };
const OM_string TCAP_C_CONTEXT                      = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x72" };
const OM_string TCAP_C_EXTENSION                    = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x73" };
const OM_string TCAP_C_GENERIC_SERVICE_ARGUMENT     = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x74" };
const OM_string TCAP_C_GENERIC_SERVICE_RESULT       = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x75" };
const OM_string TCAP_C_MAP_ASSOC_ARGS               = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x76" };
const OM_string TCAP_C_NOTICE_RESULT                = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x77" };
const OM_string TCAP_C_OPEN_ARGUMENT                = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x78" };
const OM_string TCAP_C_OPERATION_ARGUMENT           = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x79" };
const OM_string TCAP_C_OPERATION_ERROR              = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7A" };
const OM_string TCAP_C_OPERATION_REJECT             = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7B" };
const OM_string TCAP_C_OPERATION_RESULT             = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7C" };
const OM_string TCAP_C_P_ABORT_RESULT               = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7D" };
const OM_string TCAP_C_PRESENTATION_CONTEXT         = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7E" };
const OM_string TCAP_C_PRESENTATION_LAYER_ARGS      = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x87\x7F" };
const OM_string TCAP_C_REFUSE_RESULT                = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x00" };
const OM_string TCAP_C_RELEASE_ARGUMENT             = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x01" };
const OM_string TCAP_C_RELEASE_RESULT               = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x02" };
const OM_string TCAP_C_SERVICE_ARGUMENT             = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x03" };
const OM_string TCAP_C_SERVICE_ERROR                = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x04" };
const OM_string TCAP_C_SERVICE_REJECT               = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x05" };
const OM_string TCAP_C_SERVICE_RESULT               = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x06" };
const OM_string TCAP_C_SESSION                      = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x08" };
const OM_string TCAP_C_TITLE                        = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x09" };
const OM_string TCAP_C_PROPRIETARY_ARGS             = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x0A" };
const OM_string TCAP_C_BAD_ARGUMENT                 = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x0B" };
const OM_string TCAP_C_ERROR                        = { 13, "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x02\x01\x88\x0C" };

/* Error attributes: */
#define TCAP_A_OM_ATTRIBUTE                       10021
#define TCAP_A_OM_BAD_ARGUMENT                    10022
#define TCAP_A_OM_CLASS                           10023
#define TCAP_A_OM_INDEX                           10024
#define TCAP_A_OM_SUBOBJECT                       10025
#define TCAP_A_PARAMETER                          10026
#define TCAP_A_PROBLEM                            10028

/* The OM syntax names that are defined are listed below. */
#define TCAP_S_TBCD_STRING                        32
#define TCAP_S_SCCP_ADDRESS_STRING                33

/* The OM attribute names that are defined are listed below. */

#define TCAP_APPLICATION_CONTEXT_NAME             11001
#define TCAP_DESTINATION_ADDRESS                  11002
#define TCAP_DESTINNATION_REFERENCE               11003
#define TCAP_ORIGINATING_ADDRESS                  11004
#define TCAP_ORIGINATING_REFERENCE                11005
#define TCAP_SPECIFIC_INFORMATION                 11006
#define TCAP_ASSOCIATION_ID                       11007
#define TCAP_RESPONDING_ADDRESS                   11008
#define TCAP_REFUSE_REASON                        11009
#define TCAP_PROVIDER_ERROR                       11010
#define TCAP_RELEASE_METHOD                       11011
#define TCAP_USER_REASON                          11012
#define TCAP_DIAGNOSTIC_INFORMATION               11013
#define TCAP_PROVIDER_REASON                      11014
#define TCAP_SOURCE                               11015
#define TCAP_PROBLEM_DIAGNOSTIC                   11016

/* common parameters */
#define TCAP_INVOKE_ID                            11017
#define TCAP_LINKED_ID                            11018
#define TCAP_SERVICE_PROVIDER_ERROR               11019
#define TCAP_SERVICE_USER_ERROR                   11020
#define TCAP_ALL_INFORMATION_SENT                 11021

/* numbering and identification parameters */
#define TCAP_ADDITIONAL_NUMBER                    11067   /* XXX */
#define TCAP_ADDITIONAL_V_GMLC_ADDRESS            11074
#define TCAP_APN                                  11063
#define TCAP_B_SUBSCRIBER_NUMBER                  11069
#define TCAP_B_SUBSCRIBER_SUBADDRESS              11060
#define TCAP_CALLED_NUMBER                        11048
#define TCAP_CALLING_NUMBER                       11049
#define TCAP_CURRENT_LOCATION_AREA_ID             11028
#define TCAP_FORWARDED_TO_NUMBER                  11044
#define TCAP_FORWARDED_TO_SUBADDRESS              11047
#define TCAP_GGSN_ADDRESS                         11062
#define TCAP_GMSC_ADDRESS                         11055
#define TCAP_GROUP_ID                             11057
#define TCAP_GSMSCF_ADDRESS                       11069
#define TCAP_HANDOVER_NUMBER                      11043
#define TCAP_H_GMLC_ADDRESS                       11071
#define TCAP_HLR_ID                               11037
#define TCAP_HLR_NUMBER                           11035
#define TCAP_IMEI                                 11024
#define TCAP_IMEISV                               11025
#define TCAP_IMSI                                 11022   /* XXX */
#define TCAP_LMSI                                 11038   /* XXX */
#define TCAP_LMU_NUMBER                           11061
#define TCAP_LOCATION_INFORMATION_FOR_GPRC        11054
#define TCAP_LOCATION_INFORMATION                 11053
#define TCAP_LONG_FORWARDED_TO_NUMBER             11045
#define TCAP_LONG_FTN_SUPPORTED                   11046
#define TCAP_MLC_NUMBER                           11062
#define TCAP_MSC_NUMBER                           11033   /* XXX */
#define TCAP_MSISDN_ALERT                         11052
#define TCAP_MS_ISDN                              11039
#define TCAP_MULTICALL_BEARER_INFORMATION         11063
#define TCAP_MULTIPLE_BEARER_NOT_SUPPORTED        11065
#define TCAP_MULTIPLE_BEARER_REQUESTED            11064
#define TCAP_NETWORK_NODE_NUMBER                  11064   /* XXX */
#define TCAP_NORTH_AMERICAN_EQUAL_ACCESS_PIC      11058
#define TCAP_OMC_ID                               11040
#define TCAP_ORIGINAL_DIALED_NUMBER               11050
#define TCAP_ORIGINATING_ENTITY_NUMBER            11032
#define TCAP_PDP_ADDRESS                          11066
#define TCAP_PDP_CHARGING_CHARACTERISTICS         11066
#define TCAP_PDP_TYPE                             11065
#define TCAP_PPR_ADDRESS                          11072
#define TCAP_PREVIOUS_LOCATION_AREA_ID            11026
#define TCAP_P_TMSI                               11068
#define TCAP_RAB_ID                               11068
#define TCAP_RELOCATION_NUMBER_LIST               11042
#define TCAP_ROAMING_NUMBER                       11041
#define TCAP_ROUTEING_NUMBER                      11073
#define TCAP_SELECTED_RAB_ID                      11067
#define TCAP_SERVICE_CENTRE_ADDRESS               11051   /* XXX */
#define TCAP_SERVING_CELL_ID                      11059
#define TCAP_SGSN_ADDRESS                         11061
#define TCAP_SGSN_NUMBER                          11060   /* XXX */
#define TCAP_STORED_LOCATION_AREA_ID              11027
#define TCAP_TARGET_CELL_ID                       11030
#define TCAP_TARGET_LOCATION_AREA_ID              11029
#define TCAP_TARGET_MSC_NUMBER                    11034
#define TCAP_TARGET_RNC_ID                        11031
#define TCAP_TMSI                                 11023
#define TCAP_V_GMLC_ADDRESS                       11070
#define TCAP_VLR_NUMBER                           11036
#define TCAP_VMSC_ADDRESS                         11056

/*
 * The following enumerations tags and enumeration constants are defined for use
 * as values of the corresponding OM attributes:
 */

/* TCAP_T_Refuse_reason: */

#define TCAP_T_NO_REASON_GIVEN                    0
#define TCAP_T_INVALID_DESTINATION_REFERENCE      1
#define TCAP_T_INVALID_ORIGINATING_REFERENCE      2
#define TCAP_T_APPLICATION_CONTEXT_NOT_SUPPORTED  3
#define TCAP_T_TRANSPORT_PROTECTION_NOT_ADEQUATE  4

#define TCAP_T_REMOTE_NOT_NOT_REACHABLE           5
#define TCAP_T_POTENTIAL_VERSION_INCOMPATIBLITY   6
#define TCAP_T_SECURED_TRANSPORT_NOT_POSSIBLE     7

/* TCAP_T_Release_method: */
#define TCAP_T_NORMAL_RELEASE                     1
#define TCAP_T_PREARRANGED_END                    2

/* map-UserAbortChoice CHOICE { */

/* TCAP_T_User_reason: */
/* userSpecificReason [0] IMPLICIT NULL */
#define TCAP_T_USER_SPECIFIC_REASON               0
/* userResourceLimitation [1] IMPLICIT NULL */
#define TCAP_T_RESOURCE_LIMITATION_CONGESTION     1
/* resourceUnavailable [2] IMPLICIT ENUMERATED */
#define TCAP_T_RESOURCE_UNAVAILABLE               2
/* applicationProcedureCancellation [3] IMPLICIT ENUMERATED */
#define TCAP_T_APPLICATION_PROCEDURE_CANCELLATION 3
#define TCAP_T_PROCEDURE_ERROR                    4

/* TCAP_T_Diagnostic_information: */
/* resourceUnavailable [2] IMPLICIT ENUMERATED */
#define TCAP_T_SHORT_TERM_PROBLEM                 0
#define TCAP_T_LONG_TERM_PROBLEM                  1
/* applicationProcedureCancellation [3] IMPLICIT ENUMERATED */
#define TCAP_T_HANDOVER_CANCELLATION              0
#define TCAP_T_RADIO_CHANNEL_RELEASED             1
#define TCAP_T_NETWORK_PATH_RELEASE               2
#define TCAP_T_CALL_RELEASE                       3
#define TCAP_T_ASSOCIATED_PROCEDURE_FAILURE       4
#define TCAP_T_TANDEM_DIALOGUE_RELEASED           5
#define TCAP_T_REMOTE_OPERATIONS_FAILURE          6

/* TCAP_T_Provider_reason: */
#define TCAP_T_PROVIDER_MALFUNCTION                       1
#define TCAP_T_SUPPORTING_DIALOGUE_TRANSACTION_RELEASED   2
#define TCAP_T_RESOURCE_LIMITATION                        3
#define TCAP_T_MAINTENANCE_ACTIVITY                       4
#define TCAP_T_VERSION_INCOMPATIBILITY                    5
#define TCAP_T_ABNORMAL_MAP_DIALOGUE                      6

/* TCAP_T_Source: */
#define TCAP_T_MAP_PROBLEM                1
#define TCAP_T_TC_PROBLEM                 2
#define TCAP_T_NETWORK_SERVICE_PROBLEM    3

/* TCAP_T_Problem_diagnostic: */
#define TCAP_T_ABNORMAL_EVENT_DETECTED_BY_PEER            1
#define TCAP_T_RESPONSE_REJECTED_BY_THE_PEER              2
#define TCAP_T_ABNORMAL_EVENT_RECEIVED_FROM_THE_PEER      3
#define TCAP_T_MESSAGE_CANNOT_BE_DELIVERED_TO_THE_PEER    4

/* TCAP_T_Abort_Source: */
#define TCAP_T_ABORT_SOURCE_ACSE_SERVICE_USER             1
#define TCAP_T_ABORT_SOURCE_ACSE_SERVICE_PROVIDER 2

/* TCAP_T_Abort_Diagnostic: */
#define TCAP_T_ABORT_DIAGNOSTIC_NO_REASON_GIVEN                                   1
#define TCAP_T_ABORT_DIAGNOSTIC_PROTOCOL_ERROR                                    2
#define TCAP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNIZED      3
#define TCAP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED            4
#define TCAP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_FAILURE                            5
#define TCAP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_REQUIRED                           6

/* TCAP_T_MAP_Abort_Souce: */

/* TCAP_T_ACSE_Service_User: */
#define TCAP_T_ACSE_SERVICE_USER_NULL                                              1
#define TCAP_T_ACSE_SERVICE_USER_NO_REASON_GIVEN                                   2
#define TCAP_T_ACSE_SERVICE_USER_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED            3
#define TCAP_T_ACSE_SERVICE_USER_CALLING_AP_TITLE_NOT_RECOGNIZED                   4
#define TCAP_T_ACSE_SERVICE_USER_CALLING_AP_INVOCATION_IDENTIFIER_NOT_RECOGNIZED   5
#define TCAP_T_ACSE_SERVICE_USER_CALLING_AE_QUALIFIER_NOT_RECOGNIZED               6
#define TCAP_T_ACSE_SERVICE_USER_CALLING_AE_INVOCATION_IDENTIFIER_NOT_RECOGNIZED   7
#define TCAP_T_ACSE_SERVICE_USER_CALLED_AP_TITLE_NOT_RECOGNIZED                    8
#define TCAP_T_ACSE_SERVICE_USER_CALLED_AP_INVOCATION_IDENTIFIER_NOT_RECOGNIZED    9
#define TCAP_T_ACSE_SERVICE_USER_CALLED_AE_QUALIFIER_NOT_RECOGNIZED               10
#define TCAP_T_ACSE_SERVICE_USER_CALLED_AE_INVOCATION_IDENTIFIER_NOT_RECOGNIZED   11
#define TCAP_T_ACSE_SERVICE_USER_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNIZED     12
#define TCAP_T_ACSE_SERVICE_USER_AUTHENTICATION_MECHANISM_NAME_REQUIRED           13
#define TCAP_T_ACSE_SERVICE_USER_AUTHENTICATION_FAILURE                           14
#define TCAP_T_ACSE_SERVICE_USER_AUTHENTICATION_REQUIRED                          15

/* TCAP_T_ACSE_Service_Provider: */
#define TCAP_T_ACSE_SERVICE_PROVIDER_NULL                 1
#define TCAP_T_ACSE_SERVICE_PROVIDER_NO_REASON_GIVEN              2
#define TCAP_T_ACSE_SERVICE_PROVIDER_NO_COMMON_ACSE_VERSION       3

/* TCAP_T_Assoc_Result: */
#define TCAP_T_ACCEPT                     1
#define TCAP_T_REJECT_PERMANENT           2
#define TCAP_T_REJECT_TRANSIENT           3

/* TCAP_T_Asynchronous: */
#define TCAP_T_FALSE                      1
#define TCAP_T_TRUE                       2

/* TCAP_E_Problem: */
#define TCAP_E_BAD_ADDRESS                1001
#define TCAP_E_BAD_ARGUMENT               1002
#define TCAP_E_BAD_CLASS                  1003
#define TCAP_E_BAD_CONTEXT                1004
#define TCAP_E_BAD_ERROR                  1005
#define TCAP_E_BAD_LINKED_REPLY           1006
#define TCAP_E_BAD_PROCEDURAL_USE         1007
#define TCAP_E_BAD_RESULT                 1008
#define TCAP_E_BAD_SESSION                1009
#define TCAP_E_BAD_SYNTAX                 1010
#define TCAP_E_BAD_TITLE                  1011
#define TCAP_E_BAD_VALUE                  1012
#define TCAP_E_BAD_WORKSPACE              1013
#define TCAP_E_BROKEN_SESSION             1014
#define TCAP_E_COMMUNICATIONS_PROBLEM     1015
#define TCAP_E_EXCLUSIVE_ATTRIBUTE        1016
#define TCAP_E_INVALID_CONNECTION_ID      1017
#define TCAP_E_INVALID_ATTRIBUTE_ID       1018
#define TCAP_E_MISCELLANEOUS              1019
#define TCAP_E_MISSING_ATTRIBUTE          1020
#define TCAP_E_NOT_MULTI_VALUED           1021
#define TCAP_E_NOT_SUPPORTED              1022
#define TCAP_E_NO_SUCH_OPERATION          1023
#define TCAP_E_REPLY_LIMIT_EXCEEDED       1024
#define TCAP_E_SESSION_TERMINATED         1025
#define TCAP_E_SYSTEM_ERROR               1026
#define TCAP_E_TIME_LIMIT_EXCEEDED        1027
#define TCAP_E_TOO_MANY_OPERATIONS        1028
#define TCAP_E_TOO_MANY_SESSIONS          1029

/* TCAP_T_Modify_Operator: */
#define TCAP_T_REPLACE            0
#define TCAP_T_ADD_VALUES 1
#define TCAP_T_REMOVE_VALUES      2
#define TCAP_T_SET_TO_DEFAULT     3

/* The typedef name TCAP_status is defined as: */
typedef int TCAP_status;

/* The following constants are defined: */
#define TCAP_SUCCESS                      0
#define TCAP_NO_WORKSPACE                 1
#define TCAP_INVALID_SESSION              2
#define TCAP_INVALID_ASSOCIATION          3
#define TCAP_INVALID_INVOKE_ID            4
#define TCAP_INSUFFICIENT_RESOURCES       5

/* The following constants are defined: */
#define TCAP_ACTIVATE                     0
#define TCAP_DEACTIVATE                   1
#define TCAP_QUERY_STATE                  2
#define TCAP_QUERY_SUPPORTED              3

typedef int TCAP_status;

/* The following constants are defined: */
#define TCAP_SUCCESS                      0
#define TCAP_NO_WORKSPACE                 1
#define TCAP_INVALID_SESSION              2
#define TCAP_INVALID_ASSOCIATION          3
#define TCAP_INVALID_INVOKE_ID            4
#define TCAP_INSUFFICIENT_RESOURCES       5

/* The following constants are defined: */
#define TCAP_ACTIVATE                             0
#define TCAP_DEACTIVATE                           1
#define TCAP_QUERY_STATE                          2
#define TCAP_QUERY_SUPPORTED                      3

typedef struct {
        OM_object_identifier feature;
        OM_sint request;
        OM_boolean response;
} TCAP_feature;

typedef struct {
        OM_private_object bound_session;
        OM_boolean activated;
} TCAP_waiting_sessions;

/* The following constants, of type OM_Object, are defined: */
#define TCAP_ABSENT_OBJECT                0
#define TCAP_DEFAULT_CONTEXT              0
#define TCAP_DEFAULT_SESSION              0
#define TCAP_DEFAULT_ASSOCIATION          0

/* The following integer constants are defined: */
#define TCAP_OPEN_IND                     15
#define TCAP_OPEN_CON                     16
#define TCAP_REFUSE_IND                   17
#define TCAP_SERV_IND                     18
#define TCAP_SERV_CON                     19
#define TCAP_DELIM_IND                    20
#define TCAP_CLOSE_IND                    21
#define TCAP_ABORT_IND                    22
#define TCAP_NOTICE_IND                   23
#define TCAP_STC1_IND                     28
#define TCAP_STC2_IND                     29
#define TCAP_STC3_IND                     30
#define TCAP_STC4_IND                     31

#define TCAP_COMPLETETED                   1
#define TCAP_INCOMING                      2
#define TCAP_NOTHING                       3
#define TCAP_OUTSTANDING                   4
#define TCAP_PARTIAL                       5
#define TCAP_NO_VALID_FILE_DESCRIPTOR     -1

#define TCAP_MAX_OUTSTANDING_OPERATIONS   1024


/*
extern OM_workspace tc_initialize(void);
extern void tc_shutdown(OM_workspace workspace);
extern TCAP_status tc_validate_object(OM_workspace workspace, OM_object test_object, OM_private_object *bad_argument_return);
extern TCAP_status tc_get_last_error(OM_workspace workspace, OM_uint32 *additional_error_return);
extern TCAP_status tc_negotiate(TCAP_feature features[], OM_workspace workspace);
extern TCAP_status tc_wait(TCAP_waiting_sessions sessions[], OM_workspace workspace, OM_uint32 timeout);
extern TCAP_status tc_receive(OM_descriptor entity[], OM_sint *primitive_return, OM_private_object *dialog_return, TCAP_status *operation_notification_status_return, OM_private_object *result_or_argument_return);
extern TCAP_status tc_bind(OM_object entity, OM_workspace workspace, OM_private_object *bound_entity_return);
extern TCAP_status tc_unbind(OM_descriptor entity[]);
extern TCAP_status tc_coord_req(OM_descriptor entity[], OM_private_object *result_return);
extern TCAP_status tc_coord_res(OM_descriptor entity[], OM_descriptor remote[]);
extern TCAP_status tc_state_req(OM_descriptor entity[], OM_object arguments);
extern TCAP_status tc_uni_req(OM_descriptor entity[], OM_object arguments);
extern TCAP_status tc_begin_req(OM_descriptor entity[], OM_descriptor dialog[], OM_object arguments, OM_private_object *result_return);
extern TCAP_status tc_begin_res(OM_descriptor dialog[], OM_object arguments, OM_private_object *result_return);
extern TCAP_status tc_cont_req(OM_descriptor dialog[], OM_object arguments, OM_private_object *result_return);
extern TCAP_status tc_end_res(OM_descriptor dialog[], OM_object arguments);
extern TCAP_status tc_end_req(OM_descriptor dialog[], OM_object arguments);
extern TCAP_status tc_abort_req(OM_descriptor dialog[], OM_object arguments);
extern TCAP_status tc_receive_components(OM_descriptor dialog[], OM_sint *primitive_return, OM_private_object *result_return, OM_uint32 *invoke_id_return);
extern TCAP_status tc_invoke_req(OM_descriptor entity[], OM_object dialog, OM_object arguments, OM_private_object *OUTPUT, OM_uint32 *invoke_id_return);
extern TCAP_status tc_result_req(OM_descriptor dialog[], OM_object arguments, OM_uint32 invoke_id);
extern TCAP_status tc_error_req(OM_descriptor dialog[], OM_object arguments, OM_uint32 invoke_id);
extern TCAP_status tc_cancel_req(OM_descriptor dialog[], OM_object arguments, OM_uint32 invoke_id);
extern TCAP_status tc_reject_req(OM_descriptor dialog[], OM_object arguments, OM_uint32 invoke_id);
*/

extern OM_workspace tc_initialize(void);
extern void tc_shutdown(OM_workspace workspace);
extern TCAP_status tc_validate_object(OM_workspace workspace, OM_object test_object, OM_descriptor * OUTPUT[]);
extern TCAP_status tc_get_last_error(OM_workspace workspace, OM_uint32 *additional_error_return);
extern TCAP_status tc_negotiate(TCAP_feature features[], OM_workspace workspace);
extern TCAP_status tc_wait(TCAP_waiting_sessions sessions[], OM_workspace workspace, OM_uint32 timeout);
extern TCAP_status tc_receive(OM_descriptor entity[], OM_sint *primitive_return, OM_descriptor * OUTPUT[], TCAP_status *operation_notification_status_return, OM_descriptor * OUTPUT[]);
extern TCAP_status tc_bind(OM_object entity, OM_workspace workspace, OM_descriptor * OUTPUT[]);
extern TCAP_status tc_unbind(OM_descriptor entity[]);
extern TCAP_status tc_coord_req(OM_descriptor entity[], OM_descriptor * OUTPUT[]);
extern TCAP_status tc_coord_res(OM_descriptor entity[], OM_descriptor remote[]);
extern TCAP_status tc_state_req(OM_descriptor entity[], OM_object arguments);
extern TCAP_status tc_uni_req(OM_descriptor entity[], OM_object arguments);
extern TCAP_status tc_begin_req(OM_descriptor entity[], OM_descriptor dialog[], OM_object arguments, OM_descriptor * OUTPUT[]);
extern TCAP_status tc_begin_res(OM_descriptor dialog[], OM_object arguments, OM_descriptor * OUTPUT[]);
extern TCAP_status tc_cont_req(OM_descriptor dialog[], OM_object arguments, OM_descriptor * OUTPUT[]);
extern TCAP_status tc_end_res(OM_descriptor dialog[], OM_object arguments);
extern TCAP_status tc_end_req(OM_descriptor dialog[], OM_object arguments);
extern TCAP_status tc_abort_req(OM_descriptor dialog[], OM_object arguments);
extern TCAP_status tc_receive_components(OM_descriptor dialog[], OM_sint *primitive_return, OM_descriptor * OUTPUT[], OM_uint32 *invoke_id_return);
extern TCAP_status tc_invoke_req(OM_descriptor entity[], OM_object dialog, OM_object arguments, OM_descriptor * OUTPUT[], OM_uint32 *invoke_id_return);
extern TCAP_status tc_result_req(OM_descriptor dialog[], OM_object arguments, OM_uint32 invoke_id);
extern TCAP_status tc_error_req(OM_descriptor dialog[], OM_object arguments, OM_uint32 invoke_id);
extern TCAP_status tc_cancel_req(OM_descriptor dialog[], OM_object arguments, OM_uint32 invoke_id);
extern TCAP_status tc_reject_req(OM_descriptor dialog[], OM_object arguments, OM_uint32 invoke_id);

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("openss7XtcapJNI");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}


/*
   vim: ft=c
 */
