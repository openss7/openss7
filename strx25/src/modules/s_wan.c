/*****************************************************************************

 @(#) $RCSfile: s_wan.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $

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

 Last Modified $Date: 2008-07-01 12:16:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: s_wan.c,v $
 Revision 0.9.2.1  2008-07-01 12:16:24  brian
 - added manual pages, specs and conversion modules

 *****************************************************************************/

#ident "@(#) $RCSfile: s_wan.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $"

static char const ident[] = "$RCSfile: s_wan.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $";

/*
 * The s_wan pushable STREAMS module is a module that converts between the
 * OpenSS7 CDI CD_HDLC service interface and input-output controls, to the the
 * Spider WAN interface and input-output controls for (source) compatibility
 * with user-level programs, drivers and modules that utlize the Spider WAN
 * service interface and input-output controls.  This module may be autopushed
 * onto specific clone minor device numbers for CDI drivers that wish to also
 * implement a Spider WAN compatible interface.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>
#include <sys/cdi.h>
#include <sys/cdi_ioctl.h>
#include <sys/snet/wan_proto.h>
#include <sys/snet/wan_control.h>

#define S_WAN_DESCRIP	"SPIDER WAN COMPATIBILITY MODULE FOR LINUX FAST-STREAMS"
#define S_WAN_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define S_WAN_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define S_WAN_REVISION	"OpenSS7 $RCSfile: s_wan.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $"
#define S_WAN_DEVICE	"SVR 4.2MP CDI CD_HDLC Devices"
#define S_WAN_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define S_WAN_LICENSE	"GPL"
#define S_WAN_BANNER	S_WAN_DESCRIP	"\n" \
			S_WAN_EXTRA	"\n" \
			S_WAN_COPYRIGHT	"\n" \
			S_WAN_REVISION	"\n" \
			S_WAN_DEVICE	"\n" \
			S_WAN_CONTACT	"\n"
#define S_WAN_SPLASH	S_WAN_DESCRIP	" - " \
			S_WAN_REVISION

#ifndef CONFIG_STREAMS_S_WAN_NAME
#error "CONFIG_STREAMS_S_WAN_NAME must be defined."
#endif				/* CONFIG_STREAMS_S_WAN_NAME */
#ifndef CONFIG_STREAMS_S_WAN_MODID
#error "CONFIG_STREAMS_S_WAN_MODID must be defined."
#endif				/* CONFIG_STREAMS_S_WAN_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(S_WAN_CONTACT);
MODULE_DESCRIPTION(S_WAN_DESCRIP);
MODULE_SUPPORTED_DEVICE(S_WAN_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(S_WAN_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-s_wan");
MODULE_ALIAS("streams-module-s_wan");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_S_WAN_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef S_WAN_MOD_NAME
#define S_WAN_MOD_NAME		CONFIG_STREAMS_S_WAN_NAME
#endif				/* S_WAN_MOD_NAME */
#ifndef S_WAN_MOD_ID
#define S_WAN_MOD_ID		CONFIG_STREAMS_S_WAN_MODID
#endif				/* S_WAN_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		S_WAN_MOD_ID
#define MOD_NAME	S_WAN_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	S_WAN_BANNER
#else				/* MODULE */
#define MOD_BANNER	S_WAN_SPLASH
#endif				/* MODULE */

static struct module_info sw_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat sw_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sw_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct priv *priv;

#define PRIV(q) ((struct priv *)q->q_ptr)

#define STRLOGIO    0		/* input-output controls */
#define STRLOGST    1		/* state transitions */
#define STRLOGTO    2		/* timeouts */
#define STRLOGRX    3		/* primitives received */
#define STRLOGTX    4		/* primitives issued */
#define STRLOGTE    5		/* timer events */
#define STRLOGDA    6		/* data */

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
		cd->proto.info.cd_state = newstate;
		mi_strlog(cd->oq, STRLOGST, SL_TRACE, "%s <- %s", cd_statename(newstate),
			  cd_statename(oldstate));
	}
	return (newstate);
}

static cd_ulong
cd_save_state(struct cd *cd)
{
	return ((cd->oldstate = cd_get_state(cd)));
}

