/*****************************************************************************

 @(#) $RCSfile: suncompat.c,v $ $Name:  $($Revision: 0.9.2.32 $) $Date: 2007/03/30 11:59:24 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/03/30 11:59:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: suncompat.c,v $
 Revision 0.9.2.32  2007/03/30 11:59:24  brian
 - heavy rework of MP syncrhonization

 Revision 0.9.2.31  2007/03/02 10:04:08  brian
 - updates to common build process and versions for all exported symbols

 Revision 0.9.2.30  2006/11/03 10:39:28  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ident "@(#) $RCSfile: suncompat.c,v $ $Name:  $($Revision: 0.9.2.32 $) $Date: 2007/03/30 11:59:24 $"

static char const ident[] = "$RCSfile: suncompat.c,v $ $Name:  $($Revision: 0.9.2.32 $) $Date: 2007/03/30 11:59:24 $";

/* 
 *  This is my solution for those who don't want to inline GPL'ed functions or
 *  who don't use optimizations when compiling or specifies
 *  -fnoinline-functions or something of the like.  This file implements all
 *  of the extern inlines from the header files by just including the header
 *  files with the functions declared 'inline' instead of 'extern inline'.
 *
 *  There are implemented here in a separate object, out of the way of the
 *  modules that don't use them.
 */

#define __SUN_EXTERN_INLINE __inline__ streamscall __unlikely

#define _SUN_SOURCE

#include "sys/os7/compat.h"

#define SUNCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SUNCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define SUNCOMP_REVISION	"LfS $RCSfile: suncompat.c,v $ $Name:  $($Revision: 0.9.2.32 $) $Date: 2007/03/30 11:59:24 $"
#define SUNCOMP_DEVICE		"Solaris(R) 8 Compatibility"
#define SUNCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SUNCOMP_LICENSE		"GPL"
#define SUNCOMP_BANNER		SUNCOMP_DESCRIP		"\n" \
				SUNCOMP_COPYRIGHT	"\n" \
				SUNCOMP_REVISION	"\n" \
				SUNCOMP_DEVICE		"\n" \
				SUNCOMP_CONTACT		"\n"
#define SUNCOMP_SPLASH		SUNCOMP_DEVICE		" - " \
				SUNCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_SUN_MODULE
MODULE_AUTHOR(SUNCOMP_CONTACT);
MODULE_DESCRIPTION(SUNCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SUNCOMP_DEVICE);
MODULE_LICENSE(SUNCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-suncompat");
#endif
#endif

__SUN_EXTERN_INLINE void freezestr_SUN(queue_t *q);

EXPORT_SYMBOL(freezestr_SUN);
__SUN_EXTERN_INLINE void unfreezestr_SUN(queue_t *q);

EXPORT_SYMBOL(unfreezestr_SUN);

#ifdef LFS
/**
 *  qwait:  - wait for a procedure to be called on a queue pair
 *  @rq:    a pointer to the read queue of the queue pair
 */
void
qwait(queue_t *rq)
{
	struct queinfo *qu = (typeof(qu)) rq;

	DECLARE_WAITQUEUE(wait, current);
	dassert(!in_interrupt());
	ensure(!test_bit(QHLIST_BIT, &rq->q_flag), qprocson(rq));
	add_wait_queue(&qu->qu_qwait, &wait);
	set_current_state(TASK_UNINTERRUPTIBLE);

	streams_schedule();

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&qu->qu_qwait, &wait);
}

EXPORT_SYMBOL(qwait);		/* sun/ddi.h */
#endif

#ifdef LFS
/**
 *  qwait_sig: - wait for a procedure on a queue pair or signal
 *  @rq:    a pointer to the read queue of the queue pair
 */
int
qwait_sig(queue_t *rq)
{
	struct queinfo *qu = (typeof(qu)) rq;
	int interrupted;

	DECLARE_WAITQUEUE(wait, current);
	dassert(!in_interrupt());
	ensure(!test_bit(QHLIST_BIT, &rq->q_flag), qprocson(rq));
	add_wait_queue(&qu->qu_qwait, &wait);
	set_current_state(TASK_UNINTERRUPTIBLE);

	streams_schedule();
	interrupted = (signal_pending(current) != 0);

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&qu->qu_qwait, &wait);
	return (interrupted ? 1 : 0);
}

