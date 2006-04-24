/*****************************************************************************

 @(#) $RCSfile: sdlm.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/04/24 05:01:01 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2006/04/24 05:01:01 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sdlm.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/04/24 05:01:01 $"

static char const ident[] =
    "$RCSfile: sdlm.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/04/24 05:01:01 $";

/*
 *  A Signalling Data Link Multiplexor for the OpenSS7 SS7 Stack.
 *
 *  This mux links SDL streams on the bottom of the multiplexor and presents
 *  SDL streams on the top of the multiplexor.  This permits independence
 *  between the streams on the top side of the multiplexor and streams on the
 *  bottom side of the multiplexor.  A binding operation is performed to bind
 *  an upper stream to a lower stream.
 */
#include <sys/os7/compat.h>

#include <sys/dlpi.h>
#include <sys/dlpi_mtp2.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/devi.h>
#include <ss7/devi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

#define SDLM_DESCRIP	"SS7/SDL: (Signalling Data Link) MULTIPLEXING STREAMS DRIVER." "\n" \
			"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SDLM_REVISION	"OpenSS7 $RCSfile: sdlm.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2006/04/24 05:01:01 $"
#define SDLM_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corp.  All Rights Reserved."
#define SDLM_DEVICE	"Supports OpenSS7 SDL Drivers."
#define SDLM_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDLM_LICENSE	"GPL"
#define SDLM_BANNER	SDLM_DESCRIP	"\n" \
			SDLM_REVISION	"\n" \
			SDLM_COPYRIGHT	"\n" \
			SDLM_DEVICE	"\n" \
			SDLM_CONTACT	"\n"
#define SDLM_SPLASH	SDLM_DESCRIP	"\n" \
			SDLM_REVISION

