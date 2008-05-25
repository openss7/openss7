/*****************************************************************************

 @(#) $RCSfile: cddev.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:56 $

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

 $Log: cddev.c,v $
 Revision 0.9.2.1  2008-05-25 12:46:56  brian
 - added manual pages, libraries, utilities and drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: cddev.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:56 $"

static char const ident[] = "$RCSfile: cddev.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:56 $";

/*
 * This is a multiplexing driver used for providing access to devices that
 * provide the Communications Device Interface (CDI) to the Linux network
 * device subsystem.  CDI streams that are linked under the driver are
 * configured to provide network device access from the Linux kernel.  When
 * streams are linked, they must be configured before being used using
 * input-output controls.  Once configured, they can be constrolled and
 * accessed in the same fashion as other Linux network devices.
 *
 * The primary purpose of this device driver is to allow CDI streams that
 * provide HDLC to be usable as Linux networking devices.
 */

#define _MPS_SOURCE		/* For MPS utilities, helpers and convenience functions. */

#include <sys/os7/compat.h>	/* For STREAMS compatibility. */
#include <sys/strsun.h>		/* For SUN convenience macros. */
#include <sys/cdi.h>		/* For CDI interface definitions. */

#define CDDEV_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CDDEV_EXTRA	"Part of the OpenSS7 X/Open Networking Serivces for Linux Fast-STREAMS"
#define CDDEV_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define CDDEV_REVISION	"OpenSS7 $RCSfile: cddev.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-25 12:46:56 $"
#define CDDEV_DEVICE	"SVR 4.2 STREAMS CDI Communication Device Interface Provider"
#define CDDEV_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CDDEV_LICENSE	"GPL"
#define CDDEV_BANNER	CDDEV_DESCRIP	"\n" \
			CDDEV_EXTRA	"\n" \
			CDDEV_COPYRIGHT	"\n" \
			CDDEV_DEVICE	"\n" \
			CDDEV_CONTACT
#define CDDEV_BANNER	CDDEV_DESCRIP	"\n" \
			CDDEV_REVISION

#ifdef LFS
#define CDDEV_DRV_ID	CONFIG_STREAMS_CDDEV_MODID
#define CDDEV_DRV_NAME	CONFIG_STREAMS_CDDEV_NAME
#define CDDEV_CMAJORS	CONFIG_STREAMS_CDDEV_NMAJOR
#define CDDEV_CMAJOR_0	CONFIG_STREAMS_CDDEV_MAJOR
#define CDDEV_UNITS	CONFIG_STREAMS_CDDEV_NMINORS
#endif

