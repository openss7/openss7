/*****************************************************************************

 @(#) $RCSfile: unix.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/29 08:28:14 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2004/05/29 08:28:14 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: unix.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/29 08:28:14 $"

static char const ident[] =
    "$RCSfile: unix.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/29 08:28:14 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

#include <linux/smp_lock.h>
#include <linux/slab.h>

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include <tihdr.h>

#include "sys/config.h"
#include "strdebug.h"
//#include "strunix.h"          /* extern verification */
#include "strreg.h"

#define UNIX_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define UNIX_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define UNIX_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/29 08:28:14 $"
#define UNIX_DEVICE	"SVR 4.2 Sockets Library UNIX Support"
#define UNIX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define UNIX_LICENSE	"GPL"
#define UNIX_BANNER	UNIX_DESCRIP	"\n" \
			UNIX_COPYRIGHT	"\n" \
			UNIX_REVISION	"\n" \
			UNIX_DEVICE	"\n" \
			UNIX_CONTACT	"\n"
#define UNIX_SPLASH	UNIX_DEVICE	" - " \
			UNIX_REVISION	"\n"

#ifdef CONFIG_STREAMS_UNIX_MODULE
MODULE_AUTHOR(UNIX_CONTACT);
MODULE_DESCRIPTION(UNIX_DESCRIP);
MODULE_SUPPORTED_DEVICE(UNIX_DEVICE);
MODULE_LICENSE(UNIX_LICENSE);
#endif

#ifndef CONFIG_STREAMS_UNIX_NAME
#define CONFIG_STREAMS_UNIX_NAME "unix"
//#error "CONFIG_STREAMS_UNIX_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_UNIX_MAJOR
#define CONFIG_STREAMS_UNIX_MAJOR 0
//#error "CONFIG_STREAMS_UNIX_MAJOR must be defined."
#endif

modID_t modid = CONFIG_STREAMS_UNIX_MODID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module id number for UNIX driver (0 for allocation).");

major_t major = CONFIG_STREAMS_UNIX_MAJOR;
MODULE_PARM(major, "h");
MODULE_PARM_DESC(major, "Major device number for UNIX driver (0 for allocation).");

static int unix_do_ioctl(queue_t *q, mblk_t *mp)
{
	/* ioctl is only one at a time.  ioctl does not change any items in the private structure
	   other than ioctl items.  we are guaranteed to be non-reentrant on this procedure */
	struct unix *unix = q->q_ptr;
	struct ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	int err = 0, rval = 0;
	mblk_t *dp = mp->b_cont;
	unsigned long flags;
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	case M_IOCDATA:
	}
      nak:
	unix->iocstate = 0;
	mp->b_datap->db_type = M_IOCNAK;
	ioc->iocblk.ioc_rval = -1;
	ioc->iocblk.ioc_error = -err;
	qreply(q, mp);
	return (0);
      ack:
	unix->iocstate = 0;
	mp->b_datap->db_type = M_IOCACK;
	ioc->iocblk.ioc_rval = rval;
	ioc->iocblk.ioc_error = 0;
	qreply(q, mp);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUTP and SRVP
 *
 *  -------------------------------------------------------------------------
 */
