/*****************************************************************************

 @(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/16 20:37:42 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/11/16 20:37:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: v401p.c,v $
 Revision 0.9.2.1  2006/11/16 20:37:42  brian
 - added MX driver or firmware for V401P cards

 *****************************************************************************/

#ident "@(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/16 20:37:42 $"

static char const ident[] = "$RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/16 20:37:42 $";

/*
 *  This is a driver for the Varion V401P card.  It provides only full multi-card access (for speed)
 *  using an MX interface.  What the driver does is run primarily off of a single card's interrupt
 *  for efficiency.  That is, the driver uses a single shared interrupt and interrupt service
 *  routine for all cards.  The order in which cards are serviced is adpative to allow for loosely
 *  synchronized operation, the theory being that the service order for the next service time frame
 *  will be the same as the service order required for the current service time frame.  This should
 *  allow us to run 4 or more cards in a single machine.
 *
 *  Because the service order of cards is independent, there is one MX channel per card consisting
 *  of 8 samples of 128 time slots or 1024 bytes per 1ms interrupt.  Also, elastic buffering is
 *  performed on a card by card basis.  Each kernel memory page consists of 4096 bytes or 4
 *  interrupt frames and represents 4ms per card.  Elastic buffering to 32ms to accomodate a 10ms
 *  tick clock is provided.  The elastic buffer consists of 8 pages per card.  In an attempt to keep
 *  signalling an voice path latency to a minimum, each interrupt time frame is delivered to the MX
 *  channel.
 *
 *  The previous X400P driver used to spawn a tasklet to process receive blocks and generate
 *  transmit blocks.  Tasklets run at bottom half and are run immediately upon return from the
 *  interrupt.  We used to process the receive block within the tasklet and then pass processed
 *  messages up to STREAMS from the tasklet.  Under Linux Fast-STREAMS it is likely a better
 *  approach to place the message block directly on the driver's read queue from the ISR.  The
 *  nature of the Linux Fast-STREAMS scheduler is such that the service procedure will run on the
 *  same processor as the ISR (i.e. not until the ISR returns) avoiding cache ping-ponging and lock
 *  contention on multiple processors.
 *
 *  A slightly different approach from the previous X400 driver is taken with regard to buffer
 *  management.  Message blocks are initially esballoc'ed for specific ranges within the elastic
 *  buffer.  The elastic buffer consists of a read area followed by a write area.  The receive side
 *  of the ISR is fed a chain of these blocks from the write side of the ISR for use in delivery to
 *  the driver's read queue.  Therefore, when the ISR completes processing on a receive block, it
 *  simply places the next block from the chain onto the driver's read queue.  number of blocks are
 *  outstanding in the system).  If no block exists on the chain between the TX and RX ISRs, or if
 *  the read queue is flow controlled, an overrun occurs.  The read service procedure simply passes
 *  the block upstream.
 *
 *  Once the upstream driver or module has completed processing the block, it recycles the block by
 *  filling it with transmit data and passes it back downstream to the driver.
 *
 *  By detecting the number of outstanding blocks in the chain between the TX and RX ISR, the read
 *  service procedure can detect potential receive overflows and allocate and add another kernel
 *  page of message blocks to the chain between the transmit and receive sides of the ISR.  Also, if
 *  the cahin between the transmit and receive sides of the ISR becomes excessive (multiple pages),
 *  unnecessary cache ping-pong will result in the system and the read service procedure can remove
 *  pages from service by removing a chain of message blocks that correspond to the same page.
 *
 *  On the TX side, recycled buffer chain coming down the write side are simply fed to the
 *  write-side ISR from the driver's write put or service procedure.  When the write-side ISR runs,
 *  if there is no block, it simply replays the last interrupt frame and marks the underrun.  If
 *  there is a block, it writes the associated interrupt frame and chains the block to the receive
 *  side of the ISR.
 *
 *  The write side will automatically adjust for system jitter by slipping to the point necessary to
 *  compensate for buffer recycling latency.  Excess latency due to system delays can be determined
 *  by tracking the number of blocks outstanding upstream.  Hopefully this number is 1 or 2 for
 *  voice applications and can be as many as 10 or 20 for signalling applications.
 *
 *  Nevertheless, the system may underrun due to exceptional circumstances and then experience high
 *  latencies while there are deep buffer chains to the TX driver.  Therefore, the write put
 *  procedure can place some upper bounds on the depth of the buffer chain to the TX ISR.  If the
 *  maximum depth is exceeded, blocks are queued to the write side queue and flow control will be
 *  asserted upstream.  If the upstream module can avoid generation of a transmit block (as is true
 *  for signalling applications when the transmit side of the link is idle, or for voice
 *  applications where the data can simply be discarded) it should check for flow control downstream
 *  before generating a transmit block.
 *
 *  When the TX ISR buffer chain falls to a low water mark, the TX ISR enables the write queue's
 *  service procedure which then fill the buffer chain to acceptable limits, backenabling the upper
 *  layer module and removing flow control restrictions.  When flow control is lifted by the driver,
 *  the upper module can then again being generating TX blocks.  If the upper module would result in
 *  a surplus of RX blocks to recycle to TX blocks, the RX block can be set to zero length and
 *  passed downstream.  These blocks will be detected both by the write put procedure and the TX ISR
 *  and simply skipped and recycled to the RX ISR (note that they take no space on the write queue).
 *
 *  Latency can be limited by careful design of the upper module.  The upper module should process
 *  RX data and TX data in the best possible order (this can be accomplished if necessary by keeping
 *  its own chain of recycled buffers) and write the TX frame downstream promptly when available.
 *  If it is necessary to pass channel information upstream, that channel information should be
 *  copied to its own buffers.  The driver will pass the TX frame directly to the TX ISR from its
 *  put procedure before returning.  On the read side, because the module's read put procedure is
 *  invoked from the driver's read service procedure, the module should process RX frames directly
 *  from its put procedure receive data and should avoid ever queuing the frame.  The transmit side
 *  should be driven from the receive side and no more than one TX frame should be passed downstream
 *  for each received RX frame.
 *
 *  The driver also permits situations where, either through design or misadventure, an RX block is
 *  freed by the upper module, or a TX block is created and inserted by the upper module.
 *
 *  To compensate for free RX blocks, The buffer free routine of the RX blocks should regenerate the
 *  RX block (with esballoc) and insert the block in the correct position (by making it available to
 *  the chain to the the RX ISR to be inserted when its proper position arrives).  To compensate for
 *  inserted TX blocks, inserted blocks should be detected by the TX ISR after processing and passed
 *  as a zero-length block to the read side queue to be freed by the read service procedure.  (This
 *  is easiest handled by setting a flag in the data block of messages passed upstream that can be
 *  detected in messages passed downstream).
 *
 *  To avoid overheads on underutilized cards, each channel in the channel map for each card can be
 *  activated independently.  The driver detects when all channels have been activated in a span and
 *  when all spans have been activated for a card for efficiency.  It also detects when no channels
 *  have been activated in a span and when no channels have been activated for a card.  This,
 *  however, does not alter delivery on the MX card channel.  Because channel samples are held in
 *  the elastic buffer and simply referred to by mblk's passed upstream, inactive channels simply
 *  contain silent code.  Note that because each channel is a separate MX stream, their read queues
 *  are independent.  Their elastic buffers could be statistically multiplexed for better cache
 *  efficiency, but that is a feature for later.
 *
 *  In the previous X400 driver, when full T1 raw frames were passed upstream (SDL interface), they
 *  were ordered as to proper channel number.  This is inefficient as the data may simply be moved
 *  (again) from one RX channel to another TX channel (for simple digital cross-connect or
 *  switching) by the upper driver.  Therefore, the MX interface now reports the channel mapping in
 *  effect as byte multiplexing and demultiplexing is more efficiently performed by the upper layer
 *  module.
 *
 *  The purpose of all this perambulation is to be able to provide a simple MX driver for specific
 *  card hardware, and remove the hardware dependencies of upper layer modules (HDLC, MTP2, LAPD,
 *  LAPB, LAPF, LAPM, MG and switching functions), allow a single tested implementation to provide
 *  capabilities for a wide range of hardware, prepare the driver for compatibility with the zaptel
 *  driver interface, and provide direct low latency switching access to the MG driver and for
 *  components such a tone detection, generation, recording and announcements.
 *
 *  The driver processing is greatly simplified, the ISR unlinks a TX block, writes its data to the
 *  card and passes it to the RX side, the RX side unlinks an RX block, fills it directly from the
 *  card and passes it to the read queue.  The rest is just a matter of setting up the card,
 *  deciding which channels to copy, and processing any bit signalling and alarm information.
 *
 *  Second buffer recycling approach:  Rather than esballoc'ing message blocks from a page allocated
 *  elastic buffer, it might be more advantageous to use fast buffers to keep message blocks hot.
 *  Initially, a pool of fast buffers can be allocated that belong to the same buffer cache page (or
 *  several buffer cache pages) for use by the RX ISR.  In the ISR, run the TX side before the RX
 *  side and always recycle the same FASTBUF from TX to RX (if possible: ref count == 1, fast
 *  buffer, marked with our flag) otherwise zero the buffer (b_wptr=b_rptr), pull one from the pool
 *  chain the zero length buffer and fill the pool buffer.  If the buffer can be recycled but the
 *  receive side is in congestion, overflow and push the buffer to the pool (using a page sorted
 *  insert).  In this fashion, if buffers are recycled by the upper module, they should stay nicely
 *  within the same page (both the message block and the data block).  If the upper layer module
 *  frees buffers, it would be nice to have a way for the driver to reclaim the fastbuf.  I need to
 *  investigate changing the free routine associated with the buffer to allow it to reclaim fast
 *  buffers.  This should work even better because it keep the entire message block, data block,
 *  data buffer 3-tuple hot.  A complication is the fact that each ISR run will require 16 fast
 *  buffers for each ISR frame (32-bit machine) or 8 fast buffers for each ISR frame (64-bit
 *  machine).  An advantage is that RX/TX latency can be reduced by interleaving RX/TX in the ISR
 *  as well as the upper layer module at the FASTBUF instead of the ISR frame, keeping cache slots
 *  hotter.
 *
 *  Unfortunately, interleaving buffers beneath the ISR frame size has no advantage (to keep buffers
 *  hot, the read service procedure must run on the same processor invoking the procedure).
 *  A similar approach would be to eballoc buffers, but allocate them from the same set of cache
 *  pages.  A buffer reclaming strategy in STREAMS would really help here too, so that buffers
 *  allocated from within the same cache page can be reclaimed if freed by the upper layer driver.
 *
 *  Several more aspects for management.  It would be good to be able to perform local and remote
 *  loopback on a per channel basis as well as a per span basis if the chip does not support it
 *  directly.
 */

