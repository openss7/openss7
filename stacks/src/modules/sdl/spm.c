/*****************************************************************************

 @(#) $RCSfile: spm.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/01/24 07:48:08 $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/01/24 07:48:08 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: spm.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/01/24 07:48:08 $"

static char const ident[] = "$RCSfile: spm.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/01/24 07:48:08 $";

/*
 *  This is an SDL pipemod driver for testing and use with pipes.  This module
 *  is pushed on one side of the pipe (after pipemod) to make a pipe appear as
 *  a directly connected pair of SDL drivers.
 */

#define _DEBUG 1

#include "compat.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>

#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

#define SPM_DESCRIP	"SS7/SDL: (Signalling Data Terminal) STREAMS PIPE MODULE."
#define SPM_REVISION	"OpenSS7 $RCSfile: spm.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/01/24 07:48:08 $"
#define SPM_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SPM_DEVICE	"Provides OpenSS7 SDL pipe driver."
#define SPM_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SPM_LICENSE	"GPL"
#define SPM_BANNER	SPM_DESCRIP	"\n" \
			SPM_REVISION	"\n" \
			SPM_COPYRIGHT	"\n" \
			SPM_DEVICE	"\n" \
			SPM_CONTACT	"\n"
#define SPM_SPLASH	SPM_DEVICE	" - " \
			SPM_REVISION

