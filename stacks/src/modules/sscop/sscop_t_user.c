/*****************************************************************************

 @(#) $RCSfile: sscop_t_user.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/05/14 08:31:10 $

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/05/14 08:31:10 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop_t_user.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/05/14 08:31:10 $"

static char const ident[] =
    "$RCSfile: sscop_t_user.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/05/14 08:31:10 $";

/*
 *  =========================================================================
 *
 *  T-User --> T-Provider (SSCOP) Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */
/*
 *  T_INFO_REQ
 *  -------------------------------------------------------------------------
 */
static int t_info_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	(void) pdu;
	if (!(mp = t_info_ack(q)))
		return (-ENOBUFS);
	freemsg(pdu);
	qreply(q, mp);
	return (0);
}

/*
 *  T_ADDR_REQ
 *  -------------------------------------------------------------------------
 */
static int t_addr_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	(void) pdu;
	if (!(mp = t_addr_ack(q)))
		return (-ENOBUFS);
	freemsg(pdu);
	qreply(q, mp);
	return (0);
}

/*
 *  T_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
static int t_optmgmt_req(queue_t * q, mblk_t * pdu)
{
	int err;
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);

	if (sp->t_state != TS_IDLE)
		goto opmgmt_req_outstate;
	{
		struct T_optmgmt_req *p = (struct T_optmgmt_req *) pdu->b_rptr;
		const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
		const size_t opt_len = p->OPT_length;
		const uint flags = p->MGMT_flags;
		switch (flags) {
		case T_NEGOTIATE:
		{
			caddr_t req = opt_ptr;
			struct t_opthdr *reqoh;
			for (req = opt_ptr; req <= opt_ptr + opt_len - sizeof(*reqoh);
			     req = req + reqoh->len) {
				reqoh = (struct t_opthdr *) req;
				switch (reqoh->level) {
				case T_SSCOP:
					switch (reqoh->name) {
					case T_ALLOPT:
					}
				}
			}
		}
		case T_CHECK:
		{
			caddr_t req = opt_ptr;
			struct t_opthdr *reqoh;
			for (req = opt_ptr; req <= opt_ptr + opt_len - sizeof(*reqoh);
			     req = req + reqoh->len) {
				reqoh = (struct t_opthdr *) req;
				switch (reqoh->level) {
				case T_SSCOP:
					switch (reqoh->name) {
					}
				}
			}
		}
		case T_DEFAULT:
		{
			caddr_t req = opt_ptr;
			struct t_opthdr *reqoh;
			for (req = opt_ptr; req <= opt_ptr + opt_len - sizeof(*reqoh);
			     req = req + reqoh->len) {
				reqoh = (struct t_opthdr *) req;
				switch (reqoh->level) {
				case T_SSCOP:
					switch (reqoh->name) {
					case T_ALLOPT:
					}
				default:
				}
			}
		}
		case T_CURRENT:
		{
			caddr_t req = opt_ptr;
			struct t_opthdr *reqoh;
			for (req = opt_ptr; req <= opt_ptr + opt_len - sizeof(*reqoh);
			     req = req + reqoh->len) {
				reqoh = (struct t_opthdr *) req;
				switch (reqoh->level) {
				case T_SSCOP:
					switch (reqoh->name) {
					case T_ALLOPT:
					}
				}
			}
		}
		}
	}

      optmgmt_req_outstate:
	err = TOUTSTATE;
	goto optmgmt_req_error;

      optmgmt_req_error:
	freemsg(pdu);
	qreply(q, t_error_ack(T_OPTMGMT_REQ, err));
	return (0);
}

/*
 *  T_BIND_REQ
 *  -------------------------------------------------------------------------
 */
