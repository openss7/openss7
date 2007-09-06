/*****************************************************************************

 @(#) $RCSfile: tr_mod.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/09/06 11:16:18 $

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

 Last Modified $Date: 2007/09/06 11:16:18 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tr_mod.c,v $
 Revision 0.9.2.4  2007/09/06 11:16:18  brian
 - testing updates

 Revision 0.9.2.3  2007/08/15 05:20:41  brian
 - GPLv3 updates

 Revision 0.9.2.2  2007/08/14 12:18:56  brian
 - GPLv3 header updates

 Revision 0.9.2.1  2007/08/03 13:02:54  brian
 - added documentation and minimal modules

 *****************************************************************************/

#ident "@(#) $RCSfile: tr_mod.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/09/06 11:16:18 $"

static char const ident[] =
    "$RCSfile: tr_mod.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/09/06 11:16:18 $";

/*
 * This is TR-MOD.  It is a simplified Transaction Interface (TRI) module for TCAP that can be
 * pushed as a pushable STREAMS module over an SCCP Stream.  Either the SCCP Stream can be bound
 * before pushing or after.  This is the simple case where all transactions are delivered on the
 * same Stream.
 *
 * This TR-MOD module presents a Transaction Interface (TRI) to its user.  To present a Transction
 * Component Interface (TCI) instead, push the TC-MOD module as well.
 *
 * Usage:
 *
 *  - Open an SCCP Stream.
 *  - Push the TR-MOD module.
 *  - Bind the Stream.
 *  - Exchange transactions.
 *  - Unbind the Stream.
 *  - Pop the TR-MOD module.
 *  - Close the SCCP Stream.
 */

#define _LFS_SOURCE 1
#define _MPS_SOURCE 1
#define _SVR4_SOURCE 1
#define _SUN_SOURCE 1

#include <sys/os7/compat.h>

#include <sys/npi.h>
#include <sys/npi_sccp.h>
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <ss7/sccpi.h>
#include <ss7/sccpi_ioctl.h>
#include <sys/tihdr.h>
#include <sys/tpi.h>
#include <sys/tpi_tcap.h>
#include <ss7/tr.h>
#include <ss7/tcap.h>
#include <ss7/tcap_ioctl.h>

#define TR_DESCRIP	"SS7/TCAP-TR (TCAP Transaction Handling) STREAMS MODULE."
#define TR_REVISION	"OpenSS7 $RCSfile: tr_mod.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/09/06 11:16:18 $"
#define TR_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define TR_DEVICE	"Provides OpenSS7 TCAP-TR module."
#define TR_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TR_LICENSE	"GPL"
#define TR_BANNER	TR_DESCRIP	"\n" \
			TR_REVISION	"\n" \
			TR_COPYRIGHT	"\n" \
			TR_DEVICE	"\n" \
			TR_CONTACT	"\n"
#define TR_SPLASH	TR_DEVICE	" - " \
			TR_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(TR_CONTACT);
