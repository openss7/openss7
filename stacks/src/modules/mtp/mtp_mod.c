/*****************************************************************************

 @(#) $RCSfile: mtp_mod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/03 13:02:50 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/08/03 13:02:50 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp_mod.c,v $
 Revision 0.9.2.1  2007/08/03 13:02:50  brian
 - added documentation and minimal modules

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp_mod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/03 13:02:50 $"

static char const ident[] = "$RCSfile: mtp_mod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/03 13:02:50 $";

/*
 * MTP-MOD is a minimal MTP in the spirit of Q.710 but which also supports ANSI and other variants.
 * It is pushed directly over an SL Stream to form a minimal MTP.  The signalling link provided by
 * the SL Stream is an F-link consisting of a single link in a single linkset.
 *
 * Usage:
 *
 *  - open an SL stream
 *  - attach the SL Stream
 *  - enable the SL Stream
 *  - push the MTP-MOD module
 *  - configure the MTP-MOD Stream
 *  - bind the MTP-MOD Stream
 *  - exchange MTP data
 *  - unbind the MTP-MOD Stream
 *  - pop the MTP-MOD module
 *  - disable the SL Stream
 *  - detach the SL Stream
 *  - close the SL Stream
 */

#define _MPS_SOURCE	1
#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#define MT_DESCRIP	"SS7/MTP (Minimal MTP) STREAMS MODULE."
#define MT_REVISION	"OpenSS7 $RCSfile: mtp_mod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/03 13:02:50 $"
#define MT_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define MT_DEVICE	"Provides OpenSS7 MTP Minimal Module."
#define MT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MT_LICENSE	"GPL v2"
#define MT_BANNER	MT_DESCRIP	"\n" \
			MT_REVISION	"\n" \
			MT_COPYRIGHT	"\n" \
			MT_DEVICE	"\n" \
			MT_CONTACT	"\n"
#define MT_SPLASH	MT_DEVICE	" - " \
			MT_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(MT_CONTACT);
