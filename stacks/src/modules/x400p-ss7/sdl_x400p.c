/*****************************************************************************

 @(#) $RCSfile: sdl_x400p.c,v $ $Name:  $($Revision: 0.9.2.18 $) $Date: 2007/03/25 02:23:09 $

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

 Last Modified $Date: 2007/03/25 02:23:09 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdl_x400p.c,v $
 Revision 0.9.2.18  2007/03/25 02:23:09  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.17  2007/03/25 00:52:19  brian
 - synchronization updates

 Revision 0.9.2.16  2006/05/08 11:01:16  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.15  2006/03/07 01:14:40  brian
 - binary compatible callouts

 Revision 0.9.2.14  2006/03/04 13:00:22  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: sdl_x400p.c,v $ $Name:  $($Revision: 0.9.2.18 $) $Date: 2007/03/25 02:23:09 $"

static char const ident[] =
    "$RCSfile: sdl_x400p.c,v $ $Name:  $($Revision: 0.9.2.18 $) $Date: 2007/03/25 02:23:09 $";

/*
 *  This is an SDL (Signalling Data Link) kernel module which provides all of
 *  the capabilities of the SDLI for the E400P-SS7 and T400P-SS7 cards.  The
 *  SDT and SL modules can be pushed over streams opened on this driver to
 *  implement a complete SS7 MTP Level 2 OpenSS7 implementation.
 */

#define _DEBUG 1
#define X400P_DOWNLOAD_FIRMWARE

#include <sys/os7/compat.h>

#ifdef LINUX
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <linux/pci.h>
#endif				/* LINUX */

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

#ifdef X400P_DOWNLOAD_FIRMWARE
#include "x400p-ss7/x400pfw.h"	/* X400P-SS7 firmware load */
#endif

#define SDL_X400P_DESCRIP	"E/T400P-SS7: SS7/SDL (Signalling Data Link) STREAMS DRIVER."
#define SDL_X400P_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SDL_X400P_REVISION	"OpenSS7 $RCSfile: sdl_x400p.c,v $ $Name:  $ ($Revision: 0.9.2.18 $) $Date: 2007/03/25 02:23:09 $"
#define SDL_X400P_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SDL_X400P_DEVICE	"Supports the T/E400P-SS7 T1/E1 PCI boards."
#define SDL_X400P_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDL_X400P_LICENSE	"GPL"
#define SDL_X400P_BANNER	SDL_X400P_DESCRIP	"\n" \
				SDL_X400P_EXTRA		"\n" \
				SDL_X400P_REVISION	"\n" \
				SDL_X400P_COPYRIGHT	"\n" \
				SDL_X400P_DEVICE	"\n" \
				SDL_X400P_CONTACT
#define SDL_X400P_SPLASH	SDL_X400P_DESCRIP	"\n" \
				SDL_X400P_REVISION

