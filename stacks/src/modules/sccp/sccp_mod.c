/*****************************************************************************

 @(#) $RCSfile: sccp_mod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-29 07:11:06 $

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

 Last Modified $Date: 2008-04-29 07:11:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sccp_mod.c,v $
 Revision 0.9.2.3  2008-04-29 07:11:06  brian
 - updating headers for release

 Revision 0.9.2.2  2007/08/14 12:18:23  brian
 - GPLv3 header updates

 Revision 0.9.2.1  2007/08/03 13:02:51  brian
 - added documentation and minimal modules

 *****************************************************************************/

#ident "@(#) $RCSfile: sccp_mod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-29 07:11:06 $"

static char const ident[] =
    "$RCSfile: sccp_mod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-29 07:11:06 $";

/*
 * This is SCCP-MOD.  It is a simplified Signalling Connection Control Part (SCCPI) module for SCCP
 * that can be pushed as a pushable STREAMS module over an MTP Stream.  Either the MTP Stream can
 * be bound before pushing or after.  This is the simple case where all SCCP messages are delivered
 * to the same Stream.
 *
 * This SCCP-MOD module presents a Signalling Connection Control Part Interface (SCCPI) to its user.
 *
 * Usage:
 * 
 *  - Open an MTP Stream.
 *  - Push the SCCP-MOD module.
 *  - Bind the Stream.
 *  - Exchange data.
 *  - Unbind the Stream.
 *  - Pop the SCCP-MOD module.
 *  - Close the MTP Stream.
 */

#define _LFS_SOURCE	1
#define _MPS_SOURCE	1
#define _SVR4_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

#include <sys/npi.h>
#include <sys/npi_mtp.h>
#include <sys/npi_sccp.h>
//#include <ss7/sccp.h>
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sccpi.h>
#include <ss7/sccpi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#define SC_DESCRIP	"SS7/SCCP (SCCP Minimal Module) STREAMS MODULE."
#define SC_REVISION	"OpenSS7 $RCSfile: sccp_mod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-29 07:11:06 $"
#define SC_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define SC_DEVICE	"Provides OpenSS7 SCCP module."
#define SC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SC_LICENSE	"GPL"
#define SC_BANNER	SC_DESCRIP	"\n" \
			SC_REVISION	"\n" \
			SC_COPYRIGHT	"\n" \
			SC_DEVICE	"\n" \
			SC_CONTACT	"\n"
#define SC_SPLASH	SC_DEVICE	" - " \
			SC_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SC_CONTACT);
