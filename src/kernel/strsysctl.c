/*****************************************************************************

 @(#) File: src/kernel/strsysctl.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

static char const ident[] = "src/kernel/strsysctl.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#endif
#include <linux/sysctl.h>
#if defined HAVE_KINC_LINUX_SECURITY_H
#include <linux/security.h>	/* avoid ptrace conflict */
#endif

#include "sys/strdebug.h"

#include <sys/stream.h>
#include <sys/strsubr.h>

#include "sys/config.h"
#include "src/modules/sth.h"	/* for str_minfo */
#include "strsysctl.h"		/* extern verification */

ulong sysctl_str_maxpsz = STRMAXPSZ;	/* stream head default max packet size */
ulong sysctl_str_minpsz = STRMINPSZ;	/* stream head default min packet size */
ulong sysctl_str_hiwat = STRHIGH;	/* stream head default hi water mark */
ulong sysctl_str_lowat = STRLOW;	/* stream head default lo water mark */
BIG_STATIC ulong sysctl_str_cltime = 15 * HZ;	/* close wait time in msec (saved in ticks) */
BIG_STATIC ulong sysctl_str_rtime = (10 * HZ) / 1000;	/* msec to wait to forward held msg (saved
							   in ticks) */
BIG_STATIC ulong sysctl_str_ioctime = 15 * HZ;	/* msec to wait for ioctl() acknowledgement (saved
						   in ticks) */
ulong sysctl_str_nstrpush = 64;	/* maximum number of pushed modules */
BIG_STATIC ulong sysctl_str_strthresh = (1 << 20);	/* memory limit */
BIG_STATIC ulong sysctl_str_strhold = 0;	/* active stream hold feature */
ulong sysctl_str_strctlsz = (1 << 12);	/* maximum stream control size */
ulong sysctl_str_strmsgsz = (1 << 18);	/* maximum stream message size */
BIG_STATIC ulong sysctl_str_nstrmsgs = (1 << 20);	/* maximum number of streams messages */
BIG_STATIC ulong sysctl_str_nband = 256;	/* number of queue bands */
BIG_STATIC int sysctl_str_reuse_fmodsw = 0;	/* reuse fmodsw entries if true */
BIG_STATIC ulong sysctl_str_max_apush = MAX_APUSH;	/* max no autopushed mods per stream */
BIG_STATIC ulong sysctl_str_max_stramod = 8;	/* max no autopushed modules */
BIG_STATIC ulong sysctl_str_max_strdev = MAX_STRDEV;	/* max no streams devices */
BIG_STATIC ulong sysctl_str_max_strmod = MAX_STRMOD;	/* max no streams modules */
BIG_STATIC ulong sysctl_str_max_mblk = 10;	/* max no of headers on free list */
BIG_STATIC int sysctl_str_msg_priority = 0;	/* use priority on allocation if true */

EXPORT_SYMBOL_GPL(sysctl_str_strmsgsz);	/* always needed for drv_getparm */
EXPORT_SYMBOL_GPL(sysctl_str_strctlsz);
EXPORT_SYMBOL_GPL(sysctl_str_nstrpush);

EXPORT_SYMBOL_GPL(sysctl_str_maxpsz);
EXPORT_SYMBOL_GPL(sysctl_str_minpsz);
EXPORT_SYMBOL_GPL(sysctl_str_hiwat);
EXPORT_SYMBOL_GPL(sysctl_str_lowat);

STATIC struct ctl_table_header *streams_sysctl_handle;

STATIC struct ctl_table streams_table[] = {
	/* stream head default maximum packet size */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_MAXPSZ,
#endif
	 .procname = "maxpsz",
	 .data = &sysctl_str_maxpsz,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = &sysctl_str_minpsz,
	 .extra2 = NULL
	},
	/* stream head default minimum packet size */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_MINPSZ,
#endif
	 .procname = "minpsz",
	 .data = &sysctl_str_minpsz,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = &sysctl_str_maxpsz
	},
	/* stream head default high water mark */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_HIWAT,
#endif
	 .procname = "hiwat",
	 .data = &sysctl_str_hiwat,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = &sysctl_str_lowat,
	 .extra2 = NULL
	},
	/* stream head default low water mark */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_LOWAT,
#endif
	 .procname = "lowat",
	 .data = &sysctl_str_lowat,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = &sysctl_str_hiwat},
	/* default close time */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_CLTIME,
#endif
	 .procname = "cltime",
	 .data = &sysctl_str_cltime,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
#ifdef HAVE_PROC_DOINTVEC_MS_JIFFIES_USABLE
	 .proc_handler = &proc_dointvec_ms_jiffies,
#else
	 .proc_handler = &proc_doulongvec_ms_jiffies_minmax,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* default hold time maximum */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_RTIME,
#endif
	 .procname = "rtime",
	 .data = &sysctl_str_rtime,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
