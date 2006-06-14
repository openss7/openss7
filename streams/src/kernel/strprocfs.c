/*****************************************************************************

 @(#) $RCSfile: strprocfs.c,v $ $Name:  $($Revision: 0.9.2.52 $) $Date: 2006/06/14 10:37:23 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/06/14 10:37:23 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strprocfs.c,v $
 Revision 0.9.2.52  2006/06/14 10:37:23  brian
 - defeat a lot of debug traces in debug mode for testing
 - changes to allow strinet to compile under LiS (why???)

 Revision 0.9.2.51  2006/05/29 08:53:00  brian
 - started zero copy architecture

 Revision 0.9.2.50  2006/02/20 10:59:21  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ident "@(#) $RCSfile: strprocfs.c,v $ $Name:  $($Revision: 0.9.2.52 $) $Date: 2006/06/14 10:37:23 $"

static char const ident[] =
    "$RCSfile: strprocfs.c,v $ $Name:  $($Revision: 0.9.2.52 $) $Date: 2006/06/14 10:37:23 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/proc_fs.h>

#include "sys/strdebug.h"

#include <sys/strsubr.h>

#include "sys/config.h"
#include "strlookup.h"
#include "strsched.h"
#include "strprocfs.h"		/* extern verification */

#ifdef CONFIG_PROC_FS
extern struct proc_dir_entry *proc_str;

/* 
 *  -------------------------------------------------------------------------
 *
 *  Proc Filesystem Interface
 *
 *  -------------------------------------------------------------------------
 */

#undef snprintf
#define snprintf safe_snprintf

STATIC int snprintf(char *buf, int size, const char *fmt, ...)
    __attribute__ ((format(printf, 3, 4)));
STATIC int
snprintf(char *buf, int size, const char *fmt, ...)
{
	int count = 0;

	if (size > 0) {
		va_list args;

		va_start(args, fmt);
		count = vsnprintf(buf, size, fmt, args);
		va_end(args);
	}
	return (count);
}

#if 0
STATIC int
get_streams_driver(char *page, int maxlen, struct cdevsw *d)
{
	int len = 0;

	if (!d)
		goto done;
      done:
	return (len);
}

/* called by proc file system to list the registered STREAMS devices */
STATIC int
get_streams_drivers_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 1024;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct list_head *cur;

	if (offset < maxlen) {
		get_streams_driver_hdr(buffer, maxlen - 1);
		len = sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	read_lock(&cdevsw_lock);
	list_for_each(cur, &cdevsw_list) {
		struct cdevsw *d = list_entry(cur, struct cdevsw, d_list);

		if ((pos += maxlen) > offset) {
			get_streams_driver(buffer, maxlen - 1, d);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	read_unlock(&cdevsw_lock);
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return (len);
}

STATIC int
get_streams_module(char *page, int maxlen, struct cdevsw *d)
{
	int len = 0;

	if (!d)
		goto done;
      done:
	return (len);
}

/* called by proc file system to list the registered STREAMS modules */
STATIC int
get_streams_modules_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 1024;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct list_head *cur;

	if (offset < maxlen) {
		get_streams_module_hdr(buffer, maxlen - 1);
		len = sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	read_lock(&fmodsw_lock);
	list_for_each(cur, &fmodsw_list) {
		struct fmodsw *f = list_entry(cur, struct fmodsw, f_list);

		if ((pos += maxlen) > offset) {
			get_streams_module(buffer, maxlen - 1, f);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	read_unlock(&fmodsw_lock);
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return (len);
}
#endif

STATIC int
get_streams_module_info_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "mi");
	len += snprintf(page + len, maxlen - len, ", mi_idnum");
	len += snprintf(page + len, maxlen - len, ", mi_idname");
	len += snprintf(page + len, maxlen - len, ", mi_minpsz");
	len += snprintf(page + len, maxlen - len, ", mi_maxpsz");
	len += snprintf(page + len, maxlen - len, ", mi_hiwat");
	len += snprintf(page + len, maxlen - len, ", mi_lowat");
	return (len);
}
STATIC int
get_streams_module_info(char *page, int maxlen, struct module_info *mi)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", mi);
	if (!mi)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %hu", mi->mi_idnum);
	len += snprintf(page + len, maxlen - len, ", %s", mi->mi_idname);
#ifdef __LP64__
	len += snprintf(page + len, maxlen - len, ", %ld", mi->mi_minpsz);
	len += snprintf(page + len, maxlen - len, ", %ld", mi->mi_maxpsz);
	len += snprintf(page + len, maxlen - len, ", %lu", mi->mi_hiwat);
	len += snprintf(page + len, maxlen - len, ", %lu", mi->mi_lowat);
#else
	len += snprintf(page + len, maxlen - len, ", %d", mi->mi_minpsz);
	len += snprintf(page + len, maxlen - len, ", %d", mi->mi_maxpsz);
	len += snprintf(page + len, maxlen - len, ", %u", mi->mi_hiwat);
	len += snprintf(page + len, maxlen - len, ", %u", mi->mi_lowat);
#endif
      done:
	return (len);
}
STATIC int
get_streams_module_stat_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "ms");
	len += snprintf(page + len, maxlen - len, ", ms_pcnt");
	len += snprintf(page + len, maxlen - len, ", ms_scnt");
	len += snprintf(page + len, maxlen - len, ", ms_ocnt");
	len += snprintf(page + len, maxlen - len, ", ms_ccnt");
	len += snprintf(page + len, maxlen - len, ", ms_acnt");
	len += snprintf(page + len, maxlen - len, ", ms_xptr");
	len += snprintf(page + len, maxlen - len, ", ms_xsize");
	len += snprintf(page + len, maxlen - len, ", ms_flags");
	return (len);
}
STATIC int
get_streams_module_stat(char *page, int maxlen, struct module_stat *ms)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", ms);
	if (!ms)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %ld", ms->ms_pcnt);
	len += snprintf(page + len, maxlen - len, ", %ld", ms->ms_scnt);
	len += snprintf(page + len, maxlen - len, ", %ld", ms->ms_ocnt);
	len += snprintf(page + len, maxlen - len, ", %ld", ms->ms_ccnt);
	len += snprintf(page + len, maxlen - len, ", %ld", ms->ms_acnt);
	len += snprintf(page + len, maxlen - len, ", %p", ms->ms_xptr);
	len += snprintf(page + len, maxlen - len, ", %hd", ms->ms_xsize);
	len += snprintf(page + len, maxlen - len, ", %#08x", ms->ms_flags);
      done:
	return (len);
}
STATIC int
get_streams_qinit_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "qi");
	len += snprintf(page + len, maxlen - len, ", qi_putp");
	len += snprintf(page + len, maxlen - len, ", qi_srvp");
	len += snprintf(page + len, maxlen - len, ", qi_qopen");
	len += snprintf(page + len, maxlen - len, ", qi_qclose");
	len += snprintf(page + len, maxlen - len, ", qi_qadmin");
	len += snprintf(page + len, maxlen - len, ", qi_minfo { ");
	len += get_streams_module_info_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, qi_mstat { ");
	len += get_streams_module_stat_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_qinit(char *page, int maxlen, struct qinit *qi)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", qi);
	if (!qi)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %p", qi->qi_putp);
	len += snprintf(page + len, maxlen - len, ", %p", qi->qi_srvp);
	len += snprintf(page + len, maxlen - len, ", %p", qi->qi_qopen);
	len += snprintf(page + len, maxlen - len, ", %p", qi->qi_qclose);
	len += snprintf(page + len, maxlen - len, ", %p", qi->qi_qadmin);
	len += snprintf(page + len, maxlen - len, ", %p { ", qi->qi_minfo);
	len += get_streams_module_info(page + len, maxlen - len, qi->qi_minfo);
	len += snprintf(page + len, maxlen - len, " }, %p { ", qi->qi_mstat);
	len += get_streams_module_stat(page + len, maxlen - len, qi->qi_mstat);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}
