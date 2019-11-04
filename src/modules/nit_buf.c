/*****************************************************************************

 @(#) File: src/modules/nit_buf.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2019  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 *****************************************************************************/

static char const ident[] = "src/modules/nit_buf.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  This is NIT_BUF, a classical STREAMS buffering module similar to bufmod(4) that performs the
 *  actions described in nit_buf(4).  Note that this is a stripped down version of bufmod(4).
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#define _SVR4_SOURCE
#define _MPS_SOURCE
#define _SUN_SOURCE

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/strlog.h>
#include <sys/cmn_err.h>

#include <net/nit_buf.h>

#include "sys/config.h"

#define NBUF_DESCRIP	"SVR 4.1 SNI Buffer Module (NBUF) STREAMS Module"
#define NBUF_EXTRA	"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NBUF_COPYRIGHT	"Copyright (c) 2008-2019  Monavacon Limited.  All Rights Reserved."
#define NBUF_REVISION	"OpenSS7 src/modules/nit_buf.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define NBUF_DEVICE	"SVR 4.1 SNIT Buffer Module (NBUF) for STREAMS."
#define NBUF_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NBUF_LICENSE	"GPL"
#define NBUF_BANNER	NBUF_DESCRIP	"\n" \
			NBUF_EXTRA	"\n" \
			NBUF_COPYRIGHT	"\n" \
			NBUF_REVISION	"\n" \
			NBUF_DEVICE	"\n" \
			NBUF_CONTACT	"\n"
#define NBUF_SPLASH	NBUF_DEVICE	" - " \
			NBUF_REVISION