MODULE_DESCRIPTION(MT_DESCRIP);
MODULE_SUPPORTED_DEVICE(MT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MT_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mtp-mod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#endif				/* LFS */

#ifndef MTP_MOD_MOD_NAME
#define MTP_MOD_MOD_NAME "mtp-mod"
#endif				/* MTP_MOD_MOD_NAME */

#ifndef MTP_MOD_MOD_ID
#define MTP_MOD_MOD_ID	0
#endif				/* MTP_MOD_MOD_ID */

#define MOD_ID		MTP_MOD_MOD_ID
#define MOD_NAME	MTP_MOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MT_BANNER
#else				/* MODULE */
#define MOD_BANNER	MT_SPLASH
#endif				/* MODULE */

/*
 *  =========================================================================
 *
 *  OPTIONS
 *
 *  =========================================================================
 */

struct mt_options {
};

struct sl_options {
};

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

struct priv;
struct sl;
struct mt;

struct {
	struct mtp_opt_conf_df option;
	struct mtp_conf_df config;
	struct mtp_statem_df statem;
	struct mtp_stats_df statsp;
	struct mtp_stats_df stats;
	struct mtp_notify_df notify;
} df;

struct mt {
	struct priv *priv;
	struct sl *sl;
	queue_t *oq;
	mtp_ulong state;
	mtp_ulong oldstate;
	mtp_ulong style;
	uint loc_len;
	struct mtp_addr loc;
	uint rem_len;
	struct mtp_addr rem;
	struct {
		struct mtp_opt_conf_na option;
		struct mtp_conf_na config;
		struct mtp_statem_na statem;
		struct mtp_stats_na statsp;
		struct mtp_stats_na stats;
		struct mtp_notify_na notify;
	} na;
	struct {
		struct mtp_opt_conf_sp option;
		struct mtp_conf_sp config;
		struct mtp_statem_sp statem;
		struct mtp_stats_sp statsp;
		struct mtp_stats_sp stats;
		struct mtp_notify_sp notify;
	} sp;
	struct {
		struct mtp_opt_conf_rs option;
		struct mtp_conf_rs config;
		struct mtp_statem_rs statem;
		struct mtp_stats_rs statsp;
		struct mtp_stats_rs stats;
		struct mtp_notify_rs notify;
	} rs;
	struct {
		struct mtp_opt_conf_rl option;
		struct mtp_conf_rl config;
		struct mtp_statem_rl statem;
		struct mtp_stats_rl statsp;
		struct mtp_stats_rl stats;
		struct mtp_notify_rl notify;
	} rl;
	struct {
		struct mtp_opt_conf_rt option;
		struct mtp_conf_rt config;
		struct mtp_statem_rt statem;
		struct mtp_stats_rt statsp;
		struct mtp_stats_rt stats;
		struct mtp_notify_rt notify;
	} rt;
	struct {
		struct mtp_opt_conf_ls option;
		struct mtp_conf_ls config;
		struct mtp_statem_ls statem;
		struct mtp_stats_ls statsp;
		struct mtp_stats_ls stats;
		struct mtp_notify_ls notify;
	} ls;
	struct {
		struct mtp_opt_conf_lk option;
		struct mtp_conf_lk config;
		struct mtp_statem_lk statem;
		struct mtp_stats_lk statsp;
		struct mtp_stats_lk stats;
		struct mtp_notify_lk notify;
	} lk;
};

struct sl {
	struct priv *priv;
	struct mt *mt;
	queue_t *oq;
	sl_ulong state;
	sl_ulong oldstate;
	struct mtp_opt_conf_sl option;
	struct mtp_conf_sl config;
	struct mtp_statem_sl statem;
	struct mtp_stats_sl statsp;
	struct mtp_stats_sl stats;
	struct mtp_notify_sl notify;
};

struct priv {
	struct sl r_priv;		/* SL structure on RD side */
	struct mt w_priv;		/* MT structure on WR side */
};

#define PRIV(q) ((struct priv *)(q)->q_ptr)

#define MT_PRIV(q) (&(PRIV(q)->w_priv))
#define SL_PRIV(q) (&(PRIV(q)->r_priv))

static kmem_cachep_t mtp_mt_cachep = NULL;	/* MTP User cache */
static kmem_cachep_t mtp_sl_cachep = NULL;	/* SL Provider cache */

#define STRLOGIO	0	/* log Stream input-output operations */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

static inline const char *
mt_iocname(int cmd)
{
	switch (cmd) {
	case MTP_IOCGOPTION:
		return ("MTP_IOCGOPTION");
	case MTP_IOCSOPTION:
		return ("MTP_IOCSOPTION");
	case MTP_IOCGCONFIG:
		return ("MTP_IOCGCONFIG");
	case MTP_IOCSCONFIG:
		return ("MTP_IOCSCONFIG");
	case MTP_IOCTCONFIG:
		return ("MTP_IOCTCONFIG");
	case MTP_IOCCCONFIG:
		return ("MTP_IOCCCONFIG");
	case MTP_IOCGSTATEM:
		return ("MTP_IOCGSTATEM");
	case MTP_IOCCMRESET:
		return ("MTP_IOCCMRESET");
	case MTP_IOCGSTATSP:
		return ("MTP_IOCGSTATSP");
	case MTP_IOCSSTATSP:
		return ("MTP_IOCSSTATSP");
	case MTP_IOCGSTATS:
		return ("MTP_IOCGSTATS");
	case MTP_IOCCSTATS:
		return ("MTP_IOCCSTATS");
	case MTP_IOCGNOTIFY:
		return ("MTP_IOCGNOTIFY");
	case MTP_IOCSNOTIFY:
		return ("MTP_IOCSNOTIFY");
	case MTP_IOCCNOTIFY:
		return ("MTP_IOCCNOTIFY");
	case MTP_IOCCMGMT:
		return ("MTP_IOCCMGMT");
	case MTP_IOCCPASS:
		return ("MTP_IOCCPASS");
	default:
		return ("(unknown)");
	}
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
mt_get_style(struct mt *mt)
{
	return (mt->style);
}

static mtp_ulong
mt_set_state(struct mt *mt, mtp_ulong newstate)
{
	mtp_ulong oldstate = mt->state;

	if (newstate != oldstate) {
		mt->state = newstate;
		mi_strlog(mt->oq, STRLOGST, SL_TRACE, "%s <- %s", mt_statename(newstate),
			  mt_statename(oldstate));
	}
	return (newstate);
}

static mtp_ulong
mt_save_state(struct mt *mt)
{
	return ((mt->oldstate = mt_get_state(mt)));
}

static mtp_ulong
mt_restore_state(struct mt *mt)
{
	return (mt_set_state(mt, mt->oldstate));
}

static inline const char *
sl_primname(sl_ulong prim)
{
	switch (prim) {
	case LMI_INFO_REQ:
		return ("LMI_INFO_REQ");
	case LMI_ATTACH_REQ:
		return ("LMI_ATTACH_REQ");
	case LMI_DETACH_REQ:
		return ("LMI_DETACH_REQ");
	case LMI_ENABLE_REQ:
		return ("LMI_ENABLE_REQ");
	case LMI_DISABLE_REQ:
		return ("LMI_DISABLE_REQ");
	case LMI_OPTMGMT_REQ:
		return ("LMI_OPTMGMT_REQ");
	case LMI_INFO_ACK:
		return ("LMI_INFO_ACK");
	case LMI_OK_ACK:
		return ("LMI_OK_ACK");
	case LMI_ERROR_ACK:
		return ("LMI_ERROR_ACK");
	case LMI_ENABLE_CON:
		return ("LMI_ENABLE_CON");
	case LMI_DISABLE_CON:
		return ("LMI_DISABLE_CON");
	case LMI_OPTMGMT_ACK:
		return ("LMI_OPTMGMT_ACK");
	case LMI_ERROR_IND:
		return ("LMI_ERROR_IND");
	case LMI_STATS_IND:
		return ("LMI_STATS_IND");
	case LMI_EVENT_IND:
		return ("LMI_EVENT_IND");
	case SL_PDU_REQ:
		return ("SL_PDU_REQ");
	case SL_EMERGENCY_REQ:
		return ("SL_EMERGENCY_REQ");
	case SL_EMERGENCY_CEASES_REQ:
		return ("SL_EMERGENCY_CEASES_REQ");
	case SL_START_REQ:
		return ("SL_START_REQ");
	case SL_STOP_REQ:
		return ("SL_STOP_REQ");
	case SL_RETRIEVE_BSNT_REQ:
		return ("SL_RETRIEVE_BSNT_REQ");
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		return ("SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
	case SL_CLEAR_BUFFERS_REQ:
		return ("SL_CLEAR_BUFFERS_REQ");
	case SL_CLEAR_RTB_REQ:
		return ("SL_CLEAR_RTB_REQ");
	case SL_CONTINUE_REQ:
		return ("SL_CONTINUE_REQ");
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		return ("SL_LOCAL_PROCESSOR_OUTAGE_REQ");
	case SL_RESUME_REQ:
		return ("SL_RESUME_REQ");
	case SL_CONGESTION_DISCARD_REQ:
		return ("SL_CONGESTION_DISCARD_REQ");
	case SL_CONGESTION_ACCEPT_REQ:
		return ("SL_CONGESTION_ACCEPT_REQ");
	case SL_NO_CONGESTION_REQ:
		return ("SL_NO_CONGESTION_REQ");
	case SL_POWER_ON_REQ:
		return ("SL_POWER_ON_REQ");
	case SL_OPTMGMT_REQ:
		return ("SL_OPTMGMT_REQ");
	case SL_NOTIFY_REQ:
		return ("SL_NOTIFY_REQ");
	case SL_PDU_IND:
		return ("SL_PDU_IND");
	case SL_LINK_CONGESTED_IND:
		return ("SL_LINK_CONGESTED_IND");
	case SL_LINK_CONGESTION_CEASED_IND:
		return ("SL_LINK_CONGESTION_CEASED_IND");
	case SL_RETRIEVED_MESSAGE_IND:
		return ("SL_RETRIEVED_MESSAGE_IND");
	case SL_RETRIEVAL_COMPLETE_IND:
		return ("SL_RETRIEVAL_COMPLETE_IND");
	case SL_RB_CLEARED_IND:
		return ("SL_RB_CLEARED_IND");
	case SL_BSNT_IND:
		return ("SL_BSNT_IND");
	case SL_IN_SERVICE_IND:
		return ("SL_IN_SERVICE_IND");
	case SL_OUT_OF_SERVICE_IND:
		return ("SL_OUT_OF_SERVICE_IND");
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		return ("SL_REMOTE_PROCESSOR_OUTAGE_IND");
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		return ("SL_REMOTE_PROCESSOR_RECOVERED_IND");
	case SL_RTB_CLEARED_IND:
		return ("SL_RTB_CLEARED_IND");
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		return ("SL_RETRIEVAL_NOT_POSSIBLE_IND");
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		return ("SL_BSNT_NOT_RETRIEVABLE_IND");
	case SL_OPTMGMT_ACK:
		return ("SL_OPTMGMT_ACK");
	case SL_NOTIFY_IND:
		return ("SL_NOTIFY_IND");
	case SL_LOCAL_PROCESSOR_OUTAGE_IND:
		return ("SL_LOCAL_PROCESSOR_OUTAGE_IND");
	case SL_LOCAL_PROCESSOR_RECOVERED_IND:
		return ("SL_LOCAL_PROCESSOR_RECOVERED_IND");
	default:
		return ("(unknown)");
	}
}
static inline const char *
sl_statename(sl_ulong state)
{
	switch (state) {
	case LMI_UNATTACHED:
		return ("LMI_UNATTACHED");
	case LMI_ATTACH_PENDING:
		return ("LMI_ATTACH_PENDING");
	case LMI_UNUSABLE:
		return ("LMI_UNUSABLE");
	case LMI_DISABLED:
		return ("LMI_DISABLED");
	case LMI_ENABLE_PENDING:
		return ("LMI_ENABLE_PENDING");
	case LMI_ENABLED:
		return ("LMI_ENABLED");
	case LMI_DISABLE_PENDING:
		return ("LMI_DISABLE_PENDING");
	case LMI_DETACH_PENDING:
		return ("LMI_DETACH_PENDING");
	default:
		return ("(unknown)");
	}
}

static sl_ulong
sl_get_state(struct sl *sl)
{
	return (sl->state);
}

static sl_ulong
sl_set_state(struct sl *sl, sl_ulong newstate)
{
	sl_ulong oldstate = sl->state;

	if (newstate != oldstate) {
		sl->state = newstate;
		mi_strlog(sl->oq, STRLOGST, SL_TRACE, "%s <- %s", sl_statename(newstate),
			  sl_statename(oldstate));
	}
	return (newstate);
}

static sl_ulong
sl_save_state(struct sl *sl)
{
	return ((sl->oldstate = sl_get_state(sl)));
}

static sl_ulong
sl_restore_state(struct sl *sl)
{
	return (sl_set_state(sl, sl->oldstate));
}

/*
 *  =========================================================================
 *
 *  OPTIONS HANDLING
 *
 *  =========================================================================
 */

static uint
mtp_size_options(struct mt *mt, queue_t *q, mtp_ulong flags, struct mt_options *opts)
{
	return (0);
}

static void
mtp_build_options(struct mt *mt, queue_t *q, mtp_ulong flags, struct mt_options *opts, mblk_t *mp)
{
	return;
}

static uint
sl_size_options(struct sl *sl, queue_t *q, sl_ulong flags, struct sl_options *opts)
{
	return (0);
}

static void
sl_build_options(struct sl *sl, queue_t *q, sl_ulong flags, struct sl_options *opts, mblk_t *mp)
{
	return;
}


/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  =========================================================================
 */

/**
 * mtp_flush: - issue M_FLUSH message
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: flush flags
 * @band: flush band
 */
static int
mtp_flush(struct mt *mt, queue_t *q, mblk_t *msg, int flags, int band)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_FLUSH;
		*mp->b_wptr++ = flags | (band ? FLUSHBAND : 0);
		*mp->b_wptr++ = band;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_FLUSH");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_error: - issue M_ERROR message
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @error: error to indicate
 */
static int
m_error(struct mt *mt, queue_t *q, mblk_t *msg, int error)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = error;
		*mp->b_wptr++ = error;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  MTP Provider -> MTP User Primitives
 *
 *  =========================================================================
 */

/**
 * mtp_ok_ack: - issue MTP_OK_ACK primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
mtp_ok_ack(struct mt *mt, queue_t *q, mblk_t *msg, mtp_ulong prim)
{
	struct MTP_ok_ack *p;
	mblk_t *mp;

	(void) mtp_ok_ack;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OK_ACK;
		p->mtp_correct_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_OK_ACK");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * mtp_error_ack: - issue MTP_ERROR_ACK primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error number
 */
static int
mtp_error_ack(struct mt *mt, queue_t *q, mblk_t *msg, mtp_ulong prim, mtp_long error)
{
	struct MTP_error_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ERROR_ACK;
		p->mtp_error_primitive = prim;
		p->mtp_mtpi_error = error < 0 ? MSYSERR : error;
		p->mtp_unix_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_ERROR_ACK");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_bind_ack: - issue MTP_BIND_ACK primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: pointer to bound MTP address
 */
static int
mtp_bind_ack(struct mt *mt, queue_t *q, mblk_t *msg, struct mtp_addr *add)
{
	struct MTP_bind_ack *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	(void) mtp_bind_ack;
	if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_BIND_ACK;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(add, mp->b_wptr, add_len);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTB_BIND_ACK");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_addr_ack: - issue MTP_ADDR_ACK primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
mtp_addr_ack(struct mt *mt, queue_t *q, mblk_t *msg)
{
	struct MTP_addr_ack *p;
	mblk_t *mp;

	size_t loc_len = mt->loc_len;
	size_t rem_len = mt->rem_len;
	size_t msg_len = sizeof(*p) + loc_len + rem_len;

	(void) mtp_addr_ack;
	if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_ACK;
		p->mtp_loc_length = loc_len;
		p->mtp_loc_offset = sizeof(*p);
		p->mtp_rem_length = rem_len;
		p->mtp_rem_offset = sizeof(*p) + loc_len;
		mp->b_wptr += sizeof(*p);
		bcopy(&mt->loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
		bcopy(&mt->rem, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_ADDR_ACK");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_info_ack: - issue MTP_INFO_ACK primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
mtp_info_ack(struct mt *mt, queue_t *q, mblk_t *msg)
{
	struct MTP_info_ack *p;
	mblk_t *mp;

	size_t loc_len = mt->loc_len;
	size_t msg_len = sizeof(*p) + loc_len;

	(void) mtp_info_ack;
	if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_msu_size = 272;
		p->mtp_addr_size = sizeof(mt->loc);
		p->mtp_addr_length = loc_len;
		p->mtp_addr_offset = sizeof(*p);
		p->mtp_current_state = mt_get_state(mt);
		p->mtp_serv_type = mt_get_style(mt);
		p->mtp_version = MTP_CURRENT_VERSION;
		mp->b_wptr += sizeof(*p);
		bcopy(&mt->loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_INFO_ACK");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_ack: - issue MTP_OPTMGMT_ACK primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: options management flags
 * @opts: options structure
 */
static int
mtp_optmgmt_ack(struct mt *mt, queue_t *q, mblk_t *msg, mtp_ulong flags, struct mt_options *opts)
{
	struct MTP_optmgmt_ack *p;
	mblk_t *mp;

	size_t opt_len = mtp_size_options(mt, q, flags, opts);
	size_t msg_len = sizeof(*p) + opt_len;

	(void) mtp_optmgmt_ack;
	if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		mtp_build_options(mt, q, flags, opts, mp);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_OPTMGMT_ACK");
		putnext(mt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_trasnfer_ind: - issue MTP_TRANSFER_IND primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @src: source address pointer
 * @mpri: message priority
 * @sls: signalling link selection code
 * @dp: user data
 */
static int
mtp_transfer_ind(struct mt *mt, queue_t *q, mblk_t *msg, struct mtp_addr *src, mtp_ulong mpri,
		 mtp_ulong sls, mblk_t *dp)
{
	struct MTP_transfer_ind *p;
	mblk_t *mp;

	size_t src_len = src ? sizeof(*src) : 0;
	size_t msg_len = sizeof(*p) + src_len;

	(void) mtp_transfer_ind;
	if (canputnext(mt->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_TRANSFER_IND;
			p->mtp_srce_length = src_len;
			p->mtp_srce_offset = sizeof(*p);
			p->mtp_mp = mpri;
			p->mtp_sls = sls;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- MTP_TRANSFER_IND");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mtp_pause_ind: - issue MTP_PAUSE_IND primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: address that has paused
 */
static int
mtp_pause_ind(struct mt *mt, queue_t *q, mblk_t *msg, struct mtp_addr *add)
{
	struct MTP_pause_ind *p;
	mblk_t *mp;

	size_t add_len = sizeof(*add);
	size_t msg_len = sizeof(*p) + add_len;

	(void) mtp_pause_ind;
	if (bcanputnext(mt->oq, 1)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_PAUSE_IND;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_PAUSE_IND");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mtp_resume_ind: - issue MTP_RESUME_IND primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: MTP address that has resumed
 */
static int
mtp_resume_ind(struct mt *mt, queue_t *q, mblk_t *msg, struct mtp_addr *add)
{
	struct MTP_resume_ind *p;
	mblk_t *mp;

	size_t add_len = sizeof(*add);
	size_t msg_len = sizeof(*p) + add_len;

	(void) mtp_resume_ind;
	if (bcanputnext(mt->oq, 1)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESUME_IND;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_RESUME_IND");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mtp_status_ind: - issue MTP_STATUS_IND primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: MTP address for status
 * @type: status type
 * @status: status indication
 */
static int
mtp_status_ind(struct mt *mt, queue_t *q, mblk_t *msg, struct mtp_addr *add, mtp_ulong type,
	       mtp_ulong status)
{
	struct MTP_status_ind *p;
	mblk_t *mp;

	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	(void) mtp_status_ind;
	if (bcanputnext(mt->oq, 1)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_STATUS_IND;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = sizeof(*p);
			p->mtp_type = type;
			p->mtp_status = status;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_STATUS_IND");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mtp_restart_begins_ind: - issue MTP_RESTART_BEGINS_IND primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * All status indications are generated on band 1.  When MTP restart begins
 * all oustanding pause, resume, status indications are flushed.
 */
static int
mtp_restart_begins_ind(struct mt *mt, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_begins_ind *p;
	mblk_t *mp;
	int err;

	(void) mtp_restart_begins_ind;
	if (!(err = mtp_flush(mt, q, NULL, FLUSHRW, 1))) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESTART_BEGINS_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_RESTART_BEGINS_IND");
			putnext(mt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (err);
}

/**
 * mtp_restart_complete_ind: - issue MTP_RESTART_COMPLETE_IND primitive
 * @mt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
mtp_restart_complete_ind(struct mt *mt, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_complete_ind *p;
	mblk_t *mp;

	(void) mtp_restart_complete_ind;
	if (bcanputnext(mt->oq, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESTART_COMPLETE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_RESTART_COMPLETE_IND");
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
 *  LM User -> LM Provider Primitives
 *
 *  =========================================================================
 */

/**
 * lmi_info_req: - issue LMI_INFO_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_info_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_info_req_t *p;
	mblk_t *mp;

	(void) lmi_info_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_INFO_REQ ->");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_attach_req: - issue LMI_ATTACH_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @ppa_len: PPA length
 * @ppa_ptr: PPA pointer
 */
static int
lmi_attach_req(struct sl *sl, queue_t *q, mblk_t *msg, size_t ppa_len, caddr_t ppa_ptr)
{
	lmi_attach_req_t *p;
	mblk_t *mp;

	(void) lmi_attach_req;
	if ((mp = mi_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ATTACH_REQ;
		mp->b_wptr += sizeof(*p);
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_ATTACH_REQ ->");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_detach_req: - issue LMI_DETACH_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_detach_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_detach_req_t *p;
	mblk_t *mp;

	(void) lmi_detach_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_DETACH_REQ ->");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_req: - issue LMI_ENABLE_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @rem_len: remote PPA length
 * @rem_ptr: remote PPA pointer
 */
static int
lmi_enable_req(struct sl *sl, queue_t *q, mblk_t *msg, size_t rem_len, caddr_t rem_ptr)
{
	lmi_enable_req_t *p;
	mblk_t *mp;

	(void) lmi_enable_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_ENABLE_REQ;
			mp->b_wptr += sizeof(*p);
			bcopy(rem_ptr, mp->b_wptr, rem_len);
			mp->b_wptr += rem_len;
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "LMI_ENABLE_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_disable_req: - issue LMI_DISABLE_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_disable_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_disable_req_t *p;
	mblk_t *mp;

	(void) lmi_disable_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "LMI_DISABLE_REQ ->");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_optmgmt_req: - issue LMI_OPTMGMT_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: management flags
 * @opts: options
 */
static int
lmi_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *msg, lmi_ulong flags, struct sl_options *opts)
{
	lmi_optmgmt_req_t *p;
	mblk_t *mp;

	size_t opt_len = sl_size_options(sl, q, flags, opts);
	size_t msg_len = sizeof(*p) + opt_len;

	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_OPTMGMT_REQ;
			p->lmi_opt_length = opt_len;
			p->lmi_opt_offset = sizeof(*p);
			p->lmi_mgmt_flags = flags;
			mp->b_wptr += sizeof(*p);
			sl_build_options(sl, q, flags, opts, mp);
			mp->b_wptr += opt_len;
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "LMI_OPTMGMT_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  SL User -> SL Provider Primitives
 *
 *  =========================================================================
 */

/**
 * sl_flush: - issue M_FLUSH message
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: flush flags
 * @band: flush band
 */
static int
sl_flush(struct sl *sl, queue_t *q, mblk_t *msg, int flags, int band)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_FLUSH;
		*mp->b_wptr++ = flags | (band ? FLUSHBAND : 0);
		*mp->b_wptr++ = band;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_FLUSH");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}


/**
 * sl_pdu_req: - issue SL_PDU_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 * @mpri: message priority
 * @dp: user data
 *
 * This is the fast path, we should not even call mi_strlog() here: packing up
 * 5 arguments and putting them on the stack and calling strlog() to find that
 * the level is trace filtered anyway is not a good idea in the main data
 * path.
 */
static int
sl_pdu_req(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong mpri, mblk_t *dp)
{
	sl_pdu_req_t *p;
	mblk_t *mp;

	(void) sl_pdu_req;
	if (unlikely(!canputnext(sl->oq)))
		goto ebusy;
	if (unlikely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->sl_primitive = SL_PDU_REQ;
	p->sl_mp = mpri;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (likely(msg && msg->b_cont == dp))
		msg->b_cont = NULL;
	freemsg(msg);
#if 0
	mi_strlog(sl->oq, STRLOGDA, SL_TRACE, "SL_PDU_REQ ->");
#endif
	putnext(sl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * sl_emergency_req: - issue SL_EMERGENCY_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_emergency_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_emergency_req_t *p;
	mblk_t *mp;

	(void) sl_emergency_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_EMERGENCY_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_EMERGENCY_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_emergency_ceases_req: - issue SL_EMERGENCY_CEASES_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_emergency_ceases_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_emergency_ceases_req_t *p;
	mblk_t *mp;

	(void) sl_emergency_ceases_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_EMERGENCY_CEASES_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_start_req: - issue SL_START_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_start_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_start_req_t *p;
	mblk_t *mp;

	(void) sl_start_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_START_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_START_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_stop_req: - issue SL_STOP_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_stop_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_stop_req_t *p;
	mblk_t *mp;

	(void) sl_stop_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_STOP_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_STOP_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_retrieve_bsnt_req: - issue SL_RETRIEVE_BSNT_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieve_bsnt_req_t *p;
	mblk_t *mp;

	(void) sl_retrieve_bsnt_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_RETRIEVE_BSNT_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_retrieval_request_and_fsnc_req: - issue SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 * @fsnc: FSNC value
 */
static int
sl_retrieval_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong fsnc)
{
	sl_retrieval_req_and_fsnc_t *p;
	mblk_t *mp;

	(void) sl_retrieval_request_and_fsnc_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
			p->sl_fsnc = fsnc;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_clear_buffers_req: - issue SL_CLEAR_BUFFERS_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 *
 * We should actualy flush both the read and write queues before sending this
 * message.
 */
static int
sl_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_clear_buffers_req_t *p;
	mblk_t *mp;
	int err;

	(void) sl_clear_buffers_req;
	if ((err = sl_flush(sl, q, NULL, FLUSHRW, 0)) == 0) {
		if (canputnext(sl->oq)) {
			if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
				DB_TYPE(mp) = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
				mp->b_wptr += sizeof(*p);
				freemsg(msg);
				mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_CLEAR_BUFFERS_REQ ->");
				putnext(sl->oq, mp);
				return (0);
			}
			return (-ENOBUFS);
		}
		return (-EBUSY);
	}
	return (err);
}

/**
 * sl_clear_rtb_req: - issue SL_CLEAR_RTB_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_clear_rtb_req_t *p;
	mblk_t *mp;

	(void) sl_clear_rtb_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_CLEAR_RTB_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_CLEAR_RTB_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_continue_req: - issue SL_CONTINUE_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_continue_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_continue_req_t *p;
	mblk_t *mp;

	(void) sl_continue_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_CONTINUE_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_CONTINUE_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_local_processor_outage_req: - issue SL_LOCAL_PROCESSOR_OUTAGE_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_local_proc_outage_req_t *p;
	mblk_t *mp;

	(void) sl_local_processor_outage_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_LOCAL_PROCESSOR_OUTAGE_REQ ->");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_resume_req: - issue SL_RESUME_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_resume_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_resume_req_t *p;
	mblk_t *mp;

	(void) sl_resume_req;
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RESUME_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_RESUME_REQ ->");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_congestion_discard_req: - issue SL_CONGESTION_DISCARD_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_cong_discard_req_t *p;
	mblk_t *mp;

	(void) sl_congestion_discard_req;
	if (bcanputnext(sl->oq, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_CONGESTION_DISCARD_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_congestion_accept_req: - issue SL_CONGESTION_ACCEPT_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_cong_accept_req_t *p;
	mblk_t *mp;

	(void) sl_congestion_accept_req;
	if (bcanputnext(sl->oq, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_CONGESTION_ACCEPT_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_no_congestion_req: - issue SL_NO_CONGESTION_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_no_cong_req_t *p;
	mblk_t *mp;

	(void) sl_no_congestion_req;
	if (bcanputnext(sl->oq, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_NO_CONGESTION_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_NO_CONGESTION_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_power_on_req: - issue SL_POWER_ON_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_power_on_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_power_on_req_t *p;
	mblk_t *mp;

	(void) sl_power_on_req;
	if (canputnext(sl->oq)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_POWER_ON_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "SL_POWER_ON_REQ ->");
			putnext(sl->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sl_optmgmt_req: - issue SL_OPTMGMT_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong flags, struct sl_options *opts)
{
	(void) sl_optmgmt_req;
	return lmi_optmgmt_req(sl, q, msg, flags, opts);
}

/**
 * sl_notify_req: - issue SL_NOTIFY_REQ primitive
 * @sl: private structure
 * @q: active queue
 * @msg: messsage to free upon success
 */
static int
sl_notify_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
#if 0
	return lmi_notify_req(sl, q, msg);
#endif
	(void) sl_notify_req;
	freemsg(msg);
	return (0);
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
 *  RECEIVED PRIMITIVES
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  MTP User -> MTP Provider Primitives
 *
 *  =========================================================================
 */

/**
 * mtp_bind_req:- process MTP_BIND_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_BIND_REQ primitive
 */
static int
mtp_bind_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, MTP_BIND_REQ, err);
}

/**
 * mtp_unbind_req:- process MTP_UNBIND_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_UNBIND_REQ primitive
 */
static int
mtp_unbind_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, MTP_UNBIND_REQ, err);
}

/**
 * mtp_conn_req:- process MTP_CONN_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_CONN_REQ primitive
 */
static int
mtp_conn_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_conn_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, MTP_CONN_REQ, err);
}

/**
 * mtp_discon_req:- process MTP_DISCON_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_DISCON_REQ primitive
 */
static int
mtp_discon_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_discon_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, MTP_DISCON_REQ, err);
}

/**
 * mtp_addr_req:- process MTP_ADDR_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_ADDR_REQ primitive
 */
static int
mtp_addr_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, MTP_ADDR_REQ, err);
}

/**
 * mtp_info_req:- process MTP_INFO_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_INFO_REQ primitive
 */
static int
mtp_info_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, MTP_INFO_REQ, err);
}

/**
 * mtp_optmgmt_req:- process MTP_OPTMGMT_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_OPTMGMT_REQ primitive
 */
static int
mtp_optmgmt_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, MTP_OPTMGMT_REQ, err);
}

/**
 * mtp_transfer_req:- process MTP_TRANSFER_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_TRANSFER_REQ primitive
 */
static int
mtp_transfer_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct MTP_transfer_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, MTP_TRANSFER_REQ, err);
}

/**
 * mtp_other_req:- process MTP_????_REQ primitive
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the MTP_????_REQ primitive
 */
static int
mtp_other_req(struct mt *mt, queue_t *q, mblk_t *mp)
{
	mtp_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupp;
      notsupp:
	err = MNOTSUPP;
	goto error;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_ack(mt, q, mp, *p, err);
}

/*
 *  =========================================================================
 *
 *  LM Provider -> LM User Primitives
 *
 *  =========================================================================
 */

/**
 * lmi_info_ack:- process LMI_INFO_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_INFO_ACK primitive
 */
static int
lmi_info_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * lmi_ok_ack:- process LMI_OK_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_OK_ACK primitive
 */
static int
lmi_ok_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * lmi_error_ack:- process LMI_ERROR_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_ERROR_ACK primitive
 */
static int
lmi_error_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * lmi_enable_con:- process LMI_ENABLE_CON primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_ENABLE_CON primitive
 */
static int
lmi_enable_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * lmi_disable_con:- process LMI_DISABLE_CON primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_DISABLE_CON primitive
 */
static int
lmi_disable_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * lmi_optmgmt_ack:- process LMI_OPTMGMT_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_OPTMGMT_ACK primitive
 */
static int
lmi_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * lmi_error_ind:- process LMI_ERROR_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_ERROR_IND primitive
 */
static int
lmi_error_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * lmi_stats_ind:- process LMI_STATS_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_STATS_IND primitive
 */
static int
lmi_stats_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * lmi_event_ind:- process LMI_EVENT_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the LMI_EVENT_IND primitive
 */
static int
lmi_event_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/*
 *  =========================================================================
 *
 *  SL Provider -> SL User Primitives
 *
 *  =========================================================================
 */

/**
 * sl_pdu_ind:- process SL_PDU_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_PDU_IND primitive
 */
static int
sl_pdu_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_pdu_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_link_congested_ind:- process SL_LINK_CONGESTED_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_LINK_CONGESTED_IND primitive
 */
static int
sl_link_congested_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_link_congestion_ceased_ind:- process SL_LINK_CONGESTION_CEASED_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_LINK_CONGESTION_CEASED_IND primitive
 */
static int
sl_link_congestion_ceased_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ceased_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_retrieved_message_ind:- process SL_RETRIEVED_MESSAGE_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_RETRIEVED_MESSAGE_IND primitive
 */
static int
sl_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieved_msg_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_retrieval_complete_ind:- process SL_RETRIEVAL_COMPLETE_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_RETRIEVAL_COMPLETE_IND primitive
 */
static int
sl_retrieval_complete_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_comp_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_rb_cleared_ind:- process SL_RB_CLEARED_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_RB_CLEARED_IND primitive
 */
static int
sl_rb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_rb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_bsnt_ind:- process SL_BSNT_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_BSNT_IND primitive
 */
static int
sl_bsnt_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_bsnt_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_in_service_ind:- process SL_IN_SERVICE_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_IN_SERVICE_IND primitive
 */
static int
sl_in_service_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_in_service_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_out_of_service_ind:- process SL_OUT_OF_SERVICE_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_OUT_OF_SERVICE_IND primitive
 */
static int
sl_out_of_service_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_out_of_service_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_remote_processor_outage_ind:- process SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 */
static int
sl_remote_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_rem_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_remote_processor_recovered_ind:- process SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 */
static int
sl_remote_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_rem_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_rtb_cleared_ind:- process SL_RTB_CLEARED_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_RTB_CLEARED_IND primitive
 */
static int
sl_rtb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_rtb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_retrieval_not_possible_ind:- process SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 */
static int
sl_retrieval_not_possible_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_not_poss_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_bsnt_not_retrievable_ind:- process SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_BSNT_NOT_RETRIEVABLE_IND primitive
 */
static int
sl_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_bsnt_not_retr_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_optmgmt_ack:- process SL_OPTMGMT_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_OPTMGMT_ACK primitive
 */
static int
sl_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_notify_ind:- process SL_NOTIFY_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_NOTIFY_IND primitive
 */
static int
sl_notify_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
#if 0
	sl_notify_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
#endif
	freemsg(mp);
	return (0);
}

/**
 * sl_local_processor_outage_ind:- process SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 */
static int
sl_local_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_loc_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_local_processor_recovered_ind:- process SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 */
static int
sl_local_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_loc_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/**
 * sl_other_ind:- process SL_????_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the SL_????_IND primitive
 */
static int
sl_other_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = EFAULT;
	goto error;
      error:
	return m_error(sl->mt, q, mp, err);
}

/*
 *  =========================================================================
 *
 *  INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
 */

static int
mt_testoption(struct mt *mt, struct mtp_option *arg)
{
	/* FIXME: check options */
	return (0);
}
static int
mt_setoption(struct mt *mt, struct mtp_option *arg)
{
	int err;

	if ((err = mt_testoption(mt, arg)) != 0)
		return (err);
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		df.option = arg->options[0].df;
		break;
	case MTP_OBJ_TYPE_NA:
		mt->na.option = arg->options[0].na;
		break;
	case MTP_OBJ_TYPE_SP:
		mt->sp.option = arg->options[0].sp;
		break;
	case MTP_OBJ_TYPE_RS:
		mt->rs.option = arg->options[0].rs;
		break;
	case MTP_OBJ_TYPE_RL:
		mt->rl.option = arg->options[0].rl;
		break;
	case MTP_OBJ_TYPE_RT:
		mt->rt.option = arg->options[0].rt;
		break;
	case MTP_OBJ_TYPE_LS:
		mt->ls.option = arg->options[0].ls;
		break;
	case MTP_OBJ_TYPE_LK:
		mt->lk.option = arg->options[0].lk;
		break;
	case MTP_OBJ_TYPE_SL:
		mt->sl->option = arg->options[0].sl;
		break;
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
	default:
		err = EINVAL;
		break;
	}
	return (0);
}
static int
mt_testconfig(struct mt *mt, struct mtp_config *arg)
{
	/* FIXME: check configuration */
	return (0);
}
static int
mt_setconfig(struct mt *mt, struct mtp_config *arg)
{
	int err;

	if ((err = mt_testconfig(mt, arg)) != 0)
		return (err);
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		df.config = arg->config[0].df;
		break;
	case MTP_OBJ_TYPE_NA:
		mt->na.config = arg->config[0].na;
		break;
	case MTP_OBJ_TYPE_SP:
		mt->sp.config = arg->config[0].sp;
		break;
	case MTP_OBJ_TYPE_RS:
		mt->rs.config = arg->config[0].rs;
		break;
	case MTP_OBJ_TYPE_RL:
		mt->rl.config = arg->config[0].rl;
		break;
	case MTP_OBJ_TYPE_RT:
		mt->rt.config = arg->config[0].rt;
		break;
	case MTP_OBJ_TYPE_LS:
		mt->ls.config = arg->config[0].ls;
		break;
	case MTP_OBJ_TYPE_LK:
		mt->lk.config = arg->config[0].lk;
		break;
	case MTP_OBJ_TYPE_SL:
		mt->sl->config = arg->config[0].sl;
		break;
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (EINVAL);
	}
	return (0);
}
static int
mt_setstatsp(struct mt *mt, struct mtp_stats *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		df.statsp = arg->stats[0].df;
		break;
	case MTP_OBJ_TYPE_NA:
		mt->na.statsp = arg->stats[0].na;
		break;
	case MTP_OBJ_TYPE_SP:
		mt->sp.statsp = arg->stats[0].sp;
		break;
	case MTP_OBJ_TYPE_RS:
		mt->rs.statsp = arg->stats[0].rs;
		break;
	case MTP_OBJ_TYPE_RL:
		mt->rl.statsp = arg->stats[0].rl;
		break;
	case MTP_OBJ_TYPE_RT:
		mt->rt.statsp = arg->stats[0].rt;
		break;
	case MTP_OBJ_TYPE_LS:
		mt->ls.statsp = arg->stats[0].ls;
		break;
	case MTP_OBJ_TYPE_LK:
		mt->lk.statsp = arg->stats[0].lk;
		break;
	case MTP_OBJ_TYPE_SL:
		mt->sl->statsp = arg->stats[0].sl;
		break;
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (EINVAL);
	}
	return (0);
}
static int
mt_setnotify(struct mt *mt, struct mtp_notify *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		df.notify.events |= arg->events[0].df.events;
		arg->events[0].df.events = df.notify.events;
		break;
	case MTP_OBJ_TYPE_NA:
		mt->na.notify.events |= arg->events[0].na.events;
		arg->events[0].na.events = mt->na.notify.events;
		break;
	case MTP_OBJ_TYPE_SP:
		mt->sp.notify.events |= arg->events[0].sp.events;
		arg->events[0].sp.events = mt->sp.notify.events;
		break;
	case MTP_OBJ_TYPE_RS:
		mt->rs.notify.events |= arg->events[0].rs.events;
		arg->events[0].rs.events = mt->rs.notify.events;
		break;
	case MTP_OBJ_TYPE_RL:
		mt->rl.notify.events |= arg->events[0].rl.events;
		arg->events[0].rl.events = mt->rl.notify.events;
		break;
	case MTP_OBJ_TYPE_RT:
		mt->rt.notify.events |= arg->events[0].rt.events;
		arg->events[0].rt.events = mt->rt.notify.events;
		break;
	case MTP_OBJ_TYPE_LS:
		mt->ls.notify.events |= arg->events[0].ls.events;
		arg->events[0].ls.events = mt->ls.notify.events;
		break;
	case MTP_OBJ_TYPE_LK:
		mt->lk.notify.events |= arg->events[0].lk.events;
		arg->events[0].lk.events = mt->lk.notify.events;
		break;
	case MTP_OBJ_TYPE_SL:
		mt->sl->notify.events |= arg->events[0].sl.events;
		arg->events[0].sl.events = mt->sl->notify.events;
		break;
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (EINVAL);
	}
	return (0);
}
static int
mt_clrnotify(struct mt *mt, struct mtp_notify *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		df.notify.events &= ~arg->events[0].df.events;
		arg->events[0].df.events = df.notify.events;
		break;
	case MTP_OBJ_TYPE_NA:
		mt->na.notify.events &= ~arg->events[0].na.events;
		arg->events[0].na.events = mt->na.notify.events;
		break;
	case MTP_OBJ_TYPE_SP:
		mt->sp.notify.events &= ~arg->events[0].sp.events;
		arg->events[0].sp.events = mt->sp.notify.events;
		break;
	case MTP_OBJ_TYPE_RS:
		mt->rs.notify.events &= ~arg->events[0].rs.events;
		arg->events[0].rs.events = mt->rs.notify.events;
		break;
	case MTP_OBJ_TYPE_RL:
		mt->rl.notify.events &= ~arg->events[0].rl.events;
		arg->events[0].rl.events = mt->rl.notify.events;
		break;
	case MTP_OBJ_TYPE_RT:
		mt->rt.notify.events &= ~arg->events[0].rt.events;
		arg->events[0].rt.events = mt->rt.notify.events;
		break;
	case MTP_OBJ_TYPE_LS:
		mt->ls.notify.events &= ~arg->events[0].ls.events;
		arg->events[0].ls.events = mt->ls.notify.events;
		break;
	case MTP_OBJ_TYPE_LK:
		mt->lk.notify.events &= ~arg->events[0].lk.events;
		arg->events[0].lk.events = mt->lk.notify.events;
		break;
	case MTP_OBJ_TYPE_SL:
		mt->sl->notify.events &= ~arg->events[0].sl.events;
		arg->events[0].sl.events = mt->sl->notify.events;
		break;
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (EINVAL);
	}
	return (0);
}

static int
mt_size_options(struct mt *mt, struct mtp_option *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (sizeof(df.option));
	case MTP_OBJ_TYPE_NA:
		return (sizeof(mt->na.option));
	case MTP_OBJ_TYPE_SP:
		return (sizeof(mt->sp.option));
	case MTP_OBJ_TYPE_RS:
		return (sizeof(mt->rs.option));
	case MTP_OBJ_TYPE_RL:
		return (sizeof(mt->rl.option));
	case MTP_OBJ_TYPE_RT:
		return (sizeof(mt->rt.option));
	case MTP_OBJ_TYPE_LS:
		return (sizeof(mt->ls.option));
	case MTP_OBJ_TYPE_LK:
		return (sizeof(mt->lk.option));
	case MTP_OBJ_TYPE_SL:
		return (sizeof(mt->sl->option));
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (0);
	}
}
static int
mt_size_config(struct mt *mt, struct mtp_config *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (sizeof(df.config));
	case MTP_OBJ_TYPE_NA:
		return (sizeof(mt->na.config));
	case MTP_OBJ_TYPE_SP:
		return (sizeof(mt->sp.config));
	case MTP_OBJ_TYPE_RS:
		return (sizeof(mt->rs.config));
	case MTP_OBJ_TYPE_RL:
		return (sizeof(mt->rl.config));
	case MTP_OBJ_TYPE_RT:
		return (sizeof(mt->rt.config));
	case MTP_OBJ_TYPE_LS:
		return (sizeof(mt->ls.config));
	case MTP_OBJ_TYPE_LK:
		return (sizeof(mt->lk.config));
	case MTP_OBJ_TYPE_SL:
		return (sizeof(mt->sl->config));
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (0);
	}
}
static int
mt_size_statem(struct mt *mt, struct mtp_statem *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (sizeof(df.statem));
	case MTP_OBJ_TYPE_NA:
		return (sizeof(mt->na.statem));
	case MTP_OBJ_TYPE_SP:
		return (sizeof(mt->sp.statem));
	case MTP_OBJ_TYPE_RS:
		return (sizeof(mt->rs.statem));
	case MTP_OBJ_TYPE_RL:
		return (sizeof(mt->rl.statem));
	case MTP_OBJ_TYPE_RT:
		return (sizeof(mt->rt.statem));
	case MTP_OBJ_TYPE_LS:
		return (sizeof(mt->ls.statem));
	case MTP_OBJ_TYPE_LK:
		return (sizeof(mt->lk.statem));
	case MTP_OBJ_TYPE_SL:
		return (sizeof(mt->sl->statem));
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (0);
	}
}
static int
mt_size_statsp(struct mt *mt, struct mtp_stats *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (sizeof(df.statsp));
	case MTP_OBJ_TYPE_NA:
		return (sizeof(mt->na.statsp));
	case MTP_OBJ_TYPE_SP:
		return (sizeof(mt->sp.statsp));
	case MTP_OBJ_TYPE_RS:
		return (sizeof(mt->rs.statsp));
	case MTP_OBJ_TYPE_RL:
		return (sizeof(mt->rl.statsp));
	case MTP_OBJ_TYPE_RT:
		return (sizeof(mt->rt.statsp));
	case MTP_OBJ_TYPE_LS:
		return (sizeof(mt->ls.statsp));
	case MTP_OBJ_TYPE_LK:
		return (sizeof(mt->lk.statsp));
	case MTP_OBJ_TYPE_SL:
		return (sizeof(mt->sl->statsp));
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (0);
	}
}
static int
mt_size_stats(struct mt *mt, struct mtp_stats *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (sizeof(df.stats));
	case MTP_OBJ_TYPE_NA:
		return (sizeof(mt->na.stats));
	case MTP_OBJ_TYPE_SP:
		return (sizeof(mt->sp.stats));
	case MTP_OBJ_TYPE_RS:
		return (sizeof(mt->rs.stats));
	case MTP_OBJ_TYPE_RL:
		return (sizeof(mt->rl.stats));
	case MTP_OBJ_TYPE_RT:
		return (sizeof(mt->rt.stats));
	case MTP_OBJ_TYPE_LS:
		return (sizeof(mt->ls.stats));
	case MTP_OBJ_TYPE_LK:
		return (sizeof(mt->lk.stats));
	case MTP_OBJ_TYPE_SL:
		return (sizeof(mt->sl->stats));
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (0);
	}
}
static int
mt_size_notify(struct mt *mt, struct mtp_notify *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (sizeof(df.notify));
	case MTP_OBJ_TYPE_NA:
		return (sizeof(mt->na.notify));
	case MTP_OBJ_TYPE_SP:
		return (sizeof(mt->sp.notify));
	case MTP_OBJ_TYPE_RS:
		return (sizeof(mt->rs.notify));
	case MTP_OBJ_TYPE_RL:
		return (sizeof(mt->rl.notify));
	case MTP_OBJ_TYPE_RT:
		return (sizeof(mt->rt.notify));
	case MTP_OBJ_TYPE_LS:
		return (sizeof(mt->ls.notify));
	case MTP_OBJ_TYPE_LK:
		return (sizeof(mt->lk.notify));
	case MTP_OBJ_TYPE_SL:
		return (sizeof(mt->sl->notify));
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (0);
	}
}

