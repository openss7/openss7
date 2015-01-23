/*****************************************************************************

 @(#) File: src/modules/nit_pf.c

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

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

/*
 *  This is NIT_PF, a classical STREAMS packet filter module similar to pfmod(4) that performs the
 *  actions described in nit_pf(4).  Note that this is a stripped down version of pfmod(4).
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#define _SRVR_SOURCE
#define _MPS_SOURCE
#define _SUN_SOURCE

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/strlog.h>
#include <sys/cmn_err.h>

#include <net/nit_pf.h>

#include "sys/config.h"

#define PF_DESCRIP	"SVR 4.1 SNIT Packet Filter (PF) STREAMS Module"
#define PF_EXTRA	"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PF_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define PF_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define PF_DEVICE	"SVR 4.1 SNIT Packet Filter (PF) for STREAMS."
#define PF_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define PF_LICENSE	"GPL"
#define PF_BANNER	PF_DESCRIP	"\n" \
			PF_EXTRA	"\n" \
			PF_COPYRIGHT	"\n" \
			PF_REVISION	"\n" \
			PF_DEVICE	"\n" \
			PF_CONTACT	"\n"
#define PF_SPLASH	PF_DEVICE	" - " \
			PF_REVISION

#ifdef CONFIG_STREAMS_PF_MODULE
MODULE_AUTHOR(PF_CONTACT);
MODULE_DESCRIPTION(PF_DESCRIP);
MODULE_SUPPORTED_DEVICE(PF_DEVICE);
MODULE_LICENSE(PF_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_PF_MODULE */

#ifndef CONFIG_STREAMS_PF_NAME
//#define CONFIG_STREAMS_PF_NAME "pf"
#error "CONFIG_STREAMS_PF_NAME must be defined."
#endif				/* CONFIG_STREAMS_PF_NAME */
#ifndef CONFIG_STREAMS_PF_MODID
//#define CONFIG_STREAMS_PF_MODID 32
#error "CONFIG_STREAMS_PF_MODID must be defined."
#endif				/* CONFIG_STREAMS_PF_MODID */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-pf");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_PF_MODULE
static
#endif					/* CONFIG_STREAMS_PF_MODULE */
modID_t modid = CONFIG_STREAMS_PF_MODID;

