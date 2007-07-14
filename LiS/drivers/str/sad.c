/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile: sad.c,v $ $Name:  $($Revision: 1.1.1.3.4.9 $) $Date: 2005/12/19 03:22:18 $"

/*
 *  sad: STREAMS Administrative Driver
 *
 *  Version: 0.1
 *
 *  Copyright (C) 1999 Ole Husgaard (sparre@login.dknet.dk)
 *
 */

#include <sys/LiS/module.h>	/* must be VERY first include */

#include <sys/stream.h>

#include <sys/sad.h>

#include <sys/LiS/config.h>
#include <sys/osif.h>

/*
 *  Some configuration sanity checks
 */
#ifndef SAD__DRV
#error Not configured
#endif

#if !defined(SAD__CMAJORS) || SAD__CMAJORS != 1
#error There should be exactly one major number
#endif

#if !defined(SAD__CMAJOR_0)
#error The major number should be defined
#endif

#if !defined(SAD_N_MINOR)
#define SAD_N_MINOR 4
#endif

#ifndef STATIC
#define STATIC static
#endif
#ifndef INLINE
#define INLINE inline
#endif

STATIC int _RP sad_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int _RP sad_close(queue_t *, int, cred_t *);
STATIC int _RP sad_wput(queue_t *, mblk_t *);

STATIC struct module_info sad_minfo = {
	0,				/* Module ID number */
	"sad",				/* Module name */
	0,				/* Min packet size accepted */
	INFPSZ,				/* Max packet size accepted */
	0,				/* Hi water mark ignored */
	0				/* Low water mark ignored */
};

STATIC struct qinit sad_rinit = {
	NULL,				/* No read put */
	NULL,				/* No read service */
	sad_open,			/* Each open */
	sad_close,			/* Last close */
	NULL,				/* Reserved */
	&sad_minfo,			/* Information */
	NULL				/* No statistics */
};

STATIC struct qinit sad_winit = {
	sad_wput,			/* Write put */
	NULL,				/* No write service */
	NULL,				/* Ignored */
	NULL,				/* Ignored */
	NULL,				/* Reserved */
	&sad_minfo,			/* Information */
	NULL				/* No statistics */
};

#if !defined __NO_VERSION__
STATIC
#endif
struct streamtab sad_info = {
	&sad_rinit,			/* Read queue */
	&sad_winit,			/* Write queue */
	NULL,				/* Unused */
	NULL				/* Unused */
};

/*
 *  Private per-stream data
 */
struct priv {
	queue_t *rq;
	caddr_t uaddr;
	int ioc_state;
	str_list_t vml;
};

/*
 *  Values of priv->ioc_state
 */
#define ST_NONE		0
#define ST_SAP_IN	1
#define ST_GAP_IN	2
#define ST_GAP_OUT	3
#define ST_VML_IN1	4
#define ST_VML_IN2	5

/*
 *  Per stream storage
 */
STATIC struct priv sad_sad[SAD_N_MINOR];

/****************************************************************************/
/*                                                                          */
/*  copyin/copyout handling.                                                */
/*                                                                          */
/****************************************************************************/

STATIC void
sad_copyio(struct priv *p, mblk_t *mp, int type, caddr_t uaddr, size_t size)
{
	struct copyreq *req;

	mp->b_datap->db_type = type;
	mp->b_wptr = mp->b_rptr + sizeof(*req);
	LISASSERT(mp->b_wptr <= mp->b_datap->db_lim);

	req = (struct copyreq *) mp->b_rptr;
	req->cq_addr = uaddr;
	req->cq_size = size;
	req->cq_flag = 0;

	putnext(p->rq, mp);
}

