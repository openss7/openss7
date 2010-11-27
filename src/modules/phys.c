/*****************************************************************************

 @(#) $RCSfile: phys.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:40:33 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2009-06-21 11:40:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: phys.c,v $
 Revision 1.1.2.1  2009-06-21 11:40:33  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: phys.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:40:33 $";


/*
 * This is an ATM PHYS MDIF pushable module.  The driver pushes over a CHI Stream that represents a
 * steady stream of PDH or SDH bits that contains ATM 53 octet cells.  The module is responsible for
 * extracting, deliminating, flow control and rate adaptiion for the cell.  The upper interface is a
 * CDI interface that provides and accepts a Stream of 53 octet cells.
 *
 * The simple purpose of this module is to extract 53-octet ATM cells from the byte-stream presented
 * at its lower layer.  This is performed using the procedures defined in ITU-T Recommednation I.432.
 * The purpose is to adapt ATM cells into PDH facilities (full or fractional T1 or E1) in accordance
 * with ITU-T Recommendation G.804 and AF-PHYS-00016.
 *
 * In general, an MXI stream is opened on a driver such as the V401P-MX driver, and linked beneath
 * the MATRIX muliplexing driver.  A CHI stream is opened on the MATRIX driver and attached to the
 * E1 or T1 MXI stream and connected to the full or fractional statistically multiplexed facility.
 * This module is then pushed over the CHI stream.  This module syncrhonizes with the incoming
 * cells and begins generation of unused or unavailable outgoing cells.  Because G.804 and
 * AF-PHYS-00016 require octet alignment between the E1 or T1 frame and the ATM cell, the HEC and
 * header syncrhonization function of I.432.2 can be on an octet-by-octet basis rather than a
 * bit-by-bit basis.
 */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>
#include <sys/strsun.h>
#include <sys/strconf.h>

#include <sys/cdi.h>
#include <sys/chi.h>

#define PHYS_DESCRIP	"MTP3B-PHYS STREAMS MODULE."
#define PHYS_REVISION	"OpenSS7 $RCSfile: phys.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:40:33 $"
#define PHYS_COPYRIGHT	"Copyright (c) 2008-2010  Monavacon Limited.  All Rights Reserved."
#define PHYS_DEVICE	"Provides OpenSS7 MTP3B-I.432.3-PHYS module."
#define PHYS_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define PHYS_LICENSE	"GPL"
#define PHYS_BANNER	PHYS_DESCRIP	"\n" \
			PHYS_REVISION	"\n" \
			PHYS_COPYRIGHT	"\n" \
			PHYS_DEVICE	"\n" \
			PHYS_CONTACT	"\n"
#define PHYS_SPLASH	PHYS_DEVICE	" - " \
			PHYS_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(PHYS_CONTACT);
MODULE_DESCRIPTION(PHYS_DESCRIP);
MODULE_SUPPORTED_DEVICE(PHYS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(PHYS_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-phys");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define PHYS_MOD_ID	CONFIG_STREAMS_PHYS_MODID
#define PHYS_MOD_NAME	CONFIG_STREAMS_PHYS_NAME

#define MOD_ID		PHYS_MOD_ID
#define MOD_NAME	PHYS_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	PHYS_BANNER
#else				/* MODULE */
#define MOD_BANNER	PHYS_SPLASH
#endif				/* MODULE */

/*
 * PRIVATE STRUCTURE
 */

struct state {
	uint32_t state;
	uint32_t pending;
	uint32_t conn_tx;
	uint32_t conn_rx;
};

struct cd;
struct ch;

struct cd {
	struct ch *ch;
	queue_t *oq;
	queue_t *iq;
	struct state state, oldstate;
	cd_info_ack_t info;
	uint32_t ppa;
};

struct ch {
	struct cd *cd;
	queue_t *iq;
	queue_t *oq;
	struct state state, oldstate;
	struct CH_info_ack info;
	uint32_t ppa;
	mblk_t *dmsg;
	size_t dlen;
};

struct priv {
	queue_t *rq;
	queue_t *wq;
	struct cd cd;
	struct ch ch;
};

static rwlock_t ph_lock = RW_LOCK_UNLOCKED;
static caddr_t ph_opens;

/*
 * PRINTING STUFF
 */

/*
 * Printing primitives
 */

/* Upper */
static inline fastcall const char *
cd_primname(const cd_ulong prim)
{
	switch (prim) {
	case CD_INFO_REQ:
		return ("CD_INFO_REQ");
	case CD_INFO_ACK:
		return ("CD_INFO_ACK");
	case CD_ATTACH_REQ:
		return ("CD_ATTACH_REQ");
	case CD_DETACH_REQ:
		return ("CD_DETACH_REQ");
	case CD_ENABLE_REQ:
		return ("CD_ENABLE_REQ");
	case CD_DISABLE_REQ:
		return ("CD_DISABLE_REQ");
	case CD_OK_ACK:
		return ("CD_OK_ACK");
	case CD_ERROR_ACK:
		return ("CD_ERROR_ACK");
	case CD_ENABLE_CON:
		return ("CD_ENABLE_CON");
	case CD_DISABLE_CON:
		return ("CD_DISABLE_CON");
	case CD_ERROR_IND:
		return ("CD_ERROR_IND");
	case CD_ALLOW_INPUT_REQ:
		return ("CD_ALLOW_INPUT_REQ");
	case CD_READ_REQ:
		return ("CD_READ_REQ");
	case CD_UNITDATA_REQ:
		return ("CD_UNITDATA_REQ");
	case CD_WRITE_READ_REQ:
		return ("CD_WRITE_READ_REQ");
	case CD_UNITDATA_ACK:
		return ("CD_UNITDATA_ACK");
	case CD_UNITDATA_IND:
		return ("CD_UNITDATA_IND");
	case CD_HALT_INPUT_REQ:
		return ("CD_HALT_INPUT_REQ");
	case CD_ABORT_OUTPUT_REQ:
		return ("CD_ABORT_OUTPUT_REQ");
	case CD_MUX_NAME_REQ:
		return ("CD_MUX_NAME_REQ");
	case CD_BAD_FRAME_IND:
		return ("CD_BAD_FRAME_IND");
	case CD_MODEM_SIG_REQ:
		return ("CD_MODEM_SIG_REQ");
	case CD_MODEM_SIG_IND:
		return ("CD_MODEM_SIG_IND");
	case CD_MODEM_SIG_POLL:
		return ("CD_MODEM_SIG_POLL");
	default:
		return ("(unknown)");
	}
}

/* Lower */
static inline fastcall const char *
ch_primname(const ch_ulong prim)
{
	switch (prim) {
	case CH_INFO_REQ:
		return ("CH_INFO_REQ");
	case CH_OPTMGMT_REQ:
		return ("CH_OPTMGMT_REQ");
	case CH_ATTACH_REQ:
		return ("CH_ATTACH_REQ");
	case CH_ENABLE_REQ:
		return ("CH_ENABLE_REQ");
	case CH_CONNECT_REQ:
		return ("CH_CONNECT_REQ");
	case CH_DATA_REQ:
		return ("CH_DATA_REQ");
	case CH_DISCONNECT_REQ:
		return ("CH_DISCONNECT_REQ");
	case CH_DISABLE_REQ:
		return ("CH_DISABLE_REQ");
	case CH_DETACH_REQ:
		return ("CH_DETACH_REQ");
	case CH_INFO_ACK:
		return ("CH_INFO_ACK");
	case CH_OPTMGMT_ACK:
		return ("CH_OPTMGMT_ACK");
	case CH_OK_ACK:
		return ("CH_OK_ACK");
	case CH_ERROR_ACK:
		return ("CH_ERROR_ACK");
	case CH_ENABLE_CON:
		return ("CH_ENABLE_CON");
	case CH_CONNECT_CON:
		return ("CH_CONNECT_CON");
	case CH_DATA_IND:
		return ("CH_DATA_IND");
	case CH_DISCONNECT_IND:
		return ("CH_DISCONNECT_IND");
	case CH_DISCONNECT_CON:
		return ("CH_DISCONNECT_CON");
	case CH_DISABLE_IND:
		return ("CH_DISABLE_IND");
	case CH_DISABLE_CON:
		return ("CH_DISABLE_CON");
	case CH_EVENT_IND:
		return ("CH_EVENT_IND");
	default:
		return ("(unknown)");
	}
}

/*
 * Printing States
 */

/* Upper */
static inline fastcall const char *
cd_statename(const cd_ulong state)
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
	case CD_NOT_AUTH:
		return ("CD_NOT_AUTH");
	default:
		return ("(unknown)");
	}
}

