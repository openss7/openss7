/*****************************************************************************

 @(#) File: src/kernle/strprocfs.c

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

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/proc_fs.h>
#if defined HAVE_KINC_LINUX_SECURITY_H
#include <linux/security.h>	/* avoid ptrace conflict */
#endif
#if defined HAVE_KINC_LINUX_SEQ_FILE_H
#include <linux/seq_file.h>
#endif

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

#if defined HAVE_KFUNC_PROC_CREATE_DATA

#if 0
static void *
streams_drivers_proc_start(struct seq_file *m, loff_t *_pos)
{
}

static void *
streams_drivers_proc_next(struct seq_file *m, void *v, loff_t *_pos)
{
}

static void
streams_drivers_proc_stop(struct seq_file *m, void *v)
{
}

static void *
streams_drivers_proc_show(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_driver_proc_seqops = {
	.start = streams_drivers_proc_start,
	.next = streams_drivers_proc_next,
	.stop = streams_drivers_proc_stop,
	.show = streams_drivers_proc_show,
};

static int
streams_drivers_proc_open(struct inode *inode, struct file *file)
{
	int ret = seq_open(file, &streams_drivers_proc_seqops);

	if (ret == 0) {
		struct seq_file *m = file->private_data;

		m->private = PDE_DATA(inode);
	}
	return ret;
}

static const struct file_operations streams_drivers_proc_fops = {
	.open = streams_drivers_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};
#endif

/* ---------------------- */

STATIC void
seq_streams_module_info_hdr(struct seq_file *m)
{
	seq_printf(m, "mi");
	seq_printf(m, ", mi_idnum");
	seq_printf(m, ", mi_idname");
	seq_printf(m, ", mi_minpsz");
	seq_printf(m, ", mi_maxpsz");
	seq_printf(m, ", mi_hiwat");
	seq_printf(m, ", mi_lowat");
}

STATIC void
seq_streams_module_info(struct seq_file *m, struct module_info *mi)
{
	seq_printf(m, "%p", mi);
	seq_printf(m, "%hu", mi->mi_idnum);
	seq_printf(m, "%s", mi->mi_idname);
#ifdef __LP64__
	seq_printf(m, "%ld", mi->mi_minpsz);
	seq_printf(m, "%ld", mi->mi_maxpsz);
	seq_printf(m, "%lu", mi->mi_hiwat);
	seq_printf(m, "%lu", mi->mi_lowat);
#else
	seq_printf(m, "%d", mi->mi_minpsz);
	seq_printf(m, "%d", mi->mi_maxpsz);
	seq_printf(m, "%d", mi->mi_hiwat);
	seq_printf(m, "%d", mi->mi_lowat);
#endif
}

STATIC void
seq_streams_module_stat_hdr(struct seq_file *m)
{
	seq_printf(m, "ms");
	seq_printf(m, ", ms_pcnt");
	seq_printf(m, ", ms_scnt");
	seq_printf(m, ", ms_ocnt");
	seq_printf(m, ", ms_ccnt");
	seq_printf(m, ", ms_acnt");
	seq_printf(m, ", ms_xptr");
	seq_printf(m, ", ms_xsize");
	seq_printf(m, ", ms_flags");
}

STATIC void
seq_streams_module_stat(struct seq_file *m, struct module_stat *ms)
{
	seq_printf(m, "%p", ms);
	seq_printf(m, ", %ld", ms->ms_pcnt);
	seq_printf(m, ", %ld", ms->ms_scnt);
	seq_printf(m, ", %ld", ms->ms_ocnt);
	seq_printf(m, ", %ld", ms->ms_ccnt);
	seq_printf(m, ", %ld", ms->ms_acnt);
	seq_printf(m, ", %p", ms->ms_xptr);
	seq_printf(m, ", %hd", ms->ms_xsize);
	seq_printf(m, ", %#08x", ms->ms_flags);
}

STATIC void
seq_streams_qinit_hdr(struct seq_file *m)
{
	seq_printf(m, "qi");
	seq_printf(m, ", qi_putp");
	seq_printf(m, ", qi_srvp");
	seq_printf(m, ", qi_qopen");
	seq_printf(m, ", qi_qclose");
	seq_printf(m, ", qi_qadmin");
	seq_printf(m, ", qi_minfo { ");
	seq_streams_module_info_hdr(m);
	seq_printf(m, " }, qi_mstat { ");
	seq_streams_module_stat_hdr(m);
	seq_printf(m, " }");
}

STATIC void
seq_streams_qinit(struct seq_file *m, struct qinit *qi)
{
	seq_printf(m, "%p", qi);
	seq_printf(m, ", %p", qi->qi_putp);
	seq_printf(m, ", %p", qi->qi_srvp);
	seq_printf(m, ", %p", qi->qi_qopen);
	seq_printf(m, ", %p", qi->qi_qclose);
	seq_printf(m, ", %p", qi->qi_qadmin);
	seq_printf(m, ", %p { ", qi->qi_minfo);
	seq_streams_module_info(m, qi->qi_minfo);
	seq_printf(m, " }, %p { ", qi->qi_mstat);
	seq_streams_module_stat(m, qi->qi_mstat);
	seq_printf(m, " }");
}

STATIC void
seq_streams_streamtab_hdr(struct seq_file *m)
{
	seq_printf(m, "st");
	seq_printf(m, ", st_rdinit { ");
	seq_streams_qinit_hdr(m);
//      seq_printf(m, " }, st_wrinit { ");
//      seq_streams_qinit_hdr(m);
	seq_printf(m, " }, st_muxrinit { ");
	seq_streams_qinit_hdr(m);
//      seq_printf(m, " }, st_muxwinit { ");
//      seq_streams_qinit_hdr(m);
	seq_printf(m, " }");
}

STATIC void
seq_stream_streamtab(struct seq_file *m, struct streamtab *st)
{
	seq_printf(m, "%p", st);
	seq_printf(m, ", %p { ", st->st_rdinit);
	seq_streams_qinit(m, st->st_rdinit);
//      seq_printf(m, " }, %p { ", st->st_wrinit);
//      seq_streams_qinit(m, st->st_wrinit);
	seq_printf(m, " }, %p { ", st->st_muxrinit);
	seq_streams_qinit(m, st->st_muxrinit);
//      seq_printf(m, " }, %p { ", st->st_muxwinit);
//      seq_streams_qinit(m, st->st_muxwinit);
	seq_printf(m, " }");
}

STATIC void
seq_streams_strapush_hdr(struct seq_file *m)
{
	seq_printf(m, ", sap [, sap]");
}

STATIC void
seq_streams_strapush(struct seq_file *m, struct list_head *list)
{
	struct list_head *cur, *tmp;

	ensure(list->next, INIT_LIST_HEAD(list));

	list_for_each_safe(cur, tmp, list) {
		struct strapush *sap = (struct strapush *) list_entry(cur, struct apinfo, api_more);

		seq_printf(m, ", %p", sap);
	}
}

STATIC void
seq_streams_cdevsw_hdr(struct seq_file *m)
{
	seq_printf(m, "dp");
	seq_printf(m, ", d_name");
	seq_printf(m, ", d_str { ");
	seq_streams_streamtab_hdr(m);
	seq_printf(m, " }, d_flag");
	seq_printf(m, ", d_modid");
	seq_printf(m, ", d_count");
	seq_printf(m, ", d_sqlvl");
	seq_printf(m, ", d_syncq");
	seq_printf(m, ", d_kmod");
	seq_printf(m, ", d_major");
	seq_printf(m, ", d_inode");
	seq_printf(m, ", d_mode");
	seq_printf(m, ", d_fop");
	seq_printf(m, ", d_apush { ");
	seq_streams_strapush_hdr(m);
	seq_printf(m, " }");
}

STATIC void
seq_streams_cdevsw(struct seq_file *m, struct cdevsw *d)
{
	seq_printf(m, "%p", d);
	seq_printf(m, ", %s", d->d_name);
	seq_printf(m, ", %p { ", d->d_str);
	seq_stream_streamtab(m, d->d_str);
	seq_printf(m, " }, %#04hx", d->d_flag);
	seq_printf(m, ", %d", d->d_modid);
	seq_printf(m, ", %hu", atomic_read(&d->d_count));
	seq_printf(m, ", %d", d->d_sqlvl);
	seq_printf(m, ", %p", d->d_syncq);
	seq_printf(m, ", %p", d->d_kmod);
	seq_printf(m, ", %d", d->d_major);
	seq_printf(m, ", %p", d->d_inode);
	seq_printf(m, ", %d", d->d_mode);
	seq_printf(m, ", %p", d->d_fop);
	seq_printf(m, ", { ");
	seq_streams_strapush(m, &d->d_apush);
	seq_printf(m, " }");
}

static int
seq_streams_cdevsw_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct cdevsw *d = list_entry(cur, struct cdevsw, d_list);
	if (d == list_first_entry(&cdevsw_list, struct cdevsw, d_list))
		 seq_streams_cdevsw_hdr(m);

	seq_streams_cdevsw(m, d);
	return (0);
}

