/*****************************************************************************

 @(#) $RCSfile: sdt_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:35 $

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

 Last Modified $Date: 2007/07/14 01:13:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdt_pmod.c,v $
 Revision 0.9.2.1  2007/07/14 01:13:35  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: sdt_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:35 $"

static char const ident[] =
    "$RCSfile: sdt_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:35 $";

#define _MPS_SOURCE 1
#define _LFS_SOURCE 1

/*
 *  This is a module that can be pushed over one end of a STREAMS-based pipe to form a simulation of
 *  a pair of connected signalling data terminals.  Each end of the pipe presents the SDTI
 *  (Signalling Data Terminal Interface) and each end appears to be a completely independent
 *  signalling terminal.
 *
 *  There are two primary purposes for this module: 1) testing of the signalling link module that
 *  can be pushed onto one end of the pipe and the normal SS7 signalling test case executable
 *  approach can be performed to validate the signalling link module; and 2) to provide a pseudo
 *  signalling link that can be used to form internal connections between signalling points for
 *  testing MTP Level 3 and forming virtual SS7 networks.
 *
 *  This is an SDT pipemod driver for testing and use with pipes.  This module is pushed on one side
 *  of the pipe (after pipemod, or alone) to make a pipe appear as a directly connected pair of SDT
 *  drivers.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

#define SDT_DESCRIP	"SS7/SDT: (Signalling Data Terminal) STREAMS PIPE MODULE."
#define SDT_REVISION	"OpenSS7 $RCSfile: sdt_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:35 $A"
#define SDT_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define SDT_DEVICE	"Supports STREAMS-based Pipes."
#define SDT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDT_LICENSE	"GPL v2"
#define SDT_BANNER	SDT_DESCRIP	"\n" \
			SDT_REVISION	"\n" \
			SDT_COPYRIGHT	"\n" \
			SDT_DEVICE	"\n" \
			SDT_CONTACT	"\n"
#define SDT_SPLASH	SDT_DEVICE	" - " \
			SDT_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SDT_CONTACT);
MODULE_DESCRIPTION(SDT_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDT_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdt-pmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SDT_PMOD_MOD_ID		CONFIG_STREAMS_SDT_PMOD_MODID
#define SDT_PMOD_MOD_NAME	CONFIG_STREAMS_SDT_PMOD_NAME
#endif				/* LFS */

#ifndef SDT_PMOD_MOD_NAME
#define SDT_PMOD_MOD_NAME "sdt-pmod"
#endif				/* SDT_PMOD_MOD_NAME */

#ifndef SDT_PMOD_MOD_ID
#define SDT_PMOD_MOD_ID 0
#endif				/* SDT_PMOD_MOD_ID */

/*
 *  ========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  ========================================================================
 */

#define MOD_ID		SDT_PMOD_MOD_ID
#define MOD_NAME	SDT_PMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SDT_BANNER
#else				/* MODULE */
#define MOD_BANNER	SDT_SPLASH
#endif				/* MODULE */

STATIC struct module_info sdt_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

STATIC struct module_stat sdt_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat sdt_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  ========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  ========================================================================
 */

struct sdt_pair;

struct sdt {
	struct sdt_pair *pair;		/* thread pointer to pair */
	struct sdt *other;		/* other half of stream */
	uint max_sdu;			/* maximum SDU size */
	uint min_sdu;			/* minimum SDU size */
	uint header_len;		/* size of Level 1 header */
	uint ppa_style;			/* PPA style */
	uint hlen;			/* level 2 header length */
	uint mlen;			/* mask for length indicator */
	uint m_state;			/* management state */
	uint m_oldstate;		/* previous state */
	struct lmi_option option;	/* LMI protocol and variant options */
	struct {
		struct sdl_notify notify;	/* SDL notification options */
		struct sdl_config config;	/* SDL configuration options */
		struct sdl_statem statem;	/* SDL state machine variables */
		struct sdl_stats statsp;	/* SDL statistics periods */
		struct sdl_stats stats;		/* SDL statistics */
	} sdl;
	struct {
		struct sdt_notify notify;	/* SDT events */
		struct sdt_config config;	/* SDT configuration */
		struct sdt_statem statem;	/* SDT state machine */
		struct sdt_stats statsp;	/* SDT statistics periods */
		struct sdt_stats stats;	/* SDT statistics */
	} sdt;
};

struct sdt_pair {
	struct sdt r_priv;
	struct sdt w_priv;
};

#define SDT_PRIV(q) \
	(((q)->q_flag & QREADR) ? \
	 &((struct sdt_pair *)(q)->q_ptr)->r_priv : \
	 &((struct sdt_pair *)(q)->q_ptr)->w_priv)

#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

/**
 * sdt_statename: display LMI state name
 * @state: the state value to display
 */
static const char *
sdt_statename(int state)
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
		return ("LMI_????");
	}
}

/**
 * sdt_get_m_state: - get management state for private structure
 * @sdt: private structure
 */
static int
sdt_get_m_state(struct sdt *sdt)
{
	return (sdt->m_state);
}

/**
 * sdt_set_m_state: - set management state for private structure
 * @sdt: private structure
 * @q: active queue
 * @newstate: new state
 */
static int
sdt_set_m_state(struct sdt *sdt, queue_t *q, int newstate)
{
	int oldstate = sdt->m_state;

	if (newstate != oldstate) {
		sdt->m_state = newstate;
		mi_strlog(q, STRLOGST, SL_TRACE, "%s <- %s", sdt_statename(newstate),
			  sdt_statename(oldstate));
	}
	return (newstate);
}

/*
 *  ========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  SDT Provider -> SDT User primitives.
 *
 *  ========================================================================
 */
#if 0
/**
 * m_error: - issue M_ERROR for stream
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: error to indicate
 */