/* Lower */
static inline fastcall const char *
ch_statename(const ch_ulong state)
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
		return ("CHS_WACK_UREQ");
	case CHS_ATTACHED:
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
		return ("(unknown)");
	}
}

/*
 * STATE CHANGES
 */

static inline fastcall cd_ulong
cd_get_state(struct cd *cd)
{
	return cd->state.state;
}
static inline fastcall cd_ulong
cd_set_state(struct cd *cd, const cd_ulong newstate)
{
	const cd_ulong oldstate = cd_get_state(cd);

	if (likely(oldstate != newstate)) {
		mi_strlog(cd->iq, STRLOGST, SL_TRACE, "%s <- %s", cd_statename(newstate),
			  cd_statename(oldstate));
		cd->state.state = newstate;
		cd->info.cd_state = newstate;
	}
	return (newstate);
}
static inline fastcall cd_ulong
cd_save_state(struct cd *cd)
{
	cd->oldstate = cd->state;
	return (cd->state.state);
}
static inline fastcall cd_ulong
cd_restore_state(struct cd *cd)
{
	cd->state = cd->oldstate;
	return (cd->info.cd_state = cd->state.state);
}
static inline fastcall cd_ulong
cd_set_pending(struct cd *cd, const cd_ulong pending)
{
	return (cd->state.pending = pending);
}
static inline fastcall cd_ulong
cd_get_pending(struct cd *cd)
{
	return cd->state.pending;
}
static inline fastcall cd_ulong
cd_clear_pending(struct cd *cd)
{
	const cd_ulong pending = cd->state.pending;

	cd->state.pending = -1U;
	return (pending);
}

