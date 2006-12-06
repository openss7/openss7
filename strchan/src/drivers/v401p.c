/*****************************************************************************

 @(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/12/06 11:26:10 $

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

 Last Modified $Date: 2006/12/06 11:26:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: v401p.c,v $
 Revision 0.9.2.4  2006/12/06 11:26:10  brian
 - current development updates

 Revision 0.9.2.3  2006/11/30 13:05:25  brian
 - checking in working copies

 Revision 0.9.2.2  2006/10/19 10:37:24  brian
 - working up drivers and modules

 Revision 0.9.2.1  2006/10/14 06:37:27  brian
 - added manpages, module, drivers, headers from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/12/06 11:26:10 $"

static char const ident[] =
    "$RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/12/06 11:26:10 $";

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
STATIC struct {
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

STATIC struct {
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

/* Map from Tormenta channel to T1 time slot (less 1). */
STATIC int vp_t1_slot_map[] = {
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
STATIC int vp_e1_slot_map[] = {
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
STATIC int vp_t1_chan_map[] = {
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
STATIC int vp_e1_chan_map[] = {
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

/*
 *  Private structures.
 */
struct ch;
struct mx;
struct sp;
struct vp;

struct ch {
	int mid;			/* Module identifier */
	int sid;			/* Stream identifier */
	int lock;			/* lock word */
	int flags;			/* flags word */
	queue_t *rq;			/* read queue (used by tasklet) */
	queue_t *wq;			/* write queue (used by tasklet) */
	bcid_t bid;			/* bufcall identifier */
	struct CH_info_ack info;	/* information */

	uint32_t addr;			/* address */
	uint32_t smask;			/* span mask, 4 bits now, 8 bits later */
	uint32_t cmask;			/* overall channel mask (OR of cmasks) */
	uint32_t cmasks[8];		/* channel mask, 4 for now, 8 later */
	struct CH_parms_circuit parm;	/* parameters */

	int spans;			/* Number of spans in span mask (0-8) */
	int channels;			/* Number of channels in channel masks (0-31) */

	struct ch_config config;	/* configuration */
	struct ch_stats stats;		/* statistics */
	struct ch_statem state;		/* state machine */
	struct ch_notify notify;	/* notification events */

	struct {
		struct vp *vp;		/* VP card to which we are attached */
		struct ch *next;	/* VP card attach list linkage */
		struct ch **prev;	/* VP card attach list linkage */
	} a;
	struct {
		struct vp *vp;		/* VP card to which we are connected */
		struct ch *next;	/* VP card connect list linkage */
		struct ch **prev;	/* VP card connect list linkage */
	} c;
	struct {
		struct vp *vp;		/* VP card to which we are cross-connected */
		struct ch *next;	/* VP card cross-connect list linkage */
		struct ch **prev;	/* VP card cross-connect list linkage */
	} x;

	/* statistics */
	struct {
		ulong overflows;
		ulong underruns;
		mblk_t *nxt;
	} rx;
	struct {
		ulong overflows;
		ulong underruns;
		mblk_t *nxt;
	} tx;
};

struct mx {
	int mid;			/* Module identifier */
	int sid;			/* Stream identifier */
	int lock;			/* lock word */
	int flags;			/* flags word */
	queue_t *rq;			/* read queue (used by tasklet) */
	queue_t *wq;			/* write queue (used by tasklet) */
	bcid_t bid;			/* bufcall identifier */
	struct MX_info_ack info;	/* information */

	uint32_t addr;			/* address */
	uint32_t smask;			/* span mask, 4 bits now, 8 bits later */
	uint32_t cmask;			/* overall channel mask (OR of cmasks) */
	uint32_t cmasks[8];		/* channel mask, 4 for now, 8 later */
	struct MX_parms_circuit parm;	/* parameters */

	int spans;			/* Number of spans in span mask (0-8) */
	int channels;			/* Number of channels in channel masks (0-31) */

	struct mx_config config;	/* configuration */
	struct mx_stats stats;		/* statistics */
	struct mx_statem state;		/* state machine */
	struct mx_notify notify;	/* notification events */

	struct {
		struct vp *vp;		/* VP card to which we are attached */
		struct mx *next;	/* VP card attach list linkage */
		struct mx **prev;	/* VP card attach list linkage */
	} a;
	struct {
		struct vp *vp;		/* VP card to which we are connected */
		struct mx *next;	/* VP card connect list linkage */
		struct mx **prev;	/* VP card connect list linkage */
	} c;
	struct {
		struct vp *vp;		/* VP card to which we are cross-connected */
		struct mx *next;	/* VP card cross-connect list linkage */
		struct mx **prev;	/* VP card cross-connect list linkage */
	} x;

	/* statistics */
	struct {
		ulong overflows;
		ulong underruns;
		mblk_t *nxt;
	} rx;
	struct {
		ulong overflows;
		ulong underruns;
		mblk_t *nxt;
	} tx;
};

/* lock word bits */
enum {
	MXB_LOCK = 0,
	MXB_RX,
	MXB_TX,
};

#define MXF_CONNECTED	0x080	/* set when the MX Stream is connected */

#define MX_PRIV(__q) ((struct mx *)((__q)->q_ptr))

struct sp {
	uint32_t cmask;			/* Mask of enabled channels for this span. */
	uint32_t xmask;			/* Mask of cross-connected channels for this span. */
	ulong bpvs;
	ulong fass;
	ulong crc4;
	ulong ebit;
};

struct vp {
	uint board;
	uint device;
	uint devrev;
	uint hw_flags;
	const char *brdname;
	const char *devname;
	spinlock_t lock;
	uint flags;
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

	struct {
		struct ch *attached;	/* attached ch list */
		struct ch *connects;	/* connected ch list */
		struct ch *xconnect;	/* cross-connected ch list */
	} ch;

	struct {
		struct mx *attached;	/* attached mx list */
		struct mx *connects;	/* connected mx list */
		struct mx *xconnect;	/* cross-connected mx list */
	} mx;

	int blocks;			/* Number of RX/TX blocks outstanding. */
	int spans;			/* Number of spans, always 4 for current cards. */
	int channels;			/* Number of channels per span: 24 or 31 for current cards. 
					 */
	uint32_t smask;			/* Mask of enabled spans. */
	uint32_t cmask;			/* Mask of enabled channels (for any span). */
	uint32_t xmask;			/* Mask of cross-connected channels (for any span). */
	int txdccs;			/* Number of digital cross-connect to this card from other
					   cards. */
	int rxdccs;			/* Number of digital cross-connect from this card to other
					   cards. */
	struct sp spans[4];		/* Only four of 'em for now. */
	volatile uint8_t *map[32][4];	/* Digital cross-connect map from this card. */
};

#ifdef CONFIG_SMP
STATIC spinlock_t vp_daccs_lock = SPIN_LOCK_UNLOCKED;
#endif

#define V400P_EBUFNO	(1<<7)	/* 128k of elastic buffers per card. */

STATIC __unlikely void
mx_init_priv(struct mx *mx, queue_t *q, major_t major, minor_t minor)
{
	bzero(mx, sizeof(*mx));
	mx->mid = major;
	mx->sid = minor;
	mx->lock = 0;
	mx->flags = 0;
	mx->bid = 0;
	mx->rq = q;
	mx->wq = _WR(q);

	mx->a.next = NULL;
	mx->a.prev = &mx->a.next;
	mx->a.vp = NULL;

	mx->c.next = NULL;
	mx->c.prev = &mx->c.next;
	x->c.vp = NULL;

	mx->x.next = NULL;
	mx->x.prev = &mx->x.next;
	mx->x.vp = NULL;

	mx->info.primitive = MX_INFO_ACK;
	mx->info.mx_addr_length = 0;
	mx->info.mx_addr_offset = 0;
	mx->info.mx_parm_length = 0;
	mx->info.mx_parm_offset = 0;
	mx->info.mx_prov_flags = 0;
	mx->info.mx_prov_class = MX_CIRCUIT;
	mx->info.mx_style = MX_STYLE2;
	mx->info.mx_version = MX_VERSION;
	mx->info.mx_state = MXS_DETACHED;

	mx->addr = 0;
	mx->smask = 0;
	mx->cmask[0] = 0;
	mx->cmask[1] = 0;
	mx->cmask[2] = 0;
	mx->cmask[3] = 0;
	mx->cmask[4] = 0;
	mx->cmask[5] = 0;
	mx->cmask[6] = 0;
	mx->cmask[7] = 0;

	mx->spans = 0;
	mx->channels = 0;

	mx->parm.mp_type = MX_PARMS_CIRCUIT;
	mx->parm.mp_encoding = MX_ENCODING_NONE;
	mx->parm.mp_block_size = 8192;
	mx->parm.mp_samples = 8;
	mx->parm.mp_sample_size = 8;
	mx->parm.mp_rate = 8000;
	mx->parm.mp_tx_channels = 31;
	mx->parm.mp_rx_channels = 31;
	mx->parm.mp_opt_flags = 0;
}

STATIC __unlikely void mx_disconnect(struct mx *mx, bool force);
STATIC __unlikely void mx_disable(struct mx *mx, bool force);
STATIC __unlikely void mx_detach(struct mx *mx, bool force);

STATIC __unlikely void
mx_term_priv(struct mx *mx)
{
	switch (mx_get_state(mx)) {
	default:
		/* don't know, run them anyway */
		/* fall through */
	case MXS_CONNECTED:
		mx_disconnect(mx, true);	/* forced */
		/* fall through */
	case MXS_ENABLED:
		mx_disable(mx, true);	/* forced */
		/* fall through */
	case MXS_ATTACHED:
		mx_detach(mx, true);	/* forced */
		/* fall through */
	case MXS_DETACHED:
		break;
	}
	unbufcall(xchg(&mx->bid, 0));
}

/**
 * mx_attach: attach an MX stream
 * @mx: MX private structure
 * @ppa: PPA to which to attach
 * @style: resulting style
 *
 * When an MX stream attaches to a ppa, we need to locate the card associated
 * with the ppa (which has already been checked by calling routines) and
 * attach the MX structure.  Also, we need to map out the default channel map
 * for the MX stream.
 *
 * Another time that an MX stream is attached to a PPA is when the MX Stream
 * is opened by minor device number (equalling the card number).  In that case
 * the ppa is (uint)(minor << 16) and the minor device number is the card
 * number.  In that case mx_attach() is called from mx_qopen() with style of
 * MX_STYLE1.
 */
STATIC __unlikely void
mx_attach(struct mx *mx, mx_ulong ppa, mx_ulong style)
{
	uint32_t smask, cmask, xmask;
	struct vp *vp;
	int card = (ppa >> 16) & 0x0ff;
	int span = (ppa >> 8) & 0x0ff;
	int i, p, slot = (ppa >> 0) & 0x0ff;
	uint32_t xmask = (vp->channels == 24) ? VP_T1_CHAN_VALID_MASK : VP_E1_CHAN_VALID_MASK;

	/* locate card */
	vp = &vp_cards[card];

	/* fill out span mask and count */
	if (span == 0) {
		mx->smask = (1 << vp->spans) - 1;
		mx->spans = vp->spans;
	} else {
		mx->smask = (1 << (span - 1));
		mx->spans = 1;
	}

	/* fill out channel mask and count */
	for (mx->cmask = 0, i = 0, p = 1; i < vp->spans; i++, p <<= 1) {
		if (!(mx->smask & p))
			continue;
		if (slot = 0) {
			mx->cmask |= xmask;
			mx->cmasks[i] = xmask;
			mx->channels = vp->channels;
		} else {
			int j, s;
			uint32_t x;

			for (j = 0, s = 1, x = 1; j < 32; j++, x <<= 1) {
				if (!(xmask & x))
					continue;
				if (s == slot) {
					mx->cmask |= x;
					mx->cmasks[i] = x;
					break;
				}
			}
			mx->channels = 1;
		}
	}

	/* save ppa */
	mx->addr = ppa;

	/* link mx into card's attached MX list */
	if ((mx->a.next = vp->mx.attached))
		mx->a.next->a.prev = &mx->a.next;
	mx->a.prev = &vp->mx.attached;
	vp->mx.attached = mx;
	mx->a.vp = vp;

	/* set up parameters */
	mx->parm.mp_type = MX_PARMS_CIRCUIT;
	mx->parm.mp_encoding = MX_ENCODING_NONE;
	mx->parm.mp_block_size = 8192;
	mx->parm.mp_samples = 8;
	mx->parm.mp_sample_size = 8;
	mx->parm.mp_rate = 8000;
	mx->parm.mp_tx_channels = mx->channels;
	mx->parm.mp_rx_channels = mx->channels;
	mx->parm.mp_opt_flags = 0;

	/* set up information */
	mx->info.mx_addr_length = sizeof(mx->addr);
	mx->info.mx_addr_offset = sizeof(mx->info);
	mx->info.mx_parm_length = sizeof(mx->parm);
	mx->info.mx_parm_offset = sizeof(mx->info) + sizeof(mx->addr);
	mx->info.mx_prov_flags = 0;
	mx->info.mx_prov_class = MX_CIRCUIT;
	mx->info.mx_style = style;
	mx->info.mx_version = MX_VERSION;
	mx_set_state(mx, MXS_ATTACHED);
}

/**
 * mx_enable: enable an MX stream
 * @mx: MX private structure
 *
 * Note that while an MX Stream is attached to a VP card, the MX Stream can be used to alter
 * characteristics about the card and the span, provided that there are no other MX Streams enabled
 * for the card.  Once the first MX Stream is enabled for the card, the card is brought up with the
 * current configuration.  Distruptive configuration changes (such as switching the card from E1 to
 * T1 operation) will be refused from this point until all MX Stream disable for the card.  When the
 * last MX Stream disables for the card, the card is not brought down, but will accept distruptive
 * configuration changes once again.  Whether to lock distruptive configuration changes or not and
 * when to bring up a card, is one of the primary purposes of the enable/disable operation.
 */
STATIC __unlikely void
mx_enable(struct mx *mx)
{
	mx_set_state(mx, MXS_ENABLED);
}

STATIC noinline __unlikely int vp_cross_connect(uint32_t addr1, uint32_t addr2, uint32_t chans);

/**
 * mx_connect: connect an MX stream
 * @mx: MX private structure
 * @flags: connect flags
 * @slot: slot in multiplex
 *
 * Note that while an MX Stream is enabled for a card, none of the channels that are associated with
 * the MX Stream are necessarily trasferring data.  Also, rx/tx bit blocks are not trasferred to an
 * MX Stream that sits in the enabled state.  When the MX Stream moves to the connected stats, the
 * card interrupt service routine begins trasnferring channel samples to rx/tx bit blocks and
 * passing references to them to the connected MX Stream.  The first MX Stream to connect that has a
 * particular span and channel in its channel map will cause the transfer of samples for that span
 * and channel to begin.  The last MX Stream to disconnect that has a particular span and channel in
 * its channel map will cause the trasnfer of samples for that span to cease.
 *
 * The slot number is always zero unless a digital cross-connect is being performed.  When a zero
 * slot number connect is perfomed, every channel in the channel map for the MX Stream is connected.
 * When the slot number is non-zero, it represents a PPA just as the ppa provided to the attach;
 * however, this ppa specifies the card, span and channels to which the attached card, span and
 * channels will be digitally cross-connected.  Digital cross-connect (well pseudo-digital
 * cross-connect really) is possible between cards as well as within cards.  Although the digital
 * cross-connect is in effect, the attached channels are still transferred to the MX Stream during
 * the cross-connect and will contain both the receive data and the transmit data as completed by
 * the digital cross-connect.  This permits the MX Stream to monitor (tap) the cross-connect and is
 * useful for wire-tapping applications.
 *
 * When performing either a digital cross-connect, it is possible to specify the direction in which
 * the cross-connect is to be performed.  This allows rx and tx path splitting to be peformed as
 * well.  When cross-connecting between cards, the procedures are kept in sycnrhonization with
 * frames received for the cards by locking across frames; however, avoiding slippage between the
 * cards themselves is the responsibilty of the card syncrhonization model: particularly if any of
 * the channels are used for data.
 *
 * When a digital cross-connect is applied, the MX Stream is also linked into the target VP card's
 * cross-connect MX Stream list so that it can properly recalculate cross-connect channel maps.
 */
STATIC __unlikely void
mx_connect(struct mx *mx, mx_ulong flags, mx_ulong flags, mx_ulong slot)
{
	uint32_t slot = mx->addr[1];

	/* save and report the new address */
	mx->addr[1] = slot;
	mx->info.mx_addr_length = sizeof(mx->addr[0]) + sizeof(mx->addr[1]);
	mx->info.mx_parm_offset = sizeof(mx->info) + mx->info.mx_addr_length;

	/* slot must have been checked for correctness by the calling routine */
	if (slot == 0) {
		struct vp *vp = mx->a.vp;
		int i;

		/* no digital cross-connect */
		spin_lock_irqsave(&vp->lock, flags);
		{
			/* Link the mx into the card's connected MX list.  Data will not be
			   transferred until after we release the card spin lock. */
			if ((mx->c.next = vp->mx.connects))
				mx->c.next->c.prev = &mx->c.next;
			mx->c.prev = &vp->mx.connects;
			mx->c.vp = vp;
			vp->mx.connects = mx;

			/* Recalculate card masks.  The new masks will be immediately effective
			   once the spin lock is released. */
			for (vp->smask = 0, vp->cmask = 0, mx = vp->mx.connects; mx;
			     mx = mx->c.next) {
				vp->smask |= mx->smask;
				for (i = 0; i < 8; i++)
					if (mx->smask & (1 << i))
						vp->cmask |= mx->cmask[i];
			}
		}
		spin_unlock_irqrestore(&vp->lock, flags);
	} else {
		struct vp *vp = &vp_cards[((slot >> 16) & 0xff)];
		unsigned long flags;

		/* digital cross-connect */

		/* link mx into card's cross-connecting MX list */
		spin_lock_irqsave(&vp->lock, flags);
		{
			if ((mx->x.next = vp->mx.xconnect))
				mx->x.next->a.prev = &mx->x.next;
			mx->a.prev = &vp->mx.xconnect;
			vp->mx.xconnect = mx;
			mx->x.vp = vp;
		}
		spin_lock_irqrestore(&vp->lock, flags);

		/* Complete the digital cross-connect and connect channel maps by OR'ing the MX
		   Streams maps into the VP cards' maps.  Complete the digital cross-connect
		   tables.  */
		vp_cross_connect(mx->addr[0], mx->addr[1], mx->channels);

	}
	/* This is problematic.  There is a brief period of time between when the MX Stream moves
	   to the connected state and the MX_CONNECT_CON is delivered.  In this time, some MX data
	   may have been delivered.  The upper layer module should be prepared to process (or
	   discard) any data received in the MX_WCON_CREQ state while awating the MX_CONNECT_CON. */
	mx_set_state(mx, MXS_CONNECTED);
}

/**
 * mx_disconnect: disconnect an MX stream
 * @mx: MX private structure
 * @force:  when true, not called in response to MX_DISCONNECT_REQ.
 *
 * When called directly (force is true), the MX Stream could have already been disconnected.
 * This function must check.
 *
 * Disconecting an MX Stream consists of changing its state so that the card tasklet will no longer
 * deliver rx/tx bit blocks to the Stream.  Then the VP card's channel map can be recalculated by
 * OR'ing together all of the connected MX Stream's individual channel maps.  When the MX Stream was
 * connected with a slot (pseudo-digital cross-connect), the cross-connect maps must also be removed
 * and the card must be removed from the target card's digital cross connect Stream list.
 *
 * Although the description here is quite complicated, the code was designed to be quite simple.
 */
STATIC __unlikely void
mx_disconnect(struct mx *mx, bool force)
{
	unsigned long flags;

	if (slot == 0) {
		struct vp *vp = mx->a.vp;
		int i;

		/* no digital cross-connect */
		/* Recalculate the VP card's channel map by OR'ing the channel maps of the
		   remaining connected cards. */
		spin_lock_irqsave(&vp->lock, flags);
		{
			/* Remove from connected list.  Data will not be trasnferred from the point
			   that we took the spin lock. */
			if ((*mx->c.prev = mx->c.next))
				mx->c.next->c.prev = mx->c.prev;
			mx->c.next = NULL;
			mx->c.prev = &mx->c.next;
			mx->c.vp = NULL;

			/* Recalculate card masks.  The new masks will be immediately effective
			   once the spin lock is released. */
			for (vp->smask = 0, vp->cmask = 0, mx = vp->mx.connects; mx;
			     mx = mx->c.next) {
				vp->smask |= mx->smask;
				for (i = 0; i < 8; i++)
					if (mx->smask & (1 << i))
						vp->cmask |= mx->cmask[i];
			}
		}
		spin_unlock_irqrestore(&vp->lock, flags);
	} else {
		/* digital cross-connect */
		/* Remove cross connect from the attached and target VP cards' xconnect tables. */
		/* Recalculate both the attached and xconnected VP card's xconnect maps by OR'in
		   the xconnect maps of the remaining cross-connected cards. */
		vp_cross_disconnect(mx->addr[0], mx->addr[1], mx->channels);

		/* remove from pseudo-digital cross-connect list */
		if (mx->x.vp) {
			struct vp *vp = mx->x.vp;

			spin_lock_irqsave(&vp->lock, flags);
			{
				if ((*mx->x.prev = mx->x.next))
					mx->x.next->x.prev = mx->x.prev;
				mx->x.next = NULL;
				mx->x.prev = &mx->x.next;
				mx->x.vp = NULL;
			}
			spin_unlock_irqrestore(&vp->lock, flags);
		}
	}

	/* This is problematic.  There is a period of time between when the MX Stream moves to the
	   disconnected state and the MX_DISCONNECT_CON or MX_DISCONNECT_IND is delivered.  In this 
	   time, some a fair amount of data could be delivered.  It is wise to flush both the read
	   and the write side of the Stream before delivering eith the MX_DISCONNECT_CON or
	   MX_DISCONNECT_IND.  That is the caller's responsibility. */
	mx_set_state(mx, MXS_ENABLED);

}

/**
 * mx_disable: disable an MX stream
 * @mx: MX private structure
 * @force: when true, not called in response to MX_DISABLE_REQ
 *
 * When called directly (force is true), the MX Stream could have already been disabled.
 * This function must check.
 *
 * Disabling an MX Stream consists of clearing the enable flag for the Stream and then recalculating
 * a new VP enable flag by OR'ing together all of the attached MX Stream's individual enable flags.
 * When the VP enable flag is cleared, the card is not stopped, but will now allow disruptive
 * reconfiguraiton commands.
 */
STATIC __unlikely void
mx_disable(struct mx *mx, bool force)
{
	mx_set_state(mx, MXS_ATTACHED);
}

/**
 * mx_detach: detach an MX stream
 * @mx: MX private structure
 * @force: when true, not called in response to MX_DETACH_REQ
 *
 * When called directly (force is true), the MX Stream could have already been detached.
 * This function must check.
 *
 * Detaching an MX Stream consists simply of removing it from the attached card.
 */
STATIC __unlikely void
mx_detach(struct mx *mx, bool force)
{
	/* remove from attach list */
	if (mx->a.vp) {
		if ((*mx->a.prev = mx->a.next))
			mx->a.next->a.prev = mx->a.prev;
		mx->a.next = NULL;
		mx->a.prev = &mx->a.next;
		mp->vp = NULL;
	}
	mx_set_state(mx, MXS_DETACHED);
}

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
 *  CH Primitives sent upstream.
 *  ============================
 */
/**
 * ch_info_ack: - send an CH_INFO_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *msg)
{
	size_t alen = ch->info.ch_addr_length;
	size_t plen = ch->info.ch_parm_length;
	caddr_t aptr = (caddr_t) &ch->addr;
	caddr_t pptr = (caddr_t) &ch->parm;

	struct CH_info_ack *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p) + alen + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = ch->info;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		bcopy(pptr, mp->b_wptr, plen);
		mp->b_wptr += plen;
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_INFO_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_ack: - send an CH_OPTMGMT_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: management flags
 * @olen: options length
 * @optr: options pointer
 */
STATIC __unlikely int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, size_t olen, caddr_t optr)
{
	struct CH_optmgmt_ack *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_ACK;
		p->ch_opt_length = olen;
		p->ch_opt_offset = olen ? sizeof(*p) : 0;
		p->ch_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_OPTMGMT_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_ok_ack: - send an CH_OK_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @prim: correct primitive
 * @state: resulting state
 */
STATIC __unlikely int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim, ch_ulong state)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = prim;
		p->ch_state = state;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, state);
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_error_ack: - send an CH_ERROR_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @type: error type
 * @error: UNIX error
 */
STATIC __unlikely int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim, ch_ulong type, ch_ulong error)
{
	struct CH_error_ack *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ERROR_ACK;
		p->ch_error_primitive = prim;
		p->ch_error_type = type;
		p->ch_unix_error = error;
		p->ch_state = ch_get_state(ch);
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, state);
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_ERROR_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_attach_ack: - send an CH_OK_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @ppa: PPA to which to attach
 */
STATIC __unlikely int
ch_attach_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong ppa)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = CH_ATTACH_REQ;
		p->ch_state = CHS_ATTACHED;
		mp->b_wptr += sizeof(*p);
		ch_attach(ch, ppa, CH_STYLE2);
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_enable_ack_con: - send an CH_OK_ACK/CH_ENABLE_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_enable_ack_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_con *p;
	struct CH_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = ch_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_ENABLE_CON;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->ch_primitive = CH_OK_ACK;
			p2->ch_correct_prim = CH_ENABLE_REQ;
			p2->ch_state = ch_set_state(CHS_WCON_EREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
			putnext(RD(q), mp2);
			ch_enable(ch);
			strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_ENABLE_CON");
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * ch_connect_ack_con: - send an CH_OK_ACK/CH_CONNECT_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: connect flags
 * @slot: connect slot
 */
STATIC __unlikely int
ch_connect_ack_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_connect_con *p;
	struct CH_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = ch_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_CONNECT_CON;
			p->ch_conn_flags = flags;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->ch_primitive = CH_OK_ACK;
			p2->ch_correct_prim = CH_CONNECT_REQ;
			p2->ch_state = ch_set_state(CHS_WCON_CREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
			putnext(RD(q), mp2);
			ch_connect(ch, flags, slot);
			strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_CONNECT_CON");
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * ch_data_ind: - send an CH_DATA_IND primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @slot: slot within multiplex
 */
STATIC __unlikely int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong slot)
{
	struct CH_data_ind *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DATA_IND;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = XCHG(&msg->b_cont, NULL);
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGDA, SL_TRACE, "<- CH_DATA_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_ind: - send an CH_DISCONNECT_IND primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: connect flags
 * @slot: connect slot
 * @cause: disconnect cause
 */
STATIC __unlikely int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot,
		  ch_ulong cause)
{
	struct CH_disconnect_ind *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_IND;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		p->ch_cause = cause;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_DISCONNECT_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_ack_con: - send an CH_OK_ACK/CH_DISCONNECT_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: connect flags
 * @slot: connect slot
 */
STATIC __unlikely int
ch_disconnect_ack_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_con *p;
	struct CH_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = ch_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISCONNECT_CON;
			p->ch_conn_flags = flags;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->ch_primitive = CH_OK_ACK;
			p2->ch_correct_prim = CH_DISCONNECT_REQ;
			p2->ch_state = ch_set_state(CHS_WCON_DREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
			ch_disconnect(ch, false);
			strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_DISCONNECT_CON");
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_ind: - send an CH_DISABLE_IND primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @cause: disable cause
 */
STATIC __unlikely int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong cause)
{
	struct CH_disable_ind *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_IND;
		p->ch_cause = cause;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_ATTACHED);
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_DISABLE_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_ack_con: - send an CH_OK_ACK/CH_DISABLE_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_disable_ack_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_con *p;
	struct CH_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = ch_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISABLE_CON;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->ch_primitive = CH_OK_ACK;
			p2->ch_correct_prim = CH_DISABLE_REQ;
			p2->ch_state = ch_set_state(CHS_WCON_RREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
			putnext(RD(q), mp2);
			ch_disable(ch, false);
			strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_DISABLE_CON");
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * ch_detach_ack: - send an CH_OK_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_detach_ack(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = CH_DETACH_REQ;
		p->ch_state = CHS_DETACHED;
		mp->b_wptr += sizeof(*p);

		freemsg(msg);
		ch_detach(ch, false);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_event_ind: - send an CH_EVENT_IND primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @event: event indicated
 * @slot: slot for event
 */
STATIC __unlikely int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong event, ch_ulong slot)
{
	struct CH_event_ind *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_EVENT_IND;
		p->ch_event = event;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(ch->mid, ch->sid, CHLOGTX, SL_TRACE, "<- CH_EVENT_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  MX Primitives sent upstream.
 *  ============================
 */
/**
 * mx_info_ack: - send an MX_INFO_ACK primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
mx_info_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
	size_t alen = mx->info.mx_addr_length;
	size_t plen = mx->info.mx_parm_length;
	caddr_t aptr = (caddr_t) &mx->addr;
	caddr_t pptr = (caddr_t) &mx->parm;

	struct MX_info_ack *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p) + alen + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = mx->info;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		bcopy(pptr, mp->b_wptr, plen);
		mp->b_wptr += plen;
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_INFO_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_optmgmt_ack: - send an MX_OPTMGMT_ACK primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: management flags
 * @olen: options length
 * @optr: options pointer
 */
STATIC __unlikely int
mx_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, size_t olen, caddr_t optr)
{
	struct MX_optmgmt_ack *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OPTMGMT_ACK;
		p->mx_opt_length = olen;
		p->mx_opt_offset = olen ? sizeof(*p) : 0;
		p->mx_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_OPTMGMT_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_ok_ack: - send an MX_OK_ACK primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @prim: correct primitive
 * @state: resulting state
 */
STATIC __unlikely int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim, mx_ulong state)
{
	struct MX_ok_ack *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = prim;
		p->mx_state = state;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, state);
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_OK_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_error_ack: - send an MX_ERROR_ACK primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @type: error type
 * @error: UNIX error
 */
STATIC __unlikely int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim, mx_ulong type, mx_ulong error)
{
	struct MX_error_ack *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ERROR_ACK;
		p->mx_error_primitive = prim;
		p->mx_error_type = type;
		p->mx_unix_error = error;
		p->mx_state = mx_get_state(mx);
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, state);
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_ERROR_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_attach_ack: - send an MX_OK_ACK primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @ppa: PPA to which to attach
 */
STATIC __unlikely int
mx_attach_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong ppa)
{
	struct MX_ok_ack *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = MX_ATTACH_REQ;
		p->mx_state = MXS_ATTACHED;
		mp->b_wptr += sizeof(*p);
		mx_attach(mx, ppa, MX_STYLE2);
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_OK_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_enable_ack_con: - send an MX_OK_ACK/MX_ENABLE_CON primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
mx_enable_ack_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_con *p;
	struct MX_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mx_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_ENABLE_CON;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->mx_primitive = MX_OK_ACK;
			p2->mx_correct_prim = MX_ENABLE_REQ;
			p2->mx_state = mx_set_state(MXS_WCON_EREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_OK_ACK");
			putnext(RD(q), mp2);
			mx_enable(mx);
			strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_ENABLE_CON");
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * mx_connect_ack_con: - send an MX_OK_ACK/MX_CONNECT_CON primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: connect flags
 * @slot: connect slot
 */
STATIC __unlikely int
mx_connect_ack_con(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_connect_con *p;
	struct MX_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mx_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_CONNECT_CON;
			p->mx_conn_flags = flags;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->mx_primitive = MX_OK_ACK;
			p2->mx_correct_prim = MX_CONNECT_REQ;
			p2->mx_state = mx_set_state(MXS_WCON_CREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_OK_ACK");
			putnext(RD(q), mp2);
			mx_connect(mx, flags, slot);
			strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_CONNECT_CON");
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * mx_data_ind: - send an MX_DATA_IND primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @slot: slot within multiplex
 */
STATIC __unlikely int
mx_data_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong slot)
{
	struct MX_data_ind *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DATA_IND;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = XCHG(&msg->b_cont, NULL);
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGDA, SL_TRACE, "<- MX_DATA_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_ind: - send an MX_DISCONNECT_IND primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: connect flags
 * @slot: connect slot
 * @cause: disconnect cause
 */
STATIC __unlikely int
mx_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot,
		  mx_ulong cause)
{
	struct MX_disconnect_ind *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_IND;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		p->mx_cause = cause;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_DISCONNECT_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_ack_con: - send an MX_OK_ACK/MX_DISCONNECT_CON primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: connect flags
 * @slot: connect slot
 */
STATIC __unlikely int
mx_disconnect_ack_con(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_disconnect_con *p;
	struct MX_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mx_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISCONNECT_CON;
			p->mx_conn_flags = flags;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->mx_primitive = MX_OK_ACK;
			p2->mx_correct_prim = MX_DISCONNECT_REQ;
			p2->mx_state = mx_set_state(MXS_WCON_DREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_OK_ACK");
			mx_disconnect(mx, false);
			strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_DISCONNECT_CON");
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_ind: - send an MX_DISABLE_IND primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @cause: disable cause
 */
STATIC __unlikely int
mx_disable_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong cause)
{
	struct MX_disable_ind *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_IND;
		p->mx_cause = cause;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, MXS_ATTACHED);
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_DISABLE_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_ack_con: - send an MX_OK_ACK/MX_DISABLE_CON primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
mx_disable_ack_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_con *p;
	struct MX_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mx_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISABLE_CON;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->mx_primitive = MX_OK_ACK;
			p2->mx_correct_prim = MX_DISABLE_REQ;
			p2->mx_state = mx_set_state(MXS_WCON_RREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_OK_ACK");
			putnext(RD(q), mp2);
			mx_disable(mx, false);
			strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_DISABLE_CON");
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * mx_detach_ack: - send an MX_OK_ACK primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
mx_detach_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_ok_ack *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = MX_DETACH_REQ;
		p->mx_state = MXS_DETACHED;
		mp->b_wptr += sizeof(*p);

		freemsg(msg);
		mx_detach(mx, false);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_OK_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_event_ind: - send an MX_EVENT_IND primitive upstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @event: event indicated
 * @slot: slot for event
 */
STATIC __unlikely int
mx_event_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong event, mx_ulong slot)
{
	struct MX_event_ind *p;
	mblk_t *mp;

	if ((mp = mx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_EVENT_IND;
		p->mx_event = event;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(mx->mid, mx->sid, MXLOGTX, SL_TRACE, "<- MX_EVENT_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  State Machine and Zaptel Integration.
 *  =====================================
 */

/**
 * __vp_start_card: - start a card
 * @vp: the card to start
 *
 * This driver, unlike the tor2 and tor3 drivers always maintains the configuration for all channels
 * and all spans while the card is running.  When the first span comes up, the card will be started.
 */
static void
__vp_start_card(struct vp *vp)
{
	/* FIXME: start the card up */
}

/**
 * __vp_stop_card: - stop a card
 * @vp: the card to stop
 *
 * This driver, unlike the tor2 and tor3 drivers always maintains the configuration for all channels
 * and all spans while the card is running.  When the last span has been taken down, the card will
 * be stopped.
 */
static void
__vp_stop_card(struct vp *vp)
{
	/* FIXME: stop the card */
}

/**
 * __vp_start_span: - start a card span
 * @vp: the card
 * @span: the span number (0-4)
 *
 * If no span is running then the card is disabled.  The card is (re)started on the first span to
 * come up.
 */
static void
__vp_start_span(struct vp *vp, int span)
{
	int base = (span << 8);
	struct sp *sp = &vp->spans[span];

	if (!vp->smask)
		__vp_start_card(vp);
	switch (vp->board) {
	case E400P:
	case E400PSS7:
		vp->xlb[CTLREG] = E1DIV;
		vp->xlb[LEDREG] = 0xff;
		/* zero all span registers */
		for (offset = 0; offset < 192; offset++)
			vp->xlb[base + offset] = 0x00;
		/* set up for interleaved bus operation, byte mode */
		vp->xlb[base + 0xb5] = (span == 0) ? 0x09 : 0x08;
		vp->xlb[base + 0x1a] = 0x04;	/* CRC2: set LOTCMC */
		for (offset = 0x00; offset <= 0x08; offset++)
			vp->xlb[base + offset] = 0x00;
		for (offset = 0x10; offset <= 0x4f; offset++)
			if (offset != 0x1a)
				vp->xlb[base + offset] = 0x00;
		vp->xlb[base + 0x10] = 0x20;	/* RCR1: Rsync as input */
		vp->xlb[base + 0x11] = 0x06;	/* RCR2: Sysclk = 2.048 MHz */
		vp->xlb[base + 0x12] = (tcrc1 = 0x09);	/* TCRC1: TSiS mode */
		switch (sp->config.ifframing) {
		default:
			sp->config.ifframing = SDL_FRAMING_CCS;
		case SDL_FRAMING_CCS:
			ccr1 |= 0x08;
			break;
		case SDL_FRAMING_CAS:
			ccr1 |= 0x20;
			break;
		}
		switch (sp->config.ifcoding) {
		default:
			sp->config.ifcoding = SDL_CODING_HDB3;
		case SDL_CODING_HDB3:
			ccr1 |= 0x44;
			break;
		case SDL_CODING_AMI:
			ccr1 |= 0x00;
			break;
		}
		switch (sp->config.ifgcrc) {
		case SDL_GCRC_CRC4:
			ccr1 |= 0x11;
			break;
		default:
			ccr1 |= 0x00;
			break;
		}
		vp->xlb[base + 0x12] = tcr1;
		vp->xlb[base + 0x14] = ccr1;
		vp->xlb[base + 0x18] = 0x20;	/* 120 Ohm, Normal */
		vp->xlb[base + 0x1b] = 0x8a;	/* CRC3: LIRST & TSCLKM */
		vp->xlb[base + 0x20] = 0x1b;	/* TAFR */
		vp->xlb[base + 0x21] - 0x5f;	/* TSR1 */
		for (offset = 0x41; offset <= 0x4f; offset++)
			vp->xlb[base + offset] = 0x55;
		for (offset = 0x22; offset <= 0x25; offset++)
			vp->xlb[base + offset] = 0xff;
		timeout = jiffies + 100 * HZ / 1000;
		spin_unlock_irqrestore(&vp->lock, flags);
		/* XXX: WHAT THE H*** IS THIS? */
		while (jiffies < timeout) ;
		spin_lock_irqsave(&vp->lock, flags);
		vp->xlb[base + 0x1b] = 0x9a;	/* CRC3: set ESR as well */
		vp->xlb[base + 0x1b] = 0x82;	/* CRC3: TSCLKM only */
		vp->smask |= (1 << span);
		/* enable interrupts */
		vp->xlb[CTLREG] = (INTENA | E1DIV);
		break;
	case T400P:
	case T400PSS7:
		vp->xlb[CTLREG] = 0x00;
		vp->xlb[LEDREG] = 0xff;
		/* zero all span registers */
		for (offset = 0; offset < 192; offset++)
			vp->xlb[base + offset] = 0x00;
		/* set up for interleaved bus operation, byte mode */
		vp->xlb[base + 0xb5] = (span == 0) ? 0x09 : 0x08;
		vp->xlb[base + 0x2b] = 0x08;	/* full-on sync required (RCR1) */
		vp->xlb[base + 0x2c] = 0x08;	/* RSYNC is an input (RCR2) */
		vp->xlb[base + 0x35] = 0x10;	/* RBS enable (TCR1) */
		vp->xlb[base + 0x36] = 0x04;	/* TSYNC to be output (TCR2) */
		vp->xlb[base + 0x37] = 0x9c;	/* Tx and Rx Elastic stor, sysclk(s) = 2.048 mhz,
						   loopback controls (CCR1) */
		vp->xlb[base + 0x12] = 0x22;	/* Set up received loopup and loopdown codes */
		vp->xlb[base + 0x14] = 0x80;
		vp->xlb[base + 0x15] = 0x80;
		vp->xlb[base + 0x09] = 0x70;	/* 20db gain for monitoring */
		/* enable F bits pattern */
		switch (sp->config.ifframing) {
		default:
			sp->config.ifframing = SDL_FRAMING_SF;
		case SDL_FRAMING_SF:
			val = 0x20;
			break;
		case SDL_FRAMING_ESF:
			val = 0x88;
			break;
		}
		switch (sp->config.ifcoding) {
		default:
			sp->config.ifcoding = SDL_CODING_AMI;
		case SDL_CODING_AMI:
			break;
		case SDL_CODING_B8ZS:
			val |= 0x44;
			break;
		}
		vp->xlb[base + 0x38] = val;
		if (sp->config.ifcoding != SDL_CODING_B8ZS)
			vp->xlb[base + 0x7e] = 0x1c;	/* Set FDL register to 0x1c */
		cd->xlb[base + 0x7c] = sp->config.iftxlevel << 5;	/* LBO */
		cd->xlb[base + 0x0a] = 0x80;	/* LIRST to reset line interface */
		timeout = jiffies + 100 * HZ / 1000;
		spin_unlock_irqrestore(&vp->lock, flags);
		/* XXX: WHAT THE H*** IS THIS? */
		while (jiffies < timeout) ;
		spin_lock_irqsave(&vp->lock, flags);
		cd->xlb[base + 0x0a] = 0x30;	/* LISRT back to normal, resetting elastic buffers */
		vp->smask |= (1 << span);
		/* enable interrupts */
		vp->xlb[CTLREG] = (INTENA);
		spin_unlock_irqrestore(&vp->lock, flags);
		/* establish which channels are clear channel */
		for (c = 0; c < 24; c++) {
			int slot = xp_t1_chan_map[c];

			byte = c >> 3;
			if (!vp->spans[span]->slots[slot]
			    || vp->spans[span]->slots[slot]->sdl.config.iftype != SDL_TYPE_DS0A)
				mask |= 1 << (c % 8);
			if ((c % 8) == 7)
				vp->xlb[base + 0x39 + byte] = mask;
		}

		break;
	case V401PE:
	{
		u_char reg33, reg35, reg40;

		vp->xlb[CTLREG] = E1DIV;
		vp->xlb[LEDREG] = 0xff;
		/* zero all span registers */
		for (offset = 0; offset < 192; offset++)
			vp->xlb[base + offset] = 0x00;
		/* The DS2155 contains an on-chip power-up reset function that automatically clears
		   the writeable register space immediately after power is supplied to the DS2155.
		   The user can issue a chip reset at any time.  Issuing a reset disrupts traffic
		   flowing through the DS2155 until the device is reprogrammed.  The reset can be
		   issued through hardware using the TSTRST pin or through software using the SFTRST
		   function in the master mode register.  The LIRST (LIC2.6) should be toggled from 0 
		   to 1 to reset the line interface circuitry.  (It takes the DS2155 about 40ms to
		   recover from the LIRST bit being toggled.) Finally, after the TSYSCLK and RSYSCLK
		   inputs are stable, the receive and transmit elastic stores should be reset (this
		   step can be skipped if the elastic stores are disabled). */
		vp->xlb[base + 0x00] = 0x02;	/* E1 mode */
		/* Note that we could change the order of interleave here (3 - span) to accomodate
		   little-endian or big-endian word reads, hah! */
		vp->xlb[base + 0xc5] = 0x28 + span;	/* 4 devices channel interleaved on bus */
		/* There are three other synchronization modes: 0x00 TCLK only, 0x02 switch to RCLK 
		   if TCLK fails, 0x04 external clock, 0x06 loop.  And then 0x80 selects the
		   TSYSCLK pin instead of the MCLK pin when in external clock mode. */
		/* Hmmm. tor3 driver has TCLK only for T1, but RCLK if TCLK fails for E1! */
		vp->xlb[base + 0x70] = 0x02;	/* LOTCMC into TCSS0 */
		/* IOCR1.0=0 Output data format is bipolar, IOCR1.1=1 TSYNC is an output, IOCR1.4=1
		   RSYNC is an input (elastic store). */
		vp->xlb[base + 0x01] = 0x12;	/* RSIO + 1 is from O12.0 */
		vp->xlb[base + 0x02] = 0x03;	/* RSYSCLK/TSYSCLK 8.192MHz IBO */
		vp->xlb[base + 0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
#if 0
		/* We should really reset the elastic store after reset like so: */
		vp->xlb[base + 0x4f] = 0x55;	/* RES/TES (elastic store) reset */
		vp->xlb[base + 0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
		/* And even align it like so: */
		vp->xlb[base + 0x4f] = 0x99;	/* RES/TES (elastic store) reset */
		vp->xlb[base + 0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
#endif

		reg33 = 0x00;
		reg35 = 0x10;	/* TSiS */
		reg40 = 0x00;

		switch (sp->config.ifframing) {
		default:
			sp->config.ifframing = SDL_FRAMING_CCS;
		case SDL_FRAMING_CCS:
			reg40 |= 0x06;
			reg33 |= 0x40;
			break;
		case SDL_FRAMING_CAS:
			break;
		}
		switch (sp->config.ifcoding) {
		default:
			sp->config.ifcoding = SDL_CODING_HDB3;
		case SDL_CODING_HDB3:
			reg33 |= 0x20;
			reg35 |= 0x04;
			break;
		case SDL_CODING_AMI:
			break;
		}
		switch (sp->config.ifgcrc) {
		case SDL_GCRC_CRC4:
			reg33 |= 0x08;
			reg35 |= 0x01;
			break;
		default:
			break;
		}
		/* We could be setting automatic report alarm generation (0x01) (T1) or automatic
		   AIS generation (0x02) (E1). */
		vp->xlb[base + 0x35] = reg35;	/* TSiS, TCRC4 (from 014.4), THDB3 (from O14.6) */
		vp->xlb[base + 0x36] = 0x04;	/* AEBE 36.2 */
		vp->xlb[base + 0x33] = reg33;	/* RCR4, RHDB3, RSM */
		vp->xlb[base + 0x34] = 0x01;	/* RCL (1ms) */
		vp->xlb[base + 0x40] = reg40;	/* RCCS, TCCS */
		/* This is a little peculiar: the host should be using Method 3 in section 22.3 of
		   the DS2155 manual instead of this method that only permits 250us to read or
		   write the bits. */
		vp->xlb[base + 0xd0] = 0x1b;	/* TAFR */
		vp->xlb[base + 0xd1] = 0x5f;	/* TNAFR */

		/* power the transmit */
		vp->xlb[base + 0x78] = 0x21;	/* 120 Ohm normal, power transmitter */
		vp->xlb[base + 0x79] = 0x98;	/* JACLK on for E1 */
		vp->xlb[base + 0x7b] = 0x0f;	/* 120 Ohm term, MCLK prescaled for 2.048 MHz */
		/* This register is where we set internal linear gain boost for monitoring
		   applications: 0x08 is 20db, 0x10 is 26db, 0x18 is 32 db. */
		vp->xlb[base + 0x7a] = 0x00;	/* no boost */

		vp->smask |= (1 << span);
		/* enable interrupts */
		vp->xlb[CTLREG] = (INTENA | E1DIV);
		spin_unlock_irqrestore(&vp->lock, flags);
		break;
	}
	case V401PT:
	{
		vp->xlb[CTLREG] = 0x00;
		vp->xlb[LEDREG] = 0xff;
		/* zero all span registers */
		for (offset = 0; offset < 192; offset++)
			vp->xlb[base + offset] = 0x00;
		/* The DS2155 contains an on-chip power-up reset function that automatically clears
		   the writeable register space immediately after power is supplied to the DS2155.
		   The user can issue a chip reset at any time.  Issuing a reset disrupts traffic
		   flowing through the DS2155 until the device is reprogrammed.  The reset can be
		   issued through hardware using the TSTRST pin or through software using the SFTRST
		   function in the master mode register.  The LIRST (LIC2.6) should be toggled from 0 
		   to 1 to reset the line interface circuitry.  (It takes the DS2155 about 40ms to
		   recover from the LIRST bit being toggled.) Finally, after the TSYSCLK and RSYSCLK
		   inputs are stable, the receive and transmit elastic stores should be reset (this
		   step can be skipped if the elastic stores are disabled). */
		vp->xlb[base + 0x00] = 0x00;	/* T1 mode */
		/* Note that we could change the order of interleave here (3 - span) to accomodate
		   little-endian or big-endian word reads, hah! */
		vp->xlb[base + 0xc5] = 0x28 + span;	/* 4 devices channel interleaved on bus */
		/* There are three other synchronization modes: 0x00 TCLK only, 0x02 switch to RCLK 
		   if TCLK fails, 0x04 external clock, 0x06 loop.  And then 0x80 selects the
		   TSYSCLK pin instead of the MCLK pin when in external clock mode. */
		/* Hmmm. tor3 driver has TCLK only for T1, but RCLK if TCLK fails for E1! */
		vp->xlb[base + 0x70] = 0x00;	/* LOTCMC into TCSS0 */
		/* IOCR1.0=0 Output data format is bipolar, IOCR1.1=1 TSYNC is an output, IOCR1.4=1
		   RSYNC is an input (elastic store). */
		vp->xlb[base + 0x01] = 0x12;	/* RSIO + 1 is from O12.0 */
		vp->xlb[base + 0x02] = 0x03;	/* RSYSCLK/TSYSCLK 8.192MHz IBO */
		vp->xlb[base + 0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
#if 0
		/* We should really reset the elastic store after reset like so: */
		vp->xlb[base + 0x4f] = 0x55;	/* RES/TES (elastic store) reset */
		vp->xlb[base + 0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
		/* And even align it like so: */
		vp->xlb[base + 0x4f] = 0x99;	/* RES/TES (elastic store) reset */
		vp->xlb[base + 0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
#endif
		reg04 = japan ? 0x02 : 0x00;
		reg05 = japan ? 0x90 : 0x00; /* japan always as TSSE? */
		reg06 = 0x00;

		switch (sp->config.ifframing) {
		default:
			sp->config.ifframing = SDL_FRAMING_ESF;
		case SDL_FRAMING_ESF:
			reg04 |= 0x40;
			break;
		case SDL_FRAMING_SF:
			break;
		}
		switch (sp->config.ifcoding) {
		default:
			sp->config.ifcoding = SDL_CODING_B8ZS;
		case SDL_CODING_B8ZS:
			reg04 |= 0x20;
			break;
		case SDL_CODING_AMI:
			break;
		}
		vp->xlb[base + 0x03] = 0x08;	/* SYNCC */
		vp->xlb[base + 0x04] = 0x60;	/* ESF B8ZS */
		vp->xlb[base + 0x05] = 0x10;	/* TSSE */
		vp->xlb[base + 0x06] = 0x80;	/* TB8ZS */
		vp->xlb[base + 0x07] = 0x04;	/* ESF */

		vp->xlb[base + 0x40] = reg40;	/* RCCS, TCCS */

		vp->xlb[base + 0x79] = 0x58;	/* JACLK on for T1 (and reset) */
		while (jiffies < timeout) ;	/* 100ms wait */
		vp->xlb[base + 0x79] = 0x18;	/* JACLK on for T1 */
		/* This is the register where we do line build out. */
		vp->xlb[base + 0x78] = 0x01;	/* Enable transmitter, DSX-1 OdB */
		vp->xlb[base + 0x7b] = 0x0a;	/* 100 ohm, MCLK prescaled for 2.048 MHz */
		/* This register is where we set internal linear gain boost for monitoring
		   applications: 0x08 is 20db, 0x10 is 26db, 0x18 is 32 db. */
		vp->xlb[base + 0x7a] = 0x00;	/* no boost */

		vp->smask |= (1 << span);
		/* enable interrupts */
		vp->xlb[CTLREG] = (INTENA);
		spin_unlock_irqrestore(&vp->lock, flags);
		break;
	}
	}
	/* FIXME: start the span */
	vp->smask |= (1 << span);
}

/**
 * __vp_stop_span: - stop a card span
 * @vp: the card
 * @span: the span number (0-4)
 *
 * If no span is running the card is disabled.  The card is (re)started on the first span to come
 * up.
 */
static void
__vp_stop_span(struct vp *vp, int span)
{
	/* FIXME: stop the span */
	vp->smask &= ~(1<<span);
	if (!vp->span)
		__vp_stop_card(vp);
}

/**
 * vp_setchunksize: - zaptel callback, set chunk size
 * @span: the span for which to set chunk size
 * @chunksize: the chunk size to set
 *
 * Required: Set the requested chunk size.  This is the unit in which you must report results for
 * conferencing, etc.  Really, this is never called by current (1.2.9.1) zaptel and is therefore
 * certainly NOT REQUIRED.
 */
static int
vp_setchunksize(struct zt_span *span, int chunksize)
{
	struct vp *vp = (struct vp *)span->pvt;

	if (chunksize != ZT_CHUNKSIZE)
		return (-1);
	return (0);
}

/**
 * vp_spanconfig: - zaptel callback, configure span
 * @span: the span to configure
 * @lc: the configuration information
 *
 * Configure the span (if appropriate).  lc has three significant fields: lbo (line build-out),
 * lineconfig (line configuration parameters), sync (level of synchronization source).  Zaptel
 * cannot be allowed to perform disruptive reconfigurations while CH or MX streams are connected.
 */
static int
vp_spanconfig(struct zt_span *span, struct zt_lineconfig *lc)
{
	struct vp *vp = (struct vp *) span->pvt;
	int spanno = span->offset;
	struct ch *ch;
	struct mx *mx;

	for (ch = vp->ch.connects; ch; ch = ch->c.next)
		smask |= ch->smask;
	for (mx = vp->mx.connects; mx; mx = mx->c.next)
		smask |= mx->smask;
	if (!(smask & (1 << spanno))) {
	} else
		err = -EBUSY;
	if (err)
		strlog(modid, 0, 0, SL_ERROR, "vp_spanconfig() span conflict");
	return (err);
}

/**
 * vp_startup: - start the span
 * @span: the span to start
 *
 * Start the span.  Zaptel can start any span that has not started.  When the first span is brought
 * up, start the card.
 */
static int
vp_startup(struct zt_span *span)
{
	if (!(span->flags & ZT_FLAG_RUNNING)) {
		struct vp *vp = (struct vp *) span->pvt;
		unsigned long flags;

		spin_lock_irqsave(&vp->lock, flags);
		{
			if (!(vp->smask & (1<<span)))
				__vp_start_span(vp, span);
		}
		spin_unlock_irqrestore(&vp->lock, flags);
	}
	return (0);
}

/**
 * vp_shutdown: - zaptel callback, shutdown span
 * @span: span to shut down
 *
 * Shut down the span.  Zaptel cannot be allowed to stop spans for which CH or MX devices are
 * enabled, but the span can be marked down to zaptel.  When the last span is taken down, stop the
 * card.
 */
static int
vp_shutdown(struct zt_span *span)
{
	if (span->flags & ZT_FLAG_RUNNING) {
		struct vp *vp = (struct vp *) span->pvt;
		unsigned long flags;
		uint32_t smask = 0;
		struct ch *ch;
		struct mx *mx;
		int spanno = span->offset;

		spin_lock_irqsave(&vp->lock, flags);
		{
			if ((vp->smask & (1<<spanno))) {
				for (ch = vp->ch.enabled; ch; ch = ch->e.next)
					smask |= ch->smask;
				for (mx = vp->mx.enabled; mx; mx = mx->e.next)
					smask |= mx->smask;
				if (!(smask & (1<<spanno)) && (vp->smask & (1<<spanno)))
					__vp_stop_span(vp, spanno);
			}
		}
		spin_unlock_irqrestore(&vp->lock, flags);
	}
	return (0);
}

/**
 * vp_maint: - zaptel callback, perform maintenance on span
 * @span: span to maintain
 * @mode: maintainenacne mode
 *
 * Zaptel cannot be allowed to perform maintenance on spans to which there is a CH device attached
 * or to which there is an MX device connected.  Otherwise, span maintenance is permitted.  Note
 * that zaptel calls this callback with a lock on the span and locak interrupts suppressed,
 * therefore, we only need to take a lock on the card.
 */
static int
vp_maint(struct zt_span *span, int mode)
{
	struct vp *vp = (struct vp *) span->pvt;
	struct ch *ch;
	struct mx *mx;
	uint32_t smask;
	int base = span << 8;

	spin_lock(&vp->lock);
	for (ch = vp->ch.attached; ch; ch = ch->a.next)
		smask |= ch->smask;
	for (mx = vp->mx.connects; mx; mx = mx->c.next)
		smask |= mx->smask;
	if (!(smask & (1 << span->offset))) {
		switch (vp->device) {
		case VP_DEV_DS2154:
		case VP_DEV_DS21354:
		case VP_DEV_DS21554:
			/* tor2 e1 card */
			switch (mode) {
			case ZT_MAINT_NONE:	/* normal mode */
				vp->xlb[base + 0xa8] = 0x00;
				break;
			case ZT_MAINT_LOCALLOOP:	/* local loopback */
				vp->xlb[base + 0xa8] = 0x40;
				break;
			case ZT_MAINT_REMOTELOOP:	/* remote loopback */
				vp->xlb[base + 0xa8] = 0x80;
				break;
			default:	/* T1 only */
			case ZT_MAINT_LOOPUP:	/* send loopup code */
			case ZT_MAINT_LOOPDOWN:	/* send loopdown code */
			case ZT_MAINT_LOOPSTOP:	/* stop sending loop codes */
				err = -ENOSYS;
				break;
			}
			break;
		case VP_DEV_DS2152:
		case VP_DEV_DS21352:
		case VP_DEV_DS21552:
			/* tor2 t1 card */
			switch (mode) {
			case ZT_MAINT_NONE:	/* normal mode */
				/* bit 0x80 is the transmit japanese CRC6 enable */
				vp->xlb[base + 0x19] = (japan ? 0x80 : 0x00) | 0x00;
				vp->xlb[base + 0x0a] = 0x00;
				break;
			case ZT_MAINT_LOCALLOOP:	/* local loopback */
				/* bit 0x80 is the transmit japanese CRC6 enable */
				vp->xlb[base + 0x19] = (japan ? 0x80 : 0x00) | 0x40;
				vp->xlb[base + 0x0a] = 0x00;
				break;
			case ZT_MAINT_REMOTELOOP:	/* remote loopback */
				/* bit 0x80 is the transmit japanese CRC6 enable */
				vp->xlb[base + 0x19] = (japan ? 0x80 : 0x00) | 0x00;
				vp->xlb[base + 0x0a] = 0x40;
				break;
			case ZT_MAINT_LOOPUP:	/* send loopup code */
				vp->xlb[base + 0x30] = 0x02;	/* transmit loop code */
				vp->xlb[base + 0x12] = 0x22;
				vp->xlb[base + 0x13] = 0x80;	/* code to transmit */
				break;
			case ZT_MAINT_LOOPDOWN:	/* send loopdown code */
				vp->xlb[base + 0x30] = 0x02;	/* transmit loop code */
				vp->xlb[base + 0x12] = 0x62;
				vp->xlb[base + 0x13] = 0x90;	/* code to transmit */
				break;
			case ZT_MAINT_LOOPSTOP:	/* stop sending loop codes */
				vp->xlb[base + 0x30] = 0x00;
				break;
			default:
				err = -ENOSYS;
				break;
			}
			break;
		case VP_DEV_DS2155:
			switch (vp->board) {
			case V401PE:
				/* tor3 e1 card */
				switch (mode) {
				case ZT_MAINT_NONE:	/* normal mode */
					vp->xlb[base + 0x4a] = 0x00;
					break;
				case ZT_MAINT_LOCALLOOP:	/* local loopback */
					vp->xlb[base + 0x4a] = 0x08;
					break;
				case ZT_MAINT_REMOTELOOP:	/* remote loopback */
					vp->xlb[base + 0x4a] = 0x04;
					break;
				default:	/* T1 only */
				case ZT_MAINT_LOOPUP:	/* send loopup code */
				case ZT_MAINT_LOOPDOWN:	/* send loopdown code */
				case ZT_MAINT_LOOPSTOP:	/* stop sending loop codes */
					err = -ENOSYS;
					break;
				}
				break;
			case V401PT:
				/* tor3 t1 card */
				switch (mode) {
				case ZT_MAINT_NONE:	/* normal mode */
					vp->xlb[base + 0x4a] = 0x00;
					break;
				case ZT_MAINT_LOCALLOOP:	/* local loopback */
					vp->xlb[base + 0x4a] = 0x08;
					break;
				case ZT_MAINT_REMOTELOOP:	/* remote loopback */
					vp->xlb[base + 0x4a] = 0x04;
					break;
				case ZT_MAINT_LOOPUP:	/* send loopup code */
					vp->xlb[base + 0xb6] &= ~0xc0;	/* 5 bit code */
					vp->xlb[base + 0xb7] = 0x80;	/* loop up code */
					vp->xlb[base + 0x07] |= 0x01;	/* send loop code */
					break;
				case ZT_MAINT_LOOPDOWN:	/* send loopdown code */
					vp->xlb[base + 0xb6] &= ~0xc0;	/* 5 bit code */
					vp->xlb[base + 0xb7] = 0x90;	/* loop down code */
					vp->xlb[base + 0x07] |= 0x01;	/* send loop code */
					break;
				case ZT_MAINT_LOOPSTOP:	/* stop sending loop codes */
					vp->xlb[base + 0x07] &= ~0x01;	/* stop sending loop code */
					break;
				default:
					err = -ENOSYS;
					break;
				}
				break;
			default:
				err = -ENOSYS;
				break;
			}
			break;
		default:
			err = -ENOSYS;
			break;
		}
	} else
		err = -EBUSY;
	spin_unlock(&vp->lock);
	if (err)
		strlog(modid, 0, 0, SL_ERROR, "vp_maint() error %d", -err);
	return (err);
}

/**
 * vp_chanconfig: zaptell callback, channel configuration
 * @chan: channel to configure
 * @sigtype: signalling type
 *
 * Zaptel cannot be allowed to reconfigure channels to which there is a CH device attached or to
 * which there is an MX device connected.  Otherwise, changes to clear channels on running T1 spans
 * cause a reconfiguration.
 */
static int
vp_chanconfig(struct zt_chan *chan, int sigtype)
{
	struct vp *vp = (struct vp *) chan->span->pvt;
	int err = 0, span = chan->span->offset;
	unsigned long flags;
	struct ch *ch;
	struct mx *mx;
	uint32_t cmask = 0;

	spin_lock_irqsave(&vp->lock, flags);
	{
		for (ch = vp->ch.attached; ch; ch = ch->a.next)
			cmask |= ch->cmasks[span];
		for (mx = vp->mx.connects; mx; mx = mx->c.next)
			cmask |= mx->cmasks[span];
		if (!(cmask & (1 << chan->chanpos))) {
			if ((chan->span->channels == 24)
			    && (chan->span->flags & ZT_FLAG_RUNNING)
			    && ((chan->sig & ZT_SIG_CLEAR) ^ (sigtype & ZT_SIG_CLEAR)))
				__vp_set_clear(vp, span);
		} else
			/* there is a channel conflict */
			err = -EBUSY;
	}
	spin_unlock_irqrestore(&vp->lock, flags);
	if (err)
		strlog(modid, 0, 0, SL_ERROR, "invalid vp_chanconfig()");
	return (err);
}

/**
 * vp_open: zaptell callback, open a channel
 * @chan: channel to open
 *
 * Prepare a channel for I/O.  The original Tor 2 and Tor 3 zaptel drivers do nothing here.  The
 * OpenSS7 driver is a little more efficient in that it only transfers data accross the PCI bus when
 * necessary and, therefore, it keeps track of spans and channels that are activated using span and
 * channel bitmasks.  Also, channels that are being used by the MX or CH drivers are not available
 * for use by zaptel.
 */
static int
vp_open(struct zt_chan *chan)
{
	struct vp *vp = (struct vp *) chan->span->pvt;
	int err = 0, span = chan->span->offset;
	unsigned long flags;
	struct zt_chan *c;
	uint32_t cmask = 0;

	spin_lock_irqsave(&vp->lock, flags);
	{
		for (c = chan; c->chanpos != chan->span->channels - 1 && c->master == chan->master; c++)
			cmask |= (1 << c->chanpos);
		if (!(cmask & vp->cmasks[span])) {
			vp->cmasks[span] |= cmask;
			vp->cmask |= cmask;
			vp->smask |= (1 << span);
		} else {
			/* there was a channel conflict */
			err = -EBUSY;
		}
	}
	spin_unlock_irqrestore(&vp->lock, flags);
	if (err)
		strlog(modid, 0, 0, SL_ERROR, "invalid vp_open()");
	return (err);
}

/**
 * vp_close: zaptell callback, close a channel
 * @chan: channel to close
 *
 * Close channel for I/O.  The original Tor 2 and Tor 3 drivers do nothing here.  The OpenSS7 driver
 * is a little more efficient in that it only trasnfers data across the PCI bus when necessary and,
 * therefore, it keeps track of spans and channels that are activated using span and channel
 * bitmasks.  Also, channels that are not being used by zaptel are available for use by the MX or CH
 * drivers.
 */
static int
vp_close(struct zt_chan *chan)
{
	struct vp *vp = (struct vp *) chan->span->pvt;
	int err = 0, span = chan->span->offset;
	unsigned long flags;
	struct zt_chan *c;
	uint32_t cmask = 0;

	spin_lock_irqsave(&vp->lock, flags);
	{
		for (c = chan; c->chanpos != chan->span->channels - 1 && c->master == chan->master; c++)
			cmask |= (1 << c->chanpos);
		if (!((cmask ^ vp->cmasks[span]) & cmask)) {
			vp->cmasks[span] &= ~cmask;
			vp->cmask = 0;
			vp->smask = 0;
			/* recalculate overall masks */
			for (span = 0; span < vp->spans; span++) {
				if (vp->cmasks[span]) {
					vp->cmask |= vp->cmasks[span];
					vp->smask |= (1 << span);
				}
			}
		} else
			err = -EINVAL;
	}
	spin_unlock_irqrestore(&vp->lock, flags);
	if (err)
		strlog(modid, 0, 0, SL_CONSOLE, "invalid vp_close()");
	return (err);
}

/**
 * vp_ioctl: zaptel callback, perform input-output control
 * @chan: channel to control
 * @cmd: I/O control command
 * @data: I/O control argument
 *
 * This is a placeholder.  When zaptel has no defined input-output control if this callback is not
 * defined, zaptel will return -ENOTTY.  Here it is defined by we take the default behavior of
 * returning -ENOTTY anyway for now.
 */
static int
vp_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data)
{
	return (-ENOTTY);
}

/**
 * vp_echocan: - native echo cancellation
 * @chan: channel to echo cancel
 * @ecval: 0 off, non-zero on, 32-256, number of filter taps.
 * 
 * Native echo cancellation
 */
static int
vp_echocan(struct zt_chan *chan, int ecval)
{
	struct vp *vp = (struct vp *)chan->span->pvt;
}

/**
 * vp_rbsbits: - zaptel callback, assert RBS bits
 * @chan: channel to signal on
 * @bits: RBS bits set
 *
 * If you are a T1 like interface, you can just provide an rbsbits function and zaptel will assert
 * robbed bits.  Be sure to set the ZT_FLAG_RBS in this case.
 */
static int
vp_rbsbits(struct zt_chan *chan, int bit)
{
	struct vp *vp = (struct vp *)chan->span->pvt;
	/* Option 1: If you're a T1 like interface, you can just provide a rbsbits function and
	   we'll assert robbed bits for you.  Be sure to set the ZT_FLAG_RBS in this case.  */

	/* Opt: If the span uses A/B bits, set them here */
}

#if 0
static int
vp_hooksig(struct zt_chan *chan, zt_txsig_t hookstate)
{
	struct vp *vp = (struct vp *)chan->span->pvt;
	/* Option 2: If you don't know about sig bits, but do have their equivalents (i.e. you can
	   disconnect battery, detect off hook, generate ring, etc directly) then you can just
	   specify a sethook function, and we'll call you with appropriate hook states to set.
	   Still set the ZT_FLAG_RBS in this case as well */
}

static int
vp_sethook(struct zt_chan *chan, int hookstate)
{
	struct vp *vp = (struct vp *)chan->span->pvt;
	/* Option 3: If you can't use sig bits, you can write a function which handles the
	   individual hook states */
}
#endif

/**
 * vp_dacs: - zaptel callback, set up pseudo-digital cross-connect
 * @chan1: from channel
 * @chan2: to channel
 *
 * Dacs the contents of chan2 into chan1 if possible.  The second argument (chan2) is NULL when
 * chan1 is to be disconnected.
 */
static int
vp_dacs(struct zt_chan *chan1, struct zt_chan *chan2)
{
	struct vp *vp = (struct vp *)chan->span->pvt;
}


/*
 *  CH Primitives received from upstream.
 *  =====================================
 */
/**
 * ch_info_req: - process CH_INFO_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_info_ack(ch, q, mp);
}

/**
 * ch_optmgmt_req: - process CH_OPTMGMT_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_error_ack(ch, q, mp, CH_OPTMGMT_REQ, CHNOTSUPP, EOPNOTSUPP);
}

/**
 * ch_attach_req: - process CH_ATTACH_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Requests that the stream be attached to the specified card and span.  The specified card and span
 * must exist and be available.
 */
STATIC noinline __unlikely int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_attach_req *p = (typeof(p)) mp->b_rptr;
	uint32_t addr;
	int card, span, smask, cmask;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch->info.ch_style != CH_STYLE2)
		goto notsupport;
	if (ch_get_state(ch) != CHS_DETACHED)
		goto outstate;
	if (p->ch_addr_length != sizeof(addr))
		goto badaddr;
	if (mp->b_wptr < mp->b_rptr + p->ch_addr_length + p->ch_addr_offset)
		goto protoshort;
	/* avoid alignment problems */
	bcopy(mp->b_rptr, &addr, sizeof(addr));
	{
		int card = (addr >> 16) & 0x0ff;
		int span = (addr >> 8) & 0x0ff;
		int slot = (addr >> 0) & 0x0ff;

		if (vp_card + 1 < card || card < 1)
			goto badaddr;
		if (span != 0 && (vp_cards[card].spans + 1 < span || span < 1))
			goto badaddr;
		if (slot != 0 && (vp_cards[card].channels + 1 < slot || slot < 1))
			goto badaddr;
	}
	return ch_attach_ack(ch, q, mp, addr);
      badaddr:
	return ch_error_ack(ch, q, mp, CH_ATTACH_REQ, CHBADADDR, EINVAL);
      notsupport:
	return ch_error_ack(ch, q, mp, CH_ATTACH_REQ, CHNOTSUPP, EOPNOTSUPP);
      outstate:
	return ch_error_ack(ch, q, mp, CH_ATTACH_REQ, CHOUTSTATE, EPROTO);
      protoshort:
	return ch_error_ack(ch, q, mp, CH_ATTACH_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_enable_req: - process CH_ENABLE_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
}

/**
 * ch_connect_req: - process CH_CONNECT_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
}

/**
 * ch_data_req: - process CH_DATA_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
}

/**
 * ch_disconnect_req: - process CH_DISCONNECT_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
}

/**
 * ch_disable_req: - process CH_DISABLE_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
}

/**
 * ch_detach_req: - process CH_DETACH_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
}

/**
 * ch_w_proto: M_PROTO/M_PCPROTO message processing
 * @q: write queue
 * @mp: the message to process
 */
STATIC noinline fastcall int
ch_w_proto(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	ch_ulong oldstate = ch_get_state(ch);
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(ch_ulong)) {
		swerr();
		freemsg(mp);
		return (0);
	}
	if (!test_and_set_bit(CHB_LOCK, &ch->lock)) {
		switch (*(ch_ulong *) mp->b_rptr) {
		case CH_INFO_REQ:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_INFO_REQ");
			err = ch_info_req(ch, q, mp);
			break;
		case CH_OPTMGMT_REQ:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_OPTMGMT_REQ");
			err = ch_optmgmt_req(ch, q, mp);
			break;
		case CH_ATTACH_REQ:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_ATTACH_REQ");
			err = ch_attach_req(ch, q, mp);
			break;
		case CH_ENABLE_REQ:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_ENABLE_REQ");
			err = ch_enable_req(ch, q, mp);
			break;
		case CH_CONNECT_REQ:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_CONNECT_REQ");
			err = ch_connect_req(ch, q, mp);
			break;
		case CH_DATA_REQ:
			strlog(ch->mid, ch->sid, CHLOGDA, SL_TRACE, "-> CH_DATA_REQ");
			err = ch_data_req(ch, q, mp);
			break;
		case CH_DISCONNECT_REQ:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_DISCONNECT_REQ");
			err = ch_disconnect_req(ch, q, mp);
			break;
		case CH_DISABLE_REQ:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_DISABLE_REQ");
			err = ch_disable_req(ch, q, mp);
			break;
		case CH_DETACH_REQ:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_DETACH_REQ");
			err = ch_detach_req(ch, q, mp);
			break;
		default:
			strlog(ch->mid, ch->sid, CHLOGRX, SL_TRACE, "-> CH_????_???");
			freemsg(mp);
			err = 0;
			break;
		}
		/* revert to previous state on error */
		if (err)
			ch_set_state(ch, oldstate);
		clear_bit(CHB_LOCK, &ch->lock);
	} else {
		err = -EDEADLOCK;
		qenable(q);
	}
	return (err);
}

STATIC noinline fastcall int
ch_w_flush(queue_t *q, mblk_t *mp)
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
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

STATIC noinline fastcall __unlikely int
ch_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return ch_w_proto(q, mp);
	case M_FLUSH:
		return ch_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		/* IO controls */
		return ch_w_ioctl(q, mp);
	case M_PCCTL:
	case M_CTL:
		/* inter module IO controls */
		return ch_w_ctl(q, mp);
	default:
		swerr();
	case M_READ:
		freemsg(mp);
		return (0);
	}
}

/**
 * ch_w_data: write data for CH stream
 * @q: write queue
 * @mp: M_DATA message
 */
STATIC inline fastcall __hot_write int
ch_w_data(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct vp *vp;

	if (likely((vp = ch->c.vp) != NULL)) {
		if (ch->tx.nxt)
			return (-EBUSY);
		ch->tx.nxt = mp;
		return (0);
	}
	freemsg(mp);
	return (0);
}

STATIC inline fastcall __hot_write int
ch_w_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return ch_w_data(q, mp);
	return ch_w_msg_slow(q, mp);
}

/**
 * ch_wput: CH write put procedure
 * @q: write queue
 * @mp: message to put
 */
STATIC streamscall __hot_write int
ch_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || ch_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * ch_wsrv: CH write service procedure
 * @q: write queue to service
 */
STATIC streamscall __hot_out int
ch_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (ch_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/*
 *  MX Primitives received from upstream.
 *  =====================================
 */
/**
 * mx_info_req: - process MX_INFO_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
mx_info_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	return mx_info_ack(mx, q, mp);
}

/**
 * mx_optmgmt_req: - process MX_OPTMGMT_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC fastcall int
mx_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	/* There really are no options to manage at the moment. */
	return mx_error_ack(mx, q, mp, MX_OPTMGMT_REQ, MXNOTSUPP, EOPNOTSUPP);
}

/**
 * mx_attach_req: - process MX_ATTACH_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Requests that the stream associated with @q be attached to the specified card and span.  The
 * specified card an span must exist, and be available.
 *
 */
STATIC fastcall int
mx_attach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_attach_req *p = (typeof(p)) mp->b_rptr;
	uint32_t addr;
	int card, span, smask, cmask;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx->info.mx_style != MX_STYLE2)
		goto notsupport;
	if (mx_get_state(mx) != MXS_DETACHED)
		goto outstate;
	if (p->mx_addr_length != sizeof(addr))
		goto badaddr;
	if (mp->b_wptr < mp->b_rptr + p->mx_addr_length + p->mx_addr_offset)
		goto protoshort;
	/* avoid alignment problems */
	bcopy(mp->b_rptr, &addr, sizeof(addr));
	{
		int card = (addr >> 16) & 0x0ff;
		int span = (addr >> 8) & 0x0ff;
		int slot = (addr >> 0) & 0x0ff;

		if (vp_card + 1 < card || card < 1)
			goto badaddr;
		if (span != 0 && (vp_cards[card].spans + 1 < span || span < 1))
			goto badaddr;
		if (slot != 0 && (vp_cards[card].channels + 1 < slot || slot < 1))
			goto badaddr;
	}
	return mx_attach_ack(mx, q, mp, addr);
      badaddr:
	return mx_error_ack(mx, q, mp, MX_ATTACH_REQ, MXBADADDR, EINVAL);
      notsupport:
	return mx_error_ack(mx, q, mp, MX_ATTACH_REQ, MXNOTSUPP, EOPNOTSUPP);
      outstate:
	return mx_error_ack(mx, q, mp, MX_ATTACH_REQ, MXOUTSTATE, EPROTO);
      protoshort:
	return mx_error_ack(mx, q, mp, MX_ATTACH_REQ, MXBADPRIM, EMSGSIZE);
}

/**
 * mx_enable_req: - process MX_ENABLE_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC fastcall int
mx_enable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_ATTACHED)
		goto outstate;
	mx_set_state(mx, MXS_WACK_EREQ);
	/* Enables are always valid when called from the correct state. */
	return mx_enable_ack_con(mx, q, mp);
	return (0);
      outstate:
	return mx_error_ack(mx, q, mp, MX_ENABLE_REQ, MXOUTSTATE, EPROTO);
      badprim:
	return mx_error_ack(mx, q, mp, MX_ENABLE_REQ, MXBADPRIM, EMSGSIZE);
}

/**
 * mx_connect_req: - process MX_CONNECT_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC fastcall int
mx_connect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_ENABLED)
		goto outstate;
	if (p->mx_conn_flags != 0)
		goto badflag;
	if (p->mx_slot != 0)
		goto badslot;
	/* Check the validity of the connect first because the connect procedure cannot return
	   errors. */
	mx_set_state(mx, MXS_WACK_CREQ);
	return mx_connect_ack_con(mx, q, mp, p->mx_conn_flags, p->mx_slot);
      badslot:
	return mx_error_ack(mx, q, mp, MX_CONNECT_REQ, MXBADADDR, EINVAL);
      badflag:
	return mx_error_ack(mx, q, mp, MX_CONNECT_REQ, MXBADFLAG, EINVAL);
      outstate:
	return mx_error_ack(mx, q, mp, MX_CONNECT_REQ, MXOUTSTATE, EPROTO);
      badprim:
	return mx_error_ack(mx, q, mp, MX_CONNECT_REQ, MXBADPRIM, EMSGSIZE);
}

/**
 * mx_data_req: - process MX_DATA_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC fastcall int
mx_data_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_data_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_CONNECTED)
		goto discard;
	/* FIXME: really process the data, even though this is wierd. */
      discard:
	freemsg(mp);
	return (0);
	return mx_data_req(mx, q, mp, p->mx_slot);
      badprim:
	return mx_error_ack(mx, q, mp, MX_DATA_REQ, MXBADPRIM, EMSGSIZE);
}

/**
 * mx_disconnect_req: - process MX_DISCONNECT_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC fastcall int
mx_disconnect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_CONNECTED)
		goto outstate;
	mx_set_state(mx, MXS_WACK_DREQ);
	/* FIXME: really do the disconnect. */
	if (mx_disconnect_ack_con(mx, q, mp, p->mx_conn_flag, p->mx_slot) != 0) {
		/* FIXME: back out of the disconnect */
		return (-ENOBUFS);
	}
	return (0);
      outstate:
	return mx_error_ack(mx, q, mp, MX_DISCONNECT_REQ, MXOUTSTATE, EPROTO);
      notsupport:
	return mx_error_ack(mx, q, mp, MX_DISCONNECT_REQ, MXNOTSUPP, EPROTO);
      badprim:
	return mx_error_ack(mx, q, mp, MX_DISCONNECT_REQ, MXBADPRIM, EMSGSIZE);

}

/**
 * mx_disable_req: - process MX_DISABLE_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC fastcall int
mx_disable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx_get_state(mx) != MXS_ENABLED)
		goto outstate;
	mx_set_state(mx, MXS_WACK_RREQ);
	/* FIXME: really do the disable. */
	if (mx_disable_ack_con(mx, q, mp) != 0) {
		/* FIXME: back out of the disable */
		return (-ENOBUFS);
	}
	return (0);
      outstate:
	return mx_error_ack(mx, q, mp, MX_DISABLE_REQ, MXOUTSTATE, EPROTO);
      notsupport:
	return mx_error_ack(mx, q, mp, MX_DISABLE_REQ, MXNOTSUPP, EPROTO);
      badprim:
	return mx_error_ack(mx, q, mp, MX_DISABLE_REQ, MXBADPRIM, EMSGSIZE);
}

/**
 * mx_detach_req: - process MX_DETACH_REQ primitive
 * @mx: MX private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC fastcall int
mx_detach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_detach_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mx->info.mx_style != MX_STYLE2)
		goto notsupport;
	if (mx_get_state(mx) != MXS_ATTACHED)
		goto outstate;
	mx_set_state(mx, MXS_WACK_UREQ);
	/* FIXME: really do the detach */
	if (mx_ok_ack(mx, q, mp, MX_DETACH_REQ, MXS_DETACHED) != 0) {
		/* FIXME: back out of the detach */
		return (-ENOBUFS);
	}
	return (0);
      outstate:
	return mx_error_ack(mx, q, mp, MX_DETACH_REQ, MXOUTSTATE, EPROTO);
      notsupport:
	return mx_error_ack(mx, q, mp, MX_DETACH_REQ, MXNOTSUPP, EPROTO);
      badprim:
	return mx_error_ack(mx, q, mp, MX_DETACH_REQ, MXBADPRIM, EMSGSIZE);
}

/**
 * mx_w_proto: M_PROTO/M_PCPROTO message processing
 * @q: write queue
 * @mp: the message to process
 */
STATIC noinline fastcall int
mx_w_proto(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	mx_ulong oldstate = mx_get_state(mx);
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(mx_ulong)) {
		swerr();
		freemsg(mp);
		return (0);
	}
	if (!test_and_set_bit(MXB_LOCK, &mx->lock)) {
		switch (*(mx_ulong *) mp->b_rptr) {
		case MX_INFO_REQ:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_INFO_REQ");
			err = mx_info_req(mx, q, mp);
			break;
		case MX_OPTMGMT_REQ:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_OPTMGMT_REQ");
			err = mx_optmgmt_req(mx, q, mp);
			break;
		case MX_ATTACH_REQ:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_ATTACH_REQ");
			err = mx_attach_req(mx, q, mp);
			break;
		case MX_ENABLE_REQ:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_ENABLE_REQ");
			err = mx_enable_req(mx, q, mp);
			break;
		case MX_CONNECT_REQ:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_CONNECT_REQ");
			err = mx_connect_req(mx, q, mp);
			break;
		case MX_DATA_REQ:
			strlog(mx->mid, mx->sid, MXLOGDA, SL_TRACE, "-> MX_DATA_REQ");
			err = mx_data_req(mx, q, mp);
			break;
		case MX_DISCONNECT_REQ:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_DISCONNECT_REQ");
			err = mx_disconnect_req(mx, q, mp);
			break;
		case MX_DISABLE_REQ:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_DISABLE_REQ");
			err = mx_disable_req(mx, q, mp);
			break;
		case MX_DETACH_REQ:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_DETACH_REQ");
			err = mx_detach_req(mx, q, mp);
			break;
		default:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "-> MX_????_???");
			freemsg(mp);
			err = 0;
			break;
		}
		/* revert to previous state on error */
		if (err)
			mx_set_state(mx, oldstate);
		clear_bit(MXB_LOCK, &mx->lock);
	} else {
		err = -EDEADLOCK;
		qenable(q);
	}
	return (err);
}

STATIC noinline fastcall __unlikely int
mx_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *dp;

	if (DB_TYPE(mp) != M_IOCTL || ioc->ioc_count == TRANSPARENT)
		goto einval;
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case MX_IOC_MAGIC:
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(MX_IOCGCONFIG):
		{
			struct mx_config *c;

			if (!(dp = allocb(sizeof(*c), BPRI_MED)))
				goto enosr;
			c = (typeof(c)) dp->b_rptr;
			dp->b_wptr += sizeof(*c);
			*c = mx->config;
			mioc2ack(q, mp, dp, sizeof(*c), 0);
			return (0);
		}
		case _IOC_NR(MX_IOCSCONFIG):
		{
			struct mx_config *c;

			if (ioc->ioc_count != sizeof(*c))
				goto einval;
			c = (typeof(c)) dp->b_rptr;
			/* FIXME: check values first */
			mx->config = *c;
			miocack(q, mp, 0, 0);
			return (0);
		}
		case _IOC_NR(MX_IOCTCONFIG):
		{
			struct mx_config *c;

			if (ioc->ioc_count != sizeof(*c))
				goto einval;
			c = (typeof(c)) dp->b_rptr;
			/* FIXME: check values */
			miocack(q, mp, 0, 0);
			return (0);
		}
		case _IOC_NR(MX_IOCCCONFIG):
		{
			struct mx_config *c;

			if (ioc->ioc_count != sizeof(*c))
				goto einval;
			c = (typeof(c)) dp->b_rptr;
			/* FIXME: check values first */
			mx->config = *c;
			miocack(q, mp, 0, 0);
			return (0);
		}
		case _IOC_NR(MX_IOCGSTATEM):
		{
			struct mx_statem *s;

			if (!(dp = allocb(sizeof(*s), BPRI_MED)))
				goto enosr;
			s = (typeof(s)) dp->b_rptr;
			dp->b_wptr += sizeof(*s);
			*s = mx->statem;
			mioc2ack(q, mp, dp, sizeof(*s), 0);
			return (0);
		}
		case _IOC_NR(MX_IOCCMRESET):
		case _IOC_NR(MX_IOCGSTATSP):
		case _IOC_NR(MX_IOCSSTATSP):
		case _IOC_NR(MX_IOCGSTATS):
		{
			struct mx_stats *s;

			if (!(dp = allocb(sizeof(*s), BPRI_MED)))
				goto enosr;
			s = (typeof(s)) dp->b_rptr;
			dp->b_wptr += sizeof(*s);
			*s = mx->stats;
			mioc2ack(q, mp, dp, sizeof(*s), 0);
			return (0);
		}
		case _IOC_NR(MX_IOCCSTATS):
		case _IOC_NR(MX_IOCGNOTIFY):
		case _IOC_NR(MX_IOCSNOTIFY):
		case _IOC_NR(MX_IOCCNOTIFY):
		case _IOC_NR(MX_IOCCMGMT):
		default:
			miocnak(q, mp, 0, EINVAL);
			return (0);
		}
	}
      einval:
	miocnak(q, mp, 0, EINVAL);
	return (0);
      enosr:
	miocnak(q, mp, 0, ENOSR);
	return (0);
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
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

