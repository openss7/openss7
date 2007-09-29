/*****************************************************************************

 @(#) $Id: xds.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xds.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XDS_H__
#define __XDS_H__

#ident "@(#) $RCSfile: xds.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#ifndef XDS_HEADER
#define XDS_HEADER

/* DS package object identifier */
#define OMP_O_DS_SERVICE_PKG "\x2A\x86\x48\xCE\x1E\x00"

/* Typedefs */
typedef OM_private_object DS_status;

typedef struct {
	OM_object_identifier feature;
	OM_boolean activated;
} DS_feature;

/* Function Prototypes */
#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif				/* __BEGIN_DECLS */

DS_status ds_abandon(OM_private_object session, OM_sint invoke_id);

DS_status ds_add_entry(OM_private_object session, OM_private_object context, OM_object name,
		       OM_object entry, OM_sint *invoke_id_return);

DS_status ds_bind(OM_object session, OM_workspace workspace,
		  OM_private_object *bound_session_return);

DS_status ds_compare(OM_private_object session, OM_private_object context, OM_object name,
		     OM_object ava, OM_private_object result_return, OM_sint *invoke_id_return);

OM_workspace ds_initialize(void);

DS_status ds_list(OM_private_object session, OM_private_object context, OM_object name,
		  OM_private_object *result_return, OM_sint *invoke_id_return);

DS_status ds_modify_entry(OM_private_object session, OM_private_object context, OM_object name,
			  OM_object changes, OM_sint *invoke_id_return);

DS_status ds_modify_rdn(OM_private_object session, OM_private_object context, OM_object name,
			OM_object new_RDN, OM_boolean delete_old_RDN, OM_sint *invoke_id_return);

DS_status ds_read(OM_private_object session, OM_private_object context, OM_object name,
		  OM_object selection, OM_private_object *result_return, OM_sint *invoke_id_return);

DS_status ds_receive_result(OM_private_object session, OM_sint invoke_id,
			    OM_uint *completion_flag_return, DS_status * operation_status_return,
			    OM_private_object *result_return, OM_sint *invoke_id_return);

DS_status ds_remove_entry(OM_private_object session, OM_private_object context, OM_object name,
			  OM_sint *invoke_id_return);

DS_status ds_search(OM_private_object session, OM_private_object context, OM_object name,
		    OM_sint subset, OM_object filter, OM_boolean search_aliases,
		    OM_object selection, OM_private_object *result_return,
		    OM_sint *invoke_id_return);

DS_status ds_shutdown(OM_workspace workspace);

DS_status ds_unbind(OM_private_object session);

DS_status ds_version(DS_feature feature_list[], OM_workspace workspace);

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif				/* __END_DECLS */

