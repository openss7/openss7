/*****************************************************************************

 @(#) $Id: sctpi_ioctl.h,v 0.9.2.5 2008-04-28 23:13:26 brian Exp $

 -----------------------------------------------------------------------------

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

 Last Modified $Date: 2008-04-28 23:13:26 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctpi_ioctl.h,v $
 Revision 0.9.2.5  2008-04-28 23:13:26  brian
 - updated headers for release

 Revision 0.9.2.4  2007/08/14 06:22:27  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/06/17 01:57:13  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SCTPI_IOCTL_H__
#define __SCTPI_IOCTL_H__

#ident "@(#) $RCSfile: sctpi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

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
