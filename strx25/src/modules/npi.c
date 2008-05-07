/*****************************************************************************

 @(#) $RCSfile: npi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:41 $

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

 Last Modified $Date: 2008-05-07 16:01:41 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi.c,v $
 Revision 0.9.2.1  2008-05-07 16:01:41  brian
 - added NLI X.25-PLP CONS and XX25 implemetnation

 *****************************************************************************/

#ident "@(#) $RCSfile: npi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:41 $"

static char const ident[] = "$RCSfile: npi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:41 $";

/*
 * This is an NPI module for NLI.  It pushes over an NLI stream and provide an
 * NPI interface to that Stream.  This is in fitting with IBM's approach with
 * AIXlink/X.25 (the "X.25 Licensed Product", whatever that means), but also
 * provides for QOS.
 *
 * From a development perspective, a competing approach is the NPI driver.
 * The driver links a single NLI stream beneath it for use by the entire NPI
 * driver.  This relies on the possibility that more than one network
 * connection can be supported by a single NLI stream.  However, the SX.25
 * documentation by SGI indicates otherwise when it says:
 *
 *   To perform any of the operations described in this section, the
 *   application must open a stream to the X.25 PLP driver.  Once the stream
 *   ahs been opened, it can be used for initiating, listening for, or
 *   accepting a connection.  There is a one-to-eon mapping between X.25
 *   virtual circuits and PLP driver streams.  Once a connection has been
 *   established on a stream, the stream cannot be used other than for passing
 *   data and protocol messages for that connection.
 *
 * Therefore, to implement the NPI, it must be implemented as a module that is
 * pushed over an NLI stream available from the x25-plp driver.
 */

#include <sys/os8/compat.h>
#include <sys/npi.h>
#include <sys/npi_x25.h>

#define NPI_DESCRIP	"NLI to NPI CONVERSION MODULE FOR LINUX FAST-STREAMS"
#define NPI_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define NPI_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define NPI_REVISION	"OpenSS7 $RCSfile: npi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:41 $"
#define NPI_DEVICE	"SVR 4.2MP NLI to NPI Conversion Module (NPI) for X.25 CONS"
#define NPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NPI_LICENSE	"GPL"
#define NPI_BANNER	NPI_DESCRIP	"\n" \
			NPI_EXTRA	"\n" \
			NPI_COPYRIGHT	"\n" \
			NPI_REVISION	"\n" \
			NPI_DEVICE	"\n" \
			NPI_CONTACT	"\n"
#define NPI_SPLASH	NPI_DESCRIP	" - " \
			NPI_REVISION

#ifndef CONFIG_STREAMS_NPI_NAME
#error "CONFIG_STREAMS_NPI_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_NPI_MODID
#error "CONFIG_STREAMS_NPI_MODID must be defined."
#endif

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(NPI_CONTACT);
MODULE_DESCRIPTION(NPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(NPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(NPI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-npi");
MODULE_ALIAS("streams-module-npi");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_SREAMS_NPI_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef NPI_MOD_NAME
#define NPI_MOD_NAME		CONFIG_STREAMS_NPI_NAME
#endif				/* NPI_MOD_NAME */
#ifndef NPI_MOD_ID
#define NPI_MOD_ID		CONFIG_STREAMS_NPI_MODID
#endif				/* NPI_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		NPI_MOD_ID
#define MOD_NAME	NPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	NPI_BANNER
#else				/* MODULE */
#define MOD_SPLASH	NPI_SPLASH
#endif				/* MODULE */

static struct module_info np_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat np_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat np_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct np;
sturct nl;

/* Upper interface structure */
struct np {
	struct nl *nl;
	queue_t *oq;
	np_ulong state;
	np_ulong oldstate;
	ushort add_len;
	struct {
		N_info_ack_t info;
		unsigned char add[20];
		N_qos_co_opt_sel_t qos;
		N_qos_co_opt_range_t qor;
	} proto;
	N_qos_co_sel_t qos;
	N_qos_co_range_t qor;
	ushort loc_len;
	unsigned char loc[20];
	ushort rem_len;
	unsigned char rem[20];
};

/* Lower interface structure */
struct nl {
	struct np *np;
	queue_t *oq;
	int state;
	int oldstate;
	struct qosformat qos;
	struct xaddrf clg;
	struct xaddrf cld;
	struct xaddrf res;
	struct xaddrf def;
};

struct priv {
	struct np npi;
	struct nl nli;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define NP_PRIV(q) (&PRIV(q)->npi)
#define NL_PRIV(q) (&PRIV(q)->nli)

static inline const char *
np_iocname(int cmd)
{
	switch (cmd) {
#if 0
	case NPI_IOCGOPTION:
		return ("NPI_IOCGOPTION");
#endif
	default:
		return ("(unknown)");
	}
}

static inline const char *
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
		return ("(unknown)");
	}
}

static inline const char *
np_statename(np_ulong state)
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
		return ("(unknown)");
	}
}