static int t_bind_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);

	switch (sp->t_state) {
	case TS_UNBND:
	{
		struct T_bind_req *p = (struct T_bind_req *) pdu->b_rptr;
		caddr_t add_ptr;
		size_t add_len;
		u16 bport = 0;
		u32 baddr;
		int err = 0;
		struct sscop_bind *bb, **bbp;

		/* pull addresses out of bind primitive */
		if ((add_len = p->ADDR_length) >= sizeof(u16)
		    && add_len <= mp->b_wptr - p->ADDR_offset) {
			struct sscop_baddr **sbp = &sp->baddr;

			add_ptr = p->ADDR_offset + pdu->b_rptr;

			bport = *((u16 *) add_ptr)++;

			for (add_len -= sizeof(u16); add_len >= sizeof(u32); add_len -= sizeof(u32)) {
				if (!((baddr = *((u32 *) add_ptr)++) & 0xff000000))
					goto t_bind_req_badaddr;

				if (!
				    (*sbp =
				     kmem_cache_alloc(sscop_badd_cachep, SLAB_HWCACHE_ALIGN)))
					goto t_bind_req_nomem;

				(*sbp)->baddr = baddr;

				(*sbp)->next = NULL;
				(*sbp)->pprev = sbp;
				sbp = &(*sbp)->next;

				sp->banum++;
			}
		}

		cons = p->CONIND_number;
		/* See if we need to assign a port number */
		if (!(sp->bport = htons(bport)) && !(sp->cons = cons)) {
			static u16 sscop_port_rover = 0;
			/* 
			 *  This stream can only be used for outgoing connections, so
			 *  if it is assigned a zero port number we choose an unused
			 *  port number for the stream and assign it.
			 */
			int low = sysctl_local_port_range[0];
			int high = sysctl_local_port_range[1];
			int rem = (high - low) + 1;

			bport = sscop_port_rover;

			for (; rem > 0; bport++, rem--) {
				if (bport > high || bport < low)
					bport = low;
				bbp = &sscop_bind_hash[bport & 0xff];
				for (bb = *bbp; bb && bb->port != bport; bb = bb->next);
				if (!bb || !bb->bound)
					break;
			}
			/* want a position with an unowned bind bucket */
			if (rem <= 0 || (bb && bb->bound))
				goto t_bind_req_noaddr;

			sscop_port_rover = bport;
		} else {
			bbp = &sscop_bind_hash[bport & 0xff];
			for (bb = *bbp; bb && sp->bport != bport; bb = bb->next);
		}
		/* If we have an existing bind bucket, check for conflicts */
		if (bb && bb->cons && cons) {
			struct sscop *sp2;

			err = TADDRBUSY;
			for (sp2 = bb->bound; sp2; sp2 = sp2->bind_next) {
				struct sscop_baddr *sb, *sb2;

				if (!sp->baddr && !sb2->baddr)
					goto t_bind_req_addrbusy;

				for (sb = sp->baddr; sb; sb = sb->next)
					for (sb2 = sp2->baddr; sb2; sb2 = sb2->next)
						if (sb->baddr == sb2->baddr)
							goto t_bind_req_addrbusy;
			}
		}
		/* place in bind hashes */
		if (!bb) {
			/* no bind bucket, create one */
			if (!(bb = kmalloc(sizeof(*bb), GFP_ATOMIC))) {
				freemsg(mp);
				goto t_bind_req_nomem;
			}
			if ((bb->next = *bbp))
				bb->next->pprev = &bb->next;
			bb->pprev = bbp;
			*bbp = bb;
			bb->port = bport;
			bb->cons = 0;
			bb->bound = NULL;
		}
		sp->bind = bb;
		if ((sp->bind_next = bb->bound))
			sp->bind_next->bind_pprev = &sp->bind_next;
		sp->bind_pprev = &bb->bound;
		bb->bound = sp;
		bb->cons++;

		sp->t_state = TS_IDLE;
		freemsg(pdu);
		mp = t_bind_ack(q);
		qreply(q, mp);
		return (0);
	}
	}

      t_bind_req_outstate:
	err = TOUTSTATE;
	goto t_bind_req_error;

      t_bind_req_badaddr:
	err = TBADADDR;
	goto t_bind_req_error;

      t_bind_req_addrbusy:
	err = TADDRBUSY;
	goto t_bind_req_error;

      t_bind_req_noaddr:
	err = TNOADDR;
	goto t_bind_req_error;

      t_bind_req_nomem:
	err = -ENOMEM;
	goto t_bind_req_error;

      t_bind_req_nobufs:
	err = -ENOBUFS;
	goto t_bind_req_error;

      t_bind_req_error:
	sscop_free_baddrs(sp);
	freemsg(pdu);
	qreply(q, t_error_ack(T_BIND_REQ, err));
	return (0);
}

