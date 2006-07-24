/*****************************************************************************

 @(#) $RCSfile: bufmod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/07/24 09:01:20 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2006/07/24 09:01:20 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: bufmod.c,v $
 Revision 0.9.2.7  2006/07/24 09:01:20  brian
 - results of udp2 optimizations

 Revision 0.9.2.6  2005/12/28 09:48:02  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.5  2005/12/19 03:23:39  brian
 - wend for simple streamscall

 Revision 0.9.2.4  2005/12/09 18:01:47  brian
 - profiling copy

 Revision 0.9.2.3  2005/10/22 19:58:19  brian
 - more performance testing an tweaks

 Revision 0.9.2.2  2005/10/21 03:54:27  brian
 - modifications for queueing testing

 Revision 0.9.2.1  2005/10/20 08:18:58  brian
 - modifications for queuing and scheduling testing


 *****************************************************************************/

#ident "@(#) $RCSfile: bufmod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/07/24 09:01:20 $"

static char const ident[] =
    "$RCSfile: bufmod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/07/24 09:01:20 $";

/*
 *  This is BUFMOD a STREAMS buffering module that performs no actions other than acting as a
 *  STREAMS module and buffering input and output.  Its purpose is primarily for testing and for
 *  serving as an example of the skeleton of a STREAMS module.
 *
 *  This is an absurdly simple module.
 *
 *  In addition, this module provide a specialized set of input-output controls for testing and
 *  verifying various internal STREAMS functions.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"

#define BUFMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define BUFMOD_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define BUFMOD_REVISION		"LfS $RCSfile: bufmod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2006/07/24 09:01:20 $"
#define BUFMOD_DEVICE		"SVR 4.2 Buffer Module (BUFMOD) for STREAMS"
#define BUFMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define BUFMOD_LICENSE		"GPL"
#define BUFMOD_BANNER		BUFMOD_DESCRIP		"\n" \
				BUFMOD_COPYRIGHT	"\n" \
				BUFMOD_REVISION		"\n" \
				BUFMOD_DEVICE		"\n" \
				BUFMOD_CONTACT		"\n"
#define BUFMOD_SPLASH		BUFMOD_DEVICE		" - " \
				BUFMOD_REVISION		"\n"

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
MODULE_AUTHOR(BUFMOD_CONTACT);
MODULE_DESCRIPTION(BUFMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(BUFMOD_DEVICE);
MODULE_LICENSE(BUFMOD_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-bufmod");
#endif
#endif

#ifndef CONFIG_STREAMS_BUFMOD_NAME
//#define CONFIG_STREAMS_BUFMOD_NAME "bufmod"
#error "CONFIG_STREAMS_BUFMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_BUFMOD_MODID
//#define CONFIG_STREAMS_BUFMOD_MODID 13
#error "CONFIG_STREAMS_BUFMOD_MODID must be defined."
#endif

modID_t modid = CONFIG_STREAMS_BUFMOD_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for BUFMOD.");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_BUFMOD_MODID));
MODULE_ALIAS("streams-module-bufmod");
#endif
#endif

STATIC struct module_info bufmod_minfo = {
	.mi_idnum = CONFIG_STREAMS_BUFMOD_MODID,
	.mi_idname = CONFIG_STREAMS_BUFMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat bufmod_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat bufmod_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

STATIC streamscall int
bufmod_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type == M_FLUSH)) {
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, FLUSHALL, mp->b_rptr[1]);
			else
				flushq(q, FLUSHALL);
		}
	}
	if (likely(mp->b_datap->db_type >= QPCTL
		   || (q->q_first == NULL && !(q->q_flag & QSVCBUSY)
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
bufmod_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type == M_FLUSH)) {
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, FLUSHALL, mp->b_rptr[1]);
			else
				flushq(q, FLUSHALL);
		}
	}
	if (likely(mp->b_datap->db_type >= QPCTL
		   || (q->q_first == NULL && !(q->q_flag & QSVCBUSY)
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
bufmod_srv(queue_t *q)
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
bufmod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
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
bufmod_close(queue_t *q, int oflag, cred_t *crp)
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
STATIC struct qinit bufmod_rinit = {
	.qi_putp = bufmod_rput,
	.qi_srvp = bufmod_srv,
	.qi_qopen = bufmod_open,
	.qi_qclose = bufmod_close,
	.qi_minfo = &bufmod_minfo,
	.qi_mstat = &bufmod_rstat,
};

STATIC struct qinit bufmod_winit = {
	.qi_putp = bufmod_wput,
	.qi_srvp = bufmod_srv,
	.qi_minfo = &bufmod_minfo,
	.qi_mstat = &bufmod_wstat,
};

STATIC struct streamtab bufmod_info = {
	.st_rdinit = &bufmod_rinit,
	.st_wrinit = &bufmod_winit,
};

STATIC struct fmodsw bufmod_fmod = {
	.f_name = CONFIG_STREAMS_BUFMOD_NAME,
	.f_str = &bufmod_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
STATIC
#endif
int __init
bufmod_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
	printk(KERN_INFO BUFMOD_BANNER);
#else
	printk(KERN_INFO BUFMOD_SPLASH);
#endif
	bufmod_minfo.mi_idnum = modid;
	if ((err = register_strmod(&bufmod_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
STATIC
#endif
void __exit
bufmod_exit(void)
{
	int err;

	if ((err = unregister_strmod(&bufmod_fmod)) < 0)
		return (void) (err);
	return (void) (0);
}

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
module_init(bufmod_init);
module_exit(bufmod_exit);
#endif
