/*****************************************************************************

 @(#) $RCSfile: sctpstr.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-28 23:13:25 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-28 23:13:25 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctpstr.c,v $
 Revision 0.9.2.3  2008-04-28 23:13:25  brian
 - updated headers for release

 Revision 0.9.2.2  2007/08/14 06:22:24  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/12/28 05:40:58  brian
 - added remote stream module

 *****************************************************************************/

#ident "@(#) $RCSfile: sctpstr.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-28 23:13:25 $"

static char const ident[] =
    "$RCSfile: sctpstr.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-28 23:13:25 $";

/*  
 *  This is a pseudo-device multiplexing driver and pushable module that can be used to open any
 *  STREAMS device on a remote machine via SCTP.  It requires an SCTPSTRD (SCTP STREAMS deamon)
 *  running on the serving host, but does not require a daemon to be running on the client host.
 *
 *  The module uses a special device naming convention on the client host.  The device name of the
 *  device upon which the module is (auto)pushed is the host name of the serving host, an asterisk,
 *  and then the full path device name with slashes '/' converted to %2F.  Therefore, /dev/loop on
 *  server ns would be ns@%2Fdev%2Floop.  No, better would be to have the full path name be the same
 *  and then the device name be "name"@host  so /dev/loop on server ns would be /dev/loop@ns on the
 *  local machine.  The host name could be an IP address, such as /dev/loop@192.168.0.1.  When these
 *  devices are given the major device number of one of the SCTP drivers, and a minor device number
 *  identifying that the module should be pushed via an autopush statement, then when the local
 *  device is opened, the module is automagically pushed and a connection is attempted with the far
 *  end, and a message is transferred requesting that the appropriate device be opened on the remote
 *  machine.  The result of the connection and remote open is returned to the open function of the
 *  module and when the open fails, the local Stream is torn down.  If it succeeds, then the open
 *  returns zero and the Stream is ready for normal operation.  Messages passed downstream are given
 *  a stream number equal to the band number of the message.  The message blocks are encapulated in
 *  SCTP data messages, by including the message block and data block information for each message
 *  and data block in the message.  Normal priority data blocks are sent as ordered messages in the
 *  SCTP stream corresponding to the band number.  High priority data block are sent as unordered
 *  messages in the SCTP stream corresponding to the band number (always zero 0).
 *
 *  A special message block type (M_OPEN) is defined as a high-priority message block of value equal
 *  to QPCTL that is used as the first message on the SCTP association.  A block of the same type is
 *  used to return the response code.
 */

struct hdr {
	uint b_band;
	uint b_flag;
	uint b_csum;
	uint b_wptr;
	uint b_cont;
	uint db_type;
	uint db_flag;
};

static inline fastcall int
st_w_msg(queue_t *q, mblk_t *mp)
{
	struct hdr *h;
	mblk_t *b, *b_next, *d, *m, *n, *np = NULL;
	N_qos_sel_data_sctp_t *q;

	if (mp->b_datap->db_type >= QPCTL) {
		N_exdata_req_t *p;

		if (!(n = st_allocb(q, sizeof(*p) + sizeof(*q), BPRI_MED)))
			return (-ENOBUFS);
		n->b_datap->db_type = M_PROTO;
		p = (typeof(p)) n->b_wptr;
		p->PRIM_type = N_EXDATA_REQ;
		n->b_wptr += sizeof(*p);
	} else {
		N_data_req_t *p;

		if (!(n = st_allocb(q, sizeof(*p) + sizeof(*q), BPRI_MED)))
			return (-ENOBUFS);
		n->b_datap->db_type = M_PROTO;
		p = (typeof(p)) n->b_wptr;
		p->PRIM_type = N_DATA_REQ;
		p->DATA_xfer_flags = 0;
		n->b_wptr += sizeof(*p);
	}
	q = (typeof(q)) n->b_wptr;
	q->n_qos_type = N_QOS_SEL_DATA_SCTP;
	q->ppi = 0;
	q->sid = mp->b_band;
	q->ssn = 0;
	s->tsn = 0;
	q->more = 0;
	n->b_wptr += sizeof(*q);

	/* all written message blocks are transformed into data messages to be sent to SCTP. */
	for (b = mp; b; b = b->b_cont) {
		size_t blen = (b->b_wptr > b->b_rptr) ? b->b_wptr - b->b_rptr : 0;

		if (!(m = st_dupb(q, n))) {
			freechunks(np);
			freeb(n);
			return (-ENOBUFS);
		}
		if (!(d = st_allocb(q, sizeof(*h) + blen, BPRI_MED))) {
			freechunks(np);
			freeb(m);
			freeb(n);
			return (-ENOBUFS);
		}
		d->b_datap->db_type = M_DATA;
		d->b_band = b->b_band;
		h = (typeof(h)) d->b_wptr;
		h->b_band = b->b_band;
		h->b_flag = b->b_flag;
		h->b_csum = b->b_csum;
		h->b_cont = b->b_cont ? 1 : 0;
		h->b_wptr = blen;
		h->db_type = b->b_datap->db_type;
		h->db_flag = b->b_datap->db_flag;
		d->b_wptr += sizeof(*h);
		bcopy(b->b_rptr, d->b_wptr, blen);
		d->b_wptr += blen;
		m->b_cont = d;
		if (np == NULL)
			np = m;
		else
			np->b_next = m;
	}
	if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band)) {
		freechunks(np);
		freeb(n);
		return (-EBUSY);
	}
	/* also flush if it is a flush message */
	if (mp->b_datap->db_type == M_FLUSH) {
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
	}
	for (b = bp; b; b = b_next) {
		b_next = XCHG(&b->b_next, NULL);
		putnext(q, b);
	}
	freemsg(mp);
	return (0);
}

