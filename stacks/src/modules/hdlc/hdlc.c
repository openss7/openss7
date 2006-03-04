/*****************************************************************************

 @(#) $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/03/04 13:00:04 $

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

 Last Modified $Date: 2006/03/04 13:00:04 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: hdlc.c,v $
 Revision 0.9.2.12  2006/03/04 13:00:04  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/03/04 13:00:04 $"

static char const ident[] =
    "$RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/03/04 13:00:04 $";

/*
 *  This is an HDLC (High-Level Data Link Control) module which
 *  provides HDLC capabilities when pushed over a Channel Interface (CHI)
 *  stream.  The module provides the Communications Device Interface (CDI).
 *  This supports both STYLE1 and STYLE2 providers.  Attach addresses are
 *  passed through to the Channel Interface (CHI) stream below permitting this
 *  module to be pushed immediately over a freshly opened Channel Interface
 *  (CHI) stream.
 *
 *  The HDLC is intended to be linked under the Q920 multiplexing driver to be
 *  accessed by LAPD and LAPF modules that are subsequently pushed under IDSN
 *  and Frame Relay drivers.
 */
#include <sys/os7/compat.h>

#include <sys/cdi.h>
#include <sys/cdi_hdlc.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/hdlc_ioctl.h>

#define HDLC_DESCRIP	"ISO 3309/4335 HDLC: (High-Level Data Link Control) STREAMS MODULE."
#define HDLC_REVISION	"LfS $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/03/04 13:00:04 $"
#define HDLC_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define HDLC_DEVICE	"Supports OpenSS7 Channel Drivers."
#define HDLC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define HDLC_LICENSE	"GPL"
#define HDLC_BANNER	HDLC_DESCRIP	"\n" \
			HDLC_REVISION	"\n" \
			HDLC_COPYRIGHT	"\n" \
			HDLC_DEVICE	"\n" \
			HDLC_CONTACT	"\n"
#define HDLC_SPLASH	HDLC_DESCRIP	"\n" \
			HDLC_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(HDLC_CONTACT);
MODULE_DESCRIPTION(HDLC_DESCRIP);
MODULE_SUPPORTED_DEVICE(HDLC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(HDLC_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-hdlc");
#endif
#endif				/* LINUX */

#ifdef LFS
#define HDLC_MOD_ID		CONFIG_STREAMS_HDLC_MODID
#define HDLC_MOD_NAME		CONFIG_STREAMS_HDLC_NAME
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define MOD_ID		HDLC_MOD_ID
#define MOD_NAME	HDLC_MOD_NAME

STATIC struct module_info cd_winfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME,		/* Module name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC struct module_info cd_rinfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME,		/* Module name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC int cd_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int cd_close(queue_t *, int, cred_t *);

STATIC struct qinit cd_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:cd_open,		/* Each open */
	qi_qclose:cd_close,		/* Last close */
	qi_minfo:&cd_rinfo,		/* Information */
};

STATIC struct qinit cd_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&cd_winfo,		/* Information */
};

STATIC struct streamtab hdlcinfo = {
	st_rdinit:&cd_rinit,		/* Upper read queue */
	st_wrinit:&cd_winit,		/* Upper write queue */
};

STATIC int cd_r_prim(queue_t *q, mblk_t *mp);
STATIC int cd_w_prim(queue_t *q, mblk_t *mp);

STATIC void cd_wakeup(queue_t *q);

/*
 *  ========================================================================
 *
 *  Private structure
 *
 *  ========================================================================
 */
struct hdlc_path {
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
};

struct cd {
	STR_DECLARATION (struct cd);	/* stream declaration */
	struct hdlc_path tx;		/* transmit path variables */
	struct hdlc_path rx;		/* receive path variables */
	uint rx_octets;			/* no received octets */
	ulong ppa;			/* physical point of attachment */
	ulong sigs;			/* modem/alarm signals */
	lmi_option_t option;		/* HDLC protocol options */
	struct {
		cd_info_ack_t cd;	/* CD information */
		CH_info_ack_t ch;	/* CH information */
	} info;
	CH_parms_circuit_t parm;	/* CH circuit parameters */
	hdlc_timers_t timers;		/* HDLC timers */
	hdlc_statem_t statem;		/* HDLC state machine variables */
	hdlc_config_t config;		/* HDLC configuration options */
	hdlc_notify_t notify;		/* HDLC notification options */
	hdlc_stats_t stats;		/* HDLC statistics */
	hdlc_stats_t stamp;		/* HDLC statistics timestamps */
	hdlc_stats_t statsp;		/* HDLC statistics periods */
};

#define PRIV(__q) ((struct cd *)(__q)->q_ptr)

#define HDLC_F_DISCONNECTED	0x00000001	/* autonomous disconnect */

#ifndef CD_UNINIT
#define CD_UNINIT	(-1)
#endif

/*
 *  State Flags
 */
#define CDF_UNATTACHED		(1<<CD_UNATTACHED     )	/* No PPA attached */
#define CDF_UNUSABLE		(1<<CD_UNUSABLE       )	/* PPA cannot be used */
#define CDF_DISABLED		(1<<CD_DISABLED       )	/* PPA attached */
#define CDF_ENABLE_PENDING	(1<<CD_ENABLE_PENDING )	/* Waiting ack of enable req */
#define CDF_ENABLED		(1<<CD_ENABLED        )	/* Awaiting use */
#define CDF_READ_ACTIVE		(1<<CD_READ_ACTIVE    )	/* Input section enabled; disabled after
							   data arrives */
#define CDF_INPUT_ALLOWED	(1<<CD_INPUT_ALLOWED  )	/* Input section permanently enabled */
#define CDF_DISABLE_PENDING	(1<<CD_DISABLE_PENDING)	/* Waiting ack of disable req */
#define CDF_OUTPUT_ACTIVE	(1<<CD_OUTPUT_ACTIVE  )	/* Output section active only */
#define	CDF_XRAY		(1<<CD_XRAY           )	/* Xray-ing another ppa */

/*
 *  -------------------------------------------------------------------------
 *
 *  Private structure allocation and deallocation
 *
 *  -------------------------------------------------------------------------
 */
