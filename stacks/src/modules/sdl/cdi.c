/*****************************************************************************

 @(#) $RCSfile: cdi.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:13 $

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

 Last Modified $Date: 2004/01/17 08:22:13 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: cdi.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:13 $"

static char const ident[] = "$RCSfile: cdi.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:13 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>

#include <sys/cdi.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/devi.h>
#include <ss7/devi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

#include "lock.h"
#include "debug.h"
#include "bufq.h"

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */
extern struct module_info cdi_minfo = {
	0,				/* Module ID number */
	"cdi-sdl",			/* Module name */
	0,				/* Min packet size accepted */
	INFPSZ,				/* Max packet size accepted */
	1,				/* Hi water mark */
	0				/* Lo water mark */
};

STATIC int cdi_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int cdi_close(queue_t *, int, cred_t *);

STATIC int cdi_rput(queue_t *, mblk_t *);

STATIC struct qinit cdi_rinit = {
	cdi_rput,			/* Read put (msg from below) */
	NULL,				/* Read queue service */
	cdi_open,			/* Each open */
	cdi_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&cdi_minfo,			/* Information */
	NULL				/* Statistics */
};

STATIC int cdi_wput(queue_t *, mblk_t *);

STATIC struct qinit cdi_winit = {
	cdi_wput,			/* Write put (msg from above) */
	NULL,				/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&cdi_minfo,			/* Information */
	NULL				/* Statistics */
};

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6

/*
 *  =========================================================================
 *
 *  CDI-SDL Private Structure
 *
 *  =========================================================================
 */

typedef struct cdi {
	struct cdi *next;		/* list of structures */
	queue_t *rq;			/* rd queue */
	queue_t *wq;			/* wr queue */
#ifndef SOLARIS
	lis_spin_lock_t lock;		/* queue lock */
	struct task_struct *user;	/* user of this queue */
#endif
	uint i_state;			/* interface state */
	uint rbid;			/* rd bufcall id */
	uint wbid;			/* wr bufcall id */
} cdi_t;

#define CDI_PRIV(__q) = ((__q)->q_ptr)

/*
 *  ========================================================================
 *
 *  Buffer Allocation
 *
 *  ========================================================================
 */
STATIC INLINE mblk_t *
cd_allocb(mblk_t *bp, int len, int prio, int *rtn)
{
	mblk_t *mp;
	if (bp) {
		if (bp->db_datap->db_lim - bp->b_rptr > len) {
			bp->db_datap->db_type = M_DATA;
			bp->b_wptr = bp->b_rptr;
			*rtn = QR_ABSORBED;
			return (bp);
		}
		if (bp->db_datap->db_lim - bp->db_datap->db_base > len) {
			bp->db_datap->db_type = M_DATA;
			bp->b_wptr = bp->b_rptr = bp->b_datap->db_base;
			*rtn = QR_ABSORBED;
			return (bp);
		}
		if ((mp = allocb(len, prio))) {
			mp->b_cont = bp->b_cont;
			*rtn = QR_TRIMMED;
			return (mp);
		}
		*rtn = -ENOBUFS;
		return (NULL);
	}
	*rtn = -EFAULT;
	return (NULL);

}

/*
 *  =========================================================================
 *
 *  CDI-Provider --> CDI-User Primitives
 *
 *  =========================================================================
 */
/*
 *  CD_INFO_ACK:
 */
STATIC INLINE int
cd_info_ack(queue_t *q, mblk_t *mp, ulong max_sdu, ulong min_sdu, ulong ppa_style)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	cd_info_ack_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cd_primitive = CD_INFO_ACK;
		p->cd_state = cd->i_state;
		p->cd_max_sdu = max_sdu;
		p->cd_min_sdu = min_sdu;
		p->cd_class = CD_HDLC;
		p->cd_duplex = CD_FULLDUPLEX;
		p->cd_output_style = CD_UNACKEDOUTPUT;
		p->cd_features = CD_CANREAD | CD_AUTOALLOW | CD_KEEPALIVE;
		p->cd_addr_length = sizeof(ulong);
		p->cd_ppa_style = ppa_style;
		putnext(q, mp);
	}
	return (err);
}

/*
 *  CD_OK_ACK:
 */
STATIC INLINE int
cd_ok_ack(queue_t *q, mblk_t *mp, int prim)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	cd_ok_ack_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cd_primitive = CD_OK_ACK;
		p->cd_state = cd->i_state;
		p->cd_correct_primitive = prim;
		putnext(q, mp);
	}
	return (err);
}

