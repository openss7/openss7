/*****************************************************************************

 @(#) File: src/modules/bpfmod.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

static char const ident[] = "src/modules/bpfmod.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  This is BPFMOD a partial implementation of the classical Berkeley Packet Filter.  The missing
 *  pieces are implemented by the driver underneath.
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1	/* for mi_ functions */
#define _SUN_SOURCE	1	/* for sun version of mi_timer_alloc */

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/strsun.h>
#include <sys/strconf.h>
#include <sys/cmn_err.h>

#include <net/bpf.h>

#include "sys/config.h"

#define BPFMOD_DESCRIP		"Berkeley Packet Filter Module (BPFMOD) STREAMS Module"
#define BPFMOD_EXTRA		"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define BPFMOD_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define BPFMOD_REVISION	"OpenSS7 src/modules/bpfmod.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define BPFMOD_DEVICE		"SVR 4.2 Berkeley Packet Filter Module (BPFMOD) for STREAMS"
#define BPFMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define BPFMOD_LICENSE		"GPL"
#define BPFMOD_BANNER		BPFMOD_DESCRIP		"\n" \
				BPFMOD_EXTRA		"\n" \
				BPFMOD_COPYRIGHT	"\n" \
				BPFMOD_REVISION		"\n" \
				BPFMOD_DEVICE		"\n" \
				BPFMOD_CONTACT		"\n"
#define BPFMOD_SPLASH		BPFMOD_DEVICE		" - " \
				BPFMOD_REVISION		"\n"

