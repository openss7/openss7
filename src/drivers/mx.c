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
 *  This is an MX multiplexing driver.  Its purpose is to allow a single device
 *  /dev/streams/matrix to represent all multiplex and channel device streams in
 *  the system.  The MX SNMP agent (mxMIB) configuration script opens the
 *  appropriate device drivers (MX and CH) and links (I_PLINKs) them under this
 *  multiplexer to form a single view of the device driver.  It is then possible to
 *  open a single MX upper stream on this multiplexer and link it, for example,
 *  under the MG multiplexer or the zaptel multiplexing driver.  This is
 *  accomplised by the SNMP managers for those multiplexers.  It is also possible
 *  to open a CH upper stream on this multiplexer and use it directly.
 *
 *  Using the MX or CH pipe modules, it is possible to open a STREAMS pipe, push
 *  the mx-pmod or ch-pmod module on one end of the pipe and then link both ends
 *  beneath this multiplexer.  This permits testing of the multiplexer as well as
 *  providing an internal switching matrix capability.
 *
 *  The driver also provides pseudo-digital cross-connect capabilities within lower
 *  MX streams, between lower MX streams, between lower CH streams, or between CH
 *  streams and MX streams.  This cross-connect can be performed on a dynamic basis
 *  (switching), a semi-permanent basis (DCCS), or a permanent basis (channel
 *  bank).  (Note, however, that pure or pseudo- digital cross connect within an MX
 *  stream, or between MX Streams belonging to the same driver, should still be
 *  performed by the driver itself.)
 *
 *  Lower MX and CH streams are always treated as Style 1 Streams.  That is, if the
 *  lower Stream is in fact a Style 2 Stream, it must be attached to a PPA before
 *  it is linked beneath the multiplexing driver.  Clone upper CH and MX Streams
 *  are always treated as Style 2 streams.  PPA addressing for the upper CH and MX
 *  Streams is performed by using a globally assigned PPA or CLEI that is assigned
 *  to a lower Stream at the time that it is linked beneath the multiplexing
 *  driver.  Addresses for CH Streams are derived from the PPA and CLEI for lower
 *  Streams and the slot map used in the enable and connect primitives is used to
 *  select the channel(s) from the lower MX Stream.  Monitoring can be
 *  accomplished, even for pseudo-digital cross-connect channels, by enabling and
 *  connecting the upper CH stream in monitoring mode.  (This is one case where the
 *  pseudo-digital cross-connect must be performed in this driver instead of by the
 *  lower device driver.)
 *
 *  Whether an upper stream is a CH stream or an MX stream is selected using the
 *  minor device number of the opened Stream.  Upper CH and MX streams can be clone
 *  streams or non-clone streams.  Clone streams are Style 2 streams that can be
 *  attached to any available PPA.  Non-clone streams have a minor device number
 *  that map to a global PPA and CLEI of a lower Stream (the mapping is assigned
 *  when the Stream is I_PLINKED beneath the multiplexing driver).  These are Style
 *  1 MX or CH streams.  Only individual channel or full span CH streams are
 *  available as Style 1 streams.  Fractional CH streams must utility the clone
 *  CH device number and attach to the appropriate fraction of the span.
 *
 *  The primary reason that the MX and CH portions of the switching matrix are
 *  combined into one multiplexing device driver is to support the switching of
 *  multi-rate ISDN and ISUP calls, where multiple 64 kbps channels are ganged
 *  together to provide the multirate connection.
 *
 *  Note that the only time that it is necessary to pass channel data to and from
 *  the driver using an upper CH stream is for data or voice connections that are
 *  terminated on the host.  For example, X.25 connections that are termianted on
 *  this hosts open a CH stream and push the appropraite HDLC and LAPB modules and
 *  link the resulting stream under the X.25 PLP driver.  Similarly, for SS7 MTP2,
 *  this host opens a CH stream and pushes an SDT and SL moulde and links the
 *  resulting stream under the MTP driver.  A voice example is where a CH stream is
 *  opend and attached to a channel and an application plays an announcement or
 *  tone into the connection.
 *
 *  Note that this switching matrix is not capable of performing channel conversion
 *  that is not provided by the lower CH or MX streams themselves.  All
 *  pseudo-digital cross connection is performed clear channel.  At a later date,
 *  one exception to this might be the non-intensive mu-law to A-law conversion.
 *  Also, pseduo-digital cross connect provides only simple connections.
 *
 *  To perform dynamic media conversion, MX and CH upper streams on this driver (or
 *  their corresponding underlying devices) should be linked beneath the MG driver.
 *  However, static media conversion can also be performed using a module or the
 *  device driver directly.  For example, VT over IP or RTP streams can have
 *  conversion modules pushed to provide an MX or CH interface which can then be
 *  linked directly underneath this multiplexing driver.  For example, the RTP
 *  stream than can convert between the codec in use on the RTP stream and either
 *  mu-law or A-law PCM presented to this driver as a CH stream.  To facilitate
 *  this, the upper management CH stream can pass whatever input-output controls it
 *  wishes to a managed lower CH stream.  For example, this could be used to set
 *  the codec used by a lower RTP stream.  The same is true for the MX management
 *  stream.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>

#define MX_DESCRIP	"MX (MX) STREAMS MULTIPLEXING DRIVER."
#define MX_REVISION	"LfS $RCSfile$ $Name$ ($Revision$) $Date$"
#define MX_COPYRIGHT	"Copyright (c) 2008-2009  Monavacon Limited.  All Rights Reserved."
#define MX_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MX_LICENSE	"GPL"
#define MX_BANNER	MX_DESCRIP	"\n" \
			MX_REVISION	"\n" \
			MX_COPYRIGHT	"\n" \
			MX_DEVICE	"\n" \
			MX_CONTACT	"\n"
#define MX_SPLASH	MX_DESCRIP	" - " \
			MX_REVISION

