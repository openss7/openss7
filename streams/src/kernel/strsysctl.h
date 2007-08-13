/*****************************************************************************

 @(#) $Id: strsysctl.h,v 0.9.2.15 2007/08/13 22:46:20 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/13 22:46:20 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strsysctl.h,v $
 Revision 0.9.2.15  2007/08/13 22:46:20  brian
 - GPLv3 header updates

 Revision 0.9.2.14  2006/02/20 10:59:22  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ifndef __LOCAL_STRSYSCTL_H__
#define __LOCAL_STRSYSCTL_H__

#ident "@(#) $RCSfile: strsysctl.h,v $ $Name:  $($Revision: 0.9.2.15 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef BIG_STATIC
#define BIG_STATIC
#endif

#ifndef BIG_STATIC_INLINE
#define BIG_STATIC_INLINE
#endif

#ifndef BIG_STATIC_STH
#define BIG_STATIC_STH
#endif

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
	STREAMS_IOCTIME = 7,
	STREAMS_NSTRPUSH = 8,
	STREAMS_STRTHRESH = 9,
	STREAMS_STRHOLD = 10,
	STREAMS_STRCTLSZ = 11,
	STREAMS_STRMSGSZ = 12,
	STREAMS_NSTRMSGS = 13,
	STREAMS_NBAND = 14,
	STREAMS_REUSE_FMODSW = 15,
	STREAMS_MAX_APUSH = 16,
	STREAMS_MAX_STRAMOD = 17,
	STREAMS_MAX_STRDEV = 18,
	STREAMS_MAX_STRMOD = 19,
	STREAMS_MAX_MBLK = 20,
	STREAMS_MSG_PRIORITY = 21,
};

BIG_STATIC ulong sysctl_str_maxpsz;	/* stream head default max packet size */
BIG_STATIC ulong sysctl_str_minpsz;	/* stream head default min packet size */
BIG_STATIC ulong sysctl_str_hiwat;	/* stream head default hi water mark */
BIG_STATIC ulong sysctl_str_lowat;	/* stream head default lo water mark */
BIG_STATIC ulong sysctl_str_cltime;	/* close wait time in msec (save in ticks) */
BIG_STATIC ulong sysctl_str_rtime;	/* msec to wait to forward held msg (save in ticks) */
BIG_STATIC ulong sysctl_str_ioctime;	/* msec to wait for ioctl() acknowledgement (save in ticks) 
					 */
BIG_STATIC_STH ulong sysctl_str_nstrpush;	/* max pushed modules */
BIG_STATIC ulong sysctl_str_strthresh;	/* memory limit */
BIG_STATIC ulong sysctl_str_strhold;	/* active stream hold feature */
BIG_STATIC_STH ulong sysctl_str_strctlsz;	/* maximum stream control size */
extern ulong sysctl_str_strmsgsz;	/* maximum stream message size */
BIG_STATIC ulong sysctl_str_nstrmsgs;	/* maximum number of streams messages */
BIG_STATIC ulong sysctl_str_nband;	/* number of queue bands */
BIG_STATIC int sysctl_str_reuse_fmodsw;	/* reuse fmodsw entries */
BIG_STATIC ulong sysctl_str_max_apush;	/* max autopushed mods per str */
BIG_STATIC ulong sysctl_str_max_stramod;	/* max autopushed modules */
BIG_STATIC ulong sysctl_str_max_strdev;	/* max streams devices */
BIG_STATIC ulong sysctl_str_max_strmod;	/* max streams modules */
BIG_STATIC ulong sysctl_str_max_mblk;	/* max of headers on free list */
BIG_STATIC int sysctl_str_msg_priority;	/* observer message allocation priority */

/* initialization for main */
BIG_STATIC int strsysctl_init(void);
BIG_STATIC void strsysctl_exit(void);

#endif				/* __LOCAL_STRSYSCTL_H__ */
