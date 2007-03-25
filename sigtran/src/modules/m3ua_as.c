/*****************************************************************************

 @(#) $RCSfile: m3ua_as.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/03/25 18:59:07 $

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

 Last Modified $Date: 2007/03/25 18:59:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua_as.c,v $
 Revision 0.9.2.5  2007/03/25 18:59:07  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.4  2007/03/05 23:01:42  brian
 - checking in release changes

 Revision 0.9.2.3  2006/12/28 05:19:34  brian
 - minor changes

 Revision 0.9.2.2  2006/12/23 13:06:56  brian
 - manual page and other package updates for release

 Revision 0.9.2.1  2006/11/30 13:17:57  brian
 - added files from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: m3ua_as.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/03/25 18:59:07 $"

static char const ident[] =
    "$RCSfile: m3ua_as.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/03/25 18:59:07 $";

/*
 *  This is the AS side of M3UA implemented as a pushable module that pushes over an SCTP NPI
 *  stream.
 *
 *  The SCTP NPI stream can be a freshly opened stream (i.e. in the NS_UNBND state), or can be bound
 *  by the opener before pushing the M3UA-AS module, or can be connected by the opener before
 *  pushing the M3UA-AS module.  When the M3UA-AS module is pushed, it interrogates the state of the
 *  SCTP NPI stream beneath it and flushes the read queue (so that no messages get past the module).
 *
 *  When an MTP bind is performed, the SCTP stream will be bound (if it is not already bound).  If
 *  the SCTP stream is already connected, the bind operation initiates ASP Up procedure with the
 *  bound ASPID.
 *
 *  When an MTP bind is performed, the SCTP stream will be connected (if it is not already
 *  connected).  If the SCTP stream is already connected, and the ASP Up operation has completed, an
 *  ASP Active for the bound RC is initiated.  Because MTP streams are bound by address
 *  specification, the default behaviour of the AS is to register the address with the peer and
 *  obtain a Routing Context if one is not provided in the address.  An M3UA address differs
 *  slightly from that of MTP in that an NA and RC value are also provided in the address.  Once UP
 *  and registered, the routing context is activated.  All this occurs during the bind opertion for
 *  MTP.  (Note that if the SI value is pseudo-connection-oriented, such as ISUP, then an additional
 *  MTP connect operation is required before a routing key can be registered, a routing context
 *  obtained, and the AS activated.)
 *
 *  Once the interface is enabled, the M3UA-AS module audits the state of the MTP for the bound
 *  address by using the DAUD query.
 *
 *  At this point, MTP-primitmives can be issued by the MTP User that will be translated into M3UA
 *  messages for the activated RC.  M3UA messages received for the activated RC will be translated
 *  into MTP-primitives and sent upstream.
 *
 *  Unlike M2UA-AS, instead of a pushable module, M3UA-AS is better implemented as a multiplexing
 *  driver.  I will probably abandon this module.
 */

#ifndef HAVE_KTYPE_BOOL
#include <stdbool.h>
#endif

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#define _DEBUG 1
//#undef _DEBUG

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#undef mi_timer
#define mi_timer mi_timer_MAC

#undef DB_TYPE
#define DB_TYPE(mp) mp->b_datap->db_type

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#include <sys/tihdr.h>
#include <sys/tpi_mtp.h>

#define MTPLOGST	1	/* log MTP state transitions */
#define MTPLOGTO	2	/* log MTP timeouts */
#define MTPLOGRX	3	/* log MTP primitives received */
#define MTPLOGTX	4	/* log MTP primitives issued */
#define MTPLOGTE	5	/* log MTP timer events */
#define MTPLOGDA	6	/* log MTP data */

/* ======================= */

#define M3UA_AS_DESCRIP		"M3UA/SCTP MESSAGE TRANSFER PART (MTP) STREAMS MODULE."
#define M3UA_AS_REVISION	"OpenSS7 $RCSfile: m3ua_as.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/03/25 18:59:07 $"
#define M3UA_AS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define M3UA_AS_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define M3UA_AS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M3UA_AS_LICENSE		"GPL"
#define M3UA_AS_BANNER		M3UA_AS_DESCRIP		"\n" \
				M3UA_AS_REVISION	"\n" \
				M3UA_AS_COPYRIGHT	"\n" \
				M3UA_AS_DEVICE		"\n" \
				M3UA_AS_CONTACT		"\n"
#define M3UA_AS_SPLASH		M3UA_AS_DEVICE		" - " \
				M3UA_AS_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M3UA_AS_CONTACT);
