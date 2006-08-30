/*****************************************************************************

 @(#) $RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.8.2.1 $) $Date: 2003/05/27 09:52:34 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2003/05/27 09:52:34 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.8.2.1 $) $Date: 2003/05/27 09:52:34 $"

static char const ident[] =
    "$RCSfile: sockmod.c,v $ $Name:  $($Revision: 0.8.2.1 $) $Date: 2003/05/27 09:52:34 $";

#include <sys/os7/compat.h>

#include <sys/sockmod.h>

#define SMOD_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SMOD_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SMOD_REVISION	"OpenSS7 $RCSfile$ $Name$($Revision$) $Date$"
#define SMOD_DEVICE	"SVR 3.2 STREAMS Socket Module for TPI Devices (SOCKMOD)"
#define SMOD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SMOD_LICENSE	"GPL"
#define SMOD_BANNER	SMOD_DESCRIP	"\n" \
			SMOD_COPYRIGHT	"\n" \
			SMOD_REVISION	"\n" \
			SMOD_DEVICE	"\n" \
			SMOD_CONTACT	"\n"
#define SMOD_SPLASH	SMOD_DEVICE	" - " \
			SMOD_REVISION

#ifdef LINUX
MODULE_AUTHOR(SMOD_CONTACT);
MODULE_DESCRIPTION(SMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(SMOD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SMOD_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sockmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef SMOD_MOD_NAME
#define SMOD_MOD_NAME		"sockmod"
#endif

#ifndef SMOD_MOD_ID
#define SMOD_MOD_ID		0
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */
#define MOD_ID		SMOD_MOD_ID
#define MOD_NAME	SMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SMOD_BANNER
#else
#define MOD_BANNER	SMOD_SPLASH
#endif

static struct module_info smod_minfo = {
	.mi_idnum = SMOD_MOD_ID,	/* Module ID number */
	.mi_idname = SMOD_MOD_NAME,	/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = (1 << 15),		/* Hi water mark */
	.mi_lowat = (1 << 10),		/* Lo water mark */
};

