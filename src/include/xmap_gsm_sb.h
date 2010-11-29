/*****************************************************************************

 @(#) $Id: xmap_gsm_sb.h,v 1.1.2.2 2010-11-28 14:21:45 brian Exp $

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

 Last Modified $Date: 2010-11-28 14:21:45 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm_sb.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:45  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:23:46  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __XMAP_GSM_SB_H__
#define __XMAP_GSM_SB_H__

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-sb(2) }
 */
#define OMP_O_MAP_GSM_SB_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x02"

/* Intermediate object identifier macro: */
#define mapP_gsm_sb(X) (OMP_O_MAP_GSM_SB_PKG# #X)

/* OM class names (prefixed MAP_C_) */
/* Service-Error parameter classes: */
#define OMP_O_MAP_C_BEARER_SERV_NOT_PROV_PARAM			mapP_gsm_sb(\x8F\x51) /* 2001 */
#define OMP_O_MAP_C_ILLEGAL_EQUIPMENT_PARAM			mapP_gsm_sb(\x8F\x52) /* 2002 */
#define OMP_O_MAP_C_ILLEGAL_SUBSCRIBER_PARAM			mapP_gsm_sb(\x8F\x53) /* 2003 */
#define OMP_O_MAP_C_ROAMING_NOT_ALLOWED_PARAM			mapP_gsm_sb(\x8F\x54) /* 2004 */
#define OMP_O_MAP_C_TELESERV_NOT_PROV_PARAM			mapP_gsm_sb(\x8F\x55) /* 2005 */

/* subscriber information enquiry operations */
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_INFO_ARG			
#define OMP_O_MAP_C_PROVIDE_SUBSCRIBER_INFO_RES			

/* subscriber data modification notification operations */
#define OMP_O_MAP_C_NOTE_SUBSCRIBER_DATA_MODIFIED_ARG		
#define OMP_O_MAP_C_NOTE_SUBSCRIBER_DATA_MODIFIED_RES		

/* subscriber management operations */
#define OMP_O_MAP_C_INSERT_SUBSCRIBER_DATA_ARG			
#define OMP_O_MAP_C_INSERT_SUBSCRIBER_DATA_RES			
#define OMP_O_MAP_C_DELETE_SUBSCRIBER_DATA_ARG			
#define OMP_O_MAP_C_DELETE_SUBSCRIBER_DATA_RES			

/* OM attribute names (prefixed MAP_) */
#define MAP_ROAMING_NOT_ALLOWED_CAUSE				((OM_type)2001)

/* Service-Error problem values: */
#define MAP_E_ROAMING_NOT_ALLOWED				mapP_problem( 8)
#define MAP_E_ILLEGAL_SUBSCRIBER				mapP_problem( 9)
#define MAP_E_ILLEGAL_EQUIPMENT					mapP_problem(12)
#define MAP_E_BEARER_SERVICE_NOT_PROVISIONED			mapP_problem(10)
#define MAP_E_TELESERVICE_NOT_PROVISIONED			mapP_problem(11)

#if 0
/* MAP_T_Roaming_Not_Allowed_Cause: */
typedef enum {
	MAP_T_PLMN_ROAMING_NOT_ALLOWED = 0,
	MAP_T_OPERATOR_DETERMINED_BARRING = 3,
} MAP_T_Roaming_Not_Allowed_Cause;
#endif


#endif				/* __XMAP_GSM_SB_H__ */

