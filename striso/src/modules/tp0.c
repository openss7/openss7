/*****************************************************************************

 @(#) $RCSfile: tp0.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $

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

 Last Modified $Date: 2008-04-25 08:33:40 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tp0.c,v $
 Revision 0.9.2.1  2008-04-25 08:33:40  brian
 - added man pages and skeletons

 *****************************************************************************/

#ident "@(#) $RCSfile: tp0.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $"

static char const ident[] = "$RCSfile: tp0.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $";

/*
 *  ISO Transport Class 0 over TCP/IP (TP0)
 *
 *  TP0 implements ISO Transport Class 0 as a pushable module.  The purpose is
 *  to push this module over Streams that provide the OSI CONS NPI interface to
 *  effect a complete ISO Transport Class 0 Stream.  One such OSI CONS NPI
 *  interface is the ISOT module that pushes over a TCP/IP Stream.  The
 *  combination of the TP0 module and the ISOT module provides RFC 1006 or RFC
 *  2126 TP0 over TCP/IP.
 */

#include <sys/os8/compat.h>

#if define HAVE_TIDHR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/npi.h>

#define TP0_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TP0_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define TP0_REVISION	"OpenSS7 $RCSfile: tp0.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $"
#define TP0_DEVICE	"SVR 4.2 STREAMS TP0 Module for RFC 1006 and RFC 2126"
#define TP0_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TP0_LICENSE	"GPL"
#define TP0_BANNER	TP0_DESCRIP	"\n" \
			TP0_COPYRIGHT	"\n" \
			TP0_REVISION	"\n" \
			TP0_DEVICE	"\n" \
			TP0_CONTACT
#define TP0_SPLASH	TP0_DEVICE	" - " \
			TP0_REVISION

#ifdef LINUX
MODULE_AUTHOR(TP0_CONTACT);
MODULE_DESCRIPTION(TP0_DESCRIP);
MODULE_SUPPORTED_DEVICE(TP0_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TP0_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tp0");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_TP0_NAME
#error CONFIG_STREAMS_TP0_NAME must be defined.
#endif				/* CONFIG_STREAMS_TP0_NAME */
#ifndef CONFIG_STREAMS_TP0_MODID
#error CONFIG_STREAMS_TP0_MODID must be defined.
#endif				/* CONFIG_STREAMS_TP0_MODID */

#ifndef TP0_MOD_NAME
#define TP0_MOD_NAME		CONFIG_STREAMS_TP0_NAME
#endif				/* TP0_MOD_NAME */
#ifndef TP0_MOD_ID
#define TP0_MOD_ID		CONFIG_STREAMS_TP0_MODID
#endif				/* TP0_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID	    TP0_MOD_ID
#define MOD_NAME    TP0_MOD_NAME
#ifdef MODULE
#define MOD_BANNER  TP0_BANNER
#else				/* MODULE */
#define MOD_BANNER  TP0_SPLASH
#endif				/* MODULE */

static struct module_info tp0_info = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct tp;
struct np;
struct priv;

struct tp {
	struct priv *priv;
	struct np *np;
	queue_t *oq;
	t_scalar_t state;
	t_scalar_t oldstate;
	struct {
		struct T_info_ack info;
		struct T_addr_ack addr;
		struct sockaddr_in loc;
		struct sockaddr_in rem;
	} proto;
};

struct np {
	struct priv *priv;
	struct tp *tp;
	queue_t *oq;
	np_ulong state;
	np_ulong oldstate;
	struct {
		N_info_ack_t info;
		unsigned char add[20];
		N_qos_co_opt_sel_t qos;
		N_qos_co_opt_range_t qor;
	} proto;
};

struct priv {
	struct np r_priv;		/* NPI structure on RD side */
	struct tp w_priv;		/* TPI structure on WR side */
};

#define PRIV(q)	    ((struct priv *)(q)->q_ptr)

#define NP_PRIV(q)  (&PRIV(q)->w_priv)
#define TP_PRIV(q)  (&PRIV(q)->r_priv)

#define PAD4(len) ((len + 3) & ~3)

#define STRLOGIO	0	/* log Stream input-output controls */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

static const char *
tp_primname(t_scalar_t prim)
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
		return ("(unknown)");
	}
}
static inline const char *
tp_statename(t_scalar_t state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WREQ_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	case TS_NOSTATES:
		return ("TS_NOSTATES");
	default:
		return ("(unknown)");
	}
}
static t_scalar_t
tp_get_state(struct tp *tp)
{
	return (tp->state);
}
static t_scalar_t
tp_set_state(struct tp *tp, t_scalar_t newstate)
{
	t_scalar_t oldstate = tp->state;

	if (newstate != oldstate) {
		tp->state = newstate;
		tp->proto.info.CURRENT_state = newstate;
		mi_strlog(tp->oq, STRLOGST, SL_TRACE, "%s <- %s", tp_statename(newstate),
			  tp_statename(oldstate));
	}
	return (newstate);
}
static t_scalar_t
tp_save_state(struct tp *tp)
{
	return ((tp->oldstate = tp_get_state(tp)));
}
static t_scalar_t
tp_restore_state(struct tp *tp)
{
	return (tp_set_state(tp, tp->oldstate));
}

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
np_strerror(int error)
{
	if (error < 0)
		error = NSYSERR;
	switch (error) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information.");
	case NBADOPT:
		return ("Options in incorrect format or contain illegal information.");
	case NACCESS:
		return ("User did not have proper permissions.");
	case NNOADDR:
		return ("NS Provider could not allocate address.");
	case NOUTSTATE:
		return ("Primitive was issues in wrong sequence.");
	case NBADSEQ:
		return ("Sequence number in primitive was incorrect/illegal.");
	case NSYSERR:
		return ("UNIX system error occurred.");
	case NBADDATA:
		return ("User data spec. outside range supported by NS provider.");
	case NBADFLAG:
		return ("Flags specified in primitive were illegal/incorrect.");
	case NNOTSUPPORT:
		return ("Primitive type not supported by the NS provider.");
	case NBOUND:
		return ("Illegal second attempt to bind listener or default listener.");
	case NBADQOSPARAM:
		return ("QOS values specified are outside the range supported by the NS provider.");
	case NBADQOSTYPE:
		return ("QOS structure type specified is not supported by the NS provider.");
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

/*
 * --------------------------------------------------------------------------
 *
 * TPI Provider -> TPI User issued primtiives
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline int
m_error(struct tp *tp, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_conn_ind:- issue T_CONN_IND message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall noinline __unlikely
tp_conn_ind(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t src_ptr, size_t src_len,
	    caddr_t opt_ptr, size_t opt_len, t_scalar_t sequence, mblk_t *dp)
{
	struct T_conn_ind *p;
	mblk_t *mp;

	(void) tp_conn_ind;
	if (tp->coninds < tp->maxinds) {
		if ((mp = mi_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
			/* It is possible that we can feed T_CONN_IND upstream
			   faster than the User has buffer to accept. */
			if (canputnext(tp->oq)) {
				DB_TYPE(mp) = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->PRIM_type = T_CONN_IND;
				p->SRC_length = src_len;
				p->SRC_offset = sizeof(*p);
				p->OPT_length = opt_len;
				p->OPT_offset = sizeof(*p) + src_len;
				p->SEQ_number = sequence;
				mp->b_wptr += sizeof(*p);
				bcopy(src_ptr, mp->b_wptr, src_len);
				mp->b_wptr += src_len;
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
				mp->b_cont = dp;
				if (msg && msg->b_cont == dp)
					msg->b_cont = NULL;
				freemsg(msg);
				mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CONN_IND");
				tp_set_state(tp, TS_WRES_CIND);
				tp->coninds++;
				putnext(tp->oq, mp);
				return (0);
			} else {
				freeb(mp);
				return (-EBUSY);
			}
		}
		return (-ENOBUFS);
	}
	/* Note that when coninds falls back beneath maxinds then we must wake
	   up the upper write and lower read queues. */
	return (-EBUSY);
}

