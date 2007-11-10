/*****************************************************************************

 @(#) $Id: xmp.h,v 0.9.2.2 2007/11/10 19:40:52 brian Exp $

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

 Last Modified $Date: 2007/11/10 19:40:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmp.h,v $
 Revision 0.9.2.2  2007/11/10 19:40:52  brian
 - documentation updates

 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XMP_H__
#define __XMP_H__

#ident "@(#) $RCSfile: xmp.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xmp.h> header declares the interface functions, the structures passed
 * to and from those functions, and the defined constants used by the
 * functions and structures.
 * 
 * All application programs which include this header must first include the
 * OSI-Abstract-Data Manipulation header <xom.h>.
 *
 * All Object Identifiers are represented by constants defined in the headers.
 * These constants are used with the macros defined in the XOM API (see
 * reference XOM). A constant is defined to represent the Object Identifier of
 * the Common Management Service package:
 *
 * { iso(1) member-national-body(2) bsi(826) disc(0) xopen(1050) xmp-cae(6) common(1) }
 */
#define OMP_O_MP_COMMON_PKG "\x2a\x86\x3a\x00\x88\x1a\x06\x01" /* 1.2.826.0.1050.6.1 */

/* 
 * Constants are also defined to represent the Object Identifiers for Features
 * defined by this specification:
 *
 * { iso(1) member-national-body(2) bsi(826) disc(0) xopen(1050) xmp-cae(6) common(1)
 *   automatic-connection-management(1) }
 */
#define OMP_O_MP_AUTOMATIC_CONNECTION_MANAGEMENT "\x2a\x86\x3a\x00\x88\x1a\x06\x01\x01" /* 1.2.826.0.1050.6.1.1 */
/*
 * { iso(1) member-national-body(2) bsi(826) disc(0) xopen(1050) xmp-cae(6) common(1)
 *   automatic-decoding(2) }
 */
#define OMP_O_MP_AUTOMATIC_DECODING "\x2a\x86\x3a\x00\x88\x1a\x06\x01\x02" /* 1.2.826.0.1050.6.1.2 */

/*
 * Solaris Solstice CMIP 8.2 extensions:
 */
/* If this implementation specific feature is enabled, each session has its own file descriptor.  If
 * this implementation specific feature is disabled, all sessions share the same file descriptor.
 * In either case, the file descriptor is set in the MP_FILE_DESCRIPTOR attribute of the session
 * object.
 *
 * { iso(1) member-national-body(2) bsi(826) disc(0) xopen(1050) xmp-cae(6) common(1)
 *   on-fd-per-session(3) }
 */
#define OMP_O_MP_ONE_FD_PER_SESSION "\x2a\x86\x3a\x00\x88\x1a\x06\x01\x03" /* 1.2.826.0.1050.6.1.3 */

/* If this implementation specific feature is enabled, then any application context can be specified
 * in the MP_APPLICATION_CONTEXT attribute of the MP_ACSE_ARGS attirbute object of the session
 * object.  For outgoing associations, the application context is sent in the associate request PDU.
 * Only associate request PDUs that contain a valid application context are accepted by incoming
 * associations.  By default, only the ISO, NMF and TMN application contexts can be specified in the
 * sesssion object.
 *
 * { iso(1) member-national-body(2) bsi(826) disc(0) xopen(1050) xmp-cae(6) common(1)
 *   any-appl-context(4) }
 */
#define OMP_O_MP_ANY_APP_CONTEXT "\x2a\x86\x3a\x00\x88\x1a\x06\x01\x04" /* 1.2.826.0.1050.6.1.4 */

/*
 * Prototypes are defined for the following functions:
 * mp_abandon( ),
 * mp_abort_req( ),
 * mp_action_req( ),
 * mp_action_rsp( ),
 * mp_assoc_req( ),
 * mp_assoc_rsp( ),
 * mp_bind( ),
 * mp_cancel_get_req( ),
 * mp_cancel_get_rsp( ),
 * mp_create_req( ),
 * mp_create_rsp( ),
 * mp_delete_req( ),
 * mp_delete_rsp( ),
 * mp_error_message(),
 * mp_event_report_req( ),
 * mp_event_report_rsp( ),
 * mp_get_assoc_info( ),
 * mp_get_last_error( ),
 * mp_get_next( ),
 * mp_get_req( ),
 * mp_get_rsp( ),
 * mp_initialize( ),
 * mp_negotiate( ). 
 * mp_receive( ),
 * mp_release_req(),
 * mp_release_rsp(),
 * mp_set_req( ),
 * mp_set_rsp( ),
 * mp_shutdown( ),
 * mp_unbind( ),
 * mp_validate_object( ),
 * mp_wait( )
 */

