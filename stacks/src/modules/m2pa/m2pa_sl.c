/*****************************************************************************

 @(#) $RCSfile: m2pa_sl.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/03/30 14:43:43 $

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

 Last Modified $Date: 2005/03/30 14:43:43 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: m2pa_sl.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/03/30 14:43:43 $"

static char const ident[] =
    "$RCSfile: m2pa_sl.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/03/30 14:43:43 $";

#include "os7/compat.h"

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

// #define _DEBUG

#define M2PA_SL_DESCRIP		"M2PA/SCTP SIGNALLING LINK (SL) STREAMS MODULE."
#define M2PA_SL_REVISION	"LfS $RCSfile: m2pa_sl.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/03/30 14:43:43 $"
#define M2PA_SL_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define M2PA_SL_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define M2PA_SL_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M2PA_SL_LICENSE		"GPL"
#define M2PA_SL_BANNER		M2PA_SL_DESCRIP		"\n" \
				M2PA_SL_REVISION	"\n" \
				M2PA_SL_COPYRIGHT	"\n" \
				M2PA_SL_DEVICE		"\n" \
				M2PA_SL_CONTACT		"\n"
#define M2PA_SL_SPLASH		M2PA_SL_DEVICE		" - " \
				M2PA_SL_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M2PA_SL_CONTACT);
MODULE_DESCRIPTION(M2PA_SL_DESCRIP);
MODULE_SUPPORTED_DEVICE(M2PA_SL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M2PA_SL_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* LINUX */

#ifdef LFS
#define M2PA_SL_MOD_ID		CONFIG_STREAMS_M2PA_SL_MODID
#define M2PA_SL_MOD_NAME	CONFIG_STREAMS_M2PA_SL_NAME
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		M2PA_SL_MOD_ID
#define MOD_NAME	M2PA_SL_MOD_NAME

STATIC struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = (1 << 15),		/* Hi water mark */
	.mi_lowat = (1 << 10),		/* Lo water mark */
};

STATIC int sl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sl_close(queue_t *, int, cred_t *);

STATIC struct qinit sl_rinit = {
	.qi_putp = ss7_oput,		/* Read put (msg from below) */
	.qi_qopen = sl_open,		/* Each open */
	.qi_qclose = sl_close,		/* Last close */
	.qi_minfo = &sl_minfo,		/* Information */
};

STATIC struct qinit sl_winit = {
	.qi_putp = ss7_iput,		/* Write put (msg from above) */
	.qi_minfo = &sl_minfo,		/* Information */
};

struct streamtab m2pa_slinfo = {
	.st_rdinit = &sl_rinit,		/* Upper read queue */
	.st_wrinit = &sl_winit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  M2PA-SL Private Structure
 *
 *  =========================================================================
 */
typedef struct sl {
	STR_DECLARATION (struct sl);	/* stream declaration */
	mblk_t *rbuf;			/* reassembly buffer */
	uint rmsu;			/* received unacked msus */
	uint tmsu;			/* transmitted unacked msus */
	uint rack;			/* received acked msus */
	uint tack;			/* transmitted acked msus */
	uint fsnr;			/* received msu seq number */
	uint bsnr;			/* received msu seq number */
	uint fsnx;			/* expected msu seq number */
	uint fsnt;			/* transmitted msu seq number */
	uint back;			/* bad acks */
	uint bbsn;			/* bad bsn */
	uint bfsn;			/* bad fsn */
	lmi_option_t option;		/* protocol and variant options */
	bufq_t rb;			/* receive buffer */
	bufq_t tb;			/* transmission buffer */
	bufq_t rtb;			/* retransmission buffer */
	struct {
		sl_timers_t timers;	/* SL protocol timers */
		sl_config_t config;	/* SL configuration options */
		sl_statem_t statem;	/* state machine variables */
		sl_notify_t notify;	/* SL notification options */
		sl_stats_t stats;	/* SL statistics */
		sl_stats_t stamp;	/* SL statistics timestamps */
		sl_stats_t statsp;	/* SL statistics periods */
	} sl;
	struct {
		sdt_timers_t timers;	/* SDT timers */
		sdt_config_t config;	/* SDT configuration options */
	} sdt;
	struct {
		sdl_timers_t timers;	/* SDL timers */
		sdl_config_t config;	/* SDL configuration options */
	} sdl;
} sl_t;
#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

STATIC struct sl *sl_alloc_priv(queue_t *q, struct sl **slp, dev_t *devp, cred_t *crp);
STATIC void sl_free_priv(queue_t *q);
STATIC struct sl *sl_get(struct sl *sl);
STATIC void sl_put(struct sl *sl);

#define M2PA_VERSION_DRAFT3	0x30
#define M2PA_VERSION_DRAFT3_1	0x31
#define M2PA_VERSION_DRAFT4	0x40
#define M2PA_VERSION_DRAFT4_1	0x41
#define M2PA_VERSION_DRAFT4_9	0x49
#define M2PA_VERSION_DRAFT5	0x50
#define M2PA_VERSION_DRAFT5_1	0x51
#define M2PA_VERSION_DRAFT6	0x60
#define M2PA_VERSION_DRAFT6_1	0x61
#define M2PA_VERSION_DRAFT6_9	0x69
#define M2PA_VERSION_DRAFT10	0xa0
#define M2PA_VERSION_DRAFT11	0xb0
#define M2PA_VERSION_DEFAULT	M2PA_VERSION_DRAFT11

/*
 *  =========================================================================
 *
 *  M2PA PDU Message Definitions
 *
 *  =========================================================================
 */
#define M2PA_PPI		5
#define M2PA_MESSAGE_CLASS	11
#define M2PA_VERSION		1
#define M2PA_MTYPE_DATA		1
#define M2PA_MTYPE_STATUS	2
#define M2PA_MTYPE_PROVING	3
#define M2PA_MTYPE_ACK		4
#define M2PA_DATA_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_DATA)
#define M2PA_STATUS_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_STATUS)
#define M2PA_PROVING_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_PROVING)
#define M2PA_ACK_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_ACK)
#define M2PA_STATUS_STREAM	0
#define M2PA_DATA_STREAM	1
#define M2PA_STATUS_ALIGNMENT			(__constant_htonl(1))
#define M2PA_STATUS_PROVING_NORMAL		(__constant_htonl(2))
#define M2PA_STATUS_PROVING_EMERGENCY		(__constant_htonl(3))
#define M2PA_STATUS_IN_SERVICE			(__constant_htonl(4))
#define M2PA_STATUS_PROCESSOR_OUTAGE		(__constant_htonl(5))
#define M2PA_STATUS_PROCESSOR_OUTAGE_ENDED	(__constant_htonl(6))
#define M2PA_STATUS_BUSY			(__constant_htonl(7))
#define M2PA_STATUS_BUSY_ENDED			(__constant_htonl(8))
#define	M2PA_STATUS_OUT_OF_SERVICE		(__constant_htonl(9))
#define M2PA_STATUS_ACK				(__constant_htonl(10))

#ifndef PAD4
#define PAD4(__x) (((__x)+0x3)&~0x3)
#endif

/*
 *  =========================================================================
 *
 *  OUTPUT Events
 *
 *  =========================================================================
 *  -------------------------------------------------------------------------
 *
 *  SL Provider (M2PA) -> SL User Primitives
 *
 *  -------------------------------------------------------------------------
 *
 *  LMI_INFO_ACK
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sl->i_state;
		p->lmi_max_sdu = -1;
		p->lmi_min_sdu = 0;
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE1;
		printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_OK_ACK
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_ok_ack(queue_t *q, struct sl *sl, long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((lmi_ok_ack_t *) mp->b_wptr)++;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl->i_state) {
		case LMI_ATTACH_PENDING:
			sl->i_state = LMI_DISABLED;
			break;
		case LMI_DETACH_PENDING:
			sl->i_state = LMI_UNATTACHED;
			break;
			/* 
			   default is don't change state */
		}
		p->lmi_state = sl->i_state;
		printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_ERROR_ACK
 *  ---------------------------------------------
 */
