/*****************************************************************************

 @(#) $RCSfile: mtp_npi.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/04 13:00:15 $

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

 Last Modified $Date: 2006/03/04 13:00:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp_npi.c,v $
 Revision 0.9.2.13  2006/03/04 13:00:15  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp_npi.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/04 13:00:15 $"

static char const ident[] =
    "$RCSfile: mtp_npi.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/04 13:00:15 $";

/*
 *  This is a MTP NPI module which can be pushed over an MTPI (Message
 *  Transfer Part Interface) stream to effect an NPI interface to the MTP.
 *  This module is intended to be used by application programs or by upper
 *  modules that expect an NPI connectionless service provider.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <sys/npi.h>
#include <sys/npi_ss7.h>
#include <sys/npi_mtp.h>

#define MTP_NPI_DESCRIP		"SS7 Message Transfer Part (MTP) NPI STREAMS MODULE."
#define MTP_NPI_REVISION	"LfS $RCSfile: mtp_npi.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/04 13:00:15 $"
#define MTP_NPI_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define MTP_NPI_DEVICE		"Part of the OpenSS7 Stack for LiS STREAMS."
#define MTP_NPI_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MTP_NPI_LICENSE		"GPL"
#define MTP_NPI_BANNER		MTP_NPI_DESCRIP		"\n" \
				MTP_NPI_REVISION	"\n" \
				MTP_NPI_COPYRIGHT	"\n" \
				MTP_NPI_DEVICE		"\n" \
				MTP_NPI_CONTACT		"\n"
#define MTP_NPI_SPLASH		MTP_NPI_DESCRIP		"\n" \
				MTP_NPI_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(MTP_NPI_CONTACT);
MODULE_DESCRIPTION(MTP_NPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(MTP_NPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MTP_NPI_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mtp_npi");
#endif
#endif				/* LINUX */

#ifdef LFS
#define MTP_NPI_MOD_ID		CONFIG_STREAMS_MTP_NPI_MODID
#define MTP_NPI_MOD_NAME	CONFIG_STREAMS_MTP_NPI_NAME
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		MTP_NPI_MOD_ID
#define MOD_NAME	MTP_NPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MTP_NPI_BANNER
#else				/* MODULE */
#define MOD_BANNER	MTP_NPI_SPLASH
#endif				/* MODULE */

STATIC struct module_info mtp_minfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME,		/* Module name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC int mtp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int mtp_close(queue_t *, int, cred_t *);

STATIC struct qinit mtp_rinit = {
	qi_putp:ss7_oput,		/* Read put (msg from below) */
	qi_qopen:mtp_open,		/* Each open */
	qi_qclose:mtp_close,		/* Last close */
	qi_minfo:&mtp_minfo,		/* Information */
};

STATIC struct qinit mtp_winit = {
	qi_putp:ss7_iput,		/* Write put (msg from above) */
	qi_minfo:&mtp_minfo,		/* Information */
};