EXPORT_SYMBOL(qwait_sig);	/* sun/ddi.h */
#endif

#ifdef LFS
/**
 *  qbufcall:	- schedule a buffer callout
 *  @q:		queue used for synchronization
 *  @size:	the number of bytes of data buffer needed
 *  @priority:	the priority of the buffer allocation (ignored)
 *  @function:	the callback function when bytes and headers are available
 *  @arg:	a client argument to pass to the callback function
 */
bufcall_id_t
qbufcall(queue_t *q, size_t size, int priority, void streamscall (*function) (void *), void *arg)
{
	// queue_t *rq = RD(q);
	// assert(!test_bit(QHLIST_BIT, &rq->q_flag));
	return __bufcall(q, size, priority, (void streamscall (*)(long)) function, (long) arg);
}

EXPORT_SYMBOL(qbufcall);	/* sun/ddi.h */
timeout_id_t
qtimeout(queue_t *q, void streamscall (*timo_fcn) (void *), void *arg, long ticks)
{
	// queue_t *rq = RD(q);
	// assert(!test_bit(QHLIST_BIT, &rq->q_flag));
	return __timeout(q, (timo_fcn_t *) timo_fcn, (caddr_t) arg, ticks, 0, smp_processor_id());
}

EXPORT_SYMBOL(qtimeout);	/* sun/ddi.h */
#endif
/**
 *  qunbufcall: - cancel a buffer callout
 *  @q:		queue used for synchronization
 *  @bcid:	buffer call id returned by qbufcall()
 *  Notices:	Don't ever call this function with an expired bufcall id.
 */
void
qunbufcall(queue_t *q, bufcall_id_t bcid)
{
	unbufcall(bcid);
}

EXPORT_SYMBOL(qunbufcall);	/* sun/ddi.h */
clock_t
quntimeout(queue_t *q, timeout_id_t toid)
{
	return untimeout(toid);
}

EXPORT_SYMBOL(quntimeout);	/* sun/ddi.h */
#ifdef LFS
/* LIS already has queclass defined */
__SUN_EXTERN_INLINE unsigned char queclass(mblk_t *mp);

EXPORT_SYMBOL(queclass);	/* sun/ddi.h */
#endif
#ifdef LFS
/**
 *  qwriter:	- deferred call to a callback function.
 *  @qp:	a pointer to the RD() queue of a queue pair
 *  @mp:	message pointer to pass to writer function
 *  @func:	writer function
 *  @perimeter:	perimeter to enter
 *
 *  qwriter() will defer the function @func until it can enter @perimeter associated with queue pair
 *  @qp.  Once the @perimeter has been entered, the STREAMS executive will call the callback
 *  function @func with arguments @qp and @mp.  qwriter() is closely related to streams_put() above.
 *
 *  Notices: @func will be called by the STREAMS executive on the same CPU as the CPU that called
 *  qwriter().  @func is guarateed not to run until the caller exits or preempts.
 */
__SUN_EXTERN_INLINE void
qwriter(queue_t *qp, mblk_t *mp, void streamscall (*func) (queue_t *qp, mblk_t *mp), int perimeter)
{
	extern void __strwrit(queue_t *q, mblk_t *mp, void streamscall (*func)(queue_t *, mblk_t *), int perim);
	__strwrit(qp, mp, func, perimeter);
}

EXPORT_SYMBOL(qwriter);		/* sun/ddi.h */
__SUN_EXTERN_INLINE cred_t *ddi_get_cred(void);

EXPORT_SYMBOL(ddi_get_cred);	/* sun/ddi.h */
#endif
__SUN_EXTERN_INLINE clock_t ddi_get_lbolt(void);

EXPORT_SYMBOL(ddi_get_lbolt);	/* sun/ddi.h */
__SUN_EXTERN_INLINE pid_t ddi_get_pid(void);

EXPORT_SYMBOL(ddi_get_pid);	/* sun/ddi.h */
__SUN_EXTERN_INLINE time_t ddi_get_time(void);

EXPORT_SYMBOL(ddi_get_time);	/* sun/ddi.h */
__SUN_EXTERN_INLINE unsigned short ddi_getiminor(dev_t dev);

EXPORT_SYMBOL(ddi_getiminor);	/* sun/ddi.h */
__SUN_EXTERN_INLINE void *ddi_umem_alloc(size_t size, int flag, ddi_umem_cookie_t * cookiep);