#ifdef CONFIG_STREAMS_BPFMOD_MODULE
MODULE_AUTHOR(BPFMOD_CONTACT);
MODULE_DESCRIPTION(BPFMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(BPFMOD_DEVICE);
MODULE_LICENSE(BPFMOD_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_BPFMOD_MODULE */

#ifndef CONFIG_STREAMS_BPFMOD_NAME
//#define CONFIG_STREAMS_BPFMOD_NAME "bpfmod"
#error "CONFIG_STREAMS_BPFMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_BPFMOD_MODID
//#define CONFIG_STREAMS_BPFMOD_MODID 13
#error "CONFIG_STREAMS_BPFMOD_MODID must be defined."
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-bpfmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_BPFMOD_MODULE
static
#endif
modID_t modid = CONFIG_STREAMS_BPFMOD_MODID;

#ifdef CONFIG_STREAMS_BPFMOD_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for BPFMOD.");
#endif				/* CONFIG_STREAMS_BPFMOD_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_BPFMOD_MODID));
MODULE_ALIAS("streams-module-bpfmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info bm_minfo = {
	.mi_idnum = CONFIG_STREAMS_BPFMOD_MODID,
	.mi_idname = CONFIG_STREAMS_BPFMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat bm_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat bm_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  Private Structure
 *
 *  -------------------------------------------------------------------------
 */

/** struct bm: - BPF module private structure
  * @bm_rq: read queue.
  * @bm_capt: number of packets passed upstream.
  * @bm_blen: block length;
  * @bm_rtimeout: read timeout (milliseconds).
  * @bm_rsig: signal to send on read if no M_READ available.
  * @bm_immediate: true when read is to return immediately.
  * @bm_fildrop: true when filter drops are counted in bs_drop.
  * @bm_lock: true when the descriptor is locked.
  * @bm_tstamp: the format for the timestamp.
  * @bm_filter: index of active filter program.
  * @bm_filter: index of active write filter program.
  * @bm_program: two filter programs: one active, other staging.
  * @bm_writprg: two write filter programs: one active, other staging.
  * @bm_memstore: scratchpad memory store.
  * @bm_wrmstore: write filter scratchpad memory store.
  */
struct bm {
	spinlock_t bm_lock;		/* lock for this structure */
	queue_t *bm_rq;			/* read queue */
	atomic_t bm_count;		/* bytes queued */
	atomic_t bm_drop;		/* number of packets dropped locally */
	atomic_t bm_capt;		/* number of packets passed upstream */
	atomic_t bm_filt;		/* number of packets filtered */
	uint bm_blen;			/* block length */
	ulong bm_rtimeout;		/* read timeout (msec) */
	uint bm_rsig;			/* read signal */
	uint bm_immediate;		/* immediate mode when true */
	uint bm_fildrop;		/* add filtered pkts to dropped pkt cnts */
	uint bm_lock;			/* the device is locked */
	uint bm_tstamp;			/* type type of timestamp taken */
	struct timespec bm_ts;		/* time value */
	mblk_t *bm_timer;		/* timer mblk */
	mblk_t *bm_mread;		/* last M_READ message */
	uint bm_filter;			/* filter program selector */
	uint bm_wrprog;			/* write program selector */
	struct bpf_program bm_program[2];	/* two programs */
	struct bpf_program bm_writprg[2];	/* two write programs */
	/* might want to align this special */
	uint32_t bpf_memstore[BPF_MEMWORDS];	/* memory store (scratch) */
	uint32_t bpf_wrmstore[BPF_MEMWORDS];	/* memory store (scratch) */
};

#define MSGHEADER	(1<<15)
#define MSGFILTERED	(1<<14)

#if	defined DEFINE_RWLOCK
static DEFINE_RWLOCK(bm_open_lock);
#elif	defined __RW_LOCK_UNLOCKED
static rwlock_t bm_open_lock = __RW_LOCK_UNLOCKED(bm_open_lock);
#elif	defined RW_LOCK_UNLOCKED
static rwlock_t bm_open_lock = RW_LOCK_UNLOCKED;
#else
#error cannot initialize read-write locks
#endif
static caddr_t bm_opens;

/*
 * The correction factor is introduced to compensate for whatever assumptions
 * the modules below have made about how much traffic is flowing through the
 * stream and the fact that bpfmod may be snipping messages with the sb_snap
 * length.
 */
#define	SNIT_HIWAT(msgsize, fudge)	((4 * msgsize * fudge) + 512)
#define	SNIT_LOWAT(msgsize, fudge)	((2 * msgsize * fudge) + 256)

#define BPFMOD_HIWAT(chunksize, snapshot) \
	 SNIT_HIWAT(chunksize, snapshot > 0 ? snapshot <= 100 ? 4 : snapshot <= 400 ? 2 : 1 : 1)
#define BPFMOD_LOWAT(chunksize, snapshot) \
	 SNIT_LOWAT(chunksize, snapshot > 0 ? snapshot <= 100 ? 4 : snapshot <= 400 ? 2 : 1 : 1)

/** bm_stropts: - set Stream head options
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
bm_stropts(struct sb *sb, int chunk, int snap)
{
	struct stroptions *so;
	mblk_t *mp;

	/* for the purposes of this calculation */
	if (chunk < 4096)
		chunk = 4096;
	if (chunk > 65536)
		chunk = 65536;

	if (likely((mp = allocb(sizeof(*so), BPRI_MED)) != NULL)) {
		DB_TYPE(mp) = M_SETOPTS;
		mp->b_wptr = mp->b_rptr + sizeof(*so);
		bzero(mp->b_rptr, sizeof(*so));
		so = (typeof(so)) mp->b_rptr;
		so->so_flags = 0;
		so->so_flags |= (sb->sb_flags & SB_NO_MREAD) ? SO_MREADOFF : SO_MREADON;
		so->so_lowat = BPFMOD_LOWAT(chunk, snap);
		if (so->so_lowat < SHEADLOWAT)
			so->so_lowat = SHEADLOWAT;
		so->so_flags |= SO_LOWAT;
		so->so_hiwat = BPFMOD_HIWAT(chunk, snap);
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
 *  BPF Program Validation and Processing
 *
 *  -------------------------------------------------------------------------
 */

/* 
 * returns 0 when no good, returns true when good.
 */
STATIC noinline __unlikely fastcall int
bm_valid_program(const struct bpf_program *bf)
{
	uint i, from;
	const struct bpf_insn *p;
	uint len;

	if ((len = bf->bf_len) < 1 || len > BPF_MAXINSNS)
		return (0);

	for (p = bf->bf_insns, i = 0; i < len; i++, p++) {
		switch (BPF_CLASS(p->code)) {
		case BPF_LD:
		case BPF_LDX:
			switch (BPF_MODE(p->code)) {
			case BPF_IMM:
				break;
			case BPF_ABS:
			case BPF_IND:
			case BPF_MSH:
				if (p->k >= BPF_MAXINSNS)
					return (0);
				break;
			case BPF_MEM:
				if (p->k >= BPF_MEMWORDS)
					return (0);
				break;
			case BPF_LEN:
				break;
			default:
				return (0);
			}
			break;
		case BPF_ST:
		case BPF_STX:
			if (p->k >= BPF_MEMWORDS)
				return (0);
			break;
		case BPF_ALU:
			switch (BPF_OP(p->code)) {
			case BPF_ADD:
			case BPF_SUB:
			case BPF_MUL:
			case BPF_OR:
			case BPF_AND:
			case BPF_LSH:
			case BPF_RSH:
			case BPF_NEG:
				break;
			case BPF_DIV:
				if (BPF_SRC(p->code) == BPF_K && p->k == 0)
					return (0);
				break;
			default:
				return (0);
			}
			break;
		case BPF_JMP:
			from = i + 1;
			switch (BPF_OP(p->code)) {
			case BPF_JA:
				if (from + p->k < from || from + p->k >= len)
					return (0);
				break;
			case BPF_JEQ:
			case BPF_JGT:
			case BPF_JGE:
			case BPF_JSET:
				if (from + p->jt >= len || from + p->jf >= len)
					return (0);
				break;
			}
		case BPF_RET:
			break;
		case BPF_MISC:
			break;
		default:
			return (0);
		}
	}
	return BPF_CLASS(f[len - 1].code) == BPF_RET;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IOCTL Processing
 *
 *  -------------------------------------------------------------------------
 */

/** bm_copyout: - M_COPYOUT interception
  * @q: the read queue
  * @mp: the M_COPYOUT message
  *
  * The purpose of this procedure is to intercept the TRANSPARENT M_COPYOUT block for BIOCGSTATS or
  * BIOCGSTATSOLD copyout step so that we can add our counts to the data.
  */
STATIC noinline void __unlikely fastcall
bm_copyout(queue_t *q, mblk_t *mp)
{
	struct bm *bm = (typeof(bm)) q->q_ptr;
	struct copyreq *cq = (typeof(cq)) mp->b_rptr;
	mblk_t *dp;

	if ((dp = mp->b_cont) != NULL) {
		switch (cq->cq_cmd) {
		case BIOCGSTATS:
		{
			struct bpf_stat *bs = (typeof(bs)) dp->b_rptr;

			bs->bs_drop += bm->bm_drop;
			bs->bs_capt = bm->bm_capt;
			break;
		}
		case BIOCGSTATSOLD:
		{
			struct bpf_stat_old *bs = (typeof(bs)) dp->b_rptr;

			bs->bs_drop += bm->bm_drop;
			break;
		}
		default:
			break;
		}
	}
	putnext(q, mp);
}

/** bm_iocack: - M_IOCACK interception
  * @q: the read queue
  * @mp: the M_IOCACK message
  *
  * The purpose of this procedure is to intercept the I_STR M_IOCACK block for BIOCGSTATS or
  * BIOCGSTATSOLD copyout step so that we can add our counts to the data.
  */
STATIC noinline void __unlikely fastcall
bm_iocack(queue_t *q, mblk_t *mp)
{
	struct bm *bm = (typeof(bm)) q->q_ptr;
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *dp;

	if ((dp = mp->b_cont) != NULL) {
		switch (ioc->ioc_cmd) {
		case BIOCGSTATS:
		{
			struct bpf_stat *bs = (typeof(bs)) dp->b_rptr;

			bs->bs_drop += bm->bm_drop;
			bs->bs_capt = bm->bm_capt;
			break;
		}
		case BIOCGSTATSOLD:
		{
			struct bpf_stat_old *bs = (typeof(bs)) dp->b_rptr;

			bs->bs_drop += bm->bm_drop;
			break;
		}
		default:
			break;
		}
	}
	putnext(q, mp);
}

/** bm_ioctl: - the first stage of the transparent IOCTL mechanism
  * @q: the write queue
  * @mp: the M_IOCTL message
  */
STATIC noinline void __unlikely fastcall
bm_ioctl(queue_t *q, mblk_t *mp)
{
	struct bm *bm = (typeof(bm)) q->q_ptr;
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *db;
	size_t size;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case _IOC_TYPE(BIOC):
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(BIOCSBLEN):
		case _IOC_NR(BIOCSRTIMEOUT):
		case _IOC_NR(BIOCSFILDROP):	/* uint */
		case _IOC_NR(BIOCSETF):
		case _IOC_NR(BIOCSETFNR):
		case _IOC_NR(BIOCSETWF):
		case _IOC_NR(BIOCSRSIG):
		case _IOC_NR(BIOCSDIRECTION):
		case _IOC_NR(BIOCSDIRFILT):
		case _IOC_NR(BIOCFLUSH):
		case _IOC_NR(BIOCSDLT):
		case _IOC_NR(BIOCPROMISC):
		case _IOC_NR(BIOCSTCPF):
		case _IOC_NR(BIOCSUDPF):
		case _IOC_NR(BIOCSETIF):
		case _IOC_NR(BIOCSHDRCMPLT):
		case _IOC_NR(BIOCSSEESENT):
		default:
			/* none of these ioctls are acessible to non-privileged users while the
			   descriptor is locked */
			if (bm->bm_lock && ioc->ioc_cr->cr_euid != 0) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			switch (_IOC_NR(ioc->ioc_cmd)) {
			case _IOC_NR(BIOCSBLEN):
				mi_copyin(q, mp, 0, sizeof(uint));
				break;
			case _IOC_NR(BIOCSRTIMEOUT):
#ifdef __LP64__
				if (ioc->ioc_flag == IOC_ILP32) {
					size = sizeof(struct timeval32);
				} else
#endif				/* __LP64__ */
				{
					size = sizeof(struct timeval);
				}
				mi_copyin(q, mp, 0, size);
				break;
			case _IOC_NR(BIOCSFILDROP):	/* uint */
				mi_copyin(q, mp, 0, sizeof(uint));
				break;
			case _IOC_NR(BIOCSETF):
			case _IOC_NR(BIOCSETFNR):
			case _IOC_NR(BIOCSETWF):
#ifdef __LP64__
				if (ioc->ioc_flag == IOC_ILP32) {
					size = sizeof(struct bpf_program32);
				} else
#endif				/* __LP64__ */
				{
					size = sizeof(struct bpf_program);
				}
				mi_copyin(q, mp, 0, size);
				break;
			case _IOC_NR(BIOCSRSIG):
				mi_copyin(q, mp, 0, sizeof(uint));
				break;
			case _IOC_NR(BIOCSTSTAMP):
				mi_copyin(q, mp, 0, sizeof(uint));
				break;
			case _IOC_NR(BIOCSDLT):
			case _IOC_NR(BIOCPROMISC):
			case _IOC_NR(BIOCFLUSH):
			case _IOC_NR(BIOCSTCPF):
			case _IOC_NR(BIOCSUDPF):
			case _IOC_NR(BIOCSETIF):
			case _IOC_NR(BIOCSETLIF):
			case _IOC_NR(BIOCSHDRCMPLT):
			case _IOC_NR(BIOCSSEESENT):
			case _IOC_NR(BIOCSDIRECTION):
			case _IOC_NR(BIOCSDIRFILT):
			case _IOC_NR(BIOCFEEDBACK):
			case _IOC_NR(BIOCSFEEDBACK):
			default:
				/* handled by the driver */
				putnext(q, mp);
				break;
			}
			break;
		case _IOC_NR(BIOCGBLEN):	/* uint */
			if ((db = mi_copyout_alloc(q, mp, 0, sizeof(uint), 1)) != NULL) {
				*(uint *) db->b_rptr = bm->bm_blen;
				mi_copyout(q, mp);
			}
			break;
		case _IOC_NR(BIOCGRTIMEOUT):	/* uint */
#ifdef __LP64__
			if (ioc->ioc_flag == IOC_ILP32) {
				size = sizeof(struct timeval32);
			} else
#endif				/* __LP64__ */
			{
				size = sizeof(struct timeval);
			}
			if ((db = mi_copyout_alloc(q, mp, 0, size, 1)) == NULL)
				break;
#ifdef __LP64__
			if (ioc->ioc_flag == IOC_ILP32) {
				struct timeval32 *tv = (typeof(tv)) db->b_rptr;

				tv->tv_sec = bm->bm_rtimeout / 1000;
				tv->tv_usec = (bm->bm_rtimeout * 1000) % 1000000;
			} else
#endif				/* __LP64__ */
			{
				struct timeval *tv = (typeof(tv)) db->b_rptr;

				tv->tv_sec = bm->bm_rtimeout / 1000;
				tv->tv_usec = (bm->bm_rtimeout * 1000) % 1000000;
			}
			mi_copyout(q, mp);
			break;
		case _IOC_NR(BIOCGFILDROP):	/* uint */
			if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(uint), 1)) != NULL) {
				*(uint *) dp->b_rptr = (bm->bm_fildrop != 0);
				mi_copyout(q, mp);
			}
			break;
		case _IOC_NR(BIOCIMMEDIATE):	/* uint */
			mi_copyin(q, mp, 0, sizeof(uint));
			break;
		case _IOC_NR(BIOCVERSION):	/* struct bpf_version */
			size = sizeof(struct bpf_version);
			if ((db = mi_copyout_alloc(q, mp, 0, size, 1)) != NULL) {
				struct bpf_version *bv = (typeof(bv)) db->b_rptr;

				bv->bv_major = BPF_MAJOR_VERSION;
				bv->bv_minor = BPF_MINOR_VERSION;
				mi_copyout(q, mp);
			}
			break;
		case _IOC_NR(BIOCGRSIG):	/* uint */
			if ((db = mi_copyout_alloc(q, mp, 0, sizeof(uint), 1)) != NULL) {
				*(uint *) db->b_rptr = bm->bm_rsig;
				mi_copyout(q, mp);
			}
			break;
		case _IOC_NR(BIOCLOCK):	/* (none) */
			bm->bm_lock = (!0);
			mi_copy_done(q, mp, 0);
			break;
		case _IOC_NR(BIOCGTSTAMP):	/* uint */
			if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(uint), 1)) != NULL) {
				*(uint *) dp->b_rptr = bm->bm_tstamp;
				mi_copyout(q, mp);
			}
			break;
		case _IOC_NR(BIOCGETBUFMODE):	/* uint */
			if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(uint), 1)) != NULL) {
				*(uint *) dp->b_rptr = bm->bm_bufmode;
				mi_copyout(q, mp);
			}
			break;
		case _IOC_NR(BIOCSETBUFMODE):	/* uint */
			mi_copyin(q, mp, 0, sizeof(uint));
			break;
		case _IOC_NR(BIOCSETZBUF):	/* struct bpf_zbuf */
			mi_copyin(q, mp, 0, sizeof(struct bpf_zbuf));
			break;
		case _IOC_NR(BIOCGETZMAX):	/* size_t */
