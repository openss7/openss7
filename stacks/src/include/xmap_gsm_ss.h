/*****************************************************************************

 @(#) $Id: xmap_gsm_ss.h,v 0.9.2.1 2009-03-13 11:20:25 brian Exp $

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

 Last Modified $Date: 2009-03-13 11:20:25 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm_ss.h,v $
 Revision 0.9.2.1  2009-03-13 11:20:25  brian
 - doc and header updates

 *****************************************************************************/

#ifndef __XMAP_GSM_SS_H__
#define __XMAP_GSM_SS_H__

#ident "@(#) $RCSfile: xmap_gsm_ss.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-ss(8) }
 */
#define OMP_O_MAP_GSM_SS_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x08"

/* Intermediate object identifier macro: */
#define mapP_gsm_ss(X) (OMP_O_MAP_GSM_SS_PKG# #X)

/* OM class names (prefixed MAP_C_) */
/* Service-Error parameter classes: */
#define OMP_O_MAP_C_ILLEGAL_SS_OPERATION_PARAM			mapP_gsm_ss(\xBE\x41) /* 8001 */
#define OMP_O_MAP_C_LONG_TERM_DENIAL_PARAM			mapP_gsm_ss(\xBE\x42) /* 8002 */
#define OMP_O_MAP_C_PW_REGISTRATION_FAILURE_CAUSE		mapP_gsm_ss(\xBE\x43) /* 8003 */
#define OMP_O_MAP_C_SHORT_TERM_DENIAL_PARAM			mapP_gsm_ss(\xBE\x44) /* 8004 */
#define OMP_O_MAP_C_SS_INCOMPATIBILITY_PARAM			mapP_gsm_ss(\xBE\x45) /* 8005 */
#define OMP_O_MAP_C_SS_NOT_AVAILABLE_PARAM			mapP_gsm_ss(\xBE\x46) /* 8006 */
#define OMP_O_MAP_C_SS_STATUS					mapP_gsm_ss(\xBE\x47) /* 8007 */
#define OMP_O_MAP_C_SS_SUBSCRIPTION_VIOLATION_PARAM		mapP_gsm_ss(\xBE\x48) /* 8008 */

#define OMP_O_MAP_C_BASIC_SERVICE_CODE				mapP_gsm_ss(\xBE\x49) /* 8009 */

#define OMP_O_MAP_C_REGISTER_SS_ARG				
#define OMP_O_MAP_C_REGISTER_SS_RES				
#define OMP_O_MAP_C_ERASE_SS_ARG				
#define OMP_O_MAP_C_ERASE_SS_RES				
#define OMP_O_MAP_C_ACTIVATE_SS_ARG				
#define OMP_O_MAP_C_ACTIVATE_SS_RES				
#define OMP_O_MAP_C_DEACTIVATE_SS_ARG				
#define OMP_O_MAP_C_DEACTIVATE_SS_RES				
#define OMP_O_MAP_C_INTERROGATE_SS_ARG				
#define OMP_O_MAP_C_INTERROGATE_SS_RES				
#define OMP_O_MAP_C_PROCESS_USSD_ARG				
#define OMP_O_MAP_C_PROCESS_USSD_RES				
#define OMP_O_MAP_C_USSD_ARG					
#define OMP_O_MAP_C_USSD_RES					
#define OMP_O_MAP_C_NOTIFY_USSD_ARG				
#define OMP_O_MAP_C_NOTIFY_USSD_RES				
#define OMP_O_MAP_C_REGISTER_PASSWORD_ARG			
#define OMP_O_MAP_C_REGISTER_PASSWORD_RES			
#define OMP_O_MAP_C_GET_PASSWORD_ARG				
#define OMP_O_MAP_C_GET_PASSWORD_RES				
#define OMP_O_MAP_C_SS_INVOCATION_NOTIFICATION_ARG		
#define OMP_O_MAP_C_SS_INVOCATION_NOTIFICATION_RES		
#define OMP_O_MAP_C_REGISTER_CC_ENTRY_ARG			
#define OMP_O_MAP_C_REGISTER_CC_ENTRY_RES			
#define OMP_O_MAP_C_ERASE_CC_ENTRY_ARG				
#define OMP_O_MAP_C_ERASE_CC_ENTRY_RES				

/* OM attribute names (prefixed MAP_) */
#define MAP_BEARER_SERVICE					((OM_type)8001)
#define MAP_TELESERVICE						((OM_type)8002)
#define MAP_SS_CODE						((OM_type)8003)
#define MAP_BASIC_SERVICE					((OM_type)8004)
#define MAP_SS_STATUS						((OM_type)8005)
#define MAP_PW_REGISTRATION_FAILURE_CAUSE			((OM_type)8006)

/* Service-Error problem values: */
#define MAP_E_ILLEGAL_SS_OPERATION				mapP_problem(16)
#define MAP_E_LONG_TERM_DENIAL					mapP_problem(30)
#define MAP_E_NEGATIVE_PW_CHECK					mapP_problem(38)
#define MAP_E_NUMBER_OF_PW_ATTEMPTS_VIOLATION			mapP_problem(43)
#define MAP_E_PW_REGISTRATION_FAILURE				mapP_problem(37)
#define MAP_E_SHORT_TERM_DENIAL					mapP_problem(29)
#define MAP_E_SS_ERROR_STATUS					mapP_problem(17)
#define MAP_E_SS_INCOMPATIBILITY				mapP_problem(20)
#define MAP_E_SS_NOT_AVAILABLE					mapP_problem(18)
#define MAP_E_SS_SUBSCRIPTION_VIOLATION				mapP_problem(19)
#define MAP_E_UNKNOWN_ALPHABET					mapP_problem(71)
#define MAP_E_USSD_BUSY						mapP_problem(72)

/* MAP_T_PW_Registration_Failure_Cause: */
typedef enum {
	MAP_T_UNDETERMINED = 0,
	MAP_T_INVALID_FORMAT,
	MAP_T_NEW_PASSWORDS_MISMATCH,
} MAP_T_PW_Registration_Failure_Cause;


#endif				/* __XMAP_GSM_SS_H__ */