/*
 * Streams definitions.
 */

STATIC struct module_info mx_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat mx_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat mx_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

STATIC struct qinit mx_rinit = {
	.qi_putp = mx_rput,
	.qi_srvp = mx_rsrv,
	.qi_qopen = mx_open,
	.qi_qclose = mx_close,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_rstat,
};

STATIC struct qinit mx_winit = {
	.qi_putp = mx_wput,
	.qi_srvp = mx_wsrv,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_wstat,
};

STATIC struct streamtab v400mxinfo = {
	.st_rdinit = &mx_rinit,
	.st_wrinit = &mx_winit,
};

enum vp_board {
	PLX9030 = 0,
	PLXDEVBRD,
	V400P,
	V400PSS7,
	T400P,
	T400PSS7,
	E400P,
	E400PSS7,
	V401PT,
	V401PE,
};

#define VPF_SUPPORTED	(1<<0)
#define VPF_E1		(1<<1)
#define VPF_T1		(1<<2)
#define VPF_J1		(1<<3)
#define VPF_

#define VP_DEV_IDMASK	0xf0
#define VP_DEV_SHIFT	4
#define VP_DEV_REVMASK	0x0f
#define VP_DEV_DS2154	0x00
#define VP_DEV_DS21354	0x01
#define VP_DEV_DS21554	0x02
#define VP_DEV_DS2152	0x08
#define VP_DEV_DS21352	0x09
#define VP_DEV_DS21552	0x0a
#define VP_DEV_DS2155	0x0b