MODULE_DESCRIPTION(SC_DESCRIP);
MODULE_SUPPORTED_DEVICE(SC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sccp-mod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SCCP_MOD_MOD_ID		CONFIG_STREAMS_SCCP_MOD_MODID
#define SCCP_MOD_MOD_NAME	CONFIG_STREAMS_SCCP_MOD_NAME
#endif				/* LFS */

#ifndef SCCP_MOD_MOD_NAME
#define SCCP_MOD_MOD_NAME	"sccp-mod"
#endif				/* SCCP_MOD_MOD_NAME */

#ifndef SCCP_MOD_MOD_ID
#define SCCP_MOD_MOD_ID		0
#endif				/* SCCP_MOD_MOD_ID */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define MOD_ID		SCCP_MOD_MOD_ID
#define MOD_NAME	SCCP_MOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SC_BANNER
#else				/* MODULE */
#define MOD_BANNER	SC_SPLASH
#endif				/* MODULE */

static struct module_info sc_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat sc_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sc_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

struct priv;
struct sc;
struct mt;

struct {
	struct sccp_opt_conf_df option;
	struct sccp_conf_df config;
	struct sccp_statem_df statem;
	struct sccp_stats_df statsp;
	struct sccp_stats_df stats;
	struct sccp_notify_df notify;
} df;

struct sc {
	struct priv *priv;
	struct mt *mt;
	queue_t *oq;
	cred_t cred;
	uint flags;
	t_uscalar_t state;
	t_uscalar_t oldstate;
	np_ulong token;
	np_ulong conind;
	np_ulong outcnt;
	np_ulong pcl;
	np_ulong ret;
	np_ulong slr;
	np_ulong dlr;
	struct sccp_addr src;
	uchar saddr[SCCP_MAX_ADDR_LENGTH];
	struct sccp_addr dst;
	uchar daddr[SCCP_MAX_ADDR_LENGTH];
	np_ulong sls;
	np_ulong mp;
	np_ulong imp;
	np_ulong credit;
	np_ulong p_cred;
	np_ulong ps;
	np_ulong pr;
	np_ulong datinds;
	np_ulong mtu;
	bufq_t conq;
	bufq_t sndq;
	bufq_t urgq;
	bufq_t ackq;
	bufq_t rcvq;
	N_qos_sel_conn_sccp_t cqos;
	N_qos_sel_data_sccp_t uqos;
	N_qos_sel_info_sccp_t iqos;
	N_qos_range_info_sccp_t iqor;
	uint8_t pro;
	struct lmi_option proto;	/* SCCP protocol variant and options */
	struct sccp_opt_conf_sc config;
	struct sccp_statem_sc statem;
	struct sccp_stats_sc statsp;
	struct sccp_stats_sc stats;
	struct sccp_notify_sc notify;
	struct {
		struct sccp_opt_conf_na config;
		struct sccp_statem_na statem;
		struct sccp_stats_na statsp;
		struct sccp_stats_na stats;
		struct sccp_notify_na notify;
	} na;
	struct {
		np_ulong ssn;		/* subsystem number */
		np_ulong smi;		/* subsystem multiplicity indicator */
		np_ulong level;		/* subsystem congestion level */
		np_ulong count;		/* count of congestion messages */
		struct sccp_opt_conf_ss config;
		struct sccp_statem_ss statem;
		struct sccp_stats_ss statsp;
		struct sccp_stats_ss stats;
		struct sccp_notify_ss notify;
	} ss;
	struct {
		np_ulong ssn;		/* subsystem number */
		np_ulong smi;		/* subsystem multiplicity indicator */
		np_ulong level;		/* subsystem congestion level */
		np_ulong count;		/* count of congestion messages */
		struct sccp_opt_conf_rs config;
		struct sccp_statem_rs statem;
		struct sccp_stats_rs statsp;
		struct sccp_stats_rs stats;
		struct sccp_notify_rs notify;
	} rs;
	struct {
		struct mtp_addr add;	/* MTP address */
		np_ulong level;		/* affected signalling point level */
		np_ulong count;		/* count for congestion dropping */
		struct sccp_opt_conf_sr config;
		struct sccp_statem_sr statem;
		struct sccp_stats_sr statsp;
		struct sccp_stats_sr stats;
		struct sccp_notify_sr notify;
	} sr;
	struct {
		struct mtp_addr add;	/* MTP address */
		np_ulong sccp_next_cl_sls;	/* rotate SLS values */
		np_ulong sccp_next_co_sls;	/* rotate SLS values */
		np_ulong sccp_next_slr;	/* rotate SLR values */
		np_ulong level;		/* congestion level */
		np_ulong count;		/* count for congestion dropping */
		struct sccp_opt_conf_sp config;
		struct sccp_statem_sp statem;
		struct sccp_stats_sp statsp;
		struct sccp_stats_sp stats;
		struct sccp_notify_sp notify;
	} sp;
};

#define SCCPF_DEFAULT_RC_SEL	0x00000001
#define SCCPF_LOC_ED		0x00000002
#define SCCPF_REM_ED		0x00000004
#define SCCPF_REC_CONF_OPT	0x00000008

#define SCCPF_EX_DATA_OPT	0x00000010
#define SCCPF_RETURN_ERROR	0x00000020
#define SCCPF_ACK_OUT		0x00000040

#define SCCPF_PROHIBITED	0x00000100
#define SCCPF_CONGESTED		0x00000200
#define SCCPF_SCCP_UPU		0x00000400
#define SCCPF_SUBSYSTEM_TEST	0x00000800

#define SCCPF_UNEQUIPPED	0x00001000
#define SCCPF_INACCESSIBLE	0x00002000
#define SCCPF_STRICT_ROUTING	0x00004000
#define SCCPF_STRICT_CONFIG	0x00008000

#define SCCPF_HOPC_VIOLATION	0x00010000
#define SCCPF_XLAT_FAILURE	0x00020000
#define SCCPF_ASSOC_REQUIRED	0x00040000
#define SCCPF_CHANGE_REQUIRED	0x00080000

struct mt {
	struct priv *priv;
	struct sc *sc;
	queue_t *oq;
	cred_t cred;
	t_uscalar_t state;
	t_uscalar_t oldstate;
	np_ulong psdu;			/* pSDU size */
	np_ulong pidu;			/* pIDU size */
	np_ulong podu;			/* pODU size */
	struct mtp_addr loc;		/* local signalling point */
	struct mtp_addr rem;		/* remote signalling point */
	struct lmi_option proto;	/* MTP protocol variant and options */
	struct sccp_opt_conf_mt config;
	struct sccp_statem_mt statem;
	struct sccp_stats_mt statsp;
	struct sccp_stats_mt stats;
	struct sccp_notify_mt notify;
};

struct priv {
	struct mt r_priv;		/* MTP structure on RD side */
	struct sc w_priv;		/* SCCP structure on WR side */
};

#define PRIV(q)	    ((struct priv *)(q)->q_ptr)

#define SC_PRIV(q)  (&PRIV(q)->w_priv)
#define MT_PRIV(q)  (&PRIV(q)->r_priv)

#define PAD4(len) ((len + 3) & ~3)

#define STRLOGIO	0	/* log Stream input-output controls */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

static inline const char *
sc_iocname(int cmd)
{
	switch (cmd) {
	case SCCP_IOCGOPTION:
		return ("SCCP_IOCGOPTION");
	case SCCP_IOCSOPTION:
		return ("SCCP_IOCSOPTION");
	case SCCP_IOCGCONFIG:
		return ("SCCP_IOCGCONFIG");
	case SCCP_IOCSCONFIG:
		return ("SCCP_IOCSCONFIG");
	case SCCP_IOCTCONFIG:
		return ("SCCP_IOCTCONFIG");
	case SCCP_IOCCCONFIG:
		return ("SCCP_IOCCCONFIG");
	case SCCP_IOCGSTATEM:
		return ("SCCP_IOCGSTATEM");
	case SCCP_IOCCMRESET:
		return ("SCCP_IOCCMRESET");
	case SCCP_IOCGSTATSP:
		return ("SCCP_IOCGSTATSP");
	case SCCP_IOCSSTATSP:
		return ("SCCP_IOCSSTATSP");
	case SCCP_IOCGSTATS:
		return ("SCCP_IOCGSTATS");
	case SCCP_IOCCSTATS:
		return ("SCCP_IOCCSTATS");
	case SCCP_IOCGNOTIFY:
		return ("SCCP_IOCGNOTIFY");
	case SCCP_IOCSNOTIFY:
		return ("SCCP_IOCSNOTIFY");
	case SCCP_IOCCNOTIFY:
		return ("SCCP_IOCCNOTIFY");
	case SCCP_IOCCMGMT:
		return ("SCCP_IOCCMGMT");
	case SCCP_IOCCPASS:
		return ("SCCP_IOCCPASS");
	default:
		return ("(unknown)");
	}
}
static inline const char *
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
sc_statename(np_ulong state)
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
static np_ulong
sc_get_state(struct sc *sc)
{
	return (sc->state);
}
static np_ulong
sc_set_state(struct sc *sc, np_ulong newstate)
{
	np_ulong oldstate = sc_get_state(sc);

	if (newstate != oldstate) {
		mi_strlog(sc->oq, STRLOGST, SL_TRACE, "%s <- %s", sc_statename(newstate),
			  sc_statename(oldstate));
		sc->state = newstate;
	}
	return (newstate);
}
static np_ulong
sc_save_state(struct sc *sc)
{
	return (sc->oldstate = sc_get_state(sc));
}
static np_ulong
sc_restore_state(struct sc *sc)
{
	return (sc_set_state(sc, sc->oldstate));
}

static inline const char *
mt_primname(mtp_ulong prim)
{
	switch (prim) {
	case MTP_BIND_REQ:
		return ("MTP_BIND_REQ");
	case MTP_UNBIND_REQ:
		return ("MTP_UNBIND_REQ");
	case MTP_CONN_REQ:
		return ("MTP_CONN_REQ");
	case MTP_DISCON_REQ:
		return ("MTP_DISCON_REQ");
	case MTP_ADDR_REQ:
		return ("MTP_ADDR_REQ");
	case MTP_INFO_REQ:
		return ("MTP_INFO_REQ");
	case MTP_OPTMGMT_REQ:
		return ("MTP_OPTMGMT_REQ");
	case MTP_TRANSFER_REQ:
		return ("MTP_TRANSFER_REQ");
	case MTP_OK_ACK:
		return ("MTP_OK_ACK");
	case MTP_ERROR_ACK:
		return ("MTP_ERROR_ACK");
	case MTP_BIND_ACK:
		return ("MTP_BIND_ACK");
	case MTP_ADDR_ACK:
		return ("MTP_ADDR_ACK");
	case MTP_INFO_ACK:
		return ("MTP_INFO_ACK");
	case MTP_OPTMGMT_ACK:
		return ("MTP_OPTMGMT_ACK");
	case MTP_TRANSFER_IND:
		return ("MTP_TRANSFER_IND");
	case MTP_PAUSE_IND:
		return ("MTP_PAUSE_IND");
	case MTP_RESUME_IND:
		return ("MTP_RESUME_IND");
	case MTP_STATUS_IND:
		return ("MTP_STATUS_IND");
	case MTP_RESTART_BEGINS_IND:
		return ("MTP_RESTART_BEGINS_IND");
	case MTP_RESTART_COMPLETE_IND:
		return ("MTP_RESTART_COMPLETE_IND");
	default:
		return ("(unknown)");
	}
}
static inline const char *
mt_statename(mtp_ulong state)
{
	switch (state) {
	case MTPS_UNBND:
		return ("MTPS_UNBND");
	case MTPS_WACK_BREQ:
		return ("MTPS_WACK_BREQ");
	case MTPS_IDLE:
		return ("MTPS_IDLE");
	case MTPS_WACK_CREQ:
		return ("MTPS_WACK_CREQ");
	case MTPS_WCON_CREQ:
		return ("MTPS_WCON_CREQ");
	case MTPS_CONNECTED:
		return ("MTPS_CONNECTED");
	case MTPS_WACK_UREQ:
		return ("MTPS_WACK_UREQ");
	case MTPS_WACK_DREQ6:
		return ("MTPS_WACK_DREQ6");
	case MTPS_WACK_DREQ9:
		return ("MTPS_WACK_DREQ9");
	case MTPS_WACK_OPTREQ:
		return ("MTPS_WACK_OPTREQ");
	case MTPS_WREQ_ORDREL:
		return ("MTPS_WREQ_ORDREL");
	case MTPS_WIND_ORDREL:
		return ("MTPS_WIND_ORDREL");
	case MTPS_WRES_CIND:
		return ("MTPS_WRES_CIND");
	case MTPS_UNUSABLE:
		return ("MTPS_UNUSABLE");
	default:
		return ("(unknown)");
	}
}
static mtp_ulong
mt_get_state(struct mt *mt)
{
	return (mt->state);
}

static mtp_ulong
mt_set_state(struct mt *mt, mtp_ulong newstate)
{
	mtp_ulong oldstate = mt_get_state(mt);

	if (newstate != oldstate) {
		mi_strlog(mt->oq, STRLOGST, SL_TRACE, "%s <- %s", mt_statename(newstate),
			  mt_statename(oldstate));
		mt->state = newstate;
	}
	return (newstate);
}

static mtp_ulong
mt_save_state(struct mt *mt)
{
	return (mt->oldstate = mt_get_state(mt));
}

static mtp_ulong
mt_restore_state(struct mt *mt)
{
	return (mt_set_state(mt, mt->oldstate));
}

/*
 *  =========================================================================
 *
 *  SCCP Message Definitions
 *
 *  =========================================================================
 */

#define SCCP_MT_NONE	0x00	/* (reserved) */
#define SCCP_MT_CR	0x01	/* Connection Request */
#define SCCP_MT_CC	0x02	/* Connection Confirm */
#define SCCP_MT_CREF	0x03	/* Connection Refused */
#define SCCP_MT_RLSD	0x04	/* Released */
#define SCCP_MT_RLC	0x05	/* Release Complete */
#define SCCP_MT_DT1	0x06	/* Data Form 1 */
#define SCCP_MT_DT2	0x07	/* Data Form 2 */
#define SCCP_MT_AK	0x08	/* Data Acknowledgement */
#define SCCP_MT_UDT	0x09	/* Unitdata */
#define SCCP_MT_UDTS	0x0a	/* Unitdata Service */
#define SCCP_MT_ED	0x0b	/* Expedited Data */
#define SCCP_MT_EA	0x0c	/* Expedited Data Acknowledgement */
#define SCCP_MT_RSR	0x0d	/* Reset Request */
#define SCCP_MT_RSC	0x0e	/* Reset Confirm */
#define SCCP_MT_ERR	0x0f	/* Protocol Data Unit Error */
#define SCCP_MT_IT	0x10	/* Inactivity Test */
#define SCCP_MT_XUDT	0x11	/* Extended Unitdata */
#define SCCP_MT_XUDTS	0x12	/* Extended Unitdata Service */
#define SCCP_MT_LUDT	0x13	/* Long Unitdata */
#define SCCP_MT_LUDTS	0x14	/* Long Unitdata Service */
#define SCCP_NOMESSAGES	0x15	/* number of message types */

#define SCMG_MT_SSA	0x01	/* Subsystem allowed */
#define SCMG_MT_SSP	0x02	/* Subsystem prohibited */
#define SCMG_MT_SST	0x03	/* Subsystem status test */
#define SCMG_MT_SOR	0x04	/* Subsystem out of service request */
#define SCMG_MT_SOG	0x05	/* Subsystem out of service grant */
#define SCMG_MT_SSC	0x06	/* Subsystem congestion */

#define SCMG_MT_SBR	0xfd	/* Subsystem backup routing */
#define SCMG_MT_SNR	0xfe	/* Subsystem normal routing */
#define SCMG_MT_SRT	0xff	/* Subsystem routing status test */

#define SCCP_MP_CR	0x0	/* 0 or 1 */
#define SCCP_MP_CC	0x1
#define SCCP_MP_CREF	0x1
#define SCCP_MP_RLSD	0x2
#define SCCP_MP_RLC	0x2
#define SCCP_MP_DT1	0x0	/* 0 or 1 */
#define SCCP_MP_DT2	0x0	/* 0 or 1 */
#define SCCP_MP_AK	0x0	/* 0 or 1 */
#define SCCP_MP_UDT	0x0	/* 0, 1 or 2 */
#define SCCP_MP_UDTS	0x0	/* 0, 1 or 2 */
#define SCCP_MP_ED	0x1
#define SCCP_MP_EA	0x1
#define SCCP_MP_RSR	0x1
#define SCCP_MP_RSC	0x1
#define SCCP_MP_ERR	0x1
#define SCCP_MP_IT	0x1
#define SCCP_MP_XUDT	0x0	/* 0, 1 or 2 */
#define SCCP_MP_XUDTS	0x0	/* 0, 1 or 2 */
#define SCCP_MP_LUDT	0x0	/* 0, 1 or 2 */
#define SCCP_MP_LUDTS	0x0	/* 0, 1 or 2 */

/*
 *  Importance values from Table 2/Q.714-1996
 */
/*
   default importance values 
 */
#define SCCP_DI_CR	0x2	/* inc at relay */
#define SCCP_DI_CC	0x3	/* inc at relay */
#define SCCP_DI_CREF	0x2	/* inc at relay */
#define SCCP_DI_DT1	0x4	/* def at relay */
#define SCCP_DI_DT2	0x4	/* def at relay */
#define SCCP_DI_AK	0x6	/* def at relay */
#define SCCP_DI_IT	0x6	/* def at relay */
#define SCCP_DI_ED	0x7	/* def at relay */
#define SCCP_DI_EA	0x7	/* def at relay */
#define SCCP_DI_RSR	0x6	/* def at relay */
#define SCCP_DI_RSC	0x6	/* def at relay */
#define SCCP_DI_ERR	0x7	/* def at relay */
#define SCCP_DI_RLC	0x4	/* def at relay */
#define SCCP_DI_RLSD	0x6	/* inc at relay */
#define SCCP_DI_UDT	0x4	/* def at relay */
#define SCCP_DI_UDTS	0x3	/* def at relay */
#define SCCP_DI_XUDT	0x4	/* inc at relay */
#define SCCP_DI_XUDTS	0x3	/* inc at relay */
#define SCCP_DI_LUDT	0x4	/* inc at relay */
#define SCCP_DI_LUDTS	0x3	/* inc at relay */

/*
   maximum importance values 
 */
#define SCCP_MI_CR	0x4
#define SCCP_MI_CC	0x4
#define SCCP_MI_CREF	0x4
#define SCCP_MI_DT1	0x6
#define SCCP_MI_DT2	0x6
#define SCCP_MI_AK	SCCP_DI_AK
#define SCCP_MI_IT	SCCP_DI_IT
#define SCCP_MI_ED	SCCP_DI_ED
#define SCCP_MI_EA	SCCP_DI_EA
#define SCCP_MI_RSR	SCCP_DI_RSR
#define SCCP_MI_RSC	SCCP_DI_RSC
#define SCCP_MI_ERR	SCCP_DI_ERR
#define SCCP_MI_RLC	SCCP_DI_RLC
#define SCCP_MI_RLSD	0x6
#define SCCP_MI_UDT	0x6
#define SCCP_MI_UDTS	0x0	/* same as returned UDT */
#define SCCP_MI_XUDT	0x6
#define SCCP_MI_XUDTS	0x0	/* same as returned XUDT */
#define SCCP_MI_LUDT	0x6
#define SCCP_MI_LUDTS	0x0	/* same as returned LUDT */

#define SCMG_MP_SSA	0x3
#define SCMG_MP_SSP	0x3
#define SCMG_MP_SST	0x2
#define SCMG_MP_SOR	0x1
#define SCMG_MP_SOG	0x1
#define SCMG_MP_SSC	0x0	/* not defined for ANSI */

#define SCMG_MP_SBR	0x0
#define SCMG_MP_SNR	0x0
#define SCMG_MP_SRT	0x0

/*
 *  Notes on message priority:
 *
 *  (1) The UDTS, XUDTS and LUDTS messages assume the message priority of the
 *      message which they contain.
 *
 *  (2) The UDT, XUDT and LUDT messages can assume the message priority of 0
 *      or 1: the value 2 is reserved for use by OMAP.
 *
 *  (3) The priority of DT1, DT2 and AK should match that of the connection
 *      request.
 */

#define SCCP_MTF_NONE	(1<<SCCP_MT_NONE	)
#define SCCP_MTF_CR	(1<<SCCP_MT_CR		)
#define SCCP_MTF_CC	(1<<SCCP_MT_CC		)
#define SCCP_MTF_CREF	(1<<SCCP_MT_CREF	)
#define SCCP_MTF_RLSD	(1<<SCCP_MT_RLSD	)
#define SCCP_MTF_RLC	(1<<SCCP_MT_RLC		)
#define SCCP_MTF_DT1	(1<<SCCP_MT_DT1		)
#define SCCP_MTF_DT2	(1<<SCCP_MT_DT2		)
#define SCCP_MTF_AK	(1<<SCCP_MT_AK		)
#define SCCP_MTF_UDT	(1<<SCCP_MT_UDT		)
#define SCCP_MTF_UDTS	(1<<SCCP_MT_UDTS	)
#define SCCP_MTF_ED	(1<<SCCP_MT_ED		)
#define SCCP_MTF_EA	(1<<SCCP_MT_EA		)
#define SCCP_MTF_RSR	(1<<SCCP_MT_RSR		)
#define SCCP_MTF_RSC	(1<<SCCP_MT_RSC		)
#define SCCP_MTF_ERR	(1<<SCCP_MT_ERR		)
#define SCCP_MTF_IT	(1<<SCCP_MT_IT		)
#define SCCP_MTF_XUDT	(1<<SCCP_MT_XUDT	)
#define SCCP_MTF_XUDTS	(1<<SCCP_MT_XUDTS	)
#define SCCP_MTF_LUDT	(1<<SCCP_MT_LUDT	)
#define SCCP_MTF_LUDTS	(1<<SCCP_MT_LUDTS	)

/*
 *  Connection-Oriented messages
 */
#define SCCP_MTM_CO	(	SCCP_MTF_CR	\
			|	SCCP_MTF_CC	\
			|	SCCP_MTF_CREF	\
			|	SCCP_MTF_RLSD	\
			|	SCCP_MTF_DT1	\
			|	SCCP_MTF_DT2	\
			|	SCCP_MTF_AK	\
			|	SCCP_MTF_ED	\
			|	SCCP_MTF_EA	\
			|	SCCP_MTF_RSR	\
			|	SCCP_MTF_RSC	\
			|	SCCP_MTF_ERR	\
			|	SCCP_MTF_IT	\
			)