static cd_ulong
cd_restore_state(struct cd *cd)
{
	return (cd_set_state(cd, cd->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
sw_m_data(struct sw *sw, queue_t *q, mblk_t *mp)
{
	mi_strlog(sw->oq, STRLOGDA, SL_TRACE, "M_DATA ->");
	return cd_data_req(sw->cd, q, NULL, mp);
}
static fastcall noinline __unlikely int
sw_m_proto(struct sw *sw, queue_t *q, mblk_t *mp)
{
	uint32_t type = 0, cmd = 0;
	int rtn = EINVAL;

	if (!MBLKIN(mp, 0, sizeof(type)))
		goto done;
	type = ((uint32_t *) mp->b_rptr)[0];
	if (!MBLKIN(mp, 0, sizeof(type) + sizeof(cmd)))
		goto done;
	cmd = ((uint32_t *) mp->b_rptr)[1];
	sw_save_state(sw);
	if (likely(type == WAN_DAT && cmd = WAN_TX)) {
		mi_strlog(sw->oq, STRLOGDA, SL_TRACE, "WAN_DAT ->");
		rtn = sw_wan_dat(sw, q, mp);
		goto done;
	}
	mi_strlog(sw->oq, STRLOGTX, SL_TRACE, "%s(%s) ->", sw_primtype(prim), sw_primcmd(cmd));
	switch (type) {
	case WAN_REG:
		rtn = wan_reg(sw, q, mp);
		break;
	case WAN_SID:
		rtn = wan_sid(sw, q, mp);
		break;
	case WAN_CTL:
		switch (cmd) {
		case WAN_CONNECT:
			rtn = wan_connect(sw, q, mp);
			break;
		case WAN_CONCNF:
			rtn = wan_concnf(sw, q, mp);
			break;
		case WAN_DISC:
			rtn = wan_disc(sw, q, mp);
			break;
		case WAN_DISCCNF:
			rtn = wan_disccnf(sw, q, mp);
			break;
		default:
			rtn = wan_other(sq, q, mp);
			break;
		}
		break;
	case WAN_DAT:
		switch (cmd) {
		case WAN_TX:
			rtn = wan_tx(sw, q, mp);
			break;
		case WAN_RX:
			rtn = wan_rx(sw, q, mp);
			break;
		default:
			rtn = wan_other(sq, q, mp);
			break;
		}
		break;
	default:
		rtn = wan_other(sq, q, mp);
		break;
	}
}

static fastcall noinline __unlikely int
cd_m_data(struct cd *cd, queue_t *q, mblk_t *mp)
{
	mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "M_DATA ->");
	return cd_data_ind(cd->cd, q, NULL, mp);
}
static fastcall noinline __unlikely int
cd_m_proto(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong prim = 0;
	int rtn = CD_PROTOSHORT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(cd_ulong *) mp->b_rptr;
	cd_save_state(cd);
	if (likely(prim == CD_UNITDATA_REQ)) {
		mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "CD_UNITDATA_REQ ->");
		rtn = cd_unitdata_req(cd, q, mp);
		goto done;
	}
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "%s ->", cd_primname(prim));
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
	case CD_BAD_FRAME_IND:
	case CD_MODEM_SIG_IND:
		rtn = CD_BADPRIM;
		break;
	default:
		rtn = CD_BADPRIM;
		break;
	}
      done:
	if (rtn)
		cd_restore_state(cd);
	return cd_error_reply(cd, q, mp, prim, rtn);
}
static fastcall noinline __unlikely int
cd_m_ctl(struct cd *cd, queue_t *q, mblk_t *mp)
{
	return cd_m_proto(cd, q, mp);
}
static fastcall noinline __unlikely int
cd_m_sig(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
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
	if (q->q_flag & QREADR) {
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
	} else {
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
	}
	/* reverse sense of flush */
	switch (mp->b_rptr[0] & (FLUSHR | FLUSHW)) {
	case 0:
	case (FLUSHR | FLUSHW):
		break;
	case (FLUSHR):
		mp->b_rptr[0] &= ~(FLUSHR | FLUSHW);
		mp->b_rptr[0] |= FLUSHW;
		break;
	case (FLUSHW):
		mp->b_rptr[0] &= ~(FLUSHR | FLUSHW);
		mp->b_rptr[0] |= FLUSHR;
		break;
	}
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall inline int
sw_w_msg_put(queue_t *q, mblk_t *mp)
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
		return sw_m_flush(q, mp);
	default:
		return sw_m_other(q, mp);
	}
}
static fastcall noinline int
sw_w_msg_srv(queue_t *q, mblk_t *mp)
{
	struct sw *sw = SW_W_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		return sw_m_data(sw, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return sw_m_proto(sw, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return sw_m_data(sw, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sw_m_proto(sw, q, mp);
	case M_CTL:
	case M_PCCTL:
		return sw_m_ctl(sw, q, mp);
	case M_SIG:
	case M_PCSIG:
		return sw_m_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sw_m_rse(q, mp);
	case M_FLUSH:
		return sw_m_flush(q, mp);
	default:
		return sw_m_other(q, mp);
	}
}
static fastcall noinline int
sw_r_msg_srv(queue_t *q, mblk_t *mp)
{
	struct sw *sw = SW_R_PRIV(q);

	if (likely(DB_TYPE(mp) == M_PROTO))
		return sw_m_proto(sw, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return sw_m_data(sw, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sw_m_proto(sw, q, mp);
	case M_SIG:
	case M_PCSIG:
		return sw_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return sw_m_ctl(sw, q, mp);
	case M_RSE:
	case M_PCRSE:
		return sw_m_rse(q, mp);
	case M_FLUSH:
		return sw_m_flush(q, mp);
	default:
		return sw_m_other(q, mp);
	}
}
static fastcall inline int
sw_r_msg_put(queue_t *q, mblk_t *mp)
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
		return sw_m_flush(q, mp);
	default:
		return sw_m_other(q, mp);
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
sw_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sw_w_msg_put(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
sw_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (sw_w_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
sw_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (sw_r_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
sw_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sw_r_msg_put(q, mp))
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

caddr_t sw_opens = NULL;

static streamscall int
sw_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct sw *sww;
	struct sw *swr;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if ((err = mi_open_comm(&sw_opens, sizeof(*p), q, devp, oflags, sflag, crp)))
		return (err);

	p = PRIV(q);
	sww = &p->w_priv;
	swr = &p->r_priv;

	/* initialize private structure */
	sww->priv = p;
	sww->sw = swr;
	sww->oq = q;
	sww->state = CD_DISABLED;
	sww->oldstate = CD_DISABLED;

	sww->proto.info.cd_primitive = CD_INFO_ACK;
	sww->proto.info.cd_state = CD_DISABLED;
	sww->proto.info.cd_max_sdu = STRMAXPSZ;
	sww->proto.info.cd_min_sdu = STRMINPSZ;
	sww->proto.info.cd_class = CD_HDLC;
	sww->proto.info.cd_duplex = CD_FULLDUPLEX;
	sww->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	sww->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	sww->proto.info.cd_addr_length = 0;
	sww->proto.info.cd_ppa_style = CD_STYLE1;

	sww->sigs = CD_DSR;
	sww->alen = 0;

	swr->priv = p;
	swr->sw = sww;
	swr->oq = WR(q);
	swr->state = CD_DISABLED;
	swr->oldstate = CD_DISABLED;

	swr->proto.info.cd_primitive = CD_INFO_ACK;
	swr->proto.info.cd_state = CD_DISABLED;
	swr->proto.info.cd_max_sdu = STRMAXPSZ;
	swr->proto.info.cd_min_sdu = STRMINPSZ;
	swr->proto.info.cd_class = CD_HDLC;
	swr->proto.info.cd_duplex = CD_FULLDUPLEX;
	swr->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	swr->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	swr->proto.info.cd_addr_length = 0;
	swr->proto.info.cd_ppa_style = CD_STYLE1;

	swr->sigs = CD_DSR;
	swr->alen = 0;

	qprocson(q);
	return (0);
}

static streamscall int
sw_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	mi_close_comm(&sw_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit sw_rinit = {
	.qi_putp = &sw_rput,
	.qi_srvp = &sw_rsrv,
	.qi_qopen = &sw_qopen,
	.qi_qclose = &sw_qclose,
	.qi_minfo = &sw_minfo,
	.qi_mstat = &sw_rstat,
};

static struct qinit sw_winit = {
	.qi_putp = &sw_wput,
	.qi_srvp = &sw_wsrv,
	.qi_minfo = &sw_minfo,
	.qi_mstat = &sw_wstat,
};

struct streamtab s_wan_info = {
	.st_rdinit = &sw_rinit,
	.st_wrinit = &sw_winit,
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
MODULE_PARM_DESC(modid, "Module ID for S_WAN.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

struct fmodsw s_wan_fmod = {
	.f_name = MOD_NAME,
	.f_str = &s_wan_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
s_wan_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&s_wan_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
s_wan_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&s_wan_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(s_wan_modinit);
module_exit(s_wan_modexit);

#endif				/* LINUX */