#ifdef CONFIG_STREAMS_NBUF_MODULE
MODULE_AUTHOR(NBUF_CONTACT);
MODULE_DESCRIPTION(NBUF_DESCRIP);
MODULE_SUPPORTED_DEVICE(NBUF_DEVICE);
MODULE_LICENSE(NBUF_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_NBUF_MODULE */

#ifndef CONFIG_STREAMS_NBUF_NAME
//#define CONFIG_STREAMS_NBUF_NAME "nbuf"
#error "CONFIG_STREAMS_NBUF_NAME must be defined."
#endif				/* CONFIG_STREAMS_NBUF_NAME */
#ifndef CONFIG_STREAMS_NBUF_MODID
//#define CONFIG_STREAMS_NBUF_MODID 32
#error "CONFIG_STREAMS_NBUF_MODID must be defined."
#endif				/* CONFIG_STREAMS_NBUF_MODID */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-nbuf");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_NBUF_MODULE
static
#endif					/* CONFIG_STREAMS_NBUF_MODULE */
modID_t modid = CONFIG_STREAMS_NBUF_MODID;

#ifdef CONFIG_STREAMS_NBUF_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for NBUF.");
#endif				/* CONFIG_STREAMS_NBUF_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NBUF_MODID));
MODULE_ALIAS("streams-module-nbuf");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info nbuf_minfo = {
	.mi_idnum = CONFIG_STREAMS_NBUF_MODID,
	.mi_idname = CONFIG_STREAMS_NBUF_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat nbuf_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat nbuf_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  Private Structure
 *
 *  -------------------------------------------------------------------------
 */

/** struct nb: - SNIT buffer private structure.
  * @nb_rq: the module read queue (write once, read only)
  * @nb_chunk: chunk size.  Written by ioctl and read by everything else.
  * @nb_ticks: timeout.  Written by ioctl and read by everything else.
  * @nb_timer: timeout id. Read and written by timer functions.
  * @nb_pdrops: dropped packets.  Read and written by read put procedure only.
  * @nb_sdrops: dropped packets.  Read and written by read service procedure only.
  */
struct nb {
	queue_t *nb_rq;			/* read queue */
	uint32_t nb_chunk;		/* chunk size */
	uint32_t nb_ticks;		/* number of ticks for timeout */
	mblk_t *nb_timer;		/* mi_timer(9) message block */
	uint32_t nb_pdrops;		/* number of rput dropped packets */
	uint32_t nb_sdrops;		/* number of rsrv dropped packets */
};

#define MSGHEADER	(1<<15)

#if	defined DEFINE_RWLOCK
static DEFINE_RWLOCK(nb_lock);
#elif	defined __RW_LOCK_UNLOCKED
static rwlock_t nb_lock = __RW_LOCK_UNLOCKED(nb_lock);
#elif	defined RW_LOCK_UNLOCKED
static rwlock_t nb_lock = RW_LOCK_UNLOCKED;
#else
#error cannot initialize read-write locks
#endif
static caddr_t nb_opens;

/*
 *  -------------------------------------------------------------------------
 *
 *  STREAM OPTIONS
 *
 *  -------------------------------------------------------------------------
 */
#define	SNIT_HIWAT(msgsize, fudge)	((4 * msgsize * fudge) + 512)
#define	SNIT_LOWAT(msgsize, fudge)	((2 * msgsize * fudge) + 256)

static noinline fastcall __unlikely int
nbuf_stropts(struct nb *nb, int chunk)
{
	struct stroptions *so;
	mblk_t *mp;

	if (chunk < 4096)
		chunk = 4096;
	if (chunk > 65536)
		chunk = 65536;

	if (likely((mp = allocb(sizeof(*so), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_SETOPTS;
		mp->b_wptr = mp->b_rptr + sizeof(*so);
		bzero(mp->b_rptr, sizeof(*so));
		so = (typeof(so)) mp->b_rptr;
		so->so_flags = 0;
		so->so_flags |= SO_MREADON;
		so->so_lowat = SNIT_LOWAT(chunk, 4);
		if (so->so_lowat < SHEADLOWAT)
			so->so_lowat = SHEADLOWAT;
		so->so_flags |= SO_LOWAT;
		so->so_hiwat = SNIT_HIWAT(chunk, 4);
		if (so->so_hiwat < SHEADHIWAT)
			so->so_hiwat = SHEADHIWAT;
		so->so_flags |= SO_HIWAT;
		putnext(nb->nb_rq, mp);
		return (0);
	}
	return (-ENOSR);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IOCTLS
 *
 *  -------------------------------------------------------------------------
 */

STATIC noinline __unlikely fastcall void
nbuf_ioctl(queue_t *q, mblk_t *mp)
{
	struct nb *nb = (typeof(nb)) q->q_ptr;
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *db;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case _IOC_TYPE(NIOC):
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(NIOCSTIME):
#ifdef __LP64__
			if (ioc->ioc_flag == IOC_ILP32)
				mi_copyin(q, mp, NULL, sizeof(struct timeval32));
			else
#endif				/* __LP64__ */
				mi_copyin(q, mp, NULL, sizeof(struct timeval));
			break;
		case _IOC_NR(NIOCGTIME):
		{
			struct timeval tn;

			if (nb->nb_ticks < 0) {
				mi_copy_done(q, mp, ERANGE);
				break;
			}
			if (nb->nb_ticks <= 0) {
				tn.tv_sec = 0;
				tn.tv_usec = 0;
			} else {
				tn.tv_sec = drv_hztomsec(nb->nb_ticks) / 1000;
				tn.tv_usec = (drv_hztomsec(nb->nb_ticks) % 1000) * 1000;
			}
#ifdef __LP64__
			if (ioc->ioc_flag == IOC_ILP32) {
				struct timeval32 *tv;

				db = mi_copyout_alloc(q, mp, NULL, sizeof(*tv), 1);
				if (likely(db != NULL)) {
					tv = (typeof(tv)) db->b_rptr;
					tv->tv_sec = tn.tv_sec;
					tv->tv_usec = tn.tv_usec;
				}
			} else
#endif				/* __LP64__ */
			{
				struct timeval *tv;

				db = mi_copyout_alloc(q, mp, NULL, sizeof(*tv), 1);
				if (likely(db != NULL)) {
					tv = (typeof(tv)) db->b_rptr;
					tv->tv_sec = tn.tv_sec;
					tv->tv_usec = tn.tv_usec;
				}
			}
			if (likely(db != NULL))
				mi_copyout(q, mp);
			break;
		}
		case _IOC_NR(NIOCCTIME):
			if (nbuf_stropts(nb, 0) != 0) {
				mi_copy_done(q, mp, ENOSR);
				break;
			}
			if (nb->nb_ticks > 0)
				mi_timer_cancel(nb->nb_timer);
			nb->nb_ticks = 0;
			nb->nb_chunk = 0;
			if (qsize(nb->nb_rq) > 0)
				qenable(nb->nb_rq);
			mi_copy_done(q, mp, 0);
			break;
		case _IOC_NR(NIOCSCHUNK):
			mi_copyin(q, mp, NULL, sizeof(uint));
			break;
		case _IOC_NR(NIOCGCHUNK):
			db = mi_copyout_alloc(q, mp, NULL, sizeof(uint), 1);
			if (likely(db != NULL))
				*(uint *) db->b_rptr = nb->nb_chunk;
			if (likely(db != NULL))
				mi_copyout(q, mp);
			break;
		default:
			putnext(q, mp);
			break;
		}
		break;
	default:
		putnext(q, mp);
		break;
	}
}

STATIC noinline __unlikely fastcall void
nbuf_iocdata(queue_t *q, mblk_t *mp)
{
	struct nb *nb = (typeof(nb)) q->q_ptr;
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp, *db;

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case _IOC_NR(NIOC):
		switch (_IOC_NR(cp->cp_cmd)) {
		case _IOC_NR(NIOCSTIME):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EFAULT);
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
			{
				struct timeval tn;

#ifdef __LP64__
				if (cp->cp_flag == IOC_ILP32) {
					struct timeval32 *tv = (typeof(tv)) dp->b_rptr;

					tn.tv_sec = tv->tv_sec;
					tn.tv_usec = tv->tv_usec;
				} else
#endif				/* __LP64__ */
				{
					struct timeval *tv = (typeof(tv)) dp->b_rptr;

					tn.tv_sec = tv->tv_sec;
					tn.tv_usec = tv->tv_usec;
				}
				if (tn.tv_sec == 0 && tn.tv_usec == 0)
					nb->nb_ticks = 0;
				else {
					nb->nb_ticks = 0;
					nb->nb_ticks += drv_msectohz(tn.tv_sec * 1000);
					nb->nb_ticks += drv_usectohz(tn.tv_usec);
					if (nb->nb_ticks <= 0)
						nb->nb_ticks = 1;
				}
				mi_copy_done(q, mp, 0);
				break;
			}
			}
			break;
		case _IOC_NR(NIOCSCHUNK):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
				if (nbuf_stropts(nb, *(uint *) dp->b_rptr) != 0) {
					mi_copy_done(q, mp, ENOSR);
					break;
				}
				if (*(uint *) dp->b_rptr > 65536) {
					db = mi_copyout_alloc(q, mp, NULL, sizeof(uint), 1);
					if (unlikely(db == NULL))
						break;
					*(uint *) db->b_rptr = 65536;
					nb->nb_chunk = 65536;
					mi_copyout(q, mp);
					break;
				}
				nb->nb_chunk = *(uint *) dp->b_rptr;
				mi_copy_done(q, mp, 0);
				break;
			case MI_COPY_CASE(MI_COPY_OUT, 1):
				mi_copy_done(q, mp, 0);
				break;
			default:
				mi_copy_done(q, mp, EFAULT);
				break;
			}
			break;
		case _IOC_NR(NIOCCTIME):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EFAULT);
				break;
			}
			break;
		case _IOC_NR(NIOCGTIME):
		case _IOC_NR(NIOCGCHUNK):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			case MI_COPY_CASE(MI_COPY_OUT, 1):
				mi_copy_done(q, mp, 0);
				break;
			default:
				mi_copy_done(q, mp, EFAULT);
				break;
			}
			break;
		default:
			putnext(q, mp);
			break;
		}
	default:
		putnext(q, mp);
		break;
	}
}