EXPORT_SYMBOL(ddi_umem_alloc);	/* sun/ddi.h */
__SUN_EXTERN_INLINE void *ddi_umem_free(ddi_umem_cookie_t * cookiep);

EXPORT_SYMBOL(ddi_umem_free);

#if 0
extern int ddi_add_intr(void);
extern int ddi_add_softintr(void);
extern int ddi_binding_name(void);
extern int ddi_btop(void);
extern int ddi_btopr(void);
extern int ddi_check_acc_handle(void);
extern int ddi_check_dma_handle(void);
extern int ddi_copyin(void);
extern int ddi_copyout(void);
extern int ddi_create_minor_node(void);
extern int ddi_dev_is_needed(void);
extern int ddi_dev_is_sid(void);
extern int ddi_dev_nintrs(void);
extern int ddi_dev_nregs(void);
extern int ddi_dev_regsize(void);
extern int ddi_dev_report_fault(void);
extern int ddi_device_copy(void);
extern int ddi_device_zero(void);
extern int ddi_devid_compare(void);
extern int ddi_devid_free(void);
extern int ddi_devid_init(void);
extern int ddi_devid_register(void);
extern int ddi_devid_sizeof(void);
extern int ddi_devid_str_decode(void);
extern int ddi_devid_str_encode(void);
extern int ddi_devid_str_free(void);
extern int ddi_devid_unregister(void);
extern int ddi_devid_valid(void);
extern int ddi_devmap_segmap(void);
extern int ddi_dma_addr_bind_handle(void);
extern int ddi_dma_addr_setup(void);
extern int ddi_dma_alloc_handle(void);
extern int ddi_dma_buf_bind_handle(void);
extern int ddi_dma_buf_setup(void);
extern int ddi_dma_burstsizes(void);
extern int ddi_dma_coff(void);
extern int ddi_dma_curwin(void);
extern int ddi_dma_devalign(void);
extern int ddi_dma_free(void);
extern int ddi_dma_free_handle(void);
extern int ddi_dma_get_attr(void);
extern int ddi_dma_getwin(void);
extern int ddi_dma_htoc(void);
extern int ddi_dma_mem_alloc(void);
extern int ddi_dma_mem_free(void);
extern int ddi_dma_movwin(void);
extern int ddi_dma_nextcookie(void);
extern int ddi_dma_nextseg(void);
extern int ddi_dma_nextwin(void);
extern int ddi_dma_numwin(void);
extern int ddi_dma_segtocookie(void);
extern int ddi_dma_set_sbus64(void);
extern int ddi_dma_setup(void);
extern int ddi_dma_sync(void);
extern int ddi_dma_unbind_handle(void);
extern int ddi_dmae(void);
extern int ddi_dmae_1stparty(void);
extern int ddi_dmae_alloc(void);
extern int ddi_dmae_disable(void);
extern int ddi_dmae_enable(void);
extern int ddi_dmae_getattr(void);
extern int ddi_dmae_getcnt(void);
extern int ddi_dmae_getlim(void);
extern int ddi_dmae_prog(void);
extern int ddi_dmae_release(void);
extern int ddi_dmae_stop(void);
extern int ddi_driver_major(void);
extern int ddi_driver_name(void);
extern int ddi_enter_critical(void);
extern int ddi_exit_critical(void);
extern int ddi_ffs(void);
extern int ddi_fls(void);
extern int ddi_get16(void);
extern int ddi_get32(void);
extern int ddi_get64(void);
extern int ddi_get8(void);
extern int ddi_get_cred(void);
extern int ddi_get_devstate(void);
extern int ddi_get_driver_private(void);
extern int ddi_get_iblock_cookie(void);
extern int ddi_get_instance(void);
extern int ddi_get_kt_did(void);
extern int ddi_get_lbolt(void);
extern int ddi_get_name(void);
extern int ddi_get_parent(void);
extern int ddi_get_pid(void);
extern int ddi_get_soft_iblock_cookie(void);
extern int ddi_get_soft_state(void);
extern int ddi_get_time(void);
extern int ddi_getb(void);
extern int ddi_getiminor(void);
extern int ddi_getl(void);
extern int ddi_getll(void);
extern int ddi_getlongprop(void);
extern int ddi_getlongprop_buf(void);
extern int ddi_getprop(void);
extern int ddi_getproplen(void);
extern int ddi_getw(void);
extern int ddi_in_panic(void);
extern int ddi_intr_hilevel(void);
extern int ddi_io_get16(void);
extern int ddi_io_get32(void);
extern int ddi_io_get64(void);
extern int ddi_io_get8(void);
extern int ddi_io_getb(void);
extern int ddi_io_getl(void);
extern int ddi_io_getw(void);
extern int ddi_io_put16(void);
extern int ddi_io_put32(void);
extern int ddi_io_put64(void);
extern int ddi_io_put8(void);
extern int ddi_io_putb(void);
extern int ddi_io_putl(void);
extern int ddi_io_putw(void);
extern int ddi_io_rep_get16(void);
extern int ddi_io_rep_get32(void);
extern int ddi_io_rep_get64(void);
extern int ddi_io_rep_get8(void);
extern int ddi_io_rep_getb(void);
extern int ddi_io_rep_getl(void);
extern int ddi_io_rep_getw(void);
extern int ddi_io_rep_put16(void);
extern int ddi_io_rep_put32(void);
extern int ddi_io_rep_put64(void);
extern int ddi_io_rep_put8(void);
extern int ddi_io_rep_putb(void);
extern int ddi_io_rep_putl(void);
extern int ddi_io_rep_putw(void);
extern int ddi_iomin(void);
extern int ddi_iopb_alloc(void);
extern int ddi_iopb_free(void);
extern int ddi_log_sysevent(void);
extern int ddi_map_regs(void);
extern int ddi_mapdev(void);
extern int ddi_mapdev_intercept(void);
extern int ddi_mapdev_nointercept(void);
extern int ddi_mem_alloc(void);
extern int ddi_mem_free(void);
extern int ddi_mem_get16(void);
extern int ddi_mem_get32(void);
extern int ddi_mem_get64(void);
extern int ddi_mem_get8(void);
extern int ddi_mem_getb(void);
extern int ddi_mem_getl(void);
extern int ddi_mem_getll(void);
extern int ddi_mem_getw(void);
extern int ddi_mem_put16(void);
extern int ddi_mem_put32(void);
extern int ddi_mem_put64(void);
extern int ddi_mem_put8(void);
extern int ddi_mem_putb(void);
extern int ddi_mem_putl(void);
extern int ddi_mem_putll(void);
extern int ddi_mem_putw(void);
extern int ddi_mem_rep_get16(void);
extern int ddi_mem_rep_get32(void);
extern int ddi_mem_rep_get64(void);
extern int ddi_mem_rep_get8(void);
extern int ddi_mem_rep_getb(void);
extern int ddi_mem_rep_getl(void);
extern int ddi_mem_rep_getll(void);
extern int ddi_mem_rep_getw(void);
extern int ddi_mem_rep_put16(void);
extern int ddi_mem_rep_put32(void);
extern int ddi_mem_rep_put64(void);
extern int ddi_mem_rep_put8(void);
extern int ddi_mem_rep_putb(void);
extern int ddi_mem_rep_putl(void);
extern int ddi_mem_rep_putll(void);
extern int ddi_mem_rep_putw(void);
extern int ddi_mmap_get_model(void);
extern int ddi_model_convert_from(void);
extern int ddi_node_name(void);
extern int ddi_peek(void);
extern int ddi_peek16(void);
extern int ddi_peek32(void);
extern int ddi_peek64(void);
extern int ddi_peek8(void);
extern int ddi_peekc(void);
extern int ddi_peekd(void);
extern int ddi_peekl(void);
extern int ddi_peeks(void);
extern int ddi_poke(void);
extern int ddi_poke16(void);
extern int ddi_poke32(void);
extern int ddi_poke64(void);
extern int ddi_poke8(void);
extern int ddi_pokec(void);
extern int ddi_poked(void);
extern int ddi_pokel(void);
extern int ddi_pokes(void);
extern int ddi_prop_create(void);
extern int ddi_prop_exists(void);
extern int ddi_prop_free(void);
extern int ddi_prop_get_int(void);
extern int ddi_prop_get_int64(void);
extern int ddi_prop_lookup(void);
extern int ddi_prop_lookup_byte_array(void);
extern int ddi_prop_lookup_int64_array(void);
extern int ddi_prop_lookup_int_array(void);
extern int ddi_prop_lookup_string(void);
extern int ddi_prop_lookup_string_array(void);
extern int ddi_prop_modify(void);
extern int ddi_prop_op(void);
extern int ddi_prop_remove(void);
extern int ddi_prop_remove_all(void);
extern int ddi_prop_undefine(void);
extern int ddi_prop_update(void);
extern int ddi_prop_update_byte_array(void);
extern int ddi_prop_update_int(void);
extern int ddi_prop_update_int64(void);
extern int ddi_prop_update_int64_array(void);
extern int ddi_prop_update_string(void);
extern int ddi_prop_update_string_array(void);
extern int ddi_ptob(void);
extern int ddi_put16(void);
extern int ddi_put32(void);
extern int ddi_put64(void);
extern int ddi_put8(void);
extern int ddi_putb(void);
extern int ddi_putl(void);
extern int ddi_putll(void);
extern int ddi_putw(void);
extern int ddi_regs_map_free(void);
extern int ddi_regs_map_setup(void);
extern int ddi_remove_intr(void);
extern int ddi_remove_minor_node(void);
extern int ddi_remove_softintr(void);
extern int ddi_removing_power(void);
extern int ddi_rep_get16(void);
extern int ddi_rep_get32(void);
extern int ddi_rep_get64(void);
extern int ddi_rep_get8(void);
extern int ddi_rep_getb(void);
extern int ddi_rep_getl(void);
extern int ddi_rep_getll(void);
extern int ddi_rep_getw(void);
extern int ddi_rep_put16(void);
extern int ddi_rep_put32(void);
extern int ddi_rep_put64(void);
extern int ddi_rep_put8(void);
extern int ddi_rep_putb(void);
extern int ddi_rep_putl(void);
extern int ddi_rep_putll(void);
extern int ddi_rep_putw(void);
extern int ddi_report_dev(void);
extern int ddi_root_node(void);
extern int ddi_segmap(void);
extern int ddi_segmap_setup(void);
extern int ddi_set_driver_private(void);
extern int ddi_slaveonly(void);
extern int ddi_soft_state(void);
extern int ddi_soft_state_fini(void);
extern int ddi_soft_state_free(void);
extern int ddi_soft_state_init(void);
extern int ddi_soft_state_zalloc(void);
extern int ddi_trigger_softintr(void);
extern int ddi_umem_alloc(void);
extern int ddi_umem_free(void);
extern int ddi_umem_lock(void);
extern int ddi_umem_unlock(void);
extern int ddi_unmap_regs(void);
#endif