/*
 *  T_UNBIND_REQ
 *  -------------------------------------------------------------------------
 */
static int t_unbind_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);

	struct sscop_bind *bb;

	if (sp->t_state != TS_IDLE)
		goto t_unbind_req_outstate;

	if ((bb = sp->bind)) {
		if (sp->cons)
			bb->cons--;
		if ((*sp->bind_pprev = sp->bind_next))
			sp->bind_next->bind_pprev = sp->bind_pprev;
		sp->bind = NULL;
		sp->bind_next = NULL;
		sp->bind_pprev = NULL;
		sp->cons = 0;
		if (!bb->bound) {
			if ((*bb->pprev = bb->next))
				bb->next->pprev = bb->pprev;
			kfree(bb);
		}
	}
	sscop_free_baddrs(sp);
	sp->t_state = TS_UNBND;
	freemsg(pdu);
	mp = t_ok_ack(T_UNBIND_REQ);
	qreply(q, mp);
	return (0);

      t_unbind_req_outstate:
	err = TOUTSTATE;
	goto t_unbind_req_error;

      t_unbind_req_error:
	freemsg(pdu);
	qreply(q, t_error_ack(T_UNBIND_REQ, err));
	return (0);
}

/*
 *  T_CONN_REQ
 *  -------------------------------------------------------------------------
 */
static int t_conn_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_conn_req *p = (struct T_conn_req *) pdu->b_rptr;
	const caddr_t dst_ptr = p->DEST_offset + pdu->b_rptr;
	const size_t dst_len = p->DEST_length;
	const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
	const size_t opt_len = p->OPT_length;

	if (sp->t_state != TS_IDLE)
		goto t_conn_req_outstate;

	/* pull addresses out of the destination */
	if (dst_len < sizeof(u16) + sizeof(u32) || dst_len > mp->b_wptr - dst_ptr)
		goto t_conn_req_badaddr;

	if (!(sp->dport = htons(*((u16 *) dst_ptr)++)))
		goto t_conn_req_badaddr;
	/* 
	 *  TODO: check if user is allowed to set port address.
	 */

	for (dst_len -= sizeof(u16); dst_len >= sizeof(u32); dst_len -= siseof(u32)) {
		if (!((daddr = *((u32 *) dst_ptr)++) & 0xff000000))
			goto t_conn_req_badaddr;
		sdp_daddr_include(sp, daddr);
	}
	/* 
	 *  TODO: check if we can route to one of the destination addresses.
	 */
	sp->v_tag = tcp_secure_sequence(sp->saddr->saddr, sp->daddr->daddr, sp->sport, sp->dport);
	sp->a_rwnd = FIXME;
	/* FIXME: these should be left at set values, set them this way when the sscop_t is
	   intialized. */
