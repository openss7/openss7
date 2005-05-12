/*****************************************************************************

 @(#) $RCSfile: liscompat.h,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/05/11 20:11:11 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/05/11 20:11:11 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_LISCOMPAT_H__
#define __LOCAL_LISCOMPAT_H__

#define LIS_MAXAPUSH	8
#define LIS_FMNAMESZ	8
struct lis_strapush {
	unsigned sap_cmd;
	unsigned long sap_major;
	unsigned long sap_minor;
	unsigned long sap_lastminor;
	unsigned int sap_npush;
	char sap_list[LIS_MAXAPUSH][LIS_FMNAMESZ + 1];
};

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

typedef struct lis_strrecvfd {
	union {
		struct file *fp;
		int fd;
	} f;
	uid_t uid;
	gid_t gid;
	struct {
		struct file *fp;
		struct stdata *hd;
	} r;
} strrecvfd_t;

extern void *lis__kfree(void *ptr);
extern void *lis__kmalloc(int nbytes, int class, int use_cache);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_add_timer(struct timer_list *timer);
#endif
extern int lis_adjmsg(mblk_t *mp, int length);
extern void *lis_alloc_atomic_fcn(int nbytes, char *file, int line);
extern void *lis_alloc_dma_fcn(int nbytes, char *file, int line);
extern void *lis_alloc_kernel_fcn(int nbytes, char *file, int line);
extern struct msgb *lis_allocb(int size, unsigned int priority, char *file_name, int line_nr);
extern struct msgb *lis_allocb_physreq(int size, unsigned int priority, void *physreq_ptr,
				       char *file_name, int line_nr);
#if LIS_DEPRECARTED_FUNCTIONS
extern queue_t *lis_allocq(const char *name);
#endif
extern void lis_appq(queue_t *q, mblk_t *mp1, mblk_t *mp2);
extern int lis_apush_get(struct lis_strapush *ap);
extern int lis_apush_set(struct lis_strapush *ap);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_apushm(dev_t dev, const char *mods[]);
#endif
extern void lis_assert_fail(const char *expr, const char *objname, const char *file,
			    unsigned int line);
extern void lis_atomic_add(lis_atomic_t *atomic_addr, int amt);
extern void lis_atomic_dec(lis_atomic_t *atomic_addr);
extern int lis_atomic_dec_and_test(lis_atomic_t *atomic_addr);
extern void lis_atomic_inc(lis_atomic_t *atomic_addr);
extern int lis_atomic_read(lis_atomic_t *atomic_addr);
extern void lis_atomic_set(lis_atomic_t *atomic_addr, int valu);
extern void lis_atomic_sub(lis_atomic_t *atomic_addr, int amt);
extern queue_t *lis_backq(queue_t *q);
extern queue_t *lis_backq_fcn(queue_t *q, char *f, int l);
extern int lis_bcanput(queue_t *q, unsigned char band);
extern int lis_bcanputnext(queue_t *q, unsigned char band);
extern int lis_bcanputnext_anyband(queue_t *q);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_bprintf(char *fmt, ...);
#endif
extern int lis_bufcall(unsigned size, int priority, void (*function) (long), long arg);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_can_unload(void);
extern int lis_check_guard(void *ptr, char *msg);
extern int lis_check_mem(void);
extern int lis_check_q_magic(queue_t *q, char *file, int line);
extern int lis_check_region(unsigned int from, unsigned int extent);
extern int lis_check_umem(struct file *fp, int rd_wr_fcn, const void *usr_addr, int lgth);
#endif
extern int lis_clone_major(void);
extern void lis_cmn_err(int err_lvl, char *fmt, ...);
extern mblk_t *lis_copyb(mblk_t *mp);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_copyin(struct file *fp, void *kbuf, const void *ubuf, int len);
#endif
extern mblk_t *lis_copymsg(mblk_t *mp);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_copyout(struct file *fp, const void *kbuf, void *ubuf, int len);
#endif
extern char lis_date[];
extern unsigned long lis_debug_mask;
extern unsigned long lis_debug_mask2;
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_dec_mod_cnt_fcn(const char *file, int line);
extern int lis_del_timer(struct timer_list *timer);
#endif
extern void lis_disable_irq(unsigned int irq);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_dobufcall(int cpu_id);
extern int lis_doclose(struct inode *i, struct file *f, stdata_t *head, cred_t *creds);
#endif
extern int lis_down_fcn(lis_semaphore_t *lsem, char *file, int line);
extern unsigned long lis_dsecs(void);
extern mblk_t *lis_dupb(mblk_t *mp);
extern mblk_t *lis_dupmsg(mblk_t *mp);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_enable_intr(struct streamtab *strtab, int major, const char *name);
#endif
extern void lis_enable_irq(unsigned int irq);
extern mblk_t *lis_esballoc(unsigned char *base, int size, int priority, frtn_t *freeinfo,
			    char *file_name, int line_nr);
extern int lis_esbbcall(int priority, void (*function) (long), long arg);
#if LIS_DEPRECARTED_FUNCTIONS
#if HAVE_KERNEL_FATTACH_SUPPORT
extern int lis_fattach(struct file *f, const char *path);
extern int lis_fdetach(const char *path);
#endif
extern void lis_fdetach_all(void);
extern void lis_fdetach_stream(stdata_t *head);
extern void lis_fifo_close_sync(struct inode *i, struct file *f);
extern int lis_fifo_open_sync(struct inode *i, struct file *f);
extern int lis_fifo_write_sync(struct inode *i, int written);
extern struct inode *lis_file_inode(struct file *f);
extern struct stdata *lis_file_str(struct file *f);
extern streamtab_t *lis_find_strdev(major_t major);
extern void lis_flush_print_buffer(void);
#endif
extern void lis_flushband(queue_t *q, unsigned char band, int flag);
extern void lis_flushq(queue_t *q, int flag);
#if LIS_DEPRECARTED_FUNCTIONS
extern struct fmodsw *lis_fmod_sw;
#endif
extern void lis_free(void *ptr, char *file_name, int line_nr);
extern void lis_free_dma(unsigned int dma_nr);
extern void lis_free_irq(unsigned int irq, void *dev_id);
extern void *lis_free_mem_fcn(void *mem_area, char *file, int line);
extern void *lis_free_pages_fcn(void *ptr, char *file, int line);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_free_passfp(mblk_t *mp);
#endif
extern void lis_freeb(mblk_t *bp);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_freedb(mblk_t *bp, int free_hdr);
#endif
extern void lis_freemsg(mblk_t *mp);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_freeq(queue_t *q);
extern struct fmodsw *lis_fstr_sw;
extern int lis_get_fifo(struct file **f);
#endif
extern void *lis_get_free_pages_atomic_fcn(int nbytes, char *file, int line);
extern void *lis_get_free_pages_fcn(int nbytes, int class, char *file, int line);
extern void *lis_get_free_pages_kernel_fcn(int nbytes, char *file, int line);
#if LIS_DEPRECARTED_FUNCTIONS
extern mblk_t *lis_get_passfp(void);
extern int lis_get_pipe(struct file **f0, struct file **f1);
#endif
extern int lis_getint(unsigned char **p);
extern mblk_t *lis_getq(queue_t *q);
extern void lis_gettimeofday(struct timeval *tv);
extern unsigned long lis_hitime(void);
#if LIS_DEPRECARTED_FUNCTIONS
extern lis_atomic_t lis_in_syscall;
extern void lis_inc_mod_cnt_fcn(const char *file, int line);
extern void lis_init_bufcall(void);
#endif
extern int lis_insq(queue_t *q, mblk_t *emp, mblk_t *mp);
extern void lis_interruptible_sleep_on(wait_queue_head_t *wq);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_ioc_fattach(struct file *f, char *path);
extern int lis_ioc_fdetach(char *path);
extern int lis_ioc_pipe(unsigned int *fildes);
#endif
extern void *lis_ioremap(unsigned long offset, unsigned long size);
extern void *lis_ioremap_nocache(unsigned long offset, unsigned long size);
extern void lis_iounmap(void *ptr);
extern unsigned long lis_jiffies(void);
extern int lis_kernel_down(struct semaphore *sem);
extern void lis_kernel_up(struct semaphore *sem);
extern char lis_kernel_version[];
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_kfree(const void *ptr);
extern int lis_kill_pg(int pgrp, int sig, int priv);
extern int lis_kill_proc(int pid, int sig, int priv);
extern void *lis_kmalloc(size_t nbytes, int type);
#endif
extern void lis_linkb(mblk_t *mp1, mblk_t *mp2);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_loadable_load(const char *name);
extern const char *lis_maj_min_name(stdata_t *head);
#endif
extern int lis_major;
extern void *lis_malloc(int nbytes, int class, int use_cache, char *file_name, int line_nr);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_mark_mem(void *ptr, const char *file_name, int line_nr);
extern long lis_max_mem;
#endif
extern long lis_milli_to_ticks(long milli_sec);
extern int lis_mknod(char *name, int mode, dev_t dev);
extern int lis_mount(char *dev_name, char *dir_name, char *fstype, unsigned long rwflag,
		     void *data);
extern unsigned long lis_msecs(void);
extern const char *lis_msg_type_name(mblk_t *mp);
extern int lis_msgdsize(mblk_t *mp);
extern mblk_t *lis_msgpullup(mblk_t *mp, int len);
extern int lis_msgsize(mblk_t *mp);
extern int lis_num_cpus;
#if LIS_DEPRECARTED_FUNCTIONS
extern struct inode *lis_old_inode(struct file *f, struct inode *i);
extern lis_atomic_t lis_open_cnt;
#endif
extern void lis_osif_cli(void);
extern void lis_osif_do_gettimeofday(struct timeval *tp);
extern void lis_osif_do_settimeofday(struct timeval *tp);
extern void *lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
					   dma_addr_t *dma_handle);
extern int lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask);
#if HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE
extern void lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev, dma64_addr_t dma_addr,
					     size_t len, int direction);
#endif
extern void lis_osif_pci_dac_dma_sync_single_for_cpu(struct pci_dev *pdev, dma64_addr_t dma_addr,
					     size_t len, int direction);
extern void lis_osif_pci_dac_dma_sync_single_for_device(struct pci_dev *pdev, dma64_addr_t dma_addr,
					     size_t len, int direction);
extern unsigned long lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev, dma64_addr_t dma_addr);
extern struct page *lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev, dma64_addr_t dma_addr);
extern dma64_addr_t lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev, struct page *page,
						 unsigned long offset, int direction);
extern int lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask);
extern void lis_osif_pci_disable_device(struct pci_dev *dev);
extern int lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask);
extern void lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nelems,
				     int direction);
extern void lis_osif_pci_dma_sync_single(struct pci_dev *hwdev, dma_addr_t dma_handle, size_t size,
					 int direction);
extern int lis_osif_pci_enable_device(struct pci_dev *dev);
extern struct pci_dev *lis_osif_pci_find_class(unsigned int class, struct pci_dev *from);
extern struct pci_dev *lis_osif_pci_find_device(unsigned int vendor, unsigned int device,
						struct pci_dev *from);
extern struct pci_dev *lis_osif_pci_find_slot(unsigned int bus, unsigned int devfn);
extern void lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size, void *vaddr,
					 dma_addr_t dma_handle);
extern dma_addr_t lis_osif_pci_map_page(struct pci_dev *hwdev, struct page *page,
					unsigned long offset, size_t size, int direction);
extern int lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
			       int direction);
extern dma_addr_t lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size,
					  int direction);
extern int lis_osif_pci_module_init(void *p);
extern int lis_osif_pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val);
extern int lis_osif_pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val);
extern int lis_osif_pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val);
extern int lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask);
extern void lis_osif_pci_set_master(struct pci_dev *dev);
extern void lis_osif_pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address, size_t size,
				    int direction);
extern void lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
				  int direction);
extern void lis_osif_pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr, size_t size,
				      int direction);
extern void lis_osif_pci_unregister_driver(struct pci_driver *p);
extern int lis_osif_pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val);
extern int lis_osif_pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val);
extern int lis_osif_pci_write_config_word(struct pci_dev *dev, u8 where, u16 val);
extern dma_addr_t lis_osif_sg_dma_address(struct scatterlist *sg);
extern size_t lis_osif_sg_dma_len(struct scatterlist *sg);
extern void lis_osif_sti(void);
extern int lis_own_spl(void);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_pci_cleanup(void);
#endif
extern void lis_pci_disable_device(lis_pci_dev_t *dev);
extern int lis_pci_enable_device(lis_pci_dev_t *dev);
extern lis_pci_dev_t *lis_pci_find_class(unsigned class, lis_pci_dev_t *previous_struct);
extern lis_pci_dev_t *lis_pci_find_device(unsigned vendor, unsigned device,
					  lis_pci_dev_t *previous_struct);
extern lis_pci_dev_t *lis_pci_find_slot(unsigned bus, unsigned dev_fcn);
extern int lis_pci_read_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char *rtn_val);
extern int lis_pci_read_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long *rtn_val);
extern int lis_pci_read_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short *rtn_val);
extern void lis_pci_set_master(lis_pci_dev_t *dev);
extern int lis_pci_write_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char val);
extern int lis_pci_write_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long val);
extern int lis_pci_write_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short val);
extern int lis_pcibios_find_class(unsigned int class_code, unsigned short index, unsigned char *bus,
				  unsigned char *dev_fn);
extern int lis_pcibios_find_device(unsigned short vendor, unsigned short dev_id,
				   unsigned short index, unsigned char *bus, unsigned char *dev_fn);
extern void lis_pcibios_init(void);
extern int lis_pcibios_present(void);
extern int lis_pcibios_read_config_byte(unsigned char bus, unsigned char dev_fn,
					unsigned char where, unsigned char *val);
extern int lis_pcibios_read_config_dword(unsigned char bus, unsigned char dev_fn,
					 unsigned char where, unsigned int *val);
extern int lis_pcibios_read_config_word(unsigned char bus, unsigned char dev_fn,
					unsigned char where, unsigned short *val);
extern const char *lis_pcibios_strerror(int error);
extern int lis_pcibios_write_config_byte(unsigned char bus, unsigned char dev_fn,
					 unsigned char where, unsigned char val);
extern int lis_pcibios_write_config_dword(unsigned char bus, unsigned char dev_fn,
					  unsigned char where, unsigned int val);
extern int lis_pcibios_write_config_word(unsigned char bus, unsigned char dev_fn,
					 unsigned char where, unsigned short val);
extern void *lis_phys_to_virt(unsigned long addr);
#if LIS_DEPRECARTED_FUNCTIONS
#if HAVE_KERNEL_PIPE_SUPPORT
extern int lis_pipe(unsigned int *fd);
#endif
extern unsigned lis_poll_2_1(struct file *fp, poll_table * wait);
extern unsigned lis_poll_bits(stdata_t *hd);
extern char *lis_poll_events(short events);
extern char *lis_poll_file;
#endif
extern void lis_print_block(void *ptr);
extern void lis_print_data(mblk_t *mp, int opt, int cont);
extern void lis_print_mem(void);
extern void lis_print_msg(mblk_t *mp, const char *prefix, int opt);
extern void lis_print_queue(queue_t *q);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_print_queues(void);
extern void lis_print_spl_track(void);
extern void lis_print_stream(stdata_t *hd);
#endif
extern int lis_printk(const char *fmt, ...);
extern int lis_pullupmsg(mblk_t *mp, int length);
extern int lis_putbq(queue_t *q, mblk_t *mp);
extern void lis_putbyte(unsigned char **p, unsigned char byte);
extern int lis_putctl(queue_t *q, int type, char *file_name, int line_nr);
extern int lis_putctl1(queue_t *q, int type, int param, char *file_name, int line_nr);
extern int lis_putnextctl(queue_t *q, int type, char *file_name, int line_nr);
extern int lis_putnextctl1(queue_t *q, int type, int param, char *file_name, int line_nr);
extern int lis_putq(queue_t *q, mblk_t *mp);
extern int lis_qcountstrm(queue_t *q);
#if LIS_DEPRECARTED_FUNCTIONS
extern void lis_qdetach(queue_t *q, int do_close, int flag, cred_t *creds);
#endif
extern void lis_qenable(queue_t *q);
extern void lis_qprocsoff(queue_t *rdq);
extern void lis_qprocson(queue_t *rdq);
extern int lis_qsize(queue_t *q);
extern const char *lis_queue_name(queue_t *q);
#if LIS_DEPRECARTED_FUNCTIONS
extern volatile unsigned long lis_queuerun_cnts[];
extern lis_atomic_t lis_queues_running;
extern int lis_recvfd(stdata_t *recvhd, strrecvfd_t * recv, struct file *fp);
#endif
extern int lis_register_strdev(major_t major, struct streamtab *strtab, int nminor,
			       const char *name);
extern modID_t lis_register_strmod(struct streamtab *strtab, const char *name);
extern void lis_release_region(unsigned int from, unsigned int extent);
extern int lis_request_dma(unsigned int dma_nr, const char *device_id);
extern int lis_request_irq(unsigned int irq, void (*handler) (int, void *, struct pt_regs *),
			   unsigned long flags, const char *device, void *dev_id);
extern void lis_request_region(unsigned int from, unsigned int extent, const char *name);
extern mblk_t *lis_rmvb(mblk_t *mp, mblk_t *bp);
extern void lis_rmvq(queue_t *q, mblk_t *mp);
#if LIS_DEPRECARTED_FUNCTIONS
extern volatile unsigned long lis_runq_cnts[];
extern lis_atomic_t lis_runq_req_cnt;
#endif
extern lis_rw_lock_t *lis_rw_lock_alloc_fcn(const char *name, char *file, int line);
extern lis_rw_lock_t *lis_rw_lock_free_fcn(lis_rw_lock_t *lock, const char *name, char *file,
					   int line);
extern void lis_rw_lock_init_fcn(lis_rw_lock_t *lock, const char *name, char *file, int line);
extern void lis_rw_read_lock_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flags, char *file, int line);
extern void lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flags, char *file,
					      int line);
extern void lis_rw_write_lock_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flags, char *file, int line);
extern void lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line);
extern void lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flags, char *file,
					       int line);
extern queue_t *lis_safe_OTHERQ(queue_t *q, char *f, int l);
extern queue_t *lis_safe_RD(queue_t *q, char *f, int l);
extern int lis_safe_SAMESTR(queue_t *q, char *f, int l);
extern queue_t *lis_safe_WR(queue_t *q, char *f, int l);
extern int lis_safe_canenable(queue_t *q, char *f, int l);
extern void lis_safe_enableok(queue_t *q, char *f, int l);
extern void lis_safe_noenable(queue_t *q, char *f, int l);
extern void lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l);
extern void lis_safe_putnext(queue_t *q, mblk_t *mp, char *f, int l);
extern void lis_safe_qreply(queue_t *q, mblk_t *mp, char *f, int l);
extern unsigned long lis_secs(void);
extern lis_semaphore_t *lis_sem_alloc(int count);
extern lis_semaphore_t *lis_sem_destroy(lis_semaphore_t *lsem);
extern void lis_sem_init(lis_semaphore_t *lsem, int count);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_sendfd(stdata_t *sendhd, unsigned int fd, struct file *fp);
extern void lis_set_file_str(struct file *f, struct stdata *s);
extern void lis_setq(queue_t *q, struct qinit *rinit, struct qinit *winit);
extern void lis_setqsched(int can_call);
extern volatile unsigned long lis_setqsched_cnts[];
extern volatile unsigned long lis_setqsched_isr_cnts[];
#endif
extern void lis_sleep_on(wait_queue_head_t *wq);
extern int lis_spin_is_locked_fcn(lis_spin_lock_t *lock, char *file, int line);
extern lis_spin_lock_t *lis_spin_lock_alloc_fcn(const char *name, char *file, int line);
extern void lis_spin_lock_fcn(lis_spin_lock_t *lock, char *file, int line);
extern lis_spin_lock_t *lis_spin_lock_free_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, char *file, int line);
extern void lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock, int *flags, char *file, int line);
extern int lis_spin_trylock_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void lis_spin_unlock_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, char *file, int line);
extern void lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock, int *flags, char *file, int line);
extern void lis_spl0_fcn(char *file, int line);
extern int lis_splstr_fcn(char *file, int line);
extern void lis_splx_fcn(int x, char *file, int line);
extern int lis_sprintf(char *bfr, const char *fmt, ...);
#if LIS_DEPRECARTED_FUNCTIONS
extern lis_atomic_t lis_stdata_cnt;
extern int lis_strclose(struct inode *i, struct file *f);
extern lis_atomic_t lis_strcount;
extern int lis_strgetpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp, int *bandp,
			  int *flagsp, int doit);
extern int lis_strioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg);
#endif
extern const char *lis_strm_name(stdata_t *head);
extern const char *lis_strm_name_from_queue(queue_t *q);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_stropen(struct inode *i, struct file *f);
#endif
extern char *lis_stropts_file;
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_strputpmsg(struct inode *, struct file *, void *, void *, int, int);
#endif
extern int lis_strqget(queue_t *q, qfields_t what, unsigned char band, long *val);
extern int lis_strqset(queue_t *q, qfields_t what, unsigned char band, long val);
#if LIS_DEPRECARTED_FUNCTIONS
extern ssize_t lis_strread(struct file *fp, char *ubuff, size_t ulen, loff_t *op);
extern lis_atomic_t lis_strstats[24][4];
extern ssize_t lis_strwrite(struct file *fp, const char *ubuff, size_t ulen, loff_t *op);
#endif
extern int lis_testb(int size, unsigned int priority);
extern pid_t lis_thread_start(int (*fcn) (void *), void *arg, const char *name);
extern int lis_thread_stop(pid_t pid);
extern toid_t lis_timeout_fcn(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, char *file_name,
			      int line_nr);
extern void lis_udelay(long micro_secs);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_umount2(char *path, int flags);
#endif
extern void lis_unbufcall(int bcid);
extern int lis_unlink(char *name);
extern mblk_t *lis_unlinkb(mblk_t *mp);
extern int lis_unregister_strdev(major_t major);
extern int lis_unregister_strmod(struct streamtab *strtab);
extern toid_t lis_untimeout(toid_t id);
extern void lis_up_fcn(lis_semaphore_t *lsem, char *file, int line);
extern unsigned long lis_usecs(void);
extern unsigned lis_usectohz(unsigned usec);
#if LIS_DEPRECARTED_FUNCTIONS
extern int lis_valid_mod_list(struct str_list ml);
#endif
extern char lis_version[];
extern void lis_vfree(void *ptr);
extern unsigned long lis_virt_to_phys(volatile void *addr);
extern void *lis_vmalloc(unsigned long size);
extern void *lis_vremap(unsigned long offset, unsigned long size);
extern int lis_vsprintf(char *bfr, const char *fmt, va_list args);
extern void lis_wake_up(wait_queue_head_t *wq);
extern void lis_wake_up_interruptible(wait_queue_head_t *wq);
extern int lis_xmsgsize(mblk_t *mp);
extern void *lis_zmalloc(int nbytes, int class, char *file_name, int line_nr);

#endif				/* __LOCAL_LISCOMPAT_H__ */
