/*****************************************************************************

 @(#) $RCSfile: x400p-ss7.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/05/24 18:29:49 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/05/24 18:29:49 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: x400p-ss7.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/05/24 18:29:49 $"

static char const ident[] =
    "$RCSfile: x400p-ss7.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/05/24 18:29:49 $";

/*
 *  This is an SL (Signalling Link) kernel module which provides all of the
 *  capabilities of the SLI for the E400P-SS7 and T400P-SS7 cards.  This is a
 *  complete SS7 MTP Level 2 OpenSS7 implementation.
 */

#define X400P_DOWNLOAD_FIRMWARE

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>

#include <linux/errno.h>
#include <linux/types.h>

#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <linux/pci.h>

#include "debug.h"
#include "bufq.h"
#include "priv.h"
#include "lock.h"
#include "queue.h"
#include "allocb.h"
#include "timer.h"
#include "bufpool.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>

#ifdef X400P_DOWNLOAD_FIRMWARE
#include "x400p-ss7/x400pfw.h"		/* X400P-SS7 firmware load */
#endif

#define X400P_DESCRIP	"E/T400P-SS7: SS7/SL (Signalling Link) STREAMS DRIVER."
#define X400P_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define X400P_DEVICE	"Supports the T/E400P-SS7 T1/E1 PCI boards."
#define X400P_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define X400P_BANNER		X400P_DESCRIP	"\n" \
				X400P_COPYRIGHT	"\n" \
				X400P_DEVICE	"\n" \
				X400P_CONTACT	"\n"
#define X400P_LICENSE	"GPL"

