/*****************************************************************************

 @(#) $Id: xmsima.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xmsima.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XMSIMA_H__
#define __XMSIMA_H__

#ident "@(#) $RCSfile: xmsima.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * The <xmsima.h> header defines MS_IM_ATTRIBUTES_PACKAGE, the constant value
 * for the MS Interpersonal Messaging Package. It also defines the object
 * identifiers of the MS Interpersonal Messaging Attribute types supported by
 * this package (see Chapter 8).
 *
 * All client programs that include this header must first include the Object
 * Management header <xom.h> (see reference XPOM) and the <xms.h> header.
 */

/*
 * xmsima.h (Message Store Interpersonal Messaging Attributes Package)
 */
#ifndef XMSIMA_HEADER
#define XMSIMA_HEADER

/* MS Interpersonal Messaging Attributes Package object identifier */
#define OMP_O_MS_IM_ATTRIBUTES_PACKAGE "\x56\x06\x01\x02\x06\x03"

/* MS Interpersonal Messaging Attributes Types */

/*
 * Note: Every client program must explicitly import into every compilation unit
 * (C source program) the classes or Object Identifiers that it uses. Each of
 * these classes or Object Identifier names must then be explicitly exported
 * from just one compilation unit.
 *
 * Importing and exporting can be done using the OM_IMPORT and OM_EXPORT macros
 * respectively (see the XOM API Specification).  For instance, the client
 * program uses OM_IMPORT( MS_IM_ACKNOWLEDGMENT_MODE) which in turn will make
 * use of OMP_O_MS_IM_ACKNOWLEDGMENT_MODE defined below.
 */
