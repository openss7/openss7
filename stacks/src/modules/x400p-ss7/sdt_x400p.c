/*****************************************************************************

 @(#) $RCSfile: sdt_x400p.c,v $ $Name:  $($Revision: 0.9.2.27 $) $Date: 2008-09-10 03:49:37 $

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

 Last Modified $Date: 2008-09-10 03:49:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdt_x400p.c,v $
 Revision 0.9.2.27  2008-09-10 03:49:37  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.26  2008-04-29 07:11:18  brian
 - updating headers for release

 Revision 0.9.2.25  2007/08/15 05:20:51  brian
 - GPLv3 updates

 Revision 0.9.2.24  2007/08/14 12:19:00  brian
 - GPLv3 header updates

 Revision 0.9.2.23  2007/07/14 01:35:22  brian
 - make license explicit, add documentation

 Revision 0.9.2.22  2007/03/25 19:00:39  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.21  2007/03/25 05:59:57  brian
 - flush corrections

 Revision 0.9.2.20  2007/03/25 02:23:10  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.19  2007/03/25 00:52:20  brian
 - synchronization updates

 Revision 0.9.2.18  2006/05/08 11:01:16  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.17  2006/03/07 01:14:48  brian
 - binary compatible callouts

 Revision 0.9.2.16  2006/03/04 13:00:23  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: sdt_x400p.c,v $ $Name:  $($Revision: 0.9.2.27 $) $Date: 2008-09-10 03:49:37 $"

static char const ident[] =
    "$RCSfile: sdt_x400p.c,v $ $Name:  $($Revision: 0.9.2.27 $) $Date: 2008-09-10 03:49:37 $";

/*
 *  This is an SDT (Signalling Data Terminal) kernel module which
 *  provides all of the capabilities of the SDTI for the E400P-SS7
 *  and T400P-SS7 cards.  The SL module can be pushed over streams opened on
 *  this driver to implement a complete SS7 MTP Level 2 OpenSS7
 *  implementation.
 */

#define X400P_DOWNLOAD_FIRMWARE

#include <sys/os7/compat.h>

#ifdef LINUX
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <linux/pci.h>

#include <linux/interrupt.h>
//#include "bufpool.h"
#endif				/* LINUX */

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

#ifdef X400P_DOWNLOAD_FIRMWARE
#include "x400p-ss7/x400pfw.h"	/* X400P-SS7 firmware load */
#endif

#define SDT_X400P_DESCRIP	"E/T400P-SS7: SS7/SDT (Signalling Data Terminal) STREAMS DRIVER."
#define SDT_X400P_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define SDT_X400P_REVISION	"OpenSS7 $RCSfile: sdt_x400p.c,v $ $Name:  $ ($Revision: 0.9.2.27 $) $Date: 2008-09-10 03:49:37 $"
#define SDT_X400P_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define SDT_X400P_DEVICE	"Supports the T/E400P-SS7 T1/E1 PCI boards."
#define SDT_X400P_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDT_X400P_LICENSE	"GPL"
#define SDT_X400P_BANNER	SDT_X400P_DESCRIP	"\n" \
				SDT_X400P_EXTRA		"\n" \
				SDT_X400P_REVISION	"\n" \
				SDT_X400P_COPYRIGHT	"\n" \
				SDT_X400P_DEVICE	"\n" \
				SDT_X400P_CONTACT
#define SDT_X400P_SPLASH	SDT_X400P_DESCRIP	"\n" \
				SDT_X400P_REVISION