static void *
seq_streams_cdevsw_start(struct seq_file *m, loff_t *_pos)
{
	read_lock(&cdevsw_lock);
	return seq_list_start(&cdevsw_list, *_pos);
}

static void *
seq_streams_cdevsw_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &cdevsw_list, _pos);
}

static void
seq_streams_cdevsw_stop(struct seq_file *m, void *v)
{
	read_unlock(&cdevsw_lock);
}

static const struct seq_operations streams_cdevsw_proc_seqops = {
	.start = seq_streams_cdevsw_start,
	.next = seq_streams_cdevsw_next,
	.stop = seq_streams_cdevsw_stop,
	.show = seq_streams_cdevsw_show,
};

static int
streams_cdevsw_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_cdevsw_proc_seqops);
}

static const struct file_operations streams_cdevsw_proc_fops = {
	.open = streams_cdevsw_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* ---------------------- */

STATIC void
seq_streams_streamtab_mod_hdr(struct seq_file *m)
{
	seq_printf(m, "st");
	seq_printf(m, ", st_rdinit { ");
	seq_streams_qinit_hdr(m);
	seq_printf(m, " }, st_wrinit { ");
	seq_streams_qinit_hdr(m);
	seq_printf(m, " }");
}

STATIC void
seq_streams_streamtab_mod(struct seq_file *m, struct streamtab *st)
{
	seq_printf(m, "%p", st);
	seq_printf(m, ", %p { ", st->st_rdinit);
	seq_streams_qinit(m, st->st_rdinit);
	seq_printf(m, " }, %p { ", st->st_wrinit);
	seq_streams_qinit(m, st->st_wrinit);
	seq_printf(m, " }");
}

STATIC void
seq_streams_fmodsw_hdr(struct seq_file *m)
{
	seq_printf(m, "f");
	seq_printf(m, ", f_name");
	seq_printf(m, ", f_str { ");
	seq_streams_streamtab_mod_hdr(m);
	seq_printf(m, " }, f_flag");
	seq_printf(m, ", f_modid");
	seq_printf(m, ", f_count");
	seq_printf(m, ", f_sqlvl");
	seq_printf(m, ", f_syncq");
	seq_printf(m, ", f_kmod");
}

STATIC void
seq_streams_fmodsw(struct seq_file *m, struct fmodsw *f)
{
	seq_printf(m, "%p", f);
	seq_printf(m, ", %s", f->f_name);
	seq_printf(m, ", %p { ", f->f_str);
	seq_streams_streamtab_mod(m, f->f_str);
	seq_printf(m, " }, %#04hx", f->f_flag);
	seq_printf(m, ", %d", f->f_modid);
	seq_printf(m, ", %d", atomic_read(&f->f_count));
	seq_printf(m, ", %d", f->f_sqlvl);
	seq_printf(m, ", %p", f->f_syncq);
	seq_printf(m, ", %p", f->f_kmod);
}

static int
seq_streams_fmodsw_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct fmodsw *f = list_entry(cur, struct fmodsw, f_list);
	if (f == list_first_entry(&fmodsw_list, struct fmodsw, f_list))
		 seq_streams_fmodsw_hdr(m);

	seq_streams_fmodsw(m, f);
	return (0);
}

