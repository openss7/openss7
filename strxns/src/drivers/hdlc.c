/*****************************************************************************

 @(#) $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:35:41 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/15 05:35:41 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: hdlc.c,v $
 Revision 0.9.2.6  2007/08/15 05:35:41  brian
 - GPLv3 updates

 Revision 0.9.2.5  2007/08/14 03:31:07  brian
 - GPLv3 header update

 Revision 0.9.2.4  2007/07/14 01:37:19  brian
 - make license explicit, add documentation

 Revision 0.9.2.3  2007/03/25 19:02:46  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.2  2006/12/21 11:33:31  brian
 - updates for release, current development

 Revision 0.9.2.1  2006/12/20 23:10:43  brian
 - added new mux drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:35:41 $"

static char const ident[] =
    "$RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:35:41 $";

/*
 *  This is a multiplexing driver used to support linking CD HDLC streams.  When CD streams are
 *  linked under the driver and configured using input-output controls, the HDLC stream appears as a
 *  HDLC device to Linux.  CD streams can also be opened on the upper multiplex and attached to
 *  Linux raw HDLC devices.  This permits CD drivers to be exported to Linux and CD users to also
 *  use Linux devices not directly supported with a STREAMS driver.
 *
 *  To manage mapping of upper multiplex CD streams to Linux devices, a specialized input-output
 *  control and CD primitive is used.  These take as an argument the ascii name of the device and
 *  return a PPA (minor device number) that can be used either to open the device directly or can be
 *  used to attach a style 2 CD stream to the device.  To manage mapping of lower multiplex CD
 *  streams to Linux devices, a specialised intput-output control is used.  This input-output
 *  control takes as an argument the multiplex id and the ascii device name.  When successful, the
 *  lower multiplex streams is mapped to the Linux device of the same name.  Devices mapped in this
 *  way begin life as a raw-HDLC device but can be configured using the sethdlc utility for Linux.
 */

#ifndef HAVE_KTYPE_BOOL
#include <stdbool.h>
#endif

#define _DEBUG 1
// #undef _DEBUG

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>

#include <sys/cdi.h>

#include <linux/hdlc.h>

#define HDLC_DESCRIP	"HDLC STREAMS MULTIPLEXING DRIVER."
#define HDLC_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define HDLC_REVISION	"OpenSS7 $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:35:41 $"
#define HDLC_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define HDLC_DEVICE	"Supports Linux HDLC devices."
#define HDLC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define HDLC_LICENSE	"GPL"
#define HDLC_BANNER	HDLC_DESCRIP	"\n" \
			HDLC_EXTRA	"\n" \
			HDLC_REVISION	"\n" \
			HDLC_COPYRIGHT	"\n" \
			HDLC_DEVICE	"\n" \
			HDLC_CONTACT	"\n"
#define HDLC_SPLASH	HDLC_DESCRIP	" - " \
			HDLC_REVISION

