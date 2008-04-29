/*****************************************************************************

 @(#) $RCSfile: isot.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-29 00:02:05 $

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

 Last Modified $Date: 2008-04-29 00:02:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: isot.c,v $
 Revision 0.9.2.6  2008-04-29 00:02:05  brian
 - updated headers for release

 Revision 0.9.2.5  2008-04-25 11:50:52  brian
 - updates to AGPLv3

 Revision 0.9.2.4  2008-04-25 08:38:32  brian
 - working up libraries modules and drivers

 Revision 0.9.2.3  2007/08/14 07:05:15  brian
 - GNUv3 header update

 Revision 0.9.2.2  2006/10/10 10:44:15  brian
 - updates for release, lots of additions and workup

 Revision 0.9.2.1  2006/07/11 12:32:05  brian
 - added ISO and other implementations to distribution

 Revision 0.9.2.1  2006/04/11 18:30:10  brian
 - added new ISO over TCP modules

 *****************************************************************************/

#ident "@(#) $RCSfile: isot.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-29 00:02:05 $"

static char const ident[] =
    "$RCSfile: isot.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-29 00:02:05 $";

/*
 *  ISO Transport over TCP/IP (ISOT)
 *
 *  ISOT implements ISO Transport Protocol Class 0 (TP0) over TCP/IP in
 *  accordance with RFC 1006.  This module implements the upper layer protocol
 *  module that constitutes a specialized ISO NS-Provider that implements the
 *  ISOT protocol to TCP.  This module presents an ISO NPI interface to the
 *  NS-User above it.  The NS-User above is expected to be a ISO Transport
 *  Protocol Class 0 (TP0).
 */

#if 0
#include <sys/os7/compat.h>

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/npi.h>

#define ISOT_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define ISOT_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define ISOT_REVISION	"OpenSS7 $RCSfile: isot.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-29 00:02:05 $"
#define ISOT_DEVICE	"SVR 4.2 STREAMS ISOT Module for RFC 1006 and RFC 2126"
#define ISOT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ISOT_LICENSE	"GPL"
#define ISOT_BANNER	ISOT_DESCRIP	"\n" \
			ISOT_COPYRIGHT	"\n" \
			ISOT_REVISION	"\n" \
			ISOT_DEVICE	"\n" \
			ISOT_CONTACT
#define ISOT_SPLASH	ISOT_DEVICE	" - " \
			ISOT_REVISION