MODULE_AUTHOR(X400P_CONTACT);
MODULE_DESCRIPTION(X400P_DESCRIP);
MODULE_SUPPORTED_DEVICE(X400P_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(X400P_LICENSE);
#endif

#define X400P_NMINOR 255

#define X400P_CMINOR_SDLI	0
#define X400P_CMINOR_CHI	1
#define X400P_CMINOR_MXI	2
#define X400P_CMINOR_LMI	3
#define X400P_CMINOR_FREE	4

#define XP_STYLE_SDLI	X400P_CMINOR_SDLI
#define XP_STYLE_CHI	X400P_CMINOR_CHI
#define XP_STYLE_MXI	X400P_CMINOR_MXI
#define XP_STYLE_LMI	X400P_CMINOR_LMI

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define XP_DRV_ID	SL_X400P_DRV_ID
#define XP_DRV_NAME	SL_X400P_DRV_NAME

STATIC struct module_info xp_rinfo = {
	mi_idnum:XP_DRV_ID,			/* Module ID number */
	mi_idname:XP_DRV_NAME "-rd",		/* Module name */
	mi_minpsz:1,				/* Min packet size accepted */
	mi_maxpsz:INFPSZ,			/* Max packet size accepted */
	mi_hiwat:1024,				/* Hi water mark */
	mi_lowat:0				/* Lo water mark */
};

STATIC struct module_info xp_winfo = {
	mi_idnum:XP_DRV_ID,			/* Module ID number */
	mi_idname:XP_DRV_NAME "-wr",		/* Module name */
	mi_minpsz:1,				/* Min packet size accepted */
	mi_maxpsz:INFPSZ,			/* Max packet size accepted */
	mi_hiwat:1024,				/* Hi water mark */
	mi_lowat:0,				/* Lo water mark */
};

STATIC int xp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int xp_close(queue_t *, int, cred_t *);

STATIC struct qinit xp_rinit = {
	qi_putp:ss7_oput,			/* Read put (message from below) */
	qi_srvp:ss7_osrv,			/* Read queue service */
	qi_qopen:xp_open,			/* Each open */
	qi_qclose:xp_close,			/* Last close */
	qi_minfo:&xp_rinfo,			/* Information */
};

STATIC struct qinit xp_winit = {
	qi_putp:ss7_iput,			/* Write put (message from above) */
	qi_srvp:ss7_isrv,			/* Write queue service */
	qi_minfo:&xp_winfo,			/* Information */
};

STATIC struct streamtab xp_info = {
	st_rdinit:&xp_rinit,			/* Upper read queue */
	st_wrinit:&xp_winit,			/* Upper write queue */
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

typedef struct xp {
	STR_DECLARATION (struct xp);		/* stream declaration */
	struct sp *sp;				/* span for this channel */
	int chan;				/* index (chan) */
	union {
		struct {
			sdl_timers_t timers;
			sdl_statem_t statem;
			sdl_notify_t notify;
			sdl_config_t config;
			sdl_stats_t stats;
			sdl_stats_t statsp;
		} sdl;
		struct {
			ch_statem_t statem;
			ch_notify_t notify;
			ch_config_t config;
			ch_stats_t stats;
			ch_stats_t statsp;
		} ch;
		struct {
			mx_statem_t statem;
			mx_notify_t notify;
			mx_config_t config;
			mx_stats_t stats;
			mx_stats_t statsp;
		} mx;
	} obj;
} xp_t;
#define XP_PRIV(__q) ((struct xp *)(__q)->q_ptr)

STATIC struct xp *xp_alloc_priv(queue_t *, struct xp **, dev_t *, cred_t *, ushort);
STATIC void xp_free_priv(struct xp *);
STATIC struct xp *xp_get(struct xp *);
STATIC void xp_put(struct xp *);

typedef struct xp_path {
	mblk_t *msg;				/* message */
	mblk_t *nxt;				/* message chain block */
	bufq_t buf;				/* buffer */
} xp_path_t;

typedef struct ts {
	HEAD_DECLARATION (struct ts);		/* head declaration */
	struct xp *xp;				/* interface for this slot */
	struct sp *sp;				/* span for this slot */
	int slot;				/* 32-bit backplane slot */
	struct xp_path rx;			/* tx path */
	struct xp_path tx;			/* rx path */
	sdl_config_t config;			/* slot configuration */
} ts_t;

STATIC struct ts *xp_alloc_ts(struct xp *, struct sp *, uint8_t);
STATIC void xp_free_ts(struct ts *);
STATIC struct ts *ts_get(struct ts *);
STATIC void ts_put(struct ts *);

typedef struct sp {
	HEAD_DECLARATION (struct sp);		/* head declaration */
	struct xp *xp;				/* interface for this span */
	struct cd *cd;				/* card for this span */
	struct ts *slots[32];			/* timeslot structures */
	ulong recovertime;			/* alarm recover time */
	ulong iobase;				/* span iobase */
	int span;				/* index (span) */
	volatile ulong loopcnt;			/* loop command count */
	sdl_config_t config;			/* span configuration */
} sp_t;

STATIC struct sp *xp_alloc_sp(struct cd *, uint8_t);
STATIC void xp_free_sp(struct sp *);
STATIC struct sp *sp_get(struct sp *);
STATIC void sp_put(struct sp *);

typedef struct cd {
	HEAD_DECLARATION (struct cd);		/* head declaration */
	ulong xll_region;			/* Xilinx 32-bit memory region */
	ulong xll_length;			/* Xilinx 32-bit memory length */
	volatile uint32_t *xll;			/* Xilinx 32-bit memory map */
	ulong xlb_region;			/* Xilinx 8-bit memory region */
	ulong xlb_length;			/* Xilinx 8-bit memory lenght */
	volatile uint8_t *xlb;			/* Xilinx 8-bit memory map */
	ulong plx_region;			/* PLX 9030 memory region */
	ulong plx_length;			/* PLX 9030 memory length */
	volatile uint16_t *plx;			/* PLX 9030 memory map */
	uint frame;				/* frame number */
	struct sp *spans[4];			/* structures for spans */
	uint32_t *wbuf;				/* wr buffer */
	uint32_t *rbuf;				/* rd buffer */
	volatile int uebno;			/* upper elastic buffer number */
	volatile int lebno;			/* lower elastic buffer number */
	volatile int eval_syncsrc;		/* need to reevaluate sync src */
	volatile int leds;			/* leds on the card */
	int card;				/* index (card) */
	ulong irq;				/* card irq */
	ulong iobase;				/* card iobase */
	struct tasklet_struct tasklet;		/* card tasklet */
	sdl_config_t config;			/* card configuration */
} cd_t;

STATIC struct cd *xp_alloc_cd(void);
STATIC void xp_free_cd(struct cd *);
STATIC struct cd *cd_get(struct cd *);
STATIC void cd_put(struct cd *);

STATIC struct ss7_bufpool xp_bufpool = { 0, };

/*
 *  ------------------------------------------------------------------------
 *
 *  Card Structures and Macros
 *
 *  ------------------------------------------------------------------------
 */
static const char *xp_t1_framer[] = {
	"DS2152",
	"DS21352",
	"DS21552",
	"DS21752",
};
static const char *xp_e1_framer[] = {
	"DS2154",
	"DS21354",
	"DS21554",
	"DS21754",
};

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

#if defined(__LITTLE_ENDIAN)
#define span_to_byte(__span) (3-(__span))
#elif defined(__BIG_ENDIAN)
#define span_to_byte(__span) (__span)
#else
#error "Must know the endianess of processor\n"
#endif

typedef enum {
	PLX9030 = 0,
	PLXDEVBRD,
	X400P,
	X400PSS7,
} xp_board_t;

/* indexed by xp_board_t above */

/* *INDENT-OFF* */
static struct {
	char *name;
	u32 hw_flags;
} xp_board_info[] __devinitdata = {
	{ "PLX 9030", 0 },
	{ "PLX Development Board", 0 },
	{ "X400P", 1 },
	{ "X400P-SS7", 1 },
};
/* *INDENT-ON* */

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
STATIC int xp_suspend(struct pci_dev *pdev, u32 state);
STATIC int xp_resume(struct pci_dev *pdev);
#endif

STATIC struct pci_driver xp_driver = {
	name:XP_DRV_NAME,
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
STATIC int m_error(queue_t *q, struct xp *xp, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		ss7_oput(xp->oq, mp);
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
STATIC INLINE int sdl_received_bits_ind(queue_t *q, struct xp *xp, mblk_t *dp)
{
	if (canputnext(xp->oq)) {
		ss7_oput(xp->oq, dp);
		return (QR_ABSORBED);
	}
	rare();
	dp->b_wptr = dp->b_rptr;	/* discard contents */
	return (-EBUSY);
}

/*
 *  SDL_DISCONNECT_IND
 *  -----------------------------------
 */
STATIC INLINE int sdl_disconnect_ind(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	sdl_disconnect_ind_t *p;
	(void) xp;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
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
STATIC INLINE int lmi_info_ack(queue_t *q, struct xp *xp, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = xp->i_state;
		p->lmi_max_sdu = 0;	/* FIXME: fill these out */
		p->lmi_min_sdu = 0;	/* FIXME: fill these out */
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
STATIC INLINE int lmi_ok_ack(queue_t *q, struct xp *xp, ulong state, long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
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
STATIC INLINE int lmi_error_ack(queue_t *q, struct xp *xp, ulong state, long prim, ulong errno, ulong reason)
{
	mblk_t *mp;
	lmi_error_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
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
STATIC INLINE int lmi_enable_con(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
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
STATIC INLINE int lmi_disable_con(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
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
STATIC INLINE int lmi_optmgmt_ack(queue_t *q, struct xp *xp, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
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
STATIC INLINE int lmi_error_ind(queue_t *q, struct xp *xp, ulong errno, ulong reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
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
STATIC INLINE int lmi_stats_ind(queue_t *q, struct xp *xp, ulong interval)
{
	if (canputnext(xp->oq)) {
		mblk_t *mp;
		lmi_stats_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
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
STATIC INLINE int lmi_event_ind(queue_t *q, struct xp *xp, ulong oid, ulong level)
{
	if (canputnext(xp->oq)) {
		mblk_t *mp;
		lmi_event_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
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
STATIC sdl_config_t sdl_default_e1_chan = {
	ifname:NULL,
	ifflags:0,
	iftype:SDL_TYPE_DS0,
	ifrate:64000,
	ifgtype:SDL_GTYPE_E1,
	ifgrate:2048000,
	ifmode:SDL_MODE_PEER,
	ifgmode:SDL_GMODE_NONE,
	ifgcrc:SDL_GCRC_CRC5,
	ifclock:SDL_CLOCK_SLAVE,
	ifcoding:SDL_CODING_HDB3,
	ifframing:SDL_FRAMING_CCS,
	ifblksize:8,
	ifleads:0,
	ifbpv:0,
	ifalarms:0,
	ifrxlevel:0,
	iftxlevel:0,
	ifsync:0,
	ifsyncsrc:{0, 0, 0, 0}
};
STATIC sdl_config_t sdl_default_t1_chan = {
	ifname:NULL,
	ifflags:0,
	iftype:SDL_TYPE_DS0,
	ifrate:64000,
	ifgtype:SDL_GTYPE_T1,
	ifgrate:1544000,
	ifmode:SDL_MODE_PEER,
	ifgmode:SDL_GMODE_NONE,
	ifgcrc:SDL_GCRC_CRC6,
	ifclock:SDL_CLOCK_SLAVE,
	ifcoding:SDL_CODING_AMI,
	ifframing:SDL_FRAMING_SF,
	ifblksize:8,
	ifleads:0,
	ifbpv:0,
	ifalarms:0,
	ifrxlevel:0,
	iftxlevel:0,
	ifsync:0,
	ifsyncsrc:{0, 0, 0, 0}
};
/*
 *  ------------------------------------------------------------------------
 *
 *  Timers
 *
 *  ------------------------------------------------------------------------
 */

/*
 *  ========================================================================
 *
 *  Events from below
 *
 *  ========================================================================
 */

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
STATIC int sdl_m_data(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	struct sp *sp;
	struct ts *ts;
	int chan, slot;
	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	if (!(sp = xp->sp))
		goto efault;
	if (!(chan = xp->chan))
		goto efault;
	if (xp->obj.sdl.config.ifgtype == SDL_GTYPE_E1) {
		if (chan < 1 || chan > 31)
			goto efault;
		slot = xp_e1_chan_map[chan];
	} else {
		if (chan < 1 || chan > 24)
			goto efault;
		slot = xp_t1_chan_map[chan];
	}
	if (!(ts = sp->slots[slot]))
		goto efault;
	if (ts->tx.buf.q_count > 64)
		goto ebusy;
	bufq_queue(&ts->tx.buf, mp);
	switch (xp->obj.sdl.config.iftype) {
	case SDL_TYPE_E1:
		if (++xp->chan > 31)
			xp->chan = 1;
		break;
	case SDL_TYPE_T1:
		if (++xp->chan > 24)
			xp->chan = 1;
		break;
	}
	return (QR_ABSORBED);
      ebusy:
	return (-EBUSY);
      efault:
	return m_error(q, xp, EFAULT);
      eproto:
	return m_error(q, xp, EPROTO);
}

/*
 *  SDL_BITS_FOR_TRANSMISSION_REQ
 *  -----------------------------------
 *  Non-preferred method.  Normally one should just send M_DATA blocks.  We
 *  just strip off the redundant M_PROTO and put it on the queue.
 */
STATIC int sdl_bits_for_transmission_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (QR_STRIP);
}

/*
 *  SDL_CONNECT_REQ
 *  -----------------------------------
 */
STATIC int sdl_connect_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;
	sdl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto eproto;
	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	lis_spin_lock_irqsave(&xp->lock, &flags);
	{
		if (p->sdl_flags & SDL_RX_DIRECTION) {
			xp->obj.sdl.config.ifflags |= SDL_IF_RX_RUNNING;
			xp->obj.sdl.statem.rx_state = SDL_STATE_IN_SERVICE;
			ptrace(("%p: Enabling Rx\n", xp));
		}
		if (p->sdl_flags & SDL_TX_DIRECTION) {
			xp->obj.sdl.config.ifflags |= SDL_IF_TX_RUNNING;
			xp->obj.sdl.statem.tx_state = SDL_STATE_IN_SERVICE;
			ptrace(("%p: Enabling Tx\n", xp));
		}
		if ((xp->obj.sdl.config.ifflags & (SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING))) {
			ptrace(("%p: Marking interface up\n", xp));
			xp->obj.sdl.config.ifflags |= SDL_IF_UP;
		}
	}
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	return (QR_DONE);
      eproto:
	return m_error(q, xp, EPROTO);
}

/*
 *  SDL_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC int sdl_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;
	sdl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto eproto;
	if (xp->i_state != LMI_ENABLED)
		goto eproto;
	lis_spin_lock_irqsave(&xp->lock, &flags);
	{
		if (p->sdl_flags & SDL_RX_DIRECTION) {
			xp->obj.sdl.config.ifflags &= ~SDL_IF_RX_RUNNING;
			xp->obj.sdl.statem.rx_state = SDL_STATE_IDLE;
			ptrace(("%p: Disabling Rx\n", xp));
		}
		if (p->sdl_flags & SDL_TX_DIRECTION) {
			xp->obj.sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
			xp->obj.sdl.statem.tx_state = SDL_STATE_IDLE;
			ptrace(("%p: Disabling Tx\n", xp));
		}
		if (!(xp->obj.sdl.config.ifflags & (SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING))) {
			ptrace(("%p: Marking interface down\n", xp));
			xp->obj.sdl.config.ifflags &= ~SDL_IF_UP;
		}
	}
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	return (QR_DONE);
      eproto:
	return m_error(q, xp, EPROTO);
}

/* 
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC int lmi_info_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	switch (xp->i_state) {
	case LMI_ENABLED:
	case LMI_DISABLED:
	{
		struct sp *sp;
		struct cd *cd;
		if ((sp = xp->sp) && (cd = sp->cd)) {
			uint16_t ppa = (xp->chan & 0xff) | ((sp->span & 0x0f) << 8) | ((cd->card & 0x0f) << 12);
			return lmi_info_ack(q, xp, (caddr_t) & ppa, sizeof(ppa));
		}
	}
	}
	return lmi_info_ack(q, xp, NULL, 0);
}

/* 
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC int lmi_attach_req(queue_t *q, mblk_t *mp)
{
	psw_t flags = 0;
	int err, card, span, chan, slot;
	struct cd *cd;
	struct sp *sp = NULL;
	uint16_t ppa;
	struct xp *xp = XP_PRIV(q);
	lmi_attach_req_t *p = ((typeof(p)) mp->b_rptr);
	if (mp->b_wptr - mp->b_rptr < sizeof(*p) + sizeof(ppa)) {
		ptrace(("%s: ERROR: primitive too small = %d bytes\n", XP_DRV_NAME, mp->b_wptr - mp->b_rptr));
		goto lmi_badprim;
	}
	if (xp->i_state != LMI_UNATTACHED) {
		ptrace(("%s: ERROR: interface out of state\n", XP_DRV_NAME));
		goto lmi_outstate;
	}
	xp->i_state = LMI_ATTACH_PENDING;
	ppa = *(typeof(ppa) *) (p + 1);
	/* check card */
	card = (ppa >> 12) & 0x0f;
	for (cd = x400p_cards; cd && cd->card != card; cd = cd->next) ;
	if (!cd) {
		ptrace(("%s: ERROR: invalid card %d\n", XP_DRV_NAME, card));
		goto lmi_badppa;
	}
	/* check span */
	span = (ppa >> 8) & 0x0f;
	if (span < 0 || span > X400_SPANS - 1) {
		ptrace(("%s: ERROR: invalid span %d\n", XP_DRV_NAME, span));
		goto lmi_badppa;
	}
	if (!(sp = cd->spans[span])) {
		ptrace(("%s: ERROR: unallocated span %d\n", XP_DRV_NAME, span));
		goto lmi_badppa;
	}
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
				ptrace(("%s: ERROR: invalid chan %d\n", XP_DRV_NAME, chan));
				goto lmi_badppa;
			}
			slot = xp_e1_chan_map[chan - 1];
			if (sp->slots[slot]) {
				ptrace(("%s: ERROR: slot %d in use\n", XP_DRV_NAME, slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(q, xp, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("%s: attaching card %d, span %d, chan %d, slot %d\n", XP_DRV_NAME, card, span,
				chan, slot));
			lis_spin_lock_irqsave(&xp->lock, &flags);
			{
				sp->slots[slot] = ts_get(xp_alloc_ts(xp, sp, slot));
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_DS0;
				sp->config.ifrate = 64000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 8;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				/* SDL configuration defaults */
				xp->obj.sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->obj.sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->obj.sdl.config = sp->config;
				xp->obj.sdl.config.ifflags = 0;
			}
			lis_spin_unlock_irqrestore(&xp->lock, &flags);
			return (QR_DONE);
		case SDL_GTYPE_T1:
			if (chan < 1 || chan > 24) {
				ptrace(("%s: ERROR: invalid chan %d\n", XP_DRV_NAME, chan));
				goto lmi_badppa;
			}
			slot = xp_t1_chan_map[chan - 1];
			if (sp->slots[slot]) {
				ptrace(("%s: ERROR: slot %d in use\n", XP_DRV_NAME, slot));
				goto lmi_badppa;
			}
			if ((err = lmi_ok_ack(q, xp, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("%s: attaching card %d, span %d, chan %d, slot %d\n", XP_DRV_NAME, card, span,
				chan, slot));
			lis_spin_lock_irqsave(&xp->lock, &flags);
			{
				sp->slots[slot] = ts_get(xp_alloc_ts(xp, sp, slot));
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_DS0;
				sp->config.ifrate = 64000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 8;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				/* SDL configuration defaults */
				xp->obj.sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->obj.sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->obj.sdl.config = sp->config;
				xp->obj.sdl.config.ifflags = 0;
			}
			lis_spin_unlock_irqrestore(&xp->lock, &flags);
			return (QR_DONE);
		}
	} else {
		int c;
		/* entire span indicated */
		switch (cd->config.ifgtype) {
		case SDL_GTYPE_E1:
			for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]); c++)
				if (sp->slots[xp_e1_chan_map[c]]) {
					ptrace(("%s: ERROR: slot in use for chan %d\n", XP_DRV_NAME, c));
					goto lmi_badppa;
				}
			if ((err = lmi_ok_ack(q, xp, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			printd(("%s: attaching card %d, entire span %d\n", XP_DRV_NAME, card, span));
			lis_spin_lock_irqsave(&xp->lock, &flags);
			{
				for (c = 0; c < sizeof(xp_e1_chan_map) / sizeof(xp_e1_chan_map[0]); c++) {
					slot = xp_e1_chan_map[c];
					sp->slots[slot] = ts_get(xp_alloc_ts(xp, sp, slot));
				}
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_E1;
				sp->config.ifrate = 2048000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 64;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				/* SDL configuration defaults */
				xp->obj.sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->obj.sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->obj.sdl.config = sp->config;
				xp->obj.sdl.config.ifflags = 0;
			}
			lis_spin_unlock_irqrestore(&xp->lock, &flags);
			return (QR_DONE);
		case SDL_GTYPE_T1:
			for (c = 0; c < (sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0])); c++)
				if (sp->slots[xp_t1_chan_map[c]]) {
					ptrace(("%s: ERROR: slot in use for chan %d\n", XP_DRV_NAME, c));
					goto lmi_badppa;
				}
			if ((err = lmi_ok_ack(q, xp, LMI_DISABLED, LMI_ATTACH_REQ)))
				return (err);
			/* commit attach */
			lis_spin_lock_irqsave(&xp->lock, &flags);
			{
				printd(("%s: attaching card %d, entire span %d\n", XP_DRV_NAME, card, span));
				for (c = 0; c < (sizeof(xp_t1_chan_map) / sizeof(xp_t1_chan_map[0])); c++) {
					slot = xp_t1_chan_map[c];
					sp->slots[slot] = ts_get(xp_alloc_ts(xp, sp, slot));
				}
				xp->sp = sp_get(sp);
				sp->config.iftype = SDL_TYPE_T1;
				sp->config.ifrate = 1544000;
				sp->config.ifmode = SDL_MODE_PEER;
				sp->config.ifblksize = 64;
				xp->i_state = LMI_DISABLED;
				xp->chan = chan;
				/* SDL configuration defaults */
				xp->obj.sdl.statem.rx_state = SDL_STATE_IDLE;
				xp->obj.sdl.statem.tx_state = SDL_STATE_IDLE;
				/* inherit SDL configuration from span */
				xp->obj.sdl.config = sp->config;
				xp->obj.sdl.config.ifflags = 0;
			}
			lis_spin_unlock_irqrestore(&xp->lock, &flags);
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
		return lmi_error_ack(q, xp, LMI_UNATTACHED, LMI_ATTACH_REQ, errno, reason);
	}
}

/* 
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC int lmi_detach_req(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	struct sp *sp;
	struct cd *cd;
	int err, slot;
	psw_t flags = 0;
	/* validate detach */
	if (xp->i_state != LMI_DISABLED)
		return lmi_error_ack(q, xp, xp->i_state, LMI_DETACH_REQ, 0, LMI_OUTSTATE);
	if (!(sp = xp->sp) || !(cd = sp->cd)) {
		swerr();
		return m_error(q, xp, EFAULT);
	}
	xp->i_state = LMI_DETACH_PENDING;
	if ((err = lmi_ok_ack(q, xp, LMI_UNATTACHED, LMI_DETACH_REQ)))
		return (err);
	/* commit detach */
	lis_spin_lock_irqsave(&xp->lock, &flags);
	{
		for (slot = 0; slot < 32; slot++)
			if (sp->slots[slot] && sp->slots[slot]->xp == xp)
				xp_free_ts(xchg(&sp->slots[slot], NULL));
		sp_put(xchg(&xp->sp, NULL));
		xp->chan = 0;
	}
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	return (QR_DONE);
}

/* 
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC int lmi_enable_req(queue_t *q, mblk_t *mp)
{
	int err, offset;
	struct xp *xp = XP_PRIV(q);
	struct cd *cd;
	struct sp *sp;
	/* validate enable */
	if (xp->i_state != LMI_DISABLED) {
		ptrace(("%s: ERROR: out of state: state = %ld\n", XP_DRV_NAME, xp->i_state));
		goto lmi_outstate;
	}
	if (!(sp = xp->sp)) {
		ptrace(("%s: ERROR: out of state: no span pointer\n", XP_DRV_NAME));
		goto lmi_outstate;
	}
	if (!(cd = sp->cd)) {
		ptrace(("%s: ERROR: out of state: no card pointer\n", XP_DRV_NAME));
		goto lmi_outstate;
	}
#ifdef _DEBUG
	if (cd->config.ifgtype != SDL_GTYPE_E1 && cd->config.ifgtype != SDL_GTYPE_T1) {
		ptrace(("%s: ERROR: card group type = %lu\n", XP_DRV_NAME, cd->config.ifgtype));
		return m_error(q, xp, EFAULT);
	}
#endif
	if (xp->obj.sdl.config.ifflags & SDL_IF_UP) {
		ptrace(("%s: ERROR: out of state: device already up\n", XP_DRV_NAME));
		goto lmi_outstate;
	}
	if ((err = lmi_enable_con(q, xp)))
		return (err);
	/* commit enable */
	printd(("%s: performing enable\n", XP_DRV_NAME));
	xp->i_state = LMI_ENABLE_PENDING;
	xp->obj.sdl.config.ifname = sp->config.ifname;
	xp->obj.sdl.config.ifflags |= SDL_IF_UP;
	xp->obj.sdl.config.iftype = xp->obj.sdl.config.iftype;
	switch (xp->obj.sdl.config.iftype) {
	case SDL_TYPE_E1:
		xp->obj.sdl.config.ifrate = 2048000;
		xp->obj.sdl.config.ifblksize = 64;
		break;
	case SDL_TYPE_T1:
		xp->obj.sdl.config.ifrate = 1544000;
		xp->obj.sdl.config.ifblksize = 64;
		break;
	case SDL_TYPE_DS0:
		xp->obj.sdl.config.ifrate = 64000;
		xp->obj.sdl.config.ifblksize = 8;
		break;
	case SDL_TYPE_DS0A:
		xp->obj.sdl.config.ifrate = 56000;
		xp->obj.sdl.config.ifblksize = 8;
		break;
	}
	xp->obj.sdl.config.ifgtype = sp->config.ifgtype;
	xp->obj.sdl.config.ifgrate = sp->config.ifgrate;
	xp->obj.sdl.config.ifmode = sp->config.ifmode;
	xp->obj.sdl.config.ifgmode = sp->config.ifgmode;
	xp->obj.sdl.config.ifgcrc = sp->config.ifgcrc;
	xp->obj.sdl.config.ifclock = sp->config.ifclock;
	xp->obj.sdl.config.ifcoding = sp->config.ifcoding;
	xp->obj.sdl.config.ifframing = sp->config.ifframing;
	xp->obj.sdl.config.ifblksize = xp->obj.sdl.config.ifblksize;
	xp->obj.sdl.config.ifleads = sp->config.ifleads;
	xp->obj.sdl.config.ifbpv = sp->config.ifbpv;
	xp->obj.sdl.config.ifalarms = sp->config.ifalarms;
	xp->obj.sdl.config.ifrxlevel = sp->config.ifrxlevel;
	xp->obj.sdl.config.iftxlevel = sp->config.iftxlevel;
	xp->obj.sdl.config.ifsync = cd->config.ifsync;
	xp->i_state = LMI_ENABLED;
	if (!(sp->config.ifflags & SDL_IF_UP)) {
		/* need to bring up span */
		int span = sp->span;
		int base = span << 8;
		uint8_t ccr1 = 0, tcr1 = 0;
		unsigned long timeout;
		switch (cd->config.ifgtype) {
		case SDL_GTYPE_E1:
		{
			psw_t flags = 0;
			printd(("%s: performing enable on E1 span %d\n", XP_DRV_NAME, span));
			lis_spin_lock_irqsave(&cd->lock, &flags);
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
			lis_spin_unlock_irqrestore(&cd->lock, &flags);
			while (jiffies < timeout) ;
			lis_spin_lock_irqsave(&cd->lock, &flags);
			cd->xlb[base + 0x1b] = 0x9a;	/* CRC3: set ESR as well */
			cd->xlb[base + 0x1b] = 0x82;	/* CRC3: TSCLKM only */
			sp->config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
			/* enable interrupts */
			cd->xlb[CTLREG] = (INTENA | E1DIV);
			lis_spin_unlock_irqrestore(&cd->lock, &flags);
			break;
		}
		case SDL_GTYPE_T1:
		{
			int byte, val, c;
			unsigned short mask = 0;
			psw_t flags = 0;
			printd(("%s: performing enable on T1 span %d\n", XP_DRV_NAME, span));
			lis_spin_lock_irqsave(&cd->lock, &flags);
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
			cd->xlb[base + 0x37] = 0x9c;	/* Tx & Rx Elastic stor, sysclk(s) = 2.048 mhz, loopback
							   controls (CCR1) */
			cd->xlb[base + 0x12] = 0x22;	/* Set up received loopup and loopdown codes */
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
			lis_spin_unlock_irqrestore(&cd->lock, &flags);
			while (jiffies < timeout) ;
			lis_spin_lock_irqsave(&cd->lock, &flags);
			cd->xlb[base + 0x0a] = 0x30;	/* LIRST bask to normal, Resetting elastic buffers */
			sp->config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
			/* enable interrupts */
			cd->xlb[CTLREG] = (INTENA);
			lis_spin_unlock_irqrestore(&cd->lock, &flags);
			/* establish which channels are clear channel */
			for (c = 0; c < 24; c++) {
				int slot = xp_t1_chan_map[c];
				byte = c >> 3;
				if (!cd->spans[span]->slots[slot] || !cd->spans[span]->slots[slot]->xp ||
				    cd->spans[span]->slots[slot]->xp->obj.sdl.config.iftype != SDL_TYPE_DS0A)
					mask |= 1 << (c % 8);
				if ((c % 8) == 7)
					cd->xlb[base + 0x39 + byte] = mask;
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
	return lmi_error_ack(q, xp, xp->i_state, LMI_ENABLE_REQ, 0, LMI_OUTSTATE);
}

/* 
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC int lmi_disable_req(queue_t *q, mblk_t *mp)
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
	if ((err = lmi_disable_con(q, xp)))
		return (err);
	/* commit disable */
	if ((sp = xp->sp) && (cd = sp->cd)) {
		psw_t flags = 0;
		lis_spin_lock_irqsave(&cd->lock, &flags);
		{
			int slot, boff;
			uchar idle = (cd->config.ifgtype == SDL_GTYPE_T1) ? 0x7f : 0xff;
			uchar *base = (uchar *) cd->wbuf + span_to_byte(sp->span);
			for (slot = 0; slot < 32; slot++)
				if (sp->slots[slot] && sp->slots[slot]->xp == xp)
					for (boff = 0; boff < X400P_EBUFNO; boff++)
						*(base + (boff << 10) + (slot << 2)) = idle;
			/* stop transmitters and receivers */
			xp->obj.sdl.config.ifflags = 0;
			xp->obj.sdl.statem.tx_state = SDL_STATE_IDLE;
			xp->obj.sdl.statem.rx_state = SDL_STATE_IDLE;
		}
		lis_spin_unlock_irqrestore(&cd->lock, &flags);
	} else
		swerr();
	return (QR_DONE);
      lmi_outstate:
	return lmi_error_ack(q, xp, xp->i_state, LMI_DISABLE_REQ, 0, LMI_OUTSTATE);
}

/* 
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	fixme(("FIXME: must check for options change\n"));
	return (QR_PASSALONG);
}

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int ch_m_data(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CH_DATA_REQ
 *  -----------------------------------
 *  Non-preferred method for sending data.  We should send just M_DATA blocks.
 */
STATIC int ch_data_req(queue_t *q, mblk_t *mp)
{
	return (QR_STRIP);
}

/*
 *  CH_INFO_REQ
 *  -----------------------------------
 */
STATIC int ch_info_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CH_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int ch_optmgmt_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CH_ATTACH_REQ
 *  -----------------------------------
 */
STATIC int ch_attach_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CH_ENABLE_REQ
 *  -----------------------------------
 */
STATIC int ch_enable_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CH_CONNECT_REQ
 *  -----------------------------------
 */
STATIC int ch_connect_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CH_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC int ch_disconnect_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CH_DISABLE_REQ
 *  -----------------------------------
 */
STATIC int ch_disable_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CH_DETACH_REQ
 *  -----------------------------------
 */
STATIC int ch_detach_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int mx_m_data(queue_t *q, mblk_t *mp)
{
	return ch_m_data(q, mp);
}

/*
 *  MX_DATA_REQ
 *  -----------------------------------
 */
STATIC int mx_data_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MX_INFO_REQ
 *  -----------------------------------
 */
STATIC int mx_info_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MX_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int mx_optmgmt_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MX_ATTACH_REQ
 *  -----------------------------------
 */
STATIC int mx_attach_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MX_ENABLE_REQ
 *  -----------------------------------
 */
STATIC int mx_enable_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MX_CONNECT_REQ
 *  -----------------------------------
 */
STATIC int mx_connect_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MX_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC int mx_disconnect_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MX_DISABLE_REQ
 *  -----------------------------------
 */
STATIC int mx_disable_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MX_DETACH_REQ
 *  -----------------------------------
 */
STATIC int mx_detach_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int lm_m_data(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
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
 *  Test SDL configuration settings
 *
 *  -------------------------------------------------------------------------
 */
STATIC int sdl_test_config(struct xp *xp, sdl_config_t * arg)
{
	int ret = 0;
	psw_t flags = 0;
	if (!xp)
		return (-EFAULT);
	lis_spin_lock_irqsave(&xp->lock, &flags);
	do {
		if (arg->ifflags) {
			ret = -EINVAL;
			break;
		}
		switch (arg->iftype) {
		case SDL_TYPE_NONE:	/* unknown/unspecified */
			arg->iftype = xp->obj.sdl.config.iftype;
			break;
		case SDL_TYPE_DS0:	/* DS0 channel */
		case SDL_TYPE_DS0A:	/* DS0A channel */
			/* yes we allow DS0A on E1 */
			break;
		case SDL_TYPE_E1:	/* full E1 span */
			if (xp->sp->config.ifgtype != SDL_GTYPE_E1) {
				ret = (-EINVAL);
				break;
			}
			break;
		case SDL_TYPE_T1:	/* full T1 span */
			if (xp->sp->config.ifgtype != SDL_GTYPE_T1) {
				ret = (-EINVAL);
				break;
			}
			break;
		default:
			ret = (-EINVAL);
			break;
		}
		if (ret)
			break;
		switch (arg->ifmode) {
		case SDL_MODE_NONE:	/* */
			arg->ifmode = xp->obj.sdl.config.ifmode;
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
			ret = (-EINVAL);
			break;
		}
		switch (arg->ifgmode) {
		case SDL_GMODE_NONE:
		case SDL_GMODE_LOC_LB:
			break;
		case SDL_GMODE_REM_LB:
		default:
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
			case SDL_GTYPE_T1:	/* */
			case SDL_GTYPE_E1:	/* */
				if (arg->ifgtype != xp->sp->config.ifgtype) {
					ret = (-EINVAL);
					break;
				}
				break;
			default:
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
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_GCRC_CRC4:	/* */
				if (arg->ifgtype != SDL_GTYPE_E1) {
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_GCRC_CRC5:	/* */
				if (arg->ifgtype != SDL_GTYPE_E1) {
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_GCRC_CRC6:	/* */
				if (arg->ifgtype != SDL_GTYPE_T1) {
					ret = (-EINVAL);
					break;
				}
				break;
			default:
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
				if (arg->ifgtype != SDL_GTYPE_T1) {
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_CODING_HDB3:	/* */
				if (arg->ifgtype != SDL_GTYPE_E1) {
					ret = (-EINVAL);
					break;
				}
				break;
			default:
			case SDL_CODING_B6ZS:	/* */
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
					ret = (-EINVAL);
					break;
				}
				break;
			case SDL_FRAMING_SF:	/* */
			case SDL_FRAMING_ESF:	/* */
				if (arg->ifgtype != SDL_GTYPE_T1) {
					ret = (-EINVAL);
					break;
				}
				break;
			default:
				ret = (-EINVAL);
				break;
			}
			if (ret)
				break;
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
				ret = (-EINVAL);
				break;
			}
			if (ret)
				break;
			if (xp->sp->cd) {
				int src;
				for (src = 0; src < SDL_SYNCS; src++)
					if (arg->ifsyncsrc[src] < 0 || arg->ifsyncsrc[src] > 4) {
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
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	return (ret);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Commit SDL configuration settings
 *
 *  -------------------------------------------------------------------------
 */
STATIC void sdl_commit_config(struct xp *xp, sdl_config_t * arg)
{
	int chan_reconfig = 0, span_reconfig = 0, card_reconfig = 0;
	struct sp *sp = NULL;
	struct cd *cd = NULL;
	psw_t flags = 0;
	if (!xp)
		return;
	lis_spin_lock_irqsave(&xp->lock, &flags);
	{
		if (xp->obj.sdl.config.iftype != arg->iftype) {
			xp->obj.sdl.config.iftype = arg->iftype;
			chan_reconfig = 1;
		}
		switch (arg->iftype) {
		case SDL_TYPE_DS0A:
			xp->obj.sdl.config.ifrate = 56000;
			break;
		case SDL_TYPE_DS0:
			xp->obj.sdl.config.ifrate = 64000;
			break;
		case SDL_TYPE_T1:
			xp->obj.sdl.config.ifrate = 1544000;
			break;
		case SDL_TYPE_E1:
			xp->obj.sdl.config.ifrate = 2048000;
			break;
		}
		if (xp->obj.sdl.config.ifrate != arg->ifrate) {
			xp->obj.sdl.config.ifrate = arg->ifrate;
			chan_reconfig = 1;
		}
		if (xp->obj.sdl.config.ifmode != arg->ifmode) {
			xp->obj.sdl.config.ifmode = arg->ifmode;
			chan_reconfig = 1;
		}
		if ((sp = xp->sp)) {
			int slot;
			if (sp->config.ifgcrc != arg->ifgcrc) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot] && sp->slots[slot]->xp)
						sp->slots[slot]->xp->obj.sdl.config.ifgcrc = arg->ifgcrc;
				sp->config.ifgcrc = arg->ifgcrc;
				span_reconfig = 1;
			}
			if (sp->config.ifgmode != arg->ifgmode) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot] && sp->slots[slot]->xp)
						sp->slots[slot]->xp->obj.sdl.config.ifgmode = arg->ifgmode;
				sp->config.ifgmode = arg->ifgmode;
				span_reconfig = 1;
			}
			if (sp->config.ifclock != arg->ifclock) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot] && sp->slots[slot]->xp)
						sp->slots[slot]->xp->obj.sdl.config.ifclock = arg->ifclock;
				sp->config.ifclock = arg->ifclock;
				span_reconfig = 1;
			}
			if (sp->config.ifcoding != arg->ifcoding) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot] && sp->slots[slot]->xp)
						sp->slots[slot]->xp->obj.sdl.config.ifcoding = arg->ifcoding;
				sp->config.ifcoding = arg->ifcoding;
				span_reconfig = 1;
			}
			if (sp->config.ifframing != arg->ifframing) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot] && sp->slots[slot]->xp)
						sp->slots[slot]->xp->obj.sdl.config.ifframing = arg->ifframing;
				sp->config.ifframing = arg->ifframing;
				span_reconfig = 1;
			}
			if (sp->config.iftxlevel != arg->iftxlevel) {
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot] && sp->slots[slot]->xp)
						sp->slots[slot]->xp->obj.sdl.config.iftxlevel = arg->iftxlevel;
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
									if (cd->spans[span]->slots[slot] &&
									    cd->spans[span]->slots[slot]->xp)
										cd->spans[span]->slots[slot]->xp->
										    obj.sdl.config.
										    ifsyncsrc[src] =
										    arg->ifsyncsrc[src];
						cd->config.ifsyncsrc[src] = arg->ifsyncsrc[src];
						card_reconfig = 1;
					}
				}
			}
		}
		if (sp && span_reconfig && sp->config.ifflags & SDL_IF_UP) {
			/* need to bring up span */
			int span = sp->span;
			int base = span << 8;
			uint8_t ccr1 = 0, tcr1 = 0;
			if (cd) {
				switch (cd->config.ifgtype) {
				case SDL_GTYPE_E1:
				{
					printd(("%s: performing reconfiguration of E1 span %d\n", XP_DRV_NAME,
						span));
					/* Tell ISR to re-evaluate the sync source */
					cd->eval_syncsrc = 1;
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
					case SDL_CODING_HDB3:
						ccr1 |= 0x44;
						break;
					default:
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
					break;
				}
				case SDL_GTYPE_T1:
				{
					int byte, val, c;
					unsigned short mask = 0;
					printd(("%s: performing reconfiguration of T1 span %d\n", XP_DRV_NAME,
						span));
					/* Tell ISR to re-evaluate the sync source */
					cd->eval_syncsrc = 1;
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
					/* establish which channels are clear channel */
					for (c = 0; c < 24; c++) {
						byte = c >> 3;
						if (!cd->spans[span]->slots[xp_t1_chan_map[c]] ||
						    !cd->spans[span]->slots[xp_t1_chan_map[c]]->xp
						    || cd->spans[span]->slots[xp_t1_chan_map[c]]->xp->obj.sdl.
						    config.iftype != SDL_TYPE_DS0A)
							mask |= 1 << (c % 8);
						if ((c % 8) == 7)
							cd->xlb[base + 0x39 + byte] = mask;
					}
					break;
				}
				default:
					swerr();
					break;
				}
			}
		}
		if (cd && card_reconfig && cd->config.ifflags & SDL_IF_UP) {
			cd->eval_syncsrc = 1;
		}
	}
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	return;
}

/* 
 *  SDL_IOCGOPTIONS:    lmi_option_t
 *  -----------------------------------
 */
STATIC int sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/* 
 *  SDL_IOCSOPTIONS:    lmi_option_t
 *  -----------------------------------
 */
STATIC int sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/* 
 *  SDL_IOCGCONFIG:     sdl_config_t
 *  -----------------------------------
 */
STATIC int sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		struct sp *sp;
		psw_t flags = 0;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		bzero(arg, sizeof(*arg));
		lis_spin_lock_irqsave(&xp->lock, &flags);
		{
			arg->ifflags = xp->obj.sdl.config.ifflags;
			arg->iftype = xp->obj.sdl.config.iftype;
			arg->ifrate = xp->obj.sdl.config.ifrate;
			arg->ifmode = xp->obj.sdl.config.ifmode;
			arg->ifblksize = xp->obj.sdl.config.ifblksize;
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
		lis_spin_unlock_irqrestore(&xp->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCSCONFIG:     sdl_config_t
 *  -----------------------------------
 */
STATIC int sdl_iocsconfig(queue_t *q, mblk_t *mp)
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
STATIC int sdl_ioctconfig(queue_t *q, mblk_t *mp)
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
STATIC int sdl_ioccconfig(queue_t *q, mblk_t *mp)
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
STATIC int sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		psw_t flags = 0;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&xp->lock, &flags);
		{
			*arg = xp->obj.sdl.statem;
		}
		lis_spin_unlock_irqrestore(&xp->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCMRESET:     sdl_statem_t
 *  -----------------------------------
 */
STATIC int sdl_ioccmreset(queue_t *q, mblk_t *mp)
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
STATIC int sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		psw_t flags = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&xp->lock, &flags);
		{
			*arg = xp->obj.sdl.statsp;
		}
		lis_spin_unlock_irqrestore(&xp->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCSSTATSP:     sdl_stats_t
 *  -----------------------------------
 */
STATIC int sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		psw_t flags = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		fixme(("FIXME: check these settings\n"));
		lis_spin_lock_irqsave(&xp->lock, &flags);
		{
			xp->obj.sdl.statsp = *arg;
		}
		lis_spin_unlock_irqrestore(&xp->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCGSTATS:      sdl_stats_t
 *  -----------------------------------
 */
STATIC int sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		psw_t flags = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&xp->lock, &flags);
		{
			*arg = xp->obj.sdl.stats;
		}
		lis_spin_unlock_irqrestore(&xp->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCSTATS:      sdl_stats_t
 *  -----------------------------------
 */
STATIC int sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags = 0;
	(void) mp;
	lis_spin_lock_irqsave(&xp->lock, &flags);
	{
		bzero(&xp->obj.sdl.stats, sizeof(xp->obj.sdl.stats));
	}
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	return (0);
}

/* 
 *  SDL_IOCGNOTIFY:     sdl_notify_t
 *  -----------------------------------
 */
STATIC int sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		psw_t flags = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&xp->lock, &flags);
		{
			*arg = xp->obj.sdl.notify;
		}
		lis_spin_unlock_irqrestore(&xp->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCSNOTIFY:     sdl_notify_t
 *  -----------------------------------
 */
STATIC int sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		psw_t flags = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&xp->lock, &flags);
		{
			xp->obj.sdl.notify.events |= arg->events;
		}
		lis_spin_unlock_irqrestore(&xp->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCNOTIFY:     sdl_notify_t
 *  -----------------------------------
 */
STATIC int sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct xp *xp = XP_PRIV(q);
		psw_t flags = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&xp->lock, &flags);
		{
			xp->obj.sdl.notify.events &= ~arg->events;
		}
		lis_spin_unlock_irqrestore(&xp->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}

/* 
 *  SDL_IOCCDISCTX:     
 *  -----------------------------------
 */
STATIC int sdl_ioccdisctx(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags = 0;
	(void) mp;
	lis_spin_lock_irqsave(&xp->lock, &flags);
	{
		xp->obj.sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
	}
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	return (0);
}

/* 
 *  SDL_IOCCONNTX:      
 *  -----------------------------------
 */
STATIC int sdl_ioccconntx(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags = 0;
	(void) mp;
	lis_spin_lock_irqsave(&xp->lock, &flags);
	{
		xp->obj.sdl.config.ifflags |= SDL_IF_TX_RUNNING;
	}
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	return (0);
}

STATIC INLINE mblk_t *sdl_transmission_request(queue_t *q, struct xp *xp, struct ts *ts)
{
	mblk_t *mp;
	if ((mp = bufq_dequeue(&ts->tx.buf))) {
		if (ts->tx.buf.q_count < 32 && q->q_count)
			qenable(q);
	} else
		xp->obj.sdl.stats.tx_buffer_overflows++;
	return (mp);
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
STATIC INLINE void xp_tx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be, const ulong type)
{
	int chan = 0;
	queue_t *q = xp->iq;
	register xp_path_t *tx = &ts->tx;
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
			if (!(tx->msg = tx->nxt = sdl_transmission_request(q, xp, ts))) {
				xp->obj.sdl.stats.tx_buffer_overflows++;
				return;
			}
		      tx_process_block:
			switch (type) {
			default:
			case SDL_TYPE_DS0:
			case SDL_TYPE_DS0A:
				*bp = *(tx->nxt->b_rptr)++;
				chan = 0;
				break;
			case SDL_TYPE_T1:
				*(bp + (xp_t1_chan_map[chan] << 2)) = *(tx->nxt->b_rptr)++;
				if (++chan > 23)
					chan = 0;
				ts = xp->sp->slots[xp_t1_chan_map[chan]];
				tx = &ts->tx;
				break;
			case SDL_TYPE_E1:
				*(bp + (xp_e1_chan_map[chan] << 2)) = *(tx->nxt->b_rptr)++;
				if (++chan > 30)
					chan = 0;
				ts = xp->sp->slots[xp_e1_chan_map[chan]];
				tx = &ts->tx;
				break;
			}
			xp->obj.sdl.stats.tx_octets++;
			while (tx->nxt && tx->nxt->b_rptr >= tx->nxt->b_wptr)
				tx->nxt = tx->nxt->b_cont;
		}
		while (chan);
	}
}

STATIC INLINE void xp_rx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be, const ulong type)
{
	int chan = 0;
	queue_t *q = xp->oq;
	register xp_path_t *rx = &ts->rx;
	for (; bp < be; bp += 128) {
		do {
			if (rx->nxt) {
				if (rx->nxt->b_wptr < rx->nxt->b_rptr + xp->obj.sdl.config.ifblksize)
					goto rx_process_block;
				if (sdl_received_bits_ind(q, xp, rx->nxt) != QR_ABSORBED)
					goto rx_process_block;
				xp->obj.sdl.stats.rx_buffer_overflows++;
			}
			/* new block */
			if (!(rx->nxt = ss7_fast_allocb(&xp_bufpool, FASTBUF, BPRI_HI))) {
				xp->obj.sdl.stats.rx_buffer_overflows++;
				return;
			}
		      rx_process_block:
			switch (type) {
			default:
			case SDL_TYPE_DS0:
			case SDL_TYPE_DS0A:
				*(rx->nxt->b_wptr)++ = *bp;
				chan = 0;
				break;
			case SDL_TYPE_T1:
				*(rx->nxt->b_wptr)++ = (*bp + (xp_t1_chan_map[chan] << 2));
				if (++chan > 23)
					chan = 0;
				ts = xp->sp->slots[xp_t1_chan_map[chan]];
				rx = &ts->rx;
				break;
			case SDL_TYPE_E1:
				*(rx->nxt->b_wptr)++ = (*bp + (xp_e1_chan_map[chan] << 2));
				if (++chan > 30)
					chan = 0;
				ts = xp->sp->slots[xp_e1_chan_map[chan]];
				rx = &ts->rx;
				break;
			}
			xp->obj.sdl.stats.rx_octets++;
		} while (chan);
	}
}