/*
 *  Solaris helper functions from sys/strsun.h
 */
__SUN_EXTERN_INLINE unsigned char *DB_BASE(mblk_t *mp);

EXPORT_SYMBOL(DB_BASE);
__SUN_EXTERN_INLINE unsigned char *DB_LIM(mblk_t *mp);

EXPORT_SYMBOL(DB_LIM);
__SUN_EXTERN_INLINE size_t DB_REF(mblk_t *mp);

EXPORT_SYMBOL(DB_REF);
__SUN_EXTERN_INLINE int DB_TYPE(mblk_t *mp);

EXPORT_SYMBOL(DB_TYPE);
__SUN_EXTERN_INLINE long MBLKL(mblk_t *mp);

EXPORT_SYMBOL(MBLKL);
__SUN_EXTERN_INLINE long MBLKSIZE(mblk_t *mp);

EXPORT_SYMBOL(MBLKSIZE);
__SUN_EXTERN_INLINE long MBLKHEAD(mblk_t *mp);

EXPORT_SYMBOL(MBLKHEAD);
__SUN_EXTERN_INLINE long MBLKTAIL(mblk_t *mp);

EXPORT_SYMBOL(MBLKTAIL);
__SUN_EXTERN_INLINE long MBLKIN(mblk_t *mp, ssize_t off, size_t len);

