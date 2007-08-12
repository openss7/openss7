/*****************************************************************************

 @(#) $RCSfile: clns-ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:24 $

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

 Last Modified $Date: 2007/08/12 15:20:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: clns-ip.c,v $
 Revision 0.9.2.1  2007/08/12 15:20:24  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: clns-ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:24 $"

static char const ident[] = "$RCSfile: clns-ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:24 $";

/*
 *  This is an X.233 CLNP driver.
 *
 *  It supports SNAcP for all manner of subnetworks and point-to-point links, including RFC 1070.
 *
 *  To support automatic reconfiguration, potential DL_CLDLS devices
 */

/*
 *  This is an X.233 CLNS driver for RFC 1070 ISO-IP.
 *
 *  The driver implements X.233 (CLNP) as a pseudo-device driver.  The driver presents an NPI CLNS
 *  service interface to the upper multiplex.  It only provides a CLNS service interface, and only
 *  accepts addresses in the form of RFC 1069/1070.
 *
 *  This driver is implemented as a multiplexing driver.  It supports the linking of multiple
 *  NP-IP driver Streams bound to IP Protocol Number 80 to the lower multiplex.  These must be
 *  provider Streams provided by the np-ip driver from the strxns package.  Each Stream must be
 *  bound before being linked.  Multiple Streams bound to individual local IP addresses, and a
 *  single wildcard IP address bound Stream (DEFAULT_DESTINATION), can be linked.
 *
 *  The Stream can either be temporarily or permanently linked.  When permanently linked, care
 *  should be taken to unlink the Stream as part of the shutdown procedure, otherwise the driver
 *  will refuse to unload.
 *
 *  Routing is performed with a specialized NSAP address (see RFC 1069, RFC 1070) which includes the
 *  IP address.  Binding is performed to a specific or wildcard address (DEFAULT_DESTINATION).
 *  Binding will associate the upper multiplex Stream with a lower multiplex Stream.  Unitdata
 *  messages are sent to specific IP addresses.  Source addresses are chosen by the lower multiplex
 *  Stream (outgoing interface for DEFAULT_DESTINATION NP-IP provider Streams, and bound address for
 *  normal NP-IP provider Streams).
 *
 *  RFC 1070 also supports ES-IS and IS-IS by prefixing a SNAcP header to each outgoing ISO
 *  datagram.  The SNAcP is supposed to keep a cache of "core" IP addresses to which to multicast
 *  ES, all ES, all IS and all messages.  We also support sending of these messages to three
 *  addresses.  We will use 224.0.0.1 (All Systems on this Subnet) for "all" messages, 224.0.0.2
 *  (All Routers on this Subnet) for "all IS", and 224.0.0.3 (Unassigned) for "all ES".  Any IP
 *  address sending or responding to one of these will be added to the SNAcP "core".  Any IP address
 *  from which a message is received will be added to the SNAcP "core".  Any IP address to which a
 *  message is sent will be added to the SNAcP "core".  Supporting the ES-IS and IS-IS protocols
 *  will also also allow X.121 addresses to be used.
 *
 *  For this to work, X.121 addresses must be added to the node and any initial X.121 to IP address
 *  mappings must be created.
 *
 *  This approach is very efficient.  The driver can merely adjust the DEST_offset and DEST_length
 *  in the user provided N_UNITDATA_REQ primitive to create an N_UNITDATA_REQ primitive for the
 *  NP-IP driver and fill out a portion of the headroom in the associated M_DATA with the CLNP
 *  protocol header.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os8/compat.h>

#include <sys/npi.h>

#define CLNS_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CLNS_EXTRA	"Part of the OpenSS7 OSI stack for Linux Fast-STREAMS"
#define CLNS_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define CLNS_REVISION	"OpenSS7 $RCSfile: clns-ip.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:24 $"
#define CLNS_DEVICE	"SVR 4.2 STREAMS CLNS OSI ISO-IP (RFC 1070) Network Provider"
#define CNLS_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CNLS_LICENSE	"GPL"
#define CLNS_BANNER	CLNS_DESCRIP	"\n" \
			CLNS_EXTRA	"\n" \
			CLNS_COPYRIGHT	"\n" \
			CLNS_DEVICE	"\n" \
			CLNS_CONTACT
#define CLNS_SPLASH	CLNS_DESCRIP	"\n" \
			CLNS_REVISION

#ifdef LINUX
MODULE_AUTHOR(CLNS_CONTACT);
MODULE_DESCRIPTION(CLNS_DESCRIP);
MODULE_SUPPORTED_DEVICE(CLNS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CLNS_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-clns-ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define CLNS_IP_DRV_ID		CONFIG_STREAMS_CLNS_IP_MODID
#define CLNS_IP_DRV_NAME	CONFIG_STREAMS_CLNS_IP_NAME
#define CLNS_IP_CMAJORS		CONFIG_STREAMS_CLNS_IP_NMAJORS
#define CLNS_IP_CMAJOR_0	CONFIG_STREAMS_CLNS_IP_MAJOR
#define CLNS_IP_UNITS		CONFIG_STREAMS_CLNS_IP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CLNS_MODID));
MODULE_ALIAS("streams-driver-clns-ip");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_CLNS_MAJOR));
MODULE_ALIAS("/dev/streams/clns-ip");
MODULE_ALIAS("/dev/streams/clns-ip/*");
MODULE_ALIAS("/dev/streams/clone/clns-ip");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(CLNS_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(CLNS_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(CLNS_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/clns-ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define DRV_ID		CLNS_IP_DRV_ID
#define DRV_NAME	CLNS_IP_DRV_NAME
#define CMAJORS		CLNS_IP_CMAJORS
#define CMAJOR_0	CLNS_IP_CMAJOR_0
#define UNITS		CLNS_UNITS
#ifdef MODULE
#define DRV_BANNER	CLNS_BANNER
#else				/* MODULE */
#define DRV_BANNER	CLNS_SPLASH
#endif				/* MODULE */