/* Defined constants */

/* Intermediate object identifier macro */
#define mpP_comn(X) (OMP_O_MP_COMMON_PKG# #X)

/* OM class names (prefixed MP_C_) */
/*
 * Every application program which makes use of a class or other Object Identifier must explicitly
 * import it into every compilation unit (C source program) which uses it. Each such class or Object
 * Identifier name must be explicitly exported from just one compilation unit.
 *
 * In the header file, OM class constants are prefixed with the OPM_O prefix to denote that they are
 * OM classes. However, when using the OM_IMPORT and OM_EXPORT macros, the base names (without the
 * OMP_O prefix) should be used.  For example:
 */

/* *INDENT-OFF* */
OM_IMPORT(MP_C_AVA);
/* *INDENT-ON* */

#define OMP_O_MP_C_ABORT_ARGUMENT		mpP_comn(\x87\x69)  /* 1001 */
#define OMP_O_MP_C_ACCESS_CONTROL		mpP_comn(\x87\x6A)  /* 1002 */
#define OMP_O_MP_C_ACSE_ARGS			mpP_comn(\x87\x6B)  /* 1003 */
#define OMP_O_MP_C_ADDRESS			mpP_comn(\x87\x6C)  /* 1004 */
#define OMP_O_MP_C_AE_TITLE			mpP_comn(\x87\x6D)  /* 1005 */
#define OMP_O_MP_C_ASSOC_ARGUMENT		mpP_comn(\x87\x6E)  /* 1006 */
#define OMP_O_MP_C_ASSOC_DIAGNOSTIC		mpP_comn(\x87\x6F)  /* 1007 */
#define OMP_O_MP_C_ASSOCIATION_INFORMATION	mpP_comn(\x87\x70)  /* 1008 */
#define OMP_O_MP_C_ASSOC_RESULT			mpP_comn(\x87\x71)  /* 1009 */
#define OMP_O_MP_C_AUTHENTICATION_INFORMATION	mpP_comn(\x87\x72)  /* 1010 */
#define OMP_O_MP_C_AUTHENTICATION_OTHER		mpP_comn(\x87\x73)  /* 1011 */
#define OMP_O_MP_C_AVA				mpP_comn(\x87\x74)  /* 1012 */
#define OMP_O_MP_C_BAD_ARGUMENT			mpP_comn(\x87\x75)  /* 1013 */
#define OMP_O_MP_C_CMIP_ASSOC_ARGS		mpP_comn(\x87\x76)  /* 1014 */
#define OMP_O_MP_C_COMMUNITY_NAME		mpP_comn(\x87\x77)  /* 1015 */
#define OMP_O_MP_C_CONTEXT			mpP_comn(\x87\x78)  /* 1016 */
#define OMP_O_MP_C_DS_DN			mpP_comn(\x87\x79)  /* 1017 */
#define OMP_O_MP_C_DS_RDN			mpP_comn(\x87\x7A)  /* 1018 */
#define OMP_O_MP_C_ENTITY_NAME			mpP_comn(\x87\x7B)  /* 1019 */
#define OMP_O_MP_C_ERROR			mpP_comn(\x87\x7C)  /* 1020 */
#define OMP_O_MP_C_EXTENSION			mpP_comn(\x87\x7D)  /* 1021 */
#define OMP_O_MP_C_EXTERNAL_AC			mpP_comn(\x87\x7E)  /* 1022 */
#define OMP_O_MP_C_FORM1			mpP_comn(\x87\x7F)  /* 1023 */
#define OMP_O_MP_C_FORM2			mpP_comn(\x88\x00)  /* 1024 */
#define OMP_O_MP_C_FUNCTIONAL_UNIT_PACKAGE	mpP_comn(\x88\x01)  /* 1025 */
#define OMP_O_MP_C_NAME				mpP_comn(\x88\x02)  /* 1026 */
#define OMP_O_MP_C_NAME_STRING			mpP_comn(\x88\x03)  /* 1027 */
#define OMP_O_MP_C_NETWORK_ADDRESS		mpP_comn(\x88\x04)  /* 1028 */
#define OMP_O_MP_C_PRESENTATION_ADDRESS		mpP_comn(\x88\x05)  /* 1029 */
#define OMP_O_MP_C_PRESENTATION_CONTEXT		mpP_comn(\x88\x06)  /* 1030 */
#define OMP_O_MP_C_PRESENTATION_LAYER_ARGS	mpP_comn(\x88\x07)  /* 1031 */
#define OMP_O_MP_C_RELATIVE_NAME		mpP_comn(\x88\x08)  /* 1032 */
#define OMP_O_MP_C_RELEASE_ARGUMENT		mpP_comn(\x88\x09)  /* 1033 */
#define OMP_O_MP_C_RELEASE_RESULT		mpP_comn(\x88\x0A)  /* 1034 */
#define OMP_O_MP_C_SESSION			mpP_comn(\x88\x0B)  /* 1035 */
#define OMP_O_MP_C_SMASE_USER_DATA		mpP_comn(\x88\x0C)  /* 1036 */
#define OMP_O_MP_C_SNMP_OBJECT_NAME		mpP_comn(\x88\x0D)  /* 1037 */
#define OMP_O_MP_C_STANDARD_EXTERNALS		mpP_comn(\x88\x0E)  /* 1038 */
#define OMP_O_MP_C_TITLE			mpP_comn(\x88\x0F)  /* 1039 */

