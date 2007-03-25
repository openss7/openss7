/*****************************************************************************

 @(#) $RCSfile: lapb.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/03/25 06:00:12 $

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

 Last Modified $Date: 2007/03/25 06:00:12 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lapb.c,v $
 Revision 0.9.2.4  2007/03/25 06:00:12  brian
 - flush corrections

 Revision 0.9.2.3  2006/11/30 13:05:27  brian
 - checking in working copies

 Revision 0.9.2.2  2006/10/19 10:37:25  brian
 - working up drivers and modules

 Revision 0.9.2.1  2006/10/17 11:56:00  brian
 - copied files into new packages from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: lapb.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/03/25 06:00:12 $"

static char const ident[] =
    "$RCSfile: lapb.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/03/25 06:00:12 $";

/*
 *  This is a pushable STREAMS module that provides the Link Access Procedure
 *  (Balanced) for X.25.  It presents a Data Link Provider Interface (DLPI) at
 *  the upper service interface and uses a Communications Device Interface
 *  (CDI) at the lower service interface.  This module is for CDI devices and
 *  supports LAPB in software.  Some devices might support LAPB in device
 *  firmware, in which case, the device driver itself will present the DLPI
 *  interface.  The DLPI interface presented supports X.25 PLP modules.
 *
 *  This module is originally intended for use with the HDLC module, but can
 *  be used with any appropriate CDI device.
 *
 *  THis module can also present a pseudo-device driver interface to provide
 *  access to LAPB device driver implementations in the Linux kernel.  Also,
 *  CDI HDLC streams can be linked under the pseudo-device driver and then
 *  addressed by interface (multiplex) identifier from the upper multiplex.
 */

#include <sys/os8/compat.h>

#include <sys/cdi.h>		/* Lower boundary. */
#include <sys/dlpi.h>		/* Upper boundary. */

#define LAPB_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LAPB_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define LAPB_REVISION	"OpenSS7 $RCSfile: lapb.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/03/25 06:00:12 $"
#define LAPB_DEVICE	"SVR 4.2 STREAMS Link Access Procedure Balanced (LAPB)"
#define LAPB_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LAPB_LICENSE	"GPL"
#define LAPB_BANNER	LAPB_DESCRIP	"\n" \
			LAPB_COPYRIGHT	"\n" \
			LAPB_REVISION	"\n" \
			LAPB_DEVICE	"\n" \
			LAPB_CONTACT
#define LAPB_SPLASH	LAPB_DEVICE	" - " \
			LAPB_REVISION

#ifdef LINUX
MODULE_AUTHOR(LAPB_CONTACT);
MODULE_DESCRIPTION(LAPB_DESCRIP);
MODULE_SUPPORTED_DEVICE(LAPB_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LAPB_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-lapb");
MODULE_ALIAS("streams-lapbmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef LAPB_MOD_NAME
#define LAPB_MOD_NAME		"lapb"
#endif

#ifndef LAPB_MOD_ID
#define LAPB_MOD_ID		0
#endif

#ifndef LAPB_DRV_NAME
#define LAPB_DRV_NAME		"lapb"
#endif

#ifndef LAPB_DRV_ID
#define LAPB_DRV_ID		0
#endif

/*
 *  STREAMS Definitions
 */

#define MOD_ID		LAPB_MOD_ID
#define MOD_NAME	LAPB_MOD_NAME
#define DRV_ID		LAPB_DRV_ID
#define DRV_NAME	LAPB_DRV_NAME

#ifdef MODULE
#define MOD_BANNER	LAPB_BANNER
#define DRV_BANNER	LAPB_BANNER
#else				/* MODULE */
#define MOD_BANNER	LAPB_SPLASH
#define DRV_BANNER	LAPB_SPLASH
#endif				/* MODULE */

static struct module_info dl_mod_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMAXPSZ,
	.mi_minpsz = STRMINPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_info dl_mux_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMAXPSZ,
	.mi_minpsz = STRMINPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

/*
 *  PRIVATE STRUCTURE.
 *  ==================
 *  When pushed as a module, a dl structure is allocated for the write side and a cd structure is
 *  allocated for the read side and the two are linked together.  Both are given the major and minor
 *  device number of the device over which the module is pushed.
 *
 *  When opened as a driver, a dl structure is allocated for the upper write and read side.  When a
 *  cd stream is linked, a cd structure is allocated for the lower write and read sides.  When
 *  bound, the dl and lower cd structures are linked.  A cd structure, cd_rput and cd_rsrv
 *  procedures can still respond (negatively) to SABM/SABME requests.
 *
 *  Whenever the structures are linked (after push or after attach), the oq pointers point to the
 *  upper read queue and the lower write queue.  These are the queues to test for canputnext and to
 *  do a putnext with messages to be passed upstream or downstream.  For a module, this will test rq
 *  and wq; for a multiplex, the upper rq and lower wq.  In this way, the put and service procedures
 *  can be written just to use oq for both.  If the oq pointer is NULL, the structures have simply
 *  not been linked yet.
 */

struct dl;
struct cd;

struct dl {
	struct dl *next;
	struct cd *cd;
	union {
		struct {
			major_t cmajor;
			minor_t cminor;
		} dev;
		long index;
	} u;
	uint i_state;
	uint i_oldstate;
	queue_t *rq;
	queue_t *wq;
	queue_t *oq;			/* lower write queue */
	dl_info_ack_t info;
	dl_qos_co_sel1_t qos;		/* quality of service selection */
	dl_qor_co_range1_t qor;		/* quality of service range */
	uchar caddr;			/* remote command address, local response address */
	uchar raddr;			/* remote response address, local command address */
	dl_ulong dl_max_conind;		/* maximum connection indications */
	uint flags;			/* boolean flags, see defines below */
	buf_queue_t conq;		/* connection queue */
	buf_queue_t sndq;		/* send queue */
	buf_queue_t rtxq;		/* retransmission queue */
};

#define DL_PRIV(__q) ((struct dl *)((__q)->q_ptr))

#define DL_FLAG_AUTOXID		01
#define DL_FLAG_AUTOTEST	02
#define DL_FLAG_CONNMGMT	04

#define DL_FLAG_EXTENDED	001
#define DL_FLAG_SUPERMODE	002
#define DL_FLAG_MLP		004
#define DL_FLAG_DCE		0001
#define DL_FLAG_DTE		0002
#define DL_FLAG_STE		0004

#define dl_extended(__dl)	(!!((__dl)->flags & DL_FLAG_EXTENDED))
#define dl_supermode(__dl)	(!!((__dl)->flags & DL_FLAG_SUPERMODE))
#define dl_mlp(__dl)		(!!((__dl)->flags & DL_FLAG_MLP))
#define dl_dce(__dl)		(!!((__dl)->flags & DL_FLAG_DCE))
#define dl_autoxid(__dl)	(!!((__dl)->flags & DL_FLAG_AUTOXID))
#define dl_autotest(__dl)	(!!((__dl)->flags & DL_FLAG_AUTOTEST))

#define DLF_UNATTACHED		(1<<DL_UNATTACHED)
#define DLF_ATTACH_PENDING	(1<<DL_ATTACH_PENDING)
#define DLF_DETACH_PENDING	(1<<DL_DETACH_PENDING)
#define DLF_UNBOUND		(1<<DL_UNBOUND)
#define DLF_BIND_PENDING	(1<<DL_BIND_PENDING)
#define DLF_UNBIND_PENDING	(1<<DL_UNBIND_PENDING)
#define DLF_IDLE		(1<<DL_IDLE)
#define DLF_UDQOS_PENDING	(1<<DL_UDQOS_PENDING)
#define DLF_OUTCON_PENDING	(1<<DL_OUTCON_PENDING)
#define DLF_INCON_PENDING	(1<<DL_INCON_PENDING)
#define DLF_CONN_RES_PENDING	(1<<DL_CONN_RES_PENDING)
#define DLF_DATAXFER		(1<<DL_DATAXFER)
#define DLF_USER_RESET_PENDING	(1<<DL_USER_RESET_PENDING)
#define DLF_PROV_RESET_PENDING	(1<<DL_PROV_RESET_PENDING)
#define DLF_RESET_RES_PENDING	(1<<DL_RESET_RES_PENDING)
#define DLF_DISCON8_PENDING	(1<<DL_DISCON8_PENDING)
#define DLF_DISCON9_PENDING	(1<<DL_DISCON9_PENDING)
#define DLF_DISCON11_PENDING	(1<<DL_DISCON11_PENDING)
#define DLF_DISCON12_PENDING	(1<<DL_DISCON12_PENDING)
#define DLF_DISCON13_PENDING	(1<<DL_DISCON13_PENDING)
#define DLF_SUBS_BIND_PND	(1<<DL_SUBS_BIND_PND)
#define DLF_SUBS_UNBIND_PND	(1<<DL_SUBS_UNBIND_PND)

struct cd {
	struct cd *next;
	struct dl *dl;
	union {
		struct {
			major_t cmajor;
			minor_t cminor;
		} dev;
		long index;
	} u;
	uint i_state;
	uint i_oldstate;
	queue_t *rq;
	queue_t *wq;
	queue_t *oq;			/* upper read queue */
	cd_info_ack_t info;
};

#define CD_PRIV(__q) ((struct cd *)((__q)->q_ptr))

#define CDF_UNATTACHED		(1<<CD_UNATTACHED)
#define CDF_UNUSABLE		(1<<CD_UNUSABLE)
#define CDF_DISABLED		(1<<CD_DISABLED)
#define CDF_ENABLE_PENDING	(1<<CD_ENABLE_PENDING)
#define CDF_ENABLED		(1<<CD_ENABLED)
#define CDF_READ_ACTIVE		(1<<CD_READ_ACTIVE)
#define CDF_INPUT_ALLOWED	(1<<CD_INPUT_ALLOWED)
#define CDF_DISABLE_PENDING	(1<<CD_DISABLE_PENDING)
#define CDF_OUTPUT_ACTIVE	(1<<CD_OUTPUT_ACTIVE)
#define	CDF_XRAY		(1<<CD_XRAY)

#define LAPB_FRAMING_SYNCHRONOUS    0x01	/* 5-bit bit stuffing */
#define LAPB_FRAMING_START_STOP	    0x02	/* escape character with start/stop bits */

#define LAPB_ESCAPE_CHAR_DEFAULT    0xBE	/* escape character */
#define LAPB_FLAG_CHAR_DEFAULT	    0x7E	/* flag character */

/*
 *  STATE TRANSISIONS
 *  =================
 */
#ifdef _DEBUG
STATIC INLINE const char *
dl_state_name(ulong state)
{
	switch (state) {
	case DL_UNATTACHED:
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:
		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:
		return ("DL_IDLE");
	case DL_UDQOS_PENDING:
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:
		return ("DL_SUBS_UNBIND_PND");
	case -1UL:
		return ("DL_UNUSABLE");
	default:
		swerr();
		return ("DL_????");
	}
}
#endif
STATIC INLINE dl_ulong
dl_set_state(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl_get_state(dl);

	(void) oldstate;
	dl->info.dl_current_state = dl->i_state = newstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, dl, dl_state_name(newstate),
		dl_state_name(oldstate)));
	return (newstate);
}
STATIC INLINE dl_ulong
dl_get_state(const struct dl *dl)
{
	return (dl->i_state);
}
STATIC INLINE dl_ulong
dl_get_statef(const struct dl *dl)
{
	return (1 << dl_get_state(dl));
}
STATIC INLINE int
dl_chk_state(const struct dl *dl, const dl_ulong mask)
{
	return ((dl_get_statef(dl) & (mask)) != 0);
}
STATIC INLINE int
dl_not_state(const struct dl *dl, const dl_ulong mask)
{
	return ((dl_get_statef(dl) & (mask)) == 0);
}