//      sp->n_ostr = -1; /* ask for as many as we can get */
//      sp->n_istr = -1; /* offer as many as the other end needs */
	sp->i_tsn = htonl(sp->v_tag);
	{
		caddr_t op;
		struct t_opthdr *oh;
		const caddr_t end_ptr = opt_ptr + opt_len - sizeof(*oh);

		u16 *sidp = NULL;
		u32 *ppip = NULL;
		u16 *istp = NULL;
		u16 *ostp = NULL;

		for (op = opt_ptr,
		     oh = (struct t_opthdr *) op;
		     op <= end_ptr; op += PADC(oh->len), oh = (struct t_opthdr *) op) {
			if (oh->level == T_INET_SSCOP) {
				switch (oh->name) {
				case T_ALLOPT:
					break;

				case SSCOP_I_STREAMS:
					if (oh->len != sizeof(*oh) + sizeof(*istp))
						goto t_conn_req_badopt;
					if (!*(istp = ((u16 *) oh->value)))
						goto t_conn_req_badopt;
					continue;
				case SSCOP_O_STREAMS:
					if (oh->len != sizeof(*oh) + sizeof(*ostp))
						goto t_conn_req_badopt;
					if (!*(ostp = ((u16 *) oh->value)))
						goto t_conn_req_badopt;
					continue;
				case SSCOP_SID:
					if (oh->len != sizeof(*oh) + sizeof(sid))
						goto t_conn_req_badopt;
					if (*(sidp = ((u16 *) oh->value)) >= sp->n_ostr)
						goto t_conn_req_badopt;
					continue;
				case SSCOP_PPI:
					if (oh->len != sizeof(*oh) + sizeof(ppi))
						goto t_conn_req_badopt;
					ppip = ((u32 *) oh->value);
				default:
					continue;
				}
				break;
			}
		}
		if (sidp)
			sp->sid = *sidp;
		if (ppip)
			sp->ppi = *ppip;
		if (istp)
			sp->n_istr = *istp;
		if (ostp)
			sp->n_ostr = *ostp;
	}
	/* 
	 *  TODO: do some connect routing on the destination address list and
	 *  determine the best alternative addresses.
	 */
	sp->taddr = sp->daddr;
	sp->raddr = sp->daddr->next ? sp->daddr->next : sp->daddr;

	if ((err = sscop_send_init(p)))
		goto t_conn_req_error;

	sp->t_state = T_WCON_CREQ;

	freemsg(pdu);
	qreply(q, t_ok_ack(T_CONN_REQ));
	return (0);

	/* Try to connect to the first address */

      t_conn_req_outstate:
	err = TOUTSTATE;
	goto t_conn_req_error;

      t_conn_req_badopt:
	err = TBADOPT;
	goto t_conn_req_error;

      t_conn_req_badaddr:
	err = TBADADDR;
	goto t_conn_req_error;

      t_conn_req_error:
	bzero(&sp->l, sizeof(sp->l));
	sscop_free_daddrs(sp);
	freemsg(pdu);
	qreply(q, t_error_ack(T_CONN_REQ, err));
	return (0);
}

/*
 *  T_CONN_RES
 *  -------------------------------------------------------------------------
 */
