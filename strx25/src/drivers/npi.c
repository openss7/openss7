/*****************************************************************************

 @(#) $RCSfile: npi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:39 $

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

 Last Modified $Date: 2008-05-07 16:01:39 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi.c,v $
 Revision 0.9.2.1  2008-05-07 16:01:39  brian
 - added NLI X.25-PLP CONS and XX25 implemetnation

 *****************************************************************************/

#ident "@(#) $RCSfile: npi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:39 $"

static char const ident[] = "$RCSfile: npi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:39 $";

/*
 * This is a multiplexing driver that converts between the Network Layer
 * Interface (NPI) and the Network Provider Interface (NPI).  An
 * initialization program opens an NLI stream an links it permanently beneath
 * the multiplexing driver using I_PLINK.  NPI streams are opened by users on
 * the upper multiplex and the driver performs conversion between the
 * per-connection NPI interface on the upper multiplex and the combined stream
 * on the lower multiplex.
 */

#include <sys/os7/compat.h>
#include <sys/npi.h>

#define NPI_DESCRIP	"NLI TO NPI CONVERSION FOR LINUX FAST STREAMS"
#define NPI_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define NPI_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define NPI_REVISION	"OpenSS7 $RCSfile: npi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:39 $"
#define NPI_DEVICE	"SVR 4.2MP NLI to NPI Conversion (NPI) for X.25 CONS"
#define NPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NPI_LICENSE	"GPL"
#define NPI_BANNER	NPI_DESCRIP	"\n" \
			NPI_EXTRA	"\n" \
			NPI_COPYRIGHT	"\n" \
			NPI_REVISION	"\n" \
			NPI_DEVICE	"\n" \
			NPI_CONTACT	"\n"
#define NPI_SPLASH	NPI_DESCRIP	" - " \
			NPI_REVISION	"\n"