#ifdef _DEBUG
STATIC INLINE const char *
cd_state_name(cd_ulong state)
{
	switch (state) {
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
STATIC INLINE cd_ulong
cd_set_state(struct cd *cd, cd_ulong newstate)
{
	cd_ulong oldstate = cd_get_state(cd);

	(void) oldstate;
	cd->info.cd_state = cd->i_state = newstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, cd, cd_state_name(newstate),
		cd_state_name(oldstate)));
	return (newstate);
}
STATIC INLINE cd_ulong
cd_get_state(struct cd *cd)
{
	return (cd->i_state);
}
STATIC INLINE cd_ulong
cd_get_statef(const struct cd *cd)
{
	return (1 << cd_get_state(cd));
}
STATIC INLINE int
cd_chk_state(const struct cd *cd, const cd_ulong mask)
{
	return ((cd_get_statef(cd) & (mask)) != 0);
}
STATIC INLINE int
cd_not_state(const struct cd *cd, const cd_ulong mask)
{
	return ((cd_get_statef(cd) & (mask)) == 0);
}

/*
 *  Primitives sent upstream
 *  ------------------------
 *  Note that by using cd->oq as the active queue instead of q works fine.  If a bufcall results, oq
 *  will be qenabled, which in the module case is the correct queue to qenable, and in the
 *  multiplexing driver case will result in oq's service procedure enabling q across the multiplex.
 */
/**
 * m_hangup: - send M_HANGUP upstream
 * @dl: the data link
 * @q: the active queue
 * @req: message to free on success
 */