#define OMP_O_MP_C_PROPRIETARY_ARGS		mpP_comn(\x88\x10)  /* 1040 */

/* The OM attribute names which are defined are listed below. */
#define MP_ABORT_DIAGNOSTIC			((OM_type)11001)
#define MP_ABORT_SOURCE				((OM_type)11002)
#define MP_ACCESS_CONTROL			((OM_type)11003)
#define MP_ACSE_ARGS				((OM_type)11004)
#define MP_ACSE_ASSOC_ARGS			((OM_type)11005)
#define MP_ACSE_SERVICE_PROVIDER		((OM_type)11006)
#define MP_ACSE_SERVICE_USER			((OM_type)11007)
#define MP_AE_INVOCATION			((OM_type)11008)
#define MP_AE_QUALIFIER_FORM1			((OM_type)11009)
#define MP_AE_QUALIFIER_FORM2			((OM_type)11010)
#define MP_AE_TITLE_FORM1			((OM_type)11011)
#define MP_AE_TITLE_FORM2			((OM_type)11012)
#define MP_AGENT_ROLE_FUNCTIONAL_UNIT		((OM_type)11013)
#define MP_APPLICATION_CONTEXT			((OM_type)11014)
#define MP_AP_INVOCATION			((OM_type)11015)
#define MP_AP_TITLE_FORM1			((OM_type)11016)
#define MP_AP_TITLE_FORM2			((OM_type)11017)
#define MP_ASSOC_EXTERN				((OM_type)11018)
#define MP_ASYNCHRONOUS				((OM_type)11019)
#define MP_AUTHENTICATION_INFORMATION		((OM_type)11020)
#define MP_AVAS					((OM_type)11021)
#define MP_BITSTRING				((OM_type)11022)
#define MP_CHARSTRING				((OM_type)11023)
#define MP_CMIP_ABORT_SOURCE			((OM_type)11024)
#define MP_CMIP_ASSOC_ARGS			((OM_type)11025)
#define MP_CMIP_USER_INFORMATION		((OM_type)11026)
#define MP_CMIS_FUNCTIONAL_UNITS		((OM_type)11027)
#define MP_COMMUNITY				((OM_type)11028)
#define MP_ENTITY				((OM_type)11029)
#define MP_EXTENSIONS				((OM_type)11030)
#define MP_EXTERNAL				((OM_type)11031)
#define MP_EXTERNAL_AC				((OM_type)11032)
#define MP_FILE_DESCRIPTOR			((OM_type)11033)
#define MP_FUNCTIONAL_UNIT_PACKAGE_ID		((OM_type)11034)
#define MP_IDENTIFIER				((OM_type)11035)
#define MP_INFORMATION				((OM_type)11036)
#define MP_IP_ADDRESS				((OM_type)11037)
#define MP_MANAGER_ROLE_FUNCTIONAL_UNIT		((OM_type)11038)
#define MP_MODE					((OM_type)11039)
#define MP_NAME_STRING				((OM_type)11040)
#define MP_NAMING_ATTRIBUTE_ID			((OM_type)11041)
#define MP_NAMING_ATTRIBUTE_VALUE		((OM_type)11042)
#define MP_N_ADDRESSES				((OM_type)11043)
#define MP_OBJECT_NAME				((OM_type)11044)
#define MP_OTHER				((OM_type)11045)
#define MP_OTHER_MECHANISM_NAME			((OM_type)11046)
#define MP_OTHER_MECHANISM_VALUE		((OM_type)11047)
#define MP_PRESENTATION_ABSTRACT		((OM_type)11048)
#define MP_PRESENTATION_CONTEXT_LIST		((OM_type)11049)
#define MP_PRESENTATION_ID			((OM_type)11050)
#define MP_PRESENTATION_LAYER_ARGS		((OM_type)11051)
#define MP_PRIORITY				((OM_type)11052)
#define MP_P_SELECTOR				((OM_type)11053)
#define MP_RDNS					((OM_type)11054)
#define MP_REASON				((OM_type)11055)
#define MP_REPLY_LIMIT				((OM_type)11056)
#define MP_REQUESTOR_ADDRESS			((OM_type)11057)
#define MP_REQUESTOR_TITLE			((OM_type)11058)
#define MP_RESPONDER_ADDRESS			((OM_type)11059)
#define MP_RESPONDER_TITLE			((OM_type)11060)
#define MP_ROLE					((OM_type)11061)
#define MP_SIGNIFICANCE				((OM_type)11062)
#define MP_SMASE_USER_DATA			((OM_type)11063)
#define MP_SMFU_PACKAGES			((OM_type)11064)
#define MP_STANDARD_EXTERNALS			((OM_type)11065)
#define MP_SYSTEMS_MANAGEMENT_USER_INFORMATION	((OM_type)11066)
#define MP_S_SELECTOR				((OM_type)11067)
#define MP_TIME_LIMIT				((OM_type)11068)
#define MP_T_SELECTOR				((OM_type)11069)
#define MP_USER_INFO				((OM_type)11070)
#define MP_USER_INFORMATION			((OM_type)11071)

