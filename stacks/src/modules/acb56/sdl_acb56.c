/*****************************************************************************

 @(#) $Id: sdl_acb56.c,v 0.9.2.8 2007/08/15 05:18:18 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/15 05:18:18 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdl_acb56.c,v $
 Revision 0.9.2.8  2007/08/15 05:18:18  brian
 - GPLv3 updates

 Revision 0.9.2.7  2007/08/14 12:17:18  brian
 - GPLv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: sdl_acb56.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:18:18 $"

static char const ident[] =
    "$RCSfile: sdl_acb56.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:18:18 $";

/*
 *  This is an implementation of the Signalling Data Link for the SeaLevel
 *  ACB56[tm] V.35 ISA card.  It provides the driver routines necessary for
 *  interface to the SDL driver kernel module.
 */

#ifdef LINUX
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/dma.h>
#endif				/* LINUX */

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/devi.h>
#include <ss7/devi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

/*
   FIXME: this driver needs to be converted to not use these includes... 
 */
#include "../lmi/lm.h"
#include "../devi/dev.h"
#include "../sdli/sdl.h"

#define ACB56_DESCRIP	"ACB56: SS7/SDL (Signalling Data Link) STREAMS DRIVER."
#define ACB56_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corpoation.  All Rights Reserved."
#define ACB56_DEVICES	"Supports the SeaLevel ACB56(tm) V.35 boards."
#define ACB56_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ACB56_LICENSE	"GPL"
#define ACB56_BANNER	ACB56_DESCRIP   "\n" \
			ACB56_COPYRIGHT "\n" \
			ACB56_DEVICES   "\n" \
			ACB56_CONTACT   "\n"

#ifdef MODULE
MODULE_AUTHOR(ACB56_CONTACT);
MODULE_DESCRIPTION(ACB56_DESCRIP);
MODULE_SUPPORTED_DEVICE(ACB56_DEVICES);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ACB56_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdl_acb56");
#endif
#endif

#ifdef ACB56_DEBUG
int acb56_debug = ACB56_DEBUG;
#else
int acb56_debug = 2;
#endif

#define DEBUG_LEVEL acb56_debug

// #define ACB56_MOD_ID 0x1111
// #define ACB56_MOD_NAME "acb56"
// #define ACB56_CMAJOR 0 /* FIXME: pick something */
// #define ACB56_NMINOR 256 /* as many as possible */

#ifndef ACB56_CMAJOR
#define ACB56_CMAJOR  248	/* FIXME: pick something */
#endif
#define ACB56_NMINOR  5		/* as many as possible */

#ifndef LSSU_SIB
#define LSSU_SIB 0x5
#endif

int debug = -1;
int io[] = { -1, -1, -1, -1, -1 };
int irq[] = { -1, -1, -1, -1, -1 };
int dma_rx[] = { -1, -1, -1, -1, -1 };
int dma_tx[] = { -1, -1, -1, -1, -1 };
int mode[] = { -1, -1, -1, -1, -1 };
int clock[] = { -1, -1, -1, -1, -1 };

MODULE_PARM(debug, "i");	/* debug flag */
MODULE_PARM(io, "1-5i");	/* i/o port for the i'th card */
MODULE_PARM(irq, "1-5i");	/* irq for the i'th card */
MODULE_PARM(dma_rx, "1-5i");	/* dma for the i'th card */
MODULE_PARM(dma_tx, "1-5i");	/* dma for the i'th card */
MODULE_PARM(mode, "i-5i");	/* interface mode */
MODULE_PARM(clock, "i-5i");	/* clock source */

enum { ACB56_MODE_DTE, ACB56_MODE_DCE, ACB56_MODE_LOOP,
	ACB56_MODE_JACK, ACB56_MODE_ECHO, ACB56_MODE_BOTH
};

static const int mode_map[] = {
	-1,				/* DEV_MODE_NONE */
	-1,				/* DEV_MODE_DSU */
	-1,				/* DEV_MODE_CSU */
	ACB56_MODE_DTE,			/* DEV_MODE_DTE */
	ACB56_MODE_DCE,			/* DEV_MODE_DCE */
	-1,				/* DEV_MODE_CLIENT */
	-1,				/* DEV_MODE_SERVER */
	-1,				/* DEV_MODE_PEER */
	ACB56_MODE_ECHO,		/* DEV_MODE_ECHO */
	-1,				/* DEV_MODE_REM_LB */
	ACB56_MODE_LOOP,		/* DEV_MODE_LOC_LB */
	ACB56_MODE_BOTH,		/* DEV_MODE_LB_ECHO */
	ACB56_MODE_JACK			/* DEV_MODE_TEST */
};

enum { ACB56_CLOCK_EXT, ACB56_CLOCK_INT, ACB56_CLOCK_DPLL };

static const int clock_map[] = {
	-1,				/* DEV_CLOCK_NONE */
	ACB56_CLOCK_INT,		/* DEV_CLOCK_INT */
	ACB56_CLOCK_EXT,		/* DEV_CLOCK_EXT */
	-1,				/* DEV_CLOCK_LOOP */
	-1,				/* DEV_CLOCK_MASTER */
	-1,				/* DEV_CLOCK_SLAVE */
	ACB56_CLOCK_DPLL,		/* DEV_CLOCK_DPLL */
	-1,				/* DEV_CLOCK_ABR */
	-1				/* DEV_CLOCK_SHAPER */
};

static int acb56_cmajor = ACB56_CMAJOR;

typedef struct acb56_stats {
	dev_ulong repeated_sus;
	dev_ulong compressed_sus;
	dev_ulong recv_fisus;
	dev_ulong recv_lssus;
	dev_ulong recv_msus;
	dev_ulong parity_errors;
	dev_ulong cts_transitions;
	dev_ulong dcd_transitions;
	dev_ulong cha_ext_status;
	dev_ulong cha_rx_char_avail;
	dev_ulong cha_rx_sp_cond;
	dev_ulong cha_tx_buf_empty;
	dev_ulong chb_ext_status;
	dev_ulong chb_rx_char_avail;
	dev_ulong chb_rx_sp_cond;
	dev_ulong chb_tx_buf_empty;
	dev_ulong interrupts;
} acb56_stats_t;