STATIC void
sad_iocdata(struct priv *p, mblk_t *mp)
{
	struct copyresp *res = (struct copyresp *) mp->b_rptr;
	mblk_t *bp = mp->b_cont;
	str_list_t *sl;
	struct iocblk *iocp;
	int err = 0, ret = 0;

	if (res->cp_rval != 0) {
		err = (int) (-(intptr_t) (long) res->cp_rval);
		ret = -1;
		goto ioctl_done;
	}

	LISASSERT(bp != NULL);
	LISASSERT(bp->b_datap->db_type == M_DATA);

	switch (p->ioc_state) {
	case ST_SAP_IN:
		p->ioc_state = ST_NONE;
		LISASSERT(res->cp_cmd == SAD_SAP);
		if (res->cp_rval != 0) {
			err = -EFAULT;
			ret = -1;
		} else if (bp->b_wptr - bp->b_rptr == sizeof(struct strapush))
			err = lis_apush_set((struct strapush *) bp->b_rptr);
		else {
			err = -EINVAL;
			ret = -1;
		}

	      ioctl_done:iocp = (struct iocblk *) mp->b_rptr;
		mp->b_datap->db_type = (err < 0) ? M_IOCNAK : M_IOCACK;
		iocp->ioc_count = 0;
		iocp->ioc_error = -err;
		iocp->ioc_rval = ret;
		putnext(p->rq, mp);
		return;

	case ST_GAP_IN:
		p->ioc_state = ST_GAP_OUT;
		LISASSERT(res->cp_cmd == SAD_GAP);
		if (res->cp_rval != 0) {
			err = -EFAULT;
			ret = -1;
		} else if (bp->b_wptr - bp->b_rptr == sizeof(struct strapush))
			err = lis_apush_get((struct strapush *) bp->b_rptr);
		else {
			err = -EINVAL;
			ret = -1;
		}
		if (err < 0) {
			p->ioc_state = ST_NONE;
			goto ioctl_done;
		}
		sad_copyio(p, mp, M_COPYOUT, p->uaddr, sizeof(struct strapush));
		return;

	case ST_GAP_OUT:
		p->ioc_state = ST_NONE;
		LISASSERT(res->cp_cmd == SAD_GAP);
		if (res->cp_rval != 0) {
			err = -EFAULT;
			ret = -1;
		}
		goto ioctl_done;

	case ST_VML_IN1:
		LISASSERT(res->cp_cmd == SAD_VML);
		sl = (str_list_t *) bp->b_rptr;
		if (bp->b_wptr - bp->b_rptr != sizeof(str_list_t)
		    || (p->vml.sl_nmods = sl->sl_nmods) <= 0) {
			p->ioc_state = ST_NONE;
			err = -EINVAL;
			ret = -1;
			goto ioctl_done;
		}
		p->ioc_state = ST_VML_IN2;
		sad_copyio(p, mp, M_COPYIN, (caddr_t) sl->sl_modlist,
			   sl->sl_nmods * sizeof(str_mlist_t));
		return;
	case ST_VML_IN2:
		p->ioc_state = ST_NONE;
		LISASSERT(res->cp_cmd == SAD_VML);
		if (res->cp_rval != 0) {
			err = -EFAULT;
			ret = -1;
		} else if (bp->b_wptr - bp->b_rptr != p->vml.sl_nmods * sizeof(str_mlist_t)) {
			err = -EINVAL;
			ret = -1;
		}
		p->vml.sl_modlist = (struct str_mlist *) bp->b_rptr;
		ret = lis_apush_vml(&p->vml);
		err = 0;
		goto ioctl_done;

	default:
		err = -EINVAL;
		ret = -1;
		goto ioctl_done;
	}
}

/****************************************************************************/
/*                                                                          */
/*  ioctl handling.                                                         */
/*                                                                          */
/****************************************************************************/

STATIC INLINE void
sad_do_ioctl(struct priv *p, mblk_t *mp)
{
	struct iocblk *iocp;
	mblk_t *dp;
	int err;

	LISASSERT(p != NULL);

	LISASSERT(mp != NULL);
	LISASSERT(mp->b_datap->db_type == M_IOCTL);
	LISASSERT(mp->b_wptr - mp->b_rptr >= sizeof(struct iocblk));

	dp = mp->b_cont;
	LISASSERT(dp != NULL);
	LISASSERT(dp->b_datap->db_type == M_DATA);
	LISASSERT(mp->b_wptr - mp->b_rptr >= sizeof(void *));

	iocp = (struct iocblk *) mp->b_rptr;
	if (iocp->ioc_count != TRANSPARENT) {
		err = -EINVAL;
	      nak_it:mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -err;
		putnext(p->rq, mp);
		return;
	}

	if (p->ioc_state != ST_NONE) {
		err = -EINVAL;
		goto nak_it;
	}

	p->uaddr = *(caddr_t *) dp->b_rptr;

	switch (iocp->ioc_cmd) {
	case SAD_SAP:
		if (iocp->ioc_uid != 0) {
			err = -EACCES;
			goto nak_it;
		}
		p->ioc_state = ST_SAP_IN;
		sad_copyio(p, mp, M_COPYIN, p->uaddr, sizeof(struct strapush));
		return;
	case SAD_GAP:
		p->ioc_state = ST_GAP_IN;
		sad_copyio(p, mp, M_COPYIN, p->uaddr, sizeof(struct strapush));
		return;
	case SAD_VML:
		p->ioc_state = ST_VML_IN1;
		sad_copyio(p, mp, M_COPYIN, p->uaddr, sizeof(str_list_t));
		return;
	default:
		err = -EINVAL;
		goto nak_it;
	}
}