static void *
seq_streams_fmodsw_start(struct seq_file *m, loff_t *_pos)
{
	read_lock(&fmodsw_lock);
	return seq_list_start(&fmodsw_list, *_pos);
}

static void *
seq_streams_fmodsw_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &fmodsw_list, _pos);
}

static void
seq_streams_fmodsw_stop(struct seq_file *m, void *v)
{
	read_unlock(&fmodsw_lock);
}

static const struct seq_operations streams_fmodsw_proc_seqops = {
	.start = seq_streams_fmodsw_start,
	.next = seq_streams_fmodsw_next,
	.stop = seq_streams_fmodsw_stop,
	.show = seq_streams_fmodsw_show,
};

static int
streams_fmodsw_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_fmodsw_proc_seqops);
}

static const struct file_operations streams_fmodsw_proc_fops = {
	.open = streams_fmodsw_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* --------------------------- */

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

static int
seq_streams_strinfo_show(struct seq_file *m, void *v)
{
	int j;

	for (j = 0; j < DYN_SIZE; j++)
		seq_printf(m, "%14s %012d %012d\n", dyn_name[j], atomic_read(&Strinfo[j].si_cnt),
			   Strinfo[j].si_hwl);
	return (0);
}

static int
streams_strinfo_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, seq_streams_strinfo_show, NULL);
}

static const struct file_operations streams_strinfo_proc_fops = {
	.open = streams_strinfo_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* -------------------------- */

#if defined _DEBUG

STATIC void
seq_streams_stdata_hdr(struct seq_file *m)
{
	seq_printf(m, "sd");
	seq_printf(m, ", sd_rq");
	seq_printf(m, ", sd_wq");
	seq_printf(m, ", sd_iocblk");
	seq_printf(m, ", sd_other");
//      seq_printf(m, ", sd_strtab");
//      seq_printf(m, ", sd_inode");
	seq_printf(m, ", sd_flag");
	seq_printf(m, ", sd_rdopt");
	seq_printf(m, ", sd_wropt");
	seq_printf(m, ", sd_eropt");
	seq_printf(m, ", sd_iocid");
//      seq_printf(m, ", sd_iocwait");
	seq_printf(m, ", sd_session");
	seq_printf(m, ", sd_pgrp");
	seq_printf(m, ", sd_wroff");
	seq_printf(m, ", sd_wrpad");
	seq_printf(m, ", sd_rerror");
	seq_printf(m, ", sd_werror");
	seq_printf(m, ", sd_opens");
	seq_printf(m, ", sd_readers");
	seq_printf(m, ", sd_writers");
//      seq_printf(m, ", sd_rwaiters");
//      seq_printf(m, ", sd_wwaiters");
	seq_printf(m, ", sd_pushcnt");
	seq_printf(m, ", sd_nanchor");
	seq_printf(m, ", sd_sigflags");
	seq_printf(m, ", sd_siglist");
	seq_printf(m, ", sd_fasync");
//      seq_printf(m, ", sd_waitq");
//      seq_printf(m, ", sd_mark");
	seq_printf(m, ", sd_closetime");
	seq_printf(m, ", sd_ioctime");
//      seq_printf(m, ", sd_rtime");
//      seq_printf(m, ", sd_qlock");
//      seq_printf(m, ", sd_owner");
//      seq_printf(m, ", sd_nest");
//      seq_printf(m, ", sd_mutex");
	seq_printf(m, ", sd_links");
	seq_printf(m, ", sd_link_next");
	seq_printf(m, ", sd_linkblk");
}

STATIC void
seq_streams_stdata(struct seq_file *m, struct stdata *sd)
{
	seq_printf(m, "%p", sd);
	seq_printf(m, ", %p", sd->sd_rq);
	seq_printf(m, ", %p", sd->sd_wq);
	seq_printf(m, ", %p", sd->sd_iocblk);
	seq_printf(m, ", %p", sd->sd_other);
//      seq_printf(m, ", %p", sd->sd_strtab);
//      seq_printf(m, ", %p", sd->sd_inode);
	seq_printf(m, ", %#08lx", sd->sd_flag);
	seq_printf(m, ", %#08x", sd->sd_rdopt);
	seq_printf(m, ", %#08x", sd->sd_wropt);
	seq_printf(m, ", %#08x", sd->sd_eropt);
	seq_printf(m, ", %u", sd->sd_iocid);
//      seq_printf(m, ", %hu", sd->sd_iocwait);
#if defined HAVE_KTYPE_STRUCT_PID
	seq_printf(m, ", %p", sd->sd_session);
	seq_printf(m, ", %p", sd->sd_pgrp);
#else
	seq_printf(m, ", %d", sd->sd_session);
	seq_printf(m, ", %d", sd->sd_pgrp);
#endif
	seq_printf(m, ", %hu", sd->sd_wroff);
	seq_printf(m, ", %hu", sd->sd_wrpad);
	seq_printf(m, ", %d", sd->sd_rerror);
	seq_printf(m, ", %d", sd->sd_werror);
	seq_printf(m, ", %d", sd->sd_opens);
	seq_printf(m, ", %d", sd->sd_readers);
	seq_printf(m, ", %d", sd->sd_writers);
//      seq_printf(m, ", %d", sd->sd_rwaiters);
//      seq_printf(m, ", %d", sd->sd_wwaiters);
	seq_printf(m, ", %d", sd->sd_pushcnt);
	seq_printf(m, ", %d", sd->sd_nanchor);
	seq_printf(m, ", %#08lx", sd->sd_sigflags);
	seq_printf(m, ", %p", sd->sd_siglist);
	seq_printf(m, ", %p", sd->sd_fasync);
//      seq_printf(m, ", %p", sd->sd_waitq);
//      seq_printf(m, ", %p", sd->sd_mark);
	seq_printf(m, ", %lu", sd->sd_closetime);
	seq_printf(m, ", %lu", sd->sd_ioctime);
//      seq_printf(m, ", %lu", sd->sd_rtime);
//      seq_printf(m, ", %p", sd->sd_qlock);
//      seq_printf(m, ", %p", sd->sd_owner);
//      seq_printf(m, ", %u", sd->sd_nest);
//      seq_printf(m, ", %p", sd->sd_mutex);
	seq_printf(m, ", %p", sd->sd_links);
	seq_printf(m, ", %p", sd->sd_link_next);
	seq_printf(m, ", %p", sd->sd_linkblk);
}

STATIC void
seq_streams_shinfo_data_hdr(struct seq_file *m)
{
	seq_printf(m, "sh");
	seq_printf(m, ", sh_stdata { ");
	seq_streams_stdata_hdr(m);
	seq_printf(m, " }");
}

STATIC void
seq_streams_shinfo_data(struct seq_file *m, struct shinfo *sh)
{
	seq_printf(m, "sh: %p", sh);
	seq_printf(m, ", sh_stdata: { ");
	seq_streams_stdata(m, &sh->sh_stdata);
	seq_printf(m, " }");
}

static int
seq_streams_shinfo_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct strinfo *sh = list_entry(cur, struct shinfo, sh_list);
	if (sh == list_first_entry(&Strinfo[DYN_STREAM].si_head, struct shinfo, sh_list))
		 seq_streams_shinfo_hdr(m);

	seq_streams_shinfo_data(m, sh);
	return (0);
}

