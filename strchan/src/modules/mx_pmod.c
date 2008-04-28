/*****************************************************************************

 @(#) $RCSfile: mx_pmod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2008-04-28 23:39:57 $

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

 Last Modified $Date: 2008-04-28 23:39:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mx_pmod.c,v $
 Revision 0.9.2.7  2008-04-28 23:39:57  brian
 - updated headers for release

 Revision 0.9.2.6  2007/10/15 17:18:50  brian
 - fix for freezestr on 2.4 kernels

 Revision 0.9.2.5  2007/08/15 05:32:59  brian
 - GPLv3 updates

 Revision 0.9.2.4  2007/08/14 06:47:37  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/08/06 04:44:06  brian
 - rework of pipe-based emulation modules

 Revision 0.9.2.2  2007/08/03 13:35:52  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.1  2007/07/14 01:13:38  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: mx_pmod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2008-04-28 23:39:57 $"

static char const ident[] =
    "$RCSfile: mx_pmod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2008-04-28 23:39:57 $";

/*
 *  This is MX-PMOD.  This is a pushable STREAMS module that can be pushed on one end of a
 *  STREAMS-based pipe to simulate an MX driver stream.  This is a PPA Style 1 stream.  Each end of
 *  the STREAMS-based pipe will present the MXI interface.
 */

#define _LFS_SOURCE 1
#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1
#define _SUN_SOURCE 1

#include <sys/os7/compat.h>

#include <sys/mxi.h>
#include <sys/mxi_ioctl.h>

/* don't really want the SUN version of these */
//#undef freezestr
//#undef unfreezestr

#define MX_DESCRIP	"MX (Multiplex) STREAMS PIPE MODULE."
#define MX_REVISION	"OpenSS7 $RCSfile: mx_pmod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2008-04-28 23:39:57 $"
#define MX_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define MX_DEVICE	"Provides OpenSS7 MX pipe driver."
#define MX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MX_LICENSE	"GPL"
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
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
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

struct mx_pair;

struct mx {
	struct mx_pair *pair;
	struct mx *other;
	queue_t *oq;
	struct st state, oldstate;
	uint reqflags;
	mblk_t *tick;
	ulong interval;
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

#define PRIV(q)	    ((struct mx_pair *)(q)->q_ptr)
#define MX_PRIV(q)  (((q)->q_flag & QREADR) ? &PRIV(q)->r_priv : &PRIV(q)->w_priv)

#define STRLOGNO	0	/* log Stream errors */
#define STRLOGIO	1	/* log Stream input-output */
#define STRLOGST	2	/* log Stream state transitions */
#define STRLOGTO	3	/* log Stream timeouts */
#define STRLOGRX	4	/* log Stream primitives received */
#define STRLOGTX	5	/* log Stream primitives issued */
#define STRLOGTE	6	/* log Stream timer events */
#define STRLOGDA	7	/* log Stream data */

/**
 * mx_iocname: display MX ioctl command name
 * @cmd: ioctl command
 */
static const char *
mx_iocname(int cmd)
{
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(MX_IOCGCONFIG):
		return ("MX_IOCGCONFIG");
	case _IOC_NR(MX_IOCSCONFIG):
		return ("MX_IOCSCONFIG");
	case _IOC_NR(MX_IOCTCONFIG):
		return ("MX_IOCTCONFIG");
	case _IOC_NR(MX_IOCCCONFIG):
		return ("MX_IOCCCONFIG");
	case _IOC_NR(MX_IOCGSTATEM):
		return ("MX_IOCGSTATEM");
	case _IOC_NR(MX_IOCCMRESET):
		return ("MX_IOCCMRESET");
	case _IOC_NR(MX_IOCGSTATSP):
		return ("MX_IOCGSTATSP");
	case _IOC_NR(MX_IOCSSTATSP):
		return ("MX_IOCSSTATSP");
	case _IOC_NR(MX_IOCGSTATS):
		return ("MX_IOCGSTATS");
	case _IOC_NR(MX_IOCCSTATS):
		return ("MX_IOCCSTATS");
	case _IOC_NR(MX_IOCGNOTIFY):
		return ("MX_IOCGNOTIFY");
	case _IOC_NR(MX_IOCSNOTIFY):
		return ("MX_IOCSNOTIFY");
	case _IOC_NR(MX_IOCCNOTIFY):
		return ("MX_IOCCNOTIFY");
	case _IOC_NR(MX_IOCCMGMT):
		return ("MX_IOCCMGMT");
	default:
		return ("(unknown ioctl)");
	}
}

/**
 * mx_primname: display MX primitive name
 * @prim: the primitive to display
 */
static const char *
mx_primname(mx_ulong prim)
{
	switch (prim) {
	case MX_INFO_REQ:
		return ("MX_INFO_REQ");
	case MX_OPTMGMT_REQ:
		return ("MX_OPTMGMT_REQ");
	case MX_ATTACH_REQ:
		return ("MX_ATTACH_REQ");
	case MX_ENABLE_REQ:
		return ("MX_ENABLE_REQ");
	case MX_CONNECT_REQ:
		return ("MX_CONNECT_REQ");
	case MX_DATA_REQ:
		return ("MX_DATA_REQ");
	case MX_DISCONNECT_REQ:
		return ("MX_DISCONNECT_REQ");
	case MX_DISABLE_REQ:
		return ("MX_DISABLE_REQ");
	case MX_DETACH_REQ:
		return ("MX_DETACH_REQ");
	case MX_INFO_ACK:
		return ("MX_INFO_ACK");
	case MX_OPTMGMT_ACK:
		return ("MX_OPTMGMT_ACK");
	case MX_OK_ACK:
		return ("MX_OK_ACK");
	case MX_ERROR_ACK:
		return ("MX_ERROR_ACK");
	case MX_ENABLE_CON:
		return ("MX_ENABLE_CON");
	case MX_CONNECT_CON:
		return ("MX_CONNECT_CON");
	case MX_DATA_IND:
		return ("MX_DATA_IND");
	case MX_DISCONNECT_IND:
		return ("MX_DISCONNECT_IND");
	case MX_DISCONNECT_CON:
		return ("MX_DISCONNECT_CON");
	case MX_DISABLE_IND:
		return ("MX_DISABLE_IND");
	case MX_DISABLE_CON:
		return ("MX_DISABLE_CON");
	case MX_EVENT_IND:
		return ("MX_EVENT_IND");
	default:
		return ("(unknown primitive)");
	}
}

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
 * mx_get_l_state: - get management state for private structure
 * @mx: private structure
 */
