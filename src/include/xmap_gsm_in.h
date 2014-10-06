/*****************************************************************************

 @(#) $Id: xmap_gsm_in.h,v 1.1.2.2 2010-11-28 14:21:44 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2010-11-28 14:21:44 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm_in.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:44  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:23:46  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __XMAP_GSM_IN_H__
#define __XMAP_GSM_IN_H__

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-in(2) }
 */
#define OMP_O_MAP_GSM_IN_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x02"

/* Intermediate object identifier macro: */
#define mapP_gsm_in(X) (OMP_O_MAP_GSM_IN_PKG# #X)

/* OM class names (prefixed MAP_C_) */
#define OMP_O_MAP_C_NUMBER_CHANGE_PARAM				mapP_gsm_in(\x8F\x51) /* 2001 */
#define OMP_O_MAP_C_UNIDENTIFIED_SUB_PARAM			mapP_gsm_in(\x8F\x52) /* 2002 */
#define OMP_O_MAP_C_UNKNOWN_SUBSCRIBER_PARAM			mapP_gsm_in(\x8F\x53) /* 2003 */

/* OM attribute names (prefixed MAP_) */
#define MAP_UNKNOWN_SUBSCRIBER_DIAGNOSTIC			((OM_type)2001)

/* Service-Error problem values: */
#define MAP_E_UNKNOWN_SUBSCRIBER				mapP_problem( 1)
#define MAP_E_NUMBER_CHANGED					mapP_problem(44)
#define MAP_E_UNKNOWN_MSC					mapP_problem( 3)
#define MAP_E_UNIDENTIFIED_SUBSCRIBER				mapP_problem( 5)
#define MAP_E_UNKNOWN_EQUIPMENT					mapP_problem( 7)

#if 0
/* MAP_T_Unknown_Subscriber_Diagnostic: */
typedef enum {
	MAP_T_IMSI_UNKNOWN = 0,
	MAP_T_GPRS_SUBSCRIPTION_UNKNOWN,
	MAP_T_NPDB_MISMATCH,
} MAP_T_Unknown_Subscriber_Diagnostic;
#endif

#endif				/* __XMAP_GSM_IN_H__ */