/*
 *  CD_ERROR_ACK:
 */
STATIC INLINE int
cd_error_ack(queue_t *q, mblk_t *mp, int prim, int errno, int exp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	cd_error_ack_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cd_primitive = CD_ERROR_ACK;
		p->cd_state = cd->i_state;
		p->cd_error_primitive = prim;
		p->cd_errno = errno;
		p->cd_explanation = exp;
		putnext(q, mp);
	}
	return (err);
}

/*
 *  CD_ENABLE_CON:
 */
STATIC INLINE int
cd_enable_con(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	cd_enable_con_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cd_primitive = CD_ENABLE_CON;
		p->cd_state = cd->i_state;
		putnext(q, mp);
	}
	return (err);
}

/*
 *  CD_DISABLE_CON:
 */
STATIC INLINE int
cd_disable_con(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	cd_disable_con_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cd_primitive = CD_DISABLE_CON;
		p->cd_state = cd->i_state;
		putnext(q, mp);
	}
	return (err);
}

/*
 *  CD_ERROR_IND:
 */
STATIC INLINE int
cd_error_ind(queue_t *q, mblk_t *mp, int errno, int exp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	cd_error_ind_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cd_primitive = CD_ERROR_IND;
		p->cd_state = cd->i_state;
		p->cd_errno = errno;
		p->cd_explanation = exp;
		putnext(q, mp);
	}
	return (err);
}

/*
 *  CD_UNITDATA_ACK:
 */
STATIC INLINE int
cd_unitdata_ack(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	cd_unitdata_ack_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cd_primitive = CD_UNITDATA_ACK;
		p->cd_state = cd->i_state;
		putnext(q, mp);
	}
	return (err);
}

/*
 *  CD_UNITDATA_IND:
 */
STATIC INLINE int
cd_unitdata_ind(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	cd_unitdata_ind_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cd_primitive = CD_UNITDATA_IND;
		p->cd_state = cd->i_state;
		p->cd_src_addr_length = 0;
		p->cd_src_addr_offset = 0;
		p->cd_addr_type = FIXME;
		p->cd_priority = FIXME;
		p->cd_dest_addr_length = 0;
		p->cd_dest_addr_offset = 0;
		putnext(q, mp);
	}
	return (err);
}

/*
 *  CD_BAD_FRAME_IND:
 */
/*
 *  CD_MODEM_SIG_IND:
 */

/*
 *  =========================================================================
 *
 *  SDL-User --> SDL-Provider Primitives
 *
 *  =========================================================================
 */
/*
 *  LMI_INFO_REQ
 */
STATIC INLINE int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	lmi_info_req_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;

		putnext(q, mp);
	}
	return (err);
}

/*
 *  LMI_ATTACH_REQ
 */
STATIC INLINE int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	lmi_attach_req_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;

		putnext(q, mp);
	}
	return (err);
}

/*
 *  LMI_DETACH_REQ
 */
STATIC INLINE int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	lmi_detach_req_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;

		putnext(q, mp);
	}
	return (err);
}

/*
 *  LMI_ENABLE_REQ
 */
STATIC INLINE int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	lmi_enable_req_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;

		putnext(q, mp);
	}
	return (err);
}

/*
 *  LMI_DISABLE_REQ
 */
STATIC INLINE int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	lmi_disable_req_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;

		putnext(q, mp);
	}
	return (err);
}

/*
 *  LMI_OPTMGMT_REQ
 */
STATIC INLINE int
lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	int err = 0;
	cdi_t *cd = CDI_PRIV(q);
	mblk_t *mp;
	lmi_optmgmt_req_t *p;
	if ((mp = cd_allocb(mp, sizeof(*p), BPRI_MED, &err))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;

		putnext(q, mp);
	}
	return (err);
}

/*
 *  =========================================================================
 *
 *  CDI-User --> CDI-Provider Primitives
 *
 *  =========================================================================
 */
/*
 *  CD_INFO_REQ:
 */
STATIC INLINE int
cd_info_req(queue_t *q, mblk_t *mp)
{
	return lmi_info_req(q, mp);
}

/*
 *  CD_ATTACH_REQ:
 */
STATIC INLINE int
cd_attach_req(queue_t *q, mblk_t *mp)
{
	ulong ppa = ((cd_attach_req_t *) mp->b_rptr)->cd_ppa;
	return lmi_attach_req(q, mp, ppa);
}

/*
 *  CD_DETACH_REQ:
 */
