/*****************************************************************************

 @(#) $RCSfile: sdt_sctp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:51:11 $

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

 Last Modified $Date: 2007/03/25 00:51:11 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdt_sctp.c,v $
 Revision 0.9.2.2  2007/03/25 00:51:11  brian
 - synchronization updates

 Revision 0.9.2.1  2006/10/17 11:55:47  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.14  2006/03/07 01:12:24  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: sdt_sctp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:51:11 $"

static char const ident[] =
    "$RCSfile: sdt_sctp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:51:11 $";

#include <sys/os7/compat.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/devi.h>
#include <ss7/devi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

#define SDT_SCTP_DESCRIP	"SS7/SCTP SIGNALLING DATA LINK (SDT) STREAMS MODULE."
#define SDT_SCTP_REVISION	"OpenSS7 $RCSfile: sdt_sctp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:51:11 $"
#define SDT_SCTP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SDT_SCTP_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SDT_SCTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDT_SCTP_LICENSE	"GPL"
#define SDT_SCTP_BANNER		SDT_SCTP_DESCRIP	"\n" \
				SDT_SCTP_REVISION	"\n" \
				SDT_SCTP_COPYRIGHT	"\n" \
				SDT_SCTP_DEVICE		"\n" \
				SDT_SCTP_CONTACT	"\n"
#define SDT_SCTP_SPLASH		SDT_SCTP_DEVICE		" - " \
				SDT_SCTP_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SDT_SCTP_CONTACT);
MODULE_DESCRIPTION(SDT_SCTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDT_SCTP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDT_SCTP_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdt_sctp");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SDT_SCTP_MOD_ID		CONFIG_STREAMS_SDT_SCTP_MODID
#define SDT_SCTP_MOD_NAME	CONFIG_STREAMS_SDT_SCTP_NAME
#endif

// #define SDT_RX_COMPRESSION
// #define SDT_TX_COMPRESSION

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID	    SDT_SCTP_MOD_ID
#define MOD_NAME    SDT_SCTP_MOD_NAME
#ifdef MODULE
#define MOD_BANNER  SDT_SCTP_BANNER
#else				/* MODULE */
#define MOD_BANNER  SDT_SCTP_SPLASH
#endif				/* MODULE */

STATIC struct module_info sdt_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted *//* FIXME */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted *//* FIXME */
	.mi_hiwat = 1 << 15,		/* Hi water mark *//* FIXME */
	.mi_lowat = 1 << 10,		/* Lo water mark *//* FIXME */
};

STATIC int streamscall sdt_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int streamscall sdt_close(queue_t *, int, cred_t *);

STATIC int streamscall sdt_rput(queue_t *, mblk_t *);
STATIC int streamscall sdt_rsrv(queue_t *);

STATIC struct qinit sdt_rinit = {
	.qi_putp = sdt_rput,		/* Read put (msg from below) */
	.qi_srvp = sdt_rsrv,		/* Read queue service */
	.qi_qopen = sdt_open,		/* Each open */
	.qi_qclose = sdt_close,		/* Last close */
	.qi_minfo = &sdt_minfo,		/* Information */
};

STATIC int streamscall sdt_wput(queue_t *, mblk_t *);
STATIC int streamscall sdt_wsrv(queue_t *);

STATIC struct qinit sdt_winit = {
	.qi_putp = sdt_wput,		/* Write put (msg from above) */
	.qi_srvp = sdt_wsrv,		/* Write queue service */
	.qi_minfo = &sdt_minfo,		/* Information */
};