static struct module_info np_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = 0,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = STRHIGH,		/* XXX: adjust these upward */
	.mi_lowat = STRLOW,		/* XXX: adjust these upward */
};

static struct module_stat np_urstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat np_uwstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat np_lrstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat np_lwstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURES
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  LOCKING
 *
 *  =========================================================================
 */

/*
 * This lock protect the association of upper and lower streams as well as protecting the
 * dereference of q->q_ptr for lower queue pairs.  Note that while this lock is held held for read,
 * no upper stream can close, no lower stream can be unlinked, and the association of upper streams
 * to lower streams is frozen, also the np_opens and np_links lists are frozen.  This lock is head
 * for write whenever an upper stream opens and closes and affects the np_opens list, a lower stream
 * links or unlinks and affects the np_links, or the association between upper and lower streams is
 * altered during the bind and unbind operations.  This driver is protected from interrupts and
 * interrupt suppression is not necessary when taking these locks (making them disappear on UP).
 */
static rwlock_t np_mux_lock = RWLOCK_UNLOCKED;

/*
 * Here is the locking strategy.  Upper multiplex queue pairs use np_acquire() and np_release().
 * Lower multiplex queue pairs use np_mux_acquire() and np_mux_release().  The former results in
 * locking upper stream.  The latter results in locking the lower stream.  np_acquire() and
 * np_release() are used by the upper write queue put and service procedures.  np_mux_acquire() and
 * np_mux_release() are used by the lower read queue put and service procedures.  np_lock() and
 * np_unlock() can be called by a lower stream to lock an upper stream and visa versa; however, the
 * caller is responsible for holding the np_mux_lock before acquiring a pointer to the other stream
 * to prevent the stream from closing or unlinking.
 */

static struct np *
np_lock(struct np *np, queue_t *q)
{
	struct np *rtn = np;

	spin_lock(np->sq.lock, flags);
	if (likely(np->sq.users == 0)) {
		q = NULL;
		np->sq.users++;
	} else {
		rtn = NULL;
		q = XCHG(&np->sq.q, q);
	}
	spin_unlock(np->sq.lock, flags);
	if (q)
		qenable(q);
	return (rtn);
};

static void
np_unlock(struct np *np)
{
	queue_t *q;

	spin_lock(np->sq.lock, flags);
	np->sq.users = 0;
	if ((q = np->sq.q) != NULL)
		np->sq.q = NULL;
	spin_unlock(np->sq.lock, flags);
	if (q)
		qenable(q);
	if (waitqueue_active(np->sq.waitq))
		wakeup_exclusive(np->sq.waitq);
	return;
};

/**
 * For lower multiplex Streams, the put or service procedure could be invoked between the point that
 * the driver receives the I_UNLINK or I_PUNLINK and removes the private structure, and the point
 * that the Stream head is placed back against the queue pair.  We NULL q->q_ptr for the queue pair
 * under np_mux_lock write lock during the unlinking process and the put or service procedure will
 * encounter a NULL q->q_ptr when invoked between the unlink and Stream head replacement.  This lock
 * protects dereference of the q->q_ptr on SMP.
 *
 * Note that the caller can check NP_PRIV(q) after this function returns NULL to determine whether
 * the failure was due to failure to acquire the lock or whether NP_PRIV(q) was simply NULL.
 */
static struct np *
np_mux_acquire(queue_t *q)
{
	struct np *rtn;

	read_lock(&np_mux_lock);
	if ((rtn = NP_PRIV(q)))
		rtn = np_lock(np, q);
	read_unlock(&np_mux_lock);
	return (rtn);
}
static void
np_mux_release(struct np *np)
{
	np_unlock(np);
}

