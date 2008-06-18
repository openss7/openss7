/*****************************************************************************

 @(#) $RCSfile: hdlc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-06-13 06:43:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: hdlc.c,v $
 Revision 0.9.2.1  2008-06-13 06:43:57  brian
 - added files

 *****************************************************************************/

#ident "@(#) $RCSfile: hdlc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $"

static char const ident[] = "$RCSfile: hdlc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $";

/*
 * This is an HDLC module that pushes of a CH stream.  It provides a CDI HDLC
 * interface at its upper boundary and uses a CH channel interface at its
 * lower boundary.  This module can be pushed to convert any raw channel
 * driver into an HDLC driver.
 */

#include <sys/os8/compat.h>

#include <sys/cdi.h>
#include <sys/ch.h>

#define HDLC_DESCRIP	"HDLC OVER CH MODULE FOR LINUX FAST-STREAMS"
#define HDLC_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define HDLC_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporaiton.  All Rights Reserved."
#define HDLC_REVISION	"OpenSS7 $RCSfile: hdlc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $"
#define HDLC_DEVICE	"SVR 4.2MP HDLC over CH Module (HDLC) for X.25"
#define HDLC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define HDLC_LICENSE	"GPL"
#define HDLC_BANNER	HDLC_DESCRIP	"\n" \
			HDLC_EXTRA	"\n" \
			HDLC_COPYRIGHT	"\n" \
			HDLC_REVISION	"\n" \
			HDLC_DEVICE	"\n" \
			HDLC_CONTACT	"\n"
#define HDLC_SPLASH	HDLC_DESCRIP	" - " \
			HDLC_REVISION