static void *
mt_ptr_options(struct mt *mt, struct mtp_option *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (&df.option);
	case MTP_OBJ_TYPE_NA:
		return (&mt->na.option);
	case MTP_OBJ_TYPE_SP:
		return (&mt->sp.option);
	case MTP_OBJ_TYPE_RS:
		return (&mt->rs.option);
	case MTP_OBJ_TYPE_RL:
		return (&mt->rl.option);
	case MTP_OBJ_TYPE_RT:
		return (&mt->rt.option);
	case MTP_OBJ_TYPE_LS:
		return (&mt->ls.option);
	case MTP_OBJ_TYPE_LK:
		return (&mt->lk.option);
	case MTP_OBJ_TYPE_SL:
		return (&mt->sl->option);
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (NULL);
	}
}
static void *
mt_ptr_config(struct mt *mt, struct mtp_config *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (&df.config);
	case MTP_OBJ_TYPE_NA:
		return (&mt->na.config);
	case MTP_OBJ_TYPE_SP:
		return (&mt->sp.config);
	case MTP_OBJ_TYPE_RS:
		return (&mt->rs.config);
	case MTP_OBJ_TYPE_RL:
		return (&mt->rl.config);
	case MTP_OBJ_TYPE_RT:
		return (&mt->rt.config);
	case MTP_OBJ_TYPE_LS:
		return (&mt->ls.config);
	case MTP_OBJ_TYPE_LK:
		return (&mt->lk.config);
	case MTP_OBJ_TYPE_SL:
		return (&mt->sl->config);
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (NULL);
	}
}
static void *
mt_ptr_statem(struct mt *mt, struct mtp_statem *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (&df.statem);
	case MTP_OBJ_TYPE_NA:
		return (&mt->na.statem);
	case MTP_OBJ_TYPE_SP:
		return (&mt->sp.statem);
	case MTP_OBJ_TYPE_RS:
		return (&mt->rs.statem);
	case MTP_OBJ_TYPE_RL:
		return (&mt->rl.statem);
	case MTP_OBJ_TYPE_RT:
		return (&mt->rt.statem);
	case MTP_OBJ_TYPE_LS:
		return (&mt->ls.statem);
	case MTP_OBJ_TYPE_LK:
		return (&mt->lk.statem);
	case MTP_OBJ_TYPE_SL:
		return (&mt->sl->statem);
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (NULL);
	}
}
static void *
mt_ptr_statsp(struct mt *mt, struct mtp_stats *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (&df.statsp);
	case MTP_OBJ_TYPE_NA:
		return (&mt->na.statsp);
	case MTP_OBJ_TYPE_SP:
		return (&mt->sp.statsp);
	case MTP_OBJ_TYPE_RS:
		return (&mt->rs.statsp);
	case MTP_OBJ_TYPE_RL:
		return (&mt->rl.statsp);
	case MTP_OBJ_TYPE_RT:
		return (&mt->rt.statsp);
	case MTP_OBJ_TYPE_LS:
		return (&mt->ls.statsp);
	case MTP_OBJ_TYPE_LK:
		return (&mt->lk.statsp);
	case MTP_OBJ_TYPE_SL:
		return (&mt->sl->statsp);
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (NULL);
	}
}
static void *
mt_ptr_stats(struct mt *mt, struct mtp_stats *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (&df.stats);
	case MTP_OBJ_TYPE_NA:
		return (&mt->na.stats);
	case MTP_OBJ_TYPE_SP:
		return (&mt->sp.stats);
	case MTP_OBJ_TYPE_RS:
		return (&mt->rs.stats);
	case MTP_OBJ_TYPE_RL:
		return (&mt->rl.stats);
	case MTP_OBJ_TYPE_RT:
		return (&mt->rt.stats);
	case MTP_OBJ_TYPE_LS:
		return (&mt->ls.stats);
	case MTP_OBJ_TYPE_LK:
		return (&mt->lk.stats);
	case MTP_OBJ_TYPE_SL:
		return (&mt->sl->stats);
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (NULL);
	}
}
static void *
mt_ptr_notify(struct mt *mt, struct mtp_notify *arg)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_DF:
		return (&df.notify);
	case MTP_OBJ_TYPE_NA:
		return (&mt->na.notify);
	case MTP_OBJ_TYPE_SP:
		return (&mt->sp.notify);
	case MTP_OBJ_TYPE_RS:
		return (&mt->rs.notify);
	case MTP_OBJ_TYPE_RL:
		return (&mt->rl.notify);
	case MTP_OBJ_TYPE_RT:
		return (&mt->rt.notify);
	case MTP_OBJ_TYPE_LS:
		return (&mt->ls.notify);
	case MTP_OBJ_TYPE_LK:
		return (&mt->lk.notify);
	case MTP_OBJ_TYPE_SL:
		return (&mt->sl->notify);
	default:
	case MTP_OBJ_TYPE_MT:
	case MTP_OBJ_TYPE_CB:
	case MTP_OBJ_TYPE_CR:
		return (NULL);
	}
}

