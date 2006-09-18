/*****************************************************************************

 @(#) $Id: lapd_ioctl.h,v 0.9.2.2 2005/05/14 08:30:45 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/05/14 08:30:45 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LAPD_IOCTL_H__
#define __LAPD_IOCTL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#define LAPD_IOC_MAGIC 'd'

#define LAPD_OBJ_TYPE_DF		0	/* default */
#define LAPD_OBJ_TYPE_DL		1	/* DL user */
#define LAPD_OBJ_TYPE_CD		2	/* CD provider */

typedef struct lapd_iochdr {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* command or argument */
} lapd_iochdr_t;

typedef struct lapd_proto {
	ulong pvar;
	ulong popt;
} lapd_proto_t;

/*
 *  Timers
 */
typedef struct lapd_timers_dl {
	ulong t200;			/* retransmission timeout */
	ulong t201;			/* retransmission timeout for TEI management */
	ulong t202;			/* response timeout for TEI management */
	ulong t203;			/* default inactivity timeout */
} lapd_timers_dl_t;

typedef struct lapd_timers_cd {
	ulong t200;			/* default retransmission timeout for data links */
	ulong t201;			/* retransmission timeout for TEI management */
	ulong t202;			/* response timeout for TEI management */
	ulong t203;			/* default inactivity timeout for data links */
} lapd_timers_cd_t;

typedef struct lapd_timers_df {
} lapd_timers_df_t;

/*
 *  Options
 */
typedef struct lapd_option_dl {
	struct lapd_timers_dl timers;
} lapd_option_dl_t;

typedef struct lapd_option_cd {
	struct lapd_timers_cd timers;
	ulong k;
	ulong n200;			/* maximum number of retransmissions */
	ulong n201;			/* maximum size of I field */
	ulong n202;			/* maximum number of retransmissions TEI management */
} lapd_option_cd_t;

typedef struct lapd_option_df {
	struct lapd_timers_df timers;
} lapd_option_df_t;

#define LAPD_IOCGOPTIONS	 _IOR(	LAPD_IOC_MAGIC,	 0,	lapd_iochdr_t	)
#define LAPD_IOCSOPTIONS	 _IOW(	LAPD_IOC_MAGIC,	 1,	lapd_iochdr_t	)

/*
 *  Configuration
 */
typedef struct lapd_config_dl {
	ulong ppa;			/* attached PPA (also ID of attached CD) */
	struct lapd_proto proto;	/* protocol options */
	struct lapd_addr dlc;		/* DL Connection address */
	ulong mode;			/* DL mode */
} lapd_config_dl_t;

typedef struct lapd_config_cd {
	ulong muxid;			/* multiplexor id */
	struct lapd_proto proto;	/* protocol options */
	ulong ppa;			/* associated (CDI) ppa */
	ulong type;			/* type of comm link */
	ulong mode;			/* mode of comm link */
} lapd_config_cd_t;

#define CD_TYPE_B	1	/* ISDN B-Channel (LAPB) */
#define CD_TYPE_D	2	/* ISDN D-Channel (LAPD, LAPB, LAPF) */
#define CD_TYPE_E	3	/* ISDN E-Channel (MTP2) */

#define CD_MODE_USER	0	/* User side */
#define CD_MODE_NTWK	1	/* Ntwk side */
#define CD_MODE_BOTH	2	/* Symm side */

typedef struct lapd_config_df {
	struct lapd_proto proto;
} lapd_config_df_t;

#define LAPD_GET	    0
#define LAPD_ADD	    1
#define LAPD_CHA	    2
#define LAPD_DEL	    3

#define LAPD_IOCGCONFIG	_IOWR(	LAPD_IOC_MAGIC,	 2,	lapd_iochdr_t	)
#define LAPD_IOCSCONFIG	_IOWR(	LAPD_IOC_MAGIC,	 3,	lapd_iochdr_t	)
#define LAPD_IOCTCONFIG	_IOWR(	LAPD_IOC_MAGIC,	 4,	lapd_iochdr_t	)
#define LAPD_IOCCCONFIG	_IOWR(	LAPD_IOC_MAGIC,	 5,	lapd_iochdr_t	)

/*
 *  State
 */
typedef struct lapd_statem_dl {
	struct lapd_timers_dl timers;
} lapd_statem_dl_t;

typedef struct lapd_statem_cd {
	struct lapd_timers_cd timers;
} lapd_statem_cd_t;

typedef struct lapd_statem_df {
	struct lapd_timers_df timers;
} lapd_statem_df_t;

#define LAPD_IOCGSTATEM	_IOWR(	LAPD_IOC_MAGIC,	 6,	lapd_iochdr_t	)
#define LAPD_IOCCMRESET	_IOWR(	LAPD_IOC_MAGIC,	 7,	lapd_iochdr_t	)

/*
 *  Statistics
 */
typedef struct lapd_stats_dl {
} lapd_stats_dl_t;

typedef struct lapd_stats_cd {
} lapd_stats_cd_t;

typedef struct lapd_stats_df {
} lapd_stats_df_t;

#define LAPD_IOCGSTATSP	_IOWR(	LAPD_IOC_MAGIC,	 8,	lapd_iochdr_t	)
#define LAPD_IOCSSTATSP	_IOWR(	LAPD_IOC_MAGIC,	 9,	lapd_iochdr_t	)
#define LAPD_IOCGSTATS	_IOWR(	LAPD_IOC_MAGIC,	10,	lapd_iochdr_t	)
#define LAPD_IOCCSTATS	_IOWR(	LAPD_IOC_MAGIC,	11,	lapd_iochdr_t	)

/*
 *  Notifications
 */
typedef struct lapd_notify_dl {
} lapd_notify_dl_t;

typedef struct lapd_notify_cd {
} lapd_notify_cd_t;

typedef struct lapd_notify_df {
} lapd_notify_df_t;

#define LAPD_IOCGNOTIFY	 _IOR(	LAPD_IOC_MAGIC,	12,	lapd_iochdr_t	)
#define LAPD_IOCSNOTIFY	 _IOW(	LAPD_IOC_MAGIC,	13,	lapd_iochdr_t	)
#define LAPD_IOCCNOTIFY	 _IOW(	LAPD_IOC_MAGIC,	14,	lapd_iochdr_t	)

/*
 *  Management
 */
#define LAPD_IOCCMGMT	_IOWR(	LAPD_IOC_MAGIC,	15,	lapd_iochdr_t	)

/*
 *  Pass-through IOCTLs
 */
#define LAPD_IOCCPASS	_IOWR(	LAPD_IOC_MAGIC,	16,	lapd_iochdr_t	)

#define LAPD_IOC_FIRST		 0
#define LAPD_IOC_LAST		16
#define LAPD_IOC_PRIVATE	32

#endif				/* __LAPD_IOCTL_H__ */
