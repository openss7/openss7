/*****************************************************************************

 @(#) File: src/drivers/rsw.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

/*
 * This is the RTP-SW multiplexing driver.  It s purpose is to allo a single
 * device, /dev/streams/rsw, to represent all RTP multiplex and channel device
 * streams in the system.  The Matrix SNMP (mxMIB) configuration script opens
 * the appropriate device drivers (NPI-UDP) and links (I_PLINKS) then under this
 * multiplexer to form a single view of the device drivers.  It is then possible
 * to open a single MX or CH upper stream on this multiplexer and either use it
 * directly or link it, with appropriate modules pushed, under another
 * multiplexing device driver.  Examples for MX streams would be linking them
 * under the MG multiplexer or ZAPTEL multiplexer.
 *
 * The driver also provides pseudo-digital cross-connect capabilities within
 * lower NPI-UDP streams or between lower NPI-UDP streams.  This cross-connect
 * can be performed on a dynamic basis (switching), a semi-permanent basis
 * (DCCS), or a permanent basis (channel bank).
 *
 * Whether an upper stream is a CH stream or an MX stream is determined using
 * the minor device number of the opened Stream.  Upper CH and MX streams can be
 * clone streams or non-clone streams.  Clone streams are Style 2 streams that
 * can be attached to any available PPA.  Non-clone streams have a minor device
 * number that maps to a global PPA or CLEI of a lower Stream (the mapping is
 * assigned when the Stream is I_PLINKED beneath the multiplexing driver).
 * These are Style 1 MX or CH streams.  Only individual channel or full span CH
 * streams are available as Style 1 streams.  Fractional CH streams must utilize
 * the clone CH device number and attach to the appropriate fraction of the
 * span.
 *
 * The primary reason that the MX and CH portion of the switching matrix are
 * combined into one multiplexing device driver is to support the switching of
 * multi-rate ISDN and ISUP calls, where multiple 64 kbps channels are ganged
 * together to provide the multirate connection.
 *
 * Note that the only time that it is necessary to pass channel data to and from
 * the driver using an upper CH stream is for data or voice connections that are
 * terminated on the host.  For example, a CH stream is opened and attached to a
 * channel and an application plays an announcement of tone into the connection.
 *
 * Note that this switching matrix is not capable of performing channle
 * conversion that is not provided by the lower CH or MX streams themselves.
 * All pseudo-digital cross-connect is performed clear channel and consists of
 * siimiply full or half-duplex connections.  At a later data, one exception to
 * this might be the non-intensive mu-Law to A-law conversion.
 *
 * To perform dynamic media conversion, conferencing, or other media conversion
 * functions, MX and CH upper streams on this driver (or their corresponding
 * underlying devices) should be linked beneath the MG driver.  However, static
 * media conversion can also be performed using a module or the device driver
 * directly.  For example, VT over IP or RTP streams can have conversion modules
 * pushed to provide an MX or CH interface that can then be linked directly
 * underneath the MATRIX multiplexing driver.  For example, the RTP stream then
 * can convert between the codec in use on the RTP stream and either mu-Law or
 * A-law PCM presented to the MATRIX driver as a CH stream.  To facilitate this,
 * the upper management CH stream on the MATRIX driver can pass whatever
 * input-output controls it wishes to a managed lower CH stream.  For example,
 * this could be used to set the codec used by a lower RTP stream before it is
 * enabled.  The same is true for the MX management stream of the MATRIX driver.
 */

