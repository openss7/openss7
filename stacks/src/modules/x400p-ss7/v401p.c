/*****************************************************************************

 @(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2008-04-29 07:11:19 $

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

 Last Modified $Date: 2008-04-29 07:11:19 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: v401p.c,v $
 Revision 0.9.2.7  2008-04-29 07:11:19  brian
 - updating headers for release

 Revision 0.9.2.6  2007/08/14 12:19:01  brian
 - GPLv3 header updates

 Revision 0.9.2.5  2007/05/25 12:19:07  brian
 - check for pm_message_t

 Revision 0.9.2.4  2007/03/25 19:00:43  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.3  2006/11/30 13:25:46  brian
 - working up driver

 Revision 0.9.2.2  2006/11/27 11:47:43  brian
 - updated CH and MX interface to Version 1.1 per strchan package

 Revision 0.9.2.1  2006/11/16 20:37:42  brian
 - added MX driver or firmware for V401P cards

 *****************************************************************************/

#ident "@(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2008-04-29 07:11:19 $"

static char const ident[] =
    "$RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2008-04-29 07:11:19 $";

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
 *  BAD, BAD, BAD -- let upper module do this if it wants
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
 *
 *  A note about local software loopback.  For local software loopback, when the upper layer module
 *  simply recycles message blocks downstream, the upper layer module need not touch the receive
 *  sample and can leave it in place in the message block recycled downstream.
 *
 *  A note about digital cross-connect.  Digital cross-connect within a span, between spans on the
 *  same card, and between spans on different cards could be performed by this driver using the MX
 *  interface.  For digital cross-connect a sample from the RX block could be copied to the next TX
 *  block providing for minimum latency; however, this is probably better performed by the upper
 *  layer module.
 */

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

static struct pci_device_id vp_pci_table[] __devinitdata = {
	{PCI_VENDOR_ID_PLX, 0x9030, PCI_ANY_ID, PCI_ANY_ID, 0, 0, PLX9030},
	{PCI_VENDOR_ID_PLX, 0x3001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, PLXDEVBRD},
	{PCI_VENDOR_ID_PLX, 0xD00D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, V400P},
	{PCI_VENDOR_ID_PLX, 0x0557, PCI_ANY_ID, PCI_ANY_ID, 0, 0, V400PSS7},
	{PCI_VENDOR_ID_PLX, 0xD33D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, V401PT},
	{PCI_VENDOR_ID_PLX, 0xD44D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, V401PE},
	{0,}
};

/* Map from Tormenta channel to T1 time slot (less 1). */
static int vp_t1_slot_map[] = {
	-1, 0, 1, 2,
	-1, 3, 4, 5,
	-1, 6, 7, 8,
	-1, 9, 10, 11,
	-1, 12, 13, 14,
	-1, 15, 16, 17,
	-1, 18, 19, 20,
	-1, 21, 22, 23
};

/* Map from Tormenta channel to E1 time slot (less 1). */
static int vp_e1_slot_map[] = {
	-1, 0, 1, 2,
	3, 4, 5, 6,
	7, 8, 9, 10,
	11, 12, 13, 14,
	15, 16, 17, 18,
	19, 20, 21, 22,
	23, 24, 25, 26,
	27, 28, 29, 30
};

/* Map from T1 time slot (less 1) to Tormenta channel. */
static int vp_t1_chan_map[] = {
	1, 2, 3,
	5, 6, 7,
	9, 10, 11,
	13, 14, 15,
	17, 18, 19,
	21, 22, 23,
	25, 26, 27,
	29, 30, 31
};

/* Map from E1 time slot (less 1) to Tormenta channel. */
static int vp_e1_chan_map[] = {
	1, 2, 3, 4,
	5, 6, 7, 8,
	9, 10, 11, 12,
	13, 14, 15, 16,
	17, 18, 19, 20,
	21, 22, 23, 24,
	25, 26, 27, 28,
	29, 30, 31
};

#define VP_T1_TS_VALID_MASK	0x00ffffff	/* Mask of valid T1 time slots. */
#define VP_E1_TS_VALID_MASK	0x7fffffff	/* Mask of valid E1 time slots. */
#define VP_T1_CHAN_VALID_MASK	0xeeeeeeee	/* Mask of valid T1 Tormenta channels. */
#define VP_E1_CHAN_VALID_MASK	0xfffffffe	/* Mask of valid E1 Tormenta channels. */

unsigned modID_t modid = DRV_ID;
unsigned major_t major = DRV_ID;

/*
 *  Private structures.
 */
struct sp {
	uint32_t cmask;			/* Mask of enabled channels for this span. */
	uint32_t xmask;			/* Mask of cross-connected channels for this span. */
	ulong bpvs;
	ulong fass;
	ulong crc4;
	ulong ebit;
};
struct vp {
	struct {
		major_t major;
		minor_t minor;
	} dev;
	spinlock_t lock;
	queue_t *iq;			/* Input (read) queue. */
	queue_t *oq;			/* Output (write) queue. */
	struct tasklet_struct tasklet;
	volatile uint32_t *buf;		/* Buffer containing sinlge RX/TX block. */
	ulong xll_region;
	ulong xll_length;
	volatile uint32_t *xll;
	ulong xlb_region;
	ulong xlb_length;
	volatile uint8_t *xlb;
	ulong plx_region;
	ulong plx_length;
	volatile uint16_t *plx;
	ulong frame;			/* frame counter */
	volatile int eval_syncsrc;	/* need to reevaluate sync src */
	volatile int leds;		/* leds on the card */
	int irq;			/* card irq */
	ulong iobase;			/* card iobase */

	int **slot_map;			/* set to vp_t1_slot_map or vp_e1_slot_map */
	int **chan_map;			/* set to vp_t1_chan_map or vp_e1_chan_map */

	int blocks;			/* Number of RX/TX blocks outstanding. */
	int spans;			/* Number of spans, always 4 for current cards. */
	uint32_t smask;			/* Mask of enabled spans. */
	uint32_t cmask;			/* Mask of enabled channels (for any span). */
	uint32_t xmask;			/* Mask of cross-connected channels (for any span). */
	int txdccs;			/* Number of digital cross-connects to this card from other 
					   cards. */
	int rxdccs;			/* Number of digital cross-connects from this card to other 
					   cards. */
	struct sp spans[4];		/* Only four of 'em for now. */
	volatile uint8_t *map[32][4];	/* Digital cross-connect map from this card. */
};

static spinlock_t vp_daccs_lock = SPIN_LOCK_UNLOCKED;

#define V400P_EBUFNO	(1<<7)	/* 128k of elastic buffers per card. */

