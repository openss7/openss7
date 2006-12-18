/*****************************************************************************

 @(#) $RCSfile: pipemod.c,v $ $Name:  $($Revision: 1.1.1.4.4.7 $) $Date: 2005/12/18 06:37:53 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2005/12/18 06:37:53 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: pipemod.c,v $ $Name:  $($Revision: 1.1.1.4.4.7 $) $Date: 2005/12/18 06:37:53 $"

/*
 *  pipemod.c - "pipemod" module
 *
 *  pipemod's only function is to facilitate flush handling at the
 *  midpoint of pipes by switching the sense of the flush flags in
 *  M_FLUSH messages.  In this implementation, we also do flush
 *  handling as well (since this is a module, and modules are expected
 *  to do flush handling...).
 *
 *  Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
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
#ifndef PIPEMOD__MOD
#error Not configured
#endif

#ifndef PIPEMOD__ID
#define PIPEMOD__ID 0x8888
#endif

#define  MOD_ID   PIPEMOD__ID
#define  MOD_NAME "pipemod"

/*
 *  function prototypes
 */
static int _RP pipemod_open(queue_t *, dev_t *, int, int, cred_t *);
static int _RP pipemod_close(queue_t *, int, cred_t *);
static int _RP pipemod_put(queue_t *, mblk_t *);

static void flush_module(queue_t *, mblk_t *);

/*
 *  module structure
 */
static struct module_info pipemod_minfo = {
	MOD_ID,				/* id */
	MOD_NAME,			/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* highwater mark */
	512L				/* lowwater mark */
};

static struct qinit pipemod_rinit = {
	pipemod_put,			/* put */
	NULL,				/* service */
	pipemod_open,			/* open */
	pipemod_close,			/* close */
	NULL,				/* admin */
	&pipemod_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit pipemod_winit = {
	pipemod_put,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&pipemod_minfo,			/* info */
	NULL				/* stat */
};

struct streamtab pipemod_info = {
	&pipemod_rinit,			/* read queue init */
	&pipemod_winit,			/* write queue init */
	NULL,				/* lower mux read queue init */
	NULL				/* lower mux write queue init */
};

/*
 *  open
 */
static int _RP
pipemod_open(q, devp, flag, sflag, credp)
	queue_t *q;
	dev_t *devp;
	int flag, sflag;
	cred_t *credp;
{
	queue_t *rq = RD(q);
	char *cp = rq->q_ptr;

#ifdef PIPE_DEBUG
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
static int _RP
pipemod_close(q, flag, credp)
	queue_t *q;
	int flag;
	cred_t *credp;
{
#ifdef PIPE_DEBUG
	cmn_err(CE_CONT, "%s_close( 0x%p, 0x%x, ... )\n", MOD_NAME, q, flag);
#endif

	RD(q)->q_ptr = WR(q)->q_ptr = NULL;
	MODPUT();
	qprocsoff(q);

	return 0;		/* success */
}

/*
 *  canonical module flush handling - this could be used in any module
 */
static void
flush_module(q, mp)
	queue_t *q;
	mblk_t *mp;
{
	queue_t *rq = RD(q), *wq = WR(q);

	if (*(mp->b_rptr) & FLUSHW) {
		if (*(mp->b_rptr) & FLUSHBAND)
			flushband(wq, FLUSHDATA, *(mp->b_rptr + 1));
		else
			flushq(wq, FLUSHDATA);
	}
	if (*(mp->b_rptr) & FLUSHR) {
		if (*(mp->b_rptr) & FLUSHBAND)
			flushband(rq, FLUSHDATA, *(mp->b_rptr + 1));
		else
			flushq(rq, FLUSHDATA);
	}
	if (!SAMESTR(q)) {
		char flush = *(mp->b_rptr) & FLUSHRW;

		switch (flush & FLUSHRW) {
		case FLUSHR:
		case FLUSHW:
			*(mp->b_rptr) = (flush & ~FLUSHRW) | (flush ^ FLUSHRW);
		default:
			break;
		}
	}
	putnext(q, mp);
}

/*
 *  put
 */
static int _RP
pipemod_put(q, mp)
	queue_t *q;
	mblk_t *mp;
{
#ifdef PIPE_DEBUG
	cmn_err(CE_CONT, "%s_wput( 0x%08x, 0x%08x )\n", MOD_NAME, q, mp);
#endif

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		flush_module(q, mp);
		break;
	default:
		putnext(q, mp);
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
pipemod_init_module(void)
#else
int
init_module(void)
#endif
{
	int ret = lis_register_strmod(&pipemod_info, MOD_NAME);

	if (ret < 0) {
		cmn_err(CE_CONT, "%s - unable to register module.\n", MOD_NAME);
		return ret;
	}

	return 0;
}

#ifdef KERNEL_2_5
void
pipemod_cleanup_module(void)
#else
void
cleanup_module(void)
#endif
{
	if (lis_unregister_strmod(&pipemod_info) < 0)
		cmn_err(CE_CONT, "%s - unable to unregister module.\n", MOD_NAME);
	return;
}

#ifdef KERNEL_2_5
module_init(pipemod_init_module);
module_exit(pipemod_cleanup_module);
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
MODULE_AUTHOR("John Boyd <jaboydjr@protologos.net>");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("STREAMS 'pipemod' pipe flushing module");
#endif
#if defined(MODULE_ALIAS)
MODULE_ALIAS("streams-" __stringify(LIS_OBJNAME));
#endif

#endif				/* LINUX */
#endif				/* MODULE */
