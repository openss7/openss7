/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 * This is a CH multiplexing driver.  Its purpose is to allow a single device,
 * /dev/streams/chmux to represent all channel device streams in the system.  The
 * CH SNMP agent opens the appropriate device drivers and links (I_PLINKs) them
 * under this multiplexing driver to form a single view of the device drivers.  It
 * is then possible to open a CH upper stream on this multiplexer and push modules
 * and link them under, for example, the SL-MUX multiplexing dirver.  Also, it is
 * possible to open a CH upper stream on the multiplexer, push appropriate
 * modules, and use the CH stream as, for example, a data link.
 *
 * Using the CH pipe module, it is possible to open a STREAMS-based pipe, push the
 * ch-pmod pipe module on one end of the pipe and then link both ends under this
 * multiplexing driver.  This permits testing of the multiplexing driver as well
 * as providing an internal switching matrix capability.
 *
 * The driver also supports pseudo-digital cross-connect between lower CH streams.
 * In general, however, it is better to use pseudo-digital cross-connect at the
 * MXMUX driver instead of this driver, unless one of the CH streams being
 * cross-connected does not belong to an MX stream.  An example of this situation
 * would be a pseudo-digital cross-connect of V.35 data to a channel in a T1 span
 * (basically channel-bank operation).
 *
 * CH streams might come from two sources: from the MXMUX pseudo-device driver
 * that provides CH access to channels within MX streams, or from drivers that
 * only support discrete channel operation (such as a V.35 interface driver).
 *
 * Lower CH Streams are always treated as Style 1 Streams.  If the lower Stream is
 * in fact a Style 2 CH Stream, it must be attached to a PPA before it is linked
 * beneath the multiplexing driver.  Clone upper CH Streams are always treated as
 * Style 2 Streams.  PPA addressing for the upper CH streams is performed using a
 * globally assigned PPA or CLEI that is assigned to a lower Stream at the time
 * that it is linked beneath the multiplexing driver.  Upper CH streams can be
 * attached to a PPA in one of two fashions: as a normal CH Stream, or as a monitor
 * Stream.  When attached as a monitor Stream, data that is passed in either
 * direction is copied to the monitor Stream.  This allows monitoring taps for
 * maintenance or for wiretap applications.
 *
 * Upper CH Stream can be clone streams or non-clone streams.  Clone streams are
 * Style 2 CH streams that can be attached to any available PPA.  Non-clone
 * Streams have a minor device number that maps to a global PPA and CLEI of a
 * lower Stream (the mapping is assigned when the Stream is I_PLINKED beneath the
 * multiplexing driver).  These are Style 1 CH streams.
 *
 * To reduce complexity initially, I_LINK operations are not permitted.
 * Eventually, they can be permitted in which case management operations between
 * lower CH streams I_LINKED under the multiplexing driver (management
 * input-output controls and connection of the control stream) will only permitted
 * within the group of channels linked by that control stream.
 */

#include <ss7/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>

#define CH_MUX_DESCRIP		"CH MULTIPLEX (CH-MUX) STREAMS MULTIPLEXING DRIVER"
#define CH_MUX_REVISION		"OpenSS7 $RCSfile$ $Name$($Revision$) $Date$"
#define CH_MUX_COPYRIGHT	"Copyright (c) 2008-2009  Monavacon Limited.  All Rights Reserved."
#define CH_MUX_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define CH_MUX_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define CH_MUX_LICENSE		"GPL"
#define CH_MUX_BANNER		CH_MUX_DESCRIP		"\n" \
				CH_MUX_REVISION		"\n" \
				CH_MUX_COPYRIGHT	"\n" \
				CH_MUX_DEVICE		"\n" \
				CH_MUX_CONTACT		"\n"
#define CH_MUX_SPLASH		CH_MUX_DESRIP		" - " \
				CH_MUX_REVISION

#ifdef MODULE
#define DRV_BANNER	CH_MUX_BANNER
#else				/* MODULE */
#define DRV_BANNER	CH_MUX_SPLASH
#endif				/* MODULE */
#define DRV_ID		CONFIG_STREAMS_CH_MUX_MODID
#define DRV_NAME	CONFIG_STREAMS_CH_MUX_NAME

#if defined LINUX
MODULE_AUTHOR(CH_MUX_CONTACT);
MODULE_DESCRIPTION(CH_MUX_DESCRIP);
MODULE_SUPPORT_DEVICE(CH_MUX_DEVICE);
#if defined MODULE_LICENSE
MODULE_LICENSE(CH_MUX_LICENSE);
#endif				/* defined MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ch-mux");
#endif				/* defined MODULE_ALIAS */
#if defined MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* defined MODULE_VERSION */
#endif				/* defined LINUX */

modID_t modid = DRV_ID;
major_t major = CONFIG_STREAMS_CH_MUX_MAJOR;

#if defined LINUX
#if defined module_param
module_param(modid, ushort, 0444);
module_Param(major, uint, 0444);
#else				/* defined module_param */
MODULE_PARM(modid, "h");
MODULE_PARM(major, "d");
#endif				/* defined module_param */
MODULE_PARM_DESC(modid, "Module id number for STREAMS CH-MUX driver (0 for allocation).");
MODULE_PARM_DESC(major, "Major device number for STREAMS CH-MUX driver (0 for allocation).");

#if defined MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CH_MUX_MAJOR));
MODULE_ALIAS("/dev/ch");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_CH_MUX_MAJOR));
MODULE_ALIAS("/dev/streams/ch");
MODULE_ALIAS("/dev/streams/ch/*");
MODULE_ALIAS("/dev/streams/clone/ch");
#endif				/* defined MODULE_ALIAS */
#endif				/* defined LINUX */

/*
 * PRIVATE STRUCTURE
 */

struct ch {
	queue_t *oq;
	queue_t *iq;
	struct ch *oth, *mon;
	struct {
		ch_ulong state;
	} state, oldstate;
	char addr[CH_MAX_ADDR_LEN];
	union CH_parms parm;
	int rerror;
	int werror;
	struct CH_info_ack info;
};

static rwlock_t ch_lock = RW_LOCK_UNLOCKED;
static caddr_t ch_opens = NULL;
static caddr_t ch_links = NULL;

/*
 * PRINT ROUTINES
 */

#define STRLOGERR	0
#define STRLOGNO	0
#define STRLOGST	1
#define STRLOGTX	2
#define STRLOGRX	3
#define STRLOGIO	4
#define STRLOGTO	5
#define STRLOGTE	6
#define STRLOGDA	7

static inline fastcall const char *
ch_primname(const ch_ulong prim)
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
		return ("(unknown)");
	}
}

static inline fastcall const char *
ch_statename(const ch_ulong state)
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
		return ("(unknown)");
	}
}