/*
 * Solstice CMIP 8.2 extensions:
 */
#define MP_PROPRIETARY_ARGS			((OM_type)11072)
#define MP_INACTIVIY_TIMER			((OM_type)11073)
#define MP_BIND_STATE				((OM_type)11074)
#define MP_CONNECT_STATE			((OM_type)11075)

#define MP_T_UNBOUND				0
#define MP_T_BOUND				1

#define MP_T_UNCONNECTED			0
#define MP_T_PARTLY_CONNECTED			1
#define MP_T_CONNECTED				2

#define MP_T_SINGLE_ASSOC			0x80

/* 
 * The following enumeration tags and enumeration constants are defined for use as values of the
 * corresponding OM attributes:
 */

/* MP_T_CMIS_Functional_Units: */
#define MP_T_FU_CANCEL_GET			(1<<0)
#define MP_T_FU_FILTER				(1<<1)
#define MP_T_FU_MULTIPLE_OBJECT_SELECTION	(1<<2)
#define MP_T_FU_UNITS_EXTENDED_SERVICE		(1<<3)
#define MP_T_FU_MULTIPLE_REPLY			(1<<4)

/* MP_T_Request-Mask: */
#define MP_T_PRESENTATION_CONTEXT_LIST		(1<<0)
#define MP_T_RESPONDER_ADDRESS			(1<<1)
#define MP_T_RESPONDER_TITLE			(1<<2)
#define MP_T_APPLICATION_CONTEXT		(1<<3)
#define MP_T_AUTHENTICATION_INFORMATION		(1<<4)
#define MP_T_ACSE_USER_INFO			(1<<5)
#define MP_T_CMIS_FUNCTIONAL_UNITS		(1<<6)
#define MP_T_ACCESS_CONTROL			(1<<7)
#define MP_T_USER_INFO				(1<<8)
#define MP_T_SMASE_USER_DATA			(1<<9)