#ifdef __LP64__
			if (ioc->ioc_flag == IOC_ILP32) {
				if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(uint32_t), 1)) != NULL) {
					*(uint32_t *) dp->b_rptr = (PAGE_SIZE << 10);
					mi_copyout(q, mp);
				}
			} else
#endif				/* __LP64__ */
			{
				if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(size_t), 1)) != NULL) {
					*(size_t *) dp->b_rptr = (PAGE_SIZE << 10);
					mi_copyout(q, mp);
				}
			}
			break;
		case _IOC_NR(BIOCROTZBUF):	/* (none) */
			mi_copy_done(q, mp, EOPNOTSUPP);
			break;
		case _IOC_NR(BIOCGDLT):	/* uint */
		case _IOC_NR(BIOCGDLTLIST):	/* struct bpf_dltlist */
		case _IOC_NR(BIOCGETIF):	/* struct ifreq */
		case _IOC_NR(BIOCGETLIF):	/* struct lifreq */
		case _IOC_NR(BIOCGSTATS):	/* struct bpf_stat or struct bpf_stat_old */
		case _IOC_NR(BIOCGHDRCMPLT):	/* uint */
		case _IOC_NR(BIOCGSEESENT):	/* uint */
		case _IOC_NR(BIOCGDIRECTION):	/* uint */
		case _IOC_NR(BIOCGDIRFILT):	/* uint */
		case _IOC_NR(BIOCGFEEDBACK):	/* uint */
			/* handled by the driver */
			putnext(q, mp);
			break;
		}
	default:
		putnext(q, mp);
		break;
	}
}

/** bm_iocdata: - stage 2 of the transparent ioctl mechanism
  * @q: the write queue
  * @mp: the M_IOCDATA message
  */
