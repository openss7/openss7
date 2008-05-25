/*****************************************************************************

 @(#) $RCSfile: lcd.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:56 $

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

 Last Modified $Date: 2008-05-25 12:46:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lcd.c,v $
 Revision 0.9.2.1  2008-05-25 12:46:56  brian
 - added manual pages, libraries, utilities and drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: lcd.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:56 $"

static char const ident[] = "$RCSfile: lcd.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:56 $";

/*
 * This is a Linux Communications Device (LCD) driver.  This driver provides
 * Communications Device Interface (CDI) interface to various devices in the
 * Linux packet system, such as LAN devices (CD_LAN), and SDLC/HDLC Framing
 * devices (CD_HDLC).  For SDLC/HDLC framing devices, frames can be delivered
 * and sent raw, or with address fields based on a link access protocol.  For
 * LAN devices, frames can be delivered and set raw, or with Ethernet II
 * framing, or with LLC framing, or with SNAP LLC framing.
 *
 * For Linux Ethernet, it would be typical to do LLC Type 1 and 2 over SNAP
 * LLC framing as this is the only way to do ISO 8022 and Ethernet II on the
 * same newtork device.
 *
 * For converting CDI providers for use under Linux, see the cddev driver.
 */

#define _MPS_SOURCE		/* For MPS utilities, helper and convenience functions. */

#include <sys/os7/compat.h>	/* For STREAMS compatibility. */
#include <sys/strsun.h>		/* For SUN convenience macros. */
#include <sys/cdi.h>		/* For CDI interface definitions. */

#define	LCD_DESCRIP	"SVR 4.2 COMMUNICATIONS DEVICE DRIVER FOR LINUX FAST-STREAMS"
#define	LCD_EXTRA	"Part of X/Open Network Services for Linux Fast-STREAMS"
#define	LCD_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define	LCD_REVISION	"OpenSS7 $RCSfile: lcd.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:56 $"
#define	LCD_DEVICE	"SVR 4.2MP Communications Device (CD) for Linux Fast-STREAMS"
#define	LCD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define	LCD_LICENSE	"GPL"
#define	LCD_BANNER	LCD_DESCRIP	"\n" \
			LCD_EXTRA	"\n" \
			LCD_REVISION	"\n" \
			LCD_COPYRIGHT	"\n" \
			LCD_DEVICE	"\n" \
			LCD_CONTACT
