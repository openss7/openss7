/*****************************************************************************

 @(#) $RCSfile: tpi_sccp.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-17 14:04:35 $

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

 Last Modified $Date: 2008-11-17 14:04:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tpi_sccp.c,v $
 Revision 0.9.2.1  2008-11-17 14:04:35  brian
 - added documentation and conversion modules

 *****************************************************************************/

#ident "@(#) $RCSfile: tpi_sccp.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-17 14:04:35 $"

static char const ident[] = "$RCSfile: tpi_sccp.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-17 14:04:35 $";

/*
 * This is a pushable STREAMS module that converts between the SCCPI (Signalling Connection Control
 * Part Interface) and TPI (Transport Provider Interface).  SCCPI is, in fact, an extension of the
 * NPI interface: it provides extension primitives in support of ITU-T Recommendation Q.711
 * primitives that are not provided in ITU-T Recommendation X.214 and thus not reflected in the TPI
 * Revision 2.0.0 interface.  The TPI Revision 2.0.0 interface was largely developed in accordance
 * with X.214 in support of OSI COTS and CLTS.  This conversion module enhances the TPI Revision
 * 2.0.0 interface in a way that it can transparently support SCCP to TPI applications and libraries
 * written to support COTS and CLTS.
 */

#define _LFS_SOURCE
#define _MPS_SOURCE
#define _SVR4_SOURCE

#include <sys/os7/compat.h>
#include <sys/tpi.h>
#include <sys/tpi_sccp.h>
#include <ss7/sccpi.h>

#include <linux/bitopts.h>

#define t_tst_bit(nr,addr)  test_bit(nr,addr)
#define t_set_bit(nr,addr)  __set_bit(nr,addr)
#define t_clr_bit(nr,addr) __clear_bit(nr,addr)

#define TPI_SCCP_DESCRIP	"SCCPI to TPI CONVERSION MODULE FOR LINUX FAST-STREAMS"
#define TPI_SCCP_EXTRA		"Part of the OpenSS7 SS7 Stack for Linux Fast-STREAMS"
#define TPI_SCCP_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define TPI_SCCP_REVISION	"OpenSS7 $RCSfile: tpi_sccp.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-17 14:04:35 $"
#define TPI_SCCP_DEVICE		"SVR 4.2MP SCCPI to TPI Conversion Module (TPI) for SCCP"
#define TPI_SCCP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TPI_SCCP_LICENSE	"GPL"
#define TPI_SCCP_BANNER		TPI_SCCP_DESCRIP	"\n" \
				TPI_SCCP_EXTRA		"\n" \
				TPI_SCCP_COPYRIGHT	"\n" \
				TPI_SCCP_REVISION	"\n" \
				TPI_SCCP_DEVICE		"\n" \
				TPI_SCCP_CONTACT	"\n"
#define TPI_SCCP_SPLASH		TPI_SCCP_DESCRIP	" - " \
				TPI_SCCP_REVISION

#ifndef CONFIG_STREAMS_TPI_SCCP_NAME
#error CONFIG_STREAMS_TPI_SCCP_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_TPI_SCCP_MODID
#error CONFIG_STREAMS_TPI_SCCP_MODID must be defined.
#endif

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(TPI_SCCP_CONTACT);
MODULE_DESCRIPTION(TPI_SCCP_DESCRIP);
MODULE_SUPPORTED_DEVICE(TPI_SCCP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TPI_SCCP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tpi-sccp");
MODULE_ALIAS("streams-module-tpi-sccp");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TPI_SCCP_MODID));
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACAKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef TPI_SCCP_MOD_NAME
#define TPI_SCCP_MOD_NAME	CONFIG_STREAMS_TPI_SCCP_NAME
#endif				/* TPI_SCCP_MOD_NAME */
#ifndef TPI_SCCP_MOD_ID
#define TPI_SCCP_MOD_ID		CONFIG_STREAMS_TPI_SCCP_MODID
#endif				/* TPI_SCCP_MOD_ID */

/*
 * STREAMS DEFINITIONS
 */

#define MOD_ID		TPI_SCCP_MOD_ID
#define MOD_NAME	TPI_SCCP_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	TPI_SCCP_BANNER
#else				/* MODULE */
#define MOD_BANNER	TPI_SCCP_SPLASH
#endif				/* MODULE */

static struct module_info tp_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int tp_rput(queue_t *q, mblk_t *mp);
static streamscall int tp_rsrv(queue_t *q);

static streamscall int tp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp);
static streamscall int tp_qclose(queue_t *q, int oflags, cred_t *credp);

static struct qinit tp_rinit = {
	.qi_putp = tp_rput,
	.qi_srvp = tp_rsrv,
	.qi_qopen = tp_qopen,
	.qi_qclose = tp_qclose,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_rstat,
};

static streamscall int tp_wput(queue_t *q, mblk_t *mp);
static streamscall int tp_wsrv(queue_t *q);

static struct qinit tp_winit = {
	.qi_putp = tp_wput,
	.qi_srvp = tp_wsrv,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_wstat,
};

static struct streamtab tpi_sccpinfo = {
	.st_rdinit = &tp_rinit,
	.st_wrinit = &tp_winit,
};

/*
 * OPTIONS HANDLING
 */

struct t_sccp_options {
	uint32_t flags[4];
	struct {
		t_scalar_t debug[4];
		struct t_linger linger;
		t_uscalar_t rcvbuf;
		t_uscalar_t rcvlowat;
		t_uscalar_t sndbuf;
		t_uscalar_t sndlowat;
	} xti;
	struct {
		t_uscalar_t pvar;
		t_uscalar_t mplev;
		t_uscalar_t cluster;
		t_uscalar_t prio;
	} mtp;
	struct {
		t_uscalar_t pvar;
		t_uscalar_t mplev;
		t_uscalar_t cluster;
		t_uscalar_t prio;
		t_uscalar_t pclass;
		t_uscalar_t imp;
		t_uscalar_t reterr;
		t_uscalar_t credit;
		struct sockaddr_sccp discon_add;
		t_uscalar_t discon_reason;
		t_uscalar_t reset_reason;
	} sccp;
} t_defaults = {
	/* *INDENT-OFF* */
	{ 0, (struct t_linger) { T_NO, 0,}, 1 << 12, 1, 1 << 12, 1,},
	{ SS7_PVAR_ITUT_2000, 4, T_NO, 2,},
	{ SS7_PVAR_ITUT_2000, 4, T_NO, 2, T_SCCP_PCLASS_0, 0, T_NO, 1 << 12, (struct sockaddr_sccp) { AF_SCCP,}, 0, 0,},
	/* *INDENT-ON* */
};

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_MTP_PVAR,
	_T_BIT_MTP_MPLEV,
	_T_BIT_MTP_SLS,
	_T_BIT_MTP_MP,
	_T_BIT_SCCP_PVAR,
	_T_BIT_SCCP_MPLEV,
	_T_BIT_SCCP_CLUSTER,
	_T_BIT_SCCP_SEQCTRL,
	_T_BIT_SCCP_PRIO,
	_T_BIT_SCCP_PCLASS,
	_T_BIT_SCCP_IMP,
	_T_BIT_SCCP_RETERR,
	_T_BIT_SCCP_CREDIT,
	_T_BIT_SCCP_DISCON_ADD,
	_T_BIT_SCCP_DISCON_REASON,
	_T_BIT_SCCP_RESET_REASON,
	_T_BIT_SCCP_LAST_FLAG,
};

/*
 * PRIVATE STRUCTURE
 */

struct tp;
struct sc;

/* Upper interface structure */
struct tp {
	struct sc *sc;
	queue_t *oq;
	struct {
		t_uscalar_t state;
		t_uscalar_t datinds;
		t_uscalar_t datacks;
		t_uscalar_t coninds;
		t_uscalar_t inform;
		t_uscalar_t accept;
	} state, oldstate;
	ushort add_len;
	struct {
		struct T_info_ack info;
		struct sockaddr_sccp add;
		N_qos_co_opt_sel_t qos;
		N_qos_co_opt_range_t qor;
	} proto;
	t_uscalar_t OPT_length;
	t_sccp_options opts;
	t_uscalar_t ADDR_length;
	struct sockaddr_sccp add;
	t_uscalar_t LOCADDR_length;
	struct sockaddr_sccp loc;
	t_uscalar_t REMADDR_length;
	struct sockaddr_sccp rem;
	t_uscalar_t RES_length;
	struct sockaddr_sccp res;
};

