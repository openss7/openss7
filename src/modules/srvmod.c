/*****************************************************************************

 @(#) $RCSfile: srvmod.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:51 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-09-02 08:46:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: srvmod.c,v $
 Revision 1.1.2.4  2011-09-02 08:46:51  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.3  2011-05-31 09:46:10  brian
 - new distros

 Revision 1.1.2.2  2010-11-28 14:22:00  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:40:30  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: srvmod.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:51 $";

/*
 *  This is SRVMOD a STREAMS buffering module that performs no actions other than acting as a
 *  STREAMS module and buffering input and output.  Its purpose is primarily for testing and for
 *  serving as an example of the skeleton of a STREAMS module.
 *
 *  This is an absurdly simple module.
 *
 *  In addition, this module provide a specialized set of input-output controls for testing and
 *  verifying various internal STREAMS functions.
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
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

#include "sys/config.h"

#define SRVMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SRVMOD_COPYRIGHT	"Copyright (c) 2008-2012  Monavacon Limited.  All Rights Reserved."
#define SRVMOD_REVISION		"LfS $RCSfile: srvmod.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:51 $"
#define SRVMOD_DEVICE		"SVR 4.2 Buffer Module (SRVMOD) for STREAMS"
#define SRVMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SRVMOD_LICENSE		"GPL"
#define SRVMOD_BANNER		SRVMOD_DESCRIP		"\n" \
				SRVMOD_COPYRIGHT	"\n" \
				SRVMOD_REVISION		"\n" \
				SRVMOD_DEVICE		"\n" \
				SRVMOD_CONTACT		"\n"
#define SRVMOD_SPLASH		SRVMOD_DEVICE		" - " \
				SRVMOD_REVISION		"\n"

#ifdef CONFIG_STREAMS_SRVMOD_MODULE
MODULE_AUTHOR(SRVMOD_CONTACT);
MODULE_DESCRIPTION(SRVMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(SRVMOD_DEVICE);
MODULE_LICENSE(SRVMOD_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_SRVMOD_MODULE */

#ifndef CONFIG_STREAMS_SRVMOD_NAME
//#define CONFIG_STREAMS_SRVMOD_NAME "srvmod"
#error "CONFIG_STREAMS_SRVMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_SRVMOD_MODID
//#define CONFIG_STREAMS_SRVMOD_MODID 13
#error "CONFIG_STREAMS_SRVMOD_MODID must be defined."
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-srvmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_SRVMOD_MODULE
static
#endif
modID_t modid = CONFIG_STREAMS_SRVMOD_MODID;

#ifdef CONFIG_STREAMS_SRVMOD_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for SRVMOD.");
#endif				/* CONFIG_STREAMS_SRVMOD_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SRVMOD_MODID));
MODULE_ALIAS("streams-module-srvmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info srvmod_minfo = {
	.mi_idnum = CONFIG_STREAMS_SRVMOD_MODID,
	.mi_idname = CONFIG_STREAMS_SRVMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat srvmod_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat srvmod_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

STATIC streamscall int
srvmod_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type == M_FLUSH)) {
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
	}
	if (likely(mp->b_datap->db_type >= QPCTL || (q->q_first == NULL && !(q->q_flag & QSVCBUSY)
						     && bcanputnext(q, mp->b_band)))) {
		putnext(q, mp);
		return (0);
	}
	/* always buffer, always schedule out of service procedure for testing */
	if (unlikely(putq(q, mp) == 0)) {
		mp->b_band = 0;
		putq(q, mp);	/* this must succeed */
	}
	return (0);
}

STATIC streamscall int
srvmod_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type == M_FLUSH)) {
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
	}
	if (likely(mp->b_datap->db_type >= QPCTL || (q->q_first == NULL && !(q->q_flag & QSVCBUSY)
						     && bcanputnext(q, mp->b_band)))) {
		putnext(q, mp);
		return (0);
	}
	if (unlikely(putq(q, mp) == 0)) {
		mp->b_band = 0;
		putq(q, mp);	/* this must succeed */
	}
	return (0);
}

STATIC streamscall int
srvmod_srv(queue_t *q)
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
srvmod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
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
srvmod_close(queue_t *q, int oflag, cred_t *crp)
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
STATIC struct qinit srvmod_rinit = {
	.qi_putp = srvmod_rput,
	.qi_srvp = srvmod_srv,
	.qi_qopen = srvmod_open,
	.qi_qclose = srvmod_close,
	.qi_minfo = &srvmod_minfo,
	.qi_mstat = &srvmod_rstat,
};

STATIC struct qinit srvmod_winit = {
	.qi_putp = srvmod_wput,
	.qi_srvp = srvmod_srv,
	.qi_minfo = &srvmod_minfo,
	.qi_mstat = &srvmod_wstat,
};

#ifdef CONFIG_STREAMS_SRVMOD_MODULE
STATIC
#endif
struct streamtab srvmodinfo = {
	.st_rdinit = &srvmod_rinit,
	.st_wrinit = &srvmod_winit,
};

STATIC struct fmodsw srvmod_fmod = {
	.f_name = CONFIG_STREAMS_SRVMOD_NAME,
	.f_str = &srvmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_SRVMOD_MODULE
STATIC
#endif
int __init
srvmodinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_SRVMOD_MODULE
	printk(KERN_INFO SRVMOD_BANNER);
#else
	printk(KERN_INFO SRVMOD_SPLASH);
#endif
	srvmod_minfo.mi_idnum = modid;
	if ((err = register_strmod(&srvmod_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_SRVMOD_MODULE
STATIC
#endif
void __exit
srvmodexit(void)
{
	int err;

	if ((err = unregister_strmod(&srvmod_fmod)) < 0)
		return (void) (err);
	return (void) (0);
}

#ifdef CONFIG_STREAMS_SRVMOD_MODULE
module_init(srvmodinit);
module_exit(srvmodexit);
#endif