#ifdef LINUX
MODULE_AUTHOR(HDLC_CONTACT);
MODULE_DESCRIPTION(HDLC_DESCRIP);
MODULE_SUPPORTED_DEVICE(HDLC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(HDLC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-hldc");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE "-"
	       PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define HDLC_DRV_ID		CONFIG_STREAMS_HDLC_MODID
#define HDLC_DRV_NAME		CONFIG_STREAMS_HDLC_NAME
#define HDLC_CMAJORS		CONFIG_STREAMS_HDLC_NMAJORS
#define HDLC_CMAJOR_0		CONFIG_STREAMS_HDLC_MAJOR
#define HDLC_UNITS		CONFIG_STREAMS_HDLC_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_HDLC_MODID));
MODULE_ALIAS("streams-driver-hdlc");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_HDLC_MAJOR));
MODULE_ALIAS("/dev/streams/hdlc");
MODULE_ALIAS("/dev/streams/hdlc/*");
MODULE_ALIAS("/dev/streams/clone/hdlc");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(HDLC_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(HDLC_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(HDLC_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/hdlc");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#define CDLOGST	    1		/* log CD state transitions */
#define CDLOGTO	    2		/* log CD timeouts */
#define CDLOGRX	    3		/* log CD received primitives */
#define CDLOGTX	    4		/* log CD issued primitives */
#define CDLOGTE	    5		/* log CD timer events */
#define CDLOGDA	    6		/* log CD data */

#if 1
#define STRLOG(mid, sid, level, flags, ...) strlog(mid, sid, level, flags, __VA_ARGS__)
#else
#define STRLOG(mid, sid, level, flags, ...) do { } while (0);
#endif

struct cd {
	spinlock_t lock;		/* structure lock */
	unsigned long users;		/* users of this structure */
	unsigned long owner;		/* owner of this structure */
	queue_t *waitq;			/* queue waiting for service */
	mblk_t *ioc;			/* waiting I_LINK ioctl */
	int mid;			/* module id */
	int sid;			/* stream id */
	union {
		dev_t dev;		/* device number (upper) */
		int index;		/* multipelx index (lower) */
	};
	queue_t *oq;			/* output queue (upper rd, lower wr) */
	queue_t *iq;			/* input queue (upper wr, lower rd) */
	struct cd *other;		/* direct link across multiplexing driver */
	struct hdlc_device_struct *hdlc;	/* hdlc device for link to Linux */
	cd_ulong ppa;			/* ppa */
	struct CD_info_ack info;	/* information */
};

static spinlock_t cd_lock = SPIN_LOCK_UNLOCKED;	/* lock for lists */

static caddr_t cd_opens = NULL;		/* list for opens */
static caddr_t cd_links = NULL;		/* list for links */

static const char *
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
	case CD_NOT_AUTH:
		return ("CD_NOT_AUTH");
	default:
		return ("????");
	}
}
static inline cd_ulong
cd_get_state(struct cd *cd)
{
	return cd->info.cd_state;
}
static inline int
cd_set_state(struct cd *cd, cd_ulong state)
{
	if (cd->info.cd_state != state) {
		STRLOG(cd->mid, cd->sid, CDLOGST, SL_TRACE, "%s -> %s",
		       cd_state_name(cd->info.cd_state), cd_state_name(state));
		cd->info.cd_state = state;
	}
	return (state);
}

static inline mblk_t *
cd_allocb(queue_t *q, size_t size, int priority)
{
	mblk_t *mp;

	if (unlikely(!(mp = allocb(size, priority))))
		mi_bufcall(q, size, priority);
	return (mp);
}

/**
 * cd_trylock: - try to lock a CD queue pair
 * @cd: CD private structure
 * @q: active queue (not necessarily in CD queue pair)
 *
 * Note that is we always run (at least initially) from a service procedure, there is no need to
 * suppress interrupts while holding the lock.  This simply lock will do because the service
 * procedure is guaranteed single threaded.  Also, because interrupts do not need to be suppressed
 * while holding the lock, the current task pointer identifies the thread and the trhead can be
 * allowed to recurse on the lock.  When a queue procedure fails to acquire the lock, it is marked
 * to have its service procedure scheduled later, but we only remember one queue, so if there are
 * two waiting, the second one is reenabled.
 */
static inline fastcall bool
cd_trylock(struct cd *cd, queue_t *q)
{
	bool rtn = false;
	pl_t pl;

	pl = LOCK(&cd->lock);
	if (cd->users == 0 && (q->q_flag & QSVCBUSY)) {
		rtn = true;
		cd->users = 1;
		cd->owner = (unsigned long) current;
	} else if (cd->users != 0 && cd->owner == (unsigned long) current) {
		rtn = true;
		cd->users++;
	} else if (!cd->waitq) {
		cd->waitq = q;
	} else if (cd->waitq != q) {
		qenable(q);
	}
	UNLOCK(&cd->lock, pl);
	return (rtn);
}

static inline fastcall void
cd_unlock(struct cd *cd)
{
	pl_t pl;

	pl = LOCK(&cd->lock);
	if (--cd->users == 0 && cd->waitq) {
		qenable(cd->waitq);
		cd->waitq = NULL;
	}
	UNLOCK(&cd->lock, pl);
}

/*
 *  CD Primitives issued upstream.
 *  ==============================
 */

/**
 * cd_info_ack: - issue CD_INFO_ACK message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_info_ack *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_INFO_ACK");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_ok_ack: - issue CD_OK_ACK message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_ok_ack *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_OK_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_OK_ACK");
		putnext(cd->oq, mp);
		return (0);
	}
	mi_bufcall(q, sizeof(*p), BPRI_MED);
	return (-ENOBUFS);
}

/**
 * cd_error_ack: - issue CD_ERROR_ACK message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_error_ack *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ERROR_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_ERROR_ACK");
		putnext(cd->oq, mp);
		return (0);
	}
	mi_bufcall(q, sizeof(*p), BPRI_MED);
	return (-ENOBUFS);
}

/**
 * cd_enable_con: - issue CD_ENABLE_CON message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_enable_con *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ENABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_ENABLE_CON");
		putnext(cd->oq, mp);
		return (0);
	}
	mi_bufcall(q, sizeof(*p), BPRI_MED);
	return (-ENOBUFS);
}

/**
 * cd_disable_con: - issue CD_DISABLE_CON message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_disable_con *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DISABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_DISABLE_CON");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_error_ind: - issue CD_ERROR_IND message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_error_ind *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ERROR_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_ERROR_IND");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_unitdata_ack: - issue CD_UNITDATA_ACK message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_unitdata_ack *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_UNITDATA_ACK");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_unitdata_ind: - issue CD_UNITDATA_IND message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_unitdata_ind *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_UNITDATA_IND");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_bad_frame_ind: - issue CD_BAD_FRAME_IND message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_bad_frame_ind *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_BAD_FRAME_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_BAD_FRAME_IND");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_modem_sig_ind: - issue CD_MODEM_SIG_IND message
 * @cd: CD private structure (upper)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_modem_sig_ind *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "<- CD_MODEM_SIG_IND");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  CD Primitives issued downstream.
 *  ================================
 */

/**
 * cd_info_req: - issue CD_INFO_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_info_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_INFO_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_attach_req: - issue CD_ATTACH_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_attach_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ATTACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_ATTACH_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_detach_req: - issue CD_DETACH_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_detach_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_DETACH_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_enable_req: - issue CD_ENABLE_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_enable_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_ENABLE_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_disable_req: - issue CD_DISABLE_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_disable_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_DISABLE_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_allow_input_req: - issue CD_ALLOW_INPUT_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_allow_input_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ALLOW_INPUT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_ALLOW_INPUT_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_read_req: - issue CD_READ_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_read_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_READ_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_READ_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_unitdata_req: - issue CD_UNITDATA_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_unitdata_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_UNITDATA_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_write_read_req: - issue CD_WRITE_READ_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_write_read_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_WRITE_READ_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_WRITE_READ_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_halt_input_req: - issue CD_HALT_INPUT_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_halt_input_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_HALT_INPUT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_HALT_INPUT_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_abort_output_req: - issue CD_ABORT_OUTPUT_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_abort_output_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ABORT_OUTPUT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_ABORT_OUTPUT_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_mux_name_req: - issue CD_MUX_NAME_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_mux_name_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MUX_NAME_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_MUX_NAME_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_modem_sig_req: - issue CD_MODEM_SIG_REQ message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_modem_sig_req *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_MODEM_SIG_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_modem_sig_poll: - issue CD_MODEM_SIG_POLL message
 * @cd: CD private structure (lower)
 * @q: active queue
 * @msg: message to free on success
 */
static noinline fastcall int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *msg)
{
	struct CD_modem_sig_poll *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_POLL;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "CD_MODEM_SIG_POLL ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  CD Primitives received from upstream.
 *  =====================================
 */

/**
 * ce_info_req: - process CD_INFO_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_INFO_REQ message
 */
static noinline fastcall int
ce_info_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_info_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return cd_info_ack(cd, q, mp);
      badprim:
	return cd_error_ack(cd, q, mp, CD_INFO_REQ, CD_TOOSHORT);
}