#define LCD_SPLASH	LCD_DEVICE	"\n" \
			LCD_REVISION

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(LCD_CONTACT);
MODULE_DESCRIPTION(LCD_DESCRIP);
MODULE_SUPPORTED_DEVICE(LCD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LCD_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-" CONFIG_STREAMS_LCD_NAME);
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LCD_MODID));
MODULE_ALIAS("streams-driver-" CONFIG_STREAMS_LCD_NAME);
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LCD_MAJOR));
MODULE_ALIAS("/dev/streams/lcd");
MODULE_ALIAS("/dev/streams/lcd/*");
MODULE_ALIAS("/dev/streams/lcd/hdlc");
MODULE_ALIAS("/dev/streams/lcd/sdlc");
MODULE_ALIAS("/dev/streams/lcd/mac");
MODULE_ALIAS("/dev/streams/lcd/lan");
MODULE_ALIAS("/dev/streams/clone/lcd");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(LCD_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(LCD_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(LCD_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(LCD_CMAJOR_0) "-1");
MODULE_ALIAS("char-major-" __stringify(LCD_CMAJOR_0) "-2");
MODULE_ALIAS("char-major-" __stringify(LCD_CMAJOR_0) "-3");
MODULE_ALIAS("char-major-" __stringify(LCD_CMAJOR_0) "-4");
MODULE_ALIAS("/dev/lcd");
MODULE_ALIAS("/dev/hdlc");
MODULE_ALIAS("/dev/sdlc");
MODULE_ALIAS("/dev/lan");
MODULE_ALIAS("/dev/mac");
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

#ifdef LFS
#define LCD_DRV_ID	CONFIG_STREAMS_LCD_MODID
#define LCD_DRV_NAME	CONFIG_STREAMS_LCD_NAME
#define LCD_CMAJORS	CONFIG_STREAMS_LCD_NMAJORS
#define LCD_CMAJOR_0	CONFIG_STREAMS_LCD_MAJOR
#define LCD_UNITS	CONFIG_STREAMS_LCD_NMINORS
#endif				/* LFS */

#define DRV_ID		LCD_DRV_ID
#define DRV_NAME	LCD_DRV_NAME
#define CMAJORS		LCD_CMAJORS
#define CMAJOR_0	LCD_CMAJOR_0
#define UNITS		LCD_UNITS

#ifdef MODULE
#define DRV_BANNER	LCD_BANNER
#else				/* MODULE */
#define DRV_BANNER	LCD_SPLASH
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
	struct packet_type pt;		/* must be first */
	minor_t cminor;
	queue_t *oq;			/* output queue (read queue) */
	cd_ulong state;
	cd_ulong oldstate;
	struct {
		struct sk_buff_head q; /* rx or tx queue */
		atomic_t buf; /* rx or tx current buffer */
		int max; /* rx or tx maximum buffer */
	} rx, tx;
	struct sk_buff_head rxq;	/* receive queue */
	struct sk_buff_head txq;	/* transmit queue */
	atomic_t rcvbuf;		/* current receive buffer */
	atomic_t xmtbuf;		/* current transmit buffer */
	int rcvmax;			/* maximum receive buffer */
	int xmtmax;			/* maximum transmit buffer */
	struct {
		cd_info_ack_t info;
	} proto;
	cd_ulong ppa;
	cd_ulong disposal;
	cd_ulong msec;
};

#define CD_PRIV(q) ((struct cd *)q->q_ptr)

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
#if 0
	case CD_ATTACH_PENDING:
		return ("CD_ATTACH_PENDING");
	case CD_DETACH_PENDING:
		return ("CD_DETACH_PENDING");
#endif
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

/*
 * --------------------------------------------------------------------------
 *
 * DEVICE FUNCTIONS
 *
 * --------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
cd_attach_dev(struct cd *cd)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI PROVIDER TO CDI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */
static fastcall inline __hot_read int
cd_data_ind(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (pcmsg(DB_TYPE(dp)) || bcanputnext(cd->oq, dp->b_band)) {
		if (msg && msg->b_cont == dp)
			msg->m_cont = NULL;
		freemsg(msg);
		mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "<- M_DATA");
		putnext(cd->oq, dp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall noinline __unlikely int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_info_ack_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_INFO_ACK;
	p->cd_state = cd_get_state(cd);
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
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong prim)
{
	cd_ok_ack_t *p;
	mblk_t *mp;
	int err;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	switch (prim) {
	case CD_ATTACH_REQ:
		if ((err = cd_attach_dev(cd)))
			goto error;
		cd_set_state(cd, CD_DISABLED);
		break;
	case CD_DETACH_REQ:
		if ((err = cd_detach_dev(cd)))
			goto error;
		cd_set_state(cd, CD_UNATTACHED);
		break;
	case CD_ALLOW_INPUT_REQ:
		if (cd_get_state(cd) != CD_INPUT_ALLOWED) {
			/* FIXME: actually allow input if possible */
			cd_set_state(cd, CD_INPUT_ALLOWED);
		}
		break;
	case CD_HALT_INPUT_REQ:
		if (cd_get_state(cd) != CD_ENABLED) {
			/* FIXME: actually halt input if possible */
			cd_set_state(cd, CD_ENABLED);
		}
		break;
	case CD_ABORT_OUTPUT_REQ:
		if (cd_get_state(cd) == CD_OUTPUT_ACTIVE) {
			/* FIXME: actually abort output if possible */
			cd_set_state(cd, CD_INPUT_ALLOWED);
		}
		break;
	case CD_READ_REQ:
		if (cd_get_state(cd) != CD_INPUT_ALLOWED) {
			/* FIXME: actually allow input if possible */
			cd_set_state(cd, CD_INPUT_ALLOWED);
		}
		/* FIXME: only enable input section for cd->msec */
		cd_set_state(cd, CD_ENABLED);
		break;
	default:
		break;
	}
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
      error:
	freeb(mp);
	return (err);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong prim, cd_long error)
{
	cd_error_ack_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_ERROR_ACK;
	p->cd_state = cd_get_state(cd);
	p->cd_error_primitive = prim;
	p->cd_errno = error < 0 ? CD_SYSERR : error;
	p->cd_explanation = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	switch (cd_get_state(cd)) {
	case CD_ENABLE_PENDING:
		p->cd_state = cd_set_state(cd, CD_DISABLED);
		goto error;
	case CD_DISABLE_PENDING:
		p->cd_state = cd_set_state(cd, CD_ENABLED);
		goto error;
	default:
		break;
	}
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ERROR_ACK");
	putnext(cd->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_enable_con_t *p;
	mblk_t *mp;
	int err;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	if (unlikely((err = cd_enable_dev(cd))))
		goto error;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_ENABLE_CON;
	p->cd_state = cd_get_state(cd);
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	p->cd_state = cd_set_state(cd, CD_ENABLED);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ENABLE_CON");
	putnext(cd->oq, mp);
	return (0);
      error:
	freeb(mp);
	return (err);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_disable_con_t *p;
	mblk_t *mp;
	int err;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	if (unlikely((err = cd_disable_dev(cd))))
		goto error;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_DISABLE_CON;
	p->cd_state = cd_get_state(cd);
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	p->cd_state = cd_set_state(cd, CD_DISABLED);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_DISABLE_CON");
	putnext(cd->oq, mp);
	return (0);
      error:
	freeb(mp);
	return (err);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_long newstate, cd_long error)
{
	cd_error_ind_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_ERROR_IND;
	p->cd_state = cd_get_state(cd);
	p->cd_errno = error < 0 ? CD_SYSERR : error;
	p->cd_explanation = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	if (newstate != -1)
		p->cd_state = cd_set_state(cd, newstate);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "<- CD_ERROR_IND");
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall inline __hot_read int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ack_t *p;
	mblk_t *mp;
	int err;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_UNITDATA_ACK;
	p->cd_state = cd_get_state(cd);
	mp->b_wptr += sizeof(*p);
      error:
	freeb(mp);
	return (err);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ind_t *p;
	mblk_t *mp;
}
static fastcall noinline __unlikely int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_bad_frame_ind_t *p;
	mblk_t *mp;
}
static fastcall noinline __unlikely int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_ind_t *p;
	mblk_t *mp;
}

