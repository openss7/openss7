/*****************************************************************************

 @(#) $RCSfile: ch_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:38 $

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

 $Log: ch_pmod.c,v $
 Revision 0.9.2.1  2007/07/14 01:13:38  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: ch_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:38 $"

static char const ident[] = "$RCSfile: ch_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:38 $";

/*
 *  This is CH-PMOD.  This is a pushable STREAMS modules that can be pushed on one end of a
 *  STREAMS-based pipe to simulate a CH driver stream.  This is a PPA Style 1 stream.  Each end of
 *  the STREAMS-based pipe will present the CHI interface.
 */

#define _MPS_SOURCE 1
#define _LFS_SOURCE 1

#include <sys/os7/compat.h>

#include <sys/chi.h>
#include <sys/chi_ioctl.h>

#define CH_DESCRIP	"CH (Channel) STREAMS PIPE MODULE."
#define CH_REVISION	"OpenSS7 $RCSfile: ch_pmod.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/07/14 01:13:38 $"
#define CH_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define CH_DEVICE	"Provides OpenSS7 CH pipe driver."
#define CH_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CH_LICENSE	"GPL v2"
#define CH_BANNER	CH_DESCRIP	"\n" \
			CH_REVISION	"\n" \
			CH_COPYRIGHT	"\n" \
			CH_DEVICE	"\n" \
			CH_CONTACT	"\n"
#define CH_SPLASH	CH_DEVICE	" - " \
			CH_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(CH_CONTACT);