/*
 *  There are two types of Streams that can be opened on the driver: a CH stream and an MX stream.
 *
 *  A CH Stream represents a single channel, fractional E1/T1/J1, full E1/T1/J1, or multiple
 *  E1/T1/J1, as determined by the CH_ATTACH_REQ primitive, statistically multiplexed into a single
 *  Nx56kbps or Nx64kbps byte-stream.  A CH stream is considered slipage sensitive and delay
 *  insensitive.  CH streams are used for data channels (such as SS7 links or perhaps Frame Relay
 *  links).  CH streams should span multiple spans unless the spans are synchonization locked
 *  together (TXCLK comes from the same source for all spans at both ends of the span).  In fact it
 *  is questionable whether this can work at all between cards, so it should be restricted to the
 *  same card.  Data on CH Streams is transferred as a contiguous sequential stream of demultiplexed
 *  octets in out-of-place blocks that vary in size depending on the number of attached channels.
 *
 *  An MX Stream represents a single channel, set of channels in an E1/T1/J1, or multiple channels
 *  in multiple E1/T1/J1, as determined by the MX_ATTACH_REQ primitive.  Statistical multiplexing is
 *  not performed.  An MX stream is considered slipage insensitive and delay sensitive.  MX streams
 *  are used for voice channels or for data channels that can withstand a high degree of noise (such
 *  as ISDN D channels, X.25 links, but not SS7 links or perhaps Frame Relay links).  MX streams
 *  that span multiple spans will slip independently for each span and simply cannot be used for
 *  statistical multiplexing across spans: use a CH Stream if you need this.  Data on MX Streams is
 *  transferred as a 1024 byte block of in-place channel data for up to 4 spans.
 *
 *  Aside from these differences, CH and MX Streams are quite similar.
 */

/*
 *  MX Primitives sent upstream.
 */
/**
 * mx_info_ack: - send an MX_INFO_ACK primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_info_ack(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_info_ack(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_optmgmt_ack: - send an MX_OPTMGMT_ACK primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_optmgmt_ack(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_optmgmt_ack(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_ok_ack: - send an MX_OK_ACK primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_ok_ack(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_ok_ack(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_error_ack: - send an MX_ERROR_ACK primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_error_ack(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_error_ack(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_enable_con: - send an MX_ENABLE_CON primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_enable_con(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_enable_con(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_connect_con: - send an MX_CONNECT_CON primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_connect_con(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_connect_con(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_data_ind: - send an MX_DATA_IND primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_data_ind(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_data_ind(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_disconnect_ind: - send an MX_DISCONNECT_IND primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_disconnect_ind(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_disconnect_ind(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_disconnect_con: - send an MX_DISCONNECT_CON primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_disconnect_con(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_disconnect_con(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_disable_ind: - send an MX_DISABLE_IND primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_disable_ind(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_disable_ind(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/**
 * mx_disable_con: - send an MX_DISABLE_CON primitive upstream
 * @q: active queue
 * @bp: message to free or consume if successful
 * @vp: VP private structure
 */
