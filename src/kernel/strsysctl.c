/*****************************************************************************

 @(#) $RCSfile: strsysctl.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:21:57 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 -----------------------------------------------------------------------------

 Last Modified $Date: 2010-11-28 14:21:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strsysctl.c,v $
 Revision 1.1.2.2  2010-11-28 14:21:57  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:37:17  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: strsysctl.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:21:57 $";

#ifdef NEED_LINUX_AUTOCONF_H
#include <linux/autoconf.h>
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

STATIC ctl_table streams_table[] = {
	/* stream head default maximum packet size */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_MAXPSZ,
#endif
	 "maxpsz", &sysctl_str_maxpsz, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 &sysctl_str_minpsz, NULL},
	/* stream head default minimum packet size */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_MINPSZ,
#endif
	 "minpsz", &sysctl_str_minpsz, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, &sysctl_str_maxpsz},
	/* stream head default high water mark */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_HIWAT,
#endif
	 "hiwat", &sysctl_str_hiwat, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 &sysctl_str_lowat, NULL},
	/* stream head default low water mark */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_LOWAT,
#endif
	 "lowat", &sysctl_str_lowat, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, &sysctl_str_hiwat},
	/* default close time */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_CLTIME,
#endif
	 "cltime", &sysctl_str_cltime, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_ms_jiffies_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* default hold time maximum */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_RTIME,
#endif
	 "rtime", &sysctl_str_rtime, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_ms_jiffies_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* default ioctl() acknowledgement time */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_IOCTIME,
#endif
	 "ioctime", &sysctl_str_ioctime, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_ms_jiffies_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* maximum number of pushed modules - system wide */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_NSTRPUSH,
#endif
	 "nstrpush", &sysctl_str_nstrpush, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* maximum memory threshold */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_STRTHRESH,
#endif
	 "strthresh", &sysctl_str_strthresh, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* hold time for STRHOLD feature in milliseconds, zero disables */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_STRHOLD,
#endif
	 "strhold", &sysctl_str_strhold, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_ms_jiffies_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* maximum ctrl part size */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_STRCTLSZ,
#endif
	 "strctlsz", &sysctl_str_strctlsz, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* maximum data part size */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_STRMSGSZ,
#endif
	 "strmsgsz", &sysctl_str_strmsgsz, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_NSTRMSGS,
#endif
	 "nstrmsgs", &sysctl_str_nstrmsgs, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* number of queue bands, 256 is too many already */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_NBAND,
#endif
	 "nband", &sysctl_str_nband, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_REUSE_FMODSW,
#endif
	 "reuse_fmodsw", &sysctl_str_reuse_fmodsw, sizeof(int), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_dointvec,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* maximum number of autopushed modules - per device */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_MAX_APUSH,
#endif
	 "max_apush", &sysctl_str_max_apush, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_MAX_STRAMOD,
#endif
	 "max_stramod", &sysctl_str_max_stramod, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* maximum number of STREAMS cdevsw[] entries */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_MAX_STRDEV,
#endif
	 "max_strdev", &sysctl_str_max_strdev, sizeof(ulong), 0444, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* maximum number of STREAMS fmodsw[] entries */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_MAX_STRMOD,
#endif
	 "max_strmod", &sysctl_str_max_strmod, sizeof(ulong), 0444, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	/* maximum number of allocated mdbblocks */
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_MAX_MBLK,
#endif
	 "max_mblk", &sysctl_str_max_mblk, sizeof(ulong), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_doulongvec_minmax,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 STREAMS_MSG_PRIORITY,
#endif
	 "msg_priority", &sysctl_str_msg_priority, sizeof(int), 0644, NULL,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	 NULL,
#endif
	 &proc_dointvec,
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_STRATEGY
	 NULL,
#endif
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_DE
	 NULL,
#endif
	 NULL, NULL},
	{0}
};

STATIC ctl_table streams_root_table[] = {
	{
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_CTL_NAME
	 CTL_STREAMS,
#endif
	 "streams", NULL, 0, 0555, streams_table,},
	{0}
};

BIG_STATIC int
strsysctl_init(void)
{
	sysctl_str_maxpsz = STRMAXPSZ;	/* stream head default max packet size */
	sysctl_str_minpsz = STRMINPSZ;	/* stream head default min packet size */
	sysctl_str_hiwat = STRHIGH;	/* stream head default hi water mark */
	sysctl_str_lowat = STRLOW;	/* stream head default lo water mark */
#ifdef HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT
	if ((streams_sysctl_handle = register_sysctl_table(streams_root_table)))
		return (0);
#else				/* HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT */
	if ((streams_sysctl_handle = register_sysctl_table(streams_root_table, 1)))
		return (0);
#endif				/* HAVE_KMEMB_STRUCT_CTL_TABLE_PARENT */
	return (-EIO);
}

BIG_STATIC void
strsysctl_exit(void)
{
	unregister_sysctl_table(streams_sysctl_handle);
	streams_sysctl_handle = NULL;
}
