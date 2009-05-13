/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 * This is a pushable STREAMS module that converts between the SCCPI (Signalling Connection Control
 * Part Interface) and NPI (Network Provider Interface).  SCCPI is, in fact, an extension of the
 * NPI interface: it provides extension primitives in support of ITU-T Recommendation Q.711
 * primitives that are not provided in ITU-T Recommendation X.213 and thus not reflected in the NPI
 * Revision 2.0.0 interface.  The NPI Revision 2.0.0 interface was largely developed in accordance
 * with X.213 in support of OSI CONS (X.25) and CLNS (CLNP).  This conversion module enhances the
 * NPI Revision 2.0.0 interface in a way that it can transparently support SCCP to NPI applications
 * and libraries written to support CONS (X.25) and CLNS (CLNP).
 */

#define _MPS_SOURCE
#define _SVR4_SOURCE

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#include <sys/npi.h>
#include <sys/npi_sccp.h>
#include <ss7/sccpi.h>

#define n_tst_bit(nr,addr)  test_bit(nr,addr)
#define n_set_bit(nr,addr)  __set_bit(nr,addr)
#define n_clr_bit(nr,addr) __clear_bit(nr,addr)

#define NPI_SCCP_DESCRIP	"SCCPI to NPI CONVERSION MODULE FOR LINUX FAST-STREAMS"
#define NPI_SCCP_EXTRA		"Part of the OpenSS7 SS7 Stack for Linux Fast-STREAMS"
#define NPI_SCCP_COPYRIGHT	"Copyright (c) 2008-2009  Monavacon Limited.  All Rights Reserved."
#define NPI_SCCP_REVISION	"OpenSS7 $RCSfile: npi_sccp.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2009-01-16 20:44:51 $"
#define NPI_SCCP_DEVICE		"SVR 4.2MP SCCPI to NPI Conversion Module (NPI) for SCCP"
#define NPI_SCCP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NPI_SCCP_LICENSE	"GPL"
#define NPI_SCCP_BANNER		NPI_SCCP_DESCRIP	"\n" \
				NPI_SCCP_EXTRA		"\n" \
				NPI_SCCP_COPYRIGHT	"\n" \
				NPI_SCCP_REVISION	"\n" \
				NPI_SCCP_DEVICE		"\n" \
				NPI_SCCP_CONTACT	"\n"
#define NPI_SCCP_SPLASH		NPI_SCCP_DESCRIP	" - " \
				NPI_SCCP_REVISION

#ifndef CONFIG_STREAMS_NPI_SCCP_NAME
#error CONFIG_STREAMS_NPI_SCCP_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_NPI_SCCP_MODID
#error CONFIG_STREAMS_NPI_SCCP_MODID must be defined.
#endif

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(NPI_SCCP_CONTACT);
MODULE_DESCRIPTION(NPI_SCCP_DESCRIP);
MODULE_SUPPORTED_DEVICE(NPI_SCCP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(NPI_SCCP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-npi-sccp");
MODULE_ALIAS("streams-module-npi-sccp");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NPI_SCCP_MODID));
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef NPI_SCCP_MOD_NAME
#define NPI_SCCP_MOD_NAME	CONFIG_STREAMS_NPI_SCCP_NAME
#endif				/* NPI_SCCP_MOD_NAME */
#ifndef NPI_SCCP_MOD_ID
#define NPI_SCCP_MOD_ID		CONFIG_STREAMS_NPI_SCCP_MODID
#endif				/* NPI_SCCP_MOD_ID */

#define STRLOGERR	0	/* log error information */
#define STRLOGNO	0	/* log notice information */
#define STRLOGST	1	/* log state transitions */
#define STRLOGTO	2	/* log timeouts */
#define STRLOGRX	3	/* log primitives received */
#define STRLOGTX	4	/* log primitives issued */
#define STRLOGTE	5	/* log timer events */
#define STRLOGIO	6	/* log additional data */
#define STRLOGDA	7	/* log data */

/*
 * STREAMS DEFINITIONS
 */

#define MOD_ID		NPI_SCCP_MOD_ID
#define MOD_NAME	NPI_SCCP_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	NPI_SCCP_BANNER
#else				/* MODULE */
#define MOD_BANNER	NPI_SCCP_SPLASH
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

static streamscall int np_rput(queue_t *q, mblk_t *mp);
static streamscall int np_rsrv(queue_t *q);

static streamscall int np_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp);
static streamscall int np_qclose(queue_t *q, int oflags, cred_t *credp);

static struct qinit np_rinit = {
	.qi_putp = np_rput,
	.qi_srvp = np_rsrv,
	.qi_qopen = np_qopen,
	.qi_qclose = np_qclose,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_rstat,
};

static streamscall int np_wput(queue_t *q, mblk_t *mp);
static streamscall int np_wsrv(queue_t *q);

static struct qinit np_winit = {
	.qi_putp = np_wput,
	.qi_srvp = np_wsrv,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_wstat,
};

static struct streamtab npi_sccpinfo = {
	.st_rdinit = &np_rinit,
	.st_wrinit = &np_winit,
};

/*
 * OPTIONS HANDLING
 */

/*
 * PRIVATE STRUCTURE
 */

struct np;
struct sc;

/* Upper interface structure */
struct np {
	struct sc *sc;
	queue_t *oq;
	np_ulong maxinds;
	struct {
		np_ulong state;
		np_ulong datinds;
		np_ulong datacks;
		np_ulong edatack;
		np_ulong coninds;
		np_ulong infinds;
		np_ulong accept;
		np_ulong resinds;
		np_ulong resacks;
		np_ulong pending;
	} state, oldstate;
	struct {
		N_info_ack_t info;
		struct sccp_addr add;
		N_qos_sel_info_sccp_t qos;
		N_qos_range_info_sccp_t qor;
	} proto;
	np_ulong TOKEN_value;
	np_ulong BIND_flags;
	np_ulong SRC_lref;
	np_ulong QOS_length;
	N_qos_sccp_t qos;
	np_ulong QOS_range_length;
	N_qos_sccp_t qor;
	np_ulong ADDR_length;
	struct sccp_addr add;
	np_ulong LOCADDR_length;
	struct sccp_addr loc;
	np_ulong REMADDR_length;
	struct sccp_addr rem;
	np_ulong RES_length;
	struct sccp_addr res;
	np_ulong PROTOID_length;
	np_ulong pro[4];
};

/* Lower interface structure */
struct sc {
	struct np *np;
	queue_t *oq;
	np_ulong maxinds;
	struct {
		np_ulong state;
		np_ulong datinds;
		np_ulong datacks;
		np_ulong edatack;
		np_ulong coninds;
		np_ulong infinds;
		np_ulong accept;
		np_ulong resinds;
		np_ulong resacks;
		np_ulong pending;
	} state, oldstate;
	struct {
		N_info_ack_t info;
		struct sccp_addr add;
		N_qos_sel_info_sccp_t qos;
		N_qos_range_info_sccp_t qor;
	} proto;
	np_ulong TOKEN_value;
	np_ulong BIND_flags;
	np_ulong SRC_lref;
	np_ulong QOS_length;
	N_qos_sccp_t qos;
	np_ulong QOS_range_length;
	N_qos_sccp_t qor;
	np_ulong ADDR_length;
	struct sccp_addr add;
	np_ulong LOCADDR_length;
	struct sccp_addr loc;
	np_ulong REMADDR_length;
	struct sccp_addr rem;
	np_ulong RES_length;
	struct sccp_addr res;
	np_ulong PROTOID_length;
	np_ulong pro[4];
};

