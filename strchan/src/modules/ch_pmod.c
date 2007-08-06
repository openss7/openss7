/*****************************************************************************

 @(#) $RCSfile: ch_pmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/06 04:44:06 $

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

 Last Modified $Date: 2007/08/06 04:44:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ch_pmod.c,v $
 Revision 0.9.2.3  2007/08/06 04:44:06  brian
 - rework of pipe-based emulation modules

 Revision 0.9.2.2  2007/08/03 13:35:52  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.1  2007/07/14 01:13:38  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: ch_pmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/06 04:44:06 $"

static char const ident[] =
    "$RCSfile: ch_pmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/06 04:44:06 $";

/*
 *  This is CH-PMOD.  This is a pushable STREAMS module that can be pushed on one end of a
 *  STREAMS-based pipe to simulate a CH driver stream.  This is a PPA Style 1 stream.  Each end of
 *  the STREAMS-based pipe will present the CHI interface.
 */

#define _LFS_SOURCE 1
#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1
#define _SUN_SOURCE 1

#include <sys/os7/compat.h>

#include <sys/chi.h>
#include <sys/chi_ioctl.h>

/* don't really want the SUN version of these */
#undef freezestr
#undef unfreezestr

#define CH_DESCRIP	"CH (Channel) STREAMS PIPE MODULE."
#define CH_REVISION	"OpenSS7 $RCSfile: ch_pmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/08/06 04:44:06 $"
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
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
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
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

struct st {
	int l_state;			/* local management state */
	int i_state;			/* interface state */
	int i_flags;			/* interface flags */
};

struct ch_pair;

struct ch {
	struct ch_pair *pair;
	struct ch *other;
	queue_t *oq;
	struct st state, oldstate;
	uint reqflags;
	mblk_t *tick;
	ulong interval;
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

#define PRIV(q)	    ((struct ch_pair *)(q)->q_ptr)
#define CH_PRIV(q)  (((q)->q_flag & QREADR) ? &PRIV(q)->r_priv : &PRIV(q)->w_priv)

#define STRLOGNO	0	/* log Stream errors */
#define STRLOGIO	1	/* log Stream input-output */
#define STRLOGST	2	/* log Stream state transitions */
#define STRLOGTO	3	/* log Stream timeouts */
#define STRLOGRX	4	/* log Stream primitives received */
#define STRLOGTX	5	/* log Stream primitives issued */
#define STRLOGTE	6	/* log Stream timer events */
#define STRLOGDA	7	/* log Stream data */

/**
 * ch_iocname: display CH ioctl command name
 * @cmd: ioctl command
 */
static const char *
ch_iocname(int cmd)
{
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(CH_IOCGCONFIG):
		return ("CH_IOCGCONFIG");
	case _IOC_NR(CH_IOCSCONFIG):
		return ("CH_IOCSCONFIG");
	case _IOC_NR(CH_IOCTCONFIG):
		return ("CH_IOCTCONFIG");
	case _IOC_NR(CH_IOCCCONFIG):
		return ("CH_IOCCCONFIG");
	case _IOC_NR(CH_IOCGSTATEM):
		return ("CH_IOCGSTATEM");
	case _IOC_NR(CH_IOCCMRESET):
		return ("CH_IOCCMRESET");
	case _IOC_NR(CH_IOCGSTATSP):
		return ("CH_IOCGSTATSP");
	case _IOC_NR(CH_IOCSSTATSP):
		return ("CH_IOCSSTATSP");
	case _IOC_NR(CH_IOCGSTATS):
		return ("CH_IOCGSTATS");
	case _IOC_NR(CH_IOCCSTATS):
		return ("CH_IOCCSTATS");
	case _IOC_NR(CH_IOCGNOTIFY):
		return ("CH_IOCGNOTIFY");
	case _IOC_NR(CH_IOCSNOTIFY):
		return ("CH_IOCSNOTIFY");
	case _IOC_NR(CH_IOCCNOTIFY):
		return ("CH_IOCCNOTIFY");
	case _IOC_NR(CH_IOCCMGMT):
		return ("CH_IOCCMGMT");
	default:
		return ("(unknown ioctl)");
	}
}

/**
 * ch_primname: display CH primitive name
 * @prim: the primitive to display
 */
static const char *
ch_primname(ch_ulong prim)
{
	switch (prim) {
	case CH_INFO_REQ:
		return ("CH_INFO_REQ");
	case CH_OPTMGMT_REQ:
		return ("CH_OPTMGMT_REQ");
	case CH_ATTACH_REQ:
		return ("CH_ATTACH_REQ");
	case CH_ENABLE_REQ:
		return ("CH_ENABLE_REQ");
	case CH_CONNECT_REQ:
		return ("CH_CONNECT_REQ");
	case CH_DATA_REQ:
		return ("CH_DATA_REQ");
	case CH_DISCONNECT_REQ:
		return ("CH_DISCONNECT_REQ");
	case CH_DISABLE_REQ:
		return ("CH_DISABLE_REQ");
	case CH_DETACH_REQ:
		return ("CH_DETACH_REQ");
	case CH_INFO_ACK:
		return ("CH_INFO_ACK");
	case CH_OPTMGMT_ACK:
		return ("CH_OPTMGMT_ACK");
	case CH_OK_ACK:
		return ("CH_OK_ACK");
	case CH_ERROR_ACK:
		return ("CH_ERROR_ACK");
	case CH_ENABLE_CON:
		return ("CH_ENABLE_CON");
	case CH_CONNECT_CON:
		return ("CH_CONNECT_CON");
	case CH_DATA_IND:
		return ("CH_DATA_IND");
	case CH_DISCONNECT_IND:
		return ("CH_DISCONNECT_IND");
	case CH_DISCONNECT_CON:
		return ("CH_DISCONNECT_CON");
	case CH_DISABLE_IND:
		return ("CH_DISABLE_IND");
	case CH_DISABLE_CON:
		return ("CH_DISABLE_CON");
	case CH_EVENT_IND:
		return ("CH_EVENT_IND");
	default:
		return ("(unknown primitive)");
	}
}

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
 * ch_get_l_state: - get management state for private structure
 * @ch: private structure
 */
static int
ch_get_l_state(struct ch *ch)
{
	return (ch->state.l_state);
}

/**
 * ch_set_l_state: - set management state for private structure
 * @ch: private structure
 * @newstate: new state
 */