/* Lower interface structure */
struct sc {
	struct tp *tp;
	queue_t *oq;
	struct {
		np_ulong state;
		np_ulong datinds;
		np_ulong datacks;
		np_ulong coninds;
		np_ulong inform;
		np_ulong accept;
	} state, oldstate;
	ushort add_len;
	struct {
		N_info_ack_t info;
		struct sockaddr_sccp add;
		N_qos_sccp_opt_sel_t qos;
		N_qos_sccp_opt_range_t qor;
	} proto;
	np_ulong QOS_length;
	N_qos_sccp_t qos;
	np_ulong QOS_range_length;
	N_qos_sccp_t qor;
	np_ulong ADDR_length;
	struct sockaddr_sccp add;
	np_ulong LOCADDR_length;
	struct sockaddr_sccp loc;
	np_ulong REMADDR_length;
	struct sockaddr_sccp rem;
	np_ulong RES_length;
	struct sockaddr_sccp res;
};

struct priv {
	struct tp tpi;
	struct sc sccpi;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define NP_PRIV(q) (&PRIV(q)->tpi)
#define SC_PRIV(q) (&PRIV(q)->sccpi)

static inline const char *
tp_primname(t_uscalar_t prim)
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
tp_statename(t_uscalar_t state)
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

static inline const char *
tp_strerror(const t_scalar_t error)
{
	if (error < 0)
		error = NSYSERR;
	switch (error) {
	case TBADADDR:
		return ("Bad address format.");
	case TBADOPT:
		return ("Bad options format.");
	case TACCES:
		return ("Bad permissions.");
	case TBADF:
		return ("Bad file descriptor.");
	case TNOADDR:
		return ("Unable to allocate an address.");
	case TOUTSTATE:
		return ("Would place interface out of state.");
	case TBADSEQ:
		return ("Bad call sequence number.");
	case TSYSERR:
		return ("System error.");
	case TLOOK:
		return ("Has to t_look() for event.");
	case TBADDATA:
		return ("Bad amount of data.");
	case TBUFOVFLW:
		return ("Buffer was too small.");
	case TFLOW:
		return ("Would block due to flow control.");
	case TNODATA:
		return ("No data indication.");
	case TNODIS:
		return ("No disconnect indication.");
	case TNOUDERR:
		return ("No unitdata error indication.");
	case TBADFLAG:
		return ("Bad flags.");
	case TNOREL:
		return ("No orderly release indication.");
	case TNOTSUPPORT:
		return ("Not supported.");
	case TSTATECHNG:
		return ("State is currently changing.");
	case TNOSTRUCTYPE:
		return ("Structure type not supported.");
	case TBADNAME:
		return ("Bad transport provider name.");
	case TBADQLEN:
		return ("Listener queue length limit is zero.");
	case TADDRBUSY:
		return ("Address already in use.");
	case TINDOUT:
		return ("Outstanding connect indications.");
	case TPROVMISMATCH:
		return ("Not same transport provider.");
	case TRESQLEN:
		return ("Connection acceptor has qlen > 0.");
	case TRESADDR:
		return ("Conn. acceptor bound to different address.");
	case TQFULL:
		return ("Connection indicator queue is full.");
	case TPROTO:
		return ("Protocol error.");
	default:
		return ("(unknown)");
	}
}

static t_uscalar_t
tp_get_state(struct tp *tp)
{
	return (tp->state.state);
}

static t_uscalar_t
tp_set_state(struct tp *tp, t_uscalar_t newstate)
{
	t_uscalar_t oldstate = tp->state.state;

	if (newstate != oldstate) {
		tp->state.state = newstate;
		tp->proto.info.CURRENT_state = newstate;
		mi_strlog(tp->oq, STRLOGST, SL_TRACE, "%s <- %s", tp_statename(newstate),
			  tp_statename(oldstate));
	}
	return (newstate);
}

static t_uscalar_t
tp_save_state(struct tp *tp)
{
	tp->oldstate = tp->state;
	return ((tp->oldstate = tp_get_state(tp)));
}

static t_uscalar_t
tp_restore_state(struct tp *tp)
{
	return (tp_set_state(tp, tp->oldstate));
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
	case N_REQUEST_ACK:
		return ("N_REQUEST_ACK");
	default:
		return ("(unknown)");
	}
}

static const char *
sc_statename(np_ulong state)
{
	return np_statename(state);
}

static const char *
sc_strerror(np_long error)
{
	return np_strerror(error);
}

static np_ulong
sc_get_state(struct sc *sc)
{
	return (sc->state);
}

static np_ulong
sc_set_state(struct sc *sc)
{
	int oldstate = sc->state;

	if (newstate != oldstate) {
		sc->state = newstate;
		sc->proto.info.CURRENT_state = newstate;
		mi_strlog(sc->oq, STRLOGST, SL_TRACE, "%s <- %s", sc_statename(newstate),
			  sc_statename(oldstate));
	}
	return (newstate);
}

static np_ulong
sc_save_state(struct sc *sc)
{
	return ((sc->oldstate = sc_get_state(sc)))
}

static np_ulong
sc_restore_state(struct sc *sc)
{
	return (sc_set_state(sc, sc->oldstate));
}

/*
 * TPI OPTION HANDLING
 */
static size_t
t_size_opts(N_qos_sccp_t * qos)
{
	size_t size = 0;

	if (qos) {
		switch (qos->n_qos_type) {
		case N_QOS_SEL_DATA_SCCP:
			if (qos->sel_data.protocol_class != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->sel_data.option_flags != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.reterr);
			}
			if (qos->sel_data.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->sel_data.message_priority != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.prio);
			}
			if (qos->sel_data.importance != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.imp);
			}
			break;
		case N_QOS_SEL_CONN_SCCP:
			if (qos->sel_conn.protocol_class != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->sel_conn.option_flags != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.reterr);
			}
			if (qos->sel_conn.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->sel_conn.message_priority != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.prio);
			}
			if (qos->sel_conn.importance != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.imp);
			}
			if (qos->sel_conn.credit != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.credit);
			}
			break;
		case N_QOS_SEL_INFO_SCCP:
			if (qos->sel_info.protocol_class != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->sel_info.option_flags != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.reterr);
			}
			if (qos->sel_info.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->sel_info.message_priority != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.prio);
			}
			if (qos->sel_info.importance != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.imp);
			}
			if (qos->sel_info.credit != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.credit);
			}
			break;
		case N_QOS_RANGE_INFO_SCCP:
			if (qos->range_info.protocol_classes != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->range_info.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->range_info.credit != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.credit);
			}
			break;
		case N_QOS_SEL_INFR_SCCP:
			if (qos->sel_infr.protocol_class != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.pclass);
			}
			if (qos->sel_infr.option_flags != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.reterr);
			}
			if (qos->sel_infr.sequence_selection != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.seqctrl);
			}
			if (qos->sel_infr.message_priority != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.prio);
			}
			if (qos->sel_infr.importance != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.imp);
			}
			if (qos->sel_infr.credit != QOS_UNKNOWN) {
				size += _T_SPACE_SIZEOF(t_defaults.sccp.credit);
			}
			break;
		default:
			break;
		}
	}
	return (size);
}