#define OMP_O_MS_IM_A_ACKNOWLEDGMENT_MODE	"\x56\x01\x09\x09"
#define OMP_O_MS_IM_A_AUTHORIZING_USERS		"\x56\x01\x07\x0A"
#define OMP_O_MS_IM_A_AUTO_FORWARD_COMMENT	"\x56\x01\x09\x06"
#define OMP_O_MS_IM_A_AUTO_FORWARDED		"\x56\x01\x07\x09"
#define OMP_O_MS_IM_A_BILATERAL_DEF_BODY_PARTS	"\x56\x01\x08\x0A"
#define OMP_O_MS_IM_A_BLIND_COPY_RECIPIENTS	"\x56\x01\x07\x0D"
#define OMP_O_MS_IM_A_BODY			"\x56\x01\x08\x00"
#define OMP_O_MS_IM_A_CONVERSION_EITS		"\x56\x01\x09\x03"
#define OMP_O_MS_IM_A_COPY_RECIPIENTS		"\x56\x01\x07\x0C"
#define OMP_O_MS_IM_A_DISCARD_REASON		"\x56\x01\x09\x05"
#define OMP_O_MS_IM_A_EXPIRY_TIME		"\x56\x01\x07\x05"
#define OMP_O_MS_IM_A_EXTENDED_BODY_PART_TYPES	"\x56\x01\x08\x0C"
#define OMP_O_MS_IM_A_G3_FAX_BODY_PARTS		"\x56\x01\x08\x03"
#define OMP_O_MS_IM_A_G3_FAX_DATA		"\x56\x01\x08\x16"
#define OMP_O_MS_IM_A_G3_FAX_PARAMETERS		"\x56\x01\x08\x0F"
#define OMP_O_MS_IM_A_G4_CLASS1_BODY_PARTS	"\x56\x01\x08\x04"
#define OMP_O_MS_IM_A_HEADING			"\x56\x01\x07\x00"
#define OMP_O_MS_IM_A_IA5_TEXT_BODY_PARTS	"\x56\x01\x08\x01"
#define OMP_O_MS_IM_A_IA5_TEXT_DATA		"\x56\x01\x08\x14"
#define OMP_O_MS_IM_A_IA5_TEXT_PARAMETERS	"\x56\x01\x08\x0D"
#define OMP_O_MS_IM_A_IMPORTANCE		"\x56\x01\x07\x07"
#define OMP_O_MS_IM_A_INCOMPLETE_COPY		"\x56\x01\x07\x11"
#define OMP_O_MS_IM_A_IPM_ENTRY_TYPE		"\x56\x01\x06\x00"
#define OMP_O_MS_IM_A_IPM_PREFERRED_RECIPIENT	"\x56\x01\x09\x02"
#define OMP_O_MS_IM_A_IPM_SYNOPSIS		"\x56\x01\x06\x01"
#define OMP_O_MS_IM_A_IPN_ORIGINATOR		"\x56\x01\x09\x01"
#define OMP_O_MS_IM_A_LANGUAGES			"\x56\x01\x07\x12"
#define OMP_O_MS_IM_A_MESSAGE_BODY_PARTS	"\x56\x01\x08\x08"
#define OMP_O_MS_IM_A_MESSAGE_DATA		"\x56\x01\x08\x1A"
#define OMP_O_MS_IM_A_MESSAGE_PARAMETERS	"\x56\x01\x08\x13"
#define OMP_O_MS_IM_A_MIXED_MODE_BODY_PARTS	"\x56\x01\x08\x09"
#define OMP_O_MS_IM_A_NATIONAL_DEF_BODY_PARTS	"\x56\x01\x08\x0B"
#define OMP_O_MS_IM_A_NON_RECEIPT_REASON	"\x56\x01\x09\x04"
#define OMP_O_MS_IM_A_NRN_REQUESTORS		"\x56\x01\x07\x14"
#define OMP_O_MS_IM_A_OBSOLETED_IPMS		"\x56\x01\x07\x0E"
#define OMP_O_MS_IM_A_ORIGINATOR		"\x56\x01\x07\x02"
#define OMP_O_MS_IM_A_PRIMARY_RECIPIENTS	"\x56\x01\x07\x0B"
#define OMP_O_MS_IM_A_RECEIPT_TIME		"\x56\x01\x09\x08"
#define OMP_O_MS_IM_A_RELATED_IPMS		"\x56\x01\x07\x0F"
#define OMP_O_MS_IM_A_REPLIED_TO_IPM		"\x56\x01\x07\x03"
#define OMP_O_MS_IM_A_REPLY_RECIPIENTS		"\x56\x01\x07\x10"
#define OMP_O_MS_IM_A_REPLY_REQUESTORS		"\x56\x01\x07\x15"
#define OMP_O_MS_IM_A_REPLY_TIME		"\x56\x01\x07\x06"
#define OMP_O_MS_IM_A_RETURNED_IPM		"\x56\x01\x09\x07"
#define OMP_O_MS_IM_A_RN_REQUESTORS		"\x56\x01\x07\x13"
#define OMP_O_MS_IM_A_SENSITIVITY		"\x56\x01\x07\x08"
#define OMP_O_MS_IM_A_SUBJECT			"\x56\x01\x07\x04"
#define OMP_O_MS_IM_A_SUBJECT_IPM		"\x56\x01\x09\x00"
#define OMP_O_MS_IM_A_SUPPL_RECEIPT_INFO	"\x56\x01\x09\x0A"
#define OMP_O_MS_IM_A_TELETEX_BODY_PARTS	"\x56\x01\x08\x05"
#define OMP_O_MS_IM_A_TELETEX_DATA		"\x56\x01\x08\x17"
#define OMP_O_MS_IM_A_TELETEX_PARAMETERS	"\x56\x01\x08\x10"
#define OMP_O_MS_IM_A_THIS_IPM			"\x56\x01\x07\x01"
#define OMP_O_MS_IM_A_VIDEOTEX_BODY_PARTS	"\x56\x01\x08\x06"
#define OMP_O_MS_IM_A_VIDEOTEX_DATA		"\x56\x01\x08\x18"
#define OMP_O_MS_IM_A_VIDEOTEX_PARAMETERS	"\x56\x01\x08\x11"

/* Classes in the MS IM Attributes Package */
#define OMP_O_MS_IM_C_BODY			"\x56\x06\x01\x02\x06\x03\x01"
#define OMP_O_MS_IM_C_G3_FAX_DATA		"\x56\x06\x01\x02\x06\x03\x02"
#define OMP_O_MS_IM_C_BODYPART_SYNOPSIS		"\x56\x06\x01\x02\x06\x03\x03"
#define OMP_O_MS_IM_C_HEADING			"\x56\x06\x01\x02\x06\x03\x04"
#define OMP_O_MS_IM_C_IPM_SYNOPSIS		"\x56\x06\x01\x02\x06\x03\x05"
#define OMP_O_MS_IM_C_MSG_BODYPART_SYNOPSIS	"\x56\x06\x01\x02\x06\x03\x06"
#define OMP_O_MS_IM_C_NON_MSG_BODYPART_SYNOPSIS	"\x56\x06\x01\x02\x06\x03\x07"
#define OMP_O_MS_IM_C_TELETEX_DATA		"\x56\x06\x01\x02\x06\x03\x08"
#define OMP_O_MS_IM_C_TELETEX_PARAMETERS	"\x56\x06\x01\x02\x06\x03\x09"