STATIC INLINE int
cd_detach_req(queue_t *q, mblk_t *mp)
{
	return lmi_detach_req(q, mp);
}

/*
 *  CD_ENABLE_REQ:
 */
STATIC INLINE int
cd_enable_req(queue_t *q, mblk_t *mp)
{
	return lmi_enable_req(q, mp);
}

/*
 *  CD_DISABLE_REQ:
 */
STATIC INLINE int
cd_disable_req(queue_t *q, mblk_t *mp)
{
	ulong disposal = ((cd_disable_req_t *) mp->b_rptr)->cd_disposal;
	return lmi_disable_req(q, mp);
}

/*
 *  CD_ALLOW_INPUT_REQ:
 */
STATIC INLINE int
cd_allow_input_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  CD_READ_REQ:
 */
STATIC INLINE int
cd_read_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  CD_UNITDATA_REQ:
 */
STATIC INLINE int
cd_unitdata_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  CD_WRITE_READ_REQ:
 */
STATIC INLINE int
cd_write_read_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  CD_HALT_INPUT_REQ:
 */
STATIC INLINE int
cd_halt_input_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  CD_ABORT_OUTPUT_REQ:
 */
STATIC INLINE int
cd_abort_output_req(queue_t *q, mblk_t *mp)
{
}

/*
 *  CD_MUX_NAME_REQ:
 */
/*
 *  CD_MODEM_SIG_REQ:
 */
/*
 *  CD_MODEM_SIG_POLL:
 */

/*
 *  =========================================================================
 *
 *  SDL-Provider --> SDL-User Primitives
 *
 *  =========================================================================
 */
/*
 *  Translate primitives.
 */
STATIC INLINE int
xlat_prim(int prim)
{
	switch (prim) {
	case LMI_INFO_REQ:
		return (CD_INFO_REQ);
	case LMI_ATTACH_REQ:
		return (CD_ATTACH_REQ);
	case LMI_DETACH_REQ:
		return (CD_DETACH_REQ);
	case LMI_ENABLE_REQ:
		return (CD_ENABLE_REQ);
	case LMI_DISABLE_REQ:
		return (CD_DISABLE_REQ);
	}
	return (0);
}

/*
 *  Translate state.
 */
STATIC INLINE int
xlat_state(int state)
{
	switch (state) {
	case LMI_UNATTACHED:
		return (CD_UNATTACHED);
	case LMI_ATTACH_PENDING:
		return (CD_UNATTACHED);
	case LMI_UNUSABLE:
		return (CD_UNUSABLE);
	case LMI_DISABLED:
		return (CD_DISABLED);
	case LMI_ENABLE_PENDING:
		return (CD_ENABLE_PENDING);
	case LMI_ENABLED:
		return (CD_ENABLED);
	case LMI_DISABLE_PENDING:
		return (CD_DISABLE_PENDING);
	case LMI_DETACH_PENDING:
		return (CD_DISABLED);
	}
	return (0);
}
STATIC INLINE int
xlat_reason(int reason)
{
}
STATIC INLINE int
xlat_errno(int errno)
{
}

/*
 * LMI_INFO_ACK
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = ((typeof(p)) mp->b_rptr);
	return cd_info_ack(q, mp, p->lmi_max_sdu, p->lmi_min_sdu, p->lmi_ppa_style);
}

/*
 * LMI_OK_ACK
 */
STATIC INLINE int
lmi_ok_ack(queue_t *q, mblk_t *mp)
{
	int err;
	cdi_t *cd = CD_PRIV(q);
	lmi_ok_ack_t *p = ((typeof(p)) mp->b_rptr);
	int oldstate = cd->i_state;
	cd->i_state = xlat_state(p->lmi_state);
	if ((err = cd_ok_ack(q, mp, xlat_prim(p->lmi_correct_primitive))) < 0)
		cd->i_state = oldstate;
	return (err);
}

/*
 * LMI_ERROR_ACK
 */