static const char *
np_strerror(np_long error)
{
	if (err < 0)
		err = NSYSERR;
	switch (error) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information.");
	case NBADOPT:
		return ("Options in incorrect format or contain illegal information.");
	case NACCESS:
		return ("User did no have proper permissions.");
	case NNOADDR:
		return ("NS Provider could not allocate address.");
	case NOUTSTATE:
		return ("Primitive was issued in wrong sequence.");
	case NBADSEQ:
		return ("Sequence number in primitive was incorrect/illegal.");
	case NSYSERR:
		return ("UNIX system error occured.");
	case NBADDATA:
		return ("User data spec. outside rnage supported by NS provider.");
	case NBADFLAG:
		return ("Flags specified in primitive were illegal/incorrect.");
	case NNOTSUPPORT:
		return ("Primitive type not supported by the NS provider.");
	case NBOUND:
		return ("Illegal second attempt to bind listenter or default listener.");
	case NBADQOSPARAM:
		return ("QOS values outside range supported by NS provider.");
	case NBADQOSTYPE:
		return ("QOS structure type not supported by NS provider.");
	case NBADTOKEN:
		return ("Token used is not associated with an open stream.");
	case NNOPROTOID:
		return ("Protocol id could not be allocated.");
	default:
		return ("(unknown)");
	}
}

static np_ulong
np_get_state(sturct np * np)
{
	return (np->state);
}

static np_ulong
np_set_state(struct np *np, np_ulong newstate)
{
	np_ulong oldstate = np->state;

	if (newstate != oldstate) {
		np->state = newstate;
		np->proto.info.CURRENT_state = newstate;
		mi_strlog(np->oq, STRLOGST, SL_TRACE, "%s <- %s", np_statename(newstate),
			  np_statename(oldstate));
	}
	return (newstate);
}

static np_ulong
np_save_state(struct np *np)
{
	return ((np->oldstate = np_get_state(np)));
}

static np_ulong
np_restore_state(struct np *np)
{
	return (np_set_state(np, np->oldstate));
}

static const char *
nl_primname(int prim)
{
	switch (prim) {
	default:
		return ("(unknown)");
	}
}

static const char *
nl_statename(int state)
{
	switch (state) {
	default:
		return ("(unknown)");
	}
}

static int
nl_get_state(struct nl *nl)
{
	return (nl->state);
}

static int
nl_set_state(struct nl *nl, int newstate)
{
	int oldstate = nl->state;

	if (newstate != oldstate) {
		nl->state = newstate;
		mi_strlog(nl->oq, STRLOGST, SL_TRACE, "%s <- %s", nl_statename(newstate),
			  nl_statename(oldstate));
	}
	return (newstate);
}

static int
nl_save_state(struct nl *nl)
{
	return ((nl->oldstate = nl_get_state(nl)));
}