typedef struct acb56 {
	struct dev dev;			/* common device structure */
	int rx_octet_mode;		/* Is octet counting on? */
	int rx_octet_count;		/* bits (not octets) counted */
	unsigned char regs[16];		/* register images */
	unsigned char rr0;		/* register image reg 0 */
	mblk_t *tx_msg;			/* transmit buffer */
	mblk_t *rx_msg;			/* receive buffer */
	mblk_t *cp_msg;			/* compressed su buffer */
	acb56_stats_t stats;		/* device private stats */
	unsigned char *tx_buf;		/* current tx buffer pointer */
	unsigned char *tx_max;		/* end of tx buffer pointer */
	int tx_error;			/* transmission error */
	unsigned char *rx_buf;		/* current rx buffer pointer */
	unsigned char *rx_max;		/* end oft rx buffer pointer */
	int rx_error;			/* reception error */
	bufq_t tinputq;
} acb56_t;

/*
 *  =========================================================================
 *
 *  BUFFER RESUPPLY
 *
 *  =========================================================================
 */

/*
 *  It is important not to attempt to free or allocate message blocks within
 *  the Interrupt Service Routine.  The purpose of these functions is to
 *  recognize when the receive buffer supply queue is running too low and
 *  perform a resupply of the queue by allocating buffers.  We also detect
 *  when the buffer supply queue is running too high and free buffers from the
 *  queue.  We use a single resupply queue for all of the ACB56 devices served
 *  by this driver.
 */

#define ACB56_SUPQ_MAXLEN   16

#ifndef SIF_MAX
#define SIF_MAX 272
#endif

static bufq_t acb56_supplyq;
static bufq_t acb56_returnq;

static void
acb56_resupply(void *unused)
{
	int i;
	mblk_t *mp;

	(void) unused;
	for (i = bufq_length(&acb56_returnq); i > 0; i--) {
		if ((mp = bufq_dequeue(&acb56_returnq)))
			freemsg(mp);
	}
	for (i = bufq_length(&acb56_supplyq); i < ACB56_SUPQ_MAXLEN; i++) {
		if ((mp = allocb(SIF_MAX + 4, BPRI_HI))) {
			mp->b_datap->db_type = M_DATA;
			bufq_queue(&acb56_supplyq, mp);
		}
	}
}

static struct tq_struct acb56_resupply_tasq = {
	{NULL, NULL},		/* list */
	0,			/* sync */
	(void *) acb56_resupply,	/* routine */
	NULL			/* data */
};

static inline void
acb56_do_resupply(void)
{
	queue_task(&acb56_resupply_tasq, &tq_immediate);
	mark_bh(IMMEDIATE_BH);
}

/*
 *  =========================================================================
 *
 *  INTERRUPT SERVICE ROUTINES
 *
 *  =========================================================================
 */

static inline void
acb56_tx_setup_next_frame(acb56_t * p)
{
	p->dev.module->stats.tx_sus++;
	p->dev.module->stats.tx_bytes += 4;
	if (bufq_length(&p->tinputq)) {
		acb56_do_resupply();	/* mark BH for resupply */
		bufq_queue(&acb56_returnq, p->tx_msg);
		p->tx_msg = bufq_dequeue(&p->tinputq);
	} else {
		int len = msgdsize(p->tx_msg);

		if (len > 5 || (len > 3 && p->tx_msg->b_rptr[3] == LSSU_SIB)) {	/* its an MSU or
										   SIB, make FISU
										   out of it */
			p->tx_msg->b_wptr = p->tx_msg->b_rptr + 3;
			p->tx_msg->b_rptr[2] = 0;
			p->stats.repeated_sus++;
		}
	}
	p->tx_buf = p->tx_msg->b_rptr;
	p->tx_max = p->tx_msg->b_wptr;
}

static inline void
acb56_tx_underrun_eom(acb56_t * p)
{
	if (p->rr0 & 0x40) {	/* set */
		p->tx_buf = p->tx_msg->b_rptr;
		p->tx_error = 1;
		p->dev.module->stats.tx_aborts++;
		p->dev.module->stats.tx_underruns++;
		p->dev.module->stats.tx_sus_in_error++;
		p->dev.module->stats.tx_bytes += 3;
		p->rr0 &= ~0x04;	/* clear indication */
		return;
	} else {
		return acb56_tx_setup_next_frame(p);
	}
}

static inline void
acb56_sync_hunt(acb56_t * p)
{
	int actrl = p->dev.iface.iobase + 1;

	if (p->rx_octet_mode && !p->rr0 & 0x10) {
		p->rx_octet_mode = 0;	/* we synced */
		outb(0x0f, actrl);
		outb(p->regs[0x0f] &= ~0x02, actrl);	/* turn of octet mode */
	} else if ((p->rr0 & 0x10) && (p->dev.iface.ifclock != DEV_CLOCK_DPLL)) {
		p->rx_octet_count = 0;
		p->rx_octet_mode = 1;
		outb(0x0f, actrl);
		outb(p->regs[0x0f] |= 0x02, actrl);	/* turn on octet mode */
	}
	p->dev.module->stats.rx_sync_transitions++;
}
static inline void
acb56_break_abort(acb56_t * p)
{
	p->dev.module->stats.rx_aborts++;
}
static inline void
acb56_dcd(acb56_t * p)
{
	if ((++p->stats.dcd_transitions) & 0x1)
		p->dev.module->stats.lead_dcd_lost++;
}
static inline void
acb56_cts(acb56_t * p)
{
	if ((++p->stats.cts_transitions) & 0x1)
		p->dev.module->stats.lead_cts_lost++;
}
static inline void
acb56_zero_count(acb56_t * p)
{
	if (!p->rx_octet_count++ & 0x0000007f || (p->dev.iface.ifmode == DEV_MODE_DTE)) {
		p->dev.module->stats.rx_sus_in_error++;
		p->dev.ucalls->daedr_N_octets(&p->dev);
	}
	p->dev.module->stats.rx_bits_octet_counted++;
}

