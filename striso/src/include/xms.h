/*****************************************************************************

 @(#) $Id: xms.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xms.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XMS_H__
#define __XMS_H__

#ident "@(#) $RCSfile: xms.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xms.h> defines MS_PACKAGE, the constant value for the Message Store
 * package.  It also declares the interface functions, the data structures
 * passed to and from those functions and the defined constants used by the
 * functions and data structures for the Message Store package.
 *
 * All client programs that include this header must first include the Object
 * Management header <xom.h> (see reference XOM).
 *
 * All object identifiers are represented by constants defined in the header.
 * These constants are used with the macros defined in the XOM API
 * Specification.
 */

/*
 * xms.h (Message Store Package)
 */
#ifndef XMS_HEADER
#define XMS_HEADER

/* MS Package object identifier */
#define OMP_O_MS_PACKAGE "\x56\x06\x01\x02\x06\x01"

/* Type Definitions */

typedef OM_private_object MS_status;

typedef OM_sint MS_invoke_id;

typedef struct {
	OM_object_identifier feature;
	OM_boolean activated;
} MS_feature;

/* Interface Functions - Function Prototypes */
#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif				/* __BEGIN_DECLS */

MS_status ms_bind(OM_object bind_argument, OM_private_object *bind_result_return,
		  OM_private_object *bound_session_return);

MS_status ms_cancel_submission(OM_private_object session, OM_object mts_identifier,
			       MS_invoke_id * invoke_id_return);

MS_status ms_check_alert(OM_private_object session, OM_private_object *check_alert_result_return,);

MS_status ms_delete(OM_private_object session, OM_object delete_argument,
		    MS_invoke_id * invoke_id_return);

MS_status ms_fetch(OM_private_object session, OM_object fetch_argument,
		   OM_private_object *fetch_result_return, MS_invoke_id * invoke_id_return);

MS_status ms_initialize(MS_feature feature_list[], OM_sint *max_sessions,
			OM_sint *max_outstanding_operations, OM_workspace *workspace_return,);

MS_status ms_list(OM_private_object session, OM_object list_argument,
		  OM_private_object *list_result_return, MS_invoke_id * invoke_id_return);

MS_status ms_receive_result(OM_private_object session, OM_uint *completion_flag_return,
			    MS_status * operation_status_return, OM_private_object *result_return,
			    MS_invoke_id * invoke_id_return);

MS_status ms_register(OM_private_object session, OM_object register_argument,
		      MS_invoke_id * invoke_id_return);

MS_status ms_register_ms(OM_private_object session, OM_object register_ms_argument,
			 MS_invoke_id * invoke_id_return);

void ms_shutdown(void);

MS_status ms_submit(OM_private_object session, OM_object communique,
		    OM_private_object *submission_results_return, MS_invoke_id * invoke_id_return);

MS_status ms_summarize(OM_private_object session, OM_object summarize_argument,
		       OM_private_object *summarize_result_return, MS_invoke_id * invoke_id_return);

MS_status ms_unbind(OM_private_object session);

MS_status ms_wait(OM_private_object session, OM_uint32 interval,
		  OM_private_object *wait_result_return, MS_invoke_id * invoke_id_return);

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif				/* __END_DECLS */

/* Classes */
/*
 * Note: Every client program must explicitly import into every compilation unit
 * (C source program) the classes or Object Identifiers that it uses. Each of
 * these classes or Object Identifier names must then be explicitly exported
 * from just one compilation unit.
 *
 * Importing and exporting can be done using the OM_IMPORT and OM_EXPORT macros
 * respectively (see the XOM API Specification).  For instance, the client
 * program uses OM_IMPORT( MS_C_ATTRIBUTE_DEFAULTS ) which in turn will make use
 * of OMP_O_MS_C_ATTRIBUTE_DEFAULTS defined below.
 */