static void *
seq_streams_shinfo_start(struct seq_file *m, loff_t *_pos)
{
	return seq_list_start(&Strinfo[DYN_STREAM].si_head, *_pos);
}

static void *
seq_streams_shinfo_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &Strinfo[DYN_STREAM].si_head, _pos);
}

static void
seq_streams_shinfo_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_shinfo_proc_seqops = {
	.start = seq_streams_shinfo_start,
	.next = seq_streams_shinfo_next,
	.stop = seq_streams_shinfo_stop,
	.show = seq_streams_shinfo_show,
};

static int
streams_shinfo_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_shinfo_proc_seqops);
}

static const struct file_operations streams_shinfo_proc_fops = {
	.open = streams_shinfo_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* ----------- */

STATIC void
seq_streams_queue_hdr(struct seq_file *m)
{
	seq_printf(m, "q");
	seq_printf(m, ", q_qinfo");
	seq_printf(m, ", q_first");
	seq_printf(m, ", q_last");
	seq_printf(m, ", q_next");
	seq_printf(m, ", q_link");
	seq_printf(m, ", q_ptr");
	seq_printf(m, ", q_count");
	seq_printf(m, ", q_flag");
	seq_printf(m, ", q_minpsz");
	seq_printf(m, ", q_maxpsz");
	seq_printf(m, ", q_hiwat");
	seq_printf(m, ", q_lowat");
	seq_printf(m, ", q_bandp");
	seq_printf(m, ", q_nband");
	seq_printf(m, ", q_msgs");
//      seq_printf(m, ", q_rwlock");
//      seq_printf(m, ", q_iflags");
}

STATIC void
seq_streams_queue(struct seq_file *m, queue_t *q)
{
	seq_printf(m, "%p", q);
	seq_printf(m, ", %p", q->q_qinfo);
	seq_printf(m, ", %p", q->q_first);
	seq_printf(m, ", %p", q->q_last);
	seq_printf(m, ", %p", q->q_next);
	seq_printf(m, ", %p", q->q_link);
	seq_printf(m, ", %p", q->q_ptr);
	seq_printf(m, ", %lu", (ulong) q->q_count);
	seq_printf(m, ", %#08lx", q->q_flag);
	seq_printf(m, ", %ld", (long) q->q_minpsz);
	seq_printf(m, ", %ld", (long) q->q_maxpsz);
	seq_printf(m, ", %lu", (ulong) q->q_hiwat);
	seq_printf(m, ", %lu", (ulong) q->q_lowat);
	seq_printf(m, ", %p", q->q_bandp);
	seq_printf(m, ", %hu", (ushort) q->q_nband);
	seq_printf(m, ", %ld", (long) q->q_msgs);
//      seq_printf(m, "%d", q->q_rwlock);
//      seq_printf(m, ", %lu", q->q_iflags);
}

STATIC void
seq_streams_queue_data_hdr(struct seq_file *m)
{
	seq_printf(m, "qu");
	seq_printf(m, ", rq { ");
	seq_streams_queue_hdr(m);
	seq_printf(m, " }, wq { ");
	seq_streams_queue_hdr(m);
	seq_printf(m, " }, qu_str");
	seq_printf(m, ", qu_refs");
	seq_printf(m, ", qu_qwait");
//      seq_printf(m, ", qu_owner");
//      seq_printf(m, ", qu_nest");
}

STATIC void
seq_streams_queue_data(struct seq_file *m, struct queinfo *qu)
{
	seq_printf(m, "%p", qu);
	seq_printf(m, ", { ");
	seq_streams_queue(m, &qu->rq);
	seq_printf(m, " }, { ");
	seq_streams_queue(m, &qu->wq);
	seq_printf(m, " }, %p", qu->qu_str);
	seq_printf(m, ", %d", atomic_read(&qu->qu_refs));
	seq_printf(m, ", %d", waitqueue_active(&qu->qu_qwait));
//      seq_printf(m, ", %d", qu->qu_owner ? qu->qu_owner->pid : 0);
//      seq_printf(m, ", %u", qu->qu_nest);
}

static int
seq_streams_queue_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct queinfo *qu = list_entry(cur, struct queinfo, qu_list);
	if (qu == list_first_entry(&Strinfo[DYN_QUEUE].si_head, struct qeuinfo, qu_list))
		 seq_streams_queue_data_hdr(m);

	seq_streams_queue_data(m, qu);
	return (0);
}

static void *
seq_streams_queue_start(struct seq_file *m, loff_t *_pos)
{
	return seq_list_start(&Strinfo[DYN_QUEUE].si_head, *_pos);
}

static void *
seq_streams_queue_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &Strinfo[DYN_QUEUE].si_head, _pos);
}

static void
seq_streams_queue_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_queue_proc_seqops = {
	.start = seq_streams_queue_start,
	.next = seq_streams_queue_next,
	.stop = seq_streams_queue_stop,
	.show = seq_streams_queue_show,
};

static int
streams_queue_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_queue_proc_seqops);
}