#ifdef LINUX
MODULE_AUTHOR(SPM_CONTACT);
MODULE_DESCRIPTION(SPM_DESCRIP);
MODULE_SUPPORTED_DEVICE(SPM_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SPM_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* LINUX */

#ifdef LFS
#define SPM_MOD_ID	CONFIG_STREAMS_SPM_MODID
#define SPM_MOD_NAME	CONFIG_STREAMS_SPM_NAME
#endif				/* LFS */

#ifndef SPM_MOD_NAME
#define SPM_MOD_NAME	"spm"
#endif

#ifndef SPM_MOD_ID
#define SPM_MOD_ID	0
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define MOD_ID		SPM_MOD_ID
#define MOD_NAME	SPM_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SPM_BANNER
#else				/* MODULE */
#define MOD_BANNER	SPM_SPLASH
#endif				/* MODULE */

STATIC struct module_info spm_winfo = {
	MOD_ID,				/* Module ID number */
	MOD_NAME "-wr",			/* Module name */
	1,				/* Min packet size accepted */
	280,				/* Max packet size accepted */
	1,				/* Hi water mark */
	0				/* Lo water mark */
};

STATIC struct module_info spm_rinfo = {
	MOD_ID,				/* Module ID number */
	MOD_NAME "-rd",			/* Module name */
	1,				/* Min packet size accepted */
	128,				/* Max packet size accepted */
	1,				/* Hi water mark */
	0				/* Lo water mark */
};

STATIC int spm_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int spm_close(queue_t *, int, cred_t *);

STATIC int spm_wput(queue_t *, mblk_t *);
STATIC int spm_wsrv(queue_t *);

STATIC struct qinit spm_winit = {
	spm_wput,			/* Write put (message from above) */
	spm_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&spm_winfo,			/* Information */
	NULL				/* Statistics */
};

STATIC int spm_rput(queue_t *, mblk_t *);
STATIC int spm_rsrv(queue_t *);

STATIC struct qinit spm_rinit = {
	spm_rput,			/* Read put (message from below) */
	spm_rsrv,			/* Read queue service */
	spm_open,			/* Each open */
	spm_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&spm_rinfo,			/* Information */
	NULL				/* Statistics */
};

STATIC struct streamtab spminfo = {
	&spm_rinit,			/* Upper read queue */
	&spm_winit,			/* Upper write queue */
	NULL,				/* Lower read queue */
	NULL				/* Lower write queue */
};

/*
 *  ========================================================================
 *
 *  Private structure
 *
 *  ========================================================================
 */

typedef struct spm {
	struct spm *next;		/* list linkage */
	struct spm **prev;		/* list linkage */
	major_t cmajor;			/* major device number */
	minor_t cminor;			/* minor device number */
	queue_t *rq;			/* rd queue */
	queue_t *wq;			/* wr queue */
	uint rbid;			/* rd bufcall id */
	uint wbid;			/* wr bufcall id */
	spinlock_t lock;		/* queue lock */
	uint nest;			/* nest of this queue lock */
	void *user;			/* user of this queue lock */
	uint state;			/* interface state */
	uint version;			/* interface version */
	uint flags;			/* interface flags */
	size_t refcnt;			/* reference count */
	lmi_option_t option;		/* LMI protocol and variant options */
	sdl_statem_t statem;		/* SDL state machine variables */
	sdl_config_t config;		/* SDL configuration options */
	sdl_notify_t notify;		/* SDL notification options */
	sdl_stats_t stats;		/* SDL statistics */
	sdl_stats_t stamp;		/* SDL statistics timestamps */
	sdl_stats_t statsp;		/* SDL statistics periods */
	unsigned long wts;		/* WR throttle timestamp */
	uint wno;			/* WR throttle count */
	uint wtim;			/* WR throttle timer */
	unsigned long rts;		/* RD throttle timestamp */
	uint rno;			/* RD throttle count */
	uint rtim;			/* WR throttle timer */
} spm_t;
#define PRIV(__q) ((spm_t *)(__q)->q_ptr)

/*
 *  ========================================================================
 *
 *  PRIVATE struct allocation and deallocation
 *
 *  ========================================================================
 */
STATIC kmem_cache_t *spm_priv_cachep = NULL;
/*
 *  Cache allocation
 *  ------------------------------------------------------------------------
 */
STATIC int
spm_init_caches(void)
{
	if (!spm_priv_cachep &&
	    !(spm_priv_cachep = kmem_cache_create
	      ("spm_priv_cachep", sizeof(spm_t), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate spm_priv_cachep", MOD_NAME);
		return (-ENOMEM);
	} else
		printd(("%s: Allocated/selected private structure cache\n", MOD_NAME));
	return (0);
}
STATIC int
spm_term_caches(void)
{
	if (spm_priv_cachep) {
		if (kmem_cache_destroy(spm_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy spm_priv_cachep.", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("spm: destroyed spm_priv_cachep\n"));
	}
	return (0);
}

/*
 *  Private structure allocation
 *  ------------------------------------------------------------------------
 */
STATIC spm_t *
spm_alloc_priv(queue_t *q, spm_t ** sp, major_t cmajor, minor_t cminor)
{
	spm_t *s;
	if ((s = kmem_cache_alloc(spm_priv_cachep, SLAB_ATOMIC))) {
		printd(("spm: allocated module private structure\n"));
		bzero(s, sizeof(*s));
		if ((s->next = *sp))
			s->next->prev = &s->next;
		s->prev = sp;
		*sp = s;
		printd(("spm: linked module private structure\n"));
		s->rq = RD(q);
		s->wq = WR(q);
		s->rq->q_ptr = s->wq->q_ptr = s;
		s->cmajor = cmajor;
		s->cminor = cminor;
		spin_lock_init(&s->lock);	/* "spm-queue-lock" */
		s->state = LMI_DISABLED;	/* Style 1 */
		s->version = 1;
		s->wts = jiffies;
		s->wno = 0;
		s->rts = jiffies;
		s->rno = 0;
		/*
		 *  Set some defaults:
		 */
		printd(("spm: setting module defaults\n"));
		/*
		   LMI configuration defaults 
		 */
		s->option.pvar = SS7_PVAR_ITUT_88;
		s->option.popt = 0;
#if 0
		/*
		   SDT configuration defaults 
		 */
		s->config.Tin = 4;	/* AERM normal proving threshold */
		s->config.Tie = 1;	/* AERM emergency proving threshold */
		s->config.T = 64;	/* SUERM error threshold */
		s->config.D = 256;	/* SUERM error rate parameter */
		s->config.t8 = 100 * HZ / 1000;	/* T8 timeout */
		s->config.Te = 577169;	/* EIM error threshold */
		s->config.De = 9308000;	/* EIM correct decrement */
		s->config.Ue = 144292000;	/* EIM error increment */
#endif
	}
	return (s);
}
STATIC void
spm_free_priv(queue_t *q)
{
	uint t;
	spm_t *s = PRIV(q);
	ensure(s, return);
	if (s->rbid)
		unbufcall(xchg(&s->rbid, 0));
	if (s->wbid)
		unbufcall(xchg(&s->wbid, 0));
	if ((*(s->prev) = s->next))
		s->next->prev = s->prev;
	s->next = NULL;
	s->prev = NULL;
	if ((t = xchg(&s->wtim, 0)))
		untimeout(t);
	if ((t = xchg(&s->rtim, 0)))
		untimeout(t);
	noenable(s->wq);
	noenable(s->rq);
	assure(s->refcnt == 0);
	printd(("spm: unlinked module private structure\n"));
	kmem_cache_free(spm_priv_cachep, s);
	printd(("spm: freed module private structure\n"));
	return;
}

/*
 *  ========================================================================
 *
 *  PRIMITIVES sent upstream or downstream
 *
 *  ========================================================================
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
#if 0
STATIC int
m_error(queue_t *q, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDL_RECEIVED_BITS_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdl_received_bits_ind(queue_t *q, mblk_t *mp)
{
	union SDL_primitives *p = (union SDL_primitives *) mp->b_rptr;
	p->sdl_primitive = SDL_RECEIVED_BITS_IND;
	putnext(q, mp);
	return (QR_ABSORBED);
}

/*
 *  SDL_DISCONNECT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdl_disconnect_ind(queue_t *q, mblk_t *mp)
{
	union SDL_primitives *p = (union SDL_primitives *) mp->b_rptr;
	p->sdl_primitive = SDL_DISCONNECT_IND;
	putnext(q, mp);
	return (QR_ABSORBED);
}

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, long state, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = state;
		p->lmi_max_sdu = 8;
		p->lmi_min_sdu = 8;
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE1;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_ok_ack(queue_t *q, long state, long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		p->lmi_state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ack(queue_t *q, long state, long prim, long err, long reason)
{
	mblk_t *mp;
	lmi_error_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err;
		p->lmi_reason = reason;
		p->lmi_state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_enable_con(queue_t *q, long state)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_disable_con(queue_t *q, long state)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
#if 0
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ind(queue_t *q, long state, long error, long reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = error;
		p->lmi_reason = reason;
		p->lmi_state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_stats_ind(queue_t *q)
{
	mblk_t *mp;
	lmi_stats_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = 0;
		p->lmi_timestamp = jiffies;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, ulong oid, ulong level, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	lmi_event_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + inf_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_EVENT_IND;
		p->lmi_objectid = oid;
		p->lmi_timestamp = jiffies;
		p->lmi_severity = level;
		bcopy(inf_ptr, mp->b_wptr, inf_len);
		mp->b_wptr += inf_len;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ========================================================================
 *
 *  EVENTS from above or below
 *
 *  ========================================================================
 *
 *  MESSAGES from above or below
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  SDL_BITS_FOR_TRANSMISSION_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_bits_for_transmission_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/*
	   strip to M_DATA 
	 */
	return (QR_STRIP);
}

/*
 *  SDL_CONNECT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_connect_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/*
	   ignore 
	 */
	return (QR_DONE);
}

/*
 *  SDL_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_disconnect_req(queue_t *q, mblk_t *mp)
{
	/*
	   translated to indication 
	 */
	return sdl_disconnect_ind(q, mp);
}

/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	(void) mp;
	return lmi_info_ack(q, 0, NULL, 0);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	(void) mp;
	return lmi_ok_ack(q, LMI_DISABLED, LMI_ATTACH_REQ);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	(void) mp;
	return lmi_ok_ack(q, LMI_UNATTACHED, LMI_DETACH_REQ);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	(void) mp;
	return lmi_enable_con(q, LMI_ENABLED);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	(void) mp;
	return lmi_disable_con(q, LMI_DISABLED);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	(void) mp;
	return lmi_error_ack(q, 0, LMI_OPTMGMT_REQ, EOPNOTSUPP, 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IO CONTROLS from above or below
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  SDL_IOCGOPTIONS
 *  -----------------------------------
 */
STATIC int
sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	*(lmi_option_t *) arg = s->option;
	return (0);
}

/*
 *  SDL_IOCSOPTIONS
 *  -----------------------------------
 */
STATIC int
sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	s->option = *(lmi_option_t *) arg;
	return (0);
}

/*
 *  SDL_IOCGCONFIG
 *  -----------------------------------
 */
STATIC int
sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	*(sdl_config_t *) arg = s->config;
	return (0);
}