/* Defined constants */
/* Intermediate object identifier macro */
#define dsP_c(X) (OMP_O_DS_SERVICE_PKG #X)
/* OM class names (prefixed DS_C_) */
/* Every application program which makes use of a class or */
/* other Object Identifier must explicitly import it into */
/* every compilation unit (C source program) which uses it. */
/* Each such class or Object Identifier name must be */
/* explicitly exported from just one compilation unit. */
/* In the header file, OM class constants are prefixed with */
/* the OPM_O prefix to denote that they are OM classes. */
/* However, when using the OM_IMPORT and OM_EXPORT macros, */
/* the base names (without the OMP_O prefix) should be used.*/
/* For example: */
/* OM_IMPORT(DS_C_AVA) */
#define OMP_O_DS_C_ABANDON_FAILED	dsP_c(\x85\x3D)
#define OMP_O_DS_C_ACCESS_POINT		dsP_c(\x85\x3E)
#define OMP_O_DS_C_ADDRESS		dsP_c(\x85\x3F)
#define OMP_O_DS_C_ATTRIBUTE		dsP_c(\x85\x40)
#define OMP_O_DS_C_ATTRIBUTE_ERROR	dsP_c(\x85\x41)
#define OMP_O_DS_C_ATTRIBUTE_LIST	dsP_c(\x85\x42)
#define OMP_O_DS_C_ATTRIBUTE_PROBLEM	dsP_c(\x85\x43)
#define OMP_O_DS_C_AVA			dsP_c(\x85\x44)
#define OMP_O_DS_C_COMMON_RESULTS	dsP_c(\x85\x45)
#define OMP_O_DS_C_COMMUNICATIONS_ERROR	dsP_c(\x85\x46)
#define OMP_O_DS_C_COMPARE_RESULT	dsP_c(\x85\x47)
#define OMP_O_DS_C_CONTEXT		dsP_c(\x85\x48)
#define OMP_O_DS_C_CONTINUATION_REF	dsP_c(\x85\x49)
#define OMP_O_DS_C_DS_DN		dsP_c(\x85\x4A)
#define OMP_O_DS_C_DS_RDN		dsP_c(\x85\x4B)
#define OMP_O_DS_C_ENTRY_INFO		dsP_c(\x85\x4C)
#define OMP_O_DS_C_ENTRY_INFO_SELECTION	dsP_c(\x85\x4D)
#define OMP_O_DS_C_ENTRY_MOD		dsP_c(\x85\x4E)
#define OMP_O_DS_C_ENTRY_MOD_LIST	dsP_c(\x85\x4F)
#define OMP_O_DS_C_ERROR		dsP_c(\x85\x50)
#define OMP_O_DS_C_EXT			dsP_c(\x85\x51)
#define OMP_O_DS_C_FILTER		dsP_c(\x85\x52)
#define OMP_O_DS_C_FILTER_ITEM		dsP_c(\x85\x53)
#define OMP_O_DS_C_LIBRARY_ERROR	dsP_c(\x85\x54)
#define OMP_O_DS_C_LIST_INFO		dsP_c(\x85\x55)
#define OMP_O_DS_C_LIST_INFO_ITEM	dsP_c(\x85\x56)
#define OMP_O_DS_C_LIST_RESULT		dsP_c(\x85\x57)
#define OMP_O_DS_C_NAME			dsP_c(\x85\x58)
#define OMP_O_DS_C_NAME_ERROR		dsP_c(\x85\x59)
#define OMP_O_DS_C_OPERATION_PROGRESS	dsP_c(\x85\x5A)
#define OMP_O_DS_C_PARTIAL_OUTCOME_QUAL	dsP_c(\x85\x5B)
#define OMP_O_DS_C_PRESENTATION_ADDRESS	dsP_c(\x85\x5C)
#define OMP_O_DS_C_READ_RESULT		dsP_c(\x85\x5D)
#define OMP_O_DS_C_REFERRAL		dsP_c(\x85\x5E)
#define OMP_O_DS_C_RELATIVE_NAME	dsP_c(\x85\x5F)
#define OMP_O_DS_C_SEARCH_INFO		dsP_c(\x85\x60)
#define OMP_O_DS_C_SEARCH_RESULT	dsP_c(\x85\x61)
#define OMP_O_DS_C_SECURITY_ERROR	dsP_c(\x85\x62)
#define OMP_O_DS_C_SERVICE_ERROR	dsP_c(\x85\x63)
#define OMP_O_DS_C_SESSION		dsP_c(\x85\x64)
#define OMP_O_DS_C_SYSTEM_ERROR		dsP_c(\x85\x65)
#define OMP_O_DS_C_UPDATE_ERROR		dsP_c(\x85\x66)