/*
 *  -----------------------------------------------------------------------------------
 *
 *  Read-Side procedures.
 *
 *  -----------------------------------------------------------------------------------
 *  Read-Side Behavior
 *	nit_buf collects incoming M_DATA and M_PROTO messages into chunks, passing each
 *	chunk upward when either the chunk becomes full or the current read timeout
 *	expires.  When a message arrives, it is processed for inclusion in a chunk, and
 *	then it is added to the current chunk.  The following paragraphs discuss each step
 *	in turn.
 *
 *	Upon receiving a message from below, nit_buf immediately converts all leading
 *	M_PROTO message blocks int the message to M_DATA message blocks, altering only the
 *	message type field and leaving the contents alone.  It then prepends a header to
 *	the converted message.  This header is defined as follows:
 *
 *	    struct nit_bufhdr {
 *		uint nhb_msglen;
 *		uint nhb_totlen;
 *	    };
 *
 *	The first field of the header gives the length in bytes of the converted message.
 *	The second field gives the distance in bytes from the start of the message in the
 *	current chunk (described below) to the start of the next message in the chunk; the
 *	value reflects any padding necessary to ensure correct data alignment for the host
 *	machine and includes the length of the header itself.
 *
 *	After preparing a message, nit_buf attempts to add it to the end of the current
 *	chunk, using the chunk size and timeout values to govern the addition.  (The chunk
 *	size and timeout values are set and inspected using the input-output control calls
 *	described below.)  If adding the new message would make the current chunk grow
 *	larger than the chunk size, nit_buf closes off the current chunk, passing it up to
 *	the next module in line, and starts a new chunk, seeding it with a zero-length
 *	message.  If adding the message would still make the current chunk overflow, the
 *	module passes it upward in an over-size chunk of its own.  Otherwise, the module
 *	concatenates the message to the end of the current chunk.
 *
 *	To ensure that messages do not languish forever in an accumulating chunk, nit_buf
 *	maintains a read timeout.  Whenever this timeout expires, the module closes off
 *	the current chunk, regardless of its length, and passes it upward; if no incoming
 *	messages have arrived, the chunk passed upward will have zero length.  Whenever
 *	the module passes a chunk upward, it restarts the timeout period.  These tow rules
 *	ensure that nit_buf minimizes the number of chunks it produces during periods of
 *	intense message activity and that it periodically disposes of all messages during
 *	slack intervals.
 *
 *	nit_buf handles other message types as follows.  Upon receiving a M_FLUSH message
 *	specifying that the read queue be flushed, the module does so, clearing the
 *	currently accumulating chunk as well, and passes the message on to the module or
 *	driver above.  It passes al other messages through unaltered to its upper
 *	neighbour.
 */