STATIC int
get_streams_streamtab_drv_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "st");
	len += snprintf(page + len, maxlen - len, ", st_rdinit { ");
	len += get_streams_qinit_hdr(page + len, maxlen - len);
//      len += snprintf(page + len, maxlen - len, " }, st_wrinit { ");
//      len += get_streams_qinit_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, st_muxrinit { ");
	len += get_streams_qinit_hdr(page + len, maxlen - len);
//      len += snprintf(page + len, maxlen - len, " }, st_muxwinit { ");
//      len += get_streams_qinit_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_streamtab_drv(char *page, int maxlen, struct streamtab *st)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", st);
	if (!st)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %p { ", st->st_rdinit);
	len += get_streams_qinit(page + len, maxlen - len, st->st_rdinit);
//      len += snprintf(page + len, maxlen - len, " }, %p { ", st->st_wrinit);
//      len += get_streams_qinit(page + len, maxlen - len, st->st_wrinit);
	len += snprintf(page + len, maxlen - len, " }, %p { ", st->st_muxrinit);
	len += get_streams_qinit(page + len, maxlen - len, st->st_muxrinit);
//      len += snprintf(page + len, maxlen - len, " }, %p { ", st->st_muxwinit);
//      len += get_streams_qinit(page + len, maxlen - len, st->st_muxwinit);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}
STATIC int
get_streams_strapush_list_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, ", sap [, sap]");
	return (len);
}
STATIC int
get_streams_strapush_list(char *page, int maxlen, struct list_head *list)
{
	int len = 0;
	struct list_head *cur, *tmp;

	ensure(list->next, INIT_LIST_HEAD(list));

	list_for_each_safe(cur, tmp, list) {
		struct strapush *sap = (struct strapush *) list_entry(cur, struct apinfo, api_more);

		len += snprintf(page + len, maxlen - len, ", %p", sap);
	}
	return (len);
}
STATIC int
get_streams_cdevsw_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "dp");
	len += snprintf(page + len, maxlen - len, ", d_name");
	len += snprintf(page + len, maxlen - len, ", d_str { ");
	len += get_streams_streamtab_drv_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, d_flag");
	len += snprintf(page + len, maxlen - len, ", d_modid");
	len += snprintf(page + len, maxlen - len, ", d_count");
	len += snprintf(page + len, maxlen - len, ", d_sqlvl");
	len += snprintf(page + len, maxlen - len, ", d_syncq");
	len += snprintf(page + len, maxlen - len, ", d_kmod");
	len += snprintf(page + len, maxlen - len, ", d_major");
	len += snprintf(page + len, maxlen - len, ", d_inode");
	len += snprintf(page + len, maxlen - len, ", d_mode");
	len += snprintf(page + len, maxlen - len, ", d_fop");
	len += snprintf(page + len, maxlen - len, ", d_apush { ");
	len += get_streams_strapush_list_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_cdevsw(char *page, int maxlen, struct cdevsw *d)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", d);
	if (!d)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %s", d->d_name);
	len += snprintf(page + len, maxlen - len, ", %p { ", d->d_str);
	len += get_streams_streamtab_drv(page + len, maxlen - len, d->d_str);
	len += snprintf(page + len, maxlen - len, " }, %#04hx", d->d_flag);
	len += snprintf(page + len, maxlen - len, ", %d", d->d_modid);
	len += snprintf(page + len, maxlen - len, ", %hu", atomic_read(&d->d_count));
	len += snprintf(page + len, maxlen - len, ", %d", d->d_sqlvl);
	len += snprintf(page + len, maxlen - len, ", %p", d->d_syncq);
	len += snprintf(page + len, maxlen - len, ", %p", d->d_kmod);
	len += snprintf(page + len, maxlen - len, ", %d", d->d_major);
	len += snprintf(page + len, maxlen - len, ", %p", d->d_inode);
	len += snprintf(page + len, maxlen - len, ", %d", d->d_mode);
	len += snprintf(page + len, maxlen - len, ", %p", d->d_fop);
	len += snprintf(page + len, maxlen - len, ", { ");
	len += get_streams_strapush_list(page + len, maxlen - len, &d->d_apush);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}

