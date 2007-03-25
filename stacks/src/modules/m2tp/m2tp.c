/*****************************************************************************

 @(#) $RCSfile: m2tp.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2007/03/25 00:51:48 $

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

 Last Modified $Date: 2007/03/25 00:51:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2tp.c,v $
 Revision 0.9.2.12  2007/03/25 00:51:48  brian
 - synchronization updates

 Revision 0.9.2.11  2006/03/07 01:09:55  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: m2tp.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2007/03/25 00:51:48 $"

static char const ident[] = "$RCSfile: m2tp.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2007/03/25 00:51:48 $";

/*
 *  This is a M2TP/SCTP driver.  This simulates one or more SS7 links using an
 *  SCTP association.  It is similar to the M2TP/UDP driver except that it has
 *  different path characteristics.
 */
#include <sys/os7/compat.h>

#include <sys/tpi.h>
#include <sys/tpi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#include <ss7/m2tp_ioctl.h>

#define M2TP_DESCRIP	"M2TP/SCTP MTP2 TUNNELING PROTOCOL (SL) STREAMS MODULE."
#define M2TP_REVISION	"OpenSS7 $RCSfile: m2tp.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Data$"
#define M2TP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define M2TP_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define M2TP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define M2TP_LICENSE	"GPL"
#define M2TP_BANNER	M2TP_DESCRIP	"\n" \
			M2TP_REVISION	"\n" \
			M2TP_COPYRIGHT	"\n" \
			M2TP_DEVICE	"\n" \
			M2TP_CONTACT	"\n"
#define M2TP_SPLASH	M2TP_DEVICE	" - " \
			M2TP_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M2TP_CONTACT);