static struct np *
np_acquire(queue_t *q)
{
	return np_lock(NP_PRIV(q), q);
}
static void
np_release(struct np *np)
{
	np_unlock(np);
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  =========================================================================
 */

/**
 * m_error: - issue an M_ERROR message
 * @np: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 * @error: the error to send
 */
static noinline fastcall int
m_error(struct np *np, queue_t *q, mblk_t *msg, int error)
{
}

/**
 * n_info_ack: - issue an N_INFO_ACK primitive
 * @np: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
n_info_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = np->prot.NSDU_size;
		p->ENSDU_size = np->prot.ENSDU_size;
		p->CDATA_size = np->prot.CDATA_size;
		p->DDATA_size = np->prot.DDATA_size;
		p->ADDR_size = np->prot.ADDR_size;
		p->ADDR_length = np->prot.ADDR_length;
		p->ADDR_offset = sizeof(*p);
		p->QOS_length = np->prot.QOS_length;
		p->QOS_offset = p->ADDR_offset + p->ADDR_length;
		p->QOS_range_length = np->prot.QOS_range_length;
		p->QOS_range_offset = p->QOS_offset + p->QOS_length;
		p->OPTIONS_flags = np->prot.OPTIONS_flags;
		p->NIDU_size = np->prot.NIDU_size;
		p->SERV_type = np->prot.SERV_type;
		p->CURRENT_state = np->prot.CURRENT_state;
		p->PROVIDER_type = np->prot.PROVIDER_type;
		p->NODU_size = np->prot.NODU_size;
		p->PROTOID_length = np->prot.PROTOID_length;
		p->PROTOID_offset = p->QOS_range_offset + p->QOS_range_length;
		p->NPI_version = np->prot.NPI_version;
		mp->b_wptr += sizeof(*p);
		bcopy(np->add, mp->b_wptr, p->ADDR_length);
		mp->b_wptr += p->ADDR_length;
		bcopy(np->qos, mp->b_wptr, p->QOS_length);
		mp->b_wptr += p->QOS_length;
		bcopy(np->qor, mp->b_wptr, p->QOS_range_length);
		mp->b_wptr += p->QOS_range_length;
		bcopy(np->pro, mp->b_wptr, p->PROTOID_length);
		mp->b_wptr += p->PROTOID_length;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_INFO_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_bind_ack: - issue an N_BIND_ACK primitive
 * @np: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
n_bind_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_bind_ack_t *p;
	mblk_t *mp;

	size_t add_len = np->prot.ADDR_length;
	size_t pro_len = np->prot.PROTOID_length;
	size_t msg_len = sizeof(*p) + add_len + pro_len;

	if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = 0;
		p->TOKEN_value = 0;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = sizeof(*p) + add_len;
		mp->b_wptr += sizeof(*p);
		bcopy(&np->add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(&np->pro, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
		freemsg(msg);
		np->prot.CURRENT_state = NS_IDLE;
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_BIND_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_error_ack: - issue an N_ERROR_ACK primitive
 * @np: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @errno: error number
 */
static noinline fastcall int
n_error_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_long errno)
{
	N_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = errno < 0 ? NSYSERR : errno;
		p->UNIX_error = errno < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		np->prot.CURRENT_state = np->oldstate;
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_ok_ack: - issue an N_OK_ACK primitive
 * @np: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static noinline fastcall int
n_ok_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim)
{
	N_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (np->prot.CURRENT_state) {
		case NS_WACK_UREQ:
			np->prot.CURRENT_state = NS_UNBND;
			break;
		case NS_WACK_OPTREQ:
			np->prot.CURRENT_state = np->oldstate;
			break;
		}
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_OK_ACK");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unitdata_ind: - issue an N_UNITDATA_IND primitive
 * @np: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dst_ptr: dest address pointer
 * @dst_len: dest address length
 * @src_ptr: srce address pointer
 * @src_len: srce address length
 * @dp: user data
 */
static inline fastcall __hot_get int
n_unitdata_ind(struct np *np, queue_t *q, mblk_t *msg,
	       caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;

	size_t msg_len = sizeof(*p) + dst_len + src_len;

	if (likely(bcanputnext(np->oq, 0))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p) + dst_len;
			p->ERROR_type = msg ? (msg->b_flag & 0x0100) ? N_UD_CONGESTION : 0 : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
#ifndef _OPTIMIZE_SPEED
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- N_UNITDATA_IND");
#endif				/* _OPTIMIZE_SPEED */
			putnext(np->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	if (msg)
		msg->b_flag |= 0x0100;
	return (-EBUSY);
}

/**
 * n_uderror_ind: - issue an N_UDERROR_IND primitive
 * @np: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 * @errno: error number
 * @dst_ptr: dest address pointer
 * @dst_len: dest address length
 * @dp: user data
 */
static inline fastcall int
n_uderror_ind(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp, np_ulong errno,
	      caddr_t dst_ptr, size_t dst_len, mblk_t *dp)
{
	N_uderror_ind_t *p;
	mblk_t *mp;

	if (likely(bcanputnext(np->oq, 1))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->RESERVED_field = 0;
			p->ERROR_type = errno;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
#ifndef _OPTIMIZE_SPEED
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- N_UDERROR_IND");
#endif				/* _OPTIMIZE_SPEED */
			putnext(np->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  NP User -> NP Provider primitives.
 */
/**
 * np_w_info_req: - process N_INFO_REQ primitive
 * @np: upper multiplex private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ primitive
 */
static noinline fastcall int
np_w_info_req(struct np *np, queue_t *q, mblk_t *mp)
{
	return n_info_ack(np, q, mp);
}

/**
 * np_w_info_req: - process N_INFO_REQ primitive
 * @np: upper multiplex private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ primitive
 */
static noinline fastcall int
np_w_bind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (!MBLKIN(mp, p->ADDR_offset, p->ADDR_length))
		goto badaddr;
	if (!MBLKIN(mp, p->PROTOID_offset, p->PROTOID_length))
		goto noprotoid;
	if (p->ADDR_length == 0)
		goto noaddr;
	if (p->ADDR_length > sizeof(np->add))
		goto badaddr;
	if (p->PROTOID_length == 0) {
		np->pro[0] = CLNS_CLNP_ID;
		np->pro[1] = CLNS_ESIS_ID;
		np->pro[2] = CLNS_ISIS_ID;
		np->prot.PROTOID_length = 3;
	} else {
		uchar *b;
		int i, j, k;

		for (b = mp->b_rptr + p->PROTOID_offset, i = j = 0; i < p->PROTOID_length; i++, b++) {
			switch (*b) {
			case CLNS_CLNP_ID:
			case CLNS_ESIS_ID:
			case CLNS_ISIS_ID:
				for (k = 0; k < j && np->pro[k] != *b; k++) ;
				if (k < j)
					continue;
				np->pro[j] = *b;
				j++;
				continue;
			default:
				goto noprotoid;
			}
		}
		np->prot.PROTOID_length = j;
	}
	if (p->BIND_flags & ~DEFAULT_DEST)
		goto badflag;
	if (p->BIND_flags & DEFAULT_DEST) {
	} else {
	}
	np->oldstate = np->prot.CURRENT_state;
	np->prot.CURRENT_state = NS_WACK_BREQ;
	return n_bind_ack(np, q, mp);
      badflag:
	err = NBADFLAG;
	goto error;
      noprotoid:
	err = NNOPROTOID;
	goto error;
      noaddr:
	err = NNOADDR;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      error:
	np->prot.ADDR_length = 0;
	np->prot.PROTOID_length = 0;
	return n_error_ack(np, q, mp, N_BIND_REQ, err);
}

/**
 * np_w_info_req: - process N_INFO_REQ primitive
 * @np: upper multiplex private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ primitive
 */
static noinline fastcall int
np_w_unbind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	np->prot.ADDR_length = 0;
	np->oldstate = np->prot.CURRENT_state;
	np->prot.CURRENT_state = NS_WACK_UREQ;
	return n_ok_ack(np, q, mp);
}

/**
 * np_w_info_req: - process N_INFO_REQ primitive
 * @np: upper multiplex private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ primitive
 */
static noinline fastcall int
np_w_optmgmt_req(struct np *np, queue_t *q, mblk_t *mp)
{
	np->oldstate = np->prot.CURRENT_state;
	np->prot.CURRENT_state = NS_WACK_OPTREQ;
	return n_ok_ack(np, q, mp);
}

/**
 * np_w_info_req: - process N_INFO_REQ primitive
 * @np: upper multiplex private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ primitive
 */
static inline fastcall __hot_put int
np_w_unitdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
}

/**
 * np_w_info_req: - process N_INFO_REQ primitive
 * @np: upper multiplex private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ primitive
 */
static noinline fastcall int
np_w_nosupp_req(struct np *np, queue_t *q, mblk_t *mp)
{
	return n_error_ack(np, q, mp, *(np_ulong *)mp->b_rptr, NNOTSUPPORT);
}

/**
 * np_w_info_req: - process N_INFO_REQ primitive
 * @np: upper multiplex private structure
 * @q: active queue (upper write queue)
 * @mp: the N_INFO_REQ primitive
 */
static noinline fastcall int
np_w_other_req(struct np *np, queue_t *q, mblk_t *mp)
{
	return n_error_ack(np, q, mp, *(np_ulong *)mp->b_rptr, NNOTSUPPORT);
}
/*
 *  NP Provider -> NP User primitives (upper multiplex).
 */
static inline fastcall __hot_read int np_r_unitdata_ind(struct np *np, queue_t *q, mblk_t *mp) { }
static inline fastcall int np_r_uderror_ind(struct np *np, queue_t *q, mblk_t *mp) { }
static noinline fastcall int np_r_other_ind(struct np *np, queue_t *q, mblk_t *mp) { }

/*
 *  NP Provider -> NP User primitives (lower multiplex).
 */
static noinline fastcall int np_m_info_ack(struct np *np, queue_t *q, mblk_t *mp) { }
static noinline fastcall int np_m_bind_ack(struct np *np, queue_t *q, mblk_t *mp) { }
static noinline fastcall int np_m_error_ack(struct np *np, queue_t *q, mblk_t *mp) { }
static noinline fastcall int np_m_ok_ack(struct np *np, queue_t *q, mblk_t *mp) { }
static inline fastcall __hot_in int np_m_unitdata_ind(struct np *np, queue_t *q, mblk_t *mp) { }
static inline fastcall int np_m_uderror_ind(struct np *np, queue_t *q, mblk_t *mp) { }
static noinline fastcall int np_m_nosupp_ind(struct np *np, queue_t *q, mblk_t *mp) { }
static noinline fastcall int np_m_other_ind(struct np *np, queue_t *q, mblk_t *mp) { }


/*
 *  =========================================================================
 *
 *  STREAMS INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
 */

/**
 * np_i_link: - process I_LINK command
 * @np: private structure (control stream)
 * @q: upper write queue
 * @mp: the I_LINK command
 */
static noinline fastcall __unlikely
np_i_link(struct np *np, queue_t *, mblk_t *mp)
{
}

/**
 * np_i_unlink: - process I_UNLINK command
 * @np: private structure (control stream)
 * @q: upper write queue
 * @mp: the I_UNLINK command
 */
static noinline fastcall __unlikely
np_i_unlink(struct np *np, queue_t *, mblk_t *mp)
{
}

/**
 * np_i_plink: - process I_PLINK command
 * @np: private structure (control stream)
 * @q: upper write queue
 * @mp: the I_PLINK command
 */
static noinline fastcall __unlikely
np_i_plink(struct np *np, queue_t *, mblk_t *mp)
{
}

/**
 * np_i_punlink: - process I_PUNLINK command
 * @np: private structure (control stream)
 * @q: upper write queue
 * @mp: the I_PUNLINK command
 */
static noinline fastcall __unlikely
np_i_punlink(struct np *np, queue_t *, mblk_t *mp)
{
}

#if 0				/* later */
static noinline fastcall __unlikely
np_ioctl(struct np *np, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
}
static noinline fastcall __unlikely
np_copyin(struct np *np, queue_t *q, mblk_t *mp, struct copyresp *cp, mblk_t *dp)
{
}
static noinline fastcall __unlikely
np_copyout(struct np *np, queue_t *q, mblk_t *mp, struct copyresp *cp)
{
}
#endif

/*
 *  =========================================================================
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  =========================================================================
 */

static noinline fastcall
__np_w_proto(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong prim;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto eproto;

	switch (prim) {
	case N_INFO_REQ:
		rtn = np_w_info_req(np, q, mp);
		break;
	case N_BIND_REQ:
		rtn = np_w_bind_req(np, q, mp);
		break;
	case N_UNBIND_REQ:
		rtn = np_w_unbind_req(np, q, mp);
		break;
	case N_OPTMGMT_REQ:
		rtn = np_w_optmgmt_req(np, q, mp);
		break;
	case N_UNITDATA_REQ:
		rtn = np_w_unitdata_req(np, q, mp);
		break;
	case N_CONN_REQ:
	case N_CONN_RES:
	case N_DISCON_REQ:
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_DATACK_REQ:
	case N_RESET_REQ:
	case N_RESET_RES:
		rtn = np_w_nosupp_req(np, q, mp);
	default:
		rtn = np_w_other_req(np, q, mp);
		break;
	}
eproto:
	return m_error(np, q, mp, EPROTO);
}
static noinline fastcall
np_w_proto(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int rtn = -EAGAIN;

	if (likely(!!(np = np_acquire(q)))) {
		rtn = __np_w_proto(np, q, mp);
		np_release(np);
	}
	return (rtn);
}
static noinline fastcall
np_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
#if 0
		/* Upstream modules are not allowed to flush our read queue.  We use the read queue
		   to hold messages from below.  It is something that will be done on closing. */
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
#endif
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static noinline fastcall __unlikely
__np_w_ioctl(struct np *np, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int rtn;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(I_LINK):
			err = np_i_link(np, q, mp);
			break;
		case _IOC_NR(I_UNLINK):
			err = np_i_unlink(np, q, mp);
			break;
		case _IOC_NR(I_PLINK):
			err = np_i_plink(np, q, mp);
			break;
		case _IOC_NR(I_PUNLINK):
			err = np_i_punlink(np, q, mp);
			break;
		default:
			err = EINVAL;
			break;
		}
		break;
	default:
		rtn = EINVAL;
		break;
	}
	if (rtn > 0) {
		mi_copy_done(q, mp, rtn);
		rtn = 0;
	}
	return (rtn);
}
static noinline fastcall __unlikely
np_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int rtn = -EAGAIN;

	if (likely(!!(np = np_acquire(q)))) {
		rtn = __np_w_ioctl(np, q, mp);
		np_release(np);
	}
	return (rtn);
}
static noinline fastcall __unlikely
__np_w_iocdata(struct np *np, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline fastcall __unlikely
np_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int rtn = -EAGAIN;

	if (likely(!!(np = np_acquire(q)))) {
		rtn = __np_w_iocdata(np, q, mp);
		np_release(np);
	}
	return (rtn);
}
static noinline fastcall __unlikely
np_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "SWERR: %s: message type %d encountered", __FUNCTION__,
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}
static noinline fastcall int
np_w_msg_srv_slow(struct np *np, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return __np_w_proto(np, q, mp);
	case M_FLUSH:
		return np_w_flush(q, mp);
	case M_IOCTL:
		return __np_w_ioctl(np, q, mp);
	case M_IOCDATA:
		return __np_w_iocdata(np, q, mp);
	default:
		return np_w_other(q, mp);
	}
}
static inline fastcall int __hot_read
np_w_msg_srv(struct np *np, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO)) {
		if (likely(MBLKIN(mp, 0, sizeof(np_ulong)))) {
			if (likely(*(np_ulong *) mp->b_rptr == N_UNITDATA_REQ)) {
				return np_w_unitdata_req(np, q, mp);
			}
		}
	}
	return np_w_msg_srv_slow(np, q, mp);
}
static noinline fastcall int
np_w_msg_put_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return np_w_proto(q, mp);
	case M_FLUSH:
		return np_w_flush(q, mp);
	case M_IOCTL:
		return np_w_ioctl(q, mp);
	case M_IOCDATA:
		return np_w_iocdata(q, mp);
	default:
		return np_w_other(q, mp);
	}
}
static inline fastcall int __hot_put
np_w_msg_put(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int rtn;

	if (likely(DB_TYPE(mp) == M_PROTO)) {
		if (likely(MBLKIN(mp, 0, sizeof(np_ulong)))) {
			if (likely(*(np_ulong *) mp->b_rptr == N_UNITDATA_REQ)) {
				if (likely(!!(np = np_acquire(q)))) {
					rtn = np_w_unitdata_req(np, q, mp);
					np_release(np);
					return (rtn);
				}
			}
		}
	}
	return np_w_msg_put_slow(q, mp);
}