static inline fastcall ch_ulong
ch_get_state(struct ch *ch)
{
	return ch->state.state;
}
static inline fastcall ch_ulong
ch_set_state(struct ch *ch, const ch_ulong newstate)
{
	const ch_ulong oldstate = ch_get_state(ch);

	if (likely(oldstate != newstate)) {
		mi_strlog(ch->iq, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
		ch->state.state = newstate;
		ch->info.ch_state = newstate;
	}
	return (newstate);
}
static inline fastcall ch_ulong
ch_save_state(struct ch *ch)
{
	ch->oldstate = ch->state;
	return (ch->state.state);
}
static inline fastcall ch_ulong
ch_restore_state(struct ch *ch)
{
	ch->state = ch->oldstate;
	return (ch->info.ch_state = ch->state.state);
}
static inline fastcall ch_ulong
ch_set_pending(struct ch *ch, const ch_ulong pending)
{
	return (ch->state.pending = pending);
}
static inline fastcall ch_ulong
ch_get_pending(struct ch *ch)
{
	return ch->state.pending;
}
static inline fastcall ch_ulong
ch_clear_pending(struct ch *ch)
{
	const ch_ulong pending = ch->state.pending;

	ch->state.pending = -1U;
	return (pending);
}

/*
 * MESSAGE LOGGING
 */

#define STRLOGERR	0	/* log error information */
#define STRLOGNO	0	/* log notice information */
#define STRLOGST	1	/* log state transitions */
#define STRLOGTO	2	/* log timeouts */
#define STRLOGRX	3	/* log primitives received */
#define STRLOGTX	4	/* log primitives issued */
#define STRLOGTE	5	/* log timer events */
#define STRLOGIO	6	/* log additional data */
#define STRLOGDA	7	/* log data */

static inline fastcall void
ph_stripmsg(mblk_t *msg, mblk_t *dp)
{
	mblk_t *b, *b_next = msg;

	while ((b = b_next) && b != dp) {
		b_next = b->b_next;
		freeb(b);
	}
	return;
}

noinline fastcall int
cd_txprim(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *mp, cd_ulong prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(cd->oq, mp->b_band))) {
		int level;

		switch (__builtin_expect(prim, CD_UNITDATA_REQ)) {
		case CD_UNITDATA_REQ:
		case CD_WRITE_READ_REQ:
		case CD_READ_REQ:
			level = STRLOGDA;
			break;
		default:
			level = STRLOGTX;
			break;
		}
		mi_strlog(cd->oq, level, SL_TRACE, "<- %s", cd_primname(prim));
		ph_stripmsg(msg, mp->b_cont);
		putnext(cd->oq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}

noinline fastcall int
ch_txprim(struct ch *ch, queue_t *q, mblk_t *msg, mblk_t *mp, ch_ulong prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(ch->oq, mp->b_band))) {
		int level;

		switch (__builtin_expect(prim, CH_DATA_REQ)) {
		case CH_DATA_REQ:
		case CH_DATA_IND:
			level = STRLOGDA;
			break;
		default:
			level = STRLOGTX;
			break;
		}
		mi_strlog(ch->oq, level, SL_TRACE, "%s ->", ch_primname(prim));
		ph_stripmsg(msg, mp->b_cont);
		putnext(ch->oq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}

noinline fastcall void
cd_rxprim(struct cd *cd, queue_t *q, mblk_t *mp, cd_ulong prim)
{
	int level;

	switch (__builtin_expect(prim, CD_UNITDATA_IND)) {
	case CD_UNITDATA_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(cd->iq, level, SL_TRACE, "-> %s", cd_primname(prim));
	return;
}

noinline fastcall void
ch_rxprim(struct ch *ch, queue_t *q, mblk_t *mp, ch_ulong prim)
{
	int level;

	switch (__builtin_expect(prim, CH_DATA_IND)) {
	case CH_DATA_IND:
	case CH_DATA_REQ:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(ch->iq, level, SL_TRACE, "%s <-", ch_primname(prim));
	return;
}

/*
 * CD PROVIDER TO CD USER PRIMITIVES
 */

/**
 * cd_info_ack: - issue CD_INFO_ACK upstream
 * @cd: CD private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_info_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_ACK;
		p->cd_state = cd->info.cd_state;
		p->cd_max_sdu = cd->info.cd_max_sdu;
		p->cd_min_sdu = cd->info.cd_min_sdu;
		p->cd_class = cd->info.cd_class;
		p->cd_duplex = cd->info.cd_duplex;
		p->cd_output_style = cd->info.cd_output_style;
		p->cd_features = cd->info.cd_features;
		p->cd_addr_length = cd->info.cd_addr_length;
		p->cd_ppa_style = cd->info.cd_ppa_style;
		mp->b_wptr += sizeof(*p);
		cd_clear_pending(cd);
		return cd_txprim(cd, q, msg, mp, CD_INFO_ACK);
	}
	return (-ENOBUFS);
}

/**
 * cd_ok_ack: - issue CD_OK_ACK upstream
 * @cd: CD private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static inline fastcall int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong prim)
{
	cd_ok_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	cd_ulong state;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		switch ((state = cd_get_state(cd))) {
		case CD_UNATTACHED:
			prim = CD_ATTACH_REQ;
			state = CD_DISABLED;
			break;
		case CD_DISABLED:
			prim = CD_DETACH_REQ;
			state = CD_UNATTACHED;
			break;
		default:
			switch (prim) {
			case CD_ALLOW_INPUT_REQ:
				state = CD_INPUT_ALLOWED;
				break;
			case CD_HALT_INPUT_REQ:
				state = CD_ENABLED;
				break;
			default:
				goto outstate;
			}
			break;
		}
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_OK_ACK;
		p->cd_state = cd_set_state(cd, state);
		p->cd_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_OK_ACK);
	}
	return (-ENOBUFS);
      outstate:
	freeb(mp);
	return (-EFAULT);
}

/**
 * cd_error_ack: - issue CD_ERROR_ACK primitive upstream
 * @cd: CD private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: positive CDI error or negative UNIX error
 */
static inline fastcall int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *msg, ch_ulong prim, ch_long error)
{
	cd_error_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	cd_ulong state;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		switch ((state = cd_get_state(cd))) {
		case CD_UNATTACHED:
			state = CD_UNATTACHED;
			break;
		case CD_DISABLED:
			state = CD_DISABLED;
			break;
		case CD_ENABLE_PENDING:
			state = CD_DISABLED;
			break;
		case CD_DISABLE_PENDING:
			state = CD_ENABLED;
			break;
		default:
			goto outstate;
		}
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ERROR_ACK;
		p->cd_state = cd_set_state(cd, state);
		p->cd_error_primitive = prim;
		p->cd_errno = error < 0 ? CD_SYSERR : error;
		p->cd_explanation = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_ERROR_ACK);
	}
	return (-ENOBUFS);
      outstate:
	freeb(mp);
	return (-EFAULT);
}
static inline fastcall int
cd_error(struct cd *cd, queue_t *q, mblk_t *msg, unsigned char rerror, unsigned char werror)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		mp->b_wptr[0] = rerror;
		mp->b_wptr[1] = rerror;
		mp->b_wptr += 2;
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
noinline fastcall __unlikely int
cd_reply_error(struct cd *cd, queue_t *q, mblk_t *msg, const cd_ulong prim, const cd_long error)
{
	switch (error) {
	case 0:
		freemsg(msg);
		return (0);
	case -EAGAIN:
	case -EDEADLK:
	case -ENOBUFS:
	case -ENOMEM:
	case -EBUSY:
		return (error);
	}
	switch (prim) {
	case CD_INFO_REQ:
		goto ack;
	case CD_ATTACH_REQ:
		goto ack;
	case CD_DETACH_REQ:
		goto ack;
	case CD_ENABLE_REQ:
		goto ack;
	case CD_DISABLE_REQ:
		goto ack;
	case CD_ALLOW_INPUT_REQ:
		goto ack;
	case CD_READ_REQ:
		goto ack;
	case CD_UNITDATA_REQ:
		goto error;
	case CD_WRITE_READ_REQ:
		goto ack;
	case CD_HALT_INPUT_REQ:
		goto ack;
	case CD_ABORT_OUTPUT_REQ:
		goto ack;
	case CD_MUX_NAME_REQ:
		goto ack;
	case CD_MODEM_SIG_REQ:
		goto ack;
	case CD_MODEM_SIG_POLL:
		goto ack;
	default:
		goto error;
	}
      error:
	return cd_error(cd, q, msg, EPROTO, EPROTO);
      ack:
	return cd_error_ack(cd, q, msg, prim, error);
}
static inline fastcall int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_enable_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	cd_ulong state;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		switch (__builtin_expect((state = cd_get_state(cd)), CD_ENABLE_PENDING)) {
		case CD_ENABLE_PENDING:
			state = CD_INPUT_ALLOWED;
			break;
		default:
			goto outstate;
		}
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ENABLE_CON;
		p->cd_state = cd_set_state(cd, state);
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_ENABLE_CON);
	}
	return (-ENOBUFS);
      outstate:
	freeb(mp);
	return (-EFAULT);
}
static inline fastcall int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_disable_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	cd_ulong state;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		switch (__builtin_expect((state = cd_get_state(cd)), CD_DISABLE_PENDING)) {
		case CD_DISABLE_PENDING:
			state = CD_DISABLED;
			break;
		default:
			goto outstate;
		}
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DISABLE_CON;
		p->cd_state = cd_set_state(cd, CD_DISABLED);
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_DISABLE_CON);
	}
	return (-ENOBUFS);
      outstate:
	freeb(mp);
	return (-EFAULT);
}
static inline fastcall int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *msg, ch_ulong state, ch_long error)
{
	cd_error_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ERROR_IND;
		p->cd_state = cd_set_state(cd, state);
		p->cd_errno = error < 0 ? CD_SYSERR : error;
		p->cd_explanation = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_ERROR_IND);
	}
	return (-ENOBUFS);
}

/**
 * cd_unitdata_ack: - issue CD_UNITDATA_ACK primitive upstream
 * @cd: CD private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 *
 * Acknowledgements are only for the purpose of pacing output.  An acknowledgement is set whenever
 * there was no room available to accept additional cells before but there is now room available to
 * accept additional cells.  Note that the upper layer module can also rely on back-enabling (normal
 * STREAMS flow control) for this as well and can ignore these acknowledgements.
 */
static inline fastcall int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		/* expedite these somewhat */
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_ACK;
		p->cd_state = cd_get_state(cd);
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_UNITDATA_ACK);
	}
	return (-ENOBUFS);
}

/**
 * cd_unitdata_ind: - issue CD_UNITDATA_IND primitive upstream
 * @cd: CD private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @priority: message priority
 * @dp: user data (53-octet ATM cell)
 *
 * This is the less preferred way of indicating to the ATM layer received ATM cells with valid and
 * permissible ATM headers.  Note that only ATM layer cells are indicated in this fashion (i.e. not
 * OAM cells).  Cells not passed to the ATM layer are handled by this module.
 *
 * The preferred method for passing 53-octet ATM cells to the ATM layer is using a single M_DATA
 * message block.
 */
static inline fastcall int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_ushort priority, mblk_t *dp)
{
	cd_unitdata_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_IND;
		p->cd_state = cd_get_state(cd);
		p->cd_src_addr_length = 0;
		p->cd_src_addr_offset = 0;
		p->cd_addr_type = CD_IMPLICIT;
		p->cd_priority = priority;
		p->cd_dest_addr_length = 0;
		p->cd_dest_addr_offset = 0;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		return cd_txprim(cd, q, msg, mp, CD_UNITDATA_IND);
	}
	return (-ENOBUFS);
}

/**
 * cd_bad_frame_ind: - issue a CD_BAD_FRAME_IND upstream
 * @cd: CD private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: the ATM cell in error
 *
 * The only error condition for which an ATM cell can be provided as a bad frame indication is when
 * the HEC fails (but when the ATM SAR is synchornized on ATM cell headers).  In this case, the ATM
 * cell is indicated as a CD_BAD_CRC CD_BAD_FRAME_IND instead of an M_DATA indication.
 */
