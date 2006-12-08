/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.23 2006/12/08 05:08:18 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/12/08 05:08:18 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 0.9.2.23  2006/12/08 05:08:18  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.22  2006/11/03 10:39:23  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_SUN_DDI_H__
#define __SYS_SUN_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.23 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_SUNDDI_H__
#warning "Do not include sys/sun/ddi.h directly, include sys/sunddi.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __SUN_EXTERN_INLINE
#define __SUN_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __SUN_EXTERN_INLINE */

#ifndef _SUN_SOURCE
#warning "_SUN_SOURCE not defined but SUN ddi.h included"
#endif

#define PERIM_INNER	1	/* Solaris used with qwriter() */
#define PERIM_OUTER	2	/* Solaris used with qwriter() */

#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)

#ifdef LFS
__SUN_EXTERN_INLINE cred_t *
ddi_get_cred(void)
{
	return (current_creds);
}
#endif
__SUN_EXTERN_INLINE clock_t
ddi_get_lbolt(void)
{
	return (jiffies);
}
__SUN_EXTERN_INLINE pid_t
ddi_get_pid(void)
{
	return (current->pid);
}

__SUN_EXTERN_INLINE time_t
ddi_get_time(void)
{
	struct timeval tv;

	do_gettimeofday(&tv);
	return (tv.tv_sec);
}
__SUN_EXTERN_INLINE unsigned short
ddi_getiminor(dev_t dev)
{
#ifdef HAVE_KFUNC_TO_KDEV_T
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

__SUN_EXTERN_INLINE void *
ddi_umem_alloc(size_t size, int flag, ddi_umem_cookie_t * cookiep)
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
__SUN_EXTERN_INLINE void *
ddi_umem_free(ddi_umem_cookie_t * cookiep)
{
	if (cookiep)
		free_pages((unsigned long) cookiep->umem, cookiep->order);
	return (NULL);
}

#if 0
int ddi_create_minor_node(dev_info_t * dip, char *name, int type, minor_t minor, char *node,
			  int flag);
void ddi_remove_minor_node(dev_info_t * dip, char *name);
major_t ddi_driver_major(dev_info_t * dip);
const char *ddi_driver_name(dev_info_t * dip);
int ddi_get_instance(dev_info_t * dip);
int ddi_get_soft_state(int yyy);
int ddi_removing_power(int yyy);
int ddi_soft_state(int yyy);
int ddi_soft_state_fini(int yyy);
int ddi_soft_state_free(int yyy);
int ddi_soft_state_init(int yyy);
int ddi_soft_state_zalloc(int yyy);
#endif

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
extern int ddi_get_kt_did(void);
extern int ddi_get_lbolt(void);
extern int ddi_get_name(void);
extern int ddi_get_parent(void);
extern int ddi_get_pid(void);
extern int ddi_get_soft_iblock_cookie(void);
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
extern int ddi_remove_softintr(void);
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
extern int ddi_trigger_softintr(void);
extern int ddi_umem_alloc(void);
extern int ddi_umem_free(void);
extern int ddi_umem_lock(void);
extern int ddi_umem_unlock(void);
extern int ddi_unmap_regs(void);
#endif

#else
#ifdef _SUN_SOURCE
#warning "_SUN_SOURCE defined but not CONFIG_STREAMS_COMPAT_SUN"
#endif
#endif				/* CONFIG_STREAMS_COMPAT_SUN */

#endif				/* __SYS_SUN_DDI_H__ */
