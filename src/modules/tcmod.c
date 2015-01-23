/*****************************************************************************

 @(#) File: src/modules/tcmod.c

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

#define _DEBUG 1

#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1

#include <sys/os8/compat.h>
#include <sys/strsun.h>

#define TCMOD_DESCRIP	"ITU_T Q.771 TC for ITU-T Q.771 TR (TCMOD) STREAMS Module"
#define TCMOD_EXTRA	"Part of the OpenSS7 SS7 Stack for Linux Fast-STREAMS"
#define TCMOD_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define TCMOD_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define TCMOD_DEVICE	"ITU-T Q.771 TC modules for ITU-T Q.771 TR"
#define TCMOD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TCMOD_LICENSE	"GPL"
#define TCMOD_BANNER	TCMOD_DESCRIP	"\n" \
			TCMOD_EXTRA	"\n" \
			TCMOD_REVISION	"\n" \
			TCMOD_COPYRIGHT	"\n" \
			TCMOD_DEVICE	"\n" \
			TCMOD_CONTACT
#define TCMOD_SPLASH	TCMOD_DESCRIP	"\n" \
			TCMOD_REVISION

#define TCMOD_MOD_ID	CONFIG_STREAMS_TCMOD_MODID
#define TCMOD_MOD_NAME	CONFIG_STREAMS_TCMOD_NAME

#ifndef TCMOD_MOD_ID
#error "TCMOD_MOD_ID must be defined."
#endif
#ifndef TCMOD_MOD_NAME
#error "TCMOD_MOD_NAME must be defined."
#endif

modID_t modid = TCMOD_MOD_ID;

#ifdef _OPTIMIZE_SPEED
#define STRLOG(priv, level, flags, fmt, ...) \
	do { } while (0)
#else
#define STRLOG(priv, level, flags, fmt, ...) \
	mi_strlog(priv->rq, level, flags, fmt, ##__VA_ARGS__)
#endif

#define STRLOGERR	0	/* log error information */
#define STRLOGNO	0	/* log notice information */
#define STRLOGST	1	/* log state transitions */
#define STRLOGTO	2	/* log timeouts */
#define STRLOGRX	3	/* log primitives received */
#define STRLOGTX	4	/* log primitives issued */
#define STRLOGTE	5	/* log timer events */
#define STRLOGIO	6	/* log additional data */
#define STRLOGDA	7	/* log data */

#if 1
#define LOGERR(priv, fmt, ...) STRLOG(priv, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(priv, fmt, ...) STRLOG(priv, STRLOGNO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGST(priv, fmt, ...) STRLOG(priv, STRLOGST, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTO(priv, fmt, ...) STRLOG(priv, STRLOGTO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGRX(priv, fmt, ...) STRLOG(priv, STRLOGRX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTX(priv, fmt, ...) STRLOG(priv, STRLOGTX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTE(priv, fmt, ...) STRLOG(priv, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(priv, fmt, ...) STRLOG(priv, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(priv, fmt, ...) STRLOG(priv, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOGERR(priv, fmt, ...) STRLOG(priv, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(priv, fmt, ...) STRLOG(priv, STRLOGNO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGST(priv, fmt, ...) STRLOG(priv, STRLOGST, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTO(priv, fmt, ...) STRLOG(priv, STRLOGTO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGRX(priv, fmt, ...) STRLOG(priv, STRLOGRX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTX(priv, fmt, ...) STRLOG(priv, STRLOGTX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTE(priv, fmt, ...) STRLOG(priv, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(priv, fmt, ...) STRLOG(priv, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(priv, fmt, ...) STRLOG(priv, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#endif

/*
 * STREAMS DEFINITIONS
 */
#define MOD_ID		TCMOD_MOD_ID
#define MOD_NAME	TCMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	TCMOD_BANNER
#else				/* MODULE */
#define MOD_BANNER	TCMOD_SPLASH
#endif				/* MODULE */

static struct module_info tc_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tc_rstat
    __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tc_wstat
    __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

#if	defined DEFINE_RWLOCK
static DEFINE_RWLOCK(tc_lock);
#elif	defined __RW_LOCK_UNLOCKED
static rwlock_t tc_lock = __RW_LOCK_UNLOCKED(tc_lock);
#elif	defined RW_LOCK_UNLOCKED
static rwlock_t tc_lock = RW_LOCK_UNLOCKED;
#else
#error cannot initialize read-write locks
#endif