/** nbuf_adjsize: - size a message
  * @nb: private structure
  * @mp: message to size
  * @nhb: pseudo SNIT buffer header
  * @epp: (return) pointer to last block in message block chain
  *
  * Calculate original length and message length and return pointer to last message block
  * in message block chain.  In the Sun module this is performed using separate calls to
  * msgdsize(9), adjmsg(9) and walking the buffer chain.  Not only is this more efficient,
  * it will work better when an M_PROTO or other message block slips into the middle of
  * the chain somewhere.
  *
  * Note: if the size of the message that is about to be placed on the queue would cause
  * flow control to be invoked for the queue, simply drop the message.  The reason for
  * this is that we never want the read queue flow controlled downstream: otherwise, the
  * driver could start dropping messages without our knowing about it.  This tells the
  * driver that everything is flowing nicely upstream and gets it to deliver each and
  * every message to us.
  */
static inline fastcall __hot_get int
nbuf_adjsize(const struct nb *nb, mblk_t *mp, struct nit_bufhdr *nhb, mblk_t **epp)
{
	register mblk_t *b, *e;
	register ssize_t mlen, dlen;
	ssize_t tlen;
	queue_t *q = nb->nb_rq;

	prefetch(q);
	for (mlen = 0, e = b = mp; b != NULL; e = b, b = b->b_cont) {
		if ((dlen = b->b_wptr - b->b_rptr) > 0)
			mlen += dlen;
		else
			b->b_wptr = b->b_rptr;
	}
	tlen = sizeof(*nhb) + ((mlen + (sizeof(ulong) - 1)) & ~(sizeof(ulong) - 1));
	if (q->q_count + tlen >= q->q_hiwat)
		return (-EBUSY);
	nhb->nhb_msglen = mlen;
	nhb->nhb_totlen = tlen;
	*epp = e;
	return (0);
}


/** nbuf_padmsg:- add padding to a message.
  * @nb: private structure
  * @nhb: pseudo SNIT buffer header
  * @ep: end pointer (last message block in chain)
  *
  * When headers are applied to messages (which is always), messages are padded so that
  * the message header is always properly aligned in the user buffer.  This is
  * accomplished by padding the message length to the size of a ulong.
  *
  * An attempt is made to use any additional space at the end of an existing message block
  * (the last block in the message chain is pointed to by ep).  For normal operation, this
  * should always succeed.  It can be the case that the data block is shared (this is form
  * monitoring after all and duping message blocks is one easy way to do that in the
  * driver).  When the data block is shared we do not zero the padding.  The header
  * identifies the padding anyway (nhb.nhb_totlen-sizeof(nhb)-nhb.nhb_msglen).
  *
  * In the sad event that we cannot use the end of the existing data block, we allocate a
  * new block and link it to the end of the message chain.
  */
STATIC noinline fastcall __hot_get int
nbuf_padmsg(const struct nb *nb, struct nit_bufhdr *nhb, mblk_t *ep)
{
	ssize_t pad;
	ssize_t plen;
	mblk_t *pp;

	plen = nhb->nhb_totlen - sizeof(*nhb);
	if (likely((pad = plen - nhb->nhb_msglen) > 0)) {
		if (likely(ep->b_datap->db_lim >= ep->b_wptr + pad)) {
			if (ep->b_datap->db_ref == 1)
				bzero(ep->b_wptr, pad);
			ep->b_wptr += pad;
		} else {
			if (unlikely((pp = allocb(pad, BPRI_MED)) == NULL))
				goto enosr;
			bzero(pp->b_rptr, pad);
			pp->b_wptr = pp->b_rptr + pad;
			ep->b_cont = pp;
			ep = pp;
		}
	}
	return (0);
      enosr:
	return (-ENOSR);
}

/** nbuf_addheader: - add a SNIT buffer header to the packet
  * @nb: private structure
  * @nhb: pseudo SNIT buffer header
  * @mpp: (return) pointer to first block in message
  *
  * This function simply adds a SNIT buffer header to the packet.  To avoid unnecessary
  * message block allocation calls, an attempt is made to see whether the SNIT buffer
  * header (struct nit_bufhdr) can fit into the first data block and whether the first
  * data block can be written.  If the header can fit in the first block without
  * adjustment, or the amount of data that needs to be moved is not too excessive, the
  * first block is adjusted to include the header.  Otherwise, a message block is
  * allocated just for the header.
  */