static const struct file_operations streams_queue_proc_fops = {
	.open = streams_queue_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* ----------- */

STATIC void
seq_streams_msgb_hdr(struct seq_file *m)
{
	seq_printf(m, "b");
	seq_printf(m, ", b_next");
	seq_printf(m, ", b_prev");
	seq_printf(m, ", b_cont");
	seq_printf(m, ", b_rptr");
	seq_printf(m, ", b_wptr");
	seq_printf(m, ", b_datap");
	seq_printf(m, ", b_band");
	seq_printf(m, ", b_flag");
}

STATIC void
seq_streams_msgb(struct seq_file *m, struct msgb *b)
{
	seq_printf(m, "%p", b);
	seq_printf(m, ", %p", b->b_next);
	seq_printf(m, ", %p", b->b_prev);
	seq_printf(m, ", %p", b->b_cont);
	seq_printf(m, ", %p", b->b_rptr);
	seq_printf(m, ", %p", b->b_wptr);
	seq_printf(m, ", %p", b->b_datap);
	seq_printf(m, ", %hu", (ushort) b->b_band);
	seq_printf(m, ", %hu", b->b_flag);
}

STATIC void
seq_streams_msgb_data_hdr(struct seq_file *m)
{
	seq_printf(m, "m");
	seq_printf(m, ", m_mblock { ");
	seq_streams_msgb_hdr(m);
	seq_printf(m, " }, m_func");
	seq_printf(m, ", m_queue");
}

STATIC void
seq_streams_msgb_data(struct seq_file *m, struct mbinfo *mb)
{
	seq_printf(m, "m: %p", mb);
	seq_printf(m, ", m_mblock: { ");
	seq_streams_msgb(m, &mb->m_mblock);
	seq_printf(m, " }, m_func: %p", mb->m_func);
	seq_printf(m, ", m_queue: %p", mb->m_queue);
}

static int
seq_streams_msgb_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct mbinfo *mb = list_entry(cur, struct mbinfo, m_list);
	if (mb == list_first_entry(&Strinfo[DYN_MSGBLOCK].si_head, struct mbinfo, m_list))
		 seq_streams_mbinfo_data_hdr(m);

	seq_streams_mbinfo_data(m, mb);
	return (0);
}

static void *
seq_streams_msgb_start(struct seq_file *m, loff_t *_pos)
{
	return seq_list_start(&Strinfo[DYN_MSGBLOCK].si_head, *_pos);
}

static void *
seq_streams_msgb_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &Strinfo[DYN_MSGBLOCK].si_head, _pos);
}

static void
seq_streams_msgb_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_msgb_proc_seqops = {
	.start = seq_streams_msgb_start,
	.next = seq_streams_msgb_next,
	.stop = seq_streams_msgb_stop,
	.show = seq_streams_msgb_show,
};

static int
streams_msgb_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_msgb_proc_seqops);
}

static const struct file_operations streams_msgb_proc_fops = {
	.open = streams_msgb_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* ----------- */

STATIC void
seq_streams_datab_hdr(struct seq_file *m)
{
	seq_printf(m, "db");
	seq_printf(m, ", db_frtnp");
	seq_printf(m, ", db_base");
	seq_printf(m, ", db_lim");
	seq_printf(m, ", db_ref");
	seq_printf(m, ", db_type");
	seq_printf(m, ", db_class");
	seq_printf(m, ", db_size");
	seq_printf(m, ", db_users");
}

STATIC void
seq_streams_datab(struct seq_file *m, struct datab *db)
{
	seq_printf(m, "db: %p", db);
	seq_printf(m, ", db_frtnp: %p", db->db_frtnp);
	seq_printf(m, ", db_base: %p", db->db_base);
	seq_printf(m, ", db_lim: %p", db->db_lim);
	seq_printf(m, ", db_ref: %hu", (ushort) db->db_ref);
	seq_printf(m, ", db_type: %hu", (ushort) db->db_type);
	seq_printf(m, ", db_class: %hu", (ushort) db->db_class);
	seq_printf(m, ", db_size: %d:", db->db_size);
//      seq_printf(m, ", db_users: %d", atomic_read(&db->db_users));
}

STATIC void
seq_streams_datab_info_hdr(struct seq_file *m)
{
	seq_printf(m, "db");
	seq_printf(m, ", d_dblock { ");
	seq_streams_datab_hdr(m);
	seq_printf(m, " }");
}

STATIC void
seq_streams_datab_info(struct seq_file *m, struct dbinfo *db)
{
	seq_printf(m, "%p", db);
	seq_printf(m, ", { ");
	seq_streams_datab(m, &db->d_dblock);
	seq_printf(m, " }");
}

static int
seq_streams_datab_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct dbinfo *db = list_entry(cur, struct dbinfo, db_list);
	if (db == list_first_entry(&Strinfo[DYN_MDBBLOCK].si_head, struct dbinfo, db_list))
		 seq_stream_dbinfo_data_hdr(m);

	seq_stream_dbinfo_data(m, db);
	return (0);
}

static void *
seq_streams_datab_start(struct seq_file *m, loff_t *_pos)
{
	return seq_list_start(&Strinfo[DYN_MDBBLOCK].si_head, *_pos);
}

static void *
seq_streams_datab_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &Strinfo[DYN_MDBBLOCK].si_head, _pos);
}

static void
seq_streams_datab_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_datab_proc_seqops = {
	.start = seq_streams_datab_start,
	.next = seq_streams_datab_next,
	.stop = seq_streams_datab_stop,
	.show = seq_streams_datab_show,
};

static int
streams_datab_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_datab_proc_seqops);
}