/* called by proc file system to list the registered STREAMS devices */
STATIC int
get_streams_cdevsw_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 1024;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct list_head *cur;

	if (offset < maxlen) {
		get_streams_cdevsw_hdr(buffer, maxlen - 1);
		len = sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	read_lock(&cdevsw_lock);
	list_for_each(cur, &cdevsw_list) {
		struct cdevsw *d = list_entry(cur, struct cdevsw, d_list);

		if ((pos += maxlen) > offset) {
			get_streams_cdevsw(buffer, maxlen - 1, d);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	read_unlock(&cdevsw_lock);
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return (len);
}

STATIC int
get_streams_streamtab_mod_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "st");
	len += snprintf(page + len, maxlen - len, ", st_rdinit { ");
	len += get_streams_qinit_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, st_wrinit { ");
	len += get_streams_qinit_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_streamtab_mod(char *page, int maxlen, struct streamtab *st)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", st);
	if (!st)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %p { ", st->st_rdinit);
	len += get_streams_qinit(page + len, maxlen - len, st->st_rdinit);
	len += snprintf(page + len, maxlen - len, " }, %p { ", st->st_wrinit);
	len += get_streams_qinit(page + len, maxlen - len, st->st_wrinit);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}
STATIC int
get_streams_fmodsw_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "f");
	len += snprintf(page + len, maxlen - len, ", f_name");
	len += snprintf(page + len, maxlen - len, ", f_str { ");
	len += get_streams_streamtab_mod_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, f_flag");
	len += snprintf(page + len, maxlen - len, ", f_modid");
	len += snprintf(page + len, maxlen - len, ", f_count");
	len += snprintf(page + len, maxlen - len, ", f_sqlvl");
	len += snprintf(page + len, maxlen - len, ", f_syncq");
	len += snprintf(page + len, maxlen - len, ", f_kmod");
	return (len);
}
STATIC int
get_streams_fmodsw(char *page, int maxlen, struct fmodsw *f)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", f);
	if (!f)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %s", f->f_name);
	len += snprintf(page + len, maxlen - len, ", %p { ", f->f_str);
	len += get_streams_streamtab_mod(page + len, maxlen - len, f->f_str);
	len += snprintf(page + len, maxlen - len, " }, %#04hx", f->f_flag);
	len += snprintf(page + len, maxlen - len, ", %d", f->f_modid);
	len += snprintf(page + len, maxlen - len, ", %d", atomic_read(&f->f_count));
	len += snprintf(page + len, maxlen - len, ", %d", f->f_sqlvl);
	len += snprintf(page + len, maxlen - len, ", %p", f->f_syncq);
	len += snprintf(page + len, maxlen - len, ", %p", f->f_kmod);
      done:
	return (len);
}

/* called by proc file system to list the registered STREAMS modules */
STATIC int
get_streams_fmodsw_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 1024;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct list_head *cur;

	if (offset < maxlen) {
		get_streams_fmodsw_hdr(buffer, maxlen - 1);
		len = sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	read_lock(&fmodsw_lock);
	list_for_each(cur, &fmodsw_list) {
		struct fmodsw *f = list_entry(cur, struct fmodsw, f_list);

		if ((pos += maxlen) > offset) {
			get_streams_fmodsw(buffer, maxlen - 1, f);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	read_unlock(&fmodsw_lock);
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return (len);
}

STATIC char *dyn_name[DYN_SIZE] = {
	"DYN_STREAM:",
	"DYN_QUEUE:",
	"DYN_MSGBLOCK:",
	"DYN_MDBBLOCK:",
	"DYN_LINKBLK:",
	"DYN_STREVENT:",
	"DYN_QBAND:",
	"DYN_STRAPUSH:",
	"DYN_DEVINFO:",
	"DYN_MODINFO:",
#if defined CONFIG_STREAMS_SYNCQS
	"DYN_SYNQ:",
#endif
};

/* list the strinfo structure counts */
STATIC int
get_streams_strinfo_data(char *page, int maxlen)
{
	int j, len = 0;

	/* we don't need to lock anything here, the counts are allowed to change as we are listing
	   them */
	for (j = 0; j < DYN_SIZE; j++)
		len += snprintf(page + len, maxlen - len, "%14s %012d %012d\n", dyn_name[j],
				atomic_read(&Strinfo[j].si_cnt), Strinfo[j].si_hwl);
	return len;
}
STATIC int
get_streams_strinfo_list(char *page, char **start, off_t offset, int length)
{
	int len;

	len = get_streams_strinfo_data(page, length + 1);
	if (offset >= len) {
		*start = page;
		return (0);
	} else {
		*start = page + offset;
		len -= offset;
		if (len > length)
			len = length;
		if (len < 0)
			len = 0;
		return (len);
	}
}

#if defined CONFIG_STREAMS_DEBUG
STATIC int
get_streams_stdata_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "sd");
	len += snprintf(page + len, maxlen - len, ", sd_rq");
	len += snprintf(page + len, maxlen - len, ", sd_wq");
	len += snprintf(page + len, maxlen - len, ", sd_iocblk");
	len += snprintf(page + len, maxlen - len, ", sd_other");
//	len += snprintf(page + len, maxlen - len, ", sd_strtab");
//      len += snprintf(page + len, maxlen - len, ", sd_inode");
	len += snprintf(page + len, maxlen - len, ", sd_flag");
	len += snprintf(page + len, maxlen - len, ", sd_rdopt");
	len += snprintf(page + len, maxlen - len, ", sd_wropt");
	len += snprintf(page + len, maxlen - len, ", sd_eropt");
	len += snprintf(page + len, maxlen - len, ", sd_iocid");
//      len += snprintf(page + len, maxlen - len, ", sd_iocwait");
	len += snprintf(page + len, maxlen - len, ", sd_session");
	len += snprintf(page + len, maxlen - len, ", sd_pgrp");
	len += snprintf(page + len, maxlen - len, ", sd_wroff");
	len += snprintf(page + len, maxlen - len, ", sd_wrpad");
	len += snprintf(page + len, maxlen - len, ", sd_rerror");
	len += snprintf(page + len, maxlen - len, ", sd_werror");
	len += snprintf(page + len, maxlen - len, ", sd_opens");
	len += snprintf(page + len, maxlen - len, ", sd_readers");
	len += snprintf(page + len, maxlen - len, ", sd_writers");
//      len += snprintf(page + len, maxlen - len, ", sd_rwaiters");
//      len += snprintf(page + len, maxlen - len, ", sd_wwaiters");
	len += snprintf(page + len, maxlen - len, ", sd_pushcnt");
	len += snprintf(page + len, maxlen - len, ", sd_nanchor");
	len += snprintf(page + len, maxlen - len, ", sd_sigflags");
	len += snprintf(page + len, maxlen - len, ", sd_siglist");
	len += snprintf(page + len, maxlen - len, ", sd_fasync");
//      len += snprintf(page + len, maxlen - len, ", sd_waitq");
//      len += snprintf(page + len, maxlen - len, ", sd_mark");
	len += snprintf(page + len, maxlen - len, ", sd_closetime");
	len += snprintf(page + len, maxlen - len, ", sd_rtime");
	len += snprintf(page + len, maxlen - len, ", sd_ioctime");
//      len += snprintf(page + len, maxlen - len, ", sd_qlock");
// 	len += snprintf(page + len, maxlen - len, ", sd_owner");
// 	len += snprintf(page + len, maxlen - len, ", sd_nest");
//      len += snprintf(page + len, maxlen - len, ", sd_mutex");
	len += snprintf(page + len, maxlen - len, ", sd_links");
	len += snprintf(page + len, maxlen - len, ", sd_link_next");
	len += snprintf(page + len, maxlen - len, ", sd_linkblk");
	return (len);
}
STATIC int
get_streams_stdata(char *page, int maxlen, struct stdata *sd)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", sd);
	if (!sd)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_rq);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_wq);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_iocblk);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_other);