/* *INDENT-OFF* */
static struct {
	char *name;
	uint32_t hw_flags;
} vp_board_info[] __devinitdata = {
	{ "PLX 9030",			0 },
	{ "PLX Development Board",	0 },
	{ "V400P",			1 },
	{ "V400P-SS7",			1 },
	{ "T400P",			1 },
	{ "T400P-SS7",			1 },
	{ "E400P",			1 },
	{ "E400P-SS7",			1 },
	{ "V401PT",			1 },
	{ "V401PE",			1 },
};

static struct {
	char *name;
	uint32_t hw_flags;
} vp_device_info[] __devinitdata = {
	{ "DS2154 (E1)",	1 },
	{ "DS21354 (E1)",	1 },
	{ "DS21554 (E1)",	1 },
	{ "Unknown ID 0011",	0 },
	{ "Unknown ID 0100",	0 },
	{ "Unknown ID 0101",	0 },
	{ "Unknown ID 0110",	0 },
	{ "Unknown ID 0111",	0 },
	{ "DS2152 (T1)",	1 },
	{ "DS21352 (T1)",	1 },
	{ "DS21552 (T1)",	1 },
	{ "DS2155 (E1/T1/J1)",	1 },
	{ "Unknown ID 1100",	0 },
	{ "Unknown ID 1101",	0 },
	{ "Unknown ID 1110",	0 },
	{ "Unknown ID 1111",	0 }
};
/* *INDENT-ON* */

STATIC struct pci_device_id vp_pci_table[] __devinitdata = {
	{PCI_VENDOR_ID_PLX, 0x9030, PCI_ANY_ID, PCI_ANY_ID, 0, 0, PLX9030},
	{PCI_VENDOR_ID_PLX, 0x3001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, PLXDEVBRD},
	{PCI_VENDOR_ID_PLX, 0xD00D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, V400P},
	{PCI_VENDOR_ID_PLX, 0x0557, PCI_ANY_ID, PCI_ANY_ID, 0, 0, V400PSS7},
	{PCI_VENDOR_ID_PLX, 0xD33D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, V401PT},
	{PCI_VENDOR_ID_PLX, 0xD44D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, V401PE},
	{0,}
};

#define V400P_EBUFNO	(1<<7)	/* 128k of elastic buffers per card. */

STATIC fastcall int
mx_info_req(queue_t *q, mblk_t *mp)
{
}
STATIC fastcall int
mx_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_attach_req: - process MX_ATTACH_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC fastcall int
mx_attach_req(queue_t *q, mblk_t *mp)
{
	struct MX_attach_req *p = (typeof(p)) mp->b_rptr;
	uint32_t addr;
	int card, span;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (p->mx_addr_length != sizeof(addr))
		goto badaddr;
	if (mp->b_wptr < mp->b_rptr + p->mx_addr_length + p->mx_addr_offset)
		goto protoshort;
	/* avoid alignment problems */
	bcopy(mp->b_rptr, &addr, sizeof(addr));
	/* address is simply 8-bit card number and 8-bit span number and 8-bit channel number
	   (ignored) */
	card = (addr >> 16) & 0x0ff;
	span = (addr >> 8) & 0x0ff;

	return (QR_DONE);
      badaddr:
	err = MXBADADDR;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return mx_error_reply(q, MX_ATTACH_REQ, err);
}
STATIC fastcall int
mx_enable_req(queue_t *q, mblk_t *mp)
{
}
STATIC fastcall int
mx_connect_req(queue_t *q, mblk_t *mp)
{
}
STATIC fastcall int
mx_data_req(queue_t *q, mblk_t *mp)
{
}
STATIC fastcall int
mx_disconnect_req(queue_t *q, mblk_t *mp)
{
}
STATIC fastcall int
mx_disable_req(queue_t *q, mblk_t *mp)
{
}
STATIC fastcall int
mx_detach_req(queue_t *q, mblk_t *mp)
{
}