static inline const char *
ch_errname(const ch_long error)
{
	switch (error) {
	case CHSYSERR:
		return ("CHSYSERR");
	case CHBADADDR:
		return ("CHBADADDR");
	case CHOUTSTATE:
		return ("CHOUTSTATE");
	case CHBADOPT:
		return ("CHBADOPT");
	case CHBADPARM:
		return ("CHBADPARM");
	case CHBADPARMTYPE:
		return ("CHBADPARMTYPE");
	case CHBADFLAG:
		return ("CHBADFLAG");
	case CHBADPRIM:
		return ("CHBADPRIM");
	case CHNOTSUPP:
		return ("CHNOTSUPP");
	case CHBADSLOT:
		return ("CHBADSLOT");
	case -EAGAIN:
		return ("EAGAIN");
	case -ENOMEM:
		return ("ENOMEM");
	case -ENOBUFS:
		return ("ENOBUFS");
	case -EDEADLK:
		return ("EDEADLK");
	case -EBUSY:
		return ("EBUSY");
	case -EIO:
		return ("EIO");
	case -EPROTO:
		return ("EPROTO");
	case -EFAULT:
		return ("EFAULT");
	case -EMSGSIZE:
		return ("EMSGSIZE");
	case -EINVAL:
		return ("EINVAL");
	case -ENOSR:
		return ("ENOSR");
	case -ETIMEDOUT:
		return ("ETIMEDOUT");
	default:
		return ("(unknown)");
	}
}

static inline const char *
ch_strerror(const ch_long error)
{
	switch (error) {
	case CHSYSERR:
		return ("UNIX system error");
	case CHBADADDR:
		return ("Bad address format or content");
	case CHOUTSTATE:
		return ("Interface out of state");
	case CHBADOPT:
		return ("Bad options format or content");
	case CHBADPARM:
		return ("Bad parameter format or content");
	case CHBADPARMTYPE:
		return ("Bad parameter structure type");
	case CHBADFLAG:
		return ("Bad flag");
	case CHBADPRIM:
		return ("Bad primitive");
	case CHNOTSUPP:
		return ("Primitive not supported");
	case CHBADSLOT:
		return ("Bad multiplex slot");
	case -EAGAIN:
		return ("Try again");
	case -ENOMEM:
		return ("Insufficient memory");
	case -ENOBUFS:
		return ("Insufficient buffers");
	case -EDEADLK:
		return ("Deadlock could ensue");
	case -EBUSY:
		return ("Resource busy");
	case -EIO:
		return ("Input-output error");
	case -EPROTO:
		return ("Protocol error");
	case -EFAULT:
		return ("Software fault");
	case -EMSGSIZE:
		return ("Bad message size");
	case -EINVAL:
		return ("Invalid argument");
	case -ENOSR:
		return ("No STREAMS resource");
	case -ETIMEDOUT:
		return ("Operation timed out");
	default:
		return ("(unknown)");
	}
}

/*
 * STATE CHANGES
 */

static inline fastcall ch_ulong
ch_get_state(struct ch *ch)
{
	return ch->state.state;
}