#ifdef LINUX
MODULE_AUTHOR(ISOT_CONTACT);
MODULE_DESCRIPTION(ISOT_DESCRIP);
MODULE_SUPPORTED_DEVICE(ISOT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ISOT_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-isot");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_ISOT_NAME
#error "CONFIG_STREAMS_ISOT_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_ISOT_MODID
#error "CONFIG_STREAMS_ISOT_MODID must be defined."
#endif

#ifndef ISOT_MOD_NAME
#define ISOT_MOD_NAME		CONFIG_STREAMS_ISOT_NAME
#endif				/* ISOT_MOD_NAME */
#ifndef ISOT_MOD_ID
#define ISOT_MOD_ID		CONFIG_STREAMS_ISOT_MODID
#endif				/* ISOT_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID	    ISOT_MOD_ID
#define MOD_NAME    ISOT_MOD_NAME
#ifdef MODULE
#define MOD_BANNER  ISOT_BANNER
#else				/* MODULE */
#define MOD_BANNER  ISOT_SPLASH
#endif				/* MODULE */

static struct module_info isot_info = {
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

struct np;
struct tp;
struct priv;

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

struct priv {
	struct tp r_priv;		/* TPI structure on RD side */
	struct np w_priv;		/* NPI structure on WR side */
};

#define PRIV(q)	    ((struct priv *)(q)->q_ptr)

#define TP_PRIV(q)  (&PRIV(q)->w_priv)
#define NP_PRIV(q)  (&PRIV(q)->r_priv)

#define PAD4(len) ((len + 3) & ~3)

#define STRLOGIO	0	/* log Stream input-output controls */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

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

/*
 * --------------------------------------------------------------------------
 *
 * NPI Provider -> NPI User issued primitives
 *
 * --------------------------------------------------------------------------
 */

static streams_fastcall streams_noinline int
m_error(struct np *np, queue_t *q, mblk_t *msg, int err)
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
 * np_conn_ind: - issue N_CONN_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_conn_ind(struct np *np, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len,
	    caddr_t src_ptr, size_t src_len, np_ulong sequence, np_ulong flags, caddr_t qos_ptr,
	    size_t qos_len, mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;

	(void) np_conn_ind;
	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + src_len + qos_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->SEQ_number = sequence;
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_CONN_IND");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_conn_con: - issue N_CONN_CON message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_conn_con(struct np *np, queue_t *q, mblk_t *msg, caddr_t res_ptr,
		size_t res_len, np_ulong flags, caddr_t qos_ptr, size_t qos_len,
		mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;

	(void) np_conn_con;
	if ((mp = mi_allocb(q, sizeof(*p) + res_len + qos_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_CON;
		p->RES_length = res_len;
		p->RES_offset = sizeof(*p);
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = sizeof(*p) + res_len;
		mp->b_wptr += sizeof(*p);
		bcopy(res_ptr, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_CONN_CON");
		np_set_state(np, NS_DATA_XFER);
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_discon_ind: - issue N_DISCON_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_discon_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong origin, np_ulong reason,
	      caddr_t res_ptr, size_t res_len, np_ulong sequence, mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;

	(void) np_discon_ind;
	if ((mp = mi_allocb(q, sizeof(*p) + res_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DISCON_IND;
		p->DISCON_orig = origin;
		p->DISCON_reason = reason;
		p->RES_length = res_len;
		p->RES_offset = sizeof(*p);
		p->SEQ_number = sequence;
		mp->b_wptr += sizeof(*p);
		bcopy(res_ptr, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DISCON_IND");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_data_ind: - issue N_DATA_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_inline __hot_in int
np_data_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;

	(void) np_data_ind;
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
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DATA_IND");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_exdata_ind: - issue N_EXDATA_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_exdata_ind(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;

	(void) np_exdata_ind;
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
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_EXDATA_IND");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_info_ack: - issue N_INFO_ACK message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_info_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;

	(void) np_info_ack;
	if ((mp = mi_allocb(q, sizeof(np->proto), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		bcopy(&np->proto, mp->b_wptr, sizeof(np->proto));
		mp->b_wptr += sizeof(np->proto);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_INFO_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_bind_ack: - issue N_BIND_ACK message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_bind_ack(struct np *np, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len,
	    caddr_t pro_ptr, size_t pro_len, np_ulong token, np_ulong conind)
{
	N_bind_ack_t *p;
	mblk_t *mp;

	(void) np_bind_ack;
	if ((mp = mi_allocb(q, sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = conind;
		p->TOKEN_value = token;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = sizeof(*p) + add_len;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(pro_ptr, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_BIND_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_error_ack: - issue N_ERROR_ACK message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative UNIX or positive NPI error
 * @func: name of originating function
 */
static streams_fastcall streams_noinline __unlikely int
np_error_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, int error, const char *func)
{
	N_error_ack_t *p;
	mblk_t *mp;

	(void) np_error_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = error < 0 ? NSYSERR : error;
		p->UNIX_error = error < 0 ? error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		if (func != NULL)
			mi_strlog(q, 0, SL_TRACE, "%s: %s", func, np_strerror(error));
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
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
			   chagne the state to NS_WACK_OPTREQ when the original 
			   state was not NS_IDLE. */
			break;
		}
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_ok_ack: - issue N_OK_ACK message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_ok_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim)
{
	N_ok_ack_t *p;
	mblk_t *mp;

	(void) np_ok_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_OK_ACK");
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
			   than NS_IDLE will not change the state to
			   NS_WACK_OPTREQ. */
			break;
		}
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_unitdata_ind: - issue N_UNITDATA_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_inline __hot_in int
np_unitdata_ind(struct np *np, queue_t *q, mblk_t *msg, caddr_t src_ptr, size_t src_len,
		caddr_t dst_ptr, size_t dst_len, np_ulong etype, mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;

	(void) np_unitdata_ind;
	if ((mp = mi_allocb(q, sizeof(*p) + src_len + dst_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNITDATA_IND;
		p->SRC_length = src_len;
		p->SRC_offset = sizeof(*p);
		p->DEST_length = dst_len;
		p->DEST_offset = sizeof(*p) + src_len;
		p->ERROR_type = etype;
		mp->b_wptr += sizeof(*p);
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_UNITDATA_IND");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_uderror_ind: - issue N_UDERROR_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dst_ptr: destnation address pointer
 * @dst_len: destnation address length
 * @error: error type
 * @dp: user data
 */
static streams_fastcall streams_noinline __unlikely int
np_uderror_ind(struct np *np, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len,
	       np_ulong error, mblk_t *dp)
{
	N_uderror_ind_t *p;
	mblk_t *mp;

	(void) np_uderror_ind;
	if ((mp = mi_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 1;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UDERROR_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = sizeof(*p);
		p->RESERVED_field = 0;
		p->ERROR_type = error;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_UDERROR_IND");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_datack_ind: - issue N_DATACK_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_inline __hot_out int
np_datack_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;

	(void) np_datack_ind;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 1;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DATACK_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DATACK_IND");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_reset_ind: - issue N_RESET_IND message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_reset_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong origin, np_ulong reason)
{
	N_reset_ind_t *p;
	mblk_t *mp;

	(void) np_reset_ind;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 1;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_IND;
		p->RESET_orig = origin;
		p->RESET_reason = reason;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_RESET_IND");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_reset_con: - issue N_RESET_CON message
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely int
np_reset_con(struct np *np, queue_t *q, mblk_t *msg)
{
	N_reset_con_t *p;
	mblk_t *mp;

	(void) np_reset_con;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_RESET_CON");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI User -> TPI Provider issued primitives
 *
 * --------------------------------------------------------------------------
 */

/**
 * tp_conn_req: - issue T_CONN_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely
tp_conn_req(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len,
	    caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_conn_req *p;
	mblk_t *mp;

	(void) tp_conn_req;
	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CONN_REQ");
		tp_set_state(tp, TS_WACK_CREQ);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_conn_res: - issue T_CONN_RES message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @acceptor: acceptor stream
 * @opt_ptr: pointer to options
 * @opt_len: length of options
 * @sequence: sequence number of connection indication
 * @dp: user data
 */
static streams_fastcall streams_noinline __unlikely
tp_conn_res(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t acceptor, caddr_t opt_ptr,
	    size_t opt_len, t_scalar_t sequence, mblk_t *dp)
{
	struct T_conn_res *p;
	mblk_t *mp;

	(void) tp_conn_res;
	if ((mp = mi_allocb(q, sizeof(*p) + opt_ptr, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_RES;
		p->ACCEPTOR_id = acceptor;
		p->SEQ_number = sequence;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CONN_RES");
		tp_set_state(tp, TS_WACK_CRES);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_discon_req: - issue T_DISCON_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @seq: seqeunce number of rejected connection indication (or zero)
 */
static streams_fastcall streams_noinline __unlikely
tp_discon_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t seq)
{
	struct T_discon_req *p;
	mblk_t *mp;

	(void) tp_discon_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_DISCON_REQ;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_DISCON_REQ");
		switch (tp_get_state(tp)) {
		case TS_WCON_CREQ:
			tp_set_state(tp, TS_WACK_DREQ6);
			break;
		case TS_WRES_CIND:
			tp_set_state(tp, TS_WACK_DREQ7);
			break;
		case TS_DATA_XFER:
			tp_set_state(tp, TS_WACK_DREQ9);
			break;
		case TS_WIND_ORDREL:
			tp_set_state(tp, TS_WACK_DREQ10);
			break;
		case TS_WREQ_ORDREL:
			tp_set_state(tp, TS_WACK_DREQ11);
			break;
		}
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_data_req: - issue T_DATA_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @more: more user data pending
 * @dp: user data
 */
static streams_fastcall streams_inline __hot_out
tp_data_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_data_req *p;
	mblk_t *mp;

	(void) tp_data_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_DATA_REQ;
		p->MORE_flag = (more != 0) ? 1 : 0;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_DATA_REQ");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_exdata_req: - issue T_EXDATA_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @more: flag indicating more data
 * @dp: user data
 */
static streams_fastcall streams_noinline __unlikely
tp_exdata_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_exdata_req *p;
	mblk_t *mp;

	(void) tp_exdata_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 1;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_EXDATA_REQ;
		p->MORE_flag = (more != 0) ? 1 : 0;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_EXDATA_REQ");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_info_req: - issue T_INFO_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely
tp_info_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_req *p;
	mblk_t *mp;

	(void) tp_info_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_INFO_REQ");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_bind_req: - issue T_BIND_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @add_ptr: pointer to address to bind
 * @add_len: length of address to bind
 * @conind: maximum number of outstanding connection indications
 */
static streams_fastcall streams_noinline __unlikely
tp_bind_req(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len,
	    t_uscalar_t conind)
{
	struct T_bind_req *p;
	mblk_t *mp;

	(void) tp_bind_req;
	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = conind;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_BIND_REQ");
		tp_set_state(tp, TS_WACK_BREQ);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_unbind_req: - issue T_UNBIND_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely
tp_unbind_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_unbind_req *p;
	mblk_t *mp;

	(void) tp_unbind_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_UNBIND_REQ");
		tp_set_state(tp, TS_WACK_UREQ);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_unitdata_req: - issue T_UNITDATA_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dst_ptr: pointer to destination address
 * @dst_len: length of destination address
 * @opt_ptr: pointer to options
 * @opt_len: length of options
 * @dp: user data
 */
static streams_fastcall streams_inline __hot_out
tp_unitdata_req(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len,
		caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_unitdata_req *p;
	mblk_t *mp;

	(void) tp_unitdata_req;
	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_UNITDATA_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_UNITDATA_REQ");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_optmgmt_req: - issue T_OPTMGMT_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: management flags (T_NEGOTIATE, etc.)
 * @opt_ptr: pointer to options
 * @opt_len: length of options
 */
static streams_fastcall streams_noinline __unlikely
tp_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flags, caddr_t opt_ptr,
	       size_t opt_len)
{
	struct T_optmgmt_req *p;
	mblk_t *mp;

	(void) tp_optmgmt_req;
	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_REQ;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OPTMGMT_REQ");
		if (tp_get_state(tp) == TS_IDLE)
			tp_set_state(tp, TS_WACK_OPTREQ);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_ordrel_req: - issue T_ORDREL_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely
tp_ordrel_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_ordrel_req *p;
	mblk_t *mp;

	(void) tp_ordrel_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ORDREL_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ORDREL_REQ");
		tp_set_state(tp, TS_WIND_ORDREL);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_optdata_req: - issue T_OPTDATA_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flag: data flags
 * @opt_ptr: the pointer to the options to pass with the message
 * @opt_len: the length of the options to pass with the message
 * @dp: message block containing user data to pass with the message
 */
static streams_fastcall streams_inline __hot_out
tp_optdata_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flag, caddr_t opt_ptr,
	       size_t opt_len, mblk_t *dp)
{
	struct T_optdata_req *p;
	mblk_t *mp;

	(void) tp_optdata_req;
	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTDATA_REQ;
		p->DATA_flag = flag;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OPTDATA_REQ");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_addr_req: - issue T_ADDR_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static streams_fastcall streams_noinline __unlikely
tp_addr_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_addr_req *p;
	mblk_t *mp;

	(void) tp_addr_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ADDR_REQ");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_capability_req: - issue T_CAPABILITY_REQ message
 * @tp: transport user private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @bits: capability bits (TC1_INFO or TC1_ACCEPTOR_ID)
 *
 */
static streams_fastcall streams_noinline __unlikely
tp_capability_req(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t bits)
{
	struct T_capability_req *p;
	mblk_t *mp;

	(void) tp_capability_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_REQ;
		p->CAP_bits1 = bits;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CAPABILITY_REQ");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * NPI User -> NPI Provider primitives from above
 *
 * --------------------------------------------------------------------------
 */

/**
 * np_conn_req: - process N_CONN_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_CONN_REQ message
 *
 * An N_CONN_REQ translates to a T_CONN_REQ.  Note that we do no permit data to
 * be sent with the N_CONN_REQ, so there is no concern about TCP's inability to
 * send data with the T_CONN_REQ.
 */
static streams_fastcall streams_noinline int
np_conn_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	caddr_t nsp_ptr, dst_ptr, qos_ptr, opt_ptr;
	size_t nsp_len, dst_len, qos_len, opt_len;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	nsp_ptr = (caddr_t) (mp->b_rptr + p->DEST_offset);
	nsp_len = (size_T) p->DEST_length;
	qos_ptr = (caddr_t) (mp->b_rptr + p->QOS_offset);
	qos_len = (size_t) p->QOS_length;
	/* cannot handle receipt confirmation nor expedited data */
	if (p->CONN_flags != 0)
		goto badflag;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	/* Network address and TCP IP address had different formats.  The
	   former is in OSI address format (sockaddr_iso), the latter is in TCP 
	   address format (sockaddr_in).  Conversion needs to be performed. */
	/* The Network QOS and TCP options are in different formats.  The
	   former is in OSI QOS format, the latter is in XTI option format.
	   Conversion needs to be performed. */
	np_set_state(np < NS_WCON_CREQ);
	return (tp_conn_req(tp, q, mp, dst_ptr, dst_len, opt_ptr, opt_len, NULL));
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
	goto rerror;
      error:
	return (np_error_ack(np, q, mp, N_CONN_REQ, err, __FUNCTION__));
}

/**
 * np_conn_res: - process N_CONN_RES message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_CONN_RES message
 *
 * This is not a very likely state of affairs unless the application is making
 * network connections, sending a couple of messages, and then dropping them,
 * which is certainly a possibility, although the transport layer should avoid
 * this (by keeping the network connection open for some time after the
 * transport connection has been dropped).
 *
 * Responding addresses are an issue.  We cannot support responding addresses
 * unless they just happen to be identical to the local address that received
 * the connection indication (that is, the DEST in the N_CONN_IND).  There is
 * not even a place to put a responding address in the T_CONN_RES.
 */
static streams_fastcall streams_noinline int
np_conn_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	caddr_t rsp_ptr, res_ptr, qos_ptr, opt_ptr;
	size_t rsp_len, res_len, qos_len, opt_len;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_offset, p->RES_length))
		goto badaddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	rsp_ptr = (caddr_t) (mp->b_rptr + p->RES_offset);
	rsp_len = (size_t) p->RES_length;
	if (rsp_len > 0)
		goto badaddr;
	qos_ptr = (caddr_t) (mp->b_rptr + p->QOS_offset);
	qos_len = (size_t) p->QOS_length;
	if (p->CONN_flags != 0)
		goto badflags;
	if (np_get_state(np) != NS_WRES_CIND)
		goto outstate;
	/* rsp_ptr and rsp_len need to be converted to res_ptr and res_len, and
	   qos_ptr and qos_len need to be converted to opt_ptr and opt_len. */
	/* TOKEN_value can run straight across, also SEQ_number, but we do need
	   to figure out which Stream and connection indication they correspond
	   to.  For NPI each connection indication is simply a list of
	   T_CONN_IND messages for each of the connection indications. */
	np_set_state(np, NS_WACK_CRES);
	return tp_conn_res(np->tp, q, mp, p->ACCEPTOR_id, opt_ptr, opt_len, p->SEQ_number,
			   mp->b_cont);
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflags:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_CONN_RES, err, __FUNCTION__));
}

/**
 * np_discon_req: - process N_DISCON_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_DISCON_REQ message
 *
 * DISCON_reason is ignored.  RES_length and RES_offset are only meaningful in
 * the NS_WRES_CIND state.  The responding address cannot be different from the
 * local address that was contained in the N_CONN_IND.  For now we simply
 * require that no responding address be supplied.
 */
static streams_fastcall streams_noinline int
np_discon_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	caddr_t res_ptr;
	size_t res_len;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_offset, p->RES_length))
		goto badaddr;
	res_ptr = (caddr_t) (mp->b_rptr + p->RES_offset);
	res_len = (size_t) p->RES_length;
	/* Ignore DISCON_reason. */
	switch (np_get_state(np)) {
	case NS_WCON_CREQ:
		if (res_len > 0)
			goto badaddr;
		np_set_state(np, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		np_set_state(np, NS_WACK_DREQ7);
		if (res_len > 0)
			goto badaddr;
		break;
	case NS_DATA_XFER:
		if (res_len > 0)
			goto badaddr;
		np_set_state(np, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		if (res_len > 0)
			goto badaddr;
		np_set_state(np, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		if (res_len > 0)
			goto badaddr;
		np_set_state(np, NS_WACK_DREQ11);
		break;
	default:
		goto outstate;
	}
	return tp_discon_req(np->tp, q, mp, p->SEQ_number);
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_DISCON_REQ, err, __FUNCTION__));
}

struct tpkt {
	unsigned char vrsn;
	unsigned char reserved;
	unsigned short length;
};

/**
 * np_data_req: - process N_DATA_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_DATA_REQ message
 *
 * This is a simple translation of an N_DATA_REQ into a T_DATA_REQ, with the
 * TPKT structure surrounding the data.
 */
static streams_fastcall streams_inline int
np_data_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p = (typeof(p)) mp->b_rptr;
	struct tpkt *tpkt;
	size_t length;
	mblk_t *dp;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (p->DATA_xfer_flags & ~(N_MORE_DATA_FLAG | N_RC_FLAG))
		goto badflag;
	if (p->DATA_xfer_flags & N_RC_FLAG)
		goto badflag;
	switch (np_get_state(np)) {
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WCON_RREQ:
		break;
	default:
		goto outstate;
	}
	length = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	if (length > np->info.NSDU_size)
		goto baddata;
	if ((dp = mi_allocb(q, sizeof(*tpkt), BPRI_MED)) == NULL)
		goto enobufs;
	tpkt = (typeof(tpkt)) dp->b_wptr;
	dp->b_wptr += sizeof(*tpkt);
	tpkt->vrsn = 3;
	tpkt->reserved = 0;
	tpkt->length = length;
	dp->b_cont = mp->b_cont;
	mp->b_cont = NULL;
	if ((err = tp_data_req(np->tp, q, mp, p->DATA_xfer_flags, dp)) == 0)
		return (0);
	mp->b_cont = dp->b_cont;
	freeb(dp);
      done:
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto done;
      badflag:
	err = NBADFLAG;
	goto error;
      baddata:
	err = NBADDATA;
	goto error;
      oustate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np, q, mp, EPROTO));
}

/**
 * np_exdata_req: - process N_EXDATA_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_EXDATA_REQ message
 */
static streams_fastcall streams_noinline int
np_exdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
}

/**
 * np_info_req: - process N_INFO_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_INFO_REQ message
 */
static streams_fastcall streams_noinline int
np_info_req(struct np *np, queue_t *q, mblk_t *mp)
{
	return (np_info_ack(np, q, mp));
}

/**
 * np_bind_req: - process N_BIND_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_BIND_REQ message
 */
static streams_fastcall streams_noinline int
np_bind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in sin;
	caddr_t add_ptr;
	size_t add_len;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->ADDR_offset, p->ADDR_length))
		goto badaddr;
	if (!MBLKIN(mp, p->PROTOID_offset, p->PROTOID_length))
		goto badproto;
	if (p->BIND_flags & ~(DEFAULT_LISTENER | TOKEN_REQUEST | DEFAULT_DEST))
		goto badopt;
	if (p->BIND_flags & DEFAULT_DEST)
		goto badopt;
	if (p->BIND_flags & DEFAULT_LISTENER)
		goto bound;
	if (np_get_state(np) != NS_UNBND)
		goto outstate;
	sin.sin_family = AF_INET;
	sin.sin_port = 0;
	sin.sin_addr.in_addr = 0x00000000;
	add_ptr = (caddr_t) (mp->b_rptr + p->ADDR_offset);
	add_len = (size_t) (p->ADDR_length);
	if (add_len > 0) {
		if (add_ptr[0] != 35)
			goto badaddr;
		if (add_ptr[1] != 0x00 || add_ptr[2] != 0x03)
			goto badaddr;
		if (add_len < 7)
			goto badaddr;
		bcopy(&add_ptr[3], &sin.sin_addr.in_addr, 4);
		if (add_len >= 7 && add_len <= 9)
			bcopy(&add_ptr[7], &sin.sin_port, 2);
	}
	np_set_state(np, NS_WACK_BREQ);
	return (tp_bind_req(np->tp, q, mp, &sin, sizeof(sin), p->CONIND_number));
      oustate:
	err = NOUTSTATE;
	goto error;
      bound:
	err = NBOUND;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_BIND_REQ, err, __FUNCTION__));
}

/**
 * np_unbind_req: - process N_UNBIND_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_UNBIND_REQ message
 */
static streams_fastcall streams_noinline int
np_unbind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
	np_set_state(np, NS_WACK_UREQ);
	return (tp_unbind_req(np->tp, q, mp));
}

/**
 * np_unitdata_req: - process N_UNITDATA_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_UNITDATA_REQ message
 */
static streams_fastcall streams_inline int
np_unitdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	return (np_error_ack(np, q, mp, N_UNITDATA_REQ, NNOTSUPPORT, __FUNCTION__));
}

/**
 * np_optmgmt_req: - process N_OPTMGMT_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_OPTMGMT_REQ message
 */
static streams_fastcall streams_noinline int
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	N_qos_co_opt_sel_t *qos;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->QOS_length, p->QOS_offset))
		goto badopt;
	if (p->OPTMGMT_flags & ~(DEFAULT_RC_SEL))
		goto badflag;
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		goto badflag;
	if (p->QOS_length > 0) {
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		if (p->QOS_length != sizeof(*qos))
			goto badqostype;
		if (qos->n_qos_type != N_QOS_CO_OPT_SEL)
			goto badqostype;
		/* FIXME: actually process QOS parameters */
	}
	if (np_get_state(np) == NS_IDLE)
		np_set_state(np, NS_WACK_OPTREQ);
	return (np_ok_ack(np, q, mp, N_OPTMGMT_REQ));
      badqostype:
	err = NBADQOSTYPE;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_OPTMGMT_REQ, err, __FUNCTION__));
}

/**
 * np_datack_req: - process N_DATACK_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_DATACK_REQ message
 */
static streams_fastcall streams_inline int
np_datack_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_datack_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_DATA_XFER)
		goto outstate;
	/* NPI Revision 2.0.0: If the NS provider has no knowledge of a
	   previous N_DATA_IND with the receipt confirmation flag set, then the 
	   NS provider should just ignore the request without generating a
	   fatal error. */
	goto discard;
#if 0
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
#endif
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (m_error(np, q, mp, EPROTO));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_reset_req: - process N_RESET_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_RESET_REQ message
 */
static streams_fastcall streams_noinline int
np_reset_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_DATA_XFER)
		goto outstate;
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_RESET_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_reset_res: - process N_RESET_RES message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_RESET_RES message
 */
static streams_fastcall streams_noinline int
np_reset_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_WRES_RIND)
		goto outstate;
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (np_error_ack(np, q, mp, N_RESET_RES, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_other_req: - process N_OTHER_REQ message
 * @np: network provider private structure
 * @q: active queue (write queue)
 * @mp: the N_OTHER_REQ message
 */
static streams_fastcall streams_noinline int
np_other_req(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (np_error_ack(np, q, mp, *p, err, __FUNCTION__));
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI Provider -> TPI User primitives from below
 *
 * --------------------------------------------------------------------------
 */

/**
 * tp_conn_ind: - process T_CONN_IND message
 * @q: active queue (read queue)
 * @mp: the T_CONN_IND message
 */
static streams_fastcall streams_noinline int
tp_conn_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_conn_con: - process T_CONN_CON message
 * @q: active queue (read queue)
 * @mp: the T_CONN_CON message
 *
 * Receive a T_CONN_CON.  This should only occur when we are in NS_WCON_CREQ and
 * TS_WCON_CREQ state.  The primitive needs to be translated to a N_CONN_CON
 * primtive.  Note that, as this is TCP/IP, we should get no connect data with
 * the TCP T-CONNECT.CONFIRMATION (but may with connection-like UDP).
 */
static streams_fastcall streams_noinline int
tp_conn_con(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_con *p = (typeof(p)) mp->b_rptr;
	caddr_t res_ptr, rsp_ptr, qos_ptr, opt_ptr;
	size_t res_len, rsp_len, qos_len, opt_len;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	rsp_ptr = (caddr_t) (mp->b_rptr + p->RES_offset);
	rsp_len = (size_t) p->RES_length;
	if (mp->b_wptr < res_ptr + res_len)
		goto badaddr;
	qos_ptr = (caddr_t) (mp->b_rptr + p->QOS_offset);
	qos_len = (size_T) p->QOS_length;
	if (mp->b_wptr < qos_ptr + qos_len)
		goto badopt;
	/* Convert rsp_ptr and rsp_len to res_ptr and res_len.  Also, convert
	   qos_ptr and qos_len to opt_ptr and opt_len. */
	tp_set_state(tp, TS_DATA_XFER);
	return np_conn_con(tp->np, q, mp, res_ptr, res_len, 0, qos_ptr, qps_len, mp->b_cont);
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
 * tp_discon_ind: - process T_DISCON_IND message
 * @q: active queue (read queue)
 * @mp: the T_DISCON_IND message
 *
 * Receive a T_DISCON_IND.  The primtiive needs to be translated to a
 * N_DISCON_IND primitive.  Note that, as this is TCP/IP, we shoudl get no
 * disconnect data with the TCP T-DISCONNECT.INDICATION (but could if we used
 * connection-like UDP).
 */
static streams_fastcall streams_noinline int
tp_discon_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_ind *p = (typeof(p)) mp->b_rptr;
	np_ulong origin, reason, sequence;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	tp_set_state(tp, TS_IDLE);
	if ((sequence = p->SEQ_number) != 0) {
		origin = N_PROVIDER;
		reason = N_REJ_UNSPECIFIED;
	} else {
		origin = N_UNDEFINED;
		reason = N_REASON_UNDEFINED;
	}
	return np_discon_ind(tp->np, q, mp, origin, reason, NULL, 0, sequence, mp->b_cont);
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_data_ind: - process T_DATA_IND message
 * @q: active queue (read queue)
 * @mp: the T_DATA_IND message
 */
static streams_fastcall streams_inline int
tp_data_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_exdata_ind: - process T_EXDATA_IND message
 * @q: active queue (read queue)
 * @mp: the T_EXDATA_IND message
 */
static streams_fastcall streams_noinline int
tp_exdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_info_ack: - process T_INFO_ACK message
 * @q: active queue (read queue)
 * @mp: the T_INFO_ACK message
 */
static streams_fastcall streams_noinline int
tp_info_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_bind_ack: - process T_BIND_ACK message
 * @q: active queue (read queue)
 * @mp: the T_BIND_ACK message
 */
static streams_fastcall streams_noinline int
tp_bind_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_error_ack: - process T_ERROR_ACK message
 * @q: active queue (read queue)
 * @mp: the T_ERROR_ACK message
 */
static streams_fastcall streams_noinline int
tp_error_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_ok_ack: - process T_OK_ACK message
 * @q: active queue (read queue)
 * @mp: the T_OK_ACK message
 */
static streams_fastcall streams_noinline int
tp_ok_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ok_ack *p = (typeof(p)) mp->b_rptr;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_WACK_CREQ:
		if (p->CORRECT_prim != T_CONN_REQ)
			mi_strlog(q, 0, SL_ERROR, "%s: CORRECT_prim should be %s instead of %s",
				  __FUNCTION__, "T_CONN_REQ", tp_primname(p->CORRECT_prim));
		tp_set_state(tp, TS_WCON_CREQ);
		break;
	case TS_WACK_CRES:
		if (p->CORRECT_prim != T_CONN_RES)
			mi_strlog(q, 0, SL_ERROR, "%s: CORRECT_prim should be %s instead of %s",
				  __FUNCTION__, "T_CONN_RES", tp_primname(p->CORRECT_prim));
		tp_set_state(tp, TS_DATA_XFER);
		return np_ok_ack(tp->np, q, mp, N_CONN_RES);
	case TS_WACK_DREQ6:
		if (p->CORRECT_prim != T_DISCON_REQ)
			mi_strlog(q, 0, SL_ERROR, "%s: CORRECT_prim should be %s instead of %s",
				  __FUNCTION__, "T_DISCON_REQ", tp_primname(p->CORRECT_prim));
		tp_set_state(tp, TS_IDLE);
		return np_ok_ack(tp->np, q, mp, N_DISCON_REQ);
	case TS_WACK_DREQ7:
		if (p->CORRECT_prim != T_DISCON_REQ)
			mi_strlog(q, 0, SL_ERROR, "%s: CORRECT_prim should be %s instead of %s",
				  __FUNCTION__, "T_DISCON_REQ", tp_primname(p->CORRECT_prim));
		tp_set_state(tp, TS_IDLE);
		return np_ok_ack(tp->np, q, mp, N_DISCON_REQ);
	case TS_WACK_DREQ9:
		if (p->CORRECT_prim != T_DISCON_REQ)
			mi_strlog(q, 0, SL_ERROR, "%s: CORRECT_prim should be %s instead of %s",
				  __FUNCTION__, "T_DISCON_REQ", tp_primname(p->CORRECT_prim));
		tp_set_state(tp, TS_IDLE);
		return np_ok_ack(tp->np, q, mp, N_DISCON_REQ);
	case TS_WACK_DREQ10:
		if (p->CORRECT_prim != T_DISCON_REQ)
			mi_strlog(q, 0, SL_ERROR, "%s: CORRECT_prim should be %s instead of %s",
				  __FUNCTION__, "T_DISCON_REQ", tp_primname(p->CORRECT_prim));
		tp_set_state(tp, TS_IDLE);
		return np_ok_ack(tp->np, q, mp, N_DISCON_REQ);
	case TS_WACK_DREQ11:
		if (p->CORRECT_prim != T_DISCON_REQ)
			mi_strlog(q, 0, SL_ERROR, "%s: CORRECT_prim should be %s instead of %s",
				  __FUNCTION__, "T_DISCON_REQ", tp_primname(p->CORRECT_prim));
		tp_set_state(tp, TS_IDLE);
		return np_ok_ack(tp->np, q, mp, N_DISCON_REQ);
	case TS_WACK_UREQ:
		if (p->CORRECT_prim != T_UNBIND_REQ)
			mi_strlog(q, 0, SL_ERROR, "%s: CORRECT_prim should be %s instead of %s",
				  __FUNCTION__, "T_UNBIND_REQ", tp_primname(p->CORRECT_prim));
		tp_set_state(tp, TS_UNBND);
		return np_ok_ack(tp->np, q, mp, N_UNBIND_REQ);
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: bad state %s for T_OK_ACK", __FUNCTION__,
			  tp_statename(tp_get_state(tp)));
	}
	return (0);
}

/**
 * tp_unitdata_ind: - process T_UNITDATA_IND message
 * @q: active queue (read queue)
 * @mp: the T_UNITDATA_IND message
 */
static streams_fastcall streams_inline int
tp_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_uderror_ind: - process T_UDERROR_IND message
 * @q: active queue (read queue)
 * @mp: the T_UDERROR_IND message
 */
static streams_fastcall streams_noinline int
tp_uderror_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_optmgmt_ack: - process T_OPTMGMT_ACK message
 * @q: active queue (read queue)
 * @mp: the T_OPTMGMT_ACK message
 */
static streams_fastcall streams_noinline int
tp_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_ordrel_ind: - process T_ORDREL_IND message
 * @q: active queue (read queue)
 * @mp: the T_ORDREL_IND message
 */
static streams_fastcall streams_noinline int
tp_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_optdata_ind: - process T_OPTDATA_IND message
 * @q: active queue (read queue)
 * @mp: the T_OPTDATA_IND message
 */
static streams_fastcall streams_inline int
tp_optdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_addr_ack: - process T_ADDR_ACK message
 * @q: active queue (read queue)
 * @mp: the T_ADDR_ACK message
 */
static streams_fastcall streams_noinline int
tp_addr_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_capability_ack: - process T_CAPABILITY_ACK message
 * @q: active queue (read queue)
 * @mp: the T_CAPABILITY_ACK message
 */
static streams_fastcall streams_noinline int
tp_capability_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_other_ind: - process T_OTHER_IND message
 * @q: active queue (read queue)
 * @mp: the T_OTHER_IND message
 */
static streams_fastcall streams_noinline int
tp_other_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

/**
 * tp_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 *
 * We are not expecting pure data messages from below.
 */
static streams_fastcall int
tp_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: M_DATA message on read queue.", __FUNCTION__);
	return (0);
}

/**
 * tp_m_proto: - process M_(PC)PROTO message
 * @q: active queue (read queue)
 * @mp: the M_(PC)PROTO message
 */
static streams_fastcall streams_inline __hot_get int
tp_m_proto(queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	struct tp *tp;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto tooshort;
	prim = *(t_uscalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	tp = TP_PRIV(q);
	tp_save_state(tp);
	np_save_state(tp->np);
	switch (prim) {
	case T_CONN_IND:
		rtn = tp_conn_ind(tp, q, mp);
		break;
	case T_CONN_CON:
		rtn = tp_conn_con(tp, q, mp);
		break;
	case T_DISCON_IND:
		rtn = tp_discon_ind(tp, q, mp);
		break;
	case T_DATA_IND:
		rtn = tp_data_ind(tp, q, mp);
		break;
	case T_EXDATA_IND:
		rtn = tp_exdata_ind(tp, q, mp);
		break;
	case T_INFO_ACK:
		rtn = tp_info_ack(tp, q, mp);
		break;
	case T_BIND_ACK:
		rtn = tp_bind_ack(tp, q, mp);
		break;
	case T_ERROR_ACK:
		rtn = tp_error_ack(tp, q, mp);
		break;
	case T_OK_ACK:
		rtn = tp_ok_ack(tp, q, mp);
		break;
	case T_UNITDATA_IND:
		rtn = tp_unitdata_ind(tp, q, mp);
		break;
	case T_UDERROR_IND:
		rtn = tp_uderror_ind(tp, q, mp);
		break;
	case T_OPTMGMT_ACK:
		rtn = tp_optmgmt_ack(tp, q, mp);
		break;
	case T_ORDREL_IND:
		rtn = tp_ordrel_ind(tp, q, mp);
		break;
	case T_OPTDATA_IND:
		rtn = tp_optdata_ind(tp, q, mp);
		break;
	case T_ADDR_ACK:
		rtn = tp_addr_ack(tp, q, mp);
		break;
	case T_CAPABILITY_ACK:
		rtn = tp_capability_ack(tp, q, mp);
		break;
/*
 * None of the following messages should appear at the read-side queue.  Only
 * upward defined TPI messages should appear at the read-side queue.  Discard
 * these.
 */
	case T_CONN_REQ:
	case T_CONN_RES:
	case T_DISCON_REQ:
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_INFO_REQ:
	case T_BIND_REQ:
	case T_UNBIND_REQ:
	case T_UNITDATA_REQ:
	case T_OPTMGMT_REQ:
	case T_ORDREL_REQ:
	case T_OPTDATA_REQ:
	case T_ADDR_REQ:
	case T_CAPABILITY_REQ:
		goto wrongway;
	default:
		rtn = tp_other_ind(tp, q, mp);
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
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short", __FUNCTION__);
	return (0);
}

/**
 * tp_m_sig: - process M_(PC)SIG message
 * @q: active queue (read queue)
 * @mp: the M_(PC)SIG message
 */
static streams_fastcall int
tp_m_sig(queue_t *q, mblk_t *mp)
{
	struct tp *tp;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(int))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);
	if (!mi_timer_valid(mp))
		return (0);
	tp = TP_PRIV(q);
	tp_save_state(tp);
	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = tp_t1_timeout(tp, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d",
			  __FUNCTION__ * (int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		tp_restore_state(tp);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
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
 * tp_m_copy: - process M_COPYIN or M_COPYOUT message
 * @q: active queue (read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 *
 * We do not pass fake M_IOCTL messages downstream, so the only M_IOCTL
 * responses that come up from below were as a result of passing M_IOCTL
 * messages down from the Stream head.  So we pass the responses back up.
 */
static streams_fastcall int
tp_m_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tp_m_iocack: - process M_IOCACK or M_IOCNAK message
 * @q: active queue (read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 *
 * We do not pass fake M_IOCTL messages downstream, so the only M_IOCTL
 * responses that come up from below were as a result of passing M_IOCTL
 * messages down from the Stream head.  So we pass the responses back up.
 */
static streams_fastcall int
tp_m_iocack(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tp_m_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 *
 * Canonical read side module flush procedure.
 */
static streams_fastcall int
tp_m_flush(queue_t *q, mblk_t *mp)
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
 * tp_m_unrec: - process M_ERROR or M_HANGUP message
 * @q: active queue (read queue)
 * @mp: the M_ERROR or M_HANGUP message
 *
 * Modules should pass M_ERROR and M_HANGUP messages along.
 */
static streams_fastcall int
tp_m_error(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_unrec: - process unrecognized message
 * @q: active queue (read queue)
 * @mp: the unrecognized message
 *
 * Modules should pass unrecognized messages along if possible.
 */
static streams_fastcall int
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
 * @q: active queue (read queue)
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
	case M_COPYIN:
	case M_COPYOUT:
		return tp_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return tp_m_iocack(q, mp);
	case M_FLUSH:
		return tp_m_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return tp_m_error(q, mp);
	default:
		return tp_m_unrec(q, mp);
	}
}

/**
 * np_m_data: - process M_DATA message
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 */
static int
np_m_data(queue_t *q, mblk_t *mp)
{
}

/**
 * np_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
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
	tp_save_state(np->tp);
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
 * None of the following messages should appear at the write-side queue.  Only
 * downward defined NPI messages should appear at the write-side queue.  Discard
 * these.
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
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short", __FUNCTION__);
	freemsg(mp);
	return (0);
}

/**
 * np_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (write queue)
 * @mp: the M_SIG or M_PCSIG message
 *
 * These are either timer messages (in which case they have a length greater
 * than or equal to 4) or true signal messages (in which case they have a length
 * of 1). Therefore, if the length of the message is less than sizeof(int) we
 * pass them along.
 */
static int
np_m_sig(queue_t *q, mblk_t *mp)
{
	struct np *np;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(int))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	if (!mi_timer_valid(mp))
		goto done;
	np = NP_PRIV(q);
	np_save_state(np);
	switch (*(int *) mp->b_rptr) {
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = np_t1_timeout(np, q, mp);
		break;
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
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
 * np_m_ioctl: - process M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static streams_fastcall int
np_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct np *np;
	caddr_t priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != ISOT_IOC_MAGIC)
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
	putnext(q, mp);
	return (0);
}

/**
 * np_m_iocdata: - process M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 *
 * Processes the M_IOCDATA reponse from a previous M_COPYIN or M_COPYOUT message
 * passed upstream, or a fake one placed here as a part of I_STR M_IOCTL
 * processing.
 */
static streams_fastcall int
np_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct np *np;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != ISOT_IOC_MAGIC)
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
      efault:
	mi_copy_done(q, mp, EFAULT);
	return (0);
      notforus:
	/* Let driver deal with these. */
	putnext(q, mp);
	return (0);
}

/**
 * np_m_flush: - process M_FLUSH message
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 *
 * Canonical write side flush procedure for modules.
 */
static streams_fastcall int
np_m_flush(queue_t *q, mblk_t *mp)
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
 * np_m_unrec: - process unrecognized message
 * @q: active queue (write queue)
 * @mp: the unrecognized message
 */
static streams_fastcall int
np_m_unrec(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_msg: - process STREAMS message
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static streams_fastcall streams_inline int
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
tp_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tp_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
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
 * QUEUE OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */

static caddr_t isot_opens = NULL;

static streamscall int
isot_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct np *np;
	struct tp *tp;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(struct T_info_req), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&isot_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	p = PRIV(q);
	np = &p->r_priv;
	tp = &p->w_priv;

	/* initialize private structure */
	np->priv = p;
	np->tp = tp;
	np->oq = WR(q);
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
	np->proto.info.QOS_range_offset = sizeof(np->proto.info) + sizeof(np->add) + sizeof(np->qos);
	np->proto.info.OPTIONS_flags = 0;
	np->proto.info.NIDU_size 4096;
	np->proto.info.SERV_type = N_CONS;
	np->proto.info.CURRENT_state = TS_UNBND;
	np->proto.info.PROVIDER_type = N_SNICFP;
	np->proto.info.NODU_size = 4096;
	np->proto.info.PROTOID_length = 0;
	np->proto.info.PROTOID_offset = 0;
	np->proto.info.NPI_version = N_CURRENT_VERSION;

	np->proto.add[0] = 35; /* AFI 35 */
	np->proto.add[1] = 0x00; /* ICP 1 */
	np->proto.add[2] = 0x01;
	np->proto.add[3] = 0x00; /* IPv4 address */
	np->proto.add[4] = 0x00;
	np->proto.add[5] = 0x00;
	np->proto.add[6] = 0x00;
	np->proto.add[7] = 0x00; /* IPv4 port number */
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

	tp->priv = p;
	tp->np = np;
	tp->oq = q;
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

	tp->proto.loc = (struct sockaddr_in) { AF_INET, 0, { 0x000000000 } };
	tp->proto.rem = (struct sockaddr_in) { AF_INET, 0, { 0x000000000 } };

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((struct T_info_req *) mp->b_wptr)->PRIM_type = T_INFO_REQ;
	mp->b_wptr += sizeof(struct T_info_req);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static streamscall int
isot_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	(void) p;
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm(&isot_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit isot_rinit = {
	.qi_putp = tp_put,		/* Read put (message from below) */
	.qi_srvp = tp_srv,		/* Read service */
	.qi_qopen = &isot_open,		/* Each open */
	.qi_qclose = &isot_close,	/* Last close */
	.qi_minfo = &isot_minfo,	/* Module information */
	.qi_mstat = &isot_rstat,	/* Module statistics */
};

static struct qinit isot_winit = {
	.qi_putp = np_put,		/* Write put (message form above) */
	.qi_srvp = np_srv,		/* Write service */
	.qi_minfo = &isot_minfo,	/* Module information */
	.qi_mstat = &isot_wstat,	/* Module statistics */
};

struct streamtab isot_info = {
	.st_rdinit = &isot_rinit,
	.st_wrinit = &isot_winit,
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
MODULE_PARM_DESC(modid, "Module ID for ISOT. (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static __init int
isot_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&isot_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
isot_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&isot_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(isot_modinit);
module_exit(isot_modexit);

#endif				/* LINUX */
#endif /* 0 */