STATIC int
m_hangup(struct dl *dl, queue_t *q, mblk_t *req)
{
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, 0, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(req);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_error: - send M_ERROR upstream
 * @dl: the data link
 * @q: the active queue
 * @req: message to free on success
 * @rerr: read-side error
 * @werr: write-side error
 */
STATIC int
m_error(struct dl *dl, queue_t *q, mblk_t *req, uchar rerr, uchar werr)
{
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*mp->b_wptr++ = rerr;
		*mp->b_wptr++ = werr;
		printd(("%s: %p: <- M_ERROR\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(req);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_info_ack: - send DL_INFO_ACK upstream
 * @dl: the data link
 * @q: the active queue
 * @req: message to free on success
 *
 * Note that LAPB does not have any addresses.
 */
STATIC INLINE int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *req)
{
	dl_info_ack_t *p;
	dl_qos_co_sel1_t *q;
	dl_qor_co_range1_t *r;
	size_t size = sizeof(*p) + sizeof(*q) + sizeof(*r);
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_wptr += size;
		p = (typeof(p)) mp->b_wptr;
		q = (typeof(q)) (p + 1);
		r = (typeof(r)) (q + 1);
		*p = dl->info;
		*q = dl->qos;
		*r = dl->qor;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_addr_length = 0;
		p->dl_addr_offset = 0;
		p->dl_brdcst_addr_length = 0;
		p->dl_brdcst_addr_offset = 0;
		printd(("%s: %p: <- DL_INFO_ACK\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(req);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_bind_ack: - send DL_BIND_ACK upstream
 * @dl: the data link
 * @q: the active queue
 * @req: request message to free if successful
 * @cd: the comm device
 *
 * Note that LAPB does not have any addresses.  Only one stream can be bound to each DLSAP.
 *
 * The DLS provider may initialize the physical line on a DL_STYLE2 provider on receipt of a
 * DL_ATTACH_REQ or DL_BIND_REQ primitive.  Otherwise, the line must be initialized through some
 * management mechanism before the DL_ATTACH_REQ is issued by the DLS user.  Either way, the physical
 * link must be initialized and ready for use on successful completion of the DL_BIND_REQ.
 */
STATIC INLINE int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *req, struct cd *cd)
{
	dl_bind_ack_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = (dl->caddr << 8) | (dl->raddr);
		p->dl_addr_length = 0;
		p->dl_addr_offset = 0;
		p->dl_max_conind = dl->dl_max_conind;
		p->dl_xidtest_flg = 0;
		p->dl_xidtest_flg |= (dl_autoxid(dl) ? DL_AUTO_XID : 0);
		p->dl_xidtest_flg |= (dl_autotest(dl) ? DL_AUTO_TEST : 0);
		cd->dl = dl;
		dl_set_state(dl, DL_IDLE);
		printd(("%s: %p: <- DL_BIND_ACK\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(req);	/* freemsg checks for NULL */
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/**
 * dl_subs_bind_ack: - send a DL_SUBS_BIND_ACK upstream
 * @dl: data link
 * @q: active queue
 * @req: request message to free if successful
 *
 * Unused, LAPB does not bind far less subs_bind.
 */
STATIC INLINE int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *req)
{
	dl_subs_bind_ack_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_SUBS_BIND_ACK;
		p->dl_subs_sap_length = 0;
		p->dl_subs_sap_offset = 0;
		printd(("%s: %p: <- DL_SUBS_BIND_ACK\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(req);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/* FIXME: need different message hooks here. */
STATIC int send_UA_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf);
STATIC int send_DM_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf);
STATIC int send_DISC_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf);

/**
 * dl_ok_ack: - send DL_OK_ACK ustream
 * @dl: data link
 * @q: active queue
 * @req: request message to free iff successful
 * @cd: comm device
 * @ap: accepting data link
 * @cp: accepted connection indication
 */
STATIC INLINE int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *req, struct cd *cd, struct dl *ap, mblk_t *cp)
{
	mblk_t *mp;
	dl_ok_ack_t *p;
	int err;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = *(dl_ulong *) req->b_rptr;
		if (dl->wait.cd)
			dl_wait_unlink(dl);	/* remove from wait list */
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:
			if (!dl->cd.cd)
				dl_atta_link(dl, cd);	/* add dl to cd attach list */
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_PENDING:
			if (dl->cd.cd)
				dl_atta_unlink(dl);	/* remove from cd attach list */
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_UNBIND_PENDING:
			if (dl->bind.cd)
				dl_bind_unlink(dl);	/* remove from bind hash */
			if (dl->list.cd)
				dl_list_unlink(dl);	/* remove from list hash */
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_CONN_RES_PENDING:
			ensure(ap, ap = dl);
			if (cp) {
				ushort sapi = (cp->b_rptr[0] >> 2) & 0x3f;
				ushort tei = (cp->b_rptr[1] >> 1) & 0x7f;
				ushort pf = (cp->b_rptr[3] & 0x01);

				if ((err = send_UA_res(q, cd, sapi, tei, pf)) < 0) {
					freemsg(mp);
					return (err);
				}
				dl_timer_start(ap, t203);
				bufq_unlink(&dl->conq, cp);
				freemsg(cp);
			}
			if (ap != dl) {
				/* 
				   auto attach */
				if (dl->cd.cd != ap->cd.cd) {
					dl_atta_unlink(ap);
					dl_atta_link(ap, dl->cd.cd);
				}
				/* 
				   auto bind */
				if (dl->dlc.dl_sap != ap->dlc.dl_sap
				    || dl->dlc.dl_tei != ap->dlc.dl_tei) {
					dl_bind_unlink(ap);
					ap->dlc = dl->dlc;
					dl_bind_link(ap, ap->cd.cd);
				}
				if (dl->conq.q_msgs)
					dl_set_state(dl, DL_INCON_PENDING);
				else
					dl_set_state(dl, DL_IDLE);
			}
			/* 
			   move to data transfer state */
			dl->state = LAPD_ESTABLISHED;
			dl_set_state(ap, DL_DATAXFER);
			/* 
			   place in connection hashes */
			dl_conn_link(ap, ap->cd.cd);
			ap->vs = ap->vr = ap->va = 0;
			break;
		case DL_RESET_RES_PENDING:
			dl->state = LAPD_ESTABLISHED;
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON9_PENDING:
			if (cp) {
				ushort sapi = (cp->b_rptr[0] >> 2) & 0x3f;
				ushort tei = (cp->b_rptr[1] >> 1) & 0x7f;
				ushort pf = (cp->b_rptr[3] & 0x01);

				if ((err = send_DM_res(q, cd, sapi, tei, pf)) < 0) {
					freemsg(mp);
					return (err);
				}
				bufq_unlink(&dl->conq, cp);
				freemsg(cp);
			}
			dl->state = LAPD_TEI_ASSIGNED;
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON8_PENDING:
		case DL_DISCON11_PENDING:
		case DL_DISCON12_PENDING:
		case DL_DISCON13_PENDING:
			if ((1 << cd_get_state(cd)) &
			    (CDF_ENABLED | CDF_READ_ACTIVE | CDF_INPUT_ALLOWED | CDF_OUTPUT_ACTIVE))
			{
				/* 
				   layer 1 active */
				ushort sapi = dl->dlc.dl_sap;
				ushort tei = dl->dlc.dl_tei;
				ushort pf = 1;

				if ((err = send_DISC_cmd(q, cd, sapi, tei, pf))) {
					freemsg(mp);
					return (err);
				}
			}
			dl->state = LAPD_WAIT_RELEASE;
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_UNBIND_REQ:
			/* 
			   just jam tei */
			dl_bind_unlink(dl);
			if (dl->dl_max_conind)
				dl_list_unlink(dl);
			dl->dlc.dl_tei = -1;
			dl_bind_link(dl, dl->cd.cd);
			if (dl->dl_max_conind)
				dl_list_link(dl, dl->cd.cd);
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, DL_IDLE);
			break;
		default:
			/* 
			   happens for promicon, promiscoff, enabmulti, disabmulti */
			/* 
			   remain in the same state */
			break;
		}
		printd(("%s: %p: <- DL_OK_ACK\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(req);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_error_ack: - send a DL_ERROR_ACK upstream
 * @dl: the data link
 * @q: the active queue
 * @req: message to free if successful
 * @prim: primtive in error
 * @error: DLPI error
 * @reason: UNIX error
 */
STATIC INLINE int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *req, ulong prim, ulong error, ulong reason)
{
	dl_error_ack_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error;
		p->dl_unix_errno = reason;
		/* restore previous state */
		dl_set_state(dl, dl->i_oldstate);
		printd(("%s: %p: <- DL_ERROR_ACK\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(req);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_connect_ind: - send DL_CONNECT_IND upstream
 * @dl: data link
 * @q: active queue
 * @cp: connection request message to queue if successful
 */
STATIC INLINE int
dl_connect_ind(queue_t *q, struct dl *dl, mblk_t *cp)
{
	dl_connect_ind_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p) + 1 + 1, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_CONNECT_IND;
		p->dl_correlation = (typeof(p->dl_correlation)) (long) cp;
		p->dl_called_addr_length = 1;
		p->dl_called_addr_offset = sizeof(*p);
		p->dl_calling_addr_length = 1;
		p->dl_calling_addr_offset = sizeof(*p) + 1;
		p->dl_qos_length = 0;
		p->dl_qos_offset = 0;
		p->dl_growth = 0;
		*mp->b_wptr++ = cp->b_rptr[0];
		*mp->b_wptr++ = cp->b_rptr[0];
		bufq_queue(&dl->conq, cp);
		dl_set_state(dl, DL_INCON_PENDING);
		printd(("%s: %p: <- DL_CONNECT_IND\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_connect_con:- send DL_CONNECT_CON upstream
 * @dl: data link
 * @q: active queue
 * @ind: indication message to free if successful
 */
STATIC INLINE int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *ind)
{
	dl_connect_con_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p) + 1, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_CONNECT_CON;
		p->dl_resp_addr_length = 1;
		p->dl_resp_addr_offset = sizeof(*p);
		p->dl_qos_length = 0;
		p->dl_qos_offset = 0;
		p->dl_growth = 0;
		*mp->b_wptr++ = ind->b_rptr[0];
		//dl->state = LAPD_ESTABLISHED;
		dl_set_state(dl, DL_DATAXFER);
		//dl_timer_start(dl, t200);
		//dl_timer_start(dl, t203);
		dl->vs = dl->vr = dl->va = 0;
		printd(("%s: %p: <- DL_CONNECT_CON\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(ind);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_token_ack: - send DL_TOKEN_ACK upstream
 * @dl: data link
 * @q: active queue
 * @req: request message to free if successful
 */
STATIC INLINE int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *req)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = (typeof(p->dl_token)) (long) dl;
		printd(("%s: %p: <- DL_TOKEN_ACK\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(req);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_disconnect_ind: - send a DL_DISCONNECT_IND upstream
 * @dl: data link
 * @q: active queue
 * @ind: indication message to free if successful
 * @orig: origin
 * @reason: reason
 * @cp: connection pointer to destroy if one exists
 *
 * If the disconnect was the result of a DISC with the P bit set, we must send
 * a DM with the F bit set.  If it was the result of a DISC or DM without the
 * P bit set, we do not have to reply.
 */
STATIC INLINE int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *ind, ulong orig, ulong reason, mblk_t *cp)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_DISCONNECT_IND;
		p->dl_originator = orig;
		p->dl_reason = reason;
		p->dl_correlation = (ulong) cp;
		if (cp) {
			bufq_unlink(&dl->conq, cp);
			if (dl->conq.q_msgs)
				dl_set_state(dl, DL_INCON_PENDING);
			else
				dl_set_state(dl, DL_IDLE);
		} else {
			//dl->state = LAPD_TEI_ASSIGNED;
			dl_set_state(dl, DL_IDLE);
		}
		printd(("%s: %p: <- DL_DISCONNECT_IND\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(ind);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_reset_ind: - send a DL_RESET_IND upstream
 * @dl: data link
 * @q: active queue
 * @ind: indication message to free if successful
 * @orig: origin
 * @reason: reason
 */
STATIC INLINE int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *ind, ulong orig, ulong reason)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_RESET_IND;
		p->dl_originator = orig;
		p->dl_reason = reason;
		dl_set_state(dl, DL_PROV_RESET_PENDING);
		printd(("%s: %p: <- DL_RESET_IND\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(ind);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/**
 * dl_reset_con: - send a DL_RESET_CON upstream
 * @dl: data link
 * @q: activeq queue
 * @con: confirmation message to free if successful
 */
STATIC INLINE int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *con)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_RESET_CON;
		//dl->state = LAPD_ESTABLISHED;
		dl_set_state(dl, DL_DATAXFER);
		printd(("%s: %p: <- DL_RESET_CON\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(con);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_PHYS_ADDR_ACK            0x32
 *  -----------------------------------
 */
STATIC INLINE int
dl_phys_addr_ack(queue_t *q, struct dl *dl, size_t add_len, caddr_t add_ptr)
{
	mblk_t *mp;
	dl_phys_addr_ack_t *p;

	if ((mp = dl_allocb(dl, q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = add_len ? sizeof(*p) : 0;
		if (add_len) {
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: <- DL_PHYS_ADDR_ACK\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_GET_STATISTICS_ACK       0x35
 *  -----------------------------------
 */
STATIC INLINE int
dl_get_statistics_ack(queue_t *q, struct dl *dl, size_t sta_len, caddr_t sta_ptr)
{
	mblk_t *mp;
	dl_get_statistics_ack_t *p;

	if ((mp = dl_allocb(dl, q, sizeof(*p) + sta_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_GET_STATISTICS_ACK;
		p->dl_stat_length = sta_len;
		p->dl_stat_offset = sta_len ? sizeof(*p) : 0;
		if (sta_len) {
			bcopy(sta_ptr, mp->b_wptr, sta_len);
			mp->b_wptr += sta_len;
		}
		printd(("%s: %p: <- DL_GET_STATISTICS_ACK\n", DRV_NAME, dl));
		putnext(dl->rq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent to downstream
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  CD_INFO_REQ                 0x00 - Information request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_info_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_info_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_INFO_REQ;
		printd(("%s: %p: <- CD_INFO_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ATTACH_REQ               0x02 - Attach a PPA
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_attach_req(queue_t *q, struct cd *cd, struct dl *dl)
{
	mblk_t *mp;
	cd_attach_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_ATTACH_REQ;
		p->cd_ppa = cd->ppa;
		dl_wait_link(dl, cd);	/* put the dl into the wait list */
		printd(("%s: %p: <- CD_ATTACH_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_DETACH_REQ               0x03 - Detach a PPA
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_detach_req(queue_t *q, struct cd *cd, struct dl *dl)
{
	mblk_t *mp;
	cd_detach_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_DETACH_REQ;
		dl_wait_link(dl, cd);	/* put the dl into the wait list */
		printd(("%s: %p: <- CD_DETACH_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ENABLE_REQ               0x04 - Prepare a device
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_enable_req(queue_t *q, struct cd *cd, struct dl *dl)
{
	mblk_t *mp;
	cd_enable_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_ENABLE_REQ;
		p->cd_dial_type = CD_NODIAL;
		p->cd_dial_length = 0;
		p->cd_dial_offset = 0;
		cd_set_state(cd, CD_ENABLE_PENDING);
		dl_wait_link(dl, cd);	/* put the dl into the wait list */
		printd(("%s: %p: <- CD_ENABLE_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_DISABLE_REQ              0x05 - Disable a device
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_disable_req(queue_t *q, struct cd *cd, struct dl *dl, ulong disposal)
{
	mblk_t *mp;
	cd_disable_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_DISABLE_REQ;
		p->cd_disposal = disposal;
		cd_set_state(cd, CD_DISABLE_PENDING);
		dl_wait_link(dl, cd);	/* put the dl into the wait list */
		printd(("%s: %p: <- CD_DISABLE_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ALLOW_INPUT_REQ          0x0b - Allow input
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_allow_input_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_allow_input_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_ALLOW_INPUT_REQ;
		cd_set_state(cd, CD_INPUT_ALLOWED);
		printd(("%s: %p: <- CD_ALLOW_INPUT_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_READ_REQ                 0x0c - Wait-for-input request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_read_req(queue_t *q, struct cd *cd, ulong msec)
{
	mblk_t *mp;
	cd_read_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_READ_REQ;
		p->cd_msec = msec;
		cd_set_state(cd, CD_READ_ACTIVE);
		printd(("%s: %p: <- CD_READ_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_UNITDATA_REQ             0x0d - Data send request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_unitdata_req(queue_t *q, struct cd *cd, ulong atype, ulong prio, size_t dst_len, caddr_t dst_ptr,
		mblk_t *dp)
{
	mblk_t *mp;
	cd_unitdata_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_UNITDATA_REQ;
		p->cd_addr_type = atype;
		p->cd_priority = prio;
		p->cd_dest_addr_length = dst_len;
		p->cd_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- CD_UNITDATA_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_WRITE_READ_REQ           0x0e - Write/read request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_write_read_req(queue_t *q, struct cd *cd, ulong atype, ulong prio, size_t dst_len,
		  caddr_t dst_ptr, mblk_t *dp, ulong msec)
{
	mblk_t *mp;
	cd_write_read_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_WRITE_READ_REQ;
		p->cd_unitdata_req.cd_primitive = CD_UNITDATA_REQ;
		p->cd_unitdata_req.cd_addr_type = atype;
		p->cd_unitdata_req.cd_priority = prio;
		p->cd_unitdata_req.cd_dest_addr_length = dst_len;
		p->cd_unitdata_req.cd_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->cd_read_req.cd_primitive = CD_READ_REQ;
		p->cd_read_req.cd_msec = msec;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- CD_WRITE_READ_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_HALT_INPUT_REQ           0x11 - Halt input
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_halt_input_req(queue_t *q, struct cd *cd, ulong disposal)
{
	mblk_t *mp;
	cd_halt_input_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_HALT_INPUT_REQ;
		p->cd_disposal = disposal;
		cd_set_state(cd, CD_ENABLED);
		printd(("%s: %p: <- CD_HALT_INPUT_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ABORT_OUTPUT_REQ         0x12 - Abort output
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_abort_output_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_abort_output_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_ABORT_OUTPUT_REQ;
		printd(("%s: %p: <- CD_ABORT_OUTPUT_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  CD_MUX_NAME_REQ             0x13 - get mux name (Gcom)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_mux_name_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_mux_name_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_MUX_NAME_REQ;
		printd(("%s: %p: <- CD_MUX_NAME_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  CD_MODEM_SIG_REQ            0x15 - Assert modem signals (Gcom)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_modem_sig_req(queue_t *q, struct cd *cd, ulong sigs)
{
	mblk_t *mp;
	cd_modem_sig_req_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_MODEM_SIG_REQ;
		p->cd_sigs = sigs;
		printd(("%s: %p: <- CD_MODEM_SIG_REQ\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_MODEM_SIG_POLL           0x17 - requests a CD_MODEM_SIG_IND (Gcom)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_modem_sig_poll(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_modem_sig_poll_t *p;

	if ((mp = cd_allocb(cd, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->cd_primitive = CD_MODEM_SIG_POLL;
		printd(("%s: %p: <- CD_MODEM_SIG_POLL\n", DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  STATE MACHINES
 *
 *  =========================================================================
 */

/*
 *  MESSAGE HANDLING.
 *  =================
 */
static int
cd_data_req(queue_t *q, mblk_t *dp)
{
	struct dl *dl = DL_PRIV(q);
	queue_t *oq;

	if (dl->oq) {
		if (canputnext(dl->oq)) {
			putnext(dl->oq, dp);
			return (0);
		}
		return (-EBUSY);
	}
	return (-EFAULT);
}

static streamscall void
dl_bufcall(long arg)
{
	queue_t *q = (queue_t *) arg;
	struct dl *dl = DL_PRIV(q);

	if (q == dl->rq) {
		if (unlikely(xchg(&dl->rbid, 0) == 0))
			return;
		qenable(q);
		return;
	}
	if (q == dl->wq) {
		if (unlikely(xchg(&dl->wbid, 0) == 0))
			return;
		qenable(q);
		return;
	}
}

/**
 * dl_allocb: - allocate a message block or generate bufcall
 * @dl: data link
 * @q: active queue
 * @size: sizeof of block
 * @pri: priority of allocation
 */
static mblk_t *
dl_allocb(struct dl *dl, queue_t *q, size_t size, int pri)
{
	mblk_t *mp;

	if (unlikely((mp = allocb(size, pri)) == NULL)) {
		bcid_t bid, *bidp;

		bidp = (q == dl->wq) ? &dl->wbid : &dl->rbid;
		if ((bid = xchg(bidp, bufcall(size, pri, &dl_bufcall, (long) q))))
			unbufcall(bid);
	}
	return (mp);
}

static mblk_t *
dl_allocb_snd(struct dl *dl, queue_t *q, size_t size, int pri, ushort addr, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = dl_allocb(dl, q, size, pri))) {
		*mp->b_wptr++ = addr;
		mp->b_cont = dp;
	}
	return (mp);
}

#define DL_MTYPE_RR	0x01 /* 0000 0001 */
#define DL_MTYPE_RNR	0x05 /* 0000 0101 */
#define DL_MTYPE_REJ	0x09 /* 0000 1001 */
#define DL_MTYPE_I	0x00 /* 0000 0000 */
#define DL_MTYPE_UI	0x03 /* 0000 0011 */
#define DL_MTYPE_SABM	0x2F /* 0010 1111 */
#define DL_MTYPE_SABME	0x6F /* 0110 1111 */
#define DL_MTYPE_SM	0xC3 /* 1100 0011 */
#define DL_MTYPE_DISC	0x43 /* 0100 0011 */
#define DL_MTYPE_DM	0x0F /* 0000 1111 */
#define DL_MTYPE_UA	0x63 /* 0110 0011 */
#define DL_MTYPE_FRMR	0x87 /* 1000 0111 */
#define DL_MTYPE_XID	0xAF /* 1010 1111 */

static int
dl_send_i_frame(queue_t *q, mblk_t *dp, ushort pf, ushort nr, ushort ns)
{
	struct dl *dl = DL_PRIV(q);
	mblk_t *mp;
	int err;

	if ((mp = dl_allocb_snd(dl, q, FASTBUF, BPRI_MED, dl->caddr, dp))) {
		if (dl_extended(dl)) {
			*mp->b_wptr++ = ns;
			*mp->b_wptr++ = (pf << 7) | nr;
		} else {
			*mp->b_wptr++ = (ns << 4) | (pf << 3) | nr;
		}
		if ((err = cd_data_req(q, mp))) {
			freeb(mp);
			return (err);
		}
	}
	return (-ENOBUFS);
}

static int
dl_send_s_frame(queue_t *q, ushort cmd, ushort pf, ushort nr, ushort mtype)
{
	struct dl *dl = DL_PRIV(q);
	mblk_t *mp;
	int err;

	if ((mp = dl_allocb_snd(dl, q, FASTBUF, BPRI_MED, cmd ? dl->caddr : dl->raddr, NULL))) {
		if (dl_extended(dl)) {
			*mp->b_wptr++ = mtype;
			*mp->b_wptr++ = (pf << 7) | nr;
		} else {
			*mp->b_wptr++ = mtype | (pf << 3) | nr;
		}
		if ((err = cd_data_req(q, mp))) {
			freeb(mp);
			return (err);
		}
	}
	return (-ENOBUFS);
}

static int
dl_send_u_frame(queue_t *q, ushort cmd, ushort pf, ushort mtype)
{
	struct dl *dl = DL_PRIV(q);
	mblk_t *mp;
	int err;

	if ((mp = dl_allocb_snd(dl, q, FASTBUF, BPRI_MED, cmd ? dl->caddr : dl->raddr, NULL))) {
		*mp->b_wptr++ = mtype | (pf << 3);
		if ((err = cd_data_req(q, mp))) {
			freeb(mp);
			return (err);
		}
	}
	return (-ENOBUFS);
}

/* RR - Receive Ready */
static int
dl_send_rr(queue_t *q, ushort cmd, ushort pf, ushort nr)
{
	return dl_send_s_frame(q, cmd, pf, nr, DL_MTYPE_RR);
}

/* RNR - Receive Not Ready */
static int
dl_send_rnr(queue_t *q, ushort cmd, ushort pf, ushort nr)
{
	return dl_send_s_frame(q, cmd, pf, nr, DL_MTYPE_RNR);
}

/* REJ - Reject */
static int
dl_send_rej(queue_t *q, ushort cmd, ushort pf, ushort nr)
{
	return dl_send_s_frame(q, cmd, pf, nr, DL_MTYPE_REJ);
}

/* SREJ - Selective Reject */
static int
dl_send_srej(queue_t *q, ushort cmd, ushort pf, ushort nr)
{
}

/* UI - Unnumbered Information */
static int
dl_send_UI(queue_t *q, ushort pf, mblk_t *dp)
{
	return dl_send_u_frame(q, 1, pf, DL_MTYPE_UI);
}

/* SABM - Set Asynchronous Balanced Mode */
static int
dl_send_sabm(queue_t *q, ushort pf)
{
	return dl_send_u_frame(q, 1, pf, DL_MTYPE_SABM);
}

/* SABME - Set Asynchronous Balanced Mode Extended */
static int
dl_send_sabme(queue_t *q, ushort pf)
{
	return dl_send_u_frame(q, 1, pf, DL_MTYPE_SABME);
}

/* DISC - Disconnect */
static int
dl_send_disc(queue_t *q, ushort pf)
{
	return dl_send_u_frame(q, 1, pf, DL_MTYPE_DISC);
}

/* DM - Disconnect Mode */
static int
dl_send_dm(queue_t *q, ushort pf)
{
	return dl_send_u_frame(q, 0, pf, DL_MTYPE_DM);
}

/* UA - Unnumbered Acknowledgement */
static int
dl_send_ua(queue_t *q, ushort pf)
{
	return dl_send_u_frame(q, 0, pf, DL_MTYPE_UA);
}

/* FRMR - Frame Reject */
static int
dl_send_frmr(queue_t *q, ushort rfcf, ushort pf, ushort vs, ushort cr, ushort vr, ushort flags)
{
	struct dl *dl = DL_PRIV(q);
	mblk_t *mp;
	int err;

	if ((mp = dl_allocb_snd(dl, q, FASTBUF, BPRI_MED, dl->raddr, NULL))) {
		*mp->b_wptr++ = DL_MTYPE_FRMR | (pf << 3);
		if (dl_extended(dl)) {
			if (rfcf & 0xff00) {
				*mp->b_wptr++ = (rfcf >> 8);
				*mp->b_wptr++ = rfcf & 0xff;
			} else {
				*mp->b_wptr++ = rfcf;
				*mp->b_wptr++ = 0;
			}
		} else {
			*mp->b_wptr++ = rfcf;
		}
		*mp->b_wptr++ = (vs << 4) | (cr << 3) | (vr << 0);
		*mp->b_wptr++ = flags;
		if ((err = cd_data_req(q, mp))) {
			freeb(mp);
			return (err);
		}
	}
	return (-ENOBUFS);
}

/* I - Information */
static int
dl_send_i(queue_t *q, mblk_t *dp, ushort pf, ushort nr, ushort ns)
{
	return dl_send_i_frame(q, dp, pf, nr, ns);
}

/* RR - Receive Ready */
/*
 * Receiving acknowlegement: When correctly receiving an I frame or a supervisory frame (RR, RNR, or
 * REJ), even in the busy condition, the DCE will consider the N(R) contained in this frame as an
 * acknowledgement for all I frames it has transmitted with an N(S) up to and including the received
 * N(R)-1.  The DCE will stop timer T1 when it correctly receives an I frame or a supervisory frame
 * with N(R) higher than the last received N(R) (actually acknowledging some I frames), or a REJ frame
 * with an N(R) equal to the last received N(R).  If timer T1 has been stopped by the receipt of an I,
 * RR or RNR frame, and if there are outstanding I frames still unacknowledged, the DCE will restart
 * timer T1.  If timer T1 then runs out, the DCE will follow the recovery procedure with respect to
 * unacknowledged I frames.  If timer T1 has been stopped by the receipt of a REJ frame, the DCE will
 * follow the retransmission procedures. */
static int
dl_recv_RR_cmd(queue_t *q, mblk_t *mp, ushort pf, ushort nr)
{
	/* A dl entity receiving an RR with the P bit set to 1 shall set the F bit to 1 in the next
	 * RR, RNR, REJ (or FRMR or DM or SREJ in LAPB) frame it transmits. */
}
/* RR - Receive Ready */
static int
dl_recv_RR_res(queue_t *q, mblk_t *mp, ushort pf, ushort nr)
{
}

/* RNR - Receive Not Ready */
/*
 * Receiving an RNR frame: After receiving an RNR frame whose N(R) acknowledges all frames previously
 * transmitted, the DCE will stop timer T1 and may then transmit an I frame, with the P bit set to 0,
 * whose send sequence number is equal to the N(R) indicated in the RNR frame, restarting timer T1 as
 * it does.  After receiving an RNR frame whose N(R) indicates a previously transmitted frame, the DCE
 * will not transmit ore retransmit any I frame, timer T1 being already running.  In either case, if
 * timer T1 runs out before receipt of a busy clearance indication, the DCE will folllow the procedure
 * described below.  In any case, the DCE will not transmit any other I frames before receiving an RR
 * or REJ frame, or before the completion of a link resetting procedure.  Alternatively, after
 * receiving an RNR frame, the DCE may wait for a period of time (e.g. the length of the timer T1) and
 * then transmit a supervisory command frame (RR, RNR or REJ) with the P bit set to 1, and start timer
 * T1, in order to determine if there is any change in the receive status of the DTE.  The DTE shall
 * respond to the P bit set to 1 with a supervisory frame (RR, RNR or REJ) with the F bit set to 1
 * indicating either continuance of the busy condition (RNR) or clearance of the busy condition (RR or
 * REJ).  Upon receipt of the DTE response, timer T1 is stopped.  1) If the response is the RR or REJ
 * response, the busy condition is cleared and the DCE may transmit I frames beginning with the I
 * frame identified by the N(R) in the received reponse frame.  2) If the response is the RNR
 * response, the busy condition still exists, and the DCE will after a period of time (e.g. the length
 * of timer T1) repeat the enquiry of the DTE receive status.  If timer T1 runs out before a status
 * reponse is received, the enquiry process above is repeated.  If N2 attempts to get a status
 * response faile (i.e. timer T1 runs out N2 times), the DCE will initiate a data link set-up
 * procedure and enter the disconnected phase.  If at any time during the enquiry process, an
 * unsolicited RR or REJ frame is received from the DTE, it will be considered to be an indication of
 * clearance of the busy condition.  Should the unsolicited RR or REJ frame be a command frame with
 * the P bit set to 1, the appropriate response frame with the F bit set to 1 must be transmitted
 * before the DCE may resume transmission of I frames.  If timer T1 is running, the DCE will wait for
 * the non-busy response with the F bit set to 1 or will wait for timer T1 to run out and then either
 * may reinitiate the enquiry process in order to realize a successful P/F bit exchange or may resume
 * transmission of I frames beginning with the I frame identified by the N(R) in the received RR or
 * REJ frame. */
static int
dl_recv_RNR_cmd(queue_t *q, mblk_t *mp, ushort pf, ushort nr)
{
	/* A dl entity receiving an RNR with the P bit set to 1 shall set the F bit to 1 in the next
	 * RR, RNR, REJ (or FRMR or DM or SREJ in LAPB) frame it transmits. */
}
/* RNR - Receive Not Ready */
static int
dl_recv_RNR_res(queue_t *q, mblk_t *mp, ushort pf, ushort nr)
{
}

/* REJ - Reject */
/*
 * Receiving a REJ frame: When receiving a REJ frame, the DCE will set its send state varilable V(S)
 * to N(R) received in the REJ control field.  It will transmit the corresponding I frame as soon as
 * it is available or retransmit it in accordance with the transmission procedures.  (Re)transmission
 * will conform to the following procedure: i) if the DCE is transmitting a supervisory command or
 * response when it receives the REJ frame, it will complete that transmission before commencing
 * transmission of the requested I frame; ii) if the DCE is transmitting an unnumbered command or
 * response when it receives the REJ frame, it will ignore the request for retransmission; iii) if the
 * DCE is tranmitting an I frame when the REJ frame is received, it may abort the I frame and commence
 * transmission of the requested I frame immediately after abortion; iv) if the DCE is not
 * transmitting any frame when the REJ frame is received, it will commence transmission of the
 * requested I frame immediately.  In all cases, of other unacknowleged I frames had already been
 * tranmitted following the one in the REJ frame, the those I frames will be retransmitted by the DCE
 * following the retransmission of the requested I frame.  Other I frames not yet transmitted may be
 * transmitted folowing the retransmitted I frames.  If the REJ frame was received from the DTE as a
 * command with the P bit set to 1, the DCE will transmit an RR, RNR or REJ response with the F bit
 * set to 1 before transmitting or retransmitting the corresponding I frame. */
static int
dl_recv_REJ_cmd(queue_t *q, mblk_t *mp, ushort pf, ushort nr)
{
	/* A dl entity receiving an REJ with the P bit set to 1 shall set the F bit to 1 in the next
	 * RR, RNR, REJ (or FRMR or DM or SREJ in LAPB) frame it transmits. */
}
/* REJ - Reject */
static int
dl_recv_REJ_res(queue_t *q, mblk_t *mp, ushort pf, ushort nr)
{
}

/* SREJ - Selective Reject */
/* Receiving an SREJ response frame: When receiving an SREJ response frame with F bit set to 0, the
 * STE shal retransmit all I frames, whose sequence numbers are indicated in the N(R) field and the
 * information field of the SREJ frame, in the order specified in the SREJ frame.  Retransmission
 * shall conform to the following: a) If the STE is transmitting a supervisory or I frame when it
 * receives the SREJ frame, it shall complete the transmission before commencing of the requested I
 * frames.  b) If the STE is transmitting an unnumbered command or response when it receives the SREJ
 * frame, it shall ignore the request for retransmission.  c) If the STE is not transmitting any frame
 * when it receives the SREJ frame, it shall commence transmission of the requested I frames
 * immediately.  If there was no outstanding poll condition, then a poll shall be sent, either by
 * transmitting an RR command (or RNR command if the STE is in the busy condition) with the P bit set
 * to 1 or by setting the P bit in the last retransmitted I frame and Timer T1 shall be restarted.  If
 * there is an outstanding poll condition, then timer T1 shall not be restarted.
 * When receiving an SREJ response frame with the F bit set to 1, the STE shall retransmit all I
 * frames whose sequence numbers are indicated in the N(R) field and the information field of the SREJ
 * frame, in the order specified in the SREJ frame, except those I frames that were sent after the
 * frame with the P bit set to 1 was sent.  Retransmission shall conform to the following: a) If the
 * STE is transmitting a supervisory or I frame when it receives the SREJ frame, it shall complete
 * that transmission before commencing transmission of the requested I frames. b) If the STE is
 * transmitting an unnumbered command or response when it receives the SREJ frame, it shall ignore the
 * request for retransmission.  c) If the STE is not transmitting any frame when it receives the SREJ
 * frame, it shall commence transmission of the requested I frames immediately.  If any frames are
 * retransmitted, then a poll shall be sent, either by transmitting an RR command (or RNR command if
 * the STE is in the busy condition) with the P bit set to 1 or by setting the P bit in the last
 * retransmitted I frame.  Timer T1 shall be restarted. */
static int
dl_recv_SREJ_res(queue_t *q, mblk_t *mp, ushort pf, ushort nr)
{
}

/* UI - Unnumbered Information */
static int
dl_recv_UI_cmd(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* SABM - Set Asynchronous Balanced Mode */
static int
dl_recv_SABM_cmd(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* SABME - Set Asynchronous Balanced Mode Extended */
static int
dl_recv_SABME_cmd(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* SM - Set Mode */
static int
dl_recv_SM_cmd(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* DISC - Disconnect */
static int
dl_recv_DISC_cmd(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* DM - Disconnect Mode */
static int
dl_recv_DM_cmd(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* UA - Unnumbered Acknowledgement */
static int
dl_recv_UA_cmd(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* FRMR - Frame Reject */
/*
 * Sent in response to the receipt of a command or response field that is unidentified or not
 * implemented; the receipt of an I ream with an information field which exceeds the maximum
 * established length; receipt of an invalid N(R); or, receipt of a frame with an information field
 * which is not permitted or receipt of a supervisory or unnumbered frame with an incorrect length. */
static int
dl_recv_FRMR_res(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* XID - Exchange Identification */
static int
dl_recv_XID_cmd(queue_t *q, mblk_t *mp, ushort pf)
{
}
/* XID - Exchange Identification */
static int
dl_recv_XID_res(queue_t *q, mblk_t *mp, ushort pf)
{
}

/* I - Information */
/*
 * Busy condition: The busy condition results when the DCE or DTE is temporarily unable to continue to
 * receive I frames due to internal constraints, e.g. receive buffering limitations.  In this case, an
 * RNR frame is transmitted from the busy DCE or DTE.  I frames pending transmission may be
 * transmitted from the busy DCE or DTE prior to or following the RNR frame.  An indication that the
 * busy condition has cleared in communicated by the tranmission of a UA (only in response to a
 * SABM/SABME command), RR, REJ or SABM/SABME (module 8/module 128) frame. */
/*
 * N(S) sequence error condition: The infromation field of all I frames received whose N(S) does not
 * equal the receive state variable V(R) will be discarded.  An N(S) sequence error exception
 * condition occurs in the receiver when an I frame received contains an N(S) which is not equal to
 * the receive state variable V(R) at the receiver.  The receiver does not acknowledge (increment its
 * receive state varibale) the I frame causing the sequence error, or any I frame which may follow,
 * until an I frame with the correct N(S) is received.  A DCE or DTE which receives one or more valid
 * I frames having sequence errors or subsequent supervisory frames (RR, RNR, and REJ) shall accept
 * the control information contained in the N(R) field and the P or F bit to perform data link control
 * functions, e.g. to receive acknowledgement of previously transmitted I frames and to cause the DCE
 * or DTE to response (P bit set to 1). */
/*
 * REJ recovery: The REJ frame is used by a receiving DCE or DTE to intiate a recovery
 * (retransmission) following the detection of an N(S) sequence error.  With respect to each direction
 * of transission on the data link, only one "sent REJ" exception condition from a DCE or DTE, to a
 * DTE or DCE, is established at a time.  A "send REJ" exception condition is cleared when the
 * requested I frame is received.  A DCE or DTE receiving a REJ frame initiates sequential
 * (re-)transmission of I frames starting with the I frame indicated by the N(R) contained in the REJ
 * frame.  The retransmitted frames may contain an N(R) and a P bit that are updated from and
 * therefore different from, the ones contained in the originally transmitted I frames.
 */
/*
 * Time-out recovery: If a DCE or DTE, due to transmission error, does not receive (or receives and
 * discards) a single I frame or the last I frame(s) in a sequence of I frames, it will not detect an
 * N(S) sequence error condtion and, therefore, will not transmit a REJ frame.  The DTE or DCE which
 * transmitted the unacknowledged I frame(s) shall, following the completion of a system specified
 * time-out period, take appropriate recovery action to determine at which I frame retransmission must
 * begin.  The retransmitted frame(s) may contain an N(R) and a P bit that is updated from, and
 * therefore different from, the ones contained in the originally transmitted frame(s).
 */
/*
 * Receiving an I frame:  When the DCE is not in a busy condition and receives a valid I frame whose
 * send sequence number N(S) is equal to the DCE receive state variable V(R), the DCE will accept the
 * information field of this frame, increment by one its receive state variable V(R), and act as
 * follows:  a) If the DCE is still not in a busy condition: i) If an I frame is available for
 * transmission by the DCE, it may acknowledge the received I frame by setting N(R) in the control
 * field of the next transmitted I frame to the value of the DCE receive state variable V(R).
 * Alternatively, the DCE may acknowledge the received I frame by transmitting an RR frame with the
 * N(R) equal to the value of the DCE receive state variable V(R).  ii) If no I frame is available for
 * tranmissoin by the DCE, it will transmit an RR frame with N(R) equal to the value of the DCE
 * receive state variable V(R). b) If the DCE is now in a busy condition, it will transmit an RNR
 * frame with N(R) equal to the value of the DCE receive state variable V(R).  When the DCE is in a
 * busy condition, it may ignore the information field contained in any received I frame. */
/*
 * Reception of out of sequence I frames: When the DCE receives a valid I frame whose send sequence
 * number N(S) is incorrect, i.e. not equal to the current DCE receive state variable V(R), it will
 * discard the information field of the I frame and transmit a REJ frame with the N(R) set to one
 * higher than the N(S) of the last correctly received I frame.  The REJ frame will be a command frame
 * with the P bit set to 1 if an acknowledged transfer of the retransmission request is required;
 * otherwise, the REJ frame may be either a command or a response frame.  The DCE will then discard
 * the information field of all I frames received until the expected I frame is correctly received.
 * When receiving the expected I frame, the DCE will then acknowledge the I frame as normal. */

static int
dl_recv_I_cmd(queue_t *q, mblk_t *mp, ushort pf, ushort ns, ushort nr)
{
	/* A dl entity receiving an I frame with the P bit set to 1 shall set the F bit to 1 in the
	 * next RR, RNR, REJ (or FRMR or DM or SREJ in LAPB) frame it transmits. */
}

static streams_inline streams_fastcall __hot_read int
dl_recv_msg_slow(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	ushort ns, pf, nr;
	int ext = dl_extended(dl);
	int sup = dl_supermode(dl);
	int mod = (sup << 1) | (ext ^ sup);
	int num = ((mp->b_rptr[1] & 0x03) != 0x03);
	int inf = ((mp->b_rptr[1] & 0x01) == 0x00);
	int size = msgdsize(mp);
	int cmd = 0;

	if (size < 2)
		goto discard;
	if (num && size < 2 + mod)
		goto badsize;

	switch (mod) {
	case 00:		/* basic - modulo 8*/
		nr = (mp->b_rptr[1] >> 5) & 0x07;
		ns = (mp->b_rptr[1] >> 1) & 0x07;
		pf = (mp->b_rptr[1] >> 4) & 0x01;
		break;
	case 01:		/* extended - modulo 127*/
		nr = (mp->b_rptr[2] >> 1) & 0x7f;
		ns = (mp->b_rptr[1] >> 1) & 0x7f;
		pf = (mp->b_rptr[2] >> 0) & 0x01;
		break;
	case 03:		/* super - modulo 32768 */
		nr = ((ushort)mp->b_rptr[4] << 7) | ((ushort)mp->b_rptr[3] >> 1);
		ns = ((ushort)mp->b_rptr[2] << 7) | ((ushort)mp->b_rptr[1] >> 1);
		pf = (mp->b_rptr[3] >> 0) & 0x01;
		break;
	case 02:		/* illegal */
		swerr();
		goto discard;
	}

	/*- Addresses:
	 *  SLP:    A 0x03  DCE->DTE command  DCE<-DTE response
	 *	    B 0x01  DTE->DCE command  DTE<-DCE response
	 *  MLP:    C 0x0F  DCE->DTE command  DCE<-DTE response
	 *	    D 0x07  DTE->DCE command  DTE<-DCE response */

	if ((mp->b_rptr[0] & 0xF0) != 0)
		goto discard;	/* bad address */

	if (mp->b_rptr[0] != dl->raddr && mp->b_rptr[0] != dl->caddr)
		goto discard;

	/* address on which I receive commands is the same as the address on which I send
	   responses. */
	/* address on which I receive responses is the same as the address on which I send
	   commands. */

	cmd = (mp->b_rptr[0] == dl->raddr) ? 1 : 0;

	switch (mp->b_rptr[1] & 0x03) {
	case 0x00:
	case 0x02:		/* Information *//* command */
		if (!cmd)
			goto discard;
		if (size > 2 + ext + dl->info.dl_max_sdu)
			goto toolong;
		return dl_recv_I_cmd(q, mp, pf, ns, nr);
	case 0x01:		/* Supervisory */
		switch ((mp->b_rptr[1] >> 2) & 0x03) {
		case 0x00:	/* RR *//* command, reponse */
			if (size != 2 + mod)
				goto badsize;
			if (cmd)
				return dl_recv_RR_cmd(q, mp, pf, nr);
			return dl_recv_RR_res(q, mp, pf, nr);
		case 0x01:	/* RNR *//* command, reponse */
			if (size != 2 + mod)
				goto badsize;
			if (cmd)
				return dl_recv_RNR_cmd(q, mp, pf, nr);
			return dl_recv_RNR_res(q, mp, pf, nr);
		case 0x02:	/* REJ *//* command, reponse */
			if (size != 2 + mod)
				goto badsize;
			if (cmd)
				return dl_recv_REJ_cmd(q, mp, pf, nr);
			return dl_recv_REJ_res(q, mp, pf, nr);
		case 0x03:
			if (cmd)
				goto discard;
			return dl_recv_SREJ_res(q, mp, pf, nr);
		}
	case 0x03:		/* Unnumbered */
		if ((mp->b_rptr[1] & 0x37) != 0x21 && size != 2)
			goto badsize;
		switch ((mp->b_rptr[1] >> 2) & 0x3B) {
		case 0x00:	/* UI *//* command *//* 0000 0000 */
			if (!cmd)
				goto discard;
			return dl_recv_UI_cmd(q, mp, pf);
		case 0x0b:	/* SABM *//* command *//* 0000 1011 */
			if (!cmd)
				goto discard;
			return dl_recv_SABM_cmd(q, mp, pf);
		case 0x1b:	/* SABME *//* command *//* 0001 1011 */
			if (!cmd)
				goto discard;
			return dl_recv_SABME_cmd(q, mp, pf);
		case 0x30:	/* SM *//* command *//* 0011 0000 */
			if (!cmd)
				goto discard;
			return dl_recv_SM_cmd(q, mp, pf);
		case 0x10:	/* DISC *//* command *//* 0001 0000 */
			if (!cmd)
				goto discard;
			return dl_recv_DISC_cmd(q, mp, pf);
		case 0x03:	/* DM *//* response *//* 0000 0011 */
			if (cmd)
				goto discard;
			return dl_recv_DM(q, mp, pf);
		case 0x18:	/* UA *//* response *//* 0001 1000 */
			if (cmd)
				goto discard;
			return dl_recv_UA_cmd(q, mp, pf);
		case 0x21:	/* FRMR *//* response *//* 0010 0001 */
			if (cmd)
				goto discard;
			return dl_recv_FRMR_res(q, mp, pf);
		case 0x2b:	/* XID */
			if (cmd)
				return dl_recv_XID_cmd(q, mp, pf);
			return dl_recv_XID_res(q, mp, pf);
		default:	/* error */
			goto unrecog;
		}
	}
	/* Frame rejection condition: A frame rejection condition is established upon the receipt
	   of an error-free frame with one of the conditions listed below.  At the DCE or DTE, this 
	   frame rejection exception condition is reported by an FRMR response for approprite DTE
	   or DCE action, respectively.  Once a DCE has established such an exception condition, no
	   additional I frames are accepted until the condition is reset by the DTE, except for
	   examination of the P bit.  The FRMR response may be repeated at each opportunity, until
	   recovery is effected by the DTE, or until the DCE initiates its own recovery in case the
	   DTE does not respond. */
	{
		ushort flags;

	      badnr:
#define DL_REJ_FLAG_BADNR	0x10
		/* send a frmr for bad N(R) */
		flags = DL_REJ_FLAG_BADNR;
		goto error;
	      toolong:
		/* send a frmr for too long info frame */
#define DL_REJ_FLAG_TOOLONG	0x20
		flags = DL_REJ_FLAG_TOOLONG;
		goto error;
	      badsize:
		/* send a frmr for bad size has info or wrong size */
#define DL_REJ_FLAG_BADSIZE	0xC0
		flags = DL_REJ_FLAG_BADSIZE;
		goto error;
	      unrecog:
		/* send a frmr for unrecognized */
#define DL_REJ_FLAG_UNDEF	0x80
		flags = DL_REJ_FLAG_UNDEF;
		goto error;
	      error:
		/* send an frmr */
		return dl_send_frmr();
	}
      discard:
	/* Invalid frame condition: Any frame that is invalid will be discarded, and no action
	   taken as a result of that frame.  An invalid frame is defined as one which: a) is not
	   properly bounded by two flags; b) in basic (modulo 8) operation, contains fewer than 32
	   bits between flags; in extended (modulo 128) operation, contains fewer than 40 bits
	   between flags of frames that contain sequence numbers or 32 bits between flags of frames 
	   that do not contain sequence numbers; c) or start/stop transmission, in addition to
	   conditions listed in b), contains an octet-framing violoation (i.e. a 0 bit occurs where 
	   a stop bit is expected); d) contains a Frame Check Sequence (FCS) error; e) contains an
	   address other than A or B (for single link operation) or other than C or D (for
	   multilink operation); or, f) frame aborted: in synchronous transmission, a frame is
	   aborted when it contains at least seven contiguous 1 bits (with no inserted 0 bit); in
	   start/stop transmission, a frame is aborted when it contains the two-octet sequence
	   composed fo the control escape octet followed by a closing flag.  For those networks
	   that are octet aligned, a detection of non-octet alignment may be made at the Data Link
	   Layer by adding a frame validity check that requires the number of bits between the
	   opening flag and the closing flag, excluding inserted bits (for transparency or for
	   transmission timing for start/stop transmission), to be an integral number of octets in
	   length, or the frame is considered invalid. */

	freemsg(mp);
	return (0);
}

/*
 *  DLPI PRIMITIVE HANDLING
 *  =======================
 */
static void
bufqenable(queue_t *q, size_t size, int priority)
{
}

/**
 *  dl_data_req: - DL_DATA_REQ (M_DATA)
 *  @q: upper write queue
 *  @mp: the primitive
 */
static streams_noinline streams_fastcall int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	size_t dlen = msgdsize(mp);
	int err;

	if (dlen < dl->info.dl_min_sdu || dlen > dl->info.dl_max_sdu)
		goto eproto;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_PROV_RESET_PENDING:
		freemsg(mp);
		return (0);
	case DL_DATAXFER:
		bufq_queue(&dl->sndq, mp);
		return (0);
	default:
	case DL_USER_RESET_PENDING:
	case DL_RESET_RES_PENDING:
		break;
	}
      eproto:
	if ((err = m_error(q, dl, EPROTO, EPROTO)) == 0)
		freemsg(mp);
	return (err);
}
static streams_noinline streams_fastcall int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_info_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_info_ack(dl, q, mp);
      badprim:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, DL_SYSERR, EMSGSIZE);
}

/**
 * dl_attach_req: - process DL_ATTACH_REQ primitive
 * @dl: the data link
 * @q: the upper write queue
 * @mp: the primitive
 *
 * When this module is pushed, the device type is a STYLE1 device an no attach or detach operation
 * is supported.  When this module is opened as a multiplexing driver, then the device type is a
 * STYLE2 device requiring an attach and detach operation.  If there is a connection manager
 * listening and the ppa is not found, cd_lookup() will request the ppa from the connection manager.
 *
 * This is a little bit different from LAPD, because LAPB only has one data link per physical link.
 * LAPD supports multiple data links per physical link.
 */
static streams_noinline streams_fastcall int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct cd *cd;

	if (!(cd = cd_lookup(p->dl_ppa)) || cd->info.cd_class == CD_NODEV)
		goto badppa;
	dl_set_state(dl, DL_ATTACH_PENDING);
	return dl_ok_ack(dl, q, mp, cd, NULL, NULL);
      badppa:
	return dl_error_ack(dl, q, mp, DL_ATTACH_REQ, DL_BADPPA, ENXIO);
}

static streams_noinline streams_fastcall int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	struct cd *cd = dl->cd;

	if (p->dl_service_mode != DL_CODLS)
		goto unsupported;
	if (cd->dl != NULL && cd->dl != dl)
		goto bound;
	if (p->dl_addr_length != 0)
		goto badaddr;
	if (p->dl_max_conind > 0)
		dl->dl_max_conind = p->dl_max_conind;
	if (p->dl_conn_mgmt)
		dl->flags |= DL_FLAG_CONNMGMT;

	if (cd_chk_state(cd, (CDF_UNUSABLE | CDF_UNATTACHED)))
		goto initfailed;
	if (cd_not_state(cd, (CDF_DISABLED))) {
		if ((dl->info.dl_min_sdu = cd->info.cd_min_sdu - (dl_exp(dl) ? 2 : 1)) < 1)
			dl->info.dl_min_sdu = 1;
	      if ((dl->info.dl_max_sdu = cd->info.cd_max_sdu - (dl_exp(dl): 2:1)) <
		    1)
			dl->info.dl_max_sdu = 1;
	}
	if (p->dl_xidtest_flg & DL_AUTO_XID)
		dl->flags |= DL_FLAG_AUTOXID;
	if (p->dl_xidtest_flg & DL_AUTO_TEST)
		dl->flags |= DL_FLAG_AUTOTEST;
	return dl_bind_ack(dl, q, mp, cd);

      badaddr:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_BADADDR, ENXIO);
      bound:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_BOUND, EADDRINUSE);
      unsupported:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_UNSUPPORTED, EINVAL);
      initfailed:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_INITFAILED, EIO);
      enodev:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_SYSERR, ENODEV);
}
static streams_noinline streams_fastcall int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_other_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_ack(q, DL_PRIV(q), *(uint32_t *) mp->b_rptr, DL_BADPRIM, EOPNOTSUPP);
}

static streams_noinline streams_fastcall int
dl_w_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	int rtn;

	dl->i_oldstate = dl->i_state;
	if (!msgdsize(mp)) {
		__swerr();
		freemsg(mp);
		return (0);
	}
	printd(("%s: %p: -> DL_DATA_REQ\n", DRV_NAME, dl));
	rtn = dl_data_req(q, mp);
	if (rtn)
		dl->i_state = dl->i_oldstate;
	return (rtn);
}

static streams_noinline streams_fastcall int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_long prim;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(dl_long))) {
		__swerr();
		freemsg(mp);
		return (0);
	}
	switch ((prim = *(dl_long *) mp->b_rptr)) {
	case DL_INFO_REQ:
		printd(("%s: %p: -> DL_INFO_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_info_req_t))
			goto badprim;
		return dl_info_req(dl, q, mp);
	case DL_ATTACH_REQ:
		printd(("%s: %p: -> DL_ATTACH_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_attach_req_t))
			goto badprim;
		if (dl_not_state(dl, DLF_UNATTACHED))
			goto outstate;
		return dl_attach_req(dl, q, mp);
	case DL_DETACH_REQ:
		printd(("%s: %p: -> DL_DETACH_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_detach_req_t))
			goto badprim;
		return dl_detach_req(dl, q, mp);
	case DL_BIND_REQ:
		printd(("%s: %p: -> DL_BIND_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_bind_req_t))
			goto badprim;
		dl_lock(dl);
		if (dl_not_state(dl, DLF_UNBOUND)) {
			dl_unlock(dl);
			goto outstate;
		}
		dl_set_state(dl, DL_BIND_PENDING);
		dl_unlock(dl);
		return dl_bind_req(dl, q, mp);
	case DL_UNBIND_REQ:
		printd(("%s: %p: -> DL_UNBIND_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_unbind_req_t))
			goto badprim;
		return dl_unbind_req(dl, q, mp);
	case DL_SUBS_BIND_REQ:
		printd(("%s: %p: -> DL_SUBS_BIND_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_subs_bind_req_t))
			goto badprim;
		return dl_subs_bind_req(dl, q, mp);
	case DL_SUBS_UNBIND_REQ:
		printd(("%s: %p: -> DL_SUBS_UNBIND_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_subs_unbind_req_t))
			goto badprim;
		return dl_subs_unbind_req(dl, q, mp);
	case DL_ENABMULTI_REQ:
		printd(("%s: %p: -> DL_ENABMULTI_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_enabmulti_req_t))
			goto badprim;
		return dl_enabmulti_req(dl, q, mp);
	case DL_DISABMULTI_REQ:
		printd(("%s: %p: -> DL_DISABMULTI_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_disabmulti_req_t))
			goto badprim;
		return dl_disabmulti_req(dl, q, mp);
	case DL_PROMISCON_REQ:
		printd(("%s: %p: -> DL_PROMISCON_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_promiscon_req_t))
			goto badprim;
		return dl_promiscon_req(dl, q, mp);
	case DL_PROMISCOFF_REQ:
		printd(("%s: %p: -> DL_PROMISCOFF_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_promiscoff_req_t))
			goto badprim;
		return dl_promiscoff_req(dl, q, mp);
	case DL_CONNECT_REQ:
		printd(("%s: %p: -> DL_CONNECT_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_connect_req_t))
			goto badprim;
		return dl_connect_req(dl, q, mp);
	case DL_TOKEN_REQ:
		printd(("%s: %p: -> DL_TOKEN_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_token_req_t))
			goto badprim;
		return dl_token_req(dl, q, mp);
	case DL_DISCONNECT_REQ:
		printd(("%s: %p: -> DL_DISCONNECT_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_disconnect_req_t))
			goto badprim;
		return dl_disconnect_req(dl, q, mp);
	case DL_RESET_REQ:
		printd(("%s: %p: -> DL_RESET_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_reset_req_t))
			goto badprim;
		return dl_reset_req(dl, q, mp);
	case DL_RESET_RES:
		printd(("%s: %p: -> DL_RESET_RES\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_reset_res_t))
			goto badprim;
		return dl_reset_res(dl, q, mp);
	case DL_UNITDATA_REQ:
		printd(("%s: %p: -> DL_UNITDATA_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_unitdata_req_t))
			goto badprim;
		return dl_unitdata_req(dl, q, mp);
	case DL_UDQOS_REQ:
		printd(("%s: %p: -> DL_UDQOS_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_udqos_req_t))
			goto badprim;
		return dl_udqos_req(dl, q, mp);
	case DL_TEST_REQ:
		printd(("%s: %p: -> DL_TEST_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_test_req_t))
			goto badprim;
		return dl_test_req(dl, q, mp);
	case DL_TEST_RES:
		printd(("%s: %p: -> DL_TEST_RES\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_test_res_t))
			goto badprim;
		return dl_test_res(dl, q, mp);
	case DL_XID_REQ:
		printd(("%s: %p: -> DL_XID_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_xid_req_t))
			goto badprim;
		return dl_xid_req(dl, q, mp);
	case DL_XID_RES:
		printd(("%s: %p: -> DL_XID_RES\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_xid_res_t))
			goto badprim;
		return dl_xid_res(dl, q, mp);
	case DL_DATA_ACK_REQ:
		printd(("%s: %p: -> DL_DATA_ACK_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_data_ack_req_t))
			goto badprim;
		return dl_data_ack_req(dl, q, mp);
	case DL_REPLY_REQ:
		printd(("%s: %p: -> DL_REPLY_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_reply_req_t))
			goto badprim;
		return dl_reply_req(dl, q, mp);
	case DL_REPLY_UPDATE_REQ:
		printd(("%s: %p: -> DL_REPLY_UPDATE_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_reply_update_req_t))
			goto badprim;
		return dl_reply_update_req(dl, q, mp);
	case DL_PHYS_ADDR_REQ:
		printd(("%s: %p: -> DL_PHYS_ADDR_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_phys_addr_req_t))
			goto badprim;
		return dl_phys_addr_req(dl, q, mp);
	case DL_SET_PHYS_ADDR_REQ:
		printd(("%s: %p: -> DL_SET_PHYS_ADDR_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_set_phys_addr_req_t))
			goto badprim;
		return dl_set_phys_addr_req(dl, q, mp);
	case DL_GET_STATISTICS_REQ:
		printd(("%s: %p: -> DL_GET_STATISTICS_REQ\n", DRV_NAME, DL_PRIV(q)));
		if (mp->b_wptr < mp->b_rptr + sizeof(dl_get_statistics_req_t))
			goto badprim;
		return dl_get_statistics_req(dl, q, mp);
	default:
		printd(("%s: %p: -> DL_?? %u ??\n", DRV_NAME, DL_PRIV(q),
			*(uint32_t *) mp->b_rptr));
		return dl_other_req(dl, q, mp);
	}
      badprim:
	return dl_error_ack(dl, q, prim, DL_SYSERR, EMSGSIZE);
      outstate:
	return dl_error_ack(dl, q, prim, DL_OUTSTATE, EPROTO);
}

static streams_noinline streams_fastcall __unlikely int
dl_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	{
		struct dl *dl = DL_PRIV(q);
		queue_t *oq;

		if ((oq = dl->oq)) {
			put(oq, mp);
			return (0);
		}
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static streams_noinline streams_fastcall __unlikely int
dl_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return dl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return dl_w_ioctl(q, mp);
	case M_FLUSH:
		return dl_w_flush(q, mp);
	default:
		return dl_w_other(q, mp);
	}
}

static streams_inline streams_fastcall __hot_write int
dl_msg(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	uchar type;

	/* FIXME: this really needs to be reworked. */
	/* optimized for data */
	if ((type = mp->b_datap->db_type) == M_DATA) {
		queue_t *oq;

		/* just pass it */
		if ((oq = dl->oq)) {
			if (bcanputnext(oq, mp->b_band)) {
				putnext(oq, mp);
				return (0);
			}
			return (-EBUSY);
		}
		/* in the wrong state, go slow */
	}
	if (type == M_PROTO || type = M_PCPROTO) {
		union DL_primitives *p = (typeof(p)) mp->b_rptr;

		dassert(mp->b_wptr >= mp->b_rptr + sizeof(p->dl_primitive));
		if (p->dl_primitive == DL_UNITDATA_REQ && mp->b_cont && msgdsize(mp->b_cont)) {
			queue_t *oq;

			/* just pass the data */
			if ((oq = dl->oq)) {
				uchar band = mp->b_band;

				if (bcanputnext(oq, band)) {
					/* strip the M_(PC)PROTO */
					freeb(XCHG(&mp, mp->b_cont));
					mp->b_band = band;
					putnext(oq, mp);
					return (0);
				}
				return (-EBUSY);
			}
			/* in the wrong state, go slow */
		}
	}
	return dl_msg_slow(q, mp);
}

/*
 *  CDI PRIMITIVE HANDLING
 *  ======================
 */
static streams_noinline streams_fastcall __unlikely int
cd_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return dl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	case M_HANGUP:
		return dl_r_hangup(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_SETOPTS:
		return dl_r_setopts(q, mp);
	default:
		return dl_r_other(q, mp);
	}
}

static streams_inline streams_fastcall __hot_read int
cd_msg(queue_t *q, mblk_t *mp)
{
	uchar type;

	/* optimized for data */
	if ((type = mp->b_datap->db_type) == M_DATA) {
		/* just pass it */
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
	if (type == M_PROTO || type == M_PCPROTO) {
		union CD_primitives *cd = (typeof(cd)) mp->b_rptr;

		dassert(mp->b_wptr >= mp->b_rptr + sizeof(cd->cd_primitive));
		if (cd->cd_primitive == CD_UNITDATA_IND && mp->b_cont && msgdsize(mp->b_cont)) {
			uchar band = mp->b_band;

			/* just pass the data */
			if (bcanputnext(q, band)) {
				/* strip the M_(PC)PROTO */
				freeb(XCHG(&mp, mp->b_cont));
				mp->b_band = band;
				putnext(q, mp);
				return (0);
			}
			return (-EBUSY);
		}
	}
	return cd_msg_slow(q, mp);
}

/*
 *  PUT and SERVICE procedures.
 *  ===========================
 */

/*
 * The cd_rput() and cd_rsrv() procedures are used if we are pushed as a module.  When opened
 * as a driver, the dl_rput() and dl_rsrv() procedures are used on the upper multiplex instead.
 */

STATIC streamscall int
cd_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || cd_msg(q, mp)) {
		if (putq(q, mp))
			return (0);
		swerr();
		freemsg(mp);
	}
	return (0);
}

STATIC streamscall int
cd_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (cd_msg(q, mp)) {
			dassert(!pcmsg(mp->b_datap->db_type));
			if (putbq(q, mp))
				break;
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

STATIC streamscall int
dl_rput(queue_t *q, mblk_t *mp)
{
	/* must never put a message to this queue, always put it to the next queue */
	swerr();
	putnext(q, mp);
	return (0);
}

STATIC streamscall int
dl_rsrv(queue_t *q)
{
	{
		mblk_t *mp;

		/* never put messages to this queue, always the next queue */
		while ((mp = getq(q))) {
			swerr();
			putnext(q, mp);
		}
	}
	{
		struct dl *dl = DL_PRIV(q);

		if (dl->oq) {
			/* backenable across the multiplexer */
			enableok(dl->oq);
			qenable(dl->oq);
			return (0);
		}
	}
}

STATIC streamscall int
cd_wput(queue_t *q, mblk_t *mp)
{
	/* never put messages to this queue, always the next queue */
	swerr();
	putnext(q, mp);
	return (0);
}

STATIC streamscall int
cd_wsrv(queue_t *q)
{
	{
		mblk_t *mp;

		/* never put messages to this queue, always the next queue */
		while ((mp = getq(q))) {
			swerr();
			putnext(q, mp);
		}
	}
	{
		struct cd *cd = CD_PRIV(q);

		if (cd->oq) {
			/* backenable across the multiplexer */
			enableok(cd->oq);
			qenable(cd->oq);
			return (0);
		}
	}
}

STATIC streamscall int
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || dl_msg(q, mp)) {
		if (putq(q, mp))
			return (0);
		swerr();
		freemsg(mp);
	}
	return (0);
}

STATIC streamscall int
dl_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (dl_msg(q, mp)) {
			dassert(!pcmsg(mp->b_datap->db_type));
			if (putbq(q, mp))
				break;
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

/*
 *  OPEN and CLOSE routines.
 *  ========================
 */
STATIC streamscall int
dl_mod_open(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct stroptions *so;
	cd_info_req_t *p;
	struct dl *dl;
	struct cd *cd;
	mblk_t *mp, *bp;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		return (ENXIO);	/* cannot be opened as driver */
	if ((mp = allocb(sizeof(*so), BPRI_WAITOK)) == NULL)
		return (ENXIO);
	if ((bp = allocb(sizeof(*p), BPRI_WAITOK)) == NULL) {
		freeb(mp);
		return (ENXIO);
	}
	if ((cd = kmem_alloc(sizeof(*cd) + sizeof(*dl), KM_SLEEP)) == NULL) {
		freeb(mp);
		freeb(bp);
		return (ENXIO);
	}
	bzero(cd, sizeof(*cd) + sizeof(*dl));
	dl = (typeof(dl)) (cd + 1);
	cd->dl = dl;
	dl->cd = cd;

	dl->info.dl_primitive = DL_INFO_ACK;
	dl->info.dl_max_sdu = 4096;
	dl->info.dl_min_sdu = 1;	/* never less than 1 */
	dl->info.dl_addr_length = 1;	/* just a SAP. */
	dl->info.dl_mac_type = DL_X25;	/* X.25 LAPB */
	dl->info.dl_reserved = 0;
	dl->info.dl_current_state = DL_UNBOUND;
	dl->info.dl_sap_length = 1;
	dl->info.dl_service_mode = DL_CODLS;
	dl->info.dl_qos_length = sizeof(dl->qos);
	dl->info.dl_qos_offset = sizeof(dl->info);
	dl->info.dl_qos_range_length = sizeof(dl->qor);
	dl->info.dl_qos_range_offset = sizeof(dl->info) + sizeof(dl->qos);
	dl->info.dl_provider_style = DL_STYLE1;
	dl->info.dl_addr_offset = 0;
	dl->info.dl_version = DL_VERSION_2;
	dl->info.dl_brdcst_addr_length = 0;
	dl->info.dl_brdcst_addr_offset = 0;
	dl->info.dl_growth = 0;

	dl->qos.dl_qos_type = DL_QOS_CO_SEL1;
	dl->qos.dl_rcv_throughput = 64000;
	dl->qos.dl_rcv_trans_delay = 0;
	dl->qos.dl_xmt_throughput = 64000;
	dl->qos.dl_xmt_trans_delay = 0;
	dl->qos.dl_priority = 0;
	dl->qos.dl_protection = DL_NONE;
	dl->qos.dl_residual_error = 0;
	dl->qos.dl_resilience = {
	0, 0};

	dl->qor.dl_qos_type = DL_QOS_CO_RANGE1;
	dl->qor.dl_rcv_throughput = {
	64000, 4800};
	dl->qor.dl_rcv_trans_delay = {
	0, 512};
	dl->qor.dl_xmt_throughput = {
	64000, 4800};
	dl->qor.dl_xmt_trans_delay = {
	0, 512};
	dl->qor.dl_priority = {
	0, 100};
	dl->qor.dl_protection = {
	DL_NONE, DL_MAXIMUM};
	dl->qor.dl_residual_error = 0;
	dl->qor.dl_resilience = {
	0, 0};

	/* until we get an ack */
	cd->info.cd_primitive = CD_INFO_ACK;
	cd->info.cd_state = -1;
	cd->info.cd_max_sdu = 4096;
	cd->info.cd_min_sdu = 1;
	cd->info.cd_class = CD_HDLC;
	cd->info.cd_duplex = CD_FULLDUPLEX;
	cd->info.cd_output_style = CD_UNACKEDOUTPUT
	    cd->info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	cd->info.cd_addr_length = 0;
	cd->info.cd_ppa_style = CD_STYLE1;

	dl->info.dl->info.cd->u.dev.cmajor = dl->u.dev.cmajor = getmajor(*devp);
	cd->u.dev.cminor = dl->u.dev.cminor = getminor(*devp);
	cd->rq = dl->rq = cd->oq = q;
	cd->wq = dl->wq = dl->oq = WR(q);
	q->q_ptr = (void *) cd;
	WR(q)->q_ptr = (void *) dl;

	so = (typeof(so)) mp->b_wptr;
#ifdef LFS
	so->so_flags |= SO_SKBUFF;
#endif				/* LFS */
	so->so_flags |= SO_WROFF;
	so->so_wroff = 12;
	/* Not really necessary for modules. */
	so->so_flags |= SO_MINPSZ;
	so->so_minpsz = dl_minfo.mi_minpsz;
	so->so_flags |= SO_MAXPSZ;
	so->so_maxpsz = dl_minfo.mi_maxpsz;
	so->so_flags |= SO_HIWAT;
	so->so_hiwat = dl_minfo.mi_hiwat;
	so->so_flags |= SO_LOWAT;
	so->so_lowat = dl_minfo.mi_lowat;
	mp->b_wptr += sizeof(*so);
	mp->b_datap->db_type = M_SETOPTS;
	putnext(q, mp);

	qprocson(q);

	/* interrogate the driver */
	p = (typeof(p)) bp->b_rptr;
	bp->b_wptr += sizeof(*p);
	p->cd_primitive = CD_INFO_REQ;
	putnext(WR(q), bp);
	return (0);
}
STATIC streamscall int
dl_mux_open(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct dl *dl, **dlp = &master.dl.list;
	unsigned long flags;
	mblk_t *mp;
	struct stroptions *so;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next)
		return (ENXIO);

	/* Linux Fast-STREAMS always passes internal major device number (module id).  Note also,
	   however, that strconf-sh attempts to allocate module ids that are identical to the base
	   major device number anyway. */
#ifdef LIS
	/* sorry, cannot open by minor device number */
	if (cmajor != CMAJOR_0 || cminor != 0) {
		ptrace(("%s: ERROR: cannot open specific minor\n", DRV_NAME));
		return (ENXIO);
	}
#endif				/* LIS */
#ifdef LFS
	/* sorry, cannot open by minor device number */
	if (cmajor != DRV_ID || cminor != 0) {
		ptrace(("%s: ERROR: cannot open specific minor\n", DRV_NAME));
		return (ENXIO);
	}
#endif				/* LFS */
	if ((mp = allocb(sizeof(*so), BPRI_WAITOK)) == NULL)
		return (ENOBUFS);
	if ((dl = kmem_alloc(sizeof(*dl), KM_SLEEP)) == NULL) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		freeb(mp);
		return (ENOMEM);
	}
	bzero(dl, sizeof(*dl));

	/* until refined */
	dl->info.dl_primitive = DL_INFO_ACK;
	dl->info.dl_max_sdu = 4096;
	dl->info.dl_min_sdu = 1;	/* never less than 1 */
	dl->info.dl_addr_length = 1;	/* just a SAP. */
	dl->info.dl_mac_type = DL_X25;	/* X.25 LAPB */
	dl->info.dl_reserved = 0;
	dl->info.dl_current_state = DL_UNATTACHED;
	dl->info.dl_sap_length = 1;
	dl->info.dl_service_mode = DL_CODLS;
	dl->info.dl_qos_length = sizeof(dl->qos);
	dl->info.dl_qos_offset = sizeof(dl->info);
	dl->info.dl_qos_range_length = sizeof(dl->qor);
	dl->info.dl_qos_range_offset = sizeof(dl->info) + sizeof(dl->qos);
	dl->info.dl_provider_style = DL_STYLE2;
	dl->info.dl_addr_offset = 0;
	dl->info.dl_version = DL_VERSION_2;
	dl->info.dl_brdcst_addr_length = 0;
	dl->info.dl_brdcst_addr_offset = 0;
	dl->info.dl_growth = 0;

	dl->qos.dl_qos_type = DL_QOS_CO_SEL1;
	dl->qos.dl_rcv_throughput = 64000;
	dl->qos.dl_rcv_trans_delay = 0;
	dl->qos.dl_xmt_throughput = 64000;
	dl->qos.dl_xmt_trans_delay = 0;
	dl->qos.dl_priority = 0;
	dl->qos.dl_protection = DL_NONE;
	dl->qos.dl_residual_error = 0;
	dl->qos.dl_resilience = {
	0, 0};

	dl->qor.dl_qos_type = DL_QOS_CO_RANGE1;
	dl->qor.dl_rcv_throughput = {
	64000, 4800};
	dl->qor.dl_rcv_trans_delay = {
	0, 512};
	dl->qor.dl_xmt_throughput = {
	64000, 4800};
	dl->qor.dl_xmt_trans_delay = {
	0, 512};
	dl->qor.dl_priority = {
	0, 100};
	dl->qor.dl_protection = {
	DL_NONE, DL_MAXIMUM};
	dl->qor.dl_residual_error = 0;
	dl->qor.dl_resilience = {
	0, 0};

	cminor = 1;
	write_lock_str(&master.lock, flags);
	for (; *dlp; dlp = &(*dlp)->next) {
		if (cmajor != (*dlp)->u.dev.cmajor)
			break;
		if (cmajor == (*dlp)->u.dev.cmajor) {
			if (cminor < (*dlp)->u.dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = dl_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		write_unlock_str(&master.lock, flags);
		kmem_free(dl, sizeof(*dl));
		freeb(mp);
		return (ENXIO);
	}
	_printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	dl->u.dev.cmajor = cmajor;
	dl->u.dev.cminor = cminor;
	dl->rq = q;
	dl->wq = WR(q);
	dl->next = *dlp;
	*dlp = dl;
	write_unlock_str(&master.lock, flags);

	q->q_ptr = WR(q)->q_ptr = (void *) dl;

	so = (typeof(so)) mp->b_wptr;
#ifdef LFS
	so->so_flags |= SO_SKBUFF;
#endif				/* LFS */
	so->so_flags |= SO_WROFF;
	so->so_wroff = 12;
	/* Not really necessary for modules. */
	so->so_flags |= SO_MINPSZ;
	so->so_minpsz = dl_minfo.mi_minpsz;
	so->so_flags |= SO_MAXPSZ;
	so->so_maxpsz = dl_minfo.mi_maxpsz;
	so->so_flags |= SO_HIWAT;
	so->so_hiwat = dl_minfo.mi_hiwat;
	so->so_flags |= SO_LOWAT;
	so->so_lowat = dl_minfo.mi_lowat;
	mp->b_wptr += sizeof(*so);
	mp->b_datap->db_type = M_SETOPTS;
	putnext(q, mp);
	qprocson(q);
	return (0);
}
STATIC streamscall int
dl_mod_close(queue_t *q, dev_t *devp, int oflags)
{
	struct cd *cd = CD_PRIV(q);

	_printd(("%s: closing character device %d:%d\n", DRV_NAME, cd->u.dev.cmajor,
		 cd->u.dev.cminor));
#ifdef LIS
	/* protect agains LIS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		return (0);
	}
	if (q->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			DRV_NAME, __FUNCTION__);
	}
	/* make sure procedures are off */
	qprocsoff(q);
	q->q_ptr = WR(q)->q_ptr = NULL;
	kmem_free(cd, sizeof(*cd) + sizeof(*cd->dl));
	return (0);
}
STATIC streamscall int
dl_mux_close(queue_t *q, dev_t *devp, int oflags)
{
	struct dl *dl = DL_PRIV(q);

	_printd(("%s: closing character device %d:%d\n", DRV_NAME, dl->u.dev.cmajor,
		 dl->u.dev.cminor));
#ifdef LIS
	/* protect agains LIS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		return (0);
	}
	if (q->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			DRV_NAME, __FUNCTION__);
	}
	/* make sure procedures are off */
	qprocsoff(q);
	q->q_ptr = WR(q)->q_ptr = NULL;
	kmem_free(dl, sizeof(*dl));
	return (0);
}

/*
 *  STREAMS Definitions
 *  ===================
 */

/*
 *  Module
 *  ------
 */

static struct module_stat dl_mod_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_mod_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit dl_mod_rinit = {
	.qi_putp = cd_rput,
	.qi_srvp = cd_rsrv,
	.qi_qopen = dl_mod_open,
	.qi_qclose = dl_mod_close,
	.qi_minfo = &dl_mod_minfo,
	.qi_mstat = &dl_mod_rstat,
};
static struct qinit dl_mod_winit = {
	.qi_putp = dl_wput,
	.qi_srvp = dl_wsrv,
	.qi_minfo = &dl_mod_minfo,
	.qi_mstat = &dl_mod_wstat,
};

MODULE_STATIC struct streamtab lapbmodinfo = {
	.st_rdinit = &dl_mod_rinit,
	.st_wrinit = &dl_mod_winit,
};

/*
 *  Upper Multiplex.
 *  ----------------
 *  Provides a DLPI interface at the upper multiplex.  This can also be pushed
 *  as a module that presents a DLPI upper service interface.
 */

static struct module_stat dl_mux_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_mux_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit dl_mux_rinit = {
	.qi_putp = dl_rput,
	.qi_srvp = dl_rsrv,
	.qi_qopen = dl_mux_open,
	.qi_qclose = dl_mux_close,
	.qi_minfo = &dl_mux_minfo,
	.qi_mstat = &dl_mux_rstat,
};
static struct qinit dl_mux_winit = {
	.qi_putp = dl_wput,
	.qi_srvp = dl_wsrv,
	.qi_minfo = &dl_mux_minfo,
	.qi_mstat = &dl_mux_wstat,
};

/*
 *  Lower Multiplex.
 *  ----------------
 *  Uses a CDI Interface at the lower multiplex.  This can also be pushed as a
 *  module that uses the CDI lower service interface.
 */

static struct module_stat cd_mux_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat cd_mux_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit cd_mux_rinit = {
	.qi_putp = cd_rput,
	.qi_srvp = cd_rsrv,
	.qi_minfo = &dl_mux_minfo,
	.qi_mstat = &cd_mux_rstat,
};
static struct qinit cd_mux_winit = {
	.qi_putp = cd_wput,
	.qi_srvp = cd_wsrv,
	.qi_minfo = &dl_mux_minfo,
	.qi_mstat = &cd_mux_wstat,
};

MODULE_STATIC struct streamtab lapbdrvinfo = {
	.st_rdinit = &dl_mux_rinit,
	.st_wrinit = &dl_mux_winit,
	.st_muxrinit = &cd_mux_rinit,
	.st_muxwinit = &cd_mux_winit,
};

/*
 *  STREAMS Registration.
 *  =====================
 */
#ifdef LIS
#define fmodsw _fmodsw
#endif

modID_t modid = MOD_ID;
major_t major = CMAJOR_0;

STATIC struct fmodsw dl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &lapbmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC __unlikely int
dl_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&dl_fmod)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
dl_unregister_strmod(void)
{
	int err;

	if ((err = unregister_strmod(&dl_fmod)) < 0)
		return (err);
	return (0);
}

modID_t drvid = DRV_ID;
major_t major = DRV_CMAJOR_0;

STATIC struct cdevsw dl_cdev = {
	.d_name = DRV_NAME,
	.d_str = &lapbdrvinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC __unlikely int
dl_register_strdev(major_t major)
{
	int err;

	if ((err = regsiter_strdev(&dl_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
dl_unregsiter_strdev(major_t major)
{
	int err;

	if (major && (err = unregister_strdev(&dl_cdev, major)) < 0)
		return (err);
	return (0);
}

/*
 *  Linux Registration.
 *  ===================
 */
#ifdef module_param
module_param(modid, ushort, 0);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for the LAPB module. (0 for allocation.)");

#ifdef module_param
module_param(drvid, ushort, 0);
#else				/* module_param */
MODULE_PARM(drvid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(drvid, "Module ID for the LAPB driver. (0 for allocation.)");

#ifdef module_param
module_param(major, uint, 0);
#else				/* module_param */
MODULE_PARM(major, "h");
#endif				/* module_param */
MODULE_PARM_DESC(major, "Device number for the LAPB driver. (0 for allocation.)");

MODULE_STATIC void __exit
lapbmodterminate(void)
{
	int err;

	if ((err = dl_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", DRV_NAME);
	if ((err = dl_term_mod_caches()))
		cmn_err(CE_WARN, "%s: could not terminate module caches", DRV_NAME);
	return;
}

MODULE_STATIC void __exit
lapbdrvterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (dl_majors[mindex]) {
			if ((err = dl_unregsiter_strdev(dl_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregsiter major %d", DRV_NAME,
					dl_majors[mindex]);
			if (mindex)
				dl_majors[mindex] = 0;
		}
	}
	if ((err = dl_term_drv_caches()))
		cmn_err(CE_WARN, "%s: could not terminate driver caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
lapbmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* console splash */
	if ((err = dl_init_mod_caches())) {
		cmn_err(CE_WARN, "%s: could not init module caches, err = %d", DRV_NAME, err);
		lapbmodterminate();
		return (err);
	}
	if ((err = dl_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", DRV_NAME, err);
		lapbmodterminate();
		return (err);
	}
	return (0);
}

MODULE_STATIC int __init
lapbdrvinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = dl_init_drv_caches())) {
		cmn_err(CE_WARN, "%s: could not init driver caches, err = %d", DRV_NAME, err);
		lapbdrvterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = dl_register_strdev(dl_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					dl_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				lapbterminate();
				return (err);
			}
		}
		if (dl_majors[mindex] == 0)
			dl_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(dl_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = dl_majors[0];
	}
	return (0);
}

MODULE_STATIC void
lapbterminate(void)
{
	lapbdrvterminate();
	lapbmodterminate();
}

MODULE_STATIC int
lapbinit(void)
{
	int err;

	if ((err = lapbmodinit())) {
		return (err);
	}
	if ((err = lapbdrvinit())) {
		lapbmodterminate();
		return (err);
	}
	return (0);
}

module_init(lapbinit);
module_exit(lapbterminate);
