/*****************************************************************************

 @(#) $RCSfile: cd-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $

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

 Last Modified $Date: 2008-06-13 06:43:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cd-llc.c,v $
 Revision 0.9.2.1  2008-06-13 06:43:56  brian
 - added files

 *****************************************************************************/

#ident "@(#) $RCSfile: cd-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $"

static char const ident[] = "$RCSfile: cd-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $";

/*
 * This is a driver for the IEEE 802.2 Logical Link Control (LLC).  It
 * presents a CDI interface to its users.  Each CDI stream on the upper
 * multiplex represents an LLC station.  These LLC station streams are
 * intended on being linked beneath a DL LLC multiplexing driver that provides
 * the LLC LSAP and Connection Component units.  Enabled upper streams will
 * respond automatically to XID and TEST frames when necessary.
 */

#define _MPS_SOURCE

#include <sys/os7/comapt.h>
#include <sys/cdi.h>

#define LLC_DESCRIP	"SVR 4.2 CDI X.25 LLC DRIVER FOR LINUX FAST-STREAMS"
#define LLC_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define LLC_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define LLC_REVISION	"OpenSS7 $RCSfile: cd-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $"
#define LLC_DEVICE	"SVR 4.2MP CDI Driver (CDI) for IEEE 802.2 LLC"
#define LLC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LLC_LICENSE	"GPL"
#define LLC_BANNER	LLC_DESCRIP	"\n" \
			LLC_EXTRA	"\n" \
			LLC_COPYRIGHT	"\n" \
			LLC_REVISION	"\n" \
			LLC_DEVICE	"\n" \
			LLC_CONTACT	"\n"
#define LLC_SPLASH	LLC_DESCRIP	" - " \
			LLC_REVISION	"\n"

#ifndef CONFIG_STREAMS_LLC_NAME
#error "CONFIG_STREAMS_LLC_NAME must be defined."
#endif				/* CONFIG_STREAMS_LLC_NAME */
#ifndef CONFIG_STREAMS_LLC_MODID
#error "CONFIG_STREAMS_LLC_MODID must be defined."
#endif				/* CONFIG_STREAMS_LLC_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(LLC_CONTACT);
MODULE_DESCRIPTION(LLC_DESCRIP);
MODULE_SUPPORTED_DEVICE(LLC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LLC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LLC_MODID));
MODULE_ALIAS("streams-driver-llc");
MODULE_ALIAS("streams-module-llc");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LLC_MAJOR));
MODULE_ALIAS("/dev/streams/llc");
MODULE_ALIAS("/dev/streams/llc/*");
MODULE_ALIAS("/dev/streams/clone/llc");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0));
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-" __stringify(LLC_CMINOR));
MODULE_ALIAS("/dev/llc");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define DRV_ID	    LLC_DRV_ID
#define DRV_NAME    LLC_DRV_NAME
#define CMAJORS	    LLC_CMAJORS
#define CMAJOR_0    LLC_CMAJOR_0
#define UNITS	    LLC_UNITS
#ifdef MODULE
#define DRV_BANNER  LLC_BANNER
#else				/* MODULE */
#define DRV_BANNER  LLC_SPLASH
#endif				/* MODULE */

static struct module_info cd_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat cd_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat cd_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct cd {
	queue_t *oq;
	cd_ulong state;
	cd_ulong oldstate;
	struct {
		cd_info_ack_t info;
	} proto;
};

static caddr_t cd_opens = NULL;