/**
 * ce_attach_req: - process CD_ATTACH_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_ATTACH_REQ message
 */
static noinline fastcall int
ce_attach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_attach_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_UNATTACHED)
		goto outstate;
	cd_set_state(cd, CD_ATTACH_PENDING);
	return cd_attach_ack(cd, q, mp, p->cd_ppa);
      oustate:
	return cd_error_ack(cd, q, mp, CD_ATTACH_REQ, CD_OUTSTATE);
      badprim:
	return cd_error_ack(cd, q, mp, CD_ATTACH_REQ, CD_TOOSHORT);
}

/**
 * ce_detach_req: - process CD_DETACH_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_DETACH_REQ message
 */
static noinline fastcall int
ce_detach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_detach_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	cd_set_state(cd, CD_DETACH_PENDING);
	return cd_detach_ack(cd, q, mp);
      oustate:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, CD_OUTSTATE);
      badprim:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, CD_TOOSHORT);
}

/**
 * ce_enable_req: - process CD_ENABLE_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_ENABLE_REQ message
 */
static noinline fastcall int
ce_enable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_enable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_DISABLED)
		goto outstate;
	cd_set_state(cd, CD_ENABLE_PENDING);
	return cd_enable_ack_con(cd, q, mp);
      oustate:
	return cd_error_ack(cd, q, mp, CD_ENABLE_REQ, CD_OUTSTATE);
      badprim:
	return cd_error_ack(cd, q, mp, CD_ENABLE_REQ, CD_TOOSHORT);
}

