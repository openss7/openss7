/*****************************************************************************

 @(#) strsysctl.h,v 0.9.2.4 2003/10/20 11:22:58 brian Exp

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified 2003/10/20 11:22:58 by brian

 *****************************************************************************/

#ifndef __LOCAL_STRSYSCTL_H__
#define __LOCAL_STRSYSCTL_H__

/* /proc/streams */
enum {
	CTL_STREAMS = 11,		/* STREAMS */
};

/* /proc/sys/streams */
enum {
	STREAMS_MAXPSZ = 1,
	STREAMS_MINPSZ = 2,
	STREAMS_HIWAT = 3,
	STREAMS_LOWAT = 4,
	STREAMS_CLTIME = 5,
	STREAMS_RTIME = 6,
	STREAMS_NSTRPUSH = 7,
	STREAMS_STRTHRESH = 8,
	STREAMS_STRHOLD = 9,
	STREAMS_STRCTLSZ = 10,
	STREAMS_STRMSGSZ = 11,
	STREAMS_NSTRMSGS = 12,
	STREAMS_NBAND = 13,
	STREAMS_REUSE_FMODSW = 14,
	STREAMS_MAX_APUSH = 15,
	STREAMS_MAX_STRAMOD = 16,
	STREAMS_MAX_STRDEV = 17,
	STREAMS_MAX_STRMOD = 18,
	STREAMS_MAX_MBLK = 19,
	STREAMS_MSG_PRIORITY = 20,
};

extern int sysctl_str_maxpsz;		/* stream head default max packet size */
extern int sysctl_str_minpsz;		/* stream head default min packet size */
extern int sysctl_str_hiwat;		/* stream head default hi water mark */
extern int sysctl_str_lowat;		/* stream head default lo water mark */
extern int sysctl_str_cltime;		/* close wait time in msec */
extern int sysctl_str_rtime;		/* HZ to wait to forward held msg */
extern int sysctl_str_nstrpush;		/* max pushed modules */
extern int sysctl_str_strthresh;	/* memory limit */
extern int sysctl_str_strhold;		/* active stream hold feature */
extern int sysctl_str_strctlsz;		/* maximum stream control size */
extern int sysctl_str_strmsgsz;		/* maximum stream message size */
extern int sysctl_str_nstrmsgs;		/* maximum number of streams messages */
extern int sysctl_str_nband;		/* number of queue bands */
extern int sysctl_str_reuse_fmodsw;	/* reuse fmodsw entries */
extern int sysctl_str_max_apush;	/* max autopushed mods per str */
extern int sysctl_str_max_stramod;	/* max autopushed modules */
extern int sysctl_str_max_strdev;	/* max streams devices */
extern int sysctl_str_max_strmod;	/* max streams modules */
extern int sysctl_str_max_mblk;		/* max of headers on free list */
extern int sysctl_str_msg_priority;	/* observer message allocation priority */

#define sysctl_str_maxpsz	str_minfo.mi_maxpsz
#define sysctl_str_minpsz	str_minfo.mi_minpsz
#define sysctl_str_hiwat	str_minfo.mi_hiwat
#define sysctl_str_lowat	str_minfo.mi_lowat

/* initialization for main */
extern int strsysctl_init(void);
extern void strsysctl_exit(void);

#endif				/* __LOCAL_STRSYSCTL_H__ */
