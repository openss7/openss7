/*****************************************************************************

 @(#) $RCSfile: sdt_acb56.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/05/27 02:18:35 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/05/27 02:18:35 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sdt_acb56.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/05/27 02:18:35 $"

static char const ident[] = "$RCSfile: sdt_acb56.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/05/27 02:18:35 $";

/*
 *  This is an implementation of the Signalling Data Terminal for the SeaLevel
 *  ACB56[tm] V.35 ISA card.  It provides the driver routines necessary for
 *  interface to the SDT driver kernel module.
 */

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <sys/stream.h>
#include <sys/cmn_err.h>

#include <linux/errno.h>
#include <linux/types.h>

#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/dma.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

#include "debug.h"		/* generic debugging macros */
#include "bufq.h"		/* generic buffer queues */
#include "priv.h"		/* generic data structures */
#include "lock.h"		/* generic queue locking functions */
#include "queue.h"		/* generic put and srv routines */
#include "allocb.h"		/* generic buffer allocation routines */

#define ACB56_DESCRIP	"ACB56: SS7/SDT (Signalling Data Terminal) STREAMS DRIVER."
#define ACB56_REVISION	"LfS $RCSfile: sdt_acb56.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/05/27 02:18:35 $"
#define ACB56_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corpoation.  All Rights Reserved."
#define ACB56_DEVICES	"Supports the SeaLevel ACB56(tm) V.35 boards."
#define ACB56_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ACB56_LICENSE	"GPL"
#define ACB56_BANNER	ACB56_DESCRIP	"\n" \
			ACB56_REVISION	"\n" \
			ACB56_COPYRIGHT	"\n" \
			ACB56_DEVICES	"\n" \
			ACB56_CONTACT	"\n"

MODULE_AUTHOR(ACB56_CONTACT);
MODULE_DESCRIPTION(ACB56_DESCRIP);
MODULE_SUPPORTED_DEVICE(ACB56_DEVICES);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ACB56_LICENSE);
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */
#define SDT_MOD_NAME	"sdt-acb56"
#define SDT_MOD_ID	SDT_IOC_MAGIC