STATIC INLINE int
lmi_error_ack(queue_t *q, mblk_t *mp)
{
	int prim;
	lmi_error_ack_t *p = ((typeof(p)) mp->b_rptr);
	switch (p->lmi_error_primitive) {
	case LMI_INFO_REQ:
		prim = CD_INFO_REQ;
		break;
	case LMI_ATTACH_REQ:
		prim = CD_ATTACH_REQ;
		break;
	case LMI_DETACH_REQ:
		prim = CD_DETACH_REQ;
		break;
	case LMI_ENABLE_REQ:
		prim = CD_ENABLE_REQ;
		break;
	case LMI_DISABLE_REQ:
		prim = CD_DISABLE_REQ;
		break;
	default:
		return (QR_DONE);
	}
	switch (p->lmi_state) {
	}
	switch (p->lmi_errno) {
	}
	switch (p->lmi_reason) {
	case LMI_UNSPEC:
		break;
	case LMI_BADADDRESS:
		errno = CD_BADADDRESS;
		break;
	case LMI_BADADDRTYPE:
		errno = CD_BADADDRTYPE;
		break;
	case LMI_BADDIAL:
		errno = CD_BADDIAL;
		break;
	case LMI_BADDIALTYPE:
		errno = CD_BADDIALTYPE;
		break;
	case LMI_BADDISPOSAL:
		errno = CD_BADDISPOSAL;
		break;
	case LMI_BADFRAME:
		errno = CD_BADFRAME;
		break;
	case LMI_BADPPA:
		errno = CD_BADPPA;
		break;
	case LMI_BADPRIM:
		errno = CD_BADPRIM;
		break;
	case LMI_DISC:
		errno = CD_DISC;
		break;
	case LMI_EVENT:
		errno = CD_EVENT;
		break;
	case LMI_FATALERR:
		errno = CD_FATALERR;
		break;
	case LMI_INITFAILED:
		errno = CD_INITFAILED;
		break;
	case LMI_NOTSUPP:
		errno = CD_NOTSUPP;
		break;
	case LMI_OUTSTATE:
		errno = CD_OUTSTATE;
		break;
	case LMI_PROTOSHORT:
		errno = CD_PROTOSHORT;
		break;
	case LMI_SYSERR:
		errno = CD_SYSERR;
		break;
	case LMI_WRITEFAIL:
		errno = CD_WRITEFAIL;
		break;
	case LMI_CRCERR:
		exp = CD_CRCERR;
		break;
	case LMI_DLE_EOT:
		exp = CD_DLE_EOT;
		break;
	case LMI_FORMAT:
		exp = CD_FORMAT;
		break;
	case LMI_HDLC_ABORT:
		exp = CD_HDLC_ABORT;
		break;
	case LMI_OVERRUN:
		exp = CD_OVERRUN;
		break;
	case LMI_TOOSHORT:
		exp = CD_TOOSHORT;
		break;
	case LMI_INCOMPLETE:
		exp = CD_INCOMPLETE;
		break;
	case LMI_BUSY:
		exp = CD_BUSY;
		break;
	case LMI_NOANSWER:
		exp = CD_NOANSWER;
		break;
	case LMI_CALLREJECT:
		exp = CD_CALLREJECT;
		break;
	case LMI_HDLC_IDLE:
		exp = CD_HDLC_IDLE;
		break;
	case LMI_HDLC_NOTIDLE:
		exp = CD_HDLC_NOTIDLE;
		break;
	case LMI_QUIESCENT:
		exp = CD_QUIESCENT;
		break;
	case LMI_RESUMED:
		exp = CD_RESUMED;
		break;
	case LMI_DSRTIMEOUT:
		exp = CD_DSRTIMEOUT;
		break;
	case LMI_LAN_COLLISIONS:
		exp = CD_LAN_COLLISIONS;
		break;
	case LMI_LAN_REFUSED:
		exp = CD_LAN_REFUSED;
		break;
	case LMI_LAN_NOSTATION:
		exp = CD_LAN_NOSTATION;
		break;
	case LMI_LOSTCTS:
		exp = CD_LOSTCTS;
		break;
	case LMI_DEVERR:
		exp = CD_DEVERR;
		break;
	}
	return cd_error_ack(q, mp);
}

/*
 * LMI_ENABLE_CON
 */
STATIC INLINE int
lmi_enable_con(queue_t *q, mblk_t *mp)
{
	return cd_enable_con(q, mp);
}

/*
 * LMI_DISABLE_CON
 */
STATIC INLINE int
lmi_disable_con(queue_t *q, mblk_t *mp)
{
	return cd_disable_con(q, mp);
}

/*
 * LMI_OPTMGMT_ACK
 */
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, mblk_t *mp)
{
}

/*
 * LMI_ERROR_IND
 */
STATIC INLINE int
lmi_error_ind(queue_t *q, mblk_t *mp)
{
	return cd_error_ind(q, mp);
}

/*
 * LMI_STATS_IND
 */
STATIC INLINE int
lmi_stats_ind(queue_t *q, mblk_t *mp)
{
}