static inline fastcall ch_ulong
ch_set_state(struct ch *ch, const ch_ulong newstate)
{
	ch_ulong oldstate = ch_get_state(ch);

	if (newstate != oldstate) {
		mi_strlog(ch->oq, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
		ch->state.state = newstate;
		ch->info.ch_state = newstate;
	}
	return (newstate);
}

static inline fastcall void
ch_save_state(struct ch *ch)
{
	ch->oldstate = ch->state;
}

static inline fastcall void
ch_restore_state(struct ch *ch)
{
	ch_set_state(ch, ch->oldstate.state);
	ch->state = ch->oldstate;
}

static inline fastcall void
ch_stripmsg(mblk_t *msg, mblk_t *mp)
{
	mblk_t *b, *b_next = msg;

	while ((b = b_next) && b != mp->b_cont) {
		b_next = b->b_next;
		freeb(b);
	}
}

static inline fastcall void
ch_rxprim(struct ch *ch, queue_t *q, mblk_t *mp, const ch_ulong prim)
{
	queue_t *oq = ch->oq;
	const char *fmt = (oq->q_flag & QREADR) ? "%s <-" : "-> %s";
	int level;

	switch (__builtin_expect(prim, CH_DATA_REQ)) {
	case CH_DATA_REQ:
	case CH_DATA_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(oq, level, SL_TRACE, fmt, ch_primname(prim));
	return;
}

static inline fastcall void
ch_txprim(struct ch *ch, queue_t *q, mblk_t *msg, mblk_t *mp, const ch_ulong prim)
{
	queue_t *oq = ch->oq;
	const char *fmt = (oq->q_flag & QREADR) ? "<- %s" : "%s ->";
	int level;

	switch (__builtin_expect(prim, CH_DATA_REQ)) {
	case CH_DATA_REQ:
	case CH_DATA_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(oq, level, SL_TRACE, fmt, ch_primname(prim));
	ch_stripmsg(msg, mp);
	putnext(oq, mp);
	return;
}

/*
 * PRIMITIVES ISSUED UPSTREAM
 */

/**
 * ch_info_ack: - issue CH_INFO_ACK primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_info_ack *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + ch->info.ch_addr_length + ch->info.ch_parm_length;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_INFO_ACK;
	p->ch_addr_length = ch->info.ch_addr_length;
	p->ch_addr_offset = ch->info.ch_addr_length ? sizeof(*p) : 0;
	p->ch_parm_length = ch->info.ch_parm_length;
	p->ch_parm_offset = ch->info.ch_parm_length ? p->ch_addr_offset + p->ch_addr_length : 0;
	p->ch_prov_flags = ch->info.ch_prov_flags;
	p->ch_prov_class = ch->info.ch_prov_class;
	p->ch_style = ch->info.ch_style;
	p->ch_version = ch->info.ch_version;
	p->ch_state = ch_get_state(ch);
	mp->b_wptr += sizeof(*p);
	bcopy(&ch->addr, mp->b_wptr, p->ch_addr_length);
	mp->b_wptr += p->ch_addr_length;
	bcopy(&ch->parm, mp->b_wptr, p->ch_parm_length);
	mp->b_wptr += p->ch_parm_length;
	ch_txprim(ch, q, msg, mp, CH_INFO_ACK);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_ack: - issue CH_OPGMGMT_ACK primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, ch_ulong flags)
{
	struct CH_optmgmt_ack *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_OPTMGMT_ACK;
	p->ch_opt_length = olen;
	p->ch_opt_offset = olen ? sizeof(*p) : 0;
	p->ch_mgmt_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(optr, mp->b_wptr, p->ch_opt_length);
	mp->b_wptr += p->ch_opt_length;
	ch_txprim(ch, q, msg, mp, CH_OPTMGMT_ACK);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_ok_ack: - issue CH_OK_ACK primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim)
{
	struct CH_ok_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	ch_ulong state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	switch ((state = ch_get_state(ch))) {
	case CHS_WACK_AREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_UREQ:
		state = CHS_DETACHED;
		break;
	case CHS_WACK_EREQ:
		state = CHS_ENABLED;
		break;
	case CHS_WACK_RREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_CREQ:
		state = CHS_CONNECTED;
		break;
	case CHS_WACK_DREQ:
		state = CHS_ENABLED;
		break;
	}
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_OK_ACK;
	p->ch_correct_prim = prim;
	p->ch_state = ch_set_state(ch, state);
	mp->b_wptr += sizeof(*p);
	ch_txprim(ch, q, msg, mp, CH_OK_ACK);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_error_ack: - issue CH_ERROR_ACK primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim, ch_long error)
{
	struct CH_error_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	ch_ulong state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	ch_restore_state(ch);
	switch ((state = ch_get_state(ch))) {
	case CHS_WACK_AREQ:
		state = CHS_DETACHED;
		break;
	case CHS_WACK_UREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_EREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_RREQ:
		state = CHS_ENABLED;
		break;
	case CHS_WACK_CREQ:
		state = CHS_ENABLED;
		break;
	case CHS_WACK_DREQ:
		state = CHS_CONNECTED;
		break;
	}
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_ERROR_ACK;
	p->ch_error_primitive = prim;
	p->ch_error_type = error < 0 ? CHSYSERR : error;
	p->ch_unix_error = error < 0 ? -error : 0;
	p->ch_state = ch_set_state(ch, state);
	mp->b_wptr += sizeof(*p);
	ch_txprim(ch, q, msg, mp, CH_ERROR_ACK);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_enable_con: - issue CH_ENABLE_CON primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_con *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_ENABLE_CON;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_ENABLED);
	ch_txprim(ch, q, msg, mp, CH_ENABLE_CON);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_connect_con: - issue CH_CONNECT_CON primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_connect_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_CONNECT_CON;
	p->ch_conn_flags = flags;
	p->ch_slot = slot;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_CONNECTED);
	ch_txprim(ch, q, msg, mp, CH_CONNECT_CON);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_data_ind: - issue CH_DATA_IND primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong slot, mblk_t *dp)
{
	struct CH_data_ind *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DATA_IND;
	p->ch_slot = slot;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(ch->oq, STRLOGDA, SL_TRACE, "<- CH_DATA_IND");
	ch_stripmsg(msg, mp);
	putnext(ch->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_disconnect_ind: - issue CH_DISCONNECT_IND primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong cause,
		  ch_ulong slot)
{
	struct CH_disconnect_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISCONNECT_IND;
	p->ch_conn_flags = flags;
	p->ch_cause = cause;
	p->ch_slot = slot;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_ENABLED);
	ch_txprim(ch, q, msg, mp, CH_DISCONNECT_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_disconnect_con: - issue CH_DISCONNECT_CON primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISCONNECT_CON;
	p->ch_conn_flags = flags;
	p->ch_slot = slot;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_ENABLED);
	ch_txprim(ch, q, msg, mp, CH_DISCONNECT_CON);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_disable_ind: - issue CH_DISABLE_IND primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong cause)
{
	struct CH_disable_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISABLE_IND;
	p->ch_cause = cause;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_ATTACHED);
	ch_txprim(ch, q, msg, mp, CH_DISABLE_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_disable_con: - issue CH_DISABLE_CON primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISABLE_CON;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_ATTACHED);
	ch_txprim(ch, q, msg, mp, CH_DISABLE_CON);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_event_ind: - issue CH_EVENT_IND primitive upstream
 * @ch: upper private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong event, ch_ulong slot)
{
	struct CH_event_ind *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_EVENT_IND;
	p->ch_event = event;
	p->ch_slot = slot;
	mp->b_wptr += sizeof(*p);
	ch_txprim(ch, q, msg, mp, CH_EVENT_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/*
 * PRIMITIVES ISSUED DOWNSTREAM
 */

/**
 * ch_info_req: - issue CH_INFO_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_info_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	ch_txprim(ch, q, msg, mp, CH_INFO_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_req: - issue CH_OPTMGMT_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, ch_ulong flags)
{
	struct CH_optmgmt_req *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_OPTMGMT_REQ;
	p->ch_opt_length = olen;
	p->ch_opt_offset = olen ? sizeof(*p) : 0;
	p->ch_mgmt_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(optr, mp->b_wptr, p->ch_opt_length);
	mp->b_wptr += p->ch_opt_length;
	ch_txprim(ch, q, msg, mp, CH_OPTMGMT_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_attach_req: - issue CH_ATTACH_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, ch_ulong flags)
{
	struct CH_attach_req *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_ATTACH_REQ;
	p->ch_addr_length = alen;
	p->ch_addr_offset = alen ? sizeof(*p) : 0;
	p->ch_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(aptr, mp->b_wptr, p->ch_addr_length);
	mp->b_wptr += p->ch_addr_length;
	ch_set_state(ch, CHS_WACK_AREQ);
	ch_txprim(ch, q, msg, mp, CH_ATTACH_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_enable_req: - issue CH_ENABLE_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_ENABLE_REQ;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_WACK_EREQ);
	ch_txprim(ch, q, msg, mp, CH_ENABLE_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_connect_req: - issue CH_CONNECT_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_connect_req *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_CONNECT_REQ;
	p->ch_conn_flags = flags;
	p->ch_slot = slot;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_WACK_CREQ);
	ch_txprim(ch, q, msg, mp, CH_CONNECT_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_data_req: - issue CH_DATA_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong slot, mblk_t *dp)
{
	struct CH_data_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DATA_REQ;
	p->ch_slot = sloft;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	mi_strlog(ch->oq, STRLOGDA, SL_TRACE, "CH_DATA_REQ ->");
	ch_stripmsg(msg, mp);
	putnext(ch->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_disconnect_req: - issue CH_DISCONNECT_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISCONNECT_REQ;
	p->ch_conn_flags = flags;
	p->ch_slot = slot;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_WACK_DREQ);
	ch_txprim(ch, q, msg, mp, CH_DISCONNECT_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_disable_req: - issue CH_DISABLE_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISABLE_REQ;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_WACK_RREQ);
	ch_txprim(ch, q, msg, mp, CH_DISABLE_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * ch_detach_req: - issue CH_DETACH_REQ primitive downstream
 * @ch: lower private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_detach_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(ch->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DETACH_REQ;
	mp->b_wptr += sizeof(*p);
	ch_set_state(ch, CHS_WACK_UREQ);
	ch_txprim(ch, q, msg, mp, CH_DETACH_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/*
 * PRIMITIVES PASSED ALONG AND ERROR REPLIES
 */

noinline fastcall int
ch_passalong(struct ch *ch, queue_t *q, mblk_t *mp, const ch_ulong prim)
{
	struct ch *oth;
	int err = 0;

	read_lock(&ch_lock);
	if (likely(!!(oth = ch->oth))) {
		/* propagate state change */
		ch_set_state(oth, ch_get_state(ch));
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(oth->oq, mp->b_band))
			err = -EBUSY;
		else
			ch_txprim(oth, q, NULL, mp, prim);
	} else
		err = -EIO;
	read_unlock(&ch_lock);
	return (err);
}

/**
 * ch_error_reply: - reply with error for primitive
 * @ch: upper private structure (locked)
 * @q: active queue (upper write queue)
 * @msg: the primitive in error
 * @prim: the primitive type of the primitive in error
 * @error: the negative UNIX error or positive CHI error
 */
noinline fastcall int
ch_error_reply(struct ch *ch, queue_t *q, mblk_t *msg, const ch_ulong prim, const ch_long error)
{
	switch (error) {
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EBUSY:
	case -EDEADLK:
	case 0:
		return (error);
	case -EIO:
		return m_error_reply(ch, q, msg, EIO, EIO);
	default:
		break;
	}
	switch (prim) {
	case CH_INFO_REQ:
	case CH_OPTMGMT_REQ:
	case CH_ATTACH_REQ:
	case CH_ENABLE_REQ:
	case CH_CONNECT_REQ:
	case CH_DISCONNECT_REQ:
	case CH_DISABLE_REQ:
	case CH_DETACH_REQ:
		return ch_error_ack(ch, q, msg, prim, error);
	default:
	case CH_DATA_REQ:
		return m_error_reply(ch, q, msg, EPROTO, EPROTO);
	}
}

/*
 * CH USER to CH PROVIDER primitives (from above)
 */

/**
 * ch_w_info_req: - process CH_INFO_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_INFO_REQ primitive
 *
 * If we are attached to a lower Stream, simply pass the message along, otherwise, respond with
 * local information.
 */
noinline fastcall int
ch_w_info_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_info_req *p = (typeof(p)) mp->b_rptr;
	struct ch *oth;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	read_lock(&ch_lock);
	if (likely(!!(oth = ch->oth))) {
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(oth->oq, mp->b_band))
			err = -EBUSY;
		else
			putnext(oth->oq, mp);
	} else
		err = ch_info_ack(ch, q, mp);
	read_unlock(&ch_lock);
      error:
	return (err);
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/**
 * ch_w_optmgmt_req: - process CH_OPTMGMT_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_OPTMGMT_REQ primitive
 *
 * If we are attached to a lower Stream, simply pass the message long, otherwise, return an out of
 * state error.
 */
noinline fastcall int
ch_w_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct ch *oth;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(!MBLKIN(mp, p->ch_opt_offset, p->ch_opt_length)))
		goto badopt;
	read_lock(&ch_lock);
	if (likely(!!(oth = ch->oth))) {
		if (!pcmsg(DB_TYPE(mp)) && !bcanpuntext(oth->oq, mp->b_band))
			err = -EBUSY;
		else
			putnext(oth->oq, mp);
	} else
		err = ch_error_ack(ch, q, mp, CH_OPTMGMT_REQ, CHOUTSTATE);
	read_unlock(&ch_lock);
      error:
	return (err);
      badopt:
	err = CHBADOPT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/**
 * ch_w_attach_req: - process CH_ATTACH_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_ATTACH_REQ primitive
 */