MODULE_STATIC struct streamtab sdt_sctpinfo = {
	.st_rdinit = &sdt_rinit,	/* Upper read queue */
	.st_wrinit = &sdt_winit,	/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  SDT Private Structure
 *
 *  =========================================================================
 */
typedef struct sdt {
	STR_DECLARATION (struct sdt);	/* stream declaration */
	ulong token;			/* my bind token */
	mblk_t *rx_buf;			/* RX compression buffer */
	size_t rx_count;		/* RX compression count */
	mblk_t *tx_buf;			/* TX compression buffer */
	size_t tx_count;		/* TX compression count */
	uint timer_tick;		/* tick timer */
	lmi_option_t lmi_conf;		/* protocol and variant options */
	sdt_config_t sdt_conf;		/* SDT configuration options */
	dev_device_t dev_conf;		/* DEV configuration options */
} sdt_t;

#define SDT_PRIV(__q) ((sdt_t *)(__q)->q_ptr)

#define SDT_LSSU_SIO		0
#define SDT_LSSU_SIN		1
#define SDT_LSSU_SIE		2
#define SDT_LSSU_SIOS		3
#define SDT_LSSU_SIPO		4
#define SDT_LSSU_SIB		5
#define SDT_LSS2_SIO		0 + 8
#define SDT_LSS2_SIN		1 + 8
#define SDT_LSS2_SIE		2 + 8
#define SDT_LSS2_SIOS		3 + 8
#define SDT_LSS2_SIPO		4 + 8
#define SDT_LSS2_SIB		5 + 8
#define SDT_FISU		16
#define SDT_MSU			17

#define SDT_FLAG_RX_ENABLED	0x01
#define SDT_FLAG_TX_ENABLED	0x02

#define SDT_EVENT_TX_WAKEUP	1

/*
 *  =========================================================================
 *
 *  SDT Provider (SDT) -> SDT User Primitives
 *
 *  =========================================================================
 */
/*
 *  LMI_INFO_ACK
 *  ---------------------------------------------
 */
STATIC int
lmi_info_ack(sdt_t * sp)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	ensure(sp, return (-EFAULT));
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sp->state;
		p->lmi_max_sdu = -1;
		p->lmi_min_sdu = 0;
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE1;
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_OK_ACK
 *  ---------------------------------------------
 */
STATIC int
lmi_ok_ack(sdt_t * sp, long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;
	ensure(sp, return (-EFAULT));
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sp->state) {
		case LMI_ATTACH_PENDING:
			sp->state = LMI_DISABLED;
			break;
		case LMI_DETACH_PENDING:
			sp->state = LMI_UNATTACHED;
			break;
			/* 
			   default is don't change state */
		}
		p->lmi_state = sp->state;
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_ERROR_ACK
 *  ---------------------------------------------
 */
STATIC int
lmi_error_ack(sdt_t * sp, long prim, long err)
{
	mblk_t *mp;
	lmi_error_ack_t *p;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	}
	ensure(sp, return (-EFAULT));
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_error_primitive = prim;
		switch (sp->state) {
		case LMI_ATTACH_PENDING:
			sp->state = LMI_UNATTACHED;
			break;
		case LMI_DETACH_PENDING:
			sp->state = LMI_DISABLED;
			break;
		case LMI_ENABLE_PENDING:
			sp->state = LMI_DISABLED;
			break;
		case LMI_DISABLE_PENDING:
			sp->state = LMI_ENABLED;
			break;
			/* 
			 *  Default is not to change state.
			 */
		}
		p->lmi_state = sp->state;
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  ---------------------------------------------
 */
STATIC int
lmi_enable_con(sdt_t * sp)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	ensure(sp, return (-EFAULT));
	ensure(sp->state == LMI_ENABLE_PENDING, return (-EFAULT));
	if (canputnext(sp->iq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_ENABLE_CON;
			p->lmi_state = sp->state = LMI_ENABLED;
			putnext(sp->iq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  LMI_DISABLE_CON
 *  ---------------------------------------------
 */
STATIC int
lmi_disable_con(sdt_t * sp)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	ensure(sp, return (-EFAULT));
	ensure(sp->state == LMI_DISABLE_PENDING, return (-EFAULT));
	if (canputnext(sp->iq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_DISABLE_CON;
			p->lmi_state = sp->state = LMI_DISABLED;
			putnext(sp->iq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

#if 0
/*
 *  LMI_OPTMGMT_ACK
 *  ---------------------------------------------
 */
STATIC int
lmi_optmgmt_ack(sdt_t * sp, ulong flags, void *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;
	ensure(sp, return (-EFAULT));
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_ERROR_IND
 *  ---------------------------------------------
 */
#if 0
STATIC int
lmi_error_ind(sdt_t * sp, long err)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	ensure(sp, return (-EFAULT));
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_state = sp->state;
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_STATS_IND
 *  ---------------------------------------------
 */
#if 0
STATIC int
lmi_stats_ind(sdt_t * sp, ulong interval, ulong timestamp)
{
	mblk_t *mp;
	lmi_stats_ind_t *p;
	ensure(sp, return (-EFAULT));
	if (canputnext(sp->iq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = timestamp;
			putnext(sp->iq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}
#endif
/*
 *  LMI_EVENT_IND
 *  ---------------------------------------------
 */
#if 0
STATIC int
lmi_event_ind(sdt_t * sp, ulong oid, ulong severity, ulong timestamp)
{
	mblk_t *mp;
	lmi_event_ind_t *p;
	ensure(sp, return (-EFAULT));
	if (canputnext(sp->iq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = timestamp;
			p->lmi_severity = severity;
			putnext(sp->iq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}
#endif
/*
 *  SDT_DAEDR_RECEIVED_BITS_IND
 *  ---------------------------------------------
 */
STATIC int
sdt_read(sdt_t * sp, mblk_t *dp)
{
	ensure(sp, return (-EFAULT));
	if (canputnext(sp->iq)) {
		putnext(sp->iq, dp);
		return (0);
	}
	seldom();
	return (-EBUSY);
}

#if 0
STATIC int
sdt_daedr_received_bits_ind(sdt_t * sp, ulong count, mblk_t *dp)
{
	mblk_t *mp;
	sdt_daedr_received_bits_ind_t *p;
	ensure(sp, return (-EFAULT));
	if (canputnext(sp->iq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sdt_primitive = SDT_DAEDR_RECEIVED_BITS_IND;
			p->sdt_count = count;
			putnext(sp->iq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}
#endif
/*
 *  SDT_DAEDR_CORRECT_SU_IND
 *  ---------------------------------------------
 */
#if 0
STATIC int
sdt_daedr_correct_su_ind(sdt_t * sp, ulong count)
{
	mblk_t *mp;
	sdt_daedr_correct_su_ind_t *p;
	ensure(sp, return (-EFAULT));
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_DAEDR_CORRECT_SU_IND;
		p->sdt_count = count;
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
#endif
/*
 *  SDT_DAEDR_SU_IN_ERROR_IND
 *  ---------------------------------------------
 */
#if 0
STATIC int
sdt_daedr_su_in_error_ind(sdt_t * sp)
{
	mblk_t *mp;
	sdt_daedr_su_in_error_ind_t *p;
	ensure(sp, return (-EFAULT));
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_DAEDR_SU_IN_ERROR_IND;
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
#endif
/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND
 *  ---------------------------------------------
 */
STATIC int
sdt_daedt_transmission_request_ind(sdt_t * sp)
{
	mblk_t *mp;
	sdt_txc_transmission_request_ind_t *p;
	ensure(sp, return (-EFAULT));
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  NPI User (SDT) -> NPI Provider (SCTP) Primitives
 *
 *  =========================================================================
 */
/*
 *  N_DATA_REQ
 *  ---------------------------------------------
 */
STATIC int
n_data_req(sdt_t * sp, ulong flags, void *qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_data_req_t *p;
	ensure(sp, return (-EFAULT));
	ensure(dp, return (-EFAULT));
	if (canputnext(sp->oq)) {
		if ((mp = allocb(sizeof(*p) + qos_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			putnext(sp->oq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_EXDATA_REQ
 *  ---------------------------------------------
 */
#if 0
STATIC int
n_exdata_req(sdt_t * sp, void *qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_req_t *p;
	ensure(sp, return (-EFAULT));
	ensure(dp, return (-EFAULT));
	if (bcanputnext(sp->oq, 1)) {
		if ((mp = allocb(sizeof(*p) + qos_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = N_EXDATA_REQ;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			putnext(sp->oq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}
#endif

/*
 *  =========================================================================
 *
 *  SDT PDU Message Functions
 *
 *  =========================================================================
 */

#define SDT_PPI	    10

STATIC int
sdt_write(sdt_t * sp, mblk_t *mp)
{
	int err;
	uint xsn = sp->lmi_conf.popt & SS7_POPT_XSN;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, SDT_PPI, 0, 0, 0, 0 };

	if ((err = n_data_req(sp, 0, &qos, sizeof(qos), mp)))
		return (err);
	if ((!xsn && mlen > 5) || (xsn && mlen > 8))
		sdt_daedt_transmission_request_ind(sp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  SDT Provider (SDT) <- SDT User Primitives
 *
 *  =========================================================================
 */
/*
 *  LMI_INFO_REQ
 *  ---------------------------------------------
 */
STATIC int
lmi_info_req(sdt_t * sp, mblk_t *mp)
{
	(void) mp;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	return lmi_info_ack(sp);
}

/*
 *  LMI_ATTACH_REQ
 *  ---------------------------------------------
 */
STATIC int
lmi_attach_req(sdt_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_attach_req_t *p = (lmi_attach_req_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	do {
		if (mlen >= sizeof(*p)) {
			if (sp->state == LMI_UNATTACHED) {
				sp->state = LMI_ATTACH_PENDING;

				lmi_error_ack(sp, LMI_ATTACH_REQ, LMI_NOTSUPP);

			}
			seldom();
			err = LMI_OUTSTATE;
			break;	/* would place interface out of state */
		}
		seldom();
		err = LMI_PROTOSHORT;
		break;		/* M_PROTO block too short */
	} while (0);
	seldom();
	return lmi_error_ack(sp, LMI_ATTACH_REQ, err);
}

/*
 *  LMI_DETACH_REQ
 *  ---------------------------------------------
 */
STATIC int
lmi_detach_req(sdt_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_detach_req_t *p = (lmi_detach_req_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	do {
		if (mlen >= sizeof(*p)) {
			if (sp->state == LMI_DISABLED) {
				sp->state = LMI_DETACH_PENDING;

				lmi_error_ack(sp, LMI_DETACH_REQ, LMI_NOTSUPP);

			}
			seldom();
			err = LMI_OUTSTATE;
			break;	/* would place interface out of state */
		}
		seldom();
		err = LMI_PROTOSHORT;
		break;		/* M_PROTO block too short */
	} while (0);
	seldom();
	return lmi_error_ack(sp, LMI_DETACH_REQ, err);
}

/*
 *  LMI_ENABLE_REQ
 *  ---------------------------------------------
 */
STATIC int
lmi_enable_req(sdt_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_enable_req_t *p = (lmi_enable_req_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	do {
		if (mlen >= sizeof(*p)) {
			if (sp->state == LMI_DISABLED) {
				sp->state = LMI_ENABLE_PENDING;

				return lmi_enable_con(sp);

			}
			seldom();
			err = LMI_OUTSTATE;
			break;	/* would place interface out of state */
		}
		seldom();
		err = LMI_PROTOSHORT;
		break;		/* M_PROTO block too short */
	} while (0);
	seldom();
	return lmi_error_ack(sp, LMI_ENABLE_REQ, err);
}

/*
 *  LMI_DISABLE_REQ
 *  ---------------------------------------------
 */
STATIC int
lmi_disable_req(sdt_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_disable_req_t *p = (lmi_disable_req_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	do {
		if (mlen >= sizeof(*p)) {
			if (sp->state == LMI_ENABLED || sp->state == LMI_ENABLE_PENDING) {

				sp->state = LMI_DISABLE_PENDING;

				freemsg(xchg(&sp->rx_buf, NULL));
				sp->rx_count = 0;
				freemsg(xchg(&sp->tx_buf, NULL));
				sp->tx_count = 0;

				return lmi_disable_con(sp);

			}
			seldom();
			err = LMI_OUTSTATE;
			break;	/* would place interface out of state */
		}
		seldom();
		err = LMI_PROTOSHORT;
		break;		/* M_PROTO block too short */
	} while (0);
	seldom();
	return lmi_error_ack(sp, LMI_DISABLE_REQ, err);
}

/*
 *  LMI_OPTMGMT_REQ
 *  ---------------------------------------------
 */
STATIC int
lmi_optmgmt_req(sdt_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	lmi_optmgmt_req_t *p = (lmi_optmgmt_req_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	do {
		if (mlen >= sizeof(*p)) {

			lmi_error_ack(sp, LMI_OPTMGMT_REQ, LMI_NOTSUPP);

		}
		seldom();
		err = LMI_PROTOSHORT;
		break;		/* M_PROTO block too short */
	} while (0);
	seldom();
	return lmi_error_ack(sp, LMI_OPTMGMT_REQ, err);
}

/*
 *  SDT_DAEDT_TRAMSMISSION_REQ
 *  ---------------------------------------------
 */
#ifndef abs
#define abs(x) ((x)<0?-(x):(x))
#endif
STATIC int
m_error_reply(sdt_t * sp, int err)
{
	mblk_t *mp;
	ensure(sp, return (-EFAULT));
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
	case 1:
	case 2:
		never();
		return (err);
	}
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = abs(err);
		*(mp->b_wptr)++ = abs(err);
		putnext(sp->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
STATIC int
sdt_daedt_xmit_req(sdt_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *dp;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sdt_daedt_transmission_req_t *p = (sdt_daedt_transmission_req_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	do {
		if ((dp = mp->b_cont)) {
			if (mlen >= sizeof(*p)) {
				if (sp->state != LMI_DISABLED) {
					if (sp->state == LMI_ENABLED) {
						if (sp->flags & SDT_FLAG_TX_ENABLED) {

							if ((err = sdt_write(sp, dp)))
								break;
							mp->b_cont = NULL;	/* absorbed data
										   portion */
							return (0);

						}
						rare();
						return (0);	/* ignore if not enabled */
					}
					seldom();
					err = LMI_OUTSTATE;
					break;	/* would place interface out of state */
				}
				seldom();
				return (0);
				break;	/* ingore data in DISABLED state */
			}
			seldom();
			err = LMI_PROTOSHORT;
			break;	/* M_PROTO block too short */
		}
		rare();
		err = -EFAULT;
		break;		/* no M_DATA blocks */
	} while (0);
	rare();
	return m_error_reply(sp, err);	/* XXX */
}

/*
 *  SDT_DAEDT_START_REQ
 *  ---------------------------------------------
 */
STATIC int
sdt_daedt_start_req(sdt_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sdt_daedt_start_req_t *p = (sdt_daedt_start_req_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	do {
		if (mlen >= sizeof(*p)) {
			if (sp->state == LMI_ENABLED) {

				/* 
				   enable the transmitter section */
				sp->flags |= SDT_FLAG_TX_ENABLED;
				return (0);

			}
			seldom();
			err = LMI_OUTSTATE;
			break;	/* would place interface out of state */
		}
		seldom();
		err = LMI_PROTOSHORT;
		break;		/* M_PROTO block too short */
	} while (0);
	rare();
	return m_error_reply(sp, err);	/* XXX */
}

/*
 *  SDT_DAEDR_START_REQ
 *  ---------------------------------------------
 */
STATIC int
sdt_daedr_start_req(sdt_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	sdt_daedr_start_req_t *p = (sdt_daedr_start_req_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	do {
		if (mlen >= sizeof(*p)) {
			if (sp->state == LMI_ENABLED) {

				/* 
				   enable the receiver section */
				sp->flags |= SDT_FLAG_RX_ENABLED;
				return (0);

			}
			seldom();
			err = LMI_OUTSTATE;
			break;	/* would place interface out of state */
		}
		seldom();
		err = LMI_PROTOSHORT;
		break;		/* M_PROTO block too short */
	} while (0);
	rare();
	return m_error_reply(sp, err);	/* XXX */
}

/*
 *  =========================================================================
 *
 *  NPI User (SDT) <- NPI Provider (SCTP) Primitives
 *
 *  =========================================================================
 */
/*
 *  N_CONN_IND
 *  ---------------------------------------------
 */
STATIC int
n_conn_ind(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_CONN_CON
 *  ---------------------------------------------
 */
STATIC int
n_conn_con(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_DISCON_IND
 *  ---------------------------------------------
 */
STATIC int
n_discon_ind(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_DATA_IND
 *  ---------------------------------------------
 */
STATIC int
n_data_ind(sdt_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *dp;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	// N_data_ind_t *p = (N_data_ind_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	ensure(mlen >= sizeof(N_data_ind_t), return (-EFAULT));
	if (mlen <= sp->sdt_conf.m) {
		if ((dp = mp->b_cont)) {
			if (sp->state == LMI_ENABLED) {
				if (sp->flags & SDT_FLAG_RX_ENABLED) {
					if (canputnext(sp->iq)) {
						if ((err = sdt_read(sp, dp)))
							return (err);
						mp->b_cont = NULL;	/* absorbed data portion */
						return (0);
					}
					seldom();
					return (-EBUSY);	/* flow controlled */
				}
				seldom();
				return (0);	/* ignore unless enabled */
			}
			rare();
			return (-EPROTO);	/* ignore data in other states */
		}
		rare();
		return (-EFAULT);
	}
	rare();
	return (-EMSGSIZE);
}

/*
 *  N_EXDATA_IND
 *  ---------------------------------------------
 */
STATIC int
n_exdata_ind(sdt_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *dp;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	// N_exdata_ind_t *p = (N_exdata_ind_t *) mp->b_rptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	ensure(mlen >= sizeof(N_exdata_ind_t), return (-EFAULT));
	if (mlen <= sp->sdt_conf.m) {
		if ((dp = mp->b_cont)) {
			if (sp->state == LMI_ENABLED) {
				if (sp->flags & SDT_FLAG_RX_ENABLED) {
					if (bcanputnext(sp->iq, 1)) {
						if ((err = sdt_read(sp, dp)))
							return (err);
						mp->b_cont = NULL;	/* absorbed data portion */
						return (0);
					}
					seldom();
					return (-EBUSY);	/* flow controlled */
				}
				seldom();
				return (0);	/* ignore unless enabled */
			}
			rare();
			return (-EPROTO);	/* ignore data in other states */
		}
		rare();
		return (-EFAULT);
	}
	rare();
	return (-EMSGSIZE);
}

/*
 *  N_DATACK_IND
 *  ---------------------------------------------
 */
STATIC int
n_datack_ind(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_INFO_ACK
 *  ---------------------------------------------
 */
STATIC int
n_info_ack(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_BIND_ACK
 *  ---------------------------------------------
 */
STATIC int
n_bind_ack(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_OK_ACK
 *  ---------------------------------------------
 */
STATIC int
n_ok_ack(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_ERROR_ACK
 *  ---------------------------------------------
 */
STATIC int
n_error_ack(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_RESET_IND
 *  ---------------------------------------------
 */
STATIC int
n_reset_ind(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  N_RESET_CON
 *  ---------------------------------------------
 */
STATIC int
n_reset_con(sdt_t * sp, mblk_t *mp)
{
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	(void) sp;
	(void) mp;
	rare();
	return (0);
}

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *  I have only implemented enough of these to get the Q.781 test programs
 *  running.
 */
/*
 *  SDT_IOCGOPTIONS
 *  ---------------------------------------------
 */
STATIC int
sdt_iocgoptions(queue_t *q, int cmd, void *arg)
{
	sdt_t *sp;
	lmi_option_t *opt = (lmi_option_t *) arg;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(arg, return (-EFAULT));
	*opt = sp->lmi_conf;
	return (0);
}

/*
 *  SDT_IOCSOPTIONS
 *  ---------------------------------------------
 */
STATIC int
sdt_iocsoptions(queue_t *q, int cmd, void *arg)
{
	sdt_t *sp;
	lmi_option_t *opt = (lmi_option_t *) arg;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(arg, return (-EFAULT));
	switch (opt->pvar) {
	case SS7_PVAR_ITUT_88:
	case SS7_PVAR_ITUT_93:
	case SS7_PVAR_ITUT_96:
	case SS7_PVAR_ITUT_00:
	case SS7_PVAR_ANSI_88:
	case SS7_PVAR_ANSI_92:
	case SS7_PVAR_ANSI_96:
	case SS7_PVAR_ANSI_00:
	case SS7_PVAR_ETSI_88:
	case SS7_PVAR_ETSI_93:
	case SS7_PVAR_ETSI_96:
	case SS7_PVAR_ETSI_00:
	case SS7_PVAR_JTTC_94:
		break;
	default:
		rare();
		return (-EINVAL);
	}
	if (opt->popt & ~(SS7_POPT_MPLEV | SS7_POPT_HSL | SS7_POPT_XSN)) {
		rare();
		return (-EINVAL);
	}
	sp->lmi_conf = *opt;
	return (0);
}

/*
 *  SDT_IOCGCONFIG
 *  ---------------------------------------------
 */
STATIC int
sdt_iocgconfig(queue_t *q, int cmd, void *arg)
{
	sdt_t *sp;
	sdt_config_t *cnf = (sdt_config_t *) arg;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(arg, return (-EFAULT));
	*cnf = sp->sdt_conf;
	return (0);
}

/*
 *  SDT_IOCSCONFIG
 *  ---------------------------------------------
 */
STATIC int
sdt_iocsconfig(queue_t *q, int cmd, void *arg)
{
	sdt_t *sp;
	sdt_config_t *cnf = (sdt_config_t *) arg;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(arg, return (-EFAULT));
	sp->sdt_conf = *cnf;
	return (0);
}

/*
 *  DEV_IOCSIFCLOCK
 *  ---------------------------------------------
 */
STATIC int
dev_iocsifclock(queue_t *q, int cmd, void *arg)
{
	sdt_t *sp;
	ulong *val = (ulong *) arg;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(arg, return (-EFAULT));
	switch (*val) {
	case DEV_CLOCK_NONE:
	case DEV_CLOCK_ABR:
	case DEV_CLOCK_SHAPER:
	case DEV_CLOCK_TICK:
		sp->dev_conf.ifclock = *val;
		return (0);

	default:
	case DEV_CLOCK_INT:
	case DEV_CLOCK_EXT:
	case DEV_CLOCK_LOOP:
	case DEV_CLOCK_MASTER:
	case DEV_CLOCK_SLAVE:
	case DEV_CLOCK_DPLL:
		break;
	}
	return (-EINVAL);
}

/*
 *  DEV_IOCGIFCLOCK
 *  ---------------------------------------------
 */
STATIC int
dev_iocgifclock(queue_t *q, int cmd, void *arg)
{
	sdt_t *sp;
	ulong *val = (ulong *) arg;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(arg, return (-EFAULT));
	*val = sp->dev_conf.ifclock;
	return (0);
}

/*
 *  DEV_IOCCDISCTX
 *  ---------------------------------------------
 *  Disconnect the transmit path.
 */
STATIC int
dev_ioccdisctx(queue_t *q, int cmd, void *arg)
{
	ensure(q, return (-EFAULT));
	ensure(arg, return (-EFAULT));
	return (-EOPNOTSUPP);
}

/*
 *  DEV_IOCCCONNTX
 *  ---------------------------------------------
 *  Reconnect the transmit path.
 */
STATIC int
dev_ioccconntx(queue_t *q, int cmd, void *arg)
{
	ensure(q, return (-EFAULT));
	ensure(arg, return (-EFAULT));
	return (-EOPNOTSUPP);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
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
	sdt_t *sp;
	ulong oldstate;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	oldstate = sp->state;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	switch ((prim = *((ulong *) mp->b_rptr))) {
	case LMI_INFO_REQ:
		rtn = lmi_info_req(sp, mp);
		break;
	case LMI_ATTACH_REQ:
		rtn = lmi_attach_req(sp, mp);
		break;
	case LMI_DETACH_REQ:
		rtn = lmi_detach_req(sp, mp);
		break;
	case LMI_ENABLE_REQ:
		rtn = lmi_enable_req(sp, mp);
		break;
	case LMI_DISABLE_REQ:
		rtn = lmi_disable_req(sp, mp);
		break;
	case LMI_OPTMGMT_REQ:
		rtn = lmi_optmgmt_req(sp, mp);
		break;
	case SDT_DAEDT_TRANSMISSION_REQ:
		rtn = sdt_daedt_xmit_req(sp, mp);
		break;
	case SDT_DAEDT_START_REQ:
		rtn = sdt_daedt_start_req(sp, mp);
		break;
	case SDT_DAEDR_START_REQ:
		rtn = sdt_daedr_start_req(sp, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		seldom();
		sp->state = oldstate;
	}
	return (rtn);
}
STATIC int
sdt_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sdt_t *sp;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	switch ((prim = *((ulong *) mp->b_rptr))) {
	case N_CONN_IND:
		rtn = n_conn_ind(sp, mp);
		break;
	case N_CONN_RES:
		rtn = n_conn_con(sp, mp);
		break;
	case N_DISCON_IND:
		rtn = n_discon_ind(sp, mp);
		break;
	case N_DATA_IND:
		rtn = n_data_ind(sp, mp);
		break;
	case N_EXDATA_IND:
		rtn = n_exdata_ind(sp, mp);
		break;
	case N_DATACK_IND:
		rtn = n_datack_ind(sp, mp);
		break;
	case N_INFO_ACK:
		rtn = n_info_ack(sp, mp);
		break;
	case N_BIND_ACK:
		rtn = n_bind_ack(sp, mp);
		break;
	case N_OK_ACK:
		rtn = n_ok_ack(sp, mp);
		break;
	case N_ERROR_ACK:
		rtn = n_error_ack(sp, mp);
		break;
	case N_RESET_IND:
		rtn = n_reset_ind(sp, mp);
		break;
	case N_RESET_CON:
		rtn = n_reset_con(sp, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		seldom();
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
sdt_w_data(queue_t *q, mblk_t *mp)
{
	int err;
	sdt_t *sp;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if ((err = sdt_write(sp, mp)))
		return err;
	return (1);		/* absorbed mblk */
}
STATIC int
sdt_r_data(queue_t *q, mblk_t *mp)
{
	int err;
	sdt_t *sp;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if ((err = sdt_read(sp, mp)))
		return err;
	return (1);		/* absorbed mblk */
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_CTL Handling (Events)
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int
sdt_r_ctl(queue_t *q, mblk_t *mp)
{
	sdt_t *sp;
	ensure(q, return (-EFAULT));
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	switch (*mp->b_rptr) {
	case SDT_EVENT_TX_WAKEUP:
		sdt_tx_wakeup(sp);
		return (0);
	}
	return (-EOPNOTSUPP);
}
#endif
/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = -EINVAL;
	int type = _IOC_TYPE(cmd);
	int nr = _IOC_NR(cmd);
	int size = _IOC_SIZE(cmd);
	(void) nr;
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	switch (type) {
	case __SID:
		switch (cmd) {
		default:
			ptrace(("Unknown IOCTL %x\n", cmd));
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
		ret = -EOPNOTSUPP;
		break;
	case SDT_IOC_MAGIC:
		if (count >= size) {
			switch (cmd) {
			case SDT_IOCGOPTIONS:
				ret = sdt_iocgoptions(q, cmd, arg);
				break;
			case SDT_IOCSOPTIONS:
				ret = sdt_iocsoptions(q, cmd, arg);
				break;
			case SDT_IOCGCONFIG:
				ret = sdt_iocgconfig(q, cmd, arg);
				break;
			case SDT_IOCSCONFIG:
				ret = sdt_iocsconfig(q, cmd, arg);
				break;
			default:
			case SDT_IOCTCONFIG:
			case SDT_IOCCCONFIG:
			case SDT_IOCGSTATEM:
			case SDT_IOCCMRESET:
			case SDT_IOCGSTATSP:
			case SDT_IOCSSTATSP:
			case SDT_IOCGSTATS:
			case SDT_IOCCSTATS:
			case SDT_IOCGNOTIFY:
			case SDT_IOCSNOTIFY:
			case SDT_IOCCNOTIFY:
				ret = -EOPNOTSUPP;
				break;
			}
		} else
			ret = -EINVAL;
		break;
	case DEV_IOC_MAGIC:
		if (count >= size) {
			switch (cmd) {
			case DEV_IOCSIFCLOCK:
				ret = dev_iocsifclock(q, cmd, arg);
				break;
			case DEV_IOCGIFCLOCK:
				ret = dev_iocgifclock(q, cmd, arg);
				break;
			case DEV_IOCCDISCTX:
				ret = dev_ioccdisctx(q, cmd, arg);
				break;
			case DEV_IOCCCONNTX:
				ret = dev_ioccconntx(q, cmd, arg);
				break;
			default:
			case DEV_IOCCIFRESET:
			case DEV_IOCGIFFLAGS:
			case DEV_IOCSIFFLAGS:
			case DEV_IOCGIFTYPE:
			case DEV_IOCSIFTYPE:
			case DEV_IOCGGRPTYPE:
			case DEV_IOCSGRPTYPE:
			case DEV_IOCGIFMODE:
			case DEV_IOCSIFMODE:
			case DEV_IOCGIFRATE:
			case DEV_IOCSIFRATE:
			case DEV_IOCGIFCODING:
			case DEV_IOCSIFCODING:
			case DEV_IOCGIFLEADS:
			case DEV_IOCSIFLEADS:
			case DEV_IOCCIFLEADS:
				ret = -EOPNOTSUPP;
				break;
			}
		} else
			ret = -EINVAL;
		break;
	default:
		if (q->q_next) {
			putnext(q, mp);
			return (1);
		}
		ret = -EOPNOTSUPP;
		break;
	}
	if (ret >= 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		seldom();
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (1);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_r_error(queue_t *q, mblk_t *mp)
{
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	putnext(q, mp);
	return (1);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag)
{
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	putnext(q, mp);
	return (1);
}
STATIC int
sdt_w_flush(queue_t *q, mblk_t *mp)
{
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	return sdt_m_flush(q, mp, FLUSHW);
}
STATIC int
sdt_r_flush(queue_t *q, mblk_t *mp)
{
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	return sdt_m_flush(q, mp, FLUSHR);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_m_other(queue_t *q, mblk_t *mp)
{
	rare();
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (1);
	} else
		return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */

/*
 *  SDT Write Put and Service
 */
STATIC streamscall int
sdt_wput(queue_t *q, mblk_t *mp)
{
	int rtn;
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type < QPCTL && q->q_count) {
		seldom();
		putq(q, mp);
		return (0);
	}
	if (ss7_trylockq(q)) {
		switch (mp->b_datap->db_type) {
		case M_DATA:
			rtn = sdt_w_data(q, mp);
			break;
		case M_PROTO:
		case M_PCPROTO:
			rtn = sdt_w_proto(q, mp);
			break;
		case M_FLUSH:
			rtn = sdt_w_flush(q, mp);
			break;
		case M_IOCTL:
			rtn = sdt_w_ioctl(q, mp);
			break;
		default:
			rtn = sdt_m_other(q, mp);
			break;
		}
		switch (rtn) {
		case 0:
			assert(mp->b_datap->db_ref);
			freemsg(mp);
		case 1:
			break;
		case 2:
			assert(mp->b_datap->db_ref);
			freeb(mp);
			break;
		case -ENOBUFS:
			/* 
			   should set up bufcall */
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
			seldom();
			putq(q, mp);
			ss7_unlockq(q);
			return (0);
		default:
			rare();
			ptrace(("Error = %d\n", rtn));
			assert(mp->b_datap->db_ref);
			freemsg(mp);
			ss7_unlockq(q);
			return (rtn);
		}
	} else {
		rare();
		putq(q, mp);
	}
	return (0);
}

STATIC streamscall int
sdt_wsrv(queue_t *q)
{
	int rtn;
	mblk_t *mp;
	ensure(q, return (-EFAULT));
	if (ss7_trylockq(q)) {
		while ((mp = getq(q))) {
			switch (mp->b_datap->db_type) {
			case M_DATA:
				rtn = sdt_w_data(q, mp);
				break;
			case M_PROTO:
			case M_PCPROTO:
				rtn = sdt_w_proto(q, mp);
				break;
			case M_FLUSH:
				rtn = sdt_w_flush(q, mp);
				break;
			case M_IOCTL:
				rtn = sdt_w_ioctl(q, mp);
				break;
			default:
				rtn = sdt_m_other(q, mp);
				break;
			}
			switch (rtn) {
			case 0:
				assert(mp->b_datap->db_ref);
				freemsg(mp);
			case 1:
				continue;
			case 2:
				assert(mp->b_datap->db_ref);
				freeb(mp);
				continue;
			case -ENOBUFS:
				/* 
				   should set up bufcall */
			case -EBUSY:
			case -EAGAIN:
			case -ENOMEM:
				seldom();
				if (mp->b_datap->db_type < QPCTL) {
					putbq(q, mp);
					ss7_unlockq(q);
					return (0);
				}
				__ptrace(("This should never happen!\n"));
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					putq(q, mp);
					ss7_unlockq(q);
					return (0);
				}
			default:
				rare();
				ptrace(("Error = %d\n", rtn));
				assert(mp->b_datap->db_ref);
				freemsg(mp);
				continue;
			}
		}
	} else {
		rare();
		qenable(q);
	}
	return (0);
}

/*
 *  SCTP Read Put and Service
 */
STATIC streamscall int
sdt_rput(queue_t *q, mblk_t *mp)
{
	int rtn;
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type < QPCTL && q->q_count) {
		seldom();
		putq(q, mp);
		return (0);
	}
	if (ss7_trylockq(q)) {
		switch (mp->b_datap->db_type) {
		case M_DATA:
			rtn = sdt_r_data(q, mp);
			break;
		case M_PROTO:
		case M_PCPROTO:
			rtn = sdt_r_proto(q, mp);
			break;
			// case M_CTL: rtn = sdt_r_ctl (q, mp); break;
		case M_ERROR:
			rtn = sdt_r_error(q, mp);
			break;
		case M_FLUSH:
			rtn = sdt_r_flush(q, mp);
			break;
		default:
			rtn = sdt_m_other(q, mp);
			break;
		}
		switch (rtn) {
		case 0:
			assert(mp->b_datap->db_ref);
			freemsg(mp);
		case 1:
			break;
		case 2:
			assert(mp->b_datap->db_ref);
			freeb(mp);
			break;
		case -ENOBUFS:
			/* 
			   should set up bufcall */
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
			seldom();
			putq(q, mp);
			ss7_unlockq(q);
			return (0);
		default:
			rare();
			ptrace(("Error = %d\n", rtn));
			assert(mp->b_datap->db_ref);
			freemsg(mp);
			ss7_unlockq(q);
			return (rtn);
		}
	} else {
		rare();
		putq(q, mp);
	}
	return (0);
}

STATIC streamscall int
sdt_rsrv(queue_t *q)
{
	int rtn;
	mblk_t *mp;
	ensure(q, return (-EFAULT));
	if (ss7_trylockq(q)) {
		while ((mp = getq(q))) {
			switch (mp->b_datap->db_type) {
			case M_DATA:
				rtn = sdt_r_data(q, mp);
				break;
			case M_PROTO:
			case M_PCPROTO:
				rtn = sdt_r_proto(q, mp);
				break;
				// case M_CTL: rtn = sdt_r_ctl (q, mp); break;
			case M_ERROR:
				rtn = sdt_r_error(q, mp);
				break;
			case M_FLUSH:
				rtn = sdt_r_flush(q, mp);
				break;
			default:
				rtn = sdt_m_other(q, mp);
				break;
			}
			switch (rtn) {
			case 0:
				assert(mp->b_datap->db_ref);
				freemsg(mp);
			case 1:
				continue;
			case 2:
				assert(mp->b_datap->db_ref);
				freeb(mp);
				continue;
			case -ENOBUFS:
				/* 
				   should set up bufcall */
			case -EBUSY:
			case -EAGAIN:
			case -ENOMEM:
				seldom();
				if (mp->b_datap->db_type < QPCTL) {
					putbq(q, mp);
					ss7_unlockq(q);
					return (0);
				}
				__ptrace(("This should never happen!\n"));
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					putq(q, mp);
					ss7_unlockq(q);
					return (0);
				}
			default:
				rare();
				ptrace(("Error = %d\n", rtn));
				assert(mp->b_datap->db_ref);
				freemsg(mp);
				continue;
			}
		}
	} else {
		rare();
		qenable(q);
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
kmem_cache_t *sdt_cachep = NULL;

STATIC int
sdt_init_caches(void)
{
	if (!sdt_cachep
	    && !(sdt_cachep =
		 kmem_cache_create("sdt_cachep", sizeof(sdt_t), 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
		cmn_err(CE_PANIC, "%s: Cannot alloc sdt_cachep.", __FUNCTION__);
	return (0);
}
STATIC int
sdt_term_caches(void)
{
	if (sdt_cachep)
		if (kmem_cache_destroy(sdt_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sdt_cachep", __FUNCTION__);
			return (-EBUSY);
		}
	return (0);
}

STATIC sdt_t *
sdt_alloc_priv(queue_t *q)
{
	sdt_t *sp;
	ensure(q, return (NULL));

	if ((sp = kmem_cache_alloc(sdt_cachep, SLAB_ATOMIC))) {
		bzero(sp, sizeof(*sp));
		RD(q)->q_ptr = WR(q)->q_ptr = sp;
		sp->iq = RD(q);
		sp->oq = WR(q);
		sp->state = LMI_DISABLED;
		spin_lock_init(&sp->lock);	/* "ss7sctp-private" */
	}
	return (sp);
}
STATIC void
sdt_free_priv(queue_t *q)
{
	sdt_t *sp;
	ensure(q, return);
	sp = (sdt_t *) q->q_ptr;
	ensure(sp, return);
	untimeout(xchg(&sp->timer_tick, 0));
	freemsg(xchg(&sp->rx_buf, NULL));
	freemsg(xchg(&sp->tx_buf, NULL));
	kmem_cache_free(sdt_cachep, sp);
	return;
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC streamscall int
sdt_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;		/* for now */
	if (q->q_ptr != NULL)	/* already open */
		return (0);
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		// fixme(("Check module we are being pushed over.\n"));
		if (!(sdt_alloc_priv(q)))
			return ENOMEM;
		return (0);
	}
	return EIO;
}
STATIC streamscall int
sdt_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;

	sdt_free_priv(q);
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
MODULE_PARM_DESC(modid, "Module ID for the SDT module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sdt_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sdt_sctpinfo,
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
	if ((err = lis_register_strmod(&sdt_sctpinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&sdt_sctpinfo);
		return (err);
	}
	return (0);
}

STATIC int
sdt_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&sdt_sctpinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sdt_sctpinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = sdt_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = sdt_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		sdt_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sdt_sctpterminate(void)
{
	int err;
	if ((err = sdt_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sdt_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sdt_sctpinit);
module_exit(sdt_sctpterminate);

#endif				/* LINUX */
