/*****************************************************************************

 @(#) $RCSfile: sl_mux.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:58 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2004/08/21 10:14:58 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sl_mux.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:58 $"

char const ident[] =
    "$RCSfile: sl_mux.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:58 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>

#include <ss7/lmi.h>
#include <ss7/sli.h>

#include "debug.h"

#ifndef MUX_CMAJOR
#define MUX_CMAJOR 220
#endif

#define SL_DESCRIP	"SS7/IP SIGNALLING LINK (SL) STREAMS MULTIPLEXING DRIVER."
#define SL_REVISION	"LfS $RCSname$ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:58 $"
#define SL_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SL_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define SL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SL_LICENSE	"GPL"
#define SL_BANNER	SL_DESCRIP	"\n" \
			SL_REVISION	"\n" \
			SL_COPYRIGHT	"\n" \
			SL_DEVICE	"\n" \
			SL_CONTACT

MODULE_AUTHOR(SL_CONTACT);
MODULE_DESCRIPTION(SL_DESCRIP);
MODULE_SUPPORTED_DEVICE(SL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SL_LICENSE);
#endif

static struct module_info sl_minfo = {
	SL_MUX_DRV_ID,			/* id */
	SL_MUX_DRV_NAME,		/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* high water mark */
	512L				/* low water mark */
};

static int sl_open(queue_t *, dev_t *, int, int, cred_t *);
static int sl_close(queue_t *, int, cred_t *);

static int sl_u_wput(queue_t *, mblk_t *);
static int sl_l_rput(queue_t *, mblk_t *);
static int sl_u_rsrv(queue_t *q);
static int sl_l_wsrv(queue_t *q);

static struct qinit sl_rinit = {
	NULL,				/* put */
	sl_u_rsrv,			/* service */
	sl_open,			/* open */
	sl_close,			/* close */
	NULL,				/* admin */
	&sl_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit sl_winit = {
	sl_u_wput,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&sl_minfo,			/* info */
	NULL				/* stat */
};

/*
 * qinit structures (rd and wr side, lower) 
 */
static struct module_info sl_lminfo = {
	0,				/* id */
	SL_MUX_DRV_NAME "lwr",		/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* high water mark */
	512L				/* low water mark */
};

static struct qinit sl_lrinit = {
	sl_l_rput,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&sl_lminfo,			/* info */
	NULL				/* stat */
};

static struct qinit sl_lwinit = {
	NULL,				/* put */
	sl_l_wsrv,			/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&sl_lminfo,			/* info */
	NULL				/* stat */
};

static struct streamtab sl_info = {
	&sl_rinit,			/* read queue */
	&sl_winit,			/* write queue */
	&sl_lrinit,			/* mux read queue */
	&sl_lwinit			/* mux write queue */
};

static queue_t *qbot = NULL;
static queue_t *qtop = NULL;

static int sltm_timer = 0;
static int recovery_timer = 0;

static int sltm_failures = 0;
static int recovery_attempts = 0;

static int
sl_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *iocb;
	struct linkblk *lb;
	int err;
	iocb = (typeof(iocb)) mp->b_rptr;
	switch (iocb->ioc_cmd) {
	case I_LINK:
	case I_PLINK:
	{
		ptrace(("Got I_(P)LINK\n"));
		if (!mp->b_cont) {
			assure(mp->b_cont);
			err = EINVAL;
			break;
		}
		if (qbot) {
			assure(!qbot);
			err = EBUSY;
			break;
		}
		lb = (typeof(lb)) mp->b_cont->b_rptr;
		if (!(qbot = lb->l_qbot)) {
			assure(qbot);
			err = EINVAL;
			break;
		}
		MOD_INC_USE_COUNT;
		sltm_failures = 0;
		recovery_attempts = 0;
		RD(qbot)->q_ptr = &qtop;
		WR(qbot)->q_ptr = &qtop;
		err = 0;
		break;
	}
	case I_UNLINK:
	case I_PUNLINK:
	{
		ptrace(("Got I_(P)UNLINK\n"));
		if (!qbot) {
			assure(!qbot);
			err = EINVAL;
			break;
		}
		if (recovery_timer)
			untimeout(xchg(&recovery_timer, 0));
		if (sltm_timer)
			untimeout(xchg(&sltm_timer, 0));
		RD(qbot)->q_ptr = NULL;
		WR(qbot)->q_ptr = NULL;
		qbot = NULL;
		MOD_DEC_USE_COUNT;
		err = 0;
		break;
	}
	default:
		ptrace(("Got IOCTL: %d\n", iocb->ioc_cmd));
		if (qbot) {
			ptrace(("Passing along IOCTL\n"));
			putq(qbot, mp);
			return (0);
		}
		assure(0);
		ptrace(("Unsupported IOCTL\n"));
		err = EOPNOTSUPP;
		break;
	}
	if (err) {
		mp->b_datap->db_type = M_IOCNAK;
		iocb->ioc_error = err;
		iocb->ioc_rval = -1;
	} else {
		mp->b_datap->db_type = M_IOCACK;
		iocb->ioc_error = 0;
		iocb->ioc_rval = 0;
	}
	qreply(q, mp);
	return (0);
}

