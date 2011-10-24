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
 *  This is PFMOD a classical STREAMS packet filtering module that performs the actions described in
 *  pfmod(4).
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

#include <sys/pfmod.h>

#include "sys/config.h"

#define PFMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PFMOD_COPYRIGHT		"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define PFMOD_REVISION		"Lfs $RCSfile$ $Name$($Revision$) $Date$"
#define PFMOD_DEVICE		"SVR 4.2 Packet Filter Module (PFMOD) for STREAMS"
#define PFMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define PFMOD_LICENSE		"GPL"
#define PFMOD_BANNER		PFMOD_DESCRIP		"\n" \
				PFMOD_COPYRIGHT		"\n" \
				PFMOD_REVISION		"\n" \
				PFMOD_DEVICE		"\n" \
				PFMOD_CONTACT		"\n"
#define PFMOD_SPLASH		PFMOD_DEVICE		" - " \
				PFMOD_REVISION

#ifdef CONFIG_STREAMS_PFMOD_MODULE
MODULE_AUTHOR(PFMOD_CONTACT);
MODULE_DESCRIPTION(PFMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(PFMOD_DEVICE);
MODULE_LICENSE(PFMOD_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_EVNR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_PFMOD_MODULE */

#ifndef CONFIG_STREAMS_PFMOD_NAME
//#define CONFIG_STREAMS_PFMOD_NAME "pfmod"
#error "CONFIG_STREAMS_PFMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_PFMOD_MODID
//#define CONFIG_STREAMS_PFMOD_MODID 15
#error "CONFIG_STREAMS_PFMOD_MODID must be defined."
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-pfmod")
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#ifndef CONFIG_STREAMS_PFMOD_MODULE
	static
#endif
	modID_t modid = CONFIG_STREAMS_PFMOD_MODID;

#ifdef CONFIG_STREAMS_PFMOD_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for PFMOD.");
#endif				/* CONFIG_STREAMS_PFMOD_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PFMOD_MODID));
MODULE_ALIAS("streams-module-pfmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info pfmod_minfo = {
	.mi_idnum = CONFIG_STREAMS_PFMOD_MODID,
	.mi_idname = CONFIG_STREAMS_PFMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat pfmod_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat pfmod_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

STATIC streamscall int
pfmod_wput(queue_t *q, mblk_t *mp)
{
	putq(q, mp);
	return (0);
}

STATIC streamscall int
pfmod_rput(queue_t *q, mblk_t *mp)
{
	putq(q, mp);
	return (0);
}

STATIC streamscall int
pfmod_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (likely(mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band))) {
			putnext(q, mp);
			continue;
		}
		putbq(q, mp);
		break;
	}
	return (0);
}

STATIC streamscall int
pfmod_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (likely(mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band))) {
			putnext(q, mp);
			continue;
		}
		putbq(q, mp);
		break;
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
pfmod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;

	if (q->q_ptr != NULL) {
		return (0); /* already open */
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
pfmod_close(queue_t *q, int oflag, cred_t *crp)
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
STATIC struct qinit pfmod_rinit = {
	.qi_putp = pfmod_rput,
	.qi_bufp = pfmod_rsrv,
	.qi_qopen = pfmod_open,
	.qi_qclose = pfmod_close,
	.qi_minfo = &pfmod_minfo,
	.qi_mstat = &pfmod_rstat,
};

STATIC struct qinit pfmod_winit = {
	.qi_putp = pfmod_wput,
	.qi_bufp = pfmod_wsrv,
	.qi_minfo = &pfmod_minfo,
	.qi_mstat = &pfmod_wstat,
};

#ifdef CONFIG_STREAMS_PFMOD_MODULE
STATIC
#endif
struct streamtab pfmodinfo = {
	.st_rdinit = &pfmod_rinit,
	.st_wrinit = &pfmod_winit,
};

STATIC struct fmodsw pfmod_fmod = {
	.f_name = CONFIG_STREAMS_PFMOD_NAME,
	.f_str = &pfmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_PFMOD_MODULE
STATIC
#endif
int __init
pfmodinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_PFMOD_MODULE
	printk(KERN_INFO PFMOD_BANNER);
#else
	printk(KERN_INFO PFMOD_SPLASH);
#endif
	pfmod_minfo.mi_idnum = modid;
	if ((err = register_strmod(&pfmod_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_PFMOD_MODULE
STATIC
#endif
void __exit
pfmodexit(void)
{
	int err;

	if ((err = unregister_strmod(&pfmod_fmod)) < 0)
		return (void) (err);
	return (void) (0);
}

#ifdef CONFIG_STREAMS_PFMOD_MODULE
module_init(pfmodinit);
module_exit(pfmodexit);
#endif