static caddr_t tc_opens = NULL;

/*
 * LOCKING
 */
static inline fastcall struct tc *
tc_trylock(queue_t *q)
{
	struct tc *tc;

	read_lock(&tc_lock);
	tc = (struct tc *) mi_trylock(q);
	read_unlock(&tc_lock);
	return (tc);
}

static inline fastcall void
tc_unlock(struct tc *tc)
{
	read_lock(&tc_lock);
	mi_unlock((caddr_t) tc);
	read_unlock(&tc_lock);
}

/*
 * CACHES
 */
static kmem_cachep_t tc_priv_cachep = NULL;

static int
tc_init_caches(void)
{
	if (!tc_priv_cachep
	    && !(tc_priv_cachep =
		 kmem_create_cache("tc_priv_cachep",
				   mi_open_size(sizeof(struct tc)), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC,
			"%s: could not allocat tc_priv_cachep",
			__FUNCTION__);
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG,
			"%s: initialized driver private structure cache",
			MOD_NAME);
	return (0);
}
static int
tc_term_caches(void)
{
	if (tc_priv_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tc_priv_cachep)) {
			cmn_err(CE_WARN,
				"%s: did not destroy tc_priv_cachep",
				__FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG,
				"%s: destroyed tc_priv_cachep",
				MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(tc_priv_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	return (0);
}

/*
 * TC-Provider -> TC-User primitives.
 */
static fastcall int
tc_info_ack(struct tc *tc, queue_t *q, mblk_t *msg)
{
	struct TC_info_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_INFO_ACK;
		p->TSDU_size = tc->info.TSDU_size;
		p->ETSDU_size = tc->info.ETSDU_size;
		p->CDATA_size = tc->info.CDATA_size;
		p->DDATA_size = tc->info.DDATA_size;
		p->ADDR_size = tc->info.ADDR_size;
		p->OPT_size = tc->info.OPT_size;
		p->TIDU_size = tc->info.TIDU_size;
		p->SERV_type = tc->info.SERV_type;
		p->CURRENT_state = tc->info.CURRENT_state;
		p->PROVIDER_flag = tc->info.PROVIDER_flag;
		p->TCI_version = tc->info.TCI_version;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(tc, "<- TC_INFO_ACK");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_bind_ack(struct tc *tc, queue_t *q, mblk_t *msg)
{
	struct TC_bind_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + tc->bind.ADDR_length;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_BIND_ACK;
		p->ADDR_length = tc->bind.ADDR_length;
		p->ADDR_offset = tc->bind.ADDR_offset;
		p->XACT_number = tc->bind.XACT_number;
		p->BIND_flags = tc->bind.BIND_flags;
		mp->b_wptr += sizeof(*p);
		bcopy((caddr_t) &tc->bind + p->ADDR_offset, mp->b_wptr,
		      p->ADDR_length);
		mp->b_wptr += p->ADDR_length;
		freemsg(msg);
		LOGTX(tc, "<- TC_BIND_ACK");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_subs_bind_ack(struct tc *tc, queue_t *q, mblk_t *msg)
{
}
static fastcall int
tc_ok_ack(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t prim)
{
	struct TC_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(tc, "<- TC_OK_ACK");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_error_ack(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t prim, t_scalar_t
	     error, t_uscalar_t dialog, t_uscalar_t invoke)
{
	struct TC_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TRPI_error = error < 0 ? TCSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		p->DIALOG_id = dialog;
		p->INVOKE_id = invoke;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(tc, "<- TC_ERROR_ACK");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_optmgmt_ack(struct tc *tc, queue_t *q, mblk_t *msg, caddr_t optr, size_t
	       olen, t_uscalar_t flags)
{
	struct TC_optmgmt_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_OPTMGMT_ACK;
		p->OPT_length = olen;
		p->OPT_offset = olen ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		LOGTX(tc, "<- TC_OPTMGMT_ACK");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_uni_ind(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	   caddr_t sptr, size_t slen, caddr_t dptr, size_t dlen,
	   caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TC_uni_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + dlen + slen + olen;

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_UNI_IND;
		p->SRC_length = slen;
		p->SRC_offset = sizeof(*p);
		p->DEST_length = dlen;
		p->DEST_offset = p->SRC_offset + slen;
		p->OPT_length = olen;
		p->OPT_offset = p->DEST_offset + dlen;
		p->DIALOG_id = dg->info.DIALOG_id;
		p->COMP_flags = dg->info.COMP_flags;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		/* no state change */
		LOGDA(tc, "<- TR_UNI_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_begin_ind(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	     caddr_t sptr, size_t slen, caddr_t dptr, size_t dlen,
	     caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TC_begin_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + dlen + slen + olen;

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_BEGIN_IND;
		p->SRC_length = slen;
		p->SRC_offset = sizeof(*p);
		p->DEST_length = dlen;
		p->DEST_offset = p->SRC_offset + slen;
		p->OPT_length = olen;
		p->OPT_offset = p->DEST_offset + dlen;
		p->DIALOG_id = dg->info.DIALOG_id;
		p->COMP_flags = dg->info.COMP_flags;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		dg_set_state(dg, TCS_WRES_CIND);
		LOGDA(tc, "<- TC_BEGIN_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_begin_con(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	     caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TC_begin_con *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_BEGIN_CON;
		p->OPT_length = olen;
		p->OPT_offset = sizeof(*p);
		p->DIALOG_id = dg->info.DIALOG_id;
		p->COMP_flags = dg->info.COMP_flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->bcont = NULL;
		freemsg(msg);
		dg_set_state(dg, TCS_DATA_XFER);
		LOGDA(tc, "<- TC_BEGIN_CON");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_cont_ind(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	    caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TC_cont_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_CONT_IND;
		p->OPT_length = olen;
		p->OPT_offset = sizeof(*p);
		p->DIALOG_id = dg->info.DIALOG_id;
		p->COMP_flags = dg->info.COMP_flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->bcont = NULL;
		freemsg(msg);
		/* no state change */
		LOGDA(tc, "<- TC_CONT_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_end_ind(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	   caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TC_end_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_END_IND;
		p->OPT_length = olen;
		p->OPT_offset = sizeof(*p);
		p->DIALOG_id = dg->info.DIALOG_id;
		p->COMP_flags = dg->info.COMP_flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->bcont = NULL;
		freemsg(msg);
		dg_set_state(dg, TCS_IDLE);
		LOGDA(tc, "<- TC_END_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_abort_ind(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	     t_uscalar_t reason, t_uscalar_t originator, caddr_t optr,
	     size_t olen, mblk_t *dp)
{
	struct TC_abort_ind *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_ABORT_IND;
		p->OPT_length = olen;
		p->OPT_offset = sizeof(*p);
		p->DIALOG_id = dg->info.DIALOG_id;
		p->COMP_flags = dg->info.COMP_flags;
		p->ABORT_reason = reason;
		p->ORIGINATOR = originator;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->bcont = NULL;
		freemsg(msg);
		dg_set_state(dg, TCS_IDLE);
		LOGDA(tc, "<- TC_ABORT_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_notice_ind(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	      t_uscalar_t cause, mblk_t *dp)
{
	struct TC_notice_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(tc->rq, 1)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 1;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_NOTICE_IND;
		p->DIALOG_id = dg->info.DIALOG_id;
		p->REPORT_cause = cause;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->bcont = NULL;
		freemsg(msg);
		/* no state change */
		LOGDA(tc, "<- TC_NOTICE_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_invoke_ind(struct tc *tc, struct dg *dg, struct iv *iv, queue_t *q,
	      mblk_t *msg, mblk_t *dp)
{
	struct TC_invoke_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_INVOKE_IND;
		p->DIALOG_id = dg->info.DIALOG_id;
		p->OP_class = iv->info.OP_class;
		p->INVOKE_id = iv->info.INVOKE_id;
		p->LINKED_id = iv->info.LINKED_id;
		p->OPERATION = iv->info.OPERATION;
		p->MORE_flag = iv->info.MORE_flag;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->bcont = NULL;
		freemsg(msg);
		/* no dialog state change */
		/* XXX invoke should change state to indicated awaiting (last) or result */
		LOGDA(tc, "<- TC_INVOKE_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_result_ind(struct tc *tc, struct dg *dg, struct iv *iv, queue_t *q,
	      mblk_t *msg, mblk_t *dp)
{
	struct TC_result_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_RESULT_IND;
		p->DIALOG_id = dg->info.DIALOG_id;
		p->INVOKE_id = iv->info.INVOKE_id;
		p->OPERATION = iv->info.OPERATION;
		p->MORE_flag = iv->info.MORE_flag;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		/* no dialog state change */
		/* XXX invoke should change state to result awaiting (last) or idle */
		LOGDA(tc, "<- TC_RESULT_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_error_ind(struct tc *tc, struct dg *dg, struct iv *iv, queue_t *q,
	     mblk_t *msg, mblk_t *dp)
{
	struct TC_error_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_ERROR_IND;
		p->DIALOG_id = dg->info.DIALOG_id;
		p->INVOKE_id = iv->info.INVOKE_id;
		p->ERROR_code = iv->info.ERROR_code;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		/* no dialog state change */
		/* XXX invoke should change state to idle */
		LOGDA(tc, "<- TC_ERROR_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_cancel_ind(struct tc *tc, struct dg *dg, struct iv *iv, queue_t *q,
	      mblk_t *msg)
{
	struct TC_cancel_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_CANCEL_IND;
		p->DIALOG_id = dg->info.DIALOG_id;
		p->INVOKE_id = iv->info.INVOKE_id;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		/* no dialog state change */
		/* XXX invoke should change state to idle */
		LOGDA(tc, "<- TC_CANCEL_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tc_reject_ind(struct tc *tc, struct dg *dg, struct iv *iv, queue_t *q,
	      mblk_t *msg, t_uscalar_t originator, t_uscalar_t cause,
	      mblk_t *dp)
{
	struct TC_reject_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!canputnext(tc->rq)))
		return (-EBUSY);
	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_REJECT_IND;
		p->DIALOG_id = dg->info.DIALOG_id;
		p->INVOKE_id = iv->info.INVOKE_id;
		p->ORIGINATOR = originator;
		p->PROBLEM_code = cause;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		/* no dialog state change */
		/* XXX invoke should change state to idle */
		LOGDA(tc, "<- TC_REJECT_IND");
		putnext(tc->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * TR-User -> TR-Provider primitives.
 */
static fastcall int
tr_info_req(struct tc *tc, queue_t *q, mblk_t *msg)
{
	struct TR_info_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(tc, "TR_INFO_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_bind_req(struct tc *tc, queue_t *q, mblk_t *msg,
	    t_uscalar_t xactinds, t_uscalar_t flags, caddr_t aptr,
	    size_t alen)
{
	struct TR_bind_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_BIND_REQ;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->XACT_number = xactinds;
		p->BIND_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tc, "TR_BIND_REQ ->");
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_unbind_req(struct tc *tc, queue_t *q, mblk_t *msg)
{
	struct TR_unbind_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(tc, "TR_UNBIND_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_optmgmt_req(struct tc *tc, queue_t *q, mblk_t *msg,
	       t_uscalar_t flags, caddr_t optr, size_t olen)
{
	struct TR_optmgmt_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_OPTMGMT_REQ;
		p->OPT_length = olen;
		p->OPT_offset = sizeof(*p);
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		LOGTX(tc, "TR_OPTMGMT_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_uni_req(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	   caddr_t dptr, size_t dlen, caddr_t sptr, size_t slen,
	   caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TR_uni_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + dlen + slen + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_UNI_REQ;
		p->DEST_length = dlen;
		p->DEST_offset = sizeof(*p);
		p->ORIG_length = slen;
		p->ORIG_offset = p->DEST_offset + p->DEST_length;
		p->OPT_length = olen;
		p->OPT_offset = p->ORIG_offset + p->ORIG_length;
		mp->b_wptr += sizeof(*p);
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		LOGDA(tc, "TR_UNI_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_begin_req(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	     caddr_t dptr, size_t dlen, caddr_t sptr, size_t slen,
	     caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TR_begin_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + dlen + slen + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_BEGIN_REQ;
		p->CORR_id = dg->info.DIALOG_id;
		p->ASSOC_flags = dg->info.ASSOC_flags;
		p->DEST_length = dlen;
		p->DEST_offset = sizeof(*p);
		p->ORIG_length = slen;
		p->ORIG_offset = p->DEST_offset + p->DEST_length;
		p->OPT_length = olen;
		p->OPT_offset = p->ORIG_offset + p->ORIG_length;
		mp->b_wptr += sizeof(*p);
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		tr_set_state(tc, TRS_WCON_CREQ);
		LOGDA(tc, "TR_BEGIN_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_begin_res(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	     caddr_t sptr, size_t slen, caddr_t optr, size_t olen,
	     mblk_t *dp)
{
	struct TR_begin_res *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + slen + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_BEGIN_RES;
		p->TRANS_id = dg->info.TRANS_id;
		p->ASSOC_flags = dg->info.ASSOC_flags;
		p->ORIG_length = slen;
		p->ORIG_offset = sizeof(*p);
		p->OPT_length = olen;
		p->OPT_offset = p->ORIG_offset + p->ORIG_length;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		tr_set_state(tc, TRS_WACK_CRES);
		LOGDA(tc, "TR_BEGIN_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_cont_req(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	    caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TR_cont_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_BEGIN_RES;
		p->TRANS_id = dg->info.TRANS_id;
		p->ASSOC_flags = dg->info.ASSOC_flags;
		p->OPT_length = olen;
		p->OPT_offset = p->ORIG_offset + p->ORIG_length;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		/* no state change */
		LOGDA(tc, "TR_BEGIN_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_end_req(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	   caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TR_end_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_END_REQ;
		p->TRANS_id = dg->info.TRANS_id;
		p->TERM_scenario = dg->info.TERM_scenario;
		p->OPT_length = olen;
		p->OPT_offset = p->ORIG_offset + p->ORIG_length;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		tr_set_state(tc, TRS_WACK_DREQ9);
		LOGDA(tc, "TR_END_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_abort_req(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg,
	     caddr_t optr, size_t olen, mblk_t *dp)
{
	struct TR_abort_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_ABORT_REQ;
		p->TRANS_id = dg->info.TRANS_id;
		p->ABORT_cause = dg->info.ABORT_cause;
		p->OPT_length = olen;
		p->OPT_offset = p->ORIG_offset + p->ORIG_length;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		tr_set_state(tc, TRS_WACK_DREQ9);
		LOGDA(tc, "TR_ABORT_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
tr_addr_req(struct tc *tc, struct dg *dg, queue_t *q, mblk_t *msg)
{
	struct TR_addr_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_ADDR_REQ;
		p->TRANS_id = dg->info.TRANS_id;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(tc, "TR_ADDR_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_coord_req(struct tc *tc, queue_t *q, mblk_t *msg, caddr_t aptr,
	     size_t alen)
{
	struct TR_coord_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_COORD_REQ;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tc, "TR_COORD_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_coord_res(struct tc *tc, queue_t *q, mblk_t *msg, caddr_t aptr,
	     size_t alen)
{
	struct TR_coord_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_COORD_RES;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tc, "TR_COORD_RES ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall int
tr_state_req(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t status,
	     caddr_t aptr, size_t alen)
{
	struct TR_state_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_STATE_REQ;
		p->ADDR_length = alen;
		p->ADDR_offset = sizeof(*p);
		p->STATUS = status;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(tc, "TR_STATE_REQ ->");
		putnext(tc->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * TC-User -> TC-Provider primitives.
 */
static fastcall int
tc_info_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	    mblk_t *mp)
{
}
static fastcall int
tc_bind_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	    mblk_t *mp)
{
}
static fastcall int
tc_unbind_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	      mblk_t *mp)
{
}
static fastcall int
tc_subs_bind_req(union TC_primitives *p, struct tc *tc, queue_t *q,
		 mblk_t *mp)
{
}
static fastcall int
tc_subs_unbind_req(union TC_primitives *p, struct tc *tc, queue_t *q,
		   mblk_t *mp)
{
}
static fastcall int
tc_optmgmt_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	       mblk_t *mp)
{
}
static fastcall int
tc_uni_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	   mblk_t *mp)
{
}
static fastcall int
tc_begin_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tc_begin_res(union TC_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tc_cont_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	    mblk_t *mp)
{
}
static fastcall int
tc_end_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	   mblk_t *mp)
{
}
static fastcall int
tc_abort_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tc_invoke_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	      mblk_t *mp)
{
}
static fastcall int
tc_result_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	      mblk_t *mp)
{
}
static fastcall int
tc_error_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tc_cancel_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	      mblk_t *mp)
{
}
static fastcall int
tc_reject_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	      mblk_t *mp)
{
}
static fastcall int
tc_coord_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tc_coord_res(union TC_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tc_state_req(union TC_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}

/*
 * TR-Provider -> TR-User primitives.
 */
static fastcall int
tr_info_ack(union TR_primitives *p, struct tc *tc, queue_t *q,
	    mblk_t *mp)
{
}
static fastcall int
tr_bind_ack(union TR_primitives *p, struct tc *tc, queue_t *q,
	    mblk_t *mp)
{
}
static fastcall int
tr_ok_ack(union TR_primitives *p, struct tc *tc, queue_t *q, mblk_t *mp)
{
}
static fastcall int
tr_error_ack(union TR_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tr_optmgmt_ack(union TR_primitives *p, struct tc *tc, queue_t *q,
	       mblk_t *mp)
{
}
static fastcall int
tr_uni_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	   mblk_t *mp)
{
}
static fastcall int
tr_begin_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tr_begin_con(union TR_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tr_cont_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	    mblk_t *mp)
{
}
static fastcall int
tr_end_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	   mblk_t *mp)
{
}
static fastcall int
tr_abort_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tr_notice_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	      mblk_t *mp)
{
}
static fastcall int
tr_addr_ack(union TR_primitives *p, struct tc *tc, queue_t *q,
	    mblk_t *mp)
{
}
static fastcall int
tr_coord_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tr_coord_con(union TR_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tr_state_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	     mblk_t *mp)
{
}
static fastcall int
tr_pcstate_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	       mblk_t *mp)
{
}
static fastcall int
tr_traffic_ind(union TR_primitives *p, struct tc *tc, queue_t *q,
	       mblk_t *mp)
{
}

/*
 * STREAMS message handling
 */

static fastcall __hot_read int
__tc_w_proto(struct tc *tc, queue_t *q, mblk_t *mp)
{
}
static fastcall __hot_write int
__tc_r_proto(struct tc *tc, queue_t *q, mblk_t *mp)
{
}

static fastcall __hot_read int
__tc_w_data(struct tc *tc, queue_t *q, mblk_t *mp)
{
}
static fastcall __hot_write int
__tc_r_data(struct tc *tc, queue_t *q, mblk_t *mp)
{
}

static fastcall __hot_put int
tc_w_proto(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int err = -EAGAIN;

	if (!!(tc = tc_trylock(q))) {
		err = __tc_w_proto(tc, q, mp);
		tc_unlock(tc);
	}
	return (err);
}
static fastcall __hot_get int
tc_r_proto(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int err = -EAGAIN;

	if (!!(tc = tc_trylock(q))) {
		err = __tc_r_proto(tc, q, mp);
		tc_unlock(tc);
	}
	return (err);
}

static fastcall __hot_put int
tc_w_data(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int err = -EAGAIN;

	if (!!(tc = tc_trylock(q))) {
		err = __tc_w_data(tc, q, mp);
		tc_unlock(tc);
	}
	return (err);
}
static fastcall __hot_get int
tc_r_data(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int err = -EAGAIN;

	if (!!(tc = tc_trylock(q))) {
		err = __tc_r_data(tc, q, mp);
		tc_unlock(tc);
	}
	return (err);
}

noinline __unlikely int
tc_w_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
noinline __unlikely int
tc_r_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall int
tc_wsrv_msg(struct tc *tc, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __tc_w_proto(tc, q, mp);
	case M_DATA:
		return __tc_w_data(tc, q, mp);
	default:
		return tc_w_other(q, mp);
	}
}
static fastcall int
tc_rsrv_msg(struct tc *tc, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __tc_r_proto(tc, q, mp);
	case M_DATA:
		return __tc_r_data(tc, q, mp);
	default:
		return tc_r_other(q, mp);
	}
}
static fastcall int
tc_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return tc_w_proto(q, mp);
	case M_DATA:
		return tc_w_data(q, mp);
	default:
		return tc_w_other(q, mp);
	}
}
static fastcall int
tc_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return tc_r_proto(q, mp);
	case M_DATA:
		return tc_r_data(q, mp);
	default:
		return tc_r_other(q, mp);
	}
}

/*
 * QUEUE PUT and SERVICE procedures
 */
static streamscall int
tc_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely
	    (DB_TYPE(mp) < QPCTL
	     && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(tc_wput_msg(q, mp))) {
		tc_wstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}
static streamscall int
tc_wsrv(queue_t *q)
{
	struct tc *tc;
	mblk_t *mp;

	if (likely(!!(tc = tc_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(tc_wsrv_msg(tc, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(tc, "write queue stalled");
				break;
			}
		}
		tc_unlock(tc);
	}
	return (0);
}
static streamscall int
tc_rsrv(queue_t *q)
{
	struct tc *tc;
	mblk_t *mp;

	if (likely(!!(tc = tc_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(tc_rsrv_msg(tc, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(tc, "read queue stalled");
				break;
			}
		}
		tc_unlock(tc);
	}
	return (0);
}
static streamscall int
tc_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely
	    (DB_TYPE(mp) < QPCTL
	     && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(tc_rput_msg(q, mp))) {
		tc_rstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}

/*
 * QUEUE OPEN and CLOSE routines
 */
static streamscall int
tc_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	unsigned long flags;
	struct tc *tc;
	caddr_t ptr;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		return (ENXIO);	/* can't be opened as driver */
	if (!mi_set_sth_lowat(q, 0))
		return (ENOBUFS);
	if (!mi_set_sth_hiwat(q, SHEADHIWAT >> 1))
		return (ENOBUFS);
	if (!
	    (tc =
	     (struct tc *) (ptr =
			    mi_open_alloc_cache(tc_priv_cachep,
						GFP_KERNEL))))
		return (ENOMEM);
	bzero(tc, sizeof(*tc));
	tc->rq = RD(q);
	tc->wq = WR(q);
	tc->cred = *crp;
	tc->dev = *devp;
	write_lock_irqsave(&tc_lock, flags);
	if (mi_acquire_sleep(ptr, &ptr, &tc_lock, &flags) == NULL) {
		err = EINTR;
		goto unlock_free;
	}
	if ((err =
	     mi_open_link(&tc_opens, ptr, devp, oflags, sflag, crp))) {
		mi_release(ptr);
		goto unlock_free;
	}
	mi_attach(q, ptr);
	mi_release(ptr);
	write_unlock_irqrestore(&tc_lock, flags);
	tc_init(tc);
	qprocson(q);
	return (0);
      unlock_free:
	mi_close_free_cache(tc_priv_cachep, ptr);
	write_unlock_irqrestore(&tc_lock, flags);
	return (err);
}
static streamscall int
tc_qclose(queue_t *q, int oflags, cred_t *crp)
{
	unsigned long flags;
	struct tc *tc = PRIV(q);
	caddr_t ptr = (caddr_t) tc;
	int err;

	write_lock_irqsave(&tc_lock, flags);
	mi_acquire_sleep_nosignal(ptr, &ptr, &tc_lock, &flags);
	qprocsoff(q);
	tc_term(tc);
	tc->rq = NULL;
	tc->wq = NULL;
	err = mi_close_comm(&tc_opens, q);
	write_unlock_irqrestore(&tc_lock, flags);
	if (err)
		cmn_err(CE_WARN, "%s: error closing stream, err = %d",
			__FUNCTION__, err);
	return (err);
}

/*
 * STREAMS DEFINITIONS
 */

static struct qinit tc_rinit = {
	.qi_putp = tc_rput,
	.qi_srvp = tc_rsrv,
	.qi_qopen = tc_qopen,
	.qi_qclose = tc_qclose,
	.qi_minfo = &tc_minfo,
	.qi_mstat = &tc_rstat,
};

static struct qinit tc_winit = {
	.qi_putp = tc_wput,
	.qi_srvp = tc_wsrv,
	.qi_minfo = &tc_minfo,
	.qi_mstat = &tc_wstat,
};

MODULE_STATIC struct streamtab tcmodinfo = {
	.st_rdinit = &tc_rinit,
	.st_wrinit = &tc_winit,
};

STATIC struct fmodsw tc_fmod = {
	.f_name = MOD_NAME,
	.f_str = &tcmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/*
 * Kernel module initialization
 */

MODULE_STATIC int __init
tcmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = tc_init_caches()) < 0) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d",
			__FUNCTION__, err);
		return (err);
	}
	if ((err = register_strmod(&tc_fmod)) < 0) {
		cmn_err(CE_WARN,
			"%s: could not register module, err = %d",
			__FUNCTION__, err);
		tc_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
tcmodexit(void)
{
	int err;

	if ((err = unregsiter_strmod(&tc_fmod)) < 0)
		cmn_err(CE_WARN,
			"%s: could not unregister module, err = %d",
			__FUNCTION__, err);
	if ((err = tc_term_caches()) < 0)
		cmn_err(CE_WARN,
			"%s: could not terminate caches, err = %d",
			__FUNCTION__, err);
	return;
}

module_init(tcmodinit);
module_exit(tcmodexit);