static void
restart_link(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	mblk_t *dp;
	recovery_timer = 0;
	if ((dp = allocb(sizeof(long), BPRI_MED))) {
		sl_start_req_t *p;
		dp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) dp->b_wptr)++;
		p->sl_primitive = SL_START_REQ;
		ptrace(("Sending SL_START_REQ\n"));
		recovery_attempts++;
		qreply(q, dp);
		return;
	}
	recovery_timer = timeout(&restart_link, (caddr_t) q, 12 * HZ);
}

static void send_sltm(caddr_t data);

static void
failed_sltm(caddr_t data)
{
	sltm_timer = 0;
	__printd(("mtp: SLTM Failed\n"));
	if (++sltm_failures >= 2) {
		queue_t *q = (queue_t *) data;
		mblk_t *dp;
		if ((dp = allocb(sizeof(long), BPRI_MED))) {
			sl_stop_req_t *p;
			dp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) dp->b_wptr)++;
			p->sl_primitive = SL_STOP_REQ;
			ptrace(("Sending SL_STOP_REQ\n"));
			recovery_timer = timeout(&restart_link, (caddr_t) q, 12 * HZ);
			qreply(q, dp);
		}
	}
	send_sltm(data);
}

static void
send_sltm(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	mblk_t *mp;
	uint dpc = 0x11d8;
	uint opc = 0x11ae;
	uint sls = 0;
	uint h0 = 1;
	uint h1 = 1;
	uint tbyte = ((jiffies >> 8) & 0x0f);
	uint tlen = jiffies & 0x0f;
	if ((mp = allocb(7 + tlen, BPRI_MED))) {
		int i;
		unsigned char *x;
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += 6;
		mp->b_wptr += 6;
		x = mp->b_wptr;
		*x++ = 0x01;	/* sio == 1 */
		*x++ = dpc;
		*x++ = ((dpc >> 8) & 0x3f) | (opc << 6);
		*x++ = (opc >> 2);
		*x++ = ((opc >> 10) & 0x0f) | (sls << 4);
		*x++ = h0 | (h1 << 4);
		*x++ = (tlen << 4) & 0xf0;
		for (i = 0; i < tlen; i++)
			*x++ = tbyte;
		mp->b_wptr = x;
		__printd(("mtp: Sent SLTM\n"));
		qreply(q, mp);
	}
	sltm_timer = timeout(&failed_sltm, (caddr_t) q, 4 * HZ);	/* T1T */
}

static int
handle_message(queue_t *q, mblk_t *mp)
{
	unsigned char *x = mp->b_rptr;
	uint sio, dpc, opc, sls, h0, h1, b;
	int i;
	__printd(("mtp: M_DATA: "));
	for (i = 0; i < mp->b_wptr - mp->b_rptr; i++)
		__printd(("%02x ", x[i] & 0x0ff));
	__printd(("\n"));
	sio = *x++;
	if ((sio & 0xf) == 1 || (sio & 0xf) == 2) {
		ptrace(("Got Test Message\n"));
		b = *x++ & 0x00ff;
		dpc = ((b << 0) & 0xff);
		b = *x++ & 0x00ff;
		dpc |= ((b << 8) & 0x3f00);
		opc = ((b >> 6) & 0x03);
		b = *x++ & 0x00ff;
		opc |= ((b << 2) & 0x3fc);
		b = *x++ & 0x00ff;
		opc |= ((b << 10) & 0x3c00);
		sls = (b >> 4) & 0x0f;
		b = *x++ & 0x00ff;
		h0 = b & 0x0f;
		h1 = b >> 4;
		if (h0 == 1 && h1 == 1) {
			mblk_t *dp;
			__printd(("mtp: Got SLTM\n"));
			if ((dp = copymsg(mp))) {
				x = dp->b_rptr;
				h1 = 2;
				x++;
				*x++ = opc;
				*x++ = ((opc >> 8) & 0x3f) | (dpc << 6);
				*x++ = (dpc >> 2);
				*x++ = ((dpc >> 10) & 0x0f) | (sls << 4);
				*x++ = h0 | (h1 << 4);
				/*
				   send back 
				 */
				__printd(("mtp: Sent SLTA\n"));
				qreply(q, dp);
				freemsg(mp);
				return (1);
			}
		} else if (h0 == 1 && h1 == 2) {
			__printd(("mtp: Got SLTA\n"));
			if (sltm_timer)
				untimeout(sltm_timer);
			sltm_failures = 0;
			sltm_timer = timeout(&send_sltm, (caddr_t) q, 2 * 60 * HZ);	/* T2T */
		}
	}
	return (0);
}

