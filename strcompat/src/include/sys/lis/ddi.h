/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.29 2008-04-28 16:47:09 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 Last Modified $Date: 2008-04-28 16:47:09 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 0.9.2.29  2008-04-28 16:47:09  brian
 - updates for release

 Revision 0.9.2.28  2007/08/12 15:51:06  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.27  2007/03/25 19:00:59  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.26  2006/12/08 05:08:11  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.25  2006/11/03 10:39:19  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_LIS_DDI_H__
#define __SYS_LIS_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.29 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __depr
#if __GNUC__ >= 3
#define __depr __attribute__ ((__deprecated__))
#else
#define __depr
#endif
#endif

#undef DEPRECATED
#define DEPRECATED(__x) STREAMSCALL(__x) __depr

#undef _RP
#define _RP streamscall __depr

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __LIS_EXTERN_INLINE
#define __LIS_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __LIS_EXTERN_INLINE */

#ifndef __LIS_EXTERN
#define __LIS_EXTERN extern streamscall
#endif				/* __AIX_EXTERN_INLINE */

#ifndef _LIS_SOURCE
#warning "_LIS_SOURCE not defined but LIS ddi.h included"
#define _LIS_SOURCE 1
#endif

#include <linux/poll.h>
#include <linux/interrupt.h>
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif

typedef volatile long lis_atomic_t;

typedef struct lis_semaphore {
	void *taskp;			/* owners task pointer */
	char *downer_file;		/* most recent "down" operation */
	int downer_line;
	int downer_cntr;		/* seq cntr */
	char *owner_file;		/* most recent successful "down" */
	int owner_line;
	int owner_cntr;			/* seq cntr */
	char *upper_file;		/* most recent "up" operation */
	int upper_line;
	int upper_cntr;			/* seq cntr */
	int allocated;			/* allocated structure */
	long sem_mem[50];
} lis_semaphore_t;

typedef volatile struct lis_spin_lock {
	lis_atomic_t nest;
	void *taskp;
	char *name;
	char *spinner_file;
	int spinner_line;
	int spinner_cntr;
	char *owner_file;
	int owner_line;
	int owner_cntr;
	char *unlocker_file;
	int unlocker_line;
	int unlocker_cntr;
	int allocated;
	long spin_lock_mem[7];
} lis_spin_lock_t;

typedef volatile struct lis_rw_lock {
	lis_atomic_t nest;
	void *taskp;
	char *name;
	char *spinner_file;
	int spinner_line;
	int spinner_cntr;
	char *owner_file;
	int owner_line;
	int owner_cntr;
	char *unlocker_file;
	int unlocker_line;
	int unlocker_cntr;
	int allocated;
	long rw_lock_mem[7];
} lis_rw_lock_t;

typedef struct lis_pci_dev {
	unsigned bus;
	unsigned dev_fcn;
	unsigned vendor;
	unsigned device;
	unsigned class;
	unsigned hdr_type;
	unsigned irq;
	unsigned long mem_addrs[12];
	void *user_ptr;
	void *kern_ptr;
	struct lis_pci_dev *next;
} lis_pci_dev_t;

#define ALLOC(__s) kmem_alloc((__s), KM_NOSLEEP)
#define FREE(__p) kmem_free((__p), sizeof(*(__p)))
#define SPLSTR(__pl) ({ (__pl) = splstr(); (void)0; })
#define SPLX(__pl) splx(__pl)

extern void *_RP lis__kfree(void *ptr);
extern void *_RP lis__kmalloc(int nbytes, int class, int use_cache);

#if 0
extern void _RP lis_add_timer(struct timer_list *timer);
#endif
extern void *_RP lis_alloc_atomic_fcn(int nbytes, char *file, int line);
extern void *_RP lis_alloc_dma_fcn(int nbytes, char *file, int line);
extern void *_RP lis_alloc_kernel_fcn(int nbytes, char *file, int line);
extern void _RP lis_assert_fail(const char *expr, const char *objname, const char *file,
				unsigned int line);