STATIC noinline fastcall int
mx_w_proto(queue_t *q, mblk_t *mp)
{
	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t))
		return (-EMSGSIZE);
	switch (*(uint32_t *) mp->b_rptr) {
	case MX_INFO_REQ:
		return mx_info_req(q, mp);
	case MX_OPTMGMT_REQ:
		return mx_optmgmt_req(q, mp);
	case MX_ATTACH_REQ:
		return mx_attach_req(q, mp);
	case MX_ENABLE_REQ:
		return mx_enable_req(q, mp);
	case MX_CONNECT_REQ:
		return mx_connect_req(q, mp);
	case MX_DATA_REQ:
		return mx_data_req(q, mp);
	case MX_DISCONNECT_REQ:
		return mx_disconnect_req(q, mp);
	case MX_DISABLE_REQ:
		return mx_disable_req(q, mp);
	case MX_DETACH_REQ:
		return mx_detach_req(q, mp);
	}
	return (-EPROTO);
}

STATIC noinline fastcall int
mx_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(_RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(_RD(q), FLUSHDATA);
		return (QR_LOOP);
	}
	return (QR_DONE);
}

STATIC noinline fastcall __hot_put int
mx_w_msg(queue_t *q, mblk_t *mp, int type)
{
	struct mx *mx = MX_PRIV(q);
	int rtn;

	switch (type) {
	case M_PROTO:
	case M_PCPROTO:
		rtn = mx_w_proto(q, mp);
		break;
	case M_FLUSH:
		rtn = mx_w_flush(q, mp);
		break;
	case M_IOCTL:
	case M_IOCDATA:
		/* IO controls */
		rtn = mx_w_ioctl(q, mp);
		break;
	case M_PCCTL:
	case M_CTL:
		/* inter module IO controls */
		rtn = m_w_ctl(q, mp);
		break;
	default:
		rtn = -EPROTO;
		break;
	}
	switch (rtn) {
	default:
		if (rtn < 0)
			printd(("%s: error %d processing message on write queue\n", DRV_NAME,
				-rtn));
		/* fall through */
	case QR_DONE:
		freemsg(mp);
		/* fall through */
	case QR_ABSORBED:
		return (0);
	case QR_LOOP:
		qreply(q, mp);
		return (0);
	case -ENOBUFS:
	case -ENOMEM:
		return (1);
	}
}

/*
 *  STREAMS put and service procedures.
 *  ===================================
 */

STATIC streamscall __hot_write int
mx_wput(queue_t *q, mblk_t *mp)
{
	int type;

	prefetch(q->q_ptr);
#ifdef _DEBUG
	q->q_qinfo->qi_mstat->ms_pcnt++;
#endif
	type = mp->b_datap->db_type;
	if (likely(type < QPCTL)) {
		if (unlikely(q->q_first || (q->q_flag & QSVCBUSY)))
			goto put_it;
		if (type == M_DATA || type == M_NOTIFY) {
			struct mx *mx = MX_PRIV(q);
			register struct vp_tx *tx = &mx->span->tx;

			if (likely(tx->count < tx->maxburst)) {
				register pl_t pl;

				spin_lock_irqsave(tx->lock, pl);
				*tx->tail = mp;
				tx->tail = &mp->b_cont;
				tx->count++;
				spin_unlock_irqrestore(tx->lock, pl);
			      done:
				return (0);
			}
			goto put_it;
		}
	}
	if (likely(mx_w_msg(q, mp, type) == 0))
		goto done;
      put_it:
	putq(q, mp);
	goto done;
}

/**
 * mx_wsrv: - write service procedure
 * @q: queue to service
 *
 * If the message is an M_DATA message or an M_NOTIFY message, simply chain it to the TX ISR.
 * Otherwise, process the message.  Note that if we are debugging we can make a few checks on the
 * message.
 */
STATIC streamscall __hot_out int
mx_wsrv(queue_t *q)
{
	struct mx *mx = MX_PRIV(q);
	register struct vp_tx *tx = &mx->span->tx;
	mblk_t *mp;

	prefetchw(tx);
#ifdef _DEBUG
	q->q_qinfo->qi_mstat->ms_scnt++;
#endif
	if (likely(!!(mp = getq(q)))) {
		do {
			int type = mp->b_datap->db_type;

			if (likely(type == M_DATA || type == M_NOTIFY)) {
				if (likely(tx->count < tx->maxburst)) {
					register pl_t pl;

					spin_lock_irqsave(tx->lock, pl);
					*tx->tail = mp;
					tx->tail = &mp->b_cont;
					tx->count++;
					spin_unlock_irqrestore(tx->lock, pl);
					continue;
				}
				goto put_it_back;
			}
			if (unlikely(mx_w_msg(q, mp, type)))
				goto put_it_back;
		} while (unlikely(!!(mp = getq(q))));
	}
      done:
	return (0);
      put_it_back:
	putbq(q, mp);
	goto done;
}

/**
 * mx_rput: - read put procedure
 * @q: queue to place message on
 * @mp: the message to place
 *
 * This simply places message to the queue.  This is invoked using put(9) directly from the RX ISR.
 * The RX ISR should have called canput(9) to ensure that we are not flow controlled before even
 * filling a receive frame.
 */
STATIC streamscall __hot_in int
mx_rput(queue_t *q, mblk_t *mp)
{
#ifdef _DEBUG
	q->q_qinfo->qi_mstat->ms_pcnt++;
#endif
	return putq(q, mp);
}

/* Reserve no more than 32ms worth of blocks. */
#define VP_RX_RESERVE_MIN (128/FASTBUF)
#define VP_RX_RESERVE_MAX ((128/FASTBUF) << 5)