static struct module_stat smod_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat smod_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int smod_qopen(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int smod_qclose(queue_t *, int, cred_t *);

static streamscall int smod_rput(queue_t *, mblk_t *);
static streamscall int smod_wput(queue_t *, mblk_t *);

static struct qinit smod_rinit = {
	.qi_putp = smod_rput,		/* Read put (msg from below) */
	.qi_qopen = smod_qopen,		/* Each open */
	.qi_qclose = smod_qclose,	/* Last close */
	.qi_minfo = &smod_minfo,	/* Information */
	.qi_mstat = &smod_rstat,	/* Statistics */
};

static struct qinit smod_winit = {
	.qi_putp = smod_wput,		/* Write put (msg from above) */
	.qi_minfo = &smod_minfo,	/* Information */
};

static struct streamtab smod_info = {
	.st_rdinit = &smod_rinit,	/* Upper read queue */
	.st_wrinit = &smod_winit,	/* Upper write queue */
	.qi_mstat = &smod_wstat,	/* Statistics */
};

/*
 *  =========================================================================
 *
 *  Private Structure
 *
 *  =========================================================================
 */
typedef struct smod {
	queue_t *rq;			/* module read queue */
	queue_t *wq;			/* module write queue */
	queue_t *hq;			/* stream head read queue */
	ulong state;			/* module state */
	ulong oldstate;			/* module state */
	ulong flags;			/* modules flags */
	mblk_t *iocblk;			/* ioctl being processed */
	ulong qlen;			/* bind ack queue length */
	ulong cons;			/* outstanding connection indications */
#if 0
	STR_DECLARATION (struct smod);	/* stream declarations */
#endif
	smod_addr_t src;		/* src address */
	smod_size_t src_len;		/* src address length */
	smod_addr_t dst;		/* src address */
	smod_size_t dst_len;		/* dst address length */
#if 0
	bufq_t conq;			/* connection indication queue */
	uint conind;			/* number of connection indications */
#endif
} smod_t;

#define SMOD_PRIV(q) ((struct smod *)((__q)->q_ptr))

/*
 *  -------------------------------------------------------------------------
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  -------------------------------------------------------------------------
 */
STATIC kmem_cache_t *smod_priv_cachep = NULL;
STATIC int
smod_init_caches(void)
{
	if (!smod_priv_cachep &&
	    !(smod_priv_cachep = kmem_cache_create
	      ("smod_priv_cachep", sizeof(struct smod), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: ERROR: could not allocate smod_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized smod private cache\n", SMOD_MOD_NAME));
	return (0);
}
STATIC void
smod_term_caches(void)
{
	if (smod_priv_cachep) {
		if (kmem_cache_destroy(smod_priv_cachep))
			cmn_err(CE_WARN, "%s: ERROR: could not destroy smod_priv_cachep",
				__FUNCTION__);
		else
			printd(("%s: destroyed smod_priv_cachep\n", SMOD_MOD_NAME));
	}
	return;
}

/*
 *  SOCKMOD allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct smod *
smod_alloc_priv(queue_t *q, struct smod **smodp, dev_t *devp, cred_t *crp)
{
	struct smod *smod;

	if ((smod = kmem_cache_alloc(smod_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated smod private structure\n", SMOD_MOD_NAME, smod));
		bzero(smod, sizeof(*smod));
		smod->u.dev.cmajor = getmajor(*devp);
		smod->u.dev.cminor = getminor(*devp);
		smod->cred = *crp;
		lis_spin_lock_init(&smod->qlock, "smod-queue-lock");
		(smod->oq = RD(q))->q_ptr = smod_get(smod);
		(smod->iq = WR(q))->q_ptr = smod_get(smod);
		smod->o_prim = &smod_r_prim;
		smod->i_prim = &smod_w_prim;
		smod->o_wakeup = NULL;
		smod->i_wakeup = NULL;
		smod->state = 0;	/* unintialized */
		lis_spin_lock_init(&smod->lock, "smod-lock");
		if ((smod->next = *smodp))
			smod->next->prev = &smod->next;
		smod->prev = smodp;
		*smodp = smod_get(smod);
		printd(("%s: %p: linked sockmod private structure\n", SMOD_MOD_NAME));
	} else
		ptrace(("%s: ERROR: could not allocate smod private structure\n", SMOD_MOD_NAME));
	return (smod);
}
STATIC void
smod_free_priv(queue_t *q)
{
	struct smod *smod = SMOD_PRIV(q);
	int flags = 0;

	ensure(smod, return);
	lis_spin_lock_irqsave(&smod->lock, &flags);
	{
		ss7_unbufcall((str_t *) smod);
		if ((*smod->prev = smod->next))
			smod->next->prev = smod->prev;
		smod->next = NULL;
		smod->prev = &smod->next;
		smod_put(smod);
		smod->oq->q_ptr = NULL;
		flushq(smod->oq, FLUSHALL);
		smod->oq = NULL;
		smod_put(smod);
		smod->iq->q_ptr = NULL;
		flushq(smod->iq, FLUSHALL);
		smod->iq = NULL;
	}
	lis_spin_lock_irqrestore(&smod->lock, &flags);
	smod_put(smod);		/* final put */
	return;
}
STATIC struct smod *
smod_get(struct smod *smod)
{
	atomic_inc(&smod->refcnt);
	return (smod);
}
STATIC void
smod_put(struct smod *smod)
{
	if (atomic_dec_and_test(&smod->refcnt)) {
		kmem_cache_free(smod_priv_cachep);
		printd(("%s: %p: freed smod private structure\n", SMOD_MOD_NAME, smod));
	}
}

/*
 *  =========================================================================
 *
 *  PROTOCOL PRIMITIVES
 *
 *  =========================================================================
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from above
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T_CONN_REQ
 *  -----------------------------------
 */
STATIC int
t_conn_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_CONN_RES
 *  -----------------------------------
 */
STATIC int
t_conn_res(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_DISCON_REQ
 *  -----------------------------------
 */
STATIC int
t_discon_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_DATA_REQ
 *  -----------------------------------
 */
STATIC int
t_data_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_EXDATA_REQ
 *  -----------------------------------
 */
STATIC int
t_exdata_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_INFO_REQ
 *  -----------------------------------
 */
STATIC int
t_info_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_BIND_REQ
 *  -----------------------------------
 */
STATIC int
t_bind_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_UNBIND_REQ
 *  -----------------------------------
 */
STATIC int
t_unbind_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_UNITDATA_REQ
 *  -----------------------------------
 */
STATIC int
t_unitdata_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
t_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_ORDREL_REQ
 *  -----------------------------------
 */
STATIC int
t_ordrel_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_OPTDATA_REQ
 *  -----------------------------------
 */
STATIC int
t_optdata_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_ADDR_REQ
 *  -----------------------------------
 */
STATIC int
t_addr_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  T_CONN_IND
 *  -----------------------------------
 */
STATIC int
t_conn_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_CONN_CON
 *  -----------------------------------
 */
STATIC int
t_conn_con(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_DISCON_IND
 *  -----------------------------------
 */
STATIC int
t_discon_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_DATA_IND
 *  -----------------------------------
 */
STATIC int
t_data_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_EXDATA_IND
 *  -----------------------------------
 */
STATIC int
t_exdata_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_INFO_ACK
 *  -----------------------------------
 */
STATIC int
t_info_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_BIND_ACK
 *  -----------------------------------
 */
STATIC int
t_bind_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_ERROR_ACK
 *  -----------------------------------
 */
STATIC int
t_error_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_OK_ACK
 *  -----------------------------------
 */
STATIC int
t_ok_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_UNITDATA_IND
 *  -----------------------------------
 */
STATIC int
t_unitdata_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_UDERROR_IND
 *  -----------------------------------
 */
STATIC int
t_uderror_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC int
t_optmgmt_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_ORDREL_IND
 *  -----------------------------------
 */
STATIC int
t_ordrel_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_OPTDATA_IND
 *  -----------------------------------
 */
STATIC int
t_optdata_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  T_ADDR_ACK
 *  -----------------------------------
 */
STATIC int
t_addr_ack(queue_t *q, mblk_t *mp)
{
}

/*
 *  =========================================================================
 *
 *  I/O Controls
 *
 *  =========================================================================
 */
/*
 *  O_SI_GETUDATA
 *  -----------------------------------
 */
STATIC int
smod_o_si_getudata(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_SHUTDOWN
 *  -----------------------------------
 */
STATIC int
smod_si_shutdown(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_LISTEN
 *  -----------------------------------
 */
STATIC int
smod_si_listen(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_SETMYNAME
 *  -----------------------------------
 */
STATIC int
smod_si_setmyname(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_SETPEERNAME
 *  -----------------------------------
 */
STATIC int
smod_si_setpeername(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_GETINTRANSIT
 *  -----------------------------------
 */
STATIC int
smod_si_getintransit(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_TCL_LINK
 *  -----------------------------------
 */
STATIC int
smod_si_tcl_link(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_TCL_UNLINK
 *  -----------------------------------
 */
STATIC int
smod_si_tcl_unlink(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_SOCKPARAMS
 *  -----------------------------------
 */
STATIC int
smod_si_sockparams(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  SI_GETUDATA
 *  -----------------------------------
 */
STATIC int
smod_si_getudata(queue_t *q, mblk_t *mp, int *rvp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		break;
	case M_IOCDATA:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCACK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	case M_IOCNAK:
		switch (smod->ioc_state) {
		case SMOD_IOCS_NULL:
			*rvp = -1;
			return (-EINVAL);
		}
		break;
	default:
		*rvp = -1;
		return (-EINVAL);
	}
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
smod_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int err = -EOPNOTSUPP, ret = -1;

	switch (type) {
	case SMOD_IOC_MAGIC:
	{
		/* These are SOCKMOD IOCTLs */
		switch (nr) {
		case _IOC_NR(O_SI_GETUDATA):
			printd(("%s: %p: -> O_SI_GETUDATA\n", SMOD_MOD_NAME, smod));
			err = smod_o_si_getudata(q, mp, &ret);
			break;
		case _IOC_NR(SI_SHUTDOWN):
			printd(("%s: %p: -> SI_SHUTDOWN\n", SMOD_MOD_NAME, smod));
			err = smod_si_shutdown(q, mp, &ret);
			break;
		case _IOC_NR(SI_LISTEN):
			printd(("%s: %p: -> SI_LISTEN\n", SMOD_MOD_NAME, smod));
			err = smod_si_listen(q, mp, &ret);
			break;
		case _IOC_NR(SI_SETMYNAME):
			printd(("%s: %p: -> SI_SETMYNAME\n", SMOD_MOD_NAME, smod));
			err = smod_si_setmyname(q, mp, &ret);
			break;
		case _IOC_NR(SI_SETPEERNAME):
			printd(("%s: %p: -> SI_SETPEERNAME\n", SMOD_MOD_NAME, smod));
			err = smod_si_setpeername(q, mp, &ret);
			break;
		case _IOC_NR(SI_GETINTRANSIT):
			printd(("%s: %p: -> SI_GETINTRANSIT\n", SMOD_MOD_NAME, smod));
			err = smod_si_getintransit(q, mp, &ret);
			break;
		case _IOC_NR(SI_TCL_LINK):
			printd(("%s: %p: -> SI_TCL_LINK\n", SMOD_MOD_NAME, smod));
			err = smod_si_tcl_link(q, mp, &ret);
			break;
		case _IOC_NR(SI_TCL_UNLINK):
			printd(("%s: %p: -> SI_TCL_UNLINK\n", SMOD_MOD_NAME, smod));
			err = smod_si_tcl_unlink(q, mp, &ret);
			break;
		case _IOC_NR(SI_SOCKPARAMS):
			printd(("%s: %p: -> SI_SOCKPARAMS\n", SMOD_MOD_NAME, smod));
			err = smod_si_sockparams(q, mp, &ret);
			break;
		case _IOC_NR(SI_GETUDATA):
			printd(("%s: %p: -> SI_GETUDATA\n", SMOD_MOD_NAME, smod));
			err = smod_si_getudata(q, mp, &ret);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported SOCKMOD ioctl %c, %d\n", SOCK_MOD_NAME,
				smod(char) type, nr));
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", SOCK_MOD_NAME, smod,
			(char) type, nr));
		break;
	}
	if (err > 0)
		return (err);
	else if (err == 0)
		mp->b_datap->db_type = M_IOCACK;
	else
		mp->b_datap->db_type = M_IOCNAK;
	iocp->ioc_error = -err;
	iocp->ioc_rval = ret;
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCDATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
smod_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	void *arg = dp ? dp->b_rptr : NULL;
	int cmd = resp->cp_cmd, count = dp ? dp->b_wptr - dp->b_rptr : 0;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int err = -EOPNOTSUPP, ret = -1;

	if (resp->cp_rval != 0)
		goto efault;
	if (smod->ioc_state == IOC_STATE_NULL)
		goto einval;
	switch (type) {
	case SMOD_IOC_MAGIC:
	{
		/* These are SOCKMOD IOCTLs */
		switch (nr) {
		case _IOC_NR(O_SI_GETUDATA):
			printd(("%s: %p: -> O_SI_GETUDATA\n", SMOD_MOD_NAME, smod));
			err = smod_o_si_getudata(q, mp, &ret);
			break;
		case _IOC_NR(SI_SHUTDOWN):
			printd(("%s: %p: -> SI_SHUTDOWN\n", SMOD_MOD_NAME, smod));
			err = smod_si_shutdown(q, mp, &ret);
			break;
		case _IOC_NR(SI_LISTEN):
			printd(("%s: %p: -> SI_LISTEN\n", SMOD_MOD_NAME, smod));
			err = smod_si_listen(q, mp, &ret);
			break;
		case _IOC_NR(SI_SETMYNAME):
			printd(("%s: %p: -> SI_SETMYNAME\n", SMOD_MOD_NAME, smod));
			err = smod_si_setmyname(q, mp, &ret);
			break;
		case _IOC_NR(SI_SETPEERNAME):
			printd(("%s: %p: -> SI_SETPEERNAME\n", SMOD_MOD_NAME, smod));
			err = smod_si_setpeername(q, mp, &ret);
			break;
		case _IOC_NR(SI_GETINTRANSIT):
			printd(("%s: %p: -> SI_GETINTRANSIT\n", SMOD_MOD_NAME, smod));
			err = smod_si_getintransit(q, mp, &ret);
			break;
		case _IOC_NR(SI_TCL_LINK):
			printd(("%s: %p: -> SI_TCL_LINK\n", SMOD_MOD_NAME, smod));
			err = smod_si_tcl_link(q, mp, &ret);
			break;
		case _IOC_NR(SI_TCL_UNLINK):
			printd(("%s: %p: -> SI_TCL_UNLINK\n", SMOD_MOD_NAME, smod));
			err = smod_si_tcl_unlink(q, mp, &ret);
			break;
		case _IOC_NR(SI_SOCKPARAMS):
			printd(("%s: %p: -> SI_SOCKPARAMS\n", SMOD_MOD_NAME, smod));
			err = smod_si_sockparams(q, mp, &ret);
			break;
		case _IOC_NR(SI_GETUDATA):
			printd(("%s: %p: -> SI_GETUDATA\n", SMOD_MOD_NAME, smod));
			err = smod_si_getudata(q, mp, &ret);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported SOCKMOD ioctl %c, %d\n", SOCK_MOD_NAME,
				smod(char) type, nr));
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", SOCK_MOD_NAME, smod,
			(char) type, nr));
		break;
	}
      done:
	if (err > 0)
		return (err);
	else if (err == 0)
		mp->b_datap->db_type = M_IOCACK;
	else
		mp->b_datap->db_type = M_IOCNAK;
	iocp->ioc_error = -err;
	iocp->ioc_rval = ret;
	qreply(q, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	ret = -1;
	goto done;
      einval:
	err = -EINVAL;
	ret = -1;
	goto done;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCACK, M_IOCNAK Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
smod_r_iocack(queue_t *q, mblk_t *mp)
{
	struct smod *smod = SMOD_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	mblk_t *bp = mp->b_cont;
	void *arg = bp ? bp->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int err = QR_PASSALONG, ret = -1;

	if (iocp->ioc_id != ss->ioc_id)
		goto passalong;
	switch (type) {
	case SMOD_IOC_MAGIC:
	{
		/* These are SOCKMOD IOCTLs */
		switch (nr) {
		case _IOC_NR(O_SI_GETUDATA):
			printd(("%s: %p: -> O_SI_GETUDATA\n", SMOD_MOD_NAME, smod));
			err = smod_o_si_getudata(q, mp, &ret);
			break;
		case _IOC_NR(SI_SHUTDOWN):
			printd(("%s: %p: -> SI_SHUTDOWN\n", SMOD_MOD_NAME, smod));
			err = smod_si_shutdown(q, mp, &ret);
			break;
		case _IOC_NR(SI_LISTEN):
			printd(("%s: %p: -> SI_LISTEN\n", SMOD_MOD_NAME, smod));
			err = smod_si_listen(q, mp, &ret);
			break;
		case _IOC_NR(SI_SETMYNAME):
			printd(("%s: %p: -> SI_SETMYNAME\n", SMOD_MOD_NAME, smod));
			err = smod_si_setmyname(q, mp, &ret);
			break;
		case _IOC_NR(SI_SETPEERNAME):
			printd(("%s: %p: -> SI_SETPEERNAME\n", SMOD_MOD_NAME, smod));
			err = smod_si_setpeername(q, mp, &ret);
			break;
		case _IOC_NR(SI_GETINTRANSIT):
			printd(("%s: %p: -> SI_GETINTRANSIT\n", SMOD_MOD_NAME, smod));
			err = smod_si_getintransit(q, mp, &ret);
			break;
		case _IOC_NR(SI_TCL_LINK):
			printd(("%s: %p: -> SI_TCL_LINK\n", SMOD_MOD_NAME, smod));
			err = smod_si_tcl_link(q, mp, &ret);
			break;
		case _IOC_NR(SI_TCL_UNLINK):
			printd(("%s: %p: -> SI_TCL_UNLINK\n", SMOD_MOD_NAME, smod));
			err = smod_si_tcl_unlink(q, mp, &ret);
			break;
		case _IOC_NR(SI_SOCKPARAMS):
			printd(("%s: %p: -> SI_SOCKPARAMS\n", SMOD_MOD_NAME, smod));
			err = smod_si_sockparams(q, mp, &ret);
			break;
		case _IOC_NR(SI_GETUDATA):
			printd(("%s: %p: -> SI_GETUDATA\n", SMOD_MOD_NAME, smod));
			err = smod_si_getudata(q, mp, &ret);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported SOCKMOD ioctl %c, %d\n", SOCK_MOD_NAME,
				smod(char) type, nr));
			break;
		}
		break;
	}
	}
      done:
	if (err > 0)
		return (err);
	else if (err == 0)
		mp->b_datap->db_type = M_IOCACK;
	else
		mp->b_datap->db_type = M_IOCNAK;
	iocp->ioc_error = -err;
	iocp->ioc_rval = ret;
	putnext(q, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	ret = -1;
	goto done;
      passalong:
	err = QR_PASSALONG;
	ret = 0;
	goto done;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Primitives from SOCKMOD user to SOCKMOD
 *  -----------------------------------------------------------
 */
STATIC int
smod_w_proto(queue_t *q, mblk_t *mp)
{
	struct smod *smod = SMOD_PRIV(q);
	int rtn;
	ulong prim;

	(void) smod;
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case T_CONN_REQ:
		printd(("%s: %p: -> T_CONN_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_conn_req(q, mp);
		break;
	case T_CONN_RES:
		printd(("%s: %p: -> T_CONN_RES\n", SMOD_MOD_NAME, smod));
		rtn = t_conn_res(q, mp);
		break;
	case T_DISCON_REQ:
		printd(("%s: %p: -> T_DISCON_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_discon_req(q, mp);
		break;
	case T_DATA_REQ:
		printd(("%s: %p: -> T_DATA_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_data_req(q, mp);
		break;
	case T_EXDATA_REQ:
		printd(("%s: %p: -> T_EXDATA_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_exdata_req(q, mp);
		break;
	case T_INFO_REQ:
		printd(("%s: %p: -> T_INFO_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_info_req(q, mp);
		break;
	case T_BIND_REQ:
		printd(("%s: %p: -> T_BIND_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_bind_req(q, mp);
		break;
	case T_UNBIND_REQ:
		printd(("%s: %p: -> T_UNBIND_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_unbind_req(q, mp);
		break;
	case T_UNITDATA_REQ:
		printd(("%s: %p: -> T_UNITDATA_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_unitdata_req(q, mp);
		break;
	case T_OPTMGMT_REQ:
		printd(("%s: %p: -> T_OPTMGMT_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_optmgmt_req(q, mp);
		break;
	case T_ORDREL_REQ:
		printd(("%s: %p: -> T_ORDREL_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_ordrel_req(q, mp);
		break;
	case T_OPTDATA_REQ:
		printd(("%s: %p: -> T_OPTDATA_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_optdata_req(q, mp);
		break;
	case T_ADDR_REQ:
		printd(("%s: %p: -> T_ADDR_REQ\n", SMOD_MOD_NAME, smod));
		rtn = t_addr_req(q, mp);
		break;
	default:
		printd(("%s: %p: -> T_????\n", SMOD_MOD_NAME, smod));
		rtn = QR_PASSALONG;
		break;
	}
	return (rtn);
}

/*
 *  Primitives from TPI provider to SOCKMOD
 *  -----------------------------------------------------------
 */
STATIC int
smod_r_proto(queue_t *q, mblk_t *mp)
{
	struct smod *smod = SMOD_PRIV(q);
	int rtn;
	ulong prim;

	(void) smod;
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case T_CONN_IND:
		printd(("%s: %p: T_CONN_IND <-\n", SMOD_MOD_NAME, smod));
		rtn = t_conn_ind(q, mp);
		break;
	case T_CONN_CON:
		printd(("%s: %p: T_CONN_CON <-\n", SMOD_MOD_NAME, smod));
		rtn = t_conn_con(q, mp);
		break;
	case T_DISCON_IND:
		printd(("%s: %p: T_DISCON_IND <-\n", SMOD_MOD_NAME, smod));
		rtn = t_discon_ind(q, mp);
		break;
	case T_DATA_IND:
		printd(("%s: %p: T_DATA_IND <-\n", SMOD_MOD_NAME, smod));
		rtn = t_data_ind(q, mp);
		break;
	case T_EXDATA_IND:
		printd(("%s: %p: T_EXDATA_IND <-\n", SMOD_MOD_NAME, smod));
		rtn = t_exdata_ind(q, mp);
		break;
	case T_INFO_ACK:
		printd(("%s: %p: T_INFO_ACK <-\n", SMOD_MOD_NAME, smod));
		rtn = t_info_ack(q, mp);
		break;
	case T_BIND_ACK:
		printd(("%s: %p: T_BIND_ACK <-\n", SMOD_MOD_NAME, smod));
		rtn = t_bind_ack(q, mp);
		break;
	case T_ERROR_ACK:
		printd(("%s: %p: T_ERROR_ACK <-\n", SMOD_MOD_NAME, smod));
		rtn = t_error_ack(q, mp);
		break;
	case T_OK_ACK:
		printd(("%s: %p: T_OK_ACK <-\n", SMOD_MOD_NAME, smod));
		rtn = t_ok_ack(q, mp);
		break;
	case T_UNITDATA_IND:
		printd(("%s: %p: T_UNITDATA_IND <-\n", SMOD_MOD_NAME, smod));
		rtn = t_unitdata_ind(q, mp);
		break;
	case T_UDERROR_IND:
		printd(("%s: %p: T_UDERROR_IND <-\n", SMOD_MOD_NAME, smod));
		rtn = t_uderror_ind(q, mp);
		break;
	case T_OPTMGMT_ACK:
		printd(("%s: %p: T_OPTMGMT_ACK <-\n", SMOD_MOD_NAME, smod));
		rtn = t_optmgmt_ack(q, mp);
		break;
	case T_ORDREL_IND:
		printd(("%s: %p: T_ORDREL_IND <-\n", SMOD_MOD_NAME, smod));
		rtn = t_ordrel_ind(q, mp);
		break;
	case T_OPTDATA_IND:
		printd(("%s: %p: T_OPTDATA_IND <-\n", SMOD_MOD_NAME, smod));
		rtn = t_optdata_ind(q, mp);
		break;
	case T_ADDR_ACK:
		printd(("%s: %p: T_ADDR_ACK <-\n", SMOD_MOD_NAME, smod));
		rtn = t_addr_ack(q, mp);
		break;
	default:
		printd(("%s: %p: T_???? <-\n", SMOD_MOD_NAME, smod));
		rtn = QR_PASSALONG;
		break;
	}
	return (rtn);
}

/*
 *  M_DATA Handling
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
smod_w_data(queue_t *q, mblk_t *mp)
{
	/* data from above */
	printd(("%s: %p: -> M_DATA\n", SMOD_MOD_NAME, SMOD_PRIV(q)));
	return (QR_PASSFLOW);
}
STATIC INLINE int
smod_r_data(queue_t *q, mblk_t *mp)
{
	/* data from below */
	printd(("%s: %p: M_DATA <-\n", SMOD_MOD_NAME, SMOD_PRIV(q)));
	return (QR_PASSFLOW);
}

/*
 *  M_FLUSH Handling
 *  -------------------------------------------------------------------------
 */

/*
 *  M_ERROR, M_HANGUP Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
smod_r_error(queue_t *q, mblk_t *mp)
{
	printd(("%s: %p: M_ERROR <-\n", SMOD_MOD_NAME, SMOD_PRIV(q)));
	smod->state = SMODS_UNUSABLE;
	return (QR_PASSALONG);
}
STATIC int
smod_r_hangup(queue_t *q, mblk_t *mp)
{
	printd(("%s: %p: M_HANGUP <-\n", SMOD_MOD_NAME, SMOD_PRIV(q)));
	smod->state = SMODS_UNUSABLE;
	return (QR_PASSALONG);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
smod_w_prim(queue_t *q, mblk_t *mp)
{
	ulong type;

	/* Fast Path */
	if ((type = mp->b_datap->db_type) == M_DATA)
		return smod_w_data(q, mp);
	switch (type) {
	case M_DATA:
		return smod_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return smod_w_proto(q, mp);
	case M_FLUSH:
		return smod_w_flush(q, mp);
	case M_IOCTL:
		return smod_w_ioctl(q, mp);
	case M_IOCDATA:
		return smod_w_iocdata(q, mp);
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
smod_r_prim(queue_t *q, mblk_t *mp)
{
	ulong type;

	/* Fast Path */
	if ((type = mp->b_datap->db_type) == M_DATA)
		return smod_r_data(q, mp);
	switch (type) {
	case M_DATA:
		return smod_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return smod_r_proto(q, mp);
	case M_FLUSH:
		return smod_r_flush(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return smod_r_iocack(q, mp);
	case M_ERROR:
		return smod_r_error(q, mp);
	case M_HANGUP:
		return smod_r_hangup(q, mp);
	}
	return (QR_PASSALONG);
}

static noinline streams_fastcall __unlikely int
smod_ioctl_slow(queue_t *q, mblk_t *mp)
{
}

/**
 *  smod_wput_slow - slow write put procedure
 *  @q: the write queue
 *  @mp: the message to put
 */
static noinline streams_fastcall __unlikely int
smod_wput_slow(queue_t *q, mblk_t *mp)
{
	struct smod *priv = q->q_ptr;

#if defined LIS
	/* LiS has this nasty bug where it breaks a STREAMS-based pipe in two _before_ popping
	   modules and, of course, does not properly suppress queue procedures while closing.  We
	   can check this as many times as we would like on SMP and the q->q_next pointer could be
	   invalidate immediately after we check it.  Never use LiS. */
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	switch (mp->b_datap->db_type) {
		union T_primitives *p;
		struct iocblk *ioc;
		mblk_t *dp;
		int err;

	case M_IOCTL:
		/* Most of the ioctls prvided here are to achieve atomic and thread-safe operations 
		   on the stream for use by the Sockets library.  Each ioctl takes a TPI message in 
		   the buffer and results in sending the TPI message downstream.  We strip off and
		   keep the io control block for latter response correlation.  We also track the
		   state of the stream and the number of outstanding connection indications. */
		ioc = (typeof(ioc)) mp->b_rptr;
		err = -EFAULT;
		if (!(dp = unlinkb(mp)))
			goto error;
		dp->b_datap->db_type = (ioc->ioc_cmd == SI_GETUDATA) ? M_PCPROTO : M_PROTO;
		p = (typeof(p)) dp->b_rptr;
		err = -EINVAL;
#if defined LIS
		/* LiS does not implement the SVR 4 Stream head correctly.  It is the
		   responsibility of the Stream head, according to the SVR 4.2 SPG, to convert
		   legacy character device, file, terminal and socket intput-output controls to
		   I_STR input-output controls.  When the Stream head works correctly (as it does
		   for Linux Fast-STREAMS) we will never receive a TRANPARENT ioctl here. */
		if (ioc->ioc_count == TRANSPARENT)
			return smod_ioctl_slow(q, mp);
#else				/* defined LIS */
		if (ioc->ioc_count == TRANSPARENT)
			goto error;
#endif				/* defined LIS */
		switch (ioc->ioc_cmd) {
		case O_SI_GETUDATA:
			err = TNOTSUPPORT;
			goto error;
		case SI_GETUDATA:
			if (p->type = T_INFO_REQ && ioc->ioc_count >= sizeof(p->info_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				break;
			}
			goto error;
		case SI_SETMYNAME:
		case SI_LISTEN:
#if defined SI_BIND
		case SI_BIND:
#endif				/* defined SI_BIND */
			if (p->type == T_BIND_REQ && ioc->ioc_count >= sizeof(p->bind_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_BREQ;
				break;
			}
			if (p->type == T_UNBIND_REQ && ioc->ioc_count >= sizeof(p->ok_ack)) {
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_UREQ;
				break;
			}
#if defined SI_ACCEPT
			break;
		case SI_ACCEPT:
#endif				/* defined SI_ACCEPT */
			if (p->type == T_CONN_RES && ioc->ioc_count >= sizeof(p->conn_res)) {
				int doff = sizeof(p->conn_res);

				if (p->conn_res.OPT_length
				    && doff < p->conn_res.OPT_offset + p->conn_res.OPT_length)
					doff = p->conn_res.OPT_offset + p->conn_res.OPT_length;
				if ((err = split_buffer(dp, doff)) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_CRES;
				break;
			}
			if (p->type == T_DISCON_REQ && ioc->ioc_count >= sizeof(p->discon_req)) {
				if ((err = split_buffer(dp, sizeof(p->discon_req))) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_DREQ7;
				break;
			}
			goto error;
		case SI_SETPEERNAME:
#if defined SI_CONNECT
		case SI_CONNECT:
#endif				/* defined SI_CONNECT */
			if (p->type == T_CONN_REQ && ioc->ioc_count >= sizeof(p->conn_req)) {
				int doff = sizeof(p->conn_req);

				if (p->conn_req.OPT_length
				    && doff < p->conn_req.OPT_offset + p->conn_req.OPT_length)
					doff = p->conn_req.OPT_offset + p->conn_req.OPT_length;
				if (p->conn_req.DEST_length
				    && doff < p->conn_req.DEST_offset + p->conn_req.DEST_length)
					doff = p->conn_req.DEST_offset + p->conn_req.DEST_length;
				if ((err = split_buffer(dp, doff)) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_CREQ;
				break;
			}
			if (p->type == T_DISCON_REQ && ioc->ioc_count >= sizeof(p->discon_req)) {
				if ((err = split_buffer(dp, sizeof(p->discon_req))) < 0)
					goto error;
				dp->b_datap->db_type = M_PROTO;
				priv->oldstate = priv->state;
				priv->state = TS_WACK_DREQ7;
				break;
			}
			goto error;
		case O_SI_GETUDATA:
		case SI_SHUTDOWN:
		case SI_GETINTRANSIT:
		case SI_SI_TCL_LINK:
		case SI_SI_TCL_UNLINK:
		case SI_SOCKPARAMS:
		case SI_GETUDATA:
		case SIOCHIWAT:
		case SIOGHIWAT:
		case SIOCLOWAT:
		case SIOGLOWAT:
		case SIOCATMARK:
		case SIOCSPGRP:
		case SIOCGPGRP:
		case SIOCPROTO:
		case SIOCGETNAME:
		case SIOCGETPEER:
		case SIOCXPROTO:
		case SIOCSOCKSYS:
		{
			/* First step is to collect the command and the file descriptor, then the
			   arguments once the command is known. */
			mp->b_datap->db_type = M_COPYIN;
			goto error;
		}
		case FIOCGPGRP:
		case FIOCSPGRP:
		case FIONREAD:
		case FIONBIO:
		case TIOCINQ:
		case TIOOUTQ:
		default:
			break;
		}
		break;
	case M_IOCDATA:
		switch (cp->cp_cmd) {
		case SIOCSOCKSYS:
			if (cp->cp_priv == NULL) {
				/* First copyin response. */
			} else {
				/* Subsequent copyin response. */
				switch ((int) (long) (cp->cp_priv->b_prev)) {
				case 1:	/* second copyin response */
					/* Now we have the argument list. */
					switch (*(cp->cp_priv->b_datap->db_base)) {
					case SO_SOCKET:
					case SO_ACCEPT:
					case SO_BIND:
					case SO_CONNECT:
					case SO_GETPEERNAME:
					case SO_GETSOCKNAME:
					case SO_LISTEN:
					case SO_RECV:
					case SO_RECVFROM:
					case SO_SEND:
					case SO_SENDTO:
					case SO_SETSOCKOPT:
					case SO_SHUTDOWN:
					case SO_GETIPDOMAIN:
					case SO_SETIPDOMAIN:
					case SO_SETREUID:
					case SO_SETREGID:
					case SO_GETTIME:
					case SO_SETTIME:
					case SO_GETITIMER:
					case SO_SETITIMER:
					case SO_SELECT:
					case SO_ADJTIME:
					case SO_RECVMSG:
					case SO_SENDMSG:
					case SO_SOCKPAIR:
					default:
					}
					goto error;
				case 2:	/* third copyin/copyout response */
				case 3:	/* forth copyin/copyout response */
				case 4:	/* fifth copyin/copyout response */
				}
			}
		}
		break;
	default:
		/* Unrecongnized STREAMS messages are simply passed along. Because we have no
		   service procedure, flow control is never performed by the module when passing
		   messages along. */
		break;
	}
	putnext(q, mp);
	return (0);
}

static streamscall __hot_out int
smod_wput(queue_t *q, mblk_t *mp)
{
	union T_primitives *p;

#if defined LIS
	/* LiS has this nasty bug where it breaks a STREAMS-based pipe in two _before_ popping
	   modules and, of course, does not properly suppress queue procedures while closing.  We
	   can check this as many times as we would like on SMP and the q->q_next pointer could be
	   invalidate immediately after we check it.  Never use LiS. */
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		freemsg(mp);
		return (0);
	}
#endif				/* defined LIS */
	/* fast path for data */
	if (unlikely(mp->b_datap->db_type != M_PROTO))
		goto go_slow;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->type)))
		goto go_slow;
	p = (typeof(p)) mp->b_rptr;
	/* conveniently none of these are > 31 */
	if (unlikely(!((1 << p->type)
		       & ((1 << T_UNITDATA_REQ) | (1 << T_EXDATA_REQ) | (1 << T_DATA_REQ)))))
		goto go_slow;
	putnext(q, mp);
	return (0);
      go_slow:
	return smod_wput_slow(q, mp);
}

#define SOCKMOD_HANGUP 01
#define SOCKMOD_EPROTO 02

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  LiS does not offer us a way to wait for an allocation.  Solaris, OSF and
 *  Linux-Fast STREAMS do.
 */
#if !defined BPRI_WAITOK
#   if defined BPRI_FT
#	define BPRI_WAITOK BPRI_FT
#   else
#	define BPRI_WAITOK BPRI_HI
#   endif
#endif

static __unlikely void
smod_pop(queue_t *q)
{
	struct smod *priv = (typeof(priv)) q->q_ptr;
	mblk_t *mp;

	switch (priv->state) {
	case TS_WREQ_ORDREL:
		if (!(priv->flags & SOCKMOD_EPROTO)) {
			if ((mp = allocb(sizeof(struct T_ordrel_req), BPRI_WAITOK))) {
				struct T_ordrel_req *prim = (typeof(prim)) mp->b_wptr;

				mp->b_wptr = (unsigned char *) (prim + 1);
				mp->b_datap->db_type = M_PROTO;
				prim->PRIM_type = T_ORDREL_REQ;
				qreply(q, mp);
			}
		}
		/* fall through */
	case TS_DATA_XFER:
		if ((mp = allocb(sizeof(struct T_discon_req), BPRI_WAITOK))) {
			struct T_discon_req *prim = (typeof(prim)) mp->b_wptr;

			mp->b_wptr = (unsigned char *) (prim + 1);
			mp->b_datap->db_type = M_PROTO;
			prim->PRIM_type = T_DISCON_REQ;
			prim->SEQ_number = 0;
			qreply(q, mp);
		}
		break;
	case TS_IDLE:
	default:
		break;
	}
	if ((priv->flags & SOCKMOD_EPROTO)) {
		if ((mp = allocb(2, BPRI_WAITOK))) {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = 0;
			*(mp->b_wptr)++ = 0;
			qreply(q, mp);
		}
#if defined M_ERROR_UNDOES_M_HANGUP
		priv->flags &= ~(SOCKMOD_EPROTO | SOCKMOD_HANGUP);
#else
		priv->flags &= ~(SOCKMOD_EPROTO);
#endif				/* defined M_ERROR_UNDOES_M_HANGUP */
	}
#if defined M_UNHANGUP
	if ((priv->flags & SOCKMOD_HANGUP)) {
		if ((mp = allocb(0, BPRI_WAITOK))) {
			mp->b_datap->db_type = M_UNHANGUP;
			qreply(q, mp);
		}
		priv->flags &= ~SOCKMOD_HANGUP;
	}
#endif				/* defined M_UNHANGUP */
}

/* Approaches: how about creating a new socket in the sockfs(5) for hacked address family AF_STREAMS
 * and then reset the address famliy in the socket after it is created?   Maybe even a slimmer
 * version of that which creates only the sockfs inode and then uses the stream head instead of a
 * struct sock? */

static int sock_tpi_release(struct socket *);
static int sock_tpi_bind(struct socket *, struct sockaddr *, int);
static int sock_tpi_connect(struct socket *, struct sockaddr *, int, int);
static int sock_tpi_socketpair(struct socket *, struct socket *);
static int sock_tpi_accept(struct socket *, struct socket *, int);
static int sock_tpi_getname(struct socket *, struct sockaddr *, int *, int);
static unsigned int sock_tpi_poll(struct file *, struct socket *, struct poll_table_struct *);
static int sock_tpi_ioctl(struct socket *, unsigned int, unsigned long);

#ifdef HAVE_KMEMB_PROTO_OPS_COMPAT_IOCTL
static int sock_tpi_compat_ioctl(struct socket *, unsigned int, unsigned long);
#endif				/* HAVE_KMEMB_PROTO_OPS_COMPAT_IOCTL */

static int sock_tpi_listen(struct socket *, int);
static int sock_tpi_shutdown(struct socket *, int);
static int sock_tpi_setsockopt(struct socket *, int, int, char __user *, int);

#ifdef HAVE_KMEMB_PROTO_OPS_COMPAT_SETSOCKOPT
static int sock_tpi_compat_setsockopt(struct socket *, int, int, char __user *, int);
#endif				/* HAVE_KMEMB_PROTO_OPS_COMPAT_SETSOCKOPT */

static int sock_tpi_getsockopt(struct socket *, int, int, char __user *, int __user *);

#ifdef HAVE_KMEMB_PROTO_OPS_COMPAT_GETSOCKOPT
static int sock_tpi_compat_getsockopt(struct socket *, int, int, char __user *, int __user *);
#endif				/* HAVE_KMEMB_PROTO_OPS_COMPAT_GETSOCKOPT */

#ifndef HAVE_KTYPE_STRUCT_KIOCB
/* These are 2.6.17 socket operations: */
static int sock_tpi_sendmsg(struct kiocb *iocb, struct socket *, struct msghdr *, size_t);
static int sock_tpi_recvmsg(struct kiocb *iocb, struct scoket *, struct msghdr *, int);
#else
/* These are 2.4.20 socket operations: */
static int sock_tpi_sendmsg(struct socket *, struct msghdr *, int, struct scm_cookie *);
static int sock_tpi_recvmsg(struct scoket *, struct msghdr *, int, int, struct scm_cookie *);
#endif

static int sock_tpi_mmap(struct file *, struct socket *, struct vm_area_struct *);
static int sock_tpi_sendpage(struct socket *, struct page *, int, size_t, int);



struct proto_ops tpi_socket_ops = {
	.family = 0,			/* huh? what a bad place to put this */
#ifdef HAVE_KMEMB_STRUCT_PROTO_OPS_OWNER
	.owner = THIS_MODULE,
#endif					/* HAVE_KMEMB_STRUCT_PROTO_OPS_OWNER */
	.release = &sock_tpi_release,
	.bind = &sock_tpi_bind,
	.connect = &sock_tpi_connect,
	.socketpair = &sock_tpi_socketpair,
	.accept = &sock_tpi_accept,
	.getname = &sock_tpi_getname,
	.poll = &sock_tpi_poll,
	.ioctl = &sock_tpi_ioctl,
#ifdef HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_IOCTL
	.compat_ioctl = &sock_tpi_compat_ioctl,
#endif					/* HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_IOCTL */
	.listen = &sock_tpi_listen,
	.shutdown = &sock_tpi_shutdown,
	.setsockopt = &sock_tpi_setsockopt,
	.getsockopt = &sock_tpi_getsockopt,
#ifdef HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_GETSOCKOPT
	.compat_setsockopt = &sock_tpi_compat_setsockopt,
#endif					/* HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_GETSOCKOPT */
#ifdef HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_SETSOCKOPT
	.compat_getsockopt = &sock_tpi_compat_getsockopt,
#endif					/* HAVE_KMEMB_STRUCT_PROTO_OPS_COMPAT_SETSOCKOPT */
	.sendmsg = &sock_tpi_sendmsg,
	.recvmsg = &sock_tpi_recvmsg,
	.mmap = &sock_tpi_mmap,
	.sendpage = &sock_tpi_sendpage,
};

/**
 * smod_qopen - open the sockmod module
 * @q: read queue
 * @devp: device to open
 * @flag: flags to open call
 * @sflag: should be just MODOPEN
 * @crp: pointer to opening process' credentials
 *
 * It is possible under Linux-Fast STREAMS that we can just transform the Stream into a socket.
 * This can be accomplished most easily by setting the inode to look like a socket (place a struct
 * socket inside of it and set i_sock) and implement the socket.ops as above.
 */
STATIC streamscall int
smod_qopen(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err = 0;

	if (q->q_ptr != NULL)
		goto quit;	/* already open */
	err = ENXIO;
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		goto quit;	/* can't be opened as driver */
	err = ENOMEM;
	if (!(smod_alloc_priv(q)))
		goto quit;
	/* generate immediate information request */
	qprocson(q);
	if ((err = t_info_req(q)) < 0) {
		smod_free_priv(q);
		return (-err);
	}
	return (0);
      quit:
	return (err);
}

STATIC streamscall int
smod_qclose(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", MOD_NAME, __FUNCTION__);
		goto quit;
	}
	/* LiS has this nasty bug where it breaks a STREAMS-based pipe in two _before_ popping
	   modules and, of course, does not properly suppress queue procedures while closing.  We
	   can check this as many times as we would like on SMP and the q->q_next pointer could be
	   invalidate immediately after we check it.  Never use LiS. */
	if (q->q_next == NULL || OTHERQ(q)->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			MOD_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	smod_pop(q);
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	smod_free_priv(q);
	goto quit;
      quit:
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */
unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for SOCKMOD module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodw sockmod_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sockmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC __unlikely int
smod_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&sockmod_fmod)) < 0)
		return (err);
	return (0);
}
STATIC __unlikely int
smod_unregister_strmod(void)
{
	int err;

	if ((err = unregister_strmod(&sockmod_fmod)) < 0)
		return (err);
	return (0);
}

#if defined WITH_32BIT_CONVERSION
struct sockmod_trans {
	unsigned int cmd;
	void *opaque;
};

STATIC sockmod_trans sockmod_trans_map[] = {
	{.cmd = O_SI_GETUDATA,}
	, {.cmd = SI_SHUTDOWN,}
	, {.cmd = SI_LISTEN,}
	, {.cmd = SI_SETMYNAME,}
	, {.cmd = SI_SETPEERNAME,}
	, {.cmd = SI_GETINTRANSIT,}
	, {.cmd = SI_TCL_LINK,}
	, {.cmd = SI_TCL_UNLINK,}
	, {.cmd = SI_SOCKPARAMS,}
	, {.cmd = SI_GETUDATA,}
	, {.cmd = SIOCHIWAT,}
	, {.cmd = SIOGHIWAT,}
	, {.cmd = SIOCLOWAT,}
	, {.cmd = SIOGLOWAT,}
	, {.cmd = SIOCATMARK,}
	, {.cmd = SIOCGPGRP,}
	, {.cmd = SIOCSPGRP,}
	, {.cmd = SIOCPROTO,}
	, {.cmd = SIOCGETNAME,}
	, {.cmd = SIOCGETPEER,}
	, {.cmd = SIOCXPROTO,}
	, {.cmd = SIOCSOCKSYS,}
	, {.cmd = FIOCGPGRP,}
	, {.cmd = FIOCSPGRP,}
	, {.cmd = FIONREAD,}
	, {.cmd = FIONBIO,}
	, {.cmd = TIOCINQ,}
	, {.cmd = TIOOUTQ,}
	, {.cmd = 0,}
};

STATIC __unlikely void
smod_ioctl32_unregister(void)
{
	struct sockmod_trans *t;

	for (t = sockmod_trans_map; t->cmd != 0; t++) {
		streams_unregister_ioctl32(t->opaque);
		t->opaque = NULL;
	}
	return;
}

STATIC __unlikely int
smod_ioctl32_register(void)
{
	struct sockmod_trans *t;

	for (t = sockmod_trans_map; t->cmd != 0; t++) {
		if ((t->opaque = streams_register_ioctl32(t->cmd)) == NULL) {
			smod_ioctl32_unregister();
			return (-ENOMEM);
		}
	}
	return (0);
}
#endif				/* defined WITH_32BIT_CONVERSION */

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC __unlikely int
smod_register_strmod(void)
{
	int err;

	if ((err = lis_register_strmod(&sockmodinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC __unlikely int
smod_unregister_strmod(void)
{
	int err;

	if ((err = lis_unregister_strmod(&sockmodinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sockmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = smod_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
#if defined WITH_32BIT_CONVERSION
	if ((err = smod_ioctl32_register())) {
		cmn_err(CE_WARN, "%s: could not register 32bit ioctls, err = %d", MOD_NAME, err);
		smod_term_caches();
		return (err);
	}
#endif				/* defined WITH_32BIT_CONVERSION */
	if ((err = smod_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
#if defined WITH_32BIT_CONVERSION
		smod_ioctl32_unregister();
#endif				/* defined WITH_32BIT_CONVERSION */
		smod_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODUL_STATIC void __exit
sockmodterminate(void)
{
	int err;

	if ((err = smod_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
#if defined WITH_32BIT_CONVERSION
	smod_ioctl32_unregister();
#endif				/* defined WITH_32BIT_CONVERSION */
	if ((err = smod_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  --------------------------------------------------------------------------
 */
module_init(sockmodinit);
module_exit(sockmodterminate);

#endif				/* LINUX */
