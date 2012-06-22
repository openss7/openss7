/*****************************************************************************

 @(#) $RCSfile: mxmux.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:35 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-09-02 08:46:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mxmux.c,v $
 Revision 1.1.2.3  2011-09-02 08:46:35  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.2  2010-11-28 14:21:34  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:20:50  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: mxmux.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:35 $";

/*
 * This is an MX/CH multiplexing driver.  Its purpose is to allow a single device,
 * /dev/streams/mxmux to represent all multiplex device streams in the system.
 * The MX SNMP agent opens the appropriate device drivers and links (I_PLINKs)
 * them under this multiplexer to form a single view of the device drivers.  It is
 * then possible to open an MX upper stream on this multiplexer and link it, for
 * example under the MG multiplexer.  Also, it is possible to open a CH upper
 * streams on this multiplexer, push appropriate modules, an use the CH stream as,
 * for example, a data link.  Also, an upper CH stream can be linked under the
 * CHMUX multiplexing driver to provide a single device representation of all CH
 * streams in the system.  Another use for an upper MX stream is to link the
 * Stream beneath the zaptel driver for use by zaptel driver applications
 * (however, it is better to direcly link driver MX streams beneath the zaptel
 * driver to minimize latency).
 *
 * Using the MX pipe module, it is possible to open a STREAMS pipe, push the
 * mx-pmod pipe module on one end of the pipe and then link both ends beneath this
 * multiplexer.  This permits testing of the multiplexer as well as providing an
 * internal switching matrix capability.
 *
 * The driver also supports pseudo-digital cross connect between lower MX streams.
 *
 * Lower MX Streams are always treated as Style 1 Streams.  If the lower Stream is
 * in fact a Style 2 MX Stream, it must be attached to a PPA before it is linked
 * beneath the multiplexing driver.  Clone upper CH Streams and MX Streams are
 * always treated as Style 2 streams.  PPA addressing for the upper CH and MX
 * Streams is peformed by using a globally assigned PPA or CLEI that is assigned
 * to a lower Stream at the time that it is linked beneath the multiplexing
 * driver.  Addresses for CH streams are derived from the PPA and CLEI for lower
 * Streams and the slot map used in the enable and connect primitives is used to
 * select the channel(s) from the lower MX stream.  Monitoring can be
 * accomplished, even for pseduo-digital cross-connected channels, by enabling and
 * connecting the upper CH stream in monitoring mode.
 *
 * Whether an upper stream is a CH stream or an MX stream is selected using the
 * minor device number of the opened Stream.  Upper MX streams can be clone
 * streams or non-clone streams.  Clone streams are Style 2 MX streams that can be
 * attached to any available PPA.  Non-clone streams have a minor device number
 * that map to a global PPA and CLEI of a lower Stream (the mapping is assigned
 * when the stream is I_PLINKED beneath the multiplexing driver).  These are Style
 * 1 MX streams.  Upper CH streams are always clone streams and are always Style 2
 * CH device streams.
 *
 */

#include <ss7/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>

#define MX_MUX_DESCRIP		"MX/CH MULTIPLEX (MX-MUX) STREAMS MULTIPLEXING DRIVER."
#define MX_MUX_REVISION		"OpenSS7 $RCSfile: mxmux.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:35 $"
#define MX_MUX_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define MX_MUX_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MX_MUX_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MX_MUX_LICENSE		"GPL"
#define MX_MUX_BANNER		MX_MUX_DESCRIP		"\n" \
				MX_MUX_REVISION		"\n" \
				MX_MUX_COPYRIGHT	"\n" \
				MX_MUX_DEVICE		"\n" \
				MX_MUX_CONTACT		"\n"
#define MX_MUX_SPLASH		MX_MUX_DESCRIP		" - " \
				MX_MUX_REVISION

#if defined LINUX

MODULE_AUTHOR(MX_MUX_CONTACT);
MODULE_DESCRIPTION(MX_MUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MX_MUX_DEVICE);
#if defined MODULE_LICENSE
MODULE_LICENSE(MX_MUX_LICENSE);
#endif				/* defined MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mx-mux");
#endif				/* defined MODULE_ALIAS */
#if defined MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* defined MODULE_VERSION */

#endif				/* defined LINUX */

