/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.19 2005/07/18 12:25:39 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/07/18 12:25:39 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_LIS_DDI_H__
#define __SYS_LIS_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2005/07/18 12:25:39 $"

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

#ifndef __LIS_EXTERN_INLINE
#define __LIS_EXTERN_INLINE __depr extern __inline__
#endif				/* __LIS_EXTERN_INLINE */

#ifndef _LIS_SOURCE
#warning "_LIS_SOURCE not defined but LIS ddi.h included"
#define _LIS_SOURCE 1
#endif

#include <linux/poll.h>
#include <linux/interrupt.h>
#if HAVE_KINC_LINUX_HARDIRQ_H
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

extern void *lis__kfree(void *ptr) __depr;
extern void *lis__kmalloc(int nbytes, int class, int use_cache) __depr;

#if 0
extern void lis_add_timer(struct timer_list *timer) __depr;
#endif
extern void *lis_alloc_atomic_fcn(int nbytes, char *file, int line) __depr;
extern void *lis_alloc_dma_fcn(int nbytes, char *file, int line) __depr;
extern void *lis_alloc_kernel_fcn(int nbytes, char *file, int line) __depr;
extern void lis_assert_fail(const char *expr, const char *objname, const char *file,
			    unsigned int line) __depr;
extern void lis_atomic_add(lis_atomic_t *atomic_addr, int amt) __depr;
extern void lis_atomic_dec(lis_atomic_t *atomic_addr) __depr;
extern int lis_atomic_dec_and_test(lis_atomic_t *atomic_addr) __depr;
extern void lis_atomic_inc(lis_atomic_t *atomic_addr) __depr;
extern int lis_atomic_read(lis_atomic_t *atomic_addr) __depr;
extern void lis_atomic_set(lis_atomic_t *atomic_addr, int valu) __depr;
extern void lis_atomic_sub(lis_atomic_t *atomic_addr, int amt) __depr;

#if 0
extern void lis_bprintf(char *fmt, ...) __depr;
extern int lis_can_unload(void) __depr;
extern int lis_check_guard(void *ptr, char *msg) __depr;
extern int lis_check_mem(void) __depr;
extern int lis_check_region(unsigned int from, unsigned int extent) __depr;
extern int lis_check_umem(struct file *fp, int rd_wr_fcn, const void *usr_addr,
			  int lgth) __depr;
#endif
extern void lis_cmn_err(int err_lvl, char *fmt, ...) __depr;
extern char lis_date[] __depr;
extern unsigned long lis_debug_mask __depr;
extern unsigned long lis_debug_mask2 __depr;

#if 0
extern void lis_dec_mod_cnt_fcn(const char *file, int line) __depr;
extern int lis_del_timer(struct timer_list *timer) __depr;
#endif
extern void lis_disable_irq(unsigned int irq) __depr;

#if 0
extern void lis_dobufcall(int cpu_id) __depr;
extern int lis_doclose(struct inode *i, struct file *f, struct stdata *head,
		       cred_t *creds) __depr;
#endif
extern int lis_down_fcn(lis_semaphore_t *lsem, char *file, int line) __depr;
extern unsigned long lis_dsecs(void) __depr;

#if 0
extern void lis_enable_intr(struct streamtab *strtab, int major, const char *name) __depr;
#endif
extern void lis_enable_irq(unsigned int irq) __depr;
extern void lis_free(void *ptr, char *file_name, int line_nr) __depr;
extern void lis_free_dma(unsigned int dma_nr) __depr;
extern void lis_free_irq(unsigned int irq, void *dev_id) __depr;
extern void *lis_free_mem_fcn(void *mem_area, char *file, int line) __depr;
extern void *lis_free_pages_fcn(void *ptr, char *file, int line) __depr;
extern void *lis_get_free_pages_atomic_fcn(int nbytes, char *file, int line) __depr;
extern void *lis_get_free_pages_fcn(int nbytes, int class, char *file, int line) __depr;
extern void *lis_get_free_pages_kernel_fcn(int nbytes, char *file, int line) __depr;
extern int lis_getint(unsigned char **p) __depr;
extern void lis_gettimeofday(struct timeval *tv) __depr;
extern unsigned long lis_hitime(void) __depr;

#if 0
extern lis_atomic_t lis_in_syscall __depr;
extern void lis_inc_mod_cnt_fcn(const char *file, int line) __depr;
extern void lis_init_bufcall(void) __depr;
#endif
extern void lis_interruptible_sleep_on(wait_queue_head_t *wq) __depr;
extern void *lis_ioremap(unsigned long offset, unsigned long size) __depr;
extern void *lis_ioremap_nocache(unsigned long offset, unsigned long size) __depr;
extern void lis_iounmap(void *ptr) __depr;
extern unsigned long lis_jiffies(void) __depr;
extern int lis_kernel_down(struct semaphore *sem) __depr;
extern void lis_kernel_up(struct semaphore *sem) __depr;
extern char lis_kernel_version[] __depr;