#ifdef LINUX
MODULE_AUTHOR(MX_CONTACT);
MODULE_DESCRIPTION(MX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mx");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif
#endif				/* LINUX */

#define MX_DRV_ID		CONFIG_STREAMS_MX_MODID
#define MX_DRV_NAME		CONFIG_STREAMS_MX_NAME
#define MX_CMAJORS		CONFIG_STREAMS_MX_NMAJORS
#define MX_CMAJOR_0		CONFIG_STREAMS_MX_MAJOR
#define MX_UNITS		CONFIG_STREAMS_MX_NMINORS

modID_t modid = MX_DRV_ID;
major_t major = MX_CMAJOR_0;

#if defined LINUX

#if defined module_param
module_param(modid, ushort, 0444);
module_param(major, uint, 0444);
#else				/* defined module_param */
MODULE_PARM(modid, "h");
MODULE_PARM(major, "d");
#endif				/* defined module_param */
MODULE_PARM_DESC(modid, "Module id number for STREAMS MX driver (0 for allocation).");
MODULE_PARM_DESC(major, "Major device number for STREAMS MX driver (0 for allocation).");

#if defined MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(MX_MODID));
MODULE_ALIAS("streams-driver-mx");
MODULE_ALIAS("streams-major-" __stringify(MX_CMAJOR_0));
MODULE_ALIAS("/dev/streams/mx");
MODULE_ALIAS("/dev/streams/mx/*");
MODULE_ALIAS("/dev/streams/mx/mx");
MODULE_ALIAS("/dev/streams/mx/ch");
MODULE_ALIAS("/dev/streams/clone/mx");
MODULE_ALIAS("char-major-" __stringify(MX_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(MX_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(MX_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/mx");
#endif				/* defined MODULE_ALIAS */

#endif				/* defined LINUX */

#define DRV_ID		MX_DRV_ID
#define DRV_NAME	MX_DRV_NAME

static struct module_stat mx_urstat __attribute__ ((__aligned__(SNMP_CACHE_BYTES)));
static struct module_stat mx_uwstat __attribute__ ((__aligned__(SNMP_CACHE_BYTES)));
static struct module_stat mx_lrstat __attribute__ ((__aligned__(SNMP_CACHE_BYTES)));
static struct module_stat mx_lwstat __attribute__ ((__aligned__(SNMP_CACHE_BYTES)));

/*
 * PRIVATE STRUCTURES
 */

struct mx {
	struct mx *next;		/* list linkage */
	struct mx **prev;		/* list linkage */
	struct mx *other;		/* upper or lower */
	queue_t *iq;			/* input queue */
	queue_t *oq;			/* ouput queue */
	dev_t dev;			/* device number */
	struct MX_info_ack info;
	struct {
		struct MX_parms_circuit circuit;
		struct MX_parms_chanmap chanmap;
	} parm;
};

/* blank structure for use by I_UNLINK/I_PUNLINK */
STATIC struct mx no_mx = {
	.next = NULL,
	.prev = &no_mx.next,
	.other = NULL,
	.oq = NULL,
	.iq = NULL,
	.dev = 0,
};

STATIC struct mx *mx_opens = NULL;
STATIC struct mx *mx_links = NULL;

#define MUX_UP		1
#define MUX_DOWN	2

STATIC rwlock_t mx_lock = RW_LOCK_UNLOCKED;
STATIC rwlock_t mx_spin = RW_LOCK_UNLOCKED;

/*
 * Locking discussion: Most MX STREAMS are persistently in the active state and the state is stable.
 * When states change, they are long-lived (a state change taking microseconds persist for minutes).
 * Therefore, a single lock should be sufficient for all state variables.  Also, reconfiguration of
 * the multiplex is also rare and long-lived.  Therefore, a single lock can protect the entire
 * multiplex, but must be able to be held across procedures.  Both are implemented as reader-writer
 * locks.
 *
 * Any procedure that wishes to walk the multiplex structure must take a read lock on the mx_lock.
 * Any procedure that needs to refer to state must take a read lock on the mx_spin.  Any procedure
 * that needs to alter state needs to take a write lock on mx_spin (and suppress interrupts).  Any
 * procedure that needs to alter 
 */

static noinline fastcall const char *
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
		return ("(unknown)");
	}
}

/**
 *  mx_uwput: - upper stream write put procedure
 *  @q: write queue
 *  @mp: message to put
 */
STATIC streamscall int
mx_uwput(queue_t *q, mblk_t *mp)
{
	struct mx *mx = q->q_ptr, *bot;
	int err;

	trace();
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	{
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;

		trace();
		switch (ioc->iocblk.ioc_cmd) {
		case I_LINK:
		case I_PLINK:
		{
			unsigned long flags;
			struct linkblk *l;

			trace();
			if (!mp->b_cont) {
				ptrace(("Error path taken!\n"));
				goto einval;
			}
			if (!(bot = kmem_alloc(sizeof(*bot), KM_NOSLEEP))) {
				ptrace(("Error path taken!\n"));
				goto enomem;
			}
			l = (typeof(l)) mp->b_cont->b_rptr;

			write_lock_irqsave(&mx_lock, flags);
			bot->next = mx_links;
			bot->prev = &mx_links;
			mx_links = bot;
			bot->dev = l->l_index;
			bot->iq = RD(l->l_qtop);
			bot->oq = l->l_qtop;
			bot->other = NULL;
			noenable(bot->iq);
			l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = bot;
			write_unlock_irqrestore(&mx_lock, flags);

			goto ack;
		}
		}
	}
	case I_UNLINK:
	case I_PUNLINK:
	{
		unsigned long flags;
		struct linkblk *l;

		trace();
		if (!mp->bcont) {
			ptrace(("Error path taken!\n"));
			goto einval;
		}
		l = (typeof(l)) mp->b_cont->b_rptr;

		write_lock_irqsave(&mx_lock, flags);
		for (bot = mx_links; bot; bot = bot->next) {
#ifdef __LP64__
			if (iocp->iocblk.ioc_flag == IOC_ILP32) {
				if ((int32_t) bot->dev == (int32_t) l->l_index)
					break;
			} else
#endif				/* __LP64__ */
			{
				if (bot->dev == l->l_index)
					break;
			}
		}
		if (bot) {
			struct mx *top;

			/* Note that the multiplex driver put and service procedures must be
			   prepared to be invoked even after the M_IOCACK for the I_UNLINK or
			   I_PUNLINK ioctl has been returned.  This is because the setq(9) back to
			   the Stream head procedures is not performed until after the
			   acknowledgement has been received.  We set q->q_ptr to a null multiplex
			   structure to keep the lower Stream functioning until the setq(9) is
			   performed. */
			l->l_qtop->q_ptr = RD(l->l_qtop)->q_ptr = &no_mx;
			if ((*(bot->prev) = bot->next)) {
				bot->next = NULL;
				bot->prev = &bot->next;
			}
			bot->other = NULL;
			kmem_free(bot, sizeof(*bot));
			/* FIXME: with the write lock held, we need to unmap this lower multiplex
			   stream from whatever maps it belongs to */
			/* hang up all upper streams that feed this lower stream */
			for (top = mx_opens; top; top = top->next) {
				if (top->other == bot) {
					putnextctl(top->oq, M_HANGUP);
					top->other = NULL;
				}
			}
		}
		write_unlock_irqrestore(&mx_lock, flags);
		if (!bot)
			goto einval;
		goto ack;
	}
	case MX_MAP:
	{
	}
	case MX_UNMAP:
	{
	}
	default:
	{
		ptrace(("Error path taken!\n"));
		if (mx->other)
			goto passmsg;
	      einval:
		err = -EINVAL;
		goto nak;
	      nak:
		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_error = -err;
		ioc->iocblk.ioc_rval = -1;
		qreply(q, mp);
		return (0);
	      ack:
		mp->b_datap->db_type = M_IOCACK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_error = 0;
		ioc->iocblk.ioc_rval = 0;
		qreply(q, mp);
		return (0);
	}
	case M_FLUSH:
	{
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		read_lock(&mx_lock);
		if (mx->other) {
			queue_t *oq;

			if ((oq = mx->other->oq)) {
				putnext(oq, mp);
				read_unlock(&mx_lock);
				return (0);
			}
		}
		read_unlock(&mx_lock);

		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
			else
				flushq(RD(q), FLUSHDATA);
			mp->b_rptr[0] &= ~FLUSHW;
			qreply(q, mp);
			return (0);
		}
		freemsg(mp);
		break;
	}
	case M_DATA:
	{
		/* If we are in zaptel mode or have been otherwise assigned a default channel,
		   determine the channel number and create a proper MX primitive to the appropriate
		   lower Stream (the one that the channel maps to).  If there is no channel in the
		   channel map or no default has been set (or has been removed) generate an
		   M_ERROR(EPROTO) upstream or an M_HANGUP as appropriate. */
	}
	case M_PROTO:
	case M_PCPROTO:
	{
		/* We always accept MX primitives unless we are in zaptel mode. */
		ulong prim;

		if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
			/* need to error ack */
		}
		switch ((prim = *(ulong *) mp->b_rptr)) {
		case MX_INFO_REQ:
		{
			struct MX_info_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_OPTMGMT_REQ:
		{
			struct MX_optmgmt_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_ATTACH_REQ:
		{
			struct MX_attach_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_ENABLE_REQ:
		{
			struct MX_enable_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_CONNECT_REQ:
		{
			struct MX_connect_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_DATA_REQ:
		{
			struct MX_data_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_DISCONNECT_REQ:
		{
			struct MX_disconnect_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_DISABLE_REQ:
		{
			struct MX_info_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		case MX_DETACH_REQ:
		{
			struct MX_info_req *p = (typeof(p)) mp->b_rptr;

			if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
				/* need to error ack */
			}
		}
		default:
		}
	}
	default:
	      passmsg:
	{
		queue_t *oq = NULL;

		read_lock(&mx_lock);
		if (mx->other)
			oq = mx->other->oq;
		read_unlock(&mx_lock);

		/* if not linked behave like echo driver */
		if (!oq)
			oq = RD(q);

		/* Check the QSVCBUSY flag in MP drivers to avoid missequencing of messages when
		   service procedure is running concurrent with put procedure. */
		if (mp->b_datap->db_type < QPCTL
		    && (q->q_first || (q->q_flag & QSVCBUSY) || !bcanputnext(oq, mp->b_band)))
			putq(q, mp);
		else
			putnext(oq, mp);
		break;
	}
	}
	return (0);
}

/**
 *  mx_lrput: - lower stream read put procedure
 *  @q: write queue
 *  @mp: message to put
 *
 *  For the lower read put procedure, only attempt to process high priority messages and queue the
 *  rest.
 */
STATIC streamscall int
mx_lrput(queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) || mx_r_msg(q, mp)) {
		if (!putq(q, mp)) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 *  mux_lwsrv: - lower stream write service procedure
 *  @q: lower write queue
 *
 *  The lower write service procedure is used merely for backenabling across the multiplexing
 *  driver.  We never put messages to the lower write queue, but put them to the next queue below
 *  the lower write queue.  When bcanput fails on the next queue to the lower write queue, a back
 *  enable will invoke the lower write queue service procedure which can then be used to explicitly
 *  enable the queue(s) feeding the lower write queue.
 */
STATIC streamscall int
mx_lwsrv(queue_t *q)
{
	struct mx *mx = q->q_ptr;
	struct mx *top;

	/* Find the queues feeding this one and enable their input queues. */
	read_lock(&mx_lock);
	for (top = mx_opens; top; top = top->next)
		qenable(mx->iq);
	read_unlock(&mx_lock);
	return (0);
}

/**
 *  mx_uwsrv: - upper stream write service procedure
 *  @q: upper write queue
 *
 *  If the upper write put procedure encounters flow control on the queue beyond the output queue on
 *  the other side of the multiplexer, it places the message back on its queue and waits for the
 *  output queue service procedure to enable it when congestion has cleared.  If the other output
 *  queue is disconnected (not connected yet or has disconnected), the write put procedure will
 *  either generate an M_ERROR (not connected yet) or M_HANGUP (disconnected) message upstream.
 *  Note that M_ERROR at the Stream head will result in an M_FLUSH being sent downstream, but an
 *  M_HANGUP will not.  We therefore, flush the queue if invoked while disconnected.
 *
 *  The upper write service procedure is invoked only by the output queue's service procedure when
 *  the output queue is back enabled.  This causes the backlog to clear.
 */
STATIC streamscall int
mx_uwsrv(queue_t *q)
{
	struct mx *mx;

	read_lock(&mx_lock);
	if ((mx = (struct mx *) mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (__mx_w_msg(mx, q, mp)) {
				if (!putbq(q, mp)) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) mx);
	}
	read_unlock(&mx_lock);
}

/**
 * mx_info_req - issup MX_INFO_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_info_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_optmgmt_req - issup MX_OPTMGMT_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_attach_req - issup MX_ATTACH_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_attach_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_enable_req - issup MX_ENABLE_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_enable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_connect_req - issup MX_CONNECT_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_connect_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_data_req - issup MX_DATA_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_data_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_disconnect_req - issup MX_DISCONNECT_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_disconnect_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_disable_req - issup MX_DISABLE_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_disable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_detach_req - issup MX_DETACH_REQ downstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_detach_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_info_ack - issue MX_INFO_ACK upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_info_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	struct MX_parms_circuit *c;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + sizeof(*c), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		p->mx_addr_length = 0;
		p->mx_addr_offset = 0;
		p->mx_parm_length = sizeof(*c);
		p->mx_parm_offset = sizeof(*p);
		p->mx_prov_flags = 0;
		p->mx_prov_class = MX_CIRCUIT;
		p->mx_style = MX_STYLE2;
		p->mx_version = MX_VERSION_1_1;
		p->mx_state = mx_get_state(mx);
		mp->b_wptr += sizeof(*p);
		c = (typeof(c)) mp->b_wptr;
		*c = mx->parm.circuit;
		mp->b_wptr += sizeof(*c);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_INFO_ACK");
		freemsg(msg);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * mx_optmgmt_ack - issue MX_OPTMGMT_ACK upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_ok_ack - issue MX_OK_ACK upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static inline fastcall int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim)
{
	struct MX_ok_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = M_OK_ACK;
		p->mx_correct_prim = prim;
		p->mx_state = mx_get_state(mx);
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_OK_ACK");
		putnext(mx->oq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_error_ack - issue MX_ERROR_ACK upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @errno: positive type or negative UNIX type
 * @func: function in which error was detected.
 */
static inline fastcall int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim, mx_long errno, const char *func)
{
	struct MX_error_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ERROR_ACK;
		p->mx_error_primitive = prim;
		p->mx_error_type = (errno > 0) ? errno : 0;
		p->mx_unix_error = (errno < 0) ? -errno : 0;
		p->mx_state = mx_get_state(mx);
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, STRLOGNO, SL_TRACE, "ERROR: %s, %s", mx_errdesc(errno),
			  func);
		mi_strlog(q, STRLOGTX, STRLOGTX, "<- MX_ERROR_ACK");
		putnext(mx->oq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_error_reply - reply with error when necessary
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @errno: positive type or negative UNIX type
 * @func: function in which error was detected
 */
noinline fastcall __unlikely int
mx_error_reply(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim, mx_long errno,
	       const char *func)
{
	switch (errno) {
	case 0:
		freemsg(msg);
		return (0);
	case -ENOBUFS:
	case -EAGAIN:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
		return (errno);
	}
	return mx_error_ack(mx, q, msg, prim, errno, func);
}

/**
 * mx_attach_ack - issue an MX_OK_ACK in response to an MX_ATTACH_REQ
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @ppa: PPA to which attach was successful
 */
static inline fastcall int
mx_attach_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong ppa)
{
	struct MX_ok_ack *p;
	mblk_t *mp;
	int err;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = MX_ATTACH_REQ;
		p->mx_state = MXS_ATTACHED;
		mp->b_wptr += sizeof(*p);
		if ((err = mx_attach(q, ppa, MX_STYLE2))) {
			freemsg(mp);
			return mx_error_reply(mx, q, msg, MX_ATTACH_REQ, err, __FUNCTION__);
		}
		if (mx_get_state(mx) == MXS_ATTACHED) {
			/* acknowledge immediately */
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_OK_ACK");
			freemsg(msg);
			putnext(mx->oq, mp);
			return (0);
		}
		/* defer acknowledgement until received by lower multiplex */
		freeb(mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_enable_con - issue MX_ENABLE_CON upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_enable_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_con *p;
	mblk_t *mp;
	int err;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primtiive = MX_ENABLE_CON;
		mp->b_wptr += sizeof(p);
		if ((err = mx_enable(mx, q))) {
			freeb(mp);
			return (err);
		}
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_ENABLE_CON");
		freemsg(msg);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_enable_ack_con - issue MX_OK_ACK and MX_ENABLE_CON upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_enable_ack_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	if (mx_get_state(mx) == MXS_WACK_EREQ) {
		if ((err = mx_ok_ack(mx, q, NULL)))
			return (err);
		mx_set_state(mx, MXS_WCON_EREQ);
		mx_save_state(mx);
	}
	return mx_enable_con(mx, q, msg);
}

/**
 * mx_connect_con - issue MX_CONNECT_CON upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_connect_con(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_connect_con *p;
	mblk_t *mp;
	int err;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_CONNECT_CON;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		if ((err = mx_connect(mx, flags, slot))) {
			freeb(mp);
			return (err);
		}
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_CONNECT_CON");
		freemsg(msg);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_connect_ack_con - issue MX_OK_ACK and MX_CONNECT_CON upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_connect_ack_con(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	int err;

	if (mx_get_state(mx) == MXS_WACK_CREQ) {
		if ((err = mx_ok_ack(mx, q, NULL)))
			return (err);
		mx_set_state(mx, MXS_WCON_CREQ);
		mx_save_state(mx);
	}
	return mx_connect_con(mx, q, msg, flags, slot);
}

/**
 * mx_data_ind - issue MX_DATA_IND upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_data_ind(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/**
 * mx_disconnect_ind - issue MX_DISCONNECT_IND upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @flags: disconnect flags
 * @slot: disconnect slot
 * @cause: disconnect cause
 */
static inline fastcall int
mx_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot,
		  mx_ulong cause)
{
	struct MX_disconnect_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_IND;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		p->mx_cause = cause;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DISCONNECT_IND");
		freemsg(msg);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_con - issue MX_DISCONNECT_CON upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @flags: disconnect flags
 * @slot: disconnect slot
 */
static inline fastcall int
mx_disconnect_con(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_disconnect_con *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_CON;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DISCONNECT_CON");
		freemsg(msg);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_ack_con - issue MX_OK_ACK and MX_DISCONNECT_CON upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @flags: disconnect flags
 * @slot: disconnect slot
 */
static inline fastcall int
mx_disconnect_ack_con(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	int err;

	if (mx_get_state(mx) == MXS_WACK_DREQ) {
		if ((err = mx_ok_ack(mx, q, NULL, MX_DISCONNECT_REQ)))
			return (err);
		mx_set_state(mx, MXS_WCON_DREQ);
		mx_save_state(mx);
	}
	return mx_disconnect_con(mx, q, msg, flags, slot);
}

/**
 * mx_disable_ind - issue MX_DISABLE_IND upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_disable_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong cause)
{
	struct MX_disable_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_IND;
		p->mx_cause = cause;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, MXS_ATTACHED);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DISABLE_IND");
		freemsg(msg);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_con - issue MX_DISABLE_CON upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_disable_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_con *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_CON;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, MXS_ATTACHED);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_DISABLE_CON");
		freemsg(msg);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_ack_con - issue MX_OK_ACK and MX_DISABLE_CON upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_disable_ack_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	int err;

	if (mx_get_state(mx) == MXS_WACK_RREQ) {
		if ((err = mx_ok_ack(mx, q, NULL, MX_DISABLE_REQ)))
			return (err);
		mx_set_state(mx, MXS_WCON_RREQ);
		mx_save_state(mx);
	}
	return mx_disable_con(mx, q, msg);
}

/**
 * mx_detach_ack - issue MX_OK_ACK in reply to MX_DETACH_REQ
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
noinline fastcall int
mx_detach_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_ok_ack *p;
	mblk_t *mp;
	int err;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = MX_DETACH_REQ;
		p->mx_state = mx_set_state(mx, MXS_DETACHED);
		mp->b_wptr += sizeof(*p);
		if ((err = mx_detach(mx, q))) {
			freeb(mp);
			return (err);
		}
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_OK_ACK");
		freemsg(msg);
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_event_ind - issue MX_EVENT_IND upstream
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mx_event_ind(struct mx *mx, queue_t *q, mblk_t *msg)
{
}

/*
 * ====================================
 * ====================================
 * ====================================
 * ====================================
 */

/**
 * mx_attach: - attach an MX stream
 * @top: upper private structure (locked)
 * @q: active queue
 * @ppa: PPA to which to attach
 * @style: resulting style
 *
 * When an MX stream attaches to a ppa, we need to locate the lower stream associated with the PPA
 * (which has already been checked by calling routines) and attach the MX structure.  Also, we need
 * to map out the default channel map for the MX stream.
 *
 * This function relies on the following characteristics.  We don't care about the link id for MX
 * streams linked under the multiplexing driver.  What we care about is the major and minor device
 * numbers.  The PPA is encoded as a device number where the major device number is that of the
 * lower Stream to which to attach.  The minor device number specifies the span.  The logic is as
 * follows: For each lower stream with a matching major device number, if the stream is a Style 1
 * stream or an attached Style 2 stream and the span matches the PPA specified, use that stream.  If
 * this first search fails, for each lower stream with a matching major device number that is a
 * Style 2 stream, issue an attach request for the specified span on that Stream.  If neither is
 * found, return an MXBADPPA error.  If either is found, connect the upper multiplex mx structure to
 * the lower multiplex mx structure.  In the former case, the state is set to MXS_ATTACHED so that
 * the caller knows to acknowledge the attach request immediately.  In the later case, the state is
 * left at MXS_WACK_AREQ so that the caller knows to wait for the lower multiplex to positively or
 * negatively acknowledge the attach request.
 *
 * Note that this procedure must be invoked with a write lock held on the mx_lock and interrupts
 * suppressed so that it may alter the multiplex configuration.
 */
noinline fastcall __unlikely int
mx_attach(struct mx *top, queue_t *q, mx_ulong ppa, mx_ulong style)
{
	unsigned long flags;
	struct mx *bot;
	int err;

	write_lock_irqsave(&mx_lock, flags);
	for (bot = (struct mx *) mi_first_ptr(&mx_links); bot; bot = mi_next_ptr(&mx_links)) {
		if (mx_get_state(bot) != MXS_ATTACHED)
			continue;
		if (bot->addr != ppa)
			continue;
		if (bot->other != NULL)
			continue;
	}
	if (bot != NULL) {
		mx_set_state(top, MXS_ATTACHED);
		bot->other = top;
		top->other = bot;
		err = 0;
	} else
		err = MXBADADDR;
	write_unlock_irqrestore(&mx_lock, flags);
	return (err);
}

noinline fastcall __unlikely int
mx_detach(struct mx *top, queue_t *q)
{
	unsigned long flags;
	struct mx *bot;
	int err;

	write_lock_irqsave(&mx_lock, flags);
	if ((bot = top->other)) {
		bot->other = NULL;
		top->other = NULL;
		mx_set_state(top, MXS_DETACHED);
		err = 0;
	} else
		err = MXOUTSTATE;
	write_unlock_irqrestore(&mx_lock, flags);
	return (err);
}

/**
 * mx_connect - connect an MX Stream
 * @mx: upper MX private structure
 * @q: active queue
 * @flag: connect flags
 * @slot: connect slot
 *
 * Note that while an MX Stream is enabled, none of the channels that are associated with the MX
 * Stream are necessarily transferring data.  Also, samples are not transferred to an MX Stream that
 * sits in the enabled state.  When the MX Stream moves to the connected state, the stream begins
 * transferring samples.  The first MX Stream to connect that has a particular span and channel in
 * its channel map will cause the transfer of samples for that span and channel to begin.  The last
 * MX Stream to disconnect that has a particular span and channe in its channel map will cause the
 * transfer of samples for that span to cease.
 *
 * The slot number is always zero unless a digital cross-connect is being performed.  WHen a zero
 * slot number connect is performed, every channel in the channel map for the MX Stream is
 * connected.  When the slot number is non-zero, it represents a PPA just as the ppa provided to the
 * attach; however, this ppa specifies the MX stream to which this MX stream will be
 * cross-connected.  This pseudo-digital cross connect is possible between any MX streams.  Although
 * the cross-connect is in effect, the attached channels are still transferred to the MX Stream
 * during the cross-connect and will contain both the receive data and the transmit data as
 * completed by the digital cross-connect.  This permits the MX stream to monitor (tap) the
 * cross-connect and is useful for wire-tapping applications.
 *
 * When performing a (pseudo-)digital cross-connect, it is possible to specify the direction in
 * which the cross-connect is to be performed.  This allows rx and tx path splitting to be performed
 * as well.  When cross-connecting between MX streams, the procedures are kept in synchronization
 * with frames received for the streams by locking across frames; however, avoiding slippage between
 * MX streams is the responsibility of the synchronization model: particularly if any of the
 * channels are used for data.
 *
 * When a digital cross-connect is applied, the upper MX Stream is also linked into the lower MX
 * Stream's cross-connect MX Stream list so that it can properly recalculate cross-connect channel
 * maps.
 */
noinline fastcall int
mx_connect(struct mx *mx, queue_t *q, mx_ulong flags, mx_ulong slot)
{

}

/**
 * mx_daccs: - perform fast pseudo-digital cross-connect
 * @mx: lower private structure
 * @mp: the RX block being processed
 *
 * To avoid slipage between MX streams, a spin lock is taken to protect the state of all TX blocks
 * while performing the cross-connect.  The ramifications of this is that interrupt synchronized
 * cross-connect will be ultra low latency (same cycle 1ms latency) where as non-interrupt
 * synchronized cross-conect could delay one cycle (minimum 1ms, maximum 2ms latency).
 *
 * The cross-connect map is laid out as an array of 32 pointers where the array index is the channel
 * number.  When the pointer is NULL, there is no cross-connect for the channel.  When a pointer is
 * set for the channel, the corresponding bit is set in mx->xmask.  When set, a pointer points to
 * the upper MX structure.
 *
 * Each channel map for an MX stream is 8 samples from each timeslot for 32 channels.  Not all
 * channels are populated.
 */
noinline fastcall __hot void
mx_daccs(struct mx *mx, mblk_t *mp)
{
	if (unlikely(mx->xmask)) {
		register uint32_t *buf = (uint32_t *) mp->b_rptr;
		register uint32_t xmask = mx->xmask;
		register int byte, slot, chan;

		for (xmask = mx->mask, chan = 1; chan < 32; chan++, xmask <<= 1) {
			if (unlikely(xmask & 0x01)) {
				struct mx *xx;

				if ((xx = mx->map[chan])) {
					mblk_t bp;

					if ((bp = xx->tx)) {
						uint32_t *dst;

						dst = (uint32_t *) bp->b_rptr;
					}
				}
			}
		}
	}
}

/*
 * ====================================
 * ====================================
 * ====================================
 * ====================================
 */

/**
 * mxe_info_req - process received MX_INFO_REQ
 * @top: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_INFO_REQ message
 */
static int
mxe_info_req(struct mx *top, queue_t *q, mblk_t *mp)
{
	struct MX_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if ((err = mx_info_ack(top, q, mp)))
		goto error;
	return (0);
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(top, q, mp, MX_INFO_REQ, MXBADPRIM, __FUNCTION__);

}

/**
 * mxe_optmgmt_req - process received MX_OPTMGMT_REQ
 * @top: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_OPTMGMT_REQ message
 */
static int
mxe_optmgmt_req(struct mx *top, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_attach_req - process received MX_ATTACH_REQ
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_ATTACH_REQ message
 *
 * Requests that the stream associated with @q be attached to the specified span.  The specified
 * span must exist, and must be available.
 */
static fastcall int
mxe_attach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_attach_req *p = (typeof(p)) mp->b_rptr;
	uint32_t ppa;
	int err, errno;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx->info.mx_style != MX_STYLE2)
		goto notsupport;
	if (mx_get_state(mx) != MXS_DETACHED)
		goto outstate;
	if (p->mx_addr_length != sizeof(ppa))
		goto badaddr;
	if (!MBLKIN(mp, p->mx_addr_offset, p->mx_addr_length))
		goto protoshort;
	bcopy(mp->b_rptr + p->mx_addr_offset, &ppa, sizeof(ppa));
	if ((err = mx_attach_ack(mx, s, mp, ppa)))
		goto error;
	return (0);
      protoshort:
	err = MXBADPRIM;
	goto error;
      badaddr:
	err = MXBADADDR;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      notsupport:
	err = MXNOTSUPP;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_ATTACH_REQ, err, __FUNCTION__);
}

/**
 * mxe_enable_req - process received MX_ENABLE_REQ
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_ENABLE_REQ message
 */
static int
mxe_enable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_state(mx) != MXS_ATTACHED)
		goto outstate;
	mx_set_state(mx, MXS_WACK_EREQ);
	putnext(mx->other->oq, mp);
	return (0);
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_ENABLE_REQ, err, __FUNCTION__);
}

/**
 * mxe_connect_req - process received MX_CONNECT_REQ
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_CONNECT_REQ message
 */
static int
mxe_connect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_state(mx) != MXS_ENABLED)
		goto outstate;
	/* XXX: maybe we should not check these */
	if (p->mx_conn_flags != 0)
		goto badflag;
	if (p->mx_slot != 0)
		goto badslot;
	putnext(mx->other->oq, mp);
	return (0);
      badslot:
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
	return mx_error_reply(mx, q, mp, MX_CONNECT_REQ, err, __FUNCTION__);
}

/**
 * mxe_data_req - process received MX_DATA_REQ
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_DATA_REQ message
 */
static int
mxe_data_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_disconnect_req - process received MX_DISCONNECT_REQ
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_DISCONNECT_REQ message
 */
static int
mxe_disconnect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_state(mx) != MXS_CONNECTED)
		goto outstate;
	mx_set_state(mx, MXS_WACK_CREQ);
	putnext(mx->other->oq, mp);
	return (0);
      badprim:
	err = MXBADPRIM;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_DISCONNECT_REQ, err, __FUNCTION__);
}