/**
 * mx_rx_resupply: - resupply buffers for the system to the reserve pool
 *
 * Keep this function out of the way because it should only run until the reserve is adequate or we
 * have already encountered an overflow.  The reserve starts out at 128/FASTBUF blocks, enough for
 * 1 RX ISR run for the span (1ms).  When the RX ISR cannot acquire a block from the pool, it marks
 * an overflow.  When the read service procedure runs, it checks if the reserve is inadequate and
 * doubles the reserve and tops up the reserve.  A hard maximum (32ms worth of blocks) is set on the
 * reserve.  Each time that the TX ISR sucessfully recycles a message block and there have been no
 * overflows, the reserve is dropped by one.
 */
STATIC noinline fastcall void
mx_rx_resupply(struct vp_span *span)
{
	struct vp_rx *rx = &span->rx;
	mblk_t *mp;
	pl_t pl;

	spin_lock_irqsave(&rx->lock, pl);
	{
		if (XCHG(&rx->overflows, 0) && (rx->reserve <<= 1) > VP_RX_RESERVE_MAX)
			rx->reserve = VP_RX_RESERVE_MAX;
		while (rx->avail < rx->reserve) {
			spin_unlock_irqsave(&rx->lock, pl);
			if (unlikely((mp = allocb(FASTBUF, BPRI_HI)) == NULL))
				goto nobufs;
			spin_lock_irqsave(&rx->lock, pl);
			*rx->tail = mp;
			rx->tail = &mp->b_cont;
			rx->avail++;
		}
	}
	spin_unlock_irqsave(&rx->lock, pl);
      nobufs:
	return;
}

/**
 * mx_tx_recycle: - recycle buffers to the system from the free list
 *
 * When the TX has a message block that cannot be recycled, it adds it to the free list.  When the
 * read service procedure runs, it frees all the blocks on this list.  Keep this function out of the
 * way because it is usually the case that the TX block can be recycled to the RX.
 */
STATIC noinline fastcall void
mx_tx_recycle(struct vp_span *span)
{
	stuct vp_tx *tx = &span->tx;
	mblk_t *mp;
	pl_t pl;

	spin_lock_irqsave(&tx->lock, pl);
	{
		while ((mp = tx->head)) {
			if ((tx->head = mp->b_cont) == NULL)
				tx->tail = &tx->head;
			spin_unlock_irqrestore(&tx->lock, pl);
			freeb(mp);
			spin_lock_irqsave(&tx->lock, pl);

		}
	}
	spin_unlock_irqrestore(&tx->lock, pl);
}

/**
 * mx_rsrv: - read service procedure
 * @q: queue to service
 *
 * This is a simple flow controlled drainage procedure.  A check is made at the end to ensure
 * that there are sufficient blocks in the reserve buffer pool and that message block that could not
 * be recycled are freed.
 */
STATIC streamscall __hot_read int
mx_rsrv(queue_t *q)
{
	mblk_t *mp;

#ifdef _DEBUG
	q->q_qinfo->qi_mstat->ms_scnt++;
#endif
	if (likely(!!(mp = getq(q)))) {
		do {
			if (likely(canputnext(q))) {
				putnext(q, mp);
				continue;
			}
			putbq(q, mp);
			break;
		} while (unlikely(!!(mp = getq(q))));
	}
	{
		struct vp_span *span = MX_PRIV(q)->span;

		/* If there are no buffers in the reserve pool, top it up. */
		if (unlikely(span->rx.avail < span->rx.reserve))
			mx_rx_resupply(span);
		/* If there are buffers to free due to receive congestion, free them now. */
		if (unlikely(!!span->tx.pool))
			mx_tx_recycle(span);
	}
	return (0);
}

/*
 *  STREAMS open and close.
 *  =======================
 */

STATIC spinlock_t mx_lock = SPIN_LOCK_UNLOCKED;
STATIC struct mx *mx_list = NULL;
STATIC major_t mx_majors[CMAJORS] = { CMAJOR_0, };

STATIC streamscall int
mx_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	pl_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct mx *mx, *mxp;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		return (EIO);
	}
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN) {
		printd(("%s: Clone open in effect on major %d\n", DRV_NAME, camjor));
		cminor = 1;
	}
	spin_lock_irqsave(&mx_lock, flags);
	for (mxp = &mx_list; *mxp; mxp = &(*mxp)->next) {
		major_t dmajor = (*xpp)->dev.cmajor;

		if (cmajor != dmajor)
			break;
		else {
			minor_t dminor = (*xpp)->dev.cminor;

			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = mx_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&mx_lock, flags);
		return (ENXIO);
	}
	printd(("%s: opened character device %hu:%hu\n", DRV_NAME, (ushort) cmajor,
		(ushort) cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(mx = vp_alloc_mx(q, mxp, devp, crp))) {
		/* never print error messages when out of memory */
		spin_unlock_irqrestore(&mx_lock, flags);
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&mx_lock, flags);

	qprocson(q);

	return (0);
}
STATIC streamscall int
mx_close(queue_t *q, int flag, cred_t *crp)
{
	struct mx *mx = MX_PRIV(q);
	pl_t flags;

	printd(("%s: closing character device %hu:%hu\n", DRV_NAME, mx->dev.cmajor,
		mx->dev.cminor));

	qprocsoff(q);

	spin_lock_irqsave(&mx_lock, flags);
	vp_free_mx(mx);
	spin_unlock_irqrestore(&mx_lock, flags);
	return (0);
}


/*
 *  PCI initialization and removal.
 *  ===============================
 */

