/*****************************************************************************

 @(#) $RCSfile: sscop2.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:12 $

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

 Last Modified $Date: 2004/08/26 23:38:12 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop2.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:12 $"

static char const ident[] =
    "$RCSfile: sscop2.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:12 $";

#include "compat.h"

#include <sys/npi.h>
#include <sys/npi_ip.h>

#define SSCOP_DESCRIP	"SSCOP STREAMS MODULE."
#define SSCOP_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SSCOP_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define SSCOP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SSCOP_LICENSE	"GPL"
#define SSCOP_BANNER	SSCOP_DESCRIP	"\n" \
			SSCOP_COPYRIGHT	"\n" \
			SSCOP_DEVICE	"\n" \
			SSCOP_CONTACT	"\n"
#define SSCOP_SPLASH	SSCOP_DEVICE	" - " \
			SSCOP_REVISION	"\n" \

#ifdef LINUX
MODULE_AUTHOR(SSCOP_CONTACT);
MODULE_DESCRIPTION(SSCOP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SSCOP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SSCOP_LICENSE);
#endif
#endif				/* LINUX */

#ifdef LFS
#define SSCOP_DRV_ID		CONFIG_STREAMS_SSCOP_MODID
#define SSCOP_DRV_NAME		CONFIG_STREAMS_SSCOP_NAME
#define SSCOP_CMAJORS		CONFIG_STREAMS_SSCOP_NMAJORS
#define SSCOP_CMAJOR_0		CONFIG_STREAMS_SSCOP_MAJOR
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */
static struct module_info s_minfo = {
	0,				/* Module ID number */
	"sscop",			/* Module name */
	0,				/* Min packet size accepted */
	INFPSZ,				/* Max packet size accepted */
	1 << 15,			/* Hi water mark */
	1 << 10				/* Lo water mark */
};

static int s_open(queue_t *, dev_t *, int, int, cred_t *);
static int s_close(queue_t *, int, cred_t *);

static INT s_rput(queue_t *, mblk_t *);

static struct qinit s_rinit = {
	s_rput,				/* Read put (msg from below) */
	NULL,				/* Read queue service */
	s_open,				/* Each open */
	s_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&s_minfo,			/* Information */
	NULL				/* Statistics */
};

static INT s_wput(queue_t *, mblk_t *);

static struct qinit s_winit = {
	s_wput,				/* Write put (msg from below) */
	NULL,				/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&s_minfo,			/* Information */
	NULL				/* Statistics */
};