STATIC noinline void __unlikely fastcall
bm_iocdata(queue_t *q, mblk_t *mp)
{
	struct bm *bm = (typeof(bm)) q->q_ptr;
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp, *db;
	uint val;

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case _IOC_TYPE(BIOC):
		switch (_IOC_NR(cp->cp_cmd)) {
		case _IOC_NR(BIOCGBLEN):	/* uint */
		case _IOC_NR(BIOCSBLEN):	/* uint */
		case _IOC_NR(BIOCGRTIMEOUT):	/* uint */
		case _IOC_NR(BIOCSRTIMEOUT):	/* uint */
		case _IOC_NR(BIOCGSTATS):	/* struct bpf_stat or struct bpf_stat_old */
		case _IOC_NR(BIOCGFILDROP):	/* uint */
		case _IOC_NR(BIOCSFILDROP):	/* uint */
		case _IOC_NR(BIOCIMMEDIATE):	/* uint */
		case _IOC_NR(BIOCSETF):	/* struct bpf_program */
		case _IOC_NR(BIOCSETFNR):	/* struct bpf_program */
		case _IOC_NR(BIOCSETWF):	/* struct bpf_program */
		case _IOC_NR(BIOCVERSION):	/* struct bpf_version */
		case _IOC_NR(BIOCGRSIG):	/* uint */
		case _IOC_NR(BIOCSRSIG):	/* uint */
		case _IOC_NR(BIOCLOCK):	/* (none) */
		case _IOC_NR(BIOCGDIRECTION):	/* uint */
		case _IOC_NR(BIOCSDIRECTION):	/* uint */
		case _IOC_NR(BIOCGDIRFILT):	/* uint */
		case _IOC_NR(BIOCSDIRFILT):	/* uint */
		case _IOC_NR(BIOCGETBUFMODE):	/* uint */
		case _IOC_NR(BIOCSETBUFMODE):	/* uint */
		case _IOC_NR(BIOCSETZBUF):	/* struct bpf_zbuf */
		case _IOC_NR(BIOCGETZMAX):	/* size_t */
		case _IOC_NR(BIOCROTZBUF):	/* (none) */
			switch (mi_copy_state(q, mp, &dp)) {
			case MI_COPY_CASE(MI_COPY_IN, 1):
				switch (_IOC_NR(cp->cp_cmd)) {
				case _IOC_NR(BIOCSBLEN):	/* uint */
					val = *(uint *) dp->b_rptr;
					if (val < BPF_MINBUFSIZE || val > BPF_MAXBUFSIZE) {
						mi_copy_done(q, mp, EINVAL);
						break;
					}
					bm->bm_blen = val;
					mi_copy_done(q, mp, 0);
					break;
				case _IOC_NR(BIOCSRTIMEOUT):	/* uint */
				{
					struct timeval tn;
					ulong rtimeout;

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
					if (tn.tv_sec > INT_MAX / 1000) {
						mi_copy_done(q, mp, ERANGE);
						break;
					}
					rtimeout = (tn.tv_sec * 1000) + ((tn.tv_usec + 999) / 1000);
					if (bm->bm_immediate) {
						bm->bm_rtimeout = rtimeout;
					} else if (rtimeout && !bm->bm_rtimeout) {
						if (RD(q)->q_count)
							mi_timer(bm->bm_timer, rtimeout);
						bm->bm_rtimeout = rtimeout;
					} else if (!rtimeout && bm->bm_rtimeout) {
						mi_timer_cancel(&bm->bm_timer);
						bm->bm_rtimeout = rtimeout;
					}
					mi_copy_done(q, mp, 0);
					break;
				}
				case _IOC_NR(BIOCSFILDROP):	/* uint */
					val = (*(uint *) dp->b_rptr != 0);
					if (val && !bm->bm_fildrop) {
						bm->bm_fildrop = val;
					} else if (!val && bm->bm_fildrop) {
						atomic_set(&bm->bm_filt, 0);
						bm->bm_fildrop = val;
					}
					mi_copy_done(q, mp, 0);
					break;
				case _IOC_NR(BIOCIMMEDIATE):	/* uint */
					val = (*(uint *) dp->b_rptr != 0);
					if (val && !bm->bm_immediate) {
						enableok(RD(q));
						qenable(RD(q));
						bm->bm_immediate = val;
					} else if (!val && bm->bm_immediate) {
						noenable(RD(q));
						if (bm->bm_rtimeout > 0 && RD(q)->q_count &&
						    !mi_timer_running(bm->bm_timer))
							mi_timer(bm->bm_timer, bm->bm_rtimeout);
						bm->bm_immediate = val;
					}
					mi_copy_done(q, mp, 0);
					break;
				case _IOC_NR(BIOCSETF):	/* struct bpf_program */
				case _IOC_NR(BIOCSETFNR):	/* struct bpf_program */
				case _IOC_NR(BIOCSETWF):	/* struct bpf_program */
				{
					struct bpf_program bn;
					uint len, size;
					ulong uaddr;

#ifdef __LP64__
					if (cp->cp_flag == IOC_ILP32) {
						struct bpf_program32 *bf = (typeof(bf)) dp->b_rptr;

						len = bf->bf_len;
						uaddr = (ulong) bf->bf_insns;
					} else
#endif				/* __LP64__ */
					{
						struct bpf_program *bf = (typeof(bf)) dp->b_rptr;

						len = bf->bf_len;
						uaddr = (ulong) bf->bf_insns;
					}
					if (len > BPF_MAXINSNS) {
						mi_copy_done(q, mp, ERANGE);
						break;
					}
					switch (_IOC_NR(cp->cp_cmd)) {
					case _IOC_NR(BIOCSETF):
					case _IOC_NR(BIOCSETFNR):
						/* stage a read filter */
						bm->bm_program[(bm->bm_filter + 1) & 0x1].bf_len =
						    len;
						break;
					case _IOC_NR(BIOCSETWF):
						/* stage a write filter */
						bm->bm_writprg[(bm->bm_wrprog + 1) & 0x1].bf_len =
						    len;
						break;
					default:
					}
					size = len * sizeof(struct bpf_insn);
					mi_copyin(q, mp, uaddr, size);
					break;
				}
				case _IOC_NR(BIOCSRSIG):	/* uint */
					val = *(uint *) dp->b_rptr;
					bm->bm_rsig = val & 0xff;
					mi_copy_done(q, mp, 0);
					break;
				case _IOC_NR(BIOCSTSTAMP):	/* uint */
					val = *(uint *) dp->b_rptr;
					if (unlikely(!BPF_T_VALID(val))) {
						mi_copy_done(q, mp, EINVAL);
						break;
					}
					if ((BPF_T_FORMAT(val) == BPF_T_MICROTIME &&
					     BPF_T_FORMAT(bm->bm_tstamp) != BPF_T_MICROTIME) ||
					    (BPF_T_FORMAT(val) != BPF_T_MICROTIME &&
					     BPF_T_FORMAT(bm->bm_tstamp) == BPF_T_MICROTIME)) {
						/* changing headers, flush the read queue */
						mblk_t *b;

						if ((b = allocb(2, BPRI_MED)) != NULL) {
							DB_TYPE(b) = M_FLUSH;
							b->b_wptr = b->b_rptr + 2;
							b->b_rptr[0] = FLUSHR;
							b->b_rptr[1] = 0;
							bm->bm_tstamp = val;
							putnext(q, b);
						}
					}
					bm->bm_tstamp = val;
					mi_copy_done(q, mp, 0);
					break;
				case _IOC_NR(BIOCSETBUFMODE):	/* uint */
					mi_copy_done(q, mp, EOPNOTSUPP);
					break;
				case _IOC_NR(BIOCSETZBUF):	/* struct bpf_zbuf */
					mi_copy_done(q, mp, EOPNOTSUPP);
					break;
				case _IOC_NR(BIOCGBLEN):	/* uint */
				case _IOC_NR(BIOCGRTIMEOUT):	/* uint */
				case _IOC_NR(BIOCGSTATS):	/* struct bpf_stat or bpf_stat_old */
				case _IOC_NR(BIOCGFILDROP):	/* uint */
				case _IOC_NR(BIOCIMMEDIATE):	/* uint */
				case _IOC_NR(BIOCVERSION):	/* struct bpf_version */
				case _IOC_NR(BIOCGRSIG):	/* uint */
				case _IOC_NR(BIOCLOCK):	/* (none) */
				case _IOC_NR(BIOCGDIRECTION):	/* uint */
				case _IOC_NR(BIOCGDIRFILT):	/* uint */
				case _IOC_NR(BIOCGETBUFMODE):	/* uint */
				case _IOC_NR(BIOCGETZMAX):	/* size_t */
				case _IOC_NR(BIOCROTZBUF):	/* (none) */
					mi_copy_done(q, mp, EPROTO);
					break;
				default:
					putnext(q, mp);
					break;
				}
				break;
			case MI_COPY_CASE(MI_COPY_IN, 2):
				switch (_IOC_NR(cp->cp_cmd)) {
				case _IOC_NR(BIOCSETWF):	/* bpf_program */
					/* same as BIOCSETF, just for write side */
				{
					struct bpf_program *bf =
					    &bm->bm_writprg[(bm->bm_wrprog + 1) & 0x1];
					mblk_t *fp;

					if (MBLKL(dp) != bf->bf_len * sizeof(struct bpf_insn)) {
						mi_copy_done(q, mp, ERANGE);
						break;
					}
					if ((fp = allocb(q, 2)) == NULL) {
						mi_copy_done(q, mp, ENOBUFS);
						break;
					}
					DB_TYPE(fp) = M_FLUSH;
					fp->b_wptr = fp->b_rptr + 2;
					fp->b_rptr[0] = FLUSHW;
					fp->b_rptr[1] = 0;

					bcopy(dp->b_rptr, bf, MBLKL(dp));
					if (!bm_valid_program(bf)) {
						freemsg(fp);
						mi_copy_done(q, mp, EINVAL);
						break;
					}
					bzero(bm->bm_wrmstore, sizeof(bm->bm_wrmstore));
					bm->bm_wrprog = (bm->bm_wrprog + 1) & 0x01;
					flushq(q, FLUSHDATA);
					putnext(q, fp);
					mi_copy_done(q, mp, 0);
					break;
				}
				case _IOC_NR(BIOCSETFNR):	/* bpf_program */
					/* same as BIOCSETF, just don't flush */
				case _IOC_NR(BIOCSETF):	/* bpf_program */
				{
					struct bpf_program *bf =
					    &bm->bm_program[(bm->bm_filter + 1) & 0x1];
					mblk_t *fp = NULL;

					if (MBLKL(dp) != bf->bf_len * sizeof(struct bpf_insn)) {
						mi_copy_done(q, mp, ERANGE);
						break;
					}
					if (_IOC_NR(cp->cp_cmd) == _IOC_NR(BIOCSETF)) {
						if ((fp = allocb(q, 2)) == NULL) {
							mi_copy_done(q, mp, ENOBUFS);
							break;
						}
						DB_TYPE(fp) = M_FLUSH;
						fp->b_wptr = fp->b_rptr + 2;
						fp->b_rptr[0] = FLUSHR;
						fp->b_rptr[1] = 0;
					}
					bcopy(dp->b_rptr, bf, MBLKL(dp));
					if (!bm_valid_program(bf)) {
						freemsg(fp);
						mi_copy_done(q, mp, EINVAL);
						break;
					}
					bzero(bm->bm_memstore, sizeof(bm->bm_memstore));
					bm->bm_filter = (bm->bm_filter + 1) & 0x01;
					if (fp != NULL)
						putnext(q, fp);
					mi_copy_done(q, mp, 0);
					break;
				}
				case _IOC_NR(BIOCSBLEN):	/* uint */
				case _IOC_NR(BIOCSRTIMEOUT):	/* uint */
				case _IOC_NR(BIOCSFILDROP):	/* uint */
				case _IOC_NR(BIOCIMMEDIATE):	/* uint */
				case _IOC_NR(BIOCSRSIG):	/* uint */
				case _IOC_NR(BIOCSDIRECTION):	/* uint */
				case _IOC_NR(BIOCSDIRFILT):	/* uint */
				case _IOC_NR(BIOCSETBUFMODE):	/* uint */
				case _IOC_NR(BIOCSETZBUF):	/* struct bpf_zbuf */
					mi_copy_done(q, mp, EPROTO);
					break;
				case _IOC_NR(BIOCGBLEN):	/* uint */
				case _IOC_NR(BIOCGRTIMEOUT):	/* uint */
				case _IOC_NR(BIOCGSTATS):	/* struct bpf_stat or bpf_stat_old */
				case _IOC_NR(BIOCGFILDROP):	/* uint */
				case _IOC_NR(BIOCVERSION):	/* struct bpf_version */
				case _IOC_NR(BIOCGRSIG):	/* uint */
				case _IOC_NR(BIOCLOCK):	/* (none) */
				case _IOC_NR(BIOCGDIRECTION):	/* uint */
				case _IOC_NR(BIOCGDIRFILT):	/* uint */
				case _IOC_NR(BIOCGETBUFMODE):	/* uint */
				case _IOC_NR(BIOCGETZMAX):	/* size_t */
				case _IOC_NR(BIOCROTZBUF):	/* (none) */
					mi_copy_done(q, mp, EPROTO);
					break;
				default:
					putnext(q, mp);
					break;
				}
				break;
			case MI_COPY_CASE(MI_COPY_OUT, 1):
				switch (_IOC_NR(cp->cp_cmd)) {
				case _IOC_NR(BIOCGBLEN):	/* uint */
				case _IOC_NR(BIOCGRTIMEOUT):	/* uint */
				case _IOC_NR(BIOCVERSION):	/* struct bpf_version */
				case _IOC_NR(BIOCGRSIG):	/* uint */
					mi_copy_done(q, mp, 0);
					break;
				case _IOC_NR(BIOCSBLEN):	/* uint */
				case _IOC_NR(BIOCSRTIMEOUT):	/* uint */
				case _IOC_NR(BIOCIMMEDIATE):	/* uint */
				case _IOC_NR(BIOCSETF):	/* struct bpf_program */
				case _IOC_NR(BIOCSRSIG):	/* uint */
					mi_copy_done(q, mp, EPROTO);
					break;
				default:
					putnext(q, mp);
					break;
				}
				break;
			case MI_COPY_CASE(MI_COPY_OUT, 2):
				switch (_IOC_NR(cp->cp_cmd)) {
					mi_copy_done(q, mp, EPROTO);
					break;
				default:
					putnext(q, mp);
					break;
				}
				break;
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EPROTO);
				break;
			}
			break;
		case _IOC_NR(BIOCFLUSH):
		case _IOC_NR(BIOCSTCPF):
		case _IOC_NR(BIOCSUDPF):
		case _IOC_NR(BIOCGDLT):
		case _IOC_NR(BIOCSDLT):
		case _IOC_NR(BIOCGDLTLIST):
		case _IOC_NR(BIOCPROMISC):
		case _IOC_NR(BIOCGETIF):
		case _IOC_NR(BIOCSETIF):
		case _IOC_NR(BIOCGSTATS):
		case _IOC_NR(BIOCSHDRCMPLT):
		case _IOC_NR(BIOCGHDRCMPLT):
		case _IOC_NR(BIOCGSEESENT):
		case _IOC_NR(BIOCSSEESENT):
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
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

/** bm_wput: - write put procedure.
  * @q: write queue.
  * @mp: message to put.
  *
  * This is largely a canonical write put procedure with expedited M_IOCTL/M_IOCDATA processing.
  * We also process M_READ messages and check whether filtering is required on M_DATA messages.
  * When write filtering is required, we always queue the M_DATA message blocks.
  */
STATIC streamscall __hot_write int
bm_wput(queue_t *q, mblk_t *mp)
{
	struct bm *bm = (typeof(bm)) q->q_ptr;
	unsigned char db_type = DB_TYPE(mp);

	switch (__builtin_expect(db_type, M_DATA)) {
	case M_DATA:
		/* only queue these when necessary or when we have a write filter program */
		mp->b_band = 0;
		if (bm->bm_writprg.bf_len == 0
		    && (q->q_first == NULL && !(q->q_flag & QSVCBUSY) && canputnext(q))) {
			putnext(q, mp);
			break;
		}
		putq(q, mp);
		break;

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
		bm_ioctl(q, mp);
		break;
	case M_IOCDATA:
		bm_iocdata(q, mp);
		break;
	case M_READ:
		/* save these out of band */
		if (bm->bm_mread)
			freemsg(XCHG(&bm->sb_mread, NULL));
		bm->sb_mread = mp;
		if (bm->bm_immediate)
			qenable(bm->bm_rq);
		else if (bm->bm_rtimeout > 0)
			mi_timer(bm->bm_timer, bm->bm_rtimeout);
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

/** bm_wsrv: - write service procedure.
  * @q: the write queue.
  *
  * This is a canonical write service procedure.  It simply passes messages along under flow
  * control.  A check must be made for M_IOCTL and M_IOCDATA because the mi_copyin functions
  * sometimes places one of these messages on the queue.  We also check for M_DATA whether there is
  * a write filter program and run it when there is.
  */
STATIC streamscall __hot_read int
bm_wsrv(queue_t *q)
{
	struct bm *bm = (typeof(bm)) q->q_ptr;
	const struct bpf_program *const bf = &bm->bm_writprg[bm->bm_wrprog];
	mblk_t *mp;

	while ((mp = getq(q))) {
		unsigned char db_type = DB_TYPE(mp);

		switch (__builtin_expect(db_type, M_DATA)) {
		case M_DATA:
			if (unlikely(bf->bf_len > 0)) {
				if (unlikely
				    (!bpf_filter(bf->bf_insns, bm->bm_wrmstore, mp, msgdsize(mp))))
				{
					freemsg(mp);
					continue;
				}
			}
			if (canputnext(q)) {
				putnext(q, mp);
				continue;
			}
			putbq(q, mp);
			break;
		case M_IOCTL:
			bm_ioctl(q, mp);
			continue;
		case M_IOCDATA:
			bm_iocdata(q, mp);
			continue;
		default:
			if (unlikely(db_type >= QPCTL) || likely(bcanputnext(q, mp->b_band))) {
				putnext(q, mp);
				continue;
			}
			putbq(q, mp);
			break;
		}
		break;
	}
	return (0);
}

/** bm_msgsizelast: -
  * @mp: the message to size
  * @mlen: (return) message length
  * @epp: (return) last message block in chain
  *
  * Calculate the length of the message (not including the header and header padding), and find the
  * pointer to the last message block in the message.
  */
static inline fastcall __hot int
bm_msgsizelast(mblk_t *mp, mblk_t **epp)
{
	register mblk_t *b, *e;
	register ssize_t len;
	register ssize_t dlen;

	for (len = 0, b = mp->b_cont, e = b; b; b = b->b_cont) {
		e = b;
		if (likely((dlen = MBLKL(b)) >= 0))
			len += dlen;
		else
			b->b_wptr = b->b_rptr;	/* fix it */
	}
	*epp = e;
	return (len);
}

static noinline fastcall __hot int
pkt(const mblk_t *mp, const uint offs, uint *reg, const uint n)
{
	register const mblk_t *b = mp;
	register uint result = 0;
	register int i = 0;
	register unsigned char *p;
	register uint olen = offs;

	do {
		if (unlikely(b->b_wptr <= b->b_rptr))
			continue;
		for (p = b->b_rptr + olen + i; i < n && likely(p < b->b_wptr); p++, i++) {
			result <<= 8;
			result |= *p;
		}
		if (likely(i == n)) {
			*reg = result;
			return (0);
		}
		olen -= b->b_wptr - b->b_rptr;
	}
	while (unlikely((b = b->b_cont) != NULL));
	return (-1);
}

static noinline fastcall __hot uint
bpf_filter(register const struct bpf_insn *p, int *m, register const mblk_t *mp, uint l)
{
	register uint a, x, k;

	for (a = 0, x = 0, k = p->k;; ++p, k = p->k) {
		/* *INDENT-OFF* */
		switch (BPF_CLASS(p->code)) {
		case BPF_LD: switch (BPF_MODE(p->code)) {
			case BPF_IMM: a = k; continue;
			case BPF_ABS: switch(BPF_SIZE(p->code)) {
				case BPF_W: if (pkt(mp, k, &a, 4)) break; continue;
				case BPF_H: if (pkt(mp, k, &a, 2)) break; continue;
				case BPF_B: if (pkt(mp, k, &a, 1)) break; continue;
				} break;
			case BPF_IND: switch(BPF_SIZE(p->code)) {
				case BPF_W: if (pkt(mp, x + k, &a, 4)) break; continue;
				case BPF_H: if (pkt(mp, x + k, &a, 2)) break; continue;
				case BPF_B: if (pkt(mp, x + k, &a, 1)) break; continue;
				} break;
			case BPF_MEM: a = m[k]; continue;
			case BPF_LEN: a = l; continue;
			} break;
		case BPF_LDX: switch (BPF_MODE(p->code)) {
			case BPF_IMM: x = k; continue;
			case BPF_MEM: x = m[k]; continue;
			case BPF_LEN: x = l; continue;
			case BPF_MSH: if (pkt(mp, k, &x, 1)) break; x = (x & 0xf) << 2; continue;
			} break;
		case BPF_ST:  m[k] = a; continue;
		case BPF_STX: m[k] = x; continue;
		case BPF_ALU: switch (BPF_OP(p->code)) {
			case BPF_ADD: switch (BPF_SRC(p->code)) {
				case BPF_X: a += x; continue;
				case BPF_K: a += k; continue;
				} break;
			case BPF_SUB: switch (BPF_SRC(p->code)) {
				case BPF_X: a -= x; continue;
				case BPF_K: a -= k; continue;
				} break;
			case BPF_MUL: switch (BPF_SRC(p->code)) {
				case BPF_X: a *= x; continue;
				case BPF_K: a *= k; continue;
				} break;
			case BPF_DIV: switch (BPF_SRC(p->code)) {
				case BPF_X: if (x == 0) break; a /= x; continue;
				case BPF_K: a /= k; continue;
				} break;
			case BPF_AND: switch (BPF_SRC(p->code)) {
				case BPF_X: a &= x; continue;
				case BPF_K: a &= k; continue;
				} break;
			case BPF_OR: switch (BPF_SRC(p->code)) {
				case BPF_X: a |= x; continue;
				case BPF_K: a |= k; continue;
				} break;
			case BPF_LSH: switch (BPF_SRC(p->code)) {
				case BPF_X: a <<= x; continue;
				case BPF_K: a <<= k; continue;
				} break;
			case BPF_RSH: switch (BPF_SRC(p->code)) {
				case BPF_X: a >>= x; continue;
				case BPF_K: a >>= k; continue;
				} break;
			case BPF_NEG: a = -a; continue;
			} break;
		case BPF_JMP: switch (BPF_OP(p->code)) {
			uchar jf = p->jf, jt = p->jt;
			case BPF_JA: p += k; continue;
			case BPF_JEQ: switch (BPF_SRC(p->code)) {
				case BPF_X: p += (a == x) ? jt : jf; continue;
				case BPF_K: p += (a == k) ? jt : jf; continue;
				} break;
			case BPF_JGT: switch (BPF_SRC(p->code)) {
				case BPF_X: p += (a > x) ? jt : jf; continue;
				case BPF_K: p += (a > k) ? jt : jf; continue;
				} break;
			case BPF_JGE: switch (BPF_SRC(p->code)) {
				case BPF_X: p += (a >= x) ? jt : jf; continue;
				case BPF_K: p += (a >= k) ? jt : jf; continue;
				} break;
			case BPF_JSET: switch (BPF_SRC(p->code)) {
				case BPF_X: p += (a & x) ? jt : jf; continue;
				case BPF_K: p += (a & k) ? jt : jf; continue;
				} break;
			} break;
		case BPF_RET: switch (BPF_SRC(p->code)) {
			case BPF_A: return (uint) a;
			case BPF_K: return (uint) k;
			} break;
		case BPF_MISC: switch (BPF_MISCOP(p->code)) {
			case BPF_TAX: x = a; continue;
			case BPF_TXA: a = x; continue;
			} break;
		default: break;
		}
		/* *INDENT-ON* */
		return (0);
	}
}

static inline fastcall __hot int
bm_adjsize(const struct bm *bm, mblk_t *mp, size_t clen)
{
	register mblk_t *b, *e;
	register ssize_t olen, mlen, dlen;
	ssize_t tlen;

	for (olen = 0, mlen = 0, e = b = mp; b; e = b, b = b->b_cont) {
		if ((dlen = MBLKL(b)) > 0) {
			olen += dlen;
			if (mlen + dlen <= clen)
				mlen += dlen;
			else {
				b->b_wptr = b->b_rptr + (clen - mlen);
				mlen = clen;
			}
		} else
			b->b_wptr = b->b_rptr;
	}

}

/** bm_filter: - filter a BPF module message
  * @bm: BPF private structure
  * @mp: the message to filter.
  * @mlen: the original message length.
  *
  * This function needs to execute the filter program (if any) on the message, truncate the message
  * if the filter program return code specifies truncation, adjust the capture length in the header,
  * pad the message for alignment (trailing padding), and return the size of the resulting message.
  * When the size is zero (0), the message is to be discarded.  When the size is less than zero, an
  * error occured and the message should be requeued.
  */
static noinline fastcall __hot int
bm_filter(struct bm *bm, mblk_t *mp, int mlen)
{
	struct bpf_program *bf = &bm->bm_program[bm->bm_filter];
	int clen = mlen;

	if (bf->bf_len > 0)
		clen = bpf_filter(bf->bf_insns, bm->bm_memstore, mp->b_cont, mlen);
	if (clen) {
		struct bpf_hdr *bh = (typeof(bh)) mp->b_rptr;

		bh->bh_caplen = clen;
		bh->bh_datalen = mlen;
	}
	mlen = bm_adjsize(bm, mp, clen);
	return (mlen);
}

/** bm_process: - process a BPF module message
  * @bm: BPF private structure
  * @mp: the message to process
  * @mlen: packet payload bytes in message.
  *
  * This procedure calculates the message length of the payload part of the packet (without the
  * header or header padding) and returns it.  It also returns a pointer to the last message block
  * in the chain.  It is responsible for filtering the packet and adjusting the capture length and
  * adding aligment padding to the end of the message.  If the returned message length is less than
  * zero, an error occured and the message should be requeued.
  */
static noinline streams_fastcall ssize_t
bm_process(struct bm *bm, mblk_t *mp, int mlen)
{
	if (!(mp->b_flag & MSGFILTERED)) {
		if ((mlen = bm_filter(bm, mp, mlen)) == 0) {
			freemsg(mp);
			return (0);
		}
		mp->b_flag |= MSGFILTERED;
	}
	return (mlen);
}

/** bm_rsrv: - read service procedure.
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
bm_rsrv(queue_t *q)
{
	struct bm *bpw = (typeof(bpw)) q->q_ptr;
	const struct bm *bm = bpw;
	int blen = bm->bm_blen;
	mblk_t *mp, *mr;
	int partial_ok = 0;

	spin_lock(&bpw->bm_lock);
	if (likely((mr = XCHG(&bpw->bm_mread, NULL)) != NULL)) {
		long rlen = *(long *) mr->b_rptr;

		if (unlikely(rlen < bm->bm_blen)) {
			spin_unlock(&bpw->bm_lock);
			DB_TYPE(mr) = M_ERROR;
			mr->b_band = 0;
			mr->b_wptr = mr->b_rptr + 2;
			mr->b_rptr[0] = 0;
			mr->b_rptr[1] = EIO;
			qreply(q, mr);
			return (0);
		}
		blen = rlen;
	}

	while ((mp = getq(q)) != NULL) {
		switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
		case M_DATA:
			if (!bm->bm_immediate) {
				mblk_t *lp;
				int mlen;

				mlen = bm_msgsizelast(mp, &lp);
				atomic_sub(&bm->bm_count, mlen);
				mlen = bm_process(bm, mp, mlen);
				if (mlen == 0)
					continue;
				if (mlen < blen) {
					mblk_t *dp;

					while ((dp = getq(q))) {
						if (likely(DB_TYPE(dp) == M_DATA)) {
							mblk_t *ep;
							int dlen;

							dlen = bm_msgsizelast(d, &ep);
							atomic_sub(&bm->bm_count, dlen);
							dlen = bm_process(bm, dp, dlen);
							if (dlen == 0)
								continue;
							if (mlen + dlen <= blen) {
								/* chunk it up */
								mlen += dlen;
								lp->b_cont = dp;
								lp = ep;
								mp->b_csum += dp->b_csum;
								continue;
							}
							if (bm->bm_rtimeout > 0
							    && !mi_timer_running(bm->bm_timer))
								mi_timer(bm->bm_timer,
									 bm->bm_rtimeout);
							atomic_add(&bm->bm_count, dlen);
						}
						putbq(q, dp);
						break;
					}
					/* not full and no timeout, stop here */
					if (dp == NULL && !(partial_ok = xchg(&bm->bm_timeout, 0))) {
						if (bm->bm_rtimeout > 0
						    && !mi_timer_running(bm->bm_timer))
							mi_timer(bm->bm_timer, bm->bm_rtimeout);
						atomic_add(&bm->bm_count, mlen);
						break;
					}
					/* nothing left - let it go */
				}
				partial_ok = 0;
				bm->bm_timedout = 0;
			}
			if (canputnext(q)) {
				mp->b_flag |= MSGDELIM;
				mp->b_flag &= ~(MSGHEADER | MSGFILTERED);
				atomic_add(&bm->bm_capt, XCHG(&mp->b_csum, 0));
				putnext(q, mp);
				if (mr != NULL) {
					/* Transform to zero-length delimited M_DATA to
					   unblock the read at the Stream head. */
					DB_TYPE(mr) = M_DATA;
					mr->b_flag |= MSGDELIM;
					mr->b_wptr = mr->b_rptr;
					putnext(q, XCHG(&mr, NULL));
				}
				continue;
			}
			atomic_add(&bm->bm_count, mlen);
			break;

		case M_PCSIG:
			if (mi_timer_valid(mp))
				bm->bm_timedout = 1;
			continue;

		default:
			if (DB_TYPE(mp) >= QPCTL || bcanputnext(q, mp->b_band)) {
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
	bm->bm_timedout = 0;
	return (0);
}