MODULE_DESCRIPTION(CH_DESCRIP);
MODULE_SUPPORTED_DEVICE(CH_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CH_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-ch-pmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define CH_PMOD_MOD_ID CONFIG_STREAMS_CH_PMOD_MODID
#define CH_PMOD_MOD_NAME CONFIG_STREAMS_CH_PMOD_NAME
#endif				/* LFS */

#ifndef CH_PMOD_MOD_NAME
#define CH_PMOD_MOD_NAME "ch-pmod"
#endif				/* CH_PMOD_MOD_NAME */

#ifndef CH_PMOD_MOD_ID
#define CH_PMOD_MOD_ID 0
#endif				/* CH_PMOD_MOD_ID */

/*
 *  ===========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  ===========================================================================
 */

#define MOD_ID		CH_PMOD_MOD_ID
#define MOD_NAME	CH_PMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	CH_BANNER
#else				/* MODULE */
#define MOD_BANNER	CH_SPLASH
#endif				/* MODULE */

static struct module_info ch_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat ch_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ch_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  ===========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  ===========================================================================
 */

struct ch_pair;

struct ch {
	struct ch_pair *pair;
	struct ch *other;
	uint flags;
	uint state;
	uint oldflags;
	uint oldstate;
	struct {
		struct ch_config config;
		struct ch_notify notify;
		struct ch_statem statem;
		struct ch_stats statsp;
		struct ch_stats stats;
	} ch;
};

struct ch_pair {
	struct ch r_priv;
	struct ch w_priv;
};

#define CH_PRIV(q) \
	(((q)->q_flag & QREADR) ? \
	 &((struct ch_pair *)(q)->q_ptr)->r_priv : \
	 &((struct ch_pair *)(q)->q_ptr)->w_priv)

#define STRLOGER	0	/* log Stream errors */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

/**
 * ch_statename: display CHI state name
 * @state: the state value to display
 */
static const char *
ch_statename(long state)
{
	switch (state) {
	case CHS_UNINIT:
		return ("CHS_UNINIT");
	case CHS_UNUSABLE:
		return ("CHS_UNUSABLE");
	case CHS_DETACHED:
		return ("CHS_DETACHED");
	case CHS_WACK_AREQ:
		return ("CHS_WACK_AREQ");
	case CHS_WACK_UREQ:
		return ("CHS_WACK_UREQ");
	case CHS_ATTACHED:
		return ("CHS_ATTACHED");
	case CHS_WACK_EREQ:
		return ("CHS_WACK_EREQ");
	case CHS_WCON_EREQ:
		return ("CHS_WCON_EREQ");
	case CHS_WACK_RREQ:
		return ("CHS_WACK_RREQ");
	case CHS_WCON_RREQ:
		return ("CHS_WCON_RREQ");
	case CHS_ENABLED:
		return ("CHS_ENABLED");
	case CHS_WACK_CREQ:
		return ("CHS_WACK_CREQ");
	case CHS_WCON_CREQ:
		return ("CHS_WCON_CREQ");
	case CHS_WACK_DREQ:
		return ("CHS_WACK_DREQ");
	case CHS_WCON_DREQ:
		return ("CHS_WCON_DREQ");
	case CHS_CONNECTED:
		return ("CHS_CONNECTED");
	default:
		return ("CHS_????");
	}
}

/**
 * ch_get_state: - get state for private structure
 * @ch: private structure
 */
static int
ch_get_state(struct ch *ch)
{
	return ch->state;
}

/**
 * ch_set_state: - set state for private structure
 * @ch: private structure
 * @q: active queue
 * @newstate: new state
 */
static int
ch_set_state(struct ch *ch, queue_t *q, int newstate)
{

	int oldstate = ch->state;

	if (newstate != oldstate) {
		ch->state = newstate;
		mi_strlog(q, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
	}
	return (newstate);
}

/**
 * ch_save_state: - reset state for private structure
 * @ch: private structure
 */
static void
ch_save_state(struct ch *ch)
{
	ch->oldstate = ch->state;
}

/**
 * ch_restore_state: - reset state for private structure
 * @ch: private structure
 */
static int
ch_restore_state(struct ch *ch, queue_t *q)
{
	return ch_set_state(ch, q, ch->oldstate);
}

/**
 * ch_get_flags: - get flags for private structure
 * @ch: private structure
 */
static inline int
ch_get_flags(struct ch *ch)
{
	return ch->flags;
}

/**
 * ch_set_flags: - set flags for private structure
 * @ch: private structure
 * @q: active queue
 * @newflags: new flags
 */
static int
ch_set_flags(struct ch *ch, queue_t *q, int newflags)
{

	int oldflags = ch->flags;

	if (newflags != oldflags) {
		ch->flags = newflags;
	}
	return (newflags);
}

static int
ch_or_flags(struct ch *ch, int orflags)
{
	return (ch->flags |= orflags);
}

static int
ch_nand_flags(struct ch *ch, int nandflags)
{
	return (ch->flags &= ~nandflags);
}

/**
 * ch_save_flags: - reset flags for private structure
 * @ch: private structure
 */
static void
ch_save_flags(struct ch *ch)
{
	ch->oldflags = ch->flags;
}

/**
 * ch_restore_flags: - reset flags for private structure
 * @ch: private structure
 */
static int
ch_restore_flags(struct ch *ch, queue_t *q)
{
	return ch_set_flags(ch, q, ch->oldflags);
}

/*
 *  ===========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  CH Provider -> CH User primitives.
 *
 *  ===========================================================================
 */

/**
 * ch_info_ack: - issue CH_INFO_ACK primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_info_ack *p;
	struct CH_parms_circuit *c;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sizeof(*c), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_INFO_ACK;
		p->ch_addr_length = 0;
		p->ch_addr_offset = 0;
		p->ch_parm_length = sizeof(*c);
		p->ch_parm_offset = sizeof(*p);
		p->ch_prov_flags = 0;
		p->ch_prov_class = CH_CIRCUIT;
		p->ch_style = CH_STYLE1;
		p->ch_version = CH_VERSION;
		p->ch_state = ch_get_state(ch);
		mp->b_wptr += sizeof(*p);
		c = (typeof(c)) mp->b_wptr;
		c->cp_type = CH_PARMS_CIRCUIT;
		c->cp_encoding = CH_ENCODING_G711_PCM_U;	/* encoding */
		c->cp_block_size = 64;	/* data block size (bits) */
		c->cp_samples = 8;	/* samples per block */
		c->cp_sample_size = 8;	/* sample size (bits) */
		c->cp_rate = 8000;	/* channel clock rate (samples/second) */
		c->cp_tx_channels = 1;	/* number of tx channels */
		c->cp_rx_channels = 1;	/* number of rx channels */
		c->cp_opt_flags = CH_PARM_OPT_CLRCH;	/* options flags */
		mp->b_wptr += sizeof(*c);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_INFO_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_ack: - issue CH_OPTMGMT_ACK primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: management flags
 * @opt_ptr: options pointer
 * @opt_len: options length
 */
static inline int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *msg, int flags, caddr_t opt_ptr, size_t opt_len)
{
	struct CH_optmgmt_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_ACK;
		p->ch_opt_length = opt_len;	/* FIXME */
		p->ch_opt_offset = opt_len ? sizeof(*p) : 0;	/* FIXME */
		p->ch_mgmt_flags = 0;	/* FIXME */
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_OPTMGMT_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_ok_ack: - issue CH_OK_ACK primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *msg, int prim)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = prim;
		switch (prim) {
		case CH_ATTACH_REQ:
			p->ch_state = ch_set_state(ch, q, CHS_ATTACHED);
			break;
		case CH_DETACH_REQ:
			p->ch_state = ch_set_state(ch, q, CHS_DETACHED);
			break;
		default:
			p->ch_state = ch_get_state(ch);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_OK_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_error_ack: - issue CH_ERROR_ACK primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error number
 */
static int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *msg, int prim, int error)
{
	struct CH_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ERROR_ACK;
		p->ch_error_primitive = prim;
		p->ch_error_type = error < 0 ? CHSYSERR : error;
		p->ch_unix_error = error < 0 ? -error : 0;
		p->ch_state = ch_restore_state(ch, q);
		mp->b_wptr += sizeof(*p);
		ch_restore_flags(ch, q);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_ERROR_ACK");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_error_reply: - reply with an error
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error number
 *
 * This is essentially the same as ch_error_ack() except that typical queue
 * return codes are filtered and returned directly.
 */
static int
ch_error_reply(struct ch *ch, queue_t *q, mblk_t *msg, int prim, int error)
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
	return ch_error_ack(ch, q, msg, prim, error);
}

/**
 * ch_enable_con: - issue CH_ENABLE_CON primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ENABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_ENABLE_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_con: - issue CH_DISABLE_CON primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_DISABLE_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_connect_con: - issue CH_CONNECT_CON primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @slot: slot connected
 */
static int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *msg, int flags, int slot)
{
	struct CH_connect_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_CONNECT_CON;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_CONNECT_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_data_ind: - issue CH_DATA_IND primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @slot: slot to which data belongs
 * @dp: data portion
 */
static int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *msg, int slot, mblk_t *dp)
{
	struct CH_data_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DATA_IND;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_DATA_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_ind: - issue CH_DISCONNECT_INT primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @cause: cause for disconnect
 * @slot: slot disconnected
 */
static inline int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *msg, int flags, int cause, int slot)
{
	struct CH_disconnect_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_IND;
		p->ch_conn_flags = flags;
		p->ch_cause = cause;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_DISCONNECT_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_con: - issue CH_DISCONNECT_CON primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @slot: slot disconnected
 */
static int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *msg, int flags, int slot)
{
	struct CH_disconnect_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_CON;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_DISCONNECT_CON");
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_ind: - issue CH_DISABLE_IND primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cause: cause for disable
 */
static inline int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *msg, int cause)
{
	struct CH_disable_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_IND;
		p->ch_cause = cause;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_DISABLE_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_event_ind: - issue CH_EVENT_IND primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @event: event to indicate
 * @slot: CH slot for event
 */
static inline int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *msg, int event, int slot)
{
	struct CH_event_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_EVENT_IND;
		p->ch_event = event;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- CH_EVENT_IND");
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
ch_attach(struct ch *ch, queue_t *q, mblk_t *msg)
{
	int err;

	ch_set_state(ch, q, CHS_WACK_AREQ);
	err = ch_ok_ack(ch, q, msg, CH_ATTACH_REQ);
	return (err);
}

static int
ch_enable(struct ch *ch, queue_t *q, mblk_t *msg)
{
	int err;

	ch_set_state(ch, q, CHS_WCON_EREQ);
	err = ch_enable_con(ch, q, msg);
	return (err);
}

static int
ch_connect(struct ch *ch, queue_t *q, mblk_t *msg, int flags, int slot)
{
	int err;

	ch_set_state(ch, q, CHS_WCON_CREQ);
	ch_or_flags(ch, flags);
	err = ch_connect_con(ch, q, msg, flags, slot);
	return (err);
}

static int
ch_data(struct ch *ch, queue_t *q, mblk_t *msg, int slot, mblk_t *dp)
{
	int err;

	if ((ch->flags & CHF_TX_DIR) && (ch->other->flags & CHF_RX_DIR)) {
		err = ch_data_ind(ch->other, q, msg, slot, dp);
		return (err);
	}
	freemsg(dp);
	return (0);
}

static int
ch_disconnect(struct ch *ch, queue_t *q, mblk_t *msg, int flags, int slot)
{
	int err;

	ch_set_state(ch, q, CHS_WCON_DREQ);
	ch_nand_flags(ch, flags);
	err = ch_disconnect_con(ch, q, msg, flags, slot);
	return (err);
}

static int
ch_disable(struct ch *ch, queue_t *q, mblk_t *msg)
{
	int err;

	ch_set_state(ch, q, CHS_WCON_RREQ);
	err = ch_disable_con(ch, q, msg);
	return (err);
}

static int
ch_detach(struct ch *ch, queue_t *q, mblk_t *msg)
{
	int err;

	ch_set_state(ch, q, CHS_WACK_UREQ);
	err = ch_ok_ack(ch, q, msg, CH_DETACH_REQ);
	return (err);
}

/*
 *  ===========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  CH User -> CH Provider primitives.
 *
 *  ===========================================================================
 */

/**
 * ch_info_req: - process CH_INFO_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_info_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_INFO_REQ, err);
}

/**
 * ch_optmgmt_req: - process CH_OPTMGMT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_optmgmt_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + p->ch_opt_offset + p->ch_opt_length)
		goto badopt;
	switch (p->ch_mgmt_flags) {
	case CH_SET_OPT:
	case CH_GET_OPT:
	case CH_NEGOTIATE:
	case CH_DEFAULT:
		break;
	default:
		goto badflag;
	}
      badflag:
	err = CHBADFLAG;
	goto error;
      badopt:
	err = CHBADOPT;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_OPTMGMT_REQ, err);
}

/**
 * ch_attach_req: - process CH_ATTACH_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_attach_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	if (ch_get_state(ch) != CHS_DETACHED
	    && (ch_get_state(ch) != CHS_ATTACHED || p->ch_addr_length == 0))
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + p->ch_addr_offset + p->ch_addr_length)
		goto badaddr;
	/* Note that we do not need to support CH_ATTACH_REQ because this is a style1 driver only,
	   but if the address is null, simply move to the appropriate state. */
	return ch_attach(ch, q, mp);
      badaddr:
	err = CHBADADDR;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_ATTACH_REQ, err);
}