#ifdef LINUX
MODULE_AUTHOR(SDT_X400P_CONTACT);
MODULE_DESCRIPTION(SDT_X400P_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDT_X400P_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDT_X400P_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdt_x400p");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SDT_X400P_DRV_ID	CONFIG_STREAMS_SDT_X400P_MODID
#define SDT_X400P_DRV_NAME	CONFIG_STREAMS_SDT_X400P_NAME
#define SDT_X400P_CMAJORS	CONFIG_STREAMS_SDT_X400P_NMAJORS
#define SDT_X400P_CMAJOR_0	CONFIG_STREAMS_SDT_X400P_MAJOR
#define SDT_X400P_UNITS		CONFIG_STREAMS_SDT_X400P_NMINORS
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define DRV_ID		SDT_X400P_DRV_ID
#define DRV_NAME	SDT_X400P_DRV_NAME
#define CMAJORS		SDT_X400P_CMAJORS
#define CMAJOR_0	SDT_X400P_CMAJOR_0
#define UNITS		SDT_X400P_UNITS
#ifdef MODULE
#define DRV_BANNER	SDT_X400P_BANNER
#else				/* MODULE */
#define DRV_BANNER	SDT_X400P_SPLASH
#endif				/* MODULE */

STATIC struct module_info xp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC struct module_stat xp_mstat = { 0, };

STATIC streamscall int xp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int xp_close(queue_t *, int, cred_t *);

STATIC streamscall int xp_rput(queue_t *, mblk_t *);
STATIC streamscall int xp_rsrv(queue_t *);

STATIC struct qinit xp_rinit = {
	.qi_putp = xp_rput,		/* Read put (message from below) */
	.qi_srvp = xp_rsrv,		/* Read queue service */
	.qi_qopen = xp_open,		/* Each open */
	.qi_qclose = xp_close,		/* Last close */
	.qi_minfo = &xp_minfo,		/* Information */
	.qi_mstat = &xp_mstat,		/* Statistics */
};

STATIC streamscall int xp_wput(queue_t *, mblk_t *);
STATIC streamscall int xp_wsrv(queue_t *);

STATIC struct qinit xp_winit = {
	.qi_putp = xp_wput,		/* Write put (message from above) */
	.qi_srvp = xp_wsrv,		/* Write queue service */
	.qi_minfo = &xp_minfo,		/* Information */
	.qi_mstat = &xp_mstat,		/* Statistics */
};

STATIC struct streamtab sdt_x400pinfo = {
	.st_rdinit = &xp_rinit,		/* Upper read queue */
	.st_wrinit = &xp_winit,		/* Upper write queue */
};

/*
 *  ========================================================================
 *
 *  Private structure
 *
 *  ========================================================================
 */
struct xp_span;

typedef struct xp_path {
	uint residue;			/* residue bits */
	uint rbits;			/* number of residue bits */
	ushort bcc;			/* crc for message */
	uint state;			/* state */
	uint mode;			/* path mode */
	uint type;			/* path frame type */
	uint bytes;			/* number of whole bytes */
	mblk_t *msg;			/* message */
	mblk_t *nxt;			/* message chain block */
	mblk_t *cmp;			/* repeat/compress buffer */
	uint repeat;			/* repeat/compress count */
	uint octets;			/* octets counted */
} xp_path_t;

typedef struct xp {
	struct xp *next;		/* list linkage */
	struct xp **prev;		/* list linkage */
	size_t refcnt;			/* reference count */
	spinlock_t lock;		/* structure lock */
	major_t cmajor;			/* major device number */
	minor_t cminor;			/* minor device number */
	queue_t *rq;			/* rd queue */
	queue_t *wq;			/* wr queue */
	spinlock_t qlock;		/* queue lock */
	queue_t *rwait;			/* rd queue waiting on qlock */
	queue_t *wwait;			/* wr queue waiting on qlock */
	uint rbid;			/* rd bufcall id */
	uint wbid;			/* wr bufcall id */
	uint state;			/* interface state */
	uint version;			/* interface version */
	struct xp_span *sp;		/* span for this channel */
	int chan;			/* index (chan) */
	int slot;			/* 32-bit backplane timeslot */
	xp_path_t tx;			/* transmit path variables */
	xp_path_t rx;			/* receive path variables */
	const char *ifname;		/* chan name */
	lmi_option_t option;		/* LMI protocol and variant options */
	struct {
		struct {
			ulong t8;	/* SDT T8 timer */
		} timers;		/* SDT timers */
		sdt_config_t config;	/* SDT configuration */
		sdt_statem_t statem;	/* SDT notification options */
		sdt_notify_t notify;	/* SDT statistics */
		sdt_stats_t stats;	/* SDT statistics */
		sdt_stats_t stamp;	/* SDT statistics timestamps */
		sdt_stats_t statsp;	/* SDT statistics periods */
	} sdt;
	struct {
		sdl_config_t config;	/* SDL configuration */
		sdl_statem_t statem;	/* SDL state machine variables */
		sdl_notify_t notify;	/* SDL notification options */
		sdl_stats_t stats;	/* SDL statistics */
		sdl_stats_t stamp;	/* SDL statistics timestamps */
		sdl_stats_t statsp;	/* SDL statistics periods */
	} sdl;
} xp_t;

#define PRIV(__q) ((xp_t *)(__q)->q_ptr)

/*
 *  ========================================================================
 *
 *  Locking
 *
 *  ========================================================================
 */
static inline int
xp_trylockq(queue_t *q)
{
	int res;
	xp_t *xp = PRIV(q);

	if (!(res = spin_trylock(&xp->qlock))) {
		if (q == xp->rq)
			xp->rwait = q;
		if (q == xp->wq)
			xp->wwait = q;
	}
	return (res);
}
static inline void
xp_unlockq(queue_t *q)
{
	xp_t *xp = PRIV(q);

	spin_unlock(&xp->qlock);
	if (xp->rwait)
		qenable(xchg(&xp->rwait, NULL));
	if (xp->wwait)
		qenable(xchg(&xp->wwait, NULL));
}

/*
 *  ------------------------------------------------------------------------
 *
 *  Card Structures and Macros
 *
 *  ------------------------------------------------------------------------
 */
#ifdef X400P_DOWNLOAD_FIRMWARE
#define GPIOC		(0x54 >> 1)	/* GPIO control register */
#define GPIO_WRITE	0x4000	/* GPIO4 data */
#define GPIO_PROGRAM	0x20000	/* GPIO5 data */
#define GPIO_INIT	0x100000	/* GPIO6 data */
#define GPIO_DONE	0x800000	/* GPIO7 data */
#endif

#define INTCSR (0x4c >> 1)
#define PLX_INTENA 0x43

#define SYNREG	0x400
#define CTLREG	0x401
#define LEDREG	0x402
#define STAREG	0x400

#define RIR2	0x31

#define LOOPUP	0x80
#define LOOPDN	0x40

#define INTENA	0x01
#define OUTBIT	0x02
#define E1DIV	0x10
#define INTACK	0x80

#define INTACTIVE 2

#define SYNCSELF 0
#define SYNC1 1
#define SYNC2 2
#define SYNC3 3
#define SYNC4 4

#define LEDBLK 0
#define LEDGRN 1
#define LEDRED 2
#define LEDYEL 3

#define X400_ABIT 8
#define X400_BBIT 4

#define SDT_X400P_ALARM_SETTLE_TIME	    5000	/* allow alarms to settle for 5 seconds */

/* *INDENT-OFF* */
/*
 *  Mapping of channels 0-23 for T1, 1-31 for E1 into PCM highway timeslots.
 */
STATIC int xp_t1_chan_map[] = {
	1,  2,  3, 5,  6,  7, 9, 10, 11, 13, 14, 15, 17, 18, 19, 21, 22, 23, 25, 26, 27, 29, 30, 31
};
STATIC int xp_e1_chan_map[] = {
	1,  2,  3, 4,  5,  6,  7, 8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};
#define X400P_T1_CHAN_DESCRIPTOR 0xEEEEEEEE
#define X400P_E1_CHAN_DESCRIPTOR 0xFFFFFFFE

#ifdef __LITTLE_ENDIAN
#define span_to_byte(__span) (3-(__span))
#else
#ifdef __BIG_ENDIAN
#define span_to_byte(__span) (__span)
#else
#error "Must know the endianess of processor\n"
#endif
#endif

typedef enum {
	PLX9030 = 0,
	PLXDEVBRD,
	X400P,
	X400PSS7,
} xp_board_t;

/* indexed by xp_board_t above */

static struct {
	char *name;
	u32 hw_flags;
} xp_board_info[] __devinitdata = {
	{ "PLX 9030", 0},
	{ "PLX Development Board", 0},
	{ "X400P-SS7", 1},
};

STATIC struct pci_device_id xp_pci_tbl[] __devinitdata = {
	{PCI_VENDOR_ID_PLX, 0x9030, PCI_ANY_ID, PCI_ANY_ID, 0, 0, PLX9030},
	{PCI_VENDOR_ID_PLX, 0x3001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, PLXDEVBRD},
	{PCI_VENDOR_ID_PLX, 0xD00D, PCI_ANY_ID, PCI_ANY_ID, 0, 0, X400P},
	{PCI_VENDOR_ID_PLX, 0xDEAD, PCI_ANY_ID, PCI_ANY_ID, 0, 0, X400PSS7},
	{0,}
};

STATIC int __devinit xp_probe(struct pci_dev *, const struct pci_device_id *);
STATIC void __devexit xp_remove(struct pci_dev *);
#ifdef CONFIG_PM
#ifndef HAVE_KTYPE_PM_MESSAGE_T
typedef u32 pm_message_t;
#endif
STATIC int xp_suspend(struct pci_dev *pdev, pm_message_t state);
STATIC int xp_resume(struct pci_dev *pdev);
#endif

STATIC struct pci_driver xp_driver = {
	name:		"x400p-ss7",
	probe:		xp_probe,
	remove:		__devexit_p(xp_remove),
	id_table:	xp_pci_tbl,
#ifdef CONFIG_PM
	suspend:	xp_suspend,
	resume:		xp_resume,
#endif
};
/* *INDENT-ON* */

struct xp_card;

typedef struct xp_span {
	struct xp_card *cp;		/* card for this span */
	size_t refcnt;			/* reference count */
	spinlock_t lock;		/* structure lock */
	xp_t *slots[32];		/* timeslot structures */
	ulong recovertime;		/* alarm recover time */
	ulong iobase;			/* span iobase */
	const char *ifname;		/* span name */
	int span;			/* index (span) */
	ulong ifflags;			/* span flags */
	volatile ulong ifalarms;	/* span alarms */
	volatile ulong ifrxlevel;	/* span rx level */
	volatile ulong ifbpv;		/* bipolar violations */
	volatile ulong loopcnt;		/* loop command count */
	ulong ifgtype;			/* span type */
	ulong ifgrate;			/* span rate */
	ulong ifgmode;			/* span mode */
	ulong ifgcrc;			/* span crc */
	ulong ifclock;			/* span clocking */
	ulong ifcoding;			/* span coding */
	ulong ifframing;		/* span framing */
	ulong iftxlevel;		/* span tx level */
	ulong iftype;			/* default chan type */
	ulong ifrate;			/* default chan rate */
	ulong ifmode;			/* default chan mode */
} xp_span_t;

typedef struct xp_card {
	struct xp_card *next;		/* list linkage */
	struct xp_card **prev;		/* list linkage */
	size_t refcnt;			/* reference count */
	spinlock_t lock;		/* structure lock */
	ulong xll_region;		/* Xilinx 32-bit memory region */
	ulong xll_length;		/* Xilinx 32-bit memory length */
	volatile uint32_t *xll;		/* Xilinx 32-bit memory map */
	ulong xlb_region;		/* Xilinx 8-bit memory region */
	ulong xlb_length;		/* Xilinx 8-bit memory lenght */
	volatile uint8_t *xlb;		/* Xilinx 8-bit memory map */
	ulong plx_region;		/* PLX 9030 memory region */
	ulong plx_length;		/* PLX 9030 memory length */
	volatile uint16_t *plx;		/* PLX 9030 memory map */
	uint frame;			/* frame number */
	xp_span_t *spans[4];		/* structures for spans */
	uint32_t *wbuf;			/* wr buffer */
	uint32_t *rbuf;			/* rd buffer */
	volatile int uebno;		/* upper elastic buffer number */
	volatile int lebno;		/* lower elastic buffer number */
	volatile int eval_syncsrc;	/* need to reevaluate sync src */
	volatile int leds;		/* leds on the card */
	int card;			/* index (card) */
	ulong irq;			/* card irq */
	ulong iobase;			/* card iobase */
	struct tasklet_struct tasklet;	/* card tasklet */
	const char *ifname;		/* card name */
	ulong ifflags;			/* card flags */
	volatile ulong ifsync;		/* card current sync source */
	ulong ifsyncsrc[SDL_SYNCS];	/* card synchronization sources */
	ulong ifgtype;			/* default span type */
	ulong ifgrate;			/* default span rate */
	ulong ifgmode;			/* default span mode */
	ulong ifgcrc;			/* default span crc */
	ulong ifclock;			/* default span clocking */
	ulong ifcoding;			/* default span coding */
	ulong ifframing;		/* default span framing */
	ulong iftxlevel;		/* default span tx level */
	ulong iftype;			/* default chan type */
	ulong ifrate;			/* default chan rate */
	ulong ifmode;			/* default chan mode */
} xp_card_t;

STATIC int x400p_boards = 0;
STATIC xp_card_t *x400p_cards;

#define X400P_EBUFNO (1<<7)	/* 128k elastic buffers */

/*
 *  ==========================================================================
 *
 *  Private and Card structure allocation and deallocation
 *
 *  ==========================================================================
 */

STATIC kmem_cachep_t xp_priv_cachep = NULL;
STATIC kmem_cachep_t xp_span_cachep = NULL;
STATIC kmem_cachep_t xp_card_cachep = NULL;
STATIC kmem_cachep_t xp_xbuf_cachep = NULL;

/*
 *  Cache allocation
 *  -------------------------------------------------------------------------
 */
STATIC int
xp_init_caches(void)
{
	if (!xp_priv_cachep
	    && !(xp_priv_cachep =
		 kmem_create_cache("xp_priv_cachep", sizeof(xp_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("X400P-SS7: initialized device private structure cache\n"));
	if (!xp_span_cachep
	    && !(xp_span_cachep =
		 kmem_create_cache("xp_span_cachep", sizeof(xp_span_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_span_cachep", __FUNCTION__);
		kmem_cache_destroy(xchg(&xp_priv_cachep, NULL));
		return (-ENOMEM);
	} else
		printd(("X400P-SS7: initialized span private structure cache\n"));
	if (!xp_card_cachep
	    && !(xp_card_cachep =
		 kmem_create_cache("xp_card_cachep", sizeof(xp_card_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_card_cachep", __FUNCTION__);
		kmem_cache_destroy(xchg(&xp_span_cachep, NULL));
		kmem_cache_destroy(xchg(&xp_priv_cachep, NULL));
		return (-ENOMEM);
	} else
		printd(("X400P-SS7: initialized card private structure cache\n"));
	if (!xp_xbuf_cachep
	    && !(xp_xbuf_cachep =
		 kmem_create_cache("xp_xbuf_cachep", X400P_EBUFNO * 1024, 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_xbuf_cachep", __FUNCTION__);
		kmem_cache_destroy(xchg(&xp_card_cachep, NULL));
		kmem_cache_destroy(xchg(&xp_span_cachep, NULL));
		kmem_cache_destroy(xchg(&xp_priv_cachep, NULL));
		return (-ENOMEM);
	} else
		printd(("X400P-SS7: initialized card read/write buffer cache\n"));
	return (0);
}
STATIC int
xp_term_caches(void)
{
	int err = 0;

	if (xp_xbuf_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(xp_xbuf_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_xbuf_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("X400P-SS7: shrunk xp_xbuf_cache to zero\n"));
#else
		kmem_cache_destroy(xp_xbuf_cachep);
#endif
	}
	if (xp_card_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(xp_card_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_card_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("X400P-SS7: shrunk xp_card_cache to zero\n"));
#else
		kmem_cache_destroy(xp_card_cachep);
#endif
	}
	if (xp_span_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(xp_span_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_span_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("X400P-SS7: shrunk xp_span_cache to zero\n"));
#else
		kmem_cache_destroy(xp_span_cachep);
#endif
	}
	if (xp_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(xp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("X400P-SS7: shrunk xp_priv_cache to zero\n"));
#else
		kmem_cache_destroy(xp_priv_cachep);
#endif
	}
	return (0);
}

/*
 *  Private structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC xp_t *
xp_alloc_priv(queue_t *q, xp_t ** xpp, major_t cmajor, minor_t cminor)
{
	xp_t *xp;

	if ((xp = kmem_cache_alloc(xp_priv_cachep, GFP_ATOMIC))) {
		printd(("X400P-SS7: allocated device private structure\n"));
		bzero(xp, sizeof(*xp));
		xp->cmajor = cmajor;
		xp->cminor = cminor;
		RD(q)->q_ptr = WR(q)->q_ptr = xp;
		xp->rq = RD(q);
		xp->wq = WR(q);
		spin_lock_init(&xp->qlock);	/* "xp-queue-lock" */
		xp->version = 1;
		xp->state = LMI_UNATTACHED;
		spin_lock_init(&xp->lock);	/* "xp-priv-lock" */
		if ((xp->next = *xpp))
			xp->next->prev = &xp->next;
		*xpp = xp;
		xp->prev = xpp;
		xp->refcnt++;
		printd(("X400P-SS7: linked device private structure\n"));
		/* LMI configuration defaults */
		xp->option.pvar = SS7_PVAR_ITUT_88;
		xp->option.popt = 0;
		/* SDL configuration defaults */
		xp->ifname = NULL;
		xp->sdl.config.ifflags = 0;
		xp->sdl.config.iftype = SDL_TYPE_DS0;
		xp->sdl.config.ifmode = SDL_MODE_PEER;
		xp->sdl.config.ifrate = 64000;
		/* SDT configuration defaults */
		xp->sdt.config.Tin = 4;
		xp->sdt.config.Tie = 1;
		xp->sdt.config.T = 64;
		xp->sdt.config.D = 256;
		xp->sdt.config.t8 = 100 * HZ / 1000;
		xp->sdt.config.Te = 577169;
		xp->sdt.config.De = 9308000;
		xp->sdt.config.Ue = 144292000;
		xp->sdt.config.N = 16;
		xp->sdt.config.m = 272;
		xp->sdt.config.b = 8;
		printd(("X400P-SS7: setting device private structure defaults\n"));
	} else
		ptrace(("X400P-SS7: ERROR: Could not allocate device private structure\n"));
	return (xp);
}
STATIC void
xp_free_priv(queue_t *q)
{
	xp_t *xp = PRIV(q);
	xp_span_t *sp;
	psw_t flags = 0;

	ensure(xp, return);
	if ((sp = xp->sp)) {
		int slot;

		spin_lock_irqsave(&xp->lock, flags);
		{
			for (slot = 0; slot < 32; slot++) {
				if (sp->slots[slot] == xp) {
					sp->slots[slot] = NULL;
					xp->refcnt--;
				}
			}
			xp->sp = NULL;
			sp->refcnt--;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		printd(("X400P-SS7: unlinked device private structure from span\n"));
	}
	if (xp->rbid)
		unbufcall(xp->rbid);
	if (xp->wbid)
		unbufcall(xp->wbid);
	if (xp->tx.msg)
		freemsg(xchg(&xp->tx.msg, NULL));
	if (xp->rx.msg)
		freemsg(xchg(&xp->rx.msg, NULL));
	if (xp->rx.nxt)
		freemsg(xchg(&xp->rx.nxt, NULL));
	if ((*xp->prev = xp->next))
		xp->next->prev = xp->prev;
	xp->next = NULL;
	xp->prev = NULL;
	xp->refcnt--;
	printd(("X400P-SS7: unlinked device private structure\n"));
	if (xp->refcnt) {
		assure(xp->refcnt == 0);
		printd(("X400P-SS7: WARNING: xp->refcnt = %d\n", xp->refcnt));
	}
	kmem_cache_free(xp_priv_cachep, xp);
	printd(("X400P-SS7: freed device private structure\n"));
	return;
}

/*
 *  Span allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC xp_span_t *
xp_alloc_span(xp_card_t * cp, uint8_t span)
{
	xp_span_t *sp;

	if ((sp = kmem_cache_alloc(xp_span_cachep, GFP_ATOMIC))) {
		printd(("X400P-SS7: allocated span private structure\n"));
		bzero(sp, sizeof(*sp));
		/* create linkage */
		sp->cp = cp;
		cp->refcnt++;
		cp->spans[span] = sp;
		sp->refcnt++;
		spin_lock_init(&sp->lock);	/* "sp-priv-lock" */
		/* fill out span structure */
		printd(("X400P-SS7: linked span private structure\n"));
		sp->iobase = cp->iobase + (span << 8);
		sp->ifname = cp->ifname;
		sp->span = span;
		sp->ifflags = 0;
		sp->ifgtype = cp->ifgtype;
		sp->ifgrate = cp->ifgrate;
		sp->ifgcrc = cp->ifgcrc;
		sp->ifclock = cp->ifclock;
		sp->ifcoding = cp->ifcoding;
		sp->ifframing = cp->ifframing;
		sp->ifrxlevel = 0;
		sp->iftxlevel = cp->iftxlevel;
		sp->ifalarms = 0;
		sp->iftype = cp->iftype;
		sp->ifrate = cp->ifrate;
		sp->ifmode = cp->ifmode;
		printd(("X400P-SS7: set span private structure defaults\n"));
	} else
		ptrace(("X400P-SS7: ERROR: Could not allocate span private structure\n"));
	return (sp);
}

/* Note: called with card interrupts disabled */
STATIC void
xp_free_span(xp_span_t * sp)
{
	xp_t *xp;
	xp_card_t *cp;

	ensure(sp, return);
	if ((cp = sp->cp)) {
		int slot;

		/* remove card linkage */
		cp->spans[sp->span] = NULL;
		sp->refcnt--;
		sp->cp = NULL;
		cp->refcnt--;
		sp->span = 0;
		printd(("X400P-SS7: unlinked span private structure from card\n"));
		/* remove channel linkage */
		for (slot = 0; slot < 32; slot++) {
			if ((xp = sp->slots[slot])) {
				xp->sp = NULL;
				sp->refcnt--;
				sp->slots[slot] = NULL;
				xp->refcnt--;
			}
		}
		printd(("X400P-SS7: unlinked span private structure from slots\n"));
	} else
		ptrace(("X400P-SS7: ERROR: spans cannot exist without cards\n"));
	if (sp->refcnt) {
		assure(sp->refcnt == 0);
		printd(("X400P-SS7: WARNING: sp->refcnt = %d\n", sp->refcnt));
	}
	kmem_cache_free(xp_span_cachep, sp);
	printd(("X400P-SS7: freed span private structure\n"));
	return;
}

/*
 *  Card allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC xp_card_t *
xp_alloc_card(void)
{
	xp_card_t *cp;

	if ((cp = kmem_cache_alloc(xp_card_cachep, GFP_ATOMIC))) {
		uint32_t *wbuf;
		uint32_t *rbuf;

		printd(("X400P-SS7: allocated card private structure\n"));
		if (!(wbuf = kmem_cache_alloc(xp_xbuf_cachep, GFP_ATOMIC))) {
			ptrace(("X400P-SS7: could not allocate write buffer\n"));
			kmem_cache_free(xp_card_cachep, cp);
			return (NULL);
		}
		if (!(rbuf = kmem_cache_alloc(xp_xbuf_cachep, GFP_ATOMIC))) {
			ptrace(("X400P-SS7: could not allocate read buffer\n"));
			kmem_cache_free(xp_xbuf_cachep, wbuf);
			kmem_cache_free(xp_card_cachep, cp);
			return (NULL);
		}
		bzero(cp, sizeof(*cp));
		if ((cp->next = x400p_cards))
			cp->next->prev = &cp->next;
		x400p_cards = cp;
		cp->prev = &x400p_cards;
		cp->card = x400p_boards++;
		cp->refcnt++;
		cp->wbuf = wbuf;
		cp->rbuf = rbuf;
		printd(("X400P-SS7: linked card private structure\n"));
	} else
		ptrace(("X400P-SS7: ERROR: Could not allocate card private structure\n"));
	return (cp);
}

/* Note: called with card interrupts disabled and pci resources deallocated */
STATIC void
xp_free_card(xp_card_t * cp)
{
	int span;

	ensure(cp, return);
	/* remove any remaining spans */
	for (span = 0; span < 4; span++) {
		xp_span_t *sp;

		if ((sp = cp->spans[span]))
			xp_free_span(sp);
	}
	/* unlink from board list */
	if ((*(cp->prev) = cp->next))
		cp->next->prev = cp->prev;
	cp->next = NULL;
	cp->prev = NULL;
	cp->refcnt--;
	printd(("X400P-SS7: unlinked card private structure\n"));
	if (cp->refcnt) {
		assure(cp->refcnt == 0);
		printd(("X400P-SS7: WARNING: cp->refcnt = %d\n", cp->refcnt));
	}
	tasklet_kill(&cp->tasklet);
	kmem_cache_free(xp_xbuf_cachep, (uint32_t *) xchg(&cp->rbuf, NULL));
	kmem_cache_free(xp_xbuf_cachep, (uint32_t *) xchg(&cp->wbuf, NULL));
	kmem_cache_free(xp_card_cachep, cp);
	printd(("X400P-SS7: freed card private structure\n"));
	return;
}

/*
 *  =========================================================================
 *
 *  BUFFER Allocation
 *
 *  =========================================================================
 */
/*
 *  BUFSRV
 *  -------------------------------------------------------------------------
 */
STATIC streamscall void
xp_bufsrv(long data)
{
	queue_t *q = (queue_t *) data;

	if (q) {
		xp_t *xp = PRIV(q);

		if (q == xp->rq)
			if (xp->rbid)
				xp->rbid = 0;
		if (q == xp->wq)
			if (xp->wbid)
				xp->wbid = 0;
		qenable(q);
	}
}

/*
 *  ALLOCB
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *
xp_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;

	if ((mp = allocb(size, prior)))
		return (mp);
	else {
		xp_t *xp = PRIV(q);

		if (q == xp->rq) {
			if (!xp->rbid)
				xp->rbid = bufcall(size, prior, &xp_bufsrv, (long) q);
			return (NULL);
		}
		if (q == xp->wq) {
			if (!xp->wbid)
				xp->wbid = bufcall(size, prior, &xp_bufsrv, (long) q);
			return (NULL);
		}
	}
	swerr();
	return (NULL);
}

#if 0
/*
 *  ESBALLOC
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *
xp_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;

	if ((mp = esballoc(base, size, prior, frtn)))
		return (mp);
	else {
		xp_t *xp = PRIV(q);

		if (q == xp->rq) {
			if (!xp->rbid)
				xp->rbid = esbbcall(prior, &xp_bufsrv, (long) q);
			return (NULL);
		}
		if (q == xp->wq) {
			if (!xp->wbid)
				xp->wbid = esbbcall(prior, &xp_bufsrv, (long) q);
			return (NULL);
		}
		swerr();
		return (NULL);
	}
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  FAST buffer allocation/deallocation for ISRs.
 *
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *xp_bufpool = NULL;
STATIC spinlock_t xp_bufpool_lock = SPIN_LOCK_UNLOCKED;

/*
 *  FAST ALLOCB
 *  -------------------------------------------------------------------------
 *  This is a fast allocation mechanism for FASTBUF sized buffers for the
 *  receive ISRs.  This increases FISU/LSSU compression performance.  If there
 *  are blocks in the pool we use one of those, otherwise we allocate as
 *  normal.
 */
STATIC INLINE mblk_t *
xp_fast_allocb(void)
{
	mblk_t *mp;

	spin_lock(&xp_bufpool_lock);
	if ((mp = xp_bufpool)) {
		xp_bufpool = mp->b_cont;
		mp->b_cont = NULL;
		mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
		spin_unlock(&xp_bufpool_lock);
	} else {
		mp = allocb(FASTBUF, BPRI_HI);
		spin_unlock(&xp_bufpool_lock);
	}
	return (mp);
}

/*
 *  FAST FREEMSG
 *  -------------------------------------------------------------------------
 *  This is a fast deallocation mechansim for FASTBUF sized buffers for the
 *  receive ISRs.  This increases FISU/LSSU compression performance.  If there
 *  is only one reference to the message block we just place the block in the
 *  pool.  If there is more than one reference, we free as normal.
 */
STATIC INLINE void
xp_fast_freemsg(mblk_t *mp)
{
	mblk_t *bp, *bp_next = mp;

	while ((bp = bp_next)) {
		bp_next = bp->b_cont;
		if (bp->b_datap->db_ref == 1) {
			spin_lock(&xp_bufpool_lock);
			bp->b_cont = xchg(&xp_bufpool, bp);
			spin_unlock(&xp_bufpool_lock);
		} else if (bp->b_datap->db_ref > 1)
			/* other references, use normal free mechanism */
			freeb(bp);
	}
}

/*
 *  FAST DUPB
 *  -------------------------------------------------------------------------
 *  This is a fast block duplication mechanism for FASTBUF sized M_DATA
 *  buffers for the receive ISRs.  This increases FISU/LSSU compression
 *  performance.  A block copy is performed because these blocks are small.
 */
STATIC INLINE mblk_t *
xp_fast_dupb(mblk_t *mp)
{
	int len;
	mblk_t *dp = NULL;

	if (mp && (len = mp->b_wptr - mp->b_rptr) > 0 && (dp = xp_fast_allocb())) {
		bcopy(mp->b_rptr, dp->b_wptr, len);
		dp->b_wptr += len;
	}
	return (dp);
}

/*
 *  FAST INIT and TERM
 *  -------------------------------------------------------------------------
 */
STATIC int
xp_bufpool_init(void)
{
	unsigned long flags;

	spin_lock_init(&xp_bufpool_lock);
	spin_lock_irqsave(&xp_bufpool_lock, flags);
	{
		int n = 256;		/* two for each channel */

		while (n-- > 0)
			xp_fast_freemsg(allocb(FASTBUF, BPRI_LO));
	}
	spin_unlock_irqrestore(&xp_bufpool_lock, flags);
	return (0);
}
STATIC int
xp_bufpool_term(void)
{
	unsigned long flags;

	spin_lock_irqsave(&xp_bufpool_lock, flags);
	{
		mblk_t *bp, *bp_next = xp_bufpool;

		while ((bp = bp_next)) {
			bp_next = bp->b_cont;
			freeb(bp);
		}
		xp_bufpool = NULL;
	}
	spin_unlock_irqrestore(&xp_bufpool_lock, flags);
	return (0);
}

/*
 *  ========================================================================
 *
 *  PRIMITIVES Sent Upstream
 *
 *  ========================================================================
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, int err)
{
	mblk_t *mp;

	if ((mp = xp_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_SIGNAL_UNIT_IND
 *  -----------------------------------
 *  We prefer to send M_DATA blocks.  When the count is 1, we simply send
 *  M_DATA.  When the count is greater than one, we send an
 *  SDT_RC_SIGNAL_UNIT_IND which also includes the count.  This is so that
 *  upper layer modules can collect SU statistics.
 *
 *  Can't user buffer service.
 */
STATIC INLINE int
sdt_rc_signal_unit_ind(queue_t *q, mblk_t *dp, ulong count)
{
	if (count) {
		if (canputnext(q)) {
			if (count > 1) {
				mblk_t *mp;
				sdt_rc_signal_unit_ind_t *p;

				if ((mp = allocb(sizeof(*p), BPRI_MED))) {
					mp->b_datap->db_type = M_PROTO;
					p = (typeof(p)) mp->b_wptr;
					mp->b_wptr += sizeof(*p);
					p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
					p->sdt_count = count;
					mp->b_cont = dp;
					putnext(q, mp);
					return (QR_ABSORBED);
				}
				rare();
				return (-ENOBUFS);
			}
			putnext(q, dp);
			return (QR_ABSORBED);
		}
		rare();
		return (-EBUSY);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SDT_RC_CONGESTION_ACCEPT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_accept_ind(queue_t *q)
{
	mblk_t *mp;
	sdt_rc_congestion_accept_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_CONGESTION_DISCARD_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_discard_ind(queue_t *q)
{
	mblk_t *mp;
	sdt_rc_congestion_discard_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_NO_CONGESTION_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_no_congestion_ind(queue_t *q)
{
	mblk_t *mp;
	sdt_rc_no_congestion_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_IAC_CORRECT_SU_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_iac_correct_su_ind(queue_t *q)
{
	if (canputnext(q)) {
		mblk_t *mp;
		sdt_iac_correct_su_ind_t *p;

		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
			putnext(q, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  SDT_IAC_ABORT_PROVING_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_iac_abort_proving_ind(queue_t *q)
{
	mblk_t *mp;
	sdt_iac_abort_proving_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_LSC_LINK_FAILURE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_lsc_link_failure_ind(queue_t *q)
{
	mblk_t *mp;
	sdt_lsc_link_failure_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_txc_transmission_request_ind(queue_t *q)
{
	mblk_t *mp;
	sdt_txc_transmission_request_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, caddr_t ppa_ptr, size_t ppa_len)
{
	xp_t *xp = PRIV(q);
	mblk_t *mp;
	lmi_info_ack_t *p;

	if ((mp = xp_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = xp->state;
		p->lmi_max_sdu = xp->sdt.config.m + 1 + ((xp->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((xp->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_ok_ack(queue_t *q, ulong state, long prim)
{
	xp_t *xp = PRIV(q);
	mblk_t *mp;
	lmi_ok_ack_t *p;

	if ((mp = xp_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		p->lmi_state = xp->state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ack(queue_t *q, ulong state, long prim, ulong errno, ulong reason)
{
	xp_t *xp = PRIV(q);
	mblk_t *mp;
	lmi_error_ack_t *p;

	if ((mp = xp_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_error_primitive = prim;
		p->lmi_state = xp->state = state;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_enable_con(queue_t *q)
{
	mblk_t *mp;
	lmi_enable_con_t *p;

	if ((mp = xp_allocb(q, sizeof(*p), BPRI_MED))) {
		xp_t *xp = PRIV(q);

		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = xp->state = LMI_ENABLED;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_disable_con(queue_t *q)
{
	mblk_t *mp;
	lmi_disable_con_t *p;

	if ((mp = xp_allocb(q, sizeof(*p), BPRI_MED))) {
		xp_t *xp = PRIV(q);

		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = xp->state = LMI_DISABLED;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;

	if ((mp = xp_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		qreply(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ind(queue_t *q, ulong errno, ulong reason)
{
	xp_t *xp = PRIV(q);
	mblk_t *mp;
	lmi_error_ind_t *p;

	if ((mp = xp_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = xp->state;
		putnext(q, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_stats_ind(queue_t *q, ulong interval)
{
	if (canputnext(q)) {
		mblk_t *mp;
		lmi_stats_ind_t *p;

		if ((mp = xp_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = jiffies;
			putnext(q, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, ulong oid, ulong level)
{
	if (canputnext(q)) {
		mblk_t *mp;
		lmi_event_ind_t *p;

		if ((mp = xp_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = jiffies;
			p->lmi_severity = level;
			putnext(q, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE FUNCTIONS
 *
 *  =========================================================================
 */

STATIC INLINE int
sdt_aerm_su_in_error(queue_t *q)
{
	xp_t *xp = PRIV(q);
	int err;

	if (xp->sdt.statem.aerm_state == SDT_STATE_MONITORING) {
		xp->sdt.statem.Ca++;
		if (xp->sdt.statem.Ca == xp->sdt.statem.Ti) {
			xp->sdt.statem.aborted_proving = 1;
			if ((err = sdt_iac_abort_proving_ind(q))) {
				xp->sdt.statem.Ca--;
				return (err);
			}
			xp->sdt.statem.aerm_state = SDT_STATE_IDLE;
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sdt_aerm_correct_su(queue_t *q)
{
	xp_t *xp = PRIV(q);
	int err;

	if (xp->sdt.statem.aerm_state == SDT_STATE_IDLE) {
		if (xp->sdt.statem.aborted_proving) {
			if ((err = sdt_iac_correct_su_ind(q)))
				return (err);
			xp->sdt.statem.aborted_proving = 0;
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sdt_suerm_start(queue_t *q)
{
	xp_t *xp = PRIV(q);

	xp->sdt.statem.Cs = 0;
	xp->sdt.statem.Ns = 0;
	xp->sdt.statem.suerm_state = SDT_STATE_IN_SERVICE;
	return (QR_DONE);
}

STATIC INLINE void
sdt_suerm_stop(queue_t *q)
{
	xp_t *xp = PRIV(q);

	xp->sdt.statem.suerm_state = SDT_STATE_IDLE;
}

STATIC INLINE int
sdt_suerm_su_in_error(queue_t *q)
{
	xp_t *xp = PRIV(q);
	int err;

	if (xp->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		xp->sdt.statem.Cs++;
		if (xp->sdt.statem.Cs >= xp->sdt.config.T) {
			if ((err = sdt_lsc_link_failure_ind(q))) {
				xp->sdt.statem.Ca--;
				return (err);
			}
			xp->sdt.statem.suerm_state = SDT_STATE_IDLE;
			return (QR_DONE);
		}
		xp->sdt.statem.Ns++;
		if (xp->sdt.statem.Ns >= xp->sdt.config.D) {
			xp->sdt.statem.Ns = 0;
			if (xp->sdt.statem.Cs)
				xp->sdt.statem.Cs--;
		}
	}
	return (QR_DONE);
}

STATIC INLINE void
sdt_eim_su_in_error(queue_t *q)
{
	xp_t *xp = PRIV(q);

	if (xp->sdt.statem.eim_state == SDT_STATE_MONITORING)
		xp->sdt.statem.interval_error = 1;
}

STATIC INLINE void
sdt_suerm_correct_su(queue_t *q)
{
	xp_t *xp = PRIV(q);

	if (xp->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		xp->sdt.statem.Ns++;
		if (xp->sdt.statem.Ns >= xp->sdt.config.D) {
			xp->sdt.statem.Ns = 0;
			if (xp->sdt.statem.Cs)
				xp->sdt.statem.Cs--;
		}
	}
}

STATIC INLINE void
sdt_eim_correct_su(queue_t *q)
{
	xp_t *xp = PRIV(q);

	if (xp->sdt.statem.eim_state == SDT_STATE_MONITORING)
		xp->sdt.statem.su_received = 1;
}

STATIC void sdt_t8_timeout(queue_t *q);

STATIC INLINE void
sdt_timer_stop_t8(queue_t *q)
{
	xp_t *xp = PRIV(q);

	if (xp->sdt.timers.t8)
		untimeout(xchg(&xp->sdt.timers.t8, 0));
}

STATIC INLINE void
sdt_timer_start_t8(queue_t *q)
{
	xp_t *xp = PRIV(q);

	sdt_timer_stop_t8(q);
	xp->sdt.timers.t8 = timeout((timo_fcn_t *) sdt_t8_timeout, (caddr_t) xp, xp->sdt.config.t8);
}

#define SDT_T8_TIMEOUT 1
STATIC int
sdt_t8_timeout_ind(queue_t *q)
{
	xp_t *xp = PRIV(q);
	int err;

	if (xp->sdt.statem.eim_state == SDT_STATE_MONITORING) {
		sdt_timer_start_t8(q);
		if (xp->sdt.statem.su_received) {
			xp->sdt.statem.su_received = 0;
			if (!xp->sdt.statem.interval_error) {
				if ((xp->sdt.statem.Ce -= xp->sdt.config.De) < 0)
					xp->sdt.statem.Ce = 0;
				return (QR_DONE);
			}
		}
		xp->sdt.statem.Ce += xp->sdt.config.Ue;
		if (xp->sdt.statem.Ce > xp->sdt.config.Te) {
			if ((err = sdt_lsc_link_failure_ind(q))) {
				xp->sdt.statem.Ce -= xp->sdt.config.Ue;
				return (err);
			}
			xp->sdt.statem.eim_state = SDT_STATE_IDLE;
		}
		xp->sdt.statem.interval_error = 0;
	}
	return (QR_DONE);
}

STATIC void
sdt_t8_timeout(queue_t *q)
{
	xp_t *xp = PRIV(q);
	mblk_t *mp;

	if (!spin_is_locked(&xp->qlock)) {
		if ((mp = allocb(sizeof(ulong), BPRI_HI))) {
			mp->b_datap->db_type = M_PCRSE;
			*(ulong *) mp->b_wptr = SDT_T8_TIMEOUT;
			mp->b_wptr += sizeof(ulong);
			putq(xp->rq, mp);
			return;
		}
	}
	/* restart the timer */
	xp->sdt.timers.t8 = timeout((timo_fcn_t *) sdt_t8_timeout, (caddr_t) q, 10);
}

STATIC INLINE int
sdt_eim_start(queue_t *q)
{
	xp_t *xp = PRIV(q);

	xp->sdt.statem.Ce = 0;
	xp->sdt.statem.interval_error = 0;
	xp->sdt.statem.su_received = 0;
	sdt_timer_start_t8(q);
	xp->sdt.statem.eim_state = SDT_STATE_MONITORING;
	return (QR_DONE);
}

STATIC INLINE void
sdt_eim_stop(queue_t *q)
{
	xp_t *xp = PRIV(q);

	xp->sdt.statem.eim_state = SDT_STATE_IDLE;
	sdt_timer_stop_t8(q);
}

STATIC INLINE int
sdt_daedr_correct_su(queue_t *q, int count)
{
	int err;
	int num;

	for (num = 0; num < count; num++) {
		sdt_eim_correct_su(q);
		sdt_suerm_correct_su(q);
		if ((err = sdt_aerm_correct_su(q)))
			return (err);
	}
	return (QR_DONE);
}

STATIC INLINE int
sdt_daedr_su_in_error(queue_t *q)
{
	int err;

	sdt_eim_su_in_error(q);
	if ((err = sdt_suerm_su_in_error(q)))
		return (err);
	if ((err = sdt_aerm_su_in_error(q)))
		return (err);
	return (QR_DONE);
}

STATIC INLINE int
sdt_daedr_received_bits(queue_t *q, mblk_t *mp, int count)
{
	int err;

	if ((err = sdt_rc_signal_unit_ind(q, mp, count)) != QR_ABSORBED)
		return (err);
	if ((err = sdt_daedr_correct_su(q, count)))
		return (err);
	return (QR_ABSORBED);
}

STATIC INLINE int
sdt_daedt_transmission_request(queue_t *q)
{
	return sdt_txc_transmission_request_ind(q);
}

/*
 *  ========================================================================
 *
 *  Soft-HDLC
 *
 *  ========================================================================
 */
#define SDT_TX_STATES	5
#define SDT_RX_STATES	14

#define SDT_TX_BUFSIZE	PAGE_SIZE
#define SDT_RX_BUFSIZE	PAGE_SIZE

#define SDT_CRC_TABLE_LENGTH	512
#define SDT_TX_TABLE_LENGTH	(2* SDT_TX_STATES * 256)
#define SDT_RX_TABLE_LENGTH	(2* SDT_RX_STATES * 256)

typedef struct tx_entry {
	uint bit_string:10 __attribute__ ((packed));	/* the output string */
	uint bit_length:4 __attribute__ ((packed));	/* length in excess of 8 bits of output
							   string */
	uint state:3 __attribute__ ((packed));	/* new state */
} tx_entry_t;

typedef struct rx_entry {
	uint bit_string:16 __attribute__ ((packed));
	uint bit_length:4 __attribute__ ((packed));
	uint state:4 __attribute__ ((packed));
	uint sync:1 __attribute__ ((packed));
	uint hunt:1 __attribute__ ((packed));
	uint flag:1 __attribute__ ((packed));
	uint idle:1 __attribute__ ((packed));
} rx_entry_t;

typedef uint16_t bc_entry_t;

STATIC bc_entry_t *bc_table = NULL;
STATIC tx_entry_t *tx_table = NULL;
STATIC rx_entry_t *rx_table = NULL;
STATIC rx_entry_t *rx_table7 = NULL;

STATIC size_t bc_order = 0;
STATIC size_t tx_order = 0;
STATIC size_t rx_order = 0;

STATIC INLINE tx_entry_t *
tx_index(uint j, uint k)
{
	return &tx_table[(j << 8) | k];
}
STATIC INLINE rx_entry_t *
rx_index7(uint j, uint k)
{
	return &rx_table7[(j << 8) | k];
}
STATIC INLINE rx_entry_t *
rx_index8(uint j, uint k)
{
	return &rx_table[(j << 8) | k];
}

#ifdef _DEBUG
STATIC INLINE void
printb(uint8_t byte)
{
	uint8_t mask = 0x80;

	while (mask) {
		if (mask & byte)
			printd(("1"));
		else
			printd(("0"));
	}
}
STATIC INLINE void
printbs(uint str, uint len)
{
	uint mask = (1 << len);

	while (mask >>= 1) {
		if (mask & str)
			printd(("1"));
		else
			printd(("0"));
	}
}
STATIC INLINE void
printr(rx_entry_t * r)
{
	printd(("rx(%2d) %d%d%d%d ", r->state, r->flag, r->sync, r->hunt, r->idle));
	printbs(r->bit_string, r->bit_length);
	printd(("\n"));
}
STATIC INLINE void
printt(tx_entry_t * t)
{
	printd(("tx(%2d) ", t->state));
	printbs(t->bit_string, t->bit_length + 8);
	printd(("\n"));
}
#else
STATIC INLINE void
printb(uint8_t byte)
{
}
STATIC INLINE void
printr(rx_entry_t * r)
{
}
STATIC INLINE void
printt(tx_entry_t * t)
{
}
#endif

/*
 *  TX REPEAT
 *  ----------------------------------------
 *  Set up the FISU/LSSU repeat buffer for a new message for transmission.
 *  The repeat buffer contains a FISU for transmitted MSUs.  It contains the
 *  FISU, LSSU or 2-byte LSSU for transmitted FISUs and LSSUs.  For SIBs it
 *  contains a FISU, for other LSSUs it contains the LSSU.  This buffer
 *  initially contains an SIOS.
 */
STATIC void
sdt_tx_repeat(xp_path_t * tx, mblk_t *mp, int xsn)
{
	mblk_t *dp;
	size_t len;

	len = msgdsize(mp);	/* how big is it? */
	if (!(dp = tx->cmp) && !(dp = tx->cmp = xp_fast_allocb()))
		return;
	dp->b_rptr = dp->b_wptr = dp->b_datap->db_base;
	/* set up new FISU/LSSU repeat buffer */
	if (!xsn) {
		if (len > 5 || ((len == 4 || len == 5) && (mp->b_rptr[3] == 5))) {
			/* this is a SIB or an MSU (not necessarily a valid one) */
			/* use only the fisu bits */
			dp->b_rptr[0] = mp->b_rptr[0];
			dp->b_rptr[1] = mp->b_rptr[1];
			dp->b_rptr[2] = 0;
			dp->b_wptr = dp->b_rptr + 3;
		} else if (len < 3) {
			ptrace(("sdt: ERROR: user violated minimum size len = %d\n", len));
			return;
		} else {	/* this is a FISU, LSSU or LSS2, use all the bits */
			bcopy(mp->b_rptr, dp->b_wptr, len);
			dp->b_wptr = dp->b_rptr + len;
		}
	} else {
		/* FISU length == 6, LSSU length = 7, LSS2 length = 8 */
		if (len > 8 || ((len == 7 || len == 8) && (mp->b_rptr[6] == 5))) {
			/* this is a SIB or an MSU (not necessarily a valid one) */
			/* use only the fisu bits */
			dp->b_rptr[0] = mp->b_rptr[0];
			dp->b_rptr[1] = mp->b_rptr[1];
			dp->b_rptr[2] = mp->b_rptr[2];
			dp->b_rptr[3] = mp->b_rptr[3];
			dp->b_rptr[4] = 0;
			dp->b_rptr[5] = 0;
			dp->b_wptr = dp->b_rptr + 6;
		} else if (len < 6) {
			ptrace(("sdt: ERROR: user violated minimum size len = %d\n", len));
			return;
		} else {	/* this is a FISU, LSSU or LSS2, use all the bits */
			bcopy(mp->b_rptr, dp->b_wptr, len);
			dp->b_wptr = dp->b_rptr + len;
		}
	}
}

/*
 *  TX GETQ
 *  -----------------------------------
 *  Get a new frame for transmission off of the write queue for the channel or
 *  span.  This permits us to send back-to-back frames at 1 Erlang (one flag
 *  between frames) when we have sufficient traffic.  Unfortunately we cannot
 *  take data from the queue if we are processing the queue (we may have
 *  removed the head M_DATA block), but in that case we will simply insert
 *  FISU/LSSU repetition.
 */
STATIC mblk_t *
xp_tx_getq(queue_t *q, xp_path_t * tx, int xsn)
{
	mblk_t *mp = NULL;

	if (!spin_is_locked(&PRIV(q)->qlock)) {
		for (mp = q->q_first; mp && mp->b_datap->db_type != M_DATA; mp = mp->b_next) ;
		if (mp) {
			// printd(("%s: taking message off of queue\n", __FUNCTION__));
			rmvq(q, mp);	/* remove from queue */
			sdt_tx_repeat(tx, mp, xsn);	/* setup new repeate frame */
			qenable(q);	/* enable other priority message */
		}
	}
	return (mp);
}

/*
 *  TX BITSTUFF
 *  -----------------------------------
 *  Bitstuff an octet and shift residue for output.
 */
STATIC INLINE void
xp_tx_bitstuff(xp_path_t * tx, unsigned char byte)
{
	tx_entry_t *t = tx_index(tx->state, byte);

	tx->state = t->state;
	tx->residue |= t->bit_string << tx->rbits;
	tx->rbits += t->bit_length + 8;
}

#define TX_MODE_IDLE	0	/* generating mark idle */
#define TX_MODE_FLAG	1	/* generating flag idle */
#define TX_MODE_BOF	2	/* generating bof flag */
#define TX_MODE_MOF	3	/* generating frames */
#define TX_MODE_BCC	4	/* generating bcc bytes */
/*
 *  TX BLOCK
 *  ----------------------------------------
 *  Generate a block for transmission for a channel or span.  We generate an
 *  entire transmit block.  If there are not sufficient messages to build the
 *  transmit block we will repeat FISU/LSSU or idle flags.
 */

STATIC INLINE void
xp_tx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats, const ulong type)
{
	queue_t *q = xp->wq;
	register xp_path_t *tx = &xp->tx;
	int xsn = xp->option.popt & SS7_POPT_XSN;
	int chan = 0, bits = (type == SDL_TYPE_DS0A) ? 7 : 8;

	if (tx->mode == TX_MODE_IDLE || tx->mode == TX_MODE_FLAG) {
		if (!tx->nxt) {
		      next_message:
			/* free previous message, if any */
			if (tx->msg) {
				if (tx->msg == tx->cmp) {
					/* restart repeat buffer */
					tx->cmp->b_rptr = tx->cmp->b_datap->db_base;
					tx->msg = NULL;
				} else
					freemsg(xchg(&tx->msg, NULL));
			}
			/* get next message */
			if (q->q_first && (tx->msg = tx->nxt = xp_tx_getq(q, tx, xsn)))
				/* got new message */
				tx->mode = TX_MODE_BOF;
			else if ((tx->msg = tx->nxt = tx->cmp)) {
				/* got repeat */
				stats->tx_sus_repeated++;
				tx->mode = TX_MODE_BOF;
			}
		}
	}
	/* check if transmission block complete */
	for (; bp < be; bp += 128) {
	      check_rbits:
		/* drain residue bits, if necessary */
		if (tx->rbits >= bits) {
		      drain_rbits:
			/* drain residue bits */
			switch (type) {
			default:
			case SDL_TYPE_DS0:
				*bp = tx->residue;
				break;
			case SDL_TYPE_DS0A:
				*bp = tx->residue & 0x7f;
				break;
			case SDL_TYPE_T1:
				*(bp + (xp_t1_chan_map[chan] << 2)) = tx->residue;
				if (++chan > 23)
					chan = 0;
				break;
			case SDL_TYPE_E1:
				*(bp + (xp_e1_chan_map[chan] << 2)) = tx->residue;
				if (++chan > 30)
					chan = 0;
				break;
			}
			tx->residue >>= bits;
			tx->rbits -= bits;
			if (chan)
				goto check_rbits;
			continue;
		}
		switch (tx->mode) {
		case TX_MODE_IDLE:
			/* mark idle */
			tx->residue |= 0xff << tx->rbits;
			tx->rbits += 8;
			goto drain_rbits;
		case TX_MODE_FLAG:
			/* idle flags */
			tx->residue |= 0x7e << tx->rbits;
			tx->rbits += 8;
			goto drain_rbits;
		case TX_MODE_BOF:
			/* add opening flag (also closing flag) */
			tx->residue |= 0x7e << tx->rbits;
			tx->rbits += 8;
			tx->state = 0;
			tx->bcc = 0x00ff;
			tx->mode = TX_MODE_MOF;
			goto drain_rbits;
		case TX_MODE_MOF:	/* transmit frame bytes */
			if (tx->nxt->b_rptr < tx->nxt->b_wptr || (tx->nxt = tx->nxt->b_cont)) {
				/* continuing in message */
				uint byte = *tx->nxt->b_rptr++;

				tx->bcc = (tx->bcc >> 8) ^ bc_table[(tx->bcc ^ byte) & 0x00ff];
				xp_tx_bitstuff(tx, byte);
				stats->tx_bytes++;
			} else {
				/* finished message: add 1st bcc byte */
				xp_tx_bitstuff(tx, tx->bcc & 0x00ff);
				tx->mode = TX_MODE_BCC;
			}
			goto drain_rbits;
		case TX_MODE_BCC:
			/* add 2nd bcc byte */
			xp_tx_bitstuff(tx, tx->bcc >> 8);
			stats->tx_sus++;
			tx->mode = TX_MODE_FLAG;
			goto next_message;
		}
		swerr();
	}
}

/* force 4 separate versions for speed */
STATIC void
xp_ds0a_tx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_DS0A);
}
STATIC void
xp_ds0_tx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_DS0);
}
STATIC void
xp_t1_tx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_T1);
}
STATIC void
xp_e1_tx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_E1);
}

/*
 *  RX COMPRESS
 *  ----------------------------------------
 *  Perform RX compression.  Deliver and copy frames as necessary.
 */
STATIC INLINE void
xp_rx_compress(queue_t *q, xp_path_t * rx, int hlen, sdt_stats_t * stats)
{
	mblk_t *mp;

	if ((mp = rx->msg)) {
		mblk_t *cp;
		int len = msgdsize(mp);
		int max = hlen + 2;	/* size of 2-byte lssu */

		if ((cp = rx->cmp)) {
			if (len <= max) {
				if (len == msgdsize(cp)) {
					int pos;

					for (pos = 0; pos < len; pos++)
						if (cp->b_rptr[pos] != mp->b_rptr[pos])
							goto nomatch;
					rx->repeat++;
					stats->rx_sus_compressed++;
					xp_fast_freemsg(mp);
					rx->msg = NULL;
					return;
				}
			}
		      nomatch:
			if (!rx->repeat
			    || (sdt_daedr_received_bits(q, cp, rx->repeat) != QR_ABSORBED))
				xp_fast_freemsg(cp);
			rx->cmp = NULL;
			rx->repeat = 0;
		}
		if (len <= max) {
			rx->cmp = xp_fast_dupb(mp);
			rx->repeat = 0;
		}
		if (sdt_daedr_received_bits(q, mp, 1) != QR_ABSORBED)
			xp_fast_freemsg(mp);
		rx->msg = NULL;
		return;
	}
	swerr();
	return;
}

/*
 *  RX LINKB
 *  ----------------------------------------
 *  Link a buffer to existing message or create new message with buffer.
 */
STATIC INLINE void
xp_rx_linkb(xp_path_t * rx)
{
	if (rx->msg)
		linkb(rx->msg, rx->nxt);
	else
		rx->msg = rx->nxt;
	rx->nxt = NULL;
	return;
}

#define RX_MODE_HUNT	0	/* hunting for flags */
#define RX_MODE_SYNC	1	/* between frames */
#define RX_MODE_MOF	2	/* middle of frame */
/*
 *  RX BLOCK
 *  ----------------------------------------
 *  Process a receive block for a channel or span.  We process all of the
 *  octets in the receive block.  Any complete messages will be delivered to
 *  the upper layer if the upper layer is not congested.  If the upper layer
 *  is congested we discard the message and indicate receive congestion.  The
 *  upper layer should be sensitive to its receive queue backlog and start
 *  sending SIB when required.  We do not use backenabling from the upper
 *  layer.  We merely start discarding complete messages when the upper layer
 *  is congested.
 */
STATIC INLINE void
xp_rx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats, const ulong type)
{
	int chan = 0;
	queue_t *q = xp->rq;
	register xp_path_t *rx = &xp->rx;
	int xsn = xp->option.popt & SS7_POPT_XSN;
	int M, mlen, hlen, N = xp->sdt.config.N;

	if (!xsn) {
		hlen = 3;
		mlen = 0x3f;
	} else {
		hlen = 6;
		mlen = 0x1ff;
	}
	M = xp->sdt.config.m + 1 + hlen;
	for (; bp < be; bp += 128) {
		rx_entry_t *r;

	      next_channel:
		switch (type) {
		default:
		case SDL_TYPE_DS0:
			r = rx_index8(rx->state, *bp);
			break;
		case SDL_TYPE_DS0A:
			r = rx_index7(rx->state, *bp);
			break;
		case SDL_TYPE_T1:
			r = rx_index8(rx->state, *(bp + (xp_t1_chan_map[chan] << 2)));
			if (++chan > 23)
				chan = 0;
			break;
		case SDL_TYPE_E1:
			r = rx_index8(rx->state, *(bp + (xp_e1_chan_map[chan] << 2)));
			if (++chan > 30)
				chan = 0;
			break;
		}
		rx->state = r->state;
		switch (rx->mode) {
		case RX_MODE_MOF:
			if (!r->sync && r->bit_length) {
				rx->residue |= r->bit_string << rx->rbits;
				rx->rbits += r->bit_length;
			}
			if (!r->flag) {
				if (!r->hunt && !r->idle) {
				      drain_rbits:
					if (rx->rbits <= 16)
						break;
					if (rx->nxt && rx->nxt->b_wptr >= rx->nxt->b_datap->db_lim)
						xp_rx_linkb(rx);
					if (rx->nxt || (rx->nxt = xp_fast_allocb())) {
						rx->bcc = (rx->bcc >> 8)
						    ^ bc_table[(rx->bcc ^ rx->residue) & 0x00ff];
						*rx->nxt->b_wptr++ = rx->residue;
						stats->rx_bytes++;
						rx->residue >>= 8;
						rx->rbits -= 8;
						if (++(rx->bytes) <= M)
							goto drain_rbits;
						else {
							stats->rx_frame_too_long++;
							stats->rx_frame_errors++;
						}
					} else {
						stats->rx_buffer_overflows++;
					}
				} else {
					stats->rx_aborts++;
					stats->rx_frame_errors++;
				}
			} else {
				if (rx->rbits == 16) {
					if (rx->bcc == (rx->residue & 0xffff)) {
						if (rx->bytes >= hlen) {
							uint len, li;

							xp_rx_linkb(rx);
							if (!xsn)
								li = rx->msg->b_rptr[2] & mlen;
							else
								li = ((rx->msg->b_rptr[5] << 8)
								      | rx->msg->b_rptr[4]) & mlen;
							len = rx->bytes - hlen;
							if (len == li || (li == mlen && len > li)) {
								stats->rx_sus++;
								xp_rx_compress(q, rx, hlen, stats);
							      new_frame:
								rx->mode = RX_MODE_SYNC;
								if (r->sync) {
								      begin_frame:
									if (r->bit_length) {
										rx->mode =
										    RX_MODE_MOF;
										rx->residue =
										    r->bit_string;
										rx->rbits =
										    r->bit_length;
										rx->bytes = 0;
										rx->bcc = 0x00ff;
									}
								}
								break;
							} else {
								stats->rx_length_error++;
							}
						} else {
							stats->rx_frame_too_short++;
							stats->rx_frame_errors++;
						}
					} else {
						stats->rx_crc_errors++;
					}
				} else {
					stats->rx_residue_errors++;
					stats->rx_frame_errors++;
				}
			}
		      abort_frame:
			if (rx->nxt)
				xp_fast_freemsg(xchg(&rx->nxt, NULL));
			if (rx->msg)
				xp_fast_freemsg(xchg(&rx->msg, NULL));
			stats->rx_sus_in_error++;
			sdt_daedr_su_in_error(q);
			if (r->flag)
				goto new_frame;
			rx->mode = RX_MODE_HUNT;
			stats->rx_sync_transitions++;
			rx->octets = 0;
			break;
		case RX_MODE_SYNC:
			if (!r->hunt && !r->idle)
				goto begin_frame;
			rx->mode = RX_MODE_HUNT;
			stats->rx_sync_transitions++;
			rx->octets = 0;
			break;
		case RX_MODE_HUNT:
			if (!r->flag) {
				if ((++rx->octets) >= N) {
					stats->rx_sus_in_error++;
					sdt_daedr_su_in_error(q);
					rx->octets -= N;
				}
				stats->rx_bits_octet_counted += 8;
				break;
			}
			stats->rx_sync_transitions++;
			goto new_frame;
		default:
			swerr();
			goto abort_frame;
		}
		if (chan)
			goto next_channel;
	}
}
STATIC void
xp_ds0a_rx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats)
{
	xp_rx_block(xp, bp, be, stats, SDL_TYPE_DS0A);
}
STATIC void
xp_ds0_rx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats)
{
	xp_rx_block(xp, bp, be, stats, SDL_TYPE_DS0);
}
STATIC void
xp_t1_rx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats)
{
	xp_rx_block(xp, bp, be, stats, SDL_TYPE_T1);
}
STATIC void
xp_e1_rx_block(xp_t * xp, unsigned char *bp, unsigned char *be, sdt_stats_t * stats)
{
	xp_rx_block(xp, bp, be, stats, SDL_TYPE_E1);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Table allocation and generation
 *
 *  -------------------------------------------------------------------------
 *  All Soft HDLC lookup stables are generated at module load time.  This
 *  permits the tables to be page-aligned in kernel memory for maximum cache
 *  performance.
 */
/*
 *  BC (Block Check) CRC Table Entries:
 *  -----------------------------------
 *  RC tables perform CRC calculation on received bits after zero deletion and
 *  delimitation.
 */
STATIC bc_entry_t
bc_table_value(int bit_string, int bit_length)
{
	int pos;

	for (pos = 0; pos < bit_length; pos++) {
		if (bit_string & 0x1)
			bit_string = (bit_string >> 1) ^ 0x8408;
		else
			bit_string >>= 1;
	}
	return (bit_string);
}

/*
 *  TX (Transmission) Table Entries:
 *  -----------------------------------
 *  TX table performs zero insertion on frame and CRC bit streams.
 */
STATIC tx_entry_t
tx_table_valueN(int state, uint8_t byte, int len)
{
	tx_entry_t result = { 0, };
	int bit_mask = 1;

	result.state = state;
	result.bit_length = 0;
	while (len--) {
		if (byte & 0x1) {
			result.bit_string |= bit_mask;
			if (result.state++ == 4) {
				result.state = 0;
				result.bit_length++;
				bit_mask <<= 1;
			}
		} else
			result.state = 0;
		bit_mask <<= 1;
		byte >>= 1;
	}
	return result;
}

STATIC tx_entry_t
tx_table_value(int state, uint8_t byte)
{
	return tx_table_valueN(state, byte, 8);
}

/*
 *  RX (Receive) Table Entries:
 *  -----------------------------------
 *  RX table performs zero deletion, flag and abort detection, BOF and EOF
 *  detection and residue on received bit streams.
 */
STATIC rx_entry_t
rx_table_valueN(int state, uint8_t byte, int len)
{
	rx_entry_t result = { 0, };
	int bit_mask = 1;

	result.state = state;
	while (len--) {
		switch (result.state) {
		case 0:	/* */
			if (result.flag && !result.sync) {
				bit_mask = 1;
				result.bit_string = 0;
				result.bit_length = 0;
				result.sync = 1;
			}
			if (byte & 0x1) {
				result.state = 8;
			} else {
				result.state = 1;
			}
			break;
		case 1:	/* 0 */
			if (byte & 0x1) {
				result.state = 2;
			} else {
				bit_mask <<= 1;
				result.bit_length += 1;
				result.state = 1;
			}
			break;
		case 2:	/* 01 */
			if (byte & 0x1) {
				result.state = 3;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 2;
				result.state = 1;
			}
			break;
		case 3:	/* 011 */
			if (byte & 0x1) {
				result.state = 4;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 3;
				result.state = 1;
			}
			break;
		case 4:	/* 0111 */
			if (byte & 0x1) {
				result.state = 5;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 4;
				result.state = 1;
			}
			break;
		case 5:	/* 01111 */
			if (byte & 0x1) {
				result.state = 6;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 5;
				result.state = 1;
			}
			break;
		case 6:	/* 011111 */
			if (byte & 0x1) {
				result.state = 7;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 6;
				result.state = 0;
			}
			break;
		case 7:	/* 0111111 */
			if (byte & 0x1) {
				result.sync = 0;
				result.flag = 0;
				result.hunt = 1;
				result.state = 12;
			} else {
				result.sync = 0;
				result.flag = 1;
				result.hunt = 0;
				result.idle = 0;
				result.state = 0;
			}
			break;
		case 8:	/* 1 */
			if (byte & 0x1) {
				result.state = 9;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 1;
				result.state = 1;
			}
			break;
		case 9:	/* 11 */
			if (byte & 0x1) {
				result.state = 10;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 2;
				result.state = 1;
			}
			break;
		case 10:	/* 111 */
			if (byte & 0x1) {
				result.state = 11;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 3;
				result.state = 1;
			}
			break;
		case 11:	/* 1111 */
			if (byte & 0x1) {
				result.state = 12;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 4;
				result.state = 1;
			}
			break;
		case 12:	/* 11111 */
			if (byte & 0x1) {
				result.state = 13;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 5;
				result.state = 0;
			}
			break;
		case 13:	/* 111111 */
			if (byte & 0x1) {
				result.hunt = 1;
				result.sync = 0;
				result.idle = 1;
				result.flag = 0;
				result.state = 12;
			} else {
				result.sync = 0;
				result.hunt = 0;
				result.idle = 0;
				result.flag = 1;
				result.state = 0;
			}
			break;
		}
		byte >>= 1;
	}
	return result;
}

STATIC rx_entry_t
rx_table_value7(int state, uint8_t byte)
{
	return rx_table_valueN(state, byte, 7);
}

STATIC rx_entry_t
rx_table_value8(int state, uint8_t byte)
{
	return rx_table_valueN(state, byte, 8);
}

/*
 *  TX (Transmit) Table:
 *  -----------------------------------
 *  There is one TX table for 8-bit (octet) output values.  The table has 256
 *  entries and is used to perform, for one sample, zero insertion on frame
 *  bits for the transmitted bitstream.
 */
STATIC void
tx_table_generate(void)
{
	int j, k;

	for (j = 0; j < SDT_TX_STATES; j++)
		for (k = 0; k < 256; k++)
			*tx_index(j, k) = tx_table_value(j, k);
}

/*
 *  RX (Received) Tables:
 *  -----------------------------------
 *  There are two RX tables: one for 8 bit (octet) values, another for 7 bit
 *  (DS0A operation).  Each table has 256 entries and is used to perform, for
 *  one sample, zero deletion, abort detection, flag detection and residue
 *  calculation on the received bitstream.
 */
STATIC void
rx_table_generate7(void)
{
	int j, k;

	for (j = 0; j < SDT_RX_STATES; j++)
		for (k = 0; k < 256; k++)
			*rx_index7(j, k) = rx_table_value7(j, k);
}
STATIC void
rx_table_generate8(void)
{
	int j, k;

	for (j = 0; j < SDT_RX_STATES; j++)
		for (k = 0; k < 256; k++)
			*rx_index8(j, k) = rx_table_value8(j, k);
}

/*
 *  BC (CRC) Tables:
 *  -----------------------------------
 *  CRC table organization:  This is a CRC table which contains lookups for
 *  all bit lengths less than or equal to 8.  There are 512 entries.  The
 *  first 256 entries are for 8-bit bit lengths, the next 128 entries are for
 *  7-bit bit lengths, the next 64 entries for 6-bit bit lengths, etc.
 */
STATIC void
bc_table_generate(void)
{
	int pos = 0, bit_string, bit_length = 8, bit_mask = 0x100;

	do {
		for (bit_string = 0; bit_string < bit_mask; bit_string++, pos++)
			bc_table[pos] = bc_table_value(bit_string, bit_length);
		bit_length--;
	} while ((bit_mask >>= 1));
}

/*
 *  Table allocation
 *  -------------------------------------------------------------------------
 */
STATIC int
xp_init_tables(void)
{
	size_t length;

	length = SDT_CRC_TABLE_LENGTH * sizeof(bc_entry_t);
	for (bc_order = 0; PAGE_SIZE << bc_order < length; bc_order++) ;
	if (!(bc_table = (bc_entry_t *) __get_free_pages(GFP_KERNEL, bc_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocated bc_table\n", __FUNCTION__);
		return (-ENOMEM);
	}
	printd(("sdt: allocated BC table size %u kernel pages\n", 1 << bc_order));
	length = SDT_TX_TABLE_LENGTH * sizeof(tx_entry_t);
	for (tx_order = 0; PAGE_SIZE << tx_order < length; tx_order++) ;
	if (!(tx_table = (tx_entry_t *) __get_free_pages(GFP_KERNEL, tx_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocated tx_table\n", __FUNCTION__);
		free_pages((unsigned long) bc_table, xchg(&bc_order, 0));
		return (-ENOMEM);
	}
	printd(("sdt: allocated Tx table size %u kernel pages\n", 1 << tx_order));
	length = 2 * (SDT_RX_TABLE_LENGTH * sizeof(rx_entry_t));
	for (rx_order = 0; PAGE_SIZE << rx_order < length; rx_order++) ;
	if (!(rx_table = (rx_entry_t *) __get_free_pages(GFP_KERNEL, rx_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocated rx_table\n", __FUNCTION__);
		free_pages((unsigned long) bc_table, xchg(&bc_order, 0));
		free_pages((unsigned long) tx_table, xchg(&tx_order, 0));
		return (-ENOMEM);
	}
	printd(("sdt: allocated Rx table size %u kernel pages\n", 1 << rx_order));
	rx_table7 = (rx_entry_t *) (((uint8_t *) rx_table) + (PAGE_SIZE << (rx_order - 1)));
	bc_table_generate();
	printd(("sdt: generated BC table\n"));
	tx_table_generate();
	printd(("sdt: generated 8-bit Tx table\n"));
	rx_table_generate8();
	printd(("sdt: generated 8-bit Rx table\n"));
	rx_table_generate7();
	printd(("sdt: generated 7-bit Rx table\n"));
	return (0);
}
STATIC int
xp_free_tables(void)
{
	free_pages((unsigned long) bc_table, bc_order);
	printd(("sdt: freed BC table kernel pages\n"));
	free_pages((unsigned long) tx_table, tx_order);
	printd(("sdt: freed Tx table kernel pages\n"));
	free_pages((unsigned long) rx_table, rx_order);
	printd(("sdt: freed Rx table kernel pages\n"));
	return (0);
}

/*
 *  =========================================================================
 *
 *  EVENTS From Above
 *
 *  =========================================================================
 */

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
xp_send_data(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	int ret;
	psw_t flags = 0;

	if (xp->state != LMI_ENABLED)
		return (-EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	{
		if (xp->sdt.statem.daedt_state != SDT_STATE_IDLE)
			ret = (-EAGAIN);	/* wait for tx-block to take message */
		else
			ret = (-EPROTO);	/* ignore transmissions when daedt shut down */
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (ret);
}

/*
 *  SDT_DAEDT_TRANSMISSION_REQ:
 *  -----------------------------------
 *  This is the non-preferred way of sending data.  It is preferred that
 *  M_DATA blocks are used (above).  We just strip off the M_PROTO and requeue
 *  only the M_DATA blocks.
 */
STATIC int
sdt_daedt_transmission_req(queue_t *q, mblk_t *mp)
{
	mblk_t *dp;

	if ((dp = mp->b_cont) && dp->b_datap->db_type == M_DATA)
		return (QR_STRIP);
	return (-EPROTO);
}

/*
 *  SDT_DAEDT_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedt_start_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	if (xp->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	do {
		if (xp->sdt.statem.daedt_state != SDT_STATE_IDLE)
			break;	/* already running */
		xp->sdt.statem.daedt_state = SDT_STATE_IN_SERVICE;
		if (xp->sdl.statem.tx_state != SDL_STATE_IDLE)
			break;	/* already running */
		xp->sdl.statem.tx_state = SDL_STATE_IN_SERVICE;
		if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING)
			break;	/* already running */
		xp->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING);
		break;		/* already running */
	} while (0);
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  SDT_DAEDR_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedr_start_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	if (xp->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	do {
		if (xp->sdt.statem.daedr_state != SDT_STATE_IDLE)
			break;	/* already running */
		xp->sdt.statem.daedr_state = SDT_STATE_IN_SERVICE;
		if (xp->sdl.statem.rx_state != SDL_STATE_IDLE)
			break;	/* already running */
		xp->sdl.statem.rx_state = SDL_STATE_IN_SERVICE;
		if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING)
			break;	/* already running */
		xp->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_RX_RUNNING);
	} while (0);
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  SDT_AERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_start_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	if (xp->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	{
		xp->sdt.statem.Ca = 0;
		xp->sdt.statem.aborted_proving = 0;
		xp->sdt.statem.aerm_state = SDT_STATE_MONITORING;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  SDT_AERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_stop_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	if (xp->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	{
		xp->sdt.statem.aerm_state = SDT_STATE_IDLE;
		xp->sdt.statem.Ti = xp->sdt.config.Tin;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  SDT_AERM_SET_TI_TO_TIN_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tin_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	if (xp->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	{
		if (xp->sdt.statem.aerm_state == SDT_STATE_IDLE)
			xp->sdt.statem.Ti = xp->sdt.config.Tin;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  SDT_AERM_SET_TI_TO_TIE_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tie_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	if (xp->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	{
		if (xp->sdt.statem.aerm_state == SDT_STATE_IDLE)
			xp->sdt.statem.Ti = xp->sdt.config.Tie;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  SDT_SUERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_start_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;
	int ret;

	(void) mp;
	if (xp->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	{
		if (xp->option.popt & SS7_POPT_HSL)
			ret = sdt_eim_start(q);
		else
			ret = sdt_suerm_start(q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (ret);
}

/* 
 *  SDT_SUERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_stop_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	if (xp->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	spin_lock_irqsave(&xp->lock, flags);
	{
		sdt_eim_stop(q);
		sdt_suerm_stop(q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);

	switch (xp->state) {
	case LMI_ENABLED:
	case LMI_DISABLED:
	{
		xp_span_t *sp;
		xp_card_t *cp;

		if ((sp = xp->sp) && (cp = sp->cp)) {
			uint16_t ppa =
			    (xp->
			     chan & 0xff) | ((sp->span & 0x0f) << 8) | ((cp->card & 0x0f) << 12);
			return lmi_info_ack(q, (caddr_t) &ppa, sizeof(ppa));
		}
	}
	}
	return lmi_info_ack(q, NULL, 0);
}

/* 
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	psw_t flags = 0;
	int err, card, span, chan, slot;
	xp_card_t *cp;
	xp_span_t *sp = NULL;
	uint16_t ppa;
	xp_t *xp = PRIV(q);
	lmi_attach_req_t *p = ((typeof(p)) mp->b_rptr);

	if (mp->b_wptr - mp->b_rptr < sizeof(*p) + sizeof(ppa)) {
		ptrace(("X400P-SS7: ERROR: primitive too small = %d bytes\n",
			mp->b_wptr - mp->b_rptr));
		goto lmi_badprim;
	}
	if (xp->state != LMI_UNATTACHED) {
		ptrace(("X400P-SS7: ERROR: interface out of state\n"));
		goto lmi_outstate;
	}
	xp->state = LMI_ATTACH_PENDING;
	ppa = *(typeof(ppa) *) (p + 1);
	/* check card */
	card = (ppa >> 12) & 0x0f;
	for (cp = x400p_cards; cp && cp->card != card; cp = cp->next) ;
	if (!cp) {
		ptrace(("X400P-SS7: ERROR: invalid card %d\n", card));
		goto lmi_badppa;
	}
	/* check span */
	span = (ppa >> 8) & 0x0f;
	if (span < 0 || span > 3) {
		ptrace(("X400P-SS7: ERROR: invalid span %d\n", span));
		goto lmi_badppa;
	}
	if (!(sp = cp->spans[span])) {
		ptrace(("X400P-SS7: ERROR: unallocated span %d\n", span));
		goto lmi_badppa;
	}
	if (sp->ifgtype != SDL_GTYPE_E1 && sp->ifgtype != SDL_GTYPE_T1) {
		swerr();
		goto efault;
	}
	/* check chan */
	chan = (ppa >> 0) & 0xff;
	if (chan) {
		/* specific channel indicated */
		switch (cp->ifgtype) {
		case SDL_GTYPE_E1:
			if (chan < 1 || chan > 31) {
				ptrace(("E400P-SS7: ERROR: invalid chan %d\n", chan));
				goto lmi_badppa;
			}
			slot = xp_e1_chan_map[chan - 1];
			if (sp->slots[slot]) {
				ptrace(("E400P-SS7: ERROR: slot %d in use\n", slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("E400P-SS7: attaching card %d, span %d, chan %d, slot %d\n", card,
				span, chan, slot));
			spin_lock_irqsave(&xp->lock, flags);
			{
				sp->slots[slot] = xp;
				xp->refcnt++;
				xp->sp = sp;
				sp->refcnt++;
				sp->iftype = SDL_TYPE_DS0;
				sp->ifrate = 64000;
				xp->state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = slot;
				/* LMI configuration defaults */
				xp->option.pvar = SS7_PVAR_ITUT_00;
				xp->option.popt = 0;
				/* SDL configuration defaults */
				xp->sdl.config.ifflags = 0;
				xp->sdl.config.iftype = SDL_TYPE_DS0;
				xp->sdl.config.ifmode = SDL_MODE_PEER;
				xp->sdl.config.ifrate = 64000;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config.Tin = 4;
				xp->sdt.config.Tie = 1;
				xp->sdt.config.T = 64;
				xp->sdt.config.D = 256;
				xp->sdt.config.t8 = 100 * HZ / 1000;
				xp->sdt.config.Te = 793544;
				xp->sdt.config.De = 11328000;
				xp->sdt.config.Ue = 198384000;
				xp->sdt.config.N = 16;
				xp->sdt.config.m = 272;
				xp->sdt.config.b = 8;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		case SDL_GTYPE_T1:
			if (chan < 1 || chan > 24) {
				ptrace(("T400P-SS7: ERROR: invalid chan %d\n", chan));
				goto lmi_badppa;
			}
			slot = xp_t1_chan_map[chan - 1];
			if (sp->slots[slot]) {
				ptrace(("T400P-SS7: ERROR: slot %d in use\n", slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("T400P-SS7: attaching card %d, span %d, chan %d, slot %d\n", card,
				span, chan, slot));
			spin_lock_irqsave(&xp->lock, flags);
			{
				sp->slots[slot] = xp;
				xp->refcnt++;
				xp->sp = sp;
				sp->refcnt++;
				sp->iftype = SDL_TYPE_DS0;
				sp->ifrate = 64000;
				xp->state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = slot;
				/* LMI configuration defaults */
				xp->option.pvar = SS7_PVAR_ANSI_00;
				xp->option.popt = SS7_POPT_MPLEV;
				/* SDL configuration defaults */
				xp->sdl.config.ifflags = 0;
				xp->sdl.config.iftype = SDL_TYPE_DS0;
				xp->sdl.config.ifmode = SDL_MODE_PEER;
				xp->sdl.config.ifrate = 64000;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config.Tin = 4;
				xp->sdt.config.Tie = 1;
				xp->sdt.config.T = 64;
				xp->sdt.config.D = 256;
				xp->sdt.config.t8 = 100 * HZ / 1000;
				xp->sdt.config.Te = 577169;
				xp->sdt.config.De = 9308000;
				xp->sdt.config.Ue = 144292000;
				xp->sdt.config.N = 16;
				xp->sdt.config.m = 272;
				xp->sdt.config.b = 8;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		}
	} else {
		int c;

		/* entire span indicated */
		switch (cp->ifgtype) {
		case SDL_GTYPE_E1:
			for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]); c++)
				if (sp->slots[xp_e1_chan_map[c]]) {
					ptrace(("E400P-SS7: ERROR: slot in use for chan %d\n", c));
					goto lmi_badppa;
				}
			if ((err = lmi_ok_ack(q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("E400P-SS7: attaching card %d, entire span %d\n", card, span));
			spin_lock_irqsave(&xp->lock, flags);
			{
				for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]);
				     c++) {
					slot = xp_e1_chan_map[c];
					sp->slots[slot] = xp;
					xp->refcnt++;
				}
				xp->sp = sp;
				sp->refcnt++;
				sp->iftype = SDL_TYPE_E1;
				sp->ifrate = 2048000;
				xp->state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = 0;
				/* LMI configuration defaults */
				xp->option.pvar = SS7_PVAR_ITUT_00;
				xp->option.popt = SS7_POPT_HSL | SS7_POPT_XSN;
				/* SDL configuration defaults */
				xp->sdl.config.ifflags = 0;
				xp->sdl.config.iftype = SDL_TYPE_E1;
				xp->sdl.config.ifmode = SDL_MODE_PEER;
				xp->sdl.config.ifrate = 2048000;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config.Tin = 4;
				xp->sdt.config.Tie = 1;
				xp->sdt.config.T = 64;
				xp->sdt.config.D = 256;
				xp->sdt.config.t8 = 100 * HZ / 1000;
				xp->sdt.config.Te = 793544;
				xp->sdt.config.De = 11328000;
				xp->sdt.config.Ue = 198384000;
				xp->sdt.config.N = 16;
				xp->sdt.config.m = 272;
				xp->sdt.config.b = 8;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		case SDL_GTYPE_T1:
			for (c = 0; c < (sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0])); c++)
				if (sp->slots[xp_t1_chan_map[c]]) {
					ptrace(("T400P-SS7: ERROR: slot in use for chan %d\n", c));
					goto lmi_badppa;
				}
			if ((err = lmi_ok_ack(q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			spin_lock_irqsave(&xp->lock, flags);
			{
				printd(("T400P-SS7: attaching card %d, entire span %d\n", card,
					span));
				for (c = 0;
				     c < (sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0]));
				     c++) {
					slot = xp_t1_chan_map[c];
					sp->slots[slot] = xp;
					xp->refcnt++;
				}
				xp->sp = sp;
				sp->refcnt++;
				sp->iftype = SDL_TYPE_T1;
				sp->ifrate = 1544000;
				xp->state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = 0;
				/* LMI configuration defaults */
				xp->option.pvar = SS7_PVAR_ANSI_00;
				xp->option.popt = SS7_POPT_MPLEV | SS7_POPT_HSL | SS7_POPT_XSN;
				/* SDL configuration defaults */
				xp->sdl.config.ifflags = 0;
				xp->sdl.config.iftype = SDL_TYPE_T1;
				xp->sdl.config.ifmode = SDL_MODE_PEER;
				xp->sdl.config.ifrate = 1544000;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config.Tin = 4;
				xp->sdt.config.Tie = 1;
				xp->sdt.config.T = 64;
				xp->sdt.config.D = 256;
				xp->sdt.config.t8 = 100 * HZ / 1000;
				xp->sdt.config.Te = 577169;
				xp->sdt.config.De = 9308000;
				xp->sdt.config.Ue = 144292000;
				xp->sdt.config.N = 16;
				xp->sdt.config.m = 272;
				xp->sdt.config.b = 8;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		}
	}
	swerr();
	{
		int errno, reason;

	      efault:
		errno = EFAULT;
		reason = LMI_SYSERR;
		goto error_out;
	      lmi_outstate:
		errno = 0;
		reason = LMI_OUTSTATE;
		goto error_out;
	      lmi_badprim:
		errno = 0;
		reason = LMI_BADPRIM;
		goto error_out;
	      lmi_badppa:
		errno = 0;
		reason = LMI_BADPPA;
		goto error_out;
	      error_out:
		return lmi_error_ack(q, LMI_UNATTACHED, LMI_ATTACH_REQ, errno, reason);
	}
}

/* 
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	xp_span_t *sp;
	xp_card_t *cp;
	int err, slot;
	psw_t flags = 0;

	/* validate detach */
	if (xp->state != LMI_DISABLED)
		return lmi_error_ack(q, xp->state, LMI_DETACH_REQ, 0, LMI_OUTSTATE);
	if (!(sp = xp->sp) || !(cp = sp->cp)) {
		swerr();
		return m_error(q, EFAULT);
	}
	xp->state = LMI_DETACH_PENDING;
	if ((err = lmi_ok_ack(q, LMI_UNATTACHED, LMI_DETACH_REQ)))
		return (err);
	/* commit detach */
	spin_lock_irqsave(&xp->lock, flags);
	{
		for (slot = 0; slot < 32; slot++) {
			if (sp->slots[slot] == xp) {
				sp->slots[slot] = NULL;
				xp->refcnt--;
			}
		}
		xp->sp = NULL;
		sp->refcnt--;
		xp->chan = 0;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	int err, offset;
	xp_t *xp = PRIV(q);
	xp_card_t *cp;
	xp_span_t *sp;

	/* validate enable */
	if (xp->state != LMI_DISABLED) {
		ptrace(("X400P-SS7: ERROR: out of state: state = %d\n", xp->state));
		goto lmi_outstate;
	}
	if (!(sp = xp->sp)) {
		ptrace(("X400P-SS7: ERROR: out of state: no span pointer\n"));
		goto lmi_outstate;
	}
	if (!(cp = sp->cp)) {
		ptrace(("X400P-SS7: ERROR: out of state: no card pointer\n"));
		goto lmi_outstate;
	}
#ifdef _DEBUG
	if (cp->ifgtype != SDL_GTYPE_E1 && cp->ifgtype != SDL_GTYPE_T1) {
		ptrace(("X400P-SS7: ERROR: card group type = %lu\n", cp->ifgtype));
		return m_error(q, EFAULT);
	}
#endif
	if (xp->sdl.config.ifflags & SDL_IF_UP) {
		ptrace(("X400P-SS7: ERROR: out of state: device already up\n"));
		goto lmi_outstate;
	}
	if ((err = lmi_enable_con(q)))
		return (err);
	/* commit enable */
	printd(("X400P-SS7: performing enable\n"));
	xp->state = LMI_ENABLE_PENDING;
	xp->ifname = sp->ifname;
	xp->sdl.config.iftype = xp->sdl.config.iftype;
	switch (xp->sdl.config.iftype) {
	case SDL_TYPE_E1:
		xp->sdl.config.ifrate = 2048000;
		break;
	case SDL_TYPE_T1:
		xp->sdl.config.ifrate = 1544000;
		break;
	case SDL_TYPE_DS0:
		xp->sdl.config.ifrate = 64000;
		break;
	case SDL_TYPE_DS0A:
		xp->sdl.config.ifrate = 56000;
		break;
	}
	xp->sdl.config.ifgtype = sp->ifgtype;
	xp->sdl.config.ifgrate = sp->ifgrate;
	xp->sdl.config.ifgcrc = sp->ifgcrc;
	xp->sdl.config.ifmode = SDL_MODE_PEER;
	xp->sdl.config.ifgmode = sp->ifgmode;
	xp->sdl.config.ifclock = sp->ifclock;
	xp->sdl.config.ifcoding = sp->ifcoding;
	xp->sdl.config.ifframing = sp->ifframing;
	xp->sdl.config.iftxlevel = sp->iftxlevel;
	xp->sdl.config.ifsync = cp->ifsync;
	xp->sdl.config.ifflags |= SDL_IF_UP;
	xp->state = LMI_ENABLED;
	if (!(sp->ifflags & SDL_IF_UP)) {
		/* need to bring up span */
		int span = sp->span;
		int base = span << 8;
		uint8_t ccr1 = 0, tcr1 = 0;
		unsigned long timeout;

		switch (cp->ifgtype) {
		case SDL_GTYPE_E1:
		{
			psw_t flags = 0;

			printd(("E400P-SS7: performing enable on E1 span %d\n", span));
			spin_lock_irqsave(&cp->lock, flags);
			// cp->xlb[SYNREG] = SYNCSELF; /* NO, NO, NO */
			/* Tell ISR to re-evaluate the sync source */
			cp->eval_syncsrc = 1;
			cp->xlb[CTLREG] = (E1DIV);
			cp->xlb[LEDREG] = 0xff;
			/* zero all span registers */
			for (offset = 0; offset < 192; offset++)
				cp->xlb[base + offset] = 0x00;
			/* Set up for interleaved serial bus operation, byte mode */
			if (span == 0)
				cp->xlb[base + 0xb5] = 0x09;
			else
				cp->xlb[base + 0xb5] = 0x08;
			cp->xlb[base + 0x1a] = 0x04;	/* CCR2: set LOTCMC */
			for (offset = 0; offset <= 8; offset++)
				cp->xlb[base + offset] = 0x00;
			for (offset = 0x10; offset <= 0x4f; offset++)
				if (offset != 0x1a)
					cp->xlb[base + offset] = 0x00;
			cp->xlb[base + 0x10] = 0x20;	/* RCR1: Rsync as input */
			cp->xlb[base + 0x11] = 0x06;	/* RCR2: Sysclk = 2.048 Mhz */
			cp->xlb[base + 0x12] = 0x09;	/* TCR1: TSiS mode */
			tcr1 = 0x09;	/* TCR1: TSiS mode */
			switch (sp->ifframing) {
			default:
			case SDL_FRAMING_CCS:
				ccr1 |= 0x08;
				break;
			case SDL_FRAMING_CAS:
				tcr1 |= 0x20;
				break;
			}
			switch (sp->ifcoding) {
			default:
			case SDL_CODING_HDB3:
				ccr1 |= 0x44;
				break;
			case SDL_CODING_AMI:
				ccr1 |= 0x00;
				break;
			}
			switch (sp->ifgcrc) {
			case SDL_GCRC_CRC4:
				ccr1 |= 0x11;
				break;
			default:
				ccr1 |= 0x00;
				break;
			}
			cp->xlb[base + 0x12] = tcr1;
			cp->xlb[base + 0x14] = ccr1;
			cp->xlb[base + 0x18] = 0x20;	/* 120 Ohm, Normal */
			cp->xlb[base + 0x1b] = 0x8a;	/* CRC3: LIRST & TSCLKM */
			cp->xlb[base + 0x20] = 0x1b;	/* TAFR */
			cp->xlb[base + 0x21] = 0x5f;	/* TNAFR */
			cp->xlb[base + 0x40] = 0x0b;	/* TSR1 */
			for (offset = 0x41; offset <= 0x4f; offset++)
				cp->xlb[base + offset] = 0x55;
			for (offset = 0x22; offset <= 0x25; offset++)
				cp->xlb[base + offset] = 0xff;
			timeout = jiffies + 100 * HZ / 1000;
			spin_unlock_irqrestore(&cp->lock, flags);
			while (jiffies < timeout) ;
			spin_lock_irqsave(&cp->lock, flags);
			cp->xlb[base + 0x1b] = 0x9a;	/* CRC3: set ESR as well */
			cp->xlb[base + 0x1b] = 0x82;	/* CRC3: TSCLKM only */
			sp->ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
			/* enable interrupts */
			cp->xlb[CTLREG] = (INTENA | E1DIV);
			spin_unlock_irqrestore(&cp->lock, flags);
			break;
		}
		case SDL_GTYPE_T1:
		{
			int byte, val, c;
			unsigned short mask = 0;
			psw_t flags = 0;

			printd(("T400P-SS7: performing enable on T1 span %d\n", span));
			spin_lock_irqsave(&cp->lock, flags);
			// cp->xlb[SYNREG] = SYNCSELF; /* NO, NO, NO */
			/* Tell ISR to re-evaluate the sync source */
			cp->eval_syncsrc = 1;
			cp->xlb[CTLREG] = 0;
			cp->xlb[LEDREG] = 0xff;
			for (offset = 0; offset < 160; offset++)
				cp->xlb[base + offset] = 0x00;
			/* Set up for interleaved serial bus operation, byte mode */
			if (span == 0)
				cp->xlb[base + 0x94] = 0x09;
			else
				cp->xlb[base + 0x94] = 0x08;
			cp->xlb[base + 0x2b] = 0x08;	/* Full-on sync required (RCR1) */
			cp->xlb[base + 0x2c] = 0x08;	/* RSYNC is an input (RCR2) */
			cp->xlb[base + 0x35] = 0x10;	/* RBS enable (TCR1) */
			cp->xlb[base + 0x36] = 0x04;	/* TSYNC to be output (TCR2) */
			cp->xlb[base + 0x37] = 0x9c;	/* Tx & Rx Elastic stor, sysclk(s) = 2.048
							   mhz, loopback controls (CCR1) */
			cp->xlb[base + 0x12] = 0x22;	/* Set up received loopup and loopdown
							   codes */
			cp->xlb[base + 0x14] = 0x80;
			cp->xlb[base + 0x15] = 0x80;
			/* Enable F bits pattern */
			switch (sp->ifframing) {
			default:
			case SDL_FRAMING_SF:
				val = 0x20;
				break;
			case SDL_FRAMING_ESF:
				val = 0x88;
				break;
			}
			switch (sp->ifcoding) {
			default:
			case SDL_CODING_AMI:
				break;
			case SDL_CODING_B8ZS:
				val |= 0x44;
				break;
			}
			cp->xlb[base + 0x38] = val;
			if (sp->ifcoding != SDL_CODING_B8ZS)
				cp->xlb[base + 0x7e] = 0x1c;	/* Set FDL register to 0x1c */
			cp->xlb[base + 0x7c] = sp->iftxlevel << 5;	/* LBO */
			cp->xlb[base + 0x0a] = 0x80;	/* LIRST to reset line interface */
			timeout = jiffies + 100 * HZ / 1000;
			spin_unlock_irqrestore(&cp->lock, flags);
			while (jiffies < timeout) ;
			spin_lock_irqsave(&cp->lock, flags);
			cp->xlb[base + 0x0a] = 0x30;	/* LIRST bask to normal, Resetting elastic
							   buffers */
			sp->ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
			/* enable interrupts */
			cp->xlb[CTLREG] = (INTENA);
			spin_unlock_irqrestore(&cp->lock, flags);
			/* establish which channels are clear channel */
			for (c = 0; c < 24; c++) {
				int slot = xp_t1_chan_map[c];

				byte = c >> 3;
				if (!cp->spans[span]->slots[slot]
				    || cp->spans[span]->slots[slot]->sdl.config.iftype !=
				    SDL_TYPE_DS0A)
					mask |= 1 << (c % 8);
				if ((c % 8) == 7)
					cp->xlb[base + 0x39 + byte] = mask;
			}
			break;
		}
		default:
			swerr();
			break;
		}
	}
	return (QR_DONE);
      lmi_outstate:
	return lmi_error_ack(q, xp->state, LMI_ENABLE_REQ, 0, LMI_OUTSTATE);
}

/* 
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	int err;
	psw_t flags = 0;

	/* validate disable */
	if (xp->state != LMI_ENABLED)
		goto lmi_outstate;
	xp->state = LMI_DISABLE_PENDING;
	/* perform disable */
	if ((err = lmi_disable_con(q)))
		return (err);
	spin_lock_irqsave(&xp->lock, flags);
	{
		/* commit disable */
		/* stop transmitters and receivers */
		xp->sdl.config.ifflags = 0;
		xp->sdl.statem.tx_state = SDL_STATE_IDLE;
		xp->sdl.statem.rx_state = SDL_STATE_IDLE;
		/* stop daedr and daedt */
		xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
		xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
		/* stop aerm */
		xp->sdt.statem.aerm_state = SDT_STATE_IDLE;
		xp->sdt.statem.Ti = xp->sdt.config.Tin;
		/* stop eim */
		xp->sdt.statem.eim_state = SDT_STATE_IDLE;
		if (xp->sdt.timers.t8)
			untimeout(xchg(&xp->sdt.timers.t8, 0));
		/* stop suerm */
		xp->sdt.statem.suerm_state = SDT_STATE_IDLE;
		/* reset transmitter and receiver state */
		if (xp->rx.msg)
			freemsg(xchg(&xp->rx.msg, NULL));
		if (xp->rx.nxt)
			freemsg(xchg(&xp->rx.nxt, NULL));
		if (xp->tx.msg)
			freemsg(xchg(&xp->tx.msg, NULL));
		if (xp->tx.nxt)
			xp->tx.nxt = NULL;
		bzero(&xp->tx, sizeof(xp->tx));
		bzero(&xp->rx, sizeof(xp->tx));
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      lmi_outstate:
	return lmi_error_ack(q, xp->state, LMI_DISABLE_REQ, 0, LMI_OUTSTATE);
}

/* 
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	fixme(("FIXME: must check for options change\n"));
	return (QR_PASSALONG);
}

/* 
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 */
/* 
 *  -------------------------------------------------------------------------
 *
 *  Test and Commit SDT configuration settings
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_test_config(xp_t * xp, sdt_config_t * arg)
{
	int ret = 0;
	psw_t flags = 0;

	spin_lock_irqsave(&xp->lock, flags);
	do {
		if (!arg->t8)
			arg->t8 = xp->sdt.config.t8;
		if (!arg->Tin)
			arg->Tin = xp->sdt.config.Tin;
		if (!arg->Tie)
			arg->Tie = xp->sdt.config.Tie;
		if (!arg->T)
			arg->T = xp->sdt.config.T;
		if (!arg->D)
			arg->D = xp->sdt.config.D;
		if (!arg->Te)
			arg->Te = xp->sdt.config.Te;
		if (!arg->De)
			arg->De = xp->sdt.config.De;
		if (!arg->Ue)
			arg->Ue = xp->sdt.config.Ue;
		if (!arg->N)
			arg->N = xp->sdt.config.N;
		if (!arg->m)
			arg->m = xp->sdt.config.m;
		if (!arg->b)
			arg->b = xp->sdt.config.b;
		else if (arg->b != xp->sdt.config.b) {
			ret = -EINVAL;
			break;
		}
	} while (0);
	spin_unlock_irqrestore(&xp->lock, flags);
	return (ret);
}
STATIC int
sdt_commit_config(xp_t * xp, sdt_config_t * arg)
{
	psw_t flags = 0;

	spin_lock_irqsave(&xp->lock, flags);
	{
		sdt_test_config(xp, arg);
		xp->sdt.config = *arg;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (0);
}

STATIC int
sdt_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->option;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->option = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdt.config;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sdt.config = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		return sdt_test_config(xp, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		return sdt_commit_config(xp, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdt.statem;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccmreset(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);

	(void) xp;
	(void) mp;
	fixme(("Master reset\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sdt_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdt.statsp;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sdt.statsp = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdt.stats;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccstats(queue_t *q, mblk_t *mp)
{
	psw_t flags = 0;
	xp_t *xp = PRIV(q);

	(void) mp;
	spin_lock_irqsave(&xp->lock, flags);
	{
		bzero(&xp->sdt.stats, sizeof(xp->sdt.stats));
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (0);
}
STATIC int
sdt_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdt.notify;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sdt.notify = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sdt.notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Test SDL configuration settings
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_test_config(xp_t * xp, sdl_config_t * arg)
{
	int ret = 0;
	psw_t flags = 0;

	if (!xp)
		return (-EFAULT);
	spin_lock_irqsave(&xp->lock, flags);
	do {
		if (arg->ifflags) {
			trace();
			ret = -EINVAL;
			break;
		}
		switch (arg->iftype) {
		case SDL_TYPE_NONE:	/* unknown/unspecified */
			arg->iftype = xp->sdl.config.iftype;
			break;
		case SDL_TYPE_DS0:	/* DS0 channel */
		case SDL_TYPE_DS0A:	/* DS0A channel */
			/* yes we allow DS0A on E1 */
			break;
		case SDL_TYPE_E1:	/* full E1 span */
			if (xp->sp->ifgtype != SDL_GTYPE_E1) {
				trace();
				ret = (-EINVAL);
				break;
			}
			break;
		case SDL_TYPE_T1:	/* full T1 span */
			if (xp->sp->ifgtype != SDL_GTYPE_T1) {
				trace();
				ret = (-EINVAL);
				break;
			}
			break;
		default:
			trace();
			ret = (-EINVAL);
			break;
		}
		if (ret)
			break;
		switch (arg->ifmode) {
		case SDL_MODE_NONE:	/* */
			arg->ifmode = xp->sdl.config.ifmode;
			break;
		case SDL_MODE_PEER:	/* */
			break;
		case SDL_MODE_ECHO:	/* */
		case SDL_MODE_REM_LB:	/* */
		case SDL_MODE_LOC_LB:	/* */
		case SDL_MODE_LB_ECHO:	/* */
		case SDL_MODE_TEST:	/* */
			break;
		default:
			trace();
			ret = (-EINVAL);
			break;
		}
		switch (arg->ifgmode) {
		case SDL_GMODE_NONE:
		case SDL_GMODE_LOC_LB:
			break;
		case SDL_GMODE_REM_LB:
		default:
			trace();
			return (-EINVAL);
			break;
		}
		if (ret)
			break;
		if (xp->sp) {
			switch (arg->ifgtype) {
			case SDL_GTYPE_NONE:	/* */
				arg->ifgtype = xp->sp->ifgtype;
				break;
			case SDL_GTYPE_T1:	/* */
			case SDL_GTYPE_E1:	/* */
				if (arg->ifgtype != xp->sp->ifgtype) {
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			default:
				trace();
				ret = (-EINVAL);
				break;
			}
			if (ret)
				break;
			switch (arg->ifgcrc) {
			case SDL_GCRC_NONE:	/* */
				switch (arg->ifgtype) {
				case SDL_GTYPE_E1:
					arg->ifgcrc = SDL_GCRC_CRC5;
					break;
				case SDL_GTYPE_T1:
					arg->ifgcrc = SDL_GCRC_CRC6;
					break;
				default:
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_GCRC_CRC4:	/* */
				if (arg->ifgtype != SDL_GTYPE_E1) {
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_GCRC_CRC5:	/* */
				if (arg->ifgtype != SDL_GTYPE_E1) {
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_GCRC_CRC6:	/* */
				if (arg->ifgtype != SDL_GTYPE_T1) {
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			default:
				trace();
				ret = (-EINVAL);
				break;
			}
			if (ret)
				break;
			switch (arg->ifclock) {
			case SDL_CLOCK_NONE:	/* */
				arg->ifclock = xp->sp->ifclock;
				break;
			case SDL_CLOCK_INT:	/* */
			case SDL_CLOCK_MASTER:	/* */
				arg->ifclock = SDL_CLOCK_MASTER;
				break;
			case SDL_CLOCK_EXT:	/* */
			case SDL_CLOCK_SLAVE:	/* */
				arg->ifclock = SDL_CLOCK_SLAVE;
				break;
			case SDL_CLOCK_LOOP:	/* */
				break;
			default:
				trace();
				ret = (-EINVAL);
				break;
			}
			if (ret)
				break;
			switch (arg->ifcoding) {
			case SDL_CODING_NONE:	/* */
				arg->ifcoding = xp->sp->ifcoding;
				break;
			case SDL_CODING_AMI:	/* */
				break;
			case SDL_CODING_B8ZS:	/* */
				if (arg->ifgtype != SDL_GTYPE_T1) {
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_CODING_HDB3:	/* */
				if (arg->ifgtype != SDL_GTYPE_E1) {
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			default:
			case SDL_CODING_B6ZS:	/* */
				trace();
				ret = (-EINVAL);
				break;
			}
			if (ret)
				break;
			switch (arg->ifframing) {
			case SDL_FRAMING_NONE:	/* */
				arg->ifframing = xp->sp->ifframing;
				break;
			case SDL_FRAMING_CCS:	/* */
			case SDL_FRAMING_CAS:	/* */
				if (arg->ifgtype != SDL_GTYPE_E1) {
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_FRAMING_SF:	/* */
			case SDL_FRAMING_ESF:	/* */
				if (arg->ifgtype != SDL_GTYPE_T1) {
					trace();
					ret = (-EINVAL);
					break;
				}
				break;
			default:
				trace();
				ret = (-EINVAL);
				break;
			}
			if (ret)
				break;
			switch (arg->iftxlevel) {
			case 0:
				arg->iftxlevel = xp->sp->iftxlevel;
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				break;
			default:
				trace();
				ret = (-EINVAL);
				break;
			}
			if (ret)
				break;
			if (xp->sp->cp) {
				int src;

				for (src = 0; src < SDL_SYNCS; src++)
					if (arg->ifsyncsrc[src] < 0 || arg->ifsyncsrc[src] > 4) {
						trace();
						ret = (-EINVAL);
						break;
					}
				if (ret)
					break;
			} else {
				ret = (-EFAULT);
				break;
			}
		} else {
			ret = (-EFAULT);
			break;
		}
	} while (0);
	spin_unlock_irqrestore(&xp->lock, flags);
	return (ret);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Commit SDL configuration settings
 *
 *  -------------------------------------------------------------------------
 */
STATIC void
sdl_commit_config(xp_t * xp, sdl_config_t * arg)
{
	int chan_reconfig = 0, span_reconfig = 0, card_reconfig = 0;
	xp_span_t *sp = NULL;
	xp_card_t *cp = NULL;
	psw_t flags = 0;

	if (!xp)
		return;
	spin_lock_irqsave(&xp->lock, flags);
	{
		if (xp->sdl.config.iftype != arg->iftype) {
			xp->sdl.config.iftype = arg->iftype;
			chan_reconfig = 1;
		}
		switch (arg->iftype) {
		case SDL_TYPE_DS0A:
			xp->sdl.config.ifrate = 56000;
			break;
		case SDL_TYPE_DS0:
			xp->sdl.config.ifrate = 64000;
			break;
		case SDL_TYPE_T1:
			xp->sdl.config.ifrate = 1544000;
			break;
		case SDL_TYPE_E1:
			xp->sdl.config.ifrate = 2048000;
			break;
		}
		if (xp->sdl.config.ifrate != arg->ifrate) {
			xp->sdl.config.ifrate = arg->ifrate;
			chan_reconfig = 1;
		}
		if (xp->sdl.config.ifmode != arg->ifmode) {
			xp->sdl.config.ifmode = arg->ifmode;
			chan_reconfig = 1;
		}
		if ((sp = xp->sp)) {
			int slot;

			if (sp->ifgcrc != arg->ifgcrc) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifgcrc = arg->ifgcrc;
				sp->ifgcrc = arg->ifgcrc;
				span_reconfig = 1;
			}
			if (sp->ifgmode != arg->ifgmode) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifgmode = arg->ifgmode;
				sp->ifgmode = arg->ifgmode;
				span_reconfig = 1;
			}
			if (sp->ifclock != arg->ifclock) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifclock = arg->ifclock;
				sp->ifclock = arg->ifclock;
				span_reconfig = 1;
			}
			if (sp->ifcoding != arg->ifcoding) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifcoding =
						    arg->ifcoding;
				sp->ifcoding = arg->ifcoding;
				span_reconfig = 1;
			}
			if (sp->ifframing != arg->ifframing) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifframing =
						    arg->ifframing;
				sp->ifframing = arg->ifframing;
				span_reconfig = 1;
			}
			if (sp->iftxlevel != arg->iftxlevel) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.iftxlevel =
						    arg->iftxlevel;
				sp->iftxlevel = arg->iftxlevel;
				span_reconfig = 1;
			}
			if ((cp = sp->cp)) {
				int src, span, slot;

				for (src = 0; src < SDL_SYNCS; src++) {
					if (cp->ifsyncsrc[src] != arg->ifsyncsrc[src]) {
						for (span = 0; span < 4; span++)
							if (cp->spans[span])
								for (slot = 0; slot < 32; slot++)
									if (cp->spans[span]->
									    slots[slot])
										cp->spans[span]->
										    slots[slot]->
										    sdl.config.
										    ifsyncsrc[src] =
										    arg->
										    ifsyncsrc[src];
						cp->ifsyncsrc[src] = arg->ifsyncsrc[src];
						card_reconfig = 1;
					}
				}
			}
		}
		if (xp && chan_reconfig && xp->sdl.config.ifflags & SDL_IF_UP) {
			if (sp && cp) {
				if (xp->chan) {
					switch (cp->ifgtype) {
					case SDL_GTYPE_E1:
						xp->slot = xp_e1_chan_map[xp->chan - 1];
						break;
					case SDL_GTYPE_T1:
						xp->slot = xp_t1_chan_map[xp->chan - 1];
						break;
					}
				} else {
					switch (cp->ifgtype) {
					case SDL_GTYPE_E1:
						xp->slot = 0;
						break;
					case SDL_GTYPE_T1:
						xp->slot = 0;
						break;
					}
				}
			}
		}
		if (sp && span_reconfig && sp->ifflags & SDL_IF_UP) {
			/* need to bring up span */
			int offset;
			int span = sp->span;
			int base = span << 8;
			uint8_t ccr1 = 0, tcr1 = 0;
			unsigned long timeout;

			if (cp) {
				switch (cp->ifgtype) {
				case SDL_GTYPE_E1:
				{
					printd(("E400P-SS7: performing reconfiguration of E1 span %d\n", span));
					/* Tell ISR to re-evaluate the sync source */
					cp->eval_syncsrc = 1;
					cp->xlb[CTLREG] = (E1DIV);
					cp->xlb[LEDREG] = 0xff;
					/* zero all span registers */
					for (offset = 0; offset < 192; offset++)
						cp->xlb[base + offset] = 0x00;
					/* Set up for interleaved serial bus operation, byte mode */
					if (span == 0)
						cp->xlb[base + 0xb5] = 0x09;
					else
						cp->xlb[base + 0xb5] = 0x08;
					cp->xlb[base + 0x1a] = 0x04;	/* CCR2: set LOTCMC */
					for (offset = 0; offset <= 8; offset++)
						cp->xlb[base + offset] = 0x00;
					for (offset = 0x10; offset <= 0x4f; offset++)
						if (offset != 0x1a)
							cp->xlb[base + offset] = 0x00;
					cp->xlb[base + 0x10] = 0x20;	/* RCR1: Rsync as input */
					cp->xlb[base + 0x11] = 0x06;	/* RCR2: Sysclk = 2.048 Mhz 
									 */
					cp->xlb[base + 0x12] = 0x09;	/* TCR1: TSiS mode */
					tcr1 = 0x09;	/* TCR1: TSiS mode */
					switch (sp->ifframing) {
					default:
					case SDL_FRAMING_CCS:
						ccr1 |= 0x08;
						break;
					case SDL_FRAMING_CAS:	/* does this mean DS0A? */
						tcr1 |= 0x20;
						break;
					}
					switch (sp->ifcoding) {
					default:
					case SDL_CODING_HDB3:
						ccr1 |= 0x44;
						break;
					case SDL_CODING_AMI:
						ccr1 |= 0x00;
						break;
					}
					switch (sp->ifgcrc) {
					case SDL_GCRC_CRC4:
						ccr1 |= 0x11;
						break;
					default:
						ccr1 |= 0x00;
						break;
					}
					cp->xlb[base + 0x12] = tcr1;
					cp->xlb[base + 0x14] = ccr1;
					cp->xlb[base + 0x18] = 0x20;	/* 120 Ohm, Normal */
					cp->xlb[base + 0x1b] = 0x8a;	/* CRC3: LIRST & TSCLKM */
					cp->xlb[base + 0x20] = 0x1b;	/* TAFR */
					cp->xlb[base + 0x21] = 0x5f;	/* TNAFR */
					cp->xlb[base + 0x40] = 0x0b;	/* TSR1 */
					for (offset = 0x41; offset <= 0x4f; offset++)
						cp->xlb[base + offset] = 0x55;
					for (offset = 0x22; offset <= 0x25; offset++)
						cp->xlb[base + offset] = 0xff;
					timeout = jiffies + 100 * HZ / 1000;
					spin_unlock_irqrestore(&xp->lock, flags);
					while (jiffies < timeout) ;
					spin_lock_irqsave(&xp->lock, flags);
					cp->xlb[base + 0x1b] = 0x9a;	/* CRC3: set ESR as well */
					cp->xlb[base + 0x1b] = 0x82;	/* CRC3: TSCLKM only */
					sp->ifflags |=
					    (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
					/* enable interrupts */
					cp->xlb[CTLREG] = (INTENA | E1DIV);
					break;
				}
				case SDL_GTYPE_T1:
				{
					int byte, val, c;
					unsigned short mask = 0;

					printd(("T400P-SS7: performing reconfiguration of T1 span %d\n", span));
					/* Tell ISR to re-evaluate the sync source */
					cp->eval_syncsrc = 1;
					cp->xlb[CTLREG] = 0;
					cp->xlb[LEDREG] = 0xff;
					for (offset = 0; offset < 160; offset++)
						cp->xlb[base + offset] = 0x00;
					/* Set up for interleaved serial bus operation, byte mode */
					if (span == 0)
						cp->xlb[base + 0x94] = 0x09;
					else
						cp->xlb[base + 0x94] = 0x08;
					cp->xlb[base + 0x2b] = 0x08;	/* Full-on sync required
									   (RCR1) */
					cp->xlb[base + 0x2c] = 0x08;	/* RSYNC is an input (RCR2) 
									 */
					cp->xlb[base + 0x35] = 0x10;	/* RBS enable (TCR1) */
					cp->xlb[base + 0x36] = 0x04;	/* TSYNC to be output
									   (TCR2) */
					cp->xlb[base + 0x37] = 0x9c;	/* Tx & Rx Elastic stor,
									   sysclk(s) = 2.048 mhz,
									   loopback controls (CCR1) 
									 */
					cp->xlb[base + 0x12] = 0x22;	/* Set up received loopup
									   and loopdown codes */
					cp->xlb[base + 0x14] = 0x80;
					cp->xlb[base + 0x15] = 0x80;
					/* Enable F bits pattern */
					switch (sp->ifframing) {
					default:
					case SDL_FRAMING_SF:
						val = 0x20;
						break;
					case SDL_FRAMING_ESF:
						val = 0x88;
						break;
					}
					switch (sp->ifcoding) {
					default:
					case SDL_CODING_AMI:
						break;
					case SDL_CODING_B8ZS:
						val |= 0x44;
						break;
					}
					cp->xlb[base + 0x38] = val;
					if (sp->ifcoding != SDL_CODING_B8ZS)
						cp->xlb[base + 0x7e] = 0x1c;	/* Set FDL register 
										   to 0x1c */
					cp->xlb[base + 0x7c] = sp->iftxlevel << 5;	/* LBO */
					cp->xlb[base + 0x0a] = 0x80;	/* LIRST to reset line
									   interface */
					timeout = jiffies + 100 * HZ / 1000;
					spin_unlock_irqrestore(&xp->lock, flags);
					while (jiffies < timeout) ;
					spin_lock_irqsave(&xp->lock, flags);
					cp->xlb[base + 0x0a] = 0x30;	/* LIRST bask to normal,
									   Resetting elastic
									   buffers */
					sp->ifflags |=
					    (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
					/* enable interrupts */
					cp->xlb[CTLREG] = (INTENA);
					/* establish which channels are clear channel */
					for (c = 0; c < 24; c++) {
						byte = c >> 3;
						if (!cp->spans[span]->slots[xp_t1_chan_map[c]]
						    || cp->spans[span]->slots[xp_t1_chan_map[c]]->
						    sdl.config.iftype != SDL_TYPE_DS0A)
							mask |= 1 << (c % 8);
						if ((c % 8) == 7)
							cp->xlb[base + 0x39 + byte] = mask;
					}
					break;
				}
				default:
					swerr();
					break;
				}
			}
		}
		if (cp && card_reconfig && cp->ifflags & SDL_IF_UP) {
			cp->eval_syncsrc = 1;
		}
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return;
}

/* 
 *  SDL_IOCGOPTIONS:    lmi_option_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		lmi_option_t *arg = (lmi_option_t *) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->option;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCSOPTIONS:    lmi_option_t
 *  -----------------------------------
 */
STATIC int
sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->option = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCGCONFIG:     sdl_config_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		xp_span_t *sp;
		psw_t flags = 0;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		bzero(arg, sizeof(*arg));
		spin_lock_irqsave(&xp->lock, flags);
		{
			arg->ifflags = xp->sdl.config.ifflags;
			arg->iftype = xp->sdl.config.iftype;
			arg->ifrate = xp->sdl.config.ifrate;
			arg->ifmode = xp->sdl.config.ifmode;
			if ((sp = xp->sp)) {
				xp_card_t *cp;

				arg->ifgtype = sp->ifgtype;
				arg->ifgrate = sp->ifgrate;
				arg->ifgmode = sp->ifgmode;
				arg->ifgcrc = sp->ifgcrc;
				arg->ifclock = sp->ifclock;
				arg->ifcoding = sp->ifcoding;
				arg->ifframing = sp->ifframing;
				arg->ifalarms = sp->ifalarms;
				arg->ifrxlevel = sp->ifrxlevel;
				arg->iftxlevel = sp->iftxlevel;
				if ((cp = sp->cp)) {
					int src;

					for (src = 0; src < SDL_SYNCS; src++)
						arg->ifsyncsrc[src] = cp->ifsyncsrc[src];
					arg->ifsync = cp->ifsync;
				}
			}
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCSCONFIG:     sdl_config_t
 *  -----------------------------------
 */
STATIC int
sdl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret;
		xp_t *xp = PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((ret = sdl_test_config(xp, arg)))
			return (ret);
		sdl_commit_config(xp, arg);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCTCONFIG:     sdl_config_t
 *  -----------------------------------
 */
STATIC int
sdl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		return sdl_test_config(xp, arg);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCCONFIG:     sdl_config_t
 *  -----------------------------------
 */
STATIC int
sdl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		sdl_commit_config(xp, arg);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCGSTATEM:     sdl_statem_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdl.statem;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCMRESET:     sdl_statem_t
 *  -----------------------------------
 */
STATIC int
sdl_ioccmreset(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;

	(void) xp;
	(void) arg;
	fixme(("FIXME: Support master reset\n"));
	return (-EOPNOTSUPP);
}

/* 
 *  SDL_IOCGSTATSP:     sdl_stats_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdl.statsp;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCSSTATSP:     sdl_stats_t
 *  -----------------------------------
 */
STATIC int
sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		fixme(("FIXME: check these settings\n"));
		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sdl.statsp = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCGSTATS:      sdl_stats_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdl.stats;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCSTATS:      sdl_stats_t
 *  -----------------------------------
 */
STATIC int
sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	spin_lock_irqsave(&xp->lock, flags);
	{
		bzero(&xp->sdl.stats, sizeof(xp->sdl.stats));
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (0);
}

/* 
 *  SDL_IOCGNOTIFY:     sdl_notify_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sdl.notify;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCSNOTIFY:     sdl_notify_t
 *  -----------------------------------
 */
STATIC int
sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sdl.notify.events |= arg->events;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCNOTIFY:     sdl_notify_t
 *  -----------------------------------
 */
STATIC int
sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		xp_t *xp = PRIV(q);
		psw_t flags = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sdl.notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCDISCTX:     
 *  -----------------------------------
 */
STATIC int
sdl_ioccdisctx(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	spin_lock_irqsave(&xp->lock, flags);
	{
		xp->sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (0);
}

/* 
 *  SDL_IOCCONNTX:      
 *  -----------------------------------
 */
STATIC int
sdl_ioccconntx(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	psw_t flags = 0;

	(void) mp;
	spin_lock_irqsave(&xp->lock, flags);
	{
		xp->sdl.config.ifflags |= SDL_IF_TX_RUNNING;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (0);
}

/*
 *  =========================================================================
 *
 *  X400P Interrupt Service Routine
 *
 *  =========================================================================
 *  We break this out into E1 and T1 versions for speed.  No need to check
 *  card type in the ISR when it is static for the board.  That is: boards do
 *  not change type from E1 to T1 or visa versa.
 */

/*
 *  X400P Tasklet
 *  -----------------------------------
 *  This tasklet is scheduled before the ISR returns to feed the next buffer
 *  of data into the write buffer and read the buffer of data from the read
 *  buffer.  This will run the soft-HDLC on each channel for 8 more bytes, or
 *  if full span will run the soft-HDLC for 192 bytes (T1) or 256 bytes (E1).
 */
/*
 *  E1C Process
 *  -----------------------------------
 *  Process a channelized E1 span, one channel at a time.
 */
STATIC void
xp_e1c_process(xp_span_t * sp, unsigned char *wspan, unsigned char *rspan, unsigned char *wend,
	       unsigned char *rend)
{
	int slot;

	/* one slot at a time, 8 frames */
	for (slot = 1; slot < 32; slot++) {
		xp_t *xp;

		if ((xp = sp->slots[slot]) && (xp->sdl.config.ifflags & SDL_IF_UP)) {
			size_t coff = slot << 2;
			sdt_stats_t *stats = &xp->sdt.stats;

			if (xp->sdl.config.iftype != SDL_TYPE_DS0A) {
				if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
					xp_ds0_tx_block(xp, wspan + coff, wend, stats);
					xp->sdl.stats.tx_octets += 8;
				}
				if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
					xp_ds0_rx_block(xp, rspan + coff, rend, stats);
					xp->sdl.stats.rx_octets += 8;
				}
			}
		}
	}
}

/*
 *  E1 Process
 *  -----------------------------------
 *  Process an entire E1 span.  This is a High-Speed Link.  All channels are
 *  concatenated to form a single link.
 */
STATIC void
xp_e1_process(xp_span_t * sp, unsigned char *wspan, unsigned char *rspan, unsigned char *wend,
	      unsigned char *rend)
{
	xp_t *xp;

	/* entire span, one frame at a time */
	if ((xp = sp->slots[1])) {
		sdt_stats_t *stats = &xp->sdt.stats;

		if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
			xp_e1_tx_block(xp, wspan, wend, stats);
			xp->sdl.stats.tx_octets += 8 * 31;
		}
		if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
			xp_e1_rx_block(xp, rspan, rend, stats);
			xp->sdl.stats.rx_octets += 8 * 31;
		}
	}
}

/*
 *  E1 Card Tasklet
 *  -----------------------------------
 *  Process an entire E1 card.
 */
STATIC void
xp_e1_card_tasklet(unsigned long data)
{
	struct xp_card *cp = (struct xp_card *) data;

	if (cp->uebno != cp->lebno) {
		size_t boff = cp->uebno << 10;
		unsigned char *wbeg = (unsigned char *) cp->wbuf + boff;
		unsigned char *wend = wbeg + 1024;
		unsigned char *rbeg = (unsigned char *) cp->rbuf + boff;
		unsigned char *rend = rbeg + 1024;
		int span;

		for (span = 0; span < 4; span++) {
			struct xp_span *sp;

			if ((sp = cp->spans[span]) && (sp->ifflags & SDL_IF_UP)) {
				int soff = span_to_byte(span);

				if (sp->iftype == SDL_TYPE_E1)
					xp_e1_process(sp, wbeg + soff, rbeg + soff, wend, rend);
				else
					xp_e1c_process(sp, wbeg + soff, rbeg + soff, wend, rend);
			}
		}
		if ((cp->uebno = (cp->uebno + 1) & (X400P_EBUFNO - 1)) != cp->lebno)
			tasklet_schedule(&cp->tasklet);
	}
}

/*
 *  T1C Process
 *  -----------------------------------
 *  Process a channelized T1 span, one channel at a time.  Each channel can be
 *  either a clear channel or a DS0A channel.
 */
STATIC void
xp_t1c_process(xp_span_t * sp, unsigned char *wspan, unsigned char *rspan, unsigned char *wend,
	       unsigned char *rend)
{
	int slot;

	/* one slot at a time, 8 frames */
	for (slot = 1; slot < 32; slot++) {
		if (slot & 0x3) {
			xp_t *xp;

			if ((xp = sp->slots[slot]) && (xp->sdl.config.ifflags & SDL_IF_UP)) {
				size_t coff = slot << 2;
				sdt_stats_t *stats = &xp->sdt.stats;

				if (xp->sdl.config.iftype != SDL_TYPE_DS0A) {
					if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp_ds0_tx_block(xp, wspan + coff, wend, stats);
						xp->sdl.stats.tx_octets += 8;
					}
					if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp_ds0_rx_block(xp, rspan + coff, rend, stats);
						xp->sdl.stats.rx_octets += 8;
					}
				} else {
					if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp_ds0a_tx_block(xp, wspan + coff, rend, stats);
						xp->sdl.stats.tx_octets += 8;
					}
					if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp_ds0a_rx_block(xp, rspan + coff, rend, stats);
						xp->sdl.stats.rx_octets += 8;
					}
				}
			}
		}
	}
}

/*
 *  T1 Process
 *  -----------------------------------
 *  Process an entire T1 span.  This is a High-Speed Link.  All channels are
 *  concatenated to form a single link.
 */
STATIC void
xp_t1_process(xp_span_t * sp, unsigned char *wspan, unsigned char *rspan, unsigned char *wend,
	      unsigned char *rend)
{
	xp_t *xp;

	/* entire span, one frame at a time */
	if ((xp = sp->slots[1])) {
		sdt_stats_t *stats = &xp->sdt.stats;

		if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
			xp_t1_tx_block(xp, wspan, wend, stats);
			xp->sdl.stats.tx_octets += 8 * 24;
		}
		if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
			xp_t1_rx_block(xp, rspan, rend, stats);
			xp->sdl.stats.rx_octets += 8 * 24;
		}
	}
}

/*
 *  T1 Card Tasklet
 *  -----------------------------------
 *  Process an entire T1 card.
 */
STATIC void
xp_t1_card_tasklet(unsigned long data)
{
	struct xp_card *cp = (struct xp_card *) data;

	if (cp->uebno != cp->lebno) {
		size_t boff = cp->uebno << 10;
		unsigned char *wbeg = (unsigned char *) cp->wbuf + boff;
		unsigned char *wend = wbeg + 1024;
		unsigned char *rbeg = (unsigned char *) cp->rbuf + boff;
		unsigned char *rend = rbeg + 1024;
		int span;

		for (span = 0; span < 4; span++) {
			struct xp_span *sp;

			if ((sp = cp->spans[span]) && (sp->ifflags & SDL_IF_UP)) {
				int soff = span_to_byte(span);

				if (sp->iftype == SDL_TYPE_T1)
					xp_t1_process(sp, wbeg + soff, rbeg + soff, wend, rend);
				else
					xp_t1c_process(sp, wbeg + soff, rbeg + soff, wend, rend);
			}
		}
		if ((cp->uebno = (cp->uebno + 1) & (X400P_EBUFNO - 1)) != cp->lebno)
			tasklet_schedule(&cp->tasklet);
	}
}

/*
 *  X400P Overflow
 *  -----------------------------------
 *  I know that this is rather like kicking them when they are down, we are
 *  doing stats in the ISR when takslets don't have enough time to run, but we
 *  are already in dire trouble if this is happening anyway.  It should not
 *  take too much time to peg these counts.
 */
STATIC void
xp_overflow(xp_card_t * cp)
{
	int span;

	printd(("%s: card %d elastic buffer overrun!\n", __FUNCTION__, cp->card));
	for (span = 0; span < 4; span++) {
		xp_t *xp;
		xp_span_t *sp;

		if ((sp = cp->spans[span]) && sp->ifflags & SDL_IF_UP) {
			switch (sp->iftype) {
			case SDL_TYPE_DS0:
			case SDL_TYPE_DS0A:
			{
				int slot;

				for (slot = 1; slot < 32; slot++) {
					if ((xp = sp->slots[slot])) {
						if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
							xp->sdl.stats.tx_underruns += 8;
							xp->sdt.stats.tx_underruns += 8;
						}
						if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
							xp->sdl.stats.rx_overruns += 8;
							xp->sdt.stats.rx_overruns += 8;
						}
					}
				}
				break;
			}
			case SDL_TYPE_T1:
				if ((xp = sp->slots[1])) {
					if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp->sdl.stats.tx_underruns += 8 * 24;
						xp->sdt.stats.tx_underruns += 8 * 24;
					}
					if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp->sdl.stats.rx_overruns += 8 * 24;
						xp->sdt.stats.rx_overruns += 8 * 24;
					}
				}
				break;
			case SDL_TYPE_E1:
				if ((xp = sp->slots[1])) {
					if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp->sdl.stats.tx_underruns += 8 * 31;
						xp->sdt.stats.tx_underruns += 8 * 31;
					}
					if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp->sdl.stats.rx_overruns += 8 * 31;
						xp->sdt.stats.rx_overruns += 8 * 31;
					}
				}
				break;
			default:
			{
				static unsigned long throttle = 0;

				if (throttle + 10 <= jiffies)
					break;
				throttle = jiffies;
				swerr();
				break;
			}
			}
		}
	}
}

/*
 *  E400P-SS7 Interrupt Service Routine
 *  -----------------------------------
 */
STATIC irqreturn_t
xp_e1_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct xp_card *cp = (struct xp_card *) dev_id;

	/* active interrupt (otherwise spurious or shared) */
	if (cp->xlb[STAREG] & INTACTIVE) {
		xp_span_t *sp;
		int span, lebno;

		cp->xlb[CTLREG] = (INTENA | OUTBIT | INTACK | E1DIV);
		if ((lebno = (cp->lebno + 1) & (X400P_EBUFNO - 1)) != cp->uebno) {
			/* write/read burst */
			unsigned int offset = lebno << 8;
			register int word, slot;
			uint32_t *wbuf = cp->wbuf + offset;
			uint32_t *rbuf = cp->rbuf + offset;
			volatile uint32_t *xll = cp->xll;

			for (word = 0; word < 256; word += 32)
				for (slot = word + 1; slot < word + 32; slot++)
					xll[slot] = wbuf[slot];
			for (word = 0; word < 256; word += 32)
				for (slot = word + 1; slot < word + 32; slot++)
					rbuf[slot] = xll[slot];
			cp->lebno = lebno;
			tasklet_schedule(&cp->tasklet);
		} else
			xp_overflow(cp);
		for (span = 0; span < 4; span++) {
			if ((sp = cp->spans[span]) && (sp->ifflags & SDL_IF_UP)) {
				int base = span << 8;

				if (sp->recovertime && !--sp->recovertime) {
					printd(("%s: alarm recovery complete\n", __FUNCTION__));
					sp->ifalarms &= ~SDL_ALARM_REC;
					cp->xlb[base + 0x21] = 0x5f;	/* turn off yellow */
					cp->eval_syncsrc = 1;
				}
			}
		}
		/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
		if ((span = ((cp->frame >> 3) & 0x3f) - 50) >= 0 && span < 4
		    && (sp = cp->spans[span])
		    && (sp->ifflags & SDL_IF_UP)) {
			int status, alarms = 0, leds = 0, all_leds;
			int base = span << 8;

			cp->xlb[base + 0x06] = 0xff;
			status = cp->xlb[base + 0x06];
			if (status & 0x09)
				alarms |= SDL_ALARM_RED;
			if (status & 0x02)
				alarms |= SDL_ALARM_BLU;
			if (alarms) {
				if (!(sp->ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just begun */
					cp->xlb[base + 0x21] = 0x7f;	/* set yellow alarm */
					cp->eval_syncsrc = 1;
				}
			} else {
				if ((sp->ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just ended */
					alarms |= SDL_ALARM_REC;
					sp->recovertime = SDT_X400P_ALARM_SETTLE_TIME;
				}
			}
			if (status & 0x04)
				alarms |= SDL_ALARM_YEL;
			sp->ifalarms = alarms;
			/* adjust leds */
			if (alarms & SDL_ALARM_RED)
				leds |= LEDRED;
			else if (alarms & SDL_ALARM_YEL)
				leds |= LEDYEL;
			else
				leds |= LEDGRN;
			all_leds = cp->leds;
			all_leds &= ~(LEDYEL << (span << 1));
			all_leds |= leds << (span << 1);
			if (cp->leds != all_leds) {
				cp->xlb[LEDREG] = all_leds;
				cp->leds = all_leds;
			}
		}
		// if (!(cp->frame % 8000))
		if (!(cp->frame & 0x1fff)) {	/* 1.024 seconds */
			for (span = 0; span < 4; span++)
				if ((sp = cp->spans[span]) && (sp->ifflags & SDL_IF_UP)) {
					int base = span << 8;

					// printd(("%s: accumulating bipolar violations\n",
					// __FUNCTION__));
					sp->ifbpv +=
					    cp->xlb[base + 0x01] + (cp->xlb[base + 0x00] << 8);
				}
		}
		if (xchg((int *) &cp->eval_syncsrc, 0)) {
			int src, syncsrc = 0;

			for (src = 0; src < SDL_SYNCS; src++) {
				if (cp->ifsyncsrc[src] && cp->spans[syncsrc - 1]
				    && (cp->spans[syncsrc - 1]->ifflags & SDL_IF_UP)
				    && !(cp->spans[syncsrc - 1]->ifclock == SDL_CLOCK_LOOP)
				    && !(cp->spans[syncsrc - 1]->
					 ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					syncsrc = cp->ifsyncsrc[src];
					break;
				}
			}
			if (cp->ifsync != syncsrc) {
				cp->xlb[SYNREG] = syncsrc;
				cp->ifsync = syncsrc;
			}
		}
		cp->frame += 8;
		cp->xlb[CTLREG] = (INTENA | E1DIV);
		return (irqreturn_t) (IRQ_HANDLED);
	}
	return (irqreturn_t) (IRQ_NONE);
}

/*
 *  T400P-SS7 Interrupt Service Routine
 *  -----------------------------------
 */
STATIC irqreturn_t
xp_t1_interrupt(int irq, void *dev_id, struct pt_regs * regs)
{
	struct xp_card *cp = (struct xp_card *) dev_id;

	/* active interrupt (otherwise spurious or shared) */
	if (cp->xlb[STAREG] & INTACTIVE) {
		xp_span_t *sp;
		int span, lebno;

		cp->xlb[CTLREG] = (INTENA | OUTBIT | INTACK);
		if ((lebno = (cp->lebno + 1) & (X400P_EBUFNO - 1)) != cp->uebno) {
			/* write/read burst */
			unsigned int offset = lebno << 8;
			register int word, slot;
			uint32_t *wbuf = cp->wbuf + offset;
			uint32_t *rbuf = cp->rbuf + offset;
			volatile uint32_t *xll = cp->xll;

			for (word = 0; word < 256; word += 32)
				for (slot = word + 1; slot < word + 32; slot++)
					if (slot & 0x3)
						xll[slot] = wbuf[slot];
			for (word = 0; word < 256; word += 32)
				for (slot = word + 1; slot < word + 32; slot++)
					if (slot & 0x3)
						rbuf[slot] = xll[slot];
			cp->lebno = lebno;
			tasklet_schedule(&cp->tasklet);
		} else
			xp_overflow(cp);
		for (span = 0; span < 4; span++) {
			if ((sp = cp->spans[span]) && (sp->ifflags & SDL_IF_UP)) {
				int base = span << 8;

				if (sp->recovertime && !--sp->recovertime) {
					/* alarm recovery complete */
					sp->ifalarms &= ~SDL_ALARM_REC;
					cp->xlb[base + 0x35] = 0x10;	/* turn off yellow */
					cp->eval_syncsrc = 1;
				}
			}
		}
		/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
		if ((span = ((cp->frame >> 3) & 0x3f) - 50) >= 0 && span < 4
		    && (sp = cp->spans[span])
		    && (sp->ifflags & SDL_IF_UP)) {
			int status, alarms = 0, leds = 0, all_leds;
			int base = span << 8;

			sp->ifrxlevel = cp->xlb[base + RIR2] >> 6;
			cp->xlb[base + 0x20] = 0xff;
			status = cp->xlb[base + 0x20];
			/* loop up code */
			if ((status & 0x80) && !(sp->ifgmode & SDL_GMODE_LOC_LB)) {
				if (sp->loopcnt++ > 80 && !(sp->ifgmode & SDL_GMODE_REM_LB)) {
					cp->xlb[base + 0x1e] = 0x00;	/* no local loop */
					cp->xlb[base + 0x40] = 0x40;	/* remote loop */
					sp->ifgmode |= SDL_GMODE_REM_LB;
				}
			} else
				sp->loopcnt = 0;
			if ((status & 0x40) && !(sp->ifgmode & SDL_GMODE_LOC_LB)) {
				/* loop down code */
				if (sp->loopcnt++ > 80 && (sp->ifgmode & SDL_GMODE_REM_LB)) {
					cp->xlb[base + 0x1e] = 0x00;	/* no local loop */
					cp->xlb[base + 0x40] = 0x00;	/* no remote loop */
					sp->ifgmode &= ~SDL_GMODE_REM_LB;
				}
			} else
				sp->loopcnt = 0;
			if (status & 0x03)
				alarms |= SDL_ALARM_RED;
			if (status & 0x08)
				alarms |= SDL_ALARM_BLU;
			if (alarms) {
				if (!(sp->ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just begun */
					cp->xlb[base + 0x35] = 0x11;	/* set yellow alarm */
					cp->eval_syncsrc = 1;
				}
			} else {
				if ((sp->ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just ended */
					alarms |= SDL_ALARM_REC;
					sp->recovertime = SDT_X400P_ALARM_SETTLE_TIME;
				}
			}
			if (status & 0x04)
				alarms |= SDL_ALARM_YEL;
			sp->ifalarms = alarms;
			/* adjust leds */
			if (alarms & SDL_ALARM_RED)
				leds |= LEDRED;
			else if (alarms & SDL_ALARM_YEL)
				leds |= LEDYEL;
			else
				leds |= LEDGRN;
			all_leds = cp->leds;
			all_leds &= ~(LEDYEL << (span << 1));
			all_leds |= leds << (span << 1);
			if (cp->leds != all_leds) {
				cp->xlb[LEDREG] = all_leds;
				cp->leds = all_leds;
			}
		}
		// if (!(cp->frame % 8000)) {
		if (!(cp->frame & 0x1fff)) {	/* 1.024 seconds */
			for (span = 0; span < 4; span++)
				if ((sp = cp->spans[span]) && (sp->ifflags & SDL_IF_UP)) {
					int base = span << 8;

					// printd(("%s: accumulating bipolar violations\n",
					// __FUNCTION__));
					sp->ifbpv +=
					    cp->xlb[base + 0x24] + (cp->xlb[base + 0x23] << 8);
				}
		}
		if (xchg((int *) &cp->eval_syncsrc, 0)) {
			int src, syncsrc = 0;

			for (src = 0; src < SDL_SYNCS; src++) {
				if (cp->ifsyncsrc[src] && cp->spans[syncsrc - 1]
				    && (cp->spans[syncsrc - 1]->ifflags & SDL_IF_UP)
				    && !(cp->spans[syncsrc - 1]->ifclock == SDL_CLOCK_LOOP)
				    && !(cp->spans[syncsrc - 1]->
					 ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					syncsrc = cp->ifsyncsrc[src];
					break;
				}
			}
			if (cp->ifsync != syncsrc) {
				cp->xlb[SYNREG] = syncsrc;
				cp->ifsync = syncsrc;
			}
		}
		cp->frame += 8;
		cp->xlb[CTLREG] = (INTENA);
		return (irqreturn_t) (IRQ_HANDLED);
	}
	return (irqreturn_t) (IRQ_NONE);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_IOCTL Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
xp_w_ioctl(queue_t *q, mblk_t *mp)
{
	xp_t *xp = PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);

	(void) nr;
	switch (type) {
	case __SID:
	{
		switch (cmd) {
		default:
			ptrace(("X400P-SS7: ERROR: Unknown IOCTL %d\n", cmd));
		case I_STR:
		case I_LINK:
		case I_PLINK:
		case I_UNLINK:
		case I_PUNLINK:
			rare();
			(void) lp;
			ret = -EINVAL;
			break;
		}
		ptrace(("X400P-SS7: ERROR: Unsupported STREAMS ioctl\n"));
		ret = -EOPNOTSUPP;
		break;
	}
	case SDT_IOC_MAGIC:
	{
		if (count < size || PRIV(q)->state == LMI_UNATTACHED) {
			ret = -EINVAL;
			break;
		}
		switch (cmd) {
		case SDT_IOCGOPTIONS:	/* lmi_option_t */
			ret = sdt_iocgoptions(q, mp);
			break;
		case SDT_IOCSOPTIONS:	/* lmi_option_t */
			ret = sdt_iocsoptions(q, mp);
			break;
		case SDT_IOCGCONFIG:	/* sdt_config_t */
			ret = sdt_iocgconfig(q, mp);
			break;
		case SDT_IOCSCONFIG:	/* sdt_config_t */
			ret = sdt_iocsconfig(q, mp);
			break;
		case SDT_IOCTCONFIG:	/* sdt_config_t */
			ret = sdt_ioctconfig(q, mp);
			break;
		case SDT_IOCCCONFIG:	/* sdt_config_t */
			ret = sdt_ioccconfig(q, mp);
			break;
		case SDT_IOCGSTATEM:	/* sdt_statem_t */
			ret = sdt_iocgstatem(q, mp);
			break;
		case SDT_IOCCMRESET:	/* sdt_statem_t */
			ret = sdt_ioccmreset(q, mp);
			break;
		case SDT_IOCGSTATSP:	/* lmi_sta_t */
			ret = sdt_iocgstatsp(q, mp);
			break;
		case SDT_IOCSSTATSP:	/* lmi_sta_t */
			ret = sdt_iocsstatsp(q, mp);
			break;
		case SDT_IOCGSTATS:	/* sdt_stats_t */
			ret = sdt_iocgstats(q, mp);
			break;
		case SDT_IOCCSTATS:	/* sdt_stats_t */
			ret = sdt_ioccstats(q, mp);
			break;
		case SDT_IOCGNOTIFY:	/* sdt_notify_t */
			ret = sdt_iocgnotify(q, mp);
			break;
		case SDT_IOCSNOTIFY:	/* sdt_notify_t */
			ret = sdt_iocsnotify(q, mp);
			break;
		case SDT_IOCCNOTIFY:	/* sdt_notify_t */
			ret = sdt_ioccnotify(q, mp);
			break;
		default:
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || xp->state == LMI_UNATTACHED) {
			ptrace(("X400P-SS7: ERROR: ioctl count = %d, size = %d, state = %d\n",
				count, size, xp->state));
			ret = -EINVAL;
			break;
		}
		switch (cmd) {
		case SDL_IOCGOPTIONS:	/* lmi_option_t */
			ret = sdl_iocgoptions(q, mp);
			break;
		case SDL_IOCSOPTIONS:	/* lmi_option_t */
			ret = sdl_iocsoptions(q, mp);
			break;
		case SDL_IOCGCONFIG:	/* sdl_config_t */
			ret = sdl_iocgconfig(q, mp);
			break;
		case SDL_IOCSCONFIG:	/* sdl_config_t */
			ret = sdl_iocsconfig(q, mp);
			break;
		case SDL_IOCTCONFIG:	/* sdl_config_t */
			ret = sdl_ioctconfig(q, mp);
			break;
		case SDL_IOCCCONFIG:	/* sdl_config_t */
			ret = sdl_ioccconfig(q, mp);
			break;
		case SDL_IOCGSTATEM:	/* sdl_statem_t */
			ret = sdl_iocgstatem(q, mp);
			break;
		case SDL_IOCCMRESET:	/* sdl_statem_t */
			ret = sdl_ioccmreset(q, mp);
			break;
		case SDL_IOCGSTATSP:	/* sdl_stats_t */
			ret = sdl_iocgstatsp(q, mp);
			break;
		case SDL_IOCSSTATSP:	/* sdl_stats_t */
			ret = sdl_iocsstatsp(q, mp);
			break;
		case SDL_IOCGSTATS:	/* sdl_stats_t */
			ret = sdl_iocgstats(q, mp);
			break;
		case SDL_IOCCSTATS:	/* sdl_stats_t */
			ret = sdl_ioccstats(q, mp);
			break;
		case SDL_IOCGNOTIFY:	/* sdl_notify_t */
			ret = sdl_iocgnotify(q, mp);
			break;
		case SDL_IOCSNOTIFY:	/* sdl_notify_t */
			ret = sdl_iocsnotify(q, mp);
			break;
		case SDL_IOCCNOTIFY:	/* sdl_notify_t */
			ret = sdl_ioccnotify(q, mp);
			break;
		case SDL_IOCCDISCTX:	/* */
			ret = sdl_ioccdisctx(q, mp);
			break;
		case SDL_IOCCCONNTX:	/* */
			ret = sdl_ioccconntx(q, mp);
			break;
		default:
			ptrace(("X400P-SS7: ERROR: Unsupported SDL ioctl\n"));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		return (QR_PASSALONG);
	}
	if (ret >= 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  M_PROTO, M_PCPROTO Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
xp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	xp_t *xp = PRIV(q);
	ulong oldstate = xp->state;

	switch ((prim = *(ulong *) mp->b_rptr)) {
	case SDT_DAEDT_TRANSMISSION_REQ:
		rtn = sdt_daedt_transmission_req(q, mp);
		break;
	case SDT_DAEDT_START_REQ:
		rtn = sdt_daedt_start_req(q, mp);
		break;
	case SDT_DAEDR_START_REQ:
		rtn = sdt_daedr_start_req(q, mp);
		break;
	case SDT_AERM_START_REQ:
		rtn = sdt_aerm_start_req(q, mp);
		break;
	case SDT_AERM_STOP_REQ:
		rtn = sdt_aerm_stop_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		rtn = sdt_aerm_set_ti_to_tin_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		rtn = sdt_aerm_set_ti_to_tie_req(q, mp);
		break;
	case SDT_SUERM_START_REQ:
		rtn = sdt_suerm_start_req(q, mp);
		break;
	case SDT_SUERM_STOP_REQ:
		rtn = sdt_suerm_stop_req(q, mp);
		break;
	case LMI_INFO_REQ:
		rtn = lmi_info_req(q, mp);
		break;
	case LMI_ATTACH_REQ:
		rtn = lmi_attach_req(q, mp);
		break;
	case LMI_DETACH_REQ:
		rtn = lmi_detach_req(q, mp);
		break;
	case LMI_ENABLE_REQ:
		rtn = lmi_enable_req(q, mp);
		break;
	case LMI_DISABLE_REQ:
		rtn = lmi_disable_req(q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		rtn = lmi_optmgmt_req(q, mp);
		break;
	default:
		/* pass along anything we don't recognize */
		rtn = QR_PASSFLOW;
		break;
	}
	if (rtn < 0)
		xp->state = oldstate;
	return (rtn);
}

/*
 *  M_DATA Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
xp_w_data(queue_t *q, mblk_t *mp)
{
	return xp_send_data(q, mp);
}

/*
 *  M_RSE, M_PCRSE Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
xp_r_pcrse(queue_t *q, mblk_t *mp)
{
	int rtn;

	switch (*(ulong *) mp->b_rptr) {
	case SDT_T8_TIMEOUT:
		rtn = sdt_t8_timeout_ind(q);
		break;
	default:
		rtn = -EFAULT;
		break;
	}
	return (rtn);
}

/*
 *  M_FLUSH Handling
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
xp_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag)
{
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	return (QR_LOOP);
}
STATIC int
xp_w_flush(queue_t *q, mblk_t *mp)
{
	return xp_m_flush(q, mp, FLUSHW);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
xp_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		return xp_r_pcrse(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
xp_w_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return xp_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return xp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return xp_w_proto(q, mp);
	case M_FLUSH:
		return xp_w_flush(q, mp);
	case M_IOCTL:
		return xp_w_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}

#ifdef _DEBUG
STATIC INLINE char *
mname(mblk_t *mp)
{
	char *ret;

	switch (mp->b_datap->db_type) {
	case M_DATA:
		ret = "M_DATA";
		break;
	case M_PROTO:
		ret = "M_PROTO";
		break;
	case M_BREAK:
		ret = "M_BREAK";
		break;
	case M_CTL:
		ret = "M_CTL";
		break;
	case M_DELAY:
		ret = "M_DELAY";
		break;
	case M_IOCTL:
		ret = "M_IOCTL";
		break;
	case M_PASSFP:
		ret = "M_PASSFP";
		break;
	case M_RSE:
		ret = "M_RSE";
		break;
	case M_SETOPTS:
		ret = "M_SETOPTS";
		break;
	case M_SIG:
		ret = "M_SIG";
		break;
	case M_COPYIN:
		ret = "M_COPYIN";
		break;
	case M_COPYOUT:
		ret = "M_COPYOUT";
		break;
	case M_ERROR:
		ret = "M_ERROR";
		break;
	case M_FLUSH:
		ret = "M_FLUSH";
		break;
	case M_HANGUP:
		ret = "M_HANGUP";
		break;
	case M_IOCACK:
		ret = "M_IOCACK";
		break;
	case M_IOCNAK:
		ret = "M_IOCNAK";
		break;
	case M_IOCDATA:
		ret = "M_IOCDATA";
		break;
	case M_PCPROTO:
		ret = "M_PCPROTO";
		break;
	case M_PCRSE:
		ret = "M_PCRSE";
		break;
	case M_PCSIG:
		ret = "M_PCSIG";
		break;
	case M_READ:
		ret = "M_READ";
		break;
	case M_STOP:
		ret = "M_STOP";
		break;
	case M_START:
		ret = "M_START";
		break;
	case M_STARTI:
		ret = "M_STARTI";
		break;
	case M_STOPI:
		ret = "M_STOPI";
		break;
	default:
		ret = "?????";
		break;
	}
	return (ret);
}
#endif

/*
 *  PUTQ Put Routine
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
xp_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0;

	if (mp->b_datap->db_type < QPCTL) {
		// printd(("%s: putting %s on queue\n", q->q_qinfo->qi_minfo->mi_idname,
		// mname(mp)));
		putq(q, mp);
		return (0);
	}
	if (xp_trylockq(q)) {
		do {
			// printd(("%s: processing priority %s\n", q->q_qinfo->qi_minfo->mi_idname,
			// mname(mp)));
			/* Fast Path */
			if ((rtn = proc(q, mp)) == QR_DONE) {
				freemsg(mp);
				break;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				break;
			case QR_STRIP:
				if (mp->b_cont)
					putq(q, mp->b_cont);
			case QR_TRIMMED:
				freeb(mp);
				break;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					break;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					break;
				}
				// ptrace(("%s: ERROR: couldn't pass message\n",
				// q->q_qinfo->qi_minfo->mi_idname));
				rtn = -EOPNOTSUPP;
			default:
				// ptrace(("%s: ERROR: (q dropping) %d\n",
				// q->q_qinfo->qi_minfo->mi_idname,
				// rtn));
				freemsg(mp);
				break;
			case QR_DISABLE:
				putq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					break;
				}
			case -ENOBUFS:
			case -EBUSY:
			case -ENOMEM:
			case -EAGAIN:
				putq(q, mp);
				break;
			}
		} while (0);
		xp_unlockq(q);
	} else {
		rare();
		// printd(("%s: putting %s on queue\n", q->q_qinfo->qi_minfo->mi_idname,
		// mname(mp)));
		putq(q, mp);
	}
	return (rtn);
}

/*
 *  SRVQ Service Routine
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
xp_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0;

	ensure(q, return (-EFAULT));
	if (xp_trylockq(q)) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			// printd(("%s: processing queued %s\n", q->q_qinfo->qi_minfo->mi_idname,
			// mname(mp)));
			/* Fast Path */
			if ((rtn = proc(q, mp)) == QR_DONE) {
				freemsg(mp);
				continue;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
			case QR_STRIP:
				if (mp->b_cont)
					putbq(q, mp->b_cont);
			case QR_TRIMMED:
				freeb(mp);
				continue;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					continue;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					continue;
				}
				// ptrace(("%s: ERROR: couldn't pass message\n",
				// q->q_qinfo->qi_minfo->mi_idname));
				rtn = -EOPNOTSUPP;
			default:
				// ptrace(("%s: ERROR: (q dropping) %d\n",
				// q->q_qinfo->qi_minfo->mi_idname,
				// rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				// ptrace(("%s: ERROR: (q disabling) %d\n",
				// q->q_qinfo->qi_minfo->mi_idname,
				// rtn));
				noenable(q);
				putbq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must have scheduled bufcall */
			case -EBUSY:	/* proc must have failed canput */
			case -ENOMEM:	/* proc must have scheduled bufcall */
			case -EAGAIN:	/* proc must re-enable on some event */
				if (mp->b_datap->db_type < QPCTL) {
					// ptrace(("%s: ERROR: (q stalled) %d\n",
					// q->q_qinfo->qi_minfo->mi_idname, rtn));
					putbq(q, mp);
					break;
				}
				/* 
				 *  Be careful not to put a priority
				 *  message back on the queue.
				 */
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					mp->b_band = 255;
					putq(q, mp);
					break;
				}
				// ptrace(("%s: ERROR: (q dropping) %d\n",
				// q->q_qinfo->qi_minfo->mi_idname,
				// rtn));
				freemsg(mp);
				continue;
			}
			break;
		}
		xp_unlockq(q);
	} else {
		rare();
	}
	return (rtn);
}

STATIC streamscall int
xp_rput(queue_t *q, mblk_t *mp)
{
	return xp_putq(q, mp, &xp_r_prim);
}
STATIC streamscall int
xp_rsrv(queue_t *q)
{
	return xp_srvq(q, &xp_r_prim);
}
STATIC streamscall int
xp_wput(queue_t *q, mblk_t *mp)
{
	return xp_putq(q, mp, &xp_w_prim);
}
STATIC streamscall int
xp_wsrv(queue_t *q)
{
	return xp_srvq(q, &xp_w_prim);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *  Open is called on the first open of a character special device stream
 *  head; close is called on the last close of the same device.
 */
STATIC spinlock_t xp_lock = SPIN_LOCK_UNLOCKED;
STATIC struct xp *xp_list = NULL;
STATIC major_t xp_majors[CMAJORS] = { CMAJOR_0, };

STATIC streamscall int
xp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct xp *xp, **xpp = &xp_list;

	(void) crp;
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN) {
		printd(("%s: Clone open in effect on major %d\n", DRV_NAME, cmajor));
		cminor = 1;
	}
	spin_lock_irqsave(&xp_lock, flags);
	for (; *xpp; xpp = &(*xpp)->next) {
		major_t dmajor = (*xpp)->cmajor;

		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*xpp)->cminor;

			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = xp_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&xp_lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %hu:%hu\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(xp = xp_alloc_priv(q, xpp, cmajor, cminor))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&xp_lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&xp_lock, flags);
	return (0);
}
STATIC streamscall int
xp_close(queue_t *q, int flag, cred_t *crp)
{
	struct xp *xp = PRIV(q);
	psw_t flags;

	(void) flag;
	(void) crp;
	(void) xp;
	printd(("%s: closing character device %hu:%hu\n", DRV_NAME, xp->cmajor, xp->cminor));
	spin_lock_irqsave(&xp_lock, flags);
	xp_free_priv(q);
	spin_unlock_irqrestore(&xp_lock, flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  PCI Initialization
 *
 *  =========================================================================
 */
/*
 *  X400P-SS7 Probe
 *  -----------------------------------
 *  Probes will be called for all PCI devices which match our criteria at pci
 *  init time (module load).  Successful return from the probe function will
 *  have the device configured for operation.
 */
STATIC int __devinit
xp_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	int byte, span;
	struct xp_card *cp;

	if (!dev || !id) {
		ptrace(("X400P-SS7: ERROR: Device or id is null!\n"));
		return (-ENXIO);
	}
	if (id->driver_data != X400PSS7 && id->driver_data != X400P) {
		ptrace(("X400P-SS7: ERROR: Driver does not support device type %ld\n",
			id->driver_data));
		return (-ENXIO);
	}
	if (pci_enable_device(dev)) {
		ptrace(("X400P-SS7: ERROR: Could not enable pci device\n"));
		return (-EIO);
	}
	printd(("X400P-SS7: enabled x400p-ss7 pci device type %ld\n", id->driver_data));
	if (!(cp = xp_alloc_card()))
		return (-ENOMEM);
	if ((cp->irq = dev->irq) < 1) {
		ptrace(("X400P-SS7: ERROR: No IRQ allocated for device\n"));
		goto error_free;
	}
	printd(("X400P-SS7: device allocated IRQ %ld\n", cp->irq));
	if ((pci_resource_flags(dev, 0) & IORESOURCE_IO)
	    || !(cp->plx_region = pci_resource_start(dev, 0))
	    || !(cp->plx_length = pci_resource_len(dev, 0))
	    || !(cp->plx = ioremap(cp->plx_region, cp->plx_length))) {
		ptrace(("X400P-SS7: ERROR: Invalid PLX 9030 base resource\n"));
		goto error_free;
	}
	printd(("X400P-SS7: plx region %ld bytes at %lx, remapped %p\n", cp->plx_length,
		cp->plx_region, cp->plx));
	if ((pci_resource_flags(dev, 2) & IORESOURCE_IO)
	    || !(cp->xll_region = pci_resource_start(dev, 2))
	    || !(cp->xll_length = pci_resource_len(dev, 2))
	    || !(cp->xll = ioremap(cp->xll_region, cp->xll_length))) {
		ptrace(("X400P-SS7: ERROR: Invalid Xilinx 32-bit base resource\n"));
		goto error_free;
	}
	printd(("X400P-SS7: xll region %ld bytes at %lx, remapped %p\n", cp->xll_length,
		cp->xll_region, cp->xll));
	if ((pci_resource_flags(dev, 3) & IORESOURCE_IO)
	    || !(cp->xlb_region = pci_resource_start(dev, 3))
	    || !(cp->xlb_length = pci_resource_len(dev, 3))
	    || !(cp->xlb = ioremap(cp->xlb_region, cp->xlb_length))) {
		ptrace(("X400P-SS7: ERROR: Invalid Xilinx 8-bit base resource\n"));
		goto error_free;
	}
	printd(("X400P-SS7: xlb region %ld bytes at %lx, remapped %p\n", cp->xlb_length,
		cp->xlb_region, cp->xlb));
	cp->ifname = xp_board_info[id->driver_data].name;
	if (!request_mem_region(cp->plx_region, cp->plx_length, cp->ifname)) {
		ptrace(("X400P-SS7: ERROR: Unable to reserve PLX memory\n"));
		goto error_free;
	}
	printd(("X400P-SS7: plx region %lx reserved %ld bytes\n", cp->plx_region, cp->plx_length));
	if (!request_mem_region(cp->xll_region, cp->xll_length, cp->ifname)) {
		ptrace(("X400P-SS7: ERROR: Unable to reserve Xilinx 32-bit memory\n"));
		goto error_free_plx_region;
	}
	printd(("X400P-SS7: xll region %lx reserved %ld bytes\n", cp->xll_region, cp->xll_length));
	if (!request_mem_region(cp->xlb_region, cp->xlb_length, cp->ifname)) {
		ptrace(("X400P-SS7: ERROR: Unable to reserve Xilinx 8-bit memory\n"));
		goto error_free_xll_region;
	}
	printd(("X400P-SS7: xlb region %lx reserved %ld bytes\n", cp->xlb_region, cp->xlb_length));
	pci_set_drvdata(dev, cp);
	__printd(("X400P-SS7: detected %s at 0x%lx/0x%lx irq %ld\n", cp->ifname, cp->xll_region,
		  cp->xlb_region, cp->irq));
#ifdef X400P_DOWNLOAD_FIRMWARE
	{
		uint8_t *f = (uint8_t *) x400pfw;
		volatile unsigned long *data;
		unsigned long timeout;

		data = (volatile unsigned long *) &cp->plx[GPIOC];
		*data |= GPIO_WRITE;
		*data &= ~GPIO_PROGRAM;
		while (*data & (GPIO_INIT | GPIO_DONE)) ;
		printd(("X400P-SS7: Xilinx Firmware Load: Init and done are low\n"));
		*data |= GPIO_PROGRAM;
		while (!(*data & GPIO_INIT)) ;
		printd(("X400P-SS7: Xilinx Firmware Load: Init is high\n"));
		*data &= ~GPIO_WRITE;
		printd(("X400P-SS7: Xilinx Firmware Load: Loading\n"));
		for (byte = 0; byte < sizeof(x400pfw); byte++) {
			*cp->xlb = *f++;
			if (*data & GPIO_DONE)
				break;
			if (!(*data & GPIO_INIT))
				break;
		}
		if (!(*data & GPIO_INIT)) {
			printd(("X400P-SS7: ERROR: Xilinx Firmware Load: Failed\n"));
			goto error_free_all;
		}
		printd(("X400P-SS7: Xilinx Firmware Load: Loaded %d bytes\n", byte));
		timeout = jiffies + 20 * HZ / 1000;
		while (jiffies < timeout) ;
		*data |= GPIO_WRITE;
		printd(("X400P-SS7: Xilinx Firmware Load: Done\n"));
		timeout = jiffies + 20 * HZ / 1000;
		while (jiffies < timeout) ;
		if (!(*data & GPIO_INIT)) {
			ptrace(("X400P-SS7: ERROR: Xilinx Firmware Load: Failed\n"));
			goto error_free_all;
		}
		if (!(*data & GPIO_DONE)) {
			ptrace(("X400P-SS7: ERROR: Xilinx Firmware Load: Failed\n"));
			goto error_free_all;
		}
		printd(("X400P-SS7: Xilinx Firmware Load: Successful\n"));
	}
#endif
	cp->plx[INTCSR] = 0;	/* disable interrupts */
	cp->xlb[SYNREG] = 0;
	cp->xlb[CTLREG] = 0;
	cp->xlb[LEDREG] = 0xff;
	if (cp->xlb[0x00f] & 0x80) {
		int word;

		printd(("E400P-SS7: E400P-SS7 Quad E1 Card\n"));
#if 0
		cp->wbuf = cp->xll;
		for (word = 0; word < 256; word++)
			cp->wbuf[word] = 0xffffffff;
#else
		for (word = 0; word < 256; word++) {
			cp->xll[word] = 0xffffffff;
			cp->wbuf[word] = 0xffffffff;
		}
#endif
		/* setup E1 card defaults */
		cp->ifgtype = SDL_GTYPE_E1;
		cp->ifgrate = 2048000;
		cp->ifgmode = SDL_GMODE_NONE;
		cp->ifgcrc = SDL_GCRC_CRC4;
		cp->ifclock = SDL_CLOCK_SLAVE;
		cp->ifcoding = SDL_CODING_HDB3;
		cp->ifframing = SDL_FRAMING_CCS;
		spin_lock_init(&cp->lock);	/* "cp-priv-lock" */
		if (request_irq(cp->irq, xp_e1_interrupt, SA_INTERRUPT | SA_SHIRQ, "x400p-ss7", cp)) {
			ptrace(("E400P-SS7: ERROR: Unable to request IRQ %ld\n", cp->irq));
			goto error_free_all;
		}
		printd(("E400P-SS7: acquired IRQ %ld for T400P-SS7 card\n", cp->irq));
		cp->ifflags = (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
		cp->iftxlevel = 0;
		cp->iftype = SDL_TYPE_DS0;
		cp->ifrate = 64000;
		cp->ifmode = SDL_MODE_PEER;
		tasklet_init(&cp->tasklet, &xp_e1_card_tasklet, (unsigned long) cp);
	} else {
		int word;

		printd(("T400P-SS7: T400P-SS7 Quad T1 Card\n"));
#if 0
		cp->wbuf = cp->xll;
		for (word = 0; word < 256; word++)
			cp->wbuf[word] = 0x7f7f7f7f;
#else
		for (word = 0; word < 256; word++) {
			cp->xll[word] = 0x7f7f7f7f;
			cp->wbuf[word] = 0x7f7f7f7f;
		}
#endif
		/* setup T1 card defaults */
		cp->ifgtype = SDL_GTYPE_T1;
		cp->ifgrate = 1544000;
		cp->ifgmode = SDL_GMODE_NONE;
		cp->ifgcrc = SDL_GCRC_CRC6;
		cp->ifclock = SDL_CLOCK_SLAVE;
		cp->ifcoding = SDL_CODING_AMI;
		cp->ifframing = SDL_FRAMING_SF;
		spin_lock_init(&cp->lock);	/* "cp-priv-lock" */
		if (request_irq(cp->irq, xp_t1_interrupt, SA_INTERRUPT | SA_SHIRQ, "x400p-ss7", cp)) {
			ptrace(("T400P-SS7: ERROR: Unable to request IRQ %ld\n", cp->irq));
			goto error_free_all;
		}
		printd(("T400P-SS7: acquired IRQ %ld for T400P-SS7 card\n", cp->irq));
		cp->ifflags = (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
		cp->iftxlevel = 0;
		cp->iftype = SDL_TYPE_DS0;
		cp->ifrate = 64000;
		cp->ifmode = SDL_MODE_PEER;
		tasklet_init(&cp->tasklet, &xp_t1_card_tasklet, (unsigned long) cp);
	}
	/* allocate span structures */
	for (span = 0; span < 4; span++)
		if (!xp_alloc_span(cp, span))
			goto error_free_all;
	cp->plx[INTCSR] = PLX_INTENA;	/* enable interrupts */
	return (0);
      error_free_all:
	release_mem_region(cp->xlb_region, cp->xlb_length);
	printd(("X400P-SS7: released xlb region %lx length %ld\n", cp->xlb_region, cp->xlb_length));
      error_free_xll_region:
	release_mem_region(cp->xll_region, cp->xll_length);
	printd(("X400P-SS7: released xll region %lx length %ld\n", cp->xll_region, cp->xll_length));
      error_free_plx_region:
	release_mem_region(cp->plx_region, cp->plx_length);
	printd(("X400P-SS7: released plx region %lx length %ld\n", cp->plx_region, cp->plx_length));
      error_free:
	xp_free_card(cp);
	return -ENODEV;
}

/*
 *  X400P-SS7 Remove
 *  -----------------------------------
 *  These cards do not support hotplug, so removes only occur after all the
 *  channels have been closed, so we only have to stop interrupts and
 *  deallocate board-level resources.  Nevertheless, if we get a hot removal
 *  of a card, we must be prepared to deallocate the span structures.
 */
STATIC void __devexit
xp_remove(struct pci_dev *dev)
{
	struct xp_card *cp = pci_get_drvdata(dev);

	ensure(cp, return);
	cp->plx[INTCSR] = 0;	/* disable interrupts */
	cp->xlb[SYNREG] = 0;
	cp->xlb[CTLREG] = 0;
	cp->xlb[LEDREG] = 0;
	free_irq(cp->irq, cp);
	release_mem_region(cp->xlb_region, cp->xlb_length);
	printd(("X400P-SS7: released xlb region %lx length %ld\n", cp->xlb_region, cp->xlb_length));
	release_mem_region(cp->xll_region, cp->xll_length);
	printd(("X400P-SS7: released xll region %lx length %ld\n", cp->xll_region, cp->xll_length));
	release_mem_region(cp->plx_region, cp->plx_length);
	printd(("X400P-SS7: released plx region %lx length %ld\n", cp->plx_region, cp->plx_length));
	if (cp->xlb)
		iounmap((void *) cp->xlb);
	printd(("X400P-SS7: unmapped xlb memory at %p\n", cp->xlb));
	if (cp->xll)
		iounmap((void *) cp->xll);
	printd(("X400P-SS7: unmapped xll memory at %p\n", cp->xll));
	if (cp->plx)
		iounmap((void *) cp->plx);
	printd(("X400P-SS7: unmapped plx memory at %p\n", cp->plx));
	xp_free_card(cp);
}

#ifdef CONFIG_PM
/*
 *  X400P-SS7 Suspend
 *  -----------------------------------
 */
STATIC int
xp_suspend(struct pci_dev *pdev, pm_message_t state)
{
	fixme(("Write a suspend routine.\n"));
	return 0;
}

/*
 *  X400P-SS7 Resume
 *  -----------------------------------
 */
STATIC int
xp_resume(struct pci_dev *pdev)
{
	fixme(("Write a resume routine.\n"));
	return 0;
}
#endif

/* 
 *  X400P-SS7 PCI Init
 *  -----------------------------------
 *  Here we need to scan for all available boards, configure the board-level
 *  structures, and then release all system resources.  The purpose of this is
 *  to identify the boards in the system at module startup or LiS
 *  initialization.
 *
 *  Later, when a stream is opened for any span, the span is configured, the
 *  drivers will be enabled and all channels in the span will have their
 *  power-on sequence completed and each channel will idle SIOS.  Closing
 *  channels will result in the transmitters resuming idle SIOS operation.
 */
STATIC INLINE int
xp_pci_init(void)
{
	return pci_module_init(&xp_driver);
}

/* 
 *  X400P-SS7 PCI Cleanup
 *  -----------------------------------
 *  Because this is a style 2 driver, if there are any boards that are atill
 *  configured, we need to stop the boards and deallocate the board-level
 *  resources and structures.
 */
STATIC INLINE int
xp_pci_cleanup(void)
{
	pci_unregister_driver(&xp_driver);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the X400-SDL driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the X400-SDL driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw xp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sdt_x400pinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
xp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&xp_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
xp_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&xp_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
xp_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &sdt_x400pinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

STATIC int
xp_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
sdt_x400pterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (xp_majors[mindex]) {
			if ((err = xp_unregister_strdev(xp_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					xp_majors[mindex]);
			if (mindex)
				xp_majors[mindex] = 0;
		}
	}
	if ((err = xp_bufpool_term()))
		cmn_err(CE_WARN, "%s: could not terminate buffer pools", DRV_NAME);
	if ((err = xp_free_tables()))
		cmn_err(CE_WARN, "%s: could not free tables", DRV_NAME);
	if ((err = xp_pci_cleanup()))
		cmn_err(CE_WARN, "%s: could not cleanup pci device", DRV_NAME);
	if ((err = xp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
sdt_x400pinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = xp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		sdt_x400pterminate();
		return (err);
	}
	if ((err = xp_pci_init())) {
		cmn_err(CE_WARN, "%s: no PCI devices found, err = %d", DRV_NAME, err);
		sdt_x400pterminate();
		return (err);
	}
	if ((err = xp_init_tables())) {
		cmn_err(CE_WARN, "%s: could not allocate tables", DRV_NAME);
		sdt_x400pterminate();
		return (err);
	}
	if ((err = xp_bufpool_init())) {
		cmn_err(CE_WARN, "%s: could not prime buffer pool", DRV_NAME);
		sdt_x400pterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = xp_register_strdev(xp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					xp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				sdt_x400pterminate();
				return (err);
			}
		}
		if (xp_majors[mindex] == 0)
			xp_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(xp_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = xp_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sdt_x400pinit);
module_exit(sdt_x400pterminate);

#endif				/* LINUX */