/**
 * mxe_disable_req - process received MX_DISABLE_REQ
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_DISABLE_REQ message
 */
static int
mxe_disable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx_get_state(mx) != MXS_ENABLED)
		goto outstate;
	mx_set_state(mx, MXS_WACK_RREQ);
	putnext(mx->other->oq, mp);
	return (0);
      outstate:
	err = MXOUTSTATE;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, mp, MX_DISABLE_REQ, err, __FUNCTION__);

}

/**
 * mxe_detach_req - process received MX_DETACH_REQ
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the MX_DETACH_REQ message
 */
static int
mxe_detach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_detach_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mx->info.mx_style != MX_STYLE2)
		goto notsupport;
	if (mx_get_state(mx) != MXS_ATTACHED)
		goto outstate;
	mx_set_state(mx, MXS_WACK_UREQ);
	if ((err = mx_detach_ack(mx, q, mp)))
		goto error;
	return (0);
      outstate:
	err = MXOUTSTATE;
	goto error;
      notsupport:
	err = MXNOTSUPP;
	goto error;
      badprim:
	err = MXBADPRIM;
	goto error;
      error:
	return mx_error_reply(mx, q, msg, MX_DETACH_REQ, err, __FUNCTION__);
}

/**
 * mxe_other_req - process other received MX message
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: other MX message
 */