static int
nl_restore_state(struct nl *nl)
{
	return (nl_set_state(nl, nl->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * NPI PROVIDER TO NPI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * Note: although the variable length fields in primitives received from the
 * user are not necessarily aligned on appropriate boundaries, when we sent
 * primitives to the user, we always align variable length fields on a pointer
 * boundary.  This permits users the ability to directly point to structures
 * within the field.
 */

#define N_ALIGN(len) ((len + sizeof(caddr_t) - 1) & ~(sizeof(caddr_t) - 1))

static fastcall noinline __unlikely int
m_error(struct np *np, queue_t *q, mblk_t *msg)
{
}

/**
 * np_conn_ind: - issue an N_CONN_IND primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dst: buffer describing destination address
 * @src: buffer describing source address
 * @seq: sequence number (always non-zero)
 * @flags: connection option flags
 * @qos: quality of service selected (N_qos_co_sel_t)
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_conn_ind(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *dst,
	    struct strbuf *src, np_ulong seq, np_ulong flags, struct strbuf *qos, mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + N_ALIGN(dst->len) + N_ALIGN(src->len) + N_ALIGN(qos->len);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_IND;
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p);
			p->SRC_length = src->len;
			p->SRC_offset = p->DEST_offset + N_ALIGN(p->DEST_length);
			p->SEQ_number = seq;
			p->CONN_flags = flags;
			p->QOS_length = qos->len;
			p->QOS_offset = p->SRC_offset + N_ALIGN(p->SRC_length);
			mp->b_wptr += sizeof(*p);
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += N_ALIGN(dst->len);
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += N_ALIGN(src->len);
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += N_ALIGN(qos->len);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_IND");
			np->coninds++;
			np_set_state(np, NS_WRES_CIND);
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_conn_con: - issue an N_CONN_CON primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: buffer describing responding address
 * @flags: connection options flags
 * @qos: buffer describing quality of service parameters (N_qos_co_sel_t)
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_conn_con(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *res, np_ulong flags,
	    struct strbuf *qos, mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + N_ALIGN(res->len) + N_ALIGN(qos->len);

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = res->len;
			p->RES_offset = sizeof(*p);
			p->CONN_flags = flags;
			p->QOS_length = qos->len;
			p->QOS_offset = p->RES_offset + N_ALIGN(p->RES_length);
			mp->b_wptr += sizeof(*p);
			bcopy(res->buf, mp->b_wptr, res->len);
			mp->b_wptr += N_ALIGN(res->len);
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += N_ALIGN(qos->len);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_CON");
			np_set_state(np, NS_DATA_XFER);
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_discon_ind: - issue an N_DISCON_IND primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @orig: disconnect origin
 * @reason; disconnect reason
 * @res: buffer describing responding address
 * @seq: sequence number (or zero)
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_discon_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason,
	      struct strbuf *res, np_ulong seq, mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + N_ALIGN(res->len);

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_IND;
			p->DISCON_orig = orig;
			p->DISCON_reason = reason;
			p->RES_length = res->len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(res->buf, mp->b_wptr, res->len);
			mp->b_wptr += N_ALIGN(res->len);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DISCON_IND");
			if (seq != 0)
				np->coninds--;
			np_set_state(np, (np->coninds > 0) ? NS_WRES_CIND : NS_IDLE);
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_data_ind: - issue an N_DATA_IND primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: data transfer flags
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_data_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_IND;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DATA_IND");
			/* When the receipt confirmation flag is set and the
			   more data flag is not set we increase the number of
			   outstanding data indication acknolwedgements
			   pending. */
			if ((flags & N_RC_FLAG) != 0 && (flags & N_MORE_DATA_FLAG) == 0)
				np->datinds++;
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_exdata_ind: - issue an N_EXDATA_IND primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_exdata_ind(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_EXDATA_IND");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_info_ack: - issue an N_INFO_ACK primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_info_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) +
	    N_ALIGN(np->proto.info.ADDR_length) +
	    N_ALIGN(np->proto.info.QOS_length) +
	    N_ALIGN(np->proto.info.QOS_range_length) + N_ALIGN(np->proto.info.PROTOID_length);

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = np->proto.info.NSDU_size;
		p->ENSDU_size = np->proto.info.ENSDU_size;
		p->CDATA_size = np->proto.info.CDATA_size;
		p->DDATA_size = np->proto.info.DDATA_size;
		p->ADDR_size = np->proto.info.ADDR_size;
		p->ADDR_length = np->proto.info.ADDR_length;
		p->ADDR_offset = sizeof(*p);
		p->QOS_length = np->proto.info.QOS_length;
		p->QOS_offset = p->ADDR_offset + N_ALIGN(p->ADDR_length);
		p->QOS_range_length = np->proto.info.QOS_range_length;
		p->QOS_range_offset = p->QOS_offset + N_ALIGN(p->QOS_length);
		p->OPTIONS_flags = np->proto.info.OPTIONS_flags;
		p->NIDU_size = np->proto.info.NIDU_size;
		p->SERV_type = np->proto.info.SERV_type;
		p->CURRENT_state = np->proto.info.CURRENT_state;
		p->PROVIDER_type = np->proto.info.PROVIDER_type;
		p->NODU_size = np->proto.info.NODU_size;
		p->PROTOID_length = np->proto.info.PROTOID_length;
		p->PROTOID_offset = p->QOS_range_offset + N_ALIGN(p->QOS_range_length);
		p->NPI_version = np->proto.info.NPI_version;
		mp->b_wptr += sizeof(*p);
		bcopy(np->proto.add, mp->b_wptr, p->ADDR_length);
		mp->b_wptr += N_ALIGN(p->ADDR_length);
		bcopy(np->proto.qos, mp->b_wptr, p->QOS_length);
		mp->b_wptr += N_ALIGN(p->QOS_length);
		bcopy(np->proto.qor, mp->b_wptr, p->QOS_range_length);
		mp->b_wptr += N_ALIGN(p->QOS_range_length);
		bcopy(np->proto.pro, mp->b_wptr, p->PROTOID_length);
		mp->b_wptr += N_ALIGN(p->PROTOID_length);
		freemsg(msg);
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_INFO_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_bind_ack: - issue an N_BIND_ACK primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @add: buffer describing address
 * @conind: maximum outstanding connection indications
 * @token: token value
 * @pro: buffer describing protocol identifiers
 */
static fastcall noinline __unlikely int
np_bind_ack(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *add, np_ulong conind,
	    np_ulong token, struct strbuf *pro)
{
	N_bind_ack_t *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + N_ALIGN(add->len) + N_ALIGN(pro->len);

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add->len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = conind;
		p->TOKEN_value = token;
		p->PROTOID_length = pro->len;
		p->PROTOID_offset = p->ADDR_offset + N_ALIGN(p->ADDR_length);
		mp->b_wptr += sizeof(*p);
		bcopy(add->buf, mp->b_wptr, add->len);
		mp->b_wptr += N_ALIGN(add->len);
		bcopy(pro->buf, mp->b_wptr, pro->len);
		mp->b - wptr += N_ALIGN(pro->len);
		freemsg(msg);
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_BIND_ACK");
		np_set_state(np, NS_IDLE);
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_error_ack: - issue an N_ERROR_ACK primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: positive NPI error or negative UNIX error
 */
static fastcall noinline __unlikely int
np_error_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_long error)
{
	N_error_ack_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = error > 0 ? error : NSYSERR;
		p->UNIX_error = error > 0 ? 0 : -error;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (np_get_state(np)) {
		case NS_WACK_BREQ:
			np_set_state(np, NS_UNBND);
			break;
		case NS_WCON_CREQ:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_CRES:
			np_set_state(np, NS_WRES_CIND);
			break;
		case NS_WACK_RRES:
			np_set_state(np, NS_WRES_RIND);
			break;
		case NS_WACK_DREQ6:
			np_set_state(np, NS_WCON_CREQ);
			break;
		case NS_WACK_DREQ7:
			np_set_state(np, NS_WRES_CIND);
			break;
		case NS_WACK_DREQ9:
			np_set_state(np, NS_DATA_XFER);
			break;
		case NS_WACK_DREQ10:
			np_set_state(np, NS_WCON_RREQ);
			break;
		case NS_WACK_DREQ11:
			np_set_state(np, NS_WRES_RIND);
			break;
		case NS_WACK_UREQ:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_OPTREQ:
			np_set_state(np, NS_IDLE);
			break;
		default:
			/* This does not necessarily indicate an error,
			   especially as the provider does not change state to
			   NS_WACK_OPTREQ when not in the NS_IDLE state. */
			break;
		}
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_error_reply: - issued N_ERROR_ACK filtering out normal return errors
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: positive NPI error negative UNIX error
 * @func: calling function
 */
static fastcall noinline __unlikely int
np_error_reply(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_long error,
	       const char *func)
{
	int err;

	switch (error) {
	case -ENOBUFS:
	case -EDEADLK:
	case -ENOSR:
	case -EBUSY:
	case -ENOMEM:
	case -EAGAIN:
		return (error);
	}
	if ((err = np_error_ack(np, q, msg, prim, error)) == 0)
		mi_strlog(np->oq, 0, SL_TRACE, "%s: %s: %s: %s", func, np_primname(prim),
			  np_errname(error), np_strerror(error));
	return (err);
}

/**
 * np_ok_ack: - issue an N_OK_ACK primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct primitive
 * @ap: accepting network provider stream for N_CONN_RES.
 */
static fastcall noinline __unlikely int
np_ok_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, struct np *ap)
{
	N_ok_ack_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (np_get_state(np)) {
		case NS_WACK_UREQ:
			np_set_state(np, NS_UNBND);
			break;
		case NS_WACK_CRES:
			np->coninds--;
			if (ap == np)
				np_set_state(np, NS_DATA_XFER);
			else
				np_set_state(np, (np->coninds > 0) ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_RRES:
			np->resinds--;
			np_set_state(np, (np->resinds > 0) ? NS_WRES_RIND : NS_DATA_XFER);
			break;
		case NS_WACK_OPTREQ:
			np_set_state(np, NS_IDLE);
			break;
		default:
			/* Not necessarily an error.  When not in the NS_IDLE
			   state, N_OPTMGMT_REQ does not change the state to
			   NS_WACK_OPTREQ. */
			break;
		}
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_OK_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_unitdata_ind: - issue an N_UNITDATA_IND primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dst: buffer describing destination address
 * @src: buffer describing source address
 * @etype: error type
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_unitdata_ind(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *dst, struct strbuf *src,
		np_ulong etype, mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + N_ALIGN(dst->len) + N_ALIGN(src->len);

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p);
			p->SRC_length = src->len;
			p->SRC_offset = p->DEST_offset + N_ALIGN(p->DEST_length);
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += N_ALIGN(dst->len);
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += N_ALIGN(src->len);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_UNITDATA_IND");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_uderror_ind: - issue an N_UDERROR_IND primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dst: buffer describing destination address
 * @etype: error type
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_uderror_ind(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *dst, np_ulong etype,
	       mblk_t *dp)
{
	N_uderror_ind_t *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + N_ALIGN(dst->len);

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p);
			p->RESERVED_field[0] = 0;
			p->RESERVED_field[1] = 0;
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += N_ALIGN(dst->len);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_UDERROR_IND");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_datack_ind: - issue an N_DATACK_IND primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_datack_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DATACK_IND");
			np->datcons--;
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_reset_ind: - issue an N_RESET_IND primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @orig: reset origin
 * @reason: reset reason
 * @dp: user data
 *
 * Note: in some circumstances, the user data may contain zero, one or two
 * octets, the first octet indicating the X.25 cause value and the second
 * octet indicating the X.25 diagnostic value.
 */
static fastcall noinline __unlikely int
np_reset_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason)
{
	N_reset_ind_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = orig;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_IND");
			np->resinds++;
			np_set_state(np, NS_WRES_RIND);
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_reset_con: - issue an N_RESET_CON primitive
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: user data
 *
 * In some circumstances the user data may contain zero, one or two octets,
 * the first octet being the X.25 CCITT cause value and the second octet
 * containing the X.25 CCITT diagnostic value.
 */
static fastcall noinline __unlikely int
np_reset_con(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_reset_con_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(canputnext(np->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_CON");
			np->rescons--;
			np_set_state(np, (np->rescons > 0) ? NS_WCON_RREQ : NS_DATA_XFER);
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * NLI USER TO NLI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

/**
 * nl_conn_req: - issue N_CI request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @conn_id: connection identfier
 * @cons: CONS call when true
 * @cld: called address
 * @clg: calling address
 * @qos: qos parameters (or NULL if no qos)
 * @dp: user data
 */
static fastcall noinline __unlikely int
nl_conn_req(struct nl *nl, queue_t *q, mblk_t *msg, int conn_id, uchar cons, struct xaddrf *cld,
	    struct xaddrf *clg, struct qosformat *qos, mblk_t *dp)
{
	struct xcallf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_CI;
			p->conn_id = conn_id;
			p->CONS_call = cons;
			p->negotiate_qos = qos ? 1 : 0;
			bcopy(cld, &p->calledaddr, sizeof(*cld));
			bcopy(clg, &p->callingaddr, sizeof(*clg));
			if (qos)
				bcopy(qos, &p->qos, sizeof(*qos));
			else
				bzero(&p->qos, sizeof(*qos));
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			nl_set_state(nl, NS_WCON_CREQ);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_CI ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_conn_res: - issue N_CC response
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @conn_id: connection identifier
 * @cons: CONS call when true
 * @res: responding address
 * @qos: qos parameters (or NULL if no qos)
 * @dp: user data
 */
static fastcall noinline __unlikely int
nl_conn_res(struct nl *nl, queue_t *q, mblk_t *msg, int conn_id, uchar cons, struct xaddrf *res,
	    struct qosformat *qos, mblk_t *dp)
{
	struct xccnff *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq) != 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_CC;
			p->conn_id = conn_id;
			p->CONS_call = cons;
			p->negotiate_qos = qos ? 1 : 0;
			bcopy(res, &p->responder, sizeof(*res));
			if (qos)
				bcopy(qos, &p->qos, sizeof(*qos));
			else
				bzero(&p->qos, sizeof(*qos));
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			nl_set_state(nl, NS_WCON_CREQ);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_CC ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_discon_req: - issue a N_DI request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: originator
 * @reason: reason
 * @cause: cause
 * @diag: diagnostic
 * @conn_id: connection id
 * @res: responding address
 * @def: deflected address
 * @qos: qos parameters (or NULL)
 * @dp: user data (CUD)
 */
static fastcall noinline __unlikely int
nl_discon_req(struct nl *nl, queue_t *q, mblk_t *msg,
		uchar orig, uchar reason, uchar cause,
	      uchar diag, int conn_id, struct xaddrf *res, struct xaddrf *def,
	      struct qosformat *qos, mblk_t *dp)
{
	struct xdiscf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_DI;
			p->originator = orig;
			p->reason = reason;
			p->cause = cause;
			p->diag = diag;
			p->conn_id = conn_id;
			p->indicated_qos = qos ? 1 : 0;
			bcopy(res, &p->responder, sizeof(*res));
			bcopy(def, &p->deflected, sizeof(*def));
			if (qos)
				bcopy(qos, &p->qos, sizeof(*qos));
			else
				bzero(&p->qos, sizeof(*qos));
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			nl_set_state(nl, nl_sequence_free(nl, conn_id) ? NS_WRES_CIND : NS_IDLE);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_DI ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_data_req: - issue N_Data request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @more: more data in nsdu
 * @dbit: D-bit data
 * @qbit: Q-bit data
 * @dp: user data
 */
struct fastcall inline __hot_write int
nl_data_req(struct nl *nl, queue_t *q, mblk_t *msg, uchar more, uchar dbit, uchar qbit, mblk_t *dp)
{
	struct xdataf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_DAT;
			p->xl_command = N_Data;
			p->More = more;
			p->setDbit = dbit;
			p->setQbit = qbit;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			if (dbit && !more)
				nl->datcons++;
			mi_strlog(nl->oq, STRLOGDA, SL_TRACE, "N_Data ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

/**
 * nl_exdata_req: - issue N_EData request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
struct fastcall inline __hot_write int
nl_exdata_req(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct xedataf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(nl->oq, 1))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_DAT;
			p->xl_command = N_EData;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			nl->edatcons++;
			mi_strlog(nl->oq, STRLOGDA, SL_TRACE, "N_EData ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_bind_req: - issue an N_Xlisten request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @lmax: maximum number of CIs at a time
 * @pri; priority request
 * @dp: user data containing called address specification
 *
 * The major features of listening are as follows:
 *
 * - Any number of processes can listen simultaneously, subject to resource
 *   constraints imposed by the system adminstrator.  Moreover, any number of
 *   these processes can listen at the same (set of) called addresses.  Note
 *   that there are no means of listening for a particular calling address.
 *
 * - An application can elect to listen and handle one or more Connect
 *   Indications at a time.  The most likely use of this feature is when the
 *   application wants to make use of the next facility.
 *
 * - An incoming connection may be accepted on a stream other than the one
 *   that received the Connect Indication (the listening stream).
 *
 * When an application wishes to listen for incoming calls, it must specify
 * the (called) address(es) and Call User Data (CUD) field values for which it
 * is prepared to accept calls.  The data that does this is passed as part of
 * a Listen Request.
 *
 * The control part of the message is accompanied by a data part containing
 * the addresses to be registered for incoming calls.  The data portion is
 * treated as a byte stream of CUD and addresses conforming to the following
 * definition:
 * 
 *     unsigned char l_cumode;
 *     unsigned char l_culength;
 *     unsigned char l_cubytes[l_culength];
 *     unsigned char l_mode;
 *     unsigned char l_type;
 *     unsigned char l_length;
 *     unsigned char l_add[(l_length+1)>>1];
 *
 * It is important to note that, depending on both the value of the "mode"
 * bytes and the lengths, not all fields need be present.  Refer to the
 * individual field descriptions for more details.
 */
static fastcall noinline __unlikely int
nl_bind_req(struct nl *nl, queue_t *q, mblk_t *msg, int lmax, uchar pri)
{
	struct xlistenf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(pri || canputnext(nl->oq))) {
			DB_TYPE(mp) = pri ? M_PCPROTO : M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_Xlisten;
			p->lmax = lmax;
			p->l_result = 0;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			nl_set_state(nl, NS_WACK_BREQ);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_Xlisten ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_unbind_req: - issue N_Xcanlis request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * The Listen Cancel Request removes all listen addresses from the stream.
 * There is no way of cancelling a Listen Request on a particular address;
 * this message is probably used when the use of the stream is about to be
 * changed by the application.
 */
static fastcall noinline __unlikely int
nl_unbind_req(struct nl *nl, queue_t *q, mblk_t *msg)
{
	struct xcanlisf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_Xcanlis;
			p->c_result = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			nl_set_state(nl, NS_WACK_UREQ);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_Xcanlis ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_datack_req: - issue N_DAck or N_EAck request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @exp: expedited acknowledgement
 */
static fastcall inline __hot_read int
nl_datack_req(struct nl *nl, queue_t *q, mblk_t *msg, uchar exp)
{
	struct xdatacf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(nl->oq, exp ? 1 : 0))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = exp ? 1 : 0;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_DAT;
			p->xl_command = exp ? N_EAck : N_DAck;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			if (exp)
				nl->edatinds--;
			else
				nl->datinds--;
			mi_strlog(nl->oq, STRLOGDA, SL_TRACE, exp ? "N_EAck ->" : "N_DAck ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_reset_req: - issue N_RI request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: originatore
 * @reason: reason
 * @cause: cause
 * @diag: diagnostic
 */
static fastcall noinline __unlikely int
nl_reset_req(struct nl *nl, queue_t *q, mblk_t *msg, uchar orig, uchar reason,
		uchar cause, uchar diag)
{
	struct xrstf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_RI;
			p->originator = orig;
			p->reason = reason;
			p->cause = cause;
			p->diag = diag;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			nl->rescons++;
			nl_set_state(nl, NS_WCON_RREQ);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_RI ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_reset_res: - issue N_RC response
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
nl_reset_res(struct nl *nl, queue_t *q, mblk_t *msg)
{
	struct xrscf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_RC;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			nl->resinds--;
			nl_set_state(nl, (nl->resinds > 0) ? NS_WRES_RIND : NS_DATA_XFER);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_RC ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_attach_req: - issue N_PVC_ATTACH request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @lci: logical channel
 * @sn_id: subnetwork (link) identifier 
 * @ack: receipt acknowledgement requested when set
 * @lmi: nsdulimit requested when set
 * @nsdulimit: the limit
 */
static fastcall noinline __unlikely int
nl_attach_req(struct nl *nl, queue_t *q, mblk_t *msg, ushort lci, uint sn_id,
		uchar ack, uchar lmi, int nsdulimit)
{
	struct pvcattf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p))mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_PVC_ATTACH;
			p->lci = lci;
			p->sn_id = sn_id;
			p->reqackservice = ack;
			p->reqnsdulimit = lim;
			p->nsdulimit = nsdulimit;
			p->result_code = 0;
			m->b_wptr += sizeof(*p);
			freemsg(msg);
			nl_set_state(nl, NS_WCON_CREQ);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_PVC_ATTACH ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * nl_detach_req: - issue N_PVC_DETACH request
 * @nl: network layer private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: reason code
 */
static fastcall noinline __unlikely int
nl_detach_req(struct nl *nl, queue_t *q, mblk_t *msg, int reason)
{
	struct pvcdetf *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(nl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->xl_type = XL_CTL;
			p->xl_command = N_PVC_DETACH;
			p->reason_code = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			nl_set_state(nl, NS_WACK_UREQ);
			mi_strlog(nl->oq, STRLOGTX, SL_TRACE, "N_PVC_DETACH ->");
			putnext(nl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * NPI USER TO NPI PROVIDER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

/**
 * np_conn_req: - process received N_CONN_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_CONN_REQ primitive
 *
 * AIXlink/X.25 uses the following address format: first octet is the line number (i.e. link_id)
 * between 0x00 and 0xFE (Sunlink X.25 says 0xFF has special meaning), the second octet is the
 * prefix: '0' for X.121 address, 'P' for PVC address. For both X.121 and PVC addresses, the
 * address follows in ASCII.  How about using N for NSAP address?
 *
 * X.121 DTE/DCE addresses are formatted as follows: where each digit in the address is a decimal
 * digit 0-9.  A 4-digit DNIC (Data Network Identification Code). When the first digit of the
 * DNIC is 1, it corresponds a satellite mobile or global public data network.  2 thru 7 indicate
 * geographic specific DNIC, where the first three digits of the DNIC are the Data Country Code
 * (DCC).  The fourth and last digit identifiers the specific network within the country.  An
 * international data number consists of a DNIC + NTN or DCC + NN where a national integrated
 * numbering scheme exists (National Number not dependent upon network within country).  NTN can
 * be 10 digits maximum and NN can be 11 digits maximum.
 *
 * In the case of X.25 access, if the non-TOA/NPI address format is used, a prefix indicating
 * international data number format can only be one digit, due to a limit of 15 digits for the
 * X.25 calling/called address field.
 *
 * A one digit escape code can be added to the beginning of the number.
 *
 * 8 -	Indicates that the digits that follow are from the F.69 Numbering Plan.  The continued
 *	use and need for Escape code 8 is for further study.
 *
 * 9 -	Indicates that the digits that follow are from the E.164 Numbering Plan.  In this case, 9
 *	is to indicate that an analogue interface on the destination network (PSTN or integrated
 *	ISDN/PSTN) is requested.
 *
 * 0 -	Indicates that the digits that follow are from E.164 Numbering Plan.  In this case, 0 is
 *	to inicate that a digital interface on the destination network (ISDN or integrated
 *	ISDN/PSTN) is requested.
 *
 * In the case of calls from a Packet Switch Public Data Network (PSPDN) to an integrated
 * ISDN/PSTN that does not require a distinction between digital and analogue interfaces, only a
 * single escape code (e.g. 9 and 0) may be required.  However, all PSPDN interworking with ISDN,
 * PSTN and intergrated ISDN/PSTN should also support both 9 and 0 estape codes when acting as an
 * originating, transit or destination network.
 *
 * Operators of private data networks that are wholly self-contained, but wish to make provision
 * for future connection with a public data network, may establish a numbering scheme for their
 * network that harmonizes with the public data network numbering plan as specified in X.121.
 * This is achieved by inserting a convenient (dummy) value in the DNIC and PNIC (ZXXXXX) fields
 * of the data terminal number such that these fields have no real significance in identifying
 * terminals on that private network.  If the private network numbering plan is tructured such
 * that of the available 14-digit address space, only the digits of the Private Network Terminal
 * field have any actual significance, (i.e. the DNIC and PNIC fields have no real significance
 * as far as identifying individual DTEs, and can be set to any convenient value) it is a
 * relatively simple operation to map that numbering plan to a fully compatible X.121 plan.  DNIC
 * (4 digits) PNIC (2 to 6 digits) PNTN (8 to 4 digits) where only the PNTN identifies a network
 * terminal.
 *
 * With TOA/NPI addressing (A-bit set to 1), addresses can be from zero (0) to 256 semi-octets in
 * length.  The first to semi-octets of each DTE/DXE address is the TOA and NPI semi-octets coded
 * as follows:
 *
 *     TOA  D Description
 *     ---- - -----------------------------------------------------
 *     0000 0 Network-dependent number or unknown
 *     0001 1 International number
 *     0010 2 National number
 *     0011 3 Network specific number (for use in private networks)
 *     0100 4 Complementary address without main address.
 *     0101 5 Alternative address.
 *     1111 F Reserved for extension
 *     XXXX X Reserved
 *     ---- - -----------------------------------------------------
 *
 *     NPI  D Description
 *     ---- - -----------------------------------------------------
 *     0000 0 Network-dependent number or unknown
 *     0001 1 Rec. E.164 (digital)
 *     0010 2 Rec. E.164 (analog)
 *     0011 3 Rec. X.121
 *     0100 4 Rec. F.69 (telex numbering plan)
 *     0101 5 Private numbering plan (for private use only)
 *     1111 F Reserved for extension
 *     XXXX X Reserved
 *     ---- - -----------------------------------------------------
 *
 *     When the TOA is Alternative Address, the NPI field becomes;
 *
 *     NPI  D Description
 *     ---- - -----------------------------------------------------
 *     0000 0 Character string coding in accordance with ISO/IEC 646.
 *     0001 1 OSI NSAP address coded in accordance with ITU Rec. X.213 ISO/IEC 8348.
 *     0010 2 Media Access Control (MAC) address coded in accordance with ISO/IEC 10039.
 *     0011 3 Internet Address coded in accordance with RFC 1166.
 *     XXXX X Reserved
 *     ---- - -----------------------------------------------------
 */
static fastcall noinline __unlikely int
np_conn_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	struct qosformat *qos = NULL;
	np_ulong n_qos_type;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	if (p->DEST_length == 0)
		goto noaddr;
	if (p->DEST_length > sizeof(np->rem))
		goto badaddr;
	bcopy(mp->b_rptr + p->DEST_offset, np->rem, p->DEST_length);
	np->rem_len = p->DEST_length;
	/* FIXME: check validity of address */
	switch (np->rem[0]) {
	}
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (p->QOS_length == 0)
		goto noqos;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->QOS_length < sizeof(n_qos_type))
		goto badopt;
	bcopy(mp->b_rptr + p->QOS_offset, &n_qos_type, sizeof(n_qos_type));
	if (n_qos_type != N_QOS_CO_RANGE)
		goto badqostype;
	if (p->QOS_length < sizeof(np->qos))
		goto badopt;
	bcopy(mp->b_rptr + p->QOS_offset, &np->qos, sizeof(np->qos));
	qos = &np->nl.qos;
      noqos:
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (qos) {
		/* FIXME: move N_qos_co_range_t and CONN_flags into qosformat */
	}
	np_set_state(np, NS_WCON_CREQ);
	/* send N_CI */
	return (nl_conn_req(np->nl, q, mp, 0, true, &nl->cld, &nl->clg, qos, mp->b_cont));
      tooshort:
	err = -EFAULT;
	goto error;
      noaddr:
	err = NNOADDR;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_CONN_REQ, err, __FUNCTION__));
}

/**
 * np_conn_res: - process received N_CONN_RES primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_CONN_RES primitive
 */
static fastcall noinline __unlikely int
np_conn_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_CC */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_CONN_RES, err, __FUNCTION__));
}

/**
 * np_discon_req: - process received N_DISCON_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_DISCON_REQ primitive
 */
static fastcall noinline __unlikely int
np_discon_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_DI */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_DISCON_REQ, err, __FUNCTION__));
}

/**
 * np_data_req: - process received N_DATA_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_DATA_REQ primitive
 */
static fastcall noinline __unlikely int
np_data_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_Data */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_DATA_REQ, err, __FUNCTION__));
}

/**
 * np_exdata_req: - process received N_EXDATA_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_EXDATA_REQ primitive
 *
 * There seems to be some confusion about the expedited data transfer service.
 * X.25 requires that when expedited data is sent that the sender await an
 * expedited data acknowledgement before sending any more data (normal or
 * expedited).  However, the CONS (X.213) service definition does not provide
 * a primitive for indicating expedited data transfer acknowledgement (neither
 * reqest nor indication). AIXlink/X.25 specifies that a N_OK_ACK is used for
 * an expedited data transfer acknolwedgement indication; however, NPI
 * Revision 2.0.0 follows X.213 CONS and has no such sequence of primitives.
 *
 * From X.223 we have:
 *
 *   When an NL entity recieves an N-EXPEDITED-DATA request primitive from an
 *   NS user, it transmits an INTERRUPT packet across the DTE/DXE interface.
 *   An NL entity shall not transmit a second INTERRUPT packet before an
 *   outstanding INTERRUPT packet has been confirmed by an INTERRUPT
 *   CONFIRMATION packet.
 *
 *   When an NL entity receives an INTERRUPT packet, it signals an
 *   N-EXPEDITED-DATA indication primitive to the NS user.  It also transmits
 *   an INTERRUPT CONFIRMATION packet accorss the DTE/DXE interface.
 *
 *   The Interrupt User Data Field of X.25/PLP INTERRUPT packets is used to
 *   transfer expedited NS-user-data between NS users.
 *
 * This indicates the AIXlink/X.25 is pretty much wrong.  It is only the
 * obligation of the NL entity not to send another INTERRUPT packet before the
 * first one is acknowledged.  This does not mean that the NS User cannot
 * queue additional N-EXPEDITED-DATA requests or N-DATA requests to the NL
 * entity as NS provider.
 *
 * We should, therefore, note in the porting notes that when porting
 * AIXlink/X.25 applications, that the NS-User should not expect an N_OK_ACK
 * response to an N_EXDATA_REQ primitive but may issue another N_EXDATA_REQ
 * primitive or an N_DATA_REQ primitive at any time.
 */
static fastcall noinline __unlikely int
np_exdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_exdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_EData */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_EXDATA_REQ, err, __FUNCTION__));
}

/**
 * np_info_req: - process received N_INFO_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ primitive
 */
static fastcall noinline __unlikely int
np_info_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_INFO_ACK */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_INFO_REQ, err, __FUNCTION__));
}

