/*****************************************************************************

 @(#) $RCSfile: cdpmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-09-10 03:50:02 $

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

 Last Modified $Date: 2008-09-10 03:50:02 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cdpmod.c,v $
 Revision 0.9.2.3  2008-09-10 03:50:02  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.2  2008-06-18 16:45:27  brian
 - widespread updates

 Revision 0.9.2.1  2008-05-12 10:37:28  brian
 - added CD pipe module

 *****************************************************************************/

#ident "@(#) $RCSfile: cdpmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-09-10 03:50:02 $"

static char const ident[] = "$RCSfile: cdpmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-09-10 03:50:02 $";

/*
 * This is a CD pipe module.  It pushes over one side of a STREAMS-based pipe
 * to provide an emulated CDI HDLC channel.  Its primary purpose is testing
 * other modules and drivers that use CDI HDLC channels.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>
#include <sys/strsun.h>
#include <sys/cdi.h>

#define CDPMOD_DESCRIP	"HDLC PIPE MODULE FOR LINUX FAST-STREAMS"
#define CDPMOD_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define CDPMOD_COPYRIGHT "Copyright (c) 1997-2008  OpenSS7 Corporaiton.  All Rights Reserved."
#define CDPMOD_REVISION	"OpenSS7 $RCSfile: cdpmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-09-10 03:50:02 $"
#define CDPMOD_DEVICE	"SVR 4.2MP CD PIPE Module (CDPMOD) for HDLC"
#define CDPMOD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CDPMOD_LICENSE	"GPL"
#define CDPMOD_BANNER	CDPMOD_DESCRIP	"\n" \
			CDPMOD_EXTRA	"\n" \
			CDPMOD_COPYRIGHT	"\n" \
			CDPMOD_REVISION	"\n" \
			CDPMOD_DEVICE	"\n" \
			CDPMOD_CONTACT	"\n"
#define CDPMOD_SPLASH	CDPMOD_DESCRIP	" - " \
			CDPMOD_REVISION

#ifndef CONFIG_STREAMS_CDPMOD_NAME
#error "CONFIG_STREAMS_CDPMOD_NAME must be defined."
#endif				/* CONFIG_STREAMS_CDPMOD_NAME */
#ifndef CONFIG_STREAMS_CDPMOD_MODID
#error "CONFIG_STREAMS_CDPMOD_MODID must be defined."
#endif				/* CONFIG_STREAMS_CDPMOD_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(CDPMOD_CONTACT);
MODULE_DESCRIPTION(CDPMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(CDPMOD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CDPMOD_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-chpmod");
MODULE_ALIAS("streams-module-chpmod");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CDPMOD_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef CDPMOD_MOD_NAME
#define CDPMOD_MOD_NAME		CONFIG_STREAMS_CDPMOD_NAME
#endif				/* CDPMOD_MOD_NAME */
#ifndef CDPMOD_MOD_ID
#define CDPMOD_MOD_ID		CONFIG_STREAMS_CDPMOD_MODID
#endif				/* CDPMOD_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		CDPMOD_MOD_ID
#define MOD_NAME	CDPMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	CDPMOD_BANNER
#else				/* MODULE */
#define MOD_BANNER	CDPMOD_SPLASH
#endif				/* MODULE */

static struct module_info cd_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
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

struct priv *priv;
struct cd *cd;

/* Upper and lower interface structure. */
struct cd {
	struct priv *priv;
	struct cd *cd;
	queue_t *oq;
	cd_ulong state;
	cd_ulong oldstate;
	struct {
		cd_info_ack_t info;
		unsigned char add[20];
	} proto;
	cd_ulong sigs;
	cd_ushort alen;
};

struct priv {
	struct cd w_priv;
	struct cd r_priv;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define CD_W_PRIV(q) (&PRIV(q)->w_priv)
#define CD_R_PRIV(q) (&PRIV(q)->r_priv)

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
		p->cd_state = cd->proto.info.cd_state;
		p->cd_max_sdu = cd->proto.info.cd_max_sdu;
		p->cd_min_sdu = cd->proto.info.cd_min_sdu;
		p->cd_class = cd->proto.info.cd_class;
		p->cd_duplex = cd->proto.info.cd_duplex;
		p->cd_output_style = cd->proto.info.cd_output_style;
		p->cd_features = cd->proto.info.cd_features;
		p->cd_addr_length = cd->proto.info.cd_addr_length;
		p->cd_ppa_style = cd->proto.info.cd_ppa_style;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_INFO_ACK");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong prim)
{
	cd_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_OK_ACK;
		p->cd_state = cd_get_state(cd);
		p->cd_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_OK_ACK");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong prim, cd_long error)
{
	cd_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ERROR_ACK;
		p->cd_state = cd->oldstate;
		p->cd_error_primitive = prim;
		p->cd_errno = error < 0 ? CD_SYSERR : error;
		p->cd_explanation = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		cd_restore_state(cd);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ERROR_ACK [%s,%s] %s",
			  cd_primname(prim), cd_errname(error), cd_strerror(error));
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_error_reply(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong prim, cd_long error)
{
	switch (error) {
	case 0:
	case -ENOBUFS:
	case -EBUSY:
	case -EDEADLK:
	case -ENOMEM:
	case -EAGAIN:
	case -ENOSR:
		return (error);
	}
	return cd_error_ack(cd, q, msg, prim, error);
}
static fastcall noinline __unlikely int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_enable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ENABLE_CON;
		p->cd_state = CD_ENABLED;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		cd_set_state(cd, CD_ENABLED);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ENABLE_CON");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_disable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DISABLE_CON;
		p->cd_state = CD_DISABLED;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		cd_set_state(cd, CD_DISABLED);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_DISABLE_CON");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_long error)
{
	cd_error_ind_t *p;
	mblk_t *mp;

	(void) cd_error_ind;
	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ERROR_IND;
		p->cd_state = cd_get_state(cd);
		p->cd_errno = error < 0 ? CD_SYSERR : error;
		p->cd_explanation = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ERROR_IND");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_data_ind(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (likely(canputnext(cd->oq))) {
		if (msg && dp && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "<- M_DATA");
		putnext(cd->oq, dp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall noinline __unlikely int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ack_t *p;
	mblk_t *mp;

	(void) cd_unitdata_ack;
	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_UNITDATA_ACK;
			p->cd_state = cd_get_state(cd);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "<- CD_UNITDATA_ACK");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *msg, caddr_t src_ptr, size_t src_len,
		caddr_t dst_ptr, size_t dst_len, cd_ulong type, cd_ulong priority, mblk_t *dp)
{
	cd_unitdata_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + src_len + dst_len, BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_UNITDATA_IND;
			p->cd_state = cd_get_state(cd);
			p->cd_src_addr_length = src_len;
			p->cd_src_addr_offset = sizeof(*p);
			p->cd_addr_type = type;
			p->cd_priority = priority;
			p->cd_dest_addr_length = dst_len;
			p->cd_dest_addr_offset = sizeof(*p) + src_len;
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "<- CD_UNITDATA_IND");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong error, mblk_t *dp)
{
	cd_bad_frame_ind_t *p;
	mblk_t *mp;

	(void) cd_bad_frame_ind;
	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_BAD_FRAME_IND;
			p->cd_state = cd_get_state(cd);
			p->cd_error = error;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "<- CD_BAD_FRAME_IND");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong ormask, cd_ulong andmask)
{
	cd_modem_sig_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_MODEM_SIG_IND;
			cd->sigs |= ormask;
			cd->sigs &= ~andmask;
			p->cd_sigs = cd->sigs;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
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
	return cd_error_reply(cd, q, mp, CD_ATTACH_REQ, CD_OUTSTATE);
}
static fastcall noinline __unlikely int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	goto notsupp;
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
	cd_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (!MBLKIN(mp, p->cd_dial_offset, p->cd_dial_length))
		goto baddial;
	if (p->cd_dial_type != CD_NODIAL)
		goto baddialtype;
	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	cd_set_state(cd, CD_ENABLE_PENDING);
	if ((err = cd_modem_sig_ind(cd, q, mp, CD_DTR | CD_RTS | CD_DSR, 0)))
		goto error;
	if ((err = cd_modem_sig_ind(cd->cd, q, mp, CD_DCD | CD_CTS, 0)))
		goto error;
	return cd_enable_con(cd, q, mp);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      baddialtype:
	err = CD_BADDIALTYPE;
	goto error;
      baddial:
	err = CD_BADDIAL;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ENABLE_REQ, err);
}
static fastcall noinline __unlikely int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (cd_get_state(cd) != CD_ENABLED)
		goto outstate;
	switch (p->cd_disposal) {
	case CD_FLUSH:
	case CD_WAIT:
	case CD_DELIVER:
		break;
	default:
		goto baddisposal;
	}
	cd_set_state(cd, CD_DISABLE_PENDING);
	if ((err = cd_modem_sig_ind(cd, q, mp, 0, CD_DTR | CD_RTS | CD_DSR)))
		goto error;
	if ((err = cd_modem_sig_ind(cd->cd, q, mp, 0, CD_DCD | CD_CTS)))
		goto error;
	return cd_disable_con(cd, q, mp);
      baddisposal:
	err = CD_BADDISPOSAL;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_DISABLE_REQ, err);
}
static fastcall noinline __unlikely int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	if (cd_get_state(cd) != CD_ENABLED)
		goto outstate;
	cd_set_state(cd, CD_INPUT_ALLOWED);
	return cd_ok_ack(cd, q, mp, CD_ALLOW_INPUT_REQ);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ALLOW_INPUT_REQ, err);
}
static fastcall noinline __unlikely int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_read_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	return cd_ok_ack(cd, q, mp, CD_READ_REQ);
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_READ_REQ, err);
}
static fastcall noinline __unlikely int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	size_t dlen;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (!MBLKIN(mp, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto badaddress;
	if (p->cd_dest_addr_length > cd->proto.info.cd_addr_length)
		goto badaddress;
	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
	case CD_READ_ACTIVE:
		break;
	default:
		goto outstate;
	}
	switch (p->cd_addr_type) {
	case CD_SPECIFIC:
		goto notsupp;
	case CD_BROADCAST:
		goto notsupp;
	case CD_IMPLICIT:
		break;
	default:
		goto badaddrtype;
	}
	dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	if (dlen > cd->proto.info.cd_max_sdu)
		goto writefail;
	if (dlen < cd->proto.info.cd_min_sdu)
		goto writefail;
	if (cd_get_state(cd->cd) == CD_DISABLED)
		goto writefail;
	return cd_unitdata_ind(cd->cd, q, mp,
			       (caddr_t) cd->proto.add, cd->alen,
			       (caddr_t) (mp->b_rptr + p->cd_dest_addr_offset),
			       p->cd_dest_addr_length, p->cd_addr_type, p->cd_priority, mp->b_cont);
      writefail:
	err = CD_WRITEFAIL;
	goto error;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      badaddrtype:
	err = CD_BADADDRTYPE;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      badaddress:
	err = CD_BADADDRESS;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_UNITDATA_REQ, err);
}
static fastcall noinline __unlikely int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_write_read_req_t *p = (typeof(p)) mp->b_rptr;
	cd_unitdata_req_t *u = &p->cd_unitdata_req;
	size_t dlen;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (!MBLKIN(mp, u->cd_dest_addr_offset, u->cd_dest_addr_length))
		goto badaddress;
	if (u->cd_dest_addr_length > cd->proto.info.cd_addr_length)
		goto badaddress;
	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
	case CD_READ_ACTIVE:
		break;
	default:
		goto outstate;
	}
	dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	if (dlen > cd->proto.info.cd_max_sdu)
		goto writefail;
	if (dlen < cd->proto.info.cd_min_sdu)
		goto writefail;
	if (cd_get_state(cd->cd) == CD_DISABLED)
		goto writefail;
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      writefail:
	err = CD_WRITEFAIL;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      badaddress:
	err = CD_BADADDRESS;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_WRITE_READ_REQ, err);
}
static fastcall noinline __unlikely int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_halt_input_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
		break;
	default:
		goto outstate;
	}
	switch (p->cd_disposal) {
	case CD_FLUSH:
	case CD_WAIT:
	case CD_DELIVER:
	default:
		goto baddisposal;
	}
	cd_set_state(cd, CD_ENABLED);
	return cd_ok_ack(cd, q, mp, CD_HALT_INPUT_REQ);
      baddisposal:
	err = CD_BADDISPOSAL;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_HALT_INPUT_REQ, err);
}
static fastcall noinline __unlikely int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
		break;
	case CD_OUTPUT_ACTIVE:
		cd_set_state(cd, CD_ENABLED);
		break;
	default:
		goto outstate;
	}
	return cd_ok_ack(cd, q, mp, CD_ABORT_OUTPUT_REQ);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ABORT_OUTPUT_REQ, err);

}
static fastcall noinline __unlikely int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	return cd_error_reply(cd, q, mp, CD_MUX_NAME_REQ, CD_NOTSUPP);
}
static fastcall noinline __unlikely int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	switch (cd_get_state(cd)) {
	case CD_UNATTACHED:
	case CD_UNUSABLE:
		goto outstate;
	default:
		break;
	}
	cd->sigs |= p->cd_sigs & (CD_DTR | CD_RTS);
	cd->sigs &= ~(CD_DTR | CD_RTS) | p->cd_sigs;
	if ((err =
	     cd_modem_sig_ind(cd, q, mp, cd->sigs & (CD_DTR | CD_RTS),
			      ~cd->sigs & (CD_DTR | CD_RTS))))
		goto error;
	return cd_ok_ack(cd, q, mp, CD_MODEM_SIG_REQ);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_MODEM_SIG_REQ, err);
}
static fastcall noinline __unlikely int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	switch (cd_get_state(cd)) {
	case CD_UNATTACHED:
	case CD_UNUSABLE:
		goto outstate;
	default:
		break;
	}
	return cd_modem_sig_ind(cd, q, mp, 0, 0);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_MODEM_SIG_REQ, err);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */
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
		return cd_m_flush(q, mp);
	default:
		return cd_m_other(q, mp);
	}
}
static fastcall noinline int
cd_w_msg_srv(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_W_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		return cd_m_data(cd, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return cd_m_proto(cd, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_m_data(cd, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_m_proto(cd, q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_m_ctl(cd, q, mp);
	case M_SIG:
	case M_PCSIG:
		return cd_m_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return cd_m_rse(q, mp);
	case M_FLUSH:
		return cd_m_flush(q, mp);
	default:
		return cd_m_other(q, mp);
	}
}
static fastcall noinline int
cd_r_msg_srv(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_R_PRIV(q);

	if (likely(DB_TYPE(mp) == M_PROTO))
		return cd_m_proto(cd, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_m_data(cd, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_m_proto(cd, q, mp);
	case M_SIG:
	case M_PCSIG:
		return cd_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_m_ctl(cd, q, mp);
	case M_RSE:
	case M_PCRSE:
		return cd_m_rse(q, mp);
	case M_FLUSH:
		return cd_m_flush(q, mp);
	default:
		return cd_m_other(q, mp);
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
		return cd_m_flush(q, mp);
	default:
		return cd_m_other(q, mp);
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

caddr_t cd_opens = NULL;

static streamscall int
cd_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct cd *cdw;
	struct cd *cdr;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if ((err = mi_open_comm(&cd_opens, sizeof(*p), q, devp, oflags, sflag, crp)))
		return (err);

	p = PRIV(q);
	cdw = &p->w_priv;
	cdr = &p->r_priv;

	/* initialize private structure */
	cdw->priv = p;
	cdw->cd = cdr;
	cdw->oq = q;
	cdw->state = CD_DISABLED;
	cdw->oldstate = CD_DISABLED;

	cdw->proto.info.cd_primitive = CD_INFO_ACK;
	cdw->proto.info.cd_state = CD_DISABLED;
	cdw->proto.info.cd_max_sdu = STRMAXPSZ;
	cdw->proto.info.cd_min_sdu = STRMINPSZ;
	cdw->proto.info.cd_class = CD_HDLC;
	cdw->proto.info.cd_duplex = CD_FULLDUPLEX;
	cdw->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	cdw->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	cdw->proto.info.cd_addr_length = 0;
	cdw->proto.info.cd_ppa_style = CD_STYLE1;

	cdw->sigs = CD_DSR;
	cdw->alen = 0;

	cdr->priv = p;
	cdr->cd = cdw;
	cdr->oq = WR(q);
	cdr->state = CD_DISABLED;
	cdr->oldstate = CD_DISABLED;

	cdr->proto.info.cd_primitive = CD_INFO_ACK;
	cdr->proto.info.cd_state = CD_DISABLED;
	cdr->proto.info.cd_max_sdu = STRMAXPSZ;
	cdr->proto.info.cd_min_sdu = STRMINPSZ;
	cdr->proto.info.cd_class = CD_HDLC;
	cdr->proto.info.cd_duplex = CD_FULLDUPLEX;
	cdr->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	cdr->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	cdr->proto.info.cd_addr_length = 0;
	cdr->proto.info.cd_ppa_style = CD_STYLE1;

	cdr->sigs = CD_DSR;
	cdr->alen = 0;

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

struct streamtab cdpmod_info = {
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
MODULE_PARM_DESC(modid, "Module ID for CDPMOD.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

struct fmodsw cd_fmod = {
	.f_name = MOD_NAME,
	.f_str = &cdpmod_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
cdpmod_modinit(void)
{
	int err;

	(void) cd_error_ind;
	(void) cd_unitdata_ack;
	(void) cd_bad_frame_ind;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&cd_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
cdpmod_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&cd_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(cdpmod_modinit);
module_exit(cdpmod_modexit);

#endif				/* LINUX */