/**
 * mt_ioctl: - process M_IOCTL message
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the MTP input-output control operation.  Step 1 consists always consists of a
 * copyin operation to determine the object type and id.
 */
static __unlikely int
mt_ioctl(struct mt *mt, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", mt_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(MTP_IOCGOPTION):
	case _IOC_NR(MTP_IOCSOPTION):
		mi_copyin(q, mp, NULL, sizeof(struct mtp_option));
		break;
	case _IOC_NR(MTP_IOCGCONFIG):
	case _IOC_NR(MTP_IOCSCONFIG):
	case _IOC_NR(MTP_IOCTCONFIG):
	case _IOC_NR(MTP_IOCCCONFIG):
		mi_copyin(q, mp, NULL, sizeof(struct mtp_config));
		break;
	case _IOC_NR(MTP_IOCGSTATEM):
	case _IOC_NR(MTP_IOCCMRESET):
		mi_copyin(q, mp, NULL, sizeof(struct mtp_statem));
		break;
	case _IOC_NR(MTP_IOCGSTATSP):
	case _IOC_NR(MTP_IOCSSTATSP):
	case _IOC_NR(MTP_IOCGSTATS):
	case _IOC_NR(MTP_IOCCSTATS):
		mi_copyin(q, mp, NULL, sizeof(struct mtp_stats));
		break;
	case _IOC_NR(MTP_IOCGNOTIFY):
	case _IOC_NR(MTP_IOCSNOTIFY):
	case _IOC_NR(MTP_IOCCNOTIFY):
		mi_copyin(q, mp, NULL, sizeof(struct mtp_notify));
		break;
	case _IOC_NR(MTP_IOCCMGMT):
		mi_copyin(q, mp, NULL, sizeof(struct mtp_mgmt));
		break;
	case _IOC_NR(MTP_IOCCPASS):
		mi_copyin(q, mp, NULL, sizeof(struct mtp_pass));
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	return (0);
}

