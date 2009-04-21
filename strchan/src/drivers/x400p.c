/*****************************************************************************

 @(#) $RCSfile: x400p.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2009-04-21 07:48:36 $

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

 Last Modified $Date: 2009-04-21 07:48:36 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x400p.c,v $
 Revision 0.9.2.4  2009-04-21 07:48:36  brian
 - updates for release

 Revision 0.9.2.3  2008-04-28 23:39:55  brian
 - updated headers for release

 Revision 0.9.2.2  2007/08/14 06:47:29  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/10/14 06:37:27  brian
 - added manpages, module, drivers, headers from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: x400p.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2009-04-21 07:48:36 $"

static char const ident[] = "$RCSfile: x400p.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2009-04-21 07:48:36 $";

#define _DEBUG 1
//#undef _DEBUG

#define _LFS_SOURCE 1
#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1
#define _SUN_SOURCE 1

#define X400P_DOWNLOAD_FIRMWARE 1

#include <sys/os7/compat.h>

#ifdef LINUX
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#endif				/* LINUX */

#include <sys/lmi.h>
#include <sys/lmi_ioctl.h>
#include <sys/mxi.h>
#include <sys/mxi_ioctl2.h>
#include <sys/dsx_ioctl.h>

#ifdef X400P_DOWNLOAD_FIRMWARE
#include <v401pfw.h>
#endif

#define MX_X400P_DESCRIP    "X400P: MX (Multiplex) STREAMS DRIVER."
#define MX_X400P_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MX_X400P_REVISION	"OpenSS7 $RCSfile: x400p.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2009-04-21 07:48:36 $"
#define MX_X400P_COPYRIGHT	"Copyright (c) 1997-2009 Monavacon Limited.  All Rights Reserved."
#define MX_X400P_DEVICE		"Supports the X40XP E1/T1/J1 (Tormenta II/III) PCI boards."
#define MX_X400P_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MX_X400P_LICENSE	"GPL"
#define MX_X400P_BANNER		MX_X400P_DESCRIP	"\n" \
				MX_X400P_EXTRA		"\n" \
				MX_X400P_REVISION	"\n" \
				MX_X400P_COPYRIGHT	"\n" \
				MX_X400P_DEVICE		"\n" \
				MX_X400P_CONTACT	"\n"
#define MX_X400P_SPLASH		MX_X400P_DESCRIP	" - " \
				MX_X400P_REVISION