//	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_strtab);
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_inode);
	len += snprintf(page + len, maxlen - len, ", %#08lx", sd->sd_flag);
	len += snprintf(page + len, maxlen - len, ", %#08lx", sd->sd_rdopt);
	len += snprintf(page + len, maxlen - len, ", %#08lx", sd->sd_wropt);
	len += snprintf(page + len, maxlen - len, ", %#08lx", sd->sd_eropt);
	len += snprintf(page + len, maxlen - len, ", %lu", sd->sd_iocid);
//      len += snprintf(page + len, maxlen - len, ", %hu", sd->sd_iocwait);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_session);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_pgrp);
	len += snprintf(page + len, maxlen - len, ", %hu", sd->sd_wroff);
	len += snprintf(page + len, maxlen - len, ", %hu", sd->sd_wrpad);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_rerror);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_werror);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_opens);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_readers);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_writers);
//      len += snprintf(page + len, maxlen - len, ", %d", sd->sd_rwaiters);
//      len += snprintf(page + len, maxlen - len, ", %d", sd->sd_wwaiters);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_pushcnt);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_nanchor);
	len += snprintf(page + len, maxlen - len, ", %#08lx", sd->sd_sigflags);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_siglist);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_fasync);
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_waitq);
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_mark);
	len += snprintf(page + len, maxlen - len, ", %lu", sd->sd_closetime);
	len += snprintf(page + len, maxlen - len, ", %lu", sd->sd_rtime);
	len += snprintf(page + len, maxlen - len, ", %lu", sd->sd_ioctime);
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_qlock);
// 	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_owner);
// 	len += snprintf(page + len, maxlen - len, ", %u", sd->sd_nest);
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_mutex);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_links);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_link_next);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_linkblk);
      done:
	return (len);
}
STATIC int
get_streams_shinfo_data_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "sh");
	len += snprintf(page + len, maxlen - len, ", sh_stdata { ");
	len += get_streams_stdata_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_shinfo_data(char *page, int maxlen, struct shinfo *sh)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "sh: %p", sh);
	if (!sh)
		goto done;
	len += snprintf(page + len, maxlen - len, ", sh_stdata: { ");
	len += get_streams_stdata(page + len, maxlen - len, &sh->sh_stdata);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}