/**
 * ch_enable_req: - process CH_ENABLE_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ATTACHED)
		goto outstate;
	if ((err = ch_enable(ch, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = CHOUTSTATE;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_ENABLE_REQ, err);
}

/**
 * ch_connect_req: - process CH_CONNECT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ENABLED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	if ((p->ch_conn_flags & ~(CHF_BOTH_DIR)) || ((p->ch_conn_flags & (CHF_BOTH_DIR)) == 0))
		goto badflag;
	if (p->ch_slot != 0)
		goto badaddr;
	if ((err = ch_connect(ch, q, mp, p->ch_conn_flags, p->ch_slot)) != 0)
		goto error;
	return (0);
      badaddr:
	err = CHBADADDR;
	goto error;
      badflag:
	err = CHBADFLAG;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_CONNECT_REQ, err);
}

/**
 * ch_data_req: - process CH_DATA_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_CONNECTED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	if (p->ch_slot != 0)
		goto badaddr;
	if ((err = ch_data(ch, q, NULL, p->ch_slot, mp->b_cont)) != 0)
		goto error;
	freeb(mp);
	return (0);
      badaddr:
	err = CHBADADDR;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_DATA_REQ, err);
}

/**
 * ch_disconnect_req: - process CH_DISCONNECT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_CONNECTED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	if ((p->ch_conn_flags & ~(CHF_BOTH_DIR)) && ((p->ch_conn_flags & (CHF_BOTH_DIR)) == 0))
		goto badflag;
	if (p->ch_slot != 0)
		goto badaddr;
	if ((err = ch_disconnect(ch, q, mp, p->ch_conn_flags, p->ch_slot)) != 0)
		goto error;
	return (0);
      badaddr:
	err = CHBADADDR;
	goto error;
      badflag:
	err = CHBADFLAG;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_DISCONNECT_REQ, err);
}

/**
 * ch_disable_req: - process CH_DISABLE_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ENABLED)
		goto outstate;
	if ((err = ch_disable(ch, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = CHOUTSTATE;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_DISABLE_REQ, err);
}

/**
 * ch_detach_req: - process CH_DETACH_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_detach_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ATTACHED)
		goto outstate;
	if ((err = ch_detach(ch, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = CHOUTSTATE;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, CH_DETACH_REQ, err);
}

/**
 * ch_other_req: - process CH_???_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
ch_other_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      badprim:
	err = CHBADPRIM;
	goto error;
      error:
	return ch_error_reply(ch, q, mp, *p, err);
}

/*
 *  ===========================================================================
 *
 *  INPUT-OUTPUT CONTROLS
 *
 *  ===========================================================================
 */