static size_t
t_build_opts(N_qos_sccp_t * qos, unsigned char *op, size_t olen)
{
	struct t_opthdr *oh;

	if (op == NULL || olen == 0)
		return (0);
	if (qos) {
		oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
		switch (qos->n_qos_type) {
		case N_QOS_SEL_DATA_SCCP:
			if (qos->sel_data.protocol_class != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->sel_data.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_data.option_flags != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.reterr);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_RETERR;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.reterr) *) T_OPT_DATA(oh) =
				    qos->sel_data.option_flags;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_data.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->sel_data.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_data.message_priority != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.prio);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PRIO;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.prio) *) T_OPT_DATA(oh) =
				    qos->sel_data.message_priority;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_data.importance != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.imp);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_IMP;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.imp) *) T_OPT_DATA(oh) =
				    qos->sel_data.importance;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			break;
		case N_QOS_SEL_CONN_SCCP:
			if (qos->sel_conn.protocol_class != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->sel_conn.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.option_flags != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.reterr);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_RETERR;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.reterr) *) T_OPT_DATA(oh) =
				    qos->sel_conn.option_flags;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->sel_conn.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.message_priority != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.prio);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PRIO;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.prio) *) T_OPT_DATA(oh) =
				    qos->sel_conn.message_priority;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.importance != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.imp);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_IMP;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.imp) *) T_OPT_DATA(oh) =
				    qos->sel_conn.importance;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_conn.credit != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.credit);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_CREDIT;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.credit) *) T_OPT_DATA(oh) =
				    qos->sel_conn.credit;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			break;
		case N_QOS_SEL_INFO_SCCP:
			if (qos->sel_info.protocol_class != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->sel_info.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.option_flags != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.reterr);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_RETERR;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.reterr) *) T_OPT_DATA(oh) =
				    qos->sel_info.option_flags;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->sel_info.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.message_priority != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.prio);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PRIO;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.prio) *) T_OPT_DATA(oh) =
				    qos->sel_info.message_priority;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.importance != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.imp);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_IMP;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.imp) *) T_OPT_DATA(oh) =
				    qos->sel_info.importance;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_info.credit != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.credit);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_CREDIT;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.credit) *) T_OPT_DATA(oh) =
				    qos->sel_info.credit;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			break;
		case N_QOS_RANGE_INFO_SCCP:
			if (qos->range_info.protocol_classes != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->range_info.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->range_info.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->range_info.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->range_info.credit != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.credit);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_CREDIT;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.credit) *) T_OPT_DATA(oh) =
				    qos->range_info.credit;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			break;
		case N_QOS_SEL_INFR_SCCP:
			if (qos->sel_infr.protocol_class != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.pclass);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PCLASS;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.pclass) *) T_OPT_DATA(oh) =
				    qos->sel_infr.protocol_class;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.option_flags != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.reterr);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_RETERR;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.reterr) *) T_OPT_DATA(oh) =
				    qos->sel_infr.option_flags;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.sequence_selection != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.seqctrl);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_SEQCTRL;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.seqctrl) *) T_OPT_DATA(oh) =
				    qos->sel_infr.sequence_selection;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.message_priority != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.prio);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_PRIO;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.prio) *) T_OPT_DATA(oh) =
				    qos->sel_infr.message_priority;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.importance != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.imp);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_IMP;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.imp) *) T_OPT_DATA(oh) =
				    qos->sel_infr.importance;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			if (qos->sel_infr.credit != QOS_UNKNOWN) {
				if (oh == NULL)
					goto efault;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.opts.sccp.credit);
				oh->level = T_SS7_SCCP;
				oh->name = T_SCCP_CREDIT;
				oh->status = T_SUCCESS;
				*(typeof(t_defaults.opts.sccp.credit) *) T_OPT_DATA(oh) =
				    qos->sel_infr.credit;
				oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
			}
			break;
		default:
			break;
		}
	}
	if (oh == NULL)
		return (olen);
	return ((unsigned char *) oh - op);
}

static int
t_read_opts(struct tp *tp, N_qos_sccp_t * qos, unsigned char *ip, size_t ilen)
{
	/* This is only for connect request, response and data requests.  Any value of any option
	   is appropriate and is stored in the transport provider private structure.  The, once all
	   options have been processed, any options that were set are added to the structure if
	   provided. */
	bzero(tp->opts.flags, sizeof(tp->opts.flags));
	if (ip == NULL || ilen == 0)
		return (0);
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		switch (ih->level) {
		case XTI_GENERIC:
			switch (ih->name) {
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (((ih->len - sizeof(*ih)) % sizeof(t_uscalar_t)) != 0)
					goto einval;
				if (ih->len >= sizeof(*ih) + 4 * sizeof(t_uscalar_t))
					tp->opts.xti.debug[3] = valp[3];
				else
					tp->opts.xti.debug[3] = 0;
				if (ih->len >= sizeof(*ih) + 3 * sizeof(t_uscalar_t))
					tp->opts.xti.debug[2] = valp[2];
				else
					tp->opts.xti.debug[2] = 0;
				if (ih->len >= sizeof(*ih) + 2 * sizeof(t_uscalar_t))
					tp->opts.xti.debug[1] = valp[1];
				else
					tp->opts.xti.debug[1] = 0;
				if (ih->len >= sizeof(*ih) + 1 * sizeof(t_uscalar_t))
					tp->opts.xti.debug[0] = valp[0];
				else
					tp->opts.xti.debug[0] = 0;
				t_set_bit(_T_BIT_XTI_DEBUG, tp->opts.flags);
				continue;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					goto einval;
				if (valp->l_linger != T_INFINITE && valp->l_linger != T_UNSPEC
				    && valp->l_linger < 0)
					goto einval;
				tp->opts.xti.linger = *valp;
				t_set_bit(_T_BIT_XTI_LINGER, tp->opts.flags);
				continue;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.xti.rcvbuf = *valp;
				t_set_bit(_T_BIT_XTI_RCVBUF, tp->opts.flags);
				continue;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.xti.rcvlowat = *valp;
				t_set_bit(_T_BIT_XTI_RCVLOWAT, tp->opts.flags);
				continue;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.xti.sndbuf = *valp;
				t_set_bit(_T_BIT_XTI_SNDBUF, tp->opts.flags);
				continue;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.xti.sndlowat = *valp;
				t_set_bit(_T_BIT_XTI_SNDLOWAT, tp->opts.flags);
				continue;
			}
			}
			continue;
		case T_SS7_MTP:
			switch (ih->name) {
			case T_MTP_PVAR:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.mtp.pvar = *valp;
				t_set_bit(_T_BIT_MTP_PVAR, tp->opts.flags);
				continue;
			}
			case T_MTP_MPLEV:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.mtp.mplev = *valp;
				t_set_bit(_T_BIT_MTP_MPLEV, tp->opts.flags);
				continue;
			}
			case T_MTP_CLUSTER:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.mtp.cluster = *valp;
				t_set_bit(_T_BIT_MTP_CLUSTER, tp->opts.flags);
				continue;
			}
			case T_MTP_PRIO:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.mtp.prio = *valp;
				t_set_bit(_T_BIT_MTP_PRIO, tp->opts.flags);
				continue;
			}
			}
			continue;
		case T_SS7_SCCP:
			switch (ih->name) {
			case T_SCCP_PVAR:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.pvar = *valp;
				t_set_bit(_T_BIT_SCCP_PVAR, tp->opts.flags);
				continue;
			}
			case T_SCCP_MPLEV:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.mplev = *valp;
				t_set_bit(_T_BIT_SCCP_MPLEV, tp->opts.flags);
				continue;
			}
			case T_SCCP_CLUSTER:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.cluster = *valp;
				t_set_bit(_T_BIT_SCCP_CLUSTER, tp->opts.flags);
				continue;
			}
			case T_SCCP_PRIO:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.prio = *valp;
				t_set_bit(_T_BIT_SCCP_PRIO, tp->opts.flags);
				continue;
			}
			case T_SCCP_PCLASS:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.pclass = *valp;
				t_set_bit(_T_BIT_SCCP_PCLASS, tp->opts.flags);
				continue;
			}
			case T_SCCP_IMP:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.imp = *valp;
				t_set_bit(_T_BIT_SCCP_IMP, tp->opts.flags);
				continue;
			}
			case T_SCCP_RETERR:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.reterr = *valp;
				t_set_bit(_T_BIT_SCCP_RETERR, tp->opts.flags);
				continue;
			}
			case T_SCCP_CREDIT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.credit = *valp;
				t_set_bit(_T_BIT_SCCP_CREDIT, tp->opts.flags);
				continue;
			}
			case T_SCCP_DISCON_ADD:
			{
				struct sockaddr_sccp *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.discon_add = *valp;
				t_set_bit(_T_BIT_SCCP_DISCON_ADD, tp->opts.flags);
				continue;
			}
			case T_SCCP_DISCON_REASON:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.discon_reason = *valp;
				t_set_bit(_T_BIT_SCCP_DISCON_REASON, tp->opts.flags);
				continue;
			}
			case T_SCCP_RESET_REASON:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->opts.sccp.reset_reason = *valp;
				t_set_bit(_T_BIT_SCCP_RESET_REASON, tp->opts.flags);
				continue;
			}
			}
			continue;
		}

	}

	if (qos) {
		switch (qos->n_qos_type) {
		case N_QOS_SEL_DATA_SCCP:
			if (t_tst_bit(_T_BIT_SCCP_PCLASS, tp->opts.flags))
				qos->sel_data.protocol_class = tp->opts.sccp.pclass;
			else
				qos->sel_data.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_RETERR, tp->opts.flags))
				qos->sel_data.protocol_class = tp->opts.sccp.reterr;
			else
				qos->sel_data.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_SEQCTRL, tp->opts.flags))
				qos->sel_data.protocol_class = tp->opts.sccp.seqctrl;
			else
				qos->sel_data.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_PRIO, tp->opts.flags))
				qos->sel_data.protocol_class = tp->opts.sccp.prio;
			else
				qos->sel_data.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_IMP, tp->opts.flags))
				qos->sel_data.protocol_class = tp->opts.sccp.imp;
			else
				qos->sel_data.protocol_class = QOS_UNKNOWN;
			break;
		case N_QOS_SEL_CONN_SCCP:
			if (t_tst_bit(_T_BIT_SCCP_PCLASS, tp->opts.flags))
				qos->sel_conn.protocol_class = tp->opts.sccp.pclass;
			else
				qos->sel_conn.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_RETERR, tp->opts.flags))
				qos->sel_conn.protocol_class = tp->opts.sccp.reterr;
			else
				qos->sel_conn.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_SEQCTRL, tp->opts.flags))
				qos->sel_conn.protocol_class = tp->opts.sccp.seqctrl;
			else
				qos->sel_conn.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_PRIO, tp->opts.flags))
				qos->sel_conn.protocol_class = tp->opts.sccp.prio;
			else
				qos->sel_conn.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_IMP, tp->opts.flags))
				qos->sel_conn.protocol_class = tp->opts.sccp.imp;
			else
				qos->sel_conn.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_CREDIT, tp->opts.flags))
				qos->sel_conn.protocol_class = tp->opts.sccp.credit;
			else
				qos->sel_conn.protocol_class = QOS_UNKNOWN;
			break;
		case N_QOS_SEL_INFO_SCCP:
			if (t_tst_bit(_T_BIT_SCCP_PCLASS, tp->opts.flags))
				qos->sel_info.protocol_class = tp->opts.sccp.pclass;
			else
				qos->sel_info.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_RETERR, tp->opts.flags))
				qos->sel_info.protocol_class = tp->opts.sccp.reterr;
			else
				qos->sel_info.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_SEQCTRL, tp->opts.flags))
				qos->sel_info.protocol_class = tp->opts.sccp.seqctrl;
			else
				qos->sel_info.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_PRIO, tp->opts.flags))
				qos->sel_info.protocol_class = tp->opts.sccp.prio;
			else
				qos->sel_info.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_IMP, tp->opts.flags))
				qos->sel_info.protocol_class = tp->opts.sccp.imp;
			else
				qos->sel_info.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_CREDIT, tp->opts.flags))
				qos->sel_info.protocol_class = tp->opts.sccp.credit;
			else
				qos->sel_info.protocol_class = QOS_UNKNOWN;
			break;
		case N_QOS_RANGE_INFO_SCCP:
			if (t_tst_bit(_T_BIT_SCCP_PCLASS, tp->opts.flags))
				qos->range_info.protocol_class = tp->opts.sccp.pclass;
			else
				qos->range_info.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_SEQCTRL, tp->opts.flags))
				qos->range_info.protocol_class = tp->opts.sccp.seqctrl;
			else
				qos->range_info.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_CREDIT, tp->opts.flags))
				qos->range_info.protocol_class = tp->opts.sccp.credit;
			else
				qos->range_info.protocol_class = QOS_UNKNOWN;
			break;
		case N_QOS_SEL_INFR_SCCP:
			if (t_tst_bit(_T_BIT_SCCP_PCLASS, tp->opts.flags))
				qos->sel_infr.protocol_class = tp->opts.sccp.pclass;
			else
				qos->sel_infr.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_RETERR, tp->opts.flags))
				qos->sel_infr.protocol_class = tp->opts.sccp.reterr;
			else
				qos->sel_infr.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_SEQCTRL, tp->opts.flags))
				qos->sel_infr.protocol_class = tp->opts.sccp.seqctrl;
			else
				qos->sel_infr.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_PRIO, tp->opts.flags))
				qos->sel_infr.protocol_class = tp->opts.sccp.prio;
			else
				qos->sel_infr.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_IMP, tp->opts.flags))
				qos->sel_infr.protocol_class = tp->opts.sccp.imp;
			else
				qos->sel_infr.protocol_class = QOS_UNKNOWN;
			if (t_tst_bit(_T_BIT_SCCP_CREDIT, tp->opts.flags))
				qos->sel_infr.protocol_class = tp->opts.sccp.credit;
			else
				qos->sel_infr.protocol_class = QOS_UNKNOWN;
			break;
		default:
			break;
		}
	}
	return (0);
}