/**
 * mt_copyin1: - process M_IOCDATA message
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: the copyied in data
 *
 * This is step 2 of the MTP input-output control operationl.  Step 2 consists of identifying the
 * object and performing a copyout for GET operations and an additional copyin for SET operations.
 */
static __unlikely int
mt_copyin1(struct mt *mt, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int alen, len, size = -1;
	int err = 0;
	mblk_t *bp;
	void *ptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", mt_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MTP_IOCGOPTION):
		alen = sizeof(struct mtp_option);
		len = mt_size_options(mt, (struct mtp_option *) dp->b_rptr);
		ptr = mt_ptr_options(mt, (struct mtp_option *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(MTP_IOCSOPTION):
		alen = sizeof(struct mtp_option);
		len = mt_size_options(mt, (struct mtp_option *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(MTP_IOCGCONFIG):
		alen = sizeof(struct mtp_config);
		len = mt_size_config(mt, (struct mtp_config *) dp->b_rptr);
		ptr = mt_ptr_config(mt, (struct mtp_config *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(MTP_IOCSCONFIG):
	case _IOC_NR(MTP_IOCTCONFIG):
	case _IOC_NR(MTP_IOCCCONFIG):
		alen = sizeof(struct mtp_config);
		len = mt_size_config(mt, (struct mtp_config *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(MTP_IOCGSTATEM):
		alen = sizeof(struct mtp_statem);
		len = mt_size_statem(mt, (struct mtp_statem *) dp->b_rptr);
		ptr = mt_ptr_statem(mt, (struct mtp_statem *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(MTP_IOCCMRESET):
		size = 0;
		switch (((struct mtp_statem *) dp->b_rptr)->type) {
		case MTP_OBJ_TYPE_DF:
		case MTP_OBJ_TYPE_NA:
		case MTP_OBJ_TYPE_SP:
		case MTP_OBJ_TYPE_RS:
		case MTP_OBJ_TYPE_RL:
		case MTP_OBJ_TYPE_RT:
		case MTP_OBJ_TYPE_LS:
		case MTP_OBJ_TYPE_LK:
		case MTP_OBJ_TYPE_SL:
			/* FIXME: reset state machine */
			break;
		default:
		case MTP_OBJ_TYPE_CR:
		case MTP_OBJ_TYPE_CB:
		case MTP_OBJ_TYPE_MT:
			err = EINVAL;
			break;
		}
		break;
	case _IOC_NR(MTP_IOCGSTATSP):
		alen = sizeof(struct mtp_stats);
		len = mt_size_statsp(mt, (struct mtp_stats *) dp->b_rptr);
		ptr = mt_ptr_statsp(mt, (struct mtp_stats *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(MTP_IOCSSTATSP):
		alen = sizeof(struct mtp_stats);
		len = mt_size_statsp(mt, (struct mtp_stats *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(MTP_IOCGSTATS):
		alen = sizeof(struct mtp_stats);
		len = mt_size_stats(mt, (struct mtp_stats *) dp->b_rptr);
		ptr = mt_ptr_stats(mt, (struct mtp_stats *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(MTP_IOCCSTATS):
		alen = sizeof(struct mtp_stats);
		len = mt_size_stats(mt, (struct mtp_stats *) dp->b_rptr);
		ptr = mt_ptr_stats(mt, (struct mtp_stats *) dp->b_rptr);
		goto copyout_clear;
	case _IOC_NR(MTP_IOCGNOTIFY):
		alen = sizeof(struct mtp_notify);
		len = mt_size_notify(mt, (struct mtp_notify *) dp->b_rptr);
		ptr = mt_ptr_notify(mt, (struct mtp_notify *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(MTP_IOCSNOTIFY):
	case _IOC_NR(MTP_IOCCNOTIFY):
		alen = sizeof(struct mtp_notify);
		len = mt_size_notify(mt, (struct mtp_notify *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(MTP_IOCCMGMT):
		size = 0;
		switch (((struct mtp_mgmt *) dp->b_rptr)->type) {
		case MTP_OBJ_TYPE_DF:
		case MTP_OBJ_TYPE_NA:
		case MTP_OBJ_TYPE_SP:
		case MTP_OBJ_TYPE_RS:
		case MTP_OBJ_TYPE_RL:
		case MTP_OBJ_TYPE_RT:
		case MTP_OBJ_TYPE_LS:
		case MTP_OBJ_TYPE_LK:
		case MTP_OBJ_TYPE_SL:
			/* FIXME: perform command */
			break;
		default:
		case MTP_OBJ_TYPE_MT:
		case MTP_OBJ_TYPE_CB:
		case MTP_OBJ_TYPE_CR:
			err = EINVAL;
			break;
		}
		break;
	case _IOC_NR(MTP_IOCCPASS):
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
 * mt_copyin2: - process M_IOCDATA message
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: the copyied in data
 *
 * This is step 3 for SET operations.  This is the result of the implicit or explicit copyin
 * operation for the object specific structure.  We can now perform sets and commits.  All SET
 * operations also include a last copyout step that copies out the information actually set.
 */
static __unlikely int
mt_copyin2(struct mt *mt, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int len, err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", mt_iocname(cp->cp_cmd));

	len = dp->b_wptr - dp->b_rptr;
	if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, len);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MTP_IOCSOPTION):
		err = mt_setoption(mt, (struct mtp_option *) bp->b_rptr);
		break;
	case _IOC_NR(MTP_IOCSCONFIG):
		err = mt_setconfig(mt, (struct mtp_config *) bp->b_rptr);
		break;
	case _IOC_NR(MTP_IOCTCONFIG):
		err = mt_testconfig(mt, (struct mtp_config *) bp->b_rptr);
		break;
	case _IOC_NR(MTP_IOCCCONFIG):
		err = mt_setconfig(mt, (struct mtp_config *) bp->b_rptr);
		break;
	case _IOC_NR(MTP_IOCSSTATSP):
		err = mt_setstatsp(mt, (struct mtp_stats *) bp->b_rptr);
		break;
	case _IOC_NR(MTP_IOCSNOTIFY):
		err = mt_setnotify(mt, (struct mtp_notify *) dp->b_rptr);
		break;
	case _IOC_NR(MTP_IOCCNOTIFY):
		err = mt_clrnotify(mt, (struct mtp_notify *) dp->b_rptr);
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
 * mt_copyout: - process M_IOCDATA message
 * @mt: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 *
 * This is the final step which is a simple copy done operation.
 */
static __unlikely int
mt_copyout(struct mt *mt, queue_t *q, mblk_t *mp)
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
 * mt_m_data: - process M_DATA message
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 */
static int
mt_m_data(queue_t *q, mblk_t *mp)
{
	/* we don't get these */
	freemsg(mp);
	return (0);
}

/**
 * mt_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall int
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
	sl_save_state(mt->sl);
	switch (prim) {
	case MTP_TRANSFER_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- MTP_TRANSFER_IND");
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", mt_primname(prim));
		break;
	}
	switch (prim) {
	case MTP_BIND_REQ:
		rtn = mtp_bind_req(mt, q, mp);
		break;
	case MTP_UNBIND_REQ:
		rtn = mtp_unbind_req(mt, q, mp);
		break;
	case MTP_CONN_REQ:
		rtn = mtp_conn_req(mt, q, mp);
		break;
	case MTP_DISCON_REQ:
		rtn = mtp_discon_req(mt, q, mp);
		break;
	case MTP_ADDR_REQ:
		rtn = mtp_addr_req(mt, q, mp);
		break;
	case MTP_INFO_REQ:
		rtn = mtp_info_req(mt, q, mp);
		break;
	case MTP_OPTMGMT_REQ:
		rtn = mtp_optmgmt_req(mt, q, mp);
		break;
	case MTP_TRANSFER_REQ:
		rtn = mtp_transfer_req(mt, q, mp);
		break;
	default:
		rtn = mtp_other_req(mt, q, mp);
		break;
	}
	if (rtn) {
		sl_restore_state(mt->sl);
		mt_restore_state(mt);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * mt_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (write queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static int
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

	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = mt_t1_timeout(mt, q);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		mt_restore_state(mt);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * mt_m_ioctl: - process M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static int
mt_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct mt *mt;
	caddr_t priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != MTP_IOC_MAGIC) {
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
	mt = MT_PRIV(q);
	err = mt_ioctl(mt, q, mp);
	mi_unlock(priv);
	return (err);
}

/**
 * mt_m_iocdata: - process M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static int
mt_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct mt *mt;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != MTP_IOC_MAGIC) {
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
	mt = MT_PRIV(q);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = mt_copyin1(mt, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		err = mt_copyin2(mt, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = mt_copyout(mt, q, mp);
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
 * mt_m_flush: - process M_FLUSH message
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 *
 * Canonical module flush procedure for the write queue.
 */
static int
mt_m_flush(queue_t *q, mblk_t *mp)
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
 * mt_m_other: - process unrecognized STREAMS message
 * @q: active queue (write queue)
 * @mp: the unrecognized STREAMS message
 *
 * Unrecognized STREAMS messages are passed to the next module considering flow control.
 */
static int
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
	case M_IOCTL:
		return mt_m_ioctl(q, mp);
	case M_IOCDATA:
		return mt_m_iocdata(q, mp);
	case M_FLUSH:
		return mt_m_flush(q, mp);
	default:
		return mt_m_other(q, mp);
	}
}

/**
 * sl_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static int
sl_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * sl_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall int
sl_m_proto(queue_t *q, mblk_t *mp)
{
	sl_ulong prim;
	struct sl *sl;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(sl_ulong *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	sl = SL_PRIV(q);
	sl_save_state(sl);
	mt_save_state(sl->mt);
	switch (prim) {
	case SL_PDU_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "SL_PDU_IND <-");
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", sl_primname(prim));
		break;
	}
	switch (prim) {
	case LMI_INFO_ACK:
		rtn = lmi_info_ack(sl, q, mp);
		break;
	case LMI_OK_ACK:
		rtn = lmi_ok_ack(sl, q, mp);
		break;
	case LMI_ERROR_ACK:
		rtn = lmi_error_ack(sl, q, mp);
		break;
	case LMI_ENABLE_CON:
		rtn = lmi_enable_con(sl, q, mp);
		break;
	case LMI_DISABLE_CON:
		rtn = lmi_disable_con(sl, q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		rtn = lmi_optmgmt_ack(sl, q, mp);
		break;
	case LMI_ERROR_IND:
		rtn = lmi_error_ind(sl, q, mp);
		break;
	case LMI_STATS_IND:
		rtn = lmi_stats_ind(sl, q, mp);
		break;
	case LMI_EVENT_IND:
		rtn = lmi_event_ind(sl, q, mp);
		break;
	case SL_PDU_IND:
		rtn = sl_pdu_ind(sl, q, mp);
		break;
	case SL_LINK_CONGESTED_IND:
		rtn = sl_link_congested_ind(sl, q, mp);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		rtn = sl_link_congestion_ceased_ind(sl, q, mp);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		rtn = sl_retrieved_message_ind(sl, q, mp);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		rtn = sl_retrieval_complete_ind(sl, q, mp);
		break;
	case SL_RB_CLEARED_IND:
		rtn = sl_rb_cleared_ind(sl, q, mp);
		break;
	case SL_BSNT_IND:
		rtn = sl_bsnt_ind(sl, q, mp);
		break;
	case SL_IN_SERVICE_IND:
		rtn = sl_in_service_ind(sl, q, mp);
		break;
	case SL_OUT_OF_SERVICE_IND:
		rtn = sl_out_of_service_ind(sl, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		rtn = sl_remote_processor_outage_ind(sl, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		rtn = sl_remote_processor_recovered_ind(sl, q, mp);
		break;
	case SL_RTB_CLEARED_IND:
		rtn = sl_rtb_cleared_ind(sl, q, mp);
		break;
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		rtn = sl_retrieval_not_possible_ind(sl, q, mp);
		break;
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		rtn = sl_bsnt_not_retrievable_ind(sl, q, mp);
		break;
	case SL_OPTMGMT_ACK:
		rtn = sl_optmgmt_ack(sl, q, mp);
		break;
	case SL_NOTIFY_IND:
		rtn = sl_notify_ind(sl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_IND:
		rtn = sl_local_processor_outage_ind(sl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_RECOVERED_IND:
		rtn = sl_local_processor_recovered_ind(sl, q, mp);
		break;
	default:
		rtn = sl_other_ind(sl, q, mp);
		break;
	}
	if (rtn) {
		mt_restore_state(sl->mt);
		sl_restore_state(sl);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * sl_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (read queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static int
sl_m_sig(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);
	if (!mi_timer_valid(mp))
		return (0);
	sl = SL_PRIV(q);
	sl_save_state(sl);
	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = sl_t1_timeout(sl, q);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		sl_restore_state(sl);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * sl_m_copy: - process M_COPYIN or M_COPYOUT message
 * @q: active queue (read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 */
static int
sl_m_copy(queue_t *q, mblk_t *mp)
{
	/* FIXME: do more than pass along */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * sl_m_iocack: - process M_IOCACK or M_IOCNAK message
 * @q: active queue (read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 */
static int
sl_m_iocack(queue_t *q, mblk_t *mp)
{
	/* FIXME: do more than pass along */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * sl_m_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 *
 * Canonical module flush procedure for the read queue.
 */
static int
sl_m_flush(queue_t *q, mblk_t *mp)
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
 * sl_m_other: - process unrecognized STREAMS message
 * @q: active queue (read queue)
 * @mp: the unrecognized STREAMS message
 *
 * Unrecognized STREAMS messages are passed to the next module considering flow control.
 */
static int
sl_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastcall int
sl_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sl_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sl_m_sig(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return sl_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return sl_m_iocack(q, mp);
	case M_FLUSH:
		return sl_m_flush(q, mp);
	default:
		return sl_m_other(q, mp);
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
mt_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mt_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
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
static streamscall __hot_read int
sl_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sl_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_in int
sl_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_msg(q, mp))
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

static caddr_t mt_opens = NULL;

static streamscall int
mt_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(lmi_info_req_t), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm_cache(&mt_opens, mtp_mt_cachep, q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	p = PRIV(q);

	/* initialize private structure */
	p->r_priv.priv = p;
	p->r_priv.mt = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state = LMI_UNUSABLE;
	p->r_priv.oldstate = LMI_UNUSABLE;
	/* FIXME initialize some more */

	p->w_priv.priv = p;
	p->w_priv.sl = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state = MTPS_UNUSABLE;
	p->w_priv.oldstate = MTPS_UNUSABLE;
	/* FIXME initialize some more */

	/* issue an immediate information request */
	mp->b_datap->db_type = M_PCPROTO;
	((lmi_info_req_t *) mp->b_wptr)->lmi_primitive = LMI_INFO_REQ;
	mp->b_wptr += sizeof(lmi_info_req_t);

	qprocson(q);
	qreply(q, mp);
	return (0);
}
static streamscall int
mt_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	(void) p;
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm_cache(&mt_opens, mtp_mt_cachep, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURES, ALLOCATION, DEALLOCATION AND CACHE
 *
 *  =========================================================================
 */
static __unlikely int
mtp_init_caches(void)
{
	size_t sizeof_struct_mt = mi_open_size(sizeof(struct mt));
	size_t sizeof_struct_sl = mi_open_size(sizeof(struct sl));

	(void) mtp_init_caches;
	if (!mtp_mt_cachep &&
	    !(mtp_mt_cachep =
	      kmem_cache_create("mtp-mod:mtp_mt_cachep", sizeof_struct_mt, 0,
				SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp-mod:mtp_mt_cachep", MOD_NAME);
		goto failed_mt;
	} else
		cmn_err(CE_NOTE, "%s: initialized sl structure cache\n", MOD_NAME);
	if (!mtp_sl_cachep &&
	    !(mtp_sl_cachep =
	      kmem_cache_create("mtp-mod:mtp_sl_cachep", sizeof_struct_sl, 0,
				SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp-mod:mtp_sl_cachep", MOD_NAME);
		goto failed_sl;
	} else
		cmn_err(CE_NOTE, "%s: initialized sl structure cache\n", MOD_NAME);
      failed_sl:
	kmem_cache_destroy(mtp_mt_cachep);
	mtp_mt_cachep = NULL;
      failed_mt:
	return (-ENOMEM);
}

static __unlikely int
mtp_term_caches(void)
{
	int err = 0;

	(void) mtp_term_caches;
	if (mtp_mt_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_mt_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_mt_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_mt_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_mt_cachep);
#endif
		mtp_mt_cachep = NULL;
	}
	if (mtp_sl_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_sl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_sl_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_sl_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_sl_cachep);
#endif
		mtp_sl_cachep = NULL;
	}
	return (err);
}

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

static struct module_info mt_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat mt_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mt_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit mt_rinit = {
	.qi_putp = sl_put,
	.qi_srvp = sl_srv,
	.qi_qopen = mt_qopen,
	.qi_qclose = mt_qclose,
	.qi_minfo = &mt_minfo,
	.qi_mstat = &mt_rstat,
};

static struct qinit mt_winit = {
	.qi_putp = mt_put,
	.qi_srvp = mt_srv,
	.qi_minfo = &mt_minfo,
	.qi_mstat = &mt_wstat,
};

static struct streamtab mtp_modinfo = {
	.st_rdinit = &mt_rinit,
	.st_wrinit = &mt_winit,
};

static unsigned short modid = MOD_ID;

/*
 *  =========================================================================
 *
 *  KERNEL MODULE INITIALIZATION
 *
 *  =========================================================================
 */
#ifdef LINUX

#ifdef module_param
module_param(modid, short, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module id for MTP-MOD module.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif

static struct fmodsw mt_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mtp_modinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
mtp_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&mt_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
mtp_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&mt_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(mtp_modinit);
module_exit(mtp_modexit);

#endif				/* LINUX */