/*
 * --------------------------------------------------------------------------
 *
 * LINUX TO CDI PROVIDER RECEIVED PACKETS
 *
 * --------------------------------------------------------------------------
 */

/**
 * cd_rcv_packet: - receive packet from Linux subsystem
 * @skb: socket buffer containing packet
 * @dev: Linux networking device
 * @pt: our registered packet type pointer
 * @dev2: sometimes another device (e.g. a master device)
 *
 * Linux passes us the packet_type structure pointer that we provided to the dev_add_pack()
 * registration call.  This is an overloaded structure that is contained within our
 * communications device private structure.  Provided that the packet is one in which we are
 * intereseted and the receive buffer is not overflowing, simply queue the packet to the device
 * private structure and enable the input queue on the device.
 *
 * ETH_P_802_3 is all packets received on an ethernet device that have a length/type field less
 * than or equal to 1500.  These are true 802.3 frames with all bytes (i.e. DSAP/SSAP) following
 * the length field included in the data.
 *
 * ETH_P_802_2 and ETH_P_TR_802_2 is all
 *
 * ETH_P_SNAP frames are all packets received on any interface that can include a snap header.
 */
static int
#ifdef HAVE_KMEMB_STRUCT_PACKET_TYPE_FUNC_4_ARGS
cd_rcv_packet(struct sk_buff *skb, struct cddev *dev, struct packet_type *pt, struct cddev *dev2)
#else					/* HAVE_KMEMB_STRUCT_PACKET_TYPE_FUNC_4_ARGS */
cd_rcv_packet(struct sk_buff *skb, struct cddev *dev, struct packet_type *pt)
#endif					/* HAVE_KMEMB_STRUCT_PACKET_TYPE_FUNC_4_ARGS */
{
	struct cd *cd = (typeof(cd)) pt;

	if (unlikely(skb->pkt_type == PACKET_OTHERHOST))
		goto drop;
	/* Note that if we want later packet handlers to drop the packet, just set the packet
	 * type to PACKET_OTHERHOST. */

	switch (skb->protocol) {
	case ETH_P_802_3: /* IPX over 802.3, 0xFFFF as DSAP/SSAP. */
	case ETH_P_802_2: /* 802.2 off of VLAN, IPX, FDDI, Ethernet with type/len < 1536 (may have SNAP) */
	case ETH_P_TR_802_2: /* 802.2 off of TR (may have SNAP) */
	}
	switch (skb->protocol) {
	case ETH_P_LOOP:
	case ETH_P_PUP:
	case ETH_P_PUPAT:
	case ETH_P_IP:
	case ETH_P_X25:
	case ETH_P_ARP:
	case ETH_P_BPQ:
	case ETH_P_IEEEPUP:
	case ETH_P_IEEEPUPAT:
	case ETH_P_DEC:
	case ETH_P_DNA_DL:
	case ETH_P_DNA_RC:
	case ETH_P_DNA_RT:
	case ETH_P_LAT:
	case ETH_P_DIAG:
	case ETH_P_CUST:
	case ETH_P_SCA:
	case ETH_P_RARP:
	case ETH_P_ATALK:
	case ETH_P_8021Q:	/* 802.1Q VLAN Extended Header */
	case ETH_P_IPX:
	case ETH_P_IPV6:
	case ETH_P_SLOW:	/* 802.3ad 43B */
	case ETH_P_WCCP:
	case ETH_P_PPP_DISC:
	case ETH_P_PPP_SES:
	case ETH_P_MPLS_UC:
	case ETH_P_MPLS_MC:
	case ETH_P_ATMMPOA:
	case ETH_P_ATMFATE:
	case ETH_P_AOE:
	case ETH_P_TIPC:
		break;
	case ETH_P_802_3:	/* 802.3 */
	case ETH_P_AX25:
	case ETH_P_ALL:
	case ETH_P_802_2:	/* 802.2 */
	case ETH_P_SNAP:	/* IEEE SNAP */
	case ETH_P_DDCMP:
	case ETH_P_WAN_PPP:	/* WAN PPP frames */
	case ETH_P_PPP_MP:	/* PPP MP frames */
	case ETH_P_LOCALTALK:
	case ETH_P_PPPTALK:	/* Atalk over PPP */
	case ETH_P_TR_802_2:	/* 802.2 token ring */
	case ETH_P_MOBITEX:
	case ETH_P_CONTROL:
	case ETH_P_IRDA:
	case ETH_P_ECONET:
	case ETH_P_HDLC:
	case ETH_P_ARCNET:
		break;
	}
	if (atomic_read(&cd->rx.buf) > cd->rx.max)
		goto drop;
	atomic_add(&cd->rx.buf, skb->len);
	skb_queue_tail(&cd->rx.q, skb);
	qenable(cd->oq);
	return (0);
      drop:
	kfree_skb(skb);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI USER TO CDI PROVIDER RECEIVED PRIMITIVES (UPPER MULTIPLEX)
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
cd_data_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err;

	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
	case CD_READ_ACTIVE:
		break;
	case CD_DISABLED:
		goto disc;
	case CD_UNUSABLE:
		goto fatalerr;
	default:
		goto outstate;
	}
	if (cd->proto.info.cd_output_style == CD_ACKEDOUTPUT)
		if ((err = cd_unitdata_ack(cd, q, mp)))
			goto error;
	/* FIXME: actually write data to device */
	freemsg(mp);
	return (0);
      disc:
	err = CD_DISC;
	goto error;
      fatalerr:
	err = CD_FATALERR;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_UNITDATA_REQ, err);
}
static fastcall noinline __unlikely int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	return cd_info_ack(cd, q, mp);
}