/*
 * TPI PROVIDER TO TPI USER ISSUED PRIMITIVES
 */

/**
 * t_conn_ind: - issue a T_CONN_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @loc: destination address
 * @rem: source address
 * @qos: quality of service parameters
 * @seq: connection indication sequence number
 * @dp: user data
 */
noinline fastcall int
t_conn_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct sockaddr_sccp *loc,
	   struct sockaddr_sccp *rem, N_qos_sel_conn_sccp_t *qos, t_uscalar_t seq, const mblk_t *dp)
{
	struct T_conn_ind *p;
	mblk_t *mp;
	const size_t olen = t_size_opts((N_qos_sccp_t *) qos);
	const size_t mlen = sizeof(*p) + sizeof(tp->rem) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 0)))
		goto ebusy;

	/* save source address for later */
	if (rem) {
		tp->rem = *rem;
		tp->REMADDR_length = sizeof(*rem);
	}
	/* save destination address for later */
	if (loc) {
		tp->loc = *loc;
		tp->LOCADDR_length = sizeof(*loc);
	}

	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = tp->REMADDR_length;
	p->SRC_offset = tp->REMADDR_length ? sizeof(*p) : 0;
	p->OPT_length = olen;
	p->OPT_offset = p->SRC_offset + p->SRC_length;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	bcopy(&tp->rem, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	olen = t_build_opts((N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_CONN_IND");
	tp_set_state(tp, TS_WRES_CIND);
	tp->state.coninds++;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_conn_con: - issue a T_CONN_CON primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: responding address
 * @qos: quality of service parameters
 * @dp: user data
 */
noinline fastcall int
t_conn_con(struct tp *tp, queue_t *q, mblk_t *msg, struct sockaddr_sccp *res,
	   N_qos_sel_conn_sccp_t *qos, const mblk_t *dp)
{
	struct T_conn_con *p;
	mblk_t *mp;
	const size_t olen = t_size_opts((N_qos_sccp_t *) qos);
	const size_t mlen = sizeof(*p) + sizeof(tp->res) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	/* save responding address for later */
	if (res) {
		tp->res = *res;
		tp->RES_length = sizeof(*res);
	}

	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_CONN_CON;
	p->RES_length = tp->RES_length;
	p->RES_offset = tp->RES_length ? sizeof(*p) : 0;
	p->OPT_length = olen;
	p->OPT_offset = olen ? p->RES_offset + p->RES_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&tp->res, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	olen = t_build_opts((N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_CONN_CON");
	tp_set_state(tp, TS_DATA_XFER);
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_discon_ind: - issue a T_DISCON_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: responding address
 * @orig: disconnect originator
 * @reason: disconnect reason
 * @seq: connection indication sequence number (or zero)
 * @dp: user data
 */
noinline fastcall int
t_discon_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct sockaddr_sccp *res, t_scalar_t reason,
	     t_scalar_t seq, const mblk_t *dp)
{
	struct T_discon_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	/* save responding address for later */
	if (res) {
		tp->res = *res;
		tp->RES_length = sizeof(*res);
	}

	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_DISCON_IND");
	if (seq)
		tp->state.coninds--;
	tp_set_state(tp, tp->state.coninds ? TS_WRES_CIND : TS_IDLE);
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_data_ind: - issue a T_DATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flag: more flag
 * @dp: user data
 */
noinline fastcall int
t_data_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flag, const mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = flag;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_DATA_IND");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_exdata_ind: - issue a T_EXDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flag: more flag
 * @dp: user data
 */
noinline fastcall int
t_exdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flag, const mblk_t *dp)
{
	struct T_exdata_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 1)))
		goto ebusy;
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = flag;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_EXDATA_IND");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_info_ack: - issue a T_INFO_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
t_info_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	*p = tp->proto.info;
	mp->b_wptr += sizeof(*p);
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_INFO_ACK");
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_bind_ack: - issue a T_BIND_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
t_bind_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_bind_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(tp->add);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = sizeof(tp->add);
	p->ADDR_offset = sizeof(*p);
	p->CONIND_number = tp->maxinds;
	mp->b_wptr += sizeof(*p);
	bcopy(&tp->add, mp->b_wtpr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_BIND_ACK");
	tp_set_state(tp, TS_IDLE);
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_error_ack: - issue a T_ERROR_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative UNIX error, positive TLI error
 */
noinline fastcall int
t_error_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t prim, t_scalar_t error)
{
	struct T_error_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	t_scalar_t state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error < 0 ? TSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_ERROR_ACK");
	tp_restore_state(tp);
	switch ((state = tp_get_state(tp))) {
	case TS_WACK_BREQ:
		state = TS_UNBND;
		break;
	case TS_WACK_UREQ:
		state = TS_IDLE;
		break;
	case TS_WACK_OPTREQ:
		state = TS_IDLE;
		break;
	case TS_WACK_CREQ:
		state = TS_IDLE;
		break;
	case TS_WACK_CRES:
		state = TS_WRES_CIND;
		break;
	case TS_WACK_DREQ6:
		state = TS_WCON_CREQ;
		break;
	case TS_WACK_DREQ7:
		state = TS_WRES_CIND;
		break;
	case TS_WACK_DREQ9:
		state = TS_DATA_XFER;
		break;
	case TS_WACK_DREQ10:
		state = TS_WREQ_ORDREL;
		break;
	case TS_WACK_DREQ11:
		state = TS_WIND_ORDREL;
		break;
	}
	tp_set_state(tp, state);
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_ok_ack: - issue a T_OK_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct prim
 */
noinline fastcall int
t_ok_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t prim)
{
	struct T_ok_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	t_uscalar_t state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch ((state = tp_get_state(tp))) {
	case TS_WACK_BREQ:
	case TS_WACK_OPTREQ:
	case TS_WACK_CREQ:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		state = TS_IDLE;
		break;
	case TS_WACK_UREQ:
		state = TS_UNBND;
		break;
	case TS_WACK_CRES:
		tp->state.coninds--;
		/* FIXME: when accept is non-zero, need to find the other Stream and set its state
		   to TS_DATA_XFER. */
		if (tp->state.accept == 0)
			state = TS_DATA_XFER;
		else if (tp->state.coninds > 0)
			state = TS_WRES_CIND;
		else
			state = TS_IDLE;
		break;
	}
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_OK_ACK");
	tp_set_state(tp, state);
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_unitdata_ind: - issue a T_UNITDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @rem: source address
 * @loc: destination address
 * @qos: quality of service parameters
 * @dp: user data
 */
noinline fastcall int
t_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct sockaddr_sccp *rem,
	       struct sockaddr_sccp *loc, N_qos_sel_data_sccp_t *qos, const mblk_t *dp)
{
	struct T_unitdata_ind *p;
	mblk_t *mp;
	const size_t olen = t_size_opts((N_qos_sccp_t *) qos);
	const size_t mlen = sizeof(*p) + sizeof(*loc) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = loc ? sizeof(*loc) : 0;
	p->SRC_offset = loc ? sizeof(*p) : 0;
	p->OPT_length = olen;
	p->OPT_offset = olen ? p->SRC_offset + p->SRC_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(loc, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	olen = t_build_opts((N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_UNITDATA_IND");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_uderror_ind: - issue a T_UDERROR_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @rem: destination address (or NULL)
 * @qos: quality of service parameters
 * @error: error type
 * @dp: user data
 */
noinline fastcall int
t_uderror_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct sockaddr_sccp *rem,
	      N_qos_sel_data_sccp_t *qos, t_scalar_t error, const mblk_t *dp)
{
	struct T_uderror_ind *p;
	mblk_t *mp;
	const size_t olen = t_size_opts((N_qos_sccp_t *) qos);
	const size_t mlen = sizeof(*p) + sizeof(*rem) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 1)))
		goto ebusy;
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = rem ? sizeof(*rem) : 0;
	p->DEST_offset = rem ? sizeof(*p) : 0;
	p->OPT_length = olen;
	p->OPT_offset = olen ? p->DEST_offset + p->DEST_length : 0;
	p->ERROR_type = error;
	mp->b_wptr += sizeof(*p);
	bcopy(rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	olen = t_build_opts((N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_UDERROR_IND");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_datack_ind: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
t_datack_ind(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_optmgmt_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext()))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flags = T_SCCP_ACK;
	mp->b_wptr += sizeof(*p);
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_OPTDATA_IND(T_SCCP_ACK)");
	tp->state.datacks--;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_optmgmt_ack: - issue a T_OPTMGMT_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @qos: quality of service parameters
 * @flags: management flags
 */
noinline fastcall int
t_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *msg, N_qos_sccp_t * qos, t_scalar_t flags)
{
	struct T_optmgmt_ack *p;
	mblk_t *mp;
	const size_t olen = t_size_opts(qos);
	const size_t mlen = sizeof(*p) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = olen;
	p->OPT_offset = olen ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	mp->b_wptr += sizeof(*p);
	olen = t_build_opts(qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_OPTMGMT_ACK");
	if (tp_get_state(tp, TS_WACK_OPTREQ))
		tp_set_state(tp, TS_IDLE);
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_ordrel_ind: - issue a T_ORDREL_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: user data
 */
noinline fastcall int
t_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *msg, const mblk_t *dp)
{
	struct T_ordrel_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_ORDREL_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_ORDREL_IND");
	switch (tp_get_state(tp)) {
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		tp_set_state(tp, TS_IDLE);
		break;
	}
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_optdata_ind: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @qos: quality of service parameters
 * @flag: data flags
 * @dp: user data
 */
noinline fastcall int
t_optdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, N_qos_sccp_t * qos, t_scalar_t flag,
	      const mblk_t *dp)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	const size_t olen = t_size_opts(qos);
	const size_t mlen = sizeof(*p) + olen;
	unsigned char band = (flag & T_ODF_EX) ? 1 : 0;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, band)))
		goto ebusy;
	mp->b_band = band;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = flag;
	p->OPT_length = olen;
	p->OPT_offset = olen ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	olen = t_build_opts(qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_OPTDATA_IND");
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_addr_ack: - issue a T_ADDR_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
t_addr_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_addr_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(tp->add) + sizeof(tp->res);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = tp->proto.addr.LOCADDR_length;
	p->LOCADDR_offset = p->LOCADDR_length ? sizeof(*p) : 0;
	p->REMADDR_length = tp->proto.addr.REMADDR_length;
	p->REMADDR_offset = p->REMADDR_length ? p->LOCADDR_offset + p->LOCADDR_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&tp->add, mp->b_wptr, p->LOCADDR_length);
	mp->b_wptr += p->LOCADDR_length;
	bcopy(&tp->res, mp->b_wptr, p->REMADDR_length);
	mp->b_wptr += p->REMADDR_length;
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_ADDR_ACK");
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_capability_ack: - issue a T_CAPABILITY_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @type: message type of request
 * @flags: capability bits
 */
noinline fastcall int
t_capability_ack(struct tp *tp, queue_t *q, mblk_t *msg, uchar type, t_uscalar_t flags)
{
	struct T_capability_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!pcmsg(type) && !bcanputnext(tp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = type;
	p->PRIM_type = T_CAPABILITY_ACK;
	p->CAP_bits1 = flags;
	p->INFO_ack = tp->proto.info;
	p->ACCEPTOR_id = tp->token;
	mp->b_wptr += sizeof(*p);
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_CAPABILITY_ACK");
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_inform_ind: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @reason: inform reason
 */
noinline fastcall int
t_inform_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t orig, t_uscalar_t reason)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	struct t_opthdr *oh;
	const size_t olen = 4 * (sizeof(*oh) + sizeof(t_scalar_t));
	const size_t mlen = sizeof(*p) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 2)))
		goto ebusy;
	mp->b_band = 2;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = T_SCCP_INF;
	p->OPT_length = olen;
	p->OPT_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_INFORM_ORIG;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = orig;
	mp->b_wptr += oh->len;
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_INFORM_REASON;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = reason;
	mp->b_wptr += oh->len;
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_PCLASS;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = tp->opts.sccp.pclass;
	mp->b_wptr += oh->len;
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_CREDIT;
	oh->len = sizeof(*oh)+ sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = tp->opts.sccp.credit;
	mp->b_wptr += oh->len;
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- T_OPTDATA_IND(T_SCCP_INF)");
	tp_set_state(tp, tp_get_state(tp));
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_reset_ind: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @orig: reset originator
 * @reason: reset reason
 */
noinline fastcall int
t_reset_ind(struct tp *tp, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	const size_t olen = 2 * (sizeof(struct t_opthdr) + sizeof(t_scalar_t));
	const size_t mlen = sizeof(*p) + olen;
	struct t_opthdr *oh;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = T_SCCP_RST;
	p->OPT_length = olen;
	p->OPT_offset = olen ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_RESET_ORIG;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = orig;
	mp->b_wptr += sizeof(*oh) + sizeof(t_scalar_t);
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_RESET_REASON;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = reason;
	mp->b_wptr += sizeof(*oh) + sizeof(t_scalar_t);
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_OPTDATA_IND(T_SCCP_RST)");
	tp_set_state(tp, NS_DATA_XFER);
	tp->state.resinds = 1;
	tp->state.datinds = 0;
	tp->state.datacks = 0;
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_reset_con: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
t_reset_con(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = T_SCCP_RST;
	p->OPT_length = 0;
	p->OPT_offset = 0;
	mp->b_wptr += sizeof(*p);
	mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_OPTDATA_IND(T_SCCP_RST)");
	tp_set_state(tp, NS_DATA_XFER);
	tp->state.resacks = 0;
	tp->state.resinds = 0;
	tp->state.datinds = 0;
	tp->state.datacks = 0;
	freemsg(msg);
	putnext(tp->oq, mp);
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
n_conn_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sockaddr_sccp *rem,
	   N_qos_sel_conn_sccp_t *qos, np_ulong flags, const mblk_t *dp)
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
		sc->qos = *qos;
		sc->QOS_length = sizeof(*qos);
	}

	DB_TYPE(mp) = M_PROTO;
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
n_conn_res(struct sc *sc, queue_t *q, mblk_t *msg, struct sockaddr_sccp *res,
	   N_qos_sel_conn_sccp_t *qos, np_ulong token, np_ulong flags, np_ulong seq,
	   const mblk_t *dp)
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
		sc->qos = *qos;
		sc->QOS_length = sizeof(*qos);
	}

	DB_TYPE(mp) = M_PROTO;
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
n_discon_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sockaddr_sccp *res, np_ulong reason,
	     np_ulong seq, const mblk_t *dp)
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
n_data_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong flags, const mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
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
n_exdata_req(struct sc *sc, queue_t *q, mblk_t *msg, const mblk_t *dp)
{
	N_exdata_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
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
n_unitdata_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sockaddr_sccp *rem,
	       N_qos_sel_data_sccp_t *qos, const mblk_t *dp)
{
	N_unitdata_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*rem) + sizeof(*qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p->PRIM_type = N_UNITDATA_REQ;
	p->DEST_length = sizeof(*rem);
	p->DEST_offset = sizeof(*p);
	p->RESERVED_field[0] = sizeof(*qos);
	p->RESERVED_field[1] = p->DEST_offset + p->DEST_length;
	mp->b_wptr += sizeof(*p);
	bcopy(rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	bcopy(qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
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
	p->PRIM_type = N_OPTMGMT_REQ;
	p->QOS_length = qlen;
	p->QOS_offset = qlen ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(qptr, mp->b_wptr, p->QOS_length);
	mp->b_wptr + p->QOS_length;
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
n_request_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sockaddr_sccp *loc, nl_ulong sref)
{
	N_request_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*loc) + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
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
	p->PRIM_type = N_INFORM_REQ;
	p->QOS_length = sizeof(sc->qos);
	p->QOS_offset = sizeof(*p);
	p->REASON = reason;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->qos, mp->b_qptr, p->QOS_length);
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
 * TPI User to TPI Provider Primitives
 */

/**
 * tp_conn_req: - process T_CONN_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_CONN_REQ primitive
 */
noinline fastcall int
tp_conn_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_sccp *rem;
	N_qos_sel_conn_sccp_t qos = { N_QOS_SEL_CONN_SCCP, };
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (p->DEST_length == 0)
		goto noaddr;
	if (p->DEST_length < sizeof(tp->rem))
		goto badaddr;
	bcopy(mp->b_wptr + p->DEST_offset, &tp->rem, sizeof(tp->rem));
	rem = &tp->rem;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if ((err = t_read_opts(tp, (N_qos_sccp_t *) qos,
			       mp->b_rptr + p->OPT_offset, p->OPT_length)))
		goto error;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.CDATA_size)
		goto baddata;
	tp_set_state(tp, TS_WACK_CREQ);
	return n_conn_req(tp->sc, q, mp, rem, qos, 0 /* FIXME */ , mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      noaddr:
	err = TNOADDR;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * tp_conn_res: - process T_CONN_RES primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_CONN_RES primitive
 */
noinline fastcall int
tp_conn_res(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_conn_sccp_t qos = { N_QOS_SEL_CONN_SCCP, };
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (tp_get_state(tp) != TS_WRES_CIND)
		goto outstate;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if ((err = t_read_opts(tp, (N_qos_sccp_t *) qos,
			       mp->b_rptr + p->OPT_offset, p->OPT_length)))
		goto error;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.CDATA_size)
		goto baddata;
	tp->state.accept = p->ACCEPTOR_id;
	tp->state.pending = T_CONN_RES;
	tp_set_state(tp, TS_WACK_CRES);
	return n_conn_res(tp->sc, q, mp, NULL, qos, p->ACCEPTOR_id, 0 /* FIXME */ , p->SEQ_number,
			  mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * tp_discon_req: - process T_DISCON_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_DISCON_REQ primitive
 *
 * Note that T_DISCON_REQ has no direct way of specifying the responding address.  The responding
 * address might be set using a TPI option, say, T_SCCP_DISCON_ADD, before issuing the T_DISCON_REQ
 * primitive.  Also, T_DISCON_REQ has no direct way of specifying the disconnect reason.  The
 * disconnect reason might be set using a TPI option, say, T_SCCP_DISCON_REASON, before issuing the
 * T_DISCON_REQ primitive.
 */
noinline fastcall int
tp_discon_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
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
	default:
		goto outstate;
	}
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.DDATA_size)
		goto baddata;
	return n_discon_req(tp->sc, q, mp, NULL, tp->DISCON_reason, p->SEQ_number, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * tp_data_req: - process T_DATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_DATA_REQ primitive
 */
noinline fastcall int
tp_data_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_IDLE:
		goto discard;
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	default:
		goto outstate;
	}
	if (tp->edatack)
		/* The TS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (p->MORE_flag & ~(0x01))
		goto badflag;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.TIDU_size)
		goto baddata;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.TSDU_size)
		goto baddata;
	return n_data_req(tp->sc, q, mp, p->MORE_flag ? N_MORE_DATA_FLAG : 0, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_exdata_req: - process T_EXDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_EXDATA_REQ primitive
 */