/*
 *  Conection-Less Messages
 */
#define SCCP_MTM_CL	(	SCCP_MTF_UDT	\
			|	SCCP_MTF_UDTS	\
			|	SCCP_MTF_XUDT	\
			|	SCCP_MTF_XUDTS	\
			|	SCCP_MTF_LUDT	\
			|	SCCP_MTF_LUDTS	\
			)
/*
 *  Messages that might require GTT
 */
#define SCCP_MTM_GT	( SCCP_MTM_CL | SCCP_MTF_CR )
/*
 *  Protocol Class 0 messages
 */
#define SCCP_MTM_PCLS0	(	SCCP_MTM_CL	)
/*
 *  Protocol Class 1 messages
 */
#define SCCP_MTM_PCLS1	(	SCCP_MTM_CL	)
/*
 *  Protocol Class 2 messages
 */
#define SCCP_MTM_PCLS2	(	SCCP_MTF_CR	\
			|	SCCP_MTF_CC	\
			|	SCCP_MTF_CREF	\
			|	SCCP_MTF_RLSD	\
			|	SCCP_MTF_DT1	\
			|	SCCP_MTF_ERR	\
			|	SCCP_MTF_IT	\
			)
/*
 *  Protocol Class 3 messages
 */
#define SCCP_MTM_PCLS3	(	SCCP_MTF_CR	\
			|	SCCP_MTF_CC	\
			|	SCCP_MTF_CREF	\
			|	SCCP_MTF_RLSD	\
			|	SCCP_MTF_DT2	\
			|	SCCP_MTF_AK	\
			|	SCCP_MTF_ED	\
			|	SCCP_MTF_EA	\
			|	SCCP_MTF_RSR	\
			|	SCCP_MTF_RSC	\
			|	SCCP_MTF_ERR	\
			|	SCCP_MTF_IT	\
			)

#define SCCP_PT_EOP	 0	/* End of Optional Parameters */
#define SCCP_PT_DLR	 1	/* Destination Local Reference */
#define SCCP_PT_SLR	 2	/* Source Local reference */
#define SCCP_PT_CDPA	 3	/* Called Party Address */
#define SCCP_PT_CGPA	 4	/* Calling Party Address */
#define SCCP_PT_PCLS	 5	/* Protocol Class */
#define SCCP_PT_SEG	 6	/* Segmenting/Reassembly */
#define SCCP_PT_RSN	 7	/* Receive Sequence Number */
#define SCCP_PT_SEQ	 8	/* Sequencing/Segmenting */
#define SCCP_PT_CRED	 9	/* Credit */
#define SCCP_PT_RELC	10	/* Release Cause */
#define SCCP_PT_RETC	11	/* Return Cause */
#define SCCP_PT_RESC	12	/* Reset Cause */
#define SCCP_PT_ERRC	13	/* Error Cause */
#define SCCP_PT_REFC	14	/* Refusal Cause */
#define SCCP_PT_DATA	15	/* Data */
#define SCCP_PT_SGMT	16	/* Segmentation */
#define SCCP_PT_HOPC	17	/* Hop Counter */
#define SCCP_PT_IMP	18	/* Importance */
#define SCCP_PT_LDATA	19	/* Long Data */
#define SCCP_PT_ISNI	-6	/* Intermediate Sig. Ntwk. Id */
#define SCCP_PT_INS	-7	/* Intermediate Network Selection */
#define SCCP_PT_MTI	-8	/* Message Type Interworking */

#define SCCP_PTF_EOP	(1<<SCCP_PT_EOP  )
#define SCCP_PTF_DLR	(1<<SCCP_PT_DLR  )
#define SCCP_PTF_SLR	(1<<SCCP_PT_SLR  )
#define SCCP_PTF_CDPA	(1<<SCCP_PT_CDPA )
#define SCCP_PTF_CGPA	(1<<SCCP_PT_CGPA )
#define SCCP_PTF_PCLS	(1<<SCCP_PT_PCLS )
#define SCCP_PTF_SEG	(1<<SCCP_PT_SEG  )
#define SCCP_PTF_RSN	(1<<SCCP_PT_RSN  )
#define SCCP_PTF_SEQ	(1<<SCCP_PT_SEQ  )
#define SCCP_PTF_CRED	(1<<SCCP_PT_CRED )
#define SCCP_PTF_RELC	(1<<SCCP_PT_RELC )
#define SCCP_PTF_RETC	(1<<SCCP_PT_RETC )
#define SCCP_PTF_RESC	(1<<SCCP_PT_RESC )
#define SCCP_PTF_ERRC	(1<<SCCP_PT_ERRC )
#define SCCP_PTF_REFC	(1<<SCCP_PT_REFC )
#define SCCP_PTF_DATA	(1<<SCCP_PT_DATA )
#define SCCP_PTF_SGMT	(1<<SCCP_PT_SGMT )
#define SCCP_PTF_HOPC	(1<<SCCP_PT_HOPC )
#define SCCP_PTF_IMP	(1<<SCCP_PT_IMP  )
#define SCCP_PTF_LDATA	(1<<SCCP_PT_LDATA)
#define SCCP_PTF_MTI	(0x80000000>>-SCCP_PT_MTI )
#define SCCP_PTF_INS	(0x80000000>>-SCCP_PT_INS )
#define SCCP_PTF_ISNI	(0x80000000>>-SCCP_PT_ISNI)

#define SCCP_MTI_OMT_UNQUAL	0x00
#define SCCP_MTI_OMT_UDT_S	0x01
#define SCCP_MTI_OMT_XUDT_S	0x02
#define SCCP_MTI_OMT_LUDT_S	0x03
#define SCCP_MTI_DROP		0x40

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  =========================================================================
 */

static inline fastcall int
m_flush(struct sc *sc, queue_t *q, mblk_t *msg, int band, int flags, int what)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_FLUSH;
		*mp->b_wptr++ = flags | (band ? FLUSHBAND : 0);
		*mp->b_wptr++ = band;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_FLUSH");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static inline fastcall int
m_error(struct sc *sc, queue_t *q, mblk_t *msg, int error)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = error < 0 ? -error : error;
		*mp->b_wptr++ = error < 0 ? -error : error;
		sc_set_state(sc, NS_NOSTATES);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static inline fastcall int
