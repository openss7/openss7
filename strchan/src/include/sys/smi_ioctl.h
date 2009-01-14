/*****************************************************************************

 @(#) $Id: smi_ioctl.h,v 0.9.2.1 2009-01-14 14:31:22 brian Exp $

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

 Last Modified $Date: 2009-01-14 14:31:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: smi_ioctl.h,v $
 Revision 0.9.2.1  2009-01-14 14:31:22  brian
 - working up agents

 *****************************************************************************/

#ifndef __SYS_SMI_IOCTL_H__
#define __SYS_SMI_IOCTL_H__

#ident "@(#) $RCSfile: smi_ioctl.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

#define X721_ADMINISTRATIVESTATE_LOCKED			0
#define X721_ADMINISTRATIVESTATE_UNLOCKED		1
#define X721_ADMINISTRATIVESTATE_SHUTTINGDOWN		2

#define X721_ALARMSTATUS_UNDERREPAIR			0
#define X721_ALARMSTATUS_CRITICAL			1
#define X721_ALARMSTATUS_MAJOR				2
#define X721_ALARMSTATUS_MINOR				3
#define X721_ALARMSTATUS_ALARMOUTSTANDING		4

#define M3100_ALARMSTATUS_CRITICAL			5
#define M3100_ALARMSTATUS_MAJOR				4
#define M3100_ALARMSTATUS_MINOR				3
#define M3100_ALARMSTATUS_INDETERMINATE			1
#define M3100_ALARMSTATUS_WARNING			2
#define M3100_ALARMSTATUS_PENDING			6
#define M3100_ALARMSTATUS_CLEARED			0

#define X721_AVAILABILITYSTATUS_INTEST			0
#define X721_AVAILABILITYSTATUS_FAILED			1
#define X721_AVAILABILITYSTATUS_POWEROFF		2
#define X721_AVAILABILITYSTATUS_OFFLINE			3
#define X721_AVAILABILITYSTATUS_OFFDUTY			4
#define X721_AVAILABILITYSTATUS_DEPENDENCY		5
#define X721_AVAILABILITYSTATUS_DEGRADED		6
#define X721_AVAILABILITYSTATUS_NOTINSTALLED		7
#define X721_AVAILABILITYSTATUS_LOGFULL			8

#define X721_CONTROLSTATUS_SUBJECTTOTEST		0
#define X721_CONTROLSTATUS_PARTOFSERVICESLOCKED		1
#define X721_CONTROLSTATUS_RESERVEDFORTEST		2
#define X721_CONTROLSTATUS_SUSPENDED			3

#define X721_OPERATIONALSTATE_DISABLED			0
#define X721_OPERATIONALSTATE_ENABLED			1

#define X721_USAGESTATE_IDLE				0
#define X721_USAGESTATE_ACTIVE				1
#define X721_USAGESTATE_BUSY				2

#define X721_PROCEDURALSTATUS_INITIALIZATIONREQUIRED	0
#define X721_PROCEDURALSTATUS_NOTINITIALIZED		1
#define X721_PROCEDURALSTATUS_INITIALIZING		2
#define X721_PROCEDURALSTATUS_REPORTING			3
#define X721_PROCEDURALSTATUS_TERMINATING		4

#define X721_STANDBYSTATUS_HOTSTANDBY			0
#define X721_STANDBYSTATUS_COLDSTANDBY			1
#define X721_STANDBYSTATUS_PROVIDINGSERVICE		2

#define X721_UNKNOWNSTATUS_FALSE			0
#define X721_UNKNOWNSTATUS_TRUE				1

/*
 * MANAGEMENT ACTIONS
 *
 * Note all actions are applicable to all objects.  Actions not applicable to an
 * object will return EINVAL or EOPNOTSUPP.  Administrative state actions will
 * return the new value of the alarm status in "result".  Alarm status actions
 * will return the new value of the alarm status in "result".  Control status
 * actions will return the new value of the control status in "result".
 */

#define X721_ADM_LOCKED					 1
#define X721_ADM_UNLOCKED				 2
#define X721_ADM_SHUTTINGDOWN				 3
#define X721_ALM_SET_UNDERREPAIR			 4
#define X721_ALM_CLR_UNDERREPAIR			 5
#define X721_ALM_SET_CRITICAL				 6
#define X721_ALM_CLR_CRITICAL				 7
#define X721_ALM_SET_MAJOR				 8
#define X721_ALM_CLR_MAJOR				 9
#define X721_ALM_SET_MINOR				10
#define X721_ALM_CLR_MINOR				11
#define X721_ALM_SET_OUTSTANDING			12
#define X721_ALM_CLR_OUTSTANDING			13
#define X721_CTL_SET_SUBJECTTOTEST			14
#define X721_CTL_CLR_SUBJECTTOTEST			15
#define X721_CTL_SET_PARTOFSERVICESLOCKED		16
#define X721_CTL_CLR_PARTOFSERVICESLOCKED		17
#define X721_CTL_SET_RESERVEDFORTEST			18
#define X721_CTL_CLR_RESERVEDFORTEST			19
#define X721_CTL_SET_SUSPENDED				20
#define X721_CTL_CLR_SUSPENDED				21


#endif				/* __SYS_SMI_IOCTL_H__ */