struct priv {
	struct np np;
	struct sc sc;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define NP_PRIV(q) (&PRIV(q)->np)
#define SC_PRIV(q) (&PRIV(q)->sc)

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
#ifdef N_EXT_BIND_REQ
	case N_EXT_BIND_REQ:
		return ("N_EXT_BIND_REQ");
#endif
#ifdef N_EXT2_BIND_REQ
	case N_EXT2_BIND_REQ:
		return ("N_EXT2_BIND_REQ");
#endif
#ifdef N_DRAIN_REQ
	case N_DRAIN_REQ:
		return ("N_DRAIN_REQ");
#endif
#ifdef N_FLOW_REQ
	case N_FLOW_REQ:
		return ("N_FLOW_REQ");
#endif
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

static inline const char *
np_strerror(np_long error)
{
	if (error < 0)
		error = NSYSERR;
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
np_get_state(struct np *np)
{
	return (np->state.state);
}

static np_ulong
np_set_state(struct np *np, np_ulong newstate)
{
	np_ulong oldstate = np->state.state;

	if (newstate != oldstate) {
		np->state.state = newstate;
		np->proto.info.CURRENT_state = newstate;
		mi_strlog(np->oq, STRLOGST, SL_TRACE, "%s <- %s", np_statename(newstate),
			  np_statename(oldstate));
	}
	return (newstate);
}

static np_ulong
np_save_state(struct np *np)
{
	np->oldstate.state = np->state.state;
	return ((np->oldstate.state = np_get_state(np)));
}

static np_ulong
np_restore_state(struct np *np)
{
	return (np_set_state(np, np->oldstate.state));
}

static const char *
sc_primname(np_ulong prim)
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
	case N_RESET_REQ:
		return ("N_RESET_REQ");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_RESET_CON:
		return ("N_RESET_CON");
	case N_INFORM_REQ:
		return ("N_INFORM_REQ");
	case N_INFORM_IND:
		return ("N_INFORM_IND");
	case N_NOTICE_IND:
		return ("N_NOTICE_IND");
	case N_STATE_REQ:
		return ("N_STATE_REQ");
	case N_STATE_IND:
		return ("N_STATE_IND");
	case N_PCSTATE_IND:
		return ("N_PCSTATE_IND");
	case N_TRAFFIC_IND:
		return ("N_TRAFFIC_IND");
	case N_COORD_REQ:
		return ("N_COORD_REQ");
	case N_COORD_IND:
		return ("N_COORD_IND");
	case N_COORD_RES:
		return ("N_COORD_RES");
	case N_COORD_CON:
		return ("N_COORD_CON");
	case N_REQUEST_REQ:
		return ("N_REQUEST_REQ");
	case N_REPLY_ACK:
		return ("N_REPLY_ACK");
	default:
		return ("(unknown)");
	}
}

static const char *
sc_statename(np_ulong state)
{
	return np_statename(state);
}

static inline const char *
sc_strerror(np_long error)
{
	return np_strerror(error);
}

static np_ulong
sc_get_state(struct sc *sc)
{
	return (sc->state.state);
}

static np_ulong
sc_set_state(struct sc *sc, np_ulong newstate)
{
	np_ulong oldstate = sc->state.state;

	if (newstate != oldstate) {
		sc->state.state = newstate;
		sc->proto.info.CURRENT_state = newstate;
		mi_strlog(sc->oq, STRLOGST, SL_TRACE, "%s <- %s", sc_statename(newstate),
			  sc_statename(oldstate));
	}
	return (newstate);
}

static np_ulong
sc_save_state(struct sc *sc)
{
	return ((sc->oldstate.state = sc_get_state(sc)));
}

static np_ulong
sc_restore_state(struct sc *sc)
{
	return (sc_set_state(sc, sc->oldstate.state));
}

/*
 * NPI PROVIDER TO NPI USER ISSUED PRIMITIVES
 */

/**
 * n_conn_ind: - issue an N_CONN_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @loc: destination address
 * @rem: source address
 * @qos: quality of service parameters
 * @seq: connection indication sequence number
 * @flags: connect flags
 * @dp: user data
 */