/*
 *  The purpose of the RTP-SW multiplexing driver is to accept RTP packets using
 *  a stream or multi-stream NPI interface at the lower multiplex and determine
 *  the upper multiplex stream onto which they are to be delivered.  The intent
 *  is that there is a separate upper multiplex stream for each codec (although
 *  this driver does not perform transcoding).  The transcoding itself is
 *  peformed by a RTP-XC transcoding module that pushed over the upper multiplex
 *  stream.
 *
 *  The driver does perform jitter buffering and statistical analysis on the RTP
 *  media stream using the RTP envelope.  The RTP envelope is also examined to
 *  determine the static or dynamic payload type mapping to determine upon which
 *  upper stream the packets are to be delivered.  A change in payload type on
 *  the RTP media stream can result in a change in selection of the upper
 *  multiplex stream.  Each upper multiplex queue pair performs jitter buffering
 *  and statistical analysis of each virtual RTP channel.
 *
 *  The upper multiplex presents a CHI interface: either single-stream using
 *  M_PROTO/M_PCPROTO primitives, or multi-stream using M_CTL/M_PCCTL
 *  primitives.  The life-cycle of an RTP media stream is somewhat different
 *  than the TDM application of the CH interface.  Whereas TDM channels are
 *  ready to be enabled and connected in either or both directions after being
 *  attached, RTP channels are only able to be connected in the receive
 *  direction once being attached.  An additional step is required to connect
 *  the channel in the transmit direction: this step requires specification of
 *  the remote IP address and port number to which to transmit and can either be
 *  peformed before or after connection.  The channel cannot transfer data in
 *  the transmit direction until this remote descriptor has been specified.
 *  Specification of the remote descriptor is accomplished using the
 *  CH_OPTMGMT_REQ primitive, (or maybe a new CH_MODIFY_REQ, CH_OK_ACK and
 *  CH_MODIFY_CON sequence).
 *
 *  A second wrinke with RTP channels is that it is advantageous to permit the
 *  underlying driver to select the IP address or port number that is to be used
 *  for the connection and this selection needs to be communicated back the the
 *  CHS user.  TDM channels are fully specified.  This means that when a
 *  CH_ATTACH_REQ is issued, the driver needs to be able to communicate the
 *  selected channel back to the CHS-user.  A difficulty with the CHI as
 *  originally specified is that the CH_ATTACH_REQ was acknowledged with a
 *  CH_OK_ACK.  The CHI interface needs to be modified to respond with a
 *  CH_ATTACH_ACK that returns the CHS provider selected address.  The state
 *  machine does not need to be altered much, a CH_ATTACH_ACK is simply used to
 *  respond to the CH_ATTACH_REQ instead of a CH_OK_ACK.
 *
 *  A third wrinkle with RTP channels is that RTP payload types can be mapped to
 *  different upper streams.  During the initial phase of connection, where
 *  receive-only is possible, it is possible that the remote sender will select
 *  one or another of a set of possible codecs in which to send information.
 *  The exact codec used is determined by the RTP payload type.  The reason the
 *  we use multiple upper multiplex streams, one for each codec, is for
 *  efficiency.  Queueing messages that need to have the same algorithmic
 *  functions performed on them increases processor instruction cache
 *  performance dramatically.  A mechanism is required to tell the driver on
 *  which upper stream to connect.  This might beg the use of an additional
 *  multiplexing driver has the individual streams opened on the upper portion
 *  of this driver under the new multiplexing driver in question.  This new
 *  multiplexing driver could then reaggregate transcoded channel information
 *  back into a single multi-stream CH interface.  The assembly procedure would
 *  then be to open a stream on this driver, push a transcoding module, and then
 *  link it under the aggregation driver.  When a channel is established on an
 *  upper multiplex stream to the aggregation driver, the mapping could be
 *  performed once the media stream is attached.  The mapping could be specified
 *  with CH_OPTMGMT_REQ/CH_OK_ACK primitives after the attach and the mapping
 *  communicated downward by issuing single-codec CH_OPTMGMT_REQ/CH_OK_ACK
 *  exchanges on the appropriate downside stream.
 *
 *  Another approach would be to perform multiple attachments of the same
 *  virtual channel interface, one for each downward stream.  This would require
 *  the ability to perform a subsequent attachment on the same tag.
 *
 *
 *  Another approach can be to uses the PROTOID fields of a slightly modified
 *  NPI-UDP driver (modified into a NPI-RTP) driver to specify the payloads
 *  associated with a connection and to have the ability to form separate NPI
 *  virtual connections for different payload types.  This would permit binding
 *  and connecting different payloads on different streams.
 *
 *  A major reason for all this perambulation is that when connecting a VoIP
 *  call over RTP it might be desirable to cross-connect same-payload types
 *  while transcoding different payload types.  For example, although the
 *  connection on one leg is G.711, it might be advantageous to offer G.711 and
 *  G.729 to the connecting leg.  Received G.711 payload could then be directly
 *  cross-connected between legs, whereas any received G.729 packets are
 *  terminated and transcoded.
 */

#define _SVR4_SOURCE

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define tp_tst_bit(nr,addr)	test_bit(nr,addr)
#define tp_set_bit(nr,addr)	__set_bit(nr,addr)
#define tp_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#ifdef HAVE_KINC_LINUX_BRLOCK_H
#include <linux/brlock.h>
#endif

#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/snmp.h>

#include <net/udp.h>

#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif

#include <net/protocol.h>

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#endif				/* LINUX */

#include "ip_hooks.h"

#include <sys/npi.h>
#include <sys/npi_ip.h>

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/xti.h>
#include <sys/xti_inet.h>

#define T_ALLLEVELS -1

#define RSW_DESCRIP	"Real-Time Protocol (RTP) Switching STREAMS Driver"
#define RSW_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define RSW_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define RSW_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define RSW_DEVICE	"SVR 4.2 MP STREAMS RTP Switch Driver"
#define RSW_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define RSW_LICENSE	"GPL"
#define RSW_BANNER	RSW_DESCRIP	"\n" \
			RSW_EXTRA	"\n" \
			RSW_REVISION	"\n" \
			RSW_COPYRIGHT	"\n" \
			RSW_DEVICE	"\n" \
			RSW_CONTACT
#define RSW_SPLASH	RSW_DESCRIP	" - " \
			RSW_REVISION

