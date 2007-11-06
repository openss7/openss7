/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __QUIPU_UIPS_DE_DEMANIFEST_H__
#define __QUIPU_UIPS_DE_DEMANIFEST_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* template.c - your comments here */

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/de/RCS/demanifest.h,v 9.0 1992/06/16 12:45:59 isode Rel
 *
 *
 * Log: demanifest.h,v
 * Revision 9.0  1992/06/16  12:45:59  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef _demanifest_h
#define _demanifest_h

#define INDENTON 1
#define INDENTOFF 0

#define NUMBER_ALLOWED 1
#define NUMBER_NOT_ALLOWED 0

#define PERSON 1
#define ORGUNIT 2
#define ORG 3
#define LOCALITY 4
#define COUNTRY 5
#define MAXTYPE 5

#define SEARCH_OK "ok"
#define SEARCH_ERROR "error"
#define SEARCH_FAIL "failed"

#define LIST_OK "ok"
#define LIST_ERROR "error"

#define QUERY_ERROR 1
#define NAME_PRINTED 2
#define LIST_PRINTED 3
#define NO_DEPT_FOUND 4
#define PARENT_PRINTED 5
#define NO_ORG_ENTERED 6
#define START_NEW_QUERY 7

#define SF_ABANDONED 1
#define SF_ACCRIGHT 2
#define SF_OPFAIL 3

#endif				/* _demanifest_h */

#endif				/* __QUIPU_UIPS_DE_DEMANIFEST_H__ */