static int t_conn_res(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_conn_res *p = (struct T_conn_res *) pdu->b_rptr;
	queue_t *aq = (queue_t *) p->ACCEPTOR_id;
	const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
	const size_t opt_len = p->OPT_length;
	const uint seq = p->SEQ_number;

	switch (sp->t_state) {
	case TS_WRES_CIND:
	{
		struct sscop *ap;
		struct sscop_cookie_echo *m;
		struct sscop_cookie *ck
		    /* first look for connection indication with indicated sequence number */
		for (mp = bufq_peek(&sp->connect_queue); mp; mp = mp->b_next) {
			struct sscop_rcb *cb = SSCOP_RCB(mp);

			if (cb->seq == seq)
				break;
		}
		if (!mp)
			goto conn_res_badseq;
		if (!aq || !aq->q_ptr || !aq->q_qinfo || !aq->q_qinfo->qi_minfo)
			goto conn_res_badq;
		if (aq->q_qinfo->qi_minfo.mi_idnum != q->q_qinfo->qi_minfo.mi_idnum)
			goto conn_res_provmismatch;

		ap = SSCOP_PRIV(aq);

		if (aq != q) {
			if (aq->t_state == TS_UNBND)
				goto conn_res_badaddr;
			if (aq->t_state != TS_IDLE)
				goto conn_res_badf;
			if (ap->cons)
				goto conn_res_resqlen;
			if (sp->sport && ap->sport && sp->sport != ap->sport)
				goto conn_res_resaddr;
			if (sp->saddr && ap->saddr && !sscop_same_bind(sp, ap))
				goto conn_res_resaddr;
		}
		if (opt_len) {
			u16 *sidp = NULL;
			u32 *ppip = NULL;
			u16 *istp = NULL;
			u16 *ostp = NULL;

			struct t_opthdr *oh;
			caddr_t op;
			for (op = opt_ptr, oh = (struct t_opthdr *) op;
			     op < opt_ptr + opt_len;
			     op += PADC(oh->len), oh = (struct t_opthdr *) op) {
				if (oh->level == T_INET_SSCOP) {
					switch (oh->name) {
					case SSCOP_SID:
						if (oh->len != sizeof(*oh) + sizeof(*sidp))
							goto conn_res_badopt;
						sidp = ((u16 *) oh->value);
						continue;
					case SSCOP_PPI:
						if (oh->len != sizeof(*oh) + sizeof(*ppip))
							goto conn_res_badopt;
						ppip = ((u32 *) oh->value);
						continue;
					case SSCOP_I_STREAMS:
						if (oh->len != sizeof(*oh) + sizeof(*istp))
							goto conn_res_badopt;
						istp = ((u16 *) oh->value);
						continue;
					case SSCOP_O_STREAMS:
						if (oh->len != sizeof(*oh) + sizeof(*ostp))
							goto conn_res_badopt;
						ostp = ((u16 *) oh->value);
						continue;
					default:
						goto conn_res_badopt;
					}
				}
				goto conn_res_badopt;
			}
			if (sidp)
				ap->sid = *sidp;
			if (ppip)
				ap->ppi = *ppip;

			ap->ostr = sscop_find_ostr(ap, ap->sid);
			ap->ostr->ppi = ap->ppi;
		}
		m = (struct sscop_cookie_echo *) mp->b_cont->b_rptr;
		ck = m->cookie;

		ap->t_state = TS_DATA_XFER;

		ap->dport = ck->dport;
		ap->sport = ck->sport;

		ap->sackf = SSCOP_SACKF_NOD;
		ap->v_tag = ck->v_tag;
		ap->p_tag = ck->p_tag;
		ap->i_strs = ck->i_strs;
		ap->o_strs = ck->o_strs;
		ap->sscop_tsn = ck->v_tag;
		ap->a_tsn = ck->v_tag - 1;
		ap->c_tsn = ck->p_tsn - 1;
		ap->e_tsn = ck->p_tsn - 1;
		ap->p_rwnd = ck->p_rwnd;

		sscop_free_strms(sp);
		sscop_free_daddrs(sp);
		sscop_free_saddrs(sp);

		{
			int dnum = ck->dta_num;
			int snum = ck->sta_num;
			struct sscop_daddr **sdp = &ap->daddr;
			struct sscop_saddr **ssp = &ap->saddr;

			u32 *addp = (u32 *) (((u8 *) (ck + 1) + ck->opt_len));

			for (i = 0; i < ck->dta_num; i++, sdp = &(*sdp)->next)
				if (!sscop_daddr_include(ap, *addp++))
					goto conn_res_nomem;

			for (i = 0; i < ck->sta_num; i++, ssp = (*ssp)->next) {
				if (!
				    (*ssp =
				     kmem_cache_alloc(sscop_srce_cachep, SLAB_HWALIGN_CACHE)))
					goto conn_res_nomem;
				bzero(*ssp, sizeof(**ssp));
				(*ssp)->saddr = *addp++;
				(*ssp)->sp = ap;
				/* 
				 *  More saddr initialization.
				 */
			}
		}

	}
	}
      conn_res_outstate:
	err = TOUTSTATE;
	goto conn_res_error;

      conn_res_nomem:
	err = -ENOMEM;
	goto conn_res_free_error;

      conn_res_badaddr:
	err = TBADADDR;
	goto conn_res_error;

      conn_res_resaddr:
	err = TRESADDR;
	goto conn_res_error;

      conn_res_badseq:
	err = TBADSEQ;
	goto conn_res_error;

      conn_res_badf:
	err = TBADF;
	goto conn_res_error;

      conn_res_badopt:
	err = TBADOPT;
	goto conn_res_error;

      conn_res_provmismatch:
	err = TPROVMISMATCH;
	goto conn_res_error;

      conn_res_resqlen:
	err = TRESQLEN;
	goto conn_res_error;

      conn_res_free_error:
	sscop_free_daddrs(sp);
	sscop_free_saddrs(sp);

      conn_res_error:
	freemsg(pdu);
	qreply(q, t_error_ack(T_CONN_RES, TOUTSTATE));
	return (0);
}

