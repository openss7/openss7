/*****************************************************************************

 @(#) $Id: sccp_ioctl.h,v 0.9 2004/01/17 08:08:12 brian Exp $

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

#ifndef __SCCP_IOCTL_H__
#define __SCCP_IOCTL_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#include <linux/ioctl.h>

typedef ulong sccp_ulong;

#define SCCP_IOC_MAGIC 's'

/*
 *  PROTOCOL VARIANT AND OPTIONS
 */
#define SCCP_IOCGOPTIONS	_IOR(	SCCP_IOC_MAGIC, LMI_IOCGOPTIONS, lmi_option_t )
#define SCCP_IOCSOPTIONS	_IOW(	SCCP_IOC_MAGIC, LMI_IOCSOPTIONS, lmi_option_t )

/*
 *  CONFIGURATION
 */
typedef struct sccp_config {
} sccp_config_t;

#define SCCP_IOCGCONFIG		_IOWR(	SCCP_IOC_MAGIC, LMI_IOCGCONFIG, sccp_config_t )
#define SCCP_IOCSCONFIG		_IOWR(	SCCP_IOC_MAGIC, LMI_IOCSCONFIG, sccp_config_t )
#define SCCP_IOCTCONFIG		_IOWR(	SCCP_IOC_MAGIC, LMI_IOCTCONFIG, sccp_config_t )
#define SCCP_IOCCCONFIG		_IOWR(	SCCP_IOC_MAGIC, LMI_IOCCCONFIG, sccp_config_t )

/*
 *  STATE
 */
typedef struct sccp_statem {
} sccp_statem_t;

#define SCCP_IOCGSTATEM		_IOR(	SCCP_IOC_MAGIC, LMI_IOCGSTATEM, sccp_statem_t )
#define SCCP_IOCCMRESET		_IOR(	SCCP_IOC_MAGIC, LMI_IOCCMRESET, sccp_statem_t )

/*
 *  STATISTICS
 */
typedef struct sccp_stats {
	lmi_sta_t header;
} sccp_stats_t;

#define SCCP_IOCGSTATSP		_IOR(	SCCP_IOC_MAGIC, LMI_IOCGSTATSP, lmi_sta_t )
#define SCCP_IOCSSTATSP		_IOWR(	SCCP_IOC_MAGIC, LMI_IOCSSTATSP, lmi_sta_t )
#define SCCP_IOCGSTATS		_IOR(	SCCP_IOC_MAGIC, LMI_IOCGSTATS, sccp_stats_t )
#define SCCP_IOCCSTATS		_IOW(	SCCP_IOC_MAGIC, LMI_IOCSSTATS, sccp_stats_t )

/*
 *  NOTIFICATIONS
 */
typedef struct sccp_notify {
	sccp_ulong events;
} sccp_notify_t;

#define SCCP_EVT_FAIL_UNSPECIFIED   0x00000001

#define SCCP_IOCGNOTIFY		_IOR(	SCCP_IOC_MAGIC, LMI_IOCGNOTIFY, sccp_notify_t )
#define SCCP_IOCSNOTIFY		_IOW(	SCCP_IOC_MAGIC, LMI_IOCSNOTIFY, sccp_notify_t )
#define SCCP_IOCCNOTIFY		_IOW(	SCCP_IOC_MAGIC, LMI_IOCCNOTIFY, sccp_notify_t )

#define SCCP_IOC_FIRST		LMI_IOC_FIRST
#define SCCP_IOC_LAST		LMI_IOC_LAST
#define SCCP_IOC_PRIVATE	LMI_IOC_PRIVATE

#define SCCP_NR_IOCGSSAP	(LMI_IOC_PRIVATE+0)
#define SCCP_NR_IOCSSSAP	(LMI_IOC_PRIVATE+1)
#define SCCP_NR_IOCCSSAP	(LMI_IOC_PRIVATE+2)
#define SCCP_NR_IOCGCPC		(LMI_IOC_PRIVATE+3)
#define SCCP_NR_IOCSCPC		(LMI_IOC_PRIVATE+4)
#define SCCP_NR_IOCCCPC		(LMI_IOC_PRIVATE+5)
#define SCCP_NR_IOCPRIV6	(LMI_IOC_PRIVATE+6)
#define SCCP_NR_IOCPRIV7	(LMI_IOC_PRIVATE+7)

typedef struct sccp_ssap {
	sccp_ulong ni;			/* network indicator */
	sccp_ulong si;			/* service indicator */
	sccp_ulong pc;			/* point code */
	sccp_ulong ssn;			/* subsystem number */
} sccp_ssap_t;

typedef struct sccp_saplist {
	sccp_ulong nsap;		/* number of SCCP-SAPs */
	sccp_ssap_t saps[0];		/* followed by nsap SCCP-SAPs */
} sccp_saplist_t;

#define SCCP_IOCGSSAP		_IOR(	SCCP_IOC_MAGIC, SCCP_NR_IOCGSSAP, sccp_saplist_t )
#define SCCP_IOCSSSAP		_IOW(	SCCP_IOC_MAGIC, SCCP_NR_IOCSSSAP, sccp_saplist_t )
#define SCCP_IOCCSSAP		_IOW(	SCCP_IOC_MAGIC, SCCP_NR_IOCCSSAP, sccp_ssap_t )

typedef struct sccp_css {
	sccp_ulong ssn;			/* subsystem number */
	sccp_ulong status;		/* subsystem status */
} sccp_css_t;

typedef struct sccp_cpc {
	sccp_ulong pc;			/* point code */
	sccp_ulong status;		/* point code status */
	sccp_ulong ncss;		/* number of concerned subsystems */
	sccp_css_t csss[0];		/* followed by ncss subsystems */
} sccp_cpc_t;

typedef struct sccp_cpclist {
	sccp_ssap_t sap;		/* SCCP Service Access Point */
	sccp_ulong ncpc;		/* number of concerned point codes */
	sccp_cpc_t cpcs[0];		/* followed by ncpc concernd point codes */
} sccp_cpclist_t;

#define SCCP_IOCGCPC		_IOR(	SCCP_IOC_MAGIC, SCCP_NR_IOCGCPC, sccp_cpclist_t )
#define SCCP_IOCSCPC		_IOW(	SCCP_IOC_MAGIC, SCCP_NR_IOCSCPC, sccp_cpclist_t )
#define SCCP_IOCCCPC		_IOW(	SCCP_IOC_MAGIC, SCCP_NR_IOCCCPC, sccp_ssap_t )

#endif				/* __SCCP_IOCTL_H__ */