#ifndef CONFIG_STREAMS_HDLC_NAME
#error "CONFIG_STREAMS_HDLC_NAME must be defined."
#endif				/* CONFIG_STREAMS_HDLC_NAME */
#ifndef CONFIG_STREAMS_HDLC_MODID
#error "CONFIG_STREAMS_HDLC_MODID must be defined."
#endif				/* CONFIG_STREAMS_HDLC_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(HDLC_CONTACT);
MODULE_DESCRIPTION(HDLC_DESCRIP);
MODULE_SUPPORTED_DEVICE(HDLC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(HDLC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-hdlc");
MODULE_ALIAS("streams-module-hdlc");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_HDLC_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef HDLC_MOD_NAME
#define HDLC_MOD_NAME		CONFIG_STREAMS_HDLC_NAME
#endif				/* HDLC_MOD_NAME */
#ifndef HDLC_MOD_ID
#define HDLC_MOD_ID		CONFIG_STREAMS_HDLC_MODID
#endif				/* HDLC_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		HDLC_MOD_ID
#define MOD_NAME	HDLC_MOD_NAME
#ifdef MODULE
#define MODULE		HDLC_BANNER
#else				/* MODULE */
#define MODULE		HDLC_SPLASH
#endif				/* MODULE */

static struct module_info hdlc_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat hdlc_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat hdlc_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct priv *priv;
struct cd *cd;
struct ch *ch;

/* Upper interface structure. */
struct cd {
	struct priv *priv;
	struct ch *ch;
	queue_t *oq;
	cd_ulong state;
	cd_ulong oldstate;
	cd_ulong coninds;
	cd_ulong oldconinds;
	struct cd *ad;
	ushort add_len;
	struct {
		cd_info_ack_t info;
		unsigned char add[20];
	} proto;
	ushort loc_len;
	unsigned char loc[20];
	ushort rem_len;
	unsigned char rem[20];
};

/* Lower interface structure. */
struct ch {
	struct priv *priv;
	struct cd *cd;
	queue_t *oq;
	ch_ulong state;
	ch_ulong oldstate;
	struct {
		ch_info_ack_t info;
		unsigned char add[20];
	} proto;
};

struct priv {
	atomic_int_t refs;
	struct cd cdi;
	struct ch chi;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define CD_PRIV(q) (&PRIV(q)->cdi)
#define CH_PRIV(q) (&PRIV(q)->chi)

static void
priv_get(struct priv *priv)
{
	ATOMIC_INT_INCR(&priv->refs);
}
static void
priv_put(struct priv *priv)
{
	if (ATOMIC_INT_DECR(&priv->refs))
		mi_close_free((caddr_t) priv);
}
static struct cd *
cd_get(struct cd *cd)
{
	if (cd)
		priv_get(cd->priv);
	return (cd);
}
static void
cd_put(struct cd *cd)
{
	if (cd)
		priv_put(cd->priv);
}

static struct ch *
ch_get(struct ch *ch)
{
	if (ch)
		priv_get(ch->priv);
	return (ch);
}
static void
ch_put(struct ch *ch)
{
	if (ch)
		priv_put(ch->priv);
}

static inline const char *
cd_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}
static inline const char *
cd_primname(cd_ulong prim)
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
static inline const char *
cd_statename(cd_ulong state)
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
static inline const char *
cd_errname(cd_long error)
{
	if (error < 0)
		return ("CD_SYSERR");
	switch (error) {
	case CD_BADADDRESS:
		return ("CD_BADADDRESS");
	case CD_BADADDRTYPE:
		return ("CD_BADADDRTYPE");
	case CD_BADDIAL:
		return ("CD_BADDIAL");
	case CD_BADDIALTYPE:
		return ("CD_BADDIALTYPE");
	case CD_BADDISPOSAL:
		return ("CD_BADDISPOSAL");
	case CD_BADFRAME:
		return ("CD_BADFRAME");
	case CD_BADPPA:
		return ("CD_BADPPA");
	case CD_BADPRIM:
		return ("CD_BADPRIM");
	case CD_DISC:
		return ("CD_DISC");
	case CD_EVENT:
		return ("CD_EVENT");
	case CD_FATALERR:
		return ("CD_FATALERR");
	case CD_INITFAILED:
		return ("CD_INITFAILED");
	case CD_NOTSUPP:
		return ("CD_NOTSUPP");
	case CD_OUTSTATE:
		return ("CD_OUTSTATE");
	case CD_PROTOSHORT:
		return ("CD_PROTOSHORT");
	case CD_READTIMEOUT:
		return ("CD_READTIMEOUT");
	case CD_SYSERR:
		return ("CD_SYSERR");
	case CD_WRITEFAIL:
		return ("CD_WRITEFAIL");
	default:
		return ("(unknown)");
	}
}
static inline const char *
cd_strerror(cd_long error)
{
	if (error < 0)
		return ("UNIX system error.");
	switch (error) {
	case CD_BADADDRESS:
		return ("Address was invalid.");
	case CD_BADADDRTYPE:
		return ("Invalid address type.");
	case CD_BADDIAL:
		return ("Dial information was invalid.");
	case CD_BADDIALTYPE:
		return ("Invalid dial information type.");
	case CD_BADDISPOSAL:
		return ("Invalid disposal parameter.");
	case CD_BADFRAME:
		return ("Defective SDU received.");
	case CD_BADPPA:
		return ("Invalid PPA identifier.");
	case CD_BADPRIM:
		return ("Unrecognized primitive.");
	case CD_DISC:
		return ("Disconnected.");
	case CD_EVENT:
		return ("Protocol-specific event occurred.");
	case CD_FATALERR:
		return ("Device has become unusable.");
	case CD_INITFAILED:
		return ("Line initialization failed.");
	case CD_NOTSUPP:
		return ("Primitive not supported by this device.");
	case CD_OUTSTATE:
		return ("Primitive was issued from an invalid state.");
	case CD_PROTOSHORT:
		return ("M_PROTO block too short.");
	case CD_READTIMEOUT:
		return ("Read request timed out before data arrived.");
	case CD_SYSERR:
		return ("UNIX system error.");
	case CD_WRITEFAIL:
		return ("Unitdata request failed.");
	default:
		return ("(unknown)");
	}
}

static inline const char *
ch_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}
static inline const char *
ch_primname(ch_ulong prim)
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
static inline const char *
ch_statename(ch_ulong state)
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
static inline const char *
ch_errname(ch_long error)
{
	if (error < 0)
		return ("CHSYSERR");
	switch (error) {
	case CHSYSERR:
		return ("CHSYSERR");
	case CHBADADDR:
		return ("CHBADADDR");
	case CHOUTSTATE:
		return ("CHOUTSTATE");
	case CHBADOPT:
		return ("CHBADOPT");
	case CHBADPARM:
		return ("CHBADPARM");
	case CHBADPARMTYPE:
		return ("CHBADPARMTYPE");
	case CHBADFLAG:
		return ("CHBADFLAG");
	case CHBADPRIM:
		return ("CHBADPRIM");
	case CHNOTSUPP:
		return ("CHNOTSUPP");
	case CHBADSLOT:
		return ("CHBADSLOT");
	default:
		return ("(unknown)");
	}
}
static inline const char *
ch_strerror(ch_long error)
{
	if (error < 0)
		return ("UNIX system error.");
	switch (error) {
	case CHSYSERR:
		return ("UNIX system error.");
	case CHBADADDR:
		return ("Bad address format or content.");
	case CHOUTSTATE:
		return ("Interface out of state.");
	case CHBADOPT:
		return ("Bad options format or content.");
	case CHBADPARM:
		return ("Bad parameter format or content.");
	case CHBADPARMTYPE:
		return ("Bad paramater structure type.");
	case CHBADFLAG:
		return ("Bad flag.");
	case CHBADPRIM:
		return ("Bad primitive.");
	case CHNOTSUPP:
		return ("Primitive not supported.");
	case CHBADSLOT:
		return ("Bad multplex slot.");
	default:
		return ("(unknown)");
	}
}

static cd_ulong
cd_get_state(struct cd *cd)
{
	return (cd->state);
}

static cd_ulong
cd_set_state(struct cd *cd, cd_ulong newstate)
{
	cd_ulong oldstate = cd->state;

	if (newstate != oldstate) {
		cd->state = newstate;
		cd->proto.info.cd_current_state = newstate;
		mi_strlog(cd->oq, STRLOGST, SL_TRACE, "%s <- %s", cd_statename(newstate),
			  cd_statename(oldstate));
	}
	return (newstate);
}

static cd_ulong
cd_save_state(struct cd *cd)
{
	cd->oldconinds = cd->coninds;
	return ((cd->oldstate = cd_get_state(cd)));
}

static cd_ulong
cd_restore_state(struct cd *cd)
{
	cd->coninds = cd->oldconinds;
	return (cd_set_state(cd, cd->oldstate));
}

static ch_ulong
ch_get_state(struct cd *cd)
{
	return (cd->state);
}

static ch_ulong
ch_set_state(struct cd *cd, ch_ulong newstate)
{
	ch_ulong oldstate = cd->state;

	if (newstate != oldstate) {
		cd->state = newstate;
		cd->proto.info.ch_current_state = newstate;
		mi_strlog(cd->oq, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
	}
	return (newstate);
}

static ch_ulong
ch_save_state(struct cd *cd)
{
	cd->oldconinds = cd->coninds;
	return ((cd->oldstate = ch_get_state(cd)));
}

static ch_ulong
ch_restore_state(struct cd *cd)
{
	cd->coninds = cd->oldconinds;
	return (ch_set_state(cd, cd->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI PROVIDER TO CDI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INFO_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_INFO_ACK");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_OK_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_OK_ACK");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ERROR_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ERROR_ACK");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_enable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ENABLE_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ENABLE_CON");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_disable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_DISABLE_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_DISABLE_CON");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_error_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ERROR_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ERROR_IND");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_UNITDATA_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_UNITDATA_ACK");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_UNITDATA_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_UNITDATA_IND");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_bad_frame_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_BAD_FRAME_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_BAD_FRAME_IND");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_MODEM_SIG_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_MODEM_SIG_IND");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * CHI USER TO CHI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_info_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_INFO_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_INFO_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_optmgmt_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_OPTMGMT_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_OPTMGMT_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_attach_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_ATTACH_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_ATTACH_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_ENABLE_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_ENABLE_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_connect_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_CONNECT_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_CONNECT_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_data_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DATA_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_DATA_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disconnect_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISCONNECT_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_DISCONNECT_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISABLE_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_DISABLE_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_detach_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DETACH_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CH_DETACH_REQ");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * BIT TRANSMISSION
 *
 * --------------------------------------------------------------------------
 */