extern void _RP lis_atomic_add(lis_atomic_t *atomic_addr, int amt);
extern void _RP lis_atomic_dec(lis_atomic_t *atomic_addr);
extern int _RP lis_atomic_dec_and_test(lis_atomic_t *atomic_addr);
extern void _RP lis_atomic_inc(lis_atomic_t *atomic_addr);
extern int _RP lis_atomic_read(lis_atomic_t *atomic_addr);
extern void _RP lis_atomic_set(lis_atomic_t *atomic_addr, int valu);
extern void _RP lis_atomic_sub(lis_atomic_t *atomic_addr, int amt);

#if 0
extern void _RP lis_bprintf(char *fmt, ...);
extern int _RP lis_can_unload(void);
extern int _RP lis_check_guard(void *ptr, char *msg);
extern int _RP lis_check_mem(void);
extern int _RP lis_check_region(unsigned int from, unsigned int extent);
extern int _RP lis_check_umem(struct file *fp, int rd_wr_fcn, const void *usr_addr, int lgth);
#endif
extern void _RP lis_cmn_err(int err_lvl, char *fmt, ...);
extern char lis_date[] __depr;
extern unsigned long lis_debug_mask __depr;
extern unsigned long lis_debug_mask2 __depr;

#if 0
extern void _RP lis_dec_mod_cnt_fcn(const char *file, int line);
extern int _RP lis_del_timer(struct timer_list *timer);
#endif
extern void _RP lis_disable_irq(unsigned int irq);

#if 0
extern void _RP lis_dobufcall(int cpu_id);
extern int _RP lis_doclose(struct inode *i, struct file *f, struct stdata *head, cred_t *creds);
#endif
extern int _RP lis_down_fcn(lis_semaphore_t *lsem, char *file, int line);
extern unsigned long _RP lis_dsecs(void);

#if 0
extern void _RP lis_enable_intr(struct streamtab *strtab, int major, const char *name);
#endif
extern void _RP lis_enable_irq(unsigned int irq);
extern void _RP lis_free(void *ptr, char *file_name, int line_nr);
extern void _RP lis_free_dma(unsigned int dma_nr);
extern void _RP lis_free_irq(unsigned int irq, void *dev_id);
extern void *_RP lis_free_mem_fcn(void *mem_area, char *file, int line);
extern void *_RP lis_free_pages_fcn(void *ptr, char *file, int line);
extern void *_RP lis_get_free_pages_atomic_fcn(int nbytes, char *file, int line);
extern void *_RP lis_get_free_pages_fcn(int nbytes, int class, char *file, int line);
extern void *_RP lis_get_free_pages_kernel_fcn(int nbytes, char *file, int line);
extern int _RP lis_getint(unsigned char **p);
extern void _RP lis_gettimeofday(struct timeval *tv);
extern unsigned long _RP lis_hitime(void);

#if 0
extern lis_atomic_t lis_in_syscall __depr;
extern void _RP lis_inc_mod_cnt_fcn(const char *file, int line);
extern void _RP lis_init_bufcall(void);
#endif
extern void _RP lis_interruptible_sleep_on(wait_queue_head_t *wq);
extern void *_RP lis_ioremap(unsigned long offset, unsigned long size);
extern void *_RP lis_ioremap_nocache(unsigned long offset, unsigned long size);
extern void _RP lis_iounmap(void *ptr);
extern unsigned long _RP lis_jiffies(void);
extern int _RP lis_kernel_down(struct semaphore *sem);
extern void _RP lis_kernel_up(struct semaphore *sem);
extern char lis_kernel_version[] __depr;

#if 0
extern void _RP lis_kfree(const void *ptr);
extern int _RP lis_kill_pg(int pgrp, int sig, int priv);
extern int _RP lis_kill_proc(int pid, int sig, int priv);
extern void *_RP lis_kmalloc(size_t nbytes, int type);
extern int _RP lis_loadable_load(const char *name);
extern const char *_RP lis_maj_min_name(struct stdata *head);
#endif
extern int lis_major __depr;
extern void *_RP lis_malloc(int nbytes, int class, int use_cache, char *file_name, int line_nr);

#if 0
extern void _RP lis_mark_mem(void *ptr, const char *file_name, int line_nr);
extern long lis_max_mem __depr;
#endif
extern long _RP lis_milli_to_ticks(long milli_sec);
extern int _RP lis_mknod(char *name, int mode, dev_t dev);
extern int _RP lis_mount(char *dev_name, char *dir_name, char *fstype, unsigned long rwflag,
			 void *data);