#ifdef LINUX
MODULE_AUTHOR(SDLM_CONTACT);
MODULE_DESCRIPTION(SDLM_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDLM_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDLM_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdlm");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SDLM_DRV_ID		CONFIG_STREAMS_SDLM_MODID
#define SDLM_DRV_NAME		CONFIG_STREAMS_SDLM_NAME
#define SDLM_CMAJORS		CONFIG_STREAMS_SDLM_NMAJORS
#define SDLM_CMAJOR_0		CONFIG_STREAMS_SDLM_MAJOR
#define SDLM_CMAJOR_1		CONFIG_STREAMS_SDLM_MAJOR_1
#define SDLM_UNITS		CONFIG_STREAMS_SDLM_NMINORS
#endif				/* LFS */

#define DL_NMAJOR	(SDLM_CMAJORS - 1)
#define LM_NMAJOR	1

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		SDLM_DRV_ID
#define DRV_NAME	SDLM_DRV_NAME
#define CMAJORS		SDLM_CMAJORS
#define CMAJOR_0	SDLM_CMAJOR_0
#define CMAJOR_1	SDLM_CMAJOR_1
#define UNITS		SDLM_UNITS
#ifdef MODULE
#define DRV_BANNER	SDLM_BANNER
#else				/* MODULE */
#define DRV_BANNER	SDLM_SPLASH
#endif				/* MODULE */

STATIC struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC int streamscall sdlm_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int streamscall sdlm_close(queue_t *, int, cred_t *);

STATIC struct module_stat dl_rmstat = {
	.ms_pcnt = 0,			/* calls to qi_putp() */
	.ms_scnt = 0,			/* calls to qi_srvp() */
	.ms_ocnt = 0,			/* calls to qi_qopen() */
	.ms_ccnt = 0,			/* calls to qi_qclose() */
	.ms_acnt = 0,			/* calls to qi_qadmin() */
	.ms_xptr = NULL,		/* module private stats */
	.ms_xsize = 0,			/* length of private stats */
	.ms_flags = 0,			/* bool stats -- for future use */
};

STATIC struct module_stat dl_wmstat = {
	.ms_pcnt = 0,			/* calls to qi_putp() */
	.ms_scnt = 0,			/* calls to qi_srvp() */
	.ms_ocnt = 0,			/* calls to qi_qopen() */
	.ms_ccnt = 0,			/* calls to qi_qclose() */
	.ms_acnt = 0,			/* calls to qi_qadmin() */
	.ms_xptr = NULL,		/* module private stats */
	.ms_xsize = 0,			/* length of private stats */
	.ms_flags = 0,			/* bool stats -- for future use */
};

STATIC struct qinit dl_rinit = {
	.qi_putp = ss7_oput,		/* Read put (msg from below) */
	.qi_srvp = ss7_osrv,		/* Read queue service */
	.qi_qopen = sdlm_open,		/* Each open */
	.qi_qclose = sdlm_close,	/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &dl_minfo,		/* Information */
	.qi_mstat = &dl_rmstat,		/* Statistics */
};

STATIC struct qinit dl_winit = {
	.qi_putp = ss7_iput,		/* Write put (msg from above) */
	.qi_srvp = ss7_isrv,		/* Write queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &dl_minfo,		/* Information */
	.qi_mstat = &dl_wmstat,		/* Statistics */
};

STATIC struct module_stat dl_lr_mstat = {
	.ms_pcnt = 0,			/* calls to qi_putp() */
	.ms_scnt = 0,			/* calls to qi_srvp() */
	.ms_ocnt = 0,			/* calls to qi_qopen() */
	.ms_ccnt = 0,			/* calls to qi_qclose() */
	.ms_acnt = 0,			/* calls to qi_qadmin() */
	.ms_xptr = NULL,		/* module private stats */
	.ms_xsize = 0,			/* length of private stats */
	.ms_flags = 0,			/* bool stats -- for future use */
};

STATIC struct module_stat dl_lw_mstat = {
	.ms_pcnt = 0,			/* calls to qi_putp() */
	.ms_scnt = 0,			/* calls to qi_srvp() */
	.ms_ocnt = 0,			/* calls to qi_qopen() */
	.ms_ccnt = 0,			/* calls to qi_qclose() */
	.ms_acnt = 0,			/* calls to qi_qadmin() */
	.ms_xptr = NULL,		/* module private stats */
	.ms_xsize = 0,			/* length of private stats */
	.ms_flags = 0,			/* bool stats -- for future use */
};

STATIC struct qinit sd_rinit = {
	.qi_putp = ss7_iput,		/* Read put (msg from below) */
	.qi_srvp = ss7_isrv,		/* Read queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &dl_minfo,		/* Information */
	.qi_mstat = &dl_lr_mstat,	/* Statistics */
};

STATIC struct qinit sd_winit = {
	.qi_putp = ss7_oput,		/* Write put (msg from above) */
	.qi_srvp = ss7_osrv,		/* Write queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &dl_minfo,		/* Information */
	.qi_mstat = &dl_lw_mstat,	/* Statistics */
};

STATIC struct streamtab sdlminfo = {
	.st_rdinit = &dl_rinit,		/* Upper read queue */
	.st_wrinit = &dl_winit,		/* Upper write queue */
	.st_muxrinit = &sd_rinit,	/* Lower read queue */
	.st_muxwinit = &sd_winit,	/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Structure and Caches
 *
 *  =========================================================================
 */
typedef struct dl {
	STR_DECLARATION (struct dl);	/* stream declaration */
} dl_t;

#define DL_PRIV(__q) ((struct dl *)((__q)->q_ptr))

typedef struct sd {
	STR_DECLARATION (struct sd);	/* stream declaration */
	ulong ppa;
} sd_t;

#define SD_PRIV(__q) ((struct sd *)((__q)->q_ptr))

typedef struct df {
	spinlock_t lock;		/* structure lock */
	SLIST_HEAD (dl, dl);		/* master list of dl structures */
	SLIST_HEAD (sd, sd);		/* master list of sd structures */
} df_t;
STATIC struct df master;

/*
 *  -------------------------------------------------------------------------
 *
 *  Caches
 *
 *  -------------------------------------------------------------------------
 */

STATIC kmem_cache_t *sdlm_dl_cachep = NULL;
STATIC kmem_cache_t *sdlm_sd_cachep = NULL;

STATIC int
sdlm_term_caches(void)
{
	int err = 0;

	if (sdlm_sd_cachep) {
		if (kmem_cache_destroy(sdlm_sd_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sdlm_sd_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sdlm_sd_cachep\n", DRV_NAME));
	}
	if (sdlm_dl_cachep) {
		if (kmem_cache_destroy(sdlm_dl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sdlm_dl_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sdlm_dl_cachep\n", DRV_NAME));
	}
	return (err);
}
STATIC int
sdlm_init_caches(void)
{
	if (!sdlm_dl_cachep
	    && !(sdlm_dl_cachep = kmem_cache_create("sdlm_dl_cachep", sizeof(struct dl), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sdlm_dl_cachep", DRV_NAME);
		goto error;
	} else
		printd(("%s: initialized sdlm dl structure cache\n", DRV_NAME));
	if (!sdlm_sd_cachep
	    && !(sdlm_sd_cachep = kmem_cache_create("sdlm_sd_cachep", sizeof(struct sd), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sdlm_sd_cachep", DRV_NAME);
		goto error;
	} else
		printd(("%s: initialized sdlm sd structure cache\n", DRV_NAME));
	return (0);
      error:
	sdlm_term_caches();
	return (-ENOMEM);
}

STATIC struct dl *
dl_get(struct dl *dl)
{
	assure(dl);
	if (dl)
		atomic_inc(&dl->refcnt);
	return (dl);
}

STATIC void
dl_put(struct dl *dl)
{
	if (dl && atomic_dec_and_test(&dl->refcnt)) {
		kmem_cache_free(sdlm_dl_cachep, dl);
		printd(("%s: %s: %p: deallocated dl structure\n", DRV_NAME, __FUNCTION__, dl));
	}
}

STATIC streamscall int dl_r_prim(queue_t *, mblk_t *);
STATIC streamscall int dl_w_prim(queue_t *, mblk_t *);

STATIC struct dl *
sdlm_alloc_dl(queue_t *q, struct dl **dpp, major_t cmajor, minor_t cminor, cred_t *crp)
{
	struct dl *dl;

	printd(("%s: %s: create dl device = %hu:%hu\n", DRV_NAME, __FUNCTION__, cmajor, cminor));
	if ((dl = kmem_cache_alloc(sdlm_dl_cachep, SLAB_ATOMIC))) {
		bzero(dl, sizeof(*dl));
		dl_get(dl);	/* first get */
		dl->u.dev.cmajor = cmajor;
		dl->u.dev.cminor = cminor;
		dl->cred = *crp;
		spin_lock_init(&dl->qlock);
		(dl->iq = RD(q))->q_ptr = dl_get(dl);
		(dl->oq = RD(q))->q_ptr = dl_get(dl);
		dl->i_prim = dl_r_prim;
		dl->o_prim = dl_w_prim;
		dl->i_wakeup = NULL;
		dl->o_wakeup = NULL;
		dl->i_state = DL_UNATTACHED;
		dl->i_style = DL_STYLE2;
		dl->i_version = 2;
		spin_lock_init(&dl->lock);
		/* place in master list */
		if ((dl->next = *dpp))
			dl->next->prev = &dl->next;
		dl->prev = dpp;
		*dpp = dl_get(dl);
		master.dl.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate dl structure %hu:%hu\n", DRV_NAME,
			__FUNCTION__, cmajor, cminor));
	return (dl);
}

STATIC void
sdlm_free_dl(queue_t *q)
{
	struct dl *dl = (struct dl *) q->q_ptr;
	psw_t flags;

	ensure(dl, return);
	printd(("%s: %s: %p: free dl %hu:%hu\n", DRV_NAME, __FUNCTION__, dl, dl->u.dev.cmajor,
		dl->u.dev.cminor));
	spin_lock_irqsave(&dl->lock, flags);
	{
		/* stopping bufcalls */
		ss7_unbufcall((str_t *) dl);
		/* flush buffers */
		flushq(dl->oq, FLUSHALL);
		flushq(dl->iq, FLUSHALL);
		/* remove from master list */
		if ((*dl->prev = dl->next))
			dl->next->prev = dl->prev;
		dl->next = NULL;
		dl->prev = &dl->next;
		ensure(atomic_read(&dl->refcnt) > 1, dl_get(dl));
		dl_put(dl);
		assure(master.dl.numb > 0);
		master.dl.numb--;
		/* remove from queues */
		ensure(atomic_read(&dl->refcnt) > 1, dl_get(dl));
		dl_put(xchg(&dl->oq->q_ptr, NULL));
		ensure(atomic_read(&dl->refcnt) > 1, dl_get(dl));
		dl_put(xchg(&dl->iq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&dl->refcnt) != 1) {
			pswerr(("%s: %s: %p: ERROR: dl lingering reference count = %d\n",
				DRV_NAME, __FUNCTION__, dl, atomic_read(&dl->refcnt)));
			atomic_set(&dl->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&dl->lock, flags);
	dl_put(dl);		/* final put */
	return;
}

STATIC struct sd *
sdlm_get(struct sd *sd)
{
	assure(sd);
	if (sd)
		atomic_inc(&sd->refcnt);
	return (sd);
}

STATIC void
sdlm_put(struct sd *sd)
{
	if (sd && atomic_dec_and_test(&sd->refcnt)) {
		kmem_cache_free(sdlm_sd_cachep, sd);
		printd(("%s: %s: %p: deallocated sd structure", DRV_NAME, __FUNCTION__, sd));
	}
}

STATIC streamscall int sd_w_prim(queue_t *, mblk_t *);
STATIC streamscall int sd_r_prim(queue_t *, mblk_t *);

STATIC struct sd *
sdlm_alloc_sd(queue_t *q, struct sd **spp, ulong index, cred_t *crp)
{
	struct sd *sd;

	printd(("%s: %s: create sd index = %lu\n", DRV_NAME, __FUNCTION__, index));
	if ((sd = kmem_cache_alloc(sdlm_sd_cachep, SLAB_ATOMIC))) {
		bzero(sd, sizeof(*sd));
		sdlm_get(sd);	/* first get */
		sd->u.mux.index = index;
		sd->cred = *crp;
		spin_lock_init(&sd->qlock);
		(sd->iq = RD(q))->q_ptr = sdlm_get(sd);
		(sd->oq = WR(q))->q_ptr = sdlm_get(sd);
		sd->o_prim = sd_w_prim;
		sd->i_prim = sd_r_prim;
		sd->o_wakeup = NULL;
		sd->i_wakeup = NULL;
		sd->i_state = LMI_UNATTACHED;
		sd->i_version = LMI_STYLE2;
		spin_lock_init(&sd->lock);
		/* place in master list */
		if ((sd->next = *spp))
			sd->next->prev = &sd->next;
		sd->prev = spp;
		*spp = sdlm_get(sd);
		master.sd.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate sd structure %lu\n", DRV_NAME,
			__FUNCTION__, index));
	return (sd);
}

STATIC void
sdlm_free_sd(queue_t *q)
{
	struct sd *sd = (struct sd *) q->q_ptr;
	psw_t flags;

	ensure(sd, return);
	printd(("%s: %s: %p: free sd %lu\n", DRV_NAME, __FUNCTION__, sd, sd->u.mux.index));
	spin_lock_irqsave(&sd->lock, flags);
	{
		/* stopping bufcalls */
		ss7_unbufcall((str_t *) sd);
		/* flush buffers */
		flushq(sd->oq, FLUSHALL);
		flushq(sd->iq, FLUSHALL);
		/* remove from master list */
		if ((*sd->prev = sd->next))
			sd->next->prev = sd->prev;
		sd->next = NULL;
		sd->prev = &sd->next;
		ensure(atomic_read(&sd->refcnt) > 1, sdlm_get(sd));
		sdlm_put(sd);
		assure(master.sd.numb > 0);
		master.sd.numb--;
		/* remove from queues */
		ensure(atomic_read(&sd->refcnt) > 1, sdlm_get(sd));
		sdlm_put(xchg(&sd->oq->q_ptr, NULL));
		ensure(atomic_read(&sd->refcnt) > 1, sdlm_get(sd));
		sdlm_put(xchg(&sd->iq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&sd->refcnt) != 1) {
			pswerr(("%s: %s: %p: ERROR: sd lingering reference count = %d\n",
				DRV_NAME, __FUNCTION__, sd, atomic_read(&sd->refcnt)));
			atomic_set(&sd->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&sd->lock, flags);
	sdlm_put(sd);		/* final put */
	return;
}

/*
 *  =========================================================================
 *
 *  Primitives
 *
 *  =========================================================================
 */
STATIC INLINE int
lmi_ok_ack_reply(queue_t *q, mblk_t *mp, int prim, int state)
{
	lmi_ok_ack_t *p;

	mp->b_wptr = mp->b_rptr;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->lmi_primitive = LMI_OK_ACK;
	p->lmi_correct_primitive = prim;
	p->lmi_state = state;
	qreply(q, mp);
	return (0);
}
STATIC INLINE int
lmi_error_ack_reply(queue_t *q, mblk_t *mp, int prim, int state, int err)
{
	lmi_error_ack_t *p;

	/* 
	   return a negative acknowledgement */
	mp->b_wptr = mp->b_rptr;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->lmi_primitive = LMI_ERROR_ACK;
	p->lmi_error_primitive = prim;
	p->lmi_state = state;
	p->lmi_errno = err > 0 ? 0 : -err;
	p->lmi_reason = err > 0 ? err : LMI_SYSERR;
	qreply(q, mp);
	return (err);
}
STATIC INLINE int
m_error_reply(queue_t *q, mblk_t *mp, int err)
{
	mp->b_wptr = mp->b_rptr;
	mp->b_datap->db_type = M_ERROR;
	*(mp->b_wptr)++ = err < 0 ? -err : err;
	*(mp->b_wptr)++ = err < 0 ? -err : err;
	qreply(q, mp);
	return (err);
}

/*
 *  LMI_ATTACH_REQ:
 *  -----------------------------------
 */
STATIC INLINE int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_attach_req_t *m = ((typeof(m)) mp->b_rptr);
	if (mlen < sizeof(*m) || mlen < 2 * sizeof(ulong))
		goto lprotoshort;
	if (q->q_next)
		goto loutstate;
	{
		struct dl *dl = DL_PRIV(q);
		struct sd *sd;
		ulong ppa = *((ulong *) &m->lmi_ppa);

		for (sd = master.sd.list; sd && sd->ppa != ppa; sd = sd->next) ;
		if (!sd || sd->iq->q_next)
			goto lbadppa;
		/* link queues together */
#ifdef LFS
		weldq(dl->iq, sd->oq, sd->iq, dl->oq, NULL, 0, dl->iq);
#else
		dl->iq->q_next = sd->oq;
		sd->iq->q_next = dl->oq;
#endif
		return lmi_ok_ack_reply(q, mp, LMI_ATTACH_REQ, LMI_DISABLED);
	}
      lbadppa:
	err = LMI_BADPPA;
	ptrace(("ERROR: PPA in use\n"));
	goto error;
      loutstate:
	err = LMI_OUTSTATE;
	ptrace(("ERROR: would place i/f out of state\n"));
	goto error;
      lprotoshort:
	err = LMI_PROTOSHORT;
	ptrace(("ERROR: proto block to short\n"));
	goto error;
      error:
	return lmi_error_ack_reply(q, mp, LMI_ATTACH_REQ, LMI_UNATTACHED, err);
}

/*
 *  LMI_DETACH_REQ:
 *  -----------------------------------
 */
STATIC INLINE int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_detach_req_t *m = ((typeof(m)) mp->b_rptr);

	if (mlen < sizeof(*m))
		goto lprotoshort;
	if (!q->q_next)
		goto loutstate;
	{
		lmi_ok_ack_t *p;
		struct dl *dl = DL_PRIV(q);
		struct sd *sd = SD_PRIV(q->q_next);

		/* disconnect them */
#ifdef LFS
		unweldq(dl->iq, sd->oq, sd->iq, dl->oq, NULL, 0, dl->iq);
#else
		dl->iq->q_next = NULL;
		sd->iq->q_next = NULL;
#endif
		mp->b_wptr = mp->b_rptr;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = LMI_DETACH_REQ;
		p->lmi_state = LMI_UNATTACHED;
		qreply(q, mp);
		return (0);
	}
      loutstate:
	err = LMI_OUTSTATE;
	ptrace(("ERROR: would place i/f out of state\n"));
	goto error;
      lprotoshort:
	err = LMI_PROTOSHORT;
	ptrace(("ERROR: proto block too short\n"));
	goto error;
      error:
	return lmi_error_ack_reply(q, mp, LMI_DETACH_REQ, LMI_UNATTACHED, err);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	long prim = *((long *) mp->b_rptr);

	switch (prim) {
	case LMI_ATTACH_REQ:
		return ((int) lmi_attach_req(q, mp));
	case LMI_DETACH_REQ:
		return ((int) lmi_detach_req(q, mp));
	case LMI_INFO_REQ:
	case LMI_ENABLE_REQ:
	case LMI_DISABLE_REQ:
	case LMI_OPTMGMT_REQ:
		if (q->q_next) {
			putnext(q, mp);
			return ((int) (0));
		}
		ptrace(("ERROR: would place i/f out of state\n"));
		return lmi_error_ack_reply(q, mp, prim, LMI_UNATTACHED, LMI_OUTSTATE);
	case SDL_BITS_FOR_TRANSMISSION_REQ:
	case SDL_CONNECT_REQ:
	case SDL_DISCONNECT_REQ:
		if (q->q_next) {
			putnext(q, mp);
			return ((int) (0));
		}
		return ((int) m_error_reply(q, mp, EPIPE));
	default:
		freemsg(mp);
		return ((int) m_error_reply(q, mp, EPROTO));
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;

	(void) dl;
	(void) size;
	(void) count;
	switch (type) {
	case _IOC_TYPE(__SID):
	{
		psw_t flags;
		struct sd *sd, **spp;
		struct linkblk *lb = (struct linkblk *) arg;

		if (!lb) {
			swerr();
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			ptrace(("%s: %p: I_PLINK\n", DRV_NAME, dl));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n", DRV_NAME,
					dl));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			ptrace(("%s: %p: I_LINK\n", DRV_NAME, dl));
			MOD_INC_USE_COUNT;	/* keep module from unloading */
			spin_lock_irqsave(&master.lock, flags);
			{
				/* place in list in ascending index order */
				for (spp = &master.sd.list;
				     *spp && (*spp)->u.mux.index < lb->l_index;
				     spp = &(*spp)->next) ;
				if ((sd =
				     sdlm_alloc_sd(lb->l_qbot, spp, lb->l_index, iocp->ioc_cr))) {
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				MOD_DEC_USE_COUNT;
				ret = -ENOMEM;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		case _IOC_NR(I_PUNLINK):
			ptrace(("%s: %p: I_PUNLINK\n", DRV_NAME, dl));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					dl));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			ptrace(("%s: %p: I_UNLINK\n", DRV_NAME, dl));
			spin_lock_irqsave(&master.lock, flags);
			{
				for (sd = master.sd.list; sd; sd = sd->next)
					if (sd->u.mux.index == lb->l_index)
						break;
				if (!sd) {
					ret = -EINVAL;
					ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n",
						DRV_NAME, dl));
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				sdlm_free_sd(sd->iq);
				MOD_DEC_USE_COUNT;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unspported STREAMS ioctl %c, %d\n", DRV_NAME, dl,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", DRV_NAME, dl, (char) type,
			nr));
		ret = -EOPNOTSUPP;
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  UPPER MUX Queues
 *  -----------------------------------
 */
STATIC streamscall int
dl_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	default:
		return (QR_PASSFLOW);
	}
}
STATIC streamscall int
dl_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(q, mp);
	case M_IOCTL:
		return dl_w_ioctl(q, mp);
	default:
	case M_DATA:
		return (QR_PASSFLOW);
	}
}

/*
 *  LOWER MUX Queues
 *  -----------------------------------
 */
STATIC streamscall int
sd_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	default:
		return (QR_PASSFLOW);
	}
}
STATIC streamscall int
sd_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	default:
		return (QR_PASSFLOW);
	}
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC major_t sdlm_majors[SDLM_CMAJORS] = { SDLM_CMAJOR_0, };

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
sdlm_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct dl *dl, **dpp;

	MOD_INC_USE_COUNT;
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (cmajor != CMAJOR_0 || cminor > 0) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* allocate a new device */
	cminor = 1;
	spin_lock_irqsave(&master.lock, flags);
	for (dpp = &master.dl.list; *dpp; dpp = &(*dpp)->next) {
		major_t dmajor = (*dpp)->u.dev.cmajor;

		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*dpp)->u.dev.cminor;

			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = sdlm_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(dl = sdlm_alloc_dl(q, dpp, cmajor, cminor, crp))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
sdlm_close(queue_t *q, int flag, cred_t *crp)
{
	struct dl *dl = DL_PRIV(q);
	psw_t flags;

	(void) dl;
	printd(("%s: %p: closing character device %hu:%hu\n", DRV_NAME, dl, dl->u.dev.cmajor,
		dl->u.dev.cminor));
	spin_lock_irqsave(&master.lock, flags);
	{
		sdlm_free_dl(q);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	MOD_DEC_USE_COUNT;
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

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SDL-MUX driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the SDL-MUX driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw sdlm_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sdlminfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
sdlm_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&sdlm_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
sdlm_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&sdlm_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
sdlm_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &sdlminfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
sdlm_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
sdlmterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (sdlm_majors[mindex]) {
			if ((err = sdlm_unregister_strdev(sdlm_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					sdlm_majors[mindex]);
			if (mindex)
				sdlm_majors[mindex] = 0;
		}
	}
	if ((err = sdlm_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
sdlminit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = sdlm_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		sdlmterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = sdlm_register_strdev(sdlm_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					sdlm_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				sdlmterminate();
				return (err);
			}
		}
		if (sdlm_majors[mindex] == 0)
			sdlm_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(sdlm_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = sdlm_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sdlminit);
module_exit(sdlmterminate);

#endif				/* LINUX */