static const struct file_operations streams_datab_proc_fops = {
	.open = streams_datab_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* ----------- */

STATIC void
seq_streams_linkblk_hdr(struct seq_file *m)
{
	seq_printf(m, "l");
	seq_printf(m, ", l_qtop");
	seq_printf(m, ", l_qbot");
	seq_printf(m, ", l_index");
}

STATIC void
seq_stream_linkblk(struct seq_file *m, struct linkblk *l)
{
	seq_printf(m, "%p", l);
	seq_printf(m, ", %p", l->l_qtop);
	seq_printf(m, ", %p", l->l_qbot);
	seq_printf(m, ", %d", l->l_index);
}

STATIC void
seq_streams_linkinfo_data_hdr(struct seq_file *m)
{
	seq_printf(m, "li");
	seq_printf(m, ", li_linkblk { ");
	seq_streams_linkblk_hdr(m);
	seq_printf(m, " }");
}

STATIC void
seq_streams_linkinfo_data(struct seq_file *m, struct linkinfo *li)
{
	seq_printf(m, "%p", li);
	seq_printf(m, ", { ");
	seq_streams_linkblk(m, &li->li_linkblk);
	seq_printf(m, " }");
}

static int
seq_streams_linkblk_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct linkinfo *li = list_entry(cur, struct linkinfo, li_list);
	if (li == list_first_entry(&Strinfo[DYN_LINKBLK].si_head, struct linkinfo, li_list))
		 seq_streams_linkinfo_data_hdr(m);

	seq_stream_linkinfo_data(m, li);
	return (0);
}

static void *
seq_streams_linkblk_start(struct seq_file *m, loff_t *_pos)
{
	return seq_list_start(&Strinfo[DYN_LINKBLK].si_head, *_pos);
}

static void *
seq_streams_linkblk_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &Strinfo[DYN_LINKBLK].si_head, _pos);
}

static void
seq_streams_linkblk_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_linkblk_proc_seqops = {
	.start = seq_streams_linkblk_start,
	.next = seq_streams_linkblk_next,
	.stop = seq_streams_linkblk_stop,
	.show = seq_streams_linkblk_show,
};

static int
streams_linkblk_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_linkblk_proc_seqops);
}

static const struct file_operations streams_linkblk_proc_fops = {
	.open = streams_linkblk_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* ----------- */

STATIC void
seq_streams_strevent_hdr(struct seq_file *m)
{
	seq_printf(m, "se");
	seq_printf(m, ", { SE_STREAM x.e.procp");
	seq_printf(m, ", x.e.events ");
	seq_printf(m, "| SE_BUFCALL x.b.queue");
	seq_printf(m, ", x.b.func");
	seq_printf(m, ", x.b.arg");
	seq_printf(m, ", x.b.size ");
	seq_printf(m, "| SE_TIMEOUT x.t.queue");
	seq_printf(m, ", x.t.func");
	seq_printf(m, ", x.t.arg");
	seq_printf(m, ", x.t.cpu");
	seq_printf(m, ", x.t.timer.expires }");
	seq_printf(m, ", se_id");
}

STATIC void
seq_streams_strevent(struct seq_file *m, int type, struct strevent *se)
{
	seq_printf(m, "%p", se);
	switch (type) {
	case SE_STREAM:
		seq_printf(m, ", { SE_STREAM %p", se->x.e.procp);
		seq_printf(m, ", %ld }", se->x.e.events);
	case SE_BUFCALL:
		seq_printf(m, ", { SE_BUFCALL %p", se->x.b.queue);
		seq_printf(m, ", %p", se->x.b.func);
		seq_printf(m, ", %ld", se->x.b.arg);
		seq_printf(m, ", %lu }", (ulong) se->x.b.size);
	case SE_TIMEOUT:
		seq_printf(m, ", { SE_TIMEOUT %p", se->x.t.queue);
		seq_printf(m, ", %p", se->x.t.func);
		seq_printf(m, ", %p", se->x.t.arg);
		seq_printf(m, ", %d", se->x.t.cpu);
		seq_printf(m, ", %lu }", se->x.t.timer.expires);
	}
	seq_printf(m, ", %ld", se->se_id);
}

STATIC void
seq_streams_seinfo_data_hdr(struct seq_file *m)
{
	seq_printf(m, "s");
	seq_printf(m, ", s_strevent { ");
	seq_streams_strevent_hdr(m);
	seq_printf(m, " }, s_type");
	seq_printf(m, ", s_queue");
}

STATIC void
seq_streams_seinfo_data(struct seq_file *m, struct seinfo *s)
{
	seq_printf(m, "%p", s);
	seq_printf(m, ", { ");
	seq_streams_strevent(m, s->s_type, &s->s_strevent);
	seq_printf(m, " }, %d", s->s_type);
	seq_printf(m, ", %p", s->s_queue);
}

static int
seq_streams_strevent_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct seinfo *s = list_entry(cur, struct seinfo, s_list);
	if (s == list_first_entry(&Strinfo[DYN_STREVENT].si_head, struct seinfo, s_list))
		 seq_streams_seinfo_data_hdr(m);

	seq_streams_seinfo_data(m, s);
	return (0);
}

static void *
seq_streams_strevent_start(struct seq_file *m, loff_t *_pos)
{
	return seq_list_start(&Strinfo[DYN_STREVENT].si_head, *_pos);
}

static void *
seq_streams_strevent_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &Strinfo[DYN_STREVENT].si_head, _pos);
}

static void
seq_streams_strevent_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_strevent_proc_seqops = {
	.start = seq_streams_strevent_start,
	.next = seq_streams_strevent_next,
	.stop = seq_streams_strevent_stop,
	.show = seq_streams_strevent_show,
};

static int
streams_strevent_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_strevent_proc_seqops);
}

