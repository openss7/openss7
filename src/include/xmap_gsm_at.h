/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __XMAP_GSM_AT_H__
#define __XMAP_GSM_AT_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2008-2009 Monavacon Limited."

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-at(6) }
 */
#define OMP_O_MAP_GSM_AT_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x06"

/* Intermediate object identifier macro: */
#define mapP_gsm_at(X) (OMP_O_MAP_GSM_AT_PKG# #X)

/* OM class names (prefixed MAP_C_) */
/* Service-Error parameter classes: */
#define OMP_O_MAP_C_ATI_NOT_ALLOWED_PARAM			mapP_gsm_at(\xAE\x31) /* 6001 */
#define OMP_O_MAP_C_ATM_NOT_ALLOWED_PARAM			mapP_gsm_at(\xAE\x32) /* 6002 */
#define OMP_O_MAP_C_ATSI_NOT_ALLOWED_PARAM			mapP_gsm_at(\xAE\x33) /* 6003 */
#define OMP_O_MAP_C_INFORMATION_NOT_AVAILABLE_PARAM		mapP_gsm_at(\xAE\x34) /* 6004 */

/* any time information enquiry operations */
#define OMP_O_MAP_C_ANY_TIME_INTERROGATION_ARG			
#define OMP_O_MAP_C_ANY_TIME_INTERROGATION_RES			

/* any time infomration handling operations */
#define OMP_O_MAP_C_ANY_TIME_SUBSCRIPTION_INTERROGATION_ARG	
#define OMP_O_MAP_C_ANY_TIME_SUBSCRIPTION_INTERROGATION_RES	
#define OMP_O_MAP_C_ANY_TIME_MODIFICATION_ARG			
#define OMP_O_MAP_C_ANY_TIME_MODIFICATION_RES			

/* OM attribute names (prefixed MAP_) */

/* Service-Error problem values: */
#define MAP_E_ATI_NOT_ALLOWED					mapP_problem(49)
#define MAP_E_ATM_NOT_ALLOWED					mapP_problem(61)
#define MAP_E_ATSI_NOT_ALLOWED					mapP_problem(60)
#define MAP_E_INFORMATION_NOT_AVAILABLE				mapP_problem(62)


#endif				/* __XMAP_GSM_AT_H__ */