#if 0
extern void lis_kfree(const void *ptr) __depr;
extern int lis_kill_pg(int pgrp, int sig, int priv) __depr;
extern int lis_kill_proc(int pid, int sig, int priv) __depr;
extern void *lis_kmalloc(size_t nbytes, int type) __depr;
extern int lis_loadable_load(const char *name) __depr;
extern const char *lis_maj_min_name(struct stdata *head) __depr;
#endif
extern int lis_major __depr;
extern void *lis_malloc(int nbytes, int class, int use_cache, char *file_name,
			int line_nr) __depr;
#if 0
extern void lis_mark_mem(void *ptr, const char *file_name, int line_nr) __depr;
extern long lis_max_mem __depr;
#endif
extern long lis_milli_to_ticks(long milli_sec) __depr;
extern int lis_mknod(char *name, int mode, dev_t dev) __depr;
extern int lis_mount(char *dev_name, char *dir_name, char *fstype, unsigned long rwflag,
		     void *data) __depr;
extern unsigned long lis_msecs(void) __depr;
extern const char *lis_msg_type_name(mblk_t *mp) __depr;
extern int lis_num_cpus __depr;
extern void lis_osif_cli(void) __depr;
extern void lis_osif_do_gettimeofday(struct timeval *tp) __depr;

#ifdef HAVE_TIMESPEC_DO_SETTIMEOFDAY
extern int lis_osif_do_settimeofday(struct timespec *tp) __depr;
#else
extern void lis_osif_do_settimeofday(struct timeval *tp) __depr;
#endif
extern void lis_osif_sti(void) __depr;
extern int lis_own_spl(void) __depr;
extern void *lis_phys_to_virt(unsigned long addr) __depr;
extern void lis_print_block(void *ptr) __depr;
extern void lis_print_data(mblk_t *mp, int opt, int cont) __depr;
extern void lis_print_mem(void) __depr;
extern void lis_print_msg(mblk_t *mp, const char *prefix, int opt) __depr;
extern void lis_print_queue(queue_t *q) __depr;

#if 0
extern void lis_print_spl_track(void) __depr;
#endif
extern int lis_printk(const char *fmt, ...) __depr;
extern void lis_putbyte(unsigned char **p, unsigned char byte) __depr;
extern const char *lis_queue_name(queue_t *q) __depr;
extern void lis_release_region(unsigned int from, unsigned int extent) __depr;
extern int lis_request_dma(unsigned int dma_nr, const char *device_id) __depr;

#if HAVE_KTYPE_IRQRETURN_T
extern int lis_request_irq(unsigned int irq,
			   irqreturn_t(*handler) (int, void *, struct pt_regs *),
			   unsigned long flags, const char *device, void *dev_id) __depr;
#else
extern int lis_request_irq(unsigned int irq,
			   void (*handler) (int, void *, struct pt_regs *),
			   unsigned long flags, const char *device, void *dev_id) __depr;
#endif
extern void lis_request_region(unsigned int from, unsigned int extent,
			       const char *name) __depr;
extern lis_rw_lock_t *lis_rw_lock_alloc_fcn(const char *name, char *file, int line) __depr;
extern lis_rw_lock_t *lis_rw_lock_free_fcn(lis_rw_lock_t *lock, const char *name,
					   char *file, int line) __depr;
extern void lis_rw_lock_init_fcn(lis_rw_lock_t *lock, const char *name, char *file,
				 int line) __depr;
extern void lis_rw_read_lock_fcn(lis_rw_lock_t *lock, char *file, int line) __depr;
extern void lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line) __depr;
extern void lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flagp, char *file,
					 int line) __depr;
extern void lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, char *file, int line) __depr;
extern void lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line) __depr;
extern void lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flagp, char *file,
					      int line) __depr;
extern void lis_rw_write_lock_fcn(lis_rw_lock_t *lock, char *file, int line) __depr;
extern void lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line) __depr;
extern void lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flagp, char *file,
					  int line) __depr;
extern void lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, char *file, int line) __depr;
extern void lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line) __depr;
extern void lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flagp, char *file,
					       int line) __depr;