MODULE_DESCRIPTION(M2TP_DESCRIP);
MODULE_SUPPORTED_DEVICE(M2TP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M2TP_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m2tp");
#endif
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID	    M2TP_MOD_ID
#define MOD_NAME    M2TP_MOD_NAME

static struct module_info m2tp_minfo = {
	M2TP_MOD_ID,			/* Module ID number *//* FIXME */
	M2TP_MOD_NAME,			/* Module name */
	1,				/* Min packet size accepted */
	512,				/* Max packet size accepted */
	8 * 512,			/* Hi water mark */
	1 * 512				/* Lo water mark */
};

static int m2tp_rput(queue_t *, mblk_t *);
static int m2tp_rsrv(queue_t *);
static int m2tp_open(queue_t *, dev_t *, int, int, cred_t *);
static int m2tp_close(queue_t *, int, cred_t *);

static struct qinit m2tp_rinit = {
	m2tp_rput,			/* Read put (msg from below) */
	m2tp_rsrv,			/* Read queue service */
	m2tp_open,			/* Each open */
	m2tp_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&m2tp_minfo,			/* Information */
	NULL				/* Statistics */
};

static int m2tp_wput(queue_t *, mblk_t *);
static int m2tp_wsrv(queue_t *);

static struct qinit m2tp_winit = {
	m2tp_wput,			/* Write put (msg from above) */
	m2tp_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&m2tp_minfo,			/* Information */
	NULL				/* Statistics */
};

MODULE_STATIC struct streamtab m2tp_info = {
	&m2tp_rinit,			/* Upper read queue */
	&m2tp_winit,			/* Upper write queue */
	NULL,				/* Lower read queue */
	NULL				/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  M2TP Private Structure
 *
 *  =========================================================================
 */

typedef struct m2tp m2tp_t;

struct m2tp {
	struct m2tp *next;
	queue_t *rq;			/* STREAM read queue */
	queue_t *wq;			/* STREAM write queue */
	uint state;			/* M2TP state */
	uint t_state;			/* T-Provider state */
	sdt_state_t sdt_state;		/* Signalling Link State */

	sdt_info_ack_t info;

	struct {
		int state;
		int prim;
		int err;
		int reason;
	} d;
} m2tp_t;

#define M2TP_STATUS_IN_SERVICE		(__constant_htonl(1))
#define	M2TP_STATUS_PROCESSOR_OUTAGE	(__constant_htonl(2))
#define M2TP_STATUS_PROCESSOR_ENDED	(__constant_htonl(3))
#define M2TP_STATUS_BUSY		(__constant_htonl(4))
#define M2TP_STATUS_BUSY_ENDED		(__constant_htnol(5))

/*
 *  =========================================================================
 *
 *  M2TP-Provider --> SDT-User Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */
static inline int
sdt_info_ack(m2tp_t * mt)
{
	mblk_t *mp;
	sdt_info_ack_t *p;
	if ((mp = allocb(sizeof(*p)), BPRI_HI)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (sdt_info_ack_t *) mp->b_wptr;
		*p = mt->info;
		mp->b_wptr += sizeof(*p);
		putnext(mt->rq, mp);
		return (0);
	}
	return -ENOBUFS;
}
static inline int
sdt_ok_ack(m2tp_t * mt)
{
	mblk_t *mp;
	sdt_ok_ack_t *p;
	if ((mp = allocb(sizeof(*p)), BPRI_HI)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (sdt_ok_ack_t *) mp->b_wptr;
		p->sdt_primitive = SDT_OK_ACK;
		p->sdt_correct_primitive = mt->d.prim;
		p->sdt_state = mt->d.state;
		mp->b_wptr += sizeof(*p);
		putnext(mt->rq, mp);
		return (0);
	}
	return -ENOBUFS;
}
static inline int
sdt_error_ack(m2tp_t * mt)
{
	mblk_t *mp;
	sdt_error_ack_t *p;
	if ((mp = allocb(sizeof(*p)), BPRI_HI)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (sdt_error_ack_t *) mp->b_wptr;
		p->sdt_primitive = SDT_ERROR_ACK;
		p->sdt_errno = mt->d.err;
		p->sdt_reason = mt->d.reason;
		p->sdt_error_primitive = mt->d.prim;
		p->sdt_state = mt->d.state;
		mp->b_wptr += sizeof(*p);
		putnext(mt->rq, mp);
		return (0);
	}
	return -ENOBUFS;
}
static inline int
sdt_optmgmt_ack(m2tp_t * mt, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	sdt_optmgmt_ack_t *p;
	if ((mp = allocb(sizeof(*p) + opt_len), BPRI_HI)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (sdt_optmgmt_ack_t *) mp->b_wptr;
		p->sdt_primitive = SDT_ERROR_ACK;
		p->sdt_opt_offset = sizeof(*p);
		p->sdt_opt_length = opt_len;
		p->sdt_mgmt_flags = mt->opt_flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		putnext(mt->rq, mp);
		return (0);
	}
	return -ENOBUFS;
}

static inline int
sdt_uprim(m2tp_t * mt, int type, int prim, mblk_t *dp)
{
	mblk_t *mp;
	if ((mp = allocb(sizeof(long)), BPRI_HI)) {
		mp->b_datap->db_type = type;
		*((long) mp->b_wptr)++ = prim;
		mp->b_cont = dp;
		putnext(mt->rq, mp);
		return (0);
	}
	return -ENOBUFS;
}
static inline int
sdt_event_ind(m2tp_t * mt)
{
	return sdt_uprim(m2, M_PROTO, SDT_EVENT_IND, NULL);
}
static inline int
sdt_rc_signal_unit_ind(m2tp_t * mt, mblk_t *dp)
{
	return sdt_uprim(m2, M_PROTO, SDT_RC_SIGNAL_UNIT_IND, dp);
}
static inline int
sdt_rc_congestion_accept_ind(m2tp_t * mt)
{
	return sdt_uprim(m2, M_PCPROTO, SDT_RC_CONGESTION_ACCEPT_IND, NULL);
}
static inline int
sdt_rc_congestion_discard_ind(m2tp_t * mt)
{
	return sdt_uprim(m2, M_PCPROTO, SDT_RC_CONGESTION_DISCARD_IND, NULL);
}
static inline int
sdt_rc_no_congestion_ind(m2tp_t * mt)
{
	return sdt_uprim(m2, M_PCPROTO, SDT_RC_NO_CONGESTION_IND, NULL);
}
static inline int
sdt_iac_correct_su_ind(m2tp_t * mt)
{
	return sdt_uprim(m2, M_PROTO, SDT_IAC_CORRECT_SU_IND, NULL);
}
static inline int
sdt_iac_abort_proving_ind(m2tp_t * mt)
{
	return sdt_uprim(m2, M_PROTO, SDT_IAC_ABORT_PROVING_IND, NULL);
}
static inline int
sdt_lsc_link_failure_ind(m2tp_t * mt)
{
	return sdt_uprim(m2, M_PCPROTO, SDT_LSC_LINK_FAILURE_IND, NULL);
}
static inline int
sdt_txc_transmission_request_ind(m2tp_t * mt)
{
	return sdt_uprim(m2, M_PROTO, SDT_TXC_TRANSMISSION_REQUEST_IND, NULL);
}

/*
 *  =========================================================================
 *
 *  M2TP-Provider --> T-Provider (SCTP) Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */
/*
 *  T_INFO_REQ
 *
 *  We might use this primitive if the T-Provider is not SCTP to check the
 *  capabilities of the T-Provider before exchanging any other primitives.
 */
static int
t_info_req(m2tp_t * mt)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_BIND_REQ
 *
 *  This is used by M2TP to bind the T-Provider to a local transport address
 *  before attempting to connect the T-Provider.
 */
static int
t_bind_req(m2tp_t * mt)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_OPTMGMT_REQ
 *
 *  This might be used by M2TP to set some local and associative options
 *  before establishing a connection.
 */
static int
t_optmgmt_req(m2tp_t * mt)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_ADDR_REQ
 *
 *  This is really not necessary, but it is here anyways.  Particularly if
 *  there is a non-SCTP T-Providder which does not provide the address
 *  information in a usable (or storable) format in the appropriate
 *  primitives.  This should be unnecessary for the OpenSS7 SCTP T-Provider.
 */
static int
t_addr_req(m2tp_t * mt)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_CONN_REQ
 *
 *  This is used to actively request a connection to the transport peer.
 */
static int
t_conn_req(m2tp_t * mt, mblk_t *dp)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_CONN_RES
 *
 *  This is used to accept a passive connection from the transport peer.  We
 *  use XTI/TLI semantics here an always accept the connection on the same
 *  stream which was listening.  Remember that SCTP can provide a T_CONN_IND
 *  while in the T_DATAXFER state (for SCTP Restart) which must also be
 *  accepted on this stream.
 */
static int
t_conn_res(m2tp_t * mt, mblk_t *dp)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_DATA_REQ
 *
 *  This is used to send normal (ordered) messages on the control or default
 *  SCTP-stream (stream 0).
 *
 */
static int
t_data_req(m2tp_t * mt, mblk_t *dp)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_EXDATA_REQ
 *
 *  This is used to send expedited (really, unordered) messages on the control
 *  or default SCTP-stream (stream 0).
 */
static int
t_exdata_req(m2tp_t * mt, mblk_t *dp)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_OPTDATA_REQ
 *
 *  This is used to send normal (ordered) or expedited (unordered) data on a
 *  specific stream.
 */
static int
t_optdata_req(m2tp_t * mt, mblk_t *dp)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_DISCON_REQ
 *
 *  This is used to ABORT the SCTP association under brutal circumstances.
 */
static int
t_discon_req(m2tp_t * mt, mblk_t *dp)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_ORDREL_REQ
 *
 *  This is used to peform a SHUTDOWN on the SCTP association.
 */
static int
t_ordrel_req(m2tp_t * mt, mblk_t *dp)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  T_UNITDATA_REQ
 *
 *  This is not normally used for a SCTP T-Provider (which is *  connection-
 *  oriented only).  However, if we use a connectionless T-Provider we may use
 *  these messages, so here it is.
 */
static int
t_unitdata_req(m2tp_t * mt, mblk_t *dp)
{
	ptrace(("Unimplemented T-Provider primitive invoked\n"));
	(void) mt;
	(void) mp;
	return (-EFAULT);
}

/*
 *  Mapping of states:
 *
 *  M2TP State              T-Provider State(s)
 *  -------------------     --------------------------------------------
 *  M2TP_STATE_IDLE         (uninitialized)
 *
 *  M2TP_STATE_OOS          TS_UNBND TS_WACK_UREQ TS_IDLE TS_WACK_OPREQ
 *                          TS_WACK_DREQ6 TS_WACK_DREQ7 TS_WACK_DREQ9
 *                          TS_WACK_DREQ10 TS_WACK_DREQ11 TS_WIND_ORDREL
 *                          TS_WACK_CREQ TS_WACK_CRES
 *
 *  M2TP_STATE_AIP          TS_WACK_BREQ TS_IDLE TS_WCON_CREQ TS_WRES_CIND
 *                          TS_WACK_OPREQ TS_WACK_CREQ TS_WACK_CRES
 *
 *  M2TP_STATE_INS_LOCAL    TS_DATA_XFER
 *
 *  M2TP_STATE_INS          TS_DATA_XFER TS_WREQ_ORDREL
 *
 *  M2TP_RETRIEVAL          TS_WIND_ORDREL TS_WACK_DREQ6 TS_WACK_DREQ7
 *                          TS_WACK_DREQ9 TS_WACK_DREQ10 TS_WACK_DREQ11
 *                          TS_WACK_CREQ TS_WACK_CRES TS_IDLE TS_WACK_UREQ
 *                          TS_UNBND
 *  -------------------     --------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  M2TP-Provider --> M2TP-Peer Primitives (Sent Messages)
 *
 *  =========================================================================
 */
static inline int
m2tp_send_data(m2tp_t * mt, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_ata_req_t *) mp->b_wptr;
		p->PRIM_type = T_OPTDATA_REQ;
		p->MORE_flag = 0;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		putnext(mt->wq, mp);
		return (0);
	}
	return -ENOBUFS;
}