noinline fastcall int
ch_w_attach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_attach_req *p = (typeof(p)) mp->b_rptr;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(!MBLKIN(mp, p->ch_addr_offset, p->ch_addr_length)))
		goto badaddr;
	if (ch_get_state(ch) != CHS_DETACHED)
		goto oustate;
      error:
	return (err);
      oustate:
	err = CHOUTSTATE;
	goto error;
      badaddr:
	err = CHBADADDR;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;

}

/**
 * ch_w_enable_req: - process CH_ENABLE_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_ENABLE_REQ primitive
 */
noinline fastcall int
ch_w_enable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_req *p = (typeof(p)) mp->b_rptr;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	ch_set_state(ch, CHS_WACK_EREQ);
	err = ch_passalong(ch, q, mp, CH_ENABLE_REQ);
      error:
	return (err);
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/**
 * ch_w_connect_req: - process CH_CONNECT_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_CONNECT_REQ primitive
 */
noinline fastcall int
ch_w_connect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_req *p = (typeof(p)) mp->b_rptr;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	ch_set_state(ch, CHS_WACK_CREQ);
	err = ch_passalong(ch, q, mp, CH_CONNECT_REQ);
      error:
	return (err);
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/**
 * ch_w_data_req: - process CH_DATA_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_DATA_REQ primitive
 */
noinline fastcall int
ch_w_data_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_req *p = (typeof(p)) mp->b_rptr;
	struct ch *oth;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	read_lock(&ch_lock);
	if (likely(!!(oth = ch->oth))) {
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(oth->oq, mp->b_band))
			err = -EBUSY;
		else {
			mi_strlog(oth->oq, STRLOGDA, SL_TRACE, "CH_DATA_REQ ->");
			putnext(oth->oq, mp);
		}
	} else
		err = -EIO;
	read_unlock(&ch_lock);
      error:
	return (err);
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/**
 * ch_w_disconnect_req: - process CH_DISCONNECT_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_DISCONNECT_REQ primitive
 */
noinline fastcall int
ch_w_disconnect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_req *p = (typeof(p)) mp->b_rptr;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	ch_set_state(ch, CHS_WACK_DREQ);
	err = ch_passalong(ch, q, mp, CH_DISCONNECT_REQ);
      error:
	return (err);
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/**
 * ch_w_disable_req: - process CH_DISABLE_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_DISABLE_REQ primitive
 */
noinline fastcall int
ch_w_disable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_req *p = (typeof(p)) mp->b_rptr;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	ch_set_state(ch, CHS_WACK_RREQ);
	err = ch_passalong(ch, q, mp, CH_DISABLE_REQ);
      error:
	return (err);
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/**
 * ch_w_detach_req: - process CH_DETACH_REQ primitive from above
 * @ch: upper private structure locked
 * @q: active queue (upper write queue)
 * @mp: the CH_DETACH_REQ primitive
 */
