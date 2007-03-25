/*****************************************************************************

 @(#) $RCSfile: sscop2.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:13:08 $

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

 Last Modified $Date: 2006/03/07 01:13:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sscop2.c,v $
 Revision 0.9.2.11  2006/03/07 01:13:08  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop2.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:13:08 $"

static char const ident[] =
    "$RCSfile: sscop2.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/03/07 01:13:08 $";

#include <sys/os7/compat.h>

#include <sys/npi.h>
#include <sys/npi_ip.h>

#define SSCOP_DESCRIP	"SSCOP STREAMS MODULE."
#define SSCOP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SSCOP_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
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
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sscop");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SSCOP_DRV_ID		CONFIG_STREAMS_SSCOP_MODID
#define SSCOP_DRV_NAME		CONFIG_STREAMS_SSCOP_NAME
#define SSCOP_CMAJORS		CONFIG_STREAMS_SSCOP_NMAJORS
#define SSCOP_CMAJOR_0		CONFIG_STREAMS_SSCOP_MAJOR
#endif				/* LFS */

#ifndef SSCOP_DRV_NAME
#define SSCOP_DRV_NAME		"sscop"
#endif				/* SSCOP_DRV_NAME */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		SSCOP_DRV_ID
#define DRV_NAME	SSCOP_DRV_NAME
#ifdef MODULE
#define DRV_BANNER	SSCOP_BANNER
#else				/* MODULE */
#define DRV_BANNER	SSCOP_SPLASH
#endif				/* MODULE */

static struct module_info s_minfo = {
	DRV_ID,				/* Module ID number */
	DRV_NAME,			/* Module name */
	0,				/* Min packet size accepted */
	INFPSZ,				/* Max packet size accepted */
	1 << 15,			/* Hi water mark */
	1 << 10				/* Lo water mark */
};

static int s_open(queue_t *, dev_t *, int, int, cred_t *);
static int s_close(queue_t *, int, cred_t *);

static int s_rput(queue_t *, mblk_t *);

static struct qinit s_rinit = {
	s_rput,				/* Read put (msg from below) */
	NULL,				/* Read queue service */
	s_open,				/* Each open */
	s_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&s_minfo,			/* Information */
	NULL				/* Statistics */
};

static int s_wput(queue_t *, mblk_t *);

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
static int
s_w_iocdata(queue_t *q, mblk_t *mp)
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
static int
s_w_iocdata(queue_t *q, mblk_t *mp)
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
static int
s_w_proto(queue_t *q, mblk_t *mp)
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
static int
s_r_proto(queue_t q, mblk_t *mp)
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
static int
s_w_data(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/* 
	 *  FIXME:  We have to attach an SSCOP trailer and place in retransmit
	 *  buffer.
	 */
	return (4);
}
static int
s_r_data(queue_t *q, mblk_t *mp)
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
static int
s_w_flush(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (3);
}
static int
s_r_flush(queue_t *q, mblk_t *mp)
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
static int
s_r_error(queue_t *q, mblk_t *mp)
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
static int
s_r_hangup(queue_t *q, mblk_t *mp)
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
static int
s_w_other(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (4);
}
static int
s_r_other(queue_t *q, mblk_t *mp)
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
static int
s_wput(queue_t *q, mblk_t *mp)
{
	int rtn = 0;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	ensure(q, return (-EFAULT));
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
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
		return (rtn);
	}
	s_tx_wakeup(sp);
	return (0);
}

static int
s_rput(queue_t *q, mblk_t *mp)
{
	int rtn = 0;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	ensure(q, return (-EFAULT));
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
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
		return (rtn);
	}
	s_rx_wakeup(sp);
	s_tx_wakeup(sp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
kmem_cache_t *s_cachep = NULL;

static void
s_init_caches(void)
{
	if (!(s_cachep) &&
	    !(s_cachep = kmem_cache_create
	      ("sscop_cachep", sizeof(sscop_t), 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
		panic("%s:Cannot alloc s_cachep.\n", __FUNCTION__);
	return;
}
static void
s_term_caches(void)
{
	if (s_cachep)
		if (kmem_cache_destroy(s_cachep))
			cmn_err(CE_WARN, "%s: did not destroy s_cache", __FUNCTION__);
	return;
}
static sscop_t *
s_alloc_priv(queue_t *q)
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
static void
s_free_priv(queue_t *q)
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
static int
s_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
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
static int
s_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	s_free_priv(q);
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
MODULE_PARM_DESC(modid, "Module ID for the SSCOP module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw aa_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sscopinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
aa_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&aa_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
aa_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&aa_fmod)) < 0)
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
aa_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&sscopinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&sscopinfo);
		return (err);
	}
	return (0);
}

STATIC int
aa_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&sscopinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sscopinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = aa_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = aa_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		aa_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sscopterminate(void)
{
	int err;
	if ((err = aa_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = aa_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sscopinit);
module_exit(sscopterminate);

#endif				/* LINUX */