static struct streamtab s_info = {
	&s_rinit,			/* Upper read queue */
	&s_winit,			/* Upper write queue */
	NULL,				/* Lower read queue */
	NULL				/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  SSCOP Private Structure
 *
 *  =========================================================================
 */

typedef struct sscop {
	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	uint i_state;			/* interface state */
	ulong token;			/* my bind token */

	bufq_t rb;			/* receive buffer */
	bufq_t tb;			/* transmission buffer */
	bufq_t rtb;			/* retransmission buffer */

} sscop_t;

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *  FIXME: Much more to do here....
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

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
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
static int s_w_iocdata(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	return (4);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCDATA Handling
 *
 *  -------------------------------------------------------------------------
 */
static int s_w_iocdata(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	return (4);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
static int s_w_proto(queue_t * q, mblk_t * mp)
{
	int rtn;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	uint oldstate = sp->i_state;
	switch (*((long *) mp->b_rptr)) {
	case N_CONN_REQ:
		rtn = n_conn_req(sp, mp);
		break;
	case N_CONN_RES:
		rtn = n_conn_res(sp, mp);
		break;
	case N_DISCON_REQ:
		rtn = n_discon_req(sp, mp);
		break;
	case N_DATA_REQ:
		rtn = n_data_req(sp, mp);
		break;
	case N_EXDATA_REQ:
		rtn = n_exdata_req(sp, mp);
		break;
	case N_INFO_REQ:
		rtn = n_info_req(sp, mp);
		break;
	case N_BIND_REQ:
		rtn = n_bind_req(sp, mp);
		break;
	case N_UNBIND_REQ:
		rtn = n_unbind_req(sp, mp);
		break;
	case N_UNITDATA_REQ:
		rtn = n_unitdata_req(sp, mp);
		break;
	case N_OPTMGMT_REQ:
		rtn = n_optmgmt_req(sp, mp);
		break;
	case N_DATACK_REQ:
		rtn = n_datack_req(sp, mp);
		break;
	case N_RESET_REQ:
		rtn = n_reset_req(sp, mp);
		break;
	case N_RESET_RES:
		rtn = n_reset_res(sp, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		seldom();
		sp->i_state = oldstate;
	}
	return (rtn);
}
static int s_r_proto(queue_t q, mblk_t * mp)
{
	int rtn;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	switch (*((long *) mp->b_rptr)) {
	case N_INFO_ACK:
		rtn = l_info_ack(sp, mp);
		break;
	case N_BIND_ACK:
		rtn = l_bind_ack(sp, mp);
		break;
	case N_ERROR_ACK:
		rtn = l_error_ack(sp, mp);
		break;
	case N_OK_ACK:
		rtn = l_ok_ack(sp, mp);
		break;
	case N_UNITDATA_IND:
		rtn = l_unitdata_ind(sp, mp);
		break;
	case N_UDERROR_IND:
		rtn = l_uderror_ind(sp, mp);
		break;
	default:
		rtn = -EFAULT;
		break;
	}
	if (rtn < 0) {
		rare();
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
static int s_w_data(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	/* 
	 *  FIXME:  We have to attach an SSCOP trailer and place in retransmit
	 *  buffer.
	 */
	return (4);
}
static int s_r_data(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	/* 
	 *  FIXME:  We have to attach an SSCOP trailer and place in retransmit
	 *  buffer.
	 */
	return (4);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
static int s_w_flush(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	return (3);
}
static int s_r_flush(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	return (3);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
static int s_r_error(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	return (4);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 */
static int s_r_hangup(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	return (4);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages
 *
 *  -------------------------------------------------------------------------
 */
static int s_w_other(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	return (4);
}
static int s_r_other(queue_t * q, mblk_t * mp)
{
	(void) q;
	(void) mp;
	return (4);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
static INT s_wput(queue_t * q, mblk_t * mp)
{
	int rtn = 0;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	ensure(q, return ((INT) (-EFAULT)));
	ensure(sp, return ((INT) (-EFAULT)));
	ensure(mp, return ((INT) (-EFAULT)));
	switch (mp->b_datap->db_type) {
	case M_DATA:
		rtn = s_w_data(q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		rtn = s_w_proto(q, mp);
		break;
	case M_FLUSH:
		rtn = s_w_flush(q, mp);
		break;
	case M_IOCTL:
		rtn = s_w_ioctl(q, mp);
		break;
	case M_IOCDATA:
		rtn = s_w_iocdata(q, mp);
		break;
	default:
		rtn = s_w_other(q, mp);
		break;
	}
	switch (rtn) {
	case 0:
		freemsg(mp);
	case 1:
		break;
	case 2:
		freeb(mp);
		break;
	case 3:
	case 4:
		putnext(q, mp);
		break;
	default:
		ptrace(("Error = %d\n", rtn));
		freemsg(mp);
		s_tx_wakeup(sp);
		return (INT) (rtn);
	}
	s_tx_wakeup(sp);
	return (INT) (0);
}

static INT s_rput(queue_t * q, mblk_t * mp)
{
	int rtn = 0;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	ensure(q, return ((INT) (-EFAULT)));
	ensure(sp, return ((INT) (-EFAULT)));
	ensure(mp, return ((INT) (-EFAULT)));
	switch (mp->b_datap->db_type) {
	case M_DATA:
		rtn = s_r_data(q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		rtn = s_r_proto(q, mp);
		break;
	case M_CTL:
		rtn = s_r_ctrl(q, mp);
		break;
	case M_ERROR:
		rtn = s_r_error(q, mp);
		break;
	case M_HANGUP:
		rtn = s_r_hangup(q, mp);
		break;
	case M_FLUSH:
		rtn = s_r_flush(q, mp);
		break;
	default:
		rtn = s_r_other(q, mp);
		break;
	}
	switch (rtn) {
	case 0:
		freemsg(mp);
	case 1:
		break;
	case 2:
		freeb(mp);
		break;
	case 3:
	case 4:
		putnext(q, mp);
		break;
	default:
		ptrace(("Error = %d\n", rtn));
		freemsg(mp);
		s_rx_wakeup(sp);
		s_tx_wakeup(sp);
		return (INT) (rtn);
	}
	s_rx_wakeup(sp);
	s_tx_wakeup(sp);
	return (INT) (0);
}

/*
 *  =========================================================================
 *
 *  Private structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
kmem_cache_t *s_cachep = NULL;

static void s_init_caches(void)
{
	if (!(s_cachep) &&
	    !(s_cachep = kmem_cache_create
	      ("sscop_cachep", sizeof(sscop_t), 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
		panic("%s:Cannot alloc s_cachep.\n", __FUNCTION__);
	return;
}
static void s_term_caches(void)
{
	if (s_cachep)
		if (kmem_cache_destroy(s_cachep))
			cmn_err(CE_WARN, "%s: did not destroy s_cache", __FUNCTION__);
	return;
}
static sscop_t *s_alloc_priv(queue_t * q)
{
	sscop_t *sp;
	if ((sp = kmem_cache_alloc(s_cachep, SLAB_ATOMIC))) {
		bzero(sp, sizeof(*sp));
		RD(q)->q_ptr = WR(q)->q_ptr = sp;
		sp->rq = RD(q);
		sp->wq = WR(q);
		bufq_init(&sp->rb);
		bufq_init(&sp->tb);
		bufq_init(&sp->rtb);
	}
	return (sp);
}
static void s_free_priv(queue_t * q)
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	bufq_purge(&sl->rb);
	bufq_purge(&sl->tb);
	bufq_purge(&sl->rtb);
	kmem_cache_free(s_cachep, sp);
	return;
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
static int s_open(queue_t * q, dev_t * devp, int flag, int sflag, cred_t * crp)
{
	(void) crp;
	if (q->q_ptr != NULL)	/* already open */
		return (0);
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		fixme(("Check module we are being pushed over.\n"));
		if (!(s_alloc_priv(q)))
			return ENOMEM;
		return (0);
	}
	return EIO;
}
static int s_close(queue_t * q, int flag, cred_t * crp)
{
	(void) flag;
	(void) crp;
	s_free_priv(q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
void s_init(void)
{
	int modnum;
	unless(s_minfo.mi_idnum, return);
	cmn_err(CE_NOTE, SSCOP_BANNER);	/* console splash */
	s_init_caches();
	if (!(modnum = lis_register_strmod(&s_info, s_minfo.mi_idname))) {
		s_minfo.mi_idnum = 0;
		rare();
		cmn_err(CE_NOTE, "sscop: couldn't register as module\n");
		return;
	}
	s_minfo.mi_idnum = modnum;
	return;
}
void s_terminate(void)
{
	ensure(s_minfo.mi_idnum, return);
	if ((s_minfo.mi_idnum = lis_unregister_strmod(&s_minfo)))
		cmn_err(CE_WARN, "sscop: couldn't unregister module!\n");
	s_term_caches();
	return;
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int init_module(void)
{
	s_init();
	return (0);
}
void cleanup_module(void)
{
	s_terminate();
	return;
}