noinline fastcall int
tp_exdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_IDLE:
		goto discard;
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	default:
		goto outstate;
	}
	if (tp->edatack)
		/* The TS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (p->MORE_flag & ~(0x01))
		goto badflag;
	if (p->MORE_flag & 0x01)
		/* Note that the NPI cannot handle segmentation of expedited data and it is not
		   permitted to use the "more" flag with expedited data. */
		goto notsupport;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.TIDU_size)
		goto baddata;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.ETSDU_size)
		goto baddata;
	tp->edatack = 1;
	return n_exdata_req(tp->sc, q, mp, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_info_req: - process T_INFO_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_INFO_REQ primitive
 */
noinline fastcall int
tp_info_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	return n_info_req(tp->sc, q, mp);
}

/**
 * tp_bind_req: - process T_BIND_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_BIND_REQ primitive
 */
noinline fastcall int
tp_bind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (tp_get_state(tp) != TS_UNBND)
		goto outstate;
	if (!MBLKIN(mp, p->ADDR_offset, p->ADDR_length))
		goto badaddr;
	if (p->ADDR_length) {
		if (p->ADDR_length < sizeof(tp->add))
			goto badaddr;
		bcopy(mp->b_rptr + p->ADDR_offset, &tp->add, sizeof(tp->add));
	}
	tp_set_state(tp, TS_WACK_BREQ);
	return n_bind_req(np->sc, q, mp, p->CONIND_number, 0 /* FIXME */ );
      badaddr:
	err = TBADADDR;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * tp_unbind_req: - process T_UNBIND_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_UNBIND_REQ primitive
 */