/**
 * ce_disable_req: - process CD_DISABLE_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_DISABLE_REQ message
 */
static noinline fastcall int
ce_disable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_disable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cd_get_state(cd) != CD_ENABLED)
		goto outstate;
	cd_set_state(cd, CD_DISABLE_PENDING);
	return cd_disable_ack_con(cd, q, mp);
      oustate:
	return cd_error_ack(cd, q, mp, CD_DISABLE_REQ, CD_OUTSTATE);
      badprim:
	return cd_error_ack(cd, q, mp, CD_DISABLE_REQ, CD_TOOSHORT);
}

/**
 * ce_allow_input_req: - process CD_ALLOW_INPUT_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_ALLOW_INPUT_REQ message
 */
static noinline fastcall int
ce_allow_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_allow_input_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_read_req: - process CD_READ_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_READ_REQ message
 */
static noinline fastcall int
ce_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_read_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_unitdata_req: - process CD_UNITDATA_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_UNITDATA_REQ message
 */
static noinline fastcall int
ce_unitdata_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_unitdata_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_write_read_req: - process CD_WRITE_READ_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_WRITE_READ_REQ message
 */
static noinline fastcall int
ce_write_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_write_read_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_halt_input_req: - process CD_HALT_INPUT_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_HALT_INPUT_REQ message
 */
static noinline fastcall int
ce_halt_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_halt_input_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_abort_output_req: - process CD_ABORT_OUTPUT_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_ABORT_OUTPUT_REQ message
 */
static noinline fastcall int
ce_abort_output_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_abort_output_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_mux_name_req: - process CD_MUX_NAME_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_MUX_NAME_REQ message
 */
static noinline fastcall int
ce_mux_name_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_mux_name_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_modem_sig_req: - process CD_MODEM_SIG_REQ message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_MODEM_SIG_REQ message
 */
static noinline fastcall int
ce_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_modem_sig_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_modem_sig_poll: - process CD_MODEM_SIG_POLL message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: CD_MODEM_SIG_POLL message
 */
static noinline fastcall int
ce_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_modem_sig_poll *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_other_req: - process unknown message
 * @cd: CD private structure (upper)
 * @q: active queue (write queue)
 * @mp: unknown message
 */
static noinline fastcall int
ce_other_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/*
 *  CD Primitives received from downstream.
 *  =======================================
 */

/**
 * ce_info_ack: - process CD_INFO_ACK message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_INFO_ACK message
 */
static noinline fastcall int
ce_info_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_info_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_ok_ack: - process CD_OK_ACK message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_OK_ACK message
 */
static noinline fastcall int
ce_ok_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_ok_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_error_ack: - process CD_ERROR_ACK message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_ERROR_ACK message
 */
static noinline fastcall int
ce_error_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_error_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_enable_con: - process CD_ENABLE_CON message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_ENABLE_CON message
 */
static noinline fastcall int
ce_enable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_enable_con *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_disable_con: - process CD_DISABLE_CON message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_DISABLE_CON message
 */
static noinline fastcall int
ce_disable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_disable_con *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_error_ind: - process CD_ERROR_IND message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_ERROR_IND message
 */
static noinline fastcall int
ce_error_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_error_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_unitdata_ack: - process CD_UNITDATA_ACK message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_UNITDATA_ACK message
 */
static noinline fastcall int
ce_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_unitdata_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_unitdata_ind: - process CD_UNITDATA_IND message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_UNITDATA_IND message
 */
static noinline fastcall int
ce_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_unitdata_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_bad_frame_ind: - process CD_BAD_FRAME_IND message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_BAD_FRAME_IND message
 */
static noinline fastcall int
ce_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_bad_frame_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_modem_sig_ind: - process CD_MODEM_SIG_IND message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: CD_MODEM_SIG_IND message
 */
static noinline fastcall int
ce_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	struct CD_modem_sig_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

/**
 * ce_other_ind: - process unknown message
 * @cd: CD private structure (lower)
 * @q: active queue (read queue)
 * @mp: unknown message
 */
static noinline fastcall int
ce_other_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
}