static inline fastcall int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	cd_bad_frame_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_BAD_FRAME_IND;
		p->cd_state = cd_get_state(cd);
		p->cd_error = CD_BAD_CRC;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		return cd_txprim(cd, q, msg, mp, CD_BAD_FRAME_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong sigs)
{
	cd_modem_sig_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_IND;
		p->cd_sigs = sigs;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_MODEM_SIG_IND);
	}
	return (-ENOBUFS);
}

/*
 * CH USER TO CH PROVIDER PRIMITIVES
 */

static inline fastcall int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_info_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		return ch_txprim(ch, q, msg, mp, CH_INFO_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_optmgmt_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_REQ;
		mp->b_wptr += sizeof(*p);
		return ch_txprim(ch, q, msg, mp, CH_OPTMGMT_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong ppa)
{
	struct CH_attach_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(ppa);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ATTACH_REQ;
		p->ch_addr_length = sizeof(ppa);
		p->ch_addr_offset = sizeof(*p);
		p->ch_flags = CHF_BOTH_DIR;
		mp->b_wptr += sizeof(*p);
		bcopy(&ppa, mp->b_wptr, sizeof(ppa));
		mp->b_wptr += sizeof(ppa);
		ch_set_state(ch, CHS_WACK_AREQ);
		return ch_txprim(ch, q, msg, mp, CH_ATTACH_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_WCON_EREQ);
		return ch_txprim(ch, q, msg, mp, CH_ENABLE_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_connect_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_CONNECT_REQ;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_WCON_CREQ);
		return ch_txprim(ch, q, msg, mp, CH_CONNECT_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_data_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DATA_REQ;
		mp->b_wptr += sizeof(*p);
		return ch_txprim(ch, q, msg, mp, CH_DATA_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_REQ;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_WCON_DREQ);
		return ch_txprim(ch, q, msg, mp, CH_DISCONNECT_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		return ch_txprim(ch, q, msg, mp, CH_DISABLE_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_detach_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_WACK_UREQ);
		return ch_txprim(ch, q, msg, mp, CH_DETACH_REQ);
	}
	return (-ENOBUFS);
}

noinline fastcall __unlikely int
ch_reply_error(struct ch *ch, queue_t *q, mblk_t *msg, const ch_ulong prim, const ch_long error)
{
	switch (error) {
	case 0:
		freemsg(msg);
		return (0);
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
		return (error);
	}
	/* FIXME: need to log error */
	freemsg(msg);
	return (0);
}

/*
 * CD USER TO CD PROVIDER PRIMITIVES
 */

noinline fastcall __unlikely int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}

/**
 * cd_attach_req: - process CD_ATTACH_REQ primitive
 * @cd: CD private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the CD_ATTACH_REQ primitive
 *
 * Requests that the Stream be attached to a 32-bit PPA address.  This 32-bit PPA address is the
 * same as the 32-bit PPA address used by the CH Stream beneath.  If the CD interface is in the
 * correct state, simply pass the request on setting the state to CD_UNATTACHED.
 */
noinline fastcall __unlikely int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(cd_get_state(cd) != CD_UNATTACHED))
		goto outstate;
	if (unlikely(cd->info.cd_ppa_style != CD_STYLE2))
		goto notsupp;
	/* remember PPA for later */
	cd->ppa = p->cd_ppa;
	cd_set_state(cd, CD_UNATTACHED);
	cd_set_pending(cd, CD_ATTACH_REQ);
	return ch_attach_req(cd->ch, q, mp, cd->ppa);
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}

noinline fastcall __unlikely int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(cd_get_state(cd) != CD_DISABLED))
		goto outstate;
	if (unlikely(cd->info.cd_ppa_style != CD_STYLE2))
		goto notsupp;
	cd_set_state(cd, CD_DISABLED);
	cd_set_pending(cd, CD_DETACH_REQ);
	return ch_detach_req(cd->ch, q, mp);
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}

noinline fastcall __unlikely int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(cd_get_state(cd) != CD_DISABLED))
		goto outstate;
	cd_set_state(cd, CD_ENABLE_PENDING);
	cd_set_pending(cd, CD_ENABLE_REQ);
	return ch_enable_req(cd->ch, q, mp);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch (__builtin_expect(cd_get_state(cd), CD_ENABLED)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
		break;
	default:
		goto outstate;
	}
	cd_set_state(cd, CD_DISABLE_PENDING);
	cd_set_pending(cd, CD_DISABLE_REQ);
	return ch_disconnect_req(cd->ch, q, mp, CHF_BOTH_DIR, -1U);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}

/**
 * cd_allow_input_req: - process CD_ALLOW_INPUT_REQ message
 * @cd: CD private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the CD_ALLOW_INPUT_REQ primitive
 *
 * The CD_ALLOW_INPUT_REQ primitive translates to a CH_CONNECT_REQ primitive for the RX direction
 * only.
 */
noinline fastcall __unlikely int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_allow_input_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch (__builtin_expect(cd_get_state(cd), CD_ENABLED)) {
	case CD_ENABLED:
		break;
	case CD_INPUT_ALLOWED:
		goto ignore;
	default:
		goto outstate;
	}
	cd_set_pending(cd, CD_ALLOW_INPUT_REQ);
	return ch_connect_req(cd->ch, q, mp, CHF_RX_DIR, -1U);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      ignore:
	cd_set_pending(cd, CD_ALLOW_INPUT_REQ);
	return cd_ok_ack(cd, q, mp, CD_HALT_INPUT_REQ);
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
static inline fastcall __hot_put int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_read_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_pending(cd, CD_READ_REQ);
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}

/**
 * cd_write: - process received M_DATA message from above
 * @cd: CD private structure (locked)
 * @q: active queue (upper write queue)
 * @dp: the M_DATA message (chain)
 *
 * M_DATA messages are CD_UNITDATA_REQ primitives without the M_PROTO portion.  These messages are
 * always CD_IMPLICIT and must contain 53 octets with the partially completed header.  HEC
 * calculation and payload scrambling has yet to be performed on the cells.  Note that these
 * messages can be of different priorities.
 */
static inline fastcall __hot_write int
cd_write(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err;

	switch (__builtin_expect(cd_get_state(cd), CD_INPUT_ALLOWED)) {
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
	case CD_ENABLED:
	case CD_OUTPUT_ACTIVE:
		break;
	case CD_DISABLED:
		goto discard;
	default:
		goto outstate;
	}
	if (unlikely(msgsize(dp) != 53))
		goto writefail;
	if (unlikely(!pullupmsg(dp, 53))) {
		mi_bufcall(q, 53, BPRI_MED);
		return (-ENOBUFS);
	}
	if (!pcmsg(DB_TYPE(dp)) && !bcanputnext(q, dp->b_band))
		return (-EBUSY);
	/* FIXME calculate HEC and scramble payload if necessary */
	ph_stripmsg(msg, dp);
	putnext(q, dp);
	return (0);
      writefail:
	err = CD_WRITEFAIL;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      discard:
	ph_stripmsg(msg, dp);
	freemsg(dp);
	return (0);
      error:
	return (err);
}
static inline fastcall __hot_put int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch (__builtin_expect(p->cd_addr_type, CD_IMPLICIT)) {
	case CD_IMPLICIT:
		if (unlikely(mp->b_cont == NULL))
			goto writefail;
		mp->b_cont->b_band = mp->b_band;
		if (DB_TYPE(mp) == M_PCPROTO)
			DB_TYPE(mp->b_cont) = M_HPDATA;
		return cd_write(cd, q, mp, mp->b_cont);
	case CD_SPECIFIC:
	case CD_BROADCAST:
		goto notsupp;
	default:
		goto badaddrtype;
	}
      badaddrtype:
	err = CD_BADADDRTYPE;
	goto error;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      writefail:
	err = CD_WRITEFAIL;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
