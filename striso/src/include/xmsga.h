/*****************************************************************************

 @(#) $Id: xmsga.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xmsga.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XMSGA_H__
#define __XMSGA_H__

#ident "@(#) $RCSfile: xmsga.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xmsga.h> header defines MS_GENERAL_ATTRIBUTES_PACKAGE, the constant
 * value for the MS General Attributes Package. It also defines the object
 * identifiers of the MS General Attribute types supported by this package (see
 * Chapter 7).
 *
 * All client programs that include this header must first include the Object
 * Management header <xom.h> (see reference XOM) and the <xms.h> header.
 */

/*
 * xmsga.h (Message Store General Attributes Package)
 */
#ifndef XMSGA_HEADER
#define XMSGA_HEADER

/* MS General Attributes Package object identifier */
#define OMP_O_MS_GENERAL_ATTRIBUTES_PACKAGE "\x56\x06\x01\x02\x06\x02"

/* MS General Attributes Types */

/*
 * Note: Every client program must explicitly import into every compilation unit
 * (C source program) the classes or Object Identifiers that it uses. Each of
 * these classes or Object Identifier names must then be explicitly exported
 * from just one compilation unit.
 *
 * Importing and exporting can be done using the OM_IMPORT and OM_EXPORT macros
 * respectively (see the XOM API Specification).
 *
 * For instance, the client program uses OM_IMPORT( MS_A_CHILD_SEQUENCE_NUMBERS
 * ) which in turn will make use of OMP_O_MS_A_CHILD_SEQUENCE_NUMBERS defined
 * below.
 */
#define OMP_O_MS_A_CHILD_SEQUENCE_NUMBERS	"\x56\x04\x03\x00"
#define OMP_O_MS_A_CONTENT			"\x56\x04\x03\x01"
#define OMP_O_MS_A_CONTENT_CONFIDENTL_ALGM_ID	"\x56\x04\x03\x02"
#define OMP_O_MS_A_CONTENT_CORRELATOR		"\x56\x04\x03\x03"
#define OMP_O_MS_A_CONTENT_IDENTIFIER		"\x56\x04\x03\x04"
#define OMP_O_MS_A_CONTENT_INTEGRITY_CHECK	"\x56\x04\x03\x05"
#define OMP_O_MS_A_CONTENT_LENGTH		"\x56\x04\x03\x06"
#define OMP_O_MS_A_CONTENT_RETURNED		"\x56\x04\x03\x07"
#define OMP_O_MS_A_CONTENT_TYPE			"\x56\x04\x03\x08"
#define OMP_O_MS_A_CONVERSION_LOSS_PROHIBITED	"\x56\x04\x03\x09"
#define OMP_O_MS_A_CONVERTED_EITS		"\x56\x04\x03\x0A"
#define OMP_O_MS_A_CREATION_TIME		"\x56\x04\x03\x0B"
#define OMP_O_MS_A_DELIVERED_EITS		"\x56\x04\x03\x0C"
#define OMP_O_MS_A_DELIVERY_FLAGS		"\x56\x04\x03\x0D"
#define OMP_O_MS_A_DL_EXPANSION_HISTORY		"\x56\x04\x03\x0E"
#define OMP_O_MS_A_ENTRY_STATUS			"\x56\x04\x03\x0F"
#define OMP_O_MS_A_ENTRY_TYPE			"\x56\x04\x03\x10"
#define OMP_O_MS_A_INTENDED_RECIPIENT_NAME	"\x56\x04\x03\x11"
#define OMP_O_MS_A_MESSAGE_DELIVERY_ENVELOPE	"\x56\x04\x03\x12"
#define OMP_O_MS_A_MESSAGE_DELIVERY_ID		"\x56\x04\x03\x13"
#define OMP_O_MS_A_MESSAGE_DELIVERY_TIME	"\x56\x04\x03\x14"
#define OMP_O_MS_A_MESSAGE_ORIGIN_AUTHEN_CHK	"\x56\x04\x03\x15"
#define OMP_O_MS_A_MESSAGE_SECURITY_LABEL	"\x56\x04\x03\x16"
#define OMP_O_MS_A_MESSAGE_SUBMISSION_TIME	"\x56\x04\x03\x17"
#define OMP_O_MS_A_MESSAGE_TOKEN		"\x56\x04\x03\x18"
#define OMP_O_MS_A_ORIGINAL_EITS		"\x56\x04\x03\x19"
#define OMP_O_MS_A_ORIGINATOR_CERTIFICATE	"\x56\x04\x03\x1A"
#define OMP_O_MS_A_ORIGINATOR_NAME		"\x56\x04\x03\x1B"
#define OMP_O_MS_A_OTHER_RECIPIENT_NAMES	"\x56\x04\x03\x1C"
#define OMP_O_MS_A_PARENT_SEQUENCE_NUMBER	"\x56\x04\x03\x1D"
#define OMP_O_MS_A_PERRECIP_REPORT_DELIV_FLDS	"\x56\x04\x03\x1E"
#define OMP_O_MS_A_PRIORITY			"\x56\x04\x03\x1F"
#define OMP_O_MS_A_PROOF_OF_DELIVERY_REQUEST	"\x56\x04\x03\x20"
#define OMP_O_MS_A_REDIRECTION_HISTORY		"\x56\x04\x03\x21"
#define OMP_O_MS_A_REPORT_DELIVERY_ENVELOPE	"\x56\x04\x03\x22"
#define OMP_O_MS_A_REPORT_ORIGIN_AUTHEN_CHK	"\x56\x04\x03\x23"
#define OMP_O_MS_A_REPORTING_DL_NAME		"\x56\x04\x03\x24"
#define OMP_O_MS_A_REPORTING_MTA_CERTIFICATE	"\x56\x04\x03\x25"
#define OMP_O_MS_A_SECURITY_CLASSIFICATION	"\x56\x04\x03\x26"
#define OMP_O_MS_A_SEQUENCE_NUMBER		"\x56\x04\x03\x27"
#define OMP_O_MS_A_SUBJECT_SUBMISSION_ID	"\x56\x04\x03\x28"
#define OMP_O_MS_A_THIS_RECIPIENT_NAME		"\x56\x04\x03\x29"

/* Enumeration Constants */

/* for MS_A_ENTRY_STATUS */
#define MS_ES_NEW				((OM_enumeration) 0)
#define MS_ES_LISTED				((OM_enumeration) 1)
#define MS_ES_PROCESSED				((OM_enumeration) 2)

/* for MS_A_ENTRY_TYPE */
#define MS_ET_DELIVERED_MESSAGE			((OM_enumeration) 0)
#define MS_ET_DELIVERED_REPORT			((OM_enumeration) 1)
#define MS_ET_RETURNED_CONTENT			((OM_enumeration) 2)

/* for MS_A_PRIORITY */
#define MS_PTY_NORMAL				((OM_enumeration) 0)
#define MS_PTY_LOW				((OM_enumeration) 1)
#define MS_PTY_URGENT				((OM_enumeration) 2)

/* for MS_A_SECURITY_CLASSIFICATION */
#define MS_SC_UNMARKED				((OM_enumeration) 0)
#define MS_SC_UNCLASSIFIED			((OM_enumeration) 1)
#define MS_SC_RESTRICTED			((OM_enumeration) 2)
#define MS_SC_CONFIDENTIAL			((OM_enumeration) 3)
#define MS_SC_SECRET				((OM_enumeration) 4)
#define MS_SC_TOP_SECRET			((OM_enumeration) 5)

#endif				/* XMSGA_HEADER */

#endif				/* __XMSGA_H__ */