#ifdef MODULE
MODULE_AUTHOR(NPI_CONTACT);
MODULE_DESCRIPTION(NPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(NPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(NPI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-npi");
#endif				/* MODULE_ALIAS */

#ifndef CONFIG_STREAMS_NPI_NAME
#error "CONFIG_STREAMS_NPI_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_NPI_MODID
#error "CONFIG_STREAMS_NPI_MODID must be defined."
#endif

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NPI_MODID));
MODULE_ALIAS("streams-driver-npi");
MODULE_ALIAS("streams-module-npi");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NPI_MAJOR));
MODULE_ALIAS("/dev/streams/npi");
MODULE_ALIAS("/dev/streams/npi/*");
MODULE_ALIAS("/dev/streams/clone/npi");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(NPI_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(NPI_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(NPI_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(NPI_CMAJOR_0) "-" __stringify(NPI_CMINOR));
MODULE_ALIAS("/dev/npi");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef NPI_DRV_NAME
#define NPI_DRV_NAME		CONFIG_STREAMS_NPI_NAME
#endif				/* NPI_DRV_NAME */
#ifndef NPI_DRV_ID
#define NPI_DRV_ID		CONFIG_STREAMS_NPI_MODID
#endif				/* NPI_DRV_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define DRV_ID		NPI_DRV_ID
#define DRV_NAME	NPI_DRV_NAME
#define MOD_ID		NPI_MOD_ID
#define CMAJORS		NPI_CMAJORS
#define CMAJOR_0	NPI_CMAJOR_0
#define UNITS		NPI_UNITS
#ifdef MODULE
#define DRV_BANNER	NPI_BANNER
#else				/* MODULE */
#define DRV_BANNER	NPI_SPLASH
#endif				/* MODULE */

static struct module_info np_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat np_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct module_info nl_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat nl_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct np;
struct nl;

/* Upper multiplex structure. */
struct np {
	struct nl *nl;
	queue_t *oq;
	np_ulong state;
	np_ulong oldstate;
	struct {
		N_info_ack_t info;
		unsigned char add[20];
		N_qos_co_opt_sel_t qos;
		N_qos_co_opt_range_t qor;
	} proto;
	unsigned char loc[20];
	unsigned char rem[20];
};

/* Lower multiplex structure. */
struct nl {
	queue_t *oq;
	int state;
	int oldstate;
};

#define NP_PRIV(q) ((struct np *)q->q_ptr)
#define NL_PRIV(q) ((struct nl *)q->q_ptr)

#define STRLOGIO    0		/* input-output controls */
#define STRLOGST    1		/* state transitions */
#define STRLOGTO    2		/* timeouts */
#define STRLOGRX    3		/* primitives received */
#define STRLOGTX    4		/* primitives issued */
#define STRLOGTE    5		/* timer events */
#define STRLOGDA    6		/* data */

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
 * NPI PROVIDER -> NPI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline int
m_error(struct np *np, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wtpr++ = err < 0 ? -err : err;
		*mp->b_wtpr++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_conn_ind: = issued N_CONN_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_conn_ind(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *dst, struct strbuf *src,
	    np_ulong sequence, np_ulong flags, struct strbuf *qos, mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + dst->len + src->len + qos->len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_IND;
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p);
			p->SRC_length = src->len;
			p->SRC_offset = sizeof(*p) + dst->len;
			p->SEQ_number = sequence;
			p->CONN_flags = flags;
			p->QOS_length = qos->len;
			p->QOS_offset = sizeof(*p) + dst->len + src->len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += qos->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_IND");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_conn_con: = issued N_CONN_CON message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_conn_con(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *res, np_ulong flags,
	    struct strbuf *qos, mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + res->len + qos->len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = res->len;
			p->RES_offset = sizeof(*p);
			p->CONN_flags = flags;
			p->QOS_length = qos->len;
			p->QOS_offset = sizeof(*p) + res->len;
			mp->b_wptr += sizeof(*p);
			bcopy(res->buf, mp->b_wptr, res->len);
			mp->b_wptr += res->len;
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += qos->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_CON");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_discon_ind: = issued N_DISCON_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_discon_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong origin, np_ulong reason,
	      struct strbuf *res, np_ulong sequence, mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + res->len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_IND;
			p->DISCON_orig = origin;
			p->DISCON_reason = reason;
			p->RES_length = res->len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = sequence;
			mp->b_wptr += sizeof(*p);
			bcopy(res->buf, mp->b_wptr, res->len);
			mp->b_wptr += res->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DISCON_IND");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_data_ind: = issued N_DATA_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot int
np_data_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_IND;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DATA_IND");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_exdata_ind: = issued N_EXDATA_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot int
np_exdata_ind(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;

	if (bcanputnext(np->oq, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_EXDATA_IND");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_info_ack: = issued N_INFO_ACK message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_info_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(np->proto), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		bcopy(&np->proto, mp->b_wptr, sizeof(np->proto));
		mp->b_wptr += sizeof(np->proto);
		freemsg(msg);
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_INFO_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_bind_ack: = issued N_BIND_ACK message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_bind_ack(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *add, struct strbuf *pro,
	    np_ulong token, np_ulong conind)
{
	N_bind_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + add->len + pro->len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add->len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = conind;
		p->TOKEN_value = token;
		p->PROTOID_length = pro->len;
		p->PROTOID_offset = sizeof(*p) + add->len;
		mp->b_wptr += sizeof(*p);
		bcopy(add->buf, mp->b_wptr, add->len);
		mp->b_wptr += add->len;
		bcopy(pro->buf, mp->b_wptr, pro->len);
		mp->b_wptr += pro->len;
		freemsg(msg);
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_BIND_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_error_ack: = issued N_ERROR_ACK message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_error_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_long error, const char *func)
{
	N_error_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = error < 0 ? NSYSERR : error;
		np->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		if (func != NULL) {
			mi_strlog(np->oq, 0, SL_TRACE, "%s: %s: %s", func, np_primname(prim),
				  np_strerror(error));
		}
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
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
		case NS_WCON_RREQ:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_RRES:
			np_set_state(np, NS_WRES_RIND);
			break;
		case NS_WACK_UREQ:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_OPTREQ:
			np_set_state(np, NS_IDLE);
			break;
		default:
			/* Not typically an error: N_OPTMGMT_REQ does not
			   change the state to NS_WACK_OPTREQ when the original 
			   state was not NS_IDLE. */
			break;
		}
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_ok_ack: = issued N_OK_ACK message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_ok_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim)
{
	N_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_OK_ACK");
		switch (np_get_state(np)) {
		case NS_WACK_CRES:
			np_set_state(np, NS_DATA_XFER);
			break;
		case NS_WACK_DREQ6:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_DREQ7:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_DREQ9:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_DREQ10:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_DREQ11:
			np_set_state(np, NS_IDLE);
			break;
		case NS_WACK_RRES:
			np_set_state(np, NS_DATA_XFER);
			break;
		case NS_WACK_UREQ:
			np_set_state(np, NS_UNBND);
			break;
		case NS_WACK_OPTREQ:
			np_set_state(np, NS_IDLE);
			break;
		default:
			/* Don't change state.  This is not necessarily an
			   error as N_OPTMGMT_REQ when issued in a state other
			   than NS_IDLE does not change the state to
			   NS_WACK_OPTREQ. */
			break;
		}
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_unitdata_ind: = issued N_UNITDATA_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot int
np_unitdata_ind(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *src, struct strbuf *dst,
		np_ulong etype, mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + src->len + dst->len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->SRC_length = src->len;
			p->SRC_offset = sizeof(*p);
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p) + src->len;
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			bcopy(src->ptr, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
			bcopy(dst->ptr, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_UNITDATA_IND");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_uderror_ind: = issued N_UDERROR_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_uderror_ind(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *dst, np_ulong error,
	       mblk_t *dp)
{
	N_uderror_ind_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + dst->len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p);
			p->RESERVED_field = 0;
			p->ERROR_type = error;
			mp->b_wptr += sizeof(*p);
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_UDERROR_IND");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_datack_ind: = issued N_DATACK_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot int
np_datack_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DATACK_IND");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_reset_ind: = issued N_RESET_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_reset_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong origin, np_ulong reason, mblk_t *dp)
{
	N_reset_ind_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = origin;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_IND");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_reset_con: = issued N_RESET_CON message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_reset_con(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_reset_con_t *p;
	mblk_t *mp;

	if (canputnext(np->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_CON");
			putnext(np->oq, mp);
			return (0);
		}
	} else {
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * NLI USER -> NLI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

/*
 * --------------------------------------------------------------------------
 *
 * NPI USER -> NPI PROVIDER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

/**
 * np_conn_req: = process N_CONN_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_CONN_REQ message
 */
static fastcall noinline __unlikely int
np_conn_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p = (typeof(p)) mp->b_rptr;
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
	/* send N_CI */
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
	return (np_error_ack(np, q, mp, N_CONN_REQ, err, __FUNCTION__));
}

/**
 * np_conn_res: = process N_CONN_RES message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_CONN_RES message
 */
static fastcall noinline __unlikely int
np_conn_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	struct np *ap = NULL;
	mblk_t *cp = NULL;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_offset, p->RES_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if ((ap = np_tok_lookup(np, p->TOKEN_value)) == NULL)
		goto badtoken;
	if ((cp = np_seq_lookup(np, p->SEQ_number)) == NULL)
		goto badseq;
	if (np_get_state(np) != NS_WRES_CIND)
		goto outstate;
	np_set_state(np, NS_WACK_CRES);
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
      badtoken:
	err = NBADTOKEN;
	goto error;
      badseq:
	err = NBADSEQ;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_CONN_RES, err, __FUNCTION__));
}

/**
 * np_discon_req: = process N_DISCON_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_DISCON_REQ message
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
	switch (np_get_state(np)) {
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	case NS_WCON_CREQ:
		np_set_state(np, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		np_set_state(np, NS_WACK_DREQ7);
		break;
	case NS_DATA_XFER:
		np_set_state(np, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		np_set_state(np, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		np_set_state(np, NS_WACK_DREQ11);
		break;
	}
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
	return (np_error_ack(np, q, mp, N_DISCON_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_data_req: = process N_DATA_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_DATA_REQ message
 */
static fastcall inline __hot int
np_data_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (p->DATA_xfer_flags & ~(N_MORE_DATA_FLAG | N_RC_FLAG))
		goto badflag;
	switch (np_get_state(np)) {
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	case NS_DATA_XFER:
		break;
	}
	/* send N_Data */
      tooshort:
	err = -EFAULT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np, q, mp, EPROTO));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_exdata_req: = process N_EXDATA_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_EXDATA_REQ message
 */
static fastcall inline __hot int
np_exdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_exdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	case NS_DATA_XFER:
		break;
	}
	/* send N_EData */
      tooshort:
	err = -EFAULT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np, q, mp, EPROTO));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_info_req: = process N_INFO_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ message
 */
