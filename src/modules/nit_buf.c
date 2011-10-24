/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 *  This is NIT_BUF, a classical STREAMS buffering module similar to bufmod(4) that performs the
 *  actions described in nit_buf(4).  Note that this is a stripped down version of bufmod(4).
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include <linux/autoconf.h>
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include <net/nit_buf.h>

#include "sys/config.h"

#define NBUF_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NBUF_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define NBUF_REVISION	"Lfs $RCSfile$ $Name$($Revision$) $Date$"
#define NBUF_DEVICE	"SVR 4.1 SNIT Buffer Module (NBUF) for STREAMS."
#define NBUF_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NBUF_LICENSE	"GPL"
#define NBUF_BANNER	NBUF_DESCRIP	"\n" \
			NBUF_COPYRIGHT	"\n" \
			NBUF_REVISION	"\n" \
			NBUF_DEVICE	"\n" \
			NBUF_CONTACT	"\n"
#define NBUF_SPLASH	NBUF_DEVICE	" - " \
			NBUF_REVISION

#ifdef CONFIG_STREAMS_NBUF_MODULE
MODULE_AUTHOR(NBUF_CONTACT);
MODULE_DESCRIPTION(NBUF_DESCRIP);
MODULE_SUPPORTED_DEVICE(NBUF_DEVICE);
MODULE_LICENSE(NBUF_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_EVNR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_NBUF_MODULE */

#ifndef CONFIG_STREAMS_NBUF_NAME
//#define CONFIG_STREAMS_NBUF_NAME "nbuf"
#error "CONFIG_STREAMS_NBUF_NAME must be defined."
#endif				/* CONFIG_STREAMS_NBUF_NAME */
#ifndef CONFIG_STREAMS_NBUF_MODID
//#define CONFIG_STREAMS_NBUF_MODID 32
#error "CONFIG_STREAMS_NBUF_MODID must be defined."
#endif				/* CONFIG_STREAMS_NBUF_MODID */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-nbuf");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_NBUF_MODULE
static
#endif					/* CONFIG_STREAMS_NBUF_MODULE */
modID_t modid = CONFIG_STREAMS_NBUF_MODID;

#ifdef CONFIG_STREAMS_NBUF_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for NBUF.");
#endif				/* CONFIG_STREAMS_NBUF_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NBUF_MODID));
MODULE_ALIAS("streams-module-nbuf");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info nbuf_modinfo = {
	.mi_idnum = CONFIG_STREAMS_NBUF_MODID,
	.mi_idname = CONFIG_STREAMS_NBUF_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat nbuf_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat nbuf_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

STATIC streamscall int
nbuf_wput(queue_t *q, mblk_t *mp)
{
}

/*
 * nit_buf collects incoming M_DATA and M_PROTO messages into chunks, passing
 * each chunk upward when either the chunk becomes full or the current read
 * timeout expires.  When a message arrives, it is processed for inclusion in a
 * chunk, and then it is added to the current chunk.
 *
 * Upon receiving a message from below, nit_buf immediately converts all leading
 * M_PROTO message blocks in the message to M_DATA message blocks, altering only
 * the type field and leaving the contents alone.
 */
STATIC streamscall int
nbuf_rput(queue_t *q, mblk_t *mp)
{
	unsigned char db_type = mp->b_datap->db_type;

	if (unlikely(db_type == M_FLUSH)) {
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
	}
	/* let priority message jump the queue */
	if (likely(db_type >= QPCTL)) {
		putnext(q, mp);
		return (0);
	}
	if (unlikely(db_type == M_PROTO)) {
		register mblk_t *b;

		for (b = mp; b != NULL && b->b_datap->db_type == M_PROTO; b = b->b_next)
			b->b_datap->db_type = M_DATA;
	}
	if (unlikely(putq(q, mp) == 0)) {
		mp->b_band = 0;
		putq(q, mp);	/* this must succeed */
	}
	return (0);
}

STATIC streamscall int
nbuf_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		unsigned char db_type = mp->b_datap->db_type;

		if (db_type >= QPCTL) {
			putnext(q, mp);
			continue;
		}
		if (db_type == M_DATA) {
			if (/* message can be released */) {
				if (likely(bcanputnext(q, mp->b_band))) {
					putnext(q, mp);
					continue;
				}
			} else {
				mblk_t *bp;

				while ((bp = getq(q))) {
				}
			}
		} else {
			if (likely(bcanputnext(q, mp->b_band))) {
				putnext(q, mp);
				continue;
			}
		}
		putbq(q, mp);
		break;
	}
	return (0);
}

STATIC streamscall int
nbuf_wsrv(queue_t *q)
{
}

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
nbuf_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	wq = WR(q);
	if (sflag == MODOPEN || wq->q_next != NULL) {
		/* inherit packet sizes of what we are pushed over */
		wq->q_minpsz = wq->q_next->q_minpsz;
		wq->q_maxpsz = wq->q_next->q_maxpsz;
		q->q_ptr = wq->q_ptr = q;	/* just set it to something */
		qprocson(q);
		return (0);
	}
	return (EIO);		/* can't be opened as driver */
}

STATIC streamscall int
nbuf_close(queue_t *q, int oflag, cred_t *crp)
{
	(void) oflag;
	(void) crp;
	if (!q->q_ptr)
		return (ENXIO);
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
STATIC struct qinit nbuf_rinit = {
	.qi_putp = nbuf_rput,
	.qi_bufp = nbuf_rsrv,
	.qi_qopen = nbuf_open,
	.qi_qclose = nbuf_close,
	.qi_minfo = &nbuf_minfo,
	.qi_mstat = &nbuf_rstat,
};

STATIC struct qinit nbuf_winit = {
	.qi_putp = nbuf_wput,
	.qi_bufp = nbuf_wsrv,
	.qi_minfo = &nbuf_minfo,
	.qi_mstat = &nbuf_wstat,
};

#ifdef CONFIG_STREAMS_NBUF_MODULE
STATIC
#endif
struct streamtab nbufinfo = {
	.st_rdinit = &nbuf_rinit,
	.st_wrinit = &nbuf_winit,
};

STATIC struct fmodsw nbuf_fmod = {
	.f_name = CONFIG_STREAMS_NBUF_NAME,
	.f_str = &nbufinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_NBUF_MODULE
STATIC
#endif
int __init
nbufinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_NBUF_MODULE
	printk(KERN_INFO NBUF_BANNER);
#else
	printk(KERN_INFO NBUF_SPLASH);
#endif
	nbuf_minfo.mi_idnum = modid;
	if ((err = register_strmod(&nbuf_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_NBUF_MODULE
STATIC
#endif
void __exit
nbufexit(void)
{
	int err;

	if ((err = unregister_strmod(&nbuf_fmod)) < 0)
		return (void) (err);
	return (void) (0);
}

#ifdef CONFIG_STREAMS_NBUF_MODULE
module_init(nbufinit);
module_exit(nbufexit);
#endif