#ifdef LINUX
MODULE_AUTHOR(RSW_CONTACT);
MODULE_DESCRIPTION(RSW_DESCRIP);
MODULE_SUPPORTED_DEVICE(RSW_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(RSW_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-rsw");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define RSW_DRV_ID	CONFIG_STREAMS_RSW_MODID
#define RSW_DRV_NAME	CONFIG_STREAMS_RSW_NAME
#define RSW_CMAJORS	CONFIG_STREAMS_RSW_NMAJORS
#define RSW_CMAJOR_0	CONFIG_STREAMS_RSW_MAJOR
#define RSW_UNITS	CONFIG_STREAMS_RSW_NMINORS

#ifdef LINUX
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_RSW_MODID));
MODULE_ALIAS("streams-driver-rsw");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_RSW_MAJOR));
MODULE_ALIAS("/dev/streams/rsw");
MODULE_ALIAS("/dev/streams/rsw/*");
MODULE_ALIAS("/dev/streams/clone/rsw");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-" __stringify(RSW_CMAJOR_0));
MODULE_ALIAS("/dev/rsw");
//MODULE_ALIAS("devname:rsw");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 * STREAMS Definitions
 */

#define DRV_ID		RSW_DRV_ID
#define DRV_NAME	RSW_DRV_NAME
#define CMAJORS		RSW_CMAJORS
#define CMAJOR_0	RSW_CMAJOR_0
#define UNITS		RSW_UNITS
#ifdef MODULE
#define DRV_BANNER	RSW_BANNER
#else				/* MODULE */
#define DRV_BANNER	RSW_SPLASH
#endif				/* MODULE */

STATIC struct module_info rsw_rinfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),	/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT << 5,	/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

STATIC struct module_stat rsw_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat rsw_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

STATIC struct streamscall int rsw_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC struct streamscall int rsw_qclose(queue_t *, int, cred_t *);

streamscall int rsw_rput(queue_t *, mblk_t *);
streamscall int rsw_rsrv(queue_t *);

STATIC struct qinit rsw_rinit = {
	.qi_putp = rsw_rput,	/* Read put procedure (message from below) */
	.qi_srvp = rsw_rsrv,	/* Read service procedure */
	.qi_qopen = rsw_qopen,	/* Each open */
	.qi_qclose = rsw_qclose,	/* Last close */
	.qi_minfo = &rsw_rinfo,	/* Module information */
	.qi_mstat = &rsw_rstat,	/* Module statistics */
};

STATIC struct module_info rsw_winfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME, /* Module name */ ,
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),	/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT,	/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

streamscall int rsw_wput(queue_t *, mblk_t *);
streamscall int rsw_wsrv(queue_t *);

STATIC struct qinit rsw_winit = {
	.qi_putp = rsw_wput,	/* Write put procedure (message from above) */
	.qi_srvp = rsw_wsrv,	/* Write service procedure */
	.qi_minfo = &rsw_winfo,	/* Module information */
	.qi_mstat = &rsw_wstat,	/* Module statistics */
};

MODULE_STATIC struct streamtab rsw_info = {
	.st_rdinit = &rsw_rinit,	/* Upper read queue */
	.st_wrinit = &rsw_winit,	/* Upper write queue */
};

struct rsw_connection {
	int valid;
	uint32_t daddr;
	uint32_t saddr;
	uint16_t dport;
	uint16_t sport;
	struct rp *rsw;
};



/*
 *  -------------------------------------------------------------------------
 *
 *  CH-User to CH-Provider primitives
 *
 *  -------------------------------------------------------------------------
 */

/** ch_attach_req - attach a CH stream to a PPA
  * @q: active queue
  * @mp: the CH_ATTACH_REQ primitive
  * @rs: private structure (locked)
  * @ch: channel structure
  */
static int
ch_attach_req(queue_t *q, mblk_t *mp, struct rs *rs, struct ch *ch)
{
	CH_attach_req_t *p;
	int err;

	err = CHBADPRIM;
	if (unlikely(!MBLKIN(mp, sizeof(ch), sizeof(*p))))
		goto error;
	p = (typeof(p))(mp->b_rptr + sizeof(ch));
	dassert(p->ch_primitive == CH_ATTACH_REQ);
	if (unlikely(!MBLKIN(mp, p->ch_addr_offset, p->ch_addr_length)))
		goto error;
}


/** rsw_w_prim_put - process primitive for write queue
  * @q: target queue in queue pair (write queue)
  * @mp: the message
  */
STATIC INLINE streamscall __hot_put int
rsw_w_prim_put(queue_t *q, mblk_t *mp)
{
}

/** rsw_w_prim_srv - process primitive on write queue
  * @q: active queue in queue pair (write queue)
  * @mp: the message
  */
STATIC INLINE streamscall __hot_put int
rsw_w_prim_srv(queue_t *q, mblk_t *mp)
{
}

/** rsw_r_prim_srv - process primitive on read queue
  * @q: active queue in queue pair (read queue)
  * @mp: the message
  */
