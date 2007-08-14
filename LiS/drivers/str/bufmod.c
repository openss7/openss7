/*****************************************************************************

 @(#) $RCSfile: bufmod.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2007/08/14 10:46:56 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 10:46:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: bufmod.c,v $
 Revision 0.9.2.13  2007/08/14 10:46:56  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: bufmod.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2007/08/14 10:46:56 $"

static char const ident[] =
    "$RCSfile: bufmod.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2007/08/14 10:46:56 $";

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

#include <sys/LiS/module.h>	/* should be first */

#include <sys/LiS/config.h>

#include <sys/stream.h>
#include <sys/stropts.h>
#ifdef LINUX
#include <linux/errno.h>
#else
#include <sys/errno.h>
#endif
#include <sys/cmn_err.h>
#include <sys/osif.h>

#define BUFMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define BUFMOD_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define BUFMOD_REVISION		"LfS $RCSfile: bufmod.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2007/08/14 10:46:56 $"
#define BUFMOD_DEVICE		"SVR 4.2 Buffer Module (BUFMOD) for STREAMS"
#define BUFMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define BUFMOD_LICENSE		"GPL v2"
#define BUFMOD_BANNER		BUFMOD_DESCRIP		"\n" \
				BUFMOD_COPYRIGHT	"\n" \
				BUFMOD_REVISION		"\n" \
				BUFMOD_DEVICE		"\n" \
				BUFMOD_CONTACT		"\n"
#define BUFMOD_SPLASH		BUFMOD_DEVICE		" - " \
				BUFMOD_REVISION		"\n"

#if defined LIS && defined MODULE
#define CONFIG_STREAMS_BUFMOD_MODULE MODULE
#endif

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
#define CONFIG_STREAMS_BUFMOD_NAME BUFMOD__MOD_NAME
//#error "CONFIG_STREAMS_BUFMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_BUFMOD_MODID
#define CONFIG_STREAMS_BUFMOD_MODID BUFMOD__ID
//#error "CONFIG_STREAMS_BUFMOD_MODID must be defined."
#endif

modID_t modid = CONFIG_STREAMS_BUFMOD_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
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
	.mi_minpsz = 0,
	.mi_maxpsz = 4096,
	.mi_hiwat = 5120,
	.mi_lowat = 1024,
};

STATIC struct module_stat bufmod_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat bufmod_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

#ifndef unlikely
#define unlikely(__x) (__x)
#endif
#ifndef likely
#define likely(__x) (__x)
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

STATIC int _RP
bufmod_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type == M_FLUSH)) {
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
	}
	if (likely(mp->b_datap->db_type >= QPCTL || (q->q_first == NULL && !(q->q_flag & QRUNNING)
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

STATIC int _RP
bufmod_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type == M_FLUSH)) {
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
	}
	if (likely(mp->b_datap->db_type >= QPCTL || (q->q_first == NULL && !(q->q_flag & QRUNNING)
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

STATIC int _RP
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
STATIC int _RP
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
STATIC int _RP
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

#ifdef MODULE
STATIC
#endif
int __init
bufmod_init(void)
{
	int err;

#ifdef MODULE
	printk(KERN_INFO BUFMOD_BANNER);
#else
	printk(KERN_INFO BUFMOD_SPLASH);
#endif
	bufmod_minfo.mi_idnum = modid;
	if ((err = lis_register_strmod(&bufmod_info, CONFIG_STREAMS_BUFMOD_NAME)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	if ((err = lis_register_module_qlock_option(modid, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&bufmod_info);
		return (err);
	}
	return (0);
}

#ifdef MODULE
STATIC
#endif
void __exit
bufmod_exit(void)
{
	int err;

	if ((err = lis_unregister_strmod(&bufmod_info)) < 0)
		return (void) (err);
	return (void) (0);
}

#ifdef MODULE
module_init(bufmod_init);
module_exit(bufmod_exit);
#endif