static const struct file_operations streams_strevent_proc_fops = {
	.open = streams_strevent_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* ----------- */

STATIC void
seq_streams_qband_hdr(struct seq_file *m)
{
	seq_printf(m, "qb");
	seq_printf(m, ", qb_next");
	seq_printf(m, ", qb_count");
	seq_printf(m, ", qb_first");
	seq_printf(m, ", qb_last");
	seq_printf(m, ", qb_hiwat");
	seq_printf(m, ", qb_lowat");
	seq_printf(m, ", qb_msgs");
//      seq_printf(m, ", qb_prev");
//      seq_printf(m, ", qb_band");
}

STATIC void
seq_streams_qband(struct seq_file *m, struct qband *qb)
{
	seq_printf(m, "%p", qb);
	seq_printf(m, ", %p", qb->qb_next);
	seq_printf(m, ", %lu", (ulong) qb->qb_count);
	seq_printf(m, ", %p", qb->qb_first);
	seq_printf(m, ", %p", qb->qb_last);
	seq_printf(m, ", %ld", (long) qb->qb_hiwat);
	seq_printf(m, ", %ld", (long) qb->qb_lowat);
	seq_printf(m, ", %ld", qb->qb_msgs);
//      seq_printf(m, ", %p", qb->qb_prev);
//      seq_printf(m, ", %hu", (ushort) qb->qb_band);
}

STATIC void
seq_streams_qbinfo_data_hdr(struct seq_file *m)
{
	seq_printf(m, "qbi");
	seq_printf(m, ", qbi_qband { ");
	seq_streams_qband_hdr(m);
	seq_printf(m, " }, qbi_refs");
}

STATIC void
seq_streams_qbinfo_data(struct seq_file *m, struct qbinfo *qbi)
{
	seq_printf(m, "%p", qbi);
	seq_printf(m, ", { ");
	seq_streams_qband(m, &qbi->qbi_qband);
	seq_printf(m, " }, %d", atomic_read(&qbi->qbi_refs));
}

static int
seq_streams_qband_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct qbinfo *qbi = list_entry(cur, struct qbinfo, qbi_list);
	if (qbi == list_first_entry(&Strinfo[DYN_QBAND].si_head, struct qbinfo, qbi_list))
		 seq_streams_sbinfo_data_hdr(m);

	seq_streams_sbinfo_data(m, qbi);
	return (0);
}

static void *
seq_streams_qband_start(struct seq_file *m, loff_t *_pos)
{
	return seq_list_start(&Strinfo[DYN_QBAND].si_head, *_pos);
}

static void *
seq_streams_qband_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &Strinfo[DYN_QBAND].si_head, _pos);
}

static void
seq_streams_qband_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_qband_proc_seqops = {
	.start = seq_streams_qband_start,
	.next = seq_streams_qband_next,
	.stop = seq_streams_qband_stop,
	.show = seq_streams_qband_show,
};

static int
streams_qband_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_qband_proc_seqops);
}

static const struct file_operations streams_qband_proc_fops = {
	.open = streams_qband_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* ----------- */

STATIC void
seq_streams_apush_hdr(struct seq_file *m)
{
	seq_printf(m, "sap");
	seq_printf(m, ", sap_cmd");
	seq_printf(m, ", sap_major");
	seq_printf(m, ", sap_minor");
	seq_printf(m, ", sap_lastminor");
	seq_printf(m, ", sap_npush");
	seq_printf(m, ", sap_list { ");
	seq_printf(m, " }");
}

STATIC void
seq_streams_apush(struct seq_file *m, struct strapush *sap)
{
	int i;

	seq_printf(m, "%p", sap);
	seq_printf(m, ", %d", sap->sap_cmd);
	seq_printf(m, ", %ld", sap->sap_major);
	seq_printf(m, ", %ld", sap->sap_minor);
	seq_printf(m, ", %ld", sap->sap_lastminor);
	seq_printf(m, ", %u", sap->sap_npush);
	seq_printf(m, ", { ");
	for (i = 0; i < sap->sap_npush; i++)
		seq_printf(m, " %s", sap->sap_list[i]);
	seq_printf(m, " }");
}

STATIC void
seq_streams_apinfo_data_hdr(struct seq_file *m)
{
	seq_printf(m, "api");
	seq_printf(m, ", api_sap { ");
	seq_streams_apush_hdr(m);
	seq_printf(m, " }");
}

STATIC void
seq_streams_apinfo_data(struct seq_file *m, struct apinfo *api)
{
	seq_printf(m, "%p", api);
	seq_printf(m, ", { ");
	seq_streams_apush(m, &api->api_sap);
	seq_printf(m, " }");
}

static int
seq_streams_strapush_show(struct seq_file *m, void *v)
{
	struct list_head *cur = v;
	struct apinfo *api = list_entry(cur, struct apinfo, api_list);
	if (api == list_first_entry(&Strinfo[DYN_STRAPUSH].si_head, struct apinfo, api_list))
		 seq_streams_apinfo_data_hdr(m);

	seq_streams_apinfo_data(m, api);
	return (0);
}

static void *
seq_streams_strapush_start(struct seq_file *m, loff_t *_pos)
{
	return seq_list_start(&Strinfo[DYN_STRAPUSH].si_head, *_pos);
}

static void *
seq_streams_strapush_next(struct seq_file *m, void *v, loff_t *_pos)
{
	return seq_list_next(v, &Strinfo[DYN_STRAPUSH].si_head, _pos);
}

static void
seq_streams_strapush_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations streams_strapush_proc_seqops = {
	.start = seq_streams_strapush_start,
	.next = seq_streams_strapush_next,
	.stop = seq_streams_strapush_stop,
	.show = seq_streams_strapush_show,
};

static int
streams_strapush_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &streams_strapush_proc_seqops);
}