/*
 *  T_DATA_REQ
 *  -------------------------------------------------------------------------
 */
static int t_data_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_data_req *p = (struct T_data_req *) pdu->b_rptr;
	const uint flag = p->MORE_flag;

	uint flags = 0;

	switch (sp->t_state) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:	/* SHUTDOWN-RECEIVED */
		if (!pdu->b_cont || pdu->b_cont->b_datap->db_type != M_DATA)
			goto data_req_error;
		if (sp->ostr->x.more)
			goto data_req_error;
		if (!sp->ostr->n.more)
			flags |= SSCOPCB_FLAG_FIRST_FRAG;
		if (!(sp->ostr->n.more = (flag & T_MORE)))
			flags |= SSCOPCB_FLAG_LAST_FRAG;
		if ((err = sscop_send_data(sp, NULL, sp->ostr, flags, pdu->b_cont)))
			return (err);
		freeb(pdu);
		return (0);

	case TS_IDLE:
		freemsg(pdu);
		return (0);
	}
      data_req_error:
	freemsg(pdu);
	qreply(q, t_m_error(EPROTO));
	return (0);
}

/*
 *  T_EXDATA_REQ
 *  -------------------------------------------------------------------------
 */
static int t_exdata_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_exdata_req *p = (struct T_exdata_req *) pdu->b_rptr;
	const uint flag = p->MORE_flag;

	uint flags = SSCOPCB_FLAG_URG;

	switch (sp->t_state) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:	/* SHUTDOWN-RECEIVED */
		if (!pdu->b_cont || pdu->b_cont->b_datap->db_type != M_DATA)
			goto exdata_req_error;
		if (!sp->ostr->x.more)
			flags |= SSCOPCB_FLAG_FIRST_FRAG;
		if (!(sp->ostr->x.more = (flag & T_MORE)))
			flags |= SSCOPCB_FLAG_LAST_FRAG;
		if ((err = sscop_send_data(sp, NULL, sp->ostr, flags pdu->b_cont)))
			return (err);
		freeb(pdu);
		return (0);

	case TS_IDLE:
		freemsg(pdu);
		return (0);
	}
	freemsg(pdu);
	qreply(q, t_m_error(EPROTO));
	return (0);
}

/*
 *  T_OPTDATA_REQ
 *  -------------------------------------------------------------------------
 */