static inline fastcall __hot_write int
cd_w_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	if (cd_trylock(cd, q)) {
		if (cd->other) {
			if (cd->other->oq) {
				if (pcmsg(DB_TYPE(mp)) || bcanputnext(cd->other->oq, mp->b_band)) {
					putnext(cd->other->oq, mp);
					cd_unlock(cd);
					return (0);
				}
				cd_unlock(cd);
				return (-EBUSY);
			}
		} else if (cd->hdlc) {
			if (hdlc_is_active(cd)) {
				/* transform M_DATA message into HDLC device transmission. */
			}
		}
		cd_unlock(cd);
		freemsg(mp);
		return (0);
	}
	return (-EDEADLK);
}
static noinline fastcall __unlikely int
cd_w_data_slow(queue_t *q, mblk_t *mp)
{
	return cd_w_data(q, mp);
}
static noinline fastcall __unlikely int
cd_w_proto(queue_t *q, mblk_t *mp)
{
	cd_ulong oldstate = cd_get_state(cd);
	int rtn;

	if (!cd_trylock(cd, q))
		return (-EDEADLK);

	/* If this upper multiplex stream is direclty connected to a lower multiplex stream, simply 
	   pass the message along with flow control. */
	if (cd->other && cd->other->oq) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(cd->other->oq, mp->b_band)) {
			putnext(cd->other->oq, mp);
			cd_unlock(cd);
			return (0);
		}
		cd_unlock(cd);
		return (-EBUSY);
	}

	switch (*(cd_ulong *) mp->b_rptr) {
	case CD_INFO_REQ:	/* 0x00 - Information request */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_INFO_REQ");
		rtn = ce_info_req(cd, q, mp);
		break;
	case CD_ATTACH_REQ:	/* 0x02 - Attach a PPA */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_ATTACH_REQ");
		rtn = ce_attach_req(cd, q, mp);
		break;
	case CD_DETACH_REQ:	/* 0x03 - Detach a PPA */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_DETACH_REQ");
		rtn = ce_detach_req(cd, q, mp);
		break;
	case CD_ENABLE_REQ:	/* 0x04 - Prepare a device */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_ENABLE_REQ");
		rtn = ce_enable_req(cd, q, mp);
		break;
	case CD_DISABLE_REQ:	/* 0x05 - Disable a device */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_DISABLE_REQ");
		rtn = ce_disable_req(cd, q, mp);
		break;
	case CD_ALLOW_INPUT_REQ:	/* 0x0b - Allow input */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_ALLOW_INPUT_REQ");
		rtn = ce_allow_input_req(cd, q, mp);
		break;
	case CD_READ_REQ:	/* 0x0c - Wait-for-input request */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_READ_REQ");
		rtn = ce_read_req(cd, q, mp);
		break;
	case CD_UNITDATA_REQ:	/* 0x0d - Data send request */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_UNITDATA_REQ");
		rtn = ce_unitdata_req(cd, q, mp);
		break;
	case CD_WRITE_READ_REQ:	/* 0x0e - Write/read request */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_WRITE_READ_REQ");
		rtn = ce_write_read_req(cd, q, mp);
		break;
	case CD_HALT_INPUT_REQ:	/* 0x11 - Halt input */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_HALT_INPUT_REQ");
		rtn = ce_halt_input_req(cd, q, mp);
		break;
	case CD_ABORT_OUTPUT_REQ:	/* 0x12 - Abort output */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_ABORT_OUTPUT_REQ");
		rtn = ce_abort_output_req(cd, q, mp);
		break;
	case CD_MUX_NAME_REQ:	/* 0x13 - get mux name (Gcom) */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_MUX_NAME_REQ");
		rtn = ce_mux_name_req(cd, q, mp);
		break;
	case CD_MODEM_SIG_REQ:	/* 0x15 - Assert modem signals (Gcom) */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_MODEM_SIG_REQ");
		rtn = ce_modem_sig_req(cd, q, mp);
		break;
	case CD_MODEM_SIG_POLL:	/* 0x17 - requests a CD_MODEM_SIG_IND (Gcom) */
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_MODEM_SIG_POLL");
		rtn = ce_modem_sig_poll(cd, q, mp);
		break;
	default:
		STRLOG(cd->mid, cd->sid, CDLOGTX, SL_TRACE, "-> CD_????_???");
		rtn = ce_other_req(cd, q, mp);
		break;
	}
	if (rtn)
		cd_set_state(cd, oldstate);
	cd_unlock(cd);
	return (rtn);
}
static noinline fastcall __unlikely int
cd_w_flush(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (cd->other && cd->other->oq) {
			putnext(cd->other->oq, mp);
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
static noinline fastcall __unlikely int
cd_w_ctl(queue_t *q, mblk_t *mp)
{
}
static noinline fastcall __unlikely int
cd_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = mp->b_cont ? (typeof(l)) mp->b_cont->b_rptr : NULL;
	struct cd *cd;
	mblk_t *rp;
	caddr_t ptr;
	int err;

	switch (ioc->ioc_cmd) {
	case I_PLINK:
		if (!drv_priv(ioc->ioc_cr)) {
			mi_copy_done(q, mp, EPERM);
			break;
		}
		/* fall through */
	case I_LINK:
		if (!(rp = allocb(sizeof(struct CD_info_req), BPRI_MED))) {
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		if (!(ptr = mi_open_alloc(sizeof(*cd)))) {
			mi_copy_done(q, mp, ENOMEM);
			break;
		}
		cd = (struct cd *) ptr;
		cd->index = l->l_index;
		cd->ppa = 0;
		if ((err = mi_open_link(&cd_links, ptr, &cd->dev, 0, DRVOPEN, ioc->ioc_cr))) {
			mi_close_free(ptr);
			break;
		}
		mi_attach(l->l_qtop, ptr);
		cd->ioc = mp;
		DB_TYPE(rp) = M_PCPROTO;
		((struct CD_info_req *) rp->b_rptr)->cd_primitive = CD_INFO_REQ;
		rp->b_wptr += sizeof(struct CD_info_req);
		putnext(cd->oq, mp);
		break;
	case I_PUNLINK:
		if (!drv_priv(ioc->ioc_cr)) {
			mi_copy_done(q, mp, EPERM);
			break;
		}
		/* fall through */
	case I_UNLINK:
		cd = CD_PRIV(l->l_qtop);
		ptr = (caddr_t) cd;
		mi_detach(l->l_qtop, ptr);
		mi_close_unlink(&cd_links, ptr);
		mi_close_free(ptr);
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	return (0);
}
static noinline fastcall __unlikely int
cd_w_iocdata(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline fastcall __unlikely int
cd_w_other(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	int err = 0;

	if (cd_trylock(cd, q)) {
		if (cd->other && cd->other->oq) {
			if (pcmsg(DB_TYPE(mp)) || bcanputnext(cd->other->oq, mp->b_band))
				putnext(cd->other->oq, mp);
			else
				err = -EBUSY;
		} else
			freemsg(mp);
		cd_unlock(cd);
	} else
		err = -EDEADLK;
	return (err);
}

static inline fastcall __hot_in int
mux_r_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	int err = 0;

	if (cd_trylock(cd, q)) {
		if (cd->other && cd->other->oq) {
			if (pcmsg(DB_TYPE(mp)) || bcanputnext(cd->other->oq, mp->b_band))
				putnext(cd->other->oq, mp);
			else
				err = -EBUSY;
		}
		cd_unlock(cd);
	} else
		err = -EDEADLK;
	return (err);
}
static noinline fastcall __unlikely int
mux_r_data_slow(queue_t *q, mblk_t *mp)
{
	return mux_r_data(q, mp);
}
static noinline fastcall __unlikely int
mux_r_proto(queue_t *q, mblk_t *mp)
{
	cd_ulong oldstate = cd_get_state(cd);
	int rtn;

	if (!cd_trylock(cd, q))
		return (-EDEADLK);

	/* If this lower multiplex stream is direclty connected to a an upper multiplex stream,
	   simply pass the message along with flow control. */
	if (cd->other && cd->other->oq) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(cd->other->oq, mp->b_band)) {
			putnext(cd->other->oq, mp);
			cd_unlock(cd);
			return (0);
		}
		cd_unlock(cd);
		return (-EBUSY);
	}

	switch (*(cd_ulong *) mp->b_rptr) {
	case CD_INFO_ACK:	/* 0x01 Information acknowledgement */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_INFO_ACK <-");
		rtn = ce_info_ack(cd, q, mp);
		break;
	case CD_OK_ACK:	/* 0x06 Success acknowledgement */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_OK_ACK <-");
		rtn = ce_ok_ack(cd, q, mp);
		break;
	case CD_ERROR_ACK:	/* 0x07 Error acknowledgement */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_ERROR_ACK <-");
		rtn = ce_error_ack(cd, q, mp);
		break;
	case CD_ENABLE_CON:	/* 0x08 Enable confirmation */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_ENABLE_CON <-");
		rtn = ce_enable_con(cd, q, mp);
		break;
	case CD_DISABLE_CON:	/* 0x09 Disable confirmation */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_DISABLE_CON <-");
		rtn = ce_disable_con(cd, q, mp);
		break;
	case CD_ERROR_IND:	/* 0x0a Error indication */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_ERROR_IND <-");
		rtn = ce_error_ind(cd, q, mp);
		break;
	case CD_UNITDATA_ACK:	/* 0x0f Data send acknowledgement */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_UNITDATA_ACK <-");
		rtn = ce_unitdata_ack(cd, q, mp);
		break;
	case CD_UNITDATA_IND:	/* 0x10 Data receive indication */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_UNITDATA_IND <-");
		rtn = ce_unitdata_ind(cd, q, mp);
		break;
	case CD_BAD_FRAME_IND:	/* 0x14 frame w/error (Gcom extension) */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_BAD_FRAME_IND <-");
		rtn = ce_bad_frame_ind(cd, q, mp);
		break;
	case CD_MODEM_SIG_IND:	/* 0x16 Report modem signal state (Gcom) */
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_MODEM_SIG_IND <-");
		rtn = ce_modem_sig_ind(cd, q, mp);
		break;
	default:
		STRLOG(cd->mid, cd->sid, CDLOGRX, SL_TRACE, "CD_????_??? <-");
		rtn = ce_other_ind(cd, q, mp);
		break;
	}
	if (rtn)
		cd_set_state(cd, oldstate);
	cd_unlock(cd);
	return (rtn);
}
static noinline fastcall __unlikely int
mux_r_flush(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (cd->other && cd->other->oq) {
			putnext(cd->other->oq, mp);
			return (0);
		}
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static noinline fastcall __unlikely int
mux_r_ctl(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	int err = 0;

	if (cd_trylock(cd, q)) {
		if (cd->other && cd->other->oq) {
			if (pcmsg(DB_TYPE(mp)) || bcanputnext(cd->other->oq, mp->b_band))
				putnext(cd->other->oq, mp);
			else
				err = -EBUSY;
		}
		cd_unlock(cd);
	} else
		err = -EDEADLK;
	return (err);
}
static noinline fastcall __unlikely int
mux_r_other(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	int err = 0;

	if (cd_trylock(cd, q)) {
		if (cd->other && cd->other->oq) {
			if (pcmsg(DB_TYPE(mp)) || bcanputnext(cd->other->oq, mp->b_band))
				putnext(cd->other->oq, mp);
			else
				err = -EBUSY;
		}
		cd_unlock(cd);
	} else
		err = -EDEADLK;
	return (err);
}

static noinline fastcall __unlikely int
cd_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return cd_w_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_w_proto(q, mp);
	case M_FLUSH:
		return cd_w_flush(q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_w_ctl(q, mp);
	case M_IOCTL:
		return cd_w_ioctl(q, mp);
	case M_IOCDATA:
		return cd_w_iocdata(q, mp);
	default:
		return cd_w_other(q, mp);
	}
}
static noinline fastcall __unlikely int
mux_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mux_r_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mux_r_proto(q, mp);
	case M_FLUSH:
		return mux_r_flush(q, mp);
	case M_CTL:
	case M_PCCTL:
		return mux_r_ctl(q, mp);
	default:
		return mux_r_other(q, mp);
	}
}