STATIC noinline fastcall __hot_put int
mx_w_msg(queue_t *q, mblk_t *mp, int type)
{
	switch (type) {
	case M_PROTO:
	case M_PCPROTO:
		return mx_w_proto(q, mp);
	case M_FLUSH:
		return mx_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		/* IO controls */
		return mx_w_ioctl(q, mp);
	case M_PCCTL:
	case M_CTL:
		/* inter module IO controls */
		return mx_w_ctl(q, mp);
	default:
		swerr();
	case M_READ:
		freemsg(mp);
		return (0);
	}
}

/**
 * mx_wput: - write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 *
 * On the write side, the upper module is responsible for its own TX block
 * allocation, but is welcome to perform in-place writes on the TX block
 * passed up in M_DATA messages with the RX block.
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

/**
 * mx_w_data_slow: unusual M_DATA, but possibly stream head
 * @q: write queue
 * @mp: the M_DATA message
 */
STATIC noinline fastcall __hot_write int
mx_w_data_slow(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct vp *vp;
	int count;

	if (unlikely((vp = mx->c.vp) == NULL))
		goto disconnected;
	if (unlikely(msgdsize(mp) != 1024))
		goto wrong_size;
	if (unlikely(mp->b_cont != NULL && mp->b_wptr < mp->b_rptr + 1024 && !pullupmsg(mp, 1024)))
		goto cannot_pullup;
	if (unlikely(xchg(&mx->tx.nxt, NULL) == NULL))
		goto overflow;
	if (unlikely(test_and_set_bit(MXB_LOCK, &mx->lock)))
		goto cannot_lock;
	/* Ok, the upper module is not smart, but we managed to get 1024 bytes into a single data
	   block and are prepared to trasnfer them to the TX buffer.  Try to do this fairly
	   efficiently and lay out bytes in order of ascending addresses. */
	{
		unsigned char *frame;
		const unsigned char *bptr;

		for (bptr = mp->b_rptr, frame = vp->buf + V400P_BLOCK_SIZE;
		     frame < vp->buf + V400P_BLOCK_SIZE * 2; frame += 128) {
			uint32_t smask;
			int span;

			for (smask = mx->mask, span = 0; smask & span < 4; span++, smask >>= 1) {
				unsigned char *pos;
				uint32_t cmask;

				if ((smask & 0x1)) {
					for (cmask = mx->cmasks[span], pos = frame + span;
					     cmask & pos < frame + 128; pos += 4, cmask >>= 1) {
						if ((cmask & 0x1))
							*pos = *bptr++;
					}
				}
			}
		}
	}
	clear_bit(MXB_LOCK, &mx->lock);
	return;
      cannot_lock:
	strlog(mx->mid, mx->sid, MXLOGDA, SL_TRACE | SL_ERROR,
	       "discarding message: cannot lock stream");
	goto error;
      overflow:
	mx->tx.overflows += 8;
	strlog(mx->mix, mx->sid, MXLOGDA, SL_TRACE | SL_ERROR,
	       "discarding message: overflow tx block");
	goto error;
      cannot_pullup:
	strlog(mx->mid, mx->sid, MXLOGDA, SL_TRACE | SL_ERROR,
	       "discarding message: cannot pullup message");
	goto error;
      wrong_size:
	strlog(mx->mid, mx->sid, MXLOGDA, SL_TRACE | SL_ERROR,
	       "discarding message: wrong size %d", msgdsize(mp));
	goto error;
      unexpected:
	strlog(mx->mix, mx->sid, MXLOGDA, SL_TRACE | SL_ERROR,
	       "discarding message: unexpected tx block");
	goto error;
      disconnected:
	strlog(mx->mix, mx->sid, MXLOGDA, SL_TRACE | SL_ERROR,
	       "discarding message: stream not connected");
	goto error;
      error:
	return (-EFAULT);
}

