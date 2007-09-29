/*****************************************************************************

 @(#) $Id: xmp_cmis.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xmp_cmis.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XMP_CMIS_H__
#define __XMP_CMIS_H__

#ident "@(#) $RCSfile: xmp_cmis.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xmp_cmis.h> header declares the interface functions, the structures
 * passed to and from those functions, and the defined constants used by the
 * functions and structures.
 *
 * All application programs which include this header must first include the
 * OSI-Abstract-Data Manipulation header <xom.h>.
 */
#include <xom.h>
/*
 * All Object Identifiers are represented by constants defined in the headers.
 * These constants are used with the macros defined in the XOM API (see
 * reference XOM). A constant is defined to represent the Object Identifier of
 * the CMIS Management Service package:
 */
#define OMP_O_MP_CMIS_PKG "\x2a\x86\x3a\x00\x88\x1a\x06\x02"

/* Defined constants */
/* Intermediate object identifier macro */
#define mpP_cmis(X) (OMP_O_MP_CMIS_PKG# #X)

/* OM class names (prefixed MP_C_) */
/* 
 * Every application program which makes use of a class or other Object
 * Identifier must explicitly import it into every compilation unit (C source
 * program) which uses it. Each such class or Object Identifier name must be
 * explicitly exported from just one compilation unit.
 * 
 * In the header file, OM class constants are prefixed with the OPM_O prefix to
 * denote that they are OM classes. However, when using the OM_IMPORT and
 * OM_EXPORT macros, the base names (without the OMP_O prefix) should be used.
 * For example:
 *
 * OM_IMPORT(MP_C_EVENT_REPLY)
 */