/****************************************************************************/
/*                                                                          */
/*  The usual STREAMS driver stuff.                                         */
/*                                                                          */
/****************************************************************************/

STATIC int _RP
sad_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	dev_t i;

	if (sflag == CLONEOPEN) {
		for (i = 1; i < SAD_N_MINOR; i++)
			if (sad_sad[i].rq == NULL)
				break;
		if (i == SAD_N_MINOR)
			return ENXIO;
	} else {
		if ((i = getminor(*devp)) >= SAD_N_MINOR)
			return EBUSY;
	}
	*devp = makedevice(getmajor(*devp), i);

	q->q_ptr = WR(q)->q_ptr = &sad_sad[i];
	sad_sad[i].rq = q;
	sad_sad[i].ioc_state = ST_NONE;

	MODGET();

	return 0;
}

STATIC int _RP
sad_close(queue_t *q, int flag, cred_t *crp)
{
	struct priv *p = q->q_ptr;

	LISASSERT(p != NULL);

	p->rq = q->q_ptr = WR(q)->q_ptr = NULL;

	MODPUT();

	return 0;
}

STATIC int _RP
sad_wput(queue_t *q, mblk_t *mp)
{
	struct priv *p;

	LISASSERT(q != NULL);
	LISASSERT(mp != NULL);

	p = q->q_ptr;
	LISASSERT(p != NULL);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
			else
				flushq(RD(q), FLUSHDATA);
			qreply(q, mp);
		} else
			freemsg(mp);
		break;
	case M_IOCTL:
		sad_do_ioctl(p, mp);
		break;
	case M_IOCDATA:
		sad_iocdata(p, mp);
		break;
	default:
		freemsg(mp);
		break;
	}
	return (0);
}

/****************************************************************************/
/*                                                                          */
/*  Loadable module initialization and shutdown.                            */
/*                                                                          */
/****************************************************************************/

#if !defined __NO_VERSION__

#ifdef KERNEL_2_5
int
sad_init_module(void)
#else
int
init_module(void)
#endif
{
	int ret = lis_register_strdev(SAD__CMAJOR_0, &sad_info,
				      SAD_N_MINOR, LIS_OBJNAME_STR);

	if (ret < 0) {
		printk("%s: Unable to register module, error %d.\n", LIS_OBJNAME_STR, -ret);
		return ret;
	}
	memset(sad_sad, 0, sizeof(sad_sad));
	return 0;
}

#ifdef KERNEL_2_5
void
sad_cleanup_module(void)
#else
void
cleanup_module(void)
#endif
{
	int err = lis_unregister_strdev(SAD__CMAJOR_0);

	if (err < 0)
		printk("%s: Unable to unregister module, error %d.\n", LIS_OBJNAME_STR, -err);
	else
		printk("%s: Unregistered, ready to be unloaded.\n", LIS_OBJNAME_STR);
	return;
}

#ifdef KERNEL_2_5
module_init(sad_init_module);
module_exit(sad_cleanup_module);
#endif

#if defined(LINUX)		/* linux kernel */
#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL v2");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("Ole Husgaard (sparre@login.dknet.dk");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("STREAMS Administrative Driver");
#endif
#if defined(MODULE_ALIAS)
MODULE_ALIAS("streams-" __stringify(LIS_OBJNAME));
#endif
#endif

#endif				/* !defined __NO_VERSION__ */

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(SAD__CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(SAD__CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(CLONE__CMAJOR_0) "-" __stringify(SAD__CMAJOR_0));
MODULE_ALIAS("/dev/sad");
#endif