/* OM attribute names */
#define DS_ACCESS_POINTS		((OM_type) 701)
#define DS_ADDRESS			((OM_type) 702)
#define DS_AE_TITLE			((OM_type) 703)
#define DS_ALIASED_RDNS			((OM_type) 704)
#define DS_ALIAS_DEREFERENCED		((OM_type) 705)
#define DS_ALIAS_ENTRY			((OM_type) 706)
#define DS_ALL_ATTRIBUTES		((OM_type) 707)
#define DS_ASYNCHRONOUS			((OM_type) 708)
#define DS_ATTRIBUTES			((OM_type) 709)
#define DS_ATTRIBUTES_SELECTED		((OM_type) 710)
#define DS_ATTRIBUTE_TYPE		((OM_type) 711)
#define DS_ATTRIBUTE_VALUE		((OM_type) 712)
#define DS_ATTRIBUTE_VALUES		((OM_type) 713)
#define DS_AUTOMATIC_CONTINUATION	((OM_type) 714)
#define DS_AVAS				((OM_type) 715)
#define DS_CHAINING_PROHIB		((OM_type) 716)
#define DS_CHANGES			((OM_type) 717)
#define DS_CRIT				((OM_type) 718)
#define DS_DONT_DEREFERENCE_ALIASES	((OM_type) 719)
#define DS_DONT_USE_COPY		((OM_type) 720)
#define DS_DSA_ADDRESS			((OM_type) 721)
#define DS_DSA_NAME			((OM_type) 722)
#define DS_ENTRIES			((OM_type) 723)
#define DS_ENTRY			((OM_type) 724)
#define DS_EXT				((OM_type) 725)
#define DS_FILE_DESCRIPTOR		((OM_type) 726)
#define DS_FILTERS			((OM_type) 727)
#define DS_FILTER_ITEMS			((OM_type) 728)
#define DS_FILTER_ITEM_TYPE		((OM_type) 729)
#define DS_FILTER_TYPE			((OM_type) 730)
#define DS_FINAL_SUBSTRING		((OM_type) 731)
#define DS_FROM_ENTRY			((OM_type) 732)
#define DS_IDENT			((OM_type) 733)
#define DS_INFO_TYPE			((OM_type) 734)
#define DS_INITIAL_SUBSTRING		((OM_type) 735)
#define DS_ITEM_PARAMETERS		((OM_type) 736)
#define DS_LIMIT_PROBLEM		((OM_type) 737)
#define DS_LIST_INFO			((OM_type) 738)
#define DS_LOCAL_SCOPE			((OM_type) 739)
#define DS_MATCHED			((OM_type) 740)
#define DS_MOD_TYPE			((OM_type) 741)
#define DS_NAME_RESOLUTION_PHASE	((OM_type) 742)
#define DS_NEXT_RDN_TO_BE_RESOLVED	((OM_type) 743)
#define DS_N_ADDRESSES			((OM_type) 744)
#define DS_OBJECT_NAME			((OM_type) 745)
#define DS_OPERATION_PROGRESS		((OM_type) 746)
#define DS_PARTIAL_OUTCOME_QUAL		((OM_type) 747)
#define DS_PERFORMER			((OM_type) 748)
#define DS_PREFER_CHAINING		((OM_type) 749)
#define DS_PRIORITY			((OM_type) 750)
#define DS_PROBLEM			((OM_type) 751)
#define DS_PROBLEMS			((OM_type) 752)
#define DS_P_SELECTOR			((OM_type) 753)
#define DS_RDN				((OM_type) 754)
#define DS_RDNS				((OM_type) 755)
#define DS_RDNS_RESOLVED		((OM_type) 756)
#define DS_REQUESTOR			((OM_type) 757)
#define DS_SCOPE_OF_REFERRAL		((OM_type) 758)
#define DS_SEARCH_INFO			((OM_type) 759)
#define DS_SIZE_LIMIT			((OM_type) 760)
#define DS_SUBORDINATES			((OM_type) 761)
#define DS_S_SELECTOR			((OM_type) 762)
#define DS_TARGET_OBJECT		((OM_type) 763)
#define DS_TIME_LIMIT			((OM_type) 764)
#define DS_T_SELECTOR			((OM_type) 765)
#define DS_UNAVAILABLE_CRIT_EXT		((OM_type) 766)
#define DS_UNCORRELATED_LIST_INFO	((OM_type) 767)
#define DS_UNCORRELATED_SEARCH_INFO	((OM_type) 768)
#define DS_UNEXPLORED			((OM_type) 769)

/* DS_Filter_Item_Type */
enum DS_Filter_Item_Type {
	DS_EQUALITY = 0,
	DS_SUBSTRINGS = 1,
	DS_GREATER_OR_EQUAL = 2,
	DS_LESS_OR_EQUAL = 3,
	DS_PRESENT = 4,
	DS_APPROXIMATE_MATCH = 5
};

/* DS_Filter_Type */
enum DS_Filter_Type {
	DS_ITEM = 0,
	DS_AND = 1,
	DS_OR = 2,
	DS_NOT = 3
};

/* DS_Information_Type */
enum DS_Information_Type {
	DS_TYPES_ONLY = 0,
	DS_TYPES_AND_VALUES = 1
};

/* DS_Limit_Problem */
enum DS_Limit_Problem {
	DS_TIME_LIMIT_EXCEEDED = 0,
	DS_SIZE_LIMIT_EXCEEDED = 1,
	DS_ADMIN_LIMIT_EXCEEDED = 2
};

/* DS_Modification_Type */
enum DS_Modification_Type {
	DS_ADD_ATTRIBUTE = 0,
	DS_REMOVE_ATTRIBUTE = 1,
	DS_ADD_VALUES = 2,
	DS_REMOVE_VALUES = 3
};

/* DS_Name_Resolution_Phase */
enum DS_Name_Resolution_Phase {
	DS_NOT_STARTED = 1,
	DS_PROCEEDING = 2,
	DS_COMPLETED = 3
};

/* DS_Priority */
enum DS_Priority {
	DS_LOW = 0,
	DS_MEDIUM = 1,
	DS_HIGH = 2
};