#define OMP_O_MP_C_ACTION_ERROR			mpP_cmis(\x8F\x51)
#define OMP_O_MP_C_ACTION_ERROR_INFO		mpP_cmis(\x8F\x52)
#define OMP_O_MP_C_ACTION_INFO			mpP_cmis(\x8F\x53)
#define OMP_O_MP_C_ACTION_REPLY			mpP_cmis(\x8F\x54)
#define OMP_O_MP_C_ACTION_TYPE_ID		mpP_cmis(\x8F\x55)
#define OMP_O_MP_C_ATTRIBUTE			mpP_cmis(\x8F\x56)
#define OMP_O_MP_C_ATTRIBUTE_ERROR		mpP_cmis(\x8F\x57)
#define OMP_O_MP_C_ATTRIBUTE_ID			mpP_cmis(\x8F\x58)
#define OMP_O_MP_C_ATTRIBUTE_ID_ERROR		mpP_cmis(\x8F\x59)
#define OMP_O_MP_C_ATTRIBUTE_ID_LIST		mpP_cmis(\x8F\x5A)
#define OMP_O_MP_C_BASE_MANAGED_OBJECT_ID	mpP_cmis(\x8F\x5B)
#define OMP_O_MP_C_CMIS_ACTION_ARGUMENT		mpP_cmis(\x8F\x5C)
#define OMP_O_MP_C_CMIS_ACTION_RESULT		mpP_cmis(\x8F\x5D)
#define OMP_O_MP_C_CMIS_CANCEL_GET_ARGUMENT	mpP_cmis(\x8F\x5E)
#define OMP_O_MP_C_CMIS_CREATE_ARGUMENT		mpP_cmis(\x8F\x5F)
#define OMP_O_MP_C_CMIS_CREATE_RESULT		mpP_cmis(\x8F\x60)
#define OMP_O_MP_C_CMIS_DELETE_ARGUMENT		mpP_cmis(\x8F\x61)
#define OMP_O_MP_C_CMIS_DELETE_RESULT		mpP_cmis(\x8F\x62)
#define OMP_O_MP_C_CMIS_EVENT_REPORT_ARGUMENT	mpP_cmis(\x8F\x63)
#define OMP_O_MP_C_CMIS_EVENT_REPORT_RESULT	mpP_cmis(\x8F\x64)
#define OMP_O_MP_C_CMIS_FILTER			mpP_cmis(\x8F\x65)
#define OMP_O_MP_C_CMIS_GET_ARGUMENT		mpP_cmis(\x8F\x66)
#define OMP_O_MP_C_CMIS_GET_LIST_ERROR		mpP_cmis(\x8F\x67)
#define OMP_O_MP_C_CMIS_GET_RESULT		mpP_cmis(\x8F\x68)
#define OMP_O_MP_C_CMIS_LINKED_REPLY_ARGUMENT	mpP_cmis(\x8F\x69)
#define OMP_O_MP_C_CMIS_SERVICE_ERROR		mpP_cmis(\x8F\x6A)
#define OMP_O_MP_C_CMIS_SERVICE_REJECT		mpP_cmis(\x8F\x6B)
#define OMP_O_MP_C_CMIS_SET_ARGUMENT		mpP_cmis(\x8F\x6C)
#define OMP_O_MP_C_CMIS_SET_LIST_ERROR		mpP_cmis(\x8F\x6D)
#define OMP_O_MP_C_CMIS_SET_RESULT		mpP_cmis(\x8F\x6E)
#define OMP_O_MP_C_COMPLEXITY_LIMITATION	mpP_cmis(\x8F\x6F)
#define OMP_O_MP_C_CREATE_OBJECT_INSTANCE	mpP_cmis(\x8F\x70)
#define OMP_O_MP_C_DELETE_ERROR			mpP_cmis(\x8F\x71)
#define OMP_O_MP_C_ERROR_INFO			mpP_cmis(\x8F\x72)
#define OMP_O_MP_C_EVENT_INFO			mpP_cmis(\x8F\x73)
#define OMP_O_MP_C_EVENT_REPLY			mpP_cmis(\x8F\x74)
#define OMP_O_MP_C_EVENT_TYPE_ID		mpP_cmis(\x8F\x75)
#define OMP_O_MP_C_FILTER_ITEM			mpP_cmis(\x8F\x76)
#define OMP_O_MP_C_GET_INFO_STATUS		mpP_cmis(\x8F\x77)
#define OMP_O_MP_C_INVALID_ARGUMENT_VALUE	mpP_cmis(\x8F\x78)
#define OMP_O_MP_C_MISSING_ATTRIBUTE_VALUE	mpP_cmis(\x8F\x79)
#define OMP_O_MP_C_MODIFICATION			mpP_cmis(\x8F\x7A)
#define OMP_O_MP_C_MODIFICATION_LIST		mpP_cmis(\x8F\x7B)
#define OMP_O_MP_C_MULTIPLE_REPLY		mpP_cmis(\x8F\x7C)
#define OMP_O_MP_C_NO_SUCH_ACTION		mpP_cmis(\x8F\x7D)
#define OMP_O_MP_C_NO_SUCH_ACTION_ID		mpP_cmis(\x8F\x7E)
#define OMP_O_MP_C_NO_SUCH_ARGUMENT		mpP_cmis(\x8F\x7F)
#define OMP_O_MP_C_NO_SUCH_EVENT_ID		mpP_cmis(\x90\x00)
#define OMP_O_MP_C_NO_SUCH_EVENT_TYPE		mpP_cmis(\x90\x01)
#define OMP_O_MP_C_OBJECT_CLASS			mpP_cmis(\x90\x02)
#define OMP_O_MP_C_OBJECT_INSTANCE		mpP_cmis(\x90\x03)
#define OMP_O_MP_C_PROCESSING_FAILURE		mpP_cmis(\x90\x04)
#define OMP_O_MP_C_SCOPE			mpP_cmis(\x90\x05)
#define OMP_O_MP_C_SET_INFO_STATUS		mpP_cmis(\x90\x06)
#define OMP_O_MP_C_SETOF_ATTRIBUTE		mpP_cmis(\x90\x07)
#define OMP_O_MP_C_SETOF_CMIS_FILTER		mpP_cmis(\x90\x08)
#define OMP_O_MP_C_SETOF_GET_INFO_STATUS	mpP_cmis(\x90\x09)
#define OMP_O_MP_C_SETOF_SET_INFO_STATUS	mpP_cmis(\x90\x0A)
#define OMP_O_MP_C_SPECIFIC_ERROR_INFO		mpP_cmis(\x90\x0B)
#define OMP_O_MP_C_SUBSTRING			mpP_cmis(\x90\x0C)
#define OMP_O_MP_C_SUBSTRINGS			mpP_cmis(\x90\x0D)