/* OM Attribute Names in the MS IM Attributes Package */
#define MS_IM_AUTHORIZING_USERS			((OM_type) 1401)
#define MS_IM_AUTO_FORWARDED			((OM_type) 1402)
#define MS_IM_BLIND_COPY_RECIPIENTS		((OM_type) 1403)
#define MS_IM_BODY_PART				((OM_type) 1404)
#define MS_IM_BODY_PART_SYNOPSIS		((OM_type) 1405)
#define MS_IM_COPY_RECIPIENTS			((OM_type) 1406)
#define MS_IM_EXPIRY_TIME			((OM_type) 1407)
#define MS_IM_EXTENSIONS			((OM_type) 1408)
#define MS_IM_IMAGES				((OM_type) 1409)
#define MS_IM_IMPORTANCE			((OM_type) 1410)
#define MS_IM_MESSAGE_BODY_PART_SYNOPSIS	((OM_type) 1411)
#define MS_IM_NON_MESSAGE_BODY_PART_SYNOPSIS	((OM_type) 1412)
#define MS_IM_NUMBER_OF_PAGES			((OM_type) 1413)
#define MS_IM_OBSOLETED_IPMS			((OM_type) 1414)
#define MS_IM_ORIGINATOR			((OM_type) 1415)
#define MS_IM_PARAMETERS			((OM_type) 1416)
#define MS_IM_PRIMARY_RECIPIENTS		((OM_type) 1417)
#define MS_IM_PROCESSED				((OM_type) 1418)
#define MS_IM_RELATED_IPMS			((OM_type) 1419)
#define MS_IM_REPLIED_TO_IPM			((OM_type) 1420)
#define MS_IM_REPLY_RECIPIENTS			((OM_type) 1421)
#define MS_IM_REPLY_TIME			((OM_type) 1422)
#define MS_IM_SENSITIVITY			((OM_type) 1423)
#define MS_IM_SEQUENCE_NUMBER			((OM_type) 1424)
#define MS_IM_SIZE				((OM_type) 1425)
#define MS_IM_SUBJECT				((OM_type) 1426)
#define MS_IM_SYNOPSIS				((OM_type) 1427)
#define MS_IM_TELETEX_DOCUMENT			((OM_type) 1428)
#define MS_IM_THIS_IPM				((OM_type) 1429)
#define MS_IM_TYPE				((OM_type) 1430)

/* Enumeration Constants */

/* for MS_IM_ACKNOWLEDGEMENT_MODE */
#define MS_AM_AUTOMATIC				((OM_enumeration) 0)
#define MS_AM_MANUAL				((OM_enumeration) 1)

/* for MS_IM_DISCARD_REASON */
#define MS_DR_NO_DISCARD			((OM_enumeration) -1)
#define MS_DR_IPM_EXPIRED			((OM_enumeration) 0)
#define MS_DR_IPM_OBSOLETED			((OM_enumeration) 1)
#define MS_DR_USER_TERMINATED			((OM_enumeration) 2)

/* MS_IM_IA5_REPERTOIRE */
#define MS_IR_IA5				((OM_enumeration) 2)
#define MS_IR_ITA2				((OM_enumeration) 5)

/* MS_IM_IMPORTANCE */
#define MS_IM_LOW				((OM_enumeration) 0)
#define MS_IM_ROUTINE				((OM_enumeration) 1)
#define MS_IM_HIGH				((OM_enumeration) 2)

/* MS_IM_IPM_ENTRY_TYPE */
#define MS_IE_IPM				((OM_enumeration) 0)
#define MS_IE_RN				((OM_enumeration) 1)
#define MS_IE_NRN				((OM_enumeration) 2)

/* MS_IM_NR_REASON */
#define MS_NR_IPM_AUTO_FORWARDED		((OM_enumeration) 0)
#define MS_NR_IPM_DISCARDED			((OM_enumeration) 1)

/* MS_IM_SENSITIVITY */
#define MS_SE_NOT_SENSITIVE			((OM_enumeration) 0)
#define MS_SE_PERSONAL				((OM_enumeration) 1)
#define MS_SE_PRIVATE				((OM_enumeration) 2)
#define MS_SE_COMPANY_CONFIDENTIAL		((OM_enumeration) 3)

/* MS_IM_VIDEOTEX_SYNTAX */
#define MS_VS_UNSPECIFIED			((OM_enumeration) -1)
#define MS_VS_IDS				((OM_enumeration) 0)
#define MS_VS_DATA_SYNTAX_1			((OM_enumeration) 1)
#define MS_VS_DATA_SYNTAX_2			((OM_enumeration) 2)
#define MS_VS_DATA_SYNTAX_3			((OM_enumeration) 3)

#endif				/* XMSIMA_HEADER */

#endif				/* __XMSIMA_H__ */
