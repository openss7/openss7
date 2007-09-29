/*****************************************************************************

 @(#) $Id: xdssap.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xdssap.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XDSSAP_H__
#define __XDSSAP_H__

#ident "@(#) $RCSfile: xdssap.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xdssap.h> header defines the object identifiers of directory attribute
 * types and object classes supported by the Strong Authentication Package. It
 * also defines OM classes used to represent the values of the attribute types.
 *
 * When an application program includes the <xdssap.h> header, all the
 * definitions made in the Object Management header <xom.h> and the <xds.h>
 * header will also apply, as though the application had included <xom.h> and
 * <xds.h> as well as <xdssap.h>.
 */

#ifndef XDSSAP_HEADER
#define XDSSAP_HEADER

#define OMP_O_DS_STRONG_AUTHENT_PKG "\x2A\x86\x48\xCE\x1E\x02"

/* Intermediate object identifier macros */

#ifndef dsP_attributeType
#define dsP_attributeType(X) ("\x55\x4" #X) joint-iso-ccitt 5 4
#endif

#ifndef dsP_objectClass
#define dsP_objectClass(X) ("\x55\x6" #X) joint-iso-ccitt 5 6
#endif

#define dsP_sap_c(X) (OMP_O_DS_STRONG_AUTHENT_PKG #X)

/*
 * OM class names (prefixed DS_C_), Directory attribute types (prefixed DS_A_),
 * and Directory object classes (prefixed DS_O_)
 *
 * Every application program which makes use of a class or other Object
 * Identifier must explicitly import it into every compilation unit (C source
 * program) which uses it.  Each such class or Object Identifier name must be
 * explicitly exported from just one compilation unit.  In the header file, OM
 * class constants are prefixed with the OPM_O prefix to denote that they are OM
 * classes.  However, when using the OM_IMPORT and OM_EXPORT macros, the base
 * names (without the OMP_O prefix) should be used.  For example:
 *
 * OM_IMPORT(DS_O_CERT_AUTHORITY)
 */

/* Directory attribute types */
#define OMP_O_DS_A_AUTHORITY_REVOC_LIST	dsP_attributeType(\x26)
#define OMP_O_DS_A_CA_CERT		dsP_attributeType(\x25)
#define OMP_O_DS_A_CERT_REVOC_LIST	dsP_attributeType(\x27)
#define OMP_O_DS_A_CROSS_CERT_PAIR	dsP_attributeType(\x28)
#define OMP_O_DS_A_USER_CERT		dsP_attributeType(\x24)

/* Directory object classes */
#define OMP_O_DS_O_CERT_AUTHORITY	dsP_objectClass(\x10)
#define OMP_O_DS_O_STRONG_AUTHENT_USER	dsP_objectClass(\x0F)

/* OM class names */
#define OMP_O_DS_C_ALGORITHM_IDENT	dsP_sap_c(\x86\x35)
#define OMP_O_DS_C_CERT			dsP_sap_c(\x86\x36)
#define OMP_O_DS_C_CERT_LIST		dsP_sap_c(\x86\x37)
#define OMP_O_DS_C_CERT_PAIR		dsP_sap_c(\x86\x38)
#define OMP_O_DS_C_CERT_SUBLIST		dsP_sap_c(\x86\x39)
#define OMP_O_DS_C_SIGNATURE		dsP_sap_c(\x86\x3A)
#define OMP_O_DS_C_CERTS		dsP_sap_c(\x86\x3B)
#define OMP_O_DS_C_CROSS_CERTS		dsP_sap_c(\x86\x3C)
#define OMP_O_DS_C_FWD_CERT_PATH	dsP_sap_c(\x86\x3D)

/* OM attribute names */
#define DS_ALGORITHM			((OM_type) 821)
#define DS_FORWARD			((OM_type) 822)
#define DS_ISSUER			((OM_type) 823)
#define DS_LAST_UPDATE			((OM_type) 824)
#define DS_ALGORITHM_PARAMETERS		((OM_type) 825)
#define DS_REVERSE			((OM_type) 826)
#define DS_REVOC_DATE			((OM_type) 827)
#define DS_REVOKED_CERTS		((OM_type) 828)
#define DS_SERIAL_NBR			((OM_type) 829)
#define DS_SERIAL_NBRS			((OM_type) 830)
#define DS_SIGNATURE			((OM_type) 831)
#define DS_SIGNATURE_VALUE		((OM_type) 832)
#define DS_SUBJECT			((OM_type) 833)
#define DS_SUBJECT_ALGORITHM		((OM_type) 834)
#define DS_SUBJECT_PUBLIC_KEY		((OM_type) 835)
#define DS_VALIDITY_NOT_AFTER		((OM_type) 836)
#define DS_VALIDITY_NOT_BEFORE		((OM_type) 837)
#define DS_VERSION			((OM_type) 838)
#define DS_CERT				((OM_type) 839)
#define DS_CERT_PATH			((OM_type) 840)
#define DS_FWD_CERT_PATH		((OM_type) 841)

/* DS_Version */
#define DS_V1988			((OM_enumeration) 1)

/*upper bounds on string lengths and number of repeated OM attribute values*/
#define DS_VL_LAST_UPDATE		((OM_value_length) 17)
#define DS_VL_REVOC_DATE		((OM_value_length) 17)
#define DS_VL_VALIDITY_NOT_AFTER	((OM_value_length) 17)
#define DS_VL_VALIDITY_NOT_BEFORE	((OM_value_length) 17)
#define DS_VN_REVOC_DATE		((OM_value_number) 2)

#endif				/* XDSSAP_HEADER */

#endif				/* __XDSSAP_H__ */