noinline fastcall int
n_conn_ind(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *loc,
	   struct sccp_addr *rem, N_qos_sel_conn_sccp_t *qos, np_ulong seq, np_ulong flags,
	   mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(np->rem) + sizeof(np->loc) + sizeof(np->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;

	/* save source address for later */
	if (rem) {
		np->rem = *rem;
		np->REMADDR_length = sizeof(*rem);
	}
	/* save destination address for later */
	if (loc) {
		np->loc = *loc;
		np->LOCADDR_length = sizeof(*loc);
	}

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_IND;
	p->DEST_length = np->LOCADDR_length;
	p->DEST_offset = np->LOCADDR_length ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	p->CONN_flags = flags;
	p->SRC_length = np->REMADDR_length;
	p->SRC_offset = np->REMADDR_length ? p->DEST_offset + p->DEST_length: 0;
	p->QOS_length = np->QOS_length;
	p->QOS_offset = np->QOS_length ? p->SRC_offset + p->SRC_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&np->loc, mp->b_wptr, p->DEST_length);
	mp->b_wptr += sizeof(np->loc);
	bcopy(&np->rem, mp->b_wptr, p->SRC_length);
	mp->b_wptr += sizeof(np->rem);
	bcopy(&np->qos, mp->b_wptr, sizeof(np->qos));
	mp->b_wptr += sizeof(np->qos);
	mp->b_cont = dp;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_IND");
	np_set_state(np, NS_WRES_CIND);
	np->state.coninds++;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_conn_con: - issue an N_CONN_CON primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: responding address
 * @qos: quality of service parameters
 * @flags: connection flags
 * @dp: user data
 */
noinline fastcall int
n_conn_con(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *res, N_qos_sel_conn_sccp_t *qos, np_ulong flags,
	   mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(np->res) + sizeof(np->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	/* save responding address for later */
	if (res) {
		np->res = *res;
		np->RES_length = sizeof(*res);
	}

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = np->RES_length;
	p->RES_offset = np->RES_length ? sizeof(*p) : 0;
	p->CONN_flags = flags;
	p->QOS_length = qos ? sizeof(*qos) : 0;
	p->QOS_offset = qos ? p->RES_offset + p->RES_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&np->res, mp->b_wptr, sizeof(np->res));
	mp->b_wptr += sizeof(np->res);
	bcopy(&np->qos, mp->b_wptr, sizeof(np->qos));
	mp->b_wptr += sizeof(np->qos);
	mp->b_cont = dp;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_CON");
	np_set_state(np, NS_DATA_XFER);
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_discon_ind: - issue an N_DISCON_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: responding address
 * @orig: disconnect originator
 * @reason: disconnect reason
 * @seq: connection indication sequence number (or zero)
 * @dp: user data
 */
noinline fastcall int
n_discon_ind(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *res, np_ulong orig, np_ulong reason, np_ulong seq,
	     mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(np->res);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	/* save responding address for later */
	if (res) {
		np->res = *res;
		np->RES_length = sizeof(*res);
	}

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = orig;
	p->DISCON_reason = reason;
	p->RES_length = np->RES_length;
	p->RES_offset = np->RES_length ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	bcopy(&np->res, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	mp->b_cont = dp;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DISCON_IND");
	if (seq)
		np->state.coninds--;
	np_set_state(np, np->state.coninds ? NS_WRES_CIND : NS_IDLE);
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_data_ind: - issue an N_DATA_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: data transfer flags
 * @dp: user data
 */
noinline fastcall int
n_data_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_IND;
	p->DATA_xfer_flags = flags;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DATA_IND");
	if (flags & N_RC_FLAG)
		np->state.datinds++;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_exdata_ind: - issue an N_EXDATA_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: user data
 */
noinline fastcall int
n_exdata_ind(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 1)))
		goto ebusy;
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_EXDATA_IND");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_info_ack: - issue an N_INFO_ACK primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
n_info_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;
	static const size_t mlen =
	    sizeof(*p) + sizeof(np->loc) + sizeof(np->qos) + sizeof(np->qor) + sizeof(np->pro);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	*p = np->proto.info;
	mp->b_wptr += sizeof(*p);
	p->ADDR_offset = MBLKL(mp);
	bcopy(&np->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	p->QOS_offset = MBLKL(mp);
	bcopy(&np->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	p->QOS_range_offset = MBLKL(mp);
	bcopy(&np->qor, mp->b_wptr, p->QOS_range_length);
	mp->b_wptr += p->QOS_range_length;
	p->PROTOID_offset = MBLKL(mp);
	bcopy(&np->pro, mp->b_wptr, p->PROTOID_length);
	mp->b_wptr += p->PROTOID_length;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_INFO_ACK");
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_bind_ack: - issue an N_BIND_ACK primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
n_bind_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_bind_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(np->add) + sizeof(np->pro);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = sizeof(np->add);
	p->ADDR_offset = sizeof(*p);
	p->CONIND_number = np->maxinds;
	p->PROTOID_length = np->proto.info.PROTOID_length;
	p->PROTOID_offset = p->ADDR_offset + p->ADDR_length;
	mp->b_wptr += sizeof(*p);
	bcopy(&np->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	bcopy(&np->pro, mp->b_wptr, p->PROTOID_length);
	mp->b_wptr += p->PROTOID_length;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_BIND_ACK");
	np_set_state(np, NS_IDLE);
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_error_ack: - issue an N_ERROR_ACK primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative UNIX error, positive NPI error
 */
noinline fastcall int
n_error_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_ulong error)
{
	N_error_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	np_ulong state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = prim;
	p->NPI_error = error < 0 ? NSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
	np_restore_state(np);
	switch ((state = np_get_state(np))) {
	case NS_WACK_BREQ:
		state = NS_UNBND;
		break;
	case NS_WACK_UREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_OPTREQ:
		state = NS_IDLE;
		break;
	case NS_WCON_CREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_CRES:
		state = NS_WRES_CIND;
		break;
	case NS_WCON_RREQ:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_RRES:
		state = NS_WRES_RIND;
		break;
	case NS_WACK_DREQ6:
		state = NS_WCON_CREQ;
		break;
	case NS_WACK_DREQ7:
		state = NS_WRES_CIND;
		break;
	case NS_WACK_DREQ9:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_DREQ10:
		state = NS_WCON_RREQ;
		break;
	case NS_WACK_DREQ11:
		state = NS_WRES_RIND;
		break;
	}
	np_set_state(np, state);
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_ok_ack: - issue an N_OK_ACK primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct prim
 */
noinline fastcall int
n_ok_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim)
{
	N_ok_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	np_ulong state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch ((state = np_get_state(np))) {
	case NS_WACK_BREQ:
	case NS_WACK_OPTREQ:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		state = NS_IDLE;
		break;
	case NS_WACK_UREQ:
		state = NS_UNBND;
		break;
	case NS_WACK_RRES:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_CRES:
		np->state.coninds--;
		/* FIXME: when accept is non-zero, need to find the other Stream and set its state
		   to NS_DATA_XFER. */
		if (np->state.accept == 0)
			state = NS_DATA_XFER;
		else if (np->state.coninds > 0)
			state = NS_WRES_CIND;
		else
			state = NS_IDLE;
		break;
	}
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_OK_ACK");
	np_set_state(np, state);
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_unitdata_ind: - issue an N_UNITDATA_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @rem: source address
 * @loc: destination address
 * @qos: quality of service parameters
 * @error: error type
 * @dp: user data
 */
noinline fastcall int
n_unitdata_ind(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *rem,
	       struct sccp_addr *loc, N_qos_sel_data_sccp_t *qos, np_ulong error,
	       mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*loc) + sizeof(*rem);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = loc ? sizeof(*loc) : 0;
	p->SRC_offset = loc ? sizeof(*p) : 0;
	p->DEST_length = rem ? sizeof(*rem) : 0;
	p->DEST_offset = rem ? p->SRC_offset + p->SRC_length : 0;
	p->ERROR_type = error;
	mp->b_wptr += sizeof(*p);
	bcopy(loc, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	bcopy(rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	mp->b_cont = dp;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_UNITDATA_IND");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_uderror_ind: - issue an N_UDERROR_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @rem: destination address (or NULL)
 * @qos: quality of service parameters
 * @error: error type
 * @dp: user data
 */
noinline fastcall int
n_uderror_ind(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *rem,
	      N_qos_sel_data_sccp_t *qos, np_ulong error, mblk_t *dp)
{
	N_uderror_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*rem);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 1)))
		goto ebusy;
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UDERROR_IND;
	p->DEST_length = rem ? sizeof(*rem) : 0;
	p->DEST_offset = rem ? sizeof(*p) : 0;
	p->RESERVED_field = 0;
	p->ERROR_type = error;
	mp->b_wptr += sizeof(*p);
	bcopy(rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	mp->b_cont = dp;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_UDERROR_IND");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_datack_ind: - issue an N_DATACK_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
n_datack_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_IND;
	mp->b_wptr += sizeof(*p);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DATACK_IND");
	np->state.datacks--;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_inform_ind: - issue an N_RESET_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @reason: inform reason
 */
noinline fastcall int
n_inform_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason)
{
	N_reset_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 2)))
		goto ebusy;
	mp->b_band = 2;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = orig;
	p->RESET_reason = reason;
	mp->b_wptr += sizeof(*p);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_IND(INFORM)");
	np_set_state(np, NS_WRES_RIND);
	np->state.resinds++;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_reset_ind: - issue an N_RESET_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @orig: reset originator
 * @reason: reset reason
 */
noinline fastcall int
n_reset_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason)
{
	N_reset_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = orig;
	p->RESET_reason = reason;
	mp->b_wptr += sizeof(*p);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_IND");
	np_set_state(np, NS_WRES_RIND);
	np->state.datinds = 0;
	np->state.datacks = 0;
	np->state.resinds = 1;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_reset_con: - issue an N_RESET_CON primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
n_reset_con(struct np *np, queue_t *q, mblk_t *msg)
{
	N_reset_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_CON;
	mp->b_wptr += sizeof(*p);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_CON");
	np_set_state(np, NS_IDLE);
	np->state.resacks = 0;
	np->state.resinds = 0;
	np->state.datinds = 0;
	np->state.datacks = 0;
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 * SCCPI USER TO SCCPI PROVIDER ISSUED PRIMITIVES
 */

/**
 * n_conn_req: - issue an N_CONN_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @rem: destination address (not NULL)
 * @qos: quality of service parameters
 * @flags: connect flags
 * @dp: user data
 */
noinline fastcall int
n_conn_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *rem,
	   N_qos_sel_conn_sccp_t *qos, np_ulong flags, mblk_t *dp)
{
	N_conn_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->rem) + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;

	/* save destination address for later */
	if (rem) {
		sc->rem = *rem;
		sc->REMADDR_length = sizeof(*rem);
	}
	/* save options for later */
	if (qos) {
		bcopy(qos, &sc->qos, sizeof(*qos));
		sc->QOS_length = sizeof(*qos);
	}

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_REQ;
	p->DEST_length = sc->REMADDR_length;
	p->DEST_offset = sc->REMADDR_length ? sizeof(*p) : 0;
	p->CONN_flags = flags;
	p->QOS_length = sc->QOS_length;
	p->QOS_offset = sc->QOS_length ? p->DEST_offset + p->DEST_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	mp->b_cont = dp;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_CONN_REQ ->");
	sc_set_state(sc, NS_WCON_CREQ);
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_conn_res: - issue an N_CONN_RES primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @res: responding address (or NULL)
 * @qos: quality of service parameters
 * @token: token of accepting stream or zero
 * @flags: connect flags
 * @seq: connection indication responded to
 */
noinline fastcall int
n_conn_res(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *res,
	   N_qos_sel_conn_sccp_t *qos, np_ulong token, np_ulong flags, np_ulong seq,
	   mblk_t *dp)
{
	N_conn_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->res) + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;

	/* save responding address for later */
	if (res) {
		sc->res = *res;
		sc->RES_length = sizeof(*res);
	}
	/* save quality of service parameters */
	if (qos) {
		bcopy(qos, &sc->qos, sizeof(*qos));
		sc->QOS_length = sizeof(*qos);
	}

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_RES;
	p->TOKEN_value = token;
	p->RES_length = sc->RES_length;
	p->RES_offset = sc->RES_length ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	p->CONN_flags = flags;
	p->QOS_length = sizeof(sc->qos);
	p->QOS_offset = p->RES_offset + p->RES_length;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->res, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	mp->b_cont = dp;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_CONN_RES ->");
	sc_set_state(sc, NS_WACK_CRES);
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	sc->state.coninds--;
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_discon_req: - issue an N_DISCON_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @res: responding address (or NULL)
 * @reason: disconnect reason
 * @seq: connection indication sequence number (or zero)
 * @dp: user data
 */
noinline fastcall int
n_discon_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *res, np_ulong reason,
	     np_ulong seq, mblk_t *dp)
{
	N_discon_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->res);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;

	/* save responding address for later */
	if (res) {
		sc->res = *res;
		sc->RES_length = sizeof(*res);
	}

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DISCON_REQ;
	p->DISCON_reason = reason;
	p->RES_length = sc->RES_length;
	p->RES_offset = sc->RES_length ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->res, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	mp->b_cont = dp;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_DISCON_REQ ->");
	sc_set_state(sc, NS_IDLE);
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_data_req: - issue an N_DATA_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @flags: data transfer flags
 * @dp: user data
 */
noinline fastcall int
n_data_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_REQ;
	p->DATA_xfer_flags = flags;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_DATA_REQ ->");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_exdata_req: - issue an N_EXDATA_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @dp: user data
 */
noinline fastcall int
n_exdata_req(struct sc *sc, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_REQ;
	mp->b_wptr += sizeof(*p);
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_EXDATA_REQ ->");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_info_req: - issue an N_INFO_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_info_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_info_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_INFO_REQ ->");
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_bind_req: - issue an N_BIND_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @maxinds: maximum number of connection indications
 * @flags: bind flags
 */
noinline fastcall int
n_bind_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong maxinds, np_ulong flags)
{
	N_bind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add) + sizeof(sc->pro);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_REQ;
	p->ADDR_length = sizeof(sc->add);
	p->ADDR_offset = sizeof(*p);
	p->CONIND_number = maxinds;
	p->BIND_flags = flags;
	p->PROTOID_length = sc->proto.info.PROTOID_length;
	p->PROTOID_offset = p->ADDR_offset + p->ADDR_length;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	bcopy(&sc->pro, mp->b_wptr, p->PROTOID_length);
	mp->b_wptr += p->PROTOID_length;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_BIND_REQ ->");
	sc_set_state(sc, NS_WACK_BREQ);
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_unbind_req: - issue an N_UNBIND_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_unbind_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNBIND_REQ;
	mp->b_wptr += sizeof(*p);
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_UNBIND_REQ ->");
	sc_set_state(sc, NS_WCON_CREQ);
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_unitdata_req: - issue an N_UNITDATA_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @rem: destination of message
 * @qos: Quality of Service parameters
 * @dp: user data
 */