/* force 4 separate versions for speed */
STATIC INLINE void xp_ds0a_tx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be)
{
	xp_tx_block(xp, ts, bp, be, SDL_TYPE_DS0A);
}
STATIC INLINE void xp_ds0_tx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be)
{
	xp_tx_block(xp, ts, bp, be, SDL_TYPE_DS0);
}
STATIC INLINE void xp_t1_tx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be)
{
	xp_tx_block(xp, ts, bp, be, SDL_TYPE_T1);
}
STATIC INLINE void xp_e1_tx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be)
{
	xp_tx_block(xp, ts, bp, be, SDL_TYPE_E1);
}

/* force 4 separate versions for speed */
STATIC void xp_ds0a_rx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be)
{
	xp_rx_block(xp, ts, bp, be, SDL_TYPE_DS0A);
}
STATIC void xp_ds0_rx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be)
{
	xp_rx_block(xp, ts, bp, be, SDL_TYPE_DS0);
}
STATIC void xp_t1_rx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be)
{
	xp_rx_block(xp, ts, bp, be, SDL_TYPE_T1);
}
STATIC void xp_e1_rx_block(struct xp *xp, struct ts *ts, uchar *bp, uchar *be)
{
	xp_rx_block(xp, ts, bp, be, SDL_TYPE_E1);
}