extern unsigned long _RP lis_msecs(void);
extern const char *_RP lis_msg_type_name(mblk_t *mp);
extern int lis_num_cpus __depr;
extern void _RP lis_osif_cli(void);
extern void _RP lis_osif_do_gettimeofday(struct timeval *tp);

#ifdef HAVE_TIMESPEC_DO_SETTIMEOFDAY
extern int _RP lis_osif_do_settimeofday(struct timespec *tp);
#else
extern void _RP lis_osif_do_settimeofday(struct timeval *tp);
#endif
extern void _RP lis_osif_sti(void);
extern int _RP lis_own_spl(void);
extern void *_RP lis_phys_to_virt(unsigned long addr);
extern void _RP lis_print_block(void *ptr);
extern void _RP lis_print_data(mblk_t *mp, int opt, int cont);
extern void _RP lis_print_mem(void);
extern void _RP lis_print_msg(mblk_t *mp, const char *prefix, int opt);
extern void _RP lis_print_queue(queue_t *q);

#if 0
extern void _RP lis_print_spl_track(void);
#endif
extern int _RP lis_printk(const char *fmt, ...);
extern void _RP lis_putbyte(unsigned char **p, unsigned char byte);
extern const char *_RP lis_queue_name(queue_t *q);
extern void _RP lis_release_region(unsigned int from, unsigned int extent);
extern int _RP lis_request_dma(unsigned int dma_nr, const char *device_id);

#ifdef HAVE_KTYPE_IRQ_HANDLER_T
extern int _RP lis_request_irq(unsigned int irq, irq_handler_t handler,
			       unsigned long flags, const char *device, void *dev_id);
#else
#ifdef HAVE_KTYPE_IRQRETURN_T
extern int _RP lis_request_irq(unsigned int irq,
			       irqreturn_t(*handler) (int, void *, struct pt_regs *),
			       unsigned long flags, const char *device, void *dev_id);
#else
extern int _RP lis_request_irq(unsigned int irq, void (*handler) (int, void *, struct pt_regs *),
			       unsigned long flags, const char *device, void *dev_id);
#endif
#endif
extern void _RP lis_request_region(unsigned int from, unsigned int extent, const char *name);
extern lis_rw_lock_t *_RP lis_rw_lock_alloc_fcn(const char *name, char *file, int line);
extern lis_rw_lock_t *_RP lis_rw_lock_free_fcn(lis_rw_lock_t *lock, const char *name, char *file,
					       int line);
extern void _RP lis_rw_lock_init_fcn(lis_rw_lock_t *lock, const char *name, char *file, int line);
extern void _RP lis_rw_read_lock_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void _RP lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void _RP lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line);
extern void _RP lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void _RP lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void _RP lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flagp, char *file,
						  int line);
extern void _RP lis_rw_write_lock_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void _RP lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void _RP lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flagp, char *file,
					      int line);
extern void _RP lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void _RP lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void _RP lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flagp, char *file,
						   int line);
extern unsigned long _RP lis_secs(void);
extern lis_semaphore_t *_RP lis_sem_alloc(int count);
extern lis_semaphore_t *_RP lis_sem_destroy(lis_semaphore_t *lsem);
extern void _RP lis_sem_init(lis_semaphore_t *lsem, int count);
extern void _RP lis_sleep_on(wait_queue_head_t *wq);
extern int _RP lis_spin_is_locked_fcn(lis_spin_lock_t *lock, char *file, int line);
extern lis_spin_lock_t *_RP lis_spin_lock_alloc_fcn(const char *name, char *file, int line);
extern void _RP lis_spin_lock_fcn(lis_spin_lock_t *lock, char *file, int line);
extern lis_spin_lock_t *_RP lis_spin_lock_free_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void _RP lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, char *file,
				       int line);
extern void _RP lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void _RP lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock, int *flagp, char *file, int line);
extern int _RP lis_spin_trylock_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void _RP lis_spin_unlock_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void _RP lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void _RP lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock, int *flagp, char *file,
					       int line);