modID_t ch_modid = CONFIG_STREAMS_CH_MUX_MODID;
major_t ch_major = CONFIG_STREAMS_CH_MUX_MAJOR;
modID_t mx_modid = CONFIG_STREAMS_MX_MUX_MODID;
major_t mx_major = CONFIG_STREAMS_MX_MUX_MAJOR;

#if defined LINUX

#if defined module_param
module_param(ch_modid, ushort, 0444);
module_param(ch_major, uint, 0444);
module_param(mx_modid, ushort, 0444);
module_param(mx_major, uint, 0444);
#else				/* defined module_param */
MODULE_PARM(ch_modid, "h");
MODULE_PARM(ch_major, "d");
MODULE_PARM(mx_modid, "h");
MODULE_PARM(mx_major, "d");
#endif				/* defined module_param */
MODULE_PARM_DESC(ch_modid, "Module id number for STREAMS CH-MUX driver (0 for allocation).");
MODULE_PARM_DESC(ch_major, "Major device number for STREAMS CH-MUX driver (0 for allocation).");
MODULE_PARM_DESC(mx_modid, "Module id number for STREAMS MX-MUX driver (0 for allocation).");
MODULE_PARM_DESC(mx_major, "Major device number for STREAMS MX-MUX driver (0 for allocation).");

#if defined MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_MX_MUX_MAJOR));
MODULE_ALIAS("/dev/mx");
MODULE_ALIAS("/dev/ch");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_MX_MUX_MAJOR));
MODULE_ALIAS("/dev/streams/mx");
MODULE_ALIAS("/dev/streams/mx/*");
MODULE_ALIAS("/dev/streams/mx/mx");
MODULE_ALIAS("/dev/streams/mx/ch");
MODULE_ALIAS("/dev/streams/clone/mx");
#endif				/* defined MODULE_ALIAS */

#endif				/* defined LINUX */

/* Private Structures. */
struct lk;
struct mx;
struct ch;

struct lk {
	struct mx *mx;			/* upper MX structures */
	struct ch *ch;			/* upper CH structures */
	queue_t *iq;			/* RD queue */
	queue_t *oq;			/* WR queue */
};

struct mx {
	struct lk *lk;			/* lower (linked) structure */
	struct mx *next;		/* next structure belonging to same lower structure */
	struct mx **prev;		/* prev structure belonging to same lower structure */
	queue_t *iq;			/* WR queue */
	queue_t *oq;			/* RD queue */
};

struct ch {
	struct lk *lk;			/* lower (linked) structure */
	struct ch *next;		/* next structure belonging to same lower structure */
	struct ch **prev;		/* prev structure belonging to same lower structure */
	queue_t *iq;			/* WR queue */
	queue_t *oq;			/* RD queue */
};

static kmem_cachep_t mx_lk_cachep = NULL;
static kmem_cachep_t mx_mx_cachep = NULL;
static kmem_cachep_t mx_ch_cachep = NULL;

#if	defined DEFINE_RWLOCK
static DEFINE_RWLOCK(mx_lock);
#elif	defined __RW_LOCK_UNLOCKED
static rwlock_t mx_lock = __RW_LOCK_UNLOCKED(mx_lock);
#elif	defined RW_LOCK_UNLOCKED
static rwlock_t mx_lock = RW_LOCK_UNLOCKED;
#else
#error cannot initialize read-write locks
#endif
static caddr_t mx_opens = NULL;		/* list of opened STREAMS */
static caddr_t mx_links = NULL;		/* list of linked STREAMS */