m_hangup(struct sc *sc, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 0, BPRI_MED))) {
		DB_TYPE(mp) = M_HANGUP;
		sc_set_state(sc, NS_NOSTATES);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_HANGUP");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static inline fastcall int
m_error_reply(struct sc *sc, queue_t *q, mblk_t *msg, int error)
{
	if (error < 0)
		error = -error;
	switch (error) {
	case 0:
	case EBUSY:
	case ENOBUFS:
	case ENOMEM:
	case EAGAIN:
	case EDEADLK:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		return m_hangup(sc, q, msg);
	default:
		return m_error(sc, q, msg, error);
	}
}

/*
 *  =========================================================================
 *
 *  SCCP Provider -> SCCP User Primitives
 *
 *  =========================================================================
 */

/**
 * sc_conn_ind: - issue N_CONN_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cp: connection pointer
 */
static inline fastcall int
sc_conn_ind(struct sc *sc, queue_t *q, mblk_t *msg, mblk_t *cp, mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;

	size_t dst_len = sizeof(sc->dst) + sc->dst.alen;
	size_t src_len = sizeof(sc->src) + sc->src.alen;
	size_t qos_len = sizeof(sc->cqos);
	size_t msg_len = sizeof(*p) + PAD4(dst_len) + PAD4(src_len) + qos_len;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->SRC_length = src_len;
			p->SRC_offset = p->DEST_offset + PAD4(p->DEST_length);
			p->SEQ_number = (np_ulong) (ulong) cp;
			p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
			p->QOS_length = qos_len;
			p->QOS_offset = p->SRC_offset + PAD4(p->SRC_length);
			mp->b_wptr += sizeof(*p);
			bcopy(&sc->dst, mp->b_wptr, dst_len);
			mp->b_wptr += PAD4(dst_len);
			bcopy(&sc->src, mp->b_wptr, src_len);
			mp->b_wptr += PAD4(src_len);
			bcopy(&sc->cqos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (cp && cp->b_cont == dp)
				cp->b_cont = NULL;
			if (msg && msg->b_cont == cp)
				msg->b_cont = NULL;
			freemsg(msg);
			bufq_queue(&sc->conq, cp);
			sc_set_state(sc, NS_WRES_CIND);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_CONN_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_conn_con: - issue N_CONN_CON primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: connection flags
 * @dp: user data
 */
static inline fastcall int
sc_conn_con(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong flags, struct sccp_addr *res,
	    N_qos_sel_conn_sccp_t * qos, mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;

	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + PAD4(res_len) + qos_len;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = p->RES_offset + PAD4(p->RES_length);
			mp->b_wptr += sizeof(*p);
			bcopy(res, mp->b_wptr, res_len);
			mp->b_wptr += PAD4(res_len);
			bcopy(qos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			sc_set_state(sc, NS_DATA_XFER);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_CONN_CON");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_discon_ind: - issue N_DISCON_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: origin of disconnect
 * @reason: reason for disconnect
 * @cp: sequence number (or NULL)
 * @dp: user data
 */
static inline fastcall int
sc_discon_ind(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason,
	      struct sccp_addr *res, mblk_t *cp, mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;

	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t msg_len = sizeof(*p) + PAD4(res_len);

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_IND;
			p->DISCON_orig = orig;
			p->DISCON_reason = reason;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = (np_ulong) (ulong) cp;
			mp->b_wptr += sizeof(*p);
			bcopy(res, mp->b_wptr, res_len);
			mp->b_wptr += PAD4(res_len);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			if (cp) {
				bufq_unlink(&sc->conq, cp);
				freemsg(cp);
			}
			sc_set_state(sc, bufq_length(&sc->conq) ? NS_WRES_CIND : NS_IDLE);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DISCON_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_data_ind: - issue N_DATA_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @more: more data in NSDU
 */
static inline fastcall int
sc_data_ind(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong more,
	    N_qos_sel_data_sccp_t * qos, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;

	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + qos_len;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_IND;
			p->DATA_xfer_flags = (more ? N_MORE_DATA_FLAG : 0);
			mp->b_wptr += sizeof(*p);
			bcopy(qos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- N_DATA_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_exdata_ind: - issue N_EXDATA_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static inline fastcall int
sc_exdata_ind(struct sc *sc, queue_t *q, mblk_t *msg, N_qos_sel_data_sccp_t * qos, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;

	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + qos_len;

	if (bcanputnext(sc->oq, 1)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_IND;
			mp->b_wptr += sizeof(*p);
			bcopy(qos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_EXDATA_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_info_ack: - issue N_INFO_ACK primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_info_ack(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;

	size_t add_len = sizeof(sc->src) + sc->src.alen;
	size_t qos_len = sizeof(sc->iqos);
	size_t qor_len = sizeof(sc->iqor);
	size_t pro_len = sizeof(sc->pro);
	size_t msg_len = sizeof(*p) + PAD4(add_len) + qos_len + qor_len + pro_len;

	if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = -1;	/* no limit on NSDU size */
		p->ENSDU_size = -1;	/* no limit on ENSDU size */
		p->CDATA_size = -1;	/* no limit on CDATA size */
		p->DDATA_size = -1;	/* no limit on DDATA size */
		p->ADDR_size = sizeof(struct sccp_addr) + SCCP_MAX_ADDR_LENGTH;
		p->ADDR_length = add_len;
		p->ADDR_offset = sizeof(*p);
		p->QOS_length = qos_len;
		p->QOS_offset = p->ADDR_offset + PAD4(p->ADDR_length);
		p->QOS_range_length = qor_len;
		p->QOS_range_offset = p->QOS_offset + p->QOS_length;
		p->OPTIONS_flags = REC_CONF_OPT | EX_DATA_OPT |
		    ((sc->flags & SCCPF_DEFAULT_RC_SEL) ? DEFAULT_RC_SEL : 0);
		p->NIDU_size = -1;
		p->SERV_type = N_CONS | N_CLNS;
		p->CURRENT_state = sc_get_state(sc);
		p->PROVIDER_type = N_SUBNET;
		p->NODU_size = 254;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = p->QOS_range_offset + p->QOS_range_length;
		p->NPI_version = N_CURRENT_VERSION;
		mp->b_wptr += sizeof(*p);
		bcopy(&sc->src, mp->b_wptr, add_len);
		mp->b_wptr += PAD4(add_len);
		bcopy(&sc->iqos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		bcopy(&sc->iqor, mp->b_wptr, qor_len);
		mp->b_wptr += qor_len;
		bcopy(&sc->pro, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_INFO_ACK");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sc_bind_ack: - issue N_BIND_ACK primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_bind_ack(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_bind_ack_t *p;
	mblk_t *mp;

	size_t add_len = sizeof(sc->src) + sc->src.alen;
	size_t pro_len = sizeof(sc->pro);
	size_t msg_len = sizeof(*p) + PAD4(add_len) + PAD4(pro_len);

	if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = sc->conind;
		p->TOKEN_value = (np_ulong) (ulong) sc->oq;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = p->ADDR_offset + PAD4(p->ADDR_length);
		mp->b_wptr += sizeof(*p);
		bcopy(&sc->src, mp->b_wptr, add_len);
		mp->b_wptr += PAD4(add_len);
		bcopy(&sc->pro, mp->b_wptr, pro_len);
		mp->b_wptr += PAD4(pro_len);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_BIND_ACK");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sc_error_ack: - issue N_ERROR_ACK primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: error number
 */
static inline fastcall int
sc_error_ack(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong prim, np_long err)
{
	N_error_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err < 0 ? NSYSERR : err;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		sc_restore_state(sc);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static inline fastcall int
sc_error_reply(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong prim, np_long err)
{
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
		return (err);
	case 0:
		freemsg(msg);
		return (0);
	}
	return sc_error_ack(sc, q, msg, prim, err);
}

/**
 * sc_ok_ack: - issue N_OK_ACK primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_ok_ack(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong prim, mblk_t *cp, struct sc *ap)
{
	N_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (sc_get_state(sc)) {
		case NS_WACK_OPTREQ:
			sc_set_state(sc, NS_IDLE);
			break;
		case NS_WACK_RRES:
			sc_set_state(sc, NS_DATA_XFER);
			break;
		case NS_WACK_UREQ:
			sc_set_state(sc, NS_UNBND);
			break;
		case NS_WACK_CRES:
			if (ap)
				sc_set_state(ap, NS_DATA_XFER);
			if (cp) {
				bufq_unlink(&sc->conq, cp);
				freemsg(cp);
			}
			if (ap != sc)
				sc_set_state(sc, bufq_length(&sc->conq) ? NS_WRES_CIND : NS_IDLE);
			break;
		case NS_WACK_DREQ7:
			if (cp)
				bufq_unlink(&sc->conq, cp);
			/* fall through */
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			sc_set_state(sc, bufq_length(&sc->conq) ? NS_WRES_CIND : NS_IDLE);
			break;
		default:
			/* Note: if we are not in a WACK state we simply do not change state.  This
			   occurs normally when we are responding to a N_OPTMGMT_REQ primitive in
			   other than the NS_IDLE state. */
			break;
		}
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_OK_ACK");
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sc_unitdata_ind: - issue N_UNITDATA_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @src: source address
 * @dst: destination address
 * @dp: user data
 */
static inline fastcall int
sc_unitdata_ind(struct sc *sc, queue_t *q, mblk_t *msg,
		struct sccp_addr *src, struct sccp_addr *dst, mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;

	size_t src_len = src ? sizeof(*src) + src->alen : 0;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t msg_len = sizeof(*p) + PAD4(src_len) + PAD4(dst_len);

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->DEST_length = dst_len;
			p->DEST_offset = p->SRC_offset + PAD4(p->SRC_length);
			mp->b_wptr += sizeof(*p);
			bcopy(src, mp->b_wptr, src_len);
			mp->b_wptr += PAD4(src_len);
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += PAD4(dst_len);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- N_UNITDATA_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_uderror_ind: - issue N_UDERROR_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * NOTE:- We might want to do a special N-NOTICE primitive here because the N_UDERROR_IND primitive
 * does not have that much information about the returned message.  Or, if we attach the M_DATA
 * block, the use might be able to gleen more information from the message itself.  Another approach
 * is to stuff some extra information at the end of the primitive.  Here we attached the returned
 * message in the M_DATA portion.
 */
static inline fastcall int
sc_uderror_ind(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong etype, struct sccp_addr *dst,
	       mblk_t *dp)
{
	N_uderror_ind_t *p;
	mblk_t *mp;

	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t msg_len = sizeof(*p) + dst_len;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->RESERVED_field = 0;
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += PAD4(dst_len);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_UDERROR_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_datack_ind: - issue N_DATACK_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_datack_ind(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;

	size_t qos_len = sizeof(sc->uqos);
	size_t msg_len = sizeof(*p) + qos_len;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_IND;
			mp->b_wptr += sizeof(*p);
			bcopy(&sc->uqos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DATACK_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_reset_ind: - issue N_RESET_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: origin of reset
 * @reason: reason for reset
 * @rp: reset pointer
 */
static inline fastcall int
sc_reset_ind(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason, mblk_t *rp)
{
	N_reset_ind_t *p;
	mblk_t *mp;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = orig;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			bufq_queue(&sc->conq, rp);
			sc_set_state(sc, NS_WRES_RIND);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_RESET_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_reset_con: - issue N_RESET_CON primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_reset_con(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_reset_con_t *p;
	mblk_t *mp;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			sc_set_state(sc, NS_DATA_XFER);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_RESET_CON");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_notice_ind: - issue N_NOTICE_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_notice_ind(struct sc *sc, queue_t *q, mblk_t *msg,
	      np_ulong cause, struct sccp_addr *dst, struct sccp_addr *src,
	      N_qos_sel_data_sccp_t * qos, mblk_t *dp)
{
	N_notice_ind_t *p;
	mblk_t *mp;

	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t src_len = dst ? sizeof(*src) + src->alen : 0;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + PAD4(dst_len) + PAD4(src_len) + qos_len;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_NOTICE_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->SRC_length = src_len;
			p->SRC_offset = p->DEST_offset + PAD4(p->DEST_length);
			p->QOS_length = qos_len;
			p->QOS_offset = p->SRC_offset + PAD4(p->SRC_length);
			p->RETURN_cause = cause;
			mp->b_wptr += sizeof(*p);
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += PAD4(dst_len);
			bcopy(src, mp->b_wptr, src_len);
			mp->b_wptr += PAD4(src_len);
			bcopy(qos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_NOTICE_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_inform_ind: - issue N_INFORM_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_inform_ind(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong reason, N_qos_sel_infr_sccp_t * qos)
{
	N_inform_ind_t *p;
	mblk_t *mp;

	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + qos_len;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_INFORM_IND;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(qos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_INFORM_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_coord_ind: - issue N_COORD_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_coord_ind(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *add)
{
	N_coord_ind_t *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + PAD4(add_len);

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_COORD_IND;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += PAD4(add_len);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_COORD_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_coord_con: - issue N_COORD_CON primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_coord_con(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *add, np_ulong smi)
{
	N_coord_con_t *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + PAD4(add_len);

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_COORD_CON;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			p->SMI = smi;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += PAD4(add_len);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_COORD_CON");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_state_ind: - issue N_STATE_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_state_ind(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *add,
	     np_ulong status, np_ulong smi)
{
	N_state_ind_t *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + PAD4(add_len);

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_STATE_IND;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			p->STATUS = status;
			p->SMI = smi;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += PAD4(add_len);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_STATE_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_pcstate_ind: - issue N_PCSTATE_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
sc_pcstate_ind(struct sc *sc, queue_t *q, mblk_t *msg, struct mtp_addr *add, np_ulong status)
{
	N_pcstate_ind_t *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_PCSTATE_IND;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			p->STATUS = status;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_PCSTATE_IND");
			putnext(sc->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sc_traffic_ind: - issue N_TRAFFIC_IND primitive
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * ANSI T1.112.1-2000 2.3.2.2.2 Affected User.  The parameter "affected user" identifies an affected
 * user that is failed, withdrawn (has requested withdrawal), congested or allowed.  The affected
 * user parameter contains the same type of information as the called address and calling address.
 * In the case of N-TRAFFIC primitive (see 2.3.2.3.3), the parameter identifies the user from which
 * a preferred subsystem is receiving backup traffic.
 */
static inline fastcall int
sc_traffic_ind(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *add, np_ulong tmix)
{
	N_traffic_ind_t *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + PAD4(add_len);

	if (canputnext(sc->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_TRAFFIC_IND;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			p->TRAFFIC_mix = tmix;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += PAD4(add_len);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_TRAFFIC_IND");
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
 *  MTP User -> MTP Provider Primitives
 *
 *  =========================================================================
 */

/**
 * mtp_bind_req: - issued MTP_BIND_REQ primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_bind_req(struct mt *mt, queue_t *q, mblk_t *msg, mtp_ulong flags, struct mtp_addr *add)
{
	struct MTP_bind_req *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (canputnext(mt->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_BIND_REQ;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = sizeof(*p);
			p->mtp_bind_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mt_set_state(mt, MTPS_WACK_BREQ);
			mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_BIND_REQ ->");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mtp_unbind_req: - issued MTP_UNBIND_REQ primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_unbind_req(struct mt *mt, queue_t *q, mblk_t *msg)
{
	struct MTP_unbind_req *p;
	mblk_t *mp;

	if (canputnext(mt->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mt_set_state(mt, MTPS_WACK_UREQ);
			mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_UNBIND_REQ ->");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mtp_conn_req: - issued MTP_CONN_REQ primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_conn_req(struct mt *mt, queue_t *q, mblk_t *msg, mtp_ulong flags, struct mtp_addr *add,
	     mblk_t *dp)
{
	struct MTP_conn_req *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (canputnext(mt->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_CONN_REQ;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = sizeof(*p);
			p->mtp_conn_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mt_set_state(mt, MTPS_WACK_CREQ);
			mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_CONN_REQ ->");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mtp_discon_req: - issued MTP_DISCON_REQ primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_discon_req(struct mt *mt, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct MTP_discon_req *p;
	mblk_t *mp;

	if (canputnext(mt->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_DISCON_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_DISCON_REQ ->");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mtp_addr_req: - issued MTP_ADD_REQ primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_addr_req(struct mt *mt, queue_t *q, mblk_t *msg)
{
	struct MTP_addr_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_ADDR_REQ ->");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_info_req: - issued MTP_INFO_REQ primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_info_req(struct mt *mt, queue_t *q, mblk_t *msg)
{
	struct MTP_info_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_INFO_REQ ->");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_req: - issued MTP_OPTMGMT_REQ primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_optmgmt_req(struct mt *mt, queue_t *q, mblk_t *msg, mtp_ulong flags, size_t opt_len,
		caddr_t opt_ptr)
{
	struct MTP_optmgmt_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OPTMGMT_REQ;
		p->mtp_opt_length = opt_len;
		p->mtp_opt_offset = sizeof(*p);
		p->mtp_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_OPTMGMT_REQ ->");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_transfer_req: - issued MTP_TRANSFER_REQ primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_transfer_req(struct mt *mt, queue_t *q, mblk_t *msg, struct mtp_addr *add, mtp_ulong prior,
		 mtp_ulong sls, mblk_t *dp)
{
	struct MTP_transfer_req *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (canputnext(mt->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_TRANSFER_REQ;
			p->mtp_dest_length = add_len;
			p->mtp_dest_offset = sizeof(*p);
			p->mtp_mp = prior;
			p->mtp_sls = sls;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "MTP_TRANSFER_REQ ->");
			putnext(mt->oq, mp);
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

/*
 *  =========================================================================
 *
 *  SEND SCCP MESSAGES
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  RECEIVE SCCP MESSAGES
 *
 *  =========================================================================
 */

#if 0
/**
 * sccp_send_cr: - send Connection Request (CR)
 * @sc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static int
sccp_send_cr(struct sc *sc, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct sr *sr;
	struct rs *rs;
	struct mt *mt;
	np_ulong refc;

	if (sc->dst.pc == -1 || sc->dst.ri != SCCP_RI_DPC_SSN)
		/* cannot route on global title */
		goto access_failure;
	if (!(sr = sc_lookup_sr(sc, sc->dst.pc, 1)))
		goto destination_address_unknown;
	if (sr->flags & (SCCPF_PROHIBITED))
		goto destination_inaccessible;
	if (sr->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
		goto sccp_failure;
	if (sr->flags & (SCCPF_CONGESTED)) {
		if (sc->proto.popt & SS7_POPT_MPLEV) {
			if (sc->cqos.message_priority < sr->level)
				goto access_congestion;
		} else if (sc->cqos.importance <= sr->level) {
			if (sc->cqos.importance < sr->level || (sr->count++ & 0x3) < sr->level)
				goto access_congestion;
		}
	}
	if (!(rs = sc_lookup_rs(sr, sc->dst.add.ssn, 1)))
		goto unequipped_user;
	if (rs->flags & (SCCPF_PROHIBITED))
		goto subsystem_failure;
	mt = sc->mt;
	if (!canputnext(mt->oq))
		goto access_congestion;
	{
		mblk_t *mp;
		size_t msg_len = sc_size_cr(sc);

		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			register uchar *p;
			register int len;

			DB_TYPE(mp) = M_DATA;
			*mp->b_wptr++ = SCCP_MT_CR;
			*mp->b_wptr++ = sc->slr >> 16;
			*mp->b_wptr++ = sc->slr >> 8;
			*mp->b_wptr++ = sc->slr >> 0;
			*mp->b_wptr++ =
			    sc->cqos.protocol_class | ((sc->cqos.protocol_class < 2) ? sc->cqos.
						       return_error : 0);
			p = mp->b_wptr;
			mp->b_wptr += 2;
			*p = mp->b_wptr - p;
			p++;
			*mp->b_wptr++ = size_cpa(&sc->dst);
			len = 0;
			*mp->b_wptr = 0;
			*mp->b_wptr |= (sc->dst.pc == -1) ? 0x00 : 0x01;
			*mp->b_wptr |= (sc->dst.ssn == 0) ? 0x02 : 0x02;
			*mp->b_wptr |= (sc->dst.gtt & 0x0f) << 2;
			*mp->b_wptr |= (sc->dst.ri & 0x01) << 6;
			*mp->b_wptr |= (sc->dst.ni & 0x01) << 7;
			mp->b_wptr++;
			switch (sc->proto.pvar & SS7_PVAR_MASK) {
			case SS7_PVAR_JTTC:
				goto skip_ni;
			case SS7_PVAR_ANSI:
				if (sc->dst.ni) {

				}
			default:
			}

		}
		return (-ENOBUFS);
	}
      subsystem_failure:
	refc = SCCP_REFC_SUBSYSTEM_FAILURE;
	mi_strlog(q, STRLOGNO, SL_TRACE, "refusal: ");
	goto refusal;
      unequipped_user:
	refc = SCCP_REFC_UNEQUIPPED_USER;
	mi_strlog(q, STRLOGNO, SL_TRACE, "refusal: ");
	goto refusal;
      access_congestion:
	refc = SCCP_REFC_ACCESS_CONGESTION;
	mi_strlog(q, STRLOGNO, SL_TRACE, "refusal: ");
	goto refusal;
      sccp_failure:
	refc = SCCP_REFC_SCCP_FAILURE;
	mi_strlog(q, STRLOGNO, SL_TRACE, "refusal: ");
	goto refusal;
      destination_inaccessible:
	refc = SCCP_REFC_DESTINATION_INACCESSIBLE;
	mi_strlog(q, STRLOGNO, SL_TRACE, "refusal: ");
	goto refusal;
      destination_address_unknown:
	refc = SCCP_REFC_DESTINATION_ADDRESS_UNKNOWN;
	mi_strlog(q, STRLOGNO, SL_TRACE, "refusal: ");
	goto refusal;
      access_failure:
	refc = SCCP_REFC_ACCESS_FAILURE;
	mi_strlog(q, STRLOGNO, SL_TRACE, "refusal: ");
	goto refusal;
      refusal:
	return sc_discon_ind(sc, q, msg, N_PROVIDER, refc, NULL, NULL);
}
#endif

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  SCCP User -> SCCP Provider Primitives
 *
 *  =========================================================================
 */

/**
 * sc_conn_req: - process N_CONN_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_CONN_REQ primitive
 *
 * Launching a connection request is like a subsequent bind to a DLRN.  We select a new LRN for the
 * connection and place ourselves in the LRN hashes for the MTP provider and send a Connection
 * Request.
 */
static noinline fastcall int
sc_conn_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sc_get_state(sc) != NS_IDLE)
		goto noutstate;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto nbadopt;
		if (p->QOS_length < sizeof(sc->cqos))
			goto nbadopt;
		bcopy(mp->b_rptr + p->QOS_offset, &sc->cqos, sizeof(sc->cqos));
		if (sc->cqos.n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto nbadqostype;
		if (sc->cqos.protocol_class > 3)
			goto nbadopt;
		if (sc->cqos.protocol_class != 2 && sc->cqos.protocol_class != 3)
			goto nnotsupport;
	}
	if (!p->DEST_length)
		goto nnoaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto nbadaddr;
	if (p->DEST_length < sizeof(sc->dst))
		goto nbadaddr;
	if (p->DEST_length > sizeof(sc->dst) + SCCP_MAX_ADDR_LENGTH)
		goto nbadaddr;
	bcopy(mp->b_rptr + p->DEST_offset, &sc->dst, p->DEST_length);
	if (p->DEST_length != sizeof(sc->dst) + sc->dst.alen)
		goto nbadaddr;
	if (sc->dst.ssn == 0 && sc->cred.cr_uid != 0)
		goto naccess;
	sc->flags &= ~(SCCPF_REC_CONF_OPT | SCCPF_EX_DATA_OPT);
	if (p->CONN_flags & REC_CONF_OPT)
		sc->flags |= SCCPF_REC_CONF_OPT;
	if (p->CONN_flags & EX_DATA_OPT)
		sc->flags |= SCCPF_EX_DATA_OPT;
	if (sc->cqos.message_priority > 1 && sc->cred.cr_uid != 0)
		goto naccess;
	sc->slr = (sc->sp.sccp_next_slr++) & 0xffff;
#if 0
	if ((err = sccp_send_cr(sc, q, mp, mp->b_cont)))
		goto error;
	sc_timer_start(sc, tcon);
#endif
	sc_set_state(sc, NS_WCON_CREQ);
	return (0);
      naccess:
	err = NACCESS;
	goto error;
      nbadaddr:
	err = NBADADDR;
	goto error;
      nnoaddr:
	err = NNOADDR;
	goto error;
      nnotsupport:
	err = NNOTSUPPORT;
	goto error;
      nbadopt:
	err = NBADOPT;
	goto error;
      nbadqostype:
	err = NBADQOSTYPE;
	goto error;
      noutstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_reply(sc, q, mp, N_CONN_REQ, err);
}

/**
 * sc_conn_res: - process N_CONN_RES primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_CONN_RES primitive
 */
static noinline fastcall int
sc_conn_res(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_CONN_RES, err);
}

/**
 * sc_discon_req: - process N_DISCON_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_DISCON_REQ primitive
 */
static noinline fastcall int
sc_discon_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_DISCON_REQ, err);
}

/**
 * sc_data_req: - process N_DATA_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_DATA_REQ primitive
 */
static noinline fastcall int
sc_data_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_DATA_REQ, err);
}

/**
 * sc_exdata_req: - process N_EXDATA_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_EXDATA_REQ primitive
 */
static noinline fastcall int
sc_exdata_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_exdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_EXDATA_REQ, err);
}

/**
 * sc_info_req: - process N_INFO_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_INFO_REQ primitive
 */
static noinline fastcall int
sc_info_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_INFO_REQ, err);
}

/**
 * sc_bind_req: - process N_BIND_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_BIND_REQ primitive
 */
static noinline fastcall int
sc_bind_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_BIND_REQ, err);
}

/**
 * sc_unbind_req: - process N_UNBIND_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_UNBIND_REQ primitive
 */
static noinline fastcall int
sc_unbind_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_UNBIND_REQ, err);
}

/**
 * sc_unitdata_req: - process N_UNITDATA_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_UNITDATA_REQ primitive
 */
static noinline fastcall int
sc_unitdata_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_UNITDATA_REQ, err);
}

/**
 * sc_optmgmt_req: - process N_OPTMGMT_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_OPTMGMT_REQ primitive
 */
static noinline fastcall int
sc_optmgmt_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_OPTMGMT_REQ, err);
}

/**
 * sc_datack_req: - process N_DATACK_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_DATACK_REQ primitive
 */
static noinline fastcall int
sc_datack_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_datack_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_DATACK_REQ, err);
}

/**
 * sc_reset_req: - process N_RESET_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_RESET_REQ primitive
 */
static noinline fastcall int
sc_reset_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_RESET_REQ, err);
}

/**
 * sc_reset_res: - process N_RESET_RES primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_RESET_RES primitive
 */
static noinline fastcall int
sc_reset_res(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_RESET_RES, err);
}

/**
 * sc_inform_req: - process N_INFORM_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_INFORM_REQ primitive
 */
static noinline fastcall int
sc_inform_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_inform_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_INFORM_REQ, err);
}

/**
 * sc_coord_req: - process N_COORD_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_COORD_REQ primitive
 */
static noinline fastcall int
sc_coord_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_COORD_REQ, err);
}

