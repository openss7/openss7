/*****************************************************************************

 @(#) $RCSfile: sc.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2006/02/20 10:59:24 $

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

 Last Modified $Date: 2006/02/20 10:59:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sc.c,v $
 Revision 0.9.2.43  2006/02/20 10:59:24  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ident "@(#) $RCSfile: sc.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2006/02/20 10:59:24 $"

static char const ident[] =
    "$RCSfile: sc.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2006/02/20 10:59:24 $";

/* 
 *  This is SC, a STREAMS Configuration module for Linux Fast-STREAMS.  This
 *  is an auxilliary module to the SAD (STREAMS Administrative Driver) and can
 *  be pushed over that module or over the NULS (Null STREAM) driver.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/kmem.h>
#include <sys/cmn_err.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include <sys/sc.h>

#include "sys/config.h"
#include "src/kernel/strlookup.h"

#define SC_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SC_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SC_REVISION	"LfS $RCSfile: sc.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2006/02/20 10:59:24 $"
#define SC_DEVICE	"SVR 4.2 STREAMS STREAMS Configuration Module (SC)"
#define SC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SC_LICENSE	"GPL"
#define SC_BANNER	SC_DESCRIP	"\n" \
			SC_COPYRIGHT	"\n" \
			SC_REVISION	"\n" \
			SC_DEVICE	"\n" \
			SC_CONTACT	"\n"
#define SC_SPLASH	SC_DEVICE	" - " \
			SC_REVISION

#ifdef CONFIG_STREAMS_SC_MODULE
MODULE_AUTHOR(SC_CONTACT);
MODULE_DESCRIPTION(SC_DESCRIP);
MODULE_SUPPORTED_DEVICE(SC_DEVICE);
MODULE_LICENSE(SC_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sc");
#endif
#endif

#ifndef CONFIG_STREAMS_SC_NAME
#error CONFIG_STREAMS_SC_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_SC_MODID
#error CONFIG_STREAMS_SC_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_SC_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for SC.");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SC_MODID));
MODULE_ALIAS("streams-module-sc");
#endif
#endif

static struct module_info sc_minfo = {
	.mi_idnum = CONFIG_STREAMS_SC_MODID,
	.mi_idname = CONFIG_STREAMS_SC_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routine
 *  
 *  -------------------------------------------------------------------------
 */
static streamscall int
sc_wput(queue_t *q, mblk_t *mp)
{
	union ioctypes *ioc;
	int err = 0, rval = 0;
	mblk_t *dp = mp->b_cont;

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		/* canonical flushing */
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			queue_t *rq = RD(q);

			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(rq, mp->b_rptr[1], FLUSHALL);
			else
				flushq(rq, FLUSHALL);
			qreply(q, mp);
			return (0);
		}
		break;
	case M_IOCTL:
		trace();
		ioc = (typeof(ioc)) mp->b_rptr;
		switch (ioc->iocblk.ioc_cmd) {
		case SC_IOC_LIST:
			/* there is really no reason why a regular user cannot list modules and
			   related information. */
#if 0
			trace();
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0) {
				ptrace(("Error path taken!\n"));
				goto nak;
			}
