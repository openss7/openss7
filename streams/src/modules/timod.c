/*****************************************************************************

 @(#) timod.c,v (0.9.2.10) 2003/10/21 21:50:22

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified 2003/10/21 21:50:22 by brian

 *****************************************************************************/

#ident "@(#) timod.c,v (0.9.2.10) 2003/10/21 21:50:22"

static char const ident[] =
    "timod.c,v (0.9.2.10) 2003/10/21 21:50:22";

/* 
 *  This is TIMOD an XTI library interface module for TPI Version 2 transport
 *  service providers supporting the T_ADDR_REQ primitive.
 *
 *  This module is suitable for supporting both Ole Husgaard's XTI library as
 *  (that does not use T_GETPROTADDR) or the more complete OpenSS7 libxnet XTI
 *  library that comes with the Linux Fast-STREAMS release.  The libxnet
 *  library has superior syncrhonization across a fork() and provides a
 *  conforming t_sync() library call.
 */

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <linux/kmem.h>
#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strconf.h>
#include <linux/strsubr.h>
#include <linux/ddi.h>

#include <tihdr.h>
#include <timod.h>

#include "strdebug.h"

#define TIMOD_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TIMOD_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define TIMOD_REVISION	"LfS timod.c,v (0.9.2.10) 2003/10/21 21:50:22"
#define TIMOD_DEVICE	"SVR 4.2 STREAMS XTI Library Module for TLI Devices (TIMOD)"
#define TIMOD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TIMOD_LICENSE	"GPL"
#define TIMOD_BANNER	TIMOD_DESCRIP	"\n" \
			TIMOD_COPYRIGHT	"\n" \
			TIMOD_REVISION	"\n" \
			TIMOD_DEVICE	"\n" \
			TIMOD_CONTACT	"\n"
#define TIMOD_SPLASH	TIMOD_DEVICE	" - " \
			TIMOD_REVISION	"\n"