/**
 * np_bind_req: - process received N_BIND_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_BIND_REQ primitive
 */
static fastcall noinline __unlikely int
np_bind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_Xlisten */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_BIND_REQ, err, __FUNCTION__));
}

/**
 * np_unbind_req: - process received N_UNBIND_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_UNBIND_REQ primitive
 */
static fastcall noinline __unlikely int
np_unbind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_Xcanlis */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_UNBIND_REQ, err, __FUNCTION__));
}

/**
 * np_unitdata_req: - process received N_UNITDATA_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_UNITDATA_REQ primitive
 */
static fastcall noinline __unlikely int
np_unitdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_ERROR_ACK */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_UNITDATA_REQ, err, __FUNCTION__));
}

/**
 * np_optmgmt_req: - process received N_OPTMGMT_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_OPTMGMT_REQ primitive
 */
static fastcall noinline __unlikely int
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_OK_ACK */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_OPTMGMT_REQ, err, __FUNCTION__));
}

/**
 * np_datack_req: - process received N_DATACK_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_DATACK_REQ primitive
 */
static fastcall noinline __unlikely int
np_datack_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_datack_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_DAck or N_EAck */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_DATACK_REQ, err, __FUNCTION__));
}

/**
 * np_reset_req: - process received N_RESET_REQ primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_RESET_REQ primitive
 */
static fastcall noinline __unlikely int
np_reset_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_RI */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_RESET_REQ, err, __FUNCTION__));
}

/**
 * np_reset_res: - process received N_RESET_RES primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the N_RESET_RES primitive
 */
static fastcall noinline __unlikely int
np_reset_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_RC */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, N_RESET_RES, err, __FUNCTION__));
}

/**
 * np_other_req: - process received unknown primitive
 * @np: network provider private structure
 * @q: active queue (upper write queue)
 * @mp: the unknown primitive
 */
static fastcall noinline __unlikely int
np_other_req(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WCON_CREQ);
	/* send N_ERROR_ACK */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_reply(np, q, mp, *p, err, __FUNCTION__));
}

