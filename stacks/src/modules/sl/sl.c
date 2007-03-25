/*****************************************************************************

 @(#) $RCSfile: sl.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2007/03/25 02:22:58 $

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

 Last Modified $Date: 2007/03/25 02:22:58 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sl.c,v $
 Revision 0.9.2.19  2007/03/25 02:22:58  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.18  2007/03/25 00:52:07  brian
 - synchronization updates

 Revision 0.9.2.17  2006/12/27 16:35:55  brian
 - added slpmod module and fixups for make check target

 Revision 0.9.2.16  2006/05/08 11:01:10  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.15  2006/04/24 05:01:02  brian
 - call interface corrections

 Revision 0.9.2.14  2006/03/07 01:11:46  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: sl.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2007/03/25 02:22:58 $"

static char const ident[] =
    "$RCSfile: sl.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2007/03/25 02:22:58 $";

/*
 *  This is an SL (Signalling Link) module which can be pushed over an SDT
 *  (Signalling Data Terminal) module to effect an OpenSS7 Signalling Link.
 *  Having the SL state machines separate permits live upgrade and allows this
 *  state machine to be rigorously conformance tested only once.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#define SL_DESCRIP	"SS7/IP SIGNALLING LINK (SL) STREAMS MODULE."
#define SL_REVISION	"OpenSS7 $RCSfile: sl.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2007/03/25 02:22:58 $"
#define SL_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SL_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SL_LICENSE	"GPL"
#define SL_BANNER	SL_DESCRIP	"\n" \
			SL_REVISION	"\n" \
			SL_COPYRIGHT	"\n" \
			SL_DEVICE	"\n" \
			SL_CONTACT	"\n"
#define SL_SPLASH	SL_DESCRIP	"\n" \
			SL_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SL_CONTACT);
MODULE_DESCRIPTION(SL_DESCRIP);
MODULE_SUPPORTED_DEVICE(SL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SL_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sl");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SL_MOD_ID	CONFIG_STREAMS_SL_MODID
#define SL_MOD_NAME	CONFIG_STREAMS_SL_NAME
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		SL_MOD_ID
#define MOD_NAME	SL_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SL_BANNER
#else				/* MODULE */
#define MOD_BANNER	SL_SPLASH
#endif				/* MODULE */

STATIC struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC int streamscall sl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int streamscall sl_close(queue_t *, int, cred_t *);

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

