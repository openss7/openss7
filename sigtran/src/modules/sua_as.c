/*****************************************************************************

 @(#) $RCSfile: sua_as.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/07/14 01:33:46 $

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

 Last Modified $Date: 2007/07/14 01:33:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sua_as.c,v $
 Revision 0.9.2.4  2007/07/14 01:33:46  brian
 - make license explicit, add documentation

 Revision 0.9.2.3  2007/03/25 18:59:08  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.2  2006/12/23 13:06:56  brian
 - manual page and other package updates for release

 Revision 0.9.2.1  2006/11/30 13:17:57  brian
 - added files from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: sua_as.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/07/14 01:33:46 $"

static char const ident[] =
    "$RCSfile: sua_as.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/07/14 01:33:46 $";

/*
 *  This is the AS side of SUA implemented as a pushable module that pushes over an SCTP NPI
 *  stream.
 *
 *  The SCTP NPI stream can be a freshly opened stream (i.e. in the NS_UNBND state), or can be bound
 *  by the opener before pushing the SUA-AS module, or can be connected by the opener before
 *  pushing the SUA-AS module.  When the SUA-AS module is pushed, it interrogates the state of the
 *  SCTP NPI stream beneath it and flushes the read queue (so that no messages get past the module).
 *
 *  When an SCCP bind is performed, the SCTP stream will be bound (if it is not already bound).  If
 *  the SCTP stream is already connected, the bind operation initiates ASP Up procedure with the
 *  bound ASPID.
 *
 *  When an SCCP bind is performed, the SCTP stream will be connected (if it is not already
 *  connected).  If the SCTP stream is already connected, and the ASP Up operation has completed, an
 *  ASP Active for the bound RC is initiated.  Because SCCP streams are bound by address
 *  specification, the default behaviour of the AS is to register the address with the peer and
 *  obtain a Routing Context if one is not provided in the address.  An SUA address differs
 *  slightly from that of SCCP in that an NA and RC value are also provided in the address.  Once UP
 *  and registered, the routing context is activated.  All this occurs during the bind operation for
 *  SCCP.  (Note that if the PCLASS is connection-oriented, then an additional SCCP connect
 *  operation is required before a routing key can be registered, a routing context obtained, and
 *  the AS activated.)
 *
 *  Once the interface is enabled, the SUA-AS module audits the state of the SCCP for the bound
 *  address by using the DAUD query.
 *
 *  At this point, SCCP-primitives can be issued by the SCCP User that will be trasnlated into SUA
 *  message for the activated RC.  SUA messages received for the activated RC will be translated
 *  into SCCP-primitives and sent upstream.
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

#ifndef DB_TYPE
#define DB_TYPE(mp) mp->b_datap->db_type
#endif

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sccpi.h>
#include <ss7/sccpi_ioctl.h>

#include <sys/npi.h>
#include <sys/npi_sccp.h>

#defined SPLOGST	1	/* log SCCP state transitions */
#defined SPLOGTO	2	/* log SCCP timeouts */
#defined SPLOGRX	3	/* log SCCP primitives received */
#defined SPLOGTX	4	/* log SCCP primitives issued */
#defined SPLOGTE	5	/* log SCCP timeout events */
#defined SPLOGDA	6	/* log SCCP data */

/* ======================= */

#define SUA_AS_DESCRIP		"SUA/SCTP SIGNALLING CONNECTION CONTROL PART (SCCP) STREAMS MODULE."
#define SUA_AS_REVISION		"OpenSS7 $RCSfile: sua_as.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/07/14 01:33:46 $"
#define SUA_AS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SUA_AS_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define SUA_AS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SUA_AS_LICENSE		"GPL v2"
#define SUA_AS_BANNER		SUA_AS_DESCRIP		"\n" \
				SUA_AS_REVISION		"\n" \
				SUA_AS_COPYRIGHT	"\n" \
				SUA_AS_DEVICE		"\n" \
				SUA_AS_CONTACT		"\n"
#define SUA_AS_SPLASH		SUA_AS_DEVICE		" - " \
				SUA_AS_REVISION		"\n"

#ifdef LINUX
MODULE_AUTHOR(SUA_AS_CONTACT);
MODULE_DESCRIPTION(SUA_AS_DESCRIP);
MODULE_SUPPORTED_DEVICE(SUA_AS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SUA_AS_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sua_as");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SUA_AS_MOD_ID		CONFIG_STREAMS_SUA_AS_MODID
#define SUA_AS_MOD_NAME		CONFIG_STREAMS_SUA_AS_NAME
#endif

#define MOD_ID	    SUA_AS_MOD_ID
#define MOD_NAME    SUA_AS_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH  SUA_AS_BANNER
#else				/* MODULE */
#define MOD_SPLASH  SUA_AS_SPLASH
#endif				/* MODULE */

static struct module_info sp_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat sp_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

/*
 *  SUA-AS Private Structure
 */
struct sp {
	STR_DECLARATION (struct sp);	/* stream declaration */
	int mid;			/* module id */
	int sid;			/* stream id */
	struct task_struct *owner;
	queue_t *waitq;
	int u_state;
	int n_state;
	uint32_t cong;
	uint32_t disc;
	struct sccp_addr orig;
	struct sccp_addr dest;
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
		struct N_info_ack sccp;	/* SCCP information */
	} info;
	size_t loc_len;			/* local address length */
	size_t rem_len;			/* remote address length */
	struct sockaddr_in loc[8];	/* up to 8 local addresses */
	struct sockaddr_in rem[8];	/* up to 8 remote addresses */
};