MODULE_AUTHOR(TIMOD_CONTACT);
MODULE_DESCRIPTION(TIMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(TIMOD_DEVICE);
MODULE_LICENSE(TIMOD_LICENSE);

#ifndef CONFIG_STREAMS_TIMOD_NAME
#define CONFIG_STREAMS_TIMOD_NAME "timod"
//#error "CONFIG_STREAMS_TIMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_TIMOD_MODID
#define CONFIG_STREAMS_TIMOD_MODID 15
//#error "CONFIG_STREAMS_TIMOD_MODID must be defined."
#endif

static modID_t modid = CONFIG_STREAMS_TIMOD_MODID;
MODULE_PARM(modid, "b");
MODULE_PARM_DESC(modid, "Module ID for TIMOD.");

static struct module_info timod_minfo = {
	mi_idnum:CONFIG_STREAMS_TIMOD_MODID,
	mi_idname:CONFIG_STREAMS_TIMOD_NAME,
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
struct timod {
	struct list_head list;		/* list of timod structures */
	dev_t dev;			/* device number of driver */
	mblk_t *iocblk;			/* ioctl being processed */
	cred_t cred;			/* credentials */
};

static rwlock_t timod_list_lock = RW_LOCK_UNLOCKED;

static struct list_head timod_opens = {
	next:&timod_opens,
	prev:&timod_opens,
};

static struct timod *timod_alloc_priv(queue_t *q, struct list_head *list, dev_t *devp, cred_t *crp)
{
	struct timod *timod;
	if ((timod = kmem_zalloc(sizeof(*timod), KM_NOSLEEP))) {
		queue_t *wq = WR(q);
		timod->dev = *devp;
		timod->cred = *crp;
		write_lock(&timod_list_lock);
		list_add_tail(&timod->list, list);
		write_unlock(&timod_list_lock);
		/* we are a module with no service routine so our hiwat, lowat don't matter;
		   however, our minpsz, maxpsz do because we are the first queue under the stream
		   head */
		wq->q_minpsz = wq->q_next->q_minpsz;
		wq->q_maxpsz = wq->q_next->q_maxpsz;
		wq->q_ptr = q->q_ptr = timod;
	}
	return (timod);
}
static void timod_free_priv(queue_t *q)
{
	struct timod *timod = q->q_ptr;
	write_lock(&timod_list_lock);
	list_del(&timod->list);
	write_unlock(&timod_list_lock);
	kmem_free(timod, sizeof(*timod));
	return;
}

#if defined TI_CONNECT || defined TI_ACCEPT || defined TI_DISCONNECT
static int split_buffer(mblk_t *mp, unsigned char *ptr)
{
	if (ptr >= mp->b_wptr) {
		mp->b_datap->db_type = M_PROTO;
		return (0);
	} else {
		mblk_t *dp;
		if ((dp = dupb(mp))) {
			mp->b_wptr = ptr;
			dp->b_rptr = ptr;
			mp->b_datap->db_type = M_PROTO;
		}
		return (-ENOSR);
	}
}
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routine
 *  
 *  -------------------------------------------------------------------------
 */
static int timod_put(queue_t *q, mblk_t *mp)
{
	struct timod *timod = q->q_ptr;
	switch (mp->b_datap->db_type) {
		union T_primitives *p;
		struct iocblk *ioc;
		mblk_t *dp;
		int err;
	case M_IOCTL:
		ioc = (typeof(ioc)) mp->b_rptr;
		dp = unlinkb(mp);
		dp->b_datap->db_type = (ioc->ioc_cmd == TI_GETINFO) ? M_PCPROTO : M_PROTO;
		p = (typeof(p)) dp->b_rptr;
		err = -EINVAL;
		if (ioc->ioc_count == TRANSPARENT)
			goto error;
		switch (ioc->ioc_cmd) {
		case TI_OPTMGMT:
			if (ioc->ioc_count >= sizeof(p->optmgmt_req) && p->type == T_OPTMGMT_REQ)
				break;
			goto error;
		case TI_BIND:
			if (ioc->ioc_count >= sizeof(p->bind_req) && p->type == T_BIND_REQ)
				break;
			goto error;
		case TI_GETINFO:
			if (ioc->ioc_count >= sizeof(p->info_req) && p->type == T_INFO_REQ)
				break;
			goto error;
		case TI_UNBIND:
			if (ioc->ioc_count >= sizeof(p->unbind_req) && p->type == T_UNBIND_REQ)
				break;
			goto error;
#ifdef TI_GETPROTADDR
		case TI_GETPROTADDR:
			if (ioc->ioc_count >= sizeof(p->addr_req) && p->type == T_ADDR_REQ)
				break;
			goto error;
#endif
#ifdef TI_CONNECT
		case TI_CONNECT:
			if (ioc->ioc_count >= sizeof(p->conn_req) && p->type == T_CONN_REQ) {
				unsigned char *dat;
				dat =
				    dp->b_rptr + (p->conn_req.OPT_length ? p->conn_req.OPT_offset +
						  p->conn_req.OPT_length : (p->conn_req.
									    DEST_length ? p->
									    conn_req.DEST_offset +
									    p->conn_req.
									    DEST_length : sizeof(p->
												 conn_req)));
				if ((err = split_buffer(dp, dat)) < 0)
					goto error;
			}
#endif
#ifdef TI_ACCEPT
		case TI_ACCEPT:
			if (ioc->ioc_count >= sizeof(p->conn_res) && p->type == T_CONN_RES) {
				unsigned char *dat;
				dat =
				    dp->b_rptr + (p->conn_res.OPT_length ? p->conn_res.OPT_offset +
						  p->conn_res.OPT_length : sizeof(p->conn_res));
				if ((err = split_buffer(dp, dat)) < 0)
					goto error;
			}
#endif
#ifdef TI_DISCONNECT
		case TI_DISCONNECT:
			if (ioc->ioc_count >= sizeof(p->discon_req) && p->type == T_DISCON_REQ) {
				unsigned char *dat;
				dat = dp->b_rptr + sizeof(p->discon_req);
				if ((err = split_buffer(dp, dat)) < 0)
					goto error;
			}
#endif
		case TI_GETMYNAME:
		case TI_GETPEERNAME:
		case TI_SETMYNAME:
		case TI_SETPEERNAME:
			err = -EOPNOTSUPP;
			goto error;
		default:
			putnext(q, mp);
			return (0);
		}
		if ((mp = xchg(&timod->iocblk, mp)))
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
		if (!(dp = xchg(&timod->iocblk, NULL)))
			break;
		ioc = (typeof(ioc)) dp->b_rptr;
		linkb(dp, mp);
		p = (typeof(p)) mp->b_rptr;
		switch (p->type) {
		case T_ERROR_ACK:
			mp->b_datap->db_type = M_IOCNAK;
			ioc->ioc_error = ((p->error_ack.UNIX_error << 8) | p->error_ack.TLI_error);
			ioc->ioc_rval = -1;
			qreply(q, dp);
			return (0);
		case T_OK_ACK:
			switch (ioc->ioc_cmd) {
			case TI_UNBIND:
#ifdef TI_ACCEPT
			case TI_ACCEPT:
#endif
#ifdef TI_CONNECT
			case TI_CONNECT:
#endif
#ifdef TI_DISCONNECT
			case TI_DISCONNECT:
#endif
				break;
			}
			goto exit;
		case T_INFO_ACK:
			if (ioc->ioc_cmd == TI_GETINFO)
				break;
			goto exit;
		case T_BIND_ACK:
			if (ioc->ioc_cmd == TI_BIND)
				break;
			goto exit;
		case T_OPTMGMT_ACK:
			if (ioc->ioc_cmd == TI_OPTMGMT)
				break;
			goto exit;
		case T_ADDR_ACK:
#ifdef TI_GETPROTADDR
			if (ioc->ioc_cmd == TI_GETPROTADDR)
				break;
#endif
			goto exit;
		      exit:
			timod->iocblk = unlinkb(dp);
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
static int timod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* alread open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		struct timod *timod;
		struct list_head *pos;
		read_lock(&timod_list_lock);
		list_for_each(pos, &timod_opens) {
			timod = list_entry(pos, struct timod, list);
			if (timod->dev == *devp) {
				read_unlock(&timod_list_lock);
				MOD_DEC_USE_COUNT;
				return (ENXIO);	/* double push */
			}
		}
		read_unlock(&timod_list_lock);
		if (!timod_alloc_priv(q, &timod_opens, devp, crp)) {
			MOD_DEC_USE_COUNT;
			return (-ENOMEM);
		}
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);		/* can't be opened as driver */
}
static int timod_close(queue_t *q, int oflag, cred_t *crp)
{
	(void) oflag;
	(void) crp;
	if (!q->q_ptr)
		return (ENXIO);
	timod_free_priv(q);
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
static struct qinit timod_qinit = {
	qi_putp:timod_put,
	qi_qopen:timod_open,
	qi_qclose:timod_close,
	qi_minfo:&timod_minfo,
};

static struct streamtab timod_info = {
	st_rdinit:&timod_qinit,
	st_wrinit:&timod_qinit,
};

static struct fmodsw timod_fmod = {
	f_name:CONFIG_STREAMS_TIMOD_NAME,
	f_str:&timod_info,
	f_flag:0,
	f_kmod:THIS_MODULE,
};

static int __init timod_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO TIMOD_BANNER);
#else
	printk(KERN_INFO TIMOD_SPLASH);
#endif
	if ((err = register_strmod(modid, &timod_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};
static void __exit timod_exit(void)
{
	int err;

	if ((err = unregister_strmod(modid, &timod_fmod)))
		return (void) (err);
	return (void) (0);
};

module_init(timod_init);
module_exit(timod_exit);