static int
mx_get_l_state(struct mx *mx)
{
	return (mx->state.l_state);
}

/**
 * mx_set_l_state: - set management state for private structure
 * @mx: private structure
 * @newstate: new state
 */
static int
mx_set_l_state(struct mx *mx, int newstate)
{
	int oldstate = mx->state.l_state;

	if (newstate != oldstate) {
		mx->state.l_state = newstate;
		mi_strlog(mx->oq, STRLOGST, SL_TRACE, "%s <- %s", mx_statename(newstate),
			  mx_statename(oldstate));
	}
	return (newstate);
}

static int
mx_save_l_state(struct mx *mx)
{
	return ((mx->oldstate.l_state = mx_get_l_state(mx)));
}

static int
mx_restore_l_state(struct mx *mx)
{
	return mx_set_l_state(mx, mx->oldstate.l_state);
}

/**
 * mx_get_i_state: - get state for private structure
 * @mx: private structure
 */
static int
mx_get_i_state(struct mx *mx)
{
	return (mx->state.i_state);
}

/**
 * mx_set_i_state: - set state for private structure
 * @mx: private structure
 * @newstate: new state
 */
static int
mx_set_i_state(struct mx *mx, int newstate)
{
	int oldstate = mx->state.i_state;

	if (newstate != oldstate) {
		/* Note that if we are setting away from a connecting or disconnecting state then
		   we must also reset the direction request flags. */
		switch (oldstate) {
		case MXS_WCON_CREQ:
		case MXS_WCON_DREQ:
			mx->reqflags = 0;
			break;
		case MXS_CONNECTED:
			if (mx_get_i_state(mx->other) != MXS_CONNECTED) {
				mi_timer_stop(mx->tick);
				qenable(mx->oq);
			}
			break;
		}
		switch (newstate) {
		case MXS_CONNECTED:
			if (!mi_timer_running(mx->tick))
				mi_timer(mx->oq, mx->tick, mx->interval);
			break;
		}
		mx->state.i_state = newstate;
		mi_strlog(mx->oq, STRLOGST, SL_TRACE, "%s <- %s", mx_statename(newstate),
			  mx_statename(oldstate));
	}
	return (newstate);
}

static int
mx_save_i_state(struct mx *mx)
{
	return ((mx->oldstate.i_state = mx_get_i_state(mx)));
}

static int
mx_restore_i_state(struct mx *mx)
{
	return mx_set_i_state(mx, mx->oldstate.i_state);
}

/**
 * mx_save_state: - reset state for private structure
 * @mx: private structure
 */
static void
mx_save_state(struct mx *mx)
{
	mx_save_l_state(mx);
	mx_save_i_state(mx);
}

/**
 * mx_restore_state: - reset state for private structure
 * @mx: private structure
 */
static int
mx_restore_state(struct mx *mx)
{
	mx_restore_l_state(mx);
	return mx_restore_i_state(mx);
}

/**
 * mx_get_flags: - get flags for private structure
 * @mx: private structure
 */
static inline int
mx_get_flags(struct mx *mx)
{
	return mx->state.i_flags;
}

/**
 * mx_set_flags: - set flags for private structure
 * @mx: private structure
 * @newflags: new flags
 */
static int
mx_set_flags(struct mx *mx, int newflags)
{
	int oldflags = mx->state.i_flags;

	if (newflags != oldflags) {
		mx->state.i_flags = newflags;
	}
	return (newflags);
}

static int
mx_or_flags(struct mx *mx, int orflags)
{
	return (mx->state.i_flags |= orflags);
}

static int
mx_nand_flags(struct mx *mx, int nandflags)
{
	return (mx->state.i_flags &= ~nandflags);
}

/**
 * mx_save_flags: - reset flags for private structure
 * @mx: private structure
 */
static void
mx_save_flags(struct mx *mx)
{
	mx->oldstate.i_flags = mx_get_flags(mx);
}

/**
 * mx_restore_flags: - reset flags for private structure
 * @mx: private structure
 */
static int
mx_restore_flags(struct mx *mx)
{
	return mx_set_flags(mx, mx->oldstate.i_flags);
}

static void
mx_save_total_state(struct mx *mx)
{
	mx_save_flags(mx->other);
	mx_save_state(mx->other);
	mx_save_flags(mx);
	mx_save_state(mx);
}

