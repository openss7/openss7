/*****************************************************************************

 @(#) $Id: xdsbdcp.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xdsbdcp.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XDSBDCP_H__
#define __XDSBDCP_H__

#ident "@(#) $RCSfile: xdsbdcp.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xdsbdcp.h> header defines the object identifiers of directory attribute types and object
 * classes supported by the Basic Directory Contents Package. It also defines OM classes used to
 * represent the values of the attribute types.
 *
 * When an application program includes the <xdsbdcp.h> header, all the definitions made in the Object
 * Management header <xom.h> and the <xds.h> header will also apply, as though the application had
 * included <xom.h> and <xds.h> as well as <xdsbdcp.h>.
 */

#ifndef XDSBDCP_HEADER
#define XDSBDCP_HEADER

#define OMP_O_DS_BASIC_DIR_CONTENTS_PKG \x2A\x86\x48\xCE\x1E\x01

/* Intermediate object identifier macros */
#ifndef dsP_attributeType
#define dsP_attributeType(X) ("\x55\x4" #X)	/* joint-iso-ccitt 5 4 */
#endif

#ifndef dsP_objectClass
#define dsP_objectClass(X) ("\x55\x6" #X)	/* joint-iso-ccitt 5 6 */
#endif

#define dsP_bdcp_c(X) (OMP_O_DS_BASIC_DIR_CONTENTS_PKG #X)

/* OM class names (prefixed DS_C_), Directory attribute types (prefixed DS_A_), and Directory object
 * classes (prefixed DS_O_). */

/* 
 * Every application program which makes use of a class or other Object Identifier must explicitly
 * import it into every compilation unit (C source program) which uses it.
 *
 * Each such class or Object Identifier name must be explicitly exported from just one compilation
 * unit.
 *
 * In the header file, OM class constants are prefixed with the OPM_O prefix to denote that they are
 * OM classes.  However, when using the OM_IMPORT and OM_EXPORT macros, the base names (without the
 * OMP_O prefix) should be used.  For example:
 *
 * OM_IMPORT(DS_O_COUNTRY)
 */

/* Directory attribute types */
#define OMP_O_DS_A_ALIASED_OBJECT_NAME		dsP_attributeType(\x01)
#define OMP_O_DS_A_BUSINESS_CATEGORY		dsP_attributeType(\x0F)
#define OMP_O_DS_A_COMMON_NAME			dsP_attributeType(\x03)
#define OMP_O_DS_A_COUNTRY_NAME			dsP_attributeType(\x06)
#define OMP_O_DS_A_DESCRIPTION			dsP_attributeType(\x0D)
#define OMP_O_DS_A_DEST_INDICATOR		dsP_attributeType(\x1B)
#define OMP_O_DS_A_FACSIMILE_PHONE_NBR		dsP_attributeType(\x17)
#define OMP_O_DS_A_INTERNAT_ISDN_NBR		dsP_attributeType(\x19)
#define OMP_O_DS_A_KNOWLEDGE_INFO		dsP_attributeType(\x02)
#define OMP_O_DS_A_LOCALITY_NAME		dsP_attributeType(\x07)
#define OMP_O_DS_A_MEMBER			dsP_attributeType(\x1F)
#define OMP_O_DS_A_OBJECT_CLASS			dsP_attributeType(\x00)
#define OMP_O_DS_A_ORG_NAME			dsP_attributeType(\x0A)
#define OMP_O_DS_A_ORG_UNIT_NAME		dsP_attributeType(\x0B)
#define OMP_O_DS_A_OWNER			dsP_attributeType(\x20)
#define OMP_O_DS_A_PHONE_NBR			dsP_attributeType(\x14)
#define OMP_O_DS_A_PHYS_DELIV_OFF_NAME		dsP_attributeType(\x13)
#define OMP_O_DS_A_POST_OFFICE_BOX		dsP_attributeType(\x12)
#define OMP_O_DS_A_POSTAL_ADDRESS		dsP_attributeType(\x10)
#define OMP_O_DS_A_POSTAL_CODE			dsP_attributeType(\x11)
#define OMP_O_DS_A_PREF_DELIV_METHOD		dsP_attributeType(\x1C)
#define OMP_O_DS_A_PRESENTATION_ADDRESS		dsP_attributeType(\x1D)
#define OMP_O_DS_A_REGISTERED_ADDRESS		dsP_attributeType(\x1A)
#define OMP_O_DS_A_ROLE_OCCUPANT		dsP_attributeType(\x21)
#define OMP_O_DS_A_SEARCH_GUIDE			dsP_attributeType(\x0E)
#define OMP_O_DS_A_SEE_ALSO			dsP_attributeType(\x22)
#define OMP_O_DS_A_SERIAL_NBR			dsP_attributeType(\x05)
#define OMP_O_DS_A_STATE_OR_PROV_NAME		dsP_attributeType(\x08)
#define OMP_O_DS_A_STREET_ADDRESS		dsP_attributeType(\x09)
#define OMP_O_DS_A_SUPPORT_APPLIC_CONTEXT	dsP_attributeType(\x1E)
#define OMP_O_DS_A_SURNAME			dsP_attributeType(\x04)
#define OMP_O_DS_A_TELETEX_TERM_IDENT		dsP_attributeType(\x16)
#define OMP_O_DS_A_TELEX_NBR			dsP_attributeType(\x15)
#define OMP_O_DS_A_TITLE			dsP_attributeType(\x0C)
#define OMP_O_DS_A_USER_PASSWORD		dsP_attributeType(\x23)
#define OMP_O_DS_A_X121_ADDRESS			dsP_attributeType(\x18)
/* Directory object classes */
#define OMP_O_DS_O_ALIAS			dsP_objectClass(\x01)
#define OMP_O_DS_O_APPLIC_ENTITY		dsP_objectClass(\x0C)
#define OMP_O_DS_O_APPLIC_PROCESS		dsP_objectClass(\x0B)
#define OMP_O_DS_O_COUNTRY			dsP_objectClass(\x02)
#define OMP_O_DS_O_DEVICE			dsP_objectClass(\x0E)
#define OMP_O_DS_O_DSA				dsP_objectClass(\x0D)
#define OMP_O_DS_O_GROUP_OF_NAMES		dsP_objectClass(\x09)
#define OMP_O_DS_O_LOCALITY			dsP_objectClass(\x03)
#define OMP_O_DS_O_ORG				dsP_objectClass(\x04)
#define OMP_O_DS_O_ORG_PERSON			dsP_objectClass(\x07)
#define OMP_O_DS_O_ORG_ROLE			dsP_objectClass(\x08)
#define OMP_O_DS_O_ORG_UNIT			dsP_objectClass(\x05)
#define OMP_O_DS_O_PERSON			dsP_objectClass(\x06)
#define OMP_O_DS_O_RESIDENTIAL_PERSON		dsP_objectClass(\x0A)
#define OMP_O_DS_O_TOP				dsP_objectClass(\x00)
/* OM class names */
#define OMP_O_DS_C_FACSIMILE_PHONE_NBR		dsP_bdcp_c(\x86\x21)
#define OMP_O_DS_C_POSTAL_ADDRESS		dsP_bdcp_c(\x86\x22)
#define OMP_O_DS_C_SEARCH_CRITERION		dsP_bdcp_c(\x86\x23)
#define OMP_O_DS_C_SEARCH_GUIDE			dsP_bdcp_c(\x86\x24)
#define OMP_O_DS_C_TELETEX_TERM_IDENT		dsP_bdcp_c(\x86\x25)
#define OMP_O_DS_C_TELEX_NBR			dsP_bdcp_c(\x86\x26)
/* OM attribute names */
#define DS_ANSWERBACK				((OM_type) 801)
#define DS_COUNTRY_CODE				((OM_type) 802)
#define DS_CRITERIA				((OM_type) 803)
#define DS_OBJECT_CLASS				((OM_type) 804)
#define DS_PARAMETERS				((OM_type) 805)
#define DS_POSTAL_ADDRESS			((OM_type) 806)
#define DS_PHONE_NBR				((OM_type) 807)
#define DS_TELETEX_TERM				((OM_type) 808)
#define DS_TELEX_NBR				((OM_type) 809)
/* DS_Preferred_Delivery_Method */
enum DS_Preferred_Delivery_Method {
	DS_ANY_DELIV_METHOD = 0,
	DS_MHS_DELIV = 1,
	DS_PHYS_DELIV = 2,
	DS_TELEX_DELIV = 3,
	DS_TELETEX_DELIV = 4,
	DS_G3_FACSIMILE_DELIV = 5,
	DS_G4_FACSIMILE_DELIV = 6,
	DS_IA5_TERMINAL_DELIV = 7,
	DS_VIDEOTEX_DELIV = 8,
	DS_PHONE_DELIV = 9,
};