STATIC noinline fastcall __hot_get int
nbuf_addheader(const struct nb *nb, struct nit_bufhdr *nhb, mblk_t **mpp)
{
	static const size_t size = sizeof(struct nit_bufhdr);
	mblk_t *mp = (*mpp);
	dblk_t *db = mp->b_datap;
	ssize_t blen, dlen;
	unsigned char *dest;
	mblk_t *bp;

	if (unlikely(mp->b_rptr > mp->b_wptr))
		mp->b_wptr = mp->b_rptr;

	blen = mp->b_wptr - mp->b_rptr;
	dlen = db->db_lim - db->db_base;
	dest = db->db_base + size;

	/* will the header fit at the front of the block? */
	if ((db->db_ref == 1) && (mp->b_rptr - db->db_base >= size)) {
		mp->b_rptr -= size;
	} else
		/* will the header fit by moving the data to the end of the block? */
	if ((db->db_ref == 1) && (blen < (FASTBUF << 2)) && (dlen - blen >= size)) {
		if (likely(dest != mp->b_rptr)) {
			bcopy(mp->b_rptr, dest, blen);
			mp->b_rptr = db->db_base;
			mp->b_wptr = dest + blen;
		}
	} else {
		if (unlikely((bp = allocb(size, BPRI_MED)) == NULL))
			goto enosr;
		bzero(bp->b_rptr, size);
		bp->b_wptr = bp->b_rptr + size;
		bp->b_cont = mp;
		*mpp = mp = bp;
	}

	*(typeof(nhb)) mp->b_rptr = *nhb;

	mp->b_flag |= MSGHEADER;

	return (0);
      enosr:
	return (-ENOSR);
}

/** nbuf_msgadjust: - size and truncate a message remembering the last block
  * @nb: private structure
  * @mpp: the message to manipulate
  *
  * This function uses the functions, above, to time-stamp the message (when headers are required),
  * truncate the message to the snapshot length, pad the message for packet headers, and prepends
  * the packet header.  The result is the message as it would appear in the user buffer.  This
  * function is only used by the read put procedure, and, although it needs write access to the
  * message, it has read-only access to the private structure.
  */
static inline fastcall __hot_get int
nbuf_msgadjust(const struct nb *nb, mblk_t **mpp)
{
	mblk_t *ep;
	struct nit_bufhdr nhb = { 0, };

	if (nbuf_adjsize  (nb, (*mpp), &nhb, &ep) != 0) return (-EBUSY);
	if (nbuf_padmsg   (nb,         &nhb, ep)  != 0) return (-ENOSR);
	if (nbuf_addheader(nb,         &nhb, mpp) != 0) return (-ENOSR);
	return (0);
}


/*
 *  -----------------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -----------------------------------------------------------------------------------
 */

/** nbuf_wput: - write put procedure.
  * @q: write queue.
  * @mp: message to put.
  *
  * This is largely a canonical write put procedure with expedited M_IOCTL/M_IOCDATA processing.
  * The only difference from the normal is that when M_READ messages are received, we
  * cancel the chunk timer and explicitly enable the read queue to deliver accumulated
  * M_DATA messages.
  */
STATIC streamscall int
nbuf_wput(queue_t *q, mblk_t *mp)
{
	struct nb *nbw = (typeof(nbw)) q->q_ptr;
	const struct nb *nb = nbw;
	unsigned char db_type = mp->b_datap->db_type;

	switch (__builtin_expect(db_type, M_PROTO)) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		break;
	case M_IOCTL:
		nbuf_ioctl(q, mp);
		break;
	case M_IOCDATA:
		nbuf_iocdata(q, mp);
		break;
	case M_READ:
		if (nb->nb_ticks > 0)
			mi_timer_cancel(nbw->nb_timer);
		putq(RD(q), mp);
		break;
	default:
		if (db_type >= QPCTL || (q->q_first == NULL && !(q->q_flag & QSVCBUSY)
					 && bcanputnext(q, mp->b_band))) {
			putnext(q, mp);
			break;
		}
		if (unlikely(putq(q, mp) == 0)) {
			mp->b_band = 0;
			(void) putq(q, mp);	/* must succeed */
		}
		break;
	}
	return (0);
}

/** nbuf_wsrv: - write service procedure.
  * @q: the write queue.
  *
  * This is a canonical write service procedure.  It simply passes messages along under flow
  * control.  A check must be made for M_IOCTL and M_IOCDATA because the mi_copyin functions
  * sometimes places one of these messages on the queue.
  */
