/*****************************************************************************

 @(#) $RCSfile: mx_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:38 $

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

 Last Modified $Date: 2007/07/14 01:13:38 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mx_pmod.c,v $
 Revision 0.9.2.1  2007/07/14 01:13:38  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: mx_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:38 $"

static char const ident[] = "$RCSfile: mx_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:38 $";

/*
 *  This is MX-PMOD.  This is a pushable STREAMS module that can be pushed on one end of a
 *  STREAMS-based pipe to simulate an MX driver stream.  This is a PPA Style 1 stream.  Each end of
 *  the STREAMS-based pipe will present the MXI interface.
 */

#define _MPS_SOURCE 1
#define _LFS_SOURCE 1

#include <sys/os7/compat.h>

#include <sys/mxi.h>
#include <sys/mxi_ioctl.h>

#define MX_DESCRIP	"MX (Multiplex) STREAMS PIPE MODULE."
#define MX_REVISION	"OpenSS7 $RCSfile: mx_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:38 $"
#define MX_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define MX_DEVICE	"Provides OpenSS7 MX pipe driver."
#define MX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MX_LICENSE	"GPL v2"
#define MX_BANNER	MX_DESCRIP	"\n" \
			MX_REVISION	"\n" \
			MX_COPYRIGHT	"\n" \
			MX_DEVICE	"\n" \
			MX_CONTACT	"\n"
#define MX_SPLASH	MX_DEVICE	" - " \
			MX_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(MX_CONTACT);