static inline fastcall __hot_put int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_write_read_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_pending(cd, CD_WRITE_READ_REQ);
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}

/**
 * cd_halt_input_req: - process CD_HALT_INPUT_REQ message
 * @cd: CD private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the CD_HALT_INPUT_REQ primitive
 *
 * The CD_HALT_INPUT_REQ primitive translates to a CH_DISCONNECT_REQ primitive for the RX direction
 * only.
 */
noinline fastcall __unlikely int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_halt_input_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch (__builtin_expect(cd_get_state(cd), CD_INPUT_ALLOWED)) {
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
		break;
	case CD_ENABLED:
		goto ignore;
	default:
		goto outstate;
	}
	switch (p->cd_disposal) {
	case CD_FLUSH:
		/* flush first */
		if (unlikely(!putnextctl2(q, M_FLUSH, FLUSHW, 0))) {
			mi_bufcall(q, 2, BPRI_MED);
			return (-ENOBUFS);
		}
		break;
	case CD_WAIT:
	case CD_DELIVER:
		break;
	default:
		goto baddisposal;
	}
	cd_set_pending(cd, CD_HALT_INPUT_REQ);
	return ch_disconnect_req(cd->ch, q, mp, CHF_RX_DIR, -1U);
      baddisposal:
	err = CD_BADDISPOSAL;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      ignore:
	cd_set_pending(cd, CD_HALT_INPUT_REQ);
	return cd_ok_ack(cd, q, mp, CD_HALT_INPUT_REQ);
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_abort_output_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_pending(cd, CD_ABORT_OUTPUT_REQ);
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_mux_name_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_pending(cd, CD_MUX_NAME_REQ);
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_pending(cd, CD_MODEM_SIG_REQ);
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_poll_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_pending(cd, CD_MODEM_SIG_POLL);
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}

/*
 * CH PROVIDER TO CH USER PRIMITIVES
 */

noinline fastcall __unlikely int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_info_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(!MBLKIN(mp, p->ch_addr_offset, p->ch_addr_length)))
		goto badaddr;
	if (unlikely(!MBLKIN(mp, p->ch_parm_offset, p->ch_parm_length)))
		goto badparm;
	if (cd_get_pending(ch->cd) == CD_INFO_REQ) {
	}
      badparm:
	err = CHBADPARM;
	goto error;
      badaddr:
	err = CHBADADDR;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

noinline fastcall __unlikely int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(!MBLKIN(mp, p->ch_opt_offset, p->ch_opt_length)))
		goto badopt;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      badopt:
	err = CHBADOPT;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_cd_prim: - translate CH primitive to CD primitive
 * @prim: CH primitive to translate
 */
noinline fastcall ch_ulong
ch_cd_prim(const ch_ulong prim)
{
	switch (prim) {
	case CH_INFO_REQ:
		return CD_INFO_REQ;
	case CH_OPTMGMT_REQ:
		return -1U;
	case CH_ATTACH_REQ:
		return CD_ATTACH_REQ;
	case CH_ENABLE_REQ:
		return CD_ENABLE_REQ;
	case CH_CONNECT_REQ:
		return -1U;
	case CH_DATA_REQ:
		return CD_UNITDATA_REQ;
	case CH_DISCONNECT_REQ:
		return -1U;
	case CH_DISABLE_REQ:
		return CD_DISABLE_REQ;
	case CH_DETACH_REQ:
		return CD_DETACH_REQ;
	case CH_INFO_ACK:
		return CD_INFO_REQ;
	case CH_OPTMGMT_ACK:
		return -1U;
	case CH_OK_ACK:
		return CD_OK_ACK;
	case CH_ERROR_ACK:
		return CD_ERROR_ACK;
	case CH_ENABLE_CON:
		return CD_ENABLE_CON;
	case CH_CONNECT_CON:
		return -1U;
	case CH_DATA_IND:
		return CD_UNITDATA_IND;
	case CH_DISCONNECT_IND:
		return CD_ERROR_IND;
	case CH_DISCONNECT_CON:
		return -1U;
	case CH_DISABLE_IND:
		return CD_ERROR_IND;
	case CH_DISABLE_CON:
		return CD_DISABLE_CON;
	case CH_EVENT_IND:
		return CD_ERROR_IND;
	default:
		return -1U;
	}
}

noinline fastcall __unlikely int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_ok_ack *p = (typeof(p)) mp->b_rptr;
	ch_ulong state;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch ((state = ch_get_state(ch))) {
	case CHS_WACK_AREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_EREQ:
		state = CHS_WCON_EREQ;
		goto discard;
	case CHS_WACK_CREQ:
		state = CHS_WCON_CREQ;
		goto discard;
	case CHS_WACK_DREQ:
		state = CHS_WCON_DREQ;
		goto discard;
	case CHS_WACK_RREQ:
		state = CHS_WCON_RREQ;
		goto discard;
	case CHS_WACK_UREQ:
		state = CHS_DETACHED;
		break;
	default:
		goto outstate;
	}
	ch_set_state(ch, state);
	return cd_ok_ack(ch->cd, q, mp, ch_cd_prim(p->ch_correct_prim));
      discard:
	ch_set_state(ch, state);
	freemsg(mp);
	return (0);
      outstate:
	err = CHOUTSTATE;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_cd_error: - convert CH error into CD error
 * @type: the CH error type
 * @error: the CH error number
 */
noinline fastcall cd_long
ch_cd_error(const ch_ulong type, const ch_ulong error)
{
	switch (type) {
	case CHSYSERR:
		return -error;
	case CHBADADDR:
		return CD_BADPPA;
	case CHOUTSTATE:
		return CD_OUTSTATE;
	case CHBADOPT:
		return -1U;
	case CHBADPARM:
		return -1U;
	case CHBADPARMTYPE:
		return -1U;
	case CHBADFLAG:
		return -1U;
	case CHBADPRIM:
		return CD_BADPRIM;
	case CHNOTSUPP:
		return CD_NOTSUPP;
	case CHBADSLOT:
		return -1U;
	default:
		return -1U;
	}
}