extern unsigned long lis_secs(void) __depr;
extern lis_semaphore_t *lis_sem_alloc(int count) __depr;
extern lis_semaphore_t *lis_sem_destroy(lis_semaphore_t *lsem) __depr;
extern void lis_sem_init(lis_semaphore_t *lsem, int count) __depr;
extern void lis_sleep_on(wait_queue_head_t *wq) __depr;
extern int lis_spin_is_locked_fcn(lis_spin_lock_t *lock, char *file, int line) __depr;
extern lis_spin_lock_t *lis_spin_lock_alloc_fcn(const char *name, char *file,
						int line) __depr;
extern void lis_spin_lock_fcn(lis_spin_lock_t *lock, char *file, int line) __depr;
extern lis_spin_lock_t *lis_spin_lock_free_fcn(lis_spin_lock_t *lock, char *file,
					       int line) __depr;
extern void lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, char *file,
				   int line) __depr;
extern void lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, char *file, int line) __depr;
extern void lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock, int *flagp, char *file,
				      int line) __depr;
extern int lis_spin_trylock_fcn(lis_spin_lock_t *lock, char *file, int line) __depr;
extern void lis_spin_unlock_fcn(lis_spin_lock_t *lock, char *file, int line) __depr;
extern void lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, char *file, int line) __depr;
extern void lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock, int *flagp, char *file,
					   int line) __depr;
extern void lis_spl0_fcn(char *file, int line) __depr;
extern int lis_splstr_fcn(char *file, int line) __depr;
extern void lis_splx_fcn(int x, char *file, int line) __depr;
extern int lis_sprintf(char *bfr, const char *fmt, ...) __depr;
extern const char *lis_strm_name(struct stdata *head) __depr;
extern const char *lis_strm_name_from_queue(queue_t *q) __depr;
extern char *lis_stropts_file __depr;
extern pid_t lis_thread_start(int (*fcn) (void *), void *arg, const char *name) __depr;
extern int lis_thread_stop(pid_t pid) __depr;
extern void lis_udelay(long micro_secs) __depr;

#if 0
extern int lis_umount2(char *path, int flags) __depr;
#endif
extern int lis_unlink(char *name) __depr;
extern void lis_up_fcn(lis_semaphore_t *lsem, char *file, int line) __depr;
extern unsigned long lis_usecs(void) __depr;
extern unsigned lis_usectohz(unsigned usec) __depr;
extern char lis_version[] __depr;
extern void lis_vfree(void *ptr) __depr;
extern unsigned long lis_virt_to_phys(volatile void *addr) __depr;
extern void *lis_vmalloc(unsigned long size) __depr;
extern void *lis_vremap(unsigned long offset, unsigned long size) __depr;
extern int lis_vsprintf(char *bfr, const char *fmt, va_list args) __depr;
extern void lis_wake_up(wait_queue_head_t *wq) __depr;
extern void lis_wake_up_interruptible(wait_queue_head_t *wq) __depr;
extern void *lis_zmalloc(int nbytes, int class, char *file_name, int line_nr) __depr;

#ifdef CONFIG_PCI
#include <linux/pci.h>

extern void *lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
					   dma_addr_t *dma_handle) __depr;
extern int lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask) __depr;

#if HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE
extern void lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev, dma64_addr_t dma_addr,
					     size_t len, int direction) __depr;
#endif
extern void lis_osif_pci_dac_dma_sync_single_for_cpu(struct pci_dev *pdev,
						     dma64_addr_t dma_addr, size_t len,
						     int direction) __depr;
extern void lis_osif_pci_dac_dma_sync_single_for_device(struct pci_dev *pdev,
							dma64_addr_t dma_addr, size_t len,
							int direction) __depr;
extern unsigned long lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev,
						    dma64_addr_t dma_addr) __depr;
extern struct page *lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev,
						 dma64_addr_t dma_addr) __depr;
extern dma64_addr_t lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev, struct page *page,
						 unsigned long offset,
						 int direction) __depr;
extern int lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask) __depr;
extern void lis_osif_pci_disable_device(struct pci_dev *dev) __depr;
extern int lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask) __depr;
extern void lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev, struct scatterlist *sg,
				     int nelems, int direction) __depr;
extern void lis_osif_pci_dma_sync_single(struct pci_dev *hwdev, dma_addr_t dma_handle,
					 size_t size, int direction) __depr;
extern int lis_osif_pci_enable_device(struct pci_dev *dev) __depr;
extern struct pci_dev *lis_osif_pci_find_class(unsigned int class,
					       struct pci_dev *from) __depr;
extern struct pci_dev *lis_osif_pci_find_device(unsigned int vendor, unsigned int device,
						struct pci_dev *from) __depr;