/**
 * cd_attach_req: - process CD_ATTACH_REQ primitive
 * @cd: communications device private structure
 * @q: active queue (upper write queue)
 * @mp: the CD_ATTACH_REQ primitive
 *
 * Attaches a CDI stream to a Linux network device by ppa.  See cd_attach_dev() for a
 * description of the PPA format an how it is used to locate a Linux network device.  Note that
 * the linux network device must not already be attached to another PPA.
 */
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
	if ((err = cd_attach_dev(cd, p->cd_ppa)))
		goto error;
	cd->ppa = p->cd_ppa;
	/* cd_ok_ack does all the work */
	return cd_ok_ack(cd, q, mp, CD_ATTACH_REQ);
      deadlk:
	err = -EDEADLK;
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
	struct cddev *dev;
	int err;

	if (cd->proto.info.cd_ppa_style != CD_STYLE2)
		goto notsupp;
	if (cd_get_state(cd) == CD_UNUSABLE)
		goto fatalerr;
	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	/* cd_ok_ack does all the work */
	return cd_ok_ack(cd, q, mp, CD_DETACH_REQ);
      deadlk:
	err = -EDEADLK;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      fatalerr:
	err = CD_FATALERR;
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
		goto protoshort;
	if (cd_get_state(cd) == CD_UNUSABLE)
		goto fatalerr;
	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	if ((cd->proto.info.cd_features & CD_CANDIAL) == 0) {
		if (p->cd_dial_type != CD_NODIAL)
			goto baddialtype;
		if (p->cd_dial_length != 0)
			goto baddial;
	}
	cd_set_state(cd, CD_ENABLE_PENDING);
	/* cd_enable_con does the work */
	return cd_enable_con(cd, q, mp);
      baddial:
	err = CD_BADDIAL;
	goto error;
      baddialtype:
	err = CD_BADDIALTYPE;
	goto error;
      oustate:
	err = CD_OUTSTATE;
	goto error;
      fatalerr:
	err = CD_FATALERR;
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
	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_READ_ACTIVE:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
		break;
	case CD_UNUSABLE:
		goto fatalerr;
	default:
		goto outstate;
	}
	switch (p->cd_disposal) {
	case CD_FLUSH:
	case CD_WAIT:
	case CD_DELIVER:
		break;
	default:
		goto baddisposal;
	}
	cd->disposal = p->cd_disposal;
	cd_set_state(cd, CD_DISABLE_PENDING);
	/* cd_disable_con does all the work */
	return cd_disable_con(cd, q, mp);
      baddisposal:
	err = CD_BADDISPOSAL;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      fatalerr:
	err = CD_FATALERR;
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

	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
		break;
	case CD_UNUSABLE:
		goto fatalerr;
	case CD_DISABLED:
		goto disc;
	default:
		goto outstate;
	}
	goto notsupp;		/* for now */
	return cd_ok_ack(cd, q, mp, CD_ALLOW_INPUT_REQ);
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      disc:
	err = CD_DISC;
	goto error;
      fatalerr:
	err = CD_FATALERR;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ALLOW_INPUT_REQ, err);

}
static fastcall noinline __unlikely int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_read_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
		break;
	case CD_UNUSABLE:
		goto fatalerr;
	case CD_DISABLED:
		goto disc;
	default:
		goto outstate;
	}
	goto notsupp;
	cd->msec = p->cd_msec;
	return cd_ok_ack(cd, q, mp, CD_READ_REQ);
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      disc:
	err = CD_DISC;
	goto error;
      fatalerr:
	err = CD_FATALERR;
	goto error;
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
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (!MBLKIN(mp, p->cd_dest_addr_offset, p->cd_dest_addr_length))
		goto protoshort;
	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
	case CD_READ_ACTIVE:
		break;
	case CD_DISABLED:
		goto disc;
	case CD_UNUSABLE:
		goto fatalerr;
	default:
		goto outstate;
	}
	switch (p->cd_addr_type) {
	case CD_SPECIFIC:
	case CD_BROADCAST:
	case CD_IMPLICIT:
		break;
	default:
		goto badaddrtype;
	}
	if (p->cd_priority > 100)
		p->cd_priority = 100;
	if (p->cd_addr_type == CD_SPECIFIC) {
		if (p->cd_dest_addr_length != cd->proto.info.cd_addr_length)
			goto badaddress;
	} else {
		if (p->cd_dest_addr_length != 0)
			goto badaddress;
	}
	if (cd->proto.info.cd_output_style == CD_ACKEDOUTPUT)
		if ((err = cd_unitdata_ack(cd, q, mp)))
			goto error;
	/* FIXME: actually write unitdata to device */
	freemsg(mp);
	return (0);
      badaddress:
	err = CD_BADADDRESS;
	goto error;
      badaddrtype:
	err = CD_BADADDRTYPE;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      fatalerr:
	err = CD_FATALERR;
	goto error;
      disc:
	err = CD_DISC;
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
	cd_ulong prim = CD_WRITE_READ_REQ;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (!MBLKIN(mp, p->cd_unitdata_req.cd_dest_addr_offset,
		    p->cd_unitdata_req.cd_dest_addr_length))
		goto protoshort;
	if (p->cd_unitdata_req.cd_primitive == CD_UNITDATA_REQ) {
		prim = CD_UNITDATA_REQ;
		switch (cd_get_state(cd)) {
		case CD_ENABLED:
		case CD_INPUT_ALLOWED:
		case CD_OUTPUT_ACTIVE:
		case CD_READ_ACTIVE:
			break;
		case CD_DISABLED:
			goto disc;
		case CD_UNUSABLE:
			goto fatalerr;
		default:
			goto outstate;
		}
		switch (p->cd_unitdata_req.cd_addr_type) {
		case CD_SPECIFIC:
		case CD_BROADCAST:
		case CD_IMPLICIT:
			break;
		default:
			goto badaddrtype;
		}
		if (p->cd_unitdata_req.cd_priority > 100)
			p->cd_unitdata_req.cd_priority = 100;
		if (p->cd_addr_type == CD_SPECIFIC) {
			if (p->cd_unitdata_req.cd_dest_addr_length != cd->proto.info.cd_addr_length)
				goto badaddress;
		} else {
			if (p->cd_unitdata_req.cd_dest_addr_length != 0)
				goto badaddress;
		}
		if (cd->proto.info.cd_output_style == CD_ACKEDOUTPUT)
			if ((err = cd_unitdata_ack(cd, q, mp)))
				goto error;
		/* FIXME: actually write unitdata to device */
		p->cd_unitdata_req.cd_primitive = 0;
	}
	prim = CD_READ_REQ;
	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
		break;
	default:
		goto outstate;
	}
	goto notsupp;
	cd->msec = p->cd_msec;
	return cd_ok_ack(cd, q, mp, prim);
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      badaddress:
	err = CD_BADADDRESS;
	goto error;
      badaddrtype:
	err = CD_BADADDRTYPE;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      fatalerr:
	err = CD_FATALERR;
	goto error;
      disc:
	err = CD_DISC;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, prim, err);
}
static fastcall noinline __unlikely int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_halt_input_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	switch (cd_get_state(cd)) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
		break;
	case CD_UNUSABLE:
		goto fatalerr;
	case CD_DISABLED:
		goto disc;
	default:
		goto outstate;
	}
	switch (p->cd_disposal) {
	case CD_FLUSH:
	case CD_WAIT:
	case CD_DELIVER:
		break;
	default:
		goto baddisposal;
	}
	cd->disposal = p->cd_disposal;
	goto notsupp;		/* for now */
	return cd_ok_ack(cd, q, mp, CD_HALT_INPUT_REQ);
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      baddisposal:
	err = CD_BADDISPOSAL;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      disc:
	err = CD_DISC;
	goto error;
      fatalerr:
	err = CD_FATALERR;
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
	case CD_OUTPUT_ACTIVE:
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
		break;
	case CD_UNUSABLE:
		goto fatalerr;
	case CD_DISABLED:
		goto disc;
	default:
		goto oustate;
	}
	goto notsupp;
	return cd_ok_ack(cd, q, mp, CD_ABORT_OUTPUT_REQ);
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      disc:
	err = CD_DISC;
	goto error;
      fatalerr:
	err = CD_FATALERR;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_ABORT_OUTPUT_REQ, err);
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
static fastcall noinline __unlikely int
cd_wrongway_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong prim = *(cd_ulong *) mp->b_rptr;

	return cd_error_reply(cd, q, mp, prim, CD_BADPRIM);
}
static fastcall noinline __unlikely int
cd_other_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong prim = *(cd_ulong *) mp->b_rptr;

	return cd_error_reply(cd, q, mp, prim, CD_BADPRIM);
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI PROVIDER TO CDI USER RECEIVED PRIMITIVES (UPPER MULTIPLEX)
 *
 * --------------------------------------------------------------------------
 * These are primitives received from below on the upper multiplex.  These
 * messages only appear here when they are placed here by the lower multiplex.
 * (Note that when the internal Linux network device interface is used,
 * messages are passed to the next module up (downstream) instead.
 */
static fastcall inline __hot_in int
cd_r_data_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_info_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_ok_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_error_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_enable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_disable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_error_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_wrongway_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_r_other_ind(struct cd *cd, queue_t *q, mblk_t *mp)
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
	struct cd *cd = CD_PRIV(q);	/* already locked */

	return cd_data_req(cd, q, mp);
}