noinline fastcall __unlikely int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_error_ack *p = (typeof(p)) mp->b_rptr;
	ch_ulong state;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch ((state = ch_get_state(ch))) {
	case CHS_WACK_AREQ:
		state = CHS_DETACHED;
		break;
	case CHS_WACK_EREQ:
	case CHS_WCON_EREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_CREQ:
	case CHS_WCON_CREQ:
		state = CHS_ENABLED;
		break;
	case CHS_WACK_DREQ:
	case CHS_WCON_DREQ:
		state = CHS_CONNECTED;
		break;
	case CHS_WACK_RREQ:
	case CHS_WCON_RREQ:
		state = CHS_ENABLED;
		break;
	case CHS_WACK_UREQ:
		state = CHS_ATTACHED;
		break;
	default:
		goto outstate;
	}
	ch_set_state(ch, state);
	return cd_error_ack(ch->cd, q, mp, ch_cd_prim(p->ch_error_primitive),
			    ch_cd_error(p->ch_error_type, p->ch_unix_error));
      outstate:
	err = CHOUTSTATE;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_con *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	ch_set_state(ch, CHS_ENABLED);
	return ch_connect_req(ch, q, mp, CHF_BOTH_DIR, -1U);
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_connect_con: - process CH_CONNECT_CON primitive
 * @ch: CH private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the CH_CONNECT_CON primitive
 *
 * Connects and disconnects in the RX direction for the CHI map onto CD_ALLOW_INPUT_REQ and
 * CD_HALT_INPUT_REQ for the CDI.  Under CDI, enabling the device always enables the output section.
 * There are two forms of connect confirmation.  One in response to a connect request that was
 * part of a CD_ENABLE_REQ.  Another in response to a connect request that was part of a
 * CD_ALLOW_INPUT_REQ.  The former has the CHF_TX_DIR flag set; the later only the CHF_RX_DIR flag
 * set.  The former is responded to with CD_ENABLE_CON whereas the later with CD_OK_ACK.
 */
noinline fastcall __unlikely int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_con *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(ch_get_state(ch) != CHS_WCON_CREQ))
		goto outstate;
	if (p->ch_conn_flags & CHF_TX_DIR)
		ch->state.conn_tx |= p->ch_slot;
	if (p->ch_conn_flags & CHF_RX_DIR)
		ch->state.conn_rx |= p->ch_slot;
	if (p->ch_conn_flags & CHF_TX_DIR) {
		ch_set_state(ch, CHS_CONNECTED);
		return cd_enable_con(ch->cd, q, mp);
	}
	ch_set_state(ch, CHS_CONNECTED);
	return cd_ok_ack(ch->cd, q, mp, CD_ALLOW_INPUT_REQ);
      outstate:
	err = CHOUTSTATE;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_read: - process received M_DATA message from below
 * @ch: CH private structure (locked)
 * @q: active queue (upper write queue)
 * @msg: message to free upon success (or NULL)
 * @dp: the M_DATA messsage (chain)
 *
 * M_DATA messages are CH_DATA_IND primitives without the M_PROTO portion.  These messages contain a
 * simple byte stream.  ATM header detection and syncrhonization, header and payload descrambling
 * must be performed on the byte Stream.
 *
 * In the HUNT state, the delineation process is performed by checking bit by bit for the correct
 * HEC (i.e. syndrome equals zero) for the assumed header field.  For the cell-based Physical Layer,
 * prior to descrambler synchronization, only the last six bits of the HEC are to be used for cell
 * delineation.  Once such an agreement is found, it is assumed that one header has been found, and
 * the process enters the PRESYNC state.  When octet boundaries are available within the the
 * receiving Physical Layer prior to cell delineation as with the SDH-based interface, the cell
 * delineation process may be performed octet-by-octet.
 *
 * In the PRESYNC state, the delineation process is performed by checking cell by cell for the
 * correct HEC.  For the cell-based Physical Layer, prior to descrambler synchronization, only the
 * last six bits of the HEC field are to be used for cell delineation checking.  The process repeats
 * until the correct HEC has been confirmed DELTA times consecutively, at which point the process
 * moves tot eh SYNC state.  If an incorrect HEC is found, the process returns to the HUNT state.
 * The total number of consequtive correct HECs required to move from the HUNT state to the SYNC
 * state is therefore DELTA + 1.
 *
 * In the SYNC state, the cell delineation will be assumed to be lost if an incorrect HEC is
 * obtained ALPHA times consecutively.
 *
 * For SDH-based Physical Layer, cells with correct HECs (or cell headers with single bit errors
 * which are corrected) that are processed while in the SYNC state shall be passed to the ATM layer.
 * For cell-based Physical Layer, cells with correct HECs (or cell headers with single bit errors
 * which are corrected) that are processed while in the SYNC state shall be passed to the ATM layer
 * provided the descrambler is in the steady state.  Idle cells and Physical Layer OAM cells are not
 * passed to the ATM layer.
 *
 * The parameters ALPHA and DELTA are to be chosen to make the cell delineation process as robust
 * and secure as possible and while satisfying the performance specified in I.432.1/7.3.3.3.
 *
 * For an SDH-based Physical Layer ALPHA = 7 and DELTA = 6.
 * For a cell-based Physical Layer ALPHA = 7 and DELTA = 8.
 *
 */
static inline fastcall __hot_read int
ch_read(struct ch *ch, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	size_t dlen;
	int err;

	switch (__builtin_expect(ch_get_state(ch), CHS_CONNECTED)) {
	case CHS_CONNECTED:
		break;
	case CHS_ENABLED:
		goto discard;
	default:
		goto outstate;
	}
	/* What we want to do here is if we are hunting for ATM headers, hunt the received bytes
	   for an ATM header.  If we are in pre-sync and there is not a full cell received yet, add 
	   the bytes to the exising cell being built.  If a full cell has been received, create a
	   new cell and copy bytes to it until there is an ATM header in the new cell.  Once there
	   is an ATM header in the new cell, test it and if it is good move to syncrhonization.
	   Under syncrhonization release full cell upstream after descrambling.  Assemble new cells 
	   and descramble and HEC check.  If they do not pass HEC check, pass them up as bad frame
	   indications.  Note that priorities (and thus queue bands) are not assigned to cells
	   until they have been extracted from the byte Stream. */
	/* Hunting for ATM headers and syncrhonizing on the headers is described in ITU-T
	   Recommendation I.432.1, but is also described in AF-PHYS-00016. */
	dlen = msgsize(dp);
	ch->dmsg = linkmsg(ch->dmsg, dp);
	ch->dlen += dlen;
	/* FIXME: process received bytes so far. */
      outstate:
	err = CHOUTSTATE;
	goto error;
      discard:
	ph_stripmsg(msg, dp);
	freemsg(dp);
	return (0);
      error:
	return (err);

}

/**
 * ch_data_ind: - process CH_DATA_IND primitive from downstream
 * @ch: CH private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the CH_DATA_IND primitive
 *
 * This is the less-preferred way of passing data upstream.  We do not use the CH_DATA_IND M_PROTO
 * message block because there is only one slot in channels used for ATM.  Therefore, we simply
 * discard the M_PROTO message block and treate the M_DATA message blocks alone.
 */
static inline fastcall __hot_get int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(mp->b_cont == NULL))
		goto badprim;
	return ch_read(ch, q, mp, mp->b_cont);
      badprim:
	err = CHBADPRIM;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	/* FIXME: determine more information from flags and slot */
	ch_set_state(ch, CHS_ENABLED);
	return cd_error_ind(ch->cd, q, mp, CD_ENABLED, CD_DISC);
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_disconnect_con: - process CH_DISCONNECT_CON primitive
 * @ch: CH private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the CH_DISCONNECT_CON primitive
 *
 * Connects and disconnects in the RX direction for the CHI map onto CD_ALLOW_INPUT_REQ and
 * CD_HALT_INPUT_REQ for the CDI.  Under CDI, enabling the device always enables the output
 * section.  There are two forms of disconnect confirmation.  One in response to a disconnect
 * request that was part of a CD_DISABLE_REQ.  Another in response to a disconnect request that was
 * part of a CD_HALT_INPUT_REQ.  The former has the CHF_TX_DIR flag set; the later only the
 * CHF_RX_DIR flag set.  The former is responded to with CH_DISABLE_REQ whereas the later with
 * CD_OK_ACK.
 */