static int unix_wsrv(queue_t *q)
{
	struct unix *unix = q->q_ptr;
	mblk_t *mp;
	int err;
	while ((mp = getq(q))) {
		switch (mp->b_datap->db_type) {
		case M_PCPROTO:
		case M_PROTO:
			switch (*(ulong *) mp->b_rptr) {
			case T_CONN_REQ:
				if (t_conn_req(q, mp) == 0)
					continue;
				break;
			case T_CONN_RES:
				if (t_conn_res(q, mp) == 0)
					continue;
				break;
			case T_DISCON_REQ:
				if (t_discon_req(q, mp) == 0)
					continue;
				break;
			case T_DATA_REQ:
				if (t_data_req(q, mp) == 0)
					continue;
				break;
			case T_EXDATA_REQ:
				if (t_exdata_req(q, mp) == 0)
					continue;
				break;
			case T_INFO_REQ:
				if (t_info_req(q, mp) == 0)
					continue;
				break;
			case T_BIND_REQ:
				if (t_bind_req(q, mp) == 0)
					continue;
				break;
			case T_UNBIND_REQ:
				if (t_unbind_req(q, mp) == 0)
					continue;
				break;
			case T_UNITDATA_REQ:
				if (t_unitdata_req(q, mp) == 0)
					continue;
				break;
			case T_OPTMGMT_REQ:
				if (t_optmgmt_req(q, mp) == 0)
					continue;
				break;
			case T_ORDREL_REQ:
				if (t_ordrel_req(q, mp) == 0)
					continue;
				break;
			default:
				freemsg(mp);
				continue;
			}
			break;
		case M_DATA:
			if (t_data(q, mp) == 0)
				continue;
			break;
		case M_READ:
			qenable(RD(q));
		default:
			freemsg(mp);
			continue;
		}
	}
	return (0);
}
static int unix_rsrv(queue_t *q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		switch (mp->b_datap->db_type) {
		case M_PCPROTO:
		case M_PROTO:
		case M_DATA:
			/* just for storage */
			putnext(q, mp);
			continue;
		default:
			freemsg(mp);
			continue;
		}
	}
	return (0);
}
static int unix_wput(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
	case M_DATA:
	case M_DELAY:
	case M_READ:
		putq(q, mp);
		return (0);
	case M_FLUSH:
		/* always do M_FLUSH in put procedure */
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
	case M_IOCDATA:
		return unix_do_ioctl(q, mp);
	}
	freemsg(mp);
	return (0);
}
static int unix_rput(queue_t *q, mblk_t *mp)
{
	putq(q, mp);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 *  We only have three clonable minors: ticlts, ticots and ticotsord.
 */
static int unix_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *credp)
{
	struct unix *unix;
	struct unix_protosw *proto;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next)
		return (EIO);	/* can't push as module */
	if (cmajor != major || cminor < 1 || cminor > 3)
		return (ENXIO);	/* no such minor */
	if ((unix = unix_cache_alloc(unix_priv_cachep, SLAB_ATOMIC))) {
		unix->cmajor = cmajor;
		unix->cminor = cminor;
		unix->rq = RD(q);
		unix->wq = WR(q);
		unix->cred = *credp;
		unix->conq = NULL;
		unix->coninds = 0;
		unix->p = &unix_protosw[cminor - 1];
		q->q_ptr = WR(q)->q_ptr = unix;
		return (0);
	}
	return (ENOMEM);
}
static int unix_close(queue_t *q, int oflag, cred_t *credp)
{
	struct unix *unix;
	if ((unix = q->q_ptr)) {
		while (unix->conq) {
			freemsg(xchg(&unix->conq, unix->conq->b_next));
			unix->coninds--;
		}
		q->q_ptr = WR(q)->q_ptr = NULL;
		kmem_cache_free(unix_priv_cachep, unix);
	}
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */
static struct qinit unix_rinit = {
	qi_putp:unix_rput,
	qi_srvp:unix_rsrv,
	qi_qopen:unix_open,
	qi_qclose:unix_close,
	qi_minfo:&unix_minfo,
};

static struct qinit unix_winit = {
	qi_putp:unix_wput,
	qi_srvp:unix_wsrv,
	qi_minfo:&unix_minfo,
};

static struct streamtab unix_info = {
	st_rdinit:&unix_rinit,
	st_wrinit:&unix_winit,
};

static struct cdevsw unix_cdev = {
	d_name:CONFIG_STREAMS_UNIX_NAME,
	d_str:&unix_info,
	d_flag:D_CLONE,
	d_fop:NULL,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

static int __init unix_init(void)
{
	int err;
#ifdef CONFIG_STREAMS_UNIX_MODULE
	printk(KERN_INFO UNIX_BANNER);
#else
	printk(KERN_INFO UNIX_SPLASH);
#endif
	unix_minfo.mi_idnum = modid;
	if ((err = register_strdev(&unix_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
}
static void __exit unix_exit(void)
{
	unregister_strdev(&unix_cdev, major);
}

#ifdef CONFIG_STREAMS_UNIX_MODULE
module_init(unix_init);
module_init(unix_exit);
#endif
