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
 *  This is PFMOD a classical STREAMS packet filtering module that performs the actions described in
 *  pfmod(4).
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
#define _SUN_SOURCE	1

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/strlog.h>
#include <sys/cmn_err.h>

#include <sys/pfmod.h>
#include <net/bpf.h>

#include "sys/config.h"

#define PFMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PFMOD_COPYRIGHT		"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define PFMOD_REVISION		"Lfs $RCSfile$ $Name$($Revision$) $Date$"
#define PFMOD_DEVICE		"SVR 4.2 Packet Filter Module (PFMOD) for STREAMS"
#define PFMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define PFMOD_LICENSE		"GPL"
#define PFMOD_BANNER		PFMOD_DESCRIP		"\n" \
				PFMOD_COPYRIGHT		"\n" \
				PFMOD_REVISION		"\n" \
				PFMOD_DEVICE		"\n" \
				PFMOD_CONTACT		"\n"
#define PFMOD_SPLASH		PFMOD_DEVICE		" - " \
				PFMOD_REVISION

#ifdef CONFIG_STREAMS_PFMOD_MODULE
MODULE_AUTHOR(PFMOD_CONTACT);
MODULE_DESCRIPTION(PFMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(PFMOD_DEVICE);
MODULE_LICENSE(PFMOD_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_PFMOD_MODULE */

#ifndef CONFIG_STREAMS_PFMOD_NAME
//#define CONFIG_STREAMS_PFMOD_NAME "pfmod"
#error "CONFIG_STREAMS_PFMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_PFMOD_MODID
//#define CONFIG_STREAMS_PFMOD_MODID 15
#error "CONFIG_STREAMS_PFMOD_MODID must be defined."
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-pfmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#ifndef CONFIG_STREAMS_PFMOD_MODULE
static
#endif
modID_t modid = CONFIG_STREAMS_PFMOD_MODID;

#ifdef CONFIG_STREAMS_PFMOD_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for PFMOD.");
#endif				/* CONFIG_STREAMS_PFMOD_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PFMOD_MODID));
MODULE_ALIAS("streams-module-pfmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info pfmod_minfo = {
	.mi_idnum = CONFIG_STREAMS_PFMOD_MODID,
	.mi_idname = CONFIG_STREAMS_PFMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat pfmod_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat pfmod_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  Private Structure
 *
 *  -------------------------------------------------------------------------
 */

struct pf {
	struct Pf_ext_packetfilt pf_filt;	/* filter code */
#define pf_Priority	pf_filt.Pf_Priority
#define pf_FilterLen	pf_filt.Pf_FilterLen
#define pf_Filter	pf_filt.Pf_Filter
	ushort *pf_Stack;		/* working stack */
	ushort pf_Warnings;
	ushort pf_Order;
	struct bpf_program pf_bpf_program;
	uint *pf_scratch_memory;
};

struct packdesc {
	ushort *pd_hdr;			/* header starting address */
	uint pd_hdrlen;		/* header length in shorts */
	ushort *pd_body;		/* body starting address */
	uint pd_bodylen;		/* body length in shorts */
};

#ifdef RW_LOCK_UNLOCKED
static rwlock_t pf_lock = RW_LOCK_UNLOCKED;
#elif defined __RW_LOCK_UNLOCKED
static rwlock_t pf_lock = __RW_LOCK_UNLOCKED(&pf_lock);
#else
#error cannot initialize read-write locks
#endif
static caddr_t pf_opens;

/*
 *  -------------------------------------------------------------------------
 *
 *  FILTER VALIDATION FUNCTIONS
 *
 *  -------------------------------------------------------------------------
 */

#define PF_CMD(x) ((x)&((1<<ENF_NBPA)-1))
#define PF_OP(x)  ((x)&(((1<<ENF_NBPO)-1)<<ENF_NBPA))
#define PF_ARG(x) (PF_CMD(x)-ENF_PUSHWORD)

STATIC noinline __unlikely fastcall int
pfmod_validate_filter(uint16_t *f, uint16_t len)
{
	uint i, s = 1;
	const uint16_t *p;

	if (len > PF_MAXFILTERS)
		return (0);

	for (p = f, i = 0; i < len; i++) {
		p = f + i;
		switch (PF_CMD(*p)) {
		case ENF_NOPUSH:
			break;
		case ENF_PUSHLIT:
		case ENF_LOAD_OFFSET:
			if (++i >= PF_MAXFILTERS || i >= len)
				return (0);
			/* fall through */
		case ENF_PUSHZERO:
		case ENF_PUSHONE:
		case ENF_PUSHFFFF:
		case ENF_PUSHFF00:
		case ENF_PUSH00FF:
		case ENF_PUSHLEN:
		case ENF_PUSHOFFSET:
		case ENF_DUP:
		default:
		case ENF_PUSHWORD:
			if (++s >= PF_MAXFILTERS)
				return (0);
			break;
		case ENF_BRTR:
		case ENF_BRFL:
			if (i + *p >= PF_MAXFILTERS || i + *p > len)
				return (0);
			break;
		case ENF_POPOFFSET:
		case ENF_POP:
			if (--s <= 1)
				return (0);
			break;
		case 15:	/* spare */
			return (0);
		}
		if (PF_OP(*p) == ENF_NOP)
			continue;
		if (--s <= 1 || --s <= 1)
			return (0);
		switch (PF_OP(*p)) {
		case ENF_NOP:
		case ENF_COR:
		case ENF_CAND:
		case ENF_CNOR:
		case ENF_CNAND:
			break;
		case ENF_SWAP:
			if (++s >= PF_MAXFILTERS)
				return (0);
			/* fall through */
		case ENF_EQ:
		case ENF_LT:
		case ENF_LE:
		case ENF_GT:
		case ENF_GE:
		case ENF_AND:
		case ENF_OR:
		case ENF_XOR:
		case ENF_SL:
		case ENF_SR:
		case ENF_ADD:
		case ENF_SUB:
		case ENF_MUL:
		case ENF_DIV:
		case ENF_REM:
			if (++s >= PF_MAXFILTERS)
				return (0);
			break;
		default:
			return (0);
		}
	}
	return (1);
}

/* 
 * returns 0 when no good, returns true when good.
 */
STATIC noinline __unlikely fastcall int
pfmod_validate_program(const struct bpf_insn *f, uint len)
{
	uint i, from;
	const struct bpf_insn *p;

	if (len < 1 || len > BPF_MAXINSNS)
		return (0);

	for (p = f, i = 0; i < len; i++, p++) {
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
 *  INPUT-OUTPUT CONTROLS
 *
 *  -------------------------------------------------------------------------
 */

STATIC noinline __unlikely fastcall void
pfmod_ioctl(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *db;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case _IOC_TYPE(PFIOC):
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(PFIOCSETF):
			switch (ioc->ioc_count) {
			case sizeof(struct Pf_ext_packetfilt):
				mi_copyin(q, mp, NULL, sizeof(struct Pf_ext_packetfilt));
				break;
			case sizeof(struct packetfilt):
			case TRANSPARENT:
				mi_copyin(q, mp, NULL, sizeof(struct packetfilt));
				break;
			}
			break;
		default:
			putnext(q, mp);
			break;
		}
		break;
	case _IOC_TYPE(BIOC):
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(BIOCFLUSH):
			/* just change this into an M_FLUSH */
			mp->b_datap->db_type = M_FLUSH;
			mp->b_band = 0;
			mp->b_flag = 0;
			mp->b_wptr = mp->b_rptr + 2;
			mp->b_rptr[0] = FLUSHW | FLUSHR;
			mp->b_rptr[1] = 0;
			qreply(q, mp);
			break;
		case _IOC_NR(BIOCSETF):
			mi_copyin(q, mp, NULL, sizeof(struct bpf_program));
			break;
		case _IOC_NR(BIOCVERSION):
		{
			struct bpf_version *bv;

			db = mi_copyout_alloc(q, mp, NULL, sizeof(*bv), 1);
			if (likely(db != NULL)) {
				bv = (typeof(bv)) db->b_rptr;
				bv->bv_major = BPF_MAJOR_VERSION;
				bv->bv_minor = BPF_MINOR_VERSION;
				mi_copyout(q, mp);
			}
			break;
		}
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
pfmod_iocdata(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp;

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case _IOC_TYPE(PFIOC):
		switch (_IOC_NR(cp->cp_cmd)) {
		case _IOC_NR(PFIOCSETF):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EPROTO);
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
				switch (dp->b_wptr - dp->b_rptr) {
				case sizeof(struct Pf_ext_packetfilt):
				{
					struct Pf_ext_packetfilt *f = (typeof(f)) dp->b_rptr;

					if (f->Pf_FilterLen > PF_MAXFILTERS) {
						mi_copy_done(q, mp, EINVAL);
						break;
					}
					bcopy(f, pf, sizeof(*f));
					if (!pfmod_validate_filter(pf->pf_Filter, pf->pf_FilterLen)) {
						mi_copy_done(q, mp, EINVAL);
						break;
					}
					mi_copy_done(q, mp, 0);
					break;
				}
				case sizeof(struct packetfilt):
				{
					struct packetfilt *f = (typeof(f)) dp->b_rptr;

					if (f->Pf_FilterLen - 1 >= ENMAXFILTERS) {
						mi_copy_done(q, mp, EINVAL);
						break;
					}
					pf->pf_Priority = f->Pf_Priority;
					pf->pf_FilterLen = (unsigned int) f->Pf_FilterLen;
					bcopy(f->Pf_Filter, pf->pf_Filter, sizeof(f->Pf_Filter));
					if (!pfmod_validate_filter(pf->pf_Filter, pf->pf_FilterLen)) {
						mi_copy_done(q, mp, EINVAL);
						break;
					}
					mi_copy_done(q, mp, 0);
					break;

				}
				default:
					mi_copy_done(q, mp, EINVAL);
					break;
				}
				break;
			}
			break;
		default:
			putnext(q, mp);
			break;
		}
		break;
	case _IOC_TYPE(BIOC):
		switch (_IOC_NR(cp->cp_cmd)) {
		case _IOC_NR(BIOCSETF):
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			default:
				mi_copy_done(q, mp, EPROTO);
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
			{
				struct bpf_program *bf;
				static const size_t insz = sizeof(struct bpf_insn);

				bf = (typeof(bf)) dp->b_rptr;
				if (bf->bf_len > 2048) {
					mi_copy_done(q, mp, ERANGE);
					break;
				}
				if (pf->pf_bpf_program.bf_insns != NULL) {
					pf->pf_bpf_program.bf_insns = 0;
					kmem_free(pf->pf_bpf_program.bf_insns,
						  pf->pf_bpf_program.bf_len * insz);
				}
				pf->pf_bpf_program.bf_insns =
				    kmem_alloc(bf->bf_len * insz, KM_NOSLEEP);
				if (pf->pf_bpf_program.bf_insns != NULL) {
					mi_copy_done(q, mp, ENOMEM);
					break;
				}
				/* discard everything until actual program comes */
				pf->pf_bpf_program.bf_insns[0] = BPF_STMT(BPF_RET + BPF_K, 0);
				pf->pf_bpf_program.bf_len = bf->bf_len;
				mi_copyin(q, mp, (caddr_t) bf->bf_insns, bf->bf_len * insz);
				break;
			}
			case MI_COPY_CASE(MI_COPY_IN, 2):
			{
				uchar *p = (typeof(p)) pf->pf_bpf_program.bf_insns;
				uint bf_len = pf->pf_bpf_program.bf_len;
				static const size_t insz = sizeof(struct bpf_insn);

				pf->pf_bpf_program.bf_len = 0;
				for (; dp != NULL; dp = dp->b_cont) {
					ssize_t blen = dp->b_wptr - dp->b_rptr;

					if (blen > 0) {
						bcopy(dp->b_rptr, p, blen);
						p += blen;
					}
				}
				if (pfmod_validate_program(pf->pf_bpf_program.bf_insns, bf_len) != 0) {
					mi_copy_done(q, mp, EINVAL);
					break;
				}
				if (!pfmod_validate_program(pf->pf_bpf_program.bf_insns, bf_len)) {
					kmem_free(pf->pf_bpf_program.bf_insns, bf_len * insz);
					pf->pf_bpf_program.bf_insns = NULL;
					pf->pf_bpf_program.bf_len = 0;
					mi_copy_done(q, mp, EINVAL);
					break;
				}
				pf->pf_bpf_program.bf_len = bf_len;
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
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Filter Routine
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
	register uint32_t olen = ((uint32_t) offs) << 1;

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
pfmod_filter(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	register ushort *s = pf->pf_Stack;		/* stack pointer */
	register const ushort *p = pf->pf_Filter;	/* instruction pointer */
	const ushort *const ss = s;	/* start of program */
	const ushort *const se = ss + PF_MAXFILTERS;	/* end of program */
	const ushort *const ps = p;	/* start of stack */
	const ushort *const pe = ps + PF_MAXFILTERS;	/* end of program */
	uint z = 0, l;

	prefetchw(s);
	prefetch(p);

	*s = 1;
	s++;

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
		case ENF_LOAD_OFFSET:	z = *p; p++; break;
		case ENF_BRTR:		p += (*s != 0) ? *p : 1; if (p >= pe) return (0); break;
		case ENF_BRFL:		p += (*s == 0) ? *p : 1; if (p >= pe) return (0); break;
		case ENF_POP:		s--; if (unlikely(s < ss)) return (0); break;
		case ENF_PUSHLEN:	s++; if (unlikely(s >= se)) return (0); if (unlikely((l = msgsize(mp)) > 65535)) l = 65535; *s = l; break;
		case ENF_PUSHOFFSET:	s++; if (unlikely(s >= se)) return (0); *s = z; break;
		case ENF_POPOFFSET:	if (unlikely(s <= ss)) return (0); z = *s; s--; break;
		case ENF_DUP:		s++; if (unlikely(s >= se)) return (0); *s = *(s-1); break;
		default:
		case ENF_PUSHWORD:	s++; if (!pf_pack(mp, PF_ARG(opcode) + z, s)) return (0); break;
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
		case ENF_SL:	*s = (v1 << v2); break;
		case ENF_SR:	*s = (v1 >> v2); break;
		case ENF_ADD:	*s = (v1 + v2); break;
		case ENF_SUB:	*s = (v1 - v2); break;
		case ENF_MUL:	*s = (v1 * v2); break;
		case ENF_DIV:	*s = (v1 / v2); break;
		case ENF_REM:	*s = (v1 % v2); break;
		case ENF_SWAP:	*s = v1; s++; if (s >= se) return (0); *s = v2; break;
		default: return (0);
		}
		/* *INDENT-ON* */
	}
	return (*s);
}
#endif

#define ENF_LITERALS	(0|(1<<ENF_PUSHLIT)|(1<<ENF_LOAD_OFFSET)|(1<<ENF_BRTR)|(1<<ENF_BRFL))
#define ENF_POPPERS	(0|(1<<ENF_POP)|(1<<ENF_POPOFFSET))
#define ENF_CONSUMERS	(0|(1<<(ENF_COR>>ENF_NBPA))|(1<<(ENF_CNOR>>ENF_NBPA))|(1<<(ENF_CAND>>ENF_NBPA))|(1<<(ENF_CNAND>>ENF_NBPA)))

STATIC noinline __hot_get fastcall int
pfmod_filter(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	ushort *s = pf->pf_Stack;
	const ushort *p = pf->pf_Filter;
	ushort si, pi, len = pf->pf_FilterLen, z = 0;
	ushort cmd, arg, op;

	for (pi = 0, s[0] = 1, si = 1; pi < len; pi++) {
		ushort o = p[pi];

		if (likely((cmd = PF_CMD(o)) != ENF_NOPUSH)) {
			{
				ushort mask = 1 << cmd;

				if (mask & ENF_LITERALS)
					if (unlikely(++pi >= len))
						goto eof;
				if (mask & ENF_POPPERS) {
					if (unlikely(si == 0))
						goto underrun;
					--si;
				} else if (cmd != ENF_LOAD_OFFSET)
					/* other commands push a value on the stack */
					if (unlikely(++si > len))
						goto overflow;
			}
			if (cmd > ENF_DUP) {
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
				case ENF_LOAD_OFFSET:
					z = p[pi];
					break;
				case ENF_BRTR:
					if (s[si] != 0)
						pi += p[pi] - 1;
					break;
				case ENF_BRFL:
					if (s[si] == 0)
						pi += p[pi] - 1;
					break;
				case ENF_POP:
					break;
				case ENF_PUSHLEN:
					s[si] = msgsize(mp);
					break;
				case ENF_PUSHOFFSET:
					s[si] = z;
					break;
				case ENF_POPOFFSET:
					z = s[si + 1];
					break;
				case ENF_DUP:
					s[si] = s[si - 1];
					break;
				default:
					goto command;
				}
			}
		}
		if (likely((op = PF_OP(o) >> ENF_NBPA) != (ENF_NOP >> ENF_NBPA))) {
			ushort v1 = s[si - 1];
			ushort v2 = s[si];

			if (unlikely(si < 2))
				goto underrun;
			if (op != ENF_SWAP)
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
				case (ENF_SL >> ENF_NBPA):
					s[si] = (v1 << v2);
					break;
				case (ENF_SR >> ENF_NBPA):
					s[si] = (v1 >> v2);
					break;
				case (ENF_ADD >> ENF_NBPA):
					s[si] = (v1 + v2);
					break;
				case (ENF_SUB >> ENF_NBPA):
					s[si] = (v1 - v2);
					break;
				case (ENF_MUL >> ENF_NBPA):
					s[si] = (v1 * v2);
					break;
				case (ENF_DIV >> ENF_NBPA):
					s[si] = (v1 / v2);
					break;
				case (ENF_REM >> ENF_NBPA):
					s[si] = (v1 % v2);
					break;
				case (ENF_SWAP >> ENF_NBPA):
					s[si] = v1;
					s[si - 1] = v2;
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
	mi_strlog(q, 0, SL_WARN, "Offset %hu is outside packet length of %zu", z + arg,
		  msgsize(mp));
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

static inline fastcall int
bpf_x(const mblk_t *const mp, const uint32_t offs, const uchar size, uint32_t *const reg)
{
	register const mblk_t *b = mp;
	register uint32_t result = 0;
	register int i = 0;
	register unsigned char *p;
	register uint32_t olen = offs;

	do {
		if (unlikely(b->b_wptr <= b->b_rptr))
			continue;
		for (p = b->b_rptr + olen + i; i < size && likely(p < b->b_wptr); p++, i++) {
			result <<= 8;
			result |= *p;
		}
		if (likely(i == size)) {
			*reg = result;
			return (1);
		}
		olen -= b->b_wptr - b->b_rptr;
	}
	while (unlikely((b = b->b_cont) != NULL));
	return (0);
}

static inline fastcall int
bpf_w(const mblk_t *const mp, const uint32_t offs, uint32_t *const reg)
{
	return bpf_x(mp, offs, 4, reg);
}

static inline fastcall int
bpf_h(const mblk_t *const mp, const uint32_t offs, uint32_t *const reg)
{
	return bpf_x(mp, offs, 2, reg);
}

static inline fastcall int
bpf_b(const mblk_t *const mp, const uint32_t offs, uint32_t *const reg)
{
	return bpf_x(mp, offs, 1, reg);
}

static inline fastcall int
bpf_m(const uint32_t *const ms, const uint32_t indx, uint32_t *const reg)
{
	if (likely(0 >= indx && indx < BPF_MEMWORDS)) {
		*reg = ms[indx];
		return (1);
	}
	return (0);
}

#if 0
STATIC noinline __hot_get fastcall int
pfmod_bpf(queue_t *q, mblk_t *mp)
{
	struct pf *pf = (typeof(pf)) q->q_ptr;
	struct bpf_program *bpf = &pf->pf_bpf_program;
	struct bpf_insn *pc = bpf->bf_insns;
	struct bpf_insn *pe = pc + bpf->bf_len;
	uint32_t a = 0, x = 0, *ms = pf->pf_scratch_memory;

	while (pc < pe) {
		struct bpf_insn o = *pc;

		pc++;
		switch (BPF_CLASS(o.code)) {
		case BPF_LD:
			switch (BPF_MODE(o.code)) {
			case BPF_IMM:
				a = o.k;
				continue;
			case BPF_ABS:
				switch (BPF_SIZE(o.code)) {
				case BPF_W:
					if (likely(bpf_w(mp, o.k, &a)))
						continue;
					break;
				case BPF_H:
					if (likely(bpf_h(mp, o.k, &a)))
						continue;
					break;
				case BPF_B:
					if (likely(bpf_b(mp, o.k, &a)))
						continue;
					break;
				}
				break;
			case BPF_IND:
				switch (BPF_SIZE(o.code)) {
				case BPF_W:
					if (likely(bpf_w(mp, x + o.k, &a)))
						continue;
					break;
				case BPF_H:
					if (likely(bpf_h(mp, x + o.k, &a)))
						continue;
					break;
				case BPF_B:
					if (likely(bpf_b(mp, x + o.k, &a)))
						continue;
					break;
				}
				break;
			case BPF_MEM:
				if (likely(bpf_m(ms, o.k, &a)))
					continue;
				break;
			case BPF_LEN:
				a = msgsize(mp);
				continue;
			}
			break;
		case BPF_LDX:
			switch (BPF_MODE(o.code)) {
			case BPF_IMM:
				switch (BPF_SIZE(o.code)) {
				case BPF_W:
					x = o.k;
					continue;
				}
				break;
			case BPF_MEM:
				switch (BPF_SIZE(o.code)) {
				case BPF_W:
					if (likely(bpf_m(ms, o.k, &x)))
						continue;
					break;
				}
				break;
			case BPF_LEN:
				switch (BPF_SIZE(o.code)) {
				case BPF_W:
					x = msgsize(mp);
					continue;
				}
				break;
			case BPF_MSH:
				switch (BPF_SIZE(o.code)) {
				case BPF_B:
					if (likely(bpf_b(mp, 1, &x))) {
						x <<= 2;
						x &= 0x0f;
						continue;
					}
					break;
				}
				break;
			}
			break;
		case BPF_ST:
			if (likely(bpf_m(ms, o.k, &a)))
				continue;
			break;
		case BPF_STX:
			if (likely(bpf_m(ms, o.k, &x)))
				continue;
			break;
		case BPF_ALU:
			switch (BPF_SRC(o.code)) {
			case BPF_K:
				switch (BPF_OP(o.code)) {
				case BPF_ADD:
					a += o.k;
					continue;
				case BPF_SUB:
					a -= o.k;
					continue;
				case BPF_MUL:
					a *= o.k;
					continue;
				case BPF_DIV:
					a /= o.k;
					continue;
				case BPF_OR:
					a |= o.k;
					continue;
				case BPF_AND:
					a &= o.k;
					continue;
				case BPF_LSH:
					a <<= o.k;
					continue;
				case BPF_RSH:
					a >>= o.k;
					continue;
				case BPF_NEG:
					a = -a;
					continue;
				}
				break;
			case BPF_X:
				switch (BPF_OP(o.code)) {
				case BPF_ADD:
					a += x;
					continue;
				case BPF_SUB:
					a -= x;
					continue;
				case BPF_MUL:
					a *= x;
					continue;
				case BPF_DIV:
					a /= x;
					continue;
				case BPF_OR:
					a |= x;
					continue;
				case BPF_AND:
					a &= x;
					continue;
				case BPF_LSH:
					a <<= x;
					continue;
				case BPF_RSH:
					a >>= x;
					continue;
				case BPF_NEG:
					a = -a;
					continue;
				}
				break;
			}
			break;
		case BPF_JMP:
			switch (BPF_OP(o.code)) {
			case BPF_JA:
				if (likely((pc += o.k) < pe))
					continue;
				break;
			case BPF_JEQ:
				if (likely((pc += (a == o.k) ? o.jt : o.jf) < pe))
					continue;
				break;
			case BPF_JGT:
				if (likely((pc += (a > o.k) ? o.jt : o.jf) < pe))
					continue;
				break;
			case BPF_JGE:
				if (likely((pc += (a >= o.k) ? o.jt : o.jf) < pe))
					continue;
				break;
			case BPF_JSET:
				if (likely((pc += (a & o.k) ? o.jt : o.jf) < pe))
					continue;
				break;
			}
			break;
		case BPF_RET:
			switch (BPF_SRC(o.code)) {
			case BPF_A:
				return (a);
			case BPF_K:
				return (o.k);
			}
			break;
		case BPF_MISC:
			switch (BPF_MISCOP(o.code)) {
			case BPF_TAX:
				x = a;
				continue;
			case BPF_TXA:
				a = x;
				continue;
			}
			break;
		}
		break;
	}
	return (-1);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

/** pfmod_wput: - write put procedure
  * @q: write queue
  * @mp: message to put
  *
  * This is a canonical pass-through write put procedure.  Process M_FLUSH and intercept appropriate
  * M_IOCTL/M_IOCDATA messages: pass through everything else with flow control.
  */
STATIC streamscall int
pfmod_wput(queue_t *q, mblk_t *mp)
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
		pfmod_ioctl(q, mp);
		break;
	case M_IOCDATA:
		pfmod_iocdata(q, mp);
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

/** pfmod_wsrv: - write service procedure.
  * @q: the write queue to service
  *
  * This is a canonical write service procedure.  It simply passes messages along under flow
  * control.  A check must be made for M_IOCTL and M_IOCDATA because the mi_copyin function
  * sometimes places one of these messages on the queue.
  */
STATIC streamscall int
pfmod_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		unsigned char db_type = mp->b_datap->db_type;

		switch (__builtin_expect(db_type, M_PROTO)) {
		case M_IOCTL:
			pfmod_ioctl(q, mp);
			continue;
		case M_IOCDATA:
			pfmod_iocdata(q, mp);
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

STATIC streamscall int
pfmod_rsrv(queue_t *q)
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
			if (!pfmod_filter(q, b)) {
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

/** pfmod_rput: - write put procedure
  * @q: write queue
  * @mp: message to put
  *
  * This ia a canonical service-procedure read put procedure.  Process M_FLUSH messages, bypass
  * priority control messages and queue everything else.  Even for a single card driver we can
  * process up to 124 messages per interrupt cycle, so we want to queue all of these and process
  * them out of the service procedure for efficiency.  Most of them are duped data blocks anyway.
  */
STATIC streamscall int
pfmod_rput(queue_t *q, mblk_t *mp)
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

#define stack_size (sizeof(ushort)*(PF_MAXFILTERS+1))
/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
pfmod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
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
		int pages = (((PF_MAXFILTERS + 1) << 2) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

		for (order = 0; (PAGE_SIZE << order) < pages; order++) ;
		sp = (typeof(sp)) __get_free_pages(order, GFP_KERNEL);
	}
	if ((sp = (ushort *) __get_free_pages(GFP_KERNEL, order)) == NULL)
		return (ENOMEM);
	fp = sp + ((PAGE_SIZE << order) >> 1);

	write_lock(&pf_lock);
	if ((err = mi_open_comm(&pf_opens, sizeof(*pf), q, devp, oflag, sflag, crp))) {
		write_unlock(&pf_lock);
		free_pages((ulong) sp, order);
		return (err);
	}
	wq = WR(q);
	wq->q_minpsz = wq->q_next->q_minpsz;
	wq->q_maxpsz = wq->q_next->q_maxpsz;

	pf = (typeof(pf)) q->q_ptr;
	bzero(pf, sizeof(*pf));
	pf->pf_Stack = sp;
	//pf->pf_Filter = fp;
	pf->pf_Warnings = 0;
	pf->pf_Order = order;

	write_unlock(&pf_lock);
	qprocson(q);
	return (0);
}

STATIC streamscall int
pfmod_close(queue_t *q, int oflag, cred_t *crp)
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
STATIC struct qinit pfmod_rinit = {
	.qi_putp = pfmod_rput,
	.qi_srvp = pfmod_rsrv,
	.qi_qopen = pfmod_open,
	.qi_qclose = pfmod_close,
	.qi_minfo = &pfmod_minfo,
	.qi_mstat = &pfmod_rstat,
};

STATIC struct qinit pfmod_winit = {
	.qi_putp = pfmod_wput,
	.qi_srvp = pfmod_wsrv,
	.qi_minfo = &pfmod_minfo,
	.qi_mstat = &pfmod_wstat,
};

#ifdef CONFIG_STREAMS_PFMOD_MODULE
STATIC
#endif
struct streamtab pfmodinfo = {
	.st_rdinit = &pfmod_rinit,
	.st_wrinit = &pfmod_winit,
};

STATIC struct fmodsw pfmod_fmod = {
	.f_name = CONFIG_STREAMS_PFMOD_NAME,
	.f_str = &pfmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef __LP64__
struct pfmod_trans {
	unsigned int cmd;
	void *opaque;
};

STATIC struct pfmod_trans pfmod_trans_map[] = {
	{.cmd = PFIOCSETF,}
	, {.cmd = 0,}
};

STATIC __unlikely void
pfmod_ioctl32_unregister(void)
{
	struct pfmod_trans *t;

	for (t = pfmod_trans_map, t->cmd != 0; t++) {
		unregister_ioctl32(t->opaque);
		t->opaque = NULL;
	}
	return;
}

STATIC __unlikely int
pfmod_ioct32_register(void)
{
	struct pfmod_trans *t;

	for (t = pfmod_trans_map; t->cmd != 0; t++) {
		if ((t->opaque = register_ioctl32(t->cmd)) == NULL) {
			pfmod_ioctl32_unregister();
			return (-ENOMEM);
		}
	}
	return (0);
}
#endif				/* __LP64__ */

#ifdef CONFIG_STREAMS_PFMOD_MODULE
STATIC
#endif
int __init
pfmodinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_PFMOD_MODULE
	printk(KERN_INFO PFMOD_BANNER);
#else
	printk(KERN_INFO PFMOD_SPLASH);
#endif
	pfmod_minfo.mi_idnum = modid;
#ifdef __LP64__
	if ((err = pfmod_ioctl32_register())) {
		cmn_err(CE_WARN, "%s: could not register 32-bit ioctls, err = %d",
			CONFIG_STREAMS_PFMOD_NAME, err);
		return (err);
	}
#endif				/* __LP64__ */
	if ((err = register_strmod(&pfmod_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d",
			CONFIG_STREAMS_PFMOD_NAME, err);
#ifdef __LP64__
		pfmod_ioctl32_unregister();
#endif				/* __LP64__ */
		return (err);
	}
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_PFMOD_MODULE
STATIC
#endif
void __exit
pfmodexit(void)
{
	int err;

#ifdef __LP64__
	pfmod_ioctl32_unregister();
#endif				/* __LP64__ */
	if ((err = unregister_strmod(&pfmod_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d",
			CONFIG_STREAMS_PFMOD_NAME, err);
		return (void) (err);
	}
	return (void) (0);
}

#ifdef CONFIG_STREAMS_PFMOD_MODULE
module_init(pfmodinit);
module_exit(pfmodexit);
#endif