noinline fastcall int
n_unitdata_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *rem,
	       N_qos_sel_data_sccp_t *qos, mblk_t *dp)
{
	N_unitdata_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*rem) + sizeof(*qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_REQ;
	p->DEST_length = sizeof(*rem);
	p->DEST_offset = sizeof(*p);
	p->RESERVED_field[0] = sizeof(*qos);
	p->RESERVED_field[1] = p->DEST_offset + p->DEST_length;
	mp->b_wptr += sizeof(*p);
	bcopy(rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	bcopy(qos, mp->b_wptr, p->RESERVED_field[0]);
	mp->b_wptr += p->RESERVED_field[0];
	mp->b_cont = dp;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_UNITDATA_REQ ->");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_optmgmt_req: - issue an N_OPTMGMT_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @qptr: QoS parameter pointer
 * @qlen: Qos parameter length
 * @flags: management flags
 */
noinline fastcall int
n_optmgmt_req(struct sc *sc, queue_t *q, mblk_t *msg, caddr_t qptr, size_t qlen, np_ulong flags)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + qlen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OPTMGMT_REQ;
	p->QOS_length = qlen;
	p->QOS_offset = qlen ? sizeof(*p) : 0;
	p->OPTMGMT_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(qptr, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_OPTMGMT_REQ ->");
	if (sc_get_state(sc) == NS_IDLE)
		sc_set_state(sc, NS_WACK_OPTREQ);
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_datack_req: - issue an N_DATACK_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_datack_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_datack_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_REQ;
	mp->b_wptr += sizeof(*p);
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_DATACK_REQ ->");
	sc->state.datacks--;
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_reset_req: - issue an N_RESET_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @reason: reset reason
 */
noinline fastcall int
n_reset_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_reset_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_REQ;
	p->RESET_reason = reason;
	mp->b_wptr += sizeof(*p);
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_RESET_REQ ->");
	sc_set_state(sc, NS_WCON_RREQ);
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_reset_res: - issue an N_RESET_RES primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_reset_res(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_reset_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_RES;
	mp->b_wptr += sizeof(*p);
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_RESET_RES ->");
	sc_set_state(sc, NS_WACK_RRES);
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_request_req: - issue an N_REQUEST_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @loc: source address (or NULL)
 * @sref: source local reference
 */
noinline fastcall int
n_request_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *loc, np_ulong sref)
{
	N_request_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*loc) + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_REQUEST_REQ;
	p->SRC_length = loc ? sizeof(*loc) : 0;
	p->SRC_offset = loc ? sizeof(*p) : 0;
	p->SRC_lref = sref;
	p->QOS_length = sizeof(sc->qos);
	p->QOS_offset = p->SRC_offset + p->SRC_length;
	mp->b_wptr += sizeof(*p);
	bcopy(loc, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_REQUEST_REQ ->");
	sc_set_state(sc, NS_WCON_CREQ);
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_inform_req: - issue an N_INFORM_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @resaon: inform reason
 */
noinline fastcall int
n_inform_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_inform_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFORM_REQ;
	p->QOS_length = sizeof(sc->qos);
	p->QOS_offset = sizeof(*p);
	p->REASON = reason;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_INFORM_REQ ->");
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_state_req: - issue an N_STATE_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_state_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong status)
{
	N_state_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_STATE_REQ;
	p->ADDR_length = sizeof(sc->add);
	p->ADDR_offset = sizeof(*p);
	p->STATUS = status;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_STATE_REQ ->");
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_coord_req: - issue an N_COORD_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_coord_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_coord_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_COORD_REQ;
	p->ADDR_length = sizeof(sc->add);
	p->ADDR_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_COORD_REQ ->");
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_coord_res: - issue an N_COORD_RES primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_coord_res(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_coord_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_COORD_RES;
	p->ADDR_length = sizeof(sc->add);
	p->ADDR_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	mi_strlog(sc->oq, STRLOGTX, SL_TRACE, "N_COORD_RES ->");
	freemsg(msg);
	putnext(sc->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/*
 * NPI User to NPI Provider Primitives
 */

/**
 * np_conn_req: - process N_CONN_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_CONN_REQ primitive
 */
noinline fastcall int
np_conn_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (p->DEST_length == 0)
		goto noaddr;
	if (p->DEST_length < sizeof(np->rem))
		goto badaddr;
	bcopy(mp->b_rptr + p->DEST_offset, &np->rem, sizeof(np->rem));
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badqos;
	if (p->QOS_length == 0)
		goto noqos;
	if (p->QOS_length < sizeof(np->qos))
		goto badqos;
	bcopy(mp->b_rptr + p->QOS_offset, &np->qos, sizeof(np->qos));
      noqos:
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (mp->b_cont && msgsize(mp->b_cont) > np->proto.info.CDATA_size)
		goto baddata;
	np_set_state(np, NS_WCON_CREQ);
	return n_conn_req(np->sc, q, mp, &np->rem, (N_qos_sel_conn_sccp_t *)&np->qos, p->CONN_flags, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      badqos:
	err = NBADOPT;
	goto error;
      noaddr:
	err = NNOADDR;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_conn_res: - process N_CONN_RES primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_CONN_RES primitive
 */
noinline fastcall int
np_conn_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_conn_sccp_t *qos;
	struct sccp_addr *res;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) != NS_WRES_CIND)
		goto outstate;
	if (!MBLKIN(mp, p->RES_offset, p->RES_length))
		goto badaddr;
	if (p->RES_length) {
		if (p->RES_length < sizeof(*res))
			goto badaddr;
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &np->res, sizeof(*res));
		np->RES_length = sizeof(*res);
	} else {
		res = NULL;	/* Responding address is implied by the bound address. */
	}
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badqos;
	if (p->QOS_length) {
		if (p->QOS_length < sizeof(*qos))
			goto badqos;
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &np->qos, sizeof(*qos));
		np->QOS_length = sizeof(*qos);
	} else {
		qos = NULL;
	}
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if (mp->b_cont && msgsize(mp->b_cont) > np->proto.info.CDATA_size)
		goto baddata;
	np->state.accept = p->TOKEN_value;
	np->state.pending = N_CONN_RES;
	np_set_state(np, NS_WACK_CRES);
	return n_conn_res(np->sc, q, mp, res, qos, p->TOKEN_value, p->CONN_flags, p->SEQ_number,
			  mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      badqos:
	err = NBADOPT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_discon_req: - process N_DISCON_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_DISCON_REQ primitive
 */
noinline fastcall int
np_discon_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
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
	default:
		goto outstate;
	}
	if (!MBLKIN(mp, p->RES_offset, p->RES_length))
		goto badaddr;
	if (p->RES_length) {
		if (p->RES_length < sizeof(np->res))
			goto badaddr;
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &np->res, sizeof(np->res));
	} else {
		res = NULL;
	}
	if (mp->b_cont && msgsize(mp->b_cont) > np->proto.info.DDATA_size)
		goto baddata;
	return n_discon_req(np->sc, q, mp, res, p->DISCON_reason, p->SEQ_number, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_data_req: - process N_DATA_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_DATA_REQ primitive
 */
noinline fastcall int
np_data_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
	case NS_IDLE:
		goto discard;
	case NS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (np->state.edatack)
		/* The NS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (p->DATA_xfer_flags & ~(N_RC_FLAG | N_MORE_DATA_FLAG))
		goto badflag;
	if (mp->b_cont && msgsize(mp->b_cont) > np->proto.info.NIDU_size)
		goto baddata;
	if (p->DATA_xfer_flags & N_RC_FLAG)
		np->state.datacks++;
	return n_data_req(np->sc, q, mp, p->DATA_xfer_flags, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_exdata_req: - process N_EXDATA_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_EXDATA_REQ primitive
 */
noinline fastcall int
np_exdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_exdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
	case NS_IDLE:
		goto discard;
	case NS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (np->state.edatack)
		/* The NS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (mp->b_cont && msgsize(mp->b_cont) > np->proto.info.ENSDU_size)
		goto baddata;
	np->state.edatack = 1;
	return n_exdata_req(np->sc, q, mp, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_info_req: - process N_INFO_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_INFO_REQ primitive
 */
noinline fastcall int
np_info_req(struct np *np, queue_t *q, mblk_t *mp)
{
	return n_info_req(np->sc, q, mp);
}

/**
 * np_bind_req: - process N_BIND_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_BIND_REQ primitive
 */
noinline fastcall int
np_bind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) != NS_UNBND)
		goto outstate;
	if (!MBLKIN(mp, p->ADDR_offset, p->ADDR_length))
		goto badaddr;
	if (p->ADDR_length) {
		if (p->ADDR_length < sizeof(np->add))
			goto badaddr;
		bcopy(mp->b_rptr + p->ADDR_offset, &np->add, sizeof(np->add));
	}
	if (!MBLKIN(mp, p->PROTOID_offset, p->PROTOID_length))
		goto badaddr;
	if (p->PROTOID_length == 0)
		goto noproto;
	if (p->PROTOID_length > sizeof(np->pro))
		goto badaddr;
	bcopy(mp->b_rptr + p->PROTOID_offset, &np->pro, p->PROTOID_length);
	if (p->BIND_flags & ~(DEFAULT_LISTENER | TOKEN_REQUEST | DEFAULT_DEST))
		goto badflag;
	np_set_state(np, NS_WACK_BREQ);
	return n_bind_req(np->sc, q, mp, p->CONIND_number, p->BIND_flags);
      badflag:
	err = NBADFLAG;
	goto error;
      noproto:
	err = NNOPROTOID;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_unbind_req: - process N_UNBIND_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_UNBIND_REQ primitive
 */
noinline fastcall int
np_unbind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WACK_UREQ);
	return n_unbind_req(np->sc, q, mp);
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_unitdata_req: - process N_UNITDATA_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_UNITDATA_REQ primitive
 */
