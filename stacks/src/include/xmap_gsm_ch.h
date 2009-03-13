/*****************************************************************************

 @(#) $Id: xmap_gsm_ch.h,v 0.9.2.1 2009-03-13 11:20:25 brian Exp $

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

 $Log: xmap_gsm_ch.h,v $
 Revision 0.9.2.1  2009-03-13 11:20:25  brian
 - doc and header updates

 *****************************************************************************/

#ifndef __XMAP_GSM_CH_H__
#define __XMAP_GSM_CH_H__

#ident "@(#) $RCSfile: xmap_gsm_ch.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-ch(5) }
 */
#define OMP_O_MAP_GSM_CH_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x05"

/* OM class names (prefixed MAP_C_) */
/* Service-Error parameter classes: */
#define OMP_O_MAP_C_ABSENT_SUBSCRIBER_PARAM			mapP_gsm_ch(\xA7\x09) /* 5001 */
#define OMP_O_MAP_C_BUSY_SUBSCRIBER_PARAM			mapP_gsm_ch(\xA7\x0A) /* 5002 */
#define OMP_O_MAP_C_CALL_BARRED_PARAM				mapP_gsm_ch(\xA7\x0B) /* 5003 */
#define OMP_O_MAP_C_CUG_REJECT_PARAM				mapP_gsm_ch(\xA7\x0C) /* 5004 */
#define OMP_O_MAP_C_FORWARDING_FAILED_PARAM			mapP_gsm_ch(\xA7\x0D) /* 5005 */
#define OMP_O_MAP_C_FORWARDING_VIOLATION_PARAM			mapP_gsm_ch(\xA7\x0E) /* 5006 */
#define OMP_O_MAP_C_NO_ROAMING_NB_PARAM				mapP_gsm_ch(\xA7\x0F) /* 5007 */
#define OMP_O_MAP_C_NO_SUBSCRIBER_REPLY_PARAM			mapP_gsm_ch(\xA7\x10) /* 5008 */
#define OMP_O_MAP_C_OR_NOT_ALLOWED_PARAM			mapP_gsm_ch(\xA7\x11) /* 5009 */

#define OMP_O_MAP_C_SEND_ROUTING_INFO_ARG			
#define OMP_O_MAP_C_SEND_ROUTING_INFO_RES			
#define OMP_O_MAP_C_PROVIDE_ROAMING_NUMBER_ARG			
#define OMP_O_MAP_C_PROVIDE_ROAMING_NUMBER_RES			
#define OMP_O_MAP_C_RESUME_CALL_HANDLING_ARG			
#define OMP_O_MAP_C_RESUME_CALL_HANDLING_RES			
#define OMP_O_MAP_C_SET_REPORTING_STATE_ARG			
#define OMP_O_MAP_C_SET_REPORTING_STATE_RES			
#define OMP_O_MAP_C_STATUS_REPORT_ARG				
#define OMP_O_MAP_C_STATUS_REPORT_RES				
#define OMP_O_MAP_C_REMOTE_USER_FREE_ARG			
#define OMP_O_MAP_C_REMOTE_USER_FREE_RES			
#define OMP_O_MAP_C_IST_ALERT_ARG				
#define OMP_O_MAP_C_IST_ALERT_RES				
#define OMP_O_MAP_C_IST_COMMAND_ARG				
#define OMP_O_MAP_C_IST_COMMAND_RES				
#define OMP_O_MAP_C_RELEASE_RESOURCES_ARG			
#define OMP_O_MAP_C_RELEASE_RESOURCES_RES			

/* OM attribute names (prefixed MAP_) */
#define MAP_ABSENT_SUBSCRIBER_REASON				((OM_type)5001)
#define MAP_CALL_BARRING_CAUSE					((OM_type)5002)
#define MAP_CCBS_BUSY						((OM_type)5003)
#define MAP_CCBS_POSSIBLE					((OM_type)5004)
#define MAP_CUG_REJECT_CAUSE					((OM_type)5005)
#define MAP_UNAUTHORIZED_MESSAGE_ORIGINATOR			((OM_type)5006)

/* Service-Errors */
#define MAP_E_ABSENT_SUBSCRIBER					mapP_problem(27)
#define MAP_E_BUSY_SUBSCRIBER					mapP_problem(45)
#define MAP_E_CALL_BARRED					mapP_problem(13)
#define MAP_E_CUG_REJECT					mapP_problem(15)
#define MAP_E_FORWARDING_FAILED					mapP_problem(47)
#define MAP_E_FORWARDING_VIOLATION				mapP_problem(14)
#define MAP_E_NO_ROAMING_NUMBER_AVAILABLE			mapP_problem(39)
#define MAP_E_NO_SUBSCRIBER_REPLY				mapP_problem(46)
#define MAP_E_OR_NOT_ALLOWED					mapP_problem(48)

/* MAP_T_Absent_Subscriber_Reason: */
typedef enum {
	MAP_T_IMSI_DETACH = 0,
	MAP_T_RESTRICTED_AREA,
	MAP_T_NO_PAGE_RESPONSE,
	MAP_T_MS_PURGED,
} MAP_T_Absent_Subscriber_Reason;

/* MAP_T_Call_Barring_Cause: */
typedef enum {
	MAP_T_BARRING_SERVICE_ACTIVE = 0,
	MAP_T_OPERATOR_BARRING,
} MAP_T_Call_Barring_Cause;

/* MAP_T_CUG_Reject_Cause: */
typedef enum {
	MAP_T_INCOMING_CALLS_BARRED_WITHIN_CUG = 0,
	MAP_T_SUBSCRIBER_NOT_MEMBER_OF_CUG,
	MAP_T_REQUESTED_BASIC_SERVICE_VIOLATES_CUG_CONSTRAINTS = 5,
	MAP_T_CALLED_PARTY_SS_INTERACTION_VIOLATION = 7,
} MAP_T_CUG_Reject_Cause;

#endif				/* __XMAP_GSM_CH_H__ */