/* DS_Problem */
enum DS_Problem {
	DS_E_ADMIN_LIMIT_EXCEEDED = 1,
	DS_E_AFFECTS_MULTIPLE_DSAS = 2,
	DS_E_ALIAS_DEREFERENCING_PROBLEM = 3,
	DS_E_ALIAS_PROBLEM = 4,
	DS_E_ATTRIBUTE_OR_VALUE_EXISTS = 5,
	DS_E_BAD_ARGUMENT = 6,
	DS_E_BAD_CLASS = 7,
	DS_E_BAD_CONTEXT = 8,
	DS_E_BAD_NAME = 9,
	DS_E_BAD_SESSION = 10,
	DS_E_BAD_WORKSPACE = 11,
	DS_E_BUSY = 12,
	DS_E_CANNOT_ABANDON = 13,
	DS_E_CHAINING_REQUIRED = 14,
	DS_E_COMMUNICATIONS_PROBLEM = 15,
	DS_E_CONSTRAINT_VIOLATION = 16,
	DS_E_DIT_ERROR = 17,
	DS_E_ENTRY_EXISTS = 18,
	DS_E_INAPPROP_AUTHENTICATION = 19,
	DS_E_INAPPROP_MATCHING = 20,
	DS_E_INSUFFICIENT_ACCESS_RIGHTS = 21,
	DS_E_INVALID_ATTRIBUTE_SYNTAX = 22,
	DS_E_INVALID_ATTRIBUTE_VALUE = 23,
	DS_E_INVALID_CREDENTIALS = 24,
	DS_E_INVALID_REF = 25,
	DS_E_INVALID_SIGNATURE = 26,
	DS_E_LOOP_DETECTED = 27,
	DS_E_MISCELLANEOUS = 28,
	DS_E_MISSING_TYPE = 29,
	DS_E_MIXED_SYNCHRONOUS = 30,
	DS_E_NAMING_VIOLATION = 31,
	DS_E_NO_INFO = 32,
	DS_E_NO_SUCH_ATTRIBUTE_OR_VALUE = 33,
	DS_E_NO_SUCH_OBJECT = 34,
	DS_E_NO_SUCH_OPERATION = 35,
	DS_E_NOT_ALLOWED_ON_NON_LEAF = 36,
	DS_E_NOT_ALLOWED_ON_RDN = 37,
	DS_E_NOT_SUPPORTED = 38,
	DS_E_OBJECT_CLASS_MOD_PROHIB = 39,
	DS_E_OBJECT_CLASS_VIOLATION = 40,
	DS_E_OUT_OF_SCOPE = 41,
	DS_E_PROTECTION_REQUIRED = 42,
	DS_E_TIME_LIMIT_EXCEEDED = 43,
	DS_E_TOO_LATE = 44,
	DS_E_TOO_MANY_OPERATIONS = 45,
	DS_E_TOO_MANY_SESSIONS = 46,
	DS_E_UNABLE_TO_PROCEED = 47,
	DS_E_UNAVAILABLE = 48,
	DS_E_UNAVAILABLE_CRIT_EXT = 49,
	DS_E_UNDEFINED_ATTRIBUTE_TYPE = 50,
	DS_E_UNWILLING_TO_PERFORM = 51
};

/* DS_Scope_Of_Referral */
enum DS_Scope_Of_Referral {
	DS_DMD = 0,
	DS_COUNTRY = 1
};

/* OM_object constants */
#define DS_DEFAULT_CONTEXT		((OM_object) 0)
#define DS_DEFAULT_SESSION		((OM_object) 0)
#define DS_OPERATION_NOT_STARTED	((OM_object) 0)
#define DS_NO_FILTER			((OM_object) 0)
#define DS_NULL_RESULT			((OM_object) 0)
#define DS_SELECT_ALL_TYPES		((OM_object) 1)
#define DS_SELECT_ALL_TYPES_AND_VALUES	((OM_object) 2)
#define DS_SELECT_NO_ATTRIBUTES		((OM_object) 0)
#define DS_SUCCESS			((DS_status) 0)
#define DS_INVALID_WORKSPACE		((DS_status) 2)
#define DS_NO_WORKSPACE			((DS_status) 1)

/* ds_search subset */
#define DS_BASE_OBJECT			((OM_sint) 0)
#define DS_ONE_LEVEL			((OM_sint) 1)
#define DS_WHOLE_SUBTREE		((OM_sint) 2)

/* ds_receive_result completion_flag_return */
#define DS_COMPLETED_OPERATION		((OM_uint) 1)
#define DS_OUTSTANDING_OPERATIONS	((OM_uint) 2)
#define DS_NO_OUTSTANDING_OPERATION	((OM_uint) 3)
#define DS_OTHER_COMPLETED_OPERATIONS	((OM_uint) 4)

/* Invocation Identifier Flags for ds_receive_result */
#define DS_ANY_OPERATION		((OM_sint) 0)

/* asynchronous operations limit (implementation-defined) */
#define DS_MAX_OUTSTANDING_OPERATIONS	1

/* asynchronous event posting */
#define DS_NO_VALID_FILE_DESCRIPTOR	-1

#endif				/* XDS_HEADER */

#endif				/* __XDS_H__ */