EXPORT_SYMBOL(MBLKIN);
__SUN_EXTERN_INLINE long OFFSET(void *p, void *base);

EXPORT_SYMBOL(OFFSET);
__SUN_EXTERN_INLINE void merror(queue_t *q, mblk_t *mp, int error);

EXPORT_SYMBOL(merror);
__SUN_EXTERN_INLINE void mioc2ack(mblk_t *mp, mblk_t *db, size_t count, int rval);

EXPORT_SYMBOL(mioc2ack);
__SUN_EXTERN_INLINE void miocack(queue_t *q, mblk_t *mp, int count, int rval);

EXPORT_SYMBOL(miocack);
__SUN_EXTERN_INLINE void miocnak(queue_t *q, mblk_t *mp, int count, int error);

EXPORT_SYMBOL(miocnak);
__SUN_EXTERN_INLINE mblk_t *mexchange(queue_t *q, mblk_t *mp, size_t size, int type,
				      uint32_t primtype);
EXPORT_SYMBOL(mexchange);
__SUN_EXTERN_INLINE mblk_t *mexpandb(mblk_t *mp, int i1, int i2);

EXPORT_SYMBOL(mexpandb);
__SUN_EXTERN_INLINE int miocpullup(mblk_t *mp, size_t len);

EXPORT_SYMBOL(miocpullup);
#if 0
/* contained in the base package */
__SUN_EXTERN_INLINE size_t msgsize(mblk_t *mp);