#define OMP_O_MS_C_ALERT_ADDRESS		"\x56\x06\x01\x02\x06\x01\x01"
#define OMP_O_MS_C_ATTRIBUTE_DEFAULTS		"\x56\x06\x01\x02\x06\x01\x02"
#define OMP_O_MS_C_ATTRIBUTE_ERROR		"\x56\x06\x01\x02\x06\x01\x03"
#define OMP_O_MS_C_ATTRIBUTE_PROBLEM		"\x56\x06\x01\x02\x06\x01\x04"
#define OMP_O_MS_C_ATTRIBUTE_SELECTION		"\x56\x06\x01\x02\x06\x01\x05"
#define OMP_O_MS_C_AUTO_ACTION			"\x56\x06\x01\x02\x06\x01\x06"
#define OMP_O_MS_C_AUTO_ACTION_DEREGISTRATION	"\x56\x06\x01\x02\x06\x01\x07"
#define OMP_O_MS_C_AUTO_ACTION_REGISTRATION	"\x56\x06\x01\x02\x06\x01\x08"
#define OMP_O_MS_C_AUTO_ACTION_REQUEST_ERROR	"\x56\x06\x01\x02\x06\x01\x09"
#define OMP_O_MS_C_AUTO_ACTION_REQUESTPROBLEM	"\x56\x06\x01\x02\x06\x01\x0A"
#define OMP_O_MS_C_AUTO_ALERT_REG_PARAMETERS	"\x56\x06\x01\x02\x06\x01\x0B"
#define OMP_O_MS_C_AUTO_FORWARD_ARGUMENTS	"\x56\x06\x01\x02\x06\x01\x0C"
#define OMP_O_MS_C_AUTO_FORWARD_REG_PARAMETERS	"\x56\x06\x01\x02\x06\x01\x0D"
#define OMP_O_MS_C_BIND_ARGUMENT		"\x56\x06\x01\x02\x06\x01\x0E"
#define OMP_O_MS_C_BIND_ERROR			"\x56\x06\x01\x02\x06\x01\x0F"
#define OMP_O_MS_C_BIND_RESULT			"\x56\x06\x01\x02\x06\x01\x10"
#define OMP_O_MS_C_CANCEL_SUBMISSION_ERROR	"\x56\x06\x01\x02\x06\x01\x11"
#define OMP_O_MS_C_CHANGE_CREDENTIALS		"\x56\x06\x01\x02\x06\x01\x12"
#define OMP_O_MS_C_CHECK_ALERT_RESULT		"\x56\x06\x01\x02\x06\x01\x13"
#define OMP_O_MS_C_COMMON_CONTROLS		"\x56\x06\x01\x02\x06\x01\x14"
#define OMP_O_MS_C_COMMUNICATIONS_ERROR		"\x56\x06\x01\x02\x06\x01\x15"
#define OMP_O_MS_C_CREATION_TIME_RANGE		"\x56\x06\x01\x02\x06\x01\x16"
#define OMP_O_MS_C_CREDENTIALS			"\x56\x06\x01\x02\x06\x01\x17"
#define OMP_O_MS_C_DEFAULT_DELIVERY_CONTROLS	"\x56\x06\x01\x02\x06\x01\x18"
#define OMP_O_MS_C_DELETE_ARGUMENT		"\x56\x06\x01\x02\x06\x01\x19"
#define OMP_O_MS_C_DELETE_ERROR			"\x56\x06\x01\x02\x06\x01\x1A"
#define OMP_O_MS_C_DELETE_PROBLEM		"\x56\x06\x01\x02\x06\x01\x1B"
#define OMP_O_MS_C_DELIVERABLE_CONTENT_TYPES	"\x56\x06\x01\x02\x06\x01\x1C"
#define OMP_O_MS_C_ELMT_NOT_SUBSCRIBED_ERROR	"\x56\x06\x01\x02\x06\x01\x1D"
#define OMP_O_MS_C_ERROR			"\x56\x06\x01\x02\x06\x01\x1E"
#define OMP_O_MS_C_FETCH_ARGUMENT		"\x56\x06\x01\x02\x06\x01\x1F"
#define OMP_O_MS_C_FETCH_ATTRIBUTE_DEFAULTS	"\x56\x06\x01\x02\x06\x01\x20"
#define OMP_O_MS_C_FETCH_RESTRICTION_ERROR	"\x56\x06\x01\x02\x06\x01\x21"
#define OMP_O_MS_C_FETCH_RESTRICTION_PROBLEM	"\x56\x06\x01\x02\x06\x01\x22"
#define OMP_O_MS_C_FETCH_RESULT			"\x56\x06\x01\x02\x06\x01\x23"
#define OMP_O_MS_C_INCONSISTENT_REQUEST_ERROR	"\x56\x06\x01\x02\x06\x01\x24"
#define OMP_O_MS_C_INVALID_PARAMETERS_ERROR	"\x56\x06\x01\x02\x06\x01\x25"
#define OMP_O_MS_C_ITEM				"\x56\x06\x01\x02\x06\x01\x26"
#define OMP_O_MS_C_ITEM_TO_FORWARD		"\x56\x06\x01\x02\x06\x01\x27"
#define OMP_O_MS_C_ITEMS			"\x56\x06\x01\x02\x06\x01\x28"
#define OMP_O_MS_C_LABEL_AND_REDIRECTION	"\x56\x06\x01\x02\x06\x01\x29"
#define OMP_O_MS_C_LABELS_AND_REDIRECTIONS	"\x56\x06\x01\x02\x06\x01\x2A"
#define OMP_O_MS_C_LIBRARY_ERROR		"\x56\x06\x01\x02\x06\x01\x2B"
#define OMP_O_MS_C_LIST_ARGUMENT		"\x56\x06\x01\x02\x06\x01\x2C"
#define OMP_O_MS_C_LIST_ATTRIBUTE_DEFAULTS	"\x56\x06\x01\x02\x06\x01\x2D"
#define OMP_O_MS_C_LIST_RESULT			"\x56\x06\x01\x02\x06\x01\x2E"
#define OMP_O_MS_C_MS_ENTRY_INFO_SELECTION	"\x56\x06\x01\x02\x06\x01\x2F"
#define OMP_O_MS_C_MS_ENTRY_INFORMATION		"\x56\x06\x01\x02\x06\x01\x30"
#define OMP_O_MS_C_ORIGINATOR_INVALID_ERROR	"\x56\x06\x01\x02\x06\x01\x31"
#define OMP_O_MS_C_PASSWORD			"\x56\x06\x01\x02\x06\x01\x32"
#define OMP_O_MS_C_RANGE			"\x56\x06\x01\x02\x06\x01\x33"
#define OMP_O_MS_C_RANGE_ERROR			"\x56\x06\x01\x02\x06\x01\x34"
#define OMP_O_MS_C_RECIPIENT_IMPROPER_ERROR	"\x56\x06\x01\x02\x06\x01\x35"
#define OMP_O_MS_C_REGISTER_ARGUMENT		"\x56\x06\x01\x02\x06\x01\x36"
#define OMP_O_MS_C_REGISTER_MS_ARGUMENT		"\x56\x06\x01\x02\x06\x01\x37"
#define OMP_O_MS_C_REGISTER_REJECTED_ERROR	"\x56\x06\x01\x02\x06\x01\x38"
#define OMP_O_MS_C_REMOTE_BIND_ERROR		"\x56\x06\x01\x02\x06\x01\x39"
#define OMP_O_MS_C_RESTRICTIONS			"\x56\x06\x01\x02\x06\x01\x3A"
#define OMP_O_MS_C_SECURITY_ERROR		"\x56\x06\x01\x02\x06\x01\x3B"
#define OMP_O_MS_C_SELECTOR			"\x56\x06\x01\x02\x06\x01\x3C"
#define OMP_O_MS_C_SEQUENCE_NUMBER_ERROR	"\x56\x06\x01\x02\x06\x01\x3D"
#define OMP_O_MS_C_SEQUENCE_NUMBER_PROBLEM	"\x56\x06\x01\x02\x06\x01\x3E"
#define OMP_O_MS_C_SEQUENCE_NUMBER_RANGE	"\x56\x06\x01\x02\x06\x01\x3F"
#define OMP_O_MS_C_SERVICE_ERROR		"\x56\x06\x01\x02\x06\x01\x40"
#define OMP_O_MS_C_SESSION			"\x56\x06\x01\x02\x06\x01\x41"
#define OMP_O_MS_C_STRONG_CREDENTIALS		"\x56\x06\x01\x02\x06\x01\x42"
#define OMP_O_MS_C_SUBMIT_CTRL_VIOLATED_ERROR	"\x56\x06\x01\x02\x06\x01\x43"
#define OMP_O_MS_C_SUMMARIZE_ARGUMENT		"\x56\x06\x01\x02\x06\x01\x44"
#define OMP_O_MS_C_SUMMARY			"\x56\x06\x01\x02\x06\x01\x45"
#define OMP_O_MS_C_SUMMARY_PRESENT		"\x56\x06\x01\x02\x06\x01\x46"
#define OMP_O_MS_C_SUMMARY_REQUESTS		"\x56\x06\x01\x02\x06\x01\x47"
#define OMP_O_MS_C_SUMMARY_RESULT		"\x56\x06\x01\x02\x06\x01\x48"
#define OMP_O_MS_C_SYSTEM_ERROR			"\x56\x06\x01\x02\x06\x01\x49"
#define OMP_O_MS_C_UNSUPT_CRITICAL_FUNC_ERROR	"\x56\x06\x01\x02\x06\x01\x4A"
#define OMP_O_MS_C_WAIT_RESULT			"\x56\x06\x01\x02\x06\x01\x4B"

