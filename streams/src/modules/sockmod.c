/*****************************************************************************

 @(#) $RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/04/22 12:08:34 $

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

 Last Modified $Date: 2004/04/22 12:08:34 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/04/22 12:08:34 $"

static char const ident[] = "$RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/04/22 12:08:34 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/modversions.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include <sys/kmem.h>
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/sockmod.h>

#include <tihdr.h>

#include "strdebug.h"

#include "sys/config.h"

#define SOCKMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SOCKMOD_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define SOCKMOD_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/04/22 12:08:34 $"
#define SOCKMOD_DEVICE		"SVR 4.2 Socket Library Module for TLI Devices"
#define SOCKMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SOCKMOD_LICENSE		"GPL"
#define SOCKMOD_BANNER	SOCKMOD_DESCRIP		"\n" \
			SOCKMOD_COPYRIGHT	"\n" \
			SOCKMOD_REVISION	"\n" \
			SOCKMOD_DEVICE		"\n" \
			SOCKMOD_CONTACT		"\n"
#define SOCKMOD_SPLASH	SOCKMOD_DEVICE		" - " \
			SOCKMOD_REVISION	"\n"

MODULE_AUTHOR(SOCKMOD_CONTACT);
MODULE_DESCRIPTION(SOCKMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(SOCKMOD_DEVICE);
MODULE_LICENSE(SOCKMOD_LICENSE);

#ifndef CONFIG_STREAMS_SOCKMOD_NAME
//#define CONFIG_STREAMS_SOCKMOD_NAME "sockmod"
#error "CONFIG_STREAMS_SOCKMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_SOCKMOD_MODID
//#define CONFIG_STREAMS_SOCKMOD_MODID 14
#error "CONFIG_STREAMS_SOCKMOD_MODID must be defined."
#endif

static modID_t modid = CONFIG_STREAMS_SOCKMOD_MODID;
MODULE_PARM(modid, "b");
MODULE_PARM_DESC(modid, "Module ID for SOCKMOD.");

static struct module_info sockmod_minfo = {
	mi_idnum:CONFIG_STREAMS_SOCKMOD_MODID,
	mi_idname:CONFIG_STREAMS_SOCKMOD_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  Private Datastructure ctors and dtors
 *  
 *  -------------------------------------------------------------------------
 */
struct sockmod {
	struct list_head list;		/* list of sockmod structures */
	dev_t dev;			/* device number of driver */
	mblk_t *iocblk;			/* ioctl being processed */
	cred_t cred;			/* credentials */
};

static rwlock_t sockmod_list_lock = RW_LOCK_UNLOCKED;

static struct list_head sockmod_opens = { &sockmod_opens, &sockmod_opens };