/**
 * mx_w_data: write data for MX stream
 * @q: write queue
 * @mp: M_DATA message
 */
STATIC inline fastcall __hot_write int
mx_w_data(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct vp *vp;

	if (likely((vp = mx->c.vp) != NULL) && DB_BASE(mp) == vp->buf) {
		/* If the upper layer module is doing the right thing and simply turning around our 
		   M_DATA messages sent upstream, then an in-place write has already occurred.
		   Simply resupply the message to the tasklet. */
		if (likely(mx->tx.nxt == NULL))
			mx->tx.nxt = mp;
		else
			freemsg(mp);
	} else {
		mx_w_data_slow(q, mp);
		freemsg(mp);
	}
	return (0);
}

/**
 * mx_wput: MX write put procedure
 * @q: write queue
 * @mp: message to put
 */
STATIC streamscall __hot_write int
mx_wput(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return mx_w_data(q, mp);
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || mx_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * mx_wsrv: MX write service procedure
 * @q: write queue to service
 */
STATIC streamscall __hot_out int
mx_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mx_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	/* resupply buffers to tasklet if necessary */
	if (mx->c.vp && mx->tx.nxt == NULL) {
		frtn_t free = { &vp_rx_free, (long) mx };

		mx->tx.nxt = esballoc(mx->c.vp->buf, V400P_BLOCK_SIZE << 1, BPRI_MED, &free);
	}
	return (0);
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

static caddr_t mx_head = NULL;

STATIC streamscall int
mx_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct mx *mx;
	int err;
	minor_t minor = getminor(*devp);
	major_t major = getmajor(*devp);
	struct vp *vp = NULL;

	if (q->q_ptr)
		return (0);	/* already open */
	if (major = MX_CMAJOR_0 && (0 < minor && minor < VP_MAX_CARDS)) {
		vp = &vp_cards[minor];
		if (!(vp->hw_flags & VPF_SUPPORTED))
			return (ENXIO);
	}
	if ((err = mi_open_comm(&mx_head, sizeof(*mx), q, devp, oflag, sflag, crp)))
		return (err);
	mx = MX_PRIV(q);
	mx_init_priv(mx, q, major, minor);
	if (vp)
		mx_attach(mx, (minor << 16), MX_STYLE1);
	qprocson(q);
	return (0);
}
STATIC streamscall int
mx_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	mx_term_priv(mx);
	if ((err = mi_close_comm(&mx_head, q)))
		return (err);
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
 * vp_rx_free: - free an RX block buffer
 * @arg: client data (vp structure)
 *
 * Simply mark the RX/TX block as consumed.
 */
