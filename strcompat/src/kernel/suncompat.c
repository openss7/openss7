/*****************************************************************************

 @(#) suncompat.c,v (1.1.2.7) 2003/10/28 08:00:05

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified 2003/10/28 08:00:05 by brian

 *****************************************************************************/

#ident "@(#) suncompat.c,v (1.1.2.7) 2003/10/28 08:00:05"

static char const ident[] = "suncompat.c,v (1.1.2.7) 2003/10/28 08:00:05";

#include <linux/config.h>
#include <linux/module.h>	/* for MOD_DEC_USE_COUNT etc */

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

#define __SUN_EXTERN_INLINE inline

#include <linux/kernel.h>	/* for vsprintf and friends */
#include <linux/vmalloc.h>	/* for vmalloc */
#ifdef CONFIG_PCI
#include <linux/pci.h>		/* for many pci functions */
#include <asm/pci.h>		/* for many pci functions */
#endif
#include <linux/ioport.h>	/* for check_region and friends */
#include <asm/uaccess.h>	/* for verify_area and friends */
#include <linux/timer.h>	/* for del_timer and friends */
#include <asm/semaphore.h>	/* for semaphores */
#include <linux/sched.h>	/* for kill_proc, jiffies and friends */
#include <linux/kmod.h>		/* for request_module and friends */
#include <linux/threads.h>	/* for NR_CPUS */
#include <asm/dma.h>		/* for request_dma and friends */
#include <linux/fs.h>		/* for filesystem related stuff */
#include <linux/time.h>		/* for do_gettimeofday and friends */
#include <asm/io.h>		/* for virt_to_page and friends */
#include <linux/slab.h>		/* for kmalloc and friends */
#include <linux/interrupt.h>	/* for in_interrupt() */
#include <asm/irq.h>		/* for disable_irq */
#include <asm/system.h>		/* for sti, cli */
#include <asm/delay.h>		/* for udelay */
#include <linux/spinlock.h>	/* for spinlock functions */
#include <asm/atomic.h>		/* for atomic functions */
#include <linux/poll.h>		/* for poll_table */
#include <linux/string.h>

#define _SUN_SOURCE
#include <linux/kmem.h>		/* for SVR4 style kmalloc functions */
#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strconf.h>
#include <linux/strsubr.h>
#include <linux/ddi.h>
#include <linux/sunddi.h>

#include "strdebug.h"
#include "strsched.h"
#include "strutil.h"
#include "strhead.h"
#include "strsad.h"

#define SUNCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SUNCOMP_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define SUNCOMP_REVISION	"LfS suncompat.c,v (1.1.2.7) 2003/10/28 08:00:05"
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

MODULE_AUTHOR(SUNCOMP_CONTACT);
MODULE_DESCRIPTION(SUNCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SUNCOMP_DEVICE);
MODULE_LICENSE(SUNCOMP_LICENSE);

/* 
 *  QWAIT
 *  -------------------------------------------------------------------------
 */
void qwait(queue_t *rq)
{
	struct queinfo *qu = (typeof(qu)) rq;
	assert(!in_interrupt());
	ensure(!test_bit(QHLIST_BIT, &rq->q_flag), qprocson(rq));
	/* Need to lock the queue and test before scheduling */
	sleep_on(&qu->qu_qwait);
}

/* 
 *  QWAIT_SIG
 *  -------------------------------------------------------------------------
 */
int qwait_sig(queue_t *rq)
{
	struct queinfo *qu = (typeof(qu)) rq;
	assert(!in_interrupt());
	ensure(!test_bit(QHLIST_BIT, &rq->q_flag), qprocson(rq));
	/* Need to lock the queue and test before scheduling */
	interruptible_sleep_on(&qu->qu_qwait);
	return (!signal_pending(current));
}

__SUN_EXTERN_INLINE unsigned char queclass(mblk_t *mp);
__SUN_EXTERN_INLINE cred_t *ddi_get_cred(void);
__SUN_EXTERN_INLINE clock_t ddi_get_lbolt(void);
__SUN_EXTERN_INLINE pid_t ddi_get_pid(void);
__SUN_EXTERN_INLINE time_t ddi_get_time(void);
__SUN_EXTERN_INLINE unsigned short ddi_getiminor(dev_t dev);
__SUN_EXTERN_INLINE void *ddi_umem_alloc(size_t size, int flag, ddi_umem_cookie_t * cookiep);
__SUN_EXTERN_INLINE void *ddi_umem_free(ddi_umem_cookie_t * cookiep);

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
 *  Configuration
 */

struct mod_ops mod_strmops = { MODREV_1, 0, };

__SUN_EXTERN_INLINE int nodev();
__SUN_EXTERN_INLINE int nulldev();
__SUN_EXTERN_INLINE int nochpoll();
__SUN_EXTERN_INLINE int ddi_prop_op();

int mod_install(struct modlinkage *ml)
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
		/* registering a module */
		struct fmodsw *fmod;
		int err;
		(void) mod;
		(void) fmod;
		(void) err;
		/* FIXME: write this */
	} else {
		struct modldrv *drv = ml->ml_linkage[0];
		/* registering a driver */
		struct cdevsw *cdev;
		int err;
		if ((cdev = kmem_zalloc(sizeof(*cdev), KM_NOSLEEP))) {
			/* call device attach function to get node information */
			cdev->d_name =
			    drv->drv_dev_ops->devo_cb_ops->cb_str->st_rdinit->qi_minfo->mi_idname;
			cdev->d_str = drv->drv_dev_ops->devo_cb_ops->cb_str;
			cdev->d_flag = drv->drv_dev_ops->devo_cb_ops->cb_flag;	/* convert these? */
			cdev->d_kmod = NULL;
			atomic_set(&cdev->d_count, 0);
			INIT_LIST_HEAD(&cdev->d_apush);
			if ((err = register_strdev_major(0, cdev)) < 0) {
				kmem_free(cdev, sizeof(*cdev));
				return (-err);
			}
		}
		return (ENOMEM);
	}
	return (ENXIO);
}
int mod_remove(struct modlinkage *ml)
{
	/* FIXME: this is our unregister function, write it! */
	return (ENXIO);
}
int mod_info(struct modlinkage *ml, struct modinfo *mi)
{
	return (0);		/* never called */
}

static int __init suncomp_init(void)
{
#ifdef MODULE
	printk(KERN_INFO SUNCOMP_BANNER);
#else
	printk(KERN_INFO SUNCOMP_SPLASH);
#endif
	return (0);
}
static void __exit suncomp_exit(void)
{
	return;
}

module_init(suncomp_init);
module_exit(suncomp_exit);