static int
ch_set_l_state(struct ch *ch, int newstate)
{
	int oldstate = ch->state.l_state;

	if (newstate != oldstate) {
		ch->state.l_state = newstate;
		mi_strlog(ch->oq, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
	}
	return (newstate);
}

static int
ch_save_l_state(struct ch *ch)
{
	return ((ch->oldstate.l_state = ch_get_l_state(ch)));
}

static int
ch_restore_l_state(struct ch *ch)
{
	return ch_set_l_state(ch, ch->oldstate.l_state);
}

/**
 * ch_get_i_state: - get state for private structure
 * @ch: private structure
 */
static int
ch_get_i_state(struct ch *ch)
{
	return (ch->state.i_state);
}

/**
 * ch_set_i_state: - set state for private structure
 * @ch: private structure
 * @newstate: new state
 */
static int
ch_set_i_state(struct ch *ch, int newstate)
{
	int oldstate = ch->state.i_state;

	if (newstate != oldstate) {
		/* Note that if we are setting away from a connecting or disconnecting state then
		   we must also reset the direction request flags. */
		switch (oldstate) {
		case CHS_WCON_CREQ:
		case CHS_WCON_DREQ:
			ch->reqflags = 0;
			break;
		case CHS_CONNECTED:
			if (ch_get_i_state(ch->other) != CHS_CONNECTED) {
				mi_timer_stop(ch->tick);
				qenable(ch->oq);
			}
			break;
		}
		switch (newstate) {
		case CHS_CONNECTED:
			if (!mi_timer_running(ch->tick))
				mi_timer(ch->oq, ch->tick, ch->interval);
			break;
		}
		ch->state.i_state = newstate;
		mi_strlog(ch->oq, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
	}
	return (newstate);
}

static int
ch_save_i_state(struct ch *ch)
{
	return ((ch->oldstate.i_state = ch_get_i_state(ch)));
}

static int
ch_restore_i_state(struct ch *ch)
{
	return ch_set_i_state(ch, ch->oldstate.i_state);
}

/**
 * ch_save_state: - reset state for private structure
 * @ch: private structure
 */
static void
ch_save_state(struct ch *ch)
{
	ch_save_l_state(ch);
	ch_save_i_state(ch);
}

/**
 * ch_restore_state: - reset state for private structure
 * @ch: private structure
 */
static int
ch_restore_state(struct ch *ch)
{
	ch_restore_l_state(ch);
	return ch_restore_i_state(ch);
}

/**
 * ch_get_flags: - get flags for private structure
 * @ch: private structure
 */
static inline int
ch_get_flags(struct ch *ch)
{
	return ch->state.i_flags;
}

/**
 * ch_set_flags: - set flags for private structure
 * @ch: private structure
 * @newflags: new flags
 */
static int
ch_set_flags(struct ch *ch, int newflags)
{
	int oldflags = ch->state.i_flags;

	if (newflags != oldflags) {
		ch->state.i_flags = newflags;
	}
	return (newflags);
}

static inline int
ch_or_flags(struct ch *ch, int orflags)
{
	return (ch->state.i_flags |= orflags);
}

static inline int
ch_nand_flags(struct ch *ch, int nandflags)
{
	return (ch->state.i_flags &= ~nandflags);
}

/**
 * ch_save_flags: - reset flags for private structure
 * @ch: private structure
 */
static void
ch_save_flags(struct ch *ch)
{
	ch->oldstate.i_flags = ch_get_flags(ch);
}

/**
 * ch_restore_flags: - reset flags for private structure
 * @ch: private structure
 */
static int
ch_restore_flags(struct ch *ch)
{
	return ch_set_flags(ch, ch->oldstate.i_flags);
}

static void
ch_save_total_state(struct ch *ch)
{
	ch_save_flags(ch->other);
	ch_save_state(ch->other);
	ch_save_flags(ch);
	ch_save_state(ch);
}

static int
ch_restore_total_state(struct ch *ch)
{
	ch_restore_flags(ch->other);
	ch_restore_state(ch->other);
	ch_restore_flags(ch);
	return ch_restore_state(ch);
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  CH Provider -> CH User primitives.
 *
 *  =========================================================================
 */
#if 0
/**
 * m_error: - issue M_ERROR for stream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: error to indicate
 */
static int
m_error(struct ch *ch, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(ch->oq, 0, SL_ERROR, "<- M_ERROR %d", err);
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

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
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_INFO_ACK;
		p->ch_addr_length = 0;
		p->ch_addr_offset = sizeof(*p);
		p->ch_parm_length = sizeof(*c);
		p->ch_parm_offset = sizeof(*p);
		p->ch_prov_flags = 0;
		p->ch_prov_class = CH_CIRCUIT;
		p->ch_style = CH_STYLE1;
		p->ch_version = CH_VERSION;
		p->ch_state = ch_get_i_state(ch);
		mp->b_wptr += sizeof(*p);
		c = (typeof(c)) mp->b_wptr;
		c->cp_type = ch->ch.config.type;
		c->cp_encoding = ch->ch.config.encoding;
		c->cp_block_size = ch->ch.config.block_size;
		c->cp_samples = ch->ch.config.samples;
		c->cp_sample_size = ch->ch.config.sample_size;
		c->cp_rate = ch->ch.config.rate;
		c->cp_tx_channels = ch->ch.config.tx_channels;
		c->cp_rx_channels = ch->ch.config.rx_channels;
		c->cp_opt_flags = ch->ch.config.opt_flags;
		mp->b_wptr += sizeof(*c);
		freemsg(msg);
		mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_INFO_ACK");
		putnext(ch->oq, mp);
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
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_ACK;
		p->ch_opt_length = opt_len;	/* FIXME */
		p->ch_opt_offset = sizeof(*p);	/* FIXME */
		p->ch_mgmt_flags = 0;	/* FIXME */
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_OPTMGMT_ACK");
		putnext(ch->oq, mp);
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
 *
 * There are only ever two primitives that are acknowledged with CH_OK_ACK and
 * those are CH_ATTACH_REQ and CH_DETACH_REQ.
 */
static int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *msg, int prim)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = prim;
		switch (prim) {
		case CH_ATTACH_REQ:
			p->ch_state = ch_set_i_state(ch, CHS_ATTACHED);
			break;
		case CH_DETACH_REQ:
			p->ch_state = ch_set_i_state(ch, CHS_DETACHED);
			break;
		default:
			p->ch_state = ch_get_i_state(ch);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_OK_ACK");
		putnext(ch->oq, mp);
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
 *
 * Only called by ch_error_reply(), but then, it can be invoked by the
 * handling procedure for just about any CH-primitive passed to either side of
 * the pipe module.  State is restored by falling back to the last checkpoint
 * state.
 */
static int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *msg, int prim, int error)
{
	struct CH_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ERROR_ACK;
		p->ch_error_primitive = prim;
		p->ch_error_type = error < 0 ? CHSYSERR : error;
		p->ch_unix_error = error < 0 ? -error : 0;
		p->ch_state = ch_restore_total_state(ch);
		mp->b_wptr += sizeof(*p);
		ch_restore_flags(ch);
		freemsg(msg);
		mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_ERROR_ACK");
		putnext(ch->oq, mp);
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
 * return codes are filtered and returned directly.  ch_error_reply() and
 * a ch_error_ack() can be invoked by any CH-primitive being issued to one or
 * the other side of the pipe module.
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

	if (likely(canputnext(ch->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_ENABLE_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			ch_set_i_state(ch, CHS_ENABLED);
			mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_ENABLE_CON");
			putnext(ch->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	if (likely(canputnext(ch->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISABLE_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			ch_set_i_state(ch, CHS_ATTACHED);
			mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_DISABLE_CON");
			putnext(ch->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * ch_connect_con: - issue CH_CONNECT_CON primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @slot: slot connected
 */
static int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *msg, int slot)
{
	struct CH_connect_con *p;
	mblk_t *mp;

	if (likely(canputnext(ch->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_CONNECT_CON;
			p->ch_conn_flags = ch->reqflags;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			ch_or_flags(ch, ch->reqflags);
			ch_set_i_state(ch, CHS_CONNECTED);
			mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_CONNECT_CON");
			putnext(ch->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	(void) ch_data_ind;
	if (likely(canputnext(ch->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DATA_IND;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_DATA_IND");
			putnext(ch->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	if (likely(canputnext(ch->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISCONNECT_IND;
			p->ch_conn_flags = flags;
			p->ch_cause = cause;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			ch_nand_flags(ch, flags);
			if ((ch->state.i_flags & CHF_BOTH_DIR) == 0)
				ch_set_i_state(ch, CHS_ENABLED);
			else
				ch_set_i_state(ch, CHS_CONNECTED);
			mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_DISCONNECT_IND");
			putnext(ch->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * ch_disconnect_con: - issue CH_DISCONNECT_CON primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @slot: slot disconnected
 */
static int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *msg, int slot)
{
	struct CH_disconnect_con *p;
	mblk_t *mp;

	if (likely(canputnext(ch->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISCONNECT_CON;
			p->ch_conn_flags = ch->reqflags;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			ch_nand_flags(ch, ch->reqflags);
			if ((ch_get_flags(ch) & CHF_BOTH_DIR) == 0)
				ch_set_i_state(ch, CHS_ENABLED);
			else
				ch_set_i_state(ch, CHS_CONNECTED);
			mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_DISCONNECT_CON");
			putnext(ch->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	if (likely(canputnext(ch->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISABLE_IND;
			p->ch_cause = cause;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			ch_set_i_state(ch, CHS_ATTACHED);
			ch_nand_flags(ch, CHF_BOTH_DIR);
			mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_DISABLE_IND");
			putnext(ch->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	if (likely(canputnext(ch->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_EVENT_IND;
			p->ch_event = event;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(ch->oq, STRLOGTX, SL_TRACE, "<- CH_EVENT_IND");
			putnext(ch->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  =========================================================================
 */

/**
 * ch_attach: - attach a channel user
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
ch_attach(struct ch *ch, queue_t *q, mblk_t *msg)
{
	int err;

	ch_set_i_state(ch, CHS_WACK_AREQ);
	switch (ch_get_i_state(ch->other)) {
	case CHS_UNUSABLE:
		/* These are unsuable states, but we are still allowed to attach locally. */
		break;
	case CHS_UNINIT:
	case CHS_DETACHED:
	case CHS_ATTACHED:
	case CHS_WCON_EREQ:
		/* These are ok states, just ack local attach. */
		break;
	case CHS_WCON_RREQ:
		/* This is a disabling state, just confirm it. */
		if ((err = ch_disable_con(ch->other, q, NULL)))
			return (err);
		break;
	case CHS_WCON_CREQ:
	case CHS_WCON_DREQ:
	case CHS_ENABLED:
	case CHS_CONNECTED:
		/* These are enabled and connected states, indicate a disable to move both ends to
		   the disabled state. */
		if ((err = ch_disable_ind(ch->other, q, NULL, 0)))
			return (err);
		break;
	case CHS_WACK_AREQ:
	case CHS_WACK_UREQ:
	case CHS_WACK_EREQ:
	case CHS_WACK_RREQ:
	case CHS_WACK_CREQ:
	case CHS_WACK_DREQ:
	default:
		/* These are erroneous states, but still ack locally. */
		mi_strlog(ch->oq, STRLOGNO, SL_ERROR | SL_TRACE, "attach in incorrect state");
		break;
	}
	return ch_ok_ack(ch, q, msg, CH_ATTACH_REQ);
}

/**
 * ch_enable: - enable a channel provider
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
ch_enable(struct ch *ch, queue_t *q, mblk_t *msg)
{
	int err, oflags;

	ch_set_i_state(ch, CHS_WCON_EREQ);
	switch (ch_get_i_state(ch->other)) {
	case CHS_UNINIT:
	case CHS_UNUSABLE:
		/* These are unusable states, refuse the enable. */
		return ch_disable_ind(ch, q, msg, 0);
	case CHS_DETACHED:
	case CHS_ATTACHED:
		/* In any of these stable states we need to wait until the other end confirms the
		   enable. */
		freemsg(msg);
		return (0);
	case CHS_WCON_EREQ:
		/* The other end is waiting for enable confirmation. */
		if ((err = ch_enable_con(ch->other, q, NULL)))
			return (err);
		break;
	case CHS_WCON_RREQ:
		/* The other end is waiting to confirm disable, complete disable and refuse enable
		   locally to move both ends to the disabled state. */
		if ((err = ch_disable_con(ch->other, q, NULL)))
			return (err);
		return ch_disable_ind(ch, q, msg, 0);
	case CHS_WCON_DREQ:
		/* The other end is waiting to disconnect, confirm it to move both ends to the
		   enabled state. */
		if ((err = ch_disconnect_con(ch->other, q, NULL, 0)))
			return (err);
		/* If the confirm does not result in a full disconnect, continue to disconnect. */
		if (ch_get_i_state(ch) != CHS_CONNECTED)
			break;
		/* fall through */
	case CHS_WCON_CREQ:
	case CHS_CONNECTED:
		/* The other end is waiting to connect or connected, disconnect it and confrim
		   locally to move both ends the enabled state. */
		oflags = ch->other->state.i_flags & CHF_BOTH_DIR;
		if ((err = ch_disconnect_ind(ch->other, q, NULL, oflags, 0, 0)))
			return (err);
		break;
	case CHS_ENABLED:
		/* The other end is enabled, just confirm. */
		break;
	case CHS_WACK_AREQ:
	case CHS_WACK_UREQ:
	case CHS_WACK_EREQ:
	case CHS_WACK_RREQ:
	case CHS_WACK_CREQ:
	case CHS_WACK_DREQ:
	default:
		/* These are erroneous states. */
		mi_strlog(ch->oq, STRLOGNO, SL_ERROR | SL_TRACE, "enable in incorrect state");
		return ch_disable_ind(ch, q, msg, 0);
	}
	return ch_enable_con(ch, q, msg);
}

/**
 * ch_connect: - connect a channel user
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @slot: slot to connect
 */
static int
ch_connect(struct ch *ch, queue_t *q, mblk_t *msg, int flags, int slot)
{
	int err, oflags;

	ch_set_i_state(ch, CHS_WCON_CREQ);
	ch->reqflags = flags;
	switch (ch_get_i_state(ch->other)) {
	case CHS_WCON_EREQ:
		/* The other end is enabling, confirm the enable and wait for connection. */
		return ch_enable_con(ch->other, q, msg);
	case CHS_WCON_RREQ:
		/* The other end is disabling, confirm the disable and refuse the connection. */
		if ((err = ch_disable_con(ch->other, q, NULL)))
			return (err);
		return ch_disconnect_ind(ch, q, msg, CHF_BOTH_DIR, 0, slot);
	case CHS_WCON_DREQ:
		/* The other end is disconnecting, confirm the disconnect and wait for connection. */
		return ch_disconnect_con(ch->other, q, msg, slot);
	case CHS_DETACHED:
	case CHS_ATTACHED:
		/* The other end is disabled, disable this end too. */
		return ch_disable_ind(ch, q, msg, 0);
	case CHS_CONNECTED:
	case CHS_ENABLED:
		/* The other end is enabled, or it is part connected but not for the direction that 
		   we want to connect, just wait for other end to connect. */
		freemsg(msg);
		return (0);
	case CHS_WCON_CREQ:
		/* is the local side connecting for the same directions for which the remote side
		   is waiting? */
		oflags = 0;
		oflags |= ((flags | ch->state.i_flags) & CHF_TX_DIR) ? CHF_RX_DIR : 0;
		oflags |= ((flags | ch->state.i_flags) & CHF_RX_DIR) ? CHF_TX_DIR : 0;
		if ((ch->other->reqflags & oflags) == ch->other->reqflags) {
			if ((err = ch_connect_con(ch->other, q, NULL, slot)))
				return (err);
			/* Next question is whether the local connection request is now satisfied. */
			oflags = 0;
			oflags |= (flags & CHF_TX_DIR) ? CHF_RX_DIR : 0;
			oflags |= (flags & CHF_RX_DIR) ? CHF_TX_DIR : 0;
			if ((oflags & ch->other->state.i_flags) == oflags)
				return ch_connect_con(ch, q, msg, slot);

		}
		/* otherwise, then the local side cannot proceed either. */
		freemsg(msg);
		return (0);
	case CHS_UNINIT:
	case CHS_UNUSABLE:
		/* These are unusable states, disable local interface. */
		return ch_disable_ind(ch, q, msg, 0);
	case CHS_WACK_AREQ:
	case CHS_WACK_UREQ:
	case CHS_WACK_EREQ:
	case CHS_WACK_RREQ:
	case CHS_WACK_CREQ:
	case CHS_WACK_DREQ:
	default:
		/* These are erroneous states, disable local interface. */
		mi_strlog(ch->oq, STRLOGNO, SL_ERROR | SL_TRACE, "connect in incorrect state");
		return ch_disable_ind(ch, q, msg, 0);
	}
}

/**
 * ch_block: - transmit a data block
 * @ch: (locked) private structure
 * @q: queue beyond which to pass message blocks
 */
static void
ch_block(struct ch *ch, queue_t *q)
{
	mblk_t *mp;
	pl_t pl;

	/* find the first M_DATA block on the queue, or M_PPROTO message block containing an
	   CH_DATA_REQ primitive. */
	pl = freezestr(q);
	for (mp = q->q_first; mp && DB_TYPE(mp) != M_DATA
	     && (DB_TYPE(mp) != M_PROTO || *(ch_ulong *) mp->b_rptr != CH_DATA_REQ);
	     mp = mp->b_next) ;
	rmvq(q, mp);
	unfreezestr(q, pl);
	if (mp) {
		if (canputnext(q)) {
			/* If it is an CH_DATA_REQ primitive, alter it to an CH_DATA_IND primitive. 
			 */
			if (DB_TYPE(mp) == M_PROTO)
				*(ch_ulong *) mp->b_rptr = CH_DATA_IND;
			putnext(q, mp);
		} else {
			freemsg(mp);
			ch->ch.stats.rx_buffer_overflows++;
			/* receive overrun condition */
		}
	} else {
		/* transmit underrun condition */
		ch->ch.stats.tx_buffer_overflows++;
		if (canputnext(q)) {
			uint blksize = ch->ch.config.block_size >> 3;

			/* prepare blank block */
			if ((mp = allocb(blksize, BPRI_MED))) {
				memset(mp->b_wptr, 0xfe, blksize);
				putnext(q, mp);
			} else {
				/* can't even send a blank, gonna be a really big slip. */
				ch->ch.stats.tx_underruns++;
			}
		} else {
			ch->ch.stats.rx_buffer_overflows++;
			/* receive overrun condition too */
		}
	}
}

/**
 * ch_runblocks: - run blocks off of tick timer
 * @ch: pair of queues to run
 *
 * This is fairly simple and straitghtforward: each interval generate the
 * number of blocks due for the interval.  However, interleave block
 * generation to appear more synchronous multiplex like.  That is, give
 * adjacent modules and drivers the ability to reply to one transmitted block
 * and then process one receive block.  The interval is typically 10ms (to
 * handle older 2.4 100Hz tick timers) and blocks consist of 8 frames per
 * block, or a 1ms block at 8000 samples per second.  Therefore, the number of
 * blocks sent is 10.
 */
static void
ch_runblocks(struct ch *ch)
{
	int i, j;

	for (i = 0, j = 0; i < ch->interval && j < ch->other->interval; i++, j++) {
		if (i < ch->interval)
			ch_block(ch, ch->other->oq);
		if (j < ch->other->interval)
			ch_block(ch->other, ch->oq);
	}
}

/**
 * ch_data: - pass data from a channel user
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @slot: slot
 * @dp: multiplex data
 *
 * There are two approaches to sending data here to emulate a PDH or SDH
 * facility: tick and throttle.
 *
 * In the tick approach we run a tight timer that runs every tick.  When the
 * timer fires, it restarts the timer and then sends X blocks.  If X blocks
 * are not available for sending, insert repeat blocks.  This is close to
 * synchronous line behaviour.  
 *
 * In the throttle approach, bandwidth calculations are only performed when
 * one side goes to send.  When data is sent, it is added to the number of
 * bytes sent in the last interval.  If the number of bytes sent in the last
 * interval exceeds X, the data is placed back on the queue and an interval
 * timer set.  When the interval timer fires, the queue is reenabled.  This
 * has the effect of stopping when idle and is not as close to syncrhonous
 * line behaviour.
 */
static int
ch_data(struct ch *ch, queue_t *q, mblk_t *msg, int slot, mblk_t *dp)
{
	/* One more thing we need to do is to allow blocks to queue when we are waiting for a
	   connection in the TX direction.  That would be when we are in the CHS_WCON_CREQ state
	   and the requested direction is CHF_TX_DIR. */
	if ((ch_get_i_state(ch) == CHS_CONNECTED && ch->state.i_flags & CHF_TX_DIR) ||
	    (ch_get_i_state(ch) == CHS_WCON_CREQ && ch->reqflags & CHF_TX_DIR)) {
		/* In the tick approach we run a tight timer that runs every tick.  When the timer
		   fires, it restarts the timer and then sends X blocks.  If X blocks are not
		   available for sending, insert repeat blocks.  This is close to synchronous line
		   behaviour.  So, just queue the message. */
		return (-EAGAIN);
	}
	freemsg(dp);
	return (0);
}

/**
 * ch_disconnect: - disconnect a channel user
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @slot: slot to disconnect
 *
 * When the local end disconnects from a correct state, the other end must
 * also be moved to a disconnected (enabled) state.  If the other end is
 * disabled, both ends must move to the disabled state.
 */
static int
ch_disconnect(struct ch *ch, queue_t *q, mblk_t *msg, int flags, int slot)
{
	int err, oflags;

	ch_set_i_state(ch, CHS_WCON_DREQ);
	ch->reqflags = flags;
	/* Our next actions depend upon not only our state but the state of the CH at the "other
	   end" of the pipe. */
	switch (ch_get_i_state(ch->other)) {
	case CHS_UNINIT:
	case CHS_UNUSABLE:
		/* These are unusable states, disable locally. */
		return ch_disable_ind(ch, q, msg, 0);
	case CHS_WCON_RREQ:
		/* The other end is disabling, confirm the disable but move the local end to the
		   disabled state too. */
		if ((err = ch_disable_con(ch->other, q, NULL)))
			return (err);
		return ch_disable_ind(ch, q, msg, 0);
	case CHS_DETACHED:
	case CHS_ATTACHED:
		/* These are disabled states, move the local end to a disabled state too. */
		return ch_disable_ind(ch, q, msg, 0);
	case CHS_ENABLED:
		/* These are stable disconnected states, just confirm the disconnect. */
		break;
	case CHS_WCON_DREQ:
		/* The other end is disconnecting too, confirm the disconnection. */
		if ((err = ch_disconnect_con(ch->other, q, NULL, slot)))
			return (err);
		if (ch_get_i_state(ch->other) != CHS_CONNECTED)
			break;
		/* fall through */
	case CHS_WCON_CREQ:
	case CHS_CONNECTED:
		oflags = 0;
		oflags |= (flags & CHF_TX_DIR) ? CHF_RX_DIR : 0;
		oflags |= (flags & CHF_RX_DIR) ? CHF_TX_DIR : 0;
		oflags &= ch->other->state.i_flags;
		if (oflags != 0)
			if ((err = ch_disconnect_ind(ch->other, q, NULL, oflags, 0, slot)))
				return (err);
		break;
	case CHS_WCON_EREQ:
		/* This is strange, but confirm the enable and thus move both ends to the enabled
		   but disconnected state. */
		if ((err = ch_enable_con(ch->other, q, NULL)))
			return (err);
		break;
	case CHS_WACK_AREQ:
	case CHS_WACK_UREQ:
	case CHS_WACK_EREQ:
	case CHS_WACK_RREQ:
	case CHS_WACK_CREQ:
	case CHS_WACK_DREQ:
	default:
		/* These are errors, but disable locally to try to sync up. */
		mi_strlog(ch->oq, STRLOGNO, SL_ERROR | SL_TRACE, "disconnect in incorrect state");
		return ch_disable_ind(ch, q, msg, 0);
	}
	return ch_disconnect_con(ch, q, msg, slot);
}

/**
 * ch_disable: - disable a channel provider
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * When we disable from a correct state, the other end must also be in a
 * disabled state.
 */
static int
ch_disable(struct ch *ch, queue_t *q, mblk_t *msg)
{
	int err;

	ch_set_i_state(ch, CHS_WCON_RREQ);
	switch (ch_get_i_state(ch->other)) {
	case CHS_UNINIT:
	case CHS_UNUSABLE:
		/* These are unusable states, confirm the disable. */
		break;
	case CHS_DETACHED:
	case CHS_ATTACHED:
		/* These are stable, disabled states, just confirm the disable. */
		break;
	case CHS_WACK_AREQ:
	case CHS_WACK_UREQ:
	case CHS_WACK_EREQ:
	case CHS_WACK_RREQ:
	case CHS_WACK_CREQ:
	case CHS_WACK_DREQ:
	default:
		/* these are erroneous states, but confirm disable locally to try to sync up. */
		mi_strlog(ch->oq, STRLOGNO, SL_ERROR | SL_TRACE, "disconnect in incorrect state");
		break;
	case CHS_WCON_RREQ:
		/* This is a strange state, but confirm both remote and local to sync state to
		   disabled at both ends. */
		if ((err = ch_disable_con(ch->other, q, NULL)))
			return (err);
		break;
	case CHS_WCON_DREQ:
	case CHS_WCON_CREQ:
	case CHS_WCON_EREQ:
	case CHS_CONNECTED:
	case CHS_ENABLED:
		/* Push the other end into a disabled state. */
		if ((err = ch_disable_ind(ch->other, q, NULL, 0)))
			return (err);
		break;
	}
	return ch_disable_con(ch, q, msg);
}

/**
 * ch_detach: - detach a channel user
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * When we go to detach from a correct state, the other end must be in a
 * detached or attached/disabled.  We do not allow the other end to hang out
 * in an enabling state when this end detaches.
 */
static int
ch_detach(struct ch *ch, queue_t *q, mblk_t *msg)
{
	int err;

	ch_set_i_state(ch, CHS_WACK_UREQ);
	switch (ch_get_i_state(ch->other)) {
	case CHS_UNINIT:
	case CHS_UNUSABLE:
		/* These are unusable states, ack the detach. */
		break;
	case CHS_WCON_RREQ:
		if ((err = ch_disable_con(ch->other, q, NULL)))
			return (err);
		break;
	case CHS_WCON_EREQ:
	case CHS_WCON_DREQ:
	case CHS_WCON_CREQ:
	case CHS_CONNECTED:
	case CHS_ENABLED:
		/* Push the other end into a disabled state. */
		if ((err = ch_disable_ind(ch->other, q, NULL, 0)))
			return (err);
		break;
	case CHS_DETACHED:
	case CHS_ATTACHED:
		/* These are stable, disabled states, just ack the detach. */
		break;
	default:
	case CHS_WACK_AREQ:
	case CHS_WACK_UREQ:
	case CHS_WACK_EREQ:
	case CHS_WACK_RREQ:
	case CHS_WACK_CREQ:
	case CHS_WACK_DREQ:
		/* These are erroneous states, but ack detach to try to sync up. */
		mi_strlog(ch->oq, STRLOGNO, SL_ERROR | SL_TRACE, "detach in incorrect state");
		break;
	}
	return ch_ok_ack(ch, q, msg, CH_DETACH_REQ);
}

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  CH User -> CH Provider primitives.
 *
 *  =========================================================================
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	if (!MBLKIN(mp, p->ch_opt_offset, p->ch_opt_length))
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	if (ch_get_i_state(ch) != CHS_UNINIT && ch_get_i_state(ch) != CHS_DETACHED
	    && (ch_get_i_state(ch) != CHS_ATTACHED || p->ch_addr_length == 0))
		goto outstate;
	if (!MBLKIN(mp, p->ch_addr_offset, p->ch_addr_length))
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (ch_get_i_state(ch) != CHS_ATTACHED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	/* must be enabled or connected in on direction */
	if (ch_get_i_state(ch) != CHS_ENABLED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	/* the connection request must request that a direction be connected */
	if ((p->ch_conn_flags & ~(CHF_BOTH_DIR)) || ((p->ch_conn_flags & (CHF_BOTH_DIR)) == 0))
		goto badflag;
	if (p->ch_slot != 0)
		goto badaddr;
	/* if the end is already connected, the request must require that the disconnected
	   direction be now connected */
	if (ch_get_i_state(ch) == CHS_CONNECTED
	    && ((ch_get_flags(ch) ^ p->ch_conn_flags) & p->ch_conn_flags))
		goto outstate;
	/* good connection request */
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (ch_get_i_state(ch) != CHS_CONNECTED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (ch_get_i_state(ch) != CHS_CONNECTED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (ch_get_i_state(ch) != CHS_ENABLED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (ch_get_i_state(ch) != CHS_ATTACHED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
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
 *  =========================================================================
 *
 *  INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
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

	mi_strlog(ch->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", ch_iocname(ioc->ioc_cmd));

	ch_save_total_state(ch);
	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(CH_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.config), false)))
			goto enobufs;
		bcopy(&ch->ch.config, bp->b_rptr, sizeof(ch->ch.config));
		break;
	case _IOC_NR(CH_IOCSCONFIG):
		size = sizeof(ch->ch.config);
		break;
	case _IOC_NR(CH_IOCTCONFIG):
		size = sizeof(ch->ch.config);
		break;
	case _IOC_NR(CH_IOCCCONFIG):
		size = sizeof(ch->ch.config);
		break;
	case _IOC_NR(CH_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.statem), false)))
			goto enobufs;
		bcopy(&ch->ch.statem, bp->b_rptr, sizeof(ch->ch.statem));
		break;
	case _IOC_NR(CH_IOCCMRESET):
		/* FIXME: reset the state machine */
		break;
	case _IOC_NR(CH_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.statsp), false)))
			goto enobufs;
		bcopy(&ch->ch.statsp, bp->b_rptr, sizeof(ch->ch.statsp));
		break;
	case _IOC_NR(CH_IOCSSTATSP):
		size = sizeof(ch->ch.statsp);
		break;
	case _IOC_NR(CH_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.stats), false)))
			goto enobufs;
		bcopy(&ch->ch.stats, bp->b_rptr, sizeof(ch->ch.stats));
		break;
	case _IOC_NR(CH_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.stats), false)))
			goto enobufs;
		bcopy(&ch->ch.stats, bp->b_rptr, sizeof(ch->ch.stats));
		bzero(&ch->ch.stats, sizeof(ch->ch.stats));
		break;
	case _IOC_NR(CH_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(ch->ch.notify), false)))
			goto enobufs;
		bcopy(&ch->ch.notify, bp->b_rptr, sizeof(ch->ch.notify));
		break;
	case _IOC_NR(CH_IOCSNOTIFY):
		size = sizeof(ch->ch.notify);
		break;
	case _IOC_NR(CH_IOCCNOTIFY):
		size = sizeof(ch->ch.notify);
		break;
	case _IOC_NR(CH_IOCCMGMT):
		size = sizeof(struct ch_mgmt);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		ch_restore_total_state(ch);
		return (err);
	}
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

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));

	mi_strlog(ch->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", ch_iocname(cp->cp_cmd));

	ch_save_total_state(ch);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(CH_IOCSCONFIG):
		if ((err = ch_testconfig(ch, (struct ch_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &ch->ch.config, sizeof(ch->ch.config));
		break;
	case _IOC_NR(CH_IOCTCONFIG):
		err = ch_testconfig(ch, (struct ch_config *) bp->b_rptr);
		break;
	case _IOC_NR(CH_IOCCCONFIG):
		if ((err = ch_testconfig(ch, (struct ch_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &ch->ch.config, sizeof(ch->ch.config));
		break;
	case _IOC_NR(CH_IOCSSTATSP):
		bcopy(bp->b_rptr, &ch->ch.statsp, sizeof(ch->ch.statsp));
		break;
	case _IOC_NR(CH_IOCSNOTIFY):
		if ((err = ch_setnotify(ch, (struct ch_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &ch->ch.notify, sizeof(ch->ch.notify));
		break;
	case _IOC_NR(CH_IOCCNOTIFY):
		if ((err = ch_clrnotify(ch, (struct ch_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &ch->ch.notify, sizeof(ch->ch.notify));
		break;
	case _IOC_NR(CH_IOCCMGMT):
		err = ch_manage(ch, (struct ch_mgmt *) bp->b_rptr);
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		ch_restore_total_state(ch);
		return (err);
	}
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
 * This is the final step which is a simple copy done operation.
 */
static int
ch_copyout(struct ch *ch, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(ch->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", ch_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

/**
 * ch_do_ioctl: - process M_IOCTL message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static int
ch_do_ioctl(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*ioc))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case CH_IOC_MAGIC:
		return ch_ioctl(ch, q, mp);
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * ch_do_copyin: - process M_IOCDATA message
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
ch_do_copyin(struct ch *ch, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case CH_IOC_MAGIC:
		return ch_copyin(ch, q, mp, dp);
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * ch_do_copyout: - process M_IOCDATA message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final stop which is a simple copy done operation.
 */
static int
ch_do_copyout(struct ch *ch, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case CH_IOC_MAGIC:
		return ch_copyout(ch, q, mp, dp);
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/*
 *  =========================================================================
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  =========================================================================
 */

/**
 * __ch_m_data: - process M_DATA message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
__ch_m_data(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_data(ch, q, NULL, 0, mp);
}

/**
 * ch_m_data: - process M_DATA message
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
ch_m_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __ch_m_data(CH_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * ch_m_proto_slow: - process M_(PC)PROTO message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 * @prim: the primitive
 */
static noinline fastcall int
ch_m_proto_slow(struct ch *ch, queue_t *q, mblk_t *mp, ch_ulong prim)
{
	int rtn;

	switch (prim) {
	case CH_DATA_REQ:
		mi_strlog(ch->oq, STRLOGDA, SL_TRACE, "-> %s", ch_primname(prim));
		break;
	default:
		mi_strlog(ch->oq, STRLOGRX, SL_TRACE, "-> %s", ch_primname(prim));
		break;
	}

	ch_save_total_state(ch);
	switch (prim) {
	case CH_INFO_REQ:
		rtn = ch_info_req(ch, q, mp);
		break;
	case CH_OPTMGMT_REQ:
		rtn = ch_optmgmt_req(ch, q, mp);
		break;
	case CH_ATTACH_REQ:
		rtn = ch_attach_req(ch, q, mp);
		break;
	case CH_ENABLE_REQ:
		rtn = ch_enable_req(ch, q, mp);
		break;
	case CH_CONNECT_REQ:
		rtn = ch_connect_req(ch, q, mp);
		break;
	case CH_DATA_REQ:
		rtn = ch_data_req(ch, q, mp);
		break;
	case CH_DISCONNECT_REQ:
		rtn = ch_disconnect_req(ch, q, mp);
		break;
	case CH_DISABLE_REQ:
		rtn = ch_disable_req(ch, q, mp);
		break;
	case CH_DETACH_REQ:
		rtn = ch_detach_req(ch, q, mp);
		break;
	default:
		rtn = ch_other_req(ch, q, mp);
		break;
	}
	if (rtn)
		ch_restore_total_state(ch);
	return (rtn);
}

/**
 * __ch_m_proto: - process M_(PC)PROTO message
 * @ch: locked private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
__ch_m_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	int rtn;

	if (unlikely(MBLKIN(mp, 0, sizeof(prim)))) {
		ch_ulong prim = *(typeof(prim) *) mp->b_rptr;

		if (likely(prim == CH_DATA_REQ)) {
#ifndef _OPTIMIZE_SPEED
			mi_strlog(ch->oq, STRLOGDA, SL_TRACE, "-> CH_DATA_REQ");
#endif				/* _OPTIMIZE_SPEED */
			rtn = ch_data_req(ch, q, mp); /* FIXME: not right */
		} else {
			rtn = ch_m_proto_slow(ch, q, mp, prim);
		}
	} else {
		freemsg(mp);
		rtn = 0;
	}
	return (rtn);
}

/**
 * ch_m_proto: = process M_(PC)PROTO message
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
ch_m_proto(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __ch_m_proto(CH_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * __ch_m_sig: process M_(PC)SIG message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 *
 * This is the method for processing tick timers.  Under the tick approach
 * each time that the tick timer fires we reset the timer.  Then we prepare as
 * many blocks as will fit in the interval and send them to the other side.
 */
static inline fastcall int
__ch_m_sig(struct ch *ch, queue_t *q, mblk_t *mp)
{
	int rtn = 0;

	if (!mi_timer_valid(mp))
		return (0);

	/* restart the timer */
	mi_timer(q, mp, ch->interval);

	/* run blocks in both directions */
	ch_runblocks(ch);

	return (rtn);
}

/**
 * ch_m_sig: process M_(PC)SIG message
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
ch_m_sig(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __ch_m_sig(CH_PRIV(q), q, mp);
		mi_unlock(priv);
	} else 
		err = mi_timer_requeue(mp) ? -EAGAIN : 0;
	return (err);
}

/**
 * ch_m_flush: - process M_FLUSH message
 * @q: active queue
 * @mp: the M_FLUSH message
 *
 * Avoid having to push pipemod.  If we are the bottommost module over a pipe
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
	/* pipemod style flush bit reversal */
	if (!SAMESTR(q)) {
		switch (mp->b_rptr[0] & FLUSHRW) {
		case FLUSHW:
			mp->b_rptr[0] &= ~FLUSHW;
			mp->b_rptr[0] |= FLUSHR;
			break;
		case FLUSHR:
			mp->b_rptr[0] &= ~FLUSHR;
			mp->b_rptr[0] |= FLUSHW;
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

static fastcall int
__ch_m_ioctl(struct ch *ch, queue_t *q, mblk_t *mp)
{
	int err;

	err = ch_do_ioctl(ch, q, mp);
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * ch_m_ioctl: - process M_IOCTL message
 * @q: active queue
 * @mp: the M_IOCTL message
 */
static fastcall int
ch_m_ioctl(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __ch_m_ioctl(CH_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

static fastcall int
__ch_m_iocdata(struct ch *ch, queue_t *q, mblk_t *mp)
{

	mblk_t *dp;
	int err;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = ch_do_copyin(ch, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = ch_do_copyout(ch, q, mp, dp);
		break;
	default:
		err = EPROTO;
		break;
	}
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * ch_m_iocdata: - process M_IOCDATA message
 * @q: active queue
 * @mp: the M_IOCDATA message
 */
static fastcall int
ch_m_iocdata(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __ch_m_iocdata(CH_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * ch_m_rse: - process M_(PC)RSE message
 * @q: active queue
 * @mp: the M_(PC)RSE message
 */
static fastcall int
ch_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * ch_m_other: - process other STREAMS message
 * @q: active queue
 * @mp: other STREAMS message
 *
 * Simply pass unrecognized messages along.
 */
static fastcall int
ch_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * ch_msg_slow: - process STREAMS message, slow
 * @q: active queue
 * @mp: the STREAMS message
 *
 * This is the slow version of the STREAMS message handling.  It is expected
 * that data is delivered in M_DATA message blocks instead of CH_DATA_IND or
 * CH_DATA_REQ message blocks.  Nevertheless, if this slower function gets
 * called, it is more likely because we have an M_PROTO message block
 * containing an CH_DATA_REQ.
 */
static noinline fastcall int
ch_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return ch_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return ch_m_sig(q, mp);
	case M_IOCTL:
		return ch_m_ioctl(q, mp);
	case M_IOCDATA:
		return ch_m_iocdata(q, mp);
	case M_FLUSH:
		return ch_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return ch_m_rse(q, mp);
	default:
		return ch_m_other(q, mp);
	case M_DATA:
		return ch_m_data(q, mp);
	}
}

/**
 * ch_msg: - process STREAMS message
 * @q: active queue
 * @mp: the message
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
ch_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return ch_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return ch_m_proto(q, mp);
	return ch_msg_slow(q, mp);
}

/**
 * __ch_msg_slow: - process STREAMS message, slow
 * @ch: locked private structure
 * @q: active queue
 * @mp: the STREAMS message
 */
static noinline fastcall int
__ch_msg_slow(struct ch *ch, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __ch_m_proto(ch, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __ch_m_sig(ch, q, mp);
	case M_IOCTL:
		return __ch_m_ioctl(ch, q, mp);
	case M_IOCDATA:
		return __ch_m_iocdata(ch, q, mp);
	case M_FLUSH:
		return ch_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return ch_m_rse(q, mp);
	default:
		return ch_m_other(q, mp);
	case M_DATA:
		return __ch_m_data(ch, q, mp);
	}
}

/**
 * __ch_msg: - process STREAMS message locked
 * @ch: locked private structure
 * @q: active queue
 * @mp: the message
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
__ch_msg(struct ch *ch, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return __ch_m_data(ch, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return __ch_m_proto(ch, q, mp);
	return __ch_msg_slow(ch, q, mp);
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */

/**
 * ch_putp: - put procedure
 * @q: active queue
 * @mp: message to put
 *
 * Quick canonical put procedure.
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
 * @q: queue to service
 *
 * Quick canonical service procedure.  This is a  little quicker for
 * processing bulked messages because it takes the lock once for the entire
 * group of M_DATA messages, instead of once for each message.
 */
static streamscall __hot_read int
ch_srvp(queue_t *q)
{
	mblk_t *mp;

	if (likely((mp = getq(q)) != NULL)) {
		caddr_t priv;

		if (likely((priv = mi_trylock(q)) != NULL)) {
			do {
				if (unlikely(__ch_msg(CH_PRIV(q), q, mp) != 0))
					break;
			} while (likely((mp = getq(q)) != NULL));
			mi_unlock(priv);
		}
		if (unlikely(mp != NULL))
			putbq(q, mp);
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE OPEN AND CLOSE ROUTINES
 *
 *  =========================================================================
 */

static caddr_t ch_opens = NULL;

/**
 * ch_qopen: - STREAMS module queue open routine
 * @q: read queue of freshly allocated queue pair
 * @devp: device number of driver
 * @oflags: flags to open(2) call
 * @sflag: STREAMS flag
 * @crp: credentials of opening process
 */
static streamscall int
ch_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct ch_pair *p;
	mblk_t *tick;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((tick = mi_timer_alloc(0)) == NULL)
		return (ENOBUFS);
	if ((err = mi_open_comm(&ch_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		mi_timer_free(tick);
		return (err);
	}

	p = PRIV(q);
	bzero(p, sizeof(*p));

	/* initialize the structure */
	p->r_priv.pair = p;
	p->r_priv.other = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state.i_state = CHS_UNINIT;
	p->r_priv.oldstate.i_state = CHS_UNINIT;
	p->r_priv.state.i_flags = 0;
	p->r_priv.oldstate.i_flags = 0;

	p->r_priv.reqflags = 0;
	p->r_priv.tick = tick;
	p->r_priv.interval = 10;	/* milliseconds */

	p->r_priv.ch.config.type = CH_PARMS_CIRCUIT;
	p->r_priv.ch.config.encoding = CH_ENCODING_G711_PCM_U;	/* encoding */
	p->r_priv.ch.config.block_size = 64;	/* data block size (bits) */
	p->r_priv.ch.config.samples = 8;	/* samples per block */
	p->r_priv.ch.config.sample_size = 8;	/* sample size (bits) */
	p->r_priv.ch.config.rate = CH_RATE_8000;	/* channel clock reate (samples/second) */
	p->r_priv.ch.config.tx_channels = 1;	/* number of tx channels */
	p->r_priv.ch.config.rx_channels = 1;	/* number of rx channels */
	p->r_priv.ch.config.opt_flags = CH_PARM_OPT_CLRCH;	/* option flags */

	p->r_priv.ch.notify.events = 0;
	p->r_priv.ch.statem.state = CHS_UNINIT;
	p->r_priv.ch.statem.flags = 0;
	p->r_priv.ch.statsp.header = 0;
	p->r_priv.ch.stats.header = 0;

	p->w_priv.pair = p;
	p->w_priv.other = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state.i_state = CHS_UNINIT;
	p->w_priv.oldstate.i_state = CHS_UNINIT;
	p->w_priv.state.i_flags = 0;
	p->w_priv.oldstate.i_flags = 0;

	p->w_priv.reqflags = 0;
	p->w_priv.tick = tick;
	p->w_priv.interval = 10;	/* milliseconds */

	p->w_priv.ch.config.type = CH_PARMS_CIRCUIT;
	p->w_priv.ch.config.encoding = CH_ENCODING_G711_PCM_U;	/* encoding */
	p->w_priv.ch.config.block_size = 64;	/* data block size (bits) */
	p->w_priv.ch.config.samples = 8;	/* samples per block */
	p->w_priv.ch.config.sample_size = 8;	/* sample size (bits) */
	p->w_priv.ch.config.rate = CH_RATE_8000;	/* channel clock reate (samples/second) */
	p->w_priv.ch.config.tx_channels = 1;	/* number of tx channels */
	p->w_priv.ch.config.rx_channels = 1;	/* number of rx channels */
	p->w_priv.ch.config.opt_flags = CH_PARM_OPT_CLRCH;	/* option flags */

	p->w_priv.ch.notify.events = 0;
	p->w_priv.ch.statem.state = CHS_UNINIT;
	p->w_priv.ch.statem.flags = 0;
	p->w_priv.ch.statsp.header = 0;
	p->w_priv.ch.stats.header = 0;

	qprocson(q);
	return (0);
}

/**
 * ch_qclose: - module queue close procedure
 * @q: read queue of queue pair
 * @oflags: flags to open(2) call
 * @crp: credentials of closing process
 */
static streamscall int
ch_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct ch *ch = CH_PRIV(q);

	qprocsoff(q);
	mi_timer_free(ch->tick);
	mi_close_comm(&ch_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit ch_rinit = {
	.qi_putp = ch_putp,		/* Read put (message from below) */
	.qi_srvp = ch_srvp,		/* Read queue service */
	.qi_qopen = ch_qopen,		/* Each open */
	.qi_qclose = ch_qclose,		/* Last close */
	.qi_minfo = &ch_minfo,		/* Information */
	.qi_mstat = &ch_rstat,		/* Statistics */
};

static struct qinit ch_winit = {
	.qi_putp = ch_putp,		/* Write put (message from above) */
	.qi_srvp = ch_srvp,		/* Write queue service */
	.qi_minfo = &ch_minfo,		/* Information */
	.qi_mstat = &ch_wstat,		/* Statistics */
};

static struct streamtab ch_pmodinfo = {
	.st_rdinit = &ch_rinit,		/* Upper read queue */
	.st_wrinit = &ch_winit,		/* Upper write queue */
};

#ifdef LINUX
/*
 *  =========================================================================
 *
 *  LINUX INITIALIZATION
 *
 *  =========================================================================
 */

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for CH-PMOD module. (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw ch_fmod = {
	.f_name = MOD_NAME,
	.f_str = &ch_pmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/**
 * ch_pmodinit: - initialize CH-PMOD
 */
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

/**
 * ch_pmodexit: - terminate CH-PMOD
 */
static __exit void
ch_pmodexit(void)
{
	int err;

	if ((err = unregister_strmod(&ch_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
		return;
	}
	return;
}

module_init(ch_pmodinit);
module_exit(ch_pmodexit);

#endif				/* LINUX */
