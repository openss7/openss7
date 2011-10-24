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
 *  This is NIT_PF, a classical STREAMS packet filter module similar to pfmod(4) that performs the
 *  actions described in nit_pf(4).  Note that this is a stripped down version of pfmod(4).
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

#include <net/nit_pf.h>

#include "sys/config.h"

#define PF_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PF_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define PF_REVISION	"Lfs $RCSfile$ $Name$($Revision$) $Date$"
#define PF_DEVICE	"SVR 4.1 SNIT Packet Filter (PF) for STREAMS."
#define PF_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define PF_LICENSE	"GPL"
#define PF_BANNER	PF_DESCRIP	"\n" \
			PF_COPYRIGHT	"\n" \
			PF_REVISION	"\n" \
			PF_DEVICE	"\n" \
			PF_CONTACT	"\n"
#define PF_SPLASH	PF_DEVICE	" - " \
			PF_REVISION

#ifdef CONFIG_STREAMS_PF_MODULE
MODULE_AUTHOR(PF_CONTACT);
MODULE_DESCRIPTION(PF_DESCRIP);
MODULE_SUPPORTED_DEVICE(PF_DEVICE);
MODULE_LICENSE(PF_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_EVNR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_PF_MODULE */

#ifndef CONFIG_STREAMS_PF_NAME
//#define CONFIG_STREAMS_PF_NAME "pf"
#error "CONFIG_STREAMS_PF_NAME must be defined."
#endif				/* CONFIG_STREAMS_PF_NAME */
#ifndef CONFIG_STREAMS_PF_MODID
//#define CONFIG_STREAMS_PF_MODID 32
#error "CONFIG_STREAMS_PF_MODID must be defined."
#endif				/* CONFIG_STREAMS_PF_MODID */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-pf");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_PF_MODULE
static
#endif					/* CONFIG_STREAMS_PF_MODULE */
modID_t modid = CONFIG_STREAMS_PF_MODID;

#ifdef CONFIG_STREAMS_PF_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for PF.");
#endif				/* CONFIG_STREAMS_PF_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PF_MODID));
MODULE_ALIAS("streams-module-pf");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info pf_modinfo = {
	.mi_idnum = CONFIG_STREAMS_PF_MODID,
	.mi_idname = CONFIG_STREAMS_PF_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat pf_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat pf_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

STATIC streamscall int
pf_wput(queue_t *q, mblk_t *mp)
{
}

STATIC streamscall int
pf_rput(queue_t *q, mblk_t *mp)
{
}

STATIC streamscall int
pf_rsrv(queue_t *q)
{
}

STATIC streamscall int
pf_wsrv(queue_t *q)
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
pf_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
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
pf_close(queue_t *q, int oflag, cred_t *crp)
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
STATIC struct qinit pf_rinit = {
	.qi_putp = pf_rput,
	.qi_bufp = pf_rsrv,
	.qi_qopen = pf_open,
	.qi_qclose = pf_close,
	.qi_minfo = &pf_minfo,
	.qi_mstat = &pf_rstat,
};

STATIC struct qinit pf_winit = {
	.qi_putp = pf_wput,
	.qi_bufp = pf_wsrv,
	.qi_minfo = &pf_minfo,
	.qi_mstat = &pf_wstat,
};

#ifdef CONFIG_STREAMS_PF_MODULE
STATIC
#endif
struct streamtab pfinfo = {
	.st_rdinit = &pf_rinit,
	.st_wrinit = &pf_winit,
};

STATIC struct fmodsw pf_fmod = {
	.f_name = CONFIG_STREAMS_PF_NAME,
	.f_str = &pfinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_PF_MODULE
STATIC
#endif
int __init
pfinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_PF_MODULE
	printk(KERN_INFO PF_BANNER);
#else
	printk(KERN_INFO PF_SPLASH);
#endif
	pf_minfo.mi_idnum = modid;
	if ((err = register_strmod(&pf_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_PF_MODULE
STATIC
#endif
void __exit
pfexit(void)
{
	int err;

	if ((err = unregister_strmod(&pf_fmod)) < 0)
		return (void) (err);
	return (void) (0);
}

#ifdef CONFIG_STREAMS_PF_MODULE
module_init(pfinit);
module_exit(pfexit);
#endif