STATIC streamscall __hot_read int
nbuf_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		unsigned char db_type = mp->b_datap->db_type;

		switch (__builtin_expect(db_type, M_PROTO)) {
		case M_IOCTL:
			nbuf_ioctl(q, mp);
			continue;
		case M_IOCDATA:
			nbuf_iocdata(q, mp);
			continue;
		default:
			if (unlikely(db_type >= QPCTL) || likely(bcanputnext(q, mp->b_band))) {
				putnext(q, mp);
				continue;
			}
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/** nbuf_msgsizelast: -
  * @mp: the message to size
  * @mlen: (return) message length
  * @epp: (return) last message block in chain
  */
static inline streams_fastcall void
nbuf_msgsizelast(mblk_t *mp, int *mlen, mblk_t **epp)
{
	register mblk_t **b, *e;
	register int len;

	for (len = 0, b = &mp, e = (*b); (*b) != NULL; b = &((*b)->b_cont)) {
		e = (*b);
		if (e->b_wptr > e->b_rptr)
			len += e->b_wptr - e->b_rptr;
	}
	*mlen = len;
	*epp = e;
}

/** nbuf_rsrv: - read service procedure.
  * @q: the read queue
  *
  * Aside from the message chunking, this is a canonical service procedure.  The read
  * queue runs with noenable(9) set, meaning that this service procedure will only run for
  * normal priority data when back-enabled or explicitly requested to be run with
  * qenable(9).  The logic is as follows:
  *
  * M_DATA:
  *	When the chunk size is zero, process each M_DATA packet as we would normally
  *	process an M_DATA message: reading the queue and processing messages until they
  *	are exhausted or we encounter flow control.  This is also true of the first
  *	message of a timeout period when SB_DEFER_CHUNK is requested.
  *
  *	When the chunk size is non-zero, we concatenate M_DATA messages together until the
  *	chunk size is or would be exceeded, or another message type is encountered in the
  *	stream.  We then treat these concatenated M_DATA messages like we would treat any
  *	other in a service procedure, subjected to flow control and the like.
  *
  *	When subject to flow control, the time has come to send M_DATA and the upper
  *	modules or user is simply not keeping up.  Instead of dropping messages, we exert
  *	flow control back-pressure instead by filling our own queue.  The back-pressure
  *	approach would causes messages to be dropped by the driver, where they can be
  *	counted.
  *
  *	We keep the number of drops plus the number of messages in the b_csum field of the
  *	first message block in the chain.  This allows us to calculate from any given
  *	message or concatenated message removed from the queue, the number of messages
  *	plus drops that it represents.  When discarding concatenated messages, this allows
  *	use to form a proper count of the number of drops.
  *
  *	The chunk size is read at the onset of the routine so that it cannot change during
  *	the course of the function.
  */
STATIC streamscall __hot_read int
nbuf_rsrv(queue_t *q)
{
	struct nb *nb = (typeof(nb)) q->q_ptr;
	long chunk = nb->nb_chunk;
	mblk_t *mp, *mr = NULL;
	int partial_ok = 1;

	while ((mp = getq(q)) != NULL) {
		switch (__builtin_expect(mp->b_datap->db_type, M_DATA)) {
		case M_DATA:
			if (chunk != 0) {
				mblk_t *lp;
				int mlen;

				nbuf_msgsizelast(mp, &mlen, &lp);
				if (mlen < chunk) {
					mblk_t *dp;

					while ((dp = getq(q))) {
						if (likely(dp->b_datap->db_type == M_DATA)) {
							mblk_t *ep;
							int dlen;

							nbuf_msgsizelast(dp, &dlen, &ep);
							if (mlen + dlen <= chunk) {
								/* chunk it up */
								mlen += dlen;
								lp->b_cont = dp;
								lp = ep;
								mp->b_csum += dp->b_csum;
								continue;
							}
						}
						putbq(q, dp);
						break;
					}
					/* already sent one - stop here */
					if (dp == NULL && !partial_ok) {
						if (nb->nb_ticks > 0)
							mi_timer_ticks(nb->nb_timer, nb->nb_ticks);
						break;
					}
					/* nothing left - let it go */
				}
				partial_ok = 0;
			}
			if (nb->nb_sdrops != 0) {
				mp->b_csum += nb->nb_sdrops;
				nb->nb_sdrops = 0;
			}
			if (canputnext(q)) {
				mp->b_flag |= MSGDELIM;
				putnext(q, mp);
				if (mr != NULL) {
					/* Transform to zero-length delimited M_DATA to
					   unblock the read at the Stream head. */
					mr->b_datap->db_type = M_DATA;
					mr->b_flag |= MSGDELIM;
					mr->b_wptr = mr->b_rptr;
					putnext(q, mr);
					mr = NULL;
				}
				continue;
			}
			/* flow controlled, either drop or propagate flow control */
			if (0) {
				nb->nb_sdrops += mp->b_csum;
				freemsg(mp);
				continue;
			}
			break;

		case M_PCSIG:
			mi_timer_valid(mp);
			continue;

		case M_READ:
			/* the write queue puts these here */
			chunk = *(long *) mp->b_rptr;
			mr = mp;
			continue;

		default:
			if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
				putnext(q, mp);
				continue;
			}
			break;
		}
		putbq(q, mp);
		break;
	}
	if (mr != NULL)
		freemsg(mr);
	return (0);
}

