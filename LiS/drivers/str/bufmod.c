/*****************************************************************************

 @(#) $RCSfile: bufmod.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2005/10/21 03:55:36 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/10/21 03:55:36 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: bufmod.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2005/10/21 03:55:36 $"

/*
 *  bufmod.c - "bufmod" module
 *
 *  bufmod is a buffering module used in performance testing of message queuing
 *  and scheduling.  It puts all received messages in both directions on the
 *  queue and schedules them from the service procedure.
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

/*
 *  Some configuration sanity checks
 */
#ifndef BUFMOD__MOD
#error Not configured
#endif

#ifndef BUFMOD__ID
#define BUFMOD__ID 0x8887
#endif

#define  MOD_ID   BUFMOD__ID
#define  MOD_NAME "bufmod"

/*
 *  function prototypes
 */
static int bufmod_open(queue_t *, dev_t *, int, int, cred_t *);
static int bufmod_close(queue_t *, int, cred_t *);

static int bufmod_rput(queue_t *, mblk_t *);
static int bufmod_wput(queue_t *, mblk_t *);
static int bufmod_srv(queue_t *);

/*
 *  module structure
 */
static struct module_info bufmod_minfo = {
	MOD_ID,				/* id */
	MOD_NAME,			/* name */
	0,				/* min packet size accepted */
	4096,				/* max packet size accepted */
	5120,				/* highwater mark */
	1024				/* lowwater mark */
};

static struct qinit bufmod_rinit = {
	bufmod_rput,			/* put */
	bufmod_srv,			/* service */
	bufmod_open,			/* open */
	bufmod_close,			/* close */
	NULL,				/* admin */
	&bufmod_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit bufmod_winit = {
	bufmod_wput,			/* put */
	bufmod_srv,			/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&bufmod_minfo,			/* info */
	NULL				/* stat */
};

struct streamtab bufmod_info = {
	&bufmod_rinit,			/* read queue init */
	&bufmod_winit,			/* write queue init */
	NULL,				/* lower mux read queue init */
	NULL				/* lower mux write queue init */
};

/*
 *  open
 */
static int
bufmod_open(q, devp, flag, sflag, credp)
	queue_t *q;
	dev_t *devp;
	int flag, sflag;
	cred_t *credp;
{
	queue_t *rq = RD(q);
	char *cp = rq->q_ptr;

#ifdef BUFMOD_DEBUG
	cmn_err(CE_CONT, "%s_open( 0x%p, 0x%x, 0x%x, 0x%x, ... )\n", MOD_NAME, q, *devp, flag,
		sflag);
#endif

	if (rq->q_ptr == NULL)
		MODGET();
	rq->q_ptr = (void *) ++cp;
	WR(q)->q_ptr = rq->q_ptr;

	qprocson(q);

	return 0;		/* success */
}

/*
 *  close
 */
static int
bufmod_close(q, flag, credp)
	queue_t *q;
	int flag;
	cred_t *credp;
{
#ifdef BUFMOD_DEBUG
	cmn_err(CE_CONT, "%s_close( 0x%p, 0x%x, ... )\n", MOD_NAME, q, flag);
#endif

	RD(q)->q_ptr = WR(q)->q_ptr = NULL;
	MODPUT();
	qprocsoff(q);

	return 0;		/* success */
}

/*
 *  put
 */
static int
bufmod_wput(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type == M_FLUSH) {
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHW)
				flushband(q, FLUSHALL, mp->b_rptr[1]);
			else
				flushq(q, FLUSHALL);
		}
	}
	if (!putq(q, mp)) {
		mp->b_band = 0;
		putq(q, mp);	/* must succeed */
	}
	return (0);
}
static int
bufmod_rput(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type == M_FLUSH) {
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHW)
				flushband(q, FLUSHALL, mp->b_rptr[1]);
			else
				flushq(q, FLUSHALL);
		}
	}
	if (!putq(q, mp)) {
		mp->b_band = 0;
		putq(q, mp);	/* must succeed */
	}
	return (0);
}
static int
bufmod_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			continue;
		}
		putbq(q, mp);	/* must succeed */
		break;
	}
	return (0);
}


#ifdef MODULE

/*
 *  Linux loadable module interface
 */

#ifdef KERNEL_2_5
int
bufmod_init_module(void)
#else
int
init_module(void)
#endif
{
	int ret = lis_register_strmod(&bufmod_info, MOD_NAME);

	if (ret < 0) {
		cmn_err(CE_CONT, "%s - unable to register module.\n", MOD_NAME);
		return ret;
	}

	return 0;
}

#ifdef KERNEL_2_5
void
bufmod_cleanup_module(void)
#else
void
cleanup_module(void)
#endif
{
	if (lis_unregister_strmod(&bufmod_info) < 0)
		cmn_err(CE_CONT, "%s - unable to unregister module.\n", MOD_NAME);
	return;
}

#ifdef KERNEL_2_5
module_init(bufmod_init_module);
module_exit(bufmod_cleanup_module);
#endif

#if defined(LINUX)		/* linux kernel */

#ifdef __attribute_used__
#undef __attribute_used__
#endif
#define __attribute_used__

#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("Brian Bidulock <bidulock@openss7.org>");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("STREAMS 'bufmod' buffering module");
#endif
#if defined(MODULE_ALIAS)
MODULE_ALIAS("streams-" __stringify(LIS_OBJNAME));
#endif

#endif				/* LINUX */
#endif				/* MODULE */