/**
 * sc_coord_res: - process N_COORD_RES primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_COORD_RES primitive
 */
static noinline fastcall int
sc_coord_res(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_COORD_RES, err);
}

/**
 * sc_state_req: - process N_STATE_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_STATE_REQ primitive
 */
static noinline fastcall int
sc_state_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_state_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, N_STATE_REQ, err);
}

/**
 * sc_other_req: - process N_????_REQ primitive
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the N_????_REQ primitive
 */
static noinline fastcall int
sc_other_req(struct sc *sc, queue_t *q, mblk_t *mp)
{
	np_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return sc_error_ack(sc, q, mp, *p, err);
}

/*
 *  =========================================================================
 *
 *  MTP Provider -> MTP User Primitives
 *
 *  =========================================================================
 */

/**
 * mtp_ok_ack: - process MTP_OK_ACK primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_OK_ACK primitive
 */
static noinline fastcall int
mtp_ok_ack(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_ok_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_error_ack: - process MTP_ERROR_ACK primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_ERROR_ACK primitive
 */
static noinline fastcall int
mtp_error_ack(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_error_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_bind_ack: - process MTP_BIND_ACK primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_BIND_ACK primitive
 */
static noinline fastcall int
mtp_bind_ack(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_bind_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_addr_ack: - process MTP_ADDR_ACK primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_ADDR_ACK primitive
 */
static noinline fastcall int
mtp_addr_ack(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_addr_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_info_ack: - process MTP_INFO_ACK primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_INFO_ACK primitive
 */
static noinline fastcall int
mtp_info_ack(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_info_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_optmgmt_ack: - process MTP_OPTMGMT_ACK primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_OPTMGMT_ACK primitive
 */
static noinline fastcall int
mtp_optmgmt_ack(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_transfer_ind: - process MTP_TRANSFER_IND primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_TRANSFER_IND primitive
 */
static noinline fastcall int
mtp_transfer_ind(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_pause_ind: - process MTP_PAUSE_IND primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_PAUSE_IND primitive
 */
static noinline fastcall int
mtp_pause_ind(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_resume_ind: - process MTP_RESUME_IND primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_RESUME_IND primitive
 */
static noinline fastcall int
mtp_resume_ind(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_resume_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_status_ind: - process MTP_STATUS_IND primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_STATUS_IND primitive
 */
static noinline fastcall int
mtp_status_ind(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_status_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_restart_begins_ind: - process MTP_RESTART_BEGINS_IND primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_RESTART_BEGINS_IND primitive
 */
static noinline fastcall int
mtp_restart_begins_ind(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_restart_begins_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/**
 * mtp_restart_complete_ind: - process MTP_RESTART_COMPLETE_IND primitive
 * @mt: private structure
 * @q: active queue (read queue)
 * @mp: the MTP_RESTART_COMPLETE_IND primitive
 */
static noinline fastcall int
mtp_restart_complete_ind(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_restart_complete_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

static noinline fastcall int
mtp_other_ind(struct mt *mt, queue_t *q, mblk_t *mp)
{
	mtp_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "invalid primitive on read queue %u", (uint) * p);
	freemsg(mp);
	return (0);
      efault:
	err = EFAULT;
	goto error;
      error:
	return m_error(mt->sc, q, mp, err);
}

/*
 *  =========================================================================
 *
 *  SCCP INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
 */

static int
sc_testoption(struct sc *sc, struct sccp_option *arg)
{
	/* FIXME: check options */
	return (0);
}
static int
sc_setoption(struct sc *sc, struct sccp_option *arg)
{
	int err;

	if ((err = sc_testoption(sc, arg)) != 0)
		return (err);
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		df.option = arg->options[0].df;
		break;
	case SCCP_OBJ_TYPE_NA:
		sc->na.config = arg->options[0].na;
		break;
	case SCCP_OBJ_TYPE_SS:
		sc->ss.config = arg->options[0].ss;
		break;
	case SCCP_OBJ_TYPE_RS:
		sc->rs.config = arg->options[0].rs;
		break;
	case SCCP_OBJ_TYPE_SR:
		sc->sr.config = arg->options[0].sr;
		break;
	case SCCP_OBJ_TYPE_SP:
		sc->sp.config = arg->options[0].sp;
		break;
	case SCCP_OBJ_TYPE_MT:
		sc->mt->config = arg->options[0].mt;
		break;
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		err = EINVAL;
		break;
	}
	return (0);
}
static int
sc_testconfig(struct sc *sc, struct sccp_config *arg)
{
	/* FIXME: check configuration */
	return (0);
}
static int
sc_setconfig(struct sc *sc, struct sccp_config *arg)
{
	int err;

	if ((err = sc_testconfig(sc, arg)) != 0)
		return (err);
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		df.config = arg->config[0].df;
		break;
	case SCCP_OBJ_TYPE_NA:
		// sc->na.config = arg->config[0].na;
		break;
	case SCCP_OBJ_TYPE_SS:
		// sc->ss.config = arg->config[0].ss;
		break;
	case SCCP_OBJ_TYPE_RS:
		// sc->rs.config = arg->config[0].rs;
		break;
	case SCCP_OBJ_TYPE_SR:
		// sc->sr.config = arg->config[0].sr;
		break;
	case SCCP_OBJ_TYPE_SP:
		// sc->sp.config = arg->config[0].sp;
		break;
	case SCCP_OBJ_TYPE_MT:
		// sc->mt->config = arg->config[0].mt;
		break;
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (EINVAL);
	}
	return (0);
}
static int
sc_setstatsp(struct sc *sc, struct sccp_stats *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		df.statsp = arg->stats[0].df;
		break;
	case SCCP_OBJ_TYPE_NA:
		sc->na.statsp = arg->stats[0].na;
		break;
	case SCCP_OBJ_TYPE_SS:
		sc->ss.statsp = arg->stats[0].ss;
		break;
	case SCCP_OBJ_TYPE_RS:
		sc->rs.statsp = arg->stats[0].rs;
		break;
	case SCCP_OBJ_TYPE_SR:
		sc->sr.statsp = arg->stats[0].sr;
		break;
	case SCCP_OBJ_TYPE_SP:
		sc->sp.statsp = arg->stats[0].sp;
		break;
	case SCCP_OBJ_TYPE_MT:
		sc->mt->statsp = arg->stats[0].mt;
		break;
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (EINVAL);
	}
	return (0);
}
static int
sc_setnotify(struct sc *sc, struct sccp_notify *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		df.notify.events |= arg->notify[0].df.events;
		arg->notify[0].df.events = df.notify.events;
		break;
	case SCCP_OBJ_TYPE_NA:
		sc->na.notify.events |= arg->notify[0].na.events;
		arg->notify[0].na.events = sc->na.notify.events;
		break;
	case SCCP_OBJ_TYPE_SS:
		sc->ss.notify.events |= arg->notify[0].ss.events;
		arg->notify[0].ss.events = sc->ss.notify.events;
		break;
	case SCCP_OBJ_TYPE_RS:
		sc->rs.notify.events |= arg->notify[0].rs.events;
		arg->notify[0].rs.events = sc->rs.notify.events;
		break;
	case SCCP_OBJ_TYPE_SR:
		sc->sr.notify.events |= arg->notify[0].sr.events;
		arg->notify[0].sr.events = sc->sr.notify.events;
		break;
	case SCCP_OBJ_TYPE_SP:
		sc->sp.notify.events |= arg->notify[0].sp.events;
		arg->notify[0].sp.events = sc->sp.notify.events;
		break;
	case SCCP_OBJ_TYPE_MT:
		sc->mt->notify.events |= arg->notify[0].mt.events;
		arg->notify[0].mt.events = sc->mt->notify.events;
		break;
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (EINVAL);
	}
	return (0);
}
static int
sc_clrnotify(struct sc *sc, struct sccp_notify *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		df.notify.events &= ~arg->notify[0].df.events;
		arg->notify[0].df.events = df.notify.events;
		break;
	case SCCP_OBJ_TYPE_NA:
		sc->na.notify.events &= ~arg->notify[0].na.events;
		arg->notify[0].na.events = sc->na.notify.events;
		break;
	case SCCP_OBJ_TYPE_SS:
		sc->ss.notify.events &= ~arg->notify[0].ss.events;
		arg->notify[0].ss.events = sc->ss.notify.events;
		break;
	case SCCP_OBJ_TYPE_RS:
		sc->rs.notify.events &= ~arg->notify[0].rs.events;
		arg->notify[0].rs.events = sc->rs.notify.events;
		break;
	case SCCP_OBJ_TYPE_SR:
		sc->sr.notify.events &= ~arg->notify[0].sr.events;
		arg->notify[0].sr.events = sc->sr.notify.events;
		break;
	case SCCP_OBJ_TYPE_SP:
		sc->sp.notify.events &= ~arg->notify[0].sp.events;
		arg->notify[0].sp.events = sc->sp.notify.events;
		break;
	case SCCP_OBJ_TYPE_MT:
		sc->mt->notify.events &= ~arg->notify[0].mt.events;
		arg->notify[0].mt.events = sc->mt->notify.events;
		break;
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (EINVAL);
	}
	return (0);
}

static int
sc_size_options(struct sc *sc, struct sccp_option *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (sizeof(df.config));
	case SCCP_OBJ_TYPE_NA:
		return (sizeof(sc->na.config));
	case SCCP_OBJ_TYPE_SS:
		return (sizeof(sc->ss.config));
	case SCCP_OBJ_TYPE_RS:
		return (sizeof(sc->rs.config));
	case SCCP_OBJ_TYPE_SR:
		return (sizeof(sc->sr.config));
	case SCCP_OBJ_TYPE_SP:
		return (sizeof(sc->sp.config));
	case SCCP_OBJ_TYPE_MT:
		return (sizeof(sc->mt->config));
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (0);
	}
}
static int
sc_size_config(struct sc *sc, struct sccp_config *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (sizeof(df.config));
	case SCCP_OBJ_TYPE_NA:
		return (sizeof(sc->na.config));
	case SCCP_OBJ_TYPE_SS:
		return (sizeof(sc->ss.config));
	case SCCP_OBJ_TYPE_RS:
		return (sizeof(sc->rs.config));
	case SCCP_OBJ_TYPE_SR:
		return (sizeof(sc->sr.config));
	case SCCP_OBJ_TYPE_SP:
		return (sizeof(sc->sp.config));
	case SCCP_OBJ_TYPE_MT:
		return (sizeof(sc->mt->config));
	default:
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
		return (0);
	}
}
static int
sc_size_statem(struct sc *sc, struct sccp_statem *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (sizeof(df.statem));
	case SCCP_OBJ_TYPE_NA:
		return (sizeof(sc->na.statem));
	case SCCP_OBJ_TYPE_SS:
		return (sizeof(sc->ss.statem));
	case SCCP_OBJ_TYPE_RS:
		return (sizeof(sc->rs.statem));
	case SCCP_OBJ_TYPE_SR:
		return (sizeof(sc->sr.statem));
	case SCCP_OBJ_TYPE_SP:
		return (sizeof(sc->sp.statem));
	case SCCP_OBJ_TYPE_MT:
		return (sizeof(sc->mt->statem));
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (0);
	}
}
static int
sc_size_statsp(struct sc *sc, struct sccp_stats *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (sizeof(df.statsp));
	case SCCP_OBJ_TYPE_NA:
		return (sizeof(sc->na.statsp));
	case SCCP_OBJ_TYPE_SS:
		return (sizeof(sc->ss.statsp));
	case SCCP_OBJ_TYPE_RS:
		return (sizeof(sc->rs.statsp));
	case SCCP_OBJ_TYPE_SR:
		return (sizeof(sc->sr.statsp));
	case SCCP_OBJ_TYPE_SP:
		return (sizeof(sc->sp.statsp));
	case SCCP_OBJ_TYPE_MT:
		return (sizeof(sc->mt->statsp));
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (0);
	}
}
static int
sc_size_stats(struct sc *sc, struct sccp_stats *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (sizeof(df.stats));
	case SCCP_OBJ_TYPE_NA:
		return (sizeof(sc->na.stats));
	case SCCP_OBJ_TYPE_SS:
		return (sizeof(sc->ss.stats));
	case SCCP_OBJ_TYPE_RS:
		return (sizeof(sc->rs.stats));
	case SCCP_OBJ_TYPE_SR:
		return (sizeof(sc->sr.stats));
	case SCCP_OBJ_TYPE_SP:
		return (sizeof(sc->sp.stats));
	case SCCP_OBJ_TYPE_MT:
		return (sizeof(sc->mt->stats));
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (0);
	}
}
static int
sc_size_notify(struct sc *sc, struct sccp_notify *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (sizeof(df.notify));
	case SCCP_OBJ_TYPE_NA:
		return (sizeof(sc->na.notify));
	case SCCP_OBJ_TYPE_SS:
		return (sizeof(sc->ss.notify));
	case SCCP_OBJ_TYPE_RS:
		return (sizeof(sc->rs.notify));
	case SCCP_OBJ_TYPE_SR:
		return (sizeof(sc->sr.notify));
	case SCCP_OBJ_TYPE_SP:
		return (sizeof(sc->sp.notify));
	case SCCP_OBJ_TYPE_MT:
		return (sizeof(sc->mt->notify));
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (0);
	}
}