noinline fastcall int
tp_unbind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	tp_set_state(tp, TS_WACK_UREQ);
	return n_unbind_req(tp->sc, q, mp);
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * tp_unitdata_req: - process T_UNITDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_UNITDATA_REQ primitive
 */
noinline fastcall int
tp_unitdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_sccp *rptr = NULL;
	N_qos_sel_data_sccp_t *qptr = NULL;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case NS_IDLE:
		if (p->DEST_length == 0)
			goto noaddr;
		rptr = &tp->rem;
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
		qptr = &tp->qos;
		if (p->QOS_length < sizeof(*qptr))
			goto badqos;
		bcopy(mp->b_rptr + p->QOS_offset, qptr, sizeof(*qptr));
	}
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.TIDU_size)
		goto baddata;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.TSDU_size)
		goto baddata;
	return n_unitdata_req(tp->sc, q, mp, rptr, qptr, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      badqos:
	err = TBADOPT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      noaddr:
	err = TNOADDR;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * tp_optmgmt_req: - process T_OPTMGMT_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTMGMT_REQ primitive
 */
noinline fastcall int
tp_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (tp_get_state(tp) == NS_IDLE)
		tp_set_state(tp, TS_WACK_OPTREQ);
	goto notsupport;
	/* FIXME: If we are reading current or default options, we need to issue an N_INFO_REQ and
	   mark the response as for T_OPTMGMT_ACK.  If we are negiotiating options, then we need to 
	   issue an N_OPTMGMT_REQ and mark the response as for T_OPTMGMT_ACK. */
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * tp_ordrel_req: - process T_ORDREL_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_ORDREL_REQ primitive
 */
noinline fastcall int
tp_ordrel_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_IDLE:
		/* Note: If the interface is in the TS_IDLE state when the provider receives the
		   T_ORDREL_REQ primitive, then the TS provider should discard the request without
		   generating a fatal error. */
		goto discard;
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_IDLE);
		break;
	default:
		goto outstate;
	}
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_inform_req: - process T_OPTDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTDATA_REQ primitive
 *
 * The N-INFORM request primitive is implemented using the T_OPTDATA_REQ primitive.
 */