extern void _RP lis_spl0_fcn(char *file, int line);
extern int _RP lis_splstr_fcn(char *file, int line);
extern void _RP lis_splx_fcn(int x, char *file, int line);
extern int _RP lis_sprintf(char *bfr, const char *fmt, ...);
extern const char *_RP lis_strm_name(struct stdata *head);
extern const char *_RP lis_strm_name_from_queue(queue_t *q);
extern char *lis_stropts_file __depr;
extern pid_t _RP lis_thread_start(int (*fcn) (void *), void *arg, const char *name);
extern int _RP lis_thread_stop(pid_t pid);
extern void _RP lis_udelay(long micro_secs);

#if 0
extern int _RP lis_umount2(char *path, int flags);
#endif
extern int _RP lis_unlink(char *name);
extern void _RP lis_up_fcn(lis_semaphore_t *lsem, char *file, int line);
extern unsigned long _RP lis_usecs(void);
extern unsigned _RP lis_usectohz(unsigned usec);
extern char lis_version[] __depr;
extern void _RP lis_vfree(void *ptr);
extern unsigned long _RP lis_virt_to_phys(volatile void *addr);
extern void *_RP lis_vmalloc(unsigned long size);
extern void *_RP lis_vremap(unsigned long offset, unsigned long size);
extern int _RP lis_vsprintf(char *bfr, const char *fmt, va_list args);
extern void _RP lis_wake_up(wait_queue_head_t *wq);
extern void _RP lis_wake_up_interruptible(wait_queue_head_t *wq);
extern void *_RP lis_zmalloc(int nbytes, int class, char *file_name, int line_nr);

#ifdef CONFIG_PCI
#include <linux/pci.h>

extern void *_RP lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
					       dma_addr_t *dma_handle);
extern int _RP lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask);

#ifdef HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE
extern void _RP lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev, dma64_addr_t dma_addr,
						 size_t len, int direction);
#endif
extern void _RP lis_osif_pci_dac_dma_sync_single_for_cpu(struct pci_dev *pdev,
							 dma64_addr_t dma_addr, size_t len,
							 int direction);
extern void _RP lis_osif_pci_dac_dma_sync_single_for_device(struct pci_dev
							    *pdev, dma64_addr_t
							    dma_addr, size_t len, int direction);
extern unsigned long _RP lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev,
							dma64_addr_t dma_addr);
extern struct page *_RP lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev, dma64_addr_t dma_addr);
extern dma64_addr_t _RP lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev, struct page *page,
						     unsigned long offset, int direction);
extern int _RP lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask);
extern void _RP lis_osif_pci_disable_device(struct pci_dev *dev);
extern int _RP lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask);
extern void _RP lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nelems,
					 int direction);
extern void _RP lis_osif_pci_dma_sync_single(struct pci_dev *hwdev, dma_addr_t dma_handle,
					     size_t size, int direction);
extern int _RP lis_osif_pci_enable_device(struct pci_dev *dev);
extern struct pci_dev *_RP lis_osif_pci_find_class(unsigned int class, struct pci_dev *from);
extern struct pci_dev *_RP lis_osif_pci_find_device(unsigned int vendor, unsigned int device,
						    struct pci_dev *from);
extern struct pci_dev *_RP lis_osif_pci_find_slot(unsigned int bus, unsigned int devfn);
extern void _RP lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size, void *vaddr,
					     dma_addr_t dma_handle);
extern dma_addr_t _RP lis_osif_pci_map_page(struct pci_dev *hwdev, struct page *page,
					    unsigned long offset, size_t size, int direction);
extern int _RP lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
				   int direction);
extern dma_addr_t _RP lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size,
					      int direction);
extern int _RP lis_osif_pci_module_init(void *p);
extern int _RP lis_osif_pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val);
extern int _RP lis_osif_pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val);
extern int _RP lis_osif_pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val);
extern int _RP lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask);
extern void _RP lis_osif_pci_set_master(struct pci_dev *dev);
extern void _RP lis_osif_pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address, size_t size,
					int direction);
extern void _RP lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
				      int direction);
extern void _RP lis_osif_pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr, size_t size,
					  int direction);
extern void _RP lis_osif_pci_unregister_driver(struct pci_driver *p);
extern int _RP lis_osif_pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val);
extern int _RP lis_osif_pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val);
extern int _RP lis_osif_pci_write_config_word(struct pci_dev *dev, u8 where, u16 val);
extern dma_addr_t _RP lis_osif_sg_dma_address(struct scatterlist *sg);
extern size_t _RP lis_osif_sg_dma_len(struct scatterlist *sg);