noinline fastcall int
ch_w_detach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_detach_req *p = (typeof(p)) mp->b_rptr;
	int err = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (ch_get_state(ch) != CHS_ATTACHED)
		goto outstate;
	/* FIXME detach stream */
	ch_set_state(ch, CHS_WACK_UREQ);
	err = t_ok_ack(chm q, mp, CH_DETACH_REQ);
      error:
	return (err);
      outstate:
	err = CHOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/*
 * CH PROVIDER to CH USER primitives (from below)
 */

/**
 * ch_r_info_ack: - process CH_INFO_ACK primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_INFO_ACK primitive
 */
noinline fastcall int
ch_r_info_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct ch *oth;
	int err;

	ch->info.ch_primitive = CH_INFO_ACK;
	bcopy(mp->b_rptr + p->ch_addr_offset, &ch->addr, p->ch_addr_length);
	ch->ch_addr_length = p->ch_addr_length;
	bcopy(mp->b_rptr + p->ch_parm_offset, &ch->parm, p->ch_parm_length);
	ch->ch_parm_length = p->ch_parm_length;
	ch->ch_prov_flags = p->ch_prov_flags;
	ch->ch_prov_class = p->ch_prov_class;
	ch->ch_style = p->ch_style;
	ch->ch_version = p->ch_version;
	ch->ch_state = p->ch_state;
	p->ch_prov_flags = 0;
	p->ch_prov_class = CH_CIRCUIT;
	p->ch_style = CH_STYLE2;
	p->ch_version = CH_VERSION;
	p->ch_state = ch_get_state(ch);
	read_lock(&ch_lock);
	if (likely(!!(oth = ch->oth))) {
		p->ch_state = ch_set_state(oth, ch_get_state(ch));
		oth->info.ch_primitive = CH_INFO_ACK;
		bcopy(mp->b_rptr + p->ch_addr_offset, &oth->addr, p->ch_addr_length);
		oth->ch_addr_length = p->ch_addr_length;
		bcopy(mp->b_rptr + p->ch_parm_offset, &oth->parm, p->ch_parm_length);
		oth->ch_parm_length = p->ch_parm_length;
		oth->ch_prov_flags = p->ch_prov_flags;
		oth->ch_prov_class = p->ch_prov_class;
		oth->ch_style = p->ch_style;
		oth->ch_version = p->ch_version;
		oth->ch_state = p->ch_state;
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(oth->oq, mp->b_band))
			err = -EBUSY;
		else
			ch_txprim(oth, q, NULL, mp, CH_INFO_ACK);
	} else
		err = -EIO;
	read_unlock(&ch_lock);
	return (err);
}

/**
 * ch_r_optmgmt_ack: - process CH_OPTMGMT_ACK primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_OPTMGMT_ACK primitive
 */
noinline fastcall int
ch_r_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_passalong(ch, q, mp, CH_OPTMGMT_ACK);
}

/**
 * ch_r_ok_ack: - process CH_OK_ACK primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_OK_ACK primitive
 */
noinline fastcall int
ch_r_ok_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	switch (ch_get_state(ch)) {
	case CHS_WACK_AREQ:
		ch_set_state(ch, CHS_ATTACHED);
		break;
	case CHS_WACK_UREQ:
		ch_set_state(ch, CHS_DETACHED);
		break;
	case CHS_WACK_EREQ:
		ch_set_state(ch, CHS_ENABLED);
		break;
	case CHS_WACK_RREQ:
		ch_set_state(ch, CHS_ATTACHED);
		break;
	case CHS_WACK_CREQ:
		ch_set_state(ch, CHS_CONNECTED);
		break;
	case CHS_WACK_DREQ:
		ch_set_state(ch, CHS_ENABLED);
		break;
	}
	return ch_passalong(ch, q, mp, CH_OK_ACK);
}

/**
 * ch_r_error_ack: - process CH_ERROR_ACK primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_ERROR_ACK primitive
 */
noinline fastcall int
ch_r_error_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	switch (ch_get_state(ch)) {
	case CHS_WACK_AREQ:
		ch_set_state(ch, CHS_DETACHED);
		break;
	case CHS_WACK_UREQ:
		ch_set_state(ch, CHS_ATTACHED);
		break;
	case CHS_WACK_EREQ:
		ch_set_state(ch, CHS_ATTACHED);
		break;
	case CHS_WACK_RREQ:
		ch_set_state(ch, CHS_ENABLED);
		break;
	case CHS_WACK_CREQ:
		ch_set_state(ch, CHS_ENABLED);
		break;
	case CHS_WACK_DREQ:
		ch_set_state(ch, CHS_CONNECTED);
		break;
	}
	return ch_passalong(ch, q, mp, CH_ERROR_ACK);
}

/**
 * ch_r_enable_con: - process CH_ENABLE_CON primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_ENABLE_CON primitive
 */
noinline fastcall int
ch_r_enable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_set_state(ch, CHS_ENABLED);
	return ch_passalong(ch, q, mp, CH_ENABLE_CON);
}

/**
 * ch_r_connect_con: - process CH_CONNECT_CON primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_CONNECT_CON primitive
 */
noinline fastcall int
ch_r_connect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_set_state(ch, CHS_CONNECTED);
	return ch_passalong(ch, q, mp, CH_CONNECT_CON);
}

/**
 * ch_r_data_ind: - process CH_DATA_IND primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_DATA_IND primitive
 */
noinline fastcall int
ch_r_data_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct ch *oth;
	int err = 0;

	read_lock(&ch_lock);
	if (likely(!!(oth = ch->oth))) {
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(oth->oq, mp->b_band))
			err = -EBUSY;
		else {
			mi_strlog(oth->oq, STRLOGDA, SL_TRACE, "<- CH_DATA_IND");
			putnext(oth->oq, mp);
		}
	} else
		err = -EIO;
	read_unlock(&ch_lock);
	return (err);
}

/**
 * ch_r_disconnect_ind: - process CH_DISCONNECT_IND primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_DISCONNECT_IND primitive
 */
noinline fastcall int
ch_r_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_set_state(ch, CHS_ENABLED);
	return ch_passalong(ch, q, mp, CH_DISCONNECT_IND);
}

/**
 * ch_r_disconnect_con: - process CH_DISCONNECT_CON primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_DISCONNECT_CON primitive
 */
noinline fastcall int
ch_r_disconnect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_set_state(ch, CHS_ENABLED);
	return ch_passalong(ch, q, mp, CH_DISCONNECT_CON);
}

/**
 * ch_r_disable_ind: - process CH_DISABLE_IND primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_DISABLE_IND primitive
 */