noinline fastcall int
tp_inform_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_IDLE:
		goto discard;
	case TS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (p->DATA_flag & ~(T_SCCP_INF))
		goto badflag;
	return n_inform_req(tp->sc, q, mp, tp->INFORM_reason);
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_reset_req: = process T_OPTDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTDATA_REQ primitive
 *
 * The N-RESET request primitive is implemented using the T_OPTDATA_REQ primitive.
 */
noinline fastcall int
tp_reset_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_RESET_REQ primitive, then the NS provider should discard the message without
		   generating an error. */
		goto discard;
	case TS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (tp->state.resreqs)
		/* If there are outstanding reset requests, we are out of state. */
		goto outstate;
	t_read_opts(tp, NULL, mp->b_rptr + p->OPT_offset, p->OPT_length);
	tp->state.resreqs = 1;
	tp_set_state(tp, NS_DATA_XFER);
	return n_reset_req(tp->sc, q, mp, tp->RESET_reason);
      outstate:
	err = TOUTSTATE;
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
 * tp_reset_res: = process T_OPTDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTDATA_REQ primitive
 *
 * The N-RESET response primitive is implemented using the T_OPTDATA_REQ primitive.
 */
noinline fastcall int
tp_reset_res(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_RESET_RES primitive, then the NS provider should discard the message without
		   generating an error. */
		goto discard;
	case TS_WRES_RIND:
		break;
	default:
		goto outstate;
	}
	if (tp->resinds <= 0)
		goto outstate;
	tp->resinds = 0;
	tp_set_state(tp, TS_DATA_XFER);
	return n_reset_res(tp->sc, q, mp);
      outstate:
	err = TOUTSTATE;
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














/**
 * tp_optdata_req: - process T_OPTDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTDATA_REQ primitive
 */
noinline fastcall int
tp_optdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (p->DATA_flag & T_SCCP_INF)
		return tp_inform_req(tp, q, mp);
	if (p->DATA_flag & T_SCCP_RST) {
		if (tp->state.resinds)
			return tp_reset_res(tp, q, mp);
		else
			return tp_reset_req(tp, q, mp);
	}
	switch (tp_get_state(tp)) {
	case TS_IDLE:
		/* Note: If the interface is in the TS_IDLE state when the provider receives the
		   T_OPTDATA_REQ primitive, then the TS provider should discard the message without
		   generating an error. */
		goto discard;
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	default:
		goto outstate;
	}
	if (tp->edatack)
		/* The TS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (p->DATA_flag & ~(T_ODF_MORE | T_ODF_EX | T_SCCP_RST | T_SCCP_INF))
		goto badflag;
	if (p->DATA_flag & T_SCCP_RST) {
		if (p->DATA_flag & (T_ODF_MORE | T_ODF_EX | T_SCCP_INF))
			goto notsupport;
		if (tp->state.resacks)
			goto outstate;
		tp->state.resacks = 1;
		return n_reset_req(tp->sc, q, mp, tp->RESET_reason);
	}
	if (p->DATA_flag & T_SCCP_INF) {
		if (p->DATA_flag & (T_ODF_MORE | T_ODF_EX | T_SCCP_RST))
			goto notsupport;
		return n_inform_req(tp->sc, q, mp, tp->INFORM_reason);
	}
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.TIDU_size)
		goto baddata;
	if (p->DATA_flag & T_ODF_EX) {
		if (p->DATA_flag & T_ODF_MORE)
			/* Note that the NPI cannot handle segmentation of expedited data and it is 
			   not permitted to use the "more" flag with expedited data. */
			goto notsupport;
		if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.ETSDU_size)
			goto baddata;
		return n_exdata_req(tp->sc, q, mp, mp->b_cont);
	}
	if (mp->b_cont && msgsize(mp->b_cont) > tp->proto.info.TSDU_size)
		goto baddata;
	return n_data_req(tp->sc, q, mp, (p->DATA_flag & T_ODF_EX) ? N_MORE_DATA_FLAG : 0,
			  mp->b_cont);
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      baddata:
	err = TBADDATA;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_addr_req: - process T_ADDR_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_ADDR_REQ primitive
 */
noinline fastcall int
tp_addr_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	tp->state.ackpending = T_ADDR_ACK;
	return n_info_req(tp->sc, q, mp);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * tp_capability_req: - process T_CAPABILITY_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_CAPABILITY_REQ primitive
 */