#define CD_PRIV(q) ((struct cd *)q->q_ptr)

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
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_ACK;
		mp->b_wptr += sizeof(p);
		freemsg(msg);
		cd_set_state(cd, cd_get_state(cd));
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_INFO_ACK");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_ACK;
		mp->b_wptr += sizeof(p);
		freemsg(msg);
		cd_set_state(cd, cd_get_state(cd));
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_INFO_ACK");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_ACK;
		mp->b_wptr += sizeof(p);
		freemsg(msg);
		cd_set_state(cd, cd_get_state(cd));
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_INFO_ACK");
		putnext(cd->oq, mp);
		return (0);
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
			p->cd_primitive = CD_INFO_ACK;
			mp->b_wptr += sizeof(p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = dp;
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
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_disable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INFO_ACK;
			mp->b_wptr += sizeof(p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = dp;
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
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_error_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INFO_ACK;
			mp->b_wptr += sizeof(p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = dp;
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
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INFO_ACK;
			mp->b_wptr += sizeof(p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = dp;
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
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INFO_ACK;
			mp->b_wptr += sizeof(p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = dp;
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
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_bad_frame_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INFO_ACK;
			mp->b_wptr += sizeof(p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = dp;
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
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INFO_ACK;
			mp->b_wptr += sizeof(p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = dp;
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

/*
 * --------------------------------------------------------------------------
 *
 * CDI PROVIDER TO LINUX SENT FRAMES
 *
 * --------------------------------------------------------------------------
 */

static fastcall inline __hot_out int
cd_send_data(struct cd *cd, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len, mblk_t *dp)
{
	cd_unitdata_ack_t *p;
	mblk_t *mp, *rp;
	caddr_t *d;

	if (likely(!!(mp = mi_allocb(q, add_len, BPRI_MED)))) {
		if (likely(!!(rp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			if (likely(canputnext(cd->oq))) {
				d = (typeof(d)) mp->b_wptr;
				bcopy(add_ptr, mp->b_wptr, add_len);
				mp->b_wptr += add_len;
				mp->b_cont = dp;
				if (msg && msg->b_cont == dp)
					msg->b_cont = NULL;
				freemsg(msg);
				/* pass mp to send function */
				DB_TYPE(rp) = M_PROTO;
				p = (typeof(p)) rp->b_wptr;
				p->cd_primitive = CD_UNITDATA_ACK;
				p->cd_state = cd_get_state(cd);
				rp->b_wptr += sizeof(*p);
				mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_UNITDATA_ACK");
				putnext(cd->oq, rp);
				return (0);
			}
			freeb(rp);
			freeb(mp);
			return (-EBUSY);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * LINUX TO CDI PROVIDER RECEIVED FRAMES
 *
 * --------------------------------------------------------------------------
 */

static fastcall inline __hot_in int
cd_recv_data(struct cd *cd, queue_t *q, mblk_t *mp)
{
}

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
	return cd_info_ack(cd, q, mp);
}
static fastcall noinline __unlikely int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (cd->proto.info.cd_ppa_style != CD_STYLE2)
		goto notsupp;
	if (cd_get_state(cd) != CD_UNATTACHED)
		goto outstate;
	if (find_ppa(cd->cd_ppa) == 0)
		goto badppa;
	cd_set_state(cd, CD_ATTACH_PENDING);
	if ((err = cd_ok_ack(cd, q, mp, cd->cd_ppa)))
		goto error;
	return (0);
      badppa:
	err = CD_BADPPA;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ATTACH_REQ, err);
}
static fastcall noinline __unlikely int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	if (cd->proto.info.cd_ppa_style != CD_STYLE2)
		goto notsupp;
	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	cd_set_state(cd, CD_DETACH_PENDING);
	if ((err = cd_ok_ack(cd, q, mp)))
		goto error;
	return (0);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_DETACH_REQ, err);
}
static fastcall noinline __unlikely int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	cd_set_state(cd, CD_ENABLE_PENDING);
	if ((err = cd_enable_con(cd, q, mp)))
		goto error;
	return (0);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ENABLE_REQ, err);
}
static fastcall noinline __unlikely int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_READ_ACTIVE:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
		break;
	default:
		goto outstate;
	}
	cd_set_state(cd, CD_DISABLE_PENDING);
	if ((err = cd_disable_con(cd, q, mp)))
		goto error;
	return (0);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_DISABLE_REQ, err);
}
static fastcall noinline __unlikely int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *mp)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
cd_w_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	return cd_data_req(cd, q, mp);
}
static fastcall noinline __unlikely int
cd_w_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_ulong prim = 0;
	int rtn = CD_PROTOSHORT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(cd_ulong *) mp->b_rptr;
	cd_save_state(cd);
	if (likely(prim = CD_UNITDATA_REQ)) {
		mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "CD_UNITDATA_REQ ->");
		if (unlikely((rtn = cd_unitdata_req(cd, q, mp))))
			goto done;
		return (0);
	}
	mi_strlog(cd->oq, STRLOGRX, SL_TRACE, "%s ->", cd_primname(prim));
	switch (prim) {
	case CD_INFO_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_ATTACH_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_DETACH_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_ENABLE_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_DISABLE_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_ALLOW_INPUT_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_READ_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_UNITDATA_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_WRITE_READ_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_HALT_INPUT_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_ABORT_OUTPUT_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_MUX_NAME_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_MODEM_SIG_REQ:
		rtn = cd_info_req(cd, q, mp);
		break;
	case CD_MODEM_SIG_POLL:
		rtn = cd_info_req(cd, q, mp);
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
	if (rtn) {
		cd_restore_state(cd);
	}
	return cd_error_reply(cd, q, mp, prim, rtn);
}
static fastcall noinline __unlikely int
cd_w_ctl(queue_t *q, mblk_t *mp)
{
	return cd_w_proto(q, mp);	/* for now */
}
static fastcall noinline __unlikely int
cd_w_sig(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_w_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_w_other(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
cd_r_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	return cd_recv_data(cd, q, mp);
}
static fastcall noinline __unlikely int
cd_r_flush(queue_t *q, mblk_t *mp)
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
cd_r_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_ERROR | SL_TRACE, "%s: bad prim on read queue %d", __FUNCTION__,
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

static fastcall inline int
cd_w_msg_put(queue_t *q, mblk_t *mp)
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
		return cd_w_flush(q, mp);
	default:
		return cd_w_other(q, mp);
	}
}
static fastcall noinline int
cd_w_msg_srv(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_W_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		return cd_w_data(cd, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return cd_w_proto(cd, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_w_data(cd, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_w_proto(cd, q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_w_ctl(cd, q, mp);
	case M_SIG:
	case M_PCSIG:
		return cd_w_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return cd_w_rse(q, mp);
	case M_FLUSH:
		return cd_w_flush(q, mp);
	default:
		return cd_w_other(q, mp);
	}
}
static fastcall noinline int
cd_r_msg_srv(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_R_PRIV(q);

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_r_data(cd, q, mp);
	case M_FLUSH:
		return cd_r_flush(q, mp);
	default:
		return cd_r_other(q, mp);
	}
}
static fastcall inline int
cd_r_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return (-EAGAIN);	/* handle from service procedure */
	case M_FLUSH:
		return cd_r_flush(q, mp);
	default:
		return cd_r_other(q, mp);
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
cd_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || cd_w_msg_put(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
cd_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (cd_w_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
cd_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (cd_r_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
cd_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || cd_r_msg_put(q, mp))
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

static streamscall int
cd_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct cd *cd;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if ((err = mi_open_comm(&cd_opens, sizeof(*cd), q, devp, oflags, sflag, crp)))
		return (err);

	cd = CD_PRIV(q);
	bzero(cd, sizeof(*cd));

	/* initialize private structure */
	cd->oq = q;
	cd->state = CD_UNATTACHED;
	cd->oldstate = CD_UNATTACHED;

	cd->proto.info.cd_primitive = CD_INFO_ACK;
	cd->proto.info.cd_state = CD_UNATTACHED;
	cd->proto.info.cd_max_sdu = 135;
	cd->proto.info.cd_min_sdu = 1;
	cd->proto.info.cd_class = CD_LAN;
	cd->proto.info.cd_duplex = CD_FULLDUPLEX;
	cd->proto.info.cd_output_style = CD_PACEDOUTPUT;
	cd->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	cd->proto.info.cd_addr_length = 8;	/* 6 octets MAC + DSAP + SSAP */
	cd->proto.info.cd_ppa_style = CD_STYLE2;

	qprocson(q);
	return (0);
}

static streamscall int
cd_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	mi_close_comm(&cd_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit cd_rinit = {
	.qi_putp = &cd_rput,
	.qi_srvp = &cd_rsrv,
	.qi_qopen = &cd_qopen,
	.qi_qclose = &cd_qclose,
	.qi_minfo = &cd_minfo,
	.qi_mstat = &cd_rstat,
};

static struct qinit cd_winit = {
	.qi_putp = &cd_wput,
	.qi_srvp = &cd_wsrv,
	.qi_minfo = &cd_minfo,
	.qi_mstat = &cd_wstat,
};

struct streamtab llc_info = {
	.st_rdinit = &cd_rinit,
	.st_wrinit = &cd_winit,
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
MODULE_PARM_DESC(modid, "Module ID for LLC.  (0 for allocation)");

#ifdef LFS

struct cdevsw cd_fmod = {
	.d_str = &llc_info,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static int
llc_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&llc_cdev, major)) < 0)
		return (err);
	return (0);
}

static int
llc_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&llc_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

#ifdef LIS

static int
llc_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &llc_info, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NAME)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

static int
llc_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

static __init int
llc_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = llc_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver major %d", DRV_NAME, (int) major);
		return (err);
	}
	return (0);
}

static __exit void
llc_modexit(void)
{
	int err;

	if ((err = llc_unregister_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", DRV_NAME, err);
		return;
	}
	return;
}

module_init(llc_modinit);
module_exit(llc_modexit);

#endif				/* LINUX */