EXPORT_SYMBOL(msgsize);
#endif
__SUN_EXTERN_INLINE void mcopymsg(mblk_t *mp, unsigned char *buf);

EXPORT_SYMBOL(mcopymsg);
__SUN_EXTERN_INLINE void mcopyin(mblk_t *mp, void *priv, size_t size, void *uaddr);

EXPORT_SYMBOL(mcopyin);
__SUN_EXTERN_INLINE void mcopyout(mblk_t *mp, void *priv, size_t size, void *uaddr, mblk_t *dp);

EXPORT_SYMBOL(mcopyout);

/* 
 *  Configuration
 */

__SUN_EXTERN_INLINE int nodev(void);

EXPORT_SYMBOL(nodev);		/* strconf.h */
__SUN_EXTERN_INLINE int nulldev(void);

EXPORT_SYMBOL(nulldev);		/* strconf.h */
__SUN_EXTERN_INLINE int nochpoll(void);

EXPORT_SYMBOL(nochpoll);	/* strconf.h */
__SUN_EXTERN_INLINE int ddi_prop_op(void);

EXPORT_SYMBOL(ddi_prop_op);

struct mod_ops mod_strmops = { MODREV_1, 0, };

EXPORT_SYMBOL(mod_strmops);	/* strconf.h */

int
mod_install(struct modlinkage *ml)
{
	/* FIXME: this is our register function, write it! */
	if (ml->ml_rev != MODREV_1)
		return (DDI_FAILURE);
	if (ml->ml_linkage[1] != NULL)
		return (DDI_FAILURE);
	if (ml->ml_linkage[0] == NULL)
		return (DDI_FAILURE);
	if (ml->ml_linkage[0] == (void *) &mod_strmops) {
		struct modlstrmod *mod = ml->ml_linkage[0];
		int err;

#ifdef LIS
		struct streamtab *st = mod->strmod_fmodsw->f_str;

		if ((err = lis_register_strmod(st, st->st_rdinit->qi_minfo->mi_idname)) >= 0) {
			/* for LiS we save the modid in the mi_idnum */
			st->st_rdinit->qi_minfo->mi_idnum = err;
			return (DDI_SUCCESS);
		}
		return (-err);
#endif
#ifdef LFS
		/* registering a module */
		struct fmodsw *fmod;

		/* FIXME: write this */
		if ((fmod = kmem_zalloc(sizeof(*fmod), KM_NOSLEEP))) {
			struct streamtab *st = mod->strmod_fmodsw->f_str;

			fmod->f_name = st->st_rdinit->qi_minfo->mi_idname;
			fmod->f_str = st;
			fmod->f_flag = mod->strmod_fmodsw->f_flag;
			fmod->f_kmod = NULL;
			atomic_set(&fmod->f_count, 0);
			/* FIXME: Solaris uses flags to indicate syncq levels, dig 'em out and
			   populate syncq level */
			if ((err = register_strmod(fmod)) < 0) {
				kmem_free(fmod, sizeof(*fmod));
				return (-err);
			}
			return (DDI_SUCCESS);
		}
#endif
	} else {
		struct modldrv *drv = ml->ml_linkage[0];
		int err;

#ifdef LIS
		struct streamtab *st = drv->drv_dev_ops->devo_cb_ops->cb_str;

		if ((err =
		     lis_register_strdev(0, st, 255, st->st_rdinit->qi_minfo->mi_idname)) >= 0) {
			/* for LiS we save the major in the mi_idnum */
			st->st_rdinit->qi_minfo->mi_idnum = err;
			return (DDI_SUCCESS);
		}
		return (-err);
#endif
#ifdef LFS
		/* registering a driver */
		struct cdevsw *cdev;

		if ((cdev = kmem_zalloc(sizeof(*cdev), KM_NOSLEEP))) {
			/* call device attach function to get node information */
			struct streamtab *st = drv->drv_dev_ops->devo_cb_ops->cb_str;

			cdev->d_name = st->st_rdinit->qi_minfo->mi_idname;
			cdev->d_str = st;
			cdev->d_flag = drv->drv_dev_ops->devo_cb_ops->cb_flag;	/* convert these? */
			cdev->d_kmod = NULL;
			atomic_set(&cdev->d_count, 0);
			INIT_LIST_HEAD(&cdev->d_majors);
			INIT_LIST_HEAD(&cdev->d_minors);
			INIT_LIST_HEAD(&cdev->d_apush);
			INIT_LIST_HEAD(&cdev->d_stlist);
			/* FIXME: Solaris uses flags to indicate syncq levels, dig 'em out and
			   populate syncq level */
			if ((err = register_strdev(cdev, 0)) < 0)
				kmem_free(cdev, sizeof(*cdev));
			return (-err);
		}
#endif
	}
	return (ENOMEM);
}