/*
 * --------------------------------------------------------------------------
 *
 * BIT RECEPTION
 *
 * --------------------------------------------------------------------------
 */

/*
 * --------------------------------------------------------------------------
 *
 * CDI USER TO CDI PROVIDER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_info_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_INFO_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_attach_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ATTACH_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_detach_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_DETACH_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_enable_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ENABLE_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_DISABLE_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_allow_input_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ALLOW_INPUT_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_read_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_READ_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_UNITDATA_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_write_read_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_WRITE_READ_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_halt_input_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_HALT_INPUT_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_abort_output_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ABORT_OUTPUT_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_mux_name_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_MUX_NAME_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_req_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_MODEM_SIG_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_poll_t *p = (typeof(p)) mp->b_prtr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, 0, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddr;
	if (cd_get_state(cd) == CDS_IDLE)
		goto discard;
	if (cd_get_state(cd) != CDS_IDLE)
		goto outstate;
	goto notsupport;
      notsupport:
	err = CD_NOTSUPP;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      toosmall:
	err = CD_BADPRIM;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_MODEM_SIG_POLL, err);
      discard:
	freemsg(mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * CHI PROVIDER TO CHI USER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_info_ack *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_ok_ack *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_error_ack *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_con *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_con *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_ind *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_ind *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_con *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_ind *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_con *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_event_ind *p = (typeof(p)) mp->b_rptr;
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGES
 *
 * --------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
cd_m_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	return cd_data_req(cd, q, mp);
}
static fastcall noinline __unlikely int
cd_m_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(cd_ulong *) mp->b_rptr;
	cd_save_state(cd);
	switch (prim) {
	case CD_INFO_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_ATTACH_REQ:
		rtn = cd_attach_req(cd, q, mp);
		break;
	case CD_DETACH_REQ:
		rtn = cd_detach_req(cd, q, mp);
		break;
	case CD_ENABLE_REQ:
		rtn = cd_enable_req(cd, q, mp);
		break;
	case CD_DISABLE_REQ:
		rtn = cd_disable_req(cd, q, mp);
		break;
	case CD_ALLOW_INPUT_REQ:
		rtn = cd_allow_input_req(cd, q, mp);
		break;
	case CD_READ_REQ:
		rtn = cd_read_req(cd, q, mp);
		break;
	case CD_UNITDATA_REQ:
		rtn = cd_unitdata_req(cd, q, mp);
		break;
	case CD_WRITE_READ_REQ:
		rtn = cd_write_read_req(cd, q, mp);
		break;
	case CD_HALT_INPUT_REQ:
		rtn = cd_halt_input_req(cd, q, mp);
		break;
	case CD_ABORT_OUTPUT_REQ:
		rtn = cd_abort_output_req(cd, q, mp);
		break;
	case CD_MUX_NAME_REQ:
		rtn = cd_mux_name_req(cd, q, mp);
		break;
	case CD_BAD_FRAME_IND:
		rtn = cd_bad_frame_ind(cd, q, mp);
		break;
	case CD_MODEM_SIG_REQ:
		rtn = cd_modem_sig_req(cd, q, mp);
		break;
	case CD_MODEM_SIG_POLL:
		rtn = cd_modem_sig_poll(cd, q, mp);
		break;
	case CD_INFO_ACK:
	case CD_OK_ACK:
	case CD_ERROR_ACK:
	case CD_ENABLE_CON:
	case CD_DISABLE_CON:
	case CD_ERROR_IND:
	case CD_UNITDATA_ACK:
	case CD_UNITDATA_IND:
	case CD_MODEM_SIG_IND:
		rtn = cd_wrongway_ind(cd, q, mp);
		break;
	default:
		rtn = cd_other_req(cd, q, mp);
		break;
	}
	if (rtn)
		cd_restore_state(cd);
	return (rtn);
}
static fastcall noinline __unlikely int
cd_m_ctl(queue_t *q, mblk_t *mp)
{
	return cd_m_proto(q, mp);
}
static fastcall noinline __unlikely int
cd_m_sig(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_flush(queue_t *q, mblk_t *mp)
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
static fastcall noinline __unlikely int
cd_m_ioctl(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_iocdata(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_read(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_other(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
ch_m_data(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);

	return ch_data_ind(ch, q, mp);
}
static fastcall noinline __unlikely int
ch_m_proto(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	ch_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(ch_ulong *) mp->b_rptr;
	ch_save_state(ch);
	switch (prim) {
	case CH_INFO_ACK:
		rtn = ch_info_ack(ch, q, mp);
		break;
	case CH_OPTMGMT_ACK:
		rtn = ch_optmgmt_ack(ch, q, mp);
		break;
	case CH_OK_ACK:
		rtn = ch_ok_ack(ch, q, mp);
		break;
	case CH_ERROR_ACK:
		rtn = ch_error_ack(ch, q, mp);
		break;
	case CH_ENABLE_CON:
		rtn = ch_enable_con(ch, q, mp);
		break;
	case CH_CONNECT_CON:
		rtn = ch_connect_con(ch, q, mp);
		break;
	case CH_DATA_IND:
		rtn = ch_data_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_IND:
		rtn = ch_disconnect_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_CON:
		rtn = ch_disconnect_con(ch, q, mp);
		break;
	case CH_DISABLE_IND:
		rtn = ch_disable_ind(ch, q, mp);
		break;
	case CH_DISABLE_CON:
		rtn = ch_disable_con(ch, q, mp);
		break;
	case CH_EVENT_IND:
		rtn = ch_event_ind(ch, q, mp);
		break;
	case CH_INFO_REQ:
	case CH_OPTMGMT_REQ:
	case CH_ATTACH_REQ:
	case CH_ENABLE_REQ:
	case CH_CONNECT_REQ:
	case CH_DATA_REQ:
	case CH_DISCONNECT_REQ:
	case CH_DISABLE_REQ:
	case CH_DETACH_REQ:
		rtn = ch_wrongway_req(ch, q, mp);
		break;
	default:
		rtn = ch_other_ind(ch, q, mp);
		break;
	}
	if (rtn)
		ch_restore_state(ch);
	return (0);
}
static fastcall noinline __unlikely int
ch_m_sig(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
ch_m_ctl(queue_t *q, mblk_t *mp)
{
	return ch_m_proto(q, mp);
}
static fastcall noinline __unlikely int
ch_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
ch_m_flush(queue_t *q, mblk_t *mp)
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
static fastcall noinline __unlikely int
ch_m_error(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
ch_m_hangup(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
ch_m_setopts(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
ch_m_copy(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
ch_m_iocack(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
ch_m_other(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall inline int
cd_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return (-EAGAIN);	/* fast path for data */
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_SIG:
	case M_PCSIG:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);
	case M_FLUSH:
		return cd_m_flush(q, mp);
	case M_IOCTL:
		return cd_m_ioctl(q, mp);
	case M_IOCDATA:
		return cd_m_iocdata(q, mp);
	case M_READ:
		return cd_m_read(q, mp);
	default:
		return cd_m_other(q, mp);
	}
}
static fastcall noinline int
cd_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return cd_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return cd_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_m_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_m_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return cd_m_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return cd_m_rse(q, mp);
	case M_FLUSH:
		return cd_m_flush(q, mp);
	case M_IOCTL:
		return cd_m_ioctl(q, mp);
	case M_IOCDATA:
		return cd_m_iocdata(q, mp);
	case M_READ:
		return cd_m_read(q, mp);
	default:
		return cd_m_other(q, mp);
	}
}
static fastcall noinline int
ch_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return ch_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return ch_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ch_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return ch_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return ch_m_ctl(q, mp);
	case M_RSE:
	case M_PCRSE:
		return ch_m_rse(q, mp);
	case M_FLUSH:
		return ch_m_flush(q, mp);
	case M_ERROR:
		return ch_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return ch_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return ch_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return ch_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return ch_m_iocack(q, mp);
	default:
		return ch_m_other(q, mp);
	}
}
static fastcall inline int
ch_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_SIG:
	case M_PCSIG:
	case M_CTL:
	case M_PCCTL:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);	/* handle from service procedure */
	case M_FLUSH:
		return ch_m_flush(q, mp);
	case M_ERROR:
		return ch_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return ch_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return ch_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return ch_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return ch_m_iocack(q, mp);
	default:
		return ch_m_other(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * PUT AND SERVICE PROCEDURES
 *
 * --------------------------------------------------------------------------
 */

static streamscall int
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || dl_msg_put(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
dl_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (dl_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
ch_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (ch_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
ch_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || ch_msg_put(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */

caddr_t hdlc_opens = NULL;
rwlock_t hdlc_rwlock = RW_LOCK_UNLOCKED;

static int
hdlc_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct dl *dl;
	struct ch *ch;
	mblk_t *mp;
	int err;
	pl_t pl;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(struct T_capability_req), BPRI_WAITOK)))
		return (ENOBUFS);
	pl = RW_WRLOCK(&hdlc_rwlock, plstr);
	if ((err = mi_open_comm(&hdlc_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		RW_UNLOCK(&hdlc_rwlock, pl);
		freeb(mp);
		return (err);
	}
	RW_UNLOCK(&hdlc_rwlock, pl);

	p = PRIV(q);
	ATOMIC_INT_INIT(&p->refs, 1);
	dl = &p->dlpi;
	ch = &p->chi;

	/* initialize private structure */
	dl->priv = p;
	dl->tp = tp;
	dl->ch = ch;
	dl->oq = q;
	dl->state = DL_UNBOUND;
	dl->oldstate = DL_UNBOUND;

	tp->priv = p;
	tp->dl = dl;
	tp->oq = WR(q);
	tp->state = TS_UNBND;
	tp->oldstate = TS_UNBND;

	ch->priv = p;
	ch->dl = dl;
	ch->oq = WR(q);
	ch->state = NS_UNBND;
	ch->oldstate = NS_UNBND;

	/* FIXME: more initialization */

	DB_TYPE(mp) = M_PCPROTO;
	((struct T_capability_req *) mp->b_wptr)->PRIM_type = T_CAPABILITY_REQ;
	mp->b_wptr += sizeof(struct T_capability_req);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

/**
 * hdlc_qclose: - STREAMS close procedure
 * @q: queue pair to close
 * @oflags: flags to open(2s)
 * @crp: credientials of closer
 *
 * Note that we do not simply use mi_close_comm.  This is because an
 * listening stream may reference this stream as an accepting stream, so we
 * must reference count.  The close procedure is broken into three steps:
 * mi_detach, detach the instance data from the queue pair; mi_close_unlink,
 * remove the instance data from the master list; and, dl_put free the
 * instance data only once the last reference count has been released.
 */
static int
hdlc_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	/* FIXME: free timers and other things. */
	{
		pl_t pl = RW_WRLOCK(&hdlc_rwlock, plstr);

		/* detach private structure from queue pair */
		mi_detach(q, (caddr_t) p);
		/* remove structure from open list */
		mi_close_unlink(&hdlc_opens, (caddr_t) p);
		RW_UNLOCK(&hdlc_rwlock, pl);
	}
	/* This does not call mi_close_free until the last reference is
	   released. */
	priv_put(p);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit hdlc_rinit = {
	.qi_putp = &cd_wput,
	.qi_srvp = &cd_wsrv,
	.qi_qopen = &hdlc_qopen,
	.qi_qclose = &hdlc_qclose,
	.qi_minfo = &hdlc_minfo,
	.qi_mstat = &hdlc_rstat,
};

static struct qinit hdlc_winit = {
	.qi_putp = &ch_wput,
	.qi_srvp = &ch_wsrv,
	.qi_minfo = &hdlc_minfo,
	.qi_mstat = &hdlc_wstat,
};

struct streamtab hdlc_info = {
	.st_rdinit = &hdlc_rinit,
	.st_wrinit = &hdlc_winit,
};

static modID_t modid = MOD_ID;

/*
 * --------------------------------------------------------------------------
 *
 * LINUX FAST STREAMS REGISTRATION
 *
 * --------------------------------------------------------------------------
 */

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for HDLC.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static __init int
hdlc_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&hdlc_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
hdlc_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&hdlc_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(hdlc_modinit);
module_exit(hdlc_modexit);

#endif				/* LINUX */