extern struct pci_dev *lis_osif_pci_find_slot(unsigned int bus, unsigned int devfn) __depr;
extern void lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size, void *vaddr,
					 dma_addr_t dma_handle) __depr;
extern dma_addr_t lis_osif_pci_map_page(struct pci_dev *hwdev, struct page *page,
					unsigned long offset, size_t size,
					int direction) __depr;
extern int lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
			       int direction) __depr;
extern dma_addr_t lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size,
					  int direction) __depr;
extern int lis_osif_pci_module_init(void *p) __depr;
extern int lis_osif_pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val) __depr;
extern int lis_osif_pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val) __depr;
extern int lis_osif_pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val) __depr;
extern int lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask) __depr;
extern void lis_osif_pci_set_master(struct pci_dev *dev) __depr;
extern void lis_osif_pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address,
				    size_t size, int direction) __depr;
extern void lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
				  int direction) __depr;
extern void lis_osif_pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr,
				      size_t size, int direction) __depr;
extern void lis_osif_pci_unregister_driver(struct pci_driver *p) __depr;
extern int lis_osif_pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val) __depr;
extern int lis_osif_pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val) __depr;
extern int lis_osif_pci_write_config_word(struct pci_dev *dev, u8 where, u16 val) __depr;
extern dma_addr_t lis_osif_sg_dma_address(struct scatterlist *sg) __depr;
extern size_t lis_osif_sg_dma_len(struct scatterlist *sg) __depr;

#if 0
extern void lis_pci_cleanup(void) __depr;
#endif
extern void lis_pci_disable_device(lis_pci_dev_t *dev) __depr;
extern int lis_pci_enable_device(lis_pci_dev_t *dev) __depr;
extern lis_pci_dev_t *lis_pci_find_class(unsigned class,
					 lis_pci_dev_t *previous_struct) __depr;
extern lis_pci_dev_t *lis_pci_find_device(unsigned vendor, unsigned device,
					  lis_pci_dev_t *previous_struct) __depr;
extern lis_pci_dev_t *lis_pci_find_slot(unsigned bus, unsigned dev_fcn) __depr;
extern int lis_pci_read_config_byte(lis_pci_dev_t *dev, unsigned index,
				    unsigned char *rtn_val) __depr;
extern int lis_pci_read_config_dword(lis_pci_dev_t *dev, unsigned index,
				     unsigned long *rtn_val) __depr;
extern int lis_pci_read_config_word(lis_pci_dev_t *dev, unsigned index,
				    unsigned short *rtn_val) __depr;
extern void lis_pci_set_master(lis_pci_dev_t *dev) __depr;
extern int lis_pci_write_config_byte(lis_pci_dev_t *dev, unsigned index,
				     unsigned char val) __depr;
extern int lis_pci_write_config_dword(lis_pci_dev_t *dev, unsigned index,
				      unsigned long val) __depr;
extern int lis_pci_write_config_word(lis_pci_dev_t *dev, unsigned index,
				     unsigned short val) __depr;
extern int lis_pcibios_find_class(unsigned int class_code, unsigned short index,
				  unsigned char *bus, unsigned char *dev_fn) __depr;
extern int lis_pcibios_find_device(unsigned short vendor, unsigned short dev_id,
				   unsigned short index, unsigned char *bus,
				   unsigned char *dev_fn) __depr;
extern void lis_pcibios_init(void) __depr;
extern int lis_pcibios_present(void) __depr;
extern int lis_pcibios_read_config_byte(unsigned char bus, unsigned char dev_fn,
					unsigned char where, unsigned char *val) __depr;
extern int lis_pcibios_read_config_dword(unsigned char bus, unsigned char dev_fn,
					 unsigned char where, unsigned int *val) __depr;
extern int lis_pcibios_read_config_word(unsigned char bus, unsigned char dev_fn,
					unsigned char where, unsigned short *val) __depr;
extern const char *lis_pcibios_strerror(int error) __depr;
extern int lis_pcibios_write_config_byte(unsigned char bus, unsigned char dev_fn,
					 unsigned char where, unsigned char val) __depr;
extern int lis_pcibios_write_config_dword(unsigned char bus, unsigned char dev_fn,
					  unsigned char where, unsigned int val) __depr;
extern int lis_pcibios_write_config_word(unsigned char bus, unsigned char dev_fn,
					 unsigned char where, unsigned short val) __depr;
#endif

__LIS_EXTERN_INLINE int lis_copyin(struct file *fp, void *kbuf, const void *ubuf, int len)
{
	return copyin(ubuf, kbuf, len);
}
__LIS_EXTERN_INLINE int lis_copyout(struct file *fp, const void *kbuf, void *ubuf, int len)
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