static int t_optdata_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_optdata_req *p = (struct T_optdata_req *) pdu->b_rptr;
	const uint flag = p->DATA_flag;
	const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
	const size_t opt_len = p->OPT_length;

	if (!opt_len)
		return (flag & T_EXPEDITED)
		    ? t_exdata_req(q, pdu) : t_data_req(q, pdu);

	switch (sp->t_state) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:	/* SHUTDOWN-RECEIVED */
	{
		uint flags = 0;
		struct sscop_strm *st = sp->ostr;
		uint *morep;
		uint *sppip;

		{
			u16 *sidp = &sp->sid;
			u32 *ppip = &sp->ppi;

			int sid = -1;
			int ppi = -1;
			caddr_t op;
			struct t_opthdr *oh;

			/* Walk through options */
			for (op = opt_ptr, oh = (struct t_opthdr *) op;
			     op < opt_ptr + opt_len;
			     op += PADC(oh->len), oh = (struct t_opthdr *) op) {
				if (oh->level == T_INET_SSCOP) {
					switch (oh->name) {
					case SSCOP_SID:
						if (oh->len == sizeof(*oh) + sizeof(*sidp))
							sidp = ((u16 *) oh->value);
						continue;

					case SSCOP_PPI:
						if (oh->len == sizeof(*oh) + sizeof(*ppip))
							ppip = ((u32 *) oh->value);
						continue;
					}
				}
			}
			if (!(st = sscop_find_ostr(sp, *sidp))
			    && !(st = sscop_alloc_ostr(sp, *sidp)))
				return -ENOMEM;
		}

		if (flag & T_EXPEDITED) {
			flags |= SSCOPCB_FLAG_URG;
			morep = &st->x.more;
			st->x.ppi = *ppip;
		} else {
			flags &= ~SSCOPCB_FLAG_URG;
			morep = &st->n.more;
			st->n.ppi = *ppip;
		}
		if (!pdu->b_cont || pdu->b_cont->b_datap->db_type != M_DATA)
			goto optdata_req_error;
		if (!*morep)
			flags |= SSCOPCB_FLAG_FIRST_FRAG;
		if (!(*morep = (flag & T_MORE)))
			flags |= SSCOPCB_FLAG_LAST_FRAG;
		if ((err = sscop_send_data(sp, NULL, st, flags, pdu->b_cont)))
			return (err);
		freeb(pdu);
		return (0);
	}
	case TS_IDLE:
		freemsg(pdu);
		return (0);
	}

      optdata_req_error:
	freemsg(pdu);
	qreply(q, t_m_error(EPROTO));
	return (0);
}

/*
 *  T_UNITDATA_REQ
 *  -------------------------------------------------------------------------
 */
static int t_unitdata_req(queue_t * q, mblk_t * pdu)
{
	freemsg(pdu);
	qreply(q, t_error_ack(T_UNIDATA_REQ, TNOTSUPPORT));
	return (0);
}

/*
 *  T_DISCON_REQ
 *  -------------------------------------------------------------------------
 */
static int t_discon_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_discon_req *p = (struct T_discon_req *) pdu->b_rptr;
	const uint seq = p->SEQ_number;

	if (sp->t_state == TS_WRES_CIND) {
		mblk_t *mp;
		for (mp = bufq_peek(&sp->connect_queue); mp; mp->next) {
			struct sscop_rcb *cb = SSCOP_RCB(mp);

			if (cb->seq == seq)
				break;
		}
		if (!mp)
			goto discon_res_badseq;

		bufq_unlink(&sp->connect_queue, mp);
		freemsg(pdu);
		qreply(q, t_ok_ack(T_DISCON_REQ));
		return (0);
	}
	if ((1 << sp->
	     t_state) & (TSF_WCON_CREQ | TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL)) {
		struct sscop_daddr *sd;
		struct sscop_saddr *ss;

		/* stop association timers */
		if (sp->timer_cookie)
			untimeout(xchg(&sp->timer_cookie, 0));
		if (sp->timer_sack)
			untimeout(xchg(&sp->timer_sack, 0));

		freemsg(xchg(&sp->retry, NULL));

		/* send abort */
		s_send_abort(sp);

		/* remove from vtag cache */
		if (*sp->vtag_pprev = sp->vtag_next)
			sp->vtag_next->vtag_pprev = sp->vtag_pprev;
		sp->vtag_next = NULL;
		sp->vtag_pprev = &sp->vtag_next;
		sp->v_tag = 0;

		/* remove from ptag cache */
		if (*sp->ptag_pprev = sp->ptag_next)
			sp->ptag_next->ptag_pprev = sp->ptag_pprev;
		sp->ptag_next = NULL;
		sp->ptag_pprev = &sp->ptag_next;
		sp->p_tag = 0;

		/* remove peer addresses */
		sscop_free_daddrs(sp);
		/* remove local addresses */
		sscop_free_saddrs(sp);

		/* purge send queues */
		bufq_purge(&sp->write_queue);
		bufq_purge(&sp->urgent_queue);
		bufq_purge(&sp->retrans_queue);

		/* purge recv queues */
		bufq_purge(&sp->out_of_order_queue);
		bufq_purge(&sp->duplicate_queue);
		sp->ngaps = 0;
		sp->ndups = 0;

		sp->t_state = T_IDLE;

		freemsg(pdu);
		qreply(q, t_ok_ack(T_DISCON_REQ));
		return (0);
	}

      discon_req_outstate:
	err = TOUTSTATE;
	goto discon_req_error;

      discon_req_badseq:
	err = TBADSEQ;
	goto discon_req_error;

      discon_req_error:
	freemsg(pdu);
	qreply(q, t_error_ack(T_DISCON_REQ, err));
	return (0);
}