noinline fastcall int
np_unitdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *rptr = NULL;
	N_qos_sel_data_sccp_t *qptr = NULL;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
	case NS_IDLE:
		if (p->DEST_length == 0)
			goto noaddr;
		rptr = &np->rem;
		break;
	case NS_DATA_XFER:
		/* Why not? If connection oriented wants to use this primitive, let it.  But ignore
		   the destination address. */
		break;
	default:
		goto outstate;
	}
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (rptr) {
		if (p->DEST_length < sizeof(*rptr))
			goto badaddr;
		bcopy(mp->b_rptr + p->DEST_offset, rptr, sizeof(*rptr));
	}
	if (!MBLKIN(mp, p->RESERVED_field[1], p->RESERVED_field[0]))
		goto badqos;
	if (p->RESERVED_field[0]) {
		qptr = (N_qos_sel_data_sccp_t *)&np->qos;
		if (p->RESERVED_field[0] < sizeof(*qptr))
			goto badqos;
		bcopy(mp->b_rptr + p->RESERVED_field[1], qptr, sizeof(*qptr));
	}
	if (mp->b_cont && msgsize(mp->b_cont) > np->proto.info.NIDU_size)
		goto baddata;
	if (mp->b_cont && msgsize(mp->b_cont) > np->proto.info.NSDU_size)
		goto baddata;
	return n_unitdata_req(np->sc, q, mp, rptr, qptr, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badqos:
	err = NBADOPT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      noaddr:
	err = NNOADDR;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_optmgmt_req: - process N_OPTMGMG_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_OPTMGMG_REQ primitive
 */
noinline fastcall int
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badqos;
	if (p->QOS_length == 0)
		goto badqostype;
	if (p->OPTMGMT_flags & ~(DEFAULT_RC_SEL))
		goto badflag;
	if (np_get_state(np) == NS_IDLE)
		np_set_state(np, NS_WACK_OPTREQ);
	return n_optmgmt_req(np->sc, q, mp, mp->b_rptr + p->QOS_offset, p->QOS_length,
			     p->OPTMGMT_flags);
      badflag:
	err = NBADFLAG;
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	goto error;
      badqos:
	err = NBADOPT;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_datack_req: - process N_DATACK_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_DATACK_REQ primitive
 */
noinline fastcall int
np_datack_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_datack_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
	case NS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_DATACK_REQ primitive, then the NS provider should discard the request without
		   generating a fatal error. */
		goto discard;
	case NS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (np->state.datinds <= 0)
		/* Note: If the NS provider had no knowledge of a previous N_DATA_IND with the
		   receipt confirmation flag set, then the NS provider should just ignore the
		   request without generating a fatal error. */
		goto discard;
	np->state.datinds--;
	return n_datack_req(np->sc, q, mp);
      outstate:
	err = -EFAULT;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_inform_req: - process N_RESET_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_REQ primitive
 *
 * The N-INFORM request primitive is implemented using the N_RESET_REQ primitive.
 */
noinline fastcall int
np_inform_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
	case NS_IDLE:
		goto discard;
	case NS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	return n_inform_req(np->sc, q, mp, p->RESET_reason);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_reset_req: - process N_RESET_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_REQ primitive
 *
 * The N-RESET request primitive is implemented using the N_RESET_REQ primitive.
 */
noinline fastcall int
np_reset_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
	case NS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_RESET_REQ primitive, then the NS provider should discard the message without
		   generating an error. */
		goto discard;
	case NS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (np->state.resacks)
		/* If there are outstanding reset requests, we are out of state. */
		goto outstate;
	np->state.resacks = 1;
	np_set_state(np, NS_WCON_RREQ);
	switch (p->RESET_reason) {
	case 0:
#if 0
	case XXX: /* FIXME */
	case YYY:
	case ZZZ:
#endif
		/* NPI-SCCP uses a combination of the N_OPTMGMT_REQ and the N_RESET_REQ to emulate
		   the N_INFORM_REQ primitive.  When the RESET_reason is one of the three reset
		   reasons used by the N_INFORM_REQ, then the primitive is transformed to an
		   N_INFORM_REQ, otherwise it is passed as an N_RESET_REQ. */
		np->state.infinds = 1;
		return n_inform_req(np->sc, q, mp, p->RESET_reason);
	}
	np->state.infinds = 0;
	return n_reset_req(np->sc, q, mp, p->RESET_reason);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * np_reset_res: - process N_RESET_RES primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_RES primitive
 *
 * The N-RESET response primitive is implemented using the N_RESET_RES primitive.
 */
noinline fastcall int
np_reset_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (np_get_state(np)) {
	case NS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_RESET_RES primitive, then the NS provider should discard the message without
		   generating an error. */
		goto discard;
	case NS_WRES_RIND:
		break;
	default:
		goto outstate;
	}
	if (np->state.resinds <= 0)
		goto outstate;
	if (np->state.infinds) {
		np->state.infinds = 0;
		np_set_state(np, NS_WACK_RRES);
		return n_ok_ack(np, q, mp, N_RESET_RES);
	}
	np->state.resinds = 0;
	np_set_state(np, NS_WACK_RRES);
	return n_reset_res(np->sc, q, mp);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/*
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
 */

















/*
 * SCCPI provider to SCCPI user primitives.
 */

/**
 * sc_conn_ind: - process N_CONN_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_CONN_IND primitive
 */
noinline fastcall int
sc_conn_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *loc, *rem;
	N_qos_sel_conn_sccp_t *qos;

	if (p->DEST_length) {
		loc = (typeof(loc)) (mp->b_rptr + p->DEST_offset);
		bcopy(loc, &sc->loc, p->DEST_length);
		sc->LOCADDR_length = p->DEST_length;
	} else {
		loc = NULL;
	}
	if (p->SRC_length) {
		rem = (typeof(rem)) (mp->b_rptr + p->SRC_offset);
		bcopy(rem, &sc->rem, p->SRC_length);
		sc->REMADDR_length = p->SRC_length;
	} else {
		rem = NULL;
	}
	if (p->QOS_length) {
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &sc->qos, p->QOS_length);
		sc->QOS_length = p->QOS_length;
	} else {
		qos = NULL;
	}
	sc->state.coninds++;
	sc_set_state(sc, NS_WRES_CIND);
	return n_conn_ind(sc->np, q, mp, loc, rem, qos, p->SEQ_number, p->CONN_flags, mp->b_cont);
}

/**
 * sc_conn_con: - process N_CONN_CON primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_CONN_CON primitive
 */
noinline fastcall int
sc_conn_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res;
	N_qos_sel_conn_sccp_t *qos;

	if (p->RES_length) {
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &sc->res, p->RES_length);
	} else
		res = NULL;
	if (p->QOS_length) {
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &sc->qos, p->QOS_length);
	} else
		qos = NULL;
	sc_set_state(sc, NS_DATA_XFER);
	return n_conn_con(sc->np, q, mp, res, qos, p->CONN_flags, mp->b_cont);
}

/**
 * sc_discon_ind: - process N_DISCON_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_DISCON_IND primitive
 */
noinline fastcall int
sc_discon_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res;

	if (p->RES_length) {
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &sc->res, p->RES_length);
	} else
		res = NULL;
	if (p->SEQ_number)
		sc->state.coninds--;
	sc_set_state(sc, sc->state.coninds ? NS_WRES_CIND : NS_IDLE);
	return n_discon_ind(sc->np, q, mp, res, p->DISCON_orig, p->DISCON_reason, p->SEQ_number,
			    mp->b_cont);
}

/**
 * sc_data_ind: - process N_DATA_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_DATA_IND primitive
 */
noinline fastcall int
sc_data_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;

	return n_data_ind(sc->np, q, mp, p->DATA_xfer_flags, mp->b_cont);
}