/*
 *  =========================================================================
 *
 *  M2TP-Peer --> M2TP-Provider Primitives (Received Messages)
 *
 *  =========================================================================
 */
static inline int
m2tp_recv_data(m2tp_t * mt, mblk_t *dp)
{
	return sdt_rc_signal_unit_ind(m2, dp);
}

/*
 *  =========================================================================
 *
 *  SDT-User --> M2TP-Provider Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 *
 *  SDT_INFO_REQ:
 *  -------------------------------------------------------------------------
 */
static int
sdt_info_req(m2tp_t * mt, mblk_t *mp)
{
	lmi_info_req_t *p = (lmi_info_req_t *) mp->b_wptr;

	ensure(p->lmi_primitive == LMI_INFO_REQ, return -EFAULT);

	if (sdt_info_ack(mt) == -ENOBUFS)
		return -EAGAIN;

	freemsg(mp);
	return (0);
}

/*
 *  SDT_ATTACH_REQ:
 *  -------------------------------------------------------------------------
 *  This is the attach request.  We take the PPA address and use it directly
 *  as an address to the T_BIND_REQ.
 *
 */
static int
sdt_attach_req(m2tp_t * mt, mblk_t *mp)
{
	lmi_attach_req_t *p = (lmi_attach_req_t *) mp->b_rptr;
	caddr_t ppa_ptr = mp->b_rptr + p->ppa_offset;
	size_t ppa_len = p->ppa_length;

	ensure(p->lmi_primitive == LMI_ATTACH_REQ, return -EFAULT);

	if (mt->d.state != LMI_UNATTACHED || mt->t_state != TS_UNBND) {
		mt->d.err = LMI_OUTSTATE;
		if (sdt_error_ack(mt) == -ENOBUFS)
			return -EAGAIN;
		return (0);
	}
	if (t_bind_req(mt, ppa_ptr, ppa_len) == -ENOBUFS)
		return -EAGAIN;

	mt->t_state = TS_WACK_BREQ;
	freemsg(mp);
	return (0);
}

/*
 *  SDT_DETACH_REQ:
 *  -------------------------------------------------------------------------
 *  This is the detach request.  We can now unbind the T-provider.
 */
static int
sdt_detach_req(m2tp_t * mt, mblk_t *mp)
{
	lmi_detach_req_t *p = (lmi_detach_req_t *) mp->b_wptr;

	ensure(p->lmi_primitive == LMI_DETACH_REQ, return -EFAULT);

	if (mt->d.state != LMI_DISABLED || mt->t_state != TS_IDLE) {
		mt->d.err = LMI_OUTSTATE;
		if (sdt_error_ack(mt) == -ENOBUFS)
			return -EAGAIN;
		return (0);
	}
	if (t_unbind_req(mt) == -ENOBUFS)
		return -EAGAIN;

	mt->t_state = TS_WACK_UREQ;
	freemsg(mp);
	return (0);
}

/*
 *  SDT_ENABLE_REQ:
 *  -------------------------------------------------------------------------
 *
 */
/*
 *  SDT_DISABLE_REQ:
 *  -------------------------------------------------------------------------
 *
 */
/*
 *  SDT_OPTMGMT_REQ:
 *  -------------------------------------------------------------------------
 *
 */
static int
sdt_optmgmt_req(m2tp_t * mt, mblk_t *mp)
{
}

/*
 *  SDT_DAEDT_TRANSMISSION_REQ:
 *  -------------------------------------------------------------------------
 *  A transmission request.  Just feed the data blocks down to the transport
 *  provider.
 */
static int
sdt_daedt_transmission_req(m2tp_t * mt, mblk_t *mp)
{
	mblk_t *dp;
	if ((dp = mp)->b_datap->db_type != M_DATA) {
		dp = mp->b_cont;
		freeb(mp);
	}
	if (dp)
		putnext(mt->wq, dp);
	return (0);
}

/*
 *  SDT_DAEDT_START_REQ:
 *  -------------------------------------------------------------------------
 *  We ignore these signals for M2TP.  Once the transport connection has been
 *  established by attaching and enabling the Signalling Data Terminal, the
 *  transmitters are already available.  This is only a power-on signal.  For
 *  SCTP we want to set the initial transmit options here.
 */
static int
sdt_daedt_start_req(m2tp_t * mt, mblk_t *mp)
{
	static struct {
		struct sctp_opt_hb hb;
		struct sctp_opt_rto rto;
		struct sctp_opt_dest dst;
		struct sctp_opt_sctp sctp;
	} opt = {
		{ {
		T_INET_SCTP, T_SCTP_HB, sizeof(opt.hb), 0}
		, 0,}
		, { {
		T_INET_SCTP, T_SCTP_RTO, sizeof(opt.rto), 0}
		, 0,}
		, { {
		T_INET_SCTP, T_SCTP_DEST, sizeof(opt.dest), 0}
		, 0,}
		, { {
		T_INET_SCTP, T_SCTP_SCTP, sizeof(opt.sctp), 0}
		, 0,}
	};

	mt->err = 0;
	mt->flags &= ~M2TP_TX_FAIL;

	if (t_optmgmt_req(mt, T_NEGOTIATE, &opt, sizeof(opt)) == -ENOBUFS)
		return -EAGAIN;

	if (mt->merr) {
		mt->flags |= M2TP_TX_FAIL;
		return -EFAULT;
	}

	mt->flags |= M2TP_DAEDT_IN_SERVICE;
	freemsg(mp);
	return (0);
}

/*
 *  SDT_DAEDR_START_REQ:
 *  -------------------------------------------------------------------------
 *  This signal tells us when we can start receiving messages.  We will dump
 *  all transport data messages received until this signal is received.  This
 *  is a power-on signal.  For SCTP we want to set the initial receive options
 *  here.
 */