STATIC struct module_info sdt_rinfo = {
	mi_idnum:SDT_MOD_ID,		/* Module ID number */
	mi_idname:SDT_MOD_NAME "-rd",	/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:9 * 279,		/* Hi water mark */
	mi_lowat:3 * 279,		/* Lo water mark */
};
STATIC struct module_info sdt_winfo = {
	mi_idnum:SDT_MOD_ID,		/* Module ID number */
	mi_idname:SDT_MOD_NAME "-wr",	/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

STATIC int sdt_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sdt_close(queue_t *, int, cred_t *);

STATIC int sdt_rput(queue_t *, mblk_t *);
STATIC int sdt_rsrv(queue_t *);
STATIC int sdt_wput(queue_t *, mblk_t *);
STATIC int sdt_wsrv(queue_t *);

STATIC struct qinit sdt_rinit = {
	qi_putp:sdt_rput,		/* Read put (msg from below) */
	qi_srvp:sdt_rsrv,		/* Read queue service */
	qi_open:sdt_open,		/* Each open */
	qi_qclose:sdt_close,		/* Last close */
	qi_minfo:&sdt_rinfo,		/* Information */
};
STATIC struct qinit sdt_winit = {
	qi_putp:sdt_wput,		/* Write put (msg from above) */
	qi_srvp:sdt_wsrv,		/* Write queue service */
	qi_minfo:&sdt_winfo,		/* Information */
};

STATIC struct streamtab sdt_info = {
	st_rdinit:&sdt_rinit,		/* Upper read queue */
	st_wrinit:&sdt_winit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  ACB56-SDT Private Structure
 *
 *  =========================================================================
 */
typedef struct sdt {
	STR_DECLARATION (struct sdt);	/* stream structure declaration */
	mblk_t *rbuf;			/* normal data reassembly */
	mblk_t *xbuf;			/* expedited data reassembly */
	lmi_option_t option;		/* LMI options */
	struct {
		bufq_t tb;		/* transmission buffer */
		mblk_t *tx_cmp;		/* tx compression buffer */
		mblk_t *rx_cmp;		/* rx compression buffer */
		uint tx_repeat;		/* tx compression count */
		uint rx_repeat;		/* rx compression count */
		struct {
			ulong t8;	/* SDT T8 timer */
		} timer;
		sdt_config_t config;	/* SDT configuration */
		sdt_statem_t statem;	/* SDT state machine */
		sdt_notify_t notify;	/* SDT notification options */
		sdt_stats_t stats;	/* SDT statistics */
		sdt_stats_t stamp;	/* SDT statistics timestamps */
		lmi_sta_t statsp;	/* SDT statistics periods */
	} sdt;
	struct {
		long tickbytes;		/* traffic bytes per tick */
		long bytecount;		/* traffic bytes this tick */
		long timestamp;		/* traffic last tick */
		struct {
			ulong t9;
		} timer;
		sdl_config_t config;	/* SDL configuration */
		sdl_statem_t statem;	/* SDL state machine variables */
		sdl_notify_t notify;	/* SDL notification options */
		sdl_stats_t stats;	/* SDL statistics */
		sdl_stats_t stamp;	/* SDL statistics timestamps */
		lmi_sta_t statsp;	/* SDL statistics periods */
	} sdl;
	struct {
		int ifindex;		/* interface index */
		int irq;		/* interrupt */
		int iobase;		/* io base */
		int dma_rx;		/* rx dma channel */
		int dma_tx;		/* tx dma channel */
		int rx_octet_mode;	/* Is octet counting on? */
		int rx_octet_count;	/* bits (not octets) counted */
		mblk_t *tx_msg;		/* transmit buffer */
		mblk_t *rx_msg;		/* receive buffer */
		mblk_t *cp_msg;		/* compressed su buffer */
		unsigned char *tx_buf;	/* current tx buffer pointer */
		unsigned char *tx_max;	/* end of tx buffer pointer */
		int tx_error;		/* transmission error */
		unsigned char *rx_buf;	/* current rx buffer pointer */
		unsigned char *rx_max;	/* end oft rx buffer pointer */
		int rx_error;		/* reception error */
		unsigned char regs[16];	/* register images */
		unsigned char rr0;	/* register image reg 0 */
		bufq_t tinputq;
	} dev;
} sdt_t;

#define SDT_PRIV(__q) ((sdt_t *)(__q)->q_ptr)

/*
 *  ========================================================================
 *
 *  PRIMITIVES
 *
 *  ========================================================================
 */
/*
 *  ------------------------------------------------------------------------
 *
 *  Primitive sent upstream
 *
 *  ------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, int err)
{
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sdt->t.state = TS_NOSTATES;
		sdt->state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_ERROR\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  M_HANGUP
 *  -----------------------------------
 */
STATIC int
m_hangup(queue_t *q, int err)
{
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sdt->t.state = TS_NOSTATES;
		sdt->state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_HANGUP\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sdt->state;
		p->lmi_max_sdu =
		    sdt->sdt.config.m + 1 + ((sdt->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((sdt->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_ok_ack(queue_t *q, long prim)
{
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sdt->state) {
		case LMI_ATTACH_PENDING:
			sdt->state = LMI_DISABLED;
			break;
		case LMI_DETACH_PENDING:
			sdt->state = LMI_UNATTACHED;
			break;
		default:
			break;
		}
		p->lmi_state = sdt->state;
		printd(("%s: %p: <- LMI_OK_ACK\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_error_ack(queue_t *q, long prim, ulong reason, ulong errno)
{
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	lmi_error_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_error_primitive = prim;
		switch (sdt->state) {
		case LMI_ATTACH_PENDING:
			sdt->state = LMI_UNATTACHED;
			break;
		case LMI_DETACH_PENDING:
			sdt->state = LMI_DISABLED;
			break;
		case LMI_ENABLE_PENDING:
			sdt->state = LMI_DISABLED;
			break;
		case LMI_DISABLE_PENDING:
			sdt->state = LMI_ENABLED;
			break;
		default:
			break;
		}
		p->lmi_state = sdt->state;
		printd(("%s: %p: <- LMI_ERROR_ACK\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ENABLE_CON;
		assure(sdt->state == LMI_ENABLE_PENDING);
		sdt->state = LMI_ENABLED;
		p->lmi_state = sdt->state;
		printd(("%s: %p: <- LMI_ENABLE_CON\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DISABLE_CON;
		assure(sdt->state == LMI_DISABLE_PENDING);
		sdt->state = LMI_DISABLED;
		p->lmi_state = sdt->state;
		printd(("%s: %p: <- LMI_DISABLE_CON\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = sdt->state;
		printd(("%s: %p: <- LMI_ERROR_IND\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
	sdt_t *sdt = SDT_PRIV(q);
	if (canputnext(sdt->oq)) {
		mblk_t *mp;
		lmi_stats_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = jiffies;
			printd(("%s: %p: <- LMI_STATS_IND\n", SDT_MOD_NAME, sdt));
			putnext(sdt->oq, mp);
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
	sdt_t *sdt = SDT_PRIV(q);
	if (canputnext(sdt->oq)) {
		mblk_t *mp;
		lmi_event_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = jiffies;
			p->lmi_severity = level;
			printd(("%s: %p: <- LMI_EVENT_IND\n", SDT_MOD_NAME, sdt));
			putnext(sdt->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  SDT_RC_SIGNAL_UNIT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_signal_unit_ind(queue_t *q, sdt_t * sdt, mblk_t *dp, ulong count)
{
	if (count) {
		if (canputnext(sdt->oq)) {
			mblk_t *mp;
			sdt_rc_signal_unit_ind_t *p;
			if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = ((typeof(p)) mp->b_wptr)++;
				p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
				p->sdt_count = count;
				mp->b_cont = dp;
				// printd(("%s: %p: <- SDT_RC_SIGNAL_UNIT_IND\n", SDT_MOD_NAME,
				// sdt));
				putnext(sdt->oq, mp);
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

/*
 *  SDT_RC_CONGESTION_ACCEPT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_accept_ind(queue_t *q, sdt_t * sdt)
{
	mblk_t *mp;
	sdt_rc_congestion_accept_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_ACCEPT_IND\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
sdt_rc_congestion_discard_ind(queue_t *q, sdt_t * sdt)
{
	mblk_t *mp;
	sdt_rc_congestion_discard_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_DISCARD_IND\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
sdt_rc_no_congestion_ind(queue_t *q, sdt_t * sdt)
{
	mblk_t *mp;
	sdt_rc_no_congestion_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
		printd(("%s: %p: <- SDT_RC_NO_CONGESTION_IND\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
sdt_iac_correct_su_ind(queue_t *q, sdt_t * sdt)
{
	if (canputnext(sdt->oq)) {
		mblk_t *mp;
		sdt_iac_correct_su_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
			printd(("%s: %p: <- SDT_IAC_CORRECT_SU_IND\n", SDT_MOD_NAME, sdt));
			putnext(sdt->oq, mp);
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
sdt_iac_abort_proving_ind(queue_t *q, sdt_t * sdt)
{
	mblk_t *mp;
	sdt_iac_abort_proving_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
		printd(("%s: %p: <- SDT_IAC_ABORT_PROVING_IND\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
sdt_lsc_link_failure_ind(queue_t *q, sdt_t * sdt)
{
	mblk_t *mp;
	sdt_lsc_link_failure_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
		printd(("%s: %p: <- SDT_LSC_LINK_FAILURE_IND\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
sdt_txc_transmission_request_ind(queue_t *q, sdt_t * sdt)
{
	mblk_t *mp;
	sdt_txc_transmission_request_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		printd(("%s: %p: <- SDT_TXC_TRANSMISSION_REQUEST_IND\n", SDT_MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
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
 *  Timers
 *
 *  ------------------------------------------------------------------------
 */
enum { tall, t8, t9 };

#define SDT_DECLARE_TIMER(__o,__t) \
STATIC int __o ## _ ## __t ## _timeout(__o ## _t *); \
STATIC void __o ## _ ## __t ## _expiry(caddr_t data) \
{ \
	__o ## _do_timeout(data, # __t, &((_o ## _t *)data)->timers. __t, &__o ## _ ## __t ## _timeout, & __o ## _t ## _expiry); \
} \
STATIC void __o ## _stop_timer_ ## __t (__o ## _t *__o) \
{ \
	__o ## _stop_timer(__o, # __t, &__o->timers. __t); \
} \
STATIC void __o ## _start_timer_ ## __t (__o ## _t *__o) \
{ \
	__o ## _start_timer(__o, # __t, &__o->timers. __t, &__o ## _ ## __t ## _expiry, __o->config. __t); \
} \

STATIC INLINE void
sdt_do_timeout(caddr_t data, const char *timer, ulong *timeo, int (*to_fnc) (ct_t *),
	       void (*exp_fnc) (caddr_t))
{
	sdt_t *sdt = (sdt_t *) data;
	if (xchg(timeo, 0)) {
		if (lis_spin_trylock(&sdt->lock)) {
			printd(("%s: %p: %s timeout at %lu\n", SDT_MOD_NAME, sdt, timer, jiffies));
			switch (to_fnc(sdt)) {
			default:
			case QR_DONE:
				sdt_put(sdt);
				lis_spin_unlock(&sdt->lock);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			lis_spin_unlock(&sdt->lock);
		} else
			printd(("%s: %p: %s timeout collision oat %lu\n", SDT_MOD_NAME, sdt, timer,
				jiffies));
		/*
		   back off timer one tick 
		 */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
sdt_stop_timer(sdt_t * sdt, const char *timer, ulong *timeo)
{
	ulong to;
	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", SDT_MOD_NAME, sdt, timer, jiffies));
		sdt_put(sdt);
	}
	return;
}
STATIC INLINE void
sdt_start_timer(sdt_t * sdt, const char *timer, ulong *timeo, void (*exp_func) (caddr_t), ulong val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", SDT_MOD_NAME, sdt, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_func, (caddr_t) sdt, val);
}

SDT_DECLARE_TIMER(sdt, t8);
SDT_DECLARE_TIMER(sdt, t9);

STATIC INLINE void
__sdt_timer_stop(sdt_t * sdt, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/*
		   fall through 
		 */
	case t8:
		sdt_stop_timer_t8(sdt);
		if (single)
			break;
		/*
		   fall through 
		 */
	case t9:
		sdt_stop_timer_t9(sdt);
		if (single)
			break;
		/*
		   fall through 
		 */
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sdt_timer_stop(sdt_t * sdt, const uint t)
{
	psw_t flags;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		__sdt_timer_stop(sdt, t);
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
}
STATIC INLINE void
sdt_timer_start(sdt_t * sdt, const uint t)
{
	psw_t flags;
	lis_spin_lock_irqsave(&sdt->lock, flags);
	{
		__sdt_timer_stop(sdt, t);
		switch (t) {
		case t8:
			sdt_start_timer_t8(sdt);
			break;
		case t9:
			sdt_start_timer_t9(sdt);
			break;
		default:
			swerr();
			break;
		}
	}
	lis_spin_unlock_irqrestore(&sdt->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDT State Machines
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sdt_aerm_stop(queue_t *q, sdt_t * sdt)
{
	sdt->sdt.statem.aerm_state = SDT_STATE_IDLE;
	sdt->sdt.statem.Ti = sdt->sdt.config.Tin;
}

STATIC INLINE void
sdt_eim_stop(queue_t *q, sdt_t * sdt)
{
	sdt->sdt.statem.eim_state = SDT_STATE_IDLE;
	sdt_timer_stop(q, t8);
}

STATIC INLINE void
sdt_suerm_stop(queue_t *q, sdt_t * sdt)
{
	sdt_eim_stop(q, sdt);
	sdt->sdt.statem.suerm_state = SDT_STATE_IDLE;
}

STATIC INLINE int
sdt_lsc_link_failure(queue_t *q, sdt_t * sdt)
{
	return sdt_lsc_link_failure_ind(q, sdt);
}

STATIC INLINE void
sdt_daedr_start(queue_t *q, sdt_t * sdt)
{
	sdt->sdt.statem.daedr_state = SDT_STATE_IN_SERVICE;
	sdt->sdl.statem.rx_state = SDL_STATE_IN_SERVICE;
	sdt->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_RX_RUNNING);
}

STATIC INLINE void
sdt_eim_start(queue_t *q, sdt_t * sdt)
{
	sdt->sdt.statem.Ce = 0;
	sdt->sdt.statem.interval_error = 0;
	sdt->sdt.statem.su_received = 0;
	sdt_timer_start(q, t8);
	sdt->sdt.statem.eim_state = SDT_STATE_MONITORING;
}

STATIC INLINE void
sdt_suerm_start(queue_t *q, sdt_t * sdt)
{
	if (sdt->option.popt & SS7_POPT_HSL)
		sdt_eim_start(q, sdt);
	else {
		sdt->sdt.statem.Cs = 0;
		sdt->sdt.statem.Ns = 0;
		sdt->sdt.statem.suerm_state = SDT_STATE_IN_SERVICE;
	}
}

STATIC INLINE void
sdt_aerm_set_ti_to_tie(queue_t *q, sdt_t * sdt)
{
	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE)
		sdt->sdt.statem.Ti = sdt->sdt.config.Tie;
}

STATIC INLINE void
sdt_aerm_start(queue_t *q, sdt_t * sdt)
{
	sdt->sdt.statem.Ca = 0;
	sdt->sdt.statem.aborted_proving = 0;
	sdt->sdt.statem.aerm_state = SDT_STATE_MONITORING;
}

STATIC INLINE void
sdt_iac_correct_su(queue_t *q, sdt_t * sdt)
{
	sdt_iac_correct_su_ind(q, sdt);
}

STATIC INLINE void
sdt_iac_abort_proving(queue_t *q, sdt_t * sdt)
{
	sdt_iac_abort_proving_ind(q, sdt);
}

STATIC INLINE void
sdt_daedt_start(queue_t *q, sdt_t * sdt)
{
	sdt_t *sdt = SDT_PRIV(q);
	sdt->sdt.statem.daedt_state = SDT_STATE_IN_SERVICE;
	sdt->sdl.statem.tx_state = SDL_STATE_IN_SERVICE;
	sdt->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING);
}

STATIC INLINE void
sdt_aerm_set_ti_to_tin(queue_t *q, sdt_t * sdt)
{
	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE)
		sdt->sdt.statem.Ti = sdt->sdt.config.Tin;
}

STATIC INLINE void
sdt_aerm_su_in_error(queue_t *q, sdt_t * sdt)
{
	if (sdt->sdt.statem.aerm_state == SDT_STATE_MONITORING) {
		sdt->sdt.statem.Ca++;
		if (sdt->sdt.statem.Ca == sdt->sdt.statem.Ti) {
			sdt->sdt.statem.aborted_proving = 1;
			sdt_iac_abort_proving(q, sdt);
			sdt->sdt.statem.Ca--;
			sdt->sdt.statem.aerm_state = SDT_STATE_IDLE;
		}
	}
}

STATIC INLINE void
sdt_aerm_correct_su(queue_t *q, sdt_t * sdt)
{
	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE) {
		if (sdt->sdt.statem.aborted_proving) {
			sdt_iac_correct_su(q, sdt);
			sdt->sdt.statem.aborted_proving = 0;
		}
	}
}

STATIC INLINE void
sdt_suerm_su_in_error(queue_t *q, sdt_t * sdt)
{
	if (sdt->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		sdt->sdt.statem.Cs++;
		if (sdt->sdt.statem.Cs >= sdt->sdt.config.T) {
			sdt_lsc_link_failure(q, sdt);
			sdt->sdt.statem.Ca--;
			sdt->sdt.statem.suerm_state = SDT_STATE_IDLE;
			return;
		}
		sdt->sdt.statem.Ns++;
		if (sdt->sdt.statem.Ns >= sdt->sdt.config.D) {
			sdt->sdt.statem.Ns = 0;
			if (sdt->sdt.statem.Cs)
				sdt->sdt.statem.Cs--;
		}
	}
}

STATIC INLINE void
sdt_eim_su_in_error(queue_t *q, sdt_t * sdt)
{
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING)
		sdt->sdt.statem.interval_error = 1;
}

STATIC INLINE void
sdt_suerm_correct_su(queue_t *q, sdt_t * sdt)
{
	if (sdt->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		sdt->sdt.statem.Ns++;
		if (sdt->sdt.statem.Ns >= sdt->sdt.config.D) {
			sdt->sdt.statem.Ns = 0;
			if (sdt->sdt.statem.Cs)
				sdt->sdt.statem.Cs--;
		}
	}
}

STATIC INLINE void
sdt_eim_correct_su(queue_t *q, sdt_t * sdt)
{
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING)
		sdt->sdt.statem.su_received = 1;
}

STATIC INLINE void
sdt_daedr_correct_su(queue_t *q, sdt_t * sdt)
{
	sdt_eim_correct_su(q, sdt);
	sdt_suerm_correct_su(q, sdt);
	sdt_aerm_correct_su(q, sdt);
}

/*
 *  Hooks to underlying driver
 *  -----------------------------------
 */
STATIC INLINE void
sdt_daedr_su_in_error(queue_t *q, sdt_t * sdt)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		/*
		   cancel compression 
		 */
		if (sdt->sdt.rx_cmp) {
			printd(("SU in error\n"));
			freemsg(xchg(&sdt->sdt.rx_cmp, NULL));
		}
		sdt_eim_su_in_error(q, sdt);
		sdt_suerm_su_in_error(q, sdt);
		sdt_aerm_su_in_error(q, sdt);
		sdt->sdt.stats.rx_sus_in_error++;
	}
}

STATIC INLINE void
sdt_daedr_received_bits(sdt_t * sdt, mblk_t *mp)
{
	if (sdt->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		int i, len, mlen = (sdt->option.popt & SS7_POPT_XSN) ? 8 : 5, min_len =
		    mlen - 2, max_len = min_len + sdt->sdt.config.m + 1;
		printd(("%s: %p: SDT_DAEDR_RECEIVED_BITS [%d]<-\n", SDT_MOD_NAME, sdt,
			msgdsize(mp)));
		if (mp) {
			int lmax, li, sif;
			len = msgdsize(mp);
			if (len > max_len || min_len > len) {
				if (len > max_len)
					sdt->sdt.stats.rx_frame_too_long++;
				if (len < min_len)
					sdt->sdt.stats.rx_frame_too_short++;
				goto error_su;
			}
			if (sdt->option.popt & SS7_POPT_XSN) {
				lmax = 0x1ff;
				li = (mp->b_rptr[4] | (mp->b_rptr[5] << 8));
			} else {
				lmax = 0x3f;
				li = mp->b_rptr[2];
			}
			sif = len - min_len;
			if ((li &= lmax) == sif || (li == lmax && sif > lmax))
				goto good_su;
			sdt->sdt.stats.rx_length_error++;
		      error_su:
			sdt_daedr_su_in_error(NULL, sdt);
			freemsg(mp);
			if (sdt->sdt.rx_cmp)
				freemsg(xchg(&sdt->sdt.rx_cmp, NULL));
			return;
		      good_su:
			if (sdt->sdt.rx_cmp) {
				if (len > mlen || len < min_len || len != msgdsize(sdt->sdt.rx_cmp))
					goto no_match;
				for (i = 0; i < len; i++)
					if (mp->b_rptr[i] != sdt->sdt.rx_cmp->b_rptr[i])
						goto no_match;
				sdt->sdt.rx_repeat++;
				sdt->sdt.stats.rx_sus_compressed++;
				freemsg(mp);
				return;
			      no_match:
				if (sdt->sdt.rx_repeat) {
#if 0
					mblk_t *cp;
					if ((cp = dupb(sdt->sdt.rx_cmp))) {
						if (sdt_rc_signal_unit_ind
						    (NULL, sdt, cp, sdt->sdt.rx_repeat)
						    != QR_ABSORBED) {
							sdt->sdt.stats.rx_buffer_overflows++;
							sdt->sdl.stats.rx_buffer_overflows++;
							freemsg(cp);
						}
					}
#endif
					sdt_daedr_correct_su(NULL, sdt);
					sdt->sdt.rx_repeat = 0;
				}
			}
			if (len <= mlen) {
				if (sdt->sdt.rx_cmp || (sdt->sdt.rx_cmp = allocb(mlen, BPRI_HI))) {
					bcopy(mp->b_rptr, sdt->sdt.rx_cmp->b_rptr, len);
					sdt->sdt.rx_cmp->b_wptr = sdt->sdt.rx_cmp->b_rptr + len;
					sdt->sdt.rx_repeat = 0;
				}
			}
			if (sdt_rc_signal_unit_ind(NULL, sdt, mp, 1) != QR_ABSORBED) {
				sdt->sdt.stats.rx_buffer_overflows++;
				sdt->sdl.stats.rx_buffer_overflows++;
				freemsg(mp);
			}
			sdt_daedr_correct_su(NULL, sdt);
		} else
			swerr();
		return;
	}
}

STATIC INLINE mblk_t *
sdt_daedt_transmission_request(queue_t *q)
{
	sdt_t *sdt = SDT_PRIV(q);
	mblk_t *mp = NULL;
	if (sdt->sdt.statem.daedt_state != SDT_STATE_IDLE) {
		if ((mp = bufq_dequeue(&sdt->sdt.tb))) {
			int len = msgdsize(mp);
			int hlen = (sdt->option.popt & SS7_POPT_XSN) ? 6 : 3;
			int mlen = hlen + 2;
			if (!sdt->sdt.tb.q_count)
				qenable(sdt->iq);	/* back-enable */
			if (mlen < hlen)
				goto dont_repeat;
			if (mlen == hlen + 1 || mlen == hlen + 2) {
				int li, sio;
				if (sdt->option.popt & SS7_POPT_XSN) {
					li = ((mp->b_rptr[5] << 8) | mp->b_rptr[4]) & 0x1ff;
					sio = mp->b_rptr[6];
				} else {
					li = mp->b_rptr[2] & 0x3f;
					sio = mp->b_rptr[3];
				}
				if (li != mlen - hlen)
					goto dont_repeat;
				switch (sio) {
				case 0x0:	/* SIO */
				case 0x1:	/* SIN */
				case 0x2:	/* SIE */
				case 0x3:	/* SIOS */
				case 0x4:	/* SIPO */
					break;
				case 0x5:	/* SIB */
				default:
					goto dont_repeat;
				}
			}
			if (len <= mlen
			    && (sdt->sdt.tx_cmp || (sdt->sdt.tx_cmp = allocb(mlen, BPRI_HI)))) {
				mblk_t *cp = sdt->sdt.tx_cmp;
				cp->b_rptr = cp->b_datap->db_base;
				bcopy(mp->b_rptr, cp->b_rptr, len);
				cp->b_wptr = cp->b_rptr + len;
				/*
				   always correct length indicator 
				 */
				if (sdt->option.popt && SS7_POPT_XSN) {
					cp->b_rptr[4] &= 0x00;
					cp->b_rptr[5] &= 0xfe;
					cp->b_rptr[4] += (len - hlen);
				} else {
					cp->b_rptr[2] &= 0xc0;
					cp->b_rptr[2] += (len - hlen);
				}
				sdt->sdt.tx_repeat = 0;
				goto done;
			}
		      dont_repeat:
			if (sdt->sdt.tx_cmp) {
				freemsg(xchg(&sdt->sdt.tx_cmp, NULL));
				sdt->sdt.tx_repeat = 0;
			}
		} else if ((sdt->sdl.config.ifclock != SDL_CLOCK_NONE)
			   && sdt->sdt.tx_cmp && (mp = dupmsg(sdt->sdt.tx_cmp))) {
			sdt->sdt.stats.tx_sus_repeated++;
			sdt->sdt.tx_repeat++;
		}
	}
      done:
	if (!mp) {
		sdt->sdt.stats.tx_buffer_overflows++;
		sdt->sdl.stats.tx_buffer_overflows++;
	}
	return (mp);
}

/*
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */
/*
 *  RX WAKEUP
 *  -----------------------------------
 */
STATIC int
sdt_rx_wakeup(queue_t *q)
{
	(void) q;
	return (0);
}

/*
 *  TX WAKEUP
 *  -----------------------------------
 */
STATIC int
sdt_tx_wakeup(queue_t *q)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->sdt.statem.daedt_state != SDT_STATE_IDLE) {
		mblk_t *mp, *dp;
		long tdiff;
		int size;
		while (canputnext(sdt->iq)) {
			switch (sdt->sdl.config.ifclock) {
			case SDL_CLOCK_TICK:
			case SDL_CLOCK_SHAPER:
				if ((tdiff = jiffies - sdt->sdl.timestamp) < 0) {
					/*
					   throttle back for a t9 
					 */
					printd(("%s: %p: %s throttling back a tick\n", SDT_MOD_NAME,
						sdt, __FUNCTION__));
					if (!sdt->sdl.timer.t9)
						sdt_timer_start(q, t9);
					return (0);
				}
				if (tdiff > 0) {
					sdt->sdl.bytecount = 0;
					sdt->sdl.timestamp = jiffies;
				}
			}
			if (sdt->t.serv_type == T_CLTS) {
				if (!(mp = t_unitdata_req(q)))
					goto overflow;
			} else {
				if (!(mp = t_data_req(q)))
					goto overflow;
			}
			if (!(dp = sdt_daedt_transmission_request(q)))
				goto done;
			size = msgdsize(dp);
			linkb(mp, dp);
			printd(("%s: %p: T_UNITDATA_REQ [%d] ->\n", SDT_MOD_NAME, sdt, size));
			putnext(sdt->iq, mp);
			sdt->sdt.stats.tx_bytes += size;
			sdt->sdt.stats.tx_sus++;
			switch (sdt->sdl.config.ifclock) {
			case SDL_CLOCK_TICK:
			case SDL_CLOCK_SHAPER:
				sdt->sdl.bytecount += size;
				ensure(sdt->sdl.tickbytes > 0, sdt->sdl.tickbytes = 1);
				while (sdt->sdl.bytecount >= sdt->sdl.tickbytes) {
					sdt->sdl.bytecount -= sdt->sdl.tickbytes;
					sdt->sdl.timestamp++;
				}
			}
			switch (sdt->sdl.config.ifclock) {
			case SDL_CLOCK_TICK:
				if (!(sdt->option.popt & SS7_POPT_PCR) && sdt->sdt.tx_cmp
				    && !sdt->sdt.tb.q_count) {
					if (!sdt->sdl.timer.t9)
						sdt_timer_start(q, t9);
					printd(("%s: %p: %s sleeping for a tick\n", SDT_MOD_NAME,
						sdt, __FUNCTION__));
					return (0);
				}
			case SDL_CLOCK_SHAPER:
				continue;
			}
			return (0);
		}
		return (0);
	      done:
		freemsg(mp);
		return (0);
	      overflow:
		printd(("%s: %p: %s buffer overflow\n", SDT_MOD_NAME, sdt, __FUNCTION__));
		sdt->sdt.stats.tx_buffer_overflows++;
		sdt->sdl.stats.tx_buffer_overflows++;
		return (0);
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Events
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T8 EXPIRY
 *  -----------------------------------
 */
STATIC int
sdt_t8_timeout(sdt_t * sdt)
{
	sdt_t *sdt = SDT_PRIV(q);
	int err;
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING) {
		sdt_timer_start(sdt, t8);
		if (sdt->sdt.statem.su_received) {
			sdt->sdt.statem.su_received = 0;
			if (!sdt->sdt.statem.interval_error) {
				if ((sdt->sdt.statem.Ce -= sdt->sdt.config.De) < 0)
					sdt->sdt.statem.Ce = 0;
				return (QR_DONE);
			}
		}
		sdt->sdt.statem.Ce += sdt->sdt.config.Ue;
		if (sdt->sdt.statem.Ce > sdt->sdt.config.Te) {
			if ((err = sdt_lsc_link_failure(NULL, sdt))) {
				sdt->sdt.statem.Ce -= sdt->sdt.config.Ue;
				return (err);
			}
			sdt->sdt.statem.eim_state = SDT_STATE_IDLE;
		}
		sdt->sdt.statem.interval_error = 0;
	}
	return (QR_DONE);
}

/*
 *  T9 EXPIRY
 *  -----------------------------------
 */
STATIC int
sdt_t9_timeout(queue_t *q)
{
	return sdt_tx_wakeup(SDT_PRIV(q)->iq);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LM User -> LM Provider (SS7IP) Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	return lmi_info_ack(q, NULL, 0);
      emsgsize:
	return lmi_error_ack(q, LMI_INFO_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 *  Attachment requires a local address as a PPA (Physical Point of Attachment).  The local
 *  address is necessary for T_BIND on both connection oriented and connectionless transports.
 *  For style 2 transports, we copy the PPA and bind the transport with T_BIND.  Style 1
 *  transports are already bound and this primitive is invalid for style 1 transports.
 */
STATIC int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sdt->state == LMI_UNUSABLE)
		goto eagain;
	if (sdt->style != LMI_STYLE2)
		goto eopnotsupp;
	if (sdt->state != LMI_UNATTACHED)
		goto outstate;
	sdt->state = LMI_ATTACH_PENDING;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sdt->t.add_size)
		goto badppa;
	bcopy(p->lmi_ppa, &sdt->t.loc, sdt->t.add_size);
	/*
	   start bind in motion 
	 */
	return t_bind_req(q);
      badppa:
	ptrace(("%s: PROTO: bad ppa (too short)\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_BADPPA, EMSGSIZE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: PROTO: primitive not support for style\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	/*
	   wait for stream to become usable 
	 */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 *  Detaching requires that the stream be detached from the local address.  This results in a
 *  T_UNBIND connection oriented and connectionless transports.  For style 2 transports, we 
 *  perform the unbinding operation.  For style 1 transports, we were already bound and this
 *  primitive is invalid for style 1 transports.
 */
STATIC int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sdt->state == LMI_UNUSABLE)
		goto eagain;
	if (sdt->style != LMI_STYLE2)
		goto eopnotsupp;
	if (sdt->state != LMI_DISABLED)
		goto outstate;
	sdt->state = LMI_DETACH_PENDING;
	bzero(&sdt->t.loc, sdt->t.add_size);
	/*
	   start unbind in motion 
	 */
	return t_unbind_req(q);
      outstate:
	ptrace(("%s: PROTO: out of state\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_DETACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: PROTO: primitive not support for style\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	/*
	   wait for stream to become usable 
	 */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_DETACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 *  For style 1 connection oriented transports, we are completely connected and can simply
 *  acknowledge the enable.  For style 1 connectionless transports we need to know the remote
 *  address of the peer and require a remote address as part of the enable.  For style 2
 *  connection oriented transports we need to know the remote address for connection or if no
 *  remote address is provided, simply stay listening.  (Note: style 2 connection oriented
 *  transports are quite insecure for this reason.  Style 1 is preferrable for connection
 *  oriented transports for a number of reasons).
 */
STATIC int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sdt->state == LMI_UNUSABLE)
		goto eagain;
	if (sdt->state != LMI_DISABLED)
		goto outstate;
	sdt->state = LMI_ENABLE_PENDING;
	switch (sdt->t.serv_type) {
	case T_CLTS:
		assure(sdt->t.state == TS_IDLE);
		if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sdt->t.add_size)
			goto badppa;
		bcopy(p->lmi_rem, &sdt->t.rem, sdt->t.add_size);
		return lmi_enable_con(q);
	case T_COTS:
	case T_COTS_ORD:
		switch (sdt->style) {
		case LMI_STYLE1:
			assure(sdt->t.state == TS_DATA_XFER);
			return lmi_enable_con(q);
		case LMI_STYLE2:
			assure(sdt->t.state == TS_IDLE);
			if (mp->b_wptr == mp->b_rptr + sizeof(*p))	/* wait for T_CONN_IND */
				return (QR_DONE);
			if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sdt->t.add_size)
				goto badppa;
			bcopy(p->lmi_rem, &sdt->t.rem, sdt->t.add_size);
			/*
			   start connection in motion 
			 */
			return t_conn_req(q);
		}
	}
      badppa:
	ptrace(("%s: PROTO: bad ppa (too short)\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_ATTACH_REQ, LMI_BADPPA, EMSGSIZE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_ENABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	/*
	   wait for stream to become usable 
	 */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_ENABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 *  For style 1 connection oriented transports, we stay completely connected and can simply
 *  acknowledge the disable.  For style 1 connectionless transports we may also simply
 *  acknowledge the disable.  Fort style 2 connection oriented transports we need to
 *  disconnect the connection.
 */
STATIC int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sdt->state == LMI_UNUSABLE)
		goto eagain;
	if (sdt->state != LMI_ENABLED)
		goto outstate;
	sdt->state = LMI_DISABLE_PENDING;
	if (sdt->style == LMI_STYLE1 || sdt->t.serv_type == T_CLTS)
		return lmi_disable_con(q);
	/*
	   start disconnect in motion 
	 */
	if (sdt->t.serv_type == T_COTS_ORD)
		return t_ordrel_req(q);
	return t_discon_req(q, 0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_DISABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	/*
	   wait for stream to become available 
	 */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_DISABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto eopnotsupp;
      eopnotsupp:
	ptrace(("%s: PROTO: Primitive is not supported\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_OPTMGMT_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", SDT_MOD_NAME));
	return lmi_error_ack(q, LMI_OPTMGMT_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDT User -> SDT Provider (SS7IP) Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
sdt_send_data(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	// printd(("%s: %p: Queuing SDT data\n", SDT_MOD_NAME, sdt));
	bufq_queue(&sdt->sdt.tb, mp);
	if (sdt->sdt.tx_cmp) {
		freemsg(xchg(&sdt->sdt.tx_cmp, NULL));
		sdt->sdt.tx_repeat = 0;
	}
	return (QR_ABSORBED);
}

/*
 *  SDT_DAEDT_TRANSMISSION_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedt_transmission_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	// printd(("%s: %p: Queuing SDT data\n", SDT_MOD_NAME, sdt));
	bufq_queue(&sdt->sdt.tb, mp->b_cont);
	return (QR_TRIMMED);
}

/*
 *  SDT_DAEDT_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedt_start_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	sdt_daedt_start(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_DAEDR_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedr_start_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	sdt_daedr_start(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_AERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_start_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	sdt_aerm_start(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_AERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_stop_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	sdt_aerm_stop(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tin_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	sdt_aerm_set_ti_to_tin(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tie_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	sdt_aerm_set_ti_to_tie(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_SUERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_start_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	sdt_suerm_start(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_SUERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_stop_req(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	if (sdt->state != LMI_ENABLED)
		return m_error(q, EPROTO);
	sdt_suerm_stop(q, sdt);
	return (QR_DONE);
}

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
	-1,				/* SDL_MODE_NONE */
	-1,				/* SDL_MODE_DSU */
	-1,				/* SDL_MODE_CSU */
	ACB56_MODE_DTE,			/* SDL_MODE_DTE */
	ACB56_MODE_DCE,			/* SDL_MODE_DCE */
	-1,				/* SDL_MODE_CLIENT */
	-1,				/* SDL_MODE_SERVER */
	-1,				/* SDL_MODE_PEER */
	ACB56_MODE_ECHO,		/* SDL_MODE_ECHO */
	-1,				/* SDL_MODE_REM_LB */
	ACB56_MODE_LOOP,		/* SDL_MODE_LOC_LB */
	ACB56_MODE_BOTH,		/* SDL_MODE_LB_ECHO */
	ACB56_MODE_JACK			/* SDL_MODE_TEST */
};

enum { ACB56_CLOCK_EXT, ACB56_CLOCK_INT, ACB56_CLOCK_DPLL };

static const int clock_map[] = {
	-1,				/* SDL_CLOCK_NONE */
	ACB56_CLOCK_INT,		/* SDL_CLOCK_INT */
	ACB56_CLOCK_EXT,		/* SDL_CLOCK_EXT */
	-1,				/* SDL_CLOCK_LOOP */
	-1,				/* SDL_CLOCK_MASTER */
	-1,				/* SDL_CLOCK_SLAVE */
	ACB56_CLOCK_DPLL,		/* SDL_CLOCK_DPLL */
	-1,				/* SDL_CLOCK_ABR */
	-1				/* SDL_CLOCK_SHAPER */
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

/*
 *  =========================================================================
 *
 *  INTERRUPT SERVICE ROUTINES
 *
 *  =========================================================================
 */

static inline void
acb56_tx_setup_next_frame(sdt_t * sdt)
{
	sdt->sdt.stats.tx_sus++;
	sdt->sdt.stats.tx_bytes += 4;
	if (bufq_length(&sdt->dev.tinputq)) {
		freemsg(xchg(&sdt->dev.tx_msg, NULL));
		sdt->dev.tx_msg = bufq_dequeue(&sdt->dev.tinputq);
	} else {
		int len = msgdsize(sdt->dev.tx_msg);
		if (len > 5 || (len > 3 && sdt->dev.tx_msg->b_rptr[3] == LSSU_SIB)) {
			/*
			   its an MSU or SIB, make FISU out of it 
			 */
			sdt->dev.tx_msg->b_wptr = sdt->dev.tx_msg->b_rptr + 3;
			sdt->dev.tx_msg->b_rptr[2] = 0;
			sdt->stats.repeated_sus++;
		}
	}
	sdt->dev.tx_buf = sdt->dev.tx_msg->b_rptr;
	sdt->dev.tx_max = sdt->dev.tx_msg->b_wptr;
}

static inline void
acb56_tx_underrun_eom(sdt_t * sdt)
{
	if (sdt->dev.rr0 & 0x40) {	/* set */
		sdt->dev.tx_buf = sdt->dev.tx_msg->b_rptr;
		sdt->dev.tx_error = 1;
		sdt->sdt.stats.tx_aborts++;
		sdt->sdt.stats.tx_underruns++;
		sdt->sdl.state.tx_underruns++;
		sdt->sdt.stats.tx_sus_in_error++;
		sdt->sdt.stats.tx_bytes += 3;
		sdt->dev.rr0 &= ~0x04;	/* clear indication */
		return;
	} else {
		return acb56_tx_setup_next_frame(sdt);
	}
}

static inline void
acb56_sync_hunt(sdt_t * sdt)
{
	int actrl = sdt->dev.iobase + 1;
	if (sdt->dev.rx_octet_mode && !sdt->dev.rr0 & 0x10) {
		sdt->dev.rx_octet_mode = 0;	/* we synced */
		outb(0x0f, actrl);
		outb(sdt->dev.regs[0x0f] &= ~0x02, actrl);	/* turn of octet mode */
	} else if ((sdt->dev.rr0 & 0x10) && (sdt->dev.iface.ifclock != SDL_CLOCK_DPLL)) {
		sdt->dev.rx_octet_count = 0;
		sdt->dev.rx_octet_mode = 1;
		outb(0x0f, actrl);
		outb(sdt->dev.regs[0x0f] |= 0x02, actrl);	/* turn on octet mode */
	}
	sdt->sdt.stats.rx_sync_transitions++;
}
static inline void
acb56_break_abort(sdt_t * sdt)
{
	sdt->sdt.stats.rx_aborts++;
}
static inline void
acb56_dcd(sdt_t * sdt)
{
	if ((++sdt->stats.dcd_transitions) & 0x1) {
		sdt->sdl.stats.lead_dcd_lost++;
		sdt->sdt.stats.carrier_dcd_lost++;
	}
}
static inline void
acb56_cts(sdt_t * sdt)
{
	if ((++sdt->stats.cts_transitions) & 0x1) {
		sdt->sdl.stats.lead_cts_lost++;
		sdt->sdt.stats.carrier_cts_lost++;
	}
}
static inline void
acb56_zero_count(sdt_t * sdt)
{
	if (!sdt->dev.rx_octet_count++ & 0x0000007f || (sdt->dev.iface.ifmode == SDL_MODE_DTE)) {
		sdt->sdt.stats.rx_sus_in_error++;
		sdt->dev.ucalls->daedr_N_octets(&sdt->dev);
	}
	sdt->sdt.stats.rx_bits_octet_counted++;
}

static inline void
acb56_frame_error(sdt_t * sdt)
{
	sdt->dev.rx_error = 0;
	sdt->dev.rx_buf = sdt->dev.rx_msg->b_rptr;
	if (sdt->dev.rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	sdt->dev.cp_msg->b_wptr = sdt->dev.cp_msg->b_rptr;
	sdt->sdt.stats.rx_sus++;
	sdt->sdt.stats.rx_bytes += 2;
	sdt->sdt.stats.rx_sus_in_error++;
	sdt_daedr_su_in_error(NULL, sdt);
}
static inline void
acb56_parity_error(sdt_t * sdt)
{
	sdt->stats.parity_errors++;
	sdt->dev.rx_error = 1;
}
static inline void
acb56_rx_overrun(sdt_t * sdt)
{
	sdt->sdt.stats.rx_overruns++;
	sdt->sdl.stats.rx_overruns++;
	sdt->dev.rx_error = 1;
}
static inline void
acb56_crc_error(sdt_t * sdt)
{
	if (sdt->dev.rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	sdt->sdt.stats.rx_crc_errors++;
	acb56_frame_error(sdt);
}
static inline void
acb56_buffer_error(sdt_t * sdt)
{
	if (sdt->dev.rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	sdt->sdt.stats.rx_buffer_overflows++;
	sdt->sdl.stats.rx_buffer_overflows++;
	acb56_frame_error(sdt);
}
static inline void
acb56_short_error(sdt_t * sdt)
{
	if (sdt->dev.rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	sdt->sdt.stats.rx_frame_too_short++;
	acb56_frame_error(sdt);
}
static inline void
acb56_long_error(sdt_t * sdt)
{
	if (sdt->dev.rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	sdt->sdt.stats.rx_frame_too_long++;
	acb56_frame_error(sdt);
}
static inline void
acb56_length_error(sdt_t * sdt)
{
	if (sdt->dev.rx_octet_mode)
		return;		/* only a good frame counts in octet mode */
	sdt->sdt.stats.rx_length_error++;
	acb56_frame_error(sdt);
}
static inline void
acb56_rx_setup_next_frame(sdt_t * sdt)
{
	int len = sdt->dev.rx_buf - sdt->dev.rx_msg->b_rptr;
	sdt->dev.rx_buf = sdt->dev.rx_msg->b_rptr;
	sdt->sdt.stats.rx_sus++;
	if (len == 3)
		sdt->stats.recv_fisus++;
	if (len >= 6)
		sdt->stats.recv_msus++;
	if (len >= 4)
		sdt->stats.recv_lssus++;
	sdt->sdt.stats.rx_bytes += 2;
}
static inline void
acb56_residue_error(sdt_t * sdt)
{
	sdt->sdt.stats.rx_residue_errors++;
	acb56_frame_error(sdt);
}
static inline void
acb56_end_of_frame(sdt_t * sdt)
{
	if (sdt->dev.rx_error)
		return acb56_frame_error(sdt);
	{
		mblk_t *mp = sdt->dev.rx_msg, *mc = sdt->dev.cp_msg;
		unsigned int len = (unsigned int) (sdt->dev.rx_buf - mp->b_rptr);
		unsigned char li = mp->b_rptr[2] & 0x3f;
		if (len < 3)
			return acb56_short_error(sdt);
		if (len > sdt->sdt.config.m + 4)
			return acb56_long_error(sdt);
		if (li != (len > 63 + 3 ? 63 + 3 : len))
			return acb56_length_error(sdt);
		mp->b_wptr = mp->b_rptr + len;
		if (len < 6) {
			int clen = mc->b_wptr - mc->b_rptr;
			if (len == clen && !memcmp(mc->b_rptr, mp->b_rptr, len)) {
				sdt->stats.compressed_sus++;
				acb56_rx_setup_next_frame(sdt);
				sdt_daedr_correct_su(NULL, sdt);
				return;
			} else {
				memcpy(mc->b_rptr, mp->b_rptr, len);
				mc->b_wptr = mc->b_rptr + len;
			}
		} else
			mc->b_wptr = mc->b_rptr;
		sdt->dev.rx_octet_mode = 0;
		acb56_rx_setup_next_frame(sdt);
		sdt_daedr_received_bits(sdt, mp);
		if (!(sdt->dev.rx_msg = sdt_allocb(NULL, FASTBUF, BPRI_MED)))
			return acb56_buffer_error(sdt);
		sdt->dev.rx_buf = sdt->dev.rx_msg->b_wptr = sdt->dev.rx_msg->b_rptr;
		sdt->dev.rx_max = sdt->dev.rx_msg->b_datap->db_lim;
		return;
	}
}
static inline void
acb56_frame_overflow_check(sdt_t * sdt)
{
	int actrl = sdt->dev.iobase + 1;
	/*
	   check for frame overflow 
	 */
	if (sdt->dev.rx_buf > sdt->dev.rx_max) {
		/*
		   FIGURE 11/Q.703 (sheet 1 of 2) "m+7 octets without flags" 
		 */
		if (!sdt->dev.rx_octet_mode && sdt->dev.iface.ifclock != SDL_CLOCK_DPLL) {
			/*
			   can't octet count on DPLL! 
			 */
			sdt->dev.rx_octet_count = 0;
			sdt->dev.rx_octet_mode = 1;
			outb(0x0f, actrl);
			outb(sdt->dev.regs[0x0f] |= 0x02, actrl);	/* octet counting isr */
			outb(0x03, actrl);
			outb(sdt->dev.regs[0x03] |= 0x10, actrl);	/* force flag hunt */
			acb56_rx_setup_next_frame(sdt);
			sdt->dev.ucalls->daedr_loss_of_sync(&sdt->dev);
		}
	}
}

static void
acb56_isr_cha_tx_buf_empty(sdt_t * sdt)
{
	sdt->stats.cha_tx_buf_empty++;
}

static void
acb56_isr_cha_ext_status(sdt_t * sdt)
{
	unsigned char rr0;
	register int actrl = sdt->dev.iobase + 1;

	rr0 = sdt->dev.rr0 & ~0x02;
	outb(0x00, actrl);
	sdt->dev.rr0 = inb(actrl);
	outb(0x00, actrl);
	outb(0x10, actrl);
	outb(0x00, actrl);
	outb(0x10, actrl);	/* debounce */
	rr0 ^= sdt->dev.rr0 & 0xfa;
	if (rr0) {
		if (rr0 & 0x40)
			acb56_tx_underrun_eom(sdt);
		if (rr0 & 0x10)
			acb56_sync_hunt(sdt);
		if (rr0 & 0x80)
			acb56_break_abort(sdt);
		if (rr0 & 0x08)
			acb56_dcd(sdt);
		if (rr0 & 0x20)
			acb56_cts(sdt);
	} else
		acb56_zero_count(sdt);
	sdt->stats.cha_ext_status++;
}

static void
acb56_isr_cha_rx_char_avail(sdt_t * sdt)
{
	register int adata = sdt->dev.iobase;
	register int i = 0;
	if (sdt->dev.rx_buf) {
		/*
		   collect bytes 
		 */
		for (i = 0; i < 4; i++)
			*(sdt->dev.rx_buf++) = inb(adata);
		acb56_frame_overflow_check(sdt);
	}
	sdt->sdt.stats.rx_bytes += 4;
	sdt->stats.cha_rx_char_avail++;

}
static void
acb56_isr_cha_rx_sp_cond(sdt_t * sdt)
{
	unsigned char rr1 = 0;
	register int adata = sdt->dev.iobase;
	register int actrl = sdt->dev.iobase + 1;
	register int i = 0;
	sdt->stats.cha_rx_sp_cond++;
	/*
	   collect bytes 
	 */
	outb(0x00, actrl);
	for (i = 0; i < 4 && (inb(actrl) & 0x01); i++) {
		*(sdt->dev.rx_buf++) = inb(adata);
		sdt->sdt.stats.rx_bytes++;
		outb(0x00, actrl);
	}
	acb56_frame_overflow_check(sdt);
	/*
	   check for special conditions 
	 */
	outb(0x07, actrl);
	if (inb(actrl) & 0x40) {
		outb(0x01, actrl);
		if ((rr1 = inb(actrl)) & 0xf0) {
			outb(0x00, actrl);
			outb(0x30, actrl);	/* reset error */
			if (rr1 & 0x10) {
				return acb56_parity_error(sdt);
			}
			if (rr1 & 0x20) {
				return acb56_rx_overrun(sdt);
			}
			if (rr1 & 0x80) {
				if (rr1 & 0x40) {
					return acb56_crc_error(sdt);
				} else {
					if ((rr1 & 0xe) ^ 0x6) {
						return acb56_residue_error(sdt);
					} else {
						return acb56_end_of_frame(sdt);
					}
				}
			}
		}
	}
}

static void
acb56_isr_donothing(sdt_t * sdt)
{
	(void) sdt;
};

static void
acb56_isr_chb_tx_buf_empty(sdt_t * sdt)
{
	sdt->stats.chb_tx_buf_empty++;
}
static void
acb56_isr_chb_ext_status(sdt_t * sdt)
{
	sdt->stats.chb_ext_status++;
}
static void
acb56_isr_chb_rx_char_avail(sdt_t * sdt)
{
	sdt->stats.chb_rx_char_avail++;
}
static void
acb56_isr_chb_rx_sp_cond(sdt_t * sdt)
{
	sdt->stats.chb_rx_sp_cond++;
}

static void
acb56_isr(int irq, void *dev_id, struct pt_regs *regs)
{
	/* *INDENT-OFF* */
	static void (*vector_map[]) (sdt_t *) = {
		acb56_isr_chb_tx_buf_empty,
		acb56_isr_chb_ext_status,
		acb56_isr_chb_rx_char_avail,
		acb56_isr_chb_rx_sp_cond,
		acb56_isr_cha_tx_buf_empty,
		acb56_isr_cha_ext_status,
		acb56_isr_cha_rx_char_avail,
		acb56_isr_cha_rx_sp_cond
	};
	/* *INDENT-ON* */
	unsigned char rr3;
	register int i;
	register int actrl = ((sdt_t *) dev_id)->dev.iobase + 1;
	for (i = 0, outb(0x03, actrl), rr3 = inb(actrl); i < 4 && rr3;
	     i++, outb(0x03, actrl), rr3 = inb(actrl)) {
		outb(0x02, actrl + 2);
		(*vector_map[inb(actrl + 2) >> 1]) (dev_id);
		/*
		   reset highest interrupt under service 
		 */
		outb(0x00, actrl);
		outb(0x38, actrl);
	}
	((sdt_t *) dev_id)->stats.interrupts++;
};

/*
 *  =========================================================================
 *
 *  DRIVER SERVICE CALLS
 *
 *  =========================================================================
 */

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
static int
acb56_attach(sdt_t * sdt)
{
	int iobase, _irq, actrl, _dma_rx, _dma_tx, i, err;
	unsigned long time, cookie;
	board = sdt->u.dev.cminor - 1;
	if ((iobase = io[board]) == -1)
		iobase = ports[board];
	if ((err = check_region(iobase, 8)))
		return (-EBUSY);
	actrl = iobase + 1;
	outb(0x02, actrl);
	outb(0x55, actrl);	/* write to unallocated 8530 bit in WR2 */
	outb(0x02, actrl);
	if (inb(actrl) != 0x55)
		return (-ENXIO);	/* probably an 8530 */
	outb(0x09, actrl);
	outb(0xc0, actrl);	/* force hardware reset */
	outb(0x0f, actrl);
	outb(0x01, actrl);	/* Access W7P register */
	outb(0x0f, actrl);
	if (!inb(actrl) & 0x01)
		return (-ENXIO);	/* probably an 8530 */
	outb(0x0f, actrl);
	outb(0x00, actrl);	/* Remove accss to W7P register */
	outb(0x0f, actrl);
	if (inb(actrl) & 0x01)
		return (-ENXIO);	/* probably an 8530 */
	/*
	   check assigned irq 
	 */
	if ((_irq = irq[board]) != -1) {
		if ((err = request_irq(_irq, dummy_isr, SA_SHIRQ, "acb56_dummy", NULL)))
			return (-EBUSY);
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
	   fill tx fifo to get an interrupt 
	 */
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
		return (-EBUSY);	/* no irq! */
	outb(0x03, actrl);
	if (!inb(actrl))
		return (-EBUSY);	/* it wasn't us */
	outb(0x09, actrl);
	outb(0x00, actrl);
	outb(0x09, actrl);
	outb(0xc0, actrl);	/* force hardware reset */
	if ((err = request_irq(_irq, dummy_isr, SA_SHIRQ, "acb56_dummy", NULL)))
		return (-EBUSY);
      acb_probe_dma:
	free_irq(_irq, NULL);
	/*
	   check for dma 
	 */
	if ((_dma_rx = dma_rx[board]) && _dma_rx != -1 && !(request_dma(_dma_rx, "acb56_probe")))
		free_dma(_dma_rx);
	else
		_dma_rx = 0;
	if ((_dma_tx = dma_tx[board]) && _dma_tx != -1 && !(request_dma(_dma_tx, "acb56_probe")))
		free_dma(_dma_tx);
	else
		_dma_tx = 0;
	sdt->dev.ifindex = board;
	sdt->dev.irq = _irq;
	sdt->dev.iobase = iobase;
	sdt->dev.dma_tx = _dma_tx;
	sdt->dev.dma_rx = _dma_rx;
	if (mode[board] != -1)
		sdt->dev.iface.ifmode = mode[board];
	if (clock[board] != -1)
		sdt->dev.iface.ifclock = clock[board];
	ptrace(("sucessful:\n"));
	ptrace(("  ifindex = %lu\n", sdt->dev.ifindex));
	ptrace(("  irq     = %lu\n", sdt->dev.irq));
	ptrace(("  iobase  = 0x%lx\n", sdt->dev.iobase));
	ptrace(("  dma_tx  = %lu\n", sdt->dev.dma_tx));
	ptrace(("  dma_rx  = %lu\n", sdt->dev.dma_rx));
	return (sdt);
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
	sdt_t *sdt = (sdt_t *) lmi;
	int err = 0;
	unsigned long flags;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		MOD_INC_USE_COUNT;
		/*
		   get io region 
		 */
		if ((err = check_region(sdt->dev.iobase, 8))) {
			MOD_DEC_USE_COUNT;
			spin_unlock_irqrestore(&sdt->lock, flags);
			return err;
		}
		request_region(sdt->dev.iobase, 8, "acb56");
		/*
		   get dma channels 
		 */
		if (sdt->dev.dma_rx && request_dma(sdt->dev.dma_rx, "acb56"))
			sdt->dev.dma_rx = 0;
		if (sdt->dev.dma_tx && request_dma(sdt->dev.dma_tx, "acb56"))
			sdt->dev.dma_tx = 0;
		/*
		   get interrupt 
		 */
		if ((err = request_irq(sdt->dev.irq, acb56_isr, SA_SHIRQ, "acb56", sdt))) {
			if (sdt->dev.dma_rx)
				free_dma(sdt->dev.dma_rx);
			if (sdt->dev.dma_tx)
				free_dma(sdt->dev.dma_tx);
			MOD_DEC_USE_COUNT;
			spin_unlock_irqrestore(&sdt->lock, flags);
			return err;
		}
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
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
	sdt_t *sdt = (sdt_t *) lmi;
	unsigned long flags;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		free_irq(sdt->dev.irq, sdt);
		if (sdt->dev.dma_tx)
			free_dma(sdt->dev.dma_tx);
		if (sdt->dev.dma_rx)
			free_dma(sdt->dev.dma_rx);
		release_region(sdt->dev.iobase, 8);
		MOD_DEC_USE_COUNT;
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
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
	sdt_t *sdt = (sdt_t *) lmi;
	unsigned long flags;
	(void) dev;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		*lenp = 0;
		*ppap = NULL;
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
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
	sdt_t *sdt = (sdt_t *) lmi;
	unsigned long flags;
	(void) dev;
	(void) ppa;
	(void) len;
	spin_lock_irqsave(&sdt->lock, flags);
	{
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
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
	sdt_t *sdt = (sdt_t *) lmi;
	unsigned long flags;
	(void) dev;
	spin_lock_irqsave(&sdt->lock, flags);
	{
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
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
	sdt_t *sdt = (sdt_t *) lmi;
	int i, actrl;
	unsigned long flags;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		actrl = sdt->dev.iobase + 1;
		for (i = 0; i < 16; i++)
			sdt->dev.regs[i] = 0;	/* register images */
		/*
		   setup chip 
		 */
		for (i = 0; i < sizeof(preset);) {
			outb(preset[i], actrl);
			i++;
			outb(sdt->dev.regs[i >> 1] = preset[i], actrl);
			i++;
		}
		/*
		   setup interface and clock modes 
		 */
		outb(0x0b, actrl);
		outb(sdt->dev.regs[0x0b] =
		     mode_clock[mode_map[sdt->sdl.config.ifmode]][clock_map
								  [sdt->sdl.config.ifclock]],
		     actrl);
		/*
		   setup baud rate generator 
		 */
		if (sdt->sdl.config.ifmode == SDL_MODE_DTE) {
			outb(0x0c, actrl);
			outb(sdt->dev.regs[0xc] = 0xca, actrl);
			outb(0x0d, actrl);
			outb(sdt->dev.regs[0xd] = 0x1c, actrl);
		} else if (sdt->sdl.config.ifmode == SDL_MODE_LOC_LB) {
			outb(0x0c, actrl);
			outb(sdt->dev.regs[0xc] = 0x00, actrl);
			outb(0x0d, actrl);
			outb(sdt->dev.regs[0xd] = 0x00, actrl);
		} else {
			outb(0x0c, actrl);
			outb(sdt->dev.regs[0xc] = 0x40, actrl);
			outb(0x0d, actrl);
			outb(sdt->dev.regs[0xd] = 0x00, actrl);
		}
		/*
		   special DPLL modes 
		 */
		if (sdt->sdl.config.ifclock == SDL_CLOCK_DPLL) {
			outb(0x0e, actrl);
			outb(0x60, actrl);
			outb(0x0e, actrl);
			outb(0xe0, actrl);
			if (sdt->sdl.config.ifmode == SDL_MODE_DTE) {
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
		outb(sdt->dev.regs[0x0e] = 0x02, actrl);
		/*
		   setup loopback and echo modes 
		 */
		switch (sdt->sdl.config.ifmode) {
		case SDL_MODE_LOC_LB:
			outb(0x0e, actrl);
			outb(sdt->dev.regs[0x0e] |= 0x10, actrl);
			break;
		case SDL_MODE_ECHO:
			outb(0x0e, actrl);
			outb(sdt->dev.regs[0x0e] |= 0x08, actrl);
			break;
		case SDL_MODE_LB_ECHO:
			outb(0x0e, actrl);
			outb(sdt->dev.regs[0x0e] |= 0x18, actrl);
			break;
		}
		/*
		   set up dma registers 
		 */
		if (sdt->dev.dma_rx || sdt->dev.dma_tx) {
			outb(0x0e, actrl);
			outb(sdt->dev.regs[0x0e] |= 0x04, actrl);
			if (sdt->dev.dma_rx && sdt->dev.dma_tx) {
				outb(0x01, actrl);
				outb(sdt->dev.regs[0x01] |= 0xf9, actrl);
			}
			if (sdt->dev.dma_tx) {
				outb(0x01, actrl);
				outb(sdt->dev.regs[0x01] |= 0xc1, actrl);
			}
			if (sdt->dev.dma_tx) {
				outb(0x01, actrl);
				outb(sdt->dev.regs[0x01] |= 0xfb, actrl);
			}
			outb(0x80, actrl + 3);
			if (sdt->dev.dma_rx)
				enable_dma(sdt->dev.dma_rx);
			if (sdt->dev.dma_tx)
				enable_dma(sdt->dev.dma_tx);
		} else {
			outb(0x0e, actrl);
			outb(sdt->dev.regs[0x0e] &= ~0x04, actrl);
			outb(0x01, actrl);
			outb(sdt->dev.regs[0x01] |= 0x13, actrl);
			outb(0x00, actrl + 3);
		}
		/*
		   disable status fifo 
		 */
		outb(0x0f, actrl);
		outb(0xfc, actrl);
		outb(0x09, actrl);
		outb(0x02, actrl);
		/*
		   reset and enable transmitters and receivers 
		 */
		outb(0x0E, actrl);
		outb(sdt->dev.regs[0x0e] |= 0x01, actrl);
		outb(0x03, actrl);
		outb(sdt->dev.regs[0x0e] |= 0x01, actrl);
		outb(0x00, actrl);
		outb(0x30, actrl);
		outb(0x05, actrl);
		outb(sdt->dev.regs[0x0e] |= 0x08, actrl);
		outb(0x00, actrl);
		outb(0x80, actrl);
		outb(0x00, actrl);
		outb(0xC0, actrl);
		outb(0x00, actrl);
		outb(0x10, actrl);
		outb(0x00, actrl);
		outb(0x10, actrl);
		sdt->sdl.config.ifflags |= SDL_IF_TX_RUNNING;
		sdt->sdl.config.ifflags |= SDL_IF_RX_RUNNING;
		bufq_init(&sdt->dev.tinputq);
		sdt->dev.tx_msg = sdt_allocb(q, FASTBUF, BPRI_MED);
		sdt->dev.rx_msg = sdt_allocb(q, FASTBUF, BPRI_MED);
		sdt->dev.cp_msg = sdt_allocb(q, FASTBUF, BPRI_MED);
		if (!sdt->dev.tx_msg || !sdt->dev.rx_msg || !sdt->dev.cp_msg) {
			if (sdt->dev.tx_msg)
				freemsg(sdt->dev.tx_msg);
			if (sdt->dev.rx_msg)
				freemsg(sdt->dev.rx_msg);
			if (sdt->dev.cp_msg)
				freemsg(sdt->dev.cp_msg);
			spin_unlock_irqrestore(&sdt->lock, flags);
			return ENOBUFS;
		}
		*(sdt->dev.tx_msg->b_wptr)++ = 0x80;	/* Initial SIOS */
		*(sdt->dev.tx_msg->b_wptr)++ = 0x80;	/* Initial SIOS */
		*(sdt->dev.tx_msg->b_wptr)++ = 0x01;	/* Initial SIOS */
		*(sdt->dev.tx_msg->b_wptr)++ = 0x00;	/* Initial SIOS */
		/*
		   enable master interrupt bit 
		 */
		outb(0x09, actrl);
		outb(sdt->dev.regs[0x09] |= 0x08, actrl);
		/*
		   we're running! phew! 
		 */
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
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
	sdt_t *sdt = (sdt_t *) lmi;
	int actrl = sdt->dev.iobase + 1;
	unsigned long flags;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		sdt->sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
		sdt->sdl.config.ifflags &= ~SDL_IF_RX_RUNNING;
		sdt->sdl.config.ifflags &= ~SDL_IF_SU_COMPRESS;
		outb(0x09, actrl);
		outb(0xc0, actrl);	/* force hw reset */
		outb(0x09, actrl);
		outb(sdt->dev.regs[0x09] &= ~0x08, actrl);	/* stop interrupts */
		if (sdt->dev.dma_tx) {
			outb(0x0e, actrl);
			outb(sdt->dev.regs[0x0e] &= ~0x04, actrl);	/* disable dma */
			disable_dma(sdt->dev.dma_tx);
		}
		if (sdt->dev.dma_rx) {
			outb(0x01, actrl);
			outb(sdt->dev.regs[0x01] &= ~0xc0, actrl);	/* disable dma */
			disable_dma(sdt->dev.dma_rx);
		}
		outb(0x09, actrl);
		outb(0xc0, actrl);	/* force hw reset */
		bufq_purge(&sdt->dev.tinputq);
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
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
	sdt_t *sdt = (sdt_t *) sdl->device;
	sdl_config_t *ureq = NULL;
	sdl_ulong uarg = 0;

	if (_IOC_TYPE(cmd) == SDL_IOC_MAGIC)
		switch (cmd) {
		case SDL_IOCTCONFIG:
		case SDL_IOCSCONFIG:
			if (!arg || size < sizeof(sdl_config_t))
				return EINVAL;
			ureq = arg;
			break;
		case SDL_IOCCCONFIG:
		case SDL_IOCCMRESET:
			break;
		default:
			return EOPNOTSUPP;
		}
	if (_IOC_TYPE(cmd) == SDL_IOC_MAGIC)
		switch (cmd) {
		case SDL_IOCGIFFLAGS:
		case SDL_IOCGIFTYPE:
		case SDL_IOCGGRPTYPE:
		case SDL_IOCGIFMODE:
		case SDL_IOCGIFRATE:
		case SDL_IOCGIFCLOCK:
		case SDL_IOCGIFCODING:
		case SDL_IOCGIFLEADS:
		case SDL_IOCSIFFLAGS:
		case SDL_IOCSIFTYPE:
		case SDL_IOCSGRPTYPE:
		case SDL_IOCSIFMODE:
		case SDL_IOCSIFRATE:
		case SDL_IOCSIFCLOCK:
		case SDL_IOCSIFCODING:
		case SDL_IOCSIFLEADS:
		case SDL_IOCCIFLEADS:
			if (!arg || size < sizeof(dev_ulong))
				return EINVAL;
			uarg = *(dev_ulong *) arg;
			break;
		case SDL_IOCCIFRESET:
		case SDL_IOCCDISCTX:
		case SDL_IOCCCONNTX:
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
	if (_IOC_TYPE(cmd) == SDL_IOC_MAGIC)
		switch (cmd) {
		case SDL_IOCCIFRESET:
			break;
			/*
			   gets 
			 */
		case SDL_IOCGIFFLAGS:
			*(dev_ulong *) arg = sdt->sdl.config.ifflags;
			return (0);
		case SDL_IOCGIFTYPE:
			*(dev_ulong *) arg = sdt->sdl.config.iftype;
			return (0);
		case SDL_IOCGGRPTYPE:
			*(dev_ulong *) arg = sdt->sdl.config.ifgtype;
			return (0);
		case SDL_IOCGIFMODE:
			*(dev_ulong *) arg = sdt->sdl.config.ifmode;
			return (0);
		case SDL_IOCGIFRATE:
			*(dev_ulong *) arg = sdt->sdl.config.ifrate;
			return (0);
		case SDL_IOCGIFCLOCK:
			*(dev_ulong *) arg = sdt->sdl.config.ifclock;
			return (0);
		case SDL_IOCGIFCODING:
			*(dev_ulong *) arg = sdt->sdl.config.ifcoding;
			return (0);
		case SDL_IOCGIFLEADS:
			*(dev_ulong *) arg = sdt->sdl.config.ifleads;
			return (0);
			/*
			   sets 
			 */
		case SDL_IOCSIFFLAGS:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			sdt->sdl.config.ifflags = uarg;
			return (0);
		case SDL_IOCSIFTYPE:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			if (uarg != SDL_TYPE_V35)
				return EINVAL;
			return (0);
		case SDL_IOCSGRPTYPE:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			if (uarg != SDL_GTYPE_NONE)
				return EINVAL;
			return (0);
		case SDL_IOCSIFMODE:
			ptrace(("uarg = %lu\n", uarg));
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			switch (uarg) {
			case SDL_MODE_DTE:
			case SDL_MODE_DCE:
			case SDL_MODE_REM_LB:
			case SDL_MODE_LOC_LB:
			case SDL_MODE_LB_ECHO:
			case SDL_MODE_TEST:
				if (mode_map[uarg] != -1) {
					sdt->sdl.config.ifmode = uarg;
					return (0);
				}
			default:
				return EINVAL;
			}
			return (0);
		case SDL_IOCSIFRATE:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			if (uarg != 56000)
				return EINVAL;
			return (0);
		case SDL_IOCSIFCLOCK:
			ptrace(("uarg = %lu\n", uarg));
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			switch (uarg) {
			case SDL_CLOCK_INT:
			case SDL_CLOCK_EXT:
			case SDL_CLOCK_DPLL:
				if (clock_map[uarg] != -1) {
					sdt->sdl.config.ifclock = uarg;
					return (0);
				}
			default:
				return EINVAL;
			}
			return (0);
		case SDL_IOCSIFCODING:
			if (sdl->state == LMI_ENABLED)
				return EBUSY;
			if (uarg != SDL_CODING_NRZI)
				return EINVAL;
			return (0);
		case SDL_IOCSIFLEADS:
		case SDL_IOCCIFLEADS:
			/*
			   FIXME: control the leads 
			 */
			if (uarg)
				return EINVAL;
			return (0);
		case SDL_IOCCDISCTX:
			sdt->sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
			return (0);
		case SDL_IOCCCONNTX:
			sdt->sdl.config.ifflags |= SDL_IF_TX_RUNNING;
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
	sdt_t *sdt = (sdt_t *) dev;
	unsigned long flags;

	spin_lock_irqsave(&sdt->lock, flags);
	{
		if (!(sdt->sdl.config.ifflags & SDL_IF_TX_RUNNING))
			freemsg(mp);
		else
			bufq_queue(&sdt->dev.tinputq, mp);
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
}

static void
acb56_tx_start(struct dev *dev)
{
	sdt_t *sdt = (sdt_t *) dev;
	sdt->sdl.config.ifflags |= SDL_IF_TX_RUNNING;
}

static void
acb56_rx_start(struct dev *dev)
{
	sdt_t *sdt = (sdt_t *) dev;
	sdt->sdl.config.ifflags |= SDL_IF_RX_RUNNING;
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
}

void
acb56_terminate(void)
{
	if (acb56_initialized <= 0)
		return;
	ptrace(("unregistering sdl_acb cmajor %d\n", acb56_cmajor));
	acb56_initialized = sdl_unregister_driver(acb56_cmajor);
	ptrace(("return = %d\n", acb56_initialized));
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

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *
 *  LM IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
lmi_test_config(sdt_t * sdt, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_commit_config(sdt_t * sdt, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->option;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->option = *arg;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			arg->version = sdt->version;
			arg->style = sdt->style;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->version = arg->version;
			sdt->style = arg->style;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_test_config(sdt, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_commit_config(sdt, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			arg->state = sdt->state;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->state = LMI_UNUSABLE;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdt.statsp;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->sdt.statsp = *arg;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			ret = -EOPNOTSUPP;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccstats(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	psw_t flags;
	int ret = 0;
	(void) mp;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		ret = -EOPNOTSUPP;
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	return (ret);
}
STATIC int
lmi_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			ret = -EOPNOTSUPP;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			ret = -EOPNOTSUPP;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			ret = -EOPNOTSUPP;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  =========================================================================
 *
 *  SDT IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_test_config(sdt_t * sdt, sdt_config_t * arg)
{
	int ret = 0;
	psw_t flags;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	do {
		if (!arg->t8)
			arg->t8 = sdt->sdt.config.t8;
		if (!arg->Tin)
			arg->Tin = sdt->sdt.config.Tin;
		if (!arg->Tie)
			arg->Tie = sdt->sdt.config.Tie;
		if (!arg->T)
			arg->T = sdt->sdt.config.T;
		if (!arg->D)
			arg->D = sdt->sdt.config.D;
		if (!arg->Te)
			arg->Te = sdt->sdt.config.Te;
		if (!arg->De)
			arg->De = sdt->sdt.config.De;
		if (!arg->Ue)
			arg->Ue = sdt->sdt.config.Ue;
		if (!arg->N)
			arg->N = sdt->sdt.config.N;
		if (!arg->m)
			arg->m = sdt->sdt.config.m;
		if (!arg->b)
			arg->b = sdt->sdt.config.b;
		else if (arg->b != sdt->sdt.config.b) {
			ret = -EINVAL;
			break;
		}
	} while (0);
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	return (ret);
}
STATIC int
sdt_commit_config(sdt_t * sdt, sdt_config_t * arg)
{
	psw_t flags;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		sdt_test_config(sdt, arg);
		sdt->sdt.config = *arg;
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	return (0);
}
STATIC int
sdt_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->option;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->option = *arg;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdt.config;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->sdt.config = *arg;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_test_config(sdt, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_commit_config(sdt, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdt.statem;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccmreset(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	(void) sdt;
	(void) mp;
	fixme(("%s: Master reset\n", SDT_MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
sdt_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdt.statsp;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->sdt.statsp = *arg;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdt.stats;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccstats(queue_t *q, mblk_t *mp)
{
	psw_t flags;
	sdt_t *sdt = SDT_PRIV(q);
	(void) mp;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		bzero(&sdt->sdt.stats, sizeof(sdt->sdt.stats));
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	return (0);
}
STATIC int
sdt_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdt.notify;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->sdt.notify = *arg;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->sdt.notify.events &= ~arg->events;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccabort(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	psw_t flags;
	int ret = 0;
	(void) mp;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		ret = -EOPNOTSUPP;
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	return (ret);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_test_config(sdt_t * sdt, sdl_config_t * arg)
{
	(void) sdt;
	(void) arg;
	// fixme(("%s: FIXME: write this function\n", SDT_MOD_NAME));
	return (0);
}
STATIC void
sdl_commit_config(sdt_t * sdt, sdl_config_t * arg)
{
	long tdiff;
	sdt->sdl.config = *arg;
	switch (sdt->sdl.config.ifclock) {
	case SDL_CLOCK_TICK:
	case SDL_CLOCK_SHAPER:
		/*
		   reshape traffic 
		 */
		if ((tdiff = sdt->sdl.timestamp - jiffies) > 0)
			sdt->sdl.bytecount += sdt->sdl.tickbytes * tdiff;
		else
			sdt->sdl.bytecount = 0;
		sdt->sdl.timestamp = jiffies;
		sdt->sdl.tickbytes = sdt->sdl.config.ifrate / HZ / 8;
		if (sdt->sdl.tickbytes < 1)
			sdt->sdl.tickbytes = 1;
		ensure(sdt->sdl.tickbytes > 0, sdt->sdl.tickbytes = 1);
		while (sdt->sdl.bytecount >= sdt->sdl.tickbytes) {
			sdt->sdl.bytecount -= sdt->sdl.tickbytes;
			sdt->sdl.timestamp++;
		}
	}
}
STATIC int
sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->option;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->option = *arg;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdl.config;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			if (!(ret = sdl_test_config(sdt, arg)))
				sdl_commit_config(sdt, arg);
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			ret = sdl_test_config(sdt, arg);
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdl_commit_config(sdt, arg);
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdl.statem;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccmreset(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	(void) sdt;
	(void) arg;
	fixme(("%s: FIXME: Support master reset\n", SDT_MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdl.statsp;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		fixme(("%s: FIXME: check these settings\n", SDT_MOD_NAME));
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->sdl.statsp = *arg;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdl.stats;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	psw_t flags;
	(void) mp;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		bzero(&sdt->sdl.stats, sizeof(sdt->sdl.stats));
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	return (0);
}
STATIC int
sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			*arg = sdt->sdl.notify;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->sdl.notify.events |= arg->events;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_t *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		lis_spin_lock_irqsave(&sdt->lock, &flags);
		{
			sdt->sdl.notify.events &= ~arg->events;
		}
		lis_spin_unlock_irqrestore(&sdt->lock, &flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccdisctx(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	psw_t flags;
	(void) mp;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		sdt->sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	return (0);
}
STATIC int
sdl_ioccconntx(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	psw_t flags;
	(void) mp;
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		sdt->sdl.config.ifflags |= SDL_IF_TX_RUNNING;
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_w_ioctl(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;
	switch (type) {
	case __SID:
	{
		switch (nr) {
		case _IOC_NR(I_STR):
		case _IOC_NR(I_LINK):
		case _IOC_NR(I_PLINK):
		case _IOC_NR(I_UNLINK):
		case _IOC_NR(I_PUNLINK):
			(void) lp;
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", SDT_MOD_NAME, nr));
			ret = -EINVAL;
			break;
		default:
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", SDT_MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case LMI_IOC_MAGIC:
	{
		if (count < size || SDT_PRIV(q)->state == LMI_UNATTACHED) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(LMI_IOCGOPTIONS):	/* lmi_option_t */
			ret = lmi_iocgoptions(q, mp);
			break;
		case _IOC_NR(LMI_IOCSOPTIONS):	/* lmi_option_t */
			ret = lmi_iocsoptions(q, mp);
			break;
		case _IOC_NR(LMI_IOCGCONFIG):	/* lmi_config_t */
			ret = lmi_iocgconfig(q, mp);
			break;
		case _IOC_NR(LMI_IOCSCONFIG):	/* lmi_config_t */
			ret = lmi_iocsconfig(q, mp);
			break;
		case _IOC_NR(LMI_IOCTCONFIG):	/* lmi_config_t */
			ret = lmi_ioctconfig(q, mp);
			break;
		case _IOC_NR(LMI_IOCCCONFIG):	/* lmi_config_t */
			ret = lmi_ioccconfig(q, mp);
			break;
		case _IOC_NR(LMI_IOCGSTATEM):	/* lmi_statem_t */
			ret = lmi_iocgstatem(q, mp);
			break;
		case _IOC_NR(LMI_IOCCMRESET):	/* lmi_statem_t */
			ret = lmi_ioccmreset(q, mp);
			break;
		case _IOC_NR(LMI_IOCGSTATSP):	/* lmi_sta_t */
			ret = lmi_iocgstatsp(q, mp);
			break;
		case _IOC_NR(LMI_IOCSSTATSP):	/* lmi_sta_t */
			ret = lmi_iocsstatsp(q, mp);
			break;
		case _IOC_NR(LMI_IOCGSTATS):	/* lmi_stats_t */
			ret = lmi_iocgstats(q, mp);
			break;
		case _IOC_NR(LMI_IOCCSTATS):	/* lmi_stats_t */
			ret = lmi_ioccstats(q, mp);
			break;
		case _IOC_NR(LMI_IOCGNOTIFY):	/* lmi_notify_t */
			ret = lmi_iocgnotify(q, mp);
			break;
		case _IOC_NR(LMI_IOCSNOTIFY):	/* lmi_notify_t */
			ret = lmi_iocsnotify(q, mp);
			break;
		case _IOC_NR(LMI_IOCCNOTIFY):	/* lmi_notify_t */
			ret = lmi_ioccnotify(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SDT ioctl %d\n", SDT_MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDT_IOC_MAGIC:
	{
		if (count < size || SDT_PRIV(q)->state == LMI_UNATTACHED) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDT_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdt_iocgoptions(q, mp);
			break;
		case _IOC_NR(SDT_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdt_iocsoptions(q, mp);
			break;
		case _IOC_NR(SDT_IOCGCONFIG):	/* sdt_config_t */
			ret = sdt_iocgconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCSCONFIG):	/* sdt_config_t */
			ret = sdt_iocsconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCTCONFIG):	/* sdt_config_t */
			ret = sdt_ioctconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCCCONFIG):	/* sdt_config_t */
			ret = sdt_ioccconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATEM):	/* sdt_statem_t */
			ret = sdt_iocgstatem(q, mp);
			break;
		case _IOC_NR(SDT_IOCCMRESET):	/* sdt_statem_t */
			ret = sdt_ioccmreset(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATSP):	/* lmi_sta_t */
			ret = sdt_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SDT_IOCSSTATSP):	/* lmi_sta_t */
			ret = sdt_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATS):	/* sdt_stats_t */
			ret = sdt_iocgstats(q, mp);
			break;
		case _IOC_NR(SDT_IOCCSTATS):	/* sdt_stats_t */
			ret = sdt_ioccstats(q, mp);
			break;
		case _IOC_NR(SDT_IOCGNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocgnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCSNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocsnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCCNOTIFY):	/* sdt_notify_t */
			ret = sdt_ioccnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCCABORT):	/* */
			ret = sdt_ioccabort(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SDT ioctl %d\n", SDT_MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || sdt->state == LMI_UNATTACHED) {
			ptrace(("%s: ERROR: ioctl count = %d, size = %d, state = %d\n",
				SDT_MOD_NAME, count, size, sdt->state));
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdl_iocgoptions(q, mp);
			break;
		case _IOC_NR(SDL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdl_iocsoptions(q, mp);
			break;
		case _IOC_NR(SDL_IOCGCONFIG):	/* sdl_config_t */
			ret = sdl_iocgconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCSCONFIG):	/* sdl_config_t */
			ret = sdl_iocsconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCTCONFIG):	/* sdl_config_t */
			ret = sdl_ioctconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCCCONFIG):	/* sdl_config_t */
			ret = sdl_ioccconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATEM):	/* sdl_statem_t */
			ret = sdl_iocgstatem(q, mp);
			break;
		case _IOC_NR(SDL_IOCCMRESET):	/* sdl_statem_t */
			ret = sdl_ioccmreset(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATSP):	/* sdl_stats_t */
			ret = sdl_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SDL_IOCSSTATSP):	/* sdl_stats_t */
			ret = sdl_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATS):	/* sdl_stats_t */
			ret = sdl_iocgstats(q, mp);
			break;
		case _IOC_NR(SDL_IOCCSTATS):	/* sdl_stats_t */
			ret = sdl_ioccstats(q, mp);
			break;
		case _IOC_NR(SDL_IOCGNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocgnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCSNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocsnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCCNOTIFY):	/* sdl_notify_t */
			ret = sdl_ioccnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCCDISCTX):	/* */
			ret = sdl_ioccdisctx(q, mp);
			break;
		case _IOC_NR(SDL_IOCCCONNTX):	/* */
			ret = sdl_ioccconntx(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SDL ioctl %d\n", SDT_MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ret = (QR_PASSALONG);
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
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
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sdt_t *sdt = SDT_PRIV(q);
	ulong oldstate = sdt->state;
	/*
	   Fast Path 
	 */
	if ((prim = *(ulong *) mp->b_rptr) == SDT_DAEDT_TRANSMISSION_REQ) {
		printd(("%s: %p: -> SDT_DAEDT_TRANSMISSION_REQ\n", SDT_MOD_NAME, sdt));
		if ((rtn = sdt_daedt_transmission_req(q, mp)) < 0)
			sdt->state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", SDT_MOD_NAME, sdt));
		rtn = lmi_info_req(q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", SDT_MOD_NAME, sdt));
		rtn = lmi_attach_req(q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", SDT_MOD_NAME, sdt));
		rtn = lmi_detach_req(q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", SDT_MOD_NAME, sdt));
		rtn = lmi_enable_req(q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", SDT_MOD_NAME, sdt));
		rtn = lmi_disable_req(q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", SDT_MOD_NAME, sdt));
		rtn = lmi_optmgmt_req(q, mp);
		break;
	case SDT_DAEDT_TRANSMISSION_REQ:
		printd(("%s: %p: -> SDT_DAEDT_TRANSMISSION_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_daedt_transmission_req(q, mp);
		break;
	case SDT_DAEDT_START_REQ:
		printd(("%s: %p: -> SDT_DAEDT_START_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_daedt_start_req(q, mp);
		break;
	case SDT_DAEDR_START_REQ:
		printd(("%s: %p: -> SDT_DAEDR_START_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_daedr_start_req(q, mp);
		break;
	case SDT_AERM_START_REQ:
		printd(("%s: %p: -> SDT_AERM_START_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_aerm_start_req(q, mp);
		break;
	case SDT_AERM_STOP_REQ:
		printd(("%s: %p: -> SDT_AERM_STOP_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_aerm_stop_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIN_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_aerm_set_ti_to_tin_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIE_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_aerm_set_ti_to_tie_req(q, mp);
		break;
	case SDT_SUERM_START_REQ:
		printd(("%s: %p: -> SDT_SUERM_START_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_suerm_start_req(q, mp);
		break;
	case SDT_SUERM_STOP_REQ:
		printd(("%s: %p: -> SDT_SUERM_STOP_REQ\n", SDT_MOD_NAME, sdt));
		rtn = sdt_suerm_stop_req(q, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		sdt->state = oldstate;
	return (rtn);
}

STATIC int
sdt_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sdt_t *sdt = SDT_PRIV(q);
	ulong oldstate = sdt->t.state;
	/*
	   Fast Path 
	 */
	if ((prim = *((ulong *) mp->b_rptr)) == T_UNITDATA_IND) {
		printd(("%s: %p: T_UNITDATA_IND [%d] <-\n", SDT_MOD_NAME, sdt,
			msgdsize(mp->b_cont)));
		if ((rtn = t_unitdata_ind(q, mp)) < 0)
			sdt->t.state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case T_CONN_IND:
		printd(("%s: %p: T_CONN_IND <-\n", SDT_MOD_NAME, sdt));
		rtn = t_conn_ind(q, mp);
		break;
	case T_CONN_CON:
		printd(("%s: %p: T_CONN_CON <-\n", SDT_MOD_NAME, sdt));
		rtn = t_conn_con(q, mp);
		break;
	case T_DISCON_IND:
		printd(("%s: %p: T_DISCON_IND <-\n", SDT_MOD_NAME, sdt));
		rtn = t_discon_ind(q, mp);
		break;
	case T_DATA_IND:
		printd(("%s: %p: T_DATA_IND <-\n", SDT_MOD_NAME, sdt));
		rtn = t_data_ind(q, mp);
		break;
	case T_EXDATA_IND:
		printd(("%s: %p: T_EXDATA_IND <-\n", SDT_MOD_NAME, sdt));
		rtn = t_exdata_ind(q, mp);
		break;
	case T_INFO_ACK:
		printd(("%s: %p: T_INFO_ACK <-\n", SDT_MOD_NAME, sdt));
		rtn = t_info_ack(q, mp);
		break;
	case T_BIND_ACK:
		printd(("%s: %p: T_BIND_ACK <-\n", SDT_MOD_NAME, sdt));
		rtn = t_bind_ack(q, mp);
		break;
	case T_ERROR_ACK:
		printd(("%s: %p: T_ERROR_ACK <-\n", SDT_MOD_NAME, sdt));
		rtn = t_error_ack(q, mp);
		break;
	case T_OK_ACK:
		printd(("%s: %p: T_OK_ACK <-\n", SDT_MOD_NAME, sdt));
		rtn = t_ok_ack(q, mp);
		break;
	case T_UNITDATA_IND:
		printd(("%s: %p: T_UNITDATA_IND [%d] <-\n", SDT_MOD_NAME, sdt,
			msgdsize(mp->b_cont)));
		rtn = t_unitdata_ind(q, mp);
		break;
	case T_UDERROR_IND:
		printd(("%s: %p: T_UDERROR_IND <-\n", SDT_MOD_NAME, sdt));
		rtn = t_uderror_ind(q, mp);
		break;
	case T_OPTMGMT_ACK:
		printd(("%s: %p: T_OPTMGMT_ACK <-\n", SDT_MOD_NAME, sdt));
		rtn = t_optmgmt_ack(q, mp);
		break;
	case T_ORDREL_IND:
		printd(("%s: %p: T_ORDREL_IND <-\n", SDT_MOD_NAME, sdt));
		rtn = t_ordrel_ind(q, mp);
		break;
	case T_OPTDATA_IND:
		printd(("%s: %p: T_OPTDATA_IND <-\n", SDT_MOD_NAME, sdt));
		rtn = t_optdata_ind(q, mp);
		break;
	case T_ADDR_ACK:
		printd(("%s: %p: T_ADDR_ACK <-\n", SDT_MOD_NAME, sdt));
		rtn = t_addr_ack(q, mp);
		break;
	default:
		swerr();
		rtn = (-EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		sdt->t.state = oldstate;
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_w_data(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	(void) sdt;
	printd(("%s: %p: -> M_DATA [%d]\n", SDT_MOD_NAME, sdt, msgdsize(mp)));
	return sdt_send_data(q, mp);
}
STATIC int
sdt_r_data(queue_t *q, mblk_t *mp)
{
	sdt_t *sdt = SDT_PRIV(q);
	(void) sdt;
	printd(("%s: %p: M_DATA [%d] <-\n", SDT_MOD_NAME, sdt, msgdsize(mp)));
	return sdt_recv_data(q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
sdt_r_prim(queue_t *q, mblk_t *mp)
{
	/*
	   Fast Path 
	 */
	if (mp->b_datap->db_type == M_DATA)
		return sdt_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sdt_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sdt_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
sdt_w_prim(queue_t *q, mblk_t *mp)
{
	/*
	   Fast Path 
	 */
	if (mp->b_datap->db_type == M_DATA)
		return sdt_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sdt_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sdt_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sdt_w_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}

STATIC int
sdt_rput(queue_t *q, mblk_t *mp)
{
	return (int) ss7_oput(q, mp);
}
STATIC int
sdt_rsrv(queue_t *q)
{
	return (int) ss7_osrv(q);
}
STATIC int
sdt_wput(queue_t *q, mblk_t *mp)
{
	return (int) ss7_iput(q, mp);
}
STATIC int
sdt_wsrv(queue_t *q)
{
	return (int) ss7_isrv(q);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *sdt_priv_cachep = NULL;
STATIC int
sdt_init_caches(void)
{
	if (!sdt_priv_cachep
	    && !(sdt_priv_cachep =
		 kmem_cache_create("sdt_priv_cachep", sizeof(sdt_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate sdt_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized module private structure cace\n", SDT_MOD_NAME));
	return (0);
}
STATIC void
sdt_term_caches(void)
{
	if (sdt_priv_cachep) {
		if (kmem_cache_destroy(sdt_priv_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sdt_priv_cachep", __FUNCTION__);
		else
			printd(("%s: destroyed sdt_priv_cachep\n", SDT_MOD_NAME));
	}
	return;
}
STATIC sdt_t *
sdt_get(sdt_t * sdt)
{
	if (sdt)
		atomic_inc(&sdt->refcnt);
	return (sdt);
}
STATIC void
sdt_put(sdt_t * sdt)
{
	if (sdt) {
		if (atomic_dec_and_test(&sdt->refcnt)) {
			kmem_cache_free(sdt_priv_cachep, sdt);
			printd(("%s: freed module private structure\n", SDT_MOD_NAME));
		}
	}
}
STATIC sdt_t *
sdt_alloc_priv(queue_t *q, sdt_t ** sdtp, ushort cmajor, ushort cminor)
{
	sdt_t *sdt;
	if ((sdt = kmem_cache_alloc(sdt_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated module private structure\n", SDT_MOD_NAME));
		bzero(sdt, sizeof(*sdt));
		sdt_get(sdt);	/* first get */
		sdt->cmajor = cmajor;
		sdt->cminor = cminor;
		lis_spin_lock_init(&sdt->qlock, "sdt-queue-lock");
		(sdt->oq = RD(q))->q_ptr = sdt_get(sdt);
		(sdt->iq = WR(q))->q_ptr = sdt_get(sdt);
		sdt->o_prim = &sdt_r_prim;
		sdt->i_prim = &sdt_w_prim;
		sdt->o_wakeup = &sdt_rx_wakeup;
		sdt->i_wakeup = &sdt_tx_wakeup;
		sdt->version = 1;
		sdt->state = LMI_UNUSABLE;
		sdt->style = LMI_STYLE1;
		lis_spin_lock_init(&sdt->lock, "sdt-priv-lock");
		if ((sdt->next = *sdtp))
			sdt->next->prev = &sdt->next;
		sdt->prev = sdtp;
		*sdtp = sdt_get(sdt);
		printd(("%s: linked module private structure\n", SDT_MOD_NAME));
		/*
		   TPI configuration defaults 
		 */
		sdt->t.state = TS_NOSTATES;
		sdt->t.serv_type = T_CLTS;
		sdt->t.sequence = 0;
		sdt->t.pdu_size = 272 + 1 + 3;
		sdt->t.opt_size = -1UL;
		sdt->t.add_size = sizeof(struct sockaddr);
		/*
		   LMI configuration defaults 
		 */
		sdt->option.pvar = SS7_PVAR_ITUT_96;
		sdt->option.popt = 0;
		/*
		   DEV initialization 
		 */
		bufq_init
		    /*
		       SDL configuration defaults 
		     */
		    sdt->sdl.config.ifflags = 0;
		sdt->sdl.config.iftype = SDL_TYPE_V35;
		sdt->sdl.config.ifrate = 56000;
		sdt->sdl.config.ifgtype = SDL_GTYPE_NONE;
		sdt->sdl.config.ifgrate = 0;
		sdt->sdl.config.ifmode = SDL_MODE_DTE;
		sdt->sdl.config.ifgmode = SDL_GMODE_NONE;
		sdt->sdl.config.ifgcrc = SDL_GCRC_NONE;
		sdt->sdl.config.ifclock = SDL_CLOCK_DPLL;
		sdt->sdl.config.ifcoding = SDL_CODING_NRZI;
		sdt->sdl.config.ifframing = SDL_FRAMING_NONE;
		sdt->sdl.config.blksize = 0;
		sdt->sdl.config.ifleads = 0;
		sdt->sdl.config.ifalarms = 0;
		sdt->sdl.config.ifrxlevel = 0;
		sdt->sdl.config.iftxlevel = 0;
		/*
		   sdt->sdt.confing.ifsyncsrc[i] = 0; 
		 */
		sdt->sdl.timestamp = jiffies;
		sdt->sdl.tickbytes = sdt->sdl.config.ifrate / HZ / 8;
		sdt->sdl.bytecount = 0;
		/*
		   rest zero 
		 */
		/*
		   SDT configuration defaults 
		 */
		bufq_init(&sdt->sdt.tb);
		sdt->sdt.config.Tin = 4;
		sdt->sdt.config.Tie = 1;
		sdt->sdt.config.T = 64;
		sdt->sdt.config.D = 256;
		sdt->sdt.config.t8 = 100 * HZ / 1000;
		sdt->sdt.config.Te = 577169;
		sdt->sdt.config.De = 9308000;
		sdt->sdt.config.Ue = 144292000;
		sdt->sdt.config.N = 16;
		sdt->sdt.config.m = 272;
		sdt->sdt.config.b = 8;
		sdt->sdt.config.f = SDT_FLAGS_ONE;
		printd(("%s: setting module private structure defaults\n", SDT_MOD_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", SDT_MOD_NAME));
	return (sdt);
}
STATIC void
sdt_free_priv(queue_t *q)
{
	sdt_t *sdt = SDT_PRIV(q);
	psw_t flags;
	ensure(sdt, return);
	lis_spin_lock_irqsave(&sdt->lock, &flags);
	{
		if (sdt->sdt.rx_cmp)
			freemsg(xchg(&sdt->sdt.rx_cmp, NULL));
		if (sdt->sdt.tx_cmp)
			freemsg(xchg(&sdt->sdt.tx_cmp, NULL));
		if (sdt->rbuf)
			freemsg(xchg(&sdt->rbuf, NULL));
		if (sdt->xbuf)
			freemsg(xchg(&sdt->xbuf, NULL));
		sdt_unbufcall(q);
		sdt_timer_stop(q, t8);
		sdt_timer_stop(q, t9);
		bufq_purge(&sdt->sdt.tb);
		if ((*sdt->prev = sdt->next))
			sdt->next->prev = sdt->prev;
		sdt->next = NULL;
		sdt->prev = NULL;
		sdt_put(sdt);
		sdt->oq->q_ptr = NULL;
		sdt_put(sdt);
		sdt->iq->q_ptr = NULL;
		sdt_put(sdt);
	}
	lis_spin_unlock_irqrestore(&sdt->lock, &flags);
	printd(("%s: unlinked module private structure\n", SDT_MOD_NAME));
	if (atomic_read(sdt->refcnt)) {
		assure(!atomic_read(sdt->refcnt));
		printd(("%s: WARNING: sdt->refcnt = %d\n", SDT_MOD_NAME, atomic_read(sdt->refcnt)));
	}
	sdt_put(sdt);		/* final put */
	return;
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
sdt_t *sdt_list = NULL;
STATIC int
sdt_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;		/* for now */
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		if (!(sdt_alloc_priv(q, &sdt_list, getmajor(*devp), getminor(*devp)))) {
			MOD_DEC_USE_COUNT;
			return ENOMEM;
		}
		/*
		   generate immediate information request 
		 */
		t_info_req(q);
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return EIO;
}
STATIC int
sdt_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sdt_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization (For unregistered driver.)
 *
 *  =========================================================================
 */
STATIC int sdt_initialized = 0;
STATIC void
sdt_init(void)
{
	unless(sdt_initialized, return);
	cmn_err(CE_NOTE, SS7IP_BANNER);	/* console splash */
	if ((sdt_initialized = sdt_init_caches()))
		return;
	if (!(sdt_initialized = lis_register_strmod(&sdt_info, SDT_MOD_NAME))) {
		sdt_term_caches();
		cmn_err(CE_WARN, "%s: couldn't register module", SDT_MOD_NAME);
		return;
	}
	sdt_initialized = 1;
	return;
}
STATIC void
sdt_terminate(void)
{
	int err;
	ensure(sdt_initialized, return);
	if ((err = lis_unregister_strmod(&sdt_info)))
		cmn_err(CE_PANIC, "%s: couldn't unregister module", SDT_MOD_NAME);
	sdt_initialized = 0;
	sdt_term_caches();
	return;
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int
init_module(void)
{
	sdt_init();
	if (sdt_initialized < 0)
		return sdt_initialized;
	return (0);
}

void
cleanup_module(void)
{
	sdt_terminate();
	return;
}
