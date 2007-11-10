/*****************************************************************************

 @(#) $Id: xmp_snmp.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xmp_snmp.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XMP_SNMP_H__
#define __XMP_SNMP_H__

#ident "@(#) $RCSfile: xmp_snmp.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xmp_snmp.h> header declares the interface functions, the structures passed to and from those
 * functions, and the defined constants used by the functions and structures.
 *
 * All application programs which include this header must first include the OSI-Abstract-Data
 * Manipulation header <xom.h>.
 */
#include <xom.h>

/*
 * All Object Identifiers are represented by constants defined in the headers.  These constants are
 * used with the macros defined in the XOM API (see reference XOM). A constant is defined to
 * represent the Object Identifier of the SNMP Management Service package:
 */
#define OMP_O_MP_SNMP_PKG "\x2a\x86\x3a\x00\x88\x1a\x06\x03"

/* Defined constants */
/* Intermediate object identifier macro */

#define mpP_snmp(X) (OMP_O_MP_SNMP_PKG# #X)

/* OM class names (prefixed MP_C_) */

/*
 * Every application program which makes use of a class or other Object Identifier must explicitly
 * import it into every compilation unit (C source program) which uses it. Each such class or Object
 * Identifier name must be explicitly exported from just one compilation unit.
 *
 * In the header file, OM class constants are prefixed with the OPM_O prefix to denote that they are
 * OM classes. However, when using the OM_IMPORT and OM_EXPORT macros, the base names (without the
 * OMP_O prefix) should be used.  For example:
 * 
 * OM_IMPORT(MP_C_OBJECT_SYNTAX)
 */

#define OMP_O_MP_C_APPLICATION_SYNTAX	mpP_snmp(\x97\x39)
#define OMP_O_MP_C_OBJECT_SYNTAX	mpP_snmp(\x97\x3A)
#define OMP_O_MP_C_PDU			mpP_snmp(\x97\x3B)
#define OMP_O_MP_C_PDUS			mpP_snmp(\x97\x3C)
#define OMP_O_MP_C_SIMPLE_SYNTAX	mpP_snmp(\x97\x3D)
#define OMP_O_MP_C_TRAP_PDU		mpP_snmp(\x97\x3E)
#define OMP_O_MP_C_VAR_BIND		mpP_snmp(\x97\x3F)
#define OMP_O_MP_C_VARIABLE_BINDINGS	mpP_snmp(\x97\x40)

/* The OM attribute names which are defined are listed below. */
#define MP_ADDRESS			((OM_type)11201)
#define MP_AGENT_ADDR			((OM_type)11202)
#define MP_APPLICATION_WIDE		((OM_type)11203)
#define MP_ARBITRARY			((OM_type)11204)
#define MP_COUNTER			((OM_type)11205)
#define MP_EMPTY			((OM_type)11206)
#define MP_ENTERPRISE			((OM_type)11207)
#define MP_ERROR_INDEX			((OM_type)11208)
#define MP_ERROR_STATUS			((OM_type)11209)
#define MP_GAUGE			((OM_type)11210)
#define MP_GENERIC_TRAP			((OM_type)11211)
#define MP_GET_NEXT_REQUEST		((OM_type)11212)
#define MP_GET_REQUEST			((OM_type)11213)
#define MP_GET_RESPONSE			((OM_type)11214)
#define MP_NAME				((OM_type)11215)
#define MP_NUMBER			((OM_type)11216)
#define MP_OBJECT			((OM_type)11217)
#define MP_REQUEST_ID			((OM_type)11218)
#define MP_SET_REQUEST			((OM_type)11219)
#define MP_SIMPLE			((OM_type)11220)
#define MP_SPECIFIC_TRAP		((OM_type)11221)
#define MP_STRING			((OM_type)11222)
#define MP_TICKS			((OM_type)11223)
#define MP_TIME_STAMP			((OM_type)11224)
#define MP_TRAP				((OM_type)11225)
#define MP_VALUE			((OM_type)11226)
#define MP_VAR_BIND			((OM_type)11227)
#define MP_VARIABLE_BINDINGS		((OM_type)11228)

/*
 * The following enumeration tags and enumeration constants are defined for use as values of the
 * corresponding OM attributes:
 */
/* MP_T_Generic_Trap: */
#define MP_T_AUTHENTICATION_FAILURE	1
#define MP_T_COLD_START			2
#define MP_T_EGP_NEIGHBOR_LOSS		3
#define MP_T_ENTERPRISE_SPECIFIC	4
#define MP_T_LINK_DOWN			5
#define MP_T_LINK_UP			6
#define MP_T_WARM_START			7

/* MP_E_Problem: */
#define MP_E_BAD_VALUE			3001
#define MP_E_GEN_ERR			3002
#define MP_E_NO_SUCH_NAME		3003
#define MP_E_READ_ONLY			3004
#define MP_E_TOO_BIG			3005

#endif				/* __XMP_SNMP_H__ */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