STATIC INLINE void xp_tx_idle(uchar *bp, uchar *be, const ulong type)
{
	int chan;
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
STATIC INLINE void xp_ds0a_tx_idle(uchar *bp, uchar *be)
{
	xp_tx_idle(bp, be, SDL_TYPE_DS0A);
}
STATIC INLINE void xp_ds0_tx_idle(uchar *bp, uchar *be)
{
	xp_tx_idle(bp, be, SDL_TYPE_DS0);
}
STATIC INLINE void xp_t1_tx_idle(uchar *bp, uchar *be)
{
	xp_tx_idle(bp, be, SDL_TYPE_T1);
}
STATIC INLINE void xp_e1_tx_idle(uchar *bp, uchar *be)
{
	xp_tx_idle(bp, be, SDL_TYPE_E1);
}

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
STATIC INLINE void xp_e1c_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	int slot;
	/* one slot at a time, 8 frames */
	for (slot = 1; slot < 32; slot++) {
		struct ts *ts;
		struct xp *xp;
		size_t coff = slot << 2;
		if ((ts = sp->slots[slot]) && (xp = ts->xp) && (xp->obj.sdl.config.ifflags & SDL_IF_UP)) {
			if (xp->obj.sdl.config.iftype != SDL_TYPE_DS0A) {
				if (xp->obj.sdl.config.ifflags & SDL_IF_TX_RUNNING) {
					xp_ds0_tx_block(xp, ts, wspan + coff, wend);
					xp->obj.sdl.stats.tx_octets += 8;
				} else {
					xp_ds0_tx_idle(wspan + coff, wend);
				}
				if (xp->obj.sdl.config.ifflags & SDL_IF_RX_RUNNING) {
					xp_ds0_rx_block(xp, ts, rspan + coff, rend);
					xp->obj.sdl.stats.rx_octets += 8;
				}
			}
		} else {
			xp_ds0_tx_idle(wspan + coff, wend);
		}
	}
}