noinline fastcall int
ch_r_disable_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_set_state(ch, CHS_ATTACHED);
	return ch_passalong(ch, q, mp, CH_DISABLE_IND);
}

/**
 * ch_r_disable_con: - process CH_DISABLE_CON primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_DISABLE_CON primitive
 */
noinline fastcall int
ch_r_disable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_set_state(ch, CHS_ATTACHED);
	return ch_passalong(ch, q, mp, CH_DISABLE_CON);
}

/**
 * ch_r_event_ind: - process CH_EVENT_IND primitive from below
 * @ch: lower private structure locked
 * @q: active queue (lower read queue)
 * @mp: the CH_EVENT_IND primitive
 */
noinline fastcall int
ch_r_event_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_passalong(ch, q, mp, CH_EVENT_IND);
}

/*
 * INPUT-OUTPUT CONTROL HANDLING
 */

/**
 * ch_i_link: - process I_LINK input-output control
 * @ch: upper private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the message
 * @ioc: the iocblk contained in the message
 */
noinline fastcall int
ch_i_link(struct ch *ch, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
}

/**
 * ch_i_plink: - process I_PLINK input-output control
 * @ch: upper private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the message
 * @ioc: the iocblk contained in the message
 */
noinline fastcall int
ch_i_plink(struct ch *ch, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
}

/**
 * ch_i_unlink: - process I_UNLINK input-output control
 * @ch: upper private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the message
 * @ioc: the iocblk contained in the message
 */
noinline fastcall int
ch_i_unlink(struct ch *ch, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
}

/**
 * ch_i_punlink: - process I_PUNLINK input-output control
 * @ch: upper private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the message
 * @ioc: the iocblk contained in the message
 */
noinline fastcall int
ch_i_punlink(struct ch *ch, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
}

/**
 * ch_i_str: - process I_STR input-output control
 * @ch: upper private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the message
 * @ioc: the iocblk contained in the message
 */
noinline fastcall int
ch_i_str(struct ch *ch, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
	int err;

	if (DB_TYPE(mp) != M_IOCTL)
		goto iocnak;
	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		err = ch_i_link(ch, q, mp, ioc);
		break;
	case _IOC_NR(I_PLINK):
		err = ch_i_plink(ch, q, mp, ioc);
		break;
	case _IOC_NR(I_UNLINK):
		err = ch_i_unlink(ch, q, mp, ioc);
		break;
	case _IOC_NR(I_PUNLINK):
		err = ch_i_punlink(ch, q, mp, ioc);
		break;
	default:
		goto iocnak;
	}
	return (err);
      iocnak:
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/*
 * Other message.
 */

/**
 * m_other: - process other message
 * @q: active queue (upper write or lower read queue)
 * @mp: the message
 */
noinline fastcall int
m_other(queue_t *q, mblk_t *mp)
{
	struct ch *oth;
	int err = 0;

	read_lock(&ch_lock);
	if (likely(!!(oth = ch->oth))) {
		if (unlikely(!pcmsg(DB_TYPE(mp)) && !bcanputnext(oth->oq, mp->b_band)))
			err = -EBUSY;
		else
			putnext(oth->oq, mp);
	} else {
		mi_strlog(q, STRLOGERR, SL_TRACE | SL_ERROR, "unexpected message type %d",
			  (int) DB_TYPE(mp));
		freemsg(mp);
	}
	read_unlock(&ch_lock);
	return (err);
}

/*
 * M_DATA, M_HPDATA HANDLING
 */

/**
 * ch_data: - process M_DATA or M_HPDATA message
 * @ch: private structure (locked)
 * @q: active queue (upper write or lower read queue)
 * @mp: the message
 */
noinline fastcall int
ch_data(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return m_other(q, mp);
}

/*
 * M_PROTO, M_PCPROTO HANDLING
 */

/**
 * ch_w_proto: - process write M_PROTO or M_PCPROTO message
 * @ch: upper private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the message
 */
noinline fastcall int
ch_w_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong prim = -1U;
	int err = -EMSGSIZE;

	ch_save_state(ch);
	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	switch (prim) {
	case CH_INFO_REQ:
		err = ch_w_info_req(ch, q, mp);
		break;
	case CH_OPTMGMT_REQ:
		err = ch_w_optmgmt_req(ch, q, mp);
		break;
	case CH_ATTACH_REQ:
		err = ch_w_attach_req(ch, q, mp);
		break;
	case CH_ENABLE_REQ:
		err = ch_w_enable_req(ch, q, mp);
		break;
	case CH_CONNECT_REQ:
		err = ch_w_connect_req(ch, q, mp);
		break;
	case CH_DATA_REQ:
		err = ch_w_data_req(ch, q, mp);
		break;
	case CH_DISCONNECT_REQ:
		err = ch_w_disconnect_req(ch, q, mp);
		break;
	case CH_DISABLE_REQ:
		err = ch_w_disable_req(ch, q, mp);
		break;
	case CH_DETACH_REQ:
		err = ch_w_detach_req(ch, q, mp);
		break;
	default:
		err = CHNOTSUPP;
		break;
	}
      tooshort:
	switch (err) {
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EBUSY:
	case -EDEADLK:
		ch_restore_state(ch);
		/* fall through */
	case 0:
		return (err);
	default:
		ch_restore_state(ch);
		break;
	}
	return ch_error_reply(ch, q, mp, prim, err);
}

/**
 * ch_r_proto: - process read M_PROTO or M_PCPROTO message
 * @ch: lower private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the message
 */
noinline fastcall int
ch_r_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong prim = -1U;
	int err = -EMSGSIZE;

	ch_save_state(ch);
	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	switch (prim) {
	case CH_INFO_ACK:
		err = ch_r_info_ack(ch, q, mp);
		break;
	case CH_OPTMGMT_ACK:
		err = ch_r_optmgmt_ack(ch, q, mp);
		break;
	case CH_OK_ACK:
		err = ch_r_ok_ack(ch, q, mp);
		break;
	case CH_ERROR_ACK:
		err = ch_r_error_ack(ch, q, mp);
		break;
	case CH_ENABLE_CON:
		err = ch_r_enable_con(ch, q, mp);
		break;
	case CH_CONNECT_CON:
		err = ch_r_connect_con(ch, q, mp);
		break;
	case CH_DATA_IND:
		err = ch_r_data_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_IND:
		err = ch_r_disconnect_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_CON:
		err = ch_r_disconnect_con(ch, q, mp);
		break;
	case CH_DISABLE_IND:
		err = ch_r_disable_ind(ch, q, mp);
		break;
	case CH_DISABLE_CON:
		err = ch_r_disable_con(ch, q, mp);
		break;
	case CH_EVENT_IND:
		err = ch_r_event_ind(ch, q, mp);
		break;
	default:
		err = CHNOTSUPP;
		break;
	}
	err = m_other(q, mp);
      tooshort:
	switch (err) {
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EBUSY:
	case -EDEADLK:
		ch_restore_state(ch);
		/* fall through */
	case 0:
		return (err);
	default:
		ch_restore_state(ch);
		break;
	}
	mi_strlog(q, STRLOGERR, SL_TRACE | SL_ERROR, "error processing primitive %s, %s: %s",
		  ch_primname(prim), ch_errname(err), ch_strerror(err));
	freemsg(mp);
	return (0);
}