/**
 * sc_exdata_ind: - process N_EXDATA_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_EXDATA_IND primitive
 */
noinline fastcall int
sc_exdata_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;

	return n_exdata_ind(sc->np, q, mp, mp->b_cont);
}

/**
 * sc_info_ack: - process N_INFO_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_INFO_ACK primitive
 */
noinline fastcall int
sc_info_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	N_qos_sel_info_sccp_t *qos;
	N_qos_range_info_sccp_t *qor;

	sc->proto.info.PRIM_type = p->PRIM_type;
	sc->proto.info.NSDU_size = p->NSDU_size;
	sc->proto.info.ENSDU_size = p->ENSDU_size;
	sc->proto.info.CDATA_size = p->CDATA_size;
	sc->proto.info.DDATA_size = p->DDATA_size;
	sc->proto.info.ADDR_size = p->ADDR_size;
	sc->proto.info.ADDR_length = p->ADDR_length;
	sc->proto.info.ADDR_offset = p->ADDR_offset;
	if (p->ADDR_length) {
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		bcopy(add, &sc->add, p->ADDR_length);
		sc->ADDR_length = p->ADDR_length;
	}
	sc->proto.info.QOS_length = p->QOS_length;
	sc->proto.info.QOS_offset = p->QOS_offset;
	if (p->QOS_length) {
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &sc->qos, p->QOS_length);
		sc->QOS_length = p->QOS_length;
	}
	sc->proto.info.QOS_range_length = p->QOS_range_length;
	sc->proto.info.QOS_range_offset = p->QOS_range_offset;
	if (p->QOS_range_length) {
		qor = (typeof(qor)) (mp->b_rptr + p->QOS_range_offset);
		bcopy(qor, &sc->qor, p->QOS_range_length);
		sc->QOS_range_length = p->QOS_length;
	}
	sc->proto.info.OPTIONS_flags = p->OPTIONS_flags;
	sc->proto.info.NIDU_size = p->NIDU_size;
	sc->proto.info.SERV_type = p->SERV_type;
	sc->proto.info.CURRENT_state = p->CURRENT_state;
	sc->proto.info.PROVIDER_type = p->PROVIDER_type;
	sc->proto.info.NODU_size = p->NODU_size;
	sc->proto.info.PROTOID_length = p->PROTOID_length;
	sc->proto.info.PROTOID_offset = p->PROTOID_offset;
	bcopy(mp->b_rptr + p->PROTOID_offset, &sc->pro, p->PROTOID_length);
	sc->proto.info.NPI_version = p->NPI_version;
	return n_info_ack(sc->np, q, mp);
}

/**
 * sc_bind_ack: - process N_BIND_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_BIND_ACK primitive
 */
noinline fastcall int
sc_bind_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	unsigned char *pro;

	if (p->ADDR_length) {
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		bcopy(add, &sc->add, p->ADDR_length);
		sc->ADDR_length = p->ADDR_length;
	}
	sc->proto.info.ADDR_length = p->ADDR_length;
	if (p->PROTOID_length) {
		pro = (typeof(pro)) (mp->b_rptr + p->PROTOID_offset);
		bcopy(pro, &sc->pro, p->PROTOID_length);
		sc->PROTOID_length = p->PROTOID_length;
	}
	sc->proto.info.PROTOID_length = p->PROTOID_length;
	sc->maxinds = p->CONIND_number;
	sc->TOKEN_value = p->TOKEN_value;
	return n_bind_ack(sc->np, q, mp);
}

/**
 * sc_error_ack: - process N_ERROR_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_ERROR_ACK primitive
 */
noinline fastcall int
sc_error_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	np_long error;

	if ((error = p->NPI_error) == NSYSERR)
		error = -p->UNIX_error;
	switch (sc_get_state(sc)) {
	case NS_WACK_BREQ:
		sc_set_state(sc, NS_UNBND);
		break;
	case NS_WACK_UREQ:
		sc_set_state(sc, NS_IDLE);
		break;
	case NS_WACK_OPTREQ:
		sc_set_state(sc, NS_IDLE);
		break;
	case NS_WACK_RRES:
		sc_set_state(sc, NS_WRES_RIND);
		break;
	case NS_WCON_CREQ:
		sc_set_state(sc, NS_IDLE);
		break;
	case NS_WACK_CRES:
		sc_set_state(sc, NS_WRES_CIND);
		break;
	case NS_WCON_RREQ:
		sc_set_state(sc, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ6:
		sc_set_state(sc, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		sc_set_state(sc, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		sc_set_state(sc, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		sc_set_state(sc, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		sc_set_state(sc, NS_WRES_RIND);
		break;
	}
	return n_error_ack(sc->np, q, mp, p->ERROR_prim, error);
}

/**
 * sc_ok_ack: - process N_OK_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_OK_ACK primitive
 */
noinline fastcall int
sc_ok_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	switch (sc_get_state(sc)) {
	case NS_WACK_BREQ:
	case NS_WACK_OPTREQ:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		sc_set_state(sc, NS_IDLE);
		break;
	case NS_WACK_UREQ:
		sc_set_state(sc, NS_UNBND);
		break;
	case NS_WACK_RRES:
	case NS_WACK_CRES:
		sc_set_state(sc, NS_DATA_XFER);
		break;
	}
	return n_ok_ack(sc->np, q, mp, p->CORRECT_prim);

}

/**
 * sc_unitdata_ind: - process N_UNITDATA_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_UNITDATA_IND primitive
 */
noinline fastcall int
sc_unitdata_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *rem;
	struct sccp_addr *loc;

	loc = p->SRC_length ? (typeof(loc)) (mp->b_rptr + p->SRC_offset) : NULL;
	rem = p->DEST_length ? (typeof(rem)) (mp->b_rptr + p->DEST_offset) : NULL;
	return n_unitdata_ind(sc->np, q, mp, rem, loc, NULL, p->ERROR_type, mp->b_cont);
}

/**
 * sc_uderror_ind: - process N_UDERROR_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_UDERROR_IND primitive
 */
noinline fastcall int
sc_uderror_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *rem;

	rem = p->DEST_length ? (typeof(rem)) (mp->b_rptr + p->DEST_offset) : NULL;
	return n_uderror_ind(sc->np, q, mp, rem, NULL, p->ERROR_type, mp->b_cont);
}

/**
 * sc_datack_ind: - process N_DATACK_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_DATACK_IND primitive
 */
noinline fastcall int
sc_datack_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sc->state.edatack)
		sc->state.edatack = 0;
	else if (sc->state.datacks >= 0)
		sc->state.datacks--;
	return n_datack_ind(sc->np, q, mp);
}

/**
 * sc_reset_ind: - process N_RESET_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_IND primitive
 */
noinline fastcall int
sc_reset_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;

	sc_set_state(sc, NS_WRES_RIND);
	return n_reset_ind(sc->np, q, mp, p->RESET_orig, p->RESET_reason);
}

/**
 * sc_reset_con: - process N_RESET_CON primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_CON primitive
 */
noinline fastcall int
sc_reset_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;

	sc_set_state(sc, NS_DATA_XFER);
	return n_reset_con(sc->np, q, mp);
}

/**
 * sc_notice_ind: - process N_NOTICE_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_NOTICE_IND primitive
 */
noinline fastcall int
sc_notice_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_notice_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *rem;

	rem = (typeof(rem)) (mp->b_rptr + p->DEST_offset);
	/* In mapping the N_NOTICE_IND primitive to the N_UDERROR_IND primitive, the source address 
	   and quality of service parameters associated with the message are lost. */
	return n_uderror_ind(sc->np, q, mp, rem, NULL, p->RETURN_cause, mp->b_cont);
}

/**
 * sc_inform_ind: - process N_INFORM_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_INFORM_IND primitive
 */
noinline fastcall int
sc_inform_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_inform_ind_t *p = (typeof(p)) mp->b_rptr;

	bcopy(mp->b_rptr + p->QOS_offset, &sc->qos, p->QOS_length);
	sc->state.infinds = 1;
	sc_set_state(sc, NS_WRES_RIND);
	return n_inform_ind(sc->np, q, mp, N_PROVIDER, p->REASON);
}

/**
 * sc_coord_ind: - process N_COORD_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_COORD_IND primitive
 */
noinline fastcall int
sc_coord_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_ind_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_coord_con: - process N_COORD_CON primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_COORD_CON primitive
 */
noinline fastcall int
sc_coord_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_con_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_state_ind: - process N_STATE_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_STATE_IND primitive
 */
noinline fastcall int
sc_state_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_state_ind_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_pcstate_ind: - process N_PCSTATE_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_PCSTATE_IND primitive
 */
noinline fastcall int
sc_pcstate_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_pcstate_ind_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_traffic_ind: - process N_TRAFFIC_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_TRAFFIC_IND primitive
 */
noinline fastcall int
sc_traffic_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_traffic_ind_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_reply_ack: - process N_REPLY_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_REPLY_ACK primitive
 */