#endif
			trace();
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				caddr_t uaddr = *(caddr_t *) dp->b_rptr;

				trace();
				if (uaddr == NULL) {
					rval = cdev_count + fmod_count;
					goto ack;
				}
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_addr = uaddr;
				ioc->copyreq.cq_size = sizeof(struct sc_list);
				ioc->copyreq.cq_flag = 0;
				ioc->copyreq.cq_private = (mblk_t *) 0;
				qreply(q, mp);
				return (0);
			}
			trace();
			err = -EINVAL;
			goto nak;
		}
		break;
	case M_IOCDATA:
		ioc = (typeof(ioc)) mp->b_rptr;
		switch (ioc->copyresp.cp_cmd) {
		case SC_IOC_LIST:
		{
			int n, count;
			ssize_t size;
			caddr_t uaddr;

			trace();
			if (ioc->copyresp.cp_rval != 0) {
				ptrace(("Aborting ioctl!\n"));
				goto abort;
			}
			trace();
			if (ioc->copyresp.cp_private == (mblk_t *) 0) {
				n = 0;
				if (!dp || dp->b_wptr == dp->b_rptr) {
					rval = cdev_count + fmod_count;
					goto ack;
				}
				{
					struct sc_list *list;

					if (dp->b_wptr < dp->b_rptr + sizeof(*list)) {
						ptrace(("Error path taken!\n"));
						err = -EFAULT;
						goto nak;
					}
					list = (typeof(list)) dp->b_rptr;
					count = list->sc_nmods;
					uaddr = (caddr_t) list->sc_mlist;
					if (count < 0) {
						ptrace(("Error path taken!\n"));
						err = -EINVAL;
						goto nak;
					}
					if (count > 100) {
						ptrace(("Error path taken!\n"));
						err = -ERANGE;
						goto nak;
					}
					if (count == 0) {
						rval = cdev_count + fmod_count;
						goto ack;
					}
					size = count * sizeof(struct sc_mlist);
					if (!(dp = allocb(size, BPRI_MED))) {
						ptrace(("Error path taken!\n"));
						err = -ENOSR;
						goto nak;
					}
					dp->b_wptr = dp->b_rptr + size;
					bzero(dp->b_rptr, size);
					freemsg(mp->b_cont);
					mp->b_cont = dp;
				}
				{
					struct sc_mlist *mlist;
					struct list_head *pos;

					mlist = (typeof(mlist)) dp->b_rptr;
					trace();
					if (n < count) {
						trace();
						/* output all devices */
						read_lock(&cdevsw_lock);
						list_for_each(pos, &cdevsw_list) {
							struct cdevsw *cdev;
							struct qinit *qinit;

							if (n >= count)
								break;
							cdev =
							    list_entry(pos, struct cdevsw, d_list);

							qinit = cdev->d_str->st_wrinit;
							mlist->major = cdev->d_major;
							strncpy(mlist->name,
									qinit->qi_minfo->mi_idname,
									FMNAMESZ + 1);
							// mlist->mi = *qinit->qi_minfo;
							mlist->mi.mi_idnum = qinit->qi_minfo->mi_idnum;
							strncpy(mlist->mi.mi_idname,
									qinit->qi_minfo->mi_idname,
									FMNAMESZ + 1);
							mlist->mi.mi_minpsz =
								qinit->qi_minfo->mi_minpsz;
							mlist->mi.mi_maxpsz =
								qinit->qi_minfo->mi_maxpsz;
							mlist->mi.mi_hiwat =
								qinit->qi_minfo->mi_hiwat;
							mlist->mi.mi_lowat =
								qinit->qi_minfo->mi_lowat;
							if (qinit->qi_mstat) {
								mlist->ms.ms_pcnt =
									qinit->qi_mstat->ms_pcnt;
								mlist->ms.ms_scnt =
									qinit->qi_mstat->ms_scnt;
								mlist->ms.ms_ocnt =
									qinit->qi_mstat->ms_ocnt;
								mlist->ms.ms_ccnt =
									qinit->qi_mstat->ms_ccnt;
								mlist->ms.ms_acnt =
									qinit->qi_mstat->ms_acnt;
								mlist->ms.ms_flags =
									qinit->qi_mstat->ms_flags;
								mlist->ms.ms_xsize =
									qinit->qi_mstat->ms_xsize;
							}
							n++;
							mlist++;
						}
						read_unlock(&cdevsw_lock);
					}
					trace();
					if (n < count) {
						trace();
						/* output all modules */
						read_lock(&fmodsw_lock);
						list_for_each(pos, &fmodsw_list) {
							struct fmodsw *fmod;
							struct qinit *qinit;

							if (n >= count)
								break;
							fmod =
							    list_entry(pos, struct fmodsw, f_list);

							qinit = fmod->f_str->st_wrinit;
							mlist->major = 0;
							strncpy(mlist->name,
									qinit->qi_minfo->mi_idname,
									FMNAMESZ + 1);
							mlist->mi.mi_idnum =
								qinit->qi_minfo->mi_idnum;
							strncpy(mlist->mi.mi_idname,
									qinit->qi_minfo->mi_idname,
									FMNAMESZ + 1);
							mlist->mi.mi_minpsz =
								qinit->qi_minfo->mi_minpsz;
							mlist->mi.mi_maxpsz =
								qinit->qi_minfo->mi_maxpsz;
							mlist->mi.mi_hiwat =
								qinit->qi_minfo->mi_hiwat;
							mlist->mi.mi_lowat =
								qinit->qi_minfo->mi_lowat;
							if (qinit->qi_mstat) {
								mlist->ms.ms_pcnt =
									qinit->qi_mstat->ms_pcnt;
								mlist->ms.ms_scnt =
									qinit->qi_mstat->ms_scnt;
								mlist->ms.ms_ocnt =
									qinit->qi_mstat->ms_ocnt;
								mlist->ms.ms_ccnt =
									qinit->qi_mstat->ms_ccnt;
								mlist->ms.ms_acnt =
									qinit->qi_mstat->ms_acnt;
								mlist->ms.ms_flags =
									qinit->qi_mstat->ms_flags;
								mlist->ms.ms_xsize =
									qinit->qi_mstat->ms_xsize;
							}
							n++;
							mlist++;
						}
						read_unlock(&fmodsw_lock);
					}
					trace();
					/* zero all excess elements */
					for (; n < count; n++, mlist++) {
						mlist->major = -1;
					}
				}
				trace();
				mp->b_datap->db_type = M_COPYOUT;
				ioc->copyreq.cq_addr = uaddr;
				ioc->copyreq.cq_size = size;
				ioc->copyreq.cq_flag = 0;
				ioc->copyreq.cq_private = (mblk_t *) (long) count;
				qreply(q, mp);
				return (0);
			} else {
				trace();
				/* done */
				rval = (int) (long) ioc->copyresp.cp_private;
				goto ack;
			}
		}
		}
	      nak:
		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_rval = -1;
		ioc->iocblk.ioc_error = -err;
		qreply(q, mp);
		return (0);
	      ack:
		mp->b_datap->db_type = M_IOCACK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_rval = rval;
		ioc->iocblk.ioc_error = 0;
		qreply(q, mp);
		return (0);
	      abort:
		ctrace(freemsg(mp));
		return (0);
	}
	putnext(q, mp);
	return (0);
}
static streamscall int
sc_rput(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static streamscall int
sc_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	if (q->q_ptr)
		return (0);	/* already open */
	if (sflag == MODOPEN && WR(q)->q_next != NULL) {
		/* don't need to be privileged to push the module, just to use it */
#if 0
		if (crp->cr_uid != 0 && crp->cr_ruid != 0)
			return (-EACCES);
#endif
		q->q_ptr = WR(q)->q_ptr = (void *) 1;
		qprocson(q);
		return (0);
	}
	return (-EIO);		/* can't be opened as driver */
}
static streamscall int
sc_close(queue_t *q, int oflag, cred_t *crp)
{
	qprocsoff(q);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */

static struct qinit sc_rqinit = {
	.qi_putp = sc_rput,
	.qi_qopen = sc_open,
	.qi_qclose = sc_close,
	.qi_minfo = &sc_minfo,
};

static struct qinit sc_wqinit = {
	.qi_putp = sc_wput,
	.qi_minfo = &sc_minfo,
};

static struct streamtab sc_info = {
	.st_rdinit = &sc_rqinit,
	.st_wrinit = &sc_wqinit,
};

static struct fmodsw sc_fmod = {
	.f_name = CONFIG_STREAMS_SC_NAME,
	.f_str = &sc_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_SC_MODULE
static
#endif
int __init
sc_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_SC_MODULE
	printk(KERN_INFO SC_BANNER);
#else
	printk(KERN_INFO SC_SPLASH);
#endif
	sc_minfo.mi_idnum = modid;
	if ((err = register_strmod(&sc_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};

#ifdef CONFIG_STREAMS_SC_MODULE
static
#endif
void __exit
sc_exit(void)
{
	int err;

	if ((err = unregister_strmod(&sc_fmod)))
		return (void) (err);
	return (void) (0);
};

#ifdef CONFIG_STREAMS_SC_MODULE
module_init(sc_init);
module_exit(sc_exit);
#endif
