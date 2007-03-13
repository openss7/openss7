/*****************************************************************************

 @(#) $RCSfile: sl_x400p.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/03/13 08:50:04 $

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

 Last Modified $Date: 2007/03/13 08:50:04 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sl_x400p.c,v $
 Revision 0.9.2.33  2007/03/13 08:50:04  brian
 - bug fixes for PR x400p-ss7/5766

 Revision 0.9.2.32  2007/03/13 05:46:00  brian
 - more info

 Revision 0.9.2.31  2007/03/09 04:09:33  brian
 - fixed timer bug in x400p driver

 Revision 0.9.2.30  2006/12/21 11:14:43  brian
 - documentation updates for release, and moved tali

 Revision 0.9.2.29  2006/12/18 10:51:27  brian
 - subpackaging changes for release

 Revision 0.9.2.28  2006/12/11 22:02:51  brian
 - performance tuning

 Revision 0.9.2.27  2006/12/11 11:57:40  brian
 - T1 works correctly, almost all test cases pass

 Revision 0.9.2.26  2006/12/11 07:40:12  brian
 - corrections from testing

 Revision 0.9.2.25  2006/12/09 10:40:24  brian
 - corrections from testing

 Revision 0.9.2.24  2006/12/08 12:16:13  brian
 - bufq lock correction

 Revision 0.9.2.23  2006/12/08 11:46:32  brian
 - a few more corrections from testing

 Revision 0.9.2.22  2006/12/08 05:32:09  brian
 - changes from testing of X400P-SS7 driver

 Revision 0.9.2.21  2006/12/07 12:56:17  brian
 - corrections from testing

 Revision 0.9.2.20  2006/12/07 09:58:39  brian
 - corrections and init scripts

 Revision 0.9.2.19  2006/12/06 11:45:22  brian
 - updated X400P driver and test suites

 Revision 0.9.2.18  2006/11/15 08:58:54  brian
 - error in sdl rx found by inspection

 Revision 0.9.2.17  2006/05/08 11:01:17  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.16  2006/03/07 01:14:58  brian
 - binary compatible callouts

 Revision 0.9.2.15  2006/03/04 13:00:27  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: sl_x400p.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/03/13 08:50:04 $"

static char const ident[] =
    "$RCSfile: sl_x400p.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/03/13 08:50:04 $";

/*
 *  This is an SL (Signalling Link) kernel module which provides all of the
 *  capabilities of the SLI for the E400P-SS7 and T400P-SS7 cards.  This is a
 *  complete SS7 MTP Level 2 OpenSS7 implementation.
 */

//#define _DEBUG 1
#undef _DEBUG

#define _LFS_SOURCE	1
#define _MPS_SOURCE	1
#define _SVR4_SOURCE	1

#define X400P_DOWNLOAD_FIRMWARE 1

#include <sys/os7/compat.h>

#include <stdbool.h>

#ifdef LINUX
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <linux/pci.h>

#include <linux/interrupt.h>
#ifndef _MPS_SOURCE
#if 0
#include "bufpool.h"
#else
#include <sys/os7/bufpool.h>
#endif
#endif				/* _MPS_SOURCE */
#endif				/* LINUX */

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#ifdef X400P_DOWNLOAD_FIRMWARE
#include "v400pfw.h"
#include "v401pfw.h"
#endif

#define SL_X400P_DESCRIP	"X400P-SS7: SS7/SL (Signalling Link) STREAMS DRIVER."
#define SL_X400P_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SL_X400P_REVISION	"OpenSS7 $RCSfile: sl_x400p.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/03/13 08:50:04 $"
#define SL_X400P_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SL_X400P_DEVICE		"Supports the V40XP E1/T1/J1 (Tormenta II/III) PCI boards."
#define SL_X400P_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SL_X400P_LICENSE	"GPL"
#define SL_X400P_BANNER		SL_X400P_DESCRIP	"\n" \
				SL_X400P_EXTRA		"\n" \
				SL_X400P_REVISION	"\n" \
				SL_X400P_COPYRIGHT	"\n" \
				SL_X400P_DEVICE		"\n" \
				SL_X400P_CONTACT	"\n"
#define SL_X400P_SPLASH		SL_X400P_DESCRIP	" - " \
				SL_X400P_REVISION