/* list stream head information for allocated streams */
STATIC int
get_streams_shinfo_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 512;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct strinfo *si = &Strinfo[DYN_STREAM];
	struct list_head *cur, *tmp;

	if (offset < maxlen) {
		get_streams_shinfo_data_hdr(buffer, maxlen - 1);
		len = sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	list_for_each_safe(cur, tmp, &si->si_head) {
		struct shinfo *sh = list_entry(cur, struct shinfo, sh_list);

		if ((pos += maxlen) > offset) {
			get_streams_shinfo_data(buffer, maxlen - 1, sh);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

STATIC int
get_streams_queue_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "q");
	len += snprintf(page + len, maxlen - len, ", q_qinfo");
	len += snprintf(page + len, maxlen - len, ", q_first");
	len += snprintf(page + len, maxlen - len, ", q_last");
	len += snprintf(page + len, maxlen - len, ", q_next");
	len += snprintf(page + len, maxlen - len, ", q_link");
	len += snprintf(page + len, maxlen - len, ", q_ptr");
	len += snprintf(page + len, maxlen - len, ", q_count");
	len += snprintf(page + len, maxlen - len, ", q_flag");
	len += snprintf(page + len, maxlen - len, ", q_minpsz");
	len += snprintf(page + len, maxlen - len, ", q_maxpsz");
	len += snprintf(page + len, maxlen - len, ", q_hiwat");
	len += snprintf(page + len, maxlen - len, ", q_lowat");
	len += snprintf(page + len, maxlen - len, ", q_bandp");
	len += snprintf(page + len, maxlen - len, ", q_nband");
	len += snprintf(page + len, maxlen - len, ", q_msgs");
	// len += snprintf(page + len, maxlen - len, ", q_rwlock");
	// len += snprintf(page + len, maxlen - len, ", q_iflags");
	return (len);
}
STATIC int
get_streams_queue(char *page, int maxlen, queue_t *q)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", q);
	if (!q)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %p", q->q_qinfo);
	len += snprintf(page + len, maxlen - len, ", %p", q->q_first);
	len += snprintf(page + len, maxlen - len, ", %p", q->q_last);
	len += snprintf(page + len, maxlen - len, ", %p", q->q_next);
	len += snprintf(page + len, maxlen - len, ", %p", q->q_link);
	len += snprintf(page + len, maxlen - len, ", %p", q->q_ptr);
	len += snprintf(page + len, maxlen - len, ", %lu", (ulong) q->q_count);
	len += snprintf(page + len, maxlen - len, ", %#08lx", q->q_flag);
	len += snprintf(page + len, maxlen - len, ", %ld", (long) q->q_minpsz);
	len += snprintf(page + len, maxlen - len, ", %ld", (long) q->q_maxpsz);
	len += snprintf(page + len, maxlen - len, ", %lu", (ulong) q->q_hiwat);
	len += snprintf(page + len, maxlen - len, ", %lu", (ulong) q->q_lowat);
	len += snprintf(page + len, maxlen - len, ", %p", q->q_bandp);
	len += snprintf(page + len, maxlen - len, ", %hu", (ushort) q->q_nband);
	len += snprintf(page + len, maxlen - len, ", %ld", (long) q->q_msgs);
	// len += snprintf(page + len, maxlen - len, "%d", q->q_rwlock);
	// len += snprintf(page + len, maxlen - len, ", %lu", q->q_iflags);
      done:
	return (len);
}
STATIC int
get_streams_queinfo_data_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "qu");
	len += snprintf(page + len, maxlen - len, ", rq { ");
	len += get_streams_queue_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, wq { ");
	len += get_streams_queue_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, qu_str");
	len += snprintf(page + len, maxlen - len, ", qu_refs");
	len += snprintf(page + len, maxlen - len, ", qu_qwait");
	// len += snprintf(page + len, maxlen - len, ", qu_owner");
	// len += snprintf(page + len, maxlen - len, ", qu_nest");
	return (len);
}
STATIC int
get_streams_queinfo_data(char *page, int maxlen, struct queinfo *qu)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", qu);
	if (!qu)
		goto done;
	len += snprintf(page + len, maxlen - len, ", { ");
	len += get_streams_queue(page + len, maxlen - len, &qu->rq);
	len += snprintf(page + len, maxlen - len, " }, { ");
	len += get_streams_queue(page + len, maxlen - len, &qu->wq);
	len += snprintf(page + len, maxlen - len, " }, %p", qu->qu_str);
	len += snprintf(page + len, maxlen - len, ", %d", atomic_read(&qu->qu_refs));
	len += snprintf(page + len, maxlen - len, ", %d", waitqueue_active(&qu->qu_qwait));
	// len += snprintf(page + len, maxlen - len, ", %d", qu->qu_owner ? qu->qu_owner->pid : 0);
	// len += snprintf(page + len, maxlen - len, ", %u", qu->qu_nest);
      done:
	return (len);
}

/* list stream head information for allocated queues */
STATIC int
get_streams_queinfo_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 512;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct strinfo *si = &Strinfo[DYN_QUEUE];
	struct list_head *cur, *tmp;

	if (offset < maxlen) {
		get_streams_queinfo_data_hdr(buffer, maxlen - 1);
		len = sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	list_for_each_safe(cur, tmp, &si->si_head) {
		struct queinfo *qu = list_entry(cur, struct queinfo, qu_list);

		if ((pos += maxlen) > offset) {
			get_streams_queinfo_data(buffer, maxlen - 1, qu);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

STATIC int
get_streams_msgb_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "b");
	len += snprintf(page + len, maxlen - len, ", b_next");
	len += snprintf(page + len, maxlen - len, ", b_prev");
	len += snprintf(page + len, maxlen - len, ", b_cont");
	len += snprintf(page + len, maxlen - len, ", b_rptr");
	len += snprintf(page + len, maxlen - len, ", b_wptr");
	len += snprintf(page + len, maxlen - len, ", b_datap");
	len += snprintf(page + len, maxlen - len, ", b_band");
	len += snprintf(page + len, maxlen - len, ", b_flag");
	return (len);
}
STATIC int
get_streams_msgb(char *page, int maxlen, struct msgb *b)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", b);
	if (!b)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %p", b->b_next);
	len += snprintf(page + len, maxlen - len, ", %p", b->b_prev);
	len += snprintf(page + len, maxlen - len, ", %p", b->b_cont);
	len += snprintf(page + len, maxlen - len, ", %p", b->b_rptr);
	len += snprintf(page + len, maxlen - len, ", %p", b->b_wptr);
	len += snprintf(page + len, maxlen - len, ", %p", b->b_datap);
	len += snprintf(page + len, maxlen - len, ", %hu", (ushort) b->b_band);
	len += snprintf(page + len, maxlen - len, ", %hu", b->b_flag);
      done:
	return (len);
}
STATIC int
get_streams_mbinfo_data_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "m");
	len += snprintf(page + len, maxlen - len, ", m_mblock { ");
	len += get_streams_msgb_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, m_func");
	len += snprintf(page + len, maxlen - len, ", m_queue");
	return (len);
}
STATIC int
get_streams_mbinfo_data(char *page, int maxlen, struct mbinfo *m)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "m: %p", m);
	if (!m)
		goto done;
	len += snprintf(page + len, maxlen - len, ", m_mblock: { ");
	len += get_streams_msgb(page + len, maxlen - len, &m->m_mblock);
	len += snprintf(page + len, maxlen - len, " }, m_func: %p", m->m_func);
	len += snprintf(page + len, maxlen - len, ", m_queue: %p", m->m_queue);
      done:
	return (len);
}