STATIC streamscall __hot_read void
vp_rx_free(caddr_t arg)
{
	((struct mx *) data)->rx.nxt = NULL;
}

/**
 * vp_tasklet: - process received block event
 * @data: caller data (vp pointer)
 *
 * The purpose of the RX tasklet is to process a received block and pass data to connected MX
 * streams.  Each MX stream is handed an esballoc'ed message that contains the RX/TX block for the
 * current cycle.  All streams are processed in order.
 *
 * Probably the best way to handle MX blocks is to esballoc one, dup the rest, and then free the
 * original.  Issue them directly to the upper module put procedure from here for minium latency to
 * the upper layer.  Each MX Stream gets the entire RX/TX block in Tormenta channel map
 * configuration regardless of how man spans or channels to which it has subscribed.
 *
 * The upper module can either complete the TX block in place and return the M_DATA back down
 * stream, or can save the M_DATA for passing downstream later, or can free the M_DATA.  In the
 * later case (which is what the stream head does), the upper module is responsible for generating
 * its own M_DATA TX blocks and a penalty will be incurred transfering bytes from the M_DATA block
 * to the TX block.
 *
 * If another block has become available, the tasklet is redispatched.
 *
 * Note that when processing data channels, we recycle the transmit block into the receive block
 * (because they are the same size) on the fly (read a byte, write a byte).  That is one of the
 * advantages of placing the channel in the driver (it could be implemented as a pushable module
 * pushed over a multiplex stream).  The upper layer module needs to keep TX blocks coming, but with
 * STREAMS flow control that is easy: extra TX blocks are queued and the upper layer modules needs
 * to simply supply them until flow control is acheived. 
 *
 * For multiplexes, recycled blocks represent blocks that have already transferred data to the
 * transmit block and are available to be used by the receive side.
 */
