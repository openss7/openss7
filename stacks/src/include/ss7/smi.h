/*****************************************************************************

 @(#) $Id: smi.h,v 0.9 2004/01/17 08:08:13 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/01/17 08:08:13 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SMI_H__
#define __SMI_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

enum {
	SMI_OK_ACK,
	SMI_ERROR_ACK,
	SMI_ERROR_IND,
	SMI_CONFIG_SETUP,
	SMI_CONFIG_COMMIT,
	SMI_CONFIG_SET,
	SMI_CONFIG_GET,
	SMI_STATE_REQ,
	SMI_STATS_SET,
	SMI_STATS_GET,
	SMI_STATS_IND,
	SMI_NOTIFY_REQ,
	SMI_EVENT_IND
};

typedef unsigned long smi_ulong;
typedef unsigned short smi_ushort;
typedef unsigned char smi_uchar;
typedef caddr_t smi_caddr;

/*
   SMI_OK_ACK, M_CTL 
 */

typedef struct {
	smi_ulong smi_primitive;	/* SMI_OK_ACK */
	smi_ulong smi_state;
	smi_ulong smi_correct_primitive;
} smi_ok_ack_t;

/*
   SMI_ERROR_ACK, M_CTL 
 */

typedef struct {
	smi_ulong smi_primitive;	/* SMI_ERROR_ACK */
	smi_ulong smi_state;
	smi_ulong smi_error_primitive;
	smi_ulong smi_errno;
	smi_ulong smi_reason;
} smi_error_ack_t;

/*
   SMI_ERROR_IND, M_CTL 
 */

typedef struct {
	smi_ulong smi_primitive;	/* SMI_ERROR_IND */
	smi_ulong smi_state;
	smi_ulong smi_errno;
	smi_ulong smi_reason;
} smi_error_ind_t;

/*
   SMI_CONFIG_SETUP, M_CTL 
 */

/*
   SMI_CONFIG_COMMIT, M_CTL 
 */

/*
   SMI_CONFIG_SET, M_CTL 
 */

/*
   SMI_CONFIG_GET, M_CTL 
 */

/*
   SMI_STATE_REQ, M_CTL 
 */

/*
   SMI_STATS_SET, M_CTL 
 */

/*
   SMI_STATS_GET, M_CTL 
 */

/*
   SMI_STATS_IND, M_CTL 
 */

typedef struct {
	smi_ulong smi_primitive;	/* SMI_STATS_IND */
	smi_ulong smi_interval;
	smi_ulong smi_timestamp;
} smi_stats_ind_t;

/*
   SMI_NOTIFY_REQ, M_CTL 
 */

typedef struct {
	smi_ulong smi_primitive;	/* SMI_NOTIFY_REQ */
	smi_ulong smi_notify;
} smi_notify_req_t;

/*
   SMI_EVENT_IND, M_CTL 
 */

typedef struct {
	smi_ulong smi_primitive;	/* SMI_EVENT_IND */
	smi_ulong smi_objectid;
	smi_ulong smi_timestamp;
	smi_ulong smi_severity;
} smi_event_ind_t;

union SMI_primitive {
	smi_ulong smi_primitive;
	smi_ok_ack_t ok_ack;
	smi_error_ack_t error_ack;
	smi_error_ind_t error_ind;
	smi_stats_ind_t stats_ind;
	smi_notify_req_t notify_req;
	smi_event_ind_t event_ind;
};

#endif				/* __SMI_H__ */