static int
m_error(struct sdt *s, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(q, 0, SL_ERROR, "<- M_ERROR %d", err);
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/**
 * lmi_info_ack: - generate LMI_INFO_ACK primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @msg: message to be freed on success
 */
static int
lmi_info_ack(struct sdt *s, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sdt_get_m_state(s);
		p->lmi_max_sdu = s->max_sdu;
		p->lmi_min_sdu = s->min_sdu;
		p->lmi_header_len = s->header_len;
		p->lmi_ppa_style = LMI_STYLE1;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_INFO_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_ok_ack: - generate LMI_OK_ACK primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @msg: message to be freed on success
 * @prim: correct primitive
 */
static int
lmi_ok_ack(struct sdt *s, queue_t *q, mblk_t *msg, int prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sdt_get_m_state(s)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = sdt_set_m_state(s, q, LMI_UNATTACHED);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = sdt_set_m_state(s, q, LMI_DISABLED);
			break;
		default:
			p->lmi_state = sdt_get_m_state(s);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_OK_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/**
 * lmi_error_ack: - generate LMI_ERROR_ACK primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @msg: message to be freed on success
 * @prim: primitive in error
 * @error: error number
 */
static noinline fastcall __unlikely int
lmi_error_ack(struct sdt *s, queue_t *q, mblk_t *msg, int prim, int error)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = error < 0 ? LMI_SYSERR : error;
		p->lmi_reason = error < 0 ? -error : 0;
		p->lmi_error_primitive = prim;
		p->lmi_state = sdt_set_m_state(s, q, s->m_oldstate);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_reply: - generate error acknolwegement as required
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @error: error to return (negative, put message back) (positive, error ack)
 */
static int
lmi_error_reply(struct sdt *s, queue_t *q, mblk_t *msg, int prim, int error)
{
	if (error == 0)
		freemsg(msg);
	else if (error > 0)
		return lmi_error_ack(s, q, msg, prim, error);
	return (error);
}

/**
 * lmi_enable_con: - generate LMI_ENABLE_CON primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @msg: message to be freed on success
 */
static int
lmi_enable_con(struct sdt *s, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = sdt_set_m_state(s, q, LMI_ENABLED);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_con: - generate LMI_DISABLE_CON primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @msg: message to be freed on success
 */
static int
lmi_disable_con(struct sdt *s, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = sdt_set_m_state(s, q, LMI_DISABLED);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_optmgmt_ack: - generate LMI_OPTMGMT_ACK primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @msg: message to be freed on success
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @flags: management flags
 */
static int
lmi_optmgmt_ack(struct sdt *s, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len, uint flags)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ind: - generate LMI_ERROR_IND primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @msg: message to be freed on success
 * @error: error to indicate
 * @state: state after error indication
 */
static int
lmi_error_ind(struct sdt *s, queue_t *q, mblk_t *msg, int error, uint state)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = error < 0 ? LMI_SYSERR : error;
		p->lmi_reason = error < 0 ? -error : 0;
		p->lmi_state = sdl_set_m_state(s, q, state);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_ERROR_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_stats_ind: - generate LMI_STATS_IND primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @msg: message to be freed on success
 */
static int
lmi_stats_ind(struct sdt *s, queue_t *q, queue_t *oq, mblk_t *msg)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = 0;
		p->lmi_timestamp = jiffies;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_STATS_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_event_ind: - generate LMI_EVENT_IND primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @msg: message to be freed on success
 * @oid: event object id
 * @level: severity
 * @inf_ptr: information pointer
 * @inf_len: information length
 */
static int
lmi_event_ind(struct sdt *s, queue_t *q, mblk_t *msg, uint oid, uint level, caddr_t inf_ptr, size_t inf_len)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + inf_len, BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_EVENT_IND;
		p->lmi_objectid = oid;
		p->lmi_timestamp = jiffies;
		p->lmi_severity = level;
		mp->b_wptr += sizeof(*p);
		bcopy(inf_ptr, mp->b_wptr, inf_len);
		mp->b_wptr += inf_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sdl_received_bits_ind: - issue SDL_RECEIVED_BITS_IND upstream
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data (bits)
 */
static int
sdl_received_bits_ind(struct sdl *s, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (bcanputnext(q, dp->b_band)) {
		sdl_received_bits_ind_t *p;
		mblk_t *mp;

		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdl_primitive = SDL_RECEIVED_BITS_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mp->b_cont = dp;
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- SDL_RECEIVED_BITS_IND");
			putnext(q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
#endif

#if 0
/**
 * sdl_disconnect_ind: - issue SDL_DISCONNECT_IND primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * SDL_DISCONNECT_IND is generated as a result of a SDL_DISCONNECT_REQ at the
 * other end of the pipe.  This primitive is issued along the same queue as
 * the SDL_DISCONNECT_REQ was received on.
 */
static int
sdl_disconnect_ind(struct spm *s, queue_t *q, mblk_t *msg)
{
	sdl_disconnect_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_DISCONNECT_IND;
		freemsg(msg);
		sdl_set_rx_state(s, SDL_STATE_IDLE);
		sdl_set_tx_state(s, SDL_STATE_IDLE);
		s->sdl.config.ifflags &= ~(SDL_IF_UP | SDL_IF_RX_RUNNING | SDL_IF_TX_RUNNING);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDT_DISCONNECT_IND");
		putnext(q, mp); /* pass to other side */
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sdt_rc_signal_unit_ind: - generate SDT_RC_SIGNAL_UNIT_IND primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @msg: message to free upon success
 * @dp: user data (su)
 */
static int
sdt_rc_signal_unit_ind(struct sdt *s, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (bcanputnext(q, dp->b_band)) {
		sdt_rc_signal_unit_ind_t *p;
		mblk_t *mp;

		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mp->b_cont = dp;
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- SDT_RC_SIGNAL_UNIT_IND");
			putnext(q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_rc_congestion_accept_ind: - generate SDT_RC_CONGESTION_ACCEPT_IND primitive
 * @sdt: private structure
 * @q: active queue (read or write queue)
 * @oq: output queue
 * @msg: message to be freed on success
 */
static int
sdt_rc_congestion_accept_ind(struct sdt *sdt, queue_t *q, queue_t *oq, mblk_t *msg)
{
	sdt_rc_congestion_accept_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDT_RC_CONGESTION_ACCEPT_IND");
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sdt_rc_congestion_discard_ind: - generate SDT_RC_CONGESTION_DISCARD_IND primitive
 * @sdt: private structure
 * @q: active queue (read or write queue)
 * @oq: output queue
 * @msg: message to be freed on success
 */
static int
sdt_rc_congestion_discard_ind(struct sdt *sdt, queue_t *q, queue_t *oq, mblk_t *msg)
{
	sdt_rc_congestion_discard_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDT_RC_CONGESTION_DISCARD_IND");
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sdt_rc_no_congestion_ind: - generate SDT_RC_NO_CONGESTION_IND primitive
 * @sdt: private structure
 * @q: active queue (read or write queue)
 * @oq: output queue
 * @msg: message to be freed on success
 */
static int
sdt_rc_no_congestion_ind(struct sdt *sdt, queue_t *q, queue_t *oq, mblk_t *msg)
{
	sdt_rc_no_congestion_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDT_RC_NO_CONGESTION_IND");
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 1
/**
 * sdt_iac_correct_su_ind: - generate SDT_IAC_CORRECT_SU_IND primitive
 * @sdt: private structure
 * @q: active queue (read or write queue)
 */
static int
sdt_iac_correct_su_ind(struct sdt *sdt, queue_t *q)
{
	sdt_iac_correct_su_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDT_IAC_CORRECT_SU_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sdt_iac_abort_proving_ind: - generate SDT_IAC_ABORT_PROVING_IND primitive
 * @sdt: private structure
 * @q: active queue (read or write queue)
 */
static int
sdt_iac_abort_proving_ind(struct sdt *sdt, queue_t *q)
{
	sdt_iac_abort_proving_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDT_IAC_ABORT_PROVING_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sdt_lsc_link_failure_ind: - generate SDT_LSC_LINK_FAILURE_IND primitive
 * @sdt: private structure
 * @q: active queue (read or write queue)
 */
static int
sdt_lsc_link_failure_ind(struct sdt *sdt, queue_t *q)
{
	sdt_lsc_link_failure_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDT_LSC_LINK_FAILURE_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sdt_txc_transmission_request_ind: - generate SDT_TXC_TRANSMISSION_REQUEST_IND primitive
 * @sdt: private structure
 * @q: active queue (read or write queue)
 * @oq: output queue
 * @msg: message to be freed on success
 */
static int
sdt_txc_transmission_request_ind(struct sdt *sdt, queue_t *q, queue_t *oq, mblk_t *msg)
{
	sdt_txc_transmission_request_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDT_TXC_TRANSMISSION_REQUEST_IND");
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  ========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  ========================================================================
 */

/* AERM */

/**
 * sdt_aerm_start: - start the AERM
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_aerm_start(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt->sdt.statem.Ca = 0;
	sdt->sdt.statem.aborted_proving = false;
	sdt->sdt.statem.aerm_state = SDT_STATE_MONITORING;
	freemsg(msg);
	return (0);
}

/**
 * sdt_aerm_stop: - stop the AERM
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_aerm_stop(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt->sdt.statem.aerm_state = SDT_STATE_IDLE;
	sdt->sdt.statem.Ti = sdt->sdt.config.Tin;
	freemsg(msg);
	return (0);
}

/**
 * sdt_aerm_set_ti_to_tie: - set AERM emergency
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_aerm_set_ti_to_tie(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE)
		sdt->sdt.statem.Ti = sdt->sdt.config.Tie;
	freemsg(msg);
	return (0);
}

/**
 * sdt_aerm_set_ti_to_tin: - set AERM normal
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_aerm_set_ti_to_tin(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE)
		sdt->sdt.statem.Ti = sdt->sdt.config.Tin;
	freemsg(msg);
	return (0);
}

/**
 * sdt_aerm_correct_su: - AERM correct signal unit
 * @sdt:private structure
 * @q: active queue
 */
static int
sdt_aerm_correct_su(struct sdt *sdt, queue_t *q)
{
	int err = 0;

	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE) {
		if (sdt->sdt.statem.aborted_proving) {
			if ((err = sdt_iac_correct_su_ind(sdt, q)) == 0) {
				sdt->sdt.statem.aborted_proving = false;
			}
		}
	}
	return (err);
}

/**
 * sdt_aerm_su_in_error: - AERM signal unit in error
 * @sdt:private structure
 * @q: active queue
 */
static int
sdt_aerm_su_in_error(struct sdt *sdt, queue_t *q)
{
	int err = 0;

	if (sdt->sdt.statem.aerm_state == SDT_STATE_MONITORING) {
		sdt->sdt.statem.Ca++;
		if (sdt->sdt.statem.Ca == sdt->sdt.statem.Ti) {
			sdt->sdt.statem.Ca--;
			if ((err = sdt_iac_abort_proving_ind(sdt, q)) == 0) {
				sdt->sdt.statem.aborted_proving = true;
				sdt->sdt.statem.aerm_state = SDT_STATE_IDLE;
			}
		}
	}
	return (err);
}

/* EIM */

/**
 * sdt_eim_start: - start EIM
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_eim_start(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt->sdt.statem.Ce = 0;
	sdt->sdt.statem.interval_error = false;
	sdt->sdt.statem.su_received = false;
	/* timer start t8 */
	sdt->sdt.statem.eim_state = SDT_STATE_MONITORING;
	freemsg(msg);
	return (0);
}

/**
 * sdt_eim_stop: - stop EIM
 * @sdt:private structure
 * @q: active queue
 */
static void
sdt_eim_stop(struct sdt *sdt, queue_t *q)
{
	sdt->sdt.statem.eim_state = SDT_STATE_IDLE;
	/* timer stop t8 */
}
static int
sdt_eim_correct_su(struct sdt *sdt, queue_t *q)
{
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING) {
		sdt->sdt.statem.su_received = true;
	}
	return (0);
}
static int
sdt_eim_su_in_error(struct sdt *sdt, queue_t *q)
{
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING) {
		sdt->sdt.statem.interval_error = true;
	}
	return (0);
}

/* SUERM */

/**
 * sdt_suerm_start: - start SUERM/EIM
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_suerm_start(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	if (sdt->option.popt & SS7_POPT_HSL) {
		return sdt_eim_start(sdt, q, msg);
	} else {
		sdt->sdt.statem.Cs = 0;
		sdt->sdt.statem.Ns = 0;
		sdt->sdt.statem.suerm_state = SDT_STATE_IN_SERVICE;
		freemsg(msg);
		return (0);
	}
}

/**
 * sdt_suerm_stop: - stop SUERM/EIM
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_suerm_stop(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_eim_stop(sdt, q);
	sdt->sdt.statem.suerm_state = SDT_STATE_IDLE;
	freemsg(msg);
	return (0);
}

/**
 * sdt_suerm_correct_su: - SUERM correct signal unit
 * @sdt:private structure
 * @q: active queue
 */
static int
sdt_suerm_correct_su(struct sdt *sdt, queue_t *q)
{
	if (sdt->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		sdt->sdt.statem.Ns++;
		if (sdt->sdt.statem.Ns >= sdt->sdt.config.D) {
			sdt->sdt.statem.Ns = 0;
			if (sdt->sdt.statem.Cs)
				sdt->sdt.statem.Cs--;
		}
	}
	return (0);
}

/**
 * sdt_suerm_su_in_error: - SUERM signal unit in error
 * @sdt:private structure
 * @q: active queue
 */
static int
sdt_suerm_su_in_error(struct sdt *sdt, queue_t *q)
{
	int err = 0;

	if (sdt->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		sdt->sdt.statem.Cs++;
		if (sdt->sdt.statem.Cs >= sdt->sdt.config.T) {
			if ((err = sdt_lsc_link_failure_ind(sdt, q)) != 0) {
				sdt->sdt.statem.Cs--;
			} else {
				sdt->sdt.statem.Ca--;
				sdt->sdt.statem.suerm_state = SDT_STATE_IDLE;
			}
		} else {
			sdt->sdt.statem.Ns++;
			if (sdt->sdt.statem.Ns >= sdt->sdt.config.D) {
				sdt->sdt.statem.Ns = 0;
				if (sdt->sdt.statem.Cs)
					sdt->sdt.statem.Cs--;
			}
		}
	}
	return (err);
}

/* DAEDR */

/**
 * sdt_daedr_start: - start DAEDR
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_daedr_start(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt->sdt.statem.octet_counting_mode = false;
	sdt->sdt.statem.daedr_state = SDT_STATE_IN_SERVICE;
	sdt->sdl.statem.rx_state = SDL_STATE_IN_SERVICE;
	sdt->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_RX_RUNNING);
	freemsg(msg);
	return (0);
}

/**
 * sdt_daedr_correct_su: - DAEDR correct signal unit
 * @sdt:private structure
 * @q: active queue
 */
static int
sdt_daedr_correct_su(struct sdt *sdt, queue_t *q)
{
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING)
		return sdt_eim_correct_su(sdt, q);
	if (sdt->sdt.statem.suerm_state == SDT_STATE_MONITORING)
		return sdt_suerm_correct_su(sdt, q);
	if (sdt->sdt.statem.aerm_state == SDT_STATE_MONITORING)
		return sdt_aerm_correct_su(sdt, q);

	return (0);
}

/**
 * sdt_daedr_su_in_error: - DAEDR signal unit in error
 * @sdt:private structure
 * @q: active queue
 */
static int
sdt_daedr_su_in_error(struct sdt *sdt, queue_t *q)
{
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING)
		return sdt_eim_su_in_error(sdt, q);
	if (sdt->sdt.statem.suerm_state == SDT_STATE_MONITORING)
		return sdt_suerm_su_in_error(sdt, q);
	if (sdt->sdt.statem.aerm_state == SDT_STATE_MONITORING)
		return sdt_aerm_su_in_error(sdt, q);

	return (0);
}

/**
 * sdt_daedr_received_bits: - DAEDR receive bits
 * @sdt:private structure
 * @q: active queue
 * @mp: received bits
 */
static int
sdt_daedr_received_bits(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	int err, susize, li;

	if (sdt->sdt.statem.daedr_state == SDT_STATE_IDLE) {
		freemsg(mp);
		goto done;
	}

	susize = msgdsize(mp);

	if (susize > sdt->max_sdu) {
		if ((err = sdt_daedr_su_in_error(sdt, q)) != 0)
			goto error;
		sdt->sdl.stats.rx_octets += susize;
		sdt->sdt.stats.rx_sus++;
		sdt->sdt.stats.rx_bytes += susize;
		sdt->sdt.stats.rx_sus_in_error++;
		sdt->sdt.stats.rx_frame_too_long++;
		freemsg(mp);
		goto done;
	}
	if (susize < sdt->min_sdu) {
		if ((err = sdt_daedr_su_in_error(sdt, q)) != 0)
			goto error;
		sdt->sdl.stats.rx_octets += susize;
		sdt->sdt.stats.rx_sus++;
		sdt->sdt.stats.rx_bytes += susize;
		sdt->sdt.stats.rx_sus_in_error++;
		sdt->sdt.stats.rx_frame_too_short++;
		freemsg(mp);
		goto done;
	}
	{
		int b, m;

		for (li = 0, b = sdt->hlen - 1, m = sdt->mlen; b >= 0 && m != 0;
		     b--, m >>= 8, li <<= 8) {
			li |= (mp->b_rptr[b] & m);
		}
	}
	if (li != susize - sdt->hlen && (li != sdt->mlen || li >= susize - sdt->hlen)) {
		if ((err = sdt_daedr_su_in_error(sdt, q)) != 0)
			goto error;
		sdt->sdl.stats.rx_octets += susize;
		sdt->sdt.stats.rx_sus++;
		sdt->sdt.stats.rx_bytes += susize;
		sdt->sdt.stats.rx_sus_in_error++;
		sdt->sdt.stats.rx_length_error++;
		freemsg(mp);
		goto done;
	}
	err = -EBUSY;
	if (!bcanputnext(q, mp->b_band)) {
		sdt->sdl.stats.rx_overruns += susize;
		sdt->sdl.stats.rx_buffer_overflows++;
		sdt->sdt.stats.rx_overruns += susize;
		sdt->sdt.stats.rx_buffer_overflows++;
		goto ebusy;
	}
	if ((err = sdt_daedr_correct_su(sdt, q)) != 0)
		goto error;
	sdt->sdl.stats.rx_octets += susize;
	sdt->sdt.stats.rx_sus++;
	sdt->sdt.stats.rx_bytes += susize;
	putnext(q, mp);
	goto done;
      done:
	return (0);
      error:
	return (err);
      ebusy:
	err = -EBUSY;
	goto error;
}

/* DAEDT */

/**
 * sdt_daedt_start: - start DAEDT
 * @sdt:private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sdt_daedt_start(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt->sdt.statem.daedt_state = SDT_STATE_IN_SERVICE;
	sdt->sdl.statem.tx_state = SDL_STATE_IN_SERVICE;
	sdt->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING);
	freemsg(msg);
	return (0);
}

/**
 * sdt_daedt_bits_for_transmission: - DAEDT transmitted bit
 * @sdt:private structure
 * @q: active queue
 * @mp: bits to transmit
 */
static int
sdt_daedt_bits_for_transmission(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	int err = 0, susize;

	if (sdt->sdt.statem.daedt_state == SDT_STATE_IN_SERVICE) {
		susize = mp ? msgdsize(mp) : 0;
		if (susize <= sdt->max_sdu && susize >= sdt->min_sdu) {
			if ((err = sdt_daedr_received_bits(sdt->other, q, mp)) == 0) {
				sdt->sdl.stats.tx_octets += susize;
				sdt->sdt.stats.tx_bytes += susize;
				sdt->sdt.stats.tx_sus++;
			}
		} else {
			sdt->sdt.stats.tx_bytes += susize;
			sdt->sdt.stats.tx_sus++;
			sdt->sdt.stats.tx_sus_in_error++;
			err = LMI_BADFRAME;
		}
	} else
		freemsg(mp);
	return (err);
}

/*
 *  ========================================================================
 *
 *  PROCESSING RECEIVED PRIMITIVES
 *
 *  SDT User -> SDT Provider primitives.
 *
 *  ========================================================================
 */

/**
 * lmi_info_req: - process LMI_INFO_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
lmi_info_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if ((err = lmi_info_ack(s, q, mp)) != 0)
		goto error;
	return (0);
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_INFO_REQ, err);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 *
 * SDT-PMOD is a Style 1 driver.  When the pipe is created and the module pushed, the
 * communications medium and physical point of attachment is implied by the stream.
 */
static int
lmi_attach_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_UNATTACHED && sdt_get_m_state(s) != LMI_DISABLED)
		goto outstate;
	sdt_set_m_state(s, q, LMI_ATTACH_PENDING);
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_ATTACH_REQ, err);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 *
 * SDT-PMOD is a Style 1 driver.  When the pipe is created and the module pushed, the
 * communications medium and physical point of attachment is implied by the stream.
 */
static int
lmi_detach_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_UNATTACHED && sdt_get_m_state(s) != LMI_DISABLED)
		goto outstate;
	sdt_set_m_state(s, q, LMI_DETACH_PENDING);
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_DETACH_REQ, err);
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 *
 * Enabling a SDT-PMOD stream is simply marking it a enabled on the appropriate side of the
 * connection and returning a confirmation primitive.
 */
static int
lmi_enable_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_DISABLED && sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	sdt_set_m_state(s, q, LMI_ENABLE_PENDING);
	if ((err = lmi_enable_con(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_ENABLE_REQ, err);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 *
 * Disabling an SDT-PMOD stream is simply marking it disabled on the appropriate side of the
 * connection and returning a confirmation primitive.
 */
static int
lmi_disable_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_DISABLED && sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	sdt_set_m_state(s, q, LMI_DISABLE_PENDING);
	if ((err = lmi_disable_con(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_DISABLE_REQ, err);
}

/**
 * lmi_optmgmt_req: - process LMI_OPTMGMT_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
lmi_optmgmt_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_OPTMGMT_REQ, err);
}

#if 0
/**
 * sdl_bits_for_transmission_req: - transmit bits
 * @s: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sdl_bits_for_transmission_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	sdl_bits_for_transmission_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if (s->sdl.statem.tx_state == SDL_STATE_IDLE)
		goto discard;
	if ((err = sdl_layer1_bits_for_transmission(s, q, mp->b_cont)) == 0) {
		freeb(mp);
		return (0);
	}
	goto error;
      discard:
	err = 0;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDL_BITS_FOR_TRANSMISSION_REQ, err);
}

/**
 * sdl_connect_req: - process SDL_CONNECT_REQ primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sdl_connect_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	sdl_connect_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdl_connect(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDL_CONNECT_REQ, err);
}

/**
 * sdl_disconnect_req: - process SDL_DISCONNECT_REQ primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sdl_disconnect_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	sdl_disconnect_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdl_disconnect(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDL_DISCONNECT_REQ, err);
}
#endif

#if 1
/**
 * sdt_daedt_trasnmission_req: - process SDT_DAEDT_TRANSMISSION_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_daedt_transmission_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	union SDT_primitives *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(p->daedt_transmission_req))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if (s->sdt.statem.daedt_state == SDT_STATE_IDLE)
		goto discard;
	if ((err = sdt_daedt_bits_for_transmission(s, q, mp->b_cont)) == 0) {
		freeb(mp);
		return (0);
	}
	goto error;
      discard:
	err = 0;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_DAEDT_TRANSMISSION_REQ, err);
}

/**
 * sdt_daedt_start_req: - process SDT_DAEDT_START_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_daedt_start_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_daedt_start_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_daedt_start(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_DAEDT_START_REQ, err);
}

/**
 * sdt_daedr_start_req: - process SDT_DAEDR_START_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_daedr_start_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_daedr_start_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_daedr_start(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_DAEDR_START_REQ, err);
}

/**
 * sdt_aerm_start_req: - process SDT_AERM_START_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_aerm_start_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_aerm_start_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_aerm_start(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_AERM_START_REQ, err);
}

/**
 * sdt_aerm_stop_req: - process SDT_AERM_STOP_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_aerm_stop_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_aerm_stop_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_aerm_stop(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_AERM_STOP_REQ, err);
}

/**
 * sdt_aerm_set_ti_to_tin_req: - process SDT_AERM_SET_TI_TO_TIN_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_aerm_set_ti_to_tin_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_aerm_set_ti_to_tin_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_aerm_set_ti_to_tin(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_AERM_SET_TI_TO_TIN_REQ, err);
}

/**
 * sdt_aerm_set_ti_to_tie_req: - process SDT_AERM_SET_TI_TO_TIE_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_aerm_set_ti_to_tie_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_aerm_set_ti_to_tie_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_aerm_set_ti_to_tie(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_AERM_SET_TI_TO_TIE_REQ, err);
}

/**
 * sdt_suerm_start_req: - process SDT_SUERM_START_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_suerm_start_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_suerm_start_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_suerm_start(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_SUERM_START_REQ, err);
}

/**
 * sdt_suerm_stop_req: - process SDT_SUERM_STOP_REQ primitive
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
sdt_suerm_stop_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_suerm_stop_req_t *p;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdt_get_m_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_suerm_stop(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_SUERM_STOP_REQ, err);
}
#endif

/**
 * lmi_other_req: - process unknown primitive 
 * @s: private structure
 * @q: active queue (read or write queue)
 * @mp: the primitive
 */
static int
lmi_other_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	lmi_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, *p, err);
}

#if 0
/*
 *  M_RSE, M_PCRSE Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_m_pcrse(queue_t *q, mblk_t *mp)
{
	int rtn;

	switch (*(ulong *) mp->b_rptr) {
	default:
		rtn = -EFAULT;
		break;
	}
	return (rtn);
}
#endif

/*
 *  =======================================================================
 *
 *  INPUT OUTPUT CONTROLS
 *
 *  =======================================================================
 */

/**
 * lmi_ioctl: - process LMI M_IOCTL message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 * @dp: data part
 *
 * This is step 1 of the LMI input-output control operation.  Step 1 consists of a copyin operation
 * for SET operations and a copyout operation for GET operations.
 */
static int
lmi_ioctl(struct sdt *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(LMI_IOCGOPTIONS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(LMI_IOCGOPTIONS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->option), false)))
			goto enobufs;
		bcopy(&s->option, bp->b_rptr, sizeof(s->option));
		break;
	case _IOC_NR(LMI_IOCSOPTIONS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(LMI_IOCSOPTIONS)");
		size = sizeof(s->option);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}
/**
 * lmi_copyin: - process LMI M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the implicit or explicit copyin
 * operation.  We can now perform sets and commits.  All SET operations also include a last copyout
 * step that copies out the information actually set.
 */
static int
lmi_copyin(struct sdt *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(LMI_IOCSOPTIONS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(LMI_IOCSOPTIONS)");
		/* FIXME: check options */
		bcopy(bp->b_rptr, &s->option, sizeof(s->option));
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
 * lmi_copyout: - process LMI M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
lmi_copyout(struct sdt *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}



/**
 * sdl_testconfig: - test SDL configuration for validity
 * @s: private structure
 * @arg: configuration to test
 */
static int
sdl_testconfig(struct sdt *s, struct sdl_config *arg)
{
	int err = 0;

	if (arg->ifflags)
		err = EINVAL;
	if (arg->iftype != SDL_TYPE_NONE)
		err = EINVAL;
	if (arg->ifmode != SDL_MODE_NONE)
		err = EINVAL;
	if (arg->ifgmode != SDL_GMODE_NONE)
		err = EINVAL;
	if (arg->ifgtype != SDL_GMODE_NONE)
		err = EINVAL;
	if (arg->ifgcrc != SDL_GCRC_NONE)
		err = EINVAL;
	if (arg->ifclock != SDL_CLOCK_NONE)
		err = EINVAL;
	if (arg->ifcoding != SDL_CODING_NONE)
		err = EINVAL;
	if (arg->ifframing != SDL_FRAMING_NONE)
		err = EINVAL;
	if (arg->iftxlevel == 0)
		arg->iftxlevel = s->sdl.config.iftxlevel;
	else if (arg->iftxlevel <= 16)
		arg->iftxlevel--;
	else
		err = EINVAL;
	return (err);
}

/**
 * sdl_setnotify: - set SDL notification bits
 * @s: private structure
 * @arg: notification arguments
 */
static int
sdl_setnotify(struct sdt *s, struct sdl_notify *arg)
{
	if (arg->events &
	    ~(SDL_EVT_LOST_SYNC | SDL_EVT_SU_ERROR | SDL_EVT_TX_FAIL | SDL_EVT_RX_FAIL))
		return (EINVAL);
	arg->events = arg->events | s->sdl.notify.events;
	return (0);
}

/**
 * sdl_clrnotify: - clear SDL notification bits
 * @s: private structure
 * @arg: notification arguments
 */
static int
sdl_clrnotify(struct sdt *s, struct sdl_notify *arg)
{
	if (arg->events &
	    ~(SDL_EVT_LOST_SYNC | SDL_EVT_SU_ERROR | SDL_EVT_TX_FAIL | SDL_EVT_RX_FAIL))
		return (EINVAL);
	arg->events = ~arg->events & s->sdl.notify.events;
	return (0);
}

/**
 * sdl_ioctl: - process SDL M_IOCTL message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 * @dp: data part
 *
 * This is step 1 of the SDL input-output control operation.  Step 1 consists of a copyin operation
 * for SET operations and a copyout operation for GET operations.
 */
static int
sdl_ioctl(struct sdt *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDL_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSCONFIG)");
		size = sizeof(s->option);
		break;
	case _IOC_NR(SDL_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGCONFIG)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.config), false)))
			goto enobufs;
		bcopy(&s->sdl.config, bp->b_rptr, sizeof(s->sdl.config));
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCTCONFIG)");
		size = sizeof(s->sdl.config);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCCONFIG)");
		size = sizeof(s->sdl.config);
		break;
	case _IOC_NR(SDL_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATEM)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.statem), false)))
			goto enobufs;
		bcopy(&s->sdl.statem, bp->b_rptr, sizeof(s->sdl.statem));
		break;
	case _IOC_NR(SDL_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCMRESET)");
		/* FIXME reset the state machine. */
		break;
	case _IOC_NR(SDL_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATSP)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.statsp), false)))
			goto enobufs;
		bcopy(&s->sdl.statsp, bp->b_rptr, sizeof(s->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSSTATSP)");
		size = sizeof(s->sdl.statsp);
		break;
	case _IOC_NR(SDL_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.stats), false)))
			goto enobufs;
		bcopy(&s->sdl.stats, bp->b_rptr, sizeof(s->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.stats), false)))
			goto enobufs;
		bcopy(&s->sdl.stats, bp->b_rptr, sizeof(s->sdl.stats));
		bzero(&s->sdl.stats, sizeof(s->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGNOTIFY)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.notify), false)))
			goto enobufs;
		bcopy(&s->sdl.notify, bp->b_rptr, sizeof(s->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSNOTIFY)");
		size = sizeof(s->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCNOTIFY)");
		size = sizeof(s->sdl.notify);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/**
 * sdl_copyin: - process SDL M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the implicit or explicit copyin
 * operation.  We can now perform sets and commits.  All SET operations also include a last copyout
 * step that copies out the information actually set.
 */
static int
sdl_copyin(struct sdt *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDL_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSCONFIG)");
		if ((err = sdl_testconfig(s, (struct sdl_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdl.config, sizeof(s->sdl.config));
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCTCONFIG)");
		err = sdl_testconfig(s, (struct sdl_config *) bp->b_rptr);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCCCONFIG)");
		if ((err = sdl_testconfig(s, (struct sdl_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdl.config, sizeof(s->sdl.config));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSSTATSP)");
		bcopy(bp->b_rptr, &s->sdl.statsp, sizeof(s->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSNOTIFY)");
		if ((err = sdl_setnotify(s, (struct sdl_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdl.notify, sizeof(s->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCCNOTIFY)");
		if ((err = sdl_clrnotify(s, (struct sdl_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdl.notify, sizeof(s->sdl.notify));
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
 * sdl_copyout: - process SDL M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
sdl_copyout(struct sdt *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

#if 1
/**
 * sdt_testconfig: - test SDT configuration
 * @s: private structure
 * @arg: configuration argument
 */
static int
sdt_testconfig(struct sdt *s, struct sdt_config *arg)
{
	/* FIXME: check configuration */
	return (0);
}

/**
 * sdt_setnotify: - set SDT notifications
 * @s: private structure
 * @arg: notification argument
 */
static int
sdt_setnotify(struct sdt *s, struct sdt_notify *arg)
{
	if (arg->events &
	    ~(SDT_EVT_LOST_SYNC | SDT_EVT_SU_ERROR | SDT_EVT_TX_FAIL | SDT_EVT_RX_FAIL |
	      SDT_EVT_CARRIER))
		return (EINVAL);
	arg->events = arg->events | s->sdt.notify.events;
	return (0);
}

/**
 * sdt_clrnotify: - clear SDT notifications
 * @s: private structure
 * @arg: notification argument
 */
static int
sdt_clrnotify(struct sdt *s, struct sdt_notify *arg)
{
	if (arg->events &
	    ~(SDT_EVT_LOST_SYNC | SDT_EVT_SU_ERROR | SDT_EVT_TX_FAIL | SDT_EVT_RX_FAIL |
	      SDT_EVT_CARRIER))
		return (EINVAL);
	arg->events = ~arg->events & s->sdt.notify.events;
	return (0);
}

/**
 * sdt_ioctl: - process SDT M_IOCTL message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 * @dp: data part
 *
 * This is step 1 of the SDT input-output control operation.  Step 1 consists of a copyin operation
 * for SET operations and a copyout operation for GET operations.
 */
static int
sdt_ioctl(struct sdt *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDT_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSCONFIG)");
		size = sizeof(s->option);
		break;
	case _IOC_NR(SDT_IOCGCONFIG):
	{
		struct sdt_config *arg;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGCONFIG)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(*arg), false)))
			goto enobufs;
		arg = (typeof(arg)) bp->b_rptr;
		*arg = s->sdt.config;
		break;
	}
	case _IOC_NR(SDT_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCTCONFIG)");
		size = sizeof(s->sdt.config);
		break;
	case _IOC_NR(SDT_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCCONFIG)");
		size = sizeof(s->sdt.config);
		break;
	case _IOC_NR(SDT_IOCGSTATEM):
	{
		struct sdt_statem *arg;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATEM)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(*arg), false)))
			goto enobufs;
		arg = (typeof(arg)) bp->b_rptr;
		*arg = s->sdt.statem;
		break;
	}
	case _IOC_NR(SDT_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCMRESET)");
		/* FIXME reset the state machine. */
		size = 0;
		break;
	case _IOC_NR(SDT_IOCGSTATSP):
	{
		struct sdt_stats *arg;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATSP)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(*arg), false)))
			goto enobufs;
		arg = (typeof(arg)) bp->b_rptr;
		*arg = s->sdt.statsp;
		break;
	}
	case _IOC_NR(SDT_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSSTATSP)");
		size = sizeof(s->sdt.statsp);
		break;
	case _IOC_NR(SDT_IOCGSTATS):
	{
		struct sdt_stats *arg;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(*arg), false)))
			goto enobufs;
		arg = (typeof(arg)) bp->b_rptr;
		*arg = s->sdt.stats;
		break;
	}
	case _IOC_NR(SDT_IOCCSTATS):
	{
		struct sdt_stats *arg;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(*arg), false)))
			goto enobufs;
		arg = (typeof(arg)) bp->b_rptr;
		*arg = s->sdt.stats;
		bzero(&s->sdt.stats, sizeof(*arg));
		break;
	}
	case _IOC_NR(SDT_IOCGNOTIFY):
	{
		struct sdt_notify *arg;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGNOTIFY)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(*arg), false)))
			goto enobufs;
		arg = (typeof(arg)) bp->b_rptr;
		*arg = s->sdt.notify;
		break;
	}
	case _IOC_NR(SDT_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSNOTIFY)");
		size = sizeof(s->sdt.notify);
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCNOTIFY)");
		size = sizeof(s->sdt.notify);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
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
 * sdt_copyin: - process SDT M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the implicit or explicit copyin
 * operation.  We can now perform sets and commits.  All SET operations also include a last copyout
 * step that copies out the information actually set.
 */