static inline fastcall bool
cd_w_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return cd_w_data(q, mp);
	return (cd_w_msg_slow(q, mp));
}

static inline fastcall bool
mux_r_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return mux_r_data(q, mp);
	return (mux_r_msg_slow(q, mp));
}

static streamscall int
cd_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || cd_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
cd_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (cd_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall int
cd_rsrv(queue_t *q)
{
	struct cd *cd = CD_PRIV(q);

	if (cd_trylock(cd, q)) {
		if (cd->other && cd->other->iq)
			qenable(cd->other->iq);
		cd_unlock(cd);
	}
}
static streamscall int
cd_rput(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	STRLOG(cd->mid, cd->sid, 0, SL_ERROR, "software error: %s %d", __FILE__, __LINE__);
	putnext(q, mp);
	return (0);
}
static streamscall int
mux_wput(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);

	STRLOG(cd->mid, cd->sid, 0, SL_ERROR, "software error: %s %d", __FILE__, __LINE__);
	putnext(q, mp);
	return (0);
}
static streamscall int
mux_wsrv(queue_t *q)
{
	struct cd *cd = CD_PRIV(q);

	if (cd_trylock(cd, q)) {
		if (cd->other && cd->other->iq)
			qenable(cd->other->iq);
		cd_unlock(cd);
	}
}
static streamscall int
mux_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mux_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall int
mux_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mux_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall int
cd_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	unsigned long flags;
	struct cd *cd;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */

	if (cmajor == major && cminor < 16)
		sflag = CLONEOPEN;

	if (sflag == MODOPEN)
		return (ENXIO);

	if (sflag == CLONEOPEN)
		cminor = (getminor(makedevice(0, 0xfffffff)) >> 1) + 1;

	if (sflag == DRVOPEN) {
		if (cminor >= (getminor(makedevice(0, -1)) >> 1) + 1)
			return (ENXIO);
	}

	*devp = makedevice(cmajor, cminor);

	spin_lock_irqsave(&cd_lock, flags);
	err = mi_open_comm(&cd_opens, q, devp, oflags, sflag, crp);
	spin_unlock_irqrestore(&cd_lock, flags);
	if (err)
		return (err);
	cd_init_priv(q);
	qprocson(q);
}