static inline fastcall int
st_r_data(queue_t *q, mblk_t *mp)
{
	struct st *st = ST_PRIV(q);
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_data_sctp_t *q = (typeof(q)) (p + 1);
	struct hdr *h;
	mblk_t *b, *dp = mp->b_cont;

	st->more[q->sid] = linkmsg(st->more[q->sid], dp);
	if ((p->DATA_xfer_flags & N_MORE_DATA_FLAG) || q->more) {
		freeb(mp);
		return (0);
	}

	if (!(b = msgpullup(st->more[q->sid], -1))) {
		st->more[q->sid] = rmvb(st->more[q->sid], dp);
		return (-ENOBUFS);
	}

	h = (typeof(h)) b->b_rptr;

	if (!bcanputnext(q, h->b_band)) {
		freemsg(b);
		st->more[q->sid] = rmvb(st->more[q->sid], dp);
		return (-EBUSY);
	}

	freemsg(XCHG(&st->more[q->sid], NULL));

	b->b_rptr += sizeof(*h);
	b->b_wptr = b->b_rptr + h->b_wptr;
	b->b_band = h->b_band;
	b->b_flag = h->b_flag;
	b->b_csum = h->b_csum;
	b->b_datap->db_type = h->db_type;
	b->b_datap->db_flag = 0;

	st->norm[q->sid] = linkmsg(st->norm[q->sid], b);

	if (h->b_cont == 0)
		putnext(q, XCHG(&st->norm[q->sid], NULL));

	freeb(mp);
	return (0);
}

static inline fastcall int
st_r_exdata(queue_t *q, mblk_t *mp)
{
	struct st *st = ST_PRIV(q);
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_data_sctp_t *q = (typeof(q)) (p + 1);
	struct hdr *h = (typeof(h)) mp->b_cont->b_rptr;
	mblk_t *b, *dp = mp->b_cont;

	st->emor = linkmsg(st->emor, dp);
	if (q->emor) {
		freeb(mp);
		return (0);
	}

	if (!(b = msgpullup(st->emor, -1))) {
		st->emor = rmvb(st->emor, dp);
		return (-ENOBUFS);
	}

	h = (typeof(h)) b->b_rptr;

	freemsg(XCHG(&st->emor, NULL));

	b->b_rptr += sizeof(*h);
	b->b_wptr = b->b_rptr + h->b_wptr;
	b->b_band = h->b_band;
	b->b_flag = h->b_flag;
	b->b_csum = h->b_csum;
	b->b_datap->db_type = h->db_type;
	b->b_datap->db_flag = 0;

	st->exp = linkmsg(st->exp, b);

	if (h->b_cont == 0)
		putnext(q, XCHG(&st->exp, NULL));

	freeb(mp);
	return (0);
}