static int
sdt_copyin(struct sdt *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDT_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSCONFIG)");
		if ((err = sdt_testconfig(s, (struct sdt_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdt.config, sizeof(s->sdt.config));
		break;
	case _IOC_NR(SDT_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCTCONFIG)");
		err = sdt_testconfig(s, (struct sdt_config *) bp->b_rptr);
		break;
	case _IOC_NR(SDT_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCCCONFIG)");
		if ((err = sdt_testconfig(s, (struct sdt_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdt.config, sizeof(s->sdt.config));
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSSTATSP)");
		bcopy(bp->b_rptr, &s->sdt.statsp, sizeof(s->sdt.statsp));
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSNOTIFY)");
		if ((err = sdt_setnotify(s, (struct sdt_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdt.notify, sizeof(s->sdt.notify));
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCCNOTIFY)");
		if ((err = sdt_clrnotify(s, (struct sdt_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdt.notify, sizeof(s->sdt.notify));
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
 * sdt_copyout: - process SDT M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
sdt_copyout(struct sdt *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}
#endif

/*
 *  =======================================================================
 *
 *  PUT AND SERVICE PROCEDURES
 *
 *  =======================================================================
 */

/**
 * sdt_m_data_slow: - process M_DATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static fastcall int
sdt_m_data_slow(struct sdt *s, queue_t *q, mblk_t *mp)
{
	return sdt_daedt_bits_for_transmission(s->other, q, mp);
}

/**
 * sdt_m_proto: - process M_(PC)PROTO message
 * @s: private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static fastcall int
sdt_m_proto(struct sdt *s, queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);

	s->m_oldstate = sdt_get_m_state(s);

	switch (*(lmi_ulong *) mp->b_rptr) {
	case LMI_INFO_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> LMI_INFO_REQ");
		rtn = lmi_info_req(s, q, mp);
		break;
	case LMI_ATTACH_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> LMI_ATTACH_REQ");
		rtn = lmi_attach_req(s, q, mp);
		break;
	case LMI_DETACH_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> LMI_DETACH_REQ");
		rtn = lmi_detach_req(s, q, mp);
		break;
	case LMI_ENABLE_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> LMI_ENABLE_REQ");
		rtn = lmi_enable_req(s, q, mp);
		break;
	case LMI_DISABLE_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> LMI_DISABLE_REQ");
		rtn = lmi_disable_req(s, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> LMI_OPTMGMT_REQ");
		rtn = lmi_optmgmt_req(s, q, mp);
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> LMI_????_??");
		rtn = lmi_other_req(s, q, mp);
		break;
#if 0
	case SDL_BITS_FOR_TRANSMISSION_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> SDL_BITS_FOR_TRANSMISSION_REQ");
		rtn = sdl_bits_for_transmission_req(s, q, mp);
		break;
	case SDL_CONNECT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDL_CONNECT_REQ");
		rtn = sdl_connect_req(s, q, mp);
		break;
	case SDL_DISCONNECT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDL_DISCONENCT_REQ");
		rtn = sdl_disconncect_req(s, q, mp);
		break;
#endif
#if 1
	case SDT_DAEDT_TRANSMISSION_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_DAEDT_TRANSMISSION_REQ");
		rtn = sdt_daedt_transmission_req(s, q, mp);
		break;
	case SDT_DAEDT_START_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_DAEDT_START_REQ");
		rtn = sdt_daedt_start_req(s, q, mp);
		break;
	case SDT_DAEDR_START_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_DAEDR_START_REQ");
		rtn = sdt_daedr_start_req(s, q, mp);
		break;
	case SDT_AERM_START_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_AERM_START_REQ");
		rtn = sdt_aerm_start_req(s, q, mp);
		break;
	case SDT_AERM_STOP_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_AERM_STOP_REQ");
		rtn = sdt_aerm_stop_req(s, q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_AERM_SET_TI_TO_TIN_REQ");
		rtn = sdt_aerm_set_ti_to_tin_req(s, q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_AERM_SET_TI_TO_TIE_REQ");
		rtn = sdt_aerm_set_ti_to_tie_req(s, q, mp);
		break;
	case SDT_SUERM_START_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_SUERM_START_REQ");
		rtn = sdt_suerm_start_req(s, q, mp);
		break;
	case SDT_SUERM_STOP_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> SDT_SUERM_STOP_REQ");
		rtn = sdt_suerm_stop_req(s, q, mp);
		break;
#endif
	}
	if (rtn)
		sdt_set_m_state(s, q, s->m_oldstate);
	mi_unlock(priv);
	return (rtn);
}

/**
 * sdt_m_flush: - process M_FLUSH message
 * @q: active queue
 * @mp: the M_FLUSH message
 *
 * Avoid having to push pipemod.  If we are the topmost module over a pipe twist then perform the
 * actions of pipemod.  This means that the sdt-pmod module must be pushed over the same side of a
 * pipe as pipemod if pipemod is pushed at all.
 */
static fastcall int
sdt_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	if (!SAMESTR(q)) {
		switch (mp->b_rptr[0]) {
		case FLUSHW:
			mp->b_rptr[0] = FLUSHR;
			break;
		case FLUSHR:
			mp->b_rptr[0] = FLUSHW;
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

/**
 * sdt_m_ioctl: - process M_IOCTL message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 */
static fastcall int
sdt_m_ioctl(struct sdt *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	caddr_t priv;
	int err = 0;

	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if ((priv = mi_trylock(q)) == NULL)
		return (-EAGAIN);

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	default:
	case __SID:
		mi_copy_done(q, mp, EINVAL);
		break;
	case LMI_IOC_MAGIC:
		err = lmi_ioctl(s, q, mp);
		break;
	case SDL_IOC_MAGIC:
		err = sdl_ioctl(s, q, mp);
		break;
#if 1
	case SDT_IOC_MAGIC:
		err = sdt_ioctl(s, q, mp);
		break;
#endif
	}
	mi_unlock(priv);
	return (err);
}

/**
 * sdt_m_iocdata: - process M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 */
static fastcall int
sdt_m_iocdata(struct sdt *s, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	caddr_t priv;
	int err = 0;
	mblk_t *dp;

	if ((priv = mi_trylock(q)) == NULL)
		return (-EAGAIN);

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case LMI_IOC_MAGIC:
			err = lmi_copyin(s, q, mp, dp);
			break;
		case SDL_IOC_MAGIC:
			err = sdl_copyin(s, q, mp, dp);
			break;
#if 1
		case SDT_IOC_MAGIC:
			err = sdt_copyin(s, q, mp, dp);
			break;
#endif
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case LMI_IOC_MAGIC:
			err = lmi_copyout(s, q, mp, dp);
			break;
		case SDL_IOC_MAGIC:
			err = sdl_copyout(s, q, mp, dp);
			break;
#if 1
		case SDT_IOC_MAGIC:
			err = sdt_copyout(s, q, mp, dp);
			break;
#endif
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		break;
	}
	mi_unlock(priv);
	return (err);
}

#ifndef DB_TYPE
#define DB_TYPE(mp) (mp->b_datap->db_type)
#endif

/**
 * sdt_m_other: - process other message
 * @q: active queue
 * @mp: the message
 *
 * Simply pass unrecognized messages along.
 */
static fastcall int
sdt_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
sdt_msg_slow(struct sdt *s, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sdt_m_data_slow(s, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sdt_m_proto(s, q, mp);
	case M_FLUSH:
		return sdt_m_flush(q, mp);
	case M_IOCTL:
		return sdt_m_ioctl(s, q, mp);
	case M_IOCDATA:
		return sdt_m_iocdata(s, q, mp);
#if 0
	case M_RSE:
	case M_PCRSE:
		return sdl_m_pcrse(s, q, mp);
#endif
	default:
		return sdt_m_other(q, mp);
	}
}

/**
 * sdt_m_data: - process M_DATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
sdt_m_data(struct sdt *s, queue_t *q, mblk_t *mp)
{
	return sdt_m_data_slow(s, q, mp);
}

/**
 * sdt_msg: - process message
 * @q: active queue
 * @mp: the message to process
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
sdt_msg(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		return sdt_m_data(s, q, mp);
	return sdt_msg_slow(s, q, mp);
}

/**
 * sdt_putp: - cannonical put procedure
 * @q: active queue
 * @mp: message to put
 */
static streamscall __hot_in int
sdt_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sdt_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * sdt_srvp: - cannonical service procedure
 * @q: active queue
 * @mp: message to put
 */
static streamscall __hot_read int
sdt_srvp(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sdt_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/*
 *  =======================================================================
 *
 *  OPEN AND CLOSE
 *
 *  =======================================================================
 */

static caddr_t sdt_opens = NULL;

/**
 * sdt_qopen: module queue open procedure
 * @q: read queue of freshly allocated queue pair
 * @devp: device number of driver
 * @oflags: flags to open(2) call
 * @sflag: STREAMS flag
 * @crp: credentials of opening process
 */
STATIC streamscall int
sdt_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct sdt_pair *sp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((err = mi_open_comm(&sdt_opens, sizeof(*sp), q, devp, oflags, sflag, crp)))
		return (err);

	sp = (struct sdt_pair *) q->q_ptr;
	bzero(sp, sizeof(*sp));

	/* initialize the structure */

	sp->r_priv.pair = sp;
	sp->r_priv.other = &sp->w_priv;
	sp->r_priv.max_sdu = 272;
	sp->r_priv.min_sdu = 3;
	sp->r_priv.header_len = 0;
	sp->r_priv.ppa_style = LMI_STYLE1;
	sp->r_priv.hlen = 3;	/* 6 */
	sp->r_priv.mlen = 0x3f;	/* 0x1ff */
	sp->r_priv.m_state = LMI_DISABLED;

	sp->r_priv.option.pvar = SS7_PVAR_ITUT_00;
	sp->r_priv.option.popt = 0;

	sp->r_priv.sdt.notify.events = 0;

	sp->r_priv.sdt.config.t8 = 100;
	sp->r_priv.sdt.config.Tin = 4;
	sp->r_priv.sdt.config.Tie = 1;
	sp->r_priv.sdt.config.T = 64;
	sp->r_priv.sdt.config.D = 256;
	sp->r_priv.sdt.config.Te = 793544;	/* E1 */
	sp->r_priv.sdt.config.De = 11328000;	/* E1 */
	sp->r_priv.sdt.config.Ue = 198384000;	/* E1 */
	sp->r_priv.sdt.config.N = 16;
	sp->r_priv.sdt.config.m = 272;
	sp->r_priv.sdt.config.b = 8;
	sp->r_priv.sdt.config.f = SDT_FLAGS_ONE;

	sp->r_priv.sdt.statem.aerm_state = SDT_STATE_IDLE;
	sp->r_priv.sdt.statem.aborted_proving = false;
	sp->r_priv.sdt.statem.Ca = 0;
	sp->r_priv.sdt.statem.Ti = 0;
	sp->r_priv.sdt.statem.suerm_state = SDT_STATE_IDLE;
	sp->r_priv.sdt.statem.Cs = 0;
	sp->r_priv.sdt.statem.Ns = 0;
	sp->r_priv.sdt.statem.eim_state = SDT_STATE_IDLE;
	sp->r_priv.sdt.statem.Ce = 0;
	sp->r_priv.sdt.statem.su_received = false;
	sp->r_priv.sdt.statem.interval_error = false;
	sp->r_priv.sdt.statem.daedt_state = SDT_STATE_IDLE;
	sp->r_priv.sdt.statem.daedr_state = SDT_STATE_IDLE;
	sp->r_priv.sdt.statem.octet_counting_mode = false;

	sp->r_priv.sdt.stats.tx_bytes = 0;
	sp->r_priv.sdt.stats.tx_sus = 0;
	sp->r_priv.sdt.stats.tx_sus_repeated = 0;
	sp->r_priv.sdt.stats.tx_underruns = 0;
	sp->r_priv.sdt.stats.tx_aborts = 0;
	sp->r_priv.sdt.stats.tx_buffer_overflows = 0;
	sp->r_priv.sdt.stats.tx_sus_in_error = 0;
	sp->r_priv.sdt.stats.rx_bytes = 0;
	sp->r_priv.sdt.stats.rx_sus = 0;
	sp->r_priv.sdt.stats.rx_sus_compressed = 0;
	sp->r_priv.sdt.stats.rx_overruns = 0;
	sp->r_priv.sdt.stats.rx_aborts = 0;
	sp->r_priv.sdt.stats.rx_buffer_overflows = 0;
	sp->r_priv.sdt.stats.rx_sus_in_error = 0;
	sp->r_priv.sdt.stats.rx_sync_transitions = 0;
	sp->r_priv.sdt.stats.rx_crc_errors = 0;
	sp->r_priv.sdt.stats.rx_frame_errors = 0;
	sp->r_priv.sdt.stats.rx_frame_overflows = 0;
	sp->r_priv.sdt.stats.rx_frame_too_long = 0;
	sp->r_priv.sdt.stats.rx_frame_too_short = 0;
	sp->r_priv.sdt.stats.rx_residue_errors = 0;
	sp->r_priv.sdt.stats.rx_length_error = 0;
	sp->r_priv.sdt.stats.carrier_cts_lost = 0;
	sp->r_priv.sdt.stats.carrier_dcd_lost = 0;
	sp->r_priv.sdt.stats.carrier_lost = 0;

	sp->r_priv.sdl.notify.events = 0;

	sp->r_priv.sdl.config.ifname = NULL;
	sp->r_priv.sdl.config.iftype = SDL_TYPE_PACKET;
	sp->r_priv.sdl.config.ifrate = SDL_RATE_NONE;
	sp->r_priv.sdl.config.ifgtype = SDL_GTYPE_NONE;
	sp->r_priv.sdl.config.ifgrate = SDL_GRATE_NONE;
	sp->r_priv.sdl.config.ifmode = SDL_MODE_PEER;
	sp->r_priv.sdl.config.ifgmode = SDL_GMODE_NONE;
	sp->r_priv.sdl.config.ifgcrc = SDL_GCRC_NONE;
	sp->r_priv.sdl.config.ifclock = SDL_CLOCK_NONE;
	sp->r_priv.sdl.config.ifcoding = SDL_CODING_NONE;
	sp->r_priv.sdl.config.ifframing = SDL_FRAMING_NONE;
	sp->r_priv.sdl.config.ifblksize = 8;
	sp->r_priv.sdl.config.ifleads = 0;
	sp->r_priv.sdl.config.ifbpv = 0;
	sp->r_priv.sdl.config.ifalarms = 0;
	sp->r_priv.sdl.config.ifrxlevel = 0;
	sp->r_priv.sdl.config.iftxlevel = 1;
	sp->r_priv.sdl.config.ifsync = 0;
	// sp->r_priv.sdl.config.ifsyncsrc = { 0, 0, 0, 0};

	sp->r_priv.sdl.statem.tx_state = SDL_STATE_IDLE;
	sp->r_priv.sdl.statem.rx_state = SDL_STATE_IDLE;

	sp->r_priv.sdl.stats.rx_octets = 0;
	sp->r_priv.sdl.stats.tx_octets = 0;
	sp->r_priv.sdl.stats.rx_overruns = 0;
	sp->r_priv.sdl.stats.tx_underruns = 0;
	sp->r_priv.sdl.stats.rx_buffer_overflows = 0;
	sp->r_priv.sdl.stats.tx_buffer_overflows = 0;
	sp->r_priv.sdl.stats.lead_cts_lost = 0;
	sp->r_priv.sdl.stats.lead_dcd_lost = 0;
	sp->r_priv.sdl.stats.carrier_lost = 0;

	sp->w_priv.pair = sp;
	sp->w_priv.other = &sp->r_priv;
	sp->w_priv.max_sdu = 272;
	sp->w_priv.min_sdu = 3;
	sp->w_priv.header_len = 0;
	sp->w_priv.ppa_style = LMI_STYLE1;
	sp->w_priv.hlen = 3;	/* 6 */
	sp->w_priv.mlen = 0x3f;	/* 0x1ff */
	sp->w_priv.m_state = LMI_DISABLED;

	sp->w_priv.option.pvar = SS7_PVAR_ITUT_00;
	sp->w_priv.option.popt = 0;

	sp->w_priv.sdt.notify.events = 0;

	sp->w_priv.sdt.config.t8 = 100;
	sp->w_priv.sdt.config.Tin = 4;
	sp->w_priv.sdt.config.Tie = 1;
	sp->w_priv.sdt.config.T = 64;
	sp->w_priv.sdt.config.D = 256;
	sp->w_priv.sdt.config.Te = 793544;	/* E1 */
	sp->w_priv.sdt.config.De = 11328000;	/* E1 */
	sp->w_priv.sdt.config.Ue = 198384000;	/* E1 */
	sp->w_priv.sdt.config.N = 16;
	sp->w_priv.sdt.config.m = 272;
	sp->w_priv.sdt.config.b = 8;
	sp->w_priv.sdt.config.f = SDT_FLAGS_ONE;

	sp->w_priv.sdt.statem.aerm_state = SDT_STATE_IDLE;
	sp->w_priv.sdt.statem.aborted_proving = false;
	sp->w_priv.sdt.statem.Ca = 0;
	sp->w_priv.sdt.statem.Ti = 0;
	sp->w_priv.sdt.statem.suerm_state = SDT_STATE_IDLE;
	sp->w_priv.sdt.statem.Cs = 0;
	sp->w_priv.sdt.statem.Ns = 0;
	sp->w_priv.sdt.statem.eim_state = SDT_STATE_IDLE;
	sp->w_priv.sdt.statem.Ce = 0;
	sp->w_priv.sdt.statem.su_received = false;
	sp->w_priv.sdt.statem.interval_error = false;
	sp->w_priv.sdt.statem.daedt_state = SDT_STATE_IDLE;
	sp->w_priv.sdt.statem.daedr_state = SDT_STATE_IDLE;
	sp->w_priv.sdt.statem.octet_counting_mode = false;

	sp->w_priv.sdt.stats.tx_bytes = 0;
	sp->w_priv.sdt.stats.tx_sus = 0;
	sp->w_priv.sdt.stats.tx_sus_repeated = 0;
	sp->w_priv.sdt.stats.tx_underruns = 0;
	sp->w_priv.sdt.stats.tx_aborts = 0;
	sp->w_priv.sdt.stats.tx_buffer_overflows = 0;
	sp->w_priv.sdt.stats.tx_sus_in_error = 0;

	sp->w_priv.sdt.stats.rx_bytes = 0;
	sp->w_priv.sdt.stats.rx_sus = 0;
	sp->w_priv.sdt.stats.rx_sus_compressed = 0;
	sp->w_priv.sdt.stats.rx_overruns = 0;
	sp->w_priv.sdt.stats.rx_aborts = 0;
	sp->w_priv.sdt.stats.rx_buffer_overflows = 0;
	sp->w_priv.sdt.stats.rx_sus_in_error = 0;
	sp->w_priv.sdt.stats.rx_sync_transitions = 0;
	sp->w_priv.sdt.stats.rx_crc_errors = 0;
	sp->w_priv.sdt.stats.rx_frame_errors = 0;
	sp->w_priv.sdt.stats.rx_frame_overflows = 0;
	sp->w_priv.sdt.stats.rx_frame_too_long = 0;
	sp->w_priv.sdt.stats.rx_frame_too_short = 0;
	sp->w_priv.sdt.stats.rx_residue_errors = 0;
	sp->w_priv.sdt.stats.rx_length_error = 0;
	sp->w_priv.sdt.stats.carrier_cts_lost = 0;
	sp->w_priv.sdt.stats.carrier_dcd_lost = 0;
	sp->w_priv.sdt.stats.carrier_lost = 0;

	sp->w_priv.sdl.notify.events = 0;

	sp->w_priv.sdl.config.ifname = NULL;
	sp->w_priv.sdl.config.iftype = SDL_TYPE_PACKET;
	sp->w_priv.sdl.config.ifrate = SDL_RATE_NONE;
	sp->w_priv.sdl.config.ifgtype = SDL_GTYPE_NONE;
	sp->w_priv.sdl.config.ifgrate = SDL_GRATE_NONE;
	sp->w_priv.sdl.config.ifmode = SDL_MODE_PEER;
	sp->w_priv.sdl.config.ifgmode = SDL_GMODE_NONE;
	sp->w_priv.sdl.config.ifgcrc = SDL_GCRC_NONE;
	sp->w_priv.sdl.config.ifclock = SDL_CLOCK_NONE;
	sp->w_priv.sdl.config.ifcoding = SDL_CODING_NONE;
	sp->w_priv.sdl.config.ifframing = SDL_FRAMING_NONE;
	sp->w_priv.sdl.config.ifblksize = 8;
	sp->w_priv.sdl.config.ifleads = 0;
	sp->w_priv.sdl.config.ifbpv = 0;
	sp->w_priv.sdl.config.ifalarms = 0;
	sp->w_priv.sdl.config.ifrxlevel = 0;
	sp->w_priv.sdl.config.iftxlevel = 1;
	sp->w_priv.sdl.config.ifsync = 0;
	// sp->w_priv.sdl.config.ifsyncsrc = { 0, 0, 0, 0};

	sp->w_priv.sdl.statem.tx_state = SDL_STATE_IDLE;
	sp->w_priv.sdl.statem.rx_state = SDL_STATE_IDLE;

	sp->w_priv.sdl.stats.rx_octets = 0;
	sp->w_priv.sdl.stats.tx_octets = 0;
	sp->w_priv.sdl.stats.rx_overruns = 0;
	sp->w_priv.sdl.stats.tx_underruns = 0;
	sp->w_priv.sdl.stats.rx_buffer_overflows = 0;
	sp->w_priv.sdl.stats.tx_buffer_overflows = 0;
	sp->w_priv.sdl.stats.lead_cts_lost = 0;
	sp->w_priv.sdl.stats.lead_dcd_lost = 0;
	sp->w_priv.sdl.stats.carrier_lost = 0;

	qprocson(q);
	return (0);
}

/**
 * sdt_qclose: module queue close procedure
 * @q: read queue of queue pair
 * @oflags: flags to open(2) call
 * @crp: credentials of closing stream
 */
STATIC streamscall int
sdt_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	mi_close_comm(&sdt_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  =========================================================================
 */

#ifdef LINUX
unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for SDT-PMOD module. (0 for allocation.)");

STATIC struct qinit sdt_rinit = {
	.qi_putp = sdt_putp,		/* Read put (message from below) */
	.qi_srvp = sdt_srvp,		/* Read queue service */
	.qi_qopen = sdt_qopen,		/* Each open */
	.qi_qclose = sdt_qclose,	/* Last close */
	.qi_minfo = &sdt_minfo,		/* Information */
	.qi_mstat = &sdt_rstat,		/* Statistics */
};

STATIC struct qinit sdt_winit = {
	.qi_putp = sdt_putp,		/* Write put (message from above) */
	.qi_srvp = sdt_srvp,		/* Write queue service */
	.qi_minfo = &sdt_minfo,		/* Information */
	.qi_mstat = &sdt_wstat,		/* Statistics */
};

STATIC struct streamtab sdt_pmodinfo = {
	.st_rdinit = &sdt_rinit,	/* Upper read queue */
	.st_wrinit = &sdt_winit,	/* Upper write queue */
};

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

STATIC struct fmodsw sdt_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sdt_pmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
sdt_pmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&sdt_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
sdt_pmodterminate(void)
{
	int err;

	if ((err = unregister_strmod(&sdt_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	return;
}

module_init(sdt_pmodinit);
module_exit(sdt_pmodterminate);

#endif				/* LINUX */