/**
 * tp_conn_con:- issue T_CONN_CON message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall noinline __unlikely
tp_conn_con(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t res_ptr, size_t res_len,
	    caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_conn_con *p;
	mblk_t *mp;

	(void) tp_conn_con;
	if ((mp = mi_allocb(q, sizeof(*p) + res_len + opt_len, BPRI_MED))) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + res_len;
			mp->b_wptr += sizeof(*p);
			bcop(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			bcop(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CONN_CON");
			tp_set_state(tp, TS_DATA_XFER);
			tp->coninds++;
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_discon_ind:- issue T_DISCON_IND message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 * @reason: disconnect reason
 * @sequence: sequence number of T_CONN_IND (or zero)
 * @dp: user data
 */
static fastcall noinline __unlikely
tp_discon_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t reason, t_scalar_t sequence,
	      mblk_t *dp)
{
	struct T_discon_ind *p;
	mblk_t *mp;

	(void) tp_discon_ind;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_IND;
			p->DISCON_reason = reason;
			p->SEQ_number = sequence;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_DISCON_IND");
			tp_set_state(tp, TS_IDLE);
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_data_ind:- issue T_DATA_IND message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 * @more: whether next primitive includes data for the same TSDU.
 * @dp: user data
 */
static fastcall inline __hot_get
tp_data_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;

	(void) tp_data_ind;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_DATA_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_exdata_ind:- issue T_EXDATA_IND message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 * @more: whether next primitive icnludes data for the same TSDU.
 * @dp: user data
 */
static fastcall inline __hot_get
tp_exdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_exdata_ind *p;
	mblk_t *mp;

	(void) tp_exdata_ind;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(tp->oq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_EXDATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_EXDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_info_ack:- issue T_INFO_ACK message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall noinline __unlikely
tp_info_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_ack *p;
	mblk_t *mp;

	(void) tp_info_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		bcopy(&tp->proto.info, mp->b_wptr, sizeof(*p));
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_INFO_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_bind_ack:- issue T_BIND_ACK message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 * @add_ptr: bound address pointer
 * @add_len: bound address length
 * @coninds: maximum number of connection indications
 */
static fastcall noinline __unlikely
tp_bind_ack(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len,
	    t_uscalar_t coninds)
{
	struct T_bind_ack *p;
	mblk_t *mp;

	(void) tp_bind_ack;
	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = coninds;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_BIND_ACK");
		tp_set_state(tp, TS_IDLE);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_error_ack:- issue T_ERROR_ACK message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 * @prim: primitive in error
 * @error: positive TPI error or negative UNIX error
 * @func: description of calling function
 */
static fastcall noinline __unlikely
tp_error_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t prim, t_scalar_t error,
	     const char *func)
{
	struct T_error_ack *p;
	mblk_t *mp;

	(void) tp_error_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = error > 0 ? error : TSYSERR;
		p->UNIX_error = error > 0 ? 0 : -error;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ERROR_ACK");
		switch (tp_get_state(tp)) {
		case TS_WACK_BREQ:
			tp_set_state(tp, TS_UNBND);
			break;
		case TS_WACK_CREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_CRES:
			tp_set_state(tp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ6:
			tp_set_state(tp, TS_WCON_CREQ);
			break;
		case TS_WACK_DREQ7:
			tp_set_state(tp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ9:
			tp_set_state(tp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ10:
			tp_set_state(tp, TS_WIND_ORDREL);
			break;
		case TS_WACK_DREQ11:
			tp_set_state(tp, TS_WREQ_ORDREL);
			break;
		case TS_WACK_UREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_OPTREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		default:
			/* Not in a wack state but this is typicall as
			   T_OPTMGMT_REQ does note change the state to
			   TS_WACK_OPTREQ when the original state was not
			   TS_IDLE. */
			break;
		}
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_ok_ack:- issue T_OK_ACK message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 * @prim: correct primtive
 */
static fastcall noinline __unlikely
tp_ok_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t prim)
{
	struct T_ok_ack *p;
	mblk_t *mp;

	(void) tp_ok_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OK_ACK");
		switch (tp_get_state(tp)) {
		case TS_WACK_CREQ:
			tp_set_state(tp, TS_WCON_CREQ);
			break;
		case TS_WACK_CRES:
			tp_set_state(tp, TS_DATA_XFER);
			if (tp->coninds-- == tp->maxinds) {
				qenable(RD(q));
				qenable(WR(q));
			}
			break;
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_UREQ:
			tp_set_state(tp, TS_UNBND);
			break;
		default:
			break;
		}
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_unitdata_ind:- issue T_UNITDATA_IND message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall inline __hot_get
tp_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t src_ptr, size_t src_len,
		caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_unitdata_ind *p;
	mblk_t *mp;

	(void) tp_unitdata_ind;
	if ((mp = mi_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNITDATA_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + src_len;
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_UNITDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_wuderror_ind:- issue T_UDERROR_IND message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall noinline __unlikely
tp_uderror_ind(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t ds_len,
	       caddr_t opt_ptr, size_t opt_len, t_scalar_t etype, mblk_t *dp)
{
	struct T_uderror_ind *p;
	mblk_t *mp;

	(void) tp_uderror_ind;
	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + dst_len;
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_UDERROR_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_optmgmt_ack:- issue T_OPTMGMT_ACK message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall noinline __unlikely
tp_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len,
	       t_scalar_t flags)
{
	struct T_optmgmt_ack *p;
	mblk_t *mp;

	(void) tp_optmgmt_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = sizeof(*p);
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OPTMGMT_ACK");
		if (tp_get_state(tp) == TS_WACK_OPTREQ)
			tp_set_state(tp, TS_IDLE);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_ordrel_ind:- issue T_ORDREL_IND message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall noinline __unlikely
tp_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct T_ordrel_ind *p;
	mblk_t *mp;

	(void) tp_ordrel_ind;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ORDREL_IND");
			switch (tp_get_state(tp)) {
			case TS_DATA_XFER:
				tp_set_state(tp, TS_WREQ_ORDREL);
				break;
			case TS_WIND_ORDREL:
				tp_set_state(tp, TS_IDLE);
				break;
			}
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_optdata_ind:- issue T_OPTDATA_IND message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall inline __hot_get
tp_optdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flags, caddr_t opt_ptr,
	       size_t opt_len, mblk_t *dp)
{
	struct T_optdata_ind *p;
	mblk_t *mp;

	(void) tp_optdata_ind;
	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_IND;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OPTDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_addr_ack:- issue T_ADDR_ACK message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall noinline __unlikely
tp_addr_ack(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t loc_ptr, size_t loc_len,
	    caddr_t rem_ptr, size_t rem_len)
{
	struct T_addr_ack *p;
	mblk_t *mp;

	(void) tp_addr_ack;
	if ((mp = mi_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = sizeof(*p);
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = sizeof(*p) + loc_len;
		mp->b_wptr += sizeof(*p);
		bcopy(loc_ptr, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
		bcopy(rem_ptr, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ADDR_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_capability_ack:- issue T_CAPABILITY_ACK message
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon susccess (or NULL)
 */
static fastcall noinline __unlikely
tp_capability_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_capability_ack *p;
	mblk_t *mp;

	(void) tp_capability_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = DB_TYPE(msg);
		if (DB_TYPE(mp) == M_PCPROTO || bcanputnext(tp->oq, 0)) {
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CAPABILITY_ACK;
			p->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
			p->INFO_ack = tp->proto.info;
			p->ACCEPTOR_id = tp->token;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CAPABILITY_ACK");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * NPI User -> NPI Provider issued primtiives
 * (Primtives issued downstream.)
 *
 * --------------------------------------------------------------------------
 */

/**
 * np_conn_req: - issued N_CONN_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_conn_req(struct np *np, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len, np_ulong flags,
	    caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	N_conn_req_t *p;
	mblk_t *mp;

	(void) np_conn_req;
	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + qos_len, BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p) + dst_len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_CONN_REQ");
			np_set_state(np, NS_WCON_CREQ);
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_conn_res: - issued N_CONN_RES message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_conn_res(struct np *np, queue_t *q, mblk_t *msg, caddr_t res_ptr, size_t res_len,
	    np_ulong sequence, np_ulong flags, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	N_conn_res_t *p;
	mblk_t *mp;

	(void) np_conn_res;
	if ((mp = mi_allocb(q, sizeof(*p) + res_len + qos_len, BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_RES;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = sequence;
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p) + res_len;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_CONN_RES");
			np_set_state(np, NS_DATA_XFER);
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_discon_req: - issued N_DISCON_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_discon_req(struct np *np, queue_t *q, mblk_t *msg, np_ulong reason, caddr_t res_ptr,
	      size_t res_len, np_ulong sequence, mblk_t *dp)
{
	N_discon_req_t *p;
	mblk_t *mp;

	(void) np_discon_req;
	if ((mp = mi_allocb(q, sizeof(*p) + res_len, BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_REQ;
			p->DISCON_reason = reason;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = sequence;
			mp->b_wptr += sizeof(*p);
			bcopy(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DISCON_REQ");
			switch (np_get_state(np)) {
			case NS_WCON_CREQ:
				np_set_state(np, NS_WACK_DREQ6);
				break;
			case NS_WACK_CRES:
				np_set_state(np, NS_WACK_DREQ7);
				break;
			case NS_DATA_XFER:
				np_set_state(np, NS_WACK_DREQ9);
				break;
			case NS_WCON_RREQ:
				np_set_state(np, NS_WACK_DREQ10);
				break;
			case NS_WACK_RRES:
				np_set_state(np, NS_WACK_DREQ11);
				break;
			default:
				break;
			}
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_data_req: - issued N_DATA_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot_out
np_data_req(struct np *np, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;

	(void) np_data_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DATA_REQ");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_exdata_req: - issued N_EXDATA_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: user data
 */
static fastcall inline __hot_out
np_exdata_req(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_req_t *p;
	mblk_t *mp;

	(void) np_exdata_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_EXDATA_REQ");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_info_req: - issued N_INFO_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_info_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_info_req_t *p;
	mblk_t *mp;

	(void) np_info_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_INFO_REQ");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_bind_req: - issued N_BIND_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_bind_req(struct np *np, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len,
	    np_ulong coninds, np_ulong flags, caddr_t pro_ptr, size_t pro_len)
{
	N_bind_req_t *p;
	mblk_t *mp;

	(void) np_bind_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_BIND_REQ;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			p->CONIND_number = coninds;
			p->BIND_flags = flags;
			p->PROTOID_length = pro_len;
			p->PROTOID_offset = sizeof(*p) + add_len;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			bcopy(pro_ptr, mp->b_wptr, pro_len);
			mp->b_wptr += pro_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_BIND_REQ");
			np_set_state(np, NS_WACK_BREQ);
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_unbind_req: - issued N_UNBIND_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_unbind_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	(void) np_unbind_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_UNBIND_REQ");
			np_set_state(np, NS_WACK_UREQ);
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_unitdata_req: - issued N_UNITDATA_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot_out
np_unitdata_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_unitdata_req_t *p;
	mblk_t *mp;

	(void) np_unitdata_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_UNITDATA_REQ");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_optmgmt_req: - issued N_OPTMGMT_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;

	(void) np_optmgmt_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_OPTMGMT_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_OPTMGMT_REQ");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_datack_req: - issued N_DATACK_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot_out
np_datack_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_datack_req_t *p;
	mblk_t *mp;

	(void) np_datack_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DATACK_REQ");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_reset_req: - issued N_RESET_REQ message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_reset_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_reset_req_t *p;
	mblk_t *mp;

	(void) np_reset_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_RESET_REQ");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_reset_res: - issued N_RESET_RES message
 * @np: network user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely
np_reset_res(struct np *np, queue_t *q, mblk_t *msg)
{
	N_reset_res_t *p;
	mblk_t *mp;

	(void) np_reset_res;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_RES;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_RESET_RES");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freemsg(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * NPI Provider -> NPI User (primitives from below)
 *
 * --------------------------------------------------------------------------
 */

/**
 * np_conn_ind: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_conn_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_conn_con: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_conn_con(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_discon_ind: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_discon_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_data_ind: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall inline __hot_in int
np_data_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_exdata_ind: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall inline __hot_in int
np_exdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_info_ack: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_info_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_bind_ack: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_bind_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_error_ack: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_error_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_ok_ack: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_ok_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_unitdata_ind: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall inline __hot_in int
np_unitdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_uderror_ind: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_uderror_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_datack_ind: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall inline __hot_in int
np_datack_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_reset_ind: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_reset_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_reset_con: - process N_CONN_IND message
 * @np: network user private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND message
 */
static fastcall noinline __unlikely int
np_reset_con(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI User -> TPI Provider (primitives from above)
 *
 * --------------------------------------------------------------------------
 */

/**
 * tp_conn_req: - process T_CONN_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_CONN_REQ message
 */
static fastcall noinline __unlikely int
tp_conn_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_conn_res: - process T_CONN_RES primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_CONN_RES message
 */
static fastcall noinline __unlikely int
tp_conn_res(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_discon_req: - process T_DISCON_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_DISCON_REQ message
 */
static fastcall noinline __unlikely int
tp_discon_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_data_req: - process T_DATA_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_DATA_REQ message
 */
static fastcall inline __hot_put int
tp_data_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_exdata_req: - process T_EXDATA_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_EXDATA_REQ message
 */
static fastcall inline __hot_put int
tp_exdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_info_req: - process T_INFO_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_INFO_REQ message
 */
static fastcall noinline __unlikely int
tp_info_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_bind_req: - process T_BIND_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_BIND_REQ message
 *
 * Each transport address consists of a TSAP and an NSAP.
 */
static fastcall noinline __unlikely int
tp_bind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->ADDR_length, p->ADDR_offset))
		goto badaddr;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_unbind_req: - process T_UNBIND_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_UNBIND_REQ message
 */
static fastcall noinline __unlikely int
tp_unbind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_unitdata_ind: - process T_UNITDATA_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_UNITDATA_REQ message
 */
static fastcall inline __hot_put int
tp_unitdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_optmgmt_req: - process T_OPTMGMT_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_OPTMGMT_REQ message
 */
static fastcall noinline __unlikely int
tp_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_ordrel_req: - process T_ORDREL_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_ORDREL_REQ message
 */
static fastcall noinline __unlikely int
tp_ordrel_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_optdata_req: - process T_OPTDATA_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_OPTDATA_REQ message
 */
static fastcall inline __hot_put int
tp_optdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_addr_req: - process T_ADDR_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_ADDR_REQ message
 */
static fastcall noinline __unlikely int
tp_addr_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_capability_req: - process T_CAPABILITY_REQ primitmive
 * @tp: transport provider private structure
 * @q: active queue (write queue)
 * @mp: the T_CAPABILITY_REQ message
 */
static fastcall noinline __unlikely int
tp_capability_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_length, p->RES_offset))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

/**
 * np_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static fastcall int
np_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: M_DATA message on read queue", __FUNCTION__);
	return (0);
}

/**
 * np_m_proto: - process M_PROTO message
 * @q: active queue (read queue)
 * @mp: the M_PROTO message
 */
static fastcall inline __hot_get int
np_m_proto(queue_t *q, mblk_t *mp)
{
	np_ulong prim;
	struct np *np;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto tooshort;
	prim = *(np_ulong *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == -NULL)
		goto edeadlk;
	np = NP_PRIV(q);
	np_save_state(np);
	tp_save_state(np->tp);
	switch (prim) {
	case N_CONN_IND:
		rtn = np_conn_ind(np, q, mp);
		break;
	case N_CONN_CON:
		rtn = np_conn_con(np, q, mp);
		break;
	case N_DISCON_IND:
		rtn = np_discon_ind(np, q, mp);
		break;
	case N_DATA_IND:
		rtn = np_data_ind(np, q, mp);
		break;
	case N_EXDATA_IND:
		rtn = np_exdata_ind(np, q, mp);
		break;
	case N_INFO_ACK:
		rtn = np_info_ack(np, q, mp);
		break;
	case N_BIND_ACK:
		rtn = np_bind_ack(np, q, mp);
		break;
	case N_ERROR_ACK:
		rtn = np_error_ack(np, q, mp);
		break;
	case N_OK_ACK:
		rtn = np_ok_ack(np, q, mp);
		break;
	case N_UNITDATA_IND:
		rtn = np_unitdata_ind(np, q, mp);
		break;
	case N_UDERROR_IND:
		rtn = np_uderror_ind(np, q, mp);
		break;
	case N_DATACK_IND:
		rtn = np_datack_ind(np, q, mp);
		break;
	case N_RESET_IND:
		rtn = np_reset_ind(np, q, mp);
		break;
	case N_RESET_CON:
		rtn = np_reset_con(np, q, mp);
		break;
/*
 * None of the following messages should appear at the read-side queue.  Only
 * upward defined NPI messages should appear at the read-side queue.  Discard
 * these.
 */
	case N_CONN_REQ:
	case N_CONN_RES:
	case N_DISCON_REQ:
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_INFO_REQ:
	case N_BIND_REQ:
	case N_UNBIND_REQ:
	case N_UNITDATA_REQ:
	case N_OPTMGMT_REQ:
	case N_DATACK_REQ:
	case N_RESET_REQ:
	case N_RESET_RES:
		goto wrongway;
	default:
		rtn = np_other_ind(np, q, mp);
		break;
	}
	if (rtn) {
		tp_restore_state(np->tp);
		np_restore_state(np);
	}
	mi_unlock(priv);
	return (rtn);
      wrongway:
	mi_unlock(priv);
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding message in wrong direction",
		  __FUNCTION__);
	return (0);
      edeadlk:
	return (-EDEADLK);
      tooshort:
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short", __FUNCTION__);
	return (0);
}

/**
 * np_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (read queue)
 * @mp the M_SIG or M_PCSIG message
 */
static fastcall int
np_m_sig(queue_t *q, mblk_t *mp)
{
	struct np *np;
	caddr_t *priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(int)))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);
	if (!mi_timer_valid(mp))
		return (0);
	np = NP_PRIV(q);
	np_save_state(np);
	switch (*(int *) mp->b_rptr) {
#if 0-
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = np_t1_timeout(np, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding undefined timeout %d",
			  __FUNCTION__, *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		np_restore_state(np);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
	/* These are either timer messages (in which case they have a length
	   greater than or equal to 4) or true signal messages (in which case
	   they have a length of 1). Therefore, if the length of the message is 
	   less than sizeof(int) we pass them along. */
      passalong:
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_sig: - process M_COPYIN or M_COPYOUT message
 * @q: active queue (read queue)
 * @mp the M_COPYIN or M_COPYOUT message
 */
static fastcall int
np_m_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_sig: - process M_IOCACK or M_IOCNAK message
 * @q: active queue (read queue)
 * @mp the M_IOCACK or M_IOCNAK message
 */
static fastcall int
np_m_iocack(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_sig: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp the M_FLUSH message
 */
static fastcall int
np_m_flush(queue_t *q, mblk_t *mp)
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
 * np_m_sig: - process M_ERROR or M_HANGUP message
 * @q: active queue (read queue)
 * @mp the M_ERROR or M_HANGUP message
 */
static fastcall int
np_m_error(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * np_m_sig: - process unrecognized STREAMS message
 * @q: active queue (read queue)
 * @mp the unrecognized STREAMS message
 */
static fastcall int
np_m_unrec(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_msg: - process STREAMS message
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static fastcall inline int
np_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return np_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return np_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return np_m_sig(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return np_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return np_m_iocack(q, mp);
	case M_FLUSH:
		return np_m_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return np_m_error(q, mp);
	default:
		return np_m_unrec(q, mp);
	}
}

/**
 * tp_m_unrec: - process M_DATA message
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 */
static fastcall int
tp_m_data(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_m_unrec: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall int
tp_m_proto(queue_t *q, mblk_t *mp)
{
	t_scalar_t prim;
	struct tp *tp;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto tooshort;
	prim = *(t_scalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	tp = TP_PRIV(q);
	tp_save_state(tp);
	np_save_state(tp->np);
	switch (prim) {
	case T_CONN_REQ:
		rtn = tp_conn_req(tp, q, mp);
		break;
	case T_CONN_RES:
		rtn = tp_conn_res(tp, q, mp);
		break;
	case T_DISCON_REQ:
		rtn = tp_discon_req(tp, q, mp);
		break;
	case T_DATA_REQ:
		rtn = tp_data_req(tp, q, mp);
		break;
	case T_EXDATA_REQ:
		rtn = tp_exdata_req(tp, q, mp);
		break;
	case T_INFO_REQ:
		rtn = tp_info_req(tp, q, mp);
		break;
	case T_BIND_REQ:
		rtn = tp_bind_req(tp, q, mp);
		break;
	case T_UNBIND_REQ:
		rtn = tp_unbind_req(tp, q, mp);
		break;
	case T_UNITDATA_REQ:
		rtn = tp_unitdata_req(tp, q, mp);
		break;
	case T_OPTMGMT_REQ:
		rtn = tp_optmgmt_req(tp, q, mp);
		break;
	case T_ORDREL_REQ:
		rtn = tp_ordrel_req(tp, q, mp);
		break;
	case T_OPTDATA_REQ:
		rtn = tp_optdata_req(tp, q, mp);
		break;
	case T_ADDR_REQ:
		rtn = tp_addr_req(tp, q, mp);
		break;
	case T_CAPABILITY_REQ:
		rtn = tp_capability_req(tp, q, mp);
		break;
/*
 * None of the following messages should appear at the write-side queue.  Only
 * downwardcaseTPI messages should appear at the write-side queue.  Discard
 * these.
 */
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
	case T_CAPABILITY_ACK:
		goto wrongway;
	default:
		rtn = tp_other_req(tp, q, mp);
		break;
	}
	if (rtn) {
		np_restore_state(tp->np);
		tp_restore_state(tp);
	}
	mi_unlock(priv);
	return (rtn);
      wrongway:
	mi_unlock(priv);
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding message in wrong direction",
		  __FUNCTION__);
	return (0);
      edeadlk:
	return (-EDEADLK);
      tooshort:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short", __FUNCTION__);
	freemsg(mp);
	return (0);
}

/**
 * tp_m_unrec: - process M_SIG or M_PCSIG STREAMS message
 * @q: active queue (write queue)
 * @mp: the M_SIG or M_PCSIG STREAMS message
 */
static fastcall int
tp_m_sig(queue_t *q, mblk_t *mp)
{
	struct tp *tp;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(int))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	if (!mi_timer_valid(mp))
		goto done;
	tp = TP_PRIV(q);
	tp_save_state(tp);
	switch (*(int *) mp->b_rptr) {
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = tp_t1_timeout(tp, q, mp);
		break;
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		tp_restore_state(tp);
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
	/* These are either timer messages (in which case they have a length
	   greater than or equal to 4) or true signal messages (in which case
	   they have a length of 1). Therefore, if the length of the message is 
	   less than sizeof(int) we pass them along. */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tp_m_unrec: - process M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static fastcall int
tp_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tp *tp;
	caddr_t priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != TP0_IOC_MAGIC)
		goto notforus;
	if (!mp->b_cont)
		goto efault;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	tp = TP_PRIV(q);
	err = tp_ioctl(tp, q, mp);
	mi_unlock(priv);
	return (err);
      edeadlk:
	return (-EDEADLK);
      efault:
	mi_copy_done(q, mp, EFAULT);
	return (0);
      notforus:
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_unrec: - process M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static fastcall int
tp_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != TP0_IOC_MAGIC)
		goto notforus;
	if (!mp->b_cont)
		goto efault;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	tp = TP_PRIV(q);
	switch (mi_copy_state(q, mp, &dp)) {
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = tp_copyin1(tp, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		err = tp_copyin2(tp, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = tp_copyout(tp, q, mp, dp);
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
      efault:
	mi_copy_done(q, mp, EFAULT);
	return (0);
      notforus:
	/* Let driver deal with these. */
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_unrec: - process M_FLUSH message
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 */
static fastcall int
tp_m_flush(queue_t *q, mblk_t *mp)
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
 * tp_m_unrec: - process unrecognized STREAMS message
 * @q: active queue (write queue)
 * @mp: the unrecognized STREAMS message
 */
static fastcall int
tp_m_unrec(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tp_msg: - process STREAMS message
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static streams_fastcall streams_inline int
tp_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return tp_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tp_m_sig(q, mp);
	case M_IOCTL:
		return tp_m_ioctl(q, mp);
	case M_IOCDATA:
		return tp_m_iocdata(q, mp);
	case M_FLUSH:
		return tp_m_flush(q, mp);
	default:
		return tp_m_unrec(q, mp);
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
np_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || np_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
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
static streamscall __hot_read int
tp_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tp_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_in int
tp_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tp_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * QUEUE OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */

static caddr_t tp0_opens = NULL;

static streamscall int
tp0_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	struct priv *p;
	struct tp *tp;
	struct np *np;
	mblk_t *mp;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(N_info_req_t), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&tp0_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	p = PRIV(q);
	tp = TP_PRIV(q);
	np = NP_PRIV(q);

	/* initialize private structure */
	tp->priv = p;
	tp->np = np;
	tp->oq = WR(q);
	tp->state = TS_UNBND;
	tp->oldstate = TS_UNBND;

	tp->proto.info.PRIM_type = T_INFO_ACK;
	tp->proto.info.TSDU_size = -1;
	tp->proto.info.ETSDU_size = 1;
	tp->proto.info.CDATA_size = 0;
	tp->proto.info.DDATA_size = 0;
	tp->proto.info.ADDR_size = sizeof(struct sockaddr_in);
	tp->proto.info.OPT_size = -1;
	tp->proto.info.TIDU_size = 4096;
	tp->proto.info.SERV_type = T_COTS_ORD;
	tp->proto.info.CURRENT_state = TS_UNBND;
	tp->proto.info.PROVIDER_flag = T_XPG4_1 & ~T_SENDZERO;

	tp->proto.addr.PRIM_type = T_ADDR_ACK;
	tp->proto.addr.LOCADDR_length = 0;
	tp->proto.addr.LOCADDR_offset = sizeof(tp->proto.addr);
	tp->proto.addr.REMADDR_length = 0;
	tp->proto.addr.REMADDR_offset = sizeof(tp->proto.addr) + sizeof(tp->proto.loc);

	tp->proto.loc = (struct sockaddr_osi) {
		AF_OSI, 0, {
	0x000000000}};
	tp->proto.rem = (struct sockaddr_osi) {
		AF_OSI, 0, {
	0x000000000}};

	np->priv = p;
	np->tp = tp;
	np->oq = RD(q);
	np->state = NS_UNBND;
	np->oldstate = NS_UNBND;

	np->proto.info.PRIM_type = N_INFO_ACK;
	np->proto.info.NSDU_size = 65532;
	np->proto.info.CDATA_size = N_UNDEFINED;
	np->proto.info.DDATA_size = N_UNDEFINED;
	np->proto.info.ADDR_size = 9;
	np->proto.info.ADDR_length = 0;
	np->proto.info.ADDR_offset = sizeof(np->proto.info);
	np->proto.info.QOS_length = sizeof(np->qos);
	np->proto.info.QOS_offset = sizeof(np->proto.info) + sizeof(np->add);
	np->proto.info.QOS_range_length = sizeof(np->qor);
	np->proto.info.QOS_range_offset =
	    sizeof(np->proto.info) + sizeof(np->add) + sizeof(np->qos);
	np->proto.info.OPTIONS_flags = 0;
	np->proto.info.NIDU_size 4096;
	np->proto.info.SERV_type = N_CONS;
	np->proto.info.CURRENT_state = TS_UNBND;
	np->proto.info.PROVIDER_type = N_SNICFP;
	np->proto.info.NODU_size = 4096;
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
	np->proto.qos.src_throughput.thru_targ_value = -1;
	np->proto.qos.src_throughput.thru_min_value = -1;
	np->proto.qos.dest_throughput.thru_targ_value = -1;
	np->proto.qos.dest_throughput.thru_min_value = -1;
	np->proto.qos.transit_delay_t.td_targ_value = -1;
	np->proto.qos.transit_delay_t.td_max_value = -1;
	np->proto.qos.nc_estab_delay = -1;
	np->proto.qos.nc_estab_fail_prob = -1;
	np->proto.qos.residual_error_rate = -1;
	np->proto.qos.xfer_fail_prob = -1;
	np->proto.qos.nc_resilience = -1;
	np->proto.qos.nc_rel_delay = -1;
	np->proto.qos.nc_rel_fail_prob = -1;
	np->proto.qos.protection_sel = -1;
	np->proto.qos.priority_sel = -1;
	np->proto.qos.max_accept_cost = -1;

	np->proto.qor.n_qos_type = N_QOS_CO_OPT_RANGE;
	np->proto.qor.src_throughput.thru_targ_value = -1;
	np->proto.qor.src_throughput.thru_min_value = -1;
	np->proto.qor.dest_throughput.thru_targ_value = -1;
	np->proto.qor.dest_throughput.thru_min_value = -1;
	np->proto.qor.transit_delay_t.td_targ_value = -1;
	np->proto.qor.transit_delay_t.td_max_value = -1;
	np->proto.qor.nc_estab_delay = -1;
	np->proto.qor.nc_estab_fail_prob = -1;
	np->proto.qor.residual_error_rate = -1;
	np->proto.qor.xfer_fail_prob = -1;
	np->proto.qor.nc_resilience = -1;
	np->proto.qor.nc_rel_delay = -1;
	np->proto.qor.nc_rel_fail_prob = -1;
	np->proto.qor.protection_range.protect_targ_value = -1;
	np->proto.qor.protection_range.protect_min_value = -1;
	np->proto.qor.priority_range.priority_targ_value = -1;
	np->proto.qor.priority_range.priority_min_value = -1;
	np->proto.qor.max_accept_cost = -1;

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((N_info_req_t *) mp->b_wptr)->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(N_info_req_t);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static streamscall int
tp0_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	(void) p;
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm(&tp0_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit tp0_rinit = {
	.qi_putp = np_put,		/* Read put (message from below) */
	.qi_srvp = np_srv,		/* Read service */
	.qi_qopen = &tp0_open,		/* Each open */
	.qi_qclose = &tp0_close,	/* Last close */
	.qi_minfo = &tp0_minfo,		/* Module information */
	.qi_mstat = &tp0_rstat,		/* Module statistics */
};

static struct qinit tp0_winit = {
	.qi_putp = tp_put,		/* Write put (message form above) */
	.qi_srvp = tp_srv,		/* Write service */
	.qi_minfo = &tp0_minfo,		/* Module information */
	.qi_mstat = &tp0_wstat,		/* Module statistics */
};

struct streamtab tp0_info = {
	.st_rdinit = &tp0_rinit,
	.st_wrinit = &tp0_winit,
};

static modID_t modid = MOD_ID;

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
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for TP0. (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static __init int
tp0_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&tp0_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
tp0_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&tp0_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(tp0_modinit);
module_exit(tp0_modexit);

#endif				/* LINUX */
