/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 * X25MOD - XX25 Module: This is an X.25 module that is pushed over a X.25-PLP stream supporting the
 * Network Provider Interface.  It implements the TPI as it is used to support the OpenGroup XX25
 * XTI interface.  As it is not expected that the XX25 interface will experience much use, I thought
 * it unnecessary to clutter the X.25-PLP drivers with supporting both an NPI and a TPI interface.
 * Thus this module.  This module should be used in an autopush(8) specification to be autopushed on
 * a minor device number on the X.25-PLP driver.
 */

#include <sys/os7/compat.h>

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/npi.h>

#define XX25_DESCRIP	"UNIT SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define XX25_COPYRIGHT	"Copyright (c) 1997-2007  OpenSS7 Corporation.  All Rights Reserved."
#define XX25_REVISION	"OpenSS7 $RCSfile$ $Name$($Revision$) $Date$"
#define XX25_DEVICE	"SVR 4.2 STREAMS XX25 Modle for X.25-PLP (X25MOD)"
#define XX25_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define XX25_LICENSE	"GPL"
#define XX25_BANNER	XX25_DESCRIP	"\n" \
			XX25_COPYRIGHT	"\n" \
			XX25_REVISION	"\n" \
			XX25_DEVICE	"\n" \
			XX25_CONTACT
#define XX25_SPLASH	XX25_DEVICE	" - " \
			XX25_REVISION