MODULE_DESCRIPTION(MX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mx-pmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define MX_PMOD_MOD_ID CONFIG_STREAMS_MX_PMOD_MODID
#define MX_PMOD_MOD_NAME CONFIG_STREAMS_MX_PMOD_NAME
#endif				/* LFS */

#ifndef MX_PMOD_MOD_NAME
#define MX_PMOD_MOD_NAME "mx-pmod"
#endif				/* MX_PMOD_MOD_NAME */

#ifndef MX_PMOD_MOD_ID
#define MX_PMOD_MOD_ID 0
#endif				/* MX_PMOD_MOD_ID */

/*
 *  ===========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  ===========================================================================
 */

#define MOD_ID		MX_PMOD_MOD_ID
#define MOD_NAME	MX_PMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MX_BANNER
#else				/* MODULE */
#define MOD_BANNER	MX_SPLASH
#endif				/* MODULE */

static struct module_info mx_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat mx_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mx_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  ===========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  ===========================================================================
 */

struct mx_pair;

struct mx {
	struct mx_pair *pair;
	struct mx *other;
	uint flags;
	uint state;
	uint oldflags;
	uint oldstate;
	struct {
		struct mx_config config;
		struct mx_notify notify;
		struct mx_statem statem;
		struct mx_stats statsp;
		struct mx_stats stats;
	} mx;
};

struct mx_pair {
	struct mx r_priv;
	struct mx w_priv;
};

#define MX_PRIV(q) \
	(((q)->q_flag & QREADR) ? \
	 &((struct mx_pair *)(q)->q_ptr)->r_priv : \
	 &((struct mx_pair *)(q)->q_ptr)->w_priv)

#define STRLOGER	0	/* log Stream errors */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

/**
 * mx_statename: display MXI state name
 * @state: the state value to display
 */
static const char *
mx_statename(long state)
{
	switch (state) {
	case MXS_UNINIT:
		return ("MXS_UNINIT");
	case MXS_UNUSABLE:
		return ("MXS_UNUSABLE");
	case MXS_DETACHED:
		return ("MXS_DETACHED");
	case MXS_WACK_AREQ:
		return ("MXS_WACK_AREQ");
	case MXS_WACK_UREQ:
		return ("MXS_WACK_UREQ");
	case MXS_ATTACHED:
		return ("MXS_ATTACHED");
	case MXS_WACK_EREQ:
		return ("MXS_WACK_EREQ");
	case MXS_WCON_EREQ:
		return ("MXS_WCON_EREQ");
	case MXS_WACK_RREQ:
		return ("MXS_WACK_RREQ");
	case MXS_WCON_RREQ:
		return ("MXS_WCON_RREQ");
	case MXS_ENABLED:
		return ("MXS_ENABLED");
	case MXS_WACK_CREQ:
		return ("MXS_WACK_CREQ");
	case MXS_WCON_CREQ:
		return ("MXS_WCON_CREQ");
	case MXS_WACK_DREQ:
		return ("MXS_WACK_DREQ");
	case MXS_WCON_DREQ:
		return ("MXS_WCON_DREQ");
	case MXS_CONNECTED:
		return ("MXS_CONNECTED");
	default:
		return ("MXS_????");
	}
}

/**
 * mx_get_state: - get state for private structure
 * @mx: private structure
 */
static int
mx_get_state(struct mx *mx)
{
	return mx->state;
}

/**
 * mx_set_state: - set state for private structure
 * @mx: private structure
 * @q: active queue
 * @newstate: new state
 */
static int
mx_set_state(struct mx *mx, queue_t *q, int newstate)
{

	int oldstate = mx->state;

	if (newstate != oldstate) {
		mx->state = newstate;
		mi_strlog(q, STRLOGST, SL_TRACE, "%s <- %s", mx_statename(newstate),
			  mx_statename(oldstate));
	}
	return (newstate);
}

/**
 * mx_save_state: - reset state for private structure
 * @mx: private structure
 */
static void
mx_save_state(struct mx *mx)
{
	mx->oldstate = mx->state;
}

/**
 * mx_restore_state: - reset state for private structure
 * @mx: private structure
 */
static int
mx_restore_state(struct mx *mx, queue_t *q)
{
	return mx_set_state(mx, q, mx->oldstate);
}

/**
 * mx_get_flags: - get flags for private structure
 * @mx: private structure
 */
static inline int
mx_get_flags(struct mx *mx)
{
	return mx->flags;
}

/**
 * mx_set_flags: - set flags for private structure
 * @mx: private structure
 * @q: active queue
 * @newflags: new flags
 */
static int
mx_set_flags(struct mx *mx, queue_t *q, int newflags)
{

	int oldflags = mx->flags;

	if (newflags != oldflags) {
		mx->flags = newflags;
	}
	return (newflags);
}

static int
mx_or_flags(struct mx *mx, int orflags)
{
	return (mx->flags |= orflags);
}

static int
mx_nand_flags(struct mx *mx, int nandflags)
{
	return (mx->flags &= ~nandflags);
}

/**
 * mx_save_flags: - reset flags for private structure
 * @mx: private structure
 */
static void
mx_save_flags(struct mx *mx)
{
	mx->oldflags = mx->flags;
}

/**
 * mx_restore_flags: - reset flags for private structure
 * @mx: private structure
 */
static int
mx_restore_flags(struct mx *mx, queue_t *q)
{
	return mx_set_flags(mx, q, mx->oldflags);
}

/*
 *  ===========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  MX Provider -> MX User primitives.
 *
 *  ===========================================================================
 */

/**
 * mx_info_ack: - issue MX_INFO_ACK primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
mx_info_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	struct MX_parms_circuit *c;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sizeof(*c), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		p->mx_addr_length = 0;
		p->mx_addr_offset = 0;
		p->mx_parm_length = sizeof(*c);
		p->mx_parm_offset = sizeof(*p);
		p->mx_prov_flags = 0;
		p->mx_prov_class = MX_CIRCUIT;
		p->mx_style = MX_STYLE1;
		p->mx_version = MX_VERSION;
		p->mx_state = mx_get_state(mx);
		mp->b_wptr += sizeof(*p);
		c = (typeof(c)) mp->b_wptr;
		c->mp_type = MX_PARMS_CIRCUIT;
		c->mp_encoding = MX_ENCODING_G711_PCM_U;	/* encoding */
		c->mp_block_size = 65536;	/* data block size (bits) */
		c->mp_samples = 8;	/* samples per block */
		c->mp_sample_size = 8;	/* sample size (bits) */
		c->mp_rate = 8000;	/* channel clock rate (samples/second) */
		c->mp_tx_channels = 672;	/* number of tx channels */
		c->mp_rx_channels = 672;	/* number of rx channels */
		c->mp_opt_flags = MX_PARM_OPT_CLRCH;	/* options flags */
		mp->b_wptr += sizeof(*c);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_INFO_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_optmgmt_ack: - issue MX_OPTMGMT_ACK primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: management flags
 * @opt_ptr: options pointer
 * @opt_len: options length
 */
static inline int
mx_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *msg, int flags, caddr_t opt_ptr, size_t opt_len)
{
	struct MX_optmgmt_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OPTMGMT_ACK;
		p->mx_opt_length = opt_len;	/* FIXME */
		p->mx_opt_offset = opt_len ? sizeof(*p) : 0;	/* FIXME */
		p->mx_mgmt_flags = 0;	/* FIXME */
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_OPTMGMT_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_ok_ack: - issue MX_OK_ACK primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *msg, int prim)
{
	struct MX_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = prim;
		switch (prim) {
		case MX_ATTACH_REQ:
			p->mx_state = mx_set_state(mx, q, MXS_ATTACHED);
			break;
		case MX_DETACH_REQ:
			p->mx_state = mx_set_state(mx, q, MXS_DETACHED);
			break;
		default:
			p->mx_state = mx_get_state(mx);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_OK_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_error_ack: - issue MX_ERROR_ACK primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error number
 */
static int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *msg, int prim, int error)
{
	struct MX_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ERROR_ACK;
		p->mx_error_primitive = prim;
		p->mx_error_type = error < 0 ? MXSYSERR : error;
		p->mx_unix_error = error < 0 ? -error : 0;
		p->mx_state = mx_restore_state(mx, q);
		mp->b_wptr += sizeof(*p);
		mx_restore_flags(mx, q);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_ERROR_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_error_reply: - reply with an error
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error number
 *
 * This is essentially the same as mx_error_ack() except that typical queue
 * return codes are filtered and returned directly.
 */
static int
mx_error_reply(struct mx *mx, queue_t *q, mblk_t *msg, int prim, int error)
{
	if (unlikely(error <= 0)) {
		switch (error) {
		case 0:
		case -EBUSY:
		case -ENOBUFS:
		case -ENOMEM:
		case -EAGAIN:
		case -EDEADLK:
			return (error);
		default:
			break;
		}
	}
	return mx_error_ack(mx, q, msg, prim, error);
}

/**
 * mx_enable_con: - issue MX_ENABLE_CON primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
mx_enable_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ENABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_ENABLE_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_con: - issue MX_DISABLE_CON primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
mx_disable_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DISABLE_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_connect_con: - issue MX_CONNECT_CON primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @slot: slot connected
 */
static int
mx_connect_con(struct mx *mx, queue_t *q, mblk_t *msg, int flags, int slot)
{
	struct MX_connect_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_CONNECT_CON;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_CONNECT_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_data_ind: - issue MX_DATA_IND primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @slot: slot to which data belongs
 * @dp: data portion
 */
static int
mx_data_ind(struct mx *mx, queue_t *q, mblk_t *msg, int slot, mblk_t *dp)
{
	struct MX_data_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DATA_IND;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DATA_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_ind: - issue MX_DISCONNECT_INT primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @cause: cause for disconnect
 * @slot: slot disconnected
 */
static inline int
mx_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *msg, int flags, int cause, int slot)
{
	struct MX_disconnect_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_IND;
		p->mx_conn_flags = flags;
		p->mx_cause = cause;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DISCONNECT_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_con: - issue MX_DISCONNECT_CON primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @slot: slot disconnected
 */
static int
mx_disconnect_con(struct mx *mx, queue_t *q, mblk_t *msg, int flags, int slot)
{
	struct MX_disconnect_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_CON;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DISCONNECT_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_ind: - issue MX_DISABLE_IND primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cause: cause for disable
 */
static inline int
mx_disable_ind(struct mx *mx, queue_t *q, mblk_t *msg, int cause)
{
	struct MX_disable_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_IND;
		p->mx_cause = cause;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DISABLE_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_event_ind: - issue MX_EVENT_IND primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @event: event to indicate
 * @slot: MX slot for event
 */
static inline int
mx_event_ind(struct mx *mx, queue_t *q, mblk_t *msg, int event, int slot)
{
	struct MX_event_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_EVENT_IND;
		p->mx_event = event;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_EVENT_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  ===========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  ===========================================================================
 */

static inline int
mx_attach(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	mx_set_state(mx, q, MXS_WACK_AREQ);
	err = mx_ok_ack(mx, q, msg, MX_ATTACH_REQ);
	return (err);
}

static int
mx_enable(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	mx_set_state(mx, q, MXS_WCON_EREQ);
	err = mx_enable_con(mx, q, msg);
	return (err);
}

static int
mx_connect(struct mx *mx, queue_t *q, mblk_t *msg, int flags, int slot)
{
	int err;

	mx_set_state(mx, q, MXS_WCON_CREQ);
	mx_or_flags(mx, flags);
	err = mx_connect_con(mx, q, msg, flags, slot);
	return (err);
}

static int
mx_data(struct mx *mx, queue_t *q, mblk_t *msg, int slot, mblk_t *dp)
{
	int err;

	if ((mx->flags & MXF_TX_DIR) && (mx->other->flags & MXF_RX_DIR)) {
		err = mx_data_ind(mx, q, msg, slot, dp);
		return (err);
	}
	freemsg(dp);
	return (0);
}

static int
mx_disconnect(struct mx *mx, queue_t *q, mblk_t *msg, int flags, int slot)
{
	int err;

	mx_set_state(mx, q, MXS_WCON_DREQ);
	mx_nand_flags(mx, flags);
	err = mx_disconnect_con(mx, q, msg, flags, slot);
	return (err);
}

static int
mx_disable(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	mx_set_state(mx, q, MXS_WCON_RREQ);
	err = mx_disable_con(mx, q, msg);
	return (err);
}

static int
mx_detach(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	mx_set_state(mx, q, MXS_WACK_UREQ);
	err = mx_ok_ack(mx, q, msg, MX_DETACH_REQ);
	return (err);
}

/*
 *  ===========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  MX User -> MX Provider primitives.
 *
 *  ===========================================================================
 */

/**
 * mx_info_req: - process MX_INFO_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_info_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_info_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_INFO_REQ, err);
}

/**
 * mx_optmgmt_req: - process MX_OPTMGMT_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_optmgmt_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + p->mx_opt_offset + p->mx_opt_length)
		goto badopt;
	switch (p->mx_mgmt_flags) {
	case MX_SET_OPT:
	case MX_GET_OPT:
	case MX_NEGOTIATE:
	case MX_DEFAULT:
		break;
	default:
		goto badflag;
	}
      badflag:
	err = MXBADFLAG;
	goto error;
      badopt:
	err = MXBADOPT;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_OPTMGMT_REQ, err);
}

/**
 * mx_attach_req: - process MX_ATTACH_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_attach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_attach_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	if (mx_get_state(mx) != MXS_DETACHED
	    && (mx_get_state(mx) != MXS_ATTACHED || p->mx_addr_length == 0))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + p->mx_addr_offset + p->mx_addr_length)
		goto badaddr;
	/* Note that we do not need to support MX_ATTACH_REQ because this is a style1 driver only,
	   but if the address is null, simply move to the appopriate state. */
	return mx_attach(mx, q, mp);
      badaddr:
	err = MXBADADDR;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_ATTACH_REQ, err);
}

/**
 * mx_enable_req: - process MX_ENABLE_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_enable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_ATTACHED)
		goto outstate;
	if ((err = mx_enable(mx, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_ENABLE_REQ, err);
}

/**
 * mx_connect_req: - process MX_CONNECT_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_connect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_ENABLED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	if ((p->mx_conn_flags & ~(MXF_BOTH_DIR)) || ((p->mx_conn_flags & (MXF_BOTH_DIR)) == 0))
		goto badflag;
	if (p->mx_slot != 0)
		goto badaddr;
	if ((err = mx_connect(mx, q, mp, p->mx_conn_flags, p->mx_slot)) != 0)
		goto error;
	return (0);
      badaddr:
	err = MXBADADDR;
	goto error;
      badflag:
	err = MXBADFLAG;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_CONNECT_REQ, err);
}

/**
 * mx_data_req: - process MX_DATA_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_data_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_data_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_CONNECTED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	if (p->mx_slot != 0)
		goto badaddr;
	if ((err = mx_data(mx, q, NULL, p->mx_slot, mp->b_cont)) != 0)
		goto error;
	freeb(mp);
	return (0);
      badaddr:
	err = MXBADADDR;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_DATA_REQ, err);
}

/**
 * mx_disconnect_req: - process MX_DISCONNECT_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_disconnect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_CONNECTED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	if ((p->mx_conn_flags & ~(MXF_BOTH_DIR)) && ((p->mx_conn_flags & (MXF_BOTH_DIR)) == 0))
		goto badflag;
	if (p->mx_slot != 0)
		goto badaddr;
	if ((err = mx_disconnect(mx, q, mp, p->mx_conn_flags, p->mx_slot)) != 0)
		goto error;
	return (0);
      badaddr:
	err = MXBADADDR;
	goto error;
      badflag:
	err = MXBADFLAG;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_DISCONNECT_REQ, err);
}

/**
 * mx_disable_req: - process MX_DISABLE_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_disable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_ENABLED)
		goto outstate;
	if ((err = mx_disable(mx, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_DISABLE_REQ, err);
}

/**
 * mx_detach_req: - process MX_DETACH_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_detach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_detach_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_ATTACHED)
		goto outstate;
	if ((err = mx_detach(mx, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_DETACH_REQ, err);
}

/**
 * mx_other_req: - process MX_???_REQ primitive
 * @mx: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
mx_other_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mx_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, *p, err);
}

/*
 *  ===========================================================================
 *
 *  INPUT-OUTPUT CONTROLS
 *
 *  ===========================================================================
 */

/**
 * mx_testconfig: - test MX cnfiguration for validity
 * @mx: (locked) private structure
 * @arg: configuration to test
 */
static int
mx_testconfig(struct mx *mx, struct mx_config *arg)
{
	int err = 0;

	if (arg->type != 0)
		err = EINVAL;
	if (arg->encoding != 0)
		err = EINVAL;
	if (arg->block_size == 0)
		err = EINVAL;
	if (arg->samples == 0 || arg->samples > 8)
		err = EINVAL;
	if (arg->sample_size == 0 || arg->sample_size > 16)
		err = EINVAL;
	if (arg->rate != 8000)
		err = EINVAL;
	if (arg->tx_channels == 0)
		err = EINVAL;
	if (arg->rx_channels == 0)
		err = EINVAL;
	if (arg->tx_channels != arg->rx_channels)
		err = EINVAL;
	if (arg->block_size < arg->rx_channels * arg->samples * arg->sample_size)
		err = EINVAL;
	if (arg->opt_flags != 0)
		err = EINVAL;
	return (err);
}

#ifndef MXF_EVT_ALL
#define MXF_EVT_ALL \
	( 0 \
	  | MXF_EVT_DCD_CHANGE \
	  | MXF_EVT_DSR_CHANGE \
	  | MXF_EVT_DTR_CHANGE \
	  | MXF_EVT_RTS_CHANGE \
	  | MXF_EVT_CTS_CHANGE \
	  | MXF_EVT_RI_CHANGE \
	  | MXF_EVT_YEL_ALARM \
	  | MXF_EVT_BLU_ALARM \
	  | MXF_EVT_RED_ALARM \
	  | MXF_EVT_NO_ALARM \
	)
#endif				/* MXF_EVT_ALL */

/**
 * mx_setnotify: - set MX notification bits
 * @mx: (locked) private structure
 * @arg: notification arguments
 */
static int
mx_setnotify(struct mx *mx, struct mx_notify *arg)
{
	if (arg->events & ~(MXF_EVT_ALL))
		return (EINVAL);
	arg->events = arg->events | mx->mx.notify.events;
	return (0);
}

/**
 * mx_clrnotify: - clear MX notification bits
 * @mx: (locked) private structure
 * @arg: notification arguments
 */
static int
mx_clrnotify(struct mx *mx, struct mx_notify *arg)
{
	if (arg->events & ~(MXF_EVT_ALL))
		return (EINVAL);
	arg->events = ~arg->events & mx->mx.notify.events;
	return (0);
}

/**
 * mx_manage: - perform MX management action
 * @mx: (locked) private structure
 * @arg: management arguments
 */
static int
mx_manage(struct mx *mx, struct mx_mgmt *arg)
{
	switch (arg->cmd) {
	case MX_MGMT_RESET:
		/* FIXME: reset */
		break;
	default:
		return (EINVAL);
	}
	return (0);
}

/**
 * mx_ioctl: - process MX M_IOCTL message
 * @mx: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the MX input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static int
mx_ioctl(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(MX_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCSCONFIG)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.config), false)))
			goto enobufs;
		bcopy(&mx->mx.config, bp->b_rptr, sizeof(mx->mx.config));
		break;
	case _IOC_NR(MX_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCGCONFIG)");
		size = sizeof(mx->mx.config);
		break;
	case _IOC_NR(MX_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCTCONFIG)");
		size = sizeof(mx->mx.config);
		break;
	case _IOC_NR(MX_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCCCONFIG)");
		size = sizeof(mx->mx.config);
		break;
	case _IOC_NR(MX_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCGSTATEM)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.statem), false)))
			goto enobufs;
		bcopy(&mx->mx.statem, bp->b_rptr, sizeof(mx->mx.statem));
		break;
	case _IOC_NR(MX_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCCMRESET)");
		/* FIXME: reset the state machine */
		break;
	case _IOC_NR(MX_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCGSTATSP)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.statsp), false)))
			goto enobufs;
		bcopy(&mx->mx.statsp, bp->b_rptr, sizeof(mx->mx.statsp));
		break;
	case _IOC_NR(MX_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCSSTATSP)");
		size = sizeof(mx->mx.statsp);
		break;
	case _IOC_NR(MX_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCGSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.stats), false)))
			goto enobufs;
		bcopy(&mx->mx.stats, bp->b_rptr, sizeof(mx->mx.stats));
		break;
	case _IOC_NR(MX_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCCSTATS)");
		size = sizeof(mx->mx.stats);
		break;
	case _IOC_NR(MX_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCGNOTIFY)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.notify), false)))
			goto enobufs;
		bcopy(&mx->mx.notify, bp->b_rptr, sizeof(mx->mx.notify));
		break;
	case _IOC_NR(MX_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCSNOTIFY)");
		size = sizeof(mx->mx.notify);
		break;
	case _IOC_NR(MX_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCCNOTIFY)");
		size = sizeof(mx->mx.notify);
		break;
	case _IOC_NR(MX_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MX_IOCCMGMT)");
		size = sizeof(struct mx_mgmt);
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
 * mx_copyin: - process MX M_IOCDATA message
 * @mx: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the
 * implicit or explicit copyin operation.  We can now perform sets and
 * commits.  All SET operations also include a last copyout step that copies
 * out the information actually set.
 */