static void *
sc_ptr_options(struct sc *sc, struct sccp_option *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (&df.config);
	case SCCP_OBJ_TYPE_NA:
		return (&sc->na.config);
	case SCCP_OBJ_TYPE_SS:
		return (&sc->ss.config);
	case SCCP_OBJ_TYPE_RS:
		return (&sc->rs.config);
	case SCCP_OBJ_TYPE_SR:
		return (&sc->sr.config);
	case SCCP_OBJ_TYPE_SP:
		return (&sc->sp.config);
	case SCCP_OBJ_TYPE_MT:
		return (&sc->mt->config);
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (NULL);
	}
}
static void *
sc_ptr_config(struct sc *sc, struct sccp_config *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (&df.config);
	case SCCP_OBJ_TYPE_NA:
		return (&sc->na.config);
	case SCCP_OBJ_TYPE_SS:
		return (&sc->ss.config);
	case SCCP_OBJ_TYPE_RS:
		return (&sc->rs.config);
	case SCCP_OBJ_TYPE_SR:
		return (&sc->sr.config);
	case SCCP_OBJ_TYPE_SP:
		return (&sc->sp.config);
	case SCCP_OBJ_TYPE_MT:
		return (&sc->mt->config);
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (NULL);
	}
}
static void *
sc_ptr_statem(struct sc *sc, struct sccp_statem *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (&df.statem);
	case SCCP_OBJ_TYPE_NA:
		return (&sc->na.statem);
	case SCCP_OBJ_TYPE_SS:
		return (&sc->ss.statem);
	case SCCP_OBJ_TYPE_RS:
		return (&sc->rs.statem);
	case SCCP_OBJ_TYPE_SR:
		return (&sc->sr.statem);
	case SCCP_OBJ_TYPE_SP:
		return (&sc->sp.statem);
	case SCCP_OBJ_TYPE_MT:
		return (&sc->mt->statem);
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (NULL);
	}
}
static void *
sc_ptr_statsp(struct sc *sc, struct sccp_stats *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (&df.statsp);
	case SCCP_OBJ_TYPE_NA:
		return (&sc->na.statsp);
	case SCCP_OBJ_TYPE_SS:
		return (&sc->ss.statsp);
	case SCCP_OBJ_TYPE_RS:
		return (&sc->rs.statsp);
	case SCCP_OBJ_TYPE_SR:
		return (&sc->sr.statsp);
	case SCCP_OBJ_TYPE_SP:
		return (&sc->sp.statsp);
	case SCCP_OBJ_TYPE_MT:
		return (&sc->mt->statsp);
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (NULL);
	}
}
static void *
sc_ptr_stats(struct sc *sc, struct sccp_stats *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (&df.stats);
	case SCCP_OBJ_TYPE_NA:
		return (&sc->na.stats);
	case SCCP_OBJ_TYPE_SS:
		return (&sc->ss.stats);
	case SCCP_OBJ_TYPE_RS:
		return (&sc->rs.stats);
	case SCCP_OBJ_TYPE_SR:
		return (&sc->sr.stats);
	case SCCP_OBJ_TYPE_SP:
		return (&sc->sp.stats);
	case SCCP_OBJ_TYPE_MT:
		return (&sc->mt->stats);
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (NULL);
	}
}
static void *
sc_ptr_notify(struct sc *sc, struct sccp_notify *arg)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_DF:
		return (&df.notify);
	case SCCP_OBJ_TYPE_NA:
		return (&sc->na.notify);
	case SCCP_OBJ_TYPE_SS:
		return (&sc->ss.notify);
	case SCCP_OBJ_TYPE_RS:
		return (&sc->rs.notify);
	case SCCP_OBJ_TYPE_SR:
		return (&sc->sr.notify);
	case SCCP_OBJ_TYPE_SP:
		return (&sc->sp.notify);
	case SCCP_OBJ_TYPE_MT:
		return (&sc->mt->notify);
	case SCCP_OBJ_TYPE_SC:
	case SCCP_OBJ_TYPE_CP:
	default:
		return (NULL);
	}
}