static struct sockmod *sockmod_alloc_priv(queue_t *q, struct list_head *list, dev_t *devp,
					  cred_t *crp)
{
	struct sockmod *sockmod;
	if ((sockmod = kmem_zalloc(sizeof(*sockmod), KM_NOSLEEP))) {
		queue_t *wq = WR(q);
		sockmod->dev = *devp;
		sockmod->cred = *crp;
		write_lock(&sockmod_list_lock);
		list_add_tail(&sockmod->list, list);
		write_unlock(&sockmod_list_lock);
		/* we are a module with no service routine so our hiwat, lowat don't matter;
		   however, our minpsz, maxpsz do because we are the first queue under the stream
		   head */
		wq->q_minpsz = wq->q_next->q_minpsz;
		wq->q_maxpsz = wq->q_next->q_maxpsz;
		wq->q_ptr = q->q_ptr = sockmod;
	}
	return (sockmod);
}
static void sockmod_free_priv(queue_t *q)
{
	struct sockmod *sockmod = q->q_ptr;
	write_lock(&sockmod_list_lock);
	list_del(&sockmod->list);
	write_unlock(&sockmod_list_lock);
	kmem_free(sockmod, sizeof(*sockmod));
	return;
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routine
 *  
 *  -------------------------------------------------------------------------
 */
static int sockmod_put(queue_t *q, mblk_t *mp)
{
	struct sockmod *sockmod = q->q_ptr;
	switch (mp->b_datap->db_type) {
		union T_primitives *p;
		struct iocblk *ioc;
		mblk_t *dp;
		int err;
	case M_IOCTL:
		ioc = (typeof(ioc)) mp->b_rptr;
		dp = unlinkb(mp);
		dp->b_datap->db_type = (ioc->ioc_cmd == SI_GETUDATA) ? M_PCPROTO : M_PROTO;
		p = (typeof(p)) dp->b_rptr;
		err = -EINVAL;
		if (ioc->ioc_count == TRANSPARENT)
			goto error;
		switch (ioc->ioc_cmd) {
		case O_SI_GETUDATA:
		case SI_SHUTDOWN:
		case SI_LISTEN:
		case SI_SETMYNAME:
		case SI_SETPEERNAME:
		case SI_GETINTRANSIT:
		case SI_TCL_LINK:
		case SI_TCL_UNLINK:
		case SI_SOCKPARAMS:
		case SI_GETUDATA:
		default:
			putnext(q, mp);
			return (0);
		}
		if ((mp = xchg(&sockmod->iocblk, mp)))
			freemsg(mp);
		putnext(q, dp);
		return (0);
	      error:
		linkb(mp, dp);
		ioc->ioc_error = (-err << 8) | TSYSERR;
		mp->b_datap->db_type = M_IOCNAK;
		ioc->ioc_rval = -1;
		qreply(q, mp);
		return (0);
	case M_PCPROTO:
	case M_PROTO:
		if (!(dp = xchg(&sockmod->iocblk, NULL)))
			break;
		ioc = (typeof(ioc)) dp->b_rptr;
		linkb(dp, mp);
		p = (typeof(p)) mp->b_rptr;
		switch (p->type) {
		case T_ERROR_ACK:
		case T_OK_ACK:
		case T_INFO_ACK:
		case T_BIND_ACK:
		case T_OPTMGMT_ACK:
		case T_ADDR_ACK:
		default:
			putnext(q, mp);
			return (0);
		}
		dp->b_datap->db_type = M_IOCACK;
		ioc->ioc_count = mp->b_wptr - mp->b_rptr;
		ioc->ioc_error = 0;
		ioc->ioc_rval = 0;
		putnext(q, dp);
		return (0);
	}
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
static int sockmod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		struct sockmod *sockmod;
		struct list_head *pos;
		read_lock(&sockmod_list_lock);
		list_for_each(pos, &sockmod_opens) {
			sockmod = list_entry(pos, struct sockmod, list);
			if (sockmod->dev == *devp) {
				read_unlock(&sockmod_list_lock);
				MOD_DEC_USE_COUNT;
				return (ENXIO);	/* double push */
			}
		}
		read_unlock(&sockmod_list_lock);
		if (!sockmod_alloc_priv(q, &sockmod_opens, devp, crp)) {
			MOD_DEC_USE_COUNT;
			return (-ENOMEM);
		}
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);		/* can't be opened as driver */
}
static int sockmod_close(queue_t *q, int oflag, cred_t *crp)
{
	(void) oflag;
	(void) crp;
	if (!q->q_ptr)
		return (ENXIO);
	sockmod_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
static struct qinit sockmod_qinit = {
	qi_putp:sockmod_put,
	qi_qopen:sockmod_open,
	qi_qclose:sockmod_close,
	qi_minfo:&sockmod_minfo,
};

static struct streamtab sockmod_info = {
	st_rdinit:&sockmod_qinit,
	st_wrinit:&sockmod_qinit,
};

static struct fmodsw sockmod_fmod = {
	f_name:CONFIG_STREAMS_SOCKMOD_NAME,
	f_str:&sockmod_info,
	f_flag:0,
	f_kmod:THIS_MODULE,
};

static int __init sockmod_init(void)
{
	int err;
	printk(KERN_INFO SOCKMOD_BANNER);
	if ((err = register_strmod(modid, &sockmod_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};
static void __exit sockmod_exit(void)
{
	int err;
	if ((err = unregister_strmod(modid, &sockmod_fmod)))
		return (void) (err);
	return (void) (0);
};

module_init(sockmod_init);
module_exit(sockmod_exit);
