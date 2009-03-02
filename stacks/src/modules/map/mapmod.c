/*****************************************************************************

 @(#) $RCSfile: mapmod.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2009-03-02 05:15:02 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-03-02 05:15:02 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mapmod.c,v $
 Revision 0.9.2.1  2009-03-02 05:15:02  brian
 - updates

 *****************************************************************************/

#ident "@(#) $RCSfile: mapmod.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2009-03-02 05:15:02 $"

static char const ident[] = "$RCSfile: mapmod.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2009-03-02 05:15:02 $";

/*
 * This is MAPMOD, a GSM Mobile Application Part module implementing the MAPI
 * interface as described in GSM 29.002 as a module pushed over a TCI stream.
 * This module does not implement all of the service invocations of MAPI as
 * described in GSM 29.002, but implements the basic MAP open, continue, close,
 * delimit and other basic commands.
 *
 * MAPI in GSM 29.002 is described for using the TCI as described in ITU-T Rec.
 * Q.771 which is implemented by the OpenSS7 projects as the TCI interface.  TCI
 * itself can be implementd as a pushable module over the TRI interface, also
 * described in Q.771 and implemented by the OpenSS7 project as the TRI
 * interace.
 */
#define _DEBUG 1

#define _LFS_SOURCE 1
#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1

#include <sys/os8/compat.h>
#include <sys/strsun.h>

#include <ss7/ss7.h>
#include <ss7/mtp.h>
#include <ss7/sccp.h>
#include <ss7/tr.h>
#include <ss7/tc.h>
#include <ss7/map.h>

#define MAPMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MAPMOD_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define MAPMOD_COPYRIGHT	"Copyright (c) 2008-2009 Monavacon Limited.  All Rights Reserved."
#define MAPMOD_REVISION		"Monavacon $RCSfile: mapmod.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2009-03-02 05:15:02 $"
#define MAPMOD_DEVICE		"GSM 29.002 MAP module for ITU-T Q.771 TC"
#define MAPMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MAPMOD_LICENSE		"GPL"
#define MAPMOD_BANNER		MAPMOD_DESCRIP		"\n" \
				MAPMOD_EXTRA		"\n" \
				MAPMOD_REVISION		"\n" \
				MAPMOD_COPYRIGHT	"\n" \
				MAPMOD_DEVICE		"\n" \
				MAPMOD_CONTACT
#define MAPMOD_SPLASH		MAPMOD_DESCRIP		" - " \
				MAPMOD_REVISION

