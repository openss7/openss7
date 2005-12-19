/*****************************************************************************

 @(#) $RCSfile: sdt.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/12/19 03:25:58 $

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/12/19 03:25:58 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sdt.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/12/19 03:25:58 $"

static char const ident[] =
    "$RCSfile: sdt.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/12/19 03:25:58 $";

/*
 *  This is a SDT (Signalling Data Terminal) kernel module.  It provides the
 *  SDT interface on the top of the module and can be pushed on top of a
 *  stream providing the SDL (Signalling Data Link) interface.  This module
 *  contains the DAEDR, DAEDT, SUERM, AERM and EIM functions to be tested once
 *  and used by many drivers.  The SDL stream upon which this module is pushed
 *  provides and accepts a blocked bit-stream as input and output.  Conversion
 *  between the packet interface of the SDT and the bit-stream interface of
 *  the SDL is performed using soft-HDLC.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

#define SDT_DESCRIP	"SS7/SDT: (Signalling Data Terminal) STREAMS MODULE."
#define SDT_REVISION	"OpenSS7 $RCSfile: sdt.c,v $ $Name:  $ ($Revision: 0.9.2.12 $) $Date: 2005/12/19 03:25:58 $"
#define SDT_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SDT_DEVICE	"Supports OpenSS7 SDL drivers."
#define SDT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDT_LICENSE	"GPL"
#define SDT_BANNER	SDT_DESCRIP	"\n" \
			SDT_REVISION	"\n" \
			SDT_COPYRIGHT	"\n" \
			SDT_DEVICE	"\n" \
			SDT_CONTACT	"\n"
#define SDT_SPLASH	SDT_DEVICE	" - " \
			SDT_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SDT_CONTACT);
MODULE_DESCRIPTION(SDT_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDT_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdt");
#endif
#endif				/* LINUX */

#ifndef SDT_MOD_NAME
#define SDT_MOD_NAME	"sdt"
#endif

#ifndef SDT_MOD_ID
#define SDT_MOD_ID	0
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define MOD_ID		SDT_MOD_ID
#define MOD_NAME	SDT_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SDT_BANNER
#else				/* MODULE */
#define MOD_BANNER	SDT_SPLASH
#endif				/* MODULE */

STATIC struct module_info sdt_rinfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 128,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC struct module_info sdt_winfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 280,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC int streamscall sdt_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int streamscall sdt_close(queue_t *, int, cred_t *);

STATIC struct qinit sdt_rinit = {
	.qi_putp = ss7_oput,		/* Read put (message from below) */
	.qi_qopen = sdt_open,		/* Each open */
	.qi_qclose = sdt_close,		/* Last close */
	.qi_minfo = &sdt_rinfo,		/* Information */
};

STATIC struct qinit sdt_winit = {
	.qi_putp = ss7_iput,		/* Write put (message from above) */
	.qi_minfo = &sdt_winfo,		/* Information */
};

STATIC struct streamtab sdtinfo = {
	.st_rdinit = &sdt_rinit,	/* Upper read queue */
	.st_wrinit = &sdt_winit,	/* Upper write queue */
};

/*
 *  ========================================================================
 *
 *  Private structure
 *
 *  ========================================================================
 */
typedef struct sdt_path {
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
} sdt_path_t;

typedef struct sdt {
	STR_DECLARATION (struct sdt);	/* streamd declaration */
	sdt_path_t tx;			/* transmit path variables */
	sdt_path_t rx;			/* receive path variables */
	uint rx_octets;			/* no received octets */
	sdt_timers_t timers;		/* SDT protocol timers */
	lmi_option_t option;		/* LMI protocol and variant options */
	sdt_statem_t statem;		/* SDT state machine variables */
	sdt_config_t config;		/* SDT configuration options */
	sdt_notify_t notify;		/* SDT notification options */
	sdt_stats_t stats;		/* SDT statistics */
	sdt_stats_t stamp;		/* SDT statistics timestamps */
	lmi_sta_t statsp;		/* SDT statistics periods */
} sdt_t;
#define SDT_PRIV(__q) ((struct sdt *)(__q)->q_ptr)

struct sdt *sdt_opens = NULL;

STATIC struct sdt *sdt_alloc_priv(queue_t *, struct sdt **, dev_t *, cred_t *);
STATIC struct sdt *sdt_get(struct sdt *);
STATIC void sdt_put(struct sdt *);
STATIC void sdt_free_priv(queue_t *);

struct lmi_option lmi_default = {
	pvar:SS7_PVAR_ITUT_96,
	popt:0,
};