/*upper bounds on string lengths and number of repeated OM attribute values*/
#define DS_VL_A_BUSINESS_CATEGORY		((OM_value_length) 128)
#define DS_VL_A_COMMON_NAME			((OM_value_length) 64)
#define DS_VL_A_DESCRIPTION			((OM_value_length) 1024)
#define DS_VL_A_DEST_INDICATOR			((OM_value_length) 128)
#define DS_VL_A_INTERNAT_ISDN_NBR		((OM_value_length) 16)
#define DS_VL_A_LOCALITY_NAME			((OM_value_length) 128)
#define DS_VL_A_ORG_NAME			((OM_value_length) 64)
#define DS_VL_A_ORG_UNIT_NAME			((OM_value_length) 64)
#define DS_VL_A_PHYS_DELIV_OFF_NAME		((OM_value_length) 128)
#define DS_VL_A_POST_OFFICE_BOX			((OM_value_length) 40)
#define DS_VL_A_POSTAL_CODE			((OM_value_length) 40)
#define DS_VL_A_SERIAL_NBR			((OM_value_length) 64)
#define DS_VL_A_STATE_OR_PROV_NAME		((OM_value_length) 128)
#define DS_VL_A_STREET_ADDRESS			((OM_value_length) 128)
#define DS_VL_A_SURNAME				((OM_value_length) 64)
#define DS_VL_A_PHONE_NBR			((OM_value_length) 32)
#define DS_VL_A_TITLE				((OM_value_length) 64)
#define DS_VL_A_USER_PASSWORD			((OM_value_length) 128)
#define DS_VL_A_X121_ADDRESS			((OM_value_length) 15)
#define DS_VL_ANSWERBACK			((OM_value_length) 8)
#define DS_VL_COUNTRY_CODE			((OM_value_length) 4)
#define DS_VL_POSTAL_ADDRESS			((OM_value_length) 30)
#define DS_VL_PHONE_NBR				((OM_value_length) 32)
#define DS_VL_TELETEX_TERM			((OM_value_length) 1024)
#define DS_VL_TELEX_NBR				((OM_value_length) 14)
#define DS_VN_POSTAL_ADDRESS			((OM_value_length) 6)

#endif				/* XDSBDCP_HEADER */

#endif				/* __XDSBDCP_H__ */
