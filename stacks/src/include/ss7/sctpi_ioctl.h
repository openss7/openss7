/*****************************************************************************

 @(#) $Id: sctpi_ioctl.h,v 0.9 2004/01/17 08:08:12 brian Exp $

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

 Last Modified $Date: 2004/01/17 08:08:12 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SCTPI_IOCTL_H__
#define __SCTPI_IOCTL_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#define SCTP_IOC_MAGIC 'c'

/*
 *  PROTOCOL VARIANT AND OPTIONS
 */

#define SCTP_IOCCOPTIONS	_IOR(  SCTP_IOC_MAGIC, 0, lmi_option_t )
#define SCTP_IOCSOPTIONS	_IOW(  SCTP_IOC_MAGIC, 1, lmi_option_t )

/*
 *  CONFIGURATION
 */
typedef struct sctp_config {
} sctp_config_t;

#define SCTP_IOCGCONFIG		_IOWR( SCTPIOC_MAGIC, 2, sctp_config_t )
#define SCTP_IOCSCONFIG		_IOWR( SCTPIOC_MAGIC, 3, sctp_config_t )
#define SCTP_IOCTCONFIG		_IOWR( SCTPIOC_MAGIC, 4, sctp_config_t )
#define SCTP_IOCCCONFIG		_IOWR( SCTPIOC_MAGIC, 5, sctp_config_t )

/*
 *  STATE
 */

typedef struct sctp_statem {
} sctp_statem_t;

#define SCTP_STATE_CLOSED		0
#define SCTP_STATE_COOKIE_WAIT		1
#define SCTP_STATE_COOKIE_ECHOED	2
#define SCTP_STATE_ESTABLISHED		3
#define SCTP_STATE_SHUTDOWN_RECEIVED	4
#define SCTP_STATE_SHUTDOWN_PENDING	5
#define SCTP_STATE_SHUTDOWN_SENT	6
#define SCTP_STATE_SHUTDOWN_ACK_SENT	7

#define SCTP_IOCGSTATEM		_IOR(  SCTP_IOC_MAGIC, 6, sctp_statem_t )
#define SCTP_IOCCMRESET		_IOR(  SCTP_IOC_MAGIC, 7, sctp_statem_t )

/*
 *  STATISTICS
 */

typedef struct sctp_stats {
} sctp_stats_t;

#define SCTP_IOCGSTATSP		_IOR(  SCTP_IOC_MAGIC,  8, lmi_sta_t )
#define SCTP_IOCSSTATSP		_IORW( SCTP_IOC_MAGIC,  9, lmi_sta_t )
#define SCTP_IOCGSTATS		_IOR(  SCTP_IOC_MAGIC, 10, sctp_stats_t )
#define SCTP_IOCCSTATS		_IOW(  SCTP_IOC_MAGIC, 11, sctp_stats_t )

/*
 *  NOTIFICATIONS
 */

typedef struct sctp_notify {
	sctp_ulong events;
} sctp_notify_t;

#define SCTP_EVENT_HEADER_LEN		16	/* for now */

#define SCTP_IOCGNOTIFY		_IOR(  SCTP_IOC_MAGIC, 12, sctp_notify_t )
#define SCTP_IOCSNOTIFY		_IOW(  SCTP_IOC_MAGIC, 13, sctp_notify_t )
#define SCTP_IOCCNOTIFY		_IOW(  SCTP_IOC_MAGIC, 14, sctp_notify_t )

#define SCTP_IOC_FIRST     0
#define SCTP_IOC_LAST     14
#define SCTP_IOC_PRIVATE  32

#endif				/* __SCTPI_IOCTL_H__ */