/*
 *  SDL_IOCSCONFIG
 *  -----------------------------------
 */
STATIC int
sdl_iocsconfig(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	s->config = *(sdl_config_t *) arg;
	return (0);
}

/*
 *  SDL_IOCTCONFIG
 *  -----------------------------------
 */
STATIC int
sdl_ioctconfig(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	(void) s;
	(void) arg;
	fixme(("Test the configuration\n"));
	return (-EOPNOTSUPP);
}

/*
 *  SDL_IOCCCONFIG
 *  -----------------------------------
 */
STATIC int
sdl_ioccconfig(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	(void) s;
	(void) arg;
	fixme(("Commit the configuration\n"));
	return (-EOPNOTSUPP);
}

/*
 *  SDL_IOCGSTATEM
 *  -----------------------------------
 */
STATIC int
sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	*(sdl_statem_t *) arg = s->statem;
	return (0);
}

/*
 *  SDL_IOCCMRESET
 *  -----------------------------------
 */
STATIC int
sdl_ioccmreset(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	(void) s;
	(void) arg;
	fixme(("Master reset\n"));
	return (-EOPNOTSUPP);
}

/*
 *  SDL_IOCGSTATSP
 *  -----------------------------------
 */
STATIC int
sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	*(sdl_stats_t *) arg = s->statsp;
	return (0);
}