/* MP_T_Abort_Source: */
#define MP_T_ABORT_SOURCE_ACSE_SERVICE_USER	1
#define MP_T_ABORT_SOURCE_ACSE_SERVICE_PROVIDER	2

/* MP_T_Abort_Diagnostic: */
#define MP_T_ABORT_DIAGNOSTIC_NO_REASON_GIVEN					1
#define MP_T_ABORT_DIAGNOSTIC_PROTOCOL_ERROR					2
#define MP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNIZED	3
#define MP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED		4
#define MP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_FAILURE				5
#define MP_T_ABORT_DIAGNOSTIC_AUTHENTICATION_REQUIRED				6

/* MP_T_CMIP_Abort_Source: */
#define MP_T_CMIP_ABORT_SOURCE_CMISE_SERVICE_PROVIDER	1
#define MP_T_CMIP_ABORT_SOURCE_CMISE_SERVICE_PROVIDER	2

/* MP_T_ACSE_Service_User: */
#define MP_T_ACSE_SERVICE_USER_NULL						 1
#define MP_T_ACSE_SERVICE_USER_NO_REASON_GIVEN					 2
#define MT_T_ACSE_SERVICE_USER_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED		 3
#define MT_T_ACSE_SERVICE_USER_CALLING_AP_TITLE_NOT_RECOGNIZED			 4
#define MT_T_ACSE_SERVICE_USER_CALLING_AP_INVOCATION_IDENTIFIER_NOT_RECOGNIZED	 5
#define MT_T_ACSE_SERVICE_USER_CALLING_AE_QUALIFIER_NOT_RECOGNIZED		 6
#define MT_T_ACSE_SERVICE_USER_CALLING_AE_INVOCATION_IDENTIFIER_NOT_RECOGNIZED	 7
#define MT_T_ACSE_SERVICE_USER_CALLED_AP_TITLE_NOT_RECOGNIZED			 8
#define MT_T_ACSE_SERVICE_USER_CALLED_AP_INVOCATION_IDENTIFIER_NOT_RECOGNIZED	 9
#define MT_T_ACSE_SERVICE_USER_CALLED_AE_QUALIFIER_NOT_RECOGNIZED		10
#define MT_T_ACSE_SERVICE_USER_CALLED_AE_INVOCATION_IDENTIFIER_NOT_RECOGNIZED	11
#define MT_T_ACSE_SERVICE_USER_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNIZED	12
#define MT_T_ACSE_SERVICE_USER_AUTHENTICATION_MECHANISM_NAME_REQUIRED		13
#define MT_T_ACSE_SERVICE_USER_AUTHENTICATION_FAILURE				14
#define MT_T_ACSE_SERVICE_USER_AUTHENTICATION_REQUIRED				15

/* MP_T_ACSE_Service_Provider: */
#define MP_T_ACSE_SERVICE_PROVIDER_NULL				1
#define MP_T_ACSE_SERVICE_PROVIDER_NO_REASON_GIVEN		2
#define MT_T_ACSE_SERVICE_PROVIDER_NO_COMMON_ACSE_VERSION	3

/* MP_T_Assoc_Result: */
#define MP_T_ACCEPT				1
#define MP_T_REJECT_PERMANENT			2
#define MT_T_REJECT_TRANSIENT			3

/* MP_T_Mode: */
#define MP_T_CONFIRMED				1
#define MP_T_NON_CONFIRMED			2

/* MP_T_Priority: */
#define MP_T_LOW				1
#define MP_T_MEDIUM				2
#define MP_T_HIGH				3

/* MP_T_Asynchronous: */
#define MP_T_FALSE				1
#define MP_T_TRUE				2

/* MP_T_Reason: */
#define MP_T_NORMAL				1
#define MP_T_URGENT				2
#define MP_T_USER_DEFINED			3
#define MP_T_NOT_FINISHED			4