/*
 * M_IOCTL, M_IOCDATA HANDLING
 */

/**
 * ch_w_ioctl: - process M_IOCTL, M_IOCDATA message
 * @ch: upper private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the message
 */
noinline fastcall int
ch_w_ioctl(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct ch *oth;
	int err = 0;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case _IOC_TYPE(I_STR):
		err = ch_i_str(ch, q, mp, ioc);
		break;
	default:
		read_lock(&ch_lock);
		if (likely(!!(oth = ch->oth))) {
			if (DB_TYPE(mp) == M_IOCTL)
				CH_PRIV(q)->ioc = ioc->ioc_id;
			putnext(oth->oq, mp);
		} else
			mi_copy_done(q, mp, EINVAL);
		read_unlock(&ch_lock);
		break;
	}
	return (err);
}

/*
 * M_IOCACK, M_IOCNAK, M_COPYIN, M_COPYOUT HANDLING
 */

/**
 * m_r_ioctl: - process M_IOCACK, M_IOCNAK, M_COPYIN or M_COPYOUT message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
noinline fastcall int
m_r_ioctl(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_IOCACK:
	case M_IOCNAK:
		CH_PRIV(q)->ioc = 0;
		break;
	}
	return m_other(q, mp);
}

/*
 * M_FLUSH HANDLING
 */

/**
 * m_flush: - process M_FLUSH message
 * @q: active queue (upper write or lower read queue)
 * @mp: the M_FLUSH message
 */
noinline fastcall int
m_flush(queue_t *q, mblk_t *mp)
{
	const int mflag = (q->q_flag & QREADR) ? FLUSHR : FLUSHW;
	const int oflag = (q->q_flag & QREADR) ? FLUSHW : FLUSHR;
	struct ch *oth;

	if (mp->b_rptr[0] & mflag) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	read_lock(&ch_lock);
	if (likely(!!(oth = ch->oth))) {
		putnext(oth->oq, mp);
		read_unlock(&ch_lock);
		return (0);
	}
	read_unlock(&ch_lock);
	if (mp->b_rptr[0] & oflag) {
		if (!(q->q_flag & QREADR) || !(mp->b_flag & MSGNOLOOP)) {
			mp->b_flag |= MSGNOLOOP;
			mp->b_rptr[0] &= ~mflag;
			qreply(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}

/*
 * M_ERROR, M_HANGUP HANDLING
 */

/**
 * m_error: - process M_ERROR or M_HANGUP message from below
 * @q: active queue (lower read queue)
 * @mp: the M_ERROR or M_HANGUP message
 */
noinline fastcall int
m_error(queue_t *q, mblk_t *mp)
{
	int err = 0;

	read_lock(&ch_lock);
	if (q->q_next) {
		putnext(q->q_next, mp);
	} else {
		struct ch *ch;

		if (!!(ch = (struct ch *) mi_trylock(q))) {
			if (DB_TYPE(mp) == M_ERROR) {
				ch->rerror = mp->b_rptr[0];
				ch->werror = mp->b_rptr[1];
			} else {
				ch->rerror = ENXIO;
				ch->werror = EPIPE;
			}
			mi_unlock((caddr_t) ch);
			freemsg(mp);
		} else
			err = -EDEADLK;
	}
	read_unlock(&ch_lock);
	return (err);
}

/*
 * STREAMS MESSAGE HANDLING
 */

/**
 * ch_w_msg: - service a write queue message
 * @q: active queue (upper write queue)
 * @mp: the message to service
 */
static inline fastcall int
ch_w_msg(struct ch *ch, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = ch_data(ch, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = ch_w_proto(ch, q, mp);
		break;
	case M_IOCTL:
	case M_IOCDATA:
		err = ch_w_ioctl(ch, q, mp);
		break;
	case M_FLUSH:
		err = m_flush(q, mp);
		break;
	default:
		err = m_other(q, mp);
		break;
	}
	return (err);
}

/**
 * ch_r_msg: - service a read queue message
 * @q: active queue (lower read queue)
 * @mp: the message to service
 */
static inline fastcall int
ch_r_msg(struct ch *ch, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = ch_data(ch, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = ch_r_proto(ch, q, mp);
		break;
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		err = m_r_ioctl(q, mp);
		break;
	case M_FLUSH:
		err = m_flush(q, mp);
		break;
	case M_ERROR:
	case M_HANGUP:
		err = m_error(q, mp);
		break;
	default:
		err = m_other(q, mp);
		break;
	}
	return (err);
}

/**
 * m_w_msg: - process a write queue message
 * @q: active queue (upper write queue)
 * @mp: the message to process
 */
static inline fastcall int
m_w_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;
	struct ch *ch;

	switch ((type = DB_TYPE(mp))) {
	case M_DATA:
	case M_PROTO:
	case M_IOCTL:
		/* always queue for speed */
		err = -EAGAIN;
		break;
	case M_HPDATA:
	case M_PCPROTO:
	case M_IOCDATA:
		if (likely(!!(ch = (struct ch *) mi_trylock(q)))) {
			switch (type) {
			case M_HPDATA:
				err = ch_data(ch, q, mp);
				break;
			case M_PCPROTO:
				err = ch_w_proto(ch, q, mp);
				break;
			case M_IOCDATA:
				err = ch_w_ioctl(ch, q, mp);
				break;
			}
			mi_unlock((caddr_t) ch);
		} else
			err = -EDEADLK;
		break;
	case M_FLUSH:
		err = m_flush(q, mp);
		break;
	default:
		err = m_other(q, mp);
		break;
	}
	return (err);
}

/**
 * m_r_msg: - process a read queue message
 * @q: active queue (lower read queue)
 * @mp: the message to process
 */
static inline fastcall int
m_r_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;
	struct ch *ch;

	switch ((type = DB_TYPE(mp))) {
	case M_DATA:
	case M_PROTO:
		/* always queue for speed */
		err = -EAGAIN;
		break;
	case M_HPDATA:
	case M_PCPROTO:
		if (likely(!!(ch = (struct ch *) mi_trylock(q)))) {
			switch (type) {
			case M_HPDATA:
				err = ch_data(ch, q, mp);
				break;
			case M_PCPROTO:
				err = ch_r_proto(ch, q, mp);
				break;
			}
			mi_unlock((caddr_t) ch);
		} else
			err = -EDEADLK;
		break;
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		err = m_r_ioctl(q, mp);
		break;
	case M_FLUSH:
		err = m_flush(q, mp);
		break;
	case M_ERROR:
	case M_HANGUP:
		err = m_error(q, mp);
		break;
	default:
		err = m_other(q, mp);
		break;
	}
	return (err);
}

/*
 * STREAMS PUT AND SERVICE PROCEDURES
 */

/**
 * ch_wput: - write put procedure
 * @q: active queue (upper write queue)
 * @mp: message to put
 */
static streamscall int
ch_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (!q->q_first || (q->q_flags & QSVCBUSY))) || m_w_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * ch_wsrv: - write service procedure
 * @q: active queue (upper write queue)
 */