STATIC void __devexit
vp_remove(struct pci_dev *dev)
{
	struct vp *vp;

	if (!(vp = pci_get_drvdata(dev)))
		goto disable;
	if (vp->plx) {
		vp->plx[INTCSR] = 0;	/* disable interrupts */
		vp->plx = NULL;
	}
	if (vp->xlb) {
		vp->xlb[SYNREG] = 0;
		vp->xlb[CTLREG] = 0;
		vp->xlb[LEDREG] = 0;
		vp->xlb = NULL;
	}
	if (vp->irq) {
		free_irq(vp->irq, vp);
		printd(("%s: freed irq\n", DRV_NAME));
		vp->irq = 0;
	}
	if (vp->xlb_region) {
		release_mem_region(vp->xlb_region, vp->xlb_length);
		printd(("%s: released xlb region %lx length %ld\n", DRV_NAME,
			vp->xlb_region, vp->xlb_length));
		vp->xlb_region = NULL;
		vp->xlb_length = 0;
	}
	if (vp->xll_region) {
		release_mem_region(vp->xll_region, vp->xll_length);
		printd(("%s: released xll region %lx length %ld\n", DRV_NAME,
			vp->xll_region, vp->xll_length));
		vp->xll_region = NULL;
		vp->xll_length = 0;
	}
	if (vp->plx_region) {
		release_mem_region(vp->plx_region, vp->plx_length);
		printd(("%s: released plx region %lx length %ld\n", DRV_NAME,
			vp->plx_region, vp->plx_length));
		vp->plx_region = NULL;
		vp->plx_length = 0;
	}
	if (vp->xlb) {
		iounmap((void *) vp->xlb);
		printd(("%s: unmapped xlb memory at %p\n", DRV_NAME, vp->xlb));
		vp->xlb = NULL;
	}
	if (vp->xll) {
		iounmap((void *) vp->xll);
		printd(("%s: unmapped xll memory at %p\n", DRV_NAME, vp->xll));
		vp->xll = NULL;
	}
	if (vp->plx) {
		iounmap((void *) vp->plx);
		printd(("%s: unmapped plx memory at %p\n", DRV_NAME, vp->plx));
		vp->plx = NULL;
	}
	vp_term_card(vp);
      disable:
	pci_disable_device(dev);
}

#include "v400pfw.h"
#include "v401pfw.h"

STATIC int __devinit
vp_download_firmware(struct vp *vp, enum vp_board board)
{
	unsigned int byte;
	unsigned char *f;
	size_t flen;
	volatile unsigned long *data;
	unsigned long timeout;

	switch (board) {
	case V400P:
	case V400PSS7:
		f = (unsigned char *) v400pfw;
		flen = sizeof(v400pfw);
		break;
	case V401PT:
	case V401PE:
		f = (unsigned char *) v401pfw;
		flen = sizeof(v401pfw);
		break;
	default:
		return (-EIO);
	};

	data = (volatile unsigned long *) &vp->plx[GPIOC];
	*data |= GPIO_WRITE;
	*data &= ~GPIO_PROGRAM;
	while (*data & (GPIO_INIT | GPIO_DONE)) ;
	printd(("%s: Xilinx Firmware Load: Init and done are low\n", DRV_NAME));
	*data |= GPIO_PROGRAM;
	while (!(*data & GPIO_INIT)) ;
	printd(("%s: Xilinx Firmware Load: Init is high\n", DRV_NAME));
	*data &= ~GPIO_WRITE;
	printd(("%s: Xilinx Firmware Load: Loading\n", DRV_NAME));
	for (byte = 0; byte < flen; byte++) {
		*vp->xlb = *f++;
		if (*data & GPIO_DONE)
			break;
		if (!(*data & GPIO_INIT))
			break;
	}
	if (!(*data & GPIO_INIT)) {
		printd(("%s: ERROR: Xilinx Firmware Load: Failed\n", DRV_NAME));
		return (-EIO);
	}
	printd(("%s: Xilinx Firmware Load: Loaded %d bytes\n", DRV_NAME, byte));
	timeout = jiffies + 20 * HZ / 1000;
	while (jiffies < timeout) ;
	*data |= GPIO_WRITE;
	printd(("%s: Xilinx Firmware Load: Done\n", DRV_NAME));
	timeout = jiffies + 20 * HZ / 1000;
	while (jiffies < timeout) ;
	if (!(*data & GPIO_INIT)) {
		ptrace(("%s: ERROR: Xilinx Firmware Load: Failed\n", DRV_NAME));
		return (-EIO);
	}
	if (!(*data & GPIO_DONE)) {
		ptrace(("%s: ERROR: Xilinx Firmware Load: Failed\n", DRV_NAME));
		return (-EIO);
	}
	__printd(("%s: Xilinx Firmware Load: Successful\n", DRV_NAME));
	return (0);
}

#ifndef module_param
MODULE_PARM(loadfw, "h");
#else
module_param(loadfw, ushort, 0);
#endif
MODULE_PARM_DESC(loadfw, "Load firmware for the V401P-MX driver.");

unsigned short loadfw = 1;

#define VP_MAX_CARDS	8	/* No machines with more than 8 PCI slots? */

/* A static array of cards for speed. */
static struct vp vp_cards[VP_MAX_CARDS];

static int vp_card;