STATIC int
lmi_error_ack(queue_t *q, struct sl *sl, long prim, long err)
{
	mblk_t *mp;
	lmi_error_ack_t *p;
	switch (err) {
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
	case QR_LOOP:
	case QR_PASSALONG:
	case QR_PASSFLOW:
	case QR_DISABLE:
		ptrace(("%s: %p: WARNING: Shouldn't pass Q returns to m_error_reply\n",
			MOD_NAME, sl));
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_error_primitive = prim;
		switch (sl->i_state) {
		case LMI_ATTACH_PENDING:
			sl->i_state = LMI_UNATTACHED;
			break;
		case LMI_DETACH_PENDING:
			sl->i_state = LMI_DISABLED;
			break;
		case LMI_ENABLE_PENDING:
			sl->i_state = LMI_DISABLED;
			break;
		case LMI_DISABLE_PENDING:
			sl->i_state = LMI_ENABLED;
			break;
			/* 
			 *  Default is not to change state.
			 */
		}
		p->lmi_state = sl->i_state;
		printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_enable_con(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	ensure(sl->i_state == LMI_ENABLE_PENDING, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = sl->i_state = LMI_ENABLED;
		printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_disable_con(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	ensure(sl->i_state == LMI_DISABLE_PENDING, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = sl->i_state = LMI_DISABLED;
		printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  ---------------------------------------------
 */
#if 0
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, struct sl *sl, ulong flags, void *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((lmi_optmgmt_ack_t *) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_ERROR_IND
 *  ---------------------------------------------
 */
#if 0
STATIC INLINE int
lmi_error_ind(queue_t *q, struct sl *sl, long err)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((lmi_error_ind_t *) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_state = sl->i_state;
		printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_STATS_IND
 *  ---------------------------------------------
 */
#if 0
STATIC INLINE int
lmi_stats_ind(queue_t *q, struct sl *sl, ulong interval, ulong timestamp)
{
	mblk_t *mp;
	lmi_stats_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((lmi_stats_ind_t *) mp->b_wptr)++;
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = interval;
		p->lmi_timestamp = timestamp;
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_EVENT_IND
 *  ---------------------------------------------
 */
STATIC int
lmi_event_ind(queue_t *q, struct sl *sl, ulong oid, ulong severity, void *inf_ptr, size_t inf_len)
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
		p->lmi_severity = severity;
		bcopy(mp->b_wptr, inf_ptr, inf_len);
		mp->b_wptr += inf_len;
		printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_PDU_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_pdu_ind(queue_t *q, struct sl *sl, mblk_t *dp)
{
	mblk_t *mp;
	sl_pdu_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_PDU_IND;
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_PDU_IND [%d]\n", MOD_NAME, sl, msgdsize(mp)));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTED_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_link_congested_ind(queue_t *q, struct sl *sl, ulong cong, ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_timestamp = jiffies;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTED_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_link_congestion_ceased_ind(queue_t *q, struct sl *sl, ulong cong, ulong disc)
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
		printd(("%s: %p: <- SL_LINK_CONGESTION_CEASED_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_retrieved_message_ind(queue_t *q, struct sl *sl, mblk_t *dp)
{
	mblk_t *mp;
	sl_retrieved_msg_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_RETRIEVED_MESSAGE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_retrieval_complete_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_retrieval_comp_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		printd(("%s: %p: <- SL_RETRIEVAL_COMPLETE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RB_CLEARED_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_rb_cleared_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_rb_cleared_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RB_CLEARED_IND;
		printd(("%s: %p: <- SL_RB_CLEARED_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_bsnt_ind(queue_t *q, struct sl *sl, ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_BSNT_IND;
		p->sl_bsnt = bsnt;
		printd(("%s: %p: <- SL_BSNT_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_IN_SERVICE_IND
 *  ---------------------------------------------
 */
STATIC int
sl_in_service_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_in_service_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_IN_SERVICE_IND;
		printd(("%s: %p: <- SL_IN_SERVICE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_out_of_service_ind(queue_t *q, struct sl *sl, ulong reason)
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
		printd(("%s: %p: <- SL_OUT_OF_SERVICE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  ---------------------------------------------
 */
STATIC int
sl_remote_processor_outage_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_rem_proc_out_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = jiffies;
		printd(("%s: %p: <- SL_PROCESSOR_OUTAGE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  ---------------------------------------------
 */
STATIC int
sl_remote_processor_recovered_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_rem_proc_recovered_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = jiffies;
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_RECOVERED_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RTB_CLEARED_IND
 *  ---------------------------------------------
 */
STATIC int
sl_rtb_cleared_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_rtb_cleared_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RTB_CLEARED_IND;
		printd(("%s: %p: <- SL_RTB_CLEARED_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_retrieval_not_possible_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_retrieval_not_poss_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		printd(("%s: %p: <- SL_RETRIEVAL_NOT_POSSIBLE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  ---------------------------------------------
 */
#if 0
STATIC INLINE int
sl_bsnt_not_retrievable_ind(queue_t *q, struct sl *sl, ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_not_retr_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((sl_bsnt_not_retr_ind_t *) mp->b_wptr)++;
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		p->sl_bsnt = bsnt;
		printd(("%s: %p: <- SL_BSNT_NOT_RETRIEVABLE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/*
 *  -------------------------------------------------------------------------
 *
 *  NPI User (M2PA) -> NPI Provider (SCTP) Primitives
 *
 *  -------------------------------------------------------------------------
 *
 *  N_DATA_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
n_data_req(queue_t *q, struct sl *sl, ulong flags, void *qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_data_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = N_DATA_REQ;
		p->DATA_xfer_flags = flags;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		mp->b_cont = dp;
		printd(("%s: %p: N_DATA_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  N_EXDATA_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
n_exdata_req(queue_t *q, struct sl *sl, void *qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = N_EXDATA_REQ;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		mp->b_cont = dp;
		printd(("%s: %p: N_EXDATA_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  M2PA State Machines
 *
 *  =========================================================================
 */
#define MF_LPO			0x00000001
#define MF_RPO			0x00000002
#define MF_LOC_INS		0x00000004
#define MF_REM_INS		0x00000008
#define MF_LOC_BUSY		0x00000010
#define MF_REM_BUSY		0x00000020
#define MF_LOC_EMERG		0x00000040
#define MF_REM_EMERG		0x00000080
#define MF_RECV_MSU		0x00000100
#define MF_SEND_MSU		0x00000200
#define MF_CONG_ACCEPT		0x00000400
#define MF_CONG_DISCARD		0x00000800
#define MF_RTB_FULL		0x00001000
#define MF_L3_CONG_DETECT	0x00002000
#define MF_L2_CONG_DETECT	0x00004000
#define MF_CONTINUE		0x00008000
#define MF_CLEAR_RTB		0x00010000
#define MF_NEED_FLUSH		0x00020000
#define MF_WAIT_SYNC		0x00040000

#define MS_POWER_OFF		0
#define MS_OUT_OF_SERVICE	1
#define MS_NOT_ALIGNED		2
#define MS_ALIGNED		3
#define MS_PROVING		4
#define MS_ALIGNED_NOT_READY	5
#define MS_ALIGNED_READY	6
#define MS_IN_SERVICE		7
#define MS_PROCESSOR_OUTAGE	8

#ifdef _DEBUG
const char *
sl_state_name(ulong state)
{
	switch (state) {
	case MS_POWER_OFF:
		return ("MS_POWER_OFF");
	case MS_OUT_OF_SERVICE:
		return ("MS_OUT_OF_SERVICE");
	case MS_NOT_ALIGNED:
		return ("MS_NOT_ALIGNED");
	case MS_ALIGNED:
		return ("MS_ALIGNED");
	case MS_PROVING:
		return ("MS_PROVING");
	case MS_ALIGNED_NOT_READY:
		return ("MS_ALIGNED_NOT_READY");
	case MS_ALIGNED_READY:
		return ("MS_ALIGNED_READY");
	case MS_IN_SERVICE:
		return ("MS_IN_SERVICE");
	case MS_PROCESSOR_OUTAGE:
		return ("MS_PROCESSOR_OUTAGE");
	default:
		return ("(unknown)");
	}
}
STATIC void
sl_set_state(struct sl *sl, ulong newstate)
{
	ulong oldstate = sl->state;
	printd(("%s: %p: %s <- %s\n", MOD_NAME, sl, sl_state_name(newstate),
		sl_state_name(oldstate)));
	sl->state = newstate;
}
#else
STATIC INLINE void
sl_set_state(struct sl *sl, ulong newstate)
{
	sl->state = newstate;
}
#endif
STATIC INLINE ulong
sl_get_state(struct sl *sl)
{
	return sl->state;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL Peer (M2PA) -> SL Peer Sent Messages
 *
 *  -------------------------------------------------------------------------
 */
static const char filler[] = {
	0x6f, 0x43, 0x79, 0x70, 0x69, 0x72, 0x68, 0x67, 0x20, 0x74, 0x63,
	0x28, 0x20, 0x29, 0x39, 0x31, 0x37, 0x39, 0x32, 0x2d, 0x30, 0x30,
	0x20, 0x34, 0x4f, 0x20, 0x65, 0x70, 0x53, 0x6e, 0x37, 0x53, 0x43,
	0x20, 0x72, 0x6f, 0x6f, 0x70, 0x61, 0x72, 0x69, 0x74, 0x6e, 0x6f,
	0x20, 0x2e, 0x41, 0x20, 0x6c, 0x6c, 0x52, 0x20, 0x67, 0x69, 0x74,
	0x68, 0x20, 0x73, 0x65, 0x52, 0x65, 0x73, 0x76, 0x72, 0x64, 0x65,
	0x0a, 0x2e, 0x6e, 0x55, 0x75, 0x61, 0x68, 0x74, 0x72, 0x6f, 0x7a,
	0x69, 0x64, 0x65, 0x64, 0x20, 0x70, 0x75, 0x69, 0x6c, 0x61, 0x63,
	0x69, 0x74, 0x6e, 0x6f, 0x70, 0x20, 0x6f, 0x72, 0x69, 0x68, 0x69,
	0x62, 0x65, 0x74, 0x2e, 0x64, 0x00, 0x0a,
};

/*
 *
 *  M2PA SEND PROVING
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_send_proving(queue_t *q, struct sl *sl, uint32_t status)
{
	int err;
	mblk_t *mp;
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, M2PA_PPI, M2PA_DATA_STREAM, };
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		if ((mp = ss7_allocb(q, 2 * sizeof(uint32_t) + sizeof(filler), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_PROVING_MESSAGE;
			*(uint32_t *) mp->b_wptr++ =
			    __constant_htonl(2 * sizeof(uint32_t) + sizeof(filler));
			bcopy(filler, mp->b_wptr, sizeof(filler));
			mp->b_wptr += sizeof(filler);
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		/* 
		   draft-ietf-sigtran-m2pa-04.txt does not have a separate Proving message: it uses 
		   the status proving message for sending proving data will fill bytes. */
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t) + sizeof(filler), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ =
			    __constant_htonl(4 * sizeof(uint32_t) + sizeof(filler));
			*(uint16_t *) mp->b_wptr++ = htons(sl->fsnr);
			*(uint16_t *) mp->b_wptr++ = htons(sl->fsnt);
			*(uint32_t *) mp->b_wptr++ = status;
			bcopy(filler, mp->b_wptr, sizeof(filler));
			mp->b_wptr += sizeof(filler);
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	default:
		/* 
		   It looks like the sequence numbers stuck. */
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t) + sizeof(filler), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ =
			    __constant_htonl(5 * sizeof(uint32_t) + sizeof(filler));
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnr & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnt & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = status;
			bcopy(filler, mp->b_wptr, sizeof(filler));
			mp->b_wptr += sizeof(filler);
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT6_9:
		/* 
		   It is anticipated that draft 7 will not include FSN or BSN in the status
		   messages. */
		if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t) + sizeof(filler), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ =
			    __constant_htonl(3 * sizeof(uint32_t) + sizeof(filler));
			*(uint32_t *) mp->b_wptr++ = status;
			bcopy(filler, mp->b_wptr, sizeof(filler));
			mp->b_wptr += sizeof(filler);
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	}
}

/*
 *  M2PA SEND STATUS
 *  ---------------------------------------------
 */
STATIC int
sl_send_status(queue_t *q, struct sl *sl, uint32_t status)
{
	int err;
	mblk_t *mp;
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, M2PA_PPI, M2PA_STATUS_STREAM, };
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(3 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = status;
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		/* 
		   draft-ietf-sigtran-m2pa-04.txt has sequence numbers in an M2PA-specific header. */
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(4 * sizeof(uint32_t));
			*(uint16_t *) mp->b_wptr++ = htons(sl->fsnr);
			*(uint16_t *) mp->b_wptr++ = htons(sl->fsnt);
			*(uint32_t *) mp->b_wptr++ = status;
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(5 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnr & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnt & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = status;
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT6_9:
		/* 
		   It is anticipated that draft 7 will not include FSN or BSN in the status
		   messages. */
		if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(3 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = status;
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(5 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnr & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnt & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = status;
			switch (status) {
			case M2PA_STATUS_PROCESSOR_OUTAGE:
			case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
				qos.sid = M2PA_DATA_STREAM;
				break;
			case M2PA_STATUS_IN_SERVICE:
				switch (sl_get_state(sl)) {
				case MS_PROCESSOR_OUTAGE:
					/* 
					   Should only really be sent on data stream when used for
					   processor outage resynchronization. */
					qos.sid = M2PA_DATA_STREAM;
					break;
				default:
					qos.sid = M2PA_STATUS_STREAM;
					break;
				}
				break;
			default:
				qos.sid = M2PA_STATUS_STREAM;
				break;
			}
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	}
}

/*
 *  M2PA SEND ACK
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_send_ack(queue_t *q, struct sl *sl)
{
	int err;
	mblk_t *mp;
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, M2PA_PPI, M2PA_DATA_STREAM, };
	if (!sl->rack)
		return (0);
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
		/* 
		   draft-ietf-sigtran-m2pa-03.txt does not have a separate acknowledgement message. 
		   Acknowledgement is done at the SCTP level only. */
		return (0);
	case M2PA_VERSION_DRAFT3_1:
		if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_ACK_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(3 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = htonl(sl->rack);
			if ((err = n_exdata_req(q, sl, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4:
		/* 
		   draft-ietf-sigtran-m2pa-04.txt again does not have a separate acknowledgement
		   message.  There is no way to acknowledge data without sending status or data in
		   M2PA-04. */
		return (0);
	case M2PA_VERSION_DRAFT4_1:
		/* 
		   This extension uses the otherwise useless "In Service" state message from
		   draft-ietf-sigtran-m2pa-04.txt as an acknowledgement message (like a FISU).
		   These messages are sent expedited to speed them up, but that means they may
		   arrive out of order and, therefore, some sequence numbers should be ignored by
		   the receiver. */
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(4 * sizeof(uint32_t));
			*(uint16_t *) mp->b_wptr++ = htons(sl->fsnr);
			*(uint16_t *) mp->b_wptr++ = htons(sl->fsnt);
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_ACK;
			if ((err = n_exdata_req(q, sl, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4_9:
		/* 
		   This is the same as extension 4.1, except that it uses the larger 32-bit
		   sequence numbers expected to appear in draft-ietf-sigtran-m2pa-05.txt. */
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(5 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnr);
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnt);
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_ACK;
			if ((err = n_exdata_req(q, sl, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT5_1:
		/* 
		   Draft 5 allows acknowledgement with DATA as well as STATUS IN_SERVICE message.
		   Draft 5 incorrectly indicates that the FSN is to be incremented. */
		if (sl->tb.q_msgs && (sl->flags & MF_SEND_MSU) && !(sl->flags & MF_RTB_FULL)) {
			printd(("%s: %p: Delaying ack for piggyback\n", MOD_NAME, sl));
			return (QR_DONE);	/* piggyback ack on outgoing message */
		}
		printd(("%s: %p: Sending separate ack\n", MOD_NAME, sl));
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(5 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnr & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnt & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = M2PA_STATUS_IN_SERVICE;
			if ((err = n_exdata_req(q, sl, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
		/* 
		   draft-ietf-signtran-m2pa-06.txt and the anticipated draft 7 wil acknowledge
		   messages with an empty data message.  Draft 6 incorrectly indicatest that the
		   FSN is to be incremented. */
		if (sl->tb.q_msgs && (sl->flags & MF_SEND_MSU) && !(sl->flags & MF_RTB_FULL)) {
			printd(("%s: %p: Delaying ack for piggyback\n", MOD_NAME, sl));
			return (QR_DONE);	/* piggyback ack on outgoing message */
		}
		printd(("%s: %p: Sending separate ack\n", MOD_NAME, sl));
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_DATA_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(4 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnr & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnt & 0xffffff);
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freemsg(mp);
			return (err);
		}
		return (-ENOBUFS);
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
		/* 
		   Acknowledgements must be sent immediately on the next outgoing message.  We
		   check to see if there is a pending MSU waiting to be sent that will actually be
		   sent during this pass of the state machine.  If there is one, we will piggyback
		   so we return now.  If there is not an MSU that will be sent on this pass, we
		   send the separate ack.  Note that we do not delay whatsoever waiting for an MSU: 
		   either one is ready now or we will generate a separate ack immediately.  Let
		   SCTP worry about Nagling and bundling. */
		if (sl->tb.q_msgs && (sl->flags & MF_SEND_MSU) && !(sl->flags & MF_RTB_FULL)) {
			printd(("%s: %p: Delaying ack for piggyback\n", MOD_NAME, sl));
			return (QR_DONE);	/* piggyback ack on outgoing message */
		}
		printd(("%s: %p: Sending separate ack\n", MOD_NAME, sl));
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr++ = M2PA_DATA_MESSAGE;
			*(uint32_t *) mp->b_wptr++ = __constant_htonl(4 * sizeof(uint32_t));
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnr & 0xffffff);
			*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnt & 0xffffff);
			if ((err = n_data_req(q, sl, 0, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freemsg(mp);
			return (err);
		}
		return (-ENOBUFS);
	}
}

/*
 *  M2PA SEND DATA
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_send_data(queue_t *q, struct sl *sl, mblk_t *dp)
{
	int err;
	mblk_t *mp, *db;
	ulong rcpt = sl->i_version < M2PA_VERSION_DRAFT3_1 ? N_RC_FLAG : 0;
	size_t dlen = msgdsize(dp);
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, M2PA_PPI, M2PA_DATA_STREAM, };
	if ((db = dupmsg(dp))) {
		switch (sl->i_version) {
		case M2PA_VERSION_DRAFT3:
		case M2PA_VERSION_DRAFT3_1:
			if ((mp = ss7_allocb(q, 2 * sizeof(uint32_t), BPRI_MED))) {
				mp->b_datap->db_type = M_DATA;
				*(uint32_t *) mp->b_wptr++ = M2PA_DATA_MESSAGE;
				*(uint32_t *) mp->b_wptr++ = htonl(dlen + 2 * sizeof(uint32_t));
				mp->b_cont = db;
				pullupmsg(mp, -1);
				if ((err = n_data_req(q, sl, rcpt, &qos, sizeof(qos), mp)) >= 0)
					return (0);
				freemsg(mp);
				return (err);
			}
			break;
		case M2PA_VERSION_DRAFT4:
		case M2PA_VERSION_DRAFT4_1:
			if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t), BPRI_MED))) {
				mp->b_datap->db_type = M_DATA;
				*(uint32_t *) mp->b_wptr++ = M2PA_DATA_MESSAGE;
				*(uint32_t *) mp->b_wptr++ = htonl(dlen + 3 * sizeof(uint32_t));
				*(uint16_t *) mp->b_wptr++ = htons(sl->fsnr);
				*(uint16_t *) mp->b_wptr++ = htons(sl->fsnt + 1);
				mp->b_cont = db;
				pullupmsg(mp, -1);
				if ((err = n_data_req(q, sl, rcpt, &qos, sizeof(qos), mp)) >= 0)
					return (0);
				freemsg(mp);
				return (err);
			}
			break;
		case M2PA_VERSION_DRAFT4_9:
		case M2PA_VERSION_DRAFT5:
		case M2PA_VERSION_DRAFT6:
		case M2PA_VERSION_DRAFT6_1:
		case M2PA_VERSION_DRAFT6_9:
		default:
		case M2PA_VERSION_DRAFT10:
		case M2PA_VERSION_DRAFT11:
			if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
				mp->b_datap->db_type = M_DATA;
				*(uint32_t *) mp->b_wptr++ = M2PA_DATA_MESSAGE;
				*(uint32_t *) mp->b_wptr++ = htonl(dlen + 4 * sizeof(uint32_t));
				*(uint32_t *) mp->b_wptr++ = htonl(sl->fsnr & 0xffffff);
				*(uint32_t *) mp->b_wptr++ = htonl((sl->fsnt + 1) & 0xffffff);
				mp->b_cont = db;
				pullupmsg(mp, -1);
				if ((err = n_data_req(q, sl, rcpt, &qos, sizeof(qos), mp)) >= 0)
					return (0);
				freemsg(mp);
				return (err);
			}
			break;
		}
		freemsg(db);
	}
	return (-ENOBUFS);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  Timers
 *
 *  ------------------------------------------------------------------------
 */
enum { tall, t1, t2, t3, t4, t5, t6, t7, t8, t9 };

STATIC int sl_t1_timeout(struct sl *);
STATIC void
sl_t1_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t1", MOD_NAME, &((struct sl *) data)->sl.timers.t1,
		       (int (*)(struct head *)) &sl_t1_timeout, &sl_t1_expiry);
}
STATIC void
sl_stop_timer_t1(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t1", MOD_NAME, &sl->sl.timers.t1);
}
STATIC void
sl_start_timer_t1(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t1", MOD_NAME, &sl->sl.timers.t1,
			&sl_t1_expiry, sl->sl.config.t1);
};

STATIC int sl_t2_timeout(struct sl *);
STATIC void
sl_t2_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t2", MOD_NAME, &((struct sl *) data)->sl.timers.t2,
		       (int (*)(struct head *)) &sl_t2_timeout, &sl_t2_expiry);
}
STATIC void
sl_stop_timer_t2(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t2", MOD_NAME, &sl->sl.timers.t2);
}
STATIC void
sl_start_timer_t2(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t2", MOD_NAME, &sl->sl.timers.t2,
			&sl_t2_expiry, sl->sl.config.t2);
};

STATIC int sl_t3_timeout(struct sl *);
STATIC void
sl_t3_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t3", MOD_NAME, &((struct sl *) data)->sl.timers.t3,
		       (int (*)(struct head *)) &sl_t3_timeout, &sl_t3_expiry);
}
STATIC void
sl_stop_timer_t3(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t3", MOD_NAME, &sl->sl.timers.t3);
}
STATIC void
sl_start_timer_t3(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t3", MOD_NAME, &sl->sl.timers.t3,
			&sl_t3_expiry, sl->sl.config.t3);
};

STATIC int sl_t4_timeout(struct sl *);
STATIC void
sl_t4_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t4", MOD_NAME, &((struct sl *) data)->sl.timers.t4,
		       (int (*)(struct head *)) &sl_t4_timeout, &sl_t4_expiry);
}
STATIC void
sl_stop_timer_t4(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t4", MOD_NAME, &sl->sl.timers.t4);
}
STATIC void
sl_start_timer_t4(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t4", MOD_NAME, &sl->sl.timers.t4,
			&sl_t4_expiry,
			(sl->flags & (MF_LOC_EMERG | MF_REM_EMERG)) ? sl->sl.config.t4e : sl->sl.
			config.t4n);
};

#if 0
STATIC int sl_t5_timeout(struct sl *);
STATIC void
sl_t5_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t5", MOD_NAME, &((struct sl *) data)->sl.timers.t5,
		       (int (*)(struct head *)) &sl_t5_timeout, &sl_t5_expiry);
}
STATIC void
sl_stop_timer_t5(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t5", MOD_NAME, &sl->sl.timers.t5);
}
STATIC void
sl_start_timer_t5(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t5", MOD_NAME, &sl->sl.timers.t5,
			&sl_t5_expiry, sl->sl.config.t5);
};
#endif

STATIC int sl_t6_timeout(struct sl *);
STATIC void
sl_t6_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t6", MOD_NAME, &((struct sl *) data)->sl.timers.t6,
		       (int (*)(struct head *)) &sl_t6_timeout, &sl_t6_expiry);
}
STATIC void
sl_stop_timer_t6(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t6", MOD_NAME, &sl->sl.timers.t6);
}
STATIC void
sl_start_timer_t6(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t6", MOD_NAME, &sl->sl.timers.t6,
			&sl_t6_expiry, sl->sl.config.t6);
};

STATIC int sl_t7_timeout(struct sl *);
STATIC void
sl_t7_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t7", MOD_NAME, &((struct sl *) data)->sl.timers.t7,
		       (int (*)(struct head *)) &sl_t7_timeout, &sl_t7_expiry);
}
STATIC void
sl_stop_timer_t7(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t7", MOD_NAME, &sl->sl.timers.t7);
}
STATIC void
sl_start_timer_t7(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t7", MOD_NAME, &sl->sl.timers.t7,
			&sl_t7_expiry, sl->sl.config.t7);
};

#if 0
STATIC int sl_t8_timeout(struct sl *);
STATIC void
sl_t8_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t8", MOD_NAME, &((struct sl *) data)->sdt.timers.t8,
		       (int (*)(struct head *)) &sl_t8_timeout, &sl_t8_expiry);
}
STATIC void
sl_stop_timer_t8(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t8", MOD_NAME, &sl->sdt.timers.t8);
}
STATIC void
sl_start_timer_t8(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t8", MOD_NAME, &sl->sdt.timers.t8,
			&sl_t8_expiry, sl->sdt.config.t8);
};
#endif

STATIC int sl_t9_timeout(struct sl *);
STATIC void
sl_t9_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t9", MOD_NAME, &((struct sl *) data)->sdl.timers.t9,
		       (int (*)(struct head *)) &sl_t9_timeout, &sl_t9_expiry);
}
STATIC void
sl_stop_timer_t9(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t9", MOD_NAME, &sl->sdl.timers.t9);
}
STATIC void
sl_start_timer_t9(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t9", MOD_NAME, &sl->sdl.timers.t9, &sl_t9_expiry, 2);
};

STATIC INLINE void
__sl_timer_stop(struct sl *sl, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t1:
		sl_stop_timer_t1(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t2:
		sl_stop_timer_t2(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t3:
		sl_stop_timer_t3(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t4:
		sl_stop_timer_t4(sl);
		if (single)
			break;
		/* 
		   fall through */
#if 0
	case t5:
		sl_stop_timer_t5(sl);
		if (single)
			break;
		/* 
		   fall through */
#endif
	case t6:
		sl_stop_timer_t6(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t7:
		sl_stop_timer_t7(sl);
		if (single)
			break;
		/* 
		   fall through */
#if 0
	case t8:
		sl_stop_timer_t8(sl);
		if (single)
			break;
		/* 
		   fall through */
#endif
	case t9:
		sl_stop_timer_t9(sl);
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
sl_timer_stop(struct sl *sl, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		__sl_timer_stop(sl, t);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
}
STATIC INLINE void
sl_timer_start(struct sl *sl, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		__sl_timer_stop(sl, t);
		switch (t) {
		case t1:
			sl_start_timer_t1(sl);
			break;
		case t2:
			sl_start_timer_t2(sl);
			break;
		case t3:
			sl_start_timer_t3(sl);
			break;
		case t4:
			sl_start_timer_t4(sl);
			break;
#if 0
		case t5:
			sl_start_timer_t5(sl);
			break;
#endif
		case t6:
			sl_start_timer_t6(sl);
			break;
		case t7:
			sl_start_timer_t7(sl);
			break;
#if 0
		case t8:
			sl_start_timer_t8(sl);
			break;
#endif
		case t9:
			sl_start_timer_t9(sl);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Error Monitoring
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sl_suerm_start(queue_t *q, struct sl *sl)
{
	/* 
	   SCTP does this for us */
	return;
}

STATIC INLINE void
sl_suerm_stop(queue_t *q, struct sl *sl)
{
	/* 
	   SCTP does this for us */
	return;
}

STATIC INLINE void
sl_aerm_start(queue_t *q, struct sl *sl)
{
	printd(("%s: %p: Start Proving...\n", MOD_NAME, sl));
	sl_timer_start(sl, t9);
}

STATIC INLINE void
sl_aerm_stop(queue_t *q, struct sl *sl)
{
	sl_timer_stop(sl, t9);
	printd(("%s: %p: Stop Proving...\n", MOD_NAME, sl));
}

STATIC int
sl_t9_timeout(struct sl *sl)
{
	int proving = !(sl->option.popt & SS7_POPT_NOPR);
	if (sl_get_state(sl) == MS_PROVING) {
		int err;
		queue_t *q = sl->iq;
		/* 
		   We use the t9 tick timer to determine how fast to send proving messages.  We
		   send a proving message once every t9 during the proving period.  To adjust the
		   bandwidth of the proving messages, adjust the t9 timer or the size of the
		   filler. */
		if (proving) {
			if (sl->flags & MF_LOC_EMERG)
				err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_EMERGENCY);
			else
				err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_NORMAL);
			if (err)
				return (err);
			sl_timer_start(sl, t9);
		}
		return (0);
	}
	ptrace(("%s: %p: Received timeout t9 in uexpected state %lu\n", MOD_NAME, sl,
		sl_get_state(sl)));
	return (-EPROTO);
}

STATIC int sl_lsc_out_of_service(queue_t *q, struct sl *sl, ulong reason);

#if 0
STATIC INLINE int
sl_iac_abort_proving(queue_t *q, struct sl *sl)
{
	int err;
	if (sl_get_state(sl) == MS_PROVING) {
		sl->sl.statem.Cp++;
		sl_timer_stop(sl, t4);
		sl_aerm_stop(q, sl);
		if (sl->sl.statem.Cp == sl->sl.config.M) {
			if (sl->sl.notify.events & SL_EVT_FAIL_ALIGNMENT)
				if ((err =
				     lmi_event_ind(q, sl, SL_EVT_FAIL_ALIGNMENT, 0, NULL, 0)) < 0)
					return (err);
			if ((err =
			     sl_lsc_out_of_service(q, sl, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)) < 0)
				return (err);
			ptrace(("%s: %p: Link failed: Alignement not possible\n", MOD_NAME, sl));
			sl->sl.stats.sl_fail_align_or_proving++;
			return (0);
		}
		sl_timer_start(sl, t4);
		sl_aerm_start(q, sl);
		return (0);
	}
	ptrace(("%s: Received signal iac-abort-proving in unexpected state %u.\n", MOD_NAME,
		sl_get_state(sl)));
	return (-EFAULT);
}
#endif

/*
 *  -----------------------------------------------------------------------
 *
 *  STATE MACHINES:- The order of the state machine primitives below may seem
 *  somewhat disorganized at first glance; however, they have been ordered by
 *  dependency because they are all inline functions.  You see, the L2 state
 *  machine does not required multiple threading because there is never a
 *  requirement to invoke the individual state machines concurrently.  This
 *  works out good for the driver, because a primitive action expands inline
 *  to the necessary procedure, while the source still takes the appearance of
 *  the SDL diagrams in the SS7 specification for inspection and debugging.
 *
 *  -----------------------------------------------------------------------
 */
/*
 *  -----------------------------------------------------------------------
 *
 *  Duration Statistics
 *
 *  -----------------------------------------------------------------------
 */
STATIC void
sl_is_stats(queue_t *q, struct sl *sl)
{
	if (sl->sl.stamp.sl_dur_unavail)
		sl->sl.stats.sl_dur_unavail += jiffies - xchg(&sl->sl.stamp.sl_dur_unavail, 0);
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
	if (sl->sl.stamp.sl_dur_unavail_failed)
		sl->sl.stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_failed, 0);
	sl->sl.stamp.sl_dur_in_service = jiffies;
}
STATIC void
sl_oos_stats(queue_t *q, struct sl *sl)
{
	if (sl->sl.stamp.sl_dur_in_service)
		sl->sl.stats.sl_dur_in_service +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_in_service, 0);
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
	if (sl->sl.stamp.sl_dur_unavail_failed)
		sl->sl.stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_failed, 0);
	sl->sl.stamp.sl_dur_unavail = jiffies;
}
STATIC void
sl_rpo_stats(queue_t *q, struct sl *sl)
{
	if (sl->sl.stamp.sl_dur_in_service)
		sl->sl.stats.sl_dur_in_service +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_in_service, 0);
	sl->sl.stamp.sl_dur_unavail_rpo = jiffies;
}
STATIC void
sl_rpr_stats(queue_t *q, struct sl *sl)
{
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
}
STATIC int
sl_lsc_stop(queue_t *q, struct sl *sl)
{
	int err;
	if (sl_get_state(sl) != MS_OUT_OF_SERVICE) {
		if ((err = sl_send_status(q, sl, M2PA_STATUS_OUT_OF_SERVICE)) < 0)
			return (err);
		sl_timer_stop(sl, tall);
		sl_aerm_stop(q, sl);
		sl_suerm_stop(q, sl);
		sl->flags &= ~MF_LPO;
		if (sl->flags & MF_RPO) {
			if (sl->sl.notify.events & SL_EVT_RPO_END)
				if ((err = lmi_event_ind(q, sl, SL_EVT_RPO_END, 0, NULL, 0)) < 0)
					return (err);
			ptrace(("%s: %p: RPO Ends\n", MOD_NAME, sl));
			sl_rpr_stats(q, sl);
			sl->flags &= ~MF_RPO;
		}
		sl->flags &= ~MF_LOC_INS;
		sl->flags &= ~MF_REM_INS;
		sl->flags &= ~MF_LOC_BUSY;
		sl->flags &= ~MF_REM_BUSY;
		sl->flags &= ~MF_LOC_EMERG;
		sl->flags &= ~MF_REM_EMERG;
		sl_oos_stats(q, sl);
		sl_set_state(sl, MS_OUT_OF_SERVICE);
		return (0);
	}
	return (0);
}
STATIC int
sl_lsc_out_of_service(queue_t *q, struct sl *sl, ulong reason)
{
	int err;
	if (sl_get_state(sl) != MS_OUT_OF_SERVICE) {
		if ((err = sl_out_of_service_ind(q, sl, reason)) < 0)
			return (err);
		if ((err = sl_lsc_stop(q, sl)) < 0)
			return (err);
		sl->sl.stamp.sl_dur_unavail_failed = jiffies;
		return (0);
	}
	return (0);
}
STATIC INLINE int
sl_lsc_power_on(queue_t *q, struct sl *sl)
{
	int err;
	if (sl_get_state(sl) == MS_POWER_OFF) {
		sl->rmsu = 0;
		sl->rack = 0;
		sl->fsnr = 0xffffff;
		sl->bsnr = 0xffffff;
		sl->fsnx = 0;
		sl->tmsu = 0;
		sl->tack = 0;
		sl->fsnt = 0xffffff;
		sl->back = 0;
		sl->bbsn = 0;
		sl->bfsn = 0;
		sl->flags = 0;
		if ((err = sl_send_status(q, sl, M2PA_STATUS_OUT_OF_SERVICE)) < 0)
			return (err);
		sl_set_state(sl, MS_OUT_OF_SERVICE);
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_POWER_ON_REQ in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}
STATIC INLINE int
sl_lsc_start(queue_t *q, struct sl *sl)
{
	int err;
	if (sl_get_state(sl) == MS_OUT_OF_SERVICE) {
		sl->rmsu = 0;
		sl->rack = 0;
		sl->fsnr = 0xffffff;
		sl->bsnr = 0xffffff;
		sl->fsnx = 0;
		sl->tmsu = 0;
		sl->tack = 0;
		sl->fsnt = 0xffffff;
		sl->back = 0;
		sl->bbsn = 0;
		sl->bfsn = 0;
		// sl->flags = 0;
		if ((err = sl_send_status(q, sl, M2PA_STATUS_ALIGNMENT)) < 0)
			return (err);
		sl_timer_start(sl, t2);
		sl_set_state(sl, MS_NOT_ALIGNED);
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_START_REQ in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}
STATIC int
sl_t2_timeout(struct sl *sl)
{
	int err;
	if (sl_get_state(sl) == MS_NOT_ALIGNED) {
		if (sl->sl.notify.events & SL_EVT_FAIL_ALIGNMENT)
			if ((err = lmi_event_ind(NULL, sl, SL_EVT_FAIL_ALIGNMENT, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(NULL, sl, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)) < 0)
			return (err);
		printd(("%s: %p: Link failed: Alignment not possible\n", MOD_NAME, sl));
		sl->sl.stats.sl_fail_align_or_proving++;
		return (0);
	}
	ptrace(("%s: %p: Received timeout t2 in uexpected state %lu\n", MOD_NAME, sl,
		sl_get_state(sl)));
	return (-EPROTO);
}

STATIC int
sl_ready(queue_t *q, struct sl *sl)
{
	int err;
	sl->flags &= ~MF_LOC_EMERG;
	sl->flags &= ~MF_REM_EMERG;
	sl_suerm_start(NULL, sl);
	sl->flags |= MF_LOC_INS;
	if (sl->flags & MF_LPO) {
		if ((err = sl_send_status(NULL, sl, M2PA_STATUS_PROCESSOR_OUTAGE)) < 0)
			return (err);
		if (sl->flags & MF_REM_INS) {
			if ((err = sl_in_service_ind(NULL, sl)) < 0)
				return (err);
			sl->flags &= ~MF_SEND_MSU;
			sl->flags &= ~MF_RECV_MSU;
			sl_oos_stats(NULL, sl);
			sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		} else {
			sl_timer_start(sl, t1);
			sl_set_state(sl, MS_ALIGNED_NOT_READY);
		}
	} else {
		mblk_t *mp;
		if ((mp = sl->tb.q_head)) {
			if ((err = sl_send_data(NULL, sl, mp)) < 0)
				return (err);
			sl->tmsu++;
			sl->fsnt++;
			sl->fsnt &= 0xffffff;
			bufq_queue(&sl->rtb, bufq_dequeue(&sl->tb));
			sl->tmsu--;
			sl->tack++;
			sl->sl.stats.sl_tran_msus++;
			sl->sl.stats.sl_tran_sio_sif_octets += mp->b_wptr - mp->b_rptr - 1;
			if (sl->rtb.q_msgs >= sl->sl.config.N1
			    || sl->rtb.q_count >= sl->sl.config.N2
			    || sl->tmsu + sl->tack > 0x7ffffff)
				sl->flags |= MF_RTB_FULL;
		} else {
			if ((err = sl_send_status(NULL, sl, M2PA_STATUS_IN_SERVICE)) < 0)
				return (err);
		}
		if (sl->flags & MF_REM_INS) {
			if ((err = sl_in_service_ind(NULL, sl)) < 0)
				return (err);
			sl->flags |= MF_SEND_MSU;
			sl->flags |= MF_RECV_MSU;
			sl_is_stats(NULL, sl);
			sl_set_state(sl, MS_IN_SERVICE);
		} else if (sl->flags & MF_RPO) {
			if ((err = sl_remote_processor_outage_ind(q, sl)) < 0)
				return (err);
			sl->flags &= ~MF_SEND_MSU;
			sl->flags &= ~MF_RECV_MSU;
			sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		} else {
			sl_timer_start(sl, t1);
			sl_set_state(sl, MS_ALIGNED_READY);
		}
	}
	return (0);
}

STATIC INLINE int
sl_lsc_status_alignment(queue_t *q, struct sl *sl)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);
	switch (sl_get_state(sl)) {
	case MS_OUT_OF_SERVICE:
		return (QR_DONE);
	case MS_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (proving) {
			if (sl->flags & MF_LOC_EMERG)
				err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_EMERGENCY);
			else
				err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_NORMAL);
			if (err)
				return (err);
			sl_timer_start(sl, t3);
			sl_set_state(sl, MS_ALIGNED);
			return (QR_DONE);
		}
	case MS_ALIGNED:
		if (proving)
			return (QR_DONE);
		return sl_ready(q, sl);
	case MS_PROVING:
		sl_aerm_stop(q, sl);
		sl_timer_stop(sl, t4);
		sl_timer_start(sl, t3);
		sl_set_state(sl, MS_ALIGNED);
		return (QR_DONE);
	default:
		if (sl->sl.notify.events & SL_EVT_FAIL_RECEIVED_SIO)
			if ((err = lmi_event_ind(q, sl, SL_EVT_FAIL_RECEIVED_SIO, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(q, sl, SL_FAIL_RECEIVED_SIO)) < 0)
			return (err);
		printd(("%s: %p: Link failed: Received SIO\n", MOD_NAME, sl));
		return (QR_DONE);
	}
}

STATIC INLINE int
sl_lsc_emergency(queue_t *q, struct sl *sl)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);
	sl->flags |= MF_LOC_EMERG;
	if (proving) {
		switch (sl_get_state(sl)) {
		case MS_PROVING:
			/* 
			   For a normal Q.703 link, when we received an emergency signal in the
			   proving state, we would stop sending normal proving messages and being
			   sending emergency proving messages.  The problem is if we have already
			   primed the SCTP transmit queue with normal messages the change would be
			   difficult.  MTP2 normally paces output of SIN so that the switch to SIE
			   can be made at any time.  The only way that we can change mid-burst is
			   to send proving messages in smaller chunks or on some sort of timed
			   basis. */
			if ((err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_EMERGENCY)) < 0)
				return (err);
			sl_timer_stop(sl, t4);
			sl_aerm_stop(q, sl);
			sl_timer_start(sl, t4);
			sl_aerm_start(q, sl);
			return (0);
		case MS_ALIGNED:
			if ((err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_EMERGENCY)) < 0)
				return (err);
			return (0);
		}
	}
	ptrace(("%s: %p: Received primitive SL_EMERGENCY_REQ in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_emergency_ceases(queue_t *q, struct sl *sl)
{
	sl->flags &= ~MF_LOC_EMERG;
	return (0);
}

STATIC INLINE int
sl_lsc_status_proving_normal(queue_t *q, struct sl *sl)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);
	switch (sl_get_state(sl)) {
	case MS_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (proving) {
			if (sl->flags & MF_LOC_EMERG)
				err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_EMERGENCY);
			else
				err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_NORMAL);
			if (err)
				return (err);
			sl_timer_start(sl, t3);
			sl_set_state(sl, MS_ALIGNED);
		}
		/* 
		   fall thru */
	case MS_ALIGNED:
		sl->flags &= ~MF_REM_EMERG;
		if (proving) {
			sl_timer_stop(sl, t3);
			sl_aerm_start(q, sl);
			sl_timer_start(sl, t4);
			sl->sl.statem.Cp = 0;
			sl_set_state(sl, MS_PROVING);
			return (QR_DONE);
		}
		return sl_ready(q, sl);
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		if (sl->sl.notify.events & SL_EVT_FAIL_RECEIVED_SIN)
			if ((err = lmi_event_ind(q, sl, SL_EVT_FAIL_RECEIVED_SIN, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(q, sl, SL_FAIL_RECEIVED_SIN)) < 0)
			return (err);
		printd(("%s: %p: Link failed: Received SIN\n", MOD_NAME, sl));
		return (QR_DONE);
	}
	ptrace(("%s: %p: Received status PROVING_NORMAL in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_status_proving_emergency(queue_t *q, struct sl *sl)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);
	switch (sl_get_state(sl)) {
	case MS_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (proving) {
			if (sl->flags & MF_LOC_EMERG)
				err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_EMERGENCY);
			else
				err = sl_send_proving(q, sl, M2PA_STATUS_PROVING_NORMAL);
			if (err)
				return (err);
			sl_timer_start(sl, t3);
			sl_set_state(sl, MS_ALIGNED);
		}
		/* 
		   fall thru */
	case MS_ALIGNED:
		if (proving) {
			sl->flags |= MF_REM_EMERG;
			sl_timer_stop(sl, t3);
			sl_aerm_start(q, sl);
			sl_timer_start(sl, t4);
			sl->sl.statem.Cp = 0;
			sl_set_state(sl, MS_PROVING);
			return (QR_DONE);
		}
		return sl_ready(q, sl);
	case MS_PROVING:
		if (sl->flags & MF_REM_EMERG)
			return (QR_DONE);
		if (sl->flags & MF_LOC_EMERG) {
			sl->flags |= MF_REM_EMERG;
			return (QR_DONE);
		}
		sl->flags |= MF_REM_EMERG;
		sl_timer_stop(sl, t4);
		sl_aerm_stop(q, sl);
		sl_aerm_start(q, sl);
		sl_timer_start(sl, t4);
		sl->sl.statem.further_proving = 0;
		return (QR_DONE);
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		if (sl->sl.notify.events & SL_EVT_FAIL_RECEIVED_SIE)
			if ((err = lmi_event_ind(q, sl, SL_EVT_FAIL_RECEIVED_SIE, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(q, sl, SL_FAIL_RECEIVED_SIE)) < 0)
			return (err);
		printd(("%s: %p: Link failed: Received SIE\n", MOD_NAME, sl));
		return (QR_DONE);
	}
	ptrace(("%s: %p: Received status PROVING_EMERGENCY in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC int
sl_t1_timeout(struct sl *sl)
{
	int err;
	if (sl->sl.notify.events & SL_EVT_FAIL_T1_TIMEOUT)
		if ((err = lmi_event_ind(NULL, sl, SL_EVT_FAIL_T1_TIMEOUT, 0, NULL, 0)) < 0)
			return (err);
	if ((err = sl_lsc_out_of_service(NULL, sl, SL_FAIL_T1_TIMEOUT)) < 0)
		return (err);
	printd(("%s: %p: Link failed: T1 Timeout\n", MOD_NAME, sl));
	sl->sl.stats.sl_fail_align_or_proving++;
	return (0);
}

STATIC int
sl_t3_timeout(struct sl *sl)
{
	int err;
	if (sl_get_state(sl) == MS_ALIGNED) {
		if (sl->sl.notify.events & SL_EVT_FAIL_ALIGNMENT)
			if ((err = lmi_event_ind(NULL, sl, SL_EVT_FAIL_ALIGNMENT, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(NULL, sl, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)) < 0)
			return (err);
		printd(("%s: %p: Link failed: T3 Timeout\n", MOD_NAME, sl));
		sl->sl.stats.sl_fail_align_or_proving++;
		return (0);
	}
	ptrace(("%s: %p: Received timeout t3 in uexpected state %lu\n", MOD_NAME, sl,
		sl_get_state(sl)));
	return (-EPROTO);
}

STATIC int
sl_t4_timeout(struct sl *sl)
{
	int err;
	if (sl_get_state(sl) == MS_PROVING) {
		sl_aerm_stop(NULL, sl);
		if ((err = sl_ready(NULL, sl)))
			return (err);
		return (0);
	}
	ptrace(("%s: %p: Received timeout t4 in unexpected state %lu\n", MOD_NAME, sl,
		sl_get_state(sl)));
	return (-EPROTO);
}

STATIC void sl_tx_wakeup(queue_t *q);
STATIC INLINE int
sl_lsc_status_in_service(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_ALIGNED:
	case MS_PROVING:
		sl->flags |= MF_REM_INS;
		return (QR_DONE);
	case MS_ALIGNED_NOT_READY:
		if ((err = sl_in_service_ind(q, sl)) < 0)
			return (err);
		sl_timer_stop(sl, t1);
		sl->flags &= ~MF_SEND_MSU;
		sl->flags &= ~MF_RECV_MSU;
		sl_oos_stats(q, sl);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		return (QR_DONE);
	case MS_ALIGNED_READY:
		if ((err = sl_in_service_ind(q, sl)) < 0)
			return (err);
		sl_timer_stop(sl, t1);
		sl->flags |= MF_SEND_MSU;
		sl->flags |= MF_RECV_MSU;
		sl_is_stats(q, sl);
		sl_set_state(sl, MS_IN_SERVICE);
		return (QR_DONE);
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		if (sl->i_version >= M2PA_VERSION_DRAFT10) {
			/* 
			   In the final, when we receive a READY in the in service or processor
			   outage states, we use the BSN in the message to synchronize when
			   synchronization was required. */
			if (sl->flags & MF_WAIT_SYNC) {
				sl->fsnt = ntohl(*((uint32_t *) mp->b_rptr)) & 0xffffff;
				sl->flags &= ~MF_WAIT_SYNC;
				sl->flags |= MF_SEND_MSU;
				sl_tx_wakeup(q);
			}
		}
	case MS_OUT_OF_SERVICE:
		ptrace(("%s: %p: Received status IN_SERVICE in unusual state %lu\n",
			MOD_NAME, sl, sl_get_state(sl)));
		return (QR_DONE);
	}
	ptrace(("%s: %p: Received status IN_SERVICE in unexpected state %lu\n", MOD_NAME,
		sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC INLINE int sl_rb_congestion_function(queue_t *q, struct sl *sl);

STATIC void
sl_rx_wakeup(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	mblk_t *mp;
	ensure(q, return);
	switch (sl_get_state(sl)) {
	default:
		if (sl->flags & MF_RECV_MSU && sl->rb.q_msgs) {
			while ((mp = bufq_dequeue(&sl->rb))) {
				if ((err = sl_pdu_ind(q, sl, mp)) < 0) {
					bufq_queue_head(&sl->rb, mp);
					rare();
					return;
				}
				sl->sl.stats.sl_recv_msus++;
				sl->sl.stats.sl_recv_sio_sif_octets += mp->b_wptr - mp->b_rptr - 1;
				sl->rmsu++;
				sl->fsnr++;
				sl->fsnr &= 0xffffff;
				sl_rb_congestion_function(q, sl);
				if (!(sl->flags & MF_CONG_ACCEPT))
					sl->rack += xchg(&sl->rmsu, 0);
			}
			if (sl->rack)
				sl_send_ack(q, sl);
		}
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return;
	}
}

/*
 *  Sequence number comparisons:
 */
/*
   is s2<=s1<=s3 
 */
#define between32(__s1,__s2,__s3) ((uint32_t)(__s3)-(uint32_t)(__s2)>=(uint32_t)(__s1)-(uint32_t)(__s2))
#define outside32(__s1,__s2,__s3) between32(__s1,__s3,__s2)
#define before32(__s1,__s2) (((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))<0))
#define after32(__s1,__s2) (((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))<0))
#define diff32(__s1,__s2) (before32(__s1,__s2) ? \
		((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))) : \
		((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))))
/*
   is s2<=s1<=s3 
 */
/* XXX: think these through again */
#define between24(__s1,__s2,__s3) ((((uint32_t)(__s3)-(uint32_t)(__s2))&0xffffff)>=(((uint32_t)(__s1)-(uint32_t)(__s2))&0xffffff))
#define outside24(__s1,__s2,__s3) between24(__s1,__s3,__s2)
#define before24(__s1,__s2) (((uint32_t)(__s1)-(uint32_t)(__s2))&0x800000)
#define after24(__s1,__s2) ((__s1)!=(__s2)&&!before24(__s1,__s2))
#define diff24(__s1,__s2) ((before24(__s1,__s2) ? \
		((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))) : \
		((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))))&0xffffff)

/*
   is s2<=s1<=s3 
 */
#define between16(__s1,__s2,__s3) ((uint16_t)(__s3)-(uint16_t)(__s2)>=(uint16_t)(__s1)-(uint16_t)(__s2))
#define outside16(__s1,__s2,__s3) between16(__s1,__s3,__s2)
#define before16(__s1,__s2) (((int16_t)((uint16_t)(__s1)-(uint16_t)(__s2))<0))
#define after16(__s1,__s2) (((int16_t)((uint16_t)(__s2)-(uint16_t)(__s1))<0))
#define diff16(__s1,__s2) (before16(__s1,__s2) ? \
		((int16_t)((uint16_t)(__s2)-(uint16_t)(__s1))) : \
		((int16_t)((uint16_t)(__s1)-(uint16_t)(__s2))))

STATIC INLINE int sl_txc_datack(queue_t *q, struct sl *sl, ulong count);

STATIC INLINE int
sl_rc_sn_check(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
	{
		size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (mlen >= 2 * sizeof(uint16_t)) {
			int msu;
			uint bsnr = ntohs(*(uint16_t *) mp->b_rptr++);
			uint fsnr = ntohs(*(uint16_t *) mp->b_rptr++);
			mlen -= 2 * sizeof(uint16_t);
			msu = (mlen > sizeof(uint32_t)) ? 0 : 1;
			if ((uint16_t) fsnr != (uint16_t) (sl->fsnx - msu)) {
				if (sl->bfsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_FIBR)
						if ((err =
						     lmi_event_ind(q, sl, SL_EVT_FAIL_ABNORMAL_FIBR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(q, sl,
								   SL_FAIL_ABNORMAL_FIBR)) < 0)
						return (err);
					printd(("%s: %p: Link failed: Abnormal FIBR\n",
						MOD_NAME, sl));
					return (-EPROTO);
				}
				printd(("%s: %p: Received bad fsn = %u, expecting %u\n",
					MOD_NAME, sl, fsnr, (sl->fsnx - msu)));
				sl->bfsn++;
				return (-EPROTO);
			} else
				sl->bfsn = 0;
			if (!between16(bsnr, sl->bsnr, sl->fsnt)) {
				if (sl->bbsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(q, sl, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(q, sl,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					printd(("%s: %p: Link failed: Abnormal BSNR\n",
						MOD_NAME, sl));
					return (-EPROTO);
				}
				printd(("%s: %p: Received bad bsn = %u, expecting %u\n",
					MOD_NAME, sl, bsnr, sl->fsnt));
				sl->bbsn++;
			} else {
				if ((err = sl_txc_datack(q, sl, diff16(sl->bsnr, bsnr))) < 0)
					return (err);
				sl->bsnr = bsnr;
				sl->bbsn = 0;
			}
			break;
		}
		rare();
		return (-EMSGSIZE);
	}
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	{
		size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (mlen >= 2 * sizeof(uint32_t)) {
			int msu;
			uint bsnr = ntohl(*(uint32_t *) mp->b_rptr++) & 0xffffff;
			uint fsnr = ntohl(*(uint32_t *) mp->b_rptr++) & 0xffffff;
			mlen -= 2 * sizeof(uint32_t);
			msu = (mlen > sizeof(uint32_t)) ? 0 : 1;
			if ((uint32_t) fsnr != (uint32_t) ((sl->fsnx - msu) & 0xffffff)) {
				if (sl->bfsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_FIBR)
						if ((err =
						     lmi_event_ind(q, sl, SL_EVT_FAIL_ABNORMAL_FIBR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(q, sl,
								   SL_FAIL_ABNORMAL_FIBR)) < 0)
						return (err);
					printd(("%s: %p: Link failed: Abnormal FIBR\n",
						MOD_NAME, sl));
					return (-EPROTO);
				}
				printd(("%s: %p: Received bad fsn = %u, expecting %u\n",
					MOD_NAME, sl, fsnr, (sl->fsnx - msu) & 0xffffff));
				sl->bfsn++;
				sl->bfsn &= 0xffffff;
				return (-EPROTO);
			} else
				sl->bfsn = 0;
			if (!between24(bsnr, sl->bsnr, sl->fsnt)) {
				if (sl->bbsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(q, sl, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(q, sl,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					printd(("%s: %p: Link failed: Abnormal BSNR\n",
						MOD_NAME, sl));
					return (-EPROTO);
				}
				printd(("%s: %p: Received bad bsn = %u, expecting %u\n",
					MOD_NAME, sl, bsnr, sl->fsnt));
				sl->bbsn++;
				sl->bbsn &= 0xffffff;
			} else {
				if ((err = sl_txc_datack(q, sl, diff24(sl->bsnr, bsnr))) < 0)
					return (err);
				sl->bsnr = bsnr;
				sl->bbsn = 0;
			}
			break;
		}
		rare();
		return (-EMSGSIZE);
	}
	default:
	case M2PA_VERSION_DRAFT10:
	{
		size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (mlen >= 2 * sizeof(uint32_t)) {
			int msu;
			uint bsnr = ntohl(*(uint32_t *) mp->b_rptr++) & 0xffffff;
			uint fsnr = ntohl(*(uint32_t *) mp->b_rptr++) & 0xffffff;
			mlen -= 2 * sizeof(uint32_t);
			msu = (mlen > sizeof(uint32_t)) ? 0 : 1;
			if ((uint32_t) fsnr != (uint32_t) ((sl->fsnx - msu) & 0xffffff)) {
				if (sl->bfsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_FIBR)
						if ((err =
						     lmi_event_ind(q, sl, SL_EVT_FAIL_ABNORMAL_FIBR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(q, sl,
								   SL_FAIL_ABNORMAL_FIBR)) < 0)
						return (err);
					printd(("%s: %p: Link failed: Abnormal FIBR\n",
						MOD_NAME, sl));
					return (-EPROTO);
				}
				printd(("%s: %p: Received bad fsn = %u, expecting %u\n",
					MOD_NAME, sl, fsnr, (sl->fsnx - msu) & 0xffffff));
				sl->bfsn++;
				sl->bfsn &= 0xffffff;
				return (-EPROTO);
			} else
				sl->bfsn = 0;
			if (!between24(bsnr, sl->bsnr, sl->fsnt)) {
				if (sl->bbsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(q, sl, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(q, sl,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					printd(("%s: %p: Link failed: Abnormal BSNR\n",
						MOD_NAME, sl));
					return (-EPROTO);
				}
				printd(("%s: %p: Received bad bsn = %u, expecting %u\n",
					MOD_NAME, sl, bsnr, sl->fsnt));
				sl->bbsn++;
				sl->bbsn &= 0xffffff;
			} else {
				if ((err = sl_txc_datack(q, sl, diff24(sl->bsnr, bsnr))) < 0)
					return (err);
				sl->bsnr = bsnr;
				sl->bbsn = 0;
			}
			break;
		}
		rare();
		return (-EMSGSIZE);
	}
	case M2PA_VERSION_DRAFT11:
	{
		/* In the final, when we are waiting for synchronization (we have sent
		   PROCESSOR_RECOVERED but have not yet received IN_SERVICE, DATA or DATA-ACK) we
		   do not check the sequence number, but resynchornize to the provided number. */
		size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (mlen >= 2 * sizeof(uint32_t)) {
			int msu;
			uint bsnr = ntohl(*(uint32_t *) mp->b_rptr++) & 0xffffff;
			uint fsnr = ntohl(*(uint32_t *) mp->b_rptr++) & 0xffffff;
			mlen -= 2 * sizeof(uint32_t);
			msu = (mlen > sizeof(uint32_t)) ? 0 : 1;
			/* In the final we discard out of sequence FSNs but do two out of three on
			   BSNs */
			if ((uint32_t) fsnr != (uint32_t) ((sl->fsnx - msu) & 0xffffff))
				return (-EPROTO);
			if (!between24(bsnr, sl->bsnr, sl->fsnt)) {
				if (sl->bbsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(q, sl, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(q, sl,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					printd(("%s: %p: Link failed: Abnormal BSNR\n",
						MOD_NAME, sl));
					return (-EPROTO);
				}
				printd(("%s: %p: Received bad bsn = %u, expecting %u\n",
					MOD_NAME, sl, bsnr, sl->fsnt));
				sl->bbsn++;
				sl->bbsn &= 0xffffff;
			} else {
				if ((err = sl_txc_datack(q, sl, diff24(sl->bsnr, bsnr))) < 0)
					return (err);
				sl->bsnr = bsnr;
				sl->bbsn = 0;
			}
			break;
		}
		rare();
		return (-EMSGSIZE);
	}
	}
	return (0);
}

STATIC INLINE int
sl_rc_signal_unit(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_ALIGNED_READY:
		if ((err = sl_in_service_ind(q, sl)) < 0)
			return (err);
		sl_timer_stop(sl, t1);
		sl->flags |= MF_SEND_MSU;
		sl->flags |= MF_RECV_MSU;
		sl_is_stats(q, sl);
		sl_set_state(sl, MS_IN_SERVICE);
	default:
		if ((err = sl_rc_sn_check(q, sl, mp)) < 0)
			return (err);
		if (sl->flags & MF_RECV_MSU) {
			if (!(sl->flags & MF_CONG_DISCARD)) {
				if (mp->b_wptr - mp->b_rptr > sizeof(uint32_t)) {
					bufq_queue(&sl->rb, mp);
					sl_rb_congestion_function(q, sl);
					sl->fsnx++;
					sl->fsnx &= 0xffffff;
					return (QR_ABSORBED);
				}
				return (QR_DONE);
			}
			return (-EBUSY);
		}
		rare();
		return (-EPROTO);
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		ptrace(("%s: %p: Received SIGNAL UNIT in unexpected state %lu\n", MOD_NAME,
			sl, sl_get_state(sl)));
		return (QR_DONE);
	}
}

STATIC void
sl_tx_wakeup(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	mblk_t *mp;
	ensure(q, return);
	switch (sl_get_state(sl)) {
	default:
		if (sl->rack)
			if ((err = sl_send_ack(q, sl)) < 0)
				return;
		if (sl->flags & MF_SEND_MSU && sl->tb.q_msgs && !(sl->flags & MF_RTB_FULL)) {
			if (!sl->rtb.q_count) {
				if (sl->flags & MF_REM_BUSY)
					sl_timer_start(sl, t6);
				else
					sl_timer_start(sl, t7);
			}
			while ((mp = sl->tb.q_head)) {
				if ((err = sl_send_data(q, sl, mp)) < 0)
					return;
				sl->tmsu++;
				sl->fsnt++;
				sl->fsnt &= 0xffffff;
				bufq_queue(&sl->rtb, bufq_dequeue(&sl->tb));
				sl->tmsu--;
				sl->tack++;
				sl->sl.stats.sl_tran_msus++;
				sl->sl.stats.sl_tran_sio_sif_octets += mp->b_wptr - mp->b_rptr - 1;
				if (sl->rtb.q_msgs >= sl->sl.config.N1
				    || sl->rtb.q_count >= sl->sl.config.N2
				    || sl->tmsu + sl->tack > 0x7ffffff) {
					sl->flags |= MF_RTB_FULL;
					break;
				}
			}
		}
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return;
	}
}
STATIC int sl_check_congestion(queue_t *q, struct sl *sl);
STATIC INLINE int
sl_txc_datack(queue_t *q, struct sl *sl, ulong count)
{
	int err;
	if (!count)
		printd(("%s: %p: WARNING: ack called with zero count\n", MOD_NAME, sl));
	switch (sl_get_state(sl)) {
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
	case MS_ALIGNED_READY:
	case MS_ALIGNED_NOT_READY:
		while (count--) {
			if (sl->rtb.q_msgs) {
				freemsg(bufq_dequeue(&sl->rtb));
				sl->tack--;
				qenable(sl->iq);
			} else {
				if (sl->back) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(q, sl, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(q, sl,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					printd(("%s: %p: Link failed: Abnormal BSNR\n",
						MOD_NAME, sl));
					return (0);
				}
				printd(("%s: %p: Received bad acknowledgement acks = %lu\n",
					MOD_NAME, sl, count + 1));
				sl->back++;
				return (0);
			}
		}
		sl->back = 0;
		if (sl->rtb.q_count == 0) {
			sl_timer_stop(sl, t7);
			sl_timer_stop(sl, t6);
		} else if (!(sl->flags & (MF_REM_BUSY | MF_RPO)))
			sl_timer_start(sl, t7);
		sl->flags &= ~MF_RTB_FULL;
		sl_check_congestion(q, sl);
		sl_tx_wakeup(q);
		return (0);
	default:
		ptrace(("%s: %p: Received ACK in unexpected state %lu\n", MOD_NAME, sl,
			sl_get_state(sl)));
		return (0);
	}
}
STATIC INLINE int
sl_lsc_pdu(queue_t *q, struct sl *sl, mblk_t *mp)
{
	bufq_queue(&sl->tb, mp);
	sl_check_congestion(q, sl);
	return (0);
}
STATIC int
sl_t7_timeout(struct sl *sl)
{
	int err;
	if (sl->sl.notify.events & SL_EVT_FAIL_ACK_TIMEOUT)
		if ((err = lmi_event_ind(NULL, sl, SL_EVT_FAIL_ACK_TIMEOUT, 0, NULL, 0)) < 0)
			return (err);
	if ((err = sl_lsc_out_of_service(NULL, sl, SL_FAIL_ACK_TIMEOUT)) < 0)
		return (err);
	printd(("%s: %p: Link failed: T7 Timeout\n", MOD_NAME, sl));
	return (0);
}
STATIC INLINE int
sl_lsc_status_busy(queue_t *q, struct sl *sl)
{
	switch (sl_get_state(sl)) {
	case MS_IN_SERVICE:
		if (!(sl->flags & MF_REM_BUSY)) {
			sl->flags |= MF_REM_BUSY;
			if (sl->rtb.q_count)
				sl_timer_start(sl, t6);
			sl_timer_stop(sl, t7);
		}
		return (QR_DONE);
	case MS_PROCESSOR_OUTAGE:
		sl->flags |= MF_REM_BUSY;
		return (QR_DONE);
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return (QR_DONE);
	}
	ptrace(("%s: %p: Received status BUSY in unexpected state %lu\n", MOD_NAME, sl,
		sl_get_state(sl)));
	return (-EPROTO);
}
STATIC INLINE int
sl_lsc_congestion_discard(queue_t *q, struct sl *sl)
{
	int err;
	if (!(sl->flags & MF_LOC_BUSY)) {
		if ((err = sl_send_status(q, sl, M2PA_STATUS_BUSY)) < 0)
			return (err);
		sl->sl.stats.sl_sibs_sent++;
		sl->flags |= MF_LOC_BUSY;
	}
	sl->flags |= MF_CONG_DISCARD;
	sl->flags |= MF_L3_CONG_DETECT;
	return (0);
}
STATIC INLINE int
sl_lsc_congestion_accept(queue_t *q, struct sl *sl)
{
	int err;
	if (!(sl->flags & MF_LOC_BUSY)) {
		if ((err = sl_send_status(q, sl, M2PA_STATUS_BUSY)) < 0)
			return (err);
		sl->sl.stats.sl_sibs_sent++;
		sl->flags |= MF_LOC_BUSY;
	}
	sl->flags |= MF_CONG_ACCEPT;
	sl->flags |= MF_L3_CONG_DETECT;
	return (0);
}
STATIC INLINE int
sl_lsc_status_busy_ended(queue_t *q, struct sl *sl)
{
	switch (sl_get_state(sl)) {
	default:
		ptrace(("%s: %p: Received status BUSY_ENDED in unexpected state %lu\n",
			MOD_NAME, sl, sl_get_state(sl)));
	case MS_IN_SERVICE:
		sl->flags &= ~MF_REM_BUSY;
		sl_timer_stop(sl, t6);
		if (sl->rtb.q_count)
			sl_timer_start(sl, t7);
		return (QR_DONE);
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return (QR_DONE);
	}
}
STATIC int
sl_t6_timeout(struct sl *sl)
{
	int err;
	if (sl->sl.notify.events & SL_EVT_FAIL_CONG_TIMEOUT)
		if ((err = lmi_event_ind(NULL, sl, SL_EVT_FAIL_CONG_TIMEOUT, 0, NULL, 0)) < 0)
			return (err);
	if ((err = sl_lsc_out_of_service(NULL, sl, SL_FAIL_CONG_TIMEOUT)) < 0)
		return (err);
	printd(("%s: %p: Link failed: T6 Timeout\n", MOD_NAME, sl));
	return (0);
}
STATIC INLINE int
sl_lsc_no_congestion(queue_t *q, struct sl *sl)
{
	int err;
	if (sl->flags & MF_LOC_BUSY)
		if ((err = sl_send_status(q, sl, M2PA_STATUS_BUSY_ENDED)) < 0)
			return (err);
	sl->flags &= ~MF_LOC_BUSY;
	sl->flags &= ~MF_CONG_DISCARD;
	sl->flags &= ~MF_CONG_ACCEPT;
	sl->flags &= ~MF_L3_CONG_DETECT;
	sl->rack += xchg(&sl->rmsu, 0);
	return (0);
}

STATIC INLINE int
sl_lsc_status_processor_outage(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
		/* ignored in these states */
		return (QR_DONE);
	case MS_ALIGNED:
	case MS_PROVING:
		if (!(sl->flags & MF_RPO)) {
			if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
				if ((err = lmi_event_ind(q, sl, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
					return (err);
			/* just remember for later */
			sl->flags |= MF_RPO;
			ptrace(("%s: %p: RPO Begins\n", MOD_NAME, sl));
		}
		return (QR_DONE);
	case MS_ALIGNED_READY:
	case MS_ALIGNED_NOT_READY:
		if (!(sl->flags & MF_RPO)) {
			if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
				if ((err = lmi_event_ind(q, sl, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
					return (err);
			if ((err = sl_remote_processor_outage_ind(q, sl)) < 0)
				return (err);
			sl->flags |= MF_RPO;
			sl_rpo_stats(q, sl);
			ptrace(("%s: %p: RPO Begins\n", MOD_NAME, sl));
		}
		sl_timer_stop(sl, t1);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		return (QR_DONE);
	case MS_IN_SERVICE:
		if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
			if ((err = lmi_event_ind(q, sl, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
				return (err);
		if (!(sl->flags & MF_RPO)) {
			if ((err = sl_remote_processor_outage_ind(q, sl)) < 0)
				return (err);
			sl->flags |= MF_RPO;
		}
		ptrace(("%s: %p: RPO Begins\n", MOD_NAME, sl));
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_SEND_MSU;
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl->flags &= ~MF_CONTINUE;
			break;
		case SS7_PVAR_ANSI_92:
			sl->rack += xchg(&sl->rmsu, 0);
			fixme(("%s: %p: FIXME: Bad idea\n", MOD_NAME, sl));
			if (sl->flags & MF_LOC_BUSY)
				if ((err = sl_send_status(q, sl, M2PA_STATUS_BUSY_ENDED)) < 0)
					return (err);
			sl->flags &= ~MF_LOC_BUSY;
			break;
		}
		sl_rpo_stats(q, sl);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		return (QR_DONE);
	case MS_PROCESSOR_OUTAGE:
		if (!(sl->flags & MF_RPO)) {
			if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
				if ((err = lmi_event_ind(q, sl, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
					return (err);
			if (!(sl->flags & MF_RPO)) {
				if ((err = sl_remote_processor_outage_ind(q, sl)) < 0)
					return (err);
				sl->flags |= MF_RPO;
			}
			ptrace(("%s: %p: RPO Begins\n", MOD_NAME, sl));
		}
		return (QR_DONE);
	}
	ptrace(("%s: %p: Received status PROCESSOR_OUTAGE in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_local_processor_outage(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
	case MS_ALIGNED:
	case MS_PROVING:
		/* 
		   just remember for later */
		sl->flags &= ~MF_WAIT_SYNC;
		sl->flags |= MF_LPO;
		return (0);
	case MS_ALIGNED_READY:
		if ((err = sl_send_status(q, sl, M2PA_STATUS_PROCESSOR_OUTAGE)) < 0)
			return (err);
		sl->flags &= ~MF_WAIT_SYNC;
		sl->flags |= MF_LPO;
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_RECV_MSU;
		sl_set_state(sl, MS_ALIGNED_NOT_READY);
		return (0);
	case MS_IN_SERVICE:
		if ((err = sl_send_status(q, sl, M2PA_STATUS_PROCESSOR_OUTAGE)) < 0)
			return (err);
		sl->flags &= ~MF_WAIT_SYNC;
		sl->flags |= MF_LPO;
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_RECV_MSU;
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl->flags &= ~MF_CONTINUE;
			break;
		case SS7_PVAR_ANSI_92:
			sl->rack += xchg(&sl->rmsu, 0);
			fixme(("%s: %p: FIXME: Bad idea\n", MOD_NAME, sl));
			if (sl->flags & MF_LOC_BUSY)
				if ((err = sl_send_status(q, sl, M2PA_STATUS_BUSY_ENDED)) < 0)
					return (err);
			sl->flags &= ~MF_LOC_BUSY;
			break;
		}
		sl_rpo_stats(q, sl);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		return (0);
	case MS_PROCESSOR_OUTAGE:
		if (!(sl->flags & MF_LPO))
			if ((err = sl_send_status(q, sl, M2PA_STATUS_PROCESSOR_OUTAGE)) < 0)
				return (err);
		sl->flags &= ~MF_WAIT_SYNC;
		sl->flags |= MF_LPO;
		sl_timer_stop(sl, t7);	/* ??? */
		sl_timer_stop(sl, t6);	/* ??? */
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_LOCAL_PROCESSOR_OUTAGE_REQ in unexpected state %lu\n", MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}
STATIC INLINE int
sl_lsc_clear_rtb(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_PROCESSOR_OUTAGE:
		bufq_purge(&sl->rtb);
		sl->tack = 0;
		sl->flags &= ~MF_RTB_FULL;
		sl->flags &= ~MF_CLEAR_RTB;
		if ((err = sl_rtb_cleared_ind(q, sl)) < 0)
			return (err);
		if ((err = sl_check_congestion(q, sl)) < 0)
			return (err);
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_LPO;
		if (sl->flags & MF_RPO) {
			if (sl->sl.notify.events & SL_EVT_RPO_END)
				if ((err = lmi_event_ind(q, sl, SL_EVT_RPO_END, 0, NULL, 0)) < 0)
					return (err);
			ptrace(("%s: %p: RPO Ends\n", MOD_NAME, sl));
			sl_rpr_stats(q, sl);
			sl->flags &= ~MF_RPO;
		}
		sl->flags &= ~MF_SEND_MSU;
		sl->flags |= MF_SEND_MSU;
		if ((err = sl_remote_processor_recovered_ind(q, sl)) < 0)
			return (err);
		sl_is_stats(q, sl);
		sl_set_state(sl, MS_IN_SERVICE);
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_CLEAR_RTB_REQ in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}
STATIC INLINE int
sl_lsc_clear_buffers(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
	case MS_ALIGNED:
	case MS_PROVING:
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			return (0);
		case SS7_PVAR_ANSI_92:
			if ((err = sl_rtb_cleared_ind(q, sl)) < 0)
				return (err);
			sl->flags &= ~MF_LPO;	/* ??? */
			return (0);
		}
		return (0);
	case MS_ALIGNED_NOT_READY:
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			return (0);
		case SS7_PVAR_ANSI_92:
			if ((err = sl_rtb_cleared_ind(q, sl)) < 0)
				return (err);
			sl->flags &= ~MF_LPO;
			sl_timer_stop(sl, t7);	/* ??? */
			sl_timer_stop(sl, t6);	/* ??? */
			sl->flags &= ~MF_SEND_MSU;
			sl_set_state(sl, MS_ALIGNED_READY);
			return (0);
		}
		return (0);
	case MS_PROCESSOR_OUTAGE:
		bufq_purge(&sl->rtb);
		sl->tack = 0;
		bufq_purge(&sl->tb);
		flushq(sl->iq, FLUSHDATA);
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_RTB_FULL;
		sl->flags &= ~MF_CLEAR_RTB;
		if ((err = sl_check_congestion(q, sl)) < 0)
			return (err);
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl->flags &= ~MF_NEED_FLUSH;
			if (sl->flags & MF_CONTINUE) {
				sl->flags |= MF_NEED_FLUSH;
				break;
			}
			return (0);
		case SS7_PVAR_ANSI_92:
			if (sl->sl.notify.events & SL_EVT_RPO_END)
				if ((err = lmi_event_ind(q, sl, SL_EVT_RPO_END, 0, NULL, 0)) < 0)
					return (err);
			if ((err = sl_rtb_cleared_ind(q, sl)) < 0)
				return (err);
			bufq_purge(&sl->rb);
			flushq(sl->oq, FLUSHDATA);
			if ((err = sl_rb_cleared_ind(q, sl)) < 0)
				return (err);
			if (sl->flags & MF_RPO) {
				if ((err = sl_remote_processor_recovered_ind(q, sl)) < 0)
					return (err);
				sl->flags &= ~MF_RPO;
			}
			ptrace(("%s: %p: RPO Ends\n", MOD_NAME, sl));
			sl_rpr_stats(q, sl);
			break;
		}
		sl->flags |= MF_RECV_MSU;
		sl->flags |= MF_SEND_MSU;
		sl->flags &= ~MF_LPO;
		sl_is_stats(q, sl);
		sl_set_state(sl, MS_IN_SERVICE);
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_CLEAR_BUFFERS_REQ in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}
STATIC void
sl_lsc_no_processor_outage(queue_t *q, struct sl *sl)
{
	switch (sl_get_state(sl)) {
	case MS_PROCESSOR_OUTAGE:
		sl->flags |= MF_CONTINUE;
		if (!(sl->flags & MF_NEED_FLUSH)) {
			sl->flags &= ~MF_LPO;
			sl->flags |= MF_NEED_FLUSH;
			sl->flags |= MF_RECV_MSU;
			if (!(sl->flags & MF_WAIT_SYNC))
				sl->flags |= MF_SEND_MSU;
			if (sl->rtb.q_count) {
				if (sl->flags & MF_REM_BUSY)
					sl_timer_start(sl, t6);
				else
					sl_timer_start(sl, t7);
			}
			sl_is_stats(q, sl);
			sl_set_state(sl, MS_IN_SERVICE);
		}
	}
}

#if 0
/*
   This was the blue book way of doing things... 
 */
STATIC INLINE int
sl_lsc_continue(queue_t *q, struct sl *sl, mblk_t *mp)
{
	switch (sl_get_state(sl)) {
	case MS_PROCESSOR_OUTAGE:
		if (sl->flags & MF_CONTINUE) {
			sl->flags |= MF_NEED_FLUSH;
			sl->flags |= MF_RECV_MSU;
			sl->flags |= MF_SEND_MSU;
			if (sl->rtb.q_count) {
				if (sl->flags & MF_REM_BUSY)
					sl_timer_start(sl, t6);
				else
					sl_timer_start(sl, t7);
			}
			sl->flags &= ~MF_LPO;
			sl_is_stats(q, sl);
			sl_set_state(sl, MS_IN_SERVICE);
		}
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_CONTINUE_REQ in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}
#endif

STATIC INLINE int
sl_lsc_status_processor_outage_ended(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_PROCESSOR_OUTAGE:
		if (sl->flags & MF_RPO) {
			if (sl->i_version >= M2PA_VERSION_DRAFT10) {
				/* 
				   In the final, when we receive PROCESSOR_RECOVERED, we need to
				   send READY, and also use the BSN from the received message to
				   synchronize FSN. */
				sl->fsnt = ntohl(*((uint32_t *) mp->b_rptr)) & 0xffffff;
				if ((err = sl_send_status(q, sl, M2PA_STATUS_IN_SERVICE)))
					return (err);
			}
			if (sl->sl.notify.events & SL_EVT_RPO_END)
				if ((err = lmi_event_ind(q, sl, SL_EVT_RPO_END, 0, NULL, 0)) < 0)
					return (err);
			if ((err = sl_remote_processor_recovered_ind(q, sl)) < 0)
				return (err);
			sl->flags &= ~MF_RPO;
			ptrace(("%s: %p: RPO Ends\n", MOD_NAME, sl));
		}
		sl_rpr_stats(q, sl);
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			/* 
			   auto recover */
			if (!(sl->flags & MF_LPO))
				sl_lsc_no_processor_outage(q, sl);
			break;
		case SS7_PVAR_ANSI_92:
			/* 
			   leave state latched */
			break;
		}
		return (QR_DONE);
	}
	ptrace(("%s: %p: Received status PROCESSOR_OUTAGE_ENDED in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_resume(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
	case MS_ALIGNED:
	case MS_PROVING:
		/* 
		   remember for later */
		sl->flags &= ~MF_LPO;
		return (0);
	case MS_ALIGNED_READY:
		fixme(("%s: %p: FIXME: This is really an error...\n", MOD_NAME, sl));
		return (0);
	case MS_ALIGNED_NOT_READY:
		sl->flags &= ~MF_LPO;
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_96:
			if (!(sl->flags & MF_RPO))
				sl_lsc_no_processor_outage(q, sl);
			sl->flags |= MF_RECV_MSU;	/* ??? */
			break;
		case SS7_PVAR_ANSI_92:
			break;
		default:
			if (!(sl->flags & MF_RPO))
				sl_lsc_no_processor_outage(q, sl);
			break;
		}
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_SEND_MSU;
		sl_set_state(sl, MS_ALIGNED_READY);
		return (0);
	case MS_PROCESSOR_OUTAGE:
		/* 
		   This is where the really wierd behavior begins... */
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			if (sl->flags & MF_LPO) {
				if ((err =
				     sl_send_status(q, sl, M2PA_STATUS_PROCESSOR_OUTAGE_ENDED)) < 0)
					return (err);
				if (sl->i_version >= M2PA_VERSION_DRAFT10)
					sl->flags |= MF_WAIT_SYNC;
				sl->flags &= ~MF_LPO;
			}
			if (!(sl->flags & MF_RPO))
				sl_lsc_no_processor_outage(q, sl);
			sl->rack += xchg(&sl->rmsu, 0);
			sl->flags &= ~MF_CONG_DISCARD;
			sl->flags &= ~MF_CONG_ACCEPT;
			if (sl->flags & MF_LOC_BUSY)
				if ((err = sl_send_status(q, sl, M2PA_STATUS_BUSY_ENDED)) < 0)
					return (err);
			sl->flags &= ~MF_LOC_BUSY;
			sl_timer_stop(sl, t7);
			sl_timer_stop(sl, t6);
			sl->flags &= ~MF_SEND_MSU;
			return (0);
		case SS7_PVAR_ANSI_92:
			/* 
			   XXX do we do this now??? */
			if (sl->flags & MF_LPO) {
				if ((err =
				     sl_send_status(q, sl, M2PA_STATUS_PROCESSOR_OUTAGE_ENDED)) < 0)
					return (err);
				if (sl->i_version >= M2PA_VERSION_DRAFT10)
					sl->flags |= MF_WAIT_SYNC;
				sl->flags &= ~MF_LPO;
			}
			sl->flags |= MF_RECV_MSU;
			if (sl->flags & MF_RPO) {
				sl_timer_stop(sl, t7);
				sl_timer_stop(sl, t6);
				sl->flags &= ~MF_SEND_MSU;
				fixme(("%s: %p: FIXME: We should not do this...\n", MOD_NAME, sl));
				if ((err = sl_remote_processor_outage_ind(q, sl)) < 0)
					return (err);
				return (0);
			}
			sl->flags |= MF_SEND_MSU;
			if (sl->rtb.q_count) {
				if (sl->flags & MF_REM_BUSY)
					sl_timer_start(sl, t6);
				else
					sl_timer_start(sl, t7);
			}
			break;
		}
		sl_is_stats(q, sl);
		sl_set_state(sl, MS_IN_SERVICE);
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_RESUME_REQ in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_status_out_of_service(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl_get_state(sl)) {
	case MS_ALIGNED_READY:
	case MS_ALIGNED_NOT_READY:
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		sl->flags &= ~MF_REM_INS;
		if (sl->sl.notify.events & SL_EVT_FAIL_RECEIVED_SIOS)
			if ((err = lmi_event_ind(q, sl, SL_EVT_FAIL_RECEIVED_SIOS, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(q, sl, SL_FAIL_RECEIVED_SIOS)) < 0)
			return (err);
		ptrace(("%s: %p: Link failed: Received SIOS\n", MOD_NAME, sl));
		return (QR_DONE);
	case MS_ALIGNED:
	case MS_PROVING:
		sl->flags &= ~MF_REM_INS;
		if (sl->sl.notify.events & SL_EVT_FAIL_ALIGNMENT)
			if ((err = lmi_event_ind(q, sl, SL_EVT_FAIL_ALIGNMENT, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(q, sl, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)) < 0)
			return (err);
		ptrace(("%s: %p: Link failed: Alignment not possible\n", MOD_NAME, sl));
		sl->sl.stats.sl_fail_align_or_proving++;
		return (QR_DONE);
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
		return (QR_DONE);
	}
	ptrace(("%s: %p: Received status OUT_OF_SERVICE in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_retrieve_bsnt(queue_t *q, struct sl *sl)
{
	switch (sl_get_state(sl)) {
	case MS_OUT_OF_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		sl_bsnt_ind(q, sl, sl->fsnr);
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_RETRIEVE_BSNT_REQ in unexpected state %lu\n",
		MOD_NAME, sl, sl_get_state(sl)));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_retrieval_request_and_fsnc(queue_t *q, struct sl *sl, ulong fsnc)
{
	int err;
	mblk_t *mp;
	switch (sl_get_state(sl)) {
	case MS_OUT_OF_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		/* 
		 *  FIXME: Q.704/5.7.2 states:
		 *
		 *  5.7.2   If a changeover order or acknowledgement
		 *  containing an unreasonable value of the forward sequence
		 *  number is received, no buffer updating or retrieval is
		 *  performed, and new traffic is started on the alternative
		 *  signalling link(s).
		 *
		 *  It will be necessary to check FSNC for "reasonableness"
		 *  here and flush RTB and TB and return retrieval-complete
		 *  indication with a return code of "unreasonable FSNC".
		 */
		fixme(("%s: %p: FIXME: Fix this check...\n", MOD_NAME, sl));
		/* 
		   this will pretty much clear the rtb if there is a problem with the FSNC */
		while (sl->rtb.q_msgs && sl->fsnt != fsnc) {
			freemsg(bufq_dequeue(&sl->rtb));
			sl->fsnt--;
			sl->fsnt &= 0xffffff;
		}
		while ((mp = bufq_dequeue(&sl->rtb))) {
			if ((err = sl_retrieved_message_ind(q, sl, mp)) < 0) {
				bufq_queue_head(&sl->rtb, mp);
				return (err);
			}
			sl->flags &= ~MF_RTB_FULL;
		}
		while ((mp = bufq_dequeue(&sl->tb))) {
			if ((err = sl_retrieved_message_ind(q, sl, mp)) < 0) {
				bufq_queue_head(&sl->tb, mp);
				return (err);
			}
		}
		if ((err = sl_retrieval_complete_ind(q, sl)) < 0)
			return (err);
		return (0);
	}
	ptrace(("%s: %p: Received primitive SL_RETRIEVAL_REQUEST_AND_FSNC_REQ in unexpected state %lu\n", MOD_NAME, sl, sl_get_state(sl)));
	if ((err = sl_retrieval_not_possible_ind(q, sl)) < 0)
		return (err);
	return (-EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  RECEIVE CONGESTION ALGORITHM
 *
 *  --------------------------------------------------------------------------
 *
 *  These congestion functions are implementation dependent.  We should define
 *  a congestion onset level and set congestion accept at that point.  We
 *  should also define a second congestion onset level and set congestion
 *  discard at that point.  For STREAMS, the upstream congestion can be
 *  detected in two ways: 1) canputnext(): is the upstream module flow
 *  controlled; and, 2) canput(): are we flow controlled.  If the upstream
 *  module is flow controlled, then we can accept MSUs and place them on our
 *  own read queue.  If we are flow contolled, then we have no choice but to
 *  discard the message.  In addition, and because upstream message processing
 *  times are likely more sensitive to the number of backlogged messages than
 *  they are to the number of backlogged message octets, we have some
 *  configurable thresholds of backlogging and keep track of backlogged
 *  messages.
 *
 *  --------------------------------------------------------------------------
 */
STATIC INLINE int
sl_rb_congestion_function(queue_t *q, struct sl *sl)
{
	int err;
	if (!(sl->flags & MF_L3_CONG_DETECT)) {
		if (sl->flags & MF_L2_CONG_DETECT) {
			if (sl->rb.q_msgs <= sl->sl.config.rb_abate && canputnext(sl->oq)) {
				if (sl->flags & MF_LOC_BUSY)
					if ((err =
					     sl_send_status(q, sl, M2PA_STATUS_BUSY_ENDED)) < 0)
						return (err);
				sl->flags &= ~MF_LOC_BUSY;
				sl->flags &= ~MF_CONG_DISCARD;
				sl->flags &= ~MF_CONG_ACCEPT;
				sl->rack += xchg(&sl->rmsu, 0);
				sl->flags &= ~MF_L2_CONG_DETECT;
			}
		} else {
			if (sl->rb.q_msgs >= sl->sl.config.rb_discard || !canput(sl->oq)) {
				sl->flags |= MF_CONG_DISCARD;
				if (!(sl->flags & MF_LOC_BUSY)) {
					if ((err = sl_send_status(q, sl, M2PA_STATUS_BUSY)) < 0)
						return (err);
					printd(("%s: %p: Receive congestion: congestion discard\n",
						MOD_NAME, sl));
					printd(("%s:   sl->rb.q_msgs           = %12u\n",
						MOD_NAME, sl->rb.q_msgs));
					printd(("%s:   sl->sl.config.rb_discard   = %12lu\n",
						MOD_NAME, sl->sl.config.rb_discard));
					printd(("%s:   canput                  = %12s\n",
						MOD_NAME, canput(sl->oq) ? "YES" : "NO"));
					sl->sl.stats.sl_sibs_sent++;
					sl->flags |= MF_LOC_BUSY;
				}
				sl->flags |= MF_L2_CONG_DETECT;
			} else {
				if (sl->rb.q_msgs >= sl->sl.config.rb_accept || !canputnext(sl->oq)) {
					sl->flags |= MF_CONG_ACCEPT;
					if (!(sl->flags & MF_LOC_BUSY)) {
						if ((err =
						     sl_send_status(q, sl, M2PA_STATUS_BUSY)) < 0)
							return (err);
						printd(("%s: %p: Receive congestion: congestion accept\n", MOD_NAME, sl));
						printd(("%s:   sl->rb.q_msgs           = %12u\n",
							MOD_NAME, sl->rb.q_msgs));
						printd(("%s:   sl->sl.config.rb_accept    = %12lu\n", MOD_NAME, sl->sl.config.rb_accept));
						printd(("%s:   cantputnext?            = %12s\n",
							MOD_NAME,
							canputnext(sl->oq) ? "YES" : "NO"));
						sl->sl.stats.sl_sibs_sent++;
						sl->flags |= MF_LOC_BUSY;
					}
					sl->flags |= MF_L2_CONG_DETECT;
				}
			}
		}
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  TRANSMIT CONGESTION ALGORITHM
 *
 *  -------------------------------------------------------------------------
 *
 *  The transmit congestion algorithm is an implementation dependent algorithm
 *  but is suggested as being based on TB and/or RTB buffer occupancy.  With
 *  STREAMS we can use octet count buffer occupancy over message count
 *  occupancy, because congestion in transmission is more related to octet
 *  count (because it determines transmission latency).
 *
 *  We check the total buffer occupancy and apply the necessary congestion
 *  control signal as per configured abatement, onset and discard thresholds.
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_check_congestion(queue_t *q, struct sl *sl)
{
	int err;
	unsigned int occupancy = sl->tb.q_count + sl->rtb.q_count;
	int old_cong_status = sl->sl.statem.cong_status;
	int old_disc_status = sl->sl.statem.disc_status;
	int multi = sl->option.popt & SS7_POPT_MPLEV;
	switch (sl->sl.statem.cong_status) {
	case 0:
		if (occupancy >= sl->sl.config.tb_onset_1) {
			sl->sl.statem.cong_status = 1;
			if (occupancy >= sl->sl.config.tb_discd_1) {
				sl->sl.statem.disc_status = 1;
				if (!multi)
					break;
				if (occupancy >= sl->sl.config.tb_onset_2) {
					sl->sl.statem.cong_status = 2;
					if (occupancy >= sl->sl.config.tb_discd_2) {
						sl->sl.statem.disc_status = 2;
						if (occupancy >= sl->sl.config.tb_onset_3) {
							sl->sl.statem.cong_status = 3;
							if (occupancy >= sl->sl.config.tb_discd_3) {
								sl->sl.statem.disc_status = 3;
							}
						}
					}
				}
			}
		}
		break;
	case 1:
		if (occupancy < sl->sl.config.tb_abate_1) {
			sl->sl.statem.cong_status = 0;
			sl->sl.statem.disc_status = 0;
		} else {
			if (!multi)
				break;
			if (occupancy >= sl->sl.config.tb_onset_2) {
				sl->sl.statem.cong_status = 2;
				if (occupancy >= sl->sl.config.tb_discd_2) {
					sl->sl.statem.disc_status = 2;
					if (occupancy >= sl->sl.config.tb_onset_3) {
						sl->sl.statem.cong_status = 3;
						if (occupancy >= sl->sl.config.tb_discd_3) {
							sl->sl.statem.disc_status = 3;
						}
					}
				}
			}
		}
		break;
	case 2:
		if (!multi) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->sl.config.tb_abate_2) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			if (occupancy < sl->sl.config.tb_abate_1) {
				sl->sl.statem.cong_status = 0;
				sl->sl.statem.disc_status = 0;
			}
		} else if (occupancy >= sl->sl.config.tb_onset_3) {
			sl->sl.statem.cong_status = 3;
			if (occupancy >= sl->sl.config.tb_discd_3) {
				sl->sl.statem.disc_status = 3;
			}
		}
		break;
	case 3:
		if (!multi) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->sl.config.tb_abate_3) {
			sl->sl.statem.cong_status = 2;
			sl->sl.statem.disc_status = 2;
			if (occupancy < sl->sl.config.tb_abate_2) {
				sl->sl.statem.cong_status = 1;
				sl->sl.statem.disc_status = 1;
				if (occupancy < sl->sl.config.tb_abate_1) {
					sl->sl.statem.cong_status = 0;
					sl->sl.statem.disc_status = 0;
				}
			}
		}
		break;
	}
	if (sl->sl.statem.cong_status != old_cong_status
	    || sl->sl.statem.disc_status != old_disc_status) {
		if (sl->sl.statem.cong_status < old_cong_status)
			sl_link_congestion_ceased_ind(q, sl, sl->sl.statem.cong_status,
						      sl->sl.statem.disc_status);
		else {
			if (sl->sl.statem.cong_status > old_cong_status) {
				if (sl->sl.notify.events & SL_EVT_CONGEST_ONSET_IND
				    && !sl->sl.stats.sl_cong_onset_ind[sl->sl.statem.cong_status]++)
					if ((err =
					     lmi_event_ind(q, sl, SL_EVT_CONGEST_ONSET_IND, 0,
							   &sl->sl.statem.cong_status,
							   sizeof(sl->sl.statem.cong_status))) < 0)
						return (err);
				ptrace(("%s: %p: Congestion onset: level %ld\n", MOD_NAME,
					sl, sl->sl.statem.cong_status));
			} else {
				if (sl->sl.notify.events & SL_EVT_CONGEST_DISCD_IND
				    && !sl->sl.stats.sl_cong_discd_ind[sl->sl.statem.disc_status]++)
					if ((err =
					     lmi_event_ind(q, sl, SL_EVT_CONGEST_DISCD_IND, 0,
							   &sl->sl.statem.disc_status,
							   sizeof(sl->sl.statem.disc_status))) < 0)
						return (err);
				ptrace(("%s: %p: Congestion discard: level %ld\n", MOD_NAME,
					sl, sl->sl.statem.cong_status));
			}
			sl_link_congested_ind(q, sl, sl->sl.statem.cong_status,
					      sl->sl.statem.disc_status);
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  INPUT Events
 *
 *  =========================================================================
 *  -------------------------------------------------------------------------
 *
 *  SL Peer -> SL Peer (M2PA) Received Messages
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  M2PA RECV ACK
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_recv_datack(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	uint count;
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3_1:
	{
		size_t mlen = mp->b_wptr - mp->b_rptr;
		if (mlen >= sizeof(uint32_t)) {
			if ((count = ntohl(*(uint32_t *) mp->b_wptr++))) {
				if ((err = sl_txc_datack(q, sl, count)) < 0)
					return (err);
				return (QR_DONE);
			}
			rare();
			return (-EPROTO);
		}
		rare();
		return (-EMSGSIZE);
	}
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
		ptrace(("%s: %p: ERROR: Invalid message for version\n", MOD_NAME, sl));
		return (-EPROTO);
	}
}

/*
 *  M2PA RECV PROVING
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_recv_proving(queue_t *q, struct sl *sl, mblk_t *mp)
{
	(void) mp;
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		/* 
		   just ignore proving packets */
		return (QR_DONE);
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
		ptrace(("%s: %p: ERROR: Invalid message for version\n", MOD_NAME, sl));
		return (-EPROTO);
	}
}

/*
 *  M2PA RECV STATUS
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_recv_status(queue_t *q, struct sl *sl, mblk_t *mp)
{
	size_t hlen;			/* M2PA-specific header length */
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
	case M2PA_VERSION_DRAFT6_9:
		hlen = 0;
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		hlen = 2 * sizeof(uint16_t);
		break;
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
		/* 
		   In the final analysis, sequence numbers are included in status messages. */
		hlen = 2 * sizeof(uint32_t);
		break;
	}
	if (mlen >= sizeof(uint32_t)) {
		uint32_t status = *((uint32_t *) (mp->b_rptr + hlen));
		switch (status) {
		case M2PA_STATUS_BUSY_ENDED:
			return sl_lsc_status_busy_ended(q, sl);
		case M2PA_STATUS_IN_SERVICE:
			return sl_lsc_status_in_service(q, sl, mp);
		case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
			return sl_lsc_status_processor_outage_ended(q, sl, mp);
		case M2PA_STATUS_OUT_OF_SERVICE:
			return sl_lsc_status_out_of_service(q, sl);
		case M2PA_STATUS_PROCESSOR_OUTAGE:
			return sl_lsc_status_processor_outage(q, sl);
		case M2PA_STATUS_BUSY:
			return sl_lsc_status_busy(q, sl);
		case M2PA_STATUS_ALIGNMENT:
			return sl_lsc_status_alignment(q, sl);
		case M2PA_STATUS_PROVING_NORMAL:
			return sl_lsc_status_proving_normal(q, sl);
		case M2PA_STATUS_PROVING_EMERGENCY:
			return sl_lsc_status_proving_emergency(q, sl);
		case M2PA_STATUS_ACK:
			return sl_rc_signal_unit(q, sl, mp);
		}
		rare();
		return (-EPROTO);
	}
	rare();
	return (-EMSGSIZE);
}

/*
 *  M2PA RECV DATA
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_recv_data(queue_t *q, struct sl *sl, mblk_t *mp)
{
	size_t hlen;			/* M2PA-specific header length */
	size_t rlen;			/* Routing Label length */
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		hlen = 0;
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		hlen = 2 * sizeof(uint16_t);
		break;
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
		/* 
		   In the final, sequence numbers are 24 bit values in a 32 bit field. */
		hlen = 2 * sizeof(uint32_t);
		break;
	}
	switch (sl->option.pvar) {
	default:
	case SS7_PVAR_ITUT_88:
	case SS7_PVAR_ITUT_93:
	case SS7_PVAR_ITUT_96:
	case SS7_PVAR_ITUT_00:
	case SS7_PVAR_ETSI_88:
	case SS7_PVAR_ETSI_93:
	case SS7_PVAR_ETSI_96:
	case SS7_PVAR_ETSI_00:
		rlen = 5;
		break;
	case SS7_PVAR_ANSI_88:
	case SS7_PVAR_ANSI_92:
	case SS7_PVAR_ANSI_96:
	case SS7_PVAR_ANSI_00:
	case SS7_PVAR_JTTC_94:
		rlen = 7;
		break;
	}
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
		if (mlen > hlen + rlen || mlen == 0)
			return sl_rc_signal_unit(q, sl, mp);
		break;
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
		/* 
		   draft6 and anticipated draft7 permit acknowledgement using zero payload data
		   messages. */
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
		/* 
		   In the final, zero-length data messages are used as spearate acknowledgements. */
		if (mlen > hlen + rlen || mlen == hlen || mlen == 0)
			return sl_rc_signal_unit(q, sl, mp);
		break;
	}
	rare();
	return (-EMSGSIZE);
}

/*
 *  M2PA RECV
 *  ---------------------------------------------
 */
STATIC int
sl_recv_msg(queue_t *q, struct sl *sl, mblk_t *mp)
{
	size_t mlen;
	uint32_t type;
	if (mp->b_wptr >= mp->b_rptr + 2 * sizeof(uint32_t)) {
		int err;
		unsigned char *oldp = mp->b_rptr;
		type = *(uint32_t *) mp->b_rptr++;
		mlen = ntohl(*(uint32_t *) mp->b_rptr++) - 2 * sizeof(uint32_t);
		switch (type) {
		case M2PA_DATA_MESSAGE:
			err = sl_recv_data(q, sl, mp);
			break;
		case M2PA_STATUS_MESSAGE:
			if (mp->b_wptr - mp->b_rptr != PAD4(mlen))
				goto emsgsize;
			err = sl_recv_status(q, sl, mp);
			break;
		case M2PA_PROVING_MESSAGE:
			if (mp->b_wptr - mp->b_rptr != PAD4(mlen))
				goto emsgsize;
			err = sl_recv_proving(q, sl, mp);
			break;
		case M2PA_ACK_MESSAGE:
			if (mp->b_wptr - mp->b_rptr != PAD4(mlen))
				goto emsgsize;
			err = sl_recv_datack(q, sl, mp);
			break;
		default:
			rare();
			return (-EPROTO);
		}
		if (err < 0)
			mp->b_rptr = oldp;
		return (err);
	      emsgsize:
		ptrace(("%s: %p: Bad message size %d != %d\n", MOD_NAME, sl,
			mp->b_wptr - mp->b_rptr, PAD4(mlen)));
		return (-EMSGSIZE);
	}
	rare();
	return (-EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL User -> SL Provider (M2PA) Primitives
 *
 *  -------------------------------------------------------------------------
 *
 *  LMI_INFO_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_info_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	return lmi_info_ack(q, sl);
}

/*
 *  LMI_ATTACH_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_attach_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_attach_req_t *p = (lmi_attach_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		if (sl->i_state == LMI_UNATTACHED) {
			sl->i_state = LMI_ATTACH_PENDING;
			ptrace(("%s: %p: PROTO: Primitive is not supported\n", MOD_NAME, sl));
			err = LMI_NOTSUPP;
		} else {
			ptrace(("%s: %p: PROTO: would place interface out of state\n",
				MOD_NAME, sl));
			err = LMI_OUTSTATE;
		}
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return lmi_error_ack(q, sl, LMI_ATTACH_REQ, err);
}

/*
 *  LMI_DETACH_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_detach_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_detach_req_t *p = (lmi_detach_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		if (sl->i_state == LMI_DISABLED) {
			sl->i_state = LMI_DETACH_PENDING;
			ptrace(("%s: %p: PROTO: Primitive is not supported\n", MOD_NAME, sl));
			err = LMI_NOTSUPP;
		} else {
			ptrace(("%s: %p: PROTO: would place interface out of state\n",
				MOD_NAME, sl));
			err = LMI_OUTSTATE;
		}
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return lmi_error_ack(q, sl, LMI_DETACH_REQ, err);
}

/*
 *  LMI_ENABLE_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_enable_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_enable_req_t *p = (lmi_enable_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		if (sl->i_state == LMI_DISABLED) {
			sl->i_state = LMI_ENABLE_PENDING;
			return lmi_enable_con(q, sl);
		} else {
			ptrace(("%s: %p: PROTO: would place interface out of state\n",
				MOD_NAME, sl));
			err = LMI_OUTSTATE;
		}
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return lmi_error_ack(q, sl, LMI_ENABLE_REQ, err);
}

/*
 *  LMI_DISABLE_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_disable_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_disable_req_t *p = (lmi_disable_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		if (sl->i_state == LMI_ENABLED || sl->i_state == LMI_ENABLE_PENDING) {
			if ((err = sl_lsc_stop(q, sl)) < 0)
				return (err);
			sl->i_state = LMI_DISABLE_PENDING;
			return lmi_disable_con(q, sl);
		} else {
			ptrace(("%s: %p: PROTO: would place interface out of state\n",
				MOD_NAME, sl));
			err = LMI_OUTSTATE;
		}
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return lmi_error_ack(q, sl, LMI_DISABLE_REQ, err);
}

/*
 *  LMI_OPTMGMT_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_optmgmt_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_optmgmt_req_t *p = (lmi_optmgmt_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		ptrace(("%s: %p: PROTO: Primitive is not supported\n", MOD_NAME, sl));
		err = LMI_NOTSUPP;
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return lmi_error_ack(q, sl, LMI_OPTMGMT_REQ, err);
}

/*
 *  M_ERROR Reply
 *  ---------------------------------------------
 */
#ifndef abs
#define abs(x) ((x)<0 ? -(x):(x))
#endif
STATIC int
m_error_reply(queue_t *q, struct sl *sl, int err)
{
	mblk_t *mp;
	switch (err) {
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
	case QR_LOOP:
	case QR_PASSALONG:
	case QR_PASSFLOW:
	case QR_DISABLE:
		ptrace(("%s: %p: WARNING: Shouldn't pass Q returns to m_error_reply\n",
			MOD_NAME, sl));
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	}
	if ((mp = ss7_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = abs(err);
		*(mp->b_wptr)++ = abs(err);
		printd(("%s: %p: <- M_ERROR\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_PDU_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_pdu_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	mblk_t *dp;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_pdu_req_t *p = (sl_pdu_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		if ((dp = mp->b_cont)) {
			if ((err = sl_lsc_pdu(q, sl, dp)) < 0)
				return (err);
			return (QR_TRIMMED);	/* data absorbed */
		} else {
			ptrace(("%s: %p: PROTO: No M_DATA block\n", MOD_NAME, sl));
			err = LMI_BADPRIM;
		}
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_EMERGENCY_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_emergency_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_emergency_req_t *p = (sl_emergency_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_emergency(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_emergency_ceases_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_emergency_ceases_req_t *p = (sl_emergency_ceases_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_emergency_ceases(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_START_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_start_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_start_req_t *p = (sl_start_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_start(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_STOP_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_stop_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_stop_req_t *p = (sl_stop_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_stop(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_retrieve_bsnt_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_retrieve_bsnt_req_t *p = (sl_retrieve_bsnt_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_retrieve_bsnt(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_retrieval_request_and_fsnc_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_retrieval_req_and_fsnc_t *p = (sl_retrieval_req_and_fsnc_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_retrieval_request_and_fsnc(q, sl, p->sl_fsnc);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_RESUME_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_resume_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_resume_req_t *p = (sl_resume_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_resume(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_clear_buffers_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_clear_buffers_req_t *p = (sl_clear_buffers_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_clear_buffers(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_clear_rtb_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_clear_rtb_req_t *p = (sl_clear_rtb_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_clear_rtb(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_local_processor_outage_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_local_proc_outage_req_t *p = (sl_local_proc_outage_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_local_processor_outage(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_congestion_discard_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_cong_discard_req_t *p = (sl_cong_discard_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_congestion_discard(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_congestion_accept_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_cong_accept_req_t *p = (sl_cong_accept_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_congestion_accept(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_no_congestion_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_no_cong_req_t *p = (sl_no_cong_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_no_congestion(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  SL_POWER_ON_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_power_on_req(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sl_power_on_req_t *p = (sl_power_on_req_t *) mp->b_rptr;
	if (mlen >= sizeof(*p)) {
		return sl_lsc_power_on(q, sl);
	} else {
		ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, sl));
		err = LMI_PROTOSHORT;
	}
	return m_error_reply(q, sl, err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NPI Provider -> NPI User (M2PA) Primitives
 *
 *  -------------------------------------------------------------------------
 *
 *  N_DATA_IND
 *  ---------------------------------------------
 */
STATIC int
n_data_ind_slow(queue_t *q, struct sl *sl, mblk_t *dp, ulong more)
{
	/* 
	   Normally we receive data unfragmented and in a singled M_DATA block.  This slower
	   routine handles the circumstance where we receive fragmented data or data that is
	   chained together in multiple M_DATA blocks. */
	mblk_t *newp = NULL;
	seldom();
	if (dp->b_cont) {
		/* 
		   We have chained M_DATA blocks: pull them up. */
		if (!pullupmsg(dp, -1)) {
			/* 
			   normall only fail because of dup'ed blocks */
			if (!(newp = msgpullup(dp, -1))) {
				/* 
				   normaly only fail because of no buffer */
				ss7_bufcall(q, xmsgsize(dp), BPRI_MED);
				return (-ENOBUFS);
			}
			dp = newp;
		}
	}
	if (more) {
		/* 
		   We have a partial delivery.  Chain normal message together.  We might have a
		   problem with messages split over multiple streams? Treat normal and expedited
		   separately. */
		if (sl->rbuf)
			linkb(sl->rbuf, dp);
		else
			sl->rbuf = dp;
	} else {
		int err;
		if (sl->rbuf) {
			linkb(sl->rbuf, dp);
			dp = xchg(&sl->rbuf, NULL);
		}
		if ((err = sl_recv_msg(q, sl, dp)) == QR_ABSORBED)
			return (newp ? QR_DONE : QR_TRIMMED);
		return (err);
	}
	return (newp ? QR_DONE : QR_TRIMMED);
}
STATIC INLINE int
n_data_ind(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	mblk_t *dp;
	N_data_ind_t *p = ((typeof(p)) mp->b_rptr);
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
	if (mlen >= sizeof(*p)) {
		if ((dp = mp->b_cont)) {
			if (sl->i_state == LMI_ENABLED) {
				if (!(p->DATA_xfer_flags & N_MORE_DATA_FLAG) && !dp->b_cont) {
					if ((err = sl_recv_msg(q, sl, dp)) == QR_ABSORBED)
						return (QR_TRIMMED);	/* data only */
					return (err);
				} else
					return n_data_ind_slow(q, sl, dp,
							       p->
							       DATA_xfer_flags & N_MORE_DATA_FLAG);
			} else {
				ptrace(("%s: %p: ERROR: Ignore data in other states\n",
					MOD_NAME, sl));
				return (-EPROTO);
			}
		} else {
			ptrace(("%s: %p: ERROR: No data\n", MOD_NAME, sl));
			return (-EFAULT);
		}
	} else {
		ptrace(("%s: %p: ERROR: M_PROTO block too short\n", MOD_NAME, sl));
		return (-EFAULT);
	}
}

/*
 *  N_EXDATA_IND
 *  ---------------------------------------------
 */
STATIC int
n_exdata_ind_slow(queue_t *q, struct sl *sl, mblk_t *dp)
{
	/* 
	   Normally we receive data unfragmented and in a singled M_DATA block.  This slower
	   routine handles the circumstance where we receive fragmented data or data that is
	   chained together in multiple M_DATA blocks. */
	int err;
	mblk_t *newp = NULL;
	seldom();
	if (dp->b_cont) {
		/* 
		   We have chained M_DATA blocks: pull them up. */
		if (!pullupmsg(dp, -1)) {
			/* 
			   normall only fail because of dup'ed blocks */
			if (!(newp = msgpullup(dp, -1))) {
				/* 
				   normaly only fail because of no buffer */
				ss7_bufcall(q, xmsgsize(dp), BPRI_MED);
				return (-ENOBUFS);
			}
			dp = newp;
		}
	}
	if ((err = sl_recv_msg(q, sl, dp)) == QR_ABSORBED)
		return (newp ? QR_DONE : QR_TRIMMED);
	if (err < 0 && newp)
		freemsg(newp);
	return (err);
}
STATIC INLINE int
n_exdata_ind(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	mblk_t *dp;
	ensure(mp->b_wptr - mp->b_rptr >= sizeof(N_exdata_ind_t), return (-EFAULT));
	if ((dp = mp->b_cont)) {
		if (sl->i_state == LMI_ENABLED) {
			if (!dp->b_cont) {
				if ((err = sl_recv_msg(q, sl, dp)) == QR_ABSORBED)
					return (QR_TRIMMED);	/* data only */
				return (err);
			} else
				return n_exdata_ind_slow(q, sl, dp);
		}
		ptrace(("%s: %p: ERROR: Ignore data in other states\n", MOD_NAME, sl));
		return (-EPROTO);
	}
	ptrace(("%s: %p: ERROR: No data\n", MOD_NAME, sl));
	return (-EFAULT);
}

/*
 *  N_DATACK_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
n_datack_ind(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	ensure(mp->b_wptr - mp->b_rptr >= sizeof(N_datack_ind_t), return (-EFAULT));
	if (sl->i_version < M2PA_VERSION_DRAFT3_1) {
		if ((err = sl_txc_datack(q, sl, 1)) < 0)
			return (err);
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  N_DISCON_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
n_discon_ind(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	ensure(mp->b_wptr - mp->b_rptr >= sizeof(N_discon_ind_t), return (-EFAULT));
	if (sl->sl.notify.events & SL_EVT_FAIL_SUERM_EIM)
		if ((err = lmi_event_ind(q, sl, SL_EVT_FAIL_SUERM_EIM, 0, NULL, 0)) < 0)
			return (err);
	if ((err = sl_lsc_out_of_service(q, sl, SL_FAIL_SUERM_EIM)) < 0)
		return (err);
	ptrace(("%s: %p: Link failed: SUERM/EIM\n", MOD_NAME, sl));
	fixme(("%s: %p: FIXME: Do something here... send a HANGUP or something...\n",
	       MOD_NAME, sl));
	return (0);
}

/*
 *  N_RESET_IND
 *  ---------------------------------------------
 */
STATIC INLINE int
n_reset_ind(queue_t *q, struct sl *sl, mblk_t *mp)
{
	ensure(mp->b_wptr - mp->b_rptr >= sizeof(N_reset_ind_t), return (-EFAULT));
	fixme(("%s: %p: FIXME: Do something here...\n", MOD_NAME, sl));
	return (-EFAULT);
}

/*
 *  Others...
 *  ---------------------------------------------
 */
STATIC INLINE int
n_other_ind(queue_t *q, struct sl *sl, mblk_t *mp)
{
	fixme(("%s: %p: FIXME: Do something here...\n", MOD_NAME, sl));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *
 *  SL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int
sl_test_config(struct sl *sl, sl_config_t * arg)
{
	fixme(("%s: %p: FIXME: write this function\n", MOD_NAME, sl));
	return (-EOPNOTSUPP);
}
STATIC int
sl_commit_config(struct sl *sl, sl_config_t * arg)
{
	fixme(("%s: %p: FIXME: write this function\n", MOD_NAME, sl));
	return (-EOPNOTSUPP);
}
#endif
STATIC int
sl_iocgoptions(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsoptions(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioctconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstatem(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.statem;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccmreset(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			bzero(&sl->sl.statem, sizeof(sl->sl.statem));
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstatsp(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.statsp;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsstatsp(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.statsp = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstats(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.stats;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccstats(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			bzero(&sl->sl.stats, sizeof(sl->sl.stats));
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.notify;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.notify.events |= arg->events;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.notify.events &= ~(arg->events);
		}
		spin_unlock_irqrestore(&sl->lock, flags);
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
sdt_test_config(struct sl *sl, sdt_config_t * arg)
{
	int ret = 0;
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	do {
		if (!arg->t8)
			arg->t8 = sl->sdt.config.t8;
		if (!arg->Tin)
			arg->Tin = sl->sdt.config.Tin;
		if (!arg->Tie)
			arg->Tie = sl->sdt.config.Tie;
		if (!arg->T)
			arg->T = sl->sdt.config.T;
		if (!arg->D)
			arg->D = sl->sdt.config.D;
		if (!arg->Te)
			arg->Te = sl->sdt.config.Te;
		if (!arg->De)
			arg->De = sl->sdt.config.De;
		if (!arg->Ue)
			arg->Ue = sl->sdt.config.Ue;
		if (!arg->N)
			arg->N = sl->sdt.config.N;
		if (!arg->m)
			arg->m = sl->sdt.config.m;
		if (!arg->b)
			arg->b = sl->sdt.config.b;
		if (!arg->f)
			arg->f = sl->sdt.config.f;
	} while (0);
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}
STATIC int
sdt_commit_config(struct sl *sl, sdt_config_t * arg)
{
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		sdt_test_config(sl, arg);
		sl->sdt.config = *arg;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (0);
}
STATIC int
sdt_iocgoptions(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsoptions(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdt.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdt.config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioctconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_test_config(sl, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_commit_config(sl, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatem(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccmreset(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatsp(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsstatsp(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstats(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccstats(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccabort(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		void *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_iocgoptions(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsoptions(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdl.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdl.config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioctconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccconfig(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstatem(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccmreset(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstatsp(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsstatsp(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstats(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccstats(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccnotify(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccdisctx(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int ret = 0;
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}
STATIC int
sdl_ioccconntx(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int ret = 0;
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
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
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
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
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, nr));
			ret = -EINVAL;
			break;
		default:
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SL_IOC_MAGIC:
	{
		if (count < size || sl->i_state == LMI_UNATTACHED) {
			ptrace(("%s: ERROR: ioctl count = %d, size = %d, state = %lu\n",
				MOD_NAME, count, size, sl->i_state));
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sl_iocgoptions(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sl_iocsoptions(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCGCONFIG):	/* sl_config_t */
			ret = sl_iocgconfig(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCSCONFIG):	/* sl_config_t */
			ret = sl_iocsconfig(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCTCONFIG):	/* sl_config_t */
			ret = sl_ioctconfig(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCCCONFIG):	/* sl_config_t */
			ret = sl_ioccconfig(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCGSTATEM):	/* sl_statem_t */
			ret = sl_iocgstatem(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCCMRESET):	/* sl_statem_t */
			ret = sl_ioccmreset(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCGSTATSP):	/* lmi_sta_t */
			ret = sl_iocgstatsp(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCSSTATSP):	/* lmi_sta_t */
			ret = sl_iocsstatsp(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCGSTATS):	/* sl_stats_t */
			ret = sl_iocgstats(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCCSTATS):	/* sl_stats_t */
			ret = sl_ioccstats(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCGNOTIFY):	/* sl_notify_t */
			ret = sl_iocgnotify(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCSNOTIFY):	/* sl_notify_t */
			ret = sl_iocsnotify(q, sl, mp);
			break;
		case _IOC_NR(SL_IOCCNOTIFY):	/* sl_notify_t */
			ret = sl_ioccnotify(q, sl, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unspported SL ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDT_IOC_MAGIC:
	{
		if (count < size || sl->i_state == LMI_UNATTACHED) {
			ptrace(("%s: ERROR: ioctl count = %d, size = %d, state = %lu\n",
				MOD_NAME, count, size, sl->i_state));
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDT_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdt_iocgoptions(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdt_iocsoptions(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCGCONFIG):	/* sdt_config_t */
			ret = sdt_iocgconfig(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCSCONFIG):	/* sdt_config_t */
			ret = sdt_iocsconfig(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCTCONFIG):	/* sdt_config_t */
			ret = sdt_ioctconfig(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCCCONFIG):	/* sdt_config_t */
			ret = sdt_ioccconfig(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATEM):	/* sdt_statem_t */
			ret = sdt_iocgstatem(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCCMRESET):	/* sdt_statem_t */
			ret = sdt_ioccmreset(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATSP):	/* lmi_sta_t */
			ret = sdt_iocgstatsp(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCSSTATSP):	/* lmi_sta_t */
			ret = sdt_iocsstatsp(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATS):	/* sdt_stats_t */
			ret = sdt_iocgstats(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCCSTATS):	/* sdt_stats_t */
			ret = sdt_ioccstats(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCGNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocgnotify(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCSNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocsnotify(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCCNOTIFY):	/* sdt_notify_t */
			ret = sdt_ioccnotify(q, sl, mp);
			break;
		case _IOC_NR(SDT_IOCCABORT):	/* */
			ret = sdt_ioccabort(q, sl, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unspported SDT ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || sl->i_state == LMI_UNATTACHED) {
			ptrace(("%s: ERROR: ioctl count = %d, size = %d, state = %lu\n",
				MOD_NAME, count, size, sl->i_state));
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdl_iocgoptions(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdl_iocsoptions(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCGCONFIG):	/* sdl_config_t */
			ret = sdl_iocgconfig(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCSCONFIG):	/* sdl_config_t */
			ret = sdl_iocsconfig(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCTCONFIG):	/* sdl_config_t */
			ret = sdl_ioctconfig(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCCCONFIG):	/* sdl_config_t */
			ret = sdl_ioccconfig(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATEM):	/* sdl_statem_t */
			ret = sdl_iocgstatem(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCCMRESET):	/* sdl_statem_t */
			ret = sdl_ioccmreset(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATSP):	/* sdl_stats_t */
			ret = sdl_iocgstatsp(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCSSTATSP):	/* sdl_stats_t */
			ret = sdl_iocsstatsp(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATS):	/* sdl_stats_t */
			ret = sdl_iocgstats(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCCSTATS):	/* sdl_stats_t */
			ret = sdl_ioccstats(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCGNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocgnotify(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCSNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocsnotify(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCCNOTIFY):	/* sdl_notify_t */
			ret = sdl_ioccnotify(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCCDISCTX):	/* */
			ret = sdl_ioccdisctx(q, sl, mp);
			break;
		case _IOC_NR(SDL_IOCCCONNTX):	/* */
			ret = sdl_ioccconntx(q, sl, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unspported SDL ioctl %d\n", MOD_NAME, nr));
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
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sl *sl = SL_PRIV(q);
	ulong oldstate = sl->i_state;
	/* 
	   Fast Path */
	if ((prim = *(ulong *) mp->b_rptr) == SL_PDU_REQ) {
		printd(("%s: %p: -> SL_PDU_REQ\n", MOD_NAME, sl));
		if ((rtn = sl_pdu_req(q, sl, mp)) < 0)
			sl->i_state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", MOD_NAME, sl));
		rtn = lmi_info_req(q, sl, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", MOD_NAME, sl));
		rtn = lmi_attach_req(q, sl, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", MOD_NAME, sl));
		rtn = lmi_detach_req(q, sl, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", MOD_NAME, sl));
		rtn = lmi_enable_req(q, sl, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", MOD_NAME, sl));
		rtn = lmi_disable_req(q, sl, mp);
		break;
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", MOD_NAME, sl));
		rtn = lmi_optmgmt_req(q, sl, mp);
		break;
	case SL_PDU_REQ:
		rtn = sl_pdu_req(q, sl, mp);
		printd(("%s: %p: -> SL_PDU_REQ\n", MOD_NAME, sl));
		break;
	case SL_EMERGENCY_REQ:
		rtn = sl_emergency_req(q, sl, mp);
		printd(("%s: %p: -> SL_EMERGENCY_REQ\n", MOD_NAME, sl));
		break;
	case SL_EMERGENCY_CEASES_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_CEASES_REQ\n", MOD_NAME, sl));
		rtn = sl_emergency_ceases_req(q, sl, mp);
		break;
	case SL_START_REQ:
		printd(("%s: %p: -> SL_START_REQ\n", MOD_NAME, sl));
		rtn = sl_start_req(q, sl, mp);
		break;
	case SL_STOP_REQ:
		printd(("%s: %p: -> SL_STOP_REQ\n", MOD_NAME, sl));
		rtn = sl_stop_req(q, sl, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		printd(("%s: %p: -> SL_RETRIEVE_BSNT_REQ\n", MOD_NAME, sl));
		rtn = sl_retrieve_bsnt_req(q, sl, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		printd(("%s: %p: -> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ\n", MOD_NAME, sl));
		rtn = sl_retrieval_request_and_fsnc_req(q, sl, mp);
		break;
	case SL_RESUME_REQ:
		printd(("%s: %p: -> SL_RESUME_REQ\n", MOD_NAME, sl));
		rtn = sl_resume_req(q, sl, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		printd(("%s: %p: -> SL_CLEAR_BUFFERS_REQ\n", MOD_NAME, sl));
		rtn = sl_clear_buffers_req(q, sl, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		printd(("%s: %p: -> SL_CLEAR_RTB_REQ\n", MOD_NAME, sl));
		rtn = sl_clear_rtb_req(q, sl, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		printd(("%s: %p: -> SL_LOCAL_PROCESSOR_OUTAGE_REQ\n", MOD_NAME, sl));
		rtn = sl_local_processor_outage_req(q, sl, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		printd(("%s: %p: -> SL_CONGESTION_DISCARD_REQ\n", MOD_NAME, sl));
		rtn = sl_congestion_discard_req(q, sl, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		printd(("%s: %p: -> SL_CONGESTION_ACCEPT_REQ\n", MOD_NAME, sl));
		rtn = sl_congestion_accept_req(q, sl, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		printd(("%s: %p: -> SL_NO_CONGESTION_REQ\n", MOD_NAME, sl));
		rtn = sl_no_congestion_req(q, sl, mp);
		break;
	case SL_POWER_ON_REQ:
		printd(("%s: %p: -> SL_POWER_ON_REQ\n", MOD_NAME, sl));
		rtn = sl_power_on_req(q, sl, mp);
		break;
	default:
		printd(("%s: %p: -> (?????)\n", MOD_NAME, sl));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		sl->i_state = oldstate;
	}
	return (rtn);
}
STATIC int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int rtn;
	ulong prim;
	switch ((prim = *((ulong *) mp->b_rptr))) {
	case N_DATA_IND:
		printd(("%s: %p: N_DATA_IND <-\n", MOD_NAME, sl));
		rtn = n_data_ind(q, sl, mp);
		break;
	case N_EXDATA_IND:
		printd(("%s: %p: N_EXDATA_IND <-\n", MOD_NAME, sl));
		rtn = n_exdata_ind(q, sl, mp);
		break;
	case N_DATACK_IND:
		printd(("%s: %p: N_DATACK_IND <-\n", MOD_NAME, sl));
		rtn = n_datack_ind(q, sl, mp);
		break;
	case N_DISCON_IND:
		printd(("%s: %p: N_DISCON_IND <-\n", MOD_NAME, sl));
		rtn = n_discon_ind(q, sl, mp);
		break;
	case N_RESET_IND:
		printd(("%s: %p: N_RESET_IND <-\n", MOD_NAME, sl));
		rtn = n_reset_ind(q, sl, mp);
		break;
	case N_CONN_IND:
		printd(("%s: %p: N_CONN_IND <-\n", MOD_NAME, sl));
		rtn = n_other_ind(q, sl, mp);
		break;
	case N_CONN_CON:
		printd(("%s: %p: N_CONN_CON <-\n", MOD_NAME, sl));
		rtn = n_other_ind(q, sl, mp);
		break;
	case N_INFO_ACK:
		printd(("%s: %p: N_INFO_ACK <-\n", MOD_NAME, sl));
		rtn = n_other_ind(q, sl, mp);
		break;
	case N_BIND_ACK:
		printd(("%s: %p: N_BIND_ACK <-\n", MOD_NAME, sl));
		rtn = n_other_ind(q, sl, mp);
		break;
	case N_OK_ACK:
		printd(("%s: %p: N_OK_ACK <-\n", MOD_NAME, sl));
		rtn = n_other_ind(q, sl, mp);
		break;
	case N_ERROR_ACK:
		printd(("%s: %p: N_ERROR_ACK <-\n", MOD_NAME, sl));
		rtn = n_other_ind(q, sl, mp);
		break;
	case N_RESET_CON:
		printd(("%s: %p: N_RESET_CON <-\n", MOD_NAME, sl));
		rtn = n_other_ind(q, sl, mp);
		break;
	default:
		printd(("%s: %p: (????) <-\n", MOD_NAME, sl));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
	}
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
sl_w_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	printd(("%s: %p: -> M_DATA [%d]\n", MOD_NAME, sl, msgdsize(mp)));
	if ((err = sl_lsc_pdu(q, sl, mp)) < 0)
		return (err);
	return (QR_ABSORBED);	/* data absorbed */
}
STATIC int
sl_r_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	printd(("%s: %p: M_DATA [%d] <-\n", MOD_NAME, sl, msgdsize(mp)));
	if ((err = sl_recv_msg(q, sl, mp)) == QR_ABSORBED)
		return (QR_ABSORBED);	/* data absorbed */
	return (err);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
sl_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sl_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
sl_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sl_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sl_w_ioctl(q, mp);
	}
	return (QR_PASSALONG);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
kmem_cache_t *sl_priv_cachep = NULL;
STATIC int
sl_init_caches(void)
{
	if (!sl_priv_cachep
	    && !(sl_priv_cachep =
		 kmem_cache_create("sl_priv_cachep", sizeof(struct sl), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate sl_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized module private structure cache\n", MOD_NAME));
	return (0);
}
STATIC int
sl_term_caches(void)
{
	if (sl_priv_cachep) {
		if (kmem_cache_destroy(sl_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sl_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed sl_priv_cachep\n", MOD_NAME));
	}
	return (0);
}
STATIC struct sl *
sl_alloc_priv(queue_t *q, struct sl **slp, dev_t *devp, cred_t *crp)
{
	struct sl *sl;
	if ((sl = kmem_cache_alloc(sl_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated module private structure\n", MOD_NAME, sl));
		bzero(sl, sizeof(*sl));
		sl_get(sl);	/* first get */
		sl->u.dev.cmajor = getmajor(*devp);
		sl->u.dev.cminor = getminor(*devp);
		sl->cred = *crp;
		sl->o_prim = &sl_r_prim;
		sl->i_prim = &sl_w_prim;
		sl->o_wakeup = &sl_rx_wakeup;
		sl->i_wakeup = &sl_tx_wakeup;
		sl->priv_put = &sl_put;
		sl->priv_get = &sl_get;
		(sl->oq = RD(q))->q_ptr = sl_get(sl);
		(sl->iq = WR(q))->q_ptr = sl_get(sl);
		spin_lock_init(&sl->qlock);	/* m2pa-queue-lock */
		sl->i_version = M2PA_VERSION_DEFAULT;
		sl->i_state = LMI_DISABLED;
		sl->i_style = 0;
		spin_lock_init(&sl->lock);	/* m2pa-priv-lock */
		if ((sl->next = *slp))
			sl->next->prev = &sl->next;
		sl->prev = slp;
		*slp = sl_get(sl);
		printd(("%s: %p: linked module private structure\n", MOD_NAME, sl));
		/* 
		   LMI configuraiton defaults */
		sl->option.pvar = SS7_PVAR_ITUT_88;
		sl->option.popt = 0;
		/* 
		   SDL configuration defaults */
		sl->sdl.config.ifflags = 0;
		sl->sdl.config.iftype = SDL_TYPE_PACKET;
		sl->sdl.config.ifrate = 10000000;
		sl->sdl.config.ifgtype = SDL_GTYPE_SCTP;
		sl->sdl.config.ifgrate = 10000000;
		sl->sdl.config.ifmode = SDL_MODE_PEER;
		sl->sdl.config.ifgmode = SDL_GMODE_NONE;
		sl->sdl.config.ifgcrc = SDL_GCRC_NONE;
		sl->sdl.config.ifclock = SDL_CLOCK_NONE;
		sl->sdl.config.ifcoding = SDL_CODING_NONE;
		sl->sdl.config.ifframing = SDL_FRAMING_NONE;
		/* 
		   rest zero */
		/* 
		   SDT configuration defaults */
		sl->sdt.config.Tin = 4;
		sl->sdt.config.Tie = 1;
		sl->sdt.config.T = 64;
		sl->sdt.config.D = 256;
		sl->sdt.config.t8 = 100 * HZ / 1000;
		sl->sdt.config.Te = 577169;
		sl->sdt.config.De = 9308000;
		sl->sdt.config.Ue = 144292000;
		sl->sdt.config.N = 16;
		sl->sdt.config.m = 272;
		sl->sdt.config.b = 8;
		sl->sdt.config.f = SDT_FLAGS_ONE;
		/* 
		   SL configuration defaults */
		bufq_init(&sl->rb);
		bufq_init(&sl->tb);
		bufq_init(&sl->rtb);
		sl->sl.config.t1 = 45 * HZ;
		sl->sl.config.t2 = 5 * HZ;
		sl->sl.config.t2l = 20 * HZ;
		sl->sl.config.t2h = 100 & HZ;
		sl->sl.config.t3 = 1 * HZ;
		sl->sl.config.t4n = 8 * HZ;
		sl->sl.config.t4e = 500 * HZ / 1000;
		sl->sl.config.t5 = 100 * HZ / 1000;
		sl->sl.config.t6 = 4 * HZ;
		sl->sl.config.t7 = 1 * HZ;
		sl->sl.config.rb_abate = 3;
		sl->sl.config.rb_accept = 6;
		sl->sl.config.rb_discard = 9;
		sl->sl.config.tb_abate_1 = 128 * 272;
		sl->sl.config.tb_onset_1 = 256 * 272;
		sl->sl.config.tb_discd_1 = 284 * 272;
		sl->sl.config.tb_abate_2 = 512 * 272;
		sl->sl.config.tb_onset_2 = 640 * 272;
		sl->sl.config.tb_discd_2 = 768 * 272;
		sl->sl.config.tb_abate_3 = 896 * 272;
		sl->sl.config.tb_onset_3 = 1024 * 272;
		sl->sl.config.tb_discd_3 = 1152 * 272;
		sl->sl.config.N1 = 127;
		sl->sl.config.N2 = 8192;
		sl->sl.config.M = 5;
		printd(("%s: %p: setting module private structure defaults\n", MOD_NAME, sl));
	} else
		ptrace(("%s: %p: ERROR: Could not allocate module private structure\n",
			MOD_NAME, sl));
	return (sl);
}
STATIC void
sl_free_priv(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	psw_t flags;
	ensure(sl, return);
	spin_lock_irqsave(&sl->lock, flags);
	{
		ss7_unbufcall((str_t *) sl);
		sl_timer_stop(sl, tall);
		bufq_purge(&sl->rb);
		bufq_purge(&sl->tb);
		bufq_purge(&sl->rtb);
		if (sl->rbuf)
			freemsg(xchg(&sl->rbuf, NULL));
		if ((*sl->prev = sl->next))
			sl->next->prev = sl->prev;
		sl->next = NULL;
		sl->prev = &sl->next;
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(sl);
		flushq(sl->oq, FLUSHALL);
		flushq(sl->iq, FLUSHALL);
		sl->oq->q_ptr = NULL;
		sl->oq = NULL;
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(sl);
		sl->iq->q_ptr = NULL;
		sl->iq = NULL;
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(sl);
		/* 
		   done, check final count */
		if (atomic_read(&sl->refcnt) != 1) {
			printd(("%s: %s: %p: ERROR: sl lingering refereice count = %d\n",
				MOD_NAME, __FUNCTION__, sl, atomic_read(&sl->refcnt)));
			atomic_set(&sl->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	sl_put(sl);		/* final put */
	return;
}
STATIC struct sl *
sl_get(struct sl *sl)
{
	assure(sl);
	if (sl)
		atomic_inc(&sl->refcnt);
	return (sl);
}
STATIC void
sl_put(struct sl *sl)
{
	assure(sl);
	if (sl && atomic_dec_and_test(&sl->refcnt)) {
		kmem_cache_free(sl_priv_cachep, sl);
		printd(("%s: %p: freed module private structure\n", MOD_NAME, sl));
	}
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC struct sl *sl_list = NULL;
STATIC int
sl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;		/* for now */
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		major_t cmajor = getmajor(*devp);
		minor_t cminor = getminor(*devp);
		struct sl *sl;
		/* test for multiple push */
		for (sl = sl_list; sl; sl = sl->next) {
			if (sl->u.dev.cmajor == cmajor && sl->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(sl_alloc_priv(q, &sl_list, devp, crp))) {
			MOD_DEC_USE_COUNT;
			return ENOMEM;
		}
		qprocson(q);
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return EIO;
}
STATIC int
sl_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sl_free_priv(q);
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
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module ID for the M2PA-SL module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &m2pa_slinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
sl_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&sl_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
sl_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&sl_fmod)) < 0)
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
sl_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&m2pa_slinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
sl_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&m2pa_slinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
m2pa_slinit(void)
{
	int err;
#ifdef MODULE
	cmn_err(CE_NOTE, M2PA_SL_BANNER);	/* banner message */
#else
	cmn_err(CE_NOTE, M2PA_SL_SPLASH);	/* console splash */
#endif
	if ((err = sl_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = sl_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		sl_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
m2pa_slterminate(void)
{
	int err;
	if ((err = sl_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sl_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(m2pa_slinit);
module_exit(m2pa_slterminate);

#endif				/* LINUX */