/*
 * I find it rather amazing that the noenable(9) and enableok(9) queue functions were provided
 * explicitly for the types of message coalescing being performed by bpfmod(4), and yet, Sun does
 * not use them, but, rather breaks flow control by using an out-of-band mechanism.
 */

/** bm_rput: - STREAMS buffer module read-side put procedure
  * @q: the read queue
  * @mp: the message to put
  *
  * M_DATA:
  *	All packets are passed up as M_DATA messages.  Any header padding that is required for
  *	network alignment in accordance with BPF format is counted in the mp->b_csum field of the
  *	first message block.
  *
  * M_FLUSH:
  *	Processed as normal; however, when band 0 data is flushed, statistics counts are reset.
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
bm_rput(queue_t *q, mblk_t *mp)
{
	struct bm *bm = (typeof(bm)) q->q_ptr;
	unsigned char db_type = DB_TYPE(mp);

	switch (__builtin_expect(db_type, M_DATA)) {
	case M_DATA:
	{
		size_t mlen, hlen;
		mblk_t *hp;
		struct timespec ts;

		switch (__builtin_expect(BPF_T_FORMAT(bm->bm_tstamp), BPF_T_MICROTIME)) {
		case BPF_T_MICROTIME:
		case BPF_T_NONE:
			hlen = sizeof(struct bpf_hdr) + mp->b_csum;
			break;
		case BPF_T_NANOTIME:
		case BPF_T_BINTIME:
			hlen = sizeof(struct bpf_xhdr) + mp->b_csum;
			break;
		}

		if (unlikely((hp = allocb(hlen, BPRI_MED)) == NULL)) {
			atomic_inc(&bm->bm_drop);
			freemsg(mp);
			return (0);
		}
		bzero(hp->b_rptr, hlen);
		hp->b_wptr = hp->b_rptr + hlen;

		if (likely(bm->bm_tstamp != BPF_T_NONE)) {
			switch (__builtin_expect(BPF_T_FLAG(bm->bm_tstamp), BPF_T_NORMAL)) {
			case BPF_T_NORMAL:
				getnstimeofday(&ts);
				break;
			case BPF_T_FAST:

				ts.tv_sec =
				    bm->bm_ts.tv_sec + (uint64_t) drv_hztomsec(jiffies) / 1000;
				if ((ts.tv_nsec =
				     bm->bm_ts.tv_nsec +
				     ((uint64_t) drv_hztousec(jiffies) % 1000000) * 1000) >=
				    (uint64_t) 1000000000LL) {
					ts.tv_sec++;
					ts.tv_nsec -= (uint64_t) 1000000000LL;
				}
				break;
			case BPF_T_MONOTONIC:
				/* time from boot */
				getnstimeofday(&ts);
				ts.tv_sec -= bm->bm_ts.tv_sec;
				if ((ts.tv_nsec -= bm->bm_ts.tv_nsec) < 0) {
					ts.tv_sec--;
					ts.tv_nsec += (uint64_t) 1000000000LL;
				}
				break;
			case BPF_T_MONOTONIC_FAST:
				ts.tv_sec = (uint64_t) drv_hztomsec(jiffies) / 1000;
				ts.tv_nsec = ((uint64_t) drv_hztousec(jiffies) % 1000000) * 1000;
				break;
			}
		}

		mlen = msgdsize(mp);

		switch (__builtin_expect(BPF_T_FORMAT(bm->bm_tstamp), BPF_T_MICROTIME)) {
		case BPF_T_MICROTIME:
		case BPF_T_NONE:
		{
			struct bpf_hdr *bh;

			bh = (typeof(bh)) hp->b_rptr;
			switch (__builtin_expect(BPF_T_FORMAT(bm->bm_tstamp), BPF_T_MICROTIME)) {
			case BPF_T_MICROTIME:
				bh->bh_tstamp.tv_sec = ts.tv_sec;
				bh->bh_tstamp.tv_usec = ts.tv_nsec / 1000;
				break;
			case BPF_T_NONE:
				bh->bh_tstamp.bt_sec = 0;
				bh->bh_tstamp.bt_frac = 0;
				break;
			}
			bh->bh_caplen = mlen;
			bh->bh_datalen = mlen;
			bh->bh_hdrlen = hlen;
			break;
		}
		case BPF_T_NANOTIME:
		case BPF_T_BINTIME:
		{
			struct bpf_xhdr *bh;

			bh = (typeof(bh)) hp->b_rptr;
			switch (__builtin_expect(BPF_T_FORMAT(bm->bm_tstamp), BPF_T_NANOTIME)) {
			case BPF_T_NANOTIME:
				bh->bh_tstamp.bt_sec = ts.tv_sec;
				bh->bh_tstamp.bt_frac = ts.tv_nsec;
				break;
			case BPF_T_BINTIME:
				bh->bh_tstamp.bt_sec = ts.tv_sec;
				bh->bh_tstamp.bt_frac = ts.tv_nsec * (uint64_t) 18446744073LL;
				break;
			}
			bh->bh_caplen = mlen;
			bh->bh_datalen = mlen;
			bh->bh_hdrlen = hlen;
			break;
		}
		}
		hp->b_cont = mp;
		mp = hp;

		mp->b_flag &= ~MSGFILTERED;
		mp->b_csum = 1;

		if (!bm->bm_immediate && bm->bm_rtimeout > 0 && atomic_read(&bm->bm_count) == 0)
			mi_timer(bm->bm_timer, bm->bm_rtimeout);
		if (atomic_add_return(&bm->bm_count, mlen) > bm->bm_blen) {
			if (!bm->bm_immediate) {
				if (bm->bm_rtimeout > 0)
					mi_timer_cancel(bm->bm_timer);
				putq(q, mp);
				qenable(q);
			} else
				putq(q, mp);
		} else
			putq(q, mp);
		break;
	}

	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if ((mp->b_rptr[0] & FLUSHBAND) && mp->b_rptr[1] != 0)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else {
				flushq(q, FLUSHDATA);

				if (!bm->bm_immediate && bm->bm_rtimeout > 0)
					mi_timer_cancel(bm->bm_timer);

				atomic_set(&bm->bm_count, 0);
				atomic_set(&bm->bm_drop, 0);
				atomic_set(&bm->bm_capt, 0);
			}
		}
		putnext(q, mp);
		break;

	case M_COPYOUT:
		bm_copyout(q, mp);
		break;

	case M_IOCACK:
		bm_iocack(q, mp);
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
		if (mp->b_band == 0 && !bm->bm_immediate) {
			if (bm->bm_rtimeout > 0)
				mi_timer_cancel(bm->bm_timer);
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

STATIC streamscall int __unlikely
bm_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;
	struct bm *bm;
	mblk_t *tb, *mp;
	struct bpf_insn *p;
	struct stroptions *so;
	static const size_t psize = BPF_MAXINSNS * sizeof(struct bpf_insn);
	int err;

	if (q->q_ptr)
		return (0);
	if (sflag != MODOPEN)
		return (ENXIO);
	if ((tb = mi_timer_alloc(0)) == NULL)
		return (ENOSR);
	if ((mp = allocb(sizeof(*so), BPRI_WAITOK)) == NULL) {
		mi_timer_free(tb);
		return (ENOSR);
	}
	/* note that programs are exactly one 4096-byte page */
	if ((p = (typeof(p)) kmem_alloc(4 * psize, KM_SLEEP)) == NULL) {
		freeb(mp);
		mi_timer_free(tb);
		return (ENOMEM);
	}
	write_lock(&bm_open_lock);
	if ((err = mi_open_comm(&bm_opens, sizeof(*bm), q, devp, oflag, sflag, crp))) {
		write_unlock(&bm_open_lock);
		kmem_free(p, psize);
		freeb(mp);
		mi_timer_free(tb);
		return (err);
	}

	/* inherit packet sizes of what we are pushed over */
	wq = WR(q);
	wq->q_minpsz = wq->q_next->q_minpsz;
	wq->q_maxpsz = wq->q_next->q_maxpsz;

	bm = (typeof(bm)) q->q_ptr;
	bzero(bm, sizeof(*bm));
	bm->bm_rq = q;
	atomic_set(&bm->bm_count, 0);
	atomic_set(&bm->bm_drop, 0);
	atomic_set(&bm->bm_capt, 0);
	atomic_set(&bm->bm_filt, 0);
	bm->bm_blen = BPF_DFLTBUFSIZE;
	/* calculate boot tick time for monotonic normal clock */
	getnstimeofday(&bm->bm_ts);
	bm->bm_ts.tv_sec -= (uint64_t) drv_hztomsec(jiffies) / 1000;
	if ((bm->bm_ts.tv_nsec -= ((uint64_t) drv_hztousec(jiffies) % 1000000) * 1000) < 0) {
		bm->bm_ts.tv_sec--;
		bm->bm_ts.tv_nsec += (uint64_t)1000000000LL;
	}
	bm->bm_timer = tb;
	bm->bm_program[0].bf_insns = p;
	bm->bm_program[1].bf_insns = (p =+ BPF_MAXINSNS);
	bm->bm_writprg[0].bf_insns = (p =+ BPF_MAXINSNS);
	bm->bm_writprg[1].bf_insns = (p =+ BPF_MAXINSNS);

	write_unlock(&bm_open_lock);

	DB_TYPE(mp) = M_SETOPTS;
	mp->b_wptr = mp->b_rptr + sizeof(*so);
	bzero(mp->b_rptr, sizeof(*so));
	so = (typeof(so)) mp->b_rptr;
	so->so_flags = 0;
	so->so_flags |= SO_MREADON;
	so->so_readopt = RFILL;
	so->so_flags |= SO_READOPT;
	so->so_lowat = SNIT_LOWAT(BPF_DFLTBUFSIZE, 1);	/* 32767 + 256 */
	so->so_flags |= SO_LOWAT;
	so->so_hiwat = SNIT_HIWAT(BPF_DFLTBUFSIZE, 1);	/* 65536 + 512 */
	so->so_flags |= SO_HIWAT;
	putnext(q, mp);

	qprocson(q);
	noenable(q);
	return (0);
}