static noinline fastcall
__np_r_proto(struct np *np, queue_t *q, mblk_t *mp)
{
	switch (prim) {
	case N_UNITDATA_IND:
		rtn = np_r_unitdata_ind(np, q, mp);
		break;
	case N_UDERROR_IND:
		rtn = np_r_uderror_ind(np, q, mp);
		break;
	default:
		rtn = np_r_other_ind(np, q, mp);
		break;
	}
}
static noinline fastcall
np_r_proto(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int rtn = -EAGAIN;

	if (likely(!!(np = np_acquire(q)))) {
		rtn = __np_r_proto(np, q, mp);
		np_release(np);
	}
	return (rtn);
}
static noinline fastcall __unlikely
np_r_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "SWERR: %s: message type %d encountered", __FUNCTION__,
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}
static noinline fastcall int
np_r_msg_srv_slow(struct np *np, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
		return __np_r_proto(np, q, mp);
	default:
		return np_r_other(q, mp);
	}
}
static inline fastcall int __hot_get
np_r_msg_srv(struct np *np, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO)) {
		if (likely(MBLKIN(mp, 0, sizeof(np_ulong)))) {
			if (likely(*(np_ulong *) mp->b_rptr == N_UNITDATA_IND)) {
				return np_r_unitdata_ind(np, q, mp);
			}
		}
	}
	return np_r_msg_srv_slow(np, q, mp);
}
static noinline fastcall int
np_r_msg_put_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
		return np_r_proto(q, mp);
	default:
		return np_r_other(q, mp);
	}
}
static inline fastcall int __hot_read
np_r_msg_put(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int rtn;

	if (likely(DB_TYPE(mp) == M_PROTO)) {
		if (likely(MBLKIN(mp, 0, sizeof(np_ulong)))) {
			if (likely(*(np_ulong *) mp->b_rptr == N_UNITDATA_IND)) {
				if (likely(!!(np = np_acquire(q)))) {
					rtn = np_r_unitdata_ind(np, q, mp);
					np_release(np);
					return (rtn);
				}
			}
		}
	}
	return np_r_msg_put_slow(q, mp);
}