/*
 *  SDL_IOCSSTATSP
 *  -----------------------------------
 */
STATIC int
sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	s->statsp = *(sdl_stats_t *) arg;
	return (0);
}

/*
 *  SDL_IOCGSTATS
 *  -----------------------------------
 */
STATIC int
sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	*(sdl_stats_t *) arg = s->stats;
	return (0);
}

/*
 *  SDL_IOCCSTATS
 *  -----------------------------------
 */
STATIC int
sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	(void) arg;
	bzero(&s->stats, sizeof(s->stats));
	return (0);
}

/*
 *  SDL_IOCGNOTIFY
 *  -----------------------------------
 */
STATIC int
sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	*(sdl_notify_t *) arg = s->notify;
	return (0);
}

/*
 *  SDL_IOCSNOTIFY
 *  -----------------------------------
 */
STATIC int
sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	s->notify = *(sdl_notify_t *) arg;
	return (0);
}

/*
 *  SDL_IOCCNOTIFY
 *  -----------------------------------
 */
STATIC int
sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	s->notify.events &= ~((sdl_notify_t *) arg)->events;
	return (0);
}

/*
 *  ========================================================================
 *
 *  STREAMS Message Handling
 *
 *  ========================================================================
 *
 *  M_IOCTL Handling
 *  -----------------------------------------------------------------------
 */
