/*****************************************************************************

 @(#) $RCSfile: pipemod.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/08/14 12:58:05 $

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

 Last Modified $Date: 2007/08/14 12:58:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: pipemod.c,v $
 Revision 0.9.2.33  2007/08/14 12:58:05  brian
 - GNUv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: pipemod.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/08/14 12:58:05 $"

static char const ident[] =
    "$RCSfile: pipemod.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/08/14 12:58:05 $";

/* 
 *  This is PIPEMOD a STREAMS-based pipe (s_pipe(3)) module that reverses the
 *  send of the M_FLUSH message.  It is pushed on one side of STREAMS-based
 *  pipes.
 *
 *  This is an absurdly simple module.
 */

#define _LFS_SOURCE

#include <sys/os7/compat.h>

#ifdef LIS
#define CONFIG_STREAMS_PIPEMOD_MODID	PIPEMOD_MOD_ID
#define CONFIG_STREAMS_PIPEMOD_NAME	PIPEMOD_MOD_NAME
#endif

#define PIPEMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PIPEMOD_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define PIPEMOD_REVISION	"LfS $RCSfile: pipemod.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/08/14 12:58:05 $"
#define PIPEMOD_DEVICE		"SVR 4.2 Pipe Module for STREAMS-based Pipes"
#define PIPEMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define PIPEMOD_LICENSE		"GPL v2"
#define PIPEMOD_BANNER		PIPEMOD_DESCRIP		"\n" \
				PIPEMOD_COPYRIGHT	"\n" \
				PIPEMOD_REVISION	"\n" \
				PIPEMOD_DEVICE		"\n" \
				PIPEMOD_CONTACT		"\n"
#define PIPEMOD_SPLASH		PIPEMOD_DEVICE		" - " \
				PIPEMOD_REVISION	"\n"

#ifdef CONFIG_STREAMS_PIPEMOD_MODULE
MODULE_AUTHOR(PIPEMOD_CONTACT);
MODULE_DESCRIPTION(PIPEMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(PIPEMOD_DEVICE);
MODULE_LICENSE(PIPEMOD_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-pipemod");
#endif
#endif

#ifndef CONFIG_STREAMS_PIPEMOD_NAME
//#define CONFIG_STREAMS_PIPEMOD_NAME "pipemod"
#error "CONFIG_STREAMS_PIPEMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_PIPEMOD_MODID
//#define CONFIG_STREAMS_PIPEMOD_MODID 13
#error "CONFIG_STREAMS_PIPEMOD_MODID must be defined."
#endif

modID_t modid = CONFIG_STREAMS_PIPEMOD_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for PIPEMOD.");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PIPEMOD_MODID));
MODULE_ALIAS("streams-module-pipemod");
#endif
#endif

static struct module_info pipemod_minfo = {
	.mi_idnum = CONFIG_STREAMS_PIPEMOD_MODID,
	.mi_idname = CONFIG_STREAMS_PIPEMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat pipemod_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

static streamscall int
pipemod_put(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type == M_FLUSH)) {
		switch (mp->b_rptr[0] & (FLUSHR | FLUSHW)) {
		case FLUSHR:
			mp->b_rptr[0] &= ~FLUSHR;
			mp->b_rptr[0] |= FLUSHW;
			break;
		case FLUSHW:
			mp->b_rptr[0] &= ~FLUSHW;
			mp->b_rptr[0] |= FLUSHR;
			break;
		}
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
static streamscall int
pipemod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
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
static streamscall int
pipemod_close(queue_t *q, int oflag, cred_t *crp)
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
static struct qinit pipemod_qinit = {
	.qi_putp = pipemod_put,
	.qi_qopen = pipemod_open,
	.qi_qclose = pipemod_close,
	.qi_minfo = &pipemod_minfo,
	.qi_mstat = &pipemod_mstat,
};

static struct streamtab pipemod_info = {
	.st_rdinit = &pipemod_qinit,
	.st_wrinit = &pipemod_qinit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif
static struct fmodsw pipemod_fmod = {
	.f_name = CONFIG_STREAMS_PIPEMOD_NAME,
	.f_str = &pipemod_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_PIPEMOD_MODULE
static
#endif
int __init
pipemod_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_PIPEMOD_MODULE
	printk(KERN_INFO PIPEMOD_BANNER);
#else
	printk(KERN_INFO PIPEMOD_SPLASH);
#endif
	pipemod_minfo.mi_idnum = modid;
	if ((err = register_strmod(&pipemod_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};

#ifdef CONFIG_STREAMS_PIPEMOD_MODULE
static
#endif
void __exit
pipemod_exit(void)
{
	int err;

	if ((err = unregister_strmod(&pipemod_fmod)) < 0)
		return (void) (err);
	return (void) (0);
};

#ifdef CONFIG_STREAMS_PIPEMOD_MODULE
module_init(pipemod_init);
module_exit(pipemod_exit);
#endif