/* list stream head information for allocated message blocks */
STATIC int
get_streams_mbinfo_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 256;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct strinfo *si = &Strinfo[DYN_MSGBLOCK];
	struct list_head *cur, *tmp;

	if (offset < maxlen) {
		get_streams_mbinfo_data_hdr(buffer, maxlen - 1);
		len = sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	list_for_each_safe(cur, tmp, &si->si_head) {
		struct mbinfo *m = list_entry(cur, struct mbinfo, m_list);

		if ((pos += maxlen) > offset) {
			get_streams_mbinfo_data(buffer, maxlen - 1, m);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

STATIC int
get_streams_datab_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "db");
	len += snprintf(page + len, maxlen - len, ", db_frtnp");
	len += snprintf(page + len, maxlen - len, ", db_base");
	len += snprintf(page + len, maxlen - len, ", db_lim");
	len += snprintf(page + len, maxlen - len, ", db_ref");
	len += snprintf(page + len, maxlen - len, ", db_type");
	len += snprintf(page + len, maxlen - len, ", db_class");
	len += snprintf(page + len, maxlen - len, ", db_size");
	len += snprintf(page + len, maxlen - len, ", db_users");
	return (len);
}
STATIC int
get_streams_datab(char *page, int maxlen, struct datab *db)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "db: %p", db);
	if (!db)
		goto done;
	len += snprintf(page + len, maxlen - len, ", db_frtnp: %p", db->db_frtnp);
	len += snprintf(page + len, maxlen - len, ", db_base: %p", db->db_base);
	len += snprintf(page + len, maxlen - len, ", db_lim: %p", db->db_lim);
	len += snprintf(page + len, maxlen - len, ", db_ref: %hu", (ushort) db->db_ref);
	len += snprintf(page + len, maxlen - len, ", db_type: %hu", (ushort) db->db_type);
	len += snprintf(page + len, maxlen - len, ", db_class: %hu", (ushort) db->db_class);
	len += snprintf(page + len, maxlen - len, ", db_size: %d:", db->db_size);
	// len += snprintf(page + len, maxlen - len, ", db_users: %d", atomic_read(&db->db_users));
      done:
	return (len);
}
STATIC int
get_streams_dbinfo_data_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "db");
	len += snprintf(page + len, maxlen - len, ", d_dblock { ");
	len += get_streams_datab_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_dbinfo_data(char *page, int maxlen, struct dbinfo *db)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", db);
	if (!db)
		goto done;
	len += snprintf(page + len, maxlen - len, ", { ");
	len += get_streams_datab(page + len, maxlen - len, &db->d_dblock);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}

/* list stream head information for allocated data blocks */
STATIC int
get_streams_dbinfo_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 256;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct strinfo *si = &Strinfo[DYN_MDBBLOCK];
	struct list_head *cur, *tmp;

	if (offset < maxlen) {
		get_streams_dbinfo_data_hdr(buffer, maxlen - 1);
		len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	list_for_each_safe(cur, tmp, &si->si_head) {
		struct dbinfo *db = list_entry(cur, struct dbinfo, db_list);

		if ((pos += maxlen) > offset) {
			get_streams_dbinfo_data(buffer, maxlen - 1, db);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

STATIC int
get_streams_linkblk_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "l");
	len += snprintf(page + len, maxlen - len, ", l_qtop");
	len += snprintf(page + len, maxlen - len, ", l_qbot");
	len += snprintf(page + len, maxlen - len, ", l_index");
	return (len);
}
STATIC int
get_streams_linkblk(char *page, int maxlen, struct linkblk *l)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", l);
	if (!l)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %p", l->l_qtop);
	len += snprintf(page + len, maxlen - len, ", %p", l->l_qbot);
	len += snprintf(page + len, maxlen - len, ", %d", l->l_index);
      done:
	return (len);
}
STATIC int
get_streams_linkinfo_data_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "li");
	len += snprintf(page + len, maxlen - len, ", li_linkblk { ");
	len += get_streams_linkblk_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_linkinfo_data(char *page, int maxlen, struct linkinfo *li)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", li);
	if (!li)
		goto done;
	len += snprintf(page + len, maxlen - len, ", { ");
	len += get_streams_linkblk(page + len, maxlen - len, &li->li_linkblk);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}