/* MP_E_Problem: */
#define MP_E_BAD_ADDRESS			1001
#define MP_E_BAD_ARGUMENT			1002
#define MP_E_BAD_CLASS				1003
#define MP_E_BAD_CONTEXT			1004
#define MP_E_BAD_ERROR				1005
#define MP_E_BAD_LINKED_REPLY			1006
#define MP_E_BAD_PROCEDURAL_USE			1007
#define MP_E_BAD_RESULT				1008
#define MP_E_BAD_SESSION			1009
#define MP_E_BAD_SYNTAX				1010
#define MP_E_BAD_TITLE				1011
#define MP_E_BAD_VALUE				1012
#define MP_E_BAD_WORKSPACE			1013
#define MP_E_BROKEN_SESSION			1014
#define MP_E_COMMUNICATIONS_PROBLEM		1015
#define MP_E_EXCLUSIVE_ATTRIBUTE		1016
#define MP_E_INVALID_CONNECTION_ID		1017
#define MP_E_INVALID_ATTRIBUTE_ID		1018
#define MP_E_MISCELLANEOUS			1019
#define MP_E_MISSING_ATTRIBUTE			1020
#define MP_E_NOT_MULTI_VALUED			1021
#define MP_E_NOT_SUPPORTED			1022
#define MP_E_NO_SUCH_OPERATION			1023
#define MP_E_REPLY_LIMIT_EXCEEDED		1024
#define MP_E_SESSION_TERMINATED			1025
#define MP_E_SYSTEM_ERROR			1026
#define MP_E_TIME_LIMIT_EXCEEDED		1027
#define MP_E_TOO_MANY_OPERATIONS		1028
#define MP_E_TOO_MANY_SESSIONS			1029

/* MP_T_Role: */
#define MP_T_MANAGING				(1<<0)
#define MP_T_MONITORING				(1<<1)
#define MP_T_PERFORMING				(1<<2)
#define MP_T_REPORTING				(1<<3)

/* The typedef name MP_status is defined as: */
typedef int MP_status;

/* The following constants are defined: */
#define MP_SUCCESS				((MP_status)0)
#define MP_NO_WORKSPACE				((MP_status)1)
#define MP_INVALID_SESSION			((MP_status)2)
#define MP_INSUFFICIENT_RESOURCES		((MP_status)3)

/* The following constants are defined: */
#define MP_ACTIVATE				0
#define MP_DEACTIVATE				1
#define MP_QUERY_STATE				2
#define MP_QUERY_SUPPORTED			3

/* The following structures are defined: */
typedef struct {
	OM_object -identifier feature;
	OM_sint request;
	OM_boolean response;
} MP_feature;

typedef struct {
	OM_private_object bound_session;
	OM_boolean activated;
} MP_waiting_sessions;

/* The following constants, of type OM_Object, are defined: */
#define MP_ABSENT_OBJECT			((OM_object)0)
#define MP_DEFAULT_CONTEXT			((OM_object)0)
#define MP_DEFAULT_SESSION			((OM_object)0)

/* The following integer constants are defined: */
#define MP_ABORT_IND				16
#define MP_ACTION_CNF				 1
#define MP_ACTION_IND				 2
#define MP_ASSOC_CNF				17
#define MP_ASSOC_IND				18
#define MP_CANCEL_GET_CNF			 3
#define MP_CANCEL_GET_IND			 4
#define MP_CREATE_CNF				 5
#define MP_CREATE_IND				 6
#define MP_DELETE_CNF				 7
#define MP_DELETE_IND				 8
#define MP_EVENT_REPORT_CNF			 9
#define MP_EVENT_REPORT_IND			10
#define MP_GET_CNF				11
#define MP_GET_IND				12
#define MP_GET_NEXT_IND				13
#define MP_RELEASE_CNF				19
#define MP_RELEASE_IND				20
#define MP_SET_CNF				14
#define MP_SET_IND				15
#define MP_COMPLETED				 1
#define MP_INCOMING				 2
#define MP_NOTHING				 3
#define MP_OUTSTANDING				 4
#define MP_PARTIAL				 5
#define MP_NO_VALID_FILE_DESCRIPTOR		-1

#define MP_MAX_OUTSTANDING_OPERATIONS		1024	/* implementation-defined */

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/* The following function prototypes are defined: */
extern MP_status mp_abandon(OM_private_object session, OM_sint32 invoke_id);

extern MP_status mp_abort_req(OM_private_object session, OM_private_object context,
			      OM_object argument);

extern MP_status mp_action_req(OM_private_object session, OM_private_object context,
			       OM_object argument, OM_private_object *result_return,
			       OM_sint32 *invoke_id_return);