static fastcall noinline __unlikely int
np_info_req(struct np *np, queue_t *q, mblk_t *mp)
{
	return (np_info_ack(np, q, mp));
}

/**
 * np_bind_req: = process N_BIND_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_BIND_REQ message
 */
static fastcall noinline __unlikely int
np_bind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	struct strbuf add, pro;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->ADDR_offset, p->ADDR_length))
		goto badaddr;
	add.buf = mp->b_rptr + p->ADDR_offset;
	add.len = p->ADDR_length;
	if (!MBLKIN(mp, p->PROTOID_offset, p->PROTOID_length))
		goto noprotoid;
	pro.buf = mp->b_rptr + p->PROTOID_offset;
	pro.len = p->PROTOID_length;
	if (p->BIND_flags & ~(DEFAULT_LISTENER | TOKEN_REQUEST | DEFAULT_DEST))
		goto badflag;
	if (np_get_state(np) != NS_UNBND)
		goto outstate;
	np_set_state(np, NS_WACK_BREQ);
	/* possibly send N_Xlisten */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      noprotoid:
	err = NNOPROTOID;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_BIND_REQ, err, __FUNCTION__));
}

/**
 * np_unbind_req: = process N_UNBIND_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_UNBIND_REQ message
 */
static fastcall noinline __unlikely int
np_unbind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	int err;

	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WACK_UREQ);
	/* possibly send N_Xcanlis */
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_UNBIND_REQ, err, __FUNCTION__));
}