/*
 *  T_ORDREL_REQ
 *  -------------------------------------------------------------------------
 */
static int t_ordrel_req(queue_t * q, mblk_t * pdu)
{
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_ordrel_req *p = (struct T_ordrel_req *) pdu->b_rptr;

	switch (sp->t_state) {
	case TS_DATA_XFER:
		freemsg(pdu);
		if (!bufq_size(&sp->write_queue) && !bufq_size(&sp->retrans_queue))
			sscop_send_shutdown(sp);
		sp->t_state = TS_WIND_ORDREL;
		return (0);

	case TS_WREQ_ORDREL:	/* SHUTDOWN-RECEIVED */
		freemsg(pdu);
		if (!bufq_size(&sp->retrans_queue))
			sscop_send_shutdown_ack(sp);
		sp->t_state = TS_IDLE;
		return (0);
	}
	sp->t_state = TS_NO_STATES;
	freemsg(pdu);
	qreply(q, t_m_error(EPROTO));
	return (0);
}

static int (*t_prim[]) (queue_t *, mblk_t *) = {
	&t_conn_req,		/* T_CONN_REQ 0 */
	    &t_conn_res,	/* T_CONN_RES 1 */
	    &t_discon_req,	/* T_DISCON_REQ 2 */
	    &t_data_req,	/* T_DATA_REQ 3 */
	    &t_exdata_req,	/* T_EXDATA_REQ 4 */
	    &t_info_req,	/* T_INFO_REQ 5 */
	    &t_bind_req,	/* T_BIND_REQ 6 */
	    &t_unbind_req,	/* T_UNBIND_REQ 7 */
	    &t_unitdata_req,	/* T_UNITDATA_REQ 8 */
	    &t_optmgmt_req,	/* T_OPTMGMT_REQ 9 */
	    &t_ordrel_req,	/* T_ORDREL_REQ 10 */
	    NULL,		/* T_CONN_IND 11 */
	    NULL,		/* T_CONN_CON 12 */
	    NULL,		/* T_DISCON_IND 13 */
	    NULL,		/* T_DATA_IND 14 */
	    NULL,		/* T_EXDATA_IND 15 */
	    NULL,		/* T_INFO_ACK 16 */
	    NULL,		/* T_BIND_ACK 17 */
	    NULL,		/* T_ERROR_ACK 18 */
	    NULL,		/* T_OK_ACK 19 */
	    NULL,		/* T_UNITDATA_IND 20 */
	    NULL,		/* T_UDERROR_IND 21 */
	    NULL,		/* T_OPTMGMT_ACK 22 */
	    NULL,		/* T_ORDREL_IND 23 */
	    &t_optdata_req,	/* T_OPTDATA_REQ 24 */
	    &t_addr_req,	/* T_ADDR_REQ 25 */
	    NULL,		/* T_OPTDATA_IND 26 */
	    NULL		/* T_ADDR_ACK 27 */
};

static __inline__ int sscop_t_proto(queue_t * q, mblk_t * mp)
{
	int prim = ((union T_primitives *) (mp->b_rptr))->type;
	if (0 <= prim && prim < sizeof(t_prim) / sizeof(void *))
		if (t_prim[prim])
			return (*t_prim[prim]) (q, mp);
	return (-EOPNOTSUPP);
}

int sscop_w_proto(queue_t * q, mblk_t * mp)
{
	return sscop_t_proto(q, mp);
}

int sscop_w_pcproto(queue_t * q, mblk_t * mp)
{
	return sscop_t_proto(q, mp);
}