#ifdef CONFIG_STREAMS_PF_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for PF.");
#endif				/* CONFIG_STREAMS_PF_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PF_MODID));
MODULE_ALIAS("streams-module-pf");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info pf_minfo = {
	.mi_idnum = CONFIG_STREAMS_PF_MODID,
	.mi_idname = CONFIG_STREAMS_PF_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat pf_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat pf_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  -------------------------------------------------------------------------
 *
 *  PRIVATE STRUCTURE
 *
 *  -------------------------------------------------------------------------
 */

struct pf {
	uchar pf_FilterLenTmp;
	uchar pf_Dummy;
	uchar pf_Priority;
	uchar pf_FilterLen;
	ushort pf_Filter[ENMAXFILTERS];
	ushort pf_Stack[ENMAXFILTERS];
	ushort pf_Warnings;
	ushort pf_Order;
};

#if	defined DEFINE_RWLOCK
static DEFINE_RWLOCK(pf_lock);
#elif	defined __RW_LOCK_UNLOCKED
static rwlock_t pf_lock = __RW_LOCK_UNLOCKED(pf_lock);
#elif	defined RW_LOCK_UNLOCKED
static rwlock_t pf_lock = RW_LOCK_UNLOCKED;
#else
#error cannot initialize read-write locks
#endif
static caddr_t pf_opens;

/*
 *  -------------------------------------------------------------------------
 *
 *  FILTER VALIDATION
 *
 *  -------------------------------------------------------------------------
 */

#define PF_CMD(x)   ((x)&((1<<ENF_NBPA)-1))
#define PF_OP(x)    ((x)&(((1<<ENF_NBPO)-1)<<ENF_NBPA))
#define PF_ARG(x)   (PF_CMD(x)-ENF_PUSHWORD)

STATIC noinline __unlikely fastcall int
pf_validate_filter(uint16_t *f, uint16_t len)
{
	uint i, s = 1;
	const uint16_t *p;

	if (len > ENMAXFILTERS)
		return (0);

	for (p = f, i = 0; i < len; i++) {
		p = f + i;
		switch (PF_CMD(*p)) {
		case ENF_NOPUSH:
			break;
		case ENF_PUSHLIT:
			if (++i >= ENMAXFILTERS || i >= len)
				return (0);
			/* fall through */
		case ENF_PUSHZERO:
		case ENF_PUSHONE:
		case ENF_PUSHFFFF:
		case ENF_PUSHFF00:
		default:
			if (PF_ARG(*p) > 255)
				return (0);
			/* fall through */
		case ENF_PUSHWORD:
			if (++s >= ENMAXFILTERS)
				return (0);
			break;
		}
		if (PF_OP(*p) == ENF_NOP)
			continue;
		if (--s <= 1 || --s <= 1)
			return (0);
		switch (PF_OP(*p)) {
		case ENF_NOP:
		case ENF_COR:
		case ENF_CAND:
		case ENF_CNAND:
			break;
		case ENF_EQ:
		case ENF_LT:
		case ENF_LE:
		case ENF_GT:
		case ENF_GE:
		case ENF_AND:
		case ENF_OR:
		case ENF_XOR:
			if (++s >= ENMAXFILTERS)
				return (0);
			break;
		default:
			return (0);
		}
	}
	return (1);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IOCTLS
 *
 *  -------------------------------------------------------------------------
 */

STATIC noinline __unlikely fastcall void
pf_ioctl(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case _IOC_TYPE(NIOC):
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(NIOCSETF):
			mi_copyin(q, mp, NULL, 2 * sizeof(uchar));
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
pf_iocdata(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp;

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case _IOC_TYPE(NIOC):
		switch (_IOC_NR(cp->cp_cmd)) {
		case _IOC_NR(NIOCSETF):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EPROTO);
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
				if (unlikely(dp->b_rptr[1] > ENMAXFILTERS)) {
					mi_copy_done(q, mp, ERANGE);
					break;
				}
				pf->pf_FilterLenTmp = dp->b_rptr[1];
				mi_copyin_n(q, mp, 2, 2 * pf->pf_FilterLenTmp);
				break;
			case MI_COPY_CASE(MI_COPY_IN, 2):
			{
				uint16_t *f;
				uchar len;

				len = pf->pf_FilterLenTmp;
				if ((f = kmem_alloc(2 * len, KM_NOSLEEP)) == NULL) {
					mi_copy_done(q, mp, ENOMEM);
					break;
				}
				bcopy(dp->b_rptr, f, len * 2);
				if (!pf_validate_filter(f, len)) {
					kmem_free(f, 2 * len);
					mi_copy_done(q, mp, EINVAL);
					break;
				}
				{
					uint16_t *f2;
					uchar len2;

					if ((len2 = pf->pf_FilterLen) != 0)
						pf->pf_FilterLen = 0;
					if ((f2 = pf->pf_Filter) != NULL) {
						pf->pf_FilterLen = 0;
						// pf->pf_Filter = NULL;
						kmem_free(f2, 2 * len2);
					}
				}
				// pf->pf_Filter = f;
				pf->pf_FilterLen = len;
				mi_copy_done(q, mp, 0);
				break;
			}
			}
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

/*
 *  -------------------------------------------------------------------------
 *
 *  FILTER routines
 *
 *  -------------------------------------------------------------------------
 */

/** pf_pack: - obtain a short word at offset from the packet
  * @mp: the packet (buffer chain)
  * @offs: the short-word offset into the packet
  * @reg: a pointer to the short-word register to fill
  *
  * This function obtains one or two bytes form the packet at the short-word
  * offset specified by the offs parameter.
  */
static inline fastcall int
pf_pack(const mblk_t *const mp, const uint16_t offs, uint16_t *reg)
{
	register const mblk_t *b = mp;
	register uint16_t result = 0;
	register int i = 0;
	register unsigned char *p;
	register uint32_t olen = ((uint32_t)offs)<<1;

	do {
		if (unlikely(b->b_wptr <= b->b_rptr))
			continue;
		for (p = b->b_rptr + olen + i; i < 2 && likely(p < b->b_wptr); p++, i++) {
			result <<= 8;
			result |= *p;
		}
		if (likely(i == 2)) {
			*reg = result;
			return (1);
		}
		olen -= b->b_wptr - b->b_rptr;
	}
	while (unlikely((b = b->b_cont) != NULL));
	if (likely(i != 0)) {
		result <<= 8;
		*reg = result;
		return (1);
	}
	return (0);
}

#if 0
STATIC noinline __hot_get fastcall int
pf_filter(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	register ushort *s = pf->pf_Stack;
	const register ushort *p = pf->pf_Filter;
	const ushort *const ss = s;
	const ushort *const se = s + ENMAXFILTERS;
	const ushort *const ps = p;
	const ushort *const pe = p + ENMAXFILTERS;

	prefetchw(s);
	prefetch(p);

	*s = 1; s++;

	while (p < pe) {
		uint16_t opcode = *p, v1, v2;

		/* *INDENT-OFF* */
		switch (PF_CMD(opcode)) {
		case ENF_NOPUSH: break;
		case ENF_PUSHLIT:	s++; *s = *p; p++; break;
		case ENF_PUSHZERO:	s++; *s = 0x0000; break;
		case ENF_PUSHONE:	s++; *s = 0x0001; break;
		case ENF_PUSHFFFF:	s++; *s = 0xffff; break;
		case ENF_PUSHFF00:	s++; *s = 0xff00; break;
		case ENF_PUSH00FF:	s++; *s = 0x00ff; break;
		default:		if (PF_ARG(opcode) > 255) return (0);
		case ENF_PUSHWORD:	s++; if (!pf_pack(mp, PF_ARG(opcode), s)) return (0); break;
		}
		if (PF_OP(opcode) == ENF_NOP) continue;
		v2 = *s; s--; v1 = *s; if (s < &s[1]) return (0);
		switch (PF_OP(opcode)) {
		case ENF_NOP:	break;
		case ENF_EQ:	*s = (v1 == v2); break;
		case ENF_LT:	*s = (v1 < v2); break;
		case ENF_LE:	*s = (v1 <= v2); break;
		case ENF_GT:	*s = (v1 > v2); break;
		case ENF_GE:	*s = (v1 >= v2); break;
		case ENF_AND:	*s = (v1 & v2); break;
		case ENF_OR:	*s = (v1 | v2); break;
		case ENF_XOR:	*s = (v1 ^ v2); break;
		case ENF_COR:	if (v1 == v2) return (1); s--; if (s < ss) return (0); break;
		case ENF_CAND:	if (v1 != v2) return (0); s--; if (s < ss) return (0); break;
		case ENF_CNOR:	if (v1 == v2) return (0); s--; if (s < ss) return (0); break;
		case ENF_CNAND:	if (v1 != v2) return (1); s--; if (s < ss) return (0); break;
		case ENF_NEQ:	*s = (v1 != v2); break;
		default: return (0);
		}
		/* *INDENT-ON* */
	}
	return (*s);
}
#endif

#define ENF_LITERALS	(0|(1<<ENF_PUSHLIT))
#define ENF_POPPERS	(0)
#define ENF_CONSUMERS	(0|(1<<(ENF_COR>>ENF_NBPA))|(1<<(ENF_CNOR>>ENF_NBPA))|(1<<(ENF_CAND>>ENF_NBPA))|(1<<(ENF_CNAND>>ENF_NBPA)))

STATIC noinline __hot_get fastcall int
pf_filter(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	ushort *s = pf->pf_Stack;
	const ushort *p = pf->pf_Filter;
	ushort si, pi, len = pf->pf_FilterLen;
	ushort cmd, arg, op;

	for (pi = 0, s[0] = 1, si = 1; pi < len; pi++) {
		ushort o = p[pi];

		if (likely((cmd = PF_CMD(o)) != ENF_NOPUSH)) {
			if (cmd == ENF_PUSHLIT && unlikely(++pi >= len))
				goto eof;
			if (unlikely(++si > len))
				goto overflow;
			if (cmd > ENF_PUSH00FF) {
				if (!pf_pack(mp, (arg = cmd - ENF_PUSHWORD), &s[si]))
					goto offset;
			} else {
				switch (cmd) {
				case ENF_PUSHLIT:
					s[si] = p[pi];
					break;
				case ENF_PUSHZERO:
					s[si] = 0x0000;
					break;
				case ENF_PUSHONE:
					s[si] = 0x0001;
					break;
				case ENF_PUSHFFFF:
					s[si] = 0xffff;
					break;
				case ENF_PUSHFF00:
					s[si] = 0xff00;
					break;
				case ENF_PUSH00FF:
					s[si] = 0x00ff;
					break;
				default:
					goto command;
				}
			}
		}
		if (likely((op = PF_OP(o)>>ENF_NBPA) != (ENF_NOP >> ENF_NBPA))) {
			ushort v1 = s[si - 1];
			ushort v2 = s[si];

			if (unlikely(si < 2))
				goto underrun;
			--si;

			if ((1 << op) & ENF_CONSUMERS) {
				--si;
				switch (op) {
				case (ENF_COR >> ENF_NBPA):
					if (v1 == v2)
						goto accept;
					break;
				case (ENF_CAND >> ENF_NBPA):
					if (v1 != v2)
						goto reject;
					break;
				case (ENF_CNOR >> ENF_NBPA):
					if (v1 != v2)
						goto reject;
					break;
				case (ENF_CNAND >> ENF_NBPA):
					if (v1 == v2)
						goto accept;
					break;
				}
			} else {
				switch (op) {
				case (ENF_EQ >> ENF_NBPA):
					s[si] = (v1 == v2);
					break;
				case (ENF_NEQ >> ENF_NBPA):
					s[si] = (v1 != v2);
					break;
				case (ENF_LT >> ENF_NBPA):
					s[si] = (v1 < v2);
					break;
				case (ENF_LE >> ENF_NBPA):
					s[si] = (v1 <= v2);
					break;
				case (ENF_GT >> ENF_NBPA):
					s[si] = (v1 > v2);
					break;
				case (ENF_GE >> ENF_NBPA):
					s[si] = (v1 >= v2);
					break;
				case (ENF_AND >> ENF_NBPA):
					s[si] = (v1 & v2);
					break;
				case (ENF_OR >> ENF_NBPA):
					s[si] = (v1 | v2);
					break;
				case (ENF_XOR >> ENF_NBPA):
					s[si] = (v1 ^ v2);
					break;
				default:
					goto operation;
				}
			}
		}
	}
	return (s[si]);
      eof:
	mi_strlog(q, 0, SL_WARN, "Premature end of program at instruction %hu", (pi + 1));
	goto reject;
      overflow:
	mi_strlog(q, 0, SL_WARN, "Stack overflow at instruction %hu", (pi + 1));
	goto reject;
      underrun:
	mi_strlog(q, 0, SL_WARN, "Stack underrun at instruction %hu", (pi + 1));
	goto reject;
      offset:
	mi_strlog(q, 0, SL_WARN, "Offset %hu is outside packet length of %zu", arg, msgsize(mp));
	goto reject;
      command:
	mi_strlog(q, 0, SL_WARN, "Illegal command %hu and instruction %hu", cmd, (pi + 1));
	goto reject;
      operation:
	mi_strlog(q, 0, SL_WARN, "Illegal operation %hu and instruction %hu", op, (pi + 1));
	goto reject;
      accept:
	return (1);
      reject:
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

STATIC streamscall __hot_put int
pf_wput(queue_t *q, mblk_t *mp)
{
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
		pf_ioctl(q, mp);
		break;
	case M_IOCDATA:
		pf_iocdata(q, mp);
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

STATIC streamscall __hot_out int
pf_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		unsigned char db_type = mp->b_datap->db_type;

		switch (__builtin_expect(db_type, M_PROTO)) {
		case M_IOCTL:
			pf_ioctl(q, mp);
			continue;
		case M_IOCDATA:
			pf_iocdata(q, mp);
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

STATIC streamscall __hot_get int
pf_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		unsigned char db_type = mp->b_datap->db_type;
		mblk_t *b = mp;

		switch (__builtin_expect(db_type, M_PROTO)) {
		case M_PROTO:
			for (; b && b->b_datap->db_type != M_DATA; b = b->b_cont) ;
			/* find the M_DATA */
			if (b == NULL)
				break;
		case M_DATA:
			/* I hate to say it, but Solaris pulls up the wrong message (as though
			   message pullup is necessary anyway). */
			if (!pf_filter(q, mp)) {
				freemsg(mp);
				continue;
			}
			break;
		default:
			break;
		}

		if (likely(db_type >= QPCTL || bcanputnext(q, mp->b_band))) {
			putnext(q, mp);
			continue;
		}
		putbq(q, mp);
		break;
	}
	return (0);
}

STATIC streamscall __hot_in int
pf_rput(queue_t *q, mblk_t *mp)
{
	unsigned char db_type = mp->b_datap->db_type;

	switch (__builtin_expect(db_type, M_PROTO)) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		break;
	default:
		if (db_type >= QPCTL) {
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

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
pf_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;
	struct pf *pf;
	ushort *sp, *fp;
	int order, err;

	if (q->q_ptr != NULL)
		return (0);
	if (sflag != MODOPEN)
		return (ENXIO);
	{
		int pages = (((ENMAXFILTERS + 1) << 2) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

		for (order = 0; (PAGE_SIZE << order) < pages; order++) ;
	}
	if ((sp = (ushort *) __get_free_pages(GFP_KERNEL, order)) == NULL)
		return (ENOMEM);
	fp = sp + ((PAGE_SIZE << order) >> 1);

	write_lock(&pf_lock);
	if ((err = mi_open_comm(&pf_opens, sizeof(*pf), q, devp, oflag, sflag, crp))) {
		write_unlock(&pf_lock);
		free_pages((ulong)sp, order);
		return (err);
	}
	wq = WR(q);
	wq->q_minpsz = wq->q_next->q_minpsz;
	wq->q_maxpsz = wq->q_next->q_maxpsz;

	pf = (typeof(pf)) q->q_ptr;
	bzero(pf, sizeof(pf));
	// pf->pf_Stack = sp;
	// pf->pf_Filter = fp;
	pf->pf_Warnings = 0;
	pf->pf_Order = order;

	write_unlock(&pf_lock);
	qprocson(q);
	return (0);
}

STATIC streamscall int
pf_close(queue_t *q, int oflag, cred_t *crp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;

	(void) oflag;
	(void) crp;
	qprocsoff(q);
	free_pages((ulong) pf->pf_Stack, pf->pf_Order);
	write_lock(&pf_lock);
	mi_close_comm(&pf_opens, q);
	write_unlock(&pf_lock);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
STATIC struct qinit pf_rinit = {
	.qi_putp = pf_rput,
	.qi_srvp = pf_rsrv,
	.qi_qopen = pf_open,
	.qi_qclose = pf_close,
	.qi_minfo = &pf_minfo,
	.qi_mstat = &pf_rstat,
};

STATIC struct qinit pf_winit = {
	.qi_putp = pf_wput,
	.qi_srvp = pf_wsrv,
	.qi_minfo = &pf_minfo,
	.qi_mstat = &pf_wstat,
};

#ifdef CONFIG_STREAMS_PF_MODULE
STATIC
#endif
struct streamtab pfinfo = {
	.st_rdinit = &pf_rinit,
	.st_wrinit = &pf_winit,
};

STATIC struct fmodsw pf_fmod = {
	.f_name = CONFIG_STREAMS_PF_NAME,
	.f_str = &pfinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_PF_MODULE
STATIC
#endif
int __init
pfinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_PF_MODULE
	printk(KERN_INFO PF_BANNER);
#else
	printk(KERN_INFO PF_SPLASH);
#endif
	pf_minfo.mi_idnum = modid;
	if ((err = register_strmod(&pf_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_PF_MODULE
STATIC
#endif
void __exit
pfexit(void)
{
	int err;

	if ((err = unregister_strmod(&pf_fmod)) < 0)
		return (void) (err);
	return (void) (0);
}

#ifdef CONFIG_STREAMS_PF_MODULE
module_init(pfinit);
module_exit(pfexit);
#endif