static fastcall noinline __unlikely int
cd_w_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);	/* already locked */
	cd_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto fault;
	prim = *(cd_ulong *) mp->b_rptr;

	cd_save_state(cd);
	switch (prim) {
	}
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
		rtn = cd_wrongway_req(cd, q, mp);
		break;
	default:
		rtn = cd_other_req(cd, q, mp);
		break;
	}
      done:
	if (rtn)
		cd_restore_state(cd);
	switch (rtn) {
	case 0:
	case -ENOMEM:
	case -ENOBUFS:
	case -EAGAIN:
	case -ENOSR:
	case -EDEADLK:
		return (rtn);
	}
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
cd_w_ioctl(queue_t *q, mblk_t *mp)
{
}

static fastcall noinline __unlikely int
cd_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (q->q_next) {
			putnext(q, mp);
			return (0);
		}
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

static fastcall noinline int
cd_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return (-EAGAIN);
	case M_PROTO:
	case M_PCPROTO:
		return (-EAGAIN);
	case M_IOCTL:
	case M_IOCDATA:
		return (-EAGAIN);
	case M_FLUSH:
		return cd_w_flush(q, mp);
	}
}
static fastcall noinline int
cd_wsrv_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_w_proto(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return cd_w_ioctl(q, mp);
	case M_FLUSH:
		return cd_w_flush(q, mp);
	}
}