static streamscall int
cd_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct cd *cd = CD_PRIV(q);
	unsigned long flags;

	qprocsoff(q);
	mi_detach(q, (caddr_t) cd);

	spin_lock_irqsave(&cd_lock, flags);
	mi_close_unlink(&cd_opens, (caddr_t) cd);
	spin_unlock_irqrestore(&cd_lock, flags);

	cd_free_priv(cd);
	mi_close_free((caddr_t) cd);
	return (0);
}

static struct module_info cd_minfo = {
	.mi_idnum = DRV_ID,		/* Driver module id number */
	.mi_idname = DRV_NAME,		/* Driver name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat cd_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit cd_rinit = {
	.qi_putp = &cd_rput,		/* Read put (message from below) */
	.qi_srvp = &cd_rsrv,		/* Read queue service */
	.qi_qopen = &cd_qopen,		/* Each open */
	.qi_qclose = &cd_qclose,	/* Last close */
	.qi_minfo = &cd_minfo,		/* Information */
	.qi_mstat = &cd_mstat,		/* Statistics */
};

static struct qinit cd_winit = {
	.qi_putp = &cd_wput,		/* Write put (message from above) */
	.qi_srvp = &cd_wsrv,		/* Write queue service */
	.qi_minfo = &cd_minfo,		/* Information */
	.qi_mstat = &cd_mstat,		/* Statistics */
};