/*
 *  E1 Process
 *  -----------------------------------
 *  Process an entire E1 span.  This is a High-Speed Link.  All channels are
 *  concatenated to form a single link.
 */
STATIC INLINE void xp_e1_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	struct xp *xp;
	struct ts *ts;
	/* entire span, one frame at a time */
	if ((ts = sp->slots[0]) && (xp = ts->xp)) {
		if (xp->obj.sdl.config.ifflags & SDL_IF_TX_RUNNING) {
			xp_e1_tx_block(xp, ts, wspan, wend);
			xp->obj.sdl.stats.tx_octets += 8 * 31;
		} else {
			xp_e1_tx_idle(wspan, wend);
		}
		if (xp->obj.sdl.config.ifflags & SDL_IF_RX_RUNNING) {
			xp_e1_rx_block(xp, ts, rspan, rend);
			xp->obj.sdl.stats.rx_octets += 8 * 31;
		}
	} else {
		xp_e1_tx_idle(wspan, wend);
	}
}

/*
 *  E1 Card Tasklet
 *  -----------------------------------
 *  Process an entire E1 card.
 */
STATIC void xp_e1_card_tasklet(unsigned long data)
{
	struct cd *cd = (struct cd *) data;
	lis_spin_lock(&cd->lock);
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
						xp_e1_process(sp, wbeg + soff, rbeg + soff, wend, rend);
					else
						xp_e1c_process(sp, wbeg + soff, rbeg + soff, wend, rend);
				}
			}
			if ((cd->uebno = (cd->uebno + 1) & (X400P_EBUFNO - 1)) != cd->lebno)
				tasklet_schedule(&cd->tasklet);
		}
	}
	lis_spin_unlock(&cd->lock);
}

/*
 *  T1C Process
 *  -----------------------------------
 *  Process a channelized T1 span, one channel at a time.  Each channel can be
 *  either a clear channel or a DS0A channel.
 */
STATIC void xp_t1c_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	int slot;
	/* one slot at a time, 8 frames */
	for (slot = 1; slot < 32; slot++) {
		if ((slot & 0x3)) {
			struct ts *ts;
			struct xp *xp;
			size_t coff = slot << 2;
			if ((ts = sp->slots[slot]) && (xp = ts->xp) && (xp->obj.sdl.config.ifflags & SDL_IF_UP)) {
				if (xp->obj.sdl.config.iftype != SDL_TYPE_DS0A) {
					if (xp->obj.sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp_ds0_tx_block(xp, ts, wspan + coff, wend);
						xp->obj.sdl.stats.tx_octets += 8;
					} else {
						xp_ds0a_tx_idle(wspan + coff, wend);
					}
					if (xp->obj.sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp_ds0_rx_block(xp, ts, rspan + coff, rend);
						xp->obj.sdl.stats.rx_octets += 8;
					}
				} else {
					if (xp->obj.sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp_ds0a_tx_block(xp, ts, wspan + coff, wend);
						xp->obj.sdl.stats.tx_octets += 8;
					} else {
						xp_ds0a_tx_idle(wspan + coff, wend);
					}
					if (xp->obj.sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp_ds0a_rx_block(xp, ts, rspan + coff, rend);
						xp->obj.sdl.stats.rx_octets += 8;
					}
				}
			} else {
				xp_ds0a_tx_idle(wspan + coff, wend);
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
STATIC void xp_t1_process(struct sp *sp, uchar *wspan, uchar *rspan, uchar *wend, uchar *rend)
{
	struct xp *xp;
	struct ts *ts;
	/* entire span, one frame at a time */
	if ((ts = sp->slots[0]) && (xp = ts->xp)) {
		if (xp->obj.sdl.config.ifflags & SDL_IF_TX_RUNNING) {
			xp_t1_tx_block(xp, ts, wspan, wend);
			xp->obj.sdl.stats.tx_octets += 8 * 24;
		} else {
			xp_t1_tx_idle(wspan, wend);
		}
		if (xp->obj.sdl.config.ifflags & SDL_IF_RX_RUNNING) {
			xp_t1_rx_block(xp, ts, rspan, rend);
			xp->obj.sdl.stats.rx_octets += 8 * 24;
		}
	} else {
		xp_t1_tx_idle(wspan, wend);
	}
}

/*
 *  T1 Card Tasklet
 *  -----------------------------------
 *  Process an entire T1 card.
 */
STATIC void xp_t1_card_tasklet(unsigned long data)
{
	struct cd *cd = (struct cd *) data;
	lis_spin_lock(&cd->lock);
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
						xp_t1_process(sp, wbeg + soff, rbeg + soff, wend, rend);
					else
						xp_t1c_process(sp, wbeg + soff, rbeg + soff, wend, rend);
				}
			}
			if ((cd->uebno = (cd->uebno + 1) & (X400P_EBUFNO - 1)) != cd->lebno)
				tasklet_schedule(&cd->tasklet);
		}
	}
	lis_spin_unlock(&cd->lock);
}

/*
 *  X400P Overflow
 *  -----------------------------------
 *  I know that this is rather like kicking them when they are down, we are
 *  doing stats in the ISR when takslets don't have enough time to run, but we
 *  are already in dire trouble if this is happening anyway.  It should not
 *  take too much time to peg these counts.
 */