#ifdef LINUX
MODULE_AUTHOR(SL_X400P_CONTACT);
MODULE_DESCRIPTION(SL_X400P_DESCRIP);
MODULE_SUPPORTED_DEVICE(SL_X400P_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SL_X400P_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sl_x400p");
#endif
#if defined MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif
#endif				/* LINUX */

#ifdef LFS
#define SL_X400P_DRV_ID		CONFIG_STREAMS_SL_X400P_MODID
#define SL_X400P_DRV_NAME	CONFIG_STREAMS_SL_X400P_NAME
#define SL_X400P_CMAJORS	CONFIG_STREAMS_SL_X400P_NMAJORS
#define SL_X400P_CMAJOR_0	CONFIG_STREAMS_SL_X400P_MAJOR
#define SL_X400P_UNITS		CONFIG_STREAMS_SL_X400P_NMINORS
#endif

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SL_X400P_MODID));
MODULE_ALIAS("streams-driver-sl-x400p");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_SL_X400P_MAJOR));
MODULE_ALIAS("/dev/streams/x400p-sl");
MODULE_ALIAS("/dev/streams/x400p-sl/*");
MODULE_ALIAS("/dev/streams/clone/x400p-sl");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(SL_X400P_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(SL_X400P_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(SL_X400P_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/x400p-sl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define DRV_ID		SL_X400P_DRV_ID
#define DRV_NAME	SL_X400P_DRV_NAME
#define CMAJORS		SL_X400P_CMAJORS
#define CMAJOR_0	SL_X400P_CMAJOR_0
#define UNITS		SL_X400P_UNITS
#ifdef MODULE
#define DRV_BANNER	SL_X400P_BANNER
#else				/* MODULE */
#define DRV_BANNER	SL_X400P_SPLASH
#endif				/* MODULE */

STATIC struct module_info xp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1024,		/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC struct module_stat xp_mstat = { 0, };

STATIC streamscall int xp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int xp_close(queue_t *, int, cred_t *);

STATIC struct qinit xp_rinit = {
	.qi_putp = ss7_oput,		/* Read put (message from below) */
	.qi_srvp = ss7_osrv,		/* Read queue service */
	.qi_qopen = xp_open,		/* Each open */
	.qi_qclose = xp_close,		/* Last close */
	.qi_minfo = &xp_minfo,		/* Information */
	.qi_mstat = &xp_mstat,		/* Statistics */
};

STATIC struct qinit xp_winit = {
	.qi_putp = ss7_iput,		/* Write put (message from above) */
	.qi_srvp = ss7_isrv,		/* Write queue service */
	.qi_minfo = &xp_minfo,		/* Information */
	.qi_mstat = &xp_mstat,		/* Statistics */
};

STATIC struct streamtab sl_x400pinfo = {
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
struct xp;
struct sp;
struct cd;

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
	mblk_t *cmp;			/* compression/repeat message */
	uint repeat;			/* compression/repeat count */
	uint octets;			/* octets counted */
} xp_path_t;

typedef struct xp {
	STR_DECLARATION (struct xp);	/* stream declaration */
	struct sp *sp;			/* span for this channel */
	int chan;			/* index (chan) */
	int slot;			/* 32-bit backplane timeslot */
	xp_path_t tx;			/* transmit path variables */
	xp_path_t rx;			/* receive path variables */
	lmi_option_t option;		/* LMI protocol and variant options */
	struct {
		bufq_t rb;		/* received buffer */
		bufq_t tb;		/* transmission buffer */
		bufq_t rtb;		/* retransmission buffer */
		sl_timers_t timers;	/* SL protocol timers */
		sl_config_t config;	/* SL configuration */
		sl_statem_t statem;	/* SL state machine */
		sl_notify_t notify;	/* SL notification options */
		sl_stats_t stats;	/* SL statistics */
		sl_stats_t stamp;	/* SL statistics timestamps */
		sl_stats_t statsp;	/* SL statistics periods */
	} sl;
	struct {
		bufq_t tb;		/* transmission buffer */
		sdt_timers_t timers;	/* SDT protocol timers */
		sdt_config_t config;	/* SDT configuration */
		sdt_statem_t statem;	/* SDT state machine */
		sdt_notify_t notify;	/* SDT notification options */
		sdt_stats_t stats;	/* SDT statistics */
		sdt_stats_t stamp;	/* SDT statistics timestamps */
		sdt_stats_t statsp;	/* SDT statistics periods */
	} sdt;
	struct {
		bufq_t tb;		/* transmission buffer */
		sdl_timers_t timers;	/* SDL protocol timers */
		sdl_config_t config;	/* SDL configuration */
		sdl_statem_t statem;	/* SDL state machine variables */
		sdl_notify_t notify;	/* SDL notification options */
		sdl_stats_t stats;	/* SDL statistics */
		sdl_stats_t stamp;	/* SDL statistics timestamps */
		sdl_stats_t statsp;	/* SDL statistics periods */
	} sdl;
} xp_t;

#define XP_PRIV(__q) ((struct xp *)(__q)->q_ptr)

STATIC struct xp *xp_alloc_priv(queue_t *, struct xp **, dev_t *, cred_t *);
STATIC void xp_free_priv(struct xp *);
STATIC struct xp *xp_get(struct xp *);
STATIC void xp_put(struct xp *);

typedef struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	struct cd *cd;			/* card for this span */
	struct xp *slots[32];		/* timeslot structures */
	ulong recovertime;		/* alarm recover time */
	ulong iobase;			/* span iobase */
	int span;			/* index (span) */
	volatile ulong loopcnt;		/* loop command count */
	sdl_config_t config;		/* span configuration */
} sp_t;

STATIC struct sp *xp_alloc_sp(struct cd *, uint8_t);
STATIC void xp_free_sp(struct sp *);
STATIC struct sp *sp_get(struct sp *);
STATIC void sp_put(struct sp *);

typedef struct cd {
	HEAD_DECLARATION (struct cd);	/* head declaration */
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
	struct sp *spans[4];		/* structures for spans */
	uint32_t *wbuf;			/* wr buffer */
	uint32_t *rbuf;			/* rd buffer */
	volatile int uebno;		/* upper elastic buffer number */
	volatile int lebno;		/* lower elastic buffer number */
	volatile int eval_syncsrc;	/* need to reevaluate sync src */
	volatile int leds;		/* leds on the card */
	int card;			/* index (card) */
	int board;			/* board hardware index */
	int device;			/* device hardware index */
	int devrev;			/* device hardware revision */
	int hw_flags;			/* board and device hardware flags */
	ulong irq;			/* card irq */
	ulong iobase;			/* card iobase */
	struct tasklet_struct tasklet;	/* card tasklet */
	 irqreturn_t(*isr) (int, void *, struct pt_regs *);	/* interrupt service routine */
	sdl_config_t config;		/* card configuration */
} cd_t;

STATIC struct cd *xp_alloc_cd(void);
STATIC void xp_free_cd(struct cd *);
STATIC struct cd *cd_get(struct cd *);
STATIC void cd_put(struct cd *);

#ifdef _MPS_SOURCE
#define ss7_fast_allocb(__bufpoolp, __size, __priority)	allocb(__size, __priority)
#define ss7_fast_freemsg(__bufpoolp, __mp) freemsg(__mp)
#define ss7_bufpool_reserve(__bufpoolp, __numb) do { } while (0)
#define ss7_bufpool_release(__bufpoolp, __numb) do { } while (0)
#define ss7_bufpool_init(__bufpoolp) do { } while (0)
#define ss7_bufpool_term(__bufpoolp) do { } while (0)
#else				/* _MPS_SOURCE */
STATIC struct ss7_bufpool xp_bufpool = { 0, };
#endif				/* _MPS_SOURCE */

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

#define LOOPUP	0x80
#define LOOPDN	0x40

#define INTENA	0x01		/* Interrupt Enable */
#define OUTBIT	0x02		/* Drives "TEST1" signal ("Interrupt" outbit) */
#define DINTENA	0x04		/* Dallas Interrupt Enable (Allows DINT signal to driver INT) */
#define MASTER	0x08		/* External Syncrhonization Enable (MASTER signal). */
#define E1DIV	0x10		/* Select E1 Divisor Mode (0 to T1, 1 for E1). */
#define RSERLB	0x20		/* Local serial loopback. */
#define LSERLB	0x40		/* Remote serial loopback. */
#define INTACK	0x80		/* Interrupt Acknowledge (set to 1 to acknowledge interrupt) */

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

#define X400_SPANS 4		/* 4 spans per card */

#define X400P_SDL_ALARM_SETTLE_TIME	    5000	/* allow alarms to settle for 5 seconds */

/*
 *  Mapping of channels 0-23 for T1, 1-31 for E1 into PCM highway timeslots.
 */
STATIC int xp_t1_chan_map[] = {
	1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15,
	17, 18, 19, 21, 22, 23, 25, 26, 27, 29, 30, 31
};
STATIC int xp_e1_chan_map[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
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
	u32 hw_flags;
	u32 idle_word;
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

#define XPF_SUPPORTED	0x01

#define XP_DEV_IDMASK	0xf0
#define XP_DEV_SHIFT	4
#define XP_DEV_REVMASK	0x0f
#define XP_DEV_DS2154	0x00
#define XP_DEV_DS21354	0x01
#define XP_DEV_DS21554	0x02
#define XP_DEV_DS2155E	0x03
#define XP_DEV_DS2152	0x08
#define XP_DEV_DS21352	0x09
#define XP_DEV_DS21552	0x0a
#define XP_DEV_DS2155T	0x0b

STATIC struct {
	char *name;
	uint32_t hw_flags;
} xp_device_info[] __devinitdata = {
	{ "DS2154 (E1)",	1 },
	{ "DS21354 (E1)",	1 },
	{ "DS21554 (E1)",	1 },
	{ "DS2155 (E1/T1/J1)",	1 },
	{ "Unknown ID 0100",	0 },
	{ "Unknown ID 0101",	0 },
	{ "Unknown ID 0110",	0 },
	{ "Unknown ID 0111",	0 },
	{ "DS2152 (T1)",	1 },
	{ "DS21352 (T1)",	1 },
	{ "DS21552 (T1)",	1 },
	{ "DS2155 (T1/J1/E1)",	1 },
	{ "Unknown ID 1100",	0 },
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
	name:DRV_NAME,
	probe:xp_probe,
	remove:__devexit_p(xp_remove),
	id_table:xp_pci_tbl,
#ifdef CONFIG_PM
	suspend:xp_suspend,
	resume:xp_resume,
#endif
};

STATIC int x400p_boards = 0;
STATIC struct cd *x400p_cards;

#define X400P_EBUFNO (1<<7)	/* 128k elastic buffers */

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
STATIC noinline __unlikely int
m_error(struct xp *xp, queue_t *q, int err)
{
	mblk_t *mp;

	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		printd(("%s: %p: <- M_ERROR\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_PDU_IND
 *  -----------------------------------
 *  We don't actually use SL_PDU_INDs, we pass along M_DATA messages.
 */
STATIC inline fastcall __hot_read int
sl_pdu_ind(struct xp *xp, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	sl_pdu_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_PDU_IND;
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_PDU_IND\n", DRV_NAME, xp));
		put(RD(q), mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_link_congested_ind(struct xp *xp, queue_t *q, sl_ulong cong, sl_ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTED_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_link_congestion_ceased_ind(struct xp *xp, queue_t *q, sl_ulong cong, sl_ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ceased_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		p->sl_timestamp = jiffies;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTION_CEASED_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_retrieved_message_ind(struct xp *xp, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	sl_retrieved_msg_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_RETRIEVED_MESSGAGE_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_retrieval_complete_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sl_retrieval_comp_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		printd(("%s: %p: <- SL_RETIREVAL_COMPLETE_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RB_CLEARED_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_rb_cleared_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sl_rb_cleared_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RB_CLEARED_IND;
		printd(("%s: %p: <- SL_RB_CLEARED_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_bsnt_ind(struct xp *xp, queue_t *q, sl_ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_BSNT_IND;
		p->sl_bsnt = bsnt;
		printd(("%s: %p: <- SL_BSNT_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 */
STATIC noinline fastcall int
sl_in_service_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sl_in_service_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_IN_SERVICE_IND;
		printd(("%s: %p: <- SL_IN_SERVICE_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  -----------------------------------
 */
STATIC noinline fastcall int
sl_out_of_service_ind(struct xp *xp, queue_t *q, sl_ulong reason)
{
	mblk_t *mp;
	sl_out_of_service_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = jiffies;
		p->sl_reason = reason;
		printd(("%s: %p: <- SL_OUT_OF_SERVICE_IND\n", DRV_NAME, xp));
		put(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_remote_processor_outage_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sl_rem_proc_out_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = jiffies;
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_OUTAGE_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_remote_processor_recovered_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sl_rem_proc_recovered_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = jiffies;
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_RECOVERED_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RTB_CLEARED_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_rtb_cleared_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sl_rtb_cleared_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RTB_CLEARED_IND;
		printd(("%s: %p: <- SL_RTB_CLEARED_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_retrieval_not_possible_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sl_retrieval_not_poss_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		printd(("%s: %p: <- SL_RETRIEVAL_NOT_POSSIBLE_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_bsnt_not_retrievable_ind(struct xp *xp, queue_t *q, sl_ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_not_retr_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		p->sl_bsnt = bsnt;
		printd(("%s: %p: <- SL_BSNT_NOT_RETRIEVABLE_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SL_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_optmgmt_ack(struct xp *xp, queue_t *q, caddr_t opt_ptr, size_t opt_len, sl_ulong flags)
{
	mblk_t *mp;
	sl_optmgmt_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_OPTMGMT_ACK;
		p->opt_length = opt_len;
		p->opt_offset = opt_len ? sizeof(*p) : 0;
		p->mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- SL_OPTMGMT_ACK\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_NOTIFY_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_notify_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sl_notify_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_NOTIFY_IND;
		printd(("%s: %p: <- SL_NOTIFY_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_info_ack(struct xp *xp, queue_t *q, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_info_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = xp->i_state;
		p->lmi_max_sdu = xp->sdt.config.m + 1 + ((xp->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((xp->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", DRV_NAME, xp));
		putnext(RD(q), mp);
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
 *  Can't use buffer service.
 */
STATIC inline fastcall __hot_in int
sdt_rc_signal_unit_ind(struct xp *xp, queue_t *q, mblk_t *dp, sl_ulong count)
{
	if (likely(count)) {
		if (likely(canput(RD(q)))) {
			mblk_t *mp;
			sdt_rc_signal_unit_ind_t *p;

			if (likely(!!(mp = allocb(sizeof(*p), BPRI_MED)))) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
				p->sdt_count = count;
				mp->b_cont = dp;
				_printd(("%s: %p: <- SDT_RC_SIGNAL_UNIT_IND\n", DRV_NAME, xp));
				put(RD(q), mp);
				return (QR_ABSORBED);
			}
			rare();
			return (-ENOBUFS);
		}
		rare();
		return (-EBUSY);
	}
	swerr();
	return (-EFAULT);
}

#if 0
/*
 *  SDT_RC_CONGESTION_ACCEPT_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_rc_congestion_accept_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sdt_rc_congestion_accept_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_ACCEPT_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_CONGESTION_DISCARD_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_rc_congestion_discard_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sdt_rc_congestion_discard_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_DISCARD_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_NO_CONGESTION_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_rc_no_congestion_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sdt_rc_no_congestion_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
		printd(("%s: %p: <- SDT_RC_NO_CONGESTION_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDT_IAC_CORRECT_SU_IND
 *  -----------------------------------
 */
STATIC inline fastcall __hot_read int
sdt_iac_correct_su_ind(struct xp *xp, queue_t *q)
{
	if (canputnext(RD(q))) {
		mblk_t *mp;
		sdt_iac_correct_su_ind_t *p;

		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
			printd(("%s: %p: <- SDT_IAC_CORRECT_SU_IND\n", DRV_NAME, xp));
			putnext(RD(q), mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

#if 0
/*
 *  SDT_IAC_ABORT_PROVING_IND
 *  -----------------------------------
 */
STATIC noinline fastcall int
sdt_iac_abort_proving_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sdt_iac_abort_proving_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
		printd(("%s: %p: <- SDT_IAC_ABORT_PROVING_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_LSC_LINK_FAILURE_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_lsc_link_failure_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sdt_lsc_link_failure_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
		printd(("%s: %p: <- SDT_LSC_LINK_FAILURE_IND\n", DRV_NAME, xp));
		put(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND
 *  -----------------------------------
 */
STATIC inline fastcall __hot_out int
sdt_txc_transmission_request_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sdt_txc_transmission_request_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		_printd(("%s: %p: <- SDT_TXC_TRANSMISSION_REQUEST_IND\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_RECEIVED_BITS_IND
 *  -----------------------------------
 *  Quickly we just copy the buffer and leave the original for the lower level
 *  driver.
 */
STATIC inline fastcall __hot_in int
sdl_received_bits_ind(struct xp *xp, queue_t *q, mblk_t *dp)
{
	if (canput(RD(q))) {
		_printd(("%s: %p: <- SDL_RECEIVED_BITS_IND\n", DRV_NAME, xp));
		put(RD(q), dp);
		return (QR_ABSORBED);
	}
	rare();
	dp->b_wptr = dp->b_rptr;	/* discard contents */
	return (-EBUSY);
}

#if 0
/*
 *  SDL_DISCONNECT_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdl_disconnect_ind(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	sdl_disconnect_ind_t *p;

	(void) xp;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_DISCONNECT_IND;
		printd(("%s: %p: <- SDL_DISCONNECT_IND\n", DRV_NAME, xp));
		put(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_ok_ack(struct xp *xp, queue_t *q, sl_ulong state, sl_long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		p->lmi_state = xp->i_state = state;
		printd(("%s: %p: <- LMI_OK_ACK\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_error_ack(struct xp *xp, queue_t *q, sl_ulong state, sl_long prim, sl_ulong errno, sl_ulong reason)
{
	mblk_t *mp;
	lmi_error_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_error_primitive = prim;
		p->lmi_state = xp->i_state = state;
		printd(("%s: %p: <- LMI_ERROR_ACK\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_enable_con(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	lmi_enable_con_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = xp->i_state = LMI_ENABLED;
		printd(("%s: %p: <- LMI_ENABLE_CON\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_disable_con(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	lmi_disable_con_t *p;

	if (putctl2(RD(q), M_FLUSH, FLUSHRW, 0)) {
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_DISABLE_CON;
			p->lmi_state = xp->i_state = LMI_DISABLED;
			printd(("%s: %p: <- LMI_DISABLE_CON\n", DRV_NAME, xp));
			putnext(RD(q), mp);
			return (QR_DONE);
		}
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_optmgmt_ack(struct xp *xp, queue_t *q, sl_ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", DRV_NAME, xp));
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_error_ind(struct xp *xp, queue_t *q, sl_ulong errno, sl_ulong reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = xp->i_state;
		printd(("%s: %p: <- LMI_ERROR_IND\n", DRV_NAME, xp));
		put(RD(q), mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_stats_ind(struct xp *xp, queue_t *q, sl_ulong interval)
{
	if (canputnext(RD(q))) {
		mblk_t *mp;
		lmi_stats_ind_t *p;

		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = jiffies;
			printd(("%s: %p: <- LMI_STATS_IND\n", DRV_NAME, xp));
			putnext(RD(q), mp);
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
STATIC noinline fastcall __unlikely int
lmi_event_ind(struct xp *xp, queue_t *q, sl_ulong oid, sl_ulong level)
{
	if (canput(RD(q))) {
		mblk_t *mp;
		lmi_event_ind_t *p;

		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = jiffies;
			p->lmi_severity = level;
			printd(("%s: %p: <- LMI_EVENT_IND\n", DRV_NAME, xp));
			put(RD(q), mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}
#endif

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE FUNCTIONS
 *
 *  =========================================================================
 */
/*
 *  ------------------------------------------------------------------------
 *
 *  Default Configuration
 *
 *  ------------------------------------------------------------------------
 */
STATIC lmi_option_t lmi_default_e1_chan = {
	.pvar = SS7_PVAR_ITUT_00,
	.popt = 0,
};
STATIC lmi_option_t lmi_default_t1_chan = {
	.pvar = SS7_PVAR_ANSI_00,
	.popt = SS7_POPT_MPLEV,
};
STATIC lmi_option_t lmi_default_j1_chan = {
	.pvar = SS7_PVAR_JTTC_94,
	.popt = SS7_POPT_MPLEV,
};
STATIC lmi_option_t lmi_default_e1_span = {
	.pvar = SS7_PVAR_ITUT_00,
	.popt = SS7_POPT_HSL | SS7_POPT_XSN,
};
STATIC lmi_option_t lmi_default_t1_span = {
	.pvar = SS7_PVAR_ANSI_00,
	.popt = SS7_POPT_MPLEV | SS7_POPT_HSL | SS7_POPT_XSN,
};
STATIC lmi_option_t lmi_default_j1_span = {
	.pvar = SS7_PVAR_JTTC_94,
	.popt = SS7_POPT_MPLEV | SS7_POPT_HSL | SS7_POPT_XSN,
};
STATIC sl_config_t sl_default_e1_chan = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
};
STATIC sl_config_t sl_default_e1_span = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
};
STATIC sl_config_t sl_default_t1_chan = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
};
STATIC sl_config_t sl_default_t1_span = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
};
STATIC sl_config_t sl_default_j1_chan = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
};
STATIC sl_config_t sl_default_j1_span = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
};
STATIC sdt_config_t sdt_default_e1_span = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 793544,
	.De = 11328000,
	.Ue = 198384000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
};
STATIC sdt_config_t sdt_default_t1_span = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 577169,
	.De = 9308000,
	.Ue = 144292000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
};
STATIC sdt_config_t sdt_default_j1_span = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 577169,
	.De = 9308000,
	.Ue = 144292000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
};
STATIC sdt_config_t sdt_default_e1_chan = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 793544,
	.De = 11328000,
	.Ue = 198384000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
};
STATIC sdt_config_t sdt_default_t1_chan = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 577169,
	.De = 9308000,
	.Ue = 144292000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
};
STATIC sdt_config_t sdt_default_j1_chan = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 577169,
	.De = 9308000,
	.Ue = 144292000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
};
STATIC sdl_config_t sdl_default_e1_chan = {
	.ifname = NULL,
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0,
	.ifrate = 64000,
	.ifgtype = SDL_GTYPE_E1,
	.ifgrate = 2048000,
	.ifmode = SDL_MODE_PEER,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_CRC5,
	.ifclock = SDL_CLOCK_SLAVE,
	.ifcoding = SDL_CODING_HDB3,
	.ifframing = SDL_FRAMING_CCS,
	.ifblksize = 8,
	.ifleads = 0,
	.ifbpv = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 1,
	.ifsync = 0,
	.ifsyncsrc = {0, 0, 0, 0}
	,
};
STATIC sdl_config_t sdl_default_t1_chan = {
	.ifname = NULL,
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0,
	.ifrate = 64000,
	.ifgtype = SDL_GTYPE_T1,
	.ifgrate = 1544000,
	.ifmode = SDL_MODE_PEER,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_CRC6,
	.ifclock = SDL_CLOCK_LOOP,
	.ifcoding = SDL_CODING_B8ZS,
	.ifframing = SDL_FRAMING_ESF,
	.ifblksize = 8,
	.ifleads = 0,
	.ifbpv = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 0,
	.ifsync = 0,
	.ifsyncsrc = {0, 0, 0, 0}
	,
};
STATIC sdl_config_t sdl_default_j1_chan = {
	.ifname = NULL,
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0A,
	.ifrate = 64000,
	.ifgtype = SDL_GTYPE_J1,
	.ifgrate = 1544000,
	.ifmode = SDL_MODE_PEER,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_CRC6J,
	.ifclock = SDL_CLOCK_LOOP,
	.ifcoding = SDL_CODING_B8ZS,
	.ifframing = SDL_FRAMING_ESF,
	.ifblksize = 8,
	.ifleads = 0,
	.ifbpv = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 0,
	.ifsync = 0,
	.ifsyncsrc = {0, 0, 0, 0}
	,
};

STATIC noinline __unlikely int
xp_span_config(struct cd *cd, int span, bool timeouts)
{
	struct sp *sp = cd->spans[span];
	volatile unsigned char *xlb = &cd->xlb[span << 8];
	int offset;
	unsigned long timeout;

	switch (cd->board) {
	case V400PE:
	case E400P:
	case E400PSS7:
	{
		uint8_t ccr1 = 0, tcr1 = 0, reg18 = 0, regac = 0;

		xlb[0x1a] = 0x04;	/* CCR2: set LOTCMC */
#if 1
		/* wierd thing to do */
		for (offset = 0; offset <= 8; offset++)
			xlb[offset] = 0x00;
		for (offset = 0x10; offset <= 0x4f; offset++)
			if (offset != 0x1a)
				xlb[offset] = 0x00;
#endif
		xlb[0x10] = 0x20;	/* RCR1: Rsync as input */
		xlb[0x11] = 0x06;	/* RCR2: Sysclk = 2.048 Mhz */
		xlb[0x12] = 0x09;	/* TCR1: TSiS mode */
		tcr1 = 0x09;	/* TCR1: TSiS mode */
		switch (sp->config.ifframing) {
		default:
		case SDL_FRAMING_CCS:
			ccr1 |= 0x08;
			break;
		case SDL_FRAMING_CAS:
			tcr1 |= 0x20;
			break;
		}
		switch (sp->config.ifcoding) {
		default:
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
		xlb[0x12] = tcr1;
		xlb[0x14] = ccr1;

		if (sp->config.iftxlevel < 8) {
			/* not monitoring mode */
			regac = 0x00;	/* TEST3 no gain */
			reg18 = 0x00;	/* 75 Ohm, Normal, transmitter on */
			reg18 |= ((sp->config.iftxlevel & 0x7) << 5);	/* LBO */
		} else {
			/* monitoring mode */
			regac = 0x00;	/* TEST3 no gain */
			reg18 = 0x01;	/* 75 Ohm norm, transmitter off */
			switch (sp->config.iftxlevel & 0x3) {
			case 0:
				break;
			case 1:
				regac |= 0x72;	/* TEST3 12dB gain */
				break;
			case 2:
			case 3:
				regac |= 0x70;	/* TEST3 30dB gain */
				break;
			}
		}
		// reg18 |= 0x01; /* disable transmitter */

		xlb[0xac] = regac;
		xlb[0x18] = reg18;

		xlb[0x1b] = 0x8a;	/* CRC3: LIRST & TSCLKM */
		xlb[0x20] = 0x1b;	/* TAFR */
		xlb[0x21] = 0x5f;	/* TNAFR */
		xlb[0x40] = 0x0b;	/* TSR1 */

		if (timeouts) {
			/* wierd thing to do */
			for (offset = 0x41; offset <= 0x4f; offset++)
				xlb[offset] = 0x55;
			for (offset = 0x22; offset <= 0x25; offset++)
				xlb[offset] = 0xff;
			timeout = jiffies + 100 * HZ / 1000;
			while (jiffies < timeout) ;
		}

		xlb[0x1b] = 0x9a;	/* CRC3: set ESR as well */
		xlb[0x1b] = 0x82;	/* CRC3: TSCLKM only */
		break;
	}
	case V401PE:
	{
		u_char reg33, reg35, reg40, reg78, reg7a;

		/* There are three other synchronization modes: 0x00 TCLK only, 0x02 switch to RCLK 
		   if TCLK fails, 0x04 external clock, 0x06 loop.  And then 0x80 selects the
		   TSYSCLK pin instead of the MCLK pin when in external clock mode. */
		/* Hmmm. tor3 driver has TCLK only for T1, but RCLK if TCLK fails for E1! */
		xlb[0x70] = 0x02;	/* LOTCMC into TCSS0 */
		/* IOCR1.0=0 Output data format is bipolar, IOCR1.1=1 TSYNC is an output, IOCR1.4=1 
		   RSYNC is an input (elastic store). */
		xlb[0x01] = 0x12;	/* RSIO + 1 is from O12.0 */
		xlb[0x02] = 0x03;	/* RSYSCLK/TSYSCLK 8.192MHz IBO */
		xlb[0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
#if 0
		/* We should really reset the elastic store after reset like so: */
		xlb[0x4f] = 0x55;	/* RES/TES (elastic store) reset */
		xlb[0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
		/* And even align it like so: */
		xlb[0x4f] = 0x99;	/* RES/TES (elastic store) reset */
		xlb[0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
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
		xlb[0x35] = reg35;	/* TSiS, TCRC4 (from 014.4), THDB3 (from O14.6) */
		xlb[0x36] = 0x04;	/* AEBE 36.2 */
		xlb[0x34] = 0x01;	/* RCL (1ms) */
		xlb[0x40] = reg40;	/* RCCS, TCCS */
		xlb[0x33] = reg33 | 0x01;	/* RCR4, RHDB3, RSM, SYNCE, RESYNC */
		xlb[0x33] = reg33 | 0x00;	/* RCR4, RHDB3, RSM, SYNCE */
		/* This is a little peculiar: the host should be using Method 3 in section 22.3 of
		   the DS2155 manual instead of this method that only permits 250us to read or
		   write the bits. */
		xlb[0xd0] = 0x1b;	/* TAFR */
		xlb[0xd1] = 0x5f;	/* TNAFR */

		xlb[0x79] = 0x98;	/* JACLK on for E1 */
		xlb[0x7b] = 0x0f;	/* 120 Ohm term, MCLK 2.048 MHz */
		xlb[0x7d] = 0x00;	/* Automatic gain control */

		/* We use TX levels to determine LBO, impedance, CSU operation, or monitoring
		   operation.  During monitoring operation, the transmitters are powered off. */
		/* For E1, LBO is: 000XXXXX 75 Ohm normal 001XXXXX 120 Ohm normal 100XXXXX 75 Ohm
		   high return loss 101XXXXX 120 Ohm high return loss For T1, LBO is: 000XXXXX
		   DSX-1 ( 0ft - 133ft) / 0dB CSU 001XXXXX DSX-1 (133ft - 266ft) 010XXXXX DSX-1
		   (266ft - 399ft) 011XXXXX DSX-1 (399ft - 533ft) 100XXXXX DSX-1 (533ft - 666ft)
		   101XXXXX -7.5dB CSU 110XXXXX -15.0dB CSU 111XXXXX -22.5dB CSU txlevel 0000 TX on 
		   DSX-1 ( 0ft - 133ft) / 0dB CSU or 75 Ohm normal 0001 TX on DSX-1 (133ft - 266ft) 
		   or 120 Ohm normal 0010 TX on DSX-1 (266ft - 399ft) or (invalid) 0011 TX on DSX-1 
		   (399ft - 533ft) or (invalid) 0100 TX on DSX-1 (533ft - 666ft) or 75 Ohm high RL
		   0101 TX on -7.5dB CSU or 120 Ohm high RL 0110 TX on -15.0dB CSU or (invalid)
		   0111 TX on -22.5dB CSU or (invalid) 1000 TX off 0dB Gain monitoring mode 1001 TX 
		   off 20dB Gain monitoring mode 1010 TX off 26dB Gain monitoring mode 1011 TX off
		   32dB Gain monitoring mode 1100 (invalid) 1101 (invalid) 1110 (invalid) 1111
		   (invalid) */

		if (sp->config.iftxlevel < 8) {
			reg7a = 0x00;	/* no gain */
			reg78 = 0x31;	/* 120 Ohm normal, transmitter on, -43dB EGL */
			// reg78 |= ((sp->config.iftxlevel & 0x1) << 5); /* LBO */
		} else {
			/* monitoring mode */
			reg7a = 0x00;	/* no gain */
			reg78 = 0x30;	/* 120 Ohm normal, transmitter off, -43dB EGL */
			reg7a |= ((sp->config.iftxlevel & 0x3) << 3);	/* Linear gain */
		}
		// reg78 &= ~0x01; /* disable transmitter */

		xlb[0x78] = reg78;
		xlb[0x7a] = reg7a;
		break;
	}
	case V400PT:
	case T400P:
	case T400PSS7:
	{
		unsigned char val, reg09, reg7c;
		int japan = (cd->config.ifgtype == SDL_GTYPE_J1);

		xlb[0x2b] = 0x08;	/* Full-on sync required (RCR1) */
		xlb[0x2c] = 0x08;	/* RSYNC is an input (RCR2) */
		xlb[0x35] = 0x10;	/* RBS enable (TCR1) */
		xlb[0x36] = 0x04;	/* TSYNC to be output (TCR2) */
		xlb[0x37] = 0x9c;	/* Tx & Rx Elastic stor, sysclk(s) = 2.048 mhz, loopback
					   controls (CCR1) */

		xlb[0x12] = 0x22;	/* IBCC 5-bit loop up, 3-bit loop down code */
		xlb[0x13] = 0x80;	/* TCD - 10000 */
		xlb[0x14] = 0x80;	/* RUPCD - 10000 */
		xlb[0x15] = 0x80;	/* RDNCD - 100 */

		xlb[0x19] = japan ? 0x80 : 0x00;	/* set japanese mode, no local loop */
		xlb[0x1e] = japan ? 0x80 : 0x00;	/* set japanese mode, no local loop */

		/* Enable F bits pattern */
		switch (sp->config.ifframing) {
		default:
		case SDL_FRAMING_SF:
			val = 0x20;
			break;
		case SDL_FRAMING_ESF:
			val = 0x88;
			break;
		}
		switch (sp->config.ifcoding) {
		default:
		case SDL_CODING_AMI:
			break;
		case SDL_CODING_B8ZS:
			val |= 0x44;
			break;
		}
		xlb[0x38] = val;
		if (sp->config.ifcoding != SDL_CODING_B8ZS)
			xlb[0x7e] = 0x1c;	/* Set FDL register to 0x1c */
		if (sp->config.iftxlevel < 8) {
			/* not monitoring mode */
			reg09 = 0x00;	/* TEST2 no gain */
			reg7c = 0x00;	/* 0dB CSU, transmitters on */
			reg7c |= ((sp->config.iftxlevel & 0x7) << 5);	/* LBO */
		} else {
			/* monitoring mode */
			reg09 = 0x00;	/* TEST2 no gain */
			reg7c = 0x01;	/* 0dB CSU, transmitters off */
			switch (sp->config.iftxlevel & 0x3) {
			case 1:
				reg09 |= 0x72;	/* TEST2 12dB gain */
				break;
			case 2:
			case 3:
				reg09 |= 0x70;	/* TEST2 20db gain */
				break;
			}
		}
		// reg7c |= 0x01; /* disable trasnmitter */

		xlb[0x09] = reg09;
		xlb[0x7c] = reg7c;

#if 0
		if (timeouts) {
			xlb[0x0a] = 0x80;	/* LIRST to reset line interface */
			timeout = jiffies + 100 * HZ / 1000;
			while (jiffies < timeout) ;
		}
#endif
		xlb[0x0a] = 0x30;	/* LIRST bask to normal, Resetting elastic buffers */
		{
			int byte, c;
			unsigned short mask = 0;

			/* establish which channels are clear channel */
			for (c = 0; c < 24; c++) {
				int slot = xp_t1_chan_map[c];

				byte = c >> 3;
				if (!cd->spans[sp->span]->slots[slot]
				    || cd->spans[sp->span]->slots[slot]->sdl.config.
				    iftype != SDL_TYPE_DS0A)
					mask |= 1 << (c % 8);
				if ((c % 8) == 7) {
					xlb[0x39 + byte] = mask;
					mask = 0;
				}
			}
		}
		break;
	}
	case V401PT:
	{
		unsigned char reg04, reg05, reg06, reg07, reg78, reg7a;

		/* There are three other synchronization modes: 0x00 TCLK only, 0x02 switch to RCLK 
		   if TCLK fails, 0x04 external clock, 0x06 loop.  And then 0x80 selects the
		   TSYSCLK pin instead of the MCLK pin when in external clock mode. */
		/* Hmmm. tor3 driver has TCLK only for T1, but RCLK if TCLK fails for E1! */
		xlb[0x70] = 0x06;	/* LOTCMC into TCSS0 */
		/* IOCR1.0=0 Output data format is bipolar, IOCR1.1=1 TSYNC is an output, IOCR1.4=1 
		   RSYNC is an input (elastic store). */
		xlb[0x01] = 0x12;	/* RSIO + 1 is from O12.0 */
		xlb[0x02] = 0x03;	/* RSYSCLK/TSYSCLK 8.192MHz IBO */
		xlb[0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
#if 0
		/* We should really reset the elastic store after reset like so: */
		xlb[0x4f] = 0x55;	/* RES/TES (elastic store) reset */
		xlb[0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
		/* And even align it like so: */
		xlb[0x4f] = 0x99;	/* RES/TES (elastic store) reset */
		xlb[0x4f] = 0x11;	/* RES/TES (elastic store) enabled */
#endif
		xlb[0xb6] = 0x22;	/* IBCC 5-bit loop up, 3-bit loop down code */
		xlb[0xb7] = 0x80;	/* TCD1 - 10000 loop up code (for now) */
		xlb[0xb8] = 0x00;	/* TCD2 don't care */
		xlb[0xb9] = 0x80;	/* RUPCD1 - 10000 receive loop up code */
		xlb[0xba] = 0x00;	/* RUPCD2 don't care */
		xlb[0xbb] = 0x80;	/* RDNCD1 - 100 receive loop down code */
		xlb[0xbc] = 0x00;	/* RDNCD2 don't card */

		reg04 = 0x00;
		reg05 = 0x10;	/* TSSE */
		reg06 = 0x00;
		reg07 = 0x00;

		switch (sp->config.ifgcrc) {
		default:
			sp->config.ifgcrc = SDL_GCRC_CRC6;
		case SDL_GCRC_CRC6:
			break;
		case SDL_GCRC_CRC6J:
			reg04 |= 0x02;
			reg05 |= 0x80;
			break;
		}
		switch (sp->config.ifframing) {
		default:
			sp->config.ifframing = SDL_FRAMING_ESF;
		case SDL_FRAMING_ESF:
			reg04 |= 0x40;
			reg07 |= 0x04;
			break;
		case SDL_FRAMING_SF:	/* D4 */
			break;
		}
		switch (sp->config.ifcoding) {
		default:
			sp->config.ifcoding = SDL_CODING_B8ZS;
		case SDL_CODING_B8ZS:
			reg04 |= 0x20;
			reg06 |= 0x80;
			break;
		case SDL_CODING_AMI:
			break;
		}
		xlb[0x04] = reg04;	/* RESF RB8ZS RCRC6J */
		xlb[0x05] = reg05;	/* TSSE TCRC6J */
		xlb[0x06] = reg06;	/* TB8ZS */
		xlb[0x07] = reg07;	/* TESF */

#if 0
		xlb[0x03] = 0x08 | 0x01;	/* SYNCC/SYNCE, RESYNC */
		xlb[0x03] = 0x08 | 0x00;	/* SYNCC/SYNCE */
#endif

		xlb[0x40] = 0x00;	/* RCCS, TCCS set to zero for T1 */

#if 0
		if (timeouts) {
			xlb[0x79] = 0x58;	/* JACLK on for T1 (and reset) */
			timeout = jiffies + 100 * HZ / 1000;
			while (jiffies < timeout) ;
		}
#endif
		xlb[0x79] = 0x18;	/* JACLK on for T1 */
		xlb[0x7d] = 0x00;	/* Automatic gain control */

		if (sp->config.iftxlevel < 8) {
			reg7a = 0x00;	/* no gain */
			reg78 = 0x01;	/* 0dB CSU, trasnmitter on, -36dB EGL */
			reg78 |= ((sp->config.iftxlevel & 0x7) << 5);	/* LBO */
		} else {
			/* monitoring mode */
			reg7a = 0x00;	/* no gain */
			reg78 = 0x00;	/* 0db CSU, transmitter off, -36dB EGL */
			reg7a |= ((sp->config.iftxlevel & 0x3) << 3);	/* Linear gain */
		}
		// reg78 &= ~0x01; /* disable transmitter */

		xlb[0x78] = reg78;
		xlb[0x7b] = 0x0a;	/* 100 ohm, MCLK 2.048 MHz */
		xlb[0x7a] = reg7a;

		{
			int byte, c;
			unsigned short mask = 0;

			/* establish which channels are clear channel */
			for (c = 0; c < 24; c++) {
				int slot = xp_t1_chan_map[c];

				byte = c >> 3;
				if (sp->slots[slot]
				    && sp->slots[slot]->sdl.config.iftype == SDL_TYPE_DS0A)
					mask |= 1 << (c % 8);
				if ((c % 8) == 7) {
					xlb[0x08 + byte] = mask;
					mask = 0;
				}
			}
		}
		break;
	}
	}
	// sp->config.ifflags = SDL_IF_UP;
	return (0);
}

STATIC noinline __unlikely int
xp_span_reconfig(struct cd *cd, int span)
{
	struct sp *sp = cd->spans[span];
	volatile unsigned char *xlb = &cd->xlb[span << 8];

	switch (cd->board) {
	case V400PE:
	case E400P:
	case E400PSS7:
	{
		uint8_t ccr1 = 0, tcr1 = 0, reg18 = 0, regac = 0;

		switch (sp->config.ifframing) {
		default:
		case SDL_FRAMING_CCS:
			ccr1 |= 0x08;
			break;
		case SDL_FRAMING_CAS:
			tcr1 |= 0x20;
			break;
		}
		switch (sp->config.ifcoding) {
		default:
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
		xlb[0x12] = tcr1;
		xlb[0x14] = ccr1;

		if (sp->config.iftxlevel < 8) {
			/* not monitoring mode */
			regac = 0x00;	/* TEST3 no gain */
			reg18 = 0x00;	/* 75 Ohm, Normal, transmitter on */
			reg18 |= ((sp->config.iftxlevel & 0x7) << 5);	/* LBO */
		} else {
			/* monitoring mode */
			regac = 0x00;	/* TEST3 no gain */
			reg18 = 0x01;	/* 75 Ohm norm, transmitter off */
			switch (sp->config.iftxlevel & 0x3) {
			case 0:
				break;
			case 1:
				regac |= 0x72;	/* TEST3 12dB gain */
				break;
			case 2:
			case 3:
				regac |= 0x70;	/* TEST3 30dB gain */
				break;
			}
		}

		xlb[0xac] = regac;
		xlb[0x18] = reg18;
		break;
	}
	case V401PE:
	{
		u_char reg33, reg35, reg40, reg70, reg78, reg7a;

		switch (sp->config.ifclock) {
		default:
			sp->config.ifclock = SDL_CLOCK_LOOP;
		case SDL_CLOCK_LOOP:
			reg70 = 0x06;	/* Use the signal present at RCLK as the transmit clokc.
					   The TCLK pin is ignored. */
			break;
		case SDL_CLOCK_INT:
			reg70 = 0x00;	/* The TCLK pin is always the source of transmit clock. */
			break;
		case SDL_CLOCK_MASTER:
			reg70 = 0x04;	/* Use the scaled signal present at MCLK as the transmit
					   clock.  The TCLK pin is ignored. */
			break;
		case SDL_CLOCK_EXT:
			reg70 = 0x84;	/* Use the scaled signal present at TSYSCLK as the transmit
					   clock.  The TCLK pin is ignored. */
			break;
		case SDL_CLOCK_SLAVE:
			reg70 = 0x02;	/* Switch to the clock present at RCLK when the signal at
					   the TCLK pin fails to transition after 1 channel time. */
			break;
		}

		/* There are four synchronization modes: 0x00 TCLK only, 0x02 switch to RCLK if
		   TCLK fails, 0x04 external clock, 0x06 loop.  And then 0x80 selects the TSYSCLK
		   pin instead of the MCLK pin when in external clock mode. */
		/* Hmmm. tor3 driver has TCLK only for T1, but RCLK if TCLK fails for E1! */
		xlb[0x70] = reg70;	/* LOTCMC into TCSS0 */

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
		xlb[0x35] = reg35;	/* TSiS, TCRC4 (from 014.4), THDB3 (from O14.6) */
		xlb[0x40] = reg40;	/* RCCS, TCCS */
		xlb[0x33] = reg33 | 0x01;	/* RCR4, RHDB3, RSM, SYNCE, RESYNC */
		xlb[0x33] = reg33 | 0x00;	/* RCR4, RHDB3, RSM, SYNCE */

		if (sp->config.iftxlevel < 8) {
			reg7a = 0x00;	/* no gain */
			reg78 = 0x31;	/* 120 Ohm normal, transmitter on, -43dB EGL */
			// reg78 |= ((sp->config.iftxlevel & 0x1) << 5); /* LBO */
		} else {
			/* monitoring mode */
			reg7a = 0x00;	/* no gain */
			reg78 = 0x30;	/* 120 Ohm normal, transmitter off, -43dB EGL */
			reg7a |= ((sp->config.iftxlevel & 0x3) << 3);	/* Linear gain */
		}

		xlb[0x78] = reg78;
		xlb[0x7a] = reg7a;
		break;
	}
	case V400PT:
	case T400P:
	case T400PSS7:
	{
		unsigned char val, reg09, reg7c;

		switch (sp->config.ifframing) {
		default:
		case SDL_FRAMING_SF:
			val = 0x20;
			break;
		case SDL_FRAMING_ESF:
			val = 0x88;
			break;
		}
		switch (sp->config.ifcoding) {
		default:
		case SDL_CODING_AMI:
			break;
		case SDL_CODING_B8ZS:
			val |= 0x44;
			break;
		}
		xlb[0x38] = val;
		if (sp->config.ifcoding != SDL_CODING_B8ZS)
			xlb[0x7e] = 0x1c;	/* Set FDL register to 0x1c */
		if (sp->config.iftxlevel < 8) {
			/* not monitoring mode */
			reg09 = 0x00;	/* TEST2 no gain */
			reg7c = 0x00;	/* 0dB CSU, transmitters on */
			reg7c |= ((sp->config.iftxlevel & 0x7) << 5);	/* LBO */
		} else {
			/* monitoring mode */
			reg09 = 0x00;	/* TEST2 no gain */
			reg7c = 0x01;	/* 0dB CSU, transmitters off */
			switch (sp->config.iftxlevel & 0x3) {
			case 1:
				reg09 |= 0x72;	/* TEST2 12dB gain */
				break;
			case 2:
			case 3:
				reg09 |= 0x70;	/* TEST2 20db gain */
				break;
			}
		}

		xlb[0x09] = reg09;
		xlb[0x7c] = reg7c;

		{
			int byte, c;
			unsigned short mask = 0;

			/* establish which channels are clear channel */
			for (c = 0; c < 24; c++) {
				int slot = xp_t1_chan_map[c];

				byte = c >> 3;
				if (!sp->slots[slot]
				    || sp->slots[slot]->sdl.config.iftype != SDL_TYPE_DS0A)
					mask |= 1 << (c % 8);
				if ((c % 8) == 7) {
					xlb[0x39 + byte] = mask;
					mask = 0;
				}
			}
		}
		break;
	}
	case V401PT:
	{
		unsigned char reg04, reg05, reg06, reg07, reg70, reg78, reg7a;

		switch (sp->config.ifclock) {
		default:
			sp->config.ifclock = SDL_CLOCK_LOOP;
		case SDL_CLOCK_LOOP:
			reg70 = 0x06;	/* Use the signal present at RCLK as the transmit clokc.
					   The TCLK pin is ignored. */
			break;
		case SDL_CLOCK_INT:
			reg70 = 0x00;	/* The TCLK pin is always the source of transmit clock. */
			break;
		case SDL_CLOCK_MASTER:
			reg70 = 0x04;	/* Use the scaled signal present at MCLK as the transmit
					   clock.  The TCLK pin is ignored. */
			break;
		case SDL_CLOCK_EXT:
			reg70 = 0x84;	/* Use the scaled signal present at TSYSCLK as the transmit
					   clock.  The TCLK pin is ignored. */
			break;
		case SDL_CLOCK_SLAVE:
			reg70 = 0x02;	/* Switch to the clock present at RCLK when the signal at
					   the TCLK pin fails to transition after 1 channel time. */
			break;
		}
		xlb[0x70] = reg70;

		reg04 = 0x00;
		reg05 = 0x10;	/* TSSE */
		reg06 = 0x00;
		reg07 = 0x00;

		switch (sp->config.ifgcrc) {
		default:
			sp->config.ifgcrc = SDL_GCRC_CRC6;
		case SDL_GCRC_CRC6:
			break;
		case SDL_GCRC_CRC6J:
			reg04 |= 0x02;
			reg05 |= 0x80;
			break;
		}
		switch (sp->config.ifframing) {
		default:
			sp->config.ifframing = SDL_FRAMING_ESF;
		case SDL_FRAMING_ESF:
			reg04 |= 0x40;
			reg07 |= 0x04;
			break;
		case SDL_FRAMING_SF:	/* D4 */
			break;
		}
		switch (sp->config.ifcoding) {
		default:
			sp->config.ifcoding = SDL_CODING_B8ZS;
		case SDL_CODING_B8ZS:
			reg04 |= 0x20;
			reg06 |= 0x80;
			break;
		case SDL_CODING_AMI:
			break;
		}
		xlb[0x04] = reg04;	/* RESF RB8ZS RCRC6J */
		xlb[0x05] = reg05;	/* TSSE TCRC6J */
		xlb[0x06] = reg06;	/* TB8ZS */
		xlb[0x07] = reg07;	/* TESF */

		xlb[0x03] = 0x08 | 0x01;	/* SYNCC/SYNCE, RESYNC */
		xlb[0x03] = 0x08 | 0x00;	/* SYNCC/SYNCE */

		if (sp->config.iftxlevel < 8) {
			reg7a = 0x00;	/* no gain */
			reg78 = 0x01;	/* 0dB CSU, trasnmitter on */
			reg78 |= ((sp->config.iftxlevel & 0x7) << 5);	/* LBO */
		} else {
			/* monitoring mode */
			reg7a = 0x00;	/* no gain */
			reg78 = 0x00;	/* 0db CSU, transmitter off */
			reg7a |= ((sp->config.iftxlevel & 0x3) << 3);	/* Linear gain */
		}

		xlb[0x78] = reg78;
		xlb[0x7a] = reg7a;

		{
			int byte, c;
			unsigned short mask = 0;

			/* establish which channels are clear channel */
			for (c = 0; c < 24; c++) {
				int slot = xp_t1_chan_map[c];

				byte = c >> 3;
				if (sp->slots[slot]
				    && sp->slots[slot]->sdl.config.iftype == SDL_TYPE_DS0A)
					mask |= 1 << (c % 8);
				if ((c % 8) == 7) {
					xlb[0x08 + byte] = mask;
					mask = 0;
				}
			}
		}
		break;
	}
	default:
		swerr();
		break;
	}
	return (0);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  Timers
 *
 *  ------------------------------------------------------------------------
 */
enum { tall, t1, t2, t3, t4, t5, t6, t7, t8, t9 };

static noinline fastcall void
xp_stop_timer_t1(struct xp *xp)
{
	printd(("%s: %p: -> T1 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sl.timers.t1);
}
static noinline fastcall void
xp_start_timer_t1(struct xp *xp)
{
	printd(("%s: %p: -> T1 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sl.config.t1), xp->sl.config.t1, (uint) HZ));
	mi_timer_MAC(xp->sl.timers.t1, xp->sl.config.t1);
}
static noinline fastcall void
xp_stop_timer_t2(struct xp *xp)
{
	printd(("%s: %p: -> T2 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sl.timers.t2);
}
static noinline fastcall void
xp_start_timer_t2(struct xp *xp)
{
	printd(("%s: %p: -> T2 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sl.config.t2), xp->sl.config.t2, (uint) HZ));
	mi_timer_MAC(xp->sl.timers.t2, xp->sl.config.t2);
}
static noinline fastcall void
xp_stop_timer_t3(struct xp *xp)
{
	printd(("%s: %p: -> T3 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sl.timers.t3);
}
static noinline fastcall void
xp_start_timer_t3(struct xp *xp)
{
	printd(("%s: %p: -> T3 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sl.config.t3), xp->sl.config.t3, (uint) HZ));
	mi_timer_MAC(xp->sl.timers.t3, xp->sl.config.t3);
}
static noinline fastcall void
xp_stop_timer_t4(struct xp *xp)
{
	printd(("%s: %p: -> T4 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sl.timers.t4);
}
static noinline fastcall void
xp_start_timer_t4(struct xp *xp)
{
	printd(("%s: %p: -> T4 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sl.statem.t4v), xp->sl.statem.t4v, (uint) HZ));
	mi_timer_MAC(xp->sl.timers.t4, xp->sl.statem.t4v);
}
static inline fastcall __hot_out void
xp_stop_timer_t5(struct xp *xp)
{
	printd(("%s: %p: -> T5 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sl.timers.t5);
}
static inline fastcall __hot_out void
xp_start_timer_t5(struct xp *xp)
{
	printd(("%s: %p: -> T5 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sl.config.t5), xp->sl.config.t5, (uint) HZ));
	mi_timer_MAC(xp->sl.timers.t5, xp->sl.config.t5);
}
static inline fastcall __hot_in void
xp_stop_timer_t6(struct xp *xp)
{
	printd(("%s: %p: -> T6 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sl.timers.t6);
}
static inline fastcall __hot_in void
xp_start_timer_t6(struct xp *xp)
{
	printd(("%s: %p: -> T6 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sl.config.t6), xp->sl.config.t6, (uint) HZ));
	mi_timer_MAC(xp->sl.timers.t6, xp->sl.config.t6);
}
static inline fastcall __hot_in void
xp_stop_timer_t7(struct xp *xp)
{
	printd(("%s: %p: -> T7 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sl.timers.t7);
}
static inline fastcall __hot_in void
xp_start_timer_t7(struct xp *xp)
{
	printd(("%s: %p: -> T7 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sl.config.t7), xp->sl.config.t7, (uint) HZ));
	mi_timer_MAC(xp->sl.timers.t7, xp->sl.config.t7);
}
static inline fastcall __hot_in void
xp_stop_timer_t8(struct xp *xp)
{
	printd(("%s: %p: -> T8 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sdt.timers.t8);
}
static inline fastcall __hot_in void
xp_start_timer_t8(struct xp *xp)
{
	printd(("%s: %p: -> T8 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sdt.config.t8), xp->sdt.config.t8, (uint) HZ));
	mi_timer_MAC(xp->sdt.timers.t8, xp->sdt.config.t8);
}

#if 0
static void
xp_stop_timer_t9(struct xp *xp)
{
	printd(("%s: %p: -> T9 STOP <-\n", DRV_NAME, xp));
	mi_timer_stop(xp->sdl.timers.t9);
}
static void
xp_start_timer_t9(struct xp *xp)
{
	printd(("%s: %p: -> T9 START <- (%lu hz, %u msec, HZ is %u)\n", DRV_NAME, xp,
		drv_msectohz(xp->sdl.config.t9), xp->sdl.config.t9, (uint) HZ));
	mi_timer_MAC(xp->sdl.timers.t9, xp->sdl.config.t9);
}
#endif

STATIC inline fastcall __hot void
xp_timer_stop(struct xp *xp, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case t1:
		xp_stop_timer_t1(xp);
		if (single)
			break;
		/* fall through */
	case t2:
		xp_stop_timer_t2(xp);
		if (single)
			break;
		/* fall through */
	case t3:
		xp_stop_timer_t3(xp);
		if (single)
			break;
		/* fall through */
	case t4:
		xp_stop_timer_t4(xp);
		if (single)
			break;
		/* fall through */
	case t5:
		xp_stop_timer_t5(xp);
		if (single)
			break;
		/* fall through */
	case t6:
		xp_stop_timer_t6(xp);
		if (single)
			break;
		/* fall through */
	case t7:
		xp_stop_timer_t7(xp);
		if (single)
			break;
		/* fall through */
	case t8:
		xp_stop_timer_t8(xp);
		if (single)
			break;
		/* fall through */
#if 0
	case t9:
		xp_stop_timer_t9(xp);
		if (single)
			break;
		/* fall through */
#endif
		break;
	default:
		swerr();
		break;
	}
}
STATIC inline fastcall __hot void
xp_timer_start(struct xp *xp, const uint t)
{
	xp_timer_stop(xp, t);
	switch (t) {
	case t1:
		xp_start_timer_t1(xp);
		break;
	case t2:
		xp_start_timer_t2(xp);
		break;
	case t3:
		xp_start_timer_t3(xp);
		break;
	case t4:
		xp_start_timer_t4(xp);
		break;
	case t5:
		xp_start_timer_t5(xp);
		break;
	case t6:
		xp_start_timer_t6(xp);
		break;
	case t7:
		xp_start_timer_t7(xp);
		break;
	case t8:
		xp_start_timer_t8(xp);
		break;
#if 0
	case t9:
		xp_start_timer_t9(xp);
		break;
#endif
	default:
		swerr();
		break;
	}
}

STATIC noinline __unlikely void
xp_free_timers(struct xp *xp)
{
	mblk_t *tp;

	if ((tp = XCHG(&xp->sl.timers.t1, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&xp->sl.timers.t2, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&xp->sl.timers.t3, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&xp->sl.timers.t4, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&xp->sl.timers.t5, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&xp->sl.timers.t6, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&xp->sl.timers.t7, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&xp->sdt.timers.t8, NULL)))
		mi_timer_free(tp);
#if 0
	if ((tp = XCHG(&xp->sdl.timers.t9, NULL)))
		mi_timer_free(tp);
#endif
}
STATIC noinline __unlikely int
xp_alloc_timers(struct xp *xp)
{
	mblk_t *tp;

	/* SDL timer allocation */
#if 0
	if (!(tp = xp->sdl.timers.t9 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t9;
#endif
	/* SDT timer allocation */
	if (!(tp = xp->sdt.timers.t8 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t8;
	/* SL timer allocation */
	if (!(tp = xp->sl.timers.t7 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t7;
	if (!(tp = xp->sl.timers.t6 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t6;
	if (!(tp = xp->sl.timers.t5 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t5;
	if (!(tp = xp->sl.timers.t4 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t4;
	if (!(tp = xp->sl.timers.t3 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t3;
	if (!(tp = xp->sl.timers.t2 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t2;
	if (!(tp = xp->sl.timers.t1 = mi_timer_alloc_MAC(xp->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t1;
	return (0);
      enobufs:
	xp_free_timers(xp);
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Duration Statistics
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC noinline fastcall __unlikely void
sl_is_stats(queue_t *q)
{
	struct xp *xp = XP_PRIV(q);

	if (xp->sl.stamp.sl_dur_unavail)
		xp->sl.stats.sl_dur_unavail += jiffies - xchg(&xp->sl.stamp.sl_dur_unavail, 0);
	if (xp->sl.stamp.sl_dur_unavail_rpo)
		xp->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&xp->sl.stamp.sl_dur_unavail_rpo, 0);
	if (xp->sl.stamp.sl_dur_unavail_failed)
		xp->sl.stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&xp->sl.stamp.sl_dur_unavail_failed, 0);
	xp->sl.stamp.sl_dur_in_service = jiffies;
}
STATIC noinline fastcall __unlikely void
sl_oos_stats(queue_t *q)
{
	struct xp *xp = XP_PRIV(q);

	if (xp->sl.stamp.sl_dur_in_service)
		xp->sl.stats.sl_dur_in_service +=
		    jiffies - xchg(&xp->sl.stamp.sl_dur_in_service, 0);
	if (xp->sl.stamp.sl_dur_unavail_rpo)
		xp->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&xp->sl.stamp.sl_dur_unavail_rpo, 0);
	if (xp->sl.stamp.sl_dur_unavail_failed)
		xp->sl.stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&xp->sl.stamp.sl_dur_unavail_failed, 0);
	xp->sl.stamp.sl_dur_unavail = jiffies;
}
STATIC noinline fastcall __unlikely void
sl_rpo_stats(queue_t *q)
{
	struct xp *xp = XP_PRIV(q);

	if (xp->sl.stamp.sl_dur_unavail_rpo)
		xp->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&xp->sl.stamp.sl_dur_unavail_rpo, 0);
}
STATIC noinline fastcall __unlikely void
sl_rpr_stats(queue_t *q)
{
	struct xp *xp = XP_PRIV(q);

	if (xp->sl.stamp.sl_dur_unavail_rpo)
		xp->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&xp->sl.stamp.sl_dur_unavail_rpo, 0);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  SL State Machines
 *
 *  -------------------------------------------------------------------------
 */
#define SN_OUTSIDE(lower,middle,upper) \
	(  ( (lower) <= (upper) ) \
	   ? ( ( (middle) < (lower) ) || ( (middle) > (upper) ) ) \
	   : ( ( (middle) < (lower) ) && ( (middle) > (upper) ) ) \
	   )

/*
 *  -----------------------------------------------------------------------
 *
 *  STATE MACHINES:- The order of the state machine primitives below may seem somewhat disorganized at first
 *  glance; however, they have been ordered by dependency because they are all inline functions.  You see, the L2
 *  state machine does not required multiple threading because there is never a requirement to invoke the
 *  individual state machines concurrently.  This works out good for the driver, because a primitive action
 *  expands inline to the necessary procedure, while the source still takes the appearance of the SDL diagrams in
 *  the SS7 specification for inspection and debugging.
 *
 *  -----------------------------------------------------------------------
 */

#define sl_cc_stop sl_cc_normal
STATIC noinline fastcall void
sl_cc_normal(struct xp *xp, queue_t *q)
{
	xp_timer_stop(xp, t5);
	xp->sl.statem.cc_state = SL_STATE_IDLE;
}

STATIC noinline fastcall void
sl_rc_stop(struct xp *xp, queue_t *q)
{
	sl_cc_normal(xp, q);
	xp->sl.statem.rc_state = SL_STATE_IDLE;
}

STATIC noinline fastcall void
sl_aerm_stop(struct xp *xp, queue_t *q)
{
	xp->sdt.statem.aerm_state = SDT_STATE_IDLE;
	xp->sdt.statem.Ti = xp->sdt.config.Tin;
}

STATIC noinline fastcall void
sl_iac_stop(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.iac_state != SL_STATE_IDLE) {
		xp_timer_stop(xp, t3);
		xp_timer_stop(xp, t2);
		xp_timer_stop(xp, t4);
		sl_aerm_stop(xp, q);
		xp->sl.statem.emergency = 0;
		xp->sl.statem.iac_state = SL_STATE_IDLE;
	}
}

STATIC noinline fastcall void
sl_txc_send_sios(struct xp *xp, queue_t *q)
{
	xp_timer_stop(xp, t7);
	if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
	    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88))
		xp_timer_stop(xp, t6);
	xp->sl.statem.lssu_available = 1;
	xp->sl.statem.tx.sio = LSSU_SIOS;
}

STATIC noinline fastcall void
sl_poc_stop(struct xp *xp, queue_t *q)
{
	xp->sl.statem.poc_state = SL_STATE_IDLE;
}

STATIC noinline fastcall void
sl_eim_stop(struct xp *xp, queue_t *q)
{
	xp->sdt.statem.eim_state = SDT_STATE_IDLE;
	xp_timer_stop(xp, t8);
}

STATIC noinline fastcall void
sl_suerm_stop(struct xp *xp, queue_t *q)
{
	sl_eim_stop(xp, q);
	xp->sdt.statem.suerm_state = SDT_STATE_IDLE;
}

STATIC noinline fastcall int
sl_lsc_link_failure(struct xp *xp, queue_t *q, sl_ulong reason)
{
	int err;

	if (xp->sl.statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
		if ((err = sl_out_of_service_ind(xp, q, reason)))
			return (err);
		xp->sl.statem.failure_reason = reason;
		sl_iac_stop(xp, q);	/* ok if not aligning */
		xp_timer_stop(xp, t1);	/* ok if not running */
		sl_suerm_stop(xp, q);	/* ok if not running */
		sl_rc_stop(xp, q);
		ctrace(sl_txc_send_sios(xp, q));
		sl_poc_stop(xp, q);	/* ok if not ITUT */
		xp->sl.statem.emergency = 0;
		xp->sl.statem.local_processor_outage = 0;
		xp->sl.statem.remote_processor_outage = 0;	/* ok if not ANSI */
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
	return (QR_DONE);
}

STATIC noinline fastcall void
sl_txc_send_sib(struct xp *xp, queue_t *q)
{
	xp->sl.statem.tx.sio = LSSU_SIB;
	xp->sl.statem.lssu_available = 1;
}

STATIC noinline fastcall void
sl_txc_send_sipo(struct xp *xp, queue_t *q)
{
	xp_timer_stop(xp, t7);
	if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
	    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88))
		xp_timer_stop(xp, t6);
	xp->sl.statem.tx.sio = LSSU_SIPO;
	xp->sl.statem.lssu_available = 1;
}

STATIC noinline fastcall void
sl_txc_send_sio(struct xp *xp, queue_t *q)
{
	xp->sl.statem.tx.sio = LSSU_SIO;
	xp->sl.statem.lssu_available = 1;
}

STATIC noinline fastcall void
sl_txc_send_sin(struct xp *xp, queue_t *q)
{
	xp->sl.statem.tx.sio = LSSU_SIN;
	xp->sl.statem.lssu_available = 1;
}

STATIC noinline fastcall void
sl_txc_send_sie(struct xp *xp, queue_t *q)
{
	xp->sl.statem.tx.sio = LSSU_SIE;
	xp->sl.statem.lssu_available = 1;
}

STATIC inline fastcall __hot_write void
sl_txc_send_msu(struct xp *xp, queue_t *q)
{
	if (xp->sl.rtb.q_count)
		xp_timer_start(xp, t7);
	xp->sl.statem.msu_inhibited = 0;
	xp->sl.statem.lssu_available = 0;
}

STATIC noinline fastcall void
sl_txc_send_fisu(struct xp *xp, queue_t *q)
{
	xp_timer_stop(xp, t7);
	if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
	    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88))
		if (!(xp->option.popt & SS7_POPT_PCR))
			xp_timer_stop(xp, t6);
	xp->sl.statem.msu_inhibited = 1;
	xp->sl.statem.lssu_available = 0;
}

STATIC inline fastcall void
sl_txc_fsnx_value(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.tx.X.fsn != xp->sl.statem.rx.X.fsn)
		xp->sl.statem.tx.X.fsn = xp->sl.statem.rx.X.fsn;
}

STATIC noinline fastcall void
sl_txc_nack_to_be_sent(struct xp *xp, queue_t *q)
{
	xp->sl.statem.tx.N.bib = xp->sl.statem.tx.N.bib ? 0 : xp->sl.statem.ib_mask;
}

STATIC noinline fastcall __unlikely int
sl_lsc_rtb_cleared(struct xp *xp, queue_t *q)
{
	int err;

	if (xp->sl.statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		xp->sl.statem.remote_processor_outage = 0;
		if (xp->sl.statem.local_processor_outage)
			return (QR_DONE);
		if ((err = sl_remote_processor_recovered_ind(xp, q)))
			return (err);
		if ((err = sl_rtb_cleared_ind(xp, q)))
			return (err);
		sl_txc_send_msu(xp, q);
		xp->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
	}
	return (QR_DONE);
}

STATIC fastcall __hot_write void sl_check_congestion(struct xp *xp, queue_t *q);

STATIC inline fastcall __hot void
sl_txc_bsnr_and_bibr(struct xp *xp, queue_t *q)
{
	int pcr = xp->option.popt & SS7_POPT_PCR;

	xp->sl.statem.tx.R.bsn = xp->sl.statem.rx.R.bsn;
	xp->sl.statem.tx.R.bib = xp->sl.statem.rx.R.bib;
	if (xp->sl.statem.clear_rtb) {
		bufq_purge(&xp->sl.rtb);
		xp->sl.statem.Ct = 0;
		sl_check_congestion(xp, q);
		xp->sl.statem.tx.F.fsn = (xp->sl.statem.tx.R.bsn + 1) & xp->sl.statem.sn_mask;
		xp->sl.statem.tx.L.fsn = xp->sl.statem.tx.R.bsn;
		xp->sl.statem.tx.N.fsn = xp->sl.statem.tx.R.bsn;
		xp->sl.statem.tx.N.fib = xp->sl.statem.tx.R.bib;
		xp->sl.statem.Z = (xp->sl.statem.tx.R.bsn + 1) & xp->sl.statem.sn_mask;
		xp->sl.statem.z_ptr = NULL;
		/* FIXME: handle error return */
		sl_lsc_rtb_cleared(xp, q);
		xp->sl.statem.clear_rtb = 0;
		xp->sl.statem.rtb_full = 0;
		return;
	}
	if (xp->sl.statem.tx.F.fsn != ((xp->sl.statem.tx.R.bsn + 1) & xp->sl.statem.sn_mask)) {
		if (xp->sl.statem.sib_received) {
			xp->sl.statem.sib_received = 0;
			xp_timer_stop(xp, t6);
		}
		do {
			freemsg(bufq_dequeue(&xp->sl.rtb));
			xp->sl.statem.Ct--;
			xp->sl.statem.tx.F.fsn =
			    (xp->sl.statem.tx.F.fsn + 1) & xp->sl.statem.sn_mask;
		} while (xp->sl.statem.tx.F.fsn !=
			 ((xp->sl.statem.tx.R.bsn + 1) & xp->sl.statem.sn_mask));
		sl_check_congestion(xp, q);
		if (xp->sl.rtb.q_count == 0) {
			xp_timer_stop(xp, t7);
		} else {
			xp_timer_start(xp, t7);
		}
		if (!pcr
		    || (xp->sl.rtb.q_msgs < xp->sl.config.N1
			&& xp->sl.rtb.q_count < xp->sl.config.N2))
			xp->sl.statem.rtb_full = 0;
		if (SN_OUTSIDE(xp->sl.statem.tx.F.fsn, xp->sl.statem.Z, xp->sl.statem.tx.L.fsn)
		    || !xp->sl.rtb.q_count) {
			xp->sl.statem.Z = xp->sl.statem.tx.F.fsn;
			xp->sl.statem.z_ptr = xp->sl.rtb.q_head;
		}
	}
	if (pcr)
		return;
	if (xp->sl.statem.tx.N.fib != xp->sl.statem.tx.R.bib) {
		if (xp->sl.statem.sib_received) {
			xp->sl.statem.sib_received = 0;
			xp_timer_stop(xp, t6);
		}
		xp->sl.statem.tx.N.fib = xp->sl.statem.tx.R.bib;
		xp->sl.statem.tx.N.fsn = (xp->sl.statem.tx.F.fsn - 1) & xp->sl.statem.sn_mask;
		if ((xp->sl.statem.z_ptr = xp->sl.rtb.q_head) != NULL)
			xp->sl.statem.retrans_cycle = 1;
		return;
	}
}

STATIC noinline fastcall void
sl_txc_sib_received(struct xp *xp, queue_t *q)
{
	/* FIXME: consider these variations for all */
	if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
	    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88))
		if (xp->sl.statem.lssu_available && xp->sl.statem.tx.sio != LSSU_SIB)
			return;
	if (xp->option.pvar != SS7_PVAR_ITUT_93)
		if (!xp->sl.rtb.q_count)
			return;
	if (!xp->sl.statem.sib_received) {
		xp_timer_start(xp, t6);
		xp->sl.statem.sib_received = 1;
	}
	xp_timer_start(xp, t7);
}

STATIC noinline fastcall __unlikely void
sl_txc_clear_rtb(struct xp *xp, queue_t *q)
{
	bufq_purge(&xp->sl.rtb);
	xp->sl.statem.Ct = 0;
	xp->sl.statem.clear_rtb = 1;
	xp->sl.statem.rtb_full = 0;	/* added */
	/* FIXME: should probably follow more of the ITUT flush_buffers stuff like reseting Z and
	   FSNF, FSNL, FSNT. */
	sl_check_congestion(xp, q);
}

STATIC noinline fastcall __unlikely void
sl_txc_clear_tb(struct xp *xp, queue_t *q)
{
	bufq_purge(&xp->sl.tb);
	flushq(xp->iq, FLUSHDATA);
	xp->sl.statem.Cm = 0;
	sl_check_congestion(xp, q);
}

STATIC noinline fastcall __unlikely void
sl_txc_flush_buffers(struct xp *xp, queue_t *q)
{
	bufq_purge(&xp->sl.rtb);
	xp->sl.statem.rtb_full = 0;
	xp->sl.statem.Ct = 0;
	bufq_purge(&xp->sl.tb);
	flushq(xp->iq, FLUSHDATA);
	xp->sl.statem.Cm = 0;
	xp->sl.statem.Z = 0;
	xp->sl.statem.z_ptr = NULL;
	/* Z =0 error in ITUT 93 and ANSI */
	xp->sl.statem.Z = xp->sl.statem.tx.F.fsn =
	    (xp->sl.statem.tx.R.bsn + 1) & xp->sl.statem.sn_mask;
	xp->sl.statem.tx.L.fsn = xp->sl.statem.rx.R.bsn;
	xp->sl.statem.rx.T.fsn = xp->sl.statem.rx.R.bsn;
	xp_timer_stop(xp, t7);
	return;
}

STATIC inline fastcall __hot_in void
sl_rc_fsnt_value(struct xp *xp, queue_t *q)
{
	xp->sl.statem.rx.T.fsn = xp->sl.statem.tx.N.fsn;
}

STATIC noinline fastcall __unlikely int
sl_txc_retrieval_request_and_fsnc(struct xp *xp, queue_t *q, sl_ulong fsnc)
{
	mblk_t *mp;
	int err;

	xp->sl.statem.tx.C.fsn = fsnc & (xp->sl.statem.sn_mask);
	/* 
	 *  FIXME: Q.704/5.7.2 states:
	 *
	 *  5.7.2   If a changeover order or acknowledgement containing an unreasonable value of the forward
	 *  sequence number is received, no buffer updating or retrieval is performed, and new traffic is started
	 *  on the alternative signalling link(s).
	 *
	 *  It will be necessary to check FSNC for "reasonableness" here and flush RTB and TB and return
	 *  retrieval-complete indication with a return code of "unreasonable FSNC".
	 *
	 *  (Tell the SIGTRAN working group and M2UA guys about this!)
	 */
	while (xp->sl.rtb.q_count && xp->sl.statem.tx.F.fsn != ((fsnc + 1) & xp->sl.statem.sn_mask)) {
		freemsg(bufq_dequeue(&xp->sl.rtb));
		xp->sl.statem.Ct--;
		xp->sl.statem.tx.F.fsn = (xp->sl.statem.tx.F.fsn + 1) & xp->sl.statem.sn_mask;
	}
	while ((mp = bufq_dequeue(&xp->sl.tb))) {
		xp->sl.statem.Cm--;
		bufq_queue(&xp->sl.rtb, mp);
		xp->sl.statem.Ct++;
		if (!xp->sl.statem.Cm)
			qenable(xp->iq);
	}
	xp->sl.statem.Z = xp->sl.statem.tx.F.fsn =
	    (xp->sl.statem.tx.C.fsn + 1) & xp->sl.statem.sn_mask;
	while ((mp = bufq_dequeue(&xp->sl.rtb))) {
		xp->sl.statem.Ct--;
		if ((err = sl_retrieved_message_ind(xp, q, mp)))
			return (err);
	}
	xp->sl.statem.rtb_full = 0;
	if ((err = sl_retrieval_complete_ind(xp, q)))
		return (err);
	xp->sl.statem.Cm = 0;
	xp->sl.statem.Ct = 0;
	xp->sl.statem.tx.N.fsn = xp->sl.statem.tx.L.fsn = xp->sl.statem.tx.C.fsn;
	return (QR_DONE);
}

STATIC inline fastcall __hot_write void
sl_daedt_fisu(struct xp *xp, queue_t *q, mblk_t *mp)
{
	if (xp->option.popt & SS7_POPT_XSN) {
		*(sl_ushort *) mp->b_wptr = htons(xp->sl.statem.tx.N.bsn | xp->sl.statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(xp->sl.statem.tx.N.fsn | xp->sl.statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = 0;
		mp->b_wptr += sizeof(sl_ushort);
	} else {
		*(sl_uchar *) mp->b_wptr = (xp->sl.statem.tx.N.bsn | xp->sl.statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = (xp->sl.statem.tx.N.fsn | xp->sl.statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = 0;
		mp->b_wptr += sizeof(sl_uchar);
	}
}

STATIC noinline fastcall __hot_write void
sl_daedt_lssu(struct xp *xp, queue_t *q, mblk_t *mp)
{
	if (xp->option.popt & SS7_POPT_XSN) {
		*(sl_ushort *) mp->b_wptr = htons(xp->sl.statem.tx.N.bsn | xp->sl.statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(xp->sl.statem.tx.N.fsn | xp->sl.statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(1);
		mp->b_wptr += sizeof(sl_ushort);
	} else {
		*(sl_uchar *) mp->b_wptr = (xp->sl.statem.tx.N.bsn | xp->sl.statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = (xp->sl.statem.tx.N.fsn | xp->sl.statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = 1;
		mp->b_wptr += sizeof(sl_uchar);
	}
	*(sl_uchar *) mp->b_wptr = (xp->sl.statem.tx.sio);
	mp->b_wptr += sizeof(sl_uchar);
}

STATIC inline fastcall __hot_write void
sl_daedt_msu(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int len = msgdsize(mp);

	if (xp->option.popt & SS7_POPT_XSN) {
		((sl_ushort *) mp->b_rptr)[0] =
		    htons(xp->sl.statem.tx.N.bsn | xp->sl.statem.tx.N.bib);
		((sl_ushort *) mp->b_rptr)[1] =
		    htons(xp->sl.statem.tx.N.fsn | xp->sl.statem.tx.N.fib);
		((sl_ushort *) mp->b_rptr)[2] = htons(len - 6 < 512 ? len - 6 : 511);
	} else {
		((sl_uchar *) mp->b_rptr)[0] = (xp->sl.statem.tx.N.bsn | xp->sl.statem.tx.N.bib);
		((sl_uchar *) mp->b_rptr)[1] = (xp->sl.statem.tx.N.fsn | xp->sl.statem.tx.N.fib);
		((sl_uchar *) mp->b_rptr)[2] = (len - 3 < 64 ? len - 3 : 63);
	}
}

STATIC inline fastcall __hot_out mblk_t *
sl_txc_transmission_request(struct xp *xp, queue_t *q)
{
	mblk_t *mp = NULL;
	int pcr;

	if (xp->sl.statem.txc_state != SL_STATE_IN_SERVICE)
		return (mp);
	pcr = xp->option.popt & SS7_POPT_PCR;
	if (xp->sl.statem.lssu_available) {
		if ((mp = ss7_fast_allocb(&xp_bufpool, 7, BPRI_HI))) {
			if (xp->sl.statem.tx.sio == LSSU_SIB)
				xp->sl.statem.lssu_available = 0;
			xp->sl.statem.tx.N.fsn = xp->sl.statem.tx.L.fsn;
			xp->sl.statem.tx.N.bib = xp->sl.statem.tx.N.bib;
			xp->sl.statem.tx.N.bsn =
			    (xp->sl.statem.tx.X.fsn - 1) & xp->sl.statem.sn_mask;
			xp->sl.statem.tx.N.fib = xp->sl.statem.tx.N.fib;
			sl_daedt_lssu(xp, q, mp);
		}
		return (mp);
	}
	if (xp->sl.statem.msu_inhibited) {
		if ((mp = ss7_fast_allocb(&xp_bufpool, 6, BPRI_HI))) {
			xp->sl.statem.tx.N.fsn = xp->sl.statem.tx.L.fsn;
			xp->sl.statem.tx.N.bib = xp->sl.statem.tx.N.bib;
			xp->sl.statem.tx.N.bsn =
			    (xp->sl.statem.tx.X.fsn - 1) & xp->sl.statem.sn_mask;
			xp->sl.statem.tx.N.fib = xp->sl.statem.tx.N.fib;
			sl_daedt_fisu(xp, q, mp);
		}
		return (mp);
	}
	if (pcr) {
		if ((xp->sl.rtb.q_msgs < xp->sl.config.N1)
		    && (xp->sl.rtb.q_count < xp->sl.config.N2)) {
			xp->sl.statem.forced_retransmission = 0;
			xp->sl.statem.rtb_full = 0;
		}
		if (xp->sl.tb.q_count && xp->sl.statem.rtb_full) {
			xp->sl.statem.forced_retransmission = 1;
		}
	}
	if ((!pcr && xp->sl.statem.retrans_cycle)
	    || (pcr
		&& (xp->sl.statem.forced_retransmission
		    || (!xp->sl.tb.q_count && xp->sl.rtb.q_count)))) {
		mblk_t *bp;

		if ((bp = xp->sl.statem.z_ptr) && !(mp = dupmsg(bp)))
			return (mp);
		if (!bp && pcr) {
			if ((bp = xp->sl.statem.z_ptr = xp->sl.rtb.q_head) && !(mp = dupmsg(bp)))
				return (mp);
			xp->sl.statem.Z = xp->sl.statem.tx.F.fsn;
		}
		if (mp) {
			xp->sl.statem.z_ptr = bp->b_next;
			if (pcr) {
				xp->sl.statem.tx.N.fsn = xp->sl.statem.Z;
				xp->sl.statem.tx.N.fib = xp->sl.statem.tx.N.fib;
				xp->sl.statem.tx.N.bsn =
				    (xp->sl.statem.tx.X.fsn - 1) & xp->sl.statem.sn_mask;
				xp->sl.statem.tx.N.bib = xp->sl.statem.tx.N.bib;
				xp->sl.statem.Z = (xp->sl.statem.Z + 1) & xp->sl.statem.sn_mask;
			} else {
				xp->sl.statem.tx.N.fsn =
				    (xp->sl.statem.tx.N.fsn + 1) & xp->sl.statem.sn_mask;
				xp->sl.statem.tx.N.fib = xp->sl.statem.tx.N.fib;
				xp->sl.statem.tx.N.bsn =
				    (xp->sl.statem.tx.X.fsn - 1) & xp->sl.statem.sn_mask;
				xp->sl.statem.tx.N.bib = xp->sl.statem.tx.N.bib;
			}
			ctrace(sl_daedt_msu(xp, q, mp));
			if (xp->sl.statem.tx.N.fsn == xp->sl.statem.tx.L.fsn
			    || xp->sl.statem.z_ptr == NULL)
				xp->sl.statem.retrans_cycle = 0;
		}
		return (mp);
	}
	if ((!pcr && (!xp->sl.tb.q_count || xp->sl.statem.rtb_full))
	    || (pcr && (!xp->sl.tb.q_count && !xp->sl.rtb.q_count))) {
		if ((mp = ss7_fast_allocb(&xp_bufpool, 6, BPRI_HI))) {
			xp->sl.statem.tx.N.fsn = xp->sl.statem.tx.L.fsn;
			xp->sl.statem.tx.N.bib = xp->sl.statem.tx.N.bib;
			xp->sl.statem.tx.N.bsn =
			    (xp->sl.statem.tx.X.fsn - 1) & xp->sl.statem.sn_mask;
			xp->sl.statem.tx.N.fib = xp->sl.statem.tx.N.fib;
			sl_daedt_fisu(xp, q, mp);
		}
		return (mp);
	} else {
		spin_lock(&xp->sl.tb.q_lock);
		if ((mp = bufq_head(&xp->sl.tb)) && (mp = dupmsg(mp))) {
			mblk_t *bp = __bufq_dequeue(&xp->sl.tb);

			spin_unlock(&xp->sl.tb.q_lock);
			xp->sl.statem.Cm--;
			if (!xp->sl.statem.Cm)
				qenable(xp->iq);
			xp->sl.statem.tx.L.fsn =
			    (xp->sl.statem.tx.L.fsn + 1) & xp->sl.statem.sn_mask;
			xp->sl.statem.tx.N.fsn = xp->sl.statem.tx.L.fsn;
			if (!xp->sl.rtb.q_count)
				xp_timer_start(xp, t7);
			bufq_queue(&xp->sl.rtb, bp);
			xp->sl.statem.Ct++;
			sl_rc_fsnt_value(xp, q);
			if (pcr) {
				if ((xp->sl.rtb.q_msgs >= xp->sl.config.N1)
				    || (xp->sl.rtb.q_count >= xp->sl.config.N2)) {
					xp->sl.statem.rtb_full = 1;
					xp->sl.statem.forced_retransmission = 1;
				}
			} else {
				if ((xp->sl.rtb.q_msgs >= xp->sl.config.N1)
				    || (xp->sl.rtb.q_count >= xp->sl.config.N2)
				    || (xp->sl.statem.tx.L.fsn ==
					((xp->sl.statem.tx.F.fsn - 2) & xp->sl.statem.sn_mask)))
					xp->sl.statem.rtb_full = 1;
			}
			xp->sl.statem.tx.N.bib = xp->sl.statem.tx.N.bib;
			xp->sl.statem.tx.N.bsn =
			    (xp->sl.statem.tx.X.fsn - 1) & xp->sl.statem.sn_mask;
			xp->sl.statem.tx.N.fib = xp->sl.statem.tx.N.fib;
			ctrace(sl_daedt_msu(xp, q, mp));
		} else
			spin_unlock(&xp->sl.tb.q_lock);
		return (mp);
	}
}

STATIC noinline fastcall __unlikely void
sl_daedr_start(struct xp *xp, queue_t *q)
{
	xp->sdt.statem.daedr_state = SDT_STATE_IN_SERVICE;
	xp->sdl.statem.rx_state = SDL_STATE_IN_SERVICE;
	xp->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_RX_RUNNING);
}

STATIC noinline fastcall __unlikely void
sl_rc_start(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.rc_state == SL_STATE_IDLE) {
		xp->sl.statem.rx.X.fsn = 0;
		xp->sl.statem.rx.X.fib = xp->sl.statem.ib_mask;
		xp->sl.statem.rx.F.fsn = 0;
		xp->sl.statem.rx.T.fsn = xp->sl.statem.sn_mask;
		xp->sl.statem.rtr = 0;	/* Basic only (note 1). */
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88))
			xp->sl.statem.msu_fisu_accepted = 1;
		else
			xp->sl.statem.msu_fisu_accepted = 0;
		xp->sl.statem.abnormal_bsnr = 0;
		xp->sl.statem.abnormal_fibr = 0;	/* Basic only (note 1). */
		xp->sl.statem.congestion_discard = 0;
		xp->sl.statem.congestion_accept = 0;
		ctrace(sl_daedr_start(xp, q));
		xp->sl.statem.rc_state = SL_STATE_IN_SERVICE;
		return;
		/* 
		 *   Note 1 - Although rtr and abnormal_fibr are only applicable to the Basic procedure (and
		 *   not PCR), these state machine variables are never examined by PCR routines, so PCR and
		 *   basic can share the same start procedures.
		 */
	}
}

STATIC noinline fastcall void
sl_rc_reject_msu_fisu(struct xp *xp, queue_t *q)
{
	xp->sl.statem.msu_fisu_accepted = 0;
}

STATIC noinline fastcall void
sl_rc_accept_msu_fisu(struct xp *xp, queue_t *q)
{
	xp->sl.statem.msu_fisu_accepted = 1;
}

STATIC noinline fastcall __unlikely void
sl_rc_retrieve_fsnx(struct xp *xp, queue_t *q)
{
	sl_txc_fsnx_value(xp, q);	/* error in 93 spec */
	xp->sl.statem.congestion_discard = 0;
	xp->sl.statem.congestion_accept = 0;
	sl_cc_normal(xp, q);
	xp->sl.statem.rtr = 0;	/* basic only */
}

STATIC noinline fastcall __unlikely void
sl_rc_align_fsnx(struct xp *xp, queue_t *q)
{
	sl_txc_fsnx_value(xp, q);
}

STATIC noinline fastcall __unlikely int
sl_rc_clear_rb(struct xp *xp, queue_t *q)
{
	bufq_purge(&xp->sl.rb);
	flushq(RD(q), FLUSHDATA);
	xp->sl.statem.Cr = 0;
	return sl_rb_cleared_ind(xp, q);
}

STATIC noinline fastcall __unlikely int
sl_rc_retrieve_bsnt(struct xp *xp, queue_t *q)
{
	xp->sl.statem.rx.T.bsn = (xp->sl.statem.rx.X.fsn - 1) & 0x7F;
	return sl_bsnt_ind(xp, q, xp->sl.statem.rx.T.bsn);
}

STATIC noinline fastcall void
sl_cc_busy(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.cc_state == SL_STATE_NORMAL) {
		ctrace(sl_txc_send_sib(xp, q));
		xp_timer_start(xp, t5);
		xp->sl.statem.cc_state = SL_STATE_BUSY;
	}
}

STATIC noinline fastcall __unlikely void
sl_rc_congestion_discard(struct xp *xp, queue_t *q)
{
	xp->sl.statem.congestion_discard = 1;
	sl_cc_busy(xp, q);
}

STATIC noinline fastcall __unlikely void
sl_rc_congestion_accept(struct xp *xp, queue_t *q)
{
	xp->sl.statem.congestion_accept = 1;
	sl_cc_busy(xp, q);
}

STATIC noinline fastcall __unlikely void
sl_rc_no_congestion(struct xp *xp, queue_t *q)
{
	xp->sl.statem.congestion_discard = 0;
	xp->sl.statem.congestion_accept = 0;
	sl_cc_normal(xp, q);
	sl_txc_fsnx_value(xp, q);
	if (xp->sl.statem.rtr == 1) {	/* rtr never set for PCR */
		sl_txc_nack_to_be_sent(xp, q);
		xp->sl.statem.rx.X.fib = xp->sl.statem.rx.X.fib ? 0 : xp->sl.statem.ib_mask;
	}
}

STATIC noinline fastcall __unlikely void
sl_lsc_congestion_discard(struct xp *xp, queue_t *q)
{
	sl_rc_congestion_discard(xp, q);
	xp->sl.statem.l3_congestion_detect = 1;
}

STATIC noinline fastcall __unlikely void
sl_lsc_congestion_accept(struct xp *xp, queue_t *q)
{
	sl_rc_congestion_accept(xp, q);
	xp->sl.statem.l3_congestion_detect = 1;
}

STATIC noinline fastcall __unlikely void
sl_lsc_no_congestion(struct xp *xp, queue_t *q)
{
	sl_rc_no_congestion(xp, q);
	xp->sl.statem.l3_congestion_detect = 0;
}

STATIC noinline fastcall void
sl_lsc_sio(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_INITIAL_ALIGNMENT:
		break;
	default:
		xp_timer_stop(xp, t1);	/* ok if not running */
		sl_out_of_service_ind(xp, q, SL_FAIL_RECEIVED_SIO);
		xp->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIO;
		sl_rc_stop(xp, q);
		sl_suerm_stop(xp, q);
		sl_poc_stop(xp, q);	/* ok if ANSI */
		ctrace(sl_txc_send_sios(xp, q));
		xp->sl.statem.emergency = 0;
		/* FIXME: reinspect */
		xp->sl.statem.local_processor_outage = 0;
		xp->sl.statem.remote_processor_outage = 0;	/* ok if ITUT */
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
}

STATIC noinline fastcall int
sl_lsc_alignment_not_possible(struct xp *xp, queue_t *q)
{
	int err;

	if ((err = sl_out_of_service_ind(xp, q, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)))
		return (err);
	xp->sl.statem.failure_reason = SL_FAIL_ALIGNMENT_NOT_POSSIBLE;
	sl_rc_stop(xp, q);
	ctrace(sl_txc_send_sios(xp, q));
	xp->sl.statem.local_processor_outage = 0;
	xp->sl.statem.emergency = 0;
	xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	return (QR_DONE);
}

STATIC noinline fastcall void
sl_iac_sio(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		xp_timer_stop(xp, t2);
		if (xp->sl.statem.emergency) {
			xp->sl.statem.t4v = xp->sl.config.t4e;
			printd(("Sending SIE at %lu\n", jiffies));
			ctrace(sl_txc_send_sie(xp, q));
		} else {
			xp->sl.statem.t4v = xp->sl.config.t4n;
			ctrace(sl_txc_send_sin(xp, q));
		}
		xp_timer_start(xp, t3);
		xp->sl.statem.iac_state = SL_STATE_ALIGNED;
		break;
	case SL_STATE_PROVING:
		ctrace(xp_timer_stop(xp, t4));
		sl_aerm_stop(xp, q);
		xp_timer_start(xp, t3);
		xp->sl.statem.iac_state = SL_STATE_ALIGNED;
		break;
	}
}

STATIC noinline fastcall void
sl_iac_sios(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.iac_state) {
	case SL_STATE_ALIGNED:
	case SL_STATE_PROVING:
		ctrace(xp_timer_stop(xp, t4));	/* ok if not running */
		ctrace(sl_lsc_alignment_not_possible(xp, q));
		sl_aerm_stop(xp, q);	/* ok if not running */
		xp_timer_stop(xp, t3);	/* ok if not running */
		xp->sl.statem.emergency = 0;
		xp->sl.statem.iac_state = SL_STATE_IDLE;
		break;
	}
}

STATIC noinline fastcall void
sl_lsc_sios(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
	case SL_STATE_ALIGNED_NOT_READY:
		xp_timer_stop(xp, t1);	/* ok to stop if not running */
	case SL_STATE_IN_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(xp, q, SL_FAIL_RECEIVED_SIOS);
		xp->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIOS;
		sl_suerm_stop(xp, q);	/* ok to stop if not running */
		sl_rc_stop(xp, q);
		sl_poc_stop(xp, q);	/* ok if ANSI */
		ctrace(sl_txc_send_sios(xp, q));
		xp->sl.statem.emergency = 0;
		xp->sl.statem.local_processor_outage = 0;
		xp->sl.statem.remote_processor_outage = 0;	/* ok if ITU */
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
}

STATIC noinline fastcall __unlikely void
sl_lsc_no_processor_outage(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		xp->sl.statem.processor_outage = 0;
		if (!xp->sl.statem.l3_indication_received) {
			trace();
			return;
		}
		xp->sl.statem.l3_indication_received = 0;
		ctrace(sl_txc_send_msu(xp, q));
		xp->sl.statem.local_processor_outage = 0;
		sl_rc_accept_msu_fisu(xp, q);
		xp->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
	}
}

STATIC noinline fastcall __unlikely int
sl_poc_remote_processor_recovered(struct xp *xp, queue_t *q)
{
	int err;

	switch (xp->sl.statem.poc_state) {
	case SL_STATE_REMOTE_PROCESSOR_OUTAGE:
		/* Indication moved from caller to remove spurious remote processor recovered
		   indications. */
		if ((err = sl_remote_processor_recovered_ind(xp, q)))
			return (err);
		sl_lsc_no_processor_outage(xp, q);
		xp->sl.statem.poc_state = SL_STATE_IDLE;
		break;
	case SL_STATE_BOTH_PROCESSORS_OUT:
		/* Indication moved from caller to remove spurious remote processor recovered
		   indications. */
		if ((err = sl_remote_processor_recovered_ind(xp, q)))
			return (err);
		xp->sl.statem.poc_state = SL_STATE_LOCAL_PROCESSOR_OUTAGE;
		break;
	}
	return (QR_DONE);
}

STATIC noinline fastcall int
sl_lsc_fisu_msu_received(struct xp *xp, queue_t *q)
{
	int err;

	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
		if ((err = sl_in_service_ind(xp, q)))
			return (err);
		if (xp->option.pvar == SS7_PVAR_ITUT_93)
			sl_rc_accept_msu_fisu(xp, q);	/* unnecessary */
		xp_timer_stop(xp, t1);
		sl_txc_send_msu(xp, q);
		xp->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if ((err = sl_in_service_ind(xp, q)))
			return (err);
		xp_timer_stop(xp, t1);
		xp->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			/* 
			 *  A deviation from the SDLs has been placed here to limit the number of remote
			 *  processor recovered indications which are delivered to L3.  One indication is
			 *  sufficient.
			 */
			if (xp->sl.statem.remote_processor_outage) {
				if ((err = sl_remote_processor_recovered_ind(xp, q)))
					return (err);
				xp->sl.statem.remote_processor_outage = 0;
			}
		} else {
			if ((err = sl_poc_remote_processor_recovered(xp, q)))
				return (err);
		}
		break;
	}
	return (QR_DONE);
}

STATIC noinline fastcall __unlikely int
sl_poc_remote_processor_outage(struct xp *xp, queue_t *q)
{
	int err = 0;

	switch (xp->sl.statem.poc_state) {
	case SL_STATE_IDLE:
		/* Moved here from caller to limit the number of remote processor outage
		   indications delivered to L3. */
		if ((err = sl_remote_processor_outage_ind(xp, q)))
			return (err);
		xp->sl.statem.poc_state = SL_STATE_REMOTE_PROCESSOR_OUTAGE;
		break;
	case SL_STATE_LOCAL_PROCESSOR_OUTAGE:
		/* Moved here from caller to limit the number of remote processor outage
		   indications delivered to L3. */
		if ((err = sl_remote_processor_outage_ind(xp, q)))
			return (err);
		xp->sl.statem.poc_state = SL_STATE_BOTH_PROCESSORS_OUT;
		break;
	}
	return (err);
}

STATIC noinline fastcall __unlikely void
sl_lsc_sib(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_txc_sib_received(xp, q);
		break;
	}
}

STATIC noinline fastcall __unlikely int
sl_lsc_sipo(struct xp *xp, queue_t *q)
{
	int err;

	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			xp_timer_stop(xp, t1);
			if ((err = sl_remote_processor_outage_ind(xp, q)))
				return (err);
			xp->sl.statem.remote_processor_outage = 1;
			xp->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			xp_timer_stop(xp, t1);
			if ((err = sl_poc_remote_processor_outage(xp, q)))
				return (err);
			xp->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			if ((err = sl_remote_processor_outage_ind(xp, q)))
				return (err);
			xp->sl.statem.remote_processor_outage = 1;
			xp_timer_stop(xp, t1);
			xp->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			if ((err = sl_poc_remote_processor_outage(xp, q)))
				return (err);
			xp_timer_stop(xp, t1);
			xp->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_IN_SERVICE:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			sl_txc_send_fisu(xp, q);
			if ((err = sl_remote_processor_outage_ind(xp, q)))
				return (err);
			xp->sl.statem.remote_processor_outage = 1;
			sl_rc_align_fsnx(xp, q);
			sl_cc_stop(xp, q);
			xp->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			sl_txc_send_fisu(xp, q);
			if ((err = sl_poc_remote_processor_outage(xp, q)))
				return (err);
			xp->sl.statem.processor_outage = 1;	/* remote? */
			xp->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
#if 0
			xp->sl.statem.remote_processor_outage = 1;
			return sl_remote_processor_outage_ind(xp, q);
#else
			/* 
			 *  A deviation from the SDLs has been placed here to limit the number of
			 *  remote processor outage indications which are delivered to L3.  One
			 *  indication is sufficient.
			 */
			if (!xp->sl.statem.remote_processor_outage) {
				xp->sl.statem.remote_processor_outage = 1;
				return sl_remote_processor_outage_ind(xp, q);
			}
#endif
		} else {
			if ((err = sl_poc_remote_processor_outage(xp, q)))
				return (err);
		}
		break;
	}
	return (QR_DONE);
}

STATIC noinline fastcall __unlikely void
sl_poc_local_processor_outage(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.poc_state) {
	case SL_STATE_IDLE:
		xp->sl.statem.poc_state = SL_STATE_LOCAL_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_REMOTE_PROCESSOR_OUTAGE:
		xp->sl.statem.poc_state = SL_STATE_BOTH_PROCESSORS_OUT;
		return;
	}
}

STATIC noinline fastcall __unlikely void
sl_eim_start(struct xp *xp, queue_t *q)
{
	xp->sdt.statem.Ce = 0;
	xp->sdt.statem.interval_error = 0;
	xp->sdt.statem.su_received = 0;
	xp_timer_start(xp, t8);
	xp->sdt.statem.eim_state = SDT_STATE_MONITORING;
}

STATIC noinline fastcall __unlikely void
sl_suerm_start(struct xp *xp, queue_t *q)
{
	if (xp->option.popt & SS7_POPT_HSL)
		sl_eim_start(xp, q);
	else {
		xp->sdt.statem.Cs = 0;
		xp->sdt.statem.Ns = 0;
		xp->sdt.statem.suerm_state = SDT_STATE_IN_SERVICE;
	}
}

STATIC noinline fastcall __unlikely void
sl_lsc_alignment_complete(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.lsc_state == SL_STATE_INITIAL_ALIGNMENT) {
		sl_suerm_start(xp, q);
		xp_timer_start(xp, t1);
		if (xp->sl.statem.local_processor_outage) {
			if (((xp->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
			    || ((xp->option.pvar & SS7_PVAR_YR) <= SS7_PVAR_88))
				sl_poc_local_processor_outage(xp, q);
			ctrace(sl_txc_send_sipo(xp, q));
			if (xp->option.pvar != SS7_PVAR_ITUT_93)	/* possible error */
				sl_rc_reject_msu_fisu(xp, q);
			xp->sl.statem.lsc_state = SL_STATE_ALIGNED_NOT_READY;
		} else {
			ctrace(sl_txc_send_msu(xp, q));	/* changed from send_fisu for Q.781 */
			sl_rc_accept_msu_fisu(xp, q);	/* error in ANSI spec? */
			xp->sl.statem.lsc_state = SL_STATE_ALIGNED_READY;
		}
	}
}

STATIC noinline fastcall void
sl_lsc_sin(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
		sl_out_of_service_ind(xp, q, SL_FAIL_RECEIVED_SIN);
		xp->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIN;
		sl_suerm_stop(xp, q);
		sl_rc_stop(xp, q);
		ctrace(sl_txc_send_sios(xp, q));
		xp->sl.statem.emergency = 0;
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(xp, q, SL_FAIL_RECEIVED_SIN);
		xp->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIN;
		sl_suerm_stop(xp, q);
		sl_rc_stop(xp, q);
		sl_poc_stop(xp, q);	/* ok if not ITUT */
		ctrace(sl_txc_send_sios(xp, q));
		xp->sl.statem.emergency = 0;
		xp->sl.statem.local_processor_outage = 0;
		xp->sl.statem.remote_processor_outage = 0;	/* ok if not ANSI */
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
}

STATIC noinline fastcall __unlikely void
sl_aerm_set_ti_to_tie(struct xp *xp, queue_t *q)
{
	if (xp->sdt.statem.aerm_state == SDT_STATE_IDLE)
		xp->sdt.statem.Ti = xp->sdt.config.Tie;
}

STATIC noinline fastcall __unlikely void
sl_aerm_start(struct xp *xp, queue_t *q)
{
	xp->sdt.statem.Ca = 0;
	xp->sdt.statem.aborted_proving = 0;
	xp->sdt.statem.aerm_state = SDT_STATE_MONITORING;
}

STATIC noinline fastcall void
sl_iac_sin(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		xp_timer_stop(xp, t2);
		if (xp->sl.statem.emergency) {
			xp->sl.statem.t4v = xp->sl.config.t4e;
			ctrace(sl_txc_send_sie(xp, q));
		} else {
			xp->sl.statem.t4v = xp->sl.config.t4n;
			ctrace(sl_txc_send_sin(xp, q));
		}
		xp_timer_start(xp, t3);
		xp->sl.statem.iac_state = SL_STATE_ALIGNED;
		return;
	case SL_STATE_ALIGNED:
		xp_timer_stop(xp, t3);
		if (xp->sl.statem.t4v == xp->sl.config.t4e)
			sl_aerm_set_ti_to_tie(xp, q);
		sl_aerm_start(xp, q);
		ctrace(xp_timer_start(xp, t4));
		xp->sl.statem.further_proving = 0;
		xp->sl.statem.Cp = 0;
		xp->sl.statem.iac_state = SL_STATE_PROVING;
		return;
	}
}

STATIC noinline fastcall void
sl_lsc_sie(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
		sl_out_of_service_ind(xp, q, SL_FAIL_RECEIVED_SIE);
		xp->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIE;
		sl_suerm_stop(xp, q);
		sl_rc_stop(xp, q);
		ctrace(sl_txc_send_sios(xp, q));
		xp->sl.statem.emergency = 0;
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(xp, q, SL_FAIL_RECEIVED_SIE);
		xp->sl.statem.failure_reason = SL_FAIL_RECEIVED_SIE;
		sl_suerm_stop(xp, q);
		sl_rc_stop(xp, q);
		sl_poc_stop(xp, q);	/* ok if not ITUT */
		ctrace(sl_txc_send_sios(xp, q));
		xp->sl.statem.emergency = 0;
		xp->sl.statem.local_processor_outage = 0;
		xp->sl.statem.remote_processor_outage = 0;	/* ok if not ANSI */
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
}

STATIC noinline fastcall void
sl_iac_sie(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		xp_timer_stop(xp, t2);
		if (xp->sl.statem.emergency) {
			xp->sl.statem.t4v = xp->sl.config.t4e;
			ctrace(sl_txc_send_sie(xp, q));
		} else {
			xp->sl.statem.t4v = xp->sl.config.t4e;	/* yes e */
			ctrace(sl_txc_send_sin(xp, q));
		}
		xp_timer_start(xp, t3);
		xp->sl.statem.iac_state = SL_STATE_ALIGNED;
		return;
	case SL_STATE_ALIGNED:
		printd(("Receiving SIE at %lu\n", jiffies));
		xp->sl.statem.t4v = xp->sl.config.t4e;
		xp_timer_stop(xp, t3);
		sl_aerm_set_ti_to_tie(xp, q);
		sl_aerm_start(xp, q);
		ctrace(xp_timer_start(xp, t4));
		xp->sl.statem.further_proving = 0;
		xp->sl.statem.Cp = 0;
		xp->sl.statem.iac_state = SL_STATE_PROVING;
		return;
	case SL_STATE_PROVING:
		if (xp->sl.statem.t4v == xp->sl.config.t4e)
			return;
		ctrace(xp_timer_stop(xp, t4));
		xp->sl.statem.t4v = xp->sl.config.t4e;
		sl_aerm_stop(xp, q);
		sl_aerm_set_ti_to_tie(xp, q);
		sl_aerm_start(xp, q);
		ctrace(xp_timer_start(xp, t4));
		xp->sl.statem.further_proving = 0;
		return;
	}
}

/*
 *  --------------------------------------------------------------------------
 *
 *  These congestion functions are implementation dependent.  We should define a congestion onset level and set
 *  congestion accept at that point.  We should also define a second congestion onset level and set congestion
 *  discard at that point.  For STREAMS, the upstream congestion can be detected in two ways: 1) canputnext(): is
 *  the upstream module flow controlled; and, 2) canput(): are we flow controlled.  If the upstream module is
 *  flow controlled, then we can accept MSUs and place them on our own read queue.  If we are flow contolled,
 *  then we have no choice but to discard the message.  In addition, and because upstream message processing
 *  times are likely more sensitive to the number of backlogged messages than they are to the number of
 *  backlogged message octets, we have some configurable thresholds of backlogging and keep track of backlogged
 *  messages.
 *
 *  --------------------------------------------------------------------------
 */

STATIC inline fastcall __hot void
sl_rb_congestion_function(struct xp *xp, queue_t *q)
{
	if (!xp->sl.statem.l3_congestion_detect) {
		if (xp->sl.statem.l2_congestion_detect) {
			if (xp->sl.statem.Cr <= xp->sl.config.rb_abate && canputnext(RD(q))) {
				sl_rc_no_congestion(xp, q);
				xp->sl.statem.l2_congestion_detect = 0;
			}
		} else {
			if (xp->sl.statem.Cr >= xp->sl.config.rb_discard || !canput(RD(q))) {
				sl_rc_congestion_discard(xp, q);
				xp->sl.statem.l2_congestion_detect = 1;
			} else if (xp->sl.statem.Cr >= xp->sl.config.rb_accept
				   || !canputnext(RD(q))) {
				sl_rc_congestion_accept(xp, q);
				xp->sl.statem.l2_congestion_detect = 1;
			}
		}
	}
}

STATIC inline fastcall __hot_in void
sl_rc_signal_unit(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int pcr = xp->option.popt & SS7_POPT_PCR;

	if (xp->sl.statem.rc_state != SL_STATE_IN_SERVICE) {
		freemsg(mp);
		return;
	}
	/* 
	 *  Note: the driver must check that the length of the frame is within appropriate bounds as specified by
	 *  the DAEDR in Q.703.  If the length of the frame is incorrect, it should indicate daedr_error- _frame
	 *  rather than daedr_received_frame.
	 */
	if (xp->option.popt & SS7_POPT_XSN) {
		xp->sl.statem.rx.R.bsn = ntohs(*((sl_ushort *) mp->b_rptr)) & 0x0fff;
		xp->sl.statem.rx.R.bib = ntohs(*(sl_ushort *) mp->b_rptr++) & 0x8000;
		xp->sl.statem.rx.R.fsn = ntohs(*((sl_ushort *) mp->b_rptr)) & 0x0fff;
		xp->sl.statem.rx.R.fib = ntohs(*(sl_ushort *) mp->b_rptr++) & 0x8000;
		xp->sl.statem.rx.len = ntohs(*(sl_ushort *) mp->b_rptr++) & 0x01ff;
	} else {
		xp->sl.statem.rx.R.bsn = *mp->b_rptr & 0x7f;
		xp->sl.statem.rx.R.bib = *mp->b_rptr++ & 0x80;
		xp->sl.statem.rx.R.fsn = *mp->b_rptr & 0x7f;
		xp->sl.statem.rx.R.fib = *mp->b_rptr++ & 0x80;
		xp->sl.statem.rx.len = *mp->b_rptr++ & 0x3f;
	}
	if (xp->sl.statem.rx.len == 1) {
		xp->sl.statem.rx.sio = *mp->b_rptr++ & 0x07;
	}
	if (xp->sl.statem.rx.len == 2) {
		xp->sl.statem.rx.sio = *mp->b_rptr++ & 0x07;
		xp->sl.statem.rx.sio = *mp->b_rptr++ & 0x07;
	}
#if 0
	ptrace(("rx: bsn=%x, bib=%x, fsn=%x, fib=%x, len=%d, sio=%d\n", xp->sl.statem.rx.R.bsn,
		xp->sl.statem.rx.R.bib, xp->sl.statem.rx.R.fsn, xp->sl.statem.rx.R.fib,
		xp->sl.statem.rx.len, xp->sl.statem.rx.sio));
#endif
	if (((xp->sl.statem.rx.len) == 1) || ((xp->sl.statem.rx.len) == 2)) {
		switch (xp->sl.statem.rx.sio) {
		case LSSU_SIO:{
			sl_iac_sio(xp, q);
			sl_lsc_sio(xp, q);
			break;
		}
		case LSSU_SIN:{
			sl_iac_sin(xp, q);
			sl_lsc_sin(xp, q);
			break;
		}
		case LSSU_SIE:{
			sl_iac_sie(xp, q);
			sl_lsc_sie(xp, q);
			break;
		}
		case LSSU_SIOS:{
			sl_iac_sios(xp, q);
			sl_lsc_sios(xp, q);
			break;
		}
		case LSSU_SIPO:{
			sl_lsc_sipo(xp, q);
			break;
		}
		case LSSU_SIB:{
			sl_lsc_sib(xp, q);
			break;
		}
		}
		freemsg(mp);
		return;
	}
	if (SN_OUTSIDE
	    (((xp->sl.statem.rx.F.fsn - 1) & xp->sl.statem.sn_mask), xp->sl.statem.rx.R.bsn,
	     xp->sl.statem.rx.T.fsn)) {
		if (xp->sl.statem.abnormal_bsnr) {
			sl_lsc_link_failure(xp, q, SL_FAIL_ABNORMAL_BSNR);
			xp->sl.statem.rc_state = SL_STATE_IDLE;
			freemsg(mp);
			return;
		} else {
			xp->sl.statem.abnormal_bsnr = 1;
			xp->sl.statem.unb = 0;
			freemsg(mp);
			return;
		}
	}
	if (xp->sl.statem.abnormal_bsnr) {
		if (xp->sl.statem.unb == 1) {
			xp->sl.statem.abnormal_bsnr = 0;
		} else {
			xp->sl.statem.unb = 1;
			freemsg(mp);
			return;
		}
	}
	if (pcr) {
		sl_lsc_fisu_msu_received(xp, q);
		sl_txc_bsnr_and_bibr(xp, q);
		xp->sl.statem.rx.F.fsn = (xp->sl.statem.rx.R.bsn + 1) & xp->sl.statem.sn_mask;
		if (!xp->sl.statem.msu_fisu_accepted) {
			freemsg(mp);
			return;
		}
		sl_rb_congestion_function(xp, q);
		if (xp->sl.statem.congestion_discard) {
			sl_cc_busy(xp, q);
			freemsg(mp);
			return;
		}
		if ((xp->sl.statem.rx.R.fsn == xp->sl.statem.rx.X.fsn)
		    && (xp->sl.statem.rx.len > 2)) {
			xp->sl.statem.rx.X.fsn =
			    (xp->sl.statem.rx.X.fsn + 1) & xp->sl.statem.sn_mask;
			putq(q, mp);
			xp->sl.statem.Cr++;
			if (xp->sl.statem.congestion_accept)
				sl_cc_busy(xp, q);
			else
				sl_txc_fsnx_value(xp, q);
			return;
		} else {
			freemsg(mp);
			return;
		}
		return;
	}
	if (xp->sl.statem.rx.R.fib == xp->sl.statem.rx.X.fib) {
		if (xp->sl.statem.abnormal_fibr) {
			if (xp->sl.statem.unf == 1) {
				xp->sl.statem.abnormal_fibr = 0;
			} else {
				xp->sl.statem.unf = 1;
				freemsg(mp);
				return;
			}
		}
		sl_lsc_fisu_msu_received(xp, q);
		sl_txc_bsnr_and_bibr(xp, q);
		xp->sl.statem.rx.F.fsn = (xp->sl.statem.rx.R.bsn + 1) & xp->sl.statem.sn_mask;
		if (!xp->sl.statem.msu_fisu_accepted) {
			freemsg(mp);
			return;
		}
		sl_rb_congestion_function(xp, q);
		if (xp->sl.statem.congestion_discard) {
			xp->sl.statem.rtr = 1;
			freemsg(mp);
			sl_cc_busy(xp, q);
			return;
		}
		if ((xp->sl.statem.rx.R.fsn == xp->sl.statem.rx.X.fsn)
		    && (xp->sl.statem.rx.len > 2)) {
			xp->sl.statem.rx.X.fsn =
			    (xp->sl.statem.rx.X.fsn + 1) & xp->sl.statem.sn_mask;
			xp->sl.statem.rtr = 0;
			putq(q, mp);
			xp->sl.statem.Cr++;
			if (xp->sl.statem.congestion_accept)
				sl_cc_busy(xp, q);
			else
				sl_txc_fsnx_value(xp, q);
			return;
		}
		if ((xp->sl.statem.rx.R.fsn ==
		     ((xp->sl.statem.rx.X.fsn - 1) & xp->sl.statem.sn_mask))) {
			freemsg(mp);
			return;
		} else {
			if (xp->sl.statem.congestion_accept) {
				xp->sl.statem.rtr = 1;
				sl_cc_busy(xp, q);	/* not required? */
				freemsg(mp);
				return;
			} else {
				sl_txc_nack_to_be_sent(xp, q);
				xp->sl.statem.rtr = 1;
				xp->sl.statem.rx.X.fib =
				    xp->sl.statem.rx.X.fib ? 0 : xp->sl.statem.ib_mask;
				freemsg(mp);
				return;
			}
		}
	} else {
		if (xp->sl.statem.abnormal_fibr) {
			sl_lsc_link_failure(xp, q, SL_FAIL_ABNORMAL_FIBR);
			freemsg(mp);
			return;
		}
		if (xp->sl.statem.rtr == 1) {
			sl_txc_bsnr_and_bibr(xp, q);
			xp->sl.statem.rx.F.fsn =
			    (xp->sl.statem.rx.R.bsn + 1) & xp->sl.statem.sn_mask;
			freemsg(mp);
			return;
		}
		xp->sl.statem.abnormal_fibr = 1;
		xp->sl.statem.unf = 0;
		freemsg(mp);
		return;
	}
}

STATIC noinline fastcall __unlikely void
sl_lsc_stop(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
		sl_iac_stop(xp, q);	/* ok if not running */
		xp_timer_stop(xp, t1);	/* ok if not running */
		sl_rc_stop(xp, q);
		sl_suerm_stop(xp, q);	/* ok if not running */
		sl_poc_stop(xp, q);	/* ok if not running or not ITUT */
		ctrace(sl_txc_send_sios(xp, q));
		xp->sl.statem.emergency = 0;
		xp->sl.statem.local_processor_outage = 0;
		xp->sl.statem.remote_processor_outage = 0;	/* ok of not ANSI */
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
}

STATIC noinline fastcall __unlikely void
sl_lsc_clear_rtb(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		xp->sl.statem.local_processor_outage = 0;
		sl_txc_send_fisu(xp, q);
		sl_txc_clear_rtb(xp, q);
	}
}

STATIC noinline fastcall void
sl_iac_correct_su(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.iac_state == SL_STATE_PROVING) {
		if (xp->sl.statem.further_proving) {
			ctrace(xp_timer_stop(xp, t4));
			sl_aerm_start(xp, q);
			ctrace(xp_timer_start(xp, t4));
			xp->sl.statem.further_proving = 0;
		}
	}
}

STATIC noinline fastcall __unlikely void
sl_iac_abort_proving(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.iac_state == SL_STATE_PROVING) {
		xp->sl.statem.Cp++;
		if (xp->sl.statem.Cp == xp->sl.config.M) {
			ctrace(sl_lsc_alignment_not_possible(xp, q));
			ctrace(xp_timer_stop(xp, t4));
			sl_aerm_stop(xp, q);
			xp->sl.statem.emergency = 0;
			xp->sl.statem.iac_state = SL_STATE_IDLE;
			return;
		}
		xp->sl.statem.further_proving = 1;
	}
}

#define sl_lsc_flush_buffers sl_lsc_clear_buffers
STATIC noinline fastcall __unlikely int
sl_lsc_clear_buffers(struct xp *xp, queue_t *q)
{
	int err;

	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			if ((err = sl_rtb_cleared_ind(xp, q)))
				return (err);
			xp->sl.statem.local_processor_outage = 0;	/* ??? */
		}
		break;
	case SL_STATE_INITIAL_ALIGNMENT:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			if ((err = sl_rtb_cleared_ind(xp, q)))
				return (err);
			xp->sl.statem.local_processor_outage = 0;
		}
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			if ((err = sl_rtb_cleared_ind(xp, q)))
				return (err);
			xp->sl.statem.local_processor_outage = 0;
			sl_txc_send_fisu(xp, q);
			xp->sl.statem.lsc_state = SL_STATE_ALIGNED_READY;
		}
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			xp->sl.statem.local_processor_outage = 0;
			if ((err = sl_rc_clear_rb(xp, q)))
				return (err);
			sl_rc_accept_msu_fisu(xp, q);
			sl_txc_send_fisu(xp, q);
			sl_txc_clear_tb(xp, q);
			sl_txc_clear_rtb(xp, q);
		} else {
			sl_txc_flush_buffers(xp, q);
			xp->sl.statem.l3_indication_received = 1;
			if (xp->sl.statem.processor_outage)
				return (QR_DONE);
			xp->sl.statem.l3_indication_received = 0;
			sl_txc_send_msu(xp, q);
			xp->sl.statem.local_processor_outage = 0;
			sl_rc_accept_msu_fisu(xp, q);
			xp->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
		}
		break;
	}
	return (QR_DONE);
}

STATIC noinline fastcall __unlikely void
sl_lsc_continue(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		xp->sl.statem.l3_indication_received = 1;
		if (xp->sl.statem.processor_outage) {
			trace();
			return;
		}
		xp->sl.statem.l3_indication_received = 0;
		ctrace(sl_txc_send_msu(xp, q));
		xp->sl.statem.local_processor_outage = 0;
		sl_rc_accept_msu_fisu(xp, q);
		xp->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
	}
}

STATIC noinline fastcall __unlikely void
sl_poc_local_processor_recovered(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.poc_state) {
	case SL_STATE_LOCAL_PROCESSOR_OUTAGE:
		sl_lsc_no_processor_outage(xp, q);
		xp->sl.statem.poc_state = SL_STATE_IDLE;
		return;
	case SL_STATE_BOTH_PROCESSORS_OUT:
		xp->sl.statem.poc_state = SL_STATE_REMOTE_PROCESSOR_OUTAGE;
		return;
	}
}

#define sl_lsc_resume sl_lsc_local_processor_recovered
STATIC noinline fastcall __unlikely void
sl_lsc_local_processor_recovered(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		xp->sl.statem.local_processor_outage = 0;
		break;
	case SL_STATE_INITIAL_ALIGNMENT:
		xp->sl.statem.local_processor_outage = 0;
		break;
	case SL_STATE_ALIGNED_READY:
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if (((xp->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		    || ((xp->option.pvar & SS7_PVAR_YR) <= SS7_PVAR_88))
			sl_poc_local_processor_recovered(xp, q);
		xp->sl.statem.local_processor_outage = 0;
		sl_txc_send_fisu(xp, q);
		if (xp->option.pvar == SS7_PVAR_ITUT_96)
			sl_rc_accept_msu_fisu(xp, q);
		xp->sl.statem.lsc_state = SL_STATE_ALIGNED_READY;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if (((xp->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		    || ((xp->option.pvar & SS7_PVAR_YR) <= SS7_PVAR_88)) {
			sl_poc_local_processor_recovered(xp, q);
			sl_rc_retrieve_fsnx(xp, q);
			if (xp->sl.statem.poc_state != SL_STATE_IDLE)
				sl_txc_send_fisu(xp, q);	/* note 3: in fisu BSN <= FSNX-1 */
			// xp->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
		} else {
			xp->sl.statem.local_processor_outage = 0;
			sl_rc_accept_msu_fisu(xp, q);
			if (xp->sl.statem.remote_processor_outage) {
				sl_txc_send_fisu(xp, q);
				sl_remote_processor_outage_ind(xp, q);
				return;
			}
			sl_txc_send_msu(xp, q);
			xp->sl.statem.lsc_state = SL_STATE_IN_SERVICE;
		}
		break;
	}
}

#define sl_lsc_level_3_failure sl_lsc_local_processor_outage
STATIC noinline fastcall __unlikely void
sl_lsc_local_processor_outage(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_INITIAL_ALIGNMENT:
		xp->sl.statem.local_processor_outage = 1;
		return;
	case SL_STATE_ALIGNED_READY:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88))
			xp->sl.statem.local_processor_outage = 1;
		else
			sl_poc_local_processor_outage(xp, q);
		ctrace(sl_txc_send_sipo(xp, q));
		if (xp->option.pvar != SS7_PVAR_ITUT_93)	/* possible error 93 specs */
			sl_rc_reject_msu_fisu(xp, q);
		xp->sl.statem.lsc_state = SL_STATE_ALIGNED_NOT_READY;
		return;
	case SL_STATE_IN_SERVICE:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			xp->sl.statem.local_processor_outage = 1;
		} else {
			sl_poc_local_processor_outage(xp, q);
			xp->sl.statem.processor_outage = 1;
		}
		ctrace(sl_txc_send_sipo(xp, q));
		sl_rc_reject_msu_fisu(xp, q);
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
			sl_rc_align_fsnx(xp, q);
			sl_cc_stop(xp, q);
		}
		xp->sl.statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_PROCESSOR_OUTAGE:
		if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88))
			xp->sl.statem.local_processor_outage = 1;
		else
			sl_poc_local_processor_outage(xp, q);
		ctrace(sl_txc_send_sipo(xp, q));
		return;
	}
}

STATIC noinline fastcall __unlikely void
sl_iac_emergency(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.iac_state) {
	case SL_STATE_PROVING:
		ctrace(sl_txc_send_sie(xp, q));
		ctrace(xp_timer_stop(xp, t4));
		xp->sl.statem.t4v = xp->sl.config.t4e;
		sl_aerm_stop(xp, q);
		sl_aerm_set_ti_to_tie(xp, q);
		sl_aerm_start(xp, q);
		ctrace(xp_timer_start(xp, t4));
		xp->sl.statem.further_proving = 0;
		return;
	case SL_STATE_ALIGNED:
		ctrace(sl_txc_send_sie(xp, q));
		xp->sl.statem.t4v = xp->sl.config.t4e;
		return;
	case SL_STATE_IDLE:
	case SL_STATE_NOT_ALIGNED:
		xp->sl.statem.emergency = 1;
	}
}

STATIC noinline fastcall __unlikely void
sl_lsc_emergency(struct xp *xp, queue_t *q)
{
	xp->sl.statem.emergency = 1;
	sl_iac_emergency(xp, q);	/* added to pass Q.781/Test 1.20 */
}

STATIC noinline fastcall __unlikely void
sl_lsc_emergency_ceases(struct xp *xp, queue_t *q)
{
	xp->sl.statem.emergency = 0;
}

STATIC noinline fastcall __unlikely void
sl_iac_start(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.iac_state == SL_STATE_IDLE) {
		ctrace(sl_txc_send_sio(xp, q));
		xp_timer_start(xp, t2);
		xp->sl.statem.iac_state = SL_STATE_NOT_ALIGNED;
	}
}

STATIC noinline fastcall __unlikely void
sl_daedt_start(struct xp *xp, queue_t *q)
{
	xp->sdt.statem.daedt_state = SDT_STATE_IN_SERVICE;
	xp->sdl.statem.tx_state = SDL_STATE_IN_SERVICE;
	xp->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING);
}

STATIC noinline fastcall __unlikely void
sl_txc_start(struct xp *xp, queue_t *q)
{
	xp->sl.statem.forced_retransmission = 0;	/* ok if basic */
	xp->sl.statem.sib_received = 0;
	xp->sl.statem.Ct = 0;
	xp->sl.statem.rtb_full = 0;
	xp->sl.statem.clear_rtb = 0;	/* ok if ITU */
	if (((xp->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
	    && ((xp->option.pvar & SS7_PVAR_YR) > SS7_PVAR_88)) {
		xp->sl.statem.tx.sio = LSSU_SIOS;
		xp->sl.statem.lssu_available = 1;
	}
	xp->sl.statem.msu_inhibited = 0;
	if (xp->option.popt & SS7_POPT_XSN) {
		xp->sl.statem.sn_mask = 0x7fff;
		xp->sl.statem.ib_mask = 0x8000;
	} else {
		xp->sl.statem.sn_mask = 0x7f;
		xp->sl.statem.ib_mask = 0x80;
	}
	xp->sl.statem.tx.L.fsn = xp->sl.statem.tx.N.fsn = xp->sl.statem.sn_mask;
	xp->sl.statem.tx.X.fsn = 0;
	xp->sl.statem.tx.N.fib = xp->sl.statem.tx.N.bib = xp->sl.statem.ib_mask;
	xp->sl.statem.tx.F.fsn = 0;
	xp->sl.statem.Cm = 0;
	xp->sl.statem.Z = 0;
	xp->sl.statem.z_ptr = NULL;	/* ok if basic */
	if (xp->sl.statem.txc_state == SL_STATE_IDLE) {
		if (((xp->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		    || ((xp->option.pvar & SS7_PVAR_YR) <= SS7_PVAR_88))
			xp->sl.statem.lssu_available = 0;
		ctrace(sl_daedt_start(xp, q));
	}
	xp->sl.statem.txc_state = SL_STATE_IN_SERVICE;
	return;
}

STATIC noinline fastcall __unlikely void
sl_lsc_start(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		ctrace(sl_rc_start(xp, q));
		ctrace(sl_txc_start(xp, q));	/* Note 2 */
		if (xp->sl.statem.emergency)
			sl_iac_emergency(xp, q);
		sl_iac_start(xp, q);
		xp->sl.statem.lsc_state = SL_STATE_INITIAL_ALIGNMENT;
	}
}

/*
 *  Note 2: There is a difference here between ANSI_92 and ITUT_93/96 in that
 *  the transmitters in the ANSI_92 case may transmit one or two SIOSs before
 *  transmitting the first SIO of the initial alignment procedure.  ITUT will
 *  continue idling FISU or LSSU as before the start, then transmit the first
 *  SIO.  These are equivalent.  Because the LSC is in the OUT OF SERVICE
 *  state, the transmitters should be idling SIOS anyway.
 */

STATIC noinline fastcall __unlikely int
sl_lsc_retrieve_bsnt(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		return sl_rc_retrieve_bsnt(xp, q);
	}
	return (QR_DONE);
}

STATIC noinline fastcall __unlikely int
sl_lsc_retrieval_request_and_fsnc(struct xp *xp, queue_t *q, sl_ulong fsnc)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		return sl_txc_retrieval_request_and_fsnc(xp, q, fsnc);
	}
	return (QR_DONE);
}

STATIC noinline fastcall __unlikely void
sl_aerm_set_ti_to_tin(struct xp *xp, queue_t *q)
{
	if (xp->sdt.statem.aerm_state == SDT_STATE_IDLE)
		xp->sdt.statem.Ti = xp->sdt.config.Tin;
}

/*
 *  This power-on sequence should only be performed once, regardless of how
 *  many times the device driver is opened or closed.  This initializes the
 *  transmitters to send SIOS and should never be changed hence.
 */

STATIC noinline fastcall __unlikely void
sl_lsc_power_on(struct xp *xp, queue_t *q)
{
	switch (xp->sl.statem.lsc_state) {
	case SL_STATE_POWER_OFF:
		ctrace(sl_txc_start(xp, q));	/* Note 3 */
		ctrace(sl_txc_send_sios(xp, q));	/* not necessary for ANSI */
		sl_aerm_set_ti_to_tin(xp, q);
		xp->sl.statem.local_processor_outage = 0;
		xp->sl.statem.emergency = 0;
		xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
}

/*
 *  Note 3: There is a difference here between ANSI_92 and ITUT_93/96 in that
 *  the transmitters in the ITUT case may transmit one or two FISUs before
 *  transmitting SIOS on initial power-up.  ANSI will send SIOS on power-up.
 *  ANSI is the correct procedure as transmitters should always idle SIOS on
 *  power-up.
 */

/*
 *  The transmit congestion algorithm is an implementation dependent algorithm
 *  but is suggested as being based on TB and/or RTB buffer occupancy.  With
 *  STREAMS we can use octet count buffer occupancy over message count
 *  occupancy, because congestion in transmission is more related to octet
 *  count (because it determines transmission latency).
 *
 *  We check the total buffer occupancy and apply the necessary congestion
 *  control signal as per configured abatement, onset and discard thresholds.
 */
STATIC fastcall __hot_write void
sl_check_congestion(struct xp *xp, queue_t *q)
{
	unsigned int occupancy = xp->iq->q_count + xp->sl.tb.q_count + xp->sl.rtb.q_count;
	int old_cong_status = xp->sl.statem.cong_status;
	int old_disc_status = xp->sl.statem.disc_status;
	int multi = xp->option.popt & SS7_POPT_MPLEV;

	switch (xp->sl.statem.cong_status) {
	case 0:
		if (occupancy >= xp->sl.config.tb_onset_1) {
			xp->sl.statem.cong_status = 1;
			if (occupancy >= xp->sl.config.tb_discd_1) {
				xp->sl.statem.disc_status = 1;
				if (!multi)
					break;
				if (occupancy >= xp->sl.config.tb_onset_2) {
					xp->sl.statem.cong_status = 2;
					if (occupancy >= xp->sl.config.tb_discd_2) {
						xp->sl.statem.disc_status = 2;
						if (occupancy >= xp->sl.config.tb_onset_3) {
							xp->sl.statem.cong_status = 3;
							if (occupancy >= xp->sl.config.tb_discd_3) {
								xp->sl.statem.disc_status = 3;
							}
						}
					}
				}
			}
		}
		break;
	case 1:
		if (occupancy < xp->sl.config.tb_abate_1) {
			xp->sl.statem.cong_status = 0;
			xp->sl.statem.disc_status = 0;
		} else {
			if (!multi)
				break;
			if (occupancy >= xp->sl.config.tb_onset_2) {
				xp->sl.statem.cong_status = 2;
				if (occupancy >= xp->sl.config.tb_discd_2) {
					xp->sl.statem.disc_status = 2;
					if (occupancy >= xp->sl.config.tb_onset_3) {
						xp->sl.statem.cong_status = 3;
						if (occupancy >= xp->sl.config.tb_discd_3) {
							xp->sl.statem.disc_status = 3;
						}
					}
				}
			}
		}
		break;
	case 2:
		if (!multi) {
			xp->sl.statem.cong_status = 1;
			xp->sl.statem.disc_status = 1;
			break;
		}
		if (occupancy < xp->sl.config.tb_abate_2) {
			xp->sl.statem.cong_status = 1;
			xp->sl.statem.disc_status = 1;
			if (occupancy < xp->sl.config.tb_abate_1) {
				xp->sl.statem.cong_status = 0;
				xp->sl.statem.disc_status = 0;
			}
		} else if (occupancy >= xp->sl.config.tb_onset_3) {
			xp->sl.statem.cong_status = 3;
			if (occupancy >= xp->sl.config.tb_discd_3) {
				xp->sl.statem.disc_status = 3;
			}
		}
		break;
	case 3:
		if (!multi) {
			xp->sl.statem.cong_status = 1;
			xp->sl.statem.disc_status = 1;
			break;
		}
		if (occupancy < xp->sl.config.tb_abate_3) {
			xp->sl.statem.cong_status = 2;
			xp->sl.statem.disc_status = 2;
			if (occupancy < xp->sl.config.tb_abate_2) {
				xp->sl.statem.cong_status = 1;
				xp->sl.statem.disc_status = 1;
				if (occupancy < xp->sl.config.tb_abate_1) {
					xp->sl.statem.cong_status = 0;
					xp->sl.statem.disc_status = 0;
				}
			}
		}
		break;
	}
	if (xp->sl.statem.cong_status != old_cong_status
	    || xp->sl.statem.disc_status != old_disc_status) {
		if (xp->sl.statem.cong_status < old_cong_status)
			sl_link_congestion_ceased_ind(xp, q, xp->sl.statem.cong_status,
						      xp->sl.statem.disc_status);
		else {
			if (xp->sl.statem.cong_status > old_cong_status) {
				if (xp->sl.notify.events & SL_EVT_CONGEST_ONSET_IND &&
				    !xp->sl.stats.sl_cong_onset_ind[xp->sl.statem.cong_status]++) {
					sl_link_congested_ind(xp, q, xp->sl.statem.cong_status,
							      xp->sl.statem.disc_status);
					return;
				}
			} else {
				if (xp->sl.notify.events & SL_EVT_CONGEST_DISCD_IND &&
				    !xp->sl.stats.sl_cong_discd_ind[xp->sl.statem.disc_status]++) {
					sl_link_congested_ind(xp, q, xp->sl.statem.cong_status,
							      xp->sl.statem.disc_status);
					return;
				}
			}
			sl_link_congested_ind(xp, q, xp->sl.statem.cong_status,
					      xp->sl.statem.disc_status);
		}
	}
}

STATIC inline fastcall __hot_write void
sl_txc_message_for_transmission(struct xp *xp, queue_t *q, mblk_t *mp)
{
	bufq_queue(&xp->sl.tb, mp);
	xp->sl.statem.Cm++;
	sl_check_congestion(xp, q);
}

STATIC inline fastcall __hot_write int
sl_lsc_pdu(struct xp *xp, queue_t *q, mblk_t *mp)
{
	mblk_t *dp = mp;
	int hlen = (xp->option.popt & SS7_POPT_XSN) ? 6 : 3;

	ensure(dp, return (-EFAULT));
	if (xp->sl.tb.q_count > 1024)
		return (-ENOBUFS);
	if (dp->b_datap->db_type == M_DATA)
		mp = NULL;
	else
		dp = mp->b_cont;
	ensure(dp, return (-EFAULT));
	if (mp) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr = mp->b_datap->db_base;
		mp->b_wptr = mp->b_rptr + hlen;
	} else if (dp->b_datap->db_base - dp->b_rptr >= hlen) {
		mp = dp;
		mp->b_rptr -= hlen;
	} else if ((mp = ss7_allocb(q, hlen, BPRI_MED))) {
		mp->b_cont = dp;
		mp->b_wptr = mp->b_rptr + hlen;
	} else {
		return (-ENOBUFS);
	}
	sl_txc_message_for_transmission(xp, q, mp);
	return (QR_ABSORBED);
}

STATIC noinline fastcall void
sl_aerm_su_in_error(struct xp *xp, queue_t *q)
{
	if (xp->sdt.statem.aerm_state == SDT_STATE_MONITORING) {
		xp->sdt.statem.Ca++;
		if (xp->sdt.statem.Ca == xp->sdt.statem.Ti) {
			xp->sdt.statem.aborted_proving = 1;
			sl_iac_abort_proving(xp, q);
			xp->sdt.statem.Ca--;
			xp->sdt.statem.aerm_state = SDT_STATE_IDLE;
		}
	}
}

STATIC inline fastcall __hot_in void
sl_aerm_correct_su(struct xp *xp, queue_t *q)
{
	if (xp->sdt.statem.aerm_state == SDT_STATE_IDLE) {
		if (xp->sdt.statem.aborted_proving) {
			sl_iac_correct_su(xp, q);
			xp->sdt.statem.aborted_proving = 0;
		}
	}
}

STATIC noinline fastcall void
sl_suerm_su_in_error(struct xp *xp, queue_t *q)
{
	if (xp->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		xp->sdt.statem.Cs++;
		if (xp->sdt.statem.Cs >= xp->sdt.config.T) {
			sl_lsc_link_failure(xp, q, SL_FAIL_SUERM_EIM);
			xp->sdt.statem.Ca--;
			xp->sdt.statem.suerm_state = SDT_STATE_IDLE;
			return;
		}
		xp->sdt.statem.Ns++;
		if (xp->sdt.statem.Ns >= xp->sdt.config.D) {
			xp->sdt.statem.Ns = 0;
			if (xp->sdt.statem.Cs)
				xp->sdt.statem.Cs--;
		}
	}
}

STATIC noinline fastcall void
sl_eim_su_in_error(struct xp *xp, queue_t *q)
{
	if (xp->sdt.statem.eim_state == SDT_STATE_MONITORING)
		xp->sdt.statem.interval_error = 1;
}

STATIC inline fastcall __hot_in void
sl_suerm_correct_su(struct xp *xp, queue_t *q)
{
	if (xp->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		xp->sdt.statem.Ns++;
		if (xp->sdt.statem.Ns >= xp->sdt.config.D) {
			xp->sdt.statem.Ns = 0;
			if (xp->sdt.statem.Cs)
				xp->sdt.statem.Cs--;
		}
	}
}

STATIC inline fastcall __hot_in void
sl_eim_correct_su(struct xp *xp, queue_t *q)
{
	if (xp->sdt.statem.eim_state == SDT_STATE_MONITORING)
		xp->sdt.statem.su_received = 1;
}

STATIC inline fastcall __hot_in void
sl_daedr_correct_su(struct xp *xp, queue_t *q)
{
	sl_eim_correct_su(xp, q);
	sl_suerm_correct_su(xp, q);
	sl_aerm_correct_su(xp, q);
}

/*
 *  Hooks to Soft-HDLC
 *  -----------------------------------
 */
STATIC noinline fastcall void
sl_daedr_su_in_error(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		sl_eim_su_in_error(xp, q);
		sl_suerm_su_in_error(xp, q);
		sl_aerm_su_in_error(xp, q);
	} else {
		/* cancel compression */
		if (xp->rx.cmp) {
			printd(("SU in error\n"));
			ss7_fast_freemsg(&xp_bufpool, xchg(&xp->rx.cmp, NULL));
		}
	}
	return;
}

STATIC inline fastcall __hot_in void
sl_daedr_received_bits(struct xp *xp, queue_t *q, mblk_t *mp)
{
	if (xp->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		sl_rc_signal_unit(xp, q, mp);
		sl_daedr_correct_su(xp, q);
	} else {
		int i, len, mlen = (xp->option.popt & SS7_POPT_XSN) ? 8 : 5;

		if (mp) {
			len = msgdsize(mp);
			if (xp->rx.cmp) {
#if 0
				if (xp->rx.repeat > 50)
					goto no_match;
#endif
				if (len > mlen || len != msgdsize(xp->rx.cmp))
					goto no_match;
				for (i = 0; i < len; i++)
					if (mp->b_rptr[i] != xp->rx.cmp->b_rptr[i])
						goto no_match;
				xp->rx.repeat++;
				xp->sdt.stats.rx_sus_compressed++;
				freemsg(mp);
				return;
			      no_match:
				if (xp->rx.repeat) {
#if 0
					mblk_t *cd;

					if ((cd = dupb(xp->rx.cmp)))
						if (sdt_rc_signal_unit_ind(xp, q, cd, xp->rx.repeat)
						    != QR_ABSORBED) {
							xp->sdt.stats.rx_buffer_overflows++;
							freeb(cd);
						}
#endif
					xp->rx.repeat = 0;
				}
			}
			if (len <= mlen) {
				if (xp->rx.cmp
				    || (xp->rx.cmp = ss7_fast_allocb(&xp_bufpool, mlen, BPRI_HI))) {
					bcopy(mp->b_rptr, xp->rx.cmp->b_rptr, len);
					xp->rx.cmp->b_wptr = xp->rx.cmp->b_rptr + len;
					xp->rx.repeat = 0;
				}
			}
			if (sdt_rc_signal_unit_ind(xp, q, mp, 1) != QR_ABSORBED) {
				xp->sdt.stats.rx_buffer_overflows++;
				freemsg(mp);
			}
		} else
			swerr();
	}
}

STATIC inline fastcall __hot_out mblk_t *
sl_daedt_transmission_request(struct xp *xp, queue_t *q)
{
	mblk_t *mp;

	if (xp->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
		if (!(mp = sl_txc_transmission_request(xp, q)))
			xp->sdt.stats.tx_buffer_overflows++;
		return (mp);
	} else if (xp->sdt.statem.daedt_state != SDT_STATE_IDLE) {
		if ((mp = bufq_dequeue(&xp->sdt.tb))) {
			int len = msgdsize(mp);
			int hlen = (xp->option.popt & SS7_POPT_XSN) ? 6 : 3;
			int mlen = hlen + 2;

			if (!xp->sdt.tb.q_count < 512 && xp->iq->q_count)
				qenable(xp->iq);	/* back-enable */
			if (mlen < hlen)
				goto dont_repeat;
			if (mlen == len + 1 || mlen == len + 2) {
				int li, sio;

				if (xp->option.popt & SS7_POPT_XSN) {
					li = ((mp->b_rptr[5] << 8) | mp->b_rptr[4]) & 0x1ff;
					sio = mp->b_rptr[6];
				} else {
					li = mp->b_rptr[2] & 0x3f;
					sio = mp->b_rptr[3];
				}
				if (li != mlen - len)
					goto dont_repeat;
				switch (sio) {
				case LSSU_SIO:
				case LSSU_SIN:
				case LSSU_SIE:
				case LSSU_SIOS:
				case LSSU_SIPO:
					break;
				case LSSU_SIB:
				default:
					goto dont_repeat;
				}
			}
			if (xp->tx.cmp
			    || (xp->tx.cmp = ss7_fast_allocb(&xp_bufpool, mlen, BPRI_HI))) {
				mblk_t *cd = xp->tx.cmp;

				if (len > mlen)
					len = hlen;
				cd->b_rptr = cd->b_datap->db_base;
				bcopy(mp->b_rptr, cd->b_rptr, len);
				cd->b_wptr = cd->b_rptr + len;
				/* always correct length indicator */
				if (xp->option.popt & SS7_POPT_XSN) {
					cd->b_rptr[4] &= 0x00;
					cd->b_rptr[5] &= 0xfe;
					cd->b_rptr[4] += (len - hlen);
				} else {
					cd->b_rptr[2] &= 0xc0;
					cd->b_rptr[2] += (len - hlen);
				}
				xp->tx.repeat = 0;
				return (mp);
			}
		      dont_repeat:
			if (xp->tx.cmp)
				ss7_fast_freemsg(&xp_bufpool, xchg(&xp->tx.cmp, NULL));
		} else {
			if ((mp = xp->tx.cmp)) {
				mp->b_rptr = mp->b_datap->db_base;
				xp->sdt.stats.tx_sus_repeated++;
				xp->tx.repeat++;
			} else
				xp->sdt.stats.tx_buffer_overflows++;
		}
		return (mp);
	} else if (xp->sdl.statem.tx_state != SDL_STATE_IDLE) {
		if ((mp = bufq_dequeue(&xp->sdl.tb))) {
			if (xp->sdl.tb.q_count < 32 && xp->iq->q_count)
				qenable(xp->iq);
		} else
			xp->sdl.stats.tx_buffer_overflows++;
		return (mp);
	} else {
		return (NULL);
	}
}

/*
 *  ========================================================================
 *
 *  Events from below
 *
 *  ========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  SL events from below (timeouts)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T1 EXPIRY
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
xp_t1_timeout(struct xp *xp, queue_t *q)
{
	int err;

	if ((err = sl_out_of_service_ind(xp, q, SL_FAIL_T1_TIMEOUT)))
		return (err);
	xp->sl.statem.failure_reason = SL_FAIL_T1_TIMEOUT;
	sl_rc_stop(xp, q);
	sl_suerm_stop(xp, q);
	ctrace(sl_txc_send_sios(xp, q));
	xp->sl.statem.emergency = 0;
	if (xp->sl.statem.lsc_state == SL_STATE_ALIGNED_NOT_READY) {
		sl_poc_stop(xp, q);	/* ok if ANSI */
		xp->sl.statem.local_processor_outage = 0;
	}
	xp->sl.statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	return (QR_DONE);
}

/*
 *  T2 EXPIRY
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
xp_t2_timeout(struct xp *xp, queue_t *q)
{
	int err;

	if (xp->sl.statem.iac_state == SL_STATE_NOT_ALIGNED) {
		if ((err = ctrace(sl_lsc_alignment_not_possible(xp, q))))
			return (err);
		xp->sl.statem.emergency = 0;
		xp->sl.statem.iac_state = SL_STATE_IDLE;
	}
	return (QR_DONE);
}

/*
 *  T3 EXPIRY
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
xp_t3_timeout(struct xp *xp, queue_t *q)
{
	int err;

	if (xp->sl.statem.iac_state == SL_STATE_ALIGNED) {
		if ((err = ctrace(sl_lsc_alignment_not_possible(xp, q))))
			return (err);
		xp->sl.statem.emergency = 0;
		xp->sl.statem.iac_state = SL_STATE_IDLE;
	}
	return (QR_DONE);
}

/*
 *  T4 EXPIRY
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
xp_t4_timeout(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.iac_state == SL_STATE_PROVING) {
		if (xp->sl.statem.further_proving) {
			sl_aerm_start(xp, q);
			ctrace(xp_timer_start(xp, t4));
			xp->sl.statem.further_proving = 0;
		} else {
			sl_lsc_alignment_complete(xp, q);
			sl_aerm_stop(xp, q);
			xp->sl.statem.emergency = 0;
			xp->sl.statem.iac_state = SL_STATE_IDLE;
		}
	}
	return (QR_DONE);
}

/*
 *  T5 EXPIRY
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
xp_t5_timeout(struct xp *xp, queue_t *q)
{
	if (xp->sl.statem.cc_state == SL_STATE_BUSY) {
		ctrace(sl_txc_send_sib(xp, q));
		xp_timer_start(xp, t5);
	}
	return (QR_DONE);
}

/*
 *  T6 EXPIRY
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
xp_t6_timeout(struct xp *xp, queue_t *q)
{
	int err;

	if ((err = sl_lsc_link_failure(xp, q, SL_FAIL_CONG_TIMEOUT)))
		return (err);
	xp->sl.statem.sib_received = 0;
	xp_timer_stop(xp, t7);
	return (QR_DONE);
}

/*
 *  T7 EXPIRY
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
xp_t7_timeout(struct xp *xp, queue_t *q)
{
	int err;

	if ((err = sl_lsc_link_failure(xp, q, SL_FAIL_ACK_TIMEOUT)))
		return (err);
	xp_timer_stop(xp, t6);
	if (((xp->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
	    || ((xp->option.pvar & SS7_PVAR_YR) <= SS7_PVAR_88))
		xp->sl.statem.sib_received = 0;
	return (QR_DONE);
}

/*
 *  T8 EXPIRY
 *  -----------------------------------
 */
STATIC noinline fastcall int
xp_t8_timeout(struct xp *xp, queue_t *q)
{
	int err;

	if (xp->sdt.statem.eim_state == SDT_STATE_MONITORING) {
		xp_timer_start(xp, t8);
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
			if ((err = sl_lsc_link_failure(xp, q, SL_FAIL_SUERM_EIM))) {
				xp->sdt.statem.Ce -= xp->sdt.config.Ue;
				return (err);
			}
			xp->sdt.statem.eim_state = SDT_STATE_IDLE;
		}
		xp->sdt.statem.interval_error = 0;
	}
	return (QR_DONE);
}

/*
 *  ========================================================================
 *
 *  Soft-HDLC
 *
 *  ========================================================================
 */
#define SDT_TX_STATES	5
#define SDT_RX_STATES	16

#define SDT_TX_BUFSIZE	PAGE_SIZE
#define SDT_RX_BUFSIZE	PAGE_SIZE

#define SDT_CRC_TABLE_LENGTH	512
#define SDT_TX_TABLE_LENGTH	(2* SDT_TX_STATES * 256)
#define SDT_RX_TABLE_LENGTH	(2* SDT_RX_STATES * 256)

typedef struct tx_entry {
	uint bit_string:10 __attribute__ ((packed));	/* the output string */
	uint bit_length:2 __attribute__ ((packed));	/* length in excess of 8 bits of output
							   string */
	uint state:3 __attribute__ ((packed));	/* new state */
} tx_entry_t;

typedef struct rx_entry {
	uint bit_string:12 __attribute__ ((packed));
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

/*
 *  TX BITSTUFF
 *  -----------------------------------
 *  Bitstuff an octet and shift residue for output.
 */
STATIC inline fastcall __hot_out void
xp_tx_bitstuff(xp_path_t * tx, uchar byte)
{
	tx_entry_t *t = tx_index(tx->state, byte);

	tx->state = t->state;
	tx->residue <<= 8 + t->bit_length;
	tx->residue |= t->bit_string;
	tx->rbits += 8 + t->bit_length;
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

STATIC noinline fastcall __hot_out void
xp_tx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats, const sl_ulong type)
{
	queue_t *q = xp->iq;
	register xp_path_t *tx = &xp->tx;
	int chan = 0;

	if (xp->sdt.statem.daedt_state != SDT_STATE_IDLE) {
		if (tx->mode == TX_MODE_IDLE || tx->mode == TX_MODE_FLAG) {
			if (!tx->nxt) {
			      next_message:
				if (tx->msg && tx->msg != tx->cmp)
					freemsg(tx->msg);
				if ((tx->msg = tx->nxt = sl_daedt_transmission_request(xp, q)))
					tx->mode = TX_MODE_BOF;
			}
		}
		/* check if transmission block complete */
		for (; bp < be; bp += 128) {
		      check_rbits:
			/* drain residue bits, if necessary */
			if (tx->rbits >= 8) {
			      drain_rbits:
				/* drain residue bits */
				tx->rbits -= 8;
				switch (type) {
				default:
				case SDL_TYPE_DS0:
				case SDL_TYPE_DS0A:
					_printd(("Tx: %p: 0x%02x\n", xp, xp_rev(*bp)));
					*bp = tx->residue >> tx->rbits;
					break;
				case SDL_TYPE_T1:
				case SDL_TYPE_J1:
					*(bp + (xp_t1_chan_map[chan] << 2)) =
					    tx->residue >> tx->rbits;
					if (++chan > 23)
						chan = 0;
					break;
				case SDL_TYPE_E1:
					*(bp + (xp_e1_chan_map[chan] << 2)) =
					    tx->residue >> tx->rbits;
					if (++chan > 30)
						chan = 0;
					break;
				}
				if (chan)
					goto check_rbits;
				continue;
			}
			switch (tx->mode) {
			case TX_MODE_IDLE:
				/* mark idle */
				tx->residue <<= 8;
				tx->residue |= 0xff;
				tx->rbits += 8;
				goto drain_rbits;
			case TX_MODE_FLAG:
				/* idle flags */
				tx->residue <<= 8;
				tx->residue |= 0x7e;
				tx->rbits += 8;
				goto drain_rbits;
			case TX_MODE_BOF:
				/* add opening flag (also closing flag) */
				switch (xp->sdt.config.f) {
				default:
				case SDT_FLAGS_ONE:
					tx->residue <<= 8;
					tx->residue |= 0x7e;
					tx->rbits += 8;
					break;
				case SDT_FLAGS_SHARED:
					tx->residue <<= 15;
					tx->residue |= 0x3f7e;
					tx->rbits += 15;
					break;
				case SDT_FLAGS_TWO:
					tx->residue <<= 16;
					tx->residue |= 0x7e7e;
					tx->rbits += 16;
					break;
				case SDT_FLAGS_THREE:
					tx->residue <<= 24;
					tx->residue |= 0x7e7e7e;
					tx->rbits += 24;
					break;
				}
				tx->state = 0;
				tx->bcc = 0xffff;
				tx->mode = TX_MODE_MOF;
				goto drain_rbits;
			case TX_MODE_MOF:	/* transmit frame bytes */
				if (tx->nxt->b_rptr < tx->nxt->b_wptr
				    || (tx->nxt = tx->nxt->b_cont)) {
					/* continuing in message */
					uint byte = *tx->nxt->b_rptr++;

					tx->bcc =
					    (tx->bcc >> 8) ^ bc_table[(tx->bcc ^ byte) & 0x00ff];
					xp_tx_bitstuff(tx, byte);
					stats->tx_bytes++;
					goto drain_rbits;
				}
				/* finished message: add 1st bcc byte */
				tx->bcc = ~(tx->bcc);
				xp_tx_bitstuff(tx, tx->bcc);
				tx->mode = TX_MODE_BCC;
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
	} else if (xp->sdl.statem.tx_state != SDL_STATE_IDLE) {
		for (; bp < be; bp += 128) {
			do {
				if (tx->nxt) {
					do {
						if (tx->nxt->b_rptr < tx->nxt->b_wptr)
							goto tx_process_block;
					} while ((tx->nxt = tx->nxt->b_cont));
				}
				/* next block */
				if (tx->msg)
					ss7_fast_freemsg(&xp_bufpool, tx->msg);
				if (!(tx->msg = tx->nxt = sl_daedt_transmission_request(xp, q))) {
					xp->sdl.stats.tx_buffer_overflows++;
					return;
				}
			      tx_process_block:
				switch (type) {
				default:
				case SDL_TYPE_DS0:
				case SDL_TYPE_DS0A:
					*bp = *tx->nxt->b_rptr++;
					chan = 0;
					break;
				case SDL_TYPE_T1:
				case SDL_TYPE_J1:
					*(bp + (xp_t1_chan_map[chan] << 2)) = *tx->nxt->b_rptr++;
					if (++chan > 23)
						chan = 0;
					break;
				case SDL_TYPE_E1:
					*(bp + (xp_e1_chan_map[chan] << 2)) = *tx->nxt->b_rptr++;
					if (++chan > 30)
						chan = 0;
					break;
				}
				xp->sdl.stats.tx_octets++;
				while (tx->nxt && tx->nxt->b_rptr >= tx->nxt->b_wptr)
					tx->nxt = tx->nxt->b_cont;
			}
			while (chan);
		}
	}
}

/* force 4 separate versions for speed */
STATIC inline fastcall __hot_out void
xp_ds0a_tx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_DS0A);
}
STATIC noinline fastcall __hot_out void
xp_ds0_tx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_DS0);
}
STATIC inline fastcall __hot_out void
xp_t1_tx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_T1);
}
STATIC inline fastcall __hot_out void
xp_j1_tx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_J1);
}
STATIC inline fastcall __hot_out void
xp_e1_tx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_block(xp, bp, be, stats, SDL_TYPE_E1);
}

STATIC noinline fastcall __hot_out void
xp_tx_idle(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats, const sl_ulong type)
{
	int chan;

	(void) xp;
	(void) stats;
	for (; bp < be; bp += 128) {
		switch (type) {
		default:
		case SDL_TYPE_DS0:
			*bp = 0xff;
			continue;
		case SDL_TYPE_DS0A:
			*bp = 0x7f;
			continue;
		case SDL_TYPE_T1:
		case SDL_TYPE_J1:
			for (chan = 0; chan < 24; chan++)
				*(bp + (xp_t1_chan_map[chan] << 2)) = 0x7f;
			continue;
		case SDL_TYPE_E1:
			for (chan = 0; chan < 31; chan++)
				*(bp + (xp_e1_chan_map[chan] << 2)) = 0xff;
			continue;
		}
	}
}

/* force 4 separate versions for speed */
STATIC inline fastcall __hot_out void
xp_ds0a_tx_idle(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_idle(xp, bp, be, stats, SDL_TYPE_DS0A);
}
STATIC inline fastcall __hot_out void
xp_ds0_tx_idle(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_idle(xp, bp, be, stats, SDL_TYPE_DS0);
}
STATIC inline fastcall __hot_out void
xp_t1_tx_idle(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_idle(xp, bp, be, stats, SDL_TYPE_T1);
}
STATIC inline fastcall __hot_out void
xp_j1_tx_idle(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_idle(xp, bp, be, stats, SDL_TYPE_J1);
}
STATIC inline fastcall __hot_out void
xp_e1_tx_idle(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_tx_idle(xp, bp, be, stats, SDL_TYPE_E1);
}

/*
 *  RX LINKB
 *  ----------------------------------------
 *  Link a buffer to existing message or create new message with buffer.
 */
STATIC inline fastcall __hot_in void
xp_rx_linkb(xp_path_t * rx)
{
	if (rx->msg)
		linkb(rx->msg, rx->nxt);
	else
		rx->msg = rx->nxt;
	rx->nxt = NULL;
	return;
}

#define RX_MODE_IDLE	0
#define RX_MODE_HUNT	1	/* hunting for flags */
#define RX_MODE_SYNC	2	/* between frames */
#define RX_MODE_MOF	3	/* middle of frame */

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
STATIC noinline fastcall __hot_in void
xp_rx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats, const sl_ulong type)
{
	int chan = 0;
	queue_t *q = xp->oq;
	register xp_path_t *rx = &xp->rx;

	if (xp->sdt.statem.daedr_state != SDT_STATE_IDLE) {
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
				_printd(("Rx: %p: 0x%02x\n", xp, *bp));
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
			case SDL_TYPE_J1:
				r = rx_index7(rx->state, *(bp + (xp_t1_chan_map[chan] << 2)));
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
					if (r->hunt || r->idle)
						goto aborted;
					while (rx->rbits > 16) {
						if (rx->nxt
						    && rx->nxt->b_wptr >= rx->nxt->b_datap->db_lim)
							xp_rx_linkb(rx);
						if (!rx->nxt &&
						    !(rx->nxt =
						      ss7_fast_allocb(&xp_bufpool, FASTBUF,
								      BPRI_HI)))
							goto buffer_overflow;
						rx->bcc = (rx->bcc >> 8)
						    ^ bc_table[(rx->bcc ^ rx->residue) & 0x00ff];
						*rx->nxt->b_wptr++ = rx->residue;
						stats->rx_bytes++;
						rx->residue >>= 8;
						rx->rbits -= 8;
						rx->bytes++;
						if (rx->bytes > M)
							goto frame_too_long;
					}
				} else {
					uint len, li;

					if (rx->rbits != 16)
						goto residue_error;
					if ((~rx->bcc & 0xffff) != (rx->residue & 0xffff))
						goto crc_error;
					if (rx->bytes < hlen)
						goto frame_too_short;
					xp_rx_linkb(rx);
					if (!xsn)
						li = rx->msg->b_rptr[2] & mlen;
					else
						li = ((rx->msg->b_rptr[5] << 8) | rx->msg->
						      b_rptr[4]) & mlen;
					len = rx->bytes - hlen;
					if (len != li && (li != mlen || len <= li))
						goto length_error;
					stats->rx_sus++;
					sl_daedr_received_bits(xp, q, xchg(&rx->msg, NULL));
				      new_frame:
					rx->mode = RX_MODE_SYNC;
					rx->residue = 0;
					rx->rbits = 0;
					rx->bytes = 0;
					rx->bcc = 0xffff;
					if (r->sync) {
					      begin_frame:
						if (r->bit_length) {
							rx->mode = RX_MODE_MOF;
							rx->residue = r->bit_string;
							rx->rbits = r->bit_length;
							rx->bytes = 0;
							rx->bcc = 0xffff;
						}
					}
				}
				break;
			      frame_too_long:
				stats->rx_frame_too_long++;
				stats->rx_frame_errors++;
				goto abort_frame;
			      buffer_overflow:
				stats->rx_buffer_overflows++;
				goto abort_frame;
			      aborted:
				stats->rx_aborts++;
				stats->rx_frame_errors++;
				goto abort_frame;
			      length_error:
				stats->rx_length_error++;
				goto abort_frame;
			      frame_too_short:
				stats->rx_frame_too_short++;
				stats->rx_frame_errors++;
				goto abort_frame;
			      crc_error:
				stats->rx_crc_errors++;
				goto abort_frame;
			      residue_error:
				stats->rx_residue_errors++;
				stats->rx_frame_errors++;
				goto abort_frame;
			      abort_frame:
				if (rx->nxt)
					ss7_fast_freemsg(&xp_bufpool, xchg(&rx->nxt, NULL));
				if (rx->msg)
					ss7_fast_freemsg(&xp_bufpool, xchg(&rx->msg, NULL));
				stats->rx_sus_in_error++;
				sl_daedr_su_in_error(xp, q);
				if (r->flag)
					goto new_frame;
				goto start_hunt;
			case RX_MODE_SYNC:
				if (!r->hunt && !r->idle)
					goto begin_frame;
			      start_hunt:
				if (r->idle)
					rx->mode = RX_MODE_IDLE;
				else
					rx->mode = RX_MODE_HUNT;
				stats->rx_sync_transitions++;
				rx->octets = 0;
				break;
			case RX_MODE_IDLE:
				if (!r->idle)
					rx->mode = RX_MODE_HUNT;
				/* fall through */
			case RX_MODE_HUNT:
				if (!r->flag) {
					if (r->idle && rx->mode != RX_MODE_IDLE)
						rx->mode = RX_MODE_IDLE;
					if ((++rx->octets) >= N) {
						stats->rx_sus_in_error++;
						sl_daedr_su_in_error(xp, q);
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
	} else if (xp->sdl.statem.rx_state != SDL_STATE_IDLE) {
		for (; bp < be; bp += 128) {
			do {
				if (rx->nxt) {
					if (rx->nxt->b_wptr <
					    rx->nxt->b_rptr + xp->sdl.config.ifblksize)
						goto rx_process_block;
					if (sdl_received_bits_ind(xp, q, rx->nxt) != QR_ABSORBED)
						goto rx_process_block;
					xp->sdl.stats.rx_buffer_overflows++;
				}
				/* new block */
				if (!(rx->nxt = ss7_fast_allocb(&xp_bufpool, FASTBUF, BPRI_HI))) {
					xp->sdl.stats.rx_buffer_overflows++;
					return;
				}
			      rx_process_block:
				switch (type) {
				default:
				case SDL_TYPE_DS0:
				case SDL_TYPE_DS0A:
					*rx->nxt->b_wptr++ = *bp;
					chan = 0;
					break;
				case SDL_TYPE_T1:
				case SDL_TYPE_J1:
					*rx->nxt->b_wptr++ = *(bp + (xp_t1_chan_map[chan] << 2));
					if (++chan > 23)
						chan = 0;
					break;
				case SDL_TYPE_E1:
					*rx->nxt->b_wptr++ = *(bp + (xp_e1_chan_map[chan] << 2));
					if (++chan > 30)
						chan = 0;
					break;
				}
				xp->sdl.stats.rx_octets++;
			} while (chan);
		}
	}
}
STATIC inline fastcall __hot_in void
xp_ds0a_rx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_rx_block(xp, bp, be, stats, SDL_TYPE_DS0A);
}
STATIC inline fastcall __hot_in void
xp_ds0_rx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_rx_block(xp, bp, be, stats, SDL_TYPE_DS0);
}
STATIC inline fastcall __hot_in void
xp_t1_rx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_rx_block(xp, bp, be, stats, SDL_TYPE_T1);
}
STATIC inline fastcall __hot_in void
xp_j1_rx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
{
	xp_rx_block(xp, bp, be, stats, SDL_TYPE_J1);
}
STATIC inline fastcall __hot_in void
xp_e1_rx_block(struct xp *xp, uchar *bp, uchar *be, sdt_stats_t * stats)
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
STATIC __devinit bc_entry_t
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
 *  TX table performs zero insertion and bit reversal on frame and CRC bit streams.
 */
static __devinit struct tx_entry
tx_table_valueN(int state, uint8_t byte, int len)
{
	struct tx_entry result = { 0, };
	int bit_mask = 0x80;

	result.state = state;
	result.bit_length = 0;
	while (len--) {
		if (byte & 0x01) {
			result.bit_string |= bit_mask;
			if (result.state++ == 4) {
				result.state = 0;
				result.bit_length++;
				result.bit_string <<= 1;
			}
		} else
			result.state = 0;
		bit_mask >>= 1;
		byte >>= 1;
	}
	return result;
}

STATIC __devinit tx_entry_t
tx_table_value(int state, uint8_t byte)
{
	return tx_table_valueN(state, byte, 8);
}

/*
 *  RX (Receive) Table Entries:
 *  -----------------------------------
 *  RX table performs zero deletion, flag and abort detection, BOF and EOF
 *  detection, residue, and bit reversal on received bit streams.
 */
static __devinit struct rx_entry
rx_table_valueN(int state, uint8_t byte, int len)
{
	struct rx_entry result = { 0, };
	int bit_mask = 1;

	result.state = state;
	while (len--) {
		switch (result.state) {
		case 0:	/* 0 *//* zero not belonging to shared flag nor stuffing bit deletion */
			if (byte & 0x80) {
				result.state = 1;
			} else {
				bit_mask <<= 1;
				result.bit_length += 1;
				result.state = 0;
			}
			break;
		case 1:	/* 01 */
			if (byte & 0x80) {
				result.state = 2;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 2;
				result.state = 0;
			}
			break;
		case 2:	/* 011 */
			if (byte & 0x80) {
				result.state = 3;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 3;
				result.state = 0;
			}
			break;
		case 3:	/* 0111 */
			if (byte & 0x80) {
				result.state = 4;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 4;
				result.state = 0;
			}
			break;
		case 4:	/* 01111 */
			if (byte & 0x80) {
				result.state = 5;
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
				result.state = 0;
			}
			break;
		case 5:	/* 011111 */
			if (byte & 0x80) {
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
				result.state = 6;
			}
			break;
		case 6:	/* [0]11111[0] *//* bit deletion */
			if (byte & 0x80) {
				result.state = 9;
			} else {
				result.state = 0;
			}
			break;
		case 7:	/* 0111111 */
			result.sync = 0;
			result.idle = 0;
			if (byte & 0x80) {
				bit_mask <<= 1;
				result.bit_length += 1;
				result.flag = 0;
				result.hunt = 1;
				result.state = 15;
			} else {
				result.flag = 1;
				result.hunt = 0;
				result.state = 8;
			}
			break;
		case 8:	/* 0111110 */
			bit_mask = 1;
			result.bit_string = 0;
			result.bit_length = 0;
			result.sync = 1;
			result.flag = 1;
			result.hunt = 0;
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 9;
			} else {
				result.state = 0;
			}
			break;
		case 9:	/* [0]1 *//* zero from end of flag or bit deletion */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 10;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 1;
				result.state = 0;
			}
			break;
		case 10:	/* [0]11 */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 11;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 2;
				result.state = 0;
			}
			break;
		case 11:	/* [0]111 */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 12;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 3;
				result.state = 0;
			}
			break;
		case 12:	/* [0]1111 */
			result.idle = 0;
			if (byte & 0x80) {
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
				result.bit_length += 4;
				result.state = 0;
			}
			break;
		case 13:	/* [0]11111 */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 14;
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
				result.state = 6;
			}
			break;
		case 14:	/* [0]111111 */
			result.sync = 0;
			result.idle = 0;
			if (byte & 0x80) {
				result.flag = 0;
				result.hunt = 1;
				result.state = 15;
			} else {
				result.flag = 1;
				result.hunt = 0;
				result.state = 8;
			}
			break;
		case 15:	/* ...1111111 *//* 7 ones (or 8 ones) */
			result.sync = 0;
			result.flag = 0;
			result.hunt = 1;
			if (byte & 0x80) {
				result.idle = 1;
				result.state = 15;
			} else {
				result.idle = 0;
				result.state = 0;
			}
			break;
		}
		byte <<= 1;
	}
	return result;
}

STATIC __devinit rx_entry_t
rx_table_value7(int state, uint8_t byte)
{
	return rx_table_valueN(state, byte, 7);
}

STATIC __devinit rx_entry_t
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
STATIC __devinit void
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
STATIC __devinit void
rx_table_generate7(void)
{
	int j, k;

	for (j = 0; j < SDT_RX_STATES; j++)
		for (k = 0; k < 256; k++)
			*rx_index7(j, k) = rx_table_value7(j, k);
}
STATIC __devinit void
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
STATIC __devinit void
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
STATIC noinline __devinit int
xp_init_tables(void)
{
	size_t length;

	length = SDT_CRC_TABLE_LENGTH * sizeof(bc_entry_t);
	for (bc_order = 0; PAGE_SIZE << bc_order < length; bc_order++) ;
	if (!(bc_table = (bc_entry_t *) __get_free_pages(GFP_KERNEL, bc_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocated bc_table\n", __FUNCTION__);
		goto bc_failed;
	}
	printd(("%s: allocated BC table size %u kernel pages\n", DRV_NAME, 1 << bc_order));
	length = SDT_TX_TABLE_LENGTH * sizeof(tx_entry_t);
	for (tx_order = 0; PAGE_SIZE << tx_order < length; tx_order++) ;
	if (!(tx_table = (tx_entry_t *) __get_free_pages(GFP_KERNEL, tx_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocated tx_table\n", __FUNCTION__);
		goto tx_failed;
	}
	printd(("%s: allocated Tx table size %u kernel pages\n", DRV_NAME, 1 << tx_order));
	length = 2 * (SDT_RX_TABLE_LENGTH * sizeof(rx_entry_t));
	for (rx_order = 0; PAGE_SIZE << rx_order < length; rx_order++) ;
	if (!(rx_table = (rx_entry_t *) __get_free_pages(GFP_KERNEL, rx_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocated rx_table\n", __FUNCTION__);
		goto rx_failed;
	}
	printd(("%s: allocated Rx table size %u kernel pages\n", DRV_NAME, 1 << rx_order));
	rx_table7 = (rx_entry_t *) (((uint8_t *) rx_table) + (PAGE_SIZE << (rx_order - 1)));
	bc_table_generate();
	printd(("%s: generated BC table\n", DRV_NAME));
	tx_table_generate();
	printd(("%s: generated 8-bit Tx table\n", DRV_NAME));
	rx_table_generate8();
	printd(("%s: generated 8-bit Rx table\n", DRV_NAME));
	rx_table_generate7();
	printd(("%s: generated 7-bit Rx table\n", DRV_NAME));
	return (0);
      rx_failed:
	free_pages((unsigned long) tx_table, xchg(&tx_order, 0));
	tx_order = 0;
      tx_failed:
	free_pages((unsigned long) bc_table, xchg(&bc_order, 0));
	bc_order = 0;
      bc_failed:
	return (-ENOMEM);
}
STATIC noinline __devexit int
xp_free_tables(void)
{
	free_pages((unsigned long) bc_table, bc_order);
	printd(("%s: freed BC table kernel pages\n", DRV_NAME));
	free_pages((unsigned long) tx_table, tx_order);
	printd(("%s: freed Tx table kernel pages\n", DRV_NAME));
	free_pages((unsigned long) rx_table, rx_order);
	printd(("%s: freed Rx table kernel pages\n", DRV_NAME));
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
STATIC inline fastcall __hot_write int
xp_send_data(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	int ret;
	psw_t flags = 0;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	/* locks are probably uncecessary here */
	spin_lock_irqsave(&xp->lock, flags);
	{
		if (xp->sl.statem.lsc_state != SL_STATE_POWER_OFF) {
			/* SL mode */
			ret = sl_lsc_pdu(xp, q, mp);
		} else if (xp->sdt.statem.daedt_state != SDT_STATE_IDLE) {
			/* SDT mode */
			if (xp->sdt.tb.q_count > 1024)
				ret = -EBUSY;
			else {
				bufq_queue(&xp->sdt.tb, mp);
				ret = QR_ABSORBED;
			}
		} else if (xp->sdl.statem.tx_state != SDL_STATE_IDLE) {
			/* SDL mode */
			if (xp->sdl.tb.q_count > 64)
				ret = -EBUSY;
			else {
				bufq_queue(&xp->sdl.tb, mp);
				ret = QR_ABSORBED;
			}
		} else {
			/* discard when not active */
			ret = QR_DONE;
		}
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (ret);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC inline fastcall __hot_write int
sl_pdu_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	int ret;
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto discard;
	spin_lock_irqsave(&xp->lock, flags);
	{
		ret = sl_lsc_pdu(xp, q, mp);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (ret);
      discard:
	return (QR_DONE);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_emergency_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_emergency(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_emergency_ceases_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_emergency_ceases(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_start_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_start(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_stop_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_stop(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_retrieve_bsnt_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		err = sl_lsc_retrieve_bsnt(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (err);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_retrieval_request_and_fsnc_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	sl_retrieval_req_and_fsnc_t *p = ((typeof(p)) mp->b_rptr);

	if (mp->b_wptr >= mp->b_rptr + sizeof(*p)) {
		struct xp *xp = XP_PRIV(q);
		int err;
		psw_t flags;

		if (xp->i_state != LMI_ENABLED)
			goto eproto;
		spin_lock_irqsave(&xp->lock, flags);
		{
			err = sl_lsc_retrieval_request_and_fsnc(xp, q, p->sl_fsnc);
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (err);
	}
	swerr();
	return (-EMSGSIZE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_clear_buffers_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		err = sl_lsc_clear_buffers(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (err);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_clear_rtb_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_clear_rtb(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_CONTINUE_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_continue_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_continue(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_local_processor_outage_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_local_processor_outage(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_resume_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_resume(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_congestion_discard_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_congestion_discard(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_congestion_accept_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_congestion_accept(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_no_congestion_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_no_congestion(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_power_on_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		(void) mp;
		sl_lsc_power_on(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

#if 0
/*
 *  SL_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_NOTIFY_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sl_notify_req(queue_t *q, mblk_t *mp)
{
}
#endif

/*
 *  SDT_DAEDT_TRANSMISSION_REQ:
 *  -----------------------------------
 *  Non-preferred way of sending frames.  One should just send M_DATA blocks.
 *  We strip the redundant M_PROTO and put the M_DATA on the queue.
 */
STATIC noinline fastcall __hot_write int
sdt_daedt_transmission_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (QR_STRIP);
}

/*
 *  SDT_DAEDT_START_REQ:
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_daedt_start_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		ctrace(sl_daedt_start(xp, q));
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDT_DAEDR_START_REQ:
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_daedr_start_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		ctrace(sl_daedr_start(xp, q));
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDT_AERM_START_REQ:
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_aerm_start_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		sl_aerm_start(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDT_AERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_aerm_stop_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		sl_aerm_stop(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ:
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_aerm_set_ti_to_tin_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		sl_aerm_set_ti_to_tin(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ:
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_aerm_set_ti_to_tie_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		sl_aerm_set_ti_to_tie(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDT_SUERM_START_REQ:
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_suerm_start_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		sl_suerm_start(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDT_SUERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdt_suerm_stop_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		sl_suerm_stop(xp, q);
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDL_BITS_FOR_TRANSMISSION_REQ
 *  -----------------------------------
 *  Non-preferred method.  Normally one should just send M_DATA blocks.  We
 *  just strip off the redundant M_PROTO and put it on the queue.
 */
STATIC noinline fastcall __hot_write int
sdl_bits_for_transmission_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (QR_STRIP);
}

/*
 *  SDL_CONNECT_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdl_connect_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;
	sdl_connect_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto eproto;
	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		if (p->sdl_flags & SDL_RX_DIRECTION) {
			xp->sdl.config.ifflags |= SDL_IF_RX_RUNNING;
			xp->sdl.statem.rx_state = SDL_STATE_IN_SERVICE;
			ptrace(("%p: Enabling Rx\n", xp));
		}
		if (p->sdl_flags & SDL_TX_DIRECTION) {
			xp->sdl.config.ifflags |= SDL_IF_TX_RUNNING;
			xp->sdl.statem.tx_state = SDL_STATE_IN_SERVICE;
			ptrace(("%p: Enabling Tx\n", xp));
		}
		if ((xp->sdl.config.ifflags & (SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING))) {
			ptrace(("%p: Marking interface up\n", xp));
			xp->sdl.config.ifflags |= SDL_IF_UP;
		}
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/*
 *  SDL_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
sdl_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;
	sdl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto eproto;
	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	spin_lock_irqsave(&xp->lock, flags);
	{
		if (p->sdl_flags & SDL_RX_DIRECTION) {
			xp->sdl.config.ifflags &= ~SDL_IF_RX_RUNNING;
			xp->sdl.statem.rx_state = SDL_STATE_IDLE;
			ptrace(("%p: Disabling Rx\n", xp));
		}
		if (p->sdl_flags & SDL_TX_DIRECTION) {
			xp->sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
			xp->sdl.statem.tx_state = SDL_STATE_IDLE;
			ptrace(("%p: Disabling Tx\n", xp));
		}
		if (!(xp->sdl.config.ifflags & (SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING))) {
			ptrace(("%p: Marking interface down\n", xp));
			xp->sdl.config.ifflags &= ~SDL_IF_UP;
		}
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
      eproto:
	return m_error(xp, q, EPROTO);
}

/* 
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);

	switch (xp->i_state) {
	case LMI_ENABLED:
	case LMI_DISABLED:
	{
		struct sp *sp;
		struct cd *cd;

		if ((sp = xp->sp) && (cd = sp->cd)) {
			uint16_t ppa =
			    (xp->
			     chan & 0xff) | ((sp->span & 0x0f) << 8) | ((cd->card & 0x0f) << 12);
			return lmi_info_ack(xp, q, (caddr_t) &ppa, sizeof(ppa));
		}
	}
	}
	return lmi_info_ack(xp, q, NULL, 0);
}

/* 
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	psw_t flags = 0;
	int err, card, span, chan, slot;
	struct cd *cd;
	struct sp *sp = NULL;
	uint16_t ppa;
	struct xp *xp = XP_PRIV(q);
	lmi_attach_req_t *p = ((typeof(p)) mp->b_rptr);

	if (mp->b_wptr - mp->b_rptr < sizeof(*p) + sizeof(ppa)) {
		ptrace(("%s: ERROR: primitive too small = %ld bytes\n", DRV_NAME,
			(long) (mp->b_wptr - mp->b_rptr)));
		goto lmi_badprim;
	}
	if (xp->i_state != LMI_UNATTACHED) {
		ptrace(("%s: ERROR: interface out of state\n", DRV_NAME));
		goto lmi_outstate;
	}
	xp->i_state = LMI_ATTACH_PENDING;
	ppa = *(typeof(ppa) *) (p + 1);
	/* check card */
	card = (ppa >> 12) & 0x0f;
	for (cd = x400p_cards; cd && cd->card != card; cd = cd->next) ;
	if (!cd) {
		ptrace(("%s: ERROR: invalid card %d\n", DRV_NAME, card));
		goto lmi_badppa;
	}
	/* check span */
	span = (ppa >> 8) & 0x0f;
	if (span < 0 || span > X400_SPANS - 1) {
		ptrace(("%s: ERROR: invalid span %d\n", DRV_NAME, span));
		goto lmi_badppa;
	}
	if (!(sp = cd->spans[span])) {
		ptrace(("%s: ERROR: unallocated span %d\n", DRV_NAME, span));
		goto lmi_badppa;
	}
	if (sp->config.ifgtype != SDL_GTYPE_E1 && sp->config.ifgtype != SDL_GTYPE_T1
	    && sp->config.ifgtype != SDL_GTYPE_J1) {
		swerr();
		goto efault;
	}
	/* check chan */
	chan = (ppa >> 0) & 0xff;
	if (chan) {
		/* specific channel indicated */
		switch (cd->config.ifgtype) {
		case SDL_GTYPE_E1:
			if (chan < 1 || chan > 31) {
				ptrace(("%s: ERROR: invalid chan %d\n", DRV_NAME, chan));
				goto lmi_badppa;
			}
			slot = xp_e1_chan_map[chan - 1];
			if (sp->slots[slot]) {
				ptrace(("%s: ERROR: slot %d in use\n", DRV_NAME, slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(xp, q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("%s: attaching card %d, span %d, chan %d, slot %d\n",
				DRV_NAME, card, span, chan, slot));
			spin_lock_irqsave(&xp->lock, flags);
			{
				sp->slots[slot] = xp_get(xp);
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_DS0;
				sp->config.ifrate = 64000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 8;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = slot;
				/* SDL configuration defaults */
				xp->sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->sdl.config = sp->config;
				xp->sdl.config.ifflags = 0;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config = sdt_default_e1_chan;
				/* SL configuration defaults */
				xp->sl.config = sl_default_e1_chan;
				/* LMI configuration defaults */
				xp->option = lmi_default_e1_chan;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		case SDL_GTYPE_T1:
			if (chan < 1 || chan > 24) {
				ptrace(("%s: ERROR: invalid chan %d\n", DRV_NAME, chan));
				goto lmi_badppa;
			}
			slot = xp_t1_chan_map[chan - 1];
			if (sp->slots[slot]) {
				ptrace(("%s: ERROR: slot %d in use\n", DRV_NAME, slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(xp, q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("%s: attaching card %d, span %d, chan %d, slot %d\n",
				DRV_NAME, card, span, chan, slot));
			spin_lock_irqsave(&xp->lock, flags);
			{
				sp->slots[slot] = xp_get(xp);
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_DS0;
				sp->config.ifrate = 64000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 8;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = slot;
				/* SDL configuration defaults */
				xp->sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->sdl.config = sp->config;
				xp->sdl.config.ifflags = 0;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config = sdt_default_t1_chan;
				/* SL configuration defaults */
				xp->sl.config = sl_default_t1_chan;
				/* LMI configuration defaults */
				xp->option = lmi_default_t1_chan;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		case SDL_GTYPE_J1:
			if (chan < 1 || chan > 24) {
				ptrace(("%s: ERROR: invalid chan %d\n", DRV_NAME, chan));
				goto lmi_badppa;
			}
			slot = xp_t1_chan_map[chan - 1];
			if (sp->slots[slot]) {
				ptrace(("%s: ERROR: slot %d in use\n", DRV_NAME, slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(xp, q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("%s: attaching card %d, span %d, chan %d, slot %d\n",
				DRV_NAME, card, span, chan, slot));
			spin_lock_irqsave(&xp->lock, flags);
			{
				sp->slots[slot] = xp_get(xp);
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_DS0A;
				sp->config.ifrate = 64000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 8;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = slot;
				/* SDL configuration defaults */
				xp->sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->sdl.config = sp->config;
				xp->sdl.config.ifflags = 0;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config = sdt_default_j1_chan;
				/* SL configuration defaults */
				xp->sl.config = sl_default_j1_chan;
				/* LMI configuration defaults */
				xp->option = lmi_default_j1_chan;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		}
	} else {
		int c;

		/* entire span indicated */
		switch (cd->config.ifgtype) {
		case SDL_GTYPE_E1:
			for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]); c++)
				if (sp->slots[xp_e1_chan_map[c]]) {
					ptrace(("%s: ERROR: slot in use for chan %d\n",
						DRV_NAME, c));
					goto lmi_badppa;
				}
			if ((err = lmi_ok_ack(xp, q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("%s: attaching card %d, entire span %d\n", DRV_NAME, card, span));
			spin_lock_irqsave(&xp->lock, flags);
			{
				for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]);
				     c++) {
					slot = xp_e1_chan_map[c];
					sp->slots[slot] = xp_get(xp);
				}
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_E1;
				sp->config.ifrate = 2048000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 64;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = 0;
				/* SDL configuration defaults */
				xp->sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->sdl.config = sp->config;
				xp->sdl.config.ifflags = 0;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config = sdt_default_e1_span;
				/* SL configuration defaults */
				xp->sl.config = sl_default_e1_span;
				/* LMI configuration defaults */
				xp->option = lmi_default_e1_span;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		case SDL_GTYPE_T1:
			for (c = 0; c < (sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0])); c++)
				if (sp->slots[xp_t1_chan_map[c]]) {
					ptrace(("%s: ERROR: slot in use for chan %d\n",
						DRV_NAME, c));
					goto lmi_badppa;
				}
			if ((err = lmi_ok_ack(xp, q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			spin_lock_irqsave(&xp->lock, flags);
			{
				printd(("%s: attaching card %d, entire span %d\n",
					DRV_NAME, card, span));
				for (c = 0;
				     c < (sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0]));
				     c++) {
					slot = xp_t1_chan_map[c];
					sp->slots[slot] = xp_get(xp);
				}
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_T1;
				sp->config.ifrate = 1544000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 64;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = 0;
				/* SDL configuration defaults */
				xp->sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->sdl.config = sp->config;
				xp->sdl.config.ifflags = 0;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config = sdt_default_t1_span;
				/* SL configuration defaults */
				xp->sl.config = sl_default_t1_span;
				/* LMI configuration defaults */
				xp->option = lmi_default_t1_span;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
			return (QR_DONE);
		case SDL_GTYPE_J1:
			for (c = 0; c < (sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0])); c++)
				if (sp->slots[xp_t1_chan_map[c]]) {
					ptrace(("%s: ERROR: slot in use for chan %d\n",
						DRV_NAME, c));
					goto lmi_badppa;
				}
			if ((err = lmi_ok_ack(xp, q, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			spin_lock_irqsave(&xp->lock, flags);
			{
				printd(("%s: attaching card %d, entire span %d\n",
					DRV_NAME, card, span));
				for (c = 0;
				     c < (sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0]));
				     c++) {
					slot = xp_t1_chan_map[c];
					sp->slots[slot] = xp_get(xp);
				}
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_J1;
				sp->config.ifrate = 1544000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 64;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				xp->slot = 0;
				/* SDL configuration defaults */
				xp->sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->sdl.config = sp->config;
				xp->sdl.config.ifflags = 0;
				/* SDT configuration defaults */
				xp->sdt.statem.daedr_state = SDT_STATE_IDLE;
				xp->sdt.statem.daedt_state = SDT_STATE_IDLE;
				xp->sdt.config = sdt_default_j1_span;
				/* SL configuration defaults */
				xp->sl.config = sl_default_j1_span;
				/* LMI configuration defaults */
				xp->option = lmi_default_j1_span;
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
		return lmi_error_ack(xp, q, LMI_UNATTACHED, LMI_ATTACH_REQ, errno, reason);
	}
}

/* 
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	struct sp *sp;
	struct cd *cd;
	int err, slot;
	psw_t flags = 0;

	/* validate detach */
	if (xp->i_state != LMI_DISABLED)
		return lmi_error_ack(xp, q, xp->i_state, LMI_DETACH_REQ, 0, LMI_OUTSTATE);
	if (!(sp = xp->sp) || !(cd = sp->cd)) {
		swerr();
		return m_error(xp, q, EFAULT);
	}
	xp->i_state = LMI_DETACH_PENDING;
	if ((err = lmi_ok_ack(xp, q, LMI_UNATTACHED, LMI_DETACH_REQ)))
		return (err);
	/* commit detach */
	spin_lock_irqsave(&xp->lock, flags);
	{
		for (slot = 0; slot < 32; slot++)
			if (sp->slots[slot] == xp)
				xp_put(xchg(&sp->slots[slot], NULL));
		sp_put(xchg(&xp->sp, NULL));
		xp->chan = 0;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (QR_DONE);
}

/* 
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	int err;
	struct xp *xp = XP_PRIV(q);
	struct cd *cd;
	struct sp *sp;

	/* validate enable */
	if (xp->i_state != LMI_DISABLED) {
		ptrace(("%s: ERROR: out of state: state = %ld\n", DRV_NAME, xp->i_state));
		goto lmi_outstate;
	}
	if (!(sp = xp->sp)) {
		ptrace(("%s: ERROR: out of state: no span pointer\n", DRV_NAME));
		goto lmi_outstate;
	}
	if (!(cd = sp->cd)) {
		ptrace(("%s: ERROR: out of state: no card pointer\n", DRV_NAME));
		goto lmi_outstate;
	}
#ifdef _DEBUG
	if (cd->config.ifgtype != SDL_GTYPE_E1 && cd->config.ifgtype != SDL_GTYPE_T1
	    && cd->config.ifgtype != SDL_GTYPE_J1) {
		ptrace(("%s: ERROR: card group type = %u\n", DRV_NAME, cd->config.ifgtype));
		return m_error(xp, q, EFAULT);
	}
#endif
	if (xp->sdl.config.ifflags & SDL_IF_UP) {
		ptrace(("%s: ERROR: out of state: device already up\n", DRV_NAME));
		goto lmi_outstate;
	}
	if ((err = lmi_enable_con(xp, q)))
		return (err);
	/* commit enable */
	printd(("%s: %p: performing enable\n", DRV_NAME, xp));
	xp->i_state = LMI_ENABLE_PENDING;
	xp->sdl.config.ifname = sp->config.ifname;
	xp->sdl.config.ifflags |= SDL_IF_UP;
	xp->sdl.config.iftype = xp->sdl.config.iftype;
	switch (xp->sdl.config.iftype) {
	case SDL_TYPE_DS0A:
		xp->sdl.config.ifrate = 56000;
		xp->sdl.config.ifblksize = 8;
		break;
	case SDL_TYPE_DS0:
		xp->sdl.config.ifrate = 64000;
		xp->sdl.config.ifblksize = 8;
		break;
	case SDL_TYPE_E1:
		xp->sdl.config.ifrate = 2048000;
		xp->sdl.config.ifblksize = 64;
		break;
	case SDL_TYPE_T1:
		xp->sdl.config.ifrate = 1544000;
		xp->sdl.config.ifblksize = 64;
		break;
	case SDL_TYPE_J1:
		xp->sdl.config.ifrate = 1544000;
		xp->sdl.config.ifblksize = 64;
		break;
	}
	xp->sdl.config.ifgtype = sp->config.ifgtype;
	xp->sdl.config.ifgrate = sp->config.ifgrate;
	xp->sdl.config.ifmode = sp->config.ifmode;
	xp->sdl.config.ifgmode = sp->config.ifgmode;
	xp->sdl.config.ifgcrc = sp->config.ifgcrc;
	xp->sdl.config.ifclock = sp->config.ifclock;
	xp->sdl.config.ifcoding = sp->config.ifcoding;
	xp->sdl.config.ifframing = sp->config.ifframing;
	xp->sdl.config.ifblksize = xp->sdl.config.ifblksize;
	xp->sdl.config.ifleads = sp->config.ifleads;
	xp->sdl.config.ifbpv = sp->config.ifbpv;
	xp->sdl.config.ifalarms = sp->config.ifalarms;
	xp->sdl.config.ifrxlevel = sp->config.ifrxlevel;
	xp->sdl.config.iftxlevel = sp->config.iftxlevel;
	xp->sdl.config.ifsync = cd->config.ifsync;
	xp->i_state = LMI_ENABLED;
	if (!(sp->config.ifflags & SDL_IF_UP)) {
		/* need to bring up span */
		psw_t flags;

		switch (cd->config.ifgtype) {
		case SDL_GTYPE_E1:
		{
			printd(("%s: performing enable on E1 span %d\n", DRV_NAME, sp->span));
			spin_lock_irqsave(&cd->lock, flags);
			// cd->xlb[SYNREG] = SYNCSELF; /* NO, NO, NO */
			/* Tell ISR to re-evaluate the sync source */
			cd->eval_syncsrc = 1;
			cd->xlb[CTLREG] = (E1DIV);
			cd->xlb[LEDREG] = 0xff;

			xp_span_reconfig(cd, sp->span);
			sp->config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);

			/* enable interrupts */
			cd->xlb[CTLREG] = (INTENA | E1DIV);
			spin_unlock_irqrestore(&cd->lock, flags);
			break;
		}
		case SDL_GTYPE_T1:
		case SDL_GTYPE_J1:
		{
			printd(("%s: performing enable on %s span %d\n", DRV_NAME,
				(cd->config.ifgtype == SDL_GTYPE_J1) ? "J1" : "T1", sp->span));
			spin_lock_irqsave(&cd->lock, flags);
			// cd->xlb[SYNREG] = SYNCSELF; /* NO, NO, NO */
			/* Tell ISR to re-evaluate the sync source */
			cd->eval_syncsrc = 1;
			cd->xlb[CTLREG] = 0;
			cd->xlb[LEDREG] = 0xff;

			xp_span_reconfig(cd, sp->span);
			sp->config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);

			/* enable interrupts */
			cd->xlb[CTLREG] = (INTENA);
			spin_unlock_irqrestore(&cd->lock, flags);
			break;
		}
		default:
			swerr();
			break;
		}
	}
	return (QR_DONE);
      lmi_outstate:
	return lmi_error_ack(xp, q, xp->i_state, LMI_ENABLE_REQ, 0, LMI_OUTSTATE);
}

/* 
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct sp *sp;
	struct cd *cd;

	/* validate disable */
	if (xp->i_state != LMI_ENABLED)
		goto lmi_outstate;
	xp->i_state = LMI_DISABLE_PENDING;
	/* perform disable */
	if ((err = lmi_disable_con(xp, q)))
		return (err);
	/* commit disable */
	if ((sp = xp->sp) && (cd = sp->cd)) {
		psw_t flags = 0;

		spin_lock_irqsave(&cd->lock, flags);
		{
			int slot, boff;
			uchar idle, *base = (uchar *) cd->wbuf + span_to_byte(sp->span);

			switch (cd->config.ifgtype) {
			case SDL_GTYPE_T1:
			case SDL_GTYPE_J1:
				idle = 0xfe;
				break;
			default:
			case SDL_GTYPE_E1:
				idle = 0xff;
				break;
			}

			for (slot = 0; slot < 32; slot++)
				if (sp->slots[slot] == xp)
					for (boff = 0; boff < X400P_EBUFNO; boff++)
						*(base + (boff << 10) + (slot << 2)) = idle;
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
			xp_timer_stop(xp, t8);
			/* stop suerm */
			xp->sdt.statem.suerm_state = SDT_STATE_IDLE;
			/* reset transmitter and receiver state */
			if (xp->tx.msg && xp->tx.msg != xp->tx.cmp)
				freemsg(xchg(&xp->tx.msg, NULL));
			if (xp->tx.cmp)
				freemsg(xchg(&xp->tx.cmp, NULL));
			if (xp->rx.msg)
				freemsg(xchg(&xp->rx.msg, NULL));
			if (xp->rx.nxt)
				freemsg(xchg(&xp->rx.nxt, NULL));
			if (xp->rx.cmp)
				freemsg(xchg(&xp->rx.cmp, NULL));
			bzero(&xp->tx, sizeof(xp->tx));
			bzero(&xp->rx, sizeof(xp->tx));
		}
		spin_unlock_irqrestore(&cd->lock, flags);
	} else
		swerr();
	return (QR_DONE);
      lmi_outstate:
	return lmi_error_ack(xp, q, xp->i_state, LMI_DISABLE_REQ, 0, LMI_OUTSTATE);
}

/* 
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC noinline fastcall __unlikely int
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
 *  Test and Commit SL configuration settings
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC noinline __unlikely int
sl_test_config(struct xp *xp, sl_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC noinline __unlikely int
sl_commit_config(struct xp *xp, sl_config_t * arg)
{
	return (-EOPNOTSUPP);
}
#endif

STATIC noinline __unlikely int
sl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->option;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->option = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sl.config;
		}
		spin_unlock_irqrestore(&xp->lock, flags);

		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sl.config = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sl.statem;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sl.statem = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sl.statsp;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sl.statsp = *arg;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sl.stats;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_ioccstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			bzero(&xp->sl.stats, sizeof(xp->sl.stats));
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			*arg = xp->sl.notify;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sl.notify.events |= arg->events;
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct xp *xp = XP_PRIV(q);
		int ret = 0;
		psw_t flags = 0;

		spin_lock_irqsave(&xp->lock, flags);
		{
			xp->sl.notify.events &= ~(arg->events);
		}
		spin_unlock_irqrestore(&xp->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Test and Commit SDT configuration settings
 *
 *  -------------------------------------------------------------------------
 */
STATIC noinline __unlikely int
sdt_test_config(struct xp *xp, sdt_config_t * arg)
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
STATIC noinline __unlikely int
sdt_commit_config(struct xp *xp, sdt_config_t * arg)
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

STATIC noinline __unlikely int
sdt_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		return sdt_test_config(xp, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sdt_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		return sdt_commit_config(xp, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC noinline __unlikely int
sdt_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_ioccmreset(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);

	(void) xp;
	(void) mp;
	fixme(("Master reset\n"));
	return (-EOPNOTSUPP);
}
STATIC noinline __unlikely int
sdt_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_ioccstats(queue_t *q, mblk_t *mp)
{
	psw_t flags = 0;
	struct xp *xp = XP_PRIV(q);

	(void) mp;
	spin_lock_irqsave(&xp->lock, flags);
	{
		bzero(&xp->sdt.stats, sizeof(xp->sdt.stats));
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (0);
}
STATIC noinline __unlikely int
sdt_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdt_ioccabort(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags = 0;

	(void) mp;
	spin_lock_irqsave(&xp->lock, flags);
	{
		xp->tx.residue |= 0x7f << xp->tx.rbits;
		xp->tx.rbits += 7;
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Test SDL configuration settings
 *
 *  -------------------------------------------------------------------------
 */
STATIC noinline __unlikely int
sdl_test_config(struct xp *xp, sdl_config_t * arg)
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
			if (xp->sp->config.ifgtype != SDL_GTYPE_E1) {
				trace();
				ret = (-EINVAL);
				break;
			}
			break;
		case SDL_TYPE_T1:	/* full T1 span */
			if (xp->sp->config.ifgtype != SDL_GTYPE_T1) {
				trace();
				ret = (-EINVAL);
				break;
			}
			break;
		case SDL_TYPE_J1:	/* full J1 span */
			if (xp->sp->config.ifgtype != SDL_GTYPE_T1 &&
			    xp->sp->config.ifgtype != SDL_GTYPE_J1) {
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
			ret = (-EINVAL);
			break;
		}
		if (ret)
			break;
		if (xp->sp) {
			switch (arg->ifgtype) {
			case SDL_GTYPE_NONE:	/* */
				arg->ifgtype = xp->sp->config.ifgtype;
				break;
			case SDL_GTYPE_E1:	/* */
			case SDL_GTYPE_T1:	/* */
			case SDL_GTYPE_J1:	/* */
				if (arg->ifgtype != xp->sp->config.ifgtype) {
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
				case SDL_GTYPE_J1:
					arg->ifgcrc = SDL_GCRC_CRC6J;
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
			case SDL_GCRC_CRC6J:
				if (arg->ifgtype != SDL_GTYPE_J1) {
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
				arg->ifclock = xp->sp->config.ifclock;
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
				arg->ifcoding = xp->sp->config.ifcoding;
				break;
			case SDL_CODING_AMI:	/* */
				break;
			case SDL_CODING_B8ZS:	/* */
				if (arg->ifgtype != SDL_GTYPE_T1 && arg->ifgtype != SDL_GTYPE_J1) {
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
				arg->ifframing = xp->sp->config.ifframing;
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
				if (arg->ifgtype != SDL_GTYPE_T1 && arg->ifgtype != SDL_GTYPE_J1) {
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
			if (arg->iftxlevel == 0)
				arg->iftxlevel = xp->sp->config.iftxlevel;
			else if (arg->iftxlevel <= 16)
				arg->iftxlevel--;
			else {
				trace();
				ret = (-EINVAL);
			}
			if (ret)
				break;
			if (xp->sp->cd) {
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
				trace();
				ret = (-EFAULT);
				break;
			}
		} else {
			trace();
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
STATIC noinline __unlikely void
sdl_commit_config(struct xp *xp, sdl_config_t * arg)
{
	int chan_reconfig = 0, span_reconfig = 0, card_reconfig = 0;
	struct sp *sp = NULL;
	struct cd *cd = NULL;
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
		case SDL_TYPE_J1:
			xp->sdl.config.ifrate = 1544000;
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

			if (sp->config.ifgcrc != arg->ifgcrc) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifgcrc = arg->ifgcrc;
				sp->config.ifgcrc = arg->ifgcrc;
				span_reconfig = 1;
			}
			if (sp->config.ifgmode != arg->ifgmode) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifgmode = arg->ifgmode;
				sp->config.ifgmode = arg->ifgmode;
				span_reconfig = 1;
			}
			if (sp->config.ifclock != arg->ifclock) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifclock = arg->ifclock;
				sp->config.ifclock = arg->ifclock;
				span_reconfig = 1;
			}
			if (sp->config.ifcoding != arg->ifcoding) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifcoding =
						    arg->ifcoding;
				sp->config.ifcoding = arg->ifcoding;
				span_reconfig = 1;
			}
			if (sp->config.ifframing != arg->ifframing) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.ifframing =
						    arg->ifframing;
				sp->config.ifframing = arg->ifframing;
				span_reconfig = 1;
			}
			if (sp->config.iftxlevel != arg->iftxlevel) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot])
						sp->slots[slot]->sdl.config.iftxlevel =
						    arg->iftxlevel;
				sp->config.iftxlevel = arg->iftxlevel;
				span_reconfig = 1;
			}
			if ((cd = sp->cd)) {
				int src, span, slot;

				for (src = 0; src < SDL_SYNCS; src++) {
					if (cd->config.ifsyncsrc[src] != arg->ifsyncsrc[src]) {
						for (span = 0; span < X400_SPANS; span++)
							if (cd->spans[span])
								for (slot = 0; slot < 32; slot++)
									if (cd->spans[span]->
									    slots[slot])
										cd->spans[span]->
										    slots[slot]->
										    sdl.config.
										    ifsyncsrc[src] =
										    arg->
										    ifsyncsrc[src];
						cd->config.ifsyncsrc[src] = arg->ifsyncsrc[src];
						card_reconfig = 1;
					}
				}
			}
		}
		if (xp && chan_reconfig && xp->sdl.config.ifflags & SDL_IF_UP) {
			if (sp && cd) {
				if (xp->chan) {
					switch (cd->config.ifgtype) {
					case SDL_GTYPE_E1:
						xp->slot = xp_e1_chan_map[xp->chan - 1];
						break;
					case SDL_GTYPE_T1:
					case SDL_GTYPE_J1:
						xp->slot = xp_t1_chan_map[xp->chan - 1];
						break;
					}
				} else {
					switch (cd->config.ifgtype) {
					case SDL_GTYPE_E1:
						xp->slot = 0;
						break;
					case SDL_GTYPE_T1:
					case SDL_GTYPE_J1:
						xp->slot = 0;
						break;
					}
				}
			}
		}
		if (sp && span_reconfig && (sp->config.ifflags & SDL_IF_UP) && cd) {
			/* need to bring up span */
			switch (cd->config.ifgtype) {
			case SDL_GTYPE_E1:
			{
				printd(("%s: performing reconfiguration of E1 span %d\n",
					DRV_NAME, sp->span));
				/* Tell ISR to re-evaluate the sync source */
				cd->eval_syncsrc = 1;
				xp_span_reconfig(cd, sp->span);
				break;
			}
			case SDL_GTYPE_T1:
			{
				printd(("%s: performing reconfiguration of T1 span %d\n",
					DRV_NAME, sp->span));
				/* Tell ISR to re-evaluate the sync source */
				cd->eval_syncsrc = 1;
				xp_span_reconfig(cd, sp->span);
				break;
			}
			case SDL_GTYPE_J1:
			{
				printd(("%s: performing reconfiguration of J1 span %d\n",
					DRV_NAME, sp->span));
				/* Tell ISR to re-evaluate the sync source */
				cd->eval_syncsrc = 1;
				xp_span_reconfig(cd, sp->span);
				break;
			}
			default:
				swerr();
				break;
			}
		}
		if (cd && card_reconfig && cd->config.ifflags & SDL_IF_UP) {
			cd->eval_syncsrc = 1;
		}
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	return;
}

/* 
 *  SDL_IOCGOPTIONS:    lmi_option_t
 *  -----------------------------------
 */
STATIC noinline __unlikely int
sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		struct sp *sp;
		psw_t flags = 0;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		bzero(arg, sizeof(*arg));
		spin_lock_irqsave(&xp->lock, flags);
		{
			arg->ifflags = xp->sdl.config.ifflags;
			arg->iftype = xp->sdl.config.iftype;
			arg->ifrate = xp->sdl.config.ifrate;
			arg->ifmode = xp->sdl.config.ifmode;
			arg->ifblksize = xp->sdl.config.ifblksize;
			if ((sp = xp->sp)) {
				struct cd *cd;

				arg->ifgtype = sp->config.ifgtype;
				arg->ifgrate = sp->config.ifgrate;
				arg->ifgmode = sp->config.ifgmode;
				arg->ifgcrc = sp->config.ifgcrc;
				arg->ifclock = sp->config.ifclock;
				arg->ifcoding = sp->config.ifcoding;
				arg->ifframing = sp->config.ifframing;
				arg->ifalarms = sp->config.ifalarms;
				arg->ifrxlevel = sp->config.ifrxlevel;
				arg->iftxlevel = sp->config.iftxlevel;
				if ((cd = sp->cd)) {
					int src;

					for (src = 0; src < SDL_SYNCS; src++)
						arg->ifsyncsrc[src] = cd->config.ifsyncsrc[src];
					arg->ifsync = cd->config.ifsync;
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
STATIC noinline __unlikely int
sdl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret;
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_ioccmreset(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_ioccdisctx(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
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
STATIC noinline __unlikely int
sdl_ioccconntx(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
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
STATIC noinline fastcall __hot void
xp_e1c_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	int slot;

	/* one slot at a time, 8 frames */
	for (slot = 1; slot < 32; slot++) {
		struct xp *xp;
		size_t coff = slot << 2;

		if ((xp = sp->slots[slot]) && (xp->sdl.config.ifflags & SDL_IF_UP)) {
			sdt_stats_t *stats = &xp->sdt.stats;

			if (xp->sdl.config.iftype != SDL_TYPE_DS0A) {
				if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
					xp_ds0_tx_block(xp, wspan + coff, wend, stats);
					xp->sdl.stats.tx_octets += 8;
				} else {
					xp_ds0_tx_idle(xp, wspan + coff, wend, stats);
				}
				if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
					xp_ds0_rx_block(xp, rspan + coff, rend, stats);
					xp->sdl.stats.rx_octets += 8;
				}
			}
		} else {
			xp_ds0_tx_idle(xp, wspan + coff, wend, NULL);
		}
	}
}

/*
 *  E1 Process
 *  -----------------------------------
 *  Process an entire E1 span.  This is a High-Speed Link.  All channels are
 *  concatenated to form a single link.
 */
STATIC noinline fastcall __hot void
xp_e1_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	struct xp *xp;

	/* entire span, one frame at a time */
	if ((xp = sp->slots[1])) {
		sdt_stats_t *stats = &xp->sdt.stats;

		if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
			xp_e1_tx_block(xp, wspan, wend, stats);
			xp->sdl.stats.tx_octets += 8 * 31;
		} else {
			xp_e1_tx_idle(xp, wspan, wend, stats);
		}
		if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
			xp_e1_rx_block(xp, rspan, rend, stats);
			xp->sdl.stats.rx_octets += 8 * 31;
		}
	} else {
		xp_e1_tx_idle(xp, wspan, wend, NULL);
	}
}

/*
 *  E1 Card Tasklet
 *  -----------------------------------
 *  Process an entire E1 card.
 */
STATIC __hot void
xp_e1_card_tasklet(unsigned long data)
{
	struct cd *cd = (struct cd *) data;

	spin_lock(&cd->lock);
	{
		if (cd->uebno != cd->lebno) {
			size_t boff = cd->uebno << 10;
			uchar *wbeg = (uchar *) cd->wbuf + boff;
			uchar *wend = wbeg + 1024;
			uchar *rbeg = (uchar *) cd->rbuf + boff;
			uchar *rend = rbeg + 1024;
			int span;

			for (span = 0; span < X400_SPANS; span++) {
				struct sp *sp;

				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					int soff = span_to_byte(span);

					if (sp->config.iftype == SDL_TYPE_E1)
						xp_e1_process(sp, wbeg + soff, rbeg + soff, wend,
							      rend);
					else
						xp_e1c_process(sp, wbeg + soff, rbeg + soff, wend,
							       rend);
				}
			}
			if ((cd->uebno = (cd->uebno + 1) & (X400P_EBUFNO - 1)) != cd->lebno)
				tasklet_hi_schedule(&cd->tasklet);
		}
	}
	spin_unlock(&cd->lock);
}

/*
 *  T1C Process
 *  -----------------------------------
 *  Process a channelized T1 span, one channel at a time.  Each channel can be
 *  either a clear channel or a DS0A channel.
 */
STATIC noinline fastcall __hot void
xp_t1c_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	int slot;

	/* one slot at a time, 8 frames */
	for (slot = 1; slot < 32; slot++) {
		if (slot & 0x3) {
			struct xp *xp;
			size_t coff = slot << 2;

			if ((xp = sp->slots[slot]) && (xp->sdl.config.ifflags & SDL_IF_UP)) {
				sdt_stats_t *stats = &xp->sdt.stats;

				if (xp->sdl.config.iftype != SDL_TYPE_DS0A) {
					if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp_ds0_tx_block(xp, wspan + coff, wend, stats);
						xp->sdl.stats.tx_octets += 8;
					} else {
						xp_ds0a_tx_idle(xp, wspan + coff, wend, stats);
					}
					if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp_ds0_rx_block(xp, rspan + coff, rend, stats);
						xp->sdl.stats.rx_octets += 8;
					}
				} else {
					if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp_ds0a_tx_block(xp, wspan + coff, wend, stats);
						xp->sdl.stats.tx_octets += 8;
					} else {
						xp_ds0a_tx_idle(xp, wspan + coff, wend, stats);
					}
					if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp_ds0a_rx_block(xp, rspan + coff, rend, stats);
						xp->sdl.stats.rx_octets += 8;
					}
				}
			} else {
				xp_ds0a_tx_idle(xp, wspan + coff, wend, NULL);
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
STATIC noinline fastcall __hot void
xp_t1_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	struct xp *xp;

	/* entire span, one frame at a time */
	if ((xp = sp->slots[1])) {
		sdt_stats_t *stats = &xp->sdt.stats;

		if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
			xp_t1_tx_block(xp, wspan, wend, stats);
			xp->sdl.stats.tx_octets += 8 * 24;
		} else {
			xp_t1_tx_idle(xp, wspan, wend, stats);
		}
		if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
			xp_t1_rx_block(xp, rspan, rend, stats);
			xp->sdl.stats.rx_octets += 8 * 24;
		}
	} else {
		xp_t1_tx_idle(xp, wspan, wend, NULL);
	}
}

/*
 *  T1 Card Tasklet
 *  -----------------------------------
 *  Process an entire T1 card.
 */
STATIC __hot void
xp_t1_card_tasklet(unsigned long data)
{
	struct cd *cd = (struct cd *) data;

	spin_lock(&cd->lock);
	{
		if (cd->uebno != cd->lebno) {
			size_t boff = cd->uebno << 10;
			uchar *wbeg = (uchar *) cd->wbuf + boff;
			uchar *wend = wbeg + 1024;
			uchar *rbeg = (uchar *) cd->rbuf + boff;
			uchar *rend = rbeg + 1024;
			int span;

			for (span = 0; span < X400_SPANS; span++) {
				struct sp *sp;

				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					int soff = span_to_byte(span);

					if (sp->config.iftype == SDL_TYPE_T1)
						xp_t1_process(sp, wbeg + soff, rbeg + soff, wend,
							      rend);
					else
						xp_t1c_process(sp, wbeg + soff, rbeg + soff, wend,
							       rend);
				}
			}
			if ((cd->uebno = (cd->uebno + 1) & (X400P_EBUFNO - 1)) != cd->lebno)
				tasklet_hi_schedule(&cd->tasklet);
		}
	}
	spin_unlock(&cd->lock);
}

/*
 *  J1C Process
 *  -----------------------------------
 *  Process a channelized J1 span, one channel at a time.  Each channel can be
 *  either a clear channel or a DS0A channel.
 */
STATIC noinline fastcall __hot void
xp_j1c_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	int slot;

	/* one slot at a time, 8 frames */
	for (slot = 1; slot < 32; slot++) {
		if (slot & 0x3) {
			struct xp *xp;
			size_t coff = slot << 2;

			if ((xp = sp->slots[slot]) && (xp->sdl.config.ifflags & SDL_IF_UP)) {
				sdt_stats_t *stats = &xp->sdt.stats;

				if (xp->sdl.config.iftype != SDL_TYPE_DS0A) {
					if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp_ds0_tx_block(xp, wspan + coff, wend, stats);
						xp->sdl.stats.tx_octets += 8;
					} else {
						xp_ds0a_tx_idle(xp, wspan + coff, wend, stats);
					}
					if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp_ds0_rx_block(xp, rspan + coff, rend, stats);
						xp->sdl.stats.rx_octets += 8;
					}
				} else {
					if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp_ds0a_tx_block(xp, wspan + coff, wend, stats);
						xp->sdl.stats.tx_octets += 8;
					} else {
						xp_ds0a_tx_idle(xp, wspan + coff, wend, stats);
					}
					if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp_ds0a_rx_block(xp, rspan + coff, rend, stats);
						xp->sdl.stats.rx_octets += 8;
					}
				}
			} else {
				xp_ds0a_tx_idle(xp, wspan + coff, wend, NULL);
			}
		}
	}
}

/*
 *  J1 Process
 *  -----------------------------------
 *  Process an entire J1 span.  This is a High-Speed Link.  All channels are
 *  concatenated to form a single link.  (For J1 they appear to be DS0A
 *  channels, however).
 */
STATIC noinline fastcall __hot void
xp_j1_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	struct xp *xp;

	/* entire span, one frame at a time */
	if ((xp = sp->slots[1])) {
		sdt_stats_t *stats = &xp->sdt.stats;

		if (xp->sdl.config.ifflags & SDL_IF_TX_RUNNING) {
			xp_j1_tx_block(xp, wspan, wend, stats);
			xp->sdl.stats.tx_octets += 8 * 24;
		} else {
			xp_j1_tx_idle(xp, wspan, wend, stats);
		}
		if (xp->sdl.config.ifflags & SDL_IF_RX_RUNNING) {
			xp_j1_rx_block(xp, rspan, rend, stats);
			xp->sdl.stats.rx_octets += 8 * 24;
		}
	} else {
		xp_j1_tx_idle(xp, wspan, wend, NULL);
	}
}

/*
 *  J1 Card Tasklet
 *  -----------------------------------
 *  Process an entire J1 card.
 */
STATIC __hot void
xp_j1_card_tasklet(unsigned long data)
{
	struct cd *cd = (struct cd *) data;

	spin_lock(&cd->lock);
	{
		if (cd->uebno != cd->lebno) {
			size_t boff = cd->uebno << 10;
			uchar *wbeg = (uchar *) cd->wbuf + boff;
			uchar *wend = wbeg + 1024;
			uchar *rbeg = (uchar *) cd->rbuf + boff;
			uchar *rend = rbeg + 1024;
			int span;

			for (span = 0; span < X400_SPANS; span++) {
				struct sp *sp;

				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					int soff = span_to_byte(span);

					if (sp->config.iftype == SDL_TYPE_J1)
						xp_j1_process(sp, wbeg + soff, rbeg + soff, wend,
							      rend);
					else
						xp_j1c_process(sp, wbeg + soff, rbeg + soff, wend,
							       rend);
				}
			}
			if ((cd->uebno = (cd->uebno + 1) & (X400P_EBUFNO - 1)) != cd->lebno)
				tasklet_hi_schedule(&cd->tasklet);
		}
	}
	spin_unlock(&cd->lock);
}

/*
 *  X400P Overflow
 *  -----------------------------------
 *  I know that this is rather like kicking them when they are down, we are
 *  doing stats in the ISR when takslets don't have enough time to run, but we
 *  are already in dire trouble if this is happening anyway.  It should not
 *  take too much time to peg these counts.
 */
STATIC noinline fastcall void
xp_overflow(struct cd *cd)
{
	int span;

	_printd(("%s: card %d elastic buffer overrun!\n", __FUNCTION__, cd->card));
	for (span = 0; span < X400_SPANS; span++) {
		struct xp *xp;
		struct sp *sp;

		if ((sp = cd->spans[span]) && sp->config.ifflags & SDL_IF_UP) {
			switch (sp->config.iftype) {
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
			case SDL_TYPE_J1:
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

static noinline fastcall __hot void
xp_e1_txrx_burst(struct cd *cd)
{
	int lebno;

	if ((lebno = (cd->lebno + 1) & (X400P_EBUFNO - 1)) != cd->uebno) {
		register int slot;
		register volatile uint32_t *xll;

		/* PCI reads and writes are soooo slooowww that we want to get the prefetch engine
		   working in parallel.  We only lead by one word to avoid a worse condition: cache
		   ping-pong.  These prefetches are for read. */
		{
			register const uint32_t *wbuf = cd->wbuf + (lebno << 8);

			for (xll = cd->xll, wbuf = cd->wbuf + (lebno << 8);
			     wbuf < cd->wbuf + (lebno << 8) + 256;) {
				for (wbuf++, xll++, slot = 1; slot < 32; slot++, xll++, wbuf++) {
					prefetch(wbuf + 1);
					*xll = *wbuf;
				}
			}
		}
		/* PCI reads and writes are soooo slooowww that we want to get the prefetch engine
		   working in parallel.  We only lead by one word to avoid a worse condition: cache
		   ping-pong.  These prefetches are for read. */
		{
			register uint32_t *rbuf = cd->rbuf + (lebno << 8);

			for (xll = cd->xll, rbuf = cd->rbuf + (lebno << 8);
			     rbuf < cd->rbuf + (lebno << 8) + 256;)
				for (rbuf++, xll++, slot = 1; slot < 32; slot++, xll++, rbuf++) {
					prefetchw(rbuf + 1);
					*rbuf = *xll;
				}
		}
		cd->lebno = lebno;
		tasklet_hi_schedule(&cd->tasklet);
	} else
		xp_overflow(cd);
}

/*
 *  E400P-SS7 Interrupt Service Routine
 *  -----------------------------------
 *  The user will always preceed a read o f any fo the SR1, SR2, and RIR registers with a write.
 *  The user will write a byte to one of these registers, with a one in the bit positions he or she
 *  wishes to read and a zero in the bit positions he or she does not wish to obtain the latest
 *  information on.  WHen a one is written to a bit location, the read register will be updated with
 *  the latest information.  When a zero is written to a bit position, the read register will not be
 *  updated and the previous value will be held.  A write to the status and information registers
 *  will be immediated followed by a read of the same register.  The read result should be logically
 *  ANDed with the mask byte that was just written and this value should be written back to the same
 *  register to insure that the bit does indeed clear.  This second write step is necessary because
 *  the alarms and events in the statue registers occur asyncrhonously in respect to their access
 *  via the parallel port.  This write-read-write scheme allows an external controller or
 *  microprocessor to individually poll certain bits without disturbing the other bits in the
 *  register.  This operation is key in controlling the DS21354/DS21554 wtih higher-order software
 *  languages.
 */
STATIC __hot irqreturn_t
xp_e400_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct cd *cd = (struct cd *) dev_id;

	/* active interrupt (otherwise spurious or shared) */
	if (cd->xlb[STAREG] & INTACTIVE) {
		struct sp *sp;
		int span;

		cd->xlb[CTLREG] = (INTENA | OUTBIT | INTACK | E1DIV);
		xp_e1_txrx_burst(cd);
		for (span = 0; span < X400_SPANS; span++) {
			if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
				volatile uint8_t *xlb = &cd->xlb[span << 8];

				if (sp->recovertime && !--sp->recovertime) {
					printd(("%s: alarm recovery complete\n", __FUNCTION__));
					sp->config.ifalarms &= ~SDL_ALARM_REC;
					xlb[0x21] = 0x5f;	/* turn off yellow (TNAF) */
					cd->eval_syncsrc = 1;
				}
			}
		}
		/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
		if ((span = ((cd->frame >> 3) & 0x3f) - 50) >= 0 && span < X400_SPANS
		    && (sp = cd->spans[span])
		    && (sp->config.ifflags & SDL_IF_UP)) {
			int status, alarms = 0, leds = 0, all_leds;
			volatile uint8_t *xlb = &cd->xlb[span << 8];

			/* write-read-write cycle */
			xlb[0x06] = 0x0f;
			status = xlb[0x06];
			xlb[0x06] = status & 0x0f;

			if (status & 0x09)	/* RUA1 or RLOS */
				alarms |= SDL_ALARM_RED;
			if (status & 0x02)	/* RCL */
				alarms |= SDL_ALARM_BLU;
			if (alarms) {
				if (!(sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just begun */
					xlb[0x21] = 0x7f;	/* set yellow alarm (TNAF) */
					cd->eval_syncsrc = 1;
				}
			} else {
				if ((sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just ended */
					alarms |= SDL_ALARM_REC;
					sp->recovertime = X400P_SDL_ALARM_SETTLE_TIME;
				}
			}
			if (status & 0x04)	/* RRA */
				alarms |= SDL_ALARM_YEL;
			sp->config.ifalarms = alarms;
			/* adjust leds */
			if (alarms & SDL_ALARM_RED)
				leds |= LEDRED;
			else if (alarms & SDL_ALARM_YEL)
				leds |= LEDYEL;
			else
				leds |= LEDGRN;
			all_leds = cd->leds;
			all_leds &= ~(LEDYEL << (span << 1));
			all_leds |= leds << (span << 1);
			if (cd->leds != all_leds) {
				cd->xlb[LEDREG] = all_leds;
				cd->leds = all_leds;
			}
		}
		// if (!(cd->frame % 8000))
		if (!(cd->frame & 0x1fff)) {	/* 1.024 seconds */
			for (span = 0; span < X400_SPANS; span++)
				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					volatile uint8_t *xlb = &cd->xlb[span << 8];

					// printd(("%s: accumulating bipolar violations\n",
					// __FUNCTION__));
					sp->config.ifbpv += ((ushort) xlb[0x00] << 8) + xlb[0x01];
				}
		}
		if (xchg((int *) &cd->eval_syncsrc, 0)) {
			int src, syncsrc = 0;

			for (src = 0; src < SDL_SYNCS; src++) {
				if ((span = cd->config.ifsyncsrc[src]) && (--span < X400_SPANS)
				    && cd->spans[span]
				    && (cd->spans[span]->config.ifflags & SDL_IF_UP)
				    && !(cd->spans[span]->config.ifclock == SDL_CLOCK_LOOP)
				    && !(cd->spans[span]->config.
					 ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					syncsrc = cd->config.ifsyncsrc[src];
					break;
				}
			}
			if (cd->config.ifsync != syncsrc) {
				cd->xlb[SYNREG] = syncsrc;
				cd->config.ifsync = syncsrc;
			}
		}
		cd->frame += 8;
		cd->xlb[CTLREG] = (INTENA | E1DIV);
		return (irqreturn_t) (IRQ_HANDLED);
	}
	return (irqreturn_t) (IRQ_NONE);
}

/*
 *  V401P-E Interrupt Service Routine
 *  ---------------------------------
 *  The user always precedes a read of any of the status registers with a write.  The byte written
 *  to the register informs the DS2155 which bits the user wishes to read and have cleared.  The
 *  user writes a byte to one of these registers, with a 1 in the bit potions the user wishes to
 *  reate and a 0 in the bit positions the user does not wish to obtain the latest information on.
 *  When a 1 is written to a bit locaktion, the read register is updated with the latest
 *  information.  When a 0 is written to t abit osition, the read regsiter is not updated and the
 *  previous value is held.  A write to the status registers is immediately followed by a read of
 *  the same register.  This read-write scheme allows an external microcontroller or microprocessor
 *  to individually poll certain bits without disturbing the other bits in the register.  This
 *  operation is key in controlling the DS2155 with higher order languages.  
 */
STATIC __hot irqreturn_t
xp_e401_interrupt(int irq, void *dev_id, struct pt_regs * regs)
{
	struct cd *cd = (struct cd *) dev_id;

	/* active interrupt (otherwise suprious or shared) */
	if (cd->xlb[STAREG] & INTACTIVE) {
		struct sp *sp;
		int span;

		cd->xlb[CTLREG] = (INTENA | OUTBIT | INTACK | E1DIV);
		xp_e1_txrx_burst(cd);
		for (span = 0; span < X400_SPANS; span++) {
			if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
				volatile uint8_t *xlb = &cd->xlb[span << 8];

				if (sp->recovertime && !--sp->recovertime) {
					printd(("%s: alarm recovery complete\n", __FUNCTION__));
					sp->config.ifalarms &= ~SDL_ALARM_REC;
					xlb[0xd1] = 0x5f;	/* turn off yellow */
					cd->eval_syncsrc = 1;
				}
			}
		}
		/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
		if ((span = ((cd->frame >> 3) & 0x3f) - 50) >= 0 && span < X400_SPANS
		    && (sp = cd->spans[span])
		    && (sp->config.ifflags & SDL_IF_UP)) {
			int status, alarms = 0, leds = 0, all_leds;
			volatile uint8_t *xlb = &cd->xlb[span << 8];

			/** Old rxlevel 00 => New tx level 0000, 0001, 0010  -0.0dB -  -7.5dB
			 **             01 =>              0011, 0100, 0101  -7.5dB - -15.0dB
			 **             10 =>              0110, 0111, 1000 -15.0dB - -22.5dB
			 **             11 =>              1001, 1010, 1011 -22.5dB - -30.0dB
			 **                                1100, 1101, 1110 -30.0dB - -37.5dB
			 **                                1111             -37.5dB and less  */
			sp->config.ifrxlevel = (xlb[0x11] & 0x0f) / 3;

			/* write-read cycle */
			xlb[0x18] = 0x07;
			status = xlb[0x18];

			if (status & 0x05)	/* RUA1 or RLOS */
				alarms |= SDL_ALARM_RED;
			if (status & 0x02)	/* RCL */
				alarms |= SDL_ALARM_BLU;
			if (alarms) {
				if (!(sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just begun */
					xlb[0xd1] = 0x7f;	/* set yellow alarm (TNAF) */
					cd->eval_syncsrc = 1;
				}
			} else {
				if ((sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just ended */
					alarms |= SDL_ALARM_REC;
					sp->recovertime = X400P_SDL_ALARM_SETTLE_TIME;
				}
			}
			xlb[0x1a] = 0x01;
			status = xlb[0x1a];
			if (status & 0x01)	/* RRA */
				alarms |= SDL_ALARM_YEL;
			sp->config.ifalarms = alarms;
			/* adjust leds */
			if (alarms & SDL_ALARM_RED)
				leds |= LEDRED;
			else if (alarms & SDL_ALARM_YEL)
				leds |= LEDYEL;
			else
				leds |= LEDGRN;
			all_leds = cd->leds;
			all_leds &= ~(LEDYEL << (span << 1));
			all_leds |= leds << (span << 1);
			if (cd->leds != all_leds) {
				cd->xlb[LEDREG] = all_leds;
				cd->leds = all_leds;
			}
		}
		// if (!(cd->frame % 8000))
		if (!(cd->frame & 0x1fff)) {	/* 1.024 seconds */
			for (span = 0; span < X400_SPANS; span++)
				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					volatile uint8_t *xlb = &cd->xlb[span << 8];

					// printd(("%s: accumulating bipolar violations\n",
					// __FUNCTION__));
					sp->config.ifbpv += ((ushort) xlb[0x42] << 8) + xlb[0x43];
				}
		}
		if (xchg((int *) &cd->eval_syncsrc, 0)) {
			int src, syncsrc = 0;

			for (src = 0; src < SDL_SYNCS; src++) {
				if ((span = cd->config.ifsyncsrc[src]) && (--span < X400_SPANS)
				    && cd->spans[span]
				    && (cd->spans[span]->config.ifflags & SDL_IF_UP)
				    && !(cd->spans[span]->config.ifclock == SDL_CLOCK_LOOP)
				    && !(cd->spans[span]->config.
					 ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					syncsrc = cd->config.ifsyncsrc[src];
					break;
				}
			}
			if (cd->config.ifsync != syncsrc) {
				cd->xlb[SYNREG] = syncsrc;
				cd->config.ifsync = syncsrc;
			}
		}
		cd->frame += 8;
		cd->xlb[CTLREG] = (INTENA | E1DIV);
		return (irqreturn_t) (IRQ_HANDLED);
	}
	return (irqreturn_t) (IRQ_NONE);
}

static noinline fastcall __hot void
xp_t1_txrx_burst(struct cd *cd)
{
	int lebno;

	if ((lebno = (cd->lebno + 1) & (X400P_EBUFNO - 1)) != cd->uebno) {
		register int slot;
		register volatile uint32_t *xll;

		/* PCI reads and writes are soooo slooowww that we want to get the prefetch engine
		   working in parallel.  We only lead by two words to avoid a worse condition:
		   cache ping-pong.  These prefetches are for read. */
		{
			register const uint32_t *wbuf = cd->wbuf + (lebno << 8);

			for (xll = cd->xll, wbuf = cd->wbuf + (lebno << 8);
			     wbuf < cd->wbuf + (lebno << 8) + 256;) {
				for (wbuf++, xll++, slot = 1; slot < 32; slot++, xll++, wbuf++)
					if (slot & 0x3) {
						prefetch(wbuf + 2);
						*xll = *wbuf;
					}
			}
		}
		/* PCI reads and writes are soooo slooowww that we want to get the prefetch engine
		   working in parallel.  We only lead by two words to avoid a worse condition:
		   cache ping-pong.  These prefetches are for read. */
		{
			register uint32_t *rbuf = cd->rbuf + (lebno << 8);

			for (xll = cd->xll, rbuf = cd->rbuf + (lebno << 8);
			     rbuf < cd->rbuf + (lebno << 8) + 256;)
				for (rbuf++, xll++, slot = 1; slot < 32; slot++, xll++, rbuf++)
					if (slot & 0x3) {
						prefetchw(rbuf + 2);
						*rbuf = *xll;
					}
		}
		cd->lebno = lebno;
		tasklet_hi_schedule(&cd->tasklet);
	} else
		xp_overflow(cd);
}

/*
 *  T400P-SS7 Interrupt Service Routine
 *  -----------------------------------
 *  The user will always preceed a read o f any fo the SR1, SR2, and RIR registers with a write.
 *  The user will write a byte to one of these registers, with a one in the bit positions he or she
 *  wishes to read and a zero in the bit positions he or she does not wish to obtain the latest
 *  information on.  WHen a one is written to a bit location, the read register will be updated with
 *  the latest information.  When a zero is written to a bit position, the read register will not be
 *  updated and the previous value will be held.  A write to the status and information registers
 *  will be immediated followed by a read of the same register.  The read result should be logically
 *  ANDed with the mask byte that was just written and this value should be written back to the same
 *  register to insure that the bit does indeed clear.  This second write step is necessary because
 *  the alarms and events in the statue registers occur asyncrhonously in respect to their access
 *  via the parallel port.  This write-read-write scheme allows an external controller or
 *  microprocessor to individually poll certain bits without disturbing the other bits in the
 *  register.  This operation is key in controlling the DS21354/DS21554 wtih higher-order software
 *  languages.
 */
STATIC __hot irqreturn_t
xp_t400_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct cd *cd = (struct cd *) dev_id;

	/* active interrupt (otherwise spurious or shared) */
	if (cd->xlb[STAREG] & INTACTIVE) {
		struct sp *sp;
		int span;

		cd->xlb[CTLREG] = (INTENA | OUTBIT | INTACK);
		xp_t1_txrx_burst(cd);
		for (span = 0; span < X400_SPANS; span++) {
			if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
				volatile uint8_t *xlb = &cd->xlb[span << 8];

				if (sp->recovertime && !--sp->recovertime) {
					/* alarm recovery complete */
					sp->config.ifalarms &= ~SDL_ALARM_REC;
					/* turn off yellow alarm */
					xlb[0x35] = 0x10;	/* TSSE TCR1.4, ~TYEL TCR1.0 */
					cd->eval_syncsrc = 1;
				}
			}
		}
		/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
		if ((span = ((cd->frame >> 3) & 0x3f) - 50) >= 0 && span < X400_SPANS
		    && (sp = cd->spans[span])
		    && (sp->config.ifflags & SDL_IF_UP)) {
			int status, alarms = 0, leds = 0, all_leds;
			volatile uint8_t *xlb = &cd->xlb[span << 8];

			/* tor2 driver has this at 0x31 for some silly reason */
			/** Old rxlevel 00 => New tx level 0000, 0001, 0010  -0.0dB -  -7.5dB
			 **             01 =>              0011, 0100, 0101  -7.5dB - -15.0dB
			 **             10 =>              0110, 0111, 1000 -15.0dB - -22.5dB
			 **             11 =>              1001, 1010, 1011 -22.5dB - -30.0dB
			 **                                1100, 1101, 1110 -30.0dB - -37.5dB
			 **                                1111             -37.5dB and less  */
			sp->config.ifrxlevel = xlb[0x10] >> 6;

			/* write-read-write cycle */
			xlb[0x20] = 0xcf;
			status = xlb[0x20];
			xlb[0x20] = status & 0xcf;

			/* loop up code LUP SR1.7 */
			if ((status & 0x80) && !(sp->config.ifgmode & SDL_GMODE_LOC_LB)) {
				if (sp->loopcnt++ > 80 && !(sp->config.ifgmode & SDL_GMODE_REM_LB)) {
					int japan = (sp->config.ifgtype == SDL_GTYPE_J1);

					xlb[0x1e] = japan ? 0x80 : 0x00;	/* no local loop */
					xlb[0x40] = 0x40;	/* remote loop */
					sp->config.ifgmode |= SDL_GMODE_REM_LB;
				}
			} else
				sp->loopcnt = 0;
			/* loop down code LDN SR1.6 */
			if ((status & 0x40) && !(sp->config.ifgmode & SDL_GMODE_LOC_LB)) {
				/* loop down code */
				if (sp->loopcnt++ > 80 && (sp->config.ifgmode & SDL_GMODE_REM_LB)) {
					int japan = (sp->config.ifgtype == SDL_GTYPE_J1);

					xlb[0x1e] = japan ? 0x80 : 0x00;	/* no local loop */
					xlb[0x40] = 0x00;	/* no remote loop */
					sp->config.ifgmode &= ~SDL_GMODE_REM_LB;
				}
			} else
				sp->loopcnt = 0;
			if (status & 0x03)	/* RLOS (SR1.0) or LRCL (SR1.1) */
				alarms |= SDL_ALARM_RED;
			if (status & 0x08)	/* RBL (SR1.3) */
				alarms |= SDL_ALARM_BLU;
			if (alarms) {
				if (!(sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just begun */
					/* turn on yellow alarm */
					xlb[0x35] = 0x11;	/* TSSE TCR1.4, TYEL TCR1.0 */
					cd->eval_syncsrc = 1;
				}
			} else {
				if ((sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just ended */
					alarms |= SDL_ALARM_REC;
					sp->recovertime = X400P_SDL_ALARM_SETTLE_TIME;
				}
			}
			if (status & 0x04)	/* RYEL (SR1.2) */
				alarms |= SDL_ALARM_YEL;
			sp->config.ifalarms = alarms;
			/* adjust leds */
			if (alarms & SDL_ALARM_RED)
				leds |= LEDRED;
			else if (alarms & SDL_ALARM_YEL)
				leds |= LEDYEL;
			else
				leds |= LEDGRN;
			all_leds = cd->leds;
			all_leds &= ~(LEDYEL << (span << 1));
			all_leds |= leds << (span << 1);
			if (cd->leds != all_leds) {
				cd->xlb[LEDREG] = all_leds;
				cd->leds = all_leds;
			}
		}
		// if (!(cd->frame % 8000))
		if (!(cd->frame & 0x1fff)) {	/* 1.024 seconds */
			for (span = 0; span < X400_SPANS; span++)
				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					volatile uint8_t *xlb = &cd->xlb[span << 8];

					// printd(("%s: accumulating bipolar violations\n",
					// __FUNCTION__));
					sp->config.ifbpv += xlb[0x24] + (xlb[0x23] << 8);
				}
		}
		if (xchg((int *) &cd->eval_syncsrc, 0)) {
			int src, syncsrc = 0;

			for (src = 0; src < SDL_SYNCS; src++) {
				if ((span = cd->config.ifsyncsrc[src]) && (--span < X400_SPANS)
				    && cd->spans[span]
				    && (cd->spans[span]->config.ifflags & SDL_IF_UP)
				    && !(cd->spans[span]->config.ifclock == SDL_CLOCK_LOOP)
				    && !(cd->spans[span]->config.
					 ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					syncsrc = cd->config.ifsyncsrc[src];
					break;
				}
			}
			if (cd->config.ifsync != syncsrc) {
				cd->xlb[SYNREG] = syncsrc;
				cd->config.ifsync = syncsrc;
			}
		}
		cd->frame += 8;
		cd->xlb[CTLREG] = (INTENA);
		return (irqreturn_t) (IRQ_HANDLED);
	}
	return (irqreturn_t) (IRQ_NONE);
}

/*
 *  V401P-T Interrupt Service Routine
 *  ---------------------------------
 *  The user always precedes a read of any of the status registers with a write.  The byte written
 *  to the register informs the DS2155 which bits the user wishes to read and have cleared.  The
 *  user writes a byte to one of these registers, with a 1 in the bit potions the user wishes to
 *  reate and a 0 in the bit positions the user does not wish to obtain the latest information on.
 *  When a 1 is written to a bit locaktion, the read register is updated with the latest
 *  information.  When a 0 is written to t abit osition, the read regsiter is not updated and the
 *  previous value is held.  A write to the status registers is immediately followed by a read of
 *  the same register.  This read-write scheme allows an external microcontroller or microprocessor
 *  to individually poll certain bits without disturbing the other bits in the register.  This
 *  operation is key in controlling the DS2155 with higher order languages.  
 */
STATIC __hot irqreturn_t
xp_t401_interrupt(int irq, void *dev_id, struct pt_regs * regs)
{
	struct cd *cd = (struct cd *) dev_id;

	/* active interrupt (otherwise spurious or shared) */
	if (cd->xlb[STAREG] & INTACTIVE) {
		struct sp *sp;
		int span;

		cd->xlb[CTLREG] = (INTENA | OUTBIT | INTACK);
		xp_t1_txrx_burst(cd);
		for (span = 0; span < X400_SPANS; span++) {
			if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
				volatile uint8_t *xlb = &cd->xlb[span << 8];

				if (sp->recovertime && !--sp->recovertime) {
					u_char japan =
					    (sp->config.ifgcrc == SDL_GCRC_CRC6J) ? 0x80 : 0x00;
					printd(("%s: alarm recovery complete\n", __FUNCTION__));
					sp->config.ifalarms &= ~SDL_ALARM_REC;
					xlb[0x05] = 0x10 | japan;	/* TSSE, turn off yellow */
					cd->eval_syncsrc = 1;
				}
			}
		}
		/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
		if ((span = ((cd->frame >> 3) & 0x3f) - 50) >= 0 && span < X400_SPANS
		    && (sp = cd->spans[span])
		    && (sp->config.ifflags & SDL_IF_UP)) {
			int status, alarms = 0, leds = 0, all_leds;
			volatile uint8_t *xlb = &cd->xlb[span << 8];

			/** Old rxlevel 00 => New tx level 0000, 0001, 0010  -0.0dB -  -7.5dB
			 **             01 =>              0011, 0100, 0101  -7.5dB - -15.0dB
			 **             10 =>              0110, 0111, 1000 -15.0dB - -22.5dB
			 **             11 =>              1001, 1010, 1011 -22.5dB - -30.0dB
			 **                                1100, 1101, 1110 -30.0dB - -37.5dB
			 **                                1111             -37.5dB and less  */
			sp->config.ifrxlevel = (xlb[0x11] & 0x0f) / 3;

			/* write-read cycle */
			xlb[0x1a] = 0xff;
			status = xlb[0x1a];

			/* loop up code LUP SR3.5 */
			if ((status & 0x20) && !(sp->config.ifgmode & SDL_GMODE_LOC_LB)) {
				if (sp->loopcnt++ > 80 && !(sp->config.ifgmode & SDL_GMODE_REM_LB)) {
					xlb[0x4a] = 0x40;	/* remote loop, no local loop */
					sp->config.ifgmode |= SDL_GMODE_REM_LB;
				}
			} else
				sp->loopcnt = 0;
			/* loop down code LDN SR3.6 */
			if ((status & 0x40) && !(sp->config.ifgmode & SDL_GMODE_LOC_LB)) {
				/* loop down code */
				if (sp->loopcnt++ > 80 && (sp->config.ifgmode & SDL_GMODE_REM_LB)) {
					xlb[0x4a] = 0x00;	/* no remote loop, no local loop */
					sp->config.ifgmode &= ~SDL_GMODE_REM_LB;
				}
			} else
				sp->loopcnt = 0;

			/* write-read cycle */
			xlb[0x18] = 0xff;
			status = xlb[0x18];

			if (status & 0x03)	/* RLOS (SR2.0) or RRCL (SR2.1) */
				alarms |= SDL_ALARM_RED;
			if (status & 0x04)	/* RUA1 (SR2.2) */
				alarms |= SDL_ALARM_BLU;
			if (alarms) {
				if (!(sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					u_char japan =
					    (sp->config.ifgcrc == SDL_GCRC_CRC6J) ? 0x80 : 0x00;

					/* alarms have just begun */
					xlb[0x05] = 0x11 | japan;	/* TSSE, set yellow alarm */
					cd->eval_syncsrc = 1;
				}
			} else {
				if ((sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just ended */
					alarms |= SDL_ALARM_REC;
					sp->recovertime = X400P_SDL_ALARM_SETTLE_TIME;
				}
			}
			if (status & 0x08)	/* RYEL (SR2.3) */
				alarms |= SDL_ALARM_YEL;
			sp->config.ifalarms = alarms;
			/* adjust leds */
			if (alarms & SDL_ALARM_RED)
				leds |= LEDRED;
			else if (alarms & SDL_ALARM_YEL)
				leds |= LEDYEL;
			else
				leds |= LEDGRN;
			all_leds = cd->leds;
			all_leds &= ~(LEDYEL << (span << 1));
			all_leds |= leds << (span << 1);
			if (cd->leds != all_leds) {
				cd->xlb[LEDREG] = all_leds;
				cd->leds = all_leds;
			}
		}
		// if (!(cd->frame % 8000))
		if (!(cd->frame & 0x1fff)) {	/* 1.024 seconds */
			for (span = 0; span < X400_SPANS; span++)
				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					volatile uint8_t *xlb = &cd->xlb[span << 8];

					// printd(("%s: accumulating bipolar violations\n",
					// __FUNCTION__));
					sp->config.ifbpv += ((ushort) xlb[0x42] << 8) + xlb[0x43];
				}
		}
		if (xchg((int *) &cd->eval_syncsrc, 0)) {
			int src, syncsrc = 0;

			for (src = 0; src < SDL_SYNCS; src++) {
				if ((span = cd->config.ifsyncsrc[src]) && (--span < X400_SPANS)
				    && cd->spans[span]
				    && (cd->spans[span]->config.ifflags & SDL_IF_UP)
				    && !(cd->spans[span]->config.ifclock == SDL_CLOCK_LOOP)
				    && !(cd->spans[span]->config.
					 ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					syncsrc = cd->config.ifsyncsrc[src];
					break;
				}
			}
			if (cd->config.ifsync != syncsrc) {
				cd->config.ifsync = syncsrc;
				if (syncsrc) {
					cd->xlb[(0 << 8) + 0x70] = 0x00;
					cd->xlb[(1 << 8) + 0x70] = 0x00;
					cd->xlb[(2 << 8) + 0x70] = 0x00;
					cd->xlb[(3 << 8) + 0x70] = 0x00;
				} else {
					cd->xlb[(0 << 8) + 0x70] = 0x06;
					cd->xlb[(1 << 8) + 0x70] = 0x06;
					cd->xlb[(2 << 8) + 0x70] = 0x06;
					cd->xlb[(3 << 8) + 0x70] = 0x06;
					syncsrc = SYNC1;
				}
				cd->xlb[SYNREG] = syncsrc;
			}
		}
		cd->frame += 8;
		cd->xlb[CTLREG] = (INTENA);
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
STATIC noinline fastcall __unlikely int
xp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
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
			ptrace(("%s: ERROR: Unknown IOCTL %d\n", DRV_NAME, cmd));
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
		ptrace(("%s: ERROR: Unsupported STREAMS ioctl\n", DRV_NAME));
		ret = -EOPNOTSUPP;
		break;
	}
	case SL_IOC_MAGIC:
	{
		if (count < size || XP_PRIV(q)->state == LMI_UNATTACHED) {
			return -EINVAL;
			break;
		}
		switch (cmd) {
		case SL_IOCGOPTIONS:	/* lmi_option_t */
			ret = sl_iocgoptions(q, mp);
			break;
		case SL_IOCSOPTIONS:	/* lmi_option_t */
			ret = sl_iocsoptions(q, mp);
			break;
		case SL_IOCGCONFIG:	/* sl_config_t */
			ret = sl_iocgconfig(q, mp);
			break;
		case SL_IOCSCONFIG:	/* sl_config_t */
			ret = sl_iocsconfig(q, mp);
			break;
		case SL_IOCTCONFIG:	/* sl_config_t */
			ret = sl_ioctconfig(q, mp);
			break;
		case SL_IOCCCONFIG:	/* sl_config_t */
			ret = sl_ioccconfig(q, mp);
			break;
		case SL_IOCGSTATEM:	/* sl_statem_t */
			ret = sl_iocgstatem(q, mp);
			break;
		case SL_IOCCMRESET:	/* sl_statem_t */
			ret = sl_ioccmreset(q, mp);
			break;
		case SL_IOCGSTATSP:	/* lmi_sta_t */
			ret = sl_iocgstatsp(q, mp);
			break;
		case SL_IOCSSTATSP:	/* lmi_sta_t */
			ret = sl_iocsstatsp(q, mp);
			break;
		case SL_IOCGSTATS:	/* sl_stats_t */
			ret = sl_iocgstats(q, mp);
			break;
		case SL_IOCCSTATS:	/* sl_stats_t */
			ret = sl_ioccstats(q, mp);
			break;
		case SL_IOCGNOTIFY:	/* sl_notify_t */
			ret = sl_iocgnotify(q, mp);
			break;
		case SL_IOCSNOTIFY:	/* sl_notify_t */
			ret = sl_iocsnotify(q, mp);
			break;
		case SL_IOCCNOTIFY:	/* sl_notify_t */
			ret = sl_ioccnotify(q, mp);
			break;
		default:
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDT_IOC_MAGIC:
	{
		if (count < size || XP_PRIV(q)->state == LMI_UNATTACHED) {
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
		case SDT_IOCCABORT:	/* */
			ret = sdt_ioccabort(q, mp);
			break;
		default:
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || xp->i_state == LMI_UNATTACHED) {
			ptrace(("%s: ERROR: ioctl count = %d, size = %d, state = %ld\n",
				DRV_NAME, count, size, xp->i_state));
			ret = -EINVAL;
			break;
		}
		switch (cmd) {
		case SDL_IOCGOPTIONS:	/* lmi_option_t */
			printd(("%s: %p: SDL_IOCGOPTIONS\n", DRV_NAME, xp));
			ret = sdl_iocgoptions(q, mp);
			break;
		case SDL_IOCSOPTIONS:	/* lmi_option_t */
			printd(("%s: %p: SDL_IOCSOPTIONS\n", DRV_NAME, xp));
			ret = sdl_iocsoptions(q, mp);
			break;
		case SDL_IOCGCONFIG:	/* sdl_config_t */
			printd(("%s: %p: SDL_IOCGCONFIG\n", DRV_NAME, xp));
			ret = sdl_iocgconfig(q, mp);
			break;
		case SDL_IOCSCONFIG:	/* sdl_config_t */
			printd(("%s: %p: SDL_IOCSCONFIG\n", DRV_NAME, xp));
			ret = sdl_iocsconfig(q, mp);
			break;
		case SDL_IOCTCONFIG:	/* sdl_config_t */
			printd(("%s: %p: SDL_IOCTCONFIG\n", DRV_NAME, xp));
			ret = sdl_ioctconfig(q, mp);
			break;
		case SDL_IOCCCONFIG:	/* sdl_config_t */
			printd(("%s: %p: SDL_IOCCCONFIG\n", DRV_NAME, xp));
			ret = sdl_ioccconfig(q, mp);
			break;
		case SDL_IOCGSTATEM:	/* sdl_statem_t */
			printd(("%s: %p: SDL_IOCGSTATEM\n", DRV_NAME, xp));
			ret = sdl_iocgstatem(q, mp);
			break;
		case SDL_IOCCMRESET:	/* sdl_statem_t */
			printd(("%s: %p: SDL_IOCCMRESET\n", DRV_NAME, xp));
			ret = sdl_ioccmreset(q, mp);
			break;
		case SDL_IOCGSTATSP:	/* sdl_stats_t */
			printd(("%s: %p: SDL_IOCGSTATSP\n", DRV_NAME, xp));
			ret = sdl_iocgstatsp(q, mp);
			break;
		case SDL_IOCSSTATSP:	/* sdl_stats_t */
			printd(("%s: %p: SDL_IOCSSTATSP\n", DRV_NAME, xp));
			ret = sdl_iocsstatsp(q, mp);
			break;
		case SDL_IOCGSTATS:	/* sdl_stats_t */
			printd(("%s: %p: SDL_IOCGSTATS\n", DRV_NAME, xp));
			ret = sdl_iocgstats(q, mp);
			break;
		case SDL_IOCCSTATS:	/* sdl_stats_t */
			printd(("%s: %p: SDL_IOCCSTATS\n", DRV_NAME, xp));
			ret = sdl_ioccstats(q, mp);
			break;
		case SDL_IOCGNOTIFY:	/* sdl_notify_t */
			printd(("%s: %p: SDL_IOCGNOTIFY\n", DRV_NAME, xp));
			ret = sdl_iocgnotify(q, mp);
			break;
		case SDL_IOCSNOTIFY:	/* sdl_notify_t */
			printd(("%s: %p: SDL_IOCSNOTIFY\n", DRV_NAME, xp));
			ret = sdl_iocsnotify(q, mp);
			break;
		case SDL_IOCCNOTIFY:	/* sdl_notify_t */
			printd(("%s: %p: SDL_IOCCNOTIFY\n", DRV_NAME, xp));
			ret = sdl_ioccnotify(q, mp);
			break;
		case SDL_IOCCDISCTX:	/* */
			printd(("%s: %p: SDL_IOCCDISCTX\n", DRV_NAME, xp));
			ret = sdl_ioccdisctx(q, mp);
			break;
		case SDL_IOCCCONNTX:	/* */
			printd(("%s: %p: SDL_IOCCCONNTX\n", DRV_NAME, xp));
			ret = sdl_ioccconntx(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SDL ioctl\n", DRV_NAME));
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
STATIC noinline fastcall __unlikely int
xp_w_proto_slow(queue_t *q, mblk_t *mp)
{
	int rtn;
	sl_ulong prim;
	struct xp *xp = XP_PRIV(q);
	int oldstate = xp->i_state;

	switch ((prim = *(sl_ulong *) mp->b_rptr)) {
	case SL_PDU_REQ:
		printd(("%s: %p: -> SL_PDU_REQ\n", DRV_NAME, xp));
		rtn = sl_pdu_req(q, mp);
		break;
	case SL_EMERGENCY_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_REQ\n", DRV_NAME, xp));
		rtn = sl_emergency_req(q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_CEASES_REQ\n", DRV_NAME, xp));
		rtn = sl_emergency_ceases_req(q, mp);
		break;
	case SL_START_REQ:
		printd(("%s: %p: -> SL_START_REQ\n", DRV_NAME, xp));
		rtn = sl_start_req(q, mp);
		break;
	case SL_STOP_REQ:
		printd(("%s: %p: -> SL_STOP_REQ\n", DRV_NAME, xp));
		rtn = sl_stop_req(q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		printd(("%s: %p: -> SL_RETRIEVE_BSNT_REQ\n", DRV_NAME, xp));
		rtn = sl_retrieve_bsnt_req(q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		printd(("%s: %p: -> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ\n", DRV_NAME, xp));
		rtn = sl_retrieval_request_and_fsnc_req(q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		printd(("%s: %p: -> SL_CLEAR_BUFFERS_REQ\n", DRV_NAME, xp));
		rtn = sl_clear_buffers_req(q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		printd(("%s: %p: -> SL_CLEAR_RTB_REQ\n", DRV_NAME, xp));
		rtn = sl_clear_rtb_req(q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		printd(("%s: %p: -> SL_LOCAL_PROCESSOR_OUTAGE_REQ\n", DRV_NAME, xp));
		rtn = sl_local_processor_outage_req(q, mp);
		break;
	case SL_CONTINUE_REQ:
		printd(("%s: %p: -> SL_CONTINUE_REQ\n", DRV_NAME, xp));
		rtn = sl_continue_req(q, mp);
		break;
	case SL_RESUME_REQ:
		printd(("%s: %p: -> SL_RESUME_REQ\n", DRV_NAME, xp));
		rtn = sl_resume_req(q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		printd(("%s: %p: -> SL_CONGESTION_DISCARD_REQ\n", DRV_NAME, xp));
		rtn = sl_congestion_discard_req(q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		printd(("%s: %p: -> SL_CONGESTION_ACCEPT_REQ\n", DRV_NAME, xp));
		rtn = sl_congestion_accept_req(q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		printd(("%s: %p: -> SL_NO_CONGESTION_REQUEST\n", DRV_NAME, xp));
		rtn = sl_no_congestion_req(q, mp);
		break;
	case SL_POWER_ON_REQ:
		printd(("%s: %p: -> SL_POWER_ON_REQ\n", DRV_NAME, xp));
		rtn = sl_power_on_req(q, mp);
		break;
	case SDT_DAEDT_TRANSMISSION_REQ:
		_printd(("%s: %p: -> SDT_DAEDT_TRANSMISSION_REQ\n", DRV_NAME, xp));
		rtn = sdt_daedt_transmission_req(q, mp);
		break;
	case SDT_DAEDT_START_REQ:
		printd(("%s: %p: -> SDT_DAEDT_START_REQ\n", DRV_NAME, xp));
		rtn = sdt_daedt_start_req(q, mp);
		break;
	case SDT_DAEDR_START_REQ:
		printd(("%s: %p: -> SDT_DAEDR_START_REQ\n", DRV_NAME, xp));
		rtn = sdt_daedr_start_req(q, mp);
		break;
	case SDT_AERM_START_REQ:
		printd(("%s: %p: -> SDT_AERM_START_REQ\n", DRV_NAME, xp));
		rtn = sdt_aerm_start_req(q, mp);
		break;
	case SDT_AERM_STOP_REQ:
		printd(("%s: %p: -> SDT_AERM_STOP_REQ\n", DRV_NAME, xp));
		rtn = sdt_aerm_stop_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIN_REQ\n", DRV_NAME, xp));
		rtn = sdt_aerm_set_ti_to_tin_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIE_REQ\n", DRV_NAME, xp));
		rtn = sdt_aerm_set_ti_to_tie_req(q, mp);
		break;
	case SDT_SUERM_START_REQ:
		printd(("%s: %p: -> SDT_SUERM_START_REQ\n", DRV_NAME, xp));
		rtn = sdt_suerm_start_req(q, mp);
		break;
	case SDT_SUERM_STOP_REQ:
		printd(("%s: %p: -> SDT_SUERM_STOP_REQ\n", DRV_NAME, xp));
		rtn = sdt_suerm_stop_req(q, mp);
		break;
	case SDL_BITS_FOR_TRANSMISSION_REQ:
		_printd(("%s: %p: -> SDL_BITS_FOR_TRANSMISSION_REQ\n", DRV_NAME, xp));
		rtn = sdl_bits_for_transmission_req(q, mp);
		break;
	case SDL_CONNECT_REQ:
		printd(("%s: %p: -> SDL_CONNECT_REQ\n", DRV_NAME, xp));
		rtn = sdl_connect_req(q, mp);
		break;
	case SDL_DISCONNECT_REQ:
		printd(("%s: %p: -> SDL_DISCONNECT_REQ\n", DRV_NAME, xp));
		rtn = sdl_disconnect_req(q, mp);
		break;
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", DRV_NAME, xp));
		rtn = lmi_info_req(q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", DRV_NAME, xp));
		rtn = lmi_attach_req(q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", DRV_NAME, xp));
		rtn = lmi_detach_req(q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", DRV_NAME, xp));
		rtn = lmi_enable_req(q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", DRV_NAME, xp));
		rtn = lmi_disable_req(q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", DRV_NAME, xp));
		rtn = lmi_optmgmt_req(q, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		xp->i_state = oldstate;
	return (rtn);
}

STATIC inline fastcall __hot_write int
xp_w_proto(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_wptr >= mp->b_rptr + sizeof(sl_ulong)))
		if (likely(*(sl_ulong *) mp->b_rptr == SL_PDU_REQ))
			return sl_pdu_req(q, mp);
	return xp_w_proto_slow(q, mp);
}

/*
 *  M_SIG/M_PCSIG Handling
 *  -------------------------------------------------------------------------
 */
static noinline fastcall __unlikely int
xp_r_sig(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	int rtn = QR_ABSORBED;

	if (likely(ctrace(mi_timer_valid(mp)))) {
		switch (*(int *) mp->b_rptr) {
		case t1:
			printd(("%s: %p: -> T1 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t1_timeout(xp, q);
			break;
		case t2:
			printd(("%s: %p: -> T2 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t2_timeout(xp, q);
			break;
		case t3:
			printd(("%s: %p: -> T3 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t3_timeout(xp, q);
			break;
		case t4:
			printd(("%s: %p: -> T4 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t4_timeout(xp, q);
			break;
		case t5:
			printd(("%s: %p: -> T5 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t5_timeout(xp, q);
			break;
		case t6:
			printd(("%s: %p: -> T6 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t6_timeout(xp, q);
			break;
		case t7:
			printd(("%s: %p: -> T7 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t7_timeout(xp, q);
			break;
		case t8:
			printd(("%s: %p: -> T8 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t8_timeout(xp, q);
			break;
#if 0
		case t9:
			printd(("%s: %p: -> T9 TIMEOUT <-\n", DRV_NAME, xp));
			rtn = xp_t9_timeout(xp, q);
			break;
#endif
		default:
			swerr();
			rtn = QR_ABSORBED;
			break;
		}
		if (rtn == QR_DONE)
			rtn = QR_ABSORBED;
	}
	return (rtn);
}

/*
 *  M_DATA Handling
 *  -------------------------------------------------------------------------
 */
STATIC noinline fastcall __hot_write int
xp_w_data(queue_t *q, mblk_t *mp)
{
	return xp_send_data(q, mp);
}
STATIC noinline fastcall __hot_read int
xp_r_data(queue_t *q, mblk_t *mp)
{
	if (canputnext(q)) {
		struct xp *xp = XP_PRIV(q);

		xp->sl.statem.Cr--;
		putnext(q, mp);
		return (QR_ABSORBED);
	}
	return (-EBUSY);
}

/*
 *  M_FLUSH Handling
 *  -------------------------------------------------------------------------
 */
STATIC noinline fastcall __unlikely int
xp_w_flush(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);

	if (*mp->b_rptr & FLUSHW) {
		if (xp) {
			psw_t flags;

			spin_lock_irqsave(&xp->lock, flags);
			if (xp->tx.cmp) {
				xp->tx.cmp = NULL;
				xp->tx.repeat = 0;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
		}
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		if (xp) {
			psw_t flags;

			spin_lock_irqsave(&xp->lock, flags);
			if (xp->rx.cmp) {
				freeb(xp->rx.cmp);
				xp->rx.cmp = NULL;
				xp->rx.repeat = 0;
			}
			spin_unlock_irqrestore(&xp->lock, flags);
		}
		if (*mp->b_rptr & FLUSHBAND)
			flushband(OTHERQ(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(OTHERQ(q), FLUSHALL);
		qreply(q, mp);
		return (QR_ABSORBED);
	}
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC streamscall __hot_in int
xp_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return xp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return xp_r_data(q, mp);
	case M_SIG:
	case M_PCSIG:
		return xp_r_sig(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC streamscall __hot_write int
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

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC streamscall __unlikely int
xp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct xp *xp, **xpp = &xp_list;

	(void) crp;
	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
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
		major_t dmajor = (*xpp)->u.dev.cmajor;

		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*xpp)->u.dev.cminor;

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
		return (ENXIO);
	}
	printd(("%s: opened character device %hu:%hu\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(xp = xp_alloc_priv(q, xpp, devp, crp))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&xp_lock, flags);
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&xp_lock, flags);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC streamscall __unlikely int
xp_close(queue_t *q, int flag, cred_t *crp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;

	(void) flag;
	(void) crp;
	(void) xp;
	printd(("%s: closing character device %hu:%hu\n", DRV_NAME, xp->u.dev.cmajor,
		xp->u.dev.cminor));
	spin_lock_irqsave(&xp_lock, flags);
	xp_free_priv(xp);
	spin_unlock_irqrestore(&xp_lock, flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  ==========================================================================
 *
 *  Private and Card structure allocation and deallocation
 *
 *  ==========================================================================
 */

STATIC kmem_cache_t *xp_priv_cachep = NULL;
STATIC kmem_cache_t *xp_span_cachep = NULL;
STATIC kmem_cache_t *xp_card_cachep = NULL;
STATIC kmem_cache_t *xp_xbuf_cachep = NULL;

/*
 *  Cache allocation
 *  -------------------------------------------------------------------------
 */
STATIC noinline __devexit int
xp_term_caches(void)
{
	int err = 0;

	if (xp_xbuf_cachep) {
		if (kmem_cache_destroy(xp_xbuf_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_xbuf_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: shrunk xp_xbuf_cache to zero\n", DRV_NAME));
	}
	if (xp_card_cachep) {
		if (kmem_cache_destroy(xp_card_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_card_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: shrunk xp_card_cache to zero\n", DRV_NAME));
	}
	if (xp_span_cachep) {
		if (kmem_cache_destroy(xp_span_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_span_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: shrunk xp_span_cache to zero\n", DRV_NAME));
	}
	if (xp_priv_cachep) {
		if (kmem_cache_destroy(xp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: shrunk xp_priv_cache to zero\n", DRV_NAME));
	}
	return (err);
}

STATIC noinline __devinit int
xp_init_caches(void)
{
	if (!xp_priv_cachep &&
	    !(xp_priv_cachep =
	      kmem_cache_create("xp_priv_cachep", sizeof(struct xp), 0, SLAB_HWCACHE_ALIGN, NULL,
				NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_priv_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized device private structure cache\n", DRV_NAME));
	if (!xp_span_cachep &&
	    !(xp_span_cachep =
	      kmem_cache_create("xp_span_cachep", sizeof(struct sp), 0, SLAB_HWCACHE_ALIGN, NULL,
				NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_span_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized span private structure cache\n", DRV_NAME));
	if (!xp_card_cachep &&
	    !(xp_card_cachep =
	      kmem_cache_create("xp_card_cachep", sizeof(struct cd), 0, SLAB_HWCACHE_ALIGN, NULL,
				NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_card_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized card private structure cache\n", DRV_NAME));
	if (!xp_xbuf_cachep &&
	    !(xp_xbuf_cachep =
	      kmem_cache_create("xp_xbuf_cachep", X400P_EBUFNO * 1024, 0, SLAB_HWCACHE_ALIGN, NULL,
				NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_xbuf_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized card read/write buffer cache\n", DRV_NAME));
	return (0);
      error:
	return (-EFAULT);
}

/*
 *  Private structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC noinline __unlikely struct xp *
xp_alloc_priv(queue_t *q, struct xp **xpp, dev_t *devp, cred_t *crp)
{
	struct xp *xp;

	if ((xp = kmem_cache_alloc(xp_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated device private structure\n", DRV_NAME));
		bzero(xp, sizeof(*xp));
		xp_get(xp);	/* first get */
		spin_lock_init(&xp->lock);	/* "xp-priv-lock" */
		xp->u.dev.cmajor = getmajor(*devp);
		xp->u.dev.cminor = getminor(*devp);
		xp->cred = *crp;
		xp->o_prim = &xp_r_prim;
		xp->i_prim = &xp_w_prim;
		xp->o_wakeup = NULL;
		xp->i_wakeup = NULL;
		(xp->oq = RD(q))->q_ptr = xp_get(xp);
		(xp->iq = WR(q))->q_ptr = xp_get(xp);
		spin_lock_init(&xp->qlock);	/* "xp-queue-lock" */
		xp->i_version = 1;
		xp->i_style = LMI_STYLE2;
		xp->i_state = LMI_UNATTACHED;
		if (xp_alloc_timers(xp)) {
			xp_put(XCHG(&xp->iq->q_ptr, NULL));
			xp_put(XCHG(&xp->oq->q_ptr, NULL));
			xp_put(XCHG(&xp, NULL));
		} else {
			if ((xp->next = *xpp))
				xp->next->prev = &xp->next;
			xp->prev = xpp;
			*xpp = xp_get(xp);
			printd(("%s: linked device private structure\n", DRV_NAME));
			/* LMI configuration defaults */
			xp->option = lmi_default_e1_chan;
			/* SDL configuration defaults */
			bufq_init(&xp->sdl.tb);
			xp->sdl.config = sdl_default_e1_chan;
			/* SDT configuration defaults */
			bufq_init(&xp->sdt.tb);
			ss7_bufpool_reserve(&xp_bufpool, 2);
			xp->sdt.config = sdt_default_e1_chan;
			/* SL configuration defaults */
			bufq_init(&xp->sl.rb);
			bufq_init(&xp->sl.tb);
			bufq_init(&xp->sl.rtb);
			xp->sl.config = sl_default_e1_chan;
			printd(("%s: setting device private structure defaults\n", DRV_NAME));
		}
	} else
		ptrace(("%s: ERROR: Could not allocate device private structure\n", DRV_NAME));
	return (xp);
}

/**
 * xp_free_priv: - free a Stream private structure
 * @xp: private structure to free
 *
 * Note: this function must be called with CPU local interrupts already supressed.
 */
STATIC noinline __unlikely void
xp_free_priv(struct xp *xp)
{
	ensure(xp, return);
	spin_lock(&xp->lock);
	{
		struct sp *sp;

		if ((sp = xp->sp)) {
			struct cd *cd;

			if ((cd = sp->cd)) {
				spin_lock(&cd->lock);
				{
					int slot;
					uchar idle =
					    (cd->config.ifgtype == SDL_GTYPE_T1) ? 0x7f : 0xff;
					uchar *base = (uchar *) cd->wbuf + span_to_byte(sp->span);

					for (slot = 0; slot < 32; slot++) {
						if (sp->slots[slot] == xp) {
							int boff;

							xp_put(xchg(&sp->slots[slot], NULL));
							for (boff = 0; boff < X400P_EBUFNO; boff++)
								*(base + (boff << 10) +
								  (slot << 2)) = idle;
						}
					}
				}
				spin_unlock(&cd->lock);
			} else {
				int slot;

				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot] == xp)
						xp_put(xchg(&sp->slots[slot], NULL));
			}
			sp_put(xchg(&xp->sp, NULL));
			printd(("%s: unlinked device private structure from span\n", DRV_NAME));
		}
		ss7_unbufcall((str_t *) xp);
		xp_free_timers(xp);
		if (xp->tx.msg && xp->tx.msg != xp->tx.cmp)
			freemsg(xchg(&xp->tx.msg, NULL));
		if (xp->tx.cmp)
			freemsg(xchg(&xp->tx.cmp, NULL));
		if (xp->rx.msg)
			freemsg(xchg(&xp->rx.msg, NULL));
		if (xp->rx.nxt)
			freemsg(xchg(&xp->rx.nxt, NULL));
		if (xp->rx.cmp)
			freemsg(xchg(&xp->rx.cmp, NULL));
		bufq_purge(&xp->sdl.tb);
		bufq_purge(&xp->sdt.tb);
		ss7_bufpool_release(&xp_bufpool, 2);
		bufq_purge(&xp->sl.rb);
		bufq_purge(&xp->sl.tb);
		bufq_purge(&xp->sl.rtb);
		if (xp->next || xp->prev != &xp->next) {
			if ((*xp->prev = xp->next))
				xp->next->prev = xp->prev;
			xp->next = NULL;
			xp->prev = &xp->next;
			xp_put(xp);
		}
		xp_put(xchg(&xp->oq->q_ptr, NULL));
		xp_put(xchg(&xp->iq->q_ptr, NULL));
	}
	spin_unlock(&xp->lock);
	assure(atomic_read(&xp->refcnt) == 1);
	xp_put(xp);		/* final put */
}

STATIC struct xp *
xp_get(struct xp *xp)
{
	if (xp)
		atomic_inc(&xp->refcnt);
	return (xp);
}

STATIC void
xp_put(struct xp *xp)
{
	if (atomic_dec_and_test(&xp->refcnt)) {
		kmem_cache_free(xp_priv_cachep, xp);
		printd(("%s: freed device private structure\n", DRV_NAME));
	}
}

/*
 *  Span allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC noinline __unlikely struct sp *
xp_alloc_sp(struct cd *cd, uint8_t span)
{
	struct sp *sp;

	if ((sp = kmem_cache_alloc(xp_span_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated span private structure\n", DRV_NAME));
		bzero(sp, sizeof(*sp));
		sp_get(sp);	/* first get */
		spin_lock_init(&sp->lock);	/* "sp-priv-lock" */
		/* create linkage */
		cd->spans[span] = sp_get(sp);
		sp->cd = cd_get(cd);
		/* fill out span structure */
		printd(("%s: linked span private structure\n", DRV_NAME));
		sp->iobase = cd->iobase + (span << 8);
		sp->span = span;
		sp->config = cd->config;
		sp->config.ifflags = 0;
		sp->config.ifalarms = 0;
		sp->config.ifrxlevel = 0;
		printd(("%s: set span private structure defaults\n", DRV_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate span private structure\n", DRV_NAME));
	return (sp);
}

/* Note: called with card interrupts disabled */
STATIC noinline __unlikely void
xp_free_sp(struct sp *sp)
{
	struct cd *cd;

	ensure(sp, return);
	if ((cd = sp->cd)) {
		int slot;

		/* remove card linkage */
		sp_put(xchg(&cd->spans[sp->span], NULL));
		cd_put(xchg(&sp->cd, NULL));
		sp->span = 0;
		printd(("%s: unlinked span private structure from card\n", DRV_NAME));
		/* remove channel linkage */
		for (slot = 0; slot < 32; slot++) {
			struct xp *xp;

			if ((xp = sp->slots[slot])) {
				sp_put(xchg(&xp->sp, NULL));
				xp_put(xchg(&sp->slots[slot], NULL));
			}
		}
		printd(("%s: unlinked span private structure from slots\n", DRV_NAME));
	} else
		ptrace(("%s: ERROR: spans cannot exist without cards\n", DRV_NAME));
	assure(atomic_read(&sp->refcnt) == 1);
	sp_put(sp);		/* final put */
}
STATIC struct sp *
sp_get(struct sp *sp)
{
	if (sp)
		atomic_inc(&sp->refcnt);
	return (sp);
}
STATIC void
sp_put(struct sp *sp)
{
	if (atomic_dec_and_test(&sp->refcnt)) {
		printd(("%s: freed span private structure\n", DRV_NAME));
		kmem_cache_free(xp_span_cachep, sp);
	}
}

/*
 *  Card allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC noinline __unlikely struct cd *
xp_alloc_cd(void)
{
	struct cd *cd;

	if ((cd = kmem_cache_alloc(xp_card_cachep, SLAB_ATOMIC))) {
		uint32_t *wbuf;
		uint32_t *rbuf;

		printd(("%s: allocated card private structure\n", DRV_NAME));
		if (!(wbuf = kmem_cache_alloc(xp_xbuf_cachep, SLAB_ATOMIC))) {
			ptrace(("%s: could not allocate write buffer\n", DRV_NAME));
			kmem_cache_free(xp_card_cachep, cd);
			return (NULL);
		}
		if (!(rbuf = kmem_cache_alloc(xp_xbuf_cachep, SLAB_ATOMIC))) {
			ptrace(("%s: could not allocate read buffer\n", DRV_NAME));
			kmem_cache_free(xp_xbuf_cachep, wbuf);
			kmem_cache_free(xp_card_cachep, cd);
			return (NULL);
		}
		bzero(cd, sizeof(*cd));
		cd_get(cd);	/* first get */
		spin_lock_init(&cd->lock);	/* "cd-priv-lock" */
		if ((cd->next = x400p_cards))
			cd->next->prev = &cd->next;
		cd->prev = &x400p_cards;
		cd->card = x400p_boards++;
		x400p_cards = cd_get(cd);
		cd->wbuf = wbuf;
		cd->rbuf = rbuf;
		tasklet_init(&cd->tasklet, NULL, 0);
		printd(("%s: linked card private structure\n", DRV_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate card private structure\n", DRV_NAME));
	return (cd);
}

/* Note: called with card interrupts disabled and pci resources deallocated */
STATIC noinline __unlikely void
xp_free_cd(struct cd *cd)
{
	psw_t flags;

	spin_lock_irqsave(&cd->lock, flags);
	{
		int span;

		ensure(cd, return);
		if (cd->tasklet.func)
			tasklet_kill(&cd->tasklet);
		if (cd->rbuf)
			kmem_cache_free(xp_xbuf_cachep, (uint32_t *) xchg(&cd->rbuf, NULL));
		if (cd->wbuf)
			kmem_cache_free(xp_xbuf_cachep, (uint32_t *) xchg(&cd->wbuf, NULL));
		/* remove any remaining spans */
		for (span = 0; span < X400_SPANS; span++) {
			struct sp *sp;

			if ((sp = cd->spans[span]))
				xp_free_sp(sp);
		}
		/* unlink from board list */
		if ((*(cd->prev) = cd->next))
			cd->next->prev = cd->prev;
		cd->next = NULL;
		cd->prev = &cd->next;
		cd_put(cd);
		printd(("%s: unlinked card private structure\n", DRV_NAME));
	}
	spin_unlock_irqrestore(&cd->lock, flags);
	assure(atomic_read(&cd->refcnt) == 1);
	cd_put(cd);		/* final put */
}
STATIC struct cd *
cd_get(struct cd *cd)
{
	if (cd)
		atomic_inc(&cd->refcnt);
	return (cd);
}
STATIC void
cd_put(struct cd *cd)
{
	if (atomic_dec_and_test(&cd->refcnt)) {
		kmem_cache_free(xp_card_cachep, cd);
		printd(("%s: freed card private structure\n", DRV_NAME));
	}
}

/*
 *  =========================================================================
 *
 *  PCI Initialization
 *
 *  =========================================================================
 */
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
	struct cd *cd;

	if (!(cd = pci_get_drvdata(dev)))
		goto disable;
	if (cd->plx) {
		cd->plx[INTCSR] = 0;	/* disable interrupts */
	}
	if (cd->xlb) {
		cd->xlb[SYNREG] = 0;
		cd->xlb[CTLREG] = 0;
		cd->xlb[LEDREG] = 0;
	}
	if (cd->irq) {
		free_irq(cd->irq, cd);
		printd(("%s: freed irq\n", DRV_NAME));
	}
	if (cd->xlb_region) {
		release_mem_region(cd->xlb_region, cd->xlb_length);
		printd(("%s: released xlb region %lx length %ld\n", DRV_NAME,
			cd->xlb_region, cd->xlb_length));
	}
	if (cd->xll_region) {
		release_mem_region(cd->xll_region, cd->xll_length);
		printd(("%s: released xll region %lx length %ld\n", DRV_NAME,
			cd->xll_region, cd->xll_length));
	}
	if (cd->plx_region) {
		release_mem_region(cd->plx_region, cd->plx_length);
		printd(("%s: released plx region %lx length %ld\n", DRV_NAME,
			cd->plx_region, cd->plx_length));
	}
	if (cd->xlb) {
		iounmap((void *) cd->xlb);
		printd(("%s: unmapped xlb memory at %p\n", DRV_NAME, cd->xlb));
	}
	if (cd->xll) {
		iounmap((void *) cd->xll);
		printd(("%s: unmapped xll memory at %p\n", DRV_NAME, cd->xll));
	}
	if (cd->plx) {
		iounmap((void *) cd->plx);
		printd(("%s: unmapped plx memory at %p\n", DRV_NAME, cd->plx));
	}
	xp_free_cd(cd);
      disable:
	pci_disable_device(dev);
}

#ifdef X400P_DOWNLOAD_FIRMWARE
STATIC int __devinit
xp_download_fw(struct cd *cd, enum xp_board board)
{
	unsigned int byte;
	unsigned char *f;
	size_t flen;
	volatile unsigned long *data;
	unsigned long timeout;

	switch (board) {
	case V400P:
	case X400P:
	case X400PSS7:
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

	data = (volatile unsigned long *) &cd->plx[GPIOC];
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
		*cd->xlb = *f++;
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
		cmn_err(CE_WARN, "%s: ERROR: Xilinx Firmware Load: Failed\n", DRV_NAME);
		return (-EIO);
	}
	if (!(*data & GPIO_DONE)) {
		cmn_err(CE_WARN, "%s: ERROR: Xilinx Firmware Load: Failed\n", DRV_NAME);
		return (-EIO);
	}
	cmn_err(CE_NOTE, "%s: Xilinx Firmware Load: Successful\n", DRV_NAME);
	return (0);
}
#endif

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
	int span, board;
	struct cd *cd;
	const char *name;
	unsigned long timeout;

	if (!dev || !id) {
		cmn_err(CE_WARN, "%s: ERROR: Device or id is null!\n", DRV_NAME);
		return (-ENXIO);
	}
	board = id->driver_data;
	name = xp_board_info[board].name;

	if (!(xp_board_info[board].hw_flags & XPF_SUPPORTED)) {
		cmn_err(CE_WARN, "%s: ERROR: Driver does not support %s card.\n", DRV_NAME, name);
		return (-ENXIO);
	}
	if (dev->irq < 1) {
		cmn_err(CE_WARN, "%s: ERROR: No IRQ allocated for %s card.\n", DRV_NAME, name);
		return (-ENXIO);
	}
	printd(("%s: card %s allocated IRQ %d\n", DRV_NAME, name, dev->irq));
	if (pci_enable_device(dev)) {
		cmn_err(CE_WARN, "%s: ERROR: Could not enable %s pci card\n", DRV_NAME, name);
		return (-ENODEV);
	}
	printd(("%s: enabled %s pci card type %ld\n", DRV_NAME, name, id->driver_data));
	if (!(cd = xp_alloc_cd()))
		return (-ENOMEM);
	pci_set_drvdata(dev, cd);
	if ((pci_resource_flags(dev, 0) & IORESOURCE_IO)
	    || !(cd->plx_region = pci_resource_start(dev, 0))
	    || !(cd->plx_length = pci_resource_len(dev, 0))
	    || !(cd->plx = ioremap(cd->plx_region, cd->plx_length))) {
		cmn_err(CE_WARN, "%s: ERROR: Invalid PLX 9030 base resource\n", DRV_NAME);
		goto error_remove;
	}
	printd(("%s: plx region %ld bytes at %lx, remapped %p\n", DRV_NAME, cd->plx_length,
		cd->plx_region, cd->plx));
	if ((pci_resource_flags(dev, 2) & IORESOURCE_IO)
	    || !(cd->xll_region = pci_resource_start(dev, 2))
	    || !(cd->xll_length = pci_resource_len(dev, 2))
	    || !(cd->xll = ioremap(cd->xll_region, cd->xll_length))) {
		cmn_err(CE_WARN, "%s: ERROR: Invalid Xilinx 32-bit base resource\n", DRV_NAME);
		goto error_remove;
	}
	printd(("%s: xll region %ld bytes at %lx, remapped %p\n", DRV_NAME, cd->xll_length,
		cd->xll_region, cd->xll));
	if ((pci_resource_flags(dev, 3) & IORESOURCE_IO)
	    || !(cd->xlb_region = pci_resource_start(dev, 3))
	    || !(cd->xlb_length = pci_resource_len(dev, 3))
	    || !(cd->xlb = ioremap(cd->xlb_region, cd->xlb_length))) {
		cmn_err(CE_WARN, "%s: ERROR: Invalid Xilinx 8-bit base resource\n", DRV_NAME);
		goto error_remove;
	}
	printd(("%s: xlb region %ld bytes at %lx, remapped %p\n", DRV_NAME, cd->xlb_length,
		cd->xlb_region, cd->xlb));
	cd->config.ifname = xp_board_info[id->driver_data].name;
	if (!request_mem_region(cd->plx_region, cd->plx_length, cd->config.ifname)) {
		cmn_err(CE_WARN, "%s: ERROR: Unable to reserve PLX memory\n", DRV_NAME);
		goto error_remove;
	}
	printd(("%s: plx region %lx reserved %ld bytes\n", DRV_NAME, cd->plx_region,
		cd->plx_length));
	if (!request_mem_region(cd->xll_region, cd->xll_length, cd->config.ifname)) {
		cmn_err(CE_WARN, "%s: ERROR: Unable to reserve Xilinx 32-bit memory\n", DRV_NAME);
		goto error_remove;
	}
	printd(("%s: xll region %lx reserved %ld bytes\n", DRV_NAME, cd->xll_region,
		cd->xll_length));
	if (!request_mem_region(cd->xlb_region, cd->xlb_length, cd->config.ifname)) {
		cmn_err(CE_WARN, "%s: ERROR: Unable to reserve Xilinx 8-bit memory\n", DRV_NAME);
		goto error_remove;
	}
	printd(("%s: xlb region %lx reserved %ld bytes\n", DRV_NAME, cd->xlb_region,
		cd->xlb_length));
	cmn_err(CE_NOTE, "%s: card detected %s at 0x%lx/0x%lx irq %d\n", DRV_NAME,
		  cd->config.ifname, cd->xll_region, cd->xlb_region, dev->irq);
#ifdef X400P_DOWNLOAD_FIRMWARE
	if (xp_download_fw(cd, board) != 0)
		goto error_remove;
#endif
	cd->plx[INTCSR] = 0;	/* disable interrupts */

	/* all boards have these registers regardless of framer */

	cd->xlb[SYNREG] = 0;	/* default autosync */
	cd->xlb[CTLREG] = 0;	/* interrupts disabled */
	cd->xlb[LEDREG] = 0xff;	/* turn off leds */

	/* Note: only check the device id of the first framer of 4. */

	cd->devrev = cd->xlb[0x0f];	/* Device id - same place for all Dallas chips */
	cd->device = (cd->devrev & XP_DEV_IDMASK) >> XP_DEV_SHIFT;
	cd->devrev &= XP_DEV_REVMASK;

	if (!(xp_device_info[cd->device].hw_flags & XPF_SUPPORTED)) {
		cmn_err(CE_WARN, "%s: Usupported framer device %s\n", DRV_NAME,
			xp_device_info[cd->device].name);
		goto error_remove;
	}
	switch (cd->device) {
	case XP_DEV_DS2154:
	case XP_DEV_DS21354:
	case XP_DEV_DS21554:
		switch (board) {
		case V400P:
			cd->board = V400PE;
			break;
		case X400P:
			cd->board = E400P;
			break;
		case X400PSS7:
			cd->board = E400PSS7;
			break;
		default:
			cd->board = -1;
			break;
		}
	case XP_DEV_DS2152:
	case XP_DEV_DS21352:
	case XP_DEV_DS21552:
		switch (board) {
		case V400P:
			cd->board = V400PT;
			break;
		case X400P:
			cd->board = T400P;
			break;
		case X400PSS7:
			cd->board = T400PSS7;
			break;
		default:
			cd->board = -1;
			break;
		}
	case XP_DEV_DS2155E:
	case XP_DEV_DS2155T:
		switch (board) {
		case V401PE:
			cd->board = V401PE;
			break;
		case V401PT:
			cd->board = V401PT;
			break;
		default:
			cd->board = -1;
			break;
		}
	}
	if (cd->board == -1) {
		cmn_err(CE_WARN, "%s: Device %s not supported for card %s\n", DRV_NAME,
			xp_device_info[cd->device].name, xp_board_info[board].name);
		goto error_remove;
	}
	cd->hw_flags = xp_board_info[cd->board].hw_flags;
	cd->hw_flags |= xp_device_info[cd->device].hw_flags;

	cmn_err(CE_NOTE, "%s: %s (%s Rev. %c)\n", DRV_NAME,
		  xp_board_info[cd->board].name,
		  xp_device_info[cd->device].name, (char) (cd->devrev + 65));

	{
		int word, idle_word = xp_board_info[cd->board].idle_word;

		/* idle out all channels */
		for (word = 0; word < 256; word++) {
			int ebuf;

			cd->xll[word] = idle_word;
			for (ebuf = 0; ebuf < X400P_EBUFNO; ebuf++)
				cd->wbuf[(ebuf << 8) + word] = idle_word;
		}
	}

	switch (cd->board) {
	case V400PE:
	case E400P:
	case E400PSS7:
	{
		int span, offset;

		/* setup E1 card defaults */
		cd->config = sdl_default_e1_chan;
		cd->isr = &xp_e400_interrupt;
		/* zero all span registers */
		for (span = 0; span < X400_SPANS; span++)
			for (offset = 0; offset < 192; offset++)
				cd->xlb[(span << 8) + offset] = 0x00;
		/* set up for interleaved serial bus operation, byte mode */
		cd->xlb[(0 << 8) + 0xb5] = 0x09;
		cd->xlb[(1 << 8) + 0xb5] = 0x08;
		cd->xlb[(2 << 8) + 0xb5] = 0x08;
		cd->xlb[(3 << 8) + 0xb5] = 0x08;
		break;
	}
	case V401PE:
	{
		/* setup E1 card defaults */
		cd->config = sdl_default_e1_chan;
		cd->isr = &xp_e401_interrupt;
		/* place all framers in E1 mode */
		cd->xlb[(0 << 8) + 0x00] = 0x02;
		cd->xlb[(1 << 8) + 0x00] = 0x02;
		cd->xlb[(2 << 8) + 0x00] = 0x02;
		cd->xlb[(3 << 8) + 0x00] = 0x02;
		/* set up for interleaved serial bus operation, byte mode */
		cd->xlb[(0 << 8) + 0xc5] = 0x28 + 0;
		cd->xlb[(1 << 8) + 0xc5] = 0x28 + 1;
		cd->xlb[(2 << 8) + 0xc5] = 0x28 + 2;
		cd->xlb[(3 << 8) + 0xc5] = 0x28 + 3;
		/* link interface reset */
		cd->xlb[(0 << 8) + 0x79] = 0xd8;	/* E1, normal, LIRST */
		printd(("%s: LIRST on span 0\n", DRV_NAME));
		cd->xlb[(1 << 8) + 0x79] = 0xd8;	/* E1, normal, LIRST */
		printd(("%s: LIRST on span 1\n", DRV_NAME));
		cd->xlb[(2 << 8) + 0x79] = 0xd8;	/* E1, normal, LIRST */
		printd(("%s: LIRST on span 2\n", DRV_NAME));
		cd->xlb[(3 << 8) + 0x79] = 0xd8;	/* E1, normal, LIRST */
		printd(("%s: LIRST on span 3\n", DRV_NAME));
		/* wait for 40 ms */
		timeout = jiffies + 100 * HZ / 1000;
		while (jiffies < timeout) ;
		/* release LIRST bit */
		cd->xlb[(0 << 8) + 0x79] = 0x98;	/* E1, normal */
		cd->xlb[(1 << 8) + 0x79] = 0x98;	/* E1, normal */
		cd->xlb[(2 << 8) + 0x79] = 0x98;	/* E1, normal */
		cd->xlb[(3 << 8) + 0x79] = 0x98;	/* E1, normal */
		break;
	}
	case V400PT:
	case T400P:
	case T400PSS7:
	{
		int span, offset;

#if 0
		if (!japan) {
			/* setup T1 card defaults */
			cd->config = sdl_default_t1_chan;
			cd->isr = &xp_t400_interrupt;
		} else {
			/* setup T1 card defaults */
			cd->config = sdl_default_t1_chan;
			cd->isr = &xp_j400_interrupt;
		}
#else
		/* setup T1 card defaults */
		cd->config = sdl_default_t1_chan;
		cd->isr = &xp_t400_interrupt;
#endif
		/* zero all span registers */
		for (span = 0; span < X400_SPANS; span++)
			for (offset = 0; offset < 160; offset++)
				cd->xlb[(span << 8) + offset] = 0x00;
		/* set up for interleaved serial bus operation, byte mode */
		cd->xlb[(0 << 8) + 0x94] = 0x09;
		cd->xlb[(1 << 8) + 0x94] = 0x08;
		cd->xlb[(2 << 8) + 0x94] = 0x08;
		cd->xlb[(3 << 8) + 0x94] = 0x08;
		break;
	}
	case V401PT:
	{
#if 0
		if (!japan) {
			/* setup T1 card defaults */
			cd->config = sdl_default_t1_chan;
			cd->isr = &xp_t401_interrupt;
		} else {
			/* setup J1 card defaults */
			cd->config = sdl_default_j1_chan;
			cd->isr = &xp_j401_interrupt;
		}
#else
		(void) sdl_default_j1_chan;
		/* setup T1 card defaults */
		cd->config = sdl_default_t1_chan;
		cd->isr = &xp_t401_interrupt;
#endif
		cd->xlb[SYNREG] = SYNC1;	/* default span1 */
		/* place all framers in T1 mode */
		cd->xlb[(0 << 8) + 0x00] = 0x00;
		cd->xlb[(1 << 8) + 0x00] = 0x00;
		cd->xlb[(2 << 8) + 0x00] = 0x00;
		cd->xlb[(3 << 8) + 0x00] = 0x00;
		/* set up for interleaved serial bus operation, byte mode */
		cd->xlb[(0 << 8) + 0xc5] = 0x28 + 0;
		cd->xlb[(1 << 8) + 0xc5] = 0x28 + 1;
		cd->xlb[(2 << 8) + 0xc5] = 0x28 + 2;
		cd->xlb[(3 << 8) + 0xc5] = 0x28 + 3;
		/* link interface reset */
		cd->xlb[(0 << 8) + 0x79] = 0x58;	/* T1, normal, LIRST */
		printd(("%s: LIRST on span 0\n", DRV_NAME));
		cd->xlb[(1 << 8) + 0x79] = 0x58;	/* T1, normal, LIRST */
		printd(("%s: LIRST on span 1\n", DRV_NAME));
		cd->xlb[(2 << 8) + 0x79] = 0x58;	/* T1, normal, LIRST */
		printd(("%s: LIRST on span 2\n", DRV_NAME));
		cd->xlb[(3 << 8) + 0x79] = 0x58;	/* T1, normal, LIRST */
		printd(("%s: LIRST on span 3\n", DRV_NAME));
		/* wait for 40 ms */
		timeout = jiffies + 100 * HZ / 1000;
		while (jiffies < timeout) ;
		/* release LIRST bit */
		cd->xlb[(0 << 8) + 0x79] = 0x18;	/* T1, normal */
		cd->xlb[(1 << 8) + 0x79] = 0x18;	/* T1, normal */
		cd->xlb[(2 << 8) + 0x79] = 0x18;	/* T1, normal */
		cd->xlb[(3 << 8) + 0x79] = 0x18;	/* T1, normal */
		break;
	}
	default:
		swerr();
		goto error_remove;
	}
	if (request_irq(dev->irq, cd->isr, SA_INTERRUPT | SA_SHIRQ, DRV_NAME, cd)) {
		cmn_err(CE_WARN, "%s: ERROR: Unable to request IRQ %d\n", DRV_NAME, dev->irq);
		goto error_remove;
	}
	cd->irq = dev->irq;
	printd(("%s: acquired IRQ %ld for %s card\n", DRV_NAME, cd->irq,
		xp_board_info[cd->board].name));

	cd->config.ifflags = (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);

	switch (cd->config.ifgtype) {
	case SDL_GTYPE_E1:
		tasklet_init(&cd->tasklet, &xp_e1_card_tasklet, (unsigned long) cd);
		break;
	case SDL_GTYPE_T1:
		tasklet_init(&cd->tasklet, &xp_t1_card_tasklet, (unsigned long) cd);
		break;
	case SDL_GTYPE_J1:
		tasklet_init(&cd->tasklet, &xp_j1_card_tasklet, (unsigned long) cd);
		break;
	}
	/* allocate span structures */
	for (span = 0; span < X400_SPANS; span++)
		if (!xp_alloc_sp(cd, span))
			goto error_remove;
	/* set up span defaults */
	xp_span_config(cd, 0, true);
	xp_span_config(cd, 1, true);
	xp_span_config(cd, 2, true);
	xp_span_config(cd, 3, true);

	cd->plx[INTCSR] = PLX_INTENA;	/* enable interrupts */
	return (0);
      error_remove:
	xp_remove(dev);
	return (-ENODEV);
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
STATIC noinline __devinit int
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
STATIC noinline __devexit int
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
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SL-X400P driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the SL-X400P driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw xp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sl_x400pinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC noinline __devinit int
xp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&xp_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC noinline __devexit int
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

STATIC noinline __devinit int
xp_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &sl_x400pinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC noinline __devexit int
xp_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
sl_x400pterminate(void)
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
	ss7_bufpool_term(&xp_bufpool);
	if ((err = xp_free_tables()))
		cmn_err(CE_WARN, "%s: could not free tables", DRV_NAME);
	if ((err = xp_pci_cleanup()))
		cmn_err(CE_WARN, "%s: could not cleanup pci device", DRV_NAME);
	if ((err = xp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
sl_x400pinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = xp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		sl_x400pterminate();
		return (err);
	}
	if ((err = xp_pci_init())) {
		cmn_err(CE_WARN, "%s: no PCI devices found, err = %d", DRV_NAME, err);
		sl_x400pterminate();
		return (err);
	}
	if ((err = xp_init_tables())) {
		cmn_err(CE_WARN, "%s: could not allocate tables, err = %d", DRV_NAME, err);
		sl_x400pterminate();
		return (err);
	}
	ss7_bufpool_init(&xp_bufpool);
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = xp_register_strdev(xp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					xp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				sl_x400pterminate();
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
module_init(sl_x400pinit);
module_exit(sl_x400pterminate);

#endif				/* LINUX */