/**
 * sc_ioctl: - process M_IOCTL message
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the SCCP input-output control operation.  Step 1 consists always consists of a
 * copyin operation to determine the object type and id.
 */
static __unlikely int
sc_ioctl(struct sc *sc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sc_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SCCP_IOCGOPTION):
	case _IOC_NR(SCCP_IOCSOPTION):
		mi_copyin(q, mp, NULL, sizeof(struct sccp_option));
		break;
	case _IOC_NR(SCCP_IOCGCONFIG):
	case _IOC_NR(SCCP_IOCSCONFIG):
	case _IOC_NR(SCCP_IOCTCONFIG):
	case _IOC_NR(SCCP_IOCCCONFIG):
		mi_copyin(q, mp, NULL, sizeof(struct sccp_config));
		break;
	case _IOC_NR(SCCP_IOCGSTATEM):
	case _IOC_NR(SCCP_IOCCMRESET):
		mi_copyin(q, mp, NULL, sizeof(struct sccp_statem));
		break;
	case _IOC_NR(SCCP_IOCGSTATSP):
	case _IOC_NR(SCCP_IOCSSTATSP):
	case _IOC_NR(SCCP_IOCGSTATS):
	case _IOC_NR(SCCP_IOCCSTATS):
		mi_copyin(q, mp, NULL, sizeof(struct sccp_stats));
		break;
	case _IOC_NR(SCCP_IOCGNOTIFY):
	case _IOC_NR(SCCP_IOCSNOTIFY):
	case _IOC_NR(SCCP_IOCCNOTIFY):
		mi_copyin(q, mp, NULL, sizeof(struct sccp_notify));
		break;
	case _IOC_NR(SCCP_IOCCMGMT):
		mi_copyin(q, mp, NULL, sizeof(struct sccp_mgmt));
		break;
	case _IOC_NR(SCCP_IOCCPASS):
		mi_copyin(q, mp, NULL, sizeof(struct sccp_pass));
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	return (0);
}

/**
 * sc_copyin1: - process M_IOCDATA message
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: the copyied in data
 *
 * This is step 2 of the SCCP input-output control operationl.  Step 2 consists of identifying the
 * object and performing a copyout for GET operations and an additional copyin for SET operations.
 */