/* OM Attribute Names */
#define MS_ABSENT				((OM_type) 1201)
#define MS_Address				((OM_type) 1202)
#define MS_Alert_Indication			((OM_type) 1203)
#define MS_ALERT_ADDRESS			((OM_type) 1204)
#define MS_ALERT_QUALIFIER			((OM_type) 1205)
#define MS_ALERT_REGISTRATION_IDENTIFIER	((OM_type) 1206)
#define MS_Allowed_Content_Types		((OM_type) 1207)
#define MS_Allowed_EITs				((OM_type) 1208)
#define MS_ALTERNATE_RECIPIENT			((OM_type) 1209)
#define MS_ALTERNATE_RECIPIENT_ALLOWED		((OM_type) 1210)
#define MS_ATTRIBUTE_TYPE			((OM_type) 1211)
#define MS_ATTRIBUTE_PROBLEMS			((OM_type) 1212)
#define MS_ATTRIBUTE_VALUE			((OM_type) 1213)
#define MS_ATTRIBUTES				((OM_type) 1214)
#define MS_Auto_Action_Deregistrations		((OM_type) 1215)
#define MS_Auto_Action_Registrations		((OM_type) 1216)
#define MS_Auto_Action_Request-Problems		((OM_type) 1217)
#define MS_AUTO_FORWARD_ARGUMENTS		((OM_type) 1218)
#define MS_AVA					((OM_type) 1219)
#define MS_Available_Attribute_Types		((OM_type) 1220)
#define MS_Available_Auto_Actions		((OM_type) 1221)
#define MS_BIND_TOKEN				((OM_type) 1222)
#define MS_CERTIFICATE				((OM_type) 1223)
#define MS_Change_Credentials			((OM_type) 1224)
#define MS_Child_Entries			((OM_type) 1225)
#define MS_CONFIDENTIALITY_ALGORITHM		((OM_type) 1226)
#define MS_CONTENT_CORRELATOR			((OM_type) 1227)
#define MS_CONTENT_IDENTIFIER			((OM_type) 1228)
#define MS_CONTENT_LENGTH			((OM_type) 1229)
#define MS_CONTENT_RETURN_REQUESTED		((OM_type) 1230)
#define MS_CONTENT_TYPE				((OM_type) 1231)
#define MS_CONTENT_TYPE-INT			((OM_type) 1232)
#define MS_CONVERSION_LOSS_PROHIBITED		((OM_type) 1233)
#define MS_CONVERSION_PROHIBITED		((OM_type) 1234)
#define MS_Content_Types_Supported		((OM_type) 1235)
#define MS_COUNT				((OM_type) 1236)
#define MS_CREATION_TIME_RANGE			((OM_type) 1237)
#define MS_Default_Delivery_Controls		((OM_type) 1238)
#define MS_DEFERRED_FELIVERY_TIME		((OM_type) 1239)
#define MS_DELETE_AFTER_AUTO_FORWARD		((OM_type) 1240)
#define MS_DELETE_PROBLEMS			((OM_type) 1241)
#define MS_Deliverable_Content_Types		((OM_type) 1242)
#define MS_Deliverable_EIT			((OM_type) 1243)
#define MS_Deliverable_Max_Content_Len		((OM_type) 1244)
#define MS_DISCLOSURE_ALLOWED			((OM_type) 1245)
#define MS_eitS					((OM_type) 1246)
#define MS_ENTRY_INFORMATION			((OM_type) 1247)
#define MS_EXPANSION_PROHIBITED			((OM_type) 1248)
#define MS_Fetch_Attribute_Defaults		((OM_type) 1249)
#define MS_Fetch_Restriction-Problems		((OM_type) 1250)
#define MS_Fetch_Restrictions			((OM_type) 1251)
#define MS_FILE_DESCRIPTOR			((OM_type) 1252)
#define MS_Filter				((OM_type) 1253)
#define MS_FROM					((OM_type) 1254)
#define MS_FROM-INT				((OM_type) 1255)
#define MS_IA5_String				((OM_type) 1256)
#define MS_INFORMATION_BASE_TYPE		((OM_type) 1257)
#define MS_Initiator				((OM_type) 1258)
#define MS_Initiator_Credentials		((OM_type) 1259)
#define MS_ITEM					((OM_type) 1260)
#define MS_ITEMS				((OM_type) 1261)
#define MS_LABEL_AND_REDIRECTION		((OM_type) 1262)
#define MS_Labels_And_Redirections		((OM_type) 1263)
#define MS_LSTEST_DELIVERY_TIME			((OM_type) 1264)
#define MS_Limit				((OM_type) 1265)
#define MS_LIST					((OM_type) 1266)
#define MS_List_Attribute_Defaults		((OM_type) 1267)
#define MS_Max_Content_Length			((OM_type) 1268)
#define ms_MS_Configuration_Request		((OM_type) 1269)
#define MS_Name					((OM_type) 1270)
#define MS_NEW_CREDENTIALS			((OM_type) 1271)
#define MS_NEW_ENTRY				((OM_type) 1272)
#define MS_NEXT					((OM_type) 1273)
#define MS_OCTET_String				((OM_type) 1274)
#define MS_OLD_CREDENTIALS			((OM_type) 1275)
#define MS_ORIGIN_CHECK				((OM_type) 1276)
#define MS_ORIGINAL_EITS			((OM_type) 1277)
#define MS_ORIGINATOR_CERTIFICATE		((OM_type) 1278)
#define MS_ORIGINATOR_NAME			((OM_type) 1279)
#define MS_ORIGINATOR_RETURN_ADDRESS		((OM_type) 1280)
#define MS_OTHER_PARAMETERS			((OM_type) 1281)
#define MS_Override				((OM_type) 1282)
#define MS_Permissible_Content_Types		((OM_type) 1283)
#define MS_PERMISSIBLE_CONTENT_TYPES_I		((OM_type) 1284)
#define MS_Permissible_EITs			((OM_type) 1285)
#define MS_Permissible_Lowest_Priority		((OM_type) 1286)
#define MS_Permissible_Max_Content_Len		((OM_type) 1287)
#define MS_Permissible_Operations		((OM_type) 1288)
#define MS_PRECISE				((OM_type) 1289)
#define MS_PRESENT				((OM_type) 1290)
#define MS_PRIORITY				((OM_type) 1291)
#define MS_PROBLEM				((OM_type) 1292)
#define MS_PROOF_OF_SUBMISSION_REQUESTED	((OM_type) 1293)
#define MS_Range				((OM_type) 1294)
#define MS_REASSIGNMENT_PROHIBITED		((OM_type) 1295)
#define MS_RECIPIENTS				((OM_type) 1296)
#define MS_RECIPIENT_DESCRIPTORS		((OM_type) 1297)
#define MS_REGISTRATION_ID			((OM_type) 1298)
#define MS_REGISTRATION_PARAMETER		((OM_type) 1299)
#define MS_REQUESTED				((OM_type) 1300)
#define MS_REQUESTED_ATTRIBUTES			((OM_type) 1301)
#define MS_REQUESTED_STR			((OM_type) 1302)
#define MS_Responder_Credentials		((OM_type) 1303)
#define MS_Restrict				((OM_type) 1304)
#define MS_Security_Context			((OM_type) 1305)
#define MS_SECURITY_LABEL			((OM_type) 1306)
#define MS_SECURITY_PROBLEM			((OM_type) 1307)
#define MS_SELECTION				((OM_type) 1308)
#define MS_SELECTOR				((OM_type) 1309)
#define MS_SEQUENCE_NUMBER			((OM_type) 1310)
#define MS_SEQUENCE_NUMBER-PROBLEMS		((OM_type) 1311)
#define MS_SEQUENCE_NUMBER_RANGE		((OM_type) 1312)
#define MS_SIMPLE				((OM_type) 1313)
#define MS_SPAN					((OM_type) 1314)
#define MS_STRONG				((OM_type) 1315)
#define MS_SUMMARIES				((OM_type) 1316)
#define MS_SUMMARY_REQUESTS			((OM_type) 1317)
#define MS_TO					((OM_type) 1318)
#define MS_TO-INT				((OM_type) 1319)
#define MS_TYPE					((OM_type) 1320)
#define MS_User_Security_Labels			((OM_type) 1321)
#define MS_WAIT_NEW_AVAILABLE			((OM_type) 1322)