noinline fastcall __unlikely int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_con *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(ch_get_state(ch) != CHS_WCON_DREQ))
		goto outstate;
	if (p->ch_conn_flags & CHF_TX_DIR)
		ch->state.conn_tx &= ~p->ch_slot;
	if (p->ch_conn_flags & CHF_RX_DIR)
		ch->state.conn_rx &= ~p->ch_slot;
	if (p->ch_conn_flags & CHF_TX_DIR) {
		ch_set_state(ch, CHS_ENABLED);
		return ch_disable_req(ch, q, mp);
	}
	ch_set_state(ch, CHS_CONNECTED);
	return cd_ok_ack(ch->cd, q, mp, CD_HALT_INPUT_REQ);
      outstate:
	err = CHOUTSTATE;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline fastcall cd_ulong
ch_cd_cause(const ch_ulong cause)
{
	/* FIXME */
	return (cause);
}
noinline fastcall __unlikely int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	ch_set_state(ch, CHS_ATTACHED);
	return cd_error_ind(ch->cd, q, mp, CD_DISABLED, ch_cd_cause(p->ch_cause));
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_con *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch (__builtin_expect(ch_get_state(ch), CHS_WCON_RREQ)) {
	case CHS_WACK_RREQ:
		/* It is optional to CH_OK_ACK the CH_CONNECT_REQ. */
	case CHS_WCON_RREQ:
		break;
	default:
		goto outstate;
	}
	ch_set_state(ch, CHS_ATTACHED);
	return cd_disable_con(ch->cd, q, mp);
      outstate:
	err = CHOUTSTATE;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline fastcall __hot_get int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_event_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
ch_other_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      protoshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * STREAMS MESSAGE TYPE HANDLING
 */

/**
 * cd_w_data: - process received M_DATA message from above
 * @cd: CD private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the M_DATA message
 *
 * M_DATA messages are CD_UNITDATA_REQ primitives without the M_PROTO portion.  These messages are
 * always CD_IMPLICIT and must contain 53 octets with the partially completed header.  HEC
 * calculation and payload scrambling has yet to be performed on the cells.
 */
static fastcall int
cd_w_data(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	cd_save_state(cd);
	err = cd_write(cd, q, NULL, mp);
	if (likely(err == 0))
		return (err);
	cd_restore_state(cd);
	return cd_reply_error(cd, q, mp, CD_UNITDATA_REQ, err);
}
static fastcall int
ch_r_data(struct ch *ch, queue_t *q, mblk_t *mp)
{
	int err;

	ch_save_state(ch);
	err = ch_read(ch, q, NULL, mp);
	if (likely(err == 0))
		return (err);
	ch_restore_state(ch);
	return ch_reply_error(ch, q, mp, CH_DATA_IND, err);
}

static fastcall int
cd_w_proto(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong prim = -1U;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	cd_rxprim(cd, q, mp, prim);
	cd_save_state(cd);
	switch (__builtin_expect(prim, CD_UNITDATA_REQ)) {
	case CD_INFO_REQ:
		err = cd_info_req(cd, q, mp);
		break;
	case CD_ATTACH_REQ:
		err = cd_attach_req(cd, q, mp);
		break;
	case CD_DETACH_REQ:
		err = cd_detach_req(cd, q, mp);
		break;
	case CD_ENABLE_REQ:
		err = cd_enable_req(cd, q, mp);
		break;
	case CD_DISABLE_REQ:
		err = cd_disable_req(cd, q, mp);
		break;
	case CD_ALLOW_INPUT_REQ:
		err = cd_allow_input_req(cd, q, mp);
		break;
	case CD_READ_REQ:
		err = cd_read_req(cd, q, mp);
		break;
	case CD_UNITDATA_REQ:
		err = cd_unitdata_req(cd, q, mp);
		break;
	case CD_WRITE_READ_REQ:
		err = cd_write_read_req(cd, q, mp);
		break;
	case CD_HALT_INPUT_REQ:
		err = cd_halt_input_req(cd, q, mp);
		break;
	case CD_ABORT_OUTPUT_REQ:
		err = cd_abort_output_req(cd, q, mp);
		break;
	case CD_MUX_NAME_REQ:
		err = cd_mux_name_req(cd, q, mp);
		break;
	case CD_MODEM_SIG_REQ:
		err = cd_modem_sig_req(cd, q, mp);
		break;
	case CD_MODEM_SIG_POLL:
		err = cd_modem_sig_poll(cd, q, mp);
		break;
	default:
		err = CD_BADPRIM;
		break;
	}
	if (likely(err == 0))
		return (err);
      out:
	cd_restore_state(cd);
	return cd_reply_error(cd, q, mp, prim, err);
      tooshort:
	err = CD_PROTOSHORT;
	goto out;
}
static fastcall int
ch_r_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong prim = -1U;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	ch_rxprim(ch, q, mp, prim);
	switch (__builtin_expect(prim, CH_DATA_IND)) {
	case CH_INFO_ACK:
		err = ch_info_ack(ch, q, mp);
		break;
	case CH_OPTMGMT_ACK:
		err = ch_optmgmt_ack(ch, q, mp);
		break;
	case CH_OK_ACK:
		err = ch_ok_ack(ch, q, mp);
		break;
	case CH_ERROR_ACK:
		err = ch_error_ack(ch, q, mp);
		break;
	case CH_ENABLE_CON:
		err = ch_enable_con(ch, q, mp);
		break;
	case CH_CONNECT_CON:
		err = ch_connect_con(ch, q, mp);
		break;
	case CH_DATA_IND:
		err = ch_data_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_IND:
		err = ch_disconnect_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_CON:
		err = ch_disconnect_con(ch, q, mp);
		break;
	case CH_DISABLE_IND:
		err = ch_disable_ind(ch, q, mp);
		break;
	case CH_DISABLE_CON:
		err = ch_disable_con(ch, q, mp);
		break;
	case CH_EVENT_IND:
		err = ch_event_ind(ch, q, mp);
		break;
	default:
		err = ch_other_ind(ch, q, mp);
		break;
	}
	if (likely(err == 0))
		return (err);
      out:
	ch_restore_state(ch);
	return ch_reply_error(ch, q, mp, prim, err);
      tooshort:
	err = -EMSGSIZE;
	goto out;
}

static fastcall int
cd_w_ctl(struct cd *cd, queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}
static fastcall int
ch_r_ctl(struct ch *ch, queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}

static fastcall int
cd_w_ioctl(struct cd *cd, queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}
static fastcall int
ch_r_ioctl(struct ch *ch, queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}

static fastcall int
m_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static fastcall int
m_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static fastcall int
m_r_error(queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}

static fastcall int
m_w_other(queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}
static fastcall int
m_r_other(queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}

/*
 * STREAMS MESSAGE HANDLING
 */