#ifdef LINUX
MODULE_AUTHOR(XX25_CONTACT);
MODULE_DESCRIPTION(XX25_DESCRIP);
MODULE_SUPPORTED_DEVICE(XX25_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(XX25_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-xx25");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef XX25_MOD_NAME
#define XX25_MOD_NAME		"xx25"
#endif				/* XX25_MOD_NAME */
#ifndef XX25_MOD_ID
#define XX25_MOD_ID		0
#endif				/* XX25_MOD_ID */

/*
 * STREAMS Definitions
 */

#define MOD_ID		XX25_MOD_ID
#define MOD_NAME	XX25_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	XX25_BANNER
#else				/* MODULE */
#define MOD_BANNER	XX25_SPLASH
#endif				/* MODULE */

static struct module_info xx25_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = 0,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat xx25_rstat __attribute__ ((aligned(SMP_CACHE_BYTES)));
static struct module_stat xx25_wstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static const char *
tp_primname(t_uscalar_t prim)
{
	switch (prim) {
	case T_CONN_REQ:
		return ("T_CONN_REQ");
	case T_CONN_RES:
		return ("T_CONN_RES");
	case T_DISCON_REQ:
		return ("T_DISCON_REQ");
	case T_DATA_REQ:
		return ("T_DATA_REQ");
	case T_EXDATA_REQ:
		return ("T_EXDATA_REQ");
	case T_INFO_REQ:
		return ("T_INFO_REQ");
	case T_BIND_REQ:
		return ("T_BIND_REQ");
	case T_UNBIND_REQ:
		return ("T_UNBIND_REQ");
	case T_UNITDATA_REQ:
		return ("T_UNITDATA_REQ");
	case T_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ");
	case T_ORDREL_REQ:
		return ("T_ORDREL_REQ");
	case T_OPTDATA_REQ:
		return ("T_OPTDATA_REQ");
	case T_ADDR_REQ:
		return ("T_ADDR_REQ");
	case T_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case T_CONN_IND:
		return ("T_CONN_IND");
	case T_CONN_CON:
		return ("T_CONN_CON");
	case T_DISCON_IND:
		return ("T_DISCON_IND");
	case T_DATA_IND:
		return ("T_DATA_IND");
	case T_EXDATA_IND:
		return ("T_EXDATA_IND");
	case T_INFO_ACK:
		return ("T_INFO_ACK");
	case T_BIND_ACK:
		return ("T_BIND_ACK");
	case T_ERROR_ACK:
		return ("T_ERROR_ACK");
	case T_OK_ACK:
		return ("T_OK_ACK");
	case T_UNITDATA_IND:
		return ("T_UNITDATA_IND");
	case T_UDERROR_IND:
		return ("T_UDERROR_IND");
	case T_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK");
	case T_ORDREL_IND:
		return ("T_ORDREL_IND");
	case T_OPTDATA_IND:
		return ("T_OPTDATA_IND");
	case T_ADDR_ACK:
		return ("T_ADDR_ACK");
	case T_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
	default:
		return ("T_????");
	}
}

static const char *
np_primname(np_ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return ("N_CONN_REQ");
	case N_CONN_RES:
		return ("N_CONN_RES");
	case N_DISCON_REQ:
		return ("N_DISCON_REQ");
	case N_DATA_REQ:
		return ("N_DATA_REQ");
	case N_EXDATA_REQ:
		return ("N_EXDATA_REQ");
	case N_INFO_REQ:
		return ("N_INFO_REQ");
	case N_BIND_REQ:
		return ("N_BIND_REQ");
	case N_UNBIND_REQ:
		return ("N_UNBIND_REQ");
	case N_UNITDATA_REQ:
		return ("N_UNITDATA_REQ");
	case N_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ");
	case N_CONN_IND:
		return ("N_CONN_IND");
	case N_CONN_CON:
		return ("N_CONN_CON");
	case N_DISCON_IND:
		return ("N_DISCON_IND");
	case N_DATA_IND:
		return ("N_DATA_IND");
	case N_EXDATA_IND:
		return ("N_EXDATA_IND");
	case N_INFO_ACK:
		return ("N_INFO_ACK");
	case N_BIND_ACK:
		return ("N_BIND_ACK");
	case N_ERROR_ACK:
		return ("N_ERROR_ACK");
	case N_OK_ACK:
		return ("N_OK_ACK");
	case N_UNITDATA_IND:
		return ("N_UNITDATA_IND");
	case N_UDERROR_IND:
		return ("N_UDERROR_IND");
	case N_DATACK_REQ:
		return ("N_DATACK_REQ");
	case N_DATACK_IND:
		return ("N_DATACK_IND");
	case N_RESET_REQ:
		return ("N_RESET_REQ");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_RESET_CON:
		return ("N_RESET_CON");
	default:
		return ("N_????");
	}
}

/*
 * Primitives issued upstream (TPI TS-Provider Primitives)
 * --------------------------------------------------------------------------
 * For all of these functions @xp is a pointer to the PLP private structure, @q is a pointer to the
 * active queue, @bp is a pointer to a message to be freed upon success (if not NULL).  Additional
 * parameters are primitive specific.
 */
static int
m_error(struct xp *xp, queue_t *q, mblk_t *bp, int rerr, int werr)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		mp->b_wptr += 2;
		mp->b_rptr[0] = rerr;
		mp->b_rptr[1] = werr;
		xp_set_n_state(xp, 0);
		xp_set_t_state(xp, 0);
		putnext(xp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static int
tp_conn_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_conn_con(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_discon_ind(struct xp *xp, queue_t *q, mblk_t *bp, np_ulong RES_offset, np_ulong RES_length,
	      np_ulong DISCON_orig, np_ulong DISCON_reason, np_ulong SEQ_number, mblk_t *dp)
{
	struct T_discon_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (pcmsg(DB_TYPE(bp)) || bcanputnext(q, bp->b_band)) {
			DB_TYPE(mp) = DB_TYPE(bp);
			mp->b_band = bp->b_band;
			p = (typeof(p)) mp->b_rptr;
			mp->b_wptr = mp->b_rptr + sizeof(*p);
			p->PRIM_type = T_DISCON_IND;
			p->DISCON_reason = T_X25_SET_CAUSE_DIAG(DISCON_orig, DISCON_reason);
			p->SEQ_number = SEQ_number;
			mp->b_cont = dp;
			if (bp->b_cont == dp)
				bp->b_cont = NULL;
			freemsg(bp);
			/* XXX: need to save these two for later, they can be retrieved with
			   options management. */
			/* For T_X25_DISCON_ADD */
			bcopy(bp->b_rptr + RES_offset, xp->RES_address, RES_length);
			xp->RES_length = RES_length;
			/* For T_X25_DISCON_REASON */
			xp->DISCON_reason = DISCON_reason;

			xp_set_n_state(xp, NS_IDLE);
			xp_set_t_state(xp, NS_IDLE);
			putnext(q, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static int
tp_data_ind(struct xp *xp, queue_t *q, mblk_t *bp, np_ulong DATA_xfer_flags, mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(pcmsg(DB_TYPE(bp)) || bcanputnext(xp->rq, bp->b_band))) {
			DB_TYPE(mp) = DB_TYPE(bp);
			mp->b_band = bp->b_band;
			p = (typeof(p)) mp->b_rptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = 0;
			if (DATA_xfer_flags & N_MORE_DATA_FLAG)
				p->MORE_flag |= T_MORE;
			if (DATA_xfer_flags & T_X25_D)
				p->MORE_flag |= T_X25_D;
			mp->b_cont = dp;
			if (bp->b_cont == dp)
				bp->b_cont = NULL;
			freemsg(bp);
			putnext(xp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static int
tp_exdata_ind(struct xp *xp, queue_t *q, mblk_t *bp, mblk_t *dp)
{
	struct T_exdata_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(pcmsg(DB_TYPE(bp)) || bcanputnext(xp->rq, bp->b_band))) {
			DB_TYPE(mp) = DB_TYPE(bp);
			mp->b_band = bp->b_band;
			p = (typeof(p)) mp->b_rptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_EXDATA_IND;
			p->MORE_flag = 0;
			mp->b_cont = dp;
			if (bp->b_cont == dp)
				bp->b_cont = NULL;
			freemsg(bp);
			putnext(xp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static int
tp_info_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_bind_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_error_ack(struct xp *xp, queue_t *q, mblk_t *bp, t_scalar_t prim, t_scalar_t errno)
{
	struct T_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_rptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = errno < 0 ? TSYSERR : errno;
		p->UNIX_error = errno < 0 ? -errno : 0;
		freemsg(bp);
		putnext(xp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static int
tp_ok_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_unitdata_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_uderror_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_optmgmt_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_ordrel_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_optdata_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_addr_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_capability_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
tp_other_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
}

/*
 * Primitives issued downstream (NPI NS-User Primitives)
 * --------------------------------------------------------------------------
 * For all of these functions @xp is a pointer to the PLP private structure, @q is a pointer to the
 * active queue, @bp is a pointer to a message to be freed upon success (if not NULL).  Additional
 * parameters are primitive specific.
 */
static int
np_conn_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_conn_res(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_discon_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_data_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_exdata_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_info_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_bind_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_unbind_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_unitdata_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_optmgmt_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}
static int
np_other_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
}

/*
 * Primitives receives from upstream (TPI TS-User Primitives)
 * --------------------------------------------------------------------------
 * For all these functions @xp is a pointer to the locked PLP private structure, @q is a pointer to
 * the active queue, @bp is a pointer to the message being processed.
 */
static int
tp_conn_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_conn_req *p = (typeof(p)) bp->b_rptr;
	struct T_ok_ack *o;
	struct t_opthdr *t;
	N_conn_req_t *r;
	mblk_t *mp = NULL, *rp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badproto;
	if (!MBLKIN(bp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(bp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (!(mp = mi_allocb(q, sizeof(*r) + p->DEST_length, BPRI_MED)))
		goto enobufs;
	if (!(rp = mi_allocb(q, sizeof(*o), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_CONN_REQ;
	r->DEST_length = p->DEST_length;
	r->DEST_offset = sizeof(*r);
	r->CONN_flags = DEFAULT_RC_SEL;
	r->QOS_length = sizeof(*qos);
	r->QOS_offset = PAD4(p->DEST_offset + p->DEST_length);
	bcopy(bp->b_rptr + p->DEST_offset, mp->b_wptr, p->DEST_length);
	mp->b_wptr = mp->b_rptr + r->QOS_offset;
	qos = (typeof(qos)) mp->b_wptr;
	mp->b_wptr += sizeof(*qos);
	bzero(qos, sizeof(*qos));
	qos->n_qos_type = N_QOS_CO_X25_RANGE;
	for (t = _T_OPT_FIRSTHDR_OFS(bp->b_rptr, p->OPT_length, p->OPT_offset);
	     t; t = _T_OPT_NEXTHDR_OFS(bp->b_rptr, p->OPT_length, t, p->OPT_offset)) {
		if (t->len < sizeof(*t))
			goto badopt;
		switch (t->level) {
		case XTI_GENERIC:
			break;
		case T_X25_NP:
			switch (t->name) {
			case T_X25_USER_DACK:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags &= ~DEFAULT_RC_SEL;
					break;
				case T_NO:
					r->CONN_flags |= DEFAULT_RC_SEL;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_USER_EACK:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags &= ~DEFAULT_RC_SEL;
					break;
				case T_NO:
					r->CONN_flags |= DEFAULT_RC_SEL;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_D_OPT:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags |= REC_CONF_OPT;
					break;
				case T_NO:
					r->CONN_flags &= ~REC_CONF_OPT;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_EDN:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags |= EX_DATA_OPT;
					break;
				case T_NO:
					r->CONN_flags &= ~EX_DATA_OPT;
					break;
				default:
					goto badopt;
				}
				break;
			}
			break;
		}
	}
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	/* TPI needs an T_OK_ACK in response to T_CONN_REQ. */
	DB_TYPE(rp) = M_PCPROTO;
	o = (typeof(o)) rp->b_rptr;
	rp->b_wptr += sizeof(*o);
	o->PRIM_type = T_OK_ACK;
	o->CORRECT_prim = T_CONN_REQ;
	xp->chkstate = xp_get_n_state(xp);
	xp_set_t_state(xp, TS_WACK_CREQ);
	qreply(q, rp);
	xp_set_n_state(xp, NS_WCON_CREQ);
	xp_set_t_state(xp, TS_WCON_CREQ);
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	freemsg(mp); /* freemsg can handle NULLs */
	freemsg(rp); /* freemsg can handle NULLs */
	return (err);
}
static int
tp_conn_res(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_conn_res *p = (typeof(p)) bp->b_rptr;
	N_conn_res_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!MBLKIN(bp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (!(mp = mi_allocb(q, sizeof(*r) + xp->LOCADDR_length + sizeof(*qos), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_CONN_RES;
	r->TOKEN_value = p->ACCEPTOR_id;
	r->RES_length = xp->LOCADDR_length;
	r->RES_offset = xp->LOCADDR_length ? sizeof(*r) : 0;
	r->SEQ_number = p->SEQ_number;
	r->CONN_flags = DEFAULT_RC_SEL;
	r->QOS_length = sizeof(*qos);
	r->QOS_offset = PAD4(p->LOCADDR_length + sizeof(*r));
	bcopy(xp->LOCADDR_address, mp->b_rptr, xp->LOCADDR_length);
	mp->b_wptr = mp->b_rptr + r->QOS_offset;
	qos = (typeof(qos)) mp->b_wptr;
	mp->b_wptr += sizeof(qos);
	bzero(qos, sizeof(*qos));
	qos->n_qos_type = N_QOS_CO_X25_SEL;
	for (t = _T_OPT_FIRSTHDR_OFS(bp->b_rptr, p->OPT_length, p->OPT_offset);
	     t; t = _T_OPT_NEXTHDR_FS(bp->b_rptr, p->OPT_length, p->OPT_offset)) {
		if (t->len < sizeof(*t))
			goto badopt;
		switch (t->level) {
		case XTI_GENERIC:
			break;
		case T_X25_NP:
			switch (t->name) {
			case T_X25_USER_DACK:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags &= ~DEFAULT_RC_SEL;
					break;
				case T_NO:
					r->CONN_flags |= DEFAULT_RC_SEL;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_USER_EACK:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags &= ~DEFAULT_RC_SEL;
					break;
				case T_NO:
					r->CONN_flags |= DEFAULT_RC_SEL;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_D_OPT:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags |= REC_CONF_OPT;
					break;
				case T_NO:
					r->CONN_flags &= ~REC_CONF_OPT;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_EDN:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags |= EX_DATA_OPT;
					break;
				case T_NO:
					r->CONN_flags &= ~EX_DATA_OPT;
					break;
				default:
					goto badopt;
				}
				break;
			}
			break;
		}
	}
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	xp->chkstate = xp_get_n_state(xp);
	xp_set_n_state(xp, NS_WACK_CRES);
	xp_set_t_state(xp, TS_WACK_CRES);
	putnext(q, mp);
	return (0);
      badprim:
	err = -EPROTO;
	goto terror;
      badopt:
	err = TBADOPT;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_CONN_RES, err);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_discon_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_discon_req *p = (typeof(p)) bp->b_rptr;
	N_discon_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r) + xp->LOCADDR_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_DISCON_REQ;
	r->DISCON_reason = xp->DISCON_reason;
	r->RES_length = xp->LOCADDR_length;
	r->RES_offset = xp->LOCADDR_length ? sizeof(*r) : 0;
	r->SEQ_number = p->SEQ_number;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	bcopy(xp->LOCADDR_address, mp->b_wptr, xp->LOCADDR_length);
	mp->b_wptr += xp->LOCADDR_length;
	freeb(bp);
	xp->chkstate = xp_get_n_state(xp);
	switch (xp_get_n_state(xp)) {
	case NS_WCON_CREQ:
		xp_set_n_state(xp, NS_WACK_DREQ6);
	case NS_WRES_CIND:
		xp_set_n_state(xp, NS_WACK_DREQ7);
	default:
	case NS_DATA_XFER:
		xp_set_n_state(xp, NS_WACK_DREQ9);
	case NS_WCON_RREQ:
		xp_set_n_state(xp, NS_WACK_DREQ10);
	case NS_WRES_RIND:
		xp_set_n_state(xp, NS_WACK_DREQ11);
	}
	putnext(q, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_data_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_data_req *p = (typeof(p)) bp->b_rptr;
	mblk_t *mp;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (p->MORE_flag & T_X25_RST) {
		N_reset_req_t *r;

		if (p->MORE_flag & ~T_X25_RST)
			goto badflag;
		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = N_RESET_REQ;
		r->RESET_reason = 0;
		xp->chkstate = xp_get_n_state(xp);
		xp_set_n_state(xp, NS_WCON_RREQ);
	} else if (p->MORE_flag & (T_X25_DACK | T_X25_EACK)) {
		N_datack_req_t *r;

		if (p->MORE_flag & ~(T_X25_DACK | T_X25_EACK))
			goto badflag;
		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = N_DATACK_REQ;
	} else {
		N_data_req_t *r;

		if (p->MORE_flag & ~(T_MORE | T_X25_Q | T_X25_D))
			goto badflag;
		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = N_DATA_REQ;
		r->DATA_xfer_flags = 0;
		if (p->MORE_flag & T_MORE)
			r->DATA_xfer_flags |= N_MORE_DATA_FLAG;
		if (p->MORE_flag & T_X25_D)
			r->DATA_xfer_flags |= N_RC_FLAG;
	}
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	putnext(q, mp);
	return (0);
      badprim:
	err = EPROTO;
	goto merror;
      badflag:
	err = EPROTO;
	goto merror;
      merror:
	return m_error(xp, q, bp, err, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_exdata_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_exdata_req *p = (typeof(p)) bp->b_rptr;
	N_exdata_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (p->MORE_flag != 0)
		goto badflag;
	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_EXDATA_REQ;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	putnext(q, mp);
	return (0);
      badprim:
	err = EPROTO;
	goto merror;
      badflag:
	err = EPROTO;
	goto merror;
      merror:
	return m_error(xp, q, bp, err, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_info_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_info_req *p = (typeof(p)) bp->b_rptr;
	N_info_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_INFO_REQ;
	freemsg(bp);
	xp->wack_ireq++;
	putnext(q, mp);
	return (0);
      badprim:
	err = EPROTO;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_INFO_REQ, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_bind_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_bind_req *p = (typeof(p)) bp->b_rptr;
	N_bind_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!MBLKIN(bp, p->ADDR_offset, p->ADDR_length))
		goto badaddr;
	if (!(mp = mi_allocb(q, sizeof(*r) + p->ADDR_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_BIND_REQ;
	r->ADDR_length = p->ADDR_length;
	r->ADDR_offset = p->ADDR_length ? sizeof(*r) : 0;
	r->CONIND_number = p->CONIND_number;
	r->BIND_flags = 0;
	r->PROTOID_length = 0;
	r->PROTOID_offset = 0;
	bcopy(bp->b_rptr + p->ADDR_offset, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	freemsg(bp);
	xp->chkstate = xp_get_n_state(xp);
	xp_set_n_state(xp, NS_WACK_BREQ);
	xp_set_t_state(xp, TS_WACK_BREQ);
	putnext(q, mp);
	return (0);
}
static int
tp_unbind_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_unbind_req *p = (typeof(p)) bp->b_rptr;
	N_unbind_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_UNBIND_REQ;
	freemsg(bp);
	xp->chkstate = xp_get_n_state(xp);
	xp_set_n_state(xp, NS_WACK_UREQ);
	xp_set_t_state(xp, TS_WACK_UREQ);
	putnext(q, mp);
	return (0);
}
static int
tp_unitdata_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	return tp_error_ack(xp, q, bp, T_UNITDATA_REQ, TNOTSUPPORT);
}
static int
tp_optmgmt_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_optmgmt_req *p = (typeof(p)) bp->b_rptr;
	N_optmgmt_req_t *r;
	mblk_t *mp;
}
static int
tp_ordrel_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_ordrel_req *p = (typeof(p)) bp->b_rptr;
	N_discon_req_t *r;
	mblk_t *mp;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	goto notsupport;
	if (!(mp = mi_allocb(q, sizeof(*r) + xp->LOCADDR_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_DISCON_REQ;
	r->DISCON_reason = 0;	/* FIXME */
	r->RES_length = xp->LOCADDR_length;
	r->RES_offset = xp->LOCADDR_length ? sizeof(*r) : 0;
	r->SEQ_number = 0;
	bcopy(xp->LOCADDR_address, mp->b_wptr, xp->LOCADDR_length);
	mp->b_wptr += xp->LOCADDR_length;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	xp->chkstate = xp_get_n_state(xp);
	switch (xp_get_n_state(xp)) {
	case NS_WCON_CREQ:
		xp_set_n_state(xp, NS_WACK_DREQ6);
	case NS_WRES_CIND:
		xp_set_n_state(xp, NS_WACK_DREQ7);
	default:
	case NS_DATA_XFER:
		xp_set_n_state(xp, NS_WACK_DREQ9);
	case NS_WCON_RREQ:
		xp_set_n_state(xp, NS_WACK_DREQ10);
	case NS_WRES_RIND:
		xp_set_n_state(xp, NS_WACK_DREQ11);
	}
	putnext(q, mp);
	return (0);
      badprim:
	err = -EPROTO;
	goto terror;
      notsupport:
	err = TNOTSUPPORT;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_ORDREL_REQ, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static int
tp_optdata_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_optdata_req *p = (typeof(p)) bp->b_rptr;
	N_optdata_req_t *r;
	mblk_t *mp;
}
static int
tp_addr_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_addr_req *p = (typeof(p)) bp->b_rptr;
	struct T_addr_ack *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r) + xp->LOCADDR_length + xp->REMADDR_length, BPRI_MED)))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_ADDR_ACK;
	r->LOCADDR_length = xp->LOCADDR_length;
	r->LOCADDR_offset = sizeof(*r);
	r->REMADDR_length = xp->REMADDR_length;
	r->REMADDR_offset = sizeof(*r) + xp->LOCADDR_length;
	bcopy(xp->LOCADDR_address, mp->b_wptr, xp->LOCADDR_length);
	mp->b_wptr += xp->LOCADDR_length;
	bcopy(xp->REMADDR_address, mp->b_wptr, xp->REMADDR_length);
	mp->b - wptr += xp->REMADDR_length;
	freemsg(bp);
	qreply(q, mp);
	return (0);
      badprim:
	err = -EPROTO;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_ADDR_REQ, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_capability_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_capability_req *p = (typeof(p)) bp->b_rptr;
	N_info_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_INFO_REQ;
	freemsg(bp);
	xp->wack_creq++;
	putnext(q, mp);
	return (0);
      badprim:
	err = EPROTO;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_INFO_REQ, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_other_req(struct xp *xp, queue_t *q, mblk_t *bp)
{
	return tp_error_ack(xp, q, bp, *(t_uscalar_t *)bp->b_rptr, TNOTSUPPORT);
}

/*
 * Primitives receive from downstream (NPI NS-Proivder Primitives)
 * --------------------------------------------------------------------------
 * For all these functions @xp is a pointer to the locked PLP private structure, @q is a pointer to
 * the active queue, @bp is a pointer to the message being processed.
 *
 * These pretty much trust the driver state machine in that it assumes that primitives are only
 * issued by the NS-provider in the correct state.  We also trust that the NS-Provider issues
 * correctly formatted primitives.  Primitives are simply translated to the corresponding TPI
 * primitives and passed upstream.
 */
static int
np_conn_con(struct xp *xp, queue_t *q, mblk_t *bp)
{
	N_conn_con_t *p = (typeof(p)) bp->b_rptr;
	struct t_opthdr *t;
	struct T_conn_con *r;
	size_t olen;
	mblk_t *mp;

	/* Translate RES_length and RES_offset, responding address */
	/* Translate QOS_length, QOS_offset and CONN_flags into OPT_length and OPT_offset */
	olen = xx25_size_options((caddr_t) p + p->QOS_offset, p->QOS_length, p->CONN_flags);
	if (!(mp = mi_allocb(q, olen + sizeof(*r) + p->RES_length, BPRI_MED)))
		goto enobufs;
	if (!bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr = mp->b_rptr + sizeof(*r);
	r->PRIM_type = T_CONN_CON;
	r->RES_length = p->RES_length;
	r->RES_offset = sizeof(*r);
	r->OPT_length = olen;
	r->OPT_offset = sizeof(*r) + p->RES_length;
	bcopy((caddr_t) p + p->RES_offset, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	t = (typeof(t)) mp->b_wptr;
	t->len = sizeof(*t) + sizeof(t_scalar_t);
	t->level = T_X25_NP;
	t->name = T_X25_D_OPT;
	t->state = T_SUCCESS;
	*(t_scalar_t *)t->value = (p->CONN_flags & REC_CONF_OPT) ? T_YES : T_NO;
	mp->b_wptr += t->len;
	t = (typeof(t)) mp->b_wptr;
	t->len = sizeof(*t) + sizeof(t_scalar_t);
	t->level = T_X25_NP;
	t->name = T_X25_EDN;
	t->state = T_SUCCESS;
	*(t_scalar_t *)t->value = (p->CONN_flags & EX_DATA_OPT) ? T_YES : T_NO;

	xx25_build_options(mp, (caddr_t) p + p->QOS_offset, p->QOS_length, p->CONN_flags);
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	xp_set_n_state(xp, NS_DATA_XFER);
	xp_set_t_state(xp, TS_DATA_XFER);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_discon_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;

	return tp_discon_ind(xp, q, mp, p->RES_offset, p->RES_length, p->DISCON_orig,
			     p->DISCON_reason, p->SEQ_number, mp->b_cont);
}
static int
np_data_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;

	return tp_data_ind(xp, q, mp, p->DATA_xfer_flags, mp->b_cont);
}
static int
np_exdata_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;

	return tp_exdata_ind(xp, q, mp, mp->b_cont);
}
/**
 * np_info_ack: - process N_INFO_ACK primitive
 * @xp: X.25 private structure
 * @q: active queue (lower read queue)
 * @bp: the N_INFO_ACK message
 *
 * This primitive must do double duty: one each in response to T_INFO_REQ or T_CAPABILITY_REQ.  Each
 * of these translates into a N_INFO_REQ on the downward path.  We mark in the upward path whether
 * we are interested in a T_INFO_ACK or T_CAPABILITY_ACK so that the appropriate responses can be
 * given.
 */
static int
np_info_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
	N_info_ack_t *p = (typeof(p)) bp->b_rptr;

	if (xp->wack_ireq) {
		struct T_info_ack *r;

		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		DB_TYPE(mp) = DB_TYPE(bp);
		mp->b_band = bp->b_band;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = T_INFO_ACK;
		r->TSDU_size = p->NSDU_size;
		r->ETSDU_size = p->ENSDU_size;
		r->CDATA_size = p->CDATA_size;
		r->DDATA_size = p->DDATA_size;
		r->ADDR_size = p->ADDR_size;
		r->OPT_size = FIXME;
		r->TIDU_size = p->NIDU_size;
		r->SERV_type = T_COTS;
		r->CURRENT_state = xx25_map_state(p->CURRENT_state);
		r->PROVIDER_flag = SNDZERO | XPG4_1;
		freemsg(bp);
		putnext(q, mp);
		xp->wack_ireq--;
	} else if (xp->wack_creq) {
		struct T_capability_ack *r;

		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		DB_TYPE(mp) = DB_TYPE(bp);
		mp->b_band = bp->b_band;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = T_CAPABILITY_ACK;
		/* always reply with both regardless of request */
		r->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		r->INFO_ack.PRIM_type = T_INFO_ACK;
		r->INFO_ack.TSDU_size = p->NSDU_size;
		r->INFO_ack.ETSDU_size = p->ENSDU_size;
		r->INFO_ack.CDATA_size = p->CDATA_size;
		r->INFO_ack.DDATA_size = p->DDATA_size;
		r->INFO_ack.ADDR_size = p->ADDR_size;
		r->INFO_ack.OPT_size = FIXME;
		r->INFO_ack.TIDU_size = p->NIDU_size;
		r->INFO_ack.SERV_type = T_COTS;
		r->INFO_ack.CURRENT_state = xx25_map_state(p->CURRENT_state);
		r->INFO_ack.PROVIDER_flag = SNDZERO | XPG4_1;
		/* We just happen to know this. */
		r->ACCEPTOR_id = (t_uscalar_t) (long) OTHERQ(q)->q_next;
		freemsg(bp);
		putnext(q, mp);
		xp->wack_creq--;
	} else {
		freemsg(bp);
	}
	return (0);
}
static int
np_bind_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
	N_bind_ack_t *p = (typeof(p)) bp->b_rptr;
	struct T_bind_ack *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r) + p->ADDR_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_BIND_ACK;
	r->ADDR_length = p->ADDR_length;
	r->ADDR_offset = sizeof(*r);
	r->CONIND_number = p->CONIND_number;
	bcopy(bp->b_rptr + p->ADDR_offset, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	xp_set_n_state(NS_IDLE);
	xp_set_t_state(TS_IDLE);
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_error_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
	N_error_ack_t *p = (typeof(p)) bp->b_rptr;
	struct T_error_ack *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_ERROR_ACK;
	switch (p->ERROR_prim) {
	case N_BIND_REQ:
		r->ERROR_prim = T_BIND_REQ;
		break;
	case N_UNBIND_REQ:
		r->ERROR_prim = T_UNBIND_REQ;
		break;
	case N_CONN_REQ:
		r->ERROR_prim = T_CONN_REQ;
		break;
	case N_CONN_RES:
		r->ERROR_prim = T_CONN_RES;
		break;
	case N_DATA_REQ:
		r->ERROR_prim = T_DATA_REQ;
		break;
	case N_EXDATA_REQ:
		r->ERROR_prim = T_EXDATA_REQ;
		break;
	case N_DISCON_REQ:
		r->ERROR_prim = T_DISCON_REQ;
		break;
	case N_RESET_REQ:
		r->ERROR_prim = T_DATA_REQ;
		break;
	case N_RESET_RES:
		r->ERROR_prim = T_DATA_REQ;
		break;
	case N_INFO_REQ:
		if (xp->wack_ireq) {
			r->ERROR_prim = T_INFO_REQ;
			xp->wack_ireq--;
		} else if (xp->wack_creq) {
			r->ERROR_prim = T_CAPABILITY_REQ;
			xp->wack_creq--;
		} else {
			r->ERROR_prim = T_INFO_REQ;
		}
		break;
	case N_UNITDATA_REQ:
		r->ERROR_prim = T_UNITDATA_REQ;
		break;
	case N_OPTMGMT_REQ:
		r->ERROR_prim = T_OPTMGMT_REQ;
		break;
	case N_DATACK_REQ:
		r->ERROR_prim = T_OPTDATA_REQ;
		break;
	default:
		r->ERROR_prim = p->ERROR_prim;
		break;
	}
	r->UNIX_error = p->UNIX_error;
	switch (p->NPI_error) {
#if 0
	case NBADADDR:		/* 1 */
		r->TLI_error = TBADADDR;	/* 1 */
		break;
	case NBADOPT:		/* 2 */
		r->TLI_error = TBADOPT;	/* 2 */
		break;
	case NACCESS:		/* 3 */
		r->TLI_error = TACCES;	/* 3 */
		break;
	case NNOADDR:		/* 5 */
		r->TLI_error = TNOADDR;	/* 5 */
		break;
	case NOUTSTATE:	/* 6 */
		r->TLI_error = TOUTSTATE;	/* 6 */
		break;
	case NBADSEQ:		/* 7 */
		r->TLI_error = TBADSEQ;	/* 7 */
		break;
	case NSYSERR:		/* 8 */
		r->TLI_error = TSYSERR;	/* 8 */
		break;
	case NBADDATA:		/* 10 */
		r->TLI_error = TBADDATA;	/* 10 */
		break;
	case NBADFLAG:		/* 16 */
		r->TLI_error = TBADFLAG;	/* 16 */
		break;
	case NNOTSUPPORT:	/* 18 */
		r->TLI_error = TNOTSUPPORT;	/* 18 */
		break;
#endif
	case NBOUND:		/* 19 */
		r->TLI_error = TADDRBUSY;	/* 23 */
		break;
	case NBADQOSPARAM:	/* 20 */
		r->TLI_error = TBADOPT;	/* 2 */
		break;
	case NBADQOSTYPE:	/* 21 */
		r->TLI_error = TBADOPT;	/* 2 */
		break;
	case NBADTOKEN:	/* 22 */
		r->TLI_error = TPROVMISMATCH;	/* 25 */
		break;
	case NNOPROTOID:	/* 23 */
		r->TLI_error = TBADADDR;	/* 1 */
		break;
	default:
		r->TLI_error = p->NPI_error;
		break;
	}
	xp_set_n_state(xp, xp->chkstate);
	xp_set_t_state(xp, xp->chkstate);
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);

}
static int
np_ok_ack(struct xp *xp, queue_t *q, mblk_t *bp)
{
	N_info_req_t *p = (typeof(p)) bp->b_rptr;
	struct T_info_req *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_rptr += sizeof(*r);
	r->PRIM_type = T_OK_ACK;
	switch (p->CORRECT_prim) {
	case N_UNBIND_REQ:
		r->CORRECT_prim = T_UNBIND_REQ;
		xp_set_n_state(xp, NS_UNBND);
		xp_set_t_state(xp, TS_UNBND);
		break;
	case N_CONN_RES:
		r->CORRECT_prim = T_CONN_RES;
		xp_set_n_state(xp, NS_DATA_XFER);
		xp_set_t_state(xp, TS_DATA_XFER);
		break;
	case N_DISCON_REQ:
		r->CORRECT_prim = T_DISCON_REQ;
		xp_set_n_state(xp, NS_IDLE);
		xp_set_t_state(xp, TS_IDLE);
		break;
	case N_RESET_RES:
		r->CORRECT_prim = T_DATA_REQ;
		xp_set_n_state(xp, NS_DATA_XFER);
		xp_set_t_state(xp, TS_DATA_XFER);
		/* Note we do not propagate acks for reset responses. */
		freemsg(bp);
		freeb(mp);
		return (0);
	case N_OPTMGMT_REQ:
		r->CORRECT_prim = T_OPTMGMT_REQ;
		if (xp_get_n_state(xp, NS_WACK_OPTREQ))
			xp_set_n_state(xp, NS_IDLE);
		if (xp_get_t_state(xp, TS_WACK_OPTREQ))
			xp_set_t_state(xp, NS_IDLE);
		break;
	default:
		r->CORRECT_prim = p->CORRECT_prim;
		break;
	}
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_unitdata_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
	N_unitdata_ind_t *p = (typeof(p)) bp->b_rptr;
	struct T_unitdata_ind *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r) + p->SRC_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_UNITDATA_IND;
	r->SRC_length = p->SRC_length;
	r->SRC_offset = sizeof(*r);
	r->OPT_length = 0;
	r->OPT_offset = 0;
	bcopy(bp->b_rptr + p->SRC_offset, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_uderror_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
	N_uderror_ind_t *p = (typeof(p)) bp->b_rptr;
	struct T_uderror_ind *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r) + p->DEST_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_UDERROR_IND;
	r->DEST_length = p->DEST_length;
	r->DEST_offset = sizeof(*r);
	r->OPT_length = 0;
	r->OPT_offset = 0;
	r->ERROR_type = p->ERROR_type;
	bcopy(bp->b_rptr + p->DEST_offset, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
/**
 * np_datack_ind: - process N_DATACK_IND primitive
 * @xp: X.25 private structure
 * @q: active queue (lower read queue)
 * @bp: the N_DATACK_IND primitive
 *
 * XX25 specifies that data acknolwedgements are translated into zero length data indications that
 * have the special T_X25_DACK or T_X25_EACK flags bits set.
 */
static int
np_datack_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_data_ind *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_DATA_IND;
	r->MORE_flag = T_X25_DACK;
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * np_reset_ind: - process N_RESET_IND primitive
 * @xp: X.25 private strucutre
 * @q: active queue (lower read queue)
 * @bp: the  N_RESET_IND primitive
 *
 * XX25 specifies that reset indications are translated into zero length data indications that have
 * the special T_X25_RST flags bit set.
 */
static int
np_reset_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_data_ind *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_DATA_IND;
	r->MORE_flag = T_X25_RST;
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
/**
 * np_reset_con: - process N_RESET_CON primitive
 * @xp: X.25 private strucutre
 * @q: active queue (lower read queue)
 * @bp: the  N_RESET_CON primitive
 *
 * XX25 specifies that reset confirmations are translated into zero length data indications that
 * have the special T_X25_RST flags bit set.
 */
static int
np_reset_con(struct xp *xp, queue_t *q, mblk_t *bp)
{
	struct T_data_ind *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_DATA_IND;
	r->MORE_flag = T_X25_RST;
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_other_ind(struct xp *xp, queue_t *q, mblk_t *bp)
{
	freemsg(bp);
	return (0);
}

/*
 *  M_IOCTL, M_IOCDATA Handling
 *  -------------------------------------------------------------------------
 *  Note that the XX25 module does not implement any input-output controls of its own and they are
 *  merely passed to the driver.
 */
static int
xx25_w_ioctl(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  M_COPYIN, M_COPYOUT Handling
 *  -------------------------------------------------------------------------
 *  Note that the XX25 module does not implement any input-output controls of its own and they are
 *  merely passed to the driver.
 */
static int
xx25_r_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  M_CTL, M_PCCTL Handling
 *  -------------------------------------------------------------------------
 *  The module uses M_CTL and M_PCCTL like input-output controls to control the driver.
 */
static int
xx25_r_cmd(struct xp *xp, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (!MBLKIN(mp, 0, sizeof(*ioc)))
		return (-EINVAL);

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case XX25_IOCGOPTION:
		return xx25_get_option(xp, q, mp);
	case XX25_IOCSOPTION:
		return xx25_set_option(xp, q, mp);
	case XX25_IOCGCONFIG:
		return xx25_get_config(xp, q, mp);
	case XX25_IOCSCONFIG:
		return xx25_set_config(xp, q, mp);
	case XX25_IOCTCONFIG:
		return xx25_test_config(xp, q, mp);
	case XX25_IOCCCONFIG:
		return xx25_confirm_config(xp, q, mp);
	case XX25_IOCLCONFIG:
		return xx25_list_config(xp, q, mp);
	case XX25_IOCGSTATEM:
		return xx25_get_statem(xp, q, mp);
	case XX25_IOCCMRESET:
		return xx25_master_reset(xp, q, mp);
	case XX25_IOCGSTATSP:
		return xx25_get_statsp(xp, q, mp);
	case XX25_IOCSSTATSP:
		return xx25_set_statsp(xp, q, mp);
	case XX25_IOCGSTATS:
		return xx25_get_stats(xp, q, mp);
	case XX25_IOCCSTATS:
		return xx25_set_stats(xp, q, mp);
	case XX25_IOCGNOTIFY:
		return xx25_get_notify(xp, q, mp);
	case XX25_IOCSNOTIFY:
		return xx25_set_notify(xp, q, mp);
	case XX25_IOCCNOTIFY:
		return xx25_clear_notify(xp, q, mp);
	case XX25_IOCCMGMT:
		return xx25_mgmt(xp, q, mp);
	case XX25_IOCCPASS:
		return xx25_pass(xp, q, mp);
	default:
		freemsg(mp);
		return (0);
	}
}

static int
__xx25_r_ctl(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err;

	err = xx25_r_cmd(xp, q, mp);
	return (err < 0 ? err : 0);
}

static int
xx25_r_ctl(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_r_ctl(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

/*
 *  M_PROTO, M_PCPROTO Handling
 *  -------------------------------------------------------------------------
 */
static int
xx25_w_prim(struct xp *xp, queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		return (-EINVAL);
	switch ((prim = *(t_uscalar_t *) mp->b_rptr)) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_OPTDATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s [%d] ->", tp_primname(prim),
			  (int) msgdsize(mp->b_cont));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s ->", tp_primname(prim));
		break;
	}
	switch (prim) {
	case T_CONN_REQ:
		return tp_conn_req(xp, q, mp);
	case T_CONN_RES:
		return tp_conn_res(xp, q, mp);
	case T_DISCON_REQ:
		return tp_discon_req(xp, q, mp);
	case T_DATA_REQ:
		return tp_data_req(xp, q, mp);
	case T_EXDATA_REQ:
		return tp_exdata_req(xp, q, mp);
	case T_INFO_REQ:
		return tp_info_req(xp, q, mp);
	case T_BIND_REQ:
		return tp_bind_req(xp, q, mp);
	case T_UNBIND_REQ:
		return tp_unbind_req(xp, q, mp);
	case T_UNITDATA_REQ:
		return tp_unitdata_req(xp, q, mp);
	case T_OPTMGMT_REQ:
		return tp_optmgmt_req(xp, q, mp);
	case T_ORDREL_REQ:
		return tp_ordrel_req(xp, q, mp);
	case T_OPTDATA_REQ:
		return tp_optdata_req(xp, q, mp);
	case T_ADDR_REQ:
		return tp_addr_req(xp, q, mp);
	case T_CAPABILITY_REQ:
		return tp_capability_req(xp, q, mp);
	default:
		return tp_other_req(xp, q, mp);
	}
}
static int
__xx25_w_proto(struct xp *xp, queue_t *q, mblk_t *mp)
{
	uint oldstate = xp_get_state(xp);
	int err = 0;

	if ((err = xx25_w_prim(xp, q, mp)))
		xp_set_state(xp, oldstate);
	return (err < 0 ? err : 0);
}
static int
xx25_w_proto(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_w_proto(xp, q mp);
		xp_release(xp);
	}
	return (err);
}
static int
xx25_r_prim(struct xp *xp, queue_t *q, mblk_t *mp)
{
	np_ulong prim;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		return (-EINVAL);
	switch ((prim = *(np_ulong *) mp->b_rptr)) {
	case N_DATA_REQ:
	case N_EXDATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s [%d] <-", np_primname(prim),
			  (int) msgdsize(mp->b_cont));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", np_primname(prim));
		break;
	}
	switch (prim) {
	case N_CONN_IND:
		return np_conn_ind(xp, q, mp);
	case N_CONN_CON:
		return np_conn_con(xp, q, mp);
	case N_DISCON_IND:
		return np_discon_ind(xp, q, mp);
	case N_DATA_IND:
		return np_data_ind(xp, q, mp);
	case N_EXDATA_IND:
		return np_exdata_ind(xp, q, mp);
	case N_INFO_ACK:
		return np_info_ack(xp, q, mp);
	case N_BIND_ACK:
		return np_bind_ack(xp, q, mp);
	case N_ERROR_ACK:
		return np_error_ack(xp, q, mp);
	case N_OK_ACK:
		return np_ok_ack(xp, q, mp);
	case N_UNITDATA_IND:
		return np_unitdata_ind(xp, q, mp);
	case N_UDERROR_IND:
		return np_uderror_ind(xp, q, mp);
	case N_DATACK_IND:
		return np_datack_ind(xp, q, mp);
	case N_RESET_IND:
		return np_reset_ind(xp, q, mp);
	case N_RESET_CON:
		return np_reset_con(xp, q, mp);
	default:
		return np_other_ind(xp, q, mp);
	}
}
static int
__xx25_r_proto(struct xp *xp, queue_t *q, mblk_t *mp)
{
	uint oldstate = xp_get_state(xp);
	int err = 0;

	if ((err = xx25_r_prim(xp, q, mp)))
		xp_set_state(xp, oldstate);
	return (err < 0 ? err : 0);
}
static int
xx25_r_proto(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_r_proto(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

/*
 * M_DATA, M_HPDATA Handling
 * --------------------------------------------------------------------------
 */
static int
__xx25_w_data(struct xp *xp, queue_t *q, mblk_t *mp)
{
	return tp_data(xp, q, mp);
}
static int
xx25_w_data(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_w_data(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

static int
__xx25_r_data(struct xp *xp, queue_t *q, mblk_t *mp)
{
	return np_data(xp, q, mp);
}
static int
xx25_r_data(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_w_data(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

/*
 * M_SIG, M_PCSIG handling
 * --------------------------------------------------------------------------
 */
static int
xx25_r_sig(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * M_RSE, M_PCRSE handling
 * --------------------------------------------------------------------------
 */
static int
xx25_r_rse(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * M_ERROR handling
 * --------------------------------------------------------------------------
 */
static int
xx25_r_error(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		/* FIXME: must record fact that there is an error */
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * M_HANGUP handling
 * --------------------------------------------------------------------------
 */
static int
xx25_r_hangup(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		/* FIXME: must record fact that there is a hangup */
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * M_FLUSH handling
 * --------------------------------------------------------------------------
 */
static int
xx25_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static int
xx25_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

/*
 *  Other STREAMS Message Handling
 *  -------------------------------------------------------------------------
 */
static int
xx25_w_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static int
xx25_r_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  STREAMS Message Handling
 *  -------------------------------------------------------------------------
 */

static int
xx25_w_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return xx25_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return xx25_w_proto(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return xx25_w_ioctl(q, mp);
	case M_FLUSH:
		return xx25_w_flush(q, mp);
	default:
		return xx25_w_other(q, mp);
	}
}
static int
__xx25_w_msg(struct xp *xp, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return __xx25_w_data(xp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __xx25_w_proto(xp, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return xx25_w_ioctl(q, mp);
	case M_FLUSH:
		return xx25_w_flush(q, mp);
	default:
		return xx25_w_other(q, mp);
	}
}
static int
__xx25_r_msg(struct xp *xp, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return xx25_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return xx25_r_proto(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return xx25_r_copy(q, mp);
	case M_SIG:
	case M_PCSIG:
		return xx25_r_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return xx25_r_rse(q, mp);
	case M_CTL:
	case M_PCCTL:
		return xx25_r_ctl(q, mp);
	case M_ERROR:
		return xx25_r_error(q, mp);
	case M_HANGUP:
		return xx25_r_hangup(q, mp);
	case M_FLUSH:
		return xx25_r_flush(q, mp);
	default:
		return xx25_r_other(q, mp);
	}
}
static int
xx25_r_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return __xx25_r_data(xp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __xx25_r_proto(xp, q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return xx25_r_copy(q, mp);
	case M_SIG:
	case M_PCSIG:
		return xx25_r_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return xx25_r_rse(q, mp);
	case M_CTL:
	case M_PCCTL:
		return __xx25_r_ctl(xp, q, mp);
	case M_ERROR:
		return xx25_r_error(q, mp);
	case M_HANGUP:
		return xx25_r_hangup(q, mp);
	case M_FLUSH:
		return xx25_r_flush(q, mp);
	default:
		return xx25_r_other(q, mp);
	}
}

/*
 *  PUT AND SERVICE PROCEDURES
 *  -------------------------------------------------------------------------
 */
static streamscall int
xx25_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || xx25_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
xx25_wsrv(queue_t *q)
{
	struct xp *xp;

	if ((xp = xp_acquire(q))) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (__xx25_w_msg(xp, q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		xp_release(q);
	}
	return (0);
}
static streamscall int
xx25_rsrv(queue_t *q)
{
	struct xp *xp;

	if ((xp = xp_acquire(q))) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (__xx25_r_msg(xp, q, mp)) {
				putbq(q, mp);
				break;
			}
		}
	}
	return (0);
}
static streamscall int
xx25_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || xx25_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  OPEN AND CLOSE ROUTINES
 *  -------------------------------------------------------------------------
 */
static int
xx25_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	if (q->q_ptr != NULL)
		return (0);	/* already open */
}

static int
xx25_qclose(queue_t *q, int oflags, cred_t *crp)
{
}

static struct qinit xx25_rinit = {
	.qi_putp = xx25_rput,
	.qi_srvp = xx25_rsrv,
	.qi_qopen = xx25_qopen,
	.qi_qclose = xx25_qclose,
	.qi_minfo = &xx25_minfo,
	.qi_mstat = &xx25_rstat,
};

static struct qinit xx25_rinit = {
	.qi_putp = xx25_wput,
	.qi_srvp = xx25_wsrv,
	.qi_minfo = &xx25_minfo,
	.qi_mstat = &xx25_wstat,
};

struct streamtab xx25_info = {
	.st_rdinit = &xx25_rinit,
	.st_wrinit = &xx25_winit,
};