/**
 * ch_testconfig: - test CH cnfiguration for validity
 * @ch: (locked) private structure
 * @arg: configuration to test
 */
static int
ch_testconfig(struct ch *ch, struct ch_config *arg)
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

#ifndef CHF_EVT_ALL
#define CHF_EVT_ALL \
	( 0 \
	  | CHF_EVT_DCD_CHANGE \
	  | CHF_EVT_DSR_CHANGE \
	  | CHF_EVT_DTR_CHANGE \
	  | CHF_EVT_RTS_CHANGE \
	  | CHF_EVT_CTS_CHANGE \
	  | CHF_EVT_RI_CHANGE \
	  | CHF_EVT_YEL_ALARM \
	  | CHF_EVT_BLU_ALARM \
	  | CHF_EVT_RED_ALARM \
	  | CHF_EVT_NO_ALARM \
	)
#endif				/* CHF_EVT_ALL */

/**
 * ch_setnotify: - set CH notification bits
 * @ch: (locked) private structure
 * @arg: notification arguments
 */
static int
ch_setnotify(struct ch *ch, struct ch_notify *arg)
{
	if (arg->events & ~(CHF_EVT_ALL))
		return (EINVAL);
	arg->events = arg->events | ch->ch.notify.events;
	return (0);
}

/**
 * ch_clrnotify: - clear CH notification bits
 * @ch: (locked) private structure
 * @arg: notification arguments
 */