STATIC struct streamtab slinfo = {
	.st_rdinit = &sl_rinit,		/* Upper read queue */
	.st_wrinit = &sl_winit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  SL Private Structure
 *
 *  =========================================================================
 */

typedef struct sl {
	STR_DECLARATION (struct sl);	/* stream declaration */
	bufq_t rb;			/* receive buffer */
	bufq_t tb;			/* transmission buffer */
	bufq_t rtb;			/* retransmission buffer */
	lmi_option_t option;		/* LMI options */
	sl_timers_t timers;		/* SL timers */
	sl_config_t config;		/* SL configuration */
	sl_statem_t statem;		/* SL state machine */
	sl_notify_t notify;		/* SL notification options */
	sl_stats_t stats;		/* SL statistics */
	sl_stats_t stamp;		/* SL statistics timestamps */
	lmi_sta_t statsp;		/* SL statistics periods */
} sl_t;

#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

struct sl *sl_opens = NULL;

STATIC struct sl *sl_alloc_priv(queue_t *, struct sl **, dev_t *, cred_t *);
STATIC struct sl *sl_get(struct sl *);
STATIC void sl_put(struct sl *);
STATIC void sl_free_priv(queue_t *);

struct lmi_option lmi_default = {
	pvar:SS7_PVAR_ITUT_96,
	popt:0,
};
struct sl_config sl_default = {
	t1:45 * HZ,
	t2:5 * HZ,
	t2l:20 * HZ,
	t2h:100 * HZ,
	t3:1 * HZ,
	t4n:8 * HZ,
	t4e:500 * HZ / 1000,
	t5:100 * HZ / 1000,
	t6:4 * HZ,
	t7:1 * HZ,
	rb_abate:3,
	rb_accept:6,
	rb_discard:9,
	tb_abate_1:128 * 272,
	tb_onset_1:256 * 272,
	tb_discd_1:384 * 272,
	tb_abate_2:512 * 272,
	tb_onset_2:640 * 272,
	tb_discd_2:768 * 272,
	tb_abate_3:896 * 272,
	tb_onset_3:1024 * 272,
	tb_discd_3:1152 * 272,
	N1:127,
	N2:8192,
	M:5,
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
 *  Primitive sent upstream
 *
 *  ------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC INLINE int
m_error(queue_t *q, struct sl *sl, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sl->i_state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_ERROR\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  M_HANGUP
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
STATIC INLINE int
m_hangup(queue_t *q, struct sl *sl, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sl->i_state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, struct sl *sl, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sl->i_state;
		p->lmi_max_sdu = 272 + 1 + ((sl->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((sl->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
STATIC INLINE int
lmi_ok_ack(queue_t *q, struct sl *sl, long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl->i_state) {
		case LMI_ATTACH_PENDING:
			sl->i_state = LMI_DISABLED;
			break;
		case LMI_DETACH_PENDING:
			sl->i_state = LMI_UNATTACHED;
			break;
		default:
			break;
		}
		p->lmi_state = sl->i_state;
		printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
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
lmi_error_ack(queue_t *q, struct sl *sl, long prim, ulong reason, ulong errno)
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
		default:
			break;
		}
		p->lmi_state = sl->i_state;
		printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
STATIC INLINE int
lmi_enable_con(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ENABLE_CON;
		assure(sl->i_state == LMI_ENABLE_PENDING);
		sl->i_state = LMI_ENABLED;
		p->lmi_state = sl->i_state;
		printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
STATIC INLINE int
lmi_disable_con(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DISABLE_CON;
		assure(sl->i_state == LMI_DISABLE_PENDING);
		sl->i_state = LMI_DISABLED;
		p->lmi_state = sl->i_state;
		printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, struct sl *sl, ulong flags, caddr_t opt_ptr, size_t opt_len)
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
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
STATIC INLINE int
lmi_error_ind(queue_t *q, struct sl *sl, ulong errno, ulong reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = sl->i_state;
		printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  LMI_STATS_IND
 *  -----------------------------------
 *  TODO: Statistics indications need to be provided in the implementation.
 */
STATIC INLINE int
lmi_stats_ind(queue_t *q, struct sl *sl, ulong interval, mblk_t *dp)
{
	mblk_t *mp;
	lmi_stats_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = interval;
		p->lmi_timestamp = jiffies;
		mp->b_cont = dp;
		printd(("%s: %p: <- LMI_STATS_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 *  TODO: Event indications need to be provided in the implementation.
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, struct sl *sl, ulong oid, ulong level)
{
	mblk_t *mp;
	lmi_event_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_EVENT_IND;
		p->lmi_objectid = oid;
		p->lmi_timestamp = jiffies;
		p->lmi_severity = level;
		printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SL_PDU_IND
 *  -----------------------------------
 *  We don't actually use SL_PDU_INDs, we pass along M_DATA messages.
 */
STATIC INLINE int
sl_pdu_ind(queue_t *q, struct sl *sl, mblk_t *dp)
{
	mblk_t *mp;
	sl_pdu_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_PDU_IND;
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_PDU_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_link_congested_ind(queue_t *q, struct sl *sl, ulong cong, ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTED_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_link_congestion_ceased_ind(queue_t *q, struct sl *sl, ulong cong, ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ceased_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		p->sl_timestamp = jiffies;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTION_CEASED_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  -----------------------------------
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
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  -----------------------------------
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
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RB_CLEARED_IND
 *  -----------------------------------
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
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
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
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
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
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_out_of_service_ind(queue_t *q, struct sl *sl, ulong reason)
{
	mblk_t *mp;
	sl_out_of_service_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = jiffies;
		p->sl_reason = reason;
		printd(("%s: %p: <- SL_OUT_OF_SERVICE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_remote_processor_outage_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_rem_proc_out_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = jiffies;
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_OUTAGE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_remote_processor_recovered_ind(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_rem_proc_recovered_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = jiffies;
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_RECOVERED_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_RTB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
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
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 *  This primitive is not necessary in the current implementation.  Retrieval
 *  is always possible.
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
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  -----------------------------------
 *  This primitive is not necessary in the current implementation.  Retrieval
 *  of BSNT is always possible.
 */
STATIC INLINE int
sl_bsnt_not_retrievable_ind(queue_t *q, struct sl *sl, ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_not_retr_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		p->sl_bsnt = bsnt;
		printd(("%s: %p: <- SL_BSNT_NOT_RETRIEVABLE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
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
 *  Option management from the local management interface is not yet
 *  implemented.
 */
STATIC INLINE int
sl_optmgmt_ack(queue_t *q, struct sl *sl, caddr_t opt_ptr, size_t opt_len, ulong flags)
{
	mblk_t *mp;
	sl_optmgmt_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_OPTMGMT_ACK;
		p->opt_length = opt_len;
		p->opt_offset = opt_len ? sizeof(*p) : 0;
		p->mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- SL_OPTMGMT_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SL_NOTIFY_IND
 *  -----------------------------------
 *  TODO: Notification indications need to be included in the current
 *  implementation.  Notifications should be provided using LMI_EVENT_IND and
 *  not SL_NOTIFY_IND ???
 */
STATIC INLINE int
sl_notify_ind(queue_t *q, struct sl *sl, ulong oid, ulong level)
{
	mblk_t *mp;
	sl_notify_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_NOTIFY_IND;
		p->sl_objectid = oid;
		p->sl_timestamp = jiffies;
		p->sl_severity = level;
		printd(("%s: %p: <- SL_NOTIFY_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  ------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  ------------------------------------------------------------------------
 */

/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 *  Generates the initial information request to synchronize the SL with the
 *  underlying SDT layer.  This must be successful or the stream will be
 *  rendered unusable.
 */
STATIC INLINE int
sdt_info_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_info_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  SDT_DAEDT_TRANSMISSION_REQ
 *  -----------------------------------
 *  We don't use transmission requests, we pass just M_DATA blocks instead.
 */
STATIC INLINE int
sdt_daedt_transmission_req(queue_t *q, struct sl *sl, mblk_t *dp)
{
	mblk_t *mp;
	sdt_daedt_transmission_req_t *p;
	ensure(dp, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_DAEDT_TRANSMISSION_REQ;
		mp->b_cont = dp;
		printd(("%s: %p: SDT_DAEDT_TRANSMISSION_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDT_DAEDT_START_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdt_daedt_start_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sdt_daedt_start_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_DAEDT_START_REQ;
		printd(("%s: %p: SDT_DAEDT_START_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_DAEDR_START_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdt_daedr_start_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sdt_daedr_start_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_DAEDR_START_REQ;
		printd(("%s: %p: SDT_DAEDR_START_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_AERM_START_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdt_aerm_start_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sdt_aerm_start_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_AERM_START_REQ;
		printd(("%s: %p: SDT_AERM_START_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_AERM_STOP_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdt_aerm_stop_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sdt_aerm_stop_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_AERM_STOP_REQ;
		printd(("%s: %p: SDT_AERM_STOP_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdt_aerm_set_ti_to_tin_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sdt_aerm_set_ti_to_tin_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_AERM_SET_TI_TO_TIN_REQ;
		printd(("%s: %p: SDT_AERM_SET_TI_TO_TIN_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdt_aerm_set_ti_to_tie_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sdt_aerm_set_ti_to_tie_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_AERM_SET_TI_TO_TIE_REQ;
		printd(("%s: %p: SDT_AERM_SET_TI_TO_TIE_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_SUERM_START_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdt_suerm_start_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sdt_suerm_start_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_SUERM_START_REQ;
		printd(("%s: %p: SDT_SUERM_START_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_SUERM_STOP_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdt_suerm_stop_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sdt_suerm_stop_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_SUERM_STOP_REQ;
		printd(("%s: %p: SDT_SUERM_STOP_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
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
enum { tall, t1, t2, t3, t4, t5, t6, t7 };

SS7_DECLARE_TIMER(MOD_NAME, sl, t1, config);
SS7_DECLARE_TIMER(MOD_NAME, sl, t2, config);
SS7_DECLARE_TIMER(MOD_NAME, sl, t3, config);
STATIC int sl_t4_timeout(struct sl *);
STATIC void streamscall
sl_t4_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t4", "sl", &((struct sl *) data)->timers.t4,
		       (int (*)(struct head *)) &sl_t4_timeout, &sl_t4_expiry);
}
STATIC void
sl_stop_timer_t4(struct sl *sl)
{
	ss7_stop_timer((struct head *) sl, "t4", "sl", &sl->timers.t4);
}
STATIC void
sl_start_timer_t4(struct sl *sl)
{
	ss7_start_timer((struct head *) sl, "t4", "sl", &sl->timers.t4, &sl_t4_expiry,
			sl->statem.t4v);
};

SS7_DECLARE_TIMER(MOD_NAME, sl, t5, config);
SS7_DECLARE_TIMER(MOD_NAME, sl, t6, config);
SS7_DECLARE_TIMER(MOD_NAME, sl, t7, config);

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
	case t5:
		sl_stop_timer_t5(sl);
		if (single)
			break;
		/* 
		   fall through */
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
		case t5:
			sl_start_timer_t5(sl);
			break;
		case t6:
			sl_start_timer_t6(sl);
			break;
		case t7:
			sl_start_timer_t7(sl);
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
 *  Duration Statistics
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC void
sl_is_stats(queue_t *q, struct sl *sl)
{
	if (sl->stamp.sl_dur_unavail)
		sl->stats.sl_dur_unavail += jiffies - xchg(&sl->stamp.sl_dur_unavail, 0);
	if (sl->stamp.sl_dur_unavail_rpo)
		sl->stats.sl_dur_unavail_rpo += jiffies - xchg(&sl->stamp.sl_dur_unavail_rpo, 0);
	if (sl->stamp.sl_dur_unavail_failed)
		sl->stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->stamp.sl_dur_unavail_failed, 0);
	sl->stamp.sl_dur_in_service = jiffies;
}
STATIC void
sl_oos_stats(queue_t *q, struct sl *sl)
{
	if (sl->stamp.sl_dur_in_service)
		sl->stats.sl_dur_in_service += jiffies - xchg(&sl->stamp.sl_dur_in_service, 0);
	if (sl->stamp.sl_dur_unavail_rpo)
		sl->stats.sl_dur_unavail_rpo += jiffies - xchg(&sl->stamp.sl_dur_unavail_rpo, 0);
	if (sl->stamp.sl_dur_unavail_failed)
		sl->stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->stamp.sl_dur_unavail_failed, 0);
	sl->stamp.sl_dur_unavail = jiffies;
}
STATIC void
sl_rpo_stats(queue_t *q, struct sl *sl)
{
	if (sl->stamp.sl_dur_unavail_rpo)
		sl->stats.sl_dur_unavail_rpo += jiffies - xchg(&sl->stamp.sl_dur_unavail_rpo, 0);
}
STATIC void
sl_rpr_stats(queue_t *q, struct sl *sl)
{
	if (sl->stamp.sl_dur_unavail_rpo)
		sl->stats.sl_dur_unavail_rpo += jiffies - xchg(&sl->stamp.sl_dur_unavail_rpo, 0);
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

#define sl_cc_stop sl_cc_normal
STATIC INLINE void
sl_cc_normal(queue_t *q, struct sl *sl)
{
	sl_timer_stop(sl, t5);
	sl->statem.cc_state = SL_STATE_IDLE;
}

STATIC INLINE void
sl_rc_stop(queue_t *q, struct sl *sl)
{
	sl_cc_normal(q, sl);
	sl->statem.rc_state = SL_STATE_IDLE;
}

STATIC INLINE int
sl_aerm_stop(queue_t *q, struct sl *sl)
{
	return sdt_aerm_stop_req(q, sl);
}

STATIC INLINE int
sl_iac_stop(queue_t *q, struct sl *sl)
{
	int err;
	if (sl->statem.iac_state != SL_STATE_IDLE) {
		sl_timer_stop(sl, t3);
		sl_timer_stop(sl, t2);
		sl_timer_stop(sl, t4);
		if ((err = sl_aerm_stop(q, sl)))
			return (err);
		sl->statem.emergency = 0;
		sl->statem.iac_state = SL_STATE_IDLE;
	}
	return (QR_DONE);
}

STATIC streamscall void sl_tx_wakeup(queue_t *q);

STATIC INLINE void
sl_daedt_transmitter_wakeup(queue_t *q, struct sl *sl)
{
	if (sl->statem.txc_state == SL_STATE_SLEEPING)
		sl->statem.txc_state = SL_STATE_IN_SERVICE;
	sl_tx_wakeup(q);
}

STATIC INLINE void
sl_txc_send_sios(queue_t *q, struct sl *sl)
{
	sl_timer_stop(sl, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92)
		sl_timer_stop(sl, t6);
	sl->statem.lssu_available = 1;
	sl->statem.tx.sio = LSSU_SIOS;
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE void
sl_poc_stop(queue_t *q, struct sl *sl)
{
	sl->statem.poc_state = SL_STATE_IDLE;
}

STATIC INLINE int
sl_suerm_stop(queue_t *q, struct sl *sl)
{
	return sdt_suerm_stop_req(q, sl);
}

STATIC INLINE int
sl_lsc_link_failure(queue_t *q, struct sl *sl, ulong reason)
{
	int err;
	if (sl->statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
		if ((err = sl_out_of_service_ind(q, sl, reason)))
			return (err);
		sl->statem.failure_reason = reason;
		if ((err = sl_iac_stop(q, sl)))	/* ok if not aligning */
			return (err);
		sl_timer_stop(sl, t1);	/* ok if not running */
		if ((err = sl_suerm_stop(q, sl)))	/* ok if not running */
			return (err);
		sl_rc_stop(q, sl);
		sl_txc_send_sios(q, sl);
		sl_poc_stop(q, sl);	/* ok if not ITUT */
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if not ANSI */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_txc_send_sib(queue_t *q, struct sl *sl)
{
	sl->statem.tx.sio = LSSU_SIB;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE void
sl_txc_send_sipo(queue_t *q, struct sl *sl)
{
	sl_timer_stop(sl, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92)
		sl_timer_stop(sl, t6);
	sl->statem.tx.sio = LSSU_SIPO;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE void
sl_txc_send_sio(queue_t *q, struct sl *sl)
{
	sl->statem.tx.sio = LSSU_SIO;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE void
sl_txc_send_sin(queue_t *q, struct sl *sl)
{
	sl->statem.tx.sio = LSSU_SIN;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE void
sl_txc_send_sie(queue_t *q, struct sl *sl)
{
	sl->statem.tx.sio = LSSU_SIE;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE void
sl_txc_send_msu(queue_t *q, struct sl *sl)
{
	if (sl->rtb.q_count)
		sl_timer_start(sl, t7);
	sl->statem.msu_inhibited = 0;
	sl->statem.lssu_available = 0;
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE void
sl_txc_send_fisu(queue_t *q, struct sl *sl)
{
	sl_timer_stop(sl, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92 && !(sl->option.popt & SS7_POPT_PCR))
		sl_timer_stop(sl, t6);
	sl->statem.msu_inhibited = 1;
	sl->statem.lssu_available = 0;
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE void
sl_txc_fsnx_value(queue_t *q, struct sl *sl)
{
	if (sl->statem.tx.X.fsn != sl->statem.rx.X.fsn) {
		sl->statem.tx.X.fsn = sl->statem.rx.X.fsn;
		sl_daedt_transmitter_wakeup(q, sl);
	}
}

STATIC INLINE void
sl_txc_nack_to_be_sent(queue_t *q, struct sl *sl)
{
	int pcr = sl->option.popt & SS7_POPT_PCR;
	(void) pcr;
	assure(!pcr);
	sl->statem.tx.N.bib = sl->statem.tx.N.bib ? 0 : sl->statem.ib_mask;
	ptrace(("%s: %p: [%x/%x] %d\n", MOD_NAME, sl, sl->statem.tx.N.bib | sl->statem.tx.N.bsn,
		sl->statem.tx.N.fib | sl->statem.tx.N.fsn, pcr));
	sl_daedt_transmitter_wakeup(q, sl);
}

STATIC INLINE int
sl_lsc_rtb_cleared(queue_t *q, struct sl *sl)
{
	int err;
	if (sl->statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->statem.remote_processor_outage = 0;
		if (sl->statem.local_processor_outage)
			return (QR_DONE);
		if ((err = sl_remote_processor_recovered_ind(q, sl)))
			return (err);
		if ((err = sl_rtb_cleared_ind(q, sl)))
			return (err);
		sl_txc_send_msu(q, sl);
		sl->statem.lsc_state = SL_STATE_IN_SERVICE;
	}
	return (QR_DONE);
}

STATIC int sl_check_congestion(queue_t *q, struct sl *sl);

STATIC INLINE int
sl_txc_bsnr_and_bibr(queue_t *q, struct sl *sl)
{
	int err;
	int pcr = sl->option.popt & SS7_POPT_PCR;
	sl->statem.tx.R.bsn = sl->statem.rx.R.bsn;
	sl->statem.tx.R.bib = sl->statem.rx.R.bib;
	printd(("%s: %p: [%x/%x] %d\n", MOD_NAME, sl, sl->statem.tx.N.bib | sl->statem.tx.N.bsn,
		sl->statem.tx.N.fib | sl->statem.tx.N.fsn, pcr));
	if (sl->statem.clear_rtb) {
		bufq_purge(&sl->rtb);
		sl->statem.Ct = 0;
		if ((err = sl_check_congestion(q, sl)))
			return (err);
		sl->statem.tx.F.fsn = (sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask;
		sl->statem.tx.L.fsn = sl->statem.tx.R.bsn;
		sl->statem.tx.N.fsn = sl->statem.tx.R.bsn;
		trace();
		sl->statem.tx.N.fib = sl->statem.tx.R.bib;	/* for PCR too? */
		sl->statem.Z = (sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask;
		sl->statem.z_ptr = NULL;
		/* 
		   FIXME: handle error return */
		if ((err = sl_lsc_rtb_cleared(q, sl)))
			return (err);
		sl->statem.clear_rtb = 0;
		sl->statem.rtb_full = 0;
		return (QR_DONE);
	}
	printd(("%s: %p: F.fsn = %x, R.bsn = %x, mask = %x\n", MOD_NAME, sl, sl->statem.tx.F.fsn,
		sl->statem.tx.R.bsn, sl->statem.sn_mask));
	if (sl->statem.tx.F.fsn != ((sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask)) {
		if (sl->statem.sib_received) {
			sl->statem.sib_received = 0;
			sl_timer_stop(sl, t6);
		}
		do {
			freemsg(bufq_dequeue(&sl->rtb));
			sl->statem.Ct--;
			sl->statem.tx.F.fsn = (sl->statem.tx.F.fsn + 1) & sl->statem.sn_mask;
		} while (sl->statem.tx.F.fsn != ((sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask));
		if ((err = sl_check_congestion(q, sl)))
			return (err);
		sl_daedt_transmitter_wakeup(q, sl);
		if (sl->rtb.q_count == 0) {
			sl_timer_stop(sl, t7);
		} else {
			sl_timer_start(sl, t7);
		}
		if (!pcr || (sl->rtb.q_msgs < sl->config.N1 && sl->rtb.q_count < sl->config.N2)) {
			sl->statem.rtb_full = 0;
		}
		if (SN_OUTSIDE(sl->statem.tx.F.fsn, sl->statem.Z, sl->statem.tx.L.fsn)
		    || !sl->rtb.q_count) {
			sl->statem.Z = sl->statem.tx.F.fsn;
			sl->statem.z_ptr = sl->rtb.q_head;
		}
	}
	if (!pcr && sl->statem.tx.N.fib != sl->statem.tx.R.bib) {
		if (sl->statem.sib_received) {
			sl->statem.sib_received = 0;
			sl_timer_stop(sl, t6);
		}
		sl->statem.tx.N.fib = sl->statem.tx.R.bib;
		sl->statem.tx.N.fsn = (sl->statem.tx.F.fsn - 1) & sl->statem.sn_mask;
		if ((sl->statem.z_ptr = sl->rtb.q_head) != NULL) {
			sl->statem.retrans_cycle = 1;
		}
		sl_daedt_transmitter_wakeup(q, sl);
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_txc_sib_received(queue_t *q, struct sl *sl)
{
	/* 
	   FIXME: consider these variations for all */
	if (sl->option.pvar == SS7_PVAR_ANSI_92 && sl->statem.lssu_available)
		if (sl->statem.tx.sio != LSSU_SIB)
			return;
	if (sl->option.pvar != SS7_PVAR_ITUT_93 && !sl->rtb.q_count)
		return;
	if (!sl->statem.sib_received) {
		sl_timer_start(sl, t6);
		sl->statem.sib_received = 1;
	}
	sl_timer_start(sl, t7);
}

STATIC INLINE int
sl_txc_clear_rtb(queue_t *q, struct sl *sl)
{
	bufq_purge(&sl->rtb);
	sl->statem.Ct = 0;
	sl->statem.clear_rtb = 1;
	sl->statem.rtb_full = 0;	/* added */
	/* 
	 * FIXME: should probably follow more of the ITUT flush_buffers stuff
	 * like reseting Z and FSNF, FSNL, FSNT.
	 */
	return sl_check_congestion(q, sl);
}

STATIC INLINE int
sl_txc_clear_tb(queue_t *q, struct sl *sl)
{
	bufq_purge(&sl->tb);
	flushq(sl->iq, FLUSHDATA);
	sl->statem.Cm = 0;
	return sl_check_congestion(q, sl);
}

STATIC INLINE void
sl_txc_flush_buffers(queue_t *q, struct sl *sl)
{
	bufq_purge(&sl->rtb);
	sl->statem.rtb_full = 0;
	sl->statem.Ct = 0;
	bufq_purge(&sl->tb);
	flushq(sl->iq, FLUSHDATA);
	sl->statem.Cm = 0;
	sl->statem.Z = 0;
	sl->statem.z_ptr = NULL;
	/* 
	   Z =0 error in ITUT 93 and ANSI */
	sl->statem.Z = sl->statem.tx.F.fsn = (sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask;
	sl->statem.tx.L.fsn = sl->statem.rx.R.bsn;
	sl->statem.rx.T.fsn = sl->statem.rx.R.bsn;
	sl_timer_stop(sl, t7);
}

STATIC INLINE void
sl_rc_fsnt_value(queue_t *q, struct sl *sl)
{
	sl->statem.rx.T.fsn = sl->statem.tx.N.fsn;
}

STATIC INLINE int
sl_txc_retrieval_request_and_fsnc(queue_t *q, struct sl *sl, sl_ulong fsnc)
{
	mblk_t *mp;
	int err;
	sl->statem.tx.C.fsn = fsnc & (sl->statem.sn_mask);
	/* 
	 *  FIXME: Q.704/5.7.2 states:
	 *
	 *  5.7.2   If a changeover order or acknowledgement containing an
	 *  unreasonable value of the forward sequence number is received, no
	 *  buffer updating or retrieval is performed, and new traffic is
	 *  started on the alternative signalling link(s).
	 *
	 *  It will be necessary to check FSNC for "reasonableness" here and
	 *  flush RTB and TB and return retrieval-complete indication with a
	 *  return code of "unreasonable FSNC".
	 *
	 *  (Tell the SIGTRAN working group and M2UA guys about this!)
	 */
	while (sl->rtb.q_count && sl->statem.tx.F.fsn != ((fsnc + 1) & sl->statem.sn_mask)) {
		freemsg(bufq_dequeue(&sl->rtb));
		sl->statem.Ct--;
		sl->statem.tx.F.fsn = (sl->statem.tx.F.fsn + 1) & sl->statem.sn_mask;
	}
	while ((mp = bufq_dequeue(&sl->tb))) {
		sl->statem.Cm--;
		bufq_queue(&sl->rtb, mp);
		sl->statem.Ct++;
		if (!sl->statem.Cm)
			qenable(sl->iq);	/* back enable */
	}
	sl->statem.Z = sl->statem.tx.F.fsn = (sl->statem.tx.C.fsn + 1) & sl->statem.sn_mask;
	while ((mp = bufq_dequeue(&sl->rtb))) {
		sl->statem.Ct--;
		if ((err = sl_retrieved_message_ind(q, sl, mp)))
			return (err);
	}
	sl->statem.rtb_full = 0;
	if ((err = sl_retrieval_complete_ind(q, sl)))
		return (err);
	sl->statem.Cm = 0;
	sl->statem.Ct = 0;
	sl->statem.tx.N.fsn = sl->statem.tx.L.fsn = sl->statem.tx.C.fsn;
	return (QR_DONE);
}

STATIC INLINE void
sl_daedt_fisu(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (sl->option.popt & SS7_POPT_XSN) {
		*(sl_ushort *) mp->b_wptr = htons(sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = 0;
		mp->b_wptr += sizeof(sl_ushort);
	} else {
		*(sl_uchar *) mp->b_wptr = (sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = (sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = 0;
		mp->b_wptr += sizeof(sl_uchar);
	}
	sl->statem.txc_state = SL_STATE_SLEEPING;
	printd(("%s: %p: FISU [%x/%x] %ld ->\n", MOD_NAME, sl,
		sl->statem.tx.N.bib | sl->statem.tx.N.bsn,
		sl->statem.tx.N.fib | sl->statem.tx.N.fsn, sl->option.popt & SS7_POPT_PCR));
}

STATIC INLINE void
sl_daedt_lssu(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (sl->option.popt & SS7_POPT_XSN) {
		*(sl_ushort *) mp->b_wptr = htons(sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(1);
		mp->b_wptr += sizeof(sl_ushort);
	} else {
		*(sl_uchar *) mp->b_wptr = (sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = (sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = 1;
		mp->b_wptr += sizeof(sl_uchar);
	}
	*(sl_uchar *) mp->b_wptr = (sl->statem.tx.sio);
	mp->b_wptr += sizeof(sl_uchar);
	if (sl->statem.tx.sio != LSSU_SIB)
		sl->statem.txc_state = SL_STATE_SLEEPING;
	switch (sl->statem.tx.sio) {
	case LSSU_SIO:
		printd(("%s: %p: SIO ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIN:
		printd(("%s: %p: SIN ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIE:
		printd(("%s: %p: SIE ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIOS:
		printd(("%s: %p: SIOS ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIPO:
		printd(("%s: %p: SIPO ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIB:
		printd(("%s: %p: SIB ->\n", MOD_NAME, sl));
		break;
	}
}

STATIC INLINE void
sl_daedt_msu(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int len = msgdsize(mp);
	if (sl->option.popt & SS7_POPT_XSN) {
		((sl_ushort *) mp->b_rptr)[0] = htons(sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		((sl_ushort *) mp->b_rptr)[1] = htons(sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		((sl_ushort *) mp->b_rptr)[2] = htons(len - 6 < 512 ? len - 6 : 511);
	} else {
		((sl_uchar *) mp->b_rptr)[0] = (sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		((sl_uchar *) mp->b_rptr)[1] = (sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		((sl_uchar *) mp->b_rptr)[2] = (len - 3 < 64 ? len - 3 : 63);
	}
	printd(("%s: %p: MSU [%d] ->\n", MOD_NAME, sl, len));
}

STATIC INLINE mblk_t *
sl_txc_transmission_request(queue_t *q, struct sl *sl)
{
	mblk_t *mp = NULL;
	int pcr;
	if (sl->statem.txc_state != SL_STATE_IN_SERVICE)
		return (mp);
	pcr = sl->option.popt & SS7_POPT_PCR;
	if (sl->statem.lssu_available) {
		if ((mp = allocb(7, BPRI_HI))) {
			if (sl->statem.tx.sio == LSSU_SIB)
				sl->statem.lssu_available = 0;
			sl->statem.tx.N.fsn = sl->statem.tx.L.fsn;
			// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			sl->statem.tx.N.bsn = (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
			// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
			sl_daedt_lssu(q, sl, mp);
		}
		return (mp);
	}
	if (sl->statem.msu_inhibited) {
		if ((mp = allocb(6, BPRI_HI))) {
			sl->statem.tx.N.fsn = sl->statem.tx.L.fsn;
			// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			sl->statem.tx.N.bsn = (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
			// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
			sl_daedt_fisu(q, sl, mp);
		}
		return (mp);
	}
	if (pcr) {
		if ((sl->rtb.q_msgs < sl->config.N1)
		    && (sl->rtb.q_count < sl->config.N2)) {
			sl->statem.forced_retransmission = 0;
			sl->statem.rtb_full = 0;
		}
		if (sl->tb.q_count && sl->statem.rtb_full) {
			sl->statem.forced_retransmission = 1;
		}
	}
	if ((!pcr && sl->statem.retrans_cycle)
	    || (pcr && (sl->statem.forced_retransmission || (!sl->tb.q_count && sl->rtb.q_count)))) {
		mblk_t *bp = sl->statem.z_ptr;
		if (bp && !(mp = dupmsg(bp)))
			return (mp);
		if (!bp && pcr) {
			bp = sl->rtb.q_head;
			if (bp && !(mp = dupmsg(bp)))
				return (mp);
			sl->statem.z_ptr = bp;
			sl->statem.Z = sl->statem.tx.F.fsn;
		}
		if (mp) {
			sl->statem.z_ptr = bp->b_next;
			if (pcr) {
				sl->statem.tx.N.fsn = sl->statem.Z;
				// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
				sl->statem.tx.N.bsn =
				    (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
				// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
				sl->statem.Z = (sl->statem.Z + 1) & sl->statem.sn_mask;
			} else {
				sl->statem.tx.N.fsn =
				    (sl->statem.tx.N.fsn + 1) & sl->statem.sn_mask;
				// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
				sl->statem.tx.N.bsn =
				    (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
				// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			}
			sl_daedt_msu(q, sl, mp);
			if (sl->statem.tx.N.fsn == sl->statem.tx.L.fsn || sl->statem.z_ptr == NULL)
				sl->statem.retrans_cycle = 0;
			return (mp);
		}
	}
	if ((!pcr && (!sl->tb.q_count || sl->statem.rtb_full))
	    || (pcr && (!sl->tb.q_count && !sl->rtb.q_count))) {
		if ((mp = allocb(6, BPRI_HI))) {
			sl->statem.tx.N.fsn = sl->statem.tx.L.fsn;
			// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			sl->statem.tx.N.bsn = (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
			// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
			sl_daedt_fisu(q, sl, mp);
		}
		return (mp);
	} else {
		spin_lock(&sl->tb.q_lock);
		if ((mp = bufq_head(&sl->tb)) && (mp = dupmsg(mp))) {
			mblk_t *bp = bufq_dequeue(&sl->tb);
			sl->statem.Cm--;
			if (!sl->statem.Cm)
				qenable(sl->iq);	/* back enable */
			sl->statem.tx.L.fsn = (sl->statem.tx.L.fsn + 1) & sl->statem.sn_mask;
			sl->statem.tx.N.fsn = sl->statem.tx.L.fsn;
			if (!sl->rtb.q_count)
				sl_timer_start(sl, t7);
			bufq_queue(&sl->rtb, bp);
			sl->statem.Ct++;
			sl_rc_fsnt_value(q, sl);
			if (pcr) {
				if ((sl->rtb.q_msgs >= sl->config.N1)
				    || (sl->rtb.q_count >= sl->config.N2)) {
					sl->statem.rtb_full = 1;
					sl->statem.forced_retransmission = 1;
				}
			} else {
				if ((sl->rtb.q_msgs >= sl->config.N1)
				    || (sl->rtb.q_count >= sl->config.N2)
				    || (sl->statem.tx.L.fsn ==
					((sl->statem.tx.F.fsn - 2) & sl->statem.sn_mask)))
					sl->statem.rtb_full = 1;
			}
			// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			sl->statem.tx.N.bsn = (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
			// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
			sl_daedt_msu(q, sl, mp);
		}
		spin_unlock(&sl->tb.q_lock);
		return (mp);
	}
}

STATIC INLINE int
sl_daedr_start(queue_t *q, struct sl *sl)
{
	return sdt_daedr_start_req(q, sl);
}

STATIC INLINE int
sl_rc_start(queue_t *q, struct sl *sl)
{
	int err;
	if (sl->statem.rc_state == SL_STATE_IDLE) {
		sl->statem.rx.X.fsn = 0;
		sl->statem.rx.X.fib = sl->statem.ib_mask;
		sl->statem.rx.F.fsn = 0;
		sl->statem.rx.T.fsn = sl->statem.sn_mask;
		sl->statem.rtr = 0;	/* Basic only (note 1). */
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->statem.msu_fisu_accepted = 1;
		else
			sl->statem.msu_fisu_accepted = 0;
		sl->statem.abnormal_bsnr = 0;
		sl->statem.abnormal_fibr = 0;	/* Basic only (note 1). */
		sl->statem.congestion_discard = 0;
		sl->statem.congestion_accept = 0;
		if ((err = sl_daedr_start(q, sl)))
			return (err);
		sl->statem.rc_state = SL_STATE_IN_SERVICE;
		/* 
		 *   Note 1 - Although rtr and abnormal_fibr are only
		 *   applicable to the Basic procedure (and not PCR), these
		 *   state machine variables are never examined by PCR
		 *   routines, so PCR and basic can share the same start
		 *   procedures.
		 */
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_rc_reject_msu_fisu(queue_t *q, struct sl *sl)
{
	sl->statem.msu_fisu_accepted = 0;
}

STATIC INLINE void
sl_rc_accept_msu_fisu(queue_t *q, struct sl *sl)
{
	sl->statem.msu_fisu_accepted = 1;
}

STATIC INLINE void
sl_rc_retrieve_fsnx(queue_t *q, struct sl *sl)
{
	sl_txc_fsnx_value(q, sl);	/* error in 93 spec */
	sl->statem.congestion_discard = 0;
	sl->statem.congestion_accept = 0;
	sl_cc_normal(q, sl);
	sl->statem.rtr = 0;	/* basic only */
}

STATIC INLINE void
sl_rc_align_fsnx(queue_t *q, struct sl *sl)
{
	sl_txc_fsnx_value(q, sl);
}

STATIC INLINE int
sl_rc_clear_rb(queue_t *q, struct sl *sl)
{
	bufq_purge(&sl->rb);
	// flushq(sl->oq, FLUSHDATA);
	sl->statem.Cr = 0;
	return sl_rb_cleared_ind(q, sl);
}

STATIC INLINE int
sl_rc_retrieve_bsnt(queue_t *q, struct sl *sl)
{
	sl->statem.rx.T.bsn = (sl->statem.rx.X.fsn - 1) & 0x7F;
	return sl_bsnt_ind(q, sl, sl->statem.rx.T.bsn);
}

STATIC INLINE void
sl_cc_busy(queue_t *q, struct sl *sl)
{
	if (sl->statem.cc_state == SL_STATE_NORMAL) {
		sl_txc_send_sib(q, sl);
		sl_timer_start(sl, t5);
		sl->statem.cc_state = SL_STATE_BUSY;
	}
}

STATIC INLINE void
sl_rc_congestion_discard(queue_t *q, struct sl *sl)
{
	sl->statem.congestion_discard = 1;
	sl_cc_busy(q, sl);
}

STATIC INLINE void
sl_rc_congestion_accept(queue_t *q, struct sl *sl)
{
	sl->statem.congestion_accept = 1;
	sl_cc_busy(q, sl);
}

STATIC INLINE void
sl_rc_no_congestion(queue_t *q, struct sl *sl)
{
	sl->statem.congestion_discard = 0;
	sl->statem.congestion_accept = 0;
	sl_cc_normal(q, sl);
	sl_txc_fsnx_value(q, sl);
	if (sl->statem.rtr == 1) {	/* rtr never set for PCR */
		ctrace(sl_txc_nack_to_be_sent(q, sl));
		sl->statem.rx.X.fib = sl->statem.rx.X.fib ? 0 : sl->statem.ib_mask;
	}
}

STATIC INLINE void
sl_lsc_congestion_discard(queue_t *q, struct sl *sl)
{
	sl_rc_congestion_discard(q, sl);
	sl->statem.l3_congestion_detect = 1;
}

STATIC INLINE void
sl_lsc_congestion_accept(queue_t *q, struct sl *sl)
{
	sl_rc_congestion_accept(q, sl);
	sl->statem.l3_congestion_detect = 1;
}

STATIC INLINE void
sl_lsc_no_congestion(queue_t *q, struct sl *sl)
{
	sl_rc_no_congestion(q, sl);
	sl->statem.l3_congestion_detect = 0;
}

STATIC INLINE int
sl_lsc_sio(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_INITIAL_ALIGNMENT:
		break;
	default:
		sl_timer_stop(sl, t1);	/* ok if not running */
		sl_out_of_service_ind(q, sl, SL_FAIL_RECEIVED_SIO);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIO;
		sl_rc_stop(q, sl);
		if ((err = sl_suerm_stop(q, sl)))
			return (err);
		sl_poc_stop(q, sl);	/* ok if ANSI */
		sl_txc_send_sios(q, sl);
		sl->statem.emergency = 0;
		/* 
		   FIXME: reinspect */
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if ITUT */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_alignment_not_possible(queue_t *q, struct sl *sl)
{
	int err;
	if ((err = sl_out_of_service_ind(q, sl, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)))
		return (err);
	sl->statem.failure_reason = SL_FAIL_ALIGNMENT_NOT_POSSIBLE;
	sl_rc_stop(q, sl);
	sl_txc_send_sios(q, sl);
	sl->statem.local_processor_outage = 0;
	sl->statem.emergency = 0;
	sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	return (QR_DONE);
}

STATIC INLINE int
sl_iac_sio(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (sl->statem.emergency) {
			sl->statem.t4v = sl->config.t4e;
			printd(("%s: Sending SIE at %lu\n", MOD_NAME, jiffies));
			sl_txc_send_sie(q, sl);
		} else {
			sl->statem.t4v = sl->config.t4n;
			sl_txc_send_sin(q, sl);
		}
		sl_timer_start(sl, t3);
		sl->statem.iac_state = SL_STATE_ALIGNED;
		break;
	case SL_STATE_PROVING:
		sl_timer_stop(sl, t4);
		if ((err = sl_aerm_stop(q, sl)))
			return (err);
		sl_timer_start(sl, t3);
		sl->statem.iac_state = SL_STATE_ALIGNED;
		break;
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_iac_sios(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.iac_state) {
	case SL_STATE_ALIGNED:
	case SL_STATE_PROVING:
		sl_timer_stop(sl, t4);	/* ok if not running */
		if ((err = sl_lsc_alignment_not_possible(q, sl)))
			return (err);
		if ((err = sl_aerm_stop(q, sl)))	/* ok if not running */
			return (err);
		sl_timer_stop(sl, t3);	/* ok if not running */
		sl->statem.emergency = 0;
		sl->statem.iac_state = SL_STATE_IDLE;
		break;
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_sios(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
	case SL_STATE_ALIGNED_NOT_READY:
		sl_timer_stop(sl, t1);	/* ok to stop if not running */
	case SL_STATE_IN_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(q, sl, SL_FAIL_RECEIVED_SIOS);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIOS;
		if ((err = sl_suerm_stop(q, sl)))	/* ok to stop if not running */
			return (err);
		sl_rc_stop(q, sl);
		sl_poc_stop(q, sl);	/* ok if ANSI */
		sl_txc_send_sios(q, sl);
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if ITU */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_lsc_no_processor_outage(queue_t *q, struct sl *sl)
{
	if (sl->statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->statem.processor_outage = 0;
		if (!sl->statem.l3_indication_received)
			return;
		sl->statem.l3_indication_received = 0;
		sl_txc_send_msu(q, sl);
		sl->statem.local_processor_outage = 0;
		sl_rc_accept_msu_fisu(q, sl);
		sl->statem.lsc_state = SL_STATE_IN_SERVICE;
	}
}

STATIC INLINE void
sl_poc_remote_processor_recovered(queue_t *q, struct sl *sl)
{
	switch (sl->statem.poc_state) {
	case SL_STATE_REMOTE_PROCESSOR_OUTAGE:
		sl_lsc_no_processor_outage(q, sl);
		sl->statem.poc_state = SL_STATE_IDLE;
		return;
	case SL_STATE_BOTH_PROCESSORS_OUT:
		sl->statem.poc_state = SL_STATE_LOCAL_PROCESSOR_OUTAGE;
		return;
	}
}

STATIC INLINE int
sl_lsc_fisu_msu_received(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
		if ((err = sl_in_service_ind(q, sl)))
			return (err);
		if (sl->option.pvar == SS7_PVAR_ITUT_93)
			sl_rc_accept_msu_fisu(q, sl);	/* unnecessary */
		sl_timer_stop(sl, t1);
		sl_txc_send_msu(q, sl);
		sl->statem.lsc_state = SL_STATE_IN_SERVICE;
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if ((err = sl_in_service_ind(q, sl)))
			return (err);
		sl_timer_stop(sl, t1);
		sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_poc_remote_processor_recovered(q, sl);
			return sl_remote_processor_recovered_ind(q, sl);
		} else {
#if 0
			sl->statem.remote_processor_outage = 0;
			return sl_remote_processor_recovered_ind(q, sl);
#else
			/* 
			 *  A deviation from the SDLs has been placed here to
			 *  limit the number of remote processor recovered
			 *  indications which are delivered to L3.  One
			 *  indication is sufficient.
			 */
			if (sl->statem.remote_processor_outage) {
				sl->statem.remote_processor_outage = 0;
				return sl_remote_processor_recovered_ind(q, sl);
			}
#endif
		}
		break;
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_poc_remote_processor_outage(queue_t *q, struct sl *sl)
{
	switch (sl->statem.poc_state) {
	case SL_STATE_IDLE:
		sl->statem.poc_state = SL_STATE_REMOTE_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_LOCAL_PROCESSOR_OUTAGE:
		sl->statem.poc_state = SL_STATE_BOTH_PROCESSORS_OUT;
		return;
	}
}

STATIC INLINE void
sl_lsc_sib(queue_t *q, struct sl *sl)
{
	switch (sl->statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_txc_sib_received(q, sl);
		break;
	}
}

STATIC INLINE int
sl_lsc_sipo(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_timer_stop(sl, t1);
			if ((err = sl_remote_processor_outage_ind(q, sl)))
				return (err);
			sl_poc_remote_processor_outage(q, sl);
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			sl_timer_stop(sl, t1);
			if ((err = sl_remote_processor_outage_ind(q, sl)))
				return (err);
			sl->statem.remote_processor_outage = 1;
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			if ((err = sl_remote_processor_outage_ind(q, sl)))
				return (err);
			sl_poc_remote_processor_outage(q, sl);
			sl_timer_stop(sl, t1);
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			if ((err = sl_remote_processor_outage_ind(q, sl)))
				return (err);
			sl->statem.remote_processor_outage = 1;
			sl_timer_stop(sl, t1);
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_IN_SERVICE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_txc_send_fisu(q, sl);
			if ((err = sl_remote_processor_outage_ind(q, sl)))
				return (err);
			sl_poc_remote_processor_outage(q, sl);
			sl->statem.processor_outage = 1;	/* remote? */
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			sl_txc_send_fisu(q, sl);
			if ((err = sl_remote_processor_outage_ind(q, sl)))
				return (err);
			sl->statem.remote_processor_outage = 1;
			sl_rc_align_fsnx(q, sl);
			sl_cc_stop(q, sl);
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			if ((err = sl_remote_processor_outage_ind(q, sl)))
				return (err);
			sl_poc_remote_processor_outage(q, sl);
		} else {
#if 0
			sl->statem.remote_processor_outage = 1;
			return sl_remote_processor_outage_ind(q, sl);
#else
			/* 
			 *  A deviation from the SDLs has been placed here to
			 *  limit the number of remote processor outage
			 *  indications which are delivered to L3.  One
			 *  indication is sufficient.
			 */
			if (!sl->statem.remote_processor_outage) {
				sl->statem.remote_processor_outage = 1;
				return sl_remote_processor_outage_ind(q, sl);
			}
#endif
		}
		break;
	}
	return (QR_DONE);
}

STATIC INLINE void
sl_poc_local_processor_outage(queue_t *q, struct sl *sl)
{
	switch (sl->statem.poc_state) {
	case SL_STATE_IDLE:
		sl->statem.poc_state = SL_STATE_LOCAL_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_REMOTE_PROCESSOR_OUTAGE:
		sl->statem.poc_state = SL_STATE_BOTH_PROCESSORS_OUT;
		return;
	}
}

STATIC INLINE int
sl_suerm_start(queue_t *q, struct sl *sl)
{
	return sdt_suerm_start_req(q, sl);
}

STATIC INLINE int
sl_lsc_alignment_complete(queue_t *q, struct sl *sl)
{
	int err;
	if (sl->statem.lsc_state == SL_STATE_INITIAL_ALIGNMENT) {
		if ((err = sl_suerm_start(q, sl)))
			return (err);
		sl_timer_start(sl, t1);
		if (sl->statem.local_processor_outage) {
			if (sl->option.pvar != SS7_PVAR_ANSI_92)
				sl_poc_local_processor_outage(q, sl);
			sl_txc_send_sipo(q, sl);
			if (sl->option.pvar != SS7_PVAR_ITUT_93)	/* possible error */
				sl_rc_reject_msu_fisu(q, sl);
			sl->statem.lsc_state = SL_STATE_ALIGNED_NOT_READY;
		} else {
			sl_txc_send_msu(q, sl);	/* changed from send_fisu for Q.781 */
			sl_rc_accept_msu_fisu(q, sl);	/* error in ANSI spec? */
			sl->statem.lsc_state = SL_STATE_ALIGNED_READY;
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_sin(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
		sl_out_of_service_ind(q, sl, SL_FAIL_RECEIVED_SIN);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIN;
		if ((err = sl_suerm_stop(q, sl)))
			return (err);
		sl_rc_stop(q, sl);
		sl_txc_send_sios(q, sl);
		sl->statem.emergency = 0;
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(q, sl, SL_FAIL_RECEIVED_SIN);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIN;
		if ((err = sl_suerm_stop(q, sl)))
			return (err);
		sl_rc_stop(q, sl);
		sl_poc_stop(q, sl);	/* ok if not ITUT */
		sl_txc_send_sios(q, sl);
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if not ANSI */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_aerm_set_ti_to_tie(queue_t *q, struct sl *sl)
{
	return sdt_aerm_set_ti_to_tie_req(q, sl);
}

STATIC INLINE int
sl_aerm_start(queue_t *q, struct sl *sl)
{
	return sdt_aerm_start_req(q, sl);
}

STATIC INLINE int
sl_iac_sin(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (sl->statem.emergency) {
			sl->statem.t4v = sl->config.t4e;
			sl_txc_send_sie(q, sl);
		} else {
			sl->statem.t4v = sl->config.t4n;
			sl_txc_send_sin(q, sl);
		}
		sl_timer_start(sl, t3);
		sl->statem.iac_state = SL_STATE_ALIGNED;
		break;
	case SL_STATE_ALIGNED:
		sl_timer_stop(sl, t3);
		if (sl->statem.t4v == sl->config.t4e)
			if ((err = sl_aerm_set_ti_to_tie(q, sl)))
				return (err);
		if ((err = sl_aerm_start(q, sl)))
			return (err);
		sl_timer_start(sl, t4);
		sl->statem.further_proving = 0;
		sl->statem.Cp = 0;
		sl->statem.iac_state = SL_STATE_PROVING;
		break;
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_sie(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
		sl_out_of_service_ind(q, sl, SL_FAIL_RECEIVED_SIE);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIE;
		if ((err = sl_suerm_stop(q, sl)))
			return (err);
		sl_rc_stop(q, sl);
		sl_txc_send_sios(q, sl);
		sl->statem.emergency = 0;
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(q, sl, SL_FAIL_RECEIVED_SIE);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIE;
		if ((err = sl_suerm_stop(q, sl)))
			return (err);
		sl_rc_stop(q, sl);
		sl_poc_stop(q, sl);	/* ok if not ITUT */
		sl_txc_send_sios(q, sl);
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if not ANSI */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_iac_sie(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (sl->statem.emergency) {
			sl->statem.t4v = sl->config.t4e;
			sl_txc_send_sie(q, sl);
		} else {
			sl->statem.t4v = sl->config.t4e;	/* yes e */
			sl_txc_send_sin(q, sl);
		}
		sl_timer_start(sl, t3);
		sl->statem.iac_state = SL_STATE_ALIGNED;
		break;
	case SL_STATE_ALIGNED:
		printd(("%s: Receiving SIE at %lu\n", MOD_NAME, jiffies));
		sl->statem.t4v = sl->config.t4e;
		sl_timer_stop(sl, t3);
		if ((err = sl_aerm_set_ti_to_tie(q, sl)))
			return (err);
		if ((err = sl_aerm_start(q, sl)))
			return (err);
		sl_timer_start(sl, t4);
		sl->statem.further_proving = 0;
		sl->statem.Cp = 0;
		sl->statem.iac_state = SL_STATE_PROVING;
		break;
	case SL_STATE_PROVING:
		if (sl->statem.t4v == sl->config.t4e)
			break;
		sl_timer_stop(sl, t4);
		sl->statem.t4v = sl->config.t4e;
		if ((err = sl_aerm_stop(q, sl)))
			return (err);
		if ((err = sl_aerm_set_ti_to_tie(q, sl)))
			return (err);
		if ((err = sl_aerm_start(q, sl)))
			return (err);
		sl_timer_start(sl, t4);
		sl->statem.further_proving = 0;
		break;
	}
	return (QR_DONE);
}

/*
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

STATIC INLINE void
sl_rb_congestion_function(queue_t *q, struct sl *sl)
{
	if (!sl->statem.l3_congestion_detect) {
		if (sl->statem.l2_congestion_detect) {
			if (sl->statem.Cr <= sl->config.rb_abate && canputnext(sl->oq)) {
				sl_rc_no_congestion(q, sl);
				sl->statem.l2_congestion_detect = 0;
			}
		} else {
			if (sl->statem.Cr >= sl->config.rb_discard || !canput(sl->oq)) {
				sl_rc_congestion_discard(q, sl);
				sl->statem.l2_congestion_detect = 1;
			} else if (sl->statem.Cr >= sl->config.rb_accept || !canputnext(sl->oq)) {
				sl_rc_congestion_accept(q, sl);
				sl->statem.l2_congestion_detect = 1;
			}
		}
	}
}

STATIC INLINE int
sl_rc_signal_unit(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	int pcr = sl->option.popt & SS7_POPT_PCR;
	if (sl->statem.rc_state != SL_STATE_IN_SERVICE)
		goto discard;
	/* 
	 *  Note: the SDT driver must check that the length of the frame is
	 *  within appropriate bounds as specified by the DAEDR in Q.703.  If
	 *  the length of the frame is incorrect, it should indicate
	 *  daedr_error_frame rather than daedr_received_frame.
	 */
	if (sl->option.popt & SS7_POPT_XSN) {
		sl->statem.rx.R.bsn = ntohs(*(sl_ushort *) mp->b_rptr) & 0x0fff;
		sl->statem.rx.R.bib = ntohs(*(sl_ushort *) mp->b_rptr++) & 0x8000;
		sl->statem.rx.R.fsn = ntohs(*(sl_ushort *) mp->b_rptr) & 0x0fff;
		sl->statem.rx.R.fib = ntohs(*(sl_ushort *) mp->b_rptr++) & 0x8000;
		sl->statem.rx.len = ntohs(*(sl_ushort *) mp->b_rptr++) & 0x01ff;
	} else {
		sl->statem.rx.R.bsn = *mp->b_rptr & 0x7f;
		sl->statem.rx.R.bib = *mp->b_rptr++ & 0x80;
		sl->statem.rx.R.fsn = *mp->b_rptr & 0x7f;
		sl->statem.rx.R.fib = *mp->b_rptr++ & 0x80;
		sl->statem.rx.len = *mp->b_rptr++ & 0x3f;
	}
	printd(("%s: %p: [%x/%x] SU [%d] li = %d <-\n", MOD_NAME, sl,
		sl->statem.rx.R.bsn | sl->statem.rx.R.bib,
		sl->statem.rx.R.fsn | sl->statem.rx.R.fib, msgdsize(mp), sl->statem.rx.len));
	if (sl->statem.rx.len == 1) {
		sl->statem.rx.sio = *mp->b_rptr++ & 0x07;
	}
	if (sl->statem.rx.len == 2) {
		sl->statem.rx.sio = *mp->b_rptr++ & 0x07;
		sl->statem.rx.sio = *mp->b_rptr++ & 0x07;
	}
	if (((sl->statem.rx.len) == 1) || ((sl->statem.rx.len) == 2)) {
		switch (sl->statem.rx.sio) {
		case LSSU_SIO:{
			printd(("%s: %p: SIO <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sio(q, sl)))
				return (err);
			if ((err = sl_iac_sio(q, sl)))
				return (err);
			break;
		}
		case LSSU_SIN:{
			printd(("%s: %p: SIN <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sin(q, sl)))
				return (err);
			if ((err = sl_iac_sin(q, sl)))
				return (err);
			break;
		}
		case LSSU_SIE:{
			printd(("%s: %p: SIE <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sie(q, sl)))
				return (err);
			if ((err = sl_iac_sie(q, sl)))
				return (err);
			break;
		}
		case LSSU_SIOS:{
			printd(("%s: %p: SIOS <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sios(q, sl)))
				return (err);
			if ((err = sl_iac_sios(q, sl)))
				return (err);
			break;
		}
		case LSSU_SIPO:{
			printd(("%s: %p: SIPO <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sipo(q, sl)))
				return (err);
			break;
		}
		case LSSU_SIB:{
			printd(("%s: %p: SIB <-\n", MOD_NAME, sl));
			sl_lsc_sib(q, sl);
			break;
		}
		}
		goto discard;
	}
	printd(("%s: %p: [%x/%x] FISU or MSU [%d] li = %d <-\n", MOD_NAME, sl,
		sl->statem.rx.R.bib | sl->statem.rx.R.bsn,
		sl->statem.rx.R.fib | sl->statem.rx.R.fsn, msgdsize(mp), sl->statem.rx.len));
	if (SN_OUTSIDE
	    (((sl->statem.rx.F.fsn - 1) & sl->statem.sn_mask), sl->statem.rx.R.bsn,
	     sl->statem.rx.T.fsn)) {
		if (sl->statem.abnormal_bsnr) {
			if ((err = sl_lsc_link_failure(q, sl, SL_FAIL_ABNORMAL_BSNR)))
				return (err);
			sl->statem.rc_state = SL_STATE_IDLE;
			goto discard;
		} else {
			sl->statem.abnormal_bsnr = 1;
			sl->statem.unb = 0;
			goto discard;
		}
	}
	if (sl->statem.abnormal_bsnr) {
		if (sl->statem.unb == 1) {
			sl->statem.abnormal_bsnr = 0;
		} else {
			sl->statem.unb = 1;
			goto discard;
		}
	}
	if (pcr) {
		if ((err = sl_lsc_fisu_msu_received(q, sl)))
			return (err);
		if ((err = sl_txc_bsnr_and_bibr(q, sl)))
			return (err);
		sl->statem.rx.F.fsn = (sl->statem.rx.R.bsn + 1) & sl->statem.sn_mask;
		if (!sl->statem.msu_fisu_accepted)
			goto discard;
		sl_rb_congestion_function(q, sl);
		if (sl->statem.congestion_discard) {
			sl_cc_busy(q, sl);
			goto discard;
		}
		if ((sl->statem.rx.R.fsn == sl->statem.rx.X.fsn)
		    && (sl->statem.rx.len > 2)) {
			sl->statem.rx.X.fsn = (sl->statem.rx.X.fsn + 1) & sl->statem.sn_mask;
			printd(("%s: %p: MSU [%d] <-\n", MOD_NAME, sl, msgdsize(mp)));
			bufq_queue(&sl->rb, mp);
			sl->statem.Cr++;
			if (sl->statem.congestion_accept)
				sl_cc_busy(q, sl);
			else
				sl_txc_fsnx_value(q, sl);
			return (QR_ABSORBED);
		}
		goto discard;
	}
	if (sl->statem.rx.R.fib == sl->statem.rx.X.fib) {
		if (sl->statem.abnormal_fibr) {
			if (sl->statem.unf == 1) {
				sl->statem.abnormal_fibr = 0;
			} else {
				sl->statem.unf = 1;
				goto discard;
			}
		}
		if ((err = sl_lsc_fisu_msu_received(q, sl)))
			return (err);
		if ((err = sl_txc_bsnr_and_bibr(q, sl)))
			return (err);
		sl->statem.rx.F.fsn = (sl->statem.rx.R.bsn + 1) & sl->statem.sn_mask;
		if (!sl->statem.msu_fisu_accepted)
			goto discard;
		sl_rb_congestion_function(q, sl);
		if (sl->statem.congestion_discard) {
			sl->statem.rtr = 1;
			sl_cc_busy(q, sl);
			goto discard;
		}
		if ((sl->statem.rx.R.fsn == sl->statem.rx.X.fsn)
		    && (sl->statem.rx.len > 2)) {
			sl->statem.rx.X.fsn = (sl->statem.rx.X.fsn + 1) & sl->statem.sn_mask;
			sl->statem.rtr = 0;
			printd(("%s: %p: MSU [%d] <-\n", MOD_NAME, sl, msgdsize(mp)));
			bufq_queue(&sl->rb, mp);
			sl->statem.Cr++;
			if (sl->statem.congestion_accept)
				sl_cc_busy(q, sl);
			else
				sl_txc_fsnx_value(q, sl);
			return (QR_ABSORBED);
		}
		if ((sl->statem.rx.R.fsn == ((sl->statem.rx.X.fsn - 1) & sl->statem.sn_mask)))
			goto discard;
		else {
			if (sl->statem.congestion_accept) {
				sl->statem.rtr = 1;
				sl_cc_busy(q, sl);	/* not required? */
				goto discard;
			} else {
				ctrace(sl_txc_nack_to_be_sent(q, sl));
				sl->statem.rtr = 1;
				sl->statem.rx.X.fib = sl->statem.rx.X.fib ? 0 : sl->statem.ib_mask;
				goto discard;
			}
		}
	} else {
		if (sl->statem.abnormal_fibr) {
			if ((err = sl_lsc_link_failure(q, sl, SL_FAIL_ABNORMAL_FIBR)))
				return (err);
			goto discard;
		}
		if (sl->statem.rtr == 1) {
			if ((err = sl_txc_bsnr_and_bibr(q, sl)))
				return (err);
			sl->statem.rx.F.fsn = (sl->statem.rx.R.bsn + 1) & sl->statem.sn_mask;
			goto discard;
		}
		sl->statem.abnormal_fibr = 1;
		sl->statem.unf = 0;
		goto discard;
	}
      discard:
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_stop(queue_t *q, struct sl *sl)
{
	int err;
	if (sl->statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
		if ((err = sl_iac_stop(q, sl)))	/* ok if not running */
			return (err);
		sl_timer_stop(sl, t1);	/* ok if not running */
		sl_rc_stop(q, sl);
		if ((err = sl_suerm_stop(q, sl)))	/* ok if not running */
			return (err);
		sl_poc_stop(q, sl);	/* ok if not running or not ITUT */
		sl_txc_send_sios(q, sl);
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok of not ANSI */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_clear_rtb(queue_t *q, struct sl *sl)
{
	if (sl->statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->statem.local_processor_outage = 0;
		sl_txc_send_fisu(q, sl);
		return sl_txc_clear_rtb(q, sl);
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_iac_correct_su(queue_t *q, struct sl *sl)
{
	int err;
	if (sl->statem.iac_state == SL_STATE_PROVING) {
		if (sl->statem.further_proving) {
			sl_timer_stop(sl, t4);
			if ((err = sl_aerm_start(q, sl)))
				return (err);
			sl->statem.further_proving = 0;
			sl_timer_start(sl, t4);
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_iac_abort_proving(queue_t *q, struct sl *sl)
{
	int err;
	if (sl->statem.iac_state == SL_STATE_PROVING) {
		sl->statem.Cp++;
		if (sl->statem.Cp == sl->config.M) {
			if ((err = sl_lsc_alignment_not_possible(q, sl)))
				return (err);
			sl_timer_stop(sl, t4);
			if ((err = sl_aerm_stop(q, sl)))
				return (err);
			sl->statem.emergency = 0;
			sl->statem.iac_state = SL_STATE_IDLE;
			return (QR_DONE);
		}
		sl->statem.further_proving = 1;
	}
	return (QR_DONE);
}

#define sl_lsc_flush_buffers sl_lsc_clear_buffers
STATIC INLINE int
sl_lsc_clear_buffers(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if ((err = sl_rtb_cleared_ind(q, sl)))
				return (err);
			sl->statem.local_processor_outage = 0;	/* ??? */
		}
		break;
	case SL_STATE_INITIAL_ALIGNMENT:
		if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if ((err = sl_rtb_cleared_ind(q, sl)))
				return (err);
			sl->statem.local_processor_outage = 0;
		}
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if ((err = sl_rtb_cleared_ind(q, sl)))
				return (err);
			sl->statem.local_processor_outage = 0;
			sl_txc_send_fisu(q, sl);
			sl->statem.lsc_state = SL_STATE_ALIGNED_READY;
		}
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_txc_flush_buffers(q, sl);
			sl->statem.l3_indication_received = 1;
			if (sl->statem.processor_outage)
				return (QR_DONE);
			sl->statem.l3_indication_received = 0;
			sl_txc_send_msu(q, sl);
			sl->statem.local_processor_outage = 0;
			sl_rc_accept_msu_fisu(q, sl);
			sl->statem.lsc_state = SL_STATE_IN_SERVICE;
		} else {
			sl->statem.local_processor_outage = 0;
			if ((err = sl_rc_clear_rb(q, sl)))
				return (err);
			sl_rc_accept_msu_fisu(q, sl);
			sl_txc_send_fisu(q, sl);
			if ((err = sl_txc_clear_tb(q, sl)))
				return (err);
			if ((err = sl_txc_clear_rtb(q, sl)))
				return (err);
		}
		break;
	}
	return (QR_DONE);
}

#if 0
STATIC INLINE void
sl_lsc_continue(queue_t *q, struct sl *sl, mblk_t *mp)
{
	if (sl->statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		if (sl->statem.processor_outage)
			return;
		sl->statem.l3_indication_received = 0;
		sl_txc_send_msu(q, sl);
		sl->statem.local_processor_outage = 0;
		sl_rc_accept_msu_fisu(q, sl);
		sl->statem.lsc_state = SL_STATE_IN_SERVICE;
	}
}
#endif

STATIC INLINE void
sl_poc_local_processor_recovered(queue_t *q, struct sl *sl)
{
	switch (sl->statem.poc_state) {
	case SL_STATE_LOCAL_PROCESSOR_OUTAGE:
		sl_lsc_no_processor_outage(q, sl);
		sl->statem.poc_state = SL_STATE_IDLE;
		return;
	case SL_STATE_BOTH_PROCESSORS_OUT:
		sl->statem.poc_state = SL_STATE_REMOTE_PROCESSOR_OUTAGE;
		return;
	}
}

#define sl_lsc_resume sl_lsc_local_processor_recovered
STATIC INLINE void
sl_lsc_local_processor_recovered(queue_t *q, struct sl *sl)
{
	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		sl->statem.local_processor_outage = 0;
		break;
	case SL_STATE_INITIAL_ALIGNMENT:
		sl->statem.local_processor_outage = 0;
		break;
	case SL_STATE_ALIGNED_READY:
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if (sl->option.pvar != SS7_PVAR_ANSI_92)
			sl_poc_local_processor_recovered(q, sl);
		sl->statem.local_processor_outage = 0;
		sl_txc_send_fisu(q, sl);
		if (sl->option.pvar == SS7_PVAR_ITUT_96)
			sl_rc_accept_msu_fisu(q, sl);
		sl->statem.lsc_state = SL_STATE_ALIGNED_READY;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_poc_local_processor_recovered(q, sl);
			sl_rc_retrieve_fsnx(q, sl);
			sl_txc_send_fisu(q, sl);	/* note 3: in fisu BSN <= FSNX-1 */
			sl->statem.lsc_state = SL_STATE_IN_SERVICE;
		} else {
			sl->statem.local_processor_outage = 0;
			sl_rc_accept_msu_fisu(q, sl);
			if (sl->statem.remote_processor_outage) {
				sl_txc_send_fisu(q, sl);
				sl_remote_processor_outage_ind(q, sl);
			} else {
				sl_txc_send_msu(q, sl);
				sl->statem.lsc_state = SL_STATE_IN_SERVICE;
			}
		}
	}
	return;
}

#define sl_lsc_level_3_failure sl_lsc_local_processor_outage
STATIC INLINE void
sl_lsc_local_processor_outage(queue_t *q, struct sl *sl)
{
	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_INITIAL_ALIGNMENT:
		sl->statem.local_processor_outage = 1;
		return;
	case SL_STATE_ALIGNED_READY:
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->statem.local_processor_outage = 1;
		else
			sl_poc_local_processor_outage(q, sl);
		sl_txc_send_sipo(q, sl);
		if (sl->option.pvar != SS7_PVAR_ITUT_93)	/* possible error 93 specs */
			sl_rc_reject_msu_fisu(q, sl);
		sl->statem.lsc_state = SL_STATE_ALIGNED_NOT_READY;
		return;
	case SL_STATE_IN_SERVICE:
		if (sl->option.pvar == SS7_PVAR_ANSI_92) {
			sl->statem.local_processor_outage = 1;
		} else {
			sl_poc_local_processor_outage(q, sl);
			sl->statem.processor_outage = 1;
		}
		sl_txc_send_sipo(q, sl);
		sl_rc_reject_msu_fisu(q, sl);
		if (sl->option.pvar == SS7_PVAR_ANSI_92) {
			sl_rc_align_fsnx(q, sl);
			sl_cc_stop(q, sl);
		}
		sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_PROCESSOR_OUTAGE:
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->statem.local_processor_outage = 1;
		else
			sl_poc_local_processor_outage(q, sl);
		sl_txc_send_sipo(q, sl);
		return;
	}
}

STATIC INLINE int
sl_iac_emergency(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.iac_state) {
	case SL_STATE_PROVING:
		sl_txc_send_sie(q, sl);
		sl_timer_stop(sl, t4);
		sl->statem.t4v = sl->config.t4e;
		if ((err = sl_aerm_stop(q, sl)))
			return (err);
		if ((err = sl_aerm_set_ti_to_tie(q, sl)))
			return (err);
		if ((err = sl_aerm_start(q, sl)))
			return (err);
		sl_timer_start(sl, t4);
		sl->statem.further_proving = 0;
		break;
	case SL_STATE_ALIGNED:
		sl_txc_send_sie(q, sl);
		sl->statem.t4v = sl->config.t4e;
		break;
	case SL_STATE_IDLE:
	case SL_STATE_NOT_ALIGNED:
		sl->statem.emergency = 1;
		break;
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_emergency(queue_t *q, struct sl *sl)
{
	int err;
	if ((err = sl_iac_emergency(q, sl)))	/* added to pass Q.781/Test 1.20 */
		return (err);
	sl->statem.emergency = 1;
	return (QR_DONE);
}

STATIC INLINE void
sl_lsc_emergency_ceases(queue_t *q, struct sl *sl)
{
	sl->statem.emergency = 0;
}

STATIC INLINE void
sl_iac_start(queue_t *q, struct sl *sl)
{
	if (sl->statem.iac_state == SL_STATE_IDLE) {
		sl_txc_send_sio(q, sl);
		sl_timer_start(sl, t2);
		sl->statem.iac_state = SL_STATE_NOT_ALIGNED;
	}
}

STATIC INLINE int
sl_daedt_start(queue_t *q, struct sl *sl)
{
	return sdt_daedt_start_req(q, sl);
}

STATIC INLINE int
sl_txc_start(queue_t *q, struct sl *sl)
{
	sl->statem.forced_retransmission = 0;	/* ok if basic */
	sl->statem.sib_received = 0;
	sl->statem.Ct = 0;
	sl->statem.rtb_full = 0;
	sl->statem.clear_rtb = 0;	/* ok if ITU */
	if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
		sl->statem.tx.sio = LSSU_SIOS;
		sl->statem.lssu_available = 1;
	}
	sl->statem.msu_inhibited = 0;
	if (sl->option.popt & SS7_POPT_XSN) {
		sl->statem.sn_mask = 0x7fff;
		sl->statem.ib_mask = 0x8000;
	} else {
		sl->statem.sn_mask = 0x7f;
		sl->statem.ib_mask = 0x80;
	}
	sl->statem.tx.L.fsn = sl->statem.tx.N.fsn = sl->statem.sn_mask;
	sl->statem.tx.X.fsn = 0;
	sl->statem.tx.N.fib = sl->statem.tx.N.bib = sl->statem.ib_mask;
	sl->statem.tx.F.fsn = 0;
	ptrace(("%s: %p: [%x/%x] %ld\n", MOD_NAME, sl, sl->statem.tx.N.bib | sl->statem.tx.N.bsn,
		sl->statem.tx.N.fib | sl->statem.tx.N.fsn, sl->option.popt & SS7_POPT_PCR));
	sl->statem.Cm = 0;
	sl->statem.Z = 0;
	sl->statem.z_ptr = NULL;	/* ok if basic */
	if (sl->statem.txc_state == SL_STATE_IDLE) {
		int err;
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
			sl->statem.lssu_available = 0;
		if ((err = sl_daedt_start(q, sl)))
			return (err);
	}
	sl->statem.txc_state = SL_STATE_SLEEPING;
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_start(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		if ((err = sl_rc_start(q, sl)))
			return (err);
		if ((err = sl_txc_start(q, sl)))	/* Note 2 */
			return (err);
		if (sl->statem.emergency)
			if ((err = sl_iac_emergency(q, sl)))
				return (err);
		sl_iac_start(q, sl);
		sl->statem.lsc_state = SL_STATE_INITIAL_ALIGNMENT;
	}
	return (QR_DONE);
}

/*
 *  Note 2: There is a difference here between ANSI_92 and ITUT_93/96 in that
 *  the transmitters in the ANSI_92 case may transmit one or two SIOSs before
 *  transmitting the first SIO of the initial alignment procedure.  ITUT will
 *  continue idling FISU or LSSU as before the start, then transmit the first
 *  SIO.  These are equivalent.  Because the LSC is in the OUT OF SERVICE
 *  state, the transmitters should be idling SIOS anyway.
 */

STATIC INLINE int
sl_lsc_retrieve_bsnt(queue_t *q, struct sl *sl)
{
	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		return sl_rc_retrieve_bsnt(q, sl);
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_lsc_retrieval_request_and_fsnc(queue_t *q, struct sl *sl, sl_ulong fsnc)
{
	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		return sl_txc_retrieval_request_and_fsnc(q, sl, fsnc);
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_aerm_set_ti_to_tin(queue_t *q, struct sl *sl)
{
	return sdt_aerm_set_ti_to_tin_req(q, sl);
}

/*
 *  This power-on sequence should only be performed once, regardless of how
 *  many times the device driver is opened or closed.  This initializes the
 *  transmitters to send SIOS and should never be changed hence.
 */

STATIC INLINE int
sl_lsc_power_on(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl->statem.lsc_state) {
	case SL_STATE_POWER_OFF:
		if ((err = sl_txc_start(q, sl)))	/* Note 3 */
			return (err);
		sl_txc_send_sios(q, sl);	/* not necessary for ANSI */
		if ((err = sl_aerm_set_ti_to_tin(q, sl)))
			return (err);
		sl->statem.local_processor_outage = 0;
		sl->statem.emergency = 0;
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
	return (QR_DONE);
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
STATIC int
sl_check_congestion(queue_t *q, struct sl *sl)
{
	unsigned int occupancy = sl->iq->q_count + sl->tb.q_count + sl->rtb.q_count;
	int old_cong_status = sl->statem.cong_status;
	int old_disc_status = sl->statem.disc_status;
	int multi = sl->option.popt & SS7_POPT_MPLEV;
	switch (sl->statem.cong_status) {
	case 0:
		if (occupancy >= sl->config.tb_onset_1) {
			sl->statem.cong_status = 1;
			if (occupancy >= sl->config.tb_discd_1) {
				sl->statem.disc_status = 1;
				if (!multi)
					break;
				if (occupancy >= sl->config.tb_onset_2) {
					sl->statem.cong_status = 2;
					if (occupancy >= sl->config.tb_discd_2) {
						sl->statem.disc_status = 2;
						if (occupancy >= sl->config.tb_onset_3) {
							sl->statem.cong_status = 3;
							if (occupancy >= sl->config.tb_discd_3) {
								sl->statem.disc_status = 3;
							}
						}
					}
				}
			}
		}
		break;
	case 1:
		if (occupancy < sl->config.tb_abate_1) {
			sl->statem.cong_status = 0;
			sl->statem.disc_status = 0;
		} else {
			if (!multi)
				break;
			if (occupancy >= sl->config.tb_onset_2) {
				sl->statem.cong_status = 2;
				if (occupancy >= sl->config.tb_discd_2) {
					sl->statem.disc_status = 2;
					if (occupancy >= sl->config.tb_onset_3) {
						sl->statem.cong_status = 3;
						if (occupancy >= sl->config.tb_discd_3) {
							sl->statem.disc_status = 3;
						}
					}
				}
			}
		}
		break;
	case 2:
		if (!multi) {
			sl->statem.cong_status = 1;
			sl->statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->config.tb_abate_2) {
			sl->statem.cong_status = 1;
			sl->statem.disc_status = 1;
			if (occupancy < sl->config.tb_abate_1) {
				sl->statem.cong_status = 0;
				sl->statem.disc_status = 0;
			}
		} else if (occupancy >= sl->config.tb_onset_3) {
			sl->statem.cong_status = 3;
			if (occupancy >= sl->config.tb_discd_3) {
				sl->statem.disc_status = 3;
			}
		}
		break;
	case 3:
		if (!multi) {
			sl->statem.cong_status = 1;
			sl->statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->config.tb_abate_3) {
			sl->statem.cong_status = 2;
			sl->statem.disc_status = 2;
			if (occupancy < sl->config.tb_abate_2) {
				sl->statem.cong_status = 1;
				sl->statem.disc_status = 1;
				if (occupancy < sl->config.tb_abate_1) {
					sl->statem.cong_status = 0;
					sl->statem.disc_status = 0;
				}
			}
		}
		break;
	}
	if (sl->statem.cong_status != old_cong_status || sl->statem.disc_status != old_disc_status) {
		if (sl->statem.cong_status < old_cong_status)
			return sl_link_congestion_ceased_ind(q, sl, sl->statem.cong_status,
							     sl->statem.disc_status);
		else {
			if (sl->statem.cong_status > old_cong_status) {
				if (sl->notify.events & SL_EVT_CONGEST_ONSET_IND
				    && !sl->stats.sl_cong_onset_ind[sl->statem.cong_status]++) {
					return sl_link_congested_ind(q, sl, sl->statem.cong_status,
								     sl->statem.disc_status);
				}
			} else {
				if (sl->notify.events & SL_EVT_CONGEST_DISCD_IND
				    && !sl->stats.sl_cong_discd_ind[sl->statem.disc_status]++) {
					return sl_link_congested_ind(q, sl, sl->statem.cong_status,
								     sl->statem.disc_status);
				}
			}
			return sl_link_congested_ind(q, sl, sl->statem.cong_status,
						     sl->statem.disc_status);
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sl_txc_message_for_transmission(queue_t *q, struct sl *sl, mblk_t *mp)
{
	int err;
	bufq_queue(&sl->tb, mp);
	sl->statem.Cm++;
	if (!(err = sl_check_congestion(q, sl))) {
		sl_daedt_transmitter_wakeup(q, sl);
		return (QR_ABSORBED);
	}
	bufq_unlink(&sl->tb, mp);
	sl->statem.Cm--;
	return (err);
}

STATIC INLINE int
sl_lsc_pdu(queue_t *q, struct sl *sl, mblk_t *mp, ulong priority)
{
	mblk_t *hp;
	int hlen = (sl->option.popt & SS7_POPT_XSN) ? 6 : 3;
	if (mp->b_rptr >= mp->b_datap->db_base + hlen) {
		hp = mp;
		hp->b_rptr -= hlen;
	} else if ((hp = ss7_allocb(q, hlen, BPRI_MED))) {
		hp->b_cont = mp;
		hp->b_wptr = hp->b_rptr + hlen;
	} else {
		rare();
		return (-ENOBUFS);
	}
	bzero(hp->b_rptr, hlen);
	if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_JTTC)
		hp->b_rptr[hlen - 1] = (priority << 6);
	return sl_txc_message_for_transmission(q, sl, hp);
}

/*
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */

/*
 *  TX WAKEUP
 *  -----------------------------------
 */
STATIC streamscall void
sl_tx_wakeup(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	while ((sl->statem.txc_state == SL_STATE_IN_SERVICE) && canputnext(sl->iq)
	       && (mp = sl_txc_transmission_request(q, sl))) {
		printd(("%s: %p: M_DATA [%d] ->\n", MOD_NAME, sl, msgdsize(mp)));
		putnext(sl->iq, mp);
	}
	return;
}

/*
 *  RX WAKEUP
 *  -----------------------------------
 */
STATIC streamscall void
sl_rx_wakeup(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	if (sl->rb.q_msgs && canputnext(sl->oq)) {
		switch (sl->statem.lsc_state) {
		case SL_STATE_INITIAL_ALIGNMENT:
		case SL_STATE_ALIGNED_READY:
		case SL_STATE_ALIGNED_NOT_READY:
			/* 
			   we shouldn't have receive buffers around in these states */
			swerr();
			bufq_purge(&sl->rb);
			break;
		case SL_STATE_OUT_OF_SERVICE:
			/* 
			   we keep receive buffers hanging around in these states */
			return;
		case SL_STATE_PROCESSOR_OUTAGE:
			if (sl->statem.local_processor_outage)
				/* 
				   we can't deliver */
				break;
			/* 
			   fall thru */
		case SL_STATE_IN_SERVICE:
			/* 
			   when in service we deliver as many buffers as we can */
			do {
				mblk_t *mp;
				mp = bufq_dequeue(&sl->rb);
				sl->statem.Cr--;
				printd(("%s: %p: <- MSU [%d]\n", MOD_NAME, sl, msgdsize(mp)));
				putnext(sl->oq, mp);
			} while (sl->rb.q_msgs && canputnext(sl->oq));
		}
	}
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
 *  T1 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t1_timeout(struct sl *sl)
{
	queue_t *q = NULL;
	int err;
	if ((err = sl_out_of_service_ind(q, sl, SL_FAIL_T1_TIMEOUT)))
		return (err);
	sl->statem.failure_reason = SL_FAIL_T1_TIMEOUT;
	sl_rc_stop(q, sl);
	if ((err = sl_suerm_stop(q, sl)))
		return (err);
	sl_txc_send_sios(q, sl);
	sl->statem.emergency = 0;
	if (sl->statem.lsc_state == SL_STATE_ALIGNED_NOT_READY) {
		sl_poc_stop(q, sl);	/* ok if ANSI */
		sl->statem.local_processor_outage = 0;
	}
	sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	return (QR_DONE);
}

/*
 *  T2 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t2_timeout(struct sl *sl)
{
	queue_t *q = NULL;
	int err;
	if (sl->statem.iac_state == SL_STATE_NOT_ALIGNED) {
		if ((err = sl_lsc_alignment_not_possible(q, sl)))
			return (err);
		sl->statem.emergency = 0;
		sl->statem.iac_state = SL_STATE_IDLE;
	}
	return (QR_DONE);
}

/*
 *  T3 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t3_timeout(struct sl *sl)
{
	queue_t *q = NULL;
	int err;
	if (sl->statem.iac_state == SL_STATE_ALIGNED) {
		if ((err = sl_lsc_alignment_not_possible(q, sl)))
			return (err);
		sl->statem.emergency = 0;
		sl->statem.iac_state = SL_STATE_IDLE;
	}
	return (QR_DONE);
}

/*
 *  T4 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t4_timeout(struct sl *sl)
{
	queue_t *q = NULL;
	int err;
	if (sl->statem.iac_state == SL_STATE_PROVING) {
		if (sl->statem.further_proving) {
			if ((err = sl_aerm_start(q, sl)))
				return (err);
			sl_timer_start(sl, t4);
			sl->statem.further_proving = 0;
		} else {
			if ((err = sl_lsc_alignment_complete(q, sl)))
				return (err);
			if ((err = sl_aerm_stop(q, sl)))
				return (err);
			sl->statem.emergency = 0;
			sl->statem.iac_state = SL_STATE_IDLE;
		}
	}
	return (QR_DONE);
}

/*
 *  T5 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t5_timeout(struct sl *sl)
{
	queue_t *q = NULL;
	if (sl->statem.cc_state == SL_STATE_BUSY) {
		sl_txc_send_sib(q, sl);
		sl_timer_start(sl, t5);
	}
	return (QR_DONE);
}

/*
 *  T6 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t6_timeout(struct sl *sl)
{
	queue_t *q = NULL;
	int err;
	if ((err = sl_lsc_link_failure(q, sl, SL_FAIL_CONG_TIMEOUT)))
		return (err);
	sl->statem.sib_received = 0;
	sl_timer_stop(sl, t7);
	return (QR_DONE);
}

/*
 *  T7 EXPIRY
 *  -----------------------------------
 */
STATIC int
sl_t7_timeout(struct sl *sl)
{
	queue_t *q = NULL;
	int err;
	if ((err = sl_lsc_link_failure(q, sl, SL_FAIL_ACK_TIMEOUT)))
		return (err);
	sl_timer_stop(sl, t6);
	if (sl->option.pvar == SS7_PVAR_ITUT_96)
		sl->statem.sib_received = 0;
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
 *  LMI_INFO_REQ
 *  -----------------------------------
 *  We just pass info requests along and service them on reply from the lower
 *  level.
 */
STATIC int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	return (QR_PASSFLOW);
      emsgsize:
	return lmi_error_ack(q, sl, LMI_INFO_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 *  We do not peform attaches at this level.  That is the responsibility of
 *  the SDT driver.  After minor checking, we pass these requests along.
 */
STATIC int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sl->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (sl->i_state != LMI_UNATTACHED)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + 2)
		goto badppa;
	sl->i_state = LMI_ATTACH_PENDING;
	return (QR_PASSFLOW);
      badppa:
	ptrace(("%s: PROTO: bad ppa (too short)\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_ATTACH_REQ, LMI_BADPPA, EMSGSIZE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_ATTACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: PROTO: primitive not support for style\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	/* 
	   wait for stream to become usable */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_ATTACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 *  We do not perform detaches.  SDT does.  We do some simple checks and pass
 *  the primitive along.
 */
STATIC int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sl->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (sl->i_state != LMI_DISABLED)
		goto outstate;
	sl->i_state = LMI_DETACH_PENDING;
	return (QR_PASSALONG);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_DETACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: PROTO: primitive not support for style\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	/* 
	   wait for stream to become usable */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_DETACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 *  We must allow SDT a crack at enabling before we fully enable.  We commit
 *  the enable when the LMI_ENABLE_CON is returned from below.
 */
STATIC int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sl->i_state != LMI_DISABLED)
		goto outstate;
	sl->i_state = LMI_ENABLE_PENDING;
	return (QR_PASSALONG);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_ENABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	/* 
	   wait for stream to become usable */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_ENABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 *  We must allow SDT a crack at disable before we fully disable.  We commit
 *  the enable when the LMI_DISABLE_CON is returned from below.
 */
STATIC int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl->i_state = LMI_DISABLE_PENDING;
	return (QR_PASSALONG);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_DISABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	/* 
	   wait for stream to become available */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_DISABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	return (QR_PASSALONG);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sl, LMI_OPTMGMT_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL User -> SL Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
sl_send_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_pdu(q, sl, mp, 0);
      outstate:
	swerr();
	return (-EPROTO);
}

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC int
sl_pdu_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_pdu(q, sl, mp->b_cont, p->sl_mp)) != QR_ABSORBED)
		return (err);
	return (QR_TRIMMED);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC int
sl_emergency_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_emergency(q, sl);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC int
sl_emergency_ceases_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_emergency_ceases(q, sl);
	return (QR_DONE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
STATIC int
sl_start_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_start(q, sl);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
STATIC int
sl_stop_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_stop(q, sl);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC int
sl_retrieve_bsnt_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_retrieve_bsnt(q, sl);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC int
sl_retrieval_request_and_fsnc_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_retrieval_request_and_fsnc(q, sl, p->sl_fsnc);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC int
sl_resume_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_resume(q, sl);
	return (QR_DONE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC int
sl_clear_buffers_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_clear_buffers(q, sl);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC int
sl_clear_rtb_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_clear_rtb(q, sl);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC int
sl_local_processor_outage_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_local_processor_outage(q, sl);
	return (QR_DONE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC int
sl_congestion_discard_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_congestion_discard(q, sl);
	return (QR_DONE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
STATIC int
sl_congestion_accept_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_congestion_accept(q, sl);
	return (QR_DONE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
STATIC int
sl_no_congestion_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_no_congestion(q, sl);
	return (QR_DONE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
STATIC int
sl_power_on_req(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_power_on_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_power_on(q, sl);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

#if 0
/*
 *  SL_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
sl_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  SL_NOTIFY_REQ
 *  -----------------------------------
 */
STATIC int
sl_notify_req(queue_t *q, mblk_t *mp)
{
}
#endif

/*
 *  ------------------------------------------------------------------------
 *
 *  SDT-Provider -> SDT-User Primitives
 *
 *  ------------------------------------------------------------------------
 */

/*
 *  LMI_INFO_ACK:
 *  -----------------------------------
 *  There are two situations in which we get such a reply: first is when we
 *  are initially pushed as a module over an SDT (in state LMI_UNUSABLE);
 *  second is whenever we need to service an LMI_INFO_REQ.
 */
STATIC int
sdt_info_ack(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	{
		int hlen = (sl->option.popt & SS7_POPT_XSN) ? 6 : 3;
		ulong state = sl->i_state;
		sl->i_state = p->lmi_state;
		sl->i_style = p->lmi_ppa_style;
		switch (state) {
		case LMI_UNUSABLE:
			if (sl->i_state == LMI_UNUSABLE)
				goto merror;
			return (QR_DONE);
		case LMI_UNATTACHED:
		case LMI_ATTACH_PENDING:
		case LMI_DETACH_PENDING:
		case LMI_DISABLED:
		case LMI_ENABLE_PENDING:
		case LMI_DISABLE_PENDING:
		case LMI_ENABLED:
			p->lmi_version = sl->i_version;
			p->lmi_state = sl->i_state;
			p->lmi_max_sdu = p->lmi_max_sdu > hlen ? p->lmi_max_sdu - hlen : 0;
			p->lmi_min_sdu = p->lmi_min_sdu > hlen ? p->lmi_min_sdu - hlen : 0;
			p->lmi_header_len += hlen;
			break;
		default:
			goto outstate;
		}
	}
	printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, sl));
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      merror:
	swerr();
	return m_error(q, sl, -EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_OK_ACK:
 *  -----------------------------------
 */
STATIC int
sdt_ok_ack(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (sl->i_state) {
	case LMI_UNUSABLE:
		goto discard;
	case LMI_UNATTACHED:
	case LMI_DISABLED:
	case LMI_ENABLE_PENDING:
	case LMI_DISABLE_PENDING:
	case LMI_ENABLED:
		goto outstate;
	case LMI_ATTACH_PENDING:
		sl->i_state = LMI_DISABLED;
		break;
	case LMI_DETACH_PENDING:
		sl->i_state = LMI_UNATTACHED;
		break;
	default:
		goto discard;
	}
	printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, sl));
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
 *  LMI_ERROR_ACK:
 *  -----------------------------------
 */
STATIC int
sdt_error_ack(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (sl->i_state) {
	case LMI_UNUSABLE:
		goto merror;
	case LMI_UNATTACHED:
	case LMI_DISABLED:
	case LMI_ENABLED:
		break;
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
	default:
		goto discard;
	}
	printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, sl));
	return (QR_PASSFLOW);
      discard:
	swerr();
	return (-EFAULT);
      merror:
	swerr();
	return m_error(q, sl, -EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_ENABLE_CON:
 *  -----------------------------------
 */
STATIC int
sdt_enable_con(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (sl->i_state) {
	case LMI_UNUSABLE:
		goto discard;
	case LMI_UNATTACHED:
	case LMI_ATTACH_PENDING:
	case LMI_DETACH_PENDING:
	case LMI_DISABLED:
	case LMI_DISABLE_PENDING:
	case LMI_ENABLED:
		goto outstate;
	case LMI_ENABLE_PENDING:
		sl->i_state = LMI_ENABLED;
		break;
	default:
		goto discard;
	}
	printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, sl));
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
 *  LMI_DISABLE_CON:
 *  -----------------------------------
 */
STATIC int
sdt_disable_con(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (sl->i_state) {
	case LMI_UNUSABLE:
		goto discard;
	case LMI_UNATTACHED:
	case LMI_ATTACH_PENDING:
	case LMI_DETACH_PENDING:
	case LMI_DISABLED:
	case LMI_ENABLE_PENDING:
	case LMI_ENABLED:
		goto outstate;
	case LMI_DISABLE_PENDING:
		sl->i_state = LMI_DISABLED;
		break;
	default:
		goto discard;
	}
	printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, sl));
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
 *  LMI_OPTMGMT_ACK:
 *  -----------------------------------
 */
STATIC int
sdt_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto discard;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, sl));
	return (QR_PASSFLOW);
      outstate:
	swerr();;
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_ERROR_IND:
 *  -----------------------------------
 */
STATIC int
sdt_error_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto discard;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, sl));
	return (QR_PASSFLOW);
      outstate:
	swerr();;
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_STATS_IND:
 *  -----------------------------------
 */
STATIC int
sdt_stats_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto discard;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	printd(("%s: %p: <- LMI_STATS_IND\n", MOD_NAME, sl));
	return (QR_PASSFLOW);
      outstate:
	swerr();;
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_EVENT_IND:
 *  -----------------------------------
 */
STATIC int
sdt_event_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto discard;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, sl));
	return (QR_PASSFLOW);
      outstate:
	swerr();;
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  M_DATA:
 *  -----------------------------------
 */
STATIC int
sl_recv_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	return sl_rc_signal_unit(q, sl, mp);
}

/*
 *  SDT_RC_SIGNAL_UNIT_IND:
 *  -----------------------------------
 */
STATIC int
sdt_rc_signal_unit_ind(queue_t *q, mblk_t *mp)
{
	int err;
	if ((err = sl_recv_data(q, mp->b_cont)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
}

/*
 *  SDT_RC_CONGESTION_ACCEPT_IND:
 *  -----------------------------------
 */
STATIC int
sdt_rc_congestion_accept_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_rc_congestion_accept(q, sl);
	return (QR_DONE);
}

/*
 *  SDT_RC_CONGESTION_DISCARD_IND:
 *  -----------------------------------
 */
STATIC int
sdt_rc_congestion_discard_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_rc_congestion_discard(q, sl);
	return (QR_DONE);
}

/*
 *  SDT_RC_NO_CONGESTION_IND:
 *  -----------------------------------
 */
STATIC int
sdt_rc_no_congestion_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_rc_no_congestion(q, sl);
	return (QR_DONE);
}

/*
 *  SDT_IAC_CORRECT_SU_IND:
 *  -----------------------------------
 */
STATIC int
sdt_iac_correct_su_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	return sl_iac_correct_su(q, sl);
}

/*
 *  SDT_IAC_ABORT_PROVING_IND:
 *  -----------------------------------
 */
STATIC int
sdt_iac_abort_proving_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	return sl_iac_abort_proving(q, sl);
}

/*
 *  SDT_LSC_LINK_FAILURE_IND:
 *  -----------------------------------
 */
STATIC int
sdt_lsc_link_failure_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	return sl_lsc_link_failure(q, sl, SL_FAIL_SUERM_EIM);
}

/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND:
 *  -----------------------------------
 */
STATIC int
sdt_txc_transmission_request_ind(queue_t *q, mblk_t *mp)
{
	sl_tx_wakeup(q);
	return (QR_DONE);
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
#if 0
STATIC int
lmi_test_config(struct sl *sl, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_commit_config(struct sl *sl, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
#endif
STATIC int
lmi_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = QR_PASSALONG;
		psw_t flags;
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
lmi_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = QR_PASSALONG;
		psw_t flags;
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
lmi_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			arg->version = sl->i_version;
			arg->style = sl->i_style;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->i_version = arg->version;
			sl->i_style = arg->style;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			arg->state = sl->i_state;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->i_state = LMI_UNUSABLE;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = QR_PASSALONG;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->statsp;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = QR_PASSALONG;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->statsp = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
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
	return (-EOPNOTSUPP);
}
STATIC int
sl_commit_config(struct sl *sl, sl_config_t * arg)
{
	return (-EOPNOTSUPP);
}
#endif
STATIC int
sl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
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
sl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
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
sl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->statem;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->statem = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->statsp;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->statsp = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->stats;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			bzero(&sl->stats, sizeof(sl->stats));
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->notify;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->notify.events |= arg->events;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		struct sl *sl = SL_PRIV(q);
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->notify.events &= ~(arg->events);
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
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
	case LMI_IOC_MAGIC:
	{
		if (count < size || sl->i_state == LMI_UNATTACHED) {
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
			ptrace(("%s: ERROR: Unsupported SL ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SL_IOC_MAGIC:
	{
		if (count < size || sl->i_state == LMI_UNATTACHED) {
			ret = (-EINVAL);
			break;
		}
		switch (nr) {
		case _IOC_NR(SL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sl_iocgoptions(q, mp);
			break;
		case _IOC_NR(SL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sl_iocsoptions(q, mp);
			break;
		case _IOC_NR(SL_IOCGCONFIG):	/* sl_config_t */
			ret = sl_iocgconfig(q, mp);
			break;
		case _IOC_NR(SL_IOCSCONFIG):	/* sl_config_t */
			ret = sl_iocsconfig(q, mp);
			break;
		case _IOC_NR(SL_IOCTCONFIG):	/* sl_config_t */
			ret = sl_ioctconfig(q, mp);
			break;
		case _IOC_NR(SL_IOCCCONFIG):	/* sl_config_t */
			ret = sl_ioccconfig(q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATEM):	/* sl_statem_t */
			ret = sl_iocgstatem(q, mp);
			break;
		case _IOC_NR(SL_IOCCMRESET):	/* sl_statem_t */
			ret = sl_ioccmreset(q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATSP):	/* lmi_sta_t */
			ret = sl_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SL_IOCSSTATSP):	/* lmi_sta_t */
			ret = sl_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATS):	/* sl_stats_t */
			ret = sl_iocgstats(q, mp);
			break;
		case _IOC_NR(SL_IOCCSTATS):	/* sl_stats_t */
			ret = sl_ioccstats(q, mp);
			break;
		case _IOC_NR(SL_IOCGNOTIFY):	/* sl_notify_t */
			ret = sl_iocgnotify(q, mp);
			break;
		case _IOC_NR(SL_IOCSNOTIFY):	/* sl_notify_t */
			ret = sl_iocsnotify(q, mp);
			break;
		case _IOC_NR(SL_IOCCNOTIFY):	/* sl_notify_t */
			ret = sl_ioccnotify(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SL ioctl %d\n", MOD_NAME, nr));
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
/*
 *  Primitives from User to SL.
 *  -----------------------------------
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
		if ((rtn = sl_pdu_req(q, mp)) < 0)
			sl->i_state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", MOD_NAME, sl));
		rtn = lmi_info_req(q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", MOD_NAME, sl));
		rtn = lmi_attach_req(q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", MOD_NAME, sl));
		rtn = lmi_detach_req(q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", MOD_NAME, sl));
		rtn = lmi_enable_req(q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", MOD_NAME, sl));
		rtn = lmi_disable_req(q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", MOD_NAME, sl));
		rtn = lmi_optmgmt_req(q, mp);
		break;
	case SL_PDU_REQ:
		printd(("%s: %p: -> SL_PDU_REQ\n", MOD_NAME, sl));
		rtn = sl_pdu_req(q, mp);
		break;
	case SL_EMERGENCY_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_REQ\n", MOD_NAME, sl));
		rtn = sl_emergency_req(q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_CEASES_REQ\n", MOD_NAME, sl));
		rtn = sl_emergency_ceases_req(q, mp);
		break;
	case SL_START_REQ:
		printd(("%s: %p: -> SL_START_REQ\n", MOD_NAME, sl));
		rtn = sl_start_req(q, mp);
		break;
	case SL_STOP_REQ:
		printd(("%s: %p: -> SL_STOP_REQ\n", MOD_NAME, sl));
		rtn = sl_stop_req(q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		printd(("%s: %p: -> SL_RETRIEVE_BSNT_REQ\n", MOD_NAME, sl));
		rtn = sl_retrieve_bsnt_req(q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		printd(("%s: %p: -> SL_RETRIEVAL_REQUEST_AND_FSNC\n", MOD_NAME, sl));
		rtn = sl_retrieval_request_and_fsnc_req(q, mp);
		break;
	case SL_RESUME_REQ:
		printd(("%s: %p: -> SL_RESUME_REQ\n", MOD_NAME, sl));
		rtn = sl_resume_req(q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		printd(("%s: %p: -> SL_CLEAR_BUFFERS_REQ\n", MOD_NAME, sl));
		rtn = sl_clear_buffers_req(q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		printd(("%s: %p: -> SL_CLEAR_RTB_REQ\n", MOD_NAME, sl));
		rtn = sl_clear_rtb_req(q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		printd(("%s: %p: -> SL_LOCAL_PROCESSOR_OUTAGE_REQ\n", MOD_NAME, sl));
		rtn = sl_local_processor_outage_req(q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		printd(("%s: %p: -> SL_CONGESTION_DISCARD_REQ\n", MOD_NAME, sl));
		rtn = sl_congestion_discard_req(q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		printd(("%s: %p: -> SL_CONGESTION_ACCEPT_REQ\n", MOD_NAME, sl));
		rtn = sl_congestion_accept_req(q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		printd(("%s: %p: -> SL_NO_CONGESTION_REQ\n", MOD_NAME, sl));
		rtn = sl_no_congestion_req(q, mp);
		break;
	case SL_POWER_ON_REQ:
		printd(("%s: %p: -> SL_POWER_ON_REQ\n", MOD_NAME, sl));
		rtn = sl_power_on_req(q, mp);
		break;
	default:
		swerr();
		/* 
		   discard anything we don't recognize */
		rtn = (-EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		sl->i_state = oldstate;
	return (rtn);
}

/*
 *  Primitives from SDT to SL.
 *  -----------------------------------
 */
STATIC int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sl *sl = SL_PRIV(q);
	ulong oldstate = sl->i_state;
	/* 
	   Fast Path */
	if ((prim = *((ulong *) mp->b_rptr)) == SDT_RC_SIGNAL_UNIT_IND) {
		printd(("%s: %p: SDT_RC_SIGNAL_UNIT_IND [%x/%x:%d] <-\n", MOD_NAME, sl,
			mp->b_cont->b_rptr[0], mp->b_cont->b_rptr[1], msgdsize(mp->b_cont)));
		if ((rtn = sdt_rc_signal_unit_ind(q, mp)) < 0)
			sl->i_state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case LMI_INFO_ACK:
		printd(("%s: %p: LMI_INFO_ACK <-\n", MOD_NAME, sl));
		rtn = sdt_info_ack(q, mp);
		break;
	case LMI_OK_ACK:
		printd(("%s: %p: LMI_OK_ACK <-\n", MOD_NAME, sl));
		rtn = sdt_ok_ack(q, mp);
		break;
	case LMI_ERROR_ACK:
		printd(("%s: %p: LMI_ERROR_ACK <-\n", MOD_NAME, sl));
		rtn = sdt_error_ack(q, mp);
		break;
	case LMI_ENABLE_CON:
		printd(("%s: %p: LMI_ENABLE_CON <-\n", MOD_NAME, sl));
		rtn = sdt_enable_con(q, mp);
		break;
	case LMI_DISABLE_CON:
		printd(("%s: %p: LMI_DISABLE_CON <-\n", MOD_NAME, sl));
		rtn = sdt_disable_con(q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		printd(("%s: %p: LMI_OPTMGMT_ACK <-\n", MOD_NAME, sl));
		rtn = sdt_optmgmt_ack(q, mp);
		break;
	case LMI_ERROR_IND:
		printd(("%s: %p: LMI_ERROR_IND <-\n", MOD_NAME, sl));
		rtn = sdt_error_ind(q, mp);
		break;
	case LMI_STATS_IND:
		printd(("%s: %p: LMI_STATS_IND <-\n", MOD_NAME, sl));
		rtn = sdt_stats_ind(q, mp);
		break;
	case LMI_EVENT_IND:
		printd(("%s: %p: LMI_EVENT_IND <-\n", MOD_NAME, sl));
		rtn = sdt_event_ind(q, mp);
		break;
	case SDT_RC_SIGNAL_UNIT_IND:
		printd(("%s: %p: SDT_RC_SIGNAL_UNIT_IND [%d] <-\n", MOD_NAME, sl,
			msgdsize(mp->b_cont)));
		rtn = sdt_rc_signal_unit_ind(q, mp);
		break;
	case SDT_RC_CONGESTION_ACCEPT_IND:
		printd(("%s: %p: SDT_RC_CONGESTION_ACCEPT_IND <-\n", MOD_NAME, sl));
		rtn = sdt_rc_congestion_accept_ind(q, mp);
		break;
	case SDT_RC_CONGESTION_DISCARD_IND:
		printd(("%s: %p: SDT_RC_CONGESTION_DISCARD_IND <-\n", MOD_NAME, sl));
		rtn = sdt_rc_congestion_discard_ind(q, mp);
		break;
	case SDT_RC_NO_CONGESTION_IND:
		printd(("%s: %p: SDT_RC_NO_CONGESTION_IND <-\n", MOD_NAME, sl));
		rtn = sdt_rc_no_congestion_ind(q, mp);
		break;
	case SDT_IAC_CORRECT_SU_IND:
		printd(("%s: %p: SDT_IAC_CORRECT_SU_IND <-\n", MOD_NAME, sl));
		rtn = sdt_iac_correct_su_ind(q, mp);
		break;
	case SDT_IAC_ABORT_PROVING_IND:
		printd(("%s: %p: SDT_IAC_ABORT_PROVING_IND <-\n", MOD_NAME, sl));
		rtn = sdt_iac_abort_proving_ind(q, mp);
		break;
	case SDT_LSC_LINK_FAILURE_IND:
		printd(("%s: %p: SDT_LSC_LINK_FAILURE_IND <-\n", MOD_NAME, sl));
		rtn = sdt_lsc_link_failure_ind(q, mp);
		break;
	case SDT_TXC_TRANSMISSION_REQUEST_IND:
		printd(("%s: %p: SDT_TXC_TRANSMISSION_REQUEST_IND <-\n", MOD_NAME, sl));
		rtn = sdt_txc_transmission_request_ind(q, mp);
		break;
	default:
		swerr();
		/* 
		   discard anything we don't recognize */
		rtn = (-EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		sl->i_state = oldstate;
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
	(void) sl;
	/* 
	   data from above */
	printd(("%s: %p: -> M_DATA [%d]\n", MOD_NAME, sl, msgdsize(mp)));
	return sl_send_data(q, mp);
}
STATIC int
sl_r_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	(void) sl;
	/* 
	   data from below */
	printd(("%s: %p: M_DATA [%d] <-\n", MOD_NAME, sl, msgdsize(mp)));
	return sl_recv_data(q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE streamscall int
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
STATIC INLINE streamscall int
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
#if 0
	case M_IOCACK:
		return sl_r_iocack(q, mp);
	case M_IOCNAK:
		return sl_r_iocnak(q, mp);
#endif
	}
	return (QR_PASSALONG);
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
STATIC streamscall int
sl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
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
		struct sl *sl;
		/* 
		   test for multiple push */
		for (sl = sl_opens; sl; sl = sl->next) {
			if (sl->u.dev.cmajor == cmajor && sl->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(sl_alloc_priv(q, &sl_opens, devp, crp))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		/* 
		   generate immediate information request */
		if ((err = sdt_info_req(q, sl)) < 0) {
			sl_free_priv(q);
			MOD_DEC_USE_COUNT;
			return (-err);
		}
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return EIO;
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
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
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *sl_priv_cachep = NULL;
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
		printd(("%s: initialized module private structure cace\n", MOD_NAME));
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
		printd(("%s: allocated module private structure\n", MOD_NAME));
		bzero(sl, sizeof(*sl));
		sl_get(sl);	/* first get */
		sl->u.dev.cmajor = getmajor(*devp);
		sl->u.dev.cminor = getminor(*devp);
		sl->cred = *crp;
		(sl->oq = RD(q))->q_ptr = sl_get(sl);
		(sl->iq = WR(q))->q_ptr = sl_get(sl);
		spin_lock_init(&sl->qlock);	/* "sl-queue-lock" */
		sl->o_prim = &sl_r_prim;
		sl->i_prim = &sl_w_prim;
		sl->o_wakeup = &sl_rx_wakeup;
		sl->i_wakeup = &sl_tx_wakeup;
		sl->i_state = LMI_UNUSABLE;
		sl->i_style = LMI_STYLE1;
		sl->i_version = 1;
		spin_lock_init(&sl->lock);	/* "sl-priv-lock" */
		if ((sl->next = *slp))
			sl->next->prev = &sl->next;
		sl->prev = slp;
		*slp = sl_get(sl);
		printd(("%s: linked module private structure\n", MOD_NAME));
		bufq_init(&sl->rb);
		bufq_init(&sl->tb);
		bufq_init(&sl->rtb);
		/* 
		   configuration defaults */
		sl->option = lmi_default;
		sl->config = sl_default;
		printd(("%s: setting module private structure defaults\n", MOD_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", MOD_NAME));
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
		if ((*sl->prev = sl->next))
			sl->next->prev = sl->prev;
		sl->next = NULL;
		sl->prev = &sl->next;
		sl_put(sl);
		sl->oq->q_ptr = NULL;
		flushq(sl->oq, FLUSHALL);
		sl->oq = NULL;
		sl_put(sl);
		sl->iq->q_ptr = NULL;
		flushq(sl->iq, FLUSHALL);
		sl->iq = NULL;
		sl_put(sl);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	sl_put(sl);		/* final put */
}
STATIC struct sl *
sl_get(struct sl *sl)
{
	atomic_inc(&sl->refcnt);
	return (sl);
}
STATIC void
sl_put(struct sl *sl)
{
	if (atomic_dec_and_test(&sl->refcnt)) {
		kmem_cache_free(sl_priv_cachep, sl);
		printd(("%s: %p: freed sl private structure\n", MOD_NAME, sl));
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
MODULE_PARM_DESC(modid, "Module ID for the SL module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &slinfo,
	.f_flag = D_MP,
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
	if ((err = lis_register_strmod(&slinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&slinfo);
		return (err);
	}
	return (0);
}

STATIC int
sl_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&slinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
slinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
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
slterminate(void)
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
module_init(slinit);
module_exit(slterminate);

#endif				/* LINUX */