#ifdef LINUX
MODULE_AUTHOR(SDL_X400P_CONTACT);
MODULE_DESCRIPTION(SDL_X400P_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDL_X400P_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDL_X400P_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdl_x400p");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SDL_X400P_DRV_ID	CONFIG_STREAMS_SDL_X400P_MODID
#define SDL_X400P_DRV_NAME	CONFIG_STREAMS_SDL_X400P_NAME
#define SDL_X400P_CMAJORS	CONFIG_STREAMS_SDL_X400P_NMAJORS
#define SDL_X400P_CMAJOR_0	CONFIG_STREAMS_SDL_X400P_MAJOR
#define SDL_X400P_UNITS		CONFIG_STREAMS_SDL_X400P_NMINORS
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define DRV_ID		SDL_X400P_DRV_ID
#define DRV_NAME	SDL_X400P_DRV_NAME
#define CMAJORS		SDL_X400P_CMAJORS
#define CMAJOR_0	SDL_X400P_CMAJOR_0
#define UNITS		SDL_X400P_UNITS
#ifdef MODULE
#define DRV_BANNER	SDL_X400P_BANNER
#else				/* MODULE */
#define DRV_BANNER	SDL_X400P_SPLASH
#endif				/* MODULE */

STATIC struct module_info xp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 280,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
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

MODULE_STATIC struct streamtab sdl_x400pinfo = {
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
struct cd;
struct sp;

typedef struct xp {
	STR_DECLARATION (struct xp);	/* stream declaration */
	struct cd *cd;			/* card for this channel */
	struct sp *sp;			/* span for this channel */
	int card;			/* index (card) */
	int span;			/* index (span) */
	int chan;			/* index (chan) */
	int slot;			/* index (slot) */
	uchar *tx_base;			/* chan tx base */
	uchar *rx_base;			/* chan rx base */
	volatile mblk_t *tx_msg;	/* chan tx message */
	lmi_option_t option;		/* LMI protocol and variant options */
	volatile sdl_config_t config;	/* SDT configuration */
	sdl_statem_t statem;		/* SDT state machine variables */
	sdl_notify_t notify;		/* SDT notification options */
	sdl_stats_t stats;		/* SDT statistics */
	sdl_stats_t stamp;		/* SDT statistics timestamps */
	sdl_stats_t statsp;		/* SDT statistics periods */
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
	int card;			/* index (card) */
	int span;			/* index (span) */
	volatile sdl_config_t config;	/* span configuration */
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
	volatile uint32_t *wbuf;	/* wr buffer */
	volatile uint32_t *rbuf;	/* rd buffer */
	volatile int eval_syncsrc;	/* need to reevaluate sync src */
	volatile int leds;		/* leds on the card */
	int card;			/* index (card) */
	ulong irq;			/* card irq */
	ulong iobase;			/* card iobase */
	struct tasklet_struct tasklet;	/* card tasklet */
	volatile sdl_config_t config;	/* card configuration */
} cd_t;

STATIC struct cd *xp_alloc_cd(void);
STATIC void xp_free_cd(struct cd *);
STATIC struct cd *cd_get(struct cd *);
STATIC void cd_put(struct cd *);

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

#define X400P_ALARM_SETTLE_TIME	    5000	/* allow alarms to settle for 5 seconds */

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
	{PCI_VENDOR_ID_PLX, 0x0557, PCI_ANY_ID, PCI_ANY_ID, 0, 0, X400PSS7},
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
STATIC int
m_error(queue_t *q, struct xp *xp, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		ss7_oput(xp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_RECEIVED_BITS_IND
 *  -----------------------------------
 *  Actually, we never want to use this function: we want to just send M_DATA.
 */
STATIC INLINE int
sdl_received_bits_ind(queue_t *q, struct xp *xp, mblk_t *dp)
{
	if (canputnext(xp->oq)) {
		mblk_t *mp;
		sdl_received_bits_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sdl_primitive = SDL_RECEIVED_BITS_IND;
			mp->b_cont = dp;
			ss7_oput(xp->oq, mp);
			return (QR_ABSORBED);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  SDL_DISCONNECT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdl_disconnect_ind(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	sdl_disconnect_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_DISCONNECT_IND;
		ss7_oput(xp->oq, mp);
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
lmi_info_ack(queue_t *q, struct xp *xp, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = LMI_UNATTACHED;
		p->lmi_max_sdu = 8;
		p->lmi_min_sdu = 8;
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		ss7_oput(xp->oq, mp);
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
lmi_ok_ack(queue_t *q, struct xp *xp, ulong state, long prim)
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
		ss7_oput(xp->oq, mp);
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
lmi_error_ack(queue_t *q, struct xp *xp, ulong state, long prim, ulong errno, ulong reason)
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
		ss7_oput(xp->oq, mp);
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
lmi_enable_con(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = xp->i_state = LMI_ENABLED;
		ss7_oput(xp->oq, mp);
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
lmi_disable_con(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		struct xp *xp = XP_PRIV(q);
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = xp->i_state = LMI_DISABLED;
		ss7_oput(xp->oq, mp);
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
lmi_optmgmt_ack(queue_t *q, struct xp *xp, ulong flags, caddr_t opt_ptr, size_t opt_len)
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
		ss7_oput(xp->oq, mp);
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
lmi_error_ind(queue_t *q, struct xp *xp, ulong errno, ulong reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = xp->i_state;
		ss7_oput(xp->oq, mp);
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
lmi_stats_ind(queue_t *q, struct xp *xp, ulong interval)
{
	if (canputnext(xp->oq)) {
		mblk_t *mp;
		lmi_stats_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = jiffies;
			ss7_oput(xp->oq, mp);
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
lmi_event_ind(queue_t *q, struct xp *xp, ulong oid, ulong level)
{
	if (canputnext(xp->oq)) {
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
			ss7_oput(xp->oq, mp);
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
 *  EVENTS From Above
 *
 *  =========================================================================
 */

/*
 *  SDL_BITS_FOR_TRANSMISSION_REQ
 *  -----------------------------------
 *  It is preffered to just send M_DATA here.  We just strip the proto block
 *  off and put it back on the queue.
 */
STATIC int
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
STATIC int
sdl_connect_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	if (xp->i_state != LMI_ENABLED)
		return m_error(q, xp, EPROTO);
	xp->config.ifflags |= (SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
	return (QR_DONE);
}

/*
 *  SDL_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC int
sdl_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	if (xp->i_state != LMI_ENABLED)
		return m_error(q, xp, EPROTO);
	xp->config.ifflags &= ~(SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
	return (QR_DONE);
}

/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	uint16_t ppa = (xp->chan & 0xff) | ((xp->span & 0x0f) << 8) | ((xp->card & 0x0f) << 12);
	return lmi_info_ack(q, xp, (caddr_t) &ppa, sizeof(ppa));
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	int err, card, span, chan, slot, byte, span_allocated = 0;
	struct cd *cd;
	struct sp *sp = NULL;
	uint16_t ppa;
	struct xp *xp = XP_PRIV(q);
	lmi_attach_req_t *p = ((typeof(p)) mp->b_rptr);
	if (mp->b_wptr - mp->b_rptr < sizeof(*p) + sizeof(ppa)) {
		ptrace(("X400P-SS7: ERROR: primitive too small = %d bytes\n",
			mp->b_wptr - mp->b_rptr));
		goto lmi_badprim;
	}
	if (xp->i_state != LMI_UNATTACHED) {
		ptrace(("X400P-SS7: ERROR: interface out of state\n"));
		goto lmi_outstate;
	}
	xp->i_state = LMI_ATTACH_PENDING;
	ppa = *(typeof(ppa) *) (p + 1);
	/* check card */
	card = (ppa >> 12) & 0x0f;
	for (cd = x400p_cards; cd && cd->card != card; cd = cd->next) ;
	if (!cd) {
		ptrace(("X400P-SS7: ERROR: invalid card %d\n", card));
		goto lmi_badppa;
	}
	/* check span */
	span = (ppa >> 8) & 0x0f;
	if (span < 0 || span > 3) {
		ptrace(("X400P-SS7: ERROR: invalid span %d\n", span));
		goto lmi_badppa;
	} else {
		/* allocate span if required */
		if (!(sp = cd->spans[span])) {
			printd(("X400P-SS7: allocating span %d to card %d\n", span, card));
			if (!(sp = xp_alloc_sp(cd, span))) {
				ptrace(("X400P-SS7: ERROR: can't allocate span\n"));
				goto enomem;
			}
			span_allocated = 1;
		}
	}
#ifdef __LITTLE_ENDIAN
	byte = 3 - span;
#else
#ifdef __BIG_ENDIAN
	byte = span;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined\n"
#endif
#endif
	if (sp->config.ifgtype != SDL_GTYPE_E1 && sp->config.ifgtype != SDL_GTYPE_T1) {
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
				ptrace(("E400P-SS7: ERROR: invalid chan %d\n", chan));
				goto lmi_badppa;
			}
			slot = xp_e1_chan_map[chan];
			if (sp->slots[slot]) {
				ptrace(("E400P-SS7: ERROR: slot %d in use\n", slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(q, xp, LMI_DISABLED, LMI_ATTACH_REQ)))
				goto error;
			/* commit attach */
			printd(("E400P-SS7: attaching card %d, span %d, chan %d, slot %d\n", card,
				span, chan, slot));
			sp->slots[slot] = xp_get(xp);
			xp->slot = slot;
			xp->tx_base = ((uchar *) cd->wbuf) + (slot << 2) + byte;
			xp->rx_base = ((uchar *) cd->rbuf) + (slot << 2) + byte;
			/* set E1 related channel defaults */
			xp->config.iftype = SDL_TYPE_DS0;
			break;
		case SDL_GTYPE_T1:
			if (chan < 1 || chan > 24) {
				ptrace(("T400P-SS7: ERROR: invalid chan %d\n", chan));
				goto lmi_badppa;
			}
			slot = xp_t1_chan_map[chan];
			if (sp->slots[slot]) {
				ptrace(("T400P-SS7: ERROR: slot %d in use\n", slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(q, xp, LMI_DISABLED, LMI_ATTACH_REQ)))
				goto error;
			/* commit attach */
			printd(("T400P-SS7: attaching card %d, span %d, chan %d, slot %d\n", card,
				span, chan, slot));
			sp->slots[slot] = xp_get(xp);
			xp->slot = slot;
			xp->tx_base = ((uchar *) cd->wbuf) + (slot << 2) + byte;
			xp->rx_base = ((uchar *) cd->rbuf) + (slot << 2) + byte;
			/* set T1 related channel defaults */
			xp->config.iftype = SDL_TYPE_DS0;
			break;
		}
	} else {
		int c;
		/* entire span indicated */
		switch (cd->config.ifgtype) {
		case SDL_GTYPE_E1:
			for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]); c++)
				if (sp->slots[xp_e1_chan_map[c]]) {
					ptrace(("E400P-SS7: ERROR: slot in use for chan %d\n", c));
					goto lmi_badppa;
				}
			xp->slot = 0;
			if ((err = lmi_ok_ack(q, xp, LMI_DISABLED, LMI_ATTACH_REQ)))
				goto error;
			/* commit attach */
			printd(("E400P-SS7: attaching card %d, entire span %d\n", card, span));
			for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]); c++) {
				slot = xp_e1_chan_map[c];
				sp->slots[slot] = xp_get(xp);
			}
			xp->tx_base = ((uchar *) cd->wbuf) + (xp_e1_chan_map[0] << 2) + byte;
			xp->rx_base = ((uchar *) cd->rbuf) + (xp_e1_chan_map[0] << 2) + byte;
			xp->config.iftype = SDL_TYPE_E1;
			break;
		case SDL_GTYPE_T1:
			for (c = 0; c < sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0]); c++)
				if (sp->slots[xp_t1_chan_map[c]]) {
					ptrace(("T400P-SS7: ERROR: slot in use for chan %d\n", c));
					goto lmi_badppa;
				}
			xp->slot = 0;
			if ((err = lmi_ok_ack(q, xp, LMI_DISABLED, LMI_ATTACH_REQ)))
				goto error;
			/* commit attach */
			printd(("T400P-SS7: attaching card %d, entire span %d\n", card, span));
			for (c = 0; c < sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0]); c++) {
				slot = xp_t1_chan_map[c];
				sp->slots[slot] = xp_get(xp);
			}
			xp->tx_base = ((uchar *) cd->wbuf) + (xp_t1_chan_map[0] << 2) + byte;
			xp->rx_base = ((uchar *) cd->rbuf) + (xp_t1_chan_map[0] << 2) + byte;
			xp->config.iftype = SDL_TYPE_T1;
			break;
		default:
			swerr();
			goto efault;
		}
	}
	xp->cd = cd_get(cd);
	cd->spans[span] = sp_get(sp);
	xp->sp = sp_get(sp);
	xp->i_state = LMI_DISABLED;
	xp->card = card;
	xp->span = span;
	xp->chan = chan;
	return (QR_DONE);
	{
		int errno, reason;
	      enomem:
		errno = ENOMEM;
		reason = LMI_SYSERR;
		goto error_out;
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
		if (span_allocated)
			xp_free_sp(sp);
		return lmi_error_ack(q, xp, LMI_UNATTACHED, LMI_ATTACH_REQ, errno, reason);
	}
      error:
	if (span_allocated)
		xp_free_sp(sp);
	return (err);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	/* validate detach */
	if (xp->i_state != LMI_DISABLED)
		return lmi_error_ack(q, xp, xp->i_state, LMI_DETACH_REQ, 0, LMI_OUTSTATE);
	xp->i_state = LMI_DETACH_PENDING;
	if ((err = lmi_ok_ack(q, xp, LMI_UNATTACHED, LMI_DETACH_REQ)))
		return (err);
	/* commit detach */
	if (xp->slot) {
		/* detaching from specific channel */
		switch (xp->sp->config.ifgtype) {
		case SDL_GTYPE_E1:
			xp_put(xchg(&xp->sp->slots[xp->slot], NULL));
			break;
		case SDL_GTYPE_T1:
			xp_put(xchg(&xp->sp->slots[xp->slot], NULL));
			break;
		}
	} else {
		int c;
		/* detaching from entire span */
		switch (xp->sp->config.ifgtype) {
		case SDL_GTYPE_E1:
			for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]); c++) {
				xp_put(xchg(&xp->sp->slots[xp_e1_chan_map[c]], NULL));
			}
			break;
		case SDL_GTYPE_T1:
			for (c = 0; c < sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0]); c++) {
				xp_put(xchg(&xp->sp->slots[xp_t1_chan_map[c]], NULL));
			}
			break;
		}
	}
	{
		// struct sp *sp = xp->sp;
		xp->tx_base = NULL;
		xp->rx_base = NULL;
		sp_put(xchg(&xp->sp, NULL));
		cd_put(xchg(&xp->cd, NULL));
		xp->card = 0;
		xp->span = 0;
		xp->chan = 0;
		xp->slot = 0;
		/* only referred to by card */
		// if (sp->refcnt == 1)
		// xp_free_sp(sp);
	}
	return (QR_DONE);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	int err, offset, slot = 0;
	struct xp *xp = XP_PRIV(q);
	struct cd *cd;
	struct sp *sp;
	/* validate enable */
	if (xp->i_state != LMI_DISABLED) {
		ptrace(("X400P-SS7: ERROR: out of state: state = %ld\n", xp->i_state));
		goto lmi_outstate;
	}
	if (!(cd = xp->cd)) {
		ptrace(("X400P-SS7: ERROR: out of state: no card pointer\n"));
		goto lmi_outstate;
	}
	if (!(sp = xp->sp)) {
		ptrace(("X400P-SS7: ERROR: out of state: no span pointer\n"));
		goto lmi_outstate;
	}
	if (xp->config.ifflags & SDL_IF_UP) {
		ptrace(("X400P-SS7: ERROR: out of state: device already up\n"));
		goto lmi_outstate;
	}
	if ((err = lmi_enable_con(q, xp)))
		return (err);
	/* commit enable */
	printd(("X400P-SS7: performing enable\n"));
	xp->i_state = LMI_ENABLE_PENDING;
	if (!(sp->config.ifflags & SDL_IF_UP)) {
		/* need to bring up span */
		int span = sp->span;
		int base = span << 8;
		uint8_t ccr1 = 0, tcr1 = 0;
		unsigned long timeout;
		switch (cd->config.ifgtype) {
		case SDL_GTYPE_E1:
		{
			printd(("E400P-SS7: performing enable on E1 span %d\n", span));
			spin_lock_irq(&cd->lock);
			// cd->xlb[SYNREG] = SYNCSELF; /* NO, NO, NO */
			/* Tell ISR to re-evaluate the sync source */
			cd->eval_syncsrc = 1;
			cd->xlb[CTLREG] = (E1DIV);
			cd->xlb[LEDREG] = 0xff;
			/* zero all span registers */
			for (offset = 0; offset < 192; offset++)
				cd->xlb[base + offset] = 0x00;
			/* Set up for interleaved serial bus operation, byte mode */
			if (span == 0)
				cd->xlb[base + 0xb5] = 0x09;
			else
				cd->xlb[base + 0xb5] = 0x08;
			cd->xlb[base + 0x1a] = 0x04;	/* CCR2: set LOTCMC */
			for (offset = 0; offset <= 8; offset++)
				cd->xlb[base + offset] = 0x00;
			for (offset = 0x10; offset <= 0x4f; offset++)
				if (offset != 0x1a)
					cd->xlb[base + offset] = 0x00;
			cd->xlb[base + 0x10] = 0x20;	/* RCR1: Rsync as input */
			cd->xlb[base + 0x11] = 0x06;	/* RCR2: Sysclk = 2.048 Mhz */
			cd->xlb[base + 0x12] = 0x09;	/* TCR1: TSiS mode */
			tcr1 = 0x09;	/* TCR1: TSiS mode */
			switch (sp->config.ifframing) {
			default:
			case SDL_FRAMING_CCS:
				ccr1 |= 0x08;
				break;
			case SDL_FRAMING_CAS:	/* does this mean DS0A? */
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
			cd->xlb[base + 0x12] = tcr1;
			cd->xlb[base + 0x14] = ccr1;
			cd->xlb[base + 0x18] = 0x20;	/* 120 Ohm, Normal */
			cd->xlb[base + 0x1b] = 0x8a;	/* CRC3: LIRST & TSCLKM */
			cd->xlb[base + 0x20] = 0x1b;	/* TAFR */
			cd->xlb[base + 0x21] = 0x5f;	/* TNAFR */
			cd->xlb[base + 0x40] = 0x0b;	/* TSR1 */
			for (offset = 0x41; offset <= 0x4f; offset++)
				cd->xlb[base + offset] = 0x55;
			for (offset = 0x22; offset <= 0x25; offset++)
				cd->xlb[base + offset] = 0xff;
			timeout = jiffies + 100 * HZ / 1000;
			spin_unlock_irq(&cd->lock);
			while (jiffies < timeout) ;
			spin_lock_irq(&cd->lock);
			cd->xlb[base + 0x1b] = 0x9a;	/* CRC3: set ESR as well */
			cd->xlb[base + 0x1b] = 0x82;	/* CRC3: TSCLKM only */
			sp->config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
			/* enable interrupts */
			cd->xlb[CTLREG] = (INTENA | E1DIV);
			spin_unlock_irq(&cd->lock);
		}
			break;
		case SDL_GTYPE_T1:
		{
			int byte, val, c;
			unsigned short mask = 0;
			printd(("T400P-SS7: performing enable on T1 span %d\n", span));
			spin_lock_irq(&cd->lock);
			// cd->xlb[SYNREG] = SYNCSELF; /* NO, NO, NO */
			/* Tell ISR to re-evaluate the sync source */
			cd->eval_syncsrc = 1;
			cd->xlb[CTLREG] = 0;
			cd->xlb[LEDREG] = 0xff;
			for (offset = 0; offset < 160; offset++)
				cd->xlb[base + offset] = 0x00;
			/* Set up for interleaved serial bus operation, byte mode */
			if (span == 0)
				cd->xlb[base + 0x94] = 0x09;
			else
				cd->xlb[base + 0x94] = 0x08;
			cd->xlb[base + 0x2b] = 0x08;	/* Full-on sync required (RCR1) */
			cd->xlb[base + 0x2c] = 0x08;	/* RSYNC is an input (RCR2) */
			cd->xlb[base + 0x35] = 0x10;	/* RBS enable (TCR1) */
			cd->xlb[base + 0x36] = 0x04;	/* TSYNC to be output (TCR2) */
			cd->xlb[base + 0x37] = 0x9c;	/* Tx & Rx Elastic stor, sysclk(s) = 2.048
							   mhz, loopback controls (CCR1) */
			cd->xlb[base + 0x12] = 0x22;	/* Set up received loopup and loopdown
							   codes */
			cd->xlb[base + 0x14] = 0x80;
			cd->xlb[base + 0x15] = 0x80;
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
			cd->xlb[base + 0x38] = val;
			if (sp->config.ifcoding != SDL_CODING_B8ZS)
				cd->xlb[base + 0x7e] = 0x1c;	/* Set FDL register to 0x1c */
			cd->xlb[base + 0x7c] = sp->config.iftxlevel << 5;	/* LBO */
			cd->xlb[base + 0x0a] = 0x80;	/* LIRST to reset line interface */
			timeout = jiffies + 100 * HZ / 1000;
			spin_unlock_irq(&cd->lock);
			while (jiffies < timeout) ;
			spin_lock_irq(&cd->lock);
			cd->xlb[base + 0x0a] = 0x30;	/* LIRST bask to normal, Resetting elastic
							   buffers */
			sp->config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
			/* enable interrupts */
			cd->xlb[CTLREG] = (INTENA);
			spin_unlock_irq(&cd->lock);
			/* establish which channels are clear channel */
			for (c = 0; c < 24; c++) {
				byte = c >> 3;
				if (!cd->spans[span]->slots[xp_t1_chan_map[c]]
				    || cd->spans[span]->slots[xp_t1_chan_map[c]]->config.iftype !=
				    SDL_TYPE_DS0A)
					mask |= 1 << (c % 8);
				if ((c % 8) == 7)
					cd->xlb[base + 0x39 + byte] = mask;
			}
		}
			break;
		}
	}
	xp->slot = slot;
	xp->config.ifname = sp->config.ifname;
	xp->config.ifflags = 0;
	xp->config.iftype = xp->config.iftype;
	switch (xp->config.iftype) {
	case SDL_TYPE_E1:
		xp->config.ifrate = 2048000;
		break;
	case SDL_TYPE_T1:
		xp->config.ifrate = 1544000;
		break;
	case SDL_TYPE_DS0:
		xp->config.ifrate = 64000;
		break;
	case SDL_TYPE_DS0A:
		xp->config.ifrate = 56000;
		break;
	}
	xp->config.ifmode = SDL_MODE_PEER;
	xp->config.ifflags |= SDL_IF_UP;
	xp->i_state = LMI_ENABLED;
	return (QR_DONE);
      lmi_outstate:
	return lmi_error_ack(q, xp, xp->i_state, LMI_ENABLE_REQ, 0, LMI_OUTSTATE);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	/* validate disable */
	if (xp->i_state != LMI_ENABLED)
		goto lmi_outstate;
	xp->i_state = LMI_DISABLE_PENDING;
	/* perform disable */
	if ((err = lmi_disable_con(q, xp)))
		return (err);
	/* commit disable */
	xp->config.ifflags = 0;
	return (QR_DONE);
      lmi_outstate:
	return lmi_error_ack(q, xp, xp->i_state, LMI_DISABLE_REQ, 0, LMI_OUTSTATE);
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
 *
 *  Test and Commit configuration settings
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_test_config(struct xp *xp, sdl_config_t * arg)
{
	if (xp) {
		if (arg->ifflags) {
			trace();
			return (-EINVAL);
		}
		switch (arg->iftype) {
		case SDL_TYPE_NONE:	/* unknown/unspecified */
			arg->iftype = xp->config.iftype;
			break;
		case SDL_TYPE_DS0:	/* DS0 channel */
		case SDL_TYPE_DS0A:	/* DS0A channel */
			/* yes we allow DS0A on E1 */
			break;
		case SDL_TYPE_E1:	/* full E1 span */
			if (xp->sp->config.ifgtype != SDL_GTYPE_E1) {
				trace();
				return (-EINVAL);
			}
			break;
		case SDL_TYPE_T1:	/* full T1 span */
			if (xp->sp->config.ifgtype != SDL_GTYPE_T1) {
				trace();
				return (-EINVAL);
			}
			break;
		default:
			trace();
			return (-EINVAL);
		}
		switch (arg->ifmode) {
		case SDL_MODE_NONE:	/* */
			arg->ifmode = xp->config.ifmode;
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
			return (-EINVAL);
		}
	}
	if (xp->sp) {
		switch (arg->ifgtype) {
		case SDL_GTYPE_NONE:	/* */
			arg->ifgtype = xp->sp->config.ifgtype;
			break;
		case SDL_GTYPE_T1:	/* */
		case SDL_GTYPE_E1:	/* */
			if (arg->ifgtype != xp->sp->config.ifgtype) {
				trace();
				return (-EINVAL);
			}
			break;
		default:
			trace();
			return (-EINVAL);
		}
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
				return (-EINVAL);
			}
			break;
		case SDL_GCRC_CRC4:	/* */
			if (arg->ifgtype != SDL_GTYPE_E1) {
				trace();
				return (-EINVAL);
			}
			break;
		case SDL_GCRC_CRC5:	/* */
			if (arg->ifgtype != SDL_GTYPE_E1) {
				trace();
				return (-EINVAL);
			}
			break;
		case SDL_GCRC_CRC6:	/* */
			if (arg->ifgtype != SDL_GTYPE_T1) {
				trace();
				return (-EINVAL);
			}
			break;
		default:
			trace();
			return (-EINVAL);
		}
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
			return (-EINVAL);
		}
		switch (arg->ifcoding) {
		case SDL_CODING_NONE:	/* */
			arg->ifcoding = xp->sp->config.ifcoding;
			break;
		case SDL_CODING_AMI:	/* */
			break;
		case SDL_CODING_B8ZS:	/* */
			if (arg->ifgtype != SDL_GTYPE_T1) {
				trace();
				return (-EINVAL);
			}
			break;
		case SDL_CODING_HDB3:	/* */
			if (arg->ifgtype != SDL_GTYPE_E1) {
				trace();
				return (-EINVAL);
			}
			break;
		default:
		case SDL_CODING_B6ZS:	/* */
			trace();
			return (-EINVAL);
		}
		switch (arg->ifframing) {
		case SDL_FRAMING_NONE:	/* */
			arg->ifframing = xp->sp->config.ifframing;
			break;
		case SDL_FRAMING_CCS:	/* */
		case SDL_FRAMING_CAS:	/* */
			if (arg->ifgtype != SDL_GTYPE_E1) {
				trace();
				return (-EINVAL);
			}
			break;
		case SDL_FRAMING_SF:	/* */
		case SDL_FRAMING_ESF:	/* */
			if (arg->ifgtype != SDL_GTYPE_T1) {
				trace();
				return (-EINVAL);
			}
			break;
		default:
			trace();
			return (-EINVAL);
		}
		switch (arg->iftxlevel) {
		case 0:
			arg->iftxlevel = xp->sp->config.iftxlevel;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			break;
		default:
			trace();
			return (-EINVAL);
		}
	}
	if (xp->cd) {
		int src;
		for (src = 0; src < SDL_SYNCS; src++)
			if (arg->ifsyncsrc[src] < 0 || arg->ifsyncsrc[src] > 4) {
				trace();
				return (-EINVAL);
			}
	}
	return (0);
}
STATIC void
sdl_commit_config(struct xp *xp, sdl_config_t * arg)
{
	int chan_reconfig = 0, span_reconfig = 0, card_reconfig = 0;
	if (xp) {
		if (xp->config.iftype != arg->iftype) {
			xp->config.iftype = arg->iftype;
			chan_reconfig = 1;
		}
		switch (arg->iftype) {
		case SDL_TYPE_DS0A:
			xp->config.ifrate = 56000;
			break;
		case SDL_TYPE_DS0:
			xp->config.ifrate = 64000;
			break;
		case SDL_TYPE_T1:
			xp->config.ifrate = 1544000;
			break;
		case SDL_TYPE_E1:
			xp->config.ifrate = 2048000;
			break;
		}
		if (xp->config.ifrate != arg->ifrate) {
			xp->config.ifrate = arg->ifrate;
			chan_reconfig = 1;
		}
		if (xp->config.ifmode != arg->ifmode) {
			xp->config.ifmode = arg->ifmode;
			chan_reconfig = 1;
		}
	}
	if (xp->sp) {
		if (xp->sp->config.ifgcrc != arg->ifgcrc) {
			xp->sp->config.ifgcrc = arg->ifgcrc;
			span_reconfig = 1;
		}
		if (xp->sp->config.ifclock != arg->ifclock) {
			xp->sp->config.ifclock = arg->ifclock;
			span_reconfig = 1;
			card_reconfig = 1;
		}
		if (xp->sp->config.ifcoding != arg->ifcoding) {
			xp->sp->config.ifcoding = arg->ifcoding;
			span_reconfig = 1;
		}
		if (xp->sp->config.ifframing != arg->ifframing) {
			xp->sp->config.ifframing = arg->ifframing;
			span_reconfig = 1;
		}
		if (xp->sp->config.iftxlevel != arg->iftxlevel) {
			xp->sp->config.iftxlevel = arg->iftxlevel;
			span_reconfig = 1;
		}
	}
	if (xp->cd) {
		int src;
		for (src = 0; src < SDL_SYNCS; src++) {
			if (xp->cd->config.ifsyncsrc[src] != arg->ifsyncsrc[src]) {
				xp->cd->config.ifsyncsrc[src] = arg->ifsyncsrc[src];
				card_reconfig = 1;
			}
		}
	}
	if (chan_reconfig && xp->config.ifflags & SDL_IF_UP) {
		fixme(("FIXME: Reconfigure the chan if already running\n"));
	}
	if (span_reconfig && xp->sp->config.ifflags & SDL_IF_UP) {
		fixme(("FIXME: Reconfigure the span if already running\n"));
	}
	if (card_reconfig && xp->cd->config.ifflags & SDL_IF_UP) {
		fixme(("FIXME: Reconfigure the card if already running\n"));
	}
	return;
}