#ifdef HAVE_PROC_DOINTVEC_MS_JIFFIES_USABLE
	 .proc_handler = &proc_dointvec_ms_jiffies,
#else
	 .proc_handler = &proc_doulongvec_ms_jiffies_minmax,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* default ioctl() acknowledgement time */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_IOCTIME,
#endif
	 .procname = "ioctime",
	 .data = &sysctl_str_ioctime,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
#ifdef HAVE_PROC_DOINTVEC_MS_JIFFIES_USABLE
	 .proc_handler = &proc_dointvec_ms_jiffies,
#else
	 .proc_handler = &proc_doulongvec_ms_jiffies_minmax,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* maximum number of pushed modules - system wide */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_NSTRPUSH,
#endif
	 .procname = "nstrpush",
	 .data = &sysctl_str_nstrpush,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* maximum memory threshold */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_STRTHRESH,
#endif
	 .procname = "strthresh",
	 .data = &sysctl_str_strthresh,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* hold time for STRHOLD feature in milliseconds, zero disables */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_STRHOLD,
#endif
	 .procname = "strhold",
	 .data = &sysctl_str_strhold,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
#ifdef HAVE_PROC_DOINTVEC_MS_JIFFIES_USABLE
	 .proc_handler = &proc_dointvec_ms_jiffies,
#else
	 .proc_handler = &proc_doulongvec_ms_jiffies_minmax,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* maximum ctrl part size */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_STRCTLSZ,
#endif
	 .procname = "strctlsz",
	 .data = &sysctl_str_strctlsz,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* maximum data part size */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_STRMSGSZ,
#endif
	 .procname = "strmsgsz",
	 .data = &sysctl_str_strmsgsz,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_NSTRMSGS,
#endif
	 .procname = "nstrmsgs",
	 .data = &sysctl_str_nstrmsgs,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* number of queue bands, 256 is too many already */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_NBAND,
#endif
	 .procname = "nband",
	 .data = &sysctl_str_nband,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategry = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_REUSE_FMODSW,
#endif
	 .procname = "reuse_fmodsw",
	 .data = &sysctl_str_reuse_fmodsw,
	 .mode = sizeof(int),
	 .maxlen = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_dointvec,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* maximum number of autopushed modules - per device */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_MAX_APUSH,
#endif
	 .procname = "max_apush",
	 .data = &sysctl_str_max_apush,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_MAX_STRAMOD,
#endif
	 .procname = "max_stramod",
	 .data = &sysctl_str_max_stramod,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* maximum number of STREAMS cdevsw[] entries */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_MAX_STRDEV,
#endif
	 .procname = "max_strdev",
	 .data = &sysctl_str_max_strdev,
	 .maxlen = sizeof(ulong),
	 .mode = 0444,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* maximum number of STREAMS fmodsw[] entries */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_MAX_STRMOD,
#endif
	 .procname = "max_strmod",
	 .data = &sysctl_str_max_strmod,
	 .maxlen = sizeof(ulong),
	 .mode = 0444,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	/* maximum number of allocated mdbblocks */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_MAX_MBLK,
#endif
	 .procname = "max_mblk",
	 .data = &sysctl_str_max_mblk,
	 .maxlen = sizeof(ulong),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = STREAMS_MSG_PRIORITY,
#endif
	 .procname = "msg_priority",
	 .data = &sysctl_str_msg_priority,
	 .maxlen = sizeof(int),
	 .mode = 0644,
	 .child = NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 .parent = NULL,
#endif
	 .proc_handler = &proc_dointvec,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	 .poll = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 .strategy = NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 .de = NULL,
#endif
	 .extra1 = NULL,
	 .extra2 = NULL
	},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = 0,
#endif
	 .procname = NULL,
	}
};

STATIC struct ctl_table streams_root_table[] = {
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = CTL_STREAMS,
#endif
	 .procname = "streams",
	 .data = NULL,
	 .maxlen = 0,
	 .mode = 0555,
	 .child = streams_table,
	},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 .ctl_name = 0,
#endif
	 .procname = NULL,
	}
};

BIG_STATIC int
strsysctl_init(void)
{
	sysctl_str_maxpsz = STRMAXPSZ;	/* stream head default max packet size */
	sysctl_str_minpsz = STRMINPSZ;	/* stream head default min packet size */
	sysctl_str_hiwat = STRHIGH;	/* stream head default hi water mark */
	sysctl_str_lowat = STRLOW;	/* stream head default lo water mark */
#if defined HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT || defined HAVE_KMEMB_STRUCT_CTL_TABLE_POLL
	if ((streams_sysctl_handle = register_sysctl_table(streams_root_table)))
		return (0);
#else
	if ((streams_sysctl_handle = register_sysctl_table(streams_root_table, 1)))
		return (0);
#endif
	return (-EIO);
}

BIG_STATIC void
strsysctl_exit(void)
{
	unregister_sysctl_table(streams_sysctl_handle);
	streams_sysctl_handle = NULL;
}