STATIC int
spm_m_ioctl(queue_t *q, mblk_t *mp)
{
	int ret;
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	(void) nr;
	switch (type) {
	case __SID:
	{
		switch (cmd) {
		default:
			ptrace(("spm: ERROR: Unknown IOCTL %d\n", cmd));
			ret = -EINVAL;
			break;
		case I_STR:
		case I_LINK:
		case I_PLINK:
		case I_UNLINK:
		case I_PUNLINK:
		{
			void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
			struct linkblk *lp = (struct linkblk *) arg;
			rare();
			(void) lp;
			ret = -EINVAL;
			break;
		}
		}
		ret = -EFAULT;
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || PRIV(q)->state == LMI_UNATTACHED) {
			ret = -EINVAL;
			break;
		}
		switch (cmd) {
		case SDL_IOCGOPTIONS:	/* lmi_option_t */
			ret = sdl_iocgoptions(q, mp);
			break;
		case SDL_IOCSOPTIONS:	/* lmi_option_t */
			ret = sdl_iocsoptions(q, mp);
			break;
		case SDL_IOCGCONFIG:	/* sdt_config_t */
			ret = sdl_iocgconfig(q, mp);
			break;
		case SDL_IOCSCONFIG:	/* sdt_config_t */
			ret = sdl_iocsconfig(q, mp);
			break;
		case SDL_IOCTCONFIG:	/* sdt_config_t */
			ret = sdl_ioctconfig(q, mp);
			break;
		case SDL_IOCCCONFIG:	/* sdt_config_t */
			ret = sdl_ioccconfig(q, mp);
			break;
		case SDL_IOCGSTATEM:	/* sdt_statem_t */
			ret = sdl_iocgstatem(q, mp);
			break;
		case SDL_IOCCMRESET:	/* sdt_statem_t */
			ret = sdl_ioccmreset(q, mp);
			break;
		case SDL_IOCGSTATSP:	/* lmi_sta_t */
			ret = sdl_iocgstatsp(q, mp);
			break;
		case SDL_IOCSSTATSP:	/* lmi_sta_t */
			ret = sdl_iocsstatsp(q, mp);
			break;
		case SDL_IOCGSTATS:	/* sdt_stats_t */
			ret = sdl_iocgstats(q, mp);
			break;
		case SDL_IOCCSTATS:	/* sdt_stats_t */
			ret = sdl_ioccstats(q, mp);
			break;
		case SDL_IOCGNOTIFY:	/* sdt_notify_t */
			ret = sdl_iocgnotify(q, mp);
			break;
		case SDL_IOCSNOTIFY:	/* sdt_notify_t */
			ret = sdl_iocsnotify(q, mp);
			break;
		case SDL_IOCCNOTIFY:	/* sdt_notify_t */
			ret = sdl_ioccnotify(q, mp);
			break;
		default:
			ret = -EOPNOTSUPP;
			break;
		}
		ret = -EFAULT;
		break;
	}
	default:
		return (QR_PASSALONG);
	}
	if (ret >= 0) {
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
 *  M_PROTO, M_PCPROTO Handling
 *  -----------------------------------------------------------------------
 */
STATIC int
spm_m_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	spm_t *s = PRIV(q);
	ulong oldstate = s->state;
	if ((prim = *(ulong *) mp->b_rptr) == SDL_BITS_FOR_TRANSMISSION_REQ)
		rtn = sdl_bits_for_transmission_req(q, mp);
	else
		switch (prim) {
		case SDL_BITS_FOR_TRANSMISSION_REQ:
			rtn = sdl_bits_for_transmission_req(q, mp);
			break;
		case SDL_CONNECT_REQ:
			rtn = sdl_connect_req(q, mp);
			break;
		case SDL_DISCONNECT_REQ:
			rtn = sdl_disconnect_req(q, mp);
			break;
		case LMI_INFO_REQ:
			rtn = lmi_info_req(q, mp);
			break;
		case LMI_ATTACH_REQ:
			rtn = lmi_attach_req(q, mp);
			break;
		case LMI_DETACH_REQ:
			rtn = lmi_detach_req(q, mp);
			break;
		case LMI_ENABLE_REQ:
			rtn = lmi_enable_req(q, mp);
			break;
		case LMI_DISABLE_REQ:
			rtn = lmi_disable_req(q, mp);
			break;
		case LMI_OPTMGMT_REQ:
			rtn = lmi_optmgmt_req(q, mp);
			break;
		default:
			/*
			   pass along anything we don't recognize 
			 */
			rtn = QR_PASSFLOW;
			break;
		}
	if (rtn < 0)
		s->state = oldstate;
	return (rtn);
}

/*
 *  M_DATA Handling
 *  -------------------------------------------------------------------------
 *  We need to throttle these to avoid locking up the processor.  The throttle
 *  is for 10 blocks every 10ms maximum which is precisely 64kbps.
 */