static int
mx_copyin(struct mx *mx, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MX_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MX_IOCSCONFIG)");
		if ((err = mx_testconfig(mx, (struct mx_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &mx->mx.config, sizeof(mx->mx.config));
		break;
	case _IOC_NR(MX_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MX_IOCTCONFIG)");
		err = mx_testconfig(mx, (struct mx_config *) bp->b_rptr);
		break;
	case _IOC_NR(MX_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MX_IOCCCONFIG)");
		if ((err = mx_testconfig(mx, (struct mx_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &mx->mx.config, sizeof(mx->mx.config));
		break;
	case _IOC_NR(MX_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MX_IOCSSTATSP)");
		bcopy(bp->b_rptr, &mx->mx.statsp, sizeof(mx->mx.statsp));
		break;
	case _IOC_NR(MX_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MX_IOCSNOTIFY)");
		if ((err = mx_setnotify(mx, (struct mx_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &mx->mx.notify, sizeof(mx->mx.notify));
		break;
	case _IOC_NR(MX_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MX_IOCCNOTIFY)");
		if ((err = mx_clrnotify(mx, (struct mx_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &mx->mx.notify, sizeof(mx->mx.notify));
		break;
	case _IOC_NR(MX_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MX_IOCCMGMT)");
		err = mx_manage(mx, (struct mx_mgmt *) bp->b_rptr);
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
 * mx_copyout: - process MX M_IOCDATA message
 * @mx: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final stop which is a simple copy done operation.
 */
static int
mx_copyout(struct mx *mx, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

/*
 *  ===========================================================================
 *
 *  PUT AND SERVICE PROCEDURES
 *
 *  ===========================================================================
 */

/**
 * mx_m_data: - process M_DATA message
 * @mx: private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static fastcall int
mx_m_data(struct mx *mx, queue_t *q, mblk_t *mp)
{
	return mx_data(mx, q, NULL, 0, mp);
}

/**
 * mx_m_proto: = process M_(PC)PROTO message
 * @mx: private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static fastcall int
mx_m_proto(struct mx *mx, queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);

	mx_save_state(mx);
	mx_save_flags(mx);

	switch (*(mx_ulong *) mp->b_rptr) {
	case MX_INFO_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_INFO_REQ");
		rtn = mx_info_req(mx, q, mp);
		break;
	case MX_OPTMGMT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_OPTMGMT_REQ");
		rtn = mx_optmgmt_req(mx, q, mp);
		break;
	case MX_ATTACH_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_ATTACH_REQ");
		rtn = mx_attach_req(mx, q, mp);
		break;
	case MX_ENABLE_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_ENABLE_REQ");
		rtn = mx_enable_req(mx, q, mp);
		break;
	case MX_CONNECT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_CONNECT_REQ");
		rtn = mx_connect_req(mx, q, mp);
		break;
	case MX_DATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> MX_DATA_REQ");
		rtn = mx_data_req(mx, q, mp);
		break;
	case MX_DISCONNECT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_DISCONNECT_REQ");
		rtn = mx_disconnect_req(mx, q, mp);
		break;
	case MX_DISABLE_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_DISABLE_REQ");
		rtn = mx_disable_req(mx, q, mp);
		break;
	case MX_DETACH_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_DETACH_REQ");
		rtn = mx_detach_req(mx, q, mp);
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MX_????_REQ");
		rtn = mx_other_req(mx, q, mp);
		break;
	}
	if (rtn) {
		mx_restore_state(mx, q);
		mx_restore_flags(mx, q);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * mx_m_flush: - process M_FLUSH message
 * @q: active queue
 * @mp: the M_FLUSH message
 *
 * Avoid having tot push pipemod.  It we are the bottommost module over a pipe
 * twist then perform the actions of pipemod.  This means that the mx-pmod
 * module must be pushed over the same side of a pipe as pipemod, if pipemod
 * is pushed at all.
 */
static fastcall int
mx_m_flush(queue_t *q, mblk_t *mp)
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
			mp->b_rptr[1] = FLUSHW;
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

/**
 * mx_m_ioctl: - process M_IOCTL message
 * @mx: private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 */
static fastcall int
mx_m_ioctl(struct mx *mx, queue_t *q, mblk_t *mp)
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
	case MX_IOC_MAGIC:
		err = mx_ioctl(mx, q, mp);
		break;
	}
	mi_unlock(priv);
	return (err);
}

/**
 * mx_m_iocdata: - process M_IOCDATA messages
 * @mx: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 */
static fastcall int
mx_m_iocdata(struct mx *mx, queue_t *q, mblk_t *mp)
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
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		case MX_IOC_MAGIC:
			err = mx_copyin(mx, q, mp, dp);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		case MX_IOC_MAGIC:
			err = mx_copyout(mx, q, mp, dp);
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

/**
 * mx_m_rse: - process M_(PC)RSE message
 * @mx: private structure
 * @q: active queue
 * @mp: the M_(PC)RSE message
 */
static fastcall int
mx_m_rse(struct mx *mx, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

#ifndef DB_TYPE
#define DB_TYPE(mp) (mp->b_datap->db_type)
#endif

/**
 * mx_m_other: - process other STREAMS message
 * @mx: private structure
 * @q: active queue
 * @mp: other STREAMS message
 */
static fastcall int
mx_m_other(struct mx *mx, queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
mx_msg_slow(struct mx *mx, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mx_m_data(mx, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_m_proto(mx, q, mp);
	case M_FLUSH:
		return mx_m_flush(q, mp);
	case M_IOCTL:
		return mx_m_ioctl(mx, q, mp);
	case M_IOCDATA:
		return mx_m_iocdata(mx, q, mp);
	case M_RSE:
	case M_PCRSE:
		return mx_m_rse(mx, q, mp);
	default:
		return mx_m_other(mx, q, mp);
	}
}

/**
 * mx_m_data_fast: - process M_DATA message
 * @mx: private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
mx_m_data_fast(struct mx *mx, queue_t *q, mblk_t *mp)
{
	return mx_m_data(mx, q, mp);
}

/**
 * mx_msg: - process STREAMS message
 * @q: active queue
 * @mp: the message
 */
static inline fastcall int
mx_msg(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		return mx_m_data_fast(mx, q, mp);
	return mx_msg_slow(mx, q, mp);
}

/**
 * mx_putp: - canonical put procedure
 * @q: active queue
 * @mp: message to put
 */
static streamscall __hot_in int
mx_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mx_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * mx_srvp: - canonical service procedure
 * @q: active queue
 */
static streamscall __hot_read int
mx_srvp(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mx_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/*
 *  ===========================================================================
 *
 *  OPEN AND CLOSE
 *
 *  ===========================================================================
 */

static caddr_t mx_opens = NULL;

/**
 * mx_qopen: - module queue open procedure
 * @q: read queue of queue pair
 * @devp: device number of driver
 * @oflags: flags to open(2) call
 * @sflag: STREAMS flag
 * @crp: credientials of opening process
 */
static streamscall int
mx_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct mx_pair *mxp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((err = mi_open_comm(&mx_opens, sizeof(*mxp), q, devp, oflags, sflag, crp)))
		return (err);

	mxp = (typeof(mxp)) q->q_ptr;
	bzero(mxp, sizeof(*mxp));

	/* initialize the structure */

	qprocson(q);
	return (0);
}

/**
 * mx_qclose: - module queue close procedure
 * @q: queue pair
 * @oflags: flags to open(2) call
 * @crp: credentials of closing process
 */
static streamscall int
mx_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	mi_close_comm(&mx_opens, q);
	return (0);
}

/*
 *  ===========================================================================
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  ===========================================================================
 */

#ifdef LINUX
unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for MX-PMOD module. (0 for allocation.)");

static struct qinit mx_rinit = {
	.qi_putp = mx_putp,
	.qi_srvp = mx_srvp,
	.qi_qopen = mx_qopen,
	.qi_qclose = mx_qclose,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_rstat,
};
static struct qinit mx_winit = {
	.qi_putp = mx_putp,
	.qi_srvp = mx_srvp,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_wstat,
};
static struct streamtab mx_pmodinfo = {
	.st_rdinit = &mx_rinit,
	.st_wrinit = &mx_winit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw mx_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mx_pmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
mx_pmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&mx_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}
static __exit void
mx_pmodterminate(void)
{
	int err;

	if ((err = unregister_strmod(&mx_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	return;
}

module_init(mx_pmodinit);
module_exit(mx_pmodterminate);

#endif				/* LINUX */