noinline fastcall int
tp_capability_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	tp->state.ackpending = T_CAPABILITY_ACK;
	return n_info_req(tp->sc, q, mp);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

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
	struct sockaddr_sccp *loc, *rem;
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
	return t_conn_ind(sc->tp, q, mp, loc, rem, qos, p->SEQ_number, p->CONN_flags, mp->b_cont);
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
	struct sockaddr_sccp *req;
	N_qos_sel_conn_sccp_t *qos;

	if (p->RES_length) {
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &sc->res, p->RES_length);
	}
	if (p->QOS_length) {
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &sc->qos, p->QOS_length);
	}
	sc_set_state(sc, NS_DATA_XFER);
	return t_conn_con(sc->tp, q, mp, res, qos, p->CONN_flags, mp->b_cont);
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
	struct sockaddr_sccp *res;

	if (p->RES_length) {
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &sc->res, p->RES_length);
	}
	if (p->SEQ_number)
		sc->state.coninds--;
	sc_set_state(sc, sc->state.coninds ? NS_WRES_CIND : NS_IDLE);
	return t_discon_ind(sc->tp, q, mp, res, p->DISCON_orig, p->DISCON_reason, p->SEQ_number,
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

	retrn t_data_ind(sc->tp, q, mp, p->DATA_xfer_flags, mp->b_cont);
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

	return t_exdata_ind(sc->tp, q, mp, mp->b_cont);
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
	struct sockaddr_sccp *add;
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
	return t_info_ack(sc->tp, q, mp);
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
	struct sockaddr_sccp *add;
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
	sc->token = p->TOKEN_value;
	sc->BIND_flags = p->BIND_flags;
	return t_bind_ack(sc->tp, q, mp, add, pro, p->BIND_flags);
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
	return t_error_ack(sc->tp, q, mp, p->ERROR_prim, error);
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
	return t_ok_ack(sc->tp, q, mp, p->CORRECT_prim);

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
	struct sockaddr_sccp *rem;
	struct sockaddr_sccp *loc;

	loc = p->SRC_length ? (typeof(loc)) (mp->b_rptr + p->SRC_offset) : NULL;
	rem = p->DEST_length ? (typeof(rem)) (mp->b_rptr + p->DEST_offset) : NULL;
	return t_unitdata_ind(sc->tp, q, mp, loc, rem, p->ERROR_type, mp->b_cont);
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
	struct sockaddr_sccp *rem;

	rem = p->DEST_length ? (typeof(rem)) (mp->b_rptr + p->DEST_offset) : NULL;
	return t_uderror_ind(sc->tp, q, mp, rem, p->ERROR_type, mp->b_cont);
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
	return t_datack_ind(sc->tp, q, mp);
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
	return t_reset_ind(sc->tp, q, mp, p->RESET_orig, p->RESET_reason);
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
	return t_reset_con(sc->tp, q, mp);
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
	struct sockaddr_sccp *rem;

	rem = (typeof(rem)) (mp->b_rptr + p->DEST_offset);
	/* In mapping the N_NOTICE_IND primitive to the N_UDERROR_IND primitive, the source address 
	   and quality of service parameters associated with the message are lost. */
	return t_uderror_ind(sc->tp, q, mp, rem, p->RETURN_cause, mp->b_cont);
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
	sc->state.inform = 1;
	sc_set_state(sc, NS_WRES_RIND);
	return t_inform_ind(sc->tp, q, mp, N_PROVIDER, p->REASON);
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
	N_coord_ind_t *p = (typeof(p)) mp->b_rtpr;

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

/**
 * tp_w_proto: - process M_PROTO or M_PCPROTO message
 * @tp: transport provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static int
tp_w_proto(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_scalar_t prim = -1;
	int err = -EFAULT, level;

	tp_save_state(tp);
	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	switch ((prim = *(typeof(prim) *) mp->b_rptr)) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_OPTDATA_REQ:
	case T_UNITDATA_REQ:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(q, level, SL_TRACE, "-> %s", tp_primname(prim));
	switch (prim) {
	case T_CONN_REQ:
		err = tp_conn_req(tp, q, mp);
		break;
	case T_CONN_RES:
		err = tp_conn_res(tp, q, mp);
		break;
	case T_DISCON_REQ:
		err = tp_discon_req(tp, q, mp);
		break;
	case T_DATA_REQ:
		err = tp_data_req(tp, q, mp);
		break;
	case T_EXDATA_REQ:
		err = tp_exdata_req(tp, q, mp);
		break;
	case T_INFO_REQ:
		err = tp_info_req(tp, q, mp);
		break;
	case T_BIND_REQ:
		err = tp_bind_req(tp, q, mp);
		break;
	case T_UNBIND_REQ:
		err = tp_unbind_req(tp, q, mp);
		break;
	case T_UNITDATA_REQ:
		err = tp_unitdata_req(tp, q, mp);
		break;
	case T_OPTMGMT_REQ:
		err = tp_optmgmt_req(tp, q, mp);
		break;
	case T_ORDREL_REQ:
		err = tp_ordrel_req(tp, q, mp);
		break;
	case T_OPTDATA_REQ:
		err = tp_optdata_req(tp, q, mp);
		break;
	case T_ADDR_REQ:
		err = tp_addr_req(tp, q, mp);
		break;
	case T_CAPABILITY_REQ:
		err = tp_capability_req(tp, q, mp);
		break;
	default:
		err = tp_other_req(tp, q, mp);
		break;
	}
      done:
	if (err)
		tp_restore_state(tp);
	return tp_error_reply(tp, q, mp, prim, err, tp_primname(prim));
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
		err = sc_other_ind(sc, q, mp);
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

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = tp_w_proto(&priv->tp, q, mp);
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

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = sc_r_proto(&priv->sc, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/**
 * tp_w_data: - process M_DATA or M_HPDATA message
 * @tp: transport provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: M_DATA or M_HPDATA message
 */
static int
tp_w_data(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_uscalar_t DATA_flag;
	int err;

	switch (tp_get_state(tp)) {
	case TS_IDLE:
		goto discard;
	case TS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (tp->edatack)
		/* The NS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (mp->b_cont && msgsize(mp) > tp->proto.info.TIDU_size)
		goto baddata;
	if (mp->b_band)
		return n_exdata_req(tp->sc, q, NULL, mp);
	if (!(mp->b_flag & MSGDELIM))
		DATA_xfer_flags |= N_MORE_DATA_FLAG;
	return n_data_req(tp->sc, q, NULL, 0, mp);
baddata:
	err = TBADDATA;
	goto error;
outstate:
	err = TOUTSTATE;
	goto error;
discard:
	err = 0;
	goto error;
error:
	return tp_error_reply(tp, q, mp, T_DATA_REQ, err, __FUNCTION__);
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

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = tp_w_data(&priv->tp, q, mp);
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
 * tp_w_msg: - process STREAMS message
 * @tp: transport provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: STREAMS message
 */
static int
tp_w_msg(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = tp_w_data(tp, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = tp_w_proto(tp, q, mp);
		break;
	case M_FLUSH:
		err = m_w_flush(q, mp);
		break;
	default:
		err = m_w_other(q, mp);
		break;
	}
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
 * tp_wput: - write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 */
static streamscall int
tp_wput(queue_t *q, mblk_t *mp)
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
 * tp_wsrv: - write service procedure
 * @q: active queue (write queue)
 */
static streamscall int
tp_wsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		while ((mp = getq(q))) {
			if (tp_w_msg(&priv->tp, q, mp)) {
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
 * tp_rsrv: - read service procedure
 * @q: active queue (read queue)
 */
static streamscall int
tp_rsrv(queue_t *q)
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
 * tp_rput: - read put procedure
 * @q: active queue (read queue)
 * @mp: message to put
 */
static streamscall int
tp_rput(queue_t *q, mblk_t *mp)
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

static caddr_t tp_opens = NULL;

/**
 * tp_qopen: - queue open routine
 * @q: read queue of queue pair
 * @devp: device number pointer
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @credp: credentials of opening process
 */
static streamscall int
tp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	struct priv *p;
	int err;

	if (q->q_ptr != NULL)
		return (0);
	if ((err = mi_open_comm(&tp_opens, sizeof(*p), q, devp, oflags, sflag, credp)))
		return (err);
	p = PRIV(q);
	p->tp.oq = RD(q);
	p->tp.state.state = TS_UNBND;
	p->tp.state.datinds = 0;
	p->tp.state.datacks = 0;
	p->tp.state.coninds = 0;
	p->tp.state.inform = 0;
	p->tp.proto.info.PRIM_type = T_INFO_ACK;
	p->tp.proto.info.TSDU_size = QOS_UNKNOWN;
	p->tp.proto.info.ETSDU_size = QOS_UNKNOWN;
	p->tp.proto.info.CDATA_size = QOS_UNKNOWN;
	p->tp.proto.info.DDATA_size = QOS_UNKNOWN;
	p->tp.proto.info.ADDR_size = QOS_UNKNOWN;
	p->tp.proto.info.OPTIONS_flags = QOS_UNKNOWN;
	p->tp.proto.info.TIDU_size = QOS_UNKNOWN;
	p->tp.proto.info.SERV_type = QOS_UNKNOWN;
	p->tp.proto.info.CURRENT_state = QOS_UNKNOWN;
	p->tp.proto.info.PROVIDER_type = QOS_UNKNOWN;
	p->tp.proto.info.TODU_size = QOS_UNKNOWN;
	p->tp.proto.info.TLI_version = QOS_UNKNOWN;
	/* ... */
	p->sc.oq = WR(q);
	p->sc.state.state = NS_UNBND;
	p->sc.state.datinds = 0;
	p->sc.state.datacks = 0;
	p->sc.state.coninds = 0;
	p->sc.state.inform = 0;
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
 * tp_qclose: - queue open routine
 * @q: read queue of queue pair
 * @oflags: open flags
 * @credp: credentials of closing process
 */
static streamscall int
tp_qclose(queue_t *q, int oflags, cred_t *credp)
{
	qprocsoff(q);
	mi_close_comm(&tp_opens, q);
	return (0);
}

static modID_t modid = MOD_ID;

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for TPI-SCCP.  (0 for allocation.)");

static struct fmodsw tp_fmod = {
	.f_name = MOD_NAME,
	.f_str = &tpi_sccpinfo,
	.f_flag = F_MP,
	.f_modid = MOD_ID,
	.f_kmod = THIS_MODULE,
};

/**
 * tpi_sccp_modinit: - initialize kernel module
 */
static int __init
tpi_sccp_modinit(void)
{
	int err;

	if ((err = register_strmod(&tp_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: Could not register module: err = %d\n", __FUNCTION__, -err);
		return (-err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

/**
 * tpi_sccp_modexit: - deinitialize kernel module
 */
static void __exit
tpi_sccp_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&tp_fmod, modid)) < 0)
		cmn_err(CE_WARN, "%s: Could not deregister module: err = %d\n", __FUNCTION__, -err);
	return;
}

module_init(tpi_sccp_modinit);
module_exit(tpi_sccp_modexit);

#endif				/* LINUX */