STATIC void xp_overflow(struct cd *cd)
{
	int span;
	printd(("%s: card %d elastic buffer overrun!\n", __FUNCTION__, cd->card));
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
					if (sp->slots[slot] && (xp = sp->slots[slot]->xp)) {
						if (xp->obj.sdl.config.ifflags & SDL_IF_TX_RUNNING) {
							xp->obj.sdl.stats.tx_underruns += 8;
						}
						if (xp->obj.sdl.config.ifflags & SDL_IF_RX_RUNNING) {
							xp->obj.sdl.stats.rx_overruns += 8;
						}
					}
				}
				break;
			}
			case SDL_TYPE_T1:
				if ((xp = sp->xp)) {
					if (xp->obj.sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp->obj.sdl.stats.tx_underruns += 8 * 24;
					}
					if (xp->obj.sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp->obj.sdl.stats.rx_overruns += 8 * 24;
					}
				}
				break;
			case SDL_TYPE_E1:
				if ((xp = sp->xp)) {
					if (xp->obj.sdl.config.ifflags & SDL_IF_TX_RUNNING) {
						xp->obj.sdl.stats.tx_underruns += 8 * 31;
					}
					if (xp->obj.sdl.config.ifflags & SDL_IF_RX_RUNNING) {
						xp->obj.sdl.stats.rx_overruns += 8 * 31;
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
STATIC void xp_e1_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct cd *cd = (struct cd *) dev_id;
	/* active interrupt (otherwise spurious or shared) */
	if (cd->xlb[STAREG] & INTACTIVE) {
		struct sp *sp;
		int span, lebno;
		cd->xlb[CTLREG] = (INTENA | OUTBIT | INTACK | E1DIV);
		if ((lebno = (cd->lebno + 1) & (X400P_EBUFNO - 1)) != cd->uebno) {
			/* write/read burst */
			unsigned int offset = lebno << 8;
			register int word, slot;
			uint32_t *wbuf = cd->wbuf + offset;
			uint32_t *rbuf = cd->rbuf + offset;
			volatile uint32_t *xll = cd->xll;
			for (word = 0; word < 256; word += 32)
				for (slot = word + 1; slot < word + 32; slot++)
					xll[slot] = wbuf[slot];
			for (word = 0; word < 256; word += 32)
				for (slot = word + 1; slot < word + 32; slot++)
					rbuf[slot] = xll[slot];
			cd->lebno = lebno;
			tasklet_schedule(&cd->tasklet);
		} else
			xp_overflow(cd);
		for (span = 0; span < X400_SPANS; span++) {
			if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
				int base = span << 8;
				if (sp->recovertime && !--sp->recovertime) {
					printd(("%s: alarm recovery complete\n", __FUNCTION__));
					sp->config.ifalarms &= ~SDL_ALARM_REC;
					cd->xlb[base + 0x21] = 0x5f;	/* turn off yellow */
					cd->eval_syncsrc = 1;
				}
			}
		}
		/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
		if ((span = ((cd->frame >> 3) & 0x3f) - 50) >= 0 && span < X400_SPANS && (sp = cd->spans[span])
		    && (sp->config.ifflags & SDL_IF_UP)) {
			int status, alarms = 0, leds = 0, all_leds;
			int base = span << 8;
			cd->xlb[base + 0x06] = 0xff;
			status = cd->xlb[base + 0x06];
			if (status & 0x09)
				alarms |= SDL_ALARM_RED;
			if (status & 0x02)
				alarms |= SDL_ALARM_BLU;
			if (alarms) {
				if (!(sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just begun */
					cd->xlb[base + 0x21] = 0x7f;	/* set yellow alarm */
					cd->eval_syncsrc = 1;
				}
			} else {
				if ((sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just ended */
					alarms |= SDL_ALARM_REC;
					sp->recovertime = X400P_SDL_ALARM_SETTLE_TIME;
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
			all_leds = cd->leds;
			all_leds &= ~(LEDYEL << (span << 1));
			all_leds |= leds << (span << 1);
			if (cd->leds != all_leds) {
				cd->xlb[LEDREG] = all_leds;
				cd->leds = all_leds;
			}
		}
		// if (!(cd->frame % 8000)) {
		if (!(cd->frame & 0x1fff)) {	/* 1.024 seconds */
			for (span = 0; span < X400_SPANS; span++)
				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					int base = span << 8;
					// printd(("%s: accumulating bipolar violations\n",
					// __FUNCTION__));
					sp->config.ifbpv += cd->xlb[base + 0x01] + (cd->xlb[base + 0x00] << 8);
				}
		}
		if (xchg((int *)&cd->eval_syncsrc, 0)) {
			int src, syncsrc = 0;
			for (src = 0; src < SDL_SYNCS; src++) {
				if ((span = cd->config.ifsyncsrc[src]) && (--span < X400_SPANS) && cd->spans[span]
				    && (cd->spans[span]->config.ifflags & SDL_IF_UP) &&
				    !(cd->spans[span]->config.ifclock == SDL_CLOCK_LOOP) &&
				    !(cd->spans[span]->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
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
	}
	return;
}

/*
 *  T400P-SS7 Interrupt Service Routine
 *  -----------------------------------
 */
STATIC void xp_t1_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct cd *cd = (struct cd *) dev_id;
	/* active interrupt (otherwise spurious or shared) */
	if (cd->xlb[STAREG] & INTACTIVE) {
		struct sp *sp;
		int span, lebno;
		cd->xlb[CTLREG] = (INTENA | OUTBIT | INTACK);
		if ((lebno = (cd->lebno + 1) & (X400P_EBUFNO - 1)) != cd->uebno) {
			/* write/read burst */
			unsigned int offset = lebno << 8;
			register int word, slot;
			uint32_t *wbuf = cd->wbuf + offset;
			uint32_t *rbuf = cd->rbuf + offset;
			volatile uint32_t *xll = cd->xll;
			for (word = 0; word < 256; word += 32)
				for (slot = word + 1; slot < word + 32; slot++)
					if (slot & 0x3)
						xll[slot] = wbuf[slot];
			for (word = 0; word < 256; word += 32)
				for (slot = word + 1; slot < word + 32; slot++)
					if (slot & 0x3)
						rbuf[slot] = xll[slot];
			cd->lebno = lebno;
			tasklet_schedule(&cd->tasklet);
		} else
			xp_overflow(cd);
		for (span = 0; span < X400_SPANS; span++) {
			if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
				int base = span << 8;
				if (sp->recovertime && !--sp->recovertime) {
					/* alarm recovery complete */
					sp->config.ifalarms &= ~SDL_ALARM_REC;
					cd->xlb[base + 0x35] = 0x10;	/* turn off yellow */
					cd->eval_syncsrc = 1;
				}
			}
		}
		/* process status span 1 frame 400/512, span 2 frame 408/512, ... */
		if ((span = ((cd->frame >> 3) & 0x3f) - 50) >= 0 && span < X400_SPANS && (sp = cd->spans[span])
		    && (sp->config.ifflags & SDL_IF_UP)) {
			int status, alarms = 0, leds = 0, all_leds;
			int base = span << 8;
			sp->config.ifrxlevel = cd->xlb[base + RIR2] >> 6;
			cd->xlb[base + 0x20] = 0xff;
			status = cd->xlb[base + 0x20];
			/* loop up code */
			if ((status & 0x80) && !(sp->config.ifgmode & SDL_GMODE_LOC_LB)) {
				if (sp->loopcnt++ > 80 && !(sp->config.ifgmode & SDL_GMODE_REM_LB)) {
					cd->xlb[base + 0x1e] = 0x00;	/* no local loop */
					cd->xlb[base + 0x40] = 0x40;	/* remote loop */
					sp->config.ifgmode |= SDL_GMODE_REM_LB;
				}
			} else
				sp->loopcnt = 0;
			if ((status & 0x40) && !(sp->config.ifgmode & SDL_GMODE_LOC_LB)) {
				/* loop down code */
				if (sp->loopcnt++ > 80 && (sp->config.ifgmode & SDL_GMODE_REM_LB)) {
					cd->xlb[base + 0x1e] = 0x00;	/* no local loop */
					cd->xlb[base + 0x40] = 0x00;	/* no remote loop */
					sp->config.ifgmode &= ~SDL_GMODE_REM_LB;
				}
			} else
				sp->loopcnt = 0;
			if (status & 0x03)
				alarms |= SDL_ALARM_RED;
			if (status & 0x08)
				alarms |= SDL_ALARM_BLU;
			if (alarms) {
				if (!(sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just begun */
					cd->xlb[base + 0x35] = 0x11;	/* set yellow alarm */
					cd->eval_syncsrc = 1;
				}
			} else {
				if ((sp->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
					/* alarms have just ended */
					alarms |= SDL_ALARM_REC;
					sp->recovertime = X400P_SDL_ALARM_SETTLE_TIME;
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
			all_leds = cd->leds;
			all_leds &= ~(LEDYEL << (span << 1));
			all_leds |= leds << (span << 1);
			if (cd->leds != all_leds) {
				cd->xlb[LEDREG] = all_leds;
				cd->leds = all_leds;
			}
		}
		// if (!(cd->frame % 8000)) {
		if (!(cd->frame & 0x1fff)) {	/* 1.024 seconds */
			for (span = 0; span < X400_SPANS; span++)
				if ((sp = cd->spans[span]) && (sp->config.ifflags & SDL_IF_UP)) {
					int base = span << 8;
					// printd(("%s: accumulating bipolar violations\n",
					// __FUNCTION__));
					sp->config.ifbpv += cd->xlb[base + 0x24] + (cd->xlb[base + 0x23] << 8);
				}
		}
		if (xchg((int *)&cd->eval_syncsrc, 0)) {
			int src, syncsrc = 0;
			for (src = 0; src < SDL_SYNCS; src++) {
				if ((span = cd->config.ifsyncsrc[src]) && (--span < X400_SPANS) && cd->spans[span]
				    && (cd->spans[span]->config.ifflags & SDL_IF_UP) &&
				    !(cd->spans[span]->config.ifclock == SDL_CLOCK_LOOP) &&
				    !(cd->spans[span]->config.ifalarms & (SDL_ALARM_RED | SDL_ALARM_BLU))) {
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
	}
	return;
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
STATIC int xp_w_ioctl(queue_t *q, mblk_t *mp)
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
			ptrace(("%s: ERROR: Unknown IOCTL %d\n", XP_DRV_NAME, cmd));
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
		ptrace(("%s: ERROR: Unsupported STREAMS ioctl\n", XP_DRV_NAME));
		ret = -EOPNOTSUPP;
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || xp->i_state == LMI_UNATTACHED) {
			ptrace(("%s: ERROR: ioctl count = %d, size = %d, state = %ld\n", XP_DRV_NAME, count, size,
				xp->i_state));
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
			ptrace(("%s: ERROR: Unsupported SDL ioctl\n", XP_DRV_NAME));
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
STATIC int xp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct xp *xp = XP_PRIV(q);
	ulong oldstate = xp->i_state;
	switch (xp->i_style) {
	case XP_STYLE_LMI:
		switch ((prim = *(ulong *) mp->b_rptr)) {
		default:
			rtn = -EOPNOTSUPP;
			break;
		}
		break;
	case XP_STYLE_SDLI:
		switch ((prim = *(ulong *) mp->b_rptr)) {
		case SDL_BITS_FOR_TRANSMISSION_REQ:
			printd(("%s: %p: -> SDL_BITS_FOR_TRANSMISSION_REQ\n", XP_DRV_NAME, xp));
			rtn = sdl_bits_for_transmission_req(q, mp);
			break;
		case SDL_CONNECT_REQ:
			printd(("%s: %p: -> SDL_CONNECT_REQ\n", XP_DRV_NAME, xp));
			rtn = sdl_connect_req(q, mp);
			break;
		case SDL_DISCONNECT_REQ:
			printd(("%s: %p: -> SDL_DISCONNECT_REQ\n", XP_DRV_NAME, xp));
			rtn = sdl_disconnect_req(q, mp);
			break;
		case LMI_INFO_REQ:
			printd(("%s: %p: -> LMI_INFO_REQ\n", XP_DRV_NAME, xp));
			rtn = lmi_info_req(q, mp);
			break;
		case LMI_ATTACH_REQ:
			printd(("%s: %p: -> LMI_ATTACH_REQ\n", XP_DRV_NAME, xp));
			rtn = lmi_attach_req(q, mp);
			break;
		case LMI_DETACH_REQ:
			printd(("%s: %p: -> LMI_DETACH_REQ\n", XP_DRV_NAME, xp));
			rtn = lmi_detach_req(q, mp);
			break;
		case LMI_ENABLE_REQ:
			printd(("%s: %p: -> LMI_ENABLE_REQ\n", XP_DRV_NAME, xp));
			rtn = lmi_enable_req(q, mp);
			break;
		case LMI_DISABLE_REQ:
			printd(("%s: %p: -> LMI_DISABLE_REQ\n", XP_DRV_NAME, xp));
			rtn = lmi_disable_req(q, mp);
			break;
		case LMI_OPTMGMT_REQ:
			printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", XP_DRV_NAME, xp));
			rtn = lmi_optmgmt_req(q, mp);
			break;
		default:
			rtn = -EOPNOTSUPP;
			break;
		}
		break;
	case XP_STYLE_CHI:
		switch ((prim = *(ulong *) mp->b_rptr)) {
		case CH_INFO_REQ:
			printd(("%s: %p: --> CH_INFO_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_info_req(q, mp);
			break;
		case CH_OPTMGMT_REQ:
			printd(("%s: %p: --> CH_OPTMGMT_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_optmgmt_req(q, mp);
			break;
		case CH_ATTACH_REQ:
			printd(("%s: %p: --> CH_ATTACH_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_attach_req(q, mp);
			break;
		case CH_ENABLE_REQ:
			printd(("%s: %p: --> CH_ENABLE_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_enable_req(q, mp);
			break;
		case CH_CONNECT_REQ:
			printd(("%s: %p: --> CH_CONNECT_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_connect_req(q, mp);
			break;
		case CH_DATA_REQ:
			printd(("%s: %p: --> CH_DATA_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_data_req(q, mp);
			break;
		case CH_DISCONNECT_REQ:
			printd(("%s: %p: --> CH_DISCONNECT_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_disconnect_req(q, mp);
			break;
		case CH_DISABLE_REQ:
			printd(("%s: %p: --> CH_DISABLE_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_disable_req(q, mp);
			break;
		case CH_DETACH_REQ:
			printd(("%s: %p: --> CH_DETACH_REQ\n", XP_DRV_NAME, xp));
			rtn = ch_detach_req(q, mp);
			break;
		default:
			rtn = -EOPNOTSUPP;
			break;
		}
		break;
	case XP_STYLE_MXI:
		switch ((prim = *(ulong *) mp->b_rptr)) {
		case MX_INFO_REQ:
			printd(("%s: %p: --> MX_INFO_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_info_req(q, mp);
			break;
		case MX_OPTMGMT_REQ:
			printd(("%s: %p: --> MX_OPTMGMT_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_optmgmt_req(q, mp);
			break;
		case MX_ATTACH_REQ:
			printd(("%s: %p: --> MX_ATTACH_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_attach_req(q, mp);
			break;
		case MX_ENABLE_REQ:
			printd(("%s: %p: --> MX_ENABLE_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_enable_req(q, mp);
			break;
		case MX_CONNECT_REQ:
			printd(("%s: %p: --> MX_CONNECT_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_connect_req(q, mp);
			break;
		case MX_DATA_REQ:
			printd(("%s: %p: --> MX_DATA_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_data_req(q, mp);
			break;
		case MX_DISCONNECT_REQ:
			printd(("%s: %p: --> MX_DISCONNECT_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_disconnect_req(q, mp);
			break;
		case MX_DISABLE_REQ:
			printd(("%s: %p: --> MX_DISABLE_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_disable_req(q, mp);
			break;
		case MX_DETACH_REQ:
			printd(("%s: %p: --> MX_DETACH_REQ\n", XP_DRV_NAME, xp));
			rtn = mx_detach_req(q, mp);
			break;
		default:
			rtn = -EOPNOTSUPP;
			break;
		}
		break;
	default:
		swerr();
		rtn = -EFAULT;
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
STATIC int xp_w_data(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);
	switch (xp->i_style) {
	default:
	case XP_STYLE_SDLI:
		return sdl_m_data(q, mp);
	case XP_STYLE_CHI:
		return ch_m_data(q, mp);
	case XP_STYLE_MXI:
		return mx_m_data(q, mp);
	case XP_STYLE_LMI:
		return lm_m_data(q, mp);
	}
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int xp_r_prim(queue_t *q, mblk_t *mp)
{
	return (QR_PASSFLOW);
}
STATIC INLINE int xp_w_prim(queue_t *q, mblk_t *mp)
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

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *  Open is called on the first open of a character special device stream
 *  head; close is called on the last close of the same device.
 */
STATIC int xp_majors[SL_X400P_CMAJORS] = { SL_X400P_CMAJOR_0, };
STATIC struct xp *xp_list = NULL;
STATIC lis_spin_lock_t xp_lock;

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int xp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	ushort cmajor = getmajor(*devp);
	ushort cminor = getminor(*devp);
	ushort bminor = cminor;
	struct xp *xp, **xpp = &xp_list;
	(void) crp;
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: Can't open as module\n", XP_DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (cmajor != SL_X400P_CMAJOR_0 || cminor >= X400P_CMINOR_FREE) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* allocate a new device */
	cminor = X400P_CMINOR_FREE;
	lis_spin_lock_irqsave(&xp_lock, &flags);
	for (; *xpp; xpp = &(*xpp)->next) {
		ushort dmajor = (*xpp)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			ushort dminor = (*xpp)->u.dev.cminor;
			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= X400P_NMINOR) {
					if (++mindex >= SL_X400P_CMAJORS || !(cmajor = xp_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= SL_X400P_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", XP_DRV_NAME));
		lis_spin_unlock_irqrestore(&xp_lock, &flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", XP_DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(xp = xp_alloc_priv(q, xpp, devp, crp, bminor))) {
		ptrace(("%s: ERROR: No memory\n", XP_DRV_NAME));
		lis_spin_unlock_irqrestore(&xp_lock, &flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	lis_spin_unlock_irqrestore(&xp_lock, &flags);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int xp_close(queue_t *q, int flag, cred_t *crp)
{
	struct xp *xp = XP_PRIV(q);
	psw_t flags;
	(void) flag;
	(void) crp;
	(void) xp;
	printd(("%s: closing character device %d:%d\n", XP_DRV_NAME, xp->u.dev.cmajor, xp->u.dev.cminor));
	lis_spin_lock_irqsave(&xp_lock, &flags);
	xp_free_priv(xp);
	lis_spin_unlock_irqrestore(&xp_lock, &flags);
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
STATIC kmem_cache_t *xp_slot_cachep = NULL;
STATIC kmem_cache_t *xp_span_cachep = NULL;
STATIC kmem_cache_t *xp_card_cachep = NULL;
STATIC kmem_cache_t *xp_xbuf_cachep = NULL;

/*
 *  Cache allocation
 *  -------------------------------------------------------------------------
 */
STATIC void xp_term_caches(void)
{
	if (xp_xbuf_cachep) {
		if (kmem_cache_destroy(xp_xbuf_cachep))
			cmn_err(CE_WARN, "%s: did not destroy xp_xbuf_cachep", __FUNCTION__);
		else
			printd(("%s: shrunk xp_xbuf_cache to zero\n", XP_DRV_NAME));
	}
	if (xp_card_cachep) {
		if (kmem_cache_destroy(xp_card_cachep))
			cmn_err(CE_WARN, "%s: did not destroy xp_card_cachep", __FUNCTION__);
		else
			printd(("%s: shrunk xp_card_cache to zero\n", XP_DRV_NAME));
	}
	if (xp_slot_cachep) {
		if (kmem_cache_destroy(xp_slot_cachep))
			cmn_err(CE_WARN, "%s: did not destroy xp_slot_cachep", __FUNCTION__);
		else
			printd(("%s: shrunk xp_slot_cache to zero\n", XP_DRV_NAME));
	}
	if (xp_span_cachep) {
		if (kmem_cache_destroy(xp_span_cachep))
			cmn_err(CE_WARN, "%s: did not destroy xp_span_cachep", __FUNCTION__);
		else
			printd(("%s: shrunk xp_span_cache to zero\n", XP_DRV_NAME));
	}
	if (xp_priv_cachep) {
		if (kmem_cache_destroy(xp_priv_cachep))
			cmn_err(CE_WARN, "%s: did not destroy xp_priv_cachep", __FUNCTION__);
		else
			printd(("%s: shrunk xp_priv_cache to zero\n", XP_DRV_NAME));
	}
	return;
}
STATIC int xp_init_caches(void)
{
	if (!xp_priv_cachep &&
	    !(xp_priv_cachep =
	      kmem_cache_create("xp_priv_cachep", sizeof(struct xp), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))
	    ) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_priv_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized device private structure cache\n", XP_DRV_NAME));
	if (!xp_slot_cachep &&
	    !(xp_slot_cachep =
	      kmem_cache_create("xp_slot_cachep", sizeof(struct ts), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))
	    ) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_slot_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized span private structure cache\n", XP_DRV_NAME));
	if (!xp_span_cachep &&
	    !(xp_span_cachep =
	      kmem_cache_create("xp_span_cachep", sizeof(struct sp), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))
	    ) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_span_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized span private structure cache\n", XP_DRV_NAME));
	if (!xp_card_cachep &&
	    !(xp_card_cachep =
	      kmem_cache_create("xp_card_cachep", sizeof(struct cd), 0, SLAB_HWCACHE_ALIGN, NULL, NULL))
	    ) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_card_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized card private structure cache\n", XP_DRV_NAME));
	if (!xp_xbuf_cachep &&
	    !(xp_xbuf_cachep =
	      kmem_cache_create("xp_xbuf_cachep", X400P_EBUFNO * 1024, 0, SLAB_HWCACHE_ALIGN, NULL, NULL))
	    ) {
		cmn_err(CE_PANIC, "%s: Cannot allocate xp_xbuf_cachep", __FUNCTION__);
		goto error;
	} else
		printd(("%s: initialized card read/write buffer cache\n", XP_DRV_NAME));
	return (0);
      error:
	xp_term_caches();
	return (-ENOMEM);
}

/*
 *  Private structure allocation
 *  -------------------------------------------------------------------------
 */
STATIC struct xp *xp_alloc_priv(queue_t *q, struct xp **xpp, dev_t *devp, cred_t *crp, ushort bminor)
{
	struct xp *xp;
	if ((xp = kmem_cache_alloc(xp_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated device private structure\n", XP_DRV_NAME, xp));
		bzero(xp, sizeof(*xp));
		xp_get(xp);	/* first get */
		xp->put = &xp_put;
		xp->u.dev.cmajor = getmajor(*devp);
		xp->u.dev.cminor = getminor(*devp);
		xp->cred = *crp;
		(xp->oq = RD(q))->q_ptr = xp_get(xp);
		(xp->iq = WR(q))->q_ptr = xp_get(xp);
		lis_spin_lock_init(&xp->qlock, "xp-queue-lock");
		xp->o_prim = &xp_r_prim;
		xp->o_wakeup = NULL;
		xp->i_prim = &xp_w_prim;
		xp->i_wakeup = NULL;
		switch (bminor) {
		default:
		case XP_STYLE_SDLI:
			xp->i_state = LMI_UNATTACHED;
			xp->i_style = XP_STYLE_SDLI;
			xp->i_version = 1;
			break;
		case XP_STYLE_CHI:
			xp->i_state = 0;
			xp->i_style = XP_STYLE_CHI;
			xp->i_version = 1;
			break;
		case XP_STYLE_MXI:
			xp->i_state = 0;
			xp->i_style = XP_STYLE_MXI;
			xp->i_version = 1;
			break;
		case XP_STYLE_LMI:
			xp->i_state = 0;
			xp->i_style = XP_STYLE_LMI;
			xp->i_version = 1;
		}
		lis_spin_lock_init(&xp->lock, "xp-priv-lock");
		if ((xp->next = *xpp))
			xp->next->prev = &xp->next;
		xp->prev = xpp;
		*xpp = xp_get(xp);
		printd(("%s: linked device private structure\n", XP_DRV_NAME));
		printd(("%s: setting device private structure defaults\n", XP_DRV_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate device private structure\n", XP_DRV_NAME));
	return (xp);
}

STATIC void xp_free_priv(struct xp *xp)
{
	psw_t flags = 0;
	ensure(xp, return);
	lis_spin_lock_irqsave(&xp->lock, &flags);
	{
		struct sp *sp;
		if ((sp = xp->sp)) {
			struct cd *cd;
			if ((cd = sp->cd)) {
				lis_spin_lock(&cd->lock);
				{
					int slot;
					uchar idle = (cd->config.ifgtype == SDL_GTYPE_T1) ? 0x7f : 0xff;
					uchar *base = (uchar *) cd->wbuf + span_to_byte(sp->span);
					for (slot = 0; slot < 32; slot++) {
						if (sp->slots[slot] && sp->slots[slot]->xp == xp) {
							int boff;
							xp_free_ts(xchg(&sp->slots[slot], NULL));
							for (boff = 0; boff < X400P_EBUFNO; boff++)
								*(base + (boff << 10) + (slot << 2)) = idle;
						}
					}
				}
				lis_spin_unlock(&cd->lock);
			} else {
				int slot;
				for (slot = 0; slot < 32; slot++)
					if (sp->slots[slot] && sp->slots[slot]->xp == xp)
						xp_free_ts(xchg(&sp->slots[slot], NULL));
			}
			sp_put(xchg(&xp->sp, NULL));
			printd(("%s: unlinked device private structure from span\n", XP_DRV_NAME));
		}
		ss7_unbufcall((str_t *) xp);
		ss7_bufpool_release(&xp_bufpool, 2);
		if (xp->next || xp->prev != &xp->next) {
			if ((*xp->prev = xp->next))
				xp->next->prev = xp->prev;
			xp->next = NULL;
			xp->prev = &xp->next;
			xp_put(xp);
		}
	}
	lis_spin_unlock_irqrestore(&xp->lock, &flags);
	xp_put(xp);		/* final put */
}

STATIC struct xp *xp_get(struct xp *xp)
{
	if (xp)
		atomic_inc(&xp->refcnt);
	return (xp);
}

STATIC void xp_put(struct xp *xp)
{
	if (atomic_dec_and_test(&xp->refcnt)) {
		kmem_cache_free(xp_priv_cachep, xp);
		printd(("%s: freed device private structure\n", XP_DRV_NAME));
	}
}

/*
 *  Slot allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct ts *xp_alloc_ts(struct xp *xp, struct sp *sp, uint8_t slot)
{
	struct ts *ts;
	if ((ts = kmem_cache_alloc(xp_slot_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated slot private structure\n", XP_DRV_NAME));
		bzero(ts, sizeof(*ts));
		ts_get(ts);	/* first get */
		lis_spin_lock_init(&ts->lock, "ts-priv-lock");
		/* create linkage */
		sp->slots[slot] = ts_get(ts);
		ts->xp = xp_get(xp);
		ts->sp = sp_get(sp);
		/* fill out slot structure */
		printd(("%s: linked slot private structure\n", XP_DRV_NAME));
		ts->slot = slot;
		bufq_init(&ts->tx.buf);
		bufq_init(&ts->rx.buf);
		ss7_bufpool_reserve(&xp_bufpool, 2);
		printd(("%s: set slot private structure defaults\n", XP_DRV_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocat slot private structure\n", XP_DRV_NAME));
	return (ts);
}
STATIC void xp_free_ts(struct ts *ts)
{
	psw_t flags;
	struct xp *xp;
	struct sp *sp;
	ensure(ts, return);
	lis_spin_lock_irqsave(&ts->lock, &flags);
	{
		if ((xp = ts->xp)) {
			xp_put(xchg(&ts->xp, NULL));
		}
		if ((sp = ts->sp)) {
			sp_put(xchg(&ts->sp, NULL));
			ts_put(xchg(&sp->slots[ts->slot], NULL));
			ts->slot = 0;
		}
		ss7_bufpool_release(&xp_bufpool, 2);
		bufq_purge(&ts->tx.buf);
		bufq_purge(&ts->rx.buf);
	}
	lis_spin_unlock_irqrestore(&ts->lock, &flags);
	ts_put(ts);		/* final put */
}
STATIC struct ts *ts_get(struct ts *ts)
{
	if (ts)
		atomic_inc(&ts->refcnt);
	return (ts);
}
STATIC void ts_put(struct ts *ts)
{
	if (atomic_dec_and_test(&ts->refcnt)) {
		printd(("%s: freed slot private structure\n", XP_DRV_NAME));
		kmem_cache_free(xp_slot_cachep, ts);
	}
}

/*
 *  Span allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct sp *xp_alloc_sp(struct cd *cd, uint8_t span)
{
	struct sp *sp;
	if ((sp = kmem_cache_alloc(xp_span_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated span private structure\n", XP_DRV_NAME));
		bzero(sp, sizeof(*sp));
		cd_get(cd);	/* first get */
		lis_spin_lock_init(&sp->lock, "sp-priv-lock");
		/* create linkage */
		cd->spans[span] = sp_get(sp);
		sp->cd = cd_get(cd);
		/* fill out span structure */
		printd(("%s: linked span private structure\n", XP_DRV_NAME));
		sp->iobase = cd->iobase + (span << 8);
		sp->span = span;
		sp->config = cd->config;
		sp->config.ifflags = 0;
		sp->config.ifalarms = 0;
		sp->config.ifrxlevel = 0;
		printd(("%s: set span private structure defaults\n", XP_DRV_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate span private structure\n", XP_DRV_NAME));
	return (sp);
}

/* Note: called with card interrupts disabled */
STATIC void xp_free_sp(struct sp *sp)
{
	struct cd *cd;
	ensure(sp, return);
	if ((cd = sp->cd)) {
		int slot;
		/* remove card linkage */
		sp_put(xchg(&cd->spans[sp->span], NULL));
		cd_put(xchg(&sp->cd, NULL));
		sp->span = 0;
		printd(("%s: unlinked span private structure from card\n", XP_DRV_NAME));
		/* remove channel linkage */
		for (slot = 0; slot < 32; slot++) {
			struct xp *xp;
			if (sp->slots[slot] && (xp = sp->slots[slot]->xp)) {
				sp_put(xchg(&xp->sp, NULL));
				xp_free_ts(xchg(&sp->slots[slot], NULL));
			}
		}
		printd(("%s: unlinked span private structure from slots\n", XP_DRV_NAME));
	} else
		ptrace(("%s: ERROR: spans cannot exist without cards\n", XP_DRV_NAME));
}
STATIC struct sp *sp_get(struct sp *sp)
{
	if (sp)
		atomic_inc(&sp->refcnt);
	return (sp);
}
STATIC void sp_put(struct sp *sp)
{
	if (atomic_dec_and_test(&sp->refcnt)) {
		printd(("%s: freed span private structure\n", XP_DRV_NAME));
		kmem_cache_free(xp_span_cachep, sp);
	}
}

/*
 *  Card allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct cd *xp_alloc_cd(void)
{
	struct cd *cd;
	if ((cd = kmem_cache_alloc(xp_card_cachep, SLAB_ATOMIC))) {
		uint32_t *wbuf;
		uint32_t *rbuf;
		printd(("%s: allocated card private structure\n", XP_DRV_NAME));
		if (!(wbuf = kmem_cache_alloc(xp_xbuf_cachep, SLAB_ATOMIC))) {
			ptrace(("%s: could not allocate write buffer\n", XP_DRV_NAME));
			kmem_cache_free(xp_card_cachep, cd);
			return (NULL);
		}
		if (!(rbuf = kmem_cache_alloc(xp_xbuf_cachep, SLAB_ATOMIC))) {
			ptrace(("%s: could not allocate read buffer\n", XP_DRV_NAME));
			kmem_cache_free(xp_xbuf_cachep, wbuf);
			kmem_cache_free(xp_card_cachep, cd);
			return (NULL);
		}
		bzero(cd, sizeof(*cd));
		cd_get(cd);	/* first get */
		lis_spin_lock_init(&cd->lock, "cd-priv-lock");
		if ((cd->next = x400p_cards))
			cd->next->prev = &cd->next;
		cd->prev = &x400p_cards;
		cd->card = x400p_boards++;
		x400p_cards = cd_get(cd);
		cd->wbuf = wbuf;
		cd->rbuf = rbuf;
		tasklet_init(&cd->tasklet, NULL, 0);
		printd(("%s: linked card private structure\n", XP_DRV_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate card private structure\n", XP_DRV_NAME));
	return (cd);
}

/* Note: called with card interrupts disabled and pci resources deallocated */
STATIC void xp_free_cd(struct cd *cd)
{
	psw_t flags;
	lis_spin_lock_irqsave(&cd->lock, &flags);
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
		printd(("%s: unlinked card private structure\n", XP_DRV_NAME));
	}
	lis_spin_unlock_irqrestore(&cd->lock, &flags);
	cd_put(cd);		/* final put */
}
STATIC struct cd *cd_get(struct cd *cd)
{
	if (cd)
		atomic_inc(&cd->refcnt);
	return (cd);
}
STATIC void cd_put(struct cd *cd)
{
	if (atomic_dec_and_test(&cd->refcnt)) {
		kmem_cache_free(xp_card_cachep, cd);
		printd(("%s: freed card private structure\n", XP_DRV_NAME));
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
STATIC void __devexit xp_remove(struct pci_dev *dev)
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
		printd(("%s: freed irq\n", XP_DRV_NAME));
	}
	if (cd->xlb_region) {
		release_mem_region(cd->xlb_region, cd->xlb_length);
		printd(("%s: released xlb region %lx length %ld\n", XP_DRV_NAME, cd->xlb_region, cd->xlb_length));
	}
	if (cd->xll_region) {
		release_mem_region(cd->xll_region, cd->xll_length);
		printd(("%s: released xll region %lx length %ld\n", XP_DRV_NAME, cd->xll_region, cd->xll_length));
	}
	if (cd->plx_region) {
		release_mem_region(cd->plx_region, cd->plx_length);
		printd(("%s: released plx region %lx length %ld\n", XP_DRV_NAME, cd->plx_region, cd->plx_length));
	}
	if (cd->xlb) {
		iounmap((void *) cd->xlb);
		printd(("%s: unmapped xlb memory at %p\n", XP_DRV_NAME, cd->xlb));
	}
	if (cd->xll) {
		iounmap((void *) cd->xll);
		printd(("%s: unmapped xll memory at %p\n", XP_DRV_NAME, cd->xll));
	}
	if (cd->plx) {
		iounmap((void *) cd->plx);
		printd(("%s: unmapped plx memory at %p\n", XP_DRV_NAME, cd->plx));
	}
	xp_free_cd(cd);
      disable:
	pci_disable_device(dev);
}

/*
 *  X400P-SS7 Probe
 *  -----------------------------------
 *  Probes will be called for all PCI devices which match our criteria at pci
 *  init time (module load).  Successful return from the probe function will
 *  have the device configured for operation.
 */
STATIC int __devinit xp_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	int span, b;
	struct cd *cd;
	if (!dev || !id) {
		ptrace(("%s: ERROR: Device or id is null!\n", XP_DRV_NAME));
		return (-ENXIO);
	}
	if (id->driver_data != X400PSS7 && id->driver_data != X400P) {
		ptrace(("%s: ERROR: Driver does not support device type %ld\n", XP_DRV_NAME, id->driver_data));
		return (-ENXIO);
	}
	if (dev->irq < 1) {
		ptrace(("%s: ERROR: No IRQ allocated for device\n", XP_DRV_NAME));
		return (-ENXIO);
	}
	printd(("%s: device allocated IRQ %d\n", XP_DRV_NAME, dev->irq));
	if (pci_enable_device(dev)) {
		ptrace(("%s: ERROR: Could not enable pci device\n", XP_DRV_NAME));
		return (-ENODEV);
	}
	printd(("%s: enabled x400p-ss7 pci device type %ld\n", XP_DRV_NAME, id->driver_data));
	if (!(cd = xp_alloc_cd()))
		return (-ENOMEM);
	pci_set_drvdata(dev, cd);
	if ((pci_resource_flags(dev, 0) & IORESOURCE_IO)
	    || !(cd->plx_region = pci_resource_start(dev, 0))
	    || !(cd->plx_length = pci_resource_len(dev, 0))
	    || !(cd->plx = ioremap(cd->plx_region, cd->plx_length))) {
		ptrace(("%s: ERROR: Invalid PLX 9030 base resource\n", XP_DRV_NAME));
		goto error_remove;
	}
	printd(("%s: plx region %ld bytes at %lx, remapped %p\n", XP_DRV_NAME, cd->plx_length, cd->plx_region,
		cd->plx));
	if ((pci_resource_flags(dev, 2) & IORESOURCE_IO)
	    || !(cd->xll_region = pci_resource_start(dev, 2))
	    || !(cd->xll_length = pci_resource_len(dev, 2))
	    || !(cd->xll = ioremap(cd->xll_region, cd->xll_length))) {
		ptrace(("%s: ERROR: Invalid Xilinx 32-bit base resource\n", XP_DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xll region %ld bytes at %lx, remapped %p\n", XP_DRV_NAME, cd->xll_length, cd->xll_region,
		cd->xll));
	if ((pci_resource_flags(dev, 3) & IORESOURCE_IO)
	    || !(cd->xlb_region = pci_resource_start(dev, 3))
	    || !(cd->xlb_length = pci_resource_len(dev, 3))
	    || !(cd->xlb = ioremap(cd->xlb_region, cd->xlb_length))) {
		ptrace(("%s: ERROR: Invalid Xilinx 8-bit base resource\n", XP_DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xlb region %ld bytes at %lx, remapped %p\n", XP_DRV_NAME, cd->xlb_length, cd->xlb_region,
		cd->xlb));
	cd->config.ifname = xp_board_info[id->driver_data].name;
	if (!request_mem_region(cd->plx_region, cd->plx_length, cd->config.ifname)) {
		ptrace(("%s: ERROR: Unable to reserve PLX memory\n", XP_DRV_NAME));
		goto error_remove;
	}
	printd(("%s: plx region %lx reserved %ld bytes\n", XP_DRV_NAME, cd->plx_region, cd->plx_length));
	if (!request_mem_region(cd->xll_region, cd->xll_length, cd->config.ifname)) {
		ptrace(("%s: ERROR: Unable to reserve Xilinx 32-bit memory\n", XP_DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xll region %lx reserved %ld bytes\n", XP_DRV_NAME, cd->xll_region, cd->xll_length));
	if (!request_mem_region(cd->xlb_region, cd->xlb_length, cd->config.ifname)) {
		ptrace(("%s: ERROR: Unable to reserve Xilinx 8-bit memory\n", XP_DRV_NAME));
		goto error_remove;
	}
	printd(("%s: xlb region %lx reserved %ld bytes\n", XP_DRV_NAME, cd->xlb_region, cd->xlb_length));
	__printd(("%s: card detected %s at 0x%lx/0x%lx irq %d\n", XP_DRV_NAME, cd->config.ifname, cd->xll_region,
		  cd->xlb_region, dev->irq));
#ifdef X400P_DOWNLOAD_FIRMWARE
	{
		uint byte;
		uint8_t *f = (uint8_t *) x400pfw;
		volatile unsigned long *data;
		unsigned long timeout;
		data = (volatile unsigned long *) &cd->plx[GPIOC];
		*data |= GPIO_WRITE;
		*data &= ~GPIO_PROGRAM;
		while (*data & (GPIO_INIT | GPIO_DONE)) ;
		printd(("%s: Xilinx Firmware Load: Init and done are low\n", XP_DRV_NAME));
		*data |= GPIO_PROGRAM;
		while (!(*data & GPIO_INIT)) ;
		printd(("%s: Xilinx Firmware Load: Init is high\n", XP_DRV_NAME));
		*data &= ~GPIO_WRITE;
		printd(("%s: Xilinx Firmware Load: Loading\n", XP_DRV_NAME));
		for (byte = 0; byte < sizeof(x400pfw); byte++) {
			*cd->xlb = *f++;
			if (*data & GPIO_DONE)
				break;
			if (!(*data & GPIO_INIT))
				break;
		}
		if (!(*data & GPIO_INIT)) {
			printd(("%s: ERROR: Xilinx Firmware Load: Failed\n", XP_DRV_NAME));
			goto error_remove;
		}
		printd(("%s: Xilinx Firmware Load: Loaded %d bytes\n", XP_DRV_NAME, byte));
		timeout = jiffies + 20 * HZ / 1000;
		while (jiffies < timeout) ;
		*data |= GPIO_WRITE;
		printd(("%s: Xilinx Firmware Load: Done\n", XP_DRV_NAME));
		timeout = jiffies + 20 * HZ / 1000;
		while (jiffies < timeout) ;
		if (!(*data & GPIO_INIT)) {
			ptrace(("%s: ERROR: Xilinx Firmware Load: Failed\n", XP_DRV_NAME));
			goto error_remove;
		}
		if (!(*data & GPIO_DONE)) {
			ptrace(("%s: ERROR: Xilinx Firmware Load: Failed\n", XP_DRV_NAME));
			goto error_remove;
		}
		printd(("%s: Xilinx Firmware Load: Successful\n", XP_DRV_NAME));
	}
#endif
	cd->plx[INTCSR] = 0;	/* disable interrupts */
	cd->xlb[SYNREG] = 0;
	cd->xlb[CTLREG] = 0;
	cd->xlb[LEDREG] = 0xff;
	if ((b = cd->xlb[0x00f]) & 0x80) {
		int word;
		__printd(("%s: E400P-SS7 (%s Rev. %d)\n", XP_DRV_NAME, xp_e1_framer[(b & 0x30) >> 4], b & 0xf));
		for (word = 0; word < 256; word++) {
			int ebuf;
			cd->xll[word] = 0xffffffff;
			for (ebuf = 0; ebuf < X400P_EBUFNO; ebuf++)
				cd->wbuf[(ebuf << 8) + word] = 0xffffffff;
		}
		/* setup E1 card defaults */
		cd->config = sdl_default_e1_chan;
		if (request_irq(dev->irq, xp_e1_interrupt, SA_INTERRUPT | SA_SHIRQ, XP_DRV_NAME, cd)) {
			ptrace(("%s: ERROR: Unable to request IRQ %d\n", XP_DRV_NAME, dev->irq));
			goto error_remove;
		}
		cd->irq = dev->irq;
		printd(("%s: acquired IRQ %ld for E400P-SS7 card\n", XP_DRV_NAME, cd->irq));
		cd->config.ifflags = (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
		tasklet_init(&cd->tasklet, &xp_e1_card_tasklet, (unsigned long) cd);
	} else {
		int word;
		__printd(("%s: T100P-SS7 (%s Rev. %d)\n", XP_DRV_NAME, xp_t1_framer[(b & 0x30) >> 4], b & 0xf));
		for (word = 0; word < 256; word++) {
			int ebuf;
			cd->xll[word] = 0x7f7f7f7f;
			for (ebuf = 0; ebuf < X400P_EBUFNO; ebuf++)
				cd->wbuf[(ebuf << 8) + word] = 0x7f7f7f7f;
		}
		/* setup T1 card defaults */
		cd->config = sdl_default_t1_chan;
		if (request_irq(dev->irq, xp_t1_interrupt, SA_INTERRUPT | SA_SHIRQ, XP_DRV_NAME, cd)) {
			ptrace(("%s: ERROR: Unable to request IRQ %d\n", XP_DRV_NAME, dev->irq));
			goto error_remove;
		}
		cd->irq = dev->irq;
		printd(("%s: acquired IRQ %ld for T400P-SS7 card\n", XP_DRV_NAME, cd->irq));
		cd->config.ifflags = (SDL_IF_UP | SDL_IF_TX_RUNNING | SDL_IF_RX_RUNNING);
		tasklet_init(&cd->tasklet, &xp_t1_card_tasklet, (unsigned long) cd);
	}
	/* allocate span structures */
	for (span = 0; span < X400_SPANS; span++)
		if (!xp_alloc_sp(cd, span))
			goto error_remove;
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
STATIC int xp_suspend(struct pci_dev *pdev, u32 state)
{
	fixme(("Write a suspend routine.\n"));
	return 0;
}

/*
 *  X400P-SS7 Resume
 *  -----------------------------------
 */
STATIC int xp_resume(struct pci_dev *pdev)
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
STATIC INLINE int xp_pci_init(void)
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
STATIC INLINE void xp_pci_cleanup(void)
{
	return pci_unregister_driver(&xp_driver);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization (For unregistered driver.)
 *
 *  =========================================================================
 */
STATIC int xp_initialized = 0;
STATIC void xp_init(void)
{
	int rtn, major;
	unless(xp_initialized, return);
	cmn_err(CE_NOTE, X400P_BANNER);	/* console splash */
	if ((rtn = xp_init_caches())) {
		cmn_err(CE_PANIC, "%s: ERROR: Could not allocate caches", XP_DRV_NAME);
		xp_initialized = rtn;
		return;
	}
	if ((rtn = xp_pci_init()) < 0) {
		cmn_err(CE_WARN, "%s: ERROR: No PCI devices found", XP_DRV_NAME);
		xp_term_caches();
		xp_initialized = rtn;
		return;
	}
	ss7_bufpool_init(&xp_bufpool);
	for (major = 0; major < SL_X400P_CMAJORS; major++) {
		if ((rtn = lis_register_strdev(xp_majors[major], &xp_info, X400P_NMINOR, XP_DRV_NAME)) <= 0) {
			if (!major) {
				cmn_err(CE_PANIC, "%s: Could not register 1'st major %d", XP_DRV_NAME,
					xp_majors[0]);
				ss7_bufpool_term(&xp_bufpool);
				xp_pci_cleanup();
				xp_term_caches();
				xp_initialized = rtn;
				return;
			}
			cmn_err(CE_WARN, "%s: Could not register %d'th major", XP_DRV_NAME, major + 1);
			xp_majors[major] = 0;
		} else if (major)
			xp_majors[major] = rtn;
	}
	lis_spin_lock_init(&xp_lock, "x400p-open-list-lock");
	xp_initialized = 1;
	return;
}
STATIC void xp_terminate(void)
{
	int rtn, major;
	ensure(xp_initialized, return);
	for (major = 0; major < SL_X400P_CMAJORS; major++) {
		if (xp_majors[major]) {
			if ((rtn = lis_unregister_strdev(xp_majors[major])))
				cmn_err(CE_PANIC, "%s: couldn't unregister driver for major %d\n", XP_DRV_NAME,
					xp_majors[major]);
			if (major)
				xp_majors[major] = 0;
		}
	}
	xp_initialized = 0;
	ss7_bufpool_term(&xp_bufpool);
	xp_pci_cleanup();
	xp_term_caches();
	return;
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int init_module(void)
{
	xp_init();
	if (xp_initialized < 0)
		return xp_initialized;
	return (0);
}
void cleanup_module(void)
{
	xp_terminate();
}