static fastcall inline int
mx_disable_con(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_disable_con(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

static fastcall inline int
mx_event_ind(queue_t *q, mblk_t *bp, struct vp *vp)
{
}
static fastcall inline int
ch_event_ind(queue_t *q, mblk_t *bp, struct vp *vp)
{
}

/*
 *  MX Primitives received from upstream.
 */
/**
 * mx_info_req: - process MX_INFO_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static fastcall int
mx_info_req(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_info_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_optmgmt_req: - process MX_OPTMGMT_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static fastcall int
mx_optmgmt_req(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_attach_req: - process MX_ATTACH_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Requests that the stream associated with @q be attached to the specified card and span.  The
 * specified card an span must exist, and be available.
 *
 */
static fastcall int
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
static fastcall int
ch_attach_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_enable_req: - process MX_ENABLE_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static fastcall int
mx_enable_req(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_enable_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_connect_req: - process MX_CONNECT_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static fastcall int
mx_connect_req(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_connect_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_data_req: - process MX_DATA_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static fastcall int
mx_data_req(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_data_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_disconnect_req: - process MX_DISCONNECT_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static fastcall int
mx_disconnect_req(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_disconnect_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_disable_req: - process MX_DISABLE_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static fastcall int
mx_disable_req(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_disable_req(queue_t *q, mblk_t *mp)
{
}

/**
 * mx_detach_req: - process MX_DETACH_REQ primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static fastcall int
mx_detach_req(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_detach_req(queue_t *q, mblk_t *mp)
{
}

static noinline fastcall int
vp_w_proto(queue_t *q, mblk_t *mp)
{
	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t))
		return (-EMSGSIZE);
	if (MX_PRIV(q)->mx) {
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
	} else {
		switch (*(uint32_t *) mp->b_rptr) {
		case CH_INFO_REQ:
			return ch_info_req(q, mp);
		case CH_OPTMGMT_REQ:
			return ch_optmgmt_req(q, mp);
		case CH_ATTACH_REQ:
			return ch_attach_req(q, mp);
		case CH_ENABLE_REQ:
			return ch_enable_req(q, mp);
		case CH_CONNECT_REQ:
			return ch_connect_req(q, mp);
		case CH_DATA_REQ:
			return ch_data_req(q, mp);
		case CH_DISCONNECT_REQ:
			return ch_disconnect_req(q, mp);
		case CH_DISABLE_REQ:
			return ch_disable_req(q, mp);
		case CH_DETACH_REQ:
			return ch_detach_req(q, mp);
		}
	}
	return (-EPROTO);
}

static noinline fastcall int
vp_w_flush(queue_t *q, mblk_t *mp)
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

static noinline fastcall __hot_put int
vp_w_msg(queue_t *q, mblk_t *mp, int type)
{
	int rtn;

	switch (type) {
	case M_PROTO:
	case M_PCPROTO:
		rtn = vp_w_proto(q, mp);
		break;
	case M_FLUSH:
		rtn = vp_w_flush(q, mp);
		break;
	case M_IOCTL:
	case M_IOCDATA:
		/* IO controls */
		rtn = vp_w_ioctl(q, mp);
		break;
	case M_PCCTL:
	case M_CTL:
		/* inter module IO controls */
		rtn = vp_w_ctl(q, mp);
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

/**
 * vp_wput: - write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 *
 * On the write side, the upper module is responsible for its own TX block allocation.
 *
 * For a CH Stream, if there is no TX data for the current frame, the card structure of the
 * corresponding card is locked and TX data written directly to the TX buffers.  If the block is
 * consumed, it is freed, otherwise, it is queued on the write queue.  If there is data already on
 * the write queue, the block is queued (to later be written by the write side tasklet).  Each card
 * structure has a list of corresponding CH Streams from which data is fed.
 *
 * The upstream module must check for downstream flow control (on band 1).  When flow controlled,
 * the upstream module should avoid (if possible) generating further downstream data.  So, for
 * example, an HDLC upstream module could suspend transmission of data until flow control subsides.
 * If an underrun occurs, data from the last TX block for the associated channels will simply be
 * repeated, however, the underrun condition and the number of octets of underrun will be recorded
 * against the channel.  Also, in the case of an underrun condition, the high water mark (and
 * possibly the low water mark) for the write queue should be adjusted.
 *
 * For an MX Stream, if there is no TX data for the current frame, the card structure of the
 * corresponding card is locked and TX data written directly to the TX buffers.  The block is always
 * consumed.  If data has already been written for the current frame, the block is queued on the
 * write queue.  If there is data already on the write queue, the block is queued.  When there is no
 * data on the write queue after transmission, the last transmitted block is saved.  If another
 * trasnmission block does not arrive before the next TX cycle, the queued block will be repeated
 * (and the next (n-1) arriving block(s) will be discarded, but the most current one will always be
 * held for repeat).
 *
 * To reduce latency to a minimum, even for out-of-place TX blocks, the procedure keeps track of the
 * low water mark of the number of blocks that were on the write queue at the time that a TX frame
 * was generated.  If this number is non-zero and persists for a period of time (some number of TX
 * frames), one message will be discarded.  However, upper stream modules should provide in-place TX
 * block data to minimize latency.  In that way, data written to the in-place TX block will always
 * be transmitted on the next TX cycle.  If the minimum number of messages is ever the same as the
 * high water mark for the write queue, the high water mark should be increased by one message (up
 * to some reasonable maximum).
 *
 * The upstream module must check for downstream flow control (on band 1).  When flow controlled,
 * the upstream module should avoid (if possible) generating further downstream data.  If in-place
 * write data TX blocks are used, downstream congestion can be considered a reason not to send
 * another block, but data can still be filled out in the in-place TX block.
 */

static streamscall __hot_write int
vp_wput(queue_t *q, mblk_t *mp)
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
			struct vp *vp = VP_PRIV(q);
			register struct vp_tx *tx = &vp->span->tx;

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
	if (likely(vp_w_msg(q, mp, type) == 0))
		goto done;
      put_it:
	/* M_DATA messages should always be band 1. */
	assure(type != M_DATA || mp->b_band == 1);
	putq(q, mp);
	goto done;
}

/**
 * vp_wsrv: - write service procedure
 * @q: queue to service
 *
 * If the message is an M_DATA message or an M_NOTIFY message, simply chain it to the TX ISR.
 * Otherwise, process the message.  Note that if we are debugging we can make a few checks on the
 * message.
 */
static streamscall __hot_out int
vp_wsrv(queue_t *q)
{
	struct vp *vp = VP_PRIV(q);
	register struct vp_tx *tx = &vp->span->tx;
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
			if (unlikely(vp_w_msg(q, mp, type)))
				goto put_it_back;
		} while (unlikely(!!(mp = getq(q))));
	}
      done:
	return (0);
      put_it_back:
	putbq(q, mp);
	goto done;
}

/*
 * Interleaved elastic store operation: Each ISR RX and TX multiframe is a 1k area representing 8
 * frames or 1ms.  For each 4096k page in the elastic store, the first 0-1024 is a TX multiframe,
 * 1025-2048 is an RX multiframe, 2049-3072 TX, 3073-4096 RX.  Initial esballoced buffers
 * representing these ranges are also maintained in sequence (so that buffers also occupy mdbblock
 * cache page slots in the same order).  A maximum elastic buffer of 128k is allocated, representing
 * 512 multiframes.  Each card has its own elastic buffer to avoid page contention between multiple
 * processors separately servicing the interrupts for each card.  The service routine transfers an
 * M_READ esballoc'ed block for the appropriate TX block upstream followed by an M_DATA message for
 * the appropriate RX block upstream.  When the upper layer module receives the M_READ message
 * block, it fills the block and passes it as an M_DATA message downstream.  If it cannot fill the
 * block, the previously transmitted block can be repeated by zeroing the block (b_wptr <- b_rptr)
 * and passing it as an M_DATA message block downstream.  When it receives the M_DATA message block,
 * it processes the contents of the RX block and returns the block downstream as an M_READ message
 * block.
 *
 * The write put and service procedures adjust the future position of the TX block within the
 * elastic store based on the differential between the current TX block and the current RX block and
 * the knowledge of the number of TX M_READ messages outstanding.  The objective of this process is
 * to have the current TX block immediately precede the current RX block to maximize the efficiency
 * of channel data transfers from the elastic store.  Also, the used portion within the elastic
 * store can be trimmed using outstanding block information to a point for which wraps are not
 * likely to occur, keeping the used pages hotter.
 *
 * An easy algorithm to acheive this is to always set the RX message block to the block following
 * the current transmit message block.  When the card starts up, the receive must idle quiet code
 * until the first transmit message block is received on the write side.  While locked, TX and RX
 * blocks will be adjacent in the elastic store.
 *
 * Upper layer modules must never free these message blocks and must always promptly recycle M_READ
 * into M_DATA and M_DATA into M_READ.  Because these are eballoc'ed blocks, if one is freed, it
 * does not break the process; however, message block may be allocated adjacent to keep them hot.
 * It is also possible for the upper layer module to generate its own TX blocks instead of recycling
 * M_READ messages.
 */

/*
 *  Once more:
 *  There is one MX stream per card.  The read service procedure checks for RX blocks from the
 *  RX ISR.  If there are RX blocks, it esballoc's RX blocks as an M_DATA message and TX blocks as
 *  an M_READ message and passes them upstream, TX then RX.  The upper module can queue as many TX
 *  blocks as it would like.  Any TX blocks that it does not wish to fill can be freed.  TX blocks
 *  that are filled are recycled downstream as M_DATA blocks.  TX blocks must be processed in the
 *  order in which they are received.  Once RX blocks are processed, they are freed.  Upper modules
 *  must free RX blocks promptly and in the order in which they were received, and should not queue
 *  them.  When RX or TX blocks are freed, the buffer free routine advances leading or trailing edge
 *  elastic buffer pointers.  When TX blocks are freed, their contents are set to predetermined
 *  values (e.g.  the values from the last frame or an idle code).  If the upper layer module is
 *  concerned about the code, it should complete the blocks and send them downstream as M_DATA
 *  blocks.  When a TX block is received on the write side, the trailing edge pointer is advanced
 *  and the block is freed.  (The fact that the pointer was advanced is a cue to the free routine
 *  that the block has been filled.)
 */

/*
 *  Again:
 *  There is one MX stream per card.  The read service procedure first drains any queued messages
 *  subject to flow control.  The read service procedure then checks for RX blocks from the
 *  RX ISR.  If there are RX blocks, it passes an esballoc'ed message block upstream subject also to
 *  flow control.  If the upstream module is flow controlled, the routine places the message blocks
 *  on its own queue (using putq with noenable in effect).  When the RX ISR runs and there are
 *  RX blocks to be processed it uses qenable to enable the q.  The queue may also become
 *  backenabled by the upstream module when congestion clears.
 *
 *  The upstream module (acting as a multiplexing driver) should perform the following actions:
 *  starting with delay sensitive upper multiplex streams (i.e. voice channels), if the frame can be
 *  passed to a stream on the upper multiplex considering flow control, it should duplicate the
 *  message block and pass the duplicate upstream for processing.  If flow control is in effect, the
 *  upper stream should simply be skipped.  Then for non-delay sensitive upper streams (i.e. data
 *  channels), the same approach is taken.  It then frees the original which releases the RX block
 *  back to the RX ISR.  On the upper multiplex, for delay sensitive streams, the read put procedure
 *  should simply copy the data into a single RX block an signal a timing pulse upstream.  Here RX
 *  data overwrites any RX data from the last RX block processed.  For delay sensitive upper
 *  streams, the read put procedure should copy necessary RX data into a private RX block queued
 *  upstream and then free the received block and return promptly.
 *
 *  The free routine advances the received data pointer allowing the RX ISR to receive more blocks.
 *  If the received data pointer is not advanced sufficiently, the RX ISR will eventually run out of
 *  buffer.
 *
 *  The upper layer module is responsible for managing its own TX buffer and TX blocks.
 *
 *  When the RX ISR wishes to enable the queue with qenable(9), it should spawn a tasklet with
 *  the queue as an argument.  The tasklet should simply call qenable(9) followed by runqueues(9).
 *  This will allow another processor to pick up the read service procedure and run it at bottom
 *  half (runqueues(9) will simply return if it is already running or if the queue has already been
 *  scheduled for another processor.  runqueues(9) when called should always put an awoken kernel
 *  thread process back to sleep before returning.)
 */

#define V400P_BLOCK_SHIFT   10
#define V400P_BLOCK_SIZE    (1<<V400P_BLOCK_SHIFT)

/* There needs to be three pointers: head, mark and tail.  RX allocates block passing them upstream
 * and pushes the mark to the tail.  As blocks are freed, it pushes the head toward the mark.  RX
 * ISR pushes the tail but short of the head. */

/*
 *  STREAMS open and close.
 *  =======================
 */

static spinlock_t vp_lock = SPIN_LOCK_UNLOCKED;
static struct vp *vp_list = NULL;
static major_t vp_majors[CMAJORS] = { CMAJOR_0, };

static streamscall int
vp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	pl_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct vp *vp, *vpp;

	if (q->q_ptr)
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
	spin_lock_irqsave(&vp_lock, flags);
	for (vpp = &vp_list; *vpp; vpp = &(*vpp)->next) {
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
					if (++mindex >= CMAJORS || !(cmajor = vp_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&vp_lock, flags);
		return (ENXIO);
	}
	printd(("%s: opened character device %hu:%hu\n", DRV_NAME, (ushort) cmajor,
		(ushort) cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(vp = vp_alloc_mx(q, vpp, devp, crp))) {
		/* never print error messages when out of memory */
		spin_unlock_irqrestore(&vp_lock, flags);
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&vp_lock, flags);

	qprocson(q);

	return (0);
}

static streamscall int
vp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct vp *vp = VP_PRIV(q);
	pl_t flags;

	printd(("%s: closing character device %hu:%hu\n", DRV_NAME, vp->dev.cmajor,
		vp->dev.cminor));

	qprocsoff(q);

	spin_lock_irqsave(&vp_lock, flags);
	vp_free_mx(vp);
	spin_unlock_irqrestore(&vp_lock, flags);
	return (0);
}

/*
 * Bottom end.
 *
 * Works quite simple.  In the ISR copy RX data to the read buf and copy the write buf to TX data.
 * Issue the tasklet to pass the timing pulse (and esballoc'ed RX/TX block) to the upper layer
 * driver.
 */

/* Firmware */
#define GPIOC		(0x54 >> 1)	/* GPIO control register */
#define GPIO_WRITE	(0x0004000)	/* GPIO4 data */
#define GPIO_PROGRAM	(0x0020000)	/* GPIO5 data */
#define GPIO_INIT	(0x0100000)	/* GPIO6 data */
#define GPIO_DONE	(0x0800000)	/* GPIO7 data */

#define INTCSR		(0x4c >> 1)
#define PLX_INTENA	0x43

/* Xilinx */
#define SYNREG	0x400
#define	    SYNCSELF	0	/* Free run */
#define	    SYNC1	1	/* Synchronize to span 1 */
#define	    SYNC2	2	/* Synchronize to span 2 */
#define	    SYNC3	3	/* Synchronize to span 3 */
#define	    SYNC4	4	/* Synchronize to span 4 */
#define	    SYNCEXTERN	5	/* External Synchornization (Drive TCLK from MASTER) */

#define STAREG	0x400		/* Status Register */
#define	    INTENA	0x01	/* Interrupt Enabled */
#define	    INTACTIVE	0x02	/* Interrupt Active */
#define	    DINTACTIVE	0x04	/* Dallas Interrupt Active */

#define CTLREG	0x401
#define	    INTENA	0x01	/* Interrupt Enable */
#define	    OUTBIT	0x02	/* Drives "TEST1" signal ("Interrupt" outbit) */
#define	    DINTENA	0x04	/* Dallas Interrupt Enable (Allows DINT to drive INT) */
#define	    MASTER	0x08	/* External Synchronzation Enable (MASTER signal) */
#define	    E1DIV	0x10	/* Select E1 Divisor Mode (0 for T1, 1 for E1) */
#define	    REMSLB	0x20	/* Remote serial loopback (When set, TSER is driven from RSER) */
#define	    LOCSLB	0x40	/* Local serial loopback (When set, Rx buffers driven from Tx
				   buffers) */
#define	    INTACK	0x80	/* Interrupt Acknowledge */

#define LEDREG	0x402		/* R4 G4 R3 G3 R2 G2 R1 G1 */
#define TS2REG	0x403		/* When bit 0 set, drivers TEST2 pin. */
#define REVREG	0x404		/* Set bit 0 for Dallas chils later than Rev. A */

/**
 * vp_tx_tasklet: - process received block event
 * @data: caller data (vp pointer)
 *
 * The purpose of the TX tasklet is to prepare the next transmit block with data from attached CH
 * and MX Streams.  Rather than preparing data on a channel by channel basis as is done on the
 * receive side, data is prepared on a Stream by Stream basis, starting with CH Streams and ending
 * with MX Streams.
 *
 * Probably the best way to get a good impeadance match to STREAMS is to only keep one current
 * message block for transmission attached to the tx_next pointer.  When each message block is
 * consumed, it is freed and any further b_cont message blocks processed.  Once the entire message
 * is consumed, a new message is removed from the message queue (automatically causing backenabling
 * to occur upstream).  This also allows the write put procedure to simply place messages on the
 * queue in band 1.
 */
static __hot_in void
vp_tx_tasklet(unsigned long data)
{
	struct vp *vp = (struct vp *) data;
	uint8_t *buf = (uint8_t *) vp->buf + 1024;	/* TX block */
	struct cx *ch, *mx;
	uint32_t cmask;
	int span, slot, frame, byte, spans = vp->spans;
	mblk_t *mp;

	for (ch = vp->ch; ch; ch = ch->vp_next) {
		queue_t *q = ch->oq;

		for (frame = 0; frame < 1024; frame += 128) {
			for (smask = ch->smask, span = 0; span < spans; span++, smask >>= 1) {
				if (!(smask & 0x01))
					continue;
				for (cmask = ch->cmask[span], byte = frame + spans + (spans - span),
				     chan = 1; chan < 32; byte += spans, chan++, cmask >>= 1) {
					if (!(cmask & 0x01))
						continue;
				      again:
					if (!(mp = ch->tx_next)) {
						pl_t pl;

						/* Freeze the stream while manipulating the queue. */
						pl = freezestr(q);
						/* Walk the queue looking for a message for us. All 
						   data messages are placed in band 1. */
						for (mp = q->q_first; mp && mp->b_band != 1;
						     mp = mp->b_next) ;
						if (mp)
							rmvq(q, mp);
						unfreezestr(q, pl);
						if (unlikely(mp == NULL)) {
							ch->underruns++;
							continue;
						}
						ch->tx_next = mp;
					}
					if ((mp->b_rptr < mp->b_wptr)) {
						buf[byte] = *mp->b_rptr++;
					} else {
						ch->tx_next = mp->b_cont;
						freeb(mp);
						goto again;
					}
				}
			}

		}
	}
	for (mx = vp->mx; mx; mx = mx->vp_next) {
		volatile unsigned char *b_rptr;

		{
			queue_t *q = mx->oq;
			pl_t pl;

			/* Freeze stream while manipulating the queue. */
			pl = freezestr(q);
			/* Walk the queue looking for a message block for us.  All data messages
			   are placed in band 1. */
			for (mp = q->q_first; mp && mp->b_band != 1; mp = mp->b_next) ;
			if (mp)
				rmvq(q, mp);
			unfreezestr(q, pl);
			if (unlikely(mp == NULL)) {
				mx->underruns += 8;
				continue;
			}
		}
		b_rptr = mp->b_rptr;
		/* MX blocks are organized as (ts1, sp1), (ts1, sp2), ... (ts1, spN), (ts2, sp1),
		   (ts2, sp2), ... (ts2, spN), ... (ts32, sp1), (ts32, sp2), ... (ts32, spN) for
		   the firsts sample, followed by the same pattern for the second sample, up to 8
		   samples.  There are always 32 time slots to allow mixing of T1 and E1 on the
		   same MX Stream.  There are always N spans (4 for 4 span cards) where N is the
		   number of spans on the card.  Multicard MX channels are not permitted. */
		for (frame = 0; frame < (spans << 8); frame += (spans << 5)) {
			for (smask = mx->smask, span = 0; span < spans; span++, smask >>= 1) {
				if (!(smask & 0x01))
					continue;
				for (cmask = mx->cmask[span], byte = frame + spans + (spans - span),
				     chan = 1, slot = frame + span; chan < 32;
				     byte += spans, chan++, slot += ((vp->span[span].flags & VPF_E1)
								     || (chan & 0x3)) ? spans : 0,
				     cmask >>= 1) {
					if (!(cmask & 0x01))
						continue;
					wbuf[byte] = b_rptr[slot];
				}
			}
		}
		freemsg(mp);
	}
}

/**
 * vp_rx_free: - free an RX block buffer
 * @arg: client data (vp structure)
 *
 * This is called when an esballoc'ed RX block is freed.  The purpose of this free routine is to
 * simply release MX Streams from overflows.
 */
static streamscall __hot_read void
vp_rx_free(caddr_t arg)
{
	struct vp *vp = (struct vp *) arg;
	struct mx *mx;

	for (mx = vp->mx; mx; mx = mp->vp_next)
		mx->rflags &= ~0x01;	/* needs protection */
}

/**
 * vp_rx_tasklet: - process received block event
 * @data: caller data (vp pointer)
 *
 * The purpose of the RX tasklet is to process a received block and pass data to attached CH and MX
 * streams.  Each channel has one CH Stream or one MX Stream or no Streams (inactive).  For each CH
 * Stream, channel data is appended to the current receive message block (and a new one allocated if
 * necessary) subject to upstream flow control.  For each MX Stream channel data is copied into the
 * receive channel map for the Stream.  Then the RX block is considered processed.  After
 * processing, MX Streams are passed an esballoc'ed block and full CH message blocks are passed
 * upstream.  If another block has become available, the tasklet is redispatched.
 */
static __hot_in void
vp_rx_tasklet(unsigned long data)
{
	struct vp *vp = (struct vp *) data;
	uint8_t *buf = (uint8_t *) vp->buf;
	int frame, span, chan, byte;
	uint32_t smask, cmask, mmask;
	struct cx *ch, *mx;
	mblk_t *mp, *db;

	/* Process all CH first. */
	for (frame = 0; frame < 1024; frame += 128) {
		for (smask = vp->smask, span = 0; smask && span < vp->spans; span++, smask >>= 1) {
			if (!(smask & 0x01))
				continue;
			for (cmask = vp->span[span].cmask, byte =
			     frame + 4 + (vp->spans - span), chan = 1; chan < 32;
			     byte += 4, chan++, cmask <<= 1) {
				if (!(cmask & 0x01))
					continue;
				ch = vp->cxmap[span][chan];
				/* process byte for CH */
				if (likely(!!(mp = ch->rx_next))) {
					if (likely(mp->b_wptr < mp->b_datap->db_lim)) {
						*mp->b_wptr++ = buf[byte];
						continue;
					}
					if (unlikely(!canput(ch->iq))) {
						ch->overflows++;
						continue;
					}
					/* Probably the best way to issue CH blocks is to call
					   putq() for the read queue and let the service procedure
					   pass them along at a later time when convenient. */
					putq(ch->iq, mp);
					ch->rx_next = NULL;
				}
				if (unlikely(!(mp = allocb(FASTBUF, BPRI_HI)))) {
					ch->overflows++;
					continue;
				}
				ch->rx_next = mp;
				*mp->b_wptr++ = buf[byte];
			}
		}
	}
	/* Issue MX blocks next. Probably the best way to handle MX blocks is to esballoc one, dup
	   the rest, and then free the original.  Issue them directly to the upper module put
	   procedure from here for minium latency to the upper layer.  Each MX Stream gets the
	   entire RX/TX block in Tormenta channel map configuration regardless of how man spans or
	   channels to which it has subscribed. */
	db = esballoc(vp->buf, 2048, BPRI_HI, &vp_rx_free);
	db->b_wptr = db->b_rptr + 1024;
	for (mx = vp->mx; mx; mx = mx->vp_next) {
		if (!(mx->mmask) || (mx->rflags & 0x01))
			continue;
		if (bcanput(mx->iq, 1) && (mp = dupb(db))) {
			mx->rflags |= 0x01;
			mp->b_band = 1;
			put(mx->iq, mp);
			continue;
		}
		mx->overflows += 8;
	}
	freeb(db);
}

/**
 * vp_free: free a RX block buffer
 * @arg: client data (vp structure)
 *
 * This is called when an RX/TX block is freed.  The argument is a pointer to the vp structure
 * associated with the RX/TX block.  The purpose of this free routine is to simply track
 * overrun/underrun conditions and to provide simple on/off flow control.
 */
static streamscall __hot_read void
vp_free(caddr_t arg)
{
	struct vp *vp = (struct vp *) arg;
	unsigned long flags;

	spin_lock_irqsave(&vp->lock, flags);
	vp->blocks = 0;
	spin_unlock_irqrestore(&vp->lock, flags);
}

/**
 * vp_tasklet: process received block event
 * @data: caller data (vp pointer)
 *
 * This tasklet is invoked by the ISR whenever there is an RX/TX block available for processing.  It
 * esballoc's a message block for the RX/TX block and passes it upstream.  The ramification is that
 * the read queues run at bottom half (which is desirable).  The only double buffering that is done
 * in the ISR to move mapped IO memory to local memory for speed of random access.  The free routine
 * simply tracks an outstanding block.  The ISR checks for overflows/underruns.  If we cannot
 * allocate a message block we simply wait for the next run (and an overflow/underrun occurs).
 * Hopefully a hot freed message block is waiting for us.
 *
 * Yet another approach.  Providing
 */
static void
vp_tasklet(unsigned long data)
{
	freertn vp_freertn = { &vp_free, (caddr_t) data };
	struct vp *vp = (struct vp *) data;
	mblk_t *mp;

	if (likely(!!(mp = esballoc(vp->buf, V400P_BLOCK_SIZE << 1, BPRI_HI, &vp_freertn)))) {
		/* We do not check for flow control because only one message can be outstanding. */
		putnext(vp->iq, mp);
	} else {
		unsigned long flags;

		spin_lock_irqsave(&vp->lock, flags);
		vp->overflows += 8;
		vp->underruns += 8;
		vp->blocks = 0;
		spin_unlock_irqrestore(&vp->lock, flags);
	}
}

/**
 * vp_cross_connect: - peform a cross-connect between channels
 * @addr1: card, span and base channel, first
 * @addr2: card, span and base channel, second
 * @chans: channel map
 *
 * Add a both-way cross-connect between a group of channels.  The cross-connect will begin on the
 * next RX cycle for either card.  This function checks if there is already a cross-connect
 * specified for any channel.  It does not, however, check if any of the channels is active.  If a
 * channel is active, the RX data will still be delivered to the channel, but TX data will be
 * ignored for the channel.  This permits establishment of monitoring taps.  Two channels can be
 * opened, attached, cross-connected and then enabled.  RX data for each channel will appear on
 * each channel.  Two MX streams can be opened, attached, cross-connected.  Then individual channels
 * can be enabled.  This can easily satisfy CALEA type requirements.
 *
 * The cross-connect map is laid out as an array of 32 arrays of 4 pointers.  The first array index
 * is the channel number, the second array index the span number.  When the pointer is null, there
 * is no cross-connect for the channel.  When any pointer for the same channel in any span is
 * non-NULL, the corresponding bit is set in vp->xmask.  When set, a pointer points to the first
 * byte corresponding to the output channel in the appropriate output TX block.
 */
static noinline __unlikely int
vp_cross_connect(uint32_t addr1, uint32_t addr2, uint32_t chans)
{
	int card1 = (addr1 >> 16) & 0x0ff;
	int span1 = (addr1 >> 8) & 0x0ff;
	int chan1 = (addr1 >> 0) & 0x0ff;
	int card2 = (addr2 >> 16) & 0x0ff;
	int span2 = (addr2 >> 8) & 0x0ff;
	int chan2 = (addr2 >> 0) & 0x0ff;
	unsigned long flags;
	uint32_t cmask;
	int c1, c2;
	int err = 0;

	if (card1 == card2) {
		/* same card cross-connects are easier */
		struct vp *vp = &cards[card1];

		spin_lock_irqsave(&vp->lock, flags);
		do {
			/* check first */
			for (cmask = chans, c1 = chan1, c2 = chan2; cmask; c1++, c2++, cmask >>= 1) {
				if (cmask & 0x01) {
					if (vp->map[c1][span1] || vp->map[c2][span2]) {
						err = -EINVAL;
						break;
					}
				}
			}
			if (err)
				break;
			for (cmask = chans, c1 = chan1, c2 = chan2; cmask; c1++, c2++, cmask >>= 1) {
				if (cmask & 0x01) {
					vp->map[c1][span1] = (uint8_t *) vp->buf[c2 + 256] + span2;
					vp->xmask |= (1 << c1);
					vp->map[c2][span2] = (uint8_t *) vp->buf[c1 + 256] + span1;
					vp->xmask |= (1 << c2);
				}
			}
		} while (0);
		spin_unlock_irqrestore(&vp->lock, flags);
	} else {
		struct vp *vp1 = &cards[card1];
		struct vp *vp2 = &cards[card2];

		if (vp1 < vp2) {
			spin_lock_irqsave(&vp1->lock, flags);
			spin_lock(&vp2->lock);
		} else {
			spin_lock_irqsave(&vp2->lock, flags);
			spin_lock(&vp1->lock);
		}
		do {
			for (cmask = chans, c1 = chan1, c2 = chan2; cmask; c1++, c2++, cmask >>= 1) {
				if (cmask & 0x01) {
					if (vp1->map[c1][span1] || vp2->map[c2][span2]) {
						err = -EINVAL;
						break;
					}
				}
			}
			if (err)
				break;
			for (cmask = chans, c1 = chan1, c2 = chan2; cmask; c1++, c2++, cmask >>= 1) {
				if (cmask & 0x01) {
					vp1->map[c1][span1] =
					    (uint8_t *) vp2->buf[c2 + 256] + span2;
					vp1->xmask |= (1 << c1);
					vp1->rxdccs++;
					vp2->txdccs++;
					vp2->map[c2][span2] =
					    (uint8_t *) vp1->buf[c1 + 256] + span1;
					vp2->xmask |= (1 << c2);
					vp2->rxdccs++;
					vp1->txdccs++;
				}
			}
		} while (0);
		if (vp1 < vp2) {
			spin_unlock(&vp2->lock);
			spin_unlock_irqrestore(&vp1->lock, flags);
		} else {
			spin_unlock(&vp1->lock);
			spin_unlock_irqrestore(&vp2->lock, flags);
		}
	}
	return (err);
}

/**
 * vp_cross_disconnect: - remove a cross-connect between channels
 * @addr1: card, span and base channel, first
 * @addr2: card, span and base channel, second
 * @chans: channel map
 *
 * Remove a both-way cross-connect between a group of channels.
 */
static noinline __unlikely int
vp_cross_disconnect(uint32_t addr1, uint32_t addr2, uint32_t chans)
{
	int card1 = (addr1 >> 16) & 0x0ff;
	int span1 = (addr1 >> 8) & 0x0ff;
	int chan1 = (addr1 >> 0) & 0x0ff;
	int card2 = (addr2 >> 16) & 0x0ff;
	int span2 = (addr2 >> 8) & 0x0ff;
	int chan2 = (addr2 >> 0) & 0x0ff;
	unsigned long flags;
	uint32_t cmask;
	int c1, c2;
	int err = 0;

	if (card1 == card2) {
		/* same card cross-connects are easier */
		struct vp *vp = &cards[card1];

		spin_lock_irqsave(&vp->lock, flags);
		do {
			/* check first */
			for (cmask = chans, c1 = chan1, c2 = chan2; cmask; c1++, c2++, cmask >>= 1) {
				if (cmask & 0x01) {
					if (!vp->map[c1][span1] || !vp->map[c2][span2]) {
						err = -EINVAL;
						break;
					}
				}
			}
			if (err)
				break;
			for (cmask = chans, c1 = chan1, c2 = chan2; cmask; c1++, c2++, cmask >>= 1) {
				if (cmask & 0x01) {
					long mapused;
					int span;

					vp->map[c1][span1] = NULL;
					mapused = 0;
					for (span = 0; span < vp->spans; span++)
						mapused |= (long) vp->map[c1][span];
					if (!mapused)
						vp->xmask &= ~(1 << c1);
					vp->map[c2][span2] = NULL;
					mapused = 0;
					for (span = 0; span < vp->spans; span++)
						mapused |= (long) vp->map[c2][span];
					if (!mapused)
						vp->xmask &= ~(1 << c2);
				}
			}
		} while (0);
		spin_unlock_irqrestore(&vp->lock, flags);
	} else {
		struct vp *vp1 = &cards[card1];
		struct vp *vp2 = &cards[card2];

		if (vp1 < vp2) {
			spin_lock_irqsave(&vp1->lock, flags);
			spin_lock(&vp2->lock);
		} else {
			spin_lock_irqsave(&vp2->lock, flags);
			spin_lock(&vp1->lock);
		}
		do {
			for (cmask = chans, c1 = chan1, c2 = chan2; cmask; c1++, c2++, cmask >>= 1) {
				if (cmask & 0x01) {
					if (!vp1->map[c1][span1] || !vp2->map[c2][span2]) {
						err = -EINVAL;
						break;
					}
				}
			}
			if (err)
				break;
			for (cmask = chans, c1 = chan1, c2 = chan2; cmask; c1++, c2++, cmask >>= 1) {
				if (cmask & 0x01) {
					long mapused;
					int span;

					vp1->map[c1][span1] = NULL;
					mapused = 0;
					for (span = 0; span < vp1->spans; span++)
						mapused |= (long) vp1->map[c1][span];
					if (!mapused)
						vp1->xmask &= ~(1 << c1);
					vp1->rxdccs--;
					vp2->txdccs--;
					vp2->map[c2][span2] = NULL;
					mapused = 0;
					for (span = 0; span < vp2->spans; span++)
						mapused |= (long) vp2->map[c2][span];
					if (!mapused)
						vp2->xmask &= ~(1 << c2);
					vp2->rxdccs--;
					vp1->txdccs--;
				}
			}
		} while (0);
		if (vp1 < vp2) {
			spin_unlock(&vp2->lock);
			spin_unlock_irqrestore(&vp1->lock, flags);
		} else {
			spin_unlock(&vp1->lock);
			spin_unlock_irqrestore(&vp2->lock, flags);
		}
	}
	return (err);
}

/**
 * vp_daccs: - perform within-card fast digital cross-connect
 * @vp: card structure
 *
 * Both same-card and card-to-card cross-connect is performed here.  To avoid slipage across card to
 * card cross-connect, a spin lock is taken to protect the state of all TX blocks while performing
 * the cross-connect.  The ramifications of this is that same-card cross-connect will be ultra low
 * latency (same cycle 1ms latency), where as card-to-card cross-connect could delay one cycle
 * (minimum 1ms, maximum 2ms latency).  It is not expected that cards for different drivers be mixed
 * in a single system, because they would have to perform cross-connect one layer up.  This means
 * that each cross-connect specification consists of a txbuf pointer offset to the channel within
 * the txbuf to be written.  For 4 cards, our rxbufs and txbufs are within 2 (4k) kernel, so cache
 * thrashing should not be a terrible problem.
 *
 * The cross-connect map is laid out as an array of 32 arrays of 4 pointers.  The first array index
 * is the channel number, the second array index the span number.  When the pointer is null, there
 * is no cross-connect for the channel.  When any pointer for the same channel in any span is
 * non-NULL, the corresponding bit is set in vp->xmask.  When set, a pointer points to the first
 * byte corresponding to the output channel in the appropriate output TX block.
 *
 * If rxdccs is non-zero then this card is cross-connecting RX channels to another card and must
 * take the global daccs spin lock to keep another processor from transmitting another card's TX
 * block while this card is building it.  rxdccs is zero when this card only has digital
 * cross-connect within the same card.  Any process changing rxdccs must irq lock the structure
 * first.  When rxdccs is non-zero the txdccs of the other card is also non-zero.  When installing a
 * cross-connect map, both the sending and receiving structure must be locked (in card address
 * order) and then maps and rxdccs and txdccs members can be modified and the locks released.  It is
 * atypical to install half-cross-connects.  Typically the RX and TX directions are connected.  When
 * mapping between cards, both cards will typically has a non-zero rxdccs and txdccs and a channel
 * map entry will be created for each card.  When mapping within the same card, neither rxdccs nor
 * txdccs is adjusted.
 */
static noinline fastcall __hot void
vp_daccs(struct vp *vp)
{
	uint32_t xmask_save;

	if (unlikely((xmask_save = vp->xmask))) {
		register uint32_t *buf = vp->buf;
		register int byte, slot, chan;
		register uint32_t xmask, xmask_save = vp->xmask;
		register uint8_t **map = vp->map;
		int rxdccs = vp->rxdccs;

		if (rxdccs)
			spin_lock(&vp_daccs_lock);
		for (byte = 0; byte < 1024; byte += 128) {
			for (xmask = xmask_save, slot = (byte >> 2) + 1, chan = 1;
			     chan < 32; slot++, chan++, xmask <<= 1) {
				if (xmask & 0x01) {
					uint32_t data = buf[slot];
					uint8_t *dest;

					if ((dest = map[chan][0]))
						dest[byte] = data;
					data >>= 8;
					if ((dest = map[chan][1]))
						dest[byte] = data;
					data >>= 8;
					if ((dest = map[chan][2]))
						dest[byte] = data;
					data >>= 8;
					if ((dest = map[chan][3]))
						dest[byte] = data;
				}
			}
		}
		if (rxdccs)
			spin_unlock(&vp_daccs_lock);
	}
}

/**
 * vp_rxtx_burst: - do a burst rx/tx transfer
 * @vp: card structure
 *
 * We have the ability here to perform some unique local serial loopback for testing.  We can set
 * LOCSLB which will feed RSER from TSER (anything written to xll (for valid channels) will be
 * read from xll); however, for testing we could swap bytes on read an effectively connect span 1 to
 * span 4 and span 2 to span 3.  This could allow some loopback testing without installing a
 * physical loopback cable.  However, this can be done just as easily by the upper module.
 *
 * If txdccs is non-zero then another card is cross-connecting to this card's TX buffer and the
 * globcal daccs spin lock must be aquired to keep another processor from writing to the TX buffer
 * while this card is transferring it (leading to mysterious slippages).  Any process changing
 * txdccs must irq lock the structure first.
 */
static inline fastcall __hot void
vp_rxtx_burst(struct vp *vp)
{
	uint32_t cmask_save;

	if (unlikely(!(cmask_save = vp->cmask | vp->xmask)))
		goto done;

	if (likely(!vp->blocks)) {
		register volatile uint32_t *xll = vp->xll;
		register uint32_t *buf = vp->buf;

		{
			register int word, slot;
			register uint32_t cmask;

			/* first half of buffer is receive */
			for (word = 0; word < 256; word += 32) {
				prefetchw(&buf[word + 32]);
				for (cmask = cmask_save, slot = word + 1;
				     cmask && slot < word + 32; slot++, cmask >>= 1)
					if (cmask & 0x01)	/* skip disabled channels */
						buf[slot] = xll[slot];
			}
		}
		tasklet_hi_schedule(&vp->rx_tasklet);
		buf += 256;
		/* perform digital cross-connect if necessary */
		if (vp->xmask)
			vp_daccs(vp);
		{
			register int word, slot;
			register uint32_t cmask;
			int txdccs = vp->txdccs;

			/* second half of buffer is transmit */
			if (txdccs)
				spin_lock(&vp_daccs_lock);
			for (word = 0; word < 256; word += 32) {
				prefetch(&buf[word + 32]);
				for (cmask = cmask_save, slot = word + 1;
				     cmask && slot < word + 32; slot++, cmask >>= 1)
					if (cmask & 0x01)	/* skip disabled channels */
						xll[slot] = buf[slot];
			}
			if (txdccs)
				spin_unlock(&vp_daccs_lock);
		}
		spin_lock(&vp->lock);
		vp->blocks++;	/* keep track of outstanding blocks */
		spin_unlock(&vp->lock);
		tasklet_hi_schedule(&vp->tx_tasklet);
	} else {
		spin_lock(&vp->lock);
		vp->underruns += 8;
		vp->overflows += 8;
		spin_unlock(&vp->lock);
	}
      done:
	return;
}

/**
 * vp_alarms: - process alarm status for one span
 * @vp: card structure
 * @span: span number
 *
 * Want this function to be fast, but we want it separate from the main loop because it is only
 * executed 4 times in 64 cycles.
 */
static noinline fastcall __hot void
vp_alarms(register struct vp *vp, register int span)
{
}

#define VP_STATS(vp, reg) (((uint)vp->xlb[reg+1]) + ((uint)vp->xlb[reg] << 8))
#define VP_STATS_CRC(vp, reg) (((uint)vp->xlb[reg+1]) + (((uint)vp->xlb[reg] & 0x03) << 8))
#define VP_STATS_FAS(vp, reg) (((uint)vp->xlb[reg+2]>>2) + (((uint)vp->xlb[reg] & 0x3c) << 6))

/**
 * vp_bpv: - accumulate bipolar violations per span
 * @vp: card structure
 *
 * Want this function to be fast, but we want it separate from the main loop because it is only
 * executed 1 time in 1000 cycles.  The purpose of this function is to collect span statistics for
 * each span for the last one second interval.
 */
static noinline fastcall __hot void
vp_bpv(register struct vp *vp)
{
	int base, smask = vp->smask;
	struct sp *sp;

	for (sp = vp->spans, base = 0; smask && base < (vp->spans << 8);
	     sp++, base += 256, smask >>= 1)
		if (smask & 0x01) {
			/* Hopefully the compiler is smart enough to unroll this. */
			switch (vp->board) {
			case E400P:
			case E400PSS7:
				if (crc4) {
					sp->crc4 += VP_STATS_CRC(vp, base + 0x02);
					sp->ebit += VP_STATS_CRC(vp, base + 0x04);
				}
				sp->fass += VP_STATS_FAS(vp, base + 0x02);
				sp->bpvs += VP_STATS(vp, base + 0x00);
				continue;
			case T400P:
			case T400PSS7:
				sp->bpvs += VP_STATS(vp, base + 0x23);
				continue;
			case V401PE:
				if (crc4) {
					sp->crc4 += VP_STATS(vp, base + 0x44);
					sp->ebit += VP_STATS(vp, base + 0x48);
				}
				sp->fass += VP_STATS(vp, base + 0x46);
				sp->bpvs += VP_STATS(vp, base + 0x42);
				continue;
			case V401PT:
				sp->bpvs += VP_STATS(vp, base + 0x42);
				continue;
			default:
				return;
			}
		}
}

static noinline fastcall __hot void
vp_eval_syncsrc(register struct vp *vp)
{
}

/**
 * vp_t1_interrupt: - process interrupt on an E1 card
 * @irq: interrupt number (useless)
 * @dev_id: client data (vp structure)
 * @regs: regsiters at interrupt (useless)
 *
 * Simply copy the RX data to the read buffer and write buffer to the TX data.  However, we are a
 * little bit smarter than in the X400P-SS7 driver in that we only copy channels that are enabled
 * in some span.
 *
 * Locking is to keep another processor from altering the vp structure while this processor is
 * servicing an interrupt for the same card.
 */
static __hot irqreturn_t
vp_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	register struct vp *vp = (struct vp *) dev_id;

	spin_lock(&vp->lock);
	if (vp->xlb[STAREG] & INTACTIVE) {
		/* ctlreg can contain INTENA | E1DIV | MASTER | REMSLB | LOCSLB */
		vp->xlb[CTLREG] = (vp->ctlreg | OUTBIT | INTACK);

		vp_rxtx_burst(vp);

		if (unlikely((vp->frame & 0x1e7) == 400)) {
			register int span;

			/* frame 400/512, 408/512, 416/512 and 424/512 */
			if (vp->smask & (1 << (span = ((vp->frame & 0x18) >> 3))))
				vp_alarms(vp, span);
		}
		/* accumulate bipolar violations every 1.000000 seconds */
		if (unlikely((vp->bpv_timer & 0x1fff) == (vp->frame & 0x1fff))) {
			vp->bpv_timer = vp->frame + 8000;
			vp_bpv(vp);
		}
		if (unlikely(xchg(&vp->eval_syncsrc, 0)))
			vp_eval_syncsrc(vp);

		vp->frame += 8;	/* Eight E1/T1/J1 frames per RX/TX block (interrupt). */
		/* clear OUTBIT and INTACK */
		vp->xlb[CTLREG] = (vp->ctlreg);
	}
	spin_unlock(&vp->lock);
	return (irqreturn_t) (IRQ_NONE);
}

/*
 *  PCI initialization and removal.
 *  ===============================
 */

static void __devexit
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

static int __devinit
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

unsigned short loadfw = 1;

#define VP_MAX_CARDS	8	/* No machines with more than 8 PCI slots? */

/* A static array of cards for speed. */
static struct vp vp_cards[VP_MAX_CARDS];

static int vp_card;

static int __devinit
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
		vp->config = vp_default_t1_card;
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
		vp->config = vp_default_e1_card;
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
	if (request_irq(dev->irq, vp_interrupt, SA_INTERRUPT | SA_SHIRQ, DRV_NAME, vp)) {
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
#ifndef HAVE_KTYPE_PM_MESSAGE_T
typedef u32 pm_message_t;
#endif
static int
vp_suspend(struct pci_dev *pdev, pm_message_t state)
{
	fixme(("Write a suspend routine.\n"));
	return 0;
}
static int
vp_resume(struct pci_dev *pdev)
{
	fixme(("Write a resume routine.\n"));
	return 0;
}
#endif				/* CONFIG_PM */

static struct pci_driver vp_driver = {
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
static noinline __unlikely int
vp_pci_init(void)
{
	return pci_module_init(&vp_driver);
}

/**
 * vp_pci_cleanup: - cleanup PCI for the V401P driver
 */
static noinline __unlikely int
vp_pci_cleanup(void)
{
	pci_unregister_driver(&vp_driver);
	return (0);
}

#ifdef LINUX
/*
 *  Linux registration
 */
#ifdef module_param
module_param(modid, modID_t, 0444);
module_param(major, major_t, 0444);
module_param(loadfw, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
MODULE_PARM(major, "d");
MODULE_PARM(loadfw, "h");
#endif				/* module_param */

MODULE_PARM_DESC(modid, "Module ID for the V401P-MX driver. (0 for allocation.)");
MODULE_PARM_DESC(major, "Device number for the V401P-MX driver. (0 for allocation.)");
MODULE_PARM_DESC(loadfw, "Load firmware for the V401P-MX driver.");

/*
 * Streams definitions.
 */

static struct module_info vp_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat vp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat vp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit vp_rinit = {
	.qi_putp = NULL,
	.qi_srvp = NULL,
	.qi_qopen = vp_qopen,
	.qi_qclose = vp_qclose,
	.qi_minfo = &vp_minfo,
	.qi_mstat = &vp_rstat,
};

static struct qinit vp_winit = {
	.qi_putp = vp_wput,
	.qi_srvp = vp_wsrv,
	.qi_minfo = &vp_minfo,
	.qi_mstat = &vp_wstat,
};

static struct streamtab v400mxinfo = {
	.st_rdinit = &vp_rinit,
	.st_wrinit = &vp_winit,
};

static struct cdevsw vp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &v400mxinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static noinline __unlikely int
vp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&vp_cdev, major)) < 0)
		return (err);
	return (0);
}

static noinline __unlikely int
vp_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&vp_cdev, major)) < 0)
		return (err);
	return (0);
}

MODULE_STATIC void __exit
v400mxterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (vp_majors[mindex]) {
			if ((err = vp_unregister_strdev(vp_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					vp_majors[mindex]);
			if (mindex)
				vp_majors[mindex] = 0;
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
		if ((err = vp_register_strdev(vp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					vp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				v400mxterminate();
				return (err);
			}
		}
		if (vp_majors[mindex] == 0)
			vp_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(vp_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = vp_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 */
module_init(v400mxinit);
module_exit(v400mxterminate);

#endif				/* LINUX */