static int
__lk_r_data(struct lk *lk, queue_t *q, mblk_t *mp)
{
}
static int
__ch_w_data(struct ch *ch, queue_t *q, mblk_t *mp)
{
}
static int
__mx_w_data(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

static int
__lk_r_proto(struct lk *lk, queue_t *q, mblk_t *mp)
{
}
static int
__ch_w_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong oldstate = ch_get_state(ch);
	ch_ulong prim;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		freemsg(mp);
		return (0);
	}
	prim = *(typeof(prim) *) mp->b_rptr;

	if (likely(prim == CH_DATA_REQ))
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> %s", mx_primname(prim));
	else
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", mx_primname(prim));

	switch (prim) {
	case CH_INFO_REQ:
		err = ch_info_req(ch, q, mp);
		break;
	case CH_OPTMGMT_REQ:
		err = ch_optmgmt_req(ch, q, mp);
		break;
	case CH_ATTACH_REQ:
		err = ch_attach_req(ch, q, mp);
		break;
	case CH_ENABLE_REQ:
		err = ch_enable_req(ch, q, mp);
		break;
	case CH_CONNECT_REQ:
		err = ch_connect_req(ch, q, mp);
		break;
	case CH_DATA_REQ:
		err = ch_data_req(ch, q, mp);
		break;
	case CH_DISCONNECT_REQ:
		err = ch_disconnect_req(ch, q, mp);
		break;
	case CH_DISABLE_REQ:
		err = ch_disable_req(ch, q, mp);
		break;
	case CH_DETACH_REQ:
		err = ch_detach_req(ch, q, mp);
		break;
	default:
		err = ch_other_req(ch, q, mp);
		break;
	}
	if (err)
		ch_set_state(ch, oldstate);
	return (err);
}
static int
__mx_w_proto(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mx_ulong oldstate = mx_get_state(mx);
	mx_ulong prim;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		freemsg(mp);
		return (0);
	}
	prim = *(typeof(prim) *) mp->b_rptr;

	if (likely(prim == MX_DATA_REQ))
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> %s", ch_primname(prim));
	else
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", ch_primname(prim));

	switch (prim) {
	case MX_INFO_REQ:
		err = mx_info_req(mx, q, mp);
		break;
	case MX_OPTMGMT_REQ:
		err = mx_optmgmt_req(mx, q, mp);
		break;
	case MX_ATTACH_REQ:
		err = mx_attach_req(mx, q, mp);
		break;
	case MX_ENABLE_REQ:
		err = mx_enable_req(mx, q, mp);
		break;
	case MX_CONNECT_REQ:
		err = mx_connect_req(mx, q, mp);
		break;
	case MX_DATA_REQ:
		err = mx_data_req(mx, q, mp);
		break;
	case MX_DISCONNECT_REQ:
		err = mx_disconnect_req(mx, q, mp);
		break;
	case MX_DISABLE_REQ:
		err = mx_disable_req(mx, q, mp);
		break;
	case MX_DETACH_REQ:
		err = mx_detach_req(mx, q, mp);
		break;
	default:
		err = mx_other_req(mx, q, mp);
		break;
	}
	if (err)
		mx_set_state(mx, oldstate);
	return (err);
}