/*
 * LMI_EVENT_IND
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, mblk_t *mp)
{
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cdi_w_data(queue_t *q, mblk_t *mp)
{
	return (QR_PASSALONG);
}
STATIC INLINE int
cdi_r_data(queue_t *q, mblk_t *mp)
{
	return (QR_PASSALONG);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cdi_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	cdi_t *cd = CD_PRIV(q);
	ulong oldstate = cd->i_state;
	switch (*((ulong *) mp->b_rptr)) {
	case CD_INFO_REQ:
		rtn = cd_info_req(q, mp);
		break;
	case CD_ATTACH_REQ:
		rtn = cd_attach_req(q, mp);
		break;
	case CD_DETACH_REQ:
		rtn = cd_detach_req(q, mp);
		break;
	case CD_ENABLE_REQ:
		rtn = cd_enable_req(q, mp);
		break;
	case CD_DISABLE_REQ:
		rtn = cd_disable_req(q, mp);
		break;
	case CD_ALLOW_INPUT_REQ:
		rtn = cd_allow_input_req(q, mp);
		break;
	case CD_READ_REQ:
		rtn = cd_read_req(q, mp);
		break;
	case CD_UNITDATA_REQ:
		rtn = cd_unitdata_req(q, mp);
		break;
	case CD_WRITE_READ_REQ:
		rtn = cd_write_read_req(q, mp);
		break;
	case CD_HALT_INPUT_REQ:
		rtn = cd_halt_input_req(q, mp);
		break;
	case CD_ABORT_OUTPUT_REQ:
		rtn = cd_abort_output_req(q, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		cd->i_state = oldstate;
	}
	return (rtn);
}

STATIC INLINE int
cdi_r_prim(queue_t *q, mblk_t *mp)
{
	switch (*((ulong *) mp->b_rptr)) {
	case LMI_INFO_ACK:
		return lmi_info_ack(q, mp);
	case LMI_OK_ACK:
		return lmi_ok_ack(q, mp);
	case LMI_ERROR_ACK:
		return lmi_error_ack(q, mp);
	case LMI_ENABLE_CON:
		return lmi_enable_con(q, mp);
	case LMI_DISABLE_CON:
		return lmi_disable_con(q, mp);
	case LMI_OPTMGMT_ACK:
		return lmi_optmgmt_ack(q, mp);
	case LMI_ERROR_IND:
		return lmi_error_ind(q, mp);
	case LMI_STATS_IND:
		return lmi_stats_ind(q, mp);
	case LMI_EVENT_IND:
		return lmi_event_ind(q, mp);
	default:
		return (-EOPNOTSUPP);
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cdi_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag)
{
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
cdi_w_flush(queue_t *q, mblk_t *mp)
{
	return cdi_m_flush(q, mp, FLUSHW);
}
STATIC INLINE int
cdi_r_flush(queue_t *q, mblk_t *mp)
{
	return cdi_m_flush(q, mp, FLUSHR);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
cdi_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return cdi_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cdi_r_proto(q, mp);
	case M_FLUSH:
		return cdi_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
cdi_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return cdi_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cdi_w_proto(q, mp);
	case M_FLUSH:
		return cdi_w_flush(q, mp);
	case M_IOCTL:
		return cdi_w_ioctl(q, mp);
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
cdi_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0;
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	cdi_spinlock(q);
	switch ((rtn = proc(q, mp))) {
	case QR_DONE:
		freemsg(mp);
	case QR_ABSORBED:
		break;
	case QR_TRIMMED:
		freeb(mp);
		break;
	case QR_LOOP:
	case QR_PASSALONG:
		putnext(q, mp);
		break;
	default:
		ptrace(("ERROR: (q dropping) %d\n", rtn));
		freemsg(mp);
		break;
	}
	cdi_spinunlock(q);
	return ((int) (rtn));
}
STATIC int
cdi_rput(queue_t *q, mblk_t *mp)
{
	return (int) cdi_putq(q, mp, &cdi_r_prim);
}
STATIC int
cdi_wput(queue_t *q, mblk_t *mp)
{
	return (int) cdi_putq(q, mp, &cdi_w_prim);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC int
cdi_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;
	if (CDI_PRIV(q))	/* already open */
		return (0);
	if (sflag == MODOPEN || WR(q)->q_next) {
		if (!(cdi_alloc_priv(q)))
			return (ENOMEM);
		return (0);
	}
	return (EIO);
}
STATIC int
cdi_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	cdi_free_priv(q);
	return (0);
}
