/*****************************************************************************

 @(#) src/snmp/ds_agent.h

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

#ifndef __LOCAL_DS_AGENT_H__
#define __LOCAL_DS_AGENT_H__

/* defines agent's default store registrations */

/* booleans */
#define DS_AGENT_VERBOSE		0	/* 1 if verbose output desired */
#define DS_AGENT_ROLE			1	/* 0 if master, 1 if client */
#define DS_AGENT_NO_ROOT_ACCESS		2	/* 1 if we can't get root access */
#define DS_AGENT_AGENTX_MASTER		3	/* 1 if AgentX desired */

/* strings */
#define DS_AGENT_PROGNAME		0	/* argv[0] */
#define DS_AGENT_X_SOCKET		1	/* AF_UNIX or ip:port socket addr */
#define DS_AGENT_PORTS			2	/* localhost:9161,tcp:localhost:9161... */

/* integers */
#define DS_AGENT_FLAGS			0	/* session.flags */
#define DS_AGENT_USERID			1
#define DS_AGENT_GROUPID		2

#endif				/* __LOCAL_DS_AGENT_H__ */