STATIC void
spm_w_timeout(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	spm_t *s = PRIV(q);
	if (!xchg(&s->wtim, 0))
		return;
	enableok(q);
	qenable(q);
	return;
}
STATIC void
spm_r_timeout(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	spm_t *s = PRIV(q);
	if (!xchg(&s->rtim, 0))
		return;
	enableok(q);
	qenable(q);
	return;
}
STATIC INLINE int
spm_w_data(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	if (s->wts == jiffies) {
		if (s->wno++ >= 10) {
			/*
			   come back in a tick 
			 */
			s->wtim = timeout(&spm_w_timeout, (caddr_t) q, 1);
			// printd(("spm: tx: blocking\n"));
			return (QR_DISABLE);
		}
	} else {
		s->wno = 1;
		s->wts = jiffies;
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
spm_r_data(queue_t *q, mblk_t *mp)
{
	spm_t *s = PRIV(q);
	if (s->rts == jiffies) {
		if (s->rno++ >= 10) {
			/*
			   come back in a tick 
			 */
			s->rtim = timeout(&spm_r_timeout, (caddr_t) q, 1);
			// printd(("spm: rx: blocking\n"));
			return (QR_DISABLE);
		}
	} else {
		s->rno = 1;
		s->rts = jiffies;
	}
	return (QR_PASSFLOW);
}

/*
 *  M_RSE, M_PCRSE Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
spm_m_pcrse(queue_t *q, mblk_t *mp)
{
	int rtn;
	switch (*(ulong *) mp->b_rptr) {
	default:
		rtn = -EFAULT;
		break;
	}
	return (rtn);
}

/*
 *  M_FLUSH Handling
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
spm_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag)
{
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	return (QR_LOOP);
}
STATIC int
spm_w_flush(queue_t *q, mblk_t *mp)
{
	return spm_m_flush(q, mp, FLUSHW);
}
STATIC int
spm_r_flush(queue_t *q, mblk_t *mp)
{
	return spm_m_flush(q, mp, FLUSHR);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
spm_r_prim(queue_t *q, mblk_t *mp)
{
	/*
	   Fast Path 
	 */
	if (mp->b_datap->db_type == M_DATA)
		return spm_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return spm_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return spm_m_proto(q, mp);
	case M_RSE:
	case M_PCRSE:
		return spm_m_pcrse(q, mp);
	case M_FLUSH:
		return spm_r_flush(q, mp);
	case M_IOCTL:
		return spm_m_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
spm_w_prim(queue_t *q, mblk_t *mp)
{
	/*
	   Fast Path 
	 */
	if (mp->b_datap->db_type == M_DATA)
		return spm_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return spm_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return spm_m_proto(q, mp);
	case M_RSE:
	case M_PCRSE:
		return spm_m_pcrse(q, mp);
	case M_FLUSH:
		return spm_w_flush(q, mp);
	case M_IOCTL:
		return spm_m_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC void
spm_rx_wakeup(queue_t *q)
{
}
STATIC int
spm_rput(queue_t *q, mblk_t *mp)
{
	return (int) ss7_putq(q, mp, &spm_r_prim, &spm_rx_wakeup);
}
STATIC int
spm_rsrv(queue_t *q)
{
	return (int) ss7_srvq(q, &spm_r_prim, &spm_rx_wakeup);
}
STATIC void
spm_tx_wakeup(queue_t *q)
{
}
STATIC int
spm_wput(queue_t *q, mblk_t *mp)
{
	return (int) ss7_putq(q, mp, &spm_w_prim, &spm_tx_wakeup);
}
STATIC int
spm_wsrv(queue_t *q)
{
	return (int) ss7_srvq(q, &spm_w_prim, &spm_tx_wakeup);
}

/*
 *  =======================================================================
 *
 *  OPEN and CLOSE
 *
 *  =======================================================================
 */
STATIC spm_t *spm_list = NULL;
STATIC int
spm_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	spm_t *s, **sp = &spm_list;
	(void) crp;
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == DRVOPEN || WR(q)->q_next == NULL) {
		ptrace(("spm: ERROR: Can't open as driver\n"));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	for (; *sp && (*sp)->cmajor < cmajor; sp = &(*sp)->next) ;
	for (; *sp && (*sp)->cmajor == cmajor && (*sp)->cminor < cminor; sp = &(*sp)->next) ;
	if (*sp && (*sp)->cmajor == cmajor && (*sp)->cminor == cminor) {
		ptrace(("spm: ERROR: Multiple push\n"));
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("spm: Pushed module for char device %d:%d\n", cmajor, cminor));
	if (!(s = spm_alloc_priv(q, sp, cmajor, cminor))) {
		ptrace(("spm: ERROR: No cache memory\n"));
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	return (0);
}
STATIC int
spm_close(queue_t *q, int flag, cred_t *crp)
{
	spm_t *s = PRIV(q);
	(void) flag;
	(void) crp;
	(void) s;
	printd(("spm: Popped module for char device %hu:%hu\n", (ushort)s->cmajor, (ushort)s->cminor));
	spm_free_priv(q);
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

unsigned short modid = MOD_ID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module ID for the SPM module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw spm_fmod = {
	.f_name = MOD_NAME,
	.f_str = &spminfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
spm_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&spm_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
spm_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&spm_fmod)) < 0)
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
spm_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&spminfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
spm_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&spminfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
spminit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = spm_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = spm_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		spm_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
spmterminate(void)
{
	int err;
	if ((err = spm_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = spm_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(spminit);
module_exit(spmterminate);

#endif				/* LINUX */