STATIC INLINE streamscall __hot_in int
rsw_r_prim_srv(queue_t *q, mblk_t *mp)
{
}

STATIC INLINE streamscall __hot_in int
rsw_r_prim_srv(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type == M_DATA)) {
		/* fast path for data */
		if (likely(RSW_PRIV(q)->info.SERV_type == T_CLTS)) {
			int rtn = -EAGAIN;

			if (likely(ss7_trylockq(q))) {
				rtn = rsw_data_ind(q, mp);
				ss7_unlockq(q);
			}
			return (rtn);
		}
	}
	return rsw_r_prim_slow(q, mp);
}

#define PRELOAD (FASTBUF<<2)

/** rsw_rput - process message from below
  * @q: the read queue
  * @mp: the message to put
  *
  * This is a classical STREAMS put procedure.  It might be possible to expedite
  * normal messages.  Priority messages must usually be expedited.  When the
  * queue contains messages, normal messages may not be expedited.  Messages
  * that cannot be processed immediately must be placed on the queue.  Messages
  * that require an amount of processing (usually much more than a state
  * change), such as most data messages, should be processed out of the service
  * procedure for performance.  Note that because there is no lower queue we do
  * not have to worry about M_FLUSH messages and stuff.
  */
streamscall __hot_out int
rsw_rput(queue_t *q, mblk_t *mp)
{
	rsw_rstat.ms_acnt++;
	mp->b_wptr += PRELOAD;
	if (likely(!putq(q, mp))) {
		mp->b_band = 0;	/* must succeed */
		putq(q, mp);
	}
	return (0);
}

/** rsw_rsrv - service the read queue
  * @q: the read queue to service
  *
  * This is a classical STREAMS service procedure.  Messages are removed from
  * the queue one at a time with getq() and processed until a message cannot be
  * processed (usually due to flow control).  Any messages that cannot be
  * processed are placed back on the queue and the procedure returns.
  */
streamscall __hot_out int
rsw_rsrv(queue_t *q)
{
	mblk_t *mp;

	while (likely((mp = getq(q)) != NULL)) {
		/* remove backpressure */
		mp->b_wptr -= PRELOAD;
		if (unlikely(rsw_r_prim_srv(q, mp) != QR_ABSORBED)) {
			if (unlikely(!putbq(q, mp))) {
				mp->b_band = 0; /* must succeed */
				putbq(q, mp);
			}
			break;
		}
	}
	return (0);
}

/** rsw_wput - process message from above
  * @q: the write queue
  * @mp: the message to put
  *
  * This is a classical STREAMS put procedure.  It might be possible to expedite
  * normal messages.  Priority messages must usually be expedited.  When the
  * queue contains messages, normal messages may not be expedited.  Messages
  * that cannot be processed immediately must be placed on the queue.  Messages
  * that require an amount of processing (usually much more than a state
  * change), such as most data messages, should be processed out of the service
  * procedure for performance.
  */
streamscall __hot_in int
rsw_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || rsw_w_prim_put(q, mp) != QR_ABSORBED) {
		rsw_wstat.ms_acnt++;
		mp->b_wptr += PRELOAD;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;	/* must succeed */
			putq(q, mp);
		}
	}
	return (0);
}

/** rsw_wsrv - service the write queue
  * @q: the write queue to service
  *
  * This is a classical STREAMS service procedure.  Messages are removed from
  * the queue one at a time with getq() and processed until a message cannot be
  * processed (usually due to flow control).  Any messages that cannot be
  * processed are placed back on the queue and the procedure returns.
  *
  * Note that putbq() is guaranteed to succeed when it follows getq() for the
  * same queue and message.
  */
streamscall __hot_in int
rsw_wsrv(queue_t *q)
{
	mblk_t *mp;

	while (likely((mp = getq(q)) != NULL)) {
		mp->b_wptr -= PRELOAD;
		if (unlikely(rsw_w_prim_srv(q, mp) != QR_ABSORBED)) {
			if (unlikely(!putbq(q, mp))) {
				mp->b_band = 0; /* must succeed */
				putbq(q, mp);
			}
			break;
		}
	}
	return (0);
}

/** rsw_lookup_common - common lookup routines for both normal and ICMP packets
  * @proto: protocol ID
  * @daddr: destination address
  * @dport: destination port
  * @saddr: source address
  * @sport: source port
  *
  * This deviates from other lookup functions for rawip and udp drivers.  This
  * is because we are performing a switching or transcoding function.  Also, we
  * want to quickly rule out any packets that are not for us.
  */