#ifdef LINUX
MODULE_AUTHOR(MAPMOD_CONTACT);
MODULE_DESCRIPTION(MAPMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(MAPMOD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MAPMOD_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mapmod");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRLEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define MAPMOD_MOD_ID	CONFIG_STREAMS_MAPMOD_MODID
#define MAPMOD_MOD_NAME	CONFIG_STREAMS_MAPMOD_NAME
#endif LFS

#ifndef MAPMOD_MOD_ID
#error "MAPMOD_MOD_ID must be defined."
#endif
#ifndef MAPMOD_MOD_NAME
#error "MAPMOD_MOD_NAME must be defined."
#endif

modID_t modid = MAPMOD_MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for MAPMOD.  (0 for allocation.)");

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
#define MOD_ID		MAPMOD_MOD_ID
#define MOD_NAME	MAPMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MAPMOD_BANNER
#else				/* MODULE */
#define MOD_BANNER	MAPMOD_SPLASH
#endif				/* MODULE */


static struct module_info map_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat map_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat map_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static rwlock_t map_lock = RW_LOCK_UNLOCKED;
static caddr_t map_opens = NULL;

/*
 * LOCKING
 */
static inline fastcall struct map *
map_trylock(queue_t *q)
{
	struct map *map;

	read_lock(&map_lock);
	map = (struct map *) mi_trylock(q);
	read_unlock(&map_lock);
	return (map);
}

static inline fastcall void
map_unlock(struct map *map)
{
	read_lock(&map_lock);
	mi_unlock((caddr_t) map);
	read_unlock(&map_lock);
}

/*
 * CACHES
 */
static kmem_cachep_t map_priv_cachep = NULL;

static int
map_init_caches(void)
{
	if (!map_priv_cachep && !(map_priv_cachep = kmem_create_cache("map_priv_cachep", mi_open_size(sizeof(struct map)), 0, SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocat map_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver private structure cache", MOD_NAME);
	return (0);
}
static int
map_term_caches(void)
{
	if (map_priv_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(map_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy map_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed map_priv_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(map_priv_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	return (0);
}

/*
 * TC-User to TC-Provider primitives
 */
static fastcall int
tc_begin_req(struct map *map, queue_t *q, mblk_t *msg)
{
	struct TC_begin_req *p;
	mblk_t *mp;

	if (likely(canputnext(map->wq))) {
		if (!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			p = (typeof(p))mp->b_wptr;
			p->PRIM_type = TC_BEGIN_REQ;
			mp->b_wptr += sizeof(p);
			LOGTX(map, "-> TC_BEGIN_REQ");
			putnext(map->wq, mp);
			freemsg(msg);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 * TC-Provider to TC-User primitives
 */
static fastcall int
tc_begin_ind(struct map *map, queue_t *q, mblk_t *msg)
{
	struct TC_begin_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_begin_con(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_begin_con *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_cont_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_cont_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_end_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_end_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_abort_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_abort_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_uni_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_uni_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_notice_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_notice_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_invoke_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_invoke_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_result_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_result_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_error_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_error_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_cancel_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_cancel_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_reject_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	struct TC_reject_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_other_ind(struct map *map, queue_t *q, mblk_t *mp)
{
	t_uscalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * STREAMS message handling
 */

static fastcall __hot_read int
__map_w_proto(struct map *map, queue_t *q, mblk_t *mp)
{
	union MAP_primitives *p;

	if (!MBLKIN(mp, 0, sizeof(p->type)))
		goto emsgsize;
	switch (p->PRIM_type) {
	}
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall __hot_write int
__map_r_proto(struct map *map, queue_t *q, mblk_t *mp)
{
	union TC_primitives *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(p->type)))
		goto emsgsize;
	map_save_state(map);
	switch (p->PRIM_type) {
	case TC_BEGIN_IND:
		err = tc_begin_ind(map, q, mp);
		break;
	case TC_BEGIN_CON:
		err = tc_begin_con(map, q, mp);
		break;
	case TC_CONT_IND:
		err = tc_cont_ind(map, q, mp);
		break;
	case TC_END_IND:
		err = tc_end_ind(map, q, mp);
		break;
	case TC_ABORT_IND:
		err = tc_abort_ind(map, q, mp);
		break;

	case TC_UNI_IND:
		err = tc_uni_ind(map, q, mp);
		break;
	case TC_NOTICE_IND:
		err = tc_notice_ind(map, q, mp);
		break;
	case TC_INVOKE_IND:
		err = tc_invoke_ind(map, q, mp);
		break;
	case TC_RESULT_IND:
		err = tc_result_ind(map, q, mp);
		break;
	case TC_ERROR_IND:
		err = tc_error_ind(map, q, mp);
		break;
	case TC_CANCEL_IND:
		err = tc_cancel_ind(map, q, mp);
		break;
	case TC_REJECT_IND:
		err = tc_reject_ind(map, q, mp);
		break;

	default:
		err = tc_other_ind(map, q, mp);
		break;
	}
	if (err < 0) {
		map_restore_state(map);
		goto error;
	}
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall __hot_read int
__map_w_data(struct map *map, queue_t *q, mblk_t *mp)
{
}
static fastcall __hot_write int
__map_r_data(struct map *map, queue_t *q, mblk_t *mp)
{
}

static fastcall __hot_put int
map_w_proto(queue_t *q, mblk_t *mp)
{
	struct map *map;
	int err = -EAGAIN;

	if (!!(map = map_trylock(q))) {
		err = __map_w_proto(map, q, mp);
		map_unlock(map);
	}
	return (err);
}
static fastcall __hot_get int
map_r_proto(queue_t *q, mblk_t *mp)
{
	struct map *map;
	int err = -EAGAIN;

	if (!!(map = map_trylock(q))) {
		err = __map_r_proto(map, q, mp);
		map_unlock(map);
	}
	return (err);
}

static fastcall __hot_put int
map_w_data(queue_t *q, mblk_t *mp)
{
	struct map *map;
	int err = -EAGAIN;

	if (!!(map = map_trylock(q))) {
		err = __map_w_data(map, q, mp);
		map_unlock(map);
	}
	return (err);
}
static fastcall __hot_get int
map_r_data(queue_t *q, mblk_t *mp)
{
	struct map *map;
	int err = -EAGAIN;

	if (!!(map = map_trylock(q))) {
		err = __map_r_data(map, q, mp);
		map_unlock(map);
	}
	return (err);
}

noinline __unlikely int
map_w_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
noinline __unlikely int
map_r_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall int
map_wsrv_msg(struct map *map, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __map_w_proto(map, q, mp);
	case M_DATA:
		return __map_w_data(map, q, mp);
	default:
		return map_w_other(q, mp);
	}
}
static fastcall int
map_rsrv_msg(struct map *map, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __map_r_proto(map, q, mp);
	case M_DATA:
		return __map_r_data(map, q, mp);
	default:
		return map_r_other(q, mp);
	}
}
static fastcall int
map_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return map_w_proto(q, mp);
	case M_DATA:
		return map_w_data(q, mp);
	default:
		return map_w_other(q, mp);
	}
}
static fastcall int
map_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return map_r_proto(q, mp);
	case M_DATA:
		return map_r_data(q, mp);
	default:
		return map_r_other(q, mp);
	}
}

/*
 * QUEUE PUT and SERVICE procedures
 */
static streamscall int
map_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(DB_TYPE(mp) < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY))) || unlikely(map_wput_msg(q, mp))) {
		map_wstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}
static streamscall int
map_wsrv(queue_t *q)
{
	struct map *map;
	mblk_t *mp;

	if (likely(!!(map = map_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(map_wsrv_msg(map, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0; /* must succeed */
					putbq(q, mp);
				}
				LOGTX(map, "write queue stalled");
				break;
			}
		}
		map_unlock(map);
	}
	return (0);
}
static streamscall int
map_rsrv(queue_t *q)
{
	struct map *map;
	mblk_t *mp;

	if (likely(!!(map = map_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(map_rsrv_msg(map, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0; /* must succeed */
					putbq(q, mp);
				}
				LOGTX(map, "read queue stalled");
				break;
			}
		}
		map_unlock(map);
	}
	return (0);
}
static streamscall int
map_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(DB_TYPE(mp) < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY))) || unlikely(map_rput_msg(q, mp))) {
		map_rstat.ms_acnt++;
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
map_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	unsigned long flags;
	struct map *map;
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
	if (!(map = (struct map *) (ptr = mi_open_alloc_cache(map_priv_cachep, GFP_KERNEL))))
		return (ENOMEM);
	bzero(map, sizeof(*map));
	map->rq = RD(q);
	map->wq = WR(q);
	map->cred = *crp;
	map->dev = *devp;
	write_lock_irqsave(&map_lock, flags);
	if (mi_acquire_sleep(ptr, &ptr, &map_lock, &flags) == NULL) {
		err = EINTR;
		goto unlock_free;
	}
	if ((err = mi_open_link(&map_opens, ptr, devp, oflags, sflag, crp))) {
		mi_release(ptr);
		goto unlock_free;
	}
	mi_attach(q, ptr);
	mi_release(ptr);
	write_unlock_irqrestore(&map_lock, flags);
	map_init(map);
	qprocson(q);
	return (0);
      unlock_free:
	mi_close_free_cache(map_priv_cachep, ptr);
	write_unlock_irqrestore(&map_lock, flags);
	return (err);
}
static streamscall int
map_qclose(queue_t *q, int oflags, cred_t *credp)
{
	unsigned long flags;
	struct map *map = PRIV(q);
	caddr_t ptr = (caddr_t) map;
	int err;

	write_lock_irqsave(&map_lock, flags);
	mi_acquire_sleep_nosignal(ptr, &ptr, &map_lock, &flags);
	qprocsoff(q);
	map_term(map);
	map->rq = NULL;
	map->wq = NULL;
	err = mi_close_comm(&map_opens, q);
	write_unlock_irqrestore(&map_lock, flags);
	if (err)
		cmn_err(CE_WARN, "%s: error closing stream, err = %d", __FUNCTION__, err);
	return (err);
}

/*
 * STREAMS DEFINITIONS
 */

static struct qinit map_rinit = {
	.qi_putp = map_rput,	/* Read put (message from below) */
	.qi_srvp = map_rsrv,	/* Read service */
	.qi_qopen = map_qopen,	/* Each open */
	.qi_qclose = map_qclose,	/* Last close */
	.qi_minfo = &map_minfo,	/* Information */
	.qi_mstat = &map_rstat,	/* Statistics */
};

static struct qinit map_winit = {
	.qi_putp = map_wput,	/* Write put (message from above) */
	.qi_srvp = map_wsrv,	/* Write service */
	.qi_minfo = &map_minfo,	/* Information */
	.qi_mstat = &map_wstat,	/* Statistics */
};

MODULE_STATIC struct streamtab mapmodinfo = {
	.st_rdinit = &map_rinit,	/* Upper read queue */
	.st_wrinit = &map_winit,	/* Upper write queue */
};

STATIC struct fmodsw map_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mapmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
mapmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = map_init_caches()) < 0) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = register_strmod(&map_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		map_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
mapmodexit(void)
{
	int err;

	if ((err = unregister_strmod(&map_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
	if ((err = map_term_caches()) < 0)
		cmn_err(CE_WARN, "%s: could not terminate caches, err = %d", MOD_NAME, err);
	return;
}

module_init(mapmodinit);
module_exit(mapmodexit);