noinline fastcall int
sc_reply_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reply_ack_t *p = (typeof(p)) mp->b_rptr;

	bcopy(mp->b_rptr + p->QOS_offset, &sc->qos, p->QOS_length);
	sc->SRC_lref = p->SRC_lref;
	freemsg(mp);
	return (0);
}

/*
 * MESSGAGE HANDLING
 */

/**
 * m_w_other: - process other message
 * @q: active queue (write queue)
 * @mp: STREAMS message
 */
static int
m_w_other(queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		return (-EBUSY);
	putnext(q, mp);
	return (0);
}

/**
 * m_r_other: - process other message
 * @q: active queue (read queue)
 * @mp: STREAMS message
 */
static int
m_r_other(queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		return (-EBUSY);
	putnext(q, mp);
	return (0);
}

static int
np_error_reply(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_long error,
	       const char *func)
{
	/* FIXME */
	return n_error_ack(np, q, msg, prim, error);
}
/**
 * np_w_proto: - process M_PROTO or M_PCPROTO message
 * @np: network provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static int
np_w_proto(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong prim = -1;
	int err = -EFAULT, level;

	np_save_state(np);
	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	switch ((prim = *(typeof(prim) *) mp->b_rptr)) {
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_DATACK_REQ:
	case N_UNITDATA_REQ:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(q, level, SL_TRACE, "-> %s", np_primname(prim));
	switch (prim) {
	case N_CONN_REQ:
		err = np_conn_req(np, q, mp);
		break;
	case N_CONN_RES:
		err = np_conn_res(np, q, mp);
		break;
	case N_DISCON_REQ:
		err = np_discon_req(np, q, mp);
		break;
	case N_DATA_REQ:
		err = np_data_req(np, q, mp);
		break;
	case N_EXDATA_REQ:
		err = np_exdata_req(np, q, mp);
		break;
	case N_INFO_REQ:
		err = np_info_req(np, q, mp);
		break;
	case N_BIND_REQ:
		err = np_bind_req(np, q, mp);
		break;
	case N_UNBIND_REQ:
		err = np_unbind_req(np, q, mp);
		break;
	case N_UNITDATA_REQ:
		err = np_unitdata_req(np, q, mp);
		break;
	case N_OPTMGMT_REQ:
		err = np_optmgmt_req(np, q, mp);
		break;
	case N_DATACK_REQ:
		err = np_datack_req(np, q, mp);
		break;
	case N_RESET_REQ:
		err = np_reset_req(np, q, mp);
		break;
	case N_RESET_RES:
		err = np_reset_res(np, q, mp);
		break;
	default:
#if 0
		err = np_other_req(np, q, mp);
#endif
		err = -EPROTO;
		break;
	}
      done:
	if (err)
		np_restore_state(np);
	return np_error_reply(np, q, mp, prim, err, np_primname(prim));
}

static int
sc_error_reply(struct sc *sc, queue_t *q, mblk_t *msg, np_long prim, np_long error,
	       const char *func)
{
	/* FIXME */
	freemsg(msg);
	return (0);
}

/**
 * sc_w_proto: - process M_PROTO or M_PCPROTO message
 * @sc: SCCPI private structure (locked)
 * @q: active queue (read queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static int
sc_r_proto(struct sc *sc, queue_t *q, mblk_t *mp)
{
	np_ulong prim = -1;
	int err = -EFAULT, level;

	sc_save_state(sc);
	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	switch ((prim = *(typeof(prim) *) mp->b_rptr)) {
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_UNITDATA_IND:
	case N_UDERROR_IND:
	case N_NOTICE_IND:
	case N_DATACK_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(q, level, SL_TRACE, "%s <-", sc_primname(prim));
	switch (prim) {
	case N_CONN_IND:
		err = sc_conn_ind(sc, q, mp);
		break;
	case N_CONN_CON:
		err = sc_conn_con(sc, q, mp);
		break;
	case N_DISCON_IND:
		err = sc_discon_ind(sc, q, mp);
		break;
	case N_DATA_IND:
		err = sc_data_ind(sc, q, mp);
		break;
	case N_EXDATA_IND:
		err = sc_exdata_ind(sc, q, mp);
		break;
	case N_INFO_ACK:
		err = sc_info_ack(sc, q, mp);
		break;
	case N_BIND_ACK:
		err = sc_bind_ack(sc, q, mp);
		break;
	case N_ERROR_ACK:
		err = sc_error_ack(sc, q, mp);
		break;
	case N_OK_ACK:
		err = sc_ok_ack(sc, q, mp);
		break;
	case N_UNITDATA_IND:
		err = sc_unitdata_ind(sc, q, mp);
		break;
	case N_UDERROR_IND:
		err = sc_uderror_ind(sc, q, mp);
		break;
	case N_DATACK_IND:
		err = sc_datack_ind(sc, q, mp);
		break;
	case N_RESET_IND:
		err = sc_reset_ind(sc, q, mp);
		break;
	case N_RESET_CON:
		err = sc_reset_con(sc, q, mp);
		break;
	case N_NOTICE_IND:
		err = sc_notice_ind(sc, q, mp);
		break;
	case N_INFORM_IND:
		err = sc_inform_ind(sc, q, mp);
		break;
	case N_COORD_IND:
		err = sc_coord_ind(sc, q, mp);
		break;
	case N_COORD_CON:
		err = sc_coord_con(sc, q, mp);
		break;
	case N_STATE_IND:
		err = sc_state_ind(sc, q, mp);
		break;
	case N_PCSTATE_IND:
		err = sc_pcstate_ind(sc, q, mp);
		break;
	case N_TRAFFIC_IND:
		err = sc_traffic_ind(sc, q, mp);
		break;
	case N_REPLY_ACK:
		err = sc_reply_ack(sc, q, mp);
		break;
	default:
		err = -EPROTO;
#if 0
		err = sc_other_ind(sc, q, mp);
#endif
		break;
	}
      done:
	if (err)
		sc_restore_state(sc);
	return sc_error_reply(sc, q, mp, prim, err, sc_primname(prim));
}

/**
 * m_w_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static int
m_w_proto(queue_t *q, mblk_t *mp)
{
	struct priv *priv;
	int err;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = np_w_proto(&priv->np, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/**
 * m_r_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (read queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static int
m_r_proto(queue_t *q, mblk_t *mp)
{
	struct priv *priv;
	int err;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = sc_r_proto(&priv->sc, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/**
 * np_w_data: - process M_DATA or M_HPDATA message
 * @np: network provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: M_DATA or M_HPDATA message
 */