#define SP_PRIV(__q) ((struct sp *)(__q)->q_ptr)

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
sp_n_state_name(int state)
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
sp_set_n_state(struct sp *sp, int newstate)
{
	int oldstate = sp->info.n.CURRENT_state;

	strlog(sp->mid, sp->sid, SLOGST, SL_TRACE, "%s <- %s", sp_n_state_name(newstate),
	       sp_n_state_name(oldstate));
	return ((sp->info.n.CURRENT_state = newstate));
}
static inline int
sp_get_n_state(struct sp *sp)
{
	return (sp->info.n.CURRENT_state);
}
static inline int
sp_get_n_statef(struct sp *sp)
{
	return ((1 << sp_get_n_state(sp)));
}
static inline int
sp_chk_n_state(struct sp *sp, int mask)
{
	return (sp_get_n_statef(sp) & mask);
}
static inline int
sp_not_n_state(struct sp *sp, int mask)
{
	return (sp_chk_n_state(sp, ~mask));
}

const char *
sp_i_state_name(int state)
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
sp_set_i_state(struct sp *sp, int newstate)
{
	int oldstate = sp->info.sp.sp_current_state;

	strlog(sp->mid, sp->sid, SLOGST, SL_TRACE, "%s <- %s", sp_i_state_name(newstate),
	       sp_i_state_name(oldstate));
	return ((sp->info.sp.sp_current_state = sp->info.sp.sp_current_state = newstate));
}
static inline int
sp_get_i_state(struct sp *sp)
{
	return sp->info.sp.sp_current_state;
}
static inline int
sp_get_i_statef(struct sp *sp)
{
	return (1 << sp_get_i_state(sp));
}
static inline int
sp_chk_i_state(struct sp *sp, int mask)
{
	return (sp_get_i_statef(sp) & mask);
}
static inline int
sp_not_i_state(struct sp *sp, int mask)
{
	return (sp_chk_i_state(sp, ~mask));
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
sp_u_state_name(int state)
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
sp_set_u_state(struct sp *sp, int newstate)
{
	int oldstate = sp->u_state;

	strlog(sp->mid, sp->sid, SLOGST, SL_TRACE, "%s <- %s", sp_u_state_name(newstate),
	       sp_u_state_name(oldstate));
	return (sp->u_state = newstate);
}
static inline int
sp_get_u_state(struct sp *sp)
{
	return (sp->u_state);
}
static inline int
sp_get_u_statef(struct sp *sp)
{
	return ((1 << sp_get_u_state(sp)));
}
static inline int
sp_chk_u_state(struct sp *sp, int mask)
{
	return (sp_get_u_statef(sp) & mask);
}
static inline int
sp_not_u_state(struct sp *sp, int mask)
{
	return (sp_chk_u_state(sp, ~mask));
}

/*
 *  Buffer allocation
 */
/**
 * sp_allocb: - allocate a buffer reliably
 * @q: active queue
 * @size: size of allocation
 * @priority: priority of allocation
 *
 * If allocation of a message block fails and this procedure returns NULL, the caller should place
 * the invoking message back on queue, q, and await q being enabled when a buffer becomes available.
 */
static inline fastcall mblk_t *
sp_allocb(queue_t *q, size_t size, int priority)
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
 * sp_trylock: - try to lock an MTP queue pair
 * @sp: MTP private structure
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
sp_trylock(struct sp *sp, queue_t *q)
{
	bool rtn = false;
	unsigned long flags;

	spin_lock_irqsave(&sp->lock, flags);
	if (sp->users == 0 && (q->q_flag & QSVCBUSY)) {
		rtn = true;
		sp->users = 1;
		sp->owner = current;
	} else if (sp->users != 0 && sp->owner == current) {
		rtn = true;
		sp->users++;
	} else if (!sp->waitq) {
		sp->waitq = q;
	} else if (sp->waitq != q) {
		qenable(q);
	}
	spin_unlock_irqrestore(&sp->lock, flags);
	return (rtn);
}

/**
 * sp_unlock: - unlock an MTP queue pair
 * @sp: MTP private structure
 */
static inline void
sp_unlock(struct sp *sp)
{
	unsigned long flags;

	spin_lock_irqsave(&sp->lock, flags);
	if (--sp->users == 0 && sp->waitq) {
		qenable(sp->waitq);
		sp->waitq = NULL;
	}
	spin_unlock_irqrestore(&sp->lock, flags);
}

/*
 *  M3UA Message Definitions
 */

#define SUA_PPI	    4

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
ua_dec_parm(struct sp *sp, queue_t *q, mblk_t *mp, struct ua_parm *parm, uint32_t tag)
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
 *  SCCP Provider (SUA) -> SCCP User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * sccp_conn_ind: - issue an N_CONN_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dptr: destination address pointer
 * @dlen: destination address length
 * @sptr: source address pointer
 * @slen: source address length
 * @qptr: quality of service pointer
 * @qlen: quality of service length
 * @flags: connect flags
 */
static int
sccp_conn_ind(struct sp *sp, queue_t *q, mblk_t *msg, np_ulong prim, caddr_t dptr, size_t dlen,
	      caddr_t sptr, size_t slen, caddr_t qptr, size_t qlen, np_ulong flags)
{
	N_conn_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p) + dlen + slen + qlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_IND;
		p->DEST_length = dlen;
		p->DEST_offset = dlen ? sizeof(*p) : 0;
		p->SRC_length = slen;
		p->SRC_offset = slen ? sizeof(*p) + dlen : 0;
		p->CONN_flags = flags;
		p->QOS_length = qlen;
		p->QOS_offset = qlen ? sizeof(*p) + dlen + slen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(qptr, mp->b_wptr, qlen);
		mp->b_wptr += qlen;
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_CONN_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_conn_con: - issue an N_CONN_CON primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @rptr: responding address pointer
 * @rlen: responding address length
 * @qptr: quality of service pointer
 * @qlen: quality of service length
 * @flags: connection flags
 */
static int
sccp_conn_con(struct sp *sp, queue_t *q, mblk_t *msg, caddr_t rptr, size_t rlen, caddr_t qptr,
	      size_t qlen, np_ulong flags)
{
	N_conn_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p) + rlen + qlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_CON;
		p->RES_length = rlen;
		p->RES_offset = rlen ? sizeof(*p) : 0;
		p->CONN_flags = flags;
		p->QOS_length = qlen;
		p->QOS_offset = qlen ? sizeof(*p) + rlen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(rptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		bcopy(qptr, mp->b_wptr, qlen);
		mp->b_wptr += qlen;
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_CONN_CON");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_discon_ind: - issue an N_DISCON_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: disconnect origin
 * @reason: disconnect reason
 * @rptr: responding address pointer
 * @rlen: responding address length
 * @seq: sequence number
 */
static int
sccp_discon_ind(struct sp *sp, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason,
		caddr_t rptr, size_t rlen, np_ulong seq)
{
	N_discon_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p) + rlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DISCON_IND;
		p->DISCON_orig = orig;
		p->DISCON_reason = reason;
		p->RES_length = rlen;
		p->RES_offset = rlen ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
		bcopy(rptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_DISCON_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_data_ind: - issue an N_DATA_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @dp: user data
 */
static int
sccp_data_ind(struct sp *sp, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DATA_IND;
		p->DATA_xfer_flags = flags;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGDA, SL_TRACE, "<- N_DATA_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_exdata_ind: - issue an N_EXDATA_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static int
sccp_exdata_ind(struct sp *sp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_EXDATA_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGDA, SL_TRACE, "<- N_EXDATA_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_info_ack: - issue an N_INFO_ACK primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_info_ack(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = sp->info.sccp;
		p->PRIM_type = N_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_INFO_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_bind_ack: - issue an N_BIND_ACK primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_bind_ack(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_BIND_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_error_ack: - issue an N_ERROR_ACK primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_error_ack(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_ERROR_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_ok_ack: - issue an N_OK_ACK primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
sccp_ok_ack(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_OK_ACK");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_unitdata_ind: - issue an N_UNITDATA_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_unitdata_ind(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNITDATA_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_UNITDATA_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_uderror_ind: - issue an N_UDERROR_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_uderror_ind(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_uderror_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UDERROR_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_UDERROR_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_datack_ind: - issue an N_DATACK_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_datack_ind(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DATACK_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_DATACK_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_reset_ind: - issue an N_RESET_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_reset_ind(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_reset_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_RESET_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_reset_con: - issue an N_RESET_CON primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_reset_con(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_reset_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_RESET_CON");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_notice_ind: - issue an N_NOTICE_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_notice_ind(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_notice_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_NOTICE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_NOTICE_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sccp_inform_ind: - issue an N_INFORM_IND primitive upstream
 * @sp: SCCP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sccp_inform_ind(struct sp *sp, queue_t *q, mblk_t *msg)
{
	N_inform_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sp_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFORM_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sp->mid, sp->sid, SLOGTX, SL_TRACE, "<- N_INFORM_IND");
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}