/*
 *  SDL_IOCGOPTIONS:	lmi_option_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		lmi_option_t *arg = (lmi_option_t *) mp->b_cont->b_rptr;
		*arg = xp->option;
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCSOPTIONS:	lmi_option_t
 *  -----------------------------------
 */
STATIC int
sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		xp->option = *arg;
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCGCONFIG:	sdl_config_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		struct sp *sp;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		bzero(arg, sizeof(*arg));
		if (xp) {
			arg->ifflags = xp->config.ifflags;
			arg->iftype = xp->config.iftype;
			arg->ifrate = xp->config.ifrate;
			arg->ifmode = xp->config.ifmode;
		}
		if ((sp = xp->sp)) {
			struct cd *cd;
			arg->ifgtype = sp->config.ifgtype;
			arg->ifgrate = sp->config.ifgrate;
			arg->ifgcrc = sp->config.ifgcrc;
			arg->ifclock = sp->config.ifclock;
			arg->ifcoding = sp->config.ifcoding;
			arg->ifframing = sp->config.ifframing;
			arg->ifalarms = sp->config.ifalarms;
			arg->ifrxlevel = sp->config.ifrxlevel;
			arg->iftxlevel = sp->config.iftxlevel;
			if ((cd = xp->cd) || (cd = sp->cd)) {
				int src;
				for (src = 0; src < SDL_SYNCS; src++)
					arg->ifsyncsrc[src] = cd->config.ifsyncsrc[src];
				arg->ifsync = cd->config.ifsync;
			}
		}
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCSCONFIG:	sdl_config_t
 *  -----------------------------------
 */