static int
np_w_data(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong DATA_xfer_flags = 0;
	int err;

	switch (np_get_state(np)) {
	case NS_IDLE:
		goto discard;
	case NS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (np->state.edatack)
		/* The NS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (mp->b_cont && msgsize(mp) > np->proto.info.NIDU_size)
		goto baddata;
	if (mp->b_band)
		return n_exdata_req(np->sc, q, NULL, mp);
	if (!(mp->b_flag & MSGDELIM))
		DATA_xfer_flags |= N_MORE_DATA_FLAG;
	return n_data_req(np->sc, q, NULL, 0, mp);
      baddata:
	err = NBADDATA;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	err = 0;
	goto error;
      error:
	return np_error_reply(np, q, mp, N_DATA_REQ, err, __FUNCTION__);
}

/**
 * sc_r_data: - process M_DATA or M_HPDATA message
 * @sc: SCCPI private structure (locked)
 * @q: active queue (read queue)
 * @mp: M_DATA or M_HPDATA message
 */
static int
sc_r_data(struct sc *sc, queue_t *q, mblk_t *mp)
{
	np_ulong DATA_xfer_flags = 0;

	if (mp->b_band)
		return n_exdata_ind(sc->np, q, NULL, mp);
	if (!(mp->b_flag & MSGDELIM))
		DATA_xfer_flags |= N_MORE_DATA_FLAG;
	return n_data_ind(sc->np, q, NULL, DATA_xfer_flags, mp);
}

/**
 * m_w_data: - process M_DATA or M_HPDATA message
 * @q: active queue (write queue)
 * @mp: M_DATA or M_HPDATA message
 */
static int
m_w_data(queue_t *q, mblk_t *mp)
{
	struct priv *priv;
	int err;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = np_w_data(&priv->np, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/**
 * m_r_data: - process M_DATA or M_HPDATA message
 * @q: active queue (read queue)
 * @mp: M_DATA or M_HPDATA message
 */
static int
m_r_data(queue_t *q, mblk_t *mp)
{
	struct priv *priv;
	int err;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = sc_r_data(&priv->sc, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/**
 * m_w_flush: - process M_FLUSH message
 * @q: active queue (write queue)
 * @mp: M_FLUSH message
 */
static int
m_w_flush(queue_t *q, mblk_t *mp)
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

/**
 * m_r_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: M_FLUSH message
 */
static int
m_r_flush(queue_t *q, mblk_t *mp)
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

/**
 * m_r_error: - process M_ERROR message
 * @q: active queue (read queue)
 * @mp: M_ERROR message
 */
static int
m_r_error(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * m_r_hangup: - process M_HANGUP message
 * @q: active queue (read queue)
 * @mp: M_HANGUP message
 */
static int
m_r_hangup(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * np_w_msg: - process STREAMS message
 * @np: network provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: STREAMS message
 */
static int
np_w_msg(struct np *np, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = np_w_data(np, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = np_w_proto(np, q, mp);
		break;
	case M_FLUSH:
		err = m_w_flush(q, mp);
		break;
	default:
		err = m_w_other(q, mp);
		break;
	}
	return (err);
}

/**
 * sc_r_msg: - process STREAMS message
 * @sc: SCCPI private structure (locked)
 * @q: active queue (read queue)
 * @mp: STREAMS message
 */
static int
sc_r_msg(struct sc *sc, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = sc_r_data(sc, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = sc_r_proto(sc, q, mp);
		break;
	case M_FLUSH:
		err = m_r_flush(q, mp);
		break;
	case M_ERROR:
		err = m_r_error(q, mp);
		break;
	case M_HANGUP:
		err = m_r_hangup(q, mp);
		break;
	default:
		err = m_w_other(q, mp);
		break;
	}
	return (err);
}

/**
 * m_w_msg: - process STREAMS message
 * @q: active queue (write queue)
 * @mp: STREAMS message
 */
static int
m_w_msg(queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = m_w_data(q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = m_w_proto(q, mp);
		break;
	case M_FLUSH:
		err = m_w_flush(q, mp);
		break;
	default:
		err = m_w_other(q, mp);
		break;
	}
	return (err);
}

/**
 * m_r_msg: - process STREAMS message
 * @q: active queue (read queue)
 * @mp: STREAMS message
 */
static int
m_r_msg(queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = m_r_data(q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = m_r_proto(q, mp);
		break;
	case M_FLUSH:
		err = m_r_flush(q, mp);
		break;
	case M_ERROR:
		err = m_r_error(q, mp);
		break;
	case M_HANGUP:
		err = m_r_hangup(q, mp);
		break;
	default:
		err = m_r_other(q, mp);
		break;
	}
	return (err);
}

/*
 * QUEUE PUT AND SERVICE PROCEDURE
 */

/**
 * np_wput: - write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 */
static streamscall int
np_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || m_w_msg(q, mp)) {
		if (!putq(q, mp)) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * np_wsrv: - write service procedure
 * @q: active queue (write queue)
 */
static streamscall int
np_wsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		while ((mp = getq(q))) {
			if (np_w_msg(&priv->np, q, mp)) {
				if (!putbq(q, mp)) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) priv);
	}
	return (0);
}

/**
 * np_rsrv: - read service procedure
 * @q: active queue (read queue)
 */
static streamscall int
np_rsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		while ((mp = getq(q))) {
			if (sc_r_msg(&priv->sc, q, mp)) {
				if (!putbq(q, mp)) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) priv);
	}
	return (0);
}

/**
 * np_rput: - read put procedure
 * @q: active queue (read queue)
 * @mp: message to put
 */
static streamscall int
np_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || m_r_msg(q, mp)) {
		if (!putq(q, mp)) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/*
 * QUEUE OPEN AND CLOSE ROUTINE
 */

static caddr_t np_opens = NULL;

/**
 * np_qopen: - queue open routine
 * @q: read queue of queue pair
 * @devp: device number pointer
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @credp: credentials of opening process
 */
static streamscall int
np_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	struct priv *p;
	int err;

	if (q->q_ptr != NULL)
		return (0);
	if ((err = mi_open_comm(&np_opens, sizeof(*p), q, devp, oflags, sflag, credp)))
		return (err);
	p = PRIV(q);
	p->np.oq = RD(q);
	p->np.state.state = NS_UNBND;
	p->np.state.datinds = 0;
	p->np.state.datacks = 0;
	p->np.state.coninds = 0;
	p->np.state.infinds = 0;
	p->np.proto.info.PRIM_type = N_INFO_ACK;
	p->np.proto.info.NSDU_size = QOS_UNKNOWN;
	p->np.proto.info.ENSDU_size = QOS_UNKNOWN;
	p->np.proto.info.CDATA_size = QOS_UNKNOWN;
	p->np.proto.info.DDATA_size = QOS_UNKNOWN;
	p->np.proto.info.ADDR_size = QOS_UNKNOWN;
	p->np.proto.info.ADDR_length = QOS_UNKNOWN;
	p->np.proto.info.ADDR_offset = QOS_UNKNOWN;
	p->np.proto.info.QOS_length = QOS_UNKNOWN;
	p->np.proto.info.QOS_offset = QOS_UNKNOWN;
	p->np.proto.info.QOS_range_length = QOS_UNKNOWN;
	p->np.proto.info.QOS_range_offset = QOS_UNKNOWN;
	p->np.proto.info.OPTIONS_flags = QOS_UNKNOWN;
	p->np.proto.info.NIDU_size = QOS_UNKNOWN;
	p->np.proto.info.SERV_type = QOS_UNKNOWN;
	p->np.proto.info.CURRENT_state = QOS_UNKNOWN;
	p->np.proto.info.PROVIDER_type = QOS_UNKNOWN;
	p->np.proto.info.NODU_size = QOS_UNKNOWN;
	p->np.proto.info.PROTOID_length = QOS_UNKNOWN;
	p->np.proto.info.PROTOID_offset = QOS_UNKNOWN;
	p->np.proto.info.NPI_version = QOS_UNKNOWN;
	/* ... */
	p->sc.oq = WR(q);
	p->sc.state.state = NS_UNBND;
	p->sc.state.datinds = 0;
	p->sc.state.datacks = 0;
	p->sc.state.coninds = 0;
	p->sc.state.infinds = 0;
	p->sc.proto.info.PRIM_type = N_INFO_ACK;
	p->sc.proto.info.NSDU_size = QOS_UNKNOWN;
	p->sc.proto.info.ENSDU_size = QOS_UNKNOWN;
	p->sc.proto.info.CDATA_size = QOS_UNKNOWN;
	p->sc.proto.info.DDATA_size = QOS_UNKNOWN;
	p->sc.proto.info.ADDR_size = QOS_UNKNOWN;
	p->sc.proto.info.ADDR_length = QOS_UNKNOWN;
	p->sc.proto.info.ADDR_offset = QOS_UNKNOWN;
	p->sc.proto.info.QOS_length = QOS_UNKNOWN;
	p->sc.proto.info.QOS_offset = QOS_UNKNOWN;
	p->sc.proto.info.QOS_range_length = QOS_UNKNOWN;
	p->sc.proto.info.QOS_range_offset = QOS_UNKNOWN;
	p->sc.proto.info.OPTIONS_flags = QOS_UNKNOWN;
	p->sc.proto.info.NIDU_size = QOS_UNKNOWN;
	p->sc.proto.info.SERV_type = QOS_UNKNOWN;
	p->sc.proto.info.CURRENT_state = QOS_UNKNOWN;
	p->sc.proto.info.PROVIDER_type = QOS_UNKNOWN;
	p->sc.proto.info.NODU_size = QOS_UNKNOWN;
	p->sc.proto.info.PROTOID_length = QOS_UNKNOWN;
	p->sc.proto.info.PROTOID_offset = QOS_UNKNOWN;
	p->sc.proto.info.NPI_version = QOS_UNKNOWN;
	/* ... */
	return (0);
}

/**
 * np_qclose: - queue open routine
 * @q: read queue of queue pair
 * @oflags: open flags
 * @credp: credentials of closing process
 */
static streamscall int
np_qclose(queue_t *q, int oflags, cred_t *credp)
{
	qprocsoff(q);
	mi_close_comm(&np_opens, q);
	return (0);
}

static modID_t modid = MOD_ID;

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for NPI-SCCP.  (0 for allocation.)");

static struct fmodsw np_fmod = {
	.f_name = MOD_NAME,
	.f_str = &npi_sccpinfo,
	.f_flag = D_MP,
	.f_modid = MOD_ID,
	.f_kmod = THIS_MODULE,
};

/**
 * npi_sccp_modinit: - initialize kernel module
 */
static int __init
npi_sccp_modinit(void)
{
	int err;

	if ((err = register_strmod(&np_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: Could not register module: err = %d\n", __FUNCTION__, -err);
		return (-err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

/**
 * npi_sccp_modexit: - deinitialize kernel module
 */
static void __exit
npi_sccp_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&np_fmod)) < 0)
		cmn_err(CE_WARN, "%s: Could not deregister module: err = %d\n", __FUNCTION__, -err);
	return;
}

module_init(npi_sccp_modinit);
module_exit(npi_sccp_modexit);

#endif				/* LINUX */