/**
 * np_unitdata_req: = process N_UNITDATA_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_UNITDATA_REQ message
 */
static fastcall noinline __unlikely int
np_unitdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	struct strbuf dst;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	dst.buf = mp->b_rptr + p->DEST_offset;
	dst.len = p->DEST_length;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	if (np->proto.info.PROVIDER_type != N_CLNS)
		goto notsupport;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      error:
	return (m_error(np, q, mp, EPROTO));
}

/**
 * np_optmgmt_req: = process N_OPTMGMT_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_OPTMGMT_REQ message
 */
static fastcall noinline __unlikely int
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	N_qos_co_opt_sel_t qos;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->OPTMGMT_flags & ~(DEFAULT_RC_SEL))
		goto badflag;
	if (p->QOS_length < sizeof(qos.n_qos_type))
		goto badqostype;
	bcopy(mp->b_rptr + p->QOS_offset, &qos, sizeof(qos.n_qos_type));
	switch (qos.n_qos_type) {
	case N_QOS_CO_OPT_SEL:
		if (p->QOS_length < sizeof(qos))
			goto badopt;
		bcopy(mp->b_rptr + p->QOS_offset, &qos, sizeof(qos));
		/* FIXME: adjust the values */
		break;
		/* Note: we should really have a N_QOS_X25_OPT_SEL type here. */
	default:
		goto badqostype;
	}
	if (np_get_state(np) == NS_IDLE)
		np_set_state(np, NS_WACK_OPTREQ);
	return (np_ok_ack(np, q, mp, N_OPTMGMT_REQ));
      tooshort:
	err = -EFAULT;
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
      badqostype:
	err = NBADQOSTYPE;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_OPTMGMT_REQ, err, __FUNCTION__));
}

/**
 * np_datack_req: = process N_DATACK_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_DATACK_REQ message
 */
static fastcall inline __hot int
np_datack_req(struct np *np, queue_t *q, mblk_t *mp)
{
	int err;

	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_DATA_XFER)
		goto outstate;
	/* possibly send N_DAck */
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np, q, mp, EPROTO));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_reset_req: = process N_RESET_REQ message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_RESET_REQ message
 */
static fastcall noinline __unlikely int
np_reset_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_DATA_XFER)
		goto oustate;
	np_set_state(np, NS_WCON_RREQ);
	/* send N_RI */
      tooshort:
	err = -EFAULT;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_RESET_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_reset_res: = process N_RESET_RES message
 * @np: network provide private structure
 * @q: active queue (upper write queue)
 * @mp: the N_RESET_RES message
 */