static int
mx_restore_total_state(struct mx *mx)
{
	mx_restore_flags(mx->other);
	mx_restore_state(mx->other);
	mx_restore_flags(mx);
	return mx_restore_state(mx);
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  MX Provider -> MX User primitives.
 *
 *  =========================================================================
 */
#if 0
/**
 * m_error: - issue M_ERROR for stream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: error to indicate
 */
static int
m_error(struct mx *mx, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(mx->oq, 0, SL_ERROR, "<- M_ERROR %d", err);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

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
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		p->mx_addr_length = 0;
		p->mx_addr_offset = sizeof(*p);
		p->mx_parm_length = sizeof(*c);
		p->mx_parm_offset = sizeof(*p);
		p->mx_prov_flags = 0;
		p->mx_prov_class = MX_CIRCUIT;
		p->mx_style = MX_STYLE1;
		p->mx_version = MX_VERSION;
		p->mx_state = mx_get_i_state(mx);
		mp->b_wptr += sizeof(*p);
		c = (typeof(c)) mp->b_wptr;
		c->mp_type = mx->mx.config.type;
		c->mp_encoding = mx->mx.config.encoding;
		c->mp_block_size = mx->mx.config.block_size;
		c->mp_samples = mx->mx.config.samples;
		c->mp_sample_size = mx->mx.config.sample_size;
		c->mp_rate = mx->mx.config.rate;
		c->mp_tx_channels = mx->mx.config.tx_channels;
		c->mp_rx_channels = mx->mx.config.rx_channels;
		c->mp_opt_flags = mx->mx.config.opt_flags;
		mp->b_wptr += sizeof(*c);
		freemsg(msg);
		mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_INFO_ACK");
		putnext(mx->oq, mp);
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
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OPTMGMT_ACK;
		p->mx_opt_length = opt_len;	/* FIXME */
		p->mx_opt_offset = sizeof(*p);	/* FIXME */
		p->mx_mgmt_flags = 0;	/* FIXME */
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_OPTMGMT_ACK");
		putnext(mx->oq, mp);
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
 *
 * There are only ever two primitives that are acknowledged with MX_OK_ACK and
 * those are MX_ATTACH_REQ and MX_DETACH_REQ.
 */
static int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *msg, int prim)
{
	struct MX_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = prim;
		switch (prim) {
		case MX_ATTACH_REQ:
			p->mx_state = mx_set_i_state(mx, MXS_ATTACHED);
			break;
		case MX_DETACH_REQ:
			p->mx_state = mx_set_i_state(mx, MXS_DETACHED);
			break;
		default:
			p->mx_state = mx_get_i_state(mx);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_OK_ACK");
		putnext(mx->oq, mp);
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
 *
 * Only called by mx_error_reply(), but then, it can be invoked by the
 * handling procedure for just about any MX-primitive passed to either side of
 * the pipe module.  State is restored by falling back to the last checkpoint
 * state.
 */
static int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *msg, int prim, int error)
{
	struct MX_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ERROR_ACK;
		p->mx_error_primitive = prim;
		p->mx_error_type = error < 0 ? MXSYSERR : error;
		p->mx_unix_error = error < 0 ? -error : 0;
		p->mx_state = mx_restore_total_state(mx);
		mp->b_wptr += sizeof(*p);
		mx_restore_flags(mx);
		freemsg(msg);
		mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_ERROR_ACK");
		putnext(mx->oq, mp);
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
 * return codes are filtered and returned directly.  mx_error_reply() and
 * an mx_error_ack() can be invoked by any MX-primitive being issued to one or
 * the other side of the pipe module.
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

	if (likely(canputnext(mx->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_ENABLE_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mx_set_i_state(mx, MXS_ENABLED);
			mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_ENABLE_CON");
			putnext(mx->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	if (likely(canputnext(mx->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISABLE_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mx_set_i_state(mx, MXS_ATTACHED);
			mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_DISABLE_CON");
			putnext(mx->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mx_connect_con: - issue MX_CONNECT_CON primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @slot: slot connected
 */
static int
mx_connect_con(struct mx *mx, queue_t *q, mblk_t *msg, int slot)
{
	struct MX_connect_con *p;
	mblk_t *mp;

	if (likely(canputnext(mx->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_CONNECT_CON;
			p->mx_conn_flags = mx->reqflags;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mx_or_flags(mx, mx->reqflags);
			mx_set_i_state(mx, MXS_CONNECTED);
			mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_CONNECT_CON");
			putnext(mx->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	(void) mx_data_ind;
	if (likely(canputnext(mx->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DATA_IND;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_DATA_IND");
			putnext(mx->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	if (likely(canputnext(mx->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISCONNECT_IND;
			p->mx_conn_flags = flags;
			p->mx_cause = cause;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mx_nand_flags(mx, flags);
			if ((mx->state.i_flags & MXF_BOTH_DIR) == 0)
				mx_set_i_state(mx, MXS_ENABLED);
			else
				mx_set_i_state(mx, MXS_CONNECTED);
			mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_DISCONNECT_IND");
			putnext(mx->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * mx_disconnect_con: - issue MX_DISCONNECT_CON primitive
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @slot: slot disconnected
 */
static int
mx_disconnect_con(struct mx *mx, queue_t *q, mblk_t *msg, int slot)
{
	struct MX_disconnect_con *p;
	mblk_t *mp;

	if (likely(canputnext(mx->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISCONNECT_CON;
			p->mx_conn_flags = mx->reqflags;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mx_nand_flags(mx, mx->reqflags);
			if ((mx_get_flags(mx) & MXF_BOTH_DIR) == 0)
				mx_set_i_state(mx, MXS_ENABLED);
			else
				mx_set_i_state(mx, MXS_CONNECTED);
			mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_DISCONNECT_CON");
			putnext(mx->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	if (likely(canputnext(mx->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISABLE_IND;
			p->mx_cause = cause;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mx_set_i_state(mx, MXS_ATTACHED);
			mx_nand_flags(mx, MXF_BOTH_DIR);
			mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_DISABLE_IND");
			putnext(mx->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
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

	if (likely(canputnext(mx->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_EVENT_IND;
			p->mx_event = event;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "<- MX_EVENT_IND");
			putnext(mx->oq, mp);
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

/**
 * mx_attach: - attach a multiplex user
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
mx_attach(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	mx_set_i_state(mx, MXS_WACK_AREQ);
	switch (mx_get_i_state(mx->other)) {
	case MXS_UNUSABLE:
		/* These are unsuable states, but we are still allowed to attach locally. */
		break;
	case MXS_UNINIT:
	case MXS_DETACHED:
	case MXS_ATTACHED:
	case MXS_WCON_EREQ:
		/* These are ok states, just ack local attach. */
		break;
	case MXS_WCON_RREQ:
		/* This is a disabling state, just confirm it. */
		if ((err = mx_disable_con(mx->other, q, NULL)))
			return (err);
		break;
	case MXS_WCON_CREQ:
	case MXS_WCON_DREQ:
	case MXS_ENABLED:
	case MXS_CONNECTED:
		/* These are enabled and connected states, indicate a disable to move both ends to
		   the disabled state. */
		if ((err = mx_disable_ind(mx->other, q, NULL, 0)))
			return (err);
		break;
	case MXS_WACK_AREQ:
	case MXS_WACK_UREQ:
	case MXS_WACK_EREQ:
	case MXS_WACK_RREQ:
	case MXS_WACK_CREQ:
	case MXS_WACK_DREQ:
	default:
		/* These are erroneous states, but still ack locally. */
		mi_strlog(mx->oq, STRLOGNO, SL_ERROR | SL_TRACE, "attach in incorrect state");
		break;
	}
	return mx_ok_ack(mx, q, msg, MX_ATTACH_REQ);
}

/**
 * mx_enable: - enable a multiplex provider
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
mx_enable(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err, oflags;

	mx_set_i_state(mx, MXS_WCON_EREQ);
	switch (mx_get_i_state(mx->other)) {
	case MXS_UNINIT:
	case MXS_UNUSABLE:
		/* These are unusable states, refuse the enable. */
		return mx_disable_ind(mx, q, msg, 0);
	case MXS_DETACHED:
	case MXS_ATTACHED:
		/* In any of these stable states we need to wait until the other end confirms the
		   enable. */
		freemsg(msg);
		return (0);
	case MXS_WCON_EREQ:
		/* The other end is waiting for enable confirmation. */
		if ((err = mx_enable_con(mx->other, q, NULL)))
			return (err);
		break;
	case MXS_WCON_RREQ:
		/* The other end is waiting to confirm disable, complete disable and refuse enable
		   locally to move both ends to the disabled state. */
		if ((err = mx_disable_con(mx->other, q, NULL)))
			return (err);
		return mx_disable_ind(mx, q, msg, 0);
	case MXS_WCON_DREQ:
		/* The other end is waiting to disconnect, confirm it to move both ends to the
		   enabled state. */
		if ((err = mx_disconnect_con(mx->other, q, NULL, 0)))
			return (err);
		/* If the confirm does not result in a full disconnect, continue to disconnect. */
		if (mx_get_i_state(mx) != MXS_CONNECTED)
			break;
		/* fall through */
	case MXS_WCON_CREQ:
	case MXS_CONNECTED:
		/* The other end is waiting to connect or connected, disconnect it and confrim
		   locally to move both ends the enabled state. */
		oflags = mx->other->state.i_flags & MXF_BOTH_DIR;
		if ((err = mx_disconnect_ind(mx->other, q, NULL, oflags, 0, 0)))
			return (err);
		break;
	case MXS_ENABLED:
		/* The other end is enabled, just confirm. */
		break;
	case MXS_WACK_AREQ:
	case MXS_WACK_UREQ:
	case MXS_WACK_EREQ:
	case MXS_WACK_RREQ:
	case MXS_WACK_CREQ:
	case MXS_WACK_DREQ:
	default:
		/* These are erroneous states. */
		mi_strlog(mx->oq, STRLOGNO, SL_ERROR | SL_TRACE, "enable in incorrect state");
		return mx_disable_ind(mx, q, msg, 0);
	}
	return mx_enable_con(mx, q, msg);
}

/**
 * mx_connect: - connect a multiplex user
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: direction flags
 * @slot: slot to connect
 */
static int
mx_connect(struct mx *mx, queue_t *q, mblk_t *msg, int flags, int slot)
{
	int err, oflags;

	mx_set_i_state(mx, MXS_WCON_CREQ);
	mx->reqflags = flags;
	switch (mx_get_i_state(mx->other)) {
	case MXS_WCON_EREQ:
		/* The other end is enabling, confirm the enable and wait for connection. */
		return mx_enable_con(mx->other, q, msg);
	case MXS_WCON_RREQ:
		/* The other end is disabling, confirm the disable and refuse the connection. */
		if ((err = mx_disable_con(mx->other, q, NULL)))
			return (err);
		return mx_disconnect_ind(mx, q, msg, MXF_BOTH_DIR, 0, slot);
	case MXS_WCON_DREQ:
		/* The other end is disconnecting, confirm the disconnect and wait for connection. */
		return mx_disconnect_con(mx->other, q, msg, slot);
	case MXS_DETACHED:
	case MXS_ATTACHED:
		/* The other end is disabled, disable this end too. */
		return mx_disable_ind(mx, q, msg, 0);
	case MXS_CONNECTED:
	case MXS_ENABLED:
		/* The other end is enabled, or it is part connected but not for the direction that 
		   we want to connect, just wait for other end to connect. */
		freemsg(msg);
		return (0);
	case MXS_WCON_CREQ:
		/* is the local side connecting for the same directions for which the remote side
		   is waiting? */
		oflags = 0;
		oflags |= ((flags | mx->state.i_flags) & MXF_TX_DIR) ? MXF_RX_DIR : 0;
		oflags |= ((flags | mx->state.i_flags) & MXF_RX_DIR) ? MXF_TX_DIR : 0;
		if ((mx->other->reqflags & oflags) == mx->other->reqflags) {
			if ((err = mx_connect_con(mx->other, q, NULL, slot)))
				return (err);
			/* Next question is whether the local connection request is now satisfied. */
			oflags = 0;
			oflags |= (flags & MXF_TX_DIR) ? MXF_RX_DIR : 0;
			oflags |= (flags & MXF_RX_DIR) ? MXF_TX_DIR : 0;
			if ((oflags & mx->other->state.i_flags) == oflags)
				return mx_connect_con(mx, q, msg, slot);

		}
		/* otherwise, then the local side cannot proceed either. */
		freemsg(msg);
		return (0);
	case MXS_UNINIT:
	case MXS_UNUSABLE:
		/* These are unusable states, disable local interface. */
		return mx_disable_ind(mx, q, msg, 0);
	case MXS_WACK_AREQ:
	case MXS_WACK_UREQ:
	case MXS_WACK_EREQ:
	case MXS_WACK_RREQ:
	case MXS_WACK_CREQ:
	case MXS_WACK_DREQ:
	default:
		/* These are erroneous states, disable local interface. */
		mi_strlog(mx->oq, STRLOGNO, SL_ERROR | SL_TRACE, "connect in incorrect state");
		return mx_disable_ind(mx, q, msg, 0);
	}
}

/**
 * mx_block: - transmit a data block
 * @mx: (locked) private structure
 * @q: queue beyond which to pass message blocks
 */
static void
mx_block(struct mx *mx, queue_t *q)
{
	mblk_t *mp;

	/* find the first M_DATA block on the queue, or M_PPROTO message block containing an
	   MX_DATA_REQ primitive. */
	freezestr(q);
	for (mp = q->q_first; mp && DB_TYPE(mp) != M_DATA
	     && (DB_TYPE(mp) != M_PROTO || *(mx_ulong *) mp->b_rptr != MX_DATA_REQ);
	     mp = mp->b_next) ;
	rmvq(q, mp);
	unfreezestr(q);
	if (mp) {
		if (canputnext(q)) {
			/* If it is an MX_DATA_REQ primitive, alter it to an MX_DATA_IND primitive. 
			 */
			if (DB_TYPE(mp) == M_PROTO)
				*(mx_ulong *) mp->b_rptr = MX_DATA_IND;
			putnext(q, mp);
		} else {
			freemsg(mp);
			mx->mx.stats.rx_buffer_overflows++;
			/* receive overrun condition */
		}
	} else {
		/* transmit underrun condition */
		mx->mx.stats.tx_buffer_overflows++;
		if (canputnext(q)) {
			uint blksize = mx->mx.config.block_size >> 3;

			/* prepare blank block */
			if ((mp = allocb(blksize, BPRI_MED))) {
				memset(mp->b_wptr, 0xfe, blksize);
				putnext(q, mp);
			} else {
				/* can't even send a blank, gonna be a really big slip. */
				mx->mx.stats.tx_underruns++;
			}
		} else {
			mx->mx.stats.rx_buffer_overflows++;
			/* receive overrun condition too */
		}
	}
}

/**
 * mx_runblocks: - run blocks off of tick timer
 * @mx: pair of queues to run
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
mx_runblocks(struct mx *mx)
{
	int i, j;

	for (i = 0, j = 0; i < mx->interval && j < mx->other->interval; i++, j++) {
		if (i < mx->interval)
			mx_block(mx, mx->other->oq);
		if (j < mx->other->interval)
			mx_block(mx->other, mx->oq);
	}
}

/**
 * mx_data: - pass data from a multiplex user
 * @mx: private structure
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
mx_data(struct mx *mx, queue_t *q, mblk_t *msg, int slot, mblk_t *dp)
{
	/* One more thing we need to do is to allow blocks to queue when we are waiting for a
	   connection in the TX direction.  That would be when we are in the MXS_WCON_CREQ state
	   and the requested direction is MXF_TX_DIR. */
	if ((mx_get_i_state(mx) == MXS_CONNECTED && mx->state.i_flags & MXF_TX_DIR) ||
	    (mx_get_i_state(mx) == MXS_WCON_CREQ && mx->reqflags & MXF_TX_DIR)) {
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
 * mx_disconnect: - disconnect a multiplex user
 * @mx: private structure
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
mx_disconnect(struct mx *mx, queue_t *q, mblk_t *msg, int flags, int slot)
{
	int err, oflags;

	mx_set_i_state(mx, MXS_WCON_DREQ);
	mx->reqflags = flags;
	/* Our next actions depend upon not only our state but the state of the MX at the "other
	   end" of the pipe. */
	switch (mx_get_i_state(mx->other)) {
	case MXS_UNINIT:
	case MXS_UNUSABLE:
		/* These are unusable states, disable locally. */
		return mx_disable_ind(mx, q, msg, 0);
	case MXS_WCON_RREQ:
		/* The other end is disabling, confirm the disable but move the local end to the
		   disabled state too. */
		if ((err = mx_disable_con(mx->other, q, NULL)))
			return (err);
		return mx_disable_ind(mx, q, msg, 0);
	case MXS_DETACHED:
	case MXS_ATTACHED:
		/* These are disabled states, move the local end to a disabled state too. */
		return mx_disable_ind(mx, q, msg, 0);
	case MXS_ENABLED:
		/* These are stable disconnected states, just confirm the disconnect. */
		break;
	case MXS_WCON_DREQ:
		/* The other end is disconnecting too, confirm the disconnection. */
		if ((err = mx_disconnect_con(mx->other, q, NULL, slot)))
			return (err);
		if (mx_get_i_state(mx->other) != MXS_CONNECTED)
			break;
		/* fall through */
	case MXS_WCON_CREQ:
	case MXS_CONNECTED:
		oflags = 0;
		oflags |= (flags & MXF_TX_DIR) ? MXF_RX_DIR : 0;
		oflags |= (flags & MXF_RX_DIR) ? MXF_TX_DIR : 0;
		oflags &= mx->other->state.i_flags;
		if (oflags != 0)
			if ((err = mx_disconnect_ind(mx->other, q, NULL, oflags, 0, slot)))
				return (err);
		break;
	case MXS_WCON_EREQ:
		/* This is strange, but confirm the enable and thus move both ends to the enabled
		   but disconnected state. */
		if ((err = mx_enable_con(mx->other, q, NULL)))
			return (err);
		break;
	case MXS_WACK_AREQ:
	case MXS_WACK_UREQ:
	case MXS_WACK_EREQ:
	case MXS_WACK_RREQ:
	case MXS_WACK_CREQ:
	case MXS_WACK_DREQ:
	default:
		/* These are errors, but disable locally to try to sync up. */
		mi_strlog(mx->oq, STRLOGNO, SL_ERROR | SL_TRACE, "disconnect in incorrect state");
		return mx_disable_ind(mx, q, msg, 0);
	}
	return mx_disconnect_con(mx, q, msg, slot);
}

/**
 * mx_disable: - disable a multiplex provider
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * When we disable from a correct state, the other end must also be in a
 * disabled state.
 */
static int
mx_disable(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	mx_set_i_state(mx, MXS_WCON_RREQ);
	switch (mx_get_i_state(mx->other)) {
	case MXS_UNINIT:
	case MXS_UNUSABLE:
		/* These are unusable states, confirm the disable. */
		break;
	case MXS_DETACHED:
	case MXS_ATTACHED:
		/* These are stable, disabled states, just confirm the disable. */
		break;
	case MXS_WACK_AREQ:
	case MXS_WACK_UREQ:
	case MXS_WACK_EREQ:
	case MXS_WACK_RREQ:
	case MXS_WACK_CREQ:
	case MXS_WACK_DREQ:
	default:
		/* these are erroneous states, but confirm disable locally to try to sync up. */
		mi_strlog(mx->oq, STRLOGNO, SL_ERROR | SL_TRACE, "disconnect in incorrect state");
		break;
	case MXS_WCON_RREQ:
		/* This is a strange state, but confirm both remote and local to sync state to
		   disabled at both ends. */
		if ((err = mx_disable_con(mx->other, q, NULL)))
			return (err);
		break;
	case MXS_WCON_DREQ:
	case MXS_WCON_CREQ:
	case MXS_WCON_EREQ:
	case MXS_CONNECTED:
	case MXS_ENABLED:
		/* Push the other end into a disabled state. */
		if ((err = mx_disable_ind(mx->other, q, NULL, 0)))
			return (err);
		break;
	}
	return mx_disable_con(mx, q, msg);
}

/**
 * mx_detach: - detach a multiplex user
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * When we go to detach from a correct state, the other end must be in a
 * detached or attached/disabled.  We do not allow the other end to hang out
 * in an enabling state when this end detaches.
 */
static int
mx_detach(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	mx_set_i_state(mx, MXS_WACK_UREQ);
	switch (mx_get_i_state(mx->other)) {
	case MXS_UNINIT:
	case MXS_UNUSABLE:
		/* These are unusable states, ack the detach. */
		break;
	case MXS_WCON_RREQ:
		if ((err = mx_disable_con(mx->other, q, NULL)))
			return (err);
		break;
	case MXS_WCON_EREQ:
	case MXS_WCON_DREQ:
	case MXS_WCON_CREQ:
	case MXS_CONNECTED:
	case MXS_ENABLED:
		/* Push the other end into a disabled state. */
		if ((err = mx_disable_ind(mx->other, q, NULL, 0)))
			return (err);
		break;
	case MXS_DETACHED:
	case MXS_ATTACHED:
		/* These are stable, disabled states, just ack the detach. */
		break;
	default:
	case MXS_WACK_AREQ:
	case MXS_WACK_UREQ:
	case MXS_WACK_EREQ:
	case MXS_WACK_RREQ:
	case MXS_WACK_CREQ:
	case MXS_WACK_DREQ:
		/* These are erroneous states, but ack detach to try to sync up. */
		mi_strlog(mx->oq, STRLOGNO, SL_ERROR | SL_TRACE, "detach in incorrect state");
		break;
	}
	return mx_ok_ack(mx, q, msg, MX_DETACH_REQ);
}

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  MX User -> MX Provider primitives.
 *
 *  =========================================================================
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	if (!MBLKIN(mp, p->mx_opt_offset, p->mx_opt_length))
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	if (mx_get_i_state(mx) != MXS_UNINIT && mx_get_i_state(mx) != MXS_DETACHED
	    && (mx_get_i_state(mx) != MXS_ATTACHED || p->mx_addr_length == 0))
		goto outstate;
	if (!MBLKIN(mp, p->mx_addr_offset, p->mx_addr_length))
		goto badaddr;
	/* Note that we do not need to support MX_ATTACH_REQ because this is a style1 driver only,
	   but if the address is null, simply move to the appropriate state. */
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_i_state(mx) != MXS_ATTACHED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	/* must be enabled or connected in on direction */
	if (mx_get_i_state(mx) != MXS_ENABLED && mx_get_i_state(mx) != MXS_CONNECTED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	/* the connection request must request that a direction be connected */
	if ((p->mx_conn_flags & ~(MXF_BOTH_DIR)) || ((p->mx_conn_flags & (MXF_BOTH_DIR)) == 0))
		goto badflag;
	if (p->mx_slot != 0)
		goto badaddr;
	/* if the end is already connected, the request must require that the disconnected
	   direction be now connected */
	if (mx_get_i_state(mx) == MXS_CONNECTED
	    && ((mx_get_flags(mx) ^ p->mx_conn_flags) & p->mx_conn_flags))
		goto outstate;
	/* good connection request */
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_i_state(mx) != MXS_CONNECTED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_i_state(mx) != MXS_CONNECTED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_i_state(mx) != MXS_ENABLED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_i_state(mx) != MXS_ATTACHED)
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
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
 *  =========================================================================
 *
 *  INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
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

	mi_strlog(mx->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", mx_iocname(ioc->ioc_cmd));

	mx_save_total_state(mx);
	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(MX_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.config), false)))
			goto enobufs;
		bcopy(&mx->mx.config, bp->b_rptr, sizeof(mx->mx.config));
		break;
	case _IOC_NR(MX_IOCSCONFIG):
		size = sizeof(mx->mx.config);
		break;
	case _IOC_NR(MX_IOCTCONFIG):
		size = sizeof(mx->mx.config);
		break;
	case _IOC_NR(MX_IOCCCONFIG):
		size = sizeof(mx->mx.config);
		break;
	case _IOC_NR(MX_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.statem), false)))
			goto enobufs;
		bcopy(&mx->mx.statem, bp->b_rptr, sizeof(mx->mx.statem));
		break;
	case _IOC_NR(MX_IOCCMRESET):
		/* FIXME: reset the state machine */
		break;
	case _IOC_NR(MX_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.statsp), false)))
			goto enobufs;
		bcopy(&mx->mx.statsp, bp->b_rptr, sizeof(mx->mx.statsp));
		break;
	case _IOC_NR(MX_IOCSSTATSP):
		size = sizeof(mx->mx.statsp);
		break;
	case _IOC_NR(MX_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.stats), false)))
			goto enobufs;
		bcopy(&mx->mx.stats, bp->b_rptr, sizeof(mx->mx.stats));
		break;
	case _IOC_NR(MX_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.stats), false)))
			goto enobufs;
		bcopy(&mx->mx.stats, bp->b_rptr, sizeof(mx->mx.stats));
		bzero(&mx->mx.stats, sizeof(mx->mx.stats));
		break;
	case _IOC_NR(MX_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(mx->mx.notify), false)))
			goto enobufs;
		bcopy(&mx->mx.notify, bp->b_rptr, sizeof(mx->mx.notify));
		break;
	case _IOC_NR(MX_IOCSNOTIFY):
		size = sizeof(mx->mx.notify);
		break;
	case _IOC_NR(MX_IOCCNOTIFY):
		size = sizeof(mx->mx.notify);
		break;
	case _IOC_NR(MX_IOCCMGMT):
		size = sizeof(struct mx_mgmt);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		mx_restore_total_state(mx);
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

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));

	mi_strlog(mx->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", mx_iocname(cp->cp_cmd));

	mx_save_total_state(mx);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MX_IOCSCONFIG):
		if ((err = mx_testconfig(mx, (struct mx_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &mx->mx.config, sizeof(mx->mx.config));
		break;
	case _IOC_NR(MX_IOCTCONFIG):
		err = mx_testconfig(mx, (struct mx_config *) bp->b_rptr);
		break;
	case _IOC_NR(MX_IOCCCONFIG):
		if ((err = mx_testconfig(mx, (struct mx_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &mx->mx.config, sizeof(mx->mx.config));
		break;
	case _IOC_NR(MX_IOCSSTATSP):
		bcopy(bp->b_rptr, &mx->mx.statsp, sizeof(mx->mx.statsp));
		break;
	case _IOC_NR(MX_IOCSNOTIFY):
		if ((err = mx_setnotify(mx, (struct mx_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &mx->mx.notify, sizeof(mx->mx.notify));
		break;
	case _IOC_NR(MX_IOCCNOTIFY):
		if ((err = mx_clrnotify(mx, (struct mx_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &mx->mx.notify, sizeof(mx->mx.notify));
		break;
	case _IOC_NR(MX_IOCCMGMT):
		err = mx_manage(mx, (struct mx_mgmt *) bp->b_rptr);
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		mx_restore_total_state(mx);
		return (err);
	}
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
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(mx->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", mx_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

/**
 * mx_do_ioctl: - process M_IOCTL message
 * @mx: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static int
mx_do_ioctl(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*ioc))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case MX_IOC_MAGIC:
		return mx_ioctl(mx, q, mp);
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * mx_do_copyin: - process M_IOCDATA message
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
mx_do_copyin(struct mx *mx, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case MX_IOC_MAGIC:
		return mx_copyin(mx, q, mp, dp);
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * mx_do_copyout: - process M_IOCDATA message
 * @mx: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final stop which is a simple copy done operation.
 */
static int
mx_do_copyout(struct mx *mx, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case MX_IOC_MAGIC:
		return mx_copyout(mx, q, mp, dp);
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
 * __mx_m_data: - process M_DATA message
 * @mx: (locked) private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
__mx_m_data(struct mx *mx, queue_t *q, mblk_t *mp)
{
	return mx_data(mx, q, NULL, 0, mp);
}

/**
 * mx_m_data: - process M_DATA message
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
mx_m_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __mx_m_data(MX_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * mx_m_proto_slow: - process M_(PC)PROTO message
 * @mx: (locked) private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 * @prim: the primitive
 */
static noinline fastcall int
mx_m_proto_slow(struct mx *mx, queue_t *q, mblk_t *mp, mx_ulong prim)
{
	int rtn;

	switch (prim) {
	case MX_DATA_REQ:
		mi_strlog(mx->oq, STRLOGDA, SL_TRACE, "-> %s", mx_primname(prim));
		break;
	default:
		mi_strlog(mx->oq, STRLOGRX, SL_TRACE, "-> %s", mx_primname(prim));
		break;
	}

	mx_save_total_state(mx);
	switch (prim) {
	case MX_INFO_REQ:
		rtn = mx_info_req(mx, q, mp);
		break;
	case MX_OPTMGMT_REQ:
		rtn = mx_optmgmt_req(mx, q, mp);
		break;
	case MX_ATTACH_REQ:
		rtn = mx_attach_req(mx, q, mp);
		break;
	case MX_ENABLE_REQ:
		rtn = mx_enable_req(mx, q, mp);
		break;
	case MX_CONNECT_REQ:
		rtn = mx_connect_req(mx, q, mp);
		break;
	case MX_DATA_REQ:
		rtn = mx_data_req(mx, q, mp);
		break;
	case MX_DISCONNECT_REQ:
		rtn = mx_disconnect_req(mx, q, mp);
		break;
	case MX_DISABLE_REQ:
		rtn = mx_disable_req(mx, q, mp);
		break;
	case MX_DETACH_REQ:
		rtn = mx_detach_req(mx, q, mp);
		break;
	default:
		rtn = mx_other_req(mx, q, mp);
		break;
	}
	if (rtn)
		mx_restore_total_state(mx);
	return (rtn);
}

/**
 * __mx_m_proto: - process M_(PC)PROTO message
 * @mx: locked private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
__mx_m_proto(struct mx *mx, queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	int rtn;

	if (unlikely(MBLKIN(mp, 0, sizeof(prim)))) {
		mx_ulong prim = *(typeof(prim) *) mp->b_rptr;

		if (likely(prim == MX_DATA_REQ)) {
#ifndef _OPTIMIZE_SPEED
			mi_strlog(mx->oq, STRLOGDA, SL_TRACE, "-> MX_DATA_REQ");
#endif				/* _OPTIMIZE_SPEED */
			rtn = mx_data_req(mx, q, mp);	/* FIXME: not right */
		} else {
			rtn = mx_m_proto_slow(mx, q, mp, prim);
		}
	} else {
		freemsg(mp);
		rtn = 0;
	}
	return (rtn);
}

/**
 * mx_m_proto: = process M_(PC)PROTO message
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
mx_m_proto(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __mx_m_proto(MX_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * __mx_m_sig: process M_(PC)SIG message
 * @mx: (locked) private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 *
 * This is the method for processing tick timers.  Under the tick approach
 * each time that the tick timer fires we reset the timer.  Then we prepare as
 * many blocks as will fit in the interval and send them to the other side.
 */
static inline fastcall int
__mx_m_sig(struct mx *mx, queue_t *q, mblk_t *mp)
{
	int rtn = 0;

	if (!mi_timer_valid(mp))
		return (0);

	/* restart the timer */
	mi_timer(q, mp, mx->interval);

	/* run blocks in both directions */
	mx_runblocks(mx);

	return (rtn);
}

/**
 * mx_m_sig: process M_(PC)SIG message
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
mx_m_sig(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __mx_m_sig(MX_PRIV(q), q, mp);
		mi_unlock(priv);
	} else
		err = mi_timer_requeue(mp) ? -EAGAIN : 0;
	return (err);
}

/**
 * mx_m_flush: - process M_FLUSH message
 * @q: active queue
 * @mp: the M_FLUSH message
 *
 * Avoid having to push pipemod.  It we are the bottommost module over a pipe
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
__mx_m_ioctl(struct mx *mx, queue_t *q, mblk_t *mp)
{
	int err;

	err = mx_do_ioctl(mx, q, mp);
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * mx_m_ioctl: - process M_IOCTL message
 * @q: active queue
 * @mp: the M_IOCTL message
 */
static fastcall int
mx_m_ioctl(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __mx_m_ioctl(MX_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

static fastcall int
__mx_m_iocdata(struct mx *mx, queue_t *q, mblk_t *mp)
{

	mblk_t *dp;
	int err;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = mx_do_copyin(mx, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = mx_do_copyout(mx, q, mp, dp);
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
 * mx_m_iocdata: - process M_IOCDATA message
 * @q: active queue
 * @mp: the M_IOCDATA message
 */
static fastcall int
mx_m_iocdata(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __mx_m_iocdata(MX_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * mx_m_rse: - process M_(PC)RSE message
 * @q: active queue
 * @mp: the M_(PC)RSE message
 */
static fastcall int
mx_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * mx_m_other: - process other STREAMS message
 * @q: active queue
 * @mp: other STREAMS message
 *
 * Simply pass unrecognized messages along.
 */
static fastcall int
mx_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * mx_msg_slow: - process STREAMS message, slow
 * @q: active queue
 * @mp: the STREAMS message
 *
 * This is the slow version of the STREAMS message handling.  It is expected
 * that data is delivered in M_DATA message blocks instead of MX_DATA_IND or
 * MX_DATA_REQ message blocks.  Nevertheless, if this slower function gets
 * called, it is more likely because we have an M_PROTO message block
 * containing an MX_DATA_REQ.
 */
static noinline fastcall int
mx_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return mx_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mx_m_sig(q, mp);
	case M_IOCTL:
		return mx_m_ioctl(q, mp);
	case M_IOCDATA:
		return mx_m_iocdata(q, mp);
	case M_FLUSH:
		return mx_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return mx_m_rse(q, mp);
	default:
		return mx_m_other(q, mp);
	case M_DATA:
		return mx_m_data(q, mp);
	}
}

/**
 * mx_msg: - process STREAMS message
 * @q: active queue
 * @mp: the message
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
mx_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return mx_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return mx_m_proto(q, mp);
	return mx_msg_slow(q, mp);
}

/**
 * __mx_msg_slow: - process STREAMS message, slow
 * @mx: locked private structure
 * @q: active queue
 * @mp: the STREAMS message
 */
static noinline fastcall int
__mx_msg_slow(struct mx *mx, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __mx_m_proto(mx, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __mx_m_sig(mx, q, mp);
	case M_IOCTL:
		return __mx_m_ioctl(mx, q, mp);
	case M_IOCDATA:
		return __mx_m_iocdata(mx, q, mp);
	case M_FLUSH:
		return mx_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return mx_m_rse(q, mp);
	default:
		return mx_m_other(q, mp);
	case M_DATA:
		return __mx_m_data(mx, q, mp);
	}
}

/**
 * __mx_msg: - process STREAMS message locked
 * @mx: locked private structure
 * @q: active queue
 * @mp: the message
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
__mx_msg(struct mx *mx, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return __mx_m_data(mx, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return __mx_m_proto(mx, q, mp);
	return __mx_msg_slow(mx, q, mp);
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */

/**
 * mx_putp: - canonical put procedure
 * @q: active queue
 * @mp: message to put
 *
 * Quick canonical put procedure.
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
 *
 * Quick canonical service procedure.  This is a  little quicker for
 * processing bulked messages because it takes the lock once for the entire
 * group of M_DATA messages, instead of once for each message.
 */
static streamscall __hot_read int
mx_srvp(queue_t *q)
{
	mblk_t *mp;

	if (likely((mp = getq(q)) != NULL)) {
		caddr_t priv;

		if (likely((priv = mi_trylock(q)) != NULL)) {
			do {
				if (unlikely(__mx_msg(MX_PRIV(q), q, mp) != 0))
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
	struct mx_pair *p;
	mblk_t *tick;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((tick = mi_timer_alloc(0)) == NULL)
		return (ENOBUFS);
	if ((err = mi_open_comm(&mx_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		mi_timer_free(tick);
		return (err);
	}

	p = PRIV(q);
	bzero(p, sizeof(*p));

	/* initialize the structure */
	p->r_priv.pair = p;
	p->r_priv.other = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state.i_state = MXS_UNINIT;
	p->r_priv.oldstate.i_state = MXS_UNINIT;
	p->r_priv.state.i_flags = 0;
	p->r_priv.oldstate.i_flags = 0;

	p->r_priv.reqflags = 0;
	p->r_priv.tick = tick;
	p->r_priv.interval = 10;	/* milliseconds */

	p->r_priv.mx.config.type = MX_PARMS_CIRCUIT;
	p->r_priv.mx.config.encoding = MX_ENCODING_G711_PCM_U;	/* encoding */
	p->r_priv.mx.config.block_size = 2048;	/* data block size (bits) */
	p->r_priv.mx.config.samples = 8;	/* samples per block */
	p->r_priv.mx.config.sample_size = 8;	/* sample size (bits) */
	p->r_priv.mx.config.rate = MX_RATE_192000;	/* multiplex clock rate (samples/second) */
	p->r_priv.mx.config.tx_channels = 24;	/* number of tx channels */
	p->r_priv.mx.config.rx_channels = 24;	/* number of rx channels */
	p->r_priv.mx.config.opt_flags = MX_PARM_OPT_CLRCH;	/* option flags */

	p->r_priv.mx.notify.events = 0;
	p->r_priv.mx.statem.state = MXS_UNINIT;
	p->r_priv.mx.statem.flags = 0;
	p->r_priv.mx.statsp.header = 0;
	p->r_priv.mx.stats.header = 0;

	p->w_priv.pair = p;
	p->w_priv.other = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state.i_state = MXS_UNINIT;
	p->w_priv.oldstate.i_state = MXS_UNINIT;
	p->w_priv.state.i_flags = 0;
	p->w_priv.oldstate.i_flags = 0;

	p->w_priv.reqflags = 0;
	p->w_priv.tick = tick;
	p->w_priv.interval = 10;	/* milliseconds */

	p->w_priv.mx.config.type = MX_PARMS_CIRCUIT;
	p->w_priv.mx.config.encoding = MX_ENCODING_G711_PCM_U;	/* encoding */
	p->w_priv.mx.config.block_size = 2048;	/* data block size (bits) */
	p->w_priv.mx.config.samples = 8;	/* samples per block */
	p->w_priv.mx.config.sample_size = 8;	/* sample size (bits) */
	p->w_priv.mx.config.rate = MX_RATE_192000;	/* multiplex clock rate (samples/second) */
	p->w_priv.mx.config.tx_channels = 24;	/* number of tx channels */
	p->w_priv.mx.config.rx_channels = 24;	/* number of rx channels */
	p->w_priv.mx.config.opt_flags = MX_PARM_OPT_CLRCH;	/* option flags */

	p->w_priv.mx.notify.events = 0;
	p->w_priv.mx.statem.state = MXS_UNINIT;
	p->w_priv.mx.statem.flags = 0;
	p->w_priv.mx.statsp.header = 0;
	p->w_priv.mx.stats.header = 0;

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
	struct mx *mx = MX_PRIV(q);

	qprocsoff(q);
	mi_timer_free(mx->tick);
	mi_close_comm(&mx_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit mx_rinit = {
	.qi_putp = mx_putp,		/* Read put (message from below) */
	.qi_srvp = mx_srvp,		/* Read queue service */
	.qi_qopen = mx_qopen,		/* Each open */
	.qi_qclose = mx_qclose,		/* Last close */
	.qi_minfo = &mx_minfo,		/* Information */
	.qi_mstat = &mx_rstat,		/* Statistics */
};

static struct qinit mx_winit = {
	.qi_putp = mx_putp,		/* Write put (message from above) */
	.qi_srvp = mx_srvp,		/* Write queue service */
	.qi_minfo = &mx_minfo,		/* Information */
	.qi_mstat = &mx_wstat,		/* Statistics */
};

static struct streamtab mx_pmodinfo = {
	.st_rdinit = &mx_rinit,		/* Upper read queue */
	.st_wrinit = &mx_winit,		/* Upper write queue */
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
MODULE_PARM_DESC(modid, "Module ID for MX-PMOD module. (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw mx_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mx_pmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/**
 * mx_pmodinit: - initialize MX-PMOD
 */
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

/**
 * mx_pmodexit: - terminate MX-PMOD
 */
static __exit void
mx_pmodexit(void)
{
	int err;

	if ((err = unregister_strmod(&mx_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
		return;
	}
	return;
}

module_init(mx_pmodinit);
module_exit(mx_pmodexit);

#endif				/* LINUX */