STATIC struct streamtab mtp_npiinfo = {
	st_rdinit:&mtp_rinit,		/* Upper read queue */
	st_wrinit:&mtp_winit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  MTP-NPI Private Structure
 *
 *  =========================================================================
 */

typedef struct mtp {
	STR_DECLARATION (struct mtp);	/* stream declaration */
	struct mtp_addr src;
	struct mtp_addr dst;
	struct {
		ulong pvar;
		ulong popt;
		ulong sls;
		ulong mp;
		ulong debug;
		ulong sls_mask;
	} options;
	N_info_ack_t prot;
} mtp_t;

#define MTP_PRIV(__q) ((struct mtp *)(__q)->q_ptr)

struct mtp *mtp_opens = NULL;

STATIC struct mtp *mtp_alloc_priv(queue_t *q, struct mtp **, dev_t *, cred_t *);
STATIC struct mtp *mtp_get(struct mtp *);
STATIC void mtp_put(struct mtp *);
STATIC void mtp_free_priv(queue_t *q);

/*
 *  -------------------------------------------------------------------------
 *
 *  QOS Parameters
 *
 *  -------------------------------------------------------------------------
 */
typedef struct mtp_opts {
	uint flags;			/* success flags */
	ulong *sls;
	ulong *mp;
	ulong *debug;
} mtp_opts_t;

struct {
	ulong sls;
	ulong mp;
	ulong debug;
} opt_defaults = {
0, 0, 0};

STATIC int
mtp_parse_qos(struct mtp *mtp, struct mtp_opts *ops, unsigned char *op, size_t len)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  STATE Functions
 *
 *  -------------------------------------------------------------------------
 */
#ifdef _DEBUG
STATIC INLINE const char *
mtp_state(ulong state)
{
	switch (state) {
	case NS_UNBND:
		return ("NS_UNBND");
	case NS_WACK_BREQ:
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:
		return ("NS_WACK_UREQ");
	case NS_IDLE:
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:
		return ("NS_WACK_DREQ11");
	case NS_NOSTATES:
		return ("NS_NOSTATES");
	default:
		return ("????");
	}
}
#endif

STATIC INLINE ulong
mtp_get_state(struct mtp *mtp)
{
	return mtp->i_state;
}
STATIC INLINE void
mtp_set_state(struct mtp *mtp, ulong newstate)
{
	ulong oldstate = mtp->i_state;
	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", MOD_NAME, mtp, mtp_state(newstate), mtp_state(oldstate)));
	mtp->i_state = mtp->prot.CURRENT_state = newstate;
}

STATIC INLINE int
mtp_bind(struct mtp *mtp, struct mtp_addr *src)
{
	if (src)
		mtp->src = *src;
	return (0);
}
STATIC INLINE int
mtp_connect(struct mtp *mtp, struct mtp_addr *dst)
{
	return (0);
}
STATIC INLINE int
mtp_unbind(struct mtp *mtp)
{
	return (0);
}
STATIC INLINE int
mtp_disconnect(struct mtp *mtp)
{
	return (0);
}

/*
 *  =========================================================================
 *
 *  PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  Primitives sent upstream
 *  -------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC INLINE int
m_error(queue_t *q, struct mtp *mtp, int etype)
{
	int err;
	mblk_t *mp;
	if (!(mp = ss7_allocb(q, 2, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_ERROR;
	*(mp->b_wptr)++ = etype < 0 ? -etype : etype;
	*(mp->b_wptr)++ = etype < 0 ? -etype : etype;
	mtp_set_state(mtp, NS_NOSTATES);
	printd(("%s: %p: <- M_ERROR\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}
STATIC INLINE int n_error_ack(queue_t *q, struct mtp *mtp, ulong prim, long error);

/*
 *  N_CONN_IND          11 - Incoming connection indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_conn_ind(queue_t *q, struct mtp *mtp, ulong seq, ulong flags, struct mtp_addr *src,
	   struct mtp_addr *dst, N_qos_sel_conn_mtp_t *qos)
{
	int err;
	mblk_t *mp;
	N_conn_ind_t *p;
	const size_t src_len = src ? sizeof(*src) : 0;
	const size_t dst_len = dst ? sizeof(*dst) : 0;
	const size_t qos_len = qos ? sizeof(*qos) : 0;
	const size_t msg_len = sizeof(*p) + src_len + dst_len + qos_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_CONN_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + src_len + dst_len : 0;
	p->SEQ_number = seq;
	p->CONN_flags = flags;
	if (src_len) {
		bcopy(mp->b_wptr, src, src_len);
		mp->b_wptr += src_len;
	}
	if (dst_len) {
		bcopy(mp->b_wptr, dst, dst_len);
		mp->b_wptr += dst_len;
	}
	if (qos_len) {
		bcopy(mp->b_wptr, qos, qos_len);
		mp->b_wptr += qos_len;
	}
	printd(("%s: %p: <- N_CONN_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_CONN_CON          12 - Connection established
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_conn_con(queue_t *q, struct mtp *mtp, ulong flags, struct mtp_addr *res,
	   N_qos_sel_conn_mtp_t *qos)
{
	int err;
	mblk_t *mp;
	N_conn_con_t *p;
	const size_t res_len = res ? sizeof(*res) : 0;
	const size_t qos_len = qos ? sizeof(*qos) : 0;
	const size_t msg_len = sizeof(*p) + res_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->CONN_flags = flags;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
	if (res_len) {
		bcopy(mp->b_wptr, res, res_len);
		mp->b_wptr += res_len;
	}
	if (qos_len) {
		bcopy(mp->b_wptr, qos, qos_len);
		mp->b_wptr += qos_len;
	}
	if (mtp_get_state(mtp) != NS_WCON_CREQ)
		swerr();
	mtp_set_state(mtp, NS_DATA_XFER);
	printd(("%s: %p: <- N_CONN_CON\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_DISCON_IND        13 - NC disconnected
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_discon_ind(queue_t *q, struct mtp *mtp, ulong orig, ulong reason, ulong seq,
	     struct mtp_addr *res, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_discon_ind_t *p;
	const size_t res_len = res ? sizeof(*res) : 0;
	const size_t msg_len = sizeof(*p) + res_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = orig;
	p->DISCON_reason = reason;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	if (res_len) {
		bcopy(mp->b_wptr, res, res_len);
		mp->b_wptr += res_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- N_DISCON_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_DATA_IND          14 - Incoming connection-mode data indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_data_ind(queue_t *q, struct mtp *mtp, ulong flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_data_ind_t *p;
	const size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DATA_IND;
	p->DATA_xfer_flags = flags;
	mp->b_cont = dp;
	printd(("%s: %p: <- N_DATA_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_EXDATA_IND        15 - Incoming expedited data indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_exdata_ind(queue_t *q, struct mtp *mtp, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_exdata_ind_t *p;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_cont = dp;
	printd(("%s: %p: <- N_EXDATA_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_INFO_ACK          16 - Information Acknowledgement
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_info_ack(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	N_info_ack_t *p;
	size_t src_len, dst_len, msg_len;
	N_qos_sel_info_mtp_t *qos;
	N_qos_range_info_mtp_t *qor;
	size_t qos_len = sizeof(*qos);
	size_t qor_len = sizeof(*qor);
	switch (mtp_get_state(mtp)) {
	default:
	case NS_UNBND:
		src_len = 0;
		dst_len = 0;
		break;
	case NS_IDLE:
		src_len = sizeof(mtp->src);
		dst_len = 0;
		break;
	case NS_DATA_XFER:
		src_len = sizeof(mtp->src);
		dst_len = sizeof(mtp->dst);
		break;
	}
	msg_len = sizeof(*p) + src_len + dst_len + qos_len + qor_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	*p = mtp->prot;
	p->ADDR_size = sizeof(struct mtp_addr);
	p->ADDR_length = src_len + dst_len;
	p->ADDR_offset = src_len + dst_len ? sizeof(*p) : 0;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + src_len + dst_len : 0;
	p->QOS_range_length = qor_len;
	p->QOS_range_offset = qor_len ? sizeof(*p) + src_len + dst_len + qos_len : 0;
	p->PROTOID_length = 0;
	p->PROTOID_offset = 0;
	if (src_len) {
		bcopy(&mtp->src, mp->b_wptr, sizeof(mtp->src));
		mp->b_wptr += sizeof(mtp->src);
	}
	if (dst_len) {
		bcopy(&mtp->dst, mp->b_wptr, sizeof(mtp->dst));
		mp->b_wptr += sizeof(mtp->dst);
	}
	if (qos_len) {
		qos = (typeof(qos)) mp->b_wptr++;
		qos->n_qos_type = N_QOS_SEL_INFO_MTP;
		qos->pvar = mtp->options.pvar;
		qos->popt = mtp->options.popt;
	}
	if (qor_len) {
		qor = (typeof(qor)) mp->b_wptr++;
		qor->n_qos_type = N_QOS_RANGE_INFO_MTP;
		qor->sls_range = mtp->options.sls_mask;
		qor->mp_range = (mtp->options.popt & SS7_POPT_MPLEV) ? 3 : 0;
	}
	printd(("%s: %p: <- N_INFO_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_BIND_ACK          17 - NS User bound to network address
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_bind_ack(queue_t *q, struct mtp *mtp, struct mtp_addr *add)
{
	int err;
	mblk_t *mp;
	N_bind_ack_t *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if (!(mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = 0;
	p->TOKEN_value = (ulong) mtp->oq;
	p->PROTOID_length = 0;
	p->PROTOID_offset = 0;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	if ((err = mtp_bind(mtp, add)))
		goto free_error;
	mtp_set_state(mtp, NS_IDLE);
	printd(("%s: %p: <- N_BIND_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
      free_error:
	freemsg(mp);
	return n_error_ack(q, mtp, N_BIND_REQ, err);
}

/*
 *  N_ERROR_ACK         18 - Error Acknowledgement
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_error_ack(queue_t *q, struct mtp *mtp, ulong prim, long etype)
{
	int err = etype;
	mblk_t *mp;
	N_error_ack_t *p;
	size_t msg_len = sizeof(*p);
	switch (etype) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		goto error;
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
		swerr();
		goto error;
	}
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = prim;
	p->NPI_error = etype < 0 ? NSYSERR : etype;
	p->UNIX_error = etype < 0 ? -etype : 0;
	printd(("%s: %p: <- N_ERROR_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	/* 
	   Retruning -EPROTO here will make sure that the old state is restored correctly. If we
	   return QR_DONE, then the state will never be restored. */
	if (etype >= 0)
		return (-EPROTO);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_OK_ACK            19 - Success Acknowledgement
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_ok_ack(queue_t *q, struct mtp *mtp, ulong prim)
{
	int err;
	mblk_t *mp;
	N_ok_ack_t *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = prim;
	switch (mtp_get_state(mtp)) {
	case NS_WACK_UREQ:
		if ((err = mtp_unbind(mtp)))
			return (err);
		mtp_set_state(mtp, NS_UNBND);
		break;
	case NS_WACK_OPTREQ:
		mtp_set_state(mtp, NS_IDLE);
		break;
	case NS_WACK_RRES:
		mtp_set_state(mtp, NS_DATA_XFER);
		break;
	case NS_WACK_CRES:
		mtp_set_state(mtp, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		if ((err = mtp_disconnect(mtp)))
			return (err);
		mtp_set_state(mtp, NS_IDLE);
		break;
	default:
		/* 
		   Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a N_OPTMGMT_REQ in other than TS_IDLE state. */
		seldom();
		break;
	}
	printd(("%s: %p: <- N_OK_ACK\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_UNITDATA_IND      20 - Connection-less data receive indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_unitdata_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *src, struct mtp_addr *dst, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_unitdata_ind_t *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t msg_len = sizeof(*p) + src_len + dst_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- N_UNITDATA_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_UDERROR_IND       21 - UNITDATA Error Indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_uderror_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *dst, mblk_t *dp, ulong etype)
{
	int err;
	mblk_t *mp;
	N_uderror_ind_t *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t msg_len = sizeof(*p) + dst_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_UDERROR_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->ERROR_type = etype;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- N_UDERROR_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_DATACK_IND        24 - Data acknowledgement indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_datack_ind(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	N_datack_ind_t *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DATACK_IND;
	printd(("%s: %p: <- N_DATACK_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_RESET_IND         26 - Incoming NC reset request indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_reset_ind(queue_t *q, struct mtp *mtp, ulong orig, ulong reason)
{
	int err;
	mblk_t *mp;
	N_reset_ind_t *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = orig;
	p->RESET_reason = reason;
	printd(("%s: %p: <- N_RESET_IND\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  N_RESET_CON         28 - Reset processing complete
 *  -----------------------------------------------------------
 */
STATIC INLINE int
n_reset_con(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	N_reset_con_t *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_RESET_CON;
	printd(("%s: %p: <- N_RESET_CON\n", MOD_NAME, mtp));
	putnext(mtp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream.
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_BIND_REQ        1 - Bind to an MTP-SAP
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_bind_req(queue_t *q, struct mtp *mtp, struct mtp_addr *add, ulong flags)
{
	int err;
	mblk_t *mp;
	struct MTP_bind_req *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_BIND_REQ;
	p->mtp_addr_length = add_len;
	p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
	p->mtp_bind_flags = flags;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_UNBIND_REQ      2 - Unbind from an MTP-SAP
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_unbind_req(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct MTP_unbind_req *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_UNBIND_REQ;
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_CONN_REQ        3 - Connect to a remote MTP-SAP
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_conn_req(queue_t *q, struct mtp *mtp, struct mtp_addr *add, ulong flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct MTP_conn_req *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_CONN_REQ;
	p->mtp_addr_length = add_len;
	p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
	p->mtp_conn_flags = flags;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	mp->b_cont = dp;
	putnext(mtp->iq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_DISCON_REQ      4 - Disconnect from a remote MTP-SAP
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_discon_req(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct MTP_discon_req *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_DISCON_REQ;
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_ADDR_REQ        5 - Address service
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_addr_req(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct MTP_addr_req *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_ADDR_REQ;
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_INFO_REQ        6 - Information service
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_info_req(queue_t *q, struct mtp *mtp)
{
	int err;
	mblk_t *mp;
	struct MTP_info_req *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_INFO_REQ;
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

#if 0
/*
 *  MTP_OPTMGMT_REQ     7 - Options management service
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_optmgmt_req(queue_t *q, struct mtp *mtp, struct mtp_opts *opt, ulong flags)
{
	int err;
	mblk_t *mp;
	struct MTP_optmgmt_req *p;
	size_t opt_len = opt ? mtp_opts_size(opt) : 0;
	size_t msg_len = sizeof(*p) + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_OPTMGMT_REQ;
	p->mtp_opt_length = opt_len;
	p->mtp_opt_offset = opt_len ? sizeof(*p) : 0;
	p->mtp_mgmt_flags = flags;
	if (opt_len) {
		mtp_build_opts(opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	putnext(mtp->iq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}
#endif

/*
 *  MTP_TRANSFER_REQ    8 - MTP data transfer request
 *  -----------------------------------------------------------
 */
STATIC INLINE int
mtp_transfer_req(queue_t *q, struct mtp *mtp, struct mtp_addr *dst, ulong pri, ulong sls,
		 mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct MTP_transfer_req *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t msg_len = sizeof(*p) + dst_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_TRANSFER_REQ;
	p->mtp_dest_length = dst_len;
	p->mtp_dest_offset = dst_len ? sizeof(*p) : 0;
	p->mtp_mp = pri;
	p->mtp_sls = sls;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	mp->b_cont = dp;
	putnext(mtp->iq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", MOD_NAME, mtp));
	goto error;
      error:
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from above.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  M_DATA
 *  -------------------------------------------------------------------
 */
STATIC int
n_data(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const size_t dlen = msgdsize(mp);
	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (dlen == 0 || dlen > mtp->prot.NSDU_size || dlen > mtp->prot.NIDU_size)
		goto baddata;
	switch (mtp_get_state(mtp)) {
	case NS_DATA_XFER:
		return mtp_transfer_req(q, mtp, &mtp->dst, mtp->options.mp, mtp->options.sls, mp);
	case NS_IDLE:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	/* 
	   If we are in the idle state this is just spurious data, ignore it */
	rare();
	return (QR_DONE);
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MOD_NAME, mtp));
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported\n", MOD_NAME, mtp));
	goto error;
      baddata:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: bad data\n", MOD_NAME, mtp));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  N_CONN_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_conn_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst;
	// unsigned char *qos;
	struct mtp_opts opts;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badprim;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (!p->DEST_length)
		goto noaddr;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	if (dst->family != AF_MTP)
		goto badaddr;
	if (dst->si == 0 && mtp->src.si == 0)
		goto noaddr;
	if (dst->si < 3 && mtp->src.si != 0)
		goto badaddr;
	if (mtp_parse_qos(mtp, &opts, mp->b_rptr + p->QOS_offset, p->QOS_length))
		goto badqostype;
	/* 
	   TODO: set options first */
	mtp->dst = *dst;
	mtp_set_state(mtp, NS_WCON_CREQ);
	return n_conn_con(q, mtp, 0, &mtp->dst, NULL);
      badqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: ERROR: bad qos type\n", MOD_NAME, mtp));
	goto error;
#if 0
      badqosparam:
	err = NBADQOSPARAM;
	ptrace(("%s: %p: ERROR: bad qos parameter\n", MOD_NAME, mtp));
	goto error;
#endif
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: bad destination address\n", MOD_NAME, mtp));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: couldn't allocate destination address\n", MOD_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MOD_NAME, mtp));
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for N_CLNS\n", MOD_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MOD_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, N_CONN_REQ, err);
}

/*
 *  N_CONN_RES:
 *  -------------------------------------------------------------------
 */
STATIC int
n_conn_res(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mtp_get_state(mtp) != NS_WRES_CIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* 
	   We never give an N_CONN_IND, so there is no reason for a N_CONN_RES.  We probably could
	   do this * (issue an N_CONN_IND on a listening stream when there is no other MTP user for 
	   the SI value and * send a UPU on an N_DISCON_REQ or just redirect all traffic for that
	   user on a N_CONN_RES) but * that is for later. */
	goto eopnotsupp;
      eopnotsupp:
	err = -EOPNOTSUPP;
	ptrace(("%s: %p: ERROR: operation not supported\n", MOD_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MOD_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MOD_NAME, mtp));
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported\n", MOD_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, N_CONN_RES, err);
}

/*
 *  N_DISCON_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_discon_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* 
	   Currently there are only three states we can disconnect from.  The first does not
	   happen. Only the second one is normal.  The third should occur during simulteneous
	   diconnect only. */
	switch (mtp_get_state(mtp)) {
	case NS_WCON_CREQ:
		mtp_set_state(mtp, NS_WACK_DREQ6);
		break;
	case NS_DATA_XFER:
		mtp_set_state(mtp, NS_WACK_DREQ9);
		break;
	case NS_IDLE:
		rare();
		break;
	default:
		goto outstate;
	}
	return n_ok_ack(q, mtp, N_DISCON_REQ);
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MOD_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MOD_NAME, mtp));
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported\n", MOD_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, N_DISCON_REQ, err);
}

/*
 *  N_DATA_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_data_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	const N_data_req_t *p = (typeof(p)) mp->b_rptr;
	const size_t dlen = msgdsize(mp);
	if (mtp->prot.SERV_type == N_CLNS)
		goto notsupport;
	if (mtp_get_state(mtp) == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << mtp_get_state(mtp)) & ~(NSF_DATA_XFER | NSF_WRES_RIND | NSF_WCON_RREQ))
		goto outstate;
	if (p->DATA_xfer_flags)
		/* 
		   N_MORE_DATA_FLAG and N_RC_FLAG not supported yet.  We could do N_MORE_DATA_FLAG
		   pretty easily by accumulating the packet until the last data request is
		   received, but this would be rather pointless for small MTP packet sizes.
		   N_RC_FLAG cannot be supported until the DLPI link driver is done and zero-loss
		   operation is completed. */
		goto notsupport;
	if (dlen == 0 || dlen > mtp->prot.NSDU_size || dlen > mtp->prot.NIDU_size)
		goto baddata;
	return mtp_transfer_req(q, mtp, &mtp->dst, mtp->options.mp, mtp->options.sls, mp->b_cont);
      baddata:
	ptrace(("%s: %p: ERROR: bad data\n", MOD_NAME, mtp));
	goto error;
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MOD_NAME, mtp));
	goto error;
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MOD_NAME, mtp));
	goto error;
      discard:
	ptrace(("%s: %p: ERROR: ignore in idle state\n", MOD_NAME, mtp));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: %p: ERROR: primitive not supported\n", MOD_NAME, mtp));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  N_EXDATA_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_exdata_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	(void) mp;
	return m_error(q, mtp, -EPROTO);
}

/*
 *  N_INFO_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_info_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	(void) mp;
	return n_info_ack(q, mtp);
}

/*
 *  N_BIND_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_bind_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr src;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length)
		goto badprim;
	if (mtp_get_state(mtp) != NS_UNBND)
		goto outstate;
	if (p->PROTOID_length)
		goto badaddr;
	if (!p->ADDR_length)
		goto noaddr;
	if (p->ADDR_length < sizeof(src))
		goto badaddr;
	bcopy(mp->b_rptr + p->ADDR_offset, &src, sizeof(src));
	if (src.family != AF_MTP)
		goto badaddr;
	if (!src.si || !src.pc)
		goto noaddr;
	if (src.si < 3 || mtp->cred.cr_uid != 0)
		goto access;
	mtp_set_state(mtp, NS_WACK_BREQ);
	return mtp_bind_req(q, mtp, &src, 0);
      access:
	err = NACCESS;
	ptrace(("%s: %p: ERROR: no priviledge for requested address\n", MOD_NAME, mtp));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: could not allocate address\n", MOD_NAME, mtp));
	goto error;
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: requested address invalid\n", MOD_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MOD_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MOD_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, N_BIND_REQ, err);
}

/*
 *  N_UNBIND_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_unbind_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	mtp_set_state(mtp, NS_WACK_UREQ);
	return mtp_unbind_req(q, mtp);
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MOD_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MOD_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, N_UNBIND_REQ, err);
}

/*
 *  N_UNITDATA_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_unitdata_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	const size_t dlen = msgdsize(mp);
	struct mtp_addr dst;
	if (mtp->prot.SERV_type != N_CLNS)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	if (dlen == 0)
		goto baddata;
	if (dlen > mtp->prot.NSDU_size || dlen > mtp->prot.NIDU_size)
		goto baddata;
	if (!p->DEST_length)
		goto noaddr;
	if (p->DEST_length < sizeof(dst))
		goto badaddr;
	bcopy(mp->b_rptr + p->DEST_length, &dst, sizeof(dst));
	if (dst.family != AF_MTP)
		goto badaddr;
	if (!dst.si || !dst.pc)
		goto badaddr;
	if (dst.si < 3 && mtp->cred.cr_uid != 0)
		goto access;
	if (dst.si != mtp->src.si)
		goto badaddr;
	fixme(("Handle options correctly\n"));
	return mtp_transfer_req(q, mtp, &dst, mtp->options.mp, mtp->options.sls, mp->b_cont);
      access:
	err = NACCESS;
	ptrace(("%s: %p: ERROR: no priviledge for requested address\n", MOD_NAME, mtp));
	goto error;
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: requested address invalid\n", MOD_NAME, mtp));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: could not allocate address\n", MOD_NAME, mtp));
	goto error;
      baddata:
	err = NBADDATA;
	ptrace(("%s: %p: ERROR: invalid amount of data\n", MOD_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", MOD_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MOD_NAME, mtp));
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive type not supported\n", MOD_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, N_UNITDATA_REQ, err);
}

/*
 *  N_OPTMGMT_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_optmgmt_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	union N_qos_mtp *qos;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badprim;
#ifdef NS_WACK_OPTREQ
	if (mtp_get_state(mtp) == NS_IDLE)
		mtp_set_state(mtp, NS_WACK_OPTREQ);
#endif
	if (p->OPTMGMT_flags)
		/* 
		   Can't support DEFAULT_RC_SEL yet */
		goto badflags;
	qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
	if (p->QOS_length < sizeof(qos->n_qos_data))
		goto badqostype;
	if (qos->n_qos_type != N_QOS_SEL_DATA_MTP)
		goto badqostype;
	if (qos->n_qos_data.sls > 255 && qos->n_qos_data.sls != -1UL)
		goto badqosparam;
	if (qos->n_qos_data.mp > 3 && qos->n_qos_data.mp != -1UL)
		goto badqosparam;
	mtp->options.sls = qos->n_qos_data.sls;
	mtp->options.mp = qos->n_qos_data.mp;
	return n_ok_ack(q, mtp, N_OPTMGMT_REQ);
      badqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: ERROR: invalid qos type\nn", MOD_NAME, mtp));
	goto error;
      badqosparam:
	err = NBADQOSPARAM;
	ptrace(("%s: %p: ERROR: invalid qos parameter\nn", MOD_NAME, mtp));
	goto error;
      badflags:
	err = NBADFLAG;
	ptrace(("%s: %p: ERROR: invalid flag\nn", MOD_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", MOD_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, N_OPTMGMT_REQ, err);
}

/*
 *  N_DATACK_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_datack_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/* 
	   We don't support DATACK yet.  With zero loss operation we will. */
	rare();
	return (QR_DONE);
}

/*
 *  N_RESET_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_reset_req(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	(void) q;
	(void) mp;
	todo(("Accept resets with reason from the user\n"));
	rare();
	return (QR_DONE);
}

/*
 *  N_RESET_RES:
 *  -------------------------------------------------------------------
 */
STATIC int
n_reset_res(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/* 
	   ignore.  if the user wishes to respond to our reset indications that's fine. */
	rare();
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from below.
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC INLINE int
mtp_data(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	if (mtp->prot.SERV_type == N_CONS)
		return (QR_PASSALONG);
	swerr();
	return (-EFAULT);
}

/*
 *  MTP_OK_ACK:
 *  -----------------------------------
 *  Simply translate the MTP_OK_ACK into a N_OK_ACK.
 */
STATIC INLINE int
mtp_ok_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	ulong prim;
	struct MTP_ok_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (p->mtp_correct_prim) {
	case MTP_BIND_REQ:
		swerr();
		prim = N_BIND_REQ;
		break;
	case MTP_UNBIND_REQ:
		prim = N_UNBIND_REQ;
		break;
	case MTP_CONN_REQ:
		prim = N_CONN_REQ;
		if ((err = n_ok_ack(q, mtp, prim)) < 0)
			return (err);
		return n_conn_con(q, mtp, 0, NULL, NULL);
	case MTP_DISCON_REQ:
		prim = N_DISCON_REQ;
		break;
	case MTP_ADDR_REQ:
		swerr();
		prim = N_INFO_REQ;
		break;
	case MTP_INFO_REQ:
		swerr();
		prim = N_INFO_REQ;
		break;
	case MTP_OPTMGMT_REQ:
		swerr();
		prim = N_OPTMGMT_REQ;
		break;
	case MTP_TRANSFER_REQ:
		swerr();
		prim = N_DATA_REQ;
		break;
	default:
		swerr();
		prim = 0;
		break;
	}
	return n_ok_ack(q, mtp, prim);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_ERROR_ACK:
 *  -----------------------------------
 *  Simply translate the MTP_ERROR_ACK into a N_ERROR_ACK.
 */
STATIC INLINE int
mtp_error_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	ulong prim;
	struct MTP_error_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (p->mtp_primitive) {
	case MTP_BIND_REQ:
		prim = N_BIND_REQ;
		break;
	case MTP_UNBIND_REQ:
		prim = N_UNBIND_REQ;
		break;
	case MTP_CONN_REQ:
		prim = N_CONN_REQ;
		break;
	case MTP_DISCON_REQ:
		prim = N_DISCON_REQ;
		break;
	case MTP_ADDR_REQ:
		swerr();
		prim = N_INFO_REQ;
		break;
	case MTP_INFO_REQ:
		prim = N_INFO_REQ;
		break;
	case MTP_OPTMGMT_REQ:
		prim = N_OPTMGMT_REQ;
		break;
	case MTP_TRANSFER_REQ:
		swerr();
		prim = N_DATA_REQ;
		break;
	default:
		swerr();
		prim = 0;
		break;
	}
	switch (p->mtp_mtpi_error) {
	case MSYSERR:
		err = -p->mtp_unix_error;
		break;
	case MACCESS:
		err = NACCESS;
		break;
	case MBADADDR:
		err = NBADADDR;
		break;
	case MNOADDR:
		err = NNOADDR;
		break;
	case MBADPRIM:
		err = -EINVAL;
		break;
	case MOUTSTATE:
		err = NOUTSTATE;
		break;
	case MNOTSUPP:
		err = NNOTSUPPORT;
		break;
	case MBADFLAG:
		err = NBADFLAG;
		break;
	case MBADOPT:
		err = NBADOPT;
		break;
	default:
		swerr();
		err = -EFAULT;
		break;
	}
	return n_error_ack(q, mtp, prim, err);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_BIND_ACK:
 *  -----------------------------------
 *  Translate the MTP_BIND_ACK into a N_BIND_ACK.
 */
STATIC INLINE int
mtp_bind_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_bind_ack *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *add = NULL;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto efault;
	if (p->mtp_addr_length == sizeof(*add))
		add = (typeof(add)) (mp->b_rptr + p->mtp_addr_offset);
	return n_bind_ack(q, mtp, add);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_ADDR_ACK:
 *  -----------------------------------
 *  Simply translate MTP_ADDR_ACK to N_ADDR_ACK.
 */
STATIC INLINE int
mtp_addr_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_addr_ack *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *loc = NULL, *rem = NULL;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_loc_offset + p->mtp_loc_length)
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_rem_offset + p->mtp_rem_length)
		goto efault;
	if (p->mtp_loc_length == sizeof(*loc))
		loc = (typeof(loc)) (mp->b_rptr + p->mtp_loc_offset);
	if (p->mtp_rem_length == sizeof(*rem))
		rem = (typeof(rem)) (mp->b_rptr + p->mtp_rem_offset);
	return (QR_DONE);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_INFO_ACK:
 *  -----------------------------------
 *  Simply translate MTP_INFO_ACK to N_INFO_ACK.
 */
STATIC INLINE int
mtp_info_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_info_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto efault;
	mtp->prot.NSDU_size = p->mtp_msu_size;
	mtp->prot.NIDU_size = p->mtp_msu_size;
	mtp->prot.ADDR_size = p->mtp_addr_size;
	switch (p->mtp_serv_type) {
	case M_COMS:
		mtp->prot.SERV_type = N_CONS;
		mtp->prot.CDATA_size = p->mtp_msu_size;
		mtp->prot.DDATA_size = p->mtp_msu_size;
		break;
	case M_CLMS:
		mtp->prot.SERV_type = N_CLNS;
		mtp->prot.CDATA_size = -2;
		mtp->prot.DDATA_size = -2;
		break;
	default:
		swerr();
		break;
	}
	return n_info_ack(q, mtp);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_OPTMGMT_ACK:
 *  -----------------------------------
 */
STATIC INLINE int
mtp_optmgmt_ack(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  MTP_TRANSFER_IND:
 *  -----------------------------------
 *  Translate MTP_TRANSFER_IND into N_OPTDATA_IND or N_UNITDATA_IND.
 */
STATIC INLINE int
mtp_transfer_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *src;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_srce_offset + p->mtp_srce_length)
		goto efault;
	if (p->mtp_srce_length != sizeof(*src))
		goto efault;
	src = (typeof(src)) (mp->b_rptr + p->mtp_srce_offset);
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		/* 
		   N_DATA_IND */
		if ((err = n_data_ind(q, mtp, 0, mp->b_cont)) < 0)
			goto error;
		return (QR_TRIMMED);
	case N_CLNS:
		/* 
		   N_UNITDATA_IND */
		if ((err = n_unitdata_ind(q, mtp, src, &mtp->src, mp->b_cont)) < 0)
			goto error;
		return (QR_TRIMMED);
	}
	swerr();
	return (-EFAULT);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  MTP_PAUSE_IND:
 *  -----------------------------------
 *  Translate MTP_PAUSE_IND into N_UDERROR_IND or N_DISCON_IND.
 */
STATIC INLINE int
mtp_pause_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto efault;
	if (p->mtp_addr_length != sizeof(*dst))
		goto efault;
	dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_offset);
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		/* 
		   N_DISCON_IND */
		return n_discon_ind(q, mtp, N_PROVIDER, N_MTP_DEST_PROHIBITED, 0, NULL, mp->b_cont);
	case N_CLNS:
	{
		struct mtp_addr *dst;
		dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_length);
		/* 
		   N_UDERROR_IND */
		return n_uderror_ind(q, mtp, dst, mp->b_cont, N_MTP_DEST_PROHIBITED);
	}
	}
	swerr();
	return (-EFAULT);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_RESUME_IND:
 *  -----------------------------------
 *  Translate MTP_RESUME_IND.
 */
STATIC INLINE int
mtp_resume_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	/* 
	   discard */
	return (QR_DONE);
}

/*
 *  MTP_STATUS_IND:
 *  -----------------------------------
 *  Translate MTP_STATUS_IND into N_UDERROR_IND or N_RESET_IND or
 *  N_DISCON_IND.
 */
STATIC INLINE int
mtp_status_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_status_ind *p = (typeof(p)) mp->b_rptr;
	ulong type;
	ulong status;
	ulong error;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	type = p->mtp_type;
	status = p->mtp_status;
	switch (type) {
	case MTP_STATUS_TYPE_UPU:
		switch (status) {
		case MTP_STATUS_UPU_UNKNOWN:
			error = N_MTP_USER_PART_UNKNOWN;
			break;
		case MTP_STATUS_UPU_UNEQUIPPED:
			error = N_MTP_USER_PART_UNEQUIPPED;
			break;
		case MTP_STATUS_UPU_INACCESSIBLE:
			error = N_MTP_USER_PART_UNAVAILABLE;
			break;
		default:
			swerr();
			return (-EFAULT);
		}
		break;
	case MTP_STATUS_TYPE_CONG:
		switch (status) {
		case MTP_STATUS_CONGESTION_LEVEL0:
			error = N_MTP_DEST_CONGESTED(0);
			break;
		case MTP_STATUS_CONGESTION_LEVEL1:
			error = N_MTP_DEST_CONGESTED(1);
			break;
		case MTP_STATUS_CONGESTION_LEVEL2:
			error = N_MTP_DEST_CONGESTED(2);
			break;
		case MTP_STATUS_CONGESTION_LEVEL3:
			error = N_MTP_DEST_CONGESTED(3);
			break;
		case MTP_STATUS_CONGESTION:
			error = N_MTP_DEST_CONGESTED(4);
			break;
		default:
			swerr();
			return (-EFAULT);
		}
		break;
	default:
		swerr();
		return (-EFAULT);
	}
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		switch (type) {
		case MTP_STATUS_TYPE_UPU:
			/* 
			   N_DISCON_IND */
			return n_discon_ind(q, mtp, N_USER, error, 0, NULL, mp->b_cont);
		case MTP_STATUS_TYPE_CONG:
			/* 
			   N_RESET_IND */
			return n_reset_ind(q, mtp, N_PROVIDER, error);
		}
		break;
	case N_CLNS:
	{
		/* 
		   N_UDERROR_IND */
		struct mtp_addr *dst;
		dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_length);
		return n_uderror_ind(q, mtp, dst, mp->b_cont, error);
	}
	}
	swerr();
	return (-EFAULT);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_RESTART_BEGINS_IND:
 *  -----------------------------------
 *  Translate MTP_RESTART_BEGINS_IND into N_UDERROR_IND or N_DISCON_IND.
 */
STATIC INLINE int
mtp_restart_begins_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct MTP_restart_begins_ind *p = (typeof(p)) mp->b_rptr;
	ulong error = N_MTP_RESTARTING;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (mtp->prot.SERV_type) {
	case N_CONS:
		/* 
		   N_DISCON_IND */
		return n_discon_ind(q, mtp, N_PROVIDER, error, 0, NULL, mp->b_cont);
	case N_CLNS:
	{
		/* 
		   N_UDERROR_IND */
		struct mtp_addr *dst = NULL;
		return n_uderror_ind(q, mtp, dst, mp->b_cont, error);
	}
	}
	swerr();
	return (-EFAULT);
      efault:
	pswerr(("%s: %p: SWERR: invalid primitive from below\n", MOD_NAME, mtp));
	return (-EFAULT);
}

/*
 *  MTP_RESTART_COMPLETE_IND:
 *  -----------------------------------
 *  Translate MTP_RESTART_COMPLETE_IND.
 */
STATIC INLINE int
mtp_restart_complete_ind(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	/* 
	   discard */
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
#if 0
/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;
	switch (type) {
	case __SID:
		switch (nr) {
		case _IOC_NR(I_STR):
		case _IOC_NR(I_LINK):
		case _IOC_NR(I_PLINK):
		case _IOC_NR(I_UNLINK):
		case _IOC_NR(I_PUNLINK):
			(void) lp;
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, nr));
			ret = -EINVAL;
			break;
		default:
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	case MTP_IOC_MAGIC:
		ret = (QR_PASSALONG);
		break;
	default:
		ret = (QR_PASSALONG);
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
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Primitives from NPI to MTP.
 *  -----------------------------------
 */
STATIC int
mtp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct mtp *mtp = MTP_PRIV(q);
	ulong oldstate = mtp_get_state(mtp);
	/* 
	   Fast Path */
	if ((prim = *((ulong *) mp->b_rptr)) == N_DATA_REQ) {
		printd(("%s: %p: -> N_DATA_REQ [%d]\n", MOD_NAME, mtp, msgdsize(mp->b_cont)));
		if ((rtn = n_data_req(q, mtp, mp)))
			mtp_set_state(mtp, oldstate);
		return (rtn);
	}
	switch (prim) {
	case N_CONN_REQ:
		printd(("%s: %p: -> N_CONN_REQ\n", MOD_NAME, mtp));
		rtn = n_conn_req(q, mtp, mp);
		break;
	case N_CONN_RES:
		printd(("%s: %p: -> N_CONN_RES\n", MOD_NAME, mtp));
		rtn = n_conn_res(q, mtp, mp);
		break;
	case N_DISCON_REQ:
		printd(("%s: %p: -> N_DISCON_REQ\n", MOD_NAME, mtp));
		rtn = n_discon_req(q, mtp, mp);
		break;
	case N_DATA_REQ:
		printd(("%s: %p: -> N_DATA_REQ\n", MOD_NAME, mtp));
		rtn = n_data_req(q, mtp, mp);
		break;
	case N_EXDATA_REQ:
		printd(("%s: %p: -> N_EXDATA_REQ\n", MOD_NAME, mtp));
		rtn = n_exdata_req(q, mtp, mp);
		break;
	case N_INFO_REQ:
		printd(("%s: %p: -> N_INFO_REQ\n", MOD_NAME, mtp));
		rtn = n_info_req(q, mtp, mp);
		break;
	case N_BIND_REQ:
		printd(("%s: %p: -> N_BIND_REQ\n", MOD_NAME, mtp));
		rtn = n_bind_req(q, mtp, mp);
		break;
	case N_UNBIND_REQ:
		printd(("%s: %p: -> N_UNBIND_REQ\n", MOD_NAME, mtp));
		rtn = n_unbind_req(q, mtp, mp);
		break;
	case N_UNITDATA_REQ:
		printd(("%s: %p: -> N_UNITDATA_REQ\n", MOD_NAME, mtp));
		rtn = n_unitdata_req(q, mtp, mp);
		break;
	case N_OPTMGMT_REQ:
		printd(("%s: %p: -> N_OPTMGMT_REQ\n", MOD_NAME, mtp));
		rtn = n_optmgmt_req(q, mtp, mp);
		break;
	case N_DATACK_REQ:
		printd(("%s: %p: -> N_DATACK_REQ\n", MOD_NAME, mtp));
		rtn = n_datack_req(q, mtp, mp);
		break;
	case N_RESET_REQ:
		printd(("%s: %p: -> N_RESET_REQ\n", MOD_NAME, mtp));
		rtn = n_reset_req(q, mtp, mp);
		break;
	case N_RESET_RES:
		printd(("%s: %p: -> N_RESET_RES\n", MOD_NAME, mtp));
		rtn = n_reset_res(q, mtp, mp);
		break;
	default:
		swerr();
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		mtp_set_state(mtp, oldstate);
	return (rtn);
}

/*
 *  Primitives from MTP to NPI.
 *  -----------------------------------
 */
STATIC int
mtp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct mtp *mtp = MTP_PRIV(q);
	ulong oldstate = mtp_get_state(mtp);
	/* 
	   Fast Path */
	if ((prim = *((ulong *) mp->b_rptr)) == MTP_TRANSFER_IND) {
		printd(("%s: %p: MTP_TRANSFER_IND [%d] <-\n", MOD_NAME, mtp, msgdsize(mp->b_cont)));
		if ((rtn = mtp_transfer_ind(q, mtp, mp)) < 0)
			mtp_set_state(mtp, oldstate);
		return (rtn);
	}
	switch (prim) {
	case MTP_OK_ACK:
		printd(("%s: %p: MTP_OK_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_ok_ack(q, mtp, mp);
		break;
	case MTP_ERROR_ACK:
		printd(("%s: %p: MTP_ERROR_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_error_ack(q, mtp, mp);
		break;
	case MTP_BIND_ACK:
		printd(("%s: %p: MTP_BIND_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_bind_ack(q, mtp, mp);
		break;
	case MTP_ADDR_ACK:
		printd(("%s: %p: MTP_ADDR_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_addr_ack(q, mtp, mp);
		break;
	case MTP_INFO_ACK:
		printd(("%s: %p: MTP_INFO_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_info_ack(q, mtp, mp);
		break;
	case MTP_OPTMGMT_ACK:
		printd(("%s: %p: MTP_OPTMGMT_ACK <-\n", MOD_NAME, mtp));
		rtn = mtp_optmgmt_ack(q, mtp, mp);
		break;
	case MTP_TRANSFER_IND:
		printd(("%s: %p: MTP_TRANSFER_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_transfer_ind(q, mtp, mp);
		break;
	case MTP_PAUSE_IND:
		printd(("%s: %p: MTP_PAUSE_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_pause_ind(q, mtp, mp);
		break;
	case MTP_RESUME_IND:
		printd(("%s: %p: MTP_RESUME_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_resume_ind(q, mtp, mp);
		break;
	case MTP_STATUS_IND:
		printd(("%s: %p: MTP_STATUS_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_status_ind(q, mtp, mp);
		break;
	case MTP_RESTART_BEGINS_IND:
		printd(("%s: %p: MTP_RESTART_BEGINS_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_restart_begins_ind(q, mtp, mp);
		break;
	case MTP_RESTART_COMPLETE_IND:
		printd(("%s: %p: MTP_RESTART_COMPLETE_IND <-\n", MOD_NAME, mtp));
		rtn = mtp_restart_complete_ind(q, mtp, mp);
		break;
	default:
		swerr();
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		mtp_set_state(mtp, oldstate);
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_w_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	/* 
	   data from above */
	printd(("%s: %p: -> M_DATA [%d]\n", MOD_NAME, mtp, msgdsize(mp)));
	return n_data(q, mtp, mp);
}
STATIC int
mtp_r_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	/* 
	   data from below */
	printd(("%s: %p: M_DATA [%d] <-\n", MOD_NAME, mtp, msgdsize(mp)));
	return mtp_data(q, mtp, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
mtp_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mtp_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mtp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
#if 0
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
#endif
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
mtp_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mtp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mtp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
#if 0
	case M_IOCACK:
		return mtp_r_iocack(q, mp);
	case M_IOCNAK:
		return mtp_r_iocnak(q, mp);
#endif
	}
	return (QR_PASSALONG);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct mtp *mtp;
		/* test for multiple push */
		for (mtp = mtp_opens; mtp; mtp = mtp->next) {
			if (mtp->u.dev.cmajor == cmajor && mtp->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(mtp_alloc_priv(q, &mtp_opens, devp, crp))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		qprocson(q);
#if 0
		/* 
		   generate immediate information request */
		if ((err = sdt_info_req(q, mtp)) < 0) {
			mtp_free_priv(q);
			MOD_DEC_USE_COUNT;
			return (-err);
		}
#endif
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	mtp_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *mtp_priv_cachep = NULL;
STATIC int
mtp_init_caches(void)
{
	if (!mtp_priv_cachep &&
	    !(mtp_priv_cachep = kmem_cache_create
	      ("mtp_priv_cachep", sizeof(struct mtp), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate mtp_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized module private structure cace\n", MOD_NAME));
	return (0);
}
STATIC int
mtp_term_caches(void)
{
	if (mtp_priv_cachep) {
		if (kmem_cache_destroy(mtp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed mtp_priv_cachep\n", MOD_NAME));
	}
	return (0);
}
STATIC struct mtp *
mtp_alloc_priv(queue_t *q, struct mtp **mtpp, dev_t *devp, cred_t *crp)
{
	struct mtp *mtp;
	if ((mtp = kmem_cache_alloc(mtp_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated module private structure\n", MOD_NAME));
		bzero(mtp, sizeof(*mtp));
		mtp_get(mtp);	/* first get */
		mtp->u.dev.cmajor = getmajor(*devp);
		mtp->u.dev.cminor = getminor(*devp);
		mtp->cred = *crp;
		(mtp->oq = RD(q))->q_ptr = mtp_get(mtp);
		(mtp->iq = WR(q))->q_ptr = mtp_get(mtp);
		spin_lock_init(&mtp->qlock);	/* "mtp-queue-lock" */
		mtp->o_prim = &mtp_r_prim;
		mtp->i_prim = &mtp_w_prim;
		mtp->o_wakeup = NULL;
		mtp->i_wakeup = NULL;
		mtp->i_state = NS_UNBND;
		mtp->i_style = LMI_STYLE1;
		mtp->i_version = 1;
		spin_lock_init(&mtp->lock);	/* "mtp-priv-lock" */
		if ((mtp->next = *mtpp))
			mtp->next->prev = &mtp->next;
		mtp->prev = mtpp;
		*mtpp = mtp_get(mtp);
		printd(("%s: linked module private structure\n", MOD_NAME));
		mtp->prot.PRIM_type = N_INFO_ACK;
		mtp->prot.NSDU_size = -1;
		mtp->prot.ENSDU_size = -2;
		mtp->prot.CDATA_size = -2;
		mtp->prot.DDATA_size = -2;
		mtp->prot.ADDR_size = sizeof(struct mtp_addr);
		mtp->prot.OPTIONS_flags = 0;
		mtp->prot.NIDU_size = -1;
		mtp->prot.SERV_type = N_CLNS;
		mtp->prot.CURRENT_state = NS_UNBND;
		mtp->prot.PROVIDER_type = N_SNICFP;
		mtp->prot.NODU_size = 279;
		mtp->prot.NPI_version = N_VERSION_2;
		printd(("%s: setting module private structure defaults\n", MOD_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", MOD_NAME));
	return (mtp);
}
STATIC void
mtp_free_priv(queue_t *q)
{
	struct mtp *mtp = MTP_PRIV(q);
	psw_t flags;
	ensure(mtp, return);
	spin_lock_irqsave(&mtp->lock, flags);
	{
		ss7_unbufcall((str_t *) mtp);
		if ((*mtp->prev = mtp->next))
			mtp->next->prev = mtp->prev;
		mtp->next = NULL;
		mtp->prev = &mtp->next;
		mtp_put(mtp);
		mtp->oq->q_ptr = NULL;
		flushq(mtp->oq, FLUSHALL);
		mtp->oq = NULL;
		mtp_put(mtp);
		mtp->iq->q_ptr = NULL;
		flushq(mtp->iq, FLUSHALL);
		mtp->iq = NULL;
		mtp_put(mtp);
	}
	spin_unlock_irqrestore(&mtp->lock, flags);
	mtp_put(mtp);		/* final put */
}
STATIC struct mtp *
mtp_get(struct mtp *mtp)
{
	atomic_inc(&mtp->refcnt);
	return (mtp);
}
STATIC void
mtp_put(struct mtp *mtp)
{
	if (atomic_dec_and_test(&mtp->refcnt)) {
		kmem_cache_free(mtp_priv_cachep, mtp);
		printd(("%s: %p: freed mtp private structure\n", MOD_NAME, mtp));
	}
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
MODULE_PARM_DESC(modid, "Module ID for the MTP-NPI module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw mtp_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mtp_npiinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
mtp_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&mtp_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
mtp_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&mtp_fmod)) < 0)
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
mtp_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&mtp_npiinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
mtp_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&mtp_npiinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
mtp_npiinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = mtp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = mtp_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		mtp_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
mtp_npiterminate(void)
{
	int err;
	if ((err = mtp_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = mtp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(mtp_npiinit);
module_exit(mtp_npiterminate);

#endif				/* LINUX */