STATIC int
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
 *  SDL_IOCTCONFIG:	sdl_config_t
 *  -----------------------------------
 */
STATIC int
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
 *  SDL_IOCCCONFIG:	sdl_config_t
 *  -----------------------------------
 */
STATIC int
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
 *  SDL_IOCGSTATEM:	sdl_statem_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		*arg = xp->statem;
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCCMRESET:	sdl_statem_t
 *  -----------------------------------
 */
STATIC int
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
 *  SDL_IOCGSTATSP:	sdl_stats_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		*arg = xp->statsp;
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCSSTATSP:	sdl_stats_t
 *  -----------------------------------
 */
STATIC int
sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		fixme(("FIXME: check these settings\n"));
		xp->statsp = *arg;
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCGSTATS:	sdl_stats_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		*arg = xp->stats;
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCCSTATS:	sdl_stats_t
 *  -----------------------------------
 */
STATIC int
sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	(void) mp;
	bzero(&xp->stats, sizeof(xp->stats));
	return (0);
}

/*
 *  SDL_IOCGNOTIFY:	sdl_notify_t
 *  -----------------------------------
 */
STATIC int
sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		*arg = xp->notify;
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCSNOTIFY:	sdl_notify_t
 *  -----------------------------------
 */
STATIC int
sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		xp->notify.events |= arg->events;
		return (0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SDL_IOCCNOTIFY:	sdl_notify_t
 *  -----------------------------------
 */
STATIC int
sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		xp->notify.events &= ~arg->events;
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
	struct xp *xp = XP_PRIV(q);
	(void) mp;
	xp->config.ifflags &= ~SDL_IF_TX_RUNNING;
	return (0);
}

/*
 *  SDL_IOCCONNTX:	
 *  -----------------------------------
 */
STATIC int
sdl_ioccconntx(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	(void) mp;
	xp->config.ifflags |= SDL_IF_TX_RUNNING;
	return (0);
}