static fastcall int
cd_wsrv_msg(struct cd *cd, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return cd_w_data(cd, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_w_proto(cd, q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_w_ctl(cd, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return cd_w_ioctl(cd, q, mp);
	case M_FLUSH:
		return m_w_flush(q, mp);
	default:
		return m_w_other(q, mp);
	}
}
static fastcall int
ch_rsrv_msg(struct ch *ch, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return ch_r_data(ch, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ch_r_proto(ch, q, mp);
	case M_CTL:
	case M_PCCTL:
		return ch_r_ctl(ch, q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return ch_r_ioctl(ch, q, mp);
	case M_FLUSH:
		return m_r_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return m_r_error(q, mp);
	default:
		return m_r_other(q, mp);
	}
}

static fastcall int
cd_wput_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;

	switch (__builtin_expect((type = DB_TYPE(mp)), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
	case M_IOCTL:
		return (-EAGAIN);	/* for speed */
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCDATA:
	{
		struct priv *priv;
		int err;

		if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
			switch (__builtin_expect(type, M_PCPROTO)) {
			case M_HPDATA:
				err = cd_w_data(&priv->cd, q, mp);
				break;
			case M_PCPROTO:
				err = cd_w_proto(&priv->cd, q, mp);
				break;
			case M_PCCTL:
				err = cd_w_ctl(&priv->cd, q, mp);
				break;
			case M_IOCDATA:
				err = cd_w_ioctl(&priv->cd, q, mp);
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_unlock((caddr_t) priv);
		} else
			err = -EDEADLK;
		return (err);
	}
	case M_FLUSH:
		return m_w_flush(q, mp);
	default:
		return m_w_other(q, mp);
	}
}
static fastcall int
ch_rput_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;

	switch (__builtin_expect((type = DB_TYPE(mp)), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
		return (-EAGAIN);	/* for speed */
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	{
		struct priv *priv;
		int err;

		if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
			switch (__builtin_expect(type, M_PCPROTO)) {
			case M_HPDATA:
				err = ch_r_data(&priv->ch, q, mp);
				break;
			case M_PCPROTO:
				err = ch_r_proto(&priv->ch, q, mp);
				break;
			case M_PCCTL:
				err = ch_r_ctl(&priv->ch, q, mp);
				break;
			case M_IOCACK:
			case M_IOCNAK:
			case M_COPYIN:
			case M_COPYOUT:
				err = ch_r_ioctl(&priv->ch, q, mp);
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_unlock((caddr_t) priv);
		} else
			err = -EDEADLK;
		return (err);
	}
	case M_FLUSH:
		return m_r_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return m_r_error(q, mp);
	default:
		return m_r_other(q, mp);
	}
}
static streamscall int
cd_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
		     || cd_wput_msg(q, mp))) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}
static streamscall int
cd_wsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(cd_wsrv_msg(&priv->cd, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) priv);
	}
	return (0);
}
static streamscall int
ch_rsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(ch_rsrv_msg(&priv->ch, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) priv);
	}
	return (0);
}
static streamscall int
ch_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
		     || ch_rput_msg(q, mp))) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * ph_qopen: - STREAMS open routine
 * @q: read queue of opened (pushed) queue pair
 * @devp: pointer to device number of driver
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @crp: credentials of opening (pushing) process
 * 
 * The effective style of the CD Stream depends on the style of the CH Stream and the state in which
 * the CH Stream was at the time that the PHYS module was pushed on the Stream.  If the CH Stream is
 * a Style 1 Stream, then the CD Stream is always a Style 1 Stream.  If the CH Stream was in the
 * attached state when the module was pushed, the CD Stream is a Style 1 Stream.  If the CH Stream
 * is in the unattached state when the module was pushed, the CD Stream is a Style 2 Stream in the
 * unattached state.  The initial style is set to Style 2.  The open routine issues an immediate
 * information request to correct style in case of attached Stream or a Style 1 CH Stream.
 */
static streamscall int
ph_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct CH_info_req *r;
	struct priv *priv;
	mblk_t *rp;
	int err;

	if (q->q_ptr)
		return (0);
	if (sflag != MODOPEN)
		return (ENXIO);
	if (unlikely(!!(rp = allocb(sizeof(*r), BPRI_WAITOK))))
		return (ENOSR);
	write_lock(&ph_lock);
	if ((err = mi_open_comm(&ph_opens, sizeof(*priv), q, devp, oflags, sflag, crp))) {
		write_unlock(&ph_lock);
		freeb(rp);
		return (err);
	}
	priv = (struct priv *) (q->q_ptr);
	bzero(priv, sizeof(*priv));
	priv->rq = RD(q);
	priv->wq = WR(q);
	priv->cd.ch = &priv->ch;
	priv->cd.iq = WR(q);
	priv->cd.oq = RD(q);
	priv->cd.state.state = CD_UNUSABLE;
	priv->cd.oldstate.state = CD_UNUSABLE;
	priv->cd.info.cd_primitive = CD_INFO_ACK;
	priv->cd.info.cd_state = CD_UNUSABLE;
	priv->cd.info.cd_max_sdu = 48;
	priv->cd.info.cd_min_sdu = 48;
	priv->cd.info.cd_class = CD_ATM;
	priv->cd.info.cd_duplex = CD_FULLDUPLEX;
	priv->cd.info.cd_output_style = CD_PACEDOUTPUT;
	priv->cd.info.cd_features = CD_CANREAD | CD_AUTOALLOW;
	priv->cd.info.cd_addr_length = 3;	/* VPI + VCI */
	priv->cd.info.cd_ppa_style = CD_STYLE2;	/* For now. */
	priv->ch.cd = &priv->cd;
	priv->ch.iq = RD(q);
	priv->ch.oq = WR(q);
	priv->ch.state.state = CHS_UNINIT;
	priv->ch.oldstate.state = CHS_UNINIT;
	priv->ch.info.ch_primitive = CH_INFO_ACK;
	priv->ch.info.ch_addr_length = 0;
	priv->ch.info.ch_addr_offset = 0;
	priv->ch.info.ch_parm_length = 0;
	priv->ch.info.ch_parm_offset = 0;
	priv->ch.info.ch_prov_flags = 0;
	priv->ch.info.ch_prov_class = CH_CIRCUIT;
	priv->ch.info.ch_style = CH_STYLE2;
	priv->ch.info.ch_version = CH_VERSION;
	priv->ch.info.ch_state = CHS_UNINIT;
	write_unlock(&ph_lock);
	r = (typeof(r)) rp->b_wptr;
	r->ch_primitive = CH_INFO_REQ;
	rp->b_wptr += sizeof(*r);
	qprocson(q);
	putnext(WR(q), rp);
	return (0);
}

static streamscall int
ph_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	write_lock(&ph_lock);
	mi_close_comm(&ph_opens, q);
	write_unlock(&ph_lock);
	return (0);
}

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for PHYS module. (0 for allocation.)");
#endif				/* LINUX */

static struct module_info ph_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ph_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ph_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit ph_rdinit = {
	.qi_putp = ch_rput,
	.qi_srvp = ch_rsrv,
	.qi_qopen = ph_qopen,
	.qi_qclose = ph_qclose,
	.qi_minfo = &ph_minfo,
	.qi_mstat = &ph_rstat,
};

static struct qinit ph_wrinit = {
	.qi_putp = cd_wput,
	.qi_srvp = cd_wsrv,
	.qi_minfo = &ph_minfo,
	.qi_mstat = &ph_wstat,
};

struct streamtab physinfo = {
	.st_rdinit = &ph_rdinit,
	.st_wrinit = &ph_wrinit,
};

static struct fmodsw ph_fmod = {
	.f_name = MOD_NAME,
	.f_str = &physinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static int __init
physinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&ph_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static int
physterminate(void)
{
	int err;

	if ((err = unregister_strmod(&ph_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	modid = 0;
	return (0);
}

static void __exit
physexit(void)
{
	physterminate();
}

module_init(physinit);
module_exit(physexit);