STATIC streamscall int
bm_close(queue_t *q, int oflag, cred_t *crp)
{
	struct bm *bm = (typeof(bm)) q->q_ptr;
	static const size_t psize = BPF_MAXINSNS * sizeof(struct bpf_insn);
	struct stroptions *so;
	mblk_t *mp;

	/* set stream head back where it was */
	if ((mp = allocb(sizeof(*so), BPRI_WAITOK)) != NULL) {
		DB_TYPE(mp) = M_PCSETOPTS;
		bzero(mp->b_rptr, sizeof(*so));
		so = (typeof(so)) mp->b_rptr;
		mp->b_wptr = mp->b_rptr + sizeof(*so);
		so->so_flags = 0;
		so->so_flags |= SO_MREADOFF;
		so->so_readopt = RNORM;
		so->so_flags |= SO_READOPT;
		so->so_lowat = SHEADLOWAT;	/* SHEADHIWAT = 8192 */
		so->so_flags |= SO_LOWAT;
		so->so_hiwat = SHEADHIWAT;	/* SHEADHIWAT = 65536 */
		so->so_flags |= SO_HIWAT;
		putnext(q, mp);
	}
	qprocsoff(q);
	if (bm->bm_mread != NULL)
		freemsg(XCHG(&bm->bm_mread, NULL));
	if (bm->bm_timer != NULL)
		mi_timer_free(XCHG(&bm->bm_timer, NULL));
	if (bm->bm_program[0].bf_insns != NULL)
		kmem_free(XCHG(&bm->bm_program[0].bf_insns, NULL), psize);
	if (bm->bm_program[1].bf_insns != NULL)
		kmem_free(XCHG(&bm->bm_program[1].bf_insns, NULL), psize);
	write_lock(&bm_open_lock);
	mi_close_com(&bm_opens, q);
	write_unlock(&bm_open_lock);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
#ifdef LINUX
STATIC struct qinit bm_rinit = {
	.qi_putp = bm_rput,
	.qi_srvp = bm_rsrv,
	.qi_qopen = bm_open,
	.qi_qclose = bm_close,
	.qi_minfo = &bm_minfo,
	.qi_mstat = &bm_rstat,
};

STATIC struct qinit bm_winit = {
	.qi_putp = bm_wput,
	.qi_srvp = bm_wsrv,
	.qi_minfo = &bm_minfo,
	.qi_mstat = &bm_wstat,
};

#ifdef CONFIG_STREAMS_BPFMOD_MODULE
STATIC
#endif
struct streamtab bpfmodinfo = {
	.st_rdinit = &bm_rinit,
	.st_wrinit = &bm_winit,
};

STATIC struct fmodsw bm_fmod = {
	.f_name = CONFIG_STREAMS_BPFMOD_NAME,
	.f_str = &bpfmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef __LP64__
struct bm_trans {
	unsigned int cmd;
	void *opaque;
};

STATIC struct bm_trans bm_trans_map[] = {
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
bm_ioctl32_unregister(void)
{
	struct bm_trans *t;

	for (t = bm_trans_map; t->cmd != 0; t++) {
		unregister_ioctl32(t->opaque);
		t->opaque = NULL;
	}
	return;
}

STATIC __unlikely int
bm_ioctl32_register(void)
{
	struct bm_trans *t;

	for (t = bm_trans_map; t->cmd != 0; t++) {
		if ((t->opaque = register_ioctl32(t->cmd)) == NULL) {
			bm_ioctl32_unregister();
			return (-ENOMEM);
		}
	}
	return (0);
}
#endif				/* __LP64__ */

#ifdef CONFIG_STREAMS_BPFMOD_MODULE
STATIC
#endif
int __init
bpfmodinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_BPFMOD_MODULE
	printk(KERN_INFO BPFMOD_BANNER);
#else
	printk(KERN_INFO BPFMOD_SPLASH);
#endif
	bm_minfo.mi_idnum = modid;
#ifdef __LP64__
	if ((err = bm_ioctl32_register())) {
		cmn_err(CE_WARN, "%s: could not register 32bit ioctls, err = %d",
			CONFIG_STREAMS_BPFMOD_NAME, err);
		return (err);
	}
#endif				/* __LP64__ */
	if ((err = register_strmod(&bm_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d",
			CONFIG_STREAMS_BPFMOD_NAME, err);
#ifdef __LP64__
		bm_ioctl32_unregister();
#endif				/* __LP64__ */
		return (err);
	}
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_BPFMOD_MODULE
STATIC
#endif
void __exit
bpfmodexit(void)
{
	int err;

#ifdef __LP64__
	bm_ioctl32_unregister();
#endif				/* __LP64__ */
	if ((err = unregister_strmod(&bm_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d",
			CONFIG_STREAMS_BPFMOD_NAME, err);
		return (void) (err);
	}
	return (void) (0);
}

#ifdef CONFIG_STREAMS_BPFMOD_MODULE
module_init(bpfmodinit);
module_exit(bpfmodexit);
#endif
#endif				/* LINUX */