static struct module_info mux_minfo = {
	.mi_idnum = DRV_ID,		/* Driver module id number */
	.mi_idname = DRV_NAME,		/* Driver name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat mux_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit mux_rinit = {
	.qi_putp = &mux_rput,		/* Read put (message from below) */
	.qi_srvp = &mux_rsrv,		/* Read queue service */
	.qi_minfo = &mux_minfo,		/* Information */
	.qi_mstat = &mux_mstat,		/* Statistics */
};

static struct qinit mux_winit = {
	.qi_putp = &mux_wput,		/* Write put (message from above) */
	.qi_srvp = &mux_wsrv,		/* Write queue service */
	.qi_minfo = &mux_minfo,		/* Information */
	.qi_mstat = &mux_mstat,		/* Statistics */
};

static struct streamtab hdlcinfo = {
	.st_rdinit = &cd_rinit,		/* Upper multiplex read queue */
	.st_wrinit = &cd_winit,		/* Upper multiplex write queue */
	.st_muxrinit = &mux_rinit,	/* Lower multiplex read queue */
	.st_muxwinit = &mux_winit,	/* Lower multiplex write queue */
};

static struct cdevsw cd_cdev = {
	.d_name = DRV_NAME,		/* Driver name */
	.d_str = &hdlcinfo,		/* Driver streamtab */
	.d_flag = D_MP,			/* Driver flags */
	.d_fop = NULL,			/* Driver operations (NULL for stream head) */
	.d_mode = S_IFCHR,		/* Device type */
	.d_kmod = THIS_MODULE,		/* Module owner */
};
static __init int
hdlcinit(void)
{
	int err;

	if ((err = register_strdev(&cd_cdev, major)) < 0) {
		STRLOG(modid, 0, 0, SL_CONSOLE | SL_FATAL,
		       "could not register STREAMS device, err = %d", err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}
static __exit void
hdlcexit(void)
{
	int err;

	if ((err = unregsiter_strdev(&cd_cdev, major)) < 0)
		STRLOG(modid, 0, 0, SL_CONSOLE | SL_FATAL,
		       "could not register STREAMS device, err = %d", err);
	return;
}

module_init(hdlcinit);
module_exit(hdlcexit);