MODULE_DESCRIPTION(M3UA_AS_DESCRIP);
MODULE_SUPPORTED_DEVICE(M3UA_AS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M3UA_AS_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m3ua_as");
#endif
#endif				/* LINUX */

#ifdef LFS
#define M3UA_AS_MOD_ID		CONFIG_STREAMS_M3UA_AS_MODID
#define M3UA_AS_MOD_NAME	CONFIG_STREAMS_M3UA_AS_NAME
#endif

#define MOD_ID	    M3UA_AS_MOD_ID
#define MOD_NAME    M3UA_AS_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH  M3UA_AS_BANNER
#else				/* MODULE */
#define MOD_SPLASH  M3UA_AS_SPLASH
#endif				/* MODULE */

static struct module_info mtp_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat mtp_mstat __attribute__((aligned(SMP_CACHE_BYTES)));

/*
 *  M3UA-AS Private Structure
 */
struct mtp {
	STR_DECLARATION (struct mtp);	/* stream declaration */
	int mid;			/* module id */
	int sid;			/* stream id */
	struct task_struct *owner;
	queue_t *waitq;
	int u_state;
	int n_state;
	uint32_t cong;
	uint32_t disc;
	struct mtp_addr orig;
	struct mtp_addr dest;
	mblk_t *aspup_tack;
	mblk_t *aspac_tack;
	mblk_t *aspdn_tack;
	mblk_t *aspia_tack;
	mblk_t *hbeat_tack;
	int audit;
	uint32_t tm;			/* traffic mode */
	uint32_t rc;			/* routing context */
	uint32_t aspid;			/* ASP identifier */
	int coninds;			/* npi connection indications */
	struct {
		N_info_ack_t n;		/* NPI information */
		struct MTP_info_ack mtp;	/* MTP information */
	} info;
	size_t loc_len;			/* local address length */
	size_t rem_len;			/* remote address length */
	struct sockaddr_in loc[8];	/* up to 8 local addresses */
	struct sockaddr_in rem[8];	/* up to 8 remote addresses */
};

#define MTP_PRIV(__q) ((struct mtp *)(__q)->q_ptr)

#if 0
#define NSF_UNBND	(1<<NS_UNBND)
#define NSF_WACK_BREQ	(1<<NS_WACK_BREQ)
#define NSF_WACK_UREQ	(1<<NS_WACK_UREQ)
#define NSF_IDLE	(1<<NS_IDLE)
#define NSF_WACK_OPTREQ	(1<<NS_WACK_OPTREQ)
#define NSF_WACK_RRES	(1<<NS_WACK_RRES)
#define NSF_WCON_CREQ	(1<<NS_WCON_CREQ)
#define NSF_WRES_CIND	(1<<NS_WRES_CIND)
#define NSF_WACK_CRES	(1<<NS_WACK_CRES)
#define NSF_DATA_XFER	(1<<NS_DATA_XFER)
#define NSF_WCON_RREQ	(1<<NS_WCON_RREQ)
#define NSF_WRES_RIND	(1<<NS_WRES_RIND)
#define NSF_WACK_DREQ6	(1<<NS_WACK_DREQ6)
#define NSF_WACK_DREQ7	(1<<NS_WACK_DREQ7)
#define NSF_WACK_DREQ9	(1<<NS_WACK_DREQ9)
#define NSF_WACK_DREQ10	(1<<NS_WACK_DREQ10)
#define NSF_WACK_DREQ11	(1<<NS_WACK_DREQ11)
#endif

const char *
mtp_n_state_name(int state)
{
	switch (state) {
	case NS_UNBND:		/* NS user not bound to network address */
		return ("NS_UNBND");
	case NS_WACK_BREQ:	/* Awaiting acknowledgement of N_BIND_REQ */
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:	/* Pending acknowledgement for N_UNBIND_REQ */
		return ("NS_WACK_UREQ");
	case NS_IDLE:		/* Idle, no connection */
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:	/* Pending acknowledgement of N_OPTMGMT_REQ */
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:	/* Pending acknowledgement of N_RESET_RES */
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:	/* Pending confirmation of N_CONN_REQ */
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:	/* Pending response of N_CONN_REQ */
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:	/* Pending acknowledgement of N_CONN_RES */
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:	/* Connection-mode data transfer */
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:	/* Pending confirmation of N_RESET_REQ */
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:	/* Pending response of N_RESET_IND */
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
static int
mtp_set_n_state(struct mtp *mtp, int newstate)
{
	int oldstate = mtp->info.n.CURRENT_state;

	strlog(mtp->mid, mtp->sid, MTPLOGST, SL_TRACE, "%s <- %s", mtp_n_state_name(newstate),
	       mtp_n_state_name(oldstate));
	return ((mtp->info.n.CURRENT_state = newstate));
}
static inline int
mtp_get_n_state(struct mtp *mtp)
{
	return (mtp->info.n.CURRENT_state);
}
static inline int
mtp_get_n_statef(struct mtp *mtp)
{
	return ((1 << mtp_get_n_state(mtp)));
}
static inline int
mtp_chk_n_state(struct mtp *mtp, int mask)
{
	return (mtp_get_n_statef(mtp) & mask);
}
static inline int
mtp_not_n_state(struct mtp *mtp, int mask)
{
	return (mtp_chk_n_state(mtp, ~mask));
}

#define MTPSF_UNBND		(1<<MTPS_UNBND)
#define MTPSF_WACK_BREQ		(1<<MTPS_WACK_BREQ)
#define MTPSF_IDLE		(1<<MTPS_IDLE)
#define MTPSF_WACK_CREQ		(1<<MTPS_WACK_CREQ)
#define MTPSF_WCON_CREQ		(1<<MTPS_WCON_CREQ)
#define MTPSF_CONNECTED		(1<<MTPS_CONNECTED)
#define MTPSF_WACK_UREQ		(1<<MTPS_WACK_UREQ)
#define MTPSF_WACK_DREQ6	(1<<MTPS_WACK_DREQ6)
#define MTPSF_WACK_DREQ9	(1<<MTPS_WACK_DREQ9)
#define MTPSF_WACK_OPTREQ	(1<<MTPS_WACK_OPTREQ)
#define MTPSF_WREQ_ORDREL	(1<<MTPS_WREQ_ORDREL)
#define MTPSF_WIND_ORDREL	(1<<MTPS_WIND_ORDREL)
#define MTPSF_WRES_CIND		(1<<MTPS_WRES_CIND)
#define MTPSF_UNUSABLE		(1<<MTPS_UNUSABLE)

const char *
mtp_i_state_name(int state)
{
	switch (state) {
	case MTPS_UNBND:
		return ("MTPS_UNBND");
	case MTPS_WACK_BREQ:
		return ("MTPS_WACK_BREQ");
	case MTPS_IDLE:
		return ("MTPS_IDLE");
	case MTPS_WACK_CREQ:
		return ("MTPS_WACK_CREQ");
	case MTPS_WCON_CREQ:
		return ("MTPS_WCON_CREQ");
	case MTPS_CONNECTED:
		return ("MTPS_CONNECTED");
	case MTPS_WACK_UREQ:
		return ("MTPS_WACK_UREQ");
	case MTPS_WACK_DREQ6:
		return ("MTPS_WACK_DREQ6");
	case MTPS_WACK_DREQ9:
		return ("MTPS_WACK_DREQ9");
	case MTPS_WACK_OPTREQ:
		return ("MTPS_WACK_OPTREQ");
	case MTPS_WREQ_ORDREL:
		return ("MTPS_WREQ_ORDREL");
	case MTPS_WIND_ORDREL:
		return ("MTPS_WIND_ORDREL");
	case MTPS_WRES_CIND:
		return ("MTPS_WRES_CIND");
	case MTPS_UNUSABLE:
		return ("MTPS_UNUSABLE");
	default:
		return ("(unknown)");
	}
}
static int
mtp_set_i_state(struct mtp *mtp, int newstate)
{
	int oldstate = mtp->info.mtp.mtp_current_state;

	strlog(mtp->mid, mtp->sid, MTPLOGST, SL_TRACE, "%s <- %s", mtp_i_state_name(newstate),
	       mtp_i_state_name(oldstate));
	return ((mtp->info.mtp.mtp_current_state = mtp->info.mtp.mtp_current_state = newstate));
}
static inline int
mtp_get_i_state(struct mtp *mtp)
{
	return mtp->info.mtp.mtp_current_state;
}
static inline int
mtp_get_i_statef(struct mtp *mtp)
{
	return (1 << mtp_get_i_state(mtp));
}
static inline int
mtp_chk_i_state(struct mtp *mtp, int mask)
{
	return (mtp_get_i_statef(mtp) & mask);
}
static inline int
mtp_not_i_state(struct mtp *mtp, int mask)
{
	return (mtp_chk_i_state(mtp, ~mask));
}

#define ASP_DOWN	0
#define ASP_WACK_ASPUP	1
#define ASP_WACK_ASPDN	2
#define ASP_UP		3
#define ASP_INACTIVE	4
#define ASP_WACK_ASPAC	5
#define ASP_WACK_ASPIA	6
#define ASP_ACTIVE	7

const char *
mtp_u_state_name(int state)
{
	switch (state) {
	case ASP_DOWN:
		return ("ASP_DOWN");
	case ASP_WACK_ASPUP:
		return ("ASP_WACK_ASPUP");
	case ASP_WACK_ASPDN:
		return ("ASP_WACK_ASPDN");
	case ASP_UP:
		return ("ASP_UP");
	case ASP_INACTIVE:
		return ("ASP_INACTIVE");
	case ASP_WACK_ASPAC:
		return ("ASP_WACK_ASPAC");
	case ASP_WACK_ASPIA:
		return ("ASP_WACK_ASPIA");
	case ASP_ACTIVE:
		return ("ASP_ACTIVE");
	default:
		return ("ASP_????");
	}
}
static inline int
mtp_set_u_state(struct mtp *mtp, int newstate)
{
	int oldstate = mtp->u_state;

	strlog(mtp->mid, mtp->sid, MTPLOGST, SL_TRACE, "%s <- %s", mtp_u_state_name(newstate),
	       mtp_u_state_name(oldstate));
	return (mtp->u_state = newstate);
}
static inline int
mtp_get_u_state(struct mtp *mtp)
{
	return (mtp->u_state);
}
static inline int
mtp_get_u_statef(struct mtp *mtp)
{
	return ((1 << mtp_get_u_state(mtp)));
}
static inline int
mtp_chk_u_state(struct mtp *mtp, int mask)
{
	return (mtp_get_u_statef(mtp) & mask);
}
static inline int
mtp_not_u_state(struct mtp *mtp, int mask)
{
	return (mtp_chk_u_state(mtp, ~mask));
}

/*
 *  Buffer allocation
 */
/**
 * mtp_allocb: - allocate a buffer reliably
 * @q: active queue
 * @size: size of allocation
 * @priority: priority of allocation
 *
 * If allocation of a message block fails and this procedure returns NULL, the caller should place
 * the invoking message back on queue, q, and await q being enabled when a buffer becomes available.
 */
static inline fastcall mblk_t *
mtp_allocb(queue_t *q, size_t size, int priority)
{
	mblk_t *mp;

	if (unlikely(!(mp = allocb(size, priority))))
		mi_bufcall(q, size, priority);
	return (mp);
}

/*
 *  Locking
 */
/**
 * mtp_trylock: - try to lock an MTP queue pair
 * @mtp: MTP private structure
 * @q: active queue (read or write queue)
 *
 * Note that if we awlays run (at least initially) from a service procedure, there is no need to
 * suppress interrupts while holding the lock.  This simple lock will do becuase the service
 * procedure is guaranteed single threaded on UP and SMP machines.  Also, because interrupts do not
 * need to be suppressed while holding the lock, the current task pointer identifies the thread and
 * the thread can be allowed to recurse on the lock.  When a queue procedure fails to acquire the
 * lock, it is marked to have its service procedure shceduled later, but we only remember one queue,
 * so if there are two waiting, the second one is reenabled.
 */
static inline bool
mtp_trylock(struct mtp *mtp, queue_t *q)
{
	bool rtn = false;
	unsigned long flags;

	spin_lock_irqsave(&mtp->lock, flags);
	if (mtp->users == 0 && (q->q_flag & QSVCBUSY)) {
		rtn = true;
		mtp->users = 1;
		mtp->owner = current;
	} else if (mtp->users != 0 && mtp->owner == current) {
		rtn = true;
		mtp->users++;
	} else if (!mtp->waitq) {
		mtp->waitq = q;
	} else if (mtp->waitq != q) {
		qenable(q);
	}
	spin_unlock_irqrestore(&mtp->lock, flags);
	return (rtn);
}

/**
 * mtp_unlock: - unlock an MTP queue pair
 * @mtp: MTP private structure
 */
static inline void
mtp_unlock(struct mtp *mtp)
{
	unsigned long flags;

	spin_lock_irqsave(&mtp->lock, flags);
	if (--mtp->users == 0 && mtp->waitq) {
		qenable(mtp->waitq);
		mtp->waitq = NULL;
	}
	spin_unlock_irqrestore(&mtp->lock, flags);
}

/*
 *  M3UA Message Definitions
 */

#define M3UA_PPI    3

#define UA_VERSION  1
#define UA_PAD4(__len) (((__len)+3)&~0x3)
#define UA_MHDR(__version, __spare, __class, __type) \
	(__constant_htonl(((__version)<<24)|((__spare)<<16)|((__class)<<8)|(__type)))

#define UA_MSG_VERS(__hdr) ((ntohl(__hdr)>>24)&0xff)
#define UA_MSG_CLAS(__hdr) ((ntohl(__hdr)>> 8)&0xff)
#define UA_MSG_TYPE(__hdr) ((ntohl(__hdr)>> 0)&0xff)

/*
 *  MESSAGE CLASSES:-
 */
#define UA_CLASS_MGMT	0x00	/* UA Management (MGMT) Message */
#define UA_CLASS_XFER	0x01	/* M3UA Data transfer message */
#define UA_CLASS_SNMM	0x02	/* Signalling Network Mgmt (SNM) Messages */
#define UA_CLASS_ASPS	0x03	/* ASP State Maintenance (ASPSM) Messages */
#define UA_CLASS_ASPT	0x04	/* ASP Traffic Maintenance (ASPTM) Messages */
#define UA_CLASS_Q921	0x05	/* Q.931 User Part Messages */
#define UA_CLASS_MAUP	0x06	/* M2UA Messages */
#define UA_CLASS_CNLS	0x07	/* SUA Connectionless Messages */
#define UA_CLASS_CONS	0x08	/* SUA Connection Oriented Messages */
#define UA_CLASS_RKMM	0x09	/* Routing Key Management Messages */
#define UA_CLASS_TDHM	0x0a	/* TUA Dialog Handling Mesages */
#define UA_CLASS_TCHM	0x0b	/* TUA Component Handling Messages */

/*
 *  MESSAGES DEFINED IN EACH CLASS:-
 */
#define UA_MGMT_ERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x00)
#define UA_MGMT_NTFY		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x01)
#define UA_MGMT_LAST		0x01

#define UA_XFER_DATA		UA_MHDR(UA_VERSION, 0, UA_CLASS_XFER, 0x01)

#define UA_SNMM_DUNA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x01)
#define UA_SNMM_DAVA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x02)
#define UA_SNMM_DAUD		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x03)
#define UA_SNMM_SCON		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x04)
#define UA_SNMM_DUPU		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x05)
#define UA_SNMM_DRST		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x06)
#define UA_SNMM_LAST		0x06

#define UA_ASPS_ASPUP_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x01)
#define UA_ASPS_ASPDN_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x02)
#define UA_ASPS_HBEAT_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x03)
#define UA_ASPS_ASPUP_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x04)
#define UA_ASPS_ASPDN_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x05)
#define UA_ASPS_HBEAT_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x06)
#define UA_ASPS_LAST		0x06

#define UA_ASPT_ASPAC_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x01)
#define UA_ASPT_ASPIA_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x02)
#define UA_ASPT_ASPAC_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x03)
#define UA_ASPT_ASPIA_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x04)
#define UA_ASPT_LAST		0x04

#define UA_RKMM_REG_REQ		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x01)
#define UA_RKMM_REG_RSP		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x02)
#define UA_RKMM_DEREG_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x03)
#define UA_RKMM_DEREG_RSP	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x04)
#define UA_RKMM_LAST		0x04

#define UA_MHDR_SIZE (sizeof(uint32_t)*2)
#define UA_PHDR_SIZE (sizeof(uint32_t))
#define UA_MAUP_SIZE (UA_MHDR_SIZE + UA_PHDR_SIZE + sizeof(uint32_t))

#define UA_TAG_MASK		(__constant_htonl(0xffff0000))
#define UA_PTAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PLEN(__phdr)		(htonl(__phdr)&0xffff)
#define UA_SIZE(__phdr)		(__constant_htonl(__phdr)&0xffff)
#define UA_TAG(__phdr)		((__constant_htonl(__phdr)>>16)&0xffff)
#define UA_PHDR(__phdr, __length) \
	(htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))
#define UA_CONST_PHDR(__phdr, __length) \
	(__constant_htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))

/*
 *  COMMON PARAMETERS:-
 *
 *  Common parameters per draft-ietf-sigtran-m2ua-10.txt
 *  Common parameters per draft-ietf-sigtran-m3ua-08.txt
 *  Common parameters per draft-ietf-sigtran-sua-07.txt
 *  Common parameters per rfc3057.txt
 *  -------------------------------------------------------------------
 */
#define UA_PARM_RESERVED	UA_CONST_PHDR(0x0000,0)
#define UA_PARM_IID		UA_CONST_PHDR(0x0001,sizeof(uint32_t))
#define UA_PARM_IID_RANGE	UA_CONST_PHDR(0x0002,0)	/* m2ua-10 */
#define UA_PARM_DATA		UA_CONST_PHDR(0x0003,0)	/* sua-07 */
#define UA_PARM_IID_TEXT	UA_CONST_PHDR(0x0003,0)
#define UA_PARM_INFO		UA_CONST_PHDR(0x0004,0)
#define UA_PARM_APC		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_DLCI		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_RC		UA_CONST_PHDR(0x0006,sizeof(uint32_t))
#define UA_PARM_DIAG		UA_CONST_PHDR(0x0007,0)
#define UA_PARM_IID_RANGE1	UA_CONST_PHDR(0x0008,0)	/* rfc3057 */
#define UA_PARM_HBDATA		UA_CONST_PHDR(0x0009,0)
#define UA_PARM_REASON		UA_CONST_PHDR(0x000a,sizeof(uint32_t))
#define UA_PARM_TMODE		UA_CONST_PHDR(0x000b,sizeof(uint32_t))
#define UA_PARM_ECODE		UA_CONST_PHDR(0x000c,sizeof(uint32_t))
#define UA_PARM_STATUS		UA_CONST_PHDR(0x000d,sizeof(uint32_t))
#define UA_PARM_ASPID		UA_CONST_PHDR(0x000e,sizeof(uint32_t))
#define UA_PARM_PROT_DATA	UA_CONST_PHDR(0x000e,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_CONG_LEVEL	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_REL_REASON	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_TEI_STATUS	UA_CONST_PHDR(0x0010,sizeof(uint32_t))	/* rfc3057 */

/*
 *  Somewhat common field values:
 */
#define   UA_ECODE_INVALID_VERSION		(0x01)
#define   UA_ECODE_INVALID_IID			(0x02)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_CLASS	(0x03)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_TYPE	(0x04)
#define   UA_ECODE_UNSUPPORTED_TRAFFIC_MODE	(0x05)
#define   UA_ECODE_UNEXPECTED_MESSAGE		(0x06)
#define   UA_ECODE_PROTOCOL_ERROR		(0x07)
#define   UA_ECODE_UNSUPPORTED_IID_TYPE		(0x08)
#define   UA_ECODE_INVALID_STREAM_IDENTIFIER	(0x09)
#define  IUA_ECODE_UNASSIGNED_TEI		(0x0a)
#define  IUA_ECODE_UNRECOGNIZED_SAPI		(0x0b)
#define  IUA_ECODE_INVALID_TEI_SAPI_COMBINATION	(0x0c)
#define   UA_ECODE_REFUSED_MANAGEMENT_BLOCKING	(0x0d)
#define   UA_ECODE_ASPID_REQUIRED		(0x0e)
#define   UA_ECODE_INVALID_ASPID		(0x0f)
#define M2UA_ECODE_ASP_ACTIVE_FOR_IIDS		(0x10)
#define   UA_ECODE_INVALID_PARAMETER_VALUE	(0x11)
#define   UA_ECODE_PARAMETER_FIELD_ERROR	(0x12)
#define   UA_ECODE_UNEXPECTED_PARAMETER		(0x13)
#define   UA_ECODE_DESTINATION_STATUS_UNKNOWN	(0x14)
#define   UA_ECODE_INVALID_NETWORK_APPEARANCE	(0x15)
#define   UA_ECODE_MISSING_PARAMETER		(0x16)
#define   UA_ECODE_ROUTING_KEY_CHANGE_REFUSED	(0x17)
#define   UA_ECODE_INVALID_ROUTING_CONTEXT	(0x19)
#define   UA_ECODE_NO_CONFIGURED_AS_FOR_ASP	(0x1a)
#define   UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN	(0x1b)

#define UA_STATUS_AS_DOWN			(0x00010001)
#define UA_STATUS_AS_INACTIVE			(0x00010002)
#define UA_STATUS_AS_ACTIVE			(0x00010003)
#define UA_STATUS_AS_PENDING			(0x00010004)
#define UA_STATUS_AS_INSUFFICIENT_ASPS		(0x00020001)
#define UA_STATUS_ALTERNATE_ASP_ACTIVE		(0x00020002)
#define UA_STATUS_ASP_FAILURE			(0x00020003)
#define UA_STATUS_AS_MINIMUM_ASPS		(0x00020004)

#define UA_TMODE_OVERRIDE			(0x1)
#define UA_TMODE_LOADSHARE			(0x2)
#define UA_TMODE_BROADCAST			(0x3)
#define UA_TMODE_SB_OVERRIDE			(0x4)
#define UA_TMODE_SB_LOADSHARE			(0x5)
#define UA_TMODE_SB_BROADCAST			(0x6)

#define UA_RESULT_SUCCESS			(0x00)
#define UA_RESULT_FAILURE			(0x01)

/* M3UA Specific parameters. */

#define M3UA_PARM_NTWK_APP	UA_CONST_PHDR(0x0200,sizeof(uint32_t))
#define M3UA_PARM_PROT_DATA1	UA_CONST_PHDR(0x0201,0)
#define M3UA_PARM_PROT_DATA2	UA_CONST_PHDR(0x0202,0)
#define M3UA_PARM_AFFECT_DEST	UA_CONST_PHDR(0x0203,sizeof(uint32_t))
#define M3UA_PARM_USER_CAUSE	UA_CONST_PHDR(0x0204,sizeof(uint32_t))
#define M3UA_PARM_CONG_IND	UA_CONST_PHDR(0x0205,sizeof(uint32_t))
#define M3UA_PARM_CONCERN_DEST	UA_CONST_PHDR(0x0206,sizeof(uint32_t))
#define M3UA_PARM_ROUTING_KEY	UA_CONST_PHDR(0x0207,0)
#define M3UA_PARM_REG_RESULT	UA_CONST_PHDR(0x0208,sizeof(uint32_t))
#define M3UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x0209,sizeof(uint32_t)*4)
#define M3UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x020a,sizeof(uint32_t))
#define M3UA_PARM_DPC		UA_CONST_PHDR(0x020b,0)
#define M3UA_PARM_SI		UA_CONST_PHDR(0x020c,0)
#define M3UA_PARM_SSN		UA_CONST_PHDR(0x020d,0)
#define M3UA_PARM_OPC		UA_CONST_PHDR(0x020e,0)
#define M3UA_PARM_CIC		UA_CONST_PHDR(0x020f,0)
#define M3UA_PARM_PROT_DATA3	UA_CONST_PHDR(0x0210,0)	/* proposed */
#define M3UA_PARM_REG_STATUS	UA_CONST_PHDR(0x0212,sizeof(uint32_t))
#define M3UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0213,sizeof(uint32_t))

/*
 *  Decode functions.
 */
struct ua_parm {
	union {
		uchar *cp;		/* pointer to parameter field */
		uint32_t *wp;		/* pointer to parameter field */
	};
	size_t len;			/* length of parameter field */
	uint32_t val;			/* value of first 4 bytes (host order) */
};

/*
 *  Extract a parameter from a message.  If the parameter does not exist in the message it returns
 *  false; otherwise true, and sets the parameter values if parm is non-NULL.
 */
static bool
ua_dec_parm(struct mtp *mtp, queue_t *q, mblk_t *mp, struct ua_parm *parm, uint32_t tag)
{
	uint32_t *wp;
	bool rtn = false;

	for (wp = (uint32_t *) mp->b_rptr + 2; (uchar *) (wp + 1) <= mp->b_wptr;
	     wp += (UA_PLEN(*wp) + 3) >> 2) {
		if (UA_TAG(*wp) == UA_TAG(tag)) {
			rtn = true;
			if (parm) {
				parm->wp = (wp + 1);
				parm->len = UA_PLEN(*wp);
				parm->val = ntohl(wp[1]);
			}
			break;
		}
	}
	return (rtn);
}

/*
 *  =========================================================================
 *
 *  OUTPUT Events
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  MTP Provider (M3UA) -> MTP User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * mtp_ok_ack: - issue an MTP_OK_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static inline int
mtp_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, mtp_ulong prim)
{
	struct MTP_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OK_ACK;
		p->mtp_correct_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_OK_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_error_ack: - issue an MTP_ERROR_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: error (MTPI error positive; UNIX error negative)
 */
static int
mtp_error_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, mtp_ulong prim, mtp_long err)
{
	struct MTP_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ERROR_ACK;
		p->mtp_error_primitive = prim;
		p->mtp_mtpi_error = err < 0 ? MSYSERR : err;
		p->mtp_unix_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_ERROR_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_bind_ack: - issue an MTP_BIND_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: length of address
 */
static int
mtp_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MTP_bind_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_BIND_ACK;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_BIND_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_addr_ack: - issue an MTP_ADDR_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @lptr: local address pointer
 * @llen: local address length
 * @rptr: remote address pointer
 * @rlen: remote address length
 */
static inline int
mtp_addr_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t lptr, size_t llen, caddr_t rptr,
	     size_t rlen)
{
	struct MTP_addr_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p) + llen + rlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_ACK;
		p->mtp_loc_length = llen;
		p->mtp_loc_offset = llen ? sizeof(*p) : 0;
		p->mtp_rem_length = rlen;
		p->mtp_rem_offset = rlen ? sizeof(*p) + llen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(lptr, mp->b_wptr, llen);
		mp->b_wptr += llen;
		bcopy(rptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_ADDR_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_info_ack: - issue an MTP_INFO_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 * @type: service type
 */
static inline int
mtp_info_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, mtp_ulong type)
{
	struct MTP_info_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_msu_size = 272;	/* XXX */
		p->mtp_addr_size = sizeof(struct mtp_addr);
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		p->mtp_current_state = mtp_get_i_state(mtp);
		p->mtp_serv_type = type;
		p->mtp_version = MTP_CURRENT_VERSION;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_INFO_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_ack: - issue an MTP_OPTMGMT_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @optr: options pointer
 * @olen: options length
 * @flags: options flags
 */
static inline int
mtp_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen,
		mtp_ulong flags)
{
	struct MTP_optmgmt_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OPTMGMT_ACK;
		p->mtp_opt_length = olen;
		p->mtp_opt_offset = olen ? sizeof(*p) : 0;
		p->mtp_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_OPTMGMT_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_transfer_ind: - issue an MTP_TRANSFER_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sptr: source address pointer
 * @slen: source address length
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static int
mtp_transfer_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t sptr, size_t slen, mtp_ulong pri,
		 mtp_ulong sls, mblk_t *dp)
{
	struct MTP_transfer_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p) + slen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_TRANSFER_IND;
		p->mtp_srce_length = slen;
		p->mtp_srce_offset = slen ? sizeof(*p) : 0;
		p->mtp_mp = pri;
		p->mtp_sls = sls;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		mp->b_cont = dp;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_TRANSFER_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_pause_ind: - issue an MTP_PAUSE_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: addres length
 */