static noinline fastcall
__np_m_proto(struct np *np, queue_t *q, mblk_t *mp)
{
	switch (prim) {
	case N_INFO_ACK:
		rtn = np_m_info_ack(np, q, mp);
		break;
	case N_BIND_ACK:
		rtn = np_m_bind_ack(np, q, mp);
		break;
	case N_ERROR_ACK:
		rtn = np_m_error_ack(np, q, mp);
		break;
	case N_OK_ACK:
		rtn = np_m_ok_ack(np, q, mp);
		break;
	case N_UNITDATA_IND:
		rtn = np_m_unitdata_ind(np, q, mp);
		break;
	case N_UDERROR_IND:
		rtn = np_m_uderror_ind(np, q, mp);
		break;
	case N_CONN_IND:
	case N_CONN_CON:
	case N_DISCON_IND:
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_DATACK_IND:
	case N_RESET_IND:
	case N_RESET_CON:
		rtn = np_m_nosupp_ind(np, q, mp);
		break;
	default:
		rtn = np_m_other_ind(np, q, mp);
		break;
	}
}
static noinline fastcall
np_m_proto(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int rtn = -EAGAIN;

	if (likely(!!(np = np_mux_acquire(q)))) {
		rtn = __np_m_proto(np, q, mp);
		np_mux_release(np);
	}
	return (rtn);
}
static noinline fastcall
np_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHW) {
#if 0
		/* We do not place messages on the lower write queue. */
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
#endif
		/* Have to behave like stream head here... */
		if (!(mp->b_flag & MSGNOLOOP)) {
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}
static noinline fastcall __unlikely
np_m_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "SWERR: %s: message type %d encountered", __FUNCTION__,
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}
static noinline fastcall int
np_m_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return np_m_proto(q, mp);
	case M_FLUSH:
		return np_m_flush(q, mp);
	default:
		return np_m_other(q, mp);
	}
}
static inline fastcall int __hot_in
np_m_msg(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int rtn;

	if (likely(DB_TYPE(mp) == M_PROTO)) {
		if (likely(MBLKIN(mp, 0, sizeof(np_ulong)))) {
			if (likely(*(np_ulong *) mp->b_rptr == N_UNITDATA_IND)) {
				if (likely(!!(np = np_mux_acquire(q)))) {
					rtn = np_m_unitdata_ind(np, q, mp);
					np_mux_release(q, mp);
					return (rtn);
				}
			}
		}
	}
	return np_m_msg_slow(q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */

/**
 * np_wput: upper multiplex write queue put procedure
 * @q: upper multiplex write queue
 * @mp: the message to put
 *
 * Canonical put procedure.
 */
static streamscall int __hot_put
np_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || np_w_msg_put(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * np_wsrv: upper multiplex write queue service procedure
 * @q: upper multiplex write queue
 *
 * Canonical service procedure with external locking.
 */
static streamscall int __hot_read
np_wsrv(queue_t *q)
{
	mblk_t *mp;

	if (likely(!!(mp = getq(q)))) {
		struct np *np;

		if (likely(!!(np = np_acquire(q)))) {
			do {
				if (np_w_msg_srv(np, q, mp))
					break;
			} while (likely(!!(mp = getq(q))));
			np_release(np);
		}
	}
	if (mp != NULL)
		putbq(q, mp);
	return (0);
}

/**
 * np_rsrv: upper multiplex read queue service procedure
 * @q: upper multiplex read queue
 *
 * Canonical service procedure with external locking.
 */
static streamscall int __hot_get
np_rsrv(queue_t *q)
{
	mblk_t *mp;

	if (likely(!!(mp = getq(q)))) {
		struct np *np;

		if (likely(!!(np = np_acquire(q)))) {
			do {
				if (np_r_msg_srv(np, q, mp))
					break;
			} while (likely(!!(mp = getq(q))));
			np_release(np);
		}
	}
	if (mp != NULL)
		putbq(q, mp);
	return (0);
}

/**
 * np_rput: upper multiplex read queue put procedure
 * @q: upper multiplex read queue
 * @mp: the message to put
 *
 * This is where the SNPA (lower NP stream) places messages that are destined for this stream.  They
 * are formatted as lower multiplex NPI N_UNITDATA_IND or NPI N_UDERROR_IND messages.  We use two
 * bands for this purpose: band 0 for N_UNITDATA_IND messages and band 1 for N_UDERROR_IND messages.
 * The feeding lower stream need not check flow control before calling this put() procedure, but
 * needs to set the band number of the message before calling put().
 */
static streamscall int __hot_read
np_rput(queue_t *q)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || np_r_msg_put(q, mp))
		if (!bcanput(q, mp->b_band) || !putq(q, mp))
			freemsg(mp);
	return (0);
}

/**
 * np_muxwsrv: lower write queue service procedure
 * @q: lower write qeuue
 *
 * This function is only invoked when flow controlled downstream on the write side and the flow
 * control has subsided resulting in this queue being back-enabled.  Downstream write side flow
 * control only occurs when an upper stream fails bcanputnext() for the queue and places the message
 * back on its own write queue.  Therefore, when back-enabled we need to wake all the flow
 * controlled upper Streams.  This function tries to be intelligent by disadvantaging noisy upper
 * Streams to keep them from starving other NSAPs on the same SNPA.  This is done by remembering
 * which sender first encountered the congestion (likely the noisy one).  The sender that first
 * encountered congestion (called the loud stream), is enabled last and will thus be last to be
 * serviced by the STREAMS scheduler on this processor.  All other blocked streams go first.
 */
static streamscall int __hot_out
np_muxwsrv(queue_t *q)
{
	struct np *np, *p = NP_PRIV(q);

	/* Note, we could be a little fairer about this by marking the loud NSAP and enabling their
	   write queue last. */
	if (!(p = np_mux_acquire(q)))
		return;
	if (p->blocked) {
		/* Note, associated upper streams cannot close until we call np_mux_release(). */
		for (np = p->np.list; np; np = np->np.next) {
			if (np->blocked) {
				if (np == p->loud)
					continue;
				np->blocked = 0;
				qenable(np->wq);
			}
		}
		if (p->loud != NULL) {
			qenable(p->loud->wq);
			p->loud = NULL;
		}
		p->blocked = 0;
	}
	np_mux_release(p);
	return (0);
}

/**
 * np_muxrsrv: lower multiplex read queue service procedure
 * @q: lower multiplex read queue
 *
 * Canonical service procedure.
 *
 * The purpose of this service procedure is not to do flow control across the multiplexing driver to
 * uppser Streams, but to handle lock contention.  When the upper NSAP stream is flow controlled,
 * the message is simply discarded.
 */
static streamscall int __hot_read
np_muxrsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q)))
		if (np_m_msg(q, mp))
			break;
	if (mp != NULL)
		putbq(q, mp);
	return (0);
}

