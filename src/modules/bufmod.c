/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 *  This is BUFMOD a classical STREAMS buffering module that performs the actions described in
 *  bufmod(4).  This used to be the Linux Fast-STREAMS test module of the same name, but that has
 *  been renamed to SRVMOD.
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include <linux/autoconf.h>
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/strsun.h>
#include <sys/strconf.h>

#include <sys/bufmod.h>

#include "sys/config.h"

#define BUFMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define BUFMOD_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define BUFMOD_REVISION		"Lfs $RCSfile$ $Name$($Revision$) $Date$"
#define BUFMOD_DEVICE		"SVR 4.2 Buffer Module (BUFMOD) for STREAMS"
#define BUFMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define BUFMOD_LICENSE		"GPL"
#define BUFMOD_BANNER		BUFMOD_DESCRIP		"\n" \
				BUFMOD_COPYRIGHT	"\n" \
				BUFMOD_REVISION		"\n" \
				BUFMOD_DEVICE		"\n" \
				BUFMOD_CONTACT		"\n"
#define BUFMOD_SPLASH		BUFMOD_DEVICE		" - " \
				BUFMOD_REVISION		"\n"

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
MODULE_AUTHOR(BUFMOD_CONTACT);
MODULE_DESCRIPTION(BUFMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(BUFMOD_DEVICE);
MODULE_LICENSE(BUFMOD_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_BUFMOD_MODULE */

#ifndef CONFIG_STREAMS_BUFMOD_NAME
//#define CONFIG_STREAMS_BUFMOD_NAME "bufmod"
#error "CONFIG_STREAMS_BUFMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_BUFMOD_MODID
//#define CONFIG_STREAMS_BUFMOD_MODID 13
#error "CONFIG_STREAMS_BUFMOD_MODID must be defined."
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-bufmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_BUFMOD_MODULE
static
#endif
modID_t modid = CONFIG_STREAMS_BUFMOD_MODID;

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for BUFMOD.");
#endif				/* CONFIG_STREAMS_BUFMOD_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_BUFMOD_MODID));
MODULE_ALIAS("streams-module-bufmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info bufmod_minfo = {
	.mi_idnum = CONFIG_STREAMS_BUFMOD_MODID,
	.mi_idname = CONFIG_STREAMS_BUFMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat bufmod_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat bufmod_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  Private Structure
 *
 *  -------------------------------------------------------------------------
 */

/** struct sb: - STREAMS buffer private structure.
  * @sb_rq: the module read queue (write once, read only)
  * @sb_chunk: chunk size.  Written by ioctl and read by everything else.
  * @sb_ticks: timeout.  Written by ioctl and read by everything else.
  * @sb_timer: timeout id. Read and written by timer functions.
  * @sb_pdrops: dropped packets.  Read and written by read put procedure only.
  * @sb_sdrops: dropped packets.  Read and written by read service procedure only.
  * @sb_snap: snapshot size.  Written by ioctl and read otherwise.
  * @sb_flags: user flags.  Written by ioctl and read otherwise.
  * @sb_state: internal state. Written by read service, read otherwise.
  */
struct sb {
	queue_t *sb_rq;			/* read queue */
	uint32_t sb_chunk;		/* chunk size */
	uint32_t sb_ticks;		/* number of ticks for timeout */
	mblk_t *sb_timer;		/* mi_timer(9) message block */
	uint32_t sb_pdrops;		/* number of rput dropped packets */
	uint32_t sb_sdrops;		/* number of rsrv dropped packets */
	uint32_t sb_snap;		/* snapshot length */
	uint32_t sb_flags;		/* user option flags */
	uint32_t sb_state;		/* internal state flags */
	mblk_t *sb_mread;		/* last M_READ message */
};

#define SB_USER		(SB_SEND_ON_WRITE\
			|SB_NO_PROTO_CVT\
			|SB_DEFER_CHUNK\
			|SB_NO_HEADER\
			|SB_NO_DROPS\
			|SB_HIPRI_OOB)

#define MSGHEADER	(1<<15)

#ifdef RW_LOCK_UNLOCKED
static rwlock_t sb_lock = RW_LOCK_UNLOCKED;
#elif defined __RW_LOCK_UNLOCKED
static rwlock_t sb_lock = __RW_LOCK_UNLOCKED(&sb_lock);
#else
#error cannot initialize read-write locks
#endif
static caddr_t sb_opens;

/*
 * The correction factor is introduced to compensate for whatever assumptions
 * the modules below have made about how much traffic is flowing through the
 * stream and the fact that bufmod may be snipping messages with the sb_snap
 * length.
 */
#define	SNIT_HIWAT(msgsize, fudge)	((4 * msgsize * fudge) + 512)
#define	SNIT_LOWAT(msgsize, fudge)	((2 * msgsize * fudge) + 256)

#define BUFMOD_HIWAT(chunksize, snapshot) \
	 SNIT_HIWAT(chunksize, snapshot > 0 ? snapshot <= 100 ? 4 : snapshot <= 400 ? 2 : 1 : 1)
#define BUFMOD_LOWAT(chunksize, snapshot) \
	 SNIT_LOWAT(chunksize, snapshot > 0 ? snapshot <= 100 ? 4 : snapshot <= 400 ? 2 : 1 : 1)


/** bufmod_stropts: - set Stream head options
  * @sb: private structure
  * @chunk: the chunk size to set
  * @snap: the snapshot size to set
  *
  * This function sets the Stream head options in accordance with the chunk and
  * snapshot sizes provided.  The fudge calculations for low and high water
  * marks are taken from the Solaris implementation.
  *
  * We set SO_MREADON, SO_NDELOFF, and SO_DELIM when the chunk size is non-zero.
  * We also set the read mode to RFILL.  Here are some reasons for doing this:
  *
  * Setting SO_NDELON (O_NONBLOCK) on the Stream head is rather pointless when
  * we are chunking.  The usage is poll() read(1) poll() read(1).  When poll()
  * succeeds, there might be only one packet at the Stream head.  With RFILL,
  * SO_NDELOFF, SO_MREAD and SO_DELIM, the read() operation will block and send
  * an M_READ message with the remaining buffer size downstream.  This module
  * can then deliver-up any messages that are still available and terminate the
  * read as a short read after delivering them.
  *
  * When the chunk size is zero, we wet SO_MREADOFF, SO_NDELON, SO_NODELIM and
  * read mode to RMSGN.
  */
static noinline fastcall __unlikely int
bufmod_stropts(struct sb *sb, int chunk, int snap)
{
	mblk_t *mp;
	struct stroptions *so;
	int effective_chunk;

	/* for the purposes of this calculation */
	if ((effective_chunk = chunk) < 4096)
		effective_chunk = 4096;
	if (effective_chunk > 65536)
		effective_chunk = 65536;

	if (likely((mp = allocb(sizeof(*so), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_SETOPTS;
		mp->b_wptr = mp->b_rptr + sizeof(*so);
		bzero(mp->b_rptr, sizeof(*so));
		so = (typeof(so)) mp->b_rptr;
		so->so_flags = 0;
		if (chunk < 0) {
			//so->so_readopt = RMSGN; /* XXX RFILL? */
			//so->so_flags |= SO_READOPT;
			so->so_flags |= SO_MREADON;
		} else {
			//so->so_readopt = RNORM;
			//so->so_flags |= SO_READOPT;
			so->so_flags |= SO_MREADOFF;
		}
		so->so_flags = (SO_LOWAT|SO_HIWAT);
		so->so_lowat = BUFMOD_LOWAT(effective_chunk, snap);
		if (so->so_lowat < SHEADLOWAT)
			so->so_lowat = SHEADLOWAT;
		so->so_flags |= SO_LOWAT;
		so->so_hiwat = BUFMOD_HIWAT(effective_chunk, snap);
		if (so->so_hiwat < SHEADHIWAT)
			so->so_hiwat = SHEADHIWAT;
		so->so_flags |= SO_HIWAT;
		putnext(sb->sb_rq, mp);
		return (0);
	}
	return (-ENOSR);
}
/*
 *  -------------------------------------------------------------------------
 *
 *  IOCTL Processing
 *
 *  -------------------------------------------------------------------------
 *  SBIOCSTIME:
 *	This input-output control is used to set the maximum interval of time that the bufmod(4)
 *	module will await messages completing a chunk before delivering the incomplete chunk.  When
 *	specified as zero (0), bufmod(4) will interpret the value as meaning an indefinitely long
 *	period of time, defeating the timeout mechanism.  That is, all chunks will be held unitl
 *	they are complete, or another mechainsm (e.g. write side message, read-side M_PCPROTO
 *	message) cause the delivery of the chunk. arg is a pointer to a (user-native sized) timeval
 *	structure specifying the duration of time to wait. See gettimeofday(2) for a description of
 *	the timeval structure.
 *
 *  SBIOCGTIME:
 *	This input-output control is used to retrieve the maximum interval of time that the bufmod
 *	will await messages completing a chunk before delivering an incomplete chunk. This is the
 *	value that was set with SBIOCSTIME or cleared with SBIOCCTIME.  arg is a pointer to a
 *	(user-native sized) timeval structure into which to retrieve the current duration of time to
 *	wait.  See gettimeofday(2) for a description of the timeval structure.  Note that any
 *	non-zero value that was set will have been rounded up to the next clock tick.
 *
 *  SBIOCCTIME:
 *	This input-output control clears to zero (0) the maximum interval of time that the bufmod
 *	module will await messages completing a chunk before delivering an incomplete chunk.  The
 *	value of zero (0) is interpreted by bufmod(4) to mean an indefinitely long period of time
 *	and, therefore, this value defeats the time period mechanism altogether.  Note that clearing
 *	the timeout to zero (0) using this input-output control will also force the chunk size to
 *	zero (0), defeating the buffering mechanism altogether.  The state of the buffer flags;
 *	however, remains the same (i.e. headers are prepended or M_PROTO(9) blocks converted as
 *	before.
 *
 *  SBIOCSCHUNK:
 *	This input-output control is used to set the chunk size.  The chunk size is the maximum size
 *	to which bufmod(4) will coalesce packets.  Any packet that, when added to an existing,
 *	partially filled chunk, will increase its size beyond this maximum will be delivered before
 *	further considering the received packet.  When set to zero (0), chunk accumulation is
 *	effectively disabled and each packet is delivered as soon as possible.  arg is a pointer to
 *	an unsigned int specifying the chunk size.  The inital chunk size is set to SB_DFLT_CHUNK,
 *	which is set to 8192 in the <sys/bufmod.h> header file.
 *
 *  SBIOCGCHUNK:
 *	This input-output control is used to retreive the maximum chunk size, the maximum to which
 *	bufmod(4) will coalesce packets.  arg is a pointer to an unsigned int into which to retrieve
 *	the current chunk size.
 *
 *  SBIOCSSNAP:
 *	This input-output control is used to get the snapshot length.  This is the maximum length of
 *	the packet (with or without a converted M_PROTO(9) message block) that will be passed to the
 *	chunk and onward upstream.  Setting a snapshot length of zero (0) is interpreted as and
 *	indefinitely long length and equivalent to not truncating any message.  arg is a pointer to
 *	an unsigned long specifying the snapshot length.  The initial snapshot lenght is set to zero
 *	(0), meaning that no message truncation will occur.
 *
 *  SBIOCGSNAP:
 *	arg is a pointer to an unsigned long into which to * retrieve the current snapshot length.
 *
 *  SBIOCSFLAGS:
 *	arg is a pointer to an unsigned long specifying the buffer flags.  The buffer flags may be a
 *	bitwise OR of any of the following flags:
 *
 *	SB_SEND_ON_WRITE:
 *	    When this bit is set, it specifies that accumultated chunks are to be delivered
 *	    immediately upon the receiption of any STREAMS message on the write queue.  This feature
 *	    permits the upper layer module or user to specify when packets should be collected on
 *	    demand regardless of the state of the chunk and its associated timeouts.  It is possible
 *	    to combine this mechanism with blocking reads using read(2s) and readv(2s) by invoking
 *	    read notifications using M_READ(9) messages.  The original implementation of bufmod did
 *	    not have this feature and it is, therefore, disabled by default.
 *
 *	SB_NO_HEADER:
 *	    When this bit is set, it specifies that headers are not to be prepended to accumulated
 *	    messages.  When headers are disabled using this feature, the reader of chunks must have
 *	    some other mechanism that may be used to determine where the concatenated packets begin
 *	    and end, as well as the timestamp associated with each message.  (For example, a
 *	    converted M_PROTO(9) message block may contain the necessary information, or it might be
 *	    contained in the message headers or pseudo header).  The original implementation of
 *	    bufmod did not have this feature and it is, therefore, disabled by default.
 *
 *	SB_NO_PROTO_CVT:
 *	    When this bit is set, it specifies that M_PROTO(9) message blocks are not to be convrted
 *	    to M_DATA(9) message blocks.  When enabled, and an M_PROTO(9) message is received, it
 *	    will be treated as other messages: i.e. causing the closure and dispatch of any
 *	    accumulated chunk before the M_PROTO(9) message itself is delivered upstream.  This
 *	    feature is useful when it is known that data messages are received only in M_DATA(9)
 *	    message blocks and that any M_PROTO(9) message needs to be delivered separated from the
 *	    data stream.  The original implementation of bufmod did not have this feature and it is,
 *	    therefore, disabled by default.
 *
 *	SB_DEFER_CHUNK:
 *	    When this bit is set, it specifies that the intial message received within the timeout
 *	    period is to be delivered immediately, deferring chunk accumulation start until the
 *	    second message arrival within the time interval.  This feature helps deliver packets per
 *	    timeout period, the packets will be delivered immediately regardless of the chunking
 *	    mechanism.  The original implementation of bufmod did not have this feature and it is,
 *	    therefore, disabled by default.
 *
 *	SB_NO_DROPS:
 *	    When this bit is set, the module will exert downstream flow control (when flow
 *	    controlled from upstream) rather than dropping messages.  (In fact, Sun's implementation
 *	    forces chunks upstream against flow control rather than exerting downstream flow
 *	    control.)  Also, if a message block allocation fails, it will invoke bufcall procedures
 *	    rather than discarding messages.  (In fact, Sun's implementation drops messages on
 *	    message block allocation failures regardless of the setting of this flag.)  The original
 *	    implementation of bufmod did not have this feature and it is, therefore, disabled by
 *	    default.
 *
 *	The initial value of all flags is clear (0).  This is equivalent to the old, flagless
 *	behaviour of ancient bufmod implementations.
 *
 *  SBIOCGFLAGS:
 *	arg is a pointer to an unsigned long into which to retreive the current buffer flags.
 *
 *  See M_IOCTL(9) for a description of how Linux Fast-STREAMS handles 32-bit user space
 *  input-output controls to a 64-bit kernel.
 *  -------------------------------------------------------------------------
 */

/** bufmod_ioctl: - the first stage of the transparent IOCTL mechanism
  * @q: the write queue
  * @mp: the M_IOCTL message
  */
STATIC noinline void __unlikely fastcall
bufmod_ioctl(queue_t *q, mblk_t *mp)
{
	struct sb *sb = (typeof(sb)) q->q_ptr;
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *db;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case _IOC_TYPE(SBIOC):
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(SBIOCSTIME):
#ifdef __LP64__
			if (ioc->ioc_flag == IOC_ILP32)
				mi_copyin(q, mp, NULL, sizeof(struct timeval32));
			else
#endif				/* __LP64__ */
				mi_copyin(q, mp, NULL, sizeof(struct timeval));
			break;
		case _IOC_NR(SBIOCSCHUNK):
			mi_copyin(q, mp, NULL, sizeof(unsigned int));
			break;
		case _IOC_NR(SBIOCSSNAP):
		case _IOC_NR(SBIOCSFLAGS):
#ifdef __LP64__
			if (ioc->ioc_flag == IOC_ILP32)
				mi_copyin(q, mp, NULL, sizeof(uint32_t));
#endif				/* __LP64__ */
			else
				mi_copyin(q, mp, NULL, sizeof(unsigned long));
			break;
		case _IOC_NR(SBIOCCTIME):
			if (bufmod_stropts(sb, -1, sb->sb_snap) != 0) {
				mi_copy_done(q, mp, ENOSR);
				break;
			}
			if (sb->sb_ticks > 0)
				mi_timer_stop(sb->sb_timer);
			sb->sb_ticks = 0;
			sb->sb_chunk = -1;
			if (qsize(sb->sb_rq) > 0)
				qenable(sb->sb_rq);
			mi_copy_done(q, mp, 0);
			break;
		case _IOC_NR(SBIOCGTIME):
		{
			struct timeval tn;

			if (sb->sb_ticks < 0) {
				mi_copy_done(q, mp, ERANGE);
				break;
			}
			if (sb->sb_ticks <= 0) {
				tn.tv_sec = 0;
				tn.tv_usec = 0;
			} else {
				tn.tv_sec = drv_hztomsecs(sb->sb_ticks) / 1000;
				tn.tv_usec = (drv_hztomsecs(sb->sb_ticks) % 1000) * 1000;
			}
#ifdef __LP64__
			if (cp->cp_flag == IOC_ILP32) {
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
		case _IOC_NR(SBIOCGCHUNK):
			db = mi_copyout_alloc(q, mp, NULL, sizeof(int), 1);
			if (likely(db != NULL))
				*(int *) db->b_rptr = sb->sb_chunk;
			if (likely(db != NULL))
				mi_copyout(q, mp);
			break;
		case _IOC_NR(SBIOCGSNAP):
#ifdef __LP64__
			if (cp->cp_flag == IOC_ILP32) {
				db = mi_copyout_alloc(q, mp, NULL, sizeof(uint32_t), 1);
				if (likely(db != NULL))
					*(uint32_t *) db->b_rptr = sb->sb_snap;
			} else
#endif				/* __LP64__ */
			{
				db = mi_copyout_alloc(q, mp, NULL, sizeof(ulong), 1);
				if (likely(db != NULL))
					*(ulong *) db->b_rptr = sb->sb_snap;
			}
			if (likely(db != NULL))
				mi_copyout(q, mp);
			break;
		case _IOC_NR(SBIOCGFLAGS):
#ifdef __LP64__
			if (cp->cp_flag == IOC_ILP32) {
				db = mi_copyout_alloc(q, mp, NULL, sizeof(uint32_t), 1);
				if (likely(db != NULL))
					*(uint32_t *) db->b_rptr = sb->sb_flags & SB_USER;
			} else
#endif				/* __LP64__ */
			{
				db = mi_copyout_alloc(q, mp, NULL, sizeof(ulong), 1);
				if (likely(db != NULL))
					*(ulong *) db->b_rptr = sb->sb_flags & SB_USER;
			}
			if (likely(db != NULL))
				mi_copyout(q, mp);
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

/** bufmod_iocdata: - stage 2 of the transparent ioctl mechanism
  * @q: the write queue
  * @mp: the M_IOCDATA message
  */
STATIC noinline void __unlikely fastcall
bufmod_iocdata(queue_t *q, mblk_t *mp)
{
	struct sb *sb = (typeof(sb)) q->q_ptr;
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *db;

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case _IOC_TYPE(SBIOC):
		switch (_IOC_NR(cp->cp_cmd)) {
		case _IOC_NR(SBIOCSTIME):
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
				sb->sb_ticks = 0;
			else {
				sb->sb_ticks = 0;
				sb->sb_ticks += drv_msectohz(tn.tv_sec * 1000);
				sb->sb_ticks += drv_usectohz(tn.tv_usec);
				if (sb->sb_ticks <= 0)
					sb->sb_ticks = 1;
			}
			mi_copy_done(q, mp, 0);
			break;
		}
		case _IOC_NR(SBIOCSCHUNK):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EPROTO);
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
				if (*(int *)dp->b_rptr < -1) {
					mi_copy_done(q, mp, EINVAL);
					break;
				}
				if (bufmod_stropts(sb, *(int *)dp->b_rptr, sb->sb_snap) != 0) {
					mi_copy_done(q, mp, ENOSR);
					break;
				}
				sb->sb_chunk = *(int *)dp->b_rptr;
				if (sb->sb_chunk > 65536)
					sb->sb_chunk = 65536;
				mi_copy_done(q, mp, 0);
				break;
			}
			break;
		case _IOC_NR(SBIOCSSNAP):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EPROTO);
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
#ifdef __LP64__
				if (cp->cp_flag == IOC_ILP32) {
					sb->sb_snap = *(uint32_t *) dp->b_rptr;
				} else
#endif				/* __LP64__ */
				{
					sb->sb_snap = *(ulong *) dp->b_rptr;
				}
				mi_copy_done(q, mp, 0);
				break;
			}
			break;
		case _IOC_NR(SBIOCSFLAGS):
			/* arg is a pointer to an unsigned long specifying the buffer
			   flags.  The buffer flags may be a bitwise OR of any of the
			   following flags. */
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EPROTO);
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
			{
				ulong flags;

#ifdef __LP64__
				if (cp->cp_flag == IOC_ILP32) {
					flags = *(uint32_t *) dp->b_rptr;
				} else
#endif				/* __LP64__ */
				{
					flags = *(ulong *) dp->b_rptr;
				}
				if (flags & ~(SB_USER)) {
					mi_copy_done(q, mp, EINVAL);
					break;
				}
				sb->sb_flags &= ~(SB_USER);
				sb->sb_flags |= flags;
				mi_copy_done(q, mp, 0);
				break;
			}
			}
			break;
		case _IOC_NR(SBIOCCTIME):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EFAULT);
				break;
			}
			break;
		case _IOC_NR(SBIOCGTIME):
		case _IOC_NR(SBIOCGCHUNK):
		case _IOC_NR(SBIOCGSNAP):
		case _IOC_NR(SBIOCGFLAGS):
			switch (mi_copy_state(q, mp, &db)) {
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
 *  -------------------------------------------------------------------------
 *
 *  Read-Side procedures.
 *
 *  -------------------------------------------------------------------------
 */

/** bufmod_gettimeval: - get a timestamp for the message if necessary
  * @sb: private structure
  * @sbh: pseudo STREAMS buffer header
  *
  * First order of business, when we are prepending headers, take a timestamp immediately.  Note
  * that the size of the structure used by sb_hdr is not always the same size as the native struct
  * timeval, so we copy over members one by one.
  */
STATIC noinline fastcall __hot_get void
bufmod_gettimeval(const struct sb *sb, struct sb_hdr *sbh)
{
	struct timeval tv;

	do_gettimeofday(&tv);
	sbh->sbh_timestamp.tv_sec = tv.tv_sec;
	sbh->sbh_timestamp.tv_usec = tv.tv_usec;
}

/** bufmod_adjsnap: -adjust a message to the snapshot length.
  * @sb: private structure
  * @mp: message to adjust
  * @sbh: pseudo STREAMS buffer header
  * @epp: (return) pointer to last block in message block chain
  *
  * Calculate original length and message length, truncate to snapshot length and return pointer to
  * last message block in message block chain.  In the Sun module this is performed using separate
  * calls to msgdsize(9), adjmsg(9) and walking the buffer chain.  Not only is this more efficient,
  * it will work better when an M_PROTO or other message block slips into the middle of the chain
  * somewhere.
  *
  * Note: if the size of the message that is about to be placed on the queue would cause flow
  * control to be invoked for the queue, simply drop the message.  The reason for this is that we
  * never want the read queue flow controlled downstream: otherwise, the driver could start dropping
  * messages without our knowing about it.  This tells the driver that everything is flowing nicely
  * upstream and gets it to deliver each and every message to us.
  */
static inline fastcall __hot_get int
bufmod_adjsnap(const struct sb *sb, mblk_t *mp, struct sb_hdr *sbh, mblk_t **epp)
{
	register mblk_t *b, *e;
	register ssize_t olen, mlen, dlen;
	const ssize_t snap = sb->sb_snap;
	queue_t *q = sb->sb_rq;

	prefetch(q);
	for (olen = 0, mlen = 0, e = b = mp; b != NULL; e = b, b = b->b_cont) {
		if ((dlen = b->b_wptr - b->b_rptr) > 0) {
			olen += dlen;
			if (!snap || mlen + dlen <= snap)
				mlen += dlen;
			else {
				b->b_wptr = b->b_rptr + (snap - mlen);
				mlen = snap;
			}
		} else if (dlen < 0)
			b->b_wptr = b->b_rptr;
	}
	/* The only problem with this is that the service procedure could have one or two 
	   messages pulled off of the queue at this point, so we need a fudge factor. */
	if (q->q_count + (sb->sb_flags & SB_NO_HEADER) ? mlen :
	    sizeof(*sbh) + ((mlen + (sizeof(ulong) - 1)) & ~(sizeof(ulong) - 1))
	    >= q->q_hiwat)
		return (-EBUSY);
	sbh->sbh_origlen = olen;
	sbh->sbh_msglen = mlen;
	sbh->sbh_totlen = mlen;	/* for now */
	*epp = e;
	return (0);
}

/** bufmod_padmsg: -  add padding to a message.
  * @sb: private structure
  * @sbh: pseudo STREAMS buffer header
  * @ep: end pointer (last message block in chain)
  *
  * When headers are applied to messages, messages are padded so that the message header is always
  * properly aligned in the user buffer.  This is accomplished by padding the message length to the
  * size of a ulong.
  *
  * An attempt is made to use any additional space at the end of an existing message block (the last
  * block in the message chain is pointed to by ep).  For normal operation, this should always
  * succeed.  It can be the case that the data block is shared (this is for monitoring after all and
  * duping message blocks is one easy way to do that in the driver).  When the data block is shared
  * we do not zero the padding.  The header identifies the padding anyway
  * (sbh.sbh_totlen-sizeof(sbh)-sbh.sbh_msglen).
  *
  * In the sad event that we cannot use the end of the existing data block, we allocate a new block
  * and link it to the end of the message chain.
  */
STATIC noinline fastcall __hot_get int
bufmod_padmsg(const struct sb *sb, struct sb_hdr *sbh, mblk_t *ep)
{
	ssize_t pad;
	ssize_t plen;
	mblk_t *pp;

	plen = (sbh->sbh_msglen + (sizeof(ulong)-1)) & ~(sizeof(ulong)-1);
	if (likely((pad = plen - sbh->sbh_msglen) > 0)) {
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
	sbh->sbh_totlen = sizeof(*sbh) + plen;
	return (0);
      enosr:
	return (-ENOSR);
}

/** bufmod_addheader: - add a STREAMS buffer header to the packet
  * @sb: private structure
  * @sbh: pseudo STREAMS buffer header
  * @mpp: (return) pointer to first block in message
  *
  * This function simply adds a STREAMS buffer header to the packet.  To avoid unnecessary message
  * block allocation calls, an attempt is made to see whether the STREAMS buffer header (struct
  * sb_hdr) can fit into the first data block and whether the first data block can be written.  If
  * the header can fit in the first block without adjustment, or the amount of data that needs to be
  * moved is not too excessive, the first block is adjusted to include the header.  Otherwise, a
  * message block is allocated just for the header.
  */
STATIC noinline fastcall __hot_get int
bufmod_addheader(const struct sb *sb, struct sb_hdr *sbh, mblk_t **mpp)
{
	static const size = sizeof(struct sb_hdr);
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

	sbh->sbh_drops = sb->sb_pdrops;

	/* We really don't know what the final alignment of mp->b_rptr will be.  */
	bcopy((unsigned char *) sbh, mp->b_rptr, size);

	mp->b_flag |= MSGHEADER;

	return (0);
      enosr:
	return (-ENOSR);
}

/** bufmod_msgadjust: - size and truncate a message remembering the last block
  * @sb: private structure
  * @mpp: the message to manipulate
  *
  * This function uses the functions, above, to time-stamp the message (when headers are required),
  * truncate the message to the snapshot length, pad the message for packet headers, and prepends
  * the packet header.  The result is the message as it would appear in the user buffer.  This
  * function is only used by the read put procedure, and, although it needs write access to the
  * message, it has read-only access to the private structure.
  *
  * The only elements read from the private structure are the SB_NO_HEADER bit in sb_flags, the
  * sb_snap snapshot length and the sb_pdrops count.  When any of these are changed, one should
  * flush the read queue of messages that use the old values.
  */
static inline fastcall __hot_get int
bufmod_msgadjust(const struct sb *sb, mblk_t **mpp)
{
	register mblk_t *ep;
	struct sb_hdr sbh = { 0, };

	if (bufmod_adjsnap(sb, (*mpp), &sbh, &ep) != 0) return (-EBUSY);

	if (likely(!(sb->sb_flags & SB_NO_HEADER))) {
		bufmod_gettimeval(sb, &sbh);
		if (bufmod_padmsg   (sb, &sbh, ep)  != 0) return (-ENOSR);
		if (bufmod_addheader(sb, &sbh, mpp) != 0) return (-ENOSR);
	}
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

/** bufmod_wput: - write put procedure.
  * @q: write queue.
  * @mp: message to put.
  *
  * This is largely a canonical write put procedure with expedited M_IOCTL/M_IOCDATA processing.
  * The only difference from the normal is that when SB_SEND_ON_WRITE is set we cancel the chunk
  * timer and explicitly enable the read queue to deliver accumulated M_DATA messages.
  */
STATIC streamscall __hot_write int
bufmod_wput(queue_t *q, mblk_t *mp)
{
	struct sb *sb = (typeof(sb)) q->q_ptr;
	unsigned char db_type = mp->b_datap->db_type;

	if (sb->sb_flags & SB_SEND_ON_WRITE) {
		if (sb->sb_ticks > 0)
			mi_timer_stop(sb->sb_timer);
		qenable(sb->sb_rq);
	}

	switch (db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		return (0);
	case M_IOCTL:
		bufmod_ioctl(q, mp);
		return (0);
	case M_IOCDATA:
		bufmod_iocdata(q, mp);
		return (0);
	case M_READ:
		if (sb->sb_mread)
			freemsg(XCHG(&sb->sb_mread,NULL));
		sb->sb_mread = mp;
		if (sb->sb_ticks > 0)
			mi_timer_stop(sbw->sb_timer);
		qenable(sb->sb_rq);
		return (0);
	default:
		break;
	}
	if (db_type >= QPCTL || (q->q_first == NULL && !(q->q_flag & QSVCBUSY) &&
				 bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	if (unlikely(putq(q, mp) == 0)) {
		mp->b_band = 0;
		putq(q, mp);	/* this must succeed */
	}
	return (0);
}

/** bufmod_wsrv: - write service procedure.
  * @q: the write queue.
  *
  * This is a canonical write service procedure.  It simply passes messages along under flow
  * control.
  */
STATIC streamscall __hot_read int
bufmod_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (likely(mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band))) {
			putnext(q, mp);
			continue;
		}
		putbq(q, mp);
		break;
	}
	return (0);
}

/** bufmod_msgsizelast: -
  * @mp: the message to size
  * @mlen: (return) message length
  * @epp: (return) last message block in chain
  */
static inline streams_fastcall void
bufmod_msgsizelast(mblk_t *mp, int *mlen, mblk_t **epp)
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

/** bufmod_rsrv: - read service procedure.
  * @q: the read queue
  *
  * Aside from the message chunking, this is a canonical service procedure.  The read queue runs
  * with noenable(9) set, meaning that this service procedure will only run for normal priority data
  * when back-enabled or explicitly requested to be run with qenable(9).  The logic is as follows:
  *
  * M_DATA:
  *	When the chunk size is zero, process each M_DATA packet as we would normally process an
  *	M_DATA message: reading the queue and processing messages until they are exhausted or we
  *	encounter flow control.  This is also true of the first message of a timeout period when
  *	SB_DEFER_CHUNK is requested.
  *
  *	When the chunk size is non-zero, we concatenate M_DATA messages together until the chunk
  *	size is or would be exceeded, or another message type is encountered in the stream.  We then
  *	treat these concatenated M_DATA messages like we would treat any other in a service
  *	procedure, subjected to flow control and the like.
  *
  *	When subjected to flow control, we simply start dropping messages.  The time has come to
  *	send M_DATA and the upper modules or user is simply not keeping up.  When SB_NO_DROPS is
  *	set, we exert flow control back-pressure instead by filling our own queue.  The default
  *	(!SB_NO_DROPS) is actually better because it allows us to determine the number of dropped
  *	messages, whereas, the back-pressure approach would likely cause them to be dropped silently
  *	by the driver.
  *
  *	We keep the number of drops plus the number of messages in the b_csum field of the first
  *	message block in the chain.  This allows us to calculate from any given message or
  *	concatenated message removed from the queue, the number of messages plus drops that it
  *	represents.  When discarding concatenated messages, this allows use to form a proper count
  *	of the number of drops.
  *
  *	The chunk size and defer flag are read at the onset of the routine so that they cannot
  *	change during the course of the function.
  */
STATIC streamscall __hot_read int
bufmod_rsrv(queue_t *q)
{
	struct sb *sb = (typeof(sb)) q->q_ptr;
	uint32_t defer = (sb->sb_flags & SB_DEFER_CHUNK);
	long chunk = sb->sb_chunk;
	mblk_t *mp, *mr;

	if ((mr = XCHG(&sb->sb_mread, NULL)) != NULL)
		chunk = *(long *) mr->b_rptr;

	while ((mp = getq(q)) != NULL) {
		unsigned char mp_type = mp->b_datap->db_type;

		switch (__builtin_expect(mp_type, M_DATA)) {
		case M_DATA:
			mp->b_flag &= ~MSGDELIM;
			if (chunk < 0 && mr == NULL) {
				/* wait for an M_READ */
				putbq(q, mp);
				break;
			}
			if (chunk == 0) {
				/* process individually */
				sb->sb_state = SB_FRCVD;
				break;
			}
			if (defer && !sb->sb_state) {
				/* process first msg individually */
				sb->sb_state = SB_FRCVD;
				break;
			}
			{
				size_t count = 0;
				mblk_t *lp, *dp;
				int mlen;

				bufmod_msgsizelast(mp, &mlen, &lp);
				if (mlen >= chunk) {
					/* let it go */
					sb->sb_state = 0;
					break;
				}
				count += mlen;
				while ((dp = getq(q))) {
					unsigned char dp_type;
					mblk_t *ep;
					int dlen;

					dp->b_flag &= ~MSGDELIM;
					dp_type = dp->b_datap->db_type;
					if (db_type != M_DATA) {
						putbq(q, dp);
						/* let it go */
						sb->sb_state = 0;
						break;
					}

					bufmod_msgsizelast(dp, &dlen, &ep);
					if (count + dlen > chunk) {
						putbq(q, dp);
						/* let it go */
						mp->b_flags |= MSGDELIM;
						sb->sb_state = 0;
						break;
					}
					/* chunk it up */
					count += dlen;
					lp->b_cont = dp;
					lp = ep;
					mp->b_csum += dp->b_csum;
				}
				/* nothing left - let it go */
				mp->b_flags |= MSGDELIM;
				sb->sb_state = 0;
				break;
			}
		case M_PCSIG:
			if (mp == sb->sb_timer) {
				mi_timer_valid(mp);
				continue;
			}
			putnext(q, mp);
			continue;
		default:
			break;
		}
		if (mp_type == M_DATA && sb->sb_sdrops) {
			mp->b_csum += sb->sb_sdrops;
			if (mp->b_flag & MSGHEADER) {
				struct sb_hdr sbh;

				/* don't know alignment of header */
				bcopy(mp->b_rptr, &sbh, sizeof(sbh));
				sbh.sbh_drops += sb->sb_sdrops;
				bcopy(&sbh, mp->b_rptr, sizeof(sbh));
			}
			sb->sb_sdrops = 0;
		}
		if (mp_type >= QPCTL || bcanputnext(q, mp->b_band)) {
			mp->b_flag &= ~MSGHEADER;
			if ((mr != NULL) && (mp->b_flag & MSGDELIM))
				freemsg(XCHG(&mr, NULL));
			putnext(q, mp);
			continue;
		}
		if (mp_type == M_DATA && !(sb->sb_flags & SB_NO_DROPS)) {
			sb->sb_sdrops += mp->b_csum;
			freemsg(mp);
			continue;
		}
		putbq(q, mp);
		break;
	}
	if (mr != NULL) {
		/* Transform to zero-length delimited M_DATA to unblock the read at the
		   Stream head. */
		mr->b_datap->db_type = M_DATA;
		mr->b_flag |= MSGDELIM;
		mr->b_wptr = mr->b_rptr;
		putnext(q, mp);
	}
	return (0);
}

/*
 * I find it rather amazing that the noenable(9) and enableok(9) queue functions were provided
 * explicitly for the types of message coalescing being performed by bufmod(4), and yet, Sun does
 * not use them, but, rather breaks flow control by using an out-of-band mechanism.
 */

/** bufmod_rput: - STREAMS buffer module read-side put procedure
  * @q: the read queue
  * @mp: the message to put
  *
  * M_PROTO:
  *	We can process high priority M_PROTO messages separately because they will jump the chunking
  *	queue which is only in band 0.  However, this could confuse some clients, so we provide an
  *	option for it: SB_HIPRI_OOB
  *
  *	When SB_NO_PROTO_CVT is set, M_PROTO messages are treated like any other non-M_DATA message.
  *	When it is not set, we perform conversion.  When performing conversion, we simply change the
  *	first data block to an M_DATA data block and process it just like an M_DATA message.  Note
  *	that we do not expect M_PROTO data blocks to be shared.
  *
  * M_DATA:
  *	Call the bufmod_msgadjust() function that will perform the necessary transformations on the
  *	packet, like truncating to snapshot length and prepending headers when requested.  Because
  *	data must be in band 0 to work correctly, we make sure to set mp->b_band = 0.  We hide the
  *	number of drops leading up to the message in the mp->b_csum field on the first block of the
  *	packet so that if it is discarded later, the number of drops will be calculated correctly.
  *	We always put M_DATA message on the noenable(9) read queue.  This queue will not schedule
  *	its service procedure unless a high-priority message is queued, or the queue is back-enabled
  *	or explicitly enabled with qenable(9).
  *
  *	When the number of bytes queued exceeds a chunk size, the timer (if any) is cancelled and
  *	the queue is enabled.  Otherwise, we ensure that a timer is running.  When the
  *	SB_DEFER_CHUNK option is active and there has been no first message yet, the queue is
  *	enabled to deliver the first message.
  *
  * M_FLUSH:
  *	Processed as normal; however, when band 0 data is flushed, the chunking state is reset back
  *	to its initial state.
  *
  * >= QPCTL:
  *	High Priority messages can be expedited around the queue without altering the behaviour of
  *	the queue.
  *
  * < QPCL:
  *	Low priority messages (other than M_DATA and M_PROTO with conversion) that arrive at an
  *	empty queue can bypass the queue as long as upstream flow control is not exerted  Otherwise,
  *	they are queued as normal.
  *
  *	Any band 0 message that is queued causes the chunk timer to be cancelled and the queue to be
  *	scheduled, otherwise they will wait for an indefinite period of time behind the chunked
  *	M_DATA messages.  An example would be an M_SIG message.
  *
  *	For a band 0 message that is not an M_DATA nor converted M_PROTO message, we need to release
  *	all of the chunks in front of the message on the queue otherwise it will get deferred.  Band
  *	1-255 messages will bypass the chunk queue as they would have anyway had there been a
  *	backlog.  Some messages that would do this are disconnect messages or error indications.
  *
  * Synchronization:
  *	This read put procedure only writes to the sb_pdrops and sb_timer members of the private
  *	structure.  All other fields are read-only.  Of these, sb_pdrops is only accessed by the
  *	read put procedure.  This means that the read put procedure can run in full concurrency with
  *	the other procedures.
  */
STATIC streamscall __hot_get int
bufmod_rput(queue_t *q, mblk_t *mp)
{
	const struct sb *sb = (typeof(sb)) q->q_ptr;
	struct sb *sbw = (typeof(sb)) q->q_ptr;
	unsigned char db_type = mp->b_datap->db_type;

	switch (__builtin_expect(db_type, M_DATA)) {
	case M_PROTO:
		if (mp->b_band != 0 && (sb->sb_flags & SB_HIPRI_OOB)) {
			break;
		}
		if (sb->sb_flags & SB_NO_PROTO_CVT) {
			break;
		}
		mp->b_datap->db_type = M_DATA;

		/* fall through */

	case M_DATA:
		if (bufmod_msgadjust(sb, &mp) != 0)
			goto dropit;

		mp->b_band = 0;
		mp->b_csum = 1 + XCHG(&sbw->sb_pdrops, 0);

		putq(q, mp);

		/* We don't really want to muck with the queue and timers on
		 * every message.  Need some check to see whether it is worth
		 * enabling the queue.  For example, if the queue is waiting on
		 * a back-enable, there is no point in enabling the queue nor in
		 * setting any timer. */

		if (sb->sb_chunk > 0 && q->q_count > sb->sb_chunk) {
			if (sb->sb_ticks > 0)
				mi_timer_stop(sbw->sb_timer, sb->sb_ticks);
			qenable(q);
			return (0);
		}
		if (sb->sb_ticks > 0) {
			mi_timer_cond(sbw->sb_timer, sb->sb_ticks);
		}
		if ((sb->sb_flags & SB_DEFER_CHUNK) && !sb->sb_state) {
			qenable(q);
		}
		return (0);

	      dropit:
		sbw->sb_pdrops++;
		freemsg(mp);
		return (0);

	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if ((mp->b_rptr[0] & FLUSHBAND) && mp->b_rptr[1] != 0)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else {
				flushq(q, FLUSHDATA);

				if (sb->sb_ticks > 0)
					mi_timer_stop(sbw->sb_timer);
				sbw->sb_state = 0;
			}
		}
		putnext(q, mp);
		return (0);

	default:
		break;
	}
	if (db_type >= QPCTL || (q->q_first == NULL && !(q->q_flag & QSVCBUSY) &&
				 bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	if (unlikely(putq(q, mp) == 0)) {
		mp->b_band = 0;
		putq(q, mp);	/* this must succeed */
	}
	if (mp->b_band == 0) {
		if (sb->sb_ticks > 0)
			mi_timer_stop(sbw->sb_timer);
		qenable(q);
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
bufmod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;
	struct sb *sb;
	mblk_t *tb, *mp;
	struct stroptions *so;

	if (q->q_ptr)
		return (0);

	if (sflag != MODOPEN)
		return (ENXIO);

	if ((tb = mi_timer_alloc(q, 0)) == NULL) {
		return (ENOSR);
	}

	if ((mp = allocb(sizeof(*so), BPRI_WAITOK)) == NULL) {
		mi_timer_free(tb);
		return (ENOSR);
	}

	write_lock(&sb_lock);
	if ((err = mi_open_comm(&sb_opens, sizeof(*sb), q, devp, oflags, sflag, crp))) {
		write_unlock(&sb_lock);
		freeb(mp);
		mi_timer_free(tb);
		return (err);
	}

	/* inherit packet sizes of what we are pushed over */
	wq = WR(q);
	wq->q_minpsz = wq->q_next->q_minpsz;
	wq->q_maxpsz = wq->q_next->q_maxpsz;

	sb = (typeof(sb)) q->q_ptr;
	sb->sb_rq = q;
	sb->sb_ticks = -1;
	sb->sb_chunk = SB_DFLT_CHUNK;
	sb->sb_timer = tb;
	sb->sb_pdrops = 0;
	sb->sb_sdrops = 0;
	sb->sb_snap = 0;
	sb->sb_flags = 0;
	sb->sb_state = 0;
	sb->sb_mread = NULL;

	write_unlock(&sb_lock);

	mp->b_datap->db_type = M_SETOPTS;
	mp->b_wptr = mp->b_rptr + sizeof(*so);
	bzero(mp->b_rptr, sizeof(*so));
	so = (typeof(so)) mp->b_rptr;

	so->so_flags = 0;

	//so->so_readopt = RNORM;	/* XXX RMSGN? */
	//so->so_flags |= SO_READOPT;
	so->so_flags |= SO_MREADOFF;

	so->so_lowat = SHEADLOWAT; /* SHEADHIWAT = 8192 */
	so->so_lowat = SNIT_LOWAT(SB_DFLT_CHUNK, 1); /* 32767 + 256 */
	so->so_flags |= SO_LOWAT;

	so->so_hiwat = SHEADHIWAT; /* SHEADHIWAT = 65536 */
	so->so_hiwat = SNIT_HIWAT(SB_DFLT_CHUNK, 1); /* 65536 + 512 */
	so->so_flfags |= SO_HIWAT;

	qprocson(q);
	noenable(q);
	return (0);
}

STATIC streamscall int
bufmod_close(queue_t *q, int oflag, cred_t *crp)
{
	struct sb *sb = (typeof(sb)) q->q_ptr;

	qprocsoff(q);
	if (sb->sb_mread != NULL)
		freemsg(XCHG(&sb->sb_mread, NULL));
	if (sb->sb_timer != NULL)
		mi_timer_free(XCHG(&sb->sb_timer, NULL));
	write_lock(&sb_lock);
	mi_close_comm(&sb_opens, q);
	write_unlock(&sb_lock);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
STATIC struct qinit bufmod_rinit = {
	.qi_putp = bufmod_rput,
	.qi_bufp = bufmod_rsrv,
	.qi_qopen = bufmod_open,
	.qi_qclose = bufmod_close,
	.qi_minfo = &bufmod_minfo,
	.qi_mstat = &bufmod_rstat,
};

STATIC struct qinit bufmod_winit = {
	.qi_putp = bufmod_wput,
	.qi_bufp = bufmod_wsrv,
	.qi_minfo = &bufmod_minfo,
	.qi_mstat = &bufmod_wstat,
};

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
STATIC
#endif
struct streamtab bufmodinfo = {
	.st_rdinit = &bufmod_rinit,
	.st_wrinit = &bufmod_winit,
};

STATIC struct fmodsw bufmod_fmod = {
	.f_name = CONFIG_STREAMS_BUFMOD_NAME,
	.f_str = &bufmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef __LP64__
struct bufmod_trans {
	unsigned int cmd;
	void *opaque;
};

STATIC struct bufmod_trans bufmod_trans_map[] = {
	{.cmd = SBIOCSTIME,}
	, {.cmd = SBIOCGTIME,}
	, {.cmd = SBIOCCTIME,}
	, {.cmd = SBIOCSCHUNK,}
	, {.cmd = SBIOCGCHUNK,}
	, {.cmd = SBIOCSSNAP,}
	, {.cmd = SBIOCGSNAP,}
	, {.cmd = SBIOCSFLAGS,}
	, {.cmd = SBIOCGFLAGS,}
	, {.cmd = SBIOCGTIME,}
	, {.cmd = SBIOCGTIME,}
	, {.cmd = SBIOCGTIME,}
	, {.cmd = SBIOCGTIME,}
	, {.cmd = 0,}
};

STATIC __unlikely void
bufmod_ioctl32_unregister(void)
{
	struct bufmod_trans *t;

	for (t = bufmod_trans_map; t->cmd != 0; t++) {
		unregister_ioctl32(t->opaque);
		t->opaque = NULL;
	}
	return;
}

STATIC __unlikely int
bufmod_ioctl32_register(void)
{
	struct bufmod_trans *t;

	for (t = bufmod_trans_map; t->cmd != 0; t++) {
		if ((t->opaque = register_ioctl32(t->cmd)) == NULL) {
			bufmod_ioctl32_unregister();
			return (-ENOMEM);
		}
	}
	return (0);
}
#endif				/* __LP64__ */

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
STATIC
#endif
int __init
bufmodinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
	printk(KERN_INFO BUFMOD_BANNER);
#else
	printk(KERN_INFO BUFMOD_SPLASH);
#endif
	bufmod_minfo.mi_idnum = modid;
#ifdef __LP64__
	if ((err = bufmod_ioctl32_register())) {
		cmn_err(CE_WARN, "%s: could not register 32bit ioctls, err = %d",
			CONFIG_STREAMS_BUFMOD_NAME, err);
		return (err);
	}
#endif				/* __LP64__ */
	if ((err = register_strmod(&bufmod_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d",
			CONFIG_STREAMS_BUFMOD_NAME, err);
#ifdef __LP64__
		bufmod_ioctl32_unregister();
#endif				/* __LP64__ */
		return (err);
	}
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
STATIC
#endif
void __exit
bufmodexit(void)
{
	int err;

#ifdef __LP64__
	bufmod_ioctl32_unregister();
#endif				/* __LP64__ */
	if ((err = unregister_strmod(&bufmod_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d",
			CONFIG_STREAMS_BUFMOD_NAME, err);
		return (void) (err);
	}
	return (void) (0);
}

#ifdef CONFIG_STREAMS_BUFMOD_MODULE
module_init(bufmodinit);
module_exit(bufmodexit);
#endif