MODULE_DESCRIPTION(TR_DESCRIP);
MODULE_SUPPORTED_DEVICE(TR_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TR_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tr-mod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define TR_MOD_MOD_ID		CONFIG_STREAMS_TR_MOD_MODID
#define TR_MOD_MOD_NAME		CONFIG_STREAMS_TR_MOD_NAME
#endif				/* LFS */

#ifndef TR_MOD_MOD_NAME
#define TR_MOD_MOD_NAME		"tr-mod"
#endif				/* TR_MOD_MOD_NAME */

#ifndef TR_MOD_MOD_ID
#define TR_MOD_MOD_ID		0
#endif				/* TR_MOD_MOD_ID */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define MOD_ID		TR_MOD_MOD_ID
#define MOD_NAME	TR_MOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	TR_BANNER
#else				/* MODULE */
#define MOD_BANNER	TR_SPLASH
#endif				/* MODULE */

static struct module_info tr_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat tr_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tr_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

static struct {
	struct tcap_opt_conf_df config;
	struct tcap_statem_df statem;
	struct tcap_stats_df statsp;
	struct tcap_stats_df stats;
	struct tcap_notify_df notify;
} df;

struct priv;
struct tr;
struct sc;

struct tr {
	struct priv *priv;
	struct sc *sc;
	queue_t *oq;
	t_uscalar_t state;
	t_uscalar_t oldstate;
	struct {
		struct tcap_opt_conf_sp config;
		struct tcap_statem_sp statem;
		struct tcap_stats_sp statsp;
		struct tcap_stats_sp stats;
		struct tcap_notify_sp notify;
	} sp;
};

struct sc {
	struct priv *priv;
	struct tr *tr;
	queue_t *oq;
	np_ulong state;
	np_ulong oldstate;
	struct sccp_addr add;
	N_info_ack_t prot;
	N_qos_sel_data_sccp_t qos;
	struct tcap_opt_conf_sc config;
	struct tcap_statem_sc statem;
	struct tcap_stats_sc statsp;
	struct tcap_stats_sc stats;
	struct tcap_notify_sc notify;
};

struct priv {
	struct sc r_priv;		/* SCCP structure on RD side */
	struct tr w_priv;		/* TCAP structure on WR side */
};

#define PRIV(q)	    ((struct priv *)(q)->q_ptr)

#define TR_PRIV(q)  (&PRIV(q)->w_priv)
#define SC_PRIV(q)  (&PRIV(q)->r_priv)

#define PAD4(len) ((len + 3) & ~3)

#define STRLOGIO	0	/* log Stream input-output controls */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

static inline const char *
tr_iocname(int cmd)
{
	switch (cmd) {
	case TCAP_IOCGOPTION:
		return ("TCAP_IOCGOPTION");
	case TCAP_IOCSOPTION:
		return ("TCAP_IOCSOPTION");
	case TCAP_IOCGCONFIG:
		return ("TCAP_IOCGCONFIG");
	case TCAP_IOCSCONFIG:
		return ("TCAP_IOCSCONFIG");
	case TCAP_IOCTCONFIG:
		return ("TCAP_IOCTCONFIG");
	case TCAP_IOCCCONFIG:
		return ("TCAP_IOCCCONFIG");
	case TCAP_IOCGSTATEM:
		return ("TCAP_IOCGSTATEM");
	case TCAP_IOCCMRESET:
		return ("TCAP_IOCCMRESET");
	case TCAP_IOCGSTATSP:
		return ("TCAP_IOCGSTATSP");
	case TCAP_IOCSSTATSP:
		return ("TCAP_IOCSSTATSP");
	case TCAP_IOCGSTATS:
		return ("TCAP_IOCGSTATS");
	case TCAP_IOCCSTATS:
		return ("TCAP_IOCCSTATS");
	case TCAP_IOCGNOTIFY:
		return ("TCAP_IOCGNOTIFY");
	case TCAP_IOCSNOTIFY:
		return ("TCAP_IOCSNOTIFY");
	case TCAP_IOCCNOTIFY:
		return ("TCAP_IOCCNOTIFY");
	case TCAP_IOCCMGMT:
		return ("TCAP_IOCCMGMT");
	case TCAP_IOCCPASS:
		return ("TCAP_IOCCPASS");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tr_primname(uint prim)
{
	switch (prim) {
	case TR_INFO_REQ:
		return ("TR_INFO_REQ");
	case TR_BIND_REQ:
		return ("TR_BIND_REQ");
	case TR_UNBIND_REQ:
		return ("TR_UNBIND_REQ");
	case TR_OPTMGMT_REQ:
		return ("TR_OPTMGMT_REQ");
	case TR_UNI_REQ:
		return ("TR_UNI_REQ");
	case TR_BEGIN_REQ:
		return ("TR_BEGIN_REQ");
	case TR_BEGIN_RES:
		return ("TR_BEGIN_RES");
	case TR_CONT_REQ:
		return ("TR_CONT_REQ");
	case TR_END_REQ:
		return ("TR_END_REQ");
	case TR_ABORT_REQ:
		return ("TR_ABORT_REQ");
	case TR_INFO_ACK:
		return ("TR_INFO_ACK");
	case TR_BIND_ACK:
		return ("TR_BIND_ACK");
	case TR_OK_ACK:
		return ("TR_OK_ACK");
	case TR_ERROR_ACK:
		return ("TR_ERROR_ACK");
	case TR_OPTMGMT_ACK:
		return ("TR_OPTMGMT_ACK");
	case TR_UNI_IND:
		return ("TR_UNI_IND");
	case TR_BEGIN_IND:
		return ("TR_BEGIN_IND");
	case TR_BEGIN_CON:
		return ("TR_BEGIN_CON");
	case TR_CONT_IND:
		return ("TR_CONT_IND");
	case TR_END_IND:
		return ("TR_END_IND");
	case TR_ABORT_IND:
		return ("TR_ABORT_IND");
	case TR_NOTICE_IND:
		return ("TR_NOTICE_IND");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tr_statename(uint state)
{
	switch (state) {
	case TRS_UNBND:
		return ("TRS_UNBND");
	case TRS_WACK_BREQ:
		return ("TRS_WACK_BREQ");
	case TRS_WACK_UREQ:
		return ("TRS_WACK_UREQ");
	case TRS_IDLE:
		return ("TRS_IDLE");
	case TRS_WACK_OPTREQ:
		return ("TRS_WACK_OPTREQ");
	case TRS_WACK_RRES:
		return ("TRS_WACK_RRES");
	case TRS_WCON_CREQ:
		return ("TRS_WCON_CREQ");
	case TRS_WRES_CIND:
		return ("TRS_WRES_CIND");
	case TRS_WACK_CRES:
		return ("TRS_WACK_CRES");
	case TRS_DATA_XFER:
		return ("TRS_DATA_XFER");
	case TRS_WCON_RREQ:
		return ("TRS_WCON_RREQ");
	case TRS_WRES_RIND:
		return ("TRS_WRES_RIND");
	case TRS_WACK_DREQ6:
		return ("TRS_WACK_DREQ6");
	case TRS_WACK_DREQ7:
		return ("TRS_WACK_DREQ7");
	case TRS_WACK_DREQ9:
		return ("TRS_WACK_DREQ9");
	case TRS_WACK_DREQ10:
		return ("TRS_WACK_DREQ10");
	case TRS_WACK_DREQ11:
		return ("TRS_WACK_DREQ11");
	case TRS_NOSTATES:
		return ("TRS_NOSTATES");
	default:
		return ("(unknown)");
	}
}

static uint
tr_get_state(struct tr *tr)
{
	return (tr->state);
}

static uint
tr_set_state(struct tr *tr, uint newstate)
{
	uint oldstate = tr->state;

	if (newstate != oldstate) {
		tr->state = newstate;
		mi_strlog(tr->oq, STRLOGST, SL_TRACE, "%s <- %s", tr_statename(newstate),
			  tr_statename(oldstate));
	}
	return (newstate);
}

static uint
tr_save_state(struct tr *tr)
{
	return ((tr->oldstate = tr_get_state(tr)));
}

static uint
tr_restore_state(struct tr *tr)
{
	return (tr_set_state(tr, tr->oldstate));
}

static const char *
sc_primname(uint prim)
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
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_CON:
		return ("N_RESET_CON");
	case N_NOTICE_IND:
		return ("N_NOTICE_IND");
	case N_INFORM_REQ:
		return ("N_INFORM_REQ");
	case N_INFORM_IND:
		return ("N_INFORM_IND");
	case N_COORD_REQ:
		return ("N_COORD_REQ");
	case N_COORD_RES:
		return ("N_COORD_RES");
	case N_COORD_IND:
		return ("N_COORD_IND");
	case N_COORD_CON:
		return ("N_COORD_CON");
	case N_STATE_REQ:
		return ("N_STATE_REQ");
	case N_STATE_IND:
		return ("N_STATE_IND");
	case N_PCSTATE_IND:
		return ("N_PCSTATE_IND");
	case N_TRAFFIC_IND:
		return ("N_TRAFFIC_IND");
	default:
		return ("(unknown)");
	}
}
static inline const char *
sc_statename(uint state)
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

static uint
sc_get_state(struct sc *sc)
{
	return (sc->state);
}

static uint
sc_set_state(struct sc *sc, uint newstate)
{
	uint oldstate = sc->state;

	if (newstate != oldstate) {
		sc->state = newstate;
		mi_strlog(sc->oq, STRLOGST, SL_TRACE, "%s <- %s", sc_statename(newstate),
			  sc_statename(oldstate));
	}
	return (newstate);
}

static uint
sc_save_state(struct sc *sc)
{
	return ((sc->oldstate = sc_get_state(sc)));
}

static uint
sc_restore_state(struct sc *sc)
{
	return (sc_set_state(sc, sc->oldstate));
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  TR Provider -> TR User primitives.
 *
 *  =========================================================================
 */

static int
m_error(struct tr *tr, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(tr->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#define FIXME (-1)

static int
tr_info_ack(struct tr *tr, queue_t *q, mblk_t *msg)
{
	struct TR_info_ack *p;
	mblk_t *mp;

	(void) tr_info_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_INFO_ACK;
		p->TSDU_size = FIXME;	/* maximum TSDU size */
		p->ETSDU_size = FIXME;	/* maximum ETSDU size */
		p->CDATA_size = FIXME;	/* connect data size */
		p->DDATA_size = FIXME;	/* discon data size */
		p->ADDR_size = FIXME;	/* address size */
		p->OPT_size = FIXME;	/* options size */
		p->TIDU_size = FIXME;	/* transaction i/f data unit size */
		p->SERV_type = FIXME;	/* service type */
		p->CURRENT_state = tr_get_state(tr);	/* current state */
		p->PROVIDER_flag = FIXME;	/* type of TR provider */
		p->TRPI_version = FIXME;	/* version # of trpi that is supported */
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_INFO_ACK");
		putnext(tr->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tr_bind_ack: - issue TR_BIND_ACK primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add_len: address length
 * @add_ptr: address pointer
 * @xact: maximum number of outstanding transaction begin indications
 */
static int
tr_bind_ack(struct tr *tr, queue_t *q, mblk_t *msg, size_t add_len, caddr_t add_ptr,
	    t_uscalar_t xact)
{
	struct TR_bind_ack *p;
	mblk_t *mp;

	(void) tr_bind_ack;
	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_BIND_ACK;
		p->ADDR_length = add_len;	/* address length */
		p->ADDR_offset = add_len ? sizeof(*p) : 0;	/* address offset */
		p->XACT_number = xact;	/* open transactions */
		p->TOKEN_value = (t_uscalar_t) (ulong) tr->oq;	/* value of "token" assigned to
								   stream */
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_BIND_ACK");
		putnext(tr->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tr_ok_ack: - issue a TR_OK_ACK primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
tr_ok_ack(struct tr *tr, queue_t *q, mblk_t *msg, t_uscalar_t prim)
{
	struct TR_ok_ack *p;
	mblk_t *mp;

	(void) tr_ok_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_OK_ACK");
		putnext(tr->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tr_error_ack: - issue a TR_ERROR_ACK primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error number
 * @tid: transaction when non-zero
 */
static int
tr_error_ack(struct tr *tr, queue_t *q, mblk_t *msg, t_uscalar_t prim, t_scalar_t error,
	     t_uscalar_t tid)
{
	struct TR_error_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TRPI_error = error < 0 ? TRSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		p->TRANS_id = tid;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_ERROR_ACK");
		putnext(tr->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tr_optmgmt_ack: - issue a TR_OPTMGMT_ACK primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @flags: management flags
 */
static int
tr_optmgmt_ack(struct tr *tr, queue_t *q, mblk_t *msg, size_t opt_len, caddr_t opt_ptr,
	       t_uscalar_t flags)
{
	struct TR_optmgmt_ack *p;
	mblk_t *mp;

	(void) tr_optmgmt_ack;
	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_OPTMGMT_ACK");
		putnext(tr->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tr_uni_ind: - issue TR_UNI_IND primtitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dest_len: destination address length
 * @dest_ptr: destination address pointer
 * @orig_len: originating address length
 * @orig_ptr: originating address pointer
 * @opts_len: options length
 * @opts_ptr: options pointer
 * @dp: transaction user data
 */
static int
tr_uni_ind(struct tr *tr, queue_t *q, mblk_t *msg,
	   size_t dest_len, caddr_t dest_ptr,
	   size_t orig_len, caddr_t orig_ptr, size_t opts_len, caddr_t opts_ptr, mblk_t *dp)
{
	struct TR_uni_ind *p;
	mblk_t *mp;

	if (bcanputnext(tr->oq, dp->b_band)) {
		if ((mp = mi_allocb(q, sizeof(*p) + dest_len + orig_len + opts_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_UNI_IND;
			p->DEST_length = dest_len;
			p->DEST_offset = dest_len ? sizeof(*p) : 0;
			p->ORIG_length = orig_len;
			p->ORIG_offset = orig_len ? sizeof(*p) + dest_len : 0;
			p->OPT_length = opts_len;
			p->OPT_offset = opts_len ? sizeof(*p) + dest_len + orig_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dest_ptr, mp->b_wptr, dest_len);
			mp->b_wptr += dest_len;
			bcopy(orig_ptr, mp->b_wptr, orig_len);
			mp->b_wptr += orig_len;
			bcopy(opts_ptr, mp->b_wptr, opts_len);
			mp->b_wptr += opts_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_UNI_IND");
			putnext(tr->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * tr_begin_ind: - issue TR_BEGIN_IND primtitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tid: transaction id
 * @flags: association flags
 * @dest_len: destination address length
 * @dest_ptr: destination address pointer
 * @orig_len: originating address length
 * @orig_ptr: originating address pointer
 * @opts_len: options length
 * @opts_ptr: options pointer
 * @dp: transaction user data
 */
static int
tr_begin_ind(struct tr *tr, queue_t *q, mblk_t *msg,
	     t_uscalar_t tid, t_uscalar_t flags,
	     size_t dest_len, caddr_t dest_ptr,
	     size_t orig_len, caddr_t orig_ptr, size_t opts_len, caddr_t opts_ptr, mblk_t *dp)
{
	struct TR_begin_ind *p;
	mblk_t *mp;

	(void) tr_begin_ind;
	if (bcanputnext(tr->oq, dp->b_band)) {
		if ((mp = mi_allocb(q, sizeof(*p) + dest_len + orig_len + opts_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_BEGIN_IND;
			p->TRANS_id = tid;
			p->ASSOC_flags = flags;
			p->DEST_length = dest_len;
			p->DEST_offset = dest_len ? sizeof(*p) : 0;
			p->ORIG_length = orig_len;
			p->ORIG_offset = orig_len ? sizeof(*p) + dest_len : 0;
			p->OPT_length = opts_len;
			p->OPT_offset = opts_len ? sizeof(*p) + dest_len + orig_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dest_ptr, mp->b_wptr, dest_len);
			mp->b_wptr += dest_len;
			bcopy(orig_ptr, mp->b_wptr, orig_len);
			mp->b_wptr += orig_len;
			bcopy(opts_ptr, mp->b_wptr, opts_len);
			mp->b_wptr += opts_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_BEGIN_IND");
			putnext(tr->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * tr_begin_ind: - issue TR_BEGIN_IND primtitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cid: correlation id
 * @flags: association flags
 * @tid: transaction id
 * @orig_len: originating address length
 * @orig_ptr: originating address pointer
 * @opts_len: options length
 * @opts_ptr: options pointer
 * @dp: transaction user data
 */
static int
tr_begin_con(struct tr *tr, queue_t *q, mblk_t *msg,
	     t_uscalar_t cid, t_uscalar_t flags, t_uscalar_t tid,
	     size_t orig_len, caddr_t orig_ptr, size_t opts_len, caddr_t opts_ptr, mblk_t *dp)
{
	struct TR_begin_con *p;
	mblk_t *mp;

	(void) tr_begin_con;
	if (bcanputnext(tr->oq, dp->b_band)) {
		if ((mp = mi_allocb(q, sizeof(*p) + orig_len + opts_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_BEGIN_CON;
			p->CORR_id = cid;
			p->ASSOC_flags = flags;
			p->TRANS_id = tid;
			p->ORIG_length = orig_len;
			p->ORIG_offset = orig_len ? sizeof(*p) : 0;
			p->OPT_length = opts_len;
			p->OPT_offset = opts_len ? sizeof(*p) + orig_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(orig_ptr, mp->b_wptr, orig_len);
			mp->b_wptr += orig_len;
			bcopy(opts_ptr, mp->b_wptr, opts_len);
			mp->b_wptr += opts_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_BEGIN_CON");
			putnext(tr->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * tr_cont_ind: - issue TR_CONT_IND primtitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tid: transaction id
 * @flags: association flags
 * @opts_len: options length
 * @opts_ptr: options pointer
 * @dp: transaction user data
 */
static int
tr_cont_ind(struct tr *tr, queue_t *q, mblk_t *msg,
	    t_uscalar_t tid, t_uscalar_t flags, size_t opts_len, caddr_t opts_ptr, mblk_t *dp)
{
	struct TR_cont_ind *p;
	mblk_t *mp;

	(void) tr_cont_ind;
	if (bcanputnext(tr->oq, dp->b_band)) {
		if ((mp = mi_allocb(q, sizeof(*p) + opts_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_CONT_IND;
			p->TRANS_id = tid;
			p->ASSOC_flags = flags;
			p->OPT_length = opts_len;
			p->OPT_offset = opts_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(opts_ptr, mp->b_wptr, opts_len);
			mp->b_wptr += opts_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_CONT_IND");
			putnext(tr->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * tr_end_ind: - issue TR_END_IND primtitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cid: correlation id
 * @tid: transaction id
 * @opts_len: options length
 * @opts_ptr: options pointer
 * @dp: transaction user data
 */
static int
tr_end_ind(struct tr *tr, queue_t *q, mblk_t *msg,
	   t_uscalar_t tid, t_uscalar_t cid, size_t opts_len, caddr_t opts_ptr, mblk_t *dp)
{
	struct TR_end_ind *p;
	mblk_t *mp;

	(void) tr_end_ind;
	if (bcanputnext(tr->oq, dp->b_band)) {
		if ((mp = mi_allocb(q, sizeof(*p) + opts_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_END_IND;
			p->CORR_id = cid;
			p->TRANS_id = tid;
			p->OPT_length = opts_len;
			p->OPT_offset = opts_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(opts_ptr, mp->b_wptr, opts_len);
			mp->b_wptr += opts_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_END_IND");
			putnext(tr->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * tr_abort_ind: - issue TR_ABORT_IND primtitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cid: correlation id
 * @tid: transaction id
 * @opts_len: options length
 * @opts_ptr: options pointer
 * @cause: abort cause
 * @orig: abort originator
 * @dp: transaction user data
 */
static int
tr_abort_ind(struct tr *tr, queue_t *q, mblk_t *msg,
	     t_uscalar_t cid, t_uscalar_t tid,
	     size_t opts_len, caddr_t opts_ptr, t_uscalar_t cause, t_uscalar_t orig, mblk_t *dp)
{
	struct TR_abort_ind *p;
	mblk_t *mp;

	(void) tr_abort_ind;
	if (bcanputnext(tr->oq, dp->b_band)) {
		if ((mp = mi_allocb(q, sizeof(*p) + opts_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_ABORT_IND;
			p->CORR_id = cid;
			p->TRANS_id = tid;
			p->OPT_length = opts_len;
			p->OPT_offset = opts_len ? sizeof(*p) : 0;
			p->ABORT_cause = cause;
			p->ORIGINATOR = orig;
			mp->b_wptr += sizeof(*p);
			bcopy(opts_ptr, mp->b_wptr, opts_len);
			mp->b_wptr += opts_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_ABORT_IND");
			putnext(tr->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * tr_notice_ind: - issue TR_NOTICE_IND primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cid: correlation id
 * @tid: transaction id
 * @cause: report cause (SCCP Return Cause)
 * @dp: transaction user data
 */
static int
tr_notice_ind(struct tr *tr, queue_t *q, mblk_t *msg,
	      t_uscalar_t cid, t_uscalar_t tid, t_uscalar_t cause, mblk_t *dp)
{
	struct TR_notice_ind *p;
	mblk_t *mp;

	(void) tr_notice_ind;
	if (bcanputnext(tr->oq, dp->b_band)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_NOTICE_IND;
			p->CORR_id = cid;
			p->TRANS_id = tid;
			p->REPORT_cause = cause;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- TR_NOTICE_IND");
			putnext(tr->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  SCCP User -> SCCP Provider primitives.
 *
 *  =========================================================================
 */

/**
 * sc_conn_req: - issue N_CONN_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: connection flags
 * @dst_len: destination address length
 * @dst_ptr: destination address pointer
 * @qos_len: QOS parameters length
 * @qos_ptr: QOS parameters pointer
 * @dp: SCCP user data
 */
static int
sc_conn_req(struct sc *sc, queue_t *q, mblk_t *msg,
	    t_uscalar_t flags,
	    size_t dst_len, caddr_t dst_ptr, size_t qos_len, caddr_t qos_ptr, mblk_t *dp)
{
	N_conn_req_t *p;
	mblk_t *mp;

	(void) sc_conn_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + dst_len + qos_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_CONN_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_conn_res: - issue N_CONN_RES primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tok: token value
 * @seq: sequence number
 * @flags: connection flags
 * @res_len: responding address length
 * @res_ptr: responding address pointer
 * @qos_len: QOS parameters length
 * @qos_ptr: QOS parameters pointer
 * @dp: SCCP user data
 */
static int
sc_conn_res(struct sc *sc, queue_t *q, mblk_t *msg,
	    t_uscalar_t tok, t_uscalar_t seq, t_uscalar_t flags,
	    size_t res_len, caddr_t res_ptr, size_t qos_len, caddr_t qos_ptr, mblk_t *dp)
{
	N_conn_res_t *p;
	mblk_t *mp;

	(void) sc_conn_res;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + res_len + qos_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_RES;
			p->TOKEN_value = tok;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->SEQ_number = seq;
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_CONN_RES ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_discon_req: - issue N_DISCON_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: disconnect reason
 * @seq: sequence number
 * @res_len: responding address length
 * @res_ptr: responding address pointer
 * @dp: SCCP user data
 */
static int
sc_discon_req(struct sc *sc, queue_t *q, mblk_t *msg,
	      t_uscalar_t reason, t_uscalar_t seq, size_t res_len, caddr_t res_ptr, mblk_t *dp)
{
	N_discon_req_t *p;
	mblk_t *mp;

	(void) sc_discon_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + res_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_REQ;
			p->DISCON_reason = reason;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_DISCON_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_data_req: - issue N_DATA_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flagsj
 */
static int
sc_data_req(struct sc *sc, queue_t *q, mblk_t *msg, t_uscalar_t flags, mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;

	(void) sc_data_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGDA, SL_TRACE, "N_DATA_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_exdata_req: - issue N_EXDATA_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sc_exdata_req(struct sc *sc, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_req_t *p;
	mblk_t *mp;

	(void) sc_exdata_req;
	if (bcanputnext(sc->oq, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGDA, SL_TRACE, "N_EXDATA_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_info_req: - issue N_INFO_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sc_info_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_info_req_t *p;
	mblk_t *mp;

	(void) sc_info_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "N_INFO_REQ ->");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sc_bind_req: - issue N_BIND_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @coninds: maximum number of request outstanding connection indications
 * @flags: bind flags
 * @add_len: address length
 * @add_ptr: address pointer
 * @pro_len: protocol ids length
 * @pro_ptr: protocol ids pointer
 */
static int
sc_bind_req(struct sc *sc, queue_t *q, mblk_t *msg,
	    np_ulong coninds, np_ulong flags,
	    size_t add_len, caddr_t add_ptr, size_t pro_len, caddr_t pro_ptr)
{
	N_bind_req_t *p;
	mblk_t *mp;

	(void) sc_bind_req;
	if ((mp = mi_allocb(q, sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = coninds;
		p->BIND_flags = flags;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(pro_ptr, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "N_BIND_REQ ->");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sc_unbind_req: - issue N_UNBIND_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sc_unbind_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	(void) sc_unbind_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "N_UNBIND_REQ ->");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sc_unitdata_req: - issue N_UNITDATA_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dst_len: destination address length
 * @dst_ptr: destination address pointer
 * @dp: SCCP user data
 */
static int
sc_unitdata_req(struct sc *sc, queue_t *q, mblk_t *msg, size_t dst_len, caddr_t dst_ptr, mblk_t *dp)
{
	N_unitdata_req_t *p;
	mblk_t *mp;

	(void) sc_unitdata_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->RESERVED_field[0] = 0;
			p->RESERVED_field[1] = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGDA, SL_TRACE, "N_UNITDATA_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_optmgmt_req: - issue N_OPTMGMT_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: options flags
 * @qos_len: QOS parameters length
 * @qos_ptr: QOS parameters pointer
 */
static int
sc_optmgmt_req(struct sc *sc, queue_t *q, mblk_t *msg,
	       np_ulong flags, size_t qos_len, caddr_t qos_ptr)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;

	(void) sc_optmgmt_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + qos_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_OPTMGMT_REQ;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) : 0;
			p->OPTMGMT_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_OPTMGMT_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_datack_req: - issue N_DATACK_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sc_datack_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_datack_req_t *p;
	mblk_t *mp;

	(void) sc_datack_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGDA, SL_TRACE, "N_DATACK_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_reset_req: - issue N_RESET_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: reset reason
 */
static int
sc_reset_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_reset_req_t *p;
	mblk_t *mp;

	(void) sc_reset_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_REQ;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_RESET_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_reset_res: - issue N_RESET_RES primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sc_reset_res(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_reset_res_t *p;
	mblk_t *mp;

	(void) sc_reset_res;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_RES;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_RESET_RES ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_inform_req: - issue N_INFORM_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: inform reason
 * @qos_len: QOS parameter length
 * @qos_ptr: QOS parameter pointer
 */
static int
sc_inform_req(struct sc *sc, queue_t *q, mblk_t *msg,
	      np_ulong reason, size_t qos_len, caddr_t qos_ptr)
{
	N_inform_req_t *p;
	mblk_t *mp;

	(void) sc_inform_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + qos_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_INFORM_REQ;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) : 0;
			p->REASON = reason;
			mp->b_wptr += sizeof(*p);
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_INFORM_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_coord_req: - issue N_COORD_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add_len: address length
 * @add_ptr: address pointer
 */
static int
sc_coord_req(struct sc *sc, queue_t *q, mblk_t *msg, size_t add_len, caddr_t add_ptr)
{
	N_coord_req_t *p;
	mblk_t *mp;

	(void) sc_coord_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_COORD_REQ;
			p->ADDR_length = add_len;
			p->ADDR_offset = add_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_COORD_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_coord_res: - issue N_COORD_RES primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: destination address
 */
static int
sc_coord_res(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *add)
{
	N_coord_res_t *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + PAD4(add_len);

	(void) sc_coord_res;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_COORD_RES;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += PAD4(add_len);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_COORD_RES ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_state_req: - issue N_STATE_REQ primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @status: status
 * @add_len: address length
 * @add_ptr: address pointer
 */
static int
sc_state_req(struct sc *sc, queue_t *q, mblk_t *msg,
	     np_ulong status, size_t add_len, caddr_t add_ptr)
{
	N_state_req_t *p;
	mblk_t *mp;

	(void) sc_state_req;
	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_STATE_REQ;
			p->ADDR_length = add_len;
			p->ADDR_offset = add_len ? sizeof(*p) : 0;
			p->STATUS = status;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "N_STATE_REQ ->");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  =========================================================================
 */

/* Message Types */
#define	TCAP_MT_UNI		1	/* Unidirectional */
#define	TCAP_MT_QWP		2	/* Query w/ permission */
#define	TCAP_MT_QWOP		3	/* Query w/o permission */
#define	TCAP_MT_CWP		4	/* Conversation w/ permission */
#define	TCAP_MT_CWOP		5	/* Conversation w/o permission */
#define	TCAP_MT_RESP		6	/* Response */
#define	TCAP_MT_ABORT		7	/* Abort */

/* Component Types */
#define	TCAP_CT_INVOKE_L	1	/* Invoke (Last) */
#define	TCAP_CT_INVOKE_NL	2	/* Invoke (Not Last) */
#define	TCAP_CT_RESULT_L	3	/* Return Result (Last) */
#define	TCAP_CT_RESULT_NL	4	/* Return Result (Not Last) */
#define	TCAP_CT_REJECT		5	/* Reject */
#define	TCAP_CT_ERROR		6	/* Error */

#define TCAP_TAG_UNIV_INT	2	/* UNIV Integer */
#define TCAP_TAG_UNIV_OSTR	4	/* UNIV Octet String */
#define TCAP_TAG_UNIV_OID	6	/* UNIV Object Id */
#define	TCAP_TAG_UNIV_PSEQ	16	/* UNIV Parameter Sequence */
#define TCAP_TAG_UNIV_UTC	17	/* UNIV Universal Time */
#define TCAP_TAG_UNIV_SEQ	48	/* UNIV Sequence */

#define	TCAP_TAG_TCAP_ACG	 1	/* TCAP ACG Indicators */
#define	TCAP_TAG_TCAP_STA	 2	/* TCAP Standard Announcement */
#define TCAP_TAG_TCAP_CUA	 3	/* TCAP Customized Announcment */
#define	TCAP_TAG_TCAP_TDIG	 4	/* TCAP Digits */
#define	TCAP_TAG_TCAP_SUEC	 5	/* TCAP Standard User Error Code */
#define	TCAP_TAG_TCAP_PDTA	 6	/* TCAP Problem Data */
#define	TCAP_TAG_TCAP_TCGPA	 7	/* TCAP SCCP Calling Party Address */
#define	TCAP_TAG_TCAP_TRSID	 8	/* TCAP Transaction ID */
#define	TCAP_TAG_TCAP_PCTY	 9	/* TCAP Package Type */
#define	TCAP_TAG_TCAP_SKEY	10	/* TCAP Service Key (Constructor) */
#define	TCAP_TAG_TCAP_BISTAT	11	/* TCAP Busy/Idle Status */
#define	TCAP_TAG_TCAP_CFSTAT	12	/* TCAP Call Forwarding Status */
#define	TCAP_TAG_TCAP_ORIGR	13	/* TCAP Origination Restrictions */
#define	TCAP_TAG_TCAP_TERMR	14	/* TCAP Terminating Restrictions */
#define	TCAP_TAG_TCAP_DNMAP	15	/* TCAP DN to Line Service TYpe Mapping */
#define	TCAP_TAG_TCAP_DURTN	16	/* TCAP Duration */
#define	TCAP_TAG_TCAP_RETD	17	/* TCAP Return Data (Constructor) */
#define	TCAP_TAG_TCAP_BCRQ	18	/* TCAP Bearer Capability Requested */
#define	TCAP_TAG_TCAP_BCSUP	19	/* TCAP Bearer Capability Supported */
#define	TCAP_TAG_TCAP_REFID	20	/* TCAP Reference Id */
#define	TCAP_TAG_TCAP_BGROUP	21	/* TCAP Business Group */
#define	TCAP_TAG_TCAP_SNI	22	/* TCAP Signalling Networks Identifier */
#define	TCAP_TAG_TCAP_MWIT	23	/* TCAP Message Waiting Indicator Type */

#define	TCAP_TAG_PRIV_UNI	 1	/* ANSI Unidirectional */
#define	TCAP_TAG_PRIV_QWP	 2	/* ANSI Query w/ permission */
#define	TCAP_TAG_PRIV_QWOP	 3	/* ANSI Query w/o permission */
#define	TCAP_TAG_PRIV_RESP	 4	/* ANSI Response */
#define	TCAP_TAG_PRIV_CWP	 5	/* ANSI Conversaion w/ permission */
#define	TCAP_TAG_PRIV_CWOP	 6	/* ANSI Conversaion w/o permission */
#define	TCAP_TAG_PRIV_TRSID	 7	/* ANSI Transaction Id */
#define	TCAP_TAG_PRIV_CSEQ	 8	/* ANSI Component Sequence */
#define	TCAP_TAG_PRIV_INKL	 9	/* ANSI Invoke (Last) */
#define	TCAP_TAG_PRIV_RRL	10	/* ANSI Return Result (Last) */
#define	TCAP_TAG_PRIV_RER	11	/* ANSI Return Error */
#define	TCAP_TAG_PRIV_REJ	12	/* ANSI Reject */
#define	TCAP_TAG_PRIV_INK	13	/* ANSI Invoke (Not Last) */
#define	TCAP_TAG_PRIV_RR	14	/* ANSI Result (Not Last) */
#define	TCAP_TAG_PRIV_CORID	15	/* ANSI Correlation Id(s) */
#define	TCAP_TAG_PRIV_NOPCO	16	/* ANSI National Operation Code */
#define	TCAP_TAG_PRIV_POPCO	17	/* ANSI Private Operation Code */
#define	TCAP_TAG_PRIV_PSET	18	/* ANSI Parameter Set */
#define TCAP_TAG_PRIV_NECODE	19	/* ANSI National Error Code */
#define TCAP_TAG_PRIV_PECODE	20	/* ANSI Private Error Code */
#define	TCAP_TAG_PRIV_PBCODE	21	/* ANSI Reject Problem Code */
#define	TCAP_TAG_PRIV_PSEQ	21	/* ANSI Parameter Sequence */
#define	TCAP_TAG_PRIV_ABORT	22	/* ANSI Abort */
#define	TCAP_TAG_PRIV_PCAUSE	23	/* ANSI P-Abort Cause */
#define	TCAP_TAG_PRIV_U_ABORT	24	/* ANSI User Abort Information */
#define	TCAP_TAG_PRIV_DLGP	25	/* ANSI Dialog Portion */
#define	TCAP_TAG_PRIV_VERSION	26	/* ANSI Protocol Version */
#define	TCAP_TAG_PRIV_CONTEXT	27	/* ANSI Integer Application Context */
#define	TCAP_TAG_PRIV_CTX_OID	28	/* ANSI OID Application Context */
#define	TCAP_TAG_PRIV_UINFO	29	/* ANSI User Information */

#define TCAP_TAG_APPL_UNI	1	/* ITUT Unidirectional */
#define TCAP_TAG_APPL_BEGIN	2	/* ITUT Begin Transaction */
#define TCAP_TAG_APPL_END	4	/* ITUT End Transaction */
#define TCAP_TAG_APPL_CONT	5	/* ITUT Continue Transaction */
#define TCAP_TAG_APPL_ABORT	7	/* ITUT Abort Transaction */
#define TCAP_TAG_APPL_ORIGID	8	/* ITUT Origination Transaction Id */
#define TCAP_TAG_APPL_DESTID	9	/* ITUT Destination Transaction Id */
#define TCAP_TAG_APPL_PCAUSE	10	/* ITUT P-Abort Cause */
#define TCAP_TAG_APPL_DLGP	11	/* ITUT Dialog Portion */
#define TCAP_TAG_APPL_CSEQ	12	/* ITUT Component Portion */

#define TCAP_TAG_APPL_AUDT_PDU	0	/* ITUT AUDT APDU */
#define TCAP_TAG_APPL_AARQ_PDU	0	/* ITUT AARQ APDU */
#define TCAP_TAG_APPL_AARE_PDU	1	/* ITUT AARE APDU */
#define TCAP_TAG_APPL_RLRQ_PDU	2	/* ITUT RLRQ APDU */
#define TCAP_TAG_APPL_RLRE_PDU	3	/* ITUT RLRE APDU */
#define TCAP_TAG_APPL_ABRT_PDU	4	/* ITUT ABRT APDU */

#define TCAP_TAG_CNTX_LID	0	/* Linked Id */
#define TCAP_TAG_CNTX_INK	1	/* Invoke */
#define TCAP_TAG_CNTX_RRL	2	/* Return Result (Last) */
#define TCAP_TAG_CNTX_RER	3	/* Return Error */
#define TCAP_TAG_CNTX_REJ	4	/* Reject */
#define TCAP_TAG_CNTX_RR	7	/* Return Result (Not Last) */

#define TCAP_CLS_UNIV_PRIM	0x00	/* universal primitive - class 0 form 0 - 0000 0000 */
#define TCAP_CLS_UNIV_CONS	0x20	/* universal constructor - class 0 form 1 - 0010 0000 */
#define TCAP_CLS_CNTX_PRIM	0x40	/* context-specific primitive - class 1 form 0 - 0100 0000 */
#define TCAP_CLS_CNTX_CONS	0x60	/* context-specific constructor - class 1 form 1 - 0110
					   0000 */
#define TCAP_CLS_APPL_PRIM	0x80	/* application-wide primitive - class 2 form 0 - 1000 0000 */
#define TCAP_CLS_APPL_CONS	0xa0	/* application-wide constructor - class 2 form 1 - 1010
					   0000 */
#define TCAP_CLS_PRIV_PRIM	0xc0	/* private primitive - class 3 form 0 - 1100 0000 */
#define TCAP_CLS_PRIV_CONS	0xe0	/* private constructor - class 3 form 1 - 1110 0000 */

/*
 *  =========================================================================
 *
 *  TCAP SEND MESSAGE FUNCTIONS
 *
 *  =========================================================================
 */

/**
 * tr_send_uni: - send Unidirectional message
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
tr_send_uni(struct tr *tr, queue_t *q, mblk_t *msg)
{
	(void) tr_send_uni;
	freemsg(msg);
	return (0);
}

/**
 * tr_send_qwp: - send Begin or Query w/ Permission message
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
tr_send_qwp(struct tr *tr, queue_t *q, mblk_t *msg)
{
	(void) tr_send_qwp;
	freemsg(msg);
	return (0);
}

/**
 * tr_send_qwop: - send Query w/o Permission message
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
tr_send_qwop(struct tr *tr, queue_t *q, mblk_t *msg)
{
	(void) tr_send_qwop;
	freemsg(msg);
	return (0);
}

/**
 * tr_send_cwp: - send Continue or Conversation w/ Permission message
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
tr_send_cwp(struct tr *tr, queue_t *q, mblk_t *msg)
{
	(void) tr_send_cwp;
	freemsg(msg);
	return (0);
}

/**
 * tr_send_cwop: - send Conversation w/o Permission message
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
tr_send_cwop(struct tr *tr, queue_t *q, mblk_t *msg)
{
	(void) tr_send_cwop;
	freemsg(msg);
	return (0);
}

/**
 * tr_send_resp: - send End or Response message
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
tr_send_resp(struct tr *tr, queue_t *q, mblk_t *msg)
{
	(void) tr_send_resp;
	freemsg(msg);
	return (0);
}

/**
 * tr_send_abort: - send Abort message
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
tr_send_abort(struct tr *tr, queue_t *q, mblk_t *msg)
{
	(void) tr_send_abort;
	freemsg(msg);
	return (0);
}

/*
 *  =========================================================================
 *
 *  TCAP RECEIVE MESSAGE FUNCTIONS
 *
 *  =========================================================================
 */

struct tr_val {
	uint cls;
	uint tag;
	uint len;
	uchar *tagp;
	uchar *lenp;
	uchar *valp;
	uchar *endp;
};

struct tr_msg {
	uint flags;
	struct sccp_addr *orig;		/* originating address */
	struct sccp_addr *dest;		/* destination address */
	N_qos_sel_data_sccp_t *qos;	/* quality of service parameters */
	uint mt;			/* package type */
	struct tr_val trp;		/* transaction portion */
	struct tr_val ids;		/* trasnaction ids */
	struct tr_val oid;		/* originating transaction id */
	struct tr_val did;		/* destination transaction id */
	struct tr_val dlg;		/* dialog portion */
	struct tr_val cmp;		/* component sequence */
	struct tr_val abt;		/* P-AbortCause */
	struct tr_val inf;		/* UserInformation */
};

/*
 * The ANSI Dialog Portion consists of a number of optional fields: an optional Protocol Version, an
 * optional Application Context, an optinal User Information, an optional Security Context, and an
 * optional Confidentiality.  These fields or constructors are delivered as options in the
 * indication or confirmation primitive.  When the Dialog Portion is not present, none of these
 * options are included in the indication or confirmation primitive.  When the Dialog Portion is
 * present, but does not contain one of these fields, the missing field is not included in the
 * options included in the indication or confirmation primitive.
 *
 * The ANSI Component Portion consists of a sequence of Component PDU, each which contains an
 * InvokeLast, InvokeNotLast, ReturnResultLast, ReturnResultNotLast, ReturnError or Reject component.
 * The sequence of components is delivered in the M_DATA portion of the indication or confirmation
 * primitive.  When the Component Portion is not present, no M_DATA block is attached to the
 * indication or confirmation.  When the Component Portion is empty, a zero-length M_DATA block is
 * attached to the indication or confirmation primitive.
 */

static noinline fastcall __unlikely int
tc_unpack_tag_extended(struct tr_val *v)
{
	uchar ptag;
	int err;

	v->tag = 0;
	if (v->lenp >= v->endp)
		goto emsgsize;
	v->tag <<= 7;
	ptag = *v->lenp++;
	v->tag |= (ptag & 0x7f);
	if (ptag & 0x80) {
		if (v->lenp >= v->endp)
			goto emsgsize;
		v->tag <<= 7;
		ptag = *v->lenp++;
		v->tag |= (ptag & 0x7f);
		if (ptag & 0x80) {
			if (v->lenp >= v->endp)
				goto emsgsize;
			v->tag <<= 7;
			ptag = *v->lenp++;
			v->tag |= (ptag & 0x7f);
			if (ptag & 0x80) {
				if (v->lenp >= v->endp)
					goto emsgsize;
				v->tag <<= 7;
				ptag = *v->lenp++;
				v->tag |= (ptag & 0x7f);
				if (ptag & 0x80) {
					goto eproto;
				}
			}
		}
	}
	return (0);
      eproto:
	err = -EPROTO;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

static noinline fastcall __unlikely int
tc_unpack_len_extended(struct tr_val *v)
{
	uint plen = (v->len & 0x7f);
	int err;

	if (plen > 4 || plen == 0)
		goto eproto;
	v->len = 0;
	while (plen--) {
		if (v->valp >= v->endp)
			goto emsgsize;
		v->len <<= 8;
		v->len |= *v->valp++;
	}
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      eproto:
	err = -EPROTO;
	goto error;
      error:
	return (err);
}

/**
 * tc_unpack_taglen: - unpack a TCAP TLV
 * @v: pointer to tr_val structure
 *
 * On entry, the v->tagp pointer must point to the start of the extent to be unpacked and v->endp
 * must point to the byte after the end of the extent.  Upon return, all fields will be set
 * appropriately.
 */
static noinline fastcall __hot_in int
tc_unpack_taglen(struct tr_val *v)
{
	uint tac;
	int err;

	if (unlikely(v->tagp >= v->endp))
		goto emsgsize;
	v->lenp = v->tagp;
	tac = *v->lenp++;
	v->tag = (tac & 0x1f);
	v->cls = (tac & 0xe0);
	if (unlikely((v->tag & 0x1f) == 0x1f) && unlikely((err = tc_unpack_tag_extended(v))))
		goto error;
	if (unlikely(v->lenp >= v->endp))
		goto emsgsize;
	v->valp = v->lenp;
	v->len = *v->valp++;
	if (unlikely((v->len & 0x80) == 0x80) && unlikely((err = tc_unpack_len_extended(v))))
		goto error;
	if (unlikely(v->valp + v->len > v->endp))
		goto emsgsize;
	v->endp = v->valp + v->len;
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

#define TF_VAR_IDS 0x0001
#define TF_VAR_DLG 0x0002
#define TF_VAR_CMP 0x0004

/**
 * tr_recv_priv_uni: - receive Unidirectional message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the empty Transaction IDs, and attach the dialog portion and component portion of an M_DATA
 * message to a completed TR_UNI_IND primitive and deliver it.
 */
static int
tr_recv_priv_uni(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	int err;

	m->mt = TCAP_MT_UNI;
	/* Empty Trans Ids, [Dialog Portion], Component Portion */
	if (unlikely((m->ids.tagp = m->trp.valp) >= (m->ids.endp = m->trp.endp)))
		goto emsgsize;
	/* Empty Trans Ids */
	if (m->ids.tagp[0] == (TCAP_CLS_PRIV_PRIM | TCAP_TAG_PRIV_TRSID)) {
		if ((err = tc_unpack_taglen(&m->ids)))
			goto error;
		if (m->ids.len != 0)
			goto eproto;
		m->flags |= TF_VAR_IDS;
		if (unlikely((m->dlg.tagp = m->ids.endp) >= (m->dlg.endp = m->trp.endp)))
			goto emsgsize;
	} else
		goto eproto;
	/* [Dialog Portion] */
	if (m->dlg.tagp[0] == (TCAP_CLS_PRIV_CONS | TCAP_TAG_PRIV_DLGP)) {
		if ((err = tc_unpack_taglen(&m->dlg)))
			goto error;
		m->flags |= TF_VAR_DLG;
		if (unlikely((m->cmp.tagp = m->dlg.endp) >= (m->cmp.endp = m->trp.endp)))
			goto emsgsize;
	} else {
		if (unlikely((m->cmp.tagp = m->ids.endp) >= (m->cmp.endp = m->trp.endp)))
			goto emsgsize;
	}
	/* Components */
	if (m->cmp.tagp[0] == (TCAP_CLS_PRIV_CONS | TCAP_TAG_PRIV_CSEQ)) {
		if ((err = tc_unpack_taglen(&m->cmp)))
			goto error;
		m->flags |= TF_VAR_CMP;
	} else
		goto eproto;
	return tr_uni_ind(tr, q, mp, 0, NULL, 0, NULL, 0, NULL, NULL);
      eproto:
	err = -EPROTO;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	/* Never need to abort unidirectional messages. */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_priv_qwp: - receive Query w/ Permission message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the originating trasnaction ID.  Create a transaction record with the given originating
 * transaction ID, assign a local interface transaction ID.  Set the state of the transaction to
 * begin indicated and deliver a TR_BEGIN_IND with the dialog and component portion in an attached
 * M_DATA message block.  Clear the ASSOC_flags and set permission in the transaction record.
 */
static int
tr_recv_priv_qwp(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_QWP;
	/* Orig Trans Id, [Dialog Portion], [Component Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_priv_qwop: - receive Query w/o Permission message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the originating transaction ID.  Create a transaction record with the given originating
 * transaction ID, assign a local interface transaction ID.  Set the state of the transaction to
 * begin indicated and deliver a TR_BEGIN_IND with the dialog and component portion in an attached
 * M_DATA message block.  Set the ASSOC_flags and clear permission in the transaction record.
 */
static int
tr_recv_priv_qwop(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_QWOP;
	/* Orig Trans Id, [Dialog Portion], [Component Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_priv_cwp: - receive Conversation w/ Permission message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the orig and dest transaction IDs.  Look up the transaction record.  Set the state of the
 * transaction to continue indicated and deliver a TR_CONT_IND with the dialog and component portion
 * in an attached M_DATA message block.  Clear the ASSOC_flags and set permission in the transaction
 * record.
 */
static int
tr_recv_priv_cwp(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_CWP;
	/* Orig and Dest Trans Id, [Dialog Portion], [Component Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_priv_cwop: - receive Conversation w/o Permission message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the orig and dest transaction IDs.  Look up the transaction record.  Set the state of the
 * transaction to continue indicated and deliver a TR_CONT_IND with the dialog and component portion
 * in an attached M_DATA message block.  Set the ASSOC_flags and clear permission in the transaction
 * record.
 */
static int
tr_recv_priv_cwop(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_CWOP;
	/* Orig and Dest Trans Id, [Dialog Portion], [Component Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_priv_resp: - receive Response message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the destination transaction ID.  Look up the transaction record.  Set the state of the
 * transaction to response indicated and deliver TR_END_IND with the dialog and component portion
 * in an attach M_DATA message block.  Set the destruction time on the transaction record.
 */
static int
tr_recv_priv_resp(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_RESP;
	/* Dest Trans Id, [Dialog Portion], [Component Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_priv_abt: - receive Abort message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the destination transaction ID.  Look up the transaction record.  Set the state of the
 * transaction to aborted indicated and deliver TR_ABORT_IND wth the P-Abort-cause and the dialog
 * portion and UserAbortInformation in the attached M_DATA message block.  Set the destruction time
 * on the transaction record. 
 */
static int
tr_recv_priv_abt(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_ABORT;
	/* Dest Trans Id, [Dialog Portion], P-Abort-cause or [U-Abort-Info] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_appl_uni: - receive Unidirectional message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Attach the dialog portion and component portion of an M_DATA message to a completed TR_UNI_IND
 * primitive and deliver it.
 */
static int
tr_recv_appl_uni(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_UNI;
	/* No Transaction Id, [Dialog Portion], Component Portion */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_appl_beg: - receive Begin message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the originating trasnaction ID.  Create a transaction record with the given originating
 * address and originating transaction ID, assign a local interface transaction ID.  Set the state
 * of the transaction to begin indicated and deliver a TR_BEGIN_IND with the dialog and component
 * portion in an attached M_DATA message block.  Clear the ASSOC_flags and set permission in the
 * transaction record.
 */
static int
tr_recv_appl_beg(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_QWP;
	/* Orig Transaction Id, [Dialog Portion], [Component Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_appl_cnt: - receive Continue message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the orig and dest transaction IDs.  Look up the transaction record.  Set the state of the
 * transaction to continue indicated and deliver a TR_CONT_IND with the dialog and component portion
 * in an attached M_DATA message block.  Clear the ASSOC_flags and set permission in the transaction
 * record.
 */
static int
tr_recv_appl_cnt(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_CWP;
	/* Orig and Dest Transaction Id, [Dialog Portion], [Component Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_appl_end: - receive End message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the destination transaction ID.  Look up the transaction record.  Set the state of the
 * transaction to response indicated and deliver TR_END_IND with the dialog and component portion
 * in an attach M_DATA message block.  Set the destruction time on the transaction record.
 */
static int
tr_recv_appl_end(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_RESP;
	/* Dest Transaction Id, [Dialog Portion], [Component Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_appl_abt: - receive Abort message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @m: message decoding cookie
 *
 * Strip the destination transaction ID.  Look up the transaction record.  Set the state of the
 * transaction to aborted indicated and deliver TR_ABORT_IND wth the P-Abort-cause and the dialog
 * portion and UserAbortInformation in the attached M_DATA message block.  Set the destruction time
 * on the transaction record. 
 */
static int
tr_recv_appl_abt(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	m->mt = TCAP_MT_ABORT;
	/* Dest Transaction Id, [P-Abort-Cause or Dialog Portion] */
	freemsg(mp);
	return (0);
}

/**
 * tr_recv_msg: - receive message
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @dp: the data portion of the message
 */
static int
tr_recv_msg(struct tr *tr, queue_t *q, mblk_t *mp, struct tr_msg *m, mblk_t *dp)
{
	struct tr_val *v = &m->trp;
	int err;

	if (!pullupmsg(dp, -1))
		goto enobufs;

	v->tagp = mp->b_cont->b_rptr;
	v->endp = mp->b_cont->b_wptr;

	if ((err = tc_unpack_taglen(v)))
		goto error;

	switch (v->cls) {
	case 0xe0:		/* private constructor */
		switch (v->tag) {
		case TCAP_TAG_PRIV_UNI:
			return tr_recv_priv_uni(tr, q, mp, m);
		case TCAP_TAG_PRIV_QWP:
			return tr_recv_priv_qwp(tr, q, mp, m);
		case TCAP_TAG_PRIV_QWOP:
			return tr_recv_priv_qwop(tr, q, mp, m);
		case TCAP_TAG_PRIV_RESP:
			return tr_recv_priv_resp(tr, q, mp, m);
		case TCAP_TAG_PRIV_CWP:
			return tr_recv_priv_cwp(tr, q, mp, m);
		case TCAP_TAG_PRIV_CWOP:
			return tr_recv_priv_cwop(tr, q, mp, m);
		case TCAP_TAG_PRIV_ABORT:
			return tr_recv_priv_abt(tr, q, mp, m);
		}
		/* FIXME: need to send tcap abort with P-Abort cause TR_APPL_UNREC_MSG_TYPE. */
		freemsg(mp);
		return (0);
	case 0xa0:		/* application-wide constructor */
		switch (v->tag) {
		case TCAP_TAG_APPL_UNI:
			return tr_recv_appl_uni(tr, q, mp, m);
		case TCAP_TAG_APPL_BEGIN:
			return tr_recv_appl_beg(tr, q, mp, m);
		case TCAP_TAG_APPL_END:
			return tr_recv_appl_end(tr, q, mp, m);
		case TCAP_TAG_APPL_CONT:
			return tr_recv_appl_cnt(tr, q, mp, m);
		case TCAP_TAG_APPL_ABORT:
			return tr_recv_appl_abt(tr, q, mp, m);
		}
		/* FIXME: need to send tcap abort with P-Abort cause TR_PRIV_UNREC_PKG_TYPE. */
		freemsg(mp);
		return (0);
	default:
		/* Invalid format of transaction portion */
		goto enotsup;
	}
      enotsup:
	err = -EOPNOTSUPP;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  TR User -> TR Provider primitives.
 *
 *  =========================================================================
 */

/**
 * tr_info_req: - process TR_INFO_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_INFO_REQ primitive
 */
static int
tr_info_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_INFO_REQ, err, 0);
}

/**
 * tr_bind_req: - process TR_BIND_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_BIND_REQ primitive
 */
static int
tr_bind_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_bind_req;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_BIND_REQ, err, 0);
}

/**
 * tr_unbind_req: - process TR_UNBIND_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_UNBIND_REQ primitive
 */
static int
tr_unbind_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_unbind_req;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_UNBIND_REQ, err, 0);
}

/**
 * tr_optmgmt_req: - process TR_OPTMGMT_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_OPTMGMT_REQ primitive
 */
static int
tr_optmgmt_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_optmgmt_req;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_OPTMGMT_REQ, err, 0);
}

/**
 * tr_uni_req: - process TR_UNI_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_UNI_REQ primitive
 */
static int
tr_uni_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_uni_req *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_uni_req;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_UNI_REQ, err, 0);
}

/**
 * tr_begin_req: - process TR_BEGIN_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_BEGIN_REQ primitive
 */
static int
tr_begin_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_begin_req *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_begin_req;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_BEGIN_REQ, err, 0);
}

/**
 * tr_begin_res: - process TR_BEGIN_RES primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_BEGIN_RES primitive
 */
static int
tr_begin_res(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_begin_res *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_begin_res;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_BEGIN_RES, err, 0);
}

/**
 * tr_cont_req: - process TR_CONT_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_CONT_REQ primitive
 */
static int
tr_cont_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_cont_req *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_cont_req;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_CONT_REQ, err, 0);
}

/**
 * tr_end_req: - process TR_END_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_END_REQ primitive
 */
static int
tr_end_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_end_req *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_end_req;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_END_REQ, err, 0);
}

/**
 * tr_abort_req: - process TR_ABORT_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_ABORT_REQ primitive
 */
static int
tr_abort_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_abort_req *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) tr_abort_req;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, TR_ABORT_REQ, err, 0);
}

/**
 * tr_other_req: - process TR_????_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TR_????_REQ primitive
 */
static int
tr_other_req(struct tr *tr, queue_t *q, mblk_t *mp)
{
	t_uscalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tr_error_ack(tr, q, mp, *p, err, 0);
}

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  SCCP Provider -> SCCP User primitives.
 *
 *  =========================================================================
 */

/**
 * sc_conn_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_conn_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_conn_con: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_conn_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_discon_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_discon_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_data_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_data_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_exdata_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_exdata_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_info_ack: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_info_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_bind_ack: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_bind_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_error_ack: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_error_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_ok_ack: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_ok_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_unitdata_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_unitdata_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	struct tr_msg msg;
	mblk_t *dp;
	size_t dlen;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (!p->SRC_length)
		goto noaddr;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badprim;
	if (p->SRC_length < sizeof(*msg.orig))
		goto badaddr;
	msg.orig = (typeof(msg.orig)) (mp->b_rptr + p->SRC_offset);
	if (p->SRC_length < sizeof(*msg.orig) + msg.orig->alen)
		goto badaddr;
	if (p->DEST_length) {
		if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
			goto badprim;
		if (p->DEST_length < sizeof(*msg.dest))
			goto badaddr;
		msg.dest = (typeof(msg.dest)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length < sizeof(*msg.dest) + msg.dest->alen)
			goto badaddr;
		if (msg.dest->ni != sc->add.ni || msg.dest->pc != sc->add.pc
		    || msg.dest->ssn != sc->add.ssn)
			goto badaddr;
	} else {
		msg.dest = &sc->add;
	}
#if 0
	if (p->QOS_length) {
		if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
			goto badprim;
		if (p->QOS_length < sizeof(*msg.qos))
			goto badqostype;
		msg.qos = (typeof(msg.qos)) (mp->b_rptr + p->QOS_offset);
		if (msg.qos->n_qos_type != N_QOS_SEL_DATA_SCCP)
			goto badqostype;
	} else {
		msg.qos = &sc->qos;
	}
#endif

	if (!(dp = mp->b_cont) || !(dlen = msgdsize(mp)) || dlen > sc->prot.NSDU_size)
		goto baddata;
	if (!pullupmsg(dp, -1)) {
		mi_bufcall(q, dlen, BPRI_MED);
		goto enobufs;
	}
	return tr_recv_msg(sc->tr, q, mp, &msg, mp->b_cont);

#if 0
      badqostype:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: bad quality of service type", __FUNCTION__);
	goto discard;
#endif
      baddata:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: bad data", __FUNCTION__);
	goto discard;
      badaddr:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: bad address", __FUNCTION__);
	goto discard;
      noaddr:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: could not assign address", __FUNCTION__);
	goto discard;
      badprim:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: invalid primitive format", __FUNCTION__);
	goto discard;
      discard:
	freemsg(mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/**
 * sc_uderror_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_uderror_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_datack_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_datack_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_reset_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_reset_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_reset_con: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_reset_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_notice_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_notice_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_notice_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_coord_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_coord_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_coord_con: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_coord_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_state_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_state_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_state_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_pcstate_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_pcstate_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_pcstate_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_traffic_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_traffic_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_traffic_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/**
 * sc_other_ind: - process N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
static int
sc_other_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	np_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	freemsg(mp);
	return (0);
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(sc->tr, q, mp, err);
}

/*
 *  =========================================================================
 *
 *  TCAP INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
 */

static int
tr_testoption(struct tr *tr, struct tcap_option *arg)
{
	/* FIXME: check options */
	return (0);
}
static int
tr_setoption(struct tr *tr, struct tcap_option *arg)
{
	int err;

	if ((err = tr_testoption(tr, arg)) != 0)
		return (err);
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		df.config = arg->options[0].df;
		break;
	case TCAP_OBJ_TYPE_SP:
		tr->sp.config = arg->options[0].sp;
		break;
	case TCAP_OBJ_TYPE_SC:
		tr->sc->config = arg->options[0].sc;
		break;
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		err = EINVAL;
		break;
	}
	return (0);
}
static int
tr_testconfig(struct tr *tr, struct tcap_config *arg)
{
	/* FIXME: check configuration */
	return (0);
}
static int
tr_setconfig(struct tr *tr, struct tcap_config *arg)
{
	int err;

	if ((err = tr_testconfig(tr, arg)) != 0)
		return (err);
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		// df.config = arg->config[0].df;
		break;
	case TCAP_OBJ_TYPE_SP:
		// tr->sp.config = arg->config[0].sp;
		break;
	case TCAP_OBJ_TYPE_SC:
		// tr->sc->config = arg->config[0].sc;
		break;
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (EINVAL);
	}
	return (0);
}
static int
tr_setstatsp(struct tr *tr, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		df.statsp = arg->stats[0].df;
		break;
	case TCAP_OBJ_TYPE_SP:
		tr->sp.statsp = arg->stats[0].sp;
		break;
	case TCAP_OBJ_TYPE_SC:
		tr->sc->statsp = arg->stats[0].sc;
		break;
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (EINVAL);
	}
	return (0);
}
static int
tr_setnotify(struct tr *tr, struct tcap_notify *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		df.notify.events |= arg->notify[0].df.events;
		arg->notify[0].df.events = df.notify.events;
		break;
	case TCAP_OBJ_TYPE_SP:
		tr->sp.notify.events |= arg->notify[0].sp.events;
		arg->notify[0].sp.events = tr->sp.notify.events;
		break;
	case TCAP_OBJ_TYPE_SC:
		tr->sc->notify.events |= arg->notify[0].sc.events;
		arg->notify[0].sc.events = tr->sc->notify.events;
		break;
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (EINVAL);
	}
	return (0);
}
static int
tr_clrnotify(struct tr *tr, struct tcap_notify *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		df.notify.events &= ~arg->notify[0].df.events;
		arg->notify[0].df.events = df.notify.events;
		break;
	case TCAP_OBJ_TYPE_SP:
		tr->sp.notify.events &= ~arg->notify[0].sp.events;
		arg->notify[0].sp.events = tr->sp.notify.events;
		break;
	case TCAP_OBJ_TYPE_SC:
		tr->sc->notify.events &= ~arg->notify[0].sc.events;
		arg->notify[0].sc.events = tr->sc->notify.events;
		break;
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (EINVAL);
	}
	return (0);
}

static int
tr_size_options(struct tr *tr, struct tcap_option *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.config));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tr->sp.config));
	case TCAP_OBJ_TYPE_SC:
		return (sizeof(tr->sc->config));
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tr_size_config(struct tr *tr, struct tcap_config *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.config));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tr->sp.config));
	case TCAP_OBJ_TYPE_SC:
		return (sizeof(tr->sc->config));
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tr_size_statem(struct tr *tr, struct tcap_statem *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.statem));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tr->sp.statem));
	case TCAP_OBJ_TYPE_SC:
		return (sizeof(tr->sc->statem));
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tr_size_statsp(struct tr *tr, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.statsp));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tr->sp.statsp));
	case TCAP_OBJ_TYPE_SC:
		return (sizeof(tr->sc->statsp));
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tr_size_stats(struct tr *tr, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.stats));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tr->sp.stats));
	case TCAP_OBJ_TYPE_SC:
		return (sizeof(tr->sc->stats));
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tr_size_notify(struct tr *tr, struct tcap_notify *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.notify));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tr->sp.notify));
	case TCAP_OBJ_TYPE_SC:
		return (sizeof(tr->sc->notify));
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}

static void *
tr_ptr_options(struct tr *tr, struct tcap_option *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.config);
	case TCAP_OBJ_TYPE_SP:
		return (&tr->sp.config);
	case TCAP_OBJ_TYPE_SC:
		return (&tr->sc->config);
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tr_ptr_config(struct tr *tr, struct tcap_config *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.config);
	case TCAP_OBJ_TYPE_SP:
		return (&tr->sp.config);
	case TCAP_OBJ_TYPE_SC:
		return (&tr->sc->config);
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tr_ptr_statem(struct tr *tr, struct tcap_statem *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.statem);
	case TCAP_OBJ_TYPE_SP:
		return (&tr->sp.statem);
	case TCAP_OBJ_TYPE_SC:
		return (&tr->sc->statem);
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tr_ptr_statsp(struct tr *tr, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.statsp);
	case TCAP_OBJ_TYPE_SP:
		return (&tr->sp.statsp);
	case TCAP_OBJ_TYPE_SC:
		return (&tr->sc->statsp);
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tr_ptr_stats(struct tr *tr, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.stats);
	case TCAP_OBJ_TYPE_SP:
		return (&tr->sp.stats);
	case TCAP_OBJ_TYPE_SC:
		return (&tr->sc->stats);
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tr_ptr_notify(struct tr *tr, struct tcap_notify *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.notify);
	case TCAP_OBJ_TYPE_SP:
		return (&tr->sp.notify);
	case TCAP_OBJ_TYPE_SC:
		return (&tr->sc->notify);
	default:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}

/**
 * tr_ioctl: - process M_IOCTL message
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the TCAP input-output control operation.  Step 1 consists always consists of a
 * copyin operation to determine the object type and id.
 */
static __unlikely int
tr_ioctl(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", tr_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
	case _IOC_NR(TCAP_IOCSOPTION):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_option));
		break;
	case _IOC_NR(TCAP_IOCGCONFIG):
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_config));
		break;
	case _IOC_NR(TCAP_IOCGSTATEM):
	case _IOC_NR(TCAP_IOCCMRESET):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_statem));
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
	case _IOC_NR(TCAP_IOCSSTATSP):
	case _IOC_NR(TCAP_IOCGSTATS):
	case _IOC_NR(TCAP_IOCCSTATS):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_stats));
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_notify));
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_mgmt));
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_pass));
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	return (0);
}

/**
 * tr_copyin1: - process M_IOCDATA message
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: the copyied in data
 *
 * This is step 2 of the TCAP input-output control operationl.  Step 2 consists of identifying the
 * object and performing a copyout for GET operations and an additional copyin for SET operations.
 */
static __unlikely int
tr_copyin1(struct tr *tr, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int alen, len, size = -1;
	int err = 0;
	mblk_t *bp;
	void *ptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", tr_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
		alen = sizeof(struct tcap_option);
		len = tr_size_options(tr, (struct tcap_option *) dp->b_rptr);
		ptr = tr_ptr_options(tr, (struct tcap_option *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCSOPTION):
		alen = sizeof(struct tcap_option);
		len = tr_size_options(tr, (struct tcap_option *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(TCAP_IOCGCONFIG):
		alen = sizeof(struct tcap_config);
		len = tr_size_config(tr, (struct tcap_config *) dp->b_rptr);
		ptr = tr_ptr_config(tr, (struct tcap_config *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
		alen = sizeof(struct tcap_config);
		len = tr_size_config(tr, (struct tcap_config *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(TCAP_IOCGSTATEM):
		alen = sizeof(struct tcap_statem);
		len = tr_size_statem(tr, (struct tcap_statem *) dp->b_rptr);
		ptr = tr_ptr_statem(tr, (struct tcap_statem *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCCMRESET):
		size = 0;
		switch (((struct tcap_statem *) dp->b_rptr)->type) {
		case TCAP_OBJ_TYPE_DF:
		case TCAP_OBJ_TYPE_SP:
		case TCAP_OBJ_TYPE_SC:
			/* FIXME: reset state machine */
			break;
		default:
		case TCAP_OBJ_TYPE_TC:
		case TCAP_OBJ_TYPE_IV:
		case TCAP_OBJ_TYPE_DG:
		case TCAP_OBJ_TYPE_TR:
			err = EINVAL;
			break;
		}
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
		alen = sizeof(struct tcap_stats);
		len = tr_size_statsp(tr, (struct tcap_stats *) dp->b_rptr);
		ptr = tr_ptr_statsp(tr, (struct tcap_stats *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCSSTATSP):
		alen = sizeof(struct tcap_stats);
		len = tr_size_statsp(tr, (struct tcap_stats *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(TCAP_IOCGSTATS):
		alen = sizeof(struct tcap_stats);
		len = tr_size_stats(tr, (struct tcap_stats *) dp->b_rptr);
		ptr = tr_ptr_stats(tr, (struct tcap_stats *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCCSTATS):
		alen = sizeof(struct tcap_stats);
		len = tr_size_stats(tr, (struct tcap_stats *) dp->b_rptr);
		ptr = tr_ptr_stats(tr, (struct tcap_stats *) dp->b_rptr);
		goto copyout_clear;
	case _IOC_NR(TCAP_IOCGNOTIFY):
		alen = sizeof(struct tcap_notify);
		len = tr_size_notify(tr, (struct tcap_notify *) dp->b_rptr);
		ptr = tr_ptr_notify(tr, (struct tcap_notify *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
		alen = sizeof(struct tcap_notify);
		len = tr_size_notify(tr, (struct tcap_notify *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(TCAP_IOCCMGMT):
		size = 0;
		switch (((struct tcap_mgmt *) dp->b_rptr)->type) {
		case TCAP_OBJ_TYPE_DF:
		case TCAP_OBJ_TYPE_SP:
		case TCAP_OBJ_TYPE_SC:
			/* FIXME: perform command */
			break;
		default:
		case TCAP_OBJ_TYPE_TC:
		case TCAP_OBJ_TYPE_IV:
		case TCAP_OBJ_TYPE_DG:
		case TCAP_OBJ_TYPE_TR:
			err = EINVAL;
			break;
		}
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		err = EOPNOTSUPP;
		break;
	default:
		err = EINVAL;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	      copyin:
		if (len == 0) {
			err = EINVAL;
			break;
		}
		size = alen + len;
		break;
	      copyout:
		if (len == 0 || ptr == NULL) {
			err = EINVAL;
			break;
		}
		if (!(bp = mi_copyout_alloc(q, mp, NULL, alen + len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, alen);
		bcopy(ptr, bp->b_rptr + alen, len);
		break;
	      copyout_clear:
		if (len == 0 || ptr == NULL) {
			err = EINVAL;
			break;
		}
		if (!(bp = mi_copyout_alloc(q, mp, NULL, alen + len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, alen);
		bcopy(ptr, bp->b_rptr + alen, len);
		bzero(ptr, len);
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == 0)
			mi_copy_done(q, mp, 0);
		else if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/**
 * tr_copyin2: - process M_IOCDATA message
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: the copyied in data
 *
 * This is step 3 for SET operations.  This is the result of the implicit or explicit copyin
 * operation for the object specific structure.  We can now perform sets and commits.  All SET
 * operations also include a last copyout step that copies out the information actually set.
 */
static __unlikely int
tr_copyin2(struct tr *tr, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int len, err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", tr_iocname(cp->cp_cmd));

	len = dp->b_wptr - dp->b_rptr;
	if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, len);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(TCAP_IOCSOPTION):
		err = tr_setoption(tr, (struct tcap_option *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCSCONFIG):
		err = tr_setconfig(tr, (struct tcap_config *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCTCONFIG):
		err = tr_testconfig(tr, (struct tcap_config *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCCCONFIG):
		err = tr_setconfig(tr, (struct tcap_config *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCSSTATSP):
		err = tr_setstatsp(tr, (struct tcap_stats *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCSNOTIFY):
		err = tr_setnotify(tr, (struct tcap_notify *) dp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCCNOTIFY):
		err = tr_clrnotify(tr, (struct tcap_notify *) dp->b_rptr);
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * tr_copyout: - process M_IOCDATA message
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 *
 * This is the final step which is a simple copy done operation.
 */
static __unlikely int
tr_copyout(struct tr *tr, queue_t *q, mblk_t *mp)
{
	mi_copyout(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  =========================================================================
 */

/**
 * tr_m_data: - process M_DATA message
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 */
static fastcall int
tr_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * tr_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall int
tr_m_proto(queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	struct tr *tr;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(t_uscalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	tr = TR_PRIV(q);
	tr_save_state(tr);
	sc_save_state(tr->sc);
	switch (prim) {
	case TR_BEGIN_REQ:
	case TR_BEGIN_RES:
	case TR_CONT_REQ:
	case TR_END_REQ:
	case TR_ABORT_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- %s", tr_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", tr_primname(prim));
		break;
	}

	switch (prim) {
	case TR_INFO_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_BIND_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_UNBIND_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_OPTMGMT_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_UNI_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_BEGIN_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_BEGIN_RES:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_CONT_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_END_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	case TR_ABORT_REQ:
		rtn = tr_info_req(tr, q, mp);
		break;
	default:
		rtn = tr_other_req(tr, q, mp);
		break;
	}
	if (rtn) {
		sc_restore_state(tr->sc);
		tr_restore_state(tr);
	}
	mi_unlock(priv);
	return (rtn);

}

/**
 * tr_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (write queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static fastcall int
tr_m_sig(queue_t *q, mblk_t *mp)
{
	struct tr *tr;
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);

	if (!mi_timer_valid(mp))
		return (0);

	tr = TR_PRIV(q);

	tr_save_state(tr);

	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = tr_t1_timeout(tr, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		tr_restore_state(tr);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * tr_m_ioctl: - process M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static fastcall int
tr_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tr *tr;
	caddr_t priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != TCAP_IOC_MAGIC) {
		/* Let driver deal with these. */
		putnext(q, mp);
		return (0);
	}
	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	tr = TR_PRIV(q);
	err = tr_ioctl(tr, q, mp);
	mi_unlock(priv);
	return (err);
}

/**
 * tr_m_iocdata: - process M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static fastcall int
tr_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tr *tr;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != TCAP_IOC_MAGIC) {
		/* Let driver deal with these. */
		putnext(q, mp);
		return (0);
	}
	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	tr = TR_PRIV(q);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = tr_copyin1(tr, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		err = tr_copyin2(tr, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = tr_copyout(tr, q, mp);
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		err = 0;
		break;
	}
	mi_unlock(priv);
	return (err);
}

/**
 * tr_m_flush: - process M_FLUSH message
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 */
static fastcall int
tr_m_flush(queue_t *q, mblk_t *mp)
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
 * tr_m_other: - process unrecognized STREAMS message
 * @q: active queue (write queue)
 * @mp: the unrecognized STREAMS message
 */
static fastcall int
tr_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastcall int
tr_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return tr_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tr_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tr_m_sig(q, mp);
	case M_IOCTL:
		return tr_m_ioctl(q, mp);
	case M_IOCDATA:
		return tr_m_iocdata(q, mp);
	case M_FLUSH:
		return tr_m_flush(q, mp);
	default:
		return tr_m_other(q, mp);
	}
}

/**
 * sc_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static fastcall int
sc_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * sc_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall int
sc_m_proto(queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	struct sc *sc;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(t_uscalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	sc = SC_PRIV(q);
	sc_save_state(sc);
	tr_save_state(sc->tr);
	switch (prim) {
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s <-", sc_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", sc_primname(prim));
		break;
	}
	switch (prim) {
	case N_CONN_IND:
		rtn = sc_conn_ind(sc, q, mp);
		break;
	case N_CONN_CON:
		rtn = sc_conn_con(sc, q, mp);
		break;
	case N_DISCON_IND:
		rtn = sc_discon_ind(sc, q, mp);
		break;
	case N_DATA_IND:
		rtn = sc_data_ind(sc, q, mp);
		break;
	case N_EXDATA_IND:
		rtn = sc_exdata_ind(sc, q, mp);
		break;
	case N_INFO_ACK:
		rtn = sc_info_ack(sc, q, mp);
		break;
	case N_BIND_ACK:
		rtn = sc_bind_ack(sc, q, mp);
		break;
	case N_ERROR_ACK:
		rtn = sc_error_ack(sc, q, mp);
		break;
	case N_OK_ACK:
		rtn = sc_ok_ack(sc, q, mp);
		break;
	case N_UNITDATA_IND:
		rtn = sc_unitdata_ind(sc, q, mp);
		break;
	case N_UDERROR_IND:
		rtn = sc_uderror_ind(sc, q, mp);
		break;
	case N_DATACK_IND:
		rtn = sc_datack_ind(sc, q, mp);
		break;
	case N_RESET_IND:
		rtn = sc_reset_ind(sc, q, mp);
		break;
	case N_RESET_CON:
		rtn = sc_reset_con(sc, q, mp);
		break;
	case N_NOTICE_IND:
		rtn = sc_notice_ind(sc, q, mp);
		break;
	case N_COORD_IND:
		rtn = sc_coord_ind(sc, q, mp);
		break;
	case N_COORD_CON:
		rtn = sc_coord_con(sc, q, mp);
		break;
	case N_STATE_IND:
		rtn = sc_state_ind(sc, q, mp);
		break;
	case N_PCSTATE_IND:
		rtn = sc_pcstate_ind(sc, q, mp);
		break;
	case N_TRAFFIC_IND:
		rtn = sc_traffic_ind(sc, q, mp);
		break;
	default:
		rtn = sc_other_ind(sc, q, mp);
		break;
	}
	if (rtn) {
		tr_restore_state(sc->tr);
		sc_restore_state(sc);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * sc_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (read queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static fastcall int
sc_m_sig(queue_t *q, mblk_t *mp)
{
	struct sc *sc;
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);

	if (!mi_timer_valid(mp))
		return (0);

	sc = SC_PRIV(q);

	sc_save_state(sc);

	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = sc_t1_timeout(sc, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		sc_restore_state(sc);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * sc_m_copy: - process M_COPYIN or M_COPYOUT message
 * @q: active queue (read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 */
static fastcall int
sc_m_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * sc_m_iocack: - process M_IOCACK or M_IOCNAK message
 * @q: active queue (read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 */
static fastcall int
sc_m_iocack(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * sc_m_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 */
static fastcall int
sc_m_flush(queue_t *q, mblk_t *mp)
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
 * sc_m_other: - process unrecognized STREAMS message
 * @q: active queue (read queue)
 * @mp: the unrecognized STREAMS message
 */
static fastcall int
sc_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastcall int
sc_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sc_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sc_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sc_m_sig(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return sc_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return sc_m_iocack(q, mp);
	case M_FLUSH:
		return sc_m_flush(q, mp);
	default:
		return sc_m_other(q, mp);
	}
}

/*
 *  =========================================================================
 *
 *  QUEUE PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */
static streamscall __hot_write int
tr_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tr_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
tr_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tr_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_read int
sc_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sc_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_in int
sc_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sc_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  QUEUE OPEN AND CLOSE ROUTINES
 *
 *  =========================================================================
 */

static caddr_t tr_opens = NULL;

static streamscall int
tr_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(struct TR_info_req), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&tr_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	p = PRIV(q);

	/* initialize private structure */
	p->r_priv.priv = p;
	p->r_priv.tr = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state = NS_NOSTATES;
	p->r_priv.oldstate = NS_NOSTATES;
	/* FIXME initialize some more */
	p->r_priv.qos.n_qos_type = N_QOS_SEL_DATA_SCCP;
	p->r_priv.qos.protocol_class = 0;
	p->r_priv.qos.option_flags = 0;
	p->r_priv.qos.sequence_selection = 0;
	p->r_priv.qos.message_priority = 0;
	p->r_priv.qos.importance = 0;

	p->w_priv.priv = p;
	p->w_priv.sc = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state = TRS_NOSTATES;
	p->w_priv.oldstate = TRS_NOSTATES;
	/* FIXME initialize some more */

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((struct TR_info_req *) mp->b_wptr)->PRIM_type = TR_INFO_REQ;
	mp->b_wptr += sizeof(struct TR_info_req);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static streamscall int
tr_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	(void) p;
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm(&tr_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit tr_rinit = {
	.qi_putp = sc_put,
	.qi_srvp = sc_srv,
	.qi_qopen = tr_qopen,
	.qi_qclose = tr_qclose,
	.qi_minfo = &tr_minfo,
	.qi_mstat = &tr_rstat,
};
static struct qinit tr_winit = {
	.qi_putp = tr_put,
	.qi_srvp = tr_srv,
	.qi_minfo = &tr_minfo,
	.qi_mstat = &tr_wstat,
};
static struct streamtab tr_modinfo = {
	.st_rdinit = &tr_rinit,
	.st_wrinit = &tr_winit,
};

static unsigned short modid = MOD_ID;

/*
 *  =========================================================================
 *
 *  KERNEL MODULE INITIALIZATION
 *
 *  =========================================================================
 */

#ifdef module_param
module_param(modid, short, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module id for TR-MOD module.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw tr_fmod = {
	.f_name = MOD_NAME,
	.f_str = &tr_modinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
tr_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&tr_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
tr_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&tr_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(tr_modinit);
module_exit(tr_modexit);