static int
ch_clrnotify(struct ch *ch, struct ch_notify *arg)
{
	if (arg->events & ~(CHF_EVT_ALL))
		return (EINVAL);
	arg->events = ~arg->events & ch->ch.notify.events;
	return (0);
}

/**
 * ch_manage: - perform CH management action
 * @ch: (locked) private structure
 * @arg: management arguments
 */
static int
ch_manage(struct ch *ch, struct ch_mgmt *arg)
{
	switch (arg->cmd) {
	case CH_MGMT_RESET:
		/* FIXME: reset */
		break;
	default:
		return (EINVAL);
	}
	return (0);
}

/**
 * ch_ioctl: - process CH M_IOCTL message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the CH input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static int
ch_ioctl(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(CH_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCSCONFIG)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.config), false)))
			goto enobufs;
		bcopy(&ch->ch.config, bp->b_rptr, sizeof(ch->ch.config));
		break;
	case _IOC_NR(CH_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCGCONFIG)");
		size = sizeof(ch->ch.config);
		break;
	case _IOC_NR(CH_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCTCONFIG)");
		size = sizeof(ch->ch.config);
		break;
	case _IOC_NR(CH_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCCCONFIG)");
		size = sizeof(ch->ch.config);
		break;
	case _IOC_NR(CH_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCGSTATEM)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.statem), false)))
			goto enobufs;
		bcopy(&ch->ch.statem, bp->b_rptr, sizeof(ch->ch.statem));
		break;
	case _IOC_NR(CH_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCCMRESET)");
		/* FIXME: reset the state machine */
		break;
	case _IOC_NR(CH_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCGSTATSP)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.statsp), false)))
			goto enobufs;
		bcopy(&ch->ch.statsp, bp->b_rptr, sizeof(ch->ch.statsp));
		break;
	case _IOC_NR(CH_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCSSTATSP)");
		size = sizeof(ch->ch.statsp);
		break;
	case _IOC_NR(CH_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCGSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.stats), false)))
			goto enobufs;
		bcopy(&ch->ch.stats, bp->b_rptr, sizeof(ch->ch.stats));
		break;
	case _IOC_NR(CH_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCCSTATS)");
		size = sizeof(ch->ch.stats);
		break;
	case _IOC_NR(CH_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCGNOTIFY)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.notify), false)))
			goto enobufs;
		bcopy(&ch->ch.notify, bp->b_rptr, sizeof(ch->ch.notify));
		break;
	case _IOC_NR(CH_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCSNOTIFY)");
		size = sizeof(ch->ch.notify);
		break;
	case _IOC_NR(CH_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCCNOTIFY)");
		size = sizeof(ch->ch.notify);
		break;
	case _IOC_NR(CH_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(CH_IOCCMGMT)");
		size = sizeof(struct ch_mgmt);
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
 * ch_copyin: - process CH M_IOCDATA message
 * @ch: (locked) private structure
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
ch_copyin(struct ch *ch, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(CH_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(CH_IOCSCONFIG)");
		if ((err = ch_testconfig(ch, (struct ch_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &ch->ch.config, sizeof(ch->ch.config));
		break;
	case _IOC_NR(CH_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(CH_IOCTCONFIG)");
		err = ch_testconfig(ch, (struct ch_config *) bp->b_rptr);
		break;
	case _IOC_NR(CH_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(CH_IOCCCONFIG)");
		if ((err = ch_testconfig(ch, (struct ch_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &ch->ch.config, sizeof(ch->ch.config));
		break;
	case _IOC_NR(CH_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(CH_IOCSSTATSP)");
		bcopy(bp->b_rptr, &ch->ch.statsp, sizeof(ch->ch.statsp));
		break;
	case _IOC_NR(CH_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(CH_IOCSNOTIFY)");
		if ((err = ch_setnotify(ch, (struct ch_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &ch->ch.notify, sizeof(ch->ch.notify));
		break;
	case _IOC_NR(CH_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(CH_IOCCNOTIFY)");
		if ((err = ch_clrnotify(ch, (struct ch_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &ch->ch.notify, sizeof(ch->ch.notify));
		break;
	case _IOC_NR(CH_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(CH_IOCCMGMT)");
		err = ch_manage(ch, (struct ch_mgmt *) bp->b_rptr);
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
 * ch_copyout: - process CH M_IOCDATA message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final stop which is a simple copy done operation.
 */
static int
ch_copyout(struct ch *ch, queue_t *q, mblk_t *mp, mblk_t *dp)
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
 * ch_m_data: - process M_DATA message
 * @ch: private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static fastcall int
ch_m_data(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_data(ch, q, NULL, 0, mp);
}

/**
 * ch_m_proto: = process M_(PC)PROTO message
 * @ch: private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static fastcall int
ch_m_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);

	ch_save_state(ch);
	ch_save_flags(ch);

	switch (*(ch_ulong *) mp->b_rptr) {
	case CH_INFO_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_INFO_REQ");
		rtn = ch_info_req(ch, q, mp);
		break;
	case CH_OPTMGMT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_OPTMGMT_REQ");
		rtn = ch_optmgmt_req(ch, q, mp);
		break;
	case CH_ATTACH_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_ATTACH_REQ");
		rtn = ch_attach_req(ch, q, mp);
		break;
	case CH_ENABLE_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_ENABLE_REQ");
		rtn = ch_enable_req(ch, q, mp);
		break;
	case CH_CONNECT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_CONNECT_REQ");
		rtn = ch_connect_req(ch, q, mp);
		break;
	case CH_DATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> CH_DATA_REQ");
		rtn = ch_data_req(ch, q, mp);
		break;
	case CH_DISCONNECT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_DISCONNECT_REQ");
		rtn = ch_disconnect_req(ch, q, mp);
		break;
	case CH_DISABLE_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_DISABLE_REQ");
		rtn = ch_disable_req(ch, q, mp);
		break;
	case CH_DETACH_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_DETACH_REQ");
		rtn = ch_detach_req(ch, q, mp);
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> CH_????_REQ");
		rtn = ch_other_req(ch, q, mp);
		break;
	}
	if (rtn) {
		ch_restore_state(ch, q);
		ch_restore_flags(ch, q);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * ch_m_flush: - process M_FLUSH message
 * @q: active queue
 * @mp: the M_FLUSH message
 *
 * Avoid having tot push pipemod.  It we are the bottommost module over a pipe
 * twist then perform the actions of pipemod.  This means that the ch-pmod
 * module must be pushed over the same side of a pipe as pipemod, if pipemod
 * is pushed at all.
 */
static fastcall int
ch_m_flush(queue_t *q, mblk_t *mp)
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
 * ch_m_ioctl: - process M_IOCTL message
 * @ch: private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 */
static fastcall int
ch_m_ioctl(struct ch *ch, queue_t *q, mblk_t *mp)
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
	case CH_IOC_MAGIC:
		err = ch_ioctl(ch, q, mp);
		break;
	}
	mi_unlock(priv);
	return (err);
}

/**
 * ch_m_iocdata: - process M_IOCDATA messages
 * @ch: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 */
static fastcall int
ch_m_iocdata(struct ch *ch, queue_t *q, mblk_t *mp)
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
		case CH_IOC_MAGIC:
			err = ch_copyin(ch, q, mp, dp);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		case CH_IOC_MAGIC:
			err = ch_copyout(ch, q, mp, dp);
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
 * ch_m_rse: - process M_(PC)RSE message
 * @ch: private structure
 * @q: active queue
 * @mp: the M_(PC)RSE message
 */
static fastcall int
ch_m_rse(struct ch *ch, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

#ifndef DB_TYPE
#define DB_TYPE(mp) (mp->b_datap->db_type)
#endif

/**
 * ch_m_other: - process other STREAMS message
 * @ch: private structure
 * @q: active queue
 * @mp: other STREAMS message
 */
static fastcall int
ch_m_other(struct ch *ch, queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
ch_msg_slow(struct ch *ch, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return ch_m_data(ch, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ch_m_proto(ch, q, mp);
	case M_FLUSH:
		return ch_m_flush(q, mp);
	case M_IOCTL:
		return ch_m_ioctl(ch, q, mp);
	case M_IOCDATA:
		return ch_m_iocdata(ch, q, mp);
	case M_RSE:
	case M_PCRSE:
		return ch_m_rse(ch, q, mp);
	default:
		return ch_m_other(ch, q, mp);
	}
}

/**
 * ch_m_data_fast: - process M_DATA message
 * @ch: private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
ch_m_data_fast(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_m_data(ch, q, mp);
}

/**
 * ch_msg: - process STREAMS message
 * @q: active queue
 * @mp: the message
 */
static inline fastcall int
ch_msg(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		return ch_m_data_fast(ch, q, mp);
	return ch_msg_slow(ch, q, mp);
}

/**
 * ch_putp: - canonical put procedure
 * @q: active queue
 * @mp: message to put
 */
static streamscall __hot_in int
ch_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || ch_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * ch_srvp: - canonical service procedure
 * @q: active queue
 */
static streamscall __hot_read int
ch_srvp(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (ch_msg(q, mp)) {
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

static caddr_t ch_opens = NULL;

/**
 * ch_qopen: - module queue open procedure
 * @q: read queue of queue pair
 * @devp: device number of driver
 * @oflags: flags to open(2) call
 * @sflag: STREAMS flag
 * @crp: credientials of opening process
 */
static streamscall int
ch_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct ch_pair *chp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((err = mi_open_comm(&ch_opens, sizeof(*chp), q, devp, oflags, sflag, crp)))
		return (err);

	chp = (typeof(chp)) q->q_ptr;
	bzero(chp, sizeof(*chp));

	/* initialize the structure */

	qprocson(q);
	return (0);
}

/**
 * ch_qclose: - module queue close procedure
 * @q: queue pair
 * @oflags: flags to open(2) call
 * @crp: credentials of closing process
 */
static streamscall int
ch_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	mi_close_comm(&ch_opens, q);
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
MODULE_PARM_DESC(modid, "Module ID for CH-PMOD module. (0 for allocation.)");

static struct qinit ch_rinit = {
	.qi_putp = ch_putp,
	.qi_srvp = ch_srvp,
	.qi_qopen = ch_qopen,
	.qi_qclose = ch_qclose,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_rstat,
};
static struct qinit ch_winit = {
	.qi_putp = ch_putp,
	.qi_srvp = ch_srvp,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_wstat,
};
static struct streamtab ch_pmodinfo = {
	.st_rdinit = &ch_rinit,
	.st_wrinit = &ch_winit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw ch_fmod = {
	.f_name = MOD_NAME,
	.f_str = &ch_pmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
ch_pmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&ch_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}
static __exit void
ch_pmodterminate(void)
{
	int err;

	if ((err = unregister_strmod(&ch_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	return;
}

module_init(ch_pmodinit);
module_exit(ch_pmodterminate);

#endif				/* LINUX */