STATIC INLINE fastcall __hot_in struct rp *
rsw_lookup_common(uint8_t proto, uint32_t daddr, uint16_t dport, uint32_t saddr, uint16_t sport)
{
	struct rp *rp = NULL;

	/* We use a port bit-map table using the fact that RTP uses even and odd sequential port
	   numbers and that port numbers at either extreme of the range are unlikely (and largely
	   unusable).  This permits one 2^15 bit map to represent every port or 2^12 bytes, or 4k
	   (one kernel page size).  So, first step is to see whether there is a bit set in the bit
	   map for the destination port number. */
	if (unlikely(!rsw_test_port(dport)))
		goto unlocked_done;
	/* We test without locking, because we are not concerned about missing a packet on the rare 
	   occassion that the port was added just after we tested.  Also, if it was removed just
	   after we tested, we will discover this under the locks anyway. */
	read_lock_irqsave(&rsw_prot_lock, flags);
	/* This is the likely path because we are expecting an extreme amount of RTP traffic and
	   only a sprinkling of other UDP traffic.  */
	/* One more test with the locks on. */
	if (unlikely(!rsw_test_port(dport)))
		goto done;
	/* Now, because we do not expect to have a large number of local IP addresses
	 * assigned to the same port (and can ensure this by exhausting all of
	 * the ports on each IP address before using another) we can simply look
	 * up the connections by destination port number and then perform a
	 * linear search of the connection list.  If we allocated each
	 * connection list as a linear sequence of connection structures within
	 * the same kernel page, we can get the processor data cache and
	 * prefetch engine working for us. */
	{
		struct rsw_connection *rcp;
		int i;

		if (unlikely((rcp = rsw_lookup_port(dport)) == NULL))
			goto done;
		/* note that rsw_lookup_port should do the initial prefetch */
		prefetch(rcp);
		/* Ok, this is fairly quick.  We lookup the page (or subpage)
		 * that contains the connection structures (hash collision list)
		 * which is bounded to RSW_CONNECTIONS_PER_PAGE. */
		for (i = 0; i < RSW_CONNECTIONS_PER_PAGE; i++, rcp++, prefetch(rcp))
		{
			if (rcp->valid == 0)
				break;
			if (rcp->daddr != daddr ||
			    rcp->dport != dport ||
			    rcp->saddr != saddr ||
			    rcp->sport != sport )
				continue;
			rp = rcp->rsw;
			prefetchw(rp);
			break;
		}
	}
      done:
	read_unlock_irqrestore(&rsw_prot_lock, flags);
      unlocked_done:
	return (rp);
}

/** rsw_lookup - lookup Stream by prtocol, address and port.
  * @iph: IP header
  * @uh: UDP header
  *
  * The UDP header here is just to get port numbers in the right place.  All
  * connection-oriented IP protocols use port numbers in the same layout as UDP.
  * This header is ignored for connectionless lookups if the Stream is not bound
  * to a non-zero port number.
  *
  * There are two places to look: connection hash and bind hash.  Connection
  * hashes and bind hashes are only searched for connection-oriented Streams if
  * there is a connection oriented Stream bound to the protocol id.  Bind hashes
  * are only searched for connectionless Streams if there is a connectionless
  * Stream bound to the protocol id.
  */
STATIC INLINE fastcall __hot_in struct rp *
rsw_lookup(struct iphdr *iph, struc udphdr *uh)
{
	return rsw_lookup_common(iph->protocol, iph->daddr, uh->dest,
			iph->saddr, uh->source);
}

/**
  * rsw_lookup_icmp - a little different from rsw_lookup(), for ICMP messages
  * @iph: returned IP header
  * @len: length of return message
  *
  * This needs to do a reverse lookup (where desintation address and port are
  * compared to source address and port, and visa versa).
  */
noinline fastcall __unlikely struct rp *
rsw_lookup_icmp(struct iphdr *iph, unsigned int len)
{
	struc udphdr *uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));

	if (len < (iph->ihl << 2) + 4)
		/* too short: don't have port numbers - ignore it */
		return (NULL);
	return ip_lookup_common(iph->protocol, iph->saddr, uh->source, iph->daddr, uh->dest);
}

#if 1
/**
  * rsw_free - message block free function for mblks esballoc'ed from sk_buffs
  * @data: client data (sk_buff pointer in this case)
  */
STATIC streamscall __hot_get void
rsw_free(caddr_t data)
{
#if 0
	struct sk_buff *skb = (typeof(skb)) data;
	struct rp *rp;
	unsigned long flags;

	dassert(skb != NULL);
	if (likely((rp = *(struct rp **) skb->cb) != NULL)) {
		spin_lock_irqsave(&rp->qlock, flags);
		// ensure(rp->rcvmem >= skb->truesize, rp->rcvmem = skb->truesize);
		rp->rcvmem -= skb->truesize;
		spin_unlock_irqrestore(&rp->qlock, flags);
#if 0
		/* put this back to null before freeing it */
		*(struct rp **) skb->cb = NULL;
#endif
		rsw_put(rp);
	      done:
		kfree_skb(skb);
		return;
	}
	goto done;
#else
	struct sk_buff *skb = (typeof(skb)) data;

	dassert(skb != NULL);
	kfree_skb(skb);
	return;
#endif
}
#endif