/* Enumeration */

/*
 * The following enumeration tags and enumeration constants are defined for use
 * as values of the corresponding OM attributes.
 */

/* Enumeration Tags for MS_Problem: */
#define MS_E_action_type_not_subscribed		((OM_enumeration) 1)
#define MS_E_asynchrony_not_supported		((OM_enumeration) 2)
#define MS_E_attrib_type_not_subscribed		((OM_enumeration) 3)
#define MS_E_authentication_error		((OM_enumeration) 4)
#define MS_E_bad_argument			((OM_enumeration) 5)
#define MS_E_bad_class				((OM_enumeration) 6)
#define MS_E_bad_session			((OM_enumeration) 7)
#define MS_E_busy				((OM_enumeration) 8)
#define MS_E_child_entry_specified		((OM_enumeration) 9)
#define MS_E_communications_problem		((OM_enumeration) 10)
#define MS_E_content_length_problem		((OM_enumeration) 11)
#define MS_E_content_type_problem		((OM_enumeration) 12)
#define MS_E_defer_deliv_cancel_reject		((OM_enumeration) 13)
#define MS_E_delETE_rESTRICTION_PROBLEM		((OM_enumeration) 14)
#define MS_E_eit_problem			((OM_enumeration) 15)
#define MS_E_elmt_serv_not_subscribed		((OM_enumeration) 16)
#define MS_E_feature_not_negotiated		((OM_enumeration) 17)
#define MS_E_feature_unavailable		((OM_enumeration) 18)
#define MS_E_inappropriate_for_operatn		((OM_enumeration) 19)
#define MS_E_inappropriate_matching		((OM_enumeration) 20)
#define MS_E_inconsistent_request		((OM_enumeration) 21)
#define MS_E_invalid_attribute_value		((OM_enumeration) 22)
#define MS_E_invalid_FEATURE			((OM_enumeration) 23)
#define MS_E_invalid_parameters			((OM_enumeration) 24)
#define MS_E_message_submiT_id_invalid		((OM_enumeration) 25)
#define MS_E_miscellaneous			((OM_enumeration) 26)
#define MS_E_no_such_class			((OM_enumeration) 27)
#define MS_E_no_such_entry			((OM_enumeration) 28)
#define MS_E_originator_invalid			((OM_enumeration) 29)
#define MS_E_out_of_memory			((OM_enumeration) 30)
#define MS_E_recipient_improperly_specified	((OM_enumeration) 31)
#define MS_E_register_rejected			((OM_enumeration) 32)
#define MS_E_remote_bind_error			((OM_enumeration) 33)
#define MS_E_reversed				((OM_enumeration) 34)
#define MS_E_security				((OM_enumeration) 35)
#define MS_E_submission_ctrl_violated		((OM_enumeration) 36)
#define MS_E_too_many_operations		((OM_enumeration) 37)
#define MS_E_too_many_sessions			((OM_enumeration) 38)
#define MS_E_unable_establish_associatn		((OM_enumeration) 39)
#define MS_E_unacceptable_securE_contxt		((OM_enumeration) 40)
#define MS_E_unavailable			((OM_enumeration) 41)
#define MS_E_unavailable_action_type		((OM_enumeration) 42)
#define MS_E_unavailable_attribute_type		((OM_enumeration) 43)
#define MS_E_unsupported_critical_func		((OM_enumeration) 44)
#define MS_E_unwilling_to_perfoRM		((OM_enumeration) 45)

/* Constants */
#define MS_DEFAULT_FEATURE_LIST ((MS_feature) 0)
#define MS_SUCCESS ((MS_status) 0)
#define MS_NO_WORKSPACE ((MS_status) 1)

/* Constants of type OM_object */
#define MS_NO_FILTER				((OM_object) 0)
#define MS_NO_NEW_ENTRIES			((OM_object) 0)
#define MS_NULL_RESULT				((OM_object) 0)
#define MS_OPERATION_NOT_STARTED		((OM_object) 0)

/* Constants of type Integer */

/* Completion-Flag (Unsigned-Integer): */
#define MS_COMPLETED_OPERATION			((OM_uint) 1)
#define MS_OUTSTANDING_OPERATION		((OM_uint) 2)
#define MS_NO_OUTSTANDING_OPERATION		((OM_uint) 3)

/* Information-Base-Type (Integer): */
#define MS_STORED_MESSAGES			((OM_integer) 0)
#define MS_INLOG				((OM_integer) 1)
#define MS_OUTLOG				((OM_integer) 2)

#endif				/* XMS_HEADER */

#endif				/* __XMS_H__ */