#if 0
extern void _RP lis_pci_cleanup(void);
#endif
extern void _RP lis_pci_disable_device(lis_pci_dev_t *dev);
extern int _RP lis_pci_enable_device(lis_pci_dev_t *dev);
extern lis_pci_dev_t *_RP lis_pci_find_class(unsigned class, lis_pci_dev_t *previous_struct);
extern lis_pci_dev_t *_RP lis_pci_find_device(unsigned vendor, unsigned device,
					      lis_pci_dev_t *previous_struct);
extern lis_pci_dev_t *_RP lis_pci_find_slot(unsigned bus, unsigned dev_fcn);
extern int _RP lis_pci_read_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char *rtn_val);
extern int _RP lis_pci_read_config_dword(lis_pci_dev_t *dev, unsigned index,
					 unsigned long *rtn_val);
extern int _RP lis_pci_read_config_word(lis_pci_dev_t *dev, unsigned index,
					unsigned short *rtn_val);
extern void _RP lis_pci_set_master(lis_pci_dev_t *dev);
extern int _RP lis_pci_write_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char val);
extern int _RP lis_pci_write_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long val);
extern int _RP lis_pci_write_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short val);
extern int _RP lis_pcibios_find_class(unsigned int class_code, unsigned short index,
				      unsigned char *bus, unsigned char *dev_fn);
extern int _RP lis_pcibios_find_device(unsigned short vendor, unsigned short dev_id,
				       unsigned short index, unsigned char *bus,
				       unsigned char *dev_fn);
extern void _RP lis_pcibios_init(void);
extern int _RP lis_pcibios_present(void);
extern int _RP lis_pcibios_read_config_byte(unsigned char bus, unsigned char dev_fn,
					    unsigned char where, unsigned char *val);
extern int _RP lis_pcibios_read_config_dword(unsigned char bus, unsigned char dev_fn,
					     unsigned char where, unsigned int *val);
extern int _RP lis_pcibios_read_config_word(unsigned char bus, unsigned char dev_fn,
					    unsigned char where, unsigned short *val);
extern const char *_RP lis_pcibios_strerror(int error);
extern int _RP lis_pcibios_write_config_byte(unsigned char bus, unsigned char dev_fn,
					     unsigned char where, unsigned char val);
extern int _RP lis_pcibios_write_config_dword(unsigned char bus, unsigned char dev_fn,
					      unsigned char where, unsigned int val);
extern int _RP lis_pcibios_write_config_word(unsigned char bus, unsigned char dev_fn,
					     unsigned char where, unsigned short val);
#endif

__LIS_EXTERN_INLINE _RP int
lis_copyin(struct file *fp, void *kbuf, const void *ubuf, int len)
{
	return copyin(ubuf, kbuf, len);
}
__LIS_EXTERN_INLINE _RP int
lis_copyout(struct file *fp, const void *kbuf, void *ubuf, int len)
{
	return copyout(kbuf, ubuf, len);
}