static streamscall int
ch_wsrv(queue_t *q)
{
	struct ch *ch;
	mblk_t *mp;

	if (likely(!!(ch = (struct ch *) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(ch_w_msg(ch, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) ch);
	}
	return (0);
}

/**
 * ch_muxsrv: - multiplex service procedure
 * @q: active queue (upper read or lower write queue)
 */
static streamscall int
ch_muxsrv(queue_t *q)
{
	struct ch *ch, *oth;
	queue_t *iq;

	read_lock(&ch_lock);
	if (likely(!!(ch = CH_PRIV(q))) && likely(!!(oth = ch->oth))
	    && likely(!!(iq = oth->iq)))
		qenable(iq);
	read_unlock(&ch_lock);
}

/**
 * ch_rsrv: - read service procedure
 * @q: active queue (lower read queue)
 */
static streamscall int
ch_rsrv(queue_t *q)
{
	struct ch *ch;
	mblk_t *mp;

	if (likely(!!(ch = (struct ch *) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(ch_r_msg(ch, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) ch);
	}
	return (0);
}

/**
 * ch_rput: - read put procedure
 * @q: active queue (lower read queue)
 * @mp: message to put
 */
static streamscall int
ch_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (!q->q_first || (q->q_flags & QSVCBUSY))) || m_r_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/*
 * STREAMS OPEN AND CLOSE ROUTINES
 */

/**
 * ch_qopen: - queue open routine
 * @q: read queue of opened queue pair
 * @devp: device number pointer
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @crp: credentials of opening process
 */
static streamscall int
ch_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	minor_t cminor = getminor(*devp);
	major_t cmajor = getmajor(*devp);
	struct ch *ch;
	int err;

	if (q->q_ptr)
		return (0);
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag = CLONEOPEN) {
		/* start half way through minor device range for clone devices */
		cminor = (getminor(makedevice(0, 0xffffffff)) >> 1) + 1;
		*devp = makedevice(cmajor, cminor);
	}
	write_lock(&ch_lock);
	if ((err = mi_open_comm(&ch_opens, sizeof(*ch), q, devp, oflags, sflag, crp))) {
		write_unlock(&ch_lock);
		return (err);
	}
	ch = CH_PRIV(q);
	/* FIXME: initialize CH structure */
	bzero(ch, sizeof(*ch));
	ch->oq = RD(q);
	ch->iq = WR(q);
	ch->oth = NULL;
	ch->mon = NULL;
	ch->state.state = CHS_DETACHED;
	ch->oldstate.state = CHS_DETACHED;
	ch->parm.circuit.cp_type = CH_PARMS_CIRCUIT;
	ch->parm.circuit.cp_encoding = CH_ENCODING_NONE;
	ch->parm.circuit.cp_block_size = 8;
	ch->parm.circuit.cp_samples = 8;
	ch->parm.circuit.cp_sample_size = 8;
	ch->parm.circuit.cp_rate = 8000;
	ch->parm.circuit.cp_tx_channels = 1;
	ch->parm.circuit.cp_rx_channels = 1;
	ch->parm.circuit.cp_opt_flags = CH_PARM_OPT_CLRCH;
	ch->rerror = 0;
	ch->werror = 0;
	ch->info.ch_primitive = 0;
	ch->info.ch_addr_length = 0;
	ch->info.ch_addr_offset = 0;
	ch->info.ch_parm_length = sizeof(ch->parm.circuit);
	ch->info.ch_parm_offset = 0;
	ch->info.ch_prov_flags = 0;
	ch->info.ch_prov_class = CH_CIRCUIT;
	ch->info.ch_style = CH_STYLE2;
	ch->info.ch_version = CH_VERSION;
	ch->info.ch_state = CHS_DETACHED;
	return (0);
}

/**
 * ch_qclose: - queue close routine
 * @q: read queue of closing queue pair
 * @oflags: open flags
 * @crp: credentials of closing process
 */
static streamscall int
ch_qclose(queue_t *q, int oflags, cred_t *crp)
{
	write_lock(&ch_lock);
	qprocsoff(q);
	mi_close_comm(&ch_opens, q);
	write_unlock(&ch_lock);
	return (0);
}

/*
 * STREAMS DEFINITIONS
 */

STATIC struct module_info ch_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ch_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit ch_wrinit = {
	.qi_putp = ch_wput,
	.qi_srvp = ch_wsrv,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_mstat,
};

static struct qinit ch_rdinit = {
	.qi_putp = NULL,
	.qi_srvp = ch_muxsrv,
	.qi_qopen = ch_qopen,
	.qi_qclose = ch_qclose,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_mstat,
};

static struct qinit ch_muxwinit = {
	.qi_putp = NULL,
	.qi_srvp = ch_muxsrv,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_mstat,
};

static struct qinit ch_muxrinit = {
	.qi_putp = ch_rput,
	.qi_srvp = ch_rsrv,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_mstat,
};

static struct streamtab ch_muxinfo = {
	.st_rdinit = &ch_rdinit,
	.st_wrinit = &ch_wrinit,
	.st_muxrinit = &ch_muxrinit,
	.st_muxwinit = &ch_muxwinit,
};

static struct cdevsw ch_dev = {
	.d_name = DRV_NAME,
	.d_str = &ch_muxinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

/**
 * ch_muxinit: - initialize CH-MUX kernel module
 */
static __init int
ch_muxinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = register_strdev(&ch_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register device %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

/**
 * ch_muxterminate: - terminate CH-MUX kernel module
 */
static __exit void
ch_muxterminate(void)
{
	int err;

	if ((err = unregsiter_strdev(&ch_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister device %d, err = %d\n", DRV_NAME, major,
			-err);
		return;
	}
	major = 0;
	return;
}

module_init(ch_muxinit);
module_exit(ch_muxterminate);