#ifdef LINUX
MODULE_AUTHOR(CDDEV_CONTACT);
MODULE_DESCRIPTION(CDDEV_DESCRIP);
MODULE_SUPPORTED_DEVICE(CDDEV_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CDDEV_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-cddev");
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CDDEV_DRV_ID));
MODULE_ALIAS("streamd-driver-cddev");
MODULE_ALIAS("streams-major-" __stringify(CDDEV_CMAJOR_0));
MODULE_ALIAS("/dev/streams/cddev");
MODULE_ALIAS("/dev/streams/cddev/*");
MODULE_ALIAS("/dev/streams/clone/cddev");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(CDDEV_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(CDDEV_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(CDDEV_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/cddev");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define DRV_ID		CDDEV_DRV_ID
#define DRV_NAME	CDDEV_DRV_NAME
#define CMAJORS		CDDEV_CMAJORS
#define CMAJOR_0	CDDEV_CMAJOR_0
#define UNITS		CDDEV_UNITS
#ifdef MODULE
#define DRV_BANNER	CDDEV_BANNER
#else				/* MODULE */
#define DRV_BANNER	CDDEV_SPLASH
#endif				/* MODULE */

static struct module_info cd_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat cd_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct cd {
	queue_t *oq;
	struct {
		cd_info_ack_t info;
	} proto;
};

#define CD_PRIV(q) ((struct cd *)q->q_ptr)

static inline const char *
cd_iocname(int cmd)
{
	switch (cmd) {
	case I_LINK:
		return ("I_LINK");
	case I_PLINK:
		return ("I_PLINK");
	case I_UNLINK:
		return ("I_UNLINK");
	case I_PUNLINK:
		return ("I_PUNLINK");
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
	case CD_ATTACH_PENDING:
		return ("CD_ATTACH_PENDING");
	case CD_DETACH_PENDING:
		return ("CD_DETACH_PENDING");
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
 * CDI USER TO CDI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */
static fastcall inline __hot_out int
cd_data_req(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (unlikely(!bcanputnext(cd->oq, dp->b_band)))
		goto ebusy;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "M_DATA ->");
	putnext(cd->oq, dp);
	return (0);
      ebusy:
	return (-EBUSY);
}
static fastcall noinline __unlikely int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_info_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_INFO_REQ ->");
	putnext(cd->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong ppa)
{
	cd_attach_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_ATTACH_REQ;
	p->cd_ppa = ppa;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ATTACH_REQ ->");
	cd_set_state(cd, CD_ATTACH_PENDING);
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_detach_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_DETACH_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_DETACH_REQ ->");
	cd_set_state(cd, CD_DETACH_PENDING);
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong type, caddr_t dial_ptr,
	      size_t dial_len)
{
	cd_enable_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + dial_len, BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_ENABLE_REQ;
	p->cd_dial_type = type;
	p->cd_dial_length = dial_len;
	p->cd_dial_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(dial_ptr, mp->b_wptr, dial_len);
	mp->b_wptr += dial_len;
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ENABLE_REQ ->");
	cd_set_state(cd, CD_ENABLE_PENDING);
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong disposal)
{
	cd_disable_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_DISABLE_REQ;
	p->cd_disposal = disposal;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_DISABLE_REQ ->");
	cd_set_state(cd, CD_DISABLE_PENDING);
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *msg, int pri)
{
	cd_allow_input_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = pri ? M_PCPROTO : M_PROTO;
	if (unlikely(!pcmsg(DB_TYPE(mp)) && !canputnext(cd->oq)))
		goto ebusy;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_ALLOW_INPUT_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ALLOW_INPUT_REQ ->");
	cd_set_state(cd, CD_INPUT_ALLOWED);
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong msec, int pri)
{
	cd_read_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = pri ? M_PCPROTO : M_PROTO;
	if (unlikely(!pcmsg(DB_TYPE(mp)) && !canputnext(cd->oq)))
		goto ebusy;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_READ_REQ;
	p->cd_msec = msec;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_READ_REQ ->");
	cd_set_state(cd, CD_INPUT_ALLOWED);
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall inline __hot_out int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong type, cd_ulong priority,
		caddr_t addr_ptr, size_t addr_len, mblk_t *dp)
{
	cd_unitdata_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + addr_len, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(cd->oq, priority)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_UNITDATA_REQ;
	p->cd_addr_type = type;
	p->cd_priority = priority;
	p->cd_dest_addr_length = addr_len;
	p->cd_dest_addr_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(addr_ptr, mp->b_wptr, addr_len);
	mp->b_wptr += addr_len;
	mp->b_cont = dp;
	if (msg && dp && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "CD_UNITDATA_REQ ->");
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall inline __hot_out int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong type, cd_ulong priority,
		  caddr_t add_ptr, size_t addr_len, mblk_t *dp, cd_ulong msec)
{
	cd_write_read_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + addr_len, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(cd->oq, priority)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_WRITE_READ_REQ;
	p->cd_unitdata_req.cd_primitive = CD_UNITDATA_REQ;
	p->cd_unitdata_req.cd_addr_type = type;
	p->cd_unitdata_req.cd_priority = priority;
	p->cd_unitdata_req.cd_dest_addr_length = addr_len;
	p->cd_unitdata_req.cd_dest_addr_offset = sizeof(*p);
	p->cd_read_req.cd_primitive = CD_READ_REQ;
	p->cd_read_req.cd_msec = msec;
	mp->b_wptr += sizeof(*p);
	bcopy(addr_ptr, mp->b_wptr, addr_len);
	mp->b_wptr += addr_len;
	mp->b_cont = dp;
	if (msg && dp && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "CD_WRITE_READ_REQ ->");
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong disposal, int pri)
{
	cd_halt_input_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = pri ? M_PCPROTO : M_PROTO;
	if (unlikely(!pcmsg(DB_TYPE(mp)) && !canputnext(cd->oq)))
		goto ebusy;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_HALT_INPUT_REQ;
	p->cd_disposal = disposal;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_HALT_INPUT_REQ ->");
	cd_set_state(cd, CD_ENABLED);
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *msg, int pri)
{
	cd_abort_output_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = pri ? M_PCPROTO : M_PROTO;
	if (unlikely(!pcmsg(DB_TYPE(mp)) && !canputnext(cd->oq)))
		goto ebusy;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_ABORT_OUTPUT_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ABORT_OUTPUT_REQ ->");
	if (cd_get_state(cd) == CD_OUTPUT_ACTIVE) {
		if (cd->proto.info.cd_duplex == CD_HALFDUPLEX) {
			if (cd->proto.info.cd_features & CD_AUTOALLOW) {
				cd_set_state(cd, CD_INPUT_ALLOWED);
			} else {
				cd_set_state(cd, CD_ENABLED);
			}
		} else {
			cd_set_state(cd, CD_ENABLED);
		}
	}
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_ulong *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = pri ? M_PCPROTO : M_PROTO;
	if (unlikely(!pcmsg(DB_TYPE(mp)) && !canputnext(cd->oq)))
		goto ebusy;
	p = (typeof(p)) mp->b_wptr;
	*p = CD_MUX_NAME_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MUX_NAME_REQ ->");
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong sigs)
{
	cd_modem_sig_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_MODEM_SIG_REQ;
	p->cd_sigs = sigs;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MODEM_SIG_REQ ->");
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_poll_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(cd->oq)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->cd_primitive = CD_MODEM_SIG_POLL;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MODEM_SIG_POLL ->");
	putnext(cd->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI PROVIDER TO CDI USER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */
static fastcall inline __hot_in int
cd_data_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
}
static fastcall noinline __unlikely int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (cd->proto.info.cd_primitive == 0) {
		/* First response. */
		cd->proto.info.cd_primitive = CD_INFO_ACK;
		cd->proto.info.cd_max_sdu = p->cd_max_sdu;
		cd->proto.info.cd_min_sdu = p->cd_min_sdu;
		cd->proto.info.cd_class = p->cd_class;
		switch (p->cd_class) {
		case CD_HDLC:
		case CD_LAN:
			break;
		case CD_BISYNC:
		case CD_NODEV:
		default:
			/* Unknown or unsupported class. */
			cd->proto.info.cd_state = CD_UNUSABLE;
			break;
		}
		cd->proto.info.cd_duplex = p->cd_duplex;
		switch (p->cd_duplex) {
		case CD_FULLDUPLEX:
		case CD_HALFDUPLEX:
			break;
		default:
			/* Unknown duplex. */
			cd->proto.info.cd_state = CD_UNUSABLE;
			break;
		}
		cd->proto.info.cd_output_style = p->cd_output_style;
		switch (p->cd_output_style) {
		case CD_UNACKEDOUTPUT:
		case CD_ACKEDOUTPUT:
		case CD_PACEDOUTPUT:
			break;
		default:
			/* Unknown output style. */
			cd->proto.info.cd_state = CD_UNUSABLE;
			break;
		}
		cd->proto.info.cd_features = p->cd_features;
		if (cd->cd_features & ~(CD_CANREAD | CD_CANDIAL | CD_AUTOALLOW)) {
			/* Unknown features */
			cd->proto.info.cd_state = CD_UNUSABLE;
		}
		cd->proto.info.cd_addr_length = p->cd_addr_length;
		cd->proto.info.cd_ppa_style = p->cd_ppa_style;
		switch (p->cd_ppa_style) {
		case CD_STYLE1:
		case CD_STYLE2:
			break;
		default:
			/* Unknown PPA style. */
			cd->proto.info.cd_state = CD_UNUSABLE;
			break;
		}
		if (p->cd_state != CD_DISABLED) {
			/* These devices must be in the disabled state when linked under the
			   multiplexing driver. */
			cd->proto.info.cd_state = CD_UNUSABLE;
		}
		cd_set_state(cd, cd->proto.info.cd_state);
	} else if (cd_get_state(cd) != CD_UNUSABLE) {
		cd->proto.info.cd_state = p->cd_state;
		cd_set_state(cd, cd->proto.info.cd_state);
	}
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (cd_get_state(cd)) {
	case CD_ATTACH_PENDING:
		cd_set_state(cd, p->cd_state);
		break;
	case CD_ENABLE_PENDING:
		cd_set_state(cd, p->cd_state);
		break;
	case CD_DISABLE_PENDING:
		cd_set_state(cd, p->cd_state);
		break;
	case CD_DETACH_PENDING:
		cd_set_state(cd, p->cd_state);
		break;
	default:
		cd_set_state(cd, p->cd_state);
		break;
	}
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (cd_get_state(cd)) {
	case CD_ATTACH_PENDING:
		cd_set_state(cd, p->cd_state);
		break;
	case CD_ENABLE_PENDING:
		cd_set_state(cd, p->cd_state);
		break;
	case CD_DISABLE_PENDING:
		cd_set_state(cd, p->cd_state);
		break;
	case CD_DETACH_PENDING:
		cd_set_state(cd, p->cd_state);
		break;
	default:
		cd_set_state(cd, p->cd_state);
		break;
	}
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_enable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	cd_set_state(cd, p->cd_state);
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	cd_set_state(cd, p->cd_state);
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	cd_set_state(cd, p->cd_state);
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall inline __hot_in int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	cd_set_state(cd, p->cd_state);
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall inline __hot_in int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	cd_set_state(cd, p->cd_state);
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_bad_frame_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	cd_set_state(cd, p->cd_state);
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	cd_set_state(cd, p->cd_state);
	freemsg(mp);
	return (0);
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_wrongway_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_other_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * INPUT-OUTPUT CONTROLS
 *
 * --------------------------------------------------------------------------
 */
static caddr_t cd_links = NULL;
static rwlock_t cd_mux_lock = RW_LOCK_UNLOCKED;

static fastcall noinline int
cd_i_plink(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	cd_info_ack_t *p;
	mblk_t *rp;
	struct cd *item;
	caddr_t state;
	pl_t pl;
	int err;

	/* l_qtop points to the upper write queue */
	/* l_qbot points to the lower write queue */
	/* l_index has the multiplexor index */
	if (!(rp = mi_allocb(q, sizeof(*rp), BPRI_MED)))
		return (-ENOBUFS);
	if (!(state = mi_open_alloc(sizeof(*item)))) {
		freeb(rp);
		mi_copy_done(q, mp, ENOMEM);
		return (0);
	}
	item = (typeof(item)) state;
	bzero(item, sizeof(*item));
	item->oq = l->l_qbot;
	mi_attach(RD(l->l_qbot), state);
	pl = RW_WRLOCK(&cd_mux_lock, plstr);
	err = mi_open_link(&cd_links, state, NULL, 0, MODOPEN, ioc->ioc_cr);
	RW_UNLOCK(&cd_mux_lock, pl);
	if (err) {
		mi_detach(RD(l->l_qbot), state);
		mi_close_free(state);
		freeb(rp);
		mi_copy_done(q, mp, err);
		return (0);
	}
	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_wptr;
	p->cd_primitive = CD_INFO_ACK;
	rp->b_wptr += sizeof(*p);
	putnext(item->oq, rp);
	mi_copy_done(q, mp, 0);
	return (0);

}
static fastcall noinline int
cd_i_punlink(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	caddr_t state;
	pl_t pl;

	/* l_qtop points to the upper write queue */
	/* l_qbot points to the lower write queue */
	/* l_index has the multiplexor index */
	pl = RW_WRLOCK(&cd_mux_lock, plstr);
	for (state = cd_links; state; state = mi_next_ptr(state)) {
		struct cd *item = (typeof(item)) state;

		if (item->oq == l->l_qbot) {
			mi_detach(RD(l->l_qbot), state);
			mi_close_unlink(&cd_links, state);
			break;
		}
	}
	RW_UNLOCK(&cd_mux_lock, pl);
	if (state == NULL) {
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	mi_close_free(state);
	mi_copy_done(q, mp, 0);
	return (0);
}
static fastcall noinline int
cd_ioctl(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
}
static fastcall noinline int
cd_copyin(struct cd *cd, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
}
static fastcall noinline int
cd_copyout(struct cd *cd, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline
cd_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct cd *cd = CD_PRIV(q);
	int err;

	if (!mp->b_cont)
		goto fault;
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case STR:
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(I_LINK):
			break;
		case _IOC_NR(I_PLINK):
			return cd_i_plink(cd, q, mp);
		case _IOC_NR(I_UNLINK):
			break;
		case _IOC_NR(I_PUNLINK):
			return cd_i_punlink(cd, q, mp);
		default:
			break;
		}
		break;
	case CD_IOC_MAGIC:
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case CD_SETTYPE:
			return cd_ioctl(cd, q, mp);
		default:
			break;
		}
		break;
	}
      invalid:
	err = EINVAL;
	goto error;
      fault:
	err = EFAULT;
	goto error;
      error:
	mi_copy_done(q, mp, err);
	return (0);
}
static fastcall noinline
cd_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct cd *cd = CD_PRIV(q);
	mblk_t *dp;
	int err;

	if (!mp->b_cont)
		goto fault;
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case CD_IOC_MAGIC:
		switch (_IOC_NR(cp->cp_cmd)) {
		case CD_SETTYPE:
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				return (0);
			case MI_COPY_CASE(MI_COPY_IN, 1):
				return cd_copyin(cd, q, mp, dp);
			case MI_COPY_CASE(MI_COPY_OUT, 1):
				return cd_copyout(cd, q, mp, dp);
			default:
				goto eproto;
			}
		}
		goto invalid;
	default:
		goto notforus;
	}
      eproto:
	err = EPROTO;
	goto error;
      fault:
	err = EFAULT;
	goto error;
      invalid:
	err = EINVAL;
	goto error;
      notforus:
	err = EINVAL;
	goto error;
      error:
	mi_copy_done(q, mp, err);
	return (0);
}
static fastcall noinline
cd_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] & ~FLUSHW;
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
static fastcall noinline
cd_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "unsupported STREAMS message %d on queue",
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