static int
mxe_other_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	return mx_error_ack(mx, q, mp, *(mx_ulong *) mp->b_rptr, MXBADPRIM, __FUNCTION__);
}

/*
 * --------------------------------------------------------------------------
 *
 * Process messages received on the bottom multiplex.
 *
 * --------------------------------------------------------------------------
 */

/**
 * mxe_info_ack - process received MX_INFO_ACK
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_INFO_ACK message
 */
static int
mxe_info_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_info_ack *p = (typeof(p)) mp->b_rptr;
	struct mx *top;

	if (MBLKIN(mp, 0, sizeof(*p))) {
		if (MBLKIN(mp, p->mx_addr_offset, p->mx_addr_length) &&
		    p->mx_addr_length && p->mx_addr_length == sizeof(mx->addr)) {
			bcopy(mp + p->mx_addr_offset, &mx->addr, sizeof(mx->addr));
		}
		if (MBLKIN(mp, p->mx_parm_offset, p->mx_parm_length) &&
		    p->mx_parm_length && p->mx_parm_length == sizeof(mx->parm)) {
			bcopy(mp + p->mx_parm_offset, &mx->parm, sizeof(mx->parm));
		}
		mx->info.mx_prov_flags = p->mx_prov_flags;
		mx->info.mx_prov_class = p->mx_prov_class;
		mx->info.mx_style = p->mx_style;
		mx->info.mx_version = p->mx_version;
		mx->info.mx_state = p->mx_state;
	}
	if (mx->info.mx_primitive == MX_INFO_ACK || mx->other == NULL || mx->other->oq == NULL) {
		freemsg(mp);
		return (0);
	}
	mx->info.mx_primitive = MX_INFO_ACK;
	mi_strlog(q, STRLOGTX, SL_TRACE, "<- MX_INFO_ACK");
	putnext(mx->other->oq, mp);
	return (0);
}

