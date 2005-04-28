/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.4 2005/04/28 11:37:41 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/04/28 11:37:41 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_SUNDDI_H__
#define __SYS_SUNDDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/04/28 11:37:41 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __SUN_EXTERN_INLINE
#define __SUN_EXTERN_INLINE extern __inline__
#endif				/* __SUN_EXTERN_INLINE */

#include <sys/strconf.h>

#ifndef _SUN_SOURCE
#warning "_SUN_SOURCE not defined but $RCSfile: ddi.h,v $ included"
#endif

#define PERIM_INNER	1	/* Solaris used with qwriter() */
#define PERIM_OUTER	2	/* Solaris used with qwriter() */

#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
extern void qprocsoff(queue_t *q);
extern void qprocson(queue_t *q);

__SUN_EXTERN_INLINE void freezestr_SUN(queue_t *q)
{
	freezestr(q);
}

#undef freezestr
#define freezestr freezestr_SUN

__SUN_EXTERN_INLINE void unfreezestr_SUN(queue_t *q)
{
	unfreezestr(q, -1UL);
}

#undef unfreezestr
#define unfreezestr unfreezestr_SUN

/**
 *  qbufcall:	- schedule a buffer callout
 *  @q:		queue used for synchronization
 *  @size:	the number of bytes of data buffer needed
 *  @priority:	the priority of the buffer allocation (ignored)
 *  @function:	the callback function when bytes and headers are available
 *  @arg:	a client argument to pass to the callback function
 */
__SUN_EXTERN_INLINE bufcall_id_t qbufcall(queue_t *q, size_t size, int priority,
					  void (*function) (void *), void *arg)
{
	extern bcid_t __bufcall(queue_t *q, unsigned size, int priority, void (*function) (long), long arg);
	// queue_t *rq = RD(q);
	// assert(!test_bit(QHLIST_BIT, &rq->q_flag));
	return __bufcall(q, size, priority, (void (*)(long)) function, (long) arg);
}

__SUN_EXTERN_INLINE timeout_id_t qtimeout(queue_t *q, void (*timo_fcn) (void *), void *arg,
					  long ticks)
{
	extern toid_t __timeout(queue_t *q, timo_fcn_t *timo_fcn, caddr_t arg, long ticks,
				unsigned long pl, int cpu);
	// queue_t *rq = RD(q);
	// assert(!test_bit(QHLIST_BIT, &rq->q_flag));
	return __timeout(q, (timo_fcn_t *) timo_fcn, (caddr_t) arg, ticks, 0, smp_processor_id());
}

/**
 *  qunbufcall: - cancel a buffer callout
 *  @q:		queue used for synchronization
 *  @bcid:	buffer call id returned by qbufcall()
 *  Notices:	Don't ever call this function with an expired bufcall id.
 */
__SUN_EXTERN_INLINE void qunbufcall(queue_t *q, bufcall_id_t bcid)
{
	unbufcall(bcid);
}

__SUN_EXTERN_INLINE clock_t quntimeout(queue_t *q, timeout_id_t toid)
{
	return untimeout(toid);
}

__SUN_EXTERN_INLINE unsigned char queclass(mblk_t *mp)
{
	return (mp->b_datap->db_type < QPCTL ? QNORM : QPCTL);
}

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
__SUN_EXTERN_INLINE void qwriter(queue_t *qp, mblk_t *mp, void (*func) (queue_t *qp, mblk_t *mp),
				 int perimeter)
{
	extern int defer_func(void (*func) (void *, mblk_t *), queue_t *q, mblk_t *mp, void *arg,
			      int perim, int type);
	if (defer_func((void (*)(void *, mblk_t *)) func, qp, mp, qp, perimeter, SE_WRITER) == 0)
		return;
	// never();
}

__SUN_EXTERN_INLINE mblk_t *mkiocb(unsigned int command)
{
	mblk_t *mp;
	union ioctypes *iocp;
	static atomic_t ioc_id = ATOMIC(0);
	if ((mp = allocb(sizeof(*iocp), BPRI_MED))) {
		mp->b_datab->db_type = M_IOCTL;
		mp->b_wptr += sizeof(*iocp);
		mp->b_cont = NULL;
		iocp = (typeof(iocp)) mp->b_rptr;
		iocp->iocblk.ioc_cmd = command;
		iocp->iocblk.ioc_id = atomic_inc(&ioc_id);	/* FIXME: need better unique id */
		iocp->iocblk.ioc_cr = NULL;	/* FIXME: need maximum credentials pointer */
		iocp->iocblk.ioc_count = 0;
		iocp->iocblk.ioc_rval = 0;
		iocp->iocblk.ioc_error = 0;
		iocp->iocblk.ioc_flags = IOC_NATIVE;
	}
	return (mp);
}

__SUN_EXTERN_INLINE cred_t *ddi_get_cred(void)
{
	return (current_creds);
}
__SUN_EXTERN_INLINE clock_t ddi_get_lbolt(void)
{
	return (jiffies);
}
__SUN_EXTERN_INLINE pid_t ddi_get_pid(void)
{
	return (current->pid);
}
__SUN_EXTERN_INLINE time_t ddi_get_time(void)
{
	struct timeval tv;
	do_gettimeofday(&tv);
	return (tv.tv_sec);
}
__SUN_EXTERN_INLINE unsigned short ddi_getiminor(dev_t dev)
{
#if HAVE_KFUNC_TO_KDEV_T
	kdev_t kdev;
	kdev = to_kdev_t(dev);
	return MINOR(kdev);
#else
	return MINOR(dev);
#endif
}

#define DDI_SUCCESS	    0
#define DDI_FAILURE	    -1

#define DDI_UMEM_SLEEP	    0x00
#define DDI_UMEM_NOSLEEP    0x01
#define DDI_UMEM_PAGEABLE   0x02

typedef struct ddi_umem_cookie {
	unsigned int gfp;
	unsigned int order;
	void *umem;
} ddi_umem_cookie_t;

__SUN_EXTERN_INLINE void *ddi_umem_alloc(size_t size, int flag, ddi_umem_cookie_t * cookiep)
{
	void *umem;
	unsigned int order = 1, gfp_mask;
	if (!cookiep)
		return (NULL);
	while ((size >>= 1) != 0)
		order <<= 1;
	gfp_mask = GFP_USER & ~(flag & DDI_UMEM_NOSLEEP ? __GFP_WAIT : 0);
	if ((umem = (void *) __get_free_pages(gfp_mask, order))) {
		cookiep->gfp = gfp_mask;
		cookiep->order = order;
		cookiep->umem = umem;
	}
	return (umem);
}
__SUN_EXTERN_INLINE void *ddi_umem_free(ddi_umem_cookie_t * cookiep)
{
	if (cookiep)
		free_pages((unsigned long) cookiep->umem, cookiep->order);
	return (NULL);
}

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

#elif defined(_SUN_SOURCE)
#warning "_SUN_SOURCE defined but not CONFIG_STREAMS_COMPAT_SUN"
#endif				/* CONFIG_STREAMS_COMPAT_SUN */

#endif				/* __SYS_SUNDDI_H__ */