static fastcall inline __unlikely int
cd_r_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);	/* already locked */
	cd_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(cd_ulong *) mp->b_rptr;

	cd_save_state(cd);
	switch (prim) {
	case CD_INFO_ACK:
		rtn = cd_r_info_ack(cd, q, mp);
		break;
	case CD_OK_ACK:
		rtn = cd_r_ok_ack(cd, q, mp);
		break;
	case CD_ERROR_ACK:
		rtn = cd_r_error_ack(cd, q, mp);
		break;
	case CD_ENABLE_CON:
		rtn = cd_r_enable_con(cd, q, mp);
		break;
	case CD_DISABLE_CON:
		rtn = cd_r_disable_con(cd, q, mp);
		break;
	case CD_ERROR_IND:
		rtn = cd_r_error_ind(cd, q, mp);
		break;
	case CD_UNITDATA_ACK:
		rtn = cd_r_unitdata_ack(cd, q, mp);
		break;
	case CD_UNITDATA_IND:
		rtn = cd_r_unitdata_ind(cd, q, mp);
		break;
	case CD_BAD_FRAME_IND:
		rtn = cd_r_bad_frame_ind(cd, q, mp);
		break;
	case CD_MODEM_SIG_IND:
		rtn = cd_r_modem_sig_ind(cd, q, mp);
		break;
	case CD_INFO_REQ:
	case CD_ATTACH_REQ:
	case CD_DETACH_REQ:
	case CD_ENABLE_REQ:
	case CD_DISABLE_REQ:
	case CD_ALLOW_INPUT_REQ:
	case CD_READ_REQ:
	case CD_UNITDATA_REQ:
	case CD_WRITE_READ_REQ:
	case CD_HALT_INPUT_REQ:
	case CD_ABORT_OUTPUT_REQ:
	case CD_MUX_NAME_REQ:
	case CD_MODEM_SIG_REQ:
	case CD_MODEM_SIG_POLL:
		rtn = cd_r_wrongway_req(cd, q, mp);
		break;
	default:
		rtn = cd_r_other_ind(cd, q, mp);
		break;
	}
      done:
	if (rtn)
		cd_restore_state(cd);
	return (0);
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
cd_r_error(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall noinline int
cd_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return cd_r_proto(q, mp);
	case M_FLUSH:
		return cd_r_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return cd_r_error(q, mp);
	}
}
static fastcall noinline int
cd_rsrv_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return (-EAGAIN);
	case M_FLUSH:
		return cd_r_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return cd_r_error(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * QUEUE WAKEUP FUNCTIONS
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline void
cd_w_wakeup(queue_t *q)
{
	/* Invoked once each time the write service procedure drains the write queue or encounters
	   an empty write queue. */
}

/**
 * cd_r_wakeup: - process read side wakeup
 * @q: active queue (read queue)
 *
 * Process a read side wakeup.  This function is called when the read side service procedure is
 * invoked and the queue is empty (or emptied).  This procedure needs to check whether there are
 * socket buffers queued from the device, and if so, create the corresponding CD_UNITDATA_IND
 * messages and deliver them upstream, subject to disposal considerations and the state of the
 * communications device.  Note that if messages cannot be passed upstream due to flow control,
 * this function returns and awaits a further enabling.  When the number of socket buffers queued
 * exceeds a maximum, they are discarded as they are received from the device.
 *
 * For the CD_UNITDATA_IND: If the message is to a broadcast address, CD_BROADCAST is set.  When
 * the communications device is a raw device (includes MAC addresses), CD_IMPLICIT is set and the
 * source and destination address lengths are zero (0).  Otherwise, the source and destination
 * addresses are set a the address type is set to CD_SPECIFIC.  The priority comes directly from
 * the socket buffer.  The data includes the MAC addresses when raw, and does not otherwise.
 * SAP addresses (LSAP/DSAP/SSAP) are always included in the data.  SNAP and other headers
 * between the MAC and the SAP addresses are not included in the data for non-raw devices.
 *
 * Raw devices (or devices with no MAC address) can be distinguished by a zero address length in
 * cd->proto.info.cd_addr_length.
 */
static fastcall noinline void
cd_r_wakeup(queue_t *q)
{
	cd_unitdata_ind_t *p;
	mblk_t *mp, *dp;
	struct sk_buff *skb;
	const size_t alen = cd->proto.info.cd_addr_length;
	const size_t mlen = sizeof(*p) + (alen << 1);
	static void cd_free_func(char *arg) {
		kfree_skb((struct sk_buff *) arg);
	}

	/* Invoked once each time the write service procedure drains the read queue or encounters
	   an empty read queue. */
	while ((skb = skb_dequeue(&cd->rx.q)) != NULL) {
		frtn_t free = { cd_free_func, (char *) skb };

		atomic_sub(&cd->rx.buf, skb->len);
		if (unlikely(!canputnext(q)))
			goto stop;
		if (unlikely(!!(mp = mi_allocb(q, mlen, BPRI_MED))))
			goto stop;
		/* make sure data points to the right place */
		skb_pull(skb, (alen ? skb->nh.raw : skb->hh.raw) - skb->data);
		if (unlikely(!!(dp = esballoc((uchar *) skb->data, skb->len, BPRI_MED, &free)))) {
			freeb(mp);
			mi_bufcall(q, 0, BPRI_MED);
			goto stop;
		}
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_IND;
		p->cd_src_addr_length = alen;
		p->cd_src_addr_offset = sizeof(*p);
		p->cd_priority = skb->priority;
		p->cd_addr_type = (alen == 0) ? CD_IMPLICIT : CD_SPECIFIC;
		p->cd_dest_addr_length = alen;
		p->cd_dest_addr_offset = sizeof(*p) + alen;
		mp->b_wptr += sizeof(*p);
		bcopy(skb->hh.raw, mp->b_wptr, (alen << 1));
		mp->b_wptr += (alen << 1);
		mp->b_cont = dp;
		putnext(q, mp);
		continue;
	      stop:
		atomic_add(&cd->rx.buf, skb->len);
		skb_queue_head(&cd->rx.q, skb);
		break;
	}
	return;
}

/*
 * --------------------------------------------------------------------------
 *
 * QUEUE PUT AND SERVICE PROCEDURES
 *
 * --------------------------------------------------------------------------
 */

static streamscall int
cd_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || cd_wput_msg(q, mp))
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
			if (dl_wsrv_msg(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		if (mp == NULL)
			cd_w_wakeup(q);
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
			if (dl_rsrv_msg(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		if (mp == NULL)
			cd_r_wakeup(q);
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
cd_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || cd_rput_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * QUEUE OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */
static streamscall int
cd_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct cd *cd;
	minor_t cminor;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN)
		return (ENXIO);
	cminor = getminor(*devp);
	if (cminor >= 10)
		return (ENXIO);
	if (sflag != CLONEOPEN)
		return (ENXIO);
	err = mi_open_comm(&cd_opens, sizeof(*cd), q, devp, oflags, sflag, crp);
	if (err)
		return (err);
	cd = CD_PRIV(q);
	bzero(cd, sizeof(*cd));

	cd->cminor = cminor;
	cd->oq = q;
	cd->state = CD_UNATTACHED;
	cd->oldstate = CD_UNATTACHED;

	skb_queue_head_init(&cd->rx.q);
	atomic_set(&cd->rx.buf, 0);
	cd->rx.max = STRHIGH;

	skb_queue_head_init(&cd->tx.q);
	atomic_set(&cd->tx.buf, 0);
	cd->tx.max = STRHIGH;

	cd->pt.type = 0;	/* for now */
	cd->pt.dev = NULL;	/* for now */
	cd->pt.func = cd_rcv_packet;	/* for now */
#ifdef HAVE_KMEMB_STRUCT_PACKET_TYPE_AF_PACKET_PRIV
	/* indicate "new style" packet handler */
	cd->pt.af_packet_priv = (void *) 1;
#else				/* HAVE_KMEMB_STRUCT_PACKET_TYPE_AF_PACKET_PRIV */
#ifdef HAVE_KMEMB_STRUCT_PACKET_TYPE_DATA
	/* indicate "new style" packet handler */
	cd->pt.data = (void *) 1;
#else				/* HAVE_KMEMB_STRUCT_PACKET_TYPE_DATA */
#error "struct packet_type must have af_packet_priv or data member."
#endif				/* HAVE_KMEMB_STRUCT_PACKET_TYPE_DATA */
#endif				/* HAVE_KMEMB_STRUCT_PACKET_TYPE_AF_PACKET_PRIV */
#ifdef HAVE_KMEMB_STRUCT_PACKET_TYPE_NEXT
	cd->pt.next = NULL;
#else				/* HAVE_KMEMB_STRUCT_PACKET_TYPE_NEXT */
#ifdef HAVE_KMEMB_STRUCT_PACKET_TYPE_LIST
	cd->pt.list = (struct list_head) LIST_HEAD_INIT(cd->pt.list);
#else				/* HAVE_KMEMB_STRUCT_PACKET_TYPE_LIST */
#error "struct packet_type must have next or list member."
#endif				/* HAVE_KMEMB_STRUCT_PACKET_TYPE_LIST */
#endif				/* HAVE_KMEMB_STRUCT_PACKET_TYPE_NEXT */
	cd->pt.next = NULL;

	cd->proto.info.cd_primitive = CD_INFO_ACK;
	cd->proto.info.cd_state = CD_UNATTACHED;
	cd->proto.info.cd_max_sdu = STRMAXPSZ;
	cd->proto.info.cd_min_sdu = 1;
	/* FIXME: fill these out proper */
	switch (cminor) {
	case 0:
		cd->proto.info.cd_class = CD_NODEV;
		cp->proto.info.cd_addr_length = 0;
		break;
	case 1:
		cd->proto.info.cd_class = CD_HDLC;
		cp->proto.info.cd_addr_length = 0;
		break;
	case 2:
		cd->proto.info.cd_class = CD_BISYNC;
		cp->proto.info.cd_addr_length = 0;
		break;
	case 3:		/* lan raw */
		cd->proto.info.cd_class = CD_LAN;
		cp->proto.info.cd_addr_length = 0;
		break;
	case 4:		/* ethernet */
		cd->proto.info.cd_class = CD_LAN;
		cp->proto.info.cd_addr_length = 6;
		break;
	case 5:		/* token ring */
		cd->proto.info.cd_class = CD_LAN;
		cp->proto.info.cd_addr_length = 8;
		break;
	case 6:		/* 802.3 */
		cd->proto.info.cd_class = CD_LAN;
		cp->proto.info.cd_addr_length = 6;
		break;
	case 7:		/* ethernet snap */
		cd->proto.info.cd_class = CD_LAN;
		cp->proto.info.cd_addr_length = 6;
		break;
	case 8:		/* token ring snap */
		cd->proto.info.cd_class = CD_LAN;
		cp->proto.info.cd_addr_length = 8;
		break;
	default:
		cd->proto.info.cd_class = CD_NODEV;
		break;
	}
	cd->proto.info.cd_duplex = CD_FULLDUPLEX;
	cd->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	cd->proto.info.cd_features = (CD_CANREAD | CD_CANDIAL | CD_AUTOALLOW);
	cd->proto.info.cd_ppa_style = CD_STYLE2;

	cd->ppa = 0;
	cd->disposal = CD_FLUSH;
	cd->msec = 0;

	qprocson(q);
	return (0);
}
static streamscall int
cd_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct cd *cd = CD_PRIV(q);
	int err;

	if (q->q_ptr == NULL)
		return (0);
	qprocsoff(q);
	switch (cd_get_state(cd)) {
	case CD_UNATTACHED:
	case CD_UNUSABLE:
	case CD_XRAY:
	case CD_NOT_AUTH:
		break;
	case CD_ENABLED:
	case CD_READ_ACTIVE:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
	case CD_DISABLE_PENDING:
		cd_disable_dev(cd);
		/* fall through */
	case CD_ENABLE_PENDING:
		cd_set_state(cd, CD_DISABLED);
		/* fall through */
	case CD_DISABLED:
		cd_detach_dev(cd);
		cd_set_state(cd, CD_UNATTACHED);
		break;
	}
	skb_queue_purge(&cd->rxq);
	skb_queue_purge(&cd->txq);
	err = mi_close_comm(&cd_opens, q);
	return (err);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

static struct qinit cd_rdinit = {
	.qi_srvp = cd_rsrv,
	.qi_putp = cd_rput,
	.qi_qopen = cd_qopen,
	.qi_qclose = cd_qclose,
	.qi_minfo = &cd_minfo,
	.qi_mstat = &cd_rstat,
};

static struct qinit cd_wrinit = {
	.qi_srvp = cd_wsrv,
	.qi_putp = cd_wput,
	.qi_minfo = &cd_minfo,
	.qi_mstat = &cd_wstat,
};

static struct streamtab cd_info = {
	.st_rdinit = &cd_rdinit,
	.st_wrinit = &cd_wrinit,
};