/**
 * np_muxrsrv: lower multiplex read queue put procedure
 * @q: lower multiplex read queue
 * @mp: the message to put
 *
 * Canonical put procedure with interrupt protection.
 *
 * We check whether we are in an interrupt to keep from running np_m_msg() in interrupt context.
 * This protects the entire driver from interrupt context processing and makes all locks simple
 * spin locks that do not need BH or IRQ protection.
 */
static streamscall int __hot_in
np_muxrput(queue_t *q, mblk_t *mp)
{
	if (in_interrupt() || (!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || np_m_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN AND CLOSE ROUTINES
 *
 *  =========================================================================
 */

static streamscall int
np_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	np->prot.PRIM_type = N_INFO_ACK;
	np->prot.NSDU_size = 32768 - 0;	/* FIXME: subtract CLNP headers */
	np->prot.ENSDU_size = QOS_UNKNOWN;
	np->prot.CDATA_size = QOS_UNKNOWN;
	np->prot.DDATA_size = QOS_UNKNOWN;
	np->prot.ADDR_size = sizeof(np->add);
	np->prot.ADDR_length = 0;
	np->prot.ADDR_offset = sizeof(np->prot);
	np->prot.QOS_length = sizeof(np->qos);
	np->prot.QOS_offset = np->prot.ADDR_offset + mp->prot.ADDR_length;
	np->prot.QOS_range_length = sizeof(np->qor);
	np->prot.QOS_range_offset = np->prot.QOS_offset + mp->prot.QOS_length;
	np->prot.OPTIONS_flags = 0;
	np->prot.NIDU_size = QOS_UNKNOWN;
	np->prot.SERV_type = N_CLNS;
	np->prot.CURRENT_state = NS_UNBND;
	np->prot.PROVIDER_type = N_SNICFP;
	np->prot.NODU_size = QOS_UNKNOWN;   /* set to MTU of provider when bound */
	np->prot.PROTOID_length = 1;
	np->prot.PROTOID_offset = np->prot.QOS_range_offset + mp->prot.QOS_range_length;
	np->prot.NPI_version = NPI_CURRENT_VERSION;
}
static streamscall int
np_qclose(queue_t *q, int oflags, cred_t *crp)
{

	/* Note that if we are still associated with a lower stream, we must wait until we can
	   acquire the lock before disassociating. */
}

/*
 *  =========================================================================
 *
 *  REGISTRATIN AND INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit np_rdinit = {
	.qi_putp = np_rput,
	.qi_srvp = np_rsrv,
	.qi_qopen = np_qopen,
	.qi_qclose = np_qclose,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_urstat,
};
static struct qinit np_wrinit = {
	.qi_putp = np_wput,
	.qi_srvp = np_wsrv,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_uwstat,
};

static struct qinit np_muxrinit = {
	.qi_putp = np_muxrput,
	.qi_srvp = np_muxrsrv,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_lrstat,
};
static struct qinit np_muxwinit = {
	.qi_putp = NULL,
	.qi_srvp = np_muxwsrv,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_lwstat,
};

static struct streamtab clns_ipinfo = {
	.st_rdinit = &np_rdinit,
	.st_wrinit = &np_wrinit,
	.st_muxrinit = &np_muxrinit,
	.st_muxwinit = &np_muxwinit,
};