static fastcall noinline int
cd_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_IOCTL:
		return (-EAGAIN);
	case M_IOCDATA:
		return (-EAGAIN);
	case M_FLUSH:
		return cd_w_flush(q, mp);
	default:
		return cd_w_other(q, mp);
	}
}
static fastcall noinline int
cd_wsrv_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_IOCTL:
		return cd_w_ioctl(q, mp);
	case M_IOCDATA:
		return cd_w_iocdata(q, mp);
	case M_FLUSH:
		return cd_w_flush(q, mp);
	default:
		return cd_w_other(q, mp);
	}
}

static fastcall noinline int
cd_r_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	mi_strlog(q, STRLOGDA, SL_TRACE, "<- M_DATA");
	return cd_data_ind(cd, q, mp);
}
static fastcall noinline int
cd_r_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(q, 0, sizeof(prim)))
		goto done;
	prim = *(cd_ulong *) mp->b_rptr;
	cd_save_state(cd);

	switch (prim) {
	case CD_UNITDATA_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- CD_UNITDATA_IND");
		rtn = cd_unitdata_ind(cd, q, mp);
		break;
	case CD_UNITDATA_ACK:
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- CD_UNITDATA_ACK");
		rtn = cd_unitdata_ack(cd, q, mp);
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", cd_primname(prim));
		switch (prim) {
		case CD_INFO_ACK:
			rtn = cd_info_ack(cd, q, mp);
			break;
		case CD_OK_ACK:
			rtn = cd_ok_ack(cd, q, mp);
			break;
		case CD_ERROR_ACK:
			rtn = cd_error_ack(cd, q, mp);
			break;
		case CD_ENABLE_CON:
			rtn = cd_enable_con(cd, q, mp);
			break;
		case CD_DISABLE_CON:
			rtn = cd_disable_con(cd, q, mp);
			break;
		case CD_ERROR_IND:
			rtn = cd_error_ind(cd, q, mp);
			break;
		case CD_UNITDATA_ACK:
			rtn = cd_unitdata_ack(cd, q, mp);
			break;
		case CD_UNITDATA_IND:
			rtn = cd_unitdata_ind(cd, q, mp);
			break;
		case CD_BAD_FRAME_IND:
			rtn = cd_bad_frame_ind(cd, q, mp);
			break;
		case CD_MODEM_SIG_IND:
			rtn = cd_modem_sig_ind(cd, q, mp);
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
			rtn = cd_wrongway_req(cd, q, mp);
			break;
		default:
			rtn = cd_other_ind(cd, q, mp);
			break;
		}
	}
	if (rtn)
		cd_restore_state(cd);
	switch (rtn) {
	case 0:
	case -ENOBUFS:
	case -EAGAIN:
	case -ENOMEM:
	case -EBUSY:
	case -EDEADLK:
	case -ENOSR:
		return (rtn);
	}
	freemsg(mp);
	return (0);
}
static fastcall noinline int
cd_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		/* Act like stream head here to avoid infinite looping. */
		if (mp->b_band & MSGNOLOOP) {
			freemsg(mp);
			return (0);
		}
		mp->b_flag |= MSGNOLOOP;
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static fastcall noinline int
cd_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return (-EAGAIN);
	case M_PROTO:
	case M_PCPROTO:
		return (-EAGAIN);
	case M_FLUSH:
		return cd_r_flush(q, mp);
	}
}
static fastcall noinline int
cd_rsrv_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_r_proto(q, mp);
	case M_FLUSH:
		return cd_r_flush(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * QUEUE WAKEUP FUNCTIONS
 *
 * --------------------------------------------------------------------------
 */

static fastcall void
cd_w_wakeup(queue_t *q)
{
}
static fastcall void
cd_r_wakeup(queue_t *q)
{
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
	pl_t pl;

	pl = RW_WRLOCK(&cd_mux_lock, plstr); {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (cd_wsrv_msg(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		if (mp == NULL)
			cd_w_wakeup(q);
		RW_UNLOCK(&cd_mux_lock, pl);
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
			if (cd_rsrv_msg(q, mp)) {
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
static caddr_t cd_opens = NULL;

static streamscall int
cd_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct cd *cd;
	int err;

	if (q->q_ptr != NULL)
		return (0);
	if (getminor(*devp) != 0)
		return (ENXIO);
	if (sflag == MODOPEN)
		return (EIO);
	sflag = DRVOPEN;
	if (crp->cr_uid != 0 && crp->cr_euid != 0)
		return (EACCES);
	err = mi_open_comm(&cd_opens, sizeof(*cd), q, devp, oflags, sflag, crp);
	if (err) {
		return (err);
	}
	cd = CD_PRIV(q);
	bzero(cd, sizeof(*cd));
	cd->oq = q;

	qprocson(q);
	return (err);
}
static streamscall int
cd_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct cd *cd = CD_PRIV(q);
	int err;

	if (q->q_next == NULL)
		return (0);
	qprocsoff(q);
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

static struct cd_wrinit = {
	.qi_putp = cd_wput,
	.qi_srvp = cd_wsrv,
	.qi_minfo = &cd_minfo,
	.qi_mstat = &cd_mstat,
};
static struct cd_rdinit = {
	.qi_putp = NULL,
	.qi_qopen = cd_qopen,
	.qi_qclose = cd_qclose,
	.qi_minfo = &cd_minfo,
	.qi_mstat = &cd_mstat,
};
static struct cd_muxwinit = {
	.qi_putp = NULL,
	.qi_minfo = &cd_minfo,
	.qi_mstat = &cd_mstat,
};
static struct cd_muxrinit = {
	.qi_putp = cd_rput,
	.qi_srvp = cd_rsrv,
	.qi_minfo = &cd_minfo,
	.qi_mstat = &cd_mstat,
};

struct streamtab cddev_info = {
	.st_rdinit = &cd_rdinit,
	.st_wrinit = &cd_wrinit,
	.st_muxrinit = &cd_muxrinit,
	.st_muxwinit = &cd_muxwinit,
};

static modID_t modid = DRV_ID;
static major_t major = CMAJOR;

/*
 * --------------------------------------------------------------------------
 *
 * LINUX KERNEL MODULE INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */
#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
module_param(major, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for CDDEV. (0 for allocation.)");
MODULE_PARM_DESC(major, "Major device number for CDDEV. (0 for allocation.)");

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS REGISTRATION
 *
 * --------------------------------------------------------------------------
 */

#ifdef LFS

static struct cdevsw cddev_cdev = {
	.d_str = &cddev_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static int
cddev_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&cddev_cdev, major)) < 0)
		return (err);
	return (0);
}

static int
cddev_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&cddev_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

#ifdef LIS

static int
cddev_regsiter_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &cddev_info, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

static int
cddev_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

static __init int
cddev_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = cddev_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver major %d", DRV_NAME, (int) major);
		return (err);
	}
	return (0);
}

static __exit void
cddev_modexit(void)
{
	int err;

	if ((err = cddev_unregister_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", DRV_NAME, err);
		return;
	}
	return;
}

module_init(cddev_modinit);
module_exit(cddev_modexit);

#endif				/* LINUX */