/**
 * mxe_optmgmt_ack - process received MX_OPTMGMT_ACK
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_OPTMGMT_ACK message
 */
static int
mxe_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_ok_ack - process received MX_OK_ACK
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_OK_ACK message
 */
static int
mxe_ok_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_error_ack - process received MX_ERROR_ACK
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_ERROR_ACK message
 */
static int
mxe_error_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_enable_con - process received MX_ENABLE_CON
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_ENABLE_CON message
 */
static int
mxe_enable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_connect_con - process received MX_CONNECT_CON
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_CONNECT_CON message
 */
static int
mxe_connect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_data_ind - process received MX_DATA_IND
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_DATA_IND message
 */
static int
mxe_data_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_disconnect_ind - process received MX_DISCONNECT_IND
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_DISCONNECT_IND message
 */
static int
mxe_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_disconnect_con - process received MX_DISCONNECT_CON
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_DISCONNECT_CON message
 */
static int
mxe_disconnect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_disable_ind - process received MX_DISABLE_IND
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_DISABLE_IND message
 */
static int
mxe_disable_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mxe_disable_con - process received MX_DISABLE_CON
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_DISABLE_CON message
 */
static int
mxe_disable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct mx *top;

	if (!(top = (struct mx *) mi_acquire(mx->other, q)))
		goto ebusy;
	mi_release((caddr_t) top);
      ebusy:
	return (-EBUSY);
}

/**
 * mxe_event_ind - process received MX_EVENT_IND
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the MX_EVENT_IND message
 */
static int
mxe_event_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