static int
mtp_pause_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MTP_pause_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_PAUSE_IND;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_PAUSE_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_resume_ind: - issue an MTP_RESUME_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 */
static int
mtp_resume_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MTP_resume_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_RESUME_IND;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_RESUME_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_status_ind: - issue an MTP_STATUS_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 * @type: status type
 * @status: status
 */
static int
mtp_status_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, mtp_ulong type,
	       mtp_ulong status)
{
	struct MTP_status_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_STATUS_IND;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		p->mtp_type = type;
		p->mtp_status = status;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_STATUS_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_begins_ind: - issue an MTP_RESTART_BEGINS_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
mtp_restart_begins_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_begins_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_RESTART_BEGINS_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_RESTART_BEGINS_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_complete_ind: - issue an MTP_RESTART_COMPLETE_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
mtp_restart_complete_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_complete_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mtp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_RESTART_COMPLETE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTPLOGTX, SL_TRACE, "<- MTP_RESTART_COMPLETE_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NPI User (M3UA) -> NPI Provider (SCTP) Primitives
 *
 *  -------------------------------------------------------------------------
 */

/**
 * n_bind_req: - issue a bind request to fullfill an attach request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add_ptr: pointer to address to bind
 * @add_len: length of bind address
 */
static inline int
n_bind_req(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len)
{
	N_bind_req_t *p;
	mblk_t *mp;

	if (likely((mp = mtp_allocb(q, sizeof(*p) + add_len, BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = 0;
		p->BIND_flags = TOKEN_REQUEST;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		mtp_set_i_state(mtp, MTPS_WACK_BREQ);
		mtp_set_n_state(mtp, NS_WACK_BREQ);
		freemsg(msg);
		printd(("%s: %p: N_BIND_REQ ->\n", MOD_NAME, mtp));
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unbind_req: - issue an unbind request to fullfull a detach request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
n_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	if (likely((mp = mtp_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		mtp_set_i_state(mtp, MTPS_WACK_UREQ);
		mtp_set_n_state(mtp, NS_WACK_UREQ);
		freemsg(msg);
		printd(("%s: %p: N_UNBIND_REQ ->\n", MOD_NAME, mtp));
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_req: - issue a connection request to fulfull an enable request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dst_ptr: destination address
 * @dst_len: destination adresss length
 * @qos_ptr: quality of service parameters
 * @qos_len: quality of service parameters length
 */
static inline int
n_conn_req(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len)
{
	N_qos_sel_conn_sctp_t *n;
	N_conn_req_t *p;
	mblk_t *mp;

	if (likely((mp = mtp_allocb(q, sizeof(*p) + dst_len + sizeof(*n), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->QOS_length = sizeof(*n);
		p->QOS_offset = sizeof(*p) + dst_len;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		n = (typeof(n)) mp->b_wptr;
		n->n_qos_type = N_QOS_SEL_CONN_SCTP;
		n->i_streams = 2;
		n->o_streams = 2;
		mp->b_wptr += sizeof(*n);
		mtp_set_i_state(mtp, MTPS_WCON_CREQ);
		mtp_set_n_state(mtp, NS_WCON_CREQ);
		freemsg(msg);
		printd(("%s: %p: N_CONN_REQ ->\n", MOD_NAME, mtp));
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_req: - issue a disconnection request to fulfill a disable request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: disconnect reason
 */
static inline int
n_discon_req(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_discon_req_t *p;
	mblk_t *mp;

	if (likely((mp = mtp_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(WR(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_REQ;
			p->DISCON_reason = reason;
			p->RES_length = 0;
			p->RES_offset = 0;
			p->SEQ_number = 0;
			mp->b_wptr += sizeof(*p);
			mtp_set_i_state(mtp, MTPS_WACK_DREQ9);
			switch (mtp_get_n_state(mtp)) {
			case NS_WCON_CREQ:
				mtp_set_n_state(mtp, NS_WACK_DREQ6);
				break;
			case NS_WRES_CIND:
				mtp_set_n_state(mtp, NS_WACK_DREQ7);
				break;
			case NS_DATA_XFER:
				mtp_set_n_state(mtp, NS_WACK_DREQ9);
				break;
			case NS_WCON_RREQ:
				mtp_set_n_state(mtp, NS_WACK_DREQ10);
				break;
			case NS_WRES_RIND:
				mtp_set_n_state(mtp, NS_WACK_DREQ11);
				break;
			default:
				mtp_set_n_state(mtp, NS_IDLE);
				break;
			}
			freemsg(msg);
			printd(("%s: %p: N_DISCON_REQ ->\n", MOD_NAME, mtp));
			putnext(WR(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_data_req: - issue a data transfer request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @sid: SCTP stream on which to send
 * @dp: user data to transfer
 */
static inline fastcall __hot_out int
n_data_req(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong flags, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *n;
	N_data_req_t *p;
	mblk_t *mp;

	if (likely((mp = mtp_allocb(q, sizeof(*p) + sizeof(*n), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(WR(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			n = (typeof(n)) mp->b_wptr;
			n->n_qos_type = N_QOS_SEL_DATA_SCTP;
			n->ppi = M3UA_PPI;
			n->sid = sid;
			n->ssn = 0;
			n->tsn = 0;
			n->more = flags;
			mp->b_wptr += sizeof(*n);
			mp->b_cont = dp;
			freemsg(msg);
			printd(("%s: %p: N_DATA_REQ ->\n", MOD_NAME, mtp));
			putnext(WR(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_exdata_req: - issue an expedited data transfer request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @sid: SCTP stream on which to send
 * @dp: user data to transfer
 */
static inline fastcall int
n_exdata_req(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong flags, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *n;
	N_exdata_req_t *p;
	mblk_t *mp;

	if (likely((mp = mtp_allocb(q, sizeof(*p) + sizeof(*n), BPRI_MED)) != NULL)) {
		mp->b_band = 1;	/* expedite */
		if (likely(bcanputnext(WR(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			n = (typeof(n)) mp->b_wptr;
			n->n_qos_type = N_QOS_SEL_DATA_SCTP;
			n->ppi = M3UA_PPI;
			n->sid = sid;
			n->ssn = 0;
			n->tsn = 0;
			n->more = flags;
			mp->b_wptr += sizeof(*n);
			mp->b_cont = dp;
			freemsg(msg);
			printd(("%s: %p: N_EXDATA_REQ ->\n", MOD_NAME, mtp));
			putnext(WR(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  MTP AS -> MTP SG (M3UA) Sent Messages
 *
 *  --------------------------------------------------------------------------
 */
/**
 * mtp_send_mgmt_err: - send MGMT ERR message
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @ecode: error code
 * @dia_ptr: pointer to diagnostics
 * @dia_len: length of diagnostics
 */
static inline __unlikely int
mtp_send_mgmt_err(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t ecode, caddr_t dia_ptr,
		  size_t dia_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_ECODE) + dia_len ? UA_SIZE(UA_PARM_DIAG) +
	    UA_PAD4(dia_len) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_MGMT_ERR;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_ECODE;
		p[3] = htonl(ecode);
		p += 4;
		if (dia_len) {
			*p++ = UA_PHDR(UA_PARM_DIAG, dia_len);
			bcopy(dia_ptr, p, dia_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(dia_len);
		/* sent unordered and expedited on management stream */
		if (unlikely((err = n_exdata_req(mtp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);

	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_aspup_req: - send ASP Up
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @inf_ptr: Info pointer
 * @inf_len: Info length
 */
static int
mtp_send_asps_aspup_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t inf_ptr,
			size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		mtp_set_u_state(mtp, ASP_WACK_ASPUP);
		mi_timer(mtp->aspup_tack, 2000);
		/* send unordered and expedited on stream 0 */
		if (unlikely((err = n_exdata_req(mtp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_aspdn_req: - send ASP Down
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @inf_ptr: Info pointer
 * @inf_len: Info length
 */
static int
mtp_send_asps_aspdn_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t inf_ptr,
			size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		mtp_set_u_state(mtp, ASP_WACK_ASPDN);
		mi_timer(mtp->aspdn_tack, 2000);
		if (unlikely((err = n_exdata_req(mtp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_hbeat_req: - send a BEAT message
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sid: SCTP stream identifier
 * @hbt_ptr: heartbeat info pointer
 * @hbt_len: heartbeat info length
 */
static inline int
mtp_send_asps_hbeat_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t sid, caddr_t hbt_ptr,
			size_t hbt_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
		mi_timer(mtp->hbeat_tack, 2000);
		/* send ordered on specified stream */
		if (unlikely((err = n_data_req(mtp, q, msg, 0, sid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_hbeat_ack: - send a BEAT message
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sid: SCTP stream identifier
 * @hbt_ptr: heartbeat info pointer
 * @hbt_len: heartbeat info length
 */
static int
mtp_send_asps_hbeat_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t sid, caddr_t hbt_ptr,
			size_t hbt_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
		/* send ordered on specified stream */
		if (unlikely((err = n_data_req(mtp, q, msg, 0, sid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_aspt_aspac_req: - send ASP Active
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tmode: traffic mode
 * @rc: Routing Context
 * @num_rc: number of routing contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
static int
mtp_send_aspt_aspac_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t tmode, uint32_t *rc,
			uint32_t num_rc, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_rc ? UA_PHDR_SIZE +
	    num_rc * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_REQ;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_TMODE;
		p[3] = htonl(tmode);
		p += 4;
		if (num_rc) {
			uint32_t *ip = rc;

			*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
			while (num_rc--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		mtp_set_u_state(mtp, ASP_WACK_ASPAC);
		mi_timer(mtp->aspac_tack, 2000);
		/* always sent on same stream as data */
		if (unlikely((err = n_data_req(mtp, q, msg, 0, rc ? *rc : 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_aspt_aspia_req: - send ASP Inactive
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @rc: Routing Context
 * @num_rc: number of routing contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
static int
mtp_send_aspt_aspia_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t *rc, uint32_t num_rc,
			caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + num_rc ? UA_PHDR_SIZE + num_rc * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (num_rc) {
			uint32_t *ip = rc;

			*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
			while (num_rc--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		mtp_set_u_state(mtp, ASP_WACK_ASPIA);
		mi_timer(mtp->aspia_tack, 2000);
		if (unlikely((err = n_data_req(mtp, q, msg, 0, rc ? *rc : 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * mtp_send_rkmm_reg_req: - send REG REQ
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @id: request identifier
 * @sdti: Signalling Data Terminal Identifier
 * @sdli: Signalling Data Link Identifier
 */
static int
mtp_send_rkmm_reg_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t id, uint32_t sdti,
		      uint32_t sdli)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_REG_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = M2UA_PARM_LINK_KEY;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl(id);
		p[5] = M2UA_PARM_SDTI;
		p[6] = htonl(sdti);
		p[7] = M2UA_PARM_SDLI;
		p[8] = htonl(sdli);
		p += 9;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = n_exdata_req(mtp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
#endif

/**
 * mtp_send_xfer_data: - send XFER DATA
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @rc: routing context
 * @orig: originating address
 * @dest: destination address
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static inline fastcall __hot_write int
mtp_send_xfer_data(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t rc, struct mtp_addr *orig,
		   struct mtp_addr *dest, uint8_t pri, uint8_t sls, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp) + 3 * sizeof(uint32_t);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC) + UA_PHDR_SIZE;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_XFER_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_RC;
		p[3] = htonl(rc);
		p[4] = UA_PHDR(M3UA_PARM_PROT_DATA3, dlen);
		p[5] = htonl(orig->pc);
		p[6] = htonl(dest->pc);
		p[7] = htonl(((uint32_t) dest->si << 24) | ((uint32_t) dest->ni << 16) |
			     ((uint32_t) pri << 8) | ((uint32_t) sls << 0));
		mp->b_wptr = (unsigned char *) &p[8];
		mp->b_cont = dp;
		if (unlikely((err = n_data_req(mtp, q, msg, 0, rc, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  MTP SG -> MTP AS (M3UA) Received Messages
 *
 *  --------------------------------------------------------------------------
 */

/**
 * mtp_recv_mgmt_err: - process MGMT ERR message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_mgmt_err(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	/* FIXME: check the current state, if we are in a WACK state then we probably have to deal
	   with the error.  If we are not in a WACK state, the error might be able to be ignored,
	   but also might require bringing the ASP down. */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_mgmt_ntfy: - process MGMT NTFY message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_mgmt_ntfy(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	/* FIXME: Mostly to do with traffic modes.  If we are in override and we came up as a
	   standby, we might now be active. */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_asps_hbeat_req: - process ASPS BEAT Req message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_asps_hbeat_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	caddr_t hptr = NULL;		/* pointer to heartbeat data */
	size_t hlen = 0;		/* length of heartbeat data */
	int sid = 0;			/* stream id for ack message */

	/* FIXME: need to dig HBEAT DATA out of message and possibly derive the stream id from the
	   IID. */

	return mtp_send_asps_hbeat_ack(mtp, q, mp, sid, hptr, hlen);
}

/**
 * mtp_recv_asps_aspup_ack: - process ASPS ASPUP Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_asps_aspup_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	if (mtp_get_u_state(mtp) != ASP_WACK_ASPUP)
		goto outstate;
	mi_timer_cancel(mtp->aspup_tack);
	mtp_set_u_state(mtp, ASP_INACTIVE);
	switch (mtp_get_i_state(mtp)) {
	case MTPS_WACK_BREQ:
		mtp_set_i_state(mtp, MTPS_IDLE);
		return mtp_bind_ack(mtp, q, mp, (caddr_t) &mtp->orig, sizeof(mtp->orig));
	case MTPS_WCON_CREQ:
	{
		uint32_t *rc;
		int rcnum;

		/* one more step */
		rc = (mtp->rc ? &mtp->rc : NULL);
		rcnum = rc ? 1 : 0;
		mtp_set_u_state(mtp, ASP_WACK_ASPAC);
		return mtp_send_aspt_aspac_req(mtp, q, mp, mtp->tm, rc, rcnum, NULL, 0);
	}
	case MTPS_UNBND:
	case MTPS_IDLE:
	case MTPS_WACK_CREQ:
	case MTPS_CONNECTED:
	case MTPS_WACK_UREQ:
	case MTPS_WACK_DREQ6:
	case MTPS_WACK_DREQ9:
	case MTPS_WACK_OPTREQ:
	case MTPS_WREQ_ORDREL:
	case MTPS_WIND_ORDREL:
	case MTPS_WRES_CIND:
	case MTPS_UNUSABLE:
	default:
		break;
	}
      outstate:
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_asps_aspdn_ack: - process ASPS ASPDN Ack ERR message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_asps_aspdn_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_asps_hbeat_ack: - process ASPS BEAT Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_asps_hbeat_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_aspt_aspac_ack: - process ASPT ASPAC Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_aspt_aspac_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_aspt_aspia_ack: - process ASPT ASPIA Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_aspt_aspia_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_rkmm_reg_rsp: - process RKMM REG Resp message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_rkmm_reg_rsp(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	/* ignore for now */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_rkmm_dereg_rsp: - process RKMM DEREG Resp message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_rkmm_dereg_rsp(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	/* ignore for now */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_xfer_data: - process XFER DATA message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_xfer_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct ua_parm data;
	struct mtp_addr orig, dest;
	mtp_ulong pri, sls;
	mblk_t *dp;
	int err;

	if (!(dp = dupmsg(mp)))
		goto nobufs;

	if (!ua_dec_parm(mtp, q, dp, &data, M3UA_PARM_PROT_DATA3))
		goto missing;

	orig.pc = htonl(data.wp[1]);
	if (mtp->orig.pc && mtp->orig.pc != orig.pc)
		goto invalid;

	dest.pc = htonl(data.wp[2]);
	dest.si = (htonl(data.wp[3]) >> 24) & 0x00ff;
	dest.ni = (htonl(data.wp[3]) >> 16) & 0x00ff;

	pri = (htonl(data.wp[3]) >> 8) & 0x00ff;
	sls = (htonl(data.wp[3]) >> 0) & 0x00ff;

	dp->b_rptr = (unsigned char *) &data.wp[4];

	/* trim tail to just the data */
	adjmsg(dp, (data.len - 4 * sizeof(uint32_t)) - msgsize(dp));

	err = mtp_transfer_ind(mtp, q, mp, (caddr_t) &dest, sizeof(dest), pri, sls, dp);
	if (err)
		freemsg(dp);
	return (err);
      missing:
	freemsg(dp);
	return (-ENXIO);	/* missing mandatory parameter */
      invalid:
	freemsg(dp);
	return (-EINVAL);	/* invalid parameter */
      nobufs:
	mi_bufcall(q, msgsize(mp), BPRI_MED);
	return (-ENOBUFS);
}

/**
 * mtp_recv_snmm_duna: - process SNMM DUNA message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_duna(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;

	addr.ni = mtp->orig.ni;
	addr.si = mtp->orig.si;
	addr.pc = apc.val;

	return mtp_pause_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr));
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
}

/**
 * mtp_recv_snmm_dava: - process SNMM DAVA message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_dava(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;

	addr.ni = mtp->orig.ni;
	addr.si = mtp->orig.si;
	addr.pc = apc.val;

	return mtp_resume_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr));
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
}

/**
 * mtp_recv_snmm_scon: - process SNMM SCON message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_scon(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc, cong;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;
	if (!ua_dec_parm(mtp, q, mp, &cong, M3UA_PARM_CONG_IND))
		cong.val = MTP_STATUS_CONGESTION;

	addr.ni = mtp->orig.ni;
	addr.si = mtp->orig.si;
	addr.pc = apc.val;

	switch (cong.val) {
	case 0:
	case 1:
	case 2:
	case 3:
		break;
	default:
		goto invalid;
	}

	return mtp_status_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr),
			      MTP_STATUS_TYPE_CONG, cong.val);
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
      invalid:
	return (-EINVAL);	/* invalid parameter */
}

/**
 * mtp_recv_snmm_dupu: - process SNMM DUPU message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_dupu(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc, uc;
	mtp_ulong user, cause;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;
	if (!ua_dec_parm(mtp, q, mp, &uc, M3UA_PARM_USER_CAUSE))
		goto missing;

	user = uc.val & 0x0000ffff;
	if (mtp->orig.si && user != mtp->orig.si)
		goto invalid;

	cause = (uc.val >> 16) & 0x0000ffff;
	switch (cause) {
	case 0:
	case 1:
	case 2:
		break;
	default:
		goto invalid;
	}

	addr.ni = mtp->orig.ni;
	addr.si = user;
	addr.pc = apc.val;

	return mtp_status_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr),
			      MTP_STATUS_TYPE_UPU, cause);
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
      invalid:
	return (-EINVAL);	/* invalid parameter */
}

/**
 * mtp_recv_snmm_drst: - process SNMM DRST message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_drst(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;

	addr.ni = mtp->orig.ni;
	addr.si = mtp->orig.si;
	addr.pc = apc.val;

	return mtp_status_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr), MTP_STATUS_TYPE_RSTR, 0);
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
}

/**
 * mtp_recv_err: - process error for received message
 * @mtp: private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 * @err: error number
 */
static noinline fastcall int
mtp_recv_err(struct mtp *mtp, queue_t *q, mblk_t *mp, int err)
{
	switch (err) {
	case 0:
	case -EBUSY:
	case -ENOMEM:
	case -ENOBUFS:
	case -ENOSR:
	case -EAGAIN:
	case -EDEADLK:
		break;
	case -EINVAL:		/* unexpected message */
		err = UA_ECODE_UNEXPECTED_MESSAGE;
		goto error;
	case -EMSGSIZE:	/* syntax error */
		err = UA_ECODE_PARAMETER_FIELD_ERROR;
		goto error;
	case -EOPNOTSUPP:	/* unrecongized message type */
		err = UA_ECODE_UNSUPPORTED_MESSAGE_TYPE;
		goto error;
	case -ENOPROTOOPT:	/* unrecognized message class */
		err = UA_ECODE_UNSUPPORTED_MESSAGE_CLASS;
		goto error;
	case -EPROTO:		/* protocol error */
		err = UA_ECODE_PROTOCOL_ERROR;
		goto error;
	default:
		if (err < 0)
			err = UA_ECODE_PROTOCOL_ERROR;
	      error:
		return mtp_send_mgmt_err(mtp, q, mp, err, mp->b_rptr, mp->b_wptr - mp->b_rptr);
	}
	return (err);
}

/**
 * mtp_recv_msg_slow: - process message received from M3UA peer (SG)
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (just the M3UA part)
 */
static noinline fastcall int
mtp_recv_msg_slow(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err = -EMSGSIZE;

	if (mp->b_wptr < mp->b_rptr + 2 * sizeof(*p))
		goto error;
	if (mp->b_wptr < mp->b_rptr + ntohl(p[1]))
		goto error;
	switch (UA_MSG_CLAS(p[0])) {
	case UA_CLASS_MGMT:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_MGMT_ERR:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "ERR <-");
			err = mtp_recv_mgmt_err(mtp, q, mp);
			break;
		case UA_MGMT_NTFY:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "NTFY <-");
			err = mtp_recv_mgmt_ntfy(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_ASPS:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPS_HBEAT_REQ:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "BEAT <-");
			err = mtp_recv_asps_hbeat_req(mtp, q, mp);
			break;
		case UA_ASPS_ASPUP_ACK:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "ASPUP Ack <-");
			err = mtp_recv_asps_aspup_ack(mtp, q, mp);
			break;
		case UA_ASPS_ASPDN_ACK:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "ASPDN Ack <-");
			err = mtp_recv_asps_aspdn_ack(mtp, q, mp);
			break;
		case UA_ASPS_HBEAT_ACK:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "BEAT Ack <-");
			err = mtp_recv_asps_hbeat_ack(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_ASPT:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPT_ASPAC_ACK:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "ASPAC Ack <-");
			err = mtp_recv_aspt_aspac_ack(mtp, q, mp);
			break;
		case UA_ASPT_ASPIA_ACK:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "ASPIA Ack <-");
			err = mtp_recv_aspt_aspia_ack(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_RKMM:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_RKMM_REG_RSP:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "REG Rsp <-");
			err = mtp_recv_rkmm_reg_rsp(mtp, q, mp);
			break;
		case UA_RKMM_DEREG_RSP:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "DEREG Rsp <-");
			err = mtp_recv_rkmm_dereg_rsp(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_XFER:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_XFER_DATA:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "DATA <-");
			err = mtp_recv_xfer_data(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_SNMM:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_SNMM_DUNA:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "DUNA <-");
			err = mtp_recv_snmm_duna(mtp, q, mp);
			break;
		case UA_SNMM_DAVA:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "DAVA <-");
			err = mtp_recv_snmm_dava(mtp, q, mp);
			break;
		case UA_SNMM_SCON:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "SCON <-");
			err = mtp_recv_snmm_scon(mtp, q, mp);
			break;
		case UA_SNMM_DUPU:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "DUPU <-");
			err = mtp_recv_snmm_dupu(mtp, q, mp);
			break;
		case UA_SNMM_DRST:
			strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "DRST <-");
			err = mtp_recv_snmm_drst(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	default:
		err = (-ENOPROTOOPT);
		break;
	}
	if (err == 0)
		return (0);
      error:
	return mtp_recv_err(mtp, q, mp, err);
}

/**
 * mtp_recv_msg: - process message received from M3UA peer (SG)
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (just the M3UA part)
 */
static inline fastcall int
mtp_recv_msg(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* fast path for data */
	if (mp->b_wptr >= mp->b_rptr + 2 * sizeof(*p))
		if (mp->b_wptr >= mp->b_rptr + ntohl(p[1]))
			if (UA_MSG_CLAS(p[0]) == UA_CLASS_XFER)
				if (UA_MSG_TYPE(p[0]) == UA_XFER_DATA) {
					if ((err = mtp_recv_xfer_data(mtp, q, mp)) == 0)
						return (0);
					return mtp_recv_err(mtp, q, mp, err);
				}
	return mtp_recv_msg_slow(mtp, q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP User -> MTP Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * mtp_bind_req: - process MTP_BIND_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
mtp_bind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_bind_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return mtp_error_ack(mtp, q, mp, MTP_BIND_REQ, MBADPRIM);
}

/**
 * mtp_unbind_req: - process MTP_UNBIND_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
mtp_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_unbind_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return mtp_error_ack(mtp, q, mp, MTP_UNBIND_REQ, MBADPRIM);
}

/**
 * mtp_conn_req: - process MTP_CONN_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
mtp_conn_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_conn_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return mtp_error_ack(mtp, q, mp, MTP_CONN_REQ, MBADPRIM);
}

/**
 * mtp_discon_req: - process MTP_DISCON_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
mtp_discon_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_discon_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return mtp_error_ack(mtp, q, mp, MTP_DISCON_REQ, MBADPRIM);
}

/**
 * mtp_addr_req: - process MTP_ADDR_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
mtp_addr_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_addr_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return mtp_error_ack(mtp, q, mp, MTP_ADDR_REQ, MBADPRIM);
}

/**
 * mtp_info_req: - process MTP_INFO_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
mtp_info_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_info_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return mtp_error_ack(mtp, q, mp, MTP_INFO_REQ, MBADPRIM);
}

/**
 * mtp_optmgmt_req: - process MTP_OPTMGMT_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
mtp_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_optmgmt_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return mtp_error_ack(mtp, q, mp, MTP_OPTMGMT_REQ, MBADPRIM);
}

/**
 * mtp_transfer_req: - process MTP_TRANSFER_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
mtp_transfer_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_transfer_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr dst, *orig = &mtp->orig, *dest;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (p->mtp_dest_length == 0) {
		dest = &mtp->dest;
	} else if (p->mtp_dest_length != sizeof(*dest))
		goto badaddr;
	else {
		dest = &dst;
		bcopy(mp->b_rptr + p->mtp_dest_offset, dest, p->mtp_dest_length);
	}

	err = mtp_send_xfer_data(mtp, q, mp, mtp->rc, orig, dest, p->mtp_mp, p->mtp_sls,
				 mp->b_cont);
	if (err == 0)
		freeb(mp);
	return (err);
      badaddr:
	return mtp_error_ack(mtp, q, mp, MTP_TRANSFER_REQ, MBADADDR);
      tooshort:
	return mtp_error_ack(mtp, q, mp, MTP_TRANSFER_REQ, MBADPRIM);
}

static int
mtp_other_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timeouts
 *
 *  -------------------------------------------------------------------------
 */
/**
 * mtp_aspup_tack_timeout: - timeout awaiting ASP Up Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_aspup_tack_timeout(struct mtp *mtp, queue_t *q)
{
	switch (mtp_get_u_state(mtp)) {
	case ASP_WACK_ASPUP:
	{
		uint32_t *aspid;

		aspid = mtp->aspid ? &mtp->aspid : NULL;
		return mtp_send_asps_aspup_req(mtp, q, NULL, aspid, NULL, 0);
	}
	default:
		break;
	}
	return (0);
}

/**
 * mtp_aspac_tack_timeout: - timeout awaiting ASP Active Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_aspac_tack_timeout(struct mtp *mtp, queue_t *q)
{
	switch (mtp_get_u_state(mtp)) {
	case ASP_WACK_ASPAC:
	{
		uint32_t *rc;

		rc = mtp->rc ? &mtp->rc : NULL;
		return mtp_send_aspt_aspac_req(mtp, q, NULL, mtp->tm, rc, 1, NULL, 0);
	}
	default:
		break;
	}
	return (0);
}

/**
 * mtp_aspdn_tack_timeout: - timeout awaiting ASP Down Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_aspdn_tack_timeout(struct mtp *mtp, queue_t *q)
{
	switch (mtp_get_u_state(mtp)) {
	case ASP_WACK_ASPDN:
	{
		uint32_t *aspid;

		aspid = mtp->aspid ? &mtp->aspid : NULL;
		return mtp_send_asps_aspdn_req(mtp, q, NULL, aspid, NULL, 0);
	}
	default:
		break;
	}
	return (0);
}

/**
 * mtp_aspia_tack_timeout: - timeout awaiting ASP Inactive Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_aspia_tack_timeout(struct mtp *mtp, queue_t *q)
{
	switch (mtp_get_u_state(mtp)) {
	case ASP_WACK_ASPIA:
	{
		uint32_t *rc;

		rc = mtp->rc ? &mtp->rc : NULL;
		return mtp_send_aspt_aspia_req(mtp, q, NULL, rc, 1, NULL, 0);
	}
	default:
		break;
	}
	return (0);
}

/**
 * mtp_hbeat_tack_timeout: - timeout awaiting Heartbeat Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_hbeat_tack_timeout(struct mtp *mtp, queue_t *q)
{
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  N Provider -> N User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * n_conn_ind: - process N_CONN_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * If we get a connection indication, we pretty much have to remember it.
 * Another possibility is issuing an LMI_ENABLE_IND upstream.
 */
static int
n_conn_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_conn_con: - process N_CONN_CON primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * When we get a valid connection confirmation it is a confirmation of our
 * attempt to connect that was part of the LMI_ENABLE_REQ operation.  Respond
 * with an LMI_OK_ACK and LMI_ENABLE_CON primitive.
 */
static int
n_conn_con(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (mtp_get_n_state(mtp) != NS_WCON_CREQ)
		goto outstate;
	mtp_set_n_state(mtp, NS_DATA_XFER);
#if 0
	if (mtp_get_i_state(mtp) != LMI_ENABLE_PENDING)
		goto outstate;
	mtp_set_i_state(mtp, LMI_ENABLED);
	return lmi_enable_con(mtp, q, mp);
#endif
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_discon_ind: - process N_DISCON_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * A disconnect indication corresponds to a disable indication.
 */
static int
n_discon_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (mtp_get_n_state(mtp) != NS_DATA_XFER)
		goto outstate;
#if 0
	switch (mtp_get_i_state(mtp)) {
	case LMI_ENABLED:
	case LMI_ENABLE_PENDING:
		mtp_set_i_state(mtp, LMI_DISABLED);
		mtp_set_n_state(mtp, NS_IDLE);
		return lmi_error_ind(mtp, q, mp, 0, true);
	}
#endif
	goto outstate;
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_data_ind: - process N_DATA_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static inline fastcall __hot_in int
n_data_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if ((err = mtp_recv_msg(mtp, q, dp)) == 0)
		freeb(mp);
	return (err);
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_exdata_ind: - process N_EXDATA_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_exdata_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if ((err = mtp_recv_msg(mtp, q, dp)) == 0)
		freeb(mp);
	return (err);
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_info_ack: - process N_INFO_ACK primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_info_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_bind_ack: - process N_BIND_ACK primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * An N_BIND_ACK is returned as a result of an LMI_ATTACH_REQ operation.
 */
static int
n_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (mtp_get_n_state(mtp) != NS_WACK_BREQ)
		goto outstate;
	mtp_set_n_state(mtp, NS_IDLE);
#if 0
	if (mtp_get_i_state(mtp) != LMI_ATTACH_PENDING)
		goto outstate;
	mtp_set_i_state(mtp, LMI_DISABLED);
	return lmi_ok_ack(mtp, q, mp, LMI_ATTACH_REQ);
#endif
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_error_ack: - process N_ERROR_ACK primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_error_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (mtp_get_n_state(mtp)) {
	case NS_WACK_BREQ:
#if 0
		mtp_set_n_state(mtp, NS_UNBND);
		if (mtp_get_i_state(mtp) != LMI_ATTACH_PENDING)
			goto outstate;
		mtp_set_i_state(mtp, LMI_UNATTACHED);
		return lmi_error_ack(mtp, q, mp, LMI_ATTACH_REQ, LMI_BADPPA);
#endif
	case NS_WACK_UREQ:
#if 0
		mtp_set_n_state(mtp, NS_IDLE);
		if (mtp_get_i_state(mtp) != LMI_DETACH_PENDING)
			goto outstate;
		mtp_set_i_state(mtp, LMI_DISABLED);
		return lmi_error_ack(mtp, q, mp, LMI_DETACH_REQ, LMI_UNSPEC);
#endif
	case NS_WCON_CREQ:
#if 0
		mtp_set_n_state(mtp, NS_IDLE);
		if (mtp_get_i_state(mtp) != LMI_ENABLE_PENDING)
			goto outstate;
		mtp_set_i_state(mtp, LMI_DISABLED);
		return lmi_error_ack(mtp, q, mp, LMI_ENABLE_REQ, LMI_UNSPEC);
#endif
	case NS_WACK_DREQ6:
		mtp_set_n_state(mtp, NS_WCON_CREQ);
		goto disconn;
	case NS_WACK_DREQ7:
		mtp_set_n_state(mtp, NS_WRES_CIND);
		goto disconn;
	case NS_WACK_DREQ9:
		mtp_set_n_state(mtp, NS_DATA_XFER);
		goto disconn;
	case NS_WACK_DREQ10:
		mtp_set_n_state(mtp, NS_WCON_RREQ);
		goto disconn;
	case NS_WACK_DREQ11:
		mtp_set_n_state(mtp, NS_WRES_RIND);
		goto disconn;
	      disconn:
#if 0
		if (mtp_get_i_state(mtp) != LMI_DISABLE_PENDING)
			goto outstate;
		mtp_set_i_state(mtp, LMI_ENABLED);
		return lmi_error_ack(mtp, q, mp, LMI_DISABLE_REQ, LMI_UNSPEC);
#endif
		break;
	}
	goto outstate;
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_ok_ack: - process N_OK_ACK primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (mtp_get_n_state(mtp)) {
	case NS_WACK_UREQ:
		mtp_set_n_state(mtp, NS_UNBND);
#if 0
		if (mtp_get_i_state(mtp) != LMI_DETACH_PENDING)
			goto outstate;
		mtp_set_i_state(mtp, LMI_UNATTACHED);
		return lmi_ok_ack(mtp, q, mp, LMI_DETACH_REQ);
#endif
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		mtp_set_n_state(mtp, NS_IDLE);
#if 0
		if (mtp_get_i_state(mtp) != LMI_DISABLE_PENDING)
			goto outstate;
		mtp_set_i_state(mtp, LMI_DISABLED);
		return lmi_ok_ack(mtp, q, mp, LMI_DISABLE_REQ);
#endif
	}
	goto outstate;
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_unitdata_ind: - process N_UNITDATA_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_unitdata_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_uderror_ind: - process N_UDERROR_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_uderror_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_datack_ind: - process N_DATACK_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_datack_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_reset_ind: - process N_RESET_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_reset_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_reset_con: - process N_RESET_CON primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_reset_con(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_other_ind: - process unknown primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_other_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP User -> MTP Provider Message Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
mtp_w_proto(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int old_i_state, old_n_state, old_u_state;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t))
		return mtp_error_ack(mtp, q, mp, -1, MBADPRIM);

	if (!mtp_trylock(mtp, q))
		return (-EDEADLK);

	old_i_state = mtp_get_i_state(mtp);
	old_n_state = mtp_get_n_state(mtp);
	old_u_state = mtp_get_u_state(mtp);

	switch (*(uint32_t *) mp->b_rptr) {
	case MTP_BIND_REQ:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_BIND_REQ");
		rtn = mtp_bind_req(mtp, q, mp);
		break;
	case MTP_UNBIND_REQ:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_UNBIND_REQ");
		rtn = mtp_unbind_req(mtp, q, mp);
		break;
	case MTP_CONN_REQ:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_CONN_REQ");
		rtn = mtp_conn_req(mtp, q, mp);
		break;
	case MTP_DISCON_REQ:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_DISCON_REQ");
		rtn = mtp_discon_req(mtp, q, mp);
		break;
	case MTP_ADDR_REQ:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_ADDR_REQ");
		rtn = mtp_addr_req(mtp, q, mp);
		break;
	case MTP_INFO_REQ:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_INFO_REQ");
		rtn = mtp_info_req(mtp, q, mp);
		break;
	case MTP_OPTMGMT_REQ:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_OPTMGMT_REQ");
		rtn = mtp_optmgmt_req(mtp, q, mp);
		break;
	case MTP_TRANSFER_REQ:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_TRANSFER_REQ");
		rtn = mtp_transfer_req(mtp, q, mp);
		break;
	default:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "-> MTP_????_???");
		rtn = mtp_other_req(mtp, q, mp);
		break;
	}
	if (rtn < 0) {
		mtp_set_i_state(mtp, old_i_state);
		mtp_set_n_state(mtp, old_n_state);
		mtp_set_u_state(mtp, old_u_state);
	}
	mtp_unlock(mtp);
	return (rtn);
}
static int
mtp_w_flush(queue_t *q, mblk_t *mp)
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
static int
mtp_w_ioctl(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static int
mtp_w_iocdata(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static int
mtp_w_other(queue_t *q, mblk_t *mp)
{
	/* pass unprocessed message types along */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
mtp_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_FLUSH:
		return mtp_w_flush(q, mp);
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
	case M_IOCDATA:
		return mtp_w_iocdata(q, mp);
	default:
		return mtp_w_other(q, mp);
	}
}
static inline fastcall int
mtp_w_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_PROTO)
		if (mp->b_wptr >= mp->b_rptr + sizeof(uint32_t))
			if (*(uint32_t *) mp->b_rptr == MTP_TRANSFER_REQ)
				return mtp_transfer_req(MTP_PRIV(q), q, mp);
	return mtp_w_msg_slow(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NS Provider -> NS User Message Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
mtp_r_proto(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int old_i_state, old_n_state, old_u_state;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t)) {
		freemsg(mp);
		return (0);
	}
	if (!mtp_trylock(mtp, q))
		return (-EDEADLK);

	old_i_state = mtp_get_i_state(mtp);
	old_n_state = mtp_get_n_state(mtp);
	old_u_state = mtp_get_u_state(mtp);

	switch (*(uint32_t *) mp->b_rptr) {
	case N_CONN_IND:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_CONN_IND <-");
		rtn = n_conn_ind(mtp, q, mp);
		break;
	case N_CONN_CON:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_CONN_CON <-");
		rtn = n_conn_con(mtp, q, mp);
		break;
	case N_DISCON_IND:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_DISCON_IND <-");
		rtn = n_discon_ind(mtp, q, mp);
		break;
	case N_DATA_IND:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_DATA_IND <-");
		rtn = n_data_ind(mtp, q, mp);
		break;
	case N_EXDATA_IND:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_EXDATA_IND <-");
		rtn = n_exdata_ind(mtp, q, mp);
		break;
	case N_INFO_ACK:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_INFO_ACK <-");
		rtn = n_info_ack(mtp, q, mp);
		break;
	case N_BIND_ACK:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_BIND_ACK <-");
		rtn = n_bind_ack(mtp, q, mp);
		break;
	case N_ERROR_ACK:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_ERROR_ACK <-");
		rtn = n_error_ack(mtp, q, mp);
		break;
	case N_OK_ACK:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_OK_ACK <-");
		rtn = n_ok_ack(mtp, q, mp);
		break;
	case N_UNITDATA_IND:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_UNITDATA_IND <-");
		rtn = n_unitdata_ind(mtp, q, mp);
		break;
	case N_UDERROR_IND:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_UDERROR_IND <-");
		rtn = n_uderror_ind(mtp, q, mp);
		break;
	case N_DATACK_IND:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_DATACK_IND <-");
		rtn = n_datack_ind(mtp, q, mp);
		break;
	case N_RESET_IND:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_RESET_IND <-");
		rtn = n_reset_ind(mtp, q, mp);
		break;
	case N_RESET_CON:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_RESET_CON <-");
		rtn = n_reset_con(mtp, q, mp);
		break;
	default:
		strlog(mtp->mid, mtp->sid, MTPLOGRX, SL_TRACE, "N_????_??? <-");
		rtn = n_other_ind(mtp, q, mp);
		break;
	}
	if (rtn < 0) {
		mtp_set_i_state(mtp, old_i_state);
		mtp_set_n_state(mtp, old_n_state);
		mtp_set_u_state(mtp, old_u_state);
	}
	mtp_unlock(mtp);
	return (rtn);
}
static int
mtp_r_flush(queue_t *q, mblk_t *mp)
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
static int
mtp_r_sig(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int rtn;

	if (unlikely(!mi_timer_valid(mp)))
		return (0);

	if (unlikely(!mtp_trylock(mtp, q)))
		return (-EDEADLK);

	switch (*(int *) mp->b_rptr) {
	case 1:
		strlog(mtp->mid, mtp->sid, MTPLOGTO, SL_TRACE, "-> ASP Up TIMEOUT <-");
		rtn = mtp_aspup_tack_timeout(mtp, q);
		break;
	case 2:
		strlog(mtp->mid, mtp->sid, MTPLOGTO, SL_TRACE, "-> ASP Active TIMEOUT <-");
		rtn = mtp_aspac_tack_timeout(mtp, q);
		break;
	case 3:
		strlog(mtp->mid, mtp->sid, MTPLOGTO, SL_TRACE, "-> ASP Down TIMEOUT <-");
		rtn = mtp_aspdn_tack_timeout(mtp, q);
		break;
	case 4:
		strlog(mtp->mid, mtp->sid, MTPLOGTO, SL_TRACE, "-> ASP Inactive TIMEOUT <-");
		rtn = mtp_aspia_tack_timeout(mtp, q);
		break;
	case 5:
		strlog(mtp->mid, mtp->sid, MTPLOGTO, SL_TRACE, "-> Heartbeat TIMEOUT <-");
		rtn = mtp_hbeat_tack_timeout(mtp, q);
		break;
	default:
		strlog(mtp->mid, mtp->sid, MTPLOGTO, SL_TRACE, "-> ??? TIMEOUT <-");
		rtn = 0;
		break;
	}
	mtp_unlock(mtp);
	return (rtn);
}
static int
mtp_r_other(queue_t *q, mblk_t *mp)
{
	/* pass unprocessed message types along */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
mtp_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return mtp_r_proto(q, mp);
	case M_FLUSH:
		return mtp_r_flush(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mtp_r_sig(q, mp);
	default:
		return mtp_r_other(q, mp);
	}
}
static inline fastcall int
mtp_r_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_PROTO)
		if (mp->b_wptr >= mp->b_rptr + sizeof(uint32_t))
			if (*(uint32_t *) mp->b_rptr == N_DATA_IND)
				return n_data_ind(MTP_PRIV(q), q, mp);
	return mtp_r_msg_slow(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Put and Service Procedures
 *
 *  -------------------------------------------------------------------------
 */
static streamscall __hot_in int
mtp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mtp_r_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_read int
mtp_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mtp_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_write int
mtp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mtp_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
mtp_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mtp_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Open and Close Routines
 *
 *  -------------------------------------------------------------------------
 */
static caddr_t mtp_opens = NULL;

static streamscall __unlikely int
mtp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct mtp *mtp;
	mblk_t *mp, *tp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */

	while (!(mp = allocb(sizeof(N_info_req_t), BPRI_WAITOK))) ;

	if ((err = mi_open_comm(&mtp_opens, sizeof(*mtp), q, devp, oflags, sflag, crp)))
		return (err);

	mtp = MTP_PRIV(q);
	/* initialize the structure */

	/* allocate timers */
	if (!(tp = mtp->aspup_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 1;
	if (!(tp = mtp->aspac_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 2;
	if (!(tp = mtp->aspdn_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 3;
	if (!(tp = mtp->aspia_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 4;
	if (!(tp = mtp->hbeat_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 5;

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((N_info_req_t *) mp->b_wptr)->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(N_info_req_t);
	qprocson(q);
	putnext(q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto error;
      error:
	freemsg(mp);
	/* cancel all timers */
	if ((tp = xchg(&mtp->aspup_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspdn_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspac_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspia_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->hbeat_tack, NULL)))
		mi_timer_free(tp);
	mi_close_comm(&mtp_opens, q);
	return (err);
}

static streamscall __unlikely int
mtp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct mtp *mtp = MTP_PRIV(q);
	mblk_t *tp;

	qprocsoff(q);

	/* cancel all timers */
	if ((tp = xchg(&mtp->aspup_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspdn_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspac_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspia_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->hbeat_tack, NULL)))
		mi_timer_free(tp);

	mi_close_comm(&mtp_opens, q);
	return (0);
}

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the M3UA-AS module. (0 for allocation.)");

static struct qinit mtp_rinit = {
	.qi_putp = mtp_rput,		/* Read put (message from below) */
	.qi_srvp = mtp_rsrv,		/* Read queue service */
	.qi_qclose = mtp_qclose,	/* Each open */
	.qi_qopen = mtp_qopen,		/* Last close */
	.qi_minfo = &mtp_minfo,		/* Information */
	.qi_mstat = &mtp_mstat,		/* Statistics */
};

static struct qinit mtp_winit = {
	.qi_putp = mtp_wput,		/* Write put (message from above) */
	.qi_srvp = mtp_wsrv,		/* Write queue service */
	.qi_minfo = &mtp_minfo,		/* Information */
	.qi_mstat = &mtp_mstat,		/* Statistics */
};

static struct streamtab m3ua_asinfo = {
	.st_rdinit = &mtp_rinit,	/* Upper read queue */
	.st_wrinit = &mtp_winit,	/* Upper write queue */
};

#ifdef LIS
#define fmodsw _fmodsw
#endif

static struct fmodsw m3_fmod = {
	.f_name = MOD_NAME,
	.f_str = &m3ua_asinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
m3ua_asinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_SPLASH);
	if ((err = register_strmod(&m3_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module id %d\n", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
m3ua_asexit(void)
{
	int err;

	if ((err = unregister_strmod(&m3_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d\n", MOD_NAME, err);
	return;
}

module_init(m3ua_asinit);
module_exit(m3ua_asexit);