extern MP_status mp_action_rsp(OM_private_object session, OM_private_object context,
			       OM_object response, OM_sint32 invoke_id);

extern MP_status mp_assoc_req(OM_private_object session, OM_private_object context,
			      OM_object argument, OM_private_object *result_return,
			      OM_sint32 *invoke_id_return);

extern MP_status mp_assoc_rsp(OM_private_object session, OM_private_object context,
			      OM_object response, OM_sint32 invoke_id);

extern MP_status mp_bind(OM_object session, OM_workspace workspace,
			 OM_private_object *bound_session_return);

extern MP_status mp_cancel_get_req(OM_private_object session, OM_private_object context,
				   OM_object argument, OM_sint32 *invoke_id_return);

extern MP_status mp_cancel_get_rsp(OM_private_object session, OM_private_object context,
				   OM_object response, OM_sint32 invoke_id);

extern MP_status mp_create_req(OM_private_object session, OM_private_object context,
			       OM_object argument, OM_private_object *result_return,
			       OM_sint32 *invoke_id_return);

extern MP_status mp_create_rsp(OM_private_object session, OM_private_object context,
			       OM_object response, OM_sint32 invoke_id);

extern MP_status mp_delete_req(OM_private_object session, OM_private_object context,
			       OM_object argument, OM_private_objt * result_return,
			       OM_sint32 *invoke_id_return);

extern MP_status mp_delete_rsp(OM_private_object session, OM_private_object context,
			       OM_object response, OM_sint32 invoke_id);

extern OM_sint mp_error_message(MP_status error, OM_sint length, unsigned char *error_text_return);

extern MP_status mp_event_report_req(OM_private_object session, OM_private_object context,
				     OM_object argument, OM_private_object *result_return,
				     OM_sint32 *invoke_id_return);

extern MP_status mp_event_report_rsp(OM_private_object session, OM_private_object context,
				     OM_object response, OM_sint32 invoke_id);

extern MP_status mp_get_assoc_info(OM_private_object receive_result_or_argument,
				   OM_uint request_mask, OM_uint result_mask,
				   OM_public_object *pres_layer_args, OM_public_object *acse_args,
				   OM_public_object *cmip_assoc_args,
				   OM_public_object *standard_externals);

extern MP_status mp_get_last_error(OM_workspace workspace, OM_uint32 *additional_error_return);

extern MP_status mp_get_next_req(OM_private_object session, OM_private_object context,
				 OM_object argument, OM_private_object *result_return,
				 OM_sint32 *invoke_id_return);

extern MP_status mp_get_req(OM_private_object session, OM_private_object context,
			    OM_object argument, OM_private_object *result_return,
			    OM_sint32 *invoke_id_return);

extern MP_status mp_get_rsp(OM_private_object session, OM_private_object context,
			    OM_object response, OM_sint32 invoke_id);

extern OM_workspace mp_initialize(void);

extern MP_status mp_negotiate(MP_feature feature_list[], OM_workspace workspace);

extern MP_status mp_receive(OM_private_object session, OM_private_object context,
			    OM_sint *primitive_return, OM_sint *completion_flag_return,
			    MP_status *operation_notification_status_return,
			    OM_private_object *result_or_argument_return,
			    OM_sint32 *invoke_id_return);

extern MP_status mp_release_req(OM_private_object session, OM_private_object context,
				OM_object argument, OM_private_object *result_return,
				OM_sint32 *invoke_id_return);

extern MP_status mp_release_rsp(OM_private_object session, OM_private_object context,
				OM_object response, OM_sint32 invoke_id);

extern MP_status mp_set_req(OM_private_object session, OM_private_object context,
			    OM_object argument, OM_private_object *result_return,
			    OM_sint32 *invoke_id_return);

extern MP_status mp_set_rsp(OM_private_object session, OM_private_object context,
			    OM_object response, OM_sint32 invoke_id);

extern void mp_shutdown(OM_workspace workspace);

extern MP_status mp_unbind(OM_private_object session);

extern MP_status mp_validate_object(OM_workspace workspace, OM_object test_object,
				    OM_private_object *bad_argument_return);

extern MP_status mp_wait(MP_waiting_sessions bound_session_list[], OM_workspace workspace,
			 OM_uint32 timeout);

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* __XMP_H__ */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