static inline void
acb56_frame_error(acb56_t * p)
{
	p->rx_error = 0;
	p->rx_buf = p->rx_msg->b_rptr;
	if (p->rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	p->cp_msg->b_wptr = p->cp_msg->b_rptr;
	p->dev.module->stats.rx_sus++;
	p->dev.module->stats.rx_bytes += 2;
	p->dev.module->stats.rx_sus_in_error++;
	p->dev.ucalls->daedr_error_frame(&p->dev);
}
static inline void
acb56_parity_error(acb56_t * p)
{
	p->stats.parity_errors++;
	p->rx_error = 1;
}
static inline void
acb56_rx_overrun(acb56_t * p)
{
	p->dev.module->stats.rx_overruns++;
	p->rx_error = 1;
}
static inline void
acb56_crc_error(acb56_t * p)
{
	if (p->rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	p->dev.module->stats.rx_crc_errors++;
	acb56_frame_error(p);
}
static inline void
acb56_buffer_error(acb56_t * p)
{
	if (p->rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	p->dev.module->stats.rx_buffer_overflows++;
	acb56_frame_error(p);
}
static inline void
acb56_short_error(acb56_t * p)
{
	if (p->rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	p->dev.module->stats.rx_frame_too_short++;
	acb56_frame_error(p);
}
static inline void
acb56_long_error(acb56_t * p)
{
	if (p->rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	p->dev.module->stats.rx_frame_too_long++;
	acb56_frame_error(p);
}
static inline void
acb56_length_error(acb56_t * p)
{
	if (p->rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	p->dev.module->stats.rx_length_error++;
	acb56_frame_error(p);
}
static inline void
acb56_rx_setup_next_frame(acb56_t * p)
{
	int len = p->rx_buf - p->rx_msg->b_rptr;

	p->rx_buf = p->rx_msg->b_rptr;
	p->dev.module->stats.rx_sus++;
	if (len == 3)
		p->stats.recv_fisus++;
	if (len >= 6)
		p->stats.recv_msus++;
	if (len >= 4)
		p->stats.recv_lssus++;
	p->dev.module->stats.rx_bytes += 2;
}
static inline void
acb56_residue_error(acb56_t * p)
{
	p->dev.module->stats.rx_residue_errors++;
	acb56_frame_error(p);
}
static inline void
acb56_end_of_frame(acb56_t * p)
{
	if (p->rx_error)
		return acb56_frame_error(p);
	{
		mblk_t *mp = p->rx_msg, *mc = p->cp_msg;
		unsigned int len = (unsigned int) (p->rx_buf - mp->b_rptr);
		unsigned char li = mp->b_rptr[2] & 0x3f;

		if (len < 3)
			return acb56_short_error(p);
		if (len > p->dev.module->config.m + 4)
			return acb56_long_error(p);
		if (li != (len > 63 + 3 ? 63 + 3 : len))
			return acb56_length_error(p);
		mp->b_wptr = mp->b_rptr + len;
		if (len < 6) {
			int clen = mc->b_wptr - mc->b_rptr;

			if (len == clen && !memcmp(mc->b_rptr, mp->b_rptr, len)) {
				p->stats.compressed_sus++;
				acb56_rx_setup_next_frame(p);
				p->dev.ucalls->daedr_compr_frame(&p->dev, 1);
				return;
			} else {
				memcpy(mc->b_rptr, mp->b_rptr, len);
				mc->b_wptr = mc->b_rptr + len;
			}
		} else
			mc->b_wptr = mc->b_rptr;
		p->rx_octet_mode = 0;
		acb56_rx_setup_next_frame(p);
		p->dev.ucalls->daedr_recvd_frame(&p->dev, mp);
		acb56_do_resupply();	/* mark BH for resupply */
		if (!(p->rx_msg = bufq_dequeue(&acb56_supplyq))
		    && !(p->rx_msg = bufq_dequeue(&acb56_returnq)))
			return acb56_buffer_error(p);
		p->rx_buf = p->rx_msg->b_wptr = p->rx_msg->b_rptr;
		p->rx_max = p->rx_msg->b_datap->db_lim;
		return;
	}
}
static inline void
acb56_frame_overflow_check(acb56_t * p)
{
	int actrl = p->dev.iface.iobase + 1;

	/* 
	   check for frame overflow */
	if (p->rx_buf > p->rx_max) {
		/* 
		   FIGURE 11/Q.703 (sheet 1 of 2) "m+7 octets without flags" */
		if (!p->rx_octet_mode && p->dev.iface.ifclock != DEV_CLOCK_DPLL) {	/* can't
											   octet
											   count on 
											   DPLL! */
			p->rx_octet_count = 0;
			p->rx_octet_mode = 1;
			outb(0x0f, actrl);
			outb(p->regs[0x0f] |= 0x02, actrl);	/* octet counting isr */
			outb(0x03, actrl);
			outb(p->regs[0x03] |= 0x10, actrl);	/* force flag hunt */
			acb56_rx_setup_next_frame(p);
			p->dev.ucalls->daedr_loss_of_sync(&p->dev);
		}
	}
}

static void
acb56_isr_cha_tx_buf_empty(acb56_t * p)
{
	p->stats.cha_tx_buf_empty++;
}

static void
acb56_isr_cha_ext_status(acb56_t * p)
{
	unsigned char rr0;
	register int actrl = p->dev.iface.iobase + 1;

	rr0 = p->rr0 & ~0x02;
	outb(0x00, actrl);
	p->rr0 = inb(actrl);
	outb(0x00, actrl);
	outb(0x10, actrl);
	outb(0x00, actrl);
	outb(0x10, actrl);	/* debounce */
	rr0 ^= p->rr0 & 0xfa;
	if (rr0) {
		if (rr0 & 0x40)
			acb56_tx_underrun_eom(p);
		if (rr0 & 0x10)
			acb56_sync_hunt(p);
		if (rr0 & 0x80)
			acb56_break_abort(p);
		if (rr0 & 0x08)
			acb56_dcd(p);
		if (rr0 & 0x20)
			acb56_cts(p);
	} else
		acb56_zero_count(p);
	p->stats.cha_ext_status++;
}

static void
acb56_isr_cha_rx_char_avail(acb56_t * p)
{
	register int adata = p->dev.iface.iobase;
	register int i = 0;

	if (p->rx_buf) {
		/* 
		   collect bytes */
		for (i = 0; i < 4; i++)
			*(p->rx_buf++) = inb(adata);
		acb56_frame_overflow_check(p);
	}
	p->dev.module->stats.rx_bytes += 4;
	p->stats.cha_rx_char_avail++;

}
static void
acb56_isr_cha_rx_sp_cond(acb56_t * p)
{
	unsigned char rr1 = 0;
	register int adata = p->dev.iface.iobase;
	register int actrl = p->dev.iface.iobase + 1;
	register int i = 0;

	p->stats.cha_rx_sp_cond++;
	/* 
	   collect bytes */
	outb(0x00, actrl);
	for (i = 0; i < 4 && (inb(actrl) & 0x01); i++) {
		*(p->rx_buf++) = inb(adata);
		p->dev.module->stats.rx_bytes++;
		outb(0x00, actrl);
	}
	acb56_frame_overflow_check(p);
	/* 
	   check for special conditions */
	outb(0x07, actrl);
	if (inb(actrl) & 0x40) {
		outb(0x01, actrl);
		if ((rr1 = inb(actrl)) & 0xf0) {
			outb(0x00, actrl);
			outb(0x30, actrl);	/* reset error */
			if (rr1 & 0x10) {
				return acb56_parity_error(p);
			}
			if (rr1 & 0x20) {
				return acb56_rx_overrun(p);
			}
			if (rr1 & 0x80) {
				if (rr1 & 0x40) {
					return acb56_crc_error(p);
				} else {
					if ((rr1 & 0xe) ^ 0x6) {
						return acb56_residue_error(p);
					} else {
						return acb56_end_of_frame(p);
					}
				}
			}
		}
	}
}

static void
acb56_isr_donothing(acb56_t * p)
{
	(void) p;
};

static void
acb56_isr_chb_tx_buf_empty(acb56_t * p)
{
	p->stats.chb_tx_buf_empty++;
}
static void
acb56_isr_chb_ext_status(acb56_t * p)
{
	p->stats.chb_ext_status++;
}
static void
acb56_isr_chb_rx_char_avail(acb56_t * p)
{
	p->stats.chb_rx_char_avail++;
}
static void
acb56_isr_chb_rx_sp_cond(acb56_t * p)
{
	p->stats.chb_rx_sp_cond++;
}

static void
acb56_isr(int irq, void *dev_id, struct pt_regs *regs)
{
	static void (*vector_map[]) (acb56_t *) = {
	acb56_isr_chb_tx_buf_empty, acb56_isr_chb_ext_status, acb56_isr_chb_rx_char_avail,
		    acb56_isr_chb_rx_sp_cond, acb56_isr_cha_tx_buf_empty,
		    acb56_isr_cha_ext_status, acb56_isr_cha_rx_char_avail,
		    acb56_isr_cha_rx_sp_cond};
	unsigned char rr3;
	register int i;
	register int actrl = ((acb56_t *) dev_id)->dev.iface.iobase + 1;

	for (i = 0, outb(0x03, actrl), rr3 = inb(actrl); i < 4 && rr3;
	     i++, outb(0x03, actrl), rr3 = inb(actrl)) {
		outb(0x02, actrl + 2);
		(*vector_map[inb(actrl + 2) >> 1]) (dev_id);
		/* 
		   reset highest interrupt under service */
		outb(0x00, actrl);
		outb(0x38, actrl);
	}
	((acb56_t *) dev_id)->stats.interrupts++;
};

/*
 *  =========================================================================
 *
 *  DRIVER SERVICE CALLS
 *
 *  =========================================================================
 */

static dev_device_t dev_acbdev_default = {
	SPIN_LOCK_UNLOCKED,		/* iflock */
	0,				/* ifflags */
	DEV_TYPE_V35,			/* iftype */
	DEV_GTYPE_NONE,			/* ifgtype */
	DEV_MODE_DTE,			/* ifmode */
	56000,				/* ifrate */
	DEV_CLOCK_DPLL,			/* ifclock */
	DEV_CODING_NRZI,		/* ifcoding */
	0,				/* ifleads */
	0,				/* ifindex */
	0,				/* irq */
	0,				/* dma_rx */
	0				/* dma_tx */
};

static const unsigned char preamble[] = {
	0x09, 0xC0, 0x0F, 0x01, 0x07, 0x6B, 0x0F, 0x00, 0x00, 0x00, 0x04, 0x20,
	0x01, 0x00, 0x02, 0x00, 0x03, 0xCA, 0x05, 0x63, 0x06, 0x00, 0x07, 0x7e,
	0x09, 0x00, 0x0A, 0x00, 0x0B, 0x16, 0x0C, 0x40, 0x0D, 0x00, 0x0E, 0x02,
	0x0E, 0x02, 0x0E, 0x02, 0x0E, 0x03, 0x03, 0xCB, 0x05, 0x6B, 0x00, 0x80,
	0x00, 0x30, 0x01, 0x00, 0x0F, 0x00, 0x00, 0x10, 0x00, 0x10, 0x01, 0x00,
	0x09, 0x00
};
static const unsigned char preset[] = {
	0x09, 0xc0, 0x00, 0x00, 0x04, 0x20, 0x03, 0xca, 0x05, 0xe3, 0x07, 0x7e,
	0x06, 0x00, 0x0F, 0x01, 0x07, 0x6b, 0x0F, 0x00, 0x01, 0x00, 0x02, 0x00,
	0x09, 0x00, 0x0A, 0x80
};
static const unsigned char mode_clock[6][3] = {
	{0x08, 0x05, 0x7f}, {0x08, 0x56, 0x7f}, {0x50, 0x50, 0x78},
	{0x16, 0x50, 0x1f}, {0x50, 0x50, 0x78}, {0x50, 0x50, 0x78}
};

static unsigned char irqprobe[] = {
	0x01, 0x19, 0x0F, 0xFA, 0x00, 0x10, 0x00, 0x10, 0x09, 0x08, 0x0E, 0x03
};

static void
dummy_isr(int irq, void *dev_id, struct pt_regs *regs)
{
	volatile int *p;

	(void) irq;
	(void) dev_id;
	(void) regs;
	p = NULL;
	p++;
}

static int board = 0;
static int ports[] = { 0x238, 0x280, 0x2A0, 0x300, 0x328, 0 };

/*
 *  DEVICE-ATTACH: This is the device attach.  It should probe for the device
 *  as specified by the minor device number and determine whether the device
 *  is there and all of the resources for the device can be acquired.  All
 *  resources associated with the device (except the allocated structure,
 *  which is freed by the caller) should be freed at the end of this routine
 *  so that the device may be used by some other module if required.
 */
static lmi_t *
acb56_devatt(dev_t dev)
{
	int iobase, _irq, actrl, _dma_rx, _dma_tx, i, err;
	unsigned long time, cookie;
	acb56_t *p = NULL;

	board = getminor(dev) - 1;

	if ((iobase = io[board]) == -1)
		iobase = ports[board];
	if ((err = check_region(iobase, 8)))
		return NULL;
	actrl = iobase + 1;
	outb(0x02, actrl);
	outb(0x55, actrl);	/* write to unallocated 8530 bit in WR2 */
	outb(0x02, actrl);
	if (inb(actrl) != 0x55)
		return NULL;	/* probably an 8530 */
	outb(0x09, actrl);
	outb(0xc0, actrl);	/* force hardware reset */
	outb(0x0f, actrl);
	outb(0x01, actrl);	/* Access W7P register */
	outb(0x0f, actrl);
	if (!inb(actrl) & 0x01)
		return NULL;	/* probably an 8530 */
	outb(0x0f, actrl);
	outb(0x00, actrl);	/* Remove accss to W7P register */
	outb(0x0f, actrl);
	if (inb(actrl) & 0x01)
		return NULL;	/* probably an 8530 */
	/* 
	   check assigned irq */
	if ((_irq = irq[board]) != -1) {
		if ((err = request_irq(_irq, dummy_isr, SA_SHIRQ, "acb56_dummy", NULL)))
			return NULL;
		else
			goto acb_probe_dma;
	}
	for (i = 0; i < sizeof(preamble);) {	/* setup chip */
		outb(preamble[i], actrl);
		i++;
		outb(preamble[i], actrl);
		i++;
	}
	cookie = probe_irq_on();
	for (i = 0; i < sizeof(irqprobe);) {	/* setup for guaranteed interrupt */
		outb(irqprobe[i], actrl);
		i++;
		outb(irqprobe[i], actrl);
		i++;
	}
	/* 
	   fill tx fifo to get an interrupt */
	outb(0x55, iobase);
	outb(0x55, iobase);
	outb(0x55, iobase);
	outb(0x55, iobase);
	outb(0x55, iobase);
	outb(0x55, iobase);
	outb(0x55, iobase);
	outb(0x55, iobase);
	outb(0x55, iobase);
	for (time = jiffies; jiffies - time < 100; i++) ;
	if (!(_irq = probe_irq_off(cookie)))
		return NULL;	/* no irq! */
	outb(0x03, actrl);
	if (!inb(actrl))
		return NULL;	/* it wasn't us */
	outb(0x09, actrl);
	outb(0x00, actrl);
	outb(0x09, actrl);
	outb(0xc0, actrl);	/* force hardware reset */
	if ((err = request_irq(_irq, dummy_isr, SA_SHIRQ, "acb56_dummy", NULL)))
		return NULL;
      acb_probe_dma:
	free_irq(_irq, NULL);
	/* 
	   check for dma */
	if ((_dma_rx = dma_rx[board]) && _dma_rx != -1 && !(request_dma(_dma_rx, "acb56_probe")))
		free_dma(_dma_rx);
	else
		_dma_rx = 0;
	if ((_dma_tx = dma_tx[board]) && _dma_tx != -1 && !(request_dma(_dma_tx, "acb56_probe")))
		free_dma(_dma_tx);
	else
		_dma_tx = 0;

	if (!(p = kmalloc(sizeof(*p), GFP_KERNEL)))
		return NULL;
	bzero(p, sizeof(*p));
	bcopy(&dev_acbdev_default, &p->dev.iface, sizeof(p->dev.iface));

	p->dev.iface.ifindex = board;
	p->dev.iface.irq = _irq;
	p->dev.iface.iobase = iobase;
	p->dev.iface.dma_tx = _dma_tx;
	p->dev.iface.dma_rx = _dma_rx;

	if (mode[board] != -1)
		p->dev.iface.ifmode = mode[board];
	if (clock[board] != -1)
		p->dev.iface.ifclock = clock[board];

	ptrace(("sucessful:\n"));
	ptrace(("  ifindex = %lu\n", p->dev.iface.ifindex));
	ptrace(("  irq     = %lu\n", p->dev.iface.irq));
	ptrace(("  iobase  = 0x%lx\n", p->dev.iface.iobase));
	ptrace(("  dma_tx  = %lu\n", p->dev.iface.dma_tx));
	ptrace(("  dma_rx  = %lu\n", p->dev.iface.dma_rx));

	return ((lmi_t *) p);
}

/*
 *  DEVICE-OPEN: Using the information contained in the device structure
 *  created at device attach, this function should reacquire all of the
 *  resources associated with the device (e.g., irqs).  The device is left in
 *  the quiescent state.
 */
static int
acb56_open(lmi_t * lmi)
{
	acb56_t *p = (acb56_t *) lmi;
	dev_device_t *dev = &p->dev.iface;
	int err = 0;
	unsigned long flags;

	spin_lock_irqsave(&dev->iflock, flags);
	MOD_INC_USE_COUNT;
	/* 
	   get io region */
	if ((err = check_region(dev->iobase, 8))) {
		MOD_DEC_USE_COUNT;
		spin_unlock_irqrestore(&dev->iflock, flags);
		return err;
	}
	request_region(dev->iobase, 8, "acb56");
	/* 
	   get dma channels */
	if (dev->dma_rx && request_dma(dev->dma_rx, "acb56"))
		dev->dma_rx = 0;
	if (dev->dma_tx && request_dma(dev->dma_tx, "acb56"))
		dev->dma_tx = 0;
	/* 
	   get interrupt */
	if ((err = request_irq(dev->irq, acb56_isr, SA_SHIRQ, "acb56", p))) {
		if (dev->dma_rx)
			free_dma(dev->dma_rx);
		if (dev->dma_tx)
			free_dma(dev->dma_tx);
		MOD_DEC_USE_COUNT;
		spin_unlock_irqrestore(&dev->iflock, flags);
		return err;
	}
	spin_unlock_irqrestore(&dev->iflock, flags);
	return (0);
}

/*
 *  DEVICE-CLOSE: Using the information contained in the device structure
 *  created at device attach, this function should deallocate all of the
 *  driver resources associated with the device with the exception of the
 *  device structure itself.  The device will be in the quiescent state when
 *  the procedure is called.
 */
static int
acb56_close(lmi_t * lmi)
{
	acb56_t *p = (acb56_t *) lmi;
	dev_device_t *dev = &p->dev.iface;
	unsigned long flags;

	spin_lock_irqsave(&dev->iflock, flags);
	free_irq(dev->irq, p);
	if (dev->dma_tx)
		free_dma(dev->dma_tx);
	if (dev->dma_rx)
		free_dma(dev->dma_rx);
	release_region(dev->iobase, 8);

	MOD_DEC_USE_COUNT;
	spin_unlock_irqrestore(&dev->iflock, flags);
	return (0);
}

/*
 *  INFO: This is for Style 2 device drivers for returning PPAs.  Since this
 *  is only a single-port card, the Signalling Data Link is determined at open
 *  time and we simply return a zero-length PPA.
 */
static int
acb56_info(lmi_t * lmi, void **ppap, int *lenp)
{
	acb56_t *p = (acb56_t *) lmi;
	dev_device_t *dev = &p->dev.iface;
	unsigned long flags;

	(void) dev;
	spin_lock_irqsave(&dev->iflock, flags);
	*lenp = 0;
	*ppap = NULL;
	spin_unlock_irqrestore(&dev->iflock, flags);
	return (0);
}

/*
 *  ATTACH: This is for Style 2 device drivers.  Since this is only a
 *  single-port card, the Signalling Data Link is determined at open time and
 *  we simply agree with any attach command.
 */
static int
acb56_attach(lmi_t * lmi, void *ppa, int len)
{
	acb56_t *p = (acb56_t *) lmi;
	dev_device_t *dev = &p->dev.iface;
	unsigned long flags;

	(void) dev;
	(void) ppa;
	(void) len;
	spin_lock_irqsave(&dev->iflock, flags);
	spin_unlock_irqrestore(&dev->iflock, flags);
	return (0);
}

/*
 *  DETACH: This is for Style 2 device drivers.  Since this is only a
 *  single-port card, the Signalling Data LInk was determined at open time and
 *  cannot be detached, so, we simply agree with any detach command.
 */
static int
acb56_detach(lmi_t * lmi)
{
	acb56_t *p = (acb56_t *) lmi;
	dev_device_t *dev = &p->dev.iface;
	unsigned long flags;

	(void) dev;
	spin_lock_irqsave(&dev->iflock, flags);
	spin_unlock_irqrestore(&dev->iflock, flags);
	return (0);
}

#ifndef abs
#define abs(x) ((x)<0 ? -(x):(x))
#endif

/*
 *  ENABLE: This should set up the hardware for the device using the resources
 *  which were reserved during the open.  The device should be activated and
 *  made ready for operation.  Interrupt service routines should be enabled.
 */
static int
acb56_enable(lmi_t * lmi)
{
	acb56_t *p = (acb56_t *) lmi;
	dev_device_t *dev = &p->dev.iface;
	int i, actrl;
	unsigned long flags;

	spin_lock_irqsave(&dev->iflock, flags);

	actrl = dev->iobase + 1;
	for (i = 0; i < 16; i++)
		p->regs[i] = 0;	/* register images */
	/* 
	   setup chip */
	for (i = 0; i < sizeof(preset);) {
		outb(preset[i], actrl);
		i++;
		outb(p->regs[i >> 1] = preset[i], actrl);
		i++;
	}
	/* 
	   setup interface and clock modes */
	outb(0x0b, actrl);
	outb(p->regs[0x0b] = mode_clock[mode_map[dev->ifmode]][clock_map[dev->ifclock]], actrl);
	/* 
	   setup baud rate generator */
	if (dev->ifmode == DEV_MODE_DTE) {
		outb(0x0c, actrl);
		outb(p->regs[0xc] = 0xca, actrl);
		outb(0x0d, actrl);
		outb(p->regs[0xd] = 0x1c, actrl);
	} else if (dev->ifmode == DEV_MODE_LOC_LB) {
		outb(0x0c, actrl);
		outb(p->regs[0xc] = 0x00, actrl);
		outb(0x0d, actrl);
		outb(p->regs[0xd] = 0x00, actrl);
	} else {
		outb(0x0c, actrl);
		outb(p->regs[0xc] = 0x40, actrl);
		outb(0x0d, actrl);
		outb(p->regs[0xd] = 0x00, actrl);
	}
	/* 
	   special DPLL modes */
	if (dev->ifclock == DEV_CLOCK_DPLL) {
		outb(0x0e, actrl);
		outb(0x60, actrl);
		outb(0x0e, actrl);
		outb(0xe0, actrl);
		if (dev->ifmode == DEV_MODE_DTE) {
			outb(0x0e, actrl);
			outb(0xa0, actrl);
		} else {
			outb(0x0e, actrl);
			outb(0x80, actrl);
		}
		outb(0x0e, actrl);
		outb(0x20, actrl);
	}
	outb(0x0e, actrl);
	outb(p->regs[0x0e] = 0x02, actrl);
	/* 
	   setup loopback and echo modes */
	switch (dev->ifmode) {
	case DEV_MODE_LOC_LB:
		outb(0x0e, actrl);
		outb(p->regs[0x0e] |= 0x10, actrl);
		break;
	case DEV_MODE_ECHO:
		outb(0x0e, actrl);
		outb(p->regs[0x0e] |= 0x08, actrl);
		break;
	case DEV_MODE_LB_ECHO:
		outb(0x0e, actrl);
		outb(p->regs[0x0e] |= 0x18, actrl);
		break;
	}
	/* 
	   set up dma registers */
	if (dev->dma_rx || dev->dma_tx) {
		outb(0x0e, actrl);
		outb(p->regs[0x0e] |= 0x04, actrl);
		if (dev->dma_rx && dev->dma_tx) {
			outb(0x01, actrl);
			outb(p->regs[0x01] |= 0xf9, actrl);
		}
		if (dev->dma_tx) {
			outb(0x01, actrl);
			outb(p->regs[0x01] |= 0xc1, actrl);
		}
		if (dev->dma_tx) {
			outb(0x01, actrl);
			outb(p->regs[0x01] |= 0xfb, actrl);
		}
		outb(0x80, actrl + 3);
		if (dev->dma_rx)
			enable_dma(dev->dma_rx);
		if (dev->dma_tx)
			enable_dma(dev->dma_tx);
	} else {
		outb(0x0e, actrl);
		outb(p->regs[0x0e] &= ~0x04, actrl);
		outb(0x01, actrl);
		outb(p->regs[0x01] |= 0x13, actrl);
		outb(0x00, actrl + 3);
	}
	/* 
	   disable status fifo */
	outb(0x0f, actrl);
	outb(0xfc, actrl);
	outb(0x09, actrl);
	outb(0x02, actrl);
	/* 
	   reset and enable transmitters and receivers */
	outb(0x0E, actrl);
	outb(p->regs[0x0e] |= 0x01, actrl);
	outb(0x03, actrl);
	outb(p->regs[0x0e] |= 0x01, actrl);
	outb(0x00, actrl);
	outb(0x30, actrl);
	outb(0x05, actrl);
	outb(p->regs[0x0e] |= 0x08, actrl);
	outb(0x00, actrl);
	outb(0x80, actrl);
	outb(0x00, actrl);
	outb(0xC0, actrl);
	outb(0x00, actrl);
	outb(0x10, actrl);
	outb(0x00, actrl);
	outb(0x10, actrl);

	dev->ifflags |= DEV_IF_TX_RUNNING;
	dev->ifflags |= DEV_IF_RX_RUNNING;

	bufq_init(&p->tinputq);

	acb56_resupply(NULL);
	p->tx_msg = bufq_dequeue(&acb56_supplyq);
	acb56_resupply(NULL);
	p->rx_msg = bufq_dequeue(&acb56_supplyq);
	acb56_resupply(NULL);
	p->cp_msg = bufq_dequeue(&acb56_supplyq);
	acb56_resupply(NULL);

	if (!p->tx_msg || !p->rx_msg || !p->cp_msg) {
		if (p->tx_msg)
			freemsg(p->tx_msg);
		if (p->rx_msg)
			freemsg(p->rx_msg);
		if (p->cp_msg)
			freemsg(p->cp_msg);
		spin_unlock_irqrestore(&dev->iflock, flags);
		return ENOBUFS;
	}

	*(p->tx_msg->b_wptr)++ = 0x80;	/* Initial SIOS */
	*(p->tx_msg->b_wptr)++ = 0x80;	/* Initial SIOS */
	*(p->tx_msg->b_wptr)++ = 0x01;	/* Initial SIOS */
	*(p->tx_msg->b_wptr)++ = 0x00;	/* Initial SIOS */

	/* 
	   enable master interrupt bit */
	outb(0x09, actrl);
	outb(p->regs[0x09] |= 0x08, actrl);

	/* 
	   we're running! phew! */
	spin_unlock_irqrestore(&dev->iflock, flags);
	return (0);
}

/*
 *  DISABLE: This should shut down the hardware and deactivate ISRs and other
 *  routines, flush buffers to the point that kernel resource which were
 *  allocated can be deallocated in the close.
 */
static int
acb56_disable(lmi_t * lmi)
{
	acb56_t *p = (acb56_t *) lmi;
	dev_device_t *dev = &p->dev.iface;
	int actrl = dev->iobase + 1;
	unsigned long flags;

	spin_lock_irqsave(&dev->iflock, flags);

	dev->ifflags &= ~DEV_IF_TX_RUNNING;
	dev->ifflags &= ~DEV_IF_RX_RUNNING;
	dev->ifflags &= ~DEV_IF_SU_COMPRESS;

	outb(0x09, actrl);
	outb(0xc0, actrl);	/* force hw reset */
	outb(0x09, actrl);
	outb(p->regs[0x09] &= ~0x08, actrl);	/* stop interrupts */
	if (dev->dma_tx) {
		outb(0x0e, actrl);
		outb(p->regs[0x0e] &= ~0x04, actrl);	/* disable dma */
		disable_dma(dev->dma_tx);
	}
	if (dev->dma_rx) {
		outb(0x01, actrl);
		outb(p->regs[0x01] &= ~0xc0, actrl);	/* disable dma */
		disable_dma(dev->dma_rx);
	}
	outb(0x09, actrl);
	outb(0xc0, actrl);	/* force hw reset */

	bufq_purge(&p->tinputq);

	spin_unlock_irqrestore(&dev->iflock, flags);
	return (0);
}

/*
 *  IOCTLS: These are ioctls which are general for SDL and which are specific
 *  to device interfaces and which are privdte for the ACB56 drivers.  They
 *  are invoked at the stream head.
 */
static int
acb56_ioctl(lmi_t * sdl, int cmd, void *arg)
{
	size_t size = _IOC_SIZE(cmd);
	acb56_t *p = (acb56_t *) sdl->device;
	dev_device_t *dev = &p->dev.iface;
	sdl_config_t *ureq = NULL;
	sdl_ulong uarg = 0;

	if (_IOC_TYPE(cmd) == SDL_IOC_MAGIC)
		switch (cmd) {
		case SDL_IOCTCONFIG:
		case SDL_IOCSCONFIG:
			if (!arg || size < sizeof(dev_device_t))
				return EINVAL;
			ureq = arg;
			break;
		case SDL_IOCCCONFIG:
		case SDL_IOCCMRESET:
			break;
		default:
			return EOPNOTSUPP;
		}
	if (_IOC_TYPE(cmd) == DEV_IOC_MAGIC)
		switch (cmd) {
		case DEV_IOCGIFFLAGS:
		case DEV_IOCGIFTYPE:
		case DEV_IOCGGRPTYPE:
		case DEV_IOCGIFMODE:
		case DEV_IOCGIFRATE:
		case DEV_IOCGIFCLOCK:
		case DEV_IOCGIFCODING:
		case DEV_IOCGIFLEADS:
		case DEV_IOCSIFFLAGS:
		case DEV_IOCSIFTYPE:
		case DEV_IOCSGRPTYPE:
		case DEV_IOCSIFMODE:
		case DEV_IOCSIFRATE:
		case DEV_IOCSIFCLOCK:
		case DEV_IOCSIFCODING:
		case DEV_IOCSIFLEADS:
		case DEV_IOCCIFLEADS:
			if (!arg || size < sizeof(dev_ulong))
				return EINVAL;
			uarg = *(dev_ulong *) arg;
			break;
		case DEV_IOCCIFRESET:
		case DEV_IOCCDISCTX:
		case DEV_IOCCCONNTX:
			break;
		default:
			return EOPNOTSUPP;
		}
	if (_IOC_TYPE(cmd) == SDL_IOC_MAGIC)
		switch (cmd) {
		case SDL_IOCTCONFIG:
		case SDL_IOCSCONFIG:
		case SDL_IOCCCONFIG:
		case SDL_IOCCMRESET:
			break;
		default:
			return EOPNOTSUPP;
		}
	if (_IOC_TYPE(cmd) == DEV_IOC_MAGIC)
		switch (cmd) {
		case DEV_IOCCIFRESET:
			break;
			/* 
			   gets */
		case DEV_IOCGIFFLAGS:
			*(dev_ulong *) arg = dev->ifflags;
			return (0);
		case DEV_IOCGIFTYPE:
			*(dev_ulong *) arg = dev->iftype;
			return (0);
		case DEV_IOCGGRPTYPE:
			*(dev_ulong *) arg = dev->ifgtype;
			return (0);
		case DEV_IOCGIFMODE:
			*(dev_ulong *) arg = dev->ifmode;
			return (0);
		case DEV_IOCGIFRATE:
			*(dev_ulong *) arg = dev->ifrate;
			return (0);
		case DEV_IOCGIFCLOCK:
			*(dev_ulong *) arg = dev->ifclock;
			return (0);
		case DEV_IOCGIFCODING:
			*(dev_ulong *) arg = dev->ifcoding;
			return (0);
		case DEV_IOCGIFLEADS:
			*(dev_ulong *) arg = dev->ifleads;
			return (0);
			/* 
			   sets */
		case DEV_IOCSIFFLAGS:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			dev->ifflags = uarg;
			return (0);
		case DEV_IOCSIFTYPE:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			if (uarg != DEV_TYPE_V35)
				return EINVAL;
			return (0);
		case DEV_IOCSGRPTYPE:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			if (uarg != DEV_GTYPE_NONE)
				return EINVAL;
			return (0);
		case DEV_IOCSIFMODE:
			ptrace(("uarg = %lu\n", uarg));
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			switch (uarg) {
			case DEV_MODE_DTE:
			case DEV_MODE_DCE:
			case DEV_MODE_REM_LB:
			case DEV_MODE_LOC_LB:
			case DEV_MODE_LB_ECHO:
			case DEV_MODE_TEST:
				if (mode_map[uarg] != -1) {
					dev->ifmode = uarg;
					return (0);
				}
			default:
				return EINVAL;
			}
			return (0);
		case DEV_IOCSIFRATE:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			if (uarg != 56000)
				return EINVAL;
			return (0);
		case DEV_IOCSIFCLOCK:
			ptrace(("uarg = %lu\n", uarg));
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			switch (uarg) {
			case DEV_CLOCK_INT:
			case DEV_CLOCK_EXT:
			case DEV_CLOCK_DPLL:
				if (clock_map[uarg] != -1) {
					dev->ifclock = uarg;
					return (0);
				}
			default:
				return EINVAL;
			}
			return (0);
		case DEV_IOCSIFCODING:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			if (uarg != DEV_CODING_NRZI)
				return EINVAL;
			return (0);
		case DEV_IOCSIFLEADS:
		case DEV_IOCCIFLEADS:
			/* 
			   FIXME: control the leads */
			if (uarg)
				return EINVAL;
			return (0);
		case DEV_IOCCDISCTX:
			dev->ifflags &= ~DEV_IF_TX_RUNNING;
			return (0);
		case DEV_IOCCCONNTX:
			dev->ifflags |= DEV_IF_TX_RUNNING;
			return (0);
		}
	return EOPNOTSUPP;
}

static struct lmi_ops acb56_lmi_ops = {
	{
	 acb56_devatt,		/* dev.attach */
	 acb56_open,		/* dev.open */
	 acb56_close,		/* dev.close */
	 },
	{
	 acb56_info,		/* lmi.info */
	 acb56_attach,		/* lmi.attach */
	 acb56_detach,		/* lmi.detach */
	 acb56_enable,		/* lmi.enable */
	 acb56_disable,		/* lmi.disable */
	 acb56_ioctl		/* lmi.ioctl */
	 }
};

/*
 *  =========================================================================
 *
 *  SERVICE ROUTINES
 *
 *  =========================================================================
 */

static void
acb56_xmit(struct dev *dev, mblk_t *mp)
{
	acb56_t *p = (acb56_t *) dev;
	unsigned long flags;

	spin_lock_irqsave(&dev->iface.iflock, flags);
	if (!(dev->iface.ifflags & DEV_IF_TX_RUNNING))
		freemsg(mp);
	else
		bufq_queue(&p->tinputq, mp);
	spin_unlock_irqrestore(&dev->iface.iflock, flags);
}

static void
acb56_tx_start(struct dev *dev)
{
	dev->iface.ifflags |= DEV_IF_TX_RUNNING;
}

static void
acb56_rx_start(struct dev *dev)
{
	dev->iface.ifflags |= DEV_IF_RX_RUNNING;
}

static dev_dcalls_t acb56_dcalls = {
	acb56_xmit,			/* daedt_xmit */
	acb56_tx_start,			/* daedt_start */
	acb56_rx_start			/* daedr_start */
};

/*
 *  =======================================================================
 *
 *  LiS Module Initialization   (For registered driver.)
 *
 *  =======================================================================
 */

static int acb56_initialized = 0;

void
acb56_init(void)
{
	if (acb56_initialized > 0)
		return;
	printk(KERN_INFO ACB56_BANNER);	/* console splash */
	ptrace(("registering sdl_acb: cmajor %d, nminors %d\n", ACB56_CMAJOR, ACB56_NMINOR));
	acb56_initialized =
	    sdl_register_driver(ACB56_CMAJOR, ACB56_NMINOR, "sdl_acb", &acb56_lmi_ops,
				&acb56_dcalls);
	ptrace(("return (device major) = %d\n", acb56_initialized));
	if (acb56_initialized > 0)
		acb56_cmajor = acb56_initialized;
	if (acb56_initialized == 0)
		acb56_initialized = acb56_cmajor;
	if (debug != -1)
		acb56_debug = debug;
	bufq_init(&acb56_supplyq);
	bufq_init(&acb56_returnq);
}

void
acb56_terminate(void)
{
	if (acb56_initialized <= 0)
		return;
	ptrace(("unregistering sdl_acb cmajor %d\n", acb56_cmajor));
	acb56_initialized = sdl_unregister_driver(acb56_cmajor);
	ptrace(("return = %d\n", acb56_initialized));
	bufq_purge(&acb56_supplyq);
	bufq_purge(&acb56_returnq);
}

/*
 *  =======================================================================
 *
 *  Kernel Module Initialization   (For unregistered driver.)
 *
 *  =======================================================================
 */

#ifdef MODULE

int
init_module(void)
{
	(void) acb56_debug;
	acb56_init();
	if (acb56_initialized < 0)
		return acb56_initialized;
	return (0);
}

void
cleanup_module(void)
{
	acb56_terminate();
}

#endif