struct sdt_config sdt_default = {
	Tin:4,				/* AERM normal proving threshold */
	Tie:1,				/* AERM emergency proving threshold */
	T:64,				/* SUERM error threshold */
	D:256,				/* SUERM error rate parameter */
	t8:100 * HZ / 1000,		/* T8 timeout */
	Te:577169,			/* EIM error threshold */
	De:9308,			/* EIM correct decrement */
	Ue:144292,			/* EIM error increment */
	N:16,				/* octets per su in OCM */
	m:272,				/* maximum SIF size */
	b:8,				/* transmit block size */
	f:SDT_FLAGS_ONE,		/* one flag between frames */
};

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
STATIC int
m_error(queue_t *q, struct sdt *s, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		s->i_state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_ERROR\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  M_HANGUP
 *  -----------------------------------
 */
STATIC int
m_hangup(queue_t *q, struct sdt *s, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		s->i_state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 *  NOTE: before pushing SDT as a module, a STYLE2 SDL driver should have
 *  already been attached so that the SDT module can run as a STYLE1 module.
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = s->i_state;
		p->lmi_max_sdu = s->config.m + 1 + ((s->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((s->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE1;	/* only STYLE1 for modules */
		printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, s));
		putnext(s->oq, mp);
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
lmi_ok_ack(queue_t *q, struct sdt *s, long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (s->i_state) {
		case LMI_ATTACH_PENDING:
			s->i_state = LMI_DISABLED;
			break;
		case LMI_DETACH_PENDING:
			s->i_state = LMI_UNATTACHED;
			break;
		}
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ack(queue_t *q, struct sdt *s, long prim, ulong reason, ulong errno)
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
		switch (s->i_state) {
		case LMI_ATTACH_PENDING:
			s->i_state = LMI_UNATTACHED;
			break;
		case LMI_DETACH_PENDING:
			s->i_state = LMI_DISABLED;
			break;
		case LMI_ENABLE_PENDING:
			s->i_state = LMI_DISABLED;
			break;
		case LMI_DISABLE_PENDING:
			s->i_state = LMI_ENABLED;
			break;
		default:
			break;
		}
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_enable_con(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, s));
		putnext(s->oq, mp);
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
lmi_disable_con(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, s));
		putnext(s->oq, mp);
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
lmi_optmgmt_ack(queue_t *q, struct sdt *s, ulong flags, caddr_t opt_ptr, size_t opt_len)
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
		p->lmi_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, s));
		putnext(s->oq, mp);
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
lmi_error_ind(queue_t *q, struct sdt *s, long error, long reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = error;
		p->lmi_reason = reason;
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
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
lmi_stats_ind(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	lmi_stats_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = 0;
		p->lmi_timestamp = jiffies;
		printd(("%s: %p: <- LMI_STATS_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, struct sdt *s, ulong oid, ulong level, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	lmi_event_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + inf_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_EVENT_IND;
		p->lmi_objectid = oid;
		p->lmi_timestamp = jiffies;
		p->lmi_severity = level;
		bcopy(inf_ptr, mp->b_wptr, inf_len);
		mp->b_wptr += inf_len;
		printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDT_RC_SIGNAL_UNIT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_signal_unit_ind(queue_t *q, struct sdt *s, mblk_t *dp, ulong count)
{
	if (count == 1) {
		printd(("%s: %p: <- M_DATA [%d]\n", MOD_NAME, s, msgdsize(dp)));
		putnext(s->oq, dp);
		return (QR_DONE);
	} else if (count > 1) {
		mblk_t *mp;
		sdt_rc_signal_unit_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
			p->sdt_count = count;
			mp->b_cont = dp;
			printd(("%s: %p: <- SDT_RC_SIGNAL_UNIT_IND [%lu x %d]\n", MOD_NAME, s,
				count, msgdsize(dp)));
			putnext(s->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SDT_RC_CONGESTION_ACCEPT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_accept_ind(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdt_rc_congestion_accept_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_ACCEPT_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  SDT_RC_CONGESTION_DISCARD_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_discard_ind(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdt_rc_congestion_discard_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_DISCARD_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDT_RC_NO_CONGESTION_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_no_congestion_ind(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdt_rc_no_congestion_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
		printd(("%s: %p: <- SDT_RC_NO_CONGESTION_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
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
sdt_iac_correct_su_ind(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdt_iac_correct_su_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
		printd(("%s: %p: <- SDT_IAC_CORRECT_SU_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_IAC_ABORT_PROVING_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_iac_abort_proving_ind(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdt_iac_abort_proving_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
		printd(("%s: %p: <- SDT_IAC_ABORT_PROVING_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
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
sdt_lsc_link_failure_ind(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdt_lsc_link_failure_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
		printd(("%s: %p: <- SDT_LSC_LINK_FAILURE_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
   We let back-enabling do this for us 
 */
/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_txc_transmission_request_ind(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdt_txc_transmission_request_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		printd(("%s: %p: <- SDT_TXC_TRANSMISSION_REQUEST_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  ========================================================================
 *
 *  PRIMITIVES Sent Downstream
 *
 *  ========================================================================
 */
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_info_req(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	lmi_info_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", MOD_NAME, s));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
   We don't need any of these because these are just passed along. 
 */
/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_attach_req(queue_t *q, struct sdt *s, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_attach_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ATTACH_REQ;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: LMI_ATTACH_REQ ->\n", MOD_NAME, s));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_detach_req(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	lmi_detach_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DETACH_REQ;
		printd(("%s: %p: LMI_DETACH_REQ ->\n", MOD_NAME, s));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_enable_req(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	lmi_enable_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_REQ;
		printd(("%s: %p: LMI_ENABLE_REQ ->\n", MOD_NAME, s));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_disable_req(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	lmi_disable_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_REQ;
		printd(("%s: %p: LMI_DISABLE_REQ ->\n", MOD_NAME, s));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_optmgmt_req(queue_t *q, struct sdt *s, long flags, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: LMI_OPTMGMT_REQ ->\n", MOD_NAME, s));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SDL_BITS_FOR_TRANSMISSION_REQ
 *  -----------------------------------
 *  This is the non-prefferred method for sending transmitted bits.  The
 *  preferred method is to send just an M_DATA block.
 */
STATIC INLINE int
sdl_bits_for_transmission_req(queue_t *q, struct sdt *s, mblk_t *dp)
{
	mblk_t *mp;
	sdl_bits_for_transmission_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_BITS_FOR_TRANSMISSION_REQ;
		mp->b_cont = dp;
		printd(("%s: %p: SDL_BITS_FOR_TRANSMISSION [%d] ->\n", MOD_NAME, s, msgdsize(dp)));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_CONNECT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_connect_req(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdl_connect_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_CONNECT_REQ;
		printd(("%s: %p: SDL_CONNECT_REQ ->\n", MOD_NAME, s));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_disconnect_req(queue_t *q, struct sdt *s)
{
	mblk_t *mp;
	sdl_disconnect_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_DISCONNECT_REQ;
		printd(("%s: %p: SDL_DISCONNECT_REQ ->\n", MOD_NAME, s));
		putnext(s->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
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
 *  -------------------------------------------------------------------------
 *
 *  Timers
 *
 *  -------------------------------------------------------------------------
 */
enum { tall, t8 };

SS7_DECLARE_TIMER(MOD_NAME, sdt, t8, config);

STATIC INLINE void
__sdt_timer_stop(struct sdt *s, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t8:
		sdt_stop_timer_t8(s);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sdt_timer_stop(struct sdt *s, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		__sdt_timer_stop(s, t);
	}
	spin_unlock_irqrestore(&s->lock, flags);
}
STATIC INLINE void
sdt_timer_start(struct sdt *s, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		__sdt_timer_stop(s, t);
		switch (t) {
		case t8:
			sdt_start_timer_t8(s);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&s->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  State Machine
 *
 *  -------------------------------------------------------------------------
 */

STATIC INLINE int
sdt_aerm_su_in_error(queue_t *q, struct sdt *s)
{
	int err;
	if (s->statem.aerm_state == SDT_STATE_MONITORING) {
		s->statem.Ca++;
		if (s->statem.Ca == s->statem.Ti) {
			s->statem.aborted_proving = 1;
			if ((err = sdt_iac_abort_proving_ind(q, s))) {
				s->statem.Ca--;
				return (err);
			}
			s->statem.aerm_state = SDT_STATE_IDLE;
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sdt_aerm_correct_su(queue_t *q, struct sdt *s)
{
	int err;
	if (s->statem.aerm_state == SDT_STATE_IDLE) {
		if (s->statem.aborted_proving) {
			if ((err = sdt_iac_correct_su_ind(q, s)))
				return (err);
			s->statem.aborted_proving = 0;
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sdt_suerm_start(queue_t *q, struct sdt *s)
{
	s->statem.Cs = 0;
	s->statem.Ns = 0;
	s->statem.suerm_state = SDT_STATE_IN_SERVICE;
	return (QR_DONE);
}

STATIC INLINE void
sdt_suerm_stop(queue_t *q, struct sdt *s)
{
	s->statem.suerm_state = SDT_STATE_IDLE;
}

STATIC INLINE int
sdt_suerm_su_in_error(queue_t *q, struct sdt *s)
{
	int err;
	if (s->statem.suerm_state == SDT_STATE_IN_SERVICE) {
		s->statem.Cs++;
		if (s->statem.Cs >= s->config.T) {
			if ((err = sdt_lsc_link_failure_ind(q, s))) {
				s->statem.Ca--;
				return (err);
			}
			s->statem.suerm_state = SDT_STATE_IDLE;
			return (QR_DONE);
		}
		s->statem.Ns++;
		if (s->statem.Ns >= s->config.D) {
			s->statem.Ns = 0;
			if (s->statem.Cs)
				s->statem.Cs--;
		}
	}
	return (QR_DONE);
}

STATIC INLINE void
sdt_eim_su_in_error(queue_t *q, struct sdt *s)
{
	if (s->statem.eim_state == SDT_STATE_MONITORING)
		s->statem.interval_error = 1;
}

STATIC INLINE void
sdt_suerm_correct_su(queue_t *q, struct sdt *s)
{
	if (s->statem.suerm_state == SDT_STATE_IN_SERVICE) {
		s->statem.Ns++;
		if (s->statem.Ns >= s->config.D) {
			s->statem.Ns = 0;
			if (s->statem.Cs)
				s->statem.Cs--;
		}
	}
}

STATIC INLINE void
sdt_eim_correct_su(queue_t *q, struct sdt *s)
{
	if (s->statem.eim_state == SDT_STATE_MONITORING)
		s->statem.su_received = 1;
}

STATIC INLINE int
sdt_eim_start(queue_t *q, struct sdt *s)
{
	s->statem.Ce = 0;
	s->statem.interval_error = 0;
	s->statem.su_received = 0;
	sdt_timer_start(s, t8);
	s->statem.eim_state = SDT_STATE_MONITORING;
	return (QR_DONE);
}

STATIC INLINE void
sdt_eim_stop(queue_t *q, struct sdt *s)
{
	s->statem.eim_state = SDT_STATE_IDLE;
	sdt_timer_stop(s, t8);
}

STATIC INLINE int
sdt_daedr_correct_su(queue_t *q, struct sdt *s)
{
	sdt_eim_correct_su(q, s);
	sdt_suerm_correct_su(q, s);
	return sdt_aerm_correct_su(q, s);
}

STATIC INLINE int
sdt_daedr_su_in_error(queue_t *q, struct sdt *s)
{
	int err;
	sdt_eim_su_in_error(q, s);
	if ((err = sdt_suerm_su_in_error(q, s)))
		return (err);
	if ((err = sdt_aerm_su_in_error(q, s)))
		return (err);
	return (QR_DONE);
}

STATIC INLINE int
sdt_daedr_received_bits(queue_t *q, struct sdt *s, mblk_t *mp)
{
	sdt_path_t *rx = &s->rx;
	int err, pos, match = 0, len = msgdsize(mp);
	int hlen = (s->option.popt & SS7_POPT_XSN) ? 6 : 3;
	if (rx->cmp) {
		if (len < hlen + 3 && len >= hlen && len == msgdsize(rx->cmp))
			for (match = 1, pos = 0; pos < len; pos++)
				if (!(match = (mp->b_rptr[pos] == rx->cmp->b_rptr[pos])))
					break;
		if (match) {
			rx->repeat++;
			s->stats.rx_sus_compressed++;
			freemsg(mp);
			sdt_daedr_correct_su(q, s);
			return (QR_ABSORBED);
		} else if (rx->repeat > 0
			   && (err =
			       sdt_rc_signal_unit_ind(q, s, xchg(&rx->cmp, NULL),
						      xchg(&rx->repeat, 0))) != QR_ABSORBED)
			goto overflow;
	}
	if (!match && len < hlen + 3 && (rx->cmp || (rx->cmp = allocb(len, BPRI_MED)))) {
		bcopy(mp->b_rptr, rx->cmp->b_rptr, len);
		rx->cmp->b_wptr = rx->cmp->b_rptr + len;
		rx->repeat = 0;
	}
	sdt_daedr_correct_su(q, s);
	if ((err = sdt_rc_signal_unit_ind(q, s, mp, 1)) == QR_ABSORBED)
		return (QR_ABSORBED);
      overflow:
	s->stats.rx_buffer_overflows++;
	return (err);
}

#if 0
STATIC INLINE int
sdt_daedt_transmission_request(queue_t *q, struct sdt *s)
{
	return sdt_txc_transmission_request_ind(q, s);
}
#endif

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

struct tx_entry __attribute__ ((packed));
typedef struct tx_entry {
	uint bit_string:10;		/* the output string */
	uint bit_length:4;		/* length in excess of 8 bits of output string */
	uint state:3;			/* new state */
} tx_entry_t;

struct rx_entry __attribute__ ((packed));
typedef struct rx_entry {
	uint bit_string:16;
	uint bit_length:4;
	uint state:4;
	uint sync:1;
	uint hunt:1;
	uint flag:1;
	uint idle:1;
} rx_entry_t;

typedef uint16_t bc_entry_t;

STATIC bc_entry_t *bc_table = NULL;
STATIC tx_entry_t *tx_table = NULL;
STATIC rx_entry_t *rx_table = NULL;
// STATIC rx_entry_t *rx_table7 = NULL;

STATIC size_t bc_order = 0;
STATIC size_t tx_order = 0;
STATIC size_t rx_order = 0;

STATIC INLINE tx_entry_t *
tx_index8(uint j, uint k)
{
	return &tx_table[(j << 8) | k];
}
#if 0
STATIC INLINE rx_entry_t *
rx_index7(uint j, uint k)
{
	return &rx_table7[(j << 8) | k];
}
#endif
STATIC INLINE rx_entry_t *
rx_index8(uint j, uint k)
{
	return &rx_table[(j << 8) | k];
}

#ifdef _DEBUG
#if 0
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
#endif
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
 *
 *  We need to set up the repeat buffer with every new message.  The repeat
 *  buffer contains a FISU for transmitted MSUs (if non-PCR).  It contains the
 *  FISU, LSSU or 2-byte LSSU for transmitted FISUs and LSSUs.  For SIBs it
 *  contains a FISU (if non-PCR), for other LSSUs it contains the LSSU.
 */
STATIC int
sdt_tx_repeat(queue_t *q, struct sdt *s, mblk_t *mp)
{
	mblk_t *cp;
	int len, hlen, clen;
	if ((len = msgdsize(mp)) < (hlen = (s->option.popt & SS7_POPT_XSN) ? 6 : 3)) {
		ptrace(("%s: %p: ERROR: user violated minimum size\n", MOD_NAME, s));
		goto dont_repeat;
	}
	if (len > hlen + 2
	    || ((len == hlen + 1 || len == hlen + 2) && (mp->b_rptr[hlen] & 0x7) == 5)) {
		/* 
		   MSU or SIB */
		if (s->option.popt & SS7_POPT_PCR)
			goto dont_repeat;
		clen = hlen;
	} else {
		/* 
		   FISU or LSSU */
		clen = len;
	}
	if (!(cp = s->tx.cmp) && !(cp = s->tx.cmp = ss7_allocb(q, clen, BPRI_MED)))
		return (-ENOBUFS);
	bcopy(mp->b_rptr, cp->b_rptr, clen);
	cp->b_wptr = cp->b_rptr + clen;
	if (!(s->option.popt & SS7_POPT_XSN))
		((uint8_t *) cp->b_rptr)[2] = clen - hlen;
	else
		((uint16_t *) cp->b_rptr)[2] = clen - hlen;
	return (0);
      dont_repeat:
	if (s->tx.cmp)
		freemsg(xchg(&s->tx.cmp, NULL));
	return (0);
}

/*
 *  TX BUFFER
 *  ----------------------------------------
 *  Pick up another TX buffer from the queue or a repeat frame.
 */
STATIC mblk_t *
sdt_tx_buffer(queue_t *q, struct sdt *s)
{
	mblk_t *dp = NULL;
	psw_t flags;
	/* 
	   lock down while we traverse queue */
	spin_lock_irqsave(&s->qlock, flags);
	{
		mblk_t *mp;
		for (mp = s->iq->q_first; mp; mp = mp->b_next) {
			switch (mp->b_datap->db_type) {
			case M_DATA:
				if (sdt_tx_repeat == 0) {
					dp = mp;
					rmvq(s->iq, dp);
				}
				break;
			case M_PROTO:
				if (*((ulong *) mp->b_rptr) != SDT_DAEDT_TRANSMISSION_REQ)
					continue;
				if (sdt_tx_repeat == 0) {
					dp = mp->b_cont;
					rmvq(s->iq, mp);
					freeb(mp);
				}
				break;
			default:
				continue;
			}
			break;
		}
	}
	spin_unlock_irqrestore(&s->qlock, flags);
	if (!dp && (dp = s->tx.cmp)) {
		fixme(("Peg some stats\n"));
	}
	return (dp);
}

/*
 *  TX BITSTUFF
 *  ----------------------------------------
 *  Bitstuff an octet and shift residue for output.
 */
STATIC INLINE void
sdt_tx_bitstuff(sdt_path_t * tx, unsigned char byte)
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
 *  repeat FISU/LSSU or idle flags.
 */
STATIC void
sdt_tx_block(queue_t *q, struct sdt *s)
{
	mblk_t *bp;
	register sdt_path_t *tx = &s->tx;
	sdt_stats_t *stats = &s->stats;
#if 0
	int bits = (s->config.iftype == SDL_TYPE_DS0A) ? 7 : 8;
#else
	const int bits = 8;
#endif
	while (canputnext(s->iq)) {
		if (!(bp = ss7_allocb(q, s->config.b, BPRI_MED)))
			break;	/* bufcall will bring us back */
		if (tx->mode == TX_MODE_IDLE || tx->mode == TX_MODE_FLAG) {
			if (!tx->nxt) {
			      next_message:
				if (tx->msg && tx->msg != tx->cmp)
					freemsg(tx->msg);
				if ((tx->msg = tx->nxt = sdt_tx_buffer(q, s)))
					tx->mode = TX_MODE_BOF;
			}
		}
		/* 
		   check if transmission block complete */
		while (bp->b_wptr < bp->b_rptr + s->config.b) {
			/* 
			   drain residue bits, if necessary */
			if (tx->rbits >= bits) {
			      drain_rbits:
				/* 
				   drain residue bits */
				if (bits == 8)
					*bp->b_wptr++ = tx->residue;
				else
					*bp->b_wptr++ = tx->residue & 0x7f;
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
				switch (s->config.f) {
				case SDT_FLAGS_ONE:
					tx->residue |= 0x7e << tx->rbits;
					tx->rbits += 8;
					break;
				case SDT_FLAGS_SHARED:
					tx->residue |= 0x3f7e << tx->rbits;
					tx->rbits += 15;
					break;
				case SDT_FLAGS_TWO:
					tx->residue |= 0x7e7e << tx->rbits;
					tx->rbits += 16;
					break;
				default:
				case SDT_FLAGS_THREE:
					tx->residue |= 0x7e7e7e << tx->rbits;
					tx->rbits += 24;
					break;
				}
				tx->state = 0;
				tx->bcc = 0x00ff;
				tx->mode = TX_MODE_MOF;
				goto drain_rbits;
			case TX_MODE_MOF:	/* transmit frame bytes */
				if (tx->nxt->b_rptr < tx->nxt->b_wptr
				    || (tx->nxt = tx->nxt->b_cont)) {
					/* 
					   continuing in message */
					uint byte = *(tx->nxt->b_rptr)++;
					tx->bcc =
					    (tx->bcc >> 8) ^ bc_table[(tx->bcc ^ byte) & 0x00ff];
					sdt_tx_bitstuff(tx, byte);
					stats->tx_bytes++;
				} else {
					/* 
					   finished message: add 1st bcc byte */
					sdt_tx_bitstuff(tx, tx->bcc & 0x00ff);
					tx->mode = TX_MODE_BCC;
				}
				goto drain_rbits;
			case TX_MODE_BCC:
				/* 
				   add 2nd bcc byte */
				sdt_tx_bitstuff(tx, tx->bcc >> 8);
				stats->tx_sus++;
				tx->mode = TX_MODE_FLAG;
				goto next_message;
			}
			swerr();
		}
		putnext(q, bp);
	}
}

#if 0
/*
 *  RX COMPRESS
 *  ----------------------------------------
 *  Check for RX compression.  Returns 1 when compression in effect, 0
 *  otherwise.
 */
STATIC INLINE int
sdt_rx_compress(queue_t *q, struct sdt *s, mblk_t *mp)
{
	mblk_t *cp = s->rx.cmp;
	int len1 = msgdsize(mp);
	int len2 = cp ? msgdsize(cp) : 0;
	int len_max = (s->option.popt & SS7_POPT_XSN) ? 8 : 5;
	if (len1 > len_max) {
		// printd(("sdt: len1 = %d > len_max = %d\n", len1, len_max));
		goto failure;
	}
	if (len1 != len2) {
		// printd(("sdt: len1 = %d != len2 = %d\n", len1, len2));
		goto failure;
	} else {
		int pos;
		// pullupmsg(mp, len1);
		for (pos = 0; pos < len1; pos++)
			if (cp->b_rptr[pos] != mp->b_rptr[pos]) {
				// printd(("sdt: mismatch at byte %d\n", pos + 1));
				goto failure;
			}
		return (1);
	}
      failure:
	return (0);
}
#endif

/*
 *  RX LINKB
 *  ----------------------------------------
 *  Link a buffer to existing message or create new message with buffer.
 */
STATIC INLINE void
sdt_rx_linkb(sdt_path_t * rx)
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
STATIC void
sdt_rx_block(queue_t *q, struct sdt *s, mblk_t *dp)
{
	register sdt_path_t *rx = &s->rx;
	sdt_stats_t *stats = &s->stats;
	while (dp->b_rptr < dp->b_wptr) {
		rx_entry_t *r;
#if 0
		if (s->config.iftype != SDL_TYPE_DS0A)
#endif
			r = rx_index8(rx->state, *dp->b_rptr++);
#if 0
		else
			r = rx_index7(rx->state, *dp->b_rptr++);
#endif
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
					if (rx->nxt && rx->nxt->b_wptr >= rx->nxt->b_datap->db_lim)
						sdt_rx_linkb(rx);
					if (!rx->nxt && !(rx->nxt = allocb(FASTBUF, BPRI_HI)))
						goto buffer_overflow;
					rx->bcc = (rx->bcc >> 8)
					    ^ bc_table[(rx->bcc ^ rx->residue) & 0x00ff];
					*(rx->nxt->b_wptr)++ = rx->residue;
					stats->rx_bytes++;
					rx->residue >>= 8;
					rx->rbits -= 8;
					rx->bytes++;
					if (!(s->option.popt & SS7_POPT_XSN)) {
						if (rx->bytes > s->config.m + 1 + 3)
							goto frame_too_long;
					} else {
						if (rx->bytes > s->config.m + 1 + 6)
							goto frame_too_long;
					}
				}
			} else {
				uint li;
				if (rx->rbits != 16)
					goto residue_error;
				if (rx->bcc != (rx->residue & 0xffff))
					goto crc_error;
				if (!(s->option.popt & SS7_POPT_XSN)) {
					if (rx->bytes < 3)
						goto frame_too_short;
					sdt_rx_linkb(rx);
					li = (rx->msg->b_rptr[2] & 0x3f) + 3;
					if (rx->bytes != li && (li != 0x3f + 3 || rx->bytes < li))
						goto length_error;
				} else {
					if (rx->bytes < 6)
						goto frame_too_short;
					sdt_rx_linkb(rx);
					li = (((rx->msg->b_rptr[5] << 8)
					       | rx->msg->b_rptr[4]) & 0x1ff) + 6;
					if (rx->bytes != li && (li != 0x1ff + 6 || rx->bytes < li))
						goto length_error;
				}
				stats->rx_sus++;
				sdt_daedr_received_bits(q, s, xchg(&rx->msg, NULL));
			      new_frame:
				rx->mode = RX_MODE_SYNC;
				if (r->sync) {
				      begin_frame:
					if (r->bit_length) {
						rx->mode = RX_MODE_MOF;
						rx->residue = r->bit_string;
						rx->rbits = r->bit_length;
						rx->bytes = 0;
						rx->bcc = 0x00ff;
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
				freemsg(xchg(&rx->nxt, NULL));
			if (rx->msg)
				freemsg(xchg(&rx->msg, NULL));
			stats->rx_sus_in_error++;
			sdt_daedr_su_in_error(q, s);
			if (r->flag)
				goto new_frame;
			rx->mode = RX_MODE_HUNT;
			stats->rx_sync_transitions++;
			s->rx_octets = 0;
			break;
		case RX_MODE_SYNC:
			if (!r->hunt && !r->idle)
				goto begin_frame;
			rx->mode = RX_MODE_HUNT;
			stats->rx_sync_transitions++;
			s->rx_octets = 0;
			break;
		case RX_MODE_HUNT:
			if (!r->flag) {
				if ((++(s->rx_octets)) >= s->config.N) {
					stats->rx_sus_in_error++;
					sdt_daedr_su_in_error(q, s);
					s->rx_octets -= s->config.N;
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
	}
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
rx_table_value(int state, uint8_t byte)
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
	for (j = 0; j < SDT_TX_STATES; j++)
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
 *  delivering bits to the SDT.
 */
STATIC void
rx_table_generate(void)
{
	int j, k;
	for (j = 0; j < SDT_RX_STATES; j++)
		for (k = 0; k < 256; k++)
			*rx_index8(j, k) = rx_table_value(j, k);
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
sdt_init_tables(void)
{
	size_t length;
	length = SDT_CRC_TABLE_LENGTH * sizeof(bc_entry_t);
	for (bc_order = 0; PAGE_SIZE << bc_order < length; bc_order++) ;
	if (!(bc_table = (bc_entry_t *) __get_free_pages(GFP_KERNEL, bc_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate bc_table\n", __FUNCTION__);
		goto bc_failed;
	}
	printd(("sdt: allocated BC table size %u kernel pages\n", 1 << bc_order));
	bc_table_generate();
	printd(("sdt: generated BC table\n"));
	length = SDT_TX_TABLE_LENGTH * sizeof(tx_entry_t);
	for (tx_order = 0; PAGE_SIZE << tx_order < length; tx_order++) ;
	if (!(tx_table = (tx_entry_t *) __get_free_pages(GFP_KERNEL, tx_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate tx_table\n", __FUNCTION__);
		goto tx_failed;
	}
	printd(("sdt: allocated Tx table size %u kernel pages\n", 1 << tx_order));
	tx_table_generate();
	printd(("sdt: generated 8-bit Tx table\n"));
	length = SDT_RX_TABLE_LENGTH * sizeof(rx_entry_t);
	for (rx_order = 0; PAGE_SIZE << rx_order < length; rx_order++) ;
	if (!(rx_table = (rx_entry_t *) __get_free_pages(GFP_KERNEL, rx_order))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate rx_table\n", __FUNCTION__);
		goto rx_failed;
	}
	printd(("sdt: allocated Rx table size %u kernel pages\n", 1 << rx_order));
	rx_table_generate();
	printd(("sdt: generated 8-bit Rx table\n"));
	return (0);
      rx_failed:
	free_pages((unsigned long) tx_table, tx_order);
	tx_order = 0;
      tx_failed:
	free_pages((unsigned long) bc_table, bc_order);
	bc_order = 0;
      bc_failed:
	return (-ENOMEM);
}
STATIC int
sdt_term_tables(void)
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
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */

/*
 *  RX WAKEUP
 *  -----------------------------------
 *  This is called before the queue service routine unlocks the queue.  We
 *  must check if back-enabling has occured on an empty read queue.
 */
STATIC void
sdt_rx_wakeup(queue_t *q)
{
	(void) q;
	return;
}

/*
 *  TX WAKEUP
 *  -----------------------------------
 *  This is called before the queue service routine unlocks the queue.
 *  sdt_tx_block will pull data from the queue as necessary.
 */
STATIC void
sdt_tx_wakeup(queue_t *q)
{
	struct sdt *s = SDT_PRIV(q);
	if ((s->i_state == LMI_ENABLED) && (s->statem.daedt_state != SDT_STATE_IDLE))
		sdt_tx_block(q, s);
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Events
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T8 TIMEOUT
 *  -----------------------------------
 */
STATIC int
sdt_t8_timeout(struct sdt *s)
{
	queue_t *q = NULL;
	int err;
	if (s->statem.eim_state == SDT_STATE_MONITORING) {
		sdt_timer_start(s, t8);
		if (s->statem.su_received) {
			s->statem.su_received = 0;
			if (!s->statem.interval_error) {
				if ((s->statem.Ce -= s->config.De) < 0)
					s->statem.Ce = 0;
				return (QR_DONE);
			}
		}
		s->statem.Ce += s->config.Ue;
		if (s->statem.Ce > s->config.Te) {
			if ((err = sdt_lsc_link_failure_ind(q, s))) {
				s->statem.Ce -= s->config.Ue;
				return (err);
			}
			s->statem.eim_state = SDT_STATE_IDLE;
		}
		s->statem.interval_error = 0;
	}
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LM User -> LM Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_REQ:
 *  -----------------------------------
 *  We just pass info request along and service them on reply from the lower
 *  level.
 */
STATIC int
sdt_info_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	return (QR_PASSFLOW);
      emsgsize:
	return lmi_error_ack(q, s, LMI_INFO_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ATTACH_REQ:
 *  -----------------------------------
 *  We do not perform attaches as this is a software SDT.  The SDT selected is
 *  the current SDT.  We do, however, pass these along to the lower level if
 *  the lower level is of the correct style.
 */
STATIC int
sdt_attach_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (s->i_state != LMI_UNATTACHED)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + 2)
		goto badppa;
	s->i_state = LMI_ATTACH_PENDING;
	return (QR_PASSFLOW);
      badppa:
	ptrace(("%s: %p: PROTO: bad ppa (too short)\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ATTACH_REQ, LMI_BADPPA, EMSGSIZE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ATTACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: %p: PROTO: primitive not supported for style\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ATTACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DETACH_REQ:
 *  -----------------------------------
 *  We do not perform detaches as this is a software SDT.  The SDT selected is
 *  the current SDT.  We do, however, pass these along to the lower level if
 *  the lower level is of the correct style.
 */
STATIC int
sdt_detach_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (s->i_state != LMI_DISABLED)
		goto outstate;
	s->i_state = LMI_DETACH_PENDING;
	return (QR_PASSFLOW);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DETACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: %p: PROTO: primitive not supported for style\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DETACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DETACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ENABLE_REQ:
 *  -----------------------------------
 *  We must allow the SDL first crack at enabling before we fully enable.  We
 *  commit the enable when the LMI_ENABLE_CON is returned from below.
 */
STATIC int
sdt_enable_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_state != LMI_DISABLED)
		goto outstate;
	s->i_state = LMI_ENABLE_PENDING;
	return (QR_PASSFLOW);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ENABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ENABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DISABLE_REQ:
 *  -----------------------------------
 *  We must allow the SDL first carack an disabling before we fully disable.
 *  We commit the disable when the LMI_DISABLE_CON is returned from below.
 */
STATIC int
sdt_disable_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	s->i_state = LMI_DISABLE_PENDING;
	return (QR_PASSFLOW);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DISABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DISABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_OPTMGMT_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	return (QR_PASSFLOW);
      emsgsize:
	(void) s;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_OPTMGMT_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDT User -> SDT Provider Primitives
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
	struct sdt *s = SDT_PRIV(q);
	int len, hlen;
	if (s->i_state != LMI_ENABLED)
		goto discard;
	if (s->statem.daedt_state == SDT_STATE_IDLE)
		goto discard;	/* ignore tramsmissions when daedt shut down */
	len = msgdsize(mp);
	hlen = (s->option.popt & SS7_POPT_XSN) ? 6 : 3;
	if (len < hlen || len > hlen + s->config.m + 1)
		goto eproto;
	/* 
	   let tx_wakeup pull from the queue */
	return (-EAGAIN);
      eproto:
	swerr();		/* length violation, error out stream */
	return m_error(q, s, EPROTO);
      discard:
	return (QR_DONE);	/* silent discard */
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
	if ((dp = mp->b_cont) && dp->b_datap->db_type == M_DATA) {
		trace();
		return (QR_STRIP);
	}
	return (-EPROTO);
}

/*
 *  SDT_DAEDT_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedt_start_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	sdt_daedt_start_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.daedt_state == SDT_STATE_IDLE)
		s->statem.daedt_state = SDT_STATE_IN_SERVICE;
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return (-EMSGSIZE);
}

/*
 *  SDT_DAEDR_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedr_start_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	sdt_daedr_start_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.daedr_state == SDT_STATE_IDLE)
		s->statem.daedr_state = SDT_STATE_IN_SERVICE;
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return (-EMSGSIZE);
}

/*
 *  SDT_AERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_start_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	sdt_aerm_start_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.aerm_state != SDT_STATE_MONITORING) {
		s->statem.Ca = 0;
		s->statem.aborted_proving = 0;
		s->statem.aerm_state = SDT_STATE_MONITORING;
	}
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return (-EMSGSIZE);
}

/*
 *  SDT_AERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_stop_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	sdt_aerm_stop_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.aerm_state != SDT_STATE_IDLE) {
		s->statem.aerm_state = SDT_STATE_IDLE;
		s->statem.Ti = s->config.Tin;
	}
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return (-EMSGSIZE);
}

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tin_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	sdt_aerm_set_ti_to_tin_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.aerm_state == SDT_STATE_IDLE)
		s->statem.Ti = s->config.Tin;
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return (-EMSGSIZE);
}

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tie_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	sdt_aerm_set_ti_to_tie_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.aerm_state == SDT_STATE_IDLE)
		s->statem.Ti = s->config.Tie;
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return (-EMSGSIZE);
}

/*
 *  SDT_SUERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_start_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	sdt_suerm_start_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.suerm_state == SDT_STATE_IDLE) {
		if (s->option.popt & SS7_POPT_HSL)
			return sdt_eim_start(q, s);
		else
			return sdt_suerm_start(q, s);
	}
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return (-EMSGSIZE);
}

/*
 *  SDT_SUERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_stop_req(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	sdt_suerm_stop_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.suerm_state != SDT_STATE_IDLE) {
		sdt_eim_stop(q, s);
		sdt_suerm_stop(q, s);
	}
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return (-EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL-Provider -> SDL-User Primitives
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 *  There are two situations in which we get such a reply: first is when we
 *  are initially pushed as a module over an SDL (in state LMI_UNUSABLE);
 *  second is whenever we need to service an LMI_INFO_REQ.
 */
STATIC int
sdl_info_ack(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	{
		ulong state = s->i_state;
		s->i_state = p->lmi_state;
		s->i_style = p->lmi_ppa_style;
		switch (state) {
		case LMI_UNUSABLE:
			if (s->i_state == LMI_UNUSABLE)
				goto merror;
			return (QR_DONE);
		case LMI_UNATTACHED:
		case LMI_ATTACH_PENDING:
		case LMI_DETACH_PENDING:
		case LMI_DISABLED:
		case LMI_ENABLE_PENDING:
		case LMI_DISABLE_PENDING:
		case LMI_ENABLED:
			p->lmi_version = s->i_version;
			p->lmi_state = s->i_state;
			p->lmi_max_sdu = s->config.m + 1;
			p->lmi_min_sdu = 3;
			p->lmi_header_len = 0;
			break;
		default:
			goto outstate;
		}
	}
	printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, s));
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      merror:
	swerr();
	return m_error(q, s, -EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC int
sdl_ok_ack(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_ok_ack_t *p = ((typeof(p)) mp->b_rptr);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (s->i_state) {
	case LMI_UNUSABLE:
		goto discard;
	case LMI_UNATTACHED:
	case LMI_DISABLED:
	case LMI_ENABLE_PENDING:
	case LMI_DISABLE_PENDING:
	case LMI_ENABLED:
		goto outstate;
	case LMI_ATTACH_PENDING:
		s->i_state = LMI_DISABLED;
		break;
	case LMI_DETACH_PENDING:
		s->i_state = LMI_UNATTACHED;
		break;
	default:
		goto discard;
	}
	printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, s));
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      discard:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC int
sdl_error_ack(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_error_ack_t *p = ((typeof(p)) mp->b_rptr);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (s->i_state) {
	case LMI_UNUSABLE:
		goto merror;
	case LMI_UNATTACHED:
	case LMI_DISABLED:
	case LMI_ENABLED:
		break;
	case LMI_ATTACH_PENDING:
		s->i_state = LMI_UNATTACHED;
		break;
	case LMI_DETACH_PENDING:
		s->i_state = LMI_DISABLED;
		break;
	case LMI_ENABLE_PENDING:
		s->i_state = LMI_DISABLED;
		break;
	case LMI_DISABLE_PENDING:
		s->i_state = LMI_ENABLED;
		break;
	default:
		goto discard;
	}
	printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, s));
	return (QR_PASSFLOW);
      discard:
	swerr();
	return (-EFAULT);
      merror:
	swerr();
	return m_error(q, s, -EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC int
sdl_enable_con(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (s->i_state) {
	case LMI_ENABLED:
		goto discard;
	case LMI_ENABLE_PENDING:
		s->i_state = LMI_ENABLED;
		break;
	case LMI_DISABLE_PENDING:
	case LMI_DISABLED:
	case LMI_DETACH_PENDING:
	case LMI_UNATTACHED:
	case LMI_ATTACH_PENDING:
	case LMI_UNUSABLE:
		goto outstate;
	default:
		goto discard;
	}
	printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, s));
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC int
sdl_disable_con(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (s->i_state) {
	case LMI_DISABLED:
		/* 
		   already disabled */
		goto discard;
	case LMI_DISABLE_PENDING:
		s->i_state = LMI_DISABLED;
		break;
	case LMI_ENABLED:
	case LMI_ENABLE_PENDING:
	case LMI_DETACH_PENDING:
	case LMI_UNATTACHED:
	case LMI_ATTACH_PENDING:
	case LMI_UNUSABLE:
		goto outstate;
	default:
		goto discard;
	}
	{
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			ss7_unbufcall((str_t *) s);
			sdt_timer_stop(s, t8);
			if (s->tx.msg && s->tx.msg != s->tx.cmp)
				freemsg(xchg(&s->tx.msg, NULL));
			if (s->tx.cmp)
				freemsg(xchg(&s->tx.cmp, NULL));
			s->tx.mode = TX_MODE_IDLE;
			s->statem.daedt_state = SDT_STATE_IDLE;
			if (s->rx.msg)
				freemsg(xchg(&s->rx.msg, NULL));
			if (s->rx.nxt)
				freemsg(xchg(&s->rx.nxt, NULL));
			if (s->rx.cmp)
				freemsg(xchg(&s->rx.cmp, NULL));
			s->rx.mode = RX_MODE_HUNT;
			s->statem.daedr_state = SDT_STATE_IDLE;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, s));
		return (QR_PASSFLOW);
	}
      outstate:
	swerr();
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC int
sdl_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto discard;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, s));
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC int
sdl_error_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto discard;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, s));
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC int
sdl_stats_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto discard;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	printd(("%s: %p: <- LMI_STATS_IND\n", MOD_NAME, s));
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC int
sdl_event_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto discard;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, s));
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
sdt_recv_data(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	int err;
	if (s->statem.daedr_state == SDT_STATE_IDLE)
		return (QR_DONE);	/* discard */
	if (canputnext(s->oq)) {
		sdt_rx_block(q, s, mp);
		if (s->statem.daedr_state == SDT_STATE_CONGESTED)
			if ((err = sdt_rc_no_congestion_ind(q, s)) >= 0)
				s->statem.daedr_state = SDT_STATE_IN_SERVICE;
		return (QR_DONE);
	}
	if (s->statem.daedr_state == SDT_STATE_IN_SERVICE) {
		if ((err = sdt_rc_congestion_accept_ind(q, s)) < 0)
			return (err);
		s->statem.daedr_state = SDT_STATE_CONGESTED;
	}
	return (-EBUSY);
}

/*
 *  SDL_RECEIVED_BITS_IND
 *  -----------------------------------
 *  This is the non-preferred way of receiving data.  It is preffered that
 *  M_DATA blocks are used (above).  We just strip off the M_PROTO and requeue
 *  only the M_DATA blocks.
 */
STATIC int
sdl_received_bits_ind(queue_t *q, mblk_t *mp)
{
	mblk_t *dp;
	if ((dp = mp->b_cont) && dp->b_datap->db_type == M_DATA) {
		trace();
		return (QR_STRIP);
	}
	return (-EPROTO);
}

/*
 *  SDL_DISCONNECT_IND
 *  -----------------------------------
 */
STATIC int
sdl_disconnect_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	int err;
	sdl_disconnect_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.daedr_state == SDT_STATE_IDLE)
		goto discard;
	if (s->statem.daedt_state == SDT_STATE_IDLE)
		goto discard;
	if (s->statem.suerm_state == SDT_STATE_IN_SERVICE) {
		if ((err = sdt_lsc_link_failure_ind(q, s)) == QR_DONE) {
			s->statem.suerm_state = SDT_STATE_IDLE;
			return (QR_DONE);
		}
		return (err);
	}
	if (s->statem.aerm_state == SDT_STATE_MONITORING) {
		if ((err = sdt_iac_abort_proving_ind(q, s)) == QR_DONE) {
			s->statem.aerm_state = SDT_STATE_IDLE;
			return (QR_DONE);
		}
		return (err);
	}
	swerr();
	return (-EFAULT);
      discard:
	swerr();
	return (QR_DONE);
      outstate:
	swerr();
	return (-EPROTO);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
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
lmi_test_config(struct sdt *sdt, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_commit_config(struct sdt *sdt, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->option;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->option = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			arg->version = s->i_version;
			arg->style = s->i_style;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->i_version = arg->version;
			s->i_style = arg->style;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_test_config(s, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_commit_config(s, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			arg->state = s->i_state;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->i_state = LMI_UNUSABLE;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->statsp;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->statsp = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccstats(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	psw_t flags;
	int ret;
	(void) mp;
	spin_lock_irqsave(&s->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&s->lock, flags);
	return (ret);
}
STATIC int
lmi_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDT IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_test_config(struct sdt *s, sdt_config_t * arg)
{
	int ret = 0;
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	do {
		if (!arg->t8)
			arg->t8 = s->config.t8;
		if (!arg->Tin)
			arg->Tin = s->config.Tin;
		if (!arg->Tie)
			arg->Tie = s->config.Tie;
		if (!arg->T)
			arg->T = s->config.T;
		if (!arg->D)
			arg->D = s->config.D;
		if (!arg->Te)
			arg->Te = s->config.Te;
		if (!arg->De)
			arg->De = s->config.De;
		if (!arg->Ue)
			arg->Ue = s->config.Ue;
		if (!arg->N)
			arg->N = s->config.N;
		if (!arg->m)
			arg->m = s->config.m;
		if (!arg->b)
			arg->b = s->config.b;
		else if (arg->b != s->config.b) {
			ret = -EINVAL;
			break;
		}
	} while (0);
	spin_unlock_irqrestore(&s->lock, flags);
	return (ret);
}
STATIC int
sdt_commit_config(struct sdt *s, sdt_config_t * arg)
{
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		sdt_test_config(s, arg);
		s->config = *arg;
	}
	spin_unlock_irqrestore(&s->lock, flags);
	return (0);
}
STATIC int
sdt_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->option;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->option = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->config;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->config = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_test_config(s, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_commit_config(s, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->statem;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccmreset(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	(void) s;
	(void) mp;
	fixme(("%s: Master reset\n", MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
sdt_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->statsp;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->statsp = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->stats;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccstats(queue_t *q, mblk_t *mp)
{
	psw_t flags;
	struct sdt *s = SDT_PRIV(q);
	(void) mp;
	spin_lock_irqsave(&s->lock, flags);
	{
		bzero(&s->stats, sizeof(s->stats));
	}
	spin_unlock_irqrestore(&s->lock, flags);
	return (0);
}
STATIC int
sdt_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->notify;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->notify = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *s = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccabort(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	int ret;
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&s->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&s->lock, flags);
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
sdt_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
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
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, s, nr));
			ret = (-EINVAL);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, s, nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
	case LMI_IOC_MAGIC:
	{
		if (count < size || s->i_state == LMI_UNATTACHED) {
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
			ptrace(("%s: %p: ERROR: Unsupported SDT ioctl %d\n", MOD_NAME, s, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDT_IOC_MAGIC:
	{
		if (count < size || s->i_state == LMI_UNATTACHED) {
			ret = (-EINVAL);
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
			ptrace(("%s: ERROR: Unsupported SDT ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ret = (QR_PASSFLOW);
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
 *  Primitives from User to SDT.
 *  -----------------------------------
 */
STATIC int
sdt_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sdt *s = SDT_PRIV(q);
	ulong oldstate = s->i_state;
	if ((prim = *(ulong *) mp->b_rptr) == SDT_DAEDT_TRANSMISSION_REQ) {
		printd(("%s: %p: -> SDT_DAEDT_TRANSMISSION_REQ [%d]\n", MOD_NAME, s,
			msgdsize(mp->b_cont)));
		if ((rtn = sdt_daedt_transmission_req(q, mp)) < 0)
			s->i_state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case SDT_DAEDT_TRANSMISSION_REQ:
		printd(("%s: %p: -> SDT_DAEDT_TRANSMISSION_REQ [%d]\n", MOD_NAME, s,
			msgdsize(mp->b_cont)));
		rtn = sdt_daedt_transmission_req(q, mp);
		break;
	case SDT_DAEDT_START_REQ:
		printd(("%s: %p: -> SDT_DAEDT_START_REQ\n", MOD_NAME, s));
		rtn = sdt_daedt_start_req(q, mp);
		break;
	case SDT_DAEDR_START_REQ:
		printd(("%s: %p: -> SDT_DAEDT_START_REQ\n", MOD_NAME, s));
		rtn = sdt_daedr_start_req(q, mp);
		break;
	case SDT_AERM_START_REQ:
		printd(("%s: %p: -> SDT_AERM_START_REQ\n", MOD_NAME, s));
		rtn = sdt_aerm_start_req(q, mp);
		break;
	case SDT_AERM_STOP_REQ:
		printd(("%s: %p: -> SDT_AERM_STOP_REQ\n", MOD_NAME, s));
		rtn = sdt_aerm_stop_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIN_REQ\n", MOD_NAME, s));
		rtn = sdt_aerm_set_ti_to_tin_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIE_REQ\n", MOD_NAME, s));
		rtn = sdt_aerm_set_ti_to_tie_req(q, mp);
		break;
	case SDT_SUERM_START_REQ:
		printd(("%s: %p: -> SDT_SUERM_START_REQ\n", MOD_NAME, s));
		rtn = sdt_suerm_start_req(q, mp);
		break;
	case SDT_SUERM_STOP_REQ:
		printd(("%s: %p: -> SDT_SUERM_STOP_REQ\n", MOD_NAME, s));
		rtn = sdt_suerm_stop_req(q, mp);
		break;
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", MOD_NAME, s));
		rtn = sdt_info_req(q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", MOD_NAME, s));
		rtn = sdt_attach_req(q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", MOD_NAME, s));
		rtn = sdt_detach_req(q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", MOD_NAME, s));
		rtn = sdt_enable_req(q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", MOD_NAME, s));
		rtn = sdt_disable_req(q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", MOD_NAME, s));
		rtn = sdt_optmgmt_req(q, mp);
		break;
	default:
		swerr();
		rtn = (-EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		s->i_state = oldstate;
	return (rtn);
}

/*
 *  Primitives from SDL to SDT.
 *  -----------------------------------
 */
STATIC int
sdt_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sdt *s = SDT_PRIV(q);
	ulong oldstate = s->i_state;
	/* 
	   Fast Path */
	if ((prim = *(ulong *) mp->b_rptr) == SDL_RECEIVED_BITS_IND) {
		printd(("%s: %p: SDL_RECEIVED_BITS_IND [%d] <-\n", MOD_NAME, s,
			msgdsize(mp->b_cont)));
		if ((rtn = sdl_received_bits_ind(q, mp)) < 0)
			s->i_state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case SDL_RECEIVED_BITS_IND:
		printd(("%s: %p: SDL_RECEIVED_BITS_IND [%d] <-\n", MOD_NAME, s,
			msgdsize(mp->b_cont)));
		rtn = sdl_received_bits_ind(q, mp);
		break;
	case SDL_DISCONNECT_IND:
		printd(("%s: %p: SDL_DISCONNECT_IND <-\n", MOD_NAME, s));
		rtn = sdl_disconnect_ind(q, mp);
		break;
	case LMI_INFO_ACK:
		printd(("%s: %p: LMI_INFO_ACK <-\n", MOD_NAME, s));
		rtn = sdl_info_ack(q, mp);
		break;
	case LMI_OK_ACK:
		printd(("%s: %p: LMI_OK_ACK <-\n", MOD_NAME, s));
		rtn = sdl_ok_ack(q, mp);
		break;
	case LMI_ERROR_ACK:
		printd(("%s: %p: LMI_ERROR_ACK <-\n", MOD_NAME, s));
		rtn = sdl_error_ack(q, mp);
		break;
	case LMI_ENABLE_CON:
		printd(("%s: %p: LMI_ENABLE_CON <-\n", MOD_NAME, s));
		rtn = sdl_enable_con(q, mp);
		break;
	case LMI_DISABLE_CON:
		printd(("%s: %p: LMI_DISABLE_CON <-\n", MOD_NAME, s));
		rtn = sdl_disable_con(q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		printd(("%s: %p: LMI_OPTMGMT_ACK <-\n", MOD_NAME, s));
		rtn = sdl_optmgmt_ack(q, mp);
		break;
	case LMI_ERROR_IND:
		printd(("%s: %p: LMI_ERROR_IND <-\n", MOD_NAME, s));
		rtn = sdl_error_ind(q, mp);
		break;
	case LMI_STATS_IND:
		printd(("%s: %p: LMI_STATS_IND <-\n", MOD_NAME, s));
		rtn = sdl_stats_ind(q, mp);
		break;
	case LMI_EVENT_IND:
		printd(("%s: %p: LMI_EVENT_IND <-\n", MOD_NAME, s));
		rtn = sdl_event_ind(q, mp);
		break;
	default:
		swerr();
		rtn = (-EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		s->i_state = oldstate;
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
sdt_w_data(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	(void) s;
	printd(("%s: %p: -> M_DATA [%d]\n", MOD_NAME, s, msgdsize(mp)));
	return sdt_send_data(q, mp);
}
STATIC INLINE int
sdt_r_data(queue_t *q, mblk_t *mp)
{
	struct sdt *s = SDT_PRIV(q);
	(void) s;
	printd(("%s: %p: M_DATA [%d] <-\n", MOD_NAME, s, msgdsize(mp)));
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
sdt_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
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
	return (QR_PASSALONG);
}
STATIC INLINE int
sdt_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
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
	return (QR_PASSALONG);
}

/*
 *  =======================================================================
 *
 *  OPEN and CLOSE
 *
 *  =======================================================================
 */
/*
 *  OPEN
 *  -----------------------------------------------------------------------
 */
STATIC streamscall int
sdt_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err;
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct sdt *s;
		/* 
		   test for multiple push */
		for (s = sdt_opens; s; s = s->next) {
			if (s->u.dev.cmajor == cmajor && s->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(sdt_alloc_priv(q, &sdt_opens, devp, crp))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		/* 
		   generate immediate information request */
		if ((err = sdl_info_req(q, s)) < 0) {
			sdt_free_priv(q);
			MOD_DEC_USE_COUNT;
			return (-err);
		}
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  CLOSED
 *  -----------------------------------------------------------------------
 */
STATIC streamscall int
sdt_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sdt_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  ========================================================================
 *
 *  PRIVATE struct allocation, deallocation and cache
 *
 *  ========================================================================
 */
STATIC kmem_cache_t *sdt_priv_cachep = NULL;
STATIC int
sdt_init_caches(void)
{
	if (!sdt_priv_cachep
	    && !(sdt_priv_cachep =
		 kmem_cache_create("sdt_priv_cachep", sizeof(struct sdt), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate sdt_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	}
	printd(("%s: Allocated/selected private structure cache\n", MOD_NAME));
	return (0);
}
STATIC int
sdt_term_caches(void)
{
	if (sdt_priv_cachep) {
		if (kmem_cache_destroy(sdt_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sdt_priv_cachep.", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("sdt: destroyed sdt_priv_cachep\n"));
	}
	return (0);
}
STATIC struct sdt *
sdt_alloc_priv(queue_t *q, struct sdt **sp, dev_t *devp, cred_t *crp)
{
	struct sdt *s;
	if ((s = kmem_cache_alloc(sdt_priv_cachep, SLAB_ATOMIC))) {
		printd(("sdt: allocated module private structure\n"));
		bzero(s, sizeof(*s));
		sdt_get(s);	/* first get */
		s->u.dev.cmajor = getmajor(*devp);
		s->u.dev.cminor = getminor(*devp);
		s->cred = *crp;
		(s->oq = RD(q))->q_ptr = sdt_get(s);
		(s->iq = WR(q))->q_ptr = sdt_get(s);
		spin_lock_init(&s->qlock);	/* "sdt-queue-lock" */
		s->o_prim = &sdt_r_prim;
		s->i_prim = &sdt_w_prim;
		s->o_wakeup = &sdt_rx_wakeup;
		s->i_wakeup = &sdt_tx_wakeup;
		s->i_state = LMI_UNUSABLE;
		s->i_style = LMI_STYLE1;
		s->i_version = 1;
		spin_lock_init(&s->qlock);	/* "sdt-priv_lock" */
		if ((s->next = *sp))
			s->next->prev = &s->next;
		s->prev = sp;
		*sp = sdt_get(s);
		printd(("%s: %p: linked module private structure\n", MOD_NAME, s));
		/* 
		   configuration defaults */
		s->option = lmi_default;
		s->config = sdt_default;
		printd(("%s: %p: setting module private structure defaults\n", MOD_NAME, s));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", MOD_NAME));
	return (s);
}
STATIC void
sdt_free_priv(queue_t *q)
{
	struct sdt *s = SDT_PRIV(q);
	psw_t flags;
	ensure(s, return);
	spin_lock_irqsave(&s->lock, flags);
	{
		ss7_unbufcall((str_t *) s);
		sdt_timer_stop(s, tall);
		if (s->tx.msg && s->tx.msg != s->tx.cmp)
			freemsg(xchg(&s->tx.msg, NULL));
		if (s->tx.cmp)
			freemsg(xchg(&s->tx.cmp, NULL));
		if (s->rx.msg)
			freemsg(xchg(&s->rx.msg, NULL));
		if (s->rx.nxt)
			freemsg(xchg(&s->rx.nxt, NULL));
		if (s->rx.cmp)
			freemsg(xchg(&s->rx.cmp, NULL));
		if ((*(s->prev) = s->next))
			s->next->prev = s->prev;
		s->next = NULL;
		s->prev = &s->next;
		sdt_put(s);
		s->oq->q_ptr = NULL;
		flushq(s->oq, FLUSHALL);
		s->oq = NULL;
		sdt_put(s);
		s->iq->q_ptr = NULL;
		flushq(s->iq, FLUSHALL);
		s->iq = NULL;
		sdt_put(s);
	}
	spin_unlock_irqrestore(&s->lock, flags);
	sdt_put(s);		/* final put */
}
STATIC struct sdt *
sdt_get(struct sdt *s)
{
	atomic_inc(&s->refcnt);
	return (s);
}
STATIC void
sdt_put(struct sdt *s)
{
	if (atomic_dec_and_test(&s->refcnt)) {
		kmem_cache_free(sdt_priv_cachep, s);
		printd(("%s: %p: freed sdt private structure\n", MOD_NAME, s));
	}
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
MODULE_PARM_DESC(modid, "Module ID for the SDT module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sdt_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sdtinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
sdt_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&sdt_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
sdt_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&sdt_fmod)) < 0)
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
sdt_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&sdtinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
sdt_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&sdtinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sdtinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = sdt_init_tables())) {
		cmn_err(CE_WARN, "%s: could not intialized tables, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = sdt_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		sdt_term_tables();
		return (err);
	}
	if ((err = sdt_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		sdt_term_caches();
		sdt_term_tables();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sdtterminate(void)
{
	int err;
	if ((err = sdt_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sdt_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	if ((err = sdt_term_tables()))
		cmn_err(CE_WARN, "%s: could not terminate tables", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sdtinit);
module_exit(sdtterminate);

#endif				/* LINUX */