static fastcall int
mx_w_data_slow(queue_t *q, mblk_t *mp)
{
}
static inline fastcall int
mx_w_data(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_w_proto(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	int rtn = -EDEADLK;

	if (mx_trylock(mx)) {
		mx_ulong oldstate = mx_get_state(mx);

		switch (*(mx_ulong *) mp->b_rptr) {
		case MX_INFO_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_INFO_REQ");
			rtn = mxe_info_req(mx, q, mp);
			break;
		case MX_OPTMGMT_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_OPTMGMT_REQ");
			rtn = mxe_optmgmt_req(mx, q, mp);
			break;
		case MX_ATTACH_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_ATTACH_REQ");
			rtn = mxe_attach_req(mx, q, mp);
			break;
		case MX_ENABLE_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_ENABLE_REQ");
			rtn = mxe_enable_req(mx, q, mp);
			break;
		case MX_CONNECT_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_CONNECT_REQ");
			rtn = mxe_connect_req(mx, q, mp);
			break;
		case MX_DATA_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_DATA_REQ");
			rtn = mxe_data_req(mx, q, mp);
			break;
		case MX_DISCONNECT_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_DISCONNECT_REQ");
			rtn = mxe_disconnect_req(mx, q, mp);
			break;
		case MX_DISABLE_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_DISABLE_REQ");
			rtn = mxe_disable_req(mx, q, mp);
			break;
		case MX_DETACH_REQ:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_DETACH_REQ");
			rtn = mxe_detach_req(mx, q, mp);
			break;
		default:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "-> MX_????_???");
			rtn = mxe_other_req(mx, q, mp);
			break;
		}
		if (rtn)
			mx_set_state(mx, oldstate);
		mx_unlock(mx);
	}
	return (rtn);
}
static fastcall int
mx_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *dp;

	switch (ioc->ioc_cmd) {
	case MX_IOCGCONFIG:
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(mx->config)))) {
			bcopy(&mx->config, dp->b_rptr, sizeof(mx->config));
			mi_copyout(q, mp);
		}
		break;
	case MX_IOCSCONFIG:
		/* fall through */
	case MX_IOCTCONFIG:
		/* fall through */
	case MX_IOCCCONFIG:
		mi_copyin(q, mp, NULL, sizeof(mx->config));
		break;
	case MX_IOCGSTATEM:
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(mx->statem)))) {
			bcopy(&mx->statem, dp->b_rptr, sizeof(mx->statem));
			mi_copyout(q, mp);
		}
		break;
	case MX_IOCCMRESET:
		/* reset state machine */
		mi_copy_set_rval(mp, 0);
		mi_copy_done(q, mp, 0);
		break;
	case MX_IOCGSTATSP:
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(mx->statsp)))) {
			bcopy(&mx->statsp, dp->b_rptr, sizeof(mx->statsp));
			mi_copyout(q, mp);
		}
		break;
	case MX_IOCSSTATSP:
		mi_copyin(q, mp, NULL, sizeof(mx->statsp));
		break;
	case MX_IOCGSTATS:
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(mx->stats)))) {
			bcopy(&mx->stats, dp->b_rptr, sizeof(mx->stats));
			mi_copyout(q, mp);
		}
		break;
	case MX_IOCCSTATS:
		/* clear statistics */
		bzero(&mx->stats, sizeof(mx->stats));
		mi_copy_set_rval(mp, 0);
		mi_copy_done(q, mp, 0);
		break;
	case MX_IOCGNOTIFY:
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(mx->events)))) {
			bcopy(&mx->events, dp->b_rptr, sizeof(mx->events));
			mi_copyout(q, mp);
		}
		break;
	case MX_IOCSNOTIFY:
		mi_copyin(q, mp, NULL, sizeof(mx->events));
		break;
	case MX_IOCCNOTIFY:
		/* clear events */
		bzero(&mx->events, sizeof(mx->events));
		mi_copy_set_rval(mp, 0);
		mi_copy_done(q, mp, 0);
		break;
	case I_PLINK:
		if (!drv_priv(ioc->ioc_cr)) {
			mi_copy_done(q, mp, EPERM);
			break;
		}
		/* fall through */
	case I_LINK:
		if (!(mx = mi_open_alloc(sizeof(*mx)))) {
			mi_copy_done(q, mp, ENOMEM);
			break;
		}
		if ((err = mi_open_link(&mx_links, (caddr_t) mx, (dev_t *) &l->l_index, 0,
					DRVOPEN, NULL))) {
			mi_copy_done(q, mp, err);
			break;
		}
		mi_attach(RD(l->l_qbot), (caddr_t) mx);
		mx_link(mx);
		break;
	case I_PUNLINK:
		if (!drv_priv(ioc->ioc_cr)) {
			mi_copy_done(q, mp, EPERM);
			break;
		}
		/* fall through */
	case I_UNLINK:
		for (mx = mi_first_ptr(&mx_links); mx && mx->sid != l->l_index;
		     mx = mi_next_ptr(&mx_links, (caddr_t) mx)) ;
		if (!mx) {
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		mx_unlink(mx);
		mi_detach(RD(mx->oq), (caddr_t) mx);
		mi_close_unlink(&mx_links, (caddr_t) mx);
		mi_close_free((caddr_t) mx);
		mi_copy_done(q, mp, 0);
		break;
	}
	return (0);
}
static fastcall int
mx_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *dp;

	switch (ioc->ioc_cmd) {
	case MX_IOCGCONFIG:
	case MX_IOCGSTATEM:
	case MX_IOCGSTATSP:
	case MX_IOCGSTATS:
	case MX_IOCGNOTIFY:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_OUT, 1):
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case MX_IOCSCONFIG:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct mx *mx2;

			if (!(mx2 = mx_find_id(mx, ((struct mx_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (mx2 != mx && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != mx2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &mx2->config, sizeof(mx2->config));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case MX_IOCTCONFIG:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct mx *mx2;

			if (!(mx2 = mx_find_id(mx, ((struct mx_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (mx2 != mx && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != mx2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &mx2->config, sizeof(mx2->config));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case MX_IOCCCONFIG:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct mx *mx2;

			if (!(mx2 = mx_find_id(mx, ((struct mx_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (mx2 != mx && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != mx2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &mx2->config, sizeof(mx2->config));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case MX_IOCCMRESET:
		/* should not happen */
		break;
	case MX_IOCSSTATSP:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct mx *mx2;

			if (!(mx2 = mx_find_id(mx, ((struct mx_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (mx2 != mx && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != mx2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &mx2->statsp, sizeof(mx2->statsp));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case MX_IOCCSTATS:
		/* should not happen */
		break;
	case MX_IOCSNOTIFY:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct mx *mx2;

			if (!(mx2 = mx_find_id(mx, ((struct mx_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (mx2 != mx && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != mx2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &mx2->events, sizeof(mx2->events));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case MX_IOCCNOTIFY:
		/* should not happen */
		break;
	}
	return (0);
}
static fastcall int
mx_w_ctl(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_w_flush(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	queue_t *oq;

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (mx->other && (oq = mx->other->oq)) {
			if (oq->q_flag & QREADR) {
				/* need to reverse sense of flush */
				switch (mp->b_rptr[0] & FLUSHRW) {
				case FLUSHR:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHW;
					break;
				case FLUSHW:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHR;
					break;
				}
			}
			putnext(oq, mp);
			return (0);
		}
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mx->other && (oq = mx->other->oq)) {
			if (oq->q_flag & QREADR) {
				/* need to reverse sense of flush */
				switch (mp->b_rptr[0] & FLUSHRW) {
				case FLUSHR:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHW;
					break;
				case FLUSHW:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHR;
					break;
				}
			}
			putnext(oq, mp);
			return (0);
		}
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(_RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(_RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static fastcall int
mx_w_other(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	queue_t *oq;

	/* pass along with flow control */
	if (mx->other && (oq = mx->other->oq)) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(oq, mp->b_band)) {
			putnext(oq, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);
	return (0);
}

static fastcall int
mx_r_data_slow(queue_t *q, mblk_t *mp)
{
}
static inline fastcall int
mx_r_data(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_r_proto(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	int rtn = -EDEADLK;

	if (mx_trylock(mx)) {
		mx_ulong oldstate = mx_get_state(mx);

		switch (*(mx_ulong *) mp->b_rptr) {
		case MX_INFO_ACK:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_INFO_ACK");
			rtn = mxe_info_ack(mx, q, mp);
			break;
		case MX_OPTMGMT_ACK:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_OPTMGMT_ACK");
			rtn = mxe_optmgmt_ack(mx, q, mp);
			break;
		case MX_OK_ACK:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_OK_ACK");
			rtn = mxe_ok_ack(mx, q, mp);
			break;
		case MX_ERROR_ACK:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_ERROR_ACK");
			rtn = mxe_error_ack(mx, q, mp);
			break;
		case MX_ENABLE_CON:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_ENABLE_CON");
			rtn = mxe_enable_con(mx, q, mp);
			break;
		case MX_CONNECT_CON:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_CONNECT_CON");
			rtn = mxe_connect_con(mx, q, mp);
			break;
		case MX_DATA_IND:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_DATA_IND");
			rtn = mxe_data_ind(mx, q, mp);
			break;
		case MX_DISCONNECT_IND:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_DISCONNECT_IND");
			rtn = mxe_disconnect_ind(mx, q, mp);
			break;
		case MX_DISCONNECT_CON:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_DISCONNECT_CON");
			rtn = mxe_disconnect_con(mx, q, mp);
			break;
		case MX_DISABLE_IND:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_DISABLE_IND");
			rtn = mxe_disable_ind(mx, q, mp);
			break;
		case MX_DISABLE_CON:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_DISABLE_CON");
			rtn = mxe_disable_con(mx, q, mp);
			break;
		case MX_EVENT_IND:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_EVENT_IND");
			rtn = mxe_event_ind(mx, q, mp);
			break;
		default:
			strlog(mx->mid, mx->sid, 0, SL_TRACE, "<- MX_????_???");
			rtn = mxe_other_ind(mx, q, mp);
			break;
		}
		if (rtn)
			mx_set_state(mx, oldstate);
		mx_unlock(mx);
	}
	return (rtn);
}
static fastcall int
mx_r_ioctl(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_r_ctl(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_r_sig(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_r_flush(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	queue_t *oq;

	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (mx->other && mx->other->oq) {
			if (!(oq->q_flag & QREADR)) {
				/* need to reverse sense of flush */
				switch (mp->b_rptr[0] & FLUSHRW) {
				case FLUSHR:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHW;
					break;
				case FLUSHW:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHR;
					break;
				}
			}
			putnext(oq, mp);
			return (0);
		}
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mx->other && (oq = mx->other->oq)) {
			if (!(oq->q_flag & QREADR)) {
				/* need to reverse sense of flush */
				switch (mp->b_rptr[0] & FLUSHRW) {
				case FLUSHR:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHW;
					break;
				case FLUSHW:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHR;
					break;
				}
			}
			putnext(oq, mp);
			return (0);
		}
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(_WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(_WR(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static fastcall int
mx_r_other(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	queue_t *oq;

	/* pass along with flow control */
	if (mx->other && (oq = mx->other->oq)) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(oq, mp->b_band)) {
			putnext(oq, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);
	return (0);
}

static noinline fastcall int
mx_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mx_w_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_w_proto(q, mp);
	case M_IOCTL:
		return mx_w_ioctl(q, mp);
	case M_IOCDATA:
		return mx_w_iocdata(q, mp);
	case M_CTL:
	case M_PCCTL:
		return mx_w_ctl(q, mp);
	case M_FLUSH:
		return mx_w_flush(q, mp);
	}
	return mx_w_other(q, mp);
}
static inline fastcall int
mx_w_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return mx_w_data(q, mp);
	return mx_w_msg_slow(q, mp);
}

static inline fastcall int
mx_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mx_r_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_r_proto(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
	case M_IOCACK:
	case M_IOCNAK:
		return mx_r_ioctl(q, mp);
	case M_CTL:
	case M_PCCTL:
		return mx_r_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mx_r_sig(q, mp);
	case M_FLUSH:
		return mx_r_flush(q, mp);
	}
	return mx_r_other(q, mp);
}
static noinline fastcall int
mx_r_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return mx_r_data(q, mp);
	return mx_r_msg_slow(q, mp);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

static noinline fastcall int
__mx_w_proto(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

static noinline fastcall int
__mx_r_proto(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

static noinline fastcall int
__mx_w_data(struct mx *top, queue_t *q, mblk_t *mp)
{
	struct mx *bot;
	int rtn = 0;

	if (top->other != NULL) {
		if ((bot = mi_acquire(top->other, q))) {
			if (bot->oq != NULL) {
				if (pcmsg(DB_TYPE(mp)) || bcanputnext(bot->oq, mp->b_band))
					putnext(bot->oq, mp);
				else
					rtn = -EBUSY;
			} else
				freemsg(mp);
			mi_release((caddr_t) bot);
		} else
			rtn = -EDEADLK;
	} else
		freemsg(mp);
	return (rtn);
}

static noinline fastcall int
__mx_r_data(struct mx *bot, queue_t *q, mblk_t *mp)
{
	struct mx *top;
	int rtn = 0;

	if (bot->other != NULL) {
		if ((top = (struct mx *) mi_acquire(bot->other, q))) {
			if (top->oq != NULL) {
				if (pcmsg(DB_TYPE(mp)) || bcanputnext(top->oq, mp->b_band))
					putnext(top->oq, mp);
				else
					rtn = -EBUSY;
			} else
				freemsg(mp);
			mi_release((caddr_t) top);
		} else
			rtn = -EDEADLK;
	} else
		freemsg(mp);
	return (rtn);
}
static inline fastcall int
mx_w_data(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	queue_t *oq;
	int rtn;

	read_lock(&mx_lock);
	if ((mx = (struct mx *) mi_trylock(q)) != NULL) {
		rtn = __mx_w_data(mx, q, mp);
		mi_unlock((caddr_t) mx);
	} else
		rtn = -EDEADLK;
	read_unlock(&mx_lock);
	return (rtn);
}
static inline fastcall int
mx_r_data(queue_t *q, mblk_t *mp)
{
	struct mx *top, *bot;
	int rtn;

	read_lock(&mx_lock);
	if ((bot = (struct mx *) mi_trylock(q)) != NULL) {
		if (bot->other != NULL) {
			if ((top = (struct mx *) mi_acquire(bot->other, q))) {
				rtn = __mx_r_data(top, q, mp);
				mi_release((caddr_t) top);
			} else
				rtn = -EDEADLK;
		} else {
			freemsg(mp);
			rtn = 0;
		}
		mi_unlock((caddr_t) bot);
	} else
		rtn = -EDEADLK;
	read_unlock(&mx_lock);
	return (rtn);
}

static noinline fastcall int
mx_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	{
		struct mx *mx;
		queue_t *oq;

		read_lock(&mx_lock);
		if ((mx = MX_PRIV(q)) != NULL)
			if ((oq = mx->other->oq) != NULL)
				putnext(oq, mp);
		read_unlock(&mx_lock);
		if (mx != NULL && oq != NULL)
			return (0);
	}
	mp->b_rptr[0] &= ~FLUSHW;
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static noinline fastcall int
mx_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	{
		struct mx *mx;

		read_lock(&mx_lock);
		if ((mx = MX_PRIV(q)) != NULL)
			if ((oq = mx->other->oq) != NULL)
				putnext(oq, mp);
		read_unlock(&mx_lock);
		if (mx != NULL && oq != NULL)
			return (0);
	}
	mp->b_rptr[0] &= ~FLUSHR;
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		if (mp->b_flag & MSGNOLOOP)
			freemsg(mp);
		else {
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);
		}
		return (0);
	}
	freemsg(mp);
	return (0);
}

static noinline fastcall __unlikely int
__mx_w_other(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_ERROR, "SWERR: %s", __FUNCTION__);
	freemsg(mp);
	return (0);
}
static noinline fastcall __unlikely int
__mx_r_other(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_ERROR, "SWERR: %s", __FUNCTION__);
	freemsg(mp);
	return (0);
}

static noinline fastcall __unlikely int
mx_w_other(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	int rtn;

	read_lock(&mx_lock);
	mx = MX_PRIV(q);
	rtn = __mx_w_other(mx, q, mp);
	read_unlock(&mx_lock);
	return (rtn);
}
static noinline fastcall __unlikely int
mx_r_other(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	int rtn;

	read_lock(&mx_lock);
	mx = MX_PRIV(q);
	rtn = __mx_r_other(mx, q, mp);
	read_unlock(&mx_lock);
	return (rtn);
}

/*
 * --------------------------------------------------------------------------
 *
 * General Message Handling
 *
 * --------------------------------------------------------------------------
 */

/**
 * mx_wsrv_msg - upper write service message
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the message to service
 */
static inline fastcall int
mx_wsrv_msg(struct mx *mx, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return __mx_w_data(mx, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __mx_w_proto(mx, q, mp);
	case M_CTL:
	case M_PCCTL:
		return __mx_w_ctl(mx, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return __mx_w_ioctl(mx, q, mp);
	default:
		return __mx_w_other(mx, q, mp);
	}
}

/**
 * mx_rsrv_msg - lower read service message
 * @mx: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the message to service
 */
static inline fastcall int
mx_rsrv_msg(struct mx *mx, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return __mx_r_data(mx, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __mx_r_proto(mx, q, mp);
	case M_CTL:
	case M_PCCTL:
		return __mx_r_ctl(mx, q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return __mx_r_ioctl(mx, q, mp);
	default:
		return __mx_r_other(mx, q, mp);
	}
}

/**
 * mx_wput_msg_slow - upper write put message (slow)
 * @q: active queue (upper write queue)
 * @mp: the message to put
 */
static noinline fastcall int
mx_wput_msg_slow(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	int rtn;

	read_lock(&mx_lock);
	if ((mx = (struct mx *) mi_trylock(q)) != NULL) {
		rtn = mx_wsrv_msg(mx, q, mp);
		mi_unlock((caddr_t) mx);
	} else
		rtn = -EDEADLK;
	read_unlock(&mx_lock);
	return (rtn);
}

/**
 * mx_rput_msg_slow - lower read put message (slow)
 * @q: active queue (lower read queue)
 * @mp: the message to put
 */
static noinline fastcall int
mx_rput_msg_slow(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	int rtn;

	read_lock(&mx_lock);
	if ((mx = (struct mx *) mi_trylock(q)) != NULL) {
		rtn = mx_rsrv_msg(mx, q, mp);
		mi_unlock((caddr_t) mx);
	} else
		rtn = -EDEADLK;
	read_unlock(&mx_lock);
	return (rtn);
}

/**
 * mx_wput_msg - upper write put message
 * @q: active queue (upper write queue)
 * @mp: the message to put
 */
static inline fastcall int
mx_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return mx_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_IOCTL:
	case M_IOCDATA:
	default:
		return mx_wput_msg_slow(q, mp);
	case M_FLUSH:
		return mx_w_flush(q, mp);
	}
}

/**
 * mx_rput_msg - lower read put message
 * @q: active queue (lower read queue)
 * @mp: the message to put
 */
static inline fastcall int
mx_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return mx_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	default:
		return mx_rput_msg_slow(q, mp);
	case M_FLUSH:
		return mx_r_flush(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * PUT and SERVICE procedures.
 *
 * --------------------------------------------------------------------------
 */

/**
 * mx_uwput - upper write put procedure
 * @q: active queue (upper write queue)
 * @mp: the message to put
 *
 * For the upper write put procedure, only try to process high priority messages immediately and
 * queue the rest.
 */
static streamscall int
mx_uwput(queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) || mx_wput_msg(q, mp)) {
		if (!putq(q, mp)) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * mx_uwsrv - upper write service procedure
 * @q: active queue (upper write queue)
 */
static streamscall int
mx_uwsrv(queue_t *q)
{
	struct mx *mx;

	read_lock(&mx_lock);
	if ((mx = (struct mx *) mi_trylock(q)) == NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (mx_wsrv_msg(mx, q, mp)) {
				if (!putbq(q, mp)) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) mx);
	}
	read_unlock(&mx_lock);
	return (0);
}

/**
 * mx_urput - upper read put procedure
 * @q: active queue (upper read queue)
 * @mp: the message to put
 *
 * The upper read queue never has messages placed on it.  Invoking the upper read put procedure is
 * an error.  Messages should allows be placed on the queue following the upper read queue.
 */
static streamscall int
mx_urput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_ERROR, "software error: %s", __FUNCTION__);
	putnext(q, mp);
	return (0);
}

/**
 * mx_ursrv - upper read service procedure
 * @q: active queue (upper read queue)
 *
 * The upper read queue service procedure is invoked when the read queue of the module above
 * backenables the queue.  The action taken here is to simply determine the queue feeding this queue
 * and explicitly enable it.  Taking a read lock on the mx_lock protects multiplexer linkage and
 * also protects the service procedure from the close procedure.
 */
static streamscall int
mx_ursrv(queue_t *q)
{
	struct mx *mx;

	read_lock(&mx_lock);
	if ((mx = MX_PRIV(q)) && mx->other && mx->other->iq)
		qenable(mx->other->iq);
	read_unlock(&mx_lock);
	return (0);
}

/**
 * mx_lwput - lower write put procedure
 * @q: active queue (lower write queue)
 * @mp: the message to put
 *
 * The lower write queue never has messages placed on it.  Invoking the lower write put procedure is
 * an error.  Messages should always be placed on the queue following the lower write queue.
 */
static streamscall int
mx_lwput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_ERROR, "software error: %s", __FUNCTION__);
	putnext(q, mp);
	return (0);
}

/**
 * mx_lwsrv - lower read queue service procedure
 * @q: the queue to service (lower read queue)
 *
 * The lower write queue service procedure is invoked when the write queue of the module below
 * backenables the queue.  The action taken here is to simply determine the queue feeding this queue
 * and explicitly enable it.  Taking a read lock on the mx_lock protects multiplexer linkage and
 * also protects the service procedure from the unlink procedure.
 */
static streamscall int
mx_lwsrv(queue_t *q)
{
	struct mx *mx;

	read_lock(&mx_lock);
	if ((mx = MX_PRIV(q)) && mx->other && mx->other->iq)
		qenable(mx->other->iq);
	read_unlock(&mx_lock);
	return (0);
}

/**
 * mx_lrput - lower read put procedure
 * @q: active queue (lower read queue)
 *
 * This is a cannonical put procedure.
 */
static streamscall int
mx_lrput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mx_rput_msg(q, mp)) {
		if (!putq(q, mp)) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * mx_lrsrv - lower read service procedure
 * @q: active queue (lower read queue)
 *
 * This is a canonical service procedure with multiplex read locking.
 */
static streamscall int
mx_lrsrv(queue_t *q)
{
	struct mx *bot, *top;

	read_lock(&mx_lock);
	if ((bot = (struct mx *) mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (mx_rsrv_msg(q, mp)) {
				if (!putbq(q, mp)) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) mx);
	}
	read_unlock(&mx_lock);
	return (0);
}

/**
 * mx_qopen - queue open procedure
 * @q: read queue of queue pair
 * @devp: device number pointer
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @credp: credential or opener
 */
static streamscall int
mx_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	minor_t cminor = getminor(*devp);
	unsigned long flags;
	struct mx *mx;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
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
	write_unlock_irqrestore(&mx_lock, flags);
	mx = MX_PRIV(q);
	/* intialize mx structure */
	mx->oq = RD(q);
	mx->iq = WR(q);
	/* parameter defaults for T1 */
	mx->parm.circuit.mp_type = MX_PARMS_CIRCUIT;
	mx->parm.circuit.mp_encoding = MX_ENCODING_NONE;
	mx->parm.circuit.mp_block_size = 32 * 8 * 8;
	mx->parm.circuit.mp_samples = 24 * 8;
	mx->parm.circuit.mp_sample_size = 8;
	mx->parm.circuit.mp_rate = 24 * 8 * 8 * 8;
	mx->parm.circuit.mp_tx_channels = 24;
	mx->parm.circuit.mp_rx_channels = 24;
	mx->parm.circuit.mp_opt_flags = MX_PARM_OPT_CLRCH;
	return (0);
}

/**
 * mx_qclose - queue close procedure
 * @q: read queue of queue pair
 * @oflags: open flags
 * @credp: credential or closer
 */
static streamscall int
mx_qclose(queue_t *q, int oflags, cred_t *credp)
{
	unsigned long flags;
	struct mx *mx;

	qprocsoff(q);
	write_lock_irqsave(&mx_opens, flags);
	mx = mi_acquire_sleep(q->q_ptr, &mx_opens, &mx_lock, &flags);
	mx->iq = mx->oq = NULL;
	mx->other = NULL;
	mi_close_comm(&mx_opens, q);
	mi_release((caddr_t) mx);
	write_unlock_irqrestore(&mx_opens, flags);
	return (0);
}

static struct module_info mx_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct qinit mx_urinit = {
	.qi_putp = mx_urput,
	.qi_srvp = mx_ursrv,
	.qi_qopen = mx_qopen,
	.qi_qclose = mx_qclose,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_urstat,
};

static struct qinit mx_uwinit = {
	.qi_putp = mx_uwput,
	.qi_srvp = mx_uwsrv,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_uwstat,
};

static struct qinit mx_lrinit = {
	.qi_putp = mx_lrput,
	.qi_srvp = mx_lrsrv,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_lrstat,
};

static struct qinit mx_lwinit = {
	.qi_putp = mx_lwput,
	.qi_srvp = mx_lwsrv,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_lwstat,
};

static struct streamstab mxinfo = {
	.st_rdinit = &mx_urinit,
	.st_wrinit = &mx_uwinit,
	.st_muxrinit = &mx_lrinit,
	.st_muxwinit = &mx_lwinit,
};

struct cdevsw mx_cdev = {
	.d_name = DRV_NAME,
	.d_str = &mxinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static __init int
mxinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = register_strdev(&mx_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register device %d, err = %d\n", DRV_NAME, (int) major, -err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

static int
mxterminate(void)
{
	int err;

	if ((err = unregsiter_strdev(&mx_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister device %d, err = %d\n", DRV_NAME, (int) major, -err);
		return (err);
	}
	major = 0;
	return (0);
}

static __exit void
mxexit(void)
{
	mxterminate();
}

module_init(mxinit);
module_exit(mxexit);
