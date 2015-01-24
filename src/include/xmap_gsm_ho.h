/*****************************************************************************

 @(#) src/include/xmap_gsm_ho.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

#ifndef __XMAP_GSM_HO_H__
#define __XMAP_GSM_HO_H__

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-ho(3) }
 */
#define OMP_O_MAP_GSM_HO_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x03"

/* Intermediate object identifier macro: */
#define mapP_gsm_ho(X) (OMP_O_MAP_GSM_HO_PKG# #X)

/* OM class names (prefixed MAP_C_) */
/* Service-Error parameter classes */
#define OMP_O_MAP_C_TARGET_CELL_OUTSIDE_GCA_PARAM		mapP_gsm(\x97\x39) /* 3001 */

/* handover operations */
#define OMP_O_MAP_C_PREPARE_HO_ARG				
#define OMP_O_MAP_C_PREPARE_HO_RES				
#define OMP_O_MAP_C_SEND_END_SIGNAL_ARG				
#define OMP_O_MAP_C_SEND_END_SIGNAL_RES				
#define OMP_O_MAP_C_PROCESS_ACCESS_SIGNALLING_ARG		
#define OMP_O_MAP_C_FORWARD_ACCESS_SIGNALLING_ARG		
#define OMP_O_MAP_C_PREPARE_SUBSEQUENT_HO_ARG			
#define OMP_O_MAP_C_PREPARE_SUBSEQUENT_HO_RES			

/* OM attribute names (prefixed MAP_) */

/* Service-Error problem values: */
#define MAP_E_NO_HANDOVER_NUMBER_AVAILABLE			mapP_problem(25)
#define MAP_E_SUBSEQUENT_HANDOVER_FAILURE			mapP_problem(26)
#define MAP_E_TARGET_CELL_OUTSIDE_GROUP_CALL_AREA		mapP_problem(42)


#endif				/* __XMAP_GSM_HO_H__ */