static fastcall noinline __unlikely int
np_reset_res(struct np *np, queue_t *q, mblk_t *mp)
{
	int err;

	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_WRES_RIND)
		goto outstate;
	np_set_state(np, NS_WACK_RRES);
	/* send N_RC */
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_RESET_RES, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * NLI PROVIDER -> NLI USER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

static fastcall int
nl_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: M_DATA message on read queue.", __FUNCTION__);
	return (0);
}

static fastcall int
nl_m_proto(queue_t *q, mblk_t *mp)
{
}

static fastcall int
nl_m_sig(queue_t *q, mblk_t *mp)
{
	struct nl *nl;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(int)))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? = EDEADLK : 0);
	if (!mi_timer_valid(mp))
		return (0);
	nl = NL_PRIV(q);
	nl_save_state(nl);
	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = nl_t1_timeout(nl, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d",
			  __FUNCTION__ * (int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		nl_restore_state(nl);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
      passalong:
	/* These are either timer messages (in which case they have a length
	   greater than or equal to 4) or true signal messages (in which case
	   they have a length of 1).  Therefore, if the length of the message
	   is less than sizeof(int) we pass them along.  But, because we are
	   multiplexing driver, discard them. */
	freemsg(mp);
	return (0);
}

static fastcall int
nl_m_copy(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static fastcall int
nl_m_iocack(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * nl_m_setopts: - process M_SETOPTS or M_PCSETOPTS message
 * @q: active queue (lower read queue)
 * @mp: the M_SETOPTS or M_PCSETOPTS message
 *
 * For now we just discard these.  Eventually we should dig into the message
 * and determine what the stream is requesting.  If it is requesting a write
 * offset or write tail, we should record that and potentially adjust the
 * write offset or write tail of our corresponding upper streams by that
 * amount.  The the lower module is requesting adjustment of a watermark or
 * message size, we could adjust those directly.
 */
static fastcall int
nl_m_setopts(queue_t *q, mblk_t *mp)
{
	struct stroptions *so = (typeof(so)) mp->b_rptr;

	if (!MBLKIN(mp, 0, sizeof(*so)))
		goto discard;
	if (so->so_flags & SO_WROFF) {
	}
	if (so->so_flags & SO_WRPAD) {
	}
	if (so->so_flags & SO_SKBUFF) {
		/* allocate socket buffers for data buffer */
	}
	if (so->so_flags & SO_NOSKBUFF) {
		/* don't allocate socket buffers for data buffer */
	}
	if (so->so_flags & SO_DELIM) {
		/* M_DATA messages are delimited */
	}
	if (so->so_flags & SO_NODELIM) {
		/* M_DATA messages are not delimited */
	}
	if (so->so_flags & SO_MREADON) {
		/* send M_READ notification */
	}
	if (so->so_flags & SO_MREADOFF) {
		/* don't send M_READ notification */
	}
	if (so->so_flags & SO_MINPSZ) {
	}
	if (so->so_flags & SO_MAXPSZ) {
	}
	if (so->so_flags & SO_HIWAT) {
	}
	if (so->so_flags & SO_LOWAT) {
	}
      discard:
	freemsg(mp);
	return (0);
}

/**
 * nl_m_flush: - process M_FLUSH message
 * @q: active queue (lower read queue)
 * @mp: the M_FLUSH message
 *
 * Canonical read side multiplexer flush procedure.
 */
static fastcall int
nl_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[1] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~(FLUSHR);
	}
	if (mp->b_wptr[0] & FLUSHW) {
		if (mp->b_rptr[1] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * nl_m_error: - process M_ERROR or M_HANGUP message
 * @q: active queue (lower read queue)
 * @mp: the M_ERROR or M_HANGUP message
 */
static fastcall int
nl_m_error(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static fastcall int
nl_m_unrec(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * nl_msg: - process STREAMS message
 * @q: active queue (lower read queue)
 * @mp: the STREAMS message
 */
static fastcall inline int
nl_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return nl_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return nl_m_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return nl_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return nl_m_sig(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return nl_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return nl_m_iocack(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return nl_m_setopts(q, mp);
	case M_FLUSH:
		return nl_m_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return nl_m_error(q, mp);
	default:
		return nl_m_unrec(q, mp);
	}
}

static int
np_m_data(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);

	return (m_error(np, q, mp, EPROTO));
}

static int
np_m_proto(queue_t *q, mblk_t *mp)
{
	np_ulong prim;
	struct np *np;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto tooshort;
	prim = *(np_ulong *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	np = NP_PRIV(q);
	np_save_state(np);
	switch (prim) {
	case N_CONN_REQ:
		rtn = np_conn_req(np, q, mp);
		break;
	case N_CONN_RES:
		rtn = np_conn_res(np, q, mp);
		break;
	case N_DISCON_REQ:
		rtn = np_discon_req(np, q, mp);
		break;
	case N_DATA_REQ:
		rtn = np_data_req(np, q, mp);
		break;
	case N_EXDATA_REQ:
		rtn = np_exdata_req(np, q, mp);
		break;
	case N_INFO_REQ:
		rtn = np_info_req(np, q, mp);
		break;
	case N_BIND_REQ:
		rtn = np_bind_req(np, q, mp);
		break;
	case N_UNBIND_REQ:
		rtn = np_unbind_req(np, q, mp);
		break;
	case N_UNITDATA_REQ:
		rtn = np_unitdata_req(np, q, mp);
		break;
	case N_OPTMGMT_REQ:
		rtn = np_optmgmt_req(np, q, mp);
		break;
	case N_DATACK_REQ:
		rtn = np_datack_req(np, q, mp);
		break;
	case N_RESET_REQ:
		rtn = np_reset_req(np, q, mp);
		break;
	case N_RESET_RES:
		rtn = np_reset_res(np, q, mp);
		break;
/*
 * None of the following messages should appear at the upper write queue.
 * Only downward defined NPI messages should appear at the upper write queue.
 * Discard these.
 */
	case N_CONN_IND:
	case N_CONN_CON:
	case N_DISCON_IND:
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_INFO_ACK:
	case N_BIND_ACK:
	case N_ERROR_ACK:
	case N_OK_ACK:
	case N_UNITDATA_IND:
	case N_UDERROR_IND:
	case N_DATACK_IND:
	case N_RESET_IND:
	case N_RESET_CON:
		goto wrongway;
	default:
		rtn = np_other_req(np, q, mp);
		break;
	}
	if (rtn) {
		np_restore_state(np);
	}
	mi_unlock(priv);
	return (rtn);
      edeadlk:
	return (-EDEADLK);
      wrongway:
	mi_unlock(priv);
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding message in wrong direction.",
		  __FUNCTION__);
	return (0);
      tooshort:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short", __FUNCTION__);
	freemsg(mp);
	return (0);
}

static int
np_m_sig(queue_t *q, mblk_t *mp)
{
	struct np *np;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(int)))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	if (!mi_timer_valid(mp))
		goto done;
	np = NP_PRIV(q);
	np_save_state(q);
	switch (*(int *) mp->b_rptr) {
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = np_t1_timeout(np, q, mp);
		break;
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		freemsg(mp);
		rtn = 0;
		break;
	}
	if (rtn) {
		np_restore_state(np);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
      edeadlk:
	if (mi_timer_requeue(mp))
		return (-EDEADLK);
      done:
	return (0);
      passalong:
	freemsg(mp);
	return (0);
}

static fastcall int
np_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct np *np;
	caddr_t *priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != NPI_IOC_MAGIC)
		goto notforus;
	if (!mp->b_cont)
		goto efault;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	np = NP_PRIV(q);
	err = np_ioctl(np, q, mp);
	mi_unlock(priv);
	return (err);
      edeadlk:
	return (-EDEADLK);
      efault:
	mi_copy_done(q, mp, EFAULT);
	return (0);
      notforus:
	if (np->nl)
		putnext(np->nl->oq, mp);
	else
		freemsg(mp);
	return (0);
}

static fastcall int
np_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct np *np;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != NPI_IOC_MAGIC)
		goto notforus;
	if (!mp->b_cont)
		goto efault;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	np = NP_PRIV(q);
	switch (mi_copy_state(q, mp, &dp)) {
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = np_copyin1(np, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		err = np_copyin2(np, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = np_copyout(np, q, mp, dp);
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		err = 0;
		break;
	}
	mi_unlock(priv);
	return (err);
      edeadlk:
	return (-EDEADLK);
}

/**
 * np_m_flush: process M_FLUSH message
 * @q: active queue (upper write queue)
 * @mp: the M_FLUSH message
 *
 * Cannonical write side flush procedure for drivers.
 */
static fastcall int
np_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~(FLUSHW);
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static fastcall int
np_m_unrec(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);

	return (m_error(np, q, mp, EPROTO));
}

static fastcall inline int
np_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return np_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return np_m_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return np_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return np_m_sig(q, mp);
	case M_IOCTL:
		return np_m_ioctl(q, mp);
	case M_IOCDATA:
		return np_m_iocdata(q, mp);
	case M_FLUSH:
		return np_m_flush(q, mp);
	default:
		return np_m_unrec(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * QUEUE PUT AND SERVICE PROCEDURES
 *
 * --------------------------------------------------------------------------
 */

static streamscall __hot_write int
nl_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || nl_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
nl_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (nl_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_read int
np_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (np_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_in int
np_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || np_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * QUEUE OPEN AND CLOSE PROCEDURES
 *
 * --------------------------------------------------------------------------
 */

static caddr_t np_opens = NULL;
static caddr_t np_links = NULL;

static streamscall int
np_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct np *np;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((err = mi_open_comm(&np_opens, sizeof(*np), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	np = NP_PRIV(q);

	/* intiialize private structure */
	np->oq = q;
	np->state = NS_UNBND;
	np->oldstate = NS_UNBND;

	np->proto.info.PRIM_type = N_INFO_ACK;
	np->proto.info.NSDU_size = 128;
	np->proto.info.CDATA_size = N_UNDEFINED;
	np->proto.info.DDATA_size = N_UNDEFINED;
	np->proto.info.ADDR_size = 0;
	np->proto.info.ADDR_length - 0;
	np->proto.info.ADDR_offset = sizeof(np->proto.info);
	np->proto.info.QOS_length = sizeof(np->qos);
	np->proto.info.QOS_offset = sizeof(np->proto.info) + sizeof(np->add);
	np->proto.info.QOS_range_length = sizeof(np->qor);
	np->proto.info.QOS_range_offset =
	    sizeof(np->proto.info) + sizeof(np->add) + sizeof(np->qos);
	np->proto.info.OPTIONS_flags = 0;
	np->proto.info.NIDU_size = 4096;
	np->proto.info.SERV_type = N_CONS;
	np->proto.info.CURRENT_state = NS_UNBND;
	np->proto.info.PROVIDER_type = N_SUBNET;
	np->proto.info.NODU_size = 128;
	np->proto.info.PROTOID_length = 0;
	np->proto.info.PROTOID_offset = 0;
	np->proto.info.NPI_version = N_CURRENT_VERSION;

	np->proto.add[0] = 35;	/* AFI 35 */
	np->proto.add[1] = 0x00;	/* ICP 1 */
	np->proto.add[2] = 0x01;
	np->proto.add[3] = 0x00;	/* IPv4 address */
	np->proto.add[4] = 0x00;
	np->proto.add[5] = 0x00;
	np->proto.add[6] = 0x00;
	np->proto.add[7] = 0x00;	/* IPv4 port number */
	np->proto.add[8] = 0x00;

	np->proto.qos.n_qos_type = N_QOS_CO_OPT_SEL;
	np->proto.qos.src_throughput.thru_targ_value = QOS_UNKNOWN;
	np->proto.qos.src_throughput.thru_min_value = QOS_UNKNOWN;
	np->proto.qos.dest_throughput.thru_targ_value = QOS_UNKNOWN;
	np->proto.qos.dest_throughput.thru_min_value = QOS_UNKNOWN;
	np->proto.qos.transit_delay_t.td_targ_value = QOS_UNKNOWN;
	np->proto.qos.transit_delay_t.td_max_value = QOS_UNKNOWN;
	np->proto.qos.nc_estab_delay = QOS_UNKNOWN;
	np->proto.qos.nc_estab_fail_prob = QOS_UNKNOWN;
	np->proto.qos.residual_error_rate = QOS_UNKNOWN;
	np->proto.qos.xfer_fail_prob = QOS_UNKNOWN;
	np->proto.qos.nc_resilience = QOS_UNKNOWN;
	np->proto.qos.nc_rel_delay = QOS_UNKNOWN;
	np->proto.qos.nc_rel_fail_prob = QOS_UNKNOWN;
	np->proto.qos.protection_sel = QOS_UNKNOWN;
	np->proto.qos.priority_sel = QOS_UNKNOWN;
	np->proto.qos.max_accept_cost = QOS_UNKNOWN;

	np->proto.qor.n_qos_type = N_QOS_CO_OPT_RANGE;
	np->proto.qor.src_throughput.thru_targ_Value = QOS_UNKNOWN;
	np->proto.qor.src_throughput.thru_min_value = QOS_UNKNOWN;
	np->proto.qor.dest_throughput.thru_targ_Value = QOS_UNKNOWN;
	np->proto.qor.dest_throughput.thru_min_value = QOS_UNKNOWN;
	np->proto.qor.transit_delay_t.td_targ_value = QOS_UNKNOWN;
	np->proto.qor.transit_delay_t.td_max_value = QOS_UNKNOWN;
	np->proto.qor.nc_estab_delay = QOS_UNKNOWN;
	np->proto.qor.nc_estab_fail_prob = QOS_UNKNOWN;
	np->proto.qor.residual_error_rate = QOS_UNKNOWN;
	np->proto.qor.xfer_fail_prob = QOS_UNKNOWN;
	np->proto.qor.nc_resilience = QOS_UNKNOWN;
	np->proto.qor.nc_rel_delay = QOS_UNKNOWN;
	np->proto.qor.nc_rel_fail_prob = QOS_UNKNOWN;
	np->proto.qor.protection_range.protection_targ_value = QOS_UNKNOWN;
	np->proto.qor.protection_range.protection_min_value = QOS_UNKNOWN;
	np->proto.qor.priority_range.priority_targ_value = QOS_UNKNOWN;
	np->proto.qor.priority_range.priority_min_value = QOS_UNKNOWN;
	np->proto.qor.max_accept_cost = QOS_UNKNOWN;

	qprocson(q);
	return (0);
}

static streamscall int
np_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm(&np_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit np_winit = {
	.qi_putp = np_put,
	.qi_srvp = np_srv,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_mstat,
};

static struct qinit np_rinit = {
	.qi_putp = NULL,
	.qi_qopen = &np_qopen,
	.qi_qclose = &np_qclose,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_mstat,
};

static struct qinit nl_winit = {
	.qi_putp = NULL,
	.qi_minfo = &nl_minfo,
	.qi_mstat = &nl_mstat,
};

static struct qinit nl_rinit = {
	.qi_putp = nl_put,
	.qi_srvp = nl_srv,
	.qi_minfo = &nl_minfo,
	.qi_mstat = &nl_mstat,
};

struct streamtab npi_info = {
	.st_rdinit = &np_rinit,
	.st_wrinit = &np_winit,
	.st_muxrinit = &nl_rinit,
	.st_muxwinit = &nl_winit,
};

static modID_t modid = MOD_ID;
static major_t major = CMAJOR;

/*
 * --------------------------------------------------------------------------
 *
 * LINUX KERNEL MODULE INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
module_param(major, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for NPI. (0 for allocation.)");
MODULE_PARM_DESC(major, "Major device number for NPI. (0 for allocation.)");

/*
 * LINUX FAST STREAMS REGISTRATION
 */

#ifdef LFS

static struct cdevsw np_cdev = {
	.d_str = &npi_info,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static int
npi_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&npi_cdev, major)) < 0)
		return (err);
	return (0);
}

static int
npi_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&npi_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

#ifdef LIS

static int
npi_regsiter_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &npi_info, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

static int
npi_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

static __init int
npi_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = npi_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver major %d", DRV_NAME, (int) major);
		return (err);
	}
	return (0);
}

static __exit void
npi_modexit(void)
{
	int err;

	if ((err = npi_unregister_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", DRV_NAME, err);
		return;
	}
	return;
}

module_init(npi_modinit);
module_exit(npi_modexit);

#endif				/* LINUX */