static int
__lk_r_ctl(struct lk *lk, queue_t *q, mblk_t *mp)
{
}
static int
__ch_w_ctl(struct ch *ch, queue_t *q, mblk_t *mp)
{
}
static int
__mx_w_ctl(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

noinline fastcall int
ch_w_flush(queue_t *q, mblk_t *mp)
{
	struct lk *lk;

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	read_lock(&mx_lock);
	if (likely(!!(lk = CH_PRIV(q)->lk))) {
		putnext(lk->oq, mp);
		read_unlock(&mx_lock);
		return (0);
	}
	read_unlock(&mx_lock);
	if (mp->b_rptr[0] & FLUSHR) {
		mp->b_rptr[0] &= ~FLUSHW;
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

noinline fastcall int
mx_w_flush(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	struct lk *lk;

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	if (mp->b_rptr[0] & FLUSHR) {
		mp->b_rptr[0] &= ~FLUSHW;
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

noinline fastcall int
lk_r_flush(queue_t *q, mblk_t *mp)
{
	struct ch *ch;
	struct mx *mx;
	int err = 0;

	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	read_lock(&mx_lock);
	if (likely(!!(lk = (typeof(lk)) mi_trylock(q)))) {
		if (!err)
			for (ch = lk->ch; ch; ch = ch->next)
				if (unlikely((err = m_flush(ch->oq, q, mp,
							    mp->b_rptr[0], mp->b_rptr[1]))))
					break;
		if (!err)
			for (mx = lk->mx; mx; mx = mx->next)
				if (unlikely((err = m_flush(mx->oq, q, mp,
							    mp->b_rptr[0], mp->b_rptr[1]))))
					break;
		mi_unlock((caddr_t) lk);
		if (!err)
			freemsg(mp);
		read_unlock(&mx_lock);
		return (err);
	}
	read_unlock(&mx_lock);
	mp->b_rptr[0] &= ~FLUSHR;
	if (mp->b_rptr[0] & FLUSHW) {
		if (!(mp->b_flag & MSGNOLOOP)) {
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}

static int
ch_wsrv_msg(struct ch *ch, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
		return __ch_w_data(ch, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __ch_w_proto(ch, q, mp);
	case M_CTL:
	case M_PCCTL:
		return __ch_w_ctl(ch, q, mp);
	case M_IOCTL:
		return __ch_w_ioctl(ch, q, mp);
	case M_IOCDATA:
		return __ch_w_iocdata(ch, q, mp);
	default:
	}
}
static int
ch_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_IOCTL:
	case M_IOCDATA:
	{
		struct ch *ch;
		int err = -EAGAIN;

		read_lock(&mx_lock);
		if ((ch = (struct ch *) mi_trylock(q)) != NULL) {
			err = ch_wsrv_msg(ch, q, mp);
			mi_unlock((caddr_t) ch);
		}
		read_unlock(&mx_lock);
		return (err);
	}
	case M_FLUSH:
		return ch_w_flush(q, mp);
	default:
		freemsg(mp);
		return (0);
	}
}

static int
mx_wsrv_msg(struct mx *mx, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
		return __mx_w_data(mx, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __mx_w_proto(mx, q, mp);
	case M_CTL:
	case M_PCCTL:
		return __mx_w_ctl(mx, q, mp);
	case M_IOCTL:
		return __mx_w_ioctl(mx, q, mp);
	case M_IOCDATA:
		return __mx_w_iocdata(mx, q, mp);
	default:
	}
}
static int
mx_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_IOCTL:
	case M_IOCDATA:
	{
		struct mx *mx;
		int err = -EAGAIN;

		read_lock(&mx_lock);
		if ((mx = (struct mx *) mi_trylock(q)) != NULL) {
			err = mx_wsrv_msg(mx, q, mp);
			mi_unlock((caddr_t) mx);
		}
		read_unlock(&mx_lock);
		return (err);
	}
	case M_FLUSH:
		return mx_w_flush(q, mp);
	default:
		freemsg(mp);
		return (0);
	}
}

static int
lk_rsrv_msg(struct lk *lk, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return __lk_r_data(lk, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __lk_r_proto(lk, q, mp);
	case M_CTL:
	case M_PCCTL:
		return __lk_r_ctl(lk, q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return __lk_r_iocack(lk, q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return __lk_r_copy(lk, q, mp);
	default:
	}
}
static int
lk_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
		return (-EAGAIN);	/* always queue for speed */
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	{
		struct lk *lk;
		int err = -EAGAIN;

		read_lock(&mx_lock);
		if ((lk = (struct lk *) mi_trylock(q)) != NULL) {
			err = lk_rsrv_msg(lk, q, mp);
			mi_unlock((caddr_t) lk);
		}
		read_unlock(&mx_lock);
		return (err);
	}
	case M_FLUSH:
		return lk_r_flush(q, mp);
	default:
		freemsg(mp);
		return (0);
	}
}

/*
 * STREAMS PUT AND SERVICE PROCEDURES
 */

static streamscall int
ch_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || ch_wput_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}
static streamscall int
ch_wsrv(queue_t *q)
{
	struct ch *ch;
	mblk_t *mp;

	if (likely(!!(ch = (typeof(ch)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(ch_wsrv_msg(ch, q, mp))) {
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
 * mx_wput: - MX upper write put procedure
 * @q: active queue (upper write queue)
 * @mp: the message to put
 */
static streamscall int
mx_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mx_wput_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * mx_wsrv: - MX upper write service procedure
 * @q: active queue (upper write queue)
 */
static streamscall int
mx_wsrv(queue_t *q)
{
	struct mx *mx;
	mblk_t *mp;

	if (likely(!!(mx = (typeof(mx)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(mx_wsrv_msg(mx, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) mx);
	}
	return (0);
}

/**
 * ch_muxrsrv: - CH multiplex read service procedure
 * @q: active queue (upper read queue)
 *
 * The upper read queues never hold messages.  The service procedure only runs when the queue is
 * back-enabled due to flow control subsiding upstream.  This procedure simply enables all lower
 * read queues feeding beyond this upper read queue.
 */
static streamscall int
ch_muxrsrv(queue_t *q)
{
	struct ch *ch;
	struct lk *lk;

	read_lock(&mx_lock);
	if (likely(!!(ch = (typeof(ch)) mi_trylock(q)))) {
		if (likely(!!(lk = ch->lk)))
			qenable(lk->iq);
		mi_unlock((caddr_t) ch);
	}
	read_unlock(&mx_lock);
	return (0);
}

/**
 * mx_muxrsrv: - MX multiplex read service procedure
 * @q: active queue (upper read queue)
 *
 * The upper read queues never hold messages.  The service procedure only runs when the queue is
 * back-enabled due to flow control subsiding upstream.  This procedure simply enables all lower
 * read queues feeding beyond this upper read queue.
 */
static streamscall int
mx_muxrsrv(queue_t *q)
{
	struct mx *mx;
	struct lk *lk;

	read_lock(&mx_lock);
	if (likely(!!(mx = (typeof(mx)) mi_trylock(q)))) {
		if (likely(!!(lk = mx->lk)))
			qenable(lk->iq);
		mi_unlock((caddr_t) mx);
	}
	read_unlock(&mx_lock);
	return (0);
}

/**
 * lk_muxwsrv: - lower multiplex write service procedure
 * @q: active queue (lower write queue)
 *
 * The lower write queues never hold mesages.  The service procedure only runs when the queue is
 * back-enabled due to flow control subsiding downstream.  This procedure simply enables all upper
 * write qeueus feeding this beyond lower write queue.
 *
 * Streams feeding beyond this lower write queue can either be individual channels each feeding a
 * single channel or group of fractional channels in a multiplex, full or fractional multiplexes
 * that feed channels in a multiplex, or an individual channel feeding an individual channel.  There
 * is only multiple feeding Streams when the feeding Streams consist of fractional multiplexes or
 * channels and the lower Stream is a multiplex.  That is, this is only a fan-in relationship from
 * top to bottom and never a fan-out relationship.  Some feeding Streams (such as interactive voice
 * channels) slip instead of queueing.
 *
 * No, this is wrong, and should not be necessary.  When messages are send to the multiplex beneath,
 * they are esballoc'ed message blocks that reference elastic buffer memory for the lower Stream.
 * When the esballoc'ed message block is freed by the mutlitplex beneath, the free routine steps the
 * elastic buffer memory.  When a data message is received on the receive side, another message
 * block is esballoc'ed and passed down in reply.  Therefore, we can discard this procedure.
 */
static streamscall int
lk_muxwsrv(queue_t *q)
{
	struct lk *lk;
	struct ch *ch;
	struct mx *mx;

	read_lock(&mx_lock);
	if (likely(!!(lk = (typeof(lk)) mi_trylock(q)))) {
		for (ch = lk->ch; ch; ch = ch->next)
			qenable(ch->iq);
		for (mx = lk->mx; mx; mx = mx->next)
			qenable(mx->iq);
		mi_unlock((caddr_t) lk);
	}
	read_unlock(&mx_lock);
	return (0);
}
/**
 * lk_rsrv: - lower read queue service procedure
 * @q: active queue (lower read queue)
 *
 * Elastic buffering on the read side is performed by queuing messages in the lower read queue.
 *
 * Matrix control messages are always processed from the service procedure when not high priority
 * messages.  M_DATA messages containing sample data and M_CTL message containing signalling
 * information are always (at least partially) processed from the service procedure.
 *
 * For each full or fractional upper MX stream, duplicate the message block and pass it up.  For
 * each individual or fractional upper CH stream perform the following procedure:
 *
 * 1. Test the stream for flow control.  If the stream is flow controlled, mark a receive overrun
 *    condition on the Stream and move to the next upper Stream.  Note that the fact that an upper
 *    Stream is flow controlled might be cached in a bit of the structure.  This bit can be cleared
 *    by the upper read service procedure.
 *
 * 2. Allocate a message block for the samples.  If a block cannot be allocated, mark a receive
 *    allocation failure condition and move to the next upper Stream.
 *
 * 3. Fill the message block with samples from the M_DATA frame and pass it upstream.
 *
 * 4. Release the M_DATA message block and return.
 */
static streamscall int
lk_rsrv(queue_t *q)
{
	struct lk *lk;
	mblk_t *mp;

	if (likely(!!(lk = (typeof(lk)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(lk_rsrv_msg(lk, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) lk);
	}
	return (0);
}

/**
 * lk_rput: - lower read queue put procedure
 * @q: active queue (lower read queue)
 * @mp: the message to put
 *
 * Matrix control messages on the lower read queue are processed on the lower read queue as normal,
 * but are always queued when not high priority messages.  However, M_DATA and M_CTL messages are
 * handled separately.  M_DATA messages contain matrix multiplex sample data.  M_CTL messages
 * contain CAS signalling.  Receipt of an M_DATA message causes a block from the transmit elastic
 * buffer to be passed downstream immediately.  This accomplishes synchronization of the transmit
 * data stream to the receive data stream.  M_CTL messages containing CAS signalling are only
 * received when a signalling change has occured and it is the responsibility of the driver below to
 * only displatch M_CTL messages upstream when signalling information has changed.  M_CTL messages
 * are only passed downstream on demand and do no require syncrhonization with the data stream.
 *
 * The procedure upon receipt of the M_DATA message is then as follows: 
 *
 * 1. First, pseudo-digital cross-connect is performed.  Samples from the received M_DATA message
 *    block are written to the current transmit block for the lower MX streams to which they are
 *    cross-connected.  This is performed first in case there is a pseudo-digital cross-connect
 *    within the frame for the same lower multiplex to minimize latency and avoid slippage.
 *
 * 2. If the MX Stream is marked for soft loopback, the samples from the received M_DATA message
 *    block are copied to the current transmit elastic buffer block for this Stream for channels in
 *    the frame marked for loopback.  If there is no frame in the transmit elastic buffer at the
 *    beginning of this procedure, create on by pushing the tail of the elastic buffer.
 *
 * 3. Transmit side processing is then performed.
 *
 *    - if there is no frame in the elastic buffer for transmission (the head and tail are equal),
 *      mark an underrun (slip) and return.
 *
 *    - if there is a frame in the elastic buffer, attempt to esballoc a message block for the
 *      frame.
 *
 *    - if a message block can be allocated, pass it downstream and exit
 *
 *    - if a message block cannot be allocated, mark an underrun (slip) due to message block
 *      allocation failure and return.
 *
 *    - when the free procedure for the message block runs, push the head of the elastic buffer.
 *
 * 4. The received M_DATA message block is always queued for later processing for upper multiplex CH
 *    and full or fractional MX Streams.
 */
static streamscall int
lk_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || lk_rput_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

static streamscall int
lk_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	lk = CH_PRIV(q);
	/* initialize LK structure */
	lk->oq = q;
	lk->iq = RD(q);
	return (0);
}
static streamscall int
lk_qclose(queue_t *q, int oflags, cred_t *credp)
{
}

static streamscall int
ch_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	minor_t cminor = getminor(*devp);
	unsigned long flags;
	struct ch *ch;
	int err;

	if (q->q_ptr)
		return (0);
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN) {
		/* start half way through minor device range for clone devices */
		cminor = (getminor(makedevice(0, 0xffffffff)) >> 1) + 1;
		*devp = makedevice(getmajor(*devp), cminor);
	}
	write_lock_irqsave(&mx_lock, flags);
	if ((err = mi_open_comm(&mx_opens, sizeof(*ch), q, devp, oflags, sflag, credp))) {
		write_unlock_irqrestore(&mx_lock, flags);
		return (err);
	}
	ch = CH_PRIV(q);
	/* initialize CH structure */
	ch->oq = q;
	ch->iq = WR(q);
	return (0);
}
static streamscall int
ch_qclose(queue_t *q, int oflags, cred_t *credp)
{
	unsigned long flags;

	write_lock_irqsave(&mx_lock, flags);
	qprocsoff(q);
	mi_close_comm(&mx_opens, q);
	write_unlock_irqrestore(&mx_lock, flags);
	return (0);
}

static streamscall int
mx_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	minor_t cminor = getminor(*devp);
	unsigned long flags;
	struct mx *mx;
	int err;

	if (q->q_ptr)
		return (0);
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN) {
		/* start half way through minor device range for clone devices */
		cminor = (getminor(makedevice(0, 0xffffffff)) >> 1) + 1;
		*devp = makedevice(getmajor(*devp), cminor);
	}
	write_lock_irqsave(&mx_lock, flags);
	if ((err = mi_open_comm(&mx_opens, sizeof(*mx), q, devp, oflags, sflag, credp))) {
		write_unlock_irqrestore(&mx_lock, flags);
		return (err);
	}
	mx = MX_PRIV(q);
	/* initialize MX structure */
	mx->oq = q;
	mx->iq = WR(q);
	return (0);
}
static streamscall int
mx_qclose(queue_t *q, int oflags, cred_t *credp)
{
	unsigned long flags;

	write_lock_irqsave(&mx_lock, flags);
	qprocsoff(q);
	mi_close_comm(&mx_opens, q);
	write_unlock_irqrestore(&mx_lock, flags);
	return (0);
}

STATIC struct module_info lk_minfo = {
	.mi_idnum = 0,
	.mi_idname = "",
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat lk_mstat __attribute((aligned(SMP_CACHE_BYTES)));

STATIC struct module_info ch_minfo = {
	.mi_idnum = CONFIG_STREAMS_CH_MUX_MODID,
	.mi_idname = CONFIG_STREAMS_CH_MUX_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ch_mstat __attribute((aligned(SMP_CACHE_BYTES)));

STATIC struct module_info mx_minfo = {
	.mi_idnum = CONFIG_STREAMS_MX_MUX_MODID,
	.mi_idname = CONFIG_STREAMS_MX_MUX_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat mx_mstat __attribute((aligned(SMP_CACHE_BYTES)));

static struct qinit lk_muxrinit = {
	.qi_putp = &lk_rput,
	.qi_srvp = &lk_rsrv,
	.qi_minfo = &lk_minfo,
	.qi_mstat = &lk_mstat,
};
static struct qinit lk_muxwinit = {
	.qi_srvp = &lk_muxwsrv,
	.qi_qopen = &lk_qopen,
	.qi_qclose = &lk_qclose,
	.qi_minfo = &lk_minfo,
	.qi_mstat = &lk_mstat,
};

static struct qinit ch_rdinit = {
	.qi_srvp = &ch_muxrsrv,
	.qi_qopen = &ch_qopen,
	.qi_qclose = &ch_qclose,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_mstat,
};
static struct qinit ch_wrinit = {
	.qi_putp = &ch_wput,
	.qi_srvp = &ch_wsrv,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_mstat,
};

static struct qinit mx_rdinit = {
	.qi_srvp = &mx_muxrsrv,
	.qi_qopen = &mx_qopen,
	.qi_qclose = &mx_qclose,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_mstat,
};
static struct qinit mx_wrinit = {
	.qi_putp = &mx_wput,
	.qi_srvp = &mx_wsrv,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_mstat,
};

struct streamtab ch_info = {
	.st_rdinit = &ch_rdinit,
	.st_wrinit = &ch_wrinit,
	.st_muxrinit = &lk_muxrinit,
	.st_muxwinit = &lk_muxwinit,
};
struct streamtab mx_info = {
	.st_rdinit = &mx_rdinit,
	.st_wrinit = &mx_wrinit,
	.st_muxrinit = &lk_muxrinit,
	.st_muxwinit = &lk_muxwinit,
};

static struct cdevsw ch_dev = {
	.d_name = CONFIG_STREAMS_CH_MUX_NAME,
	.d_str = &ch_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static struct cdevsw mx_dev = {
	.d_name = CONFIG_STREAMS_MX_MUX_NAME,
	.d_str = &mx_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static __init int
mxmuxinit(void)
{
	int err;

	if ((err = register_strdev(&ch_cdev, ch_major)) < 0) {
		cmn_err(CE_WARN, "could not register STREAMS CH device, err = %d\n", err);
		return (err);
	}
	if (ch_major == 0)
		ch_major = err;
	if ((err = register_strdev(&mx_cdev, mx_major)) < 0) {
		cmn_err(CE_WARN, "could not register STREAMS MX device, err = %d\n", err);
		unregister_strdev(&ch_dev, ch_major);
		return (err);
	}
	if (mx_major == 0)
		mx_major = err;
	return (0);
}

static __exit void
mxmuxexit(void)
{
	int err;

	if ((err = unregister_strdev(&mx_cdev, mx_major)) < 0)
		cmn_err(CE_WARN, "could not unregister STREAMS CH device, err = %d\n", err);
	if ((err = unregister_strdev(&ch_cdev, ch_major)) < 0)
		cmn_err(CE_WARN, "could not unregister STREAMS MX device, err = %d\n", err);
	return;
}

module_init(mxmuxinit);
module_exit(mxmuxexit);