static __unlikely int
sc_copyin1(struct sc *sc, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int alen, len, size = -1;
	int err = 0;
	mblk_t *bp;
	void *ptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sc_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SCCP_IOCGOPTION):
		alen = sizeof(struct sccp_option);
		len = sc_size_options(sc, (struct sccp_option *) dp->b_rptr);
		ptr = sc_ptr_options(sc, (struct sccp_option *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(SCCP_IOCSOPTION):
		alen = sizeof(struct sccp_option);
		len = sc_size_options(sc, (struct sccp_option *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(SCCP_IOCGCONFIG):
		alen = sizeof(struct sccp_config);
		len = sc_size_config(sc, (struct sccp_config *) dp->b_rptr);
		ptr = sc_ptr_config(sc, (struct sccp_config *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(SCCP_IOCSCONFIG):
	case _IOC_NR(SCCP_IOCTCONFIG):
	case _IOC_NR(SCCP_IOCCCONFIG):
		alen = sizeof(struct sccp_config);
		len = sc_size_config(sc, (struct sccp_config *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(SCCP_IOCGSTATEM):
		alen = sizeof(struct sccp_statem);
		len = sc_size_statem(sc, (struct sccp_statem *) dp->b_rptr);
		ptr = sc_ptr_statem(sc, (struct sccp_statem *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(SCCP_IOCCMRESET):
		size = 0;
		switch (((struct sccp_statem *) dp->b_rptr)->type) {
		case SCCP_OBJ_TYPE_DF:
		case SCCP_OBJ_TYPE_NA:
		case SCCP_OBJ_TYPE_SS:
		case SCCP_OBJ_TYPE_RS:
		case SCCP_OBJ_TYPE_SR:
		case SCCP_OBJ_TYPE_SP:
		case SCCP_OBJ_TYPE_MT:
			/* FIXME: reset state machine */
			break;
		case SCCP_OBJ_TYPE_SC:
		case SCCP_OBJ_TYPE_CP:
		default:
			err = EINVAL;
			break;
		}
		break;
	case _IOC_NR(SCCP_IOCGSTATSP):
		alen = sizeof(struct sccp_stats);
		len = sc_size_statsp(sc, (struct sccp_stats *) dp->b_rptr);
		ptr = sc_ptr_statsp(sc, (struct sccp_stats *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(SCCP_IOCSSTATSP):
		alen = sizeof(struct sccp_stats);
		len = sc_size_statsp(sc, (struct sccp_stats *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(SCCP_IOCGSTATS):
		alen = sizeof(struct sccp_stats);
		len = sc_size_stats(sc, (struct sccp_stats *) dp->b_rptr);
		ptr = sc_ptr_stats(sc, (struct sccp_stats *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(SCCP_IOCCSTATS):
		alen = sizeof(struct sccp_stats);
		len = sc_size_stats(sc, (struct sccp_stats *) dp->b_rptr);
		ptr = sc_ptr_stats(sc, (struct sccp_stats *) dp->b_rptr);
		goto copyout_clear;
	case _IOC_NR(SCCP_IOCGNOTIFY):
		alen = sizeof(struct sccp_notify);
		len = sc_size_notify(sc, (struct sccp_notify *) dp->b_rptr);
		ptr = sc_ptr_notify(sc, (struct sccp_notify *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(SCCP_IOCSNOTIFY):
	case _IOC_NR(SCCP_IOCCNOTIFY):
		alen = sizeof(struct sccp_notify);
		len = sc_size_notify(sc, (struct sccp_notify *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(SCCP_IOCCMGMT):
		size = 0;
		switch (((struct sccp_mgmt *) dp->b_rptr)->type) {
		case SCCP_OBJ_TYPE_DF:
		case SCCP_OBJ_TYPE_NA:
		case SCCP_OBJ_TYPE_SS:
		case SCCP_OBJ_TYPE_RS:
		case SCCP_OBJ_TYPE_SR:
		case SCCP_OBJ_TYPE_SP:
		case SCCP_OBJ_TYPE_MT:
			/* FIXME: perform command */
			break;
		case SCCP_OBJ_TYPE_SC:
		case SCCP_OBJ_TYPE_CP:
		default:
			err = EINVAL;
			break;
		}
		break;
	case _IOC_NR(SCCP_IOCCPASS):
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
 * sc_copyin2: - process M_IOCDATA message
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: the copyied in data
 *
 * This is step 3 for SET operations.  This is the result of the implicit or explicit copyin
 * operation for the object specific structure.  We can now perform sets and commits.  All SET
 * operations also include a last copyout step that copies out the information actually set.
 */
static __unlikely int
sc_copyin2(struct sc *sc, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int len, err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sc_iocname(cp->cp_cmd));

	len = dp->b_wptr - dp->b_rptr;
	if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, len);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SCCP_IOCSOPTION):
		err = sc_setoption(sc, (struct sccp_option *) bp->b_rptr);
		break;
	case _IOC_NR(SCCP_IOCSCONFIG):
		err = sc_setconfig(sc, (struct sccp_config *) bp->b_rptr);
		break;
	case _IOC_NR(SCCP_IOCTCONFIG):
		err = sc_testconfig(sc, (struct sccp_config *) bp->b_rptr);
		break;
	case _IOC_NR(SCCP_IOCCCONFIG):
		err = sc_setconfig(sc, (struct sccp_config *) bp->b_rptr);
		break;
	case _IOC_NR(SCCP_IOCSSTATSP):
		err = sc_setstatsp(sc, (struct sccp_stats *) bp->b_rptr);
		break;
	case _IOC_NR(SCCP_IOCSNOTIFY):
		err = sc_setnotify(sc, (struct sccp_notify *) dp->b_rptr);
		break;
	case _IOC_NR(SCCP_IOCCNOTIFY):
		err = sc_clrnotify(sc, (struct sccp_notify *) dp->b_rptr);
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
 * sc_copyout: - process M_IOCDATA message
 * @sc: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 *
 * This is the final step which is a simple copy done operation.
 */
static __unlikely int
sc_copyout(struct sc *sc, queue_t *q, mblk_t *mp)
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
 * sc_m_data: - process M_DATA message
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 */
static noinline fastcall int
sc_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * sc_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static noinline fastcall int
sc_m_proto(queue_t *q, mblk_t *mp)
{
	np_ulong prim;
	struct sc *sc;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(np_ulong *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	sc = SC_PRIV(q);
	sc_save_state(sc);
	mt_save_state(sc->mt);
	switch (prim) {
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_DATACK_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> %s", sc_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", sc_primname(prim));
		break;
	}
	switch (prim) {
	case N_CONN_REQ:
		rtn = sc_conn_req(sc, q, mp);
		break;
	case N_CONN_RES:
		rtn = sc_conn_res(sc, q, mp);
		break;
	case N_DISCON_REQ:
		rtn = sc_discon_req(sc, q, mp);
		break;
	case N_DATA_REQ:
		rtn = sc_data_req(sc, q, mp);
		break;
	case N_EXDATA_REQ:
		rtn = sc_exdata_req(sc, q, mp);
		break;
	case N_INFO_REQ:
		rtn = sc_info_req(sc, q, mp);
		break;
	case N_BIND_REQ:
		rtn = sc_bind_req(sc, q, mp);
		break;
	case N_UNBIND_REQ:
		rtn = sc_unbind_req(sc, q, mp);
		break;
	case N_UNITDATA_REQ:
		rtn = sc_unitdata_req(sc, q, mp);
		break;
	case N_OPTMGMT_REQ:
		rtn = sc_optmgmt_req(sc, q, mp);
		break;
	case N_DATACK_REQ:
		rtn = sc_datack_req(sc, q, mp);
		break;
	case N_RESET_REQ:
		rtn = sc_reset_req(sc, q, mp);
		break;
	case N_RESET_RES:
		rtn = sc_reset_res(sc, q, mp);
		break;
	case N_INFORM_REQ:
		rtn = sc_inform_req(sc, q, mp);
		break;
	case N_COORD_REQ:
		rtn = sc_coord_req(sc, q, mp);
		break;
	case N_COORD_RES:
		rtn = sc_coord_res(sc, q, mp);
		break;
	case N_STATE_REQ:
		rtn = sc_state_req(sc, q, mp);
		break;
	default:
		rtn = sc_other_req(sc, q, mp);
		break;
	}
	if (rtn) {
		mt_restore_state(sc->mt);
		sc_restore_state(sc);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * sc_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (write queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static noinline fastcall int
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
	mt_save_state(sc->mt);
	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = sc_t1_timeout(sc, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding undefined timeout %d",
			  __FUNCTION__, *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		mt_restore_state(sc->mt);
		sc_restore_state(sc);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * sc_m_ioctl: - process M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static noinline fastcall int
sc_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct sc *sc;
	caddr_t priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != SCCP_IOC_MAGIC) {
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
	sc = SC_PRIV(q);
	err = sc_ioctl(sc, q, mp);
	mi_unlock(priv);
	return (err);
}

/**
 * sc_m_iocdata: - process M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static noinline fastcall int
sc_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct sc *sc;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != SCCP_IOC_MAGIC) {
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
	sc = SC_PRIV(q);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = sc_copyin1(sc, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		err = sc_copyin2(sc, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = sc_copyout(sc, q, mp);
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
 * sc_m_flush: - process M_FLUSH message
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 *
 * Canonical module flush procedure for the write queue.
 */
static noinline fastcall int
sc_m_flush(queue_t *q, mblk_t *mp)
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
 * sc_m_other: - process unrecognized STREAMS message
 * @q: active queue (write queue)
 * @mp: the unrecognized STREAMS message
 *
 * Unrecognized STREAMS messages are passed to the next module considering flow control.
 */
static noinline fastcall int
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
	case M_IOCTL:
		return sc_m_ioctl(q, mp);
	case M_IOCDATA:
		return sc_m_iocdata(q, mp);
	case M_FLUSH:
		return sc_m_flush(q, mp);
	default:
		return sc_m_other(q, mp);
	}
}

/**
 * mt_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static noinline fastcall int
mt_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * mt_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static noinline fastcall int
mt_m_proto(queue_t *q, mblk_t *mp)
{
	mtp_ulong prim;
	struct mt *mt;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(mtp_ulong *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	mt = MT_PRIV(q);
	mt_save_state(mt);
	sc_save_state(mt->sc);
	switch (prim) {
	case MTP_TRANSFER_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s <-", mt_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", mt_primname(prim));
		break;
	}
	switch (prim) {
	case MTP_OK_ACK:
		rtn = mtp_ok_ack(mt, q, mp);
		break;
	case MTP_ERROR_ACK:
		rtn = mtp_error_ack(mt, q, mp);
		break;
	case MTP_BIND_ACK:
		rtn = mtp_bind_ack(mt, q, mp);
		break;
	case MTP_ADDR_ACK:
		rtn = mtp_addr_ack(mt, q, mp);
		break;
	case MTP_INFO_ACK:
		rtn = mtp_info_ack(mt, q, mp);
		break;
	case MTP_OPTMGMT_ACK:
		rtn = mtp_optmgmt_ack(mt, q, mp);
		break;
	case MTP_TRANSFER_IND:
		rtn = mtp_transfer_ind(mt, q, mp);
		break;
	case MTP_PAUSE_IND:
		rtn = mtp_pause_ind(mt, q, mp);
		break;
	case MTP_RESUME_IND:
		rtn = mtp_resume_ind(mt, q, mp);
		break;
	case MTP_STATUS_IND:
		rtn = mtp_status_ind(mt, q, mp);
		break;
	case MTP_RESTART_BEGINS_IND:
		rtn = mtp_restart_begins_ind(mt, q, mp);
		break;
	case MTP_RESTART_COMPLETE_IND:
		rtn = mtp_restart_complete_ind(mt, q, mp);
		break;
	default:
		rtn = mtp_other_ind(mt, q, mp);
		break;
	}
	if (rtn) {
		sc_restore_state(mt->sc);
		mt_restore_state(mt);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * mt_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (read queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static noinline fastcall int
mt_m_sig(queue_t *q, mblk_t *mp)
{
	struct mt *mt;
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);
	if (!mi_timer_valid(mp))
		return (0);
	mt = MT_PRIV(q);
	mt_save_state(mt);
	sc_save_state(mt->sc);
	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = mt_t1_timeout(mt, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding undefined timeout %d",
			  __FUNCTION__, *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		sc_restore_state(mt->sc);
		mt_restore_state(mt);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * mt_m_copy: - process M_COPYIN or M_COPYOUT message
 * @q: active queue (read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 */
static noinline fastcall int
mt_m_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * mt_m_iocack: - process M_IOCACK or M_IOCNAK message
 * @q: active queue (read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 */
static noinline fastcall int
mt_m_iocack(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * mt_m_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 *
 * Canonical module flush procedure for the read queue.
 */
static noinline fastcall int
mt_m_flush(queue_t *q, mblk_t *mp)
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
 * mt_m_other: - process unrecognized STREAMS message
 * @q: active queue (read queue)
 * @mp: the unrecognized STREAMS message
 *
 * Unrecognized STREAMS messages are passed to the next module considering flow control.
 */
static noinline fastcall int
mt_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastcall int
mt_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mt_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mt_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mt_m_sig(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return mt_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return mt_m_iocack(q, mp);
	case M_FLUSH:
		return mt_m_flush(q, mp);
	default:
		return mt_m_other(q, mp);
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
sc_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sc_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
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
static streamscall __hot_read int
mt_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mt_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_in int
mt_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mt_msg(q, mp))
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

static caddr_t sc_opens = NULL;

static streamscall int
sc_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(N_info_req_t), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&sc_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	p = PRIV(q);

	/* initialize private structure */
	p->r_priv.priv = p;
	p->r_priv.sc = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state = MTPS_UNUSABLE;
	p->r_priv.oldstate = MTPS_UNUSABLE;
	/* FIXME initialize some more */

	p->w_priv.priv = p;
	p->w_priv.mt = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state = NS_NOSTATES;
	p->w_priv.oldstate = NS_NOSTATES;
	/* FIXME initialize some more */

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((N_info_req_t *) mp->b_wptr)->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(N_info_req_t);

	qprocson(q);
	qreply(q, mp);
	return (0);
}

static streamscall int
sc_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	(void) p;
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm(&sc_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit sc_rinit = {
	.qi_putp = mt_put,
	.qi_srvp = mt_srv,
	.qi_qopen = sc_qopen,
	.qi_qclose = sc_qclose,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_rstat,
};

static struct qinit sc_winit = {
	.qi_putp = sc_put,
	.qi_srvp = sc_srv,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_wstat,
};

static struct streamtab sccp_modinfo = {
	.st_rdinit = &sc_rinit,
	.st_wrinit = &sc_winit,
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
MODULE_PARM_DESC(modid, "Module id for SCCP-MOD module.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw sc_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sccp_modinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
sccp_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&sc_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
sccp_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&sc_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(sccp_modinit);
module_exit(sccp_modexit);