/*
 *  =========================================================================
 *
 *  EVENTS From Below
 *
 *  =========================================================================
 */
STATIC INLINE void
xp_rx_chan_block(struct xp *xp)
{
	if (xp->config.ifflags & SDL_IF_RX_RUNNING) {
		mblk_t *mp;
		if (canput(xp->oq) && (mp = allocb(8, BPRI_MED))) {
			uchar *offset;
			for (offset = xp->rx_base; offset < 1024 + xp->rx_base; offset += 128) {
				*mp->b_wptr++ = *offset;
			}
			ss7_oput(xp->oq, mp);
		} else {
			xp->stats.rx_buffer_overflows++;
			xp->stats.rx_overruns += 8;
		}
		xp->stats.rx_octets += 8;
	}
}
STATIC INLINE void
xp_tx_chan_block(struct xp *xp)
{
	if (xp->config.ifflags & SDL_IF_TX_RUNNING) {
		mblk_t *mp;
		uchar *offset;
		if ((mp = (mblk_t *) xchg(&xp->tx_msg, NULL))) {
			for (offset = xp->tx_base; offset < 1024 + xp->tx_base; offset += 128) {
				if (mp->b_rptr < mp->b_wptr)
					*offset = *mp->b_rptr++;
				else {
					*offset = 0xff;
					xp->stats.tx_underruns++;
				}
			}
			freemsg(mp);
		} else {
			xp->stats.tx_buffer_overflows++;
			for (offset = xp->tx_base; offset < 1024 + xp->tx_base; offset += 128)
				*offset = 0xff;	/* mark idle to abort frames */
			xp->stats.tx_underruns += 8;
		}
		xp->stats.tx_octets += 8;
	}
}
STATIC INLINE void
xp_rx_t1_span_block(struct xp *xp)
{
	if (xp->config.ifflags & SDL_IF_RX_RUNNING) {
		mblk_t *mp;
		if (canput(xp->oq) && (mp = allocb(8 * 24, BPRI_MED))) {
			int chan;
			uchar *offset;
			for (offset = xp->rx_base; offset < 1024 + xp->rx_base; offset += 128) {
				for (chan = 0; chan < 24; chan++) {
					*mp->b_wptr++ = *(offset + xp_t1_chan_map[chan]);
				}
			}
			ss7_oput(xp->oq, mp);
		} else {
			xp->stats.rx_buffer_overflows++;
			xp->stats.rx_overruns += 8 * 24;
		}
		xp->stats.rx_octets += 8 * 24;
	}
}
STATIC INLINE void
xp_tx_t1_span_block(struct xp *xp)
{
	if (xp->config.ifflags & SDL_IF_TX_RUNNING) {
		mblk_t *mp;
		int chan;
		uchar *offset;
		if ((mp = (mblk_t *) xchg(&xp->tx_msg, NULL))) {
			for (offset = xp->tx_base; offset < 1024 + xp->tx_base; offset += 128) {
				for (chan = 0; chan < 24; chan++) {
					if (mp->b_rptr < mp->b_wptr)
						*(offset + xp_t1_chan_map[chan]) = *mp->b_rptr++;
					else {
						*(offset + xp_t1_chan_map[chan]) = 0xff;
						xp->stats.tx_underruns++;
					}
				}
			}
			freemsg(mp);
		} else {
			xp->stats.tx_buffer_overflows++;
			for (offset = xp->tx_base; offset < 1024 + xp->tx_base; offset += 128)
				for (chan = 0; chan < 24; chan++)
					*offset = 0xff;	/* mark idle to abort frames */
			xp->stats.tx_underruns += 8 * 24;
		}
		xp->stats.tx_octets += 8 * 24;
	}
}
STATIC INLINE void
xp_rx_e1_span_block(struct xp *xp)
{
	if (xp->config.ifflags & SDL_IF_RX_RUNNING) {
		mblk_t *mp;
		if (canput(xp->oq) && (mp = allocb(8 * 31, BPRI_MED))) {
			int chan;
			uchar *offset;
			for (offset = xp->rx_base; offset < 1024 + xp->rx_base; offset += 128) {
				for (chan = 0; chan < 31; chan++) {
					*mp->b_wptr++ = *(offset + xp_e1_chan_map[chan]);
				}
			}
			ss7_oput(xp->oq, mp);
		} else {
			xp->stats.rx_buffer_overflows++;
			xp->stats.rx_overruns += 8 * 31;
		}
		xp->stats.rx_octets += 8 * 31;
	}
}
STATIC INLINE void
xp_tx_e1_span_block(struct xp *xp)
{
	if (xp->config.ifflags & SDL_IF_TX_RUNNING) {
		mblk_t *mp;
		int chan;
		uchar *offset;
		if ((mp = (mblk_t *) xchg(&xp->tx_msg, NULL))) {
			for (offset = xp->tx_base; offset < 1024 + xp->tx_base; offset += 128) {
				for (chan = 0; chan < 31; chan++) {
					if (mp->b_rptr < mp->b_wptr)
						*(offset + xp_e1_chan_map[chan]) = *mp->b_rptr++;
					else {
						*(offset + xp_e1_chan_map[chan]) = 0xff;
						xp->stats.tx_underruns++;
					}
				}
			}
			freemsg(mp);
		} else {
			xp->stats.tx_buffer_overflows++;
			for (offset = xp->tx_base; offset < 1024 + xp->tx_base; offset += 128)
				for (chan = 0; chan < 24; chan++)
					*offset = 0xff;	/* mark idle to abort frames */
			xp->stats.tx_underruns += 8 * 31;
		}
		xp->stats.tx_octets += 8 * 31;
	}
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
 *
 *  X400P Tasklet
 *  -----------------------------------
 *  This tasklet is scheduled before the ISR returns to feed the next buffer
 *  of data into the write buffer and read the buffer of data from the read
 *  buffer.  This will run the soft-HDLC on each channel for 8 more bytes, or
 *  if full span will run the soft-HDLC for 192 bytes (T1) or 256 bytes (E1).
 */
STATIC void
xp_e1_card_tasklet(unsigned long data)
{
	struct cd *cd = (struct cd *) data;
	int span;
	/* address transmit and receive first */
	for (span = 0; span < 4; span++) {
		struct sp *sp;
		if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
			struct xp *xp;
			int base = span << 8;
			switch (sp->config.iftype) {
			case SDL_TYPE_DS0:
			case SDL_TYPE_DS0A:
			{
				int chan;
				/* one chan at a time, 8 frames */
				for (chan = 0; chan < 31; chan++) {
					if ((xp = sp->slots[xp_e1_chan_map[chan]])) {
						xp_tx_chan_block(xp);
						xp_rx_chan_block(xp);
					}
				}
				break;
			}
			case SDL_TYPE_E1:
				/* entire span, one frame at a time */
				if ((xp = sp->slots[1])) {
					xp_tx_e1_span_block(xp);
					xp_rx_e1_span_block(xp);
				}
				break;
			default:
				swerr();
				break;
			}
			if (sp->recovertime && !--sp->recovertime) {
				sp->config.ifalarms &= ~SDL_ALARM_REC;
				cd->xlb[base + 0x21] = 0x5f;	/* turn off yellow */
				cd->eval_syncsrc = 1;
			}
			/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
			if ((((cd->frame >> 3) & 0x3f) - 51) == span) {
				int status, alarms = 0, leds = 0;
				cd->xlb[base + 0x06] = 0xff;
				status = cd->xlb[base + 0x06];
				if (status & 0x09)
					alarms |= SDL_ALARM_RED;
				if (status & 0x02)
					alarms |= SDL_ALARM_BLU;
				if (alarms) {
					if (!
					    (sp->config.
					     ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
						/* alarms have just begun */
						cd->xlb[base + 0x21] = 0x7f;	/* set yellow alarm 
										 */
						cd->eval_syncsrc = 1;
					}
				} else {
					if ((sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
						/* alarms have just ended */
						alarms |= SDL_ALARM_REC;
						sp->recovertime = X400P_ALARM_SETTLE_TIME;
					}
				}
				if (status & 0x04)
					alarms |= SDL_ALARM_YEL;
				sp->config.ifalarms = alarms;
				/* adjust leds */
				if (alarms & SDL_ALARM_RED)
					leds |= LEDRED;
				else if (alarms & SDL_ALARM_YEL)
					leds |= LEDYEL;
				else
					leds |= LEDGRN;
				cd->leds &= ~(LEDYEL << (span << 1));
				cd->leds |= leds << (span << 1);
				cd->xlb[LEDREG] = cd->leds;
			}
			if (((cd->frame - 8) % 8000))
				sp->config.ifbpv +=
				    cd->xlb[base + 0x01] + (cd->xlb[base + 0x00] << 8);
		}
	}
	if (cd->eval_syncsrc) {
		int src, syncsrc = 0;
		for (src = 0; src < SDL_SYNCS; src++) {
			if (cd->config.ifsyncsrc[src] && cd->spans[syncsrc - 1]
			    && !(cd->spans[syncsrc - 1]->config.ifclock == SDL_CLOCK_LOOP)
			    && !(cd->spans[syncsrc - 1]->config.
				 ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
				syncsrc = cd->config.ifsyncsrc[src];
				break;
			}
		}
		cd->xlb[SYNREG] = syncsrc;
		cd->config.ifsync = syncsrc;
		cd->eval_syncsrc = 0;
	}
}
STATIC void
xp_t1_card_tasklet(unsigned long data)
{
	struct cd *cd = (struct cd *) data;
	int span;
	/* address transmit and receive first */
	for (span = 0; span < 4; span++) {
		struct sp *sp;
		if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
			struct xp *xp;
			int base = span << 8;
			switch (sp->config.iftype) {
			case SDL_TYPE_DS0:
			case SDL_TYPE_DS0A:
			{
				int chan;
				/* one chan at a time, 8 frames */
				for (chan = 0; chan < 24; chan++) {
					if ((xp = sp->slots[xp_t1_chan_map[chan]])) {
						xp_tx_chan_block(xp);
						xp_rx_chan_block(xp);
					}
				}
				break;
			}
			case SDL_TYPE_T1:
				/* entire span, one frame at a time */
				if ((xp = sp->slots[1])) {
					xp_tx_t1_span_block(xp);
					xp_rx_t1_span_block(xp);
				}
				break;
			default:
				swerr();
				break;
			}
			if (sp->recovertime && !--sp->recovertime) {
				sp->config.ifalarms &= ~SDL_ALARM_REC;
				cd->xlb[base + 0x35] = 0x10;	/* turn off yellow */
				cd->eval_syncsrc = 1;
			}
			/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
			if ((((cd->frame >> 3) & 0x3f) - 51) == span) {
				int status, alarms = 0, leds = 0;
				sp->config.ifrxlevel = cd->xlb[base + RIR2] >> 6;
				cd->xlb[base + 0x20] = 0xff;
				status = cd->xlb[base + 0x20];
				if (status & 0x03)
					alarms |= SDL_ALARM_RED;
				if (status & 0x08)
					alarms |= SDL_ALARM_BLU;
				if (alarms) {
					if (!
					    (sp->config.
					     ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
						/* alarms have just begun */
						cd->xlb[base + 0x35] = 0x11;	/* set yellow alarm 
										 */
						cd->eval_syncsrc = 1;
					}
				} else {
					if ((sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
						/* alarms have just ended */
						alarms |= SDL_ALARM_REC;
						sp->recovertime = X400P_ALARM_SETTLE_TIME;
					}
				}
				if (status & 0x04)
					alarms |= SDL_ALARM_YEL;
				sp->config.ifalarms = alarms;
				/* adjust leds */
				if (alarms & SDL_ALARM_RED)
					leds |= LEDRED;
				else if (alarms & SDL_ALARM_YEL)
					leds |= LEDYEL;
				else
					leds |= LEDGRN;
				cd->leds &= ~(LEDYEL << (span << 1));
				cd->leds |= leds << (span << 1);
				cd->xlb[LEDREG] = cd->leds;
			}
			if (((cd->frame - 8) % 8000))
				sp->config.ifbpv +=
				    cd->xlb[base + 0x24] + (cd->xlb[base + 0x23] << 8);
		}
	}
	if (cd->eval_syncsrc) {
		int src, syncsrc = 0;
		for (src = 0; src < SDL_SYNCS; src++) {
			if (cd->config.ifsyncsrc[src] && cd->spans[syncsrc - 1]
			    && !(cd->spans[syncsrc - 1]->config.ifclock == SDL_CLOCK_LOOP)
			    && !(cd->spans[syncsrc - 1]->config.
				 ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
				syncsrc = cd->config.ifsyncsrc[src];
				break;
			}
		}
		cd->xlb[SYNREG] = syncsrc;
		cd->config.ifsync = syncsrc;
		cd->eval_syncsrc = 0;
	}
}
STATIC irqreturn_t
xp_e1_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct cd *cd = (struct cd *) dev_id;
	/* active interrupt (otherwise spurious or shared) */
	if (cd->xlb[STAREG] & INTACTIVE) {
		int word;
		volatile uint32_t *xll, *wbuf, *rbuf;
		cd->xlb[CTLREG] = (INTENA | OUTBIT | INTACK | E1DIV);
		/* write/read burst */
		for (xll = cd->xll, wbuf = cd->wbuf, rbuf = cd->rbuf, word = 0; word < 256; word++) {
			*xll = *wbuf++;
			*rbuf++ = *xll++;
		}
		cd->frame += 8;
		cd->xlb[CTLREG] = (INTENA | E1DIV);
		tasklet_hi_schedule(&cd->tasklet);
		return (irqreturn_t)(IRQ_HANDLED);
	}
	return (irqreturn_t)(IRQ_NONE);
}
STATIC irqreturn_t
xp_t1_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct cd *cd = (struct cd *) dev_id;
	/* active interrupt (otherwise spurious or shared) */
	if (cd->xlb[STAREG] & INTACTIVE) {
		int word;
		volatile uint32_t *xll, *wbuf, *rbuf;
		cd->xlb[CTLREG] = (INTENA | OUTBIT | INTACK);
		/* write/read burst */
		for (xll = cd->xll, wbuf = cd->wbuf, rbuf = cd->rbuf, word = 0; word < 256; word++) {
			*xll = *wbuf++;
			*rbuf++ = *xll++;
		}
		cd->frame += 8;
		cd->xlb[CTLREG] = (INTENA);
		tasklet_hi_schedule(&cd->tasklet);
		return (irqreturn_t)(IRQ_HANDLED);
	}
	return (irqreturn_t)(IRQ_NONE);
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
	case SDL_IOC_MAGIC:
		if (count < size || xp->i_state == LMI_UNATTACHED) {
			ptrace(("X400P-SS7: ERROR: ioctl count = %d, size = %d, state = %ld\n",
				count, size, xp->i_state));
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
	struct xp *xp = XP_PRIV(q);
	ulong oldstate = xp->i_state;
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case SDL_BITS_FOR_TRANSMISSION_REQ:
		rtn = sdl_bits_for_transmission_req(q, mp);
		break;
	case SDL_CONNECT_REQ:
		rtn = sdl_connect_req(q, mp);
		break;
	case SDL_DISCONNECT_REQ:
		rtn = sdl_disconnect_req(q, mp);
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
		xp->i_state = oldstate;
	return (rtn);
}

/*
 *  M_DATA Handling
 *  -------------------------------------------------------------------------
 */
STATIC int
xp_w_data(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	if (xp->tx_msg) {
		/* wait for tx-block to remove */
		return (-EBUSY);
	}
	xp->tx_msg = mp;
	return (QR_ABSORBED);
}
STATIC int
xp_r_data(queue_t *q, mblk_t *mp)
{
	/* pass it along (but really an error) */
	return (QR_PASSFLOW);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
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
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return xp_w_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
xp_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return xp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return xp_r_data(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
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
		printd(("%s: Clone open in effect on major %d\n", DRV_NAME, (int)cmajor));
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
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %hu:%hu\n", DRV_NAME, (ushort)cmajor, (ushort)cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(xp = xp_alloc_priv(q, xpp, devp, crp))) {
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
	struct xp *xp = XP_PRIV(q);
	psw_t flags;
	(void) flag;
	(void) crp;
	(void) xp;
	printd(("%s: closing character device %hu:%hu\n", DRV_NAME, (ushort)xp->u.dev.cmajor,
		(ushort)xp->u.dev.cminor));
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
STATIC int
xp_init_caches(void)
{
	if (!xp_priv_cachep
	    && !(xp_priv_cachep =
		 kmem_cache_create("xp_priv_cachep", sizeof(struct xp), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("X400P-SS7: initialized device private structure cache\n"));
	if (!xp_span_cachep
	    && !(xp_span_cachep =
		 kmem_cache_create("xp_span_cachep", sizeof(struct sp), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_span_cachep", __FUNCTION__);
		kmem_cache_destroy(xchg(&xp_priv_cachep, NULL));
		return (-ENOMEM);
	} else
		printd(("X400P-SS7: initialized span private structure cache\n"));
	if (!xp_card_cachep
	    && !(xp_card_cachep =
		 kmem_cache_create("xp_card_cachep", sizeof(struct cd), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_card_cachep", __FUNCTION__);
		kmem_cache_destroy(xchg(&xp_span_cachep, NULL));
		kmem_cache_destroy(xchg(&xp_priv_cachep, NULL));
		return (-ENOMEM);
	} else
		printd(("X400P-SS7: initialized card private structure cache\n"));
	if (!xp_xbuf_cachep
	    && !(xp_xbuf_cachep =
		 kmem_cache_create("xp_xbuf_cachep", 1024, 0, SLAB_HWCACHE_ALIGN, NULL, NULL))) {
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
		if (kmem_cache_destroy(xp_xbuf_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_xbuf_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("X400P-SS7: destroyed xp_xbuf_cache\n"));
	}
	if (xp_card_cachep) {
		if (kmem_cache_destroy(xp_card_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_card_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("X400P-SS7: destroyed xp_card_cache\n"));
	}
	if (xp_span_cachep) {
		if (kmem_cache_destroy(xp_span_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_span_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("X400P-SS7: destroyed xp_span_cache\n"));
	}
	if (xp_priv_cachep) {
		if (kmem_cache_destroy(xp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy xp_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("X400P-SS7: destroyed xp_priv_cache\n"));
	}
	return (err);
}

/*
 *  Private structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC struct xp *
xp_alloc_priv(queue_t *q, struct xp **xpp, dev_t *devp, cred_t *crp)
{
	struct xp *xp;
	if ((xp = kmem_cache_alloc(xp_priv_cachep, SLAB_ATOMIC))) {
		printd(("X400P-SS7: allocated device private structure\n"));
		bzero(xp, sizeof(*xp));
		xp_get(xp);	/* first get */
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
		spin_lock_init(&xp->lock);	/* "xp-priv-lock" */
		if ((xp->next = *xpp))
			xp->next->prev = &xp->next;
		xp->prev = xpp;
		*xpp = xp_get(xp);
		printd(("X400P-SS7: linked device private structure\n"));
		/* LMI configuration defaults */
		xp->option.pvar = SS7_PVAR_ITUT_88;
		xp->option.popt = 0;
		/* SDL configuration defaults */
		xp->config.ifname = NULL;
		xp->config.ifflags = 0;
		xp->config.iftype = SDL_TYPE_DS0;
		xp->config.ifrate = 64000;
		xp->config.ifmode = SDL_MODE_PEER;
		xp->config.ifblksize = 8;
		xp->tx_base = 0;
		xp->rx_base = 0;
		printd(("X400P-SS7: setting device private structure defaults\n"));
	} else
		ptrace(("X400P-SS7: ERROR: Could not allocate device private structure\n"));
	return (xp);
}

STATIC void
xp_free_priv(struct xp *xp)
{
	psw_t flags = 0;
	ensure(xp, return);
	spin_lock_irqsave(&xp->lock, flags);
	{
		struct sp *sp;
		ss7_unbufcall((str_t *) xp);
		if (xp->cd)
			cd_put(xchg(&xp->cd, NULL));
		if ((sp = xp->sp)) {
			int slot;
			for (slot = 0; slot < 32; slot++)
				if (sp->slots[slot] == xp)
					xp_put(xchg(&sp->slots[slot], NULL));
			sp_put(xchg(&xp->sp, NULL));
			printd(("X400P-SS7: unlinked device private structure from span\n"));
		} else
			ptrace(("X400P-SS7: ERROR: Arrrrgggh! sp = %p\n", sp));
		if (xp->tx_msg)
			freemsg((mblk_t *) xchg(&xp->tx_msg, NULL));
		if (xp->next || xp->prev != &xp->next) {
			if ((*xp->prev = xp->next))
				xp->next->prev = xp->prev;
			xp->next = NULL;
			xp->prev = &xp->next;
			xp_put(xp);
		}
		xp_put(xchg(&xp->oq->q_ptr, NULL));
		xp_put(xchg(&xp->iq->q_ptr, NULL));
		flushq(xchg(&xp->oq, NULL), FLUSHALL);
		flushq(xchg(&xp->iq, NULL), FLUSHALL);
		printd(("X400P-SS7: unlinked device private structure\n"));
	}
	spin_unlock_irqrestore(&xp->lock, flags);
	xp_put(xp);		/* final put */
	return;
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
		printd(("X400P-SS7: freed device private structure\n"));
	}
}

/*
 *  Span allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct sp *
xp_alloc_sp(struct cd *cd, uint8_t span)
{
	struct sp *sp;
	if ((sp = kmem_cache_alloc(xp_span_cachep, SLAB_ATOMIC))) {
		printd(("X400P-SS7: allocated span private structure\n"));
		bzero(sp, sizeof(*sp));
		cd_get(cd);	/* first get */
		/* create linkage */
		cd->spans[span] = sp_get(sp);
		sp->cd = cd_get(cd);
		spin_lock_init(&sp->lock);	/* "sp-priv-lock" */
		/* fill out span structure */
		printd(("X400P-SS7: linked span private structure\n"));
		sp->iobase = cd->iobase + (span << 8);
		sp->card = cd->card;
		sp->span = span;
		sp->config.ifname = cd->config.ifname;
		sp->config.ifflags = 0;
		sp->config.iftype = cd->config.iftype;
		sp->config.ifrate = cd->config.ifrate;
		sp->config.ifgtype = cd->config.ifgtype;
		sp->config.ifgrate = cd->config.ifgrate;
		sp->config.ifmode = cd->config.ifmode;
		sp->config.ifgcrc = cd->config.ifgcrc;
		sp->config.ifclock = cd->config.ifclock;
		sp->config.ifcoding = cd->config.ifcoding;
		sp->config.ifframing = cd->config.ifframing;
		sp->config.ifalarms = 0;
		sp->config.ifrxlevel = 0;
		sp->config.iftxlevel = cd->config.iftxlevel;
		printd(("X400P-SS7: set span private structure defaults\n"));
	} else
		ptrace(("X400P-SS7: ERROR: Could not allocate span private structure\n"));
	return (sp);
}

/* Note: called with card interrupts disabled */
STATIC void
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
		printd(("X400P-SS7: unlinked span private structure from card\n"));
		/* remove channel linkage */
		for (slot = 0; slot < 32; slot++) {
			struct xp *xp;
			if ((xp = sp->slots[slot])) {
				sp_put(xchg(&xp->sp, NULL));
				cd_put(xchg(&xp->cd, NULL));
				xp_put(xchg(&sp->slots[slot], NULL));
			}
		}
		printd(("X400P-SS7: unlinked span private structure from slots\n"));
	} else
		ptrace(("X400P-SS7: ERROR: spans cannot exist without cards\n"));
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
		printd(("X400P-SS7: freed span private structure\n"));
		kmem_cache_free(xp_span_cachep, sp);
	}
}

/*
 *  Card allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct cd *
xp_alloc_cd(void)
{
	struct cd *cd;
	if ((cd = kmem_cache_alloc(xp_card_cachep, SLAB_ATOMIC))) {
		uint32_t *wbuf;
		uint32_t *rbuf;
		printd(("X400P-SS7: allocated card private structure\n"));
		if (!(wbuf = kmem_cache_alloc(xp_xbuf_cachep, SLAB_ATOMIC))) {
			ptrace(("X400P-SS7: could not allocate write buffer\n"));
			kmem_cache_free(xp_card_cachep, cd);
			return (NULL);
		}
		if (!(rbuf = kmem_cache_alloc(xp_xbuf_cachep, SLAB_ATOMIC))) {
			ptrace(("X400P-SS7: could not allocate read buffer\n"));
			kmem_cache_free(xp_xbuf_cachep, wbuf);
			kmem_cache_free(xp_card_cachep, cd);
			return (NULL);
		}
		bzero(cd, sizeof(*cd));
		cd_get(cd);	/* first get */
		if ((cd->next = x400p_cards))
			cd->next->prev = &cd->next;
		x400p_cards = cd_get(cd);
		cd->prev = &x400p_cards;
		cd->card = x400p_boards++;
		cd->wbuf = wbuf;
		cd->rbuf = rbuf;
		printd(("X400P-SS7: linked card private structure\n"));
	} else
		ptrace(("X400P-SS7: ERROR: Could not allocate card private structure\n"));
	return (cd);
}
STATIC void
xp_free_cd(struct cd *cd)
{
	psw_t flags;
	spin_lock_irqsave(&cd->lock, flags);
	{
		int span;
		ensure(cd, return);
		/* remove any remaining spans */
		for (span = 0; span < 4; span++) {
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
		printd(("X400P-SS7: unlinked card private structure\n"));
		tasklet_kill(&cd->tasklet);
		kmem_cache_free(xp_xbuf_cachep, (uint32_t *) xchg(&cd->rbuf, NULL));
		kmem_cache_free(xp_xbuf_cachep, (uint32_t *) xchg(&cd->wbuf, NULL));
	}
	spin_unlock_irqrestore(&cd->lock, flags);
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
		printd(("X400P-SS7: freed card private structure\n"));
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
	struct cd *cd;
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
	if (!(cd = xp_alloc_cd()))
		return (-ENOMEM);
	if ((cd->irq = dev->irq) < 1) {
		ptrace(("X400P-SS7: ERROR: No IRQ allocated for device\n"));
		goto error_free;
	}
	printd(("X400P-SS7: device allocated IRQ %ld\n", cd->irq));
	if ((pci_resource_flags(dev, 0) & IORESOURCE_IO)
	    || !(cd->plx_region = pci_resource_start(dev, 0))
	    || !(cd->plx_length = pci_resource_len(dev, 0))
	    || !(cd->plx = ioremap(cd->plx_region, cd->plx_length))) {
		ptrace(("X400P-SS7: ERROR: Invalid PLX 9030 base resource\n"));
		goto error_free;
	}
	printd(("X400P-SS7: plx region %ld bytes at %lx, remapped %p\n", cd->plx_length,
		cd->plx_region, cd->plx));
	if ((pci_resource_flags(dev, 2) & IORESOURCE_IO)
	    || !(cd->xll_region = pci_resource_start(dev, 2))
	    || !(cd->xll_length = pci_resource_len(dev, 2))
	    || !(cd->xll = ioremap(cd->xll_region, cd->xll_length))) {
		ptrace(("X400P-SS7: ERROR: Invalid Xilinx 32-bit base resource\n"));
		goto error_free;
	}
	printd(("X400P-SS7: xll region %ld bytes at %lx, remapped %p\n", cd->xll_length,
		cd->xll_region, cd->xll));
	if ((pci_resource_flags(dev, 3) & IORESOURCE_IO)
	    || !(cd->xlb_region = pci_resource_start(dev, 3))
	    || !(cd->xlb_length = pci_resource_len(dev, 3))
	    || !(cd->xlb = ioremap(cd->xlb_region, cd->xlb_length))) {
		ptrace(("X400P-SS7: ERROR: Invalid Xilinx 8-bit base resource\n"));
		goto error_free;
	}
	printd(("X400P-SS7: xlb region %ld bytes at %lx, remapped %p\n", cd->xlb_length,
		cd->xlb_region, cd->xlb));
	cd->config.ifname = xp_board_info[id->driver_data].name;
	if (!request_mem_region(cd->plx_region, cd->plx_length, cd->config.ifname)) {
		ptrace(("X400P-SS7: ERROR: Unable to reserve PLX memory\n"));
		goto error_free;
	}
	printd(("X400P-SS7: plx region %lx reserved %ld bytes\n", cd->plx_region, cd->plx_length));
	if (!request_mem_region(cd->xll_region, cd->xll_length, cd->config.ifname)) {
		ptrace(("X400P-SS7: ERROR: Unable to reserve Xilinx 32-bit memory\n"));
		goto error_free_plx_region;
	}
	printd(("X400P-SS7: xll region %lx reserved %ld bytes\n", cd->xll_region, cd->xll_length));
	if (!request_mem_region(cd->xlb_region, cd->xlb_length, cd->config.ifname)) {
		ptrace(("X400P-SS7: ERROR: Unable to reserve Xilinx 8-bit memory\n"));
		goto error_free_xll_region;
	}
	printd(("X400P-SS7: xlb region %lx reserved %ld bytes\n", cd->xlb_region, cd->xlb_length));
	pci_set_drvdata(dev, cd);
	__printd(("X400P-SS7: detected %s at 0x%lx/0x%lx irq %ld\n", cd->config.ifname,
		  cd->xll_region, cd->xlb_region, cd->irq));
#ifdef X400P_DOWNLOAD_FIRMWARE
	{
		uint8_t *f = (uint8_t *) x400pfw;
		volatile unsigned long *data;
		unsigned long timeout;
		data = (volatile unsigned long *) &cd->plx[GPIOC];
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
			*cd->xlb = *f++;
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
	cd->plx[INTCSR] = 0;	/* disable interrupts */
	cd->xlb[SYNREG] = 0;
	cd->xlb[CTLREG] = 0;
	cd->xlb[LEDREG] = 0xff;
	if (cd->xlb[0x00f] & 0x80) {
		int word;
		printd(("E400P-SS7: E400P-SS7 Quad E1 Card\n"));
		for (word = 0; word < 256; word++) {
			cd->xll[word] = 0xffffffff;
			cd->wbuf[word] = 0xffffffff;
		}
		tasklet_init(&cd->tasklet, &xp_e1_card_tasklet, (unsigned long) cd);
		/* setup E1 card defaults */
		cd->config.ifgtype = SDL_GTYPE_E1;
		cd->config.ifgrate = 2048000;
		cd->config.ifgcrc = SDL_GCRC_CRC4;
		cd->config.ifclock = SDL_CLOCK_SLAVE;
		cd->config.ifcoding = SDL_CODING_HDB3;
		cd->config.ifframing = SDL_FRAMING_CCS;
		spin_lock_init(&cd->lock);	/* "cd-priv-lock" */
		if (request_irq(cd->irq, xp_e1_interrupt, SA_INTERRUPT | SA_SHIRQ, "x400p-ss7", cd)) {
			ptrace(("E400P-SS7: ERROR: Unable to request IRQ %ld\n", cd->irq));
			goto error_free_all;
		}
		printd(("E400P-SS7: acquired IRQ %ld for T400P-SS7 card\n", cd->irq));
		cd->config.ifflags = (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
		cd->config.iftype = SDL_TYPE_DS0;
		cd->config.ifrate = 64000;
		cd->config.ifmode = SDL_MODE_PEER;
		cd->config.iftxlevel = 0;
	} else {
		int word;
		printd(("T400P-SS7: T400P-SS7 Quad T1 Card\n"));
		for (word = 0; word < 256; word++) {
			cd->xll[word] = 0x7f7f7f7f;
			cd->wbuf[word] = 0x7f7f7f7f;
		}
		tasklet_init(&cd->tasklet, &xp_t1_card_tasklet, (unsigned long) cd);
		/* setup T1 card defaults */
		cd->config.ifgtype = SDL_GTYPE_T1;
		cd->config.ifgrate = 1544000;
		cd->config.ifgcrc = SDL_GCRC_CRC6;
		cd->config.ifclock = SDL_CLOCK_SLAVE;
		cd->config.ifcoding = SDL_CODING_AMI;
		cd->config.ifframing = SDL_FRAMING_SF;
		spin_lock_init(&cd->lock);	/* "cd-priv-lock" */
		if (request_irq(cd->irq, xp_t1_interrupt, SA_INTERRUPT | SA_SHIRQ, "x400p-ss7", cd)) {
			ptrace(("T400P-SS7: ERROR: Unable to request IRQ %ld\n", cd->irq));
			goto error_free_all;
		}
		printd(("T400P-SS7: acquired IRQ %ld for T400P-SS7 card\n", cd->irq));
		cd->config.ifflags = (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
		cd->config.iftype = SDL_TYPE_DS0;
		cd->config.ifrate = 64000;
		cd->config.ifmode = SDL_MODE_PEER;
		cd->config.iftxlevel = 0;
	}
	/* allocate span structures (if possible now; otherwise, later) */
	for (span = 0; span < 4; span++)
		xp_alloc_sp(cd, span);
	cd->plx[INTCSR] = PLX_INTENA;	/* enable interrupts */
	return (0);
      error_free_all:
	release_mem_region(cd->xlb_region, cd->xlb_length);
	printd(("X400P-SS7: released xlb region %lx length %ld\n", cd->xlb_region, cd->xlb_length));
      error_free_xll_region:
	release_mem_region(cd->xll_region, cd->xll_length);
	printd(("X400P-SS7: released xll region %lx length %ld\n", cd->xll_region, cd->xll_length));
      error_free_plx_region:
	release_mem_region(cd->plx_region, cd->plx_length);
	printd(("X400P-SS7: released plx region %lx length %ld\n", cd->plx_region, cd->plx_length));
      error_free:
	xp_free_cd(cd);
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
	struct cd *cd = pci_get_drvdata(dev);
	struct sp *sp;
	int i;
	ensure(cd, return);
	/* disable interrupts */
	if (cd->plx_length) {
		cd->plx[INTCSR] = 0;
	}
	if (cd->xlb_length) {
		cd->xlb[SYNREG] = 0;
		cd->xlb[CTLREG] = 0;
		cd->xlb[LEDREG] = 0;
	}
	/* free any lingering span structures */
	for (i = 0; i < 4; i++)
		if ((sp = cd->spans[i]))
			xp_free_sp(sp);
	if (cd->irq) {
		free_irq(cd->irq, cd);
		cd->irq = 0;
	}
	if (cd->xlb_length) {
		release_mem_region(cd->xlb_region, cd->xlb_length);
		printd(("X400P-SS7: released xlb region %lx length %ld\n", cd->xlb_region,
			cd->xlb_length));
		cd->xlb_region = 0;
		cd->xlb_length = 0;
	}
	if (cd->xll_length) {
		release_mem_region(cd->xll_region, cd->xll_length);
		printd(("X400P-SS7: released xll region %lx length %ld\n", cd->xll_region,
			cd->xll_length));
		cd->xll_region = 0;
		cd->xll_length = 0;
	}
	if (cd->plx_length) {
		release_mem_region(cd->plx_region, cd->plx_length);
		cd->plx_region = 0;
		cd->plx_length = 0;
		printd(("X400P-SS7: released plx region %lx length %ld\n", cd->plx_region,
			cd->plx_length));
	}
	if (cd->xlb) {
		iounmap((void *) cd->xlb);
		printd(("X400P-SS7: unmapped xlb memory at %p\n", cd->xlb));
		cd->xlb = NULL;
	}
	if (cd->xll) {
		iounmap((void *) cd->xll);
		printd(("X400P-SS7: unmapped xll memory at %p\n", cd->xll));
		cd->xll = NULL;
	}
	if (cd->plx) {
		iounmap((void *) cd->plx);
		printd(("X400P-SS7: unmapped plx memory at %p\n", cd->plx));
		cd->plx = NULL;
	}
	xp_free_cd(cd);
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
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the X400-SDL driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the X400-SDL driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw xp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sdl_x400pinfo,
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
	if ((err = lis_register_strdev(major, &sdl_x400pinfo, UNITS, DRV_NAME)) < 0)
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
sdl_x400pterminate(void)
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
	if ((err = xp_pci_cleanup()))
		cmn_err(CE_WARN, "%s: could not cleanup pci device", DRV_NAME);
	if ((err = xp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
sdl_x400pinit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = xp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		sdl_x400pterminate();
		return (err);
	}
	if ((err = xp_pci_init())) {
		cmn_err(CE_WARN, "%s: no PCI devices found, err = %d", DRV_NAME, err);
		sdl_x400pterminate();
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
				sdl_x400pterminate();
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
module_init(sdl_x400pinit);
module_exit(sdl_x400pterminate);

#endif				/* LINUX */