/*
 * nit_buf collects incoming M_DATA and M_PROTO messages into chunks, passing
 * each chunk upward when either the chunk becomes full or the current read
 * timeout expires.  When a message arrives, it is processed for inclusion in a
 * chunk, and then it is added to the current chunk.
 *
 * Upon receiving a message from below, nit_buf immediately converts all leading
 * M_PROTO message blocks in the message to M_DATA message blocks, altering only
 * the type field and leaving the contents alone.
 */
STATIC streamscall __hot_get int
nbuf_rput(queue_t *q, mblk_t *mp)
{
	struct nb *nbw = (typeof(nbw)) q->q_ptr;
	const struct nb *nb = nbw;
	unsigned char db_type = mp->b_datap->db_type;

	switch (__builtin_expect(db_type, M_DATA)) {
	case M_PROTO:
		mp->b_datap->db_type = M_DATA;
		/* fall through */

	case M_DATA:
		if (nbuf_msgadjust(nb, &mp) == 0) {
			mp->b_band = 0;
			mp->b_csum = 1 + XCHG(&nbw->nb_pdrops, 0);

			if (q->q_count == 0) {
				if (nb->nb_ticks > 0)
					mi_timer_ticks(nbw->nb_timer, nb->nb_ticks);
			}

			putq(q, mp);

			if (nb->nb_chunk > 0 && q->q_count > nb->nb_chunk) {
				if (nb->nb_ticks > 0)
					mi_timer_cancel(nbw->nb_timer);
				qenable(q);
				break;
			}
			break;
		}
		nbw->nb_pdrops++;
		freemsg(mp);
		break;

	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if ((mp->b_rptr[0] & FLUSHBAND) && mp->b_rptr[1] != 0)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else {
				flushq(q, FLUSHDATA);

				if (nb->nb_ticks > 0)
					mi_timer_cancel(nbw->nb_timer);

				nbw->nb_pdrops = 0;
				nbw->nb_sdrops = 0;
			}
		}
		putnext(q, mp);
		break;

	default:
		if (db_type >= QPCTL || (q->q_first == NULL && !(q->q_flag & QSVCBUSY) &&
					 bcanputnext(q, mp->b_band))) {
			putnext(q, mp);
			break;
		}
		if (unlikely(putq(q, mp) == 0)) {
			mp->b_band = 0;
			putq(q, mp);	/* this must succeed */
		}
		if (mp->b_band == 0) {
			if (nb->nb_ticks > 0)
				mi_timer_cancel(nbw->nb_timer);
			qenable(q);
		}
		break;
	}
	return (0);
}


/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
nbuf_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;
	struct nb *nb;
	mblk_t *tb, *mp;
	struct stroptions *so;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag != MODOPEN)
		return (ENXIO);
	if ((tb = mi_timer_alloc(0)) == NULL)
		return (ENOSR);
	if ((mp = allocb(sizeof(*so), BPRI_WAITOK)) == NULL) {
		mi_timer_free(tb);
		return (ENOSR);
	}
	write_lock(&nb_lock);
	if ((err = mi_open_comm(&nb_opens, sizeof(*nb), q, devp, oflag, sflag, crp))) {
		write_unlock(&nb_lock);
		freeb(mp);
		mi_timer_free(tb);
		return (err);
	}
	/* inherit packet sizes of what we are pushed over */
	wq = WR(q);
	wq->q_minpsz = wq->q_next->q_minpsz;
	wq->q_maxpsz = wq->q_next->q_maxpsz;

	nb = (typeof(nb)) q->q_ptr;
	nb->nb_rq = q;
	nb->nb_ticks = -1;
	nb->nb_chunk = NB_DFLT_CHUNK;
	nb->nb_timer = tb;
	nb->nb_pdrops = 0;
	nb->nb_sdrops = 0;

	write_unlock(&nb_lock);

	mp->b_datap->db_type = M_SETOPTS;
	mp->b_wptr = mp->b_rptr + sizeof(*so);
	bzero(mp->b_wptr, sizeof(*so));
	so = (typeof(so)) mp->b_rptr;

	so->so_flags = 0;
	so->so_flags |= SO_MREADON;

	so->so_lowat = SHEADLOWAT;	/* SHEADLOWAT = 8192 */
	so->so_lowat = SNIT_LOWAT(NB_DFLT_CHUNK, 1);	/* 32767 + 256 */
	so->so_flags |= SO_LOWAT;

	so->so_hiwat = SHEADHIWAT;	/* SHEADHIWAT = 65536 */
	so->so_hiwat = SNIT_HIWAT(NB_DFLT_CHUNK, 1);	/* 65536 + 512 */
	so->so_flags |= SO_HIWAT;

	qprocson(q);
	noenable(q);
	return (0);
}