/* The OM attribute names which are defined are listed below. */

#define MP_ACTION_ARGUMENT			((OM_type)11101)
#define MP_ACTION_ERROR				((OM_type)11102)
#define MP_ACTION_ERROR_INFO			((OM_type)11103)
#define MP_ACTION_ID				((OM_type)11104)
#define MP_ACTION_INFO				((OM_type)11105)
#define MP_ACTION_INFO_ARG			((OM_type)11106)
#define MP_ACTION_REPLY				((OM_type)11107)
#define MP_ACTION_REPLY_INFO			((OM_type)11108)
#define MP_ACTION_RESULT			((OM_type)11109)
#define MP_ACTION_TYPE				((OM_type)11110)
#define MP_ACTION_VALUE				((OM_type)11111)
#define MP_AND					((OM_type)11112)
#define MP_ANY_SUBSTRING			((OM_type)11113)
#define MP_ARGUMENT_VALUE			((OM_type)11114)
#define MP_ATTRIBUTE				((OM_type)11115)
#define MP_ATTRIBUTE_ERROR			((OM_type)11116)
#define MP_ATTRIBUTE_ID				((OM_type)11117)
#define MP_ATTRIBUTE_ID_ERROR			((OM_type)11118)
#define MP_ATTRIBUTE_ID_LIST			((OM_type)11119)
#define MP_ATTRIBUTE_LIST			((OM_type)11120)
#define MP_ATTRIBUTE_VALUE			((OM_type)11121)
#define MP_BASE_MANAGED_OBJECT_CLASS		((OM_type)11122)
#define MP_BASE_MANAGED_OBJECT_INSTANCE		((OM_type)11123)
#define MP_BASE_TO_NTH_LEVEL			((OM_type)11124)
#define MP_CREATE_OBJECT_INSTANCE		((OM_type)11125)
#define MP_CURRENT_TIME				((OM_type)11126)
#define MP_DELETE_ERROR				((OM_type)11127)
#define MP_DELETE_ERROR_INFO			((OM_type)11128)
#define MP_DELETE_RESULT			((OM_type)11129)
#define MP_DISTINGUISHED_NAME			((OM_type)11130)
#define MP_EQUALITY				((OM_type)11131)
#define MP_ERROR_ID				((OM_type)11132)
#define MP_ERROR_INFO				((OM_type)11133)
#define MP_ERROR_STATUS				((OM_type)11134)
#define MP_EVENT_ID				((OM_type)11135)
#define MP_EVENT_INFO				((OM_type)11136)
#define MP_EVENT_REPLY				((OM_type)11137)
#define MP_EVENT_REPLY_INFO			((OM_type)11138)
#define MP_EVENT_TIME				((OM_type)11139)
#define MP_EVENT_TYPE				((OM_type)11140)
#define MP_EVENT_VALUE				((OM_type)11141)
#define MP_FILTER				((OM_type)11142)
#define MP_FINAL_SUBSTRING			((OM_type)11143)
#define MP_GET_INFO_LIST			((OM_type)11144)
#define MP_GET_INFO_STATUS			((OM_type)11145)
#define MP_GET_INVOKE_ID			((OM_type)11146)
#define MP_GET_LIST_ERROR			((OM_type)11147)
#define MP_GET_RESULT				((OM_type)11148)
#define MP_GLOBAL_FORM				((OM_type)11149)
#define MP_GREATER_OR_EQUAL			((OM_type)11150)
#define MP_INDIVIDUAL_LEVELS			((OM_type)11151)
#define MP_INITIAL_SUBSTRING			((OM_type)11152)
#define MP_ITEM					((OM_type)11153)
#define MP_LESS_OR_EQUAL			((OM_type)11154)
#define MP_LOCAL_DN				((OM_type)11155)
#define MP_LOCAL_FORM				((OM_type)11156)
#define MP_MANAGED_OBJECT_CLASS			((OM_type)11157)
#define MP_MANAGED_OBJECT_INSTANCE		((OM_type)11158)
#define MP_MODIFICATION_LIST			((OM_type)11159)
#define MP_MODIFY_OPERATOR			((OM_type)11160)
#define MP_NAMED_NUMBERS			((OM_type)11161)
#define MP_NON_NULL_SET_INTERSECTION		((OM_type)11162)
#define MP_NON_SPECIFIC_FORM			((OM_type)11163)
#define MP_NOT					((OM_type)11164)
#define MP_OR					((OM_type)11165)
#define MP_PRESENT				((OM_type)11166)
#define MP_PROCESSING_FAILURE			((OM_type)11167)
#define MP_REFERENCE_OBJECT_INSTANCE		((OM_type)11168)
#define MP_REPLIES				((OM_type)11169)
#define MP_SCOPE				((OM_type)11170)
#define MP_SET_INFO_LIST			((OM_type)11171)
#define MP_SET_INFO_STATUS			((OM_type)11172)
#define MP_SET_LIST_ERROR			((OM_type)11173)
#define MP_SET_OF_ATTRIBUTE_ID			((OM_type)11174)
#define MP_SET_RESULT				((OM_type)11175)
#define MP_SPECIFIC_ERROR_INFO			((OM_type)11176)
#define MP_SUBSET_OF				((OM_type)11177)
#define MP_SUBSTRING				((OM_type)11178)
#define MP_SUBSTRINGS				((OM_type)11179)
#define MP_SUPERIOR_OBJECT_INSTANCE		((OM_type)11180)
#define MP_SUPERSET_OF				((OM_type)11181)
#define MP_SYNCHRONIZATION			((OM_type)11182)