static const struct file_operations streams_strapush_proc_fops = {
	.open = streams_strapush_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

#endif				/* defined _DEBUG */

#else				/* defined HAVE_KFUNC_PROC_CREATE_DATA */

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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_drivers_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_drivers_list(char *page, char **start, off_t offset, int length)
#endif
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_modules_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_modules_list(char *page, char **start, off_t offset, int length)
#endif
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_cdevsw_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_cdevsw_list(char *page, char **start, off_t offset, int length)
#endif
{
	int len = 0;
	static const int maxlen = 512;
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_fmodsw_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_fmodsw_list(char *page, char **start, off_t offset, int length)
#endif
{
	int len = 0;
	static const int maxlen = 512;
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

	/* we don't need to lock anything here, the counts are allowed to change as we are listing them */
	for (j = 0; j < DYN_SIZE; j++)
		len +=
		    snprintf(page + len, maxlen - len, "%14s %012d %012d\n", dyn_name[j],
			     atomic_read(&Strinfo[j].si_cnt), Strinfo[j].si_hwl);
	return len;
}

#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_strinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_strinfo_list(char *page, char **start, off_t offset, int length)
#endif
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

#if defined _DEBUG
STATIC int
get_streams_stdata_hdr(char *page, int maxlen)
{
	int len = 0;

	len += snprintf(page + len, maxlen - len, "sd");
	len += snprintf(page + len, maxlen - len, ", sd_rq");
	len += snprintf(page + len, maxlen - len, ", sd_wq");
	len += snprintf(page + len, maxlen - len, ", sd_iocblk");
	len += snprintf(page + len, maxlen - len, ", sd_other");
//      len += snprintf(page + len, maxlen - len, ", sd_strtab");
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
	len += snprintf(page + len, maxlen - len, ", sd_ioctime");
//      len += snprintf(page + len, maxlen - len, ", sd_rtime");
//      len += snprintf(page + len, maxlen - len, ", sd_qlock");
//      len += snprintf(page + len, maxlen - len, ", sd_owner");
//      len += snprintf(page + len, maxlen - len, ", sd_nest");
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
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_strtab);
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_inode);
	len += snprintf(page + len, maxlen - len, ", %#08lx", sd->sd_flag);
	len += snprintf(page + len, maxlen - len, ", %#08x", sd->sd_rdopt);
	len += snprintf(page + len, maxlen - len, ", %#08x", sd->sd_wropt);
	len += snprintf(page + len, maxlen - len, ", %#08x", sd->sd_eropt);
	len += snprintf(page + len, maxlen - len, ", %u", sd->sd_iocid);
//      len += snprintf(page + len, maxlen - len, ", %hu", sd->sd_iocwait);
#if defined HAVE_KTYPE_STRUCT_PID
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_session);
	len += snprintf(page + len, maxlen - len, ", %p", sd->sd_pgrp);
#else
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_session);
	len += snprintf(page + len, maxlen - len, ", %d", sd->sd_pgrp);
#endif
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
	len += snprintf(page + len, maxlen - len, ", %lu", sd->sd_ioctime);
//      len += snprintf(page + len, maxlen - len, ", %lu", sd->sd_rtime);
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_qlock);
//      len += snprintf(page + len, maxlen - len, ", %p", sd->sd_owner);
//      len += snprintf(page + len, maxlen - len, ", %u", sd->sd_nest);
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_shinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_shinfo_list(char *page, char **start, off_t offset, int length)
#endif
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_queinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_queinfo_list(char *page, char **start, off_t offset, int length)
#endif
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_mbinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_mbinfo_list(char *page, char **start, off_t offset, int length)
#endif
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_dbinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_dbinfo_list(char *page, char **start, off_t offset, int length)
#endif
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_linkinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_linkinfo_list(char *page, char **start, off_t offset, int length)
#endif
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
	len += snprintf(page + len, maxlen - len, ", %ld", se->se_id);
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_seinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_seinfo_list(char *page, char **start, off_t offset, int length)
#endif
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_qbinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_qbinfo_list(char *page, char **start, off_t offset, int length)
#endif
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
#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
STATIC int
get_streams_apinfo_list(char *page, char **start, off_t offset, int length, int *eof, void *data)
#else
STATIC int
get_streams_apinfo_list(char *page, char **start, off_t offset, int length)
#endif
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

#endif				/* defined HAVE_KFUNC_PROC_CREATE_DATA */

struct proc_dir_entry *proc_str = NULL;

#endif				/* CONFIG_PROC_FS */

#ifndef HAVE_KFUNC_CREATE_PROC_INFO_ENTRY
#ifdef HAVE_KFUNC_CREATE_PROC_READ_ENTRY
#undef create_proc_info_entry
#define create_proc_info_entry(name, mode, base, info) create_proc_read_entry(name, mode, base, info, NULL)
#endif
#endif				/* HAVE_KFUNC_CREATE_PROC_INFO_ENTRY */

BIG_STATIC int
strprocfs_init(void)
{
#ifdef CONFIG_PROC_FS
	proc_str = proc_mkdir("streams", NULL);
	if (!proc_str)
		return (-ENOMEM);
#if defined HAVE_KFUNC_PROC_CREATE_DATA
#if 0
	proc_create_data("drivers", 0444, proc_str, &streams_drivers_proc_fops, NULL);
	proc_create_data("modules", 0444, proc_str, &streams_modules_proc_fops, NULL);
#else
	proc_create_data("cdevsw", 0444, proc_str, &streams_cdevsw_proc_fops, NULL);
	proc_create_data("fmodsw", 0444, proc_str, &streams_fmodsw_proc_fops, NULL);
#endif
	proc_create_data("strinfo", 0444, proc_str, &streams_strinfo_proc_fops, NULL);
#if defined _DEBUG
#if 0
	proc_create_data("cdevsw", 0444, proc_str, &streams_cdevsw_proc_fops, NULL);
	proc_create_data("fmodsw", 0444, proc_str, &streams_fmodsw_proc_fops, NULL);
#endif
	proc_create_data("stdata", 0444, proc_str, &streams_shinfo_proc_fops, NULL);
	proc_create_data("queue", 0444, proc_str, &streams_queue_proc_fops, NULL);
	proc_create_data("msgb", 0444, proc_str, &streams_msgb_proc_fops, NULL);
	proc_create_data("datab", 0444, proc_str, &streams_datab_proc_fops, NULL);
	proc_create_data("linkblk", 0444, proc_str, &streams_linkblk_proc_fops, NULL);
	proc_create_data("strevent", 0444, proc_str, &streams_strevent_proc_fops, NULL);
	proc_create_data("qband", 0444, proc_str, &streams_qband_proc_fops, NULL);
	proc_create_data("strapush", 0444, proc_str, &streams_strapush_proc_fops, NULL);
#endif
#else				/* defined HAVE_KFUNC_PROC_CREATE_DATA */
#if 0
	create_proc_info_entry("drivers", 0444, proc_str, get_streams_drivers_list);
	create_proc_info_entry("modules", 0444, proc_str, get_streams_modules_list);
#else
	create_proc_info_entry("cdevsw", 0444, proc_str, get_streams_cdevsw_list);
	create_proc_info_entry("fmodsw", 0444, proc_str, get_streams_fmodsw_list);
#endif
	create_proc_info_entry("strinfo", 0444, proc_str, get_streams_strinfo_list);
#if defined _DEBUG
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
#endif				/* defined HAVE_KFUNC_PROC_CREATE_DATA */
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
#if defined _DEBUG
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