STATIC kmem_cache_t *hdlc_priv_cachep = NULL;
STATIC int
hdlc_init_caches(void)
{
	if (!hdlc_priv_cachep
	    && !(hdlc_priv_cachep =
		 kmem_cache_create("hdlc_priv_cachep", sizeof(struct cd), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate hdlc_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	}
	printd(("%s: Allocated/selected private structure cache\n", MOD_NAME));
	return (0);
}
STATIC int
hdlc_term_caches(void)
{
	if (hdlc_priv_cachep) {
		if (kmem_cache_destroy(hdlc_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy hdlc_priv_cachep.", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("cd: destroyed hdlc_priv_cachep\n"));
	}
	return (0);
}
STATIC struct cd *
cd_get(struct cd *cd)
{
	atomic_inc(&cd->refcnt);
	return (cd);
}
STATIC void
cd_put(struct cd *cd)
{
	if (atomic_dec_and_test(&cd->refcnt)) {
		kmem_cache_free(hdlc_priv_cachep, cd);
		printd(("%s: %p: freed cd private structure\n", MOD_NAME, cd));
	}
	return;
}
STATIC void
hdlc_free_priv(queue_t *q)
{
	struct cd *cd = PRIV(q);
	psw_t flags;
	ensure(cd, return);
	spin_lock_irqsave(&cd->lock, flags);
	{
		ss7_unbufcall((str_t *) cd);
		if (cd->tx.msg && cd->tx.msg != cd->tx.cmp)
			freemsg(xchg(&cd->tx.msg, NULL));
		if (cd->tx.cmp)
			freemsg(xchg(&cd->tx.cmp, NULL));
		if (cd->rx.msg)
			freemsg(xchg(&cd->rx.msg, NULL));
		if (cd->rx.nxt)
			freemsg(xchg(&cd->rx.nxt, NULL));
		if (cd->rx.cmp)
			freemsg(xchg(&cd->rx.cmp, NULL));
		if ((*cd->prev = cd->next))
			cd->next->prev = cd->prev;
		cd->next = NULL;
		cd->prev = &cd->next;
		cd_put(cd);
		flushq(xchg(&cd->oq, NULL), FLUSHALL);
		cd_put(cd);
		flushq(xchg(&cd->iq, NULL), FLUSHALL);
		cd_put(cd);
	}
	spin_unlock_irqrestore(&cd->lock, flags);
	cd_put(cd);		/* final put */
	return;
}
STATIC struct cd *
hdlc_alloc_priv(queue_t *q, struct cd **hpp, dev_t *devp, cred_t *crp)
{
	struct cd *cd;
	if ((cd = kmem_cache_alloc(hdlc_priv_cachep, SLAB_ATOMIC))) {
		printd(("cd: allocated module private structure\n"));
		bzero(cd, sizeof(*cd));
		cd->priv_put = &cd_put;	/* set put method */
		cd_get(cd);	/* first get */
		cd->u.dev.cmajor = getmajor(*devp);
		cd->u.dev.cminor = getminor(*devp);
		cd->cred = *crp;
		(cd->oq = RD(q))->q_ptr = cd_get(cd);
		(cd->iq = WR(q))->q_ptr = cd_get(cd);
		spin_lock_init(&cd->qlock);	/* "cd-queue-lock" */
		cd->o_prim = &cd_r_prim;
		cd->i_prim = &cd_w_prim;
		cd->o_wakeup = &cd_wakeup;
		cd->i_wakeup = &cd_wakeup;
		cd->i_version = 1;
		cd->i_state = CD_UNINIT;
		cd->i_style = CD_STYLE2;
		spin_lock_init(&cd->qlock);	/* "cd-priv_lock" */
		if ((cd->next = *hpp))
			cd->next->prev = &cd->next;
		cd->prev = hpp;
		*hpp = cd_get(cd);
		printd(("%s: %p: linked module private structure\n", MOD_NAME, cd));
		printd(("%s: %p: setting module private structure defaults\n", MOD_NAME, cd));
		cd->info.cd.cd_primitive = CD_INFO_ACK;
		cd->info.cd.cd_state = CD_UNINIT;
		cd->info.cd.cd_max_sdu = 260;
		cd->info.cd.cd_min_sdu = 3;
		cd->info.cd.cd_class = CD_HDLC;
		cd->info.cd.cd_duplex = CD_FULLDUPLEX;
		cd->info.cd.cd_features = CD_CANREAD | CD_AUTOALLOW;
		cd->info.cd.cd_addr_length = 0;
		cd->info.cd.cd_ppa_style = CD_STYLE2;	/* for now */
		/* 
		   assume default circuit info */
		cd->info.ch.ch_primitive = CH_INFO_ACK;
		cd->info.ch.ch_addr_length = 0;
		cd->info.ch.ch_addr_offset = 0;
		cd->info.ch.ch_parm_length = 0;
		cd->info.ch.ch_parm_offset = 0;
		cd->info.ch.ch_prov_flags = 0;
		cd->info.ch.ch_style = CH_STYLE2;
		cd->info.ch.ch_version = 1;
		cd->info.ch.ch_state = CHS_UNINIT;
		/* 
		   assume default circuit parameters */
		cd->parm.cp_type = CH_PARMS_CIRCUIT;
		cd->parm.cp_block_size = 64;	/* bits */
		cd->parm.cp_encoding = CH_ENCODING_NONE;
		cd->parm.cp_sample_size = 8;	/* bits */
		cd->parm.cp_rate = CH_RATE_8000;	/* samples per second */
		cd->parm.cp_tx_channels = 1;
		cd->parm.cp_rx_channels = 1;
		cd->parm.cp_opt_flags = CH_PARM_OPT_CLRCH;
		todo(("set module defaults\n"));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", MOD_NAME));
	return (cd);
}

/*
 *  ========================================================================
 *
 *  FUNCTIONS
 *
 *  ========================================================================
 */

/*
 *  CD State Changes
 */
#ifdef _DEBUG
STATIC INLINE const char *
cd_state_name(ulong state)
{
	switch (state) {
	case CD_UNINIT:
		return ("CD_UNINIT");
	case CD_UNATTACHED:
		return ("CD_UNATTACHED");
	case CD_UNUSABLE:
		return ("CD_UNUSABLE");
	case CD_DISABLED:
		return ("CD_DISABLED");
	case CD_ENABLE_PENDING:
		return ("CD_ENABLE_PENDING");
	case CD_ENABLED:
		return ("CD_ENABLED");
	case CD_READ_ACTIVE:
		return ("CD_READ_ACTIVE");
	case CD_INPUT_ALLOWED:
		return ("CD_INPUT_ALLOWED");
	case CD_DISABLE_PENDING:
		return ("CD_DISABLE_PENDING");
	case CD_OUTPUT_ACTIVE:
		return ("CD_OUTPUT_ACTIVE");
	case CD_XRAY:
		return ("CD_XRAY");
	default:
		swerr();
		return ("CD_????");
	}
}
#endif
STATIC INLINE ulong
cd_get_state(struct cd *cd)
{
	return cd->i_state;
}
STATIC INLINE ulong
cd_tst_state(struct cd *cd, ulong flags)
{
	return ((1 << cd_get_state(cd)) & flags);
}
STATIC INLINE ulong
cd_set_state(struct cd *cd, ulong newstate)
{
	ulong oldstate = cd->i_state;
	(void) oldstate;
	cd->info.cd.cd_state = cd->i_state = newstate;
	printd(("%s: %p: %s <- %s\n", MOD_NAME, cd, cd_state_name(newstate),
		cd_state_name(oldstate)));
	return (newstate);
}

/*
 *  CH State Changes
 */
#ifdef _DEBUG
STATIC INLINE const char *
ch_state_name(ulong state)
{
	switch (state) {
	case CHS_UNINIT:
		return ("CHS_UNINIT");
	case CHS_UNUSABLE:
		return ("CHS_UNUSABLE");
	case CHS_DETACHED:
		return ("CHS_DETACHED");
	case CHS_WACK_AREQ:
		return ("CHS_WACK_AREQ");
	case CHS_WACK_UREQ:
		return ("CHS_ATTACHED");
	case CHS_WACK_EREQ:
		return ("CHS_WACK_EREQ");
	case CHS_WCON_EREQ:
		return ("CHS_WCON_EREQ");
	case CHS_WACK_RREQ:
		return ("CHS_WACK_RREQ");
	case CHS_WCON_RREQ:
		return ("CHS_WCON_RREQ");
	case CHS_ENABLED:
		return ("CHS_ENABLED");
	case CHS_WACK_CREQ:
		return ("CHS_WACK_CREQ");
	case CHS_WCON_CREQ:
		return ("CHS_WCON_CREQ");
	case CHS_WACK_DREQ:
		return ("CHS_WACK_DREQ");
	case CHS_WCON_DREQ:
		return ("CHS_WCON_DREQ");
	case CHS_CONNECTED:
		return ("CHS_CONNECTED");
	default:
		swerr();
		return ("CHS_????");
	}
}
#endif
STATIC INLINE ulong
ch_get_state(struct cd *cd)
{
	return cd->state;
}
STATIC INLINE ulong
ch_set_state(struct cd *cd, ulong newstate)
{
	ulong oldstate = cd->state;
	(void) oldstate;
	cd->info.ch.ch_state = cd->state = newstate;
	printd(("%s: %p: %s <- %s\n", MOD_NAME, cd, ch_state_name(newstate),
		ch_state_name(oldstate)));
	return (newstate);
}

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
 *  Primitives sent upstream
 *
 *  ------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC INLINE int
m_error(queue_t *q, int err)
{
	struct cd *cd = PRIV(q);
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		cd_set_state(cd, CD_UNUSABLE);
		ch_set_state(cd, CHS_UNUSABLE);
		printd(("%s: %p: <- M_ERROR\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  M_HANGUP
 *  -----------------------------------
 */
STATIC INLINE int
m_hangup(queue_t *q, int err)
{
	struct cd *cd = PRIV(q);
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		cd_set_state(cd, CD_UNUSABLE);
		ch_set_state(cd, CHS_UNUSABLE);
		printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_INFO_ACK         0x01 - Information acknowledgement
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_info_ack(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		*p = cd->info.cd;
		printd(("%s: %p: <- CD_INFO_ACK\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_OK_ACK           0x06 - Success acknowledgement
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_ok_ack(queue_t *q, struct cd *cd, ulong prim)
{
	mblk_t *mp;
	cd_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_OK_ACK;
		p->cd_correct_primitive = prim;
		switch (prim) {
		case CD_ABORT_OUTPUT_REQ:
			if (cd_get_state(cd) == CD_OUTPUT_ACTIVE)
				p->cd_state = cd_set_state(cd, CD_ENABLED);
			else
				p->cd_state = cd_get_state(cd);
			break;
		case CD_ALLOW_INPUT_REQ:
			p->cd_state = cd_set_state(cd, CD_INPUT_ALLOWED);
			break;
		case CD_ATTACH_REQ:
			p->cd_state = cd_set_state(cd, CD_DISABLED);
			break;
		case CD_DETACH_REQ:
			p->cd_state = cd_set_state(cd, CD_UNATTACHED);
			break;
		case CD_HALT_INPUT_REQ:
			p->cd_state = cd_set_state(cd, CD_ENABLED);
			break;
		case CD_MODEM_SIG_REQ:
			p->cd_state = cd_set_state(cd, CD_ENABLED);
			break;
		case CD_MUX_NAME_REQ:
			p->cd_state = cd_set_state(cd, CD_ENABLED);
			break;
		default:
			p->cd_state = cd_get_state(cd);
			swerr();
			break;
		}
		printd(("%s: %p: <- CD_OK_ACK\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ERROR_ACK        0x07 - Error acknowledgement
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_error_ack(queue_t *q, struct cd *cd, ulong prim, ulong error, ulong reason)
{
	mblk_t *mp;
	cd_error_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_ERROR_ACK;
		p->cd_error_primitive = prim;
		p->cd_errno = error;
		p->cd_explanation = reason;
		if (error == CD_FATALERR)
			cd_set_state(cd, CD_UNUSABLE);
		else
			cd_set_state(cd, cd->i_oldstate);
		p->cd_state = cd_get_state(cd);
		printd(("%s: %p: <- CD_ERROR_ACK\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ENABLE_CON       0x08 - Enable confirmation
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_enable_con(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_ENABLE_CON;
		p->cd_state = cd_set_state(cd, CD_ENABLED);
		printd(("%s: %p: <- CD_ENABLE_CON\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_DISABLE_CON      0x09 - Disable confirmation
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_disable_con(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_DISABLE_CON;
		p->cd_state = cd_set_state(cd, CD_DISABLED);
		printd(("%s: %p: <- CD_DISABLE_CON\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ERROR_IND        0x0a - Error indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_error_ind(queue_t *q, struct cd *cd, ulong error, ulong reason, ulong state, mblk_t *dp)
{
	mblk_t *mp;
	cd_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_ERROR_IND;
		p->cd_state = cd_set_state(cd, state);
		p->cd_errno = error;
		p->cd_explanation = reason;
		mp->b_cont = dp;
		printd(("%s: %p: <- CD_ERROR_IND\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_UNITDATA_ACK     0x0f - Data send acknowledgement
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_uintdata_ack(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_unitdata_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_UNITDATA_ACK;
		p->cd_state = cd_get_state(cd);
		printd(("%s: %p: <- CD_UNITDATA_ACK\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_UNITDATA_IND     0x10 - Data receive indication
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_unitdata_ind(queue_t *q, struct cd *cd, mblk_t *dp)
{
	mblk_t *mp;
	cd_unitdata_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_UNITDATA_IND;
		p->cd_state = cd_get_state(cd);
		p->cd_src_addr_length = 0;
		p->cd_src_addr_offset = 0;
		p->cd_addr_type = CD_IMPLICIT;
		p->cd_priority = 0;
		p->cd_dest_addr_length = 0;
		p->cd_dest_addr_offset = 0;
		mp->b_cont = dp;
		printd(("%s: %p: <- CD_UNITDATA_IND\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_BAD_FRAME_IND    0x14 - frame w/error (Gcom extension)
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_bad_frame_ind(queue_t *q, struct cd *cd, ulong error, mblk_t *dp)
{
	mblk_t *mp;
	cd_bad_frame_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_BAD_FRAME_IND;
		p->cd_state = cd_get_state(cd);
		p->cd_error = error;
		mp->b_cont = dp;
		printd(("%s: %p: <- CD_BAD_FRAME_IND\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_MODEM_SIG_IND    0x16 - Report modem signal state (Gcom)
 *  -----------------------------------------------------------
 */
STATIC INLINE int
cd_modem_sig_ind(queue_t *q, struct cd *cd, ulong sigs)
{
	mblk_t *mp;
	cd_modem_sig_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 1;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_MODEM_SIG_IND;
		p->cd_sigs = sigs;
		printd(("%s: %p: <- CD_MODEM_SIG_IND\n", MOD_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  PRIMITIVES Sent Downstream
 *
 *  ------------------------------------------------------------------------
 */

/*
 *  CH_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_info_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	struct CH_info_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_INFO_REQ;
		printd(("%s: %p: CH_INFO_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_optmgmt_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	struct CH_optmgmt_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_OPTMGMT_REQ;
		printd(("%s: %p: CH_OPTMGMT_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_ATTACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_attach_req(queue_t *q, struct cd *cd, size_t add_len, caddr_t add_ptr, ulong flags)
{
	mblk_t *mp;
	struct CH_attach_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_ATTACH_REQ;
		p->ch_addr_length = add_len;
		p->ch_addr_offset = add_len ? sizeof(*p) : 0;
		p->ch_flags = flags;
		if (add_len) {
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		ch_set_state(cd, CHS_WACK_AREQ);
		printd(("%s: %p: CH_ATTACH_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_ENABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_enable_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	struct CH_enable_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_ENABLE_REQ;
		ch_set_state(cd, CHS_WACK_EREQ);
		printd(("%s: %p: CH_ENABLE_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_CONNECT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_connect_req(queue_t *q, struct cd *cd, ulong flags)
{
	mblk_t *mp;
	struct CH_connect_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_CONNECT_REQ;
		p->ch_conn_flags = flags;
		p->ch_slot = 0;
		ch_set_state(cd, CHS_WACK_CREQ);
		printd(("%s: %p: CH_CONNECT_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_data_req(queue_t *q, struct cd *cd, mblk_t *dp)
{
	mblk_t *mp;
	struct CH_data_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_DATA_REQ;
		p->ch_slot = 0;
		mp->b_cont = dp;
		printd(("%s: %p: CH_DATA_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_disconnect_req(queue_t *q, struct cd *cd, ulong flags)
{
	mblk_t *mp;
	struct CH_disconnect_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_DISCONNECT_REQ;
		p->ch_conn_flags = flags;
		p->ch_slot = 0;
		ch_set_state(cd, CHS_WACK_DREQ);
		printd(("%s: %p: CH_DISCONNECT_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DISABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_disable_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	struct CH_disable_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_DISABLE_REQ;
		ch_set_state(cd, CHS_WACK_RREQ);
		printd(("%s: %p: CH_DISABLE_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DETACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
ch_detach_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	struct CH_detach_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->ch_primitive = CH_DETACH_REQ;
		ch_set_state(cd, CHS_WACK_UREQ);
		printd(("%s: %p: CH_DETACH_REQ ->\n", MOD_NAME, cd));
		putnext(cd->iq, mp);
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
 *  ========================================================================
 *
 *  Soft-HDLC
 *
 *  ========================================================================
 */
#define HDLC_TX_STATES	5
#define HDLC_RX_STATES	14

#define HDLC_TX_BUFSIZE	PAGE_SIZE
#define HDLC_RX_BUFSIZE	PAGE_SIZE

#define HDLC_CRC_TABLE_LENGTH	512
#define HDLC_TX_TABLE_LENGTH	(2* HDLC_TX_STATES * 256)
#define HDLC_RX_TABLE_LENGTH	(2* HDLC_RX_STATES * 256)

typedef struct tx_entry {
	uint bit_string:10 __attribute__ ((packed));		/* the output string */
	uint bit_length:4 __attribute__ ((packed));		/* length in excess of 8 bits of output string */
	uint state:3 __attribute__ ((packed));			/* new state */
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
tx_index8(uint j, uint k)
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
 *  REV
 *  -----------------------------------
 *  Reverse bits.  (There must be a better way to do this...)
 */
STATIC INLINE uchar
cd_rev(uchar byte)
{
	int i;
	uchar output = 0;
	for (i = 0; i < 8; i++) {
		output <<= 1;
		if (byte & 0x01)
			output |= 1;
		byte >>= 1;
	}
	return (output);
}

/*
 *  TX BUFFER
 *  ----------------------------------------
 *  Pick up another TX buffer from the queue or a repeat frame.
 */
STATIC INLINE mblk_t *
cd_tx_buffer(queue_t *q, struct cd *cd)
{
	mblk_t *mp, *dp = NULL;
	for (mp = cd->iq->q_first; mp; mp = mp->b_next) {
		switch (mp->b_datap->db_type) {
		case M_DATA:
			dp = mp;
			rmvq(cd->iq, dp);
			break;
		case M_PROTO:
			if (*((ulong *) mp->b_rptr) != CD_UNITDATA_REQ)
				continue;
			dp = mp->b_cont;
			rmvq(cd->iq, mp);
			freeb(mp);
			break;
		default:
			continue;
		}
		break;
	}
	return (dp);
}

/*
 *  TX BITSTUFF
 *  ----------------------------------------
 *  Bitstuff an octet and shift residue for output.
 */
STATIC INLINE void
cd_tx_bitstuff(struct hdlc_path *tx, unsigned char byte)
{
	tx_entry_t *t = tx_index8(tx->state, byte);
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
 *  Generate blocks for transmission.  We generate entire transmit blocks.  If
 *  there are not sufficient messages to build the transmit blocks we will
 *  idle flags.
 */
STATIC int
cd_tx_block(queue_t *q, struct cd *cd)
{
	mblk_t *bp;
	struct hdlc_path *tx = &cd->tx;
	struct hdlc_stats *stats = &cd->stats;
	int bits = cd->parm.cp_sample_size;
	int blks = (cd->parm.cp_block_size + 7) >> 3;
	int mask = (1 << bits) - 1;
	if (!(bp = ss7_allocb(q, blks, BPRI_MED)))
		goto enobufs;	/* bufcall will bring us back */
	if (tx->mode == TX_MODE_IDLE || tx->mode == TX_MODE_FLAG) {
		if (!tx->nxt) {
		      next_message:
			if (tx->msg && tx->msg != tx->cmp)
				freemsg(tx->msg);
			if ((tx->msg = tx->nxt = cd_tx_buffer(q, cd)))
				tx->mode = TX_MODE_BOF;
		}
	}
	/* 
	   check if transmission block complete */
	while (bp->b_wptr < bp->b_rptr + blks) {
		/* 
		   drain residue bits, if necessary */
		if (tx->rbits >= bits) {
		      drain_rbits:
			/* 
			   drain residue bits */
			*bp->b_wptr++ = cd_rev(tx->residue & mask);
			tx->residue >>= bits;
			tx->rbits -= bits;
			continue;
		}
		switch (tx->mode) {
		case TX_MODE_IDLE:
			/* 
			   mark idle */
			tx->residue |= 0xff << tx->rbits;
			tx->rbits += 8;
			goto drain_rbits;
		case TX_MODE_FLAG:
			/* 
			   idle flags */
			tx->residue |= 0x7e << tx->rbits;
			tx->rbits += 8;
			goto drain_rbits;
		case TX_MODE_BOF:
			/* 
			   add opening flag (also closing flag) */
			switch (cd->config.f) {
			case HDLC_FLAGS_ONE:
				tx->residue |= 0x7e << tx->rbits;
				tx->rbits += 8;
				break;
			case HDLC_FLAGS_SHARED:
				tx->residue |= 0x3f7e << tx->rbits;
				tx->rbits += 15;
				break;
			case HDLC_FLAGS_TWO:
				tx->residue |= 0x7e7e << tx->rbits;
				tx->rbits += 16;
				break;
			default:
			case HDLC_FLAGS_THREE:
				tx->residue |= 0x7e7e7e << tx->rbits;
				tx->rbits += 24;
				break;
			}
			tx->state = 0;
			tx->bcc = 0x00ff;
			tx->mode = TX_MODE_MOF;
			goto drain_rbits;
		case TX_MODE_MOF:	/* transmit frame bytes */
			if (tx->nxt->b_rptr < tx->nxt->b_wptr || (tx->nxt = tx->nxt->b_cont)) {
				/* 
				   continuing in message */
				uint byte = *(tx->nxt->b_rptr)++;
				tx->bcc = (tx->bcc >> 8) ^ bc_table[(tx->bcc ^ byte) & 0x00ff];
				cd_tx_bitstuff(tx, byte);
				stats->tx_bytes++;
			} else {
				/* 
				   finished message: add 1st bcc byte */
				cd_tx_bitstuff(tx, tx->bcc & 0x00ff);
				tx->mode = TX_MODE_BCC;
			}
			goto drain_rbits;
		case TX_MODE_BCC:
			/* 
			   add 2nd bcc byte */
			cd_tx_bitstuff(tx, tx->bcc >> 8);
			stats->tx_frames++;
			tx->mode = TX_MODE_FLAG;
			goto next_message;
		}
		swerr();
	}
	putnext(q, bp);
	return (QR_DONE);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  RX LINKB
 *  ----------------------------------------
 *  Link a buffer to existing message or create new message with buffer.
 */
STATIC INLINE void
cd_rx_linkb(struct hdlc_path *rx)
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
STATIC INLINE int
cd_rx_block(queue_t *q, struct cd *cd, mblk_t *dp)
{
	struct hdlc_path *rx = &cd->rx;
	struct hdlc_stats *stats = &cd->stats;
	int bits = cd->parm.cp_sample_size;
#if 0
	int N = (cd->info.cd.cd_min_sdu + 5) << 1;
#endif
	int err = 0;
	while (dp->b_rptr < dp->b_wptr) {
		rx_entry_t *r;
		if (bits == 8)
			r = rx_index8(rx->state, cd_rev(*dp->b_rptr++));
		else
			r = rx_index7(rx->state, cd_rev(*dp->b_rptr++));
		rx->state = r->state;
		switch (rx->mode) {
		case RX_MODE_MOF:
			if (!r->sync && r->bit_length) {
				rx->residue |= r->bit_string << rx->rbits;
				rx->rbits += r->bit_length;
			}
			if (r->flag)
				goto end_of_frame;
			if (r->hunt || r->idle)
				goto aborted;
			while (rx->rbits > 16) {
				if (rx->nxt && rx->nxt->b_wptr >= rx->nxt->b_datap->db_lim)
					cd_rx_linkb(rx);
				if (!rx->nxt && !(rx->nxt = ss7_allocb(q, FASTBUF, BPRI_HI)))
					goto enobufs;
				rx->bcc = (rx->bcc >> 8)
				    ^ bc_table[(rx->bcc ^ rx->residue) & 0x00ff];
				*(rx->nxt->b_wptr)++ = rx->residue;
				stats->rx_bytes++;
				rx->residue >>= 8;
				rx->rbits -= 8;
				rx->bytes++;
				if (rx->bytes > cd->info.cd.cd_max_sdu)
					goto frame_too_long;
			}
			break;
		      end_of_frame:
			if (rx->rbits != 16)
				goto residue_error;
			if ((~rx->bcc & 0xffff) != (rx->residue & 0xffff))
				goto crc_error;
			if (rx->bytes < cd->info.cd.cd_min_sdu)
				goto frame_too_short;
			cd_rx_linkb(rx);
			if (!canputnext(cd->oq))
				goto buffer_overflow;
			if ((err = cd_unitdata_ind(q, cd, rx->msg)) != QR_ABSORBED)
				goto error;
			rx->msg = NULL;
			stats->rx_frames++;
		      new_frame:
			rx->mode = RX_MODE_SYNC;
			rx->residue = 0;
			rx->rbits = 0;
			rx->bytes = 0;
			rx->bcc = 0xffff;
			if (!r->sync)
				break;
		      begin_frame:
			if (!r->bit_length)
				break;
			rx->mode = RX_MODE_MOF;
			rx->residue = r->bit_string;
			rx->rbits = r->bit_length;
			rx->bytes = 0;
			rx->bcc = 0x00ff;
			break;
		      frame_too_long:
			stats->rx_frame_too_long++;
			stats->rx_frame_errors++;
			err = CD_FORMAT;
			goto abort_frame;
		      buffer_overflow:
			stats->rx_buffer_overflows++;
			err = CD_OVERRUN;
			goto abort_frame;
		      aborted:
			stats->rx_aborts++;
			stats->rx_frame_errors++;
			err = CD_HDLC_ABORT;
			goto abort_frame;
		      frame_too_short:
			stats->rx_frame_too_short++;
			stats->rx_frame_errors++;
			err = CD_TOOSHORT;
			goto abort_frame;
		      crc_error:
			stats->rx_crc_errors++;
			err = CD_CRCERR;
			goto abort_frame;
		      residue_error:
			stats->rx_residue_errors++;
			stats->rx_frame_errors++;
			err = CD_INCOMPLETE;
			goto abort_frame;
		      abort_frame:
			if (rx->nxt)
				cd_rx_linkb(rx);
			if (cd_error_ind(q, cd, CD_BADFRAME, err, cd_get_state(cd), rx->msg))
				if (rx->msg)
					freemsg(rx->msg);
			rx->msg = NULL;
			stats->rx_frames_in_error++;
			if (r->flag)
				goto new_frame;
			rx->mode = RX_MODE_HUNT;
			stats->rx_sync_transitions++;
			cd->rx_octets = 0;
			break;
		case RX_MODE_SYNC:
			if (!r->hunt && !r->idle)
				goto begin_frame;
			rx->mode = RX_MODE_HUNT;
			stats->rx_sync_transitions++;
			cd->rx_octets = 0;
			break;
		case RX_MODE_HUNT:
			if (!r->flag) {
#if 0
				cd->rx_octets++;
				while (cd->rx_octets >= N) {
					if (cd_error_ind
					    (q, cd, CD_BADFRAME, 0, cd_get_state(cd), NULL))
						break;
					stats->rx_sus_in_error++;
					cd->rx_octets -= N;
				}
				stats->rx_bits_octet_counted += 8;
#endif
				break;
			}
			stats->rx_sync_transitions++;
			goto new_frame;
		default:
			swerr();
			err = EIO;
			goto abort_frame;
		}
	}
	return (QR_ABSORBED);
      error:
	return (err);
      enobufs:
	return (-ENOBUFS);
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
 *  bits for the transmitted bitstream.  It is the reponsiblity of the SDL
 *  driver to perform 8-bit to 7-bit conversion to DS0A.
 */
STATIC void
tx_table_generate(void)
{
	int j, k;
	for (j = 0; j < HDLC_TX_STATES; j++)
		for (k = 0; k < 256; k++)
			*tx_index8(j, k) = tx_table_value(j, k);
}

/*
 *  RX (Received) Tables:
 *  -----------------------------------
 *  There is one RX table for 8 bit (octet) values.  The table has 256 entries
 *  and is used to perform, for one sample, zero deletion, abort detection,
 *  flag detection and residue calculation on the received bitstream.  The SDL
 *  driver is responsible for performing 7-bit to 8-bit conversion before
 *  delivering bits to the HDLC.
 */
STATIC void
rx_table_generate7(void)
{
	int j, k;
	for (j = 0; j < HDLC_RX_STATES; j++)
		for (k = 0; k < 256; k++)
			*rx_index7(j, k) = rx_table_value7(j, k);
}
STATIC void
rx_table_generate8(void)
{
	int j, k;
	for (j = 0; j < HDLC_RX_STATES; j++)
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
hdlc_free_tables(void)
{
	if (bc_table) {
		free_pages((unsigned long) xchg(&bc_table, NULL), xchg(&bc_order, 0));
		printd(("cd: freed BC table kernel pages\n"));
	}
	if (tx_table) {
		free_pages((unsigned long) xchg(&tx_table, NULL), xchg(&tx_order, 0));
		printd(("cd: freed Tx table kernel pages\n"));
	}
	if (rx_table) {
		free_pages((unsigned long) xchg(&rx_table, NULL), xchg(&rx_order, 0));
		printd(("cd: freed Rx table kernel pages\n"));
	}
	return (0);
}
STATIC int
hdlc_init_tables(void)
{
	size_t length;
	if (!bc_table) {
		length = HDLC_CRC_TABLE_LENGTH * sizeof(bc_entry_t);
		for (bc_order = 0; PAGE_SIZE << bc_order < length; bc_order++) ;
		if (!(bc_table = (bc_entry_t *) __get_free_pages(GFP_KERNEL, bc_order)))
			goto enomem;
		printd(("cd: allocated BC table size %u kernel pages\n", 1 << bc_order));
		bc_table_generate();
		printd(("cd: generated BC table\n"));
	}
	if (!tx_table) {
		length = HDLC_TX_TABLE_LENGTH * sizeof(tx_entry_t);
		for (tx_order = 0; PAGE_SIZE << tx_order < length; tx_order++) ;
		if (!(tx_table = (tx_entry_t *) __get_free_pages(GFP_KERNEL, tx_order)))
			goto enomem;
		printd(("cd: allocated Tx table size %u kernel pages\n", 1 << tx_order));
		tx_table_generate();
		printd(("cd: generated 8-bit Tx table\n"));
	}
	if (!rx_table) {
		length = 2 * (HDLC_RX_TABLE_LENGTH * sizeof(rx_entry_t));
		for (rx_order = 0; PAGE_SIZE << rx_order < length; rx_order++) ;
		if (!(rx_table = (rx_entry_t *) __get_free_pages(GFP_KERNEL, rx_order)))
			goto enomem;
		printd(("cd: allocated Rx table size %u kernel pages\n", 1 << rx_order));
		rx_table_generate8();
		printd(("cd: generated 8-bit Rx table\n"));
		rx_table7 = (rx_entry_t *) (((uint8_t *) rx_table) + (PAGE_SIZE << (rx_order - 1)));
		rx_table_generate7();
		printd(("cd: generated 7-bit Rx table\n"));
	}
	return (0);
      enomem:
	hdlc_free_tables();
	return (-ENOMEM);
}

/*
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */

/*
 *  WAKEUP
 *  -----------------------------------
 *  This is called before the queue service routine unlocks the queue whenever
 *  we have received or transmitted message blocks.  cd_tx_block will pull
 *  data from the queue as necessary.  Thus the module takes its transmit data
 *  timing from its receive data timing.
 */
STATIC void
cd_wakeup(queue_t *q)
{
	struct cd *cd = PRIV(q);
	if (!cd_tst_state(cd, CDF_ENABLED | CDF_OUTPUT_ACTIVE | CDF_READ_ACTIVE))
		return;
	if (ch_get_state(cd) != CHS_CONNECTED)
		return;
	cd_tx_block(q, cd);
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CD User -> CD Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
cd_write(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	if (!cd_tst_state(cd, CDF_ENABLED | CDF_OUTPUT_ACTIVE | CDF_READ_ACTIVE))
		goto discard;
	if (ch_get_state(cd) != CHS_CONNECTED)
		goto discard;
	/* 
	   let cd_wakeup pull from the queue */
	return (-EAGAIN);
      discard:
	return (QR_DONE);	/* silent discard */
}

/*
 *  CD_INFO_REQ         0x00 - Information request
 *  -----------------------------------------------------------
 */
STATIC int
cd_info_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_info_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return cd_info_ack(q, cd);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_ATTACH_REQ       0x02 - Attach a PPA
 *  -----------------------------------------------------------
 */
STATIC int
cd_attach_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_attach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_UNATTACHED)
		goto outstate;
	if (cd->info.cd.cd_ppa_style != CD_STYLE2)
		goto notsupp;
	cd->ppa = p->cd_ppa;
	/* 
	   issue attach request to channel and wait for response */
	return ch_attach_req(q, cd, sizeof(cd->ppa), (caddr_t) &cd->ppa, 0);
      notsupp:
	return cd_error_ack(q, cd, p->cd_primitive, CD_NOTSUPP, EOPNOTSUPP);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, EPROTO);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, EMSGSIZE);
}

/*
 *  CD_DETACH_REQ       0x03 - Detach a PPA
 *  -----------------------------------------------------------
 */
STATIC int
cd_detach_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_detach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	/* 
	   issue detach request to channel and wait for response */
	return ch_detach_req(q, cd);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_ENABLE_REQ       0x04 - Prepare a device
 *  -----------------------------------------------------------
 */
STATIC int
cd_enable_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_enable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	/* 
	   issue enable request to channel and wait for response */
	return ch_enable_req(q, cd);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_DISABLE_REQ      0x05 - Disable a device
 *  -----------------------------------------------------------
 */
STATIC int
cd_disable_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_disable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_ENABLED)
		goto outstate;
	/* 
	   issue disconnect request to channel and wait for response */
	return ch_disconnect_req(q, cd, CHF_BOTH_DIR);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_ALLOW_INPUT_REQ  0x0b - Allow input
 *  -----------------------------------------------------------
 */
STATIC int
cd_allow_input_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_allow_input_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (cd_get_state(cd)) {
	case CD_READ_ACTIVE:
	case CD_OUTPUT_ACTIVE:
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
		break;
	default:
		goto outstate;
	}
	return cd_ok_ack(q, cd, p->cd_primitive);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_READ_REQ         0x0c - Wait-for-input request
 *  -----------------------------------------------------------
 */
STATIC int
cd_read_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_read_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_ENABLED)
		goto outstate;
	return cd_error_ack(q, cd, p->cd_primitive, CD_NOTSUPP, 0);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_UNITDATA_REQ     0x0d - Data send request
 *  -----------------------------------------------------------
 *  We strip off the CD_UNITDATA_REQ header and place it back on the queue as
 *  just M_DATA.  The M_DATA and outstanding CD_UNITDATA_REQ message blocks
 *  will be pulled off of the queue by cd_tx_block().
 */
STATIC int
cd_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->cd_addr_type != CD_IMPLICIT)
		goto badaddrtype;
	if (p->cd_dest_addr_length != 0)
		goto badaddress;
	if (!cd_tst_state(cd, CDF_ENABLED | CDF_OUTPUT_ACTIVE | CDF_READ_ACTIVE))
		goto outstate;
	if (ch_get_state(cd) != CHS_CONNECTED)
		goto outstate;
	return (QR_STRIP);	/* strip down to data only */
      badaddress:
	return cd_error_ack(q, cd, p->cd_primitive, CD_BADADDRESS, 0);
      badaddrtype:
	return cd_error_ack(q, cd, p->cd_primitive, CD_BADADDRTYPE, 0);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_WRITE_READ_REQ   0x0e - Write/read request
 *  -----------------------------------------------------------
 */
STATIC int
cd_write_read_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_write_read_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_ENABLED)
		goto outstate;
	return cd_error_ack(q, cd, p->cd_primitive, CD_NOTSUPP, 0);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_HALT_INPUT_REQ   0x11 - Halt input
 *  -----------------------------------------------------------
 */
STATIC int
cd_halt_input_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_halt_input_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_INPUT_ALLOWED)
		goto outstate;
	return cd_error_ack(q, cd, p->cd_primitive, CD_NOTSUPP, 0);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_ABORT_OUTPUT_REQ 0x12 - Abort output
 *  -----------------------------------------------------------
 */
STATIC int
cd_abort_output_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_abort_output_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_OUTPUT_ACTIVE)
		goto outstate;
	return cd_error_ack(q, cd, p->cd_primitive, CD_NOTSUPP, 0);
      outstate:
	return cd_error_ack(q, cd, p->cd_primitive, CD_OUTSTATE, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_MUX_NAME_REQ     0x13 - get mux name (Gcom)
 *  -----------------------------------------------------------
 */
STATIC int
cd_mux_name_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
#if 0
	cd_mux_name_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return cd_error_ack(q, cd, p->cd_primitive, CD_NOTSUPP, 0);
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
#endif
	return cd_error_ack(q, cd, *(ulong *) mp->b_rptr, CD_NOTSUPP, 0);
}

/*
 *  CD_MODEM_SIG_REQ    0x15 - Assert modem signals (Gcom)
 *  -----------------------------------------------------------
 */
STATIC int
cd_modem_sig_req(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_modem_sig_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
#if 0
	return cd_ok_ack(q, cd, p->cd_primitive);
#else
	return cd_error_ack(q, cd, p->cd_primitive, CD_NOTSUPP, 0);
#endif
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  CD_MODEM_SIG_POLL   0x17 - requests a CD_MODEM_SIG_IND (Gcom)
 *  -----------------------------------------------------------
 */
STATIC int
cd_modem_sig_poll(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	cd_modem_sig_poll_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
#if 0
	return cd_modem_sig_ind(q, cd, 0);
#else
	return cd_error_ack(q, cd, p->cd_primitive, CD_NOTSUPP, 0);
#endif
      badprim:
	return cd_error_ack(q, cd, p->cd_primitive, CD_PROTOSHORT, 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CH-Provider -> CH-User Primitives
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
ch_read(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	if (!cd_tst_state(cd, CDF_INPUT_ALLOWED | CDF_READ_ACTIVE))
		goto discard;
	if (ch_get_state(cd) != CHS_CONNECTED)
		goto discard;
	return cd_rx_block(q, cd, mp);
      discard:
	return (QR_DONE);
}

/*
 *  CH_INFO_ACK
 *  -----------------------------------
 */
STATIC int
ch_info_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_info_ack *p = (typeof(p)) mp->b_rptr;
	ulong cp_type;
	ulong oldstate = ch_get_state(cd);
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	cd->info.ch = *p;
	if (ch_get_state(cd) == CHS_UNINIT) {
		switch (p->ch_state) {
		case CHS_DETACHED:
			cd->info.cd.cd_ppa_style = cd->i_style = CD_STYLE2;
			break;
		case CHS_ATTACHED:
			/* 
			   treat already attached channels as CD_STYLE1 */
			cd->info.cd.cd_ppa_style = cd->i_style = CD_STYLE1;
			break;
		default:
			goto eio;
		}
		ch_set_state(cd, p->ch_state);
	}
	if (ch_get_state(cd) == CHS_ATTACHED) {
		if (!p->ch_parm_length)
			goto eio;
		if (mp->b_wptr > mp->b_rptr + p->ch_parm_offset + p->ch_parm_length)
			goto eio;
		if (p->ch_parm_length < sizeof(cp_type))
			goto eio;
		bcopy(mp->b_rptr + p->ch_parm_offset, &cp_type, sizeof(cp_type));
		if (cp_type != CH_PARMS_CIRCUIT)
			goto eio;
		if (p->ch_parm_length < sizeof(cd->parm))
			goto eio;
		bcopy(mp->b_rptr + p->ch_parm_offset, &cd->parm, sizeof(cd->parm));
		/* 
		   check circuit parameters */
		if (cd->parm.cp_block_size == 0 || cd->parm.cp_block_size > 2048)
			goto eio;
		if (cd->parm.cp_encoding != CH_ENCODING_NONE)
			goto eio;
		if (cd->parm.cp_sample_size != 7 && cd->parm.cp_sample_size != 8)
			goto eio;
		if (cd->parm.cp_tx_channels != 1 || cd->parm.cp_rx_channels != 1)
			goto eio;
	}
	if (oldstate == CHS_ATTACHED)
		return cd_ok_ack(q, cd, CD_ATTACH_REQ);
	return (QR_DONE);
      eio:
	switch (oldstate) {
	case CHS_UNINIT:
		ch_set_state(cd, CHS_UNUSABLE);
		return (-EIO);
	case CHS_ATTACHED:
		return cd_error_ack(q, cd, CD_ATTACH_REQ, CD_FATALERR, EIO);
	}
	swerr();
	return (-EIO);
}

/*
 *  CH_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC int
ch_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	return (QR_DONE);
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_OK_ACK
 *  -----------------------------------
 */
STATIC int
ch_ok_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_ok_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	switch (ch_get_state(cd)) {
	case CHS_WACK_AREQ:
		ch_set_state(cd, CHS_ATTACHED);
		/* 
		   request information concerning attached circuit */
		return ch_info_req(q, cd);
	case CHS_WACK_UREQ:
		ch_set_state(cd, CHS_DETACHED);
		return cd_ok_ack(q, cd, CD_DETACH_REQ);
	case CHS_WACK_EREQ:
		ch_set_state(cd, CHS_WCON_EREQ);
		return (QR_DONE);
	case CHS_WACK_RREQ:
		ch_set_state(cd, CHS_WCON_RREQ);
		return (QR_DONE);
	case CHS_WACK_CREQ:
		ch_set_state(cd, CHS_WCON_CREQ);
		return (QR_DONE);
	case CHS_WACK_DREQ:
		ch_set_state(cd, CHS_WCON_DREQ);
		return (QR_DONE);
	}
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_ERROR_ACK
 *  -----------------------------------
 */
STATIC int
ch_error_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_error_ack *p = (typeof(p)) mp->b_rptr;
	ulong error = CD_EVENT, reason = 0;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	switch (p->ch_error_type) {
	case CHSYSERR:
		error = CD_SYSERR;
		reason = p->ch_unix_error;
		break;
	case CHBADADDR:
		error = CD_BADADDRESS;
		break;
	case CHOUTSTATE:
		error = CD_OUTSTATE;
		break;
	case CHBADFLAG:
		error = CD_BADPRIM;
		break;
	case CHBADPRIM:
		error = CD_PROTOSHORT;
		break;
	case CHNOTSUPP:
		error = CD_NOTSUPP;
		break;
	default:
	case CHBADOPT:
	case CHBADPARM:
	case CHBADPARMTYPE:
		error = CD_EVENT;
		reason = p->ch_error_type;
		break;
	}
	switch (ch_get_state(cd)) {
	case CHS_WACK_AREQ:
		ch_set_state(cd, CHS_DETACHED);
		return cd_error_ack(q, cd, CD_ATTACH_REQ, CD_EVENT, 0);
	case CHS_WACK_UREQ:
		ch_set_state(cd, CHS_ATTACHED);
		return cd_error_ack(q, cd, CD_DETACH_REQ, CD_EVENT, 0);
	case CHS_WACK_EREQ:
		ch_set_state(cd, CHS_ATTACHED);
		return cd_error_ack(q, cd, CD_ENABLE_REQ, CD_EVENT, 0);
	case CHS_WACK_CREQ:
		return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
	case CHS_WACK_DREQ:
		ch_set_state(cd, CHS_CONNECTED);
		return cd_error_ack(q, cd, CD_DISABLE_REQ, CD_EVENT, 0);
	case CHS_WACK_RREQ:
		return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
	}
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_ENABLE_CON
 *  -----------------------------------
 */
STATIC int
ch_enable_con(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_enable_con *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	if (ch_get_state(cd) != CHS_WCON_EREQ)
		goto eio;
	/* 
	   issue connect request to channel and wait for response */
	return ch_connect_req(q, cd, CHF_BOTH_DIR);
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_CONNECT_CON
 *  -----------------------------------
 */
STATIC int
ch_connect_con(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_connect_con *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	if (ch_get_state(cd) != CHS_WCON_CREQ)
		goto eio;
	ch_set_state(cd, CHS_CONNECTED);
	return cd_enable_con(q, cd);
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_DATA_IND
 *  -----------------------------------
 */
STATIC int
ch_data_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_data_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	if (!mp->b_cont)
		goto eio;
	return (QR_STRIP);
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_DISCONNECT_IND
 *  -----------------------------------
 */
STATIC int
ch_disconnect_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_disconnect_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	if (ch_get_state(cd) != CHS_CONNECTED)
		goto eio;
	cd->flags |= HDLC_F_DISCONNECTED;
	return ch_disable_req(q, cd);
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_DISCONNECT_CON
 *  -----------------------------------
 */
STATIC int
ch_disconnect_con(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_disconnect_con *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	if (ch_get_state(cd) != CHS_WCON_DREQ)
		goto eio;
	/* 
	   issue disable request to channel and wait for response */
	return ch_disable_req(q, cd);
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_DISABLE_IND
 *  -----------------------------------
 */
STATIC int
ch_disable_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_disable_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	if (ch_get_state(cd) != CHS_ENABLED)
		goto eio;
	ch_set_state(cd, CHS_ATTACHED);
	return cd_error_ind(q, cd, CD_DISC, 0, CD_DISABLED, NULL);
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

/*
 *  CH_DISABLE_CON
 *  -----------------------------------
 */
STATIC int
ch_disable_con(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_disable_con *p = (typeof(p)) mp->b_rptr;
	int rtn;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	if (ch_get_state(cd) != CHS_WCON_RREQ)
		goto eio;
	if (cd->flags & HDLC_F_DISCONNECTED)
		rtn = cd_error_ind(q, cd, CD_DISC, 0, CD_DISABLED, NULL);
	else
		rtn = cd_disable_con(q, cd);
	if (rtn >= 0) {
		ch_set_state(cd, CHS_ATTACHED);
		cd->flags &= ~HDLC_F_DISCONNECTED;
	}
	return (rtn);
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

#ifndef CD_YEL
#define CD_YEL (CD_RI << 1)
#define CD_BLU (CD_RI << 2)
#define CD_RED (CD_RI << 3)
#endif

/*
 *  CH_EVENT_IND
 *  -----------------------------------
 */
STATIC int
ch_event_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	struct CH_event_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	switch (p->ch_event) {
	case CH_EVT_DCD_ASSERT:
		cd->sigs |= CD_DCD;
		break;
	case CH_EVT_DCD_DEASSERT:
		cd->sigs &= ~CD_DCD;
		break;
	case CH_EVT_DSR_ASSERT:
		cd->sigs |= CD_DSR;
		break;
	case CH_EVT_DSR_DEASSERT:
		cd->sigs &= ~CD_DSR;
		break;
	case CH_EVT_DTR_ASSERT:
		cd->sigs |= CD_DTR;
		break;
	case CH_EVT_DTR_DEASSERT:
		cd->sigs &= ~CD_DTR;
		break;
	case CH_EVT_RTS_ASSERT:
		cd->sigs |= CD_RTS;
		break;
	case CH_EVT_RTS_DEASSERT:
		cd->sigs &= ~CD_RTS;
		break;
	case CH_EVT_CTS_ASSERT:
		cd->sigs |= CD_CTS;
		break;
	case CH_EVT_CTS_DEASSERT:
		cd->sigs &= ~CD_CTS;
		break;
	case CH_EVT_RI_ASSERT:
		cd->sigs |= CD_RI;
		break;
	case CH_EVT_RI_DEASSERT:
		cd->sigs &= ~CD_RI;
		break;
	case CH_EVT_YEL_ALARM:
		cd->sigs |= CD_YEL;
		break;
	case CH_EVT_BLU_ALARM:
		cd->sigs |= CD_BLU;
		break;
	case CH_EVT_RED_ALARM:
		cd->sigs |= CD_RED;
		break;
	case CH_EVT_NO_ALARM:
		cd->sigs &= ~(CD_YEL | CD_BLU | CD_RED);
		break;
	default:
		goto discard;
	}
	return cd_modem_sig_ind(q, cd, cd->sigs);
      discard:
	swerr();
	return (-EIO);		/* ignore */
      eio:
	swerr();
	return cd_error_ind(q, cd, CD_FATALERR, 0, CD_UNUSABLE, NULL);
}

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
lmi_test_config(struct cd *cd, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_commit_config(struct cd *cd, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			*arg = cd->option;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->option = *arg;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			arg->version = cd->i_version;
			arg->style = cd->i_style;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->i_version = arg->version;
			cd->i_style = arg->style;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_test_config(cd, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_commit_config(cd, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			arg->state = cd->state;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->state = LMI_UNUSABLE;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		hdlc_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&cd->lock, flags);
		{
			*arg = cd->statsp;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		hdlc_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->statsp = *arg;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		lmi_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&cd->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccstats(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	int ret = 0;
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&cd->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&cd->lock, flags);
	return (ret);
}
STATIC int
lmi_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&cd->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&cd->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&cd->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  HDLC IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_test_config(struct cd *cd, hdlc_config_t * arg)
{
	int ret = 0;
	psw_t flags;
	spin_lock_irqsave(&cd->lock, flags);
	do {
#if 0
		if (!arg->t8)
			arg->t8 = cd->config.t8;
		if (!arg->Tin)
			arg->Tin = cd->config.Tin;
		if (!arg->Tie)
			arg->Tie = cd->config.Tie;
		if (!arg->T)
			arg->T = cd->config.T;
		if (!arg->D)
			arg->D = cd->config.D;
		if (!arg->Te)
			arg->Te = cd->config.Te;
		if (!arg->De)
			arg->De = cd->config.De;
		if (!arg->Ue)
			arg->Ue = cd->config.Ue;
		if (!arg->N)
			arg->N = cd->config.N;
#endif
		if (!arg->m)
			arg->m = cd->config.m;
		if (!arg->b)
			arg->b = cd->config.b;
		else if (arg->b != cd->config.b) {
			ret = -EINVAL;
			break;
		}
	} while (0);
	spin_unlock_irqrestore(&cd->lock, flags);
	return (ret);
}
STATIC int
cd_commit_config(struct cd *cd, hdlc_config_t * arg)
{
	psw_t flags;
	spin_lock_irqsave(&cd->lock, flags);
	{
		cd_test_config(cd, arg);
		cd->config = *arg;
	}
	spin_unlock_irqrestore(&cd->lock, flags);
	return (0);
}
STATIC int
cd_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			*arg = cd->option;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->option = *arg;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			*arg = cd->config;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->config = *arg;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		hdlc_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return cd_test_config(cd, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		hdlc_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return cd_commit_config(cd, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			*arg = cd->statem;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_ioccmreset(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	(void) cd;
	(void) mp;
	fixme(("%s: Master reset\n", MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
cd_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			*arg = cd->statsp;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->statsp = *arg;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			*arg = cd->stats;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_ioccstats(queue_t *q, mblk_t *mp)
{
	psw_t flags;
	struct cd *cd = PRIV(q);
	(void) mp;
	spin_lock_irqsave(&cd->lock, flags);
	{
		bzero(&cd->stats, sizeof(cd->stats));
	}
	spin_unlock_irqrestore(&cd->lock, flags);
	return (0);
}
STATIC int
cd_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			*arg = cd->notify;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->notify = *arg;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct cd *cd = PRIV(q);
		psw_t flags;
		hdlc_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&cd->lock, flags);
		{
			cd->notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&cd->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
cd_ioccmgmt(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	int ret = 0;
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&cd->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&cd->lock, flags);
	return (ret);
}

/*
 *  ========================================================================
 *
 *  STREAMS Message Handling
 *
 *  ========================================================================
 *
 *  M_IOCTL Handling
 *  -----------------------------------------------------------------------
 */
STATIC int
cd_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
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
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, cd, nr));
			ret = (-EINVAL);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, cd, nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
	case LMI_IOC_MAGIC:
	{
		if (count < size || cd_get_state(cd) == CD_UNUSABLE) {
			ret = (-EINVAL);
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
			ptrace(("%s: %p: ERROR: Unsupported HDLC ioctl %d\n", MOD_NAME, cd, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case HDLC_IOC_MAGIC:
	{
		if (count < size || cd_get_state(cd) == CD_UNUSABLE) {
			ret = (-EINVAL);
			break;
		}
		switch (nr) {
		case _IOC_NR(HDLC_IOCGOPTIONS):	/* lmi_option_t */
			ret = cd_iocgoptions(q, mp);
			break;
		case _IOC_NR(HDLC_IOCSOPTIONS):	/* lmi_option_t */
			ret = cd_iocsoptions(q, mp);
			break;
		case _IOC_NR(HDLC_IOCGCONFIG):	/* hdlc_config_t */
			ret = cd_iocgconfig(q, mp);
			break;
		case _IOC_NR(HDLC_IOCSCONFIG):	/* hdlc_config_t */
			ret = cd_iocsconfig(q, mp);
			break;
		case _IOC_NR(HDLC_IOCTCONFIG):	/* hdlc_config_t */
			ret = cd_ioctconfig(q, mp);
			break;
		case _IOC_NR(HDLC_IOCCCONFIG):	/* hdlc_config_t */
			ret = cd_ioccconfig(q, mp);
			break;
		case _IOC_NR(HDLC_IOCGSTATEM):	/* hdlc_statem_t */
			ret = cd_iocgstatem(q, mp);
			break;
		case _IOC_NR(HDLC_IOCCMRESET):	/* hdlc_statem_t */
			ret = cd_ioccmreset(q, mp);
			break;
		case _IOC_NR(HDLC_IOCGSTATSP):	/* lmi_sta_t */
			ret = cd_iocgstatsp(q, mp);
			break;
		case _IOC_NR(HDLC_IOCSSTATSP):	/* lmi_sta_t */
			ret = cd_iocsstatsp(q, mp);
			break;
		case _IOC_NR(HDLC_IOCGSTATS):	/* hdlc_stats_t */
			ret = cd_iocgstats(q, mp);
			break;
		case _IOC_NR(HDLC_IOCCSTATS):	/* hdlc_stats_t */
			ret = cd_ioccstats(q, mp);
			break;
		case _IOC_NR(HDLC_IOCGNOTIFY):	/* hdlc_notify_t */
			ret = cd_iocgnotify(q, mp);
			break;
		case _IOC_NR(HDLC_IOCSNOTIFY):	/* hdlc_notify_t */
			ret = cd_iocsnotify(q, mp);
			break;
		case _IOC_NR(HDLC_IOCCNOTIFY):	/* hdlc_notify_t */
			ret = cd_ioccnotify(q, mp);
			break;
		case _IOC_NR(HDLC_IOCCMGMT):	/* */
			ret = cd_ioccmgmt(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported HDLC ioctl %d\n", MOD_NAME, nr));
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
 *  -----------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -----------------------------------------------------------------------
 */
/*
 *  Primitives from User to HDLC.
 *  -----------------------------------
 */
STATIC int
cd_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct cd *cd = PRIV(q);
	cd->i_oldstate = cd_get_state(cd);
	if ((prim = *(ulong *) mp->b_rptr) == CD_UNITDATA_REQ) {
		printd(("%s: %p: -> CD_UNITDATA_REQ [%d]\n", MOD_NAME, cd, msgdsize(mp->b_cont)));
		if ((rtn = cd_unitdata_req(q, mp)) < 0)
			cd_set_state(cd, cd->i_oldstate);
		return (rtn);
	}
	switch (prim) {
	case CD_INFO_REQ:
		printd(("%s: %p: -> CD_INFO_REQ\n", MOD_NAME, cd));
		rtn = cd_info_req(q, mp);
		break;
	case CD_ATTACH_REQ:
		printd(("%s: %p: -> CD_ATTACH_REQ\n", MOD_NAME, cd));
		rtn = cd_attach_req(q, mp);
		break;
	case CD_DETACH_REQ:
		printd(("%s: %p: -> CD_DETACH_REQ\n", MOD_NAME, cd));
		rtn = cd_detach_req(q, mp);
		break;
	case CD_ENABLE_REQ:
		printd(("%s: %p: -> CD_ENABLE_REQ\n", MOD_NAME, cd));
		rtn = cd_enable_req(q, mp);
		break;
	case CD_DISABLE_REQ:
		printd(("%s: %p: -> CD_DISABLE_REQ\n", MOD_NAME, cd));
		rtn = cd_disable_req(q, mp);
		break;
	case CD_ALLOW_INPUT_REQ:
		printd(("%s: %p: -> CD_ALLOW_INPUT_REQ\n", MOD_NAME, cd));
		rtn = cd_allow_input_req(q, mp);
		break;
	case CD_READ_REQ:
		printd(("%s: %p: -> CD_READ_REQ\n", MOD_NAME, cd));
		rtn = cd_read_req(q, mp);
		break;
	case CD_UNITDATA_REQ:
		printd(("%s: %p: -> CD_UNITDATA_REQ [%d]\n", MOD_NAME, cd, msgdsize(mp->b_cont)));
		rtn = cd_unitdata_req(q, mp);
		break;
	case CD_WRITE_READ_REQ:
		printd(("%s: %p: -> CD_WRITE_READ_REQ\n", MOD_NAME, cd));
		rtn = cd_write_read_req(q, mp);
		break;
	case CD_HALT_INPUT_REQ:
		printd(("%s: %p: -> CD_HALT_INPUT_REQ\n", MOD_NAME, cd));
		rtn = cd_halt_input_req(q, mp);
		break;
	case CD_ABORT_OUTPUT_REQ:
		printd(("%s: %p: -> CD_ABORT_OUTPUT_REQ\n", MOD_NAME, cd));
		rtn = cd_abort_output_req(q, mp);
		break;
	case CD_MUX_NAME_REQ:
		printd(("%s: %p: -> CD_MUX_NAME_REQ\n", MOD_NAME, cd));
		rtn = cd_mux_name_req(q, mp);
		break;
	case CD_MODEM_SIG_REQ:
		printd(("%s: %p: -> CD_MODEM_SIG_REQ\n", MOD_NAME, cd));
		rtn = cd_modem_sig_req(q, mp);
		break;
	case CD_MODEM_SIG_POLL:
		printd(("%s: %p: -> CD_MODEM_SIG_POLL\n", MOD_NAME, cd));
		rtn = cd_modem_sig_poll(q, mp);
		break;
	default:
		printd(("%s: %p -> CD_????\n", MOD_NAME, cd));
		rtn = cd_error_ack(q, cd, prim, CD_BADPRIM, 0);
		break;
	}
	if (rtn < 0)
		cd_set_state(cd, cd->i_oldstate);
	return (rtn);
}

/*
 *  Primitives from SDL to HDLC.
 *  -----------------------------------
 */
STATIC int
cd_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct cd *cd = PRIV(q);
	ulong oldstate = ch_get_state(cd);
	/* 
	   Fast Path */
	if ((prim = *(ulong *) mp->b_rptr) == CH_DATA_IND) {
		printd(("%s: %p: CH_DATA_IND [%d] <-\n", MOD_NAME, cd, msgdsize(mp->b_cont)));
		if ((rtn = ch_data_ind(q, mp)) < 0)
			ch_set_state(cd, oldstate);
		return (rtn);
	}
	switch (prim) {
	case CH_INFO_ACK:
		printd(("%s: %p: CH_INFO_ACK\n", MOD_NAME, cd));
		rtn = ch_info_ack(q, mp);
		break;
	case CH_OPTMGMT_ACK:
		printd(("%s: %p: CH_OPTMGMT_ACK\n", MOD_NAME, cd));
		rtn = ch_optmgmt_ack(q, mp);
		break;
	case CH_OK_ACK:
		printd(("%s: %p: CH_OK_ACK\n", MOD_NAME, cd));
		rtn = ch_ok_ack(q, mp);
		break;
	case CH_ERROR_ACK:
		printd(("%s: %p: CH_ERROR_ACK\n", MOD_NAME, cd));
		rtn = ch_error_ack(q, mp);
		break;
	case CH_ENABLE_CON:
		printd(("%s: %p: CH_ENABLE_CON\n", MOD_NAME, cd));
		rtn = ch_enable_con(q, mp);
		break;
	case CH_CONNECT_CON:
		printd(("%s: %p: CH_CONNECT_CON\n", MOD_NAME, cd));
		rtn = ch_connect_con(q, mp);
		break;
	case CH_DATA_IND:
		printd(("%s: %p: CH_DATA_IND\n", MOD_NAME, cd));
		rtn = ch_data_ind(q, mp);
		break;
	case CH_DISCONNECT_IND:
		printd(("%s: %p: CH_DISCONNECT_IND\n", MOD_NAME, cd));
		rtn = ch_disconnect_ind(q, mp);
		break;
	case CH_DISCONNECT_CON:
		printd(("%s: %p: CH_DISCONNECT_CON\n", MOD_NAME, cd));
		rtn = ch_disconnect_con(q, mp);
		break;
	case CH_DISABLE_IND:
		printd(("%s: %p: CH_DISABLE_IND\n", MOD_NAME, cd));
		rtn = ch_disable_ind(q, mp);
		break;
	case CH_DISABLE_CON:
		printd(("%s: %p: CH_DISABLE_CON\n", MOD_NAME, cd));
		rtn = ch_disable_con(q, mp);
		break;
	case CH_EVENT_IND:
		printd(("%s: %p: CH_EVENT_IND\n", MOD_NAME, cd));
		rtn = ch_event_ind(q, mp);
		break;
	default:
		/* 
		   dump anthing we don't recognize */
		swerr();
		rtn = (-EFAULT);
		break;
	}
	if (rtn < 0)
		ch_set_state(cd, oldstate);
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_w_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	(void) cd;
	printd(("%s: %p: -> M_DATA [%d]\n", MOD_NAME, cd, msgdsize(mp)));
	return cd_write(q, mp);
}
STATIC INLINE int
cd_r_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = PRIV(q);
	(void) cd;
	printd(("%s: %p: M_DATA [%d] <-\n", MOD_NAME, cd, msgdsize(mp)));
	return ch_read(q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC int
cd_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return cd_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return cd_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC int
cd_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return cd_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return cd_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return cd_w_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC struct cd *cd_list = NULL;
STATIC int
cd_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct cd *cd;
		/* test for multiple push */
		for (cd = cd_list; cd; cd = cd->next) {
			if (cd->u.dev.cmajor == cmajor && cd->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(cd = hdlc_alloc_priv(q, &cd_list, devp, crp))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		qprocson(q);
		/* generate immediate information request */
		ch_info_req(q, cd);
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	hdlc_free_priv(q);
	MOD_DEC_USE_COUNT;
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

unsigned short modid = MOD_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the HDLC module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw hdlc_fmod = {
	.f_name = MOD_NAME,
	.f_str = &hdlcinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
hdlc_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&hdlc_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
hdlc_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&hdlc_fmod)) < 0)
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
hdlc_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&hdlcinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
hdlc_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&hdlcinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
hdlcinit(void)
{
	int err;
#ifdef MODULE
	cmn_err(CE_NOTE, HDLC_BANNER);	/* banner message */
#else
	cmn_err(CE_NOTE, HDLC_SPLASH);	/* console splash */
#endif
	if ((err = hdlc_init_tables())) {
		cmn_err(CE_WARN, "%s: could not init tables, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = hdlc_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = hdlc_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		hdlc_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
hdlcterminate(void)
{
	int err;
	if ((err = hdlc_free_tables()))
		cmn_err(CE_WARN, "%s: could not free tables", MOD_NAME);
	if ((err = hdlc_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = hdlc_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(hdlcinit);
module_exit(hdlcterminate);

#endif				/* LINUX */