static int
sl_l_rput(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if (handle_message(q, mp))
			return (0);
		break;
	case M_PROTO:
	case M_PCPROTO:
	{
		ptrace(("Got M_PROTO = %ld\n", *(long *) mp->b_rptr));
		switch (*(long *) mp->b_rptr) {
		case SL_OUT_OF_SERVICE_IND:
		{
			__printd(("mtp: Got SL_OUT_OF_SERVICE_IND\n"));
			if (sltm_timer)
				untimeout(xchg(&sltm_timer, 0));
			if (recovery_timer)
				untimeout(xchg(&recovery_timer, 0));
			if (!recovery_attempts)
				restart_link((caddr_t) q);
			else
				recovery_timer = timeout(&restart_link, (caddr_t) q, 1 * HZ);	/* T17 
												 */
			freemsg(mp);
			return (0);
		}
		case SL_PDU_IND:
			__printd(("mtp: Got SL_PDU_IND\n"));
			if (handle_message(q, mp->b_cont)) {
				freeb(mp);
				return (0);
			}
			break;
		case SL_IN_SERVICE_IND:
			__printd(("mtp: Got SL_IN_SERVICE_IND\n"));
			recovery_attempts = 0;
			send_sltm((caddr_t) q);
			freemsg(mp);
			return (0);
		}
		break;
	}
	}
	if (qtop) {
		ptrace(("Passing along\n"));
		putq(qtop, mp);
		return (0);
	}
	ptrace(("Discarding\n"));
	freemsg(mp);
	return (0);
}

static int
sl_u_wput(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		ptrace(("Got IOCTL\n"));
		sl_ioctl(q, mp);
		return (0);
	case M_FLUSH:
		if (*mp->b_rptr & FLUSHW)
			flushq(q, FLUSHDATA);
		if (*mp->b_rptr & FLUSHR) {
			*mp->b_rptr &= ~FLUSHW;
			flushq(OTHER(q), FLUSHDATA);
			qreply(q, mp);
		} else {
			ptrace(("Discarding\n"));
			freemsg(mp);
		}
		return (0);
	}
	if (qbot) {
		ptrace(("Passing along\n"));
		putq(qbot, mp);
		return (0);
	}
	ptrace(("Discarding\n"));
	freemsg(mp);
	return (0);
}

static int
sl_u_rsrv(queue_t *q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		if (q->q_next)
			putnext(q, mp);
		else
			freemsg(mp);
	}
	return (0);
}

static int
sl_l_wsrv(queue_t *q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		if (q->q_next)
			putnext(q, mp);
		else
			freemsg(mp);
	}
	return (0);
}

static int
sl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	if (q->q_ptr)
		return (0);
	if (qtop) {
		assure(!qtop);
		return (ENXIO);
	}
	MOD_INC_USE_COUNT;
	qtop = q;
	RD(q)->q_ptr = &qbot;
	WR(q)->q_ptr = &qbot;
	return (0);
}

static int
sl_close(queue_t *q, int sflag, cred_t *crp)
{
	qtop = NULL;
	RD(q)->q_ptr = NULL;
	WR(q)->q_ptr = NULL;
	MOD_DEC_USE_COUNT;
	return (0);
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
	cmn_err(CE_NOTE, SL_BANNER);	/* console splash */
	lis_register_strdev(MUX_CMAJOR, &sl_info, 256, SL_MUX_DRV_NAME);
	return (0);
}

void
cleanup_module(void)
{
	lis_unregister_strdev(MUX_CMAJOR);
	return;
}