static int
sdt_daedr_start_req(m2tp_t * mt, mblk_t *mp)
{
	static struct {
		struct sctp_opt_hb hb;
		struct sctp_opt_rto rto;
		struct sctp_opt_dest dst;
		struct sctp_opt_sctp sctp;
	} opt = {
		{ {
		T_INET_SCTP, T_SCTP_HB, sizeof(opt.hb), 0}
		, 0,}
		, { {
		T_INET_SCTP, T_SCTP_RTO, sizeof(opt.rto), 0}
		, 0,}
		, { {
		T_INET_SCTP, T_SCTP_DEST, sizeof(opt.dest), 0}
		, 0,}
		, { {
		T_INET_SCTP, T_SCTP_SCTP, sizeof(opt.sctp), 0}
		, 0,}
	};

	mt->err = 0;
	mt->flags &= ~M2TP_RX_FAIL;

	if (t_optmgmt_req(mt, T_NEGOTIATE, &opt, sizeof(opt)) == -ENOBUFS)
		return -EAGAIN;

	if (mt->err) {
		mt->flags |= M2TP_RX_FAIL;
		return -EFAULT;
	}

	mt->flags |= M2TP_DAEDR_IN_SERVICE;
	freemsg(mp);
	return (0);
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  SCTP OPTIONS used for AERM and SUERM functions.
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  These are the SCTP options which we use when performing Normal alignment
 *  and Emergency alignment.  For both alignments we want to "prove" the SCTP
 *  association, so arrange to generate some heartbeats and set the other
 *  parameters (retrans maxes, rto max, heartbeat interval) so that the
 *  association will fail within the appropriate proving period (normal or
 *  emergency) is the association is unstable.
 */
static struct m2tp_options {
	struct sctp_opt hb hb;
	struct sctp_opt_rto rot;
	struct sctp_opt_dest dst;
	struct sctp_opt_sctp sctp;
} m2tp_norm_options = {
	{ {
	T_INET_SCTP, T_SCTP_HB, sizeof(opt.hb), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_RTO, sizeof(opt.rto), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_DEST, sizeof(opt.dest), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_SCTP, sizeof(opt.sctp), 0}
	, 0,}
}

, m2tp_emer_options = {
	{ {
	T_INET_SCTP, T_SCTP_HB, sizeof(opt.hb), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_RTO, sizeof(opt.rto), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_DEST, sizeof(opt.dest), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_SCTP, sizeof(opt.sctp), 0}
	, 0,}
}

, m2tp_stop_options = {
	{ {
	T_INET_SCTP, T_SCTP_HB, sizeof(opt.hb), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_RTO, sizeof(opt.rto), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_DEST, sizeof(opt.dest), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_SCTP, sizeof(opt.sctp), 0}
	, 0,}
}

, m2tp_init_options = {
	{ {
	T_INET_SCTP, T_SCTP_HB, sizeof(opt.hb), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_RTO, sizeof(opt.rto), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_DEST, sizeof(opt.dest), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_SCTP, sizeof(opt.sctp), 0}
	, 0,}
}

, m2tp_estb_options = {
	{ {
	T_INET_SCTP, T_SCTP_HB, sizeof(opt.hb), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_RTO, sizeof(opt.rto), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_DEST, sizeof(opt.dest), 0}
	, 0,}
	, { {
	T_INET_SCTP, T_SCTP_SCTP, sizeof(opt.sctp), 0}
	, 0,}
};

/*
 *  SDT_AERM_START_REQ:
 *  -------------------------------------------------------------------------
 *  This signal tells us when we can start sending alignment error monitor
 *  signals.  For SCTP we monitor constantly, so there is no need to turn
 *  monitoring on or off, we just need to know what signals to delivery when.
 */
static int
sdt_aerm_start_req(m2tp_t * mt, mblk_t *mp)
{
	if (mt->t_state == TS_DATA_XFER)
		if (!(mt->flags & (M2TP_RX_FAIL | M2TP_TX_FAIL))) {
			struct m2tp_options *opt =
			    (m2->flags & M2TP_EMERGENCY) ? &m2tp_emer_options : &m2tp_norm_options;

			mt->oflags |= M2TP_RX_FAIL | M2TP_TX_FAIL;

			if (t_optmgmt_req(mt, T_NEGOTIATE, opt, sizeof(*opt)) == -ENOBUFS)
				return -EAGAIN;

			mt->flags |= M2TP_AERM_IN_SERVICE;
		}
	freemsg(mp);
	return (0);
}

/*
 *  SDT_AERM_STOP_REQ:
 *  -------------------------------------------------------------------------
 *  This signal tells us when we must stop sending alignment error monitor
 *  signals.  For SCTP we monitor constantly, so there is no need to turn
 *  monitoring on or off, we just need to know what signals to deliver when.
 *  For SCTP we may want to turn off heartbeating.
 */
static int
sdt_aerm_stop_req(m2tp_t * mt, mblk_t *mp)
{
	if (mt->t_state == TS_DATA_XFER)
		if (!(mt->flags & (M2TP_RX_FAIL | M2TP_TX_FAIL))) {
			struct m2tp_options *opt = &m2tp_stop_options;

			mt->oflags |= M2TP_RX_FAIL | M2TP_TX_FAIL;

			if (t_optmgmt_req(mt, T_NEGOTIATE, opt, sizeof(*opt)) == -ENOBUFS)
				return -EAGAIN;
		}

	m2->flags &= ~M2TP_AERM_IN_SERVICE;
	freemsg(mp);
	return (0);
}

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ:
 *  -------------------------------------------------------------------------
 *  This signal tells us that we are performing normal alignment procedures.
 *  We may want to adjust some of the T-Provider parameters for error
 *  monitoring or QOS.  For example, for SCTP we may want to adjust the
 *  heartbeat interval, Assoc.Max.Retrans and Path.Max.Retrans and RTO.max for
 *  the longer proving period.
 */
static int
sdt_aerm_set_ti_to_tin_req(m2tp_t * mt, mblk_t *mp)
{
	/*
	   we don't change proving parameters in the middle of proving 
	 */
	m2->flags &= ~M2TP_EMERGENCY;
	freemsg(mp);
	return (0);
}

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ:
 *  -------------------------------------------------------------------------
 *  This signal tells us that we are performing emergency alignment
 *  procedures.  We may want to adjust some of the T-Provider parameters for
 *  error monitoring or QOS.  For example, for SCTP we may want to adjust the
 *  heartbeat interval, Assoc.Max.Retrans and Path.Max.Retrans and RTO.max for
 *  the shorter proving period.
 */
static int
sdt_aerm_set_ti_to_tie_req(m2tp_t * mt, mblk_t *mp)
{
	/*
	   we don't change proving parameters in the middle of proving 
	 */
	m2->flags |= M2TP_EMERGENCY;
	freemsg(mp);
	return (0);
}

/*
 *  SDT_SUERM_START_REQ:
 *  -------------------------------------------------------------------------
 *  This signal tells us when we must start sending error monitor signals.
 *  For reliable transport providers we monitor constantly, so there is no
 *  need to turn monitoring on or off, we just need to know what signals to
 *  deliver when.  For SCTP we may want to adjust the heartbeat interval,
 *  Assoc.Max.Retrans and Path.Max.Retrans and RTO.max for stable error
 *  monitoring.
 */
static int
sdt_suerm_start_req(m2tp_t * mt, mblk_t *mp)
{
	if (mt->t_state == TS_DATA_XFER)
		if (!(mt->flags & (M2TP_RX_FAIL | M2TP_TX_FAIL))) {
			struct m2tp_options *opt = &m2tp_estb_options;

			mt->oflags |= M2TP_RX_FAIL | M2TP_TX_FAIL;

			if (t_optmgmt_req(mt, T_NEGOTIATE, &opt, sizeof(opt)) == -ENOBUFS)
				return -EAGAIN;

			m2->flags |= M2TP_SUERM_IN_SERVICE;
		}
	freemsg(mp);
	return (0);
}

/*
 *  SDT_SUERM_STOP_REQ:
 *  -------------------------------------------------------------------------
 *  This signal tells use when we must stop sending error monitor signals.
 *  For reliable transport providers, we monitor constantly, so there is no
 *  need to turn monitoring on or off, we just need to know what signals to
 *  deliver when.  For SCTP we may want to turn off hearbeating.
 */
static int
sdt_suerm_stop_req(m2tp_t * mt, mblk_t *mp)
{
	if (mt->t_state == TS_DATA_XFER)
		if (!(mt->flags & (M2TP_RX_FAIL | M2TP_TX_FAIL))) {
			struct m2tp_options *opt = &m2tp_stop_options;

			mt->oflags |= M2TP_RX_FAIL | M2TP_TX_FAIL;

			if (t_optmgmt_req(mt, T_NEGOTIATE, &opt, sizeof(opt)) == -ENOBUFS)
				return -EAGAIN;
		}
	m2->flags &= ~M2TP_SUERM_IN_SERVICE;
	freemsg(mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  T-Provider (SCTP) --> M2TP-Provider Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 *
 *  T_INFO_ACK
 *
 *  We might need this if we are using other transports and we send a
 *  T_INFO_REQ to the T-Provider to see what semantics we should use for
 *  encapsulating messages and stuff.  If we are only using SCTP, we know what
 *  to expect and will never see this message.
 */
static int
t_info_ack(m2tp_t * mt, mblk_t *mp)
{
	struct T_info_ack *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_BIND_ACK
 *
 *  SCTP will send us this response when we go to bind the stream for
 *  connection or listening.  We should acknowledge this fact and move to the
 *  next state in attempting to bring up the SCTP association.
 */
static int
t_bind_ack(m2tp_t * mt, mblk_t *mp)
{
	struct T_bind_ack *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_OPTMGMT_ACK
 *
 *  SCTP will send us this repsonse when we use a T_OPTMGMT_REQ to set the
 *  number of inbound and outbound SCTP-streams.  Actually, we never really
 *  need this message (T_CONN_REQ and T_CONN_RES work just fine) so we should
 *  not expect this message.  This is a T-Provider error.  We should M_ERROR
 *  out the stream.
 */
static int
t_optmgmt_ack(m2tp_t * mt, mblk_t *mp)
{
	struct T_optmgmt_ack *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_ADDR_ACK
 *
 *  SCTP should no send us this message, because we never send a T_ADDR_REQ to
 *  SCTP, so we do not expect the response.  This is a T-Provider error.  We
 *  should M_ERROR out the queue.
 */
static int
t_addr_ack(m2tp_t * mt, mblk_t *mp)
{
	struct T_addr_ack *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_OK_ACK
 *
 *  SCTP has acknowledged receipt of our last primitive.  We can move to the
 *  successful state.
 */
static int
t_ok_ack(m2tp_t * mt, mblk_t *mp)
{
	struct T_ok_ack *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_ERROR_ACK
 *
 *  SCTP has refused our last primitive.  We can either respond with an error
 *  to the M2TP user or we will have to M_ERROR out the stream.
 */
static int
t_error_ack(m2tp_t * mt, mblk_t *mp)
{
	struct T_error_ack *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_CONN_IND
 *
 *  SCTP has given us a connection indication.  Even though we might not set
 *  the CONIND_number to a positive integer (we set it to zero), we may still
 *  receive a T_CONN_IND if the SCTP association has restarted.  We should
 *  take appropriate action depending on what we think the state of the
 *  association is: if we are waiting to connect we should accept the
 *  connection with a T_CONN_RES with the current stream, or we should refuse
 *  the request with a T_DISCON_REQ.
 */
static int
t_conn_ind(m2tp_t * mt, mblk_t *mp)
{
	struct T_conn_ind *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_CONN_CON
 *
 *  SCTP has informed us that our connection request was successful.  We
 *  should inform the M2TP user and get ready to transfer data.
 */
static int
t_conn_con(m2tp_t * mt, mblk_t *mp)
{
	if (mt->t_state == TS_WCON_CREQ) {
		struct T_conn_con *p = (struct T_conn_con *) mp->b_wptr;

		/*
		 *  TODO: check for 2 streams inbound only.
		 */
		switch (mt->state) {
		case M2TP_AIP:
			mt->state = M2TP_INS_LOCAL;
			mt->t_state = TS_DATA_XFER;
			/*
			 *  TODO: Get SRTT report.
			 */
			if (mp->b_cont)
				m2tp_recv_data(mp->b_cont);
			if (srtt <= mt->srtt_max) {
				if (m2tp_send_in_service(mt) == -ENOBUFS)
					return -EAGAIN;
			} else {
				mt->failure_reason = FIXME;
				t_discon_req(mt);
				mt->t_state = TS_WACK_DREQ9;
				sdt_out_of_service(mt);
				mt->state = M2TP_OOS;
			}
			break;
		default:
		case M2TP_IDLE:
		case M2TP_OOS:
		case M2TP_RETRIEVAL:
		case M2TP_INS_LOCAL:
		case M2TP_INS:
			break;
		}
	}
	freemsg(mp);
	return (0);
}

/*
 *  T_DATA_IND
 *
 *  SCTP has given us data on STREAM 0.  This is a control channel for M2TP,
 *  so we should process these messages as control messages.
 */
static int
t_data_ind(m2tp_t * mt, mblk_t *mp)
{
	struct T_data_ind *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_EXDATA_IND
 *
 *  SCTP has given us out-of-order data on STREAM 0.  This is a control
 *  channel for M2TP so we should process these messages as control messages.
 */
static int
t_exdata_ind(m2tp_t * mt, mblk_t *mp)
{
	struct T_exdata_ind *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_OPTDATA_IND
 *
 *  SCTP has given us data on a specific SCTP-stream.  We should process this
 *  message for the SCTP-stream indicated.
 */
static int
t_optdata_ind(m2tp_t * mt, mblk_t *mp)
{
	struct T_optdata_ind *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_OPTDATA_CON (Not in XTI/TLI)
 *
 *  SCTP has given us an indication that an outstanding data message with
 *  given user and provider/peer token values has been acknowledged received
 *  by the peer.  We can strike this message from our retrieval buffers.
 */
static int
t_optdata_con(m2tp_t * mt, mblk_t *mp)
{
	struct T_optdata_con *p = (T_optdata_con *) mp->b_rptr;
	mblk_t *dp = (mblk_t *) p->TOKEN_value;
	mblk_t *bp = mt->txq.q_head;

	/*
	   little check just to be sure that it is in the bufq 
	 */
	for (; bp && bp != dp; bp = bp->next) ;
	if (bp != dp)
		return (-EPROTO);
	bufq_unlink(&mt->txq, dp);
	m2tp_recalc_congestion(mt);
	freemsg(mp);
	return (0);
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  T_DISCON_IND
 *
 *  We have received an abortive disconnect.  This is either a response to a
 *  previous T_CONN_REQ during the connection establishment phase or it is
 *  an abort from the data transfer or connection release phases.  Which phase
 *  we are in pretty much determines what we do in response to this
 *  indication.
 */
static int
t_discon_ind(m2tp_t * mt, mblk_t *mp)
{
	struct T_discon_ind *p = (struct T_discon_ind *) mp->b_wptr;

	mt->failure_reason = t_to_sl_reason(p->DISCON_reason);
	if (sdt_out_of_service_ind(mt) == -ENOBUFS)
		return -EAGAIN;
	switch (mt->state) {
	case M2TP_STATE_INS:
		mt->state = M2TP_STATE_RETRIEVAL;
		break;
	default:
		mt->state = M2TP_STATE_OOS;
		break;
	}
	mt->t_state = TS_IDLE;
	if (t_unbind_req(mt) == -ENOBUFS)
		return -EAGAIN;
	mt->t_state = TS_WACK_UREQ;
	freemsg(mp);
	return (0);
}

/*
 *  T_ORDREL_IND
 *
 *  We have received an orderly release indication from the peer.  This is
 *  peculiar if we are not expecting it.
 */
static int
t_ordrel_ind(m2tp_t * mt, mblk_t *mp)
{
	if (sdt_out_of_service_ind(mt) == -ENOBUFS)
		return -EAGAIN;
	if (t_ordrel_req(mt) == -ENOBUFS)
		return -EAGAIN;
	mt->state = M2TP_STATE_RETRIEVAL;
	mt->t_state = TS_IDLE;
	if (t_unbind_req(mt) == -ENOBUFS)
		return -EAGAIN;
	mt->t_state = TS_WACK_UREQ;
	freemsg(mp);
	return (0);
}

/*
 *  T_UNITDATA_IND
 *
 *  SCTP should not send us this message; however, we might want to use other
 *  transports for M2TP, so we process this message as well as possible.
 */
static int
t_unitdata_ind(m2tp_t * mt, mblk_t *mp)
{
	struct T_unitdata_ind *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  SCTP should not send us this message; however, we might want to use other
 *  transports for M2TP, so we process this message as well as possible.
 */
static int
t_uderror_ind(m2tp_t * mt, mblk_t *mp)
{
	struct T_uderror_ind *p;
	ptrace(("Unexpected primitive received from T-Provider\n"));
	(void) mt;
	(void) mp;
	return (-EPROTO);
}

/*
 *  =========================================================================
 *
 *  M2TP IOCTLs
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  Message Handling
 *
 *  =========================================================================
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 *
 *  M_DATA messages are data messages which are passed between protocol
 *  modules or between protocol modules and users.
 *  
 */
static inline int
m2tp_w_data(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	return m2tp_send_data(mt, mp);
}
static inline int
m2tp_r_data(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	return m2tp_recv_data(mt, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO Handling
 *
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO message are normal protocol messages which are passed between
 *  protocol modules or between protocol modules and users.
 */
static inline int
m2tp_w_proto(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	switch ((mt->dprim = ((union M2TP_primitives *) (mp->b_rptr))->type)) {
	case LMI_INFO_REQ:
		return sdt_info_req(mt, mp);
	case LMI_ATTACH_REQ:
		return sdt_attach_req(mt, mp);
	case LMI_DETACH_REQ:
		return sdt_detach_req(mt, mp);
	case LMI_ENABLE_REQ:
		return sdt_enable_req(mt, mp);
	case LMI_DISABLE_REQ:
		return sdt_disable_req(mt, mp);
	case LMI_OPTMGMT_REQ:
		return sdt_optmgmt_req(mt, mp);
	case SDT_DAEDT_TRANSMISSION_REQ:
		return sdt_daedt_transmission_req(mt, mp);
	case SDT_DAEDT_START_REQ:
		return sdt_daedt_start_req(mt, mp);
	case SDT_DAEDR_START_REQ:
		return sdt_daedr_start_req(mt, mp);
	case SDT_AERM_START_REQ:
		return sdt_aerm_start_req(mt, mp);
	case SDT_AERM_STOP_REQ:
		return sdt_aerm_stop_req(mt, mp);
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		return sdt_aerm_set_ti_to_tin_req(mt, mp);
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		return sdt_aerm_set_ti_to_tie_req(mt, mp);
	case SDT_SUERM_START_REQ:
		return sdt_suerm_start_req(mt, mp);
	case SDT_SUERM_STOP_REQ:
		return sdt_suerm_stop_req(mt, mp);
	}
	return (-EOPNOTSUPP);
}
static inline int
m2tp_r_proto(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	switch ((mt->uprim = ((union T_primitives *) (mp->b_rptr))->type)) {
	case T_DATA_IND:
		return t_data_ind(mt, mp);
	case T_EXDATA_IND:
		return t_exdata_ind(mt, mp);
	case T_OPTDATA_IND:
		return t_optdata_ind(mt, mp);
	case T_CONN_IND:
		return t_conn_ind(mt, mp);
	case T_CONN_CON:
		return t_conn_con(mt, mp);
	case T_DISCON_IND:
		return t_discon_ind(mt, mp);
	case T_ORDREL_IND:
		return t_ordrel_ind(mt, mp);
	case T_UNITDATA_IND:
		return t_unitdata_ind(mt, mp);
	case T_UDERROR_IND:
		return t_uderror_ind(mt, mp);
	}
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 *
 *  M_PCPROTO messages are priority protocol messages which are passed between
 *  protocol modules or between protocol modules and users.
 */
static inline int
m2tp_w_pcproto(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	switch ((mt->dprim = ((union M2TP_primitives *) (mp->b_rptr))->type)) {
	case SDT_INFO_REQ:
		return sdt_info_req(mt, mp);
	case SDT_OPTMGMT_REQ:
		return sdt_optmgmt_req(mt, mp);
	case SDT_DAEDT_TRANSMISSION_REQ:
		return sdt_daedt_transmission_req(mt, mp);
	case SDT_DAEDT_START_REQ:
		return sdt_daedt_start_req(mt, mp);
	case SDT_DAEDR_START_REQ:
		return sdt_daedr_start_req(mt, mp);
	case SDT_AERM_START_REQ:
		return sdt_aerm_start_req(mt, mp);
	case SDT_AERM_STOP_REQ:
		return sdt_aerm_stop_req(mt, mp);
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		return sdt_aerm_set_ti_to_tin_req(mt, mp);
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		return sdt_aerm_set_ti_to_tie_req(mt, mp);
	case SDT_SUERM_START_REQ:
		return sdt_suerm_start_req(mt, mp);
	case SDT_SUERM_STOP_REQ:
		return sdt_suerm_stop_req(mt, mp);
	}
	return (-EOPNOTSUPP);

}
static inline int
m2tp_r_pcproto(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	switch ((mt->uprim = ((union T_primitives *) (mp->b_rptr))->type)) {
	case T_INFO_ACK:
		return t_info_ack(mt, mp);
	case T_OPTMGMT_ACK:
		return t_optmgmt_ack(mt, mp);
	case T_BIND_ACK:
		return t_bind_ack(mt, mp);
	case T_OK_ACK:
		return t_ok_ack(mt, mp);
	case T_ERROR_ACK:
		return t_error_ack(mt, mp);
	case T_ADDR_ACK:
		return t_addr_ack(mt, mp);
		/*
		   new primitives for M2TP support 
		 */
	case T_OPTDATA_CON:
		return t_optdata_con(mt, mp);
	}
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_CTL Handling
 *
 *  -------------------------------------------------------------------------
 *
 *  M_CTL messages are used to pass control messages between protocol modules
 *  (not to or from user space).
 */
static inline int
m2tp_w_ctl(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	switch (((union M2TP_controls *) (mp->b_wptr))->type) {
	case M2TP_CTL_XXXX:
		return m2tp_ctl_xxxx(mt, mp);
	}
	return (-EOPNOTSUPP);
}
static inline int
m2tp_r_ctl(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	switch (((union M2TP_controls *) (mp->b_wptr))->type) {
	case T_CTL_XXXX:
		return t_ctl_xxxx(mt, mp);
	}
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
static inline int
m2tp_do_ioctl(m2tp_t * mt, int cmd, void *arg)
{
	int nr = _IOC_NR(cmd);
	(void) nr;

	if (_IOC_TYPE(cmd) == M2TP_IOC_MAGIC)
		switch (cmd) {
		case M2TP_IOCXXXXX:
			return m2tp_iocxxxx(mt, cmd, arg);
		case M2TP_IOCXXXXX:
			return m2tp_iocxxxx(mt, cmd, arg);
		case M2TP_IOCXXXXX:
			return m2tp_iocxxxx(mt, cmd, arg);
		case M2TP_IOCXXXXX:
			return m2tp_iocxxxx(mt, cmd, arg);
		}
	return -ENXIO;
}
static inline int
m2tp_w_ioctl(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int ret = -EINVAL;
	struct linkblk *lp = (struct linkblk *) arg;

	switch (cmd) {
	case I_STR:
		if (count >= _IOC_SIZE(cmd)) {
			ret = m2tp_do_ioctl(mt, cmd, arg);
		}
		if (ret == -ENXIO) {
			if (q->q_next) {
				putnext(q, mp);
				return (0);
			}
		}
		break;
	default:
	case I_LINK:
	case I_PLINK:
	case I_UNLINK:
	case I_PUNLINK:
		ret = -EINVAL;
		break;
	}
	if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
static inline void
m2tp_w_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHW) {
		flushq(q, FLUSHALL);
		if (q->q_next) {
			putnext(q, mp);
			return;
		}
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		flushq(RD(q), FLUSHALL);
		qreply(q, mp);
		return;
	}
	if (q->q_next) {
		putnext(q, mp);
		return;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
static inline void
m2tp_r_error(queue_t *q, mblk_t *mp)
{
	m2tp_t *mt = (m2tp_t *) q->q_ptr;
	m2tp->state = M2TP_STATE_MERROR;
	if (q->q_next) {
		putnext(q, mp);
		return;
	}
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE PUT and QUEUE SERVICE routines
 *
 *  =========================================================================
 *
 *  READ QUEUE PUT and SRV routines
 *
 *  -------------------------------------------------------------------------
 *
 *  M2TP RPUT - Message from below.
 *
 *  If the message is a priority message we attempt to process it immediately.
 *  If the message is a non-priority message, but there are no messages on the
 *  queue yet, we attempt to process it immediately.  If the message is not
 *  supported, we pass it down-queue if possible.  If the message cannot be
 *  processed immediately we place it on the queue.
 */
static int
m2tp_rput(queue_t *q, mblk_t *mp)
{
	int err = -EOPNOTSUPP;

	if (mp->b_datap->db_type < QPCTL && q->q_count) {
		putq(q, mp);
		/*
		 *  NOTE:- after placing messages on the queue here, I should
		 *  check if placing the message on the queue crosses a band
		 *  threshold for congestion accept and congestion discard.
		 *  When crossing congestion accept, I should sent busy to the
		 *  peer and notify MTP3.  When crossing congestion discard I
		 *  should notify MTP3.
		 */
		return (0);
	}
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if ((err = m2tp_r_data(q, mp)))
			break;
		return (0);
	case M_PROTO:
		if ((err = m2tp_r_proto(q, mp)))
			break;
		return (0);
	case M_PCPROTO:
		if ((err = m2tp_r_pcproto(q, mp)))
			break;
		return (0);
	case M_CTL:
		if ((err = m2tp_r_ctl(q, mp)))
			break;
		return (0);
	case M_ERROR:
		m2tp_r_error(q, mp);
		return (0);
	}
	switch (err) {
	case -EAGAIN:
		putq(q, mp);
		return (0);
	case -EOPNOTSUPP:
		if (q->q_next) {
			putnext(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (err);
}

/*
 *  M2TP RSRV - Queued message from below.
 *
 *  If the message is a priority message we attempt to process it immediately
 *  and without flow control.  If the message is a non-priority message and
 *  the next queue is flow controlled, we put the message back on the queue
 *  and wait.  If we cannot process a priority message immediately we cannot
 *  place it back on the queue and discard it.  We requeue non-priority
 *  messages which cannot be processed immediately.  Unrecognized messages are
 *  passed down-queue.
 */
static int
m2tp_rsrv(queue_t *q)
{
	mblk_t *mp;
	int err = -EOPNOTSUPP;

	while ((mp = getq(q))) {
		if (mp->b_datap->db_type < QPCTL && !canputnext(q)) {
			putbq(q, mp);
			return (0);
		}
		switch (mp->b_datap->db_type) {
		case M_DATA:
			if ((err = m2tp_r_data(q, mp)))
				break;
			goto rsrv_continue;
		case M_PROTO:
			if ((err = m2tp_r_proto(q, mp)))
				break;
			goto rsrv_continue;
		case M_PCPROTO:
			if ((err = m2tp_r_pcproto(q, mp)))
				break;
			goto rsrv_continue;
		case M_CTL:
			if ((err = m2tp_r_ctl(q, mp)))
				break;
			goto rsrv_continue;
		}
		switch (err) {
		case -EAGAIN:
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				return (0);
			}
		case -EOPNOTSUPP:
			if (q->q_next) {
				putnext(q, mp);
				goto rsrv_continue;
			}
			break;
		}
		freemsg(mp);
	      rsrv_continue:
		/*
		 *  NOTE:-  I have removed and processed a message from the
		 *  receive queue, so I should check for receive congestion
		 *  abatement.  If receive congestion has abated below the
		 *  accept threshold, I should signal MTP that there is no
		 *  longer any receive congestion.
		 */
		continue;
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  WRITE QUEUE PUT and SRV routines
 *
 *  -------------------------------------------------------------------------
 *
 *  SCTP WPUT - Message from above.
 *
 *  If the message is priority message we attempt to process it immediately.
 *  If the message is non-priority message, but there are no messages on the
 *  queue yet, we attempt to process it immediately.  If the message is not
 *  supported, we pass it down-queue if possible.  If the message cannot be
 *  processed immediately, we place it on the queue.
 */
static int
m2tp_wput(queue_t *q, mblk_t *mp)
{
	mblk_t *mp;
	int err = -EOPNOTSUPP;
	if (q->q_count && mp->b_datap->db_type < QPCTL) {
		putq(q, mp);
		/*
		 *  NOTE:- after placing messages on the queue here, I should
		 *  check for transmit congestion.  I should check if placing
		 *  the message on the queue crosses a band threshold for
		 *  congestion onset and abatement.  When crossing congestion
		 *  thresholds, I should notify MTP3.
		 */
		return (0);
	}
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if ((err = m2tp_w_data(q, mp)))
			break;
		return (0);
	case M_PROTO:
		if ((err = m2tp_w_proto(q, mp)))
			break;
		return (0);
	case M_PCPROTO:
		if ((err = m2tp_w_pcproto(q, mp)))
			break;
		return (0);
	case M_CTL:
		if ((err = m2tp_w_ctl(q, mp)))
			break;
		return (0);
	case M_IOCTL:
		if ((err = m2tp_w_ioctl(q, mp)))
			break;
		return (0);
	case M_FLUSH:
		m2tp_w_flush(q, mp);
		return (0);
	}
	switch (err) {
	case -EAGAIN:
		if (mp->b_datap->db_type < QPCTL) {
			putq(q, mp);
			return (0);
		}
	case -EOPNOTSUPP:
		if (q->q_next) {
			putnext(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (err);
}

/*
 *  M2TP WSRV = Queued message from above.
 *
 *  If the message is a priority message we attempt to process it immediately
 *  and without flow control.  If the message is a non-priority message and
 *  the next queue is flow controlled, we put the message back on the queue
 *  and wait.  If we cannot process a priority message immediately we cannot
 *  place it back on the queue, we discard it.  We requeue non-priority
 *  messages which cannot be processed immediately.  Unrecognized messages are
 *  passed down-queue.
 */
static int
m2tp_wsrv(queue_t *q)
{
	int err = -EOPNOTSUPP;
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (q->q_next && mp->b_datap->db_type < QPCTL && !canputnext(q)) {
			putbq(q, mp);
			return (0);
		}
		switch (mp->b_datap->db_type) {
		case M_DATA:
			if ((err = m2tp_w_data(q, mp)))
				break;
			goto wsrv_continue;
		case M_PROTO:
			if ((err = m2tp_w_proto(q, mp)))
				break;
			goto wsrv_continue;
		case M_PCPROTO:
			if ((err = m2tp_w_pcproto(q, mp)))
				break;
			goto wsrv_continue;
		case M_CTL:
			if ((err = m2tp_w_ctl(q, mp)))
				break;
			goto wsrv_continue;
		}
		switch (err) {
		case -EAGAIN:
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				return (0);
			}
		case -EOPNOTSUPP:
			if (q->q_next) {
				putnext(q, mp);
				goto wsrv_continue;
			}
		}
		freemsg(mp);
	      wsrv_continue:
		/*
		 *  NOTE:-   I have removed a message from the queue, so I
		 *  should check for band congestion abatement for the data
		 *  band to see if transmit congestion has abated.  If it has,
		 *  I should notify MTP3 of transmit abatement.
		 */
		continue;
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/*
 *  Private structure allocation and deallocation.
 *
 *  We use Linux hardware aligned cache here for speedy access to information
 *  contained in the private data structure.
 */
kmem_cache_t *m2tp_cachep = NULL;

static m2tp_t *
m2tp_alloc_priv(queue_t *q)
{
	m2tp_t *m2tp;

	if ((m2tp = kmem_cache_alloc(m2tp_cachep))) {
		bzero(m2tp, sizeof(*m2tp));
		RD(q)->q_ptr = WR(q)->q_tpr = m2tp;
		m2tp->rq = RD(q);
		m2tp->wq = WR(q);
	}
	return (m2tp);
}

static void
m2tp_free_priv(queue_t *q)
{
	m2tp_t *m2tp = (m2tp_t *) q->q_ptr;
	kmem_cache_free(m2tp_cachep, m2tp);
	return;
}

static void
m2tp_init_priv(void)
{
	if (!(m2tp_cachep = kmem_find_general_cachep(sizeof(m2tp_t)))) {
		/*
		   allocate a new cachep 
		 */
	}
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN - Each open
 *
 *  -------------------------------------------------------------------------
 */
static int
m2tp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;		/* for now */
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		major_t cmajor = getmajor(*devp);
		minor_t cminor = getminor(*devp);
		struct m2tp *m2tp;
		/* test for multiple push */
		for (m2tp = m2tp_list; m2tp; m2tp = m2tp->next) {
			if (m2tp->u.dev.cmajor == cmajor && m2tp->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		/* 
		 *  FIXME: check to make sure that the module we are being
		 *  pushed over is compatible (i.e. it is the right kind of
		 *  transport module.
		 */
		if (!(m2tp_alloc_priv(q))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		qprocson(q);
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CLOSE - Last close
 *
 *  -------------------------------------------------------------------------
 */
static int
m2tp_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;

	m2tp_free_priv(q);
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
MODULE_PARM_DESC(modid, "Module ID for the M2PA-SL module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &m2tpinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
sl_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&sl_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
sl_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&sl_fmod)) < 0)
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
sl_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&m2tpinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&m2tpinfo);
		return (err);
	}
	return (0);
}

STATIC int
sl_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&m2tpinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
m2tpinit(void)
{
	int err;
#ifdef MODULE
	cmn_err(CE_NOTE, M2TP_BANNER);	/* banner message */
#else
	cmn_err(CE_NOTE, M2TP_SPLASH);	/* console splash */
#endif
	if ((err = sl_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = sl_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		sl_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
m2tpterminate(void)
{
	int err;
	if ((err = sl_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sl_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(m2tpinit);
module_exit(m2tpterminate);

#endif				/* LINUX */
