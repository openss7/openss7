/*****************************************************************************

 @(#) $Id: xdsmdup.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xdsmdup.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XDSMDUP_H__
#define __XDSMDUP_H__

#ident "@(#) $RCSfile: xdsmdup.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xdsmdup.h> header defines the object identifiers of directory attribute
 * types and object classes supported by the MHS Directory User Package. It also
 * defines OM classes used to represent the values of the attribute types.
 *
 * When an application program includes the <xdsmdup.h> header, all the
 * definitions made in the Object Management header <xom.h> and the <xds.h>
 * header will also apply, as though the application had included <xom.h> and
 * <xds.h> as well as <xdsmdup.h>.
 */

#ifndef XDSMDUP_HEADER
#define XDSMDUP_HEADER

#ifndef XMHP_HEADER
#include <xmhp.h>
#endif				/* XMHP_HEADER */

/* MDUP package object identifier */
#define OMP_O_DS_MHS_DIR_USER_PKG "\x2A\x86\x48\xCE\x1E\x03"

/* Intermediate object identifier macros */
#define dsP_MHSattributeType(X)	("\x56\x5\x2" #X)	/* joint-iso-ccitt 6 5 2 */
#define dsP_MHSobjectClass(X)	("\x56\x5\x1" #X)	/* joint-iso-ccitt 6 5 1 */
#define dsP_mdup_c(X) (OMP_O_DS_MHS_DIR_USER_PKG #X)

/*
 * OM class names (prefixed DS_C_), Directory attribute types (prefixed DS_A_),
 * and Directory object classes (prefixed DS_O_)
 *
 * Every application program which makes use of a class or other Object
 * Identifier must explicitly import it into every compilation unit (C source
 * program) which uses it.
 *
 * Each such class or Object Identifier name must be explicitly exported from
 * just one compilation unit.  In the header file, OM class constants are
 * prefixed with the OPM_O prefix to denote that they are OM classes.  However,
 * when using the OM_IMPORT and OM_EXPORT macros, the base names (without the
 * OMP_O prefix) should be used.  For example:
 *
 * OM_IMPORT(DS_O_CERT_AUTHORITY)
 */

/* Directory attribute types */
#define OMP_O_DS_A_DELIV_CONTENT_LENGTH		dsP_MHSattributeType(\x00)
#define OMP_O_DS_A_DELIV_CONTENT_TYPES		dsP_MHSattributeType(\x01)
#define OMP_O_DS_A_DELIV_EITS			dsP_MHSattributeType(\x02)
#define OMP_O_DS_A_DL_MEMBERS			dsP_MHSattributeType(\x03)
#define OMP_O_DS_A_DL_SUBMIT_PERMS		dsP_MHSattributeType(\x04)
#define OMP_O_DS_A_MESSAGE_STORE		dsP_MHSattributeType(\x05)
#define OMP_O_DS_A_OR_ADDRESSES			dsP_MHSattributeType(\x06)
#define OMP_O_DS_A_PREF_DELIV_METHODS		dsP_MHSattributeType(\x07)
#define OMP_O_DS_A_SUPP_AUTO_ACTIONS		dsP_MHSattributeType(\x08)
#define OMP_O_DS_A_SUPP_CONTENT_TYPES		dsP_MHSattributeType(\x09)
#define OMP_O_DS_A_SUPP_OPT_ATTRIBUTES		dsP_MHSattributeType(\x0A)

/* Directory object classes */
#define OMP_O_DS_O_MHS_DISTRIBUTION_LIST	dsP_MHSobjectClass(\x00)
#define OMP_O_DS_O_MHS_MESSAGE_STORE		dsP_MHSobjectClass(\x01)
#define OMP_O_DS_O_MHS_MESSAGE_TRANS_AG		dsP_MHSobjectClass(\x02)
#define OMP_O_DS_O_MHS_USER			dsP_MHSobjectClass(\x03)
#define OMP_O_DS_O_MHS_USER_AG			dsP_MHSobjectClass(\x04)

/* OM class names */
#define OMP_O_DS_C_DL_SUBMIT_PERMS		dsP_mdup_c(\x87\x05)

/* OM attribute names */
#define DS_PERM_TYPE				((OM_type) 901 )
#define DS_INDIVIDUAL				((OM_type) 902 )
#define DS_MEMBER_OF_DL				((OM_type) 903 )
#define DS_PATTERN_MATCH			((OM_type) 904 )
#define DS_MEMBER_OF_GROUP			((OM_type) 905 )

/* DS_Permission_Type */
enum DS_Permission_Type {
	DS_PERM_INDIVIDUAL = 0,
	DS_PERM_MEMBER_OF_DL = 1,
	DS_PERM_PATTERN_MATCH = 2,
	DS_PERM_MEMBER_OF_GROUP = 3
};

#endif				/* XDSMDUP_HEADER */

#endif				/* __XDSMDUP_H__ */