/* list stream head information for allocated link blocks */
STATIC int
get_streams_linkinfo_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 256;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct strinfo *si = &Strinfo[DYN_LINKBLK];
	struct list_head *cur, *tmp;

	if (offset < maxlen) {
		get_streams_linkinfo_data_hdr(buffer, maxlen - 1);
		len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	list_for_each_safe(cur, tmp, &si->si_head) {
		struct linkinfo *li = list_entry(cur, struct linkinfo, li_list);

		if ((pos += maxlen) > offset) {
			get_streams_linkinfo_data(buffer, maxlen - 1, li);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

STATIC int
get_streams_strevent_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "se");
	len += snprintf(page + len, maxlen - len, ", { SE_STREAM x.e.procp");
	len += snprintf(page + len, maxlen - len, ", x.e.events ");
	len += snprintf(page + len, maxlen - len, "| SE_BUFCALL x.b.queue");
	len += snprintf(page + len, maxlen - len, ", x.b.func");
	len += snprintf(page + len, maxlen - len, ", x.b.arg");
	len += snprintf(page + len, maxlen - len, ", x.b.size ");
	len += snprintf(page + len, maxlen - len, "| SE_TIMEOUT x.t.queue");
	len += snprintf(page + len, maxlen - len, ", x.t.func");
	len += snprintf(page + len, maxlen - len, ", x.t.arg");
	len += snprintf(page + len, maxlen - len, ", x.t.cpu");
	len += snprintf(page + len, maxlen - len, ", x.t.timer.expires }");
	len += snprintf(page + len, maxlen - len, ", se_id");
	len += snprintf(page + len, maxlen - len, ", se_seq");
	return (len);
}
STATIC int
get_streams_strevent(char *page, int maxlen, int type, struct strevent *se)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", se);
	if (!se)
		goto done;
	switch (type) {
	case SE_STREAM:
		len += snprintf(page + len, maxlen - len, ", { SE_STREAM %p", se->x.e.procp);
		len += snprintf(page + len, maxlen - len, ", %ld }", se->x.e.events);
	case SE_BUFCALL:
		len += snprintf(page + len, maxlen - len, ", { SE_BUFCALL %p", se->x.b.queue);
		len += snprintf(page + len, maxlen - len, ", %p", se->x.b.func);
		len += snprintf(page + len, maxlen - len, ", %ld", se->x.b.arg);
		len += snprintf(page + len, maxlen - len, ", %lu }", (ulong) se->x.b.size);
	case SE_TIMEOUT:
		len += snprintf(page + len, maxlen - len, ", { SE_TIMEOUT %p", se->x.t.queue);
		len += snprintf(page + len, maxlen - len, ", %p", se->x.t.func);
		len += snprintf(page + len, maxlen - len, ", %p", se->x.t.arg);
		len += snprintf(page + len, maxlen - len, ", %d", se->x.t.cpu);
		len += snprintf(page + len, maxlen - len, ", %lu }", se->x.t.timer.expires);
	}
	len += snprintf(page + len, maxlen - len, ", %d", se->se_id);
	len += snprintf(page + len, maxlen - len, ", %d", se->se_seq);
      done:
	return (len);
}
STATIC int
get_streams_seinfo_data_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "s");
	len += snprintf(page + len, maxlen - len, ", s_strevent { ");
	len += get_streams_strevent_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, s_type");
	len += snprintf(page + len, maxlen - len, ", s_queue");
	return (len);
}
STATIC int
get_streams_seinfo_data(char *page, int maxlen, struct seinfo *s)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", s);
	if (!s)
		goto done;
	len += snprintf(page + len, maxlen - len, ", { ");
	len += get_streams_strevent(page + len, maxlen - len, s->s_type, &s->s_strevent);
	len += snprintf(page + len, maxlen - len, " }, %d", s->s_type);
	len += snprintf(page + len, maxlen - len, ", %p", s->s_queue);
      done:
	return (len);
}

/* list stream head information for allocated strevents */
STATIC int
get_streams_seinfo_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 256;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct strinfo *si = &Strinfo[DYN_STREVENT];
	struct list_head *cur, *tmp;

	if (offset < maxlen) {
		get_streams_seinfo_data_hdr(buffer, maxlen - 1);
		len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	list_for_each_safe(cur, tmp, &si->si_head) {
		struct seinfo *s = list_entry(cur, struct seinfo, s_list);

		if ((pos += maxlen) > offset) {
			get_streams_seinfo_data(buffer, maxlen - 1, s);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

STATIC int
get_streams_qband_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "qb");
	len += snprintf(page + len, maxlen - len, ", qb_next");
	len += snprintf(page + len, maxlen - len, ", qb_count");
	len += snprintf(page + len, maxlen - len, ", qb_first");
	len += snprintf(page + len, maxlen - len, ", qb_last");
	len += snprintf(page + len, maxlen - len, ", qb_hiwat");
	len += snprintf(page + len, maxlen - len, ", qb_lowat");
	len += snprintf(page + len, maxlen - len, ", qb_msgs");
	// len += snprintf(page + len, maxlen - len, ", qb_prev");
	// len += snprintf(page + len, maxlen - len, ", qb_band");
	return (len);
}
STATIC int
get_streams_qband(char *page, int maxlen, struct qband *qb)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", qb);
	if (!qb)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %p", qb->qb_next);
	len += snprintf(page + len, maxlen - len, ", %lu", (ulong) qb->qb_count);
	len += snprintf(page + len, maxlen - len, ", %p", qb->qb_first);
	len += snprintf(page + len, maxlen - len, ", %p", qb->qb_last);
	len += snprintf(page + len, maxlen - len, ", %ld", (long) qb->qb_hiwat);
	len += snprintf(page + len, maxlen - len, ", %ld", (long) qb->qb_lowat);
	len += snprintf(page + len, maxlen - len, ", %ld", qb->qb_msgs);
	// len += snprintf(page + len, maxlen - len, ", %p", qb->qb_prev);
	// len += snprintf(page + len, maxlen - len, ", %hu", (ushort) qb->qb_band);
      done:
	return (len);
}
STATIC int
get_streams_qbinfo_data_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "qbi");
	len += snprintf(page + len, maxlen - len, ", qbi_qband { ");
	len += get_streams_qband_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }, qbi_refs");
	return (len);
}
STATIC int
get_streams_qbinfo_data(char *page, int maxlen, struct qbinfo *qbi)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", qbi);
	if (!qbi)
		goto done;
	len += snprintf(page + len, maxlen - len, ", { ");
	len += get_streams_qband(page + len, maxlen - len, &qbi->qbi_qband);
	len += snprintf(page + len, maxlen - len, " }, %d", atomic_read(&qbi->qbi_refs));
      done:
	return (len);
}