STATIC __hot_in void
vp_tasklet(unsigned long data)
{
	struct vp *vp = (struct vp *) data;
	mblk_t *mp;
	struct ch *ch;
	struct mx *mx;

	/* To only use spin locks here requires that the ISR never run (on the same processor)
	   while this tasklet is running.  Rather than block all interrupts on the processor, we
	   need to only block interrupts on the current card (e.g. by deferring acknowledgment of
	   the interrupt until after this procedure completes). */
	spin_lock(&vp->lock);

	/* process statitical multiplexed data channels */
	for (ch = vp->ch.connects; ch; ch = ch->c.next) {
		if (canput(ch->rq) && (mp = xchg(ch->tx.nxt, NULL))) {
			unsigned char *frame;
			unsigned char *bptr;

			for (bptr = mp->b_rptr, frame = vp->buf;
			     frame < vp->buf + V400P_BLOCK_SIZE; frame += 128) {
				uint32_t smask;
				int span;

				for (smask = ch->smask, span = 0; smask & span < 4;
				     span++, smask >>= 1) {
					unsigned char *pos;
					uint32_t cmask;

					if (!(smask & 0x1))
						continue;
					for (cmask = mx->cmasks[span], pos = frame + span;
					     cmask && pos < frame + 128; pos += 4, cmask >>= 1) {
						if (!(cmask & 0x1))
							continue;
						pos[1024] = *bptr;
						*bptr = *pos;
						bptr++;
					}
				}
			}
			/* The upper layer module must be very careful to simply process the data
			   quickly and return.  In the case of data channels, this should really be 
			   just a putq unless minimal latency is required. */
			putnext(ch->rq, mp);
			qenable(ch->wq);	/* resupply buffers */
		} else {
			ch->tx.underruns += ch->channels * 8;
			ch->rx.overflows += ch->channels * 8;
		}
	}

	/* process switching matrix multiplexers */
	for (mx = vp->mx.connects; mx; mx = mx->c.next) {
		/* No flow control check because only 1 block can be outstanding. */
		if (!mx->rx.nxt && (mp = xchg(ch->tx.nxt, NULL))) {
			mx->rx.nxt = mp;
			/* The upper layer module must be very careful to simply process the data
			   quickly and return.  In the case of multiplexers, this should really be
			   just a copy, qreply of the message and return. */
			putnext(mx->rq, mp);
			qenable(mx->wq);	/* resupply buffers */
		} else {
			mx->rx.overflows += mx->channels * 8;
			mx->tx.underruns += mx->channels * 8;
		}
	}

	spin_unlock(&vp->lock);

	runqueues();
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
STATIC noinline __unlikely int
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
STATIC noinline __unlikely int
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
STATIC noinline fastcall __hot void
vp_daccs(struct vp *vp)
{
	uint32_t xmask_save;

	if (unlikely((xmask_save = vp->xmask))) {
		register uint32_t *buf = vp->buf;
		register int byte, slot, chan;
		register uint32_t xmask, xmask_save = vp->xmask;
		register uint8_t **map = vp->map;

#ifdef CONFIG_SMP
		int rxdccs = vp->rxdccs;

		if (rxdccs)
			spin_lock(&vp_daccs_lock);
#endif
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
#ifdef CONFIG_SMP
		if (rxdccs)
			spin_unlock(&vp_daccs_lock);
#endif
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
STATIC inline fastcall __hot void
vp_rxtx_burst(struct vp *vp)
{
	uint32_t cmask, dmask;
	struct mx *mx;

	/* first set of 4 channels are always dead */
	if (unlikely(!((cmask = vp->cmask | vp->xmask) >> 1)))
		goto done;

	dmask = (vp->channels == 24) ? VP_TI_CHAN_VALID_MASK : VP_E1_CHAN_VALID_MASK;

	if (likely(!vp->blocks)) {
		register int chans = ((vp->channels + 7) & ~7);	/* 24 or 32 */
		register volatile uint32_t *xll = vp->xll;
		register uint32_t *buf = vp->buf;
		register int iword, islot, oword, oslot;
		register uint32_t x;

		/* first half of buffer is receive */
		for (oword = 0, iword = 0; iword < 256; iword += 32, oword += chans) {
			prefetchw(&buf[oword + chans]);
			for (x = 0x01, oslot = oword, islot = iword + 1;
			     (cmask & ~(x - 1)) && islot < iword + 32; islot++, x <<= 1) {
				if (cmask & x)	/* skip disabled channels */
					buf[oslot++] = xll[islot];
				else if (dmask & x)	/* step over allocated channels */
					oslot++;
			}

		}
		buf += chans << 3;	/* 8 words per channel */
		/* perform digital cross-connect if necessary */
		if (vp->xmask)
			vp_daccs(vp);
#ifdef CONFIG_SMP
		/* second half of buffer is transmit */
		if (vp->txdccs)
			spin_lock(&vp_daccs_lock);
#endif
		for (iword = 0, oword = 0; oword < 256; oword += 32, iword += chans) {
			prefetch(&buf[iword + chans]);
			for (x = 0x01, islot = iword, oslot = oword + 1;
			     (cmask & ~(x - 1)) && oslot < oword + 32; oslot++, x <<= 1) {
				if (cmask & x)	/* skip disabled channels */
					xll[oslot] = buf[islot++];
				else if (dmask & x)
					islot++;
			}
		}
#ifdef CONFIG_SMP
		if (vp->txdccs)
			spin_unlock(&vp_daccs_lock);
#endif

		vp->blocks++;	/* keep track of outstanding blocks */
		tasklet_hi_schedule(&vp->tasklet);
	} else {
		vp->rx.overflows += 8;
		vp->tx.underruns += 8;
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
STATIC noinline fastcall __hot void
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
STATIC noinline fastcall __hot void
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

STATIC noinline fastcall __hot void
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
STATIC __hot irqreturn_t
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

unsigned short vp_loadfw = 1;

#define VP_MAX_CARDS	8	/* No machines with more than 8 PCI slots? */

/* A static array of cards for speed. */
STATIC struct vp vp_cards[VP_MAX_CARDS];

STATIC int vp_card;

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
	if (vp_loadfw && vp_download_firmware(vp, board) != 0)
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

unsigned modID_t mx_modid = DRV_ID;
unsigned major_t mx_major = DRV_ID;

#ifdef LINUX
/*
 *  Linux registration
 */
#ifdef module_param
module_param(mx_modid, modID_t, 0);
module_param(mx_major, major_t, 0);
module_param(vp_loadfw, ushort, 0);
#else				/* module_param */
MODULE_PARM(mx_modid, "h");
MODULE_PARM(mx_major, "d");
MODULE_PARM(vp_loadfw, "h");
#endif				/* module_param */

MODULE_PARM_DESC(mx_modid, "Module ID for the V401P-MX driver. (0 for allocation.)");
MODULE_PARM_DESC(mx_major, "Device number for the V401P-MX driver. (0 for allocation.)");
MODULE_PARM_DESC(vp_loadfw, "Load firmware for the V401P driver.");

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

STATIC struct module_stat mx_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

STATIC struct qinit mx_rinit = {
	.qi_putp = NULL,
	.qi_srvp = NULL,
	.qi_qopen = mx_qopen,
	.qi_qclose = mx_qclose,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_mstat,
};

STATIC struct qinit mx_winit = {
	.qi_putp = mx_wput,
	.qi_srvp = mx_wsrv,
	.qi_minfo = &mx_minfo,
	.qi_mstat = &mx_mstat,
};

STATIC struct streamtab v400mxinfo = {
	.st_rdinit = &mx_rinit,
	.st_wrinit = &mx_winit,
};

STATIC struct cdevsw mx_cdev = {
	.d_name = DRV_NAME,
	.d_str = &v400mxinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC major_t mx_majors[MX_CMAJORS] = { MX_CMAJOR_0, };

MODULE_STATIC void __exit
v400exit(void)
{
	int err, mindex;

	for (mindex = MX_CMAJORS - 1; mindex >= 0; mindex--) {
		if (mx_majors[mindex]) {
			if ((err = unregister_strdev(&mx_cdev, mx_majors[mindex])))
				strlog(mx_modid, 0, 0, SL_FATAL, "cannot unregister major %d",
				       mx_majors[mindex]);
			if (mindex)
				mx_majors[mindex] = 0;
		}
	}
	if ((err = vp_pci_cleanup()))
		strlog(0, 0, 0, SL_WARN, "could not cleanup pci device");
	return;
}

MODULE_STATIC void __init
v400init(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = vp_pci_init())) {
		strlog(0, 0, 0, SL_WARN, "no PCI devices found, err = %d", err);
		v400exit();
		return (err);
	}
	for (mindex = 0; mindex < MX_CMAJORS; mindex++) {
		if ((err = register_strdev(&mx_cdev, mx_majors[mindex])) < 0) {
			if (mindex) {
				strlog(mx_modid, 0, 0, SL_WARN, "could not register major %d",
				       mx_majors[mindex]);
				continue;
			} else {
				strlog(mx_modid, 0, 0, SL_WARN,
				       "could not register driver, err = %d", err);
				v400exit();
				return (err);
			}
		}
		if (mx_majors[mindex] == 0)
			mx_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(mx_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (mx_major == 0)
			mx_major = mx_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 */
module_init(v400init);
module_exit(v400exit);

#endif				/* LINUX */