EXPORT_SYMBOL(mod_install);	/* strconf.h */

int
mod_remove(struct modlinkage *ml)
{
	/* FIXME: this is our unregister function, write it! */
	if (ml->ml_rev != MODREV_1)
		return (DDI_FAILURE);
	if (ml->ml_linkage[1] != NULL)
		return (DDI_FAILURE);
	if (ml->ml_linkage[0] == NULL)
		return (DDI_FAILURE);
	if (ml->ml_linkage[0] == (void *) &mod_strmops) {
		/* was a module */
		struct modlstrmod *mod = ml->ml_linkage[0];
		int err;

#ifdef LIS
		struct streamtab *st = mod->strmod_fmodsw->f_str;

		if ((err = lis_unregister_strmod(st)) >= 0)
			return (DDI_SUCCESS);
		return (-err);
#endif
#ifdef LFS
		struct fmodsw *fmod;
		struct streamtab *st = mod->strmod_fmodsw->f_str;

		if ((fmod = fmod_str(st))) {
			printd(("%s: %s: streams module\n", __FUNCTION__, fmod->f_name));
			err = unregister_strmod(fmod);
			printd(("%s: %s: putting module\n", __FUNCTION__, fmod->f_name));
			fmod_put(fmod);
			if (err >= 0)
				kmem_free(fmod, sizeof(*fmod));
			return (-err);
		}
#endif
	} else {
		/* was a driver */
		struct modldrv *drv = ml->ml_linkage[0];
		int err;

#ifdef LIS
		struct streamtab *st = drv->drv_dev_ops->devo_cb_ops->cb_str;

		if ((err = lis_unregister_strdev(st->st_rdinit->qi_minfo->mi_idnum)) >= 0)
			return (DDI_SUCCESS);
		return (-err);
#endif
#ifdef LFS
		struct cdevsw *cdev;
		struct streamtab *st = drv->drv_dev_ops->devo_cb_ops->cb_str;

		if ((cdev = cdev_str(st))) {
			if ((err = unregister_strdev(cdev, 0)) == 0)
				kmem_free(cdev, sizeof(*cdev));
			return (-err);
		}
#endif
	}
	return (ENXIO);
}

EXPORT_SYMBOL(mod_remove);	/* strconf.h */

int
mod_info(struct modlinkage *ml, struct modinfo *mi)
{
	return (0);		/* never called */
}

EXPORT_SYMBOL(mod_info);	/* strconf.h */

#ifdef CONFIG_STREAMS_COMPAT_SUN_MODULE
static
#endif
int __init
suncomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_SUN_MODULE
	printk(KERN_INFO SUNCOMP_BANNER);
#else
	printk(KERN_INFO SUNCOMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_SUN_MODULE
static
#endif
void __exit
suncomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_SUN_MODULE
module_init(suncomp_init);
module_exit(suncomp_exit);
#endif