/**
  * rsw_v4_rcv - receive IPv4 protocol packets
  * @skb: socket buffer containing IP packet
  *
  * This function is a callback function called by the Linux IP code when a
  * packet is delivered to an IP protocol nmber to which a Stream is bound.  If
  * the destination address is a broadcast or multicast address, pass it for
  * distribution to multiple Streams.  If the detination address is a unicast
  * address, look up the receiving IP Stream based on the protocol number and IP
  * addresses.  If no receiving IP Stream exists for a unicast packet, or if the
  * packet is a broadcast or mulicast packet, pass the packet along to the next
  * handler if any.  If there is no next handler and the packet was not sent to
  * any Stream, generate an appropriate ICMP error.  If the receiving Stream is
  * flow controlled, simply discard its copy of the IP packet.  Otherwise,
  * generate an (internal) M_DATA message and pass it to the Stream.
  */
STATIC __hot_in int
rsw_v4_rcv(struct sk_buff *skb)
{
	struct rp *rp;
	struct iphdr *iph = (typeof(iph)) skb_network_header(skb);
	struct udphdr *uh(struct udphdr *) (skb_network_header(skb) + (iph->ihl << 2));
	struct rtable *rt;
	ushort ulen;

#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif
	if (unlikely(!pskb_may_pull(skb, sizeof(struct udphdr))))
		goto too_small;
#if 0
	/* I don't think that ip_rcv will ever give us a packet that is not PACKET_HOST. */
	if (unlikely(skb->pkt_type != PACKET_HOST))
		goto bad_pkt_type;
#endif
	rt = skb_rtable(skb);
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		/* need to do something about broadcast and multicast. */ ;
	_printd(("%s: %s: packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	uh = (typeof(uh)) skb_transport_header(skb);
	ulen = ntohs(uh->len);
	/* sanity check UDP length */
	if (unlikely(ulen > skb->len || ulen < sizeof(struct udphdr)))
		goto too_small;
	if (unlikely(pskb_trim(skb, ulen)))
		goto too_small;
	/* we do the lookup before the checksum */
	if (unlikely((rp = rsw_lookup(iph, uh)) == NULL))
		goto no_stream;
	/* checksum initialization */
	if (likely(uh->check == 0))
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else {
		if (skb->ip_summed = CHECKSUM_HW)
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		else if (skb->ip_summed != CHECKSUM_UNNECESSARY)
			skb->csum = csum_tcpudp_nofold(iph->saddr, iph->daddr,
						       ulen, IPPROTO_UDP, 0);
	}
#if 0
	/* For now... We should actually place non-linear fragments into separate mblks and pass
	   them up as a chain, or deal with non-linear sk_buffs directly.  As it winds up, the
	   netfilter hooks linearize anyway. */
#ifdef HAVE_KFUNC_SKB_LINEARIZE_1_ARG
	if (unlikely(skb_is_nonlinear(skb) && skb_linearize(skb) != 0))
		goto linear_fail;
#else
	if (unlikely(skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0))
		goto linear_fail;
#endif				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
#else
	if (unlikely(skb_is_nonlinear(skb))) {
		_ptrace(("Non-linear sk_buff encountered!\n"));
		goto linear_fail;
	}
#endif
#if 0
	/* Before passing the message up, check that there is room in the receive buffer.  Allows
	   slope over by 1 buffer per processor. */
	if (unlikely(rp->rcvmem > tp->options.xti.rcvbuf))
		goto flow_controlled;
	{
		mblk_t *mp;
		frtn_t fr = { &rsw_free, (caddr_t) skb };
		size_t plen = skb->len + (skb->data - skb->nh.raw);

		/* now allocated an mblk */
		if (unlikely((mp = esballoc(skb->nh.raw, plen, BPRI_MED, &fr)) == NULL))
			goto no_buffers;
		/* tell others it is a socket buffer */
		mp->b_datap->db_flags |= DB_SKBUFF;
		_ptrace(("Allocated external buffer message block %p\n", mp));
		/* We can do this because the buffer belongs to us. */
		*(struct rp **) skb->cb = rsw_get(rp);
		/* already in bottom half */
		spin_lock(&rp->qlock);
		rp->rcvmem += skb->truesize;
		spin_unlock(&rp->qlock);
		// mp->b_datap->db_type = M_DATA;
		mp->b_wptr += plen;
		put(rp->oq, mp);
		UDP_INC_STATS_BH(UdpInDatagrams);
		/* release reference from lookup */
		rsw_put(rp);
		return (0);
	}
#else
	{
		mblk_t *mp;

#if 1
		frtn_t fr = { &rsw_free, (caddr_t) skb };
		size_t plen = skb->len + (skb->data - skb_network_address(skb));

		/* now allocate a mblk */
		if (unlikely((mp = esballoc(skb_network_header(skb), plen, BPRI_MED, &fr)) == NULL))
			goto no_buffers;
		/* tell others it is a socket buffer */
		mp->b_datap->db_flag |= DB_SKBUFF;
#else
		if (unlikely((mp = skballoc(skb, BPRI_MED)) == NULL))
			goto no_buffers;
#endif
		_ptrace(("Allocated external buffer message block %p\n", mp));
		/* check flow control only after we have a buffer */
		if (unlikely(!canput(rp->oq)))
			goto flow_controlled;
		// mp->b_datap->db_type = M_DATA; /* already done */
#if 1
		mp->b_wptr += plen;
#else
		mp->b_wptr += skb->len;
#endif
		put(rp->oq, mp);
		/* release reference from lookup */
		rsw_put(rp);
		return (0);
	      flow_controlled:
		rsw_rstat.ms_ccnt++;
		freeb(mp);	/* will take sk_buff with it */
		rsw_put(rp);
		return (0);
	}
#endif
      bad_checksum:
	// UDP_INC_STATS_BH(UdpInErrors);
	// IP_INC_STATS_BH(IpInDiscards);
	/* decrement IpInDelivers ?? */
	// goto linear_fail;
      no_buffers:
#if 0
      flow_controlled:
#endif
      linear_fail:
	if (rp)
		rsw_put(rp);
	kfree_skb(skb);
	return (0);
      no_stream:
	ptrace(("ERROR: No stream\n"));
	/* Note, in case there is nobody to pass it to, we have to complete the checksum check
	   before dropping it to handle stats correctly. */
	if (skb->ip_summed != CHECKSUM_UNNECESSARY
	    && (unsigned short) csum_fold(skb_checksum(skb, 0, skb->len, skb->csum)))
		goto bad_checksum;
	// UDP_INC_STATS_BH(UdpNoPorts); /* should wait... */
#if 0
      bad_pkt_type:
#endif
      too_small:
	if (rsw_v4_rcv_next(skb)) {
		/* TODO: want to generate ICMP port unreachable error here */
	}
	return (0);
}

/**
  * rsw_v4_err - process a received ICMP packet
  * @skb: socket buffer containing ICMP packet
  * @info: additional information (unused)
  *
  * This function is a network protocol callback that is invoked when transport
  * specific ICMP errors are received.  The function looks up the Stream and, if
  * found, wraps the packet in an M_CTL message and passes it ot the read queue
  * of the Stream.
  *
  * ICMP packet consists of ICMP IP header, ICMP header, IP header of returned
  * packet and IP payload of returned packet (up to some number of bytes of
  * total payload).  The passed in sk_buff has skb->data point to the ICMP
  * payload which is the beginning of the returned IP header.  However, we
  * include the entire packet in the message.
  *
  * LOCKING: master.lock protects the master list and protects from open, close,
  * link and unlink.  rp->qlock protects the state of private structure.
  * rp->refs protects the private structure from being deallocated before
  * locking.
  */
STATIC __unlikely void
rsw_v4_err(struct sk_buff *skb, u32 info)
{
	struct rp *rp;
	struct iphdr *iph = (struct iphdr *) skb->data;

#if 0
	/* icmp.c does this for us */
#define ICMP_MIN_LENGTH sizeof(struct udphdr)
	if (skb->len < (iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
#endif
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
	if ((rp = rsw_lookup_icmp(iph, skb->len)) == NULL)
		goto no_stream;
	if (rsw_get_state(rp) == TS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		queue_t *q;
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* Create and queue a specialized M_CTL message to the Stream's read queue for
		   further processing.  The Stream will convert this message into a T_UDERROR_IND
		   or T_DISCON_IND message and pass it along. */
		if ((mp = allocb(plen, BPRI_MED)) == NULL)
			goto no_buffers;
		/* Check flow control only after we have a buffer */
		if ((q = rp->oq) == NULL || !bcanput(q, 1))
			goto flow_controlled;
		mp->b_datap->db_type = M_CTL;
		mp->b_band = 1;
		bcopy(skb_network_header(skb), mp->b_wptr, plen);
		mp->b_wptr += plen;
		putq(q, mp);
		goto discard_put;
	      flow_controlled:
		rsw_rstat.ms_cnt++;
		ptrace(("ERROR: stream is flow controlled\n"));
		freeb(mp);
		goto discard_put;
	}
      discard_put:
	/* release reference from lookup */
	rsw_put(rp);
	rsw_v4_err_next(skb, info);	/* anyway */
	return;
      no_buffers:
	ptrace(("ERROR: could not allocated buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	rsw_v4_err_next(skb, info);
#if 0
	goto drop;
      drop:
#endif
#ifdef HAVE_KINC_LINUX_SNMP_H
#ifdef HAVE_ICMP_INC_STATS_BH_2_ARGS
	ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#endif
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
	return;
}

/*
 *  Open and Close
 */
STATIC int rsw_majors[CMAJORS] = { CMAJOR_0, };

/**
  * rsw_open - RTP Switch driver STREAMS open routine
  * @q: read queue of opened stream
  * @devp: point to device number opened
  * @oflag: flags to the open call
  * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
  * @crp: pointer to opener's credentials
  */
STATIC streamscall int
rsw_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int mindex = 0;
	int type = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct rsw *rsw, **rswp = &master.rsw.list;
	unsigned long flags;
	mblk_t *mp;
	struct stroptions *so;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		return (EIO);
	}
	/* Linux Fast-STREAMS always passes internal major device number (module id).  Note also,
	   however, that strconf-sh attempts to allocate module ids that are identical to the base
	   major device number anyway. */
	/* Linux Fast-STREAMS always passes internal major device numbers (modules ids) */
	if (cmajor != DRV_ID) {
		return (ENXIO);
	}
	/* sorry, you can't open by minor device */
	if (cminor > LAST_CMINOR) {
		return (ENXIO);
	}
	type = cminor;
#if 0
	if (sflag == CLONEOPEN)
#endif
		cminor = FREE_CMINOR;
	if (!(mp = allocb(sizeof(*so), BPRI_WAITOK)))
		return (ENOBUFS);
	write_lock_irqsave(&master.lock, flags);
	for (; *rswp; rswp = &(*rswp)->next) {
		if (cmajor != (*rswp)->u.dev.cmajor)
			break;
		if (cmajor == (*rswp)->u.dev.cmajor) {
			if (cminor < (*rswp)->u.dev.cminor)
				break;
			if (cminor == (*rswp)->u.dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = rsw_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		write_unlock_irqrestore(&master.lock, flags);
		freeb(mp);
		return (ENXIO);
	}
	_printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(rswp = rsw_alloc_priv(q, rswp, type, devp, crp))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		write_unlock_irqrestore(&master.lock, flags);
		freeb(mp);
		return (ENOMEM);
	}
	write_unlock_irqrestore(&master.lock, flags);
	so = (typeof(so)) mp->b_wptr;
	bzero(so, sizeof(*so));
	so->so_flags |= SO_SKBUFF;
	/* want to set a write offset of MAX_HEADER bytes */
	so->so_flags |= SO_WROFF;
	so->so_wroff = MAX_HEADER;
	so->so_flags |= SO_MINPSZ;
	so->so_minpsz = rsw_winfo.mi_minpsz;
	so->so_flags |= SO_MAXPSZ;
	so->so_maxpsz = rsw_winfo.mi_maxpsz;
#if 0
	so->so_flags |= SO_HIWAT;
	so->so_hiwat = (SHEADHIWAT << 3) + (STRHIGH << 1);
	so->so_flags |= SO_LOWAT;
	so->so_lowat = 0;
#else
	so->so_flags |= SO_HIWAT;
	so->so_hiwat = SHEADHIWAT;
	so->so_flags |= SO_LOWAT;
	so->so_lowat = 0;
#endif
	mp->b_wptr += sizeof(*so);
	mp->b_datap->db_type = M_SETOPTS;
	qprocson(q);
	putnext(q, mp);
	return (0);
}

/**
  * rsw_qclose - RTP Switch driver STREAMS close routine
  * @q: read queue of closing Stream
  * @oflag: flags to open call
  * @crp: pointer to closer's credentials
  */
STATIC streamscall int
rsw_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct rsw *rsw = RSW_PRIV(q);

	(void) oflag;
	(void) crp;
	(void) rsw;
	_printd(("%s: closing character device %d:%d\n", DRV_NAME,
		 rsw->u.dev.cmajor, rsw->u.dev.cminor));
	/* make sure procedures are off */
	flushq(WR(q), FLUSHALL);
	flushq(RD(q), FLUSHALL);
	qprocsoff(q);
	rsw_free_priv(q);
	goto quit;
      quit:
	return (0);
}

/*
 *  Registration and initialization.
 */
#ifdef LINUX
/*
 *  Linux registration
 */

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, short, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the RTP-Switch driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, unit, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the RTP-Switch driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 */

STATIC struct cdevsw rsw_cdev = {
	.d_name = DRV_NAME,
	.d_str = &rsw_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC __unlikely int
rsw_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&rsw_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
rsw_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&rsw_cdev, major)) < 0)
		return (err);
	return (0);
}

MODULE_STATIC void __exit
rsw_terminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (rsw_majors[mindex]) {
			if ((err = rsw_unregister_strdev(rsw_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d",
					DRV_NAME, rsw_majors[mindex]);
			if (mindex)
				rsw_majors[mindex] = 0;
		}
	}
	if ((err = rsw_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	rsw_term_hashes();
	return;
}

MODULE_STATIC int __init
rsw_init(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	rsw_init_hashes();
	if ((err = rsw_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		rsw_terminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = rsw_register_strdev(rsw_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d",
					DRV_NAME, rsw_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				rsw_terminate();
				return (err);
			}
		}
		if (rsw_majors[mindex] = 0)
			rsw_majors[mindex] = err;
		if (major == 0)
			major = rsw_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 */
module_init(rsw_init);
module_exit(rsw_terminate);

#endif				/* LINUX */