#ifdef LINUX
MODULE_AUTHOR(MX_X400P_CONTACT);
MODULE_DESCRIPTION(MX_X400P_DESCRIP);
MODULE_SUPPORTED_DEVICE(MX_X400P_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MX_X400P_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mx_x400p");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
		PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define MX_X400P_DRV_ID		CONFIG_STREAMS_MX_X400P_MODID
#define MX_X400P_DRV_NAME	CONFIG_STREAMS_MX_X400P_NAME
#define MX_X400P_CMAJORS	CONFIG_STREAMS_MX_X400P_NMAJORS
#define MX_X400P_CMAJOR_0	CONFIG_STREAMS_MX_X400P_MAJOR
#define MX_X400P_UNITS		CONFIG_STREAMS_MX_X400P_NMINORS
#endif

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_MX_X400P_MODID));
MODULE_ALIAS("streams-driver-mx-x400p");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_MX_X400P_MAJOR));
MODULE_ALIAS("/dev/streams/x400p-mx");
MODULE_ALIAS("/dev/streams/x400p-mx/*");
MODULE_ALIAS("/dev/streams/clone/x400p-mx");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(MX_X400P_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(MX_X400P_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(MX_X400P_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/x400p-mx");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

enum xp_board {
	PLX9030 = 0,
	PLXDEVBRD,
	X400P,
	E400P,
	T400P,
	X400PSS7,
	E400PSS7,
	T400PSS7,
	V400P,
	V400PE,
	V400PT,
	V401PE,
	V401PT,
};

/* indexed by xp_board above */

/* *INDENT-OFF* */
static struct {
	char *name;
	uint32_t hw_flags;
	uint32_t idle_word;
} xp_board_info[] __devinitdata = {
	{ "PLX 9030",			0, 0x00000000 },
	{ "PLX Development Board",	0, 0x00000000 },
	{ "X400P",			1, 0xffffffff },
	{ "E400P",			1, 0xffffffff },
	{ "T400P",			1, 0xfefefefe },
	{ "X400P-SS7",			1, 0xffffffff },
	{ "E400P-SS7",			1, 0xffffffff },
	{ "T400P-SS7",			1, 0xfefefefe },
	{ "V400P",			1, 0xffffffff },
	{ "V400PE",			1, 0xffffffff },
	{ "V400PT",			1, 0xfefefefe },
	{ "V401PE",			1, 0xffffffff },
	{ "V401PT",			1, 0xfefefefe },
};

#define XPF_SUPPORTED	(1<<0)

#define XP_DEV_IDMASK	0xf0
#define XP_DEV_SHIFT	4
#define XP_DEV_REVMASK	0x0f
#define XP_DEV_DS2152	0x00
#define XP_DEV_DS21352	0x01
#define XP_DEV_DS21552	0x02
#define XP_DEV_DS2154	0x08
#define XP_DEV_DS21354	0x09
#define XP_DEV_DS21554	0x0a
#define XP_DEV_DS2155	0x0b
#define XP_DEV_DS2156	0x0c

STATIC struct {
	char *name;
	uint32_t hw_flags;
} xp_device_info[] __devinitdata = {
	{ "DS2152 (T1)",	1 },
	{ "DS21352 (T1)",	1 },
	{ "DS21552 (T1)",	1 },
	{ "Unknown ID 0011",	0 },
	{ "Unknown ID 0100",	0 },
	{ "Unknown ID 0101",	0 },
	{ "Unknown ID 0110",	0 },
	{ "Unknown ID 0111",	0 },
	{ "DS2154 (E1)",	1 },
	{ "DS21354 (E1)",	1 },
	{ "DS21554 (E1)",	1 },
	{ "DS2155 (E1/T1/J1)",	1 },
	{ "DS2156 (E1/T1/J1)",	1 },
	{ "Unknown ID 1101",	0 },
	{ "Unknown ID 1110",	0 },
	{ "Unknown ID 1111",	0 }
};

STATIC struct pci_device_id xp_pci_tbl[] __devinitdata = {
	{PCI_VENDOR_ID_PLX, 0x9030, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_BRIDGE_OTHER << 8, 0xffff00, PLX9030},
	{PCI_VENDOR_ID_PLX, 0x3001, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_BRIDGE_OTHER << 8, 0xffff00, PLXDEVBRD},
	{PCI_VENDOR_ID_PLX, 0xD00D, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_BRIDGE_OTHER << 8, 0xffff00, X400P},
	{PCI_VENDOR_ID_PLX, 0x0557, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_BRIDGE_OTHER << 8, 0xffff00, X400PSS7},
	{PCI_VENDOR_ID_PLX, 0x4000, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_BRIDGE_OTHER << 8, 0xffff00, V400P},
	{PCI_VENDOR_ID_PLX, 0xD33D, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_BRIDGE_OTHER << 8, 0xffff00, V401PT},
	{PCI_VENDOR_ID_PLX, 0xD44D, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_BRIDGE_OTHER << 8, 0xffff00, V401PE},
	{0,}
};
/* *INDENT-ON* */

#ifdef MODULE_DEVICE_TABLE
MODULE_DEVICE_TABLE(pci, xp_pci_tbl);
#ifdef MODULE_ALIAS
MODULE_ALIAS("pci:v000010B5d000009030sv*sd*bc06sc80i*");
MODULE_ALIAS("pci:v000010B5d000003001sv*sd*bc06sc80i*");
MODULE_ALIAS("pci:v000010B5d00000D00Dsv*sd*bc06sc80i*");
MODULE_ALIAS("pci:v000010B5d000000557sv*sd*bc06sc80i*");
MODULE_ALIAS("pci:v000010B5d000004000sv*sd*bc06sc80i*");
MODULE_ALIAS("pci:v000010B5d00000D33Dsv*sd*bc06sc80i*");
MODULE_ALIAS("pci:v000010B5d00000D44Dsv*sd*bc06sc80i*");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE_DEVICE_TABLE */

/* Map from Tormenta channel to T1 time slot (less 1). */
STATIC int xp_t1_slot_map[] = {
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
STATIC int xp_e1_slot_map[] = {
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
STATIC int xp_t1_chan_map[] = {
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
STATIC int xp_e1_chan_map[] = {
	1, 2, 3, 4,
	5, 6, 7, 8,
	9, 10, 11, 12,
	13, 14, 15, 16,
	17, 18, 19, 20,
	21, 22, 23, 24,
	25, 26, 27, 28,
	29, 30, 31
};

#define XP_T1_TS_VALID_MASK	0x00ffffff	/* Mask of valid T1 time slots. */
#define XP_E1_TS_VALID_MASK	0x7fffffff	/* Mask of valid E1 time slots. */
#define XP_T1_CHAN_VALID_MASK	0xeeeeeeee	/* Mask of valid T1 Tormenta channels. */
#define XP_E1_CHAN_VALID_MASK	0xfffffffe	/* Mask of valid E1 Tormenta channels. */

#ifdef __LITTLE_ENDIAN
#define span_to_byte(__span) (3-(__span))
#else
#ifdef __BIG_ENDIAN
#define span_to_byte(__span) (__span)
#else
#error "Must know the endianess of processor\n"
#endif
#endif

/*
 *  Private structures.
 */
struct mx;
struct sp;
struct xp;

struct mx {
	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	struct MX_info_ack info;	/* information */
	uint32_t addr;			/* address */
	struct MX_parms_circuit parm;	/* parameters */
	int spans;			/* number of spans in span mask (0-8) */
	int channels;			/* number of channels in channel mask (0-31) */

	struct mx_config config;
	struct mx_stats stats;
	struct mx_statem state;
	struct mx_notify notify;

	struct {
		uint32_t smask;		/* mask of spans */
		uint32_t cmask;		/* mask of slots */
		uint32_t cmasks[4];	/* mask of chans */
		struct xp *xp;		/* xp card */
		struct mx *next;	/* xp card linkage */
		struct mx **prev;	/* xp card linkage */
	} a, c, x;			/* attached, connected and cross-connected */

	struct {
		ulong overflows;
		ulong underruns;
		mblk_t *nxt;
	} rx, tx;
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
	uint32_t cmask;			/* mask of enabled channels for this span */
	uint32_t xmask;			/* mask of cross-connected channels for this span */
	ulong bpvs;			/* bipolar violations */
	ulong fass;
	ulong crc4;
	ulong ebit;
	struct xp *xp;			/* card for this span */
	ulong recovertime;		/* alarm recover time */
	ulong iobase;			/* span iobase */
	int span;			/* index (span) */
	volatile ulong loopcnt;		/* loop command count */
	struct mx_attr_span attr;	/* span attributes */
};

struct xp {
	uint card;
	uint board;
	uint device;
	uint devrev;
	uint hw_flags;
	spinlock_t lock;
	uint flags;
	struct tasklet_struct tasklet;
	volatile uint32_t *buf;
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
	volatile int eval_syncsrc;	/* need to reevaluate sync source */
	volatile int leds;		/* leds on the card */
	int irq;			/* card irq */
	ulong iobase;			/* card iobase */

	int **slot_map;			/* set to xp_t1_slot_map or xp_e1_slot_map */
	int **chan_map;			/* set to xp_t1_chan_map or xp_e1_chan_map */

	struct mx_attr_card attr;	/* card attributes */

	struct {
		struct mx *attached;	/* attached mx list */
		struct mx *connects;	/* connected mx list */
		struct mx *xconnect;	/* cross-connected mx list */
	} mx;

	int blocks;			/* number of RX/TX blocks outstanding */
	int spans;			/* number of spans, always 4 for current cards */
	int channels;			/* number of channels per span: 24 or 31 for current cards */

	uint32_t smask;			/* mask of enabled spans */
	uint32_t cmask;			/* mask of enabled channels (for any span) */
	uint32_t xmask;			/* mask of cross-connected channels (for any span) */
	struct {
		volatile uint32_t *buf;	/* elastic buffer */
		int uebno;		/* upper elastic buffer number */
		int dccs;		/* number of digital cross-connects to this card from other cards */
	} tx;
	struct {
		volatile uint32_t *buf;	/* elastic buffer */
		int uebno;		/* upper elastic buffer number */
		int dccs;		/* number of digital corss-connects from this card to other cards */
	} tx;
	struct sp spans[4];		/* only four spans for now */
	volatile uint8_t *map[32][4];	/* digital cross-connect map from this card */
};

#ifdef CONFIG_SMP
STATIC spinlock_t xp_daccs_lock = SPIN_LOCK_UNLOCKED;
#endif

#define X400P_EBUFNO	(1<<7)	/* 128k of elastic buffers per card. */

static __unlikely void
mx_init_priv(queue_t *q, major_t major, minor_t minor)
{
	struct mx *mx = MX_PRIV(q);

	bzero(mx, sizeof(*mx));
	mx->rq = q;
	mx->wq = _WR(q);

	mx->info.mx_primitive = MX_INFO_ACK;
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

	mx->parm.mp_type = MX_PARMS_CIRCUIT;
	mx->parm.mp_encoding = MX_ENCODING_NONE;
	mx->parm.mp_block_size = 8192;
	mx->parm.mp_samples = 8;
	mx->parm.mp_sample_size = 8;
	mx->parm.mp_rate = 8000;
	mx->parm.mp_tx_channels = 31;
	mx->parm.mp_rx_channels = 31;
	mx->parm.mp_opt_flags = 0;

	mx->spans = 0;
	mx->channels = 0;

	mx->a.smask = 0;
	mx->a.cmask = 0;
	mx->a.cmasks[0] = 0;
	mx->a.cmasks[1] = 0;
	mx->a.cmasks[2] = 0;
	mx->a.cmasks[3] = 0;
	mx->a.xp = NULL;
	mx->a.next = NULL;
	mx->a.prev = &mx->a.next;

	mx->c.smask = 0;
	mx->c.cmask = 0;
	mx->c.cmasks[0] = 0;
	mx->c.cmasks[1] = 0;
	mx->c.cmasks[2] = 0;
	mx->c.cmasks[3] = 0;
	mx->c.xp = NULL;
	mx->c.next = NULL;
	mx->c.prev = &mx->c.next;

	mx->x.smask = 0;
	mx->x.cmask = 0;
	mx->x.cmasks[0] = 0;
	mx->x.cmasks[1] = 0;
	mx->x.cmasks[2] = 0;
	mx->x.cmasks[3] = 0;
	mx->x.xp = NULL;
	mx->x.next = NULL;
	mx->x.prev = &mx->x.next;

	mx->rx.overflows = 0;
	mx->rx.underruns = 0;
	mx->rx.nxt = NULL;

	mx->tx.overflows = 0;
	mx->tx.underruns = 0;
	mx->tx.nxt = NULL;
}

STATIC __unlikely void mx_disconnect(struct mx *mx, bool force);
STATIC __unlikely void mx_disable(struct mx *mx, bool force);
STATIC __unlikely void mx_detach(struct mx *mx, bool force);

STATIC __unlikely void
mx_term_priv(queue_t *q)
{
	struct mx *mx = MX_PRIV(q);

	switch (mx_get_state(mx)) {
	default:
		/* don't know, run them anyway */
		/* fall through */
	case MXS_CONNECTED:
		mx_disconnect(mx, true); /* forced */
		/* fall through */
	case MXS_ENABLED:
		mx_disable(mx, true); /* forced */
		/* fall through */
	case MXS_ATTACHED:
		mx_detach(mx, true); /* forced */
		/* fall through */
	case MXS_DETACHED:
		break;
	}

}

/**
 * mx_attach: attach an MX stream
 * @mx: MX private structure (locked)
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
	struct xp *xp;
	int card = (ppa >> 16) & 0x0ff;
	int span = (ppa >> 8) & 0x0ff;
	int i, p, slot = (ppa >> 0) & 0x0ff;

	/* locate card */
	xp = &xp_cards[card];

	/* fill out span mask and count */
	if (span == 0) {
		mx->smask = (1 << vp->spans) - 1;
		mx->spans = xp->spans;
	} else {
		mx->smask = (1 << (span - 1));
		mx->spans = 1;
	}
	/* fill out channel mask and count */
	xmask = (xp->channels == 24) ? XP_T1_CHAN_VALID_MASK : XP_E1_CHAN_VALID_MASK;
	for (mx->cmask = 0, i = 0, p = 1; i < xp->spans; i++, p <<= 1) {
		if (!(mx->smask & p))
			continue;
		if (slot = 0) {
			mx->cmask |= xmask;
			mx->cmasks[i] = xmask;
			mx->channels = xp->channels;
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
	if ((mx->a.next = xp->mx.attached))
		mx->a.next->a.prev = &mx->a.next;
	mx->a.prev = &xp->mx.attached;
	xp->mx_attached = mx;
	mx->a.xp = xp;

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

noinline __unlikely int xp_cross_connect(uint32_t, uint32_t, uint32_t);

/**
 * mx_connect: connect an MX stream
 * @mx: MX private structure
 * @cflags: connect flags
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
mx_connect(struct mx *mx, mx_ulong cflags, mx_ulong slot)
{
	uint32_t slot = mx->addr[1];
	unsigned long flags;

	/* save and report the new address */
	mx->addr[1] = slot;
	mx->info.mx_addr_length = sizeof(mx->addr[0]) + sizeof(mx->addr[1]);
	mx->info.mx_parm_offset = sizeof(mx->info) + mx->info.mx_addr_length;

	/* slot must have been checked for correctness by the calling routine */
	if (slot == 0) {
		struct xp *xp, = mx->a.xp;
		int i;

		/* no digital cross-connect */
		spin_lock_irqsave(&xp->lock, flags);
		{
			/* Link the mx into the card's connected MX list.  Data will not be
			   transferred until after we release the card spin lock. */
			if ((mx->c.next = xp->mx_connects))
				mx->c.next->c.prev = &mx->c.next;
			mx->c.prev = &xp->mx_connects;
			mx->c.xp = xp;
			xp->mx.connects = mx;

			/* Recalculate card masks.  The new masks will be immediately effective
			   once the spin lock is released. */
			for (xp->smask = 0, xp->cmask = 0, mx = xp->mx.connects; mx;
			     mx = mx->c.next) {
				xp->smask |= mx->smask;
				for (i = 0; i < 8; i++)
					if (mx->smask & (1 << i))
						xp->cmask |= mx->cmask[i];
			}
		}
		spin_unlock_irqrestore(&xp->lock, flags);
	} else {
		struct xp *xp = &xp->cards[((slot >> 16) & 0xff)];

		/* digital cross-connect */

		/* link mx into card's cross-connecting MX list */
		spin_lock_irqsave(&xp->lock, flags);
		{
			if ((mx->x.next = xp->mx.xconnect))
				mx->x.next->a.prev = &mx->x.next;
			mx->a.prev = &xp->mx.xconnect;
			xp->mx.xconnect = mx;
			mx->x.xp = xp;
		}
		spin_unlock_irqrestore(&xp->lock, flags);

		/* Complete the digital cross-connect and connect channel maps by OR'ing the MX
		   Streams maps into the VP cards' maps.  Complete the digital cross-connect
		   tables.  */
		xp_cross_connect(mx->addr[0], mx->addr[1], mx->channels);

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
		struct xp *xp = mx->a.xp;
		int i;

		/* no digital cross-connect */
		/* Recalculate the XP card's channel map by OR'ing the channel maps of the
		   remaining connected cards. */
		spin_lock_irqsave(&xp->lock, flags);
		{
			/* Remove from connected list.  Data will not be transferred from the point
			   that we took the spin lock. */
			if ((*mx->c.prev = mx->c.next))
				mx->c.next->c.prev = mx->c.prev;
			mx->c.next = NULL;
			mx->c.prev = &mx->c.next;
			mx->c.xp = NULL;

			/* Recalculate card masks.  The new masks will be immediately effective
			   once the spin lock is released. */
			for (xp->smask = 0, xp->cmask = 0, mx = xp->mx.connects; mx;
			     mx = mx->c.next) {
				xp->smask |= mx->smask;
				for (i = 0; i < 8; i++)
					if (mx->smask & (1 << i))
						xp->cmask |= mx->cmask[i];
			}
		}
		spin_unlock_irqrestore(&xp->lock, flags);
	} else {
		/* digital cross-connect */
		/* Remove cross connect from the attached and target XP cards' xconnect tables. */
		/* Recalculate both the attached and xconnected XP card's xconnect maps by OR'ing
		   the xconnect maps of the remaining cross-connected cards. */
		xp_cross_disconnect(mx->addr[0], mx->addr[1], mx->channels);

		/* remove from pseudo-digital cross-connect list */
		if (mx->x.xp) {
			struct xp *xp = mx->x.xp;

			spin_lock_irqsave(&xp->lock, flags);
			{
				if ((*mx->x.prev = mx->x.next))
					mx->x.next->x.prev = mx->x.prev;
				mx->x.next = NULL;
				mx->x.prev = &mx->x.next;
				mx->x.xp = NULL;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
		}
	}

	/* This is problematic.  There is a period of time between when the MX Stream moves to the
	   disconnected state and the MX_DISCONNECT_CON or MX_DISCONNECT_IND is delivered.  In this 
	   time, some a fair amount of data could be delivered.  It is wise to flush both the read
	   and the write side of the Stream before delivering either the MX_DISCONNECT_CON or
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
	if (mx->a.xp) {
		if ((*mx->a.prev = mx->a.next))
			mx->a.next->a.prev = mx->a.prev;
		mx->a.next = NULL;
		mx->a.prev = &mx->a.next;
		mp->xp = NULL;
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
 *  links).  CH streams should not span multiple spans unless the spans are synchonization locked
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

#if 0
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

	if ((mp = mi_allocb(q, sizeof(*p) + alen + plen, BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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
#endif

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

	if ((mp = mi_allocb(q, sizeof(*p) + alen + plen, BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = MX_ATTACH_REQ;
		p->mx_state = MXS_ATTACHED;
		mp->b_wptr += sizeof(*p);
		mx_attach(q, ppa, MX_STYLE2);
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
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

/**
 * xp_span_config: - initial configuration of a single span
 * @vp: card structure
 * @span: span number to configure
 * @timeouts: whether busy looping is permissable
 *
 * NOTE: we should actually do a schedule_timeout instead of a busy loop to get the 100ms delay
 * that are necessary.  What we are really looking for is usleep() from SVR4.  For now we busy
 * loop but it MUST CHANGE.  Once spans are configured at start-up with this longer
 * configuration process, all but a few registers do not need to change.
 *
 * A couple notes.  It might be an idea to leave transmitters powered off or transmit all ones
 * to blue alarm the other end.  When the span is reconfigured, we can power transmitters or
 * resume normal framing operation.  During the first configuration, it might be an idea to set
 * up the DS2155 idle registers and per-channel idle registers.
 */
STATIC noinline __unlikely int
xp_span_config(struct xp *xp, int span, bool timeouts)
{
	struct sp *sp = xp->spans[span];
	volatile unsigned char *xlb = &xp->xlb[span << 8];
	int offset;
	unsigned long timeout;

	switch (xp->board) {
	case V400PE:
	case E400P:
	case E400PSS7:
	{
		uint8_t ccr1 = 0, ccr2 = 0, tcr2 = 0, licr = 0, test3 = 0;

		switch (sp->options.mxSpanClocking) {
		case MXSPANCLOCKING_NONE:
		case MXSPANCLOCKING_LOOP:
		case MXSPANCLOCKING_LOCAL:
		case MXSPANCLOCKING_INTERNAL:
		case MXSPANCLOCKING_EXTERNAL:
		case MXSPANCLOCKING_ADAPTIVE:
		}

		xlb[0x1a] = 0x04;	/* CCR2: set LOTCMC */

		for (offset = 0; offset <= 8; offset++)
			xlb[offset] = 0x00;
		for (offset = 0x10; offset <= 0x4f; offset++)
			if (offset != 0x1a)
				xlb[offset] = 0x00;

		xlb[0x1a] = ccr2;	/* CCR2 */
		/* CCR2.7: 0 = update error counters once a second */
		/* CCR2.6: 0 = count BPVs */
		/* CCR2.5: 0 = automatic transmit AIS generation disabled */
		/* CCR2.4: 0 = automatic transmit RAI generation disabled */
		/* CCR2.3: 0 = RSER as received under all conditions */
		/* CCR2.2: X = do not/do switch to RCLKO if TCLK stops */
		/* CCR2.1: 0 = do not force a freeze event */
		/* CCR2.0: 0 = no freezing of receive signalling data */

		xlb[0x1b] = 0x82;	/* CCR3 */
		/* CCR3.7: 1 = Tx elastic store is enabled */
		/* CCR3.6: 0 = define operation of TCHBLK output pin */
		/* CCR3.5: 0 = TIRs define in which channels to insert idle code */
		/* CCR3.4: 0 = unassigned (elastic store reset DS2154) */
		/* CCR3.3: 0 = do not reinsert signalling bits at RSER (LIRST on DS2153) */
		/* CCR3.2: 0 = do not insert signalling from TSIG pin */
		/* CCR3.1: 1 = TSYSCLK is 2.048/4.096/8.192 MHz */
		/* CCR3.0: 0 = RCL declared upon 255 consecutive zeros */

		switch (cd->device) {
		case 0:	/* no IDR means DS2153 */
			/* No CCR4 or CCR5 on DS2153. */
			break;
		case XP_DEV_DS2154:
		case XP_DEV_DS21354:
		case XP_DEV_DS21554:
			xlb[0xa8] = 0x00;	/* CCR4 */
			/* CCR4.7: 0 = remote loopback disabled */
			/* CCR4.6: 0 = local loopback disabled */
			/* CCR4.5: 0 = transmit normal data */
			/* CCR4.4: 0 = TCM bit 4 (unused) */
			/* CCR4.3: 0 = TCM bit 3 (unused) */
			/* CCR4.2: 0 = TCM bit 2 (unused) */
			/* CCR4.1: 0 = TCM bit 1 (unused) */
			/* CCR4.0: 0 = TCM bit 0 (unused) */

			xlb[0xaa] = 0x00;	/* CCR5 */
			/* CCR5.7: 0 = no immediate LIRST */
			/* CCR5.6: 0 = no RES align (not assigned DS2154) */
			/* CCR5.5: 0 = no TES align (not assigned DS2154) */
			/* CCR5.4: 0 = RCM bit 4 (unused) */
			/* CCR5.3: 0 = RCM bit 3 (unused) */
			/* CCR5.2: 0 = RCM bit 2 (unused) */
			/* CCR5.1: 0 = RCM bit 1 (unused) */
			/* CCR5.0: 0 = RCM bit 0 (unused) */
			break;
		}

		switch (cd->device) {
		case 0:	/* no IDR means DS2153 */
			/* No CCR6 on DS2153. */
			break;
		case XP_DEV_DS2154:
			/* Note: no CCR6 on DS2154. */
			break;
		case XP_DEV_DS21354:
		case XP_DEV_DS21554:
			xlb[0x1d] = ccr6;	/* CRC6 */
			/* CCR6.7: 0 = TTIP and TRING normal */
			/* CCR6.6: 0 = normal data */
			/* CCR6.5: 0 = E1 signal */
			/* CCR6.4: 0 = unassigned */
			/* CCR6.3: 0 = unassigned */
			/* CCR6.2: X = Tx clock determined by CRC2.2 (LOTCMC)/Tx clock is RCLK */
			/* CCR6.1: 0 = no RES reset */
			/* CCR6.0: 0 = no TES reset */
			break;
		}

		xlb[0x10] = 0x20;	/* RCR1 */
		/* RCR1.7: 0 = CAS/CRC4 multiframe */
		/* RCR1.6: 0 = frame mode */
		/* RCR1.5: 1 = RSYNC is an input */
		/* RCR1.4: 0 = unassigned */
		/* RCR1.3: 0 = unassigned */
		/* RCR1.2: 0 = resync if FAS received in error 3 consecutive times */
		/* RCR1.1: 0 = auto resync enabled */
		/* RCR1.0: 0 = no immediate resync */

		xlb[0x11] = 0x06;	/* RCR2 */
		/* RCR2.7: 0 = RLCLK low during Sa8 bit */
		/* RCR2.6: 0 = RLCLK low during Sa7 bit */
		/* RCR2.5: 0 = RLCLK low during Sa6 bit */
		/* RCR2.4: 0 = RLCLK low during Sa5 bit */
		/* RCR2.3: 0 = RLCLK low during Sa4 bit */
		/* RCR2.2: 1 = RSYSCLK is 2.048/4.096/8.192 MHz */
		/* RCR2.1: 1 = elastic store (receive) enabled */
		/* RCR2.0: 0 = unassigned */

		tcr1 = 0x09;
		/* TCR1.7: 0 = bipolar data at TPOS0 and TNEG0 */
		/* TCR1.6: 0 = FAS bits/Sa bits/RAI sources from TAF and TNAF registers */
		/* TCR1.5: 0 = sample time slot 16 at TSER pin */
		/* TCR1.4: 0 = transmit data normally */
		/* TCR1.3: 1 = source Si bits from TAF and TNAF regsiters (TCR1.6 must be zero) */
		/* TCR1.2: 0 = transmit data normally */
		/* TCR1.1: 0 = frame mode */
		/* TCR1.0: 1 = TSYNC is an output */

		switch (sp->config.mxSpanFraming) {
		default:
			sp->config.mxSpanFraming = MXSPANFRAMING_CCS;
		case MXSPANFRAMING_CCS:
			ccr1 |= 0x08;	/* CCR1.3: 1 = Rx CCS signaling mode */
			tcr1 &= ~0x20;	/* TCR1.5: 0 = sample time slot 16 at TSER pin */
			break;
		case MXSPANFRAMING_CAS:
			ccr1 &= ~0x08;	/* CCR1.3: 0 = Rx CAS signaling mode */
			tcr1 |= 0x20;	/* TCR1.5: 1 = source time slot 16 from TS0 to TS15
					   registers */
			break;
		}
		switch (sp->config.mxSpanCoding) {
		default:
			sp->config.mxSpanCoding = MXSPANCODING_HDB3;
		case MXSPANCODING_HDB3:
			ccr1 |= 0x40;	/* CCR1.6: 1 = Tx HDB3 enabled */
			ccr1 |= 0x04;	/* CCR1.2: 1 = Rx HDB3 enabled */
			break;
		case MXSPANCODING_AMI:
			ccr1 &= ~0x40;	/* CCR1.6: 0 = Tx HDB3 disabled */
			ccr1 &= ~0x04;	/* CCR1.2: 0 = Rx HDB3 disabled */
			break;
		}
		switch (sp->config.mxSpanCrc) {
			sp->config.mxSpanCrc = MXSPANCRC_CRC5;
		case MXSPANCRC_CRC5:
			ccr1 &= ~0x10;	/* CRC1.4: 0 = Tx CRC4 disabled */
			ccr1 &= ~0x01;	/* CRC1.0: 0 = Rx CRC4 disabled */
			break;
		case MXSPANCRC_CRC4:
			ccr1 |= 0x10;	/* CRC1.4: 1 = Tx CRC4 enabled */
			ccr1 |= 0x01;	/* CRC1.0: 1 = Rx CRC4 enalled */
			tcr2 |= 0x02;	/* TCR2.1: 1 = E-bits automatically set */
#if 0
			if (sp->config.ifframing == SDL_FRAMING_CAS)
				tcr1 |= 0x02;	/* TCR1.1: 1 = CAS and CRC4 multiframe mode */
#endif
			break;
		}

		xlb[0x12] = tcr1;	/* TCR1 */
		/* TCR1.7: 0 = bipolar data at TPOS0 and TNEG0 */
		/* TCR1.6: 0 = FAS bits/Sa bits/RAI sources from TAF and TNAF registers */
		/* TCR1.5: X = sample TS 16 at TSER pin/source TS 16 from TS0 to TS15 */
		/* TCR1.4: 0 = transmit data normally */
		/* TCR1.3: 1 = source Si bits from TAF and TNAF regsiters (TCR1.6 must be zero) */
		/* TCR1.2: 0 = transmit data normally */
		/* TCR1.1: X = frame mode/CAS and CRC4 multiframe mode */
		/* TCR1.0: 1 = TSYNC is an output */

		xlb[0x13] = tcr2;	/* TCR2 */
		/* TCR2.7: 0 = do not source Sa8 bit from TLINK pin */
		/* TCR2.6: 0 = do not source Sa7 bit from TLINK pin */
		/* TCR2.5: 0 = do not source Sa6 bit from TLINK pin */
		/* TCR2.4: 0 = do not source Sa5 bit from TLINK pin */
		/* TCR2.3: 0 = do not source Sa4 bit from TLINK pin */
		/* TCR2.2: 0 = TPOSO/TNEGO full clock period */
		/* TCR2.1: X = E-bits not automatically/automatically set */
		/* TCR2.0: 0 = RLOS/LOTC pin is RLOS */

		xlb[0x14] = ccr1;	/* CCR1 */
		/* CCR1.7: 0 = framer loopback disabled */
		/* CCR1.6: X = Tx HDB3 disabled/enabled */
		/* CCR1.5: 0 = Tx G.802 disabled */
		/* CCR1.4: X = Tx CRC4 disabled */
		/* CCR1.3: X = signalling mode CAS/CCS */
		/* CCR1.2: X = Rx HDB3 disabled/enabled */
		/* CCR1.1: 0 = Rx G.802 disabled */
		/* CCR1.0: X = Rx CRC4 disabled */

		licr = 0x00;
		/* LICR.7-5: 0x0 = 75 Ohm w/o protection resistors */
		/* LICR.4: 0 = -12dB Rx EGL */
		/* LICR.3: 0 = JA on receive side */
		/* LICR.2: 0 = JA buffer depth 128 bits */
		/* LICR.1: 0 = JA enabled */
		/* LICR.0: X = transmitters on/off */

		test3 = 0x00;
		/* TEST3: 0x00 = no Rx gain */

		switch (sp->mxSpanLineImpedance) {
		default:
			sp->mxSpanLineImpedance = MXSPANLINEIMPEDANCE_BALANCED120OHM;
		case MXSPANLINEIMPEDANCE_BALANCED120OHM:
		case MXSPANLINEIMPEDANCE_BALANCED100OHM:
			licr = 0x20; /* LICR: 120 Ohm -12db Rx EGL */
			break;
		case MXSPANLINEIMPEDANCE_UNBALANCED75OHM:
			licr = 0x00; /* LICR: 75 Ohm -12db Rx EGL */
			break;
		}
		switch (sp->mxSpanLineGain) {
		default:
			sp->mxSpanLineGain = MXSPANLINEGAIN_NONE;
		case MXSPANLINEGAIN_NONE:
		case MXSPANLINEGAIN_MON0DB:
			test3 = 0x00;	/* TEST3: 0x00 = no Rx gain */
			break;
		case MXSPANLINEGAIN_MON12DB:
		case MXSPANLINEGAIN_MON20DB:
		case MXSPANLINEGAIN_MON26DB:
			test3 = 0x72;	/* TEST3: 0x72 = 12dB Rx gain */
			break;
		case MXSPANLINEGAIN_MON30DB:
		case MXSPANLINEGAIN_MON32DB:
			test3 = 0x70;	/* TEST3: 0x70 = 30dB Rx gain */
			break;
		}
		switch (sp->mxSpanTxLevel) {
		default:
			sp->mxSpanTxLevel = MXSPANTXLEVEL_ON;
		case MXSPANTXLEVEL_ON:
			licr &= ~0x01;
			break;
		case MXSPANTXLEVEL_OPEN:
		case MXSPANTXLEVEL_OFF:
			licr |= 0x01;
			break;
		}
		switch (sp->mxSpanRxLevel) {
		default:
			sp->mxSpanRxLevel = MXSPANRXLEVEL_ON;
		case MXSPANRXLEVEL_ON:
			break;
		case MXSPANRXLEVEL_OPEN:
		case MXSPANRXLEVEL_OFF:
			break;
		}

		xlb[0xac] = test3;
		/* TEST3: 0x00 = no Rx gain */
		/* TEST3: 0x70 = 12dB Rx gain */
		/* TEST3: 0x72 = 30dB Rx gain */

		xlb[0x18] = licr;
		/* LICR.7: X = LBO bit 2 */
		/* LICR.6: X = LBO bit 1 */
		/* LICR.5: X = LBO bit 0 */
		/* LICR.4: X = -12dB/-43dB Rx EGL */
		/* LICR.3: 0 = JA on receive side */
		/* LICR.2: 0 = JA buffer depth 128 bits */
		/* LICR.1: 0 = JA enabled */
		/* LICR.0: X = transmitters on/off */

		/* Note: The TAF register must be programmed with the 7-bit FAS word (0x1b).  The
		   DS21354/DS21554 do not automatically set these bits. */
		xlb[0x20] = 0x1b;	/* TAFR */
		/* TAF.7: 0 = Si bit */
		/* TAF.6: 0 = frame alignment signal bit */
		/* TAF.5: 0 = frame alignment signal bit */
		/* TAF.4: 1 = frame alignment signal bit */
		/* TAF.3: 1 = frame alignment signal bit */
		/* TAF.2: 0 = frame alignment signal bit */
		/* TAF.1: 1 = frame alignment signal bit */
		/* TAF.0: 1 = frame alignment signal bit */

		/* Note: bit 6 of the TNAF register must be programmed to one.  The DS21354/DS21554 
		   does not automaticallly set this bit. */
		xlb[0x21] = 0x5f;	/* TNAFR */
		/* TNAF.7 0 = Si bit */
		/* TNAF.6 1 = FNA signal bit */
		/* TNAF.5 0 = remote alarm */
		/* TNAF.4 1 = additional bit 4 */
		/* TNAF.3 1 = additional bit 5 */
		/* TNAF.2 1 = additional bit 6 */
		/* TNAF.1 1 = additional bit 7 */
		/* TNAF.0 1 = additional bit 8 */

		/* set up transmit signalling */
		xlb[0x40] = 0x0b;	/* TS1: no alarm, spare bits one */

		for (offset = 0x41; offset <= 0x4f; offset++)
			/* TS2 thru TS16 */
			/* Set b and d bits transmit signalling each channel. (ITU-T specification
			   recommend that ABCD signalling not be set to all zero because they wille 
			   emulate a CAS multiframe alignment word. */
			xlb[offset] = 0x55;
		/* Note: If CCR3.6 == 1, then a zero in the TCBRs implies that signaling data is to 
		   be sourced from TSER (or TSIG if CCR3.2 == 1), and a one implies that signaling
		   data for that channel is to be sourced from the Transmit Signaling (TS)
		   registers.  In this mode, the voice-channel number scheme (CH1 to CH30) is used. 
		 */
		for (offset = 0x22; offset <= 0x25; offset++)	/* TCB1 thru TCB4 */
			xlb[offset] = 0xff;

#if 1
		if (timeouts) {
			unsigned long timeout;

			/* line interface reset */
			switch (cd->device) {
			case 0:	/* No IDR on DS2153. */
				/* DS2153 uses CCR3.3 for LIRST. */
				xlb[0x1b] = 0x8a;	/* CCR3 line interface reset (LIRST) */
				break;
			case XP_DEV_DS2154:
			case XP_DEV_DS21354:
			case XP_DEV_DS21554:
				xlb[0xaa] = 0x80;	/* CCR5 line interface reset (LIRST) */
				break;
			}
			timeout = (volatile unsigned long) jiffies + 100 * HZ / 1000;
			while ((volatile unsigned long) jiffies < timeout) ;
			switch (cd->device) {
			case 0:	/* No IDR on DS2153. */
				/* DS2153 uses CCR3.3 for LIRST. */
				xlb[0x1b] = 0x82;	/* CCR3 */
				break;
			case XP_DEV_DS2154:
			case XP_DEV_DS21354:
			case XP_DEV_DS21554:
				xlb[0xaa] = 0x00;	/* CCR5 */
				break;
			}

			/* reset and realign elastic stores */
			switch (cd->device) {
			case XP_DEV_DS2154:
				/* Note: no CCR6 on DS2154. */
				xlb[0x1b] = 0x92;	/* CCR3: reset elastic stores */
				xlb[0x1b] = 0x82;	/* CCR3 */
				break;
			case XP_DEV_DS21354:
			case XP_DEV_DS21554:
				xlb[0x1d] = ccr6 | 0x03;	/* CRC6: reset elastic stores */
				xlb[0x1d] = ccr6;	/* CRC6 */
				/* CCR6.7: 0 = TTIP and TRING normal */
				/* CCR6.6: 0 = normal data */
				/* CCR6.5: 0 = E1 signal */
				/* CCR6.4: 0 = unassigned */
				/* CCR6.3: 0 = unassigned */
				/* CCR6.2: X = Tx clock determined by CRC2.2 (LOTCMC)/Tx clock is
				   RCLK */
				/* CCR6.1: X = no RES reset */
				/* CCR6.0: X = no TES reset */
				xlb[0xaa] = 0x60;	/* CCR5 realign elastic stores */
				xlb[0xaa] = 0x00;	/* CCR5 */
				break;
			}
		}
#endif
		break;
	}
	case V401PE:
	{

		}
	}
	}
}