#ifndef __LIS_NO_MACROS
#define lis_alloc_atomic(__p1) lis_alloc_atomic_fcn(__p1, __FILE__, __LINE__)
#define lis_alloc_dma(__p1) lis_alloc_dma_fcn(__p1, __FILE__, __LINE__)
#define lis_alloc_kernel(__p1) lis_alloc_kernel_fcn(__p1, __FILE__, __LINE__)
//#define lis_dec_mod_cnt() lis_dec_mod_cnt_fcn(__FILE__, __LINE__)
#define lis_down(__p1) lis_down_fcn(__p1, __FILE__, __LINE__)
#define lis_free_mem(__p1) lis_free_mem_fcn(__p1, __FILE__, __LINE__)
#define lis_free_pages(__p1) lis_free_pages_fcn(__p1, __FILE__, __LINE__)
#define lis_get_free_pages_atomic(__p1) lis_get_free_pages_atomic_fcn(__p1, __FILE__, __LINE__)
#define lis_get_free_pages_kernel(__p1) lis_get_free_pages_kernel_fcn(__p1, __FILE__, __LINE__)
#define lis_get_free_pages(__p1, __p2) lis_get_free_pages_fcn(__p1, __p2, __FILE__, __LINE__)
//#define lis_inc_mod_cnt() lis_inc_mod_cnt_fcn(__FILE__, __LINE__)
#define lis_rw_lock_init(__p1, __p2) lis_rw_lock_init_fcn(__p1, __p2, __FILE__, __LINE__)
#define lis_rw_lock_alloc(__p1) lis_rw_lock_alloc_fcn(__p1, __FILE__, __LINE__)
#define lis_rw_lock_free(__p1, __p2) lis_rw_lock_free_fcn(__p1, __p2, __FILE__, __LINE__)
#define lis_rw_read_lock_irq(__p1) lis_rw_read_lock_irq_fcn(__p1, __FILE__, __LINE__)
#define lis_rw_read_lock_irqsave(__p1, __p2) lis_rw_read_lock_irqsave_fcn(__p1, __p2, __FILE__, __LINE__)
#define lis_rw_read_lock(__p1) lis_rw_read_lock_fcn(__p1, __FILE__, __LINE__)
#define lis_rw_read_unlock_irq(__p1) lis_rw_read_unlock_irq_fcn(__p1, __FILE__, __LINE__)
#define lis_rw_read_unlock_irqrestore(__p1, __p2) lis_rw_read_unlock_irqrestore_fcn(__p1, __p2, __FILE__, __LINE__)
#define lis_rw_read_unlock(__p1) lis_rw_read_unlock_fcn(__p1, __FILE__, __LINE__)
#define lis_rw_write_lock_irq(__p1) lis_rw_write_lock_irq_fcn(__p1, __FILE__, __LINE__)
#define lis_rw_write_lock_irqsave(__p1, __p2) lis_rw_write_lock_irqsave_fcn(__p1, __o2, __FILE__, __LINE__)
#define lis_rw_write_lock(__p1) lis_rw_write_lock_fcn(__p1, __FILE__, __LINE__)
#define lis_rw_write_unlock_irq(__p1) lis_rw_write_unlock_irq_fcn(__p1, __FILE__, __LINE__)
#define lis_rw_write_unlock_irqrestore(__p1, __p2) lis_rw_write_unlock_irqrestore_fcn(__p1, __p2, __FILE__, __LINE__)
#define lis_rw_write_unlock(__p1) lis_rw_write_unlock_fcn(__p1, __FILE__, __LINE__)
#define lis_spin_is_locked(__p1) lis_spin_is_locked_fcn(__p1, __FILE__, __LINE__)
#define lis_spin_lock_alloc(__p1) lis_spin_lock_alloc_fcn(__p1, __FILE__, __LINE__)
#define lis_spin_lock_free(__p1) lis_spin_lock_free_fcn(__p1, __FILE__, __LINE__)
#define lis_spin_lock_init(__p1, __p2) lis_spin_lock_init_fcn(__p1, __p2, __FILE__, __LINE__)
#define lis_spin_lock_irq(__p1) lis_spin_lock_irq_fcn(__p1, __FILE__, __LINE__)
#define lis_spin_lock_irqsave(__p1, __p2) lis_spin_lock_irqsave_fcn(__p1, __p2, __FILE__, __LINE__)
#define lis_spin_lock(__p1) lis_spin_lock_fcn(__p1, __FILE__, __LINE__)
#define lis_spin_trylock(__p1) lis_spin_trylock_fcn(__p1, __FILE__, __LINE__)
#define lis_spin_unlock_irq(__p1) lis_spin_unlock_irq_fcn(__p1, __FILE__, __LINE__)
#define lis_spin_unlock_irqrestore(__p1, __p2) lis_spin_unlock_irqrestore_fcn(__p1, __p2, __FILE__, __LINE__)
#define lis_spin_unlock(__p1) lis_spin_unlock_fcn(__p1, __FILE__, __LINE__)
#define lis_spl0() lis_spl0_fcn(__FILE__, __LINE__)
#define lis_splstr() lis_splstr_fcn(__FILE__,__LINE__)
#define lis_splx(__p1) lis_splx_fcn(__p1, __FILE__, __LINE__)
#define lis_up(__p2) lis_up_fcn(__p2, __FILE__, __LINE__)
#endif

#endif				/* __SYS_LIS_DDI_H__ */