/* list stream head information for allocated qbands */
STATIC int
get_streams_qbinfo_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 256;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct strinfo *si = &Strinfo[DYN_QBAND];
	struct list_head *cur, *tmp;

	if (offset < maxlen) {
		get_streams_qbinfo_data_hdr(buffer, maxlen - 1);
		len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	list_for_each_safe(cur, tmp, &si->si_head) {
		struct qbinfo *qbi = list_entry(cur, struct qbinfo, qbi_list);

		if ((pos += maxlen) > offset) {
			get_streams_qbinfo_data(buffer, maxlen - 1, qbi);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

STATIC int
get_streams_strapush_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "sap");
	len += snprintf(page + len, maxlen - len, ", sap_cmd");
	len += snprintf(page + len, maxlen - len, ", sap_major");
	len += snprintf(page + len, maxlen - len, ", sap_minor");
	len += snprintf(page + len, maxlen - len, ", sap_lastminor");
	len += snprintf(page + len, maxlen - len, ", sap_npush");
	len += snprintf(page + len, maxlen - len, ", sap_list { ");
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_strapush(char *page, int maxlen, struct strapush *sap)
{
	int i, len = 0;

	len += snprintf(page + len, maxlen - len, "%p", sap);
	if (!sap)
		goto done;
	len += snprintf(page + len, maxlen - len, ", %d", sap->sap_cmd);
	len += snprintf(page + len, maxlen - len, ", %ld", sap->sap_major);
	len += snprintf(page + len, maxlen - len, ", %ld", sap->sap_minor);
	len += snprintf(page + len, maxlen - len, ", %ld", sap->sap_lastminor);
	len += snprintf(page + len, maxlen - len, ", %u", sap->sap_npush);
	len += snprintf(page + len, maxlen - len, ", { ");
	for (i = 0; i < sap->sap_npush; i++)
		len += snprintf(page + len, FMNAMESZ + 1, " %s", sap->sap_list[i]);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}
STATIC int
get_streams_apinfo_data_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "api");
	len += snprintf(page + len, maxlen - len, ", api_sap { ");
	len += get_streams_strapush_hdr(page + len, maxlen - len);
	len += snprintf(page + len, maxlen - len, " }");
	return (len);
}
STATIC int
get_streams_apinfo_data(char *page, int maxlen, struct apinfo *api)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "%p", api);
	if (!api)
		goto done;
	len += snprintf(page + len, maxlen - len, ", { ");
	len += get_streams_strapush(page + len, maxlen - len, &api->api_sap);
	len += snprintf(page + len, maxlen - len, " }");
      done:
	return (len);
}

/* list stream head information for allocate strapush structures */
STATIC int
get_streams_apinfo_list(char *page, char **start, off_t offset, int length)
{
	int len = 0;
	static const int maxlen = 256;
	int num = 0;
	off_t begin, pos;
	char buffer[maxlen + 1];
	struct strinfo *si = &Strinfo[DYN_STRAPUSH];
	struct list_head *cur, *tmp;

	if (offset < maxlen) {
		get_streams_apinfo_data_hdr(buffer, maxlen - 1);
		len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
	}
	pos = maxlen;
	list_for_each_safe(cur, tmp, &si->si_head) {
		struct apinfo *api = list_entry(cur, struct apinfo, api_list);

		if ((pos += maxlen) > offset) {
			get_streams_apinfo_data(buffer, maxlen - 1, api);
			len += sprintf(page + len, "%-*s\n", maxlen - 1, buffer);
			if (pos >= offset + length)
				break;
		}
		num++;
	}
	begin = len - (pos - offset);
	*start = page + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}
#endif

struct proc_dir_entry *proc_str = NULL;

#endif				/* CONFIG_PROC_FS */

BIG_STATIC int
strprocfs_init(void)
{
#ifdef CONFIG_PROC_FS
	proc_str = proc_mkdir("streams", NULL);
	if (!proc_str)
		return (-ENOMEM);
#if 0
	create_proc_info_entry("drivers", 0444, proc_str, get_streams_drivers_list);
	create_proc_info_entry("modules", 0444, proc_str, get_streams_modules_list);
#else
	create_proc_info_entry("cdevsw", 0444, proc_str, get_streams_cdevsw_list);
	create_proc_info_entry("fmodsw", 0444, proc_str, get_streams_fmodsw_list);
#endif
	create_proc_info_entry("strinfo", 0444, proc_str, get_streams_strinfo_list);
#if defined CONFIG_STREAMS_DEBUG
#if 0
	create_proc_info_entry("cdevsw", 0444, proc_str, get_streams_cdevsw_list);
	create_proc_info_entry("fmodsw", 0444, proc_str, get_streams_fmodsw_list);
#endif
	create_proc_info_entry("stdata", 0444, proc_str, get_streams_shinfo_list);
	create_proc_info_entry("queue", 0444, proc_str, get_streams_queinfo_list);
	create_proc_info_entry("msgb", 0444, proc_str, get_streams_mbinfo_list);
	create_proc_info_entry("datab", 0444, proc_str, get_streams_dbinfo_list);
	create_proc_info_entry("linkblk", 0444, proc_str, get_streams_linkinfo_list);
	create_proc_info_entry("strevent", 0444, proc_str, get_streams_seinfo_list);
	create_proc_info_entry("qband", 0444, proc_str, get_streams_qbinfo_list);
	create_proc_info_entry("strapush", 0444, proc_str, get_streams_apinfo_list);
#endif
#endif				/* CONFIG_PROC_FS */
	return (0);
}

BIG_STATIC void
strprocfs_exit(void)
{
#ifdef CONFIG_PROC_FS
#if 0
	remove_proc_entry("drivers", proc_str);
	remove_proc_entry("modules", proc_str);
#else
	remove_proc_entry("cdevsw", proc_str);
	remove_proc_entry("fmodsw", proc_str);
#endif
	remove_proc_entry("strinfo", proc_str);
#if defined CONFIG_STREAMS_DEBUG
#if 0
	remove_proc_entry("cdevsw", proc_str);
	remove_proc_entry("fmodsw", proc_str);
#endif
	remove_proc_entry("stdata", proc_str);
	remove_proc_entry("queue", proc_str);
	remove_proc_entry("msgb", proc_str);
	remove_proc_entry("datab", proc_str);
	remove_proc_entry("linkblk", proc_str);
	remove_proc_entry("strevent", proc_str);
	remove_proc_entry("qband", proc_str);
	remove_proc_entry("strapush", proc_str);
#endif
	remove_proc_entry("streams", NULL);
#endif				/* CONFIG_PROC_FS */
	return;
}