static int
n_data(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_info_ack(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_ok_ack(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_error_ack(struct st *st, queue_t *q, mblk_t *mp)
{
	/* If we are waiting bind ack or connect confirmation, set the failure value and wake the
	   open process. */
}
static int
n_bind_ack(struct st *st, queue_t *q, mblk_t *mp)
{
	/* Next step in the intialization sequence is to issue an N_CONN_REQ downstream to form the
	   connection. */
}
static int
n_conn_ind(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_conn_con(struct st *st, queue_t *q, mblk_t *mp)
{
	/* Next step in the intialization sequence is to issue an N_EXDATA_REQ downstream to
	   request the opening of a specific Stream on the remote host. */
}
static int
n_data_ind(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_exdata_ind(struct st *st, queue_t *q, mblk_t *mp)
{
	/* If we are waiting the response to our M_OPEN request, store the return value and wake up
	   the waiting open process. */
}
static int
n_datack_ind(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_reset_ind(struct st *st, queue_t *q, mblk_t *mp)
{
	/* If we are waiting connection confirmation, set the failure value and wake the open
	   process. */
}
static int
n_reset_con(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_discon_ind(struct st *st, queue_t *q, mblk_t *mp)
{
	/* If we are waiting connection confirmation, set the failure value and wake the open
	   process. */
}
static int
n_unitdata_ind(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_uderror_ind(struct st *st, queue_t *q, mblk_t *mp)
{
}
static int
n_other_ind(struct st *st, queue_t *q, mblk_t *mp)
{
}

static noinline fastcall int
st_r_msg_slow(queue_t *q, mblk_t *mp)
{
	struct st *st = ST_PRIV(q);

	switch (mp->b_datap->db_type) {
	case M_DATA:
		return n_data(st, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		if (mp->b_wptr >= mp->b_rptr + sizeof(np_ulong)) {
			switch (*(np_ulong *) mp->b_rptr) {
			case N_INFO_ACK:
				return n_info_ack(st, q, mp);
			case N_OK_ACK:
				return n_ok_ack(st, q, mp);
			case N_ERROR_ACK:
				return n_error_ack(st, q, mp);
			case N_BIND_ACK:
				return n_bind_ack(st, q, mp);
			case N_CONN_IND:
				return n_conn_ind(st, q, mp);
			case N_CONN_CON:
				return n_conn_con(st, q, mp);
			case N_DATA_IND:
				return n_data_ind(st, q, mp);
			case N_EXDATA_IND:
				return n_exdata_ind(st, q, mp);
			case N_DATACK_IND:
				return n_datack_ind(st, q, mp);
			case N_RESET_IND:
				return n_reset_ind(st, q, mp);
			case N_RESET_CON:
				return n_reset_con(st, q, mp);
			case N_DISCON_IND:
				return n_discon_ind(st, q, mp);
			case N_UNITDATA_IND:
				return n_unitdata_ind(st, q, mp);
			case N_UDERROR_IND:
				return n_uderror_ind(st, q, mp);
			default:
				return n_other_ind(st, q, mp);
			}
		}
		freemsg(mp);
		return (0);
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		return (0);
	default:
		if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band))
			return (-EBUSY);
		putnext(q, mp);
		return (0);
	}
}

static inline fastcall int
st_r_msg(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type == M_PROTO) {
		if (mp->b_wptr >= mp->b_wptr + sizeof(np_ulong)) {
			switch (*(np_ulong *) mp->b_rptr) {
			case N_DATA_IND:
				return st_r_data(q, mp);
			case N_EXDATA_IND:
				return st_r_exdata(q, mp);
			default:
				break;
			}
		}
	}
	return st_r_msg_slow(q, mp);
}

static streamscall int
st_rput(queue_t *q, mblk_t *mp)
{
	if ((mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || st_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall int
st_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (st_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall int
st_wput(queue_t *q, mblk_t *mp)
{
	if ((mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || st_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall int
st_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (st_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall int
st_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	N_bind_req_t *p;
	struct st *st;
	struct sv *sv;
	mblk_t *mp;
	int err;
	pl_t pl;

	if (q->q_ptr)
		return (0);	/* already open */

	if (!(sv = SV_ALLOC(KM_SLEEP)))
		return (ENOMEM);

	if (!(mp = allocb(sizeof(*p), BPRI_WAITOK))) {
		SV_DEALLOC(sv);
		return (ENOBUFS);
	}

	if ((err = mi_open_com(&st_opens, sizeof(*st), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		SV_DEALLOC(sv);
		return (err);
	}

	st = ST_PRIV(q);
	st->sv = sv;		/* make sure this is volatile */
	/* initialize strcuture some more */
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_rptr;
	p->PRIM_type = N_BIND_REQ;
	p->ADDR_length = 0;
	p->ADDR_offset = 0;
	p->CONIND_number = 0;
	p->BIND_flags = 0;
	p->PROTOID_length = 0;
	p->PROTOID_offset = 0;
	qprocson(q);
	putnext(q, mp);

	/* wait here until the entire open operations complete */
	while (st->sv) {
		pl = LOCK(&st->lock, plhigh);
		if (st->sv) {
			SV_WAIT(&st->sv, &st->lock, plbase);
			continue;
		}
		UNLOCK(&st->lock, pl);
		break;
	}

	if ((err = st->err)) {
		qprocsoff(q);
		mi_close_comm(&st_opens, q);
		return (err);
	}
	return (0);
}

static streamscall int
st_qclose(queue_t *q, int oflags, cred_t *crp)
{
	mi_close_comm(&st_opens, q);
	return (0);
}

static struct qinit st_rinit = {
	.qi_putp = st_rput,
	.qi_srvp = st_rsrv,
	.qi_qopen = st_qopen,
	.qi_qclose = st_qclose,
	.qi_minfo = &st_minfo,
	.qi_mstat = &st_mstat,
};

static struct qinit st_winit = {
	.qi_putp = st_wput,
	.qi_srvp = st_wsrv,
	.qi_minfo = &st_minfo,
	.qi_mstat = &st_mstat,
};

static struct streamtab sctpstrinfo = {
	.st_rdinit = &st_rinit,
	.st_wrinit = &st_winit,
};

static struct fmodsw st_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sctpstrinfo,
	.f_flags = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
sctpstrinit(void)
{
	int err;

	cmn_err(CE_NOTE, SCTPSTR_BANNER);
	if ((err = register_strmod(&st_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
sctpstrexit(void)
{
	int err;

	if ((err = unregsiter_strmod(&st_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(sctpstrinit);
module_exit(sctpstrexit);