/*
 * The following enumeration tags and enumeration constants are defined for use
 * as values of the corresponding OM attributes:
 */
/* MP_T_CMIS_Sync: */
#define MP_T_BEST_EFFORT			 0
#define MP_T_ATOMIC				 1

/* MP_T_Modify_Operator: */
#define MP_T_REPLACE				 0
#define MP_T_ADD_VALUES				 1
#define MP_T_REMOVE_VALUES			 2
#define MP_T_SET_TO_DEFAULT			 3

/* MP_E_Problem: */
#define MP_E_NO_SUCH_OBJECT_CLASS		 0
#define MP_E_NO_SUCH_OBJECT_INSTANCE		 1
#define MP_E_ACCESS_DENIED			 2
#define MP_E_SYNCHRONIZATION_NOT_SUPPORTED	 3
#define MP_E_INVALID_FILTER			 4
#define MP_E_NO_SUCH_ATTRIBUTE			 5
#define MP_E_INVALID_ATTRIBUTE_VALUE		 6
#define MP_E_GET_LIST_ERROR			 7
#define MP_E_SET_LIST_ERROR			 8
#define MP_E_NO_SUCH_ACTION			 9
#define MP_E_PROCESSING_FAILURE			10
#define MP_E_DUPLICATE_MANAGED_OBJECT_INSTANCE	11
#define MP_E_NO_SUCH_REFERENCE_OBJECT		12
#define MP_E_NO_SUCH_EVENT_TYPE			13
#define MP_E_NO_SUCH_ARGUMENT			14
#define MP_E_INVALID_ARGUMENT_VALUE		15
#define MP_E_INVALID_SCOPE			16
#define MP_E_INVALID_OBJECT_INSTANCE		17
#define MP_E_MISSING_ATTRIBUTE_VALUE		18
#define MP_E_CLASS_INSTANCE_CONFLICT		19
#define MP_E_COMPLEXITY_LIMITATION		20
#define MP_E_MISTYPED_OPERATION			21
#define MP_E_NO_SUCH_INVOKE_ID			22
#define MP_E_OPERATION_CANCELLED		23
#define MP_E_INVALID_OPERATION			24
#define MP_E_INVALID_OPERATOR			25

/* MP_E_Invoke_Problem: */
#define MP_E_DUPLICATE_INVOCATION		 0
#define MP_E_MISTYPED_ARGUMENT			 2
#define MP_E_RESOURCE_LIMITATION		 3
#define MP_E_UNRECOGNIZED_OPERATION		 1

/* MP_T_Scope: */
#define MP_T_BASE_OBJECT			 0
#define MP_T_FIRST_LEVEL_ONLY			 1
#define MP_T_WHOLE_SUBTREE			 2

#endif				/* __XMP_CMIS_H__ */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