STATIC int __devinit
vp_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	int span, b, board, device, devrev, hw_flags;
	const char *name;
	struct vp *vp;

	if (vp_card >= VP_MAX_CARDS) {
		printd(("%s: ERROR: Too many cards.\n", DRV_NAME));
		return (-ENXIO);
	}
	if (!dev || !id) {
		pswerr(("Device or id is null!\n"));
		return (-EIO);
	}
	board = id->driver_data;
	name = vp_board_info[board].name;

	if (!(vp_board_info[board].hw_flags & VPF_SUPPORTED)) {
		printd(("%s: ERROR: Driver does not support %s device.\n", DRV_NAME, name));
		return (-ENXIO);
	}
	if (dev->irq < 1) {
		printd(("%s: ERROR: No IRQ allocated for %s device.\n", DRV_NAME, name));
		return (-ENXIO);
	}
	printd(("%s: %s device allocated IRQ %d\n", DRV_NAME, name, dev->irq));
	if (pci_enable_dev(dev)) {
		printd(("%s: ERROR: Could not enable %s pci device\n", DRV_NAME, name));
		return (-ENODEV);
	}
	if (!(vp = vp_init_card(vp_card))) {
		/* do not print debug messages when out of memory */
		return (-ENOMEM);
	}
	pci_set_drvdata(dev, vp);
	if ((pci_resource_flags(dev, 0) & IORESOURCE_IO)
	    || !(vp->plx_region = pci_resource_start(dev, 0))
	    || !(vp->plx_length = pci_resource_len(dev, 0))
	    || !(vp->plx = ioremap(vp->plx_region, vp->plx_length))) {
		printd(("%s: ERROR: Invalid PLX 9030 base resource\n", DRV_NAME));
		goto error_remove;
	}
	printd(("%s: plx region %ld bytes at %lx, remapped %p\n", DRV_NAME,
		vp->plx_length, vp->plx_region, vp->plx));
	if ((pci_resource_flags(dev, 2) & IORESOURCE_IO)
	    || !(vp->xll_region = pci_resource_start(dev, 2))
	    || !(vp->xll_length = pci_resource_len(dev, 2))
	    || !(vp->xll = ioremap(vp->xll_region, vp->xll_length))) {
		printd(("%s: ERROR: Invalid Xilinx 32-bit base resource\n", DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xll region %ld bytes at %lx, remapped %p\n", DRV_NAME,
		vp->xll_length, vp->xll_region, vp->xll));
	if ((pci_resource_flags(dev, 3) & IORESOURCE_IO)
	    || !(vp->xlb_region = pci_resource_start(dev, 3))
	    || !(vp->xlb_length = pci_resource_len(dev, 3))
	    || !(vp->xlb = ioremap(vp->xlb_region, vp->xlb_length))) {
		printd(("%s: ERROR: Invalid Xilinx 32-bit base resource\n", DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xlb region %ld bytes at %lx, remapped %p\n", DRV_NAME,
		vp->xlb_length, vp->xlb_region, vp->xlb));
	if (!request_mem_region(vp->plx_region, vp->plx_length, name)) {
		printd(("%s: ERROR: Unable to reserve PLX memory\n", DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xll region %lx reserved %ld bytes\n", DRV_NAME,
		vp->plx_region, vp->plx_length));
	if (!request_mem_region(vp->xll_region, vp->xll_length, name)) {
		printd(("%s: ERROR: Unable to reserve Xilinx 32-bit memory\n", DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xll region %lx reserved %ld bytes\n", DRV_NAME,
		vp->xll_region, vp->xll_length));
	if (!request_mem_region(vp->xlb_region, vp->xlb_length, name)) {
		printd(("%s: ERROR: Unable to reserve Xilinx 8-bit memory\n", DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xll region %lx reserved %ld bytes\n", DRV_NAME,
		vp->xlb_region, vp->xlb_length));
	__printd(("%s: %s card detected at 0x%lx/0x%lx irq %d\n", DRV_NAME, name, vp->xll_region,
		  vp->xlb_region, dev->irq));
	/* Only load firmware when requested to do so. */
	if (loadfw && vp_download_firmware(vp, board) != 0)
		goto error_remove;

	vp->plx[INTCSR] = 0;	/* disable interrupts */

	/* all boards have these registers regardless of framer */

	vp->xlb[SYNREG] = 0;	/* default autosync */
	vp->xlb[CTLREG] = 0;	/* */
	vp->xlb[LEDREG] = 0xff;	/* turn off leds */

	/* Note: only check the device id of the first framer of 4.  The V401P can mix E1 and T1 on
	   the same card, but I think the SLIC has to be different.  Perhaps not... */

	devrev = vp->xlb[0x0f];	/* Device id - same place for all Dallas chips. */
	device = (devrev & VP_DEV_IDMASK) >> VP_DEV_SHIFT;
	devrev &= VP_DEV_REVMASK;

	if (!(vp_device_info[device].hw_flags & VDF_SUPPORTED)) {
		printd(("%s: Unsupported framer device %s\n", DRV_NAME,
			vp_device_info[device].name));
		goto error_remove;
	}
	switch (device) {
	case VP_DEV_DS2154:
	case VP_DEV_DS21354:
	case VP_DEV_DS21554:
		switch (board) {
		case V400P:
			board = E400P;
			name = vp_board_info[board].name;
			break;
		case V400PSS7:
			board = E400PSS7;
			name = vp_board_info[board].name;
			break;
		default:
			printd(("%s: Device %s not supported for card %s\n", DRV_NAME,
				vp_board_info[board].name, vp_device_info[device].name));
			goto error_remove;
		}
	case VP_DEV_DS2152:
	case VP_DEV_DS21352:
	case VP_DEV_DS21552:
		switch (board) {
		case V400P:
			board = T400P;
			name = vp_board_info[board].name;
			break;
		case V400PSS7:
			board = T400PSS7;
			name = vp_board_info[board].name;
			break;
		default:
			printd(("%s: Device %s not supported for card %s\n", DRV_NAME,
				vp_board_info[board].name, vp_device_info[device].name));
			goto error_remove;
		}
	case VP_DEV_DS2155:
		switch (board) {
		case V401PE:
		case V401PT:
			break;
		default:
			printd(("%s: Device %s not supported for card %s\n", DRV_NAME,
				vp_board_info[board].name, vp_device_info[device].name));
			goto error_remove;
		}
	}
	vp->board = board;
	vp->device = device;
	vp->devrev = devrev;
	vp->brdname = vp_board_info[board].name;
	vp->devname = vp_device_info[device].name;
	vp->hw_flags = vp_board_info[board].hw_flags;
	vp->hw_flags |= vp_device_info[device].hw_flags;

	__printd(("%s: %s (%s Rev. %d)\n", DRV_NAME, vp->brdname, vp->devname, vp->devrev));

	switch (vp->board) {
		int word;
#if 0
		int ebuf;
#endif

	case V401PT:
	case T400P:
	case T400PSS7:
		/* setup T1 card defaults */
		vp->config = mx_default_t1_card;
		/* initialize channel values for T1 */
		for (word = 0; word < 256; word++) {
			vp->xll[word] = 0x7f7f7f7f;
#if 0
			for (ebuf = 0; ebuf < V400P_EBUFNO; ebuf++)
				vp->wbuf[(ebuf << 8) + word] = 0x7f7f7f7f;
#endif
		}
		break;
	case V401PE:
	case E400P:
	case E400PSS7:
		/* setup E1 card defaults */
		vp->config = mx_default_e1_card;
		/* initialize channel values for E1 */
		for (word = 0; word < 256; word++) {
			vp->xll[word] = 0xffffffff;
#if 0
			for (ebuf = 0; ebuf < V400P_EBUFNO; ebuf++)
				vp->wbuf[(ebuf << 8) + word] = 0xffffffff;
#endif
		}
		break;
	default:
		swerr();
		goto error_remove;
	}
	if (request_irq(dev->irq, mx_interrupt, SA_INTERRUPT | SA_SHIRQ, DRV_NAME, vp)) {
		ptrace(("%s: ERROR: Unable to request IRQ %d\n", DRV_NAME, dev->irq));
		goto error_remove;
	}
	vp->irq = dev->irq;
	printd(("%s: acquired IRQ %ld for %s card\n", DRV_NAME, vp->irq, vp->brdname));
	vp->plx[INTCSR] = PLX_INTENA;	/* enable interrupts */
	/* I don't think that we need an atomic here.  I think that hardware probes are never done
	   in parallel. */
	vp_card++;
	return (0);
      error_remove:
	vp_remove(dev);
	return (-ENODEV);
}

#ifdef CONFIG_PM
STATIC int
vp_suspend(struct pci_dev *pdev, pm_message_t state)
{
	fixme(("Write a suspend routine.\n"));
	return 0;
}
STATIC int
vp_resume(struct pci_dev *pdev)
{
	fixme(("Write a resume routine.\n"));
	return 0;
}
#endif				/* CONFIG_PM */

STATIC struct pci_driver vp_driver = {
	.name = DRV_NAME,
	.probe = vp_probe,
	.remove = __devexit_p(vp_remove),
	.id_table = &vp_pci_table,
#ifdef CONFIG_PM
	.suspend = vp_suspend,
	.resume = vp_resume,
#endif					/* CONFIG_PM */
};

/**
 * vp_pci_init: - init PCI for the V401P driver
 */
STATIC noinline __unlikely int
vp_pci_init(void)
{
	return pci_module_init(&vp_driver);
}

/**
 * vp_pci_cleanup: - cleanup PCI for the V401P driver
 */
STATIC noinline __unlikely int
vp_pci_cleanup(void)
{
	pci_unregister_driver(&vp_driver);
	return (0);
}

#ifdef LINUX

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the V401P-MX driver. (0 for allocation.)");

unsigned short major = DRV_ID;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, ushort, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the V401P-MX driver. (0 for allocation.)");

STATIC struct cdevsw vp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &v400mxinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC noinline __unlikely int
vp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&vp_cdev, major)) < 0)
		return (err);
	return (0);
}
STATIC noinline __unlikely int
vp_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&vp_cdev, major)) < 0)
		return (err);
	return (0);
};

MODULE_STATIC void __exit
v400mxterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (mx_majors[mindex]) {
			if ((err = vp_unregister_strdev(mx_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					mx_majors[mindex]);
			if (mindex)
				mx_majors[mindex] = 0;
		}
	}
	if ((err = vp_pci_cleanup()))
		cmn_err(CE_WARN, "%s: could not cleanup pci device", DRV_NAME);
	if ((err = vp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC void __init
v400mxinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = vp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		v400mxterminate();
		return (err);
	}
	if ((err = vp_pci_init())) {
		cmn_err(CE_WARN, "%s: no PCI devices found, err = %d", DRV_NAME, err);
		v400mxterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = vp_register_strdev(mx_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					mx_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				v400mxterminate();
				return (err);
			}
		}
		if (mx_majors[mindex] == 0)
			mx_majors[mindex] = err;
		if (major == 0)
			major = mx_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 */
module_init(v400mxinit);
module_exit(v400mxterminate);

#endif				/* LINUX */