STATIC streamscall int
nbuf_close(queue_t *q, int oflag, cred_t *crp)
{
	struct nb *nb = (typeof(nb)) q->q_ptr;

	(void) oflag;
	(void) crp;
	if (!q->q_ptr)
		return (ENXIO);
	qprocsoff(q);
	if (nb->nb_timer != NULL)
		mi_timer_free(XCHG(&nb->nb_timer, NULL));
	write_lock(&nb_lock);
	mi_close_comm(&nb_opens, q);
	write_unlock(&nb_lock);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
STATIC struct qinit nbuf_rinit = {
	.qi_putp = nbuf_rput,
	.qi_srvp = nbuf_rsrv,
	.qi_qopen = nbuf_open,
	.qi_qclose = nbuf_close,
	.qi_minfo = &nbuf_minfo,
	.qi_mstat = &nbuf_rstat,
};

STATIC struct qinit nbuf_winit = {
	.qi_putp = nbuf_wput,
	.qi_srvp = nbuf_wsrv,
	.qi_minfo = &nbuf_minfo,
	.qi_mstat = &nbuf_wstat,
};

#ifdef __LP64__
struct nbuf_trans {
	unsigned int cmd;
	void *opaque;
};

STATIC struct nbuf_trans nbuf_trans_map[] = {
	{.cmd = NIOCSTIME,}
	, {.cmd = NIOCGTIME,}
	, {.cmd = NIOCCTIME,}
	, {.cmd = NIOCSCHUNK,}
	, {.cmd = NIOCGCHUNK,}
	, {.cmd = 0,}
};

STATIC __unlikely void
nbuf_ioctl32_unregister(void)
{
	struct nbuf_trans *t;

	for (t = nbuf_trans_map; t->cmd != 0; t++) {
		unregister_ioctl32(t->opaque);
		t->opaque = NULL;
	}
	return;
}


STATIC __unlikely int
nbuf_ioctl32_register(void)
{
	struct nbuf_trans *t;

	for (t = nbuf_trans_map; t->cmd != 0; t++) {
		if ((t->opaque = register_ioctl32(t->cmd)) == NULL) {
			nbuf_ioctl32_unregister();
			return (-ENOMEM);
		}
	}
	return (0);
}
#endif				/* __LP64__ */

#ifdef CONFIG_STREAMS_NBUF_MODULE
STATIC
#endif
struct streamtab nbufinfo = {
	.st_rdinit = &nbuf_rinit,
	.st_wrinit = &nbuf_winit,
};

STATIC struct fmodsw nbuf_fmod = {
	.f_name = CONFIG_STREAMS_NBUF_NAME,
	.f_str = &nbufinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_NBUF_MODULE
STATIC int
#else
int __init
#endif
nbufinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_NBUF_MODULE
	printk(KERN_INFO NBUF_BANNER);
#else
	printk(KERN_INFO NBUF_SPLASH);
#endif
	nbuf_minfo.mi_idnum = modid;
#ifdef __LP64__
	if ((err = nbuf_ioctl32_register())) {
		cmn_err(CE_WARN, "%s: could not register 32-bit ioctls, err = %d",
			CONFIG_STREAMS_NBUF_NAME, err);
		return (err);
	}
#endif				/* __LP64__ */
	if ((err = register_strmod(&nbuf_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d",
			CONFIG_STREAMS_NBUF_NAME, err);
#ifdef __LP64__
		nbuf_ioctl32_unregister();
#endif				/* __LP64__ */
		return (err);
	}
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_NBUF_MODULE
STATIC void
#else
void __exit
#endif
nbufexit(void)
{
	int err;

#ifdef __LP64__
	nbuf_ioctl32_unregister();
#endif				/* __LP64__ */
	if ((err = unregister_strmod(&nbuf_fmod)) < 0)
		return (void) (err);
	return (void) (0);
}

#ifdef CONFIG_STREAMS_NBUF_MODULE
module_init(nbufinit);
module_exit(nbufexit);
#endif
